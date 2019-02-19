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


#ifndef __ARAD_SW_DB_TCAM_MGMT_INCLUDED__
/* { */
#define __ARAD_SW_DB_TCAM_MGMT_INCLUDED__

/*************
* INCLUDES  *
*************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/PPC/ppc_api_fp.h>

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#endif

/* } */

/*************
* DEFINES   *
*************/
/* { */

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
typedef enum
{
    ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0 = 0,
    ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1 = 1,
    ARAD_KBP_FRWRD_TBL_ID_IPV4_MC       = 2,

    ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0 = 3,
    ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1 = 4,
    ARAD_KBP_FRWRD_TBL_ID_IPV6_MC       = 5,

    ARAD_KBP_FRWRD_TBL_ID_LSR           = 6,
    ARAD_KBP_FRWRD_TBL_ID_TRILL_UC      = 7,
    ARAD_KBP_FRWRD_TBL_ID_TRILL_MC      = 8,

    ARAD_KBP_FRWRD_TBL_ID_DUMMY_0       = 9,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_1       = 10,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_2       = 11,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_3       = 12,

    ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED = 13,
    ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP = 14,
    ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_LSR = 15,

    ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6 = 16,
    
    ARAD_KBP_FRWRD_IP_NOF_TABLES        = 17,
    ARAD_KBP_ACL_TABLE_ID_OFFSET        = ARAD_KBP_FRWRD_IP_NOF_TABLES,

    ARAD_KBP_MAX_NUM_OF_TABLES          = ARAD_PMF_LOW_LEVEL_NOF_DBS,

} ARAD_KBP_FRWRD_IP_TBL_ID;

#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

typedef enum
{
  ARAD_IP_TCAM_ENTRY_TYPE_IPV4_UC_0  = 1,
  ARAD_IP_TCAM_ENTRY_TYPE_IPV4_UC_1  = 2,
  ARAD_IP_TCAM_ENTRY_TYPE_IPV4_MC  = 3,
  ARAD_IP_TCAM_ENTRY_TYPE_IPV6_UC  = 4,
  ARAD_IP_TCAM_ENTRY_TYPE_IPV6_VPN = 5,
  ARAD_IP_TCAM_ENTRY_TYPE_IPV6_MC  = 6, 
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  /* Insert the KBP table types for the common entry add/get/get_block/remove/table_clear code */
    ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_FORWARDING,
    ARAD_IP_TCAM_ENTRY_TYPE_KBP_LAST_FORWARDING = ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_FORWARDING + ARAD_KBP_FRWRD_IP_NOF_TABLES - 1, /* Last one for forwarding */
    ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_ACL,
    ARAD_IP_TCAM_ENTRY_TYPE_KBP_LAST_ACL = ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_ACL + SOC_PPC_FP_NOF_DBS - 1, /* Last one for ACL */
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
  ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND,
  ARAD_IP_TCAM_ENTRY_TYPE_TRILL_MC,
  ARAD_IP_NOF_TCAM_ENTRY_TYPES
} ARAD_IP_TCAM_ENTRY_TYPE;


#define ARAD_TRILL_TCAM_ENTRY_TYPE      (ARAD_IP_NOF_TCAM_ENTRY_TYPES + 1)

/* } */

/*************
*  MACROS   *
*************/
/* { */

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#define ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_FORWARDING(tcam_key_type) ((tcam_key_type >= ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_FORWARDING) && (tcam_key_type <= ARAD_IP_TCAM_ENTRY_TYPE_KBP_LAST_FORWARDING))
#define ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_ACL(tcam_key_type)        ((tcam_key_type >= ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_ACL) && (tcam_key_type <= ARAD_IP_TCAM_ENTRY_TYPE_KBP_LAST_ACL))
#define ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(tcam_key_type)            (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_FORWARDING(tcam_key_type) || ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_ACL(tcam_key_type))
#define ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(tcam_key_type)       ((ARAD_KBP_FRWRD_IP_TBL_ID) (tcam_key_type - ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_FORWARDING))
#define ARAD_IP_TCAM_FROM_KBP_FRWRD_IP_TBL_ID(table_id)          ((ARAD_IP_TCAM_ENTRY_TYPE) (table_id + ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_FORWARDING))
#else
#define ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(tcam_key_type) (0)
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */


/* } */

/*************
* TYPE DEFS *
*************/
/* { */

typedef enum
{
  ARAD_SW_DB_TCAM_AUX_DS_BITMAP_FREED_PLACES=0,

  ARAD_SW_DB_TCAM_AUX_DS_BITMAP_USED_PLACES=1,

  ARAD_SW_DB_TCAM_AUX_DS_LIST_PLACES=2

}ARAD_SW_DB_TCAM_AUX_DS_SELECT;

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __ARAD_SW_DB_TCAM_MGMT_INCLUDED__*/
#endif

