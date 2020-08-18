/** \file stat_pp.c
 * $Id$
 *  stat module gather all functionality that related to the counters(crps, stif, mib) in DNX.
 *  stat.c holds the functionality that is common for more than one counters sub-module (for example: bcm_stat_control API).
 *  It's matching h file is bcm/stat.h. 
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif /* */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <src/bcm/dnx/stat/crps/crps_mgmt_internal.h>
#include <bcm_int/dnx/algo/stat_pp/algo_stat_pp.h>
#include <soc/dnx/swstate/auto_generated/access/algo_stat_pp_access.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <shared/trunk.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

#define STAT_PP_SW_STATE_RESOURCE_EXEC_BY_ENGINE_PROFILE(engine, exec)\
        engine == bcmStatCounterInterfaceIngressReceivePp ? stat_pp_db.stat_pp_irpp_profile_id.exec :\
        engine == bcmStatCounterInterfaceEgressReceivePp ? stat_pp_db.stat_pp_erpp_profile_id.exec :\
        stat_pp_db.stat_pp_etpp_profile_id.exec\


/*
 * }
 */

/**
 * \brief - Verify function for pp_profile_create API
 * 
 * \param [in] unit - Relevant unit
 * \param [in] flags - flags that were passed to the API
 * \param [in] profile_id - used in case of REPLACE flag being 
 *        set, in which case we check if the profile already
 *        exists before replacing it
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] stat_pp_profile_info - A struct containing the 
 *        profile info to be written
 *   
 * \return
 *   shr_error_e, negative in case of an error. For example:
 *   REPLACE flag is set, and the profile isn't found
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_profile_create_verify(
    int unit,
    int flags,
    int *profile_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_profile_info_t * stat_pp_profile_info)
{
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(profile_id, _SHR_E_PARAM, "profile_id");
    SHR_NULL_CHECK(stat_pp_profile_info, _SHR_E_PARAM, "stat_pp_profile_info");

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            if (stat_pp_profile_info->meter_command_id != STAT_PP_METER_COMMAND_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "metering is invalid for ingress, meter_command_id must be 0 (was %d)",
                             stat_pp_profile_info->meter_command_id);
            }
            if (stat_pp_profile_info->meter_qos_map_id != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "meter_qos_map_id is invalid for ingress, must be 0 (was %d)",
                             stat_pp_profile_info->meter_qos_map_id);
            }
            break;

        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            if (stat_pp_profile_info->is_fp_cs_var != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "is_fp_cs_var must be off for egress (was %d)",
                             stat_pp_profile_info->is_fp_cs_var);
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unknown engine_source option");
            break;
    }

    if ((stat_pp_profile_info->stat_object_type < 0) ||
        (stat_pp_profile_info->stat_object_type >= BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stat object type given:%d. Should be between 0 and %d",
                     stat_pp_profile_info->stat_object_type, BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_STAT_PP_PROFILE_REPLACE))
    {
        SHR_IF_ERR_EXIT(STAT_PP_SW_STATE_RESOURCE_EXEC_BY_ENGINE_PROFILE(engine_source, is_allocated
                                                                         (unit, *profile_id, &is_allocated)));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Profile id %d doesn't exist\n", *profile_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for get, delete pp profiles APIs 
 * Validate that the profile exists before get or delete it 
 * 
 * \param [in] unit - Relevant unit
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] profile_id - statistics profile id
 *   
 * \return
 *   shr_error_e, Negative in case of an error. For example -
 *   profile is not found
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_profile_get_delete_verify(
    int unit,
    uint32 engine_source,
    uint32 profile_id)
{
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(STAT_PP_SW_STATE_RESOURCE_EXEC_BY_ENGINE_PROFILE(engine_source, is_allocated
                                                                     (unit, profile_id, &is_allocated)));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Profile id %d doesn't exist\n", profile_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set statistic profile properties in relevant dbal 
 *        tables: 
 *          DBAL_TABLE_IRPP_STATISTIC_PROFILE_INFO
 *          DBAL_TABLE_ERPP_STATISTIC_PROFILE_INFO
 *          DBAL_TABLE_ETPP_STATISTIC_PROFILE_INFO
 * \param [in] unit - Relevant unit
 * \param [in] stat_pp_profile_info - A struct containing the 
 *        relevant profile info to be written
 * \param [in] stat_pp_profile - profile id, serves as key to 
 *        the dbal tables
 * \param [in] engine_source - counting engine, used to 
 *        distinguish between the tables
 *   
 * \return
 *    Negative in case of an error. See \ref shr_error_e,
 *     for example: dbal failure
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_stat_pp_profile_hw_set(
    int unit,
    bcm_stat_pp_profile_info_t * stat_pp_profile_info,
    int stat_pp_profile,
    bcm_stat_counter_interface_type_t engine_source)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write values to stat_pp dbal tables according to type
     */
    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, stat_pp_profile);
            dbal_entry_value_field32_set(unit,
                                         entry_handle_id,
                                         DBAL_FIELD_STAT_IF_OBJECT_ID,
                                         INST_SINGLE, stat_pp_profile_info->counter_command_id);
            dbal_entry_value_field32_set(unit,
                                         entry_handle_id,
                                         DBAL_FIELD_STAT_IF_TYPE_ID, INST_SINGLE,
                                         stat_pp_profile_info->stat_object_type);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_METER, INST_SINGLE,
                                         stat_pp_profile_info->is_meter_enable);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPING_REQUIRED, INST_SINGLE,
                                         stat_pp_profile_info->is_fp_cs_var);
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, stat_pp_profile);
            dbal_entry_value_field32_set(unit,
                                         entry_handle_id,
                                         DBAL_FIELD_STAT_IF_OBJECT_ID,
                                         INST_SINGLE, stat_pp_profile_info->counter_command_id);
            dbal_entry_value_field32_set(unit,
                                         entry_handle_id,
                                         DBAL_FIELD_STAT_IF_TYPE_ID, INST_SINGLE,
                                         stat_pp_profile_info->stat_object_type);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_METER, INST_SINGLE,
                                         stat_pp_profile_info->is_meter_enable);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_INTERFACE, INST_SINGLE,
                                         stat_pp_profile_info->meter_command_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_QOS_PROFILE, INST_SINGLE,
                                         DNX_QOS_MAP_PROFILE_GET(stat_pp_profile_info->meter_qos_map_id));
            break;

        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unknown engine_source option");
            break;
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See stat_pp.h file for summary
 */
