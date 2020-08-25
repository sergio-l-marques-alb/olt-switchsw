/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */

#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dbal/dbal.h>
#include "imb_ile_internal.h"

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/**
 * \brief - Enable ILE port in wrapper. This action essentially reset the port in the ILKN core, 
 * but without affecting the ILKN core configurations. this reset is done before any access to the PM phys. 
 * see .h file 
 */
int
imb_ile_port_wrapper_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    uint32 entry_handle_id;
    int ilkn_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set number of segments to be used by the port 
 * see .h file 
 */
int
imb_ile_port_nof_segments_set(
    int unit,
    bcm_port_t port,
    int nof_segments)
{
    uint32 entry_handle_id;
    int ilkn_id;
    dbal_enum_value_field_nif_ilkn_segments_number_e segment_num;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CONNECTIVITY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    /*
     * set value fields
     */
    switch (nof_segments)
    {
        case 0:
            segment_num = DBAL_ENUM_FVAL_NIF_ILKN_SEGMENTS_NUMBER_NO_SEGMENTS;
            break;
        case 2:
            segment_num = DBAL_ENUM_FVAL_NIF_ILKN_SEGMENTS_NUMBER_TWO_SEGMENTS;
            break;
        case 4:
            segment_num = DBAL_ENUM_FVAL_NIF_ILKN_SEGMENTS_NUMBER_FOUR_SEGMENTS;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid nof segments %d\n", nof_segments);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENTS_NUMBER, INST_SINGLE, segment_num);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable segments to be used by the port
 * see .h file
 */
int
imb_ile_port_segments_enable_set(
    int unit,
    bcm_port_t port,
    int nof_segments)
{
    uint32 entry_handle_id;
    int ilkn_id;
    dbal_enum_value_field_nif_ilkn_segments_selection_e segment_sel;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CONNECTIVITY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    /*
     * set value fields
     */
    switch (nof_segments)
    {
        case 0:
            segment_sel = DBAL_ENUM_FVAL_NIF_ILKN_SEGMENTS_SELECTION_NO_SEGMENTS;
            break;
        case 2:
            segment_sel =
                (ilkn_id ==
                 0) ? DBAL_ENUM_FVAL_NIF_ILKN_SEGMENTS_SELECTION_TWO_SEGMENTS_0_1 :
                DBAL_ENUM_FVAL_NIF_ILKN_SEGMENTS_SELECTION_TWO_SEGMENTS_2_3;
            break;
        case 4:
            segment_sel = DBAL_ENUM_FVAL_NIF_ILKN_SEGMENTS_SELECTION_FOUR_SEGMENTS;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid nof segments %d\n", nof_segments);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEGMENTS_SELECTION, INST_SINGLE, segment_sel);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ILKN to operate over fabric links instead of NIF 
 *        phys.
 * see .h file 
 */
int
imb_ile_port_ilkn_over_fabric_set(
    int unit,
    bcm_port_t port,
    int is_over_fabric)
{
    uint32 entry_handle_id;
    int ilkn_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_CONNECTIVITY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OVER_FABRIC, INST_SINGLE,
                                 is_over_fabric ?
                                 DBAL_ENUM_FVAL_NIF_ILKN_FABRIC_NIF_SELECT_ILKN_OVER_FABRIC :
                                 DBAL_ENUM_FVAL_NIF_ILKN_FABRIC_NIF_SELECT_ILKN_OVER_NIF);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ILKN port to operate as ELK port (in JER2 this 
 *        is the only valid option for ILKN)
 * see .h file 
 */
int
imb_ile_port_ilkn_elk_set(
    int unit,
    bcm_port_t port,
    int is_elk)
{
    uint32 entry_handle_id;
    int ilkn_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ELK_ENABLE, INST_SINGLE, is_elk);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - reset the ILKN port. this is the main reset for the 
 *        port which affect all prior configurations. this
 *        should be the first reset before starting any ILKN
 *        core configurations.
 * see .h file 
 */
int
imb_ile_port_reset_set(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    uint32 entry_handle_id;
    int ilkn_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - reset the bypass interface in the CDU. 
 * when ILKN port is operating over NIF, the CDU logic needs to 
 * be bypassed. this enables the bypass interface to take place. 
 * see .h file 
 */
int
imb_ile_port_bypass_if_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    uint32 entry_handle_id;
    int ilkn_id, first_lane = -1, last_lane = -1;
    bcm_pbmp_t ilkn_lanes;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_lanes_get(unit, port, &ilkn_lanes));

    
    _SHR_PBMP_FIRST(ilkn_lanes, first_lane);
    _SHR_PBMP_LAST(ilkn_lanes, last_lane);

    if (flags & IMB_ILE_DIRECTION_RX)
    {
        /*
         * alloc DBAL table handle 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_LANE_CTRL, &entry_handle_id));
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION, DBAL_ENUM_FVAL_CONNECTION_DIRECTION_RX);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_LANE, first_lane, last_lane);
        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYPASS_IF_ENABLE, INST_SINGLE, enable);
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Clear the handle before re-use
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ILE_LANE_CTRL, entry_handle_id));
    }
    if (flags & IMB_ILE_DIRECTION_TX)
    {
        /*
         * alloc DBAL table handle 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_LANE_CTRL, &entry_handle_id));
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DIRECTION, DBAL_ENUM_FVAL_CONNECTION_DIRECTION_TX);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_LANE, first_lane, last_lane);
        /*
         * set value fields
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYPASS_IF_ENABLE, INST_SINGLE, enable);
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get latch down indication for the port. 
 * see .h file 
 */
int
imb_ile_port_latch_down_get(
    int unit,
    bcm_port_t port,
    uint32 *is_latch_low_aligned)
{
    uint32 entry_handle_id;
    int ilkn_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_LATCH_LOW_ALIGNED, INST_SINGLE,
                               is_latch_low_aligned);
    /*
     * commit the request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get pm boundaries for ILKN port. If ILKN port occupies
 *       more than one PM, it will return the lower boundary should
 *       reside in the first PM and the upper boundary should be reside
 *       in the last PM.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_over_fabric - if the ilkn port is over fabric
 * \param [out] lower_bound - PM lower bound for the first PM
 * \param [out] upper_bound - PM upper bound for the last PMS
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

static shr_error_e
imb_ile_port_pms_boundary_get(
    int unit,
    bcm_port_t port,
    int is_over_fabric,
    int *lower_bound,
    int *upper_bound)
{
    int ilkn_id;
    bcm_pbmp_t ilkn_phys;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));

    /*
     * Get ILKN phys
     */
    if (is_over_fabric)
    {
        ilkn_phys = dnx_data_nif.ilkn.supported_phys_get(unit, ilkn_id)->fabric_phys;
    }
    else
    {
        ilkn_phys = dnx_data_nif.ilkn.supported_phys_get(unit, ilkn_id)->nif_phys;
    }

    _SHR_PBMP_FIRST(ilkn_phys, *lower_bound);
    _SHR_PBMP_LAST(ilkn_phys, *upper_bound);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the lane map info for ILKN core.
 *
 * see .h file 
 */
shr_error_e
imb_ile_port_ilkn_core_lane_map_get(
    int unit,
    bcm_port_t port,
    int nof_lanes,
    int *rx_lane_map,
    int *tx_lane_map)
{
    int is_over_fabric, ilkn_id, lane_id;
    int lower_bound = 0, upper_bound = 0;
    dnx_algo_lane_map_type_e type;
    soc_dnxc_lane_map_db_map_t lane2serdes[DNX_DATA_MAX_NIF_PHYS_NOF_PHYS];
    int max_nof_pms, pm, pm_id, pm_first_phy;
    int max_lanes_per_pm;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    SHR_IF_ERR_EXIT(imb_ile_port_pms_boundary_get(unit, port, is_over_fabric, &lower_bound, &upper_bound));

    type = is_over_fabric ? DNX_ALGO_LANE_MAP_FABRIC_SIDE : DNX_ALGO_LANE_MAP_NIF_SIDE;

    /** Get all the lane to serdes mapping for all the PMs */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_pm_lane_to_serdes_map_get(unit, type, lower_bound, upper_bound, lane2serdes));

    max_nof_pms = dnx_data_nif.ilkn.pms_nof_get(unit);
    max_lanes_per_pm = dnx_data_nif.ilkn.lanes_max_nof_get(unit) / max_nof_pms;

    for (pm = 0; pm < max_nof_pms; ++pm)
    {
        if (is_over_fabric)
        {
            pm_id = dnx_data_nif.ilkn.ilkn_pms_get(unit, ilkn_id)->fabric_pm_ids[pm];
            pm_first_phy = dnx_data_nif.ilkn.fabric_pms_get(unit, pm_id)->first_phy;
        }
        else
        {
            pm_id = dnx_data_nif.ilkn.ilkn_pms_get(unit, ilkn_id)->nif_pm_ids[pm];
            pm_first_phy = dnx_data_nif.ilkn.nif_pms_get(unit, pm_id)->first_phy;
        }
        for (lane_id = 0; lane_id < max_lanes_per_pm; ++lane_id)
        {
            rx_lane_map[lane_id + pm * max_lanes_per_pm] = lane2serdes[pm_first_phy - lower_bound + lane_id].rx_id;
            tx_lane_map[lane_id + pm * max_lanes_per_pm] = lane2serdes[pm_first_phy - lower_bound + lane_id].tx_id;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - prepare to get rx counter for setting ilkn rx ctrl.
 *
 * see .h file 
 */
int
imb_ile_port_ilkn_rx_counter_get_prepared_by_channel(
    int unit,
    bcm_port_t port,
    int channel)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_RX_STATS_ACC, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PORT, port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TYPE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMD, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDR, INST_SINGLE, channel);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - prepare to get tx counter for setting ilkn tx ctrl.
 *
 * see .h file 
 */
int
imb_ile_port_ilkn_tx_counter_get_prepared_by_channel(
    int unit,
    bcm_port_t port,
    int channel)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_TX_STATS_ACC, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PORT, port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TYPE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMD, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDR, INST_SINGLE, channel);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get one ILKN counter from HW.
 *
 * see .h file 
 */
int
imb_ile_port_ilkn_counter_dbal_get(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    uint32 *val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_STATS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOGICAL_PORT, port);
    /*
     * get value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, field_id, INST_SINGLE, val);
    /*
     *  Preforming the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get one ILKN fec counter from HW.
 *
 * see .h file 
 */
int
imb_ile_port_ilkn_fec_counter_dbal_get(
    int unit,
    int ilkn_id,
    uint32 fec_instance,
    dbal_fields_e field_id,
    uint32 *val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_ELK_FEC_COUNTERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_INSTANCE, fec_instance);
    /*
     * get value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, field_id, INST_SINGLE, val);
    /*
     *  Preforming the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable ILKN TX traffic pause in reaction to RxLLFC.
 * see .h file
 */
int
imb_ile_port_rx_llfc_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    uint32 entry_handle_id;
    int ilkn_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILE_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_LLFC_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
