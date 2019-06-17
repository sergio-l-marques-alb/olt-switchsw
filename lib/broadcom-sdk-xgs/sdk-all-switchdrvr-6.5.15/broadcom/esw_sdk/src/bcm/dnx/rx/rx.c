/**
 *  \file /src/bcm/dnx/rx/rx.c
 * $Id$
 * RX procedures for DNX.
 * Traps
 * Adapter
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */

#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm/switch.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/dnx/switch/switch.h>
#include <include/bcm_int/dnx/rx/rx.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <netdb.h>
#include <shared/bitop.h>
#include <shared/bslenum.h>
#include <bcm/port.h>
#include <bcm/knet.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_err_recovery_manager_utils.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnxc/dnxc_ha.h>

#ifdef ADAPTER_SERVER_MODE
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <fcntl.h>
#include <unistd.h>
#endif

/*
 * }
 */

/*
 * Macros and defines for rx thread management
 * {
 */
#ifdef ADAPTER_SERVER_MODE

/*
 * This is the default time between minimum token refreshes. It is also
 * the maximum time between RX thread wake-ups.
 */
#define BCM_RX_TOKEN_CHECK_US_DEFAULT   100000  /* 10 times/sec. */

#define BASE_SLEEP_VAL   500000

/*
 * Set sleep to base value
 */
#define INIT_SLEEP    rx_control.sleep_cur = BASE_SLEEP_VAL

/*
 * Lower sleep time if val is < current
 */
#define SLEEP_MIN_SET(val)                                           \
    (rx_control.sleep_cur = ((val) < rx_control.sleep_cur) ?         \
     (val) : rx_control.sleep_cur)
#endif

/*
 * Get the bits number
 */
#define DNX_RX_PARSE_HEADER_BITS_GET(_len)   ((_len) * SAL_UINT8_NOF_BITS)

/*
 * Get system header data information
 */
#define DNX_RX_PARSE_HEADER_DATA_GET(_unit, _type, _field)  (dnx_data_headers._type._field##_get(_unit))

/*
 * Get field value from Jericho2 system header
 * This is used to read this field's offset and length in the system header.
 * Then get this field's value with offset and length.
 */
#define DNX_RX_PARSE_HEADER_FIELD_GET(_unit, _type, _field, _val) \
    offset = DNX_RX_PARSE_HEADER_DATA_GET(_unit, _type, _field##_offset); \
    nof_bits = DNX_RX_PARSE_HEADER_DATA_GET(_unit, _type, _field##_bits); \
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &_val));

/*
 * Max length for certain system header
 */
#define DNX_RX_PARSE_HEADER_MAX_LEN                 0x20
/*
 * There is two padding bytes appendeded to packet in pipeline, which is included by packet_size of FTMH
 */
#define DNX_RX_PARSE_HEADER_OUT_LIF_EXT_TYPE        0x4
#define DNX_RX_PARSE_HEADER_PPH_FHEI_SIZE_SZ0       0x1
#define DNX_RX_PARSE_HEADER_PPH_FHEI_SIZE_SZ1       0x2
#define DNX_RX_PARSE_HEADER_PPH_FHEI_SIZE_SZ2       0x3

/*
 * }
 */
int bcm_adapter_rx_start(
    int unit,
    bcm_rx_cfg_t * cfg);

/**
 * \brief - Return boolean as to whether unit is running
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Boolean: TRUE if unit is running
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_active(
    int unit)
{
    return _bcm_common_rx_active(unit);
}

/**
 * \brief - Initialize the rx thread and resources
 *
 * \param [in] unit - Relevant unit
 * \param [in] cfg - RX configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   Starts the packet receive thread if not already running.
 *   cfg may be null:  Use default config.
 *   alloc/free in cfg may be null:  Use default alloc/free functions
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_start(
    int unit,
    bcm_rx_cfg_t * cfg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
       /*
        * Workaround to disable RPE ATE
        * When ATE test mode is enabled, only opcode byte is checked to decide whether a packet is RPE or not
        * Packets whose opcode byte happens to be a valid value will be sent to RPE instead of packet dma
        */
       SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RCPU_ENABLE_CTRL, &entry_handle_id));
       dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RCPU_FOR_ATE, INST_SINGLE, FALSE);
       SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

#ifdef ADAPTER_SERVER_MODE
    SHR_IF_ERR_EXIT(bcm_adapter_rx_start(unit, cfg));

#else
    if (bcm_dnx_rx_active(unit))
    {
        SHR_EXIT();
    }

    /*
     * Use default config, see _cmicx_rx_dflt_cfg
     */
    SHR_IF_ERR_EXIT(_bcm_common_rx_start(unit, cfg));
#endif
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate RX packet buffer
 *
 * \param [in] unit - Relevant unit
 * \param [in] pkt_size - Packet Size
 * \param [in] flags - Used to set up packet flags
 * \param [out] buf - Pointer to allocated buffer
 *
 * \return
 *   Pointer to new packet buffer or NULL if cannot alloc memory
 *
 * \remark
 *   If pkt_size <= 0, then the default packet size for the unit is used.
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_alloc(
    int unit,
    int pkt_size,
    uint32 flags,
    void **buf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_alloc(unit, pkt_size, flags, buf));
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free RX packet buffer. Generally, packet buffer was allocated with bcm_rx_alloc.
 *
 * \param [in] unit - Relevant unit
 * \param [in] pkt_data - packet to free
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   packets with BCM_RX_HANDLED_OWNED should use this to free packets
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_free(
    int unit,
    void *pkt_data)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_free(unit, pkt_data));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    /** Warmboot skipped, because of a usage between pkt_send and pkt_verify.*/
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/**
 *  \brief Stop RX for the given unit; saves current configuration.
 *
 *  \param [in] unit - Relevant unit
 *  \param [in] cfg - Pointer to RX configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_stop(
    int unit,
    bcm_rx_cfg_t * cfg)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_stop(unit, cfg));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Initialize the user level configuration.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   Can't use if currently running.  Should be called before
 *   doing a simple modification of the RX configuration in case
 *   the previous user has left it in a strange state.
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_cfg_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_cfg_init(unit));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Register an upper layer driver
 *
 * \param [in] unit - Relevant unit
 * \param [in] name - Constant character string for debug purposes
 * \param [in] cb_f - Callback function when packet arrives
 * \param [in] pri - Priority of handler in list (0:lowest priority)
 * \param [in] cookie - Cookie passed to driver when packet arrives
 * \param [in] flags - Flags to indicate interrup or non-interrupt callback and which COS should this callback be run
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_register(
    int unit,
    const char *name,
    bcm_rx_cb_f cb_f,
    uint8 pri,
    void *cookie,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_register(unit, name, cb_f, pri, cookie, flags));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Unregister a callback from the Rx handlers.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit
 *   \param [in] cb_f - Callback to unregister
 *   \param [in] pri - Priority of the callback
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_unregister(
    int unit,
    bcm_rx_cb_f cb_f,
    uint8 pri)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_unregister(unit, cb_f, pri));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Assign a RX channel to a cosq
 *
 * \param [in] unit - Relevant unit
 * \param [in] queue_id - CPU cos queue index (0 - (max cosq - 1)), Negative for all
 * \param [in] chan_id - channel index (0 - (BCM_RX_CHANNELS-1)), -1 for no channel
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_channel_set(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t chan_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_channel_set(unit, queue_id, chan_id));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Get the associated rx channel with a given cosq
 *
 * \param [in] unit - Relevant unit
 * \param [in] queue_id - CPU cos queue index (0 - (max cosq - 1))
 * \param [out] chan_id - channel index (0 - (BCM_RX_CHANNELS-1))
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_channel_get(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t * chan_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_channel_get(unit, queue_id, chan_id));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Register an application callback for the specified CPU queue
 *
 * \param [in] unit - Relevant unit
 * \param [in] name - Constant character string for debug purposes
 * \param [in] cosq - CPU cos queue
 * \param [in] callback - Callback function pointer
 * \param [in] priority - Priority of handler in list (0:lowest priority)
 * \param [in] cookie - Cookie passed to driver when packet arrives
 * \param [in] flags - Flags to indicate interrup or non-interrupt callback
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_register(
    int unit,
    const char *name,
    bcm_cos_queue_t cosq,
    bcm_rx_cb_f callback,
    uint8 priority,
    void *cookie,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_register(unit, name, cosq, callback, priority, cookie, flags));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Unregister a callback.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit
 *   \param [in] cosq - CPU cos queue
 *   \param [in] callback - Registered callback function
 *   \param [in] priority - Priority of registered callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_unregister(
    int unit,
    bcm_cos_queue_t cosq,
    bcm_rx_cb_f callback,
    uint8 priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_unregister(unit, cosq, callback, priority));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert DNX system header type to char string.
 * \param [in] dnx_hdr_type - DNX header type
 * \return
 *   Name of DNX header type
 * \remark
 *   None
 * \see
 *   None
 */
