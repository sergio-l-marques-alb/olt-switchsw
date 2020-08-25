/** \file fabric.c
 * $Id$
 *
 * Fabric APIs functions for DNX. \n
 * Note - most of the code is currently ported and is located under legacy folder
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

#include <bcm_int/dnx_dispatch.h>

#include <bcm_int/dnx/legacy/fabric.h>

#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/ingress/system_red.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/fabric/fabric_cgm.h>
#include <bcm_int/dnx/fabric/fabric_control_cells.h>
#include <bcm_int/dnx/fabric/fabric_rci_throttling.h>
#include <bcm_int/dnx/fabric/fabric_mesh.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>

#include <bcm/fabric.h>

#include <soc/dnx/dbal/dbal.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lane_map.h>

#include <soc/dnx/legacy/mbcm.h>
#include <soc/dnx/legacy/JER2/jer2_regs.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_fabric_access.h>

#include <soc/dnxc/legacy/dnxc_port.h>

#include <shared/shrextend/shrextend_debug.h>
#include <soc/sand/sand_aux_access.h>

/**
 * \brief
 *   Initialize sw_state database.
 */
static shr_error_e
dnx_fabric_sw_state_db_init(
    int unit)
{
    uint32 dest = 0;
    uint32 nof_max_dest = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init dnx_fabric_db
     */
    SHR_IF_ERR_EXIT(dnx_fabric_db.init(unit));

    /*
     * Alloc groups
     */
    SHR_IF_ERR_EXIT(dnx_fabric_db.modids_to_group_map.alloc(unit));

    /*
     * Alloc modids for each group (destination)
     */
    nof_max_dest = dnx_data_fabric.mesh.nof_max_dest_get(unit);
    for (dest = 0; dest < nof_max_dest; ++dest)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_db.modids_to_group_map.modids.alloc(unit, dest));
    }

    /*
     * Alloc links
     */
    SHR_IF_ERR_EXIT(dnx_fabric_db.links.alloc(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set number of pipes configurations
 */
static shr_error_e
dnx_fabric_pipes_nof_init(
    int unit)
{
    uint32 max_pipe_id = dnx_data_fabric.pipes.nof_pipes_get(unit) - 1;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PIPES, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_PIPE_ID, INST_SINGLE, max_pipe_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set pipes mapping configurations
 */
static shr_error_e
dnx_fabric_pipes_mapping_init(
    int unit)
{
    const dnx_data_fabric_pipes_map_t *fabric_pipe_map = dnx_data_fabric.pipes.map_get(unit);
    int strict_priority_pipe = -1;
    dbal_enum_value_field_fabric_dtq_mode_e dtq_mode = DBAL_NOF_ENUM_FABRIC_DTQ_MODE_VALUES;
    uint32 entry_handle_id;
    int fabric_priority = 0;
    int nof_fabric_priorities = dnx_data_fabric.cell.nof_priorities_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure pipe mapping
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PIPES_MAPPING, &entry_handle_id));
    for (fabric_priority = 0; fabric_priority < nof_fabric_priorities; ++fabric_priority)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAST, DBAL_ENUM_FVAL_TRAFFIC_CAST_UC);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, fabric_priority);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, INST_SINGLE,
                                     fabric_pipe_map->uc[fabric_priority]);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAST, DBAL_ENUM_FVAL_TRAFFIC_CAST_MC);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, fabric_priority);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, INST_SINGLE,
                                     fabric_pipe_map->mc[fabric_priority]);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Configure strict priority and DTQ mode:
     * 1. Give strict priority to TDM pipe (if exist).
     * 2. TDM traffic goes straight to FDT (bypass) without entering DTQs, so DTQ_MODE ignores TDM.
     */
    switch (fabric_pipe_map->type)
    {
        case soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
        {
            strict_priority_pipe = -1;
            dtq_mode = DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_UC_HPMC_LPMC;
            break;
        }
        case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
        {
            strict_priority_pipe = 2;
            dtq_mode = DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_UC_MC;
            break;
        }
        case soc_dnxc_fabric_pipe_map_dual_uc_mc:
        {
            strict_priority_pipe = -1;
            dtq_mode = DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_UC_MC;
            break;
        }
        case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
        {
            strict_priority_pipe = 1;
            dtq_mode = DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_SINGLE_QUEUE;
            break;
        }
        case soc_dnxc_fabric_pipe_map_single:
        {
            strict_priority_pipe = -1;
            dtq_mode = DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_SINGLE_QUEUE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "fabric pipes map type is not supported");
            break;
        }
    }

    /*
     * Set strict priority to TDM pipe (if exists)
     */
    if (strict_priority_pipe != -1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_PIPES_STRICT_PRIORITY, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, INST_SINGLE, strict_priority_pipe);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Set DTQ mode
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_PIPES_DTQ_MODE, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DTQ_MODE, INST_SINGLE, dtq_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set multi-pipe system configurations
 */
static shr_error_e
dnx_fabric_pipes_multi_pipe_system_init(
    int unit)
{
    uint32 ot_size = 0;
    uint32 gt_size = 0;
    int nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    int multi_pipe_system = dnx_data_fabric.pipes.multi_pipe_system_get(unit);
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PIPES_MULTI_PIPES_SYSTEM, &entry_handle_id));

    ot_size = ((nof_pipes > 1) || (multi_pipe_system == TRUE)) ? 0x2 : 0x0;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OT_SIZE, INST_SINGLE, ot_size);

    gt_size = dnx_data_fabric.debug.mesh_topology_size_get(unit);
    if (gt_size == -1)
    {
        gt_size = ot_size;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GT_SIZE, INST_SINGLE, gt_size);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialization of fabric pipes
 */
static shr_error_e
dnx_fabric_pipes_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure nof pipes
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pipes_nof_init(unit));

    /*
     * Configure pipe mapping
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pipes_mapping_init(unit));

    /*
     * Set multi-pipe system configurations
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pipes_multi_pipe_system_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric priority mapping
 */
static shr_error_e
dnx_fabric_priority_map_init(
    int unit)
{
    uint32 entry_handle_id;
    int is_hp = 0;
    int is_mc = 0;
    int dp = 0;
    int tc = 0;
    int is_ocb_only = 0;
    int min_hp_mc = dnx_data_fabric.pipes.map_get(unit)->min_hp_mc;
    int fabric_priority = 0;
    soc_dnxc_fabric_pipe_map_type_t map_type = dnx_data_fabric.pipes.map_get(unit)->type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNXC_PCID_LITE_SKIP(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CELL_PRIORITY, &entry_handle_id));

    /*
     * Loop over all table indices
     */
    for (is_hp = 0; is_hp <= 1; ++is_hp)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HP, is_hp);

        for (is_mc = 0; is_mc <= 1; ++is_mc)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);

            for (dp = 0; dp < DNX_COSQ_NOF_DP; ++dp)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

                for (tc = 0; tc < DNX_COSQ_NOF_TC; ++tc)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);

                    for (is_ocb_only = 0; is_ocb_only <= 1; ++is_ocb_only)
                    {
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OCB_ONLY, is_ocb_only);

                        if (map_type == soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc)
                        {
                            /*
                             * fabric_priority must be fully aligned with hp_bit since dtq uses hp_bit to decide queue id while FDR
                             * uses fabric_priority to decide pipe id.  Therefore, in "uc / hp mc / lp mc" configuration => LP bit ==>
                             * priority 0 (= lowest priority of lp mc) HP bit ==> lowest priority of hp mc 
                             */
                            fabric_priority = is_hp ? min_hp_mc : 0;
                        }
                        else
                        {
                            /*
                             * tc=0,1,2 ==> prio=0
                             * tc=3,4,5 ==> prio=1
                             * tc=6,7   ==> prio=2
                             */
                            fabric_priority = tc / 3;
                        }

                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, INST_SINGLE,
                                                     fabric_priority);
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, 0);
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                    }
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric TDM priority
 */