shr_error_e
dnx_stat_pp_result_type_verify(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 dbal_result_type)
{
    dbal_table_field_info_t dbal_info;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Make sure that the result type chosen has the statistics fields inside it
     */
    rv = (dbal_tables_field_info_get_no_err
          (unit, dbal_table_id, DBAL_FIELD_STAT_CMD, 0,
           dbal_result_type, 0, &dbal_info) == _SHR_E_NONE)
        ||
        (dbal_tables_field_info_get_no_err
         (unit, dbal_table_id, DBAL_FIELD_STAT_OBJECT_CMD, 0, dbal_result_type, 0, &dbal_info) == _SHR_E_NONE);

    if (rv)
    {
        SHR_EXIT();
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify gport stat set api: crps range validation, 
 *          core mode validation, profile range and engine source
 * 
 * \param [in] unit - Relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] core_id - core id
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e,
 *           for example: unknown engine_source
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_gport_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t stat_info)
{
    dbal_core_mode_e core_mode;
    uint8 is_physical_port;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags;
    uint8 is_ingress;
    dbal_tables_e dbal_table_id;
    uint32 dbal_table_result_type;
    dbal_table_field_info_t dbal_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));

    if (engine_source != STAT_PP_PROFILE_ENGINE_GET(stat_info.stat_pp_profile))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Engine source %d is inconsistent with stat_info.stat_pp_profile engine %d",
                     engine_source, stat_info.stat_pp_profile);
    }

    is_ingress = FALSE;
    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            is_ingress = TRUE;
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unknown engine_source option");
            break;
    }

    SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                    (unit, core_id, stat_info.stat_id, stat_info.stat_pp_profile, engine_source));

    if (is_physical_port == 0)
    {
        if (BCM_GPORT_IS_VLAN_PORT(gport) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
        {
            /*
             * The LIF is Egress Virtual, only valid for ETPP:
             */
            if (engine_source != bcmStatCounterInterfaceEgressTransmitPp)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, gport = 0x%08X is Egress Virtual but engine_source = %d is not bcmStatCounterInterfaceEgressTransmitPp (%d)\n",
                             gport, engine_source, bcmStatCounterInterfaceEgressTransmitPp);
            }

            
        }
        else
        {
            if (BCM_GPORT_IS_MPLS_PORT(gport))
            {
                
                uint8 is_stat_cmd_valid;
                dbal_physical_tables_e physical_table_id = DBAL_PHYSICAL_TABLE_NONE;

                /** get lifs */
                lif_flags =
                    is_ingress ? DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS :
                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));
                if (is_ingress)
                {
                    lif_mapping_local_lif_info_t local_lif_info;
                    if (BCM_GPORT_IS_VLAN_PORT(gport) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport))
                    {
                        /** Virtual native vlan lifs always use INLIF-2. */
                        physical_table_id = DBAL_PHYSICAL_TABLE_INLIF_2;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_global_to_local_get
                                        (unit, DNX_ALGO_LIF_INGRESS, gport_hw_resources.global_in_lif,
                                         &local_lif_info));
                        physical_table_id = local_lif_info.phy_table;
                    }
                }
                SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                                (unit, core_id,
                                 (is_ingress) ? gport_hw_resources.local_in_lif : gport_hw_resources.local_out_lif,
                                 is_ingress, physical_table_id, DBAL_FIELD_STAT_OBJECT_CMD, &is_stat_cmd_valid));
                /*
                 * Make sure that the result type chosen has the statistics fields inside it
                 */
                if (is_stat_cmd_valid == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Entry type with no statistics. Make sure the statistics flag is on in the lif creation API");
                }
            }
            else
            {
                
                 /** get lifs */
                if (is_ingress)
                {
                    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
                }
                else
                {
                    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
                }
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));
                dbal_table_id =
                    (is_ingress) ? gport_hw_resources.inlif_dbal_table_id : gport_hw_resources.outlif_dbal_table_id;
                dbal_table_result_type =
                    (is_ingress) ? gport_hw_resources.
                    inlif_dbal_result_type : gport_hw_resources.outlif_dbal_result_type;
                SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));

                if ((core_mode == DBAL_CORE_MODE_DPC) && (core_id != 0) && (core_id != 1))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Core %d, must be 0\1 in DPC mode", core_id);
                }
                else if ((core_mode == DBAL_CORE_MODE_SBC) && (core_id != DBAL_CORE_ALL))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Core %d, must be set to CORE_ALL %d", core_id, DBAL_CORE_ALL);
                }

                /*
                 * Make sure that the result type chosen has the statistics fields inside it
                 */
                if ((dbal_tables_field_info_get_no_err
                     (unit, dbal_table_id, DBAL_FIELD_STAT_CMD, 0,
                      dbal_table_result_type, 0, &dbal_info) == _SHR_E_NOT_FOUND)
                    &&
                    (dbal_tables_field_info_get_no_err
                     (unit, dbal_table_id, DBAL_FIELD_STAT_OBJECT_CMD, 0,
                      dbal_table_result_type, 0, &dbal_info) == _SHR_E_NOT_FOUND))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Entry type with no statistics. Make sure the statistics flag is on in the lif creation API");
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify gport stat get api
 * 
 * \param [in] unit - relevant unit
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_gport_get_delete_verify(
    int unit,
    bcm_stat_counter_interface_type_t engine_source)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if ((engine_source != bcmStatCounterInterfaceIngressReceivePp) &&
        (engine_source != bcmStatCounterInterfaceEgressTransmitPp))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported engine source %d", engine_source);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - retreive pp port from physical gport
 * 
 * \param [in] unit - relevant unit
 * \param [in] gport - physical port
 * \param [in] core_id - core id
 * \param [out] internal_pp_port - pp port found
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_port_get_intenal_pp_port(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    int *internal_pp_port)
{
    int core;
    int nof_cores;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    for (core = 0; core < nof_cores; core++)
    {
        if (core_id == gport_info.internal_port_pp_info.core_id[core])
        {
            break;
        }
    }

    if (core_id != gport_info.internal_port_pp_info.core_id[core])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport not defined on core %d", core_id);
    }

    *internal_pp_port = gport_info.internal_port_pp_info.pp_port[core];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - called by stat gport set api in case of a physical 
 *        port. Write the statistic data to the ingress port or
 *        egress port dbal table according to the engine_source
 * 
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] core_id - core id
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_physical_gport_set(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t stat_info)
{
    uint32 entry_handle_id;
    int internal_pp_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp port */
    SHR_IF_ERR_EXIT(dnx_stat_pp_port_get_intenal_pp_port(unit, gport, core_id, &internal_pp_port));

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported engine_source");
            break;
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, internal_pp_port);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, stat_info.stat_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                 STAT_PP_PROFILE_ID_GET(stat_info.stat_pp_profile));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - called by stat gport get api in case of a physical 
 *        port. Read the statistic data from the relevant pp
 *        port dbal table (ingress or egress)
 * 
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] core_id - core id
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_physical_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t * stat_info)
{
    uint32 entry_handle_id;
    uint32 stat_pp_profile;
    int internal_pp_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get PP Port 
     */
    SHR_IF_ERR_EXIT(dnx_stat_pp_port_get_intenal_pp_port(unit, gport, core_id, &internal_pp_port));

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
            break;
        case bcmStatCounterInterfaceEgressTransmitPp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
            break;
        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported engine_source");
            break;
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, internal_pp_port);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, &stat_info->stat_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));
    STAT_PP_ENGINE_PROFILE_SET(stat_info->stat_pp_profile, stat_pp_profile, engine_source);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - called by stat logical gport set api in case the logical 
 *        port is Egress Virtual.
 *        The function writes the statistic data to the lif
 *        dbal table (ESEM, native and non-native).
 * 
 * \param [in] unit - relevant unit
 * \param [in] gport - egress virtual logical port
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *  dnx_stat_pp_logical_gport_set
 */

