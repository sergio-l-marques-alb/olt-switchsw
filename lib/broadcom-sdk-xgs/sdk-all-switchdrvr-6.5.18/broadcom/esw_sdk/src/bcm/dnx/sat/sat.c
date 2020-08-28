/** \file sat.c
 * $Id$
 *
 * SAT procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SAT

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <bcm/sat.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <bcm_int/dnx/sat/sat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/intr/auto_generated/jr2/jr2_intr.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/dnxc_ha.h>

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** Index of LSB in MAC */
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

 /*
  * Global and Static
  */

/*
 * }
 */

/**
* \brief
*   Set SAT time & clock information .
* \par
* \param [in] unit  -  Relevant unit.
* \param [in] sat_init  - SAT init information.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See  shr_error_e
* \remark
* * None
*/

static shr_error_e
dnx_sat_timer_cfg_init(
    int unit,
    dnx_sat_init_t * sat_init)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TIMER_CONFIG, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_CLKS_IN_CYCLE, INST_SINGLE,
                                 sat_init->rate_num_clks_cycle);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_CLOCKS_SEC, INST_SINGLE, sat_init->num_clks_sec);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Set SAT GTF credit backpressure information .
* \par
* \param [in] unit  -  Relevant unit.
* \param [in] credit_backpressure  - SAT GTF credit backpressure information.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See  shr_error_e
* \remark
* * None
*/

static shr_error_e
dnx_sat_credit_cfg_set(
    int unit,
    int credit_backpressure)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (int i = 0; i < 4; i++)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_CONFIG, &entry_handle_id));

        /** setting key fields */
        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_SAT_PIPE_ID, i);

        /** setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_BACKPRESSURE, INST_SINGLE,
                                     credit_backpressure);

        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Set SAT EVC paring information .
* \par
* \param [in] unit  -  Relevant unit.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See  shr_error_e
* \remark
* * None
*/

static shr_error_e
dnx_sat_evc_paring_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 evc_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (evc_id = 0; evc_id < DNX_DATA_MAX_SAT_GENERATOR_NOF_EVC_IDS; evc_id++)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_EVC_PAIR_CONFIG, &entry_handle_id));

        /** setting key fields */
        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_EVC_ID, evc_id);

        /** setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVC_PAIR_SET, INST_SINGLE, 0);

        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function initializes the SAT interript
 *          pending event mask.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * This function is part of the SAT initialization process,
 *     and is called from dnx_sat_init.
 */
shr_error_e
dnx_sat_init_interrupt(
    int unit)
{
    bcm_switch_event_control_t type;
    bcm_switch_event_t switch_event = BCM_SWITCH_EVENT_DEVICE_INTERRUPT;
    uint32 enable = 1;

    SHR_FUNC_INIT_VARS(unit);

    /** Force unmask the interrupt first and then set mask  to zero */
    type.event_id = dnx_data_oam.oamp.intr_oamp_stat_pending_event_get(unit);
    type.action = bcmSwitchEventForceUnmask;
    type.index = BCM_CORE_ALL;

    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, switch_event, type, enable));

    type.action = bcmSwitchEventMask;
    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, switch_event, type, !enable));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Init SAT function.
* \par
*   \param [in] unit  -  Relevant unit.
*   \param [out]   - none
*
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See shr_error_e
*
* \remark
*   * None
*/

