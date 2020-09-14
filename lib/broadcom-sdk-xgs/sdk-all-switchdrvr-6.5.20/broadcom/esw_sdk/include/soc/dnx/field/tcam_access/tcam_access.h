/**
* \file        tcam_access.h
*
* DESCRIPTION :
*       This file implements the TCAM access layer. This layer is the back-end
*       of the DBAL-TCAM interface. It implements all the necessary HW
*       functionality in order to read/write/invalidate/find entries in the
*       TCAM.
*
*       TCAM access is part of the FIELD ecosystem, which means it's intended
*       to work with groups of entries such that each group of entries is
*       associated with a certain field-group (database) that has certain
*       properties.
*
*       Those properties affect the entry positioning and sizing that are
*       allocated by the TCAM Manager. The DBAL-TCAM interface, underlying
*       the TCAM Manager, is unable to pass through this information to lower
*       layers. Therefore, a mechanism for passing this information indirectly
*       is required. In this implementation, SWSTATE is used for this purpose.
*
*
*
*
*       And due to the inability of the
*       DBAL-TCAM interface to introduce the required data- to
*       accommodate such needs, the need for an indirect data input between
*       the TCAM Manager and the TCAM access layer is inevitable.
*
*
*       Therefore, the procedure for using TCAM ACCSES is:
*       1. Call TCAM Manager (Handler) to allocate a position for the entry.
*       2. Invoke DBAL with the given access_id.
*
*                    * USER *
*                      V  V
*                      V  V (1)
*                      V  V
*                   (2)V  V    ***************
*                      V  +*->|               |
*                      V      |  TCAM MANAGER |
*                      V      |               |
*                      V       ***************
*                      V                   V       BCM
*  ****************************************************************
*                      V                   V       SOC
*                      V                   V
*                    **********            V
*                   |          |           V
*                   |  DBAL    |           V
*                   |          |           v
*                    **********        **************************
*                        V            |                          |
*                        V            | Indirect Input (SWSTATE) |
*                        V            |                          |
* DBAL*TCAM Interface**->V             **************************
*                        V                             ^
*                        V                             V
*                        V                             V
*                        V                             V
*                     ******************************   V
*                    |                              |**+
*                    | TCAM ACCESS (Current Module) |
*                    |                              |
*                     ******************************
*                           V
*                           V
*                           V
*                           V
*                           V
*                        *******
*                       |       |
*                       |  HW   |
*                       |       |
*                        *******
*
*
*       And what TCAM ACCESS does is the following:
*       1. Receive access_id from the DBAL-TCAM interface.
*       2. Retrieve the location/sizes for the entry from the indirect input
*          (SWSTATE that is managed by TCAM Manager) by using the given
*          access_id.
*       3. Write entry to HW
*
*
* Sample API usage:
* \code{.c}
* \endcode
*
*
* PUBLIC FUNCTIONS (dnx_field_tcam omitted):
*       shr_error_e    access_entry_add( unit, dbal_table_id, app_id, entry )
*/
/**
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef TCAM_ACCESS_H_INCLUDED
/** { */
#define TCAM_ACCESS_H_INCLUDED

#include <include/soc/dnx/dbal/dbal_structures.h>

/**
 * \brief
 *   Adds the given dbal entry to TCAM. This function is associated with
 *   DBAL and should not be called directly.
 *
 * \param [in] unit - unit
 * \param [in] physical_tables - an Array of physical tables that should be updated.
 * \param [in] app_id - The app_id (handler_id) that defines the context for the given entry
 * \param [in] entry - The entry to add
 * \param [in] flags - DBAL_MDB_ENTRY_FLAGS
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL_INPUTS :
 *       HW:
 *           TCAM_TCAM_BANK - Adds the entry to the TCAM BANK
 *           TCAM_TCAM_ACTION - Adds the entry action (payload) to the TCAM
 *                              ACTION
 * PROCESS :
 *                   [1] Retrive access_id/key from the entry
 *                       (access_id is the first 32b in the key buffer, key
 *                        is the rest of the key buffer)
 *                   [2] Retrive location/key and action sizes of the entry by
 *                       querying tcam_access.hash and tcam_fg_info SWSTATE
 *                   [3] Write entry to HW
 */
