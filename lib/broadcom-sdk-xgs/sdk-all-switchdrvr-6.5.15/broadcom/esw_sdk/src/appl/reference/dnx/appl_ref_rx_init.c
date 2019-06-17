/** \file appl_ref_rx_init.c
 * $Id$
 *
 * CPU RX procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_RX

/*
 * Includes
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_error.h>
#include <shared/bitop.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/common/rx.h>
#include <bcm/pkt.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/**
 * \brief - Example of RX callback function which is called in interrupt context
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in,out] pkt -  pkt
 *   \param [in] cookie -  cookie
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static bcm_rx_t
dnx_rx_intr_callback_example(
    int unit,
    bcm_pkt_t * pkt,
    void *cookie)
{
    return BCM_RX_NOT_HANDLED;
}

/**
 * \brief - Example of RX callback function which is called in rx thread
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in,out] pkt -  pkt
 *   \param [in] cookie -  cookie
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static bcm_rx_t
dnx_rx_nonintr_callback_example(
    int unit,
    bcm_pkt_t * pkt,
    void *cookie)
{
    return BCM_RX_NOT_HANDLED;
}

shr_error_e
dnx_rx_appl_active(
    int unit)
{
    bcm_rx_cfg_t cfg;
    uint32 cosq;
    int nof_cpu_ports;
    int nof_rx_channel;
    bcm_port_config_t port_config;
    bcm_gport_t logical_port;
    int channel_index = 1;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    if (bcm_rx_active(unit))
    {
        SHR_EXIT();
    }

#ifdef INCLUDE_KNET
    /*
     * Init KNET
     */
    SHR_IF_ERR_EXIT(bcm_knet_init(unit));
#endif

    /** Initialize Dune RX Configuration Structure */
    bcm_rx_cfg_t_init(&cfg);

    cfg.pkt_size = RX_PKT_SIZE_DFLT;
    cfg.global_pps = RX_PPS_DFLT;
    cfg.max_burst = 0;
    cfg.flags = 0;

    /** Scenario of multiple CMCs is not considered here */
    nof_rx_channel = BCM_CMICX_RX_CHANNELS - 1;
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
    BCM_PBMP_COUNT(port_config.cpu, nof_cpu_ports);

    if (nof_cpu_ports >= nof_rx_channel)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of CPU ports(%d) exceeds the number of RX channels(%d)\n",
                     nof_cpu_ports, nof_rx_channel);
    }

    /*
     * Update packets per chain
     * RX_CMICX_PPC_DFLT is for the case that there is only one rx channel
     * Resource might be not enough for multiple rx channels
     */
    if (nof_cpu_ports >= RX_CMICX_PPC_DFLT)
    {
        /** At least one packet per chain */
        cfg.pkts_per_chain = RX_CMICX_PPC_DFLT / RX_CMICX_PPC_DFLT;
    }
    else
    {
        /** Decide packet number per chain on number of CPU ports */
        cfg.pkts_per_chain = RX_CMICX_PPC_DFLT / nof_cpu_ports;
    }

    BCM_PBMP_ITER(port_config.cpu, logical_port)
    {
        int channel;
        int nof_priorities;
        uint32 dummy_flags;
        bcm_port_interface_info_t interface_info;
        bcm_port_mapping_info_t mapping_info;
        SHR_BITDCL channel_cos_bmp;

        /** Get number of priorities per port */
        SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &dummy_flags, &interface_info, &mapping_info));
        nof_priorities = mapping_info.num_priorities;
 
        SHR_BITCLR_RANGE(&channel_cos_bmp, 0, SHR_BITWID);

        cfg.chan_cfg[channel_index].chains = RX_CHAINS_DFLT;
        cfg.chan_cfg[channel_index].rate_pps = 1000;
        cfg.chan_cfg[channel_index].flags = 0;

        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &channel));

        for (cosq = 0; cosq < nof_priorities; cosq++)
        {
            if (channel + cosq < SHR_BITWID)
            {
                SHR_BITSET(&channel_cos_bmp, channel + cosq);
            }
            /** Port -> COSQ -> channel */
            SHR_IF_ERR_EXIT(bcm_rx_queue_channel_set(unit, (channel + cosq), channel_index));
        }
        cfg.chan_cfg[channel_index].cos_bmp = channel_cos_bmp;
        channel_index++;
    }

    SHR_IF_ERR_EXIT(bcm_rx_register
                    (unit, "EXAMPLE RX INTR CALLBACK", dnx_rx_intr_callback_example, 108, NULL,
                     (BCM_RCO_F_ALL_COS | BCM_RCO_F_INTR)));
    SHR_IF_ERR_EXIT(bcm_rx_register
                    (unit, "EXAMPLE RX NONINTR CALLBACK", dnx_rx_nonintr_callback_example, 109, NULL,
                     BCM_RCO_F_ALL_COS));

    SHR_IF_ERR_EXIT(bcm_rx_start(unit, &cfg));
exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}
