/** \file algo_field_action.c
 * $Id$
 *
 * Field procedures for DNX.
 *
 * Will hold the needed algorithm functions for Field module
 * related to the PMF action (including FES, FEM, entries).
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
/*
 * $Copyright:.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_field_action_access.h>

/*
 * }
 */

/*
 * typedefs
 * {
 */

/**
 * Structure containing the information regarding a FES quartet.
 * see dnx_algo_field_action_fes_allocate()
 */
typedef struct
{
    /*
     * The index of new FES quartet in dnx_algo_field_action_fes_alloc_in_t fes quartet.
     * If it is a movement and not a new FES quartet, the value is DNX_ALGO_FIELD_ACTION_MOVEMENT.
     */
    int place_in_alloc;
    /*
     * The action's priority
     */
    dnx_field_action_priority_t priority;
    /*
     * The field group ID for each FES quartet.
     */
    dnx_field_group_t fg_id;
    /*
     * The ordinal number of the FES quartets within field group.
     */
    unsigned int place_in_fg;
    /*
     * A list of possible allocation for masks (the old allocation if the fes quartet exists).
     */
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    /*
     * The action types used by each fes2msb instruction in the FES quartet..
     */
    dnx_field_action_type_t fes_action_type[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} dnx_algo_field_action_fes_alloc_fes_quartet_t;

/**
 * Structure containing the current state of the context, including SW state information
 * Also used to find the priority of FES quartets.
 * see dnx_algo_field_action_fes_allocate()
 */
typedef struct
{
    /*
     * The FES pgm ID in use by each FES ID.
     */
    dnx_field_fes_pgm_id_t fes_pgm_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    /*
     * Information about the FES quartet in each FES ID, if fes_pgm_id isn't the zero fes program ID.
     */
        dnx_algo_field_action_fes_alloc_fes_quartet_t
        fes_quartet_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    /*
     * If the FES quartet is shared.
     */
    uint8 is_shared[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_algo_field_action_fes_alloc_current_state_inclusive_t;

/**
 * Enum that represents the result of comparing two action priorities of two FES quartets.
 */
typedef enum
{
    /** First FES quartet has a higher precedence.*/
    DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS,
    /** Second FES quartet has a higher precedence.*/
    DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS,
    /** Both FES quartets have the same precedence. shouldn't occur as long as we use tie breakers.*/
    DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_TIE,
    /** At least one is a mandatory FES/FEM ID or a "don't care" priority.*/
    DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_INCOMPARABLE,
    /** The number of options in this enum.*/
    DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_NOF
} dnx_algo_field_action_fes_alloc_priority_compare_result_e;

/*
 * }
 */

/*
 * typedefs
 * {
 */

/**
 * MACRO to print stage name, while treating stage iPMF2 as a special case.
 */
#define DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(_unit,_dnx_stage) \
    (((_dnx_stage == DNX_FIELD_STAGE_IPMF1) || (_dnx_stage == DNX_FIELD_STAGE_IPMF2)) ? \
     ("IPMF1/2") : (dnx_field_stage_text(_unit, _dnx_stage)))

/*
 * }
 */

/*
 * Procedure prototypes
 * {
 */
/*See declaration below.*/
static shr_error_e dnx_algo_field_action_fes_allocate_non_mandatory_position(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    dnx_field_fes_id_t fes_id_currently_placed,
    int min_fes_id,
    int max_fes_id,
    int allow_contention_moving,
    int allow_non_contention_moving,
    int allow_fes_pgm_sharing,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    int *found_allocation_p);
/*
 * }
 */

/*
 * Procedures related to 'FEM machine'
 * {
 */

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_action_pmf_a_fem_pgm_id_allocate(
    int unit,
    dnx_field_fem_id_t fem_id,
    int alloc_flags,
    dnx_field_fem_program_t * fem_program_p)
{
    int alloc_id;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fem_program_p, _SHR_E_PARAM, "fem_program_p");
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    alloc_id = 0;
    rv = algo_field_info_sw.ipmf_a_fem_pgm_id.allocate_single(unit, fem_id, alloc_flags, NULL, &alloc_id);
    /** Do not print an error if we expect to perhaps run out of resources. */
    if (((alloc_flags & (DNX_ALGO_RES_ALLOCATE_SIMULATION)) != 0) && (rv == _SHR_E_RESOURCE))
    {
        SHR_IF_ERR_EXIT_NO_MSG(rv);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
        *fem_program_p = alloc_id;
        DNX_FIELD_FEM_PROGRAM_VERIFY(unit, *fem_program_p);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * see H file: algo_field.h
 */
shr_error_e
dnx_algo_field_action_pmf_a_fem_pgm_id_deallocate(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, fem_program);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_a_fem_pgm_id.free_single(unit, fem_id, fem_program, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_action_pmf_a_fem_map_index_allocate(
    int unit,
    dnx_field_fem_id_t fem_id,
    int alloc_flags,
    dnx_field_fem_map_index_t * fem_map_index_p)
{
    int alloc_id;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fem_map_index_p, _SHR_E_PARAM, "fem_map_index_p");
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    if ((alloc_flags & DNX_ALGO_RES_ALLOCATE_WITH_ID) != 0)
    {
        alloc_id = (*fem_map_index_p);
    }
    else
    {
        alloc_id = 0;
    }
    rv = algo_field_info_sw.ipmf_a_fem_map_index.allocate_single(unit, fem_id, alloc_flags, NULL, &alloc_id);
    /** Do not print an error if we expect to perhaps run out of resources. */
    if (((alloc_flags & (DNX_ALGO_RES_ALLOCATE_SIMULATION)) != 0) &&
        ((rv == _SHR_E_RESOURCE) || (rv == _SHR_E_FULL) || (rv == _SHR_E_EXISTS)))
    {
        *fem_map_index_p = alloc_id;
        SHR_IF_ERR_EXIT_NO_MSG(rv);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
        *fem_map_index_p = alloc_id;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see H file: algo_field.h
 */
shr_error_e
dnx_algo_field_action_pmf_a_fem_map_index_deallocate(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_map_index_t fem_map_index)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_MAP_INDEX_VERIFY(unit, fem_map_index);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_a_fem_map_index.free_single(unit, fem_id, fem_map_index, NULL));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see H file: algo_field.h
 */
shr_error_e
dnx_algo_field_action_find_available_fem_program(
    int unit,
    int allocate,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t * available_fem_program_p,
    uint8 *sum_allocated_actions_p)
{
    int alloc_flags;
    uint8 sw_ignore_actions;
    dnx_field_group_t sw_fg_id;
    dnx_field_fem_program_t fem_program_index, fem_program_max;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    if (allocate)
    {
        alloc_flags = 0;
    }
    else
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_SIMULATION;
    }
    /*
     * Get a free 'fem program' for this 'fem id'
     */
    rv = dnx_algo_field_action_pmf_a_fem_pgm_id_allocate(unit, fem_id, alloc_flags, available_fem_program_p);
    if ((allocate == FALSE) && (rv == _SHR_E_RESOURCE))
    {
        SHR_IF_ERR_EXIT_NO_MSG(rv);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    *sum_allocated_actions_p = 0;
    for (fem_program_index = FIRST_AVAILABLE_FEM_PROGRAM; fem_program_index < fem_program_max; fem_program_index++)
    {
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id, fem_program_index, &sw_fg_id));
        if (sw_fg_id != DNX_FIELD_GROUP_INVALID)
        {
            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.
                            ignore_actions.get(unit, fem_id, fem_program_index, &sw_ignore_actions));
            /*
             * Set all bits in 'sum_allocated_actions' which are marked as 'busy' (= NOT ignored) for this pair
             * ('fem_id','fem_program') in swstate.
             */
            (*sum_allocated_actions_p) |= (~(sw_ignore_actions));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Search input 'source condition's array and copy into 'destination condition's array
 *   only entries which are marked with specified 'fem_map_index'. While copying replace
 *   specified 'fem_map_index' by a new value, 'fem_map_index_new'.
 *   This operation is required since input from user only indicates required
 *   'action's but the actual selection of the 'action's resource is done within
 *   the code. 'Action's are, then, treated as a controlled resource.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_condition_entry_source -
 *   Pointer to array of structures of type 'dnx_field_fem_condition_entry_t' containing
 *   info corresponding to one 'condition'.
 *   See DBAL table: FIELD_PMF_A_FEM_MAP_INDEX
 *   Those elements, on this array, which have a 'fem_map_index' member, whose value is
 *   'fem_map_index_to_replace' are copied to 'fem_condition_entry_destination' but the
 *   value is replaced to 'fem_map_index_new'.
 * \param [in] fem_condition_entry_destination -
 *   See 'fem_condition_entry_source' above.
 * \param [in] fem_map_index_to_replace -
 *   This procedure searches 'fem_condition_entry' and replaces all 'fem_map_index'
 *   elements, whose value is 'fem_map_index_to_replace', by a new value: 'fem_map_index_new'
 * \param [in] fem_map_index_new -
 *   See 'fem_map_index_to_replace' above.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Copy operation is done regardless of the value of the 'fem_action_valid' element
 * \see
 *   * dnx_field_fem_action_add()
 */
static shr_error_e
dnx_field_fem_copy_replace_map_indices(
    int unit,
    dnx_field_fem_condition_entry_t fem_condition_entry_source[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_fem_condition_entry_t
    fem_condition_entry_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_fem_map_index_t fem_map_index_to_replace,
    dnx_field_fem_map_index_t fem_map_index_new)
{
    dnx_field_fem_condition_t fem_condition_index, fem_condition_max;

    SHR_FUNC_INIT_VARS(unit);
    fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
    for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
    {
        if (fem_condition_entry_source[fem_condition_index].fem_map_index == fem_map_index_to_replace)
        {
            fem_condition_entry_destination[fem_condition_index] = fem_condition_entry_source[fem_condition_index];
            fem_condition_entry_destination[fem_condition_index].fem_map_index = fem_map_index_new;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Indicate whether an 'action', which is stored in template manager's memory
 *   (and consequently, in HW) for specified 'fem_id', is exactly the same as
 *   marked on input and specify its index (in the array of 4 actions
 *   available per each FEM).
 *   If such an 'action' is not found, then an attempt is made to write it
 *   into the template manager's memory.
 *   If this is successful then an indication on the index ('profile') of newly allocated
 *   'action' is returned, together with an indication that this is a new 'profile'.
 *   Otherwise, a 'not found' error indication is returned 
 *   
 *   Note that 'action' includes an array of 'bit descriptors (either 24 bits
 *   of 4 bits, depending on 'fem_id) plus an 'adder' (relevant for 'fem_id'
 *   2-23 only).
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] silence_errors -
 *   If TRUE silence printed errors.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] dnx_encoded_fem_action -
 *   Encoded value of action corresponding to one 'action type'
 *   on 'fem_action_entry_p[]' below. Used for verification.
 * \param [in] fem_action_entry_p -
 *    Pointer to structure containing all info corresponding to one 'action' (= all 4/24
 *    bit descriptors). See, for example, DBAL tables: FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_*_MAP
 *    Note that, for 'FEM id's 0/1, only 4 bits may be specified. In that case, the per-bit
 *    element 'fem_bit_format' on 'dnx_field_fem_action_entry_t' should be marked
 *    DNX_FIELD_FEM_BIT_FORMAT_INVALID for bits 4 to 23. Also, 'fem_adder' should be 0.
 *    Otherwise, *fem_map_index_p is loaded by
 *    DNX_FIELD_FEM_MAP_INDEX_INVALID.
 * \param [in] sum_allocated_actions -
 *    An 8-bits bitmap which indicates which 'action's, in HW, should be compared
 *    with input 'action' (fem_action_entry_p). Each bit represents the corresponding
 *    index so if, for example, BIT(1) is set (i.e., sum_allocated_actions=0x02) then
 *    'fem_map_index' of value '2' is included.
 * \param [in] sum_allocated_actions_this_program_p -
 *    An 8-bits bitmap which indicates which 'action's were allocated for this action.
 * \param [out] fem_map_index_p -
 *   This procedure loads pointed memory by the 'fem_map_index' which corresponds
 *   to selected 'action' index (either matched or not), in template manager's memory
 *   (and in HW), on specified 'fem_id'.
 *   If no match is found on 'fem_id', a value of DNX_FIELD_FEM_MAP_INDEX_INVALID is loaded.
 * \param [out] already_in_hw_p -
 *   This procedure loads pointed memory by a non-zero value if 'action', specified by
 *   'fem_action_entry_p' has been found to already be in in template manager's memory
 *   (and in HW), on specified 'fem_id'.
 *   Otherwise, zero is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_field_fem_action_add()
 */
static shr_error_e
dnx_field_fem_is_hw_action_the_same(
    int unit,
    int silence_errors,
    dnx_field_fem_id_t fem_id,
    dnx_field_action_t dnx_encoded_fem_action,
    dnx_field_fem_action_entry_t * fem_action_entry_p,
    uint8 sum_allocated_actions,
    uint8 *sum_allocated_actions_this_program_p,
    dnx_field_fem_map_index_t * fem_map_index_p,
    unsigned int *already_in_hw_p)
{
    int alloc_flags;
    dnx_field_fem_map_index_t fem_map_index_chosen;
    int is_existing_map_index_found;
    shr_error_e shr_error;
    dnx_field_fem_map_index_t fem_map_index;
    dnx_field_fem_map_index_t fem_map_index_max;
    dnx_field_fem_action_info_t fem_action_entry_swstate;
    uint8 extractions_equivalent;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Initialize output to indicate 'not space available'.
     */
    *fem_map_index_p = DNX_FIELD_FEM_MAP_INDEX_INVALID;
    /*
     * Initialize output to indicate 'not found in hw'.
     */
    *already_in_hw_p = 0;
    /*
     * Make sure input 'action' is legal. That includes verifying that FEMs 0/1 only have
     * (a) 4 bits marked as 'valid' (b) 'fem_adder' marked as 'invalid'.
     */
    SHR_IF_ERR_EXIT(dnx_field_fem_num_action_bits_verify(unit, fem_id, dnx_encoded_fem_action, fem_action_entry_p));
    fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;

    /*
     * See if there is already such an action/extraction
     */
    is_existing_map_index_found = FALSE;
    fem_action_entry_swstate.fem_id = fem_id;
    for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
    {
        if (((*sum_allocated_actions_this_program_p) & SAL_BIT(fem_map_index)) != 0)
        {
            /** Do not resuse extractions allocated for this program.*/
            continue;
        }
        fem_action_entry_swstate.fem_map_index = fem_map_index;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_action_info_hw_get(unit, &fem_action_entry_swstate));
        SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_compare
                        (unit, &(fem_action_entry_swstate.fem_action_entry), fem_action_entry_p,
                         &extractions_equivalent));
        if (extractions_equivalent)
        {
            dnx_field_fem_map_index_t fem_map_index_to_check;
            /** Check if the extraction is allocated or if it's just left in the HW.*/
            fem_map_index_to_check = fem_map_index;
            shr_error =
                dnx_algo_field_action_pmf_a_fem_map_index_allocate(unit, fem_id,
                                                                   DNX_ALGO_RES_ALLOCATE_WITH_ID |
                                                                   DNX_ALGO_RES_ALLOCATE_SIMULATION,
                                                                   &fem_map_index_to_check);
            if ((shr_error == _SHR_E_RESOURCE) || (shr_error == _SHR_E_FULL) || (shr_error == _SHR_E_EXISTS))
            {
                is_existing_map_index_found = TRUE;
                fem_map_index_chosen = fem_map_index;
                break;
            }
            else
            {
                SHR_IF_ERR_EXIT(shr_error);
            }
        }
    }
    if (is_existing_map_index_found == FALSE)
    {
        /*
         * Allocate a new action/extraction
         */
        if (silence_errors)
        {
            alloc_flags = DNX_ALGO_RES_ALLOCATE_SIMULATION;
        }
        else
        {
            alloc_flags = 0;
        }
        shr_error =
            dnx_algo_field_action_pmf_a_fem_map_index_allocate(unit, fem_id, alloc_flags, &fem_map_index_chosen);
        if ((shr_error == _SHR_E_RESOURCE) || (shr_error == _SHR_E_FULL) || (shr_error == _SHR_E_EXISTS))
        {
            /*
             * No matching 'profile data' has been found and, also, no free space has been found.
             * *fem_map_index_p is already loaded by DNX_FIELD_FEM_MAP_INDEX_INVALID
             */
            if ((sum_allocated_actions & SAL_UPTO_BIT(fem_map_index_max)) != SAL_UPTO_BIT(fem_map_index_max))
            {
                if (silence_errors == FALSE)
                {
                    SHR_ERR_EXIT(shr_error,
                                 "For FEM id %d: No free extraction found. Extraction bitmap 0x%08lX.\r\n",
                                 fem_id, (unsigned long) sum_allocated_actions);
                }
            }
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(shr_error);
        if (silence_errors)
        {
            /** Allocation possible, allocate for real this time.*/
            alloc_flags &= ~(DNX_ALGO_RES_ALLOCATE_SIMULATION);
            SHR_IF_ERR_EXIT(dnx_algo_field_action_pmf_a_fem_map_index_allocate
                            (unit, fem_id, alloc_flags, &fem_map_index_chosen));
        }
    }

    /*
     * At this point, a place has been found for specified data.
     * It is either an existing 'action' of a newly added entry.
     */
    *fem_map_index_p = fem_map_index_chosen;
    (*sum_allocated_actions_this_program_p) |= SAL_BIT(*fem_map_index_p);
    if (is_existing_map_index_found == FALSE)
    {
        /*
         * This is a newly added entry (action/extraction).
         * Note that '*already_in_hw_p' is set to '0' by default.
         * Verify synchronization with local bitmap.
         */
        if ((sum_allocated_actions & SAL_BIT(*fem_map_index_p)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For FEM id %d: Extraction %d seen as 'not occupied' but local bitmap (0x%08lX) indicates 'occupied'. Quit.\r\n",
                         fem_id, fem_map_index_chosen, (unsigned long) sum_allocated_actions);
        }
    }
    else
    {
        /*
         * This is an 'already existing' entry (action/extraction).
         */
        if ((sum_allocated_actions & SAL_BIT(*fem_map_index_p)) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For FEM id %d: Extraction %d seen as 'occupied' but local bitmap (0x%08lX) indicates 'not occupied'. Quit.\r\n",
                         fem_id, fem_map_index_chosen, (unsigned long) sum_allocated_actions);
        }
        *already_in_hw_p = 1;
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * see H file: algo_field.h
 */
shr_error_e
dnx_algo_field_action_update_conditions(
    int unit,
    int allocate,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    uint8 sum_allocated_actions,
    dnx_field_fem_condition_entry_t
    fem_condition_entry_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    uint8 *ignore_actions_p,
    uint8 *already_in_hw_p,
    dnx_field_action_t fem_encoded_actions_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_action_entry_t fem_action_entry_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX])
{
    /*
     * Inspect input 'condition's array (fem_condition_entry[]):
     * Check whether any of the specified actions (See fem_action_entry[]) is the same as 'action's
     * currently in HW.
     * If so, update conditions' array (fem_condition_entry_destination) accordingly and remove from 'ignore_actions'
     */
    dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;
    dnx_field_fem_map_index_t fem_map_index_matched;
    uint8 ignore_actions_image;
    uint8 sum_allocated_actions_destination;
    uint8 sum_allocated_actions_this_program = 0;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Use 'sum_allocated_actions_destination' bitmap to store all 'actions': Both those which are
     * already in HW and those which are newly added below and which will be added to HW at the
     * end of this procedure.
     */
    sum_allocated_actions_destination = sum_allocated_actions;
    /*
     * 'fem_condition_entry_destination' is the array of conditions after having been modified
     * by replacing indices into 'action's by real HW values.
     */
    SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_array_init(unit, fem_condition_entry_destination));
    /*
     * 'already_in_hw' is a bitmap of the 'action's which are required by the caller and
     * which are already written into HW.
     */
    *already_in_hw_p = 0;
    /*
     * 'ignore_actions_image' is a bitmap of the 'action's after having been modified to
     * reflect real HW status.
     */
    ignore_actions_image = DNX_FIELD_IGNORE_ALL_ACTIONS;
    fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
    {
        unsigned int already_in_hw_flag;
        if ((*ignore_actions_p & SAL_BIT(fem_map_index)) == 0)
        {
            /*
             * Enter if 'action' with this index is meaningful on input array 'fem_action_entry'
             */
            SHR_IF_ERR_EXIT(dnx_field_fem_is_hw_action_the_same(unit, !allocate, fem_id,
                                                                fem_encoded_actions[fem_map_index],
                                                                &(fem_action_entry[fem_map_index]),
                                                                sum_allocated_actions |
                                                                sum_allocated_actions_destination,
                                                                &sum_allocated_actions_this_program,
                                                                &fem_map_index_matched, &already_in_hw_flag));
            if (fem_map_index_matched != DNX_FIELD_FEM_MAP_INDEX_INVALID)
            {
                if (already_in_hw_flag != 0)
                {
                    /*
                     * Enter if an 'action' was found, in HW, which matches, exactly, input 'action'
                     * and which is already used by one of the other Field Groups on this FEM.
                     */
                    /*
                     * Mark the 'action's which are already in HW so we can optimize and not
                     * write to HW again the very same data.
                     */
                    *already_in_hw_p |= SAL_BIT(fem_map_index_matched);
                    /*
                     * Update image of 'ignore_actions' (and not 'ignore_actions' itself) since it is
                     * being used within this loop.
                     */
                    ignore_actions_image &= SAL_RBIT(fem_map_index_matched);
                    /*
                     * Update 'destination condition's array with new 'fem_map_index'
                     */
                    SHR_IF_ERR_EXIT(dnx_field_fem_copy_replace_map_indices
                                    (unit, fem_condition_entry, fem_condition_entry_destination, fem_map_index,
                                     fem_map_index_matched));
                    /*
                     * Update 'destination action's array with new 'fem_map_index'
                     */
                    fem_encoded_actions_destination[fem_map_index_matched] = fem_encoded_actions[fem_map_index];
                    /*
                     * Since 'action's are already in HW, there is no need to update 'fem_action_entry_destination' 
                     * Note that this map index will not be rewritten into HW because of the 'already_in_hw' flags.
                     */
                }
                else
                {
                    /*
                     * Enter if 'action' was NOT found in HW. There was a free space and it was
                     * updated in 'template manager's memory.
                     */
                    sum_allocated_actions_destination |= SAL_BIT(fem_map_index_matched);
                    /*
                     * Update image of 'ignore_actions' (and not 'ignore_actions' itself) since it is
                     * being used within this loop.
                     */
                    ignore_actions_image &= SAL_RBIT(fem_map_index_matched);
                    /*
                     * Update 'destination condition's array with new 'fem_map_index'
                     */
                    SHR_IF_ERR_EXIT(dnx_field_fem_copy_replace_map_indices
                                    (unit, fem_condition_entry, fem_condition_entry_destination,
                                     fem_map_index, fem_map_index_matched));
                    /*
                     * Update 'destination action's array with new 'fem_map_index'
                     */
                    fem_encoded_actions_destination[fem_map_index_matched] = fem_encoded_actions[fem_map_index];
                    /*
                     * Update 'fem_action_entry_destination' so the newly updated map index points
                     * to the original data. These arrays will be used, below, to update HW.
                     */
                    sal_memcpy(&fem_action_entry_destination[fem_map_index_matched],
                               &fem_action_entry[fem_map_index], sizeof(fem_action_entry_destination[0]));

                }
            }
            else
            {
                if (allocate)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "No free 'action' space was found in fem_id %d. "
                                 "sum_allocated_actions (bitmap of allocated actions) 0x%08lX Quit.\r\n",
                                 fem_id, (unsigned long) sum_allocated_actions);
                }
                else
                {
                    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_RESOURCE);
                }
            }
        }
    }
    if (allocate == FALSE)
    {
        /*
         * Free extractions that were allocated.
         * This is done becasue we are only checking possibility of allocation, and don't want to allocate profiles.
         */
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            if (((sum_allocated_actions_destination & SAL_BIT(fem_map_index)) != 0) &&
                ((sum_allocated_actions & SAL_BIT(fem_map_index)) == 0))
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_pmf_a_fem_map_index_deallocate(unit, fem_id, fem_map_index));
            }
        }
    }
    /*
     * At this point, 'fem_condition_entry_destination' contains the updated 'condition's table
     * and 'ignore_actions_image' contains the updated 'ignore_actions' value.
     * 'already_in_hw' contains the actions which are already in the HW and do not necessarily
     * need to be written again.
     */
    *ignore_actions_p = ignore_actions_image;
