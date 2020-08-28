/** \file adapter_mdb_access.h
 * This file handles the communication with the adapter's server for MDB logical access
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ $
 */

#ifndef ADAPTER_MDB_ACCESS_H_INCLUDED
/* { */
#define ADAPTER_MDB_ACCESS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Include files
 * {
 */

#include <soc/dnx/dbal/dbal_structures.h>

/*
 * }
 */

/*
 * Defines
 * {
 */

/*
 * }
 */

/**
 * \brief - Add an entry to the MDB stub LPM DB
 *
 * Packet request format:                   Opcode = 200 (1B) | overall_length (4B) |
 *                                          opcode (1 byte) | packet size (4 bytes) | Block ID (uint32) | Memory type (uint8)  | command (uint8)  |
 *                                          Memory ID (uint8) | App ID (uint8) | key prefix length (uint8) | key size (uint32)  |
 *                                          key ~~~  | payload (uint32) |
 *
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] dbal_physical_table_id - DBAL physical table id
 *   \param [in] app_id - the app ID
 *   \param [in] entry - the entry to be inserted
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * None
 *
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e adapter_mdb_access_lpm_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

/**
 * \brief - Delete an entry to the MDB stub LPM DB
 *
 * Packet request format:                   Opcode = 200 (1B) | overall_length (4B) |
 *                                          opcode (1 byte) | packet size (4 bytes) | Block ID (uint32) | Memory type (uint8)  | command (uint8)  |
 *                                          Memory ID (uint8) | App ID (uint8) | key prefix length (uint8) | key size (uint32)  |
 *                                          key ~~~  |
 *
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] dbal_physical_table_id - DBAL physical table id
 *   \param [in] app_id - the app ID
 *   \param [in] entry - the entry to be inserted
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * None
 *
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e adapter_mdb_access_lpm_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

/**
 * \brief - Get an entry from the MDB stub LPM DB
 *
 * Packet request format:
 *                                          opcode (1 byte) | packet size (4 bytes) | Block ID (uint32) | Memory type (uint8)  | command (uint8)  |
 *                                          Memory ID (uint8) | App ID (uint8) | key prefix length (uint8) | key size (uint32)  |
 *                                          key ~~~  |
 *
 * Response:
 *
 *  *            16              17                          18                   22
 *            ------------------------------------------------------------------------ ~~~ -----------------------------------
 *            | App ID (uint8) | key prefix length (uint8) | key size (uint32)  | key ~~~  | payload (uint32) |Error (uint8) |
 *            ------------------------------------------------------------------------ ~~~ -----------------------------------
 *
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] dbal_physical_table_id - DBAL physical table id
 *   \param [in] app_id - the app ID
 *   \param [in] entry - the entry to be inserted
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * None
 *
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e adapter_mdb_access_lpm_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

/**ADAPTER_MDB_ACCESS_H*/
/* } */
#endif