static shr_error_e
dnx_fabric_priority_tdm_init(
    int unit)
{
    int tdm_priority = dnx_data_fabric.tdm.priority_get(unit);
    int tdm_priority_disabled = dnx_data_fabric.cell.nof_priorities_get(unit);
    int tdm_header_priority;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    tdm_header_priority = (tdm_priority == -1) ? tdm_priority_disabled : tdm_priority;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TDM_PRIORITY, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TDM_PRIORITY, INST_SINGLE, tdm_header_priority);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric priority configurations
 */
static shr_error_e
dnx_fabric_priority_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure priority mapping
     */
    SHR_IF_ERR_EXIT(dnx_fabric_priority_map_init(unit));

    /*
     * Configure TDM priority
     */
    SHR_IF_ERR_EXIT(dnx_fabric_priority_tdm_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize Fabric PCP (Packet Cell Packing)
 */
static shr_error_e
dnx_fabric_pcp_init(
    int unit)
{
    uint32 entry_handle_id;
    int pcp_enable;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * enable/disable fabric_pcp
     */
    pcp_enable = dnx_data_fabric.cell.pcp_enable_get(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PCP_ENABLE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, pcp_enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize Fabric WFQs.
 */
static shr_error_e
dnx_fabric_wfq_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_nof_pipes = dnx_data_fabric.pipes.max_nof_pipes_get(unit);
    uint32 nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    uint32 pipe_id = 0;
    uint32 max_nof_contexts = max_nof_pipes;
    uint32 nof_contexts = nof_pipes;
    uint32 context_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * WFQ will be affected by Mixed traffic only
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_INIT, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_WFQ_AFFECTED_BY_OCB_TRAFFIC, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Enable WFQ between pipes (only between active pipes)
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_WFQ_PIPE_ENABLE, entry_handle_id));
    for (pipe_id = 0; pipe_id < max_nof_pipes; ++pipe_id)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, pipe_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE,
                                     (pipe_id < nof_pipes) ? 1 : 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Enable WFQ between contexts (only between active contexts).
     * Note: currently contexts are equal to pipes, but in future implementation
     * we plan to separate them, so nof_contexts won't necessarily equal nof_pipes.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_WFQ_CONTEXT_ENABLE, entry_handle_id));
    for (context_id = 0; context_id < max_nof_contexts; ++context_id)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_ID, context_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE,
                                     (context_id < nof_contexts) ? 1 : 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric features.
 */
static shr_error_e
dnx_fabric_features_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init ALUWP
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_aluwp_init, (unit)));

    /*
     * Init PDQ & DTQ
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_transmit_init, (unit)));

    /*
     * Init Multicast
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_multicast_init, (unit)));

    /*
     * Init Mesh Topology 
     */
    SHR_IF_ERR_EXIT(dnx_fabric_mesh_topology_init(unit));

    /*
     * Configure Fabric Pipes mapping 
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pipes_init(unit));

    /*
     * Configure Fabric Priority
     */
    SHR_IF_ERR_EXIT(dnx_fabric_priority_init(unit));

    /*
     * Configure packet cell packing 
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pcp_init(unit));

    /*
     * Configure MESH settings
     */
    SHR_IF_ERR_EXIT(dnx_fabric_mesh_init(unit));

    /*
     * Configure segmentation and interleaving 
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_interleaving_init, (unit)));

    /*
     * Configure load balance mode 
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_load_balance_init, (unit)));

    /*
     * Configure Fabric WFQs
     */
    SHR_IF_ERR_EXIT(dnx_fabric_wfq_init(unit));

    /*
     * Configure flow control 
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_init(unit));

    /*
     * Enable minimal links to destination device.
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_minimal_links_to_dest_init, (unit)));

    /*
     * Configure fabric link to core mapping.
     * Note relevant for single core.
     */
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_scheduler_adaptation_init, (unit)));
    }

    /*
     * Init fabric delay single cell in fabric rx
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_delay_single_cell_in_fabric_rx_init, (unit)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric ports.
 */