static char *
dnx_rx_dump_header_type_to_string(
    bcm_pkt_dnx_type_t dnx_hdr_type)
{
    char *str = NULL;
    switch (dnx_hdr_type)
    {
        case bcmPktDnxTypeItmh:
            str = "itmh";
            break;
        case bcmPktDnxTypeFtmh:
            str = "ftmh";
            break;
        case bcmPktDnxTypeOtsh:
            str = "otsh";
            break;
        case bcmPktDnxTypeOtmh:
            str = "otmh";
            break;
        case bcmPktDnxTypeInternals:
            str = "internals";
            break;
        default:
            str = " Unknown";
    }
    return str;
}

/**
 * \brief - Convert FTMH action type to char string.
 * \param [in] action_type - FTMH action type
 * \return
 *   Name of FTMH action type
 * \remark
 *   None
 * \see
 *   None
 */
static char *
dnx_rx_dump_ftmh_action_type_to_string(
    bcm_pkt_dnx_ftmh_action_type_t action_type)
{
    char *str = NULL;
    switch (action_type)
    {
        case bcmPktDnxFtmhActionTypeForward:
            str = "Forward";
            break;
        case bcmPktDnxFtmhActionTypeSnoop:
            str = "Snoop";
            break;
        case bcmPktDnxFtmhActionTypeInboundMirror:
            str = "InboundMirror";
            break;
        case bcmPktDnxFtmhActionTypeOutboundMirror:
            str = "OutboundMirror";
            break;
        default:
            str = "Unknown";
    }
    return str;
}

/**
 * \brief - Dump system header of type ftmh.
 * \param [in] ftmh - FTMH header structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_ftmh_dump(
    bcm_pkt_dnx_ftmh_t * ftmh)
{
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("%s %u %s %u %s %u %s %u\n\r"),
                                 "packet_size:", ftmh->packet_size,
                                 "prio:", ftmh->prio, "src_sysport:", ftmh->src_sysport, "dst_port:", ftmh->dst_port));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("%s %u %s %s %s %u %s %u\n\r"),
                                 "ftmh_dp:", ftmh->ftmh_dp,
                                 "action_type:", dnx_rx_dump_ftmh_action_type_to_string(ftmh->action_type),
                                 "out_mirror_disable:", ftmh->out_mirror_disable,
                                 "is_mc_traffic:", ftmh->is_mc_traffic));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("%s %u %s 0x%x %s %u\n\r"),
                                 "multicast_id:", ftmh->multicast_id,
                                 "out_vport:", ftmh->out_vport, "cni:", ftmh->cni));
}

/**
 * \brief - Dump system header of type internal.
 * \param [in] internal - Internal header structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_internal_dump(
    bcm_pkt_dnx_internal_t * internal)
{
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("%s 0x%x %s 0x%x\n\r"),
                                 "qualifier:", internal->trap_qualifier, "code:", internal->trap_id));

    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
}

/**
 * \brief - Dump system header stack.
 * \param [in] pkt - Packet structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_system_header_stack(
    bcm_pkt_t * pkt)
{
    int i;

    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("dnx_header_count: %u\n\r"), pkt->dnx_header_count));

    for (i = 0; i < pkt->dnx_header_count; i++)
    {
        LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("===Header : %u======\n\r"), i));

        LOG_VERBOSE(BSL_LS_BCM_PKT,
                    (BSL_META("type: %s \n\r"), dnx_rx_dump_header_type_to_string(pkt->dnx_header_stack[i].type)));

        switch (pkt->dnx_header_stack[i].type)
        {
            case bcmPktDnxTypeFtmh:
                dnx_rx_dump_ftmh_dump((bcm_pkt_dnx_ftmh_t *) & (pkt->dnx_header_stack[i].ftmh));
                break;
            case bcmPktDnxTypeInternals:
                dnx_rx_dump_internal_dump(&(pkt->dnx_header_stack[i].internal));
                break;
            default:
                break;
        }
    }
    return;
}

/**
 * \brief - Dump raw packet data.
 * \param [in] unit - Device number.
 * \param [in] pkt - Packet structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_raw_packet_data(
    int unit,
    bcm_pkt_t * pkt)
{
    int i;
    char *packet_ptr;
    char *ch_ptr;
    uint32 buf_size = 0;

    if (bsl_check(bslLayerBcm, bslSourceRx, bslSeverityVerbose, unit))
    {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                    (BSL_META_U(unit,
                                "*************************************RX: (Egress) port %d, cos %d, tot len %d, reason %04x\n"),
                     BCM_GPORT_LOCAL_GET(pkt->dst_gport), pkt->cos, pkt->tot_len, pkt->rx_reason));

        packet_ptr = sal_alloc(3 * pkt->tot_len, "Packet print");
        buf_size = 3 * pkt->tot_len;
        if (packet_ptr != NULL)
        {
            ch_ptr = packet_ptr;
            /*
             * Dump the packet data block
             */
            for (i = 0; i < pkt->tot_len; i++)
            {
                if ((i % 4) == 0)
                {
                    sal_snprintf(ch_ptr, buf_size, " ");
                    ch_ptr++;
                    buf_size--;
                }
                if ((i % 32) == 0)
                {
                    sal_snprintf(ch_ptr, buf_size, "\n");
                    ch_ptr++;
                    buf_size--;
                }

                sal_snprintf(ch_ptr, buf_size, "%02x", (pkt->_pkt_data.data)[i]);
                ch_ptr += 2;
                buf_size -= 2;
            }
            sal_snprintf(ch_ptr, buf_size, "\n");
            LOG_VERBOSE(BSL_LS_BCM_RX, (BSL_META_U(unit, "%s"), packet_ptr));
            sal_free(packet_ptr);
        }
    }
}

