/*
 * $Id: jer2_arad_stat.c,v 1.14 Broadcom SDK $
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC JER2 STAT
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_STAT

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/legacy/drv.h>
#include <soc/dnxc/legacy/dnxc_port.h>

#include <soc/dnx/stat/jer2_stat.h>

#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>

#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/stat/mib/mib_stat_dbal.h>

/*
 * Local defines
 *
 */
#define DNX_STAT_PAD_SIZE_MIN    (64)
#define DNX_STAT_PAD_SIZE_MAX    (96)
#define DNX_STAT_PAD_SIZE_IS_IN_RANGE(pad_size) \
    (pad_size >= DNX_STAT_PAD_SIZE_MIN && pad_size <= DNX_STAT_PAD_SIZE_MAX)

static int
soc_jer2_fmac_controlled_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{

    int fmac_idx, lane_idx, link;
    uint64 mask;
    int length;
    int offset = 0;
    int phy_port;
    int ilkn_over_fabric_port = 0;
    int nof_links_in_mac;
    SHR_FUNC_INIT_VARS(unit);

    nof_links_in_mac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    if (clear_on_read)
    {
        *clear_on_read = TRUE;
    }
    if (IS_IL_PORT(unit, port))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &offset));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &ilkn_over_fabric_port));
    }

    if (IS_SFI_PORT(unit, port))
    {

        link = SOC_DNX_FABRIC_PORT_TO_LINK(unit, port);
        fmac_idx = link / nof_links_in_mac;
        lane_idx = link % nof_links_in_mac;
        SHR_IF_ERR_EXIT(dnx_fmac_stat_dbal_get(unit, fmac_idx, lane_idx, counter_id, val));

        SHR_IF_ERR_EXIT(soc_jer2_stat_counter_length_get(unit, counter_id, &length));
        COMPILER_64_MASK_CREATE(mask, length, 0);
        COMPILER_64_AND((*val), mask);

    }
    else if (ilkn_over_fabric_port)
    {
        /** for ilkn_over_fabric_port, only for the asyn_fifo_rate counters*/
        if (counter_id == soc_jer2_counters_tx_asyn_fifo_rate || counter_id == soc_jer2_counters_rx_asyn_fifo_rate)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &phy_port));
            link = phy_port - SOC_DNX_FIRST_FABRIC_PHY_PORT(unit);
            fmac_idx = link / nof_links_in_mac;
            lane_idx = link % nof_links_in_mac;
            SHR_IF_ERR_EXIT(dnx_fmac_stat_dbal_get(unit, fmac_idx, lane_idx, counter_id, val));

            SHR_IF_ERR_EXIT(soc_jer2_stat_counter_length_get(unit, counter_id, &length));
            COMPILER_64_MASK_CREATE(mask, length, 0);
            COMPILER_64_AND((*val), mask);

        }
        else
        {
            COMPILER_64_SET(*val, 0, 0);
        }

    }
    else
    {
        COMPILER_64_SET(*val, 0, 0);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
soc_jer2_nif_controlled_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{

    dnx_algo_port_type_e port_type;
    bcm_port_prio_config_t priority_config;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy;
    int phys_returned;
    uint32 val32 = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = 0;
    sal_memset(&priority_config, 0, sizeof(priority_config));

    if (clear_on_read)
    {
        *clear_on_read = TRUE;
    }
    if (!IS_SFI_PORT(unit, port))
    {

        COMPILER_64_ZERO(*val);
        /** get port type */
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
        switch (counter_id)
        {
            case soc_jer2_counters_rx_eth_stats_drop_events_sch_low:
                if ((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_type) == FALSE) &&
                    (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type) == FALSE))
                {
                    SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, bcmPortNifSchedulerLow, val));
                }
                break;
            case soc_jer2_counters_rx_eth_stats_drop_events_sch_high:
                if ((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_type) == FALSE) &&
                    (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type) == FALSE))
                {
                    SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, bcmPortNifSchedulerHigh, val));
                }
                break;
            case soc_jer2_counters_rx_eth_stats_drop_events_sch_tdm:
                if ((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_type) == FALSE) &&
                    (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type) == FALSE))
                {
                    SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, bcmPortNifSchedulerTDM, val));
                }
                break;
            case soc_jer2_counters_nif_rx_fec_correctable_error:
                if (!IS_IL_PORT(unit, port))
                {
                    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                                    (unit, port, &params, 1, &phy, &phys_returned, NULL));
                    SHR_IF_ERR_EXIT(phymod_phy_fec_cl91_correctable_counter_get(&phy, &val32));
                    COMPILER_64_SET(*val, 0, val32);
                }
                else
                {
                    SHR_IF_ERR_EXIT(imb_port_ilkn_counter_get
                                    (unit, port, soc_jer2_counters_nif_rx_fec_correctable_error, val));
                }
                break;
            case soc_jer2_counters_nif_rx_fec_uncorrrectable_errors:
                if (!IS_IL_PORT(unit, port))
                {
                    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                                    (unit, port, &params, 1, &phy, &phys_returned, NULL));
                    SHR_IF_ERR_EXIT(phymod_phy_fec_cl91_uncorrectable_counter_get(&phy, &val32));
                    COMPILER_64_SET(*val, 0, val32);
                }
                else
                {
                    SHR_IF_ERR_EXIT(imb_port_ilkn_counter_get
                                    (unit, port, soc_jer2_counters_nif_rx_fec_uncorrrectable_errors, val));
                }
                break;
            case soc_jer2_counters_nif_rx_bip_error:
                if (!IS_IL_PORT(unit, port))
                {
                    COMPILER_64_SET(*val, 0, 0);
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is not supported for port %d", counter_id, port);
        }

    }
    else
    {
        COMPILER_64_SET(*val, 0, 0);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
soc_jer2_ilkn_controlled_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{
    SHR_FUNC_INIT_VARS(unit);

    if (clear_on_read)
    {
        *clear_on_read = FALSE;
    }
    if (!IS_SFI_PORT(unit, port))
    {

        COMPILER_64_ZERO(*val);

        switch (counter_id)
        {
            case soc_jer2_counters_ilkn_rx_pkt_counter:
            case soc_jer2_counters_ilkn_tx_pkt_counter:
            case soc_jer2_counters_ilkn_rx_byte_counter:
            case soc_jer2_counters_ilkn_tx_byte_counter:
            case soc_jer2_counters_ilkn_rx_err_pkt_counter:
            case soc_jer2_counters_ilkn_tx_err_pkt_counter:
                if (IS_IL_PORT(unit, port))
                {
                    SHR_IF_ERR_EXIT(imb_port_ilkn_counter_get(unit, port, counter_id, val));
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is not supported for port %d", counter_id, port);
        }
    }
    else
    {
        COMPILER_64_SET(*val, 0, 0);
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_jer2_fmac_controlled_counter_clear(
    int unit,
    int counter_id,
    int port)
{
    uint64 data;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * FMAC counter is clear on read
     */
    SHR_IF_ERR_EXIT(soc_jer2_fmac_controlled_counter_get(unit, counter_id, port, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

int
soc_jer2_nif_controlled_counter_clear(
    int unit,
    int counter_id,
    int port)
{

    uint64 data;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * assume clear on read 
     */
    SHR_IF_ERR_EXIT(soc_jer2_nif_controlled_counter_get(unit, counter_id, port, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

int
soc_jer2_ilkn_controlled_counter_clear_by_port(
    int unit,
    int port)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IS_IL_PORT(unit, port))
    {
        SHR_IF_ERR_EXIT(imb_port_ilkn_counter_clear(unit, port));
    }
exit:
    SHR_FUNC_EXIT;
}

int
soc_jer2_ilkn_controlled_counter_clear(
    int unit,
    int counter_id,
    int port)
{

    SHR_FUNC_INIT_VARS(unit);
    /*
     * ignor counter_id 
     */
    SHR_IF_ERR_EXIT(soc_jer2_ilkn_controlled_counter_clear_by_port(unit, port));

exit:
    SHR_FUNC_EXIT;
}

int
soc_jer2_cdmib_controlled_counter_get(
    int unit,
    int counter_id,
    int port,
    uint64 *val,
    uint32 *clear_on_read)
{
    dbal_fields_e field_id;
    SHR_FUNC_INIT_VARS(unit);

    if (clear_on_read)
    {
        *clear_on_read = FALSE;
    }

    if ((!IS_SFI_PORT(unit, port)) && (!IS_IL_PORT(unit, port)))
    {

        COMPILER_64_ZERO(*val);
        SHR_IF_ERR_EXIT(dnx_cdmib_counter_id_to_dbal_field_id_get(unit, port, counter_id, &field_id));
        SHR_IF_ERR_EXIT(dnx_cdmib_stat_dbal_get(unit, port, field_id, val));

    }
    else
    {
        COMPILER_64_SET(*val, 0, 0);
    }

exit:
    SHR_FUNC_EXIT;

}

int
soc_jer2_cdmib_controlled_counter_clear(
    int unit,
    int counter_id,
    int port)
{
    uint64 val;
    dbal_fields_e field_id;
    SHR_FUNC_INIT_VARS(unit);

    if ((!IS_SFI_PORT(unit, port)) && (!IS_IL_PORT(unit, port)))
    {

        COMPILER_64_ZERO(val);
        SHR_IF_ERR_EXIT(dnx_cdmib_counter_id_to_dbal_field_id_get(unit, port, counter_id, &field_id));
        SHR_IF_ERR_EXIT(dnx_cdmib_stat_dbal_set(unit, port, field_id, val));
    }

exit:
    SHR_FUNC_EXIT;

}

soc_controlled_counter_t soc_jer2_controlled_counter[] = {
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_tx_control_cells_counter,
     "TX Control cells",
     "TX Control cells",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_tx_data_cell_counter,
     "TX Data cell",
     "TX Data cell",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_tx_data_byte_counter,
     "TX Data byte",
     "TX Data byte",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_crc_errors_data_cells,
     "RX CRC errors",
     "RX CRC errors",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_crc_errors_control_cells_nonbypass,
     "RX CRC errors nonbypass",
     "RX CRC errors nonbypass",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_crc_errors_control_cells_bypass,
     "RX CRC errors bypass",
     "RX CRC errors bypass",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_fec_correctable_error,
     "RX FEC correctable",
     "RX FEC correctable",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_control_cells_counter,
     "RX Control cells",
     "RX Control cells",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_data_cell_counter,
     "RX Data cell",
     "RX Data cell",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_data_byte_counter,
     "RX Data byte ",
     "RX Data byte",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_dropped_retransmitted_control,
     "RX Dropped retransmitted control",
     "RX Dropped retransmitted control",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_dummy_val_1,
     "Dummy value",
     "Dummy value",
     _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_dummy_val_2,
     "Dummy value",
     "Dummy value",
     _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_tx_asyn_fifo_rate,
     "TX Asyn fifo rate",
     "TX Asyn fifo rate",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_asyn_fifo_rate,
     "RX Asyn fifo rate",
     "RX Asyn fifo rate",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_lfec_fec_uncorrrectable_errors,
     "RX FEC uncorrectable errors",
     "RX FEC uncorrectable errors",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_llfc_primary_pipe,
     "RX Llfc primary pipe",
     "RX Llfc primary pipe",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_llfc_second_pipe,
     "RX Llfc second pipe",
     "RX Llfc second pipe",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_llfc_third_pipe,
     "RX Llfc third pipe",
     "RX Llfc third pipe",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_kpcs_errors_counter,
     "RX Kpcs errors",
     "RX Kpcs errors",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_kpcs_bypass_errors_counter,
     "RX Kpcs bypass errors",
     "RX Kpcs bypass errors",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_rs_fec_bit_error_counter,
     "RX RS-FEC bit error",
     "RX RS-FEC bit error",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_fmac_controlled_counter_get,
     soc_jer2_counters_rx_rs_fec_symbol_error_rate_counter,
     "RX RS-FEC symbol error rate",
     "RX RS-FEC symbol error rate",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_fmac_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_ilkn_controlled_counter_get,
     soc_jer2_counters_ilkn_rx_pkt_counter,
     "RX snmp stats no errors",
     "RX snmp stats no errors",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_ilkn_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_ilkn_controlled_counter_get,
     soc_jer2_counters_ilkn_tx_pkt_counter,
     "TX snmp stats no errors",
     "TX snmp stats no errors",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_ilkn_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_ilkn_controlled_counter_get,
     soc_jer2_counters_ilkn_rx_byte_counter,
     "RX ILKN byte counter",
     "RX ILKN byte counter",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_ilkn_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_ilkn_controlled_counter_get,
     soc_jer2_counters_ilkn_tx_byte_counter,
     "TX ILKN byte counter",
     "TX ILKN byte counter",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_ilkn_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_ilkn_controlled_counter_get,
     soc_jer2_counters_ilkn_rx_err_pkt_counter,
     "RX snmp If in err pkts",
     "RX snmp If in err pkts",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_ilkn_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_ilkn_controlled_counter_get,
     soc_jer2_counters_ilkn_tx_err_pkt_counter,
     "RX snmp If out err pkts",
     "RX snmp If out err pkts",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_ilkn_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_nif_controlled_counter_get,
     soc_jer2_counters_nif_rx_fec_correctable_error,
     "RX NIF FEC correctable",
     "RX NIF FEC correctable",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_NIF | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_nif_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_nif_controlled_counter_get,
     soc_jer2_counters_nif_rx_fec_uncorrrectable_errors,
     "RX NIF FEC uncorrectable",
     "RX NIF FEC uncorrectable",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_NIF | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_nif_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_nif_controlled_counter_get,
     soc_jer2_counters_nif_rx_bip_error,
     "RX NIF BIP error count",
     "RX NIF BIP error count",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_nif_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_nif_controlled_counter_get,
     soc_jer2_counters_rx_eth_stats_drop_events_sch_low,
     "RX nif drop low events",
     "RX nif drop low events",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_nif_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_nif_controlled_counter_get,
     soc_jer2_counters_rx_eth_stats_drop_events_sch_high,
     "RX nif drop high events",
     "RX nif drop high events",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_nif_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_nif_controlled_counter_get,
     soc_jer2_counters_rx_eth_stats_drop_events_sch_tdm,
     "RX nif drop tdm events",
     "RX nif drop tdm events",
     _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_nif_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_r64,
     "RX 64-byte frame",
     "RX 64-byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_r127,
     "RX 65 to 127 byte frame",
     "RX 65 to 127 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_r255,
     "RX 128 to 255 byte frame",
     "RX 128 to 255 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_r511,
     "RX 256 to 511 byte frame",
     "RX 256 to 511 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_r1023,
     "RX 512 to 1023 byte frame",
     "RX 512 to 1023 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_r1518,
     "RX 1024 to 1518 byte frame",
     "RX 1024 to 1518 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rmgv,
     "RX 1519 to 1522 byte frame",
     "RX 1519 to 1522 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_r2047,
     "RX 1519 to 2047 byte frame",
     "RX 1519 to 2047 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_r4095,
     "RX 2048 to 4095 byte frame",
     "RX 2048 to 4095 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_r9216,
     "RX 4096 to 9216 byte frame",
     "RX 4096 to 9216 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_r16383,
     "RX 9217 to 16383 byte frame",
     "RX 9217 to 16383 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rbca,
     "RX BC frame",
     "RX BC frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rprog0,
     "RX RPROG0 frame",
     "RX RPROG0 frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rprog1,
     "RX RPROG1 frame",
     "RX RPROG1 frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rprog2,
     "RX RPROG2 frame",
     "RX RPROG2 frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rprog3,
     "RX RPROG3 frame",
     "RX RPROG3 frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpkt,
     "RX all packets frame",
     "RX all packets frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpok,
     "RX good frame",
     "RX good frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_ruca,
     "RX UC frame",
     "RX UC frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_reserved0,
     "RESERVED0",
     "RESERVED0",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rmca,
     "RX MC frame",
     "RX MC frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rxpf,
     "RX pause frame",
     "RX pause frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rxpp,
     "RX PFC frame",
     "RX PFC frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rxcf,
     "RX control frame",
     "RX control frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rfcs,
     "RX FCS error frame",
     "RX FCS error frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rerpkt,
     "RX code error frame",
     "RX code error frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rflr,
     "RX out-of-range length frame",
     "RX out-of-range length frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rjbr,
     "RX jabber frame",
     "RX jabber frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rmtue,
     "RX MTU check error frame",
     "RX MTU check error frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rovr,
     "RX oversized frame",
     "RX oversized frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rvln,
     "RX single and double VLAN tagged frame",
     "RX single and double VLAN tagged frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rdvln,
     "RX double VLAN tagged frame",
     "RX double VLAN tagged frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rxuo,
     "RX unsupported opcode frame",
     "RX unsupported opcode frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rxuda,
     "RX unsupported DA for pause/PFC frame",
     "RX unsupported DA for pause/PFC frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rxwsa,
     "RX incorrect SA frame",
     "RX incorrect SA frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rprm,
     "RX promiscuous frame",
     "RX promiscuous frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfc0,
     "RX PFC frame with enable bit set for Priority0",
     "RX PFC frame with enable bit set for Priority0",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfcoff0,
     "RX PFC frame transition XON to XOFF for Priority0",
     "RX PFC frame transition XON to XOFF for Priority0",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfc1,
     "RX PFC frame with enable bit set for Priority1",
     "RX PFC frame with enable bit set for Priority1",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfcoff1,
     "RX PFC frame transition XON to XOFF for Priority1",
     "RX PFC frame transition XON to XOFF for Priority1",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfc2,
     "RX PFC frame with enable bit set for Priority2",
     "RX PFC frame with enable bit set for Priority2",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfcoff2,
     "RX PFC frame transition XON to XOFF for Priority2",
     "RX PFC frame transition XON to XOFF for Priority2",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfc3,
     "RX PFC frame with enable bit set for Priority3",
     "RX PFC frame with enable bit set for Priority3",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfcoff3,
     "RX PFC frame transition XON to XOFF for Priority3",
     "RX PFC frame transition XON to XOFF for Priority3",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfc4,
     "RX PFC frame with enable bit set for Priority4",
     "RX PFC frame with enable bit set for Priority4",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfcoff4,
     "RX PFC frame transition XON to XOFF for Priority4",
     "RX PFC frame transition XON to XOFF for Priority4",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfc5,
     "RX PFC frame with enable bit set for Priority5",
     "RX PFC frame with enable bit set for Priority5",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfcoff5,
     "RX PFC frame transition XON to XOFF for Priority5",
     "RX PFC frame transition XON to XOFF for Priority5",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfc6,
     "RX PFC frame with enable bit set for Priority6",
     "RX PFC frame with enable bit set for Priority6",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfcoff6,
     "RX PFC frame transition XON to XOFF for Priority6",
     "RX PFC frame transition XON to XOFF for Priority6",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfc7,
     "RX PFC frame with enable bit set for Priority7",
     "RX PFC frame with enable bit set for Priority7",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rpfcoff7,
     "RX PFC frame transition XON to XOFF for Priority7",
     "RX PFC frame transition XON to XOFF for Priority7",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rund,
     "RX undersized frame",
     "RX undersized frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rfrg,
     "RX fragment frame",
     "RX fragment frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rrpkt,
     "RX RUNT frame",
     "RX RUNT frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_reserved1,
     "RESERVED1",
     "RESERVED1",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_t64,
     "TX 64-byte frame",
     "TX 64-byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_t127,
     "TX 65 to 127 byte frame",
     "TX 65 to 127 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_t255,
     "TX 128 to 255 byte frame",
     "TX 128 to 255 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_t511,
     "TX 256 to 511 byte frame",
     "TX 256 to 511 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_t1023,
     "TX 512 to 1023 byte frame",
     "TX 512 to 1023 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_t1518,
     "TX 1024 to 1518 byte frame",
     "TX 1024 to 1518 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tmgv,
     "TX 1519 to 1522 byte frame",
     "TX 1519 to 1522 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_t2047,
     "TX 1519 to 2047 byte frame",
     "TX 1519 to 2047 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_t4095,
     "TX 2048 to 4095 byte frame",
     "TX 2048 to 4095 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_t9216,
     "TX 4096 to 9216 byte frame",
     "TX 4096 to 9216 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_t16383,
     "TX 9217 to 16383 byte frame",
     "TX 9217 to 16383 byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tbca,
     "TX Broadcast frame",
     "TX Broadcast frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfc0,
     "TX PFC frame with enable bit set for Priority0",
     "TX PFC frame with enable bit set for Priority0",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfcoff0,
     "TX PFC frame transition XON to XOFF for Priority0",
     "TX PFC frame transition XON to XOFF for Priority0",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfc1,
     "TX PFC frame with enable bit set for Priority1",
     "TX PFC frame with enable bit set for Priority1",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfcoff1,
     "TX PFC frame transition XON to XOFF for Priority1",
     "TX PFC frame transition XON to XOFF for Priority1",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfc2,
     "TX PFC frame with enable bit set for Priority2",
     "TX PFC frame with enable bit set for Priority2",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfcoff2,
     "TX PFC frame transition XON to XOFF for Priority2",
     "TX PFC frame transition XON to XOFF for Priority2",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfc3,
     "TX PFC frame with enable bit set for Priority3",
     "TX PFC frame with enable bit set for Priority3",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfcoff3,
     "TX PFC frame transition XON to XOFF for Priority3",
     "TX PFC frame transition XON to XOFF for Priority3",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfc4,
     "TX PFC frame with enable bit set for Priority4",
     "TX PFC frame with enable bit set for Priority4",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfcoff4,
     "TX PFC frame transition XON to XOFF for Priority4",
     "TX PFC frame transition XON to XOFF for Priority4",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfc5,
     "TX PFC frame with enable bit set for Priority5",
     "TX PFC frame with enable bit set for Priority5",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfcoff5,
     "TX PFC frame transition XON to XOFF for Priority5",
     "TX PFC frame transition XON to XOFF for Priority5",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfc6,
     "TX PFC frame with enable bit set for Priority6",
     "TX PFC frame with enable bit set for Priority6",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfcoff6,
     "TX PFC frame transition XON to XOFF for Priority6",
     "TX PFC frame transition XON to XOFF for Priority6",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfc7,
     "TX PFC frame with enable bit set for Priority7",
     "TX PFC frame with enable bit set for Priority7",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpfcoff7,
     "TX PFC frame transition XON to XOFF for Priority7",
     "TX PFC frame transition XON to XOFF for Priority7",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpkt,
     "TX all packets frame",
     "TX all packets frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tpok,
     "TX good frame",
     "TX good frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tuca,
     "TX UC frame",
     "TX UC frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tufl,
     "TX FIFO Underrun frame",
     "TX FIFO Underrun frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tmca,
     "TX MC frame",
     "TX MC frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_txpf,
     "TX pause frame",
     "TX pause frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_txpp,
     "TX PFC frame",
     "TX PFC frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_txcf,
     "TX control frame",
     "TX control frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tfcs,
     "TX FCS error frame",
     "TX FCS error frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_terr,
     "TX Error frame",
     "TX Error frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tovr,
     "TX Oversize frame",
     "TX Oversize frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tjbr,
     "TX jabber frame",
     "TX jabber frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_trpkt,
     "TX RUNT frame",
     "TX RUNT frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tfrg,
     "TX runt packet with invalid FCS frame",
     "TX runt packet with invalid FCS frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tvln,
     "TX single and double VLAN tagged frame",
     "TX single and double VLAN tagged frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tdvln,
     "TX double VLAN tagged frame",
     "TX double VLAN tagged frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rbyt,
     "RX Byte frame",
     "RX Byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_rrbyt,
     "RX Runt Byte frame",
     "RX Runt Byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     soc_jer2_cdmib_controlled_counter_get,
     soc_jer2_counters_cdmib_tbyt,
     "TX Byte frame",
     "TX Byte frame",
     _SOC_CONTROLLED_COUNTER_FLAG_NIF,
     COUNTER_IDX_NOT_INITIALIZED,
     soc_jer2_cdmib_controlled_counter_clear,
     soc_jer2_stat_counter_length_get}
    ,
    {
     NULL,
     -1,
     "",
     "",
     0,
     COUNTER_IDX_NOT_INITIALIZED,
     NULL,
     NULL}
};

shr_error_e
soc_jer2_mapping_stat_get(
    int unit,
    soc_port_t port,
    uint32 *counters,
    int *array_size,
    uint32 *sub_counters,
    int *sub_array_size,
    bcm_stat_val_t type,
    int max_array_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(counters, _SHR_E_PARAM, "counters");
    SHR_NULL_CHECK(array_size, _SHR_E_PARAM, "array_size");
    SHR_NULL_CHECK(sub_counters, _SHR_E_PARAM, "counters");
    SHR_NULL_CHECK(sub_array_size, _SHR_E_PARAM, "array_size");
    *sub_array_size = 0;
    switch (type)
    {
        case snmpBcmTxControlCells:
            *array_size = 1;
            counters[0] = soc_jer2_counters_tx_control_cells_counter;
            break;
        case snmpBcmTxDataCells:
            *array_size = 1;
            counters[0] = soc_jer2_counters_tx_data_cell_counter;
            break;
        case snmpBcmTxDataBytes:
            *array_size = 1;
            counters[0] = soc_jer2_counters_tx_data_byte_counter;
            break;
        case snmpBcmRxCrcErrors:
            *array_size = 3;
            counters[0] = soc_jer2_counters_rx_crc_errors_data_cells;
            counters[1] = soc_jer2_counters_rx_crc_errors_control_cells_nonbypass;
            counters[2] = soc_jer2_counters_rx_crc_errors_control_cells_bypass;
            break;
        case snmpBcmRxFecCorrectable:
            *array_size = 1;
            if (IS_SFI_PORT(unit, port))
            {
                counters[0] = soc_jer2_counters_rx_fec_correctable_error;
            }
            else
            {
                counters[0] = soc_jer2_counters_nif_rx_fec_correctable_error;
            }
            break;
        case snmpBcmRxControlCells:
            *array_size = 1;
            counters[0] = soc_jer2_counters_rx_control_cells_counter;
            break;
        case snmpBcmRxDataCells:
            *array_size = 1;
            counters[0] = soc_jer2_counters_rx_data_cell_counter;
            break;
        case snmpBcmRxDataBytes:
            *array_size = 1;
            counters[0] = soc_jer2_counters_rx_data_byte_counter;
            break;
        case snmpBcmRxDroppedRetransmittedControl:
            *array_size = 1;
            counters[0] = soc_jer2_counters_rx_dropped_retransmitted_control;
            break;
        case snmpBcmTxAsynFifoRate:
            *array_size = 1;
            counters[0] = soc_jer2_counters_tx_asyn_fifo_rate;
            break;
        case snmpBcmRxAsynFifoRate:
            *array_size = 1;
            counters[0] = soc_jer2_counters_rx_asyn_fifo_rate;
            break;
        case snmpBcmRxFecUncorrectable:
            *array_size = 1;
            if (IS_SFI_PORT(unit, port))
            {
                counters[0] = soc_jer2_counters_rx_lfec_fec_uncorrrectable_errors;
            }
            else
            {
                counters[0] = soc_jer2_counters_nif_rx_fec_uncorrrectable_errors;
            }
            break;
        case snmpBcmRxBipErrorCount:
            *array_size = 1;
            if (IS_IL_PORT(unit, port))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "snmpBcmRxBipErrorCount not supported for ILKN");
            }
            else if (IS_SFI_PORT(unit, port))
            {
                *array_size = 0;
            }
            else
            {
                counters[0] = soc_jer2_counters_nif_rx_bip_error;
            }
            break;
            /*
             * Above type are only support for SFI, not for ILKN.
             */
            /*
             * Below list in comment is supported for NIF and ILKN(i.e. not SFI) 
             */
            /*
             * snmpEtherStatsDropEvents is not supported also for SFI and ELK, 
             */
            /*
             * (i.e. supported only for NIF and data ILKN) 
             */
            /*
             * SFI and ILKN are not supported except 
             */
            /*
             * snmpIfInNUcastPkts 
             */
            /*
             * snmpIfOutNUcastPkts 
             */
            /*
             * snmpIfInErrors 
             */
            /*
             * snmpIfOutErrors 
             */
            /*
             * snmpEtherStatsRXNoErrors 
             */
            /*
             * snmpEtherStatsTXNoErrors 
             */

            /*
             *** RFC 1213 *** */
        case snmpIfInOctets:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_rbyt;
            counters[1] = soc_jer2_counters_cdmib_rrbyt;
            break;
        case snmpIfInUcastPkts:        /* Unicast frames */
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_ruca;
            break;
        case snmpIfInNUcastPkts:       /* Non-unicast frames */
            *array_size = 2;
            if (IS_IL_PORT(unit, port) || IS_SFI_PORT(unit, port))
            {
                *array_size = 0;
            }
            else
            {
                counters[0] = soc_jer2_counters_cdmib_rmca;
                counters[1] = soc_jer2_counters_cdmib_rbca;
            }
            break;
        case snmpIfInBroadcastPkts:    /* Broadcast frames */
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rbca;
            break;
        case snmpIfInMulticastPkts:    /* Multicast frames */
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rmca;
            break;
        case snmpIfInErrors:   /* RX Errors or Receive packets - non-error frames */
            *array_size = 4;
            if (IS_IL_PORT(unit, port))
            {
                *array_size = 1;
                counters[0] = soc_jer2_counters_ilkn_rx_err_pkt_counter;
            }
            else if (IS_SFI_PORT(unit, port))
            {
                *array_size = 0;
            }
            else
            {
                counters[0] = soc_jer2_counters_cdmib_rfcs;
                counters[1] = soc_jer2_counters_cdmib_rjbr;
                counters[2] = soc_jer2_counters_cdmib_rrpkt;
                counters[3] = soc_jer2_counters_cdmib_rmtue;
            }
            break;
        case snmpIfOutOctets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tbyt;
            break;
        case snmpIfOutUcastPkts:       /* Unicast frames */
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tuca;
            break;
        case snmpIfOutNUcastPkts:      /* Non-unicast frames */
            *array_size = 2;
            if (IS_IL_PORT(unit, port) || IS_SFI_PORT(unit, port))
            {
                *array_size = 0;
            }
            else
            {
                counters[0] = soc_jer2_counters_cdmib_tmca;
                counters[1] = soc_jer2_counters_cdmib_tbca;
            }
            break;
        case snmpIfOutBroadcastPkts:   /* Broadcast frames */
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tbca;
            break;
        case snmpIfOutMulticastPkts:   /* Multicast frames */
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tmca;
            break;
        case snmpIfOutErrors:
            if (IS_IL_PORT(unit, port))
            {
                *array_size = 1;
                counters[0] = soc_jer2_counters_ilkn_tx_err_pkt_counter;
            }
            else if (IS_SFI_PORT(unit, port))
            {
                *array_size = 0;
            }
            else
            {
                /*
                 * Work around for padding enable 
                 */
                int pad_size = 0;
                if (!SAL_BOOT_PLISIM)
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_port_control_get(unit, port, bcmPortControlPadToSize, &pad_size));
                }

                if (DNX_STAT_PAD_SIZE_IS_IN_RANGE(pad_size))
                {
                    *array_size = 2;
                    counters[0] = soc_jer2_counters_cdmib_tfcs;
                    counters[1] = soc_jer2_counters_cdmib_tjbr;
                }
                else
                {
                    *array_size = 3;
                    counters[0] = soc_jer2_counters_cdmib_trpkt;
                    counters[1] = soc_jer2_counters_cdmib_tfcs;
                    counters[2] = soc_jer2_counters_cdmib_tjbr;
                }
            }
            break;
            /*
             *** RFC 1493 *** */
        case snmpDot1dBasePortMtuExceededDiscards:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rmtue;
            break;
        case snmpDot1dTpPortInFrames:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rpkt;
            break;
        case snmpDot1dTpPortOutFrames:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tpkt;
            break;

            /*
             *** RFC 1757 *** */
        case snmpEtherStatsDropEvents:
            *array_size = 3;
            counters[0] = soc_jer2_counters_rx_eth_stats_drop_events_sch_low;
            counters[1] = soc_jer2_counters_rx_eth_stats_drop_events_sch_high;
            counters[2] = soc_jer2_counters_rx_eth_stats_drop_events_sch_tdm;
            break;
        case snmpEtherStatsOctets:
            *array_size = 3;
            counters[0] = soc_jer2_counters_cdmib_rbyt;
            counters[1] = soc_jer2_counters_cdmib_tbyt;
            counters[2] = soc_jer2_counters_cdmib_rrbyt;
            break;
        case snmpEtherStatsPkts:
            *array_size = 4;
            counters[0] = soc_jer2_counters_cdmib_rpkt;
            counters[1] = soc_jer2_counters_cdmib_rflr;
            counters[2] = soc_jer2_counters_cdmib_tpkt;
            counters[3] = soc_jer2_counters_cdmib_rrpkt;        /* Runts */
            break;
        case snmpEtherStatsBroadcastPkts:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_rbca;
            counters[1] = soc_jer2_counters_cdmib_tbca;
            break;
        case snmpEtherStatsMulticastPkts:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_rmca;
            counters[1] = soc_jer2_counters_cdmib_tmca;
            break;
        case snmpEtherStatsCRCAlignErrors:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rfcs;
            break;
        case snmpEtherStatsTxCRCAlignErrors:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tfcs;
            break;
        case snmpEtherStatsUndersizePkts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rund;
            break;
        case snmpEtherStatsOversizePkts:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_rovr;
            counters[1] = soc_jer2_counters_cdmib_tovr;
            break;
        case snmpEtherRxOversizePkts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rovr;
            break;
        case snmpEtherTxOversizePkts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tovr;
            break;
        case snmpEtherStatsFragments:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rfrg;
            break;
        case snmpEtherStatsJabbers:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rjbr;
            break;
        case snmpEtherStatsTxJabbers:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tjbr;
            break;
            /*
             *** rfc1757 definition counts receive packet only *** */
        case snmpEtherStatsPkts64Octets:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_r64;
            counters[1] = soc_jer2_counters_cdmib_t64;
            break;
        case snmpEtherStatsPkts65to127Octets:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_r127;
            counters[1] = soc_jer2_counters_cdmib_t127;
            break;
        case snmpEtherStatsPkts128to255Octets:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_r255;
            counters[1] = soc_jer2_counters_cdmib_t255;
            break;
        case snmpEtherStatsPkts256to511Octets:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_r511;
            counters[1] = soc_jer2_counters_cdmib_t511;
            break;
        case snmpEtherStatsPkts512to1023Octets:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_r1023;
            counters[1] = soc_jer2_counters_cdmib_t1023;
            break;
        case snmpEtherStatsPkts1024to1518Octets:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_r1518;
            counters[1] = soc_jer2_counters_cdmib_t1518;
            break;
        case snmpBcmEtherStatsPkts1519to1522Octets:    /* not in rfc1757 */
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_rmgv;
            counters[1] = soc_jer2_counters_cdmib_tmgv;
            break;
        case snmpBcmEtherStatsPkts1522to2047Octets:    /* not in rfc1757 */
            *array_size = 2;
            *sub_array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_r2047;
            counters[1] = soc_jer2_counters_cdmib_t2047;
            sub_counters[0] = soc_jer2_counters_cdmib_rmgv;
            sub_counters[1] = soc_jer2_counters_cdmib_tmgv;
            break;
        case snmpBcmEtherStatsPkts2048to4095Octets:    /* not in rfc1757 */
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_r4095;
            counters[1] = soc_jer2_counters_cdmib_t4095;
            break;
        case snmpBcmEtherStatsPkts4095to9216Octets:    /* not in rfc1757 */
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_r9216;
            counters[1] = soc_jer2_counters_cdmib_t9216;
            break;
        case snmpBcmEtherStatsPkts9217to16383Octets:   /* not in rfc1757 */
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_r16383;
            counters[1] = soc_jer2_counters_cdmib_t16383;
            break;
        case snmpBcmReceivedPkts64Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_r64;
            break;
        case snmpBcmReceivedPkts65to127Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_r127;
            break;
        case snmpBcmReceivedPkts128to255Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_r255;
            break;
        case snmpBcmReceivedPkts256to511Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_r511;
            break;
        case snmpBcmReceivedPkts512to1023Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_r1023;
            break;
        case snmpBcmReceivedPkts1024to1518Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_r1518;
            break;
        case snmpBcmReceivedPkts1519to2047Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_r2047;
            break;
        case snmpBcmReceivedPkts2048to4095Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_r4095;
            break;
        case snmpBcmReceivedPkts4095to9216Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_r9216;
            break;
        case snmpBcmReceivedPkts9217to16383Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_r16383;
            break;
        case snmpBcmTransmittedPkts64Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_t64;
            break;
        case snmpBcmTransmittedPkts65to127Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_t127;
            break;
        case snmpBcmTransmittedPkts128to255Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_t255;
            break;
        case snmpBcmTransmittedPkts256to511Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_t511;
            break;
        case snmpBcmTransmittedPkts512to1023Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_t1023;
            break;
        case snmpBcmTransmittedPkts1024to1518Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_t1518;
            break;
        case snmpBcmTransmittedPkts1519to2047Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_t2047;
            break;
        case snmpBcmTransmittedPkts2048to4095Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_t4095;
            break;
        case snmpBcmTransmittedPkts4095to9216Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_t9216;
            break;
        case snmpBcmTransmittedPkts9217to16383Octets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_t16383;
            break;
        case snmpEtherStatsTXNoErrors:
            *array_size = 1;
            if (IS_IL_PORT(unit, port))
            {
                counters[0] = soc_jer2_counters_ilkn_tx_pkt_counter;
            }
            else if (IS_SFI_PORT(unit, port))
            {
                *array_size = 0;
            }
            else
            {
                counters[0] = soc_jer2_counters_cdmib_tpok;
            }
            break;
        case snmpEtherStatsRXNoErrors:
            *array_size = 2;
            if (IS_IL_PORT(unit, port))
            {
                *array_size = 1;
                counters[0] = soc_jer2_counters_ilkn_rx_pkt_counter;
            }
            else if (IS_SFI_PORT(unit, port))
            {
                *array_size = 0;
            }
            else
            {
                counters[0] = soc_jer2_counters_cdmib_rpok;
                counters[1] = soc_jer2_counters_cdmib_rflr;
            }
            break;
            /*
             *** RFC 2665 *** */
        case snmpDot3StatsFCSErrors:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rfcs;
            break;
        case snmpDot3StatsInternalMacTransmitErrors:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_tufl;
            counters[1] = soc_jer2_counters_cdmib_terr;
            break;
        case snmpDot3StatsFrameTooLongs:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rmtue;
            break;
        case snmpDot3StatsInternalMacReceiveErrors:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_rrpkt;
            counters[1] = soc_jer2_counters_cdmib_rerpkt;
            break;
        case snmpDot3StatsSymbolErrors:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rerpkt;
            break;
        case snmpDot3ControlInUnknownOpcodes:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rxuo;
            break;
        case snmpDot3InPauseFrames:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rxpf;
            break;
        case snmpDot3OutPauseFrames:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_txpf;
            break;
        case snmpIfHCInOctets:
            *array_size = 2;
            counters[0] = soc_jer2_counters_cdmib_rbyt;
            counters[1] = soc_jer2_counters_cdmib_rrbyt;
            break;
        case snmpIfHCInUcastPkts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_ruca;
            break;
        case snmpIfHCInMulticastPkts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rmca;
            break;
        case snmpIfHCInBroadcastPkts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rbca;
            break;
        case snmpIfHCOutOctets:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tbyt;
            break;
        case snmpIfHCOutUcastPkts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tuca;
            break;
        case snmpIfHCOutMulticastPkts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tmca;
            break;
        case snmpIfHCOutBroadcastPckts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_tbca;
            break;
            /*
             *** RFC 2465 *** */
            /*
             *** IEEE 802.1bb *** */
        case snmpIeee8021PfcRequests:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_txpp;
            break;
        case snmpIeee8021PfcIndications:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rxpp;
            break;
        case snmpBcmReceivedUndersizePkts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_rrpkt;
            break;
        case snmpBcmTransmittedUndersizePkts:
            *array_size = 1;
            counters[0] = soc_jer2_counters_cdmib_trpkt;
            break;
        default:
            if (type < snmpValCount)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Statistic type %d is not supported for port %d\n", type, port);
            }
            SHR_ERR_EXIT(_SHR_E_PARAM, "port: %d, Statistic not supported: %d\n", port, type);
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_jer2_stat_counter_length_get(
    int unit,
    int counter_id,
    int *length)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (counter_id)
    {
        case soc_jer2_counters_rx_crc_errors_control_cells_nonbypass:
        case soc_jer2_counters_rx_crc_errors_control_cells_bypass:
        case soc_jer2_counters_rx_lfec_fec_uncorrrectable_errors:
        case soc_jer2_counters_rx_kpcs_errors_counter:
        case soc_jer2_counters_rx_kpcs_bypass_errors_counter:
            *length = 16;
            break;

        case soc_jer2_counters_rx_crc_errors_data_cells:
        case soc_jer2_counters_rx_fec_correctable_error:
        case soc_jer2_counters_tx_asyn_fifo_rate:
        case soc_jer2_counters_rx_asyn_fifo_rate:
            *length = 32;
            break;

        case soc_jer2_counters_tx_control_cells_counter:
        case soc_jer2_counters_tx_data_cell_counter:
        case soc_jer2_counters_tx_data_byte_counter:
        case soc_jer2_counters_rx_control_cells_counter:
        case soc_jer2_counters_rx_data_cell_counter:
        case soc_jer2_counters_rx_data_byte_counter:
        case soc_jer2_counters_rx_rs_fec_bit_error_counter:
        case soc_jer2_counters_rx_rs_fec_symbol_error_rate_counter:
            *length = 48;
            break;

        case soc_jer2_counters_rx_llfc_primary_pipe:
        case soc_jer2_counters_rx_llfc_second_pipe:
        case soc_jer2_counters_rx_llfc_third_pipe:
            *length = 64;
            break;
        case soc_jer2_counters_ilkn_rx_pkt_counter:
        case soc_jer2_counters_ilkn_tx_pkt_counter:
        case soc_jer2_counters_ilkn_rx_byte_counter:
        case soc_jer2_counters_ilkn_tx_byte_counter:
        case soc_jer2_counters_ilkn_rx_err_pkt_counter:
        case soc_jer2_counters_ilkn_tx_err_pkt_counter:
            *length = 64;
            break;
        case soc_jer2_counters_nif_rx_fec_correctable_error:
        case soc_jer2_counters_nif_rx_fec_uncorrrectable_errors:
        case soc_jer2_counters_nif_rx_bip_error:
        case soc_jer2_counters_rx_eth_stats_drop_events_sch_low:
        case soc_jer2_counters_rx_eth_stats_drop_events_sch_high:
        case soc_jer2_counters_rx_eth_stats_drop_events_sch_tdm:
            *length = 64;
            break;
        case soc_jer2_counters_cdmib_r64:
        case soc_jer2_counters_cdmib_r127:
        case soc_jer2_counters_cdmib_r255:
        case soc_jer2_counters_cdmib_r511:
        case soc_jer2_counters_cdmib_r1023:
        case soc_jer2_counters_cdmib_r1518:
        case soc_jer2_counters_cdmib_rmgv:
        case soc_jer2_counters_cdmib_r2047:
        case soc_jer2_counters_cdmib_r4095:
        case soc_jer2_counters_cdmib_r9216:
        case soc_jer2_counters_cdmib_r16383:
        case soc_jer2_counters_cdmib_rbca:
        case soc_jer2_counters_cdmib_rprog0:
        case soc_jer2_counters_cdmib_rprog1:
        case soc_jer2_counters_cdmib_rprog2:
        case soc_jer2_counters_cdmib_rprog3:
        case soc_jer2_counters_cdmib_rpkt:
        case soc_jer2_counters_cdmib_rpok:
        case soc_jer2_counters_cdmib_ruca:
        case soc_jer2_counters_cdmib_reserved0:
        case soc_jer2_counters_cdmib_rmca:
        case soc_jer2_counters_cdmib_rxpf:
        case soc_jer2_counters_cdmib_rxpp:
        case soc_jer2_counters_cdmib_rxcf:
        case soc_jer2_counters_cdmib_rfcs:
        case soc_jer2_counters_cdmib_rerpkt:
        case soc_jer2_counters_cdmib_rflr:
        case soc_jer2_counters_cdmib_rjbr:
        case soc_jer2_counters_cdmib_rmtue:
        case soc_jer2_counters_cdmib_rovr:
        case soc_jer2_counters_cdmib_rvln:
        case soc_jer2_counters_cdmib_rdvln:
        case soc_jer2_counters_cdmib_rxuo:
        case soc_jer2_counters_cdmib_rxuda:
        case soc_jer2_counters_cdmib_rxwsa:
        case soc_jer2_counters_cdmib_rprm:
        case soc_jer2_counters_cdmib_rpfc0:
        case soc_jer2_counters_cdmib_rpfcoff0:
        case soc_jer2_counters_cdmib_rpfc1:
        case soc_jer2_counters_cdmib_rpfcoff1:
        case soc_jer2_counters_cdmib_rpfc2:
        case soc_jer2_counters_cdmib_rpfcoff2:
        case soc_jer2_counters_cdmib_rpfc3:
        case soc_jer2_counters_cdmib_rpfcoff3:
        case soc_jer2_counters_cdmib_rpfc4:
        case soc_jer2_counters_cdmib_rpfcoff4:
        case soc_jer2_counters_cdmib_rpfc5:
        case soc_jer2_counters_cdmib_rpfcoff5:
        case soc_jer2_counters_cdmib_rpfc6:
        case soc_jer2_counters_cdmib_rpfcoff6:
        case soc_jer2_counters_cdmib_rpfc7:
        case soc_jer2_counters_cdmib_rpfcoff7:
        case soc_jer2_counters_cdmib_rund:
        case soc_jer2_counters_cdmib_rfrg:
        case soc_jer2_counters_cdmib_rrpkt:
        case soc_jer2_counters_cdmib_reserved1:
        case soc_jer2_counters_cdmib_t64:
        case soc_jer2_counters_cdmib_t127:
        case soc_jer2_counters_cdmib_t255:
        case soc_jer2_counters_cdmib_t511:
        case soc_jer2_counters_cdmib_t1023:
        case soc_jer2_counters_cdmib_t1518:
        case soc_jer2_counters_cdmib_tmgv:
        case soc_jer2_counters_cdmib_t2047:
        case soc_jer2_counters_cdmib_t4095:
        case soc_jer2_counters_cdmib_t9216:
        case soc_jer2_counters_cdmib_t16383:
        case soc_jer2_counters_cdmib_tbca:
        case soc_jer2_counters_cdmib_tpfc0:
        case soc_jer2_counters_cdmib_tpfcoff0:
        case soc_jer2_counters_cdmib_tpfc1:
        case soc_jer2_counters_cdmib_tpfcoff1:
        case soc_jer2_counters_cdmib_tpfc2:
        case soc_jer2_counters_cdmib_tpfcoff2:
        case soc_jer2_counters_cdmib_tpfc3:
        case soc_jer2_counters_cdmib_tpfcoff3:
        case soc_jer2_counters_cdmib_tpfc4:
        case soc_jer2_counters_cdmib_tpfcoff4:
        case soc_jer2_counters_cdmib_tpfc5:
        case soc_jer2_counters_cdmib_tpfcoff5:
        case soc_jer2_counters_cdmib_tpfc6:
        case soc_jer2_counters_cdmib_tpfcoff6:
        case soc_jer2_counters_cdmib_tpfc7:
        case soc_jer2_counters_cdmib_tpfcoff7:
        case soc_jer2_counters_cdmib_tpkt:
        case soc_jer2_counters_cdmib_tpok:
        case soc_jer2_counters_cdmib_tuca:
        case soc_jer2_counters_cdmib_tufl:
        case soc_jer2_counters_cdmib_tmca:
        case soc_jer2_counters_cdmib_txpf:
        case soc_jer2_counters_cdmib_txpp:
        case soc_jer2_counters_cdmib_txcf:
        case soc_jer2_counters_cdmib_tfcs:
        case soc_jer2_counters_cdmib_terr:
        case soc_jer2_counters_cdmib_tovr:
        case soc_jer2_counters_cdmib_tjbr:
        case soc_jer2_counters_cdmib_trpkt:
        case soc_jer2_counters_cdmib_tfrg:
        case soc_jer2_counters_cdmib_tvln:
        case soc_jer2_counters_cdmib_tdvln:
            *length = 40;
            break;

        case soc_jer2_counters_cdmib_rbyt:
        case soc_jer2_counters_cdmib_rrbyt:
        case soc_jer2_counters_cdmib_tbyt:
            *length = 48;
            break;

        default:
            *length = 0;
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
soc_jer2_stat_controlled_counter_enable_get(
    int unit,
    soc_port_t port,
    int index,
    int *enable,
    int *printable)
{
    uint32 flags;
    int ilkn_id;
    int offset = 0;
    int ilkn_over_fabric_port = 0;
    soc_control_t *soc;
    SHR_FUNC_INIT_VARS(unit);

    *enable = 1;
    *printable = 1;
    soc = SOC_CONTROL(unit);

    if (IS_SFI_PORT(unit, port))
    {
        if (!(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_MAC))
        {
            *enable = 0;
            SHR_EXIT();
        }
    }
    else if (IS_IL_PORT(unit, port))
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
        flags = _SOC_CONTROLLED_COUNTER_FLAG_ILKN;

        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &offset));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &ilkn_over_fabric_port));
        /*
         * ILKN over fabric - support fabric counters too
         */
        if (ilkn_over_fabric_port &&
            ((soc->controlled_counters[index].counter_id == soc_jer2_counters_tx_asyn_fifo_rate) ||
             (soc->controlled_counters[index].counter_id == soc_jer2_counters_rx_asyn_fifo_rate)))
        {
            flags |= _SOC_CONTROLLED_COUNTER_FLAG_MAC;
        }

         /*ILKN*/ if (!(soc->controlled_counters[index].flags & flags))
        {
            *enable = 0;
        }
    }
    else
    {
         /*NIF*/ if (!(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_NIF))
        {
            *enable = 0;
        }
    }

    if (soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_NOT_PRINTABLE)
    {
        *printable = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
