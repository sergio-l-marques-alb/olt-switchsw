/**
 * \file instru_eventor.c
 *
 * Eventor: Eventor APIs implementation
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EVENTOR

#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <shared/shrextend/shrextend_debug.h>
#include <soc/drv.h>
#include <soc/mem.h>

#if !defined(BCM_ESW_SUPPORT)
/* { */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif
/* } */
#endif
#include <soc/mcm/memregs.h>

/*
 * Include files.
 * {
 */
#include <soc/sbusdma.h>
#include <soc/cm.h>
#include <bcm/instru.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>       /* needed for the next line */
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/instru_access.h>
#include <bcm_int/dnx/instru/instru_eventor.h>

#include <soc/dnx/dbal/dbal.h>  /* for dbal */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>    /* for dnx_data_device */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>      /* for ucode_port */
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/cmicx.h>

/*
 * }
 */
/* *INDENT-OFF* */

/*************
 * TYPE DEFS *
 *************/
#define DNX_EVENTOR_NOF_BUFFERS_IN_DOUBLE_BUFFER 2

#define MAX_NOF_EVENTOR_SBUS_DMA_CHANNELS (2*8)

/** The SRAM is mapped to the 7th 1MB window in the 2nd (CMIC) BAR */
#define EVENTOR_SRAM_OFFSET_IN_BAR (7*1024*1024)

#define EVENTOR_SRAM_DIRECT_MEM_ACCESS(unit, offset_in_bytes) \
    ((EVENTOR_SRAM_OFFSET_IN_BAR + (offset_in_bytes)))

/** macro used for getting DNX data constants instead of constant macros */
#define DDC(dnx_data_field) dnx_data_instru.eventor.dnx_data_field##_get(unit)

#define DNX_EVENTOR_NOF_CONTEXTS(unit) DDC(nof_contexts) /** number of contexts (rx channels/tx contexts) value=16 */
#define DNX_EVENTOR_NOF_BUILDERS(unit) DDC(nof_builders) /** number of builders, assumed to not be smaller than nof contexts value=16 */
#define DNX_EVENTOR_NOF_SRAM_BANKS(unit) DDC(nof_sram_banks) /** number of SRAM banks value=8 */
#define DNX_EVENTOR_SRAM_BANK_BITS(unit) DDC(sram_bank_bits) /** number of bits needed to store an SRAM bank number value=3 */
#define DNX_EVENTOR_SRAM_BANK_MASK(unit) DDC(sram_bank_mask) /** mask needed to store an SRAM bank number value=((1 << DNX_EVENTOR_SRAM_BANK_BITS) - 1)) */
#define DNX_EVENTOR_SRAM_BANKS_FULL_SIZE(unit) DDC(sram_banks_full_size) /** size of an SRAM including ECC value=16*1024) */
#define DNX_EVENTOR_SRAM_BANKS_NET_SIZE(unit) DDC(sram_banks_net_size) /** size of an SRAM bank excluding ECC value=13*1024) */
#define DNX_EVENTOR_SRAM_BANKS_FULL_SIZE_WORDS(unit) (DDC(sram_banks_full_size) / 4) /** size of an SRAM bank including ECC value=16*1024/4) */
#define DNX_EVENTOR_SRAM_BANKS_NET_SIZE_WORDS(unit) (DDC(sram_banks_net_size) / 4) /** size of an SRAM excluding ECC value=13*1024/4) */
#define DNX_EVENTOR_BUILDER_MAX_BUFFER_SIZE_WORDS(unit) (DDC(builder_max_buffer_size) / 4) /** max size of a builder buffer in words value=1600/4) */
#define DNX_EVENTOR_RX_MAX_BUFFER_SIZE_WORDS(unit) (DDC(rx_max_buffer_size) / 4) /** max size of an RX buffer in words value=1600/4) */
#define DNX_EVENTOR_MAX_BUILDER_TIMEOUT_MS(unit) DDC(builder_max_timeout_ms) /** max possible configurable timeout value=(((uint32)-1) / 1000000)) */
#define DNX_EVENTOR_AXI_SRAM_OFFSET(unit) DDC(axi_sram_offset) /** SRAM offset in AXI: 0xe8000000 */
#define DNX_EVENTOR_AXI_SRAM_WORD_OFFSET(unit) (DDC(axi_sram_offset) / 4) /** AXI SRAM offset in words */


#define DNX_EVENTOR_SBUS_DMA_BUFFERS_DEFAULT_THRESH 10 /** SBUS desc & fifo DMA default buffers threshold for polling Done bit */
#define DNX_EVENTOR_EVPCK_DEFAULT_HEADER_STACK_SIZE 12 /** EVPCK header stack size default */
#define DNX_EVENTOR_BUILDER_DEFAULT_TRANSMIT_STACK_SIZE 20 /** builder transmit header stack size default */

#define DNX_EVENTOR_BUILDER_QUEUE_MAX_SIZE_TH_GRANULARITY 4 /** builder queue max size threshold granularity */

/* size of rx buffer for unused rx context
 * The alignment of the buffer also needs to be 4,
 * so that when CMIC reads from the first word, it will read the whole 4 words,
 * and we depend on it for the Eventor's counting of reads from the buffer.
 */
#define UNUSED_RX_CONTEXT_BUFFER_SIZE 4

/* size of descriptor chain for unused rx context to hold one descriptor */
#define UNUSED_RX_CONTEXT_DESCRIPTOR_CHAIN_SIZE 8

/* number of ns in 1 msec */
#define ONE_MS_IN_NS 1000000
/* 48 bit mask for TOD timer */
#define TOD_COUNTER_MASK COMPILER_64_LITERAL(0xffffffffffff)

/**
 * \brief - Transmit header MAX size, in bytes.
 * MAX Header size in bytes: 1024 bits; 1024 / 8 = 128 bytes
 */
#define DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE (1024/8)

/**
 * \brief SW state flags
 */
#define DNX_EVENTOR_STATE_FLAGS_ENABLED 0x1 /** indication whether Eventor is enabled */

/*************
 * FUNCTIONS *
 *************/
int
dnx_instru_eventor_write_dma_descriptors(
        int unit,
        bcm_eventor_context_id_t context,
        const bcm_instru_eventor_context_conf_t *context_conf);
void
dnx_instru_eventor_generate_tx_event_eventor_header(
        int unit,
        uint32 flags,
        bcm_eventor_context_id_t context,
        bcm_instru_eventor_context_conf_t *context_conf,
        uint32 *eventor_header);

/**
 * \brief
 *  Eventor init function.
 *  Used to init the Eventor block at startup
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e dnx_instru_eventor_init(
    int unit)
{
    uint32 core = 0;
    uint32 entry_handle_id;
    bcm_port_t port;
    int builder;
    uint8 port_is_found = FALSE;
    const dnx_data_port_static_add_ucode_port_t *ucode_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Prevent receiving events from connected blocks PMF1/IPPC1 till this is supported in software */
    /** In IPPC_EVENTOR_FIFO_CFGr modify EVENTOR_ALMOST_FULL_THf, EVENTOR_CODEf */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_IPP_FIFO_CFG, &entry_handle_id)); /** create handle */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_ALMOST_FULL_TH, INST_SINGLE, 9); /** set value - almost full threshold */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CODE, INST_SINGLE, 0xfd); /** set value - code */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /** disable events from blocks: write 0xfc00 to EVNT_TX_HANDLER_REQ_ENr */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_TX_HANDLER_REQ_EN, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_HANDLER_REQ_EN, INST_SINGLE, 0xfc00); /** set value - disabled blocks */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */


    /** Find the core used for all the Eventor interface ports. */
    /** If there are Eventor ports on different cores - return an error */
    if (dnx_data_device.general.nof_cores_get(unit) > 1) {
        uint32 max_num_ports = dnx_data_port.general.fabric_port_base_get(unit);
        for (port = 0; port < max_num_ports; port++) {
            ucode_port = dnx_data_port.static_add.ucode_port_get(unit, port);
            if (ucode_port->interface == BCM_PORT_IF_EVENTOR) {
                /** found first Eventor port, save its core and indicate that first port was found */
                if (port_is_found == FALSE) {
                    core = ucode_port->core;
                    port_is_found = TRUE;
                } else {
                    if (core != ucode_port->core) {
                        SHR_ERR_EXIT(_SHR_E_CONFIG, "All Eventor ports must use the same core\n");
                    }
                }
            }
        }
    }

    /** select the core used by for the Eventor interface for both IRE and EPNI, based on the Eventor ports definitions */
    /** write the core to EVNT_PIPE_SELECTORr: IRE_SEL_PIPEf, EPNI_SEL */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_PIPE_SELECTOR, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_PIPE_ID, INST_SINGLE, core); /** set value - core */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /* Define the RX Eventor header (EVPCK) size. (global)
     * Packets that the Eventor receives need to contain this header,
     * which will be generated by ETPP after the packets are replicated and
     * before they reach the Eventor. The destination field is used as the
     * RX channel ID for the received packet.
     */
    /** write to EVNT_RX_EVPCK_HEADER_STACK_SIZEr */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_RX_EVPCK_HEADER_STACK_SIZE, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_TRANSMIT_HEADER_SIZE, INST_SINGLE,
                                 DDC(rx_evpck_header_size)); /** set value - rx packet header size */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */


    /** Disable the sequence check for all RX channels */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_RX_SEQUENCE_CHECK, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEQUENCE_CHECK_ENABLE, INST_SINGLE, 0); /** set value - enable sequence check */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */
    /*
     * Map Context ID to standard/non standard context for FIFO DMA.
     * Use 0=non-standard (contain non-standard events) for FIFO and RX.
     * For non standard events the builder will be equal to the context ID and
     * not according to EVNT_TRANSACTION_ENGINE_BIT_MAPPING.
     * Write 0 to EVNT_TX_ENG_CONTEXT_QUEUE_TYPEr.
     * Disable packet padding (padding is done per builder up to maximum size of 1600B).
     * write 0 to EVNT_TX_ENG_EVPCK_PADDING_ENABLEr */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_TX_ENG_CFG, entry_handle_id));
    for (builder = 0; builder < DNX_EVENTOR_NOF_BUILDERS(unit); builder++) {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_TX_ENGINE_ID, builder); /** set key - builder id */
        /** Disable packet padding (padding is done per builder up to maximum size of 1600B). */
        /** write 0 to EVNT_TX_ENG_EVPCK_PADDING_ENABLEr */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PADDING_ENABLE, INST_SINGLE, FALSE); /** set value - padding */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


/**
 * \brief - check if the given buffers collide
 *
 * \param [in] buf1_start - buffer 1 start
 * \param [in] buf1_end - buffer 1 end
 * \param [in] buf2_start - buffer 1 start
 * \param [in] buf2_end - buffer 1 end
 *
 * \return non zero if there is a collision between the two buffers
 *
 */
static inline int dnx_instru_eventor_do_buffers_collide(
    uint32 buf1_start, /* buffer 1 start */
    uint32 buf1_end,   /* buffer 1 end + 1 */
    uint32 buf2_start, /* buffer 1 start */
    uint32 buf2_end)   /* buffer 1 end + 1 */
{
    return buf1_end > buf2_start && buf2_end > buf1_start;
}

/**
 * \brief -manually generate single event
 */
shr_error_e
bcm_dnx_instru_eventor_event_generate(
        int unit,
        uint32 flags,
        bcm_eventor_event_id_t event_id)
{
    uint32 done_indication = 0;
    uint32 entry_handle_id;
    uint8 done_count;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    

    /** create single event - EVNT_INTERNAL_HOST_EVENT_TRIGGER register */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_EVENT_GENERATE, &entry_handle_id)); /** create handle */
    /** Setting EVNT_INTERNAL_HOST_EVENT_TRIGGERr to zero */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** Generate Event by setting INT_HOST_EVENT_IDf and HOST_EVENT_REQf=1 in EVNT_INTERNAL_HOST_EVENT_TRIGGERr*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENT_ID, INST_SINGLE, event_id); /** set value - event_id */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENT_REQ, INST_SINGLE, 1); /** set value - req */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    done_count = 0;
    while (!done_indication && done_count < 5)
    {
        /** Read event done indication from HOST_EVENT_DONEf in EVNT_INTERNAL_HOST_EVENT_TRIGGERr*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_EVENT_GENERATE, entry_handle_id)); /** clear handle */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_EVENT_DONE, INST_SINGLE,
                                                        &done_indication));
        done_count++;
    }

    if (!done_indication)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Eventor generate Event %d failed\n", event_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/* API to read ns value of TOD counter. */
shr_error_e bcm_dnx_instru_synced_trigger_time_of_day_get(
    int unit,
    uint32 flags,
    uint64 *time_of_day)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** read current time , read ECI_GP_STATUS_1r TODW_48_BIT_NS_TIMERf */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INST_TOD, &entry_handle_id));
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_TIMESTAMP_NS, INST_SINGLE, time_of_day);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


/**
 * enable or disable each of the specified trigger sources
 *
 * \param [in] unit - unit id
 * \param [in] nof_sources - number of sources to work on, the size of the next two arguments
 * \param [in] sources - the sources to enable/disable
 * \param [in] enable - enable or disable each specified source
 */
