/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file lif_mngr.c
 *
 *  Lif algorithms initialization and deinitialization.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */
#include <sal/appl/sal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/swstate/auto_generated/access/lif_mngr_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include "global_lif_allocation.h"
#include "lif_mngr_internal.h"
#include "lif_table_mngr.h"
#include <bcm_int/dnx/cmn/dnxcmn.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */




/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */
char *lif_mngr_outlif_phase_name[LIF_MNGR_OUTLIF_PHASE_COUNT] = {
    "PHASE_AC",
    "PHASE_NATIVE_AC",
    "PHASE_ARP",
    "PHASE_NATIVE_ARP",
    "PHASE_MPLS_TUNNEL_1",
    "PHASE_MPLS_TUNNEL_2",
    "PHASE_MPLS_TUNNEL_3",
    "PHASE_MPLS_TUNNEL_4",
    "PHASE_SRV6_BASE",
    "PHASE_IPV6_RAW_SRV6_TUNNEL",
    "PHASE_SRV6_TUNNEL_1",
    "PHASE_SRV6_TUNNEL_2",
    "PHASE_SRV6_TUNNEL_3",
    "PHASE_SRV6_TUNNEL_4",
    "PHASE_IP_TUNNEL_1",
    "PHASE_IP_TUNNEL_2",
    "PHASE_IP_TUNNEL_3",
    "PHASE_IP_TUNNEL_4",
    "PHASE_VPLS_1",
    "PHASE_VPLS_2",
    "PHASE_RIF",
    "PHASE_SFLOW",
    "PHASE_SFLOW_RAW_1",
    "PHASE_SFLOW_RAW_2",
    "PHASE_REFLECTOR",
    "PHASE_RCH"
};

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

