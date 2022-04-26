/** \file adapter_mdb_access.h
 * This file handles the communication with the adapter's server for MDB logical access
 *
 * $Copyright: (c) 2021 Broadcom.
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
 * Packet Write format:
 *   ------------- -------------- ---------- ------------- --------- -----------
 *  | Opcode(145) | payload size | Block ID | Memory Type | Command | Memory ID | -->
 *  |   1 byte    |   4 bytes    | 4 bytes  |   1 byte    | 1 byte  |  1 byte   | -->
 *   ------------- -------------- ---------- ------------- --------- -----------
 *
 *       -------- ---------- ----- ------------------- ---------
 *  --> | App_ID | Key Prefix Length | Key Size | Key | Payload |
 *  --> | 1 byte |     1 bytes       |  4 bytes | --- |  4 bytes|
 *       -------- ---------- ----- ------------------- ---------
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
 * Packet Delete format:
 *   ------------- -------------- ---------- ------------- --------- -----------
 *  | Opcode(145) | payload size | Block ID | Memory Type | Command | Memory ID | -->
 *  |   1 byte    |   4 bytes    | 4 bytes  |   1 byte    | 1 byte  |  1 byte   | -->
 *   ------------- -------------- ---------- ------------- --------- -----------
 *
 *       -------- ---------- ----- -------------------
 *  --> | App_ID | Key Prefix Length | Key Size | Key |
 *  --> | 1 byte |     1 bytes       |  4 bytes | --- |
 *       -------- ---------- ----- -------------------
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
 * Packet Read format:
 *   ------------- -------------- ---------- ------------- --------- -----------
 *  | Opcode(145) | payload size | Block ID | Memory Type | Command | Memory ID | -->
 *  |   1 byte    |   4 bytes    | 4 bytes  |   1 byte    | 1 byte  |  1 byte   | -->
 *   ------------- -------------- ---------- ------------- --------- -----------
 *
 * Request:
 *       -------- ---------- ----- -------------------
 *  --> | App_ID | Key Prefix Length | Key Size | Key |
 *  --> | 1 byte |     1 bytes       |  4 bytes | --- |
 *       -------- ---------- ----- -------------------
 *
 * Response:
 *       -------- ---------- ----- ------------------- -------- ---------
 *  --> | App_ID | Key Prefix Length | Key Size | Key | Error  | Payload |
 *  --> | 1 byte |     1 bytes       |  4 bytes | --- | 1 byte | 4 bytes |
 *       -------- ---------- ----- ------------------- -------- ---------
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


/**
 * \brief - Add an entry to the MDB stub Direct/EEDB Table
 *
 * Packet Write format:
 *   ------------- -------------- ---------- ------------- --------- -----------
 *  | Opcode(145) | payload size | Block ID | Memory Type | Command | Memory ID | -->
 *  |   1 byte    |   4 bytes    | 4 bytes  |   1 byte    | 1 byte  |  1 byte   | -->
 *   ------------- -------------- ---------- ------------- --------- -----------
 *
 *       ---------- -------------- ---------
 *  --> |   Key    | Payload Size | Payload |
 *  --> | 4 bytes  |   4 bytes    |   ---   |
 *       ---------- -------------- ---------
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
shr_error_e adapter_mdb_access_direct_table_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

/**
 * \brief - Get an entry from the MDB stub Direct/EEDB Table
 *
 * Packet Read format:
 *   ------------- -------------- ---------- ------------- --------- -----------
 *  | Opcode(145) | payload size | Block ID | Memory Type | Command | Memory ID | -->
 *  |   1 byte    |   4 bytes    | 4 bytes  |   1 byte    | 1 byte  |  1 byte   | -->
 *   ------------- -------------- ---------- ------------- --------- -----------
 *
 * Request:
 *       ---------- -------------- ---------
 *  --> |   Key    | Payload Size | Payload |
 *  --> | 4 bytes  |   4 bytes    |   ---   |
 *       ---------- -------------- ---------
 *
 * Response:
 *       ---------- -------- -------------- ---------
 *  --> |   Key    |  Error | Payload Size | Payload |
 *  --> | 4 bytes  | 1 byte |   4 bytes    |   ---   |
 *       ---------- -------- -------------- ---------

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
shr_error_e adapter_mdb_access_direct_table_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

/**
 * \brief - Add an entry to the MDB stub Exact Match DB
 *
 * Packet Write format:
 *   ------------- -------------- ---------- ------------- --------- -----------
 *  | Opcode(145) | payload size | Block ID | Memory Type | Command | Memory ID | -->
 *  |   1 byte    |   4 bytes    | 4 bytes  |   1 byte    | 1 byte  |  1 byte   | -->
 *   ------------- -------------- ---------- ------------- --------- -----------
 *
 * Write:
 *       -------- ---------- ----- ------------- ---------
 *  --> | App_ID | Key Size | Key | Payload Size| Payload |
 *  --> | 1 byte | 4 bytes  | --- |   4 bytes   |   ---   |
 *       -------- ---------- ----- ------------- ---------
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
shr_error_e adapter_mdb_access_em_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);
/**
 * \brief - Delete an entry from the MDB stub Exact Match Table
 *
 * Packet Delete format:
 *   ------------- -------------- ---------- ------------- --------- -----------
 *  | Opcode(145) | payload size | Block ID | Memory Type | Command | Memory ID | -->
 *  |   1 byte    |   4 bytes    | 4 bytes  |   1 byte    | 1 byte  |  1 byte   | -->
 *   ------------- -------------- ---------- ------------- --------- -----------
 *
 *       -------- ---------- -----
 *  --> | App_ID | Key Size | Key |
 *  --> | 1 byte | 4 bytes  | --- |
 *       -------- ---------- -----
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
shr_error_e adapter_mdb_access_em_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

/**
 * \brief - Get an entry from the MDB stub Exact Match
 *
 * Packet Delete format:
 *   ------------- -------------- ---------- ------------- --------- -----------
 *  | Opcode(145) | payload size | Block ID | Memory Type | Command | Memory ID | -->
 *  |   1 byte    |   4 bytes    | 4 bytes  |   1 byte    | 1 byte  |  1 byte   | -->
 *   ------------- -------------- ---------- ------------- --------- -----------
 *
 * Request:
 *       -------- ---------- -----
 *  --> | App_ID | Key Size | Key |
 *  --> | 1 byte | 4 bytes  | --- |
 *       -------- ---------- -----
 *
 * Response:
 *       -------- ---------- ----- -------- ------------- ---------
 *  --> | App_ID | Key Size | Key | Error  | Payload Size| Payload |
 *  --> | 1 byte | 4 bytes  | --- | 1 byte |   4 bytes   |   ---   |
 *       -------- ---------- ----- -------- ------------- ---------
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
shr_error_e adapter_mdb_access_em_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

/**
 * \brief - Add an entry to the MDB stub EEDB
 *
 * Packet Write format:
 *   ------------- -------------- ---------- ------------- --------- -----------
 *  | Opcode(145) | payload size | Block ID | Memory Type | Command | Memory ID | -->
 *  |   1 byte    |   4 bytes    | 4 bytes  |   1 byte    | 1 byte  |  1 byte   | -->
 *   ------------- -------------- ---------- ------------- --------- -----------
 *
 * Write:
 *       ---------- --------- ---------- ------------- ---------
 *  --> |    Key   |    LL   | Phase ID | Payload Size| Payload |
 *  --> |  4 bytes | 4 bytes |   1 byte |   4 bytes   |   ~~~   |
 *       ---------- --------- ---------- ------------- ---------
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] dbal_physical_table_id - DBAL physical table id
 *   \param [in] app_id - the app ID
 *   \param [in] ll_exist - indication,that LL exist
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
shr_error_e adapter_mdb_access_eedb_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    int ll_exist,
    dbal_physical_entry_t * entry);

/**
 * \brief - Read entry from the MDB stub EEDB
 *
 * Packet Write format:
 *   ------------- -------------- ---------- ------------- --------- -----------
 *  | Opcode(145) | payload size | Block ID | Memory Type | Command | Memory ID | -->
 *  |   1 byte    |   4 bytes    | 4 bytes  |   1 byte    | 1 byte  |  1 byte   | -->
 *   ------------- -------------- ---------- ------------- --------- -----------
 *
 * Write:
 *       ---------- -------- --------- ---------- ------------- ----------
 *  --> |    Key   |  Error |  LL     | Phase ID | Payload Size| Payload |
 *  --> |  4 bytes | 1 byte | 4 bytes |   1 byte |   4 bytes   |   ~~~   |
 *       ---------- -------- --------- ---------- ------------- ---------
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
 *   \param [in] dbal_physical_table_id - DBAL physical table id
 *   \param [in] app_id - the app ID
 *   \param [out] entry - the entry to be inserted
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
shr_error_e adapter_mdb_access_eedb_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry);

/**
 * \brief - Init MDB stub
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit id
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
shr_error_e adapter_mdb_init(
    int unit);
/**ADAPTER_MDB_ACCESS_H*/

/* } */
#endif
