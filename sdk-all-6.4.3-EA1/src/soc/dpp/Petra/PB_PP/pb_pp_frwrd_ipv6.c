/* $Id: pb_pp_frwrd_ipv6.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_frwrd_ipv6.c
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
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv6.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ip_tcam.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_ipv6.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FRWRD_IPV6_TYPE_MAX                              (SOC_PB_PP_NOF_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPES-1)


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

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_frwrd_ipv6[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IPV6_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IP_TCAM_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IP_TCAM_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IP_TCAM_ROUTE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IP_TCAM_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_IP_TCAM_ROUTING_TABLE_CLEAR_UNSAFE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_frwrd_ipv6[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_FRWRD_IPV6_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_IPV6_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_IPV6_ROUTE_STATUS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_IPV6_ROUTE_STATUS_OUT_OF_RANGE_ERR",
    "The parameter 'route_status' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_IP_ROUTE_STATUSS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_IPV6_LOCATION_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_IPV6_LOCATION_OUT_OF_RANGE_ERR",
    "The parameter 'location' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_IPV6_FOUND_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_IPV6_FOUND_OUT_OF_RANGE_ERR",
    "The parameter 'found' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_IPV6_ROUTES_STATUS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_IPV6_ROUTES_STATUS_OUT_OF_RANGE_ERR",
    "The parameter 'routes_status' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_IP_ROUTE_STATUSS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_IPV6_ROUTES_LOCATION_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_IPV6_ROUTES_LOCATION_OUT_OF_RANGE_ERR",
    "The parameter 'routes_location' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_IPV6_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_IPV6_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_IPV6_EXACT_MATCH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_IPV6_EXACT_MATCH_OUT_OF_RANGE_ERR",
    "The parameter 'exact_match' is out of range. \n\r "
    "The range is: 1 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_IPV6_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_IPV6_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_IPV6_DEFAULT_ACTION_TYPE_NOT_SUPPORTED_ERR  ,
    "SOC_PB_PP_IPV6_DEFAULT_ACTION_TYPE_NOT_SUPPORTED_ERR  ",
    "IPv6 default action type is not supported \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_IPV6_DEFAULT_ACTION_WRONG_TRAP_CODE_ERR  ,
    "SOC_PB_PP_IPV6_DEFAULT_ACTION_WRONG_TRAP_CODE_ERR  ",
    "Trap code of IPv6 default action is wrong. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_IPV6_MC_ILLEGAL_DEST_TYPE_ERR,
    "SOC_PB_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR",
    "Destination in IPv4 MC routing info can be \n\r"
    "FEC-ptr or MC-group only\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_IP_TCAM_ENTRY_DOESNT_EXIST_ERR,
    "SOC_PB_PP_FRWRD_IP_TCAM_ENTRY_DOESNT_EXIST_ERR",
    "No entry matching the routing key was found.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_pp_frwrd_ipv6_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Setting global information of the IP routing (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_SET(regs->ihb.ipv6_cfg_reg.action_profile_default_mcv6_fwd, glbl_info->router_info.mc_default_action.value.action_profile.frwrd_action_strength, 10, exit);
  SOC_PB_PP_FLD_SET(regs->ihb.ipv6_cfg_reg.action_profile_default_mcv6_snp, glbl_info->router_info.mc_default_action.value.action_profile.snoop_action_strength, 20, exit);
  SOC_PB_PP_FLD_SET(regs->ihb.ipv6_cfg_reg.action_profile_default_ucv6_fwd, glbl_info->router_info.uc_default_action.value.action_profile.frwrd_action_strength, 30, exit);
  SOC_PB_PP_FLD_SET(regs->ihb.ipv6_cfg_reg.action_profile_default_ucv6_snp, glbl_info->router_info.uc_default_action.value.action_profile.snoop_action_strength, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_glbl_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ipv6_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO, glbl_info, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_info->router_info.uc_default_action.value.action_profile.frwrd_action_strength, SOC_PB_PP_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_MAX, SOC_PB_PP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_info->router_info.uc_default_action.value.action_profile.snoop_action_strength, SOC_PB_PP_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_MAX, SOC_PB_PP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_info->router_info.mc_default_action.value.action_profile.frwrd_action_strength, SOC_PB_PP_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_MAX, SOC_PB_PP_FRWRD_DEST_TRAP_FWD_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_info->router_info.mc_default_action.value.action_profile.snoop_action_strength, SOC_PB_PP_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_MAX, SOC_PB_PP_FRWRD_DEST_TRAP_SNOOP_OUT_OF_RANGE_ERR, 50, exit);

  if(glbl_info->router_info.uc_default_action.type != SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_IPV6_DEFAULT_ACTION_TYPE_NOT_SUPPORTED_ERR, 60, exit);
  }
  if(glbl_info->router_info.uc_default_action.value.action_profile.trap_code != SOC_PB_PP_TRAP_CODE_DEFAULT_UCV6)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_IPV6_DEFAULT_ACTION_WRONG_TRAP_CODE_ERR, 70, exit);
  }

  if(glbl_info->router_info.mc_default_action.type != SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_IPV6_DEFAULT_ACTION_TYPE_NOT_SUPPORTED_ERR, 80, exit);
  }
  if(glbl_info->router_info.mc_default_action.value.action_profile.trap_code != SOC_PB_PP_TRAP_CODE_DEFAULT_MCV6)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_IPV6_DEFAULT_ACTION_WRONG_TRAP_CODE_ERR, 90, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_glbl_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ipv6_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_GET_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_glbl_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Setting global information of the IP routing (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_clear(glbl_info);

  regs = soc_pb_pp_regs();

  glbl_info->router_info.mc_default_action.type = SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE;
  glbl_info->router_info.uc_default_action.type = SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE;

  SOC_PB_PP_FLD_GET(regs->ihb.ipv6_cfg_reg.action_profile_default_mcv6_fwd, glbl_info->router_info.mc_default_action.value.action_profile.frwrd_action_strength, 10, exit);
  SOC_PB_PP_FLD_GET(regs->ihb.ipv6_cfg_reg.action_profile_default_mcv6_snp, glbl_info->router_info.mc_default_action.value.action_profile.snoop_action_strength, 20, exit);
  SOC_PB_PP_FLD_GET(regs->ihb.ipv6_cfg_reg.action_profile_default_ucv6_fwd, glbl_info->router_info.uc_default_action.value.action_profile.frwrd_action_strength, 30, exit);
  SOC_PB_PP_FLD_GET(regs->ihb.ipv6_cfg_reg.action_profile_default_ucv6_snp, glbl_info->router_info.uc_default_action.value.action_profile.snoop_action_strength, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_glbl_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Add IPv6 route entry to the routing table. Binds between
 *     Ipv6 Unicast route key (IPv6-address/prefix) and a FEC
 *     entry identified by fec_id for a given virtual router.
 *     As a result of this operation, Unicast Ipv6 packets
 *     designated to IP address matching the given key (as long
 *     there is no more-specific route key) will be routed
 *     according to the information in the FEC entry identified
 *     by fec_id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_uc_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
	  res = SOC_SAND_OK,
    dest;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_ADD_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC;
  key.key.ipv6_uc = *route_key;
  key.vrf_ndx = 0;
  dest = (1 << 15) | fec_id;
  res = soc_pb_pp_frwrd_ip_tcam_route_add_unsafe(
          unit,
          &key,
          dest,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_uc_route_add_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ipv6_uc_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_ADD_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY, route_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fec_id, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_uc_route_add_verify()", 0, 0);
}

/*********************************************************************
*     Gets the routing information (system-fec-id) associated
 *     with the given route key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_uc_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
	  res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  SOC_SAND_CHECK_NULL_INPUT(found);

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC;
  key.key.ipv6_uc = *route_key;
  key.vrf_ndx = 0;
  res = soc_pb_pp_frwrd_ip_tcam_route_get_unsafe(
          unit,
          &key,
          exact_match,
          fec_id,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /*
   *  Convert from EM encoding to FEC pointer
   */
  *fec_id &= 0x3fff;
  
  if(location)
  {
    *location = SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION_TCAM;
  }
  if(route_status)
  {
    *route_status = SOC_PB_PP_FRWRD_IP_ROUTE_STATUS_COMMITED;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_route_get_unsafe()",0,0);
}