shr_error_e
dnx_instru_synced_trigger_enable(
        int unit,
        uint32 nof_sources,
        bcm_instru_synced_trigger_source_type_t *sources,
        uint32 *enable)
{
    uint32 entry_handle_id;
    uint8 source_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INST_SYNCED_TRIGGER_CTRL, &entry_handle_id)); /** create handle */

    /* set the correct register register fields according to synchronized trigger type
     * Set ECI_BROAD_VIEW_CTRLr CRPS_INSTRUMENTATION_ENABLEf ,CGM_INSTRUMENTATION_ENABLEf, CDU_INSTRUMENTATION_ENABLEf
     */
    for (source_index=0; source_index<nof_sources; source_index++)
    {
        switch (sources[source_index]) {
            case bcmInstruSyncedTriggerTypeIcgm:
                dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_CGM_ENABLE, INST_SINGLE, enable[source_index]); /** set value - cgm_enable */
                break;
            case bcmInstruSyncedTriggerTypeNif:
                dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_CDU_ENABLE, INST_SINGLE, enable[source_index]); /** set value - cdu_enable */
                break;
            case bcmInstruSyncedTriggerTypeCrps:
                dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ENABLE, INST_SINGLE, enable[source_index]); /** set value - crps_enable */
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Trigger type does not exist %d.\n",sources[source_index]);
                break;
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /*
     * Update source status in SW state
     */
    for (source_index=0; source_index<nof_sources; source_index++)
    {
        SHR_IF_ERR_EXIT(instru.trigger_info.enable.set(unit,sources[source_index],enable[source_index]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * For each of the specified trigger sources,
 * enable or disable it , affecting if it will generate
 * synchronized triggers or not.
 *
 * \param [in] unit - unit id
 * \param [in] flags - relevant flags
 * \param [in] nof_sources - number of sources to work on, the size of the next two arguments
 * \param [in] sources - the sources to enable/disable
 * \param [in] enable - enable or disable each specified source
 */
shr_error_e
bcm_dnx_instru_synced_triggers_enable_set(
        int unit,
        uint32 flags,
        uint32 nof_sources,
        bcm_instru_synced_trigger_source_type_t *sources,
        uint32 *enable)
{
    bcm_instru_synced_trigger_config_t config;
    uint32 entry_handle_id;
    uint8 source_index;
    uint64 start_time, start_time_calc, start_time_wrapped;
    uint64 immediate_time;
    uint64 timestamp;
    uint64 nof_triggers;
    uint32 is_continuous;
    uint32 config_flags;
    int wait_till_wrap_around = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (nof_sources >= bcmInstruSyncedTriggerTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "nof sources %d exceed the number of available sources %d.\n",nof_sources, bcmInstruSyncedTriggerTypeCount);
    }

    /** allocate the handle once here and reuse it*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, &entry_handle_id));

    for (source_index=0; source_index<nof_sources; source_index++)
    {
        if (enable[source_index])
        {
            SHR_IF_ERR_EXIT(instru.trigger_info.trigger_period.get(unit,sources[source_index], &config.trigger_period));
            SHR_IF_ERR_EXIT(instru.trigger_info.nof_triggers.get(unit,sources[source_index], &config.nof_triggers));
            SHR_IF_ERR_EXIT(instru.trigger_info.start_time.get(unit,sources[source_index], &config.start_time));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_TYPE, sources[source_index]);
            /** write to ECI_XXX_INST_CTRLr register  XXX_INST_START_TIMEf field. */
            dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_START_TIME, INST_SINGLE, config.start_time);
            /** write to ECI_XXX_INST_CTRLr register XXX_INST_INTERVAL_PERIODf field. */
            dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_INTERVAL_PERIOD, INST_SINGLE, config.trigger_period);
            /** Number of triggers to generate, 0 means infinite.*/
            nof_triggers = (config.nof_triggers ? config.nof_triggers-1 : 0);
            /** write to ECI_XXX_INST_CTRLr register XXX_INST_NOF_INTERVALSf field. */
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_NOF_INTERVALS, INST_SINGLE, nof_triggers);
            is_continuous = (config.nof_triggers ? 0 : 1);
            /** write to ECI_XXX_INST_CTRLr register XXX_INST_OVERRIDE_MODEf field. */
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_CONTINUOUS, INST_SINGLE, is_continuous);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, entry_handle_id));
        }
    }

    /** read current time */
    SHR_IF_ERR_EXIT(bcm_dnx_instru_synced_trigger_time_of_day_get(unit, 0,&timestamp));
    immediate_time = ((timestamp + ONE_MS_IN_NS) & TOD_COUNTER_MASK);

    /** set the start time for each source */
    for (source_index=0; source_index<nof_sources; source_index++)
    {
        if (enable[source_index])
        {
            SHR_IF_ERR_EXIT(instru.trigger_info.start_time.get(unit,sources[source_index], &start_time));
            SHR_IF_ERR_EXIT(instru.trigger_info.flags.get(unit,sources[source_index], &config_flags));

            /** Set start time */
            if ((config_flags & BCM_INSTRU_SYNCED_TRIGGER_CONFIG_START_TIME_ABSOLUTE) == 0)
            { /* handle relative time specification */
                start_time_calc = start_time + timestamp;
                start_time_wrapped = (start_time_calc & TOD_COUNTER_MASK);
                if (start_time_wrapped < immediate_time) { /* Do we have a wrap around causing the time to configure to be smaller then immediately */
                    start_time_wrapped = immediate_time;
                }
                if (start_time_wrapped < timestamp) { /* should we wait for a wrap around. */
                    wait_till_wrap_around = 1;
                }
                /* write the actual start time to the register */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_TYPE, sources[source_index]);
                dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_START_TIME, INST_SINGLE, start_time_wrapped);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            else /* start time is absolute */
            {
                if (start_time < timestamp)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "absolute time cannot be older then current time.\n");
                }
            }
        }
    }
    if (wait_till_wrap_around)
    {
        /** read TOD while TOD wraps */
        while (1)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_instru_synced_trigger_time_of_day_get(unit, 0,&timestamp));
            if (timestamp <= TOD_COUNTER_MASK/4)
            {
                break;
            }
            /** sleep 1 usec */
            sal_usleep(1);
        };
    }

    /** enable/disable all configured sources */
    SHR_IF_ERR_EXIT(dnx_instru_synced_trigger_enable(unit, nof_sources, sources, enable));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -Get the status of synchronized triggers.
 * This is done by reading the following register:
 * g ECI_BROAD_VIEW_CTRL
 */
shr_error_e
bcm_dnx_instru_synced_triggers_enable_get(
        int unit,
        uint32 flags,
        uint32 nof_trigger_types,
        bcm_instru_synced_trigger_source_type_t *sources,
        uint8 *enable)
{
    uint32 entry_handle_id;
    uint8 source_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (nof_trigger_types >= bcmInstruSyncedTriggerTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "number of trigger types %d exceed the number of available trigger types %d.\n",nof_trigger_types, bcmInstruSyncedTriggerTypeCount);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INST_SYNCED_TRIGGER_CTRL, &entry_handle_id)); /** create handle */

    /** Get all fields */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (source_index=0; source_index<nof_trigger_types; source_index++)
    {
        switch (sources[source_index]) {
            case bcmInstruSyncedTriggerTypeIcgm:
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_CGM_ENABLE, INST_SINGLE,
                                                                   &enable[source_index]));
                break;
            case bcmInstruSyncedTriggerTypeNif:
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_CDU_ENABLE, INST_SINGLE,
                                                                   &enable[source_index]));
                break;
            case bcmInstruSyncedTriggerTypeCrps:
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_CRPS_ENABLE, INST_SINGLE,
                                                                   &enable[source_index]));
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Instrumentation source does not  exist %d.\n",sources[source_index]);
                break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_synced_trigger_config_verify(
    int unit,
    uint32 flags,
    bcm_instru_synced_trigger_source_type_t source_type,
    uint8 is_set,
    bcm_instru_synced_trigger_config_t *config)
{
    int trigger_period_nof_bits_hw;
    int trigger_period_nof_bits_maximum_usable = dnx_data_instru.synced_counters.max_interval_period_size_get(unit);
    int max_trigger_period_nof_bits;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify input flags */
    SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "Unsupported flags for synchronized triggers configuration!\n");

    /** Verify input source type */
    if ((source_type != bcmInstruSyncedTriggerTypeIcgm) && (source_type != bcmInstruSyncedTriggerTypeNif))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported source type %d for synchronized triggers configuration!\n",
                     source_type);
    }

    if (is_set)
    {
        uint32 trigger_source_enable;
        /*
         * Verify the configured trigger source is disabled
         */
        SHR_IF_ERR_EXIT(instru.trigger_info.enable.get(unit,source_type,&trigger_source_enable));
        if (trigger_source_enable)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Trigger source need to be disabled before configuration.\n");
        }
        /*
         * Verify the size of the period does not exceed the maximum number of bits,
         * either the size in HW or the maximum usable size.
         * Also verify that the period isn't zero.
         */
        if (config->trigger_period == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Period cannot be zero.\n");
        }
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                        (unit, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, DBAL_FIELD_INTERVAL_PERIOD, FALSE, 0, 0,
                         &trigger_period_nof_bits_hw));
        max_trigger_period_nof_bits = (trigger_period_nof_bits_hw < trigger_period_nof_bits_maximum_usable) ?
            (trigger_period_nof_bits_hw) : (trigger_period_nof_bits_maximum_usable);
        if ((config->trigger_period >> max_trigger_period_nof_bits) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Period (%u,%u)is out of range, cannot be more than %u bits.\n",
                         COMPILER_64_HI(config->trigger_period), COMPILER_64_LO(config->trigger_period),
                         max_trigger_period_nof_bits);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set the configuration for the given type of synchronized triggers.
 * The configuration will be written to the device only when it is activated by bcm_instru_synced_triggers_enable_set().
 * The configuration may not be set while the triggers are active.
 */
shr_error_e
bcm_dnx_instru_synced_trigger_config_set(
    int unit, 
    uint32 flags, 
    bcm_instru_synced_trigger_source_type_t source_type,
    bcm_instru_synced_trigger_config_t *config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_instru_synced_trigger_config_verify(unit, flags, source_type, TRUE, config));

    SHR_IF_ERR_EXIT(instru.trigger_info.flags.set(unit,source_type,flags));
    SHR_IF_ERR_EXIT(instru.trigger_info.start_time.set(unit,source_type,config->start_time));
    SHR_IF_ERR_EXIT(instru.trigger_info.trigger_period.set(unit,source_type,config->trigger_period));
    SHR_IF_ERR_EXIT(instru.trigger_info.nof_triggers.set(unit,source_type,config->nof_triggers));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  generic event configuration set
 */
shr_error_e
bcm_dnx_instru_synced_trigger_config_get(
    int unit, 
    uint32 flags, 
    bcm_instru_synced_trigger_source_type_t source_type,
    bcm_instru_synced_trigger_config_t *config)
{
    uint32 entry_handle_id;
    uint8 is_continuous;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_instru_synced_trigger_config_verify(unit, flags, source_type, FALSE, config));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_TYPE, source_type);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_START_TIME, INST_SINGLE, &config->start_time);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_INTERVAL_PERIOD, INST_SINGLE, &config->trigger_period);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NOF_INTERVALS, INST_SINGLE, &config->nof_triggers);
    config->nof_triggers++;
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_CONTINUOUS, INST_SINGLE, &is_continuous);
    if (is_continuous)
    {
        config->trigger_period = 0;
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  generic event configuration set
 */
shr_error_e
bcm_dnx_instru_eventor_event_id_config_set(
         int unit,
         uint32 flags,
         bcm_eventor_event_id_t event_id,
         bcm_instru_eventor_event_id_config_t *config)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** map event ID (256) to context ID (16) */
    /** set EVNT_TX_EVENT_ID_TO_CONTEXT_ID_MAPm, CONTEXT_IDf */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_EVENT_ID_TO_TX_CONTEXT_ID_MAP, &entry_handle_id)); /** create handle */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENT_ID,event_id);                    /** set Key - event id */
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CONTEXT_ID, INST_SINGLE, config->context); /** set value - context_id */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function returns the event Id configuration.
 */
shr_error_e
bcm_dnx_instru_eventor_event_id_config_get(
        int unit,
        uint32 flags,
        bcm_eventor_event_id_t event_id,
        bcm_instru_eventor_event_id_config_t *config)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** get EVNT_TX_EVENT_ID_TO_CONTEXT_ID_MAPm, CONTEXT_IDf */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_EVENT_ID_TO_TX_CONTEXT_ID_MAP, &entry_handle_id)); /** create handle */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENT_ID,event_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CONTEXT_ID, INST_SINGLE,
                                                        &config->context));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_instru_eventor_generate_periodic_verify(
    int unit,
    uint32 flags,
    bcm_instru_eventor_periodic_source_t *source,
    bcm_instru_eventor_periodic_config_t *config)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify input flags */
    SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "Unsupported flags for synchronized triggers configuration!\n");

    /** Verify input source type */
    if ((source->source != bcmInstruSyncedTriggerTypeIcgm) && (source->source != bcmInstruSyncedTriggerTypeNif))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported source type %d for Eventor generate periodic configuration!\n",
                source->source);
    }
    if (source->source_index > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported source index %d for Eventor generate periodic configuration!\n",
                source->source_index);
    }

