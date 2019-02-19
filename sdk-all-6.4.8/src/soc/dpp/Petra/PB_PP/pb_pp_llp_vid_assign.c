/* $Id: pb_pp_llp_vid_assign.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_llp_vid_assign.c
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
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_vid_assign.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_sa_auth.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_cos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LLP_VID_ASSIGN_ENTRY_NDX_MAX                     (15)
#define SOC_PB_PP_LLP_VID_ASSIGN_PORT_PROFILE_NDX_MAX              (7)
#define SOC_PB_PP_LLP_VID_ASSIGN_RULE_TYPE_MAX                     (SOC_PB_PP_NOF_LLP_VID_ASSIGN_MATCH_RULE_TYPES-1)

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

static
  SOC_PROCEDURE_DESC_ELEMENT
    Soc_pb_pp_procedure_desc_element_llp_vid_assign[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_VID_ASSIGN_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_llp_vid_assign[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LLP_VID_ASSIGN_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_VID_ASSIGN_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_VID_ASSIGN_ENTRY_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_VID_ASSIGN_ENTRY_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'entry_ndx' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_VID_ASSIGN_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_VID_ASSIGN_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_VID_ASSIGN_ETHER_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_VID_ASSIGN_ETHER_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'ether_type_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_VID_ASSIGN_RULE_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_VID_ASSIGN_RULE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'rule_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LLP_SA_MATCH_RULE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_LLP_VID_ASSIGN_SUBNET_PREF_IS_ZERO_ERR,
    "SOC_PB_PP_LLP_VID_ASSIGN_SUBNET_PREF_IS_ZERO_ERR",
    "subnet prefix cannot be zero. \n\r "
    "soc_pb_pp_llp_vid_assign_ipv4_subnet_based_set, \n\r "
    "The range is: 1 - 32.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_VID_ASSIGN_DIS_AUTH_EN_ERR,
    "SOC_PB_PP_LLP_VID_ASSIGN_DIS_AUTH_EN_ERR",
    "Cannot disable VID assignment on port when Authentication. \n\r "
    "is enabled on port first call: soc_ppd_llp_sa_auth_port_info_set\n\r "
    "to disable authentication.\n\r ",
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
  soc_pb_pp_llp_vid_assign_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets port information for VID assignment, including PVID
 *     and which assignment mechanism to enable for this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_vid_assign_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  )
{
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    ihp_pinfo_llr_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_vid_assign_info);

  res =   soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
            unit,
            local_port_ndx,
            &ihp_pinfo_llr_tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (ihp_pinfo_llr_tbl_data.enable_sa_authentication && !port_vid_assign_info->enable_sa_based)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_VID_ASSIGN_DIS_AUTH_EN_ERR,15,exit);
  }
  ihp_pinfo_llr_tbl_data.default_initial_vid = port_vid_assign_info->pvid;
  ihp_pinfo_llr_tbl_data.ignore_incoming_vid = SOC_SAND_BOOL2NUM(port_vid_assign_info->ignore_incoming_tag);
  ihp_pinfo_llr_tbl_data.vid_port_protocol_enable = SOC_SAND_BOOL2NUM(port_vid_assign_info->enable_protocol);
  ihp_pinfo_llr_tbl_data.vid_ipv4_subnet_enable = SOC_SAND_BOOL2NUM(port_vid_assign_info->enable_subnet_ip);
  ihp_pinfo_llr_tbl_data.sa_lookup_enable = SOC_SAND_BOOL2NUM(port_vid_assign_info->enable_sa_based);

  res =   soc_pb_pp_ihp_pinfo_llr_tbl_set_unsafe(
            unit,
            local_port_ndx,
            &ihp_pinfo_llr_tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

 /*
  * save to SW DB
  */
  res = soc_pb_pp_sw_db_llp_vid_assign_port_sa_set(
          unit,
          local_port_ndx,
          port_vid_assign_info->enable_sa_based
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_port_info_set_unsafe()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_llp_vid_assign_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO, port_vid_assign_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_llp_vid_assign_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_port_info_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets port information for VID assignment, including PVID
 *     and which assignment mechanism to enable for this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_vid_assign_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  )
{
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    ihp_pinfo_llr_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_vid_assign_info);

  SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_clear(port_vid_assign_info);

  res =   soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
            unit,
            local_port_ndx,
            &ihp_pinfo_llr_tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  port_vid_assign_info->pvid = ihp_pinfo_llr_tbl_data.default_initial_vid;
  port_vid_assign_info->ignore_incoming_tag = SOC_SAND_NUM2BOOL(ihp_pinfo_llr_tbl_data.ignore_incoming_vid);
  port_vid_assign_info->enable_protocol = SOC_SAND_NUM2BOOL(ihp_pinfo_llr_tbl_data.vid_port_protocol_enable);
  port_vid_assign_info->enable_subnet_ip = SOC_SAND_NUM2BOOL(ihp_pinfo_llr_tbl_data.vid_ipv4_subnet_enable);
  port_vid_assign_info->enable_sa_based = SOC_SAND_NUM2BOOL(ihp_pinfo_llr_tbl_data.sa_lookup_enable);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_port_info_get_unsafe()", local_port_ndx, 0);
}

