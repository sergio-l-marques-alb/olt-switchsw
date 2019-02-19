/* $Id: pb_pp_llp_trap.c,v 1.11 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_llp_trap.c
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

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_trap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_NDX_MAX                       (3)
#define SOC_PB_PP_LLP_TRAP_RESERVED_MC_PROFILE_MAX                 (3)
#define SOC_PB_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_MAX                  (63)
#define SOC_PB_PP_LLP_TRAP_DEST_MAC_NOF_BITS_MAX                   (48)
#define SOC_PB_PP_LLP_TRAP_SUB_TYPE_MAX                            (SOC_SAND_U8_MAX) /*need to uncomment this line below if this value reduced: SOC_SAND_ERR_IF_ABOVE_MAX(info->sub_type, SOC_PB_PP_LLP_TRAP_SUB_TYPE_MAX*/
#define SOC_PB_PP_LLP_TRAP_SUB_TYPE_BITMAP_MAX                     (SOC_SAND_U8_MAX)

#define SOC_PB_PP_LLP_TRAP_PORT_INFO_TRAP_ENABLE_MASK_MAX (0x1ff)
#define SOC_PB_PP_LLP_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_MAX (SOC_PB_PP_TRAP_CODE_RESERVED_MC_7)
#define SOC_PB_PP_LLP_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_MIN (SOC_PB_PP_TRAP_CODE_RESERVED_MC_0)

#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_DA (1)
#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_ETHER_TYPE (2)
#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_SUB_TYPE (4)
#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_IP_PROTOCOL (8)
#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_L4_PORTS (0x10)

#define SOC_PB_PP_LLP_TRAP_PROG_L4_PORTS_FLD_SET(src, dest) \
  ((src<<16) | dest)
#define SOC_PB_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_SRC(fld) \
  SOC_SAND_GET_BITS_RANGE(fld, 31, 16)
