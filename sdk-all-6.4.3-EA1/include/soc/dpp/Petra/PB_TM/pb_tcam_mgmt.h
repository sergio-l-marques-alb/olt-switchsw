/* $Id: soc_pb_tcam_mgmt.h,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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

#ifndef __SOC_PB_TCAM_MGMT_INCLUDED__
/* { */
#define __SOC_PB_TCAM_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>

#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

#include <soc/dpp/Petra/PB_TM/pb_tcam.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_pb_tcam_mgmt.h,v 1.5 Broadcom SDK $
 * maximum number of different ACLs (can be reduced to 64)
 */
#define SOC_PB_TCAM_MAX_NOF_ACLS  128
/*
 * maximum number of different list
 */
#define SOC_PB_TCAM_MAX_NOF_LISTS   (2*SOC_PB_TCAM_MAX_NOF_ACLS + SOC_PB_NOF_TCAM_USERS + SOC_PB_NOF_IP_TCAM_ENTRY_TYPES)
/*
 * Number Of cycles
 */
#define SOC_PB_TCAM_NOF_CYCLES  2
/*
 * The maximum number of users (application type)
 *  ACL l2, l3, l3a, ipv4 mc, ipv6  per TCAM bank.
 */
#define SOC_PB_TCAM_MAX_USERS_PER_BANK  SOC_PB_TCAM_NOF_CYCLES
/*
 * number of different keys in cycle
 */
#define SOC_PB_TCAM_NOF_ACL_KEYS_PER_CYCLE  2

/*
 * number of different keys in cycle
 */

#define SOC_PB_TCAM_MGMT_NOF_PRIO_ENC 2

/*
 * bank size
 */
#define SOC_PB_TCAM_BANK_SIZE  512




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

typedef enum
{
  /*
   *  Even cycles.
   */
  SOC_PB_TCAM_ACCESS_CYCLE_0 = 0,
  /*
   *  Odd cycles.
   */
  SOC_PB_TCAM_ACCESS_CYCLE_1 = 1,
  /*
   *  Number of types in SOC_PB_TCAM_ACCESS_CYCLE
   */
  SOC_PB_TCAM_NOF_ACCESS_CYCLES = 2
}SOC_PB_TCAM_ACCESS_CYCLE;

typedef enum
{
  /*
   *  4-bank priority decoder.
   */
  SOC_PB_TCAM_ACCESS_DEVICE_PD1 = 0,
  /*
   *  2-bank priority decoder.
   */
  SOC_PB_TCAM_ACCESS_DEVICE_PD2 = 1,
  /*
   *  Bank selector.
   */
  SOC_PB_TCAM_ACCESS_DEVICE_SEL3 = 2,
  /*
   *  Bank selector.
   */
  SOC_PB_TCAM_ACCESS_DEVICE_SEL4 = 3,
  /*
   *  Number of types in SOC_PB_TCAM_ACCESS_DEVICE
   */
  SOC_PB_TCAM_NOF_ACCESS_DEVICES = 4
}SOC_PB_TCAM_ACCESS_DEVICE;

typedef uint32
  (*SOC_PB_TCAM_MGMT_SIGNAL)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  user_data
  );

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Type of the user.
   */
  SOC_PB_TCAM_USER use_type;
  /*
   *  Number of entries in use from the bank for the use_type.
   */
  uint32 used_entries;
}SOC_PB_TCAM_BANK_USER_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Number of free entries in the bank,
   */
  uint32 free_entries;
  /*
   *  Number of applications that use this bank
   */
  uint8 nof_users;
  /*
   *  The information for the application use this bank.
   */
  SOC_PB_TCAM_BANK_USER_INFO users_info[SOC_PB_TCAM_MAX_USERS_PER_BANK];
}SOC_PB_TCAM_BANK_STATUS;

typedef struct
{
 /*
  * offset in the bank
  */
  uint16 offset;
  /*
  *  banks ID
  */
  uint8 bank_id;
}SOC_PB_TCAM_PLACE_ID;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  cycle X bank --> TCAM Use
   */
  SOC_PB_TCAM_USER cycle_bank_use[SOC_PB_TCAM_NOF_CYCLES][SOC_PB_TCAM_NOF_BANKS];
  /*
   *  cycle --> two ACL keys
   */
  SOC_PB_TCAM_USER cycle_keys[SOC_PB_TCAM_NOF_CYCLES][SOC_PB_TCAM_NOF_ACL_KEYS_PER_CYCLE];
  /*
   *  encoder X cycle --> banks bitmap
   */
  uint8 pe_cycle_banks[SOC_PB_TCAM_MGMT_NOF_PRIO_ENC][SOC_PB_TCAM_NOF_CYCLES];
}SOC_PB_TCAM_HW_STATUS;


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