/**
* \brief
*  Set bcm_pkt_t according the information from header parser.
* \param [in] unit     -  Unit id
* \param [in] packet_info  -  Information from header parser
* \param [in,out] pkt   -  Packet structure
* \return
*   None
* \remark
*   None
* \see
*   None
*/
static shr_error_e
dnx_rx_parser_packet_info_to_bcm_pkt(
    int unit,
    dnx_rx_parse_headers_t * packet_info,
    bcm_pkt_t * pkt)
{
    int index;
    bcm_rx_trap_t trap_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");
    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");

    for (index = 0; index < packet_info->dnx_header_count; index++)
    {
        sal_memcpy(&(pkt->dnx_header_stack[index]), &(packet_info->dnx_header_stack[index]), sizeof(bcm_pkt_dnx_t));
    }

    pkt->dnx_header_count = packet_info->dnx_header_count;

    pkt->pkt_len = pkt->tot_len - packet_info->header_length;

    if (rx_ctl[unit]->cpu_port_priorities != 64)
    {
        pkt->cos = packet_info->tc;
    }

    if (packet_info->is_trapped == TRUE)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, packet_info->cpu_trap_code, &trap_type));
        pkt->rx_reason = trap_type;
        if (trap_type == bcmRxTrapUserDefine)
        {
            pkt->rx_trap_data = packet_info->cpu_trap_code;
        }
        else
        {
            pkt->rx_trap_data = packet_info->cpu_trap_qualifier;
        }
    }
    else
    {
        pkt->rx_reason = 0;
        /** If packet is snooped, get cpu trap qualifier */
        if (packet_info->frwrd_type == bcmPktDnxFtmhActionTypeSnoop)
        {
            pkt->rx_trap_data = packet_info->cpu_trap_qualifier;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Convert the bytes stream in network byte order to the type of array of uint32
 *
 * \param [in] unit  - Device number
 * \param [in] bytes_stream  - Pointer to the bytes stream in network byte order
 * \param [in] btyes_offset - The first byte to convert (the next one is 'btye+1')
 * \param [in] nof_bytes - Number of bytes to get from bytes stream and to array of uint32
 * \param [out] val - Pointer to array of uint32
 * \return
 *   None
 * \remark
 *   Example:
 *     bytes_stream: 0x00112233445566778899aabbccddeeff
 *     btyes_offset: 10
 *     nof_bytes:    5
 *
 *     outputs:
 *      var[0] = 0xbbccddee
 *      var[1] = 0xaa
 * \see
 *   None
 */
static shr_error_e
dnx_rx_byte_string_to_uint32(
    int unit,
    uint8 *bytes_stream,
    int btyes_offset,
    int nof_bytes,
    uint32 *val)
{
    int i;
    int j;
    int nof_words;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bytes_stream, _SHR_E_PARAM, "bytes_stream");
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    i = nof_bytes;
    j = 0;
    nof_words = BYTES2WORDS(nof_bytes);

    /** Move to start byte position */
    bytes_stream += btyes_offset;

    while (nof_words && (i >= 4))
    {
        *val++ =
            ((bytes_stream[i - 4] << 24) | (bytes_stream[i - 3] << 16) | (bytes_stream[i - 2] << 8) |
             bytes_stream[i - 1]);
        i -= 4;
        nof_words--;
    }
    *val = 0;
    while (i)
    {
        *val |= (bytes_stream[i - 1] << (j * 8));
        j++;
        i--;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Get UDH header length per type
 *
 * \param [in] unit  - Unit id
 * \param [in] udh_data_type - type of UDH data
 * \return
 *   Lenght of UDH data
 * \remark
 *   None
 * \see
 *   None
 */
static uint32
dnx_rx_udh_data_length_per_type(
    int unit,
    uint8 udh_data_type)
{
    uint32 udh_data_length = 0;
    switch (udh_data_type)
    {
        case 0:
            udh_data_length = dnx_data_field.udh.type_0_length_get(unit);
            break;
        case 1:
            udh_data_length = dnx_data_field.udh.type_1_length_get(unit);
            break;
        case 2:
            udh_data_length = dnx_data_field.udh.type_2_length_get(unit);
            break;
        case 3:
            udh_data_length = dnx_data_field.udh.type_3_length_get(unit);
            break;
        default:
            break;
    }

    return udh_data_length;
}

/**
* \brief
*  parser to system headers
* \param [in] unit     -  Unit id
* \param [in] pkt     -  pointer to packet data buffer
* \param [in] pkt_len     -  total length of packet from DMA
* \param [out] packet_info  -  information from header parser
* \return
*   \retval Non-zero in case of an error
*   \retval Zero in case of NO ERROR
* \remark
*   Flow of Ingress-trapped, the packet is  "Original-System-headeroOriginal-Netowrk-headersoPayload"
*   Flow of non Ingress-trapped. the packet is  "New-System-header oNew-Network-headeroPayload",
*   "New-System-header" is FTMH(80b), TSH(32b), PPH(96b), FHEI(40b).
* \see
*   None
*/
static shr_error_e
dnx_rx_system_header_parse(
    int unit,
    uint8 *pkt,
    uint32 pkt_len,
    dnx_rx_parse_headers_t * packet_info)
{
    int index;
    uint32 out_lif_ext_size[DNX_RX_PARSE_HEADER_OUT_LIF_EXT_TYPE];
    uint32 entry_handle_id;
    uint32 lb_key_ext_size;
    uint32 stacking_ext_size;
    uint32 bier_bfr_ext_size;
    uint32 inter_hdr_base_size;
    uint32 system_header[DNX_RX_PARSE_HEADER_MAX_LEN];
    uint32 offset, nof_bits;
    uint32 fld_val;
    uint32 pkt_offset;
    uint32 pkt_offset_ingress_untrapped;
    bcm_pkt_dnx_ftmh_t *ftmh;
    bcm_pkt_dnx_internal_t *internal;
    bcm_pkt_dnx_t *curr_dnx_header;
    uint8 is_inter_hdr_en = FALSE;
    uint8 is_tsh_en = FALSE;
    uint8 udh_en = FALSE;
    uint32 fhei_type = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");
    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");

    /** Read LB_KEY SIZE, Stacking size ,internal header base size */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE, &lb_key_ext_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                               &stacking_ext_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_SIZE, INST_SINGLE, &inter_hdr_base_size);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_USER_HEADER_ENABLE, INST_SINGLE, &udh_en);
    /** Read 1xOutLIF, 2xOutLIF, 3xOutLIF extension header size */
    for (index = 0; index < (DNX_RX_PARSE_HEADER_OUT_LIF_EXT_TYPE - 1); index++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, index,
                                   &out_lif_ext_size[index]);
    }
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF_EXT_SIZE, INST_SINGLE, &out_lif_ext_size[index]);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Always start with FTMH, given the packet is trapped to CPU */
    packet_info->dnx_header_count = 0;
    curr_dnx_header = &(packet_info->dnx_header_stack[packet_info->dnx_header_count]);
    curr_dnx_header->type = bcmPktDnxTypeFtmh;
    ftmh = &curr_dnx_header->ftmh;
    packet_info->dnx_header_count++;

    pkt_offset = 0;
    fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, ftmh, base_header_size);
    SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, fld_val, system_header));
    /** Move to start byte of next header, also the total length of system headers */
    pkt_offset += fld_val;
    pkt_offset_ingress_untrapped = pkt_offset;

    /** FTMH.Packet-Size*/
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, packet_size, fld_val);
    ftmh->packet_size = fld_val;

    /** FTMH.Traffic-Class*/
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, traffic_class, fld_val);
    ftmh->prio = fld_val;
    packet_info->tc = ftmh->prio;

    /** FTMH.Source-System-Port-Aggregate */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, src_sys_port_aggregate, fld_val);
    ftmh->src_sysport = fld_val;

    /** FTMH.PP-DSP */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, pp_dsp, fld_val);
    ftmh->dst_port = fld_val;

    /** FTMH.Drop-Precedence */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, drop_precedence, fld_val);
    ftmh->ftmh_dp = fld_val;
    packet_info->dp = fld_val;

    /** FTMH.TM-Action-Type */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, tm_action_type, fld_val);
    switch (fld_val)
    {
        case 0:
            ftmh->action_type = bcmPktDnxFtmhActionTypeForward;
            packet_info->frwrd_type = bcmPktDnxFtmhActionTypeForward;
            break;
        case 1:
            ftmh->action_type = bcmPktDnxFtmhActionTypeSnoop;
            packet_info->frwrd_type = bcmPktDnxFtmhActionTypeSnoop;
            break;
        case 2:
            ftmh->action_type = bcmPktDnxFtmhActionTypeInboundMirror;
            packet_info->frwrd_type = bcmPktDnxFtmhActionTypeInboundMirror;
            break;
        case 3:
            ftmh->action_type = bcmPktDnxFtmhActionTypeOutboundMirror;
            packet_info->frwrd_type = bcmPktDnxFtmhActionTypeOutboundMirror;
            break;
        default:
            break;
    }

    /** FTMH.TM-Action-Is-MC */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, tm_action_is_mc, fld_val);
    ftmh->is_mc_traffic = fld_val;

    /** FTMH.Multicast-ID-or-MC-REP-IDX-or-OutLIF */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, outlif, fld_val);
    if (ftmh->is_mc_traffic == TRUE)
    {
        ftmh->multicast_id = fld_val;
    }
    else
    {
        if (fld_val == 0)
        {
            /** an OutLIF value of '0' is treated at the egress as no encapsulation (i.e., packet is destined to a port level interface) */
            ftmh->out_vport = 0;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                            (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, _SHR_CORE_ALL, fld_val,
                             &ftmh->out_vport));
        }

    }

    /** FTMH.CNI */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, cni, fld_val);
    ftmh->cni = fld_val;

    /** FTMH.ECN-Enable -- ignore */
    /** FTMH.TM profile -- ignore */
    /** FTMH.Visibility -- ignore */

    /** FTMH.PPH-Type */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, internal_header_en, fld_val);
    is_inter_hdr_en = fld_val;
    /** TS existence is indicated by bit 6 of FTMH */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, tsh_en, fld_val);
    is_tsh_en = fld_val;

    /** FTMH.TM-Destination-Extension-Present */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, tm_dest_ext_present, fld_val);
    ftmh->dest_ext.valid = fld_val;

    /** FTMH.Application-Specific-Extension-Size */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, ase_present, fld_val);
    ftmh->ase_ext.valid = fld_val;

    /** FTMH.Flow-ID-Extension-Size */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, flow_id_ext_present, fld_val);
    ftmh->flow_id_ext.valid = fld_val;

    /** FTMH.BIER-BFR-Extension-Size */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, bier_bfr_ext_present, fld_val);
    if (fld_val)
    {
        bier_bfr_ext_size = 2;
    }
    else
    {
        bier_bfr_ext_size = 0;
    }

    /** FTMH LB-Key Extension */
    if (lb_key_ext_size > 0)
    {
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, lb_key_ext_size, system_header));
        /** Move to start byte of next header */
        pkt_offset += lb_key_ext_size;

        nof_bits = DNX_RX_PARSE_HEADER_BITS_GET(lb_key_ext_size);
        /** Start bit of FTMH LB-Key Extension */
        offset = 0;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &ftmh->lb_ext.lb_key));
    }

    /** FTMH Stacking Extension */
    if (stacking_ext_size > 0)
    {
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, stacking_ext_size, system_header));
        /** Move to start byte of next header */
        pkt_offset += stacking_ext_size;
        /** Move backward to start position Stacking-Route-History-Bitmap in FTMH Stacking Extension Header */
        offset = DNX_RX_PARSE_HEADER_DATA_GET(unit, ftmh, stack_route_history_bmp_offset);

        nof_bits = DNX_RX_PARSE_HEADER_BITS_GET(stacking_ext_size);
        nof_bits -= offset;

        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (system_header, offset, nof_bits, &ftmh->stack_ext.stack_route_history_bmp));
    }

    /** FTMH BIER BFR Extension -- bypass */
    if (bier_bfr_ext_size > 0)
    {
        /** Move to start byte of next header */
        pkt_offset += bier_bfr_ext_size;
    }

    /** FTMH TM Destination Extension */
    if (ftmh->dest_ext.valid)
    {
        fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, ftmh, tm_dst_size);
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, fld_val, system_header));
        /** Move to start byte of next header */
        pkt_offset += fld_val;

        nof_bits = DNX_RX_PARSE_HEADER_BITS_GET(fld_val);

        DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, tm_dst, fld_val);
        SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, fld_val, &(ftmh->dest_ext.dst_sysport)));
    }

    /** FTMH Application Specific Extension Header -- bypass */
    if ((ftmh->ase_ext.valid))
    {
        fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, ftmh, ase_header_size);
        /** Move to start byte of next header */
        pkt_offset += fld_val;
    }

    /*
     * FTMH Flow-ID Extension
     */
    if (ftmh->flow_id_ext.valid == TRUE)
    {
        fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, ftmh, flow_id_header_size);
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, fld_val, system_header));
        /** Move to start byte of next header */
        pkt_offset += fld_val;
        DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, flow_id, fld_val);
        ftmh->flow_id_ext.flow_id = fld_val;
        DNX_RX_PARSE_HEADER_FIELD_GET(unit, ftmh, flow_profile, fld_val);
        ftmh->flow_id_ext.flow_profile = fld_val;
    }

    /** Time-Stamp Header -- bypass */
    if (is_tsh_en == TRUE)
    {
        fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, tsh, base_header_size);
        /** Move to start byte of next header */
        pkt_offset += fld_val;
    }

    /** Packet Processing Header */
    if (is_inter_hdr_en == TRUE)
    {
        uint32 fhei_size_type;
        uint32 lif_ext_type;
        uint32 lif_ext_size;
        uint8 learning_ext_present;

        /** Increase count in header stack */
        curr_dnx_header = &(packet_info->dnx_header_stack[packet_info->dnx_header_count]);
        curr_dnx_header->type = bcmPktDnxTypeInternals;
        internal = &curr_dnx_header->internal;
        packet_info->dnx_header_count++;

        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, inter_hdr_base_size, system_header));
        /** Move to start byte of next header */
        pkt_offset += inter_hdr_base_size;

        DNX_RX_PARSE_HEADER_FIELD_GET(unit, internal, fhei_size, fhei_size_type);
        DNX_RX_PARSE_HEADER_FIELD_GET(unit, internal, lif_ext_type, lif_ext_type);
        DNX_RX_PARSE_HEADER_FIELD_GET(unit, internal, learning_ext_present, fld_val);
        learning_ext_present = fld_val;

        /** FHEI Extension*/
        if (fhei_size_type > 0)
        {
            switch (fhei_size_type)
            {
                case DNX_RX_PARSE_HEADER_PPH_FHEI_SIZE_SZ0:
                    /** Bypass */
                    fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, internal, fhei_size_sz0);
                    pkt_offset += fld_val;
                    break;
                case DNX_RX_PARSE_HEADER_PPH_FHEI_SIZE_SZ1:
                    /** FHEI-Size == 5B, FHEI-Type == Trap/Sniff */
                    fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, internal, fhei_size_sz1);
                    SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, fld_val, system_header));
                    /** Move to start byte of next header */
                    pkt_offset += fld_val;
                    DNX_RX_PARSE_HEADER_FIELD_GET(unit, fhei_sz1_trap_sniff, fhei_type, fhei_type);
                    /** FHEI.Type 4'b0101: Trap/Snoop/Mirror */
                    if (fhei_type == 0x5)
                    {
                        if ((ftmh->action_type != bcmPktDnxFtmhActionTypeSnoop)
                            && (ftmh->action_type != bcmPktDnxFtmhActionTypeInboundMirror))
                        {
                            packet_info->is_trapped = TRUE;
                        }
                        DNX_RX_PARSE_HEADER_FIELD_GET(unit, fhei_sz1_trap_sniff, qualifier, fld_val);
                        internal->trap_qualifier = fld_val;
                        packet_info->cpu_trap_qualifier = fld_val;
                        DNX_RX_PARSE_HEADER_FIELD_GET(unit, fhei_sz1_trap_sniff, code, fld_val);
                        internal->trap_id = fld_val;
                        packet_info->cpu_trap_code = fld_val;
                    }
                    break;
                case DNX_RX_PARSE_HEADER_PPH_FHEI_SIZE_SZ2:
                    /** Bypass */
                    fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, internal, fhei_size_sz2);
                    pkt_offset += fld_val;
                    break;
            }
        }

        /** PPH.LIF Extension -- bypass*/
        if ((lif_ext_type > 0) && (lif_ext_type <= DNX_RX_PARSE_HEADER_OUT_LIF_EXT_TYPE))
        {
            lif_ext_size = out_lif_ext_size[lif_ext_type-1];
            pkt_offset += lif_ext_size;
        }

        /** PPH.Learn Extension -- bypass*/
        if (learning_ext_present == TRUE)
        {
            fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, internal, learning_ext_size);
            pkt_offset += fld_val;
        }
    }
    /** UDH Header */
    if (udh_en)
    {
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32
                        (unit, pkt, pkt_offset, DNX_DATA_MAX_HEADERS_UDH_BASE_SIZE, system_header));
        pkt_offset += DNX_DATA_MAX_HEADERS_UDH_BASE_SIZE;

        DNX_RX_PARSE_HEADER_FIELD_GET(unit, udh, data_type0, fld_val);
        pkt_offset += dnx_rx_udh_data_length_per_type(unit, fld_val);

        DNX_RX_PARSE_HEADER_FIELD_GET(unit, udh, data_type1, fld_val);
        pkt_offset += dnx_rx_udh_data_length_per_type(unit, fld_val);

        DNX_RX_PARSE_HEADER_FIELD_GET(unit, udh, data_type2, fld_val);
        pkt_offset += dnx_rx_udh_data_length_per_type(unit, fld_val);

        DNX_RX_PARSE_HEADER_FIELD_GET(unit, udh, data_type3, fld_val);
        pkt_offset += dnx_rx_udh_data_length_per_type(unit, fld_val);
    }

    packet_info->header_length = pkt_offset;

    /** Check if fhei_type if of type trap */
    if (fhei_type == 0x5)
    {
        /** Done for ingress trapped packets */
        goto exit;
    }
    else
    {
        /** New generated header will be FTMH(80b), TSH(32b), PPH(96b), FHEI(40b) */

        /** Revert packet_info except info from FTHM base header */
        while (packet_info->dnx_header_count > 1)
        {
            curr_dnx_header = &(packet_info->dnx_header_stack[(packet_info->dnx_header_count - 1)]);
            sal_memset(curr_dnx_header, 0, sizeof(bcm_pkt_dnx_t));
            packet_info->dnx_header_count--;
        }
        packet_info->header_length = 0;
        packet_info->is_trapped = 0;
        packet_info->cpu_trap_code = 0;
        packet_info->cpu_trap_code = 0;
        packet_info->tc = 0;
        packet_info->dp = 0;

        /*
         * Time-Stamp Header -- bypass
         */
        fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, tsh, base_header_size);
        /** Move to start byte of next header */
        pkt_offset_ingress_untrapped += fld_val;

        /** Packet Processing Header */
        if (is_inter_hdr_en == TRUE)
        {
            uint32 fhei_size_type;

            /** Increase count in header stack */
            curr_dnx_header = &(packet_info->dnx_header_stack[packet_info->dnx_header_count]);
            curr_dnx_header->type = bcmPktDnxTypeInternals;
            internal = &curr_dnx_header->internal;
            packet_info->dnx_header_count++;

            SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32
                            (unit, pkt, pkt_offset_ingress_untrapped, inter_hdr_base_size, system_header));
            /** Move to start byte of next header */
            pkt_offset_ingress_untrapped += inter_hdr_base_size;

            DNX_RX_PARSE_HEADER_FIELD_GET(unit, internal, fhei_size, fhei_size_type);

            /*
             * FHEI(Trap,40)
             * 5B-FHEI format for sys_hdr_generation_profile:J2-OAM
             *  8b' 0
             *  19b'oam_id
             *  9b' cpu_trap_code: INGRESS_TRAP_ID is always 0 here
             *  4b' type(0x5): J2-Configuration FHEI Type for CPU trap
             */
            packet_info->is_trapped = FALSE;
            internal->trap_qualifier = internal->trap_id = 0;
            packet_info->cpu_trap_qualifier = packet_info->cpu_trap_code = 0;
            /** Bypass */
            fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, internal, fhei_size_sz1);
            pkt_offset_ingress_untrapped += fld_val;

            packet_info->header_length = pkt_offset_ingress_untrapped;
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_rx_packet_parse(
    int unit,
    bcm_pkt_t * pkt)
{
    dnx_rx_parse_headers_t *packet_info;
    bcm_switch_control_info_t value;
    bcm_switch_control_key_t key;
    int header_type_out;
    int system_headers_mode;
    uint16 vlan_control_tag = 0;
    uint16 tpid = 0;
    uint8 *eth_pkt_header = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");

    dnx_rx_dump_raw_packet_data(unit, pkt);

    packet_info = (dnx_rx_parse_headers_t *) sal_alloc(sizeof(dnx_rx_parse_headers_t), "packet_info");
    if (packet_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "RX Parser: failed to allocate packetInfo\n");
    }
    sal_memset(packet_info, 0, sizeof(dnx_rx_parse_headers_t));

    value.value = 0;
    key.type = bcmSwitchPortHeaderType;
    key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(unit, pkt->dst_gport, key, &value));
    header_type_out = value.value;

    if ((header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_CPU) || (header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU))
    {
        system_headers_mode = DNX_RX_PARSE_HEADER_DATA_GET(unit, system_headers, system_headers_mode);
        if (system_headers_mode == DNX_RX_PARSE_HEADER_DATA_GET(unit, system_headers, system_headers_mode_jericho2))
        {
            /** JR2 system header parser */
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse(unit, pkt->_pkt_data.data, pkt->tot_len, packet_info));
            /** Update bcm_packet according to information from system headers  */
            SHR_IF_ERR_EXIT(dnx_rx_parser_packet_info_to_bcm_pkt(unit, packet_info, pkt));
        }
    }

    /** Update priority and vlan id based on outer vlan tag of pkt */
    eth_pkt_header = pkt->_pkt_data.data + packet_info->header_length;

    /*
     * 0x88a8 S-Tag in IEEE 802.1ad
     * 0x9100 S-Tag in old non-standard 802.1QinQ protocol
     * 0x8100 C-Tag in IEEE 802.1Q
     */
    pkt->rx_untagged = (BCM_PKT_OUTER_UNTAGGED | BCM_PKT_INNER_UNTAGGED);
    tpid = ((uint16) ((eth_pkt_header[12] << 8) | eth_pkt_header[13]));
    if ((tpid == 0x88a8) || (tpid == 0x9100))
    {
        pkt->rx_untagged &= ~BCM_PKT_OUTER_UNTAGGED;
        pkt->rx_untagged &= ~BCM_PKT_INNER_UNTAGGED;
    }
    if (tpid == 0x8100)
    {
        pkt->rx_untagged &= ~BCM_PKT_OUTER_UNTAGGED;
    }

    vlan_control_tag = ((uint16) ((eth_pkt_header[14] << 8) | eth_pkt_header[15]));
    pkt->prio_int = ((vlan_control_tag >> 13) & 0x007);
    pkt->vlan = ((vlan_control_tag >> 0) & 0xfff);

    dnx_rx_dump_system_header_stack(pkt);

    if (packet_info != NULL)
        sal_free(packet_info);

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/*
 * Adapter functions for RX
 */
