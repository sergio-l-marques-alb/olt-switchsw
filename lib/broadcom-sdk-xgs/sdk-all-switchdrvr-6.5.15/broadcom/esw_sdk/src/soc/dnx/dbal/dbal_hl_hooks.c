/*
 * $Id: dbal_hl_hooks.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#include "dbal_hl_access.h"
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_port_access.h>

extern uint32 G_dbal_field_full_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];

/** specific tables hooks implementation   */

/**
 * \brief
 * Set the hooked fields
 * Update the key of entry_handle_hook
 * The hooked fields are mapped to ETPPA_PER_PORT_TABLE or ERPP_PER_PORT_TABLE
 * In this case the key should not be the PP port but the DSP port
 */
static shr_error_e
dbal_hl_hook_egress_pp_port_hooked_fields_set(
    int unit,
    int access_type,
    dbal_entry_handle_t * entry_handle_hook)
{
    bcm_pbmp_t pbmp_tm_ports;
    bcm_port_t pp_port;
    uint32 dsp_pp = 0;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(pbmp_tm_ports);
    pp_port = entry_handle_hook->phy_entry.key[0];
    core_id = entry_handle_hook->core_id;

    /** Get all tm ports that correspond to the pp_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_tm_pbmp_get(unit, core_id, pp_port, &pbmp_tm_ports));

    /** In case that pbmp_tm_ports is empty, exit from the function (relevant for trunk members) */
    if (BCM_PBMP_IS_NULL(pbmp_tm_ports))
    {
        entry_handle_hook->error_info.error_exists = 0;
        SHR_EXIT();
    }
    /** Iterate over all tm ports (dsp_pp ports) in order to set all the entries according to them */
    BCM_PBMP_ITER(pbmp_tm_ports, dsp_pp)
    {
        /** Update the key to EGRESS_PP_PORT - instead of using pp port, use dsp_pp as a key to the table */
        entry_handle_hook->phy_entry.key[0] = dsp_pp;
        /** Set the hooked fields according to dsp_pp */
        SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle_hook));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Update the entry handles for the regular fields and for the hooked fields
 * Remove the hooked fields from entry_handle_regular_fields
 * Remove the regular fields from entry_handle_hook
 */