exit:
    SHR_FUNC_EXIT;
}

/*
 * see H file: algo_field.h
 */
shr_error_e
dnx_algo_field_fem_priority_is_position_set(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    uint8 is_position)
{
    SHR_FUNC_INIT_VARS(unit);

    if (fem_id >= dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEM ID %d must not exceed %d.\r\n",
                     fem_id, dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id - 1);
    }
    if (fem_program >= dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEM program %d must not exceed %d.\r\n",
                     fem_program,
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs - 1);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.fem.is_priority_position.set(unit, fem_id, fem_program, is_position));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see H file: algo_field.h
 */
shr_error_e
dnx_algo_field_fem_priority_is_position_get(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    uint8 *is_position_p)
{
    SHR_FUNC_INIT_VARS(unit);

    if (fem_id >= dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEM ID %d must not exceed %d.\r\n",
                     fem_id, dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id - 1);
    }
    if (fem_program >= dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEM program %d must not exceed %d.\r\n",
                     fem_program,
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs - 1);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.fem.is_priority_position.get(unit, fem_id, fem_program, is_position_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see H file: algo_field.h
 */
shr_error_e
dnx_algo_field_fem_priority_verify_contention_inside_group(
    int unit,
    dnx_field_group_t fg_id,
    uint8 ignore_actions_initial,
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    uint8 priority_is_position)
{
    dnx_field_fem_id_t fem_id_index;
    dnx_field_fem_id_t fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    dnx_field_fem_program_t fem_program;
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit_swstate;
    dnx_field_fem_condition_entry_t fem_condition_entry_swstate[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION];
    dnx_field_fem_action_entry_t fem_action_entry_swstate[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];
    dnx_field_fem_map_index_t fem_map_index_max =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    dnx_field_fem_map_index_t fem_map_index_sw_state;
    dnx_field_fem_map_index_t fem_map_index_new;
    dnx_field_action_type_t action_type_invalid;
    uint8 priority_is_position_swstate;
    uint8 ignore_actions_sw_state;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, DNX_FIELD_STAGE_IPMF1, &action_type_invalid));

    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_owner(unit, fem_id_index, fg_id, &fem_program));
        if (fem_program == DNX_FIELD_FEM_PROGRAM_INVALID)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_get(unit, fem_id_index, fem_program,
                                                  &fem_condition_ms_bit_swstate, fem_condition_entry_swstate,
                                                  fem_action_entry_swstate));
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.
                        ignore_actions.get(unit, fem_id_index, fem_program, &ignore_actions_sw_state));

        for (fem_map_index_new = 0; fem_map_index_new < fem_map_index_max; fem_map_index_new++)
        {
            if ((ignore_actions_initial & SAL_BIT(fem_map_index_new)) == 0)
            {
                if (fem_action_entry[fem_map_index_new].fem_action != action_type_invalid)
                {
                    for (fem_map_index_sw_state = 0; fem_map_index_sw_state < fem_map_index_max;
                         fem_map_index_sw_state++)
                    {
                        if ((ignore_actions_sw_state & SAL_BIT(fem_map_index_sw_state)) == 0)
                        {
                            if (fem_action_entry[fem_map_index_new].fem_action ==
                                fem_action_entry_swstate[fem_map_index_new].fem_action)
                            {
                                SHR_IF_ERR_EXIT(dnx_algo_field_fem_priority_is_position_get
                                                (unit, fem_id_index, fem_program, &priority_is_position_swstate));
                                if (priority_is_position_swstate && (priority_is_position == FALSE))
                                {
                                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between new FEM action "
                                                 "(\"don't care\" priority) and existing FEM action (FEM ID %d) "
                                                 "with mandatory position priority within fg_id %d.\r\n",
                                                 fem_id_index, fg_id);
                                }
                                else if ((priority_is_position_swstate == FALSE) && priority_is_position)
                                {
                                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between new FEM action "
                                                 "(mandatory position priority) and existing FEM action (FEM ID %d) "
                                                 "with \"don't care\" priority within fg_id %d.\r\n",
                                                 fem_id_index, fg_id);
                                }
                                else if ((priority_is_position_swstate == FALSE) && (priority_is_position == FALSE))
                                {
                                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between new FEM action "
                                                 "(\"don't care\" priority) and existing FEM action (FEM ID %d) "
                                                 "with \"don't care\" priority within fg_id %d.\r\n",
                                                 fem_id_index, fg_id);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see H file: algo_field.h
 */
shr_error_e
dnx_algo_field_fem_priority_verify_contention_inside_context(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id)
{
    dnx_field_fem_id_t fem_id_index;
    dnx_field_fem_id_t fem_id_index_2;
    dnx_field_fem_id_t fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    dnx_field_fem_program_t fem_program;
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit_swstate;
    dnx_field_fem_condition_entry_t fem_condition_entry_swstate[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION];
    dnx_field_fem_action_entry_t fem_action_entry_swstate[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];
    dnx_field_action_type_t
        fem_actions_context[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID]
        [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];
    dnx_field_action_type_t
        efes_actions_context[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
        [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    dnx_field_fem_map_index_t fem_map_index_max =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    dnx_field_fem_map_index_t fem_map_index;
    dnx_field_fem_map_index_t fem_map_index_2;
    dnx_field_action_type_t action_type_invalid;
    uint8 priority_is_position_swstate;
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;
    dnx_field_group_t sw_fg_id;
    dnx_field_fes_id_t fes_id_ndx;
    dnx_field_fes_id_t fes_id_max =
        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
    dnx_field_fes_pgm_id_t fes_pgm_id;
    dnx_field_fes_key_select_t key_select;
    unsigned int fes2msb_ndx;
    unsigned int fes2msb_max = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
    dnx_field_actions_fes_common_info_fes2msb_t fes_inst_common_info_fes2msb;
    dnx_field_fes_chosen_mask_t chosen_mask;
    int fem_is_dont_care[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID];
    int efes_is_dont_care[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_action_priority_t efes_priority[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_action_priority_t stored_priority;
    dnx_field_group_t fem_fg_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID];
    dnx_field_group_t efes_fg_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];

    SHR_FUNC_INIT_VARS(unit);

    /** Sanity check.*/
    if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_IPMF2))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage %d (\"%s\") not supported for FEM.\n",
                     field_stage, dnx_field_stage_text(unit, field_stage));
    }

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, DNX_FIELD_STAGE_IPMF1, &action_type_invalid));

    /*
     * Init FEM action array.
     */
    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            fem_actions_context[fem_id_index][fem_map_index] = action_type_invalid;
        }
    }

    /*
     * Init EFES action array.
     */
    for (fes_id_ndx = 0; fes_id_ndx < fes_id_max; fes_id_ndx++)
    {
        for (fes2msb_ndx = 0; fes2msb_ndx < fes2msb_max; fes2msb_ndx++)
        {
            efes_actions_context[fes_id_ndx][fes2msb_ndx] = action_type_invalid;
        }
    }

    /*
     * Collect FEM actions.
     */
    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        uint8 ignore_actions;
        dnx_field_pmf_fem_program_entry.fem_id = fem_id_index;
        dnx_field_pmf_fem_program_entry.context_id = context_id;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry));
        if (dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select == DNX_FIELD_INVALID_FEM_KEY_SELECT)
        {
            continue;
        }
        fem_program = dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_program;
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id_index, fem_program, &sw_fg_id));
        /** Sanity check. */
        if (sw_fg_id == DNX_FIELD_GROUP_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context %d uses FEM ID %d FEM program %d but no field group found there.\n",
                         context_id, fem_id_index, fem_program);
        }
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_get(unit, fem_id_index, fem_program,
                                                  &fem_condition_ms_bit_swstate, fem_condition_entry_swstate,
                                                  fem_action_entry_swstate));
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.ignore_actions.get(unit, fem_id_index, fem_program, &ignore_actions));
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            if ((ignore_actions & SAL_BIT(fem_map_index)) == 0)
            {
                fem_actions_context[fem_id_index][fem_map_index] = fem_action_entry_swstate[fem_map_index].fem_action;
            }
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_fem_priority_is_position_get
                        (unit, fem_id_index, fem_program, &priority_is_position_swstate));
        fem_is_dont_care[fem_id_index] = !(priority_is_position_swstate);
        fem_fg_id[fem_id_index] = sw_fg_id;
    }

    /*
     * Collect EFES actions.
     */
    for (fes_id_ndx = 0; fes_id_ndx < fes_id_max; fes_id_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_fes_program_fes_instruction_hw_get
                        (unit, field_stage, context_id, fes_id_ndx, &fes_pgm_id, &key_select));
        if (fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            for (fes2msb_ndx = 0; fes2msb_ndx < fes2msb_max; fes2msb_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_2nd_instruction_hw_get
                                (unit, field_stage, fes_id_ndx, fes_pgm_id, fes2msb_ndx,
                                 &fes_inst_common_info_fes2msb, &chosen_mask));
                efes_actions_context[fes_id_ndx][fes2msb_ndx] = fes_inst_common_info_fes2msb.action_type;
            }
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                            priority.get(unit, context_id, fes_id_ndx, &stored_priority));
            if (stored_priority == DNX_FIELD_ACTION_PRIORITY_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid priority for context %d fes_id %d.\n", context_id, fes_id_ndx);
            }
            if (stored_priority == BCM_FIELD_ACTION_DONT_CARE)
            {
                efes_is_dont_care[fes_id_ndx] = TRUE;
            }
            else
            {
                efes_is_dont_care[fes_id_ndx] = FALSE;
            }
            efes_priority[fes_id_ndx] = stored_priority;
            /** Collect the fg_id just for error logging purposes.*/
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                            field_group.get(unit, fes_id_ndx, fes_pgm_id, &sw_fg_id));
            /** Sanity check. */
            if (sw_fg_id == DNX_FIELD_GROUP_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Context %d uses EFES_ID %d EFES program ID %d but no field group found there.\n",
                             context_id, fes_id_ndx, fes_pgm_id);
            }
            efes_fg_id[fes_id_ndx] = sw_fg_id;
        }
    }

    /*
     * Check FEM-FEM contention
     */
    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        for (fem_id_index_2 = 0; fem_id_index_2 < fem_id_index; fem_id_index_2++)
        {
            for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
            {
                for (fem_map_index_2 = 0; fem_map_index_2 < fem_map_index_max; fem_map_index_2++)
                {
                    if (fem_actions_context[fem_id_index][fem_map_index] ==
                        fem_actions_context[fem_id_index_2][fem_map_index_2])
                    {
                        if (fem_actions_context[fem_id_index][fem_map_index] != action_type_invalid)
                        {
                            if (fem_is_dont_care[fem_id_index] && fem_is_dont_care[fem_id_index_2])
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between FEM action "
                                             "(FEM ID %d, fg_id %d) with "
                                             "\"don't care\" priority and FEM action (FEM ID %d, fg_id %d) "
                                             "with \"don't care\" priority. Context %d.\r\n",
                                             fem_id_index, fem_fg_id[fem_id_index],
                                             fem_id_index_2, fem_fg_id[fem_id_index_2], context_id);
                            }
                            else if (fem_is_dont_care[fem_id_index] && (fem_is_dont_care[fem_id_index_2] == FALSE))
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between FEM action "
                                             "(FEM ID %d, fg_id %d) with "
                                             "\"don't care\" priority and FEM action (FEM ID %d, fg_id %d) "
                                             "with mandatory position priority. Context %d.\r\n",
                                             fem_id_index, fem_fg_id[fem_id_index],
                                             fem_id_index_2, fem_fg_id[fem_id_index_2], context_id);
                            }
                            else if ((fem_is_dont_care[fem_id_index] == FALSE) && fem_is_dont_care[fem_id_index_2])
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between FEM action "
                                             "(FEM ID %d, fg_id %d) with "
                                             "mandatory position priority and FEM action (FEM ID %d, fg_id %d) "
                                             "with \"don't care\" priority. Context %d.\r\n",
                                             fem_id_index, fem_fg_id[fem_id_index],
                                             fem_id_index_2, fem_fg_id[fem_id_index_2], context_id);
                            }
                        }
                    }
                }
            }
        }
    }

    /*
     * Check FEM-EFES contention
     */
    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        for (fes_id_ndx = 0; fes_id_ndx < fes_id_max; fes_id_ndx++)
        {
            for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
            {
                for (fes2msb_ndx = 0; fes2msb_ndx < fes2msb_max; fes2msb_ndx++)
                {
                    if (fem_actions_context[fem_id_index][fem_map_index] ==
                        efes_actions_context[fes_id_ndx][fes2msb_ndx])
                    {
                        if (fem_actions_context[fem_id_index][fem_map_index] != action_type_invalid)
                        {
                            if (fem_is_dont_care[fem_id_index] && efes_is_dont_care[fes_id_ndx])
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between FEM action "
                                             "(FEM ID %d, fg_id %d) with "
                                             "\"don't care\" priority and EFES action (EFES ID %d, fg_id %d) "
                                             "with \"don't care\" priority. Context %d.\r\n",
                                             fem_id_index, fem_fg_id[fem_id_index],
                                             fes_id_ndx, efes_fg_id[fes_id_ndx], context_id);
                            }
                            else if (fem_is_dont_care[fem_id_index] && (efes_is_dont_care[fes_id_ndx] == FALSE))
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between FEM action "
                                             "(FEM ID %d, fg_id %d) with "
                                             "\"don't care\" priority and EFES action (EFES ID %d, fg_id %d) "
                                             "with 0x%x priority. Context %d.\r\n",
                                             fem_id_index, fem_fg_id[fem_id_index], fes_id_ndx, efes_fg_id[fes_id_ndx],
                                             efes_priority[fes_id_ndx], context_id);
                            }
                            else if ((fem_is_dont_care[fem_id_index] == FALSE) && efes_is_dont_care[fes_id_ndx])
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between FEM action "
                                             "(FEM ID %d, fg_id %d) with "
                                             "mandatory position priority and EFES action (EFES ID %d, fg_id %d) "
                                             "with \"don't care\" priority. Context %d.\r\n",
                                             fem_id_index, fem_fg_id[fem_id_index],
                                             fes_id_ndx, efes_fg_id[fes_id_ndx], context_id);
                            }
                        }
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see H file: algo_field.h
 */
shr_error_e
dnx_algo_field_fem_position_allocate(
    int unit,
    uint8 ignore_actions_initial,
    dnx_field_group_t fg_id,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_id_t * fem_id_p)
{
    int fem_id_can_be_used;
    int allocation_found = FALSE;
    dnx_field_fem_id_t fem_id_index;
    dnx_field_fem_id_t fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    SHR_FUNC_INIT_VARS(unit);

    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_action_fem_is_fem_id_available(unit, fem_id_index, fg_id, second_fg_id,
                                                                 fem_condition_entry, fem_encoded_actions,
                                                                 fem_action_entry, ignore_actions_initial,
                                                                 &fem_id_can_be_used));
        if (fem_id_can_be_used)
        {
            allocation_found = TRUE;
            break;
        }
    }

    if (allocation_found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "No FEM ID found for allocation.\r\n");
    }

    (*fem_id_p) = fem_id_index;

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/**
 * \brief
 *  write a FES quartet's priority in regards to a specific context ID to the  software state of a certain PMF stage.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] context_id -
 *  Cotnext ID.
 * \param [in] fes_id -
 *  The FES ID, the EFES to which we write.
 * \param [in] priority -
 *  The priority of the FES quartet.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 */
static shr_error_e
dnx_algo_field_action_fes_update_sw_state_priority(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    unsigned int fes_id,
    dnx_field_action_priority_t priority)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Write to SW state
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                            priority.set(unit, context_id, fes_id, priority));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                            priority.set(unit, context_id, fes_id, priority));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                            priority.set(unit, context_id, fes_id, priority));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "In stage \"%s\" FES ID %d context ID %d priority was set to 0x%08X.\r\n.",
                 DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), fes_id, context_id, priority);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  write to the fes_state software state of a certain PMF stage.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] fg_id -
 *  The field group id.
 * \param [in] fes_id -
 *  The FES ID, the EFES to which we write.
 * \param [in] fes_pgm_id -
 *  The FES program ID to which we write.
 * \param [in] action_number -
 *  The ordinal number of the FES quartet within the field group.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 */
static shr_error_e
dnx_algo_field_action_fes_update_sw_state_fes_state(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    unsigned int fes_id,
    unsigned int fes_pgm_id,
    unsigned int action_number)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Write to SW state
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.field_group.set(unit, fes_id, fes_pgm_id, fg_id));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                            place_in_fg.set(unit, fes_id, fes_pgm_id, action_number));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.field_group.set(unit, fes_id, fes_pgm_id, fg_id));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                            place_in_fg.set(unit, fes_id, fes_pgm_id, action_number));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.field_group.set(unit, fes_id, fes_pgm_id, fg_id));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                            place_in_fg.set(unit, fes_id, fes_pgm_id, action_number));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "In stage \"%s\" FES ID %d FES program ID %d was set to:%s\r\n.",
                 DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), fes_id, fes_pgm_id, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "==> field group ID %d place in field group %d.%s%s\r\n.",
                 fg_id, action_number, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  write to one bit of the action mask bitmap software state of a certain PMF stage.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] fes_id -
 *  The FES ID, the EFES to which we write.
 * \param [in] mask_id -
 *  The mask ID. of the FES to allocate/deallocate
 * \param [in] set -
 *  If true, writes 1 (allocates), if false writes zero.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 */
