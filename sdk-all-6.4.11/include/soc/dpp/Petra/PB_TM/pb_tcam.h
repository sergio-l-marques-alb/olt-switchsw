/* $Id: pb_tcam.h,v 1.7 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_tcam.h
*
* MODULE PREFIX:  soc_pb_tcam
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_PB_TCAM_INCLUDED__
/* { */
#define __SOC_PB_TCAM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_tcam.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     The maximum length of a TCAM entry, in longs.           */
#define  SOC_PB_TCAM_ENTRY_MAX_LEN (9)

/*     Number of unique TCAM database prefixes.                */
#define  SOC_PB_TCAM_NOF_PREFIXES (16)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

#define SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS                    SOC_TMC_TCAM_BANK_ENTRY_SIZE_72_BITS
#define SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS                   SOC_TMC_TCAM_BANK_ENTRY_SIZE_144_BITS
#define SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS                   SOC_TMC_TCAM_BANK_ENTRY_SIZE_288_BITS
#define SOC_PB_TCAM_NOF_BANK_ENTRY_SIZES                       SOC_TMC_NOF_TCAM_BANK_ENTRY_SIZES
typedef SOC_TMC_TCAM_BANK_ENTRY_SIZE                           SOC_PB_TCAM_BANK_ENTRY_SIZE;

#define SOC_PB_TCAM_USER_FWDING_IPV6_UC                        SOC_TMC_TCAM_USER_FWDING_IPV6_UC
#define SOC_PB_TCAM_USER_FWDING_IPV6_MC                        SOC_TMC_TCAM_USER_FWDING_IPV6_MC
#define SOC_PB_TCAM_USER_FP                                    SOC_TMC_TCAM_USER_FP
#define SOC_PB_NOF_TCAM_USERS                                  SOC_TMC_NOF_TCAM_USERS
typedef SOC_TMC_TCAM_USER                                      SOC_PB_TCAM_USER;

typedef SOC_TMC_TCAM_DB_PREFIX                                 SOC_PB_TCAM_DB_PREFIX;
typedef SOC_TMC_TCAM_DB_USER                                   SOC_PB_TCAM_DB_USER;
typedef SOC_TMC_TCAM_DB_INFO                                   SOC_PB_TCAM_DB_INFO;

typedef enum
{
  /*
   *  Entries in lower banks have priority over entries from
   *  higher banks.
   */
  SOC_PB_TCAM_DB_PRIO_MODE_BANK = 0,
  /*
   *  Entry priorities are interlaced.
   */
  SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED = 1,
  /*
   *  Number of types in SOC_PB_TCAM_DB_PRIO_MODE
   */
  SOC_PB_TCAM_NOF_DB_PRIO_MODES = 2
}SOC_PB_TCAM_DB_PRIO_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The bits comprising the database's prefix, aligned to
   *  the LSB.
   */
  uint32 bits;
  /*
   *  The number of LSBs in 'bits' comprising the database's
   *  prefix.
   */
  uint32 length;

} SOC_PB_TCAM_PREFIX;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The TCAM bank in which the specified location resides.
   */
  uint32 bank_id;
  /*
   *  The specified location's entry in the appropriate TCAM
   *  bank.
   */
  uint32 entry;

} SOC_PB_TCAM_LOCATION;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The lowest location in the range, inclusive.
   */
  SOC_PB_TCAM_LOCATION min;
  /*
   *  The highest location in the range, inclusive.
   */
  SOC_PB_TCAM_LOCATION max;

} SOC_PB_TCAM_RANGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The entry's value.
   */
  uint32 value[SOC_PB_TCAM_ENTRY_MAX_LEN];
  /*
   *  The entry's mask.
   */
  uint32 mask[SOC_PB_TCAM_ENTRY_MAX_LEN];

} SOC_PB_TCAM_ENTRY;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


  uint32
    soc_pb_tcam_db_data_structure_entry_get(
      SOC_SAND_IN  int prime_handle,
      SOC_SAND_IN  uint32 sec_handle,
      SOC_SAND_IN  uint8  *buffer,
      SOC_SAND_IN  uint32 offset,
      SOC_SAND_IN  uint32 len,
      SOC_SAND_OUT uint8  *data
    );
  
  uint32
    soc_pb_tcam_db_data_structure_entry_set(
      SOC_SAND_IN    int prime_handle,
      SOC_SAND_IN    uint32 sec_handle,
      SOC_SAND_INOUT uint8  *buffer,
      SOC_SAND_IN    uint32 offset,
      SOC_SAND_IN    uint32 len,
      SOC_SAND_IN    uint8  *data
    );

  int32
    soc_pb_tcam_db_priority_list_cmp_bank(
      SOC_SAND_IN uint8  *buffer1,
      SOC_SAND_IN uint8  *buffer2,
              uint32 size
    );

  int32
    soc_pb_tcam_db_priority_list_cmp_interlaced(
      SOC_SAND_IN uint8  *buffer1,
      SOC_SAND_IN uint8  *buffer2,
              uint32 size
    );