shr_error_e
bcm_dnx_sat_init(
    int unit)
{
    uint32 freq_hz = 0;
    int granularity = 1000;     /* 1kbits per sec */
    uint8 dma_event_interface_enable = 0;

    /*
     * dnx_algo_res_create_data_t data;
     */
    dnx_sat_init_t dnx_sat_init;
    dnx_sat_gtf_pkt_hdr_tbl_info_t pkt_hdr_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Time tickets
     */
    freq_hz = dnx_data_device.general.core_clock_khz_get(unit) * 1000;  /* Khz*1000 */

    granularity = DNX_DATA_MAX_SAT_GENERAL_GRANULARITY;
    dnx_sat_init.num_clks_sec = freq_hz;
    dnx_sat_init.rate_num_clks_cycle = freq_hz / (granularity / DNX_SAT_BITS_PER_BYTE); /* 1kbits per sec(125 cycles
                                                                                         * per sec), since the rate
                                                                                         * granularity 1byte/s */

    SHR_IF_ERR_EXIT(dnx_sat_timer_cfg_init(unit, &dnx_sat_init));
    SHR_IF_ERR_EXIT(dnx_sat_credit_cfg_set(unit, DNX_SAT_DEFAULT_CREDIT_BACKPRESSURE));

    SHR_IF_ERR_EXIT(dnx_sat_evc_paring_init(unit));

    sal_memset(&pkt_hdr_info, 0x0, sizeof(dnx_sat_gtf_pkt_hdr_tbl_info_t));
    SHR_IF_ERR_EXIT(dnx_am_template_sat_pkt_header_create(unit, DNX_SAT_PKT_HEADER_DEFAULT_PROFILE, &pkt_hdr_info));

    dma_event_interface_enable = dnx_data_oam.oamp.oamp_fifo_dma_report_interface_enable_get(unit);
    /*
     * code 
     */
    if (dma_event_interface_enable)
    {
        /** Initialization of SAT interrupts DMA */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_stat_dma_init(unit));
    }
    else
    {
        /** Initialization of SAT interrupt*/
        if (!soc_is(unit, J2P_DEVICE))
        {
            SHR_IF_ERR_EXIT(dnx_sat_init_interrupt(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

 /**
  * \brief - initialize SAT module, to be called from init
  *    sequence.
  *
  * \param [in] unit - unit number
  *
  * \return
  *   shr_error_e
  *
  * \remark
  *   * None
  * \see
  *   * None
  */
shr_error_e
dnx_sat_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_sat_init(unit));
exit:
    SHR_FUNC_EXIT;
}

 /**
  * \brief - de-init SAT module, to be called from init
  *    sequence.
  *
  * \param [in] unit - unit number
  *
  * \return
  *   shr_error_e
  *
  * \remark
  *   * None
  * \see
  *   * None
  */
shr_error_e
dnx_sat_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Deinit SAT's interrupts DMA */
    SHR_IF_ERR_CONT(dnx_oam_oamp_stat_dma_deinit(unit));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *    Get SAT general configuration information from HW table.
 * \par
 *  \param [in] unit  -  Relevant unit.
 *  \param [out] conf - SAT configuration information.
 * \retval
 *  \retval Zero if no error was detected
 *  \retval Negative if error was detected. See shr_error_e
 * \remark
 *  * None
 */
static shr_error_e
dnx_sat_gen_cfg_get(
    int unit,
    bcm_sat_config_t * conf)
{
    uint32 entry_handle_id;
    uint32 field_data_crc = 0;
    uint32 field_rev_crc = 0;
    uint32 field_crc_high = 0;
    uint32 field_tod = 0;
    uint32 field_prbs = 0;
    uint32 field_twamp_mode = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * (1) GEN config table
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_GEN_CONFIG, &entry_handle_id));

    /** getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RVERSED_DATA_FOR_CRC, INST_SINGLE, &field_data_crc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INVERT_REVERSED_CRC, INST_SINGLE, &field_rev_crc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RST_CRC_TO_HIGH, INST_SINGLE, &field_crc_high);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_USE_NTP_TOD, INST_SINGLE, &field_tod);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_USE_NXOR_FOR_PRBS, INST_SINGLE, &field_prbs);
    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_twamp_mode))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TWAMP_MODE, INST_SINGLE, &field_twamp_mode);
    }
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (field_data_crc)
    {
        conf->config_flags |= BCM_SAT_CONFIG_CRC_REVERSED_DATA;
    }
    if (field_rev_crc)
    {
        conf->config_flags |= BCM_SAT_CONFIG_CRC_INVERT;
    }
    if (field_crc_high)
    {
        conf->config_flags |= BCM_SAT_CONFIG_CRC_HIGH_RESET;
    }
    conf->timestamp_format = (field_tod ? bcmSATTimestampFormatNTP : bcmSATTimestampFormatIEEE1588v1);
    if (field_twamp_mode)
    {
        conf->config_flags |= BCM_SAT_CONFIG_TWAMP_MODE;
    }
    if (field_prbs)
    {
        conf->config_flags |= BCM_SAT_CONFIG_PRBS_USE_NXOR;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 * To verify sat general config for BCM-API: bcm_sat_config_get*() .
 * \par
 * \param [in] unit  -Relevant unit.
 * \param [in] conf  -  Sat common configuration Object
 * \retval
 * \retval Zero if no error was detected
 * \retval Negative if error was detected. See shr_error_e
 * \remark
 * * None
 */
static shr_error_e
dnx_sat_config_get_verify(
    int unit,
    bcm_sat_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(conf, _SHR_E_PARAM, "conf ");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     Get SAT general configuration information.
* \par DIRECT INPUT:
*   \param [in] unit  -  Relevant unit.
*   \param [out] conf  - SAT configuration information.
*
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See shr_error_e
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_config_get(
    int unit,
    bcm_sat_config_t * conf)
{
    bcm_sat_config_t dnx_conf;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_config_get_verify(unit, conf));

    sal_memset(&dnx_conf, 0, sizeof(bcm_sat_config_t));
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) \n", BSL_FUNC, BSL_LINE, unit));

    SHR_IF_ERR_EXIT(dnx_sat_gen_cfg_get(unit, &dnx_conf));

    conf->config_flags = dnx_conf.config_flags;
    conf->timestamp_format = dnx_conf.timestamp_format;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *    SAT general configuration in HW table .
 * \par
 *  \param [in] unit  -  Relevant unit.
 *  \param [in] conf  - SAT general information.
 * \retval
 *  \retval Zero if no error was detected
 *  \retval Negative if error was detected. See shr_error_e
 * \remark
 *  * None
 */
static shr_error_e
dnx_sat_gen_cfg_set(
    int unit,
    bcm_sat_config_t * conf)
{
    uint32 entry_handle_id;
    uint32 field = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * (1) GEN config table
     */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_GEN_CONFIG, &entry_handle_id));

    /** Setting value fields */
    field = (conf->config_flags & BCM_SAT_CONFIG_CRC_REVERSED_DATA ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RVERSED_DATA_FOR_CRC, INST_SINGLE, field);

    field = (conf->config_flags & BCM_SAT_CONFIG_CRC_INVERT ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INVERT_REVERSED_CRC, INST_SINGLE, field);

    field = (conf->config_flags & BCM_SAT_CONFIG_CRC_HIGH_RESET ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RST_CRC_TO_HIGH, INST_SINGLE, field);

    field = (conf->timestamp_format == bcmSATTimestampFormatNTP ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_NTP_TOD, INST_SINGLE, field);

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_twamp_mode))
    {
        field = (conf->config_flags & BCM_SAT_CONFIG_TWAMP_MODE ? 1 : 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TWAMP_MODE, INST_SINGLE, field);
    }

    field = (conf->config_flags & BCM_SAT_CONFIG_PRBS_USE_NXOR ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_NXOR_FOR_PRBS, INST_SINGLE, field);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * SEQ number wrap-around only for J2_B0
     */
    if (dnx_data_sat.tx_flow_param.feature_get(unit, dnx_data_sat_tx_flow_param_seq_number_wrap_around))
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_SEQ_NUM_WRAP_AROUND, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEQ_NUM_WRAP_AROUND, INST_SINGLE,
                                     conf->seq_number_wrap_around);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 * To verify sat general config for BCM-API: bcm_sat_config_set*() .
 * \par
 * \param [in] unit  - Relevant unit.
 * \param [in] conf  -  Sat common configuration Object
 * \retval
 * \retval Zero if no error was detected
 * \retval Negative if error was detected. See shr_error_e
 * \remark
 * * None
 */
