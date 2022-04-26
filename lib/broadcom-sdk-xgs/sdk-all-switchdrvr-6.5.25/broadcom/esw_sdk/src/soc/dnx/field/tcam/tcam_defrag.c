/**
 * \file       tcam_defrag.c
 *
 * This file implements all the TCAM defragmentation functions.
 */
/**
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <soc/dnx/field/tcam/tcam_defrag.h>

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/dnx/field/tcam/tcam_access_profile_manager.h>
#include <soc/dnx/field/tcam/tcam_bank_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_location_access.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

/**
 * \brief
 *  Default (JER1) defrag, tries to make a place in the given location for the
 *  given handler_id on the given core.
 *  This is done by:
 *  1. Finding closest empty location to the given location on the given core
 *  2. Moving the handler's entries to the empty location by keeping the order
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler_id to do defrag for
 * \param [in] location   - The location to make empty
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_defrag_default(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location)
{
    uint8 found;
    uint32 empty_place;
    uint32 absolute_location;

    SHR_FUNC_INIT_VARS(unit);

    absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);
    if (absolute_location == DNX_FIELD_TCAM_LOCATION_INVALID)
    {
        /*
         * If no suggested location is given, there's nothing to pursue after this point 
         */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_empty_location_find
                    (unit, core, handler_id, location, DNX_FIELD_TCAM_LOCATION_FIND_MODE_CLOSEST, &empty_place,
                     &found));
    if (found && (absolute_location != empty_place))
    {
        if (absolute_location > empty_place)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_prev_get
                            (unit, core, handler_id, absolute_location, &absolute_location, &found));
            if (!found)
            {
                /*
                 * If no such entry was found, then there's nothing to move 
                 */
                SHR_EXIT();
            }
        }
        else
        {
            /*
             * Check that we have an entry to move 
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                            (unit, core, handler_id, absolute_location - 1, &absolute_location, &found));
            if (!found)
            {
                /*
                 * If no such entry was found, then there's nothing to move 
                 */
                SHR_EXIT();
            }
        }

        /*
         * Perform move operation
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_move(unit, core, handler_id, empty_place, &absolute_location));
        location->bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(absolute_location);
        location->bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(absolute_location);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Not enough resources to add the requested entry\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_defrag(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_mode(unit, core, handler_id, location, DNX_FIELD_TCAM_DEFRAG_MODE_DEFAULT));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_defrag_mode(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location,
    int mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (mode)
    {
        case DNX_FIELD_TCAM_DEFRAG_MODE_DEFAULT:
            SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_default(unit, core, handler_id, location));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported mode - %d\n", mode);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Returns the number of free entries in all banks that belong to the given handler_id at
 *   the given core.
 *   The return is split into two out params, first out param is "nof_free_forward" that
 *   is the number of free entries in all the bank IDs that their ID is larger (or equal)
 *   than the given bank_id, and the second out param is "nof_free_backward which is the
 *   number of free entries in all bank IDs that their ID is smaller than the given bank_id.
 *
 * \param[in] unit               - Device ID
 * \param[in] core               - Core ID
 * \param[in] handler_id         - Handler ID to count number of free entries in its allocated
 *                                 banks.
 * \param[in] bank_id            - Bank ID to split the return value according to (see brief)
 * \param[out] nof_free_forward  - Number of free entries in all handler's banks that their ID
 *                                 is larger (or equal) than the given bank_id
 * \param[out] nof_free_backward - Number of free entries in all handler's banks that their ID
 *                                 is smaller than the given bank_id
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_defrag_banks_nof_free(
    int unit,
    int core,
    uint32 handler_id,
    int bank_id,
    uint32 *nof_free_forward,
    uint32 *nof_free_backward)
{
    int *bank_ids_p = NULL;

    int bank_count;
    int bank_i;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bank_ids_p, sizeof(*bank_ids_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    *nof_free_forward = 0;
    *nof_free_backward = 0;

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get(unit, handler_id, core, bank_ids_p, &bank_count));

    for (bank_i = 0; bank_i < bank_count; bank_i++)
    {
        uint32 bank_nof_free;

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_available_entries_get(unit, core, bank_ids_p[bank_i], 0, &bank_nof_free));

        if (bank_ids_p[bank_i] < bank_id)
        {
            *nof_free_backward = *nof_free_backward + bank_nof_free;
        }
        else
        {
            *nof_free_forward = *nof_free_forward + bank_nof_free;
        }
    }

exit:
    SHR_FREE(bank_ids_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Returns the location of the entry that belongs to the given handler_id in the given bank_id
 *  that its index is 'num' at max at the given core (if less than 'num' entries are present in
 *  the given bank for the given handler, then the last entry is returned).
 *  If reverse is TRUE, iteration is reveresed and index counting starts from the last entry.
 *
 * \param[in] unit       - Device ID
 * \param[in] core       - Core ID
 * \param[in] handler_id - Handler ID to get the location of the entry for
 * \param[in] bank_id    - Bank ID to iterate over the handler's entries in
 * \param[in] num        - The max index of the entry to return the location for
 * \param[in] reverse    - Whether to reverse the order of iteration
 * \param[out] location  - The location of the entry that meets the function conditions
 *                         (see brief)
 * \param[out] found     - Whether at least one entry for the given handler is found in the given
 *                         bank
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_defrag_entry_in_bank_get(
    int unit,
    int core,
    uint32 handler_id,
    int bank_id,
    uint32 num,
    uint8 reverse,
    uint32 *location,
    uint8 *found)
{
    uint8 next_found;
    uint32 next_location;

    SHR_FUNC_INIT_VARS(unit);

    *found = FALSE;

    *location = (reverse) ?
        DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id + 1, 0) :
        DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, 0) - 1;

    while (num > 0)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get_extended
                        (unit, core, handler_id, *location, reverse, &next_location, &next_found));
        if (!next_found || (DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(next_location) != bank_id))
        {
            /*
             * Entry not found or not in requested bank 
             */
            break;
        }
        /*
         * At least one found, so found is set to TRUE 
         */
        *found = TRUE;
        /*
         * Iteratre to next entry 
         */
        *location = next_location;
        num--;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Evacuates the entries of the given handler from the given bank at the given core to other
 *  banks allocated by the handler at the same core.
 *  This function starts moving the entries as long as there's free space to move them to,
 *  when no more free space is left before moving all the entries, the function fails with
 *  _SHR_E_FAIL as the error return value.
 *
 * \param[in] unit       - Device ID
 * \param[in] core       - Core ID (SOC_CORE_ALL excluded)
 * \param[in] handler_id - Handler to evacuate its entries from the bank
 * \param[in] bank_id    - Bank to evacuate Handler's entries from
 *
 * \return
 *  \retval _SHR_E_NONE     - Success
 *  \retval _SHR_E_FAIL     - No more space left to move entries to
 *  \retval _SHR_E_INTERNAL - Something wrong happened with the algorithm (sanity checks failure)
 */
