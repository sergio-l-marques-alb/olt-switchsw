/** \file crps_eviction.c
 * $Id$
 *  crps module gather all functionality that related to the counter/statisitc processor driver for DNX.
 *  crps_eviction.c responsible of the eviction mechanism. copy the HW counters into a software image.
 *  It is internal crps file .   (do not have interfaces with other modules)
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include "crps_mgmt_internal.h"
#include "crps_eviction.h"
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_err_recovery_manager_utils.h>
#include <include/soc/dnx/swstate/auto_generated/access/crps_access.h>
#include <include/shared/utilex/utilex_integer_arithmetic.h>
#include <include/shared/utilex/utilex_str.h>
#include <include/shared/utilex/utilex_bitstream.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <include/bcm_int/common/debug.h>
#include <include/bcm_int/control.h>
#include <include/bcm_int/dnx/algo/crps/crps_ctrs_db.h>
#include <include/soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/dnxc_ha.h>

/*
 * DEFINES
 * {
 */

/** 
* \brief
* defines the time interval in msec for the algorithm timer
*/
#define CRPS_ENGINE_ALG_TIMER_INTERVAL_MSEC (10)

#define DNX_CRPS_INVALID_DMA_CHANNEL (-1)

#define DNX_CRPS_FIFO_ID_ALL (-1)

#define DNX_CRPS_CNT_CACHE_LENGTH (32*1024)

#define DNX_CRPS_UINT32_NOF_BITS (32)

#define CRPS_EVICTION_FIFO_DMA_MAX_ENTRIES (16384)
#define CRPS_EVICTION_FIFO_DMA_TIMOUT (1000)
#define CRPS_EVICTION_FIFO_DMA_THRESHOLD (1)

/*
 * }
 */

/*
 * }
 */

/*
 * strucutres
 * {
 */

/**
 * \brief one counters result taken from the host fifo
 */
typedef struct
{
  /** Index of the returned counter entry*/
    uint32 counter_entry;
  /** Index of the returned the countet engine */
    uint32 engine_id;
  /** Index of the returned the countet engine */
    uint32 core_id;
  /** Value of the counter1.  (for mode=packets_and_bytes, it represent the packets) */
    uint64 cnt1;
  /** Value of counter2.  (for mode=packets_and_bytes, it represent the bytes)*/
    uint64 cnt2;
} dnx_crps_eviction_ctr_result_t;

/**
 * \brief arr which gather the cached counters taken from the host fifo
 */
typedef struct
{
  /** Array of the cache counters. Only the first 'nof_counters' counters are relevant. */
    dnx_crps_eviction_ctr_result_t *cnt_result;
  /** Number of valid counters in the counter result 'cnt_result'. Range: 0 - 16. */
    uint32 nof_counters;
} dnx_crps_eviction_ctr_result_arr_t;

/**
 * \brief holds data on the bg thread 
 */
typedef struct
{
    int running;                   /** unit background thread enable */
    int bgWait;                    /** delay per bg iter (microsec) */
    uint32 fifo_read_background;   /**  diagnostics for how many times reading the fifo from bg thread function */
    sal_thread_t background;       /** unit background thread */
    sal_sem_t bgSem;               /** unit background waiting sem */
} dnx_crps_eviction_bg_thread_data_t;

/*
 * }
 */

/*
 * globals
 * {
 */

/** array per unit of fifo results */
dnx_crps_eviction_ctr_result_arr_t dnx_crps_fifo_results[BCM_LOCAL_UNITS_MAX];
/** bg thread data per unit */
dnx_crps_eviction_bg_thread_data_t dnx_crps_bg_thread_data[BCM_LOCAL_UNITS_MAX];
/** array of unit number of the bg thread */
int dnx_crps_bg_thread_unit[BCM_LOCAL_UNITS_MAX]; /** unit number of the bg thread */
/** array per unit, flag which indicates if thread is still running after it was asked to stop*/
int dnx_crps_thread_is_still_running[BCM_LOCAL_UNITS_MAX];
/** cache access locking */
sal_mutex_t dnx_crps_cache_lock[BCM_LOCAL_UNITS_MAX];
/** fifo access locking */
sal_mutex_t dnx_crps_fifo_lock[BCM_LOCAL_UNITS_MAX];

/** global array that holds the dma channel for each CRPS FIFO */
int dnx_crps_fifo_channels[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES][DNX_DATA_MAX_CRPS_EVICTION_NOF_COUNTERS_FIFO];

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** 
 * \brief - iterate over the relevant fifo_ids 
 * if fifo_id is DNX_CRPS_FIFO_ID_ALL will iterate over all supported fifos. 
 * Otherwise the iteration will be over the given fifo. 
 */
#define DNX_CRPS_FIFOS_ITER(unit, fifo_id, fifo_index) \
    for(fifo_index = ((fifo_id == DNX_CRPS_FIFO_ID_ALL) ? 0 : fifo_id);\
        fifo_index < ((fifo_id == DNX_CRPS_FIFO_ID_ALL) ?  dnx_data_crps.eviction.nof_counters_fifo_get(unit) : \
                                                                                                    (fifo_id + 1));\
        fifo_index++)

/**
 * \brief set of macros to define the bg thread operation
 */
#define DNX_CRPS_FIFO_DEPTH_MIN   2
#define DNX_CRPS_FIFO_DEPTH_MAX(unit)   (dnx_data_crps.eviction.counters_fifo_depth_get(unit) - 1)
#define DNX_CRPS_FIFO_DEPTH_PREF(unit)  ((DNX_CRPS_FIFO_DEPTH_MIN + DNX_CRPS_FIFO_DEPTH_MAX(unit)) / 2)
#define DNX_CRPS_THREAD_WAIT_TIMEOUT    (2000000) /** wait two seconds */
#define DNX_CRPS_BACKGROUND_THREAD_ITER_MIN      (10000)
#define DNX_CRPS_BACKGROUND_THREAD_ITER_MAX(unit)     (DNX_CRPS_BACKGROUND_THREAD_ITER_MIN * 100)
#define DNX_CRPS_BACKGROUND_THREAD_ITER_DEFAULT(unit) (DNX_CRPS_BACKGROUND_THREAD_ITER_MAX(unit))
#define DNX_CRPS_BACKGROUND_THREAD_ITER_ADJUST(unit)    \
            ((DNX_CRPS_BACKGROUND_THREAD_ITER_MAX(unit) + DNX_CRPS_BACKGROUND_THREAD_ITER_MIN) / 20)

