/**
 * \file fabric_mesh.c $Id$ Fabric Mesh Configuration for DNX.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm/error.h>
#include <bcm/fabric.h>

#include <bcm_int/common/multicast.h>

#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/fabric/fabric_mesh.h>
#include <bcm_int/dnx/legacy/fabric.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_fabric_access.h>

#include <shared/utilex/utilex_u64.h>
#include <soc/dnx/legacy/TMC/tmc_api_fabric.h>
#include <soc/dnx/legacy/JER2/jer2_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/sand/sand_aux_access.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>

#define DNX_FABRIC_SYS_FREQ_RES_19 (1 << 19)

/**
 * \brief
 *   Verify local destination id, which is actually the group id
 *   after removing the group's bit using BCM_FABRIC_GROUP_MODID_UNSET.
 * \param [in] unit         - *   The unit number.
 * \param [in] destination  - *   Destination, that should be group.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_local_dest_id_verify(
    int unit,
    bcm_module_t destination)
{
    bcm_module_t local_dest_id;
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_FABRIC_MODID_IS_GROUP(destination))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "destination should be a group");
    }
    local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(destination);

    if (local_dest_id < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid dest_id");
    }

    if (local_dest_id >= dnx_data_fabric.mesh.nof_max_dest_get(unit))
    {
        int is_mesh_mc = dnx_data_fabric.mesh.multicast_enable_get(unit);

        SHR_ERR_EXIT(_SHR_E_PARAM, "in %s mode there can only be %d destinations",
                     (is_mesh_mc ? "Mesh MC" : "Mesh"), dnx_data_fabric.mesh.nof_max_dest_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_fabric_mesh_init(
    int unit)
{
    uint32 is_single_fap, is_mesh, is_mesh_mc;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_single_fap = (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_SINGLE_FAP);
    is_mesh = (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_MESH);
    is_mesh_mc = dnx_data_fabric.mesh.multicast_enable_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_CONFIG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_MODE, INST_SINGLE,
                                 (is_mesh || is_single_fap) ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_MC_ENABLE, INST_SINGLE,
                                 ((is_mesh || is_single_fap) && !is_mesh_mc) ? 0 : 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_MC_BMP_SRC, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_MULTI_FAP, INST_SINGLE,
                                 (is_mesh || is_single_fap) ? 1 : 0);
    if (is_mesh)
    {
        int index;
        uint32 invalid_modid;
        for (index = 0; index < DNX_FABRIC_MESH_DEVICE_ID_NOF; index++)
        {
            invalid_modid = dnx_data_device.general.invalid_fap_id_get(unit);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_MODE_DEV_ID, index, invalid_modid);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set devices ids of Mesh system.
 * \param [in] unit -
 *   The unit number.
 * \param [in] local_dest -
 *   Mesh local destination index.
 *   Use '-1' for local device.
 * \param [in] modid_count -
 *   Number of modids in modid_array.
 * \param [in] modid_array -
 *   The modids to set.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e
dnx_fabric_mesh_device_id_set(
    int unit,
    int local_dest,
    int modid_count,
    bcm_module_t * modid_array)
{
    int device_id_index;
    int field_index;
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (modid_count > DNX_FABRIC_MESH_DEVICE_ID_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "modid_count is too big for mesh device configuration\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_CONFIG, &entry_handle_id));

    for (device_id_index = 0; device_id_index < modid_count; device_id_index++)
    {
        field_index = (local_dest + 1) * nof_cores + device_id_index;
        if (field_index > DNX_FABRIC_MESH_DEVICE_ID_NOF)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "field_index is too big for mesh device configuration\n");
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_MODE_DEV_ID, field_index,
                                     modid_array[device_id_index]);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Associate between the group (local_dest) and the module-ids in it.
 * \param [in] unit         - *   The unit number.
 * \param [in] group        - *   The group id.
 * \param [in] modid_count  - *   Number of modids in modid_array param.
 * \param [in] modid_array  - *   Array of modids to be associated with local_dest group.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * dnx_fabric_modid_group_get.
 */
