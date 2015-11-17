#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_general.c,v 1.7 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

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
  ARAD_PP_TRAP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_OUTLIF_clear(
    SOC_SAND_OUT ARAD_PP_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OUTLIF_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_COMMAND_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_COMMAND_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EEI_VAL_clear(
    SOC_SAND_OUT ARAD_PP_EEI_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EEI_clear(
    SOC_SAND_OUT ARAD_PP_EEI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_DECISION_TYPE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_DECISION_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_TYPE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_ACTION_PROFILE_clear(
    SOC_SAND_OUT ARAD_PP_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_ACTION_PROFILE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TPID_PROFILE_clear(
    SOC_SAND_OUT ARAD_PP_TPID_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TPID_PROFILE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_PEP_KEY_clear(
    SOC_SAND_OUT ARAD_PP_PEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_PEP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_IP_ROUTING_TABLE_ITER_clear(
    SOC_SAND_OUT ARAD_PP_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_ITER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_IP_ROUTING_TABLE_RANGE_clear(
    SOC_SAND_OUT ARAD_PP_IP_ROUTING_TABLE_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/*********************************************************************
* Checks whether l2_next_prtcl_type can be successfully allocated.
*********************************************************************/
uint32
  arad_pp_l2_next_prtcl_type_allocate_test(
    SOC_SAND_IN  int 							  unit,
    SOC_SAND_IN  uint32 							  l2_next_prtcl_type,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE			  *success
  )
{ 
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_l2_next_prtcl_type_allocate_test_verify(
          unit,
          l2_next_prtcl_type,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_next_prtcl_type_allocate_test_unsafe(
		unit,
		l2_next_prtcl_type,
		success
	  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_next_prtcl_type_allocate_test()", l2_next_prtcl_type, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_EEI_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_EEI_TYPE enum_val
  )
{
  return SOC_PPC_EEI_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_OUTLIF_ENCODE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_OUTLIF_ENCODE_TYPE enum_val
  )
{
  return SOC_PPC_OUTLIF_ENCODE_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_FRWRD_DECISION_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_DECISION_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_MPLS_COMMAND_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_MPLS_COMMAND_TYPE enum_val
  )
{
  return SOC_PPC_MPLS_COMMAND_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_L2_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_L2_NEXT_PRTCL_TYPE enum_val
  )
{
  return SOC_PPC_L2_NEXT_PRTCL_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_L3_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_L3_NEXT_PRTCL_TYPE enum_val
  )
{
  return SOC_PPC_L3_NEXT_PRTCL_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_PKT_TERM_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_PKT_TERM_TYPE enum_val
  )
{
  return SOC_PPC_PKT_TERM_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_PKT_FRWRD_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_PKT_FRWRD_TYPE enum_val
  )
{
  return SOC_PPC_PKT_FRWRD_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_PKT_HDR_TYPE enum_val
  )
{
  return SOC_PPC_PKT_HDR_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_PKT_HDR_STK_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_PKT_HDR_STK_TYPE enum_val
  )
{
  return SOC_PPC_PKT_HDR_STK_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_IP_ROUTING_TABLE_ITER_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_IP_ROUTING_TABLE_ITER_TYPE enum_val
  )
{
  return SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_HASH_MASKS_to_string(
    SOC_SAND_IN  ARAD_PP_HASH_MASKS enum_val
  )
{
  return SOC_PPC_HASH_MASKS_to_string(enum_val);
}

void
  ARAD_PP_TRAP_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_OUTLIF_print(
    SOC_SAND_IN  ARAD_PP_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OUTLIF_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_COMMAND_print(
    SOC_SAND_IN  ARAD_PP_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_COMMAND_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EEI_VAL_print(
    SOC_SAND_IN  ARAD_PP_EEI_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_EEI_print(
    SOC_SAND_IN  ARAD_PP_EEI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_DECISION_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE      frwrd_type,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_TYPE_INFO_print(frwrd_type,info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_ACTION_PROFILE_print(
    SOC_SAND_IN  ARAD_PP_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_ACTION_PROFILE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TPID_PROFILE_print(
    SOC_SAND_IN  ARAD_PP_TPID_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TPID_PROFILE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_PEP_KEY_print(
    SOC_SAND_IN  ARAD_PP_PEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_PEP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_IP_ROUTING_TABLE_ITER_print(
    SOC_SAND_IN  ARAD_PP_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_ITER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_IP_ROUTING_TABLE_RANGE_print(
    SOC_SAND_IN  ARAD_PP_IP_ROUTING_TABLE_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_RANGE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