/*
 * }
 */

/** 
 * \brief - take lock of the counters dma fifos 
 */
shr_error_e
dnx_crps_fifo_state_lock_take(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (0 != sal_mutex_take(dnx_crps_fifo_lock[unit], sal_mutex_FOREVER))
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "unable to take unit %d counter dma fifo mutex\n", unit);
    }
exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief - give lock of the counters dma fifos 
 */
shr_error_e
dnx_crps_fifo_state_lock_give(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (0 != sal_mutex_give(dnx_crps_fifo_lock[unit]))
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "unable to give unit %d counter dma fifo mutex\n", unit);
    }
exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief - take lock of the cache counters
 */
shr_error_e
dnx_crps_counter_state_lock_take(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (0 != sal_mutex_take(dnx_crps_cache_lock[unit], sal_mutex_FOREVER))
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "unable to take unit %d counter mutex\n", unit);
    }
exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief - give lock of the cache counters
 */
shr_error_e
dnx_crps_counter_state_lock_give(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (0 != sal_mutex_give(dnx_crps_cache_lock[unit]))
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "unable to give unit %d counter mutex\n", unit);
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_engine_hw_set(
    int unit,
    uint32 seq_timer_interval,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction)
{
    uint32 alg_timer_interval;
    uint32 entry_handle_id;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ENABLE_CLEAR_ON_READ_RESET_VALUE,
                                 INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ENABLE_CLEAR_ON_READ, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_DMA_SELECT, INST_SINGLE,
                                 eviction->dma_fifo_select);
    /** alg_timer_interval calculation. resault is in clocks resolution */
    COMPILER_64_ZERO(time.time);
    COMPILER_64_ADD_32(time.time, CRPS_ENGINE_ALG_TIMER_INTERVAL_MSEC);
    time.time_units = DNXCMN_TIME_UNIT_MILISEC;
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &alg_timer_interval));
    /** add one more to alg_timer_interval in order that the alg and seq timers will not have the same periodicity intervals. 
            (they should not be multiple times to each other). otherwise, algo timer will always get priority over seq */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_ALG_SCANS,
                                 INST_SINGLE, (alg_timer_interval + 1));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_SEQ_SCANS,
                                 INST_SINGLE, seq_timer_interval);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_MODE,
                                 INST_SINGLE, DBAL_ENUM_FVAL_CRPS_ENGINE_SCAN_MODE_SEQUENTIAL_AND_ALGORITHIM_SCAN);
    /** set the algorithim threshold. field refer to 8 msb bits of the counter.  */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_THRESHOLD, INST_SINGLE, 0x10);
    /** set the algorithim threshold. field refer to 4 msb bits of the packet counter.  relevant only in case of packet and bytes format */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_PKT_THRESHOLD, INST_SINGLE, 0x1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_OBJECT_TYPE_FOR_DMA, INST_SINGLE,
                                 eviction->eviction_event_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_DMA_RECORD_LOGICAL_SEL, INST_SINGLE,
                                 (eviction->record_format == bcmStatEvictionRecordFormatLogical));

    /** scan bubble parameters */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_BUBBLE_REQUEST_ENABLE, INST_SINGLE,
                                 TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_BUBBLE_TIMEOUT_BEFORE_REQUEST,
                                 INST_SINGLE, dnx_data_crps.eviction.bubble_before_request_timeout_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_BUBBLE_TIMEOUT_AFTER_REQUEST,
                                 INST_SINGLE, dnx_data_crps.eviction.bubble_after_request_timeout_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_FORCE_BUBBLE, INST_SINGLE, FALSE);

    /** Note: eviction bundaries is set in different function once all the relevant paramenters are known */
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_crps_eviction_counter_result_update(
    int unit,
    uint32 *curr_entry)
{
    uint32 err_ind;
    uint32 opcode;
    uint32 ctr_id, engine_id, core_id;
    const dnx_data_crps_eviction_phy_record_format_t *rcd_format = dnx_data_crps.eviction.phy_record_format_get(unit);
    const dnx_data_crps_engine_engines_info_t *engine_info;
    bcm_stat_counter_format_type_t format;
    /** array of uint32 that holds the counter value (68 bits for Jer2) */
    uint32 ctr[(DNX_DATA_MAX_CRPS_EVICTION_DMA_RECORD_ENTRY_NOF_BITS / DNX_CRPS_UINT32_NOF_BITS + 1)] = { 0 };
    int ctr_index;
    int ctr_size_first_section;
    uint32 temp32_arr[2] = { 0 };
    uint64 temp64;
    int engine_valid;
    int nof_counters_per_entry;
    SHR_FUNC_INIT_VARS(unit);

    /** extract the opcode */
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                    (curr_entry, rcd_format->opcode_offset, rcd_format->opcode_size, &opcode));

    /** only if the opcode is 0, which means Physical format */
    if (opcode == 0)
    {
        /** extract the error indication */
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (curr_entry, rcd_format->err_ind_offset, rcd_format->err_ind_size, &err_ind));
        if (err_ind == FALSE)
        {
            /** extract the core_id */
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                            (curr_entry, rcd_format->core_id_offset, rcd_format->core_id_size, &core_id));
            if (core_id > dnx_data_device.general.nof_cores_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "core_id=%d. total_nof_cores= %d\n",
                             core_id, dnx_data_device.general.nof_cores_get(unit));
            }
            dnx_crps_fifo_results[unit].cnt_result[dnx_crps_fifo_results[unit].nof_counters].core_id = core_id;

            /** extract the engine_id */
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                            (curr_entry, rcd_format->engine_id_offset, rcd_format->engine_id_size, &engine_id));
            if (engine_id > dnx_data_crps.engine.nof_engines_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "engine_id=%d. device total_nof_engine= %d\n",
                             engine_id, dnx_data_crps.engine.nof_engines_get(unit));
            }
            dnx_crps_fifo_results[unit].cnt_result[dnx_crps_fifo_results[unit].nof_counters].engine_id = engine_id;

            /** extract the counter_id */
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                            (curr_entry, rcd_format->local_ctr_id_offset, rcd_format->local_ctr_id_size, &ctr_id));
            engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);

            /** get how many counters in each one counter entry: 2 counters for pkts_and_pkts and max formats, */
            /** else 1 counter */
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, core_id, engine_id, &format));
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_per_entry_get(unit, format, &nof_counters_per_entry));

            if (ctr_id >= engine_info->nof_counters * nof_counters_per_entry)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "counter_id=%d. bigger than nof_counters(%d) for engine=%d\n",
                             ctr_id, engine_info->nof_counters * nof_counters_per_entry, engine_id);
            }
            dnx_crps_fifo_results[unit].cnt_result[dnx_crps_fifo_results[unit].nof_counters].counter_entry =
                ctr_id / nof_counters_per_entry;

            /** extract the ctr value into array of 3 ctrs. */
            for (ctr_index = 0; ctr_index < (rcd_format->ctr_value_size / DNX_CRPS_UINT32_NOF_BITS + 1); ctr_index++)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                                (curr_entry, (rcd_format->ctr_value_offset + DNX_CRPS_UINT32_NOF_BITS * ctr_index),
                                 ((ctr_index < (rcd_format->ctr_value_size / DNX_CRPS_UINT32_NOF_BITS)) ?
                                  DNX_CRPS_UINT32_NOF_BITS : (rcd_format->ctr_value_size -
                                                              DNX_CRPS_UINT32_NOF_BITS * ctr_index)), &ctr[ctr_index]));
            }

            /** init ctr1 and ctr2 */
            COMPILER_64_ZERO(dnx_crps_fifo_results[unit].cnt_result[dnx_crps_fifo_results[unit].nof_counters].cnt1);
            COMPILER_64_ZERO(dnx_crps_fifo_results[unit].cnt_result[dnx_crps_fifo_results[unit].nof_counters].cnt2);
            COMPILER_64_ZERO(temp64);

            DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_id, engine_id, engine_valid);
            /** only if the engine is fully valid (meaning, configuration finished), it is allowed to update the counters */
            if (engine_valid)
            {
                /** according to the format, decide how to split the ctr value into ctr1 and ctr2 */

                /** according to the format, update the relevant counter */
                /** if wide mode (one counter per entry), we write to ctr1. */
                /** if double entry we write just one counter - LSB bits. even counter to ctr1, or odd to ctr2 */
                /** if FormatPacketsAndBytes - the packets is written to ctr1, bytes to ctr2*/
                switch (format)
                {
                    case bcmStatCounterFormatPackets:
                    case bcmStatCounterFormatBytes:
                        /** wide format ==> it is not possible that the counter will be >= 2^64 */
                        if (ctr[rcd_format->ctr_value_size / DNX_CRPS_UINT32_NOF_BITS] != 0)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "counter_value msb bits=%d. must be zero for WIDE format \n",
                                         ctr[rcd_format->ctr_value_size / DNX_CRPS_UINT32_NOF_BITS]);
                        }
                        COMPILER_64_SET(temp64, ctr[1], ctr[0]);
                        COMPILER_64_ADD_64
                            (dnx_crps_fifo_results[unit].cnt_result[dnx_crps_fifo_results[unit].nof_counters].cnt1,
                             temp64);
                        break;
                    case bcmStatCounterFormatPacketsAndBytes:
                        ctr_size_first_section = dnx_data_crps.eviction.packet_and_bytes_packet_size_get(unit);
                        /** extract the first counter */
                        SHR_BITCOPY_RANGE(temp32_arr, 0, ctr, 0, ctr_size_first_section);
                        COMPILER_64_SET(dnx_crps_fifo_results[unit].cnt_result
                                        [dnx_crps_fifo_results[unit].nof_counters].cnt1, temp32_arr[1], temp32_arr[0]);
                        /** init the temp counter array */
                        temp32_arr[0] = 0;
                        temp32_arr[1] = 0;

                        /** extract the second counter */
                        SHR_BITCOPY_RANGE(temp32_arr, 0, ctr, ctr_size_first_section,
                                          (rcd_format->ctr_value_size - ctr_size_first_section));
                        COMPILER_64_SET(dnx_crps_fifo_results[unit].cnt_result
                                        [dnx_crps_fifo_results[unit].nof_counters].cnt2, temp32_arr[1], temp32_arr[0]);

                        break;
                    case bcmStatCounterFormatDoublePackets:
                    case bcmStatCounterFormatMaxSize:
                        /** one counter size from total counter_value size (68 bits for Jer2) in the record. */
                        ctr_size_first_section = rcd_format->ctr_value_size / nof_counters_per_entry;

                        /** extract the LSB counter to the right SW counter - even to ctr1, odd to ctr2  */
                        SHR_BITCOPY_RANGE(temp32_arr, 0, ctr, 0, ctr_size_first_section);
                        if (ctr_id % 2 == 0)
                        {
                            COMPILER_64_SET(dnx_crps_fifo_results[unit].cnt_result
                                            [dnx_crps_fifo_results[unit].nof_counters].cnt1, temp32_arr[1],
                                            temp32_arr[0]);
                        }
                        else
                        {
                            COMPILER_64_SET(dnx_crps_fifo_results[unit].cnt_result
                                            [dnx_crps_fifo_results[unit].nof_counters].cnt2, temp32_arr[1],
                                            temp32_arr[0]);
                        }

                        break;
                    default:
                        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid format type (value=%d) \n", format);
                        break;
                }

            }
            /** increment the array index */
            dnx_crps_fifo_results[unit].nof_counters++;
        }
        else
        {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d err_ind=%d \n"), "record_update", unit, err_ind));
        }
    }
    else
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d opcode=%d \n"), "record_update", unit, opcode));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*  run over all valid entries in the fifo, read the records and fill the 
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit id
*   \param [in] core_id -  core_id. (0 or 1 or all)
*   \param [in] fifo_id -  fifo_id. (0 or 1 or DNX_CRPS_FIFO_ID_ALL)
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * dnx_crps_fifo_results[unit] - global array which is used outside this function
* \remark 
*     This is called by both the background task and the demand access
*     functions, so it does not itself handle the locking protocol.
*
*     Because the calls to "read buffer" from the background process
*     and API caller may collide, or may collide with the direct read, we
*     don't do any background updates while performing updates in preparation
*     for foreground access. 
* \see
*   * None
*/
static shr_error_e
dnx_crps_eviction_counter_fifo_read(
    int unit,
    int core_id,
    int fifo_id)
{
    uint32 entry_words = 0, entry_bytes;
    int counters_2read = 0, nof_read_cnt = 0;
    uint8 *host_buff;
    uint32 buff_index;
    uint32 *curr_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** init nof_ counters of the result array */
    dnx_crps_fifo_results[unit].nof_counters = 0;

    if (dnx_crps_fifo_channels[core_id][fifo_id] == DNX_CRPS_INVALID_DMA_CHANNEL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "channel is invalid for core_id=%d, fifo_id=%d \n", core_id, fifo_id);
    }

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_num_entries_get(unit, dnx_crps_fifo_channels[core_id][fifo_id], &counters_2read,
                                                    &host_buff, &entry_bytes));
    entry_words = BYTES2WORDS(entry_bytes);

    for (buff_index = 0; buff_index < counters_2read; buff_index++)
    {
        curr_entry = (uint32 *) host_buff + (buff_index * entry_words);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "ctr read: fifo=%d, counters_2read=%d, curr_line=%x val=%.8x %.8x %.8x %.8x\n"),
                     fifo_id, counters_2read, buff_index, (curr_entry[0]),
                     curr_entry[(1 % entry_words)], curr_entry[(2 % entry_words)], curr_entry[(3 % entry_words)]));

        /** copy the counter to the results array */
        nof_read_cnt++;
        SHR_IF_ERR_EXIT(dnx_crps_eviction_counter_result_update(unit, curr_entry));
    }

