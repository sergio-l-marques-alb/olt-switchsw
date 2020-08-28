/** \file sw_state_hash_table.h
 *
 * Definitions and prototypes for all common utilities related to hash table.
 */
#ifndef SW_STATE_HASHTABLE_H_INCLUDED
/*
 * { 
 */
#define SW_STATE_HASHTABLE_H_INCLUDED

/*************
* INCLUDES  *
*************/
/** { */
/* must be first */
#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>

/** } */

/*************
 * DEFINES   *
 *************/
/** { */
/**
 * \brief
 * Internal indication for an empty hash table
 */
#define SW_STATE_HASH_TABLE_NULL  UTILEX_U32_MAX
/** } */

/**********************************************/
/********** HASH TABLE MACROS ****************/
/**********************************************/
#define SW_STATE_HTB_CREATE(module_id, htb, init_info, key_type, value_type, is_index, nof_elements, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_create(unit, module_id, &htb, init_info, sizeof(key_type), sizeof(value_type), is_index, nof_elements, alloc_flags))

#define SW_STATE_HTB_CREATE_WITH_SIZE(module_id, htb, init_info, key_size, value_size, is_index, nof_elements, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_create(unit, module_id, &htb, init_info, key_size, value_size, is_index, nof_elements, alloc_flags))

#define SW_STATE_HTB_DESTROY(module_id, htb) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_destroy(unit, module_id, &htb))

#define SW_STATE_HTB_INSERT(module_id, htb, key, data, success) \
    do { \
        if (htb->is_index) { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_entry_add(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key, NULL, (uint32*)data, success)); \
        } else { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_entry_add(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key, (SW_STATE_HASH_TABLE_DATA*)data, NULL, success)); \
        } \
    } while (0)

#define SW_STATE_HTB_FIND(module_id, htb, key, data, found) \
    do { \
        if (htb->is_index) { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_entry_lookup(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key, NULL, (uint32*)data, found)); \
        } else { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_entry_lookup(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key, (SW_STATE_HASH_TABLE_DATA*)data, NULL, found)); \
        } \
    } while(0)

#define SW_STATE_HTB_GET_NEXT(module_id, htb, iter, key, data) \
    do { \
        if (htb->is_index) { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_get_next(unit, module_id, htb, iter, (SW_STATE_HASH_TABLE_KEY*)key, NULL, (uint32*)data)); \
        } else { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_get_next(unit, module_id, htb, iter, (SW_STATE_HASH_TABLE_KEY*)key, (SW_STATE_HASH_TABLE_DATA*)data, NULL)); \
        } \
    } while(0)

#define SW_STATE_HTB_CLEAR(module_id, htb) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_clear(unit, module_id, htb))

#define SW_STATE_HTB_DELETE(module_id, htb, key) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_remove(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key))

#define SW_STATE_HTB_DELETE_ALL(module_id, htb) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_remove_all(unit, module_id, htb))

#define SW_STATE_HTB_PRINT(module_id, htb) \
    SHR_IF_ERR_EXIT(sw_state_htb_print(unit, module_id, htb))

#define SW_STATE_HTB_DEFAULT_PRINT(unit, module_id, htb) \
    sw_state_htb_print(unit, module_id, *(htb))

#define SW_STATE_HTB_INSERT_AT_INDEX(module_id, htb, key, data_indx, success) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_add_at_index(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key, data_indx, success))

#define SW_STATE_HTB_DELETE_BY_INDEX(module_id, htb, data_indx) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_remove_by_index(unit, module_id, htb, data_indx))

#define SW_STATE_HTB_GET_BY_INDEX(module_id, htb, data_indx, key, found) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_get_by_index(unit, module_id, htb, data_indx, (SW_STATE_HASH_TABLE_KEY*)key, found))

#define SW_STATE_HTB_SIZE_GET(init_info, key_type, value_type, is_index) \
    sw_state_hash_table_size_get(init_info, sizeof(key_type), sizeof(value_type), is_index)

/*************
 * MACROS    *
 *************/
/** { */

/**
 * \brief Set hash table iterator to the beginning of the hash table
 */
#define SW_STATE_HASH_TABLE_ITER_SET_BEGIN(iter) ((*iter) = 0)
/**
 * \brief Check whether hash table iterator has reached the end of the hash table
 */