static shr_error_e
dnx_fabric_ports_init(
    int unit)
{
    int pm_id = 0;
    int quad_id = 0;
    int link_id = 0;
    int fmac_index = 0;
    dnx_algo_port_fabric_add_t info;
    int fabric_port_base = 0;
    int nof_fabric_links = 0;
    int nof_links_in_fmac = 0;
    int nof_links_in_pm = 0;
    int nof_fabric_pms = 0;
    int nof_fmacs_in_pm = 0;
    int quad_enable = 0;
    int i_link = 0;
    bcm_pbmp_t pbmp;

    bcm_pbmp_t okay_pbmp;
    int port_enable = 0;
    bcm_port_t logical_port = 0;
    bcm_port_resource_t port_resource;
    int init_speed = 0;
    int tx_pam4_precoder;
    int lp_tx_precoder;
    uint32 pm_bypassed_links = 0;
    uint32 is_bypassed = 0;

    soc_dnxc_lane_map_db_map_t lane2serdes[DNX_DATA_MAX_FABRIC_LINKS_NOF_LINKS];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&info, 0, sizeof(dnx_algo_port_fabric_add_t));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(okay_pbmp);

    /*
     * Init local Fabric ports 
     */
    fabric_port_base = dnx_data_port.general.fabric_port_base_get(unit);
    nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);
    nof_links_in_fmac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);
    nof_links_in_pm = dnx_data_fabric.blocks.nof_links_in_pm_get(unit);
    nof_fmacs_in_pm = dnx_data_fabric.blocks.nof_fmacs_in_pm_get(unit);

    /** Create fabric ports bitmap */
    for (pm_id = 0; pm_id < nof_fabric_pms; ++pm_id)
    {
        /** Get PM links (octet) bypass mode */
        pm_bypassed_links = dnx_data_fabric.ilkn.bypass_info_get(unit, pm_id)->links;

        for (fmac_index = 0; fmac_index < nof_fmacs_in_pm; fmac_index++)
        {
            /** Check quad enable status */
            quad_id = (pm_id * nof_fmacs_in_pm) + fmac_index;
            quad_enable = dnx_data_port.static_add.fabric_quad_info_get(unit, quad_id)->quad_enable;

            for (i_link = 0; i_link < nof_links_in_fmac; ++i_link)
            {
                /** get link bypass mode */
                is_bypassed = SHR_BITGET(&pm_bypassed_links, fmac_index * nof_links_in_fmac + i_link) ? 1 : 0;

                /** calculate link-id */
                link_id = pm_id * nof_links_in_pm + fmac_index * nof_links_in_fmac + i_link;

                /** Add logical port if not in bypass and quad is enabled*/
                if (quad_enable == FALSE || is_bypassed)
                {
                    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL
                                    (unit, mbcm_dnx_fabric_link_force_signal_set, (unit, link_id, TRUE)));
                }
                else
                {
                    BCM_PBMP_PORT_ADD(pbmp, fabric_port_base + link_id);
                }

            }
        }
    }

    /** Set Lane Map DB according to dnx-data */
    for (link_id = 0; link_id < nof_fabric_links; link_id++)
    {
        lane2serdes[link_id].rx_id = dnx_data_lane_map.fabric.mapping_get(unit, link_id)->serdes_rx_id;
        lane2serdes[link_id].tx_id = dnx_data_lane_map.fabric.mapping_get(unit, link_id)->serdes_tx_id;
    }
    /** Store lane mapping info to swstate */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_set(unit, DNX_ALGO_LANE_MAP_FABRIC_SIDE, nof_fabric_links, lane2serdes));

    /*
     * Power down all FSRD blocks.
     * On bcm_dnx_port_probe the enabled blocks will be powered up again.
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fsrd_blocks_low_power_init, (unit)));

    /*
     * Probe fabric ports
     */
    if (!BCM_PBMP_IS_NULL(pbmp))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_probe(unit, pbmp, &okay_pbmp));
    }

    /*
     * Post-probe operations 
     */
    PBMP_ITER(okay_pbmp, logical_port)
    {
#ifdef BCM_PORT_DEFAULT_DISABLE
        port_enable = FALSE;
#else
        port_enable = TRUE;
#endif /* BCM_PORT_DEFAULT_DISABLE */

        init_speed = dnx_data_port.static_add.speed_get(unit, logical_port)->val;
        if (DNXC_PORT_INVALID_SPEED == init_speed)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Port init speed must be configured for port %d. To disable port, set its speed to -1.",
                         logical_port);
        }
        else if (BCM_PORT_RESOURCE_DEFAULT_REQUEST == init_speed)
        {
            port_enable = FALSE;
        }
        else
        {
            bcm_port_resource_t_init(&port_resource);

            /*
             * get SoC properties values - speed, encoding, CL72 
             * Speed must be overriden with SoC property value.
             * if fec_type and link_training soc propeties are not defined, BCM_PORT_RESOURCE_DEFAULT_REQUEST
             * will be set as their default value.
             */
            port_resource.speed = init_speed;
            port_resource.fec_type = dnx_data_port.static_add.fec_type_get(unit, logical_port)->val;
            port_resource.link_training = dnx_data_port.static_add.link_training_get(unit, logical_port)->val;
            port_resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;

            /*
             * get default recommended encoding, CL72, lane_config based on what we got from SoC properties above
             * (fill values that were not set by SoC properties) 
             */
            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_default_get(unit, logical_port, 0, &port_resource));

            /** override phy lane config bits with values from SoC property */
            SHR_IF_ERR_EXIT(dnx_port_resource_lane_config_soc_properties_get(unit, logical_port, &port_resource));

            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_set(unit, logical_port, &port_resource));

            /** set precoder according to SoC property */
            tx_pam4_precoder = dnx_data_port.static_add.tx_pam4_precoder_get(unit, logical_port)->val;

            if (port_resource.link_training == 0)
            {
                SHR_IF_ERR_EXIT(bcm_dnx_port_phy_control_set
                                (unit, logical_port, BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE, tx_pam4_precoder));
            }
            else if ((port_resource.link_training == 1) && (tx_pam4_precoder == 1))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "port %d: if link training is enabled, enabling tx_pam4_precoder is not allowed.",
                             logical_port);
            }

            /** set lp precoder according to SoC property */
            lp_tx_precoder = dnx_data_port.static_add.lp_tx_precoder_get(unit, logical_port)->val;

            if (port_resource.link_training == 0)
            {
                SHR_IF_ERR_EXIT(bcm_dnx_port_phy_control_set
                                (unit, logical_port, BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE, lp_tx_precoder));
            }
            else if ((port_resource.link_training == 1) && (lp_tx_precoder == 1))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "port %d: if link training is enabled, enabling lp_tx_precoder is not allowed.",
                             logical_port);
            }

            /*
             * if link training is disabled - set TX FIR params according to SoC property value 
             */
            if (port_resource.link_training == 0)
            {
                SHR_IF_ERR_EXIT(dnx_port_init_serdes_tx_taps_config(unit, logical_port));
            }
        }

        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_set(unit, logical_port, port_enable));
    }

    /*
     * Configure ALDWP
     */
    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_aldwp_config, (unit)));

    /*
     * Set all fabric links in loopback if single fap system 
     */
    if (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)
    {
        PBMP_ITER(okay_pbmp, logical_port)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, logical_port, &port_enable));
            if (port_enable == TRUE)
            {
                SHR_IF_ERR_EXIT(bcm_dnx_port_loopback_set(unit, logical_port, BCM_PORT_LOOPBACK_MAC));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_EMULATION_1_CORE
#ifdef DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit) != 0)
    {
        LOG_CLI_EX("Skip Fabric for emulation!! %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }
#endif
#endif
    /*
     * init sw state db 
     */
    SHR_IF_ERR_EXIT(dnx_fabric_sw_state_db_init(unit));

    /*
     * Init fabric features
     */
    SHR_IF_ERR_EXIT(dnx_fabric_features_init(unit));

    /*
     * Init fabric ports
     */
    SHR_IF_ERR_EXIT(dnx_fabric_ports_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Deinitialize sw_state database.
 */
static shr_error_e
dnx_fabric_sw_state_db_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deinit sw state db
     */
    SHR_IF_ERR_CONT(dnx_fabric_sw_state_db_deinit(unit));

    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Verify Input parameters of bcm_dnx_fabric_module_control_get(). 
 * Refer to bcm_dnx_fabric_module_control_get() for additional details  
 */
static int
dnx_fabric_module_control_set_verify(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_module_control_t control,
    int value)
{

    SHR_FUNC_INIT_VARS(unit);

    /** High level verify */
    SHR_RANGE_VERIFY(modid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM, "modid out of bound.\n");

    /** Value will be verified per control */
exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - 
 * General function for simple configuration per module ID. 
 *  
 * Use cases: 
 * ** Use case: "FSM Mode"
 * Configure FSM mode.
 * There are two mechanisms which handle FSM:
 * - Delayed FSM - wait before sending a new FSM.
 * - Sequential FSM - add sequence number to FSM control cell.
 * Default is sequential FSM (recommended)
 * Delayed FSM should be set when the remote device is legacy generation (done per destination FAP-ID).
 * Parameters: 
 * - flags - not used, set to 0 
 * - modid - represent FAP-ID 
 * - control - set to bcmFabricModuleControlFSMSeqNumberEnable 
 * - value - 0 - Delayed FSM
 *           1 - Sequential FSM
 *  
 *  ** Use case: ""
 *  TBD
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] flags - description per use case
 * \param [in] modid - module ID (FAP-ID) 
 * \param [in] control - action to preform (see use cases for the supported controls) 
 * \param [in] value - description per use case 
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_module_control_set(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_module_control_t control,
    int value)
{
    dnx_iqs_fsm_mode_e fsm_mode;
    SHR_FUNC_INIT_VARS(unit);
    /** High level verify */
    SHR_INVOKE_VERIFY_DNX(dnx_fabric_module_control_set_verify(unit, flags, modid, control, value));
    /*
     * Use case per control
     */
    switch (control)
    {
            /*
             * Use case: "FSM Mode"
             */
        case bcmFabricModuleControlFSMSeqNumberEnable:
        {
            fsm_mode = (value ? dnx_iqs_fsm_mode_sequential : dnx_iqs_fsm_mode_delayed);
            SHR_IF_ERR_EXIT(dnx_iqs_fsm_mode_set(unit, modid, fsm_mode));
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Control not supported: %d", control);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - 
 * Verify Input parameters of bcm_dnx_fabric_module_control_get(). 
 * Refer to bcm_dnx_fabric_module_control_get() for additional details  
 */
int
dnx_fabric_module_control_get_verify(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_module_control_t control,
    int *value)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(modid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM, "modid out of bound.\n");

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - 
 * General function for simple configuration per module ID. 
 * See description in bcm_dnx_fabric_module_control_set()
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] flags - description per use case
 * \param [in] modid - module ID (FAP-ID) 
 * \param [in] control - action to preform (see use cases for the supported controls) 
 * \param [out] value - description per use case 
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_module_control_get(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_module_control_t control,
    int *value)
{
    dnx_iqs_fsm_mode_e fsm_mode;
    SHR_FUNC_INIT_VARS(unit);
    /** High level verify */
    SHR_INVOKE_VERIFY_DNX(dnx_fabric_module_control_get_verify(unit, flags, modid, control, value));
    /*
     * Use case per control
     */
    switch (control)
    {
            /*
             * Use case: "FSM Mode"
             */
        case bcmFabricModuleControlFSMSeqNumberEnable:
        {
            SHR_IF_ERR_EXIT(dnx_iqs_fsm_mode_get(unit, modid, &fsm_mode));
            *value = (fsm_mode == dnx_iqs_fsm_mode_sequential);
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Control not supported: %d", control);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set link isolation, i.e.
 *   Isolate/Activate a fabric link.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   Fabric link to isolate/activate.
 * \param [in] enable -
 *   Isolate/Activate the link.
 *   1 - isolate the link.
 *   0 - activate the link.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_link_isolation_set(
    int unit,
    int link,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_LINKS_ALLOWED, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ALLOWED_REACHABILITY, INST_SINGLE,
                                 enable ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get link isolation status, i.e.
 *   Get whether a fabric link is isolated/active.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   The fabric link.
 * \param [out] enable -
 *   Whether the link is isolated/active:
 *   1 - the link is isolated.
 *   0 - the link is active.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_link_isolation_get(
    int unit,
    int link,
    int *enable)
{
    uint32 entry_handle_id;
    uint32 is_allowed_reachability = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_LINKS_ALLOWED, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_ALLOWED_REACHABILITY, INST_SINGLE,
                               &is_allowed_reachability);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *enable = is_allowed_reachability ? FALSE : TRUE;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_link_tx_traffic_disable_set(
    int unit,
    int link,
    int disable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Disable/Enable cells
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_LINKS_ALLOWED, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ALLOWED, INST_SINGLE, disable ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * General function for simple fabric configuration. 
 *  
 * Use cases:
 *
 * ** Use case: "watchdog queue range"
 * Range of queues supporting credit watchdog mechanism. 
 * Parameters: 
 * - type - bcmFabricWatchdogQueueMin or bcmFabricWatchdogQueueMax 
 * - arg - Minimum queue / maximum queue in queue range 
 * 
 * ** Use case: "Enable watchdog"
 * Enable / Disable credit watchdog mechanism
 * Parameters: 
 * - type - bcmFabricWatchdogQueueEnable 
 * - arg - Enable / Disable
 *  
 * ** use case: "multicast scheduling mode"
 * Set multicast scheduling mode.
 * Enhance scheduling of fabric multicast queues allows to create enhance scheduling scheme similarly to egress credit scheduling schemes.
 * In this mode, the local credit generate credits to be distributed be the local scheduler.
 * Notes: 
 * - Must be called before creating egress scheduling scheme
 * - Allocating HRs 0-3 (HRs 0-3 could not be used for egress scheduling scheme) 
 * Parameters: 
 * - type - bcmFabricMulticastSchedulerMode 
 * - arg - 1:enable enhance mode, 0:disable enhance mode (0 is the default). 
 *  
 * ** use case: "isolate"
 * isolate / unisolate device from the system.
 * Sequence which safely removes the device from the system (by stopping sending control cells)
 * Withoout any traffic loss.
 * Parameters:
 * - type - bcmFabricIsolate or bcmFabricControlCellsEnable (does the same operation)
 * - arg -  For bcmFabricIsolate: 0 - unisolate, 1 - isolate. 
 *          For bcmFabricControlCellsEnable: 1 - unisolate, 0 - isolate. 
 * ** use case: "FMQs range"
 * Set range for multicast queues (up to max_fmq_id)
 * By default, FMQ range is 0-3. 
 * FMQ range can be expanded using this API. The remaining queues can be used for non-FMQ traffic. 
 * Parameters: 
 * - type - bcmFabricMulticastQueueMax 
 * - arg - Maximal FMQ id. Must be in 2K resolution
 *
 * ** use case: "minimum links to destination"
 * Set minimum links to destination FAP.
 * Will set the min links to ALL the FAPs.
 * Notes:
 * - Setting min links through this function is deprecated, and is maintained
 *   here only for backward compatability reasons.
 * - User is advised to use bcm_fabric_destination_link_min_set instead.
 * Parameters: 
 * - type - bcmFabricMinimalLinksToDestination 
 * - arg - minimum links, must be between 0-nof_fabric_links.
 *
 * ** use case: "single cell delay"
 * Enable/Disable delay of single cell in Fabric RX.
 * Parameters: 
 * - type - bcmFabricDelaySingleCellInFabricRx 
 * - arg - 1: enable delay. 0: disable delay.
 *
 * ** use case: "GCI Leaky Bucket Enable"
 * Enable/Disable GCI Leaky Bucket mechanism.
 * LEGACY - this use case is deprecated and used only for backward compatibility.
 * Calls the new implementation.
 * - type - bcmFabricCgmGciLeakyBucketEn
 * - arg - 1: enable leaky bucket. 0: disable leaky bucket.
 *
 * ** use case: "GCI Random-Backoff Enable"
 * Enable/Disable GCI Random-Backoff mechanism.
 * LEGACY - this use case is deprecated and used only for backward compatibility.
 * Calls the new implementation.
 * - type - bcmFabricCgmGciBackoffEn
 * - arg - 1: enable random-backoff. 0: disable random-backoff.
 *
 * ** Use case: "RCI throttling mode"
 * Set rci throttling mode
 * Parameters: 
 * - type - bcmFabricControlRciThrottlingMode
 * - arg - bcmFabricControlRciThrottlingModeGlobal -- Basic mode (Default mode).
 * -     - bcmFabricControlRciThrottlingModeDeviceBiasing -- Device biasing
 * -     - bcmFabricControlRciThrottlingModeFlowBiasing -- Flow biasing
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] type - action to preform (see use cases for the supported controls) 
 * \param [in] arg - description per use case 
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_control_set(
    int unit,
    bcm_fabric_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Use case per type
     */
    switch (type)
    {
            /*
             * Use case: "watchdog queue range"
             */
        case bcmFabricWatchdogQueueMin:
        {
            /** Verify input paramter */
            SHR_RANGE_VERIFY(arg, 0, dnx_data_ipq.queues.nof_queues_get(unit) - 1, _SHR_E_PARAM,
                             "queue_min out of bound");
            /** update data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_min_sw_set(unit, /* queue-id */ arg));
            /** verify all watchdog parameters*/
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_verify(unit));
            /** set hw accordingly */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_hw_set(unit));
            break;
        }
            /*
             * Use case: "watchdog queue range"
             */
        case bcmFabricWatchdogQueueMax:
        {
            /** verify input paramter */
            SHR_RANGE_VERIFY(arg, 0, dnx_data_ipq.queues.nof_queues_get(unit) - 1, _SHR_E_PARAM,
                             "queue_max out of bound");
            /** update data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_max_sw_set(unit, /* queue-id */ arg));
            /** verify all watchdog parameters */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_verify(unit));
            /** set hw accordingly */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_hw_set(unit));
            break;
        }
            /*
             * Use case: "Enable watchdog"
             */
        case bcmFabricWatchdogQueueEnable:
        {
            /** verify input parameter */
            if (arg != BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE
                && arg != BCM_FABRIC_WATCHDOG_QUEUE_DISABLE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "unexpected arg: expecting BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE or BCM_FABRIC_WATCHDOG_QUEUE_DISABLE.\n");
            }
            /** update data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_enable_sw_set(unit, /* enable/disable */ arg));
            /** verify */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_verify(unit));
            /** set hw accordingly */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_hw_set(unit));
            break;
        }
            /*
             * use case: "multicast scheduling mode"
             */
        case bcmFabricMulticastSchedulerMode:
        {
            /** update data base */
            SHR_IF_ERR_EXIT(dnx_fabric_multicast_scheduler_mode_set(unit, /* scheduler mode - is enahanced */ arg));
            break;
        }
            /*
             * use case: "isolate"
             */
        case bcmFabricIsolate:
        {
            SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_set(unit, arg ? FALSE : TRUE, 0));
            break;
        }
            /*
             * use case: "isolate"
             */
        case bcmFabricControlCellsEnable:
        {
            SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_set(unit, arg ? TRUE : FALSE, 0));
            break;
        }
            /*
             * use case: "FMQs range"
             */
        case bcmFabricMulticastQueueMax:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_fabric_multicast_queue_range_set(unit, arg));
            break;
        }

            /*
             * Throw error for controls not that are not supported in DNX
             */
        case bcmFabricShaperQueueMin:
        case bcmFabricShaperQueueMax:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ISQs are not supported.");
            break;
        }
        case bcmFabricQueueMin:
        case bcmFabricQueueMax:
        case bcmFabricMulticastQueueMin:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Use bcmFabricMulticastQueueMax to get max FMQ range (min FMQ range is 0).");
            break;
        }
            /*
             * use case: "minimum links to destination"
             */
        case bcmFabricMinimalLinksToDestination:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_destination_link_min_set(unit, 0, BCM_MODID_ALL, arg));
            break;
        }
            /*
             * use case: "single cell delay"
             */
        case bcmFabricDelaySingleCellInFabricRx:
        {
            /** Verify input paramter */
            SHR_RANGE_VERIFY(arg, 0, 1, _SHR_E_PARAM, "arg must be either 0 or 1.");

            SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL
                            (unit, mbcm_dnx_fabric_delay_single_cell_in_fabric_rx_enable_set, (unit, arg)));
            break;
        }
            /*
             * use case: "GCI Leaky Bucket Enable"
             */
        case bcmFabricGciLeakyBucketEnable:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, 0, bcmFabricCgmGciLeakyBucketEn, -1, arg));
            break;
        }
            /*
             * use case: "GCI Random-Backoff Enable"
             */
        case bcmFabricGciBackoffEnable:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, 0, bcmFabricCgmGciBackoffEn, -1, arg));
            break;
        }
            /*
             * use case: "RCI throttling mode"
             */
        case bcmFabricControlRciThrottlingMode:
            SHR_IF_ERR_EXIT(dnx_fabric_rci_throttling_mode_set(unit, (bcm_fabric_control_rci_throttling_mode_t) arg));
            break;

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Type not supported: %d", type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * General function for simple fabric configuration. 
 * See bcm_dnx_fabric_control_set() for additional details. 
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] type - action to preform (see use cases for the supported controls) 
 * \param [in] arg - description per use case 
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_control_get(
    int unit,
    bcm_fabric_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Global Verification
     */
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    /*
     * Use case per type
     */
    switch (type)
    {
            /*
             * Use case: "watchdog queue range"
             */
        case bcmFabricWatchdogQueueMin:
        {
            /** update data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_min_sw_get(unit, /* queue-id */ arg));
            break;
        }
            /*
             * Use case: "watchdog queue range"
             */
        case bcmFabricWatchdogQueueMax:
        {
            /** get from sw data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_max_sw_get(unit, /* queue-id */ arg));
            break;
        }
            /*
             * Use case: "Enable watchdog"
             */
        case bcmFabricWatchdogQueueEnable:
        {
            /** get from sw data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_enable_sw_get(unit, /* enable/disable */ arg));
            *arg = *arg ? BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE : BCM_FABRIC_WATCHDOG_QUEUE_DISABLE;
            break;
        }
            /*
             * use case: "multicast scheduling mode"
             */
        case bcmFabricMulticastSchedulerMode:
        {
            /** get from sw data base */
            SHR_IF_ERR_EXIT(dnx_fabric_multicast_scheduler_mode_get(unit, /* scheduler mode - is enahanced */ arg));
            break;
        }
            /*
             * use case: "isolate"
             */
        case bcmFabricIsolate:
        {
            int is_control_cells_enabled = 0;

            SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_get(unit, &is_control_cells_enabled));
            *arg = (is_control_cells_enabled == FALSE) ? 1 : 0;
            break;
        }
            /*
             * use case: "isolate"
             */
        case bcmFabricControlCellsEnable:
        {
            SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_get(unit, arg));
            break;
        }
            /*
             * use case: "FMQs range"
             */
        case bcmFabricMulticastQueueMax:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_fabric_multicast_queue_range_get(unit, arg));
            break;
        }

            /*
             * Throw error for controls not that are not supported in DNX
             */
        case bcmFabricShaperQueueMin:
        case bcmFabricShaperQueueMax:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ISQs are not supported.");
            break;
        }
        case bcmFabricQueueMin:
        case bcmFabricQueueMax:
        case bcmFabricMulticastQueueMin:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Use bcmFabricMulticastQueueMax to get max FMQ range (min FMQ range is 0).");
            break;
        }
            /*
             * use case: "minimum links to destination"
             */
        case bcmFabricMinimalLinksToDestination:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_destination_link_min_get(unit, 0, BCM_MODID_ALL, arg));
            break;
        }
            /*
             * use case: "single cell delay"
             */
        case bcmFabricDelaySingleCellInFabricRx:
        {
            SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL
                            (unit, mbcm_dnx_fabric_delay_single_cell_in_fabric_rx_enable_get, (unit, arg)));
            break;
        }
            /*
             * use case: "GCI Leaky Bucket Enable"
             */
        case bcmFabricGciLeakyBucketEnable:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get(unit, 0, bcmFabricCgmGciLeakyBucketEn, -1, arg));
            break;
        }
            /*
             * use case: "GCI Random-Backoff Enable"
             */
        case bcmFabricGciBackoffEnable:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get(unit, 0, bcmFabricCgmGciBackoffEn, -1, arg));
            break;
        }
            /*
             * use case: "RCI throttling mode"
             */
        case bcmFabricControlRciThrottlingMode:
            SHR_IF_ERR_EXIT(dnx_fabric_rci_throttling_mode_get(unit, (bcm_fabric_control_rci_throttling_mode_t *) arg));
            break;

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Type not supported: %d", type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * General function for simple fabric link configuration.
 *  
 * Use cases:
 *
 * ** Use case: "LLFC Enable"
 * Enable/Disable link's LLFC.
 * Parameters: 
 * - type - bcmFabricLLFControlSource
 * - arg - 1 - enable LLFC. 0 - disable LLFC.
 * 
 * ** Use case: "Link isolation"
 * Isolate/Unisolate link.
 * Parameters: 
 * - type - bcmFabricLinkIsolate
 * - arg - 1 - isolate link. 0 - unisolate link.
 * 
 * ** Use case: "Disable Tx Traffic"
 * Disable/Enable Tx traffic.
 * Parameters: 
 * - type - bcmFabricLinkTxTrafficDisable
 * - arg - 1 - disable Tx traffic. 0 - enable Tx traffic.
 * 
 * ** Use case: "Connect link to a repeater"
 * Connecting the device to a repeater is not supported, and this use case
 * is here only to print error for user.
 * Parameters: 
 * - type - bcmFabricLinkRepeaterEnable
 * 
 * \param [in] unit - Unit-ID 
 * \param [in] link - port number
 * \param [in] type - action to perform (see use cases for the supported controls) 
 * \param [in] arg - description per use case 
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_link_control_set(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int arg)
{
    bcm_port_t logical_port = 0;
    int link_i = 0;
    uint32 nof_links = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * General validation
     */
    /*
     * Get local port from port.
     * Note that param 'link' is actually refering to 'port'.
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, link));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, link, &logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_i));
    nof_links = dnx_data_fabric.links.nof_links_get(unit);
    SHR_RANGE_VERIFY(link_i, 0, nof_links - 1, _SHR_E_PARAM, "Link %d is out-of-range", link_i);

    SHR_RANGE_VERIFY(arg, 0, 1, _SHR_E_PARAM, "Unsupported arg %d", arg);

    switch (type)
    {
            /*
             * use case: "LLFC Enable"
             */
        case bcmFabricLLFControlSource:
        {
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_llfc_enable_hw_set(unit, link_i, arg));
            break;
        }
            /*
             * use case: "Link isolation"
             */
        case bcmFabricLinkIsolate:
        {
            /** Need to stop traffic before isolation */
            if (arg == 1)
            {
                /** Stop tx traffic on the link */
                SHR_IF_ERR_EXIT(dnx_fabric_link_tx_traffic_disable_set(unit, link_i, arg));
            }

            /** Stop transmitting reachability cells on the link */
            SHR_IF_ERR_EXIT(dnx_fabric_link_isolation_set(unit, link_i, arg));

            /** Need to enable traffic after un-isolation */
            if (arg == 0)
            {
                /** Enable tx traffic on the link */
                SHR_IF_ERR_EXIT(dnx_fabric_link_tx_traffic_disable_set(unit, link_i, arg));
            }
            break;
        }
            /*
             * use case: "Disable Tx Traffic"
             */
        case bcmFabricLinkTxTrafficDisable:
        {
            int link_traffic_disable = arg ? 1 : 0;
            int is_port_enabled = 0;

            /*
             * Update sw_state 
             */
            SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_allowed.set(unit, link_i, link_traffic_disable ? 0 : 1));

            /*
             * Update HW only if port is enabled. If port is currently
             * disabled, HW will be updated according to sw_state when
             * port will be enabled again.
             */
            SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, link, &is_port_enabled));
            if (is_port_enabled)
            {
                SHR_IF_ERR_EXIT(dnx_fabric_link_tx_traffic_disable_set(unit, link_i, link_traffic_disable));
            }
            break;
        }
            /*
             * use case: "Connect link to a repeater"
             */
        case bcmFabricLinkRepeaterEnable:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Connecting this device to a repeater is not supported");
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported Type %d", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * General function for simple fabric link configuration. 
 * See bcm_dnx_fabric_link_control_set() for additional details. 
 *  
 * \param [in] unit - Unit-ID 
 * \param [in] link - link number
 * \param [in] type - action to preform (see use cases for the supported controls) 
 * \param [in] arg - description per use case 
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_link_control_get(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int *arg)
{
    bcm_port_t logical_port = 0;
    bcm_port_t link_i;
    uint32 nof_links = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * General validation
     */
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    /*
     * Get local port from port.
     * Note that param 'link' is actually refering to 'port'.
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, link));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, link, &logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_i));
    nof_links = dnx_data_fabric.links.nof_links_get(unit);
    SHR_RANGE_VERIFY(link_i, 0, nof_links - 1, _SHR_E_PARAM, "Link %d is out-of-range", link_i);

    switch (type)
    {
            /*
             * use case: "LLFC Enable"
             */
        case bcmFabricLLFControlSource:
        {
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_llfc_enable_hw_get(unit, link_i, arg));
            break;
        }
            /*
             * use case: "Link isolation"
             */
        case bcmFabricLinkIsolate:
        {
            SHR_IF_ERR_EXIT(dnx_fabric_link_isolation_get(unit, link_i, arg));
            break;
        }
            /*
             * use case: "Disable Tx Traffic"
             */
        case bcmFabricLinkTxTrafficDisable:
        {
            int is_fabric_link_allowed = 0;

            /*
             * The status is taken from sw_state and not from HW since
             * traffic can be conceptualy enabled even though the port
             * is currently disabled.
             */
            SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_allowed.get(unit, link_i, &is_fabric_link_allowed));
            *arg = !is_fabric_link_allowed;
            break;
        }
            /*
             * use case: "Connect link to a repeater"
             */
        case bcmFabricLinkRepeaterEnable:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Connecting this device to a repeater is not supported");
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported Type %d", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_control_cells_disable(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Disable fabric control cells */
    SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_set(unit, bcmFabricControlCellsEnable, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set Fabric CGM GCI thresholds.
 *   This function is deprecated and used only for backward compatibility.
 *   Use bcm_fabric_cgm_control_set instead.
 *  
 * \param [in] unit -
 *   Unit-ID 
 * \param [in] fifo_type -
 *   should be set to -1.
 * \param [in] count -
 *   number of threshold types in 'type' array.
 * \param [in] type -
 *   array of threshold types.
 * \param [in] value -
 *   array of threshold values that match the threshold types array.
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_link_thresholds_set(
    int unit,
    int fifo_type,
    uint32 count,
    bcm_fabric_link_threshold_type_t * type,
    int *value)
{
    int index = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (fifo_type == -1)
    {
        /*
         * special functionality - no fifo_type required
         */
        for (index = 0; index < count; index++)
        {
            switch (type[index])
            {
                    /*
                     * GCI Backoff related thresholds configuration
                     */
                case bcmFabricLinkTxGciLvl1FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel0, value[index]));
                    break;
                }
                case bcmFabricLinkTxGciLvl2FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel1, value[index]));
                    break;
                }
                case bcmFabricLinkTxGciLvl3FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel2, value[index]));
                    break;
                }

                    /*
                     * GCI Leaky bucket flow control thresholds
                     */
                case bcmFabricLinkGciLeakyBucket1Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket0,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket2Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket1,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket3Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket2,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket4Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket3,
                                     value[index]));
                    break;
                }

                    /*
                     * GCI Leaky bucket full thresholds
                     */
                case bcmFabricLinkGciLeakyBucket1Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket0,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket2Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket1,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket3Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket2,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket4Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket3,
                                     value[index]));
                    break;
                }

                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown type %d", type[index]);
                    break;
                }
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Fifo type not supported: %d", fifo_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get Fabric CGM GCI thresholds.
 *   This function is deprecated and used only for backward compatibility.
 *   Use bcm_fabric_cgm_control_get instead.
 *  
 * \param [in] unit -
 *   Unit-ID 
 * \param [in] fifo_type -
 *   should be set to -1.
 * \param [in] count -
 *   number of threshold types in 'type' array.
 * \param [in] type -
 *   array of threshold types.
 * \param [out] value -
 *   array of threshold values that match the threshold types array.
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_link_thresholds_get(
    int unit,
    int fifo_type,
    uint32 count,
    bcm_fabric_link_threshold_type_t * type,
    int *value)
{
    int index = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (fifo_type == -1)
    {
        /*
         * special functionality - no fifo_type required
         */
        for (index = 0; index < count; index++)
        {
            switch (type[index])
            {
                    /*
                     * GCI Backoff related thresholds configuraion
                     */
                case bcmFabricLinkTxGciLvl1FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel0, &(value[index])));
                    break;
                }
                case bcmFabricLinkTxGciLvl2FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel1, &(value[index])));
                    break;
                }
                case bcmFabricLinkTxGciLvl3FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel2, &(value[index])));
                    break;
                }

                    /*
                     * GCI Leaky bucket flow control thresholds
                     */
                case bcmFabricLinkGciLeakyBucket1Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket0,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket2Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket1,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket3Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket2,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket4Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket3,
                                     &(value[index])));
                    break;
                }

                    /*
                     * GCI Leaky bucket full thresholds
                     */
                case bcmFabricLinkGciLeakyBucket1Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket0,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket2Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket1,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket3Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket2,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket4Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket3,
                                     &(value[index])));
                    break;
                }

                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown type %d", type[index]);
                    break;
                }
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Fifo type not supported: %d", fifo_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_dtqs_contexts_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_dtqs_contexts_init, (unit)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set fabric priority according to several parameters
 * \param [in] unit -
 *   Unit id.
 * \param [in] tc -
 *   Traffic Class.
 * \param [in] dp -
 *   Color.
 * \param [in] flags -
 *   Additional parametes. Can be one of the following:
 *   BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY
 *   BCM_FABRIC_PRIORITY_MULTICAST
 *   BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY
 * \param [in] fabric_priority -
 *   The fabric priority to set for the above parameters.
 */