#ifdef ADAPTER_SERVER_MODE

void
adapter_convert_rx_data_from_chars(
    int unit,
    int data_length_in_bits,
    unsigned char *data_chars,
    uint8 *data_binary)
{
    int convert_position = 0;
    int result_index = 0;
    char zero_char = '0';

    while (convert_position + 8 <= data_length_in_bits)
    {
        data_binary[result_index] = (((uint8) (data_chars[convert_position]) - (uint8) (zero_char)) << 7 |
                                     ((uint8) (data_chars[convert_position + 1]) - (uint8) (zero_char)) << 6 |
                                     ((uint8) (data_chars[convert_position + 2]) - (uint8) (zero_char)) << 5 |
                                     ((uint8) (data_chars[convert_position + 3]) - (uint8) (zero_char)) << 4 |
                                     ((uint8) (data_chars[convert_position + 4]) - (uint8) (zero_char)) << 3 |
                                     ((uint8) (data_chars[convert_position + 5]) - (uint8) (zero_char)) << 2 |
                                     ((uint8) (data_chars[convert_position + 6]) - (uint8) (zero_char)) << 1 |
                                     ((uint8) (data_chars[convert_position + 7]) - (uint8) (zero_char)));
        convert_position += 8;
        result_index++;
    }
}

/*
 * Look for a generated header signal. Return a pointer to the signal's data or NULL if it doesn't exist.
 */