#define SW_STATE_HASH_TABLE_ITER_IS_END(iter)    ((*iter) == UTILEX_U32_MAX)
/**
 * \brief Verify specific hash table is active. If not, software goes to
 * exit with error code.
 * \par DIRECT INPUT:
 *   \param [in] hash_table -
 *      hash table pointer
 * \par INDIRECT INPUT:
 *   SWSTATE system
 * \par DIRECT OUTPUT:
 *   * None
 * \par INDIRECT OUTPUT:
 *   In case of error condition, goto exit with _SHR_E_MEMORY
 * \remark
 *   * 'exit' is assumed to be defined in the caller's scope.
 *   * SHR_FUNC_INIT_VARS(unit) is supposed to have been invoked at the beginning of the procedure
 */
#define SW_STATE_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table) \
  { \
    if (hash_table == NULL) { \
      SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL) ; \
      SHR_EXIT() ; \
    } \
  }
/**
 * \brief Verify specified unit has a legal value. If not, software goes to
 * exit with error code.
 * \par DIRECT INPUT:
 *   \param [in] _unit -
 *     Identifier of unit to handle
 * \par INDIRECT INPUT:
 *   SOC_MAX_NUM_DEVICES
 *   \ref shr_error_e
 * \par DIRECT OUTPUT:
 *   * None
 * \par INDIRECT OUTPUT:
 *   In case of error condition, goto exit with _SHR_E_PARAM
 * \remark
 *   * 'exit' is assumed to be defined in the caller's scope.
 *   * SHR_FUNC_INIT_VARS(unit) is supposed to have been invoked at the beginning of the procedure
 */
#define SW_STATE_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(_unit) \
  if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    SHR_SET_CURRENT_ERR(_SHR_E_PARAM) ; \
    SHR_EXIT() ; \
  }

/** } */

/*************
 * TYPE DEFS *
 *************/
/** { */

typedef uint32 SW_STATE_HASH_TABLE_PTR;

/**
 * Type of the hash table key
 */
typedef uint8 SW_STATE_HASH_TABLE_KEY;

/**
 * \brief Type of the hash table data
 */
typedef uint8 SW_STATE_HASH_TABLE_DATA;
/*
 * the ADT of hash table, use this type to manipulate the hash table. 
 * just a workaround to allow cyclic reference from hash func to hash info and vice versa
 */

/**
 * \brief Iterator over the hash table, use this type to traverse the hash table.
 */
typedef uint32 SW_STATE_HASH_TABLE_ITER;

typedef uint32 (
    *SW_STATE_HASH_MAP_HASH_FUN_CALL_BACK) (
    SW_STATE_HASH_TABLE_KEY * const key,
    uint32 seed,
    uint32 *hash_val);
/*
 * STRUCTs
 */

/**
 * Includes the information user has to supply for hash table creation
 * \see
 * SW_STATE_HASH_TABLE_INFO
 * sw_state_hash_table_init
 * sw_state_hash_table_create
 */
typedef struct
{
    /**
     * maximum number of elements to insert to the hash table
     */
    uint32 max_nof_elements;
    /**
     * Expected number of elements.
     */
    uint32 expected_nof_elements;

    /**
     * Hash functions used to map from key to hash value (number)
     * Set to NULL (or don't touch after clear) to use the default hash function.
     */
    SW_STATE_HASH_MAP_HASH_FUN_CALL_BACK hash_function;

    char *print_cb_name;

} sw_state_htbl_init_info_t;

/**
 * Structure at the root of each hash table, containg the table (lists_head),
 * the keys (keys) and 'next' elements for each lists_head entry which contains
 * more than one key (more than one hit).
 * All pointers refer to place on OCC (occupation bit map) table.
 * \see
 * sw_state_hash_table_entry_add()
 */