static shr_error_e
dnx_fabric_priority_set(
    int unit,
    uint32 tc,
    uint32 dp,
    uint32 flags,
    int fabric_priority)
{
    uint32 entry_handle_id;
    int is_hp = 0;
    int is_mc = 0;
    int is_ocb_only = 0;
    int is_tdm = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get whether the configured packet is TDM packet
     */
    is_tdm = (dnx_data_fabric.tdm.priority_get(unit) == fabric_priority) ? 1 : 0;

    /*
     * 'tc' and 'dp' keys are given as params.
     * Get additional keys from 'flags' param.
     */
    is_hp = (flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) ? 1 : 0;
    is_mc = (flags & BCM_FABRIC_PRIORITY_MULTICAST) ? 1 : 0;
    is_ocb_only = (flags & BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY) ? 1 : 0;

    /*
     * Fill table with fabric priority in the index found 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CELL_PRIORITY, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HP, is_hp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OCB_ONLY, is_ocb_only);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, INST_SINGLE, fabric_priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get fabric priority according to several parameters
 * \param [in] unit -
 *   Unit id.
 * \param [in] tc -
 *   Traffic Class.
 * \param [in] dp -
 *   Color.
 * \param [in] flags -
 *   Additional parametes. Can be one of the following:
 *   BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY
 *   BCM_FABRIC_PRIORITY_MULTICAST
 *   BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY
 * \param [out] fabric_priority -
 *   The fabric priority to set for the above parameters.
 */
static shr_error_e
dnx_fabric_priority_get(
    int unit,
    uint32 tc,
    uint32 dp,
    uint32 flags,
    int *fabric_priority)
{
    uint32 entry_handle_id;
    int is_hp = 0;
    int is_mc = 0;
    int is_ocb_only = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * The keys 'tc' and 'dp' are given as params.
     * Get additional keys from 'flags' param.
     */
    is_hp = (flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) ? 1 : 0;
    is_mc = (flags & BCM_FABRIC_PRIORITY_MULTICAST) ? 1 : 0;
    is_ocb_only = (flags & BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY) ? 1 : 0;

    /*
     * Retrieve table entry according to keys
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CELL_PRIORITY, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HP, is_hp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OCB_ONLY, is_ocb_only);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, INST_SINGLE,
                               (uint32 *) fabric_priority);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Validate params of bcm_dnx_fabric_priority_set/get()
 */
static int
dnx_fabric_priority_verify(
    int unit,
    uint32 flags,
    bcm_cos_t ingress_pri,
    bcm_color_t color)
{
    uint32 mask =
        BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY | BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY | BCM_FABRIC_PRIORITY_MULTICAST |
        BCM_FABRIC_PRIORITY_TDM | BCM_FABRIC_PRIORITY_OCB_MIX_ONLY | BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY;
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(ingress_pri, 0, DNX_COSQ_NOF_TC - 1, _SHR_E_PARAM, "Invalid ingress_pri parameter %d",
                     ingress_pri);

    SHR_RANGE_VERIFY(color, 0, DNX_COSQ_NOF_DP - 1, _SHR_E_PARAM, "Invalid ingress_dp parameter %d", color);

    SHR_MASK_VERIFY(flags, mask, _SHR_E_PARAM, "Unrecognized flags");

    if (dnx_data_fabric.tdm.priority_get(unit) == -1)
    {
        if (flags & BCM_FABRIC_PRIORITY_TDM)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_FABRIC_PRIORITY_TDM is not supported");
        }
    }

    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) && (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "flags includes both BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY and BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY");
    }

    if ((flags & BCM_FABRIC_PRIORITY_OCB_MIX_ONLY) && (flags & BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "flags includes both BCM_FABRIC_PRIORITY_OCB_MIX_ONLY and BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY");
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_priority_set(
    int unit,
    uint32 flags,
    bcm_cos_t ingress_pri,
    bcm_color_t color,
    int fabric_priority)
{
    int traffic_enabled = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Input validation
     */
    SHR_IF_ERR_EXIT(dnx_fabric_priority_verify(unit, flags, ingress_pri, color));

    /** nof_priorities is 1 based, whereas fabric_priority is 0 based */
    SHR_RANGE_VERIFY(fabric_priority, 0, dnx_data_fabric.cell.nof_priorities_get(unit) - 1, _SHR_E_PARAM,
                     "fabric PKT priority is %d, but it must be between 0 and %d", fabric_priority,
                     dnx_data_fabric.cell.nof_priorities_get(unit) - 1);

    /*
     * Validate traffic is disabled
     */
    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_get(unit, &traffic_enabled));
    if (traffic_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Traffic must be disabled is order to run this API");
    }

    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) == 0 && (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY) == 0)
    {
        /*
         * set both hp and lp
         */
        uint32 flags_high, flags_low;

        flags_high = flags | BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY;
        SHR_IF_ERR_EXIT(dnx_fabric_priority_set(unit, ingress_pri, color, flags_high, fabric_priority));

        flags_low = flags | BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY;
        SHR_IF_ERR_EXIT(dnx_fabric_priority_set(unit, ingress_pri, color, flags_low, fabric_priority));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_priority_set(unit, ingress_pri, color, flags, fabric_priority));
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_priority_get(
    int unit,
    uint32 flags,
    bcm_cos_t ingress_pri,
    bcm_color_t color,
    int *fabric_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Input validation 
     */
    SHR_NULL_CHECK(fabric_priority, _SHR_E_PARAM, "fabric_priority");
    SHR_IF_ERR_EXIT(dnx_fabric_priority_verify(unit, flags, ingress_pri, color));

    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) == 0 && (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY) == 0)
    {
        /*
         * get both hp and lp
         */
        uint32 flags_high, flags_low;
        int fabric_priority_low = 0, fabric_priority_high = 0;

        flags_high = flags | BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY;
        SHR_IF_ERR_EXIT(dnx_fabric_priority_get(unit, ingress_pri, color, flags_high, &fabric_priority_high));

        flags_low = flags | BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY;
        SHR_IF_ERR_EXIT(dnx_fabric_priority_get(unit, ingress_pri, color, flags_low, &fabric_priority_low));

        SHR_VAL_VERIFY(fabric_priority_low, fabric_priority_high, _SHR_E_FAIL,
                       "fabric priority is different for QUEUE_PRIORITY_LOW and QUEUE_PRIORITY_HIGH. use flags BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY or BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY.");

        *fabric_priority = fabric_priority_high;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_priority_get(unit, ingress_pri, color, flags, fabric_priority));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set Fabric PCP (Packet Cell Packing) mode per destination device.
 *   There are 3 supported PCP modes:
 *     0 - No packing.
 *     1 - Simple packing.
 *     2 - Continuous packing.
 */
