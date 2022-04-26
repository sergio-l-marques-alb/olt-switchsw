/**
 * \file       tcam_location_algo.c
 * This file implements all the functions for the TCAM Location Algo.
 */
/**
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <soc/dnx/field/tcam/tcam_location_algo.h>

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/dnx/field/tcam/tcam_bank_algo.h>
#include <soc/dnx/field/tcam/tcam_bank_manager.h>
#include <soc/dnx/field/tcam/tcam_access_profile_manager.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_location_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define DNX_FIELD_TCAM_LOCATION_IS_LOCATION_BANK_ODD(location) (DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location) % (2))

/* Enable this for debugging */
/* #define DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE */

shr_error_e
dnx_field_tcam_location_priorities_range_init(
    int unit)
{
    sw_state_ll_init_info_t priorities_init_info;
    dnx_field_tcam_location_prio_list_key_t key;
    dnx_field_tcam_location_range_t range;
    int core;
    uint32 handler_id;
    sw_state_ll_node_t iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.alloc(unit));
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * -- Init the priorities list -- 
         */
        sal_memset(&priorities_init_info, 0x0, sizeof(priorities_init_info));
        /*
         * Max piorities is equal to the max 80b entries we can have in TCAM + 2 for boundary nodes
         */
        priorities_init_info.max_nof_elements =
            dnx_data_field.tcam.nof_entries_80_bits_get(unit) + 1 + 2 * dnx_data_field.tcam.nof_tcam_handlers_get(unit);

        priorities_init_info.key_cmp_cb_name = "tcam_sorted_list_cmp_priority";

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.create_empty(unit, core, &priorities_init_info));

        /*
         * Add two 'limit' nodes per handler to the linked list to prevent limit checking
         * within the linked list handling code, and also to enforce unique region per hander
         */
        for (handler_id = 0; handler_id < dnx_data_field.tcam.nof_tcam_handlers_get(unit); handler_id++)
        {
            /*
             * The smallest priority that exists per handler
             */
            key.priority = 0;
            key.handler_id = handler_id;
            range.min = 0xFFFFFFFF;
            range.max = 0xFFFFFFFF;
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.add(unit, core, &key, &range));

            /*
             * The highest priority valid for TCAM entry per handler.
             */
            key.priority = dnx_data_field.tcam.max_tcam_priority_get(unit);
            key.handler_id = handler_id;
            range.min = dnx_data_field.tcam.tcam_banks_last_index_get(unit);
            range.max = dnx_data_field.tcam.tcam_banks_last_index_get(unit);
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.add(unit, core, &key, &range));

            /*
             * Set handler's prio_list_first/last 
             */
            if (handler_id == 0)
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.get_first(unit, core, &iter));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.next_node(unit, core, iter, &iter));
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                            prio_list_first_iter.set(unit, handler_id, core, iter));

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.next_node(unit, core, iter, &iter));
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                            prio_list_last_iter.set(unit, handler_id, core, iter));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Returns the insertion range suitable for the given priority according to the
 *  content of handler_priority_list on the given core.
 *
 * \param [in] unit             - Device ID
 * \param [in] core             - Core ID
 * \param [in] handler_id       - The handler ID to get the insertion range from its priority list
 * \param [in] priority         - The priority to return the insertion range for
 * \param [out] insertion_range - The suitable insertion range for the given priority
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_insertion_range_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 priority,
    dnx_field_tcam_location_range_t * insertion_range)
{
    uint8 found;
    sw_state_ll_node_t iter, prev_iter, next_iter;
    dnx_field_tcam_location_prio_list_key_t node_key;
    dnx_field_tcam_location_range_t next_data, prev_data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * This ha!ck enables the user to keep using priority 0 while also enabling us to add a priority 0 limit node to the
     * linked list
     */
    priority++;
    node_key.priority = priority;
    node_key.handler_id = handler_id;

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.find(unit, core, &iter, &node_key, &found));

    if (found)
    {
        /*
         * When entry is found, iter is current node
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.previous_node(unit, core, iter, &prev_iter));
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_value(unit, core, prev_iter, &prev_data));
    }
    else
    {
        /*
         * When entry is not found, iter is prev node
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_value(unit, core, iter, &prev_data));
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.next_node(unit, core, iter, &next_iter));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_value(unit, core, next_iter, &next_data));
    insertion_range->min = prev_data.max + 1;
    insertion_range->max = next_data.min - 1;
#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "insertion range: min: %d max: %d%s%s\n", insertion_range->min, insertion_range->max, EMPTY, EMPTY);
#endif

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Updates the priority_list on the given core, about the removal of an entry with the given priority/place
 *  See dnx_field_tcam_location_algo_priority_list_entry_add for more information about the procedure
 *
 * \param [in] unit          - Device ID
 * \param [in] core          - Core ID
 * \param [in] handler_id    - The handler ID to remove the place from its priority_list
 * \param [in] priority      - The priority of the entry to remove
 * \param [in] place         - The location of the entry to remove
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_priority_list_entry_remove(
    int unit,
    int core,
    uint32 handler_id,
    uint32 priority,
    uint32 place)
{
    uint8 found = TRUE;
    sw_state_ll_node_t iter;
    dnx_field_tcam_location_range_t range;
    dnx_field_tcam_location_prio_list_key_t node_key;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Removing entry with priority %d location 0x%08x from handler %d\n",
                 core, priority, place, handler_id);
#endif

    /*
     * This ha!ck enables the user to keep using priority 0 while also enabling us to add a priority 0 limit node to the
     * linked list
     */
    priority++;

    node_key.priority = priority;
    node_key.handler_id = handler_id;

    /*
     * Get entry's node 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.find(unit, core, &iter, &node_key, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal Error on Priority-list remove entry:"
                     " Node for priority %d (handler %d) not found\n", priority - 1, handler_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_value(unit, core, iter, &range));
    /*
     * Sanity check 
     */
    if (range.min > place || place > range.max)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal Error on Priority-list remove entry:"
                     " Node for priority %d (handler %d) has range (0x%08x:0x%08x) while entry location is 0x%08x",
                     priority - 1, handler_id, range.min, range.max, place);
    }
    /**
     * When min is equal to max, node should be deleted
    */
    if (range.min == range.max)
    {
#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Removing node%s%s%s\n", core, EMPTY, EMPTY, EMPTY);
#endif
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.remove_node(unit, core, iter));
    }
    else if (range.min == place)
    {
        /*
         * Get the next entry for this handler to be the new min
         */
#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Updateing lower limit from: 0x%08x%s%s\n", core, range.min,
                     EMPTY, EMPTY);