static shr_error_e
dnx_algo_field_action_fes_update_sw_state_mask(
    int unit,
    dnx_field_stage_e field_stage,
    unsigned int fes_id,
    dnx_field_fes_mask_id_t mask_id,
    int set)
{
    uint8 is_alloc;

    SHR_FUNC_INIT_VARS(unit);

    if (set)
    {
        is_alloc = TRUE;
    }
    else
    {
        is_alloc = FALSE;
    }

    /*
     * Write to SW state
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                            mask_is_alloc.set(unit, fes_id, mask_id, is_alloc));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                            mask_is_alloc.set(unit, fes_id, mask_id, is_alloc));

            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.mask_is_alloc.set(unit, fes_id, mask_id, is_alloc));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "In stage \"%s\" FES ID %d mask %d was set to \"%s\".\r\n.",
                 DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), fes_id, mask_id,
                 (set ? "allocated" : "free"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Update the algo_field_action SW state.
 *  Used to consolidate the SW state write functions so that noting will be left out.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] fg_id -
 *  HW identifier of field group.
 * \param [in] context_id -
 *  Cotnext ID.
 * \param [in] is_shared -
 *  If the new addition/deletion is a sharewd FES quartet (and thus only the context state should be updatad).
 * \param [in] place_in_fg -
 *  The ordinal position of the FES quartet within its field group.
 * \param [in] fes_id -
 *  The FES ID that we change.
 * \param [in] fes_pgm_id -
 *  The FES program ID that we change.
 * \param [in] fes_mask_id_delete -
 *  The FES mask IDs to deallocate.
 * \param [in] dest_fes_mask_id_add -
 *  The FES mask IDs to allocate.
 * \param [in] priority -
 *  The priority of the FES quartet.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 *   * dnx_algo_field_action_fes_allocate_update_sw_state_single().
 */
static shr_error_e
dnx_algo_field_action_fes_update_sw_state_single(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    uint8 is_shared,
    unsigned int place_in_fg,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t fes_pgm_id,
    dnx_field_fes_mask_id_t fes_mask_id_delete[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES],
    dnx_field_fes_mask_id_t dest_fes_mask_id_add[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES],
    dnx_field_action_priority_t priority)
{
    unsigned int fes2msb_ndx;
    unsigned int mask_ndx;
    int mask_delete[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    int mask_add[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_mask_id_delete, _SHR_E_PARAM, "fes_mask_id_delete");
    SHR_NULL_CHECK(dest_fes_mask_id_add, _SHR_E_PARAM, "dest_fes_mask_id_add");

    /*
     * Only write to the FES state if the FES quartet is unshared.
     */
    if (is_shared == FALSE)
    {
        /*
         * Add and/or delete masks.
         */
        for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
        {
            mask_delete[mask_ndx] = FALSE;
            mask_add[mask_ndx] = FALSE;
        }
        for (fes2msb_ndx = 0; fes2msb_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes2msb_ndx++)
        {
            if (fes_mask_id_delete[fes2msb_ndx] != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                mask_delete[fes_mask_id_delete[fes2msb_ndx]] = TRUE;
            }
            if (dest_fes_mask_id_add[fes2msb_ndx] != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                mask_add[dest_fes_mask_id_add[fes2msb_ndx]] = TRUE;
            }
        }
        for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
        {
            if (mask_add[mask_ndx])
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_mask
                                (unit, field_stage, fes_id, mask_ndx, TRUE));
            }
            else if (mask_delete[mask_ndx])
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_mask
                                (unit, field_stage, fes_id, mask_ndx, FALSE));
            }
        }

        /*
         * Write the allocated FES state.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_fes_state
                        (unit, field_stage, fg_id, fes_id, fes_pgm_id, place_in_fg));
    }

    /*
     * Update the context state (priority).
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_priority
                    (unit, field_stage, context_id, fes_id, priority));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Collect information from the current state and the SW state to fill a 
 *  dnx_algo_field_action_fes_alloc_current_state_inclusive_t structure and after_invalidate_next.
 * \param [in] unit -
 *   Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] fg_id -
 *   HW identifier of field group.
 * \param [in] context_id -
 *   The context ID.
 * \param [in] post_attach -
 *   If we add more EFES after context attach.
 * \param [in] current_state_in_p -
 *   The current state of the FESes given to the allocation algorithm from the outside
 *   (as opposed to the current state in algo field SW state).
 * \param [out] current_state_changing_p -
 *   To be loaded with the current state for each FES quartet, including SW state information. 
 *   This information wil be used for updating during the allocation process.
 * \param [out] after_invalidate_next -
 *   To be loaded for each FES ID if it comes after invalidate next.
 * \param [out] first_place_in_fg_p -
 *   To be loaded with the place in fg to allocate.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 *   * dnx_algo_field_action_fes_allocate_non_mandatory_position()
 *   * dnx_algo_field_action_fes_allocate__mandatory_position()
 *   * dnx_algo_field_action_fes_allocate_add()
 *   * dnx_algo_field_action_fes_allocate_collect_new_actions_info()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_collect_current_state_info(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    int post_attach,
    dnx_algo_field_action_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    unsigned int *first_place_in_fg_p)
{
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes2msb_ndx;
    int invalidate_next_found;
    signed int previous_last_ordinal_place;
    dnx_field_group_t stored_fg_id;
    uint8 stored_place_in_fg;
    dnx_field_action_priority_t stored_priority;
    dnx_field_action_type_t invalidate_next_action_type;
    unsigned int nof_existing_actions_in_fg;
    uint8 occupied_spot_post_attach[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP] = { 0 };
    int available_place_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_in_p, _SHR_E_PARAM, "current_state_in_p");
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(after_invalidate_next, _SHR_E_PARAM, "after_invalidate_next");
    SHR_NULL_CHECK(first_place_in_fg_p, _SHR_E_PARAM, "first_place_in_fg_p");

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalidate_next_action_type(unit, field_stage, &invalidate_next_action_type));

    /*
     * Zero out current_state_changing_p for predictability.
     */
    sal_memset(current_state_changing_p, 0x0, sizeof(*current_state_changing_p));
    /*
     * Initialize after_invalidate_next.
     * The first FES ID cannot be after invalidate next. Note we assume FES always come before FEM.
     */
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        after_invalidate_next[fes_id_ndx] = FALSE;
    }

    /*
     * Find the last ordinal place in the field group to start counting from for the new actions.
     * Fill current_state_changing_p.
     * Fill after invalidate next.
     */
    nof_existing_actions_in_fg = 0;
    previous_last_ordinal_place = -1;
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        current_state_changing_p->fes_pgm_id[fes_id_ndx] =
            current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id;
        if (current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            /*
             * Read from SW state
             */
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                case DNX_FIELD_STAGE_IPMF2:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.field_group.get
                                    (unit, fes_id_ndx,
                                     current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id,
                                     &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.place_in_fg.get(unit, fes_id_ndx,
                                                                                             current_state_in_p->
                                                                                             context_state.
                                                                                             fes_id_info[fes_id_ndx].
                                                                                             fes_pgm_id,
                                                                                             &stored_place_in_fg));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.get(unit, context_id, fes_id_ndx, &stored_priority));
                    break;
                case DNX_FIELD_STAGE_IPMF3:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.field_group.get
                                    (unit, fes_id_ndx,
                                     current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id,
                                     &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.place_in_fg.get(unit, fes_id_ndx,
                                                                                             current_state_in_p->
                                                                                             context_state.
                                                                                             fes_id_info[fes_id_ndx].
                                                                                             fes_pgm_id,
                                                                                             &stored_place_in_fg));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.get(unit, context_id, fes_id_ndx, &stored_priority));
                    break;
                case DNX_FIELD_STAGE_EPMF:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.field_group.get
                                    (unit, fes_id_ndx,
                                     current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id,
                                     &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.place_in_fg.get(unit, fes_id_ndx,
                                                                                            current_state_in_p->
                                                                                            context_state.
                                                                                            fes_id_info[fes_id_ndx].
                                                                                            fes_pgm_id,
                                                                                            &stored_place_in_fg));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.get(unit, context_id, fes_id_ndx, &stored_priority));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
            }
            if (stored_fg_id == fg_id)
            {
                if (previous_last_ordinal_place < (signed int) stored_place_in_fg)
                {
                    previous_last_ordinal_place = stored_place_in_fg;
                }
                if (stored_place_in_fg >= DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG)
                {
                    if ((stored_place_in_fg - DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG) >=
                        (sizeof(occupied_spot_post_attach) / sizeof(occupied_spot_post_attach[0])))
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "stored_place_in_fg (%d) above maximum (%d). \r\n",
                                     stored_place_in_fg,
                                     (int) (DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG +
                                            (sizeof(occupied_spot_post_attach) /
                                             sizeof(occupied_spot_post_attach[0]))));
                    }
                    occupied_spot_post_attach[stored_place_in_fg -
                                              DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG] = 1;
                }
                nof_existing_actions_in_fg++;
            }
            /*
             * Fill current_state_changing_p.
             */
            current_state_changing_p->is_shared[fes_id_ndx] =
                current_state_in_p->context_state.fes_id_info[fes_id_ndx].is_shared;
            sal_memcpy(current_state_changing_p->fes_quartet_info[fes_id_ndx].fes_action_type,
                       current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_action_type,
                       sizeof(current_state_changing_p->fes_quartet_info[0].fes_action_type));
            sal_memcpy(current_state_changing_p->fes_quartet_info[fes_id_ndx].fes_mask_id,
                       current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_mask_id,
                       sizeof(current_state_changing_p->fes_quartet_info[0].fes_mask_id));
            current_state_changing_p->fes_quartet_info[fes_id_ndx].place_in_fg = stored_place_in_fg;
            current_state_changing_p->fes_quartet_info[fes_id_ndx].place_in_fg = stored_place_in_fg;
            current_state_changing_p->fes_quartet_info[fes_id_ndx].fg_id = stored_fg_id;
            current_state_changing_p->fes_quartet_info[fes_id_ndx].priority = stored_priority;
            current_state_changing_p->fes_quartet_info[fes_id_ndx].place_in_alloc = DNX_ALGO_FIELD_ACTION_MOVEMENT;

            /*
             * Check if we have invalidate next.
             */
            invalidate_next_found = FALSE;
            for (fes2msb_ndx = 0;
                 fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes; fes2msb_ndx++)
            {
                if (current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_action_type[fes2msb_ndx] ==
                    invalidate_next_action_type)
                {
                    invalidate_next_found = TRUE;
                    break;
                }
            }
            if (invalidate_next_found && (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(stored_priority) == FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "In context ID %d, field group %d, action number %d in FES ID %d "
                             "FES program Id %d was found an invalidate next with priority that "
                             "does not mandate position 0x%x.\r\n",
                             context_id, stored_fg_id, stored_place_in_fg, fes_id_ndx,
                             current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id, stored_priority);
            }
            /*
             * Fill 'after_invalidate_next'.
             * Note we do note account forr FEMs. For now disregard invalidate next at the end of a FES array.
             */
            if (invalidate_next_found
                && (((fes_id_ndx + 1) % (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array)) != 0))
            {
                /*
                 * Sanity check.
                 */
                if (fes_id_ndx ==
                    (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context - 1))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Last FES ID %d is not the end of an array.\r\n", fes_id_ndx);
                }
                after_invalidate_next[fes_id_ndx + 1] = TRUE;
            }
        }
    }
    /*
     * Sanity check: verify that the number of existing FES quartets matches the maximum ordinal number.
     */
    if ((post_attach == FALSE) &&
        (nof_existing_actions_in_fg > 0) && (nof_existing_actions_in_fg - 1 != previous_last_ordinal_place))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of existing actions in field group %d context %d is %d, does not match "
                     "maximum ordinal place %d.\r\n",
                     fg_id, context_id, nof_existing_actions_in_fg, previous_last_ordinal_place);
    }

    if (post_attach)
    {
        for (available_place_index = 0;
             available_place_index < (sizeof(occupied_spot_post_attach) / sizeof(occupied_spot_post_attach[0]));
             available_place_index++)
        {
            if (occupied_spot_post_attach[available_place_index] == 0)
            {
                (*first_place_in_fg_p) = available_place_index + DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG;
                break;
            }

        }
        if (available_place_index >= (sizeof(occupied_spot_post_attach) / sizeof(occupied_spot_post_attach[0])))
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "No available place to allocate an EFES post attach. "
                         "fg_id %d context_id %d.\r\n", fg_id, context_id);
        }
    }
    else
    {
        (*first_place_in_fg_p) = nof_existing_actions_in_fg;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Collects information from the new FES quartets to add so that they will have the same format as the FES quartet.
 *  Also adds the new after_invalidate_next stemming from new FES quartets.
 * \param [in] unit -
 *   Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] fg_id -
 *   HW identifier of field group.
 * \param [in] context_id -
 *   The context ID.
 * \param [in] fes_fg_in_p -
 *   The information about the new field group and context ID and what allocations they require.
 * \param [in] first_place_in_fg -
 *   The number from which to allocate place_in_fg
 * \param [in,out] after_invalidate_next -
 *   To be updated with the FES IDs that will become after invalidate next after the allocation.
 * \param [out] after_invalidate_next_new -
 *   To be loaded with the FES IDs that will become after invalidate next after the allocation.
 *   Used for clearing those FES IDs of actions with non mandatory placement.
 * \param [out] fes_quartet -
 *   To be loaded with the new FES quartet and their information.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 *   * dnx_algo_field_action_fes_allocate_non_mandatory_position()
 *   * dnx_algo_field_action_fes_allocate__mandatory_position()
 *   * dnx_algo_field_action_fes_allocate_add()
 *   * dnx_algo_field_action_fes_allocate_collect_current_state_info()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_collect_new_actions_info(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_in_t * fes_fg_in_p,
    unsigned int first_place_in_fg,
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next_new[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t fes_quartet[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    unsigned int fes_quartet_ndx;
    int invalidate_next_found;
    unsigned int fes2msb_ndx;
    dnx_field_fes_id_t fes_id_ndx;
    dnx_field_fes_id_t fes_id;
    dnx_field_action_type_t invalidate_next_action_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_fg_in_p, _SHR_E_PARAM, "fes_fg_in_p");
    SHR_NULL_CHECK(fes_quartet, _SHR_E_PARAM, "fes_quartet");

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalidate_next_action_type(unit, field_stage, &invalidate_next_action_type));

    /*
     * Zero out fes_quartet for predictability.
     */
    sal_memset(fes_quartet, 0x0, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP * sizeof(fes_quartet[0]));
    /*
     * Initialize ''after_invalidate_next_new'.
     */
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        after_invalidate_next_new[fes_id_ndx] = FALSE;
    }

    for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
    {
        /*
         * Sanity check
         */
        if (fes_quartet_ndx >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FES quartets to allocate %d larger than number of action per "
                         "field group %d.\r\n",
                         fes_fg_in_p->nof_fes_quartets, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);
        }
        /*
         * Fill fes_quartet.
         */
        sal_memcpy(fes_quartet[fes_quartet_ndx].fes_action_type,
                   fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_action_type, sizeof(fes_quartet[0].fes_action_type));
        sal_memcpy(fes_quartet[fes_quartet_ndx].fes_mask_id,
                   fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_mask_id, sizeof(fes_quartet[0].fes_mask_id));
        fes_quartet[fes_quartet_ndx].fg_id = fg_id;
        fes_quartet[fes_quartet_ndx].priority = fes_fg_in_p->fes_quartet[fes_quartet_ndx].priority;
        fes_quartet[fes_quartet_ndx].place_in_alloc = fes_quartet_ndx;
        fes_quartet[fes_quartet_ndx].place_in_fg = fes_quartet_ndx + first_place_in_fg;
        /** Sanity check*/
        if ((first_place_in_fg < DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG) &&
            (fes_quartet[fes_quartet_ndx].place_in_fg >= DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "first_place_in_fg is %d, but allocating after %d with place_in_fg %d. action %d.\r\n",
                         first_place_in_fg, DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG,
                         fes_quartet[fes_quartet_ndx].place_in_fg, fes_quartet_ndx);
        }

        /*
         * Check if we have invalidate next.
         */
        invalidate_next_found = FALSE;
        for (fes2msb_ndx = 0; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            if (fes_quartet[fes_quartet_ndx].fes_action_type[fes2msb_ndx] == invalidate_next_action_type)
            {
                invalidate_next_found = TRUE;
                break;
            }
        }
        if (invalidate_next_found
            && (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(fes_quartet[fes_quartet_ndx].priority) == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Cannot allocate a FES quartet with invalidate next and priority that doesn't mandate a "
                         "FES position. Attempted to do so in context ID %d, field group %d, action number %d, "
                         "priority 0x%x.\r\n",
                         context_id, fg_id, fes_quartet[fes_quartet_ndx].place_in_fg,
                         fes_quartet[fes_quartet_ndx].priority);
        }
        /*
         * Add to 'after_invalidate_next' and 'after_invalidate_next_new'.
         */
        if (invalidate_next_found
            && (DNX_FIELD_ACTION_PRIORITY_PRIORITY_POSITION(fes_quartet[fes_quartet_ndx].priority) !=
                dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array - 1))
        {
            DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id, unit, field_stage,
                                                          fes_quartet[fes_quartet_ndx].priority);
            /*
             * Sanity check.
             */
            if (fes_id == (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context - 1))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Last FES ID %d is not the end of an array.\r\n", fes_id);
            }
            after_invalidate_next[fes_id + 1] = TRUE;
            after_invalidate_next_new[fes_id + 1] = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Compare two FES quartets, which has the higher precedence (going first by priority, then by field group then 
 *  by place in field group).
 * \param [in] unit -
 *  Device ID.
 * \param [in] first_priority -
 *  The first priority to be compared.
 * \param [in] first_fg_id -
 *  The field group ID associated with the first priority to be used as tie breaker.
 * \param [in] first_place_in_fg -
 *  The place within field group ID associated with the first priority to be used as tie breaker.
 * \param [in] second_priority -
 *  The second priority to be compared.
 * \param [in] second_fg_id -
 *  The field group ID associated with the second priority to be used as tie breaker.
 * \param [in] second_place_in_fg -
 *  The second place within field group ID associated with the first priority to be used as tie breaker.
 * \param [out] compare_result_p -
 *  To be loaded with which priority is the higher priority.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * The function blocks the possibility for not tie breaking, so if the same FES quartet is compared with itself,
 *     it will send an error.
 * \see
 *   * dnx_algo_field_action_fes_allocate_non_mandatory_position()
 *   * dnx_algo_field_action_fes_allocate()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_priority_compare(
    int unit,
    dnx_field_action_priority_t first_priority,
    dnx_field_group_t first_fg_id,
    unsigned int first_place_in_fg,
    dnx_field_action_priority_t second_priority,
    dnx_field_group_t second_fg_id,
    unsigned int second_place_in_fg,
    dnx_algo_field_action_fes_alloc_priority_compare_result_e * compare_result_p)
{
    uint32 first_fes_array;
    uint32 second_fes_array;
    uint32 first_fes_position;
    uint32 second_fes_position;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(compare_result_p, _SHR_E_PARAM, "compare_result_p");

    if (first_priority == DNX_FIELD_ACTION_PRIORITY_INVALID || second_priority == DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot compare invalid priorities.\r\n");
    }
    if (first_priority == BCM_FIELD_ACTION_INVALIDATE || second_priority == BCM_FIELD_ACTION_INVALIDATE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot compare invalidate priorities.\r\n");
    }
    if (first_priority == BCM_FIELD_ACTION_DONT_CARE || second_priority == BCM_FIELD_ACTION_DONT_CARE)
    {
        (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_INCOMPARABLE;
    }
    else if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(first_priority)
             || DNX_FIELD_ACTION_PRIORITY_IS_POSITION(second_priority))
    {
        (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_INCOMPARABLE;
    }
    else
    {
        first_fes_array = DNX_FIELD_ACTION_PRIORITY_ARRAY(first_priority);
        second_fes_array = DNX_FIELD_ACTION_PRIORITY_ARRAY(second_priority);
        first_fes_position = DNX_FIELD_ACTION_PRIORITY_PRIORITY_POSITION(first_priority);
        second_fes_position = DNX_FIELD_ACTION_PRIORITY_PRIORITY_POSITION(second_priority);

        if (first_fes_array > second_fes_array)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS;
        }
        else if (second_fes_array > first_fes_array)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS;
        }
        else if (first_fes_position > second_fes_position)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS;
        }
        else if (second_fes_position > first_fes_position)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS;
        }
        else if (first_fg_id > second_fg_id)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS;
        }
        else if (second_fg_id > first_fg_id)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS;
        }
        else if (first_place_in_fg > second_place_in_fg)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS;
        }
        else if (second_place_in_fg > first_place_in_fg)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to perform tie breaking between priorities 0x%x and 0x%x.\r\n",
                         first_priority, second_priority);
        }
    }

    /*
     * For now we do not expect to compare "don't care" priorities, so we perform a sanity check.
     */
    if ((*compare_result_p) == DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_INCOMPARABLE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "\"don't care\" priority compares, unexpected.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Given two priorites of actions that have contention with one another, check if the two priorities are allowed 
 *  to have contention.
 *  Contention is allowed if there is a way to tell which action has a higher priority.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for. Used for error logging.
 * \param [in] context_id -
 *  The context ID. Used for error logging.
 * \param [in] first_priority -
 *  The priority of the first action in contention.
 * \param [in] first_fg_id -
 *  The field group ID of the first action. Used for error logging.
 * \param [in] first_place_in_fg -
 *  The place in the field group of the first action. Used for error logging.
 * \param [in] second_priority -
 *  The priority of the second action in contention.
 * \param [in] second_fg_id -
 *  The field group ID of the second action. Used for error logging.
 * \param [in] second_place_in_fg -
 *  The place in the field group of the second action. Used for error logging.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None.
 * \see
 *   * dnx_algo_field_action_fes_allocate_contention_find()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_contention_priorities_allowed(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_action_priority_t first_priority,
    dnx_field_group_t first_fg_id,
    unsigned int first_place_in_fg,
    dnx_field_action_priority_t second_priority,
    dnx_field_group_t second_fg_id,
    unsigned int second_place_in_fg)
{
    int first_is_dont_care;
    int second_is_dont_care;
    int first_is_mandatory_position;
    int second_is_mandatory_position;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If any priority is invalid, send an error.
     */
    if (first_priority == DNX_FIELD_ACTION_PRIORITY_INVALID || second_priority == DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid priority encountered.\r\n");
    }

    first_is_dont_care = (first_priority == BCM_FIELD_ACTION_DONT_CARE);
    second_is_dont_care = (second_priority == BCM_FIELD_ACTION_DONT_CARE);
    first_is_mandatory_position = DNX_FIELD_ACTION_PRIORITY_IS_POSITION(first_priority);
    second_is_mandatory_position = DNX_FIELD_ACTION_PRIORITY_IS_POSITION(second_priority);

    /*
     * If only one action is "don't care" we cannot compare them.
     */
    if ((first_is_dont_care && second_is_dont_care == FALSE) || (first_is_dont_care == FALSE && second_is_dont_care))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between non void action %d of fg_id %d (priority 0x%x) "
                     "and non void action %d of fg_id %d (priority 0x%x) on context %d, stage %s. "
                     "Cannot compare \"don't care\" priority with a non \"don't care\" priority.\r\n",
                     first_place_in_fg, first_fg_id, first_priority, second_place_in_fg, second_fg_id, second_priority,
                     context_id, DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
    }
    /*
     * If only one action mandates position we cannot compare them.
     */
    if ((first_is_mandatory_position && second_is_mandatory_position == FALSE) ||
        (first_is_mandatory_position == FALSE && second_is_mandatory_position))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between non void action %d of fg_id %d (priority 0x%x) "
                     "and non void action %d of fg_id %d (priority 0x%x) on context %d, stage %s. "
                     "Cannot compare mandatory position priority with a non mandatory position priority.\r\n",
                     first_place_in_fg, first_fg_id, first_priority, second_place_in_fg, second_fg_id, second_priority,
                     context_id, DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
    }
    /*
     * If both priorities are indenticle, they cannot be compared.
     */
    if (first_priority == second_priority)
    {
        /*
         * If both proirities are "don't care", provide a special error log.
         */
        if (first_is_dont_care && second_is_dont_care)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between non void action %d of fg_id %d (priority 0x%x) "
                         "and non void action %d of fg_id %d (priority 0x%x) on context %d, stage %s. "
                         "Both actions use \"don't care\" priorities.\r\n",
                         first_place_in_fg, first_fg_id, first_priority,
                         second_place_in_fg, second_fg_id, second_priority,
                         context_id, DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
        }
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between non void action %d of fg_id %d (priority 0x%x) "
                     "and non void action %d of fg_id %d (priority 0x%x) on context %d, stage %s. "
                     "The priorities are identical.\r\n",
                     first_place_in_fg, first_fg_id, first_priority, second_place_in_fg, second_fg_id, second_priority,
                     context_id, DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Check if two action type have contention (i.e. they can override one another).
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] fes_action_type_1 -
 *  An action type, to check if there is contention between it and fes_action_type_2.
 * \param [in] fes_action_type_2 -
 *  An action type, to check if there is contention between it and fes_action_type_1.
 * \param [out] contention_exists -
 *  To be loaded with TRUE if there is contention between any of the actions in 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None.
 * \see
 *   * dnx_algo_field_action_fes_allocate_contention_find()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_contention_check(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_type_t fes_action_type_1,
    dnx_field_action_type_t fes_action_type_2,
    uint8 *contention_exists)
{
    dnx_field_action_type_t invalidate_next_action_type;
    dnx_field_action_type_t invalid_action_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(contention_exists, _SHR_E_PARAM, "contention_exists");

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalidate_next_action_type(unit, field_stage, &invalidate_next_action_type));
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, field_stage, &invalid_action_type));

    (*contention_exists) = FALSE;
    /*
     * At this point the only contention we know of is if we have the same action type.
     * Action type invalid and invalidate next not included.
     */
    if ((fes_action_type_1 == invalid_action_type) ||
        (fes_action_type_2 == invalid_action_type) ||
        (fes_action_type_1 == invalidate_next_action_type) || (fes_action_type_2 == invalidate_next_action_type))
    {
        (*contention_exists) = FALSE;
    }
    else if (fes_action_type_1 == fes_action_type_2)
    {
        (*contention_exists) = TRUE;;
    }
    else
    {
        (*contention_exists) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find all FES quartets that have contention with a certain FES quartet (that is, at least one action type in one 
 *  FES quartet can override at least one action type in the othe FES quartet)
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] context_id -
 *  The context ID. Used for error logging.
 * \param [in] current_state_changing_p -
 *  The current state of the FES configuration at this point of the allocation algorithm.
 * \param [in] fes_id_currently_placed -
 *  If the is a movement, the FES ID we move from. If the FES quartet is new, we expect it to be the number of FES IDs.
 * \param [in] min_fes_id -
 *  The lowest FES ID to check against.
 * \param [in] max_fes_id -
 *  The highest FES ID to check against.
 * \param [in] fes_quartet_p -
 *  The information about the new action, including the array of the action types of the FES quartet, 
 *  check for contention.
 * \param [out] contention -
 *  To be loaded with TRUE if there is contention between any of the actions in 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None.
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_contention_find(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id_currently_placed,
    int min_fes_id,
    int max_fes_id,
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    uint8 contention[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT])
{
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes2msb_ndx_1;
    unsigned int fes2msb_ndx_2;
    dnx_field_fes_id_t min_fes_id_intern;
    dnx_field_fes_id_t max_fes_id_intern;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(contention, _SHR_E_PARAM, "contention");

    /*
     * Initialize contention array.
     */
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        contention[fes_id_ndx] = FALSE;
    }

    if (min_fes_id < 0)
    {
        min_fes_id_intern = 0;
    }
    else
    {
        min_fes_id_intern = min_fes_id;
    }
    if (max_fes_id >=
        (signed int) (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context))
    {
        max_fes_id_intern = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context - 1;
    }
    else
    {
        max_fes_id_intern = max_fes_id;
    }

    if (max_fes_id >= 0)
    {
        for (fes_id_ndx = min_fes_id_intern; (signed int) fes_id_ndx <= max_fes_id_intern; fes_id_ndx++)
        {
            /*
             * Don't check contention again the action itself if it is a movement.
             */
            if (fes_id_ndx == fes_id_currently_placed)
            {
                continue;
            }
            /*
             * Check contention between any of the action types in the FES quartets, 
             * if there is an allocated FES quartet in the FES ID.
             */
            if (current_state_changing_p->fes_pgm_id[fes_id_ndx] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                for (fes2msb_ndx_1 = 0;
                     (fes2msb_ndx_1 < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes)
                     && (contention[fes_id_ndx] == FALSE); fes2msb_ndx_1++)
                {
                    for (fes2msb_ndx_2 = 0;
                         (fes2msb_ndx_2 < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes)
                         && (contention[fes_id_ndx] == FALSE); fes2msb_ndx_2++)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_contention_check
                                        (unit, field_stage,
                                         fes_quartet_p->fes_action_type[fes2msb_ndx_1],
                                         current_state_changing_p->
                                         fes_quartet_info[fes_id_ndx].fes_action_type[fes2msb_ndx_2],
                                         &(contention[fes_id_ndx])));
                    }
                }
                if (contention[fes_id_ndx])
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_contention_priorities_allowed
                                    (unit, field_stage, context_id,
                                     fes_quartet_p->priority,
                                     fes_quartet_p->fg_id,
                                     fes_quartet_p->place_in_fg,
                                     current_state_changing_p->fes_quartet_info[fes_id_ndx].priority,
                                     current_state_changing_p->fes_quartet_info[fes_id_ndx].fg_id,
                                     current_state_changing_p->fes_quartet_info[fes_id_ndx].place_in_fg));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  A function to check if the FES quartet can be shared in the FES ID,
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] fg_id -
 *  HW identifier of field group.
 * \param [in] current_state_changing_p -
 *   The current state of the FESes.
 * \param [in] fes_id -
 *  The FES ID to look for sharing options in.
 * \param [in] place_in_fg -
 *  The ordinal number of the action in the field group.
 * \param [in] allow_fes_pgm_sharing -
 *   Whether we allow FES program sharing. Disabled for direct extraction.
 * \param [out] is_shareable_p -
 *  Pointer to an integer returning whether we found a sharing option.
 * \param [out] fes_pgm_id_p -
 *  Pointer to an unsigned integer. If we found a sharing option,
 *  Returns which FES quartet it is.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 *   * dnx_algo_field_action_fes_allocate_is_allocable()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_shareble_fes_pgm(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id,
    unsigned int place_in_fg,
    int allow_fes_pgm_sharing,
    uint8 *is_shareable_p,
    dnx_field_fes_pgm_id_t * fes_pgm_id_p)
{
    unsigned int fes_pgm_ndx;
    dnx_field_group_t stored_fg_id;
    uint8 stored_place_in_fg;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(is_shareable_p, _SHR_E_PARAM, "is_shareable_p");
    SHR_NULL_CHECK(fes_pgm_id_p, _SHR_E_PARAM, "fes_pgm_id_p");

    if (allow_fes_pgm_sharing)
    {
        /*
         * Find The FES quartet with the lowest FES program ID that has the same field group and the
         * same ordinal number within the field group.
         */
        (*is_shareable_p) = FALSE;
        (*fes_pgm_id_p) = (dnx_field_fes_pgm_id_t) (-1);
        for (fes_pgm_ndx = 0; fes_pgm_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_programs;
             fes_pgm_ndx++)
        {
            /*
             * The zero fes program ID isn't allocable.
             */
            if (fes_pgm_ndx == DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                continue;
            }
            /*
             * Read from SW state
             */
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                case DNX_FIELD_STAGE_IPMF2:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.field_group.get
                                    (unit, fes_id, fes_pgm_ndx, &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.place_in_fg.get
                                    (unit, fes_id, fes_pgm_ndx, &stored_place_in_fg));
                    break;
                case DNX_FIELD_STAGE_IPMF3:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.field_group.get
                                    (unit, fes_id, fes_pgm_ndx, &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                    place_in_fg.get(unit, fes_id, fes_pgm_ndx, &stored_place_in_fg));
                    break;
                case DNX_FIELD_STAGE_EPMF:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.field_group.get
                                    (unit, fes_id, fes_pgm_ndx, &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.place_in_fg.get
                                    (unit, fes_id, fes_pgm_ndx, &stored_place_in_fg));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
            }
            /*
             * Compare the field group and position within the field group.
             * Assuming user requested field group cannot be invalid
             * (tested in dnx_algo_field_action_fes_allocate_verify()).
             */
            if (fg_id == stored_fg_id && place_in_fg == stored_place_in_fg)
            {
                /*
                 * Sanity check: Verify that the shared FES quartet isn't used by the same context ID and may be 
                 * evacuated. Shouldn't happen since the two FES quartets shouldn't have the same fg_id and place_in_fg.
                 */
                if (current_state_changing_p->fes_pgm_id[fes_id] == fes_pgm_ndx)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached a result where FES quartet may share itself.\r\n");
                }
                /*
                 * If sharable, return as such and stop looking.
                 */
                (*is_shareable_p) = TRUE;
                (*fes_pgm_id_p) = fes_pgm_ndx;
                break;
            }
        }
    }
    else
    {
        (*is_shareable_p) = FALSE;
        (*fes_pgm_id_p) = (dnx_field_fes_pgm_id_t) (-1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  A function to check if the FES ID has available action masks for allocation.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] current_state_changing_p -
 *   The current state of the FESes.
 * \param [in] fes_id -
 *  The FES ID to look for allocation options in.
 * \param [in] fes_quartet_p -
 *  The FES quartet we allocate for.
 * \param [in] allow_force_evacuation -
 *  Both allows and forces the evacuation of an existing FES quartet in the FES ID.
 *  Fails if not such FES quartet is found.
 *  FES quartet belonging to the context.
 * \param [out] masks_available_p -
 *  Loaded with whether or not we found available mask IDs to allocate.
 * \param [out] fes_mask_id -
 *  Loaded with the mask IDs that can be allocated for each 2msb instruction.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate_space_available()
 *   * dnx_algo_field_action_fes_allocate()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_masks(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id,
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    int allow_force_evacuation,
    uint8 *masks_available_p,
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES])
{
    unsigned int fes_mask_ndx;
    unsigned int fes_inst_ndx;
    uint8 mask_allocated;
    int found;
    uint8 masks_used_vacated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    dnx_field_fes_mask_id_t mask_requested_allocated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    int run_check;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(masks_available_p, _SHR_E_PARAM, "masks_available_p");
    SHR_NULL_CHECK(fes_mask_id, _SHR_E_PARAM, "fes_mask_id");

    /*
     * Initialize mask_requested_allocated and masks_used_vacated
     */
    for (fes_mask_ndx = 0; fes_mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; fes_mask_ndx++)
    {
        masks_used_vacated[fes_mask_ndx] = FALSE;
        mask_requested_allocated[fes_mask_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }

    run_check = TRUE;

    /*
     * For each mask in the FES, check if it is used by the context and not shared.
     */
    if ((current_state_changing_p->fes_pgm_id[fes_id] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM) &&
        (current_state_changing_p->is_shared[fes_id] == FALSE) && allow_force_evacuation)
    {
        for (fes_mask_ndx = 0; fes_mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
             fes_mask_ndx++)
        {
            if (fes_mask_ndx != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
            {
                for (fes_inst_ndx = 0;
                     fes_inst_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
                     fes_inst_ndx++)
                {
                    if (current_state_changing_p->fes_quartet_info[fes_id].fes_mask_id[fes_inst_ndx] == fes_mask_ndx)
                    {
                        masks_used_vacated[fes_mask_ndx] = TRUE;
                    }
                }
            }
        }
    }
    /*
     * If we force evacuation, only check if we have a program to evacuate.
     */
    if ((current_state_changing_p->fes_pgm_id[fes_id] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM) && allow_force_evacuation)
    {
        run_check = FALSE;
    }

    if (run_check)
    {
        /*
         * Try to allocate for each distinct mask.
         */
        fes_mask_ndx = 0;
        found = TRUE;
        for (fes_inst_ndx = 0; fes_inst_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes_inst_ndx++)
        {
            /*
             * Check if the instruction requires mask allocation. If so, check if it was already allocated.
             */
            if (fes_quartet_p->fes_mask_id[fes_inst_ndx] == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                fes_mask_id[fes_inst_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
                continue;
            }
            else if (mask_requested_allocated[fes_quartet_p->fes_mask_id[fes_inst_ndx]] !=
                     DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                fes_mask_id[fes_inst_ndx] = mask_requested_allocated[fes_quartet_p->fes_mask_id[fes_inst_ndx]];
                continue;
            }

            /*
             * Look for a free mask.
             */
            found = FALSE;
            for (; fes_mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
                 fes_mask_ndx++)
            {
                /*
                 * Zero mask isn't allocable, and we skip it.
                 */
                if (fes_mask_ndx == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
                {
                    continue;
                }
                /*
                 * Read from SW state
                 */
                switch (field_stage)
                {
                    case DNX_FIELD_STAGE_IPMF1:
                    case DNX_FIELD_STAGE_IPMF2:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                                        mask_is_alloc.get(unit, fes_id, fes_mask_ndx, &mask_allocated));
                        break;
                    case DNX_FIELD_STAGE_IPMF3:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                        mask_is_alloc.get(unit, fes_id, fes_mask_ndx, &mask_allocated));
                        break;
                    case DNX_FIELD_STAGE_EPMF:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                                        mask_is_alloc.get(unit, fes_id, fes_mask_ndx, &mask_allocated));
                        break;
                    default:
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
                }
                /*
                 * Check if mask already allocated, and not evacuated.
                 */
                if (mask_allocated && masks_used_vacated[fes_mask_ndx] == FALSE)
                {
                    continue;
                }
                /*
                 * If mask not allocated, or cabaple of being evacuated, allocate it.
                 */
                fes_mask_id[fes_inst_ndx] = fes_mask_ndx;
                mask_requested_allocated[fes_quartet_p->fes_mask_id[fes_inst_ndx]] = fes_mask_ndx;
                found = TRUE;
                fes_mask_ndx++;
                break;
            }
            if (found == FALSE)
            {
                break;
            }
        }
        if (found == FALSE)
        {
            (*masks_available_p) = FALSE;
        }
        else
        {
            (*masks_available_p) = TRUE;
        }
    }
    else
    {
        /*
         * No evacuation can occur.
         */
        (*masks_available_p) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  A function to check if the FES ID has a FES quartet available for allocation, including available action masks,
 *  without sharing.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] current_state_changing_p -
 *   The current state of the FESes.
 * \param [in] fes_id -
 *  The FES ID to look for allocation options in.
 * \param [in] fes_quartet_p -
 *  The FES quartet we allocate for.
 * \param [in] allow_evacuation -
 *  Whether check also the option of removing a FES ID belonging to the context in the FES ID to allocate a new FES ID.
 * \param [out] space_available_p -
 *  Loaded with indication whether the FES ID can be allocated for the FES quartet.
 * \param [out] space_available_if_evacuated_p -
 *  Loaded with indication whether the FES ID can be allocated for the FES quartet, if we remove an existing 
 *  FES quartet belonging to the context.
 * \param [out] fes_pgm_id_p -
 *  Loaded with the lowest available (or shareable) fes program ID in the FES ID.
 * \param [out] fes_mask_id -
 *  Loaded with the mask IDs that can be allocated for each 2msb instruction.

 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 *   * dnx_algo_field_action_fes_allocate_is_allocable()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_space_available(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id,
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    int allow_evacuation,
    uint8 *space_available_p,
    uint8 *space_available_if_evacuated_p,
    dnx_field_fes_pgm_id_t * fes_pgm_id_p,
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES])
{
    dnx_field_group_t field_group_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(space_available_p, _SHR_E_PARAM, "space_available_p");
    SHR_NULL_CHECK(space_available_if_evacuated_p, _SHR_E_PARAM, "space_available_if_evacuated_p");
    SHR_NULL_CHECK(fes_pgm_id_p, _SHR_E_PARAM, "fes_pgm_id_p");
    SHR_NULL_CHECK(fes_mask_id, _SHR_E_PARAM, "fes_mask_id");

    (*space_available_p) = FALSE;
    (*space_available_if_evacuated_p) = FALSE;
    /*
     * If the FES ID is in use by the context, it can only be allocated if evacuated, and then only if the priority
     * enables it to move.
     */
    if (current_state_changing_p->fes_pgm_id[fes_id] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM || allow_evacuation)
    {

        for ((*fes_pgm_id_p) = 0;
             (*fes_pgm_id_p) < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_programs;
             (*fes_pgm_id_p)++)
        {
            /*
             * The zero fes program ID isn't allocable.
             */
            if ((*fes_pgm_id_p) == DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                continue;
            }
            /*
             * Read from SW state
             */
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                case DNX_FIELD_STAGE_IPMF2:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.field_group.get
                                    (unit, fes_id, (*fes_pgm_id_p), &field_group_id));
                    break;
                case DNX_FIELD_STAGE_IPMF3:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.field_group.get
                                    (unit, fes_id, (*fes_pgm_id_p), &field_group_id));
                    break;
                case DNX_FIELD_STAGE_EPMF:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.field_group.get
                                    (unit, fes_id, (*fes_pgm_id_p), &field_group_id));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
            }
            /*
             * Check if free.
             */
            if (field_group_id == DNX_FIELD_GROUP_INVALID)
            {
                if (current_state_changing_p->fes_pgm_id[fes_id] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
                {
                    (*space_available_p) = TRUE;
                }
                (*space_available_if_evacuated_p) = TRUE;
                break;
            }
        }
        /*
         * Check if we can evacuate a FES program.
         * Note that if the FES quartet is shared we still only use a free FES program ID.
         */
        if ((*space_available_p) == FALSE
            && current_state_changing_p->fes_pgm_id[fes_id] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM
            && DNX_FIELD_ACTION_PRIORITY_IS_POSITION(current_state_changing_p->fes_quartet_info[fes_id].priority) ==
            FALSE && current_state_changing_p->is_shared[fes_id] == FALSE)
        {
            (*space_available_if_evacuated_p) = TRUE;
            if (current_state_changing_p->fes_pgm_id[fes_id] < (*fes_pgm_id_p))
            {
                (*fes_pgm_id_p) = current_state_changing_p->fes_pgm_id[fes_id];
            }
        }

        /*
         * If we have available fes quartet, test for masks and return the result.
         */
        if ((*space_available_if_evacuated_p) == TRUE)
        {
            /*
             * Check if we can allocate masks without evacuation.
             */
            if (*space_available_p)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_masks
                                (unit, field_stage, current_state_changing_p, fes_id, fes_quartet_p,
                                 FALSE, space_available_p, fes_mask_id));
            }
            /*
             * Check if we can allocate masks with evacuation.
             * Note the space_available_p could have been set to false by dnx_algo_field_action_fes_allocate_masks().
             */
            if ((*space_available_p) == FALSE)
            {
                /*
                 * Check if we can allocate masks with evacuation.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_masks
                                (unit, field_stage, current_state_changing_p, fes_id, fes_quartet_p,
                                 TRUE, space_available_if_evacuated_p, fes_mask_id));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  A function to check if the FES ID has a FES quartet available for allocation, including available action masks.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] current_state_changing_p -
 *   The current state of the FESes.
 * \param [in] fes_id -
 *  The FES ID to look for sharing/allocation options in.
 * \param [in] fes_quartet_p -
 *  The FES quartet we allocate/share for.
 * \param [in] allow_fes_pgm_sharing -
 *  Whether we allow FES program sharing. Disabled for direct extraction.
 * \param [in] allow_evacuation -
 *  Whether check also the option of removing a FES ID belonging to the context in the FES ID to allocate a new FES ID.
 * \param [out] is_shareable_p -
 *  Loaded with indication whether the FES ID can be shared.
 * \param [out] is_allocable_p -
 *  Loaded with indication whether the FES ID can be allocated for the FES quartet.
 * \param [out] is_allocable_if_evacuated_p -
 *  Loaded with indication whether the FES ID can be allocated for the FES quartet, if we remove an existing 
 *  FES quartet belonging to the context.
 * \param [out] fes_pgm_id_p -
 *  Loaded with the lowest available (or shareable) fes program ID in the FES ID.
 * \param [out] fes_mask_id -
 *  Loaded with the mask IDs that can be allocated for each 2msb instruction.
 *  Not loaded with anything if it is shareable.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_is_allocable(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id,
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    int allow_fes_pgm_sharing,
    int allow_evacuation,
    uint8 *is_shareable_p,
    uint8 *is_allocable_p,
    uint8 *is_allocable_if_evacuated_p,
    dnx_field_fes_pgm_id_t * fes_pgm_id_p,
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES])
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(is_allocable_p, _SHR_E_PARAM, "is_allocable_p");
    SHR_NULL_CHECK(fes_pgm_id_p, _SHR_E_PARAM, "fes_pgm_id_p");
    SHR_NULL_CHECK(fes_mask_id, _SHR_E_PARAM, "fes_mask_id");

    (*is_allocable_p) = FALSE;
    (*is_allocable_if_evacuated_p) = FALSE;

    /*
     * Check if the FES ID isn't occupied by an unmoveable FES quartet in the same context ID.
     */
    if (current_state_changing_p->fes_pgm_id[fes_id] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        DNX_FIELD_ACTION_PRIORITY_IS_POSITION(current_state_changing_p->fes_quartet_info[fes_id].priority) == FALSE)
    {
        /*
         * Check if the FES quartet is shareable.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_shareble_fes_pgm
                        (unit, field_stage, fes_quartet_p->fg_id, current_state_changing_p, fes_id,
                         fes_quartet_p->place_in_fg, allow_fes_pgm_sharing, is_shareable_p, fes_pgm_id_p));
        /*
         * If it is shareable, mark as allocable with or without evacuation. 
         * If it isn't, check if we can allocate a new FES quartet.
         * Note that we assume that we can't share the evacuated FES quartet due to different place_in_fg.
         */
        if (*is_shareable_p)
        {
            if (current_state_changing_p->fes_pgm_id[fes_id] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                (*is_allocable_p) = TRUE;
                (*is_allocable_if_evacuated_p) = TRUE;
            }
            else if (allow_evacuation)
            {
                (*is_allocable_if_evacuated_p) = TRUE;
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_space_available
                            (unit, field_stage, current_state_changing_p, fes_id,
                             fes_quartet_p, allow_evacuation, is_allocable_p, is_allocable_if_evacuated_p,
                             fes_pgm_id_p, fes_mask_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Calculates the order of preference by which a new FES quartet to is be allocated, without pushing aside any 
 *  contention-priority FES quartets.
 *  Also finds where we can try to allocate within the range.
 * This is the procedure to try and shape the location of the allocations.
 * This function is used to set the distribution of the allocation. At the moment it only allocated
 * from the default FES ID to the right and then the left.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] banned_fes_id -
 *  FES IDs to to allocate.
 * \param [in] after_invalidate_next -
 *  FES IDs after invalidate next, to to allocate (because the priorities the use this function don't have mandatory 
 *  placement).
 * \param [in] fes_quartet_p -
 *  The FES quartet to allocate.
 * \param [in] fes_id_currently_placed -
 *  If this is a movement, the FES ID it moves from (and to which it shouldn't move)
 *  If it is a new FES quartet, is shouldn't be any FES quartet within the permissible range.
 * \param [in] min_fes_id -
 *  The minimum FES ID to look find allocation in.
 * \param [in] max_fes_id -
 *  The maximum FES ID to look find allocation in.
 * \param [in] allow_fes_pgm_sharing -
 *  Whether to allow sharing the FES quartet with other contexts. Disabled for direct extraction.
 * \param [in] current_state_changing_p -
 *  The current state of the FES configuration.
 * \param [out] fes_id_order -
 *  The order to try and allocate FES IDs.
 * \param [out] nof_fes_id_order_p -
 *  The number of elements in fes_id_order.
 * \param [out] shareable -
 *  What FES IDs can be shared with other contexts.
 * \param [out] allocable -
 *  What FES IDs can be allocated without moving any other FES IDs.
 * \param [out] allocable_if_evacuated -
 *  What FES IDs can be allocated if we move the FES quartet of the context already in the FES ID to another FES ID.
 * \param [out] fes_pgm_id -
 *  The FES program ID to be allocated for each FES ID.
 * \param [out] fes_mask_id -
 *  The FES action masks to be allocated for each FES ID.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate_non_mandatory_position()
 *   * dnx_algo_field_action_fes_allocate()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_order(
    int unit,
    dnx_field_stage_e field_stage,
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    dnx_field_fes_id_t fes_id_currently_placed,
    int min_fes_id,
    int max_fes_id,
    int allow_fes_pgm_sharing,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id_order[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    unsigned int *nof_fes_id_order_p,
    uint8 shareable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 allocable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 allocable_if_evacuated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_field_fes_pgm_id_t fes_pgm_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_field_fes_mask_id_t
    fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
    [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES])
{
    unsigned int fes_id_ndx;
    unsigned int fes_id_order_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(after_invalidate_next, _SHR_E_PARAM, "after_invalidate_next");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(fes_id_order, _SHR_E_PARAM, "fes_id_order");
    SHR_NULL_CHECK(nof_fes_id_order_p, _SHR_E_PARAM, "nof_fes_id_order_p");
    SHR_NULL_CHECK(shareable, _SHR_E_PARAM, "shareable");
    SHR_NULL_CHECK(allocable, _SHR_E_PARAM, "allocable");
    SHR_NULL_CHECK(allocable_if_evacuated, _SHR_E_PARAM, "allocable_if_evacuated");

    /*
     * Sanity check: FES quartets with mandatory placement shouldn't use this function.
     */
    if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(fes_quartet_p->priority))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Function for allocation order called for mandatory placement priority 0x%x field group %d "
                     "action number %d.\r\n",
                     fes_quartet_p->priority, fes_quartet_p->fg_id, fes_quartet_p->place_in_fg);
    }

    /*
     * Find the allocation status of each FES ID
     */
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if ((min_fes_id > (signed int) fes_id_ndx) || (max_fes_id < (signed int) fes_id_ndx) ||
            (banned_fes_id[fes_id_ndx] == TRUE) || (after_invalidate_next[fes_id_ndx] == TRUE) ||
            (fes_id_ndx == fes_id_currently_placed))
        {
            shareable[fes_id_ndx] = FALSE;
            allocable[fes_id_ndx] = FALSE;
            allocable_if_evacuated[fes_id_ndx] = FALSE;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_is_allocable
                            (unit, field_stage, current_state_changing_p, fes_id_ndx, fes_quartet_p,
                             allow_fes_pgm_sharing, TRUE,
                             &shareable[fes_id_ndx], &allocable[fes_id_ndx], &allocable_if_evacuated[fes_id_ndx],
                             &fes_pgm_id[fes_id_ndx], fes_mask_id[fes_id_ndx]));
        }
    }
    for (; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        shareable[fes_id_ndx] = FALSE;
        allocable[fes_id_ndx] = FALSE;
        allocable_if_evacuated[fes_id_ndx] = FALSE;
    }

    fes_id_order_ndx = 0;
    /*
     * First list all no evacuation shareable options.
     */
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if (allocable[fes_id_ndx] && shareable[fes_id_ndx])
        {
            fes_id_order[fes_id_order_ndx] = fes_id_ndx;
            fes_id_order_ndx++;
        }
    }
    /*
     * Secondly list all no evacuation non shareable options.
     */
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if (allocable[fes_id_ndx] && (shareable[fes_id_ndx] == FALSE))
        {
            fes_id_order[fes_id_order_ndx] = fes_id_ndx;
            fes_id_order_ndx++;
        }
    }
    /*
     * Thirdly list all evacuation shareable options.
     */
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if ((allocable[fes_id_ndx] == FALSE) && allocable_if_evacuated[fes_id_ndx] && shareable[fes_id_ndx])
        {
            fes_id_order[fes_id_order_ndx] = fes_id_ndx;
            fes_id_order_ndx++;
        }
    }
    /*
     * Lastly list all evacuation non shareable options.
     */
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if ((allocable[fes_id_ndx] == FALSE) && allocable_if_evacuated[fes_id_ndx] && (shareable[fes_id_ndx] == FALSE))
        {
            fes_id_order[fes_id_order_ndx] = fes_id_ndx;
            fes_id_order_ndx++;
        }
    }

    /*
     * Update the number of FES IDs in the order list.
     */
    (*nof_fes_id_order_p) = fes_id_order_ndx;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Updated current stae, SW state and allocation output with a new allocation/movement.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] context_id -
 *  The context ID.
 * \param [in] fes_quartet_p -
 *   The FES quartet we allocated/moved.
 * \param [in] is_new -
 *   If this is a new FES quartet or a movement of an existing FES quartet.
 * \param [in] source_fes_id -
 *   If this is a movement, where did it move from.
 * \param [in] dest_fes_id -
 *   The FES ID allocated/moved to.
 * \param [in] dest_fes_pgm_id -
 *   The FES program ID allocated/moved to.
 * \param [in] dest_is_shared -
 *   If the place it is allocated/moving to is shared with other contexts.
 * \param [in] dest_fes_mask_id -
 *   If it isn't sharing, what FES action mask IDs was it allocated.
 * \param [in,out] current_state_changing_p -
 *  The current state of the allocationin the context, to be updated with Updated with the allocation/movement.
 * \param [in,out] alloc_result_p -
 *   To be loaded/updated with the new allocation/movement.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * Assumes all movements are within the context ID.
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 *   * dnx_algo_field_action_fes_allocate_non_mandatory_position()
 *   * dnx_algo_field_action_fes_allocate_mandatory_position()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_add(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    int is_new,
    dnx_field_fes_id_t source_fes_id,
    dnx_field_fes_id_t dest_fes_id,
    dnx_field_fes_pgm_id_t dest_fes_pgm_id,
    int dest_is_shared,
    dnx_field_fes_mask_id_t dest_fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES],
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p)
{
    unsigned int nof_elements_out_array;
    unsigned int fes_quartet_ndx;
    unsigned int fes_2msb_ndx;
    dnx_field_fes_mask_id_t masks_unchanged[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(dest_fes_mask_id, _SHR_E_PARAM, "dest_fes_mask_id");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");

    /** Sanity checks.*/
    if (is_new && fes_quartet_p->place_in_alloc == DNX_ALGO_FIELD_ACTION_MOVEMENT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "FES quartet movement was called from a place that allocates a new FES quartet.\r\n");
    }
    if (fes_quartet_p->place_in_alloc != DNX_ALGO_FIELD_ACTION_MOVEMENT && fes_quartet_p->place_in_alloc < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal 'place_in_alloc' value %d.\r\n", fes_quartet_p->place_in_alloc);
    }
    if (is_new == FALSE && source_fes_id == dest_fes_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES movement moved from a FES ID to itself %d.\r\n", source_fes_id);
    }
    /*
     * We allow the scenario of (is_new == FALSE && fes_quartet_p->place_in_alloc != DNX_ALGO_FIELD_ACTION_MOVEMENT)
     * Since we now move actions one by one from withing a field group.
     */

    /*
     * Verify that we do not exceed the maximum size of the FES.
     */
    nof_elements_out_array =
        (sizeof(alloc_result_p->fes_quartet_change) / sizeof(alloc_result_p->fes_quartet_change[0]));
    if (alloc_result_p->nof_fes_quartet_changes >= nof_elements_out_array)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of FES changes exceeds array maximum size %d.\r\n", nof_elements_out_array);
    }

    fes_quartet_ndx = alloc_result_p->nof_fes_quartet_changes;
    /*
     * Initialize the structure for predictability.
     */
    sal_memset(&(alloc_result_p->fes_quartet_change[fes_quartet_ndx]), 0x0,
               sizeof(alloc_result_p->fes_quartet_change[0]));
    for (fes_2msb_ndx = 0; fes_2msb_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes_2msb_ndx++)
    {
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_mask_id[fes_2msb_ndx] =
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id[fes_2msb_ndx] =
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }

    /*
     * Write the status (new or movement).
     */
    if (is_new)
    {
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc = fes_quartet_p->place_in_alloc;
    }
    else
    {
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc = DNX_ALGO_FIELD_ACTION_MOVEMENT;
    }
    /*
     * Write the destination.
     */
    alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_id = dest_fes_id;
    alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_pgm_id = dest_fes_pgm_id;
    alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_is_shared = dest_is_shared;
    if (dest_is_shared == FALSE)
    {
        sal_memcpy(alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id, dest_fes_mask_id,
                   sizeof(alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id));
    }
    /*
     * Write the source if it is a movement.
     */
    if (is_new == FALSE)
    {
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_id = source_fes_id;
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_pgm_id =
            current_state_changing_p->fes_pgm_id[source_fes_id];
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_is_shared =
            current_state_changing_p->is_shared[source_fes_id];
        if (alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_is_shared == FALSE)
        {
            sal_memcpy(alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_mask_id,
                       current_state_changing_p->fes_quartet_info[source_fes_id].fes_mask_id,
                       sizeof(alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_mask_id));
        }
    }
    /*
     * Increment the number of FES changes.
     */
    (alloc_result_p->nof_fes_quartet_changes)++;

    /*
     * Update current_state_changing_p.
     */
    sal_memcpy(&(current_state_changing_p->fes_quartet_info[dest_fes_id]), fes_quartet_p,
               sizeof(current_state_changing_p->fes_quartet_info[0]));
    current_state_changing_p->fes_quartet_info[dest_fes_id].place_in_alloc =
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc;
    sal_memcpy(current_state_changing_p->fes_quartet_info[dest_fes_id].fes_mask_id,
               alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id,
               sizeof(current_state_changing_p->fes_quartet_info[dest_fes_id].fes_mask_id));
    current_state_changing_p->fes_pgm_id[dest_fes_id] = dest_fes_pgm_id;
    current_state_changing_p->is_shared[dest_fes_id] = dest_is_shared;
    if (is_new == FALSE)
    {
        current_state_changing_p->fes_pgm_id[source_fes_id] = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
    }

    /*
     * Update SW state.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes_2msb_ndx++)
    {
        masks_unchanged[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_single
                    (unit, field_stage, fes_quartet_p->fg_id, context_id,
                     dest_is_shared,
                     fes_quartet_p->place_in_fg,
                     alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_id,
                     alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_pgm_id,
                     masks_unchanged, alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id,
                     fes_quartet_p->priority));
    if (is_new == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_single
                        (unit, field_stage, DNX_FIELD_GROUP_INVALID, context_id,
                         alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_is_shared,
                         0,
                         alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_id,
                         alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_pgm_id,
                         alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_mask_id, masks_unchanged,
                         DNX_FIELD_ACTION_PRIORITY_INVALID));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  A sub procedure of dnx_algo_field_action_fes_allocate_non_mandatory_position() for moving aside FES quartets with
 *  contention to extend the range of possible allocation priority wise, and using the same code for both directions
 *  (moving to higher or lower FES IDs).
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] is_direction_up -
 *  If true, direction of movement is to higher FES IDs.
 *  If flase, direction of movement is to lower FES IDs.
 * \param [in] min_fes_id_intern -
 *  The minimum FES ID including contention and priority considerations.
 * \param [in] max_fes_id_intern -
 *  The maximum FES ID including contention and priority considerations.
 * \param [in] context_id -
 *  The context ID.
 * \param [in] banned_fes_id -
 *  FES IDs not to allocate. intended to be used for recursive allocation, which we dont do at the moment
 *  so ther should be only one banned FES ID at the time (when moving contention FES quartets).
 * \param [in] after_invalidate_next -
 *  FES IDs coming after invalidate next cannot be allocated to non mandatory position FES quartet priorities.
 * \param [in] fes_quartet_p -
 *  The information about the FES quartet that we allocate for.
 * \param [in] fes_id_currently_placed -
 *  If the is a movement, the FES ID we move from. If the FES quartet is new, we expect it to be the number of FES IDs.
 * \param [in] is_new -
 *  Whether this is a new FES quartet or a new FES quartet. Deduced from fes_id_currently_placed. 
 * \param [in] min_fes_id -
 *  The minimal FES ID to allocate (not including priority considerations).
 *  Should be at most 0 if this is a new FES quartet.
 * \param [in] max_fes_id -
 *  The maximal FES ID to allocate (not including priority considerations).
 *  Should be at least the last FES ID if this is a new FES quartet.
 * \param [in] min_in_array -
 *  The minimal FES ID to allocate considering the array encoded in the priority.
 *  If the priority is "don't care", it is actually the entire FES ID range.
 * \param [in] max_in_array -
 *  The maximal FES ID to allocate considering the array encoded in the priority.
 *  If the priority is "don't care", it is actually the entire FES ID range.
 * \param [in] allow_contention_moving -
 *  Whether we allow pushing actions with contention. blocks recursion.
 * \param [in] allow_non_contention_moving -
 *  Whether we allow moving away actions without any contention.
 * \param [in] allow_fes_pgm_sharing -
 *  Whether we allow FES program ID sharing. We disallow it for direct extraction.
 * \param [in,out] current_state_changing_p -
 *  The current state of the allocationin the context. Updated with the allocation.
 * \param [in,out] alloc_result_p -
 *   To be loaded/updated with the new allocation/movement.
 * \param [out] found_allocation_p -
 *   Whether the function was successful in finding a legal allocation.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate_non_mandatory_position()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_non_mandatory_position_push(
    int unit,
    dnx_field_stage_e field_stage,
    int is_direction_up,
    int min_fes_id_intern,
    int max_fes_id_intern,
    dnx_field_context_t context_id,
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    dnx_field_fes_id_t fes_id_currently_placed,
    int is_new,
    int min_fes_id,
    int max_fes_id,
    int min_in_array,
    int max_in_array,
    int allow_contention_moving,
    int allow_non_contention_moving,
    int allow_fes_pgm_sharing,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    int *found_allocation_p)
{
    dnx_field_fes_id_t fes_id_ndx;
    uint8 shareable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 allocable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 allocable_if_evacuated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    int from_here_cannot_move;
    dnx_field_fes_pgm_id_t fes_pgm_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_fes_mask_id_t
        fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
        [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    int push_succeeded;
    int found_evacuated_only_place;
    int move_succeeded;
    int min_fes_id_hard;
    int max_fes_id_hard;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(banned_fes_id, _SHR_E_PARAM, "banned_fes_id");
    SHR_NULL_CHECK(after_invalidate_next, _SHR_E_PARAM, "after_invalidate_next");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(found_allocation_p, _SHR_E_PARAM, "found_allocation_p");

    found_evacuated_only_place = FALSE;

    min_fes_id_hard = MAX(min_fes_id, min_in_array);
    max_fes_id_hard = MIN(max_fes_id, max_in_array);

    /*
     * Find first FES ID that is allocable without evacuation 
     * (or with evacuation if we evacuate by pushing the boundaries).
     */
    /** Loop initialization*/
    if (is_direction_up)
    {
        fes_id_ndx = MAX(max_fes_id_intern + 1, min_fes_id_intern);
    }
    else
    {
        fes_id_ndx = MIN(min_fes_id_intern - 1, max_fes_id_intern);
    }
    while (TRUE)
    {
        /** Loop condition.*/
        if (is_direction_up)
        {
            if (!(((signed int) fes_id_ndx) <= max_fes_id_hard && (*found_allocation_p) == FALSE))
            {
                break;
            }
        }
        else
        {
            /** We also add protection from wrap around.*/
            if (!(((signed int) fes_id_ndx) >= min_fes_id_hard && (*found_allocation_p) == FALSE &&
                  fes_id_ndx <= dnx_data_field.stage.stage_info_get(unit,
                                                                    field_stage)->nof_fes_instruction_per_context))
            {
                break;
            }
        }
        if (banned_fes_id[fes_id_ndx] == FALSE && after_invalidate_next[fes_id_ndx] == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_is_allocable
                            (unit, field_stage, current_state_changing_p, fes_id_ndx, fes_quartet_p,
                             allow_fes_pgm_sharing, TRUE, &shareable[fes_id_ndx], &allocable[fes_id_ndx],
                             &allocable_if_evacuated[fes_id_ndx], &fes_pgm_id[fes_id_ndx], fes_mask_id[fes_id_ndx]));
            if (allocable[fes_id_ndx] ||
                (allocable_if_evacuated[fes_id_ndx] &&
                 fes_id_ndx == (is_direction_up ? (max_fes_id_intern + 1) : (min_fes_id_intern - 1))))
            {
                /*
                 * Try to move the boundary.
                 */
                banned_fes_id[fes_id_ndx] = TRUE;
                if (is_direction_up)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                                    (unit, field_stage, context_id,
                                     banned_fes_id, after_invalidate_next,
                                     &(current_state_changing_p->fes_quartet_info[max_fes_id_intern + 1]),
                                     max_fes_id_intern + 1,
                                     fes_id_ndx + 1,
                                     dnx_data_field.stage.stage_info_get(unit,
                                                                         field_stage)->nof_fes_instruction_per_context,
                                     TRUE, allow_non_contention_moving, allow_fes_pgm_sharing,
                                     current_state_changing_p, alloc_result_p, &push_succeeded));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                                    (unit, field_stage, context_id,
                                     banned_fes_id, after_invalidate_next,
                                     &(current_state_changing_p->fes_quartet_info[min_fes_id_intern - 1]),
                                     min_fes_id_intern - 1,
                                     0, fes_id_ndx - 1,
                                     TRUE, allow_non_contention_moving, allow_fes_pgm_sharing,
                                     current_state_changing_p, alloc_result_p, &push_succeeded));
                }
                banned_fes_id[fes_id_ndx] = FALSE;
                if (push_succeeded)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                                    (unit, field_stage, context_id,
                                     fes_quartet_p, is_new, fes_id_currently_placed, fes_id_ndx,
                                     fes_pgm_id[fes_id_ndx], shareable[fes_id_ndx], fes_mask_id[fes_id_ndx],
                                     current_state_changing_p, alloc_result_p));
                    (*found_allocation_p) = TRUE;
                }
                else
                {
                    /*
                     * If we can't push the boundry under this condition, a stronger condition will fail.
                     */
                    break;
                }
            }
            else if (allocable_if_evacuated[fes_id_ndx])
            {
                found_evacuated_only_place = TRUE;
            }
        }
        /** Loop increment.*/
        if (is_direction_up)
        {
            fes_id_ndx++;
        }
        else
        {
            if (fes_id_ndx == 0)
            {
                break;
            }
            fes_id_ndx--;
        }
    }
    if ((*found_allocation_p == FALSE) && found_evacuated_only_place && allow_contention_moving)
    {
        /*
         * Find The furthest push possible, and then try to replace actions. 
         */
        from_here_cannot_move = fes_id_ndx;
        push_succeeded = FALSE;
        /** Loop initialization*/
        if (is_direction_up)
        {
            fes_id_ndx = MIN(from_here_cannot_move, max_fes_id_hard);
        }
        else
        {
            fes_id_ndx = MAX(from_here_cannot_move, min_fes_id_hard);
        }
        while (TRUE)
        {
            /** Loop condition.*/
            if (is_direction_up)
            {
                if (!(push_succeeded == FALSE && (signed int) fes_id_ndx >= max_fes_id_intern))
                {
                    break;
                }
            }
            else
            {
                if (!(push_succeeded == FALSE && (signed int) fes_id_ndx <= min_fes_id_intern))
                {
                    break;
                }
            }
            if (allocable_if_evacuated[fes_id_ndx])
            {
                /*
                 * Try to move the boundary.
                 */
                banned_fes_id[fes_id_ndx] = TRUE;
                if (is_direction_up)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                                    (unit, field_stage, context_id,
                                     banned_fes_id, after_invalidate_next,
                                     &(current_state_changing_p->fes_quartet_info[max_fes_id_intern + 1]),
                                     max_fes_id_intern + 1,
                                     fes_id_ndx + 1,
                                     dnx_data_field.stage.stage_info_get(unit,
                                                                         field_stage)->nof_fes_instruction_per_context,
                                     TRUE, allow_non_contention_moving, allow_fes_pgm_sharing,
                                     current_state_changing_p, alloc_result_p, &push_succeeded));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                                    (unit, field_stage, context_id,
                                     banned_fes_id, after_invalidate_next,
                                     &(current_state_changing_p->fes_quartet_info[min_fes_id_intern - 1]),
                                     min_fes_id_intern - 1,
                                     0, fes_id_ndx - 1,
                                     TRUE, allow_non_contention_moving, allow_fes_pgm_sharing,
                                     current_state_changing_p, alloc_result_p, &push_succeeded));
                }
                banned_fes_id[fes_id_ndx] = FALSE;
            }
            /** Loop increment.*/
            if (is_direction_up)
            {
                if (fes_id_ndx == 0)
                {
                    break;
                }
                fes_id_ndx--;
            }
            else
            {
                fes_id_ndx++;
            }
        }
        /*
         * If succedded in pushing the boundaries, try and replace 
         */
        if (push_succeeded)
        {
            /** Loop initialization*/
            if (is_direction_up)
            {
                fes_id_ndx = MAX(max_fes_id_intern + 1, min_fes_id_intern);
            }
            else
            {
                fes_id_ndx = MIN(min_fes_id_intern - 1, max_fes_id_intern);
            }
            while (TRUE)
            {
                /** Loop condition.*/
                if (is_direction_up)
                {
                    if (!(((signed int) fes_id_ndx) <= max_fes_id_hard && (*found_allocation_p) == FALSE))
                    {
                        break;
                    }
                }
                else
                {
                    if (!(((signed int) fes_id_ndx) >= min_fes_id_hard && (*found_allocation_p) == FALSE))
                    {
                        break;
                    }
                }

                if (banned_fes_id[fes_id_ndx] == FALSE && after_invalidate_next[fes_id_ndx] == FALSE
                    && allocable[fes_id_ndx] == FALSE && allocable_if_evacuated[fes_id_ndx])
                {
                    /*
                     * Try to move the FES quartet in the FES ID.
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                                    (unit, field_stage, context_id,
                                     banned_fes_id, after_invalidate_next,
                                     &(current_state_changing_p->fes_quartet_info[fes_id_ndx]),
                                     fes_id_ndx,
                                     0, dnx_data_field.stage.stage_info_get(unit,
                                                                            field_stage)->nof_fes_instruction_per_context,
                                     FALSE, FALSE, allow_fes_pgm_sharing, current_state_changing_p, alloc_result_p,
                                     &move_succeeded));
                    if (move_succeeded)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                                        (unit, field_stage, context_id,
                                         fes_quartet_p, is_new, fes_id_currently_placed, fes_id_ndx,
                                         fes_pgm_id[fes_id_ndx], shareable[fes_id_ndx], fes_mask_id[fes_id_ndx],
                                         current_state_changing_p, alloc_result_p));
                        (*found_allocation_p) = TRUE;
                    }
                }

                /** Loop increment.*/
                if (is_direction_up)
                {
                    fes_id_ndx++;
                }
                else
                {
                    if (fes_id_ndx == 0)
                    {
                        break;
                    }
                    fes_id_ndx--;
                }
            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  A Function for allocation a place for a FES quartet with a non mandatory field group, whether new or movement.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] context_id -
 *  The context ID.
 * \param [in] banned_fes_id -
 *  FES IDs not to allocate. intended to be used for recursive allocation, which we dont do at the moment
 *  so ther should be only one banned FES ID at the time (when moving contention FES quartets).
 * \param [in] after_invalidate_next -
 *  FES IDs coming after invalidate next cannot be allocated to non mandatory position FES quartet priorities.
 * \param [in] fes_quartet_p -
 *  The information about the FES quartet that we allocate for.
 * \param [in] fes_id_currently_placed -
 *  If the is a movement, the FES ID we move from. If the FES quartet is new, we expect it to be the number of FES IDs.
 * \param [in] min_fes_id -
 *  The minimal FES ID to allocate.
 *  Should be at most 0 if this is a new FES quartet.
 * \param [in] max_fes_id -
 *  The maximal FES ID to allocate.
 *  Should be at least the last FES ID if this is a new FES quartet.
 * \param [in] allow_contention_moving -
 *  Whether we allow pushing actions with contention. blocks recursion.
 * \param [in] allow_non_contention_moving -
 *  Whether we allow moving away actions without any contention.
 * \param [in] allow_fes_pgm_sharing -
 *  Whether we allow FES program ID sharing. We disallow it for direct extraction.
 * \param [in,out] current_state_changing_p -
 *  The current state of the allocationin the context. Updated with the allocation.
 * \param [in,out] alloc_result_p -
 *   To be loaded/updated with the new allocation/movement.
 * \param [out] found_allocation_p -
 *   Whether the function was successful in finding a legal allocation.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 *   * dnx_algo_field_action_fes_allocate_mandatory_position()
 *   * Fes allocation description on ALGORITHM DESCRIPTIONS section in algo_field_action.c.
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_non_mandatory_position(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    dnx_field_fes_id_t fes_id_currently_placed,
    int min_fes_id,
    int max_fes_id,
    int allow_contention_moving,
    int allow_non_contention_moving,
    int allow_fes_pgm_sharing,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    int *found_allocation_p)
{
    int min_fes_id_intern;
    int max_fes_id_intern;
    dnx_field_fes_id_t fes_id_order[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes_id_order_ndx;
    uint8 contention[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_algo_field_action_fes_alloc_priority_compare_result_e compare_result;
    uint8 shareable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 allocable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 allocable_if_evacuated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    unsigned int nof_fes_id_order;
    int min_in_array;
    int max_in_array;
    dnx_field_fes_pgm_id_t fes_pgm_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_fes_mask_id_t
        fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
        [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    int is_new;
    int move_succeeded;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(banned_fes_id, _SHR_E_PARAM, "banned_fes_id");
    SHR_NULL_CHECK(after_invalidate_next, _SHR_E_PARAM, "after_invalidate_next");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");
    SHR_NULL_CHECK(found_allocation_p, _SHR_E_PARAM, "found_allocation_p");

    /*
     * Initializations to apease the compiler.
     */
    nof_fes_id_order = 0;
    compare_result = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_NOF;

    is_new =
        (fes_id_currently_placed >=
         dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context);

    (*found_allocation_p) = FALSE;

    /*
     * Check contention against existing FES quartets.
     * Note that for a new FES quartet we should have min_fes_id=0 max_fes_id=nof_fes_ids for the verification.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_contention_find
                    (unit, field_stage, context_id, current_state_changing_p, fes_id_currently_placed,
                     min_fes_id, max_fes_id, fes_quartet_p, contention));

    min_fes_id_intern = min_fes_id;
    max_fes_id_intern = max_fes_id;

    /*
     * Unless the priority is "don't care" limit the range to the array.
     */
    if (fes_quartet_p->priority != BCM_FIELD_ACTION_DONT_CARE)
    {
        min_in_array =
            (DNX_FIELD_ACTION_PRIORITY_ARRAY(fes_quartet_p->priority) / 2) *
            dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_id_per_array;
        max_in_array = min_in_array + dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_id_per_array - 1;
    }
    else
    {
        min_in_array = 0;
        max_in_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context - 1;
    }
    if (min_fes_id_intern < min_in_array)
    {
        min_fes_id_intern = min_in_array;
    }
    if (max_fes_id_intern > max_in_array)
    {
        max_fes_id_intern = max_in_array;
    }

    if (min_fes_id > max_in_array || max_fes_id < min_in_array)
    {
        SHR_EXIT();
    }

    /*
     * Find the min and max fes IDs given priorities and contention.
     */
    if (fes_quartet_p->priority != BCM_FIELD_ACTION_DONT_CARE)
    {
        for (fes_id_ndx = 0;
             fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
             fes_id_ndx++)
        {
            if (fes_id_ndx == fes_id_currently_placed)
            {
                continue;
            }
            /*
             * We tested earlier that all priorities with contention don't have mandatory placement or 
             * "don't care" priority. However, we will still check for both for the case that we may later want to 
             * change  dnx_algo_field_action_fes_allocate_contention_priorities_allowed.
             */
            if (contention[fes_id_ndx] &&
                (current_state_changing_p->fes_quartet_info[fes_id_ndx].priority != BCM_FIELD_ACTION_DONT_CARE) &&
                (FALSE ==
                 DNX_FIELD_ACTION_PRIORITY_IS_POSITION(current_state_changing_p->fes_quartet_info
                                                       [fes_id_ndx].priority)))
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_priority_compare
                                (unit,
                                 fes_quartet_p->priority, fes_quartet_p->fg_id, fes_quartet_p->place_in_fg,
                                 current_state_changing_p->fes_quartet_info[fes_id_ndx].priority,
                                 current_state_changing_p->fes_quartet_info[fes_id_ndx].fg_id,
                                 current_state_changing_p->fes_quartet_info[fes_id_ndx].place_in_fg, &compare_result));
                if ((compare_result == DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS) &&
                    (min_fes_id_intern <= (signed int) fes_id_ndx))
                {
                    min_fes_id_intern = fes_id_ndx + 1;
                }
                if ((compare_result == DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS) &&
                    (max_fes_id_intern >= (signed int) fes_id_ndx))
                {
                    max_fes_id_intern = fes_id_ndx - 1;
                }
                /** Sanity check*/
                if (compare_result == DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_NOF ||
                    compare_result == DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_INCOMPARABLE)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Priority comparison not performed. \r\n");
                }
            }
        }
    }

    /*
     * Find the allocation order without pushing the boundaries of the min/max FES ID.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_order
                    (unit, field_stage, banned_fes_id, after_invalidate_next,
                     fes_quartet_p, fes_id_currently_placed, min_fes_id_intern, max_fes_id_intern,
                     allow_fes_pgm_sharing, current_state_changing_p,
                     fes_id_order, &nof_fes_id_order, shareable, allocable, allocable_if_evacuated,
                     fes_pgm_id, fes_mask_id));

    /*
     * Begin the allocation process.
     */
    for (fes_id_order_ndx = 0; fes_id_order_ndx < nof_fes_id_order && (*found_allocation_p) == FALSE;
         fes_id_order_ndx++)
    {
        fes_id_ndx = fes_id_order[fes_id_order_ndx];
        (*found_allocation_p) = TRUE;
        if (allocable[fes_id_ndx])
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                            (unit, field_stage, context_id,
                             fes_quartet_p, is_new, fes_id_currently_placed, fes_id_ndx,
                             fes_pgm_id[fes_id_ndx], shareable[fes_id_ndx], fes_mask_id[fes_id_ndx],
                             current_state_changing_p, alloc_result_p));
        }
        else if (allocable_if_evacuated[fes_id_ndx] && allow_non_contention_moving)
        {
            /*
             * Try to move the FES quartet in the FES ID.
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                            (unit, field_stage, context_id,
                             banned_fes_id, after_invalidate_next,
                             &(current_state_changing_p->fes_quartet_info[fes_id_ndx]),
                             fes_id_ndx,
                             0, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             FALSE, FALSE, allow_fes_pgm_sharing,
                             current_state_changing_p, alloc_result_p, &move_succeeded));
            if (move_succeeded)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                                (unit, field_stage, context_id,
                                 fes_quartet_p, is_new, fes_id_currently_placed, fes_id_ndx,
                                 fes_pgm_id[fes_id_ndx], shareable[fes_id_ndx], fes_mask_id[fes_id_ndx],
                                 current_state_changing_p, alloc_result_p));
            }
            else
            {
                (*found_allocation_p) = FALSE;
            }
        }
        else
        {
            (*found_allocation_p) = FALSE;
        }
    }

    /*
     * Sanity check
     */
    if ((*found_allocation_p) == TRUE && alloc_result_p == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation for allocable FES ID failed. \r\n");
    }

    /*
     * Try to push the boundaries aside.
     */
    if (allow_contention_moving)
    {
        /*
         * First try to push higher.
         */
        if ((*found_allocation_p) == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position_push
                            (unit, field_stage, TRUE, min_fes_id_intern, max_fes_id_intern,
                             context_id, banned_fes_id, after_invalidate_next,
                             fes_quartet_p, fes_id_currently_placed,
                             is_new, min_fes_id, max_fes_id, min_in_array, max_in_array,
                             allow_contention_moving, allow_non_contention_moving, allow_fes_pgm_sharing,
                             current_state_changing_p, alloc_result_p, found_allocation_p));
        }
        /*
         * Then try to push lower.
         */
        if ((*found_allocation_p) == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position_push
                            (unit, field_stage, FALSE, min_fes_id_intern, max_fes_id_intern,
                             context_id, banned_fes_id, after_invalidate_next,
                             fes_quartet_p, fes_id_currently_placed,
                             is_new, min_fes_id, max_fes_id, min_in_array, max_in_array,
                             allow_contention_moving, allow_non_contention_moving, allow_fes_pgm_sharing,
                             current_state_changing_p, alloc_result_p, found_allocation_p));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This procedure allocates a placement for a FES quartet with mandatory placement.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] context_id -
 *  The context ID.
 * \param [in] banned_fes_id -
 *  FES IDs not to allocate. None should be banned for this function at the moment, as we do not perform any 
 *  contention moving.
 * \param [in] after_invalidate_next -
 *  FES IDs coming after invalidate next cannot be allocated to non mandatory position FES quartet priorities.
 *  Relevent for when clearing a space for the mandatory placement FES quartet.
 * \param [in] fes_quartet_p -
 *  The information about the FES quartet that we allocate for.
 * \param [in] allow_fes_pgm_sharing -
 *  Whether we allow FES program ID sharing. We disallow it for direct extraction.
 * \param [in,out] current_state_changing_p -
 *  The current state of the allocationin the context. Updated with the allocation.
 * \param [in,out] alloc_result_p -
 *   To be loaded/updated with the new allocation/movement.
 * \param [out] found_allocation_p -
 *   Whether the function was successful in finding a legal allocation.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 *   * Fes allocation description on ALGORITHM DESCRIPTIONS section in algo_field_action.c.
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_mandatory_position(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    int allow_fes_pgm_sharing,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    int *found_allocation_p)
{
    uint8 contention[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_fes_id_t requested_fes_id;
    uint8 shareable;
    uint8 allocable;
    uint8 allocable_if_evacuated;
    dnx_field_fes_pgm_id_t fes_pgm_id;
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    int move_succeeded;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(banned_fes_id, _SHR_E_PARAM, "banned_fes_id");
    SHR_NULL_CHECK(after_invalidate_next, _SHR_E_PARAM, "after_invalidate_next");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");
    SHR_NULL_CHECK(found_allocation_p, _SHR_E_PARAM, "found_allocation_p");

    /*
     * Check contention against existing FES quartets.
     * Note that this is done only for verification.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_contention_find
                    (unit, field_stage, context_id, current_state_changing_p,
                     dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                     0, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context - 1,
                     fes_quartet_p, contention));

    /*
     * Get the FES ID from the priority.
     */
    DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(requested_fes_id, unit, field_stage, fes_quartet_p->priority);

    /*
     * Try to allocate.
     */
    (*found_allocation_p) = FALSE;
    if (banned_fes_id[requested_fes_id] == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_is_allocable
                        (unit, field_stage, current_state_changing_p, requested_fes_id, fes_quartet_p,
                         allow_fes_pgm_sharing, TRUE, &shareable, &allocable, &allocable_if_evacuated,
                         &fes_pgm_id, fes_mask_id));
        (*found_allocation_p) = TRUE;
        if (allocable)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                            (unit, field_stage, context_id,
                             fes_quartet_p,
                             TRUE, dnx_data_field.stage.stage_info_get(unit,
                                                                       field_stage)->nof_fes_instruction_per_context,
                             requested_fes_id, fes_pgm_id, shareable, fes_mask_id, current_state_changing_p,
                             alloc_result_p));
        }
        else if (allocable_if_evacuated)
        {
            /*
             * Try to move the FES quartet in the FES ID.
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                            (unit, field_stage, context_id,
                             banned_fes_id, after_invalidate_next,
                             &(current_state_changing_p->fes_quartet_info[requested_fes_id]),
                             requested_fes_id,
                             0, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             TRUE, TRUE, allow_fes_pgm_sharing,
                             current_state_changing_p, alloc_result_p, &move_succeeded));
            if (move_succeeded)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                                (unit, field_stage, context_id,
                                 fes_quartet_p,
                                 TRUE, dnx_data_field.stage.stage_info_get(unit,
                                                                           field_stage)->nof_fes_instruction_per_context,
                                 requested_fes_id, fes_pgm_id, shareable, fes_mask_id, current_state_changing_p,
                                 alloc_result_p));
            }
            else
            {
                (*found_allocation_p) = FALSE;
            }
        }
        else
        {
            (*found_allocation_p) = FALSE;
        }
    }
    else
    {
        /*
         * Sanity check
         */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached mandatory placement with a banned FES ID, shouldn't happen.\r\n");
    }

    /*
     * Sanity check
     */
    if ((*found_allocation_p) == TRUE && alloc_result_p == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation for allocable FES ID failed. \r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify parameters of dnx_algo_field_action_fes_allocate().
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] fg_id -
 *  HW identifier of field group.
 * \param [in] context_id -
 *  The context_id.
 * \param [in] current_state_in_p -
 *  The current state of the allocation given to the allocation algorithm from the outside
 *  (as opposed to the current state in algo field SW state).
 *  At the moment contains the allocated FES quartets to the context ID we allocate for.
 * \param [in] fes_fg_in_p -
 *   The information about the new field group and context ID and what allocations they require.
 * \param [in] post_attach -
 *   If we add more EFES after context attach.
 * \param [in] alloc_result_p -
 *   Pointer to the memory to be loaded with the result of the allocation.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_allocate()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_fes_alloc_in_t * fes_fg_in_p,
    int post_attach,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p)
{
    unsigned int action_ndx;
    unsigned int action_ndx_2;
    dnx_field_fes_id_t fes_id_priority;
    dnx_field_action_priority_t priority;
    dnx_field_action_priority_t priority_2;
    unsigned int fes_id_ndx;
    unsigned int num_existing_fes_quartets;
    unsigned int fes2msb_ndx;
    unsigned int fes2msb_2_ndx;
    unsigned int nof_masks;
    int mask_repeats;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_in_p, _SHR_E_PARAM, "current_state_in_p");
    SHR_NULL_CHECK(fes_fg_in_p, _SHR_E_PARAM, "fes_fg_in_p");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");

    /*
     * Verify that the stage has FESes.
     */
    if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage \"%s\" (%d). \r\n",
                     DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), field_stage);
    }
    /*
     * Verify that the field group isn't invalid
     */
    if (fg_id == DNX_FIELD_GROUP_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid field group id (%d) received. Context ID is %d.\r\n", fg_id, context_id);
    }
    /*
     * Verify that the context ID is within range.
     */
    if (context_id > dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context ID (%d) cannot exceed %d.\r\n",
                     context_id, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts - 1);
    }
    /*
     * Verify that the number of FES quartets doesn't exceed the number of FESes.
     * Also verify that the number of FES quartets doesn't exceed the maximum number of actions per FG.
     */
    if (fes_fg_in_p->nof_fes_quartets >
        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_fes_quartets (%d) exceeds the maximum number of FESes for the stage (%d).\r\n",
                     fes_fg_in_p->nof_fes_quartets, dnx_data_field.stage.stage_info_get(unit,
                                                                                        field_stage)->nof_fes_instruction_per_context);
    }
    if (fes_fg_in_p->nof_fes_quartets > DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_fes_quartets (%d) exceeds the maximum number of actions per field group (%d).\r\n",
                     fes_fg_in_p->nof_fes_quartets, DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG);
    }
    if (fes_fg_in_p->nof_fes_quartets <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_fes_quartets (%d) 0 is not supported at the moment. Field group %d, context ID %d.\r\n",
                     fes_fg_in_p->nof_fes_quartets, fg_id, context_id);
    }
    /*
     * Count the number of existing FES quartets.
     */
    num_existing_fes_quartets = 0;
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        if (current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            /*
             * Sanity check: Verify that we don't have Allocated 
             */
            if (fes_id_ndx >= dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "FES ID %d is marked as allocated, even though there are only %d FES IDs "
                             "in stage %d.\r\n",
                             fes_id_ndx,
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             field_stage);
            }
            num_existing_fes_quartets++;
        }
    }
    /*
     * Verify that the number of FES quartets to be added plus the existing FES quartets does not exceed the maximum 
     * for the stage.
     */
    if (fes_fg_in_p->nof_fes_quartets + num_existing_fes_quartets >
        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "nof_fes_quartets (%d) plus existing fes quartets in context ID %d (%d) is %d, "
                     " exceeds the maximum number of FESes for the stage (%d).\r\n",
                     fes_fg_in_p->nof_fes_quartets, context_id, num_existing_fes_quartets,
                     fes_fg_in_p->nof_fes_quartets + num_existing_fes_quartets,
                     dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context);
    }
    /*
     * Verify that the number of distinct non zero required masks doesn't exceed the number of masks per FES minus 1 (for zero mask).
     */
    for (action_ndx = 0; action_ndx < fes_fg_in_p->nof_fes_quartets; action_ndx++)
    {
        nof_masks = 0;
        for (fes2msb_ndx = 0; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            if (fes_fg_in_p->fes_quartet[action_ndx].fes_mask_id[fes2msb_ndx] != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                mask_repeats = FALSE;
                for (fes2msb_2_ndx = 0; fes2msb_2_ndx < fes2msb_ndx; fes2msb_2_ndx++)
                {
                    if (fes_fg_in_p->fes_quartet[action_ndx].fes_mask_id[fes2msb_ndx] ==
                        fes_fg_in_p->fes_quartet[action_ndx].fes_mask_id[fes2msb_2_ndx])
                    {
                        mask_repeats = TRUE;
                        break;
                    }
                }
                if (mask_repeats == FALSE)
                {
                    nof_masks++;
                }
            }
        }
        if (nof_masks > dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes - 1)
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Cannot allocate more than %d action masks per FES quartet.\r\n",
                         dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes - 1);
    }
    /*
     * Verify that the priorities are within bounds.
     */
    for (action_ndx = 0; action_ndx < fes_fg_in_p->nof_fes_quartets; action_ndx++)
    {
        priority = fes_fg_in_p->fes_quartet[action_ndx].priority;
        DNX_FIELD_ACTION_PRIORITY_VERIFY(priority);
        /*
         * Checking if the priority is invalid.
         */
        if (priority == DNX_FIELD_ACTION_PRIORITY_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Priority 0x%04x is invalid!\r\n", priority);
        }
        /*
         * If the priority has a mandatory position, check that it points to a valid FES ID.
         */
        if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(priority))
        {
            DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id_priority, unit, field_stage, priority)
                /*
                 * Verify that the FES ID is within range
                 */
                if (fes_id_priority >=
                    dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Priority 0x%08x points to FES ID %d, "
                             "but there are only %d FESes in stage %s.\r\n",
                             priority,
                             fes_id_priority,
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
            }
        }
        /*
         * If the priority isn't don't care, verify that it points to a legal FES array.
         */
        if (priority != BCM_FIELD_ACTION_DONT_CARE)
        {
            /*
             * Verify that the FES array exists in the stage
             */
            if (DNX_FIELD_ACTION_PRIORITY_ARRAY(priority) >=
                dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array * 2)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Priority 0x%04x points to FES array %d, "
                             "but there are only %d FES arrays in stage %s.\r\n",
                             priority,
                             DNX_FIELD_ACTION_PRIORITY_ARRAY(priority),
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array,
                             DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
            }
            if (DNX_FIELD_ACTION_PRIORITY_ARRAY(priority) % 2 != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Priority 0x%04x points to array %d, "
                             "which is not a FEM array in stage %s.\r\n",
                             priority,
                             DNX_FIELD_ACTION_PRIORITY_ARRAY(priority),
                             DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
            }
        }
    }
    /*
     * If we have two actions with position priority, provide an explicit error.
     */
    for (action_ndx = 0; action_ndx < fes_fg_in_p->nof_fes_quartets; action_ndx++)
    {
        priority = fes_fg_in_p->fes_quartet[action_ndx].priority;
        if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(priority))
        {
            /*
             * Check if there isn't an existing action with such a priority. 
             */
            DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id_priority, unit, field_stage, priority)
                if (current_state_in_p->context_state.fes_id_info[fes_id_priority].fes_pgm_id !=
                    DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                switch (field_stage)
                {
                    case DNX_FIELD_STAGE_IPMF1:
                    case DNX_FIELD_STAGE_IPMF2:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                        priority.get(unit, context_id, fes_id_priority, &priority_2));
                        break;
                    case DNX_FIELD_STAGE_IPMF3:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                        priority.get(unit, context_id, fes_id_priority, &priority_2));
                        break;
                    case DNX_FIELD_STAGE_EPMF:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                        priority.get(unit, context_id, fes_id_priority, &priority_2));
                        break;
                    default:
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
                }
                if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(priority_2))
                {
                    if (priority != priority_2)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Two position pirorities 0x%x and 0x%x on the same EFES "
                                     "ID %d. \r\n", priority, priority_2, fes_id_priority);
                    }
                    SHR_ERR_EXIT(_SHR_E_PARAM, "The position priority 0x%x was provided twice on context %d "
                                 "stage \"%s\". \r\n", priority, context_id,
                                 DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
                }
            }
            /*
             * Check if there isn't another action with the same position to be allocated.
             */
            for (action_ndx_2 = 0; action_ndx_2 < action_ndx; action_ndx_2++)
            {
                priority_2 = fes_fg_in_p->fes_quartet[action_ndx_2].priority;
                if (priority == priority_2)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "The position priority 0x%x was provided twice on context %d "
                                 "stage \"%s\". \r\n", priority, context_id,
                                 DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
                }
            }
        }
    }

    /*
     * Post attach, we only allow adding one EFES at a time
     */
    if (post_attach && (fes_fg_in_p->nof_fes_quartets > 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Post attach, we only support adding one EFES at a time. nof_fes_quartets is %d. "
                     "Field group %d, context ID %d.\r\n", fes_fg_in_p->nof_fes_quartets, fg_id, context_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_action_fes_allocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_fes_alloc_in_t * fes_fg_in_p,
    int allow_fes_pgm_sharing,
    int post_attach,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p)
{
    int found_allocation;
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t current_state_changing;
    unsigned int first_place_in_fg;
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 after_invalidate_next_new[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_algo_field_action_fes_alloc_fes_quartet_t fes_quartet[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    unsigned int fes_quartet_ndx;
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes_change_ndx;
    unsigned int nof_invalidated_quartets;
    unsigned int nof_allocation_holes;
    int last_new_fes_change_ndx;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify Input parameters.
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_algo_field_action_fes_allocate_verify
                           (unit, field_stage, fg_id, context_id, current_state_in_p, fes_fg_in_p, post_attach,
                            alloc_result_p));

    /*
     * Collect current state information.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_collect_current_state_info
                    (unit, field_stage, fg_id, context_id, post_attach, current_state_in_p,
                     &current_state_changing, after_invalidate_next, &first_place_in_fg));
    /*
     * Translate New FES quartets information
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_collect_new_actions_info
                    (unit, field_stage, fg_id, context_id, fes_fg_in_p, first_place_in_fg,
                     after_invalidate_next, after_invalidate_next_new, fes_quartet));

    /*
     * Initialize alloc_result_p.
     */
    alloc_result_p->nof_fes_quartet_changes = 0;
    /*
     * Initialize banned_fes_id
     */
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        banned_fes_id[fes_id_ndx] = FALSE;
    }

    /*
     * Move FES quartets that without mandatory placement that after allocation will be after invalidate next.
     */
    found_allocation = FALSE;
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if ((after_invalidate_next_new[fes_id_ndx]) &&
            (current_state_changing.fes_pgm_id[fes_id_ndx] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM) &&
            (DNX_FIELD_ACTION_PRIORITY_IS_POSITION
             (current_state_changing.fes_quartet_info[fes_id_ndx].priority) == FALSE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                            (unit, field_stage, context_id,
                             banned_fes_id, after_invalidate_next,
                             &(current_state_changing.fes_quartet_info[fes_id_ndx]),
                             fes_id_ndx, 0, dnx_data_field.stage.stage_info_get(unit,
                                                                                field_stage)->nof_fes_instruction_per_context,
                             TRUE, TRUE, allow_fes_pgm_sharing, &current_state_changing, alloc_result_p,
                             &found_allocation));

            if (found_allocation == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE,
                             "Failed to move aside existing FES instructions in field group (%d), context ID (%d), "
                             "action %d with non mandatory placement (priority 0x%x) from FES ID %d, to clear "
                             "the FES ID after a new invalidate next.\r\n",
                             fg_id, context_id, current_state_changing.fes_quartet_info[fes_id_ndx].place_in_fg,
                             current_state_changing.fes_quartet_info[fes_id_ndx].priority, fes_id_ndx);
            }
        }
    }

    /*
     * Find correct allocation for each new action.
     */
    nof_invalidated_quartets = 0;
    found_allocation = FALSE;
    for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
    {
        if (fes_quartet[fes_quartet_ndx].priority == BCM_FIELD_ACTION_INVALIDATE)
        {
            nof_invalidated_quartets++;
            continue;
        }
        if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(fes_quartet[fes_quartet_ndx].priority))
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_mandatory_position
                            (unit, field_stage, context_id, banned_fes_id, after_invalidate_next,
                             &fes_quartet[fes_quartet_ndx], allow_fes_pgm_sharing,
                             &current_state_changing, alloc_result_p, &found_allocation));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                            (unit, field_stage, context_id, banned_fes_id,
                             after_invalidate_next, &fes_quartet[fes_quartet_ndx],
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             0, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             TRUE, TRUE, allow_fes_pgm_sharing,
                             &current_state_changing, alloc_result_p, &found_allocation));
        }

        if (found_allocation == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                         "\r\n"
                         "Failed to allocate FES instructions for field group (%d) context ID (%d). "
                         "Failed allocating action %d of the field group.\r\n",
                         fg_id, context_id, fes_quartet[fes_quartet_ndx].place_in_fg);
        }
    }

    /*
     * Sanity check: Verify that the number of new FES quartets in alloc_result_p is as requested, and that they
     *               are in the correct order.
     */
    last_new_fes_change_ndx = -1;
    for (fes_change_ndx = 0, fes_quartet_ndx = 0, nof_allocation_holes = 0;
         fes_change_ndx < alloc_result_p->nof_fes_quartet_changes; fes_change_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc != DNX_ALGO_FIELD_ACTION_MOVEMENT)
        {
            if (alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Negative 'place_in_alloc' that isn't movement found "
                             "(value %d fes quartet change %d).\r\n",
                             alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc, fes_change_ndx);
            }
            if (fes_quartet_ndx == 0)
            {
                nof_allocation_holes += alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc;
            }
            else
            {
                if (alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc <=
                    alloc_result_p->fes_quartet_change[last_new_fes_change_ndx].place_in_alloc)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Non consecutive 'place_in_alloc' found (value %d fes quartet change %d "
                                 "action %d previous %d).\r\n",
                                 alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc,
                                 fes_change_ndx, fes_quartet_ndx, last_new_fes_change_ndx);
                }
                else
                {
                    nof_allocation_holes +=
                        alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc -
                        alloc_result_p->fes_quartet_change[last_new_fes_change_ndx].place_in_alloc - 1;
                }
            }
            last_new_fes_change_ndx = fes_change_ndx;
            fes_quartet_ndx++;
        }
    }
    if (nof_allocation_holes > nof_invalidated_quartets)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "FES allocation algorithm has %d allocation holes, but only %d invalidate priority quartets.\r\n",
                     nof_allocation_holes, nof_invalidated_quartets);
    }
    if (fes_quartet_ndx + nof_invalidated_quartets != fes_fg_in_p->nof_fes_quartets)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "FES allocation algorithm should have allocated %d FES quartets, allocated %d.\r\n",
                     fes_fg_in_p->nof_fes_quartets, fes_quartet_ndx);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Deletes from SW state what was deallocated by dnx_algo_field_action_fes_dealloc().
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] context_id -
 *    The context ID that we remove the field group from.
 * \param [in] dealloc_result_p -
 *   A pointer to s a structure indicating what is to be deallocated.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_dealloc()
 */