typedef struct
{
    /**
     * Array to include hash table entries. Index of this array is
     * the HASH VALUE of a specific key. Value is the place (index) on corresponding
     * OCC table (bit map).
     */
    DNX_SW_STATE_BUFF *lists_head;
    /**
     * Array of full keys. Index is 'place on OCC' (OCC stands for Occupation Bit Map)
     */
    DNX_SW_STATE_BUFF *keys;
    /**
     * Array of full data. Index is 'place on OCC' (OCC stands for Occupation Bit Map)
     */
    DNX_SW_STATE_BUFF *data;
    /**
     * Array of 'pointer's to the next in the list.
     * Index is 'place on OCC' (OCC stands for Occupation Bit Map)
     * Value (pointer) is also 'place on OCC' (of the 'next' match to
     * the original hash value).
     */
    DNX_SW_STATE_BUFF *next;
    /**
     * The size of one pointer in bytes.
     */
    uint32 ptr_size;

    /**
     * Size of the key (in bytes)
     */
    uint32 key_size;
    /**
     * Size of the data (in bytes)
     */
    uint32 data_size;

    /**
     * Mapping of hash table memory, for efficient manipulation.
     * Each hash table entry is represented by one bit.
     */
    sw_state_occ_bm_t memory_use;
    /**
     * Value assigned to a 'null pointer' (empty entry).
     */
    uint32 null_ptr;

    /*
     * There are two types of hashtables. In the first one (index) the data is the index of key in the table of keys.
     * In the second, the data determined by the user of the hashtable. 
     */
    uint8 is_index;

    char print_cb_name[SW_STATE_CB_DB_NAME_STR_SIZE];

    sw_state_cb_t print_cb_db;

    sw_state_htbl_init_info_t init_info;
}  *sw_state_htbl_t;

/** } */

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
 *   Creates a new Hash table instance.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     Pointer to memory to load output into. \n
 *     \b As \b output - \n
 *       This procedure loads pointed memory by handle of newly created
 *       hash table
 *   \param [in] init_info -
 *     Pointed memory contains setup parameters required for for the
 *     creation of the hash table. See \ref sw_state_htbl_init_info_t
 *   \param [in] key_size - key size in bytes
 *   \param [in] data_size - data size in bytes
 *   \param [in] is_index - specify if the hash table is index hash table or data (regular) hash table
 *   \param [in] nof_elements - Hash table nof members. Specified in the relevant sw state xml file.
 *   \param [in] alloc_flags - SW state allocation flags.
 *          This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *hash_table \n
 *     See 'hash_table' in DIRECT INPUT above
 * \see
 *   * sw_state_hash_table_init()
 */
shr_error_e sw_state_hash_table_create(
    int unit,
    uint32 module_id,
    sw_state_htbl_t * hash_table,
    sw_state_htbl_init_info_t * init_info,
    uint32 key_size,
    uint32 data_size,
    uint8 is_index,
    uint32 nof_elements,
    uint32 alloc_flags);
/**
 * \brief
 *   Free the specified hash table instance.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table_ptr -
 *     Pointer of the Hash table to destroy.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   All resources of specified hash table are freed.
 * \see
 *   * sw_state_hash_table_create()
 */
shr_error_e sw_state_hash_table_destroy(
    int unit,
    uint32 module_id,
    sw_state_htbl_t * hash_table_ptr);

/**
 * \brief
 *   Insert an entry into the hash table, if entry already exist then
 *   the operation succeeds and returned values refers to the entry
 *   which already exists.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer of the Hash table to add a key to.
 *   \param [in] key -
 *     The key to add into the hash table.
 *   \param [in] data -
 *     The data to add into the hash table.
 *   \param [in] data_indx -
 *     Pointer to memory to load output into. \n
 *     \b As \b output - \n
 *       Pointed memory is to be loaded by index (identifier) of the newly added   \n
 *       key in the hash table. This is, essentially, the data carried by the key  \n
 *       If key already exists, pointed memory is loaded by its index (identifier) \n
 *       If '*success' is loaded by '0' then *data_indx is set to SW_STATE_HASH_TABLE_NULL \n
 *   \param [in] success -
 *     Pointer to memory to load output into. \n
 *     \b As \b output - \n
 *       Pointed memory is to be loaded by              \n
 *       * TRUE if operation was completed to ssuccess (new key was added or aleady existing key was found)  \n
 *       * FALSE if operation has failed
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *data_indx -\n
 *     See 'data_indx' in DIRECT INPUT above.
 *   \b *success -\n
 *     See 'success' in DIRECT INPUT above.
 * \see
 *   * sw_state_hash_table_entry_add_at_index()
 *   * sw_state_hash_table_entry_remove()
 */
shr_error_e sw_state_hash_table_entry_add(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key,
    SW_STATE_HASH_TABLE_DATA * const data,
    uint32 *data_indx,
    uint8 *success);
