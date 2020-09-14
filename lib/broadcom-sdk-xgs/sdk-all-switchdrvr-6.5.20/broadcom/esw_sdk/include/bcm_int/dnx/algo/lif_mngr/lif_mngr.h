/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file lif_mngr.h
 * $Id$
 *
 * This file contains the semi-private APIs required for lif algorithms.
 * The functions that appear here are used in the dbal and algo gpm modules.
 *
 * If you only require lif allocation, don't include this file. Only include it if you're involved
 * in lif allocation in some way.
 *
 */

#ifndef  INCLUDE_LIF_MNGR_H_INCLUDED
#define  INCLUDE_LIF_MNGR_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>
#include <soc/dnx/mdb.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

/**
 * Flags for local outlif allocation.
 *
 * \see
 *  dnx_lif_mngr_lif_allocate
 *  dnx_lif_mngr_lif_free
 * {
 */

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: Map or unmap the data bank this lif belongs to in HW.
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK         SAL_BIT(18)

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: Map or unmap the linked list this lif has in HW.
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK           SAL_BIT(19)

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: Map or unmap the pointers of the outlif's MSB to physical DB in HW.
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS           SAL_BIT(20)

/**
 * \brief a combination of both previous flags.
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW \
    (DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK \
                | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS)

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: When this flag is set, don't use an existing outlif bank to allocate an entry.
 * Instead, allocate a new bank.
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_NEW_BANK_ALLOCATION           SAL_BIT(21)

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: When this bank is set, don't attempt to use an MDB cluster for data, and instead
 * use an EEDB bank.
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_DATA_IN_EEDB_BANKS            SAL_BIT(22)

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: Don't allocate new outlif bank for this allocation. Only use existing banks.
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_ALLOCATE_NEW_BANK               SAL_BIT(23)

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: Replace result type of an existing lif.
 * See documentation of \ref dnx_algo_local_outlif_can_be_reused for full instructions of using this feature.
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE                              SAL_BIT(24)

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: Reserve an outlif entry that will immediately be replaced by a different result type.
 * See documentation of \ref dnx_algo_local_outlif_can_be_reused for full instructions of using this feature.
 *
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE                              SAL_BIT(25)

/**
 * \brief
 * When this flag is set, always use linked list.
 * This flag must be compatible with arch xml ll indication (can be either OPTIONAL or MANDATORY)
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST               SAL_BIT(26)

/**
 * \brief
 * When this flag is set, don't use linked list.
 * This flag must be compatible with arch xml ll indication (can be either OPTIONAL or NEVER)
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST               SAL_BIT(27)

/**
 * \brief This flag is internal, used only by dbal and lif ctests, in order to avoid outlif logical phase <----> {APPDB, RT} mapping errors
 *
 * \see dnx_lif_mngr_lif_allocate
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH                 SAL_BIT(28)

/**
 * \brief
 * INTERNAL FLAG.
 * When this flag is set - use id provided for the local outlif
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_WITH_ID               SAL_BIT(29)

/**
 * \brief
 * INTERNAL FLAG.
 * When this flag is set - ignore tag in res mngr allocation
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RES_IGNORE_TAG               SAL_BIT(30)

/**
 * \brief
 * INTERNAL FLAG.
 * When this flag is set - don't update hw tables (used for simulation and testing)
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_UPDATE_HW               SAL_BIT(31)

/**
 * }
 */

/**
 * Flags for local inlif allocation.
 *
 * \see
 *  dnx_lif_mngr_lif_allocate
 *  dnx_lif_mngr_lif_free
 * {
 */

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: Replace result type of an existing lif.
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE                              SAL_BIT(20)

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: Allocate entry of old result_type - to be replaced with an entry with the new result_type. Serves as an indication for reserve only
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE                              SAL_BIT(21)

/**
 * \brief
 * TO BE USED BY LIF_MNGR ONLY: Allocate entry of old result_type - to be replaced with an entry with the new result_type for smaller size (in indexes)
 */
#define DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE_FOR_SMALLER                  SAL_BIT(22)