shr_error_e
dnx_algo_field_action_fes_dealloc_sw_state(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_dealloc_out_t * dealloc_result_p)
{
    unsigned int fes_id_ndx;
    unsigned int fes_pgm_id;
    unsigned int mask_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dealloc_result_p, _SHR_E_PARAM, "dealloc_result_p");

    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if (dealloc_result_p->belongs_to_fg[fes_id_ndx])
        {
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                case DNX_FIELD_STAGE_IPMF2:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_id, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                    break;
                case DNX_FIELD_STAGE_IPMF3:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_id, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                    break;
                case DNX_FIELD_STAGE_EPMF:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_id, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
            }
            /*
             * FES program ID of zero FES program marks not to delete. All other FES program IDs indicate
             * what FES program ID to delete.
             */
            fes_pgm_id = dealloc_result_p->fes_pgm_id_to_delete[fes_id_ndx];
            if (fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                /*
                 * Mark the FES quartet as invalid and initialize its SW state for predictability.
                 */
                switch (field_stage)
                {
                    case DNX_FIELD_STAGE_IPMF1:
                    case DNX_FIELD_STAGE_IPMF2:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                                        field_group.set(unit, fes_id_ndx, fes_pgm_id, DNX_FIELD_GROUP_INVALID));
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                                        place_in_fg.set(unit, fes_id_ndx, fes_pgm_id, 0));
                        break;
                    case DNX_FIELD_STAGE_IPMF3:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                        field_group.set(unit, fes_id_ndx, fes_pgm_id, DNX_FIELD_GROUP_INVALID));
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                        place_in_fg.set(unit, fes_id_ndx, fes_pgm_id, 0));
                        break;
                    case DNX_FIELD_STAGE_EPMF:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                                        field_group.set(unit, fes_id_ndx, fes_pgm_id, DNX_FIELD_GROUP_INVALID));
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                                        place_in_fg.set(unit, fes_id_ndx, fes_pgm_id, 0));
                        break;
                        /*
                         * coverity explanation: There is same switch case in the beginning of the function,
                         * which is covering the same stages like here, that causes the coverity.
                         * Leaving the default for the case where the other switch case is changed.
                         */
                         /* coverity[dead_error_begin:FALSE]  */
                    default:
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
                }
                /*
                 * Delete the masks to be deallocated from SW state.
                 * Note that we only delete masks if we deleted a FES quartet in the same FES ID.
                 */
                for (mask_ndx = 0; mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
                     mask_ndx++)
                {
                    /*
                     * check if the mask is to be deleted
                     */
                    if (dealloc_result_p->delete_mask[fes_id_ndx][mask_ndx])
                    {
                        /*
                         * Sanity check.
                         */
                        if (mask_ndx == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "Zero action mask marked for deletion for context ID %d, FES ID %d. \r\n",
                                         context_id, fes_id_ndx);
                        }
                        /*
                         * Delete mask allocation from SW state
                         */
                        switch (field_stage)
                        {
                            case DNX_FIELD_STAGE_IPMF1:
                            case DNX_FIELD_STAGE_IPMF2:
                                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                                                mask_is_alloc.set(unit, fes_id_ndx, mask_ndx, FALSE));
                                break;
                            case DNX_FIELD_STAGE_IPMF3:
                                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                                mask_is_alloc.set(unit, fes_id_ndx, mask_ndx, FALSE));
                                break;
                            case DNX_FIELD_STAGE_EPMF:
                                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                                                mask_is_alloc.set(unit, fes_id_ndx, mask_ndx, FALSE));
                                break;
                                /*
                                 * coverity explanation: There is same switch case in the beginning of the function,
                                 * which is covering the same stages like here, that causes the coverity.
                                 * Leaving the default for the case where the other switch case is changed.
                                 */
                                 /* coverity[dead_error_begin:FALSE]  */
                            default:
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
                            }
                        }
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify parameters of dnx_algo_field_action_fes_dealloc().
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for.
 * \param [in] fg_id -
 *    HW identifier of field group to remove.
 * \param [in] context_id -
 *    The context ID that we remove the field group from.
 * \param [in] current_state_dealloc_in_p -
 *  The current state of the allocation given to the allocation algorithm from the outside
 *  (as opposed to the current state in algo field SW state).
 *  At the moment contains the allocated FES quartets to the context ID we allocate for,
 *  and the number of context IDs that are allocated each FES quartet in each FES.
 * \param [in] dealloc_result_p -
 *   Pointer to the memory to be loaded with the result of the allocation.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_dealloc()
 */