static shr_error_e
dnx_sat_config_set_verify(
    int unit,
    bcm_sat_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(conf, _SHR_E_PARAM, "conf ");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   SAT general configuration set .
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] conf  -
*   SAT configuration information.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_config_set(
    int unit,
    bcm_sat_config_t * conf)
{
    bcm_sat_config_t dnx_conf;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_config_set_verify(unit, conf));

    SHR_RANGE_VERIFY(conf->timestamp_format, bcmSATTimestampFormatIEEE1588v1, bcmSATTimestampFormatNTP, _SHR_E_PARAM,
                     "timestamp_format");
    LOG_VERBOSE(BSL_LS_BCM_SAT,
                ("%s %d: u(%d) timestamp_format (%d) config_flags(%d)\n", BSL_FUNC, BSL_LINE, unit,
                 conf->timestamp_format, conf->config_flags));

    sal_memset(&dnx_conf, 0, sizeof(bcm_sat_config_t));
    dnx_conf.config_flags = conf->config_flags;
    dnx_conf.timestamp_format = conf->timestamp_format;

    SHR_IF_ERR_EXIT(dnx_sat_gen_cfg_set(unit, &dnx_conf));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   SAT trap data set in SW table .
* \par
* \param [in] unit  -  Relevant unit.
* \param [in] psat_data  - SAT sw table information.
* \param [in] index  - trap index.
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_trap_data_set(
    int unit,
    dnx_sat_data_t * psat_data,
    int index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TRAP_DATA_SW, &entry_handle_id));

    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TRAP_INDEX, index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SSID, INST_SINGLE, psat_data->trap_data[index].ssid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_DATA, INST_SINGLE,
                                 psat_data->trap_data[index].trap_data);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_MASK, INST_SINGLE,
                                 psat_data->trap_data[index].trap_mask);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *    SAT sw data set.
 * \par
 *  \param [in] unit  -  Relevant unit.
 *  \param [in] psat_data  - SAT sw table information.
 * \retval
 *  \retval Zero if no error was detected
 *  \retval Negative if error was detected. See shr_error_e
 * \remark
 *  * None
 */