void
adapter_find_generated_header(
    int unit,
    uint32 nof_signals,
    uint32 pkt_length,
    unsigned char *pkt_data,
    unsigned char **generated_header_signal,
    uint32 *generated_header_length)
{
    int signal_index;
    unsigned char *pkt_data_current_signal;
    uint32 signal_id;
    uint32 signal_length;

    pkt_data_current_signal = pkt_data;
    *generated_header_signal = NULL;

    /** If only one signal came on the packet use it as the data */
    if (nof_signals == 1)
    {
        *generated_header_length = ntohl(*(uint32 *) &(pkt_data_current_signal[4]));
        *generated_header_signal = pkt_data + 2 * sizeof(uint32);
    }
    else
    {
        /** Run over the signals and look for the generated header signal */
        for (signal_index = 0; signal_index < nof_signals; signal_index++)
        {
            signal_id = ntohl(*(uint32 *) pkt_data_current_signal);
            signal_length = ntohl(*(int *) &(pkt_data_current_signal[4]));

            /** Length validation check */
            if (signal_length + (uint32) (pkt_data_current_signal - pkt_data) > pkt_length)
            {
                /** Illegal packet format */
                break;
            }

            if (signal_id == dnx_data_adapter.rx.packet_header_signal_id_get(unit))
            {
                /** Update the generated header signal */
                *generated_header_length = signal_length;
                *generated_header_signal = &(pkt_data_current_signal[8]);

                break;
            }
            /** Point to the next signal. Add the signal's length + length field + signal id field  */
            pkt_data_current_signal += signal_length + 2 * sizeof(uint32);
        }
    }
}

