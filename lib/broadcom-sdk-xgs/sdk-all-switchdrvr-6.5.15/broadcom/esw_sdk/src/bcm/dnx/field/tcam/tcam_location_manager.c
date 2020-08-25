/**
 * \file        tcam_location_manager.c
 *
 * This file implements all function for the location manager of the TCAM.
 */
/**
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <bcm_int/dnx/field/tcam/tcam_location_manager.h>
#include <bcm_int/dnx/field/tcam/tcam_bank_manager.h>
#include <bcm_int/dnx/field/tcam/tcam_access_profile_manager.h>
#include <bcm_int/dnx/field/field_entry.h>

#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

shr_error_e
dnx_field_tcam_location_find(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 *found)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_find(unit, core, handler_id, entry, location, found));

    if (found)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Found location for entry %d(prio %d): (%d, %d)\n",
                     entry->id, entry->priority, location->bank_id, location->bank_offset);
    }
    else
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Failure finding free location! Location for"
                     "entry %d(prio %d): (%d, %d)\n",
                     entry->id, entry->priority, location->bank_id, location->bank_offset);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location)
{
    uint32 absolute_location;
    uint8 success;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate the new entry by adding it to the entry_location_hash
     */

    absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.
                    entry_location_hash.insert_at_index(unit, core, &(entry->id), absolute_location, &success));

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error adding entry to entry_location_hash!");
    }
    else
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Entry %d added to location hash at %d%s%s\n", entry->id, absolute_location, EMPTY, EMPTY);
    }

    /*
     * Update the location algo regarding entry allocation
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_update_alloc(unit, core, handler_id, entry, location));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_free(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id)
{
    dnx_field_tcam_location_t location;
    uint32 absolute_location;
    uint8 found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Free entry by removing it from entry_location_hash
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.
                    entry_location_hash.find(unit, core, &entry_id, &absolute_location, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error freeing entry %d, entry not allocated", entry_id);
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.delete(unit, core, &entry_id));
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Entry at location %d removed from hash%s%s%s\n", absolute_location, EMPTY, EMPTY, EMPTY);

    /*
     * Update the location algo regarding entry removal
     */
    location.bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(absolute_location);
    location.bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(absolute_location);
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_update_free(unit, core, handler_id, &location));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Moves one entry in location hash from 'location_from' to 'location_to' on the given core.
 *
 * \param [in] unit          - Device ID
 * \param [in] core          - Core ID
 * \param [in] location_from - Location to move the entry from
 * \param [in] location_to   - Location to move the entry to
 *
 * \return
 *  \retval _SHR_E_NONE      - success
 *  \retval _SHR_E_NOT_FOUND - No entry present at 'location_from'
 *  \retval _SHR_E_INTERNAL  - Could not insert entry at new 'location_to'
 */
static shr_error_e
dnx_field_tcam_location_hash_move_one(
    int unit,
    int core,
    uint32 location_from,
    uint32 location_to)
{
    uint32 entry_id;
    uint8 found;
    uint8 success;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the entry from the 'copy_from' location
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.get_by_index
                    (unit, core, location_from, &entry_id, &found));
    /*
     * Sanity check
     */
    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Something went wrong while moving entries\r\n"
                     "Entry at location %d not found\r\n", location_from);
    }
    /*
     * Remove the entry from the 'copy_from' location
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.delete_by_index(unit, core, location_from));
    /*
     * Set the entry in 'copy_to' location
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.insert_at_index
                    (unit, core, &entry_id, location_to, &success));

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong while moving entries\r\n"
                     "Could not insert entry at absolute location %d\r\n", location_to);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_move_extended(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_start,
    uint32 location_end)
{
    uint32 location_to_copy_to;
    uint8 found;
    uint8 backward;
    dnx_field_tcam_location_find_mode_e find_mode;
    dnx_field_tcam_location_t bank_location;
    /*
     * This initial assignment is just for sanity check
     */
    uint32 location_to_copy_from = location_end;

    SHR_FUNC_INIT_VARS(unit);

    backward = (location_start < location_end);
    find_mode = (backward) ? DNX_FIELD_TCAM_LOCATION_FIND_MODE_BACKWARD : DNX_FIELD_TCAM_LOCATION_FIND_MODE_FORWARD;

    /*
     * We start updating from 'location_to' till 'location_from'.
     * Since 'location_to' is empty, we can safley move entries
     * one by one to this location and the ones that get emptied
     * in the way of moving to it.
     */
    location_to_copy_to = location_end;
    while (location_to_copy_from != location_start)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get_extended
                        (unit, core, handler_id, location_to_copy_to, backward, &location_to_copy_from, &found));
        /*
         * Sanity check
         */
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong while moving entries\r\n"
                         "entries location bitmap at location %d has no %s for handler %d\r\n",
                         location_to_copy_to, (backward) ? "prev" : "next", handler_id);
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_hash_move_one(unit, core, location_to_copy_from, location_to_copy_to));

        /*
         * Perform move operation in tcam access and update bank manager/location algo about move operation
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_move(unit, core, handler_id, location_to_copy_from, location_to_copy_to));
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_move
                        (unit, core, handler_id, location_to_copy_from, location_to_copy_to));
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_move_update
                        (unit, core, location_to_copy_from, location_to_copy_to, handler_id));

        /*
         * Advance copy_to location to the next/prev (according to copy direction) empty location
         */
        bank_location.bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location_to_copy_to);
        bank_location.bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(location_to_copy_to);

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_empty_location_find
                        (unit, core, handler_id, &bank_location, find_mode, &location_to_copy_to, &found));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Updates the entry location hash regarding a move operation from location_from
 *  to location_to on the given core that belongs to the handler which its ID is
 *  given handler_id.
 *
 * \param [in] unit          - Device ID
 * \param [in] core          - Core ID
 * \param [in] handler_id    - The handler ID to which the move opreation belongs to
 * \param [in] location_start - The starting location for the move operation
 * \param [in] location_end   - The ending location for the move operation
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_hash_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_start,
    uint32 location_end)
{
    uint32 location_to_copy_to;
    uint8 found;
    uint8 backward;
    /*
     * This initial assignment is just for sanity check
     */
    uint32 location_to_copy_from = location_end;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * We start updating from 'location_to' till 'location_from'.
     * Since 'location_to' is empty, we can safley move entries
     * one by one to this location and the ones that get emptied
     * in the way of moving to it.
     */
    location_to_copy_to = location_end;

    backward = (location_start < location_end);

    while (location_to_copy_from != location_start)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get_extended
                        (unit, core, handler_id, location_to_copy_to, backward, &location_to_copy_from, &found));
        /*
         * Sanity check
         */
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong while moving entries\r\n"
                         "entries location bitmap at location %d has no %s for handler %d\r\n",
                         location_to_copy_to, (backward) ? "prev" : "next", handler_id);
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_hash_move_one(unit, core, location_to_copy_from, location_to_copy_to));

        /*
         * Update tcam access about move operation
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_move(unit, core, handler_id, location_to_copy_from, location_to_copy_to));

        /*
         * Move on to the next location
         */
        location_to_copy_to = location_to_copy_from;
    }
    /*
     * Update the bank manager about the move operation
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_move_update(unit, core, location_start, location_end, handler_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_start,
    uint32 location_end)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Move the entries in the location hash
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_hash_move(unit, core, handler_id, location_start, location_end));

    /*
     * Update the algo about the moving
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_move(unit, core, handler_id, location_start, location_end));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Returns the absoulte location for the given entry_id on the given core.
 *
 * \param [in] unit      - Device ID
 * \param [in] core      - Core to get the location for
 * \param [in] entry_id  - The entry ID to get the location for
 * \param [out] location - The absoulte location for the given entry_id
 *
 * \return
 *   \retval _SHR_E_NONE success
 */