uint32
  soc_pb_tcam_db_entry_id_to_location_entry_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        tcam_db_id,
    SOC_SAND_IN  uint32        entry_id,
    SOC_SAND_OUT SOC_PB_TCAM_LOCATION *location,
    SOC_SAND_OUT uint8        *found
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_bank_init_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function initializes a TCAM bank.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               bank_id -
 *     The ID of the bank to initialize.
 *   SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size -
 *     The entry size to initialize the bank to.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_bank_init_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size
  );

uint32
  soc_pb_tcam_bank_init_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_create_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Creates a new TCAM database.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database to create.
 *   SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size -
 *     The entry size of the database's entries.
 *   SOC_SAND_IN  uint32               prefix_size -
 *     Number of bits in the database's prefix. Range: 0 - 4.
 *   SOC_SAND_IN  SOC_PB_TCAM_DB_PRIO_MODE    prio_mode -
 *     The new database's priority mode (IP forwarding uses
 *     bank mode, the field processor and the egress ACLs use
 *     interlaced mode).
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_create_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  uint32               prefix_size,
    SOC_SAND_IN  SOC_PB_TCAM_DB_PRIO_MODE    prio_mode
  );

uint32
  soc_pb_tcam_db_create_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  uint32               prefix_size,
    SOC_SAND_IN  SOC_PB_TCAM_DB_PRIO_MODE    prio_mode
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_destroy_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Destroys a TCAM database and frees the resources
 *   allocated to it.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database to destroy.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_destroy_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  );

uint32
  soc_pb_tcam_db_destroy_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_bank_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Adds a TCAM bank to a database.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database to that will receive the new bank.
 *   SOC_SAND_IN  uint32               bank_id -
 *     The bank that will be added to the database.
 *   SOC_SAND_IN  SOC_PB_TCAM_PREFIX          *prefix -
 *     The database's prefix in the new bank.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_bank_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_PREFIX          *prefix
  );

uint32
  soc_pb_tcam_db_bank_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_PREFIX          *prefix
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_bank_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Removes a bank from the database's resource pool.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database.
 *   SOC_SAND_IN  uint32               bank_id -
 *     ID of the bank to remove.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_bank_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  );

uint32
  soc_pb_tcam_db_bank_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_nof_banks_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the number of banks occupied by the database.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database to query.
 *   SOC_SAND_OUT uint32               *nof_banks -
 *     The number of banks occupied by the database.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_nof_banks_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_OUT uint32               *nof_banks
  );

uint32
  soc_pb_tcam_db_nof_banks_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_bank_prefix_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the database's prefix in the bank.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database to query.
 *   SOC_SAND_IN  uint32               bank_id -
 *     The bank to query.
 *   SOC_SAND_OUT SOC_PB_TCAM_PREFIX          *prefix -
 *     The database's prefix in the bank.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_bank_prefix_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_OUT SOC_PB_TCAM_PREFIX          *prefix
  );

uint32
  soc_pb_tcam_db_bank_prefix_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_entry_size_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the database's entry size.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database to query.
 *   SOC_SAND_OUT SOC_PB_TCAM_BANK_ENTRY_SIZE *entry_size -
 *     The database's entry_size.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_entry_size_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_OUT SOC_PB_TCAM_BANK_ENTRY_SIZE *entry_size
  );