/**
 * }
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
/**
 * \brief Generic macro to verify lif id.
 *
 * Check whether a given lif is less than 0, or too high. If so, log an error (with some fixed extended
 * info) and exit with _SHR_E_PARAM.
 *
 * The macro is intended to be used as internal macro for the macros below, and
 * not as a standalone.
 *
 * \par DIRECT INPUT:
 *   \param [in] _unit
 *     Relevant unit
 *   \param [in] _nof_lifs
 *     Number of lifs of this specific type. Should be taken from Device data.
 *   \param [in] _lif_name
 *     Ascii. Name of tested lif type. MUST be encapsulated in double quotes.
 *   \param [in] _lif_id
 *     Integer value of the lif to test.
 *
 */
#define INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, _nof_lifs, _lif_name, _lif_id) \
            if ((int)_lif_id < 0)    \
            { \
                SHR_ERR_EXIT(_SHR_E_PARAM, "%s can't be negative: %d", _lif_name, _lif_id); \
            } \
            if (_lif_id >= _nof_lifs) \
            { \
                SHR_ERR_EXIT(_SHR_E_PARAM, "%s is too high. Given 0x%08X but maximum is 0x%08X", _lif_name, _lif_id, _nof_lifs - 1); \
            }

/**
 * \brief General utility macros used to validate that lif IDs are legal.
 *   The information is taken from the lif module of dnx_device data,
 *   and uses INTERNAL_LIF_MNGR_LIF_ID_VERIFY as internal implementation.
 *
 * \see
 * INTERNAL_LIF_MNGR_LIF_ID_VERIFY
 */
#define LIF_MNGR_GLOBAL_IN_LIF_VERIFY(_unit, _lif_id) \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, dnx_data_lif.global_lif.nof_global_in_lifs_get(_unit), \
                                    "Global inlif", _lif_id)

#define LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(_unit, _lif_id) \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, dnx_data_lif.global_lif.nof_global_out_lifs_get(_unit), \
                                    "Global outlif", _lif_id)

#define LIF_MNGR_LOCAL_SBC_IN_LIF_VERIFY(_unit, _lif_id) \
        { \
            int __nof_lifs; \
            SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, &__nof_lifs, NULL)); \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, __nof_lifs, \
                                    "Local inlif shared by cores", _lif_id) \
        }

#define LIF_MNGR_LOCAL_DPC_IN_LIF_VERIFY(_unit, _lif_id) \
        { \
            int __nof_lifs; \
            SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, NULL, &__nof_lifs)); \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, __nof_lifs, \
                                    "Local inlif duplicated per core", _lif_id) \
        }

#define LIF_MNGR_LOCAL_OUT_LIF_VERIFY(_unit, _lif_id) \
            INTERNAL_LIF_MNGR_LIF_ID_VERIFY(_unit, dnx_data_lif.out_lif.nof_local_out_lifs_get(unit), \
                                    "Local outlif", _lif_id)

/**
 * \brief The size of outlif bank.
 *
 * The outlif bank size is determined by the maximum of how many linked list entries fit into a LL bank, and the number of data entries
 *     that can be mapped to a cluster at a time.
 * It is currently assumed that this number is greater than or bigger than the number of logical entries on a single physical cluster.
 *     This assumption will not be correct if we LL banks to hold data.
 *
 */
#define DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(_unit)      (dnx_data_lif.out_lif.allocation_bank_size_get(_unit))

#define DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(_unit)      ((dnx_data_lif.out_lif.nof_local_out_lifs_get(_unit) \
                                                             / DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(_unit)))

#define DNX_ALGO_LOCAL_OUTLIF_INVALID_BANK          (-1)

/*
 * \brief Maximum size of entry in the data banks.
 */
#define EEDB_MAX_DATA_ENTRY_SIZE    MDB_NOF_CLUSTER_ROW_BITS

/**
 * \brief Given an outlif bank, return its first element.
 */
#define OUTLIF_BANK_FIRST_INDEX(_unit, _outlif_bank) (_outlif_bank * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(_unit))

/**
 * \brief Break up an outlif to its bank and offset within the bank.
 */
#define DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(_local_outlif) \
        ((_local_outlif) / DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit))

#define DNX_ALGO_OUTLIF_LOCAL_LIF_TO_OFFSET(_local_outlif) \
        ((_local_outlif) % DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit))