static shr_error_e
dnx_field_tcam_location_entry_id_to_abs_location(
    int unit,
    int core,
    uint32 entry_id,
    uint32 *location)
{
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get entry position from entry_location_hash SWSTATE 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.find(unit, core, &entry_id, location, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry with entry_id: %d not found in entry->location Hash", entry_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * /brief
 *  Returns the entry ID of the entry placed in the given absolute location
 *  in TCAM at the given core.
 *
 * \param [in] unit      - Device ID
 * \param [in] core      - Core ID
 * \param [in] location  - The absolute location of the entry to return the entry ID for
 * \param [out] entry_id - The entry ID for the entry in the given absolute location
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_location_to_entry_id(
    int unit,
    int core,
    uint32 location,
    uint32 *entry_id)
{
    uint8 found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the access_id from the location_entry_hash
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.get_by_index(unit, core, location, entry_id, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry at location: %d not found in location->entry Hash", location);
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_field_tcam_location_handler_first_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 *first_entry_id)
{
    uint32 first_location;
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Start at location 0 to get first entry
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get(unit, core, handler_id, -1, &first_location, &found));

    if (!found)
    {
        *first_entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_location_to_entry_id(unit, core, first_location, first_entry_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_handler_next_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *next_entry_id)
{
    uint32 location;
    uint32 next_location;
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert the current entry_id to its location
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_entry_id_to_abs_location(unit, core, entry_id, &location));

    /*
     * Use given entry's location to find the entry next in place for the given handler
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                    (unit, core, handler_id, location, &next_location, &found));

    if (!found)
    {
        *next_entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
    }
    else
    {
        /*
         * Convert the found location to the entry's id
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_location_to_entry_id(unit, core, next_location, next_entry_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_entry_id_to_location(
    int unit,
    int core,
    uint32 entry_id,
    dnx_field_tcam_location_t * location)
{
    uint32 abs_location;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_entry_id_to_abs_location(unit, core, entry_id, &abs_location));
    location->bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(abs_location);
    location->bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(abs_location);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_bank_entries_count(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_id,
    uint32 *entries_count_out)
{
    uint32 bank_min_address;
    uint32 bank_max_address;
    uint32 address_iter;
    uint32 entries_count;
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init control vars
     */
    entries_count = 0;
    bank_min_address = FIELD_TCAM_LOCATION_ALGO_MIN_BANK_INDEX(bank_id);
    bank_max_address = FIELD_TCAM_LOCATION_ALGO_MAX_BANK_INDEX(bank_id);

    /*
     * Starting position to look from
     */
    address_iter = bank_min_address - 1;
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get(unit, core,
                                                                  handler_id, address_iter, &address_iter, &found));
    /*
     * Loop through all addresses in this bank for the given handler, or
     * until we run out of handler's entries
     */
    while (found && address_iter <= bank_max_address)
    {
        /*
         * If it meets the "while" conditions, then it's a legitemate 
         * entry
         */
        entries_count++;
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get(unit, core,
                                                                      handler_id, address_iter, &address_iter, &found));
    }

    *entries_count_out = entries_count;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_entry_priority_get(
    int unit,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *priority)
{
    uint32 entry_location;
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_id);
    SHR_FUNC_INIT_VARS(unit);

    if (core == DNX_FIELD_TCAM_CORE_ALL)
    {
        core = DNX_FIELD_TCAM_CORE_FIRST;
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_entry_id_to_abs_location(unit, core, entry_id, &entry_location));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_priority_get(unit, core, handler_id, entry_location, priority));

exit:
    SHR_FUNC_EXIT;
}
