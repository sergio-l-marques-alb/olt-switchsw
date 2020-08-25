/** \file mib_stat_dbal.c
 * $Id$
 *
 * MIB stat procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/stat/jer2_stat.h>
#include <soc/dnx/legacy/JER2/jer2_regs.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/legacy/mbcm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <sal/types.h>
#include "mib_stat_dbal.h"

/**
* \brief
*   initialize mib stat fabric
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
mib_stat_fabric_init(
    int unit)
{
    uint32 fmac_idx;
    uint32 nof_instances_fmac = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_instances_fmac = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);
    /** Write default values to DBAL_TABLE_STIF_ENABLERS table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_STAT_CTRL, &entry_handle_id));

    for (fmac_idx = 0; fmac_idx < nof_instances_fmac; fmac_idx++)
    {
        int enable;
        SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fmac_block_enable_get, (unit, fmac_idx, &enable)));
        if (enable)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMAC_ID, fmac_idx);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLEAR_ON_READ, INST_SINGLE, 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_MODE, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_COUNTER_HEADER, INST_SINGLE, 1);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - This function get one fabric counter
 * 
 * \param [in] unit     - unit id.
 * \param [in] fmac_idx - fabric index.
 * \param [in] lane_idx - fabric lane index.
 * \param [in] counter_id - counter select id.
 * \param [out] val - the vaule of the counter.
 *   
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_fmac_stat_dbal_get(
    int unit,
    uint32 fmac_idx,
    uint32 lane_idx,
    int counter_id,
    uint64 *val)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    /** select read which counter */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_STAT_CTRL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMAC_ID, fmac_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_SELECT, INST_SINGLE, lane_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_SELECT, INST_SINGLE, counter_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /** request the relevant counter*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_STAT_COUNTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMAC_ID, fmac_idx);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_FMAC_COUNTER, INST_SINGLE, val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function get one cdmib counter
 * 
 * \param [in] unit     - unit id.
 * \param [in] field_id - dbal_field.
 * \param [in] port - logic port num.
 * \param [out] val - the vaule of the counter.
 *   
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_cdmib_stat_dbal_get(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    uint64 *val)
{

    uint32 entry_handle_id;
    uint32 nof_phys_per_cdu, pm_idx, lane_idx;
    int first_phy;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

    nof_phys_per_cdu = dnx_data_nif.eth.cdu_lanes_nof_get(unit);
    pm_idx = first_phy / nof_phys_per_cdu;
    lane_idx = first_phy % nof_phys_per_cdu;
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_MIB_COUNTERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_INDEX, pm_idx);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane_idx);

    dbal_value_field64_request(unit, entry_handle_id, field_id, INST_SINGLE, val);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function set one cdmib counter
 * 
 * \param [in] unit     - unit id.
 * \param [in] field_id - dbal_field.
 * \param [in] port - logic port num.
 * \param [out] val - the vaule of the counter.
 *   
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_cdmib_stat_dbal_set(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    uint64 val)
{

    uint32 entry_handle_id;
    uint32 nof_phys_per_cdu, pm_idx, lane_idx;
    int first_phy;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));
    nof_phys_per_cdu = dnx_data_nif.eth.cdu_lanes_nof_get(unit);
    pm_idx = first_phy / nof_phys_per_cdu;
    lane_idx = first_phy % nof_phys_per_cdu;
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_MIB_COUNTERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_INDEX, pm_idx);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane_idx);
    /*
     * set value field
     */
    dbal_entry_value_field64_set(unit, entry_handle_id, field_id, INST_SINGLE, val);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function set one cdmib counter
 * 
 * \param [in]  unit     - unit id.
 * \param [in]  port - logic port num.
 * \param [in]  counter_id - control counter id.
 * \param [out] field_id - dbal_field
 *   
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_cdmib_counter_id_to_dbal_field_id_get(
    int unit,
    int port,
    int counter_id,
    dbal_fields_e * field_id)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (counter_id)
    {
        case soc_jer2_counters_cdmib_r64:
            *field_id = DBAL_FIELD_R64;
            break;
        case soc_jer2_counters_cdmib_r127:
            *field_id = DBAL_FIELD_R127;
            break;
        case soc_jer2_counters_cdmib_r255:
            *field_id = DBAL_FIELD_R255;
            break;
        case soc_jer2_counters_cdmib_r511:
            *field_id = DBAL_FIELD_R511;
            break;
        case soc_jer2_counters_cdmib_r1023:
            *field_id = DBAL_FIELD_R1023;
            break;
        case soc_jer2_counters_cdmib_r1518:
            *field_id = DBAL_FIELD_R1518;
            break;
        case soc_jer2_counters_cdmib_rmgv:
            *field_id = DBAL_FIELD_RMGV;
            break;
        case soc_jer2_counters_cdmib_r2047:
            *field_id = DBAL_FIELD_R2047;
            break;
        case soc_jer2_counters_cdmib_r4095:
            *field_id = DBAL_FIELD_R4095;
            break;
        case soc_jer2_counters_cdmib_r9216:
            *field_id = DBAL_FIELD_R9216;
            break;
        case soc_jer2_counters_cdmib_r16383:
            *field_id = DBAL_FIELD_R16383;
            break;
        case soc_jer2_counters_cdmib_rbca:
            *field_id = DBAL_FIELD_RBCA;
            break;
        case soc_jer2_counters_cdmib_rprog0:
            *field_id = DBAL_FIELD_RPROG0;
            break;
        case soc_jer2_counters_cdmib_rprog1:
            *field_id = DBAL_FIELD_RPROG1;
            break;
        case soc_jer2_counters_cdmib_rprog2:
            *field_id = DBAL_FIELD_RPROG2;
            break;
        case soc_jer2_counters_cdmib_rprog3:
            *field_id = DBAL_FIELD_RPROG3;
            break;
        case soc_jer2_counters_cdmib_rpkt:
            *field_id = DBAL_FIELD_RPKT;
            break;
        case soc_jer2_counters_cdmib_rpok:
            *field_id = DBAL_FIELD_RPOK;
            break;
        case soc_jer2_counters_cdmib_ruca:
            *field_id = DBAL_FIELD_RUCA;
            break;
        case soc_jer2_counters_cdmib_reserved0:
            *field_id = DBAL_FIELD_RESERVED0;
            break;
        case soc_jer2_counters_cdmib_rmca:
            *field_id = DBAL_FIELD_RMCA;
            break;
        case soc_jer2_counters_cdmib_rxpf:
            *field_id = DBAL_FIELD_RXPF;
            break;
        case soc_jer2_counters_cdmib_rxpp:
            *field_id = DBAL_FIELD_RXPP;
            break;
        case soc_jer2_counters_cdmib_rxcf:
            *field_id = DBAL_FIELD_RXCF;
            break;
        case soc_jer2_counters_cdmib_rfcs:
            *field_id = DBAL_FIELD_RFCS;
            break;
        case soc_jer2_counters_cdmib_rerpkt:
            *field_id = DBAL_FIELD_RERPKT;
            break;
        case soc_jer2_counters_cdmib_rflr:
            *field_id = DBAL_FIELD_RFLR;
            break;
        case soc_jer2_counters_cdmib_rjbr:
            *field_id = DBAL_FIELD_RJBR;
            break;
        case soc_jer2_counters_cdmib_rmtue:
            *field_id = DBAL_FIELD_RMTUE;
            break;
        case soc_jer2_counters_cdmib_rovr:
            *field_id = DBAL_FIELD_ROVR;
            break;
        case soc_jer2_counters_cdmib_rvln:
            *field_id = DBAL_FIELD_RVLN;
            break;
        case soc_jer2_counters_cdmib_rdvln:
            *field_id = DBAL_FIELD_RDVLN;
            break;
        case soc_jer2_counters_cdmib_rxuo:
            *field_id = DBAL_FIELD_RXUO;
            break;
        case soc_jer2_counters_cdmib_rxuda:
            *field_id = DBAL_FIELD_RXUDA;
            break;
        case soc_jer2_counters_cdmib_rxwsa:
            *field_id = DBAL_FIELD_RXWSA;
            break;
        case soc_jer2_counters_cdmib_rprm:
            *field_id = DBAL_FIELD_RPRM;
            break;
        case soc_jer2_counters_cdmib_rpfc0:
            *field_id = DBAL_FIELD_RPFC0;
            break;
        case soc_jer2_counters_cdmib_rpfcoff0:
            *field_id = DBAL_FIELD_RPFCOFF0;
            break;
        case soc_jer2_counters_cdmib_rpfc1:
            *field_id = DBAL_FIELD_RPFC1;
            break;
        case soc_jer2_counters_cdmib_rpfcoff1:
            *field_id = DBAL_FIELD_RPFCOFF1;
            break;
        case soc_jer2_counters_cdmib_rpfc2:
            *field_id = DBAL_FIELD_RPFC2;
            break;
        case soc_jer2_counters_cdmib_rpfcoff2:
            *field_id = DBAL_FIELD_RPFCOFF2;
            break;
        case soc_jer2_counters_cdmib_rpfc3:
            *field_id = DBAL_FIELD_RPFC3;
            break;
        case soc_jer2_counters_cdmib_rpfcoff3:
            *field_id = DBAL_FIELD_RPFCOFF3;
            break;
        case soc_jer2_counters_cdmib_rpfc4:
            *field_id = DBAL_FIELD_RPFC4;
            break;
        case soc_jer2_counters_cdmib_rpfcoff4:
            *field_id = DBAL_FIELD_RPFCOFF4;
            break;
        case soc_jer2_counters_cdmib_rpfc5:
            *field_id = DBAL_FIELD_RPFC5;
            break;
        case soc_jer2_counters_cdmib_rpfcoff5:
            *field_id = DBAL_FIELD_RPFCOFF5;
            break;
        case soc_jer2_counters_cdmib_rpfc6:
            *field_id = DBAL_FIELD_RPFC6;
            break;
        case soc_jer2_counters_cdmib_rpfcoff6:
            *field_id = DBAL_FIELD_RPFCOFF6;
            break;
        case soc_jer2_counters_cdmib_rpfc7:
            *field_id = DBAL_FIELD_RPFC7;
            break;
        case soc_jer2_counters_cdmib_rpfcoff7:
            *field_id = DBAL_FIELD_RPFCOFF7;
            break;
        case soc_jer2_counters_cdmib_rund:
            *field_id = DBAL_FIELD_RUND;
            break;
        case soc_jer2_counters_cdmib_rfrg:
            *field_id = DBAL_FIELD_RFRG;
            break;
        case soc_jer2_counters_cdmib_rrpkt:
            *field_id = DBAL_FIELD_RRPKT;
            break;
        case soc_jer2_counters_cdmib_reserved1:
            *field_id = DBAL_FIELD_RESERVED1;
            break;
        case soc_jer2_counters_cdmib_t64:
            *field_id = DBAL_FIELD_T64;
            break;
        case soc_jer2_counters_cdmib_t127:
            *field_id = DBAL_FIELD_T127;
            break;
        case soc_jer2_counters_cdmib_t255:
            *field_id = DBAL_FIELD_T255;
            break;
        case soc_jer2_counters_cdmib_t511:
            *field_id = DBAL_FIELD_T511;
            break;
        case soc_jer2_counters_cdmib_t1023:
            *field_id = DBAL_FIELD_T1023;
            break;
        case soc_jer2_counters_cdmib_t1518:
            *field_id = DBAL_FIELD_T1518;
            break;
        case soc_jer2_counters_cdmib_tmgv:
            *field_id = DBAL_FIELD_TMGV;
            break;
        case soc_jer2_counters_cdmib_t2047:
            *field_id = DBAL_FIELD_T2047;
            break;
        case soc_jer2_counters_cdmib_t4095:
            *field_id = DBAL_FIELD_T4095;
            break;
        case soc_jer2_counters_cdmib_t9216:
            *field_id = DBAL_FIELD_T9216;
            break;
        case soc_jer2_counters_cdmib_t16383:
            *field_id = DBAL_FIELD_T16383;
            break;
        case soc_jer2_counters_cdmib_tbca:
            *field_id = DBAL_FIELD_TBCA;
            break;
        case soc_jer2_counters_cdmib_tpfc0:
            *field_id = DBAL_FIELD_TPFC0;
            break;
        case soc_jer2_counters_cdmib_tpfcoff0:
            *field_id = DBAL_FIELD_TPFCOFF0;
            break;
        case soc_jer2_counters_cdmib_tpfc1:
            *field_id = DBAL_FIELD_TPFC1;
            break;
        case soc_jer2_counters_cdmib_tpfcoff1:
            *field_id = DBAL_FIELD_TPFCOFF1;
            break;
        case soc_jer2_counters_cdmib_tpfc2:
            *field_id = DBAL_FIELD_TPFC2;
            break;
        case soc_jer2_counters_cdmib_tpfcoff2:
            *field_id = DBAL_FIELD_TPFCOFF2;
            break;
        case soc_jer2_counters_cdmib_tpfc3:
            *field_id = DBAL_FIELD_TPFC3;
            break;
        case soc_jer2_counters_cdmib_tpfcoff3:
            *field_id = DBAL_FIELD_TPFCOFF3;
            break;
        case soc_jer2_counters_cdmib_tpfc4:
            *field_id = DBAL_FIELD_TPFC4;
            break;
        case soc_jer2_counters_cdmib_tpfcoff4:
            *field_id = DBAL_FIELD_TPFCOFF4;
            break;
        case soc_jer2_counters_cdmib_tpfc5:
            *field_id = DBAL_FIELD_TPFC5;
            break;
        case soc_jer2_counters_cdmib_tpfcoff5:
            *field_id = DBAL_FIELD_TPFCOFF5;
            break;
        case soc_jer2_counters_cdmib_tpfc6:
            *field_id = DBAL_FIELD_TPFC6;
            break;
        case soc_jer2_counters_cdmib_tpfcoff6:
            *field_id = DBAL_FIELD_TPFCOFF6;
            break;
        case soc_jer2_counters_cdmib_tpfc7:
            *field_id = DBAL_FIELD_TPFC7;
            break;
        case soc_jer2_counters_cdmib_tpfcoff7:
            *field_id = DBAL_FIELD_TPFCOFF7;
            break;
        case soc_jer2_counters_cdmib_tpkt:
            *field_id = DBAL_FIELD_TPKT;
            break;
        case soc_jer2_counters_cdmib_tpok:
            *field_id = DBAL_FIELD_TPOK;
            break;
        case soc_jer2_counters_cdmib_tuca:
            *field_id = DBAL_FIELD_TUCA;
            break;
        case soc_jer2_counters_cdmib_tufl:
            *field_id = DBAL_FIELD_TUFL;
            break;
        case soc_jer2_counters_cdmib_tmca:
            *field_id = DBAL_FIELD_TMCA;
            break;
        case soc_jer2_counters_cdmib_txpf:
            *field_id = DBAL_FIELD_TXPF;
            break;
        case soc_jer2_counters_cdmib_txpp:
            *field_id = DBAL_FIELD_TXPP;
            break;
        case soc_jer2_counters_cdmib_txcf:
            *field_id = DBAL_FIELD_TXCF;
            break;
        case soc_jer2_counters_cdmib_tfcs:
            *field_id = DBAL_FIELD_TFCS;
            break;
        case soc_jer2_counters_cdmib_terr:
            *field_id = DBAL_FIELD_TERR;
            break;
        case soc_jer2_counters_cdmib_tovr:
            *field_id = DBAL_FIELD_TOVR;
            break;
        case soc_jer2_counters_cdmib_tjbr:
            *field_id = DBAL_FIELD_TJBR;
            break;
        case soc_jer2_counters_cdmib_trpkt:
            *field_id = DBAL_FIELD_TRPKT;
            break;
        case soc_jer2_counters_cdmib_tfrg:
            *field_id = DBAL_FIELD_TFRG;
            break;
        case soc_jer2_counters_cdmib_tvln:
            *field_id = DBAL_FIELD_TVLN;
            break;
        case soc_jer2_counters_cdmib_tdvln:
            *field_id = DBAL_FIELD_TDVLN;
            break;
        case soc_jer2_counters_cdmib_rbyt:
            *field_id = DBAL_FIELD_RBYT;
            break;
        case soc_jer2_counters_cdmib_rrbyt:
            *field_id = DBAL_FIELD_RRBYT;
            break;
        case soc_jer2_counters_cdmib_tbyt:
            *field_id = DBAL_FIELD_TBYT;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is not supported for port %d.", counter_id, port);
    }

exit:
    SHR_FUNC_EXIT;
}