#define DNX_ALGO_OUTLIF_BANK_AND_OFFSET_TO_LOCAL_LIF(_bank, _offset) \
        ((_bank) * DNX_ALGO_LOCAL_OUTLIF_BANK_SIZE(unit) + (_offset))

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/**
 * \brief The logical phases for local outlif.
 */
typedef enum
{
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1 = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_3,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_4,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_5,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_6,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_7,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_8,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT
} dnx_algo_local_outlif_logical_phase_e;

/**
 * \brief This struct is used to collect outlif statistics for diagnostics or tests.
 */
typedef struct
{
    int nof_outlif_banks_in_external_memory;
    int nof_used_outlif_banks_in_external_memory;
    int nof_eedb_banks;
    int nof_used_eedb_banks;
    int nof_glem_entries;
    int nof_used_glem_entries;
} dnx_algo_local_outlif_stats_t;

/**
 * \brief This struct is used to configure the HW, if necessary, after an outlif was allocated or freed.
 */
typedef struct
{
    /*
     * DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_* flags.
     */
    uint32 flags;
    /*
     * MDB Cluster info for the data bank of the outlif bank.
     */
    mdb_cluster_alloc_info_t data_bank_info;
    /*
     * MDB Cluster info for the ll bank of the outlif bank.
     */
    mdb_cluster_alloc_info_t ll_bank_info;
    /*
     * Logical phase of the outlif.
     */
    dnx_algo_local_outlif_logical_phase_e logical_phase;
} dnx_algo_local_outlif_hw_info_t;

/**
 * \brief Criteria to use when assigning an outlif bank.
 *
 * Bank assignment signifies whether it's mapped to some physical DB.
 *
 * A bank can be assigned to either an MDB cluster, or available/assigned to an EEDB bank.
 */
