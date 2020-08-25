/** \file stk/stk_sys.c
 *
 * Switch system procedures.
 *
 * FAP_ID, System Ports, Module Ports ...
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STK
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/stack.h>
#include <bcm/trunk.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx_dispatch.h>

#include <soc/dnx/dbal/dbal.h>

#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/fabric/fabric_control_cells.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stack.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#include <soc/dnx/legacy/TMC/tmc_api_fabric.h>

#include <shared/shrextend/shrextend_debug.h>

/*
 * }
 */

/*************
 * DEFINES   *
 *************/
/* { */

/*
 * Max modid is translated in HW to be (HW_field_val+1)*32-1
 */
#define DNX_STK_SYS_MODULE_BASE_INDEX_TO_MODID_GET(hw_field_val)  ((hw_field_val + 1) * dnx_data_fabric.reachability.resolution_get(unit) - 1)
#define DNX_STK_SYS_MODULE_MODID_TO_BASE_INDEX_GET(modid)  (modid / dnx_data_fabric.reachability.resolution_get(unit))

/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_fmq_sysport_set(
    int unit,
    bcm_core_t core,
    uint32 my_modid)
{
    uint32 sysport_id;
    uint32 tm_port;

    SHR_FUNC_INIT_VARS(unit);

    /** get dedicated FMQ system port id */
    sysport_id = dnx_data_device.general.fmq_system_port_get(unit);

    /** PP-DSP is Irrelevant, it's overridden by HW to 0Xff */
    tm_port = dnx_data_port.general.nof_tm_ports_get(unit) - 1;

    /** Map sysport to modport - modid should be set to local fap id*/
    SHR_IF_ERR_EXIT(dnx_stk_sys_sysport_modport_map_set(unit, core, sysport_id, my_modid, tm_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_modid_set(
    int unit,
    bcm_core_t core,
    uint32 modid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_CFG, &entry_handle_id));

    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, INST_SINGLE, modid);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    if (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_FE)
    {
        if (core == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_CLOS_DQCQ_MAP, &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID_LOCAL_0, INST_SINGLE, modid);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASK_LOCAL_0, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }
        else if (core == 1)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_CLOS_DQCQ_MAP, &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID_LOCAL_1, INST_SINGLE, modid);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASK_LOCAL_1, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected core %d.\n", core);
        }
    }
    else        /* mesh mode */
    {
        uint32 mask;

        SHR_BITSET_RANGE(&mask, 0, utilex_msb_bit_on(dnx_data_device.general.nof_faps_get(unit)));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_MESH_DQCQ_MAP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID_LSB, modid & mask);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_MESH_CONTEXT, INST_SINGLE,
                                     DBAL_ENUM_FVAL_SYSTEM_MESH_CONTEXT_LOCAL_0 + core);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_modid_get(
    int unit,
    bcm_core_t core,
    uint32 *modid)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_CFG, &entry_handle_id));

    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Get value
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FAP_ID, INST_SINGLE, modid);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_modid_local_core_get(
    int unit,
    uint32 modid,
    bcm_core_t * core)
{
    int core_index;
    uint32 core_modid;
    SHR_FUNC_INIT_VARS(unit);

    /** init core to non local */
    *core = -1;

    /** look for a core with the same modid */
    for (core_index = 0; core_index < dnx_data_device.general.nof_cores_get(unit); core_index++)
    {
        /** get core_index modid */
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get(unit, core_index, &core_modid));
        if (core_modid == modid)
        {
            *core = core_index;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_logical_port_to_sysport_map_set(
    int unit,
    bcm_port_t logical_port,
    uint32 sysport)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    uint32 tm_port;
    uint32 pp_port;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Set TM mapping
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    /** key construction */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /**
     * when mapping between a logical port to system port it is assumed that:
     *                  TM_PORT == PP_DSP
     * and no conversion between the two is required
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, tm_port);

    /** Set value */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, sysport);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * Set PP mapping
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));
    if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_type))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, logical_port, &core, &pp_port));
        SHR_IF_ERR_EXIT(dnx_port_pp_src_system_port_set(unit, core, pp_port, sysport));
        SHR_IF_ERR_EXIT(dnx_port_dsp_table_mapping_set(unit, tm_port, core, pp_port, sysport));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_logical_port_to_sysport_map_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *sysport)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    uint32 tm_port;
    int is_spa;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get tm port and core */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
    /** key construction */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /**
     * when mapping between a logical port to system port it is assumed that:
     *                  TM_PORT == PP_DSP
     * and no conversion between the two is required
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, tm_port);

    /** Get value */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, sysport);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** if received system port is system port aggregate */
    SHR_IF_ERR_EXIT(dnx_trunk_system_port_is_spa(unit, *sysport, &is_spa));
    if (is_spa)
    {
        bcm_gport_t sysport_gport;
        /** map SPA to system port */
        SHR_IF_ERR_EXIT(bcm_dnx_trunk_spa_to_system_phys_port_map_get(unit, 0, *sysport, &sysport_gport));
        *sysport = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_sysport_map_clear(
    int unit,
    uint32 sysport)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    uint32 tm_port;
    uint32 sysport_i;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        for (tm_port = 0; tm_port < dnx_data_port.general.nof_tm_ports_get(unit); tm_port++)
        {
            /*
             * get from dbal
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
            /** key construction */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /**
             * when mapping between a logical port to system port it is assumed that:
             *                  TM_PORT == PP_DSP
             * and no conversion between the two is required
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, tm_port);

            /** Get value */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, &sysport_i);

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);

            if (sysport_i == sysport)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_PP_DSP_MAPPING, &entry_handle_id));
                /** key construction */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
                /**
                 * when mapping between a logical port to system port it is assumed that:
                 *                  TM_PORT == PP_DSP
                 * and no conversion between the two is required
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, tm_port);

                /** Set value */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE,
                                             dnx_data_device.general.invalid_system_port_get(unit));

                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                DBAL_HANDLE_FREE(unit, entry_handle_id);

            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_sysport_modport_map_set(
    int unit,
    int core,
    uint32 sysport,
    uint32 modid,
    uint32 tm_port)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_SYSPORT_MODPORT_MAP, &entry_handle_id));
    /** key construction */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DST_SYS_PORT_ID, sysport);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Set value */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, INST_SINGLE, modid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, INST_SINGLE, tm_port);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_sysport_modport_map_get(
    int unit,
    uint32 sysport,
    uint32 *modid,
    uint32 *tm_port)
{
    uint32 entry_handle_id;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_SYSPORT_MODPORT_MAP, &entry_handle_id));
    /** key construction */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DST_SYS_PORT_ID, sysport);
    /** core is irrelevant as configuration is symmetric */
    core = 0;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Get value */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FAP_ID, INST_SINGLE, modid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TM_PORT, INST_SINGLE, tm_port);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_traffic_disable(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Disable traffic */
    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_set(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_traffic_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * HW blocks should be disabled/enabled in a sequential order to properly stop traffic propogation. 
     * Current sequence is taken from dpp 
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_TRAFFIC_ENABLER, &entry_handle_id));

    if (enable == FALSE) /** disable traffic */
    {
        /** Ingress pipe*/
        {
            /** Disable new incoming traffic - IRE block */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                       DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_IRE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, FALSE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Disable internal traffic (enqueue/dequeue packets and control commands) - CGM block */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                       DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_CGM);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, FALSE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Disable internal traffic - SPB (SRAM) block */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                       DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_SPB);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, FALSE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        }
        /** FDR disabling **/
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_TRAFFIC_ENABLE, entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_FIELD_1, INST_SINGLE, 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_FIELD_2, INST_SINGLE, 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_FIELD_3, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
        /** Egress pipe */
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SYSTEM_TRAFFIC_ENABLER, entry_handle_id));
            /** Disable popping new packets from fabric - EGQ block */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                       DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_EGQ);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, FALSE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Disable accepting new fabric status messages (FSMs) - SCH block */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                       DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_SCH_FSM_PROCESS);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, FALSE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Disable credits generation - SCH block */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                       DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_SCH_CREDIT_GENERATION);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, FALSE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Addittional SCH configuration **/
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SCH_CONFIGURATION, entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DLM_ENABLE, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        }
    }
    else
    {
        /**
         * Decide whether the device should be in standalone mode
         * according to received control cells.
         * This should be done before enabling traffic.
         */
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_standalone_state_modify(unit));

        /** Enable popping new packets from fabric - EGQ block */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                   DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_EGQ);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Enable internal traffic - SPB (SRAM) block */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                   DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_SPB);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Enable internal traffic (enqueue/dequeue packets and control commands) - CGM block */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                   DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_CGM);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Fabric traffic enabling 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_TRAFFIC_ENABLE, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_FIELD_1, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_FIELD_2, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_FIELD_3, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Enable new incoming traffic - IRE block */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SYSTEM_TRAFFIC_ENABLER, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                   DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_IRE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Additional SCH configuration **/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SCH_CONFIGURATION, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DLM_ENABLE, INST_SINGLE, 7);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Enable accepting new fabric status messages (FSMs) - SCH block */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SYSTEM_TRAFFIC_ENABLER, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                   DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_SCH_FSM_PROCESS);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Enable credits generation - SCH block */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                                   DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_SCH_CREDIT_GENERATION);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_sys_traffic_enable_get(
    int unit,
    int *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_TRAFFIC_ENABLER, &entry_handle_id));

    /*
     * It's sufficient to see if IRE is enabled to know if traffic is enabled.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_TRAFFIC_STAGE,
                               DBAL_ENUM_FVAL_SYSTEM_TRAFFIC_STAGE_IRE);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_stk_sys_module_all_reachable_ignore_id_set(
    int unit,
    bcm_module_t module,
    int enable)
{
    uint32 entry_handle_id;
    uint32 index = 0;
    uint32 max_index = 0;
    uint32 first_empty = -1;
    uint32 is_excluded = 0;
    bcm_module_t fap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_MODULE_MC_FAP_ID_EXCLUDE, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, DBAL_TABLE_SYSTEM_MODULE_MC_FAP_ID_EXCLUDE, DBAL_FIELD_INDEX, TRUE, 0, 0,
                     DBAL_PREDEF_VAL_MAX_VALUE, &max_index));

    for (index = 0; index <= max_index; ++index)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FAP_ID, INST_SINGLE, (uint32 *) &fap_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_EXCLUDED, INST_SINGLE, &is_excluded);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /** The register is holding a fap id which is ignored */
        if (is_excluded == 1)
        {
            if (fap_id == module)
            {
                /** need to clear this register */
                if (enable == 0)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, INST_SINGLE, 0);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_EXCLUDED, INST_SINGLE, 0);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

                    SHR_EXIT();
                }
                /** fap id is already ignored */
                else
                {
                    SHR_EXIT();
                }
            }
        }
        /** Haven't found a place in the array to configure the module id*/
        else if (first_empty == -1 && enable == 1)
        {
            first_empty = index;
        }
    }

    /** there was nothing to clear */
    if (enable == 0)
    {
        SHR_EXIT();
    }

    /** No empty place to ignore the new fap id */
    if (first_empty == -1)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Can't ignore this FAP ID - 16 FAP IDs are already ignored \n");
    }
    /** Ignore FAP ID*/
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, first_empty);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, INST_SINGLE, module);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_EXCLUDED, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_stk_sys_module_all_reachable_ignore_id_get(
    int unit,
    bcm_module_t module,
    int *enable)
{
    uint32 entry_handle_id;
    uint32 index = 0;
    uint32 max_index = 0;
    uint32 is_excluded = 0;
    bcm_module_t fap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *enable = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_MODULE_MC_FAP_ID_EXCLUDE, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, DBAL_TABLE_SYSTEM_MODULE_MC_FAP_ID_EXCLUDE, DBAL_FIELD_INDEX, TRUE, 0, 0,
                     DBAL_PREDEF_VAL_MAX_VALUE, &max_index));

    for (index = 0; index <= max_index; ++index)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FAP_ID, INST_SINGLE, (uint32 *) &fap_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_EXCLUDED, INST_SINGLE, &is_excluded);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (module == fap_id)
        {
            *enable |= is_excluded;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Validate that the modid the user want to set as maximum for all-rachable vector
 *   is not bigger than maximum allowed modid in the system.
 */
static shr_error_e
dnx_stk_sys_module_max_all_reachable_verify(
    int unit,
    soc_module_t max_all_reachable_module)
{
    soc_module_t max_module = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_stk_sys_module_max_get(unit, &max_module));
    if (max_all_reachable_module > max_module)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Max Fap Id for all reachable vector can't be higher than Max number of Faps in the system configured\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_stk_sys_module_max_all_reachable_set(
    int unit,
    soc_module_t max_module)
{
    uint32 entry_handle_id;
    uint32 base_index = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    base_index = DNX_STK_SYS_MODULE_MODID_TO_BASE_INDEX_GET(max_module);

    SHR_IF_ERR_EXIT(dnx_stk_sys_module_max_all_reachable_verify
                    (unit, DNX_STK_SYS_MODULE_BASE_INDEX_TO_MODID_GET(base_index)));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_MODULE_MAX_BASE_INDEX, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_BASE_INDEX_MC, INST_SINGLE, base_index);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_stk_sys_module_max_all_reachable_get(
    int unit,
    bcm_module_t * max_module)
{
    uint32 entry_handle_id;
    uint32 base_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_MODULE_MAX_BASE_INDEX, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_BASE_INDEX_MC, INST_SINGLE, &base_index);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *max_module = DNX_STK_SYS_MODULE_BASE_INDEX_TO_MODID_GET(base_index);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Validate that the modid the user want to set as maximum modid in the system
 *   is not lower than maximum modid for all-reachable vector.
 *   If this is the case:
 *     1. Update maximum modid for all-reachable vector to be the
 *        same as the new maimum modid in the system.
 *     2. Print warning about the change to the user.
 */