#endif
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                        (unit, core, handler_id, range.min, &(range.min), &found));
#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Updateing lower limit to: 0x%08x%s%s\n", core, range.min, EMPTY,
                     EMPTY);
#endif
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_update(unit, core, iter, &range));
    }
    else if (range.max == place)
    {
        /*
         * Get the prev entry for this handler to be the new min
         */
#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Updateing upper limit from: 0x%08x%s%s\n", core, range.max,
                     EMPTY, EMPTY);
#endif
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_prev_get
                        (unit, core, handler_id, range.max, &(range.max), &found));
#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Updateing upper limit to: 0x%08x%s%s\n", core, range.max, EMPTY,
                     EMPTY);
#endif
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_update(unit, core, iter, &range));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *  Updates the priority list on the given core to consider the given place for the
 *  given priority. This is done by first trying to search for the given priority's
 *  node, if no such node is found, a new entry gets add for the given priority,
 *  with the given place being both the min/max limits for the added node.
 */
/*
 * \param [in] unit          - Device ID
 * \param [in] core          - Core ID
 * \param [in] handler_id    - The handler ID to add the new place to its priority list
 * \param [in] priority      - The given priority to add/update.
 * \param [in] place         - The given place to update limits according to.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_priority_list_entry_add(
    int unit,
    int core,
    uint32 handler_id,
    uint32 priority,
    uint32 place)
{
    uint8 found;
    sw_state_ll_node_t iter;
    dnx_field_tcam_location_prio_list_key_t key;
    dnx_field_tcam_location_range_t range;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * This ha!ck enables the user to keep using priority 0 while also enabling us to add a priority 0 limit node to the
     * linked list
     */
    priority++;

    key.priority = priority;
    key.handler_id = handler_id;
#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Adding priority %d for handler %d on location 0x%08x\n", core,
                 priority - 1, handler_id, place);
#endif

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.find(unit, core, &iter, &key, &found));

    if (found)
    {
#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Pre-existing node found for priority%s%s%s\n", core, EMPTY,
                     EMPTY, EMPTY);
#endif
        /*
         * Priority node already in the list.
         * Update edges in case affected by move
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_value(unit, core, iter, &range));
        if (range.min > place)
        {
#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Updating old lower boundary 0x%08x with new location%s%s\n",
                         core, range.min, EMPTY, EMPTY);
#endif
            range.min = place;
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_update(unit, core, iter, &range));
        }
        if (range.max < place)
        {
#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Updating old upper boundary 0x%08x with new location%s%s\n",
                         core, range.max, EMPTY, EMPTY);
#endif
            range.max = place;
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_update(unit, core, iter, &range));
        }
    }
    else
    {
        /*
         * Priority node does not exist, create a new one
         */
        range.min = place;
        range.max = place;

#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Prio-list core%d: Adding new node for priority%s%s%s\n", core, EMPTY, EMPTY,
                     EMPTY);