static shr_error_e
dnx_stat_pp_logical_gport_egress_virtual_set(
    int unit,
    bcm_gport_t gport,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t * stat_info)
{
    uint32 sw_table_handle_id;
    uint32 esem_handle_id;
    uint32 entry_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));

    SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_ac_match_info_get(unit, gport, sw_table_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &entry_type));

    
    switch (entry_type)
    {
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC:
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB, &esem_handle_id));

            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_native_key_set(unit, sw_table_handle_id, esem_handle_id));

            /*
             * When setting ESEM table, must set also the result-type:
             */
            dbal_entry_value_field32_set(unit, esem_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB_ETPS_AC_STAT);

            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC:
        {

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &esem_handle_id));

            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_key_set(unit, sw_table_handle_id, esem_handle_id));

            /*
             * When setting ESEM table, must set also the result-type:
             */
            dbal_entry_value_field32_set(unit, esem_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_AC_STAT);

            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! gport = 0x%08X is ESM Namespace-vsi which does not support statistics. \n", gport);

            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT:
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &esem_handle_id));

            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_namespace_port_key_set
                            (unit, sw_table_handle_id, esem_handle_id));

            /*
             * When setting ESEM table, must set also the result-type:
             */
            dbal_entry_value_field32_set(unit, esem_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_AC_STAT);

            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error!, accessing ESM SW DB table (%d) with the key gport = 0x%08X but"
                         " resultType = %d is not supported.\n",
                         DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, gport, entry_type);

    }

    /*
     * Set the statistic values:
     */
    dbal_entry_value_field32_set(unit, esem_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, stat_info->stat_id);
    dbal_entry_value_field32_set(unit, esem_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                 STAT_PP_PROFILE_ID_GET(stat_info->stat_pp_profile));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, esem_handle_id, DBAL_COMMIT_UPDATE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - called by stat logical gport get api in case the logical 
 *        port  is Egress Virtual.
 *        This function reads the statistic data from the relevant lif
 *        dbal table (ESEM, native and non-native)
 * 
 * \param [in] unit - relevant unit
 * \param [in] gport - egress virtual logical port
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   dnx_stat_pp_logical_gport_get
 */