static shr_error_e
dnx_stk_sys_module_max_verify(
    int unit,
    soc_module_t max_module)
{
    soc_module_t max_all_reachable_module = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_stk_sys_module_max_all_reachable_get(unit, &max_all_reachable_module));
    if (max_module < max_all_reachable_module)
    {
        SHR_IF_ERR_EXIT(dnx_stk_sys_module_max_all_reachable_set(unit, max_module));
        LOG_WARN(BSL_LS_SOC_FABRIC,
                 (BSL_META_U(unit, "Max Fap Id for all reachable vector is now equal to Max Fap Id\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_stk_sys_module_max_set(
    int unit,
    soc_module_t max_module)
{
    uint32 entry_handle_id;
    uint32 base_index = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    base_index = DNX_STK_SYS_MODULE_MODID_TO_BASE_INDEX_GET(max_module);

    SHR_IF_ERR_EXIT(dnx_stk_sys_module_max_verify(unit, DNX_STK_SYS_MODULE_BASE_INDEX_TO_MODID_GET(base_index)));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_MODULE_MAX_BASE_INDEX, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_BASE_INDEX, INST_SINGLE, base_index);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_stk_sys_module_max_get(
    int unit,
    soc_module_t * max_module)
{
    uint32 entry_handle_id;
    uint32 base_index = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_MODULE_MAX_BASE_INDEX, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_BASE_INDEX, INST_SINGLE, &base_index);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *max_module = DNX_STK_SYS_MODULE_BASE_INDEX_TO_MODID_GET(base_index);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