exit:
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_set_entries_read(unit, dnx_crps_fifo_channels[core_id][fifo_id], nof_read_cnt));
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   process the counters fifos. each call, empty the entire fifo and update the sw counters memory.
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit id
*   \param [in] core_id -  core_id. (0 or 1 or all)
*   \param [in] fifo_id -  fifo_id. (0 or 1 or DNX_CRPS_FIFO_ID_ALL)
*   \param [in] depth -  max number of valid entries read from each FIFO
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*     Need to take mutex "dnx_crps_fifo_lock[unit]" before calling this function !!!
*     This is called by both the background task and the demand access
*     functions, so it does not itself handle the locking protocol.
*
*     Because the calls to "read buffer" from the background process
*     and API caller may collide, or may collide with the direct read, we
*     don't do any background updates while performing updates in preparation
*     for foreground access. 
* \see
*   * None
*/
static shr_error_e
dnx_crps_eviction_counter_fifo_process(
    int unit,
    bcm_core_t core_id,
    int fifo_id,
    int *depth)
{
    int core_index, fifo_index, ctr_idx;
    int nof_counters = 0, flags = 0;
    int counter_lock_taken = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /** * run over all valid fifos and pull counters records from them */
    DNXCMN_CORES_ITER(unit, core_id, core_index)
    {
        DNX_CRPS_FIFOS_ITER(unit, fifo_id, fifo_index)
        {
            if (dnx_crps_fifo_channels[core_index][fifo_index] != DNX_CRPS_INVALID_DMA_CHANNEL)
            {
                /** read the data records from the current sw fifo */
                SHR_IF_ERR_CONT(dnx_crps_eviction_counter_fifo_read(unit, core_index, fifo_index));

                /** update the max nof_counters read from each FIFO */
                if (dnx_crps_fifo_results[unit].nof_counters > nof_counters)
                {
                    nof_counters = dnx_crps_fifo_results[unit].nof_counters;
                }

                /** update the sw counters memory */
                /** while there are counters to update, update them */
                /** update anyway, ctr1 and ctr2 even if one of them not relevant. It is expected that it was init to zero */
                SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_take(unit));
                counter_lock_taken = TRUE;
                for (ctr_idx = 0; ctr_idx < dnx_crps_fifo_results[unit].nof_counters; ctr_idx++)
                {
                    /** update ctr1 */
                    SHR_IF_ERR_CONT(dnx_crps_ctrs_db_counter_set
                                    (unit, flags, dnx_crps_fifo_results[unit].cnt_result[ctr_idx].core_id,
                                     dnx_crps_fifo_results[unit].cnt_result[ctr_idx].engine_id,
                                     dnx_crps_fifo_results[unit].cnt_result[ctr_idx].counter_entry,
                                     DNX_CRPS_MGMT_SUB_CTR1, dnx_crps_fifo_results[unit].cnt_result[ctr_idx].cnt1));
                    /** update ctr2 */
                    SHR_IF_ERR_CONT(dnx_crps_ctrs_db_counter_set
                                    (unit, flags, dnx_crps_fifo_results[unit].cnt_result[ctr_idx].core_id,
                                     dnx_crps_fifo_results[unit].cnt_result[ctr_idx].engine_id,
                                     dnx_crps_fifo_results[unit].cnt_result[ctr_idx].counter_entry,
                                     DNX_CRPS_MGMT_SUB_CTR2, dnx_crps_fifo_results[unit].cnt_result[ctr_idx].cnt2));
                }
                SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_give(unit));
                counter_lock_taken = FALSE;
            }
        }
    }

    *depth = nof_counters;
