/* $Id: ppd_api_general.c,v 1.14 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_general.c
*
* MODULE PREFIX:  ppd
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPD

#include <shared/bsl.h>
#include <soc/dpp/drv.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_general.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_general.h>
#endif

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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Ppd_procedure_desc_element_general[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_GENERAL_GET_PROCS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FWD_DECISION_TO_SAND_DEST),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SAND_DEST_TO_FWD_DECISION),
  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};
/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */



uint32
  soc_ppd_fwd_decision_to_sand_dest(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN SOC_PPD_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID *dest_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FWD_DECISION_TO_SAND_DEST);

  soc_sand_SAND_PP_DESTINATION_ID_clear(dest_id);
  switch (fwd_decision->type)
  {
  case SOC_PPD_FRWRD_DECISION_TYPE_DROP:
    dest_id->dest_type = SOC_SAND_PP_DEST_TYPE_DROP;
  break;
  case SOC_PPD_FRWRD_DECISION_TYPE_UC_FLOW:
    dest_id->dest_type = SOC_SAND_PP_DEST_EXPLICIT_FLOW;
    dest_id->dest_val = fwd_decision->dest_id;
  break;
  case SOC_PPD_FRWRD_DECISION_TYPE_UC_LAG:
    dest_id->dest_type = SOC_SAND_PP_DEST_LAG;
    dest_id->dest_val = fwd_decision->dest_id;
  break;
  case SOC_PPD_FRWRD_DECISION_TYPE_UC_PORT:
    dest_id->dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
    dest_id->dest_val = fwd_decision->dest_id;
  break;
  case SOC_PPD_FRWRD_DECISION_TYPE_MC:
    dest_id->dest_type = SOC_SAND_PP_DEST_MULTICAST;
    dest_id->dest_val = fwd_decision->dest_id;
  break;
  case SOC_PPD_FRWRD_DECISION_TYPE_FEC:
    dest_id->dest_type = SOC_SAND_PP_DEST_FEC;
    dest_id->dest_val = fwd_decision->dest_id;
  break;
  case SOC_PPD_FRWRD_DECISION_TYPE_TRAP:
    dest_id->dest_type = SOC_SAND_PP_DEST_TRAP;
    SOC_PPD_DEST_TRAP_VAL_SET(dest_id->dest_val,
                          fwd_decision->additional_info.trap_info.action_profile.trap_code,
                          fwd_decision->additional_info.trap_info.action_profile.frwrd_action_strength,
                          fwd_decision->additional_info.trap_info.action_profile.snoop_action_strength
                         );
  break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR,10,exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_fwd_decision_to_sand_dest()",0,0);
}


uint32
  soc_ppd_sand_dest_to_fwd_decision(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *dest_id,
    SOC_SAND_OUT SOC_PPD_FRWRD_DECISION_INFO *fwd_decision
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_SAND_DEST_TO_FWD_DECISION);

  SOC_PPD_FRWRD_DECISION_INFO_clear(fwd_decision);
  switch (dest_id->dest_type)
  {
  case SOC_SAND_PP_DEST_TYPE_DROP:
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, fwd_decision, res);
  break;
  case SOC_SAND_PP_DEST_EXPLICIT_FLOW:
    SOC_PPD_FRWRD_DECISION_EXPL_FLOW_SET(unit, fwd_decision, dest_id->dest_val, res);
  break;
  case SOC_SAND_PP_DEST_LAG:
    SOC_PPD_FRWRD_DECISION_LAG_SET(unit, fwd_decision, dest_id->dest_val, res);
  break;
  case SOC_SAND_PP_DEST_SINGLE_PORT:
    SOC_PPD_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, fwd_decision, dest_id->dest_val, res);
  break;
  case SOC_SAND_PP_DEST_MULTICAST:
    SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, fwd_decision, dest_id->dest_val, res);
  break;
  case SOC_SAND_PP_DEST_FEC:
    SOC_PPD_FRWRD_DECISION_FEC_SET(unit, fwd_decision, dest_id->dest_val, res);
  break;
  case SOC_SAND_PP_DEST_TRAP: /* not supported as no trap information in soc_sand-dest */
      SOC_PPD_FRWRD_DECISION_TRAP_SET(unit, fwd_decision, SOC_PPD_DEST_TRAP_VAL_GET_TRAP_CODE(dest_id->dest_val), SOC_PPD_DEST_TRAP_VAL_GET_FWD_STRENGTH(dest_id->dest_val), SOC_PPD_DEST_TRAP_VAL_GET_SNP_STRENGTH(dest_id->dest_val), res);
  break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_sand_dest_to_fwd_decision()",0,0);
}