shr_error_e
dnx_algo_field_action_fes_dealloc_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_dealloc_state_in_t * current_state_dealloc_in_p,
    dnx_algo_field_action_fes_dealloc_out_t * dealloc_result_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_dealloc_in_p, _SHR_E_PARAM, "current_state_dealloc_in_p");
    SHR_NULL_CHECK(dealloc_result_p, _SHR_E_PARAM, "dealloc_result_p");

    /*
     * Verify that the stage has FESes.
     */
    if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage \"%s\" (%d). \r\n",
                     DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), field_stage);
    }
    /*
     * Verify that the field group isn't invalid
     */
    if (fg_id == DNX_FIELD_GROUP_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid field group id received (%d). Context ID is %d.\r\n", fg_id, context_id);
    }
    /*
     * Verify that the context ID is within range.
     */
    if ((unsigned int) context_id > (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts - 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid context ID id received (%d), must be between %d and %d. Field group id is %d.\r\n",
                     context_id, 0, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts - 1, fg_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_action_fes_dealloc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id,
    dnx_algo_field_action_fes_dealloc_state_in_t * current_state_dealloc_in_p,
    dnx_algo_field_action_fes_dealloc_out_t * dealloc_result_p)
{
    unsigned int fes_id_ndx;
    unsigned int mask_ndx;
    unsigned int fes_pgm_id;
    dnx_field_group_t curr_fg_id;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify Input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_algo_field_action_fes_dealloc_verify
                           (unit, field_stage, fg_id, context_id, current_state_dealloc_in_p, dealloc_result_p));
    /*
     * Initialize dealloc_result_p.
     * Note that as an extra safety measure, we initialize the entire arrays inside the structure and not just
     * the part of the array that is required by the stage (for example instead of iterating over the FES ID until
     * dnx_data_nof_fes_instruction_per_context we iterate until
     * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT).
     * Technically we could have skipped this initialization.
     */
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        dealloc_result_p->belongs_to_fg[fes_id_ndx] = FALSE;
        dealloc_result_p->fes_pgm_id_to_delete[fes_id_ndx] = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
        {
            dealloc_result_p->delete_mask[fes_id_ndx][mask_ndx] = FALSE;
        }
    }
    /*
     * Mark any FES quartet that isn't the zero program (and belongs to the field group) and any mask that isn't
     * the zero mask and has exactly one reference to delete.
     * Note that if we have zero number of references for a mask it means that it is not allocated to the context ID,
     * but may still be allocated to other context IDs.
     * Also note that a FES quartet with a zero number of references shouldn't occur.
     * Thirdly, note that if we do not delete a FES quartet in a specific FES ID, we do not delete any FES
     * action masks in that FES ID.
     */
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if ((fes_id != DNX_FIELD_EFES_ID_INVALID) && (fes_id != fes_id_ndx))
        {
            continue;
        }
        fes_pgm_id = current_state_dealloc_in_p->context_state.fes_pgm_id[fes_id_ndx];
        if (fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            /*
             * The quartet is allocated to the context ID.
             * Now we need to find out if it belongs to the field group,
             * by reading from SW state.
             */
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                case DNX_FIELD_STAGE_IPMF2:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                                    field_group.get(unit, fes_id_ndx, fes_pgm_id, &curr_fg_id));
                    break;
                case DNX_FIELD_STAGE_IPMF3:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                    field_group.get(unit, fes_id_ndx, fes_pgm_id, &curr_fg_id));
                    break;
                case DNX_FIELD_STAGE_EPMF:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                                    field_group.get(unit, fes_id_ndx, fes_pgm_id, &curr_fg_id));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
            }
            if (curr_fg_id == fg_id)
            {
                dealloc_result_p->belongs_to_fg[fes_id_ndx] = TRUE;
                /*
                 * Check if no other context uses the same FES quartet.
                 */
                if (current_state_dealloc_in_p->context_state.fes_quartet_nof_refs[fes_id_ndx] == 1)
                {
                    /*
                     * We mark the FES quartet to be deleted and any masks that aren't shared.
                     */
                    dealloc_result_p->fes_pgm_id_to_delete[fes_id_ndx] =
                        current_state_dealloc_in_p->context_state.fes_pgm_id[fes_id_ndx];
                    for (mask_ndx = 0;
                         mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
                         mask_ndx++)
                    {
                        if ((mask_ndx != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID) &&
                            (current_state_dealloc_in_p->context_state.mask_nof_refs[fes_id_ndx][mask_ndx] == 1))
                        {
                            /*
                             * The mask is allocated to the context ID, and only to it, and is therefore to be deleted.
                             */
                            dealloc_result_p->delete_mask[fes_id_ndx][mask_ndx] = TRUE;
                        }
                    }
                }
            }
        }
    }
    /*
     * Delete from SW state what is deallocated.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_dealloc_sw_state(unit, field_stage, context_id, dealloc_result_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify parameters of dnx_algo_field_action_ace_fes_allocate().
 * \param [in] unit -
 *  Device ID.
 * \param [in] ace_id -
 *  Identifier of ACE format. Also serves as result type in the ACE table and context for the ACE ACR.
 *  Not used in this algorithm.
 * \param [in] current_state_in_p -
 *  The current state of the allocation given to the allocation algorithm from the outside
 *  (as opposed to the current state in algo field SW state).
 *  At the moment contains the allocated FES quartets to the context ID we allocate for.
 * \param [in] fes_fg_in_p -
 *   The information about the new field group and context ID and what allocations they require.
 * \param [in] alloc_result_p -
 *   Pointer to the memory to be loaded by the calling function with the result of the allocation.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_ace_fes_allocate()
 */