uint32
  soc_pb_tcam_db_entry_size_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to a database.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database.
 *   SOC_SAND_IN  uint32               entry_id -
 *     The new entry's ID.
 *   SOC_SAND_IN  uint16                priority -
 *     The new entry's priority. Range: 0 - 65535.
 *   SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry -
 *     The new entry's data.
 *   SOC_SAND_IN  uint32                action -
 *     The action associated with the new entry.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success -
 *     Indication whether the insertion succeeded or not.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_entry_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint16                priority,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry,
    SOC_SAND_IN  uint32                action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  soc_pb_tcam_db_entry_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint16                priority,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry,
    SOC_SAND_IN  uint32                action
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_entry_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Reads an entry from the database.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database.
 *   SOC_SAND_IN  uint32               entry_id -
 *     The entry's ID.
 *   SOC_SAND_OUT uint16                *priority -
 *     The entry's priority.
 *   SOC_SAND_OUT SOC_PB_TCAM_ENTRY           *entry -
 *     The entry's data.
 *   SOC_SAND_OUT uint32                *action -
 *     The action associated with the entry.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_entry_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_OUT uint16                *priority,
    SOC_SAND_OUT SOC_PB_TCAM_ENTRY           *entry,
    SOC_SAND_OUT uint32                *action,
    SOC_SAND_OUT uint8               *found
  );

uint32
  soc_pb_tcam_db_entry_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  );
  
/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_entry_search_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Search for an entry in a database using the same logic
 *   used by the hardware.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database.
 *   SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *key -
 *     The key for the search operation. If a mask bit is set,
 *     the corresponding value bit is used in the comparison;
 *     otherwise that value bit is ignored.
 *   SOC_SAND_OUT uint32               *entry_id -
 *     The first hit's entry ID.
 *   SOC_SAND_OUT uint16                *priority -
 *     The first hit's priority.
 *   SOC_SAND_OUT uint8               *found -
 *     Indicates whether a match was found or not.
 * REMARKS:
 *   This function only works for databases with 72- or
 *   144-bit keys.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_entry_search_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *key,
    SOC_SAND_OUT uint32               *entry_id,
    SOC_SAND_OUT uint8               *found
  );

uint32
  soc_pb_tcam_db_entry_search_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *key
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_entry_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Removes an entry from the database.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database.
 *   SOC_SAND_IN  uint32               entry_id -
 *     ID of the entry to remove.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_entry_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  );

uint32
  soc_pb_tcam_db_entry_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_db_is_bank_used_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Queries whether the bank belongs to the database or not.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     ID of the database to query.
 *   SOC_SAND_IN  uint32               bank_id -
 *     The bank in question.
 *   SOC_SAND_OUT uint8               *is_used -
 *     Whether the bank belongs to the database or not.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_db_is_bank_used_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_OUT uint8               *is_used
  );

uint32
  soc_pb_tcam_db_is_bank_used_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  );

void
  SOC_PB_TCAM_PREFIX_clear(
    SOC_SAND_OUT SOC_PB_TCAM_PREFIX *info
  );

void
  SOC_PB_TCAM_LOCATION_clear(
    SOC_SAND_OUT SOC_PB_TCAM_LOCATION *info
  );

void
  SOC_PB_TCAM_RANGE_clear(
    SOC_SAND_OUT SOC_PB_TCAM_RANGE *info
  );

void
  SOC_PB_TCAM_ENTRY_clear(
    SOC_SAND_OUT SOC_PB_TCAM_ENTRY *info
  );

uint32
  SOC_PB_TCAM_PREFIX_verify(
    SOC_SAND_IN  SOC_PB_TCAM_PREFIX *info
  );

uint32
  SOC_PB_TCAM_LOCATION_verify(
    SOC_SAND_IN  SOC_PB_TCAM_LOCATION *info
  );

uint32
  SOC_PB_TCAM_RANGE_verify(
    SOC_SAND_IN  SOC_PB_TCAM_RANGE *info
  );

uint32
  SOC_PB_TCAM_ENTRY_verify(
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_TCAM_BANK_ENTRY_SIZE_to_string(
    SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE enum_val
  );

const char*
  SOC_PB_TCAM_DB_PRIO_MODE_to_string(
    SOC_SAND_IN  SOC_PB_TCAM_DB_PRIO_MODE enum_val
  );

void
  SOC_PB_TCAM_PREFIX_print(
    SOC_SAND_IN  SOC_PB_TCAM_PREFIX *info
  );

void
  SOC_PB_TCAM_LOCATION_print(
    SOC_SAND_IN  SOC_PB_TCAM_LOCATION *info
  );

void
  SOC_PB_TCAM_RANGE_print(
    SOC_SAND_IN  SOC_PB_TCAM_RANGE *info
  );

void
  SOC_PB_TCAM_ENTRY_print(
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_TCAM_INCLUDED__*/
#endif