/*
 * Build the pkt struct
 * buf format - [signal id (4 bytes) | signal data length (4 bytes) NW order | signal data (each bit is a char)]*
 */
void
adapter_build_rx_packet(
    int unit,
    adapter_ms_id_e ms_id,
    uint32 nof_signals,
    uint32 src_port,
    int len,
    unsigned char *buf,
    bcm_pkt_t * pkt)
{
    unsigned char *generated_header_signal;
    uint32 generated_header_length;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    adapter_find_generated_header(unit, nof_signals, len, buf, &generated_header_signal, &generated_header_length);

    if (generated_header_signal != NULL)
    {
        /*
         * Currently there are no signals, only the packet's data. Take the data after the signal ID and data size
         */
        adapter_convert_rx_data_from_chars(unit, generated_header_length, generated_header_signal, pkt->pkt_data->data);
        pkt->pkt_data->len = (generated_header_length >> 3);    /* Convert from bit to bytes. The first 8 bytes are the
                                                                 * signal ID and * data length */
        pkt->_pkt_data.len = pkt->pkt_data->len;
        pkt->pkt_len = pkt->pkt_data->len;
        pkt->tot_len = pkt->pkt_data->len;

        pkt->blk_count = 1;     /* Number of blocks in data array. */
        pkt->unit = unit;       /* Unit number. */
        /*
         * pkt->cos;
         *//*
         * The local COS queue to use.
         */
        /*
         * pkt->color;
         *//*
         * Packet color.
         */
        pkt->src_port = (int16) src_port;       /* Source port used in header/tag. */
        /*
         * dest_port;
         *//*
         * Destination port used in header/tag.
         */
        pkt->opcode = BCM_HG_OPCODE_CPU;        /* BCM_HG_OPCODE_xxx. */

        /*
         * bcm_gport_t dst_gport;
         *//*
         * Destination virtual port
         */
        /*
         * bcm_gport_t src_gport;
         *//*
         * Source virtual port
         */
        /*
         * bcm_multicast_t multicast_group;
         *//*
         * Destination multicast group.
         */
        SHR_EXIT();
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "No generated header was found in the packet!");
    }

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_VOID_FUNC_EXIT;
}