typedef enum
{
    DNX_ALGO_OUTLIF_BANK_UNASSIGNED,
    DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_EEDB,
    DNX_ALGO_OUTLIF_BANK_ASSIGNED_TO_MDB
} dnx_algo_outlif_bank_assignment_mode_e;

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/**
 * \brief
 * Given an outlif bank, returns the next allocated outlif bank
 * including the given bank. After a bank was found, advancing
 * the bank for the next iteration should be done outside of the
 * function.
 *
 * When there are no more outlif banks,
 * DNX_ALGO_RES_ILLEGAL_ELEMENT will be returned.
 *
 *  \param [in] unit - Unit-ID
 *  \param [in,out] next_bank - As input: the first outlif bank
 *         to start searching for an allocated bank from.
 *          As output: an allocated outlif bank, or no more
 *          banks indication.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_next_bank_get(
    int unit,
    uint32 *next_bank);

/**
 * \brief
 * Given an outlif info, with the old result type in the alterntaive_result_type and
 * the new result type in dbal_result_type, returns indication whether the same outlif ID can
 * be used, or if a new entry needs to be allocated.
 *
 * If the same result can be used, then lif_mngr_lif_allocate can be called with the
 * DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE flag (local oulif ID must be given as well).
 *
 * Otherwise, allocate a new local outlif with the lif_mngr_lif_allocate with the
 * DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE flag,
 * then after all the pointers are moved use the same outlif info with the REPLACE and RESERVE flags.
 * This second call must be made before any other access to outlif allocation, otherwise the
 * success of the operation is not guaranteed.
 *
 *
 *  \param [in] unit - Unit-ID
 *  \param [in] outlif_info - see above.
 *  \param [out] can_be_reused - see above.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_can_be_reused(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *can_be_reused);

/**
 * \brief
 * Given an inlif info, with the old result type in the old_result_type and
 * the new result type in dbal_result_type, returns indication whether the same inlif ID can
 * be used, or if a new entry needs to be allocated.
 *
 * If the same result can be used, then lif_mngr_lif_allocate can be called with the
 * DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE flag (local inlif ID must be given as well).
 *
 * Otherwise, allocate a new local inlif with the lif_mngr_lif_allocate with the
 * DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE flag,
 * then after all the pointers are moved use the same inlif info with the REPLACE and RESERVE flags
 *
 *
 *  \param [in] unit - Unit-ID
 *  \param [in] inlif_info - see above.
 *  \param [out] can_be_reused - see above.
 *  \param [out] smaller_inlif - In case of new smaller outlif - mark it for caller, for later res allocation.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_inlif_can_be_reused(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info,
    int *can_be_reused,
    int *smaller_inlif);

/**
 * \brief
 * Fills input arrays with the outlif hw infos required to configure all outrif banks on the MDB.
 *
 *  \param [in] unit - Unit-ID
 *  \param [out] outrif_bank_starts - Pointer to array of ints.
 *          Returns the bank starts for all outrif physical banks.
 *  \param [out] outlif_hw_infos - Pointer to an array of \ref dnx_algo_local_outlif_hw_info_t.
 *          Fills the outlif hw infos corresponding to the bank starts in outrif_bank_starts.
 *  \param [out] nof_outrifs_info - Number of physical banks to be configured in HW.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_outrif_physical_bank_hw_info_get(
    int unit,
    int *outrif_bank_starts,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_infos,
    int *nof_outrifs_info);

/**
 * \brief
 *   Allocate global lif and local lif, and create mapping between them.
 *
 *   This API performs several functions:
 *   1. Allocate local and global lifs (ingress and egress, as required).
 *   2. Keep mapping of global to local lifs and vice versa in the sw state.
 *   3: Keep LIF TABLE DBAL ID and type in SW state.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] flags -
 *     Currently supported flags are \ref LIF_MNGR_GLOBAL_LIF_WITH_ID \n
 *        to allocate the global lif with the ID given in global lif argument,
 *        and \ref LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF to allocate local lif only.
 *   \param [in,out] global_lif -
 *     Int pointer to memory to write output into. \n
 *     For \ref LIF_MNGR_GLOBAL_LIF_WITH_ID, this may also be indirect input.
 *     \b As \b output - \n
 *       This procedure loads pointed memory with the global lif allocated. \n
 *       Not used as output when \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is used.
 *       If flag \ref LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF is set, then this will be LIF_MNGR_INVALID. In that case,
 *       the pointer can also be NULL, and it will not be accessed.
 *     \b As \b input - \n
 *       If flag LIF_MNGR_GLOBAL_LIF_WITH_ID is set, this holds
 *         the id to be allocated.
 *   \param [in,out] inlif_info -
 *     Pointer to memory for local inlif allocation input/output. \n
 *     If NULL, then this will be ignored, local inlifs will not be allocated,
 *        and outlif_info must not be NULL.
 *     \b As \b input - \n
 *       All elements in inlif info are required as input, except for local_inlifs which will be ignored.
 *       In the case of duplicated per core lif table, the field 'core_id' will specify which core is used
 *          for allocation, or it can be set to _SHR_CORE_ALL to allocate one lif per core.
 *       See \ref lif_mngr_local_inlif_info_t for detailed description of each field.
 *     \b As \b output - \n
 *       This procedure loads the field inlif_info->local_inlifs with the local in lifs allocated, in the manner
 *       detailed in the struct's description.
 *   \param [in,out] outlif_info -
 *     Pointer to memory for local outlif allocation input/output. \n
 *     If NULL, then this will be ignored, local outlif will not be allocated,
 *        and inlif_info must not be NULL.
 *     \b As \b input - \n
 *       All elements in outlif info are inputs, except for local_outlif.
 *     \b As \b output - \n
 *       This procedure loads the field outlif_info->local_outlif with the local out lif allocated.
 *   \param [out] outlif_hw_info - Pointer to memory where the info to configure outlif HW will be filled.
 *
 * \par INDIRECT INPUT
 *   Resource manager of the ingress, egress and global lifs.
 *   Device data used for input validation.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Resource managers of the ingress, egress and global lifs.
 * \remark
 *   None.
 */
shr_error_e dnx_lif_mngr_lif_allocate(
    int unit,
    uint32 flags,
    int *global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info);