static shr_error_e
dnx_fabric_modid_group_set(
    int unit,
    bcm_module_t group,
    int modid_count,
    bcm_module_t * modid_array)
{
    int modid_index;
    bcm_module_t local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(group);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_db.modids_to_group_map.nof_modids.set(unit, local_dest_id, modid_count));
    for (modid_index = 0; modid_index < modid_count; ++modid_index)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_db.modids_to_group_map.modids.set(unit, local_dest_id, modid_index,
                                                                     modid_array[modid_index]));
    }
    /*
     * Set modids in HW.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_mesh_device_id_set(unit, local_dest_id, modid_count, modid_array));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the modids associated with a group.
 * \param [in]  unit            - *   The unit number.
 * \param [in]  group           - *   The group id.
 * \param [in]  modid_max_count - *   The length of the given modid_array.
 * \param [out] modid_array     - *   Array of modids associated with local_dest group.
 * \param [out] modid_count     - *   Number of actual modids in modid_array.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * dnx_fabric_modid_group_set.
 */
static shr_error_e
dnx_fabric_modid_group_get(
    int unit,
    bcm_module_t group,
    int modid_max_count,
    bcm_module_t * modid_array,
    int *modid_count)
{
    int modid_index;
    int id_count = 0;
    bcm_module_t local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(group);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_db.modids_to_group_map.nof_modids.get(unit, local_dest_id, &id_count));
    if (modid_max_count < id_count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "modid_array is too small to contain all FAP id's");
    }

    *modid_count = id_count;

    for (modid_index = 0; modid_index < id_count; modid_index++)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_db.modids_to_group_map.modids.get(unit, local_dest_id, modid_index,
                                                                     &modid_array[modid_index]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set per mc_id to which destinations to replicate.
 * \param [in] unit         - The unit number.
 * \param [in] mc_id        - The multicast id.
 * \param [in] destid_count - Number of destinations in destid_array.
 * \param [in] destid_array - Array of destinations to replicate to.
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_multicast_set(
    int unit,
    bcm_multicast_t mc_id,
    uint32 destid_count,
    bcm_module_t * destid_array)
{
    int dest;
    uint32 mc_rep = 0;
    bcm_module_t local_dest;
    uint32 my_cores_fap_ids[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES] = { 0 };
    uint32 core;
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);

    DBAL_FUNC_INIT_VARS(unit);

    for (core = 0; core < nof_cores; ++core)
    {
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get(unit, core, &my_cores_fap_ids[core]));
    }

    for (dest = 0; dest < destid_count; ++dest)
    {
        /** Group destination */
        if (BCM_FABRIC_MODID_IS_GROUP(destid_array[dest]))
        {
            local_dest = BCM_FABRIC_GROUP_MODID_UNSET(destid_array[dest]);
            switch (local_dest)
            {
                case 0:
                {
                    mc_rep |= DNX_FABRIC_MESH_MC_REPLICATION_DEST_0_BIT;
                    break;
                }
                case 1:
                {
                    mc_rep |= DNX_FABRIC_MESH_MC_REPLICATION_DEST_1_BIT;
                    break;
                }
                case 2:
                {
                    mc_rep |= DNX_FABRIC_MESH_MC_REPLICATION_DEST_2_BIT;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "dest %d is invalid", destid_array[dest]);
                }
            }
        }
        /** Local core destination */
        else
        {
            for (core = 0; core < nof_cores; ++core)
            {
                if (destid_array[dest] == my_cores_fap_ids[core])
                {
                    if (core == 0)
                    {
                        mc_rep |= DNX_FABRIC_MESH_MC_REPLICATION_LOCAL_0_BIT;
                    }
                    else if (core == 1)
                    {
                        mc_rep |= DNX_FABRIC_MESH_MC_REPLICATION_LOCAL_1_BIT;
                    }
                }
            }
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_MC_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_MC_ID, mc_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_MC_REP, INST_SINGLE, mc_rep);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get per mc_id which destinations gets replicated.
 * \param [in] unit             - The unit number.
 * \param [in] mc_id            - The multicast id.
 * \param [in] destid_count_max - Size of the given destid_array.
 * \param [out] destid_count    - Number of destinations in destid_array.
 * \param [out] destid_array    - Array of destinations which gets replicated.
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_multicast_get(
    int unit,
    bcm_multicast_t mc_id,
    uint32 destid_count_max,
    int *destid_count,
    bcm_module_t * destid_array)
{
    uint32 mc_rep = 0;
    uint32 my_cores_fap_ids[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES] = { 0 };
    bcm_module_t *pdest = destid_array;
    bcm_module_t dest = 0;
    uint32 core;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *destid_count = 0;

    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); ++core)
    {
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get(unit, core, &my_cores_fap_ids[core]));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_MC_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_MC_ID, mc_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Get MC_REP from entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MESH_MC_REP,
                                                        INST_SINGLE, &mc_rep));

    SHR_BITCOUNT_RANGE(&mc_rep, *destid_count, 0, DNX_FABRIC_MESH_MC_REPLICATION_LENGTH);

    if (destid_count_max < *destid_count)
    {
        *destid_count = 0;
        SHR_ERR_EXIT(_SHR_E_PARAM, "destid_array is too short to contain all dest id's");
    }

    if (mc_rep & DNX_FABRIC_MESH_MC_REPLICATION_DEST_0_BIT)
    {
        dest = 0;
        *pdest = BCM_FABRIC_GROUP_MODID_SET(dest);
        ++pdest;
    }
    if (mc_rep & DNX_FABRIC_MESH_MC_REPLICATION_DEST_1_BIT)
    {
        dest = 1;
        *pdest = BCM_FABRIC_GROUP_MODID_SET(dest);
        ++pdest;
    }
    if (mc_rep & DNX_FABRIC_MESH_MC_REPLICATION_DEST_2_BIT)
    {
        dest = 2;
        *pdest = BCM_FABRIC_GROUP_MODID_SET(dest);
        ++pdest;
    }
    if (mc_rep & DNX_FABRIC_MESH_MC_REPLICATION_LOCAL_0_BIT)
    {
        *pdest = my_cores_fap_ids[0];
        ++pdest;
    }
    if (dnx_data_device.general.nof_cores_get(unit) == 2)
    {
        if (mc_rep & DNX_FABRIC_MESH_MC_REPLICATION_LOCAL_1_BIT)
        {
            *pdest = my_cores_fap_ids[1];
            ++pdest;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Set/Unset links topology.
 * \param [in] unit          - The unit number.
 * \param [in] local_dest_id - The destination id.
 * \param [in] links_count   - Number of links in links_array.
 * \param [in] links_array   - Array of links to be mapped to local_dest_id.
 * \param [in] enable -
 *                      1 - Set links topology to local_dest_id.
 *                      0 - Unset links topology to local_dest_id.
 */
static shr_error_e
dnx_fabric_link_topology_set(
    int unit,
    bcm_module_t group,
    int links_count,
    bcm_port_t * links_array,
    int enable)
{
    int i_link;
    uint32 local_core0_id = 0;
    bcm_module_t dest_fap_id_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES] = { 0 };
    int fap_id_count = 0;
    bcm_module_t local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(group);
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Map local_dest_id with its links
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_REPLICATION_LINK_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_DEST_ID, local_dest_id);

    if (enable == TRUE)
    {
        int fabric_link;
        for (i_link = 0; i_link < links_count; ++i_link)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, links_array[i_link], &fabric_link));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_LINK_ENABLE, fabric_link, TRUE);
        }
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_LINK_ENABLE, INST_ALL, FALSE);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Update modid to context map
     */
    SHR_IF_ERR_EXIT(dnx_fabric_modid_group_get
                    (unit, local_dest_id, DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES, dest_fap_id_arr, &fap_id_count));

    if (fap_id_count > 0)
    {
        /** The first 2 contexts are for local EGQ-0 and EGQ-1 */
        uint32 group_ctx = (enable) ? (local_dest_id + 2) : 0;
        int i_fap_id;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_FAP_ID_TO_CONTEXT_MAP, entry_handle_id));

        for (i_fap_id = 0; i_fap_id < fap_id_count; ++i_fap_id)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP_CONTEXT,
                                         dest_fap_id_arr[i_fap_id], group_ctx);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get(unit, 0, &local_core0_id));
    if (dest_fap_id_arr[0] > local_core0_id)
    {
        uint32 nof_links = dnx_data_fabric.links.nof_links_get(unit);
        int is_empty = 1;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, entry_handle_id));
        for (i_link = 0; i_link < links_count; ++i_link)
        {
            int fabric_link;
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, links_array[i_link], &fabric_link));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, fabric_link);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable ? 0 : 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, entry_handle_id));
        is_empty = 1;
        for (i_link = 0; i_link < nof_links; ++i_link)
        {
            uint32 is_enabled = 0;

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, i_link);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &is_enabled);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (is_enabled == 1)
            {
                is_empty = 0;
                break;
            }
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_INIT, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_CONFIG_1, INST_SINGLE,
                                     is_empty ? 0x0 : 0x2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /*
         * FMAC Does not work in emulation 
         */
        if (soc_sand_is_emulation_system(unit) == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, entry_handle_id));
            for (i_link = 0; i_link < links_count; ++i_link)
            {
                int fabric_link;

                SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, links_array[i_link], &fabric_link));

                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, fabric_link);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_0, INST_SINGLE, enable);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_1, INST_SINGLE, !enable);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get association of links with a destination.
 * \param [in] unit             - The unit number.
 * \param [in] group            - The destination id.
 * \param [in] max_links_count  - Size of the given links_array array.
 * \param [out] links_count     - Number of received links in links_array.
 * \param [out] links_array     - Array of links associated with local_dest_id.
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_link_topology_get(
    int unit,
    bcm_module_t group,
    int max_links_count,
    int *links_count,
    bcm_port_t * links_array)
{
    int i_link;
    int nof_links = dnx_data_fabric.links.nof_links_get(unit);
    bcm_port_t link_arr[DNX_DATA_MAX_FABRIC_LINKS_NOF_LINKS];
    uint32 entry_handle_id;
    bcm_module_t local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(group);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *links_count = 0;

    /*
     * Get links bitmap associated with local_dest_id
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_REPLICATION_LINK_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_DEST_ID, local_dest_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Count valid links
     */
    for (i_link = 0; i_link < nof_links; i_link++)
    {
        uint32 status;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MESH_LINK_ENABLE,
                                                            i_link, &status));
        if (status == TRUE)
        {
            bcm_port_t logical_port;
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_to_logical_get(unit, i_link, &logical_port));
            link_arr[*links_count] = logical_port;
            ++(*links_count);
        }
    }
    /*
     * Update array if sufficient for all links
     */
    if (max_links_count < *links_count)
    {
        *links_count = 0;
        SHR_ERR_EXIT(_SHR_E_PARAM, "links_array is too short to contain all links");
    }
    else
    {
        for (i_link = 0; i_link < *links_count; ++i_link)
        {
            links_array[i_link] = link_arr[i_link];
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify routine for associated BCM API
 * \param [in] unit        - The unit number.
 * \param [in] destination - The destination id.
 * \param [in] links_count - Number of links in links_array.
 * \param [in] links_array - Array of links to be mapped to local_dest_id.
 * \return
 *   See \ref shr_error_e
 */
static int
dnx_fabric_link_topology_set_verify(
    int unit,
    bcm_module_t destination,
    int links_count,
    bcm_port_t * links_array)
{
    int nof_fabric_links, i;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Validate links_count
     */
    if (links_count < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "links_count should be 0 and up");
    }
    else if (links_count != 0)
    {
        SHR_NULL_CHECK(links_array, _SHR_E_PARAM, "links_array");
    }
    /*
     * Validate mesh mode
     */
    if ((dnx_data_fabric.general.connect_mode_get(unit) != SOC_DNX_FABRIC_CONNECT_MODE_MESH))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "bcm_fabric_link_topology_set is only valid in mesh mode");
    }

    nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);
    if (links_count > nof_fabric_links)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "links_count value is too big");
    }
    /*
     * Validate links in links_array are valid
     */
    for (i = 0; i < links_count; ++i)
    {
        /*
         * dnx_algo_port_valid_verify
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_type_verify(unit, links_array[i], DNX_ALGO_PORT_TYPE_FABRIC));
    }
    /*
     * Validate destination
     */
    SHR_IF_ERR_EXIT(dnx_fabric_local_dest_id_verify(unit, destination));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Associate links with destination.
 * \param [in] unit        - The unit number.
 * \param [in] destination - The destination id.
 * \param [in] links_count - Number of links in links_array.
 * \param [in] links_array - Array of links to be mapped to local_dest_id.
 * \return
 *   See \ref shr_error_e
 */
int
bcm_dnx_fabric_link_topology_set(
    int unit,
    bcm_module_t destination,
    int links_count,
    bcm_port_t * links_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_link_topology_set_verify(unit, destination, links_count, links_array));

    /*
     * Set links topology
     */
    if (links_count == 0)
    {
        int unset_links_count = 0;
        bcm_module_t unset_links_array[DNX_DATA_MAX_FABRIC_LINKS_NOF_LINKS] = { 0 };
        int nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);
        /*
         * Get links needed to remove
         */
        SHR_IF_ERR_EXIT(dnx_fabric_link_topology_get(unit, destination, nof_fabric_links, &unset_links_count,
                                                     unset_links_array));
        SHR_IF_ERR_EXIT(dnx_fabric_link_topology_set(unit, destination, unset_links_count, unset_links_array, 0));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_link_topology_set(unit, destination, links_count, links_array, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify routine for associated BCM API
 * \param [in] unit             - The unit number.
 * \param [in] destination      - The destination group.
 * \param [in] max_links_count  - Size of the given links_array array.
 * \param [out] links_count     - Number of received links in links_array.
 * \param [out] links_array     - Array of links associated with local_dest_id.
 * \return
 *   See \ref shr_error_e
 */
static int
dnx_fabric_link_topology_get_verify(
    int unit,
    bcm_module_t destination,
    int max_links_count,
    int *links_count,
    bcm_port_t * links_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(links_array, _SHR_E_PARAM, "links_array");
    /*
     * Validate max_links_count
     */
    if (max_links_count < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal max_links_count");
    }
    else if (max_links_count != 0)
    {
        SHR_NULL_CHECK(links_count, _SHR_E_PARAM, "links_count");
    }
    /*
     * Validate mesh mode
     */
    if ((dnx_data_fabric.general.connect_mode_get(unit) != SOC_DNX_FABRIC_CONNECT_MODE_MESH))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "bcm_fabric_link_topology_get is only valid in mesh mode");
    }
    /*
     * Validate destination
     */
    SHR_IF_ERR_EXIT(dnx_fabric_local_dest_id_verify(unit, destination));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get association of links with a destination.
 * \param [in] unit             - The unit number.
 * \param [in] destination      - The destination group.
 * \param [in] max_links_count  - Size of the given links_array array.
 * \param [out] links_count     - Number of received links in links_array.
 * \param [out] links_array     - Array of links associated with local_dest_id.
 * \return
 *   See \ref shr_error_e
 */
int
bcm_dnx_fabric_link_topology_get(
    int unit,
    bcm_module_t destination,
    int max_links_count,
    int *links_count,
    bcm_port_t * links_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_link_topology_get_verify(unit, destination, max_links_count, links_count,
                                                              links_array));
    /*
     * Get links topology
     */
    SHR_IF_ERR_EXIT(dnx_fabric_link_topology_get(unit, destination, max_links_count, links_count, links_array));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify routine for associated BCM API
 * \param [in] unit         - The unit number.
 * \param [in] mc_id        - The multicast id.
 * \param [in] flags        - Flags (not in use)
 * \param [in] destid_count - Number of destinations in destid_array.
 * \param [in] destid_array - Array of destinations to replicate to.
 * \return
 *   See \ref shr_error_e
 */
static int
dnx_fabric_multicast_set_verify(
    int unit,
    bcm_multicast_t mc_id,
    uint32 flags,
    uint32 destid_count,
    bcm_module_t * destid_array)
{
    int dest = 0;
    uint32 is_mesh = FALSE;
    uint32 is_tdm_supported = dnx_data_fabric.tdm.priority_get(unit) != -1;
    uint32 is_mesh_mc = dnx_data_fabric.mesh.multicast_enable_get(unit);
    uint32 max_mesh_replication = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(destid_array, _SHR_E_PARAM, "destid_array");

    /** activate mesh-mc for mesh and single fap topology */
    if ((dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_MESH) ||
        (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_SINGLE_FAP))
    {
        is_mesh = TRUE;
    }
    /*
     * Validate device mode
     */
    if (!is_mesh || (!is_mesh_mc && !is_tdm_supported))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "bcm_fabric_multicast_set is supported only in MESH mode with TDM support or in MESH_MC mode");
    }
    /*
     * Validate mc_id
     */
    SHR_RANGE_VERIFY(mc_id, 0, dnx_data_fabric.mesh.max_mc_id_get(unit), _SHR_E_PARAM, "illegal mc_id");
    /*
     * Validate flags
     */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "Unrecognized flags");
    /*
     * Validate destid_count
     */
    max_mesh_replication = dnx_data_fabric.mesh.nof_max_dest_get(unit) + dnx_data_device.general.nof_cores_get(unit);
    if (destid_count > max_mesh_replication)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "too many destinations");
    }
    /*
     * Validate destinations in dest_array
     */
    for (dest = 0; dest < destid_count; ++dest)
    {
        if (BCM_FABRIC_MODID_IS_GROUP(destid_array[dest]))
        {
            /** validate local_dest fits configuration */
            SHR_IF_ERR_EXIT(dnx_fabric_local_dest_id_verify(unit, destid_array[dest]));
        }
        else
        {
            int is_core_modid_valid = 0;
            uint32 my_core_modid = 0;
            uint32 core;

            /** Validate destination fits local modids */
            for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); ++core)
            {
                SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get(unit, core, &my_core_modid));
                if (destid_array[dest] == my_core_modid)
                {
                    is_core_modid_valid = 1;
                    break;
                }
            }

            if (!is_core_modid_valid)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid dest_id %d", destid_array[dest]);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set per mc_id to which destinations to replicate.
 * \param [in] unit         - The unit number.
 * \param [in] mc_id        - The multicast id.
 * \param [in] flags        - Flags (not in use)
 * \param [in] destid_count - Number of destinations in destid_array.
 * \param [in] destid_array - Array of destinations to replicate to.
 * \return
 *   See \ref shr_error_e
 */