exit:
    SHR_FUNC_EXIT;
}

/* Periodic event configuration set */
int
bcm_dnx_instru_eventor_generate_periodic_set(
    int unit,
    uint32 flags,
    bcm_instru_eventor_periodic_source_t * source,
    bcm_instru_eventor_periodic_config_t * config)
{
    uint32 entry_handle_id;
    uint32 source_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_instru_eventor_generate_periodic_verify(unit,flags, source, config));

    /** set register EVNT_INTERNAL_TOD_EVENT_VALUE */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_EVENT_SOURCE_MAPPING, &entry_handle_id)); /** create handle */

    source_index = ((source->source * 2) + source->source_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_INDEX, source_index); /** set key - source index */
    /** set register field TOD_EVENT_ID_N */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENT_ID, INST_SINGLE, config->event_id); /** set value - event ID */
    /** set register field TOD_SRC_N */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENT_SRC, INST_SINGLE, 0); /** set value - event source to ECI */
    /** set register field TOD_ENABLE_N */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, config->enable); /** set values enable*/
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* Periodic event configuration get */
int
bcm_dnx_instru_eventor_generate_periodic_get(
    int unit,
    uint32 flags,
    bcm_instru_eventor_periodic_source_t * source,
    bcm_instru_eventor_periodic_config_t * config)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_instru_eventor_generate_periodic_verify(unit,flags, source, config));

    /** get EVNT_TX_EVENT_ID_TO_CONTEXT_ID_MAPm, CONTEXT_IDf */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_EVENT_SOURCE_MAPPING, &entry_handle_id)); /** create handle */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_INDEX,source->source_index);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get(unit, entry_handle_id, DBAL_FIELD_EVENT_ID, INST_SINGLE, &config->event_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *)&config->enable));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify procedure for bcm_dnx_instru_eventor_context_set() API
 */
static shr_error_e
dnx_instru_eventor_context_set_verify(
    int unit,
    uint32 flags,
    bcm_eventor_context_id_t context,
    bcm_eventor_context_type_t context_type,
    bcm_instru_eventor_context_conf_t *conf)
{
    uint32 buffer1_end, buffer2_end, buf_i1_start, buf_i1_end, buf_i2_start, buf_i2_end, buf_i1_bank, buf_i2_bank;
    uint8 bank_1_is_used_in_tx, bank_2_is_used_in_tx, bank_1_is_used_in_rx, bank_2_is_used_in_rx;
    uint32 sw_state_flags, buffer_size_i;
    unsigned context_i;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(conf, _SHR_E_PARAM, "conf");
    buffer1_end = conf->buffer1_start + conf->buffer_size;
    buffer2_end = conf->buffer2_start + conf->buffer_size;

    /** verify whether the Eventor is supported for this device */
    if (dnx_data_instru.eventor.feature_get(unit, dnx_data_instru_eventor_is_supported) == FALSE) {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "There is no Eventor support for this device.\n");
    } else if (flags) { /** verify flags */
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags were specified.\n");
    } else if (context >= DNX_EVENTOR_NOF_CONTEXTS(unit)) { /** verify context id */
        SHR_ERR_EXIT(_SHR_E_PARAM, "context ID %u is out of range, should be under %u.\n", context, DNX_EVENTOR_NOF_CONTEXTS(unit));
    } else if (conf->buffer_size > DNX_EVENTOR_RX_MAX_BUFFER_SIZE_WORDS(unit)) { /** verify buffer size */
        SHR_ERR_EXIT(_SHR_E_PARAM, "RX buffer size in 4 byte words %u is bigger than the allowed %u.\n",
        conf->buffer_size, DNX_EVENTOR_RX_MAX_BUFFER_SIZE_WORDS(unit));
    } else if (conf->bank1 >= DNX_EVENTOR_NOF_SRAM_BANKS(unit)) { /** verify bank sizes */
        SHR_ERR_EXIT(_SHR_E_PARAM, "SRAM bank %u is out of range, should be under %u.\n", conf->bank1, DNX_EVENTOR_NOF_SRAM_BANKS(unit));
    } else if (conf->bank2 >= DNX_EVENTOR_NOF_SRAM_BANKS(unit)) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SRAM bank %u is out of range, should be under %u.\n", conf->bank2, DNX_EVENTOR_NOF_SRAM_BANKS(unit));
    } else if (buffer1_end > DNX_EVENTOR_SRAM_BANKS_NET_SIZE_WORDS(unit)) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "1st buffer offset + size in words is out of range, should be <= %u.\n", DNX_EVENTOR_SRAM_BANKS_NET_SIZE_WORDS(unit));
    } else if (buffer2_end > DNX_EVENTOR_SRAM_BANKS_NET_SIZE_WORDS(unit)) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "2nd buffer offset + size in words is out of range, should be <= %u.\n", DNX_EVENTOR_SRAM_BANKS_NET_SIZE_WORDS(unit));
    } else if ((conf->flags & ~(BCM_INSTRU_EVENTOR_CONTEXT_CONF_NO_AGGREGATION | BCM_INSTRU_EVENTOR_CONTEXT_CONF_MAX_AGGREGATION_2)) != 0) { /** verify conf flags */
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flag/s in context configuration flags %u\n", conf->flags);
    }

    /** get sw state flags */
    SHR_IF_ERR_EXIT(instru.eventor_info.flags.get(unit, &sw_state_flags));

    /** verify whether modifications to eventor are allowed at this stage */
    if (sw_state_flags & DNX_EVENTOR_STATE_FLAGS_ENABLED) {
        SHR_ERR_EXIT(_SHR_E_BUSY, "Cannot change configuration after the Eventor is started.\n");
    }

    switch (context_type) {
        case bcmEventorContextTypeRx: /** RX context type */
            /** check that the buffers do not overlap with existing buffers */
            for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++)
            {
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer_size.get(unit, context_i, &buffer_size_i));
                if ((buffer_size_i > 0) && (context != context_i))
                {
                    SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer0_offset.get(unit, context_i, &buf_i1_start));
                    SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer1_offset.get(unit, context_i, &buf_i2_start));

                    buf_i1_bank = buf_i1_start & DNX_EVENTOR_SRAM_BANK_MASK(unit);
                    buf_i2_bank = buf_i2_start & DNX_EVENTOR_SRAM_BANK_MASK(unit);

                    SHR_IF_ERR_EXIT(instru.eventor_info.tx_sram_banks_bitmap.bit_get(unit, buf_i1_bank, &bank_1_is_used_in_tx));
                    SHR_IF_ERR_EXIT(instru.eventor_info.tx_sram_banks_bitmap.bit_get(unit, buf_i2_bank, &bank_2_is_used_in_tx));
                    if (bank_1_is_used_in_tx || bank_2_is_used_in_tx)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "May not use same SRAM bank for RX and TX contexts: bank1: %u used by tx: %u bank2: %u used by tx: %u.\n",
                                buf_i1_bank, bank_1_is_used_in_tx, buf_i1_bank, bank_2_is_used_in_tx);
                    }
                    buf_i1_start >>= DNX_EVENTOR_SRAM_BANK_BITS(unit);
                    buf_i2_start >>= DNX_EVENTOR_SRAM_BANK_BITS(unit);

                    buf_i1_end = buf_i1_start + buffer_size_i;
                    buf_i2_end = buf_i2_start + buffer_size_i;

                    if (conf->bank1 == buf_i1_bank && dnx_instru_eventor_do_buffers_collide(
                        conf->buffer1_start, buffer1_end, buf_i1_start, buf_i1_end))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 1st buffer bank %u word offset %u collides with specified 1st buffer.\n",
                          context_i, buf_i1_bank, buf_i1_start);
                    } else if (conf->bank2 == buf_i1_bank && dnx_instru_eventor_do_buffers_collide(
                               conf->buffer2_start, buffer2_end, buf_i1_start, buf_i1_end))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 1st buffer bank %u word offset %u collides with specified 2nd buffer.\n",
                          context_i, buf_i1_bank, buf_i1_start);
                    } else if (conf->bank1 == buf_i2_bank && dnx_instru_eventor_do_buffers_collide(
                               conf->buffer1_start, buffer1_end, buf_i2_start, buf_i2_end))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 2nd buffer bank %u word offset %u collides with specified 1st buffer.\n",
                          context_i, buf_i2_bank, buf_i2_start);
                    } else if (conf->bank2 == buf_i2_bank && dnx_instru_eventor_do_buffers_collide(
                               conf->buffer2_start, buffer2_end, buf_i2_start, buf_i2_end))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 2nd buffer bank %u word offset %u collides with specified 2nd buffer.\n",
                          context_i, buf_i2_bank, buf_i2_start);
                    }
                }
            }
            break;
        case bcmEventorContextTypeTx: /** TX context type */
            /** check that the buffers do not overlap with existing buffers */
            for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++)
            {
                SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer_size.get(unit, context_i, &buffer_size_i));
                if ((buffer_size_i > 0) && (context != context_i))
                {
                    SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer0_offset.get(unit, context_i, &buf_i1_start));
                    SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer1_offset.get(unit, context_i, &buf_i2_start));

                    buf_i1_bank = buf_i1_start & DNX_EVENTOR_SRAM_BANK_MASK(unit);
                    buf_i2_bank = buf_i2_start & DNX_EVENTOR_SRAM_BANK_MASK(unit);

                    SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_get(unit, buf_i1_bank, &bank_1_is_used_in_rx));
                    SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_get(unit, buf_i2_bank, &bank_2_is_used_in_rx));
                    if (bank_1_is_used_in_rx || bank_2_is_used_in_rx)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "May not use same SRAM bank for RX and TX contexts: bank1: %u used by rx: %u bank2: %u used by rx: %u.\n",
                                buf_i1_bank, bank_1_is_used_in_rx, buf_i1_bank, bank_2_is_used_in_rx);
                        SHR_ERR_EXIT(_SHR_E_PARAM, "SRAM bank is not TX: bank1 %u is_rx_bank %u bank2 %u is_rx_bank %u.\n",
                                buf_i1_bank, bank_1_is_used_in_rx, buf_i1_bank, bank_2_is_used_in_rx);
                    }

                    buf_i1_start >>= DNX_EVENTOR_SRAM_BANK_BITS(unit);
                    buf_i2_start >>= DNX_EVENTOR_SRAM_BANK_BITS(unit);

                    buf_i1_end = buf_i1_start + buffer_size_i;
                    buf_i2_end = buf_i2_start + buffer_size_i;

                    if (conf->bank1 == buf_i1_bank && dnx_instru_eventor_do_buffers_collide(
                        conf->buffer1_start, buffer1_end, buf_i1_start, buf_i1_end))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 1st buffer bank %u word offset %u collides with specified 1st buffer.\n",
                          context_i, buf_i1_bank, buf_i1_start);
                    } else if (conf->bank2 == buf_i1_bank && dnx_instru_eventor_do_buffers_collide(
                               conf->buffer2_start, buffer2_end, buf_i1_start, buf_i1_end))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 1st buffer bank %u word offset %u collides with specified 2nd buffer.\n",
                          context_i, buf_i1_bank, buf_i1_start);
                    } else if (conf->bank1 == buf_i2_bank && dnx_instru_eventor_do_buffers_collide(
                               conf->buffer1_start, buffer1_end, buf_i2_start, buf_i2_end))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 2nd buffer bank %u word offset %u collides with specified 1st buffer.\n",
                          context_i, buf_i2_bank, buf_i2_start);
                    } else if (conf->bank2 == buf_i2_bank && dnx_instru_eventor_do_buffers_collide(
                               conf->buffer2_start, buffer2_end, buf_i2_start, buf_i2_end))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 2nd buffer bank %u word offset %u collides with specified 2nd buffer.\n",
                          context_i, buf_i2_bank, buf_i2_start);
                    }
                }
            }
            break;

        default:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "unsupported context type flag %d%s%s.\n", context_type, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM API: Set the configuration of the given context ID (its type, sram buffers offsets, etc...)
 *
 * \param [in] unit - unit id
 * \param [in] flags - flags
 * \param [in] context - context id
 * \param [in] context_type - type of context (rx, tx...)
 * \param [in] conf - context attributes to set, see description of structure.
 * \param [in] mark_context_as_rx - If none zero, will make the RX context work (in rx to tx mode), and mark it in
 *                                  DnxData as an RX context.
 *                                  If zero, will not make RX events get copied to builders, use a DMA descriptor
 *                                  chain that will be configured and not touch the
 *                                  DnxData marking of the context as used for RX.
 *
 * \remark
 *   * This API only updates SW in order to assure proper sequence of configurations, commit to HW is done only once the
 *     module is init. \see bcm_dnx_instru_eventor_active_set() API.
 */