shr_error_e
dnx_sat_data_set(
    int unit,
    dnx_sat_data_t * psat_data)
{
    uint32 entry_handle_id;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_STATE_SW, &entry_handle_id));

    /** Value fields */
    for (index = 0; index < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_ID_ENTRIES; index++)
    {
        /** Loop over all TRAP ID */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_ID, index,
                                     psat_data->ctf_trap_id[index]);
    }

    for (index = 0; index <= DNX_SAT_GTF_ID_MAX; index++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GTF_HEADER_ID, index,
                                     psat_data->gtf_header_id[index]);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_MODE, INST_SINGLE, psat_data->rate_mode);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GRANULARITY_SET, INST_SINGLE,
                                 psat_data->granularity_flag);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Get ctf trap DATA
     */
    for (index = 0; index < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_DATA_ENTRIES; index++)
    {
        SHR_IF_ERR_EXIT(dnx_sat_trap_data_set(unit, psat_data, index));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *    Get SAT trap data from sw data table .
 * \par
 *  \param [in] unit  -  Relevant unit.
 *  \param [in] psat_data  - SAT sw table information.
 *  \param [out] index -   trap index.
 * \retval
 *  \retval Zero if no error was detected
 *  \retval Negative if error was detected. See shr_error_e
 * \remark
 *  * None
 */
static shr_error_e
dnx_sat_trap_data_get(
    int unit,
    dnx_sat_data_t * psat_data,
    int index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TRAP_DATA_SW, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TRAP_INDEX, index);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SSID, INST_SINGLE, &psat_data->trap_data[index].ssid);
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_DATA, INST_SINGLE,
                               &psat_data->trap_data[index].trap_data);
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_MASK, INST_SINGLE,
                               &psat_data->trap_data[index].trap_mask);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

  /**
  * \brief
  *  Get SAT sw data .
  * \par
  *   \param [in] unit -  Relevant unit.
  *   \param [out] psat_data  - SAT sw table information.
  * \retval
  *   \retval Zero if no error was detected
  *   \retval Negative if error was detected. See shr_error_e
  * \remark
  *   * None
  */
