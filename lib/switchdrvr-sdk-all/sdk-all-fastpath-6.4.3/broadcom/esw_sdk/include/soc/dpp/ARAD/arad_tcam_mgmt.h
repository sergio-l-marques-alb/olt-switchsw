/* $Id$
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

#ifndef __ARAD_TCAM_MGMT_INCLUDED__
/* { */
#define __ARAD_TCAM_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_framework.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/arad_tcam.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* TCAM DB IDs */
#define ARAD_TCAM_DB_ID_INVALID                                  (0)
/* FLP TCAM DEFINES */
#define ARAD_PP_FRWRD_IP_TCAM_ID_BASE                            (ARAD_TCAM_DB_ID_INVALID + 0x1)
#define ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_0                       (0 + ARAD_PP_FRWRD_IP_TCAM_ID_BASE) 
#define ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_UC_1 /* Duplication */     (1 + ARAD_PP_FRWRD_IP_TCAM_ID_BASE) 
#define ARAD_PP_FRWRD_IP_TCAM_ID_IPV4_MC                         (2 + ARAD_PP_FRWRD_IP_TCAM_ID_BASE)
#define ARAD_PP_FRWRD_IP_TCAM_ID_IPV6_UC                         (3 + ARAD_PP_FRWRD_IP_TCAM_ID_BASE)
#define ARAD_PP_FRWRD_IP_TCAM_ID_IPV6_MC                         (4 + ARAD_PP_FRWRD_IP_TCAM_ID_BASE)
#define ARAD_PP_SRC_BIND_TCAM_ID_IPV6_STATIC                     (5 + ARAD_PP_FRWRD_IP_TCAM_ID_BASE)
#define ARAD_PP_OAM_IDENTIFICATION                               (6 + ARAD_PP_FRWRD_IP_TCAM_ID_BASE)
#define ARAD_PP_FRWRD_TRILL_TCAM_ID_MC                           (7 + ARAD_PP_FRWRD_IP_TCAM_ID_BASE)
#define ARAD_PP_FLP_LAST                                         ARAD_PP_FRWRD_TRILL_TCAM_ID_MC

/* VTT TCAM DEFINES { */
/* TCAM DB profiles for VTT */
#define ARAD_PP_ISEM_ACCESS_TCAM_BASE                                   (ARAD_PP_FLP_LAST + 0x1)
#define ARAD_PP_ISEM_ACCESS_TCAM_MPLS_FRR_DB_PROFILE                    (0x0 + ARAD_PP_ISEM_ACCESS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV6_SPOOF_DB_PROFILE                  (0x1 + ARAD_PP_ISEM_ACCESS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV4_TT_ETH_DB_PROFILE                 (0x2 + ARAD_PP_ISEM_ACCESS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV6_TT_DB_PROFILE                     (0x3 + ARAD_PP_ISEM_ACCESS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_TCAM_ETH_INNER_OUTER_PCP_DB_PROFILE         (0x4 + ARAD_PP_ISEM_ACCESS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_TCAM_PON_EXTEND_LKP_DB_PROFILE              (0x6 + ARAD_PP_ISEM_ACCESS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_TCAM_MPLS_COUPLING_DB_PROFILE               (0x7 + ARAD_PP_ISEM_ACCESS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV4_MATCH_VT_DB_PROFILE               (0x8 + ARAD_PP_ISEM_ACCESS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_TCAM_MPLS_EXPLICIT_NULL_VT_DB_PROFILE       (0x9 + ARAD_PP_ISEM_ACCESS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_TCAM_TT_TRILL_TRANSPARENT_SERVICE           (0xa + ARAD_PP_ISEM_ACCESS_TCAM_BASE)

#define ARAD_PP_ISEM_ACCESS_VT_CLASSIFICATIONS_TCAM_BASE                (ARAD_PP_ISEM_ACCESS_TCAM_TT_TRILL_TRANSPARENT_SERVICE)
#define ARAD_PP_ISEM_ACCESS_VT_CLASSIFICATIONS_EFP                      (0x1 + ARAD_PP_ISEM_ACCESS_VT_CLASSIFICATIONS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_VT_CLASSIFICATIONS_TCAM_END                 (ARAD_PP_ISEM_ACCESS_VT_CLASSIFICATIONS_EFP)

#define ARAD_PP_ISEM_ACCESS_TCAM_END                                    (ARAD_PP_ISEM_ACCESS_VT_CLASSIFICATIONS_TCAM_END)

/* skip first ones as reserved for IPMC routing and VTT */
#define ARAD_PP_FP_DB_ID_TO_TCAM_DB_SHIFT  (ARAD_PP_ISEM_ACCESS_TCAM_END + 0x1)
#define ARAD_PP_FP_DB_ID_TO_TCAM_DB(fp_id)  ((fp_id) + ARAD_PP_FP_DB_ID_TO_TCAM_DB_SHIFT)
#define ARAD_PP_FP_TCAM_DB_TO_FP_ID(tcam_db_id)  ((tcam_db_id) - ARAD_PP_FP_DB_ID_TO_TCAM_DB_SHIFT)