static shr_error_e
dnx_algo_field_action_ace_fes_allocate_verify(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_algo_field_action_ace_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_ace_fes_alloc_in_t * fes_fg_in_p,
    dnx_algo_field_action_ace_fes_alloc_out_t * alloc_result_p)
{
    unsigned int fes_id_ndx;
    unsigned int fes_mask_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_in_p, _SHR_E_PARAM, "current_state_in_p");
    SHR_NULL_CHECK(fes_fg_in_p, _SHR_E_PARAM, "fes_fg_in_p");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");

    /*
     * Verify that the ACE ID is within range.
     */
    if (ace_id > dnx_data_field.ace.nof_ace_id_get(unit) - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ACE ID (%d) cannot exceed %d.\r\n",
                     ace_id, dnx_data_field.ace.nof_ace_id_get(unit) - 1);
    }
    /*
     * Verify that the number of actions doesn't exceed the number of FESes,
     * and that there is at least one action.
     * Also verify that the number of actions doesn't exceed the maximum number of actions per FG.
     */
    if (fes_fg_in_p->nof_actions > dnx_data_field.ace.nof_fes_instruction_per_context_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of actions (%d) exceeds the maximum number of FESes for ACE (%d).\r\n",
                     fes_fg_in_p->nof_actions, dnx_data_field.ace.nof_fes_instruction_per_context_get(unit));
    }
    else if (fes_fg_in_p->nof_actions == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of actions must be at least one.\r\n");
    }
    if (fes_fg_in_p->nof_actions > dnx_data_field.ace.nof_action_per_ace_format_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_fes_quartets (%d) exceeds the maximum number of actions per field group (%d).\r\n",
                     fes_fg_in_p->nof_actions, dnx_data_field.ace.nof_action_per_ace_format_get(unit));
    }
    /*
     * Verify that no zero mask is different from all zeros (verifying earlier state, not input).
     */
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        for (fes_mask_ndx = 0; fes_mask_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_MASKS_PER_FES; fes_mask_ndx++)
        {
            if (fes_mask_ndx == DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID)
            {
                if (current_state_in_p->masks[fes_id_ndx][fes_mask_ndx] != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "In FES ID %d in ACE ACR, zero mask isn't composed of all zeroes.\r\n", fes_id_ndx);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_action_ace_fes_allocate(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_algo_field_action_ace_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_ace_fes_alloc_in_t * fes_fg_in_p,
    dnx_algo_field_action_ace_fes_alloc_out_t * alloc_result_p)
{
    unsigned int num_masks_to_alloc;
    unsigned int num_fes_available_for_mask;
    unsigned int num_shareable;
    unsigned int fes_inst_ndx;
    unsigned int fes_id_ndx;
    unsigned int fes_mask_ndx;
    unsigned int fes_available_for_mask[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    unsigned int available_mask_id[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    unsigned int fes_allocated[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    unsigned int instr_allocated[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    int shared_fes_id_found;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Note that this algorithm does not assume that there are only 2 acions masks.
     */

    /*
     * Verify Input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_algo_field_action_ace_fes_allocate_verify
                           (unit, ace_id, current_state_in_p, fes_fg_in_p, alloc_result_p));

    /*
     * Initialize fes_allocated
     */
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        fes_allocated[fes_id_ndx] = FALSE;
    }
    /*
     * Initialize instr_allocated
     */
    for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
    {
        instr_allocated[fes_inst_ndx] = FALSE;
    }

    /*
     * Count the number of masks that need to be allocated.
     */
    num_masks_to_alloc = 0;
    for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
    {
        if (fes_fg_in_p->masks[fes_inst_ndx] != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
        {
            num_masks_to_alloc++;
        }
    }

    /*
     * If we have action masks to allocate, allocate FES IDs for instructions that require action masks first.
     */
    if (num_masks_to_alloc > 0)
    {
        /*
         * Find the FESes that have masks that aren't all zeros outside of the zero mask, and count them.
         */
        num_fes_available_for_mask = 0;
        for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
        {
            fes_available_for_mask[fes_id_ndx] = FALSE;
            for (fes_mask_ndx = 0; fes_mask_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_MASKS_PER_FES; fes_mask_ndx++)
            {
                if (fes_mask_ndx != DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID)
                {
                    if (current_state_in_p->masks[fes_id_ndx][fes_mask_ndx] == DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
                    {
                        fes_available_for_mask[fes_id_ndx] = TRUE;
                        available_mask_id[fes_id_ndx] = fes_mask_ndx;
                        num_fes_available_for_mask++;
                        break;
                    }
                }
            }
        }
        /*
         * Allocate required masks that can be shared.
         */
        num_shareable = 0;
        for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
        {
            shared_fes_id_found = FALSE;
            if (fes_fg_in_p->masks[fes_inst_ndx] != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
            {
                for (fes_id_ndx = 0;
                     fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT
                     && shared_fes_id_found == FALSE; fes_id_ndx++)
                {
                    if (fes_allocated[fes_id_ndx] == FALSE)
                    {
                        for (fes_mask_ndx = 0; fes_mask_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_MASKS_PER_FES; fes_mask_ndx++)
                        {
                            if (fes_mask_ndx != DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID)
                            {
                                if (current_state_in_p->masks[fes_id_ndx][fes_mask_ndx] ==
                                    fes_fg_in_p->masks[fes_inst_ndx])
                                {
                                    fes_allocated[fes_id_ndx] = TRUE;
                                    instr_allocated[fes_inst_ndx] = TRUE;
                                    alloc_result_p->fes_id[fes_inst_ndx] = fes_id_ndx;
                                    alloc_result_p->mask_id[fes_inst_ndx] = fes_mask_ndx;
                                    num_shareable++;
                                    shared_fes_id_found = TRUE;
                                    /*
                                     * If the FES is marked as available for mask allocation, remove that marking.
                                     */
                                    if (fes_available_for_mask[fes_id_ndx])
                                    {
                                        fes_available_for_mask[fes_id_ndx] = FALSE;
                                        num_fes_available_for_mask--;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (num_fes_available_for_mask < num_masks_to_alloc - num_shareable)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                         "ACE ID %d: Number of FESes with action masks available for allocation is %d, "
                         "while umber of actions that require action mask allocation "
                         "that could not be shared is %d (shareable %d).\r\n",
                         ace_id, num_fes_available_for_mask, num_masks_to_alloc - num_shareable, num_shareable);
        }

        /*
         * If we still have more masks to allocate, continue allocating masks.
         * Allocate available FESes to actions that require action masks.
         */
        for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
        {
            if (fes_fg_in_p->masks[fes_inst_ndx] != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK
                && instr_allocated[fes_inst_ndx] == FALSE)
            {
                for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
                {
                    if (fes_allocated[fes_id_ndx] == FALSE && fes_available_for_mask[fes_id_ndx])
                    {
                        fes_allocated[fes_id_ndx] = TRUE;
                        instr_allocated[fes_inst_ndx] = TRUE;
                        alloc_result_p->fes_id[fes_inst_ndx] = fes_id_ndx;
                        alloc_result_p->mask_id[fes_inst_ndx] = available_mask_id[fes_id_ndx];;
                        break;
                    }
                }
            }
        }
    }

    /*
     * Alloacte to the instructions that don't require any action mask, 
     * with FES IDs that weren't previously allocated.
     */
    for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
    {
        if (fes_fg_in_p->masks[fes_inst_ndx] == DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK
            && instr_allocated[fes_inst_ndx] == FALSE)
        {
            /*
             * For each FES ID, check if it is allocated. we start looking from the number of instructions we 
             * have looped through, since if the lower indices were available, they would have already been used.
             */
            for (fes_id_ndx = fes_inst_ndx; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                if (fes_allocated[fes_id_ndx] == FALSE)
                {
                    fes_allocated[fes_id_ndx] = TRUE;
                    instr_allocated[fes_inst_ndx] = TRUE;
                    alloc_result_p->fes_id[fes_inst_ndx] = fes_id_ndx;
                    alloc_result_p->mask_id[fes_inst_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID;
                    break;
                }
            }
        }
    }
    /*
     * Fill the number of actions.
     */
    alloc_result_p->nof_actions = fes_fg_in_p->nof_actions;

    /*
     * Sanity check. Verify that all actions were allocate. Can be removed.
     */
    for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
    {
        instr_allocated[fes_inst_ndx] = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify parameters of dnx_algo_field_action_fes_quartet_sw_state_info_get().
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *   For which of the PMF blocks the configuration is done (iPMF-1, iPMF2, iPMF-3, ePMF-1).
 * \param [in] context_id -
 *    The context ID that we remove the field group from.
 * \param [in] fes_id -
 *   FES ID.
 * \param [in] fes_pgm_id -
 *   The FES program ID of the FEQ quartet for the context ID and FES ID.
 * \param [in] fes_quartet_sw_state_info_p -
 *   Pointer to an array of structures dnx_algo_field_action_fes_quartet_sw_state_get_info_t with the number of elements
 *   DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT.
 *   to be loaded with the SW state info for the FES quartet.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_algo_field_action_fes_quartet_sw_state_info_get()
 */
shr_error_e
dnx_algo_field_action_fes_quartet_sw_state_info_get_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t fes_pgm_id,
    dnx_algo_field_action_fes_quartet_sw_state_get_info_t * fes_quartet_sw_state_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_quartet_sw_state_info_p, _SHR_E_PARAM, "fes_quartet_sw_state_info_p");

    /*
     * Verify that the stage has FESes.
     */
    if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage \"%s\" (%d). \r\n",
                     DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), field_stage);
    }
    /*
     * Verify indices ranges.
     */
    if ((unsigned int) context_id >= dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context ID (%d) must be between zero and %d. \r\n",
                     context_id, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts - 1);
    }
    if ((unsigned int) fes_id >=
        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FES ID (%d) must be between zero and %d. \r\n",
                     fes_id, dnx_data_field.stage.stage_info_get(unit,
                                                                 field_stage)->nof_fes_instruction_per_context - 1);
    }
    if ((unsigned int) fes_pgm_id >= dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_programs)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FES program ID (%d) must be between zero and %d. \r\n",
                     fes_pgm_id, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_programs - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_action_fes_quartet_sw_state_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t fes_pgm_id,
    dnx_algo_field_action_fes_quartet_sw_state_get_info_t * fes_quartet_sw_state_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the input.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_quartet_sw_state_info_get_verify
                    (unit, field_stage, context_id, fes_id, fes_pgm_id, fes_quartet_sw_state_info_p));

    /*
     * Read the SW state for the FES ID.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                            priority.get(unit, context_id, fes_id, &(fes_quartet_sw_state_info_p->priotrity)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                            field_group.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->fg_id)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                            place_in_fg.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->place_in_fg)));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                            priority.get(unit, context_id, fes_id, &(fes_quartet_sw_state_info_p->priotrity)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                            field_group.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->fg_id)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                            place_in_fg.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->place_in_fg)));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                            priority.get(unit, context_id, fes_id, &(fes_quartet_sw_state_info_p->priotrity)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                            field_group.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->fg_id)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                            place_in_fg.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->place_in_fg)));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }
    /*
     * Sanity check, verify that the FES ID has a FES quartet allocated for the context ID according to
     * priority SW state.
     */
    if (fes_quartet_sw_state_info_p->priotrity == DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context ID %d has no FES quartet allocated in FES ID %d. \r\n", context_id, fes_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_action_init(
    int unit)
{
    unsigned int fes_id_ndx;
    unsigned int fes_pgm_ndx;
    unsigned int mask_ndx;
    dnx_algo_field_action_fes_state_t zero_fes_state;
    unsigned int context_ndx;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify action priorities.
     */
    if (DNX_FIELD_ACTION_PRIORITY_INVALID & ((uint32) 1 << BCM_FIELD_ACTION_VALID_OFFSET))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DNX_FIELD_ACTION_PRIORITY_INVALID (0x%x) has "
                     "the bit BCM_FIELD_ACTION_VALID_OFFSET (0x%x) set.\r\n",
                     DNX_FIELD_ACTION_PRIORITY_INVALID, (1 << BCM_FIELD_ACTION_VALID_OFFSET));
    }
    if (BCM_FIELD_ACTION_INVALIDATE & ((uint32) 1 << BCM_FIELD_ACTION_VALID_OFFSET))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "BCM_FIELD_ACTION_INVALIDATE (0x%x) has "
                     "the bit BCM_FIELD_ACTION_VALID_OFFSET (0x%x) set.\r\n",
                     BCM_FIELD_ACTION_INVALIDATE, ((uint32) 1 << BCM_FIELD_ACTION_VALID_OFFSET));
    }
    if (DNX_FIELD_ACTION_PRIORITY_INVALID == BCM_FIELD_ACTION_INVALIDATE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DNX_FIELD_ACTION_PRIORITY_INVALID (0x%x) is the "
                     "same as BCM_FIELD_ACTION_INVALIDATE (0x%x).\r\n",
                     DNX_FIELD_ACTION_PRIORITY_INVALID, BCM_FIELD_ACTION_INVALIDATE);
    }

    /*
     * Call SW state init.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.init(unit));
    /*
     * Initialize zero_fes_state.
     * Use invalid indications for field group to invalidate all elements.
     * The rest are zeroed for predictability, though not required.
     * It uses the maximum of all NOF_FES_PROGRAMS because at the moment all stages have the same number.
     */
    for (fes_pgm_ndx = 0; fes_pgm_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS; fes_pgm_ndx++)
    {
        zero_fes_state.field_group[fes_pgm_ndx] = DNX_FIELD_GROUP_INVALID;
        zero_fes_state.place_in_fg[fes_pgm_ndx] = 0;
    }
    for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
    {
        zero_fes_state.mask_is_alloc[mask_ndx] = FALSE;
    }
    /*
     * iPMF-1 shares it's action resolution with iPMF-2 and therefore needs no fes SW sate.
     */
    /*
     * Initialize iPMF-2.
     */
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        /*
         * Initialize iPMF-2 fes state using zero_fes_state.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
        /*
         * Initialize iPMF-2 context state.
         */
        for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                            priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
        }
    }
    /*
     * Initialize iPMF-3.
     */
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        /*
         * Initialize iPMF-3 fes state using zero_fes_state.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
        /*
         * Initialize iPMF-3 context state.
         */
        for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_CONTEXTS; context_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                            priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
        }
    }
    /*
     * Initialize ePMF.
     */
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        /*
         * Initialize ePMF fes state using zero_fes_state.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
        /*
         * Initialize ePMF context state.
         */
        for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_CONTEXTS; context_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                            priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_action_sw_state_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/* } */
