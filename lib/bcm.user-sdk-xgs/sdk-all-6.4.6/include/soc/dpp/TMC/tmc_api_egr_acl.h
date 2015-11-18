/* $Id: soc_tmcapi_egr_acl.h,v 1.3 Broadcom SDK $
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

#ifndef __SOC_TMC_API_EGR_ACL_INCLUDED__
/* { */
#define __SOC_TMC_API_EGR_ACL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_tcam.h>
#include <soc/dpp/TMC/tmc_api_tcam_key.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Number of Drop Precedence values                        */
#define  SOC_TMC_EGR_ACL_NOF_DP_VALUES (2)

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

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  DP value. Range: 0 - 3.
   */
  uint32 val[SOC_TMC_EGR_ACL_NOF_DP_VALUES];

} SOC_TMC_EGR_ACL_DP_VALUES;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Port ACL profile Range: 0 - 3.
   */
  uint32 profile;
  /*
   *  ACL Data part of the key construction. Range: 0 - 63.
   */
  uint32 acl_data;

} SOC_TMC_EGR_ACL_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If True, the packet is trapped according to the Action
   *  profile 'trap_code'.
   */
  uint8 trap_en;
  /*
   *  Trap code: relevant only if the trapping 'trap_en' is
   *  enabled. Range: 0 - 7.
   */
  uint32 trap_code;
  /*
   *  OFP override. If True, the value in 'ofp' parameter is
   *  the Out-TM-Port.
   */
  uint8 ofp_ov;
  /*
   *  OFP (Out-TM-Port) value if an override is allowed.
   *  Range: 0 - 79.
   */
  uint32 ofp;
  /*
   *  Traffic Class and Drop Precedence override. If True, the
   *  values in 'tc' and 'dp' parameters are meaningful.
   */
  uint8 dp_tc_ov;
  /*
   *  Traffic Class value if an override is allowed. Range: 0
   *  - 7.
   */
  uint32 tc;
  /*
   *  Drop Precedence value if an override is allowed. Range:
   *  0 - 1. Set according to soc_pb_egr_acl_dp_values_set
   */
  uint32 dp;
  /*
   *  Copy-Unique-Data value if an override is allowed. A
   *  value equal to '0' disables the overriding.
   */
  uint32 cud;

} SOC_TMC_EGR_ACL_ACTION_VAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Entry format: must be identical to its Database type.
   */
  SOC_TMC_EGR_ACL_DB_TYPE type;
  /*
   *  Key value. The key can be built with the use of helper
   *  functions (e.g., soc_pb_tcam_key_masked_val_set).
   */
  SOC_TMC_TCAM_KEY key;
  /*
   *  Entry priority.
   */
  uint16 priority;
  /*
   *  Action values for all the possible actions.
   */
  SOC_TMC_EGR_ACL_ACTION_VAL action_val;

} SOC_TMC_EGR_ACL_ENTRY_INFO;


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
  SOC_TMC_EGR_ACL_DP_VALUES_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_DP_VALUES *info
  );

void
  SOC_TMC_EGR_ACL_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_PORT_INFO *info
  );

void
  SOC_TMC_EGR_ACL_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_ACTION_VAL *info
  );

void
  SOC_TMC_EGR_ACL_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_ENTRY_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_EGR_ACL_DP_VALUES_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_DP_VALUES *info
  );

void
  SOC_TMC_EGR_ACL_PORT_INFO_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_PORT_INFO *info
  );

void
  SOC_TMC_EGR_ACL_ACTION_VAL_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_ACTION_VAL *info
  );

void
  SOC_TMC_EGR_ACL_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_ENTRY_INFO *info
  );

#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_TMC_API_EGR_ACL_INCLUDED__*/
#endif