static shr_error_e
dbal_hl_hook_egress_pp_port_handles_update(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_entry_handle_t * entry_handle_regular_fields,
    dbal_entry_handle_t * entry_handle_hook,
    uint32 iter,
    int field_pos,
    uint32 *has_regular_fields,
    uint32 *has_hook_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((*entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
         l2p_fields_info[iter].memory != ETPPA_PER_PORT_TABLEm)
        && (*entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
            l2p_fields_info[iter].memory != ERPP_PER_PORT_TABLEm))
    {
        /*
         * In case of DBAL_CORE_ALL if in entry_handle_regular_fields->value_field_ids[field_pos]
         * there is no field id, it should be added
         */
        if ((entry_handle->get_all_fields == TRUE)
            && (entry_handle_regular_fields->value_field_ids[field_pos] == DBAL_FIELD_EMPTY))
        {
            entry_handle_regular_fields->value_field_ids[field_pos] =
                entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info
                [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].field_id;
            entry_handle_regular_fields->nof_result_fields = entry_handle_regular_fields->nof_result_fields + 1;
            entry_handle_regular_fields->num_of_fields = entry_handle_regular_fields->num_of_fields + 1;
        }

        /** Remove the field from entry_handle_hook only if the field is not mapped also to hook memory (ETPPA_PER_PORT_TABLE or ERPP_PER_PORT_TABLE)  */
        if (((entry_handle_hook->value_field_ids[field_pos] != DBAL_FIELD_EMPTY) && (iter == 0))
            || ((entry_handle_hook->value_field_ids[field_pos] != DBAL_FIELD_EMPTY) && (field_pos > 0) && (iter > 0)
                && (entry_handle->table->
                    hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter].field_id ==
                    entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                              cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter - 1].field_id)
                && (*entry_handle->table->
                    hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter - 1].memory != ETPPA_PER_PORT_TABLEm
                    && *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                  cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter - 1].memory != ERPP_PER_PORT_TABLEm)) || ((field_pos > 0) && (iter > 0)
                                                                                   && (entry_handle->table->
                                                                                       hl_mapping_multi_res
                                                                                       [entry_handle->
                                                                                        cur_res_type].l2p_hl_info
                                                                                       [DBAL_HL_ACCESS_MEMORY].
                                                                                       l2p_fields_info[iter].field_id !=
                                                                                       entry_handle->table->
                                                                                       hl_mapping_multi_res
                                                                                       [entry_handle->
                                                                                        cur_res_type].l2p_hl_info
                                                                                       [DBAL_HL_ACCESS_MEMORY].
                                                                                       l2p_fields_info[iter -
                                                                                                       1].field_id)
                                                                                   && (entry_handle->table->
                                                                                       hl_mapping_multi_res
                                                                                       [entry_handle->
                                                                                        cur_res_type].l2p_hl_info
                                                                                       [DBAL_HL_ACCESS_MEMORY].
                                                                                       l2p_fields_info[iter].field_id !=
                                                                                       entry_handle->table->
                                                                                       hl_mapping_multi_res
                                                                                       [entry_handle->
                                                                                        cur_res_type].l2p_hl_info
                                                                                       [DBAL_HL_ACCESS_MEMORY].
                                                                                       l2p_fields_info[iter +
                                                                                                       1].field_id))
            || ((field_pos > 0) && (iter > 0)
                && (iter <
                    entry_handle->table->hl_mapping_multi_res[0].
                    l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields - 1)
                && (*entry_handle->table->
                    hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter + 1].memory != ETPPA_PER_PORT_TABLEm
                    && *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                  cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter + 1].memory != ERPP_PER_PORT_TABLEm)
                && (entry_handle->table->
                    hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter].field_id ==
                    entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                              cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter + 1].field_id)))
        {
            entry_handle_hook->value_field_ids[field_pos] = DBAL_FIELD_EMPTY;
            entry_handle_hook->nof_result_fields = entry_handle_hook->nof_result_fields - 1;
            entry_handle_hook->num_of_fields = entry_handle_hook->num_of_fields - 1;
        }

        *has_regular_fields = TRUE;
        /** Update the payload mask */
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (G_dbal_field_full_mask,
                         entry_handle_regular_fields->table->hl_mapping_multi_res[entry_handle_regular_fields->
                                                                                  cur_res_type].l2p_hl_info
                         [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].offset_in_interface,
                         entry_handle_regular_fields->table->hl_mapping_multi_res[entry_handle_regular_fields->
                                                                                  cur_res_type].l2p_hl_info
                         [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].nof_bits_in_interface,
                         &(entry_handle_regular_fields->phy_entry.p_mask[0])));

    }
    else
    {

        /*
         * In case of DBAL_CORE_ALL if in entry_handle_hook->value_field_ids[field_pos]
         * there is no field id, it should be added
         */
        if ((entry_handle->get_all_fields == TRUE)
            && (entry_handle_hook->value_field_ids[field_pos] == DBAL_FIELD_EMPTY))
        {
            entry_handle_hook->value_field_ids[field_pos] =
                entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info
                [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].field_id;
            entry_handle_hook->nof_result_fields = entry_handle_hook->nof_result_fields + 1;
            entry_handle_hook->num_of_fields = entry_handle_hook->num_of_fields + 1;
        }

        /*
         * In case that the access is to memory ETPPA_PER_PORT_TABLE or ERPP_PER_PORT_TABLE the field is deleted from
         * entry_handle_regular_fields only if the field has no additional mapping to 'regular' memory
         */
        if (((entry_handle_regular_fields->value_field_ids[field_pos] != DBAL_FIELD_EMPTY) && (iter == 0))
            || ((entry_handle_regular_fields->value_field_ids[field_pos] != DBAL_FIELD_EMPTY) && (field_pos > 0)
                && (iter > 0)
                && (entry_handle->table->
                    hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter].field_id ==
                    entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                              cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter - 1].field_id)
                && (*entry_handle->table->
                    hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter - 1].memory == ETPPA_PER_PORT_TABLEm
                    || *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                  cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter - 1].memory == ERPP_PER_PORT_TABLEm)) || ((field_pos > 0) && (iter > 0)
                                                                                   && (entry_handle->table->
                                                                                       hl_mapping_multi_res
                                                                                       [entry_handle->
                                                                                        cur_res_type].l2p_hl_info
                                                                                       [DBAL_HL_ACCESS_MEMORY].
                                                                                       l2p_fields_info[iter].field_id !=
                                                                                       entry_handle->table->
                                                                                       hl_mapping_multi_res
                                                                                       [entry_handle->
                                                                                        cur_res_type].l2p_hl_info
                                                                                       [DBAL_HL_ACCESS_MEMORY].
                                                                                       l2p_fields_info[iter -
                                                                                                       1].field_id)
                                                                                   && (entry_handle->table->
                                                                                       hl_mapping_multi_res
                                                                                       [entry_handle->
                                                                                        cur_res_type].l2p_hl_info
                                                                                       [DBAL_HL_ACCESS_MEMORY].
                                                                                       l2p_fields_info[iter].field_id !=
                                                                                       entry_handle->table->
                                                                                       hl_mapping_multi_res
                                                                                       [entry_handle->
                                                                                        cur_res_type].l2p_hl_info
                                                                                       [DBAL_HL_ACCESS_MEMORY].
                                                                                       l2p_fields_info[iter +
                                                                                                       1].field_id))
            || ((field_pos > 0) && (iter > 0)
                && (iter <
                    entry_handle->table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields
                    - 1)
                && (*entry_handle->table->
                    hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].l2p_fields_info
                    [iter + 1].memory == ETPPA_PER_PORT_TABLEm
                    || *entry_handle->table->hl_mapping_multi_res[entry_handle->
                                                                  cur_res_type].l2p_hl_info
                    [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter + 1].memory == ERPP_PER_PORT_TABLEm)
                && (entry_handle->table->
                    hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].
                    l2p_fields_info[iter + 1].field_id ==
                    entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info
                    [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].field_id)))
        {
            entry_handle_regular_fields->value_field_ids[field_pos] = DBAL_FIELD_EMPTY;
            entry_handle_regular_fields->nof_result_fields = entry_handle_regular_fields->nof_result_fields - 1;
            entry_handle_regular_fields->num_of_fields = entry_handle_regular_fields->num_of_fields - 1;
        }

        *has_hook_fields = TRUE;
        /** Update the payload mask */
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (G_dbal_field_full_mask,
                         entry_handle_hook->table->hl_mapping_multi_res[entry_handle_hook->cur_res_type].l2p_hl_info
                         [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].offset_in_interface,
                         entry_handle_hook->table->hl_mapping_multi_res[entry_handle_hook->cur_res_type].l2p_hl_info
                         [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].nof_bits_in_interface,
                         &(entry_handle_hook->phy_entry.p_mask[0])));
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/** table=egress_pp_port hooks  */

