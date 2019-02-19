/** \file utilex_multi_set.h
 *
 * Definitions and prototypes for all common utilities related to multi set.
 *
 * A multi-set is essentially a hash table with control over
 * the number of duplications (reference count) both per member and
 * over the total number of duplications. 
 */
#ifndef UTILEX_MULTI_SET_H_INCLUDED
/** { */
#define UTILEX_MULTI_SET_H_INCLUDED

/*************
* INCLUDES  *
*************/
/*
 * { 
 */
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_hashtable.h>
#include <shared/utilex/utilex_common.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/swstate/auto_generated/types/utilex_multi_set_types.h>
/*
 * } 
 */

/*************
 * DEFINES   *
 *************/
/** { */
/**
 * Value assigned to multi-set handle to indicate it is not initialized
 * (empty).
 * See, for example, utilex_multi_set_member_remove()
 */
#define UTILEX_MULTI_SET_NULL    UTILEX_U32_MAX
/** } */

/*************
 * MACROS    *
 *************/
/** { */
#define UTILEX_MULTI_SET_ACCESS          utilex_multi_set_db
#define UTILEX_MULTI_SET_ACCESS_DATA     UTILEX_MULTI_SET_ACCESS.multis_array.multiset_data
#define UTILEX_MULTI_SET_ACCESS_INFO     UTILEX_MULTI_SET_ACCESS.multis_array.init_info
/**
 * Reset the multiset iterator to point to the beginning of the multiset
 */
#define UTILEX_MULTI_SET_ITER_BEGIN(iter) (iter = 0)
/**
 * Check whether the multiset iterator has reached the end of the multiset
 */
#define UTILEX_MULTI_SET_ITER_END(iter)   (iter == UTILEX_U32_MAX)
/**
 * Verify specific multi set index is marked as 'occupied'. If not, software goes to
 * exit with error code.
 * 
 * Notes:
 *   'unit' is assumed to be defined in the caller's scope.
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define UTILEX_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(_multi_set_index) \
  { \
    uint8 bit_val ; \
    uint32 max_nof_multis ; \
    SHR_IF_ERR_EXIT(UTILEX_MULTI_SET_ACCESS.max_nof_multis.get(unit, &max_nof_multis)) ; \
    if (_multi_set_index >= max_nof_multis) \
    { \
      /* \
       * If multiset handle is out of range then quit with error. \
       */ \
      bit_val = 0 ; \
    } \
    else \
    { \
      SHR_IF_ERR_EXIT(UTILEX_MULTI_SET_ACCESS.occupied_multis.bit_get(unit, (int)_multi_set_index, &bit_val)) ; \
    } \
    if (bit_val == 0) \
    { \
      /* \
       * If multiset structure is not indicated as 'occupied' then quit \
       * with error. \
       */ \
      SHR_IF_ERR_EXIT(_SHR_E_MEMORY) ; \
    } \
  }
/**
 * Verify specified unit has a legal value. If not, software goes to
 * exit with error code.
 * 
 * Notes:
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define UTILEX_MULTI_SET_VERIFY_UNIT_IS_LEGAL(_unit) \
  if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    SHR_IF_ERR_EXIT(_SHR_E_UNIT) ; \
  }
/**
 * Convert input multi set handle to index in 'occupied_multis' array.
 * Convert input index in 'occupied_multis' array to multi set handle.
 * Indices go from 0 -> (occupied_multis - 1)
 * Handles go from 1 -> occupied_multis
 */
#define UTILEX_MULTI_SET_CONVERT_HANDLE_TO_MULTISET_INDEX(_multi_set_index,_handle) (_multi_set_index = _handle - 1)
#define UTILEX_MULTI_SET_CONVERT_MULTISET_INDEX_TO_HANDLE(_handle,_multi_set_index) (_handle = _multi_set_index + 1)

/** } */

/*************
 * TYPE DEFS *
 *************/
/** { */

/**
 * Type of the multiset key. Key is, essentially, an array of UTILEX_MULTI_SET_KEY.
 *
 */
typedef uint8 UTILEX_MULTI_SET_KEY;
/**
 * Type of the multiset data
 */