/**
 * \brief
 *   Deallocate the given global lif and its local in/outlif.
 *
 *   All the steps described in lif_mngr_lif_allocte will be reverted.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] global_lif -
 *     Global lif to be deallocated. If the given local lif doesn't have a global lif, then this must be LIF_MNGR_INVALID.
 *   \param [in] inlif_info -
 *     Pointer to memory for local inlif deallocation. \n
 *     If NULL, then this will be ignored, local inlif will not be freed,
 *        and local_outlif must not be LIF_MNGR_INVALID.
 *     \b As \b input - \n
 *       The local inlif[s] in the local_inlifs array will be freed. \n
 *       All elements in inlif info are required as input, except for dbal_result_type, which should be 0.
 *       See \ref lif_mngr_local_inlif_info_t for detailed description of each field.
 *   \param [in,out] outlif_info -
 *     \b As \b input - \n
 *       The local_outlif field holds the local outlif to be deallocated, or LIF_MNGR_INVALID to deallocate ingress only.
 *     \b As \b output - \n
 *       If hw needs to be updated after the allocation, then the outlif_flags field will hold the flag
 *       \ref DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW will be set.
 *   \param [out] outlif_hw_info - Pointer to memory where the info to configure outlif HW will be filled.
 *
 * \par INDIRECT INPUT
 *   Resource manager of the ingress, egress and global lifs.
 *   Device data used for input validation.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Resource manager of the ingress, egress and global lifs.
 * \remark
 *   None.
 */
shr_error_e dnx_lif_mngr_lif_free(
    int unit,
    int global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info);

/**
 * \brief
 *   Init lif mngr module, including the lif mappings and global/local lif allocation
 *   submodules.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 * \par INDIRECT INPUT
 *   Lif table sizes from the lif submodule DNX data, used to initialize lif resources.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Local and gloal lif resouce managers, lif mapping and lif allocation
 *     sw state sub modules are initialized by this function.
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mngr_init(
    int unit);

/**
 * \brief
 *   Deinit lif mngr module, including the lif mappings and global/local lif allocation
 *   submodules.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 * \par INDIRECT INPUT
 *   Local and global lif resource managers, lif mapping and lif allocation
 *     sw state sub modules.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Local and global lif resource managers, lif mapping and lif allocation
 *     sw state sub modules are deinitialized by this function.
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mngr_deinit(
    int unit);

/**
 * \brief - Given a unit, returns the number of inlifs (INLIF-1,
 *        INLIF-2/3 or both) in the device.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [out] nof_inlif1_lifs - if not null, filled with the
 *        number of INLIF-1 LIFs in the device.
 * \param [out] nof_inlif2_lifs - if not null, filled with the
 *        number of INLIF-2/3 LIFs in the device.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_lif_mngr_nof_inlifs_get(
    int unit,
    int *nof_inlif1_lifs,
    int *nof_inlif2_lifs);

/**
* \brief
*   setes the table_id and result type for inlif use mainly for internal resource mngr and DBAL.
*   \param [in] unit - Relevant unit.
*   \param [in] local_in_lif - Relevant lif.
*   \param [in] core_id - Relevant core. If lif is per core, it must be a valid core id, otherwise, must be
*          _SHR_CORE_ANY. If DPC lif that's allocated on all cores, then all cores will be set.
*   \param [in] table_id - related dbal table ID.
*   \param [in] result_type - related result type.
*   \param [in] fixed_entry_size - Minimal entry size
*  \return
*    \retval Zero if no error occurred
*    \retval  See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
*****************************************************/

shr_error_e dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_set(
    int unit,
    int local_in_lif,
    int core_id,
    dbal_tables_e table_id,
    uint32 result_type,
    uint32 fixed_entry_size);

/**
* \brief
*   gets the table_id and result type for inlif use mainly for internal resource mngr and DBAL.
*   \param [in] unit - Relevant unit.
*   \param [in] local_in_lif - Relevant lif.
*   \param [in] core_id - Relevant core. If resource is per core, it must be a valid core id, otherwise, must be
*          _SHR_CORE_ANY.
*   \param [in] physical_table_id - Which physical table should be accessed. Legal options are DBAL_PHYSICAL_TABLE_INLIF_[1|2].
*                        This argument is not passed in _set and _delete because we can produce it from the logical table id.
*   \param [out] table_id - returned related dbal table ID.
*   \param [out] result_type - returned related result type.
*   \param [out] fixed_entry_size - Minimal entry size
*  \return
*    \retval Zero if no error occurred
*    \retval  See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
*****************************************************/
shr_error_e dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(
    int unit,
    int local_in_lif,
    int core_id,
    dbal_physical_tables_e physical_table_id,
    dbal_tables_e * table_id,
    uint32 *result_type,
    uint32 *fixed_entry_size);