/**
 * \brief
 * Get entry from EGRESS_PP_PORT table
 */
static shr_error_e
dbal_hl_hook_egress_pp_port_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int access_type;
    dbal_logical_table_t *table = entry_handle->table;
    SHR_FUNC_INIT_VARS(unit);

    for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
    {
        if (table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type].num_of_access_fields > 0)
        {
            /** In case of memory access, get from SW state */
            if ((access_type == DBAL_HL_ACCESS_MEMORY))
            {
                SHR_IF_ERR_EXIT(dbal_hl_get_by_access_type(unit, DBAL_HL_ACCESS_SW, entry_handle, DBAL_INVALID_PARAM));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_hl_get_by_access_type(unit, access_type, entry_handle, DBAL_INVALID_PARAM));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set entry in EGRESS_PP_PORT table
 */
static shr_error_e
dbal_hl_hook_egress_pp_port_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int access_type;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_entry_handle_t *entry_handle_regular_fields = NULL;
    dbal_entry_handle_t *entry_handle_hook = NULL;
    uint32 iter;
    uint32 has_regular_fields;
    uint32 has_hook_fields;
    int is_valid;
    bcm_core_t core_id;
    bcm_port_t pp_port;
    uint8 save_only_in_sw_state = 0;
    SHR_FUNC_INIT_VARS(unit);

    has_regular_fields = FALSE;
    has_hook_fields = FALSE;
    pp_port = entry_handle->phy_entry.key[0];

    /** Create copies of entry_handle */
    SHR_ALLOC_SET_ZERO(entry_handle_regular_fields, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY,
                       EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(entry_handle_hook, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);
    *entry_handle_regular_fields = *entry_handle;
    *entry_handle_hook = *entry_handle;

    if (entry_handle->core_id == DBAL_CORE_ALL)
    {
        for (core_id = 0; core_id < DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES; core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.valid.get(unit, core_id, pp_port, &is_valid));
            if ((core_id == 0) && !is_valid)
            {
                entry_handle->core_id = 1;
            }

            if ((core_id == 1) && !is_valid)
            {
                /** In case that the pp port is not valid (there is no pp_port -> tm_port resolution), save the configuration only in SW state */
                save_only_in_sw_state = 1;
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.valid.get(unit, entry_handle->core_id, pp_port, &is_valid));
        if (!is_valid)
        {
            /** In case that the pp port is not valid (there is no pp_port -> tm_port resolution), save the configuration only in SW state */
            save_only_in_sw_state = 1;
        }
    }

    for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
    {
        if (table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type].num_of_access_fields > 0)
        {
            if (access_type == DBAL_HL_ACCESS_MEMORY)
            {
                /** Save the dbal entry also in Sw state when access_type is DBAL_HL_ACCESS_MEMORY */
                SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, DBAL_HL_ACCESS_SW, entry_handle));
            }
            /** Set the dbal entry to the hw in case that the pp port is valid */
            if ((access_type == DBAL_HL_ACCESS_MEMORY) && (save_only_in_sw_state == 0))
            {
                for (iter = 0;
                     iter <
                     entry_handle->table->hl_mapping_multi_res[0].
                     l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields; iter++)
                {
                    int field_pos;
                    field_pos =
                        entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info
                        [DBAL_HL_ACCESS_MEMORY].l2p_fields_info[iter].field_pos_in_interface;

                    if (entry_handle->value_field_ids[field_pos] == DBAL_FIELD_EMPTY)
                    {
                        /** Field not requested */
                        continue;
                    }

                    /** Update the entry handles */
                    SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_handles_update
                                    (unit, entry_handle, entry_handle_regular_fields, entry_handle_hook,
                                     iter, field_pos, &has_regular_fields, &has_hook_fields));
                }

                /** Set the regular fields if such should be set */
                if (has_regular_fields == TRUE)
                {
                    SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle_regular_fields));
                }

                /*
                 * In case that DBAL_CORE_ALL is used and there are fields that are mapped to ETPPA_PER_PORT_TABLE or ERPP_PER_PORT_TABLE 
                 * dbal_hl_set_by_access_type should be run separately for each core_id
                 */
                if ((entry_handle_hook->core_id == DBAL_CORE_ALL) && (has_hook_fields == TRUE))
                {
                    pp_port = entry_handle_hook->phy_entry.key[0];

                    for (core_id = 0; core_id < DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES; core_id++)
                    {
                        entry_handle_hook->core_id = core_id;
                        /** Update the key - the hooked fields should be accessed per DSP, not per PP port */
                        SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_hooked_fields_set
                                        (unit, access_type, entry_handle_hook));
                        if (core_id == 0)
                        {
                            /** Set again the key to pp_port because it was replaced with dsp_pp in dbal_hl_hook_egress_pp_port_hooked_fields_set */
                            entry_handle_hook->phy_entry.key[0] = pp_port;
                        }
                    }
                }
                /** Set the hooked fields in case that there are such and in case that core_id is not set to DBAL_CORE_ALL */
                else if ((entry_handle_hook->core_id != DBAL_CORE_ALL) && (has_hook_fields == TRUE))
                {
                    /** Set the hooked fields - they should be accessed per DSP, not per PP port */
                    SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_hooked_fields_set
                                    (unit, access_type, entry_handle_hook));

                }
                if (((has_regular_fields == TRUE) && (entry_handle_regular_fields->error_info.error_exists == 0))
                    && ((has_hook_fields == TRUE) && (entry_handle_hook->error_info.error_exists == 0)))
                {
                    entry_handle->error_info.error_exists = 0;
                }
                else if ((has_regular_fields == TRUE) && (entry_handle_regular_fields->error_info.error_exists == 0)
                         && (has_hook_fields == FALSE))
                {
                    entry_handle->error_info.error_exists = 0;
                }
                else if ((has_hook_fields == TRUE) && (entry_handle_hook->error_info.error_exists == 0)
                         && (has_regular_fields == FALSE))
                {
                    entry_handle->error_info.error_exists = 0;
                }

            }
            else if (access_type != DBAL_HL_ACCESS_MEMORY)
            {
                SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle));
            }
        }
    }