static int dnx_instru_eventor_context_set_internal(
    int unit,
    uint32 flags,
    bcm_eventor_context_id_t context,
    bcm_eventor_context_type_t context_type,
    bcm_instru_eventor_context_conf_t *conf,
    int mark_context_as_rx)
{
    uint32 eventor_buffer_address[DNX_EVENTOR_NOF_BUFFERS_IN_DOUBLE_BUFFER];
    uint32 eventor_header = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNXC(dnx_instru_eventor_context_set_verify(unit, flags, context, context_type, conf));

    /** save context info to sw state */
    switch (context_type) {
        case bcmEventorContextTypeRx: /** RX context type */
            /** update double buffer offsets */
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer0_offset.set(unit, context, conf->bank1 |
                                                                     (conf->buffer1_start << DNX_EVENTOR_SRAM_BANK_BITS(unit))));
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer1_offset.set(unit, context, conf->bank2 |
                                                                     (conf->buffer2_start << DNX_EVENTOR_SRAM_BANK_BITS(unit))));
            /** mark the SRAM banks of the context's buffers as used by RX in sw state */
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_set(unit, conf->bank1));
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_set(unit, conf->bank2));
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer_size.set(unit, context, conf->buffer_size)); /** set rx buffer size */
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.program0_offset.set(unit, context, conf->program1_start));
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.program1_offset.set(unit, context, conf->program2_start));
            if (mark_context_as_rx)
            {
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_set(unit, context)); /** mark as RX context */
            }
            else
            {
                /** Configure DMA descriptors */
                SHR_IF_ERR_EXIT(dnx_instru_eventor_write_dma_descriptors(unit, context, conf));
            }
            break;

        case bcmEventorContextTypeTx:
            /** update double buffer offsets */
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer_size.set(unit, context, conf->buffer_size));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer0_offset.set(unit, context, conf->bank1 |
                                                                     (conf->buffer1_start << DNX_EVENTOR_SRAM_BANK_BITS(unit))));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer1_offset.set(unit, context, conf->bank2 |

                    (conf->buffer2_start << DNX_EVENTOR_SRAM_BANK_BITS(unit))));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_sram_banks_bitmap.bit_set(unit, conf->bank1));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_sram_banks_bitmap.bit_set(unit, conf->bank2));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.program0_offset.set(unit, context, conf->program1_start));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.program1_offset.set(unit, context, conf->program2_start));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.nof_operations.set(unit, context, conf->nof_operations));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.builders.set(unit, context, conf->builders));

            

            eventor_buffer_address[0] = DNX_EVENTOR_AXI_SRAM_OFFSET(unit) + DNX_EVENTOR_SRAM_BANKS_FULL_SIZE(unit) * conf->bank1 + conf->buffer1_start * 4;
            eventor_buffer_address[1] = DNX_EVENTOR_AXI_SRAM_OFFSET(unit) + DNX_EVENTOR_SRAM_BANKS_FULL_SIZE(unit) * conf->bank2 + conf->buffer2_start * 4;
            /** Configure DMA descriptors */
            SHR_IF_ERR_EXIT(dnx_instru_eventor_write_dma_descriptors(unit, context, conf));
            /** write eventor header to sram first buffer entry  */
            dnx_instru_eventor_generate_tx_event_eventor_header(unit, conf->flags, context, conf, &eventor_header);
            soc_pci_write(unit, EVENTOR_SRAM_DIRECT_MEM_ACCESS(unit,eventor_buffer_address[0] & 0xfffff), eventor_header);
            if ((conf->flags & BCM_INSTRU_EVENTOR_CONTEXT_CONF_MAX_AGGREGATION_2) != 0)
            {
                dnx_instru_eventor_generate_tx_event_eventor_header(unit, BCM_INSTRU_EVENTOR_CONTEXT_CONF_NO_AGGREGATION, context, conf, &eventor_header);
            }
            soc_pci_write(unit, EVENTOR_SRAM_DIRECT_MEM_ACCESS(unit,eventor_buffer_address[1] & 0xfffff), eventor_header);

            /** mark as TX context */
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_bitmap.bit_set(unit, context));

            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "unsupported context type flag %d%s%s.\n", context_type, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM API: Set the configuration of the given context ID (its type, sram buffers offsets, etc...)
 *
 * \param [in] unit - unit id
 * \param [in] flags - flags
 * \param [in] context - context id
 * \param [in] context_type - type of context (rx, tx...)
 * \param [in] conf - context attributes to set, see description of structure.
 *
 * \remark
 *   * This API only updates SW in order to assure proper sequence of configurations, commit to HW is done only once the
 *     module is init. \see bcm_dnx_instru_eventor_active_set() API.
 */
int bcm_dnx_instru_eventor_context_set(
    int unit,
    uint32 flags,
    bcm_eventor_context_id_t context,
    bcm_eventor_context_type_t context_type,
    bcm_instru_eventor_context_conf_t *conf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_instru_eventor_context_set_internal(unit, flags, context, context_type,conf, TRUE));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - verify procedure for bcm_dnx_instru_eventor_context_get() API
 */
static shr_error_e
dnx_instru_eventor_context_get_verify(
    int unit,
    uint32 flags,
    bcm_eventor_context_id_t context,
    bcm_eventor_context_type_t context_type,
    bcm_instru_eventor_context_conf_t *out_conf)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(out_conf, _SHR_E_PARAM, "out_conf");

    /** verify whether eventor is supported */
    if (dnx_data_instru.eventor.feature_get(unit, dnx_data_instru_eventor_is_supported) == FALSE) {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "There is no Eventor support for this device.\n");
    } else if (flags) { /** verify flags */
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags were specified.\n");
    } else if (context >= DNX_EVENTOR_NOF_CONTEXTS(unit)) { /** verify context id */
        SHR_ERR_EXIT(_SHR_E_PARAM, "context ID %u is out of range, should be under %u.\n", context, DNX_EVENTOR_NOF_CONTEXTS(unit));
    }

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 * BCM API: Get the configuration of the given context ID (its type, sram buffers offsets, etc...)
 *
 * \param [in] unit - unit id
 * \param [in] flags - flags
 * \param [in] context - context id
 * \param [in] context_type - type of context (rx, tx...)
 * \param [out] out_conf - context attributes to get, see description of structure.
 *
 * \return non zero if there is a collision between the two buffers
 *
 * \remark
 *   * None
 */
int bcm_dnx_instru_eventor_context_get(
    int unit,
    uint32 flags,
    bcm_eventor_context_id_t context,
    bcm_eventor_context_type_t context_type,
    bcm_instru_eventor_context_conf_t *out_conf)
{
    uint8 is_rx_context, is_tx_context;
    uint32 sram_words_offset;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNXC(dnx_instru_eventor_context_get_verify(unit, flags, context, context_type, out_conf));

    SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_get(unit, context, &is_rx_context));
    SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_bitmap.bit_get(unit, context, &is_tx_context));

    /** get info from sw state */
    if (is_rx_context && (context_type == bcmEventorContextTypeRx || context_type == bcmEventorContextTypeAny))
    {
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer0_offset.get(unit, context, &sram_words_offset));
            out_conf->bank1 = sram_words_offset & DNX_EVENTOR_SRAM_BANK_MASK(unit);
            out_conf->buffer1_start = sram_words_offset >> DNX_EVENTOR_SRAM_BANK_BITS(unit);

            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer1_offset.get(unit, context, &sram_words_offset));
            out_conf->bank2 = sram_words_offset & DNX_EVENTOR_SRAM_BANK_MASK(unit);
            out_conf->buffer2_start = sram_words_offset >> DNX_EVENTOR_SRAM_BANK_BITS(unit);

            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer_size.get(unit, context, &out_conf->buffer_size));
    }
    else if (is_tx_context && (context_type == bcmEventorContextTypeTx || context_type == bcmEventorContextTypeAny))
    {
        SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer0_offset.get(unit, context, &sram_words_offset));
        out_conf->bank1 = sram_words_offset & DNX_EVENTOR_SRAM_BANK_MASK(unit);
        out_conf->buffer1_start = sram_words_offset >> DNX_EVENTOR_SRAM_BANK_BITS(unit);

        SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer1_offset.get(unit, context, &sram_words_offset));
        out_conf->bank2 = sram_words_offset & DNX_EVENTOR_SRAM_BANK_MASK(unit);
        out_conf->buffer2_start = sram_words_offset >> DNX_EVENTOR_SRAM_BANK_BITS(unit);

        SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer_size.get(unit, context, &out_conf->buffer_size));
        SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.program0_offset.get(unit, context, &out_conf->program1_start));
        SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.program1_offset.get(unit, context, &out_conf->program2_start));
        SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.nof_operations.get(unit, context, &out_conf->nof_operations));
        SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.nof_const_values.get(unit, context, &out_conf->nof_const_values));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Context %u of the requested type was not found.\n", context);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for bcm_dnx_instru_eventor_builder_set() API
 */