shr_error_e
dnx_lif_mngr_phase_string_to_id(
    int unit,
    char *outlif_phase_str,
    lif_mngr_outlif_phase_e *outlif_phase)
{
    int outlif_phase_index;

    SHR_FUNC_INIT_VARS(unit);

    for (outlif_phase_index = 0; outlif_phase_index < LIF_MNGR_OUTLIF_PHASE_COUNT; outlif_phase_index++)
    {
        if (sal_strcasecmp(lif_mngr_outlif_phase_name[outlif_phase_index], outlif_phase_str) == 0)
        {
            *outlif_phase = outlif_phase_index;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "outlif phase type not found (%s)\n", outlif_phase_str);

exit:
    SHR_FUNC_EXIT;
}



/*
 * Set table id and result type from SW state
 * by local inlif
 */
shr_error_e
dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_set(
    int unit,
    int local_in_lif,
    int core_id,
    dbal_tables_e table_id,
    uint32 result_type)
{
    uint32 entry_handle_id;
    dbal_physical_tables_e physical_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the physical table associated with this lif format.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /*
     * Take table handle and set core key, according to physical table.
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_INLIF_INFO_SW, &entry_handle_id));
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_INLIF_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given dbal table id is not a lif table.");
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_in_lif);

    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME, INST_SINGLE, table_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, INST_SINGLE, result_type);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(
    int unit,
    int local_in_lif,
    int core_id,
    dbal_physical_tables_e physical_table_id,
    dbal_tables_e * table_id,
    uint32 *result_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Take table handle and set core key, according to physical table.
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_INLIF_INFO_SW, &entry_handle_id));
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_INLIF_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given dbal table id is not a lif table.");
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_in_lif);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME, INST_SINGLE, table_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, INST_SINGLE, result_type);

    SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_delete(
    int unit,
    int core_id,
    uint32 local_in_lif,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    dbal_physical_tables_e physical_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the physical table associated with this lif format.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /*
     * Take table handle and set core key, according to physical table.
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_INLIF_INFO_SW, &entry_handle_id));
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_INLIF_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given dbal table id is not a lif table.");
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_in_lif);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Set table id and result type from SW state
 * by local outlif
 */
shr_error_e
dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set(
    int unit,
    int local_out_lif,
    dbal_tables_e table_id,
    uint32 result_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_OUT_LIF_INFO_SW, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_out_lif);
    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME, INST_SINGLE, table_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_get(
    int unit,
    int local_out_lif,
    dbal_tables_e * dbal_table_id,
    uint32 *result_type,
    lif_mngr_outlif_phase_e * outlif_phase)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_OUT_LIF_INFO_SW, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_out_lif);
    /*
     * Get values:
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME, INST_SINGLE, dbal_table_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (outlif_phase)
    {
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_to_eedb_logical_phase(unit, local_out_lif, *dbal_table_id, outlif_phase));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(
    int unit,
    uint32 local_out_lif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Delete lif info from SW state
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_OUT_LIF_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_out_lif);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify inlif info struct: \ref lif_mngr_local_inlif_info_t.
 *  By default, all fields are always checked, according to the dbal/device data legal values.
 *
 *  This functions assumes that for DPC lifs all core id options are legal, so if only a specific
 *  core is required, it must be checked outside this function.
 *
 *  The field 'dbal_result_type' is not required in most APIs that use this sturct, and it can be set
 *     to 0 if it's not required.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] ignore_ids - If this boolean arguemnt is TRUE, then the 'local_inlifs' field of 'inlif_info' \n
 *                          will not be verified.
 * \param [in] inlif_info - The fields in this struct will be checked as described above. \n
 *                          See \ref lif_mngr_local_inlif_info_t for description of specific fields.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   lif_mngr_local_inlif_info_t
 */
static shr_error_e
dnx_lif_mngr_local_inlif_info_verify(
    int unit,
    int ignore_ids,
    lif_mngr_local_inlif_info_t * inlif_info)
{
    dbal_physical_tables_e physical_table_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(inlif_info, _SHR_E_PARAM, "inlif_info");

    /*
     * Get the physical table associated with this lif format. The physical table will tell us
     *  whether it's a DPC or SBC lif table.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, inlif_info->dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                   &physical_table_id));

    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        /*
         * This is the SBC table. Verify that core is _SHR_CORE_ALL.
         */
        if (inlif_info->core_id != _SHR_CORE_ALL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Dbal logical table %s is shared by cores, so core_id must be _SHR_CORE_ALL. Given %d.",
                         dbal_logical_table_to_string(unit, inlif_info->dbal_table_id), inlif_info->core_id);
        }

        if (ignore_ids == FALSE)
        {
            /*
             * If required, verify that lif id in index 0 is legal.
             * We only check this ID because this is the ID shared by cores inlif table is using.
             * Lif IDs in different array indexes are not used.
             */
            LIF_MNGR_LOCAL_SBC_IN_LIF_VERIFY(unit, inlif_info->local_inlif);
        }
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        /*
         * This is a DPC table, so just verify that the core is legal. _SHR_CORE_ALL is also legal.
         */
        DNXCMN_CORE_VALIDATE(unit, inlif_info->core_id, TRUE);

        if (ignore_ids == FALSE)
        {
            uint8 is_allocated_on_all_cores;
            /*
             * Check that the lif id is legal, and that it's either allocated on all cores or on a single core, and compare it to
             * the input.
             */
            LIF_MNGR_LOCAL_DPC_IN_LIF_VERIFY(unit, inlif_info->local_inlif);

            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                            inlif_table_dpc_all_cores_indication.bit_get(unit, inlif_info->local_inlif,
                                                                         &is_allocated_on_all_cores));

            if (!is_allocated_on_all_cores && inlif_info->core_id == _SHR_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Inlif is indicated to be allocated on all cores, but it's actually allocated"
                             "on every core separately.");
            }
            else if (is_allocated_on_all_cores && inlif_info->core_id != _SHR_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Inlif is indicated to be allocated only on core %d, but it's actually allocated"
                             "on all cores at once.", inlif_info->core_id);

            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong dbal logical table: %s",
                     dbal_logical_table_to_string(unit, inlif_info->dbal_table_id));
    }

    /*
     * Only check result type if it's >0, otherwise it's not interesting.
     */
    if (inlif_info->dbal_result_type != 0)
    {
        int entry_size_bits;
        /*
         * Calling the get function is enough, because it has a built in error indication if the result type
         * is illegal.
         */
        SHR_IF_ERR_EXIT(dbal_tables_payload_size_get
                        (unit, inlif_info->dbal_table_id, inlif_info->dbal_result_type, &entry_size_bits));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify out info struct: \ref lif_mngr_local_outlif_info_t.
 *  By default, all fields are always checked, according to the dbal/device data legal values.
 *
 *  The field 'dbal_result_type' is not required in most APIs that use this sturct, and it can be set
 *     to 0 if it's not required.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] ignore_id - If this boolean arguemnt is TRUE, then the 'local_outlif' field of 'outlif_info' \n
 *                          will not be verified.
 * \param [in] outlif_info - The fields in this struct will be checked as described above. \n
 *                          See \ref lif_mngr_local_outlif_info_t for description of specific fields.
 * \param [in] outlif_hw_info - This struct will be NULL checked. \n
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   lif_mngr_local_outlif_info_t
 */
static shr_error_e
dnx_lif_mngr_local_outlif_info_verify(
    int unit,
    int ignore_id,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    dbal_physical_tables_e physical_table_id;
    dnx_algo_local_outlif_logical_phase_e outlif_logical_phase;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(outlif_info, _SHR_E_PARAM, "outlif_info");
    SHR_NULL_CHECK(outlif_hw_info, _SHR_E_INTERNAL, "outlif_hw_info");

    /*
     * Get the physical table associated with this lif format. Verify that the physical table is EEDB.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, outlif_info->dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                   &physical_table_id));

    if (physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_1
        && physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_2
        && physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_3 && physical_table_id != DBAL_PHYSICAL_TABLE_EEDB_4)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong dbal logical table: %s",
                     dbal_logical_table_to_string(unit, outlif_info->dbal_table_id));
    }

    /*
     * Only check result type if it's >0, otherwise it's not interesting.
     */
    if (outlif_info->dbal_result_type != 0)
    {
        int entry_size_bits;
        /*
         * Calling the get function is enough, because it has a built in error indication if the result type
         * is illegal.
         */
        SHR_IF_ERR_EXIT(dbal_tables_payload_size_get
                        (unit, outlif_info->dbal_table_id, outlif_info->dbal_result_type, &entry_size_bits));
    }

    /*
     * Check outlif phase.
     * Calling the get function is enough, because it has a built in error indication if the no phase mapping
     * was found.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_phase_enum_to_logical_phase_num
                    (unit, outlif_info->outlif_phase, &outlif_logical_phase));

    if (!ignore_id)
    {
        LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, outlif_info->local_outlif);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verification fuction for dnx_lif_mngr_lif_allocate.
 * Note - it's possible to verify the global lif id if the WITH_ID flag was given,
 *  but we currently skip it because it's checked in the main function by calling the global lif
 *  verify function.
 */
static shr_error_e
dnx_lif_mngr_lif_allocate_verify(
    int unit,
    uint32 flags,
    int *global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    int is_ingress, is_egress, dont_allocate_global_lif;
    SHR_FUNC_INIT_VARS(unit);

    dont_allocate_global_lif = _SHR_IS_FLAG_SET(flags, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF);

    if (!dont_allocate_global_lif)
    {
        SHR_NULL_CHECK(global_lif, _SHR_E_PARAM, "global_lif");
    }

    is_ingress = (inlif_info != NULL);
    is_egress = (outlif_info != NULL);

    if ((!is_ingress) && (!is_egress))
    {
        /*
         * A lif can be either an inlif or an outlif, or both (symmetric). If neither side is given as
         *   input, then the lif has no meaning.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "At least one of inlif_info or outlif_info must be not NULL.");
    }

    if (is_ingress && is_egress && dont_allocate_global_lif)
    {
        /*
         * If there's no global lif, then allocating both ingress and egress global lifs is meaningless,
         * since without global lifs they can't be symmetric. Instead, allocate them in two separate calls.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "If don't allocate global lif was indicated, only one direction (ingress or egress) "
                     "can be used.");
    }

    /*
     * Verify inlif info.
     */
    if (is_ingress)
    {
        /*
         * We set the argument 'ignore_ids' to TRUE because local lifs can't be allocated WITH_ID,
         * so the IDs are ignored and shouldn't be verified.
         */
        SHR_IF_ERR_EXIT(dnx_lif_mngr_local_inlif_info_verify(unit, TRUE, inlif_info));
    }

    /*
     * Verify outlif info.
     */
    if (is_egress)
    {
        /*
         * We set the argument 'ignore_ids' to TRUE because local lifs can't be allocated WITH_ID,
         * so the IDs are ignored and shouldn't be verified.
         */
        SHR_IF_ERR_EXIT(dnx_lif_mngr_local_outlif_info_verify(unit, TRUE, outlif_info, outlif_hw_info));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_lif_allocate(
    int unit,
    uint32 flags,
    int *global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    int global_lif_tmp;
    /*
     * The is_ingress, is_egress flags are used to indicate whether we'll allocate ingress/egress lifs in this function.
     * In some cases, we might choose not to allocate the egress global lif. In this case, the allocate_egress_global_lif flags
     * will be set to FALSE.
     */
    int is_ingress, is_egress, allocate_global_lif;
    uint32 global_lif_allocation_flags, global_lif_direction_flags;
    lif_mapping_local_lif_info_t local_lif_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Step 0: Verify input and set allocation flags.
     *
     * Verify local lif info. We verify global lif info below so we don't need to calculate all the
     * flags twice.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_lif_mngr_lif_allocate_verify
                          (unit, flags, global_lif, inlif_info, outlif_info, outlif_hw_info));

    is_ingress = (inlif_info != NULL);
    is_egress = (outlif_info != NULL);
    global_lif_allocation_flags = 0;
    global_lif_direction_flags = 0;

    /*
     * Don't allocate egress global lif if don't allocate global lif flag is set.
     */
    allocate_global_lif = !_SHR_IS_FLAG_SET(flags, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF);

    if (allocate_global_lif)
    {
        /*
         * Take only the flags relevant for global lif allocation.
         */
        global_lif_allocation_flags |=
            (flags &
             (LIF_MNGR_GLOBAL_LIF_WITH_ID | LIF_MNGR_L2_GPORT_GLOBAL_LIF | LIF_MNGR_ONE_SIDED_SYMMETRIC_GLOBAL_LIF));

        /*
         * Set DNX_ALGO_LIF_INGRESS/EGRESS flags according to the required allocation.
         */
        global_lif_direction_flags = DNX_ALGO_GLOBAL_LIF_ALLOCATION_SET_DIRECTION_FLAGS(is_ingress, is_egress);

        /*
         * We use a temporary variable for global lif throughout the function for code simplicity.
         */
        global_lif_tmp = *global_lif;

        /*
         * Verify that all global lif flags are legal, and if allocation is WITH_ID, verify the ID as well.
         */
        SHR_INVOKE_VERIFY_DNX(dnx_algo_global_lif_allocation_verify
                              (unit, global_lif_allocation_flags, global_lif_direction_flags, global_lif_tmp));
    }

    /*
     * Step 1: Allocate lifs.
     *
     */

    /*
     * Allocate local inlif.
     */
    if (is_ingress)
    {
        /*
         * Allocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_inlif_allocate(unit, inlif_info));
    }

    /*
     * Allocate local outlif.
     */
    if (is_egress)
    {
        /*
         * Allocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocate(unit, outlif_info, outlif_hw_info));
    }

    /*
     * Allocate global lif.
     */
    if (allocate_global_lif)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate(unit, global_lif_allocation_flags,
                                                                global_lif_direction_flags, &global_lif_tmp));

        *global_lif = global_lif_tmp;
    }
    else
    {
        /*
         * If no global lif was required, then if the user even passed a global lif pointer, fill it
         * with invalid indication.
         */
        if (global_lif)
        {
            *global_lif = LIF_MNGR_INVALID;
        }
    }

    /*
     * Step 2: Create lif mapping.
     */
    if (is_ingress && allocate_global_lif)
    {
        /*
         * Create ingress mapping.
         */
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = inlif_info->local_inlif;
        local_lif_info.core_id = inlif_info->core_id;

        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, inlif_info->dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                       &local_lif_info.phy_table));

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create(unit, DNX_ALGO_LIF_INGRESS, global_lif_tmp, &local_lif_info));
    }

    if (is_egress && allocate_global_lif)
    {
        /*
         * Create egress mapping.
         * Don't do it if we didn't allocate egress global lif.
         */
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = outlif_info->local_outlif;
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create(unit, DNX_ALGO_LIF_EGRESS, global_lif_tmp, &local_lif_info));
    }

    /*
     * Step 3: Save LIF TABLE DBAL ID and type in SW state.
     */
    if (is_ingress)
    {
        /*
         * Store info in SW state
         */
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_set
                        (unit, inlif_info->local_inlif, inlif_info->core_id, inlif_info->dbal_table_id,
                         inlif_info->dbal_result_type));
    }
    if (is_egress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set(unit, outlif_info->local_outlif,
                                                                                 outlif_info->dbal_table_id,
                                                                                 outlif_info->dbal_result_type));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_mngr_lif_free_verify(
    int unit,
    int global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    int is_ingress, is_egress, free_global_lif;
    uint32 global_lif_direction_flags;
    SHR_FUNC_INIT_VARS(unit);

    is_ingress = (inlif_info != NULL);
    is_egress = (outlif_info != NULL);
    free_global_lif = (global_lif != LIF_MNGR_INVALID);

    if (!is_ingress && !is_egress)
    {
        /*
         * A lif can be either an inlif or an outlif, or both (symmetric). If neither side is given as
         *   input, then the lif has no meaning.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "inlif_info is NULL and local_outlif is LIF_MNGR_INVALID. At least one must be valid.");
    }

    if (!free_global_lif && is_ingress && is_egress)
    {
        /*
         * If there's no global lif, then freeing both ingress and egress global lifs is meaningless,
         * since without global lifs they can't be symmetric. Instead, free them in two separate calls.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "If don't free global lif was indicated, only one direction (ingress or egress) "
                     "can be used.");
    }

    /*
     * Verify global lif is it's in use.
     */
    if (free_global_lif)
    {
        /*
         * Set DNX_ALGO_LIF_INGRESS/EGRESS flags according to the required deallocation.
         */
        global_lif_direction_flags = DNX_ALGO_GLOBAL_LIF_ALLOCATION_SET_DIRECTION_FLAGS(is_ingress, is_egress);

        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_verify
                        (unit, DNX_ALGO_GLOBAL_LIF_ALLOCATION_DEALLOCATE_VERIFY, global_lif_direction_flags,
                         global_lif));
    }

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_local_inlif_info_verify(unit, FALSE, inlif_info));
    }

    if (is_egress)
    {
        LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, outlif_info->local_outlif);

        SHR_NULL_CHECK(outlif_hw_info, _SHR_E_INTERNAL, "outlif_hw_info");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_mngr_lif_free(
    int unit,
    int global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info)
{
    int is_ingress, is_egress, free_global_lif;
    uint32 global_lif_direction_flags;
    SHR_FUNC_INIT_VARS(unit);

    is_ingress = (inlif_info != NULL);
    is_egress = (outlif_info != NULL);
    free_global_lif = (global_lif != LIF_MNGR_INVALID);

    /*
     * 0. Verify lif information.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_lif_mngr_lif_free_verify(unit, global_lif, inlif_info, outlif_info, outlif_hw_info));

    /*
     * Steps 1,2: Dellocate lifs, and remove mapping.
     */

    /*
     * Free global lif is it's in use.
     */
    if (free_global_lif)
    {
        /*
         * Set DNX_ALGO_LIF_INGRESS/EGRESS flags according to the required deallocation.
         */
        global_lif_direction_flags = DNX_ALGO_GLOBAL_LIF_ALLOCATION_SET_DIRECTION_FLAGS(is_ingress, is_egress);

        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, global_lif_direction_flags, global_lif));
    }

    /*
     * Deallocate and unmap inlif.
     */
    if (is_ingress)
    {
        /*
         * Deallocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_inlif_free(unit, inlif_info));

        if (free_global_lif)
        {
            /*
             * Remove ingress mapping.
             */
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, global_lif));
        }
    }

    /*
     * Deallocate and unmap outlif.
     */
    if (is_egress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_get
                        (unit, outlif_info->local_outlif, &outlif_info->dbal_table_id,
                         &outlif_info->dbal_result_type, NULL));

        /*
         * Deallocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_free(unit, outlif_info, outlif_hw_info));
        /*
         * Remove egress mapping, only if the global lif exists.
         */
        if (free_global_lif)
        {
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, global_lif));
        }
    }

    /*
     * Delete from SW state LIF to DBAL table id and type mapping.
     */
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_delete
                        (unit, inlif_info->core_id, inlif_info->local_inlif, inlif_info->dbal_table_id));
    }

    if (is_egress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(unit, outlif_info->local_outlif));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mngr_init(
    int unit)
{
    sw_state_algo_res_create_data_t inlif_data, outlif_data;
    uint8 is_init;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&inlif_data, 0, sizeof(sw_state_algo_res_create_data_t));
    sal_memset(&outlif_data, 0, sizeof(sw_state_algo_res_create_data_t));

    /*
     * Initialize lif mngr sw state.
     */
    SHR_IF_ERR_EXIT(lif_mngr_db.is_init(unit, &is_init));

    if (!is_init)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.init(unit));
    }

    /*
     * Initialize the local inlif allocation module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_inlif_allocation_init(unit));

    /*
     * Initialize the local outlif allocation module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocation_init(unit));

    /*
     * Initialize the global lif allocation module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_init(unit));

    /*
     * Initialize the lif mapping module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_init(unit));

    /*
     * Initialize the lif table manager
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_table_mngr_sorted_result_types_table_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_lif_mngr_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deinitialize the local outlif allocation module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_allocation_deinit(unit));

    /*
     * Deinitialize the lif mapping module.
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_deinit(unit));

    /*
     * sw state module deinit is done automatically at device deinit
     */

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