shr_error_e
dnx_sat_data_get(
    int unit,
    dnx_sat_data_t * psat_data)
{
    uint32 entry_handle_id;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_STATE_SW, &entry_handle_id));

     /** Value fields */
    for (index = 0; index < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_ID_ENTRIES; index++)
    {
        /** Loop over all TRAP ID */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_ID, index,
                                   &psat_data->ctf_trap_id[index]);
    }
    for (index = 0; index <= DNX_SAT_GTF_ID_MAX; index++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GTF_HEADER_ID, index,
                                   &psat_data->gtf_header_id[index]);
    }

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_RATE_MODE, INST_SINGLE, &(psat_data->rate_mode));

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_GRANULARITY_SET, INST_SINGLE,
                              &(psat_data->granularity_flag));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Get ctf trap DATA
     */
    for (index = 0; index < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_DATA_ENTRIES; index++)
    {
        SHR_IF_ERR_EXIT(dnx_sat_trap_data_get(unit, psat_data, index));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify SAT EVENT unregister function for BCM-API: bcm_sat_event_register*() .
* \par
* \param [in] unit - Relevant unit.
* \param [out] user_data  -user data
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_event_register_verify(
    int unit,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(user_data, _SHR_E_PARAM, "user_data ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  SAT EVENT register function.
* \param [in] unit  -
*   Relevant unit.
* \param [in] event_type  -
*    SAT event types
* \param [in] cb  -
*     Callback function type for SAT event handling
* \param [out] user_data  -
*    user data
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_event_register(
    int unit,
    bcm_sat_event_type_t event_type,
    bcm_sat_event_cb cb,
    void *user_data)
{
    bcm_sat_event_type_t type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_event_register_verify(unit, user_data));

    for (type = 0; type < bcmSATEventCount; type++)
    {
        if (type == event_type)
        {
            if (_g_dnx_sat_event_cb[unit][type] && (_g_dnx_sat_event_cb[unit][type] != cb))
            {
                /*
                 * A different calblack is already registered for this event. Return error
                 */
                SHR_ERR_EXIT(_SHR_E_EXISTS, "EVENT %d already has a registered callback ", type);
            }
            _g_dnx_sat_event_cb[unit][type] = cb;
            _g_dnx_sat_event_ud[unit][type] = user_data;
        }
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* To verify SAT EVENT unregister function for BCM-API: bcm_sat_event_unregister*() .
* \par
* \param [in] unit - Relevant unit.
* \param [in] event_type  - SAT event types
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See shr_error_e
* \remark
* * None
*/
static shr_error_e
dnx_sat_event_unregister_verify(
    int unit,
    bcm_sat_event_type_t event_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_RANGE_VERIFY(event_type, bcmSATEventReport, bcmSATEventCount, _SHR_E_PARAM, "event_type");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*SAT report handle*/
shr_error_e
dnx_sat_ctf_report_process(
    int unit,
    dnx_sat_event_data_t * event)
{
    bcm_sat_report_event_data_t report;
    bcm_sat_event_type_t type = bcmSATEventReport;
    SHR_FUNC_INIT_VARS(unit);

    if (!event || (!event->is_valid))
    {
        SHR_IF_ERR_EXIT(BCM_E_PARAM);
    }
    sal_memset(&report, 0, sizeof(bcm_sat_report_event_data_t));

    report.ctf_id = event->flow_id;
    if (event->d_set)
    {
        report.delay = event->frame_delay;
        report.delay_valid = 1;
    }

    if (event->s_set)
    {
        report.sequence_number = event->seq_num;
        report.sequence_number_valid = 1;
    }

    if (_g_dnx_sat_event_cb[unit][type])
    {
        _g_dnx_sat_event_cb[unit][type] (unit, type, (void *) &report, _g_dnx_sat_event_ud[unit][type]);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  SAT EVENT unregister function.
* \param [in] unit  -
*   Relevant unit.
* \param [in] event_type  -
*    SAT event types
* \param [in] cb  -
*     Callback function type for SAT event handling
* \param [out]   -
*    None
* \retval
* \retval Zero if no error was detected
* \retval Negative if error was detected. See shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_event_unregister(
    int unit,
    bcm_sat_event_type_t event_type,
    bcm_sat_event_cb cb)
{
    bcm_sat_event_type_t type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_sat_event_unregister_verify(unit, event_type));

    for (type = 0; type < bcmSATEventCount; type++)
    {
        if (type == event_type)
        {
            if (_g_dnx_sat_event_cb[unit][type] && (_g_dnx_sat_event_cb[unit][type] != cb))
            {
                /*
                 * A different calblack is already registered for this event. Return error
                 */
                SHR_ERR_EXIT(_SHR_E_EXISTS, "EVENT %d already has a registered callback ", type);
            }
            _g_dnx_sat_event_cb[unit][type] = NULL;
            _g_dnx_sat_event_ud[unit][type] = NULL;
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