/**
* \brief
*   setes the table_id and result type for inlif use mainly for internal resource mngr and DBAL.
*   \param [in] unit - Relevant unit.
*   \param [in] outlif_info - outlif struct.
*   \param [in] local_out_lif - Relevant lif.
*   \param [in] table_id - related dbal table ID.
*   \param [in] result_type - related result type.
*   \param [in] fixed_entry_size - Minimal entry size
*  \return
*    \retval Zero if no error occurred
*    \retval  See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
*****************************************************/
shr_error_e dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int local_out_lif,
    dbal_tables_e table_id,
    uint32 result_type,
    uint32 fixed_entry_size);

/**
* \brief
* Retrieve dbal table id, dbal result type and eedb phase from SW state by outlif id
* \par DIRECT INPUT:
*   \param [in] unit                    - Relevant unit.
*   \param [in] local_out_lif           - out lif id.
*   \param [out] dbal_table_id          - DBAL table ID.
*   \param [out] result_type            - DBAL table result
*          type.
*   \param [out] outlif_phase           - EEDB phase.
*   \param [out] fixed_entry_size       - Minimal entry size
*   \param [out] has_ll                 - LL exists per lif
 *  \param [out] is_count_profile       - Counting Profile
 *                                        indication
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
*/
shr_error_e dnx_lif_mngr_outlif_sw_info_get(
    int unit,
    int local_out_lif,
    dbal_tables_e * dbal_table_id,
    uint32 *result_type,
    lif_mngr_outlif_phase_e * outlif_phase,
    uint32 *fixed_entry_size,
    uint8 *has_ll,
    uint8 *is_count_profile);

/**
* \brief
*   sets the is_count_profile indication in the SW DB.
*
*   \param [in] unit - Relevant unit.
*   \param [in] local_out_lif - Relevant local Out-LIF.
*  \return
*    \retval Zero if no error occurred
*    \retval  See \ref shr_error_e
*****************************************************/
shr_error_e dnx_lif_mngr_outlif_sw_info_is_count_profile_set(
    int unit,
    int local_out_lif);

/**
 * \brief
 * Given a local outlif and dbal table id, returns the eedb phase this outlif points to.
 * \par DIRECT INPUT:
 *   \param [in] unit                    - Relevant unit.
 *   \param [in] local_outlif           - outlif id.
 *   \param [in] dbal_table_id          - DBAL table ID.
 *   \param [out] outlif_phase      - EEDB phase.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
*/

shr_error_e dnx_algo_local_outlif_to_eedb_logical_phase(
    int unit,
    int local_outlif,
    dbal_tables_e dbal_table_id,
    lif_mngr_outlif_phase_e * outlif_phase);

/**
* \brief
*   deletes the table_id and result type for inlif use mainly for internal resource mngr and DBAL.
*   \param [in] unit - Relevant unit.
*   \param [in] core_id - Relevant core. If DPC lif that's allocated on all cores, then all cores will be set.
*   \param [in] local_in_lif - Relevant lif.
*   \param [in] table_id - Dbal table of the lif to delete.
*  \return
*    \retval Zero if no error occurred
*    \retval  See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
*****************************************************/
shr_error_e dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_delete(
    int unit,
    int core_id,
    uint32 local_in_lif,
    dbal_tables_e table_id);

/**
* \brief
*   deletes the table_id and result type for outlif use mainly
*   for internal resource mngr and DBAL.
*   \param [in] unit - Relevant unit.
*   \param [in] local_out_lif - Relevant lif.
*  \return
*    \retval Zero if no error occurred
*    \retval  See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
*****************************************************/
shr_error_e dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(
    int unit,
    int local_out_lif);