/**
 * \brief
 *   Insert an entry into the hash table at specified index, entry
 *   must either already exist (with the same key) or specified index must
 *   be free.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the Hash table to add a key to.
 *   \param [in] key -
 *     The key to add into the hash table.
 *   \param [in] data_indx -
 *     If key is not in the table, this is the index (identifier)
 *     of the newly added entry. Otherwise, it must match index of
 *     existing  key.
 *   \param [in] success -
 *     Pointer to memory to load output into. \n
 *     \b As \b output - \n
 *       Pointed memory is to be loaded by              \n
 *       * TRUE if operation was completed to ssuccess (new key was added or aleady existing key was found)  \n
 *       * FALSE if operation has failed
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *success -\n
 *     See 'success' in DIRECT INPUT above
 * \see
 *   * sw_state_hash_table_entry_add_at_index()
 *   * sw_state_hash_table_entry_remove()
 */
shr_error_e sw_state_hash_table_entry_add_at_index(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key,
    uint32 data_indx,
    uint8 *success);
/**
 * \brief
 *   Remove an entry (one key) from a hash table, if the key does
 *   not exist then the operation has no effect (No error is
 *   reported).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the hash table to remove entry from.
 *   \param [in] key -
 *     Pointer to an array of uint8s containing the key to remove.
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e sw_state_hash_table_entry_remove(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key);
/**
* \brief
*   Remove all entries from the hash table.
*
* \param [in] unit - Identifier of the device to access.
* \param [in] module_id - Module id.
* \param [in] hash_table - pointer to the hash table to remove entry from.
*
* \return
*   shr_error_e
*
* \remark
*    None
* \see
*    sw_state_hash_table_entry_remove
*    shr_error_e
*/
shr_error_e sw_state_hash_table_entry_remove_all(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table);

/**
 * \brief
 *   Remove an entry from a hash table given the index identifying
 *   this entry. if there is no entry on specified index then
 *   the operation has no effect (No error is reported).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the hash table to remove entry from.
 *   \param [in] data_indx -
 *     Index of the entry to remove. (Index is on OCC bit map)
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e sw_state_hash_table_entry_remove_by_index(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    uint32 data_indx);
/**
 * \brief
 *   Lookup the hash table for the given key and return the index
 *   (unique) identifier of the given key.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the hash table to look for the key in.
 *   \param [in] key -
 *     Pointer to an array of uint8s containing the key to look for.
 *   \param [in] data    - pointer to be filled with  content of 
 *      the found entry
 *   \param [in] data_indx -
 *     Poniter to memory to be loaded by this procedure by the unique
 *     index id associated with the given key, valid only if
 *     'found' is true.
 *   \param [in] found -
 *     Pointer to memory to be loaded by this procedure by indication
 *     on whether the key was found in the hash table (TRUE) or not (FALSE)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *data (See 'data' on DIRECT INPUT above).    \n
 *   \b *found (See 'found' on DIRECT INPUT above).  \n
 */
shr_error_e sw_state_hash_table_entry_lookup(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key,
    SW_STATE_HASH_TABLE_DATA * data,
    uint32 *data_indx,
    uint8 *found);

/**
 * \brief
 *   Get a key from a hash table given the index identifying
 *   this entry.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the hash table to remove entry from.
 *   \param [in] data_indx -
 *     Index of the key (entry) to find and retrieve. (Index is on OCC bit map)
 *   \param [in] key -
 *     Pointer to an array of uint8s to be loaded, by this procedure, by
 *     the key (in case it has been found)
 *   \param [in] found -
 *     Pointer to memory to be loaded by this procedure by indication
 *     on whether a key was found on 'data_indx' in the hash table
 *     (TRUE) or not (FALSE)
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *found (See 'data' on DIRECT INPUT above).
 *   \b *key (See 'key' on DIRECT INPUT above).
 */
shr_error_e sw_state_hash_table_get_by_index(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    uint32 data_indx,
    SW_STATE_HASH_TABLE_KEY * key,
    uint8 *found);

/**
 * \brief
 *   Clear the indicated Hash table's contents without releasing the memory.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the hash table to clear contents of.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Cleared memory on SWSTATE buffer: Both directly on memory assigned
 *   to hash table and on corresponding occupation bit map.
 * \remarks
 *   The cleared memory (of hash table) is accessed by:     \n
 *   sw_state_hash_table[unit]    \n
 *   The Occupation bit map is cleared using: sw_state occupation bitmap clear.
 */
shr_error_e sw_state_hash_table_clear(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table);