shr_error_e dnx_field_tcam_access_entry_add(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

/**
 * \brief
 *   Returns the key/key_mask/payload according to the given entry's
 *   hw id. This function is associated with DBAL and should not
 *   be called directly.
 *
 * \param [in] unit - unit
 * \param [in] dbal_physical_table_id - The table id
 * \param [in] app_id - The app_id (handler_id) that defines the context for the given entry
 * \param [in,out] entry - We use the entry's hw id as input to identify the entry with,
 *                         and key/payload buffers as the output of the entry's key/payload
 *                         in HW.
 * \param [in] flags - DBAL_MDB_ENTRY_FLAGS
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_access_entry_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

/**
 * \brief
 *   Deletes the given entry from TCAM according to entry's hw id.
 *   This function is associated with DBAL and should not
 *   be called directly.
 *
 * \param [in] unit - unit
 * \param [in] physical_tables - an Array of physical tables that should be updated.
 * \param [in] app_id - The app_id (handler_id) that defines the context for the given entry
 * \param [in] entry - Entry to delete
 * \param [in] flags - DBAL_MDB_ENTRY_FLAGS
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_access_entry_delete(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

/**
 * \brief
 *   Searches for the given dbal_key in TCAM at the given core, and
 *   returns the access_id for the entry associated with the given
 *   key/mask pair.
 *   If no such key/mask pair were found, an error is thrown.
 *   This function is associated with DBAL and should not
 *   be called directly.
 *
 * \param [in] unit       - unit
 * \param [in] core       - Core ID
 * \param [in] app_id     - The app_id (handler_id) that defines the context for the entry to search for
 * \param [in] key        - The dbal key to search for its corresponding entry along with the mask
 * \param [in] key_mask   - The dbal key_mask to search for its corresponding entry along with the key
 * \param [out] access_id - The access_id of the found entry
 *
 * \return
 *  \retval _SHR_E_NONE - success
 *  \retval _SHR_E_NOT_FOUND - Key/mask pair not found
 */
shr_error_e dnx_field_tcam_access_key_to_access_id(
    int unit,
    int core,
    uint32 app_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 *access_id);

/**
 * \brief
 *   Performs move operation in TCAM Access.
 *   This function is called to perform a move operation in TCAM Access at the given core.
 *   Caller needs to verify that position_to is empty (otherwise it will be overridden),
 *   and caller also needs to supply the app_id of the entry to be moved from the position_from.
 *
 * \param [in] unit          - unit
 * \param [in] core          - Core ID
 * \param [in] app_id        - The app_id (handler_id) that defines the context of the entry at position_from
 * \param [in] position_from - Current position of the entry we want to move
 * \param [in] position_to   - The position to move the entry to (Should be empty, otherwise
 *                              TCAM data will be lost)
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_access_move(
    int unit,
    int core,
    uint32 app_id,
    uint32 position_from,
    uint32 position_to);

/**
 * \brief
 *  Inits the iterator given for it to be able to iterate on the given app_id
 *
 * \param [in] unit - unit
 * \param [in] dbal_physical_table_id - The table id
 * \param [in] app_id - The app_id (handler_id) that defines the context for the iterator
 * \param [in] physical_entry_iterator - The DBAL iterator we want to deinit
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_access_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

/**
 * \brief
 *  Returns the entry next that:
 *    1) Belongs to the given app_id
 *    2) Is the first entry that meets condition 1, and after the entry pointed to by iterator
 *  In addition to that, it progresses the iterator to point to the new entry.
 *
 * \param [in] unit - unit
 * \param [in] dbal_physical_table_id - The table id
 * \param [in] app_id - The app_id (handler_id) that defines the context for the iterator
 * \param [in, out] physical_entry_iterator - The DBAL iterator we want to deinit
 * \param [out] entry - The next entry to be returned
 * \param [out] is_end - TRUE if the iterator points to the last node
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_access_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);

/**
 * \brief
 *  Deinits the iterator inited in 'iterator_init'
 *
 * \param [in] unit - unit
 * \param [in] dbal_physical_table_id - The table id
 * \param [in] app_id - The app_id (handler_id) that defines the context for the iterator
 * \param [in] physical_entry_iterator - The DBAL iterator we want to deinit
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_access_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

/**
 * \brief
 *  Dump changed tcam entries for specific bank id
 *
 * \param [in] unit - unit
 * \param [in] core_id - Core ID
 * \param [in] bank_id - The TCAM bank id
 * \param [in] mode - dump mode
 *                      0: dump from HW
 *                      1: dump from shadow
 *                      2: match check between shsadow & HW
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_ser_tcam_access_entry_key_dump(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 mode);

/**
 * \brief
 *  Rewrite an tcam entry from shadow. Used for TCAM ser correction
 *
 * \param [in] unit         - unit
 * \param [in] core_id      - Core ID
 * \param [in] bank_id      - The TCAM bank id
 * \param [in] bank_offset  - The TCAM entry offset
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_ser_tcam_access_rewrite_key_from_shadow(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 bank_offset);

/**
 * \brief
 *  Enable/Disable tcam ecc protect machine
 *
 * \param [in] unit - unit
 * \param [in] enable - 1: enable machine; 0:disable machine
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_ser_tcam_protect_machine_enable(
    int unit,
    int enable);

/**
 * \brief
 *  Writes the given key and key_mask to the given position, Just for SER
 *  debug purpose, Don't update shadow at this function.
 *
 * \param [in] unit        - Device ID
 * \param [in] core_id     - Core ID
 * \param [in] bank_id     - The TCAM_BANK ID to read from
 * \param [in] bank_offset - The offset inside the TCAM_BANK to read from, by counting Single sized entries.
 *                           Equal to the TCAM bank offset, twice the action bank offset.
 * \param [in] valid       - The valid bit
 * \param [in] key_mode    - The search mode (is set according to key size)
 * \param [in] key         - The key buffer to write
 * \param [in] mask        - The key_mask buffer to write
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL OUTPUT:
 *     HW:
 *        TCAM_TCAM_BANK_COMMAND - The memory to write data/command to
 */
shr_error_e dnx_ser_tcam_access_write_hw_entry(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 valid,
    uint32 key_mode,
    uint32 *key,
    uint32 *mask);

/** } */
#endif