/**
 * \brief
 * translate outlif phase string to outlif phase id.
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit
 *    \param [in] outlif_phase_str - \n
 *      string to translate
 *    \param [in] outlif_phase - \n
 *      pointer to phase id
 *
 *  \par DIRECT OUTPUT:
 *    shr_error_e - \n
 *      Error code
 */

shr_error_e dnx_lif_mngr_phase_string_to_id(
    int unit,
    char *outlif_phase_str,
    lif_mngr_outlif_phase_e * outlif_phase);

/**
 * \brief
 *      To be called during the lif lib init, this function assigns outlif banks to all MDB clusters.
 *      It returns the mdb clusters info that should be configured.
 *
 * \param [in] unit - Unit-ID
 * \param [out] init_clusters_info - The info for the mdb clusters that were assigned.
 * \param [out] nof_init_clusters - How many mdb clusters were assigned.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_mdb_bank_database_init(
    int unit,
    mdb_cluster_alloc_info_t * init_clusters_info,
    int *nof_init_clusters);

/**
 * \brief
 * To be called during device init.
 * Alocate all the rifs in the resource manager, mark their banks as in use and remove them from their partner
 * phase's availble banks.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_rif_init(
    int unit);

/**
 * \brief
 *      Given a local outlif logical phase, returns the physical phase mapped to it.
 *
 * \param [in] unit - Unit-ID
 * \param [in] logical_phase - Logical phase to be checked.
 * \param [out] physical_phase - This function loads pointed memory with the physical phase
 *      that this logical phase is mapped to.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_logical_phase_to_physical_phase(
    int unit,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint8 *physical_phase);

/**
 * \brief
 *      Given a unit and outlif info, returns the number of outlifs of this type that can be allocated
 *      on MDB clusters on the device.
 *      This function is used mainly for testing.
 *
 * \param [in] unit - Unit-ID
 * \param [in] outlif_info - The number of potential outlif for this outlif info.
 * \param [out] nof_potential_outlifs - Number of outlifs that can be allocated on MDB clusters for this outlif info.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_get_potential_nof_mdb_outlifs_for_entry(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *nof_potential_outlifs);

/**
 * \brief
 *      Given an outlif info, including dbal logical table and result type and eedb phase, returns how many outlifs
 *      this entry would occupy. This value depends on the entry size and phase granularity.
 *
 * \param [in] unit - Unit-ID
 * \param [in] outlif_info - See full description in \ref dnx_lif_lib_allocate.
 * \param [out] nof_outlifs_per_entry - This function loads pointed memory with the number of outlifs occupied by this entry.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_info_to_nof_outlifs_per_entry(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *nof_outlifs_per_entry);

/**
 * \brief
 *      Turn the eedb test on or off. When the test is on, all outlif allocations will be done in the eedb banks
 *      rather than the MDB clusters.
 *
 * \param [in] unit - Unit-ID
 * \param [in] value - Set to TRUE to enable EEDB test, set to false to disable it.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_mdb_clusters_disable_set(
    int unit,
    uint8 value);

/**
 * \brief
 *      Turn the mdb clusters test on or off. When the test is on, all outlif allocations will be done in the mdb clusters
 *      rather than the eedb banks.
 *
 * \param [in] unit - Unit-ID
 * \param [in] value - Set to TRUE to enable mdb clusters test, set to false to disable it.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_eedb_data_banks_disable_set(
    int unit,
    uint8 value);

/**
 * \brief
 *      Given a unit, return statistics about the outlif tables utilization. This can be used for diagnostics or tests.
 *
 * \param [in] unit - Unit-ID
 * \param [in] outlif_stats - Outlif stats for this unit.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_stats_get(
    int unit,
    dnx_algo_local_outlif_stats_t * outlif_stats);

/**
 * \brief
 *      returns local phase info
 *
 * \param [in] unit - Unit-ID
 * \param [out] init_info - array of at least two elements, contains phase address granularity.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_rif_init_info_get(
    int unit,
    int *init_info);

/**
 * \brief
 *   Checks whether the maximum number of global lifs was not exceeded in the device.
 *
 *   While the global lif range is large, the number of global lifs that can actually be allocated
 *   is limited by the size of the tables that they are mapped to, so the resource manager will
 *   never return an out of resource error to indicate we don't have more free HW entries.
 *
 *   The mapping of global to local egress lifs is made in the GLEM table.
 *   Each GLEM entry holds one global lif, so the number of egress global lifs is limited
 *   by the number of entries in the glem.
 *
 *   The number of ingress global lifs is limited by the number of entries in the inlif table,
 *   because every inlif entry holds its own global lif as part of its data. Because we assume
 *   that global lif allocation is called after local inlif allocation, it means that there was
 *   a free local lif. Because of that, we don't check the ingress direction.
 *
 *
 * \param [in] unit - Unit-ID
 * \param [in] direction_flags - DNX_ALGO_LIF_INGRESS/EGRESS
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   dnx_algo_global_lif_allocation_verify
 */