typedef uint8 *UTILEX_MULTI_SET_DATA;
/**
 * Type of iterator over the multiset. Use this type to traverse the multiset.
 */
typedef uint32 UTILEX_MULTI_SET_ITER;
/**
 * Note: UTILEX_MULTI_SET_PTR is just a handle to the 'multi set'
 * structure (actually, index into 'multis_array' {of pointers})
 *
 * Note that the name is kept as is to minimize changes in current code relative
 * to the original which has been ported.
 */
typedef uint32 UTILEX_MULTI_SET_PTR;

/** } */

/*************
* GLOBALS   *
*************/
/** { */

/** } */

/*************
* FUNCTIONS *
*************/
/** { */

/**
 * \brief
 *   Get element 'member_size' for specified multi set instance. (This is
 *   the number of bytes on the 'data' related to each member of the multiset
 *   and, in this implementation, this is the key to the accompanying hash table).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] multi_set -
 *     Handle identifier of the multi set to get info about.
 *   \param [in] member_size_ptr -
 *     Pointer to uint32. \n
 *     \b As \b output - \n
 *       This procedure loads pointed memory by a value indicating the
 *       number of bytes of data assigned to each member of the multi set
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *member_size_ptr \n
 *     See 'member_size_ptr' in DIRECT INPUT above
 * \see
 *   * utilex_multi_set_test_1()
 */
shr_error_e utilex_multi_set_get_member_size(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    uint32 *member_size_ptr);
/**
 * \brief
 *   Initialize control structures for ALL multi set instances expected.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] max_nof_multis -
 *     Maximal number of multi sets which can be sustained simultaneously.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 * \see
 *   * soc_dnx_sw_state_utils_init()
 */
shr_error_e utilex_multi_set_init(
    int unit,
    uint32 max_nof_multis);
/**
 * \brief
 *   Creates a new Multi set instance.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] multi_set_ptr -
 *     Pointer to memory, of type UTILEX_MULTI_SET_PTR, to load output into. \n
 *     \b As \b output - \n
 *       This procedure loads pointed memory by handle of newly created
 *       multi set
 *   \param [in] init_info -
 *     Pointed memory, of type UTILEX_MULTI_SET_INIT_INFO, contains setup
 *     parameters required for for the creation of the multi set.
 *     See \ref UTILEX_MULTI_SET_INIT_INFO for details of each element.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *multi_set \n
 *     See 'multi_set' in DIRECT INPUT above
 * \see
 *   * utilex_multi_set_init()
 */
shr_error_e utilex_multi_set_create(
    int unit,
    UTILEX_MULTI_SET_PTR * multi_set_ptr,
    UTILEX_MULTI_SET_INIT_INFO init_info);

/**
 * \brief
 *   Free the multiset instance (Return all resources allocated for it).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] multi_set -
 *     Handle (identifier) of the multi set to destroy.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 * \see
 *   * utilex_multi_set_test_1()
 */
shr_error_e utilex_multi_set_destroy(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set);

/**
 * \brief
 *   Free all occupied multiset instances (Return all resources allocated for them).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 * \see
 *   * utilex_multi_set_test_1()
 *   * utilex_multi_set_destroy()
 */
shr_error_e utilex_multi_set_destroy_all(
    int unit);