int
bcm_dnx_fabric_multicast_set(
    int unit,
    bcm_multicast_t mc_id,
    uint32 flags,
    uint32 destid_count,
    bcm_module_t * destid_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_multicast_set_verify(unit, mc_id, flags, destid_count, destid_array));
    /*
     * Configure fabric replication
     */
    SHR_IF_ERR_EXIT(dnx_fabric_multicast_set(unit, mc_id, destid_count, destid_array));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify routine for associated BCM API
 * \param [in]  unit             - The unit number.
 * \param [in]  mc_id            - The multicast id.
 * \param [in]  flags            - Flags (not in use)
 * \param [in]  destid_count_max - Size of the given destid_array.
 * \param [out] destid_count     - Number of destinations in destid_array.
 * \param [out] destid_array     - Array of destinations which gets replicated.
 * \return
 *   See \ref shr_error_e
 */
static int
dnx_fabric_multicast_get_verify(
    int unit,
    bcm_multicast_t mc_id,
    uint32 flags,
    int destid_count_max,
    int *destid_count,
    bcm_module_t * destid_array)
{
    uint32 is_mesh = FALSE;
    uint32 is_tdm_supported = dnx_data_fabric.tdm.priority_get(unit) != -1;
    uint32 is_mesh_mc = dnx_data_fabric.mesh.multicast_enable_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(destid_array, _SHR_E_PARAM, "destid_array");
    SHR_NULL_CHECK(destid_count, _SHR_E_PARAM, "destid_count");

    /** activate mesh-mc for mesh and single fap topology */
    if ((dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_MESH) ||
        (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_SINGLE_FAP))
    {
        is_mesh = TRUE;
    }
    /*
     * Validate device mode
     */
    if (!is_mesh || (!is_mesh_mc && !is_tdm_supported))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "bcm_fabric_multicast_get is supported only in MESH mode with TDM support or in MESH_MC mode");
    }
    /*
     * Validate mc_id
     */
    SHR_RANGE_VERIFY(mc_id, 0, dnx_data_fabric.mesh.max_mc_id_get(unit), _SHR_E_PARAM, "illegal mc_id");
    /*
     * Validate flags
     */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "Unrecognized flags");
    /*
     * Validate destid_count_max
     */
    if (destid_count_max <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal destid_count_max");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get per mc_id which destinations gets replicated.
 * \param [in]  unit             - The unit number.
 * \param [in]  mc_id            - The multicast id.
 * \param [in]  flags            - Flags (not in use)
 * \param [in]  destid_count_max - Size of the given destid_array.
 * \param [out] destid_count     - Number of destinations in destid_array.
 * \param [out] destid_array     - Array of destinations which gets replicated.
 * \return
 *   See \ref shr_error_e
 */
int
bcm_dnx_fabric_multicast_get(
    int unit,
    bcm_multicast_t mc_id,
    uint32 flags,
    int destid_count_max,
    int *destid_count,
    bcm_module_t * destid_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_multicast_get_verify(unit, mc_id, flags, destid_count_max, destid_count,
                                                          destid_array));
    /*
     * Get fabric replication configuration
     */
    SHR_IF_ERR_EXIT(dnx_fabric_multicast_get(unit, mc_id, destid_count_max, destid_count, destid_array));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify routine for BCM API
 * \param [in] unit         - *   The unit number.
 * \param [in] group        - *   The group id.
 * \param [in] modid_count  - *   Number of modids in modid_array param.
 * \param [in] modid_array  - *   Array of modids to be associated with local_dest group.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * bcm_dnx_fabric_modid_group_get.
 */
static int
dnx_fabric_modid_group_set_verify(
    int unit,
    bcm_module_t group,
    int modid_count,
    bcm_module_t * modid_array)
{
    int i;
    uint32 max_nof_cores = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(modid_array, _SHR_E_PARAM, "modid_array");

    /*
     * Validate MESH mode
     */
    if (dnx_data_fabric.general.connect_mode_get(unit) != SOC_DNX_FABRIC_CONNECT_MODE_MESH)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "bcm_fabric_modid_group_set is only valid in mesh mode");
    }

    /*
     * Validate modid_count
     */
    max_nof_cores = dnx_data_device.general.max_nof_cores_get(unit);
    SHR_RANGE_VERIFY(modid_count, 1, max_nof_cores, _SHR_E_PARAM, "modid_count should be between 1 and %u",
                     max_nof_cores);

    /*
     * Validate faps in array are valid
     */
    for (i = 0; i < modid_count; ++i)
    {
        if (!BCM_FABRIC_MODID_IS_VALID(unit, modid_array[i]))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "fap id %d is invalid", modid_array[i]);
        }
    }
    /*
     * Validate group
     */
    SHR_IF_ERR_EXIT(dnx_fabric_local_dest_id_verify(unit, group));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Associate between the group (local_dest) and the module-ids in it.
 * \param [in] unit         - *   The unit number.
 * \param [in] group        - *   The group id.
 * \param [in] modid_count  - *   Number of modids in modid_array param.
 * \param [in] modid_array  - *   Array of modids to be associated with local_dest group.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * bcm_dnx_fabric_modid_group_get.
 */