exit:
    if (counter_lock_taken)
    {
        SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_give(unit));
    }
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Background handler (one thread per unit)
* \par DIRECT INPUT: 
*   \param [in] unit_ptr    -  unit id pointer
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*     This sets running to TRUE to indicate it has started.
*
*     This thread will terminate itself and free the unit descriptor during
*     the next iteration if running is set to FALSE.  This allows
*     the destroy function to safely shut down the background thread by simply
*     detaching it from the globals and setting the running flag to FALSE.
* \see
*   * None
*/
void
dnx_crps_eviction_counter_bg(
    void *unit_ptr)
{
    int semTaken = FALSE;
    int mutexTaken;
    int depth = 0;
    int unit = *(int *) (unit_ptr);
    int rv;
    /*
     *int start, end;
     */
    /*
     * unsigned int proc_mask = 0;
     * 
     */
    SHR_FUNC_INIT_VARS(unit);

    dnx_err_recovery_utils_excluded_thread_add(unit);
    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, TRUE));

    SHR_NULL_CHECK(unit_ptr, _SHR_E_PARAM, "unit_ptr");

    /*
     * if the value of *unit_ptr does not match to the value in the dnx_crps_bg_thread_unit, it indicates that the
     * pointer was overwritten * For example, unit=2: in the attach function, it was updated to be
     * dnx_crps_bg_thread_unit[2]=2, * if unit_ptr overwritten and value != 2, we will check that
     * dnx_crps_bg_thread_unit[*unit_ptr]==*unit_ptr, * and this will probably will not be true. 
     */
    if (dnx_crps_bg_thread_unit[unit] != unit)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "*unit_ptr=%d, but dnx_crps_bg_thread_unit[%d]=%d\n",
                     unit, unit, dnx_crps_bg_thread_unit[unit]);
    }

    /*
     * loop until told to go away 
     */
    while (dnx_crps_bg_thread_data[unit].running)
    {

        /*
         *  This should wait for the specified interval and then give up,
         *  but we don't care whether it gets triggered or it gives up, so
         *  just discard the result deliberately.  Note that this mechanism
         *  can also be used to force an immediate trigger, such as when
         *  destroying a unit.
         */
        semTaken = (!sal_sem_take(dnx_crps_bg_thread_data[unit].bgSem, dnx_crps_bg_thread_data[unit].bgWait));

        /** background processing is not deferred or disabled */
        /** interlock using mutex instead of spin loops */
        mutexTaken = (0 == sal_mutex_take(dnx_crps_fifo_lock[unit], sal_mutex_FOREVER));

        if (!mutexTaken)
        {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d mutexTaken=%d \n"), "mutex", unit, mutexTaken));
            continue;
        }
        /** accounting -- background FIFO read attempt */
        dnx_crps_bg_thread_data[unit].fifo_read_background++;

        /** time measurements for FIFO processing */
        /** start = sal_time_usecs(); */
        /** process all of the active FIFOs */
        rv = dnx_crps_eviction_counter_fifo_process(unit, _SHR_CORE_ALL, DNX_CRPS_FIFO_ID_ALL, &depth);
        /** end=sal_time_usecs();*/
        /**LOG_CLI((BSL_META("%d unit: BG thread - eviction FIFO processing %dus\n FIFO depth %d\n "), unit, SAL_USECS_SUB(end,start), depth));*/
        if (rv != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "unit %d error processing counter fifo:  dnx_crps_eviction_counter_fifo_process \n"),
                       unit));
        }

        /*
         * Automatically adjust delay based upon how busy.  Tries to  reduce this thread's overhead on the system
         * while avoiding conditions that result in missed counter updates. Basically, hardware docs say should
         * read every 100ms or so  as long as we process 10 counters per FIFO read.  This  thread prefers to keep it
         * to fewer than 8 counters per FIFO  read and so will back out to 100ms if it can, but if it  starts to get
         * more, will reduce update delay until it hits 40ms or 8 counters per FIFO. 
         */
        if ((DNX_CRPS_FIFO_DEPTH_MIN > depth) &&
            (DNX_CRPS_BACKGROUND_THREAD_ITER_MAX(unit) > dnx_crps_bg_thread_data[unit].bgWait))
        {
            /** increase the bgWait, but don't go up from the min value */
            dnx_crps_bg_thread_data[unit].bgWait += DNX_CRPS_BACKGROUND_THREAD_ITER_ADJUST(unit);
            if (dnx_crps_bg_thread_data[unit].bgWait > DNX_CRPS_BACKGROUND_THREAD_ITER_MAX(unit))
            {
                dnx_crps_bg_thread_data[unit].bgWait = DNX_CRPS_BACKGROUND_THREAD_ITER_MAX(unit);
            }
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d counter background thread delay to %dus\n"),
                                       "increasing", unit, dnx_crps_bg_thread_data[unit].bgWait));
        }

        if ((DNX_CRPS_FIFO_DEPTH_MAX(unit) < depth) &&
            (DNX_CRPS_BACKGROUND_THREAD_ITER_MIN < dnx_crps_bg_thread_data[unit].bgWait))
        {
            dnx_crps_bg_thread_data[unit].bgWait = DNX_CRPS_BACKGROUND_THREAD_ITER_MIN;
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d counter background thread delay to %dus\n"),
                                       "decreasing", unit, dnx_crps_bg_thread_data[unit].bgWait));
        }
        else if ((DNX_CRPS_FIFO_DEPTH_PREF(unit) < depth) &&
                 (DNX_CRPS_BACKGROUND_THREAD_ITER_MIN < dnx_crps_bg_thread_data[unit].bgWait))
        {
            /** decrease the bgWait, but don't go down from the min value */
            if (dnx_crps_bg_thread_data[unit].bgWait >=
                (DNX_CRPS_BACKGROUND_THREAD_ITER_ADJUST(unit) + DNX_CRPS_BACKGROUND_THREAD_ITER_MIN))
            {
                dnx_crps_bg_thread_data[unit].bgWait -= DNX_CRPS_BACKGROUND_THREAD_ITER_ADJUST(unit);
            }
            else
            {
                dnx_crps_bg_thread_data[unit].bgWait = DNX_CRPS_BACKGROUND_THREAD_ITER_MIN;
            }

            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "%s unit %d counter background thread delay to %dus\n"),
                                       "decreasing", unit, dnx_crps_bg_thread_data[unit].bgWait));
        }

        /*
         * allow foreground access again 
         */
        sal_mutex_give(dnx_crps_fifo_lock[unit]);
    }

    /** dispose of the unit information */
    if (semTaken)
    {
        /*
         *  If we have it, then release it.  If not, nobody should have it
         *  now since it was released by the detach code, which is also
         *  where running was set to FALSE.
         */
        sal_sem_give(dnx_crps_bg_thread_data[unit].bgSem);
    }