/**
 * \brief
 *   Add a member to the mutli-set. If member already exists then
 *   increment its reference counter (number occurrences of this member).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] multi_set -
 *     Handle (identifier) of the multi set to add a member to (or
 *     just update the reference counter on member which already exists
 *     on).
 *   \param [in] key -
 *     Pointer to UTILEX_MULTI_SET_KEY \n
 *     \b As \b input - \n
 *       Points to the identifying key of the new/existing member.
 *       Key is an array of uint8s.
 *       This is referred to as the 'data' of the member.
 *   \param [in] data_indx -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *       This procedure loads pointed memory by the identifying index of the
 *       new member. (This is, essentailly, index into the array where all
 *       members are stored).
 *   \param [in] first_appear -
 *     Pointer to uint8 \n
 *     \b As \b output - \n
 *       This procedure loads pointed memory by a zero value if a
 *       member corresponding to the specified 'key' already exists.
 *   \param [in] success -
 *     Pointer to uint8 \n
 *     \b As \b output - \n
 *       This procedure loads pointed memory by a non-zero value if this
 *       operation was successful. Succes is flagged if
 *       * New member was added or, either
 *         * 'max_duplications' is '1' and existing member was found or
 *         * 'max_duplications' is larger than '1' and existing member was found and \n
 *           its 'ref_counter' was increased by '1'.
 *       \see utilex_multi_set_member_add_internal() for full list of cases where
 *       *success will be loaded by '0'.
 * \par INDIRECT INPUT
 *   * SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 *   * \b *key \n
 *     See 'key' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 *   * \b *data_indx \n
 *     See 'data_indx' in DIRECT INPUT above
 *   * \b *first_appear \n
 *     See 'first_appear' in DIRECT INPUT above
 *   * \b *success \n
 *     See 'success' in DIRECT INPUT above
 * \see
 *   * utilex_multi_set_member_add_internal()
 */
shr_error_e utilex_multi_set_member_add(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    UTILEX_MULTI_SET_KEY * const key,
    UTILEX_OUT uint32 *data_indx,
    UTILEX_OUT uint8 *first_appear,
    UTILEX_OUT uint8 *success);

/**
 * \brief
 *   Add a member to the mutli-set at a specific index.
 *   * If the id ('data_indx') does not exist then a new member is \n
 *     allocated with the supplied key (reference counter = 1).
 *   * If it DOES exist, then its key is retrieved and verified against \n
 *     input 'key'.
 *     * If there is a match then reference counter of specified member \n
 *       is incremented.
 *     * Otherwise, an error is flagged
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] multi_set -
 *     Handle (identifier) of the multi set to add a member to (or
 *     just update the reference counter on member which already exists
 *     on).
 *   \param [in] key -
 *     Pointer to UTILEX_MULTI_SET_KEY \n
 *     \b As \b input - \n
 *       Points to the identifying key of the new/existing member.
 *       See general description above.
 *   \param [in] data_indx -
 *     This is the identifying index of the new member to add or of an
 *     existing member to increment 'reference counter' of.
 *     See general description above.
 *   \param [in] first_appear -
 *     Pointer to uint8 \n
 *     \b As \b output - \n
 *       This procedure loads pointed memory by a zero value if a
 *       member corresponding to the specified 'key' and 'data_indx'
 *       already exists.
 *   \param [in] success -
 *     Pointer to uint8 \n
 *     \b As \b output - \n
 *       This procedure loads pointed memory by a non-zero value if this
 *       operation was successful. Succes is flagged if
 *       * New member was added or, either
 *         * 'max_duplications' is '1' and existing member was found or
 *         * 'max_duplications' is larger than '1' and existing member was found and \n
 *           its 'ref_counter' was increased by '1'.
 *       \see utilex_multi_set_member_add_internal() for full list of cases where
 *       *success will be loaded by '0'.
 * \par INDIRECT INPUT
 *   * SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 *   * \b *key \n
 *     See 'key' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * SWSTATE system (Specifically, see \ref UTILEX_MULTI_SET_ACCESS)
 *   * \b *first_appear \n
 *     See 'first_appear' in DIRECT INPUT above
 *   * \b *success \n
 *     See 'success' in DIRECT INPUT above
 * \see
 *   * utilex_multi_set_member_add_internal()
 */
shr_error_e utilex_multi_set_member_add_at_index(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    UTILEX_MULTI_SET_KEY * const key,
    uint32 data_indx,
    UTILEX_OUT uint8 *first_appear,
    UTILEX_OUT uint8 *success);

