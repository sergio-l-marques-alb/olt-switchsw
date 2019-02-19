/** \file sw_state_multi_set.h
 *
 * Definitions and prototypes for all common utilities related to multi set.
 *
 * A multi-set is essentially a hash table with control over
 * the number of duplications (reference count) both per member and
 * over the total number of duplications.
 */
#ifndef SW_STATE_MULTI_SET_H_INCLUDED
/** { */
#define SW_STATE_MULTI_SET_H_INCLUDED

/*************
* INCLUDES  *
*************/
/*
 * {
 */
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_common.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/sw_state_hash_table.h>
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
 * See, for example, sw_state_multi_set_member_remove()
 */
#define SW_STATE_MULTI_SET_NULL    UTILEX_U32_MAX
/** } */

/**********************************************/
/************ MULTI SET MACROS ****************/
/**********************************************/
/**
 * {
 */

#define SW_STATE_MULTI_SET_CREATE(module_id, multi_set, init_info)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_create(unit, module_id, &multi_set, init_info))

#define SW_STATE_MULTI_SET_DESTROY(module_id, multi_set)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_destroy(unit, module_id, &(multi_set)))

#define SW_STATE_MULTI_SET_ADD(module_id, multi_set, key, data_indx, first_appear, success)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_member_add(unit, module_id, multi_set, key, data_indx, first_appear, success))

#define SW_STATE_MULTI_SET_ADD_AT_INDEX(module_id, multi_set, key, data_indx, first_appear, success)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_member_add_at_index(unit, module_id, multi_set, key, data_indx, first_appear, success))

#define SW_STATE_MULTI_SET_ADD_AT_INDEX_MULTIPLE(module_id, multi_set, key, data_indx, nof_additions, first_appear, success)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_member_add_at_index_nof_additions(unit, module_id, multi_set, key, data_indx, nof_additions, first_appear, success))

#define SW_STATE_MULTI_SET_REMOVE(module_id, multi_set, key, data_indx, last_appear)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_member_remove(unit, module_id, multi_set, key, data_indx, last_appear))

#define SW_STATE_MULTI_SET_REMOVE_BY_INDEX(module_id, multi_set, data_indx, last_appear)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_member_remove_by_index(unit, module_id, multi_set, data_indx, last_appear))

#define SW_STATE_MULTI_SET_REMOVE_BY_INDEX_MULTIPLE(module_id, multi_set, data_indx, remove_amount, last_appear)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_member_remove_by_index_multiple(unit, module_id, multi_set, data_indx, remove_amount, last_appear))

#define SW_STATE_MULTI_SET_FIND(module_is, multi_set, key, data_indx, ref_count)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_member_lookup(unit, module_is, multi_set, key, data_indx, ref_count))

#define SW_STATE_MULTI_SET_GET_NEXT(module_id, multi_set, iter, key, data_indx, ref_count)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_get_next(unit, module_id, multi_set, iter, key, data_indx, ref_count))

#define SW_STATE_MULTI_SET_GET_BY_INDEX(module_id, multi_set, data_indx, key, ref_count)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_get_by_index(unit, module_id, multi_set, data_indx, key, ref_count))

#define SW_STATE_MULTI_SET_CLEAR(module_id, multi_set)\
    SHR_IF_ERR_EXIT(sw_state_multi_set_clear(unit, module_id, multi_set))

#define SW_STATE_MULTI_SET_PRINT(module_id, multi_set, clear_on_print)\
        SHR_IF_ERR_EXIT(sw_state_multi_set_print(unit, module_id, multi_set, clear_on_print))

/**
 * }
 */
/*************
 * MACROS    *
 *************/
/** { */
/**
 * Reset the multiset iterator to point to the beginning of the multiset
 */
#define SW_STATE_MULTI_SET_ITER_BEGIN(iter) (iter = 0)
/**
 * Check whether the multiset iterator has reached the end of the multiset
 */
#define SW_STATE_MULTI_SET_ITER_END(iter)   (iter == UTILEX_U32_MAX)
/**
 * Verify specific multi set is active. If not, software goes to
 * exit with error code.
 */
#define SW_STATE_MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(_multi_set) \
  { \
    if (_multi_set == NULL) { \
      SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL) ; \
    } \
  }
/**
 * Verify specified unit has a legal value. If not, software goes to
 * exit with error code.
 *
 * Notes:
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define SW_STATE_MULTI_SET_VERIFY_UNIT_IS_LEGAL(_unit) \
  if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    SHR_IF_ERR_EXIT(_SHR_E_UNIT) ; \
  }

/** } */

/*************
 * TYPE DEFS *
 *************/
/** { */

/**
 * Type of the multiset key. Key is, essentially, an array of SW_STATE_MULTI_SET_KEY.
 *
 */