exit:
    dnx_crps_thread_is_still_running[unit] = FALSE;
    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, FALSE));
    dnx_err_recovery_utils_excluded_thread_remove(unit);
    SHR_VOID_FUNC_EXIT;
}
/** see .h file */
shr_error_e
dnx_crps_eviction_bg_thread_attach(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_crps_bg_thread_data[unit].bgSem != NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unit %d bg sem already exist\n", unit);
    }
    if (dnx_crps_bg_thread_data[unit].running)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unit %d bg thread already run\n", unit);
    }
    dnx_crps_bg_thread_data[unit].bgSem = sal_sem_create("dnx_crps_counters_wait", sal_sem_BINARY, 1);
    if (!dnx_crps_bg_thread_data[unit].bgSem)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unit %d unable to create bg sem\n", unit);
    }
    /** update the unit number, use as global varaiable that the bg function can read from */
    dnx_crps_bg_thread_unit[unit] = unit;
    dnx_crps_bg_thread_data[unit].running = TRUE;
    dnx_crps_bg_thread_data[unit].background =
        sal_thread_create("DnxCtrBg", SAL_THREAD_STKSZ, 50, dnx_crps_eviction_counter_bg,
                          &dnx_crps_bg_thread_unit[unit]);
    if (SAL_THREAD_ERROR == dnx_crps_bg_thread_data[unit].background)
    {
        sal_sem_destroy(dnx_crps_bg_thread_data[unit].bgSem);
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unit %d unable to create counter background thread\n", unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_bg_thread_detach(
    int unit)
{
    soc_timeout_t to;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_crps_bg_thread_data[unit].running)
    {
        dnx_crps_thread_is_still_running[unit] = TRUE;
        dnx_crps_bg_thread_data[unit].running = FALSE;
        sal_sem_give(dnx_crps_bg_thread_data[unit].bgSem);
        soc_timeout_init(&to, DNX_CRPS_THREAD_WAIT_TIMEOUT, 0);
        /** wait up to two seconds for bg thread to finish its current loop */
        while (dnx_crps_thread_is_still_running[unit] == TRUE)
        {
            if (soc_timeout_check(&to))
            {
                if (dnx_crps_thread_is_still_running[unit] == FALSE)
                {
                    break;
                }
                else
                {
                    dnx_crps_thread_is_still_running[unit] = FALSE;
                    /** we cannot release the bgsem because we are not sure in this case, what is the thread status */
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "unit %d DnxCtrBg thread is not responding\n", unit);
                }
            }
        }
        sal_thread_destroy(dnx_crps_bg_thread_data[unit].background);
        sal_sem_destroy(dnx_crps_bg_thread_data[unit].bgSem);

        dnx_crps_bg_thread_data[unit].bgSem = NULL;
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "unit %d counter background thread exit\n"), unit));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
* \brief  
*   disconnect one CRPS FIFO from DMA channel
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit id
*   \param [in] core_id    -  0/1
*   \param [in] fifo_id    -  0/1
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
/** see .h file */
shr_error_e
dnx_crps_dma_fifo_detach(
    int unit,
    int core_id,
    int fifo_id)
{
    int lock_taken = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    /** only if the fifo wasn't connected to DMA channle, connect it */

    if (dnx_crps_fifo_channels[core_id][fifo_id] != DNX_CRPS_INVALID_DMA_CHANNEL)
    {
        /** use the cache lock to lock the bg thread process function. otherwise, we might free the memory the that process function works on. */
        SHR_IF_ERR_EXIT(dnx_crps_fifo_state_lock_take(unit));
        lock_taken = TRUE;

        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_free(unit, dnx_crps_fifo_channels[core_id][fifo_id]));

        dnx_crps_fifo_channels[core_id][fifo_id] = DNX_CRPS_INVALID_DMA_CHANNEL;
    }