#endif

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.add(unit, core, &key, &range));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Search for a suitable place for an entry in the given search_range.
 *  The search is performed in TCAM SWSTATE on the given core.
 *  The size of the entry is calculated according to the key_size given.
 *
 * \param [in] unit         - Device ID
 * \param [in] core         - Core ID
 * \param [in] handler_id   - The handler ID to find empty space for
 * \param [in] key_size     - The size of the key for the entry's database
 * \param [in] search_range - The range to search in
 * \param [in] forward      - If true, do a forward search, otherwise do a backward search
 * \param [out] place       - The empty location suitable for the entry to be placed in
 * \param [out] found       - TRUE if a suitable location was found or FALSE otherwise
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_search_for_entry(
    int unit,
    int core,
    uint32 handler_id,
    uint32 key_size,
    dnx_field_tcam_location_range_t * search_range,
    uint8 forward,
    uint32 *place,
    uint8 *found)
{
    int bank_id;

    SHR_FUNC_INIT_VARS(unit);

    *found = FALSE;

    while (!*found)
    {
        uint8 contains_half_key_handler;

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        tcam_banks_occupation_bitmap.get_next_in_range(unit, core, search_range->min, search_range->max,
                                                                       forward, 0, place, found));
        if (*found)
        {
            /*
             * We've a candidate, now we need to check the other adjacent slots for entries larger than one slot
             */
            uint8 occupied1 = FALSE;
            uint8 occupied2 = FALSE;
            uint8 occupied3 = FALSE;
            uint8 check_80b = FALSE;

            /*
             * For single/double key entries, place should default to even addresses.
             * If place found is odd, we should continue searching.
             */
            if (key_size != dnx_data_field.tcam.key_size_half_get(unit) && *place % 2)
            {
                /*
                 * Continue searching from the last place held as candidate
                 */
                if (forward)
                {
                    search_range->min = *place + 1;
                }
                else
                {
                    search_range->max = *place - 1;
                }
                *found = FALSE;
                continue;
            }

            /*
             * Check whether complementary entry in the same line belongs to another FG
             */
            if (key_size == dnx_data_field.tcam.key_size_half_get(unit)
                && !dnx_data_field.tcam.feature_get(unit, dnx_data_field_tcam_tcam_80b_traffic_safe_write))
            {
                uint32 comp_place = DNX_FIELD_TCAM_LOCATION_COMPLEMENTARY_GET(*place);
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                                tcam_banks_occupation_bitmap.is_occupied(unit, core, comp_place, &occupied1));
                if (occupied1)
                {
                    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                                    entries_occupation_bitmap.is_occupied(unit, handler_id, core, comp_place,
                                                                          &occupied1));
                    /*
                     * Flip occupied to skip candidate place if its complementary is occupied by another FG. This is
                     * done to prevent different 80b FGs from sharing the same line in TCAM 
                     */
                    occupied1 = occupied1 ? FALSE : TRUE;
                }
                else
                {
                    check_80b = TRUE;
                }
            }

            /*
             * Check if other slots are free according to the entry size we want to add
             */
            if (key_size >= dnx_data_field.tcam.key_size_single_get(unit))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                                tcam_banks_occupation_bitmap.is_occupied(unit, core, *place + 1, &occupied1));
            }
            if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
            {
                uint32 next_bank_place = *place + dnx_data_field.tcam.hw_bank_size_get(unit);
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                                tcam_banks_occupation_bitmap.is_occupied(unit, core, next_bank_place, &occupied2));
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                                tcam_banks_occupation_bitmap.is_occupied(unit, core, next_bank_place + 1, &occupied3));
            }

            bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(*place);
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_algo_bank_contains_half_key_handler
                            (unit, bank_id, core, &contains_half_key_handler));
            /*
             * Check for free line mandate when bank contains half-key sized handler
             */
            if (check_80b || (key_size >= dnx_data_field.tcam.key_size_single_get(unit) && contains_half_key_handler))
            {
                uint32 nof_free_160_entries;
                /*
                 * If complementary is empty and only one free line left, skip candidate to enforce free one-line
                 * mandate 
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_available_entries_get
                                (unit, core, bank_id, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE, &nof_free_160_entries));
                if (nof_free_160_entries == 1)
                {
                    /*
                     * Turn on the skip flag 
                     */
                    occupied1 = TRUE;
                }
                if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
                {
                    uint32 nof_free_160_entries;
                    int comp_bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(*place) + 1;
                    /*
                     * If complementary is empty and only one free line left, skip candidate to enforce free one-line
                     * mandate 
                     */
                    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_available_entries_get
                                    (unit, core, comp_bank_id, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE,
                                     &nof_free_160_entries));
                    if (nof_free_160_entries == 1)
                    {
                        /*
                         * Turn on the skip flag 
                         */
                        occupied1 = TRUE;
                    }
                }
            }

            /*
             * Check flags that indicate whether we should continue searching or stop
             */
            if (occupied1 | occupied2 | occupied3)
            {
                /*
                 * Continue searching from the last place held as candidate
                 */
                if (forward)
                {
                    search_range->min = *place + 1;
                }
                else
                {
                    search_range->max = *place - 1;
                }
                *found = FALSE;
            }
        }
        else
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Revises the given suggestion to be included in the valid range of the handler.
 *  Key size is needed since Double key sized handlers can't start on odd bank numbers
 *
 * \param [in] unit           - Device ID
 * \param [in] key_size       - The handler's key size the suggestion belongs to
 * \param [in,out] suggestion - The given suggestion (might be changed to handler's vaild range)
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_suggestion_revise(
    int unit,
    uint32 key_size,
    uint32 *suggestion)
{
    uint32 bank_id;

    SHR_FUNC_INIT_VARS(unit);

    if (*suggestion == DNX_FIELD_TCAM_LOCATION_INVALID)
    {
        SHR_EXIT();
    }

    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(*suggestion);
    if (!FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
    {
        /*
         * Check suggestion is in a valid range of the small banks (since small banks are not continuous)
         */
        if (*suggestion > DNX_FIELD_TCAM_LOCATION_BANK_LAST(bank_id))
        {
            /*
             * Suggestion is above the small bank limit, therefore we re-set it to the min index of the bank afterwards
             */
            *suggestion = DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_id + 1);
        }
    }
    if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        /*
         * Double sized handlers can't start from odd banks entries
         */
        bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(*suggestion);
        if (bank_id % 2)
        {
            /*
             * Round suggestion to the min entry of the next bank
             */
            *suggestion = DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_id + 1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Searches for an empty place in 80b granularity inside the given range in the given bitmap
 *  of the given core.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler ID to find empty space for
 * \param [in] range      - The range to search inside
 * \param [in] bank_ids   - The bank IDs to search in
 * \param [in] bank_count - Number of bank IDs to search in
 * \param [out] place     - The empty place in 80b granularity found inside the given range
 * \param [out] found     - Whether an empty space in range was found or not
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_search_in_range(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_range_t * range,
    int *bank_ids,
    int bank_count,
    uint32 *place,
    uint8 *found)
{
    int bank_index;
    dnx_field_tcam_location_range_t bank_range;
    dnx_field_tcam_location_range_t search_range;
    uint32 key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    *found = FALSE;
    search_range.min = -1;
    search_range.max = -1;

    bank_range.max = dnx_data_field.tcam.tcam_banks_size_get(unit);
    for (bank_index = 0; bank_index < bank_count; bank_index++)
    {
        if (key_size == dnx_data_field.tcam.key_size_double_get(unit) && bank_ids[bank_index] % 2)
        {
            /*
             * Double sized key FGs can't start from odd bank numbers
             */
            continue;
        }
        /*
         * Bank index is in 80b granularity
         */
        bank_range.min = DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_ids[bank_index]);
        bank_range.max = DNX_FIELD_TCAM_LOCATION_BANK_LAST(bank_ids[bank_index]);
        if ((bank_range.min > range->max) || (range->max == -1))
        {
            /*
             * Banks are sorted, so if range below the current bank
             * then there's no point in searching the next banks
             */
            break;
        }
        if (range->min > bank_range.max)
        {
            /*
             * Range is outside this bank, skip this bank.
             */
            continue;
        }
        search_range.min = MAX(bank_range.min, range->min);
        search_range.max = MIN(bank_range.max, range->max);
        /*
         * Search for an empty slot(s) for the entry in the search_range
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_search_for_entry
                        (unit, core, handler_id, key_size, &search_range, TRUE, place, found));

        if (*found)
        {
            break;
        }
    }
    if (!(*found))
    {
        /*
         * - -
         *   In case no empty place was found, we return a
         *   valid place suggestion for this entry to be in,
         *   to later be used by defrag.
         *                                               - -
         */

        /*
         * Get global MIN/MAX range for all banks.
         * Since banks are sorted, it's as easy as the getting min/max for the first/last indexes.
         */
        uint32 global_min = (bank_count == 0) ? 0 : DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_ids[0]);
        uint32 global_max = (bank_count == 0) ? 0 : DNX_FIELD_TCAM_LOCATION_BANK_LAST(bank_ids[bank_count - 1]) + 1;
        if (range->max == -1)
        {
            *place = 0;
        }
        else if (range->max < global_max)
        {
            *place = range->max + 1;
        }
        else if (range->min > global_min)
        {
            /*
             * This time we do the opposite check for the range min.
             */
            *place = range->min + 1;
        }
        else
        {
            /*
             * When neither range limits is inside the bank, there's no place to suggest - Invalid is returned.
             */
            *place = DNX_FIELD_TCAM_LOCATION_INVALID;
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_suggestion_revise(unit, key_size, place));
    }

#ifdef DNX_FIELD_TCAM_LOCATION_ALGO_DEBUG_ENABLE
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Search in range returned %d found: %d%s%s\n", *place, *found, EMPTY, EMPTY);
#endif

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Changes state of the global occupation bitmap of the given core to the given new_state
 *  at the given location.
 *  This function decides which bits should be updated according to the key_size given.
 *
 * \param [in] unit      - Device ID
 * \param [in] core      - Core ID
 * \param [in] key_size  - Size of the key for the entry which its state is going to change
 * \param [in] location  - The start location for the entry we want to change the state for
 * \param [in] new_state - The new state to set for the relevant bits for the entry.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_occ_bitmap_state_change(
    int unit,
    int core,
    uint32 key_size,
    uint32 location,
    uint8 new_state)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                    tcam_banks_occupation_bitmap.status_set(unit, core, location, new_state));
    if (key_size >= dnx_data_field.tcam.key_size_single_get(unit))
    {
        /*
         * (*2*) Occupy second 80b
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        tcam_banks_occupation_bitmap.status_set(unit, core, location + 1, new_state));
    }
    if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        /*
         * (*3*) Occupy 160b in the next bank
         */
        uint32 next_bank_location = location + dnx_data_field.tcam.hw_bank_size_get(unit);
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        tcam_banks_occupation_bitmap.status_set(unit, core, next_bank_location, new_state));
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        tcam_banks_occupation_bitmap.status_set(unit, core, next_bank_location + 1, new_state));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Changes state of the global occupation bitmap of the given core to occupied at
 *  the given location.
 *  This function decides which bits should be updated according to the key_size given.
 *
 * \param [in] unit      - Device ID
 * \param [in] core      - Core ID
 * \param [in] key_size  - Size of the key for the entry which is going to be marked as occupied
 * \param [in] location  - The start location for the entry which is going to be marked as occupied
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_occ_bitmap_occupy(
    int unit,
    int core,
    uint32 key_size,
    uint32 location)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_state_change(unit, core, key_size, location, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Changes state of the global occupation bitmap of the given core to free at the
 *  given location.
 *  This function decides which bits should be updated according to the key_size given.
 *
 * \param [in] unit      - Device ID
 * \param [in] core      - Core ID
 * \param [in] key_size  - Size of the key for the entry which is going to be marked as free
 * \param [in] location  - The start location for the entry which is going to be marked as free
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_occ_bitmap_free(
    int unit,
    int core,
    uint32 key_size,
    uint32 location)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_state_change(unit, core, key_size, location, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Changes state of the handler occupation bitmap of the given core to the given
 *  new_state at the given location.
 *  This function decides which bits should be updated according to the key_size given.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler ID to change its occupation bitmap state
 * \param [in] key_size   - Size of the key for the entry which its state is going to change
 * \param [in] location   - The start location for the entry we want to change the state for
 * \param [in] new_state  - The new state to set for the relevant bits for the entry.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_handler_bitmap_state_change(
    int unit,
    int core,
    uint32 handler_id,
    uint32 key_size,
    uint32 location,
    uint8 new_state)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Unlike the occupation bitmap, the handler bitmap doesn't need to mark
     * all the half-entries it occupies, instead, we only mark the start of
     * the entry which will make it easier to iterate through the bitmap.
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    entries_occupation_bitmap.status_set(unit, handler_id, core, location, new_state));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Changes state of the handler occupation bitmap of the given core to occupied at the
 *  given location.
 *  This function decides which bits should be updated according to the key_size given.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler ID to occupy the location for
 * \param [in] key_size   - Size of the key for the entry which is going to be marked as occupied
 * \param [in] location   - The start location for the entry which is going to be marked as occupied
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_handler_bitmap_occupy(
    int unit,
    int core,
    uint32 handler_id,
    uint32 key_size,
    uint32 location)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_state_change
                    (unit, core, handler_id, key_size, location, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Changes state of the handler occupation bitmap of the given core to free at the
 *  given location.
 *  This function decides which bits should be updated according to the key_size given.
 *
 * \param [in] unit       - Device ID
 * \param [in] core       - Core ID
 * \param [in] handler_id - The handler ID to free the location for
 * \param [in] key_size   - Size of the key for the entry which is going to be marked as free
 * \param [in] location   - The start location for the entry which is going to be marked as free
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_location_algo_handler_bitmap_free(
    int unit,
    int core,
    uint32 handler_id,
    uint32 key_size,
    uint32 location)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_state_change
                    (unit, core, handler_id, key_size, location, FALSE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_update_free(
    int unit,
    int core,
    uint32 handler_id,
    uint32 priority,
    dnx_field_tcam_location_t * location,
    uint8 is_dt)
{

    uint32 key_size;
    uint32 place;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    place = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_free(unit, core, handler_id, key_size, place));

    if (!is_dt)
    {
        /**
         * DT does not play a role in the global bitmap (it only allocates action slot), and does not need
         * a priority list
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_free(unit, core, key_size, place));

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_priority_list_entry_remove
                        (unit, core, handler_id, priority, place));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_update_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 is_dt)
{

    uint32 key_size;
    uint32 insertion_place;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    insertion_place = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_occupy
                    (unit, core, handler_id, key_size, insertion_place));

    if (!is_dt)
    {
        /**
         * DT does not play a role in the global bitmap (it only allocates action slot), and does not need
         * a priority list
         */

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_occupy(unit, core, key_size, insertion_place));

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_priority_list_entry_add
                        (unit, core, handler_id, entry->priority, insertion_place));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_find(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 *found)
{
    int *bank_ids_p = NULL;

    int bank_count;
    dnx_field_tcam_location_range_t insertion_range;
    uint32 insertion_place;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bank_ids_p, sizeof(*bank_ids_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    insertion_range.min = 0;
    insertion_range.max = 0;

    /*
     * Get the banks used by the current handler
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get(unit, handler_id, core, bank_ids_p, &bank_count));
    /*
     * - -
     *     1. Find the insertion range
     *                              - -
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_insertion_range_get
                    (unit, core, handler_id, entry->priority, &insertion_range));

    /*
     * - -
     *     2. Find an empty spot inside the
     *        insertion range
     *                                   - -
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_search_in_range(unit, core, handler_id,
                                                                 &insertion_range,
                                                                 bank_ids_p, bank_count, &insertion_place, found));

    /*
     * When no insertion place was found, the 'insertion_place' variable might represent
     * the suggested location to insert the new entry at.
     */
    location->bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(insertion_place);
    location->bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(insertion_place);

exit:
    SHR_FREE(bank_ids_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Updates the given priority list of the given core regarding a move operation
 *  of one entry from location_from to location_to that belongs to the handler which its ID
 *  is given handler_id.
 *
 * \param [in] unit          - Device ID
 * \param [in] core          - Core ID
 * \param [in] handler_id    - The handler ID to which the move opreation belongs to
 * \param [in] location_from - The location from which the entry was moved from
 * \param [in] location_to   - The location to which the entry was moved to
 * \param [in] priority      - Priority of the entry to be moved
 *
 * \return
 *  \retval _SHR_E_NONE - success
 *  \retval _SHR_E_INTERNAL - internal error due to unexpected data in given list
 */
static shr_error_e
dnx_field_tcam_location_algo_prio_list_move_one(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_from,
    uint32 location_to,
    uint32 priority)
{
    sw_state_ll_node_t iter;
    dnx_field_tcam_location_range_t range;
    uint8 found;
    dnx_field_tcam_location_prio_list_key_t node_key;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * This ha!ck enables the user to keep using priority 0 while also enabling us to add a priority 0 limit node to the
     * linked list
     */
    priority++;

    node_key.priority = priority;
    node_key.handler_id = handler_id;

    /*
     * Get entry's node 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.find(unit, core, &iter, &node_key, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal Error on Priority-list move-one: entry-to-be-moved node with"
                     " priority %d on handler %d not found\n", priority - 1, handler_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_value(unit, core, iter, &range));
    /*
     * Update edges in case affected by move 
     */
    if (range.min == location_from)
    {
        range.min = location_to;
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_update(unit, core, iter, &range));
    }
    if (range.max == location_from)
    {
        range.max = location_to;
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_update(unit, core, iter, &range));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Updates the given priority list of the given core regarding a move operation
 *  from location_from to location_to that belongs to the handler which its ID
 *  is given handler_id.
 *
 * \param [in] unit          - Device ID
 * \param [in] core          - Core ID
 * \param [in] handler_id    - The handler ID to which the move opreation belongs to
 * \param [in] location_from - The location from which the move operation starts
 * \param [in] location_to   - The location to which the move operation ends
 *
 * \return
 *  \retval _SHR_E_NONE - success
 *  \retval _SHR_E_INTERNAL - internal error due to unexpected data in given list
 */
static shr_error_e
dnx_field_tcam_location_algo_prio_list_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_from,
    uint32 location_to)
{
    sw_state_ll_node_t iter;
    uint32 first_iter;
    uint32 last_iter;
    dnx_field_tcam_location_range_t range;
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate through the sub-list of the given handler to perform the moving operation on the handler's nodes
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    prio_list_first_iter.get(unit, handler_id, core, &first_iter));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    prio_list_last_iter.get(unit, handler_id, core, &last_iter));

    for (iter = first_iter; iter != last_iter;
         dnx_field_tcam_location_sw.priorities_range.next_node(unit, core, iter, &iter))
    {
        found = TRUE;
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_value(unit, core, iter, &range));
        /*
         * Check for each range limit if it's in the moving range and update it
         */
        if (range.min >= location_from && range.min <= location_to)
        {
            /*
             * This is the case where location_from < location_to
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                            (unit, core, handler_id, range.min, &(range.min), &found));
        }
        else if (range.min <= location_from && range.min >= location_to)
        {
            /*
             * This is the case where location_from > location_to
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_prev_get
                            (unit, core, handler_id, range.min, &(range.min), &found));
        }
        /*
         * Sanity check
         */
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while moving in priority list for"
                         " handler %d\r\n No next or prev found for location %d\r\n", handler_id, range.min);
        }

        if (range.max >= location_from && range.max <= location_to)
        {
            /*
             * This is the case where location_from < location_to
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                            (unit, core, handler_id, range.max, &(range.max), &found));
        }
        else if (range.max <= location_from && range.max >= location_to)
        {
            /*
             * This is the case where location_from > location_to
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_prev_get
                            (unit, core, handler_id, range.max, &(range.max), &found));
        }
        /*
         * Sanity check
         */
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while moving in priority list for"
                         "handler %d\r\n No next or prev found for location %d\r\n", handler_id, range.max);
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.priorities_range.node_update(unit, core, iter, &range));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_mark_as_occupied(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location)
{
    uint32 key_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_occupy(unit, core, key_size, location));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Moves the bits in the bitmap for global/handler bitmap arrays from 'location_from' to 'location_to'
 *
 * \param [in] unit          - Device ID
 * \param [in] core          - Core ID
 * \param [in] handler_id    - The handler ID to which the move opreation belongs to
 * \param [in] location_from - The location from for the move operation
 * \param [in] location_to   - The location to for the move operation
 *
 * \return
 *  \retval _SHR_E_NONE - success
 *  \retval _SHR_E_INTERNAL - internal error due to unexpected data in given list
 */
static shr_error_e
dnx_field_tcam_location_algo_bmp_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_from,
    uint32 location_to)
{
    uint32 key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    /*
     * Global occupation bitmap move update
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_free(unit, core, key_size, location_from));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_occupy(unit, core, key_size, location_to));

    /*
     * Handler's entries occupation bitmap move update
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_free(unit, core, handler_id, key_size, location_from));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_occupy(unit, core, handler_id, key_size, location_to));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_from,
    uint32 location_to)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_bmp_move(unit, core, handler_id, location_from, location_to));

    /*
     * Handler's priority ranges list move-all update
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_prio_list_move(unit, core, handler_id, location_from, location_to));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_move_one(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_from,
    uint32 location_to,
    uint32 priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_bmp_move(unit, core, handler_id, location_from, location_to));

    /*
     * Handler's priority ranges list move one update
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_prio_list_move_one
                    (unit, core, handler_id, location_from, location_to, priority));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_handler_next_get_extended(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint8 backward,
    uint32 *next_location,
    uint8 *found)
{
    SHR_FUNC_INIT_VARS(unit);

    if (backward)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_prev_get
                        (unit, core, handler_id, location, next_location, found));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                        (unit, core, handler_id, location, next_location, found));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_handler_next_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint32 *next_location,
    uint8 *found)
{
    uint32 max_size;

    SHR_FUNC_INIT_VARS(unit);

    max_size = dnx_data_field.tcam.tcam_banks_size_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    entries_occupation_bitmap.get_next_in_range(unit, handler_id, core, location + 1, max_size, TRUE, 1,
                                                                next_location, found));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_handler_is_bank_handler_free(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_id,
    uint8 *found)
{
    uint32 next_location;
    uint32 bank_min_address;
    uint32 bank_max_address;
    SHR_FUNC_INIT_VARS(unit);
    bank_min_address = DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_id);
    bank_max_address = DNX_FIELD_TCAM_LOCATION_BANK_LAST(bank_id);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    entries_occupation_bitmap.get_next_in_range(unit, handler_id, core, bank_min_address,
                                                                bank_max_address, TRUE, 1, &next_location, found));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_handler_prev_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint32 *prev_location,
    uint8 *found)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    entries_occupation_bitmap.get_next_in_range(unit, handler_id, core, 0, location - 1, FALSE, 1,
                                                                prev_location, found));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_entry_comp_state(
    int unit,
    int core,
    uint32 location,
    uint8 *single_occupied,
    uint8 *double_occupied)
{
    uint8 d1_occupied;
    uint8 d2_occupied;
    uint32 comp_single_location;
    uint32 bank_id;
    uint32 comp_bank_id;
    uint32 comp_double_location1;
    uint32 comp_double_location2;

    SHR_FUNC_INIT_VARS(unit);
    comp_single_location = DNX_FIELD_TCAM_LOCATION_COMPLEMENTARY_GET(location);
    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location);
    comp_bank_id = bank_id + ((bank_id % 2) ? -1 : 1);
    comp_double_location1 = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(comp_bank_id,
                                                                     DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET
                                                                     (location));
    comp_double_location2 =
        DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(comp_bank_id,
                                                 DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(comp_single_location));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                    tcam_banks_occupation_bitmap.is_occupied(unit, core, comp_single_location, single_occupied));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                    tcam_banks_occupation_bitmap.is_occupied(unit, core, comp_double_location1, &d1_occupied));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                    tcam_banks_occupation_bitmap.is_occupied(unit, core, comp_double_location2, &d2_occupied));

    *double_occupied = d1_occupied | d2_occupied;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_empty_location_find(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location,
    dnx_field_tcam_location_find_mode_e find_mode,
    uint32 *empty_place,
    uint8 *found)
{
    dnx_field_tcam_location_range_t search_range;
    uint32 absolute_location;
    int *bank_ids_p = NULL;

    int bank_count;
    int bank_i;
    int search_start_i = -1;
    uint32 key_size;
    uint32 forward_empty_place = 0;
    uint32 backward_empty_place = 0;
    uint8 forward_found = FALSE;
    uint8 backward_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bank_ids_p, sizeof(*bank_ids_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get(unit, handler_id, core, bank_ids_p, &bank_count));

    absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);

    *found = FALSE;

    /*
     * Find the first bank larger or equal to the suggested location
     * bank. To set it as the start index for searching
     */
    for (bank_i = 0; bank_i < bank_count; bank_i++)
    {
        if (bank_ids_p[bank_i] >= location->bank_id)
        {
            search_start_i = bank_i;
            break;
        }
    }

    if (search_start_i == -1)
    {
        /*
         * When no bank is larger or equal to the location, we start from the end and only do backward search
         */
        search_start_i = bank_count - 1;
    }
    else if (find_mode & DNX_FIELD_TCAM_LOCATION_FIND_MODE_FORWARD)
    {

        /*
         * Search forward
         */
        for (bank_i = search_start_i; bank_i < bank_count; bank_i++)
        {
            if (key_size == dnx_data_field.tcam.key_size_double_get(unit) && bank_ids_p[bank_i] % 2)
            {
                /*
                 * Double sized key FGs can't start from odd bank numbers
                 */
                continue;
            }

            search_range.min = DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_ids_p[bank_i]);
            search_range.max = DNX_FIELD_TCAM_LOCATION_BANK_LAST(bank_ids_p[bank_i]);

            if (location->bank_id == bank_ids_p[bank_i])
            {
                search_range.min = absolute_location;
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_search_for_entry
                            (unit, core, handler_id, key_size, &search_range, TRUE, &forward_empty_place,
                             &forward_found));

            if (forward_found)
            {
                break;
            }
        }
    }

    if (find_mode & DNX_FIELD_TCAM_LOCATION_FIND_MODE_BACKWARD)
    {
        /*
         * Search backwards
         */
        for (bank_i = search_start_i; bank_i >= 0; bank_i--)
        {
            if (key_size == dnx_data_field.tcam.key_size_double_get(unit) && bank_ids_p[bank_i] % 2)
            {
                /*
                 * Double sized key FGs can't start from odd bank numbers
                 */
                continue;
            }

            search_range.min = DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_ids_p[bank_i]);
            search_range.max = DNX_FIELD_TCAM_LOCATION_BANK_LAST(bank_ids_p[bank_i]);

            if (location->bank_id == bank_ids_p[bank_i])
            {
                /**
                 * Forward search is absolute_location inclusive.
                 * Backward search is absolute_location exclusive because of bank upper limit suggestion.
                 */
                search_range.max = absolute_location - 1;
            }

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_search_for_entry
                            (unit, core, handler_id, key_size, &search_range, FALSE, &backward_empty_place,
                             &backward_found));

            if (backward_found)
            {
                break;
            }
        }
    }

    if (forward_found || backward_found)
    {
        uint32 backward_diff = absolute_location - backward_empty_place;
        uint32 forward_diff = forward_empty_place - absolute_location;
        *found = TRUE;
        /** Take min diff in case both directions found, otherwise take valid place */
        *empty_place = (!forward_found) ?
            backward_empty_place :
            (backward_found && backward_diff < forward_diff) ? backward_empty_place : forward_empty_place;
    }