static void
adapter_rx_pkt_thread(
    void *param)
{
    int unit = 0;
    unsigned char buf[MAX_PACKET_SIZE_ADAPTER] = { 0 };
    /** data_buf[MAX_PACKETS_IN_PARALLEL][MAX_PACKET_SIZE_ADAPTER] */
    uint8 *data_buf;
    int len = 0;
    int rv = _SHR_E_NONE;
    uint32 src_port = 0;
    adapter_ms_id_e ms_id = 0;
    uint32 nof_signals = 0;
    bcm_pkt_t *pkt;
    uint32 next_packet_index;
    uint32 ii;
    bcm_pkt_blk_t pkt_data[MAX_PACKETS_IN_PARALLEL];

    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, TRUE));
    dnx_err_recovery_utils_excluded_threads_add_all_units();

    sal_memset(pkt_data, 0, sizeof(pkt_data));
    /** Allocate a pool of MAX_PACKETS_IN_PARALLEL packets */
    data_buf = sal_alloc(MAX_PACKETS_IN_PARALLEL * MAX_PACKET_SIZE_ADAPTER, "Buffers for rx packets pool");
    assert(data_buf);
    sal_memset(data_buf, 0, (MAX_PACKETS_IN_PARALLEL * MAX_PACKET_SIZE_ADAPTER));

    pkt = sal_alloc(sizeof(bcm_pkt_t) * MAX_PACKETS_IN_PARALLEL, "Rx packets pool");
    assert(pkt);
    sal_memset(pkt, 0, (sizeof(bcm_pkt_t) * MAX_PACKETS_IN_PARALLEL));

    for (ii = 0; ii < MAX_PACKETS_IN_PARALLEL; ii++)
    {
        pkt[ii].pkt_data = &(pkt_data[ii]);
        pkt[ii].pkt_data->data = data_buf + ii * MAX_PACKET_SIZE_ADAPTER;
        pkt[ii]._pkt_data.data = pkt[ii].pkt_data->data;
    }

    next_packet_index = 0;

    INIT_SLEEP;
    while (rx_control.thread_running)
    {
        /*
         * Lock system rx start/stop mechanism.
         */
        _BCM_RX_SYSTEM_LOCK;

        /*
         * Wait until a packet is ready and read it
         */
        rv = adapter_read_buffer(unit, &ms_id, &nof_signals, &src_port, &len, &(buf[0]));

        /*
         * Packet was read from buffer
         */
        if (rv == _SHR_E_NONE)
        {
            /*
             * Build the packets struct
             */
            adapter_build_rx_packet(unit, ms_id, nof_signals, src_port, len, &(buf[0]), &(pkt[next_packet_index]));

            /*
             * Process the packet
             */
            rx_adapter_process_packet(unit, &(pkt[next_packet_index]));

            next_packet_index = (next_packet_index + 1) % MAX_PACKETS_IN_PARALLEL;
        }
        else if (rv == _SHR_E_DISABLED)
        {
        /** c model server disconnected. Close Rx thread (this one) in the SDK */
            rx_control.thread_running = FALSE;
        }
        /*
         * Unlock system rx start/stop mechanism.
         */
        _BCM_RX_SYSTEM_UNLOCK;

        _BCM_RX_CHECK_THREAD_DONE;

        SLEEP_MIN_SET(BASE_SLEEP_VAL);

        sal_sem_take(rx_control.pkt_notify, rx_control.sleep_cur);
        rx_control.pkt_notify_given = FALSE;

        INIT_SLEEP;
    }

    /*
     * Done using self-pipe, close fds
     */
    close(pipe_fds[0]);
    close(pipe_fds[1]);

    rx_control.thread_exit_complete = TRUE;

    sal_free(data_buf);
    sal_free(pkt);

    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, FALSE));
    dnx_err_recovery_utils_excluded_threads_remove_all_units();

    sal_thread_exit(0);
}

/*
 * Init the Rx thread that deals with packets to the CPU
 */