exit:
    if (lock_taken)
    {
        /** allow access again */
        SHR_IF_ERR_EXIT(dnx_crps_fifo_state_lock_give(unit));
    }
    SHR_FUNC_EXIT;
}

/**
* \brief  
*    run over all CRPS FIFOs and disconnect them from CMIC dma channels mechanism.
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit id
*   \param [in] core    -  0/1/ALL
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_crps_eviction_dma_fifo_full_detach(
    int unit,
    bcm_core_t core)
{
    int core_id;
    int fifo_id;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, core, core_id)
    {
        DNX_CRPS_FIFOS_ITER(unit, DNX_CRPS_FIFO_ID_ALL, fifo_id)
        {
            SHR_IF_ERR_EXIT(dnx_crps_dma_fifo_detach(unit, core_id, fifo_id));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_dma_fifo_attach(
    int unit,
    int core_id,
    int fifo_id)
{
    int channel_number;
    soc_dnx_fifodma_config_t info;
    soc_dnx_fifodma_src_t src[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES][DNX_DATA_MAX_CRPS_EVICTION_NOF_COUNTERS_FIFO] =
        { {soc_dnx_fifodma_src_crps0_dma0, soc_dnx_fifodma_src_crps0_dma1},
    {soc_dnx_fifodma_src_crps1_dma0, soc_dnx_fifodma_src_crps1_dma1}
    };
    SHR_FUNC_INIT_VARS(unit);
    /** only if the fifo wasn't connected to DMA channle, connect it */
    if (dnx_crps_fifo_channels[core_id][fifo_id] == DNX_CRPS_INVALID_DMA_CHANNEL)
    {
        if (core_id >= DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid core id (=%d)\n", core_id);
        }
        if (fifo_id >= DNX_DATA_MAX_CRPS_EVICTION_NOF_COUNTERS_FIFO)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid fifo id (=%d)\n", fifo_id);
        }

        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_alloc(unit, src[core_id][fifo_id], &channel_number));

        SHR_IF_ERR_EXIT(soc_dnx_fifodma_config_t_init(unit, &info));

        dnx_crps_fifo_channels[core_id][fifo_id] = channel_number;

        info.is_mem = FALSE;
        info.is_poll = TRUE;
        info.force_entry_size = 0;
        info.max_entries = CRPS_EVICTION_FIFO_DMA_MAX_ENTRIES;
        info.reg = CRPS_CRPS_DMA_ACCESSr;
        info.threshold = CRPS_EVICTION_FIFO_DMA_THRESHOLD;
        info.timeout = CRPS_EVICTION_FIFO_DMA_TIMOUT;
        info.copyno = core_id;
        info.array_index = fifo_id;
        /*
         * Temporarily allow Sw state writes for current thread even if it's generally disabled.
         */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SW_STATE));

        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_start(unit, channel_number, &info, NULL));

        /*
         * revert dnxc_allow_hw_write_enable.
         */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SW_STATE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   run over all engines and find all active CRPS FIFOs and connect DMA channels for them
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit id
*   \param [in] core    -  0/1/ALL
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * used for warm-boot. (in cold boot, all engines are not valid yet)
* \see
*   * None
*/
shr_error_e
dnx_crps_dma_fifo_full_attach(
    int unit,
    bcm_core_t core)
{
    int core_id, engine_id;
    int fifo_id;
    int valid;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, core, core_id)
    {
        for (engine_id = 0; engine_id < dnx_data_crps.engine.nof_engines_get(unit); engine_id++)
        {
            /** if engine is valid, connected to local host and in phy record_format, need to attach it */
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get(unit, core_id, engine_id, &valid));
            if (valid)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.get(unit, core_id, engine_id, &fifo_id));
                /** attach this fifo_id. will be made only if not already attached */
                SHR_IF_ERR_EXIT(dnx_crps_dma_fifo_attach(unit, core_id, fifo_id));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   wrapper for attach the bg thread and connect the DMA FIFO to host
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_crps_eviction_counter_attach_init(
    int unit)
{
    int core_idx, engine_idx;
    int bg_thread_enable_called = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /** attach dma channels for all active engines and if needed, also acivate the bg thread one time */
    /** attached it anyway, even if bg thread is disabled, for users who has their own bg thread */
    /** relevant only in warmboot. for cold boot, all engines are not active yet, so no need to attach. */
    /** each time new engine will be enabled, we will attached its fifo */
    if (SOC_WARM_BOOT(unit))
    {
        DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_idx)
        {
            for (engine_idx = 0; engine_idx < dnx_data_crps.engine.nof_engines_get(unit); engine_idx++)
            {
                /** attach dma fifo and activate bg thread if needed. The handle function check and activate FIFO/Bg thread if need to */
                SHR_IF_ERR_EXIT(dnx_crps_eviction_collection_handle
                                (unit, core_idx, engine_idx, &bg_thread_enable_called));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  enable/disable the eviction for all engines, that the database indicated that their eviction should be enabled.
 *  Enable/Disable is made in this function without update the sw state.
 *  It is made to ensure during warmboot, there will not be any eviction process.
 * \param [in] unit -unit id
 * \param [in] enable -True=enable/ false=disable
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_eviction_enable_all_active(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    int core_idx;
    int engine_idx;
    int sw_state_eviction_enable, valid;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_idx)
    {
        for (engine_idx = 0; engine_idx < dnx_data_crps.engine.nof_engines_get(unit); engine_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get(unit, core_idx, engine_idx, &valid));
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.enablers.eviction_enable.get
                            (unit, core_idx, engine_idx, &sw_state_eviction_enable));
            /** only if the engine eviction is valid and eviction is enabled - enable/disable the eviction */
            if (sw_state_eviction_enable == TRUE && valid == TRUE)
            {
                /** Taking a handle */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_ENABLE, &entry_handle_id));

                /** Setting key fields */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine_idx);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_idx);
                /** enable/disable the eviction */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_ENABLE, INST_SINGLE,
                                             enable);
                /** Preforming the action */
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                DBAL_HANDLE_FREE(unit, entry_handle_id);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_eviction_init(
    int unit)
{
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** allocate array of the cache counters and memset it. Only the first 'counter_fifo_depth' counters are relevant. */
    dnx_crps_fifo_results[unit].nof_counters = 0;
    dnx_crps_fifo_results[unit].cnt_result = (dnx_crps_eviction_ctr_result_t *) utilex_alloc
        (sizeof(*(dnx_crps_fifo_results[unit].cnt_result)) * dnx_data_crps.eviction.counters_fifo_depth_get(unit));

    SHR_NULL_CHECK(dnx_crps_fifo_results[unit].cnt_result, _SHR_E_INTERNAL, "cnt_result");

    /** init to invalid (-1) the fifo_channel db */
    for (core_id = 0; core_id < DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES; core_id++)
    {
        sal_memset(dnx_crps_fifo_channels[core_id], DNX_CRPS_INVALID_DMA_CHANNEL,
                   (sizeof(int) * DNX_DATA_MAX_CRPS_EVICTION_NOF_COUNTERS_FIFO));
    }

    /** clear bg thread database */
    sal_memset(&dnx_crps_bg_thread_data[unit], 0, sizeof(dnx_crps_bg_thread_data[unit]));
    dnx_crps_fifo_lock[unit] = sal_mutex_create("dnx crps fifo interlock");
    /** create mutex interlock on the dma fifo  access */
    if (!dnx_crps_fifo_lock[unit])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unable to create unit %d fifo" " interlock mutex\n", unit);
    }
    dnx_crps_cache_lock[unit] = sal_mutex_create("dnx crps cache interlock");
    /** create mutex interlock on the cache memory access */
    if (!dnx_crps_cache_lock[unit])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unable to create unit %d cache" " interlock mutex\n", unit);
    }
    /** attach the bg thread and DMA FIFO */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_counter_attach_init(unit));

    /** if warmboot, enable eviction for all engines that their sw state indicates on eviction enabled  */
    if (SOC_WARM_BOOT(unit))
    {
        /** Allow writing to DBAL for the DMA init time */
        SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_DBAL));

        SHR_IF_ERR_EXIT(dnx_crps_eviction_enable_all_active(unit, TRUE));

        /** Return to warmboot normal mode */
        SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_DBAL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  function calling the fifo process procedure by demend, as foreground operation. 
 *  the function lock the counters database, before calling the process and release it after.
 *
 * \param [in] unit -unit id
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_crps_eviction_foreground_fifo_process(
    int unit)
{
    int depth;
    int lock_taken = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /** lock using mutex instead of spin loops */
    SHR_IF_ERR_EXIT(dnx_crps_fifo_state_lock_take(unit));
    lock_taken = TRUE;
    /** process all of the active FIFOs */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_counter_fifo_process(unit, _SHR_CORE_ALL, DNX_CRPS_FIFO_ID_ALL, &depth));