exit:
    SHR_FREE(bank_ids_p);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_allocating_handler(
    int unit,
    int core,
    dnx_field_tcam_location_t * location,
    uint32 *handler_id)
{
    uint32 handler_index;
    uint32 absolute_location;
    uint8 is_occupied = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);
    for (handler_index = 0; handler_index < dnx_data_field.tcam.nof_tcam_handlers_get(unit); handler_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        entries_occupation_bitmap.is_occupied(unit, handler_index, core, absolute_location,
                                                              &is_occupied));

        if (is_occupied)
        {
            *handler_id = handler_index;
            break;
        }
    }

    if (!is_occupied)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Given location is not associated with any TCAM handler\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_free_line_get(
    int unit,
    int core,
    uint32 location,
    uint8 direction,
    uint32 *free_line)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Search in both directions, starting with the direction best suited for move opertaion
     */
    for (i = 0; i < 2; i++)
    {
        uint32 cur = DNX_FIELD_TCAM_LOCATION_LINE_GET(location);
        while (TRUE)
        {
            uint8 found;
            uint32 start;
            uint32 end;
            uint8 occupied;
            uint32 edge =
                direction ? DNX_FIELD_TCAM_LOCATION_BANK_LAST(DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location)) :
                DNX_FIELD_TCAM_LOCATION_BANK_FIRST(DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location));
            cur = direction ? cur + 1 : cur - 1;
            if (direction)
            {
                start = cur;
                end = edge;
            }
            else
            {
                start = edge;
                end = cur;
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_banks_occupation_bitmap.get_next_in_range(unit, core, start, end, direction, 0      /* find 
                                                                                                                                                 * free 
                                                                                                                                                 * space 
                                                                                                                                                 */ ,
                                                                                                      free_line,
                                                                                                      &found));
            if (!found)
            {
                /*
                 * No free entries found, quit search 
                 */
                break;
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                            tcam_banks_occupation_bitmap.is_occupied(unit, core, *free_line + 1, &occupied));
            /*
             * free_line should start from even entry and have 2 subsequent free 80b slots 
             */
            if (*free_line % 2 == 0 && !occupied)
            {
                /*
                 * Found it 
                 */
                SHR_EXIT();
            }
            /*
             * Not the one we're looking for, minimize search range 
             */
            cur = *free_line;
        }
        /*
         * Now try opposite search 
         */
        direction = !direction;
    }
    /*
     * We should not reach this point 
     */
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal Error While looking for a free line inside the bank: \n"
                 "No free line found! core = %d, bank = %d", core,
                 DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location));

exit:
    SHR_FUNC_EXIT;
}