int
bcm_dnx_fabric_modid_group_set(
    int unit,
    bcm_module_t group,
    int modid_count,
    bcm_module_t * modid_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_modid_group_set_verify(unit, group, modid_count, modid_array));
    /*
     * Associate modids with group
     */
    SHR_IF_ERR_EXIT(dnx_fabric_modid_group_set(unit, group, modid_count, modid_array));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief *   Verify routine for BCM API that obtains the modids associated with a group.
 * \param [in]  unit            - *   The unit number.
 * \param [in]  group           - *   The group id.
 * \param [in]  modid_max_count - *   The length of the given modid_array.
 * \param [out] modid_array     - *   Array of modids associated with local_dest group.
 * \param [out] modid_count     - *   Number of actual modids in modid_array.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * bcm_dnx_fabric_modid_group_set.
 */
static int
dnx_fabric_modid_group_get_verify(
    int unit,
    bcm_module_t group,
    int modid_max_count,
    bcm_module_t * modid_array,
    int *modid_count)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(modid_array, _SHR_E_PARAM, "modid_array");
    SHR_NULL_CHECK(modid_count, _SHR_E_PARAM, "modid_count");

    /*
     * Validate mesh mode
     */
    if (dnx_data_fabric.general.connect_mode_get(unit) != SOC_DNX_FABRIC_CONNECT_MODE_MESH)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "bcm_fabric_modid_group_get is only valid in mesh mode");
    }
    /*
     * Validate modid_max_count
     */
    if (modid_max_count <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal modid_max_count");
    }
    /*
     * Validate group
     */
    SHR_IF_ERR_EXIT(dnx_fabric_local_dest_id_verify(unit, group));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief *   Get the modids associated with a group.
 * \param [in]  unit            - *   The unit number.
 * \param [in]  group           - *   The group id.
 * \param [in]  modid_max_count - *   The length of the given modid_array.
 * \param [out] modid_array     - *   Array of modids associated with local_dest group.
 * \param [out] modid_count     - *   Number of actual modids in modid_array.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * bcm_dnx_fabric_modid_group_set.
 */