static shr_error_e
dnx_stat_pp_logical_gport_egress_virtual_get(
    int unit,
    bcm_gport_t gport,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t * stat_info)
{
    uint32 sw_table_handle_id;
    uint32 esem_handle_id;
    uint32 entry_type;
    uint32 stat_pp_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));

    SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_ac_match_info_get(unit, gport, sw_table_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &entry_type));

    
    switch (entry_type)
    {
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC:
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB, &esem_handle_id));

            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_native_key_set(unit, sw_table_handle_id, esem_handle_id));

            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC:
        {

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &esem_handle_id));

            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_key_set(unit, sw_table_handle_id, esem_handle_id));

            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! gport = 0x%08X is ESM Namespace-vsi which does not support statistics. \n", gport);

            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT:
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &esem_handle_id));

            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_namespace_port_key_set
                            (unit, sw_table_handle_id, esem_handle_id));

            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error!, accessing ESM SW DB table (%d) with the key gport = 0x%08X but"
                         " resultType = %d is not supported.\n",
                         DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, gport, entry_type);

    }

    /*
     * Get the statistic values:
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, esem_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, esem_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, &stat_info->stat_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, esem_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));
    STAT_PP_ENGINE_PROFILE_SET(stat_info->stat_pp_profile, stat_pp_profile, engine_source);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - called by stat gport set api in case of a logical 
 *        port. Write the statistic data to the relevant lif
 *        dbal table
 * 
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_logical_gport_set(
    int unit,
    bcm_gport_t gport,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t stat_info)
{
    uint32 stat_id = stat_info.stat_id;
    int stat_pp_profile = stat_info.stat_pp_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Handle Virtual Egress (native and non-native)
     */
    if (BCM_GPORT_IS_VLAN_PORT(gport) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_logical_gport_egress_virtual_set(unit, gport, engine_source, &stat_info));
    }
    else
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 lif_flags;
        int lif;
        dbal_tables_e dbal_table_id;
        dbal_fields_e dbal_table_key;
        uint32 res_type;
        uint32 entry_handle_id;

        /** get local and global lifs. The verify should have already checked that the lif is valid. */
        lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS
            | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));

        switch (engine_source)
        {
            case bcmStatCounterInterfaceIngressReceivePp:
                lif = gport_hw_resources.local_in_lif;
                dbal_table_id = gport_hw_resources.inlif_dbal_table_id;
                dbal_table_key = DBAL_FIELD_IN_LIF;
                res_type = gport_hw_resources.inlif_dbal_result_type;
                break;
            case bcmStatCounterInterfaceEgressTransmitPp:
                lif = gport_hw_resources.local_out_lif;
                dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
                dbal_table_key = DBAL_FIELD_OUT_LIF;
                res_type = gport_hw_resources.outlif_dbal_result_type;
                if ((dnx_data_stat.stat_pp.etpp_reversed_stat_cmd_get(unit) == 1) &&
                    (res_type != DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_PHP_STAT))
                {
                    
                    if (STAT_PP_PROFILE_ID_GET(stat_pp_profile) > STAT_PP_ETPP_SWITCH_PROFILE_MAX_PROFILE)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "ETPP lif profiles range in A0 are 1-3");
                    }
                    STAT_PP_ETPP_SWITCH_CMD_SET(stat_id, stat_pp_profile);
                }
                break;
            case bcmStatCounterInterfaceEgressReceivePp:
                
                SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported engine_source");
                break;
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, dbal_table_key, lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(stat_pp_profile));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - called by stat gport get api in case of a logical 
 *        port. Read the statistic data from the relevant lif
 *        dbal table
 * 
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_stat_pp_logical_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t * stat_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Handle Virtual Egress (native and non-native)
     */
    if (BCM_GPORT_IS_VLAN_PORT(gport) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_logical_gport_egress_virtual_get(unit, gport, engine_source, stat_info));
    }
    else
    {

        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 lif_flags;
        int lif;
        dbal_tables_e dbal_table_id;
        dbal_fields_e dbal_table_key;
        uint32 res_type;
        uint32 entry_handle_id;
        uint32 stat_pp_profile;

        switch (engine_source)
        {
            case bcmStatCounterInterfaceIngressReceivePp:
                lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
                break;
            case bcmStatCounterInterfaceEgressTransmitPp:
                lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
                break;
            case bcmStatCounterInterfaceEgressReceivePp:
                
                SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported engine_source");
                break;
        }

        /** get local and global out lifs */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));

        switch (engine_source)
        {
            case bcmStatCounterInterfaceIngressReceivePp:
                lif = gport_hw_resources.local_in_lif;
                dbal_table_id = gport_hw_resources.inlif_dbal_table_id;
                dbal_table_key = DBAL_FIELD_IN_LIF;
                res_type = gport_hw_resources.inlif_dbal_result_type;
                break;
            case bcmStatCounterInterfaceEgressTransmitPp:
                lif = gport_hw_resources.local_out_lif;
                dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
                dbal_table_key = DBAL_FIELD_OUT_LIF;
                res_type = gport_hw_resources.outlif_dbal_result_type;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Shouldn't get here!");
                break;
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type);
        dbal_entry_key_field32_set(unit, entry_handle_id, dbal_table_key, lif);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id,
                                                            DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                            &stat_info->stat_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));

        if ((engine_source == bcmStatCounterInterfaceEgressTransmitPp) &&
            (dnx_data_stat.stat_pp.etpp_reversed_stat_cmd_get(unit) == 1) &&
            (res_type != DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_PHP_STAT))
        {
            
            STAT_PP_ETPP_SWITCH_CMD_GET(stat_info->stat_id, stat_pp_profile);
        }
        STAT_PP_ENGINE_PROFILE_SET(stat_info->stat_pp_profile, stat_pp_profile, engine_source);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See stat_pp.h file for summary
 */
