/** \file appl_ref_rx_init.c
 * $Id$
 *
 * CPU RX procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <shared/bitop.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/common/rx.h>
#include <bcm/pkt.h>
#include <bcm/knet.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

shr_error_e
appl_dnx_rx_init(
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
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

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

    /** Scenario of multiple CMCs is not considered here */
    nof_rx_channel = BCM_CMICX_RX_CHANNELS - 1;
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
    BCM_PBMP_COUNT(port_config.cpu, nof_cpu_ports);

    if (nof_cpu_ports >= nof_rx_channel)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of CPU ports(%d) exceeds the number of RX channels(%d)\n", nof_cpu_ports, nof_rx_channel);
    }
    else if (nof_cpu_ports == 0)
    {
        /*
         * don't start CPU rx if no cpu ports
         */
        SHR_EXIT();
    }

    /*
     * Initialize Dune RX Configuration Structure
     */
    bcm_rx_cfg_t_init(&cfg);

    cfg.pkt_size = RX_PKT_SIZE_DFLT;
    /*
     * Disable RX rate limitation
     */
    cfg.global_pps = 0;
    cfg.max_burst = 0;
    cfg.flags = 0;
    cfg.pkts_per_chain = RX_PPC_MAX;

    BCM_PBMP_ITER(port_config.cpu, logical_port)
    {
        int channel;
        int nof_priorities;
        uint32 dummy_flags;
        bcm_port_interface_info_t interface_info;
        bcm_port_mapping_info_t mapping_info;

        /** Get number of priorities per port */
        SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &dummy_flags, &interface_info, &mapping_info));
        nof_priorities = mapping_info.num_priorities;

        cfg.chan_cfg[channel_index].chains = 2;
        cfg.chan_cfg[channel_index].rate_pps = 0;
        cfg.chan_cfg[channel_index].flags = 0;

        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &channel));

        for (cosq = 0; cosq < nof_priorities; cosq++)
        {
            cfg.chan_cfg[channel_index].cos_bmp |= (1U << ((channel + cosq) % SHR_BITWID));
            /** Port -> COSQ -> channel */
            SHR_IF_ERR_EXIT(bcm_rx_queue_channel_set(unit, (channel + cosq), channel_index));
        }
        channel_index++;
    }

    SHR_IF_ERR_EXIT(bcm_rx_start(unit, &cfg));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
appl_dnx_rx_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    /*
     * Stop RX
     */
    SHR_IF_ERR_EXIT(bcm_rx_stop(unit, NULL));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}
