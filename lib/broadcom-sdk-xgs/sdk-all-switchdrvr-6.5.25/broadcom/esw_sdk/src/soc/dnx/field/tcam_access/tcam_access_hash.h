/**
 * \file       tcam_access_hash.h
 * $Id$
 *   this file implements hashing logic to map TCAM keys
 *   to their corresponding location in TCAM.
 */
/**
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef TCAM_ACCESS_HASH_H_INCLUDED
/** { */
#define TCAM_ACCESS_HASH_H_INCLUDED

#include <include/soc/dnx/dbal/dbal_structures.h>

/**
 * \brief
 *   Inserts given key at given index in hash table of the given bank_id.
 *
 * \param [in] unit       - unit
 * \param [in] core       - Core ID
 * \param [in] bank_id    - Bank to insert key for
 * \param [in] key        - The entry's key to add for the hash table
 * \param [in] key_mask   - The entry's mask to add
 * \param [in] key_size   - Key size to add
 * \param [in] index      - The index at the bank to insert key to
 *
 * \return
 *  \retval _SHR_E_NONE - success
 *  \retval _SHR_E_FOUND - Key/mask pair already in hash table
 */
shr_error_e dnx_field_tcam_access_hash_key_insert_at_index(
    int unit,
    int core,
    uint32 bank_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 index);

/**
 * \brief
 *   Searches for the given key inside the hash table of the given bank_id.
 *
 * \param [in] unit       - unit
 * \param [in] core       - Core ID
 * \param [in] bank_id    - Bank to search key in
 * \param [in] key        - The entry's key to search in the hash table
 * \param [in] key_mask   - The entry's mask to search
 * \param [in] key_size   - Key size to search
 * \param [out] index     - The index the entry is found on (only valid when entry is found)
 * \param [out] found     - Whether the entry was found on the hash table or not
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_access_hash_key_find(
    int unit,
    int core,
    uint32 bank_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 *index,
    uint8 *found);

/**
 * \brief
 *   Deletes the entry at given bank_offset in hash table of the given bank_id.
 *
 * \param [in] unit        - unit
 * \param [in] core        - Core ID
 * \param [in] bank_id     - Bank to delete entry from
 * \param [in] bank_offset - Bank offset of the entry to delete
 * \param [in] key_size    - Key size of the entry to delete
 *
 * \return
 *  \retval _SHR_E_NONE - success
 *  \retval _SHR_E_NOT_FOUND - Key/mask pair not found in hash table
 */
shr_error_e dnx_field_tcam_access_hash_delete(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 key_size);

/** } */
#endif