static shr_error_e
dnx_instru_eventor_builder_set_verify(
    int unit,
    uint32 flags,
    bcm_eventor_builder_id_t builder,
    bcm_instru_eventor_builder_conf_t *conf)
{
    uint32 sw_state_flags, buffer_size, tx_max_header_size;
    uint8 is_rx_context, is_tx_context;
    uint8 context_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(conf, _SHR_E_PARAM, "conf");

    /** verify if eventor is supported */
    if (dnx_data_instru.eventor.feature_get(unit, dnx_data_instru_eventor_is_supported) == FALSE) {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "There is no Eventor support for this device.\n");
    } else if (builder >= DNX_EVENTOR_NOF_BUILDERS(unit)) { /** verify builder id */
        SHR_ERR_EXIT(_SHR_E_PARAM, "builder ID %u is out of range, should be under %u.\n", builder, DNX_EVENTOR_NOF_BUILDERS(unit));
    } else if (conf->header_length & 0x3) { /** verify header_length is 32 bits multiplication */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! header_length = %u, must be multiple of 4 bytes!\n", conf->header_length);
    }

    /* Verify supported flags */
    if ((flags & ~(BCM_INSTRU_EVENTOR_BUILDER_RECONFIG_WITH_NO_TRAFFIC)) != 0) {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags were specified.\n");
    }

    /** verify header_length is in range */
    tx_max_header_size = DDC(tx_builder_max_header_size);
    if (conf->header_length > tx_max_header_size) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! header_length = %d, can't be larger then %d!\n", conf->header_length, tx_max_header_size);
    } else if (conf->header_length != 0) { /** verify header_data */
        /** verify that the pointer to the buffer is NOT NULL*/
        SHR_NULL_CHECK(conf->header_data, _SHR_E_PARAM, "conf->header_data");
    }

    SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_get(unit, builder, &is_rx_context));
    SHR_IF_ERR_EXIT(instru.eventor_info.flags.get(unit, &sw_state_flags));

    /** check if eventor can be modified at this stage , we allow reconfiguration for builder config only for rx contexts when reconfig flag is present */
    if ((sw_state_flags & DNX_EVENTOR_STATE_FLAGS_ENABLED) && !(is_rx_context && (flags & BCM_INSTRU_EVENTOR_BUILDER_RECONFIG_WITH_NO_TRAFFIC) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_BUSY, "May not change configuration after the Eventor is started. sw_state_flags = 0x%08X, is_rx_context = %d, flags = 0x%08X\n",
            sw_state_flags, is_rx_context, flags);
    }

    if (is_rx_context)
    {
        context_found = TRUE;
        /* there is a hidden assumption that context id = builder id need to verify it or support builder to context mapping */
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer_size.get(unit, builder, &buffer_size));
        if (conf->thresh_size <= 0 || (conf->thresh_size % buffer_size))  /** verify threshold size limits */
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "thresh_size given %u is not a positive multiple of the buffer size %u.\n",
              conf->thresh_size, buffer_size);
        } else if (conf->thresh_size > DNX_EVENTOR_BUILDER_MAX_BUFFER_SIZE_WORDS(unit)) {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Builder size threshold in words %u is greater that the max %u.\n",
              conf->thresh_size, DNX_EVENTOR_BUILDER_MAX_BUFFER_SIZE_WORDS(unit));
        }
    }
    else
    {
        uint32 context_i ,builders = 0;

        for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_bitmap.bit_get(unit, context_i, &is_tx_context));
            if (is_tx_context)
            {
                SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.builders.get(unit, context_i, &builders));
                if (builders & (0x1 << builder))
                {
                    context_found = TRUE;
                    SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer_size.get(unit, context_i, &buffer_size));
                    if (conf->thresh_size <= 0) /** verify threshold size limits */
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "thresh_size given %u is not a positive number %u.\n",
                          conf->thresh_size, buffer_size);
                    } else if (conf->thresh_size > DNX_EVENTOR_BUILDER_MAX_BUFFER_SIZE_WORDS(unit)) {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Builder threshold size in words %u is greater that the max %u.\n",
                          conf->thresh_size, DNX_EVENTOR_BUILDER_MAX_BUFFER_SIZE_WORDS(unit));
                    }
                }
            }
        }
    }

    if (!context_found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Builder ID %u was not configured to any context.\n", builder);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* The input buffer is raw data, need to swap LSB <-> MSB.
* see SDK_170437 for description.
*/
static shr_error_e  SDK_170437_Buffer_flip(
    int unit,
    uint8 *buffer,
    uint32 length)
{
    int i;
    uint8 *startP, *endP;
    uint8 tmp;

    SHR_FUNC_INIT_VARS(unit);

    startP = buffer;
    endP = &buffer[length -1];

    for (i=0; i< (length/2); i++)
    {
        tmp = *startP;
        *startP = *endP;
        *endP = tmp;

        startP++;
        endP--;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Set the configuration of TX header of the given builder.
 */
static shr_error_e dnx_instru_eventor_builder_tx_header_set(
    int unit,
    bcm_eventor_builder_id_t builder,
    bcm_instru_eventor_builder_conf_t *conf)
{
    uint8 header_data_high[(DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2)];
    uint8 header_data_low[(DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2)];
    uint32 nof_samples;
    uint32 buffer_size = 0;
    uint8 is_rx_context;
    uint32 entry_handle_id;


    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_BUILDER_TRANSMIT_HEADER_STACK, &entry_handle_id));
    /** set key - builder id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_BUILDER_ID, builder);


    /*
    * Configure the TX header:
    *   1 - header size (must be multiple of 4 bytes)
    */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRANSMIT_HEADER_STACK_SIZE, INST_SINGLE,
                                 conf->header_length);

    /*
    * Configure the TX header:
    *   2 - header data:
    *           The data can be up to 1024 bits (128 bytes).
    *           The HW has two registers, each is 512 (64 bytes) bits to hold this header.
    */
    if (conf->header_length == 0)
    {
        sal_memset(header_data_high, 0, (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2));
        sal_memset(header_data_low, 0, (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2));
    }
    else
    {
        uint8 *destPtr, *srcPtr;
        uint32 length;

        if (conf->header_length < (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2))
        {
            /*
            * Set the MSBs of header_data_high:
            */
            destPtr = header_data_high;
            srcPtr = conf->header_data;
            length = conf->header_length;
            sal_memcpy(destPtr, srcPtr,length);

            /*
            * Zero the LSBs of header_data_high
            */
            destPtr += length;
            length = (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2) - length;
            sal_memset(destPtr, 0, length);

            /*
            * Zero header_data_low
            */
            sal_memset(header_data_low, 0, (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2));

            
            SHR_IF_ERR_EXIT(SDK_170437_Buffer_flip(unit, header_data_high, (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2)));

        }
        else
        {
            /*
            * Copy the first 512 bits to header_data_high:
            */
            destPtr = header_data_high;
            srcPtr = conf->header_data;
            sal_memcpy(destPtr, srcPtr, (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2));


            
            SHR_IF_ERR_EXIT(SDK_170437_Buffer_flip(unit, header_data_high, (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2)));


            /* Set header_data_low:
            *   512 bits were configured, configure the remaining bits.
            *   Set the MSBs header_data_low
            */
            destPtr = header_data_low;
            srcPtr += (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2);
            length = conf->header_length - (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2);
            sal_memcpy(destPtr, srcPtr, length);

            /*
            * Zero the LSBs of header_data_low
            */
            destPtr += length;
            length = (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2) - length;
            sal_memset(destPtr, 0, length);


            
            SHR_IF_ERR_EXIT(SDK_170437_Buffer_flip(unit, header_data_low, (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2)));
        }

    }

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_TRANSMIT_HEADER_STACK_DATA_HIGH, INST_SINGLE, header_data_high);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_TRANSMIT_HEADER_STACK_DATA_LOW, INST_SINGLE, header_data_low);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));


    /*
    * Configure the EVPCK header:
    *   1. Configure the EVPCK header size to be 3 words
    *   2. Configure header contains sequence number, time-stamp and nof-samples
    *   EVNT_EVPCK_HEADER_STACK(0..15).EVNT EVPCK_HEADER_STACK_EN=0
    *   EVNT_EVPCK_HEADER_STACK(0..15).EVPCK_HEADER_STACK_DESTINATION= NOF samples
    *   Note:
    *   NOF samples is calculated using thresh_size / buffer_size (both values are in 4 bytes/32 bits size)
    */
    SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_get(unit, builder, &is_rx_context));
    if (is_rx_context)
    {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer_size.get(unit, builder, &buffer_size));
    }
    else
    {
       uint32 context_i, builders = 0;
        uint8 is_tx_context;

        for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_bitmap.bit_get(unit, context_i, &is_tx_context));
            if (is_tx_context)
            {
                SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.builders.get(unit, context_i, &builders));
                if (builders & (0x1 << builder))
                {
                    SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer_size.get(unit, context_i, &buffer_size));
                    break;
                }
            }
        }
        if (context_i == DNX_EVENTOR_NOF_CONTEXTS(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "context with builder %d not found.\n",builder);
        }
    }

    if (buffer_size != 0)
    {
        nof_samples = conf->thresh_size / buffer_size;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "valid buffer size not found for builder %d.\n",builder);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_TX_CONTEXT_EVPCK_HEADER_STACK, entry_handle_id));
    /** set key - context id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CONTEXT_ID, builder);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVPCK_HEADER_STACK_SIZE, INST_SINGLE, DNX_EVENTOR_EVPCK_DEFAULT_HEADER_STACK_SIZE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVPCK_HEADER_STACK_EN, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVPCK_HEADER_STACK_NOF_SAMPLES, INST_SINGLE, nof_samples);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM API: Set the configuration of the given builder ID (destination, headers, transmission thresholds).
 *
 * \param [in] unit - unit id
 * \param [in] flags - flags
 * \param [in] builder - builder id
 * \param [in] conf - builder attributes to set, see description of structure.
 *
 * \return non zero if there is a collision between the two buffers
 *
 * \remark
 *   * None
 */
int bcm_dnx_instru_eventor_builder_set(
    int unit,
    uint32 flags,
    bcm_eventor_builder_id_t builder,
    bcm_instru_eventor_builder_conf_t *conf)
{
    uint32 u32;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_instru_eventor_builder_set_verify(unit, flags, builder, conf));

    /** This builder serves - set tx builder timeout and size thresholds for transmission  */
    switch (conf->thresh_time) {
      case BCM_INSTRU_EVENTOR_TIMEOUT_NONE:
        u32 = 0;
        break;
      case BCM_INSTRU_EVENTOR_TIMEOUT_MINIMAL:
        u32 = 1;
        break;
      default:
        if ( conf->thresh_time > DNX_EVENTOR_MAX_BUILDER_TIMEOUT_MS(unit)) {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Value bigger than max timeout threshold %ums.\n",
              DNX_EVENTOR_MAX_BUILDER_TIMEOUT_MS(unit));
        }
        u32 = conf->thresh_time * dnx_data_device.general.core_clock_khz_get(unit);
    }

    /** write u32 to EVNT_TX_ENG_CONTEXT_QUEUE_TIMEOUT_VALUEr[builder] */
    /** write conf->thresh_size*4 to EVNT_TX_ENG_CONTEXT_QUEUE_MAX_SIZEr[builder] */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_TX_ENG_CFG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_TX_ENGINE_ID, builder); /** set key - builder id */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_QUEUE_TIMEOUT, INST_SINGLE, u32); /** set value - timeout */
    /** set value - threshold */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_QUEUE_MAX_SIZE, INST_SINGLE,
                                 conf->thresh_size * DNX_EVENTOR_BUILDER_QUEUE_MAX_SIZE_TH_GRANULARITY);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /*
    * Configure the TX header:
    */
    SHR_IF_ERR_EXIT(dnx_instru_eventor_builder_tx_header_set(unit, builder, conf));

    /** mark builder as configured */
    SHR_IF_ERR_EXIT(instru.eventor_info.builder_configured_bitmap.bit_set(unit, builder));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for bcm_dnx_instru_eventor_builder_get() API
 */
static shr_error_e
dnx_instru_eventor_builder_get_verify(
    int unit,
    uint32 flags,
    bcm_eventor_builder_id_t builder,
    bcm_instru_eventor_builder_conf_t *out_conf)
{
    uint8 is_builder_configured;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(out_conf, _SHR_E_PARAM, "out_conf");

    /** check if The Eventor is supported */
    if (dnx_data_instru.eventor.feature_get(unit, dnx_data_instru_eventor_is_supported) == FALSE) {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "There is no Eventor support for this device.\n");
    } else if (flags) { /** check flags */
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags were specified.\n");
    } else if (builder >= DNX_EVENTOR_NOF_BUILDERS(unit)) { /** verify builder id */
        SHR_ERR_EXIT(_SHR_E_PARAM, "builder ID %u is out of range, should be under %u.\n", builder, DNX_EVENTOR_NOF_BUILDERS(unit));
    }

    SHR_IF_ERR_EXIT(instru.eventor_info.builder_configured_bitmap.bit_get(unit, builder, &is_builder_configured));
    if (is_builder_configured == FALSE) { /** Currently only support RX */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Builder ID %u was not configured as RX.\n", builder);
    }

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief - Get the configuration of TX header of the given builder.
 */
static shr_error_e dnx_instru_eventor_builder_tx_header_get(
    int unit,
    bcm_eventor_builder_id_t builder,
    bcm_instru_eventor_builder_conf_t *conf)
{
    uint8 header_data_high[(DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2)];
    uint8 header_data_low[(DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2)];
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_BUILDER_TRANSMIT_HEADER_STACK, &entry_handle_id));
    /** set key - builder id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_BUILDER_ID, builder);

    /** Get all fields */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));


    /*
    * Get Configure the TX header:
    */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TRANSMIT_HEADER_STACK_SIZE, INST_SINGLE,
                                                        &conf->header_length));

    if (conf->header_length > 0)
    {
        uint8 *destPtr;
        int length;

        /** verify that the pointer to the buffer is NOT NULL*/
        SHR_NULL_CHECK(conf->header_data, _SHR_E_PARAM, "conf->header_data");


        /** Get the TX header MSBs data */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_TRANSMIT_HEADER_STACK_DATA_HIGH, INST_SINGLE, header_data_high));


        
        SHR_IF_ERR_EXIT(SDK_170437_Buffer_flip(unit, header_data_high, (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2)));

        /** Copy MSBs */
        length = (conf->header_length > (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2))? (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2):conf->header_length;
        destPtr = &conf->header_data[0];
        sal_memcpy(destPtr, header_data_high, length);

        length = conf->header_length - (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2);

        if (length > 0)
        {
            /** Get the TX header LSBs data */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_TRANSMIT_HEADER_STACK_DATA_LOW, INST_SINGLE, header_data_low));

            
            SHR_IF_ERR_EXIT(SDK_170437_Buffer_flip(unit, header_data_low, (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2)));

            /** Copy LSBs */
            destPtr += (DNX_EVENTOR_TRANSMIT_HEADER_MAX_SIZE/2);
            sal_memcpy(destPtr, header_data_low, length);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


/**
 * \brief
 * BCM API: Get the configuration of the given builder ID (destination, headers, transimission thresholds).
 *
 * \param [in] unit - unit id
 * \param [in] flags - flags
 * \param [in] builder - builder id
 * \param [out] out_conf - builder attributes to get, see description of structure.
 *
 * \remark
 *   * None
 */
int bcm_dnx_instru_eventor_builder_get(
    int unit,
    uint32 flags,
    bcm_eventor_builder_id_t builder,
    bcm_instru_eventor_builder_conf_t *out_conf)
{
    uint32 entry_handle_id, thresh_size, thresh_timeout;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNXC(dnx_instru_eventor_builder_get_verify(unit, flags, builder, out_conf));

    out_conf->flags = 0;

    /** get tx builder timeout and size thresholds for transmission */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_TX_ENG_CFG, &entry_handle_id));
    /** set key - builder id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_TX_ENGINE_ID, builder);

    /** read thresh_timeout from EVNT_TX_ENG_CONTEXT_QUEUE_MAX_SIZEr[builder] */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONTEXT_QUEUE_TIMEOUT, INST_SINGLE, &thresh_timeout);

    /** read thresh_size frin EVNT_TX_ENG_CONTEXT_QUEUE_MAX_SIZEr[builder] */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONTEXT_QUEUE_MAX_SIZE, INST_SINGLE, &thresh_size);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT)); /** Get the entry */

    out_conf->thresh_size = thresh_size / DNX_EVENTOR_BUILDER_QUEUE_MAX_SIZE_TH_GRANULARITY;
    out_conf->thresh_time = thresh_timeout ? (thresh_timeout == 1 ? BCM_INSTRU_EVENTOR_TIMEOUT_MINIMAL :
      thresh_timeout / dnx_data_device.general.core_clock_khz_get(unit)) :
      BCM_INSTRU_EVENTOR_TIMEOUT_NONE;

    /*
    * Get the TX header configure:
    */
    SHR_IF_ERR_EXIT(dnx_instru_eventor_builder_tx_header_get(unit, builder, out_conf));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for bcm_dnx_instru_eventor_active_set() API
 */
