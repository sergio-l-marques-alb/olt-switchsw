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
/******************************************************************
*
* FILENAME:       DuneDriver/ppc/include/soc_ppc_api_llp_sa_auth.h
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

#ifndef __SOC_PPC_API_LLP_SA_AUTH_INCLUDED__
/* { */
#define __SOC_PPC_API_LLP_SA_AUTH_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/TMC/tmc_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Use to accept SA from all VIDS. Note that when SA is not
 *     found in the authentication DB, then it will be
 *     accepted.                                               */
#define  SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_VIDS (0xFFFFFFFF)

/*     Use to accept SA from all source system ports. Note that
 *     when SA is not found in the authentication DB, then it
 *     will be accepted.                                       */
#define  SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_PORTS (0xFFFFFFFF)

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
   *  Entries that used for port authentication
   */
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_PORT = 0x1,
  /*
   *  Entries that used for VID authentication
   */
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_VID = 0x2,
  /*
   *  All enitres used for SA-authentication
   */
  SOC_PPC_LLP_SA_MATCH_RULE_TYPE_ALL = (int)0xFFFFFFFF,
  /*
   *  Number of types in SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE
   */
  SOC_PPC_NOF_LLP_SA_AUTH_MATCH_RULE_TYPES = 3
}SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Whether to enable MAC SA authentication over packets
   *  entering the device from this port.
   */
  uint8 sa_auth_enable;

} SOC_PPC_LLP_SA_AUTH_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE and packets are untagged, then the
   *  Authentication test fails and proper action is taken.
   */
  uint8 tagged_only;
  /*
   *  Expected outer VLAN ID. If the outer VID of the incoming
   *  packet is not equal to this, the VID check fails and
   *  proper action is taken. Set to
   *  SOC_PPD_LLP_SA_AUTH_ACCEPT_ALL_VIDS in order to skip this
   *  check.
   */
  SOC_SAND_PP_VLAN_ID expect_tag_vid;
  /*
   *  Expected in system port. If the incoming packet enters
   *  from another system port, then this check fails and
   *  proper action is taken. Set id to
   *  SOC_PPD_LLP_SA_AUTH_ACCEPT_ALL_PORTS in order to skip this
   *  check. System port can be also LAG.
   */
  SOC_SAND_PP_SYS_PORT_ID expect_system_port;

} SOC_PPC_LLP_SA_AUTH_MAC_INFO;
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Rule type specifies which entries to return
   */
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE rule_type;
  /*
   *  Port value to match, use SOC_PPD_IGNORE_VAL to not compare
   *  to port.id value.
   */
  SOC_SAND_PP_SYS_PORT_ID port;
  /*
   *  VID value to match, use SOC_PPD_IGNORE_VAL to not compare to
   *  VID value.
   */
  SOC_SAND_PP_VLAN_ID vid;

} SOC_PPC_LLP_SA_AUTH_MATCH_RULE;


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
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_PORT_INFO *info
  );

void
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MAC_INFO *info
  );

void
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MATCH_RULE *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE enum_val
  );

void
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_PORT_INFO *info
  );

void
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MAC_INFO *info
  );

void
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MATCH_RULE *info
  );

#endif /* SOC_PPC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPC_API_LLP_SA_AUTH_INCLUDED__*/
#endif