int
bcm_adapter_rx_start(
    int unit,
    bcm_rx_cfg_t * cfg)
{
    int priority = RX_THREAD_PRI_DFLT;
    int flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    /*
     * Timer/Event semaphore thread sleeping on.
     */
    if (NULL == rx_control.pkt_notify)
    {
        rx_control.pkt_notify = sal_sem_create("RX pkt ntfy", sal_sem_BINARY, 0);
        if (NULL == rx_control.pkt_notify)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "NULL == rx_control.pkt_notify");
        }
        rx_control.pkt_notify_given = FALSE;
    }

    /*
     * RX start/stop on one of the units protection mutex.
     */
    if (NULL == rx_control.system_lock)
    {
        rx_control.system_lock = sal_mutex_create("RX system lock");
        if (NULL == rx_control.system_lock)
        {
            sal_sem_destroy(rx_control.pkt_notify);
            SHR_ERR_EXIT(_SHR_E_MEMORY, "NULL == rx_control.system_lock");
        }
    }

    /*
     * Setup pipe for select exit notification. We use a "self-pipe" trick: - The write end is maintained by the main
     * thread - The read end is selected in the RX thread When we need to close the RX thread, we simply write to the
     * pipe, and we exit the blocking select call
     */
    if (pipe(pipe_fds) == -1)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "pipe(pipe_fds) == -1");
    }

    /*
     * Make read and write ends of pipe nonblocking: Get read end flags
     */
    flags = fcntl(pipe_fds[0], F_GETFL);
    if (flags == -1)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "fcntl(pipe_fds[0], F_GETFL)=-1");
    }

    /*
     * Make read end nonblocking
     */
    flags |= O_NONBLOCK;
    if (fcntl(pipe_fds[0], F_SETFL, flags) == -1)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "fcntl(pipe_fds[0], F_SETFL, flags) == -1");
    }

    /*
     * Get write end flags
     */
    flags = fcntl(pipe_fds[1], F_GETFL);
    if (flags == -1)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "fcntl(pipe_fds[1], F_GETFL)= -1");
    }

    /*
     * Make write end nonblocking
     */
    flags |= O_NONBLOCK;
    if (fcntl(pipe_fds[1], F_SETFL, flags) == -1)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "fcntl(pipe_fds[1], F_SETFL, flags) == -1");
    }

    /*
     * Start rx thread.
     */
    rx_control.thread_running = TRUE;
    rx_control.adapter_rx_tid =
        sal_thread_create("bcmAdapterRX", SAL_THREAD_STKSZ, priority, adapter_rx_pkt_thread, NULL);

    /*
     * Indicate RX is running
     */
    rx_ctl[unit]->flags |= BCM_RX_F_STARTED;

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/*
 * End of adapter functions
 */
#endif /* ADAPTER_SERVER_MODE */

/**
* \brief
*  Configure all that relevant for device initiation
* \par DIRECT INPUT:
*   \param [in] unit  -  Unit ID
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_rx_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_init(unit));

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   De-allocate all that relevant for device deinit
* \par DIRECT INPUT:
*   \param [in] unit  -  Unit ID
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_rx_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}

/**
* \brief
*  Create and id for given trap type, i.e allocate an id in relevant pool
* \par DIRECT INPUT:
*   \param [in] unit     -  Unit ID
*   \param [in] flags    -  Can be WITH_ID or 0.
*   \param [in] type     -  bcmRxTrapType.. to create
*   \param [out] trap_id  -  the allocated trap id
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_rx_trap_type_create(
    int unit,
    int flags,
    bcm_rx_trap_t type,
    int *trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Note that verify is called from within the internal function*/
    SHR_IF_ERR_EXIT(dnx_rx_trap_type_create(unit, flags, type, trap_id));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_type_get(
    int unit,
    int flags,
    bcm_rx_trap_t type,
    int *trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_type_get(unit, flags, type, trap_id));

exit:
    /** Warmboot skipped, because of a usage between pkt_send and pkt_verify.*/
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_type_from_id_get(
    int unit,
    int flags,
    int trap_id,
    bcm_rx_trap_t * trap_type)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_type_from_id_get(unit, flags, trap_id, trap_type));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the trap action profile: trap stregnth and action.
* \par DIRECT INPUT:
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  trap id to condifugre
*   \param [in] config   -  action profile configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_rx_trap_set(
    int unit,
    int trap_id,
    bcm_rx_trap_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Note that verify is called from within the internal function*/
    SHR_IF_ERR_EXIT(dnx_rx_trap_set(unit, trap_id, config));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the trap action profile: trap stregnth and action.
* \par DIRECT INPUT:
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  trap id to get its info, action profile
*   \param [out] config   -  action profile configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_rx_trap_get(
    int unit,
    int trap_id,
    bcm_rx_trap_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Note that verify is called from within the internal function*/
    SHR_IF_ERR_EXIT(dnx_rx_trap_get(unit, trap_id, config));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*
* \par DIRECT INPUT:
*   \param [in] unit     -
*   \param [in] trap_id  -
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_rx_trap_type_destroy(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Note that verify is called from within the internal function*/
    SHR_IF_ERR_EXIT(dnx_rx_trap_type_destroy(unit, trap_id));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_protocol_set(
    int unit,
    bcm_rx_trap_protocol_key_t * key_p,
    bcm_gport_t trap_gport)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_set(unit, key_p, trap_gport));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_protocol_get(
    int unit,
    bcm_rx_trap_protocol_key_t * key_p,
    bcm_gport_t * trap_gport_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_get(unit, key_p, trap_gport_p));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_protocol_clear(
    int unit,
    bcm_rx_trap_protocol_key_t * key_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_clear(unit, key_p));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_protocol_profiles_set(
    int unit,
    bcm_gport_t port,
    bcm_rx_trap_protocol_profiles_t * protocol_profiles_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_profiles_set(unit, port, protocol_profiles_p));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_protocol_profiles_get(
    int unit,
    bcm_gport_t port,
    bcm_rx_trap_protocol_profiles_t * protocol_profiles_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_protocol_profiles_get(unit, port, protocol_profiles_p));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_action_profile_set(
    int unit,
    uint32 flags,
    bcm_rx_trap_t trap_type,
    bcm_gport_t trap_gport)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_action_profile_set(unit, flags, trap_type, trap_gport));

exit:
    SHR_FUNC_EXIT;

}

int
bcm_dnx_rx_trap_action_profile_get(
    int unit,
    bcm_rx_trap_t trap_type,
    bcm_gport_t * trap_gport)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_action_profile_get(unit, trap_type, trap_gport));

exit:
    SHR_FUNC_EXIT;

}

int
bcm_dnx_rx_trap_action_profile_clear(
    int unit,
    bcm_rx_trap_t trap_type)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_action_profile_clear(unit, trap_type));

exit:
    SHR_FUNC_EXIT;

}

int
bcm_dnx_rx_trap_prog_set(
    int unit,
    int flags,
    uint8 prog_index,
    bcm_rx_trap_prog_config_t * prog_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_prog_set(unit, prog_index, prog_config));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_prog_get(
    int unit,
    uint8 prog_index,
    bcm_rx_trap_prog_config_t * prog_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_prog_get(unit, prog_index, prog_config));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_mtu_get(
    int unit,
    bcm_rx_mtu_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_mtu_get(unit, config));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_mtu_set(
    int unit,
    bcm_rx_mtu_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_mtu_set(unit, config));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_lif_set(
    int unit,
    uint32 flags,
    bcm_rx_trap_lif_config_t * lif_config_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_set(unit, flags, lif_config_p));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_trap_lif_get(
    int unit,
    bcm_rx_trap_lif_config_t * lif_config_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_get(unit, lif_config_p));

exit:
    SHR_FUNC_EXIT;
}