uint32
  soc_pb_pp_frwrd_ipv6_uc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY, route_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_uc_route_get_verify()", 0, 0);
}

/*********************************************************************
*     Gets the Ipv6 UC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_uc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE            *block_range_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    indx;

  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    *keys = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(fec_ids);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  if(block_range_key->entries_to_act == 0)
  {
    *nof_entries = 0;
    goto exit;
  }

  keys = sal_dpp_alloc(sizeof(SOC_PB_PP_IP_TCAM_ENTRY_KEY) * block_range_key->entries_to_act,"keys ipv6_uc_route_get_block MEM");
  if (keys == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit);
  }
  sal_memset(keys, 0, sizeof(SOC_PB_PP_IP_TCAM_ENTRY_KEY) * block_range_key->entries_to_act);

  for(indx = 0; indx < block_range_key->entries_to_act; ++indx)
  {
    keys[indx].type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC;
    keys[indx].key.ipv6_uc = route_keys[indx];
    keys[indx].vrf_ndx = 0;
  }
  res = soc_pb_pp_frwrd_ip_tcam_route_get_block_unsafe(
           unit,
           block_range_key,
           keys,
           fec_ids,
           nof_entries
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  for(indx = 0; indx < *nof_entries; ++indx)
  {
    route_keys[indx] = keys[indx].key.ipv6_uc;
    fec_ids[indx] &= 0x3fff;
    if(routes_location)
    {
      routes_location[indx] = SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION_TCAM;
    }
    if(routes_status)
    {
      routes_status[indx] = SOC_PB_PP_FRWRD_IP_ROUTE_STATUS_COMMITED;
    }
  }
exit:
  if(keys != NULL)
  {
    sal_free(keys);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_uc_route_get_block_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_frwrd_ipv6_uc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_IP_ROUTING_TABLE_RANGE, block_range_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_uc_route_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Remove IPv6 route entry from the routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_uc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
	  res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC;
  key.key.ipv6_uc = *route_key;
  key.vrf_ndx = 0;
  res = soc_pb_pp_frwrd_ip_tcam_route_remove_unsafe(
          unit,
          &key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  *success = SOC_SAND_SUCCESS;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_route_remove_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ipv6_uc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY, route_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_uc_route_remove_verify()", 0, 0);
}

/*********************************************************************
*     Clear the IPv6 UC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_uc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
	  res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC;
  key.vrf_ndx = 0;
  res = soc_pb_pp_frwrd_ip_tcam_routing_table_clear_unsafe(
          unit,
          &key,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_uc_route_clear_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ipv6_uc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_uc_routing_table_clear_verify()", 0, 0);
}

/*********************************************************************
*     Add IPv6 MC route entry to the routing table. Binds
 *     between Ipv6 Unicast route key (IPv6-address/prefix) and
 *     a FEC entry identified by fec_id for a given virtual
 *     router. As a result of this operation, Unicast Ipv6
 *     packets designated to IP address matching the given key
 *     (as long there is no more-specific route key) will be
 *     routed according to the information in the FEC entry
 *     identified by fec_id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_mc_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;
  uint32
    dest_val;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_ADD_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_MC;
  key.key.ipv6_mc = *route_key;

  /* build dest value */
  res = soc_pb_pp_frwrd_ipv4_sand_dest_to_em_dest(
          unit,
          &(route_info->dest_id),
          &dest_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_frwrd_ip_tcam_route_add_unsafe(
          unit,
          &key,
          dest_val,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_mc_route_add_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ipv6_mc_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *route_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_ADD_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY, route_key, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO, route_info, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_mc_route_add_verify()", 0, 0);
}

/*********************************************************************
*     Gets the routing information (system-fec-id) associated
 *     with the given route key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_mc_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;
  uint32
    dest_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_MC;
  key.key.ipv6_mc = *route_key;
  res = soc_pb_pp_frwrd_ip_tcam_route_get_unsafe(
          unit,
          &key,
          exact_match,
          &dest_val,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  if (*found)
  {
    /* parse dest value */
    res = soc_pb_pp_frwrd_ipv4_em_dest_to_sand_dest(
            unit,
            dest_val,
            &(route_info->dest_id)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if(location)
  {
    *location = SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION_TCAM;
  }
  if(*route_status)
  {
    *route_status = SOC_PB_PP_FRWRD_IP_ROUTE_STATUS_COMMITED;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_mc_route_get_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ipv6_mc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY, route_key, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_mc_route_get_verify()", 0, 0);
}

/*********************************************************************
*     Gets the Ipv6 MC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_mc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    indx;

  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    *keys = NULL;
  uint32
    *dest_vals = NULL;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_key);
    SOC_SAND_CHECK_NULL_INPUT(routes_info);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  if(block_range_key->entries_to_act == 0)
  {
    *nof_entries = 0;
    goto exit;
  }

  SOC_PETRA_ALLOC_ANY_SIZE(keys,SOC_PB_PP_IP_TCAM_ENTRY_KEY,block_range_key->entries_to_act);
  SOC_PETRA_ALLOC_ANY_SIZE(dest_vals,uint32,block_range_key->entries_to_act);

  for(indx = 0; indx < block_range_key->entries_to_act; ++indx)
  {
    keys[indx].type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_MC;
    keys[indx].key.ipv6_mc = route_key[indx];
  }

  res = soc_pb_pp_frwrd_ip_tcam_route_get_block_unsafe(
           unit,
           block_range_key,
           keys,
           dest_vals,
           nof_entries
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  for(indx = 0; indx < *nof_entries; ++indx)
  {
    route_key[indx] = keys[indx].key.ipv6_mc;
   /* parse dest value */
    res = soc_pb_pp_frwrd_ipv4_em_dest_to_sand_dest(
            unit,
            dest_vals[indx],
            &(routes_info[indx].dest_id)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if(routes_location)
    {
      routes_location[indx] = SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION_TCAM;
    }
    if(routes_status)
    {
      routes_status[indx] = SOC_PB_PP_FRWRD_IP_ROUTE_STATUS_COMMITED;
    }
  }
exit:
  if(keys != NULL)
  {
    sal_free(keys);
  }
  if(dest_vals != NULL)
  {
    SOC_PETRA_FREE_ANY_SIZE(dest_vals);
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_mc_route_get_block_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ipv6_mc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_IP_ROUTING_TABLE_RANGE, block_range_key, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_mc_route_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Remove IPv6 route entry from the routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_mc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_MC;
  key.key.ipv6_mc = *route_key;
  res = soc_pb_pp_frwrd_ip_tcam_route_remove_unsafe(
          unit,
          &key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  *success = SOC_SAND_SUCCESS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_mc_route_remove_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ipv6_mc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY             *route_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY, route_key, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_mc_route_remove_verify()", 0, 0);
}

/*********************************************************************
*     Clear the IPv6 MC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_mc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_MC;

  res = soc_pb_pp_frwrd_ip_tcam_routing_table_clear_unsafe(
          unit,
          &key,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_mc_route_clear_unsafe()",0,0);
}
uint32
  soc_pb_pp_frwrd_ipv6_mc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_mc_routing_table_clear_verify()", 0, 0);
}

/*********************************************************************
*     Setting global information of the VRF including
 *     (defaults forwarding).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VRF_INFO                 *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY
    route_key;
  SOC_SAND_SUCCESS_FAILURE
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vrf_info);

  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY_clear(&route_key);

  /*
   *  Enter a unicast catch-all route
   */
  route_key.subnet.prefix_len = 0;
  res = soc_pb_pp_frwrd_ipv6_vrf_route_add_unsafe(
          unit,
          vrf_ndx,
          &route_key,
          vrf_info->router_info.uc_default_action.value.fec_id,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_info_set_unsafe()", vrf_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_ipv6_vrf_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VRF_INFO                 *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, SOC_PB_PP_VRF_ID_MIN, SOC_PB_PP_VRF_ID_MAX, SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_VRF_INFO, vrf_info, 20, exit);
  if (vrf_info->router_info.uc_default_action.type != SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_FEC)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_IPV6_DEFAULT_ACTION_TYPE_NOT_SUPPORTED_ERR, 30, exit);
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_info_set_verify()", vrf_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_ipv6_vrf_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, SOC_PB_PP_VRF_ID_MIN, SOC_PB_PP_VRF_ID_MAX, SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_info_get_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Setting global information of the VRF including
 *     (defaults forwarding).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_VRF_INFO                 *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY
    route_key;
  SOC_PB_PP_FRWRD_IP_ROUTE_STATUS
    route_status;
  SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION
    location;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vrf_info);

  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_clear(vrf_info);
  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY_clear(&route_key);

  vrf_info->router_info.uc_default_action.type = SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_FEC;

  /*
   *  Get the unicast catch-all route
   */
  route_key.subnet.prefix_len = 0;
  res = soc_pb_pp_frwrd_ipv6_vrf_route_get_unsafe(
          unit,
          vrf_ndx,
          &route_key,
          TRUE,
          &vrf_info->router_info.uc_default_action.value.fec_id,
          &route_status,
          &location,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_info_get_unsafe()", vrf_ndx, 0);
}

/*********************************************************************
*     Add IPv6 route entry to the virtual routing table (VRF).
 *     Binds between Ipv6 route key (UC/MC IPv6-address\prefix)
 *     and a FEC entry identified by fec_id for a given virtual
 *     router. As a result of this operation, Unicast Ipv6
 *     packets designated to IP address matching the given key
 *     (as long there is no more-specific route key) will be
 *     routed according to the information in the FEC entry
 *     identified by fec_id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
	  res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_ADD_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN;
  key.key.ipv6_vpn = *route_key;
  key.vrf_ndx = vrf_ndx;
  res = soc_pb_pp_frwrd_ip_tcam_route_add_unsafe(
          unit,
          &key,
          SOC_SAND_BIT(15) | fec_id,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_vrf_route_add_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_ADD_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, SOC_PB_PP_VRF_ID_MIN, SOC_PB_PP_VRF_ID_MAX, SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY, route_key, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fec_id, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_route_add_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Gets the routing information (system-fec-id) associated
 *     with the given route key on VRF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
	  res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  SOC_SAND_CHECK_NULL_INPUT(found);

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN;
  key.key.ipv6_vpn = *route_key;
  key.vrf_ndx = vrf_ndx;
  res = soc_pb_pp_frwrd_ip_tcam_route_get_unsafe(
          unit,
          &key,
          exact_match,
          fec_id,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /*
   *  Convert from EM encoding to FEC pointer
   */
  *fec_id &= 0x3fff;

  if(location)
  {
    *location = SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION_TCAM;
  }
  if(route_status)
  {
    *route_status = SOC_PB_PP_FRWRD_IP_ROUTE_STATUS_COMMITED;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_vrf_route_get_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, SOC_PB_PP_VRF_ID_MIN, SOC_PB_PP_VRF_ID_MAX, SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY, route_key, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_route_get_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Gets the routing table of a virtual router (VRF).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    indx;

  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    *keys = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(fec_ids);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  if(block_range_key->entries_to_act == 0)
  {
    *nof_entries = 0;
    goto exit;
  }

  keys = sal_dpp_alloc(sizeof(SOC_PB_PP_IP_TCAM_ENTRY_KEY) * block_range_key->entries_to_act,"keys ipv6_vrf_route_get_block MEM");
  if (keys == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit);
  }
  sal_memset(keys, 0, sizeof(SOC_PB_PP_IP_TCAM_ENTRY_KEY) * block_range_key->entries_to_act);

  for(indx = 0; indx < block_range_key->entries_to_act; ++indx)
  {
    keys[indx].type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN;
    keys[indx].key.ipv6_vpn = route_keys[indx];
    keys[indx].vrf_ndx = vrf_ndx;
  }
  res = soc_pb_pp_frwrd_ip_tcam_route_get_block_unsafe(
           unit,
           block_range_key,
           keys,
           fec_ids,
           nof_entries
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  for(indx = 0; indx < *nof_entries; ++indx)
  {
    route_keys[indx] = keys[indx].key.ipv6_vpn;
    fec_ids[indx] &= 0x3fff;
    if(routes_location)
    {
      routes_location[indx] = SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION_TCAM;
    }
    if(routes_status)
    {
      routes_status[indx] = SOC_PB_PP_FRWRD_IP_ROUTE_STATUS_COMMITED;
    }
  }
exit:
  if(keys != NULL)
  {
    sal_free(keys);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_route_get_block_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, SOC_PB_PP_VRF_ID_MIN, SOC_PB_PP_VRF_ID_MAX, SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_IP_ROUTING_TABLE_RANGE, block_range_key, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_route_get_block_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Remove IPv6 route entry from the routing table of a
 *     virtual router (VRF).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
	  res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  
  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN;
  key.key.ipv6_vpn = *route_key;
  key.vrf_ndx = vrf_ndx;
  res = soc_pb_pp_frwrd_ip_tcam_route_remove_unsafe(
          unit,
          &key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  *success = SOC_SAND_SUCCESS;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_vrf_route_remove_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, SOC_PB_PP_VRF_ID_MIN, SOC_PB_PP_VRF_ID_MAX, SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY, route_key, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_route_remove_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Clear IPv6 routing table of VRF
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  )
{
  uint32
    res;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN;
  key.vrf_ndx = vrf_ndx;
  res = soc_pb_pp_frwrd_ip_tcam_routing_table_clear_unsafe(
          unit,
          &key,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ipv6_vrf_route_clear_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ipv6_vrf_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, SOC_PB_PP_VRF_ID_MIN, SOC_PB_PP_VRF_ID_MAX, SOC_PB_PP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_routing_table_clear_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Clear IPv6 routing tables for all VRFs.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_all_routing_tables_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_UNSAFE);

  key.type = SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN;
  res = soc_pb_pp_frwrd_ip_tcam_routing_table_clear_unsafe(
          unit,
          &key,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_all_routing_tables_clear_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ipv6_vrf_all_routing_tables_clear_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv6_vrf_all_routing_tables_clear_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_frwrd_ipv6 module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_ipv6_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_frwrd_ipv6;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_frwrd_ipv6 module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_ipv6_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_frwrd_ipv6;
}
uint32
  SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fec_id, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_ACTION_PROFILE, &(info->action_profile), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_FRWRD_IPV6_TYPE_MAX, SOC_PB_PP_FRWRD_IPV6_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL, &(info->value), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_IPV6_ROUTER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_ROUTER_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION, &(info->uc_default_action), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION, &(info->mc_default_action), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_IPV6_ROUTER_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_ROUTER_INFO, &(info->router_info), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VRF_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_IPV6_ROUTER_INFO, &(info->router_info), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_IPV6_VRF_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->dest_id.dest_type != SOC_SAND_PP_DEST_MULTICAST && info->dest_id.dest_type != SOC_SAND_PP_DEST_FEC)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
  }
  if (info->dest_id.dest_type == SOC_SAND_PP_DEST_FEC)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id.dest_val, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (info->dest_id.dest_type == SOC_SAND_PP_DEST_MULTICAST)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id.dest_val, SOC_PETRA_MULT_NOF_MULTICAST_GROUPS-1,SOC_PETRA_MULT_MC_ID_OUT_OF_RANGE_ERR,30,exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