#define SOC_PB_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_DEST(fld) \
  SOC_SAND_GET_BITS_RANGE(fld, 15, 0)


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
  Soc_pb_pp_procedure_desc_element_llp_trap[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_ARP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_ARP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_ARP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_ARP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_ARP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_ARP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_ARP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_ARP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_L3_PRTCL_PROCCESS),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_llp_trap[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LLP_TRAP_PROG_TRAP_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_PROG_TRAP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'prog_trap_ndx' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_RESERVED_MC_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_RESERVED_MC_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'reserved_mc_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'trap_enable_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'da_mac_address_lsb' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_DEST_MAC_NOF_BITS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_DEST_MAC_NOF_BITS_OUT_OF_RANGE_ERR",
    "The parameter 'dest_mac_nof_bits' is out of range. \n\r "
    "The range is: 0 - 48.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_SUB_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_SUB_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'sub_type' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_SUB_TYPE_BITMAP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_SUB_TYPE_BITMAP_OUT_OF_RANGE_ERR",
    "The parameter 'sub_type_bitmap' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_IP_PROTOCOL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_IP_PROTOCOL_OUT_OF_RANGE_ERR",
    "The parameter 'ip_protocol' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_SRC_PORT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_SRC_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'src_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_SRC_PORT_BITMAP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_SRC_PORT_BITMAP_OUT_OF_RANGE_ERR",
    "The parameter 'src_port_bitmap' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_DEST_PORT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_DEST_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'dest_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_DEST_PORT_BITMAP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_DEST_PORT_BITMAP_OUT_OF_RANGE_ERR",
    "The parameter 'dest_port_bitmap' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_ENABLE_BITMAP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_ENABLE_BITMAP_OUT_OF_RANGE_ERR",
    "The parameter 'enable_bitmap' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_INVERSE_BITMAP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_INVERSE_BITMAP_OUT_OF_RANGE_ERR",
    "The parameter 'inverse_bitmap' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_ACTION_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_ACTION_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_action_strength' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_ACTION_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_ACTION_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'snoop_action_strength' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_L3_PROTOCOL_EXCEEDS_CAPACITY_ERR,
    "SOC_PB_PP_LLP_TRAP_L3_PROTOCOL_EXCEEDS_CAPACITY_ERR",
    "Only 7 user defined l3 protocol are supported. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_TO_NDX_MISMATCH_ERR,
    "SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_TO_NDX_MISMATCH_ERR",
    "'prog_trap_ndx' larger than 1 may only be used to. \n\r "
    "test DA and ETHER_TYPE.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'trap_enable_mask' is out of range. \n\r "
    "The range is: 0 - 255 or 0xffffffff (see SOC_PB_PP_LLP_TRAP_PORT_ENABLE).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_TRAP_FAILED_TO_ALLOCATE_ETHER_TYPE_ERR,
    "SOC_PB_PP_LLP_TRAP_FAILED_TO_ALLOCATE_ETHER_TYPE_ERR",
    "Failed while trying to allocate ether_type.\n\r",
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
  soc_pb_pp_llp_trap_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets port information for Link Layer Traps, including
 *     which reserved Multicast profile and which Traps are
 *     enabled...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_port_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_INFO                      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    ihp_pinfo_llr_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
          unit,
          local_port_ndx,
          &ihp_pinfo_llr_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ihp_pinfo_llr_tbl_data.reserved_mc_trap_profile =
    port_info->reserved_mc_profile;
  
  ihp_pinfo_llr_tbl_data.enable_arp_trap =
    SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PB_PP_LLP_TRAP_PORT_ENABLE_ARP);

  ihp_pinfo_llr_tbl_data.enable_dhcp_trap =
    SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PB_PP_LLP_TRAP_PORT_ENABLE_DHCP);

  ihp_pinfo_llr_tbl_data.enable_igmp_trap =
    SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PB_PP_LLP_TRAP_PORT_ENABLE_IGMP);

  ihp_pinfo_llr_tbl_data.enable_mld_trap =
    SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PB_PP_LLP_TRAP_PORT_ENABLE_MLD);

  /* Enable bit for each prog trap, if enabled */
  ihp_pinfo_llr_tbl_data.general_trap_enable = (0xf &
    ((SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0)) |
     (SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_1) << 1) |
     (SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_2) << 2) |
     (SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_3) << 3)));

  res = soc_pb_pp_ihp_pinfo_llr_tbl_set_unsafe(
    unit,
    local_port_ndx,
    &ihp_pinfo_llr_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_port_info_set_unsafe()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_llp_trap_port_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_INFO                      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_TRAP_PORT_INFO, port_info, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_llp_trap_port_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_port_info_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets port information for Link Layer Traps, including
 *     which reserved Multicast profile and which Traps are
 *     enabled...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_port_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PORT_INFO                      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    ihp_pinfo_llr_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PB_PP_LLP_TRAP_PORT_INFO_clear(port_info);

  res = soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
    unit,
    local_port_ndx,
    &ihp_pinfo_llr_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  port_info->reserved_mc_profile =
    ihp_pinfo_llr_tbl_data.reserved_mc_trap_profile;

  port_info->trap_enable_mask = 0;

  if (ihp_pinfo_llr_tbl_data.enable_arp_trap)
  {
    port_info->trap_enable_mask |= SOC_PB_PP_LLP_TRAP_PORT_ENABLE_ARP;
  }
  if (ihp_pinfo_llr_tbl_data.enable_dhcp_trap)
  {
    port_info->trap_enable_mask |= SOC_PB_PP_LLP_TRAP_PORT_ENABLE_DHCP;
  }
  if (ihp_pinfo_llr_tbl_data.enable_igmp_trap)
  {
    port_info->trap_enable_mask |= SOC_PB_PP_LLP_TRAP_PORT_ENABLE_IGMP;
  }
  if (ihp_pinfo_llr_tbl_data.enable_mld_trap)
  {
    port_info->trap_enable_mask |= SOC_PB_PP_LLP_TRAP_PORT_ENABLE_MLD;
  }
  
  /* Enable bit for each prog trap, if enabled */
  if (SOC_SAND_GET_BIT(ihp_pinfo_llr_tbl_data.general_trap_enable,0))
  {
    port_info->trap_enable_mask |= SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0;
  }
  if (SOC_SAND_GET_BIT(ihp_pinfo_llr_tbl_data.general_trap_enable,1))
  {
    port_info->trap_enable_mask |= SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_1;
  }
  if (SOC_SAND_GET_BIT(ihp_pinfo_llr_tbl_data.general_trap_enable,2))
  {
    port_info->trap_enable_mask |= SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_2;
  }
  if (SOC_SAND_GET_BIT(ihp_pinfo_llr_tbl_data.general_trap_enable,3))
  {
    port_info->trap_enable_mask |= SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_3;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_port_info_get_unsafe()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets information for ARP trapping, including My-IP
 *     addresses (used to Trap ARP Requests)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_arp_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_ARP_INFO                       *arp_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_ARP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(arp_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_SET(regs->ihp.my_arp_ip1_reg.my_arp_ip1, arp_info->my_ips[0], 10, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.my_arp_ip2_reg.my_arp_ip2, arp_info->my_ips[1], 20, exit);
  
  SOC_PB_PP_FLD_SET(regs->ihp.mac_layer_trap_arp_reg.arp_trap_ignore_da,
    SOC_SAND_BOOL2NUM(arp_info->ignore_da),
    30,
    exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_arp_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_llp_trap_arp_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_ARP_INFO                       *arp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_ARP_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_TRAP_ARP_INFO, arp_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_arp_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_llp_trap_arp_info_get_verify(
    SOC_SAND_IN  int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_ARP_INFO_GET_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_arp_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets information for ARP trapping, including My-IP
 *     addresses (used to Trap ARP Requests)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_arp_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_ARP_INFO                       *arp_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_ARP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(arp_info);

  SOC_PB_PP_LLP_TRAP_ARP_INFO_clear(arp_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->ihp.my_arp_ip1_reg.my_arp_ip1, fld_val, 10, exit);
  arp_info->my_ips[0] = fld_val;

  SOC_PB_PP_FLD_GET(regs->ihp.my_arp_ip2_reg.my_arp_ip2, fld_val, 20, exit);
  arp_info->my_ips[1] = fld_val;

  SOC_PB_PP_FLD_GET(regs->ihp.mac_layer_trap_arp_reg.arp_trap_ignore_da,
    fld_val,
    30,
    exit);
  arp_info->ignore_da = SOC_SAND_BOOL2NUM(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_arp_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets Trap information for IEEE reserved multicast
 *     (Ethernet Header. DA matches 01-80-c2-00-00-XX where XX =
 *     8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_reserved_mc_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY                *reserved_mc_key,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                          *trap_action
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  SOC_PB_PP_IHP_RESERVED_MC_TBL_DATA
    ihp_reserved_mc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(reserved_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(trap_action);

  entry_offset = SOC_PB_PP_TBL_IHP_RESERVED_MC_KEY_ENTRY_OFFSET(
    reserved_mc_key->reserved_mc_profile,
    reserved_mc_key->da_mac_address_lsb);

  ihp_reserved_mc_tbl_data.cpu_trap_code = (trap_action->trap_code - SOC_PB_PP_TRAP_CODE_RESERVED_MC_0);
  ihp_reserved_mc_tbl_data.fwd = trap_action->frwrd_action_strength;
  ihp_reserved_mc_tbl_data.snp = trap_action->snoop_action_strength;

  res = soc_pb_pp_ihp_reserved_mc_tbl_set_unsafe(
    unit,
    entry_offset,
    &ihp_reserved_mc_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_reserved_mc_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_llp_trap_reserved_mc_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY                *reserved_mc_key,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                          *trap_action
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY, reserved_mc_key, 10, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_action->frwrd_action_strength, SOC_PB_PP_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_MAX, SOC_PB_PP_ACTION_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_action->snoop_action_strength, SOC_PB_PP_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_MAX, SOC_PB_PP_ACTION_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(trap_action->trap_code, SOC_PB_PP_LLP_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_MIN, SOC_PB_PP_LLP_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_MAX, SOC_PB_PP_LLP_TRAP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_reserved_mc_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_llp_trap_reserved_mc_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY                *reserved_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY, reserved_mc_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_reserved_mc_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets Trap information for IEEE reserved multicast
 *     (Ethernet Header. DA matches 01-80-c2-00-00-XX where XX =
 *     8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_reserved_mc_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY                *reserved_mc_key,
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE                          *trap_action
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  SOC_PB_PP_IHP_RESERVED_MC_TBL_DATA
    ihp_reserved_mc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_RESERVED_MC_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(reserved_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(trap_action);

  SOC_PB_PP_ACTION_PROFILE_clear(trap_action);

  entry_offset = SOC_PB_PP_TBL_IHP_RESERVED_MC_KEY_ENTRY_OFFSET(
    reserved_mc_key->reserved_mc_profile,
    reserved_mc_key->da_mac_address_lsb);

  res = soc_pb_pp_ihp_reserved_mc_tbl_get_unsafe(
    unit,
    entry_offset,
    &ihp_reserved_mc_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  trap_action->trap_code = ihp_reserved_mc_tbl_data.cpu_trap_code + SOC_PB_PP_TRAP_CODE_RESERVED_MC_0;
  trap_action->frwrd_action_strength = ihp_reserved_mc_tbl_data.fwd;
  trap_action->snoop_action_strength = ihp_reserved_mc_tbl_data.snp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_reserved_mc_info_get_unsafe()", 0, 0);
}


STATIC uint32
  soc_pb_pp_llp_trap_prog_trap_info_l3_prtcl_proccess(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER            *prog_trap_qual,
    SOC_SAND_OUT uint8                                       *prctl_ndx
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint8
    ip_protocol_ndx,
    ip_protocol_ndx_in_db;
  uint8
    is_found,
    is_current_pre_defined;
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PB_PP_L3_NEXT_PRTCL_TYPE
    current_l3_protocol;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_L3_PRTCL_PROCCESS);

  SOC_SAND_CHECK_NULL_INPUT(prog_trap_qual);

  regs = soc_pb_pp_regs();

  /* Check that l3 protocol is a pre-defined one, or user-defined but previously
  allocated, or that there's space for one more */

  res = soc_pb_pp_l3_prtcl_to_ndx(
          prog_trap_qual->l3_info.ip_protocol,
          &ip_protocol_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_pp_l3_next_protocol_find(
          unit,
          prog_trap_qual->l3_info.ip_protocol,
          &ip_protocol_ndx_in_db,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /* Check protocol currently configured for this trap index */
  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_ip_protocol,
    fld_val,
    10,
    exit);

  res = soc_pb_pp_llp_trap_ndx_to_l3_prtcl(
          (uint8)fld_val,
          &current_l3_protocol,
          &is_current_pre_defined
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!is_current_pre_defined)
  {
    /* protocol is user defined */
    if (ip_protocol_ndx_in_db != fld_val)
    {
      res = soc_pb_pp_l3_next_protocol_remove(
              unit,
              (uint8) fld_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      is_found = FALSE;
    }
  }

  if (ip_protocol_ndx == SOC_PB_PP_L3_NEXT_PRTCL_NDX_USER_DEFINED)
  {
    if (is_found == FALSE)
    {
      /* Add new user-defined protocol */
      res = soc_pb_pp_l3_next_protocol_add(
              unit,
              prog_trap_qual->l3_info.ip_protocol,
              &ip_protocol_ndx,
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      if (success != SOC_SAND_SUCCESS)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_TRAP_L3_PROTOCOL_EXCEEDS_CAPACITY_ERR, 60, exit);
      }
    }
    else
    {
      ip_protocol_ndx = ip_protocol_ndx_in_db;
    }
  }

  *prctl_ndx = ip_protocol_ndx;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_prog_trap_info_l3_prtcl_proccess()", 0, 0);
}

/*********************************************************************
*     Sets a programmable trap, a trap that may be set to
 *     packets according to L2/L3/L4 attributes.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_prog_trap_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER            *prog_trap_qual
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    split_fld_val[2],
    ether_type_internal,
    ether_type_internal_old,
    ether_type_old;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint8
    ip_protocol_ndx=0;
  uint8
    found;
  SOC_SAND_SUCCESS_FAILURE
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prog_trap_qual);

  regs = soc_pb_pp_regs();

  /* ether type */
  SOC_PB_PP_FLD_GET(
    regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_ethernet_type,
    ether_type_internal_old,
    4,
    exit);

  res = soc_pb_pp_l2_next_prtcl_type_from_internal_find(
          unit,
          ether_type_internal_old,
          &ether_type_old,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

  if (found)
  {
    res = soc_pb_pp_l2_next_prtcl_type_deallocate(
            unit,
            ether_type_old
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);
  }

  res = soc_pb_pp_l2_next_prtcl_type_allocate(
          unit,
          prog_trap_qual->l2_info.ether_type,
          &ether_type_internal,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

  if (success == SOC_SAND_SUCCESS)
  {
    SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_ethernet_type,
      ether_type_internal,
      10,
      exit);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_TRAP_FAILED_TO_ALLOCATE_ETHER_TYPE_ERR, 12, exit);
  }
 

  /* IP protocol */
  res = soc_pb_pp_llp_trap_prog_trap_info_l3_prtcl_proccess(
    unit,
    prog_trap_ndx,
    prog_trap_qual,
    &ip_protocol_ndx
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_ip_protocol,
    ip_protocol_ndx,
    20,
    exit);

  /* Select DA */
  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
    &(prog_trap_qual->l2_info.dest_mac),
    mac_add_long
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_0[prog_trap_ndx].general_trap_da, mac_add_long[0], 35, exit);

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_da, mac_add_long[1], 40, exit);

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_da_bits,
    SOC_SAND_PP_MAC_ADDRESS_NOF_BITS - prog_trap_qual->l2_info.dest_mac_nof_bits,
    50,
    exit);

  /* sub type */
  fld_val = prog_trap_qual->l2_info.sub_type;

  SOC_PETRA_FIELD_VAL_TO_SPLIT_FLDS_SET(
    &fld_val,
    &(regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_sub_type),
    &(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_sub_type),
    &(split_fld_val[0]),
    &(split_fld_val[1])
    );

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_sub_type,
    split_fld_val[0],
    70,
    exit);

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_sub_type,
    split_fld_val[1],
    80,
    exit);

  fld_val = prog_trap_qual->l2_info.sub_type_bitmap;

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_sub_type_mask,
    fld_val,
    85,
    exit);

  /* L4 ports */
  fld_val =
    SOC_PB_PP_LLP_TRAP_PROG_L4_PORTS_FLD_SET(
      prog_trap_qual->l4_info.src_port,
      prog_trap_qual->l4_info.dest_port);

  SOC_PETRA_FIELD_VAL_TO_SPLIT_FLDS_SET(
    &fld_val,
    &(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_l4_port),
    &(regs->ihp.general_trap_reg_3[prog_trap_ndx].general_trap_l4_port),
    &(split_fld_val[0]),
    &(split_fld_val[1])
    );

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_l4_port,
    split_fld_val[0],
    90,
    exit);

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_3[prog_trap_ndx].general_trap_l4_port,
    split_fld_val[1],
    100,
    exit);

  fld_val =
    SOC_PB_PP_LLP_TRAP_PROG_L4_PORTS_FLD_SET(
    prog_trap_qual->l4_info.src_port_bitmap,
    prog_trap_qual->l4_info.dest_port_bitmap);

  SOC_PETRA_FIELD_VAL_TO_SPLIT_FLDS_SET(
    &fld_val,
    &(regs->ihp.general_trap_reg_3[prog_trap_ndx].general_trap_l4_port_mask),
    &(regs->ihp.general_trap_reg_4[prog_trap_ndx].general_trap_l4_port_mask),
    &(split_fld_val[0]),
    &(split_fld_val[1])
    );

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_3[prog_trap_ndx].general_trap_l4_port_mask,
    split_fld_val[0],
    120,
    exit);

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_4[prog_trap_ndx].general_trap_l4_port_mask,
    split_fld_val[1],
    130,
    exit);

  /* Enable bitmap */
  fld_val = 0;
  if (prog_trap_qual->enable_bitmap & SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_DA)
  {
    fld_val |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_DA;
  }
  if (prog_trap_qual->enable_bitmap & SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE)
  {
    fld_val |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_ETHER_TYPE;
  }
  if (prog_trap_qual->enable_bitmap & SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE)
  {
    fld_val |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_SUB_TYPE;
  }
  if (prog_trap_qual->enable_bitmap & SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL)
  {
    fld_val |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_IP_PROTOCOL;
  }
  if (prog_trap_qual->enable_bitmap & SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS)
  {
    fld_val |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_L4_PORTS;
  }

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_4[prog_trap_ndx].general_trap_enable_bmp,
    fld_val,
    140,
    exit);

  /* inverse bitmap */
  fld_val = 0;
  if (prog_trap_qual->inverse_bitmap & SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_DA)
  {
    fld_val |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_DA;
  }
  if (prog_trap_qual->inverse_bitmap & SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE)
  {
    fld_val |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_ETHER_TYPE;
  }
  if (prog_trap_qual->inverse_bitmap & SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE)
  {
    fld_val |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_SUB_TYPE;
  }
  if (prog_trap_qual->inverse_bitmap & SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL)
  {
    fld_val |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_IP_PROTOCOL;
  }
  if (prog_trap_qual->inverse_bitmap & SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS)
  {
    fld_val |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_L4_PORTS;
  }

  SOC_PB_PP_FLD_SET(regs->ihp.general_trap_reg_4[prog_trap_ndx].general_trap_inverse_bmp,
    fld_val,
    150,
    exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_prog_trap_info_set_unsafe()", prog_trap_ndx, 0);
}

uint32
  soc_pb_pp_llp_trap_prog_trap_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER            *prog_trap_qual
  )
{
  uint32
    res = SOC_SAND_OK,
    enable_bitmap_test,
    inverse_bitmap_test;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(prog_trap_ndx, SOC_PB_PP_LLP_TRAP_PROG_TRAP_NDX_MAX, SOC_PB_PP_LLP_TRAP_PROG_TRAP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER, prog_trap_qual, 20, exit);

  if (prog_trap_ndx > 1)
  {
    /* prog_trap_ndx 0,1 are valid for all tests, Other indices are only valid
       for DA and ETHER_TYPE */
    enable_bitmap_test =
      (prog_trap_qual->enable_bitmap & (~(SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_DA | SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE)));
    inverse_bitmap_test =
      (prog_trap_qual->inverse_bitmap & (~(SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_DA | SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE)));

    if (enable_bitmap_test || inverse_bitmap_test)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_TO_NDX_MISMATCH_ERR, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_prog_trap_info_set_verify()", prog_trap_ndx, 0);
}

uint32
  soc_pb_pp_llp_trap_prog_trap_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(prog_trap_ndx, SOC_PB_PP_LLP_TRAP_PROG_TRAP_NDX_MAX, SOC_PB_PP_LLP_TRAP_PROG_TRAP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_prog_trap_info_get_verify()", prog_trap_ndx, 0);
}

/*********************************************************************
*     Sets a programmable trap, a trap that may be set to
 *     packets according to L2/L3/L4 attributes.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_prog_trap_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER            *prog_trap_qual
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    split_fld_val[2],
    ether_type_internal,
    ether_type;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint8
    is_current_pre_defined,
    found;
  SOC_PB_PP_L3_NEXT_PRTCL_TYPE
    current_l3_protocol;
  SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA
    ihp_parser_ip_protocols_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_TRAP_PROG_TRAP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prog_trap_qual);

  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(prog_trap_qual);

  regs = soc_pb_pp_regs();

  /* IP protocol */
  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_ip_protocol,
    fld_val,
    10,
    exit);

  res = soc_pb_pp_llp_trap_ndx_to_l3_prtcl(
    (uint8)fld_val,
    &current_l3_protocol,
    &is_current_pre_defined
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (is_current_pre_defined)
  {
    prog_trap_qual->l3_info.ip_protocol = current_l3_protocol;
  }
  else
  {
    res = soc_pb_pp_ihp_parser_ip_protocols_tbl_get_unsafe(
      unit,
      SOC_PB_PP_IP_FIELD_VAL_TO_PROTOCOL_UD_NDX(fld_val),
      &ihp_parser_ip_protocols_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    prog_trap_qual->l3_info.ip_protocol =
      (uint8)ihp_parser_ip_protocols_tbl_data.ip_protocol;
  }

  /* Select DA */
  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_0[prog_trap_ndx].general_trap_da, mac_add_long[0], 40, exit);

  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_da, mac_add_long[1], 50, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
    mac_add_long,
    &(prog_trap_qual->l2_info.dest_mac)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_da_bits,
    fld_val,
    70,
    exit);

  prog_trap_qual->l2_info.dest_mac_nof_bits = (uint8)(SOC_SAND_PP_MAC_ADDRESS_NOF_BITS - fld_val);

  SOC_PB_PP_FLD_GET(
    regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_ethernet_type,
    ether_type_internal,
    4,
    exit);

  res = soc_pb_pp_l2_next_prtcl_type_from_internal_find(
          unit,
          ether_type_internal,
          &ether_type,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

  if (found)
  {
    prog_trap_qual->l2_info.ether_type = (uint16)ether_type;
  }

  /* sub type */
  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_sub_type,
    split_fld_val[0],
    80,
    exit);

  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_sub_type,
    split_fld_val[1],
    90,
    exit);

  SOC_PETRA_FIELD_VAL_TO_SPLIT_FLDS_GET(
    &(split_fld_val[0]),
    &(split_fld_val[1]),
    &(regs->ihp.general_trap_reg_1[prog_trap_ndx].general_trap_sub_type),
    &(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_sub_type),
    &fld_val
    );

  prog_trap_qual->l2_info.sub_type = (uint8)fld_val ;

  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_sub_type_mask,
    fld_val,
    95,
    exit);

  prog_trap_qual->l2_info.sub_type_bitmap = (uint8)fld_val;

  /* L4 ports  - src and dest */
  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_l4_port,
    split_fld_val[0],
    95,
    exit);

  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_3[prog_trap_ndx].general_trap_l4_port,
    split_fld_val[1],
    100,
    exit);

  SOC_PETRA_FIELD_VAL_TO_SPLIT_FLDS_GET(
    &(split_fld_val[0]),
    &(split_fld_val[1]),
    &(regs->ihp.general_trap_reg_2[prog_trap_ndx].general_trap_l4_port),
    &(regs->ihp.general_trap_reg_3[prog_trap_ndx].general_trap_l4_port),
    &fld_val
    );

  prog_trap_qual->l4_info.src_port = (uint8)(SOC_PB_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_SRC(fld_val));
  prog_trap_qual->l4_info.dest_port = (uint8)(SOC_PB_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_DEST(fld_val));

  /* L4 ports  - src and dest bitmaps */

  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_3[prog_trap_ndx].general_trap_l4_port_mask,
    split_fld_val[0],
    120,
    exit);

  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_4[prog_trap_ndx].general_trap_l4_port_mask,
    split_fld_val[1],
    130,
    exit);

  SOC_PETRA_FIELD_VAL_TO_SPLIT_FLDS_GET(
    &(split_fld_val[0]),
    &(split_fld_val[1]),
    &(regs->ihp.general_trap_reg_3[prog_trap_ndx].general_trap_l4_port_mask),
    &(regs->ihp.general_trap_reg_4[prog_trap_ndx].general_trap_l4_port_mask),
    &fld_val
    );

  prog_trap_qual->l4_info.src_port_bitmap = (uint16)(SOC_PB_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_SRC(fld_val));
  prog_trap_qual->l4_info.dest_port_bitmap = (uint16)(SOC_PB_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_DEST(fld_val));

  /* Enable bitmap */
  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_4[prog_trap_ndx].general_trap_enable_bmp,
    fld_val,
    140,
    exit);

  prog_trap_qual->enable_bitmap = 0;
  if (fld_val & SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_DA)
  {
    prog_trap_qual->enable_bitmap |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_DA;
  }
  if (fld_val & SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_ETHER_TYPE)
  {
    prog_trap_qual->enable_bitmap |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE;
  }
  if (fld_val & SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_SUB_TYPE)
  {
    prog_trap_qual->enable_bitmap |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE;
  }
  if (fld_val & SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_IP_PROTOCOL)
  {
    prog_trap_qual->enable_bitmap |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL;
  }
  if (fld_val & SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_L4_PORTS)
  {
    prog_trap_qual->enable_bitmap |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS;
  }

  /* inverse bitmap */
  SOC_PB_PP_FLD_GET(regs->ihp.general_trap_reg_4[prog_trap_ndx].general_trap_inverse_bmp,
    fld_val,
    140,
    exit);

  prog_trap_qual->inverse_bitmap = 0;
  if (fld_val & SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_DA)
  {
    prog_trap_qual->inverse_bitmap |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_DA;
  }
  if (fld_val & SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_ETHER_TYPE)
  {
    prog_trap_qual->inverse_bitmap |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE;
  }
  if (fld_val & SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_SUB_TYPE)
  {
    prog_trap_qual->inverse_bitmap |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE;
  }
  if (fld_val & SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_IP_PROTOCOL)
  {
    prog_trap_qual->inverse_bitmap |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL;
  }
  if (fld_val & SOC_PB_PP_LLP_TRAP_PROG_TRAP_BITMAP_L4_PORTS)
  {
    prog_trap_qual->inverse_bitmap |= SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_trap_prog_trap_info_get_unsafe()", prog_trap_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_llp_trap module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_llp_trap_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_llp_trap;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_llp_trap module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_llp_trap_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_llp_trap;
}

