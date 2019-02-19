/* $Id: pb_pp_llp_filter.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_llp_filter.c
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

#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_filter.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_parse.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LLP_FILTER_PORT_PROFILE_NDX_MAX                  (7)
#define SOC_PB_PP_LLP_FILTER_VLAN_FORMAT_NDX_MAX                   (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)

/* min entry in SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA which is not NULL */
#define SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_MIN_NOT_NULL (1)
/* Entry reserved for NULL entry */
#define SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL         (0)

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
  Soc_pb_pp_procedure_desc_element_llp_filter[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_FILTER_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_llp_filter[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LLP_FILTER_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_FILTER_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_format_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_FILTER_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_FILTER_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_FILTER_ACCEPT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_FILTER_ACCEPT_OUT_OF_RANGE_ERR",
    "The parameter 'accept' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_PORTS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_PORTS_OUT_OF_RANGE_ERR",
    "The parameter 'ports' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_UNEXPECTED_TAG_FORMAT_VLAN_FORMAT_ERR,
    "SOC_PB_PP_LLP_UNEXPECTED_TAG_FORMAT_VLAN_FORMAT_ERR",
    "The parameter 'vlan_format_ndx' was not found in llvp table. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FILTER_VID_NOT_DESIGNATED_ERR,
    "SOC_PB_PP_FILTER_VID_NOT_DESIGNATED_ERR",
    "The parameter 'vid' should be configured as designated when \n\r "
    "parameter 'accept' is false",
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
  soc_pb_pp_llp_filter_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = soc_pb_pp_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  SOC_PB_PP_FLD_SET(regs->egq.ehpgeneral_settings_reg.nwk_offset_add, 0x0, 10, exit);
  SOC_PB_PP_FLD_SET(regs->epni.general_pp_config_reg.nwk_offset_add, 0x0, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets ingress VLAN membership; which incoming local ports
 *     belong to the VLAN. Packets received on a port that is
 *     not a member of the VLAN the packet is classified to be
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                      ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_UNSAFE);

  sal_memcpy(&tbl_data.vlan_port_member_line, ports, sizeof(tbl_data.vlan_port_member_line));
  res = soc_pb_pp_ihp_vsi_port_membership_tbl_set_unsafe(
          unit,
          vid_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_set_unsafe()", vid_ndx, 0);
}

uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    bit_indx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  for (bit_indx = SOC_PB_PP_MAX_NOF_LOCAL_PORTS;
       bit_indx < SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE * SOC_SAND_NOF_BITS_IN_UINT32;
       ++bit_indx)
  {
    if (soc_sand_bitstream_test_bit(ports,bit_indx))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_PORTS_OUT_OF_RANGE_ERR, 20, exit);
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_set_verify()", vid_ndx, bit_indx);
}

uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_get_verify()", vid_ndx, 0);
}