static shr_error_e
dnx_field_tcam_defrag_bank_evacuate_internal(
    int unit,
    int core,
    uint32 handler_id,
    int bank_id)
{
    uint32 nof_free_forward;
    uint32 nof_free_backward;
    uint8 found;
    uint32 dummy;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Retrieve number of free spaces in the previous and the next banks before/after this bank
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_banks_nof_free
                    (unit, core, handler_id, bank_id, &nof_free_forward, &nof_free_backward));

    /**
     * If free spots are available in the previous banks, move entries in this bank until either no
     * free space is left, or no more entries in the given bank for the given handler are present
     */
    if (nof_free_backward > 0)
    {
        uint32 first_empty;
        uint32 location_from;
        dnx_field_tcam_location_t first_location = { 0, 0 };
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_empty_location_find
                        (unit, core, handler_id, &first_location, DNX_FIELD_TCAM_LOCATION_FIND_MODE_FORWARD,
                         &first_empty, &found));
        /*
         * Sanity check 
         */
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Unexpected error while evacuating handler %d from bank %d core %d: No empty location found while expecting at least one to be\r\n",
                         handler_id, bank_id, core);
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_entry_in_bank_get
                        (unit, core, handler_id, bank_id, nof_free_backward, FALSE, &location_from, &found));
        if (!found)
        {
            /*
             * No entries for handler found on bank, we exit silently 
             */
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_move_extended(unit, core, handler_id, location_from, first_empty));
    }
    /** Same as backward empty spaces but for forward ones */
    if (nof_free_forward > 0)
    {
        uint32 last_empty;
        uint32 location_from;
        dnx_field_tcam_location_t last_location =
            { dnx_data_field.tcam.nof_banks_get(unit) - 1, dnx_data_field.tcam.nof_small_bank_lines_get(unit) };
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_empty_location_find
                        (unit, core, handler_id, &last_location, DNX_FIELD_TCAM_LOCATION_FIND_MODE_BACKWARD,
                         &last_empty, &found));
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Unexpected error while evacuating handler %d from bank %d core %d: No empty location found while expecting at least one to be\r\n",
                         handler_id, bank_id, core);
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_entry_in_bank_get
                        (unit, core, handler_id, bank_id, nof_free_forward, TRUE, &location_from, &found));
        if (!found)
        {
            /*
             * No entries for handler found on bank, we exit silently 
             */
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_move_extended(unit, core, handler_id, location_from, last_empty));
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_defrag_entry_in_bank_get(unit, core, handler_id, bank_id, 1, TRUE, &dummy, &found));
    if (found)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "Failed to evacuate handler %d from bank %d on core %d, no more empty space in other allocated banks to move handler's entries to\r\n",
                     handler_id, bank_id, core);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_defrag_bank_evacuate(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[],
    bcm_core_t core_ids[])
{
    bcm_core_t core;
    int bank_idx;
    int found;
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    /**
     * Remove handler from banks.
     * This is done before calling evacuation so that empty space in the banks we want to evacuate aren't used
     * by the internal evacuation to move entries to.
     */
    for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
    {
        DNXCMN_CORES_ITER(unit, core_ids[bank_idx], core)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_remove_handler_from_bank
                            (unit, bank_ids[bank_idx], handler_id, core, &found));
            if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_remove_handler_from_bank
                                (unit, bank_ids[bank_idx] + 1, handler_id, core, &found));
            }
        }
    }

    for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
    {
        /** Evacuate all entries on current bank from given cores */
        DNXCMN_CORES_ITER(unit, core_ids[bank_idx], core)
        {
            uint32 entries_on_bank;
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_bank_entries_count
                            (unit, core, handler_id, bank_ids[bank_idx], &entries_on_bank));
            if (entries_on_bank == 0)
            {
                /*
                 * No entries on this core, skip to next iteration 
                 */
                continue;
            }
            else if (fg_params.direct_table)
            {
                /*
                 * Its not allowed to evacuate a DT handler when it has entries left on bank 
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_add_handler_to_bank(unit, bank_ids[bank_idx], handler_id, core));
                SHR_ERR_EXIT(_SHR_E_DISABLED,
                             "Can't evacuate DT handler (%d) from bank (%d) when there are valid entries left",
                             handler_id, bank_ids[bank_idx]);
            }
            SHR_SET_CURRENT_ERR(dnx_field_tcam_defrag_bank_evacuate_internal
                                (unit, core, handler_id, bank_ids[bank_idx]));
            if (SHR_FUNC_ERR())
            {
                char msg[100];
                sal_snprintf(msg, 100, "Failed evacuating handler %d from bank %d!", handler_id, bank_ids[bank_idx]);
                /** Re-add handler to remaining unevacuated banks when evacuation fails at a certain point */
                for (; bank_idx < nof_banks; bank_idx++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_add_handler_to_bank
                                    (unit, bank_ids[bank_idx], handler_id, core));
                    if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
                    {
                        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_add_handler_to_bank
                                        (unit, bank_ids[bank_idx] + 1, handler_id, core));
                    }
                }
                SHR_ERR_EXIT(SHR_GET_CURRENT_ERR(), "%s", msg);
            }
        }
        DNXCMN_CORES_ITER(unit, core_ids[bank_idx], core)
        {
            /** Complete removal of bank (no entry on given core) */
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_remove(unit, handler_id, bank_ids[bank_idx], core));

            if (!fg_params.direct_table && fg_params.key_size != dnx_data_field.tcam.key_size_double_get(unit))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free(unit, bank_ids[bank_idx], handler_id, core));
            }

            if (fg_params.key_size == dnx_data_field.tcam.key_size_double_get(unit))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_bank_remove
                                (unit, handler_id, bank_ids[bank_idx] + 1, core));

                SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free(unit, bank_ids[bank_idx] + 1, handler_id, core));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