int
bcm_dnx_fabric_modid_group_get(
    int unit,
    bcm_module_t group,
    int modid_max_count,
    bcm_module_t * modid_array,
    int *modid_count)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_fabric_modid_group_get_verify(unit, group, modid_max_count, modid_array, modid_count));
    /*
     * Get modids associated with this group
     */
    SHR_IF_ERR_EXIT(dnx_fabric_modid_group_get(unit, group, modid_max_count, modid_array, modid_count));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set system ref clock frequency.
 */
static shr_error_e
dnx_fabric_mesh_topology_core_clock_frequency_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 device_core_clock_khz = 0;
    uint32 system_ref_clock_khz = 0;
    uint32 intg = 0;
    UTILEX_U64 frac, frac_x, frac_x2;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Enable the fraction mode over cal. 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_INIT, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MULTI_FAP_3, INST_SINGLE, 0x1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    device_core_clock_khz = DNXCMN_CORE_CLOCK_KHZ_GET(unit);
    system_ref_clock_khz = dnx_data_device.general.system_ref_core_clock_khz_get(unit);
    intg = system_ref_clock_khz / device_core_clock_khz;

    utilex_u64_clear(&frac);
    utilex_u64_clear(&frac_x);

    utilex_u64_multiply_longs(system_ref_clock_khz, DNX_FABRIC_SYS_FREQ_RES_19, &frac_x);
    utilex_u64_devide_u64_long(&frac_x, device_core_clock_khz, &frac);

    if (device_core_clock_khz <= system_ref_clock_khz)
    {
        utilex_u64_multiply_longs(intg, DNX_FABRIC_SYS_FREQ_RES_19, &frac_x2);
        utilex_u64_subtract_u64(&frac, &frac_x2);
    }

    /*
     * Set the System clock frequency 
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_CONFIG_1, INST_SINGLE, frac.arr[0]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_CONFIG_2, INST_SINGLE, intg);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_mesh_topology_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_mesh_topology_core_clock_frequency_init(unit));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_INIT, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_INIT, INST_SINGLE, 0xd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_CONFIG_1, INST_SINGLE, 0x0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_CONFIG_2, INST_SINGLE, 0xd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_CONFIG_3, INST_SINGLE, 0x0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_INIT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAST_CONFIG_1, INST_SINGLE, 0x5);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAST_CONFIG_2, INST_SINGLE, 0xc);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAST, INST_SINGLE, 0x0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_INIT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_ENABLE, INST_SINGLE, 0x1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_TH, INST_SINGLE, 0x1800);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_DC, INST_SINGLE, 0x0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_mesh_topology_get(
    int unit,
    soc_dnxc_fabric_mesh_topology_diag_t * mesh_topology_diag)
{
    uint32 entry_handle_id;
    uint32 link = 0;
    uint32 is_valid = 0;
    uint32 value = 0;
    uint32 nof_links = dnx_data_fabric.links.nof_links_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_STATUS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INDICATION_1, INST_SINGLE,
                               &mesh_topology_diag->indication_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INDICATION_2, INST_SINGLE,
                               &mesh_topology_diag->indication_2);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS_STATUS, entry_handle_id));
    for (link = 0; link < nof_links; ++link)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LINK_STATUS_IS_VALID, INST_SINGLE, &is_valid);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LINK_STATUS_VALUE, INST_SINGLE, &value);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        mesh_topology_diag->type2_list[link] = is_valid ? value : -1;
    }
    mesh_topology_diag->link_list_count = nof_links;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_STATUS, entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_CONTROL_CELLS_TYPE1, INST_SINGLE,
                               &mesh_topology_diag->rx_control_cells_type1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_CONTROL_CELLS_TYPE2, INST_SINGLE,
                               &mesh_topology_diag->rx_control_cells_type2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INDICATION_3, INST_SINGLE,
                               &mesh_topology_diag->indication_3);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INDICATION_4, INST_SINGLE,
                               &mesh_topology_diag->indication_4);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONTROL_CELLS_TYPE3, INST_SINGLE,
                               &mesh_topology_diag->control_cells_type3);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_1, INST_SINGLE, &mesh_topology_diag->status_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_1_ID1, INST_SINGLE,
                               &mesh_topology_diag->status_1_id1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_1_ID2, INST_SINGLE,
                               &mesh_topology_diag->status_1_id2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_2, INST_SINGLE, &mesh_topology_diag->status_2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_2_ID1, INST_SINGLE,
                               &mesh_topology_diag->status_2_id1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_2_ID2, INST_SINGLE,
                               &mesh_topology_diag->status_2_id2);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