/*********************************************************************
* Checks whether l2_next_prtcl_type can be successfully allocated.
*********************************************************************/
uint32
  soc_ppd_l2_next_prtcl_type_allocate_test(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                l2_next_prtcl_type,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(l2_next_prtcl_type_allocate_test,(unit, l2_next_prtcl_type, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_next_prtcl_type_allocate_test()", l2_next_prtcl_type, 0);
}


/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_general module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/


CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_general_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_general;
}
void
  SOC_PPD_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OUTLIF_clear(
    SOC_SAND_OUT SOC_PPD_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OUTLIF_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MPLS_COMMAND_clear(
    SOC_SAND_OUT SOC_PPD_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_COMMAND_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EEI_VAL_clear(
    SOC_SAND_OUT SOC_PPD_EEI_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EEI_clear(
    SOC_SAND_OUT SOC_PPD_EEI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_DECISION_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_DECISION_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_TYPE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_ACTION_PROFILE_clear(
    SOC_SAND_OUT SOC_PPD_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_ACTION_PROFILE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TPID_PROFILE_clear(
    SOC_SAND_OUT SOC_PPD_TPID_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TPID_PROFILE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_PEP_KEY_clear(
    SOC_SAND_OUT SOC_PPD_PEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_PEP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_IP_ROUTING_TABLE_ITER_clear(
    SOC_SAND_OUT SOC_PPD_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_ITER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_IP_ROUTING_TABLE_RANGE_clear(
    SOC_SAND_OUT SOC_PPD_IP_ROUTING_TABLE_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_ADDITIONAL_TPID_VALUES_clear(
    SOC_SAND_OUT SOC_PPC_ADDITIONAL_TPID_VALUES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_ADDITIONAL_TPID_VALUES_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_EEI_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_EEI_TYPE enum_val
  )
{
  return SOC_PPC_EEI_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_OUTLIF_ENCODE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_OUTLIF_ENCODE_TYPE enum_val
  )
{
  return SOC_PPC_OUTLIF_ENCODE_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_DECISION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_DECISION_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_DECISION_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_MPLS_COMMAND_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_MPLS_COMMAND_TYPE enum_val
  )
{
  return SOC_PPC_MPLS_COMMAND_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_L2_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_L2_NEXT_PRTCL_TYPE enum_val
  )
{
  return SOC_PPC_L2_NEXT_PRTCL_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_L3_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_L3_NEXT_PRTCL_TYPE enum_val
  )
{
  return SOC_PPC_L3_NEXT_PRTCL_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_PKT_TERM_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_PKT_TERM_TYPE enum_val
  )
{
  return SOC_PPC_PKT_TERM_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_PKT_FRWRD_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_PKT_FRWRD_TYPE enum_val
  )
{
  return SOC_PPC_PKT_FRWRD_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_PKT_HDR_TYPE enum_val
  )
{
  return SOC_PPC_PKT_HDR_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_PKT_HDR_STK_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_PKT_HDR_STK_TYPE enum_val
  )
{
  return SOC_PPC_PKT_HDR_STK_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_IP_ROUTING_TABLE_ITER_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_IP_ROUTING_TABLE_ITER_TYPE enum_val
  )
{
  return SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_HASH_MASKS_to_string(
    SOC_SAND_IN  SOC_PPD_HASH_MASKS enum_val
  )
{
  return SOC_PPC_HASH_MASKS_to_string(enum_val);
}

void
  SOC_PPD_TRAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_OUTLIF_print(
    SOC_SAND_IN  SOC_PPD_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_OUTLIF_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MPLS_COMMAND_print(
    SOC_SAND_IN  SOC_PPD_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_COMMAND_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EEI_VAL_print(
    SOC_SAND_IN  SOC_PPD_EEI_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EEI_print(
    SOC_SAND_IN  SOC_PPD_EEI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EEI_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_DECISION_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_DECISION_TYPE      frwrd_type,
    SOC_SAND_IN  SOC_PPD_FRWRD_DECISION_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_TYPE_INFO_print(frwrd_type, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_DECISION_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_ACTION_PROFILE_print(
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_ACTION_PROFILE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_TPID_PROFILE_print(
    SOC_SAND_IN  SOC_PPD_TPID_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TPID_PROFILE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_PEP_KEY_print(
    SOC_SAND_IN  SOC_PPD_PEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_PEP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_IP_ROUTING_TABLE_ITER_print(
    SOC_SAND_IN  SOC_PPD_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_ITER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_IP_ROUTING_TABLE_RANGE_print(
    SOC_SAND_IN  SOC_PPD_IP_ROUTING_TABLE_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_IP_ROUTING_TABLE_RANGE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_ADDITIONAL_TPID_VALUES_print(
    SOC_SAND_IN  SOC_PPD_ADDITIONAL_TPID_VALUES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_ADDITIONAL_TPID_VALUES_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