int
dnx_stat_pp_crps_verify(
    int unit,
    bcm_core_t core_id,
    uint32 stat_id,
    int stat_pp_profile,
    int interface_type)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    uint32 type_id;
    uint32 counter_command;
    int profile_id;
    uint32 is_meter;
    uint32 max_pp_profile_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**Exit in case of invalid pp profile*/
    if (stat_pp_profile == STAT_PP_PROFILE_INVALID)
    {
        SHR_EXIT();
    }

    if (interface_type == bcmStatCounterInterfaceIngressReceivePp)
    {
        dbal_table_id = DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO;
        max_pp_profile_value = STAT_PP_IRPP_PROFILE_MAX_VALUE;
    }
    else if (interface_type == bcmStatCounterInterfaceEgressReceivePp)
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
    }
    else if (interface_type == bcmStatCounterInterfaceEgressTransmitPp)
    {
        dbal_table_id = DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO;
        max_pp_profile_value = STAT_PP_ETPP_PROFILE_MAX_VALUE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported engine source");
    }

    profile_id = STAT_PP_PROFILE_ID_GET(stat_pp_profile);
    if (profile_id > max_pp_profile_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal pp profile value %d", profile_id);
    }
    /** Get statistic type stored in HW for crps validation */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID, INST_SINGLE,
                                                        &type_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IS_METER, INST_SINGLE, &is_meter));

    /** CRPS validation */
    /** if is_meter=TRUE, it doesn't have to be configure CRPS engine. therefore, skip validation */
    if (is_meter == FALSE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, INST_SINGLE,
                                                            &counter_command));
        SHR_INVOKE_VERIFY_DNX(dnx_crps_mgmt_counter_generation_verify
                              (unit, core_id, interface_type, counter_command, type_id, stat_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See stat_pp.h file for summary
 */
shr_error_e
dnx_stat_pp_init(
    int unit)
{
    uint32 entry_handle_id;
    int stat_pp_profile;
    int statistic_if;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    stat_pp_profile = STAT_PP_PROFILE_INVALID;
    statistic_if = STAT_PP_STAT_IF_NO_STATISTICS_INDICATION;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, stat_pp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, INST_SINGLE, statistic_if);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, stat_pp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, INST_SINGLE, statistic_if);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * APIs
 * {
 */

/**
 * \brief - Delete a statistic profile according to the profile 
 *        id. Use the source engine, which is taken from the 2
 *        MSBs of the stat_pp_profile to choose the relevant
 *        dbal table and resource manager. Use the profile id as
 *        key to the dbal table
 * 
 * \param [in] unit - Relevant unit
 * \param [in] stat_pp_profile - An integer, containing the 
 *        profile id and the engine source
 *   
 * \return
 *    *   Negative in case of an error. See \ref shr_error_e,
 *           for example: profile is not found
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_pp_profile_delete(
    int unit,
    int stat_pp_profile)
{
    uint32 entry_handle_id;
    uint32 engine_source;
    uint32 profile_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    engine_source = STAT_PP_PROFILE_ENGINE_GET(stat_pp_profile);
    profile_id = STAT_PP_PROFILE_ID_GET(stat_pp_profile);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_profile_get_delete_verify(unit, engine_source, profile_id));

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, &entry_handle_id));
            break;

        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;

        case bcmStatCounterInterfaceEgressTransmitPp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, &entry_handle_id));
            break;

        default:
            break;
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(STAT_PP_SW_STATE_RESOURCE_EXEC_BY_ENGINE_PROFILE(engine_source, free_single(unit, profile_id)));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Map statistic profile to profile properties 
 * Profile id is saved in designated resource manager, profile 
 * info in dbal tables: IRPP_STATISTIC_PROFILE_INFO, 
 * ERPP_STATISTIC_PROFILE_INFO, ETPP_STATISTIC_PROFILE_INFO
 * \param [in] unit - Relevant unit
 * \param [in] flags - BCM_STAT_PP_PROFILE_WITH_ID - allocate with id
 *     BCM_STAT_PP_PROFILE_REPLACE - replace existing profile
 * \param [in] engine_source - counting source: IRPP, ERPP, ETPP
 * \param [in,out] stat_pp_profile - A pointer to an integer containing the profile id.
 *     Can be either passed empty (without id) or valid (with_id
 *     or replace flags are on). engine_source will be encoded
 *     on 2 MSBs
 * \param [in] stat_pp_profile_info - A struct contatining the profile info to be saved and
 *     mapped 
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e,
 *           for example: REPLACE flag is on, but profile id
 *           doesn't exist.
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_pp_profile_create(
    int unit,
    int flags,
    bcm_stat_counter_interface_type_t engine_source,
    int *stat_pp_profile,
    bcm_stat_pp_profile_info_t * stat_pp_profile_info)
{
    int algo_flags;
    uint32 profile_id;

    SHR_FUNC_INIT_VARS(unit);
    algo_flags = 0;

    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(int), stat_pp_profile);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_profile_create_verify
                          (unit, flags, stat_pp_profile, engine_source, stat_pp_profile_info));

    if ((_SHR_IS_FLAG_SET(flags, BCM_STAT_PP_PROFILE_WITH_ID))
        || (_SHR_IS_FLAG_SET(flags, BCM_STAT_PP_PROFILE_REPLACE)))
    {
        algo_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
    }

    if (!(_SHR_IS_FLAG_SET(flags, BCM_STAT_PP_PROFILE_REPLACE)))
    {
        SHR_IF_ERR_EXIT(STAT_PP_SW_STATE_RESOURCE_EXEC_BY_ENGINE_PROFILE(engine_source, allocate_single
                                                                         (unit, algo_flags, NULL, stat_pp_profile)));
    }

    profile_id = *stat_pp_profile;
    STAT_PP_ENGINE_PROFILE_SET(*stat_pp_profile, profile_id, engine_source);
    SHR_IF_ERR_EXIT(dnx_stat_pp_profile_hw_set(unit, stat_pp_profile_info, profile_id, engine_source));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get profile_info according to profile index 
 * Use the engine source to choose the relevant dbal table and 
 * resource manager resource, and the profile id as key to the 
 * dbal table 
 * 
 * \param [in] unit - Relevant unit
 * \param [in] stat_pp_profile - statistics profile id, from 
 *        which the profile id is retreived, along with the
 *        engine source
 * \param [in] stat_pp_profile_info - A strcut that will be 
 *        filled with the retreived profile info
 *   
 * \return
 *   Negative in case of an error. See \ref shr_error_e,
 *           for example: profile is not found
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_pp_profile_get(
    int unit,
    int stat_pp_profile,
    bcm_stat_pp_profile_info_t * stat_pp_profile_info)
{
    uint32 entry_handle_id;
    uint32 engine_source;
    uint32 profile_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stat_pp_profile_info, _SHR_E_PARAM, "stat_pp_profile_info");
    sal_memset(stat_pp_profile_info, 0, sizeof(bcm_stat_pp_profile_info_t));

    engine_source = STAT_PP_PROFILE_ENGINE_GET(stat_pp_profile);
    profile_id = STAT_PP_PROFILE_ID_GET(stat_pp_profile);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_profile_get_delete_verify(unit, engine_source, profile_id));

    switch (engine_source)
    {
        case bcmStatCounterInterfaceIngressReceivePp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, profile_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_IF_OBJECT_ID,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->counter_command_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_IF_TYPE_ID,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->stat_object_type));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit,
                                                               entry_handle_id,
                                                               DBAL_FIELD_IS_METER,
                                                               INST_SINGLE, &stat_pp_profile_info->is_meter_enable));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit,
                                                               entry_handle_id,
                                                               DBAL_FIELD_MAPPING_REQUIRED,
                                                               INST_SINGLE, &stat_pp_profile_info->is_fp_cs_var));
            break;

        case bcmStatCounterInterfaceEgressTransmitPp:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, profile_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_IF_OBJECT_ID,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->counter_command_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_IF_TYPE_ID,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->stat_object_type));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit,
                                                               entry_handle_id,
                                                               DBAL_FIELD_IS_METER,
                                                               INST_SINGLE, &stat_pp_profile_info->is_meter_enable));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_METER_INTERFACE,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->meter_command_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_METER_QOS_PROFILE,
                                                                INST_SINGLE,
                                                                (uint32 *) &stat_pp_profile_info->meter_qos_map_id));
            break;

        case bcmStatCounterInterfaceEgressReceivePp:
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP counting engine is not supported");
            break;

        default:
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map physical or logical port to stat id and stat 
 *        profile. Incase of DPC LIF the api can accept core
 *        parameter to distinguish between the stat id on the
 *        two cores
 * 
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] core_id - core id
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *   
 * \return
 *   Negative in case of an error 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_gport_stat_set(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t stat_info)
{
    uint8 is_physical_port;

    SHR_FUNC_INIT_VARS(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_gport_set_verify(unit, gport, core_id, engine_source, stat_info));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));

    if (is_physical_port)
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_physical_gport_set(unit, gport, core_id, engine_source, stat_info));
    }
    /** gport is lif */
    else
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_logical_gport_set(unit, gport, engine_source, stat_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get mapping of physical or logical port to stat id 
 *        and stat profile. In case of DPC LIF the api can
 *        accept core parameter to distinguish between the stat
 *        id on the two cores
 * 
 * \param [in] unit - relevant unit
 * \param [in] gport - physical or logical port
 * \param [in] core_id - core id
 * \param [in] engine_source - counting source (IRPP, ERPP, 
 *        ETPP)
 * \param [in] stat_info - statistic information per object
 *   
 * \return
 *   Negative in case of an error 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_gport_stat_get(
    int unit,
    bcm_gport_t gport,
    bcm_core_t core_id,
    bcm_stat_counter_interface_type_t engine_source,
    bcm_stat_pp_info_t * stat_info)
{
    uint8 is_physical_port;

    SHR_FUNC_INIT_VARS(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_stat_pp_gport_get_delete_verify(unit, engine_source));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));
    if (is_physical_port)
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_physical_gport_get(unit, gport, core_id, engine_source, stat_info));
    }
    /** gport is lif */
    else
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_logical_gport_get(unit, gport, engine_source, stat_info));
    }

exit:
    SHR_FUNC_EXIT;
}