#define ARAD_TCAM_MAX_NOF_ACLS  128

#define ARAD_TCAM_MAX_NOF_LISTS   ARAD_PP_FP_DB_ID_TO_TCAM_DB(ARAD_TCAM_MAX_NOF_ACLS)
/*
 * Number Of cycles
 */
#define ARAD_TCAM_NOF_CYCLES  1
/*
 * The maximum number of users (application type)
 *  ACL l2, l3, l3a, ipv4 mc, ipv6  per TCAM bank.
 */
#define ARAD_TCAM_MAX_USERS_PER_BANK  ARAD_TCAM_NOF_CYCLES
/*
 * number of different keys in cycle
 */
#define ARAD_TCAM_NOF_ACL_KEYS_PER_CYCLE  2

#define ARAD_TCAM_NOF_ACCESS_PROFILE_IDS                    (48)

/* 
 * Due to an HW limitation, an access profile must be always allocated 
 * at egress to prevent the lookup result FIFOs to be mistakely 
 * full. 
 * It requires the Driver to use a dummy access profile performing 
 * a lookup even when unnecessary - allocate the last one for this
 */
#define ARAD_TCAM_EGQ_DUMMY_ACCESS_PROFILE_WA_ENABLE        1
#ifdef ARAD_TCAM_EGQ_DUMMY_ACCESS_PROFILE_WA_ENABLE
#define ARAD_TCAM_EGQ_DUMMY_ACCESS_PROFILE_NO_LOOKUP        (ARAD_TCAM_NOF_ACCESS_PROFILE_IDS - 1)
#endif /* ARAD_TCAM_EGQ_DUMMY_ACCESS_PROFILE_WA_ENABLE */

/*
 * number of different keys in cycle
 */
#define ARAD_TCAM_NOF_ACL_KEYS  (ARAD_TCAM_NOF_CYCLES * ARAD_TCAM_NOF_ACL_KEYS_PER_CYCLE)

#define ARAD_TCAM_MGMT_NOF_PRIO_ENC 2

#define ARAD_TCAM_MGMT_BANK_MSB  1
#define ARAD_TCAM_MGMT_BANK_LSB  0
#define ARAD_TCAM_MGMT_OFFSET_MSB  11
#define ARAD_TCAM_MGMT_OFFSET_LSB  2

#define ARAD_TCAM_ENTRY_SIZE_MAX                                 (ARAD_TCAM_NOF_BANK_ENTRY_SIZES-1)
#define ARAD_TCAM_PREFIX_SIZE_MAX                                (4)
#define ARAD_TCAM_ACTION_SIZE_MAX                                ((1 << ARAD_TCAM_NOF_ACTION_SIZES)-1)
#define ARAD_TCAM_BANK_OWNER_MAX                                 (ARAD_TCAM_NOF_BANK_OWNERS-1)
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

typedef uint32
  (*ARAD_TCAM_MGMT_SIGNAL)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  user_data
  );

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
}ARAD_TCAM_PLACE_ID;


typedef enum
{
    /* 
     * Allow small banks as well as large banks.
     */
    ARAD_TCAM_SMALL_BANKS_ALLOW = 0,
    /* 
     * Force small banks to be used for certain DB.
     */
    ARAD_TCAM_SMALL_BANKS_FORCE = 1,
    /* 
     * Forbid small bank to be used - used in case 
     * of DT and large key (key size > 7 bits).
     */
    ARAD_TCAM_SMALL_BANKS_FORBID = 2
    
} ARAD_TCAM_SMALL_BANKS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /* 
   * The minimum number of banks this database requires.
   */
  uint32 min_banks;
  /* 
   * The entry size of the database's entries.
   */
  ARAD_TCAM_BANK_ENTRY_SIZE entry_size;
  /* 
   * Number of bits in the database's prefix. Range: 0 - 4.
   */
  uint32 prefix_size;
  /* 
   * The action size of the database's action table.
   */
  ARAD_TCAM_ACTION_SIZE action_bitmap_ndx;
  /* 
   * Used to signal the owner of the database when the
   * database's topology is changed.
   */
  ARAD_TCAM_MGMT_SIGNAL callback;
  /* 
   * A user-defined parameter that will be passed to the callback function.
   */
  uint32 user_data;
  /* 
   * Identify bank owner 
   */
  ARAD_TCAM_BANK_OWNER bank_owner;
  /* 
   * Is direct access to TCAM action without TCAM lookup
   */
  uint8 is_direct;

  /* List of Databases forbidden to share a TCAM Bank with this one */
  uint32  forbidden_dbs[ARAD_BIT_TO_U32(ARAD_TCAM_MAX_NOF_LISTS)];

  /* 
   * Enum which states the wanted use of the small banks. 
   * TCAM Database can be limited to small banks. Also, the 
   * use of small banks can be forbidden (e.g. when direct table 
   * and key size is bigger than 7 bits).
   */
  ARAD_TCAM_SMALL_BANKS use_small_banks;

  /* 
   * If True, then the TCAM Database does not respect the BCM rule . 
   * for the entry insertion order when same priority                                                              .
   */
  uint8 no_insertion_priority_order;

  /* 
   * If True, then the TCAM Database has only few priorities and                                              .
   * acts liek in IP TCAM, using the old method                                                                                                         .
   */
  uint8 sparse_priorities;

} ARAD_TCAM_ACCESS_INFO;

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
 *   arad_tcam_access_create_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Creates a new tcam database and access profile. 
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     The database that is to be accessed by the profile.
 *   SOC_SAND_IN  ARAD_TCAM_ACCESS_INFO  *tcam_info -
 *     Structurs consists all TCAM inforamtion needed on create.
 *   SOC_SAND_OUT uint32                  *access_profile_id -
 *     The new access profile's ID.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success -
 *     Indicates whether the operation succeeded or not.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_tcam_access_create_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,   
    SOC_SAND_IN  ARAD_TCAM_ACCESS_INFO *tcam_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  arad_tcam_access_create_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_ACCESS_INFO *tcam_info
  );

