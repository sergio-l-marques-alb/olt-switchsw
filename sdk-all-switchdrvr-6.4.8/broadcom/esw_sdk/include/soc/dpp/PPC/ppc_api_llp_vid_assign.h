/* $Id$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
* FILENAME:       DuneDriver/ppc/include/soc_ppc_api_llp_vid_assign.h
*
* MODULE PREFIX:  soc_ppc_llp
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

#ifndef __SOC_PPC_API_LLP_VID_ASSIGN_INCLUDED__
/* { */
#define __SOC_PPC_API_LLP_VID_ASSIGN_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

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
   *  Entries that used for VID assignment for tagged packets
   */
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_TAGGED = 0x4,
  /*
   *  Entries that used for VID assignment for untagged
   *  packets
   */
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_UNTAGGED = 0x8,
  /*
   *  0xFFFFFFFF All enitres used for VID assignment
   */
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_ALL = (int)0xFFFFFFFF,
  /*
   *  Number of types in SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE
   */
  SOC_PPC_NOF_LLP_VID_ASSIGN_MATCH_RULE_TYPES = 3
}SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Port default VLAN ID. Range: 0 - 4095.
   */
  SOC_SAND_PP_VLAN_ID pvid;
  /*
   *  If TRUE, then ignore VID coming in the packet tag. Note
   *  that for SA-based, assignment of whether to ignore
   *  (override) the VID of the packet is per entry (MAC)
   *  attribute-See soc_ppd_llp_vid_assign_mac_based_set().
   */
  uint8 ignore_incoming_tag;
  /*
   *  Enable VID resolution according to IP subnet
   */
  uint8 enable_subnet_ip;
  /*
   *  Enable VID resolution according to L2 protocol
   */
  uint8 enable_protocol;
  /*
   *  Enable VID resolution according to SA
   */
  uint8 enable_sa_based;

} SOC_PPC_LLP_VID_ASSIGN_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  VLAN ID. Range: 0 - 4095.
   */
  SOC_SAND_PP_VLAN_ID vid;
  /*
   *  If TRUE, then the VID is used when packet comes
   *  untagged.
   */
  uint8 use_for_untagged;
  /*
   *  If TRUE, then the VID is used when the packet comes
   *  tagged and overrides it.
   */
  uint8 override_tagged;

} SOC_PPC_LLP_VID_ASSIGN_MAC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  VLAN ID. Range: 0 - 4095.
   */
  SOC_SAND_PP_VLAN_ID vid;
  /*
   *  Is the VID valid for this entry. If TRUE, then when
   *  there is subnet match, the above VID is taken.
   */
  uint8 vid_is_valid;

} SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  VLAN ID. Range: 0 - 4095.
   */
  SOC_SAND_PP_VLAN_ID vid;
  /*
   *  Is the VID valid for this entry. If TRUE, then when
   *  there are protocol matches, the VID is taken.
   */
  uint8 vid_is_valid;

} SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Rule type specifies which entries to return
   */
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE rule_type;
  /*
   *  VID value to match, use SOC_PPD_IGNORE_VAL to not compare to
   *  VID value.
   */
  SOC_SAND_PP_VLAN_ID vid;

} SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE;


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

void
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_PORT_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MAC_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE enum_val
  );

void
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE *info
  );

#endif /* SOC_PPC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPC_API_LLP_VID_ASSIGN_INCLUDED__*/
#endif

