/* $Id: pb_pp_api_general.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_api_general.c
*
* MODULE PREFIX:  soc_pb_pp
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>

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
  SOC_PB_PP_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_OUTLIF_clear(
    SOC_SAND_OUT SOC_PB_PP_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OUTLIF_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_MPLS_COMMAND_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_COMMAND_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EEI_VAL_clear(
    SOC_SAND_OUT SOC_PB_PP_EEI_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EEI_clear(
    SOC_SAND_OUT SOC_PB_PP_EEI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_DECISION_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_DECISION_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_TYPE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_ACTION_PROFILE_clear(
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_ACTION_PROFILE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_TPID_PROFILE_clear(
    SOC_SAND_OUT SOC_PB_PP_TPID_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TPID_PROFILE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_PEP_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_PEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_PEP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_IP_ROUTING_TABLE_ITER_clear(
    SOC_SAND_OUT SOC_PB_PP_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_ITER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_IP_ROUTING_TABLE_RANGE_clear(
    SOC_SAND_OUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_EEI_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EEI_TYPE enum_val
  )
{
  return SOC_PPC_EEI_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_OUTLIF_ENCODE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_OUTLIF_ENCODE_TYPE enum_val
  )
{
  return SOC_PPC_OUTLIF_ENCODE_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FRWRD_DECISION_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_DECISION_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_MPLS_COMMAND_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_MPLS_COMMAND_TYPE enum_val
  )
{
  return SOC_PPC_MPLS_COMMAND_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_L2_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_L2_NEXT_PRTCL_TYPE enum_val
  )
{
  return SOC_PPC_L2_NEXT_PRTCL_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_L3_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_L3_NEXT_PRTCL_TYPE enum_val
  )
{
  return SOC_PPC_L3_NEXT_PRTCL_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_PKT_TERM_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_PKT_TERM_TYPE enum_val
  )
{
  return SOC_PPC_PKT_TERM_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_PKT_FRWRD_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_PKT_FRWRD_TYPE enum_val
  )
{
  return SOC_PPC_PKT_FRWRD_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_PKT_HDR_TYPE enum_val
  )
{
  return SOC_PPC_PKT_HDR_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_PKT_HDR_STK_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_PKT_HDR_STK_TYPE enum_val
  )
{
  return SOC_PPC_PKT_HDR_STK_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_IP_ROUTING_TABLE_ITER_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_IP_ROUTING_TABLE_ITER_TYPE enum_val
  )
{
  return SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_HASH_MASKS_to_string(
    SOC_SAND_IN  SOC_PB_PP_HASH_MASKS enum_val
  )
{
  return SOC_PPC_HASH_MASKS_to_string(enum_val);
}

void
  SOC_PB_PP_TRAP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_OUTLIF_print(
    SOC_SAND_IN  SOC_PB_PP_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OUTLIF_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_MPLS_COMMAND_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_COMMAND_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EEI_VAL_print(
    SOC_SAND_IN  SOC_PB_PP_EEI_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EEI_print(
    SOC_SAND_IN  SOC_PB_PP_EEI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_DECISION_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE      frwrd_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_TYPE_INFO_print(frwrd_type,info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_ACTION_PROFILE_print(
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_ACTION_PROFILE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_TPID_PROFILE_print(
    SOC_SAND_IN  SOC_PB_PP_TPID_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TPID_PROFILE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_PEP_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_PEP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_IP_ROUTING_TABLE_ITER_print(
    SOC_SAND_IN  SOC_PB_PP_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_ITER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_IP_ROUTING_TABLE_RANGE_print(
    SOC_SAND_IN  SOC_PB_PP_IP_ROUTING_TABLE_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_RANGE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

