/**
 * \file instru_eventor.c
 *
 * Eventor: Eventor APIs implementation
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EVENTOR

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_error.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/instru.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>       /* needed for the next line */
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/instru_access.h>
#include <bcm_int/dnx/instru/instru_eventor.h>

#include <soc/dnx/dbal/dbal.h>  /* for dbal */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>    /* for dnx_data_device */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>      /* for ucode_port */
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */
/* *INDENT-OFF* */

/*************
 * TYPE DEFS *
 *************/

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
    int builder, context;
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
     * Map context ID to standard/non standard context for FIFO DMA.
     * Use 0=non-standard (contain non-standard events) for FIFO and RX.
     * For non standard events the builder will be equal to the context ID and
     * not according to EVNT_TRANSACTION_ENGINE_BIT_MAPPING.
     * Write 0 to EVNT_TX_ENG_CONTEXT_QUEUE_TYPEr.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_TX_CONTEXT_QUEUE_TYPE, entry_handle_id));
    for (context = 0; context < DNX_EVENTOR_NOF_CONTEXTS(unit); context++) {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CONTEXT_ID, context); /** set key - context id */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_STANDARD_QUEUE_TYPE, INST_SINGLE, FALSE); /** set value:non-standard */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */
    }

    /** Disable packet padding (padding is done per builder up to maximum size of 1600B). */
    /** write 0 to EVNT_TX_ENG_EVPCK_PADDING_ENABLEr */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_TX_ENG_CFG, entry_handle_id));
    for (builder = 0; builder < DNX_EVENTOR_NOF_BUILDERS(unit); builder++) {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_TX_ENGINE_ID, builder); /** set key - builder id */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PADDING_ENABLE, INST_SINGLE, FALSE); /** set value - padding */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Eventor deinit function.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e soc_dnx_eventor_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Currently nothing to deinit
     */

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
    uint32 sw_state_flags, rx_buffer_size_i;
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
    } else if (conf->flags) { /** verify conf flags */
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flag/s in context configuration flags %u\n", conf->flags);
    }

    /** get sw state flags */
    SHR_IF_ERR_EXIT(instru.eventor_info.flags.get(unit, &sw_state_flags));

    switch (context_type) {
      case bcmEventorContextTypeRx: /** RX context type */
        /** verify whether modifications to eventor are allowed at this stage */
        if (sw_state_flags & DNX_EVENTOR_STATE_FLAGS_ENABLED) {
            SHR_ERR_EXIT(_SHR_E_BUSY, "Cannot change RX configuration after the Eventor is started.\n");
        }

        /** check that the buffers do not overlap with existing buffers */
        for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer_size.get(unit, context_i, &rx_buffer_size_i));
            if ((rx_buffer_size_i > 0) && (context != context_i)) {
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer0_offset.get(unit, context_i, &buf_i1_start));
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer1_offset.get(unit, context_i, &buf_i2_start));

                buf_i1_bank = buf_i1_start & DNX_EVENTOR_SRAM_BANK_MASK(unit);
                buf_i2_bank = buf_i2_start & DNX_EVENTOR_SRAM_BANK_MASK(unit);

                buf_i1_start >>= DNX_EVENTOR_SRAM_BANK_BITS(unit);
                buf_i2_start >>= DNX_EVENTOR_SRAM_BANK_BITS(unit);

                buf_i1_end = buf_i1_start + rx_buffer_size_i;
                buf_i2_end = buf_i2_start + rx_buffer_size_i;

                if (conf->bank1 == buf_i1_bank && dnx_instru_eventor_do_buffers_collide(
                  conf->buffer1_start, buffer1_end, buf_i1_start, buf_i1_end)) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 1st buffer bank %u word offset %u collides with specified 1st buffer.\n",
                      context_i, buf_i1_bank, buf_i1_start);
                } else if (conf->bank2 == buf_i1_bank && dnx_instru_eventor_do_buffers_collide(
                  conf->buffer2_start, buffer2_end, buf_i1_start, buf_i1_end)) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 1st buffer bank %u word offset %u collides with specified 2nd buffer.\n",
                      context_i, buf_i1_bank, buf_i1_start);
                } else if (conf->bank1 == buf_i2_bank && dnx_instru_eventor_do_buffers_collide(
                  conf->buffer1_start, buffer1_end, buf_i2_start, buf_i2_end)) {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "context %u 2nd buffer bank %u word offset %u collides with specified 1st buffer.\n",
                      context_i, buf_i2_bank, buf_i2_start);
                } else if (conf->bank2 == buf_i2_bank && dnx_instru_eventor_do_buffers_collide(
                  conf->buffer2_start, buffer2_end, buf_i2_start, buf_i2_end)) {
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
    unsigned context_i;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_eventor_context_set_verify(unit, flags, context, context_type, conf));

    switch (context_type) {
        case bcmEventorContextTypeRx: /** RX context type */
            /** save context info to sw state */
            for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
                /** update double buffer offsets */
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer0_offset.set(unit, context, conf->bank1 |
                                                                         (conf->buffer1_start << DNX_EVENTOR_SRAM_BANK_BITS(unit))));
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer1_offset.set(unit, context, conf->bank2 |
                                                                         (conf->buffer2_start << DNX_EVENTOR_SRAM_BANK_BITS(unit))));
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_set(unit, context)); /** mark as RX context */
                SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer_size.set(unit, context, conf->buffer_size)); /** set rx buffer size */
            }
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "unsupported context type flag %d%s%s.\n", context_type, EMPTY, EMPTY);
    }

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
    uint8 is_rx_context;
    uint32 sram_words_offset;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_eventor_context_get_verify(unit, flags, context, context_type, out_conf));

    SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_get(unit, context, &is_rx_context));

    /** handle rx context - get info from sw state */
    if (is_rx_context && (context_type == bcmEventorContextTypeRx || context_type == bcmEventorContextTypeAny)) {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer0_offset.get(unit, context, &sram_words_offset));
        out_conf->bank1 = sram_words_offset & DNX_EVENTOR_SRAM_BANK_MASK(unit);
        out_conf->buffer1_start = sram_words_offset >> DNX_EVENTOR_SRAM_BANK_BITS(unit);

        SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer1_offset.get(unit, context, &sram_words_offset));
        out_conf->bank2 = sram_words_offset & DNX_EVENTOR_SRAM_BANK_MASK(unit);
        out_conf->buffer2_start = sram_words_offset >> DNX_EVENTOR_SRAM_BANK_BITS(unit);

        SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer_size.get(unit, context, &out_conf->buffer_size));
    } else {
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
    uint32 sw_state_flags, rx_buffer_size, tx_max_header_size;
    uint8 is_rx_context;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(conf, _SHR_E_PARAM, "conf");

    /** verify if eventor is supported */
    if (dnx_data_instru.eventor.feature_get(unit, dnx_data_instru_eventor_is_supported) == FALSE) {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "There is no Eventor support for this device.\n");
    } else if (flags) { /** verify flags */
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags were specified.\n");
    } else if (builder >= DNX_EVENTOR_NOF_BUILDERS(unit)) { /** verify builder id */
        SHR_ERR_EXIT(_SHR_E_PARAM, "builder ID %u is out of range, should be under %u.\n", builder, DNX_EVENTOR_NOF_BUILDERS(unit));
    } else if (conf->header_length & 0x3) { /** verify header_length is 32 bits multiplication */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! header_length = %u, must be multiple of 4 bytes!\n", conf->header_length);
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
    SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer_size.get(unit, builder, &rx_buffer_size));

    if (is_rx_context) { /** This builder serves RX */
        /** check if eventor can be modified at this stage */
        if (sw_state_flags & DNX_EVENTOR_STATE_FLAGS_ENABLED) {
            SHR_ERR_EXIT(_SHR_E_BUSY, "Cannot change RX configuration after the Eventor is started.\n");
        } else if (conf->thresh_size <= 0 || (conf->thresh_size % rx_buffer_size)) { /** verify threshold size limits */
            SHR_ERR_EXIT(_SHR_E_PARAM, "thresh_size given %u is not a positive multiple of the rx buffer size %u.\n",
              conf->thresh_size, rx_buffer_size);
        } else if (conf->thresh_size > DNX_EVENTOR_BUILDER_MAX_BUFFER_SIZE_WORDS(unit)) {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Builder size threshold in words %u is greater that the max %u.\n",
              conf->thresh_size, DNX_EVENTOR_BUILDER_MAX_BUFFER_SIZE_WORDS(unit));
        }
    } else {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Currently only RX contexts are supported, and context ID %u was not configured as RX.\n", builder);
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
    uint32 buffer_size; 

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
            * Copy the firs 512 bits to header_data_high:
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
    SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer_size.get(unit, builder, &buffer_size));
    nof_samples = conf->thresh_size / buffer_size;
    
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
    uint8 is_rx_context;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_eventor_builder_set_verify(unit, flags, builder, conf));

    SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.bit_get(unit, builder, &is_rx_context));

    /** This builder serves RX - set tx builder timeout and size thresholds for transmission  */
    if (is_rx_context) {
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
    }

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
    SHR_INVOKE_VERIFY_DNX(dnx_instru_eventor_builder_get_verify(unit, flags, builder, out_conf));

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
    uint32 thresh_size, context_i, rx_buffer_size_i, buf0_offset, buf1_offset, entry_handle_id;
    uint8 is_builder_configured;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_instru.eventor.feature_get(unit, dnx_data_instru_eventor_is_supported) == FALSE) { /** check if eventor is supported */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "There is no Eventor support for this device.\n");
    } else if (active == 0) { /** stopping The Eventor is not allowed */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Stopping the Eventor is not supported.\n");
    }

    /** check that each rx context has a configured builder */
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer_size.get(unit, context_i, &rx_buffer_size_i));
        if (rx_buffer_size_i) {
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer0_offset.get(unit, context_i, &buf0_offset));
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer1_offset.get(unit, context_i, &buf1_offset));

            SHR_IF_ERR_EXIT(instru.eventor_info.builder_configured_bitmap.bit_get(unit, context_i, &is_builder_configured));

            if (is_builder_configured == 0) {
                LOG_ERROR(BSL_LOG_MODULE, ("Context %u configured as RX, and the same builder was not configured.\n", context_i));
                error = 1;
            } else {
                /* Verify that the builder max size threshold is a positive multiple of the rx buffer size */
                /** read entry_handle_id from EVNT_TX_ENG_CONTEXT_QUEUE_MAX_SIZEr[context_i] */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVENTOR_TX_ENG_CFG, &entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_TX_ENGINE_ID, context_i); /** set key - builder id */
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONTEXT_QUEUE_MAX_SIZE, INST_SINGLE, &thresh_size); /** request - size */
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT)); /** Get the entry */

                if (thresh_size == 0 || (thresh_size % rx_buffer_size_i)) {
                    LOG_ERROR(BSL_LOG_MODULE, ("RX context %u has a builder thresh_size %u which is not a positive multiple of the rx buffer size %u.\n",
                      context_i, thresh_size, rx_buffer_size_i));
                    error = 1;
                }
            }
        }
    }
    if (error) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Wrong Eventor RX configuration detailed above.\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
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
    uint32 context_i, bank_i, buff0_start_address, buff1_start_address, sw_state_flags, rx_buffer_size_i;
    uint32 buf0_offset, buf1_offset;
    uint8 is_rx_bank;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_eventor_active_set_verify(unit, flags, active));

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
    SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_range_clear(unit, 0,
                                                                             DNX_EVENTOR_NOF_SRAM_BANKS(unit)));
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer_size.get(unit, context_i, &rx_buffer_size_i));
        if (rx_buffer_size_i) { /** this is an RX context that will be mapped directly to a builder */
            /** set RX context to be used directly by the builder with the same ID */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_MAP, context_i, TRUE);

            /** get buffers bank+offset */
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer0_offset.get(unit, context_i, &buf0_offset));
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer1_offset.get(unit, context_i, &buf1_offset));
            /** mark the SRAM banks of the context's buffers as RX in sw state */
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_set(unit, buf0_offset & DNX_EVENTOR_SRAM_BANK_MASK(unit)));
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.bit_set(unit, buf1_offset & DNX_EVENTOR_SRAM_BANK_MASK(unit)));
        } else {
            /** set context to not be mapped directly to the builder with the same ID */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_MAP, context_i, FALSE);
        }
    }
    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));


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
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FIFO_DMA, context_i, FALSE); /** set value - is_fifo_dma */
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /*
     * per context TX size thresholds. Start polling CMIC for DMA to be done
     * after this number of words are written to the buffer. Used for both SBUS
     * descriptor DMA and FIFO DMA.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_TX_CONTEXT_MAIN_BUFFER_TH, entry_handle_id));
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
        /** set value - sbus dma buffers threshold */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_BUFFER_TH, context_i,
                                     DNX_EVENTOR_SBUS_DMA_BUFFERS_DEFAULT_THRESH);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /** write rx buffer sizes to EVNT_RX_MAIN_BUFFER_THr */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_RX_CONTEXT_MAIN_BUFFER_TH, entry_handle_id));
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer_size.get(unit, context_i, &rx_buffer_size_i));
        /** set value - rx buffer threshold */
        if (rx_buffer_size_i) {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_BUFFER_TH, context_i, rx_buffer_size_i);
        } else {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_BUFFER_TH, context_i, 1);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

    /*
     * Configure rx contexts in device: double buffer.
     * A bank uses offsets 0-0x33ff for data and 0x3400-0x3fff for ECC.
     * Here SRAM addresses are divided by 4, so data offset is in 0-0xcff.
     */
    for (context_i = 0; context_i < DNX_EVENTOR_NOF_CONTEXTS(unit); context_i++) {
        SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer_size.get(unit, context_i, &rx_buffer_size_i));
        if (rx_buffer_size_i) {
            

            /** calculate buffer 0 start and end address */
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer0_offset.get(unit, context_i, &buff0_start_address));
            buff0_start_address = (buff0_start_address & DNX_EVENTOR_SRAM_BANK_MASK(unit)) * DNX_EVENTOR_SRAM_BANKS_FULL_SIZE_WORDS(unit) +
              (buff0_start_address >> DNX_EVENTOR_SRAM_BANK_BITS(unit));

            /** calculate buffer 1 start and end address */
            SHR_IF_ERR_EXIT(instru.eventor_info.rx_buffer1_offset.get(unit, context_i, &buff1_start_address));
            buff1_start_address = (buff1_start_address & DNX_EVENTOR_SRAM_BANK_MASK(unit)) * DNX_EVENTOR_SRAM_BANKS_FULL_SIZE_WORDS(unit) +
              (buff1_start_address >> DNX_EVENTOR_SRAM_BANK_BITS(unit));

            /** set double buffer start and end address for the given context */
            /** EVNT_SPACE_REGIONSm[context_i*2+{0,1}] BUFFER_END_ADDRf=(BUFFER_START_ADDRf=buff{0,1}_start_address) + rx_buffer_size_i */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_SPACE_REGIONS, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TX_CONTEXT, FALSE); /** set key - is tx context */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CONTEXT_ID, context_i); /** set key - context id */
            /** set values: buffers 0,1 start and end addresses */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFFER_START_ADDR, 0, buff0_start_address);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFFER_END_ADDR, 0, buff0_start_address + rx_buffer_size_i);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFFER_START_ADDR, 1, buff1_start_address);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFFER_END_ADDR, 1, buff1_start_address + rx_buffer_size_i);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */

            /** set context descriptor chains start AXI addresses to the RX buffers */
            /** EVNT_CONTEXT_PROPERTIESm[context_i] {,DOUBLE_}BD_PTRf=AXI_offset+buff{0,1}_start_address */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EVENTOR_CONTEXT_PROPERITES, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TX_CONTEXT, FALSE); /** set key - is tx context */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENTOR_CONTEXT_ID, context_i); /** set key - context id */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_BUFFER_START_ADDR, 0,
                                         DNX_EVENTOR_AXI_SRAM_WORD_OFFSET(unit) + buff0_start_address);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMIC_BUFFER_START_ADDR, 1,
                                         DNX_EVENTOR_AXI_SRAM_WORD_OFFSET(unit) + buff1_start_address);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** commit */
        }
    }

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
    SHR_INVOKE_VERIFY_DNX(dnx_instru_eventor_active_get_verify(unit, flags, out_active));

    /** get active indication from sw state */
    SHR_IF_ERR_EXIT(instru.eventor_info.flags.get(unit, &sw_state_flags));
    *out_active = (sw_state_flags & DNX_EVENTOR_STATE_FLAGS_ENABLED) ? 1 : 0;

exit:
    SHR_FUNC_EXIT;
}