exit:
    if (lock_taken)
    {
        /** allow access again */
        SHR_IF_ERR_EXIT(dnx_crps_fifo_state_lock_give(unit));
    }
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_disable_wrapper(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    

    /** disable the eviction from all engines */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_enable_all_active(unit, FALSE));

    /** wait 20 micro to ensure last counters were copied to cpu */
    sal_usleep(20);

    /** process the fifo one more time before exit */
    SHR_IF_ERR_EXIT(dnx_crps_eviction_foreground_fifo_process(unit));

    /** destroy the background thread */
    SHR_IF_ERR_CONT(dnx_crps_eviction_bg_thread_detach(unit));
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Allow writing to registers and changing SW state for the DMA deinit time */
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_DBAL));
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SCHAN));
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SW_STATE));

    /** disable the eviction from all engines */
    SHR_IF_ERR_CONT(dnx_crps_eviction_disable_wrapper(unit));
    /** detach the DMA FIFO */
    SHR_IF_ERR_CONT(dnx_crps_eviction_dma_fifo_full_detach(unit, _SHR_CORE_ALL));

    /** Return to warmboot normal mode */
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SW_STATE));
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SCHAN));
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_DBAL));

    /** destroy the sw fifo memory mutex */
    if (dnx_crps_fifo_lock[unit])
    {
        sal_mutex_destroy(dnx_crps_fifo_lock[unit]);
        dnx_crps_fifo_lock[unit] = NULL;
    }
    /** destroy the sw cache memory mutex */
    if (dnx_crps_cache_lock[unit])
    {
        sal_mutex_destroy(dnx_crps_cache_lock[unit]);
        dnx_crps_cache_lock[unit] = NULL;
    }

    /** free the fifo result array */
    utilex_free((void *) dnx_crps_fifo_results[unit].cnt_result);

    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_counters_get(
    int unit,
    int flags,
    int core_id,
    int engine_id,
    int nstat,
    int *counters_id,
    int *sub_count,
    bcm_stat_counter_output_data_t * output_data)
{
    int cnt_idx;
    uint64 value;
    int lock_taken = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_take(unit));
    lock_taken = TRUE;
    /** read the counter from sw state counters database */
    for (cnt_idx = 0; cnt_idx < nstat; cnt_idx++)
    {
        if ((counters_id[cnt_idx] != -1) && (sub_count[cnt_idx] != -1))
        {
            SHR_IF_ERR_EXIT(dnx_crps_ctrs_db_counter_get
                            (unit, flags, core_id, engine_id, counters_id[cnt_idx], sub_count[cnt_idx], &value));
            output_data->value_arr[cnt_idx].value = value;
        }
    }