exit:
    SHR_FREE(entry_handle_hook);
    SHR_FREE(entry_handle_regular_fields);
    SHR_FUNC_EXIT;

}

/** general functions (per action) that called if the hook is enabled  */
shr_error_e
dbal_hl_hook_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table_id)
    {
        case DBAL_TABLE_EGRESS_PP_PORT:
            SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_entry_get(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "no hook supported for table %s operatoion get\n",
                         entry_handle->table->table_name);
            break;
    }

    /*
     * This is a reference to the standart flow of getting an entry. It should be used and modified inside each get
     * hook function.
     *
     * for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
     * {
     *  if (table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type].num_of_access_fields > 0)
     *  {
     *    SHR_IF_ERR_EXIT(dbal_hl_get_by_access_type(unit, access_type, entry_handle, DBAL_INVALID_PARAM));
     *  }
     * }
     */

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_hook_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table_id)
    {
        case DBAL_TABLE_EGRESS_PP_PORT:
            SHR_IF_ERR_EXIT(dbal_hl_hook_egress_pp_port_entry_set(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "no hook supported for table %s operatoion set\n",
                         entry_handle->table->table_name);
            break;
    }

    /*
     * This is a reference to the standart flow for setting an entry. It should be used and modified inside each set
     * hook function.
     * for (access_type = 0; access_type < DBAL_NOF_HL_ACCESS_TYPES; access_type++)
     * {
     *  if (table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[access_type].num_of_access_fields > 0)
     *  {
     *      SHR_IF_ERR_EXIT(dbal_hl_set_by_access_type(unit, access_type, entry_handle));
     * } }
     */
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_hook_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table_id)
    {
        case DBAL_TABLE_EGRESS_PP_PORT:
            /** code should not reach here */
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "res_type_resolution not needed for table egress_pp_port \n");
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "no hook supported for table %s operatoion res_type_resolution\n",
                         entry_handle->table->table_name);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