typedef uint8 SW_STATE_MULTI_SET_KEY;
/**
 * Type of the multiset data
 */
typedef uint8 *SW_STATE_MULTI_SET_DATA;
/**
 * Type of iterator over the multiset. Use this type to traverse the multiset.
 */
typedef uint32 SW_STATE_MULTI_SET_ITER;
/**
 * Note: SW_STATE_MULTI_SET_PTR is just a handle to the 'multi set'
 * structure (actually, index into 'multis_array' {of pointers})
 *
 * Note that the name is kept as is to minimize changes in current code relative
 * to the original which has been ported.
 */
typedef uint32 SW_STATE_MULTI_SET_PTR;

/**
 * Includes the information user needs to supply for multiset creation
 * \see sw_state_multi_set_create()
 */
typedef struct
{
    /**
     * Number of different members can be added to the set.
     * A member can be any values in the range
     * 0,1,...,nof_members - 1
     */
    uint32 nof_members;
    /**
     * Size of a member (in bytes). For this implementation, this is the size of the key
     * on the corresponding hash table.
     */
    uint32 member_size;
    /**
     * The maximum duplications/occurrences of a member in the multi_set.
     * If set to '1' then the multi_set act as Set i.e.
     * no matter how many time an element was added,
     * one 'remove' operation will remove it from the set.
     * Otherwise, a member is not removed till
     * all the 'add' operation reverted with 'remove' operation
     * i.e. # removes = # add.
     */
    uint32 max_duplications;
    /**
     * Procedures to move entries to/from software DB. Not used
     * on this implementation.
     */
    char *print_cb_name;
} sw_state_multi_set_info_t;

/**
 * Includes the information software updates for a LIVE multiset while various
 * operations (such as 'add' or 'remove') are applied.
 * \see sw_state_multimet_info_t
 */
typedef struct
{
    /**
     * Array to include reference counting of the used members.
     */
    DNX_SW_STATE_BUFF *ref_counter;
    /**
     * The size of the reference counter in bytes. This is
     * the number of bytes used for storage of ref_counter. It
     * is calculated using 'max_duplications'. \see sw_state_multi_set_create()
     */
    uint32 counter_size;
    /**
     * Size of the key (in bytes)
     */
    uint32 key_size;
    /**
     * Size of the data (in bytes)
     */
    uint32 data_size;
    /**
     * Hash table structure.
     */
    sw_state_htbl_t hash_table;
    /**
     * Includes the information user needs to supply for multiset creation
     */
    sw_state_multi_set_info_t init_info;
}  *sw_state_multi_set_t;

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
 * \brief - Creates a new Multi set instance.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - Multi set to be created.
 * \param [in] init_info - Pointed memory contains setup parameters required for for the creation of the hash table.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_create(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t * multi_set,
    sw_state_multi_set_info_t * init_info);