/*********************************************************************
* NAME:
*     utilex_multi_set_member_add_at_index_nof_additions
* TYPE:
*   PROC
* DATE:
*   June 01 2015
* FUNCTION:
*  Add nof members to the mutli-set to a specific index, if already exist then
*  then update the occurences of this member.
* INPUT:
*   int                           unit -
*     Identifier of the device to access.
*   UTILEX_MULTI_SET_PTR     multi_set -
*     The multi-set to add a member to.
*   UTILEX_MULTI_SET_KEY        key -
*     The member to add.
*   UTILEX_OUT  uint32                      *data_indx -
*     Index identifies the place of the added member.
*     the given key
*   uint32                        nof_additions -
*     Declare nof_additions to add the given key.   
*   UTILEX_OUT  uint8                       *success -
*     whether the add operation success, this may be FALSE,
*     if there are no more space available in the multi-set
* Remarks: 
*     Similair to  utilex_multi_set_member_add_at_index,
*     where nof_additions can be more than 1
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e utilex_multi_set_member_add_at_index_nof_additions(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    UTILEX_MULTI_SET_KEY * const key,
    uint32 data_indx,
    uint32 nof_additions,
    UTILEX_OUT uint8 *first_appear,
    UTILEX_OUT uint8 *success);

/*********************************************************************
* NAME:
*     utilex_multi_set_member_remove
* TYPE:
*   PROC
* DATE:
*   June 01 2015
* FUNCTION:
*  Remove a member from a mutli-set, if the member is not belong to the multi-set
*  the operation has no effect.
* INPUT:
*  int                          unit -
*     Identifier of the device to access.
*  UTILEX_MULTI_SET_PTR    multi_set -
*     The mutli-set instance.
*  UTILEX_MULTI_SET_KEY       *const key -
*     The key to remove from the mutli-set
*  UTILEX_OUT  uint32                      *data_indx -
*     where the key was found.
*  UTILEX_OUT  uint8                       *last_appear -
*     whether this was the last occurrence of this key in the multiset.
*     so no more occurrences of this key after this remove.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e utilex_multi_set_member_remove(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    UTILEX_MULTI_SET_KEY * const key,
    UTILEX_OUT uint32 *data_indx,
    UTILEX_OUT uint8 *last_appear);

/*********************************************************************
* NAME:
*     utilex_multi_set_member_remove_by_index
* TYPE:
*   PROC
* DATE:
*   June 01 2015
* FUNCTION:
*  Remove a member from a mutli-set according the
*  identifying index.
* INPUT:
*  int                         unit -
*    Identifier of the device to access.
*  UTILEX_MULTI_SET_PTR   multi_set -
*    The mutli-set instance.
*  uint32                      data_indx -
*    The index identifying the placement of the data
*    in the mult-set.
*  UTILEX_OUT  uint8                      *last_appear -
*     whether this was the last occurrence of this key in the multiset.
*     so no more occurrences of this key after this remove.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e utilex_multi_set_member_remove_by_index(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    uint32 data_indx,
    UTILEX_OUT uint8 *last_appear);

/*********************************************************************
* NAME:
*     utilex_multi_set_member_remove_multiple
* TYPE:
*   PROC
* FUNCTION:
*  Like remove single member by index but done multiple times (without using a loop).
* INPUT:
*  int                        unit -
*     Identifier of the device to access.
*  UTILEX_MULTI_SET_PTR  multi_set -
*     The mutli-set instance.
*  uint32                     data_indx -
*    The index identifying the placement of the data
*    in the mult-set.
*  UTILEX_OUT  uint32                    remove_amt -
*    How many members should be removed
*  UTILEX_OUT  uint8                     *last_appear -
*     whether this was the last occurrence of this key in the multiset.
*     so no more occurrences of this key after this remove.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e utilex_multi_set_member_remove_by_index_multiple(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    uint32 data_indx,
    uint32 remove_amt,
    UTILEX_OUT uint8 *last_appear);

/*********************************************************************
* NAME:
*     utilex_multi_set_member_lookup
* TYPE:
*   PROC
* DATE:
*   June 01 2015
* FUNCTION:
*  lookup in the mutli-set for a member and return the occurrences/duplications of this
*  member in the multi-set, and the index identifying this member place.
*  the given key.
* INPUT:
*  int                        unit -
*     Identifier of the device to access.
*  UTILEX_MULTI_SET_PTR     multi_set -
*     The mutli-set.
*   UTILEX_MULTI_SET_KEY*   const key -
*     The key to lookup
*   UTILEX_OUT  uint32                  *data_indx -
*     index identifying this member place.
*   UTILEX_OUT  uint32                  *ref_count -
*     the occurrences/duplications of this member in the multi-set
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e utilex_multi_set_member_lookup(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    UTILEX_MULTI_SET_KEY * const key,
    UTILEX_OUT uint32 *data_indx,
    UTILEX_OUT uint32 *ref_count);

/*********************************************************************
* NAME:
*     utilex_multi_set_get_next
* TYPE:
*   PROC
* DATE:
*   June 01 2015
* FUNCTION:
*  get the next valid entry (key and data) in the multiset.
*  start traversing from the place pointed by the given iterator.
* INPUT:
*  int                           unit -
*     Identifier of the device to access.
*  UTILEX_MULTI_SET_PTR        multi_set -
*     The multiset.
*  UTILEX_MULTI_SET_ITER    *iter
*     iterator points to the entry to start traverse from.
*   UTILEX_OUT  UTILEX_MULTI_SET_KEY*     const key -
*     the multiset key returned
*   UTILEX_OUT  uint32                      *ref_count -
*     the occurrences/duplications of this member in the multi-set
* REMARKS:
*     - to start traverse the multiset from the beginning.
*       use UTILEX_MULTI_SET_ITER_BEGIN(iter)
*     - to check if the iterator get to the end of the table use.
*       use UTILEX_MULTI_SET_ITER_END(iter)
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e utilex_multi_set_get_next(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    UTILEX_MULTI_SET_ITER * iter,
    UTILEX_OUT UTILEX_MULTI_SET_KEY * key,
    UTILEX_OUT uint32 *data_indx,
    UTILEX_OUT uint32 *ref_count);

/*********************************************************************
* NAME:
*     utilex_multi_set_get_by_index
* TYPE:
*   PROC
* DATE:
*   June 01 2015
* FUNCTION:
*   Get the number of occurences of an index (zero if does not exist).
* INPUT:
*  int                       unit -
*     Identifier of the device to access.
*  UTILEX_MULTI_SET_PTR    multi_set -
*     The multiset.
*  uint32                    data_indx -
*     The index.
*  UTILEX_OUT  UTILEX_MULTI_SET_KEY   *key -
*     the multiset key returned
*  UTILEX_OUT  uint32                   *ref_count -
*     the occurrences/duplications of this member in the multi-set
* REMARKS:
*     - to start traverse the multiset from the beginning.
*       use UTILEX_MULTI_SET_ITER_BEGIN(iter)
*     - to check if the iterator get to the end of the table use.
*       use UTILEX_MULTI_SET_ITER_END(iter)
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e utilex_multi_set_get_by_index(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    uint32 data_indx,
    UTILEX_OUT UTILEX_MULTI_SET_KEY * key,
    UTILEX_OUT uint32 *ref_count);

/*********************************************************************
* NAME:
*     utilex_multi_set_clear
* TYPE:
*   PROC
* DATE:
*   June 01 2015
* FUNCTION:
*     Clear the Multiset without freeing the memory
* INPUT:
*  int                        unit -
*     Identifier of the device to access.
*  UTILEX_MULTI_SET_PTR     multi_set -
*     The mutli-set to clear.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e utilex_multi_set_clear(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set);

#ifdef UTILEX_DEBUG
/*
 * { 
 */
shr_error_e utilex_multi_set_test_1(
    int unit);

/*********************************************************************
* NAME:
*     utilex_multi_set_print
* TYPE:
*   PROC
* DATE:
*   June 01 2015
* FUNCTION:
*     prints the mutli-set content, the members in the multi set, the number of occurrences of
*     each member and the index.
* INPUT:
*  int                          unit -
*     Identifier of the device to access.
*  UTILEX_MULTI_SET_PTR       multi_set -
*     The mutli-set to print.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e utilex_multi_set_print(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set,
    uint8 clear_on_print);

shr_error_e utilex_SAND_MULTI_SET_INFO_print(
    int unit,
    UTILEX_MULTI_SET_PTR multi_set);
/*
 * } 
 */
#endif /* UTILEX_DEBUG */

/** } */

#else
/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e utilex_multi_set_dummy_empty_function_to_make_compiler_happy(
    int unit);

#endif

/** } UTILEX_MULTI_SET_H_INCLUDED */
#endif