static shr_error_e
dnx_fabric_pcp_dest_mode_config_set(
    int unit,
    bcm_module_t modid,
    bcm_fabric_pcp_mode_t pcp_mode)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pcp_mode_e fabric_pcp_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Convert bcm enum to corresponding dbal enum
     */
    switch (pcp_mode)
    {
        case bcmFabricPcpModeNoPacking:
        {
            fabric_pcp_mode = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_NO_PACKING;
            break;
        }
        case bcmFabricPcpModeSimple:
        {
            fabric_pcp_mode = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_SIMPLE;
            break;
        }
        case bcmFabricPcpModeContinuous:
        {
            fabric_pcp_mode = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_CONTINUOUS;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown PCP mode\n");
        }
    }

    /*
     * Set pcp mode for modid
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PCP_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, modid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PCP_MODE, INST_SINGLE, fabric_pcp_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get Fabric PCP (Packet Cell Packing) mode per destination device.
 *   There are 3 supported PCP modes:
 *     0 - No packing.
 *     1 - Simple packing.
 *     2 - Continuous packing.
 */
static shr_error_e
dnx_fabric_pcp_dest_mode_config_get(
    int unit,
    bcm_module_t modid,
    bcm_fabric_pcp_mode_t * pcp_mode)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pcp_mode_e fabric_pcp_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get pcp mode for modid
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PCP_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, modid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FABRIC_PCP_MODE, INST_SINGLE, &fabric_pcp_mode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Convert dbal enum to corresponding bcm enum
     */
    switch (fabric_pcp_mode)
    {
        case DBAL_ENUM_FVAL_FABRIC_PCP_MODE_NO_PACKING:
        {
            *pcp_mode = bcmFabricPcpModeNoPacking;
            break;
        }
        case DBAL_ENUM_FVAL_FABRIC_PCP_MODE_SIMPLE:
        {
            *pcp_mode = bcmFabricPcpModeSimple;
            break;
        }
        case DBAL_ENUM_FVAL_FABRIC_PCP_MODE_CONTINUOUS:
        {
            *pcp_mode = bcmFabricPcpModeContinuous;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown fabric PCP mode\n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify params of bcm_fabric_pcp_dest_mode_config_set/get
 */
static shr_error_e
dnx_fabric_pcp_dest_mode_config_verify(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_pcp_mode_config_t * pcp_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "Unrecognized flags");

    SHR_RANGE_VERIFY(modid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM, "modid out of bound.\n");

    SHR_NULL_CHECK(pcp_config, _SHR_E_PARAM, "pcp_config");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_pcp_dest_mode_config_set(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_pcp_mode_config_t * pcp_config)
{
    bcm_core_t core = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.cell.pcp_enable_get(unit) == 1)
    {
        SHR_INVOKE_VERIFY_DNX(dnx_fabric_pcp_dest_mode_config_verify(unit, flags, modid, pcp_config));

        switch (pcp_config->pcp_mode)
        {
            case bcmFabricPcpModeNoPacking:
            case bcmFabricPcpModeSimple:
            case bcmFabricPcpModeContinuous:
            {
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "pcp_mode is invalid\n");
            }
        }

        /*
         * Local modids must be configured with continuous pcp mode
         */
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_local_core_get(unit, modid, &core));
        if (core != -1)
        {
            if (pcp_config->pcp_mode != bcmFabricPcpModeContinuous)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "pcp_mode must be continuous for local modids\n");
            }
        }

        SHR_IF_ERR_EXIT(dnx_fabric_pcp_dest_mode_config_set(unit, modid, pcp_config->pcp_mode));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "fabric_pcp is disabled");
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_pcp_dest_mode_config_get(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_pcp_mode_config_t * pcp_config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.cell.pcp_enable_get(unit) == 1)
    {
        SHR_INVOKE_VERIFY_DNX(dnx_fabric_pcp_dest_mode_config_verify(unit, flags, modid, pcp_config));

        SHR_IF_ERR_EXIT(dnx_fabric_pcp_dest_mode_config_get(unit, modid, &pcp_config->pcp_mode));

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "fabric_pcp is disabled");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_wfq_set(
    int unit,
    int pipe,
    int weight,
    dnx_fabric_wfq_type_e fabric_wfq_type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * configure ingress part (FDT)
     */
    if (fabric_wfq_type != DNX_FABRIC_WFQ_TYPE_PIPE_EGRESS)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_CONTEXTS_WEIGHTS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_ID, pipe);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, weight);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    /*
     * configure Egress part (FDR)
     */
    if (fabric_wfq_type != DNX_FABRIC_WFQ_TYPE_CONTEXT_INGRESS)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_PIPES_WEIGHTS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, pipe);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, weight);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_wfq_get(
    int unit,
    int pipe,
    int *weight,
    dnx_fabric_wfq_type_e fabric_wfq_type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Egress WFQ (FDR)
     */
    if (fabric_wfq_type == DNX_FABRIC_WFQ_TYPE_PIPE_EGRESS)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_PIPES_WEIGHTS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, pipe);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, (uint32 *) weight);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Get Ingress WFQ (FDT)
     * (also when calling with fabric_wfq_type=DNX_FABRIC_WFQ_TYPE_ALL)
     */
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_CONTEXTS_WEIGHTS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_ID, pipe);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, (uint32 *) weight);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_wfq_dynamic_set(
    int unit,
    dbal_enum_value_field_fabric_wfq_fifo_e fabric_wfq_fifo,
    int is_congested,
    int weight)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_DYNAMIC_WEIGHTS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CONGESTED, is_congested);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_WFQ_FIFO, fabric_wfq_fifo);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, weight);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_wfq_dynamic_get(
    int unit,
    dbal_enum_value_field_fabric_wfq_fifo_e fabric_wfq_fifo,
    int is_congested,
    int *weight)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_DYNAMIC_WEIGHTS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CONGESTED, is_congested);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_WFQ_FIFO, fabric_wfq_fifo);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, (uint32 *) weight);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * System RED
 * set discard configuration. Configuration to enable/ disable aging and
 * its settings
 */
shr_error_e
bcm_dnx_fabric_config_discard_set(
    int unit,
    bcm_fabric_config_discard_t * discard)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_system_red_aging_config_set(unit, discard));

exit:
    SHR_FUNC_EXIT;
}

/* get system RED discard configuration. */
shr_error_e
bcm_dnx_fabric_config_discard_get(
    int unit,
    bcm_fabric_config_discard_t * discard)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_system_red_aging_config_get(unit, discard));

exit:
    SHR_FUNC_EXIT;
}

