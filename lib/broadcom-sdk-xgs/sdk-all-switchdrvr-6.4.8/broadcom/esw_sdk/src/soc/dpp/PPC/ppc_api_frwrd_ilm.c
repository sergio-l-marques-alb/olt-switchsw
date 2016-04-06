/* $Id: ppc_api_frwrd_ilm.c,v 1.10 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppc/src/soc_ppc_api_frwrd_ilm.c
*
* MODULE PREFIX:  soc_ppc_frwrd
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

#include <soc/dpp/PPC/ppc_api_frwrd_ilm.h>

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
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO));
  info->mask_port = 0;
  info->mask_inrif = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO));
  soc_sand_SAND_U32_RANGE_clear(&(info->labels_range));
  for (ind = 0; ind < SOC_SAND_PP_NOF_BITS_IN_EXP; ++ind)
  {
    info->exp_map_tbl[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_ILM_KEY));
  info->in_label = 0;
  info->mapped_exp = 0;
  info->in_local_port = 0;
  info->inrif = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_ILM_GLBL_INFO));
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_clear(&(info->key_info));
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_clear(&(info->elsp_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mask_port: %u\n\r"),info->mask_port));
  LOG_CLI((BSL_META_U(unit,
                      "mask_inrif: %u\n\r"),info->mask_inrif));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "labels_range: ")));
  soc_sand_SAND_U32_RANGE_print(&(info->labels_range));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  for (ind = 0; ind < SOC_SAND_PP_NOF_BITS_IN_EXP; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "exp_map_tbl[%u]: %u\n\r"), ind,info->exp_map_tbl[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "in_label: %u\n\r"),info->in_label));
  LOG_CLI((BSL_META_U(unit,
                      "mapped_exp: %u\n\r"), info->mapped_exp));
  LOG_CLI((BSL_META_U(unit,
                      "in_local_port: %u\n\r"),info->in_local_port));
  LOG_CLI((BSL_META_U(unit,
                      "inrif: %u\n\r"),info->inrif));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "key_info:")));
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_print(&(info->key_info));
  LOG_CLI((BSL_META_U(unit,
                      "elsp_info:")));
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_print(&(info->elsp_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