/*********************************************************************
* NAME:
 *   soc_pb_tcam_access_profile_create_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Creates a new access profile for an existing TCAM
 *   database.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               profile_id -
 *     The new access profile's ID.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     The database that is to be accessed by the profile.
 *   SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE    cycle -
 *     The access cycle.
 *   SOC_SAND_IN  uint8               uniform_prefix -
 *     Indicates whether the access requires that all banks of
 *     the database have the same prefix or not.
 *   SOC_SAND_IN  uint32               min_banks -
 *     The minimum number of banks this database requires.
 *   SOC_SAND_IN  SOC_PB_TCAM_MGMT_SIGNAL     callback -
 *     Used to signal the owner of the database when the
 *     database's topology is changed.
 *   SOC_SAND_IN  uint32                user_data -
 *     A user-defined parameter that will be passed to the
 *     callback function.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success -
 *     Indicates whether the operation succeeded or not.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_access_profile_create_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE    cycle,
    SOC_SAND_IN  uint8               uniform_prefix,
    SOC_SAND_IN  uint32               min_banks,
    SOC_SAND_IN  SOC_PB_TCAM_MGMT_SIGNAL     callback,
    SOC_SAND_IN  uint32                user_data,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  soc_pb_tcam_access_profile_create_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE    cycle,
    SOC_SAND_IN  uint8               uniform_prefix,
    SOC_SAND_IN  uint32               min_banks,
    SOC_SAND_IN  SOC_PB_TCAM_MGMT_SIGNAL     callback,
    SOC_SAND_IN  uint32                user_data
  );

uint32
  soc_pb_tcam_access_profile_destroy_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_managed_db_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Adds an entry to a TCAM database, and allocates more
 *   TCAM banks, as needed.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               profile_id -
 *     The access profile's ID.
 *   SOC_SAND_IN  uint32               entry_id -
 *     The new entry's ID.
 *   SOC_SAND_IN  uint16                priority -
 *     The new entry's priority.
 *   SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry -
 *     The new entry's data.
 *   SOC_SAND_IN  uint32                action -
 *     The action associated with the new entry.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success -
 *     Indicates whether the operation succeeded or not.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_managed_db_entry_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint16                priority,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry,
    SOC_SAND_IN  uint32                action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  soc_pb_tcam_managed_db_entry_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint16                priority,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry,
    SOC_SAND_IN  uint32                action
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_managed_db_entry_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Adds an entry to a TCAM database, and allocates more
 *   TCAM banks, as needed.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               profile_id -
 *     The access profile's ID.
 *   SOC_SAND_IN  uint32               entry_id -
 *     The entry's ID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_managed_db_entry_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               entry_id
  );

uint32
  soc_pb_tcam_managed_db_entry_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               entry_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_managed_bank_accessed_db_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the ID of the database that is accessed in a
 *   bank in a particular cycle.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               bank_id -
 *     The TCAM bank's ID.
 *   SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE    cycle -
 *     The bank's access cycle.
 *   SOC_SAND_OUT uint32               *tcam_db_id -
 *     The database accessed.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_managed_bank_accessed_db_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE    cycle,
    SOC_SAND_OUT uint32               *tcam_db_id
  );

uint32
  soc_pb_tcam_managed_bank_accessed_db_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE    cycle
  );

/*********************************************************************
* NAME:
 *   soc_pb_tcam_access_profile_access_device_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the access device assigned to an access profile.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               profile_id -
 *     The access profile's ID.
 *   SOC_SAND_OUT SOC_PB_TCAM_ACCESS_DEVICE   *access_device -
 *     The designated access device.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_tcam_access_profile_access_device_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_OUT SOC_PB_TCAM_ACCESS_DEVICE   *access_device
  );

uint32
  soc_pb_tcam_access_profile_access_device_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id
  );

void
  soc_pb_PB_TCAM_ENTRY_clear(
    SOC_SAND_OUT SOC_PB_TCAM_ENTRY *info
  );

void
  soc_pb_PB_TCAM_BANK_USER_INFO_clear(
    SOC_SAND_OUT SOC_PB_TCAM_BANK_USER_INFO *info
  );

void
  soc_pb_PB_TCAM_BANK_STATUS_clear(
    SOC_SAND_OUT SOC_PB_TCAM_BANK_STATUS *info
  );

void
  soc_pb_PB_TCAM_HW_STATUS_clear(
    SOC_SAND_OUT SOC_PB_TCAM_HW_STATUS *info
  );

#if SOC_PB_DEBUG

void
  soc_pb_PB_TCAM_ENTRY_print(
    SOC_SAND_IN SOC_PB_TCAM_ENTRY *info
  );



void
  soc_pb_PB_TCAM_BANK_USER_INFO_print(
    SOC_SAND_IN SOC_PB_TCAM_BANK_USER_INFO *info
  );



void
  soc_pb_PB_TCAM_BANK_STATUS_print(
    SOC_SAND_IN SOC_PB_TCAM_BANK_STATUS *info
  );

void
  soc_pb_PB_TCAM_HW_STATUS_print(
    SOC_SAND_IN SOC_PB_TCAM_HW_STATUS *info
  );

#endif /* SOC_PB_DEBUG */

uint32
  soc_pb_tcam_mgmt_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

int32
  soc_pb_tcam_mgmt_cmp_key(
    SOC_SAND_IN uint8             *buffer1,
    SOC_SAND_IN uint8             *buffer2,
    uint32                    size
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_TCAM_ACCESS_CYCLE_to_string(
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE enum_val
  );

const char*
  SOC_PB_TCAM_ACCESS_DEVICE_to_string(
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_DEVICE enum_val
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_TCAM_MGMT_INCLUDED__*/
#endif