/**
 * \brief - Free the multiset instance
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - Pointer to multi set to be destroyed.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_destroy(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t * multi_set);

/**
 * \brief - Add a member to the mutli-set. If member already exists then
 *   increment its reference counter (number occurrences of this member).
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - Multi set.
 * \param [in] key - Points to the identifying key of the new/existing member.
 *       Key is an array of uint8s.
 *       This is referred to as the 'data' of the member.
 * \param [out] data_indx - This procedure loads pointed memory by the identifying index of the
 *       new member. (This is, essentailly, index into the array where all
 *       members are stored).
 * \param [out] first_appear - This procedure loads pointed memory by a zero value if a
 *       member corresponding to the specified 'key' already exists.
 * \param [out] success - This procedure loads pointed memory by a non-zero value if this
 *       operation was successful. Succes is flagged if
 *       * New member was added or, either
 *         * 'max_duplications' is '1' and existing member was found or
 *         * 'max_duplications' is larger than '1' and existing member was found and \n
 *           its 'ref_counter' was increased by '1'.
 *       \see sw_state_multi_set_member_add_internal() for full list of cases where
 *       *success will be loaded by '0'.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_member_add(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set,
    SW_STATE_MULTI_SET_KEY * const key,
    uint32 *data_indx,
    uint8 *first_appear,
    uint8 *success);

/**
 * \brief - Add a member to the mutli-set at a specific index.
 *     If the id ('data_indx') does not exist then a new member is \n
 *     allocated with the supplied key (reference counter = 1).
 *     If it DOES exist, then its key is retrieved and verified against \n
 *     input 'key'.
 *       If there is a match then reference counter of specified member \n
 *       is incremented.
 *       Otherwise, an error is flagged
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - Multi set.
 * \param [in] key - Points to the identifying key of the new/existing member. See general description above.
 * \param [in] data_indx - This is the identifying index of the new member to add
 * \param [out] first_appear - This procedure loads pointed memory by a zero value if a
 *       member corresponding to the specified 'key' already exists.
 * \param [out] success - This procedure loads pointed memory by a non-zero value if this
 *       operation was successful. Succes is flagged if New member was added or, either
 *         'max_duplications' is '1' and existing member was found or
 *         'max_duplications' is larger than '1' and existing member was found and \n
 *           its 'ref_counter' was increased by '1'.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_member_add_at_index(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set,
    SW_STATE_MULTI_SET_KEY * const key,
    uint32 data_indx,
    uint8 *first_appear,
    uint8 *success);

/**
 * \brief - Add nof members to the mutli-set to a specific index, if already exist then
 *  then update the occurences of this member.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - Multi set.
 * \param [in] key - Points to the identifying key of the new/existing member. See general description above.
 * \param [in] nof_additions - Declare nof_additions to add the given key.
 * \param [in] data_indx - This is the identifying index of the new member to add.
 * \param [out] first_appear - This procedure loads pointed memory by a zero value if a
 *       member corresponding to the specified 'key' already exists.
 * \param [out] success - This procedure loads pointed memory by a non-zero value if this
 *       operation was successful. Succes is flagged if New member was added or, either
 *         'max_duplications' is '1' and existing member was found or
 *         'max_duplications' is larger than '1' and existing member was found and \n
 *           its 'ref_counter' was increased by '1'.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_member_add_at_index_nof_additions(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set,
    SW_STATE_MULTI_SET_KEY * const key,
    uint32 data_indx,
    uint32 nof_additions,
    uint8 *first_appear,
    uint8 *success);

/**
 * \brief - Remove a member from a mutli-set, if the member is not belong to the multi-set
 *  the operation has no effect.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - Multi set.
 * \param [in] key - Points to the identifying key of the new/existing member. See general description above.
 * \param [in] data_indx - This is the identifying index of the new member to add.
 * \param [out] last_appear - Whether this was the last occurrence of this key in the multiset.
*     so no more occurrences of this key after this remove.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_member_remove(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set,
    SW_STATE_MULTI_SET_KEY * const key,
    uint32 *data_indx,
    uint8 *last_appear);

/**
 * \brief - Remove a member from a mutli-set according the identifying index.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - Multi set.
 * \param [in] data_indx - This is the identifying index of the new member to add.
 * \param [out] last_appear - This procedure loads pointed memory by a zero value if a
 *       member corresponding to the specified 'key' already exists.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_member_remove_by_index(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set,
    uint32 data_indx,
    uint8 *last_appear);

/**
 * \brief - Like remove single member by index but done multiple times (without using a loop).
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] data_indx - This is the identifying index of the new member to add.
 * \param [in] remove_amount - How many members should be removed.
 * \param [out] last_appear - This procedure loads pointed memory by a zero value if a
 *       member corresponding to the specified 'key' already exists.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_member_remove_by_index_multiple(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set,
    uint32 data_indx,
    uint32 remove_amount,
    uint8 *last_appear);

/**
 * \brief - Like remove single member by index but done multiple times (without using a loop).
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] key - The key to lookup.
 * \param [out] data_indx - index identifying this member place.
 * \param [out] ref_count - The occurrences/duplications of this member in the multi-set.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_member_lookup(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set,
    SW_STATE_MULTI_SET_KEY * const key,
    uint32 *data_indx,
    uint32 *ref_count);

/**
 * \brief - get the next valid entry (key and data) in the multiset.
 *  start traversing from the place pointed by the given iterator.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] iter - iterator points to the entry to start traverse from.
 * \param [in] key - The key to lookup.
 * \param [out] data_indx - index identifying this member place.
 * \param [out] ref_count - The occurrences/duplications of this member in the multi-set.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_get_next(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set,
    SW_STATE_MULTI_SET_ITER * iter,
    SW_STATE_MULTI_SET_KEY * key,
    uint32 *data_indx,
    uint32 *ref_count);

/**
 * \brief - Get the number of occurences of an index (zero if does not exist).
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] data_indx - Data index.
 * \param [out] key - the multiset key returned.
 * \param [out] ref_count - The occurrences/duplications of this member in the multi-set.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_get_by_index(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set,
    uint32 data_indx,
    SW_STATE_MULTI_SET_KEY * key,
    uint32 *ref_count);

/**
 * \brief - Clear the Multiset without freeing the memory.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set to clear.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_clear(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set);

/**
 * \brief - Print the Multiset data.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set.
 * \param [in] clear_on_print - falg that specifies if we want to clear the data after printing it.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_multi_set_print(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set,
    uint8 clear_on_print);

/** } */

#endif /* SW_STATE_MULTI_SET_H_INCLUDED */