/*********************************************************************
*     Set assignment to VLAN ID according to source MAC
 *     address.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_vid_assign_mac_based_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  uint8
    is_found;
  SOC_PB_PP_LLP_SA_AUTH_MAC_INFO
    old_sa_auth_info;
  uint32
    old_asd,
    new_asd;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_based_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);


  request.stamp = 0;
  /* convert to LEM key */
  res = soc_pb_pp_sa_based_key_to_lem_key_map(
          unit,
          mac_address_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /* set payload */

  /* get previous payload. ASD , as part of the ASD used for VID-assignment */
  /* if key is found and destination is not relevant then keep exist destination payload.dest*/
  res = soc_pb_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  old_asd = payload.asd;

  /* set defaults */
  payload.age = SOC_PB_PP_SA_AUTH_ENTRY_AGE;
  payload.is_dynamic = SOC_PB_PP_SA_AUTH_ENTRY_IS_DYNAMIC;

 /*
  * if found then leave SA-Auth untouched (except VID which is shared)
  * if not found then set to zeros, i.e. don't used authentication.
  */
  SOC_PB_PP_LLP_SA_AUTH_MAC_INFO_clear(&old_sa_auth_info);

  if (is_found)
  {
    res = soc_pb_pp_lem_access_sa_based_asd_parse(
            unit,
            old_asd,
            &old_sa_auth_info,
            NULL
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

    if (old_sa_auth_info.expect_tag_vid != SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL_VIDS)
    {
      old_sa_auth_info.expect_tag_vid = mac_based_info->vid;
    }
  }
  else
  {
    SOC_PB_PP_LLP_SA_SET_NO_AUTH(&old_sa_auth_info);
  }

  res = soc_pb_pp_lem_access_sa_based_asd_build(
          unit,
          &old_sa_auth_info,
          mac_based_info,
          &new_asd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
   /*
    * set updated bit in old ASD: use for-tagged/untagged and VID
    
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(&new_asd, SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LSB, 2, &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_sand_set_field(&updated_asd, SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_MSB, SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LSB, tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    tmp = 0;
    res = soc_sand_bitstream_get_any_field(&new_asd, SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LSB, SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LEN, &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = soc_sand_set_field(&updated_asd, SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_MSB, SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LSB, tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }
  else
  {
    updated_asd = new_asd;
  }
*/
  payload.asd = new_asd;
  /* add entry */
  request.command = SOC_PB_PP_LEM_ACCESS_CMD_INSERT;
  res = soc_pb_pp_lem_access_entry_add_unsafe(
          unit,
          &request,
          &payload,
          &ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  *success = (ack_status.is_success ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_INTERNAL_ERR);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_mac_based_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_llp_vid_assign_mac_based_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_ADD_VERIFY);

  /* SOC_PB_PP_STRUCT_VERIFY(SOC_SAND_PP_MAC_ADDRESS, mac_address_key, 10, exit); */
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO, mac_based_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_mac_based_add_verify()", 0, 0);
}

/*********************************************************************
*     Remove assignment to VLAN ID according to source MAC
 *     address.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_vid_assign_mac_based_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key
  )
{
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  SOC_PB_PP_LLP_SA_AUTH_MAC_INFO
    old_sa_auth_info;
  SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO
    no_vid_assign_info;
  uint8
    is_found;
  uint32
    old_asd,
    new_asd;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);

  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  request.stamp = 0;
  /* convert to LEM key */
  res = soc_pb_pp_sa_based_key_to_lem_key_map(
          unit,
          mac_address_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* set payload */

  /* get previous payload. ASD , as part of the ASD used for SA-AUTH*/
  res = soc_pb_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* if remove and not found then done */
  if (!is_found)
  {
    goto exit;
  }

  old_asd = payload.asd;

  /* set defaults */
  payload.age = SOC_PB_PP_SA_AUTH_ENTRY_AGE;
  payload.is_dynamic = SOC_PB_PP_SA_AUTH_ENTRY_IS_DYNAMIC;

  /* check if auth field are not in use */
  /* i.e. Permit all ports is set and drop-if-vid-diff is unset, accept untagged is set*/
  SOC_PB_PP_LLP_SA_AUTH_MAC_INFO_clear(&old_sa_auth_info);
  res = soc_pb_pp_lem_access_sa_based_asd_parse(
          unit,
          old_asd,
          &old_sa_auth_info,
          NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (SOC_PB_PP_LLP_SA_NO_AUTH(&old_sa_auth_info)) /* not in use --> remove entry */
  {
    request.command = SOC_PB_PP_LEM_ACCESS_CMD_DELETE;
    res = soc_pb_pp_lem_access_entry_remove_unsafe(
            unit,
            &request,
            &ack_status
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  else /* in use --> update ASD entry to disable VID assignment */
  {
    SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO_clear(&no_vid_assign_info);
    no_vid_assign_info.override_tagged = FALSE;
    no_vid_assign_info.use_for_untagged = FALSE;
    no_vid_assign_info.vid = old_sa_auth_info.expect_tag_vid;
    res = soc_pb_pp_lem_access_sa_based_asd_build(
            unit,
            &old_sa_auth_info,
            &no_vid_assign_info,
            &new_asd
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* add entry */
    request.command = SOC_PB_PP_LEM_ACCESS_CMD_INSERT;
    payload.asd = new_asd;
    res = soc_pb_pp_lem_access_entry_add_unsafe(
            unit,
            &request,
            &payload,
            &ack_status
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_mac_based_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_llp_vid_assign_mac_based_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE_VERIFY);

  /* SOC_PB_PP_STRUCT_VERIFY(SOC_SAND_PP_MAC_ADDRESS, mac_address_key, 10, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_mac_based_remove_verify()", 0, 0);
}

/*********************************************************************
*     Get VLAN ID assignment information according to source
 *     MAC address.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_vid_assign_mac_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_OUT SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;
  uint8
    is_found;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_based_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO_clear(mac_based_info);

  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

  request.stamp = 0;
  /* convert to LEM key */
  res = soc_pb_pp_sa_based_key_to_lem_key_map(
          unit,
          mac_address_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /* get  payload. ASD , as part of the ASD used for VID-assignment */
  res = soc_pb_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* if not found then done */
  if (!is_found)
  {
    *found = FALSE;
    goto exit;
  }

  *found = TRUE;
  /*found, check get values*/
  res = soc_pb_pp_lem_access_sa_based_asd_parse(
          unit,
          payload.asd,
          NULL,
          mac_based_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_mac_based_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_llp_vid_assign_mac_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_VERIFY);

  /* SOC_PB_PP_STRUCT_VERIFY(SOC_SAND_PP_MAC_ADDRESS, mac_address_key, 10, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_mac_based_get_verify()", 0, 0);
}

/*********************************************************************
*     Get VLAN ID assignment and SA authentation information
 *     according to source MAC address.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_vid_assign_mac_based_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_MATCH_RULE           *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key_arr,
    SOC_SAND_OUT SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO             *vid_assign_info_arr,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    num_scanned = 0;
  uint8
    is_found,
    is_valid;
  SOC_PB_PP_LEM_ACCESS_KEY
    lem_key;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(mac_address_key_arr);
  SOC_SAND_CHECK_NULL_INPUT(vid_assign_info_arr);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  *nof_entries = 0;

  SOC_PB_PP_LEM_ACCESS_KEY_clear(&lem_key);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

  /*
   *	Loop on the entry index
   */
  for ( num_scanned = 0;
        (num_scanned < block_range->entries_to_scan) &&
         (*nof_entries < block_range->entries_to_act) &&
         (block_range->iter < SOC_PB_PP_LEM_NOF_ENTRIES);
        ++block_range->iter, ++num_scanned)
  {
    res = soc_pb_pp_lem_access_entry_by_index_get_unsafe(
            unit,
            block_range->iter,
            &lem_key,
            &is_valid
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if ((is_valid == TRUE) && (lem_key.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH))
    {
      res = soc_pb_pp_sa_based_key_from_lem_key_map(
              unit,
              &lem_key,
              &(mac_address_key_arr[*nof_entries])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_pb_pp_llp_vid_assign_mac_based_get_unsafe(
              unit,
               &(mac_address_key_arr[*nof_entries]),
               &(vid_assign_info_arr[*nof_entries]),
               &is_found
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      /* does it match rule ?*/
      if (!is_found)
      {
        continue;
      }
      if ((!(rule->rule_type & SOC_PB_PP_LLP_VID_ASSIGN_MATCH_RULE_TYPE_TAGGED))&& vid_assign_info_arr[*nof_entries].override_tagged)
      {
        continue;
      }
      if ((!(rule->rule_type & SOC_PB_PP_LLP_VID_ASSIGN_MATCH_RULE_TYPE_UNTAGGED))&& vid_assign_info_arr[*nof_entries].use_for_untagged)
      {
        continue;
      }
      ++*nof_entries;
    }
 }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_mac_based_get_block_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_llp_vid_assign_mac_based_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_MATCH_RULE           *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_VID_ASSIGN_MATCH_RULE, rule, 10, exit);
  /* SOC_PB_PP_STRUCT_VERIFY(SOC_SAND_TABLE_BLOCK_RANGE, block_range, 20, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_mac_based_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Set assignment of VLAN ID based on source IPv4 subnet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_vid_assign_ipv4_subnet_based_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  )
{
  SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA
    tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  res = soc_pb_pp_ihp_subnet_classify_tbl_get_unsafe(
          unit,
          entry_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Override subnet only if vid is valid, in order not to ruin the cos entry (if valid) */
  if (subnet_based_info->vid_is_valid)
  {
    tbl_data.ipv4_subnet = subnet->ip_address;
    /* set number of invalid bits*/
    tbl_data.ipv4_subnet_mask = (32 - subnet->prefix_len);
  }
  tbl_data.ipv4_subnet_vid = subnet_based_info->vid;
  tbl_data.ipv4_subnet_valid = SOC_SAND_BOOL2NUM(subnet_based_info->vid_is_valid);

  res = soc_pb_pp_ihp_subnet_classify_tbl_set_unsafe(
          unit,
          entry_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_ipv4_subnet_based_set_unsafe()", entry_ndx, 0);
}

uint32
  soc_pb_pp_llp_vid_assign_ipv4_subnet_based_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_LLP_VID_ASSIGN_ENTRY_NDX_MAX, SOC_PB_PP_LLP_VID_ASSIGN_ENTRY_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO, subnet_based_info, 30, exit);

  res = soc_sand_pp_ipv4_subnet_verify(subnet);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* prefix cannot be zero if entry is valid */
  if (subnet_based_info->vid_is_valid && (subnet->prefix_len == 0))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_VID_ASSIGN_SUBNET_PREF_IS_ZERO_ERR,50,exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_ipv4_subnet_based_set_verify()", entry_ndx, 0);
}

uint32
  soc_pb_pp_llp_vid_assign_ipv4_subnet_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_LLP_VID_ASSIGN_ENTRY_NDX_MAX, SOC_PB_PP_LLP_VID_ASSIGN_ENTRY_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_ipv4_subnet_based_get_verify()", entry_ndx, 0);
}

/*********************************************************************
*     Set assignment of VLAN ID based on source IPv4 subnet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_vid_assign_ipv4_subnet_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_OUT SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  )
{
  SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA
    tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  soc_sand_SAND_PP_IPV4_SUBNET_clear(subnet);
  SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(subnet_based_info);

  res = soc_pb_pp_ihp_subnet_classify_tbl_get_unsafe(
          unit,
          entry_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  subnet->ip_address = tbl_data.ipv4_subnet;
  subnet->prefix_len = (uint8)(32 - tbl_data.ipv4_subnet_mask);
  subnet_based_info->vid = (SOC_SAND_PP_TC)tbl_data.ipv4_subnet_vid;
  subnet_based_info->vid_is_valid = SOC_SAND_NUM2BOOL(tbl_data.ipv4_subnet_valid);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_ipv4_subnet_based_get_unsafe()", entry_ndx, 0);
}

/*********************************************************************
*     Sets mapping from Ethernet Type and profile to VID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_vid_assign_protocol_based_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK,
    tbl_offset,
    ether_type_internal;
  uint8
    update_hw,
    alloc_ether_type,
    found;
  SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA
    ihp_port_protocol_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  tbl_offset = 0;
  *success = SOC_SAND_SUCCESS;
  alloc_ether_type = TRUE;
  update_hw = TRUE;

  res = soc_pb_pp_l2_next_prtcl_type_find(
          unit,
          ether_type_ndx,
          &ether_type_internal,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (found)
  {
    /* Ether type already allocated. Let's get the table entry */
    tbl_offset =
      SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(ether_type_internal, port_profile_ndx);

    res = soc_pb_pp_ihp_port_protocol_tbl_get_unsafe(
            unit,
            tbl_offset,
            &ihp_port_protocol_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (ihp_port_protocol_tbl_data.vid_valid)
    {
      if (prtcl_assign_info->vid_is_valid)
      {
        if (ihp_port_protocol_tbl_data.vid == prtcl_assign_info->vid)
        {
          /* Same data is already configured. Nothing to do. */
          update_hw = FALSE;
          alloc_ether_type = FALSE;
        }
        else /* (ihp_port_protocol_tbl_data.vid != prtcl_assign_info->vid) */
        {
          /* Entry will be later update to new value */
        }
      }
      else /* !prtcl_assign_info->vid_is_valid */
      {
        /* Entry is currently valid, but will be configured to be invalid */
        res = soc_pb_pp_l2_next_prtcl_type_deallocate(
                unit,
                ether_type_ndx
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        alloc_ether_type = FALSE;
      }
    }
  }
  
  if (alloc_ether_type)
  {
    /* Allocate ether type */
    res = soc_pb_pp_l2_next_prtcl_type_allocate(
            unit,
            ether_type_ndx,
            &ether_type_internal,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (*success == SOC_SAND_SUCCESS)
  {
    if (!found)
    {
      tbl_offset =
        SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(ether_type_internal, port_profile_ndx);

      res = soc_pb_pp_ihp_port_protocol_tbl_get_unsafe(
              unit,
              tbl_offset,
              &ihp_port_protocol_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

    if (update_hw)
    {
      ihp_port_protocol_tbl_data.vid = prtcl_assign_info->vid;
      ihp_port_protocol_tbl_data.vid_valid = SOC_SAND_BOOL2NUM(prtcl_assign_info->vid_is_valid);

      res = soc_pb_pp_ihp_port_protocol_tbl_set_unsafe(
              unit,
              tbl_offset,
              &ihp_port_protocol_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_protocol_based_set_unsafe()", port_profile_ndx, ether_type_ndx);
}

uint32
  soc_pb_pp_llp_vid_assign_protocol_based_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_LLP_VID_ASSIGN_PORT_PROFILE_NDX_MAX, SOC_PB_PP_LLP_VID_ASSIGN_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_VID_ASSIGN_PRTCL_INFO, prtcl_assign_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_protocol_based_set_verify()", port_profile_ndx, ether_type_ndx);
}

uint32
  soc_pb_pp_llp_vid_assign_protocol_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_LLP_VID_ASSIGN_PORT_PROFILE_NDX_MAX, SOC_PB_PP_LLP_VID_ASSIGN_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_protocol_based_get_verify()", port_profile_ndx, ether_type_ndx);
}

/*********************************************************************
*     Sets mapping from Ethernet Type and profile to VID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_vid_assign_protocol_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info
  )
{
  uint32
    res = SOC_SAND_OK,
    tbl_offset,
    ether_type_internal;
  uint8
    found;
  SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA
    ihp_port_protocol_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);

  SOC_PB_PP_LLP_VID_ASSIGN_PRTCL_INFO_clear(prtcl_assign_info);

  res = soc_pb_pp_l2_next_prtcl_type_find(
          unit,
          ether_type_ndx,
          &ether_type_internal,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (found)
  {
    tbl_offset =
      SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(ether_type_internal, port_profile_ndx);

    res = soc_pb_pp_ihp_port_protocol_tbl_get_unsafe(
      unit,
      tbl_offset,
      &ihp_port_protocol_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    prtcl_assign_info->vid_is_valid =
      SOC_SAND_NUM2BOOL(ihp_port_protocol_tbl_data.vid_valid);
    prtcl_assign_info->vid = (SOC_SAND_PP_VLAN_ID)ihp_port_protocol_tbl_data.vid;
  }
  else /* if not found, then VID assignment is not valid*/
  {
    prtcl_assign_info->vid = 0;
    prtcl_assign_info->vid_is_valid = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_vid_assign_protocol_based_get_unsafe()", port_profile_ndx, ether_type_ndx);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_llp_vid_assign module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_llp_vid_assign_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_llp_vid_assign;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_llp_vid_assign module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_llp_vid_assign_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_llp_vid_assign;
}
uint32
  SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->pvid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_VID_ASSIGN_PORT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_VID_ASSIGN_PRTCL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_VID_ASSIGN_PRTCL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_VID_ASSIGN_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /*SOC_SAND_ERR_IF_ABOVE_MAX(info->rule_type, SOC_PB_PP_LLP_VID_ASSIGN_RULE_TYPE_MAX, SOC_PB_PP_LLP_VID_ASSIGN_RULE_TYPE_OUT_OF_RANGE_ERR, 10, exit);*/
  if (info->vid != SOC_PB_PP_IGNORE_VAL) {
	  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 11, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_VID_ASSIGN_MATCH_RULE_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