/*********************************************************************
*     Sets ingress VLAN membership; which incoming local ports
 *     belong to the VLAN. Packets received on a port that is
 *     not a member of the VLAN the packet is classified to be
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT SOC_PB_PP_PORT                                ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_UNSAFE);

  res = soc_pb_pp_ihp_vsi_port_membership_tbl_get_unsafe(
    unit,
    vid_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  sal_memset(ports, 0, SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE * sizeof(SOC_PB_PP_PORT));
  sal_memcpy(ports, &tbl_data.vlan_port_member_line, sizeof(tbl_data.vlan_port_member_line));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_get_unsafe()", vid_ndx, 0);
}

/*********************************************************************
*     Add a local port as a member in a VLAN.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_port_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_UNSAFE);

  res = soc_pb_pp_ihp_vsi_port_membership_tbl_get_unsafe(
    unit,
    vid_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_sand_bitstream_set_bit((uint32*)&tbl_data.vlan_port_member_line, local_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = soc_pb_pp_ihp_vsi_port_membership_tbl_set_unsafe(
    unit,
    vid_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_port_add_unsafe()", vid_ndx, 0);
}

uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_port_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_port, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_port_add_verify()", vid_ndx, 0);
}

/*********************************************************************
*     Remove a local port from the VLAN membership.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_UNSAFE);

  res = soc_pb_pp_ihp_vsi_port_membership_tbl_get_unsafe(
    unit,
    vid_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_sand_bitstream_reset_bit((uint32*)&tbl_data.vlan_port_member_line, local_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = soc_pb_pp_ihp_vsi_port_membership_tbl_set_unsafe(
    unit,
    vid_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove_unsafe()", vid_ndx, 0);
}

uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_port, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove_verify()", vid_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type on incoming port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_acceptable_frames_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT            vlan_format_ndx,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                          *action_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_profile);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_acceptable_frames_set_unsafe()", port_profile_ndx, 0);
}

uint32
  soc_pb_pp_llp_filter_ingress_acceptable_frames_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT            vlan_format_ndx,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                          *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_LLP_FILTER_PORT_PROFILE_NDX_MAX, SOC_PB_PP_LLP_FILTER_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vlan_format_ndx, SOC_PB_PP_LLP_FILTER_VLAN_FORMAT_NDX_MAX, SOC_PB_PP_LLP_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_ACTION_PROFILE, action_profile, 30, exit);
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_acceptable_frames_set_verify()", port_profile_ndx, 0);
}

uint32
  soc_pb_pp_llp_filter_ingress_acceptable_frames_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT            vlan_format_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_LLP_FILTER_PORT_PROFILE_NDX_MAX, SOC_PB_PP_LLP_FILTER_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vlan_format_ndx, SOC_PB_PP_LLP_FILTER_VLAN_FORMAT_NDX_MAX, SOC_PB_PP_LLP_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_acceptable_frames_get_verify()", port_profile_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type on incoming port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_acceptable_frames_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT            vlan_format_ndx,
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE                          *action_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_acceptable_frames_get_unsafe()", port_profile_ndx, 0);
}

/*********************************************************************
*     Set a designated VLAN for a port. Incoming Trill packet
 *     will be checked if it has this T-VID; otherwise, packet
 *     will be dropped.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_designated_vlan_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid,
    SOC_SAND_IN  uint8                                     accept,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset_i,
    offset_free,
    offset_inc,
    offset_dec;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_in_pp_port_config_tbl_data;
  SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA
    designated_vlan_table_tbl_data;
  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_REF_COUNT
    ref_count;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_sw_db_llp_filter_desig_vlan_table_ref_count_get(
    unit,
          &ref_count
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
    unit,
    local_port_ndx,
    &vtt_in_pp_port_config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = soc_pb_pp_ihp_designated_vlan_table_tbl_get_unsafe(
    unit,
    vtt_in_pp_port_config_tbl_data.designated_vlan_index,
    &designated_vlan_table_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  offset_inc = offset_dec = offset_free = SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL;

  if (accept)
  {
    if (designated_vlan_table_tbl_data.designated_vlan == vid)
    {
      /* Vid is already configured as designated. Nothing to do */
      *success = SOC_SAND_SUCCESS;
      goto exit;
    }

    if (vtt_in_pp_port_config_tbl_data.designated_vlan_index !=
        SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
    {
      /* Designated vlan index currently points to a different value. Let's
         decrease it before searching for a free spot */
      if (--(ref_count.arr[vtt_in_pp_port_config_tbl_data.designated_vlan_index]) == 0)
      {
        designated_vlan_table_tbl_data.designated_vlan = 0;

        res = soc_pb_pp_ihp_designated_vlan_table_tbl_set_unsafe(
          unit,
          vtt_in_pp_port_config_tbl_data.designated_vlan_index,
          &designated_vlan_table_tbl_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
      }
    }

    for (offset_i = SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_MIN_NOT_NULL;
         offset_i < SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_SIZE;
         ++offset_i)
    {
      if (ref_count.arr[offset_i] == 0)
      {
        /* remember first free offset */
        if (offset_free == SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
        {
          offset_free = offset_i;
        }
      }
      else
      {
        /* Entry is not free. Compare value with vid */
        res = soc_pb_pp_ihp_designated_vlan_table_tbl_get_unsafe(
          unit,
          offset_i,
          &designated_vlan_table_tbl_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

        if (designated_vlan_table_tbl_data.designated_vlan == vid)
        {
          /* Vid was found in table. Remember and break */
          offset_inc = offset_i;
          break;
        }
      }
    }

    if (offset_inc == SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
    {
      /* Vid was not found. Add new entry if there is room */

      if (offset_free == SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
      {
        /* Table is full */
        *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
        goto exit;
      }
      else
      {
        /* Add vid to designated vlan table */
        designated_vlan_table_tbl_data.designated_vlan = vid;

        res = soc_pb_pp_ihp_designated_vlan_table_tbl_set_unsafe(
          unit,
          offset_free,
          &designated_vlan_table_tbl_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
        
        offset_inc = offset_free;
      }
    }
  }
  else /* Do not accept */
  {
    if (vtt_in_pp_port_config_tbl_data.designated_vlan_index ==
        SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
    {
      /* Nothing to be done. Designated vlan is already NULL */
    }
    else
    {
      /* Verify that vid is currently configured as the designated */
      if (designated_vlan_table_tbl_data.designated_vlan != vid)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FILTER_VID_NOT_DESIGNATED_ERR, 20, exit);
      }

      /* Decrease reference counter, and set to invalid if no references */
      if (--(ref_count.arr[vtt_in_pp_port_config_tbl_data.designated_vlan_index]) == 0)
      {
        designated_vlan_table_tbl_data.designated_vlan = 0;

        res = soc_pb_pp_ihp_designated_vlan_table_tbl_set_unsafe(
          unit,
          vtt_in_pp_port_config_tbl_data.designated_vlan_index,
          &designated_vlan_table_tbl_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
      }

      offset_inc = SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL;
    }
  }
    
  vtt_in_pp_port_config_tbl_data.designated_vlan_index = offset_inc;
  ++(ref_count.arr[offset_inc]);

  res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
    unit,
    local_port_ndx,
    &vtt_in_pp_port_config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_pb_pp_sw_db_llp_filter_desig_vlan_table_ref_count_set(
    unit,
    &ref_count);
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  *success = SOC_SAND_SUCCESS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_designated_vlan_set_unsafe()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_llp_filter_designated_vlan_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid,
    SOC_SAND_IN  uint8                                     accept
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_designated_vlan_set_verify()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_llp_filter_designated_vlan_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_designated_vlan_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Set a designated VLAN for a port. Incoming Trill packet
 *     will be checked if it has this T-VID; otherwise, packet
 *     will be dropped.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_designated_vlan_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_SAND_PP_VLAN_ID                           *vid,
    SOC_SAND_OUT uint8                                     *accept
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_in_pp_port_config_tbl_data;
  SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA
    designated_vlan_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vid);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
    unit,
    local_port_ndx,
    &vtt_in_pp_port_config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (vtt_in_pp_port_config_tbl_data.designated_vlan_index ==
      SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_OFFSET_NULL)
  {
    *vid = 0;
    *accept = FALSE;
  }
  else
  {
    res = soc_pb_pp_ihp_designated_vlan_table_tbl_get_unsafe(
      unit,
      vtt_in_pp_port_config_tbl_data.designated_vlan_index,
      &designated_vlan_table_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

    *vid = designated_vlan_table_tbl_data.designated_vlan;
    *accept = (vtt_in_pp_port_config_tbl_data.designated_vlan_index == 0 ? FALSE : TRUE);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_designated_vlan_get_unsafe()", local_port_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_llp_filter module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_llp_filter_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_llp_filter;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_llp_filter module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_llp_filter_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_llp_filter;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

