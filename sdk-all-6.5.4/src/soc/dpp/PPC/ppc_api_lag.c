/* $Id: ppc_api_lag.c,v 1.11 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppc/src/soc_ppc_api_lag.c
*
* MODULE PREFIX:  ppc
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPC

#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PPC/ppc_api_lag.h>

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
  SOC_PPC_LAG_HASH_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LAG_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LAG_HASH_GLOBAL_INFO));
  info->use_port_id = 0;
  info->seed = 0;
  info->hash_func_id = 0;
  info->key_shift = 0;
  info->eli_search = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_HASH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LAG_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LAG_HASH_PORT_INFO));
  info->nof_headers = 0;
  info->first_header_to_parse = SOC_PPC_NOF_LAG_HASH_FRST_HDRS;
  info->include_bos = FALSE;
  info->start_from_bos = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_HASH_MASK_INFO_clear(
    SOC_SAND_OUT SOC_PPC_HASH_MASK_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_HASH_MASK_INFO));
  info->mask = 0;
  info->is_symmetric_key = 0;
  info->expect_cw = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_MEMBER_clear(
    SOC_SAND_OUT SOC_PPC_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LAG_MEMBER));
  info->sys_port = 0;
  info->member_id = 0;
  info->flags = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LAG_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LAG_INFO));
  info->nof_entries = 0;
  for (ind = 0; ind < SOC_PPC_LAG_MEMBERS_MAX; ++ind)
  {
    SOC_PPC_LAG_MEMBER_clear(&(info->members[ind]));
  }
  info->lb_type = SOC_PPC_NOF_LAG_LB_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LAG_HASH_FRST_HDR_to_string(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_FRST_HDR enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LAG_HASH_FRST_HDR_FARWARDING:
    str = "farwarding";
  break;
  case SOC_PPC_LAG_HASH_FRST_HDR_LAST_TERMINATED:
    str = "last_terminated";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_LAG_LB_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LAG_LB_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LAG_LB_TYPE_HASH:
    str = "hash";
    break;
  case SOC_PPC_LAG_LB_TYPE_ROUND_ROBIN:
    str = "round_robin";
    break;
  case SOC_PPC_LAG_LB_TYPE_SMOOTH_DIVISION:
    str = "smooth_division";
    break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_LAG_HASH_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "use_port_id: %u\n\r"),info->use_port_id));
  LOG_CLI((BSL_META_U(unit,
                      "seed: %u\n\r"), info->seed));
  LOG_CLI((BSL_META_U(unit,
                      "hash_func_id: %u\n\r"), info->hash_func_id));
  LOG_CLI((BSL_META_U(unit,
                      "key_shift: %u\n\r"), info->key_shift));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_HASH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "nof_headers: %u\n\r"), info->nof_headers));
  LOG_CLI((BSL_META_U(unit,
                      "first_header_to_parse %s "), SOC_PPC_LAG_HASH_FRST_HDR_to_string(info->first_header_to_parse)));

  LOG_CLI((BSL_META_U(unit,
                      "start_from_bos: %u\n\r"), info->start_from_bos));
  LOG_CLI((BSL_META_U(unit,
                      "include_bos: %u\n\r"), info->include_bos));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_HASH_MASK_INFO_print(
    SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "key includes:\n\r")));
  for (ind = 0 ; ind < SOC_PPC_NOF_HASH_MASKS; ++ind)
  {
    if ((SOC_SAND_BIT(ind) & info->mask) == 0)
    {
      LOG_CLI((BSL_META_U(unit,
                          " %s \n\r"), SOC_PPC_HASH_MASKS_to_string(SOC_SAND_BIT(ind))));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "key excludes:\n\r")));
    for (ind = 0 ; ind < SOC_PPC_NOF_HASH_MASKS; ++ind)
    {
      if ((SOC_SAND_BIT(ind) & info->mask) != 0)
      {
        LOG_CLI((BSL_META_U(unit,
                            " %s \n\r"), SOC_PPC_HASH_MASKS_to_string(SOC_SAND_BIT(ind))));
      }
    }
  LOG_CLI((BSL_META_U(unit,
                      "is_symmetric_key: %u\n\r"),info->is_symmetric_key));
  LOG_CLI((BSL_META_U(unit,
                      "expect_cw: %u\n\r"),info->expect_cw));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_MEMBER_print(
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "sys_port: %u\n\r"),info->sys_port));
  LOG_CLI((BSL_META_U(unit,
                      "member_id: %u\n\r"),info->member_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LAG_INFO_print(
    SOC_SAND_IN  SOC_PPC_LAG_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "nof_entries: %u\n\r"),info->nof_entries));
  for (ind = 0; ind < info->nof_entries; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "members[%u]:"),ind));
    SOC_PPC_LAG_MEMBER_print(&(info->members[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "lb_type %s "), SOC_PPC_LAG_LB_TYPE_to_string(info->lb_type)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

