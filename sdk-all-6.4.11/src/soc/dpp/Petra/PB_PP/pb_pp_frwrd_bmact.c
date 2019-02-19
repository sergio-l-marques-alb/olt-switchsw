/* $Id: pb_pp_frwrd_bmact.c,v 1.16 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_frwrd_bmact.c
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
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_bmact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FRWRD_BMACT_STP_TOPOLOGY_ID_MAX                  (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FRWRD_BMACT_B_FID_PROFILE_MAX                    (SOC_SAND_UINT_MAX)
#define SOC_PB_PP_FRWRD_BMACT_SYS_PORT_ID_MAX                      (SOC_SAND_UINT_MAX)
#define SOC_PB_PP_FRWRD_BMACT_PCP_NOF_BITS                         (3)
#define SOC_PB_PP_FRWRD_BMACT_DEI_NOF_BITS                         (1)
#define SOC_PB_PP_FRWRD_BMACT_PCP_PROFILE_NDX_MAX                  (0)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/*
 * MACRO to check if Mac in mac is enabled.
 * will be called in beginning of each API
 */
#define SOC_PB_PP_MAC_IN_MAC_CHECK_IF_ENABLED(unit)  \
	do{			\
		uint8 __mim_enabled__;	\
		if (soc_pb_pp_is_mac_in_mac_enabled(unit, &__mim_enabled__) != SOC_SAND_OK || !__mim_enabled__)	\
		{  		\
			SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_BMACT_MAC_IN_MAC_CHECK_IF_ENABLED_ERR, 150, exit);   \
		}			\
	}while(0)

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
  Soc_pb_pp_procedure_desc_element_frwrd_bmact[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_GET_ERRS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_INIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_BMACT_INIT_PRINT),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_frwrd_bmact[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_FRWRD_BMACT_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_BMACT_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_BMACT_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_BMACT_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR",
    "The parameter 'stp_topology_id' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_BMACT_B_FID_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_BMACT_B_FID_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'b_fid_profile' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_BMACT_SYS_PORT_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_BMACT_SYS_PORT_ID_OUT_OF_RANGE_ERR",
    "The parameter 'sys_port_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_BMACT_I_SID_DOMAIN_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_BMACT_I_SID_DOMAIN_OUT_OF_RANGE_ERR",
    "The parameter 'i_sid_domain' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
  SOC_PB_PP_FRWRD_BMACT_DA_NOT_FOUND_ACTION_PROFILE_NDX_OUT_OF_RANGE_ERR,
  "SOC_PB_PP_FRWRD_BMACT_DA_NOT_FOUND_ACTION_PROFILE_NDX_OUT_OF_RANGE_ERR",
  "The parameter 'da_not_found_action_profile_ndx' is out of range. \n\r "
  "The range is: 0 - 1.\n\r ",
  SOC_SAND_SVR_ERR,
  FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
  SOC_PB_PP_FRWRD_BMACT_MAC_IN_MAC_CHECK_IF_ENABLED_ERR,
  "SOC_PB_PP_FRWRD_BMACT_MAC_IN_MAC_CHECK_IF_ENABLED_ERR",
  "Mac in mac is not enabled on this device. \n\r ",
  SOC_SAND_SVR_ERR,
  FALSE
  },
  {
  SOC_PB_PP_FRWRD_BMACT_SA_AUTH_ENABLED_ERR,
  "SOC_PB_PP_FRWRD_BMACT_SA_AUTH_ENABLED_ERR",
  "SA authentication is enabled on this device.\n\r "
  "SA authentication and mac in mac cannot be both enabled.\n\r ",
  SOC_SAND_SVR_ERR,
  FALSE
  },
  {
  SOC_PB_PP_FRWRD_BMACT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR,
  "SOC_PB_PP_FRWRD_BMACT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR",
  "The parameter 'pcp_profile_ndx' is out of range. \n\r "
  "The range is: 0.\n\r ",
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

/*********************************************************************
*     Init device to support Mac in mac.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_init_unsafe(
      SOC_SAND_IN  int                                 unit
  )
{
  uint32
	  fld_val,
      res = SOC_SAND_OK;
  SOC_PB_PP_REGS
	  *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_INIT_UNSAFE);

  regs = soc_pb_pp_regs();

  /* 
   * set default setting for forwarding enable 
   */
  SOC_PB_PP_FLD_GET(regs->ihb.flp_general_cfg_reg.enable_forwarding_lookup, fld_val, 20, exit);
  
  /* set bit 7 in the Enable Forwarding Lookup field, to enable forwarding lookup for isid p2p and mp */
  fld_val |= 0x80;
  SOC_PB_PP_FLD_SET(regs->ihb.flp_general_cfg_reg.enable_forwarding_lookup, fld_val, 20, exit);

  /*
  * set SA lookup type:
  */
  fld_val = 0x2;
  SOC_PB_PP_FLD_SET(regs->ihp.link_layer_lookup_cfg_reg.pbp_mact_prefix, fld_val, 5, exit);
  
  /* Set SA lookup type to be 0 for Mac in mac (provider backbone ports) */
  fld_val = 0x0;
  SOC_PB_PP_FLD_SET(regs->eci.sa_lookup_type_reg.sa_lookup_type, fld_val, 20, exit);

   /*
   * set Tpid Profile Mac In Mac to be 3 (default tpid for Mac in Mac)
   */
  fld_val = 0x3;
  SOC_PB_PP_FLD_SET(regs->epni.tpid_profile_mac_in_mac_reg.tpid_profile_mac_in_mac,fld_val,5,exit);

  /* 
   * setting mac in mac to 1 means system-vsi is now used as isid
   */
  fld_val = 0x1;
  SOC_PB_PP_FLD_SET(regs->ihp.vtt_general_configs_reg.mac_in_mac, fld_val, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_init_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_is_mac_in_mac_enabled(
  SOC_SAND_IN  int      unit,
  SOC_SAND_OUT  uint8     *enabled
  )
{
  uint32
	  fld_val,
	  res = SOC_SAND_OK;
  SOC_PB_PP_REGS
	  *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(enabled);

  regs = soc_pb_pp_regs();

  /*
   * get Tpid Profile. if profile =3 then Mac In Mac is enabled
   */
  SOC_PB_PP_FLD_GET(regs->epni.tpid_profile_mac_in_mac_reg.tpid_profile_mac_in_mac,fld_val,5,exit);
  *enabled = (uint8)(fld_val > 0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_is_mac_in_mac_enabled()", 0, 0);
}

/*********************************************************************
*     Set Mac-in_mac TPID profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_mac_in_mac_enable(
    SOC_SAND_IN  int           unit
  )
{
	uint32
	  fld_val,
	  res = SOC_SAND_OK;
  SOC_PB_PP_REGS
	  *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_pb_pp_regs();

  /* Set Tpid Profile for Mac-in-Mac. if profile =3 then Mac In Mac is enabled */
  fld_val = 0x3;
  SOC_PB_PP_FLD_SET(regs->epni.tpid_profile_mac_in_mac_reg.tpid_profile_mac_in_mac,fld_val,5,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_mac_in_mac_enable()", 0, 0);
}

/*********************************************************************
*     Set the B-VID bridging attributes. Backbone Mac
 *     addresses that do not serve as MyMAC for I-components
 *     that are processed according to their B-VID
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_bvid_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           bvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_BMACT_BVID_INFO                     *bvid_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA
    bvd_da_not_found_tbl_data;
  SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_DATA
    bvd_fid_class_tbl_data;
  SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA
    bvid_topology_id_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bvid_info);

  SOC_PB_PP_MAC_IN_MAC_CHECK_IF_ENABLED(unit);

  res = soc_pb_pp_ihp_bvd_da_not_found_tbl_get_unsafe(
    unit,
    bvid_ndx / 4,
    &bvd_da_not_found_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  bvd_da_not_found_tbl_data.profile_index[bvid_ndx % 4] = bvid_info->default_forward_profile;

  res = soc_pb_pp_ihp_bvd_da_not_found_tbl_set_unsafe(
    unit,
    bvid_ndx / 4,
    &bvd_da_not_found_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* set FID class */
  res = soc_pb_pp_ihp_bvd_fid_class_tbl_get_unsafe(
    unit,
    bvid_ndx / 4,
    &bvd_fid_class_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (bvid_info->b_fid_profile == SOC_PB_PP_BFID_EQUAL_TO_BVID) {
	  bvd_fid_class_tbl_data.fid_class[bvid_ndx % 4] = 0; /* independent */ 
  }
  else { /* SOC_PB_PP_BFID_IS_0 */
	  bvd_fid_class_tbl_data.fid_class[bvid_ndx % 4] = 1; /* shared */ 
  }
    
  res = soc_pb_pp_ihp_bvd_fid_class_tbl_set_unsafe(
    unit,
    bvid_ndx / 4,
    &bvd_fid_class_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_ihp_bvd_topology_id_tbl_get_unsafe(
    unit,
    bvid_ndx,
    &bvid_topology_id_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
  bvid_topology_id_tbl_data.bvd_topology_id = bvid_info->stp_topology_id;
  
  res = soc_pb_pp_ihp_bvd_topology_id_tbl_set_unsafe(
    unit,
    bvid_ndx,
    &bvid_topology_id_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_bvid_info_set_unsafe()", bvid_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_bmact_bvid_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           bvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_BMACT_BVID_INFO                     *bvid_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(bvid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_BMACT_BVID_INFO, bvid_info, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_bvid_info_set_verify()", bvid_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_bmact_bvid_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           bvid_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(bvid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_bvid_info_get_verify()", bvid_ndx, 0);
}

/*********************************************************************
*     Set the B-VID bridging attributes. Backbone Mac
 *     addresses that do not serve as MyMAC for I-components
 *     that are processed according to their B-VID
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_bvid_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           bvid_ndx,
    SOC_SAND_OUT SOC_PB_PP_BMACT_BVID_INFO                     *bvid_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA 
  bvid_da_not_found_tbl_data;
  SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_DATA
  bvid_fid_class_tbl_data;
  SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA
  bvid_topology_id_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_BVID_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bvid_info);

  SOC_PB_PP_MAC_IN_MAC_CHECK_IF_ENABLED(unit);

  SOC_PB_PP_BMACT_BVID_INFO_clear(bvid_info);

  res = soc_pb_pp_ihp_bvd_da_not_found_tbl_get_unsafe(
    unit,
    bvid_ndx / 4,
    &bvid_da_not_found_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  bvid_info->default_forward_profile = bvid_da_not_found_tbl_data.profile_index[bvid_ndx % 4];

  res = soc_pb_pp_ihp_bvd_fid_class_tbl_get_unsafe(
    unit,
    bvid_ndx / 4,
    &bvid_fid_class_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  bvid_info->b_fid_profile = bvid_fid_class_tbl_data.fid_class[bvid_ndx % 4];

  res = soc_pb_pp_ihp_bvd_topology_id_tbl_get_unsafe(
    unit,
    bvid_ndx,
    &bvid_topology_id_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  bvid_info->stp_topology_id = bvid_topology_id_tbl_data.bvd_topology_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_bvid_info_get_unsafe()", bvid_ndx, 0);
}

/*********************************************************************
*     Set the BVID range for Traffic Engineered Provider
 *     Backbone Bridging
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pbb_te_bvids);

  SOC_PB_PP_MAC_IN_MAC_CHECK_IF_ENABLED(unit);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_SET(regs->ihp.pbp_te_bvid_range_reg.pbp_te_bvid_range_low, pbb_te_bvids->first_vid, 10, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.pbp_te_bvid_range_reg.pbp_te_bvid_range_high, pbb_te_bvids->last_vid, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_BMACT_PBB_TE_VID_RANGE, pbb_te_bvids, 10, exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
   uint32
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the BVID range for Traffic Engineered Provider
 *     Backbone Bridging
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pbb_te_bvids);

  SOC_PB_PP_MAC_IN_MAC_CHECK_IF_ENABLED(unit);

  SOC_PB_PP_BMACT_PBB_TE_VID_RANGE_clear(pbb_te_bvids);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->ihp.pbp_te_bvid_range_reg.pbp_te_bvid_range_low, pbb_te_bvids->first_vid, 10, exit);
  SOC_PB_PP_FLD_GET(regs->ihp.pbp_te_bvid_range_reg.pbp_te_bvid_range_high, pbb_te_bvids->last_vid, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mim_asd_build(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN SOC_PB_PP_BMACT_ENTRY_INFO       *bmact_entry_info,
    SOC_SAND_OUT uint32                    *asd
  )
{
  uint32
    is_dynamic = 0,
    learn_type = 0,
    tmp,
    dest_encoded,
    asd_encoded,
    asd_lcl[1],
    res = SOC_SAND_OK;
  SOC_PB_PP_FRWRD_DECISION_INFO
    fwd_decision;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *asd_lcl = 0;
  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&fwd_decision);

  res = soc_sand_bitstream_set_any_field(
       &(is_dynamic),
       SOC_PB_PP_LEM_ACCESS_ASD_IS_DYNAMIC,
       SOC_PB_PP_LEM_ACCESS_ASD_IS_DYNAMIC_LEN,
       asd_lcl
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  tmp = bmact_entry_info->drop_sa;
  res = soc_sand_bitstream_set_any_field(
       &(tmp),
       SOC_PB_PP_LEM_ACCESS_ASD_SA_DROP,
       SOC_PB_PP_LEM_ACCESS_ASD_SA_DROP_LEN,
       asd_lcl
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  /* only fec (learn_type = 0) is supported */
  res = soc_sand_bitstream_set_any_field(
       &(learn_type),
       SOC_PB_PP_LEM_ACCESS_ASD_LEARN_TYPE,
       SOC_PB_PP_LEM_ACCESS_ASD_LEARN_TYPE_LEN,
       asd_lcl
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  tmp = bmact_entry_info->i_sid_domain;
  res = soc_sand_bitstream_set_any_field(
       &(tmp),
       SOC_PB_PP_LEM_ACCESS_ASD_I_SID_DOMAIN,
       SOC_PB_PP_LEM_ACCESS_ASD_I_SID_DOMAIN_LEN,
       asd_lcl
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* 
   * get encoded dest for the learn fec
   */
  SOC_PB_PP_FRWRD_DECISION_FEC_SET(unit, &fwd_decision, bmact_entry_info->sa_learn_fec_id);
  
  res = soc_pb_pp_fwd_decision_in_buffer_build( 
        SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT, 
        &fwd_decision, 
        0,
        &dest_encoded,
        &asd_encoded
     );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  res = soc_sand_bitstream_set_any_field(
       &(dest_encoded),
       SOC_PB_PP_LEM_ACCESS_ASD_LEARN_DESTINATION,
       SOC_PB_PP_LEM_ACCESS_ASD_LEARN_DESTINATION_LEN,
       asd_lcl
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *asd = *asd_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mim_asd_build()", 0, 0);
}

STATIC uint32
  soc_pb_pp_frwrd_bmact_key_convert(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY      *bmac_key,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY       *key
   )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(bmac_key);

  key->type = SOC_PB_PP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC;
  key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_BACKBONE_MAC;

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
    &(bmac_key->b_mac_addr),
    key->param[0].value
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  key->param[1].value[0] = bmac_key->b_vid;
  key->param[1].value[1] = 0;
  key->param[0].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC;
  key->param[1].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC;
  key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_BACKBONE_MAC;
  key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_BACKBONE_MAC;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_key_convert()", 0, 0);
}

uint32
	soc_pb_pp_frwrd_bmact_key_parse(
		SOC_SAND_IN   int                  unit,
		SOC_SAND_IN   SOC_PB_PP_LEM_ACCESS_KEY       *key,
		SOC_SAND_OUT  SOC_PB_PP_BMACT_ENTRY_KEY      *bmac_key
	)
{
	uint32
		res = SOC_SAND_OK;

	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	SOC_SAND_CHECK_NULL_INPUT(key);
	SOC_SAND_CHECK_NULL_INPUT(bmac_key);

	SOC_PB_PP_BMACT_ENTRY_KEY_clear(bmac_key);

    /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
	res = soc_sand_pp_mac_address_long_to_struct(
		key->param[0].value,
		&(bmac_key->b_mac_addr)
		);
	SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

	bmac_key->b_vid = key->param[1].value[0];

exit:
	SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_key_parse()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_bmact_payload_convert(
      SOC_SAND_IN  int                             unit,
      SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_PAYLOAD              *payload,
      SOC_SAND_OUT SOC_PB_PP_BMACT_ENTRY_INFO                *bmact_entry_info
    )
{
  uint32
 dest_encoded = 0,
 tmp = 0,
    res;
  SOC_PB_PP_FRWRD_DECISION_INFO
    frwrd_decision_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(bmact_entry_info);

  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&frwrd_decision_info);
  SOC_PB_PP_BMACT_ENTRY_INFO_clear(bmact_entry_info);

  /*
   * Get the sys_port from the encoded destination
   */
  res = soc_pb_pp_fwd_decision_in_buffer_parse(
          SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
          payload->dest,
          payload->asd,
          &frwrd_decision_info, 
          &(bmact_entry_info->drop_sa)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  bmact_entry_info->sys_port_id = frwrd_decision_info.dest_id;

  res = soc_sand_bitstream_get_any_field(
       &(payload->asd),
       SOC_PB_PP_LEM_ACCESS_ASD_I_SID_DOMAIN, 
       SOC_PB_PP_LEM_ACCESS_ASD_I_SID_DOMAIN_LEN,
       &tmp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  bmact_entry_info->i_sid_domain = tmp;

  /*
   * Get the learn_fec from ASD
   */
  res = soc_sand_bitstream_get_any_field(
   &(payload->asd),
   SOC_PB_PP_LEM_ACCESS_ASD_LEARN_DESTINATION, 
   SOC_PB_PP_LEM_ACCESS_ASD_LEARN_DESTINATION_LEN,
   &dest_encoded
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /*bmact_entry_info->sa_learn_fec_id = dest_encoded;*/

  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&frwrd_decision_info);
  res = soc_pb_pp_fwd_decision_in_buffer_parse(
   SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT,
   dest_encoded,
   payload->asd,
   &frwrd_decision_info, 
   &(bmact_entry_info->drop_sa)
   );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  bmact_entry_info->sa_learn_fec_id = frwrd_decision_info.dest_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_payload_convert()", 0, 0);
}

/*********************************************************************
*     Add an entry to the B-MACT DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_INFO                    *bmact_entry_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
  dest_encoded,
  asd_encoded,
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  SOC_PB_PP_FRWRD_DECISION_INFO
    frwrd_decision_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bmac_key);
  SOC_SAND_CHECK_NULL_INPUT(bmact_entry_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PP_MAC_IN_MAC_CHECK_IF_ENABLED(unit);

  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);
  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&frwrd_decision_info);

  /*
   *  Adapt the variables for the generic access to LEM (Large exact match)
   */
  request.command = SOC_PB_PP_LEM_ACCESS_CMD_INSERT; 
  request.stamp = 0;
  res = soc_pb_pp_frwrd_bmact_key_convert(
          unit,
          bmac_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  
  SOC_PB_PP_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, &frwrd_decision_info, bmact_entry_info->sys_port_id);

  /*
   *  Get the encoded destination. ASD will be built separately 
   */
  res = soc_pb_pp_fwd_decision_in_buffer_build( 
          SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_MACT, 
          &frwrd_decision_info, 
          bmact_entry_info->drop_sa,
          &dest_encoded,
          &asd_encoded
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  payload.dest = dest_encoded;
  
  res = soc_pb_pp_mim_asd_build(
     unit,
     bmact_entry_info,
     &asd_encoded
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  payload.asd = asd_encoded;

  /* aging is static, so all fields are 0 */
  payload.age = 0;
  payload.is_dynamic = 0;

  /*
   *  Access to the LEM (i.e., B-MAC Table)
   */
  res = soc_pb_pp_lem_access_entry_add_unsafe(
          unit,
          &request,
          &payload,
          &ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (ack_status.is_success == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    switch(ack_status.reason)
    {
    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_MACT_FULL:
    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_CAM_FULL:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      break;

    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_FID_LIMIT:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
      break;

    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT:
   case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_WRONG_STAMP:
    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_FID_UNKNOWN:
    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_UNKNOWN:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_3;
      break;

   case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_CHANGE_STATIC:
    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_LEARN_STATIC:
      *success = SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST;
      break;

    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN:
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 50, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_entry_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_bmact_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_INFO                    *bmact_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_ENTRY_ADD_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_BMACT_ENTRY_KEY, bmac_key, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_BMACT_ENTRY_INFO, bmact_entry_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_entry_add_verify()", 0, 0);
}

/*********************************************************************
*     Remove an entry from the B-MACT DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_ENTRY_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bmac_key);

  SOC_PB_PP_MAC_IN_MAC_CHECK_IF_ENABLED(unit);

  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  /*
   * Adapt the variables for the generic access to LEM (Large exact match)
   */
  request.command = SOC_PB_PP_LEM_ACCESS_CMD_DELETE;
  request.stamp = 0;

  res = soc_pb_pp_frwrd_bmact_key_convert(
          unit,
          bmac_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   * Access the LEM (i.e., B-MAC Table)
   */
  res = soc_pb_pp_lem_access_entry_remove_unsafe(
          unit,
          &request,
          &ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (!((ack_status.is_success == TRUE) || (ack_status.reason == SOC_PB_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN)))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 50, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_entry_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_bmact_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_ENTRY_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_BMACT_ENTRY_KEY, bmac_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_entry_remove_verify()", 0, 0);
}

/*********************************************************************
*     Get an entry from the B-MACT DB.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key,
 SOC_SAND_OUT SOC_PB_PP_BMACT_ENTRY_INFO                    *bmact_entry_info,
 SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_KEY
   key;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
   payload;
  uint8
   is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bmac_key);
  SOC_SAND_CHECK_NULL_INPUT(bmact_entry_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_MAC_IN_MAC_CHECK_IF_ENABLED(unit);

  SOC_PB_PP_BMACT_ENTRY_INFO_clear(bmact_entry_info);
  SOC_PB_PP_LEM_ACCESS_KEY_clear(&key);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

  /*
   * Adapt the variables for the generic access to LEM (Large exact match)
   */
  res = soc_pb_pp_frwrd_bmact_key_convert(
          unit,
          bmac_key,
          &(key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Access to the B-MAC Table
   */
  res = soc_pb_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &key,
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Convert the payload
   */
  res = soc_pb_pp_frwrd_bmact_payload_convert(
          unit,
          &payload,
          bmact_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *found = is_found;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_entry_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_bmact_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_ENTRY_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_BMACT_ENTRY_KEY, bmac_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_entry_get_verify()", 0, 0);
}

/*********************************************************************
*   Set mapping from COS parameters (DP and TC) to the PCP
*   and DEI values to be set in the transmitted packet's
*   I-tag. This is the mapping to be used when the incoming
*   packet has no tags or pcp profile is set to use TC and
*   DP for the mapping.
*   Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx, /* ?? */
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  )
{
    uint32
        dp_tc = 0,
        reg_offset,
        fld_offset,
        fld_val,
        pcp_dei,
        res = SOC_SAND_OK;
    SOC_PB_PP_REGS
        *regs = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET_UNSAFE);
    
    SOC_PB_PP_MAC_IN_MAC_CHECK_IF_ENABLED(unit);
    
    regs = soc_pb_pp_regs();
    
    /*
     * the key is system-DP(2) and system-TC(3).
     * the table (4 regs) covers all possible combinations (32 entries, 4 bits each) 
     */
    dp_tc = (tc_ndx << 2) | dp_ndx;
    reg_offset = dp_tc / 8; 
    fld_offset = (dp_tc % 8) * 4;
    SOC_PB_PP_REG_GET(regs->epni.itag_tc_dp_map_regs[reg_offset].itag_tc_dp_map, fld_val, 20, exit);
  
    pcp_dei = (out_pcp << SOC_PB_PP_FRWRD_BMACT_DEI_NOF_BITS) | out_dei;  
    res = soc_sand_bitstream_set_any_field(
        &pcp_dei,
        fld_offset,
        SOC_PB_PP_FRWRD_BMACT_PCP_NOF_BITS + SOC_PB_PP_FRWRD_BMACT_DEI_NOF_BITS,
        &fld_val
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    SOC_PB_PP_REG_SET(regs->epni.itag_tc_dp_map_regs[reg_offset].itag_tc_dp_map, fld_val, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_set_unsafe()", 0, 0);  
}

uint32
  soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET_VERIFY);

    SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_FRWRD_BMACT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_BMACT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 40, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 50, exit);
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_set_verify()", 0, 0);
}

/*********************************************************************
*     Gets the configuration set by the
*     "soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_set" API.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  )
{
    uint32
        dp_tc = 0,
        reg_offset,
        fld_offset,
        fld_val,
        dei = 0,
        pcp = 0,
        res = SOC_SAND_OK;
    SOC_PB_PP_REGS
        *regs = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET_UNSAFE);
    
    SOC_SAND_CHECK_NULL_INPUT(out_pcp);
    SOC_SAND_CHECK_NULL_INPUT(out_dei);
    
    SOC_PB_PP_MAC_IN_MAC_CHECK_IF_ENABLED(unit);
    
    regs = soc_pb_pp_regs();
    
    /*
     * the key is system-DP(2) and system-TC(3).
     * the table (4 regs) covers all possible combinations (32 entries, 4 bits each) 
     */
    dp_tc = (tc_ndx << 2) | dp_ndx;
    reg_offset = dp_tc / 8; 
    fld_offset = (dp_tc % 8) * 4;
    SOC_PB_PP_REG_GET(regs->epni.itag_tc_dp_map_regs[reg_offset].itag_tc_dp_map, fld_val, 20, exit);
    
    res = soc_sand_bitstream_get_any_field(
        &fld_val,
        fld_offset,
        SOC_PB_PP_FRWRD_BMACT_DEI_NOF_BITS,
        &dei
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
    res = soc_sand_bitstream_get_any_field(
        &fld_val,
        fld_offset + SOC_PB_PP_FRWRD_BMACT_DEI_NOF_BITS,
        SOC_PB_PP_FRWRD_BMACT_PCP_NOF_BITS,
        &pcp
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
    *out_pcp = (SOC_SAND_PP_PCP_UP)pcp;
    *out_dei = (SOC_SAND_PP_DEI_CFI)dei;
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_get_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET_VERIFY);

    SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_FRWRD_BMACT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_BMACT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_get_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_bmact_default_sem_index_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ac_id
  )
{
    uint32
        res = SOC_SAND_OK;
    SOC_PB_PP_REGS
        *regs = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
        
    regs = soc_pb_pp_regs();
    
    SOC_SAND_ERR_IF_ABOVE_MAX(ac_id, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);

    SOC_PB_PP_REG_SET(regs->epni.default_sem_index_reg.default_sem_index, ac_id, 20, exit);
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_bmact_default_sem_index_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_frwrd_bmact module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_bmact_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_frwrd_bmact;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_frwrd_bmact module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_bmact_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_frwrd_bmact;
}
uint32
  SOC_PB_PP_BMACT_BVID_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_BMACT_BVID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /*SOC_SAND_ERR_IF_ABOVE_MAX(info->stp_topology_id, SOC_PB_PP_FRWRD_BMACT_STP_TOPOLOGY_ID_MAX, SOC_PB_PP_FRWRD_BMACT_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->b_fid_profile, SOC_PB_PP_FRWRD_BMACT_B_FID_PROFILE_MAX, SOC_PB_PP_FRWRD_BMACT_B_FID_PROFILE_OUT_OF_RANGE_ERR, 11, exit);*/

  SOC_SAND_ERR_IF_ABOVE_MAX(info->b_fid_profile, 1, SOC_PB_PP_FRWRD_BMACT_B_FID_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->uknown_da_dest.dest_id, 1, SOC_PB_PP_FRWRD_BMACT_DA_NOT_FOUND_ACTION_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->stp_topology_id, 0xf, SOC_PB_PP_FRWRD_BMACT_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_BMACT_BVID_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_BMACT_PBB_TE_VID_RANGE_verify(
    SOC_SAND_IN  SOC_PB_PP_BMACT_PBB_TE_VID_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->last_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_BMACT_PBB_TE_VID_RANGE_verify()",0,0);
}

uint32
  SOC_PB_PP_BMACT_ENTRY_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->b_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_BMACT_ENTRY_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_BMACT_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sys_port_id, SOC_PB_PP_FRWRD_BMACT_SYS_PORT_ID_MAX, SOC_PB_PP_FRWRD_BMACT_SYS_PORT_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->i_sid_domain, SOC_PB_PP_ISID_DOMAIN_MAX, SOC_PB_PP_ISID_DOMAIN_OUT_OF_RANGE_ERR, 30, exit);
  if (info->i_sid_domain != 0 && info->i_sid_domain < 16)
  {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ISID_DOMAIN_OUT_OF_RANGE_ERR, 30, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->sa_learn_fec_id, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_BMACT_ENTRY_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