/**
 * \brief
 *   Get the next valid entry (key and data) in the hash table.
 *   Start traversing from the place pointed by the given iterator.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the hash table to search.
 *   \param [in] iter -
 *     Pointer to index into the table: 'interator'. \n
 *     \b As \b input: \n
 *         Iterator points to the entry to start traverse from. \n
 *     \b As \b output: \n
 *         This is the index into the table which is the one following
 *         the valid entry (valid key) which was found. If no valid
 *         entry was found, *iterator is loaded by UTILEX_U32_MAX.
 *         Note that *iter may be loaded by 'max_nof_elements' (namely, one
 *         index following the last on the hash table!).
 *         See \ref sw_state_htbl_init_info_t
 *   \param [in] key -
 *     Pointer. This procedure loads pointed memory by the 'key' found
 *     on the 'next' valid entry.That is, this is the key at entry
 *     whose index is *data_indx. This output is NOT valid if *iter is
 *     loaded by UTILEX_U32_MAX. \n
 *     Note:  The number of bytes in a key is 'key_size. See \ref sw_state_htbl_init_info_t
 *   \param [in] data - Pointer. This procedure loads pointed memory by the 'data' found ...
 *   \param [in] data_indx -
 *     Pointer. This procedure loads pointed memory by the 'index' found
 *     on the 'next' valid entry. That is, this is the index at entry
 *     whose key is *key. This output is NOT valid if *iter is
 *     loaded by UTILEX_U32_MAX
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *iter - \n
 *     See 'iter' on DIRECT INPUT
 *   \b *key - \n
 *     See 'key' on DIRECT INPUT
 *   \b *data - \n
 *     See 'data' on DIRECT INPUT
 *   \b *data_indx - \n
 *     See 'data_indx' on DIRECT INPUT
 * \remarks
 */
shr_error_e sw_state_hash_table_get_next(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_ITER * iter,
    SW_STATE_HASH_TABLE_KEY * const key,
    SW_STATE_HASH_TABLE_DATA * const data,
    uint32 *data_indx);

/**
 * \brief
 *   Clear the 'info' section of a hash table. Used as a necessary
 *   step in the creation of a new table.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the hash table to remove entry from.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Erased entry (corresponding to hash_table) in
 *   sw_state_hash_table[unit].hashs_array.init_info[]
 */
shr_error_e sw_state_hash_table_info_clear(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table);

/**
 * \brief
 *   Prints the hash table contents, all active entries, NOT including
 *   empty entries.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the hash table to print.
  * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Printed hash table.
 * \remarks
 *   LOG_CLI is used for the printing.
 */
shr_error_e sw_state_htb_print(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table);

/**
 * \brief
 *   Gets the size of a new hash table instance.
 *
 *   \param [in] init_info - Pointed memory contains setup parameters required for for the
 *     creation of the hash table. See \ref sw_state_htbl_init_info_t
 *   \param [in] key_size - key size in bytes
 *   \param [in] data_size - data size in bytes
 *   \param [in] is_index - specify if the hash table is index hash table or data (regular) hash table
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int sw_state_hash_table_size_get(
    sw_state_htbl_init_info_t * init_info,
    uint32 key_size,
    uint32 data_size,
    uint8 is_index);

#ifdef SW_STATE_DEBUG
/*
 * { 
 */
/**
 * \brief
 *   Get the number of active entries on specified hash table.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the hash table to get num entries on.
 *   \param [in] num_active_ptr -
 *     Pointer to be loaded by output.\n
 *     \b As \b Output - \n
 *       This procedure loads pointed memory by the number of active
 *       entries on specified hash table.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *num_active_ptr - See 'num_active_ptr' in DIRECT INPUT above.
 */
shr_error_e sw_state_hash_table_get_num_active(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    uint32 *num_active_ptr);

/**
 * \brief
 *   Prints the control (info) data corresponding to specified hash table.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] hash_table -
 *     pointer to the hash table to print.
  * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Printed 'info' structure. Only relevant entries are printed
 *   (and not, say, elements which are not used by current code).
 * \remarks
 *   LOG_CLI is used for the printing.
 */
shr_error_e sw_state_hash_table_info_print(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table);

/**
 * \brief
 *   Test hash table, Create, fill, verify number of active, 
 *   verify actions that should fail, delete, etc.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Printed hash table.
 * \remarks
 *   * LOG_CLI is used for the printing.
 *   * In the end, newly created hash table is destroyed.
 */
shr_error_e sw_state_htbl_test_1(
    int unit);
/*
 * } 
 */
#endif /* SW_STATE_DEBUG */

/*
 * } 
 */

/*
 * } SW_STATE_HASHTABLE_H_INCLUDED
 */
#endif