static shr_error_e
dnx_instru_eventor_active_set_verify(
    int unit,
    uint32 flags,
    int active)
{
    int error = 0;
    uint32 thresh_size, context_i, buffer_size_i, buf0_offset, buf1_offset, entry_handle_id;
    uint8 is_builder_configured;
    uint8 is_rx_context;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_instru.eventor.feature_get(unit, dnx_data_instru_eventor_is_supported) == FALSE) { /** check if eventor is supported */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "There is no Eventor support for this device.\n");
    } else if (active == 0) { /** stopping The Eventor is not allowed */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Stopping the Eventor is not supported.\n");
    }

    /** check that each rx context has a configured builder */
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_get(unit, context_i, &is_rx_context));
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer_size.get(unit, context_i, &buffer_size_i));
        if (is_rx_context && buffer_size_i) {
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer0_offset.get(unit, context_i, &buf0_offset));
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer1_offset.get(unit, context_i, &buf1_offset));

            SHR_IF_ERR_EXIT(instru.eventor_info.builder_configured_bitmap.bit_get(unit, context_i, &is_builder_configured));

            if (is_builder_configured == 0) {
                LOG_ERROR(BSL_LOG_MODULE, ("Context %u configured as RX, and the same builder was not configured.\n", context_i));
                error = 1;
            } else {
                /* Verify that the builder max size threshold is a positive multiple of the buffer size */
                /** read entry_handle_id from EVNT_TX_ENG_CONTEXT_QUEUE_MAX_SIZEr[context_i] */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_TX_ENG_CFG, &entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_TX_ENGINE_ID, context_i); /** set key - builder id */
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONTEXT_QUEUE_MAX_SIZE, INST_SINGLE, &thresh_size); /** request - size */
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT)); /** Get the entry */

                if (thresh_size == 0 || (thresh_size % buffer_size_i)) {
                    LOG_ERROR(BSL_LOG_MODULE, ("RX context %u has a builder thresh_size %u which is not a positive multiple of the rx buffer size %u.\n",
                      context_i, thresh_size, buffer_size_i));
                    error = 1;
                }
            }
        }
    }
    

    if (error) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Wrong Eventor configuration detailed above.\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Used to init an Eventor single context after the initial configuration is set
 * \param [in] unit - The unit number.
 * \param [in] context - the configured context id.
 * \param [in] buffer_size - context buffer size
 * \param [in] buff0_start_address - offset in 4 byte words in the SRAM bank of the first buffer.
 * \param [in] buff1_start_address - offset in 4 byte words in the SRAM bank of the second buffer.
 * \param [in] program0_offset - offset in 4 byte words in the SRAM bank of the first buffer's DMA program filling the buffer.
 * \param [in] program1_offset - offset in 4 byte words in the SRAM bank of the second buffer's DMA program filling the buffer.
 * \param [in] is_tx_context - 0 - context is rx, 1 - context is tx
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static shr_error_e
dnx_instru_eventor_active_context_set(
        int unit,
        uint32 context,
        uint32 buffer_size,
        uint32 buff0_start_address,
        uint32 buff1_start_address,
        uint32 program0_offset,
        uint32 program1_offset,
        uint8 is_tx_context)
{
    uint32 entry_handle_id;
    uint32 bank1,bank2;
    uint32 cmic_buff0_start, cmic_buff1_start;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** calculate buffer 0 and buffer 1 start and end address */
    bank1 = buff0_start_address & DNX_EVENTOR_SRAM_BANK_MASK(unit);
    bank2 = buff1_start_address & DNX_EVENTOR_SRAM_BANK_MASK(unit);

    buff0_start_address = bank1 * DNX_EVENTOR_SRAM_BANKS_FULL_SIZE_WORDS(unit) +
      (buff0_start_address >> DNX_EVENTOR_SRAM_BANK_BITS(unit));

    /** calculate buffer 1 start and end address */
    buff1_start_address = bank2 * DNX_EVENTOR_SRAM_BANKS_FULL_SIZE_WORDS(unit) +
      (buff1_start_address >> DNX_EVENTOR_SRAM_BANK_BITS(unit));

    /** set double buffer start and end address for the given context */
    /** EVNT_SPACE_REGIONSm[context_i*2+{0,1}] BUFFER_END_ADDRf=(BUFFER_START_ADDRf=buff{0,1}_start_address) + rx_buffer_size_i */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_SPACE_REGIONS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TX_CONTEXT, is_tx_context); /** set key - is tx context */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CONTEXT_ID, context); /** set key - context id */
    /** set values: buffers 0,1 start and end addresses */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFFER_START_ADDR, 0, buff0_start_address);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFFER_END_ADDR, 0, buff0_start_address + buffer_size-1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFFER_START_ADDR, 1, buff1_start_address);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFFER_END_ADDR, 1, buff1_start_address + buffer_size-1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    cmic_buff0_start = DNX_EVENTOR_AXI_SRAM_WORD_OFFSET(unit) +
            (DNX_EVENTOR_SRAM_BANKS_FULL_SIZE_WORDS(unit) * bank1) + program0_offset;
    cmic_buff1_start = DNX_EVENTOR_AXI_SRAM_WORD_OFFSET(unit) +
            (DNX_EVENTOR_SRAM_BANKS_FULL_SIZE_WORDS(unit) * bank2) + program1_offset;

    /** set context descriptor chains start AXI addresses to the RX buffers */
    /** EVNT_CONTEXT_PROPERTIESm[context_i] {,DOUBLE_}BD_PTRf=AXI_offset+buff{0,1}_start_address */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_CONTEXT_PROPERITES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TX_CONTEXT, is_tx_context); /** set key - is tx context */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CONTEXT_ID, context); /** set key - context id */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_BUFFER_START_ADDR, 0, cmic_buff0_start);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_BUFFER_START_ADDR, 1, cmic_buff1_start);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Configure Eventor unused RX contexts to do nothing with received events, and prevent the Eventor from entering a bad state.
 * To achieve this it is mandatory to have at least 1 RX bank or 1 free bank.
 * The context is not configured in rx to tx mode.
 * For every context buffer we allocate 4 words for the buffer containing the packet start,
 * 8 words for the descriptor chain containing one descriptor, 4 words to copy the packet data to.
 * If we have free banks (not used for rx and tx), we use one or two (if available) of them for the unused RX contexts.
 * If not, we use one of the existing rx banks
 * In case 2 banks are used for rx a set for all unused contexts is placed in the end of the buffer.
 *
 *
 * */
static shr_error_e
dnx_instru_eventor_configure_unused_rx_contexts(int unit, uint32 nof_rx_context)
{

    uint32 nof_rx_banks = 0;
    uint32 bank_index;
    uint32 context_i, buffer_size_i;
    uint8 is_rx_bank, is_tx_bank, is_rx_context;
    uint8 context_index, unused_context_i;
    int unused_rx_context_bank_1 = -1;
    int unused_rx_context_bank_2 = -1;
    uint32 buffer_1_start=0, program_1_start=0;
    uint32 buffer_2_start=0, program_2_start = 0;
    uint32 nof_unused_rx_contexts = DNX_EVENTOR_NOF_CONTEXTS(unit) - nof_rx_context;
    uint32 buf_i1_start, buf_i1_end, buf_i2_start, buf_i2_end, buf_i1_bank, buf_i2_bank;
    bcm_instru_eventor_operation_t operation = {0};
    bcm_instru_eventor_context_conf_t conf;
    uint32 free_words_needed_in_bank = nof_unused_rx_contexts * (UNUSED_RX_CONTEXT_BUFFER_SIZE + UNUSED_RX_CONTEXT_DESCRIPTOR_CHAIN_SIZE);
    uint32 needed_free_area_start_for_one_alloc = DNX_EVENTOR_SRAM_BANKS_NET_SIZE_WORDS(unit) - free_words_needed_in_bank;
    uint32 needed_free_area_start_for_two_allocs = DNX_EVENTOR_SRAM_BANKS_NET_SIZE_WORDS(unit) - DNX_EVENTOR_NOF_BUFFERS_IN_DOUBLE_BUFFER*free_words_needed_in_bank;
    uint32 *bank_first_free_address  = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO( bank_first_free_address, sizeof(uint32) * DNX_EVENTOR_NOF_SRAM_BANKS(unit),
                       "sram bank highest used address", "%s%s%s", EMPTY, EMPTY, EMPTY);

    if (nof_unused_rx_contexts == 0) {
        SHR_EXIT(); /** If we have no unused RX contexts, exit */
    }

    /* Find the 1/2 banks to use for unused RX contexts */
    /* First look for free banks and rx banks */
    for (bank_index = 0; bank_index < DNX_EVENTOR_NOF_SRAM_BANKS(unit); bank_index++)
    {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_get(unit, bank_index, &is_rx_bank));
        if (is_rx_bank)
        {
            nof_rx_banks++;
        }
        else
        {
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_sram_banks_bitmap.bit_get(unit, bank_index, &is_tx_bank));
            if (!is_tx_bank)
            {
                /** Found first free bank use it for both unused rx context banks*/
                if (unused_rx_context_bank_1 == -1) {
                    unused_rx_context_bank_1 = bank_index;
                    unused_rx_context_bank_2 = bank_index;
                    /** mark bank as rx bank */
                    SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_set(unit, bank_index));
                }
                else
                {
                    /* found second free buffer use it for second double buffer - end search */
                    unused_rx_context_bank_2 = bank_index;
                    /** mark bank as rx bank */
                    SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_set(unit, bank_index));
                    break;
                }
            }
        }
    }

    /* no free bank was found try to use allocated rx banks */
    if (unused_rx_context_bank_1 == -1)
    {
        /* Check that we have at least one rx bank */
        if (nof_rx_banks == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,"at least one free SRAM bank or an RX bank\n");
        }

        /* Find the start of the unused area at the end of each RX bank for each rx bank */
        for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++)
        {
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_get(unit, context_i, &is_rx_context));
            if (is_rx_context)
            {
                /** For each RX context */
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer_size.get(unit, context_i, &buffer_size_i));
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer0_offset.get(unit, context_i, &buf_i1_start));
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer1_offset.get(unit, context_i, &buf_i2_start));

                buf_i1_bank = buf_i1_start & DNX_EVENTOR_SRAM_BANK_MASK(unit);
                buf_i2_bank = buf_i2_start & DNX_EVENTOR_SRAM_BANK_MASK(unit);

                buf_i1_start >>= DNX_EVENTOR_SRAM_BANK_BITS(unit);
                buf_i2_start >>= DNX_EVENTOR_SRAM_BANK_BITS(unit);

                buf_i1_end = buf_i1_start + buffer_size_i;
                buf_i2_end = buf_i2_start + buffer_size_i;

                if ( bank_first_free_address[buf_i1_bank] < buf_i1_end)
                {
                    bank_first_free_address[buf_i1_bank] = buf_i1_end;
                }
                if (bank_first_free_address[buf_i2_bank] < buf_i2_end)
                {
                    bank_first_free_address[buf_i2_bank] = buf_i2_end;
                }
            }
        }

        /** find RX sram banks with free space at the end for unused rx contexts */
        for (bank_index = 0; bank_index < DNX_EVENTOR_NOF_SRAM_BANKS(unit); bank_index++)
        {
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_get(unit, bank_index, &is_rx_bank));
            if (is_rx_bank && bank_first_free_address[bank_index] <= needed_free_area_start_for_one_alloc)
            {
                if (unused_rx_context_bank_1 == -1) {
                    unused_rx_context_bank_1 = bank_index;
                    /** check if there is room for second buffer */
                    if (bank_first_free_address[bank_index] <= needed_free_area_start_for_two_allocs)
                    {
                        unused_rx_context_bank_2 = bank_index;
                    }
                }
                else
                {
                    unused_rx_context_bank_2 = bank_index;
                    /** found 2 rx banks for unused  rx contexts - end the search */
                    break;
                }
            }

        }
    }

    if (unused_rx_context_bank_2 == -1) {
        SHR_ERR_EXIT(_SHR_E_PARAM,"There is no free bank and no RX banks with enough free space at their end.\n");
    }

    /** calculate the locations use in unused_rx_context_bank_1 : 4 words input data, 8 words descriptor for each context */
    buffer_1_start = DNX_EVENTOR_SRAM_BANKS_NET_SIZE_WORDS(unit) - (nof_unused_rx_contexts * UNUSED_RX_CONTEXT_BUFFER_SIZE);
    program_1_start = buffer_1_start - (nof_unused_rx_contexts * UNUSED_RX_CONTEXT_DESCRIPTOR_CHAIN_SIZE);

    /** calculate the locations use in unused_rx_context_bank_2 */
    buffer_2_start = program_1_start;
    if (unused_rx_context_bank_1 != unused_rx_context_bank_2)
    {
        buffer_2_start = DNX_EVENTOR_SRAM_BANKS_NET_SIZE_WORDS(unit);
    }
    buffer_2_start -= (nof_unused_rx_contexts * UNUSED_RX_CONTEXT_BUFFER_SIZE);
    program_2_start = buffer_2_start - (nof_unused_rx_contexts * UNUSED_RX_CONTEXT_DESCRIPTOR_CHAIN_SIZE);

    SHR_IF_ERR_EXIT(bcm_instru_get_register_access_info(unit, BCM_INSTRU_ACCESS_FLAG_IS_WRITE, ECI_VERSION_REGISTERr, 0, 0, &operation.access_info));

    /* loop over RX contexts, and handle the unused ones */
    for (unused_context_i = 0, context_index = 0; context_index < DNX_EVENTOR_NOF_CONTEXTS(unit); context_index++)
    {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_get(unit, context_index, &is_rx_context));
        if (!is_rx_context) { /** this is a rx context that will be set to default buffer and descriptor set */
            if (unused_context_i >= nof_unused_rx_contexts) {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,"found too many unused contexts\n");
            }
            /** allocate default buffer space and descriptors */
            conf.flags = 0;
            conf.bank1  = unused_rx_context_bank_1;
            conf.bank2  = unused_rx_context_bank_2;

            /* buffer address - each buffer size is a minimum of 4 words */
            conf.buffer1_start = buffer_1_start + unused_context_i * UNUSED_RX_CONTEXT_BUFFER_SIZE;
            conf.buffer2_start = buffer_2_start + unused_context_i * UNUSED_RX_CONTEXT_BUFFER_SIZE;
            conf.buffer_size = UNUSED_RX_CONTEXT_BUFFER_SIZE;
            conf.program1_start = program_1_start + unused_context_i * UNUSED_RX_CONTEXT_DESCRIPTOR_CHAIN_SIZE;
            conf.program2_start = program_2_start + unused_context_i * UNUSED_RX_CONTEXT_DESCRIPTOR_CHAIN_SIZE;
            conf.nof_operations = 1;
            conf.operations = &operation;
            /* call bcm_dnx_instru_eventor_context_set(), but without marking the context as used and without setting it to rx to tx mode */
            SHR_IF_ERR_EXIT(dnx_instru_eventor_context_set_internal(unit, 0, context_index, bcmEventorContextTypeRx, &conf, 0));

            unused_context_i++;
        }
    }