shr_error_e dnx_algo_global_lif_allocation_count_verify(
    int unit,
    uint32 direction_flags);

/**
 * \brief
 *      Updates the global ingress/egress (indicated by direction flags) lif counter
 *      (lif_mngr_db.global_lif_allocation.in/egress_global_lif_counter) by the value
 *      given in diff: 1 for allocation, -1 for deallocation.
 *
 *      Note that the global ingress lif counter is not currently used, but we keep it
 *      in case we'll need it in the future.
 *
 * \param [in] unit - Unit-ID
 * \param [in] direction_flags - DNX_ALGO_LIF_INGRESS/EGRESS
 * \param [in] diff - diff in  lif count. 1 for allocation, -1 for deallocation.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *  dnx_algo_global_lif_allocation_count_verify
 */
shr_error_e dnx_algo_global_lif_allocation_update_count(
    int unit,
    uint32 direction_flags,
    int diff);

/**
 *  \brief
 *     Given a global lif id, returns whether it is a symmetric global lif id (ingress == egress).
 *     Note: If it is not, then ingress and egress global lifs are unrelated
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] global_lif -
 *     The global lif whose mapping we require.
 *   \param [out] is_symmetric -
 *     Indication if this lif is symmetric.
 *  \retval
 *    - Error indication
 */
shr_error_e dnx_algo_global_lif_allocation_lif_id_is_symmetric(
    int unit,
    int global_lif,
    uint8 *is_symmetric);

/**
 * \brief
 *   Get the global lif mapped to this local lif.
 *
 * The structure lif_mapping_local_lif_info_t is defined in:
 * include/soc/dnx/swstate/auto_generated/types/lif_mngr_types.h
 *
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] flags -
 *     Exactly one of DNX_ALGO_LIF_INGRESS or
 *     DNX_ALGO_LIF_EGRESS.
 *   \param [in] local_lif_info -
 *     Info for the local lif whose mapping we require.
 *   \param [in] global_lif -
 *     Int pointer to memory to write output into.
 *     \b As \b output -
 *       This procedure loads pointed memory with the global lif mapped
 *       from the given local lif.
 *   Ingress and egress local to global lif maps in the lif mapping sw state submodule.
 *   \retval _SHR_E_NOT_FOUND if the global lif's mapping
 *           doesn't exist.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mapping_local_to_global_get(
    int unit,
    uint32 flags,
    lif_mapping_local_lif_info_t * local_lif_info,
    int *global_lif);

/**
 * \brief
 *   Check if result type contains link list field
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] table_id -
 *     dbal table id.
 *   \param [in] res_type_idx -
 *     Result type index in the dbal table.
 *   \param [out] has_link_list -
 *     Indication if this result type has link list.
 *  \retval - Error indication
 */
shr_error_e dnx_lif_mngr_eedb_table_rt_has_linked_list(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    uint8 *has_link_list);

/**
 * \brief - Given a unit and a dbal table, returns the default phase number associated with this table.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] outlif_phase - \ref lif_mngr_outlif_phase_e member to check.
 * \param [out] logical_phase - outlif logical phase number associated with this logical table.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_local_outlif_phase_enum_to_logical_phase_num(
    int unit,
    lif_mngr_outlif_phase_e outlif_phase,
    dnx_algo_local_outlif_logical_phase_e * logical_phase);

/*
 * }
 */

#endif /* INCLUDE_LIF_MNGR_H_INCLUDED */
