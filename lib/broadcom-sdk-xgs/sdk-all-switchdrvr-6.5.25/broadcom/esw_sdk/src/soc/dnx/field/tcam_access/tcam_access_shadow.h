/**
 * \file       tcam_access_shadow.h
 * $Id$
 *   this file implements shadow logic for saving TCAM data
 *   in SW cache.
 */
/**
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef TCAM_ACCESS_SHADOW_H_INCLUDED
/** { */
#define TCAM_ACCESS_SHADOW_H_INCLUDED

#include <include/soc/dnx/dbal/dbal_structures.h>

/**
 * \brief
 *  Reads a single size key/key_mask from the shadow of the given core
 *
 * \param [in] unit              - Device ID
 * \param [in] core              - Core ID
 * \param [in] bank_id           - The bank ID to read from
 * \param [in] tcam_bank_offset  - The offset inside the bank to read ifrom, twice the action bank offset.
 * \param [out] data             - The buffer to copy read data to.
 * \param [out] valid_bits       - The entry's valid bits.
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL INPUT:
 *  * None.
 */
shr_error_e dnx_field_tcam_access_entry_key_read_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 tcam_bank_offset,
    uint32 *data,
    uint8 *valid_bits);

/**
 * \brief
 *  Writes a single size key/key_mask to the shadow of the given core
 *
 * \param [in] unit              - Device ID
 * \param [in] core              - Core ID
 * \param [in] bank_id           - The bank ID to write to
 * \param [in] tcam_bank_offset  - The offset inside the bank to write to, twice the action bank offset.
 * \param [in] valid             - 2 bit valid indication for both key parts
 * \param [in] part              - The part of the entry to write to.
 * \param [in] data              - The buffer to copy write data to.

 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL INPUT:
 *  * Writes to TCAM shadow.
 */
shr_error_e dnx_field_tcam_access_entry_key_write_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 tcam_bank_offset,
    int valid,
    dnx_field_tcam_access_part_e part,
    uint32 *data);

/**
 * \brief
 *  Reads a single entry payload from the shadow of the given core
 *
 * \param [in] unit               - Device ID
 * \param [in] core               - Core ID
 * \param [in] action_bank_id     - The actiont able to write to (there are two for each bank ID).
 * \param [in] action_bank_offset - The offset inside the bank to read from. Half the offset in the TCAM bank.
 * \param [out] payload           - The buffer to copy read data to (should be
 *                                  pre-allocated to at least standard payload size)
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 *  * None.
 */
shr_error_e dnx_field_tcam_access_entry_payload_read_shadow(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 action_bank_offset,
    uint32 *payload);

/**
 * \brief
 *  Writes a single size payload to the shadow of the given core
 *
 * \param [in] unit               - Device ID
 * \param [in] core               - Core ID
 * \param [in] action_bank_id     - The actiont able to write to (there are two for each bank ID).
 * \param [in] action_bank_offset - The offset inside the bank to read from. Half the offset in the TCAM bank.
 * \param [in] payload            - The buffer to copy write data to (should be
 *                                  pre-allocated to at least standard payload size)
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL INPUT:
 *  * Wrties to TCAM shadow
 */
shr_error_e dnx_field_tcam_access_entry_payload_write_shadow(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 action_bank_offset,
    uint32 *payload);

/**
 * \brief
 *  Reads a single-size entry (key and mask) from the shadow of the given core.
 *  After reading the entry data, function decodes data to restore key/mask to their
 *  original value.
 *
 * \param [in] unit              - Device ID
 * \param [in] core              - Core ID
 * \param [in] bank_id           - The bank ID to read from
 * \param [in] bank_offset       - The offset inside the bank to read from, twice the action bank offset.
 * \param [out] key              - Read entry's key
 * \param [out] key_mask         - Read entry's mask
 *
 * \return
 *   \retval _SHR_E_NONE success
 *   \retval _SHR_E_FAIL offset is odd
 *
 * \remark
 * SPECIAL INPUT:
 *  * None.
 */

shr_error_e dnx_field_tcam_access_entry_decoded_read_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *key,
    uint32 *key_mask);

/**
 * \brief
 *  Compares given key/mask pair with given size are equal to the entry in the
 *  given bank_id/bank_offset, sets "equal" indication accordingly
 *
 * \param [in] unit               - Device ID
 * \param [in] core               - Core ID
 * \param [in] key                - Key to compare along with given mask
 * \param [in] key_mask           - Mask to compare along with given key
 * \param [in] key_size           - Size of key to compare
 * \param [in] bank_id            - bank_id of the entry to compare pair with
 * \param [in] bank_offset        - bank_offset of the entry to compare pair with
 * \param [in] skip_entry_in_use  - Skip entry in use check when comparing
 * \param [out] equal             - "equal" indication
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 *    None
 */
shr_error_e dnx_field_tcam_access_entry_equal_shadow(
    int unit,
    int core,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 bank_id,
    uint32 bank_offset,
    uint8 skip_entry_in_use,
    uint8 *equal);

/** } */
#endif