uint32
  SOC_PB_PP_LLP_TRAP_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->reserved_mc_profile, SOC_PB_PP_LLP_TRAP_RESERVED_MC_PROFILE_MAX, SOC_PB_PP_LLP_TRAP_RESERVED_MC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  
  if (info->trap_enable_mask != SOC_PB_PP_LLP_TRAP_PORT_ENABLE_ALL)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->trap_enable_mask, SOC_PB_PP_LLP_TRAP_PORT_INFO_TRAP_ENABLE_MASK_MAX, SOC_PB_PP_LLP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_TRAP_PORT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_TRAP_ARP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_ARP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* Nothing to verify for IPs */
  for (ind = 0; ind < SOC_PB_PP_TRAP_NOF_MY_IPS; ++ind)
  {
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_TRAP_ARP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->reserved_mc_profile, SOC_PB_PP_LLP_TRAP_RESERVED_MC_PROFILE_MAX, SOC_PB_PP_LLP_TRAP_RESERVED_MC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_mac_address_lsb, SOC_PB_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_MAX, SOC_PB_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_mac_nof_bits, SOC_PB_PP_LLP_TRAP_DEST_MAC_NOF_BITS_MAX, SOC_PB_PP_LLP_TRAP_DEST_MAC_NOF_BITS_OUT_OF_RANGE_ERR, 11, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->ether_type, SOC_SAND_PP_ETHER_TYPE_MAX, SOC_SAND_PP_ETHER_TYPE_OUT_OF_RANGE_ERR, 12, exit); */
  /*
  warning: comparison is always false due to limited range of data type
  SOC_SAND_ERR_IF_ABOVE_MAX(info->sub_type, SOC_PB_PP_LLP_TRAP_SUB_TYPE_MAX, SOC_PB_PP_LLP_TRAP_SUB_TYPE_OUT_OF_RANGE_ERR, 13, exit);
  */
  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->sub_type_bitmap, SOC_PB_PP_LLP_TRAP_SUB_TYPE_BITMAP_MAX, SOC_PB_PP_LLP_TRAP_SUB_TYPE_BITMAP_OUT_OF_RANGE_ERR, 14, exit); */

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* Nothing to verify */

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* Nothing to verify */

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO, &(info->l2_info), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO, &(info->l3_info), 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO, &(info->l4_info), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