uint32
  arad_tcam_access_destroy_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  );

uint32 
  arad_tcam_managed_db_direct_table_bank_add(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32     tcam_db_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  );

uint32 
  arad_tcam_managed_db_direct_table_bank_remove(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32     tcam_db_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  );

/*********************************************************************
* NAME:
 *   arad_tcam_managed_db_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Adds an entry to a TCAM database, and allocates more
 *   TCAM banks, as needed.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     The database accessed.
 *   SOC_SAND_IN  uint32               entry_id -
 *     The new entry's ID.
 *   SOC_SAND_IN  uint32                priority -
 *     The new entry's priority.
 *   SOC_SAND_IN  ARAD_TCAM_ENTRY           *entry -
 *     The new entry's data.
 *   SOC_SAND_IN  ARAD_TCAM_ACTION          action -
 *     The action associated with the new entry.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success -
 *     Indicates whether the operation succeeded or not.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_tcam_managed_db_entry_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint8                is_single_bank,
    SOC_SAND_IN  uint32                priority,
    SOC_SAND_IN  ARAD_TCAM_ENTRY        *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION       *action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  arad_tcam_managed_db_entry_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint32                priority,
    SOC_SAND_IN  ARAD_TCAM_ENTRY           *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION          *action
  );

/*********************************************************************
* NAME:
 *   arad_tcam_managed_db_entry_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Adds an entry to a TCAM database, and allocates more
 *   TCAM banks, as needed.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               tcam_db_id -
 *     The database accessed.
 *   SOC_SAND_IN  uint32               entry_id -
 *     The entry's ID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_tcam_managed_db_entry_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  );

uint32
  arad_tcam_managed_db_entry_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  );
/*********************************************************************
* NAME:
 *   arad_tcam_access_pd_profile_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the pd profile assigned to an access profile.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32               access_profile_id -
 *     The access profile's ID.
 *   SOC_SAND_OUT uint32   *access_device -
 *     The pd profile id
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_tcam_access_pd_profile_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               access_profile_id,
    SOC_SAND_OUT uint32               *pd_profile_id
  );

uint32
  arad_tcam_access_pd_profile_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               access_profile_id
  );

uint32
  arad_tcam_access_profile_destroy_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 access_profile_id
  );

void
  arad_ARAD_TCAM_ENTRY_clear(
    SOC_SAND_OUT ARAD_TCAM_ENTRY *info
  );
uint32
  ARAD_TCAM_ACCESS_INFO_verify(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  ARAD_TCAM_ACCESS_INFO *info
  );

void
  arad_ARAD_TCAM_ACCESS_INFO_clear(
    SOC_SAND_OUT  ARAD_TCAM_ACCESS_INFO *info
  );


#if ARAD_DEBUG

void
  arad_ARAD_TCAM_ENTRY_print(
    SOC_SAND_IN ARAD_TCAM_ENTRY *info
  );


#endif /* ARAD_DEBUG */

uint32
  arad_tcam_mgmt_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

int32
  arad_tcam_mgmt_cmp_key(
    SOC_SAND_IN uint8             *buffer1,
    SOC_SAND_IN uint8             *buffer2,
    uint32                    size
  );

#if ARAD_DEBUG_IS_LVL1

#endif /* ARAD_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_TCAM_MGMT_INCLUDED__*/
#endif