exit:
    SHR_FREE(bank_first_free_address);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Start or stop the Eventor, with current devices only start is supported.
 *  Eventor activation init function.
 *  Used to init the Eventor after the initial configuration is set
 *  using APIs, before the Eventor is used.
 * \param [in] unit - The unit number.
 * \param [in] flags - flags
 * \param [in] active - make the Eventor active or not
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

int bcm_dnx_instru_eventor_active_set(
    int unit,
    uint32 flags,
    int active)
{
    int sbus_dma_channel_values[MAX_NOF_EVENTOR_SBUS_DMA_CHANNELS];  /* values read from soc property */
    int nof_sbus_dma_channel_values, cmc, channel;
    uint8 cmic_engine_index;
    uint32 context_i, bank_i, buff0_start_address, buff1_start_address, sw_state_flags, buffer_size_i, program0_offset, program1_offset, builders;
    uint32 index;
    uint32 tx_context_type_bitmap = 0;
    uint32 addr;
    uint32 nof_rx_context = 0;

    uint8 is_rx_bank;
    uint32 entry_handle_id;
    uint8 is_rx_context;
    uint8 is_tx_context;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_instru_eventor_active_set_verify(unit, flags, active));

    SHR_IF_ERR_EXIT(instru.eventor_info.flags.get(unit, &sw_state_flags));

    /** Already enabled, skip */
    if ((sw_state_flags & DNX_EVENTOR_STATE_FLAGS_ENABLED) != 0) {
        SHR_EXIT();
    }

    /*
     * 1. Configure RX context buffers to be read directly by TX builders in EVNT_RX_TO_TX_BUFFERr
     * 2. Mark RX banks in sw state
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_RX_CONTEXT_TO_TX_MAP, &entry_handle_id));

    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_get(unit, context_i, &is_rx_context));
        if (is_rx_context) { /** this is a rx context that will be mapped directly to a builder, Note this builder cannot be used by any other context */
            /** set RX context to be used directly by the builder with the same ID */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_MAP, context_i, TRUE);
            /**Count the number of configured rx contexts */
            nof_rx_context++;
        } else {
            /** set context to not be mapped directly to the builder with the same ID */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_MAP, context_i, FALSE);
        }
    }

    SHR_IF_ERR_EXIT(dnx_instru_eventor_configure_unused_rx_contexts(unit, nof_rx_context));

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Enable eventor Rx functionality */
    if (nof_rx_context > 0 && dnx_data_instru.eventor.is_rx_en_get(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_RX_INTERFACE_EN, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RXI_EN, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */
    }

    /** assign SRAM banks for usage by RX or by TX in EVNT_MAIN_MEM_BANK_TYPEr[bank] */
    /** Uses software state that was set in the previous loop */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_BANK_TYPE, entry_handle_id));
    for (bank_i = 0; bank_i < DNX_EVENTOR_NOF_SRAM_BANKS(unit); bank_i++) {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_SRAM_BANK_ID, bank_i); /** set key - sram bank id */
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_get(unit, bank_i, &is_rx_bank));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TX_BANK, INST_SINGLE, !is_rx_bank); /** set value - enable_map */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */
    }


    /*
     * The IS_FIFO_DMA_CFG field configures if the context is used in RX as
     * FIFO DMA or as sbus descriptor DMA.
     * If used as sbus descriptor DMA, the engine field is set dynamically
     * for each handled event. If used as FIFO DMA, the software needs to
     * configure the engine to use.
     * The same engine configuration table is used for FIFO and sbus DMA,
     * so some engines need to be configured in EVNT_CMIC_ENG_PROPERTIES as
     * FIFO DMA channels while others  as FIFO DMA channels.
     *
     * We assume here that EVNT_ASSIGNED_CMIC_ENGINE_TABLEm was initialized to zero.
     */

    /** FIFO DMA contexts also need to be marked here */
    /** We write zero to EVNT_TX_CONTEXT_IS_FIFO_DMAr to mark non-FIFO */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_TX_CONTEXT_IS_FIFO_DMA, entry_handle_id));
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FIFO_DMA, context_i, FALSE); /** set value - is_fifo_dma */
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /*
     * per context TX size thresholds. Start polling CMIC for DMA to be done
     * after this number of words are written to the buffer. Used for both SBUS
     * descriptor DMA and FIFO DMA.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_TX_CONTEXT_MAIN_BUFFER_TH, entry_handle_id));
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++)
    {
        /** set value - sbus dma buffers threshold */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_BUFFER_TH, context_i,
                                     DNX_EVENTOR_SBUS_DMA_BUFFERS_DEFAULT_THRESH);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /** write rx buffer sizes to EVNT_RX_MAIN_BUFFER_THr */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_RX_CONTEXT_MAIN_BUFFER_TH, entry_handle_id));
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++)
    {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer_size.get(unit, context_i, &buffer_size_i));
        /** set value - rx buffer threshold */
        if (buffer_size_i)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_BUFFER_TH, context_i, buffer_size_i);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_BUFFER_TH, context_i, 1);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /*
     * Configure RX contexts in device: double buffer.
     * A bank uses offsets 0-0x33ff for data and 0x3400-0x3fff for ECC.
     * Here SRAM addresses are divided by 4, so data offset is in 0-0xcff.
     */
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++)
    {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_get(unit, context_i, &is_rx_context));
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer_size.get(unit, context_i, &buffer_size_i));
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer0_offset.get(unit, context_i, &buff0_start_address));
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.buffer1_offset.get(unit, context_i, &buff1_start_address));
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.program0_offset.get(unit, context_i, &program0_offset));
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.program1_offset.get(unit, context_i, &program1_offset));

        if (buffer_size_i) {
            
            SHR_IF_ERR_EXIT(dnx_instru_eventor_active_context_set(
                    unit, context_i, buffer_size_i, buff0_start_address, buff1_start_address, program0_offset, program1_offset, FALSE));
        }
    }

    /* Configure TX Context */
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++)
    {
        SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_bitmap.bit_get(unit, context_i, &is_tx_context));
        if (is_tx_context)
        {
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer_size.get(unit, context_i, &buffer_size_i));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer0_offset.get(unit, context_i, &buff0_start_address));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.buffer1_offset.get(unit, context_i, &buff1_start_address));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.program0_offset.get(unit, context_i, &program0_offset));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.program1_offset.get(unit, context_i, &program1_offset));
            SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.builders.get(unit, context_i, &builders));

            if (buffer_size_i) {
                SHR_IF_ERR_EXIT(dnx_instru_eventor_active_context_set(
                        unit, context_i, buffer_size_i, buff0_start_address,buff1_start_address, program0_offset, program1_offset, TRUE));

                /** Set threshold for tx to 7*/
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_TX_CONTEXT_MAIN_BUFFER_TH, entry_handle_id));
                /** set value - sbus dma buffers threshold */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_BUFFER_TH, context_i, 7);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */
            }

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_CONTEXT_TO_BUILDER_MAPPING, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CONTEXT_ID, context_i); /** set key - context_i */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPING_DATA, INST_SINGLE, builders);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

            /** All TX contexts with the same ID as a builder used by a standard TX context must be marked standard */
            tx_context_type_bitmap |= builders;

            tx_context_type_bitmap |= (0x1 << context_i);
        }
    }

    /*
     * Assign CMIC DMA channels to Eventor DMA engines and configure them
     * configure CMIC registers AXI addresses per Eventor CMIC engine.
     * The "engines" (an Eventor only term) to FIFO DMA channels while others need to be mapped to FIFO DMA channels.
     * The mapping is done in EVNT_CMIC_ENG_PROPERTIES.
     * For SBUSDMA configure:
     * ENGINE_CMIC_DSA_ADDR    - CMIC_CMCx_SBUSDMA_CHy_DESC_START_ADDRESS_LO
     * ENGINE_CMIC_CTRL_ADDR   - CMIC_CMCx_SBUSDMA_CHy_CONTROL
     * ENGINE_CMIC_STATUS_ADDR - CMIC_CMCx_SBUSDMA_CHy_STATUS
     */
    nof_sbus_dma_channel_values = soc_property_get_csv(unit, spn_EVENTOR_SBUS_DMA_CHANNELS, MAX_NOF_EVENTOR_SBUS_DMA_CHANNELS, sbus_dma_channel_values);
    if (nof_sbus_dma_channel_values > 0) {
        if ((nof_sbus_dma_channel_values & 1)) {
            SHR_ERR_EXIT(_SHR_E_PARAM,"soc property %s must have an even number of comma separated values\n", spn_EVENTOR_SBUS_DMA_CHANNELS);
        } else if (nof_sbus_dma_channel_values > MAX_NOF_EVENTOR_SBUS_DMA_CHANNELS) {
            SHR_ERR_EXIT(_SHR_E_PARAM,"soc property %s can not have more than %u channels\n", spn_EVENTOR_SBUS_DMA_CHANNELS, MAX_NOF_EVENTOR_SBUS_DMA_CHANNELS);
        }
        cmic_engine_index = 0;
        for (index = 0; index < nof_sbus_dma_channel_values; index++) {
            cmc = sbus_dma_channel_values[index++];
            channel = sbus_dma_channel_values[index];
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_CMIC_ENGINE_PROPERTIES, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_ENGINE_ID, cmic_engine_index); /** set key - cmic engine index id */
            addr = AXI_CMIC_OFFSET + CMIC_CMCx_SBUSDMA_CHy_DESCADDR_LO(cmc,channel);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_DSA_ADDR, INST_SINGLE, addr);

            addr = AXI_CMIC_OFFSET + CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,channel);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_CTRL_ADDR, INST_SINGLE, addr);
            addr = AXI_CMIC_OFFSET + CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc,channel);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_STATUS_ADDR, INST_SINGLE, addr);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

            addr = CMIC_CMCx_SBUSDMA_CHy_DESCADDR_HI(cmc,channel);
            soc_pci_write(unit, EVENTOR_SRAM_DIRECT_MEM_ACCESS(unit,addr), 0);
            /** add the SBUS DMA channel to the engines pool - register name: EVNT_SHARED_CMIC_ENG_POOLr */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_SHARED_CMIC_ENG_POOL, entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_ENGINE_NUM, 0, cmic_engine_index);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */


            cmic_engine_index++;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Eventor SBUS DMA # of channels cannot be 0\n");
    }

    /** set eventor queue type 0-none standard, 1- standard */
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++)
    {
        uint32 queue_type;
        queue_type = tx_context_type_bitmap & (0x1 <<context_i);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_TX_CONTEXT_QUEUE_TYPE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CONTEXT_ID, context_i); /** set key - builder id */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_STANDARD_QUEUE_TYPE, INST_SINGLE, (queue_type ? 1 : 0));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */
    }

    /** SBUSDMA slices of SBUSDMA channels used for the Eventor (and only them)
     * should be configured to the same AXI ID, different from other slices.
     * In iproc >=17 this affects the selection of the channels to use as a slice contains two channels. */
    
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, CMIC_CMC1_SHARED_SBUS_DMA_READ_AXI_MAP_CTRLr, REG_PORT_ANY,
                    SLICE0_DATA_READ_AXIIDf, 0x6));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, CMIC_CMC1_SHARED_SBUS_DMA_READ_AXI_MAP_CTRLr, REG_PORT_ANY,
                    SLICE1_DATA_READ_AXIIDf, 0x6));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, CMIC_CMC1_SHARED_SBUS_DMA_READ_AXI_MAP_CTRLr, REG_PORT_ANY,
                    SLICE2_DATA_READ_AXIIDf, 0x6));
    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, CMIC_CMC1_SHARED_SBUS_DMA_READ_AXI_MAP_CTRLr, REG_PORT_ANY,
                    SLICE3_DATA_READ_AXIIDf, 0x6));
    /*
     * write EVNT_TX_CMIC_FLEX_CMDe configuring how the Eventor controls DMA operations
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_CMIC_FLEX_CMD, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TX_CMIC_FLEX, 1); /** set key - cmic flex is TX */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_DESC_DMA_CONTROL, INST_SINGLE, 0x5);
    /**Value used to start SBUS DMA operations*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_FIFO_DMA_CFG, INST_SINGLE, 0x3e8093);
    /** TX_CMIC_FIFO_DMA_CFG_WR_STROBEf unfortunately does not work (does not mask writes)*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_FIFO_DMA_CFG_WR_STROBE, INST_SINGLE, 0xffffffff);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */
    /** We do not use DMA in RX, configure zeroes in its configuration.*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TX_CMIC_FLEX, 0); /** set key - cmic flex is RX */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_DESC_DMA_CONTROL, INST_SINGLE, 0x5);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_FIFO_DMA_CFG, INST_SINGLE, 0x3e8093);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_FIFO_DMA_CFG_WR_STROBE, INST_SINGLE, 0xffffffff);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /** mark the Eventor as active */
    SHR_IF_ERR_EXIT(instru.eventor_info.flags.get(unit, &flags));
    SHR_IF_ERR_EXIT(instru.eventor_info.flags.set(unit, flags | DNX_EVENTOR_STATE_FLAGS_ENABLED));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for bcm_dnx_instru_eventor_active_get() API
 */
