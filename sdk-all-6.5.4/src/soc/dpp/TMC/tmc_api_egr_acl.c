/* $Id: tmc_api_egr_acl.c,v 1.5 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */



#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_egr_acl.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 *  MACROS   *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

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
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_EGR_ACL_DP_VALUES));
  for (ind = 0; ind < SOC_TMC_EGR_ACL_NOF_DP_VALUES; ++ind)
  {
    info->val[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_EGR_ACL_PORT_INFO));
  info->profile = 0;
  info->acl_data = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_EGR_ACL_ACTION_VAL));
  info->trap_en = 0;
  info->trap_code = 0;
  info->ofp_ov = 0;
  info->ofp = 0;
  info->dp_tc_ov = 0;
  info->tc = 0;
  info->dp = 0;
  info->cud = 0xFFFF;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_EGR_ACL_ENTRY_INFO));
  info->type = SOC_TMC_EGR_NOF_ACL_DB_TYPES;
  info->priority = 0;
  SOC_TMC_TCAM_KEY_clear(&(info->key));
  SOC_TMC_EGR_ACL_ACTION_VAL_clear(&(info->action_val));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_EGR_ACL_DP_VALUES_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_DP_VALUES *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_EGR_ACL_NOF_DP_VALUES; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "val[%u]: %u\n\r"),ind,info->val[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_PORT_INFO_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "profile: %u\n\r"),info->profile));
  LOG_CLI((BSL_META_U(unit,
                      "acl_data: %u\n\r"),info->acl_data));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_ACTION_VAL_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "trap_en: %u\n\r"),info->trap_en));
  LOG_CLI((BSL_META_U(unit,
                      "trap_code: %u\n\r"),info->trap_code));
  LOG_CLI((BSL_META_U(unit,
                      "ofp_ov: %u\n\r"),info->ofp_ov));
  LOG_CLI((BSL_META_U(unit,
                      "ofp: %u\n\r"),info->ofp));
  LOG_CLI((BSL_META_U(unit,
                      "dp_tc_ov: %u\n\r"),info->dp_tc_ov));
  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"),info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "cud: %u\n\r"),info->cud));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_TMC_EGR_ACL_DB_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "priority: %u\n\r"),info->priority));
  LOG_CLI((BSL_META_U(unit,
                      "action_val:")));
  SOC_TMC_EGR_ACL_ACTION_VAL_print(&(info->action_val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