exit:
    if (lock_taken)
    {
        SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_give(unit));
    }
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_seq_timer_set(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 time_between_scan_usec)
{
    uint32 seq_timer_interval, alg_timer_interval;
    uint32 entry_handle_id;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    /** Get alg timer interval */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_ALG_SCANS,
                               INST_SINGLE, &alg_timer_interval);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Setting value fields */
    /** seq_timer_interval calculation. result is in clocks resolution */
    COMPILER_64_ZERO(time.time);
    COMPILER_64_ADD_32(time.time, time_between_scan_usec);
    time.time_units = DNXCMN_TIME_UNIT_USEC;
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &seq_timer_interval));
    /** if alg and seq overlap, alg has priority over seq causing
     *  seq not to be able to evict */
    if (alg_timer_interval == seq_timer_interval)
    {
        seq_timer_interval += 1;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_SEQ_SCANS,
                                 INST_SINGLE, seq_timer_interval);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_seq_timer_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 *time_between_scan_usec)
{
    uint32 entry_handle_id;
    uint32 seq_timer_interval;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_SEQ_SCANS,
                               INST_SINGLE, &seq_timer_interval);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** convert clocks to micro sec */
    SHR_IF_ERR_EXIT(dnxcmn_clock_cycles_to_time_get(unit, seq_timer_interval, DNXCMN_TIME_UNIT_USEC, &time));

    *time_between_scan_usec = COMPILER_64_LO(time.time);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_boundaries_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    bcm_stat_counter_format_type_t format;
    int nof_counters_per_entry;
    uint32 entry_handle_id;
    uint32 first, last;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    /** Setting value fields */
    /** set default values */
    if (flags & BCM_STAT_EVICTION_RANGE_ALL)
    {
        /** fisrt addres to scan sequentialy is alsway zero, unless the user asked something else explicitly. */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_START,
                                     INST_SINGLE, 0);

        /** get how many counters in each one counter entry: 2 counters for pkts_and_pkts and max formats, else 1 counter */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.format.get(unit, engine->core_id, engine->engine_id, &format));
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_nof_counters_per_entry_get(unit, format, &nof_counters_per_entry));
         /** the last address to scan sequentialy, is calculated according to the range size */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_first.get
                        (unit, engine->core_id, engine->engine_id, &first));
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.logical_object_id_last.get
                        (unit, engine->core_id, engine->engine_id, &last));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_END,
                                     INST_SINGLE, (last - first));
    }
    /** set the user given values */
    else
    {
        /**  fisrt addres to scan sequentialy is given by the user  */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_START,
                                     INST_SINGLE, boundaries->start);

         /** the last address to scan sequentialy is given by the user */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_END,
                                     INST_SINGLE, boundaries->end);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_boundaries_get(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries)
{
    uint32 start, end;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ENGINE_SCAN_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_ID, engine->engine_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, engine->core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_START,
                               INST_SINGLE, &start);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_END,
                               INST_SINGLE, &end);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    boundaries->start = start;
    boundaries->end = end;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_eviction_collection_handle(
    int unit,
    int core_id,
    int engine_id,
    int *bg_thread_enable_called)
{
    int bg_thread_enable;
    int valid;
    int fifo_id;

    SHR_FUNC_INIT_VARS(unit);

    /** attach dma fifo before enable the CRPS hw. attach will be made only if fifo_id is not already attached */
    /** if engine is valid, connected to local host and has phy record format, need to attach it and also activate the bg thread (if not active) */
    SHR_IF_ERR_EXIT(dnx_crps_mgmt_engine_eviction_valid_get(unit, core_id, engine_id, &valid));
    if (valid)
    {
        /** get from db the fifo_id parameters */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.eviction.dma_fifo.get(unit, core_id, engine_id, &fifo_id));

        /** dma fifo attach. it will be made only if not already attached. */
        /** (attach should be made even if bg thread disable for user who mange their own bg thread */
        SHR_IF_ERR_EXIT(dnx_crps_dma_fifo_attach(unit, core_id, fifo_id));

        /** get from sw state  (updated by Soc/API user config, if the user decide to enable the thread  */
        SHR_IF_ERR_EXIT(dnx_crps_db.bg_thread_enable.get(unit, &bg_thread_enable));

        /** attach the bg thread one time. */
        /** it happens in the first engine enable with valid eviction */
        if ((bg_thread_enable == TRUE) &&
            (dnx_crps_bg_thread_data[unit].running == FALSE) && (*bg_thread_enable_called == FALSE))
        {
            LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "unit %d counter background thread init\n"), unit));
            dnx_crps_bg_thread_data[unit].bgWait = DNX_CRPS_BACKGROUND_THREAD_ITER_DEFAULT(unit);
            SHR_IF_ERR_EXIT(dnx_crps_eviction_bg_thread_attach(unit));
            *bg_thread_enable_called = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_bg_thread_detach_check(
    int unit)
{
    int core_idx, fifo_id;
    int bg_thread_detach = TRUE;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_idx)
    {
        DNX_CRPS_FIFOS_ITER(unit, DNX_CRPS_FIFO_ID_ALL, fifo_id)
        {
            if (dnx_crps_fifo_channels[core_idx][fifo_id] != DNX_CRPS_INVALID_DMA_CHANNEL)
            {
                bg_thread_detach = FALSE;
                break;
            }
        }
    }
    /** if all FIFOs (all cores) are invalid, no need to keep the bg thread active.*/
    if (bg_thread_detach == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_crps_eviction_bg_thread_detach(unit));
    }

exit:
    SHR_FUNC_EXIT;
}