static shr_error_e
dnx_instru_eventor_active_get_verify(
    int unit,
    uint32 flags,
    int *out_active)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(out_active, _SHR_E_PARAM, "out_active");

    /** check if the Eventor is supported */
    if (dnx_data_instru.eventor.feature_get(unit, dnx_data_instru_eventor_is_supported) == FALSE) {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "There is no Eventor support for this device.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check whether the Eventor is active/started
 * \param [in] unit - The unit number.
 * \param [in] flags - flags
 * \param [out] out_active - Indication whether the Eventor is active (TRUE/FALSE)
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
int
bcm_dnx_instru_eventor_active_get(
    int unit,
    uint32 flags,
    int *out_active)
{
    uint32 sw_state_flags;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNXC(dnx_instru_eventor_active_get_verify(unit, flags, out_active));

    /** get active indication from sw state */
    SHR_IF_ERR_EXIT(instru.eventor_info.flags.get(unit, &sw_state_flags));
    *out_active = (sw_state_flags & DNX_EVENTOR_STATE_FLAGS_ENABLED) ? 1 : 0;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - get the low level access information for a register
* \param [in] unit - The unit number.
* \param [in] flags - read or write operation default is read (so we will not get broadcast blocks returned by mistake)
* \param [in] reg - register
* \param [in] instance - In case of port registers the port number, otherwise the (block) instance number, BCM_BLOCK_ALL will provide a broadcast write if possible or an error
* \param [in] array_index - in case the register is an array this is the array index to be accessed, otherwise should be 0
* \param [out] access_info - The output access information
* \return
*   See \ref shr_error_e
* \remark
*   None.
* \see
*   None.
*/
int
bcm_dnx_instru_get_register_access_info(
  int unit,
  uint32 flags,
  soc_reg_t reg,
  uint32 instance,
  uint32 array_index,
  bcm_instru_access_info_t* access_info)
{
    soc_reg_access_info_t reg_access_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(access_info, _SHR_E_PARAM, "access_info");
    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    /** Get the register address and block ID */
    if (soc_reg_xaddr_get(unit, reg, instance, array_index,
           (flags & BCM_INSTRU_ACCESS_FLAG_IS_WRITE) != 0 ?
             SOC_REG_ADDR_OPTION_WRITE : SOC_REG_ADDR_OPTION_NONE,
           &reg_access_info) != SOC_E_NONE ||
        reg_access_info.num_blks != 1) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "could not get the address for register %s%s%s.\n",  SOC_REG_NAME(unit, reg), EMPTY, EMPTY);
    }

    /** return the access information in access_info */
    access_info->flags = flags; /* Mark that this is a read/write */
    access_info->address = reg_access_info.offset; /* address/offset inside the block */
    access_info->phys_block = reg_access_info.blk_list[0]; /** get sbus block ID */
    access_info->nof_entries = 1; /** number of consecutive entries to read/write */
    access_info->entry_size = /** size of register entry in 32 bit words */
      SOC_REG_IS_ABOVE_64(unit, reg) ? SOC_REG_ABOVE_64_INFO(unit, reg).size :
        (SOC_REG_IS_64(unit, reg) ? 2 : 1);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - get the low level access information for a memory entry
* \param [in] unit - The unit number.
* \param [in] flags - read or write operation, default is read (so we will not get broadcast blocks returned by mistake)
* \param [in] mem - memory
* \param [in] instance_number - The instance number, BCM_BLOCK_ALL will provide a broadcast write if possible or an error
* \param [in] index - the entry number (index) in the memory to be accessed
* \param [in] array_index - in case the memory is an array, this is the array index to be accessed, otherwise should be 0
* \param [out] access_info - The output access information
* \return
*   See \ref shr_error_e
* \remark
*   None.
* \see
*   None.
*/
int
bcm_dnx_instru_get_memory_access_info(
   int unit,
   uint32 flags,
   soc_mem_t mem,
   int instance_number,
   uint32 index,
   uint32 array_index,
   bcm_instru_access_info_t *access_info)
{
    int copyno;
    uint8 access_type;
    int sbus_blk, copyno_override;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(access_info, _SHR_E_PARAM, "access_info");
    if (!soc_mem_is_valid(unit, mem)) {
        return SOC_E_MEMORY;
    }
    SOC_MEM_ALIAS_TO_ORIG(unit, mem); /** Get the real memory from a memory alias */

    copyno = SOC_MEM_BLOCK_MIN(unit,mem) + instance_number;
    /** If this is a write operation, get a broadcast block if needed */
    if ((flags & BCM_INSTRU_ACCESS_FLAG_IS_WRITE) != 0 ) {
        soc_mem_write_copyno_update(unit, mem, &copyno, &copyno_override);
        if (copyno_override) {
            copyno = copyno_override;
        }
    }
    /** if requesting all instances, but do not have a broadcast block to write, use only one instance */
    if (copyno == SOC_CORE_ALL || copyno == COPYNO_ALL) {
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }

    /** return the access information in access_info */
    access_info->flags = flags | BCM_INSTRU_ACCESS_FLAG_IS_MEM; /* Mark that this is a memory, read/write */
    access_info->address = soc_mem_addr_get(unit, mem, array_index, copyno, index, &access_type); /* address/offset inside the block */

    /** get sbus block ID */
    soc_mem_dst_blk_update(unit, copyno, access_info->address, &sbus_blk);
    access_info->phys_block = sbus_blk;

    access_info->nof_entries = 1; /** number of consecutive entries to read/write */
    access_info->entry_size = soc_mem_entry_words(unit, mem); /** size of memory entry in 32 bit words */

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - Generate Eventor data event header,
* Eventor data events consist with a 32 bit header written before the event data.
* The header structure is : (msb to lsb)
* - 8 bit dataevent id
* - 1 bit push flag
* - 2 bits format
* - 21 bit length (in long size mode)
* This function create the data event header according to the context configuration.
*
* \param [in] unit - The unit number.
* \param [in] flags - relevant flags.
* \param [in] context - The configured contex Id.
* \param [in] context_conf - the configured context configuration
* \param [out] eventor_header - the generated eventor header
*
* \return
*   See \ref shr_error_e
* \remark
*   None.
* \see
*   None.
*/
void
dnx_instru_eventor_generate_tx_event_eventor_header(
        int unit,
        uint32 flags,
        bcm_eventor_context_id_t context,
        bcm_instru_eventor_context_conf_t *context_conf,
        uint32 *eventor_header)
{
    bcm_instru_eventor_operation_t *operation_ptr;
    uint8 push = 0;

    uint8 operation_index;
    uint32 access_length = 0;

    for (operation_index=0; operation_index<context_conf->nof_operations; operation_index++)
    {
        operation_ptr = &context_conf->operations[operation_index];

        access_length += operation_ptr->access_info.entry_size * operation_ptr->access_info.nof_entries;
        if (dnx_data_instru.eventor.is_extra_words_copy_get(unit))
        {
            /** overcome extra words written to the Eventor buffer by DMA in JR2 */
            access_length += (0x1000 -(operation_ptr->buf_offset + operation_ptr->access_info.entry_size)) & 0x3;
        }
    }

    if (flags & BCM_INSTRU_EVENTOR_CONTEXT_CONF_NO_AGGREGATION)
    {
        push = 0x1;
    }

    /** set format to explicit long size , no push */
    *eventor_header = (context << 24) | (push << 23) | (1<<21) | (access_length * 4);
    context_conf->out_event_size = access_length;
}
/**
* \brief - write dma descriptor chain to SRAM
* \param [in] unit - The unit number.
* \param [in] context - The relevat context ID.
* \param [in] context_conf - the configured context configuration
* \return
*   See \ref shr_error_e
* \remark
*   None.
* \see
*   None.
*/
int
dnx_instru_eventor_write_dma_descriptors(
        int unit,
        bcm_eventor_context_id_t context,
        const bcm_instru_eventor_context_conf_t *context_conf)
{
    soc_sbusdma_desc_ctrl_t desc_ctrl = {0};
    soc_sbusdma_desc_cfg_t  *desc_cfg = NULL;
    uint8 desc_index;
    uint8 buffer_index;
    uint32 eventor_chain_address[DNX_EVENTOR_NOF_BUFFERS_IN_DOUBLE_BUFFER];
    uint32 eventor_buffer_address[DNX_EVENTOR_NOF_BUFFERS_IN_DOUBLE_BUFFER];
    uint32 buff_addr;
    void *desc_ptr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(context_conf, _SHR_E_PARAM, "context_conf");

    eventor_chain_address[0] = DNX_EVENTOR_AXI_SRAM_OFFSET(unit) + DNX_EVENTOR_SRAM_BANKS_FULL_SIZE(unit) * context_conf->bank1 + context_conf->program1_start * 4;
    eventor_chain_address[1] = DNX_EVENTOR_AXI_SRAM_OFFSET(unit) + DNX_EVENTOR_SRAM_BANKS_FULL_SIZE(unit) * context_conf->bank2 + context_conf->program2_start * 4;
    eventor_buffer_address[0] = DNX_EVENTOR_AXI_SRAM_OFFSET(unit) + DNX_EVENTOR_SRAM_BANKS_FULL_SIZE(unit) * context_conf->bank1 + context_conf->buffer1_start * 4;
    eventor_buffer_address[1] = DNX_EVENTOR_AXI_SRAM_OFFSET(unit) + DNX_EVENTOR_SRAM_BANKS_FULL_SIZE(unit) * context_conf->bank2 + context_conf->buffer2_start * 4;

    desc_ctrl.flags = SOC_SBUSDMA_CFG_USE_FLAGS;
    desc_ctrl.name[0] = '\0';
    desc_ctrl.cfg_count = context_conf->nof_operations;             /** Number of items in descriptor chain */
    desc_ctrl.hw_desc = NULL;                                       /** Custom hw descriptor info */
    /** we write the complete desc chain no need to send buff ptr */
    desc_ctrl.buff = NULL;                                          /** Common/contiguous host memory buffer for the complete chain */
    desc_ctrl.cb = NULL;                                            /** Callback function for completion */
    desc_ctrl.data = NULL;                                          /** Caller's data transparently returned in callback */

    SHR_ALLOC_SET_ZERO(desc_cfg, sizeof(soc_sbusdma_desc_cfg_t) * context_conf->nof_operations,
                           "wanted_members_array_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    for (buffer_index=0; buffer_index<DNX_EVENTOR_NOF_BUFFERS_IN_DOUBLE_BUFFER; buffer_index++)
    {
        for (desc_index=0; desc_index<context_conf->nof_operations; desc_index++)
        {
            soc_sbusdma_desc_cfg_t * desc_cfg_ptr = &desc_cfg[desc_index];
            bcm_instru_eventor_operation_t *operation_ptr = &context_conf->operations[desc_index];

            desc_cfg_ptr->blk = operation_ptr->access_info.phys_block;              /** CMIC block ID */
            desc_cfg_ptr->addr = operation_ptr->access_info.address;                /** address/offset inside the block */
            desc_cfg_ptr->width = operation_ptr->access_info.entry_size;            /** size of register/memory entry in 32 bit words */
            desc_cfg_ptr->count = operation_ptr->access_info.nof_entries;           /** number of consecutive entries to read/write */
            desc_cfg_ptr->addr_shift = 0;
            buff_addr = (eventor_buffer_address[buffer_index] + operation_ptr->buf_offset * 4); /** host (SRAM) address to use for sbus operation */
            desc_cfg_ptr->buff = INT_TO_PTR(buff_addr);
            desc_cfg_ptr->flags = (operation_ptr->access_info.flags & BCM_INSTRU_ACCESS_FLAG_IS_WRITE) ? SOC_SBUSDMA_WRITE_CMD_MSG : 0;
            if ((operation_ptr->access_info.flags & BCM_INSTRU_ACCESS_FLAG_IS_MEM)) {
                desc_cfg_ptr->flags |= SOC_SBUSDMA_MEMORY_CMD_MSG;
            }
        }
        desc_ptr = (void *)(sal_vaddr_t)EVENTOR_SRAM_DIRECT_MEM_ACCESS(unit, (eventor_chain_address[buffer_index] & 0xfffff));
        SHR_IF_ERR_EXIT(soc_sbusdma_desc_create(unit, &desc_ctrl, desc_cfg, FALSE, desc_ptr));
    }

exit:
    SHR_FREE(desc_cfg);
    SHR_FUNC_EXIT;
}
