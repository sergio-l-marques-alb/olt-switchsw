#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_vsi.c,v 1.25 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_VSI
#include <soc/mem.h>


/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vsi.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#define ARAD_PP_VSI_FID_PROFILE_ID_MAX                           6
#define ARAD_PP_VSI_PROFILE_ID_MAX                               15
#define ARAD_PP_HIGH_VSI_PROFILE_ID_MAX                          3
#define ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_MAX                     7
#define ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MAX                     3
#define ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MIN                     1


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


CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_vsi[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_SYS_TO_LOCAL_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_MAP_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_DEFAULT_FRWRD_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_DEFAULT_FRWRD_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_DEFAULT_FRWRD_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_DEFAULT_FRWRD_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_DEFAULT_FRWRD_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_DEFAULT_FRWRD_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_DEFAULT_FRWRD_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_DEFAULT_FRWRD_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_VSI_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_vsi[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_VSI_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'default_forward_profile' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR",
    "The parameter 'stp_topology_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'fid_profile_id' is out of range. \n\r "
    "The range is: 0 - 6/ or SOC_PPD_VSI_FID_IS_VSID.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'mac_learn_profile_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_ORIENTATION_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_DA_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_DA_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'da_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_DA_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    ARAD_PP_VSI_TRAP_CODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'action_profile.trap_code' is out of range. \n\r "
    "The range is: ARAD_PP_TRAP_CODE_UNKNOWN_DA_0 - ARAD_PP_TRAP_CODE_UNKNOWN_DA_7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_VSI_OUT_OF_RANGE_ERR,
    "ARAD_PP_VSI_OUT_OF_RANGE_ERR",
    "'vsi' is out of range. \n\r "
    "The range is: 1 to 16K-1.\n\r ",
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
  arad_pp_vsi_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 value = 0;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* No high-vsi from Jericho */
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      /* set a constant 1-1 mapping from 2b high VSI profile to 4b VSI profile */
      soc_reg_field_set(unit, IHP_HIGH_VSI_PROFILEr, &value, HIGH_VSI_PROFILE_0f, 0);
      soc_reg_field_set(unit, IHP_HIGH_VSI_PROFILEr, &value, HIGH_VSI_PROFILE_1f, 1);
      soc_reg_field_set(unit, IHP_HIGH_VSI_PROFILEr, &value, HIGH_VSI_PROFILE_2f, 2);
      soc_reg_field_set(unit, IHP_HIGH_VSI_PROFILEr, &value, HIGH_VSI_PROFILE_3f, 3);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, WRITE_IHP_HIGH_VSI_PROFILEr(unit, value));
  }

  /* Enable MTU check for Header codes IPV4, IPV6 & MPLS */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1050, exit, READ_EPNI_LINK_FILTER_ENABLEr(unit, REG_PORT_ANY, &value));
  soc_reg_field_set(unit, EPNI_LINK_FILTER_ENABLEr, &value, MTU_CHECK_ENABLEf, 0x3E);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1060, exit, WRITE_EPNI_LINK_FILTER_ENABLEr(unit, REG_PORT_ANY, value));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Map VSI to sys-VSI and system VSI to egress VSI
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_vsi_map_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          sys_vsid,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              eg_local_vsid,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{ /* not supported in Arad */
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_MAP_ADD_UNSAFE);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_VSI_MAP_NOT_SUPPORTED_ERR, 1, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_vsi_map_add_unsafe() not supported", local_vsi_ndx, 0);
}


/*********************************************************************
*     Map VSI to sys-VSI and system VSI to egress VSI
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_vsi_sys_to_local_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          sys_vsid,
    SOC_SAND_OUT  ARAD_PP_VSI_ID                             *eg_local_vsid,
    SOC_SAND_OUT uint8                                 *found
  )
{ /* not supported in Arad */
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_MAP_ADD_UNSAFE);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_VSI_MAP_NOT_SUPPORTED_ERR, 1, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_vsi_map_get_unsafe() not supported", sys_vsid, 0);
}

uint32
  arad_pp_vsi_map_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          sys_vsid,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              eg_local_vsid
  )
{ /* not supported in Arad */
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_MAP_ADD_UNSAFE);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_VSI_MAP_NOT_SUPPORTED_ERR, 1, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_vsi_map_add_verify() not supported", local_vsi_ndx, 0);
}

/*********************************************************************
*     Remove mapping of local VSI to system VSI, and system
 *     VSI to local VSI.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_vsi_map_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          sys_vsid
  )
{ /* not supported in Arad */
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_MAP_ADD_UNSAFE);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_VSI_MAP_NOT_SUPPORTED_ERR, 1, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_vsi_map_remove_unsafe() not supported", local_vsi_ndx, 0);
}

uint32
  arad_pp_vsi_map_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          sys_vsid
  )
{ /* not supported in Arad */
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_MAP_ADD_UNSAFE);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_VSI_MAP_NOT_SUPPORTED_ERR, 1, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_pp_vsi_map_remove_verify() not supported", local_vsi_ndx, 0);
}


/*********************************************************************
*     Set the Virtual Switch Instance information. After
*     setting the VSI, the user may attach L2 Logical
*     Interfaces to it: ACs; PWEs
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_vsi_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  ARAD_PP_VSI_INFO                            *vsi_info
  )
{
  uint32  fid_val;
  uint32  res = SOC_SAND_OK;
  uint32  frwrd_dest;
  uint32  temp;
  uint32  entry_data[2];
  uint32 entry_index, vsi_profile_data_entry, vsi_profile;

  ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_DATA
    mact_fid_profile_db_tbl_data;
  ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA
    fid_class_2_fid_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  /* convert forwarding decision data to encoded da_not_found_destination */

   
  res = arad_pp_fwd_decision_in_buffer_build(
    unit,
    ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
    &vsi_info->default_forwarding,
    &frwrd_dest,
    &temp /* ASD */
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  if (vsi_ndx < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)) {

    /* handle the first 4K VSI IDs that support full configuration */
    ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA
      vsi_low_cfg_1_data;
    ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA
      vsi_low_cfg_2_data;

    ARAD_CLEAR(&vsi_low_cfg_1_data, ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA, 1);
    ARAD_CLEAR(&vsi_low_cfg_2_data, ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA, 1);    

    res = arad_pp_ihp_vsi_low_cfg_1_tbl_get_unsafe(unit, vsi_ndx, &vsi_low_cfg_1_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

    res = arad_pp_ihp_vsi_low_cfg_2_tbl_get_unsafe(unit, vsi_ndx, &vsi_low_cfg_2_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    /* vsi_low_cfg_1_data.my_mac is not changed */
    vsi_low_cfg_1_data.my_mac_valid = SOC_SAND_NUM2BOOL(vsi_info->enable_my_mac);
    vsi_low_cfg_2_data.da_not_found_destination = frwrd_dest;
    vsi_low_cfg_2_data.fid_class = (vsi_info->fid_profile_id != ARAD_PP_VSI_FID_IS_VSID) ?
                                    vsi_info->fid_profile_id : 7; /* set FID class */

    vsi_low_cfg_1_data.topology_id = vsi_info->stp_topology_id;
    vsi_low_cfg_2_data.profile = vsi_info->profile_ingress; /* VSI general profile, used in PMF */

    res = arad_pp_ihp_vsi_low_cfg_1_tbl_set_unsafe(unit, vsi_ndx, &vsi_low_cfg_1_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = arad_pp_ihp_vsi_low_cfg_2_tbl_set_unsafe(unit, vsi_ndx, &vsi_low_cfg_2_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    /* map VSI to FID */
    if (vsi_info->fid_profile_id == ARAD_PP_VSI_FID_IS_VSID)
    {
      fid_val = vsi_ndx;
    }
    else
    {
      res = arad_pp_ihp_fid_class_2_fid_tbl_get_unsafe(
              unit,
              vsi_info->fid_profile_id,
              &fid_class_2_fid_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
      fid_val = fid_class_2_fid_tbl_data.fid;
    }

	/*setting the mtu into the profile: write into VSI_PROFILE_MEMORY the 2 lsbs of vsi_info->profile*/
	/* get profile */
    entry_index = vsi_ndx >> 4; /*each entry contains 16 vsi profiles*/
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 27, exit, READ_EGQ_VSI_PROFILEm(unit, MEM_BLOCK_ANY, entry_index, entry_data));
    soc_EGQ_VSI_PROFILEm_field_get(unit, entry_data, VSI_PROFILE_DATAf, &vsi_profile_data_entry); 

    /* set relevant vsi entry with 2 lsbits of vsi_info->profile. The entry is of 2 bits starting from (vsi % 16)*2 */
    vsi_profile = vsi_info->profile_egress & 0x3;
    SHR_BITCOPY_RANGE(&vsi_profile_data_entry, ((vsi_ndx%16)*2), &vsi_profile, 0, 2);
    soc_EGQ_VSI_PROFILEm_field_set(unit, entry_data, VSI_PROFILE_DATAf, &vsi_profile_data_entry);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 28, exit, WRITE_EGQ_VSI_PROFILEm(unit, MEM_BLOCK_ANY, entry_index, entry_data));

  } else {

    /* handle the remain remaining 28K VSI IDs */
    ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_DATA
      vsi_high_da_not_found_dest_data;
    ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_DATA
      vsi_high_profile_data;
    ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_DATA
      vsi_high_my_mac_data;
    const ARAD_PP_VSI_ID rel_index = vsi_ndx - SOC_DPP_DEFS_GET(unit, nof_vsi_lowers);

    vsi_high_da_not_found_dest_data.da_not_found_destination = frwrd_dest;
    res = arad_pp_ihp_vsi_high_da_not_found_destination_tbl_set_unsafe(unit, rel_index, &vsi_high_da_not_found_dest_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = arad_pp_ihp_vsi_high_profile_tbl_get_unsafe(unit, rel_index / 8, &vsi_high_profile_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
    vsi_high_profile_data.index[rel_index % 8] = vsi_info->profile_ingress;
    res = arad_pp_ihp_vsi_high_profile_tbl_set_unsafe(unit, rel_index / 8, &vsi_high_profile_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

    res = arad_pp_ihp_vsi_high_my_mac_tbl_get_unsafe(unit, rel_index / 8, &vsi_high_my_mac_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);
    vsi_high_my_mac_data.valid[rel_index % 8] = vsi_info->enable_my_mac;
    res = arad_pp_ihp_vsi_high_my_mac_tbl_set_unsafe(unit, rel_index / 8, &vsi_high_my_mac_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

    fid_val = vsi_ndx;

  }

  res = arad_pp_ihp_mact_fid_profile_db_tbl_get_unsafe(unit, fid_val / 8, &mact_fid_profile_db_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  mact_fid_profile_db_tbl_data.profile_pointer[fid_val % 8] = vsi_info->mac_learn_profile_id;
  res = arad_pp_ihp_mact_fid_profile_db_tbl_set_unsafe(unit, fid_val / 8, &mact_fid_profile_db_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_info_set_unsafe()", vsi_ndx, 0);
}

uint32
  arad_pp_vsi_info_set_verify(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID    vsi_ndx,
    SOC_SAND_IN  ARAD_PP_VSI_INFO  *vsi_info
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, ARAD_PP_VSI_ID_MAX, ARAD_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  res = ARAD_PP_VSI_INFO_verify(unit, vsi_info, vsi_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_info_set_verify()", vsi_ndx, 0);
}

uint32
  arad_pp_vsi_info_get_verify(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID    vsi_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_INFO_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, ARAD_PP_VSI_ID_MAX, ARAD_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_info_get_verify()", vsi_ndx, 0);
}

/*********************************************************************
*     Get the Virtual Switch Instance information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT ARAD_PP_VSI_INFO                            *vsi_info
  )
{
  uint32  fid_val;
  uint32  res = SOC_SAND_OK;
  uint32  frwrd_dest;
  uint32  temp = 0;
  uint32 whole_entry_data[2];
  uint32 vsi_profile_data, vsi_profile;
  int entry_index;


  ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_DATA
    mact_fid_profile_db_tbl_data;
  ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA
    fid_class_2_fid_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);
  ARAD_PP_VSI_INFO_clear(vsi_info);

  if (vsi_ndx < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)) {

    /* handle the first 4K VSI IDs that support full configuration */
    ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA
      vsi_low_cfg_1_data;
    ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA
      vsi_low_cfg_2_data;

    res = arad_pp_ihp_vsi_low_cfg_1_tbl_get_unsafe(unit, vsi_ndx, &vsi_low_cfg_1_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = arad_pp_ihp_vsi_low_cfg_2_tbl_get_unsafe(unit, vsi_ndx, &vsi_low_cfg_2_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    vsi_info->enable_my_mac = SOC_SAND_NUM2BOOL(vsi_low_cfg_1_data.my_mac_valid);
    vsi_info->stp_topology_id = vsi_low_cfg_1_data.topology_id;
    vsi_info->profile_ingress = vsi_low_cfg_2_data.profile;
    frwrd_dest = vsi_low_cfg_2_data.da_not_found_destination;

    /* get fid_profile_id and map VSI to FID */
    if (vsi_low_cfg_2_data.fid_class == 7)
    {
      vsi_info->fid_profile_id = ARAD_PP_VSI_FID_IS_VSID;
      fid_val = vsi_ndx;
    }
    else
    {
      vsi_info->fid_profile_id = vsi_low_cfg_2_data.fid_class;
      res = arad_pp_ihp_fid_class_2_fid_tbl_get_unsafe( unit, vsi_info->fid_profile_id, &fid_class_2_fid_tbl_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
      fid_val = fid_class_2_fid_tbl_data.fid;
    }

    /* get vsi profile data from VSI_PROFILE register */
    entry_index = vsi_ndx >> 4; /* each entry contains 16 vsi profiles */
    res = READ_EGQ_VSI_PROFILEm(unit, MEM_BLOCK_ANY, entry_index, whole_entry_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
    soc_EGQ_VSI_PROFILEm_field_get(unit, whole_entry_data, VSI_PROFILE_DATAf, &vsi_profile_data);   
    /*vsi_profile_data -> get field of 2 bits starting from (vsi % 16)*2 */
    vsi_profile = 0;
    SHR_BITCOPY_RANGE(&vsi_profile, 0, &vsi_profile_data, (vsi_ndx%16)*2, 2);
    vsi_info->profile_egress = (vsi_info->profile_egress & (0xFFFFFFFC)) | vsi_profile; /*vsi profile is 2lsbs of vsi_info->profile*/

  } else {

    /* handle the remain remaining 28K VSI IDs */
    ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_DATA
      vsi_high_da_not_found_dest_data;
    ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_DATA
      vsi_high_profile_data;
    ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_DATA
      vsi_high_my_mac_data;
    const ARAD_PP_VSI_ID rel_index = vsi_ndx - SOC_DPP_DEFS_GET(unit, nof_vsi_lowers);

    res = arad_pp_ihp_vsi_high_da_not_found_destination_tbl_get_unsafe(unit, rel_index, &vsi_high_da_not_found_dest_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    frwrd_dest = vsi_high_da_not_found_dest_data.da_not_found_destination;

    res = arad_pp_ihp_vsi_high_profile_tbl_get_unsafe(unit, rel_index / 8, &vsi_high_profile_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
    vsi_info->profile_ingress = vsi_high_profile_data.index[rel_index % 8];

    res = arad_pp_ihp_vsi_high_my_mac_tbl_get_unsafe(unit, rel_index / 8, &vsi_high_my_mac_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
    vsi_info->enable_my_mac = vsi_high_my_mac_data.valid[rel_index % 8];

    vsi_info->stp_topology_id = 0;
    vsi_info->fid_profile_id = ARAD_PP_VSI_FID_IS_VSID;
    fid_val = vsi_ndx;

  }

  /* convert encoded da_not_found_destination to forwarding decision data  */

  res = arad_pp_fwd_decision_in_buffer_parse(
    unit,
    ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
    frwrd_dest,
    temp /* ASD */,
    ARAD_PP_FWD_DECISION_PARSE_DEST,
    &vsi_info->default_forwarding
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihp_mact_fid_profile_db_tbl_get_unsafe(unit, fid_val / 8, &mact_fid_profile_db_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  vsi_info->mac_learn_profile_id = mact_fid_profile_db_tbl_data.profile_pointer[fid_val % 8];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_info_get_unsafe()", vsi_ndx, 0);
}

/*********************************************************************
 *     Set egress vsi profile info to profile index.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_vsi_egress_mtu_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  )
{
  uint32  res;
  uint64 reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_EGRESS_PROFILE_SET_UNSAFE);

  /*Setting both EGQ_MTU and EPNI_MTU*/
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EGQ_MTUr(unit, REG_PORT_ANY, &reg_val));
  switch (vsi_profile_ndx) {
  case 1:
       soc_reg64_field32_set(unit, EGQ_MTUr, &reg_val, MTU_1f, mtu_val);
	   break;
  case 2:
       soc_reg64_field32_set(unit, EGQ_MTUr, &reg_val, MTU_2f, mtu_val);
	   break;
  case 3:
       soc_reg64_field32_set(unit, EGQ_MTUr, &reg_val, MTU_3f, mtu_val);
	   break;
  default:
	  SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 15, exit);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_EGQ_MTUr(unit, REG_PORT_ANY, reg_val));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EPNI_MTUr(unit, REG_PORT_ANY, &reg_val));
  switch (vsi_profile_ndx) {
  case 1:
       soc_reg64_field32_set(unit, EPNI_MTUr, &reg_val, MTU_1f, mtu_val);
	   break;
  case 2:
       soc_reg64_field32_set(unit, EPNI_MTUr, &reg_val, MTU_2f, mtu_val);
	   break;
  case 3:
       soc_reg64_field32_set(unit, EPNI_MTUr, &reg_val, MTU_3f, mtu_val);
	   break;
  /* we must default. overwise - compilation error */
  /* coverity[dead_error_begin : FALSE] */
  default:
	  SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 35, exit);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_EPNI_MTUr(unit, REG_PORT_ANY, reg_val));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_set_unsafe()", vsi_profile_ndx, 0);
}

uint32
  arad_pp_vsi_egress_mtu_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_EGRESS_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_profile_ndx, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MAX ,ARAD_PP_VSI_EGRESS_PROFILE_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_ERR_IF_BELOW_MIN(vsi_profile_ndx, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MIN ,ARAD_PP_VSI_EGRESS_PROFILE_INDEX_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_set_verify()", vsi_profile_ndx, 0);
}

uint32
  arad_pp_vsi_egress_mtu_get_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_EGRESS_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_profile_ndx, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MAX, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_ERR_IF_BELOW_MIN(vsi_profile_ndx, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_MIN, ARAD_PP_VSI_EGRESS_PROFILE_INDEX_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_get_verify()", vsi_profile_ndx, 0);
}

/*********************************************************************
 *     Get egress vsi profile info from profile index.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_vsi_egress_mtu_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_OUT  uint32                              *mtu_val
  )
{
  uint32  res;
  uint64  reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_VSI_EGRESS_PROFILE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(mtu_val);

  res = READ_EPNI_MTUr(unit, REG_PORT_ANY, &reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  switch (vsi_profile_ndx) {
  case 1:
	   *mtu_val = soc_reg64_field32_get(unit, EPNI_MTUr, reg_val, MTU_1f);
	   break;
  case 2:
	   *mtu_val = soc_reg64_field32_get(unit, EPNI_MTUr, reg_val, MTU_2f);
	   break;
  case 3:
	   *mtu_val = soc_reg64_field32_get(unit, EPNI_MTUr, reg_val, MTU_3f);
	   break;
  default:
	  SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_egress_mtu_get_unsafe()", vsi_profile_ndx, 0);
}


/*********************************************************************
*     Sets Trap information for Layer 2 control protocol
 *     frames. Packet is an MEF layer 2 control protocol
 *     service frame When DA matches 01-80-c2-00-00-XX where XX
 * = 8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_vsi_l2cp_trap_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_HANDLE_TYPE               handle_type
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    index_ingress, index_egress;
  soc_reg_above_64_val_t  reg_above_64;
  uint32 is_tunnel=0;
  uint32 is_peer=0;
  uint32 is_drop=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);

  index_ingress = SOC_PPD_VSI_L2CP_KEY_ENTRY_OFFSET(l2cp_key->l2cp_profile_ingress, l2cp_key->da_mac_address_lsb);
  index_egress = SOC_PPD_VSI_L2CP_KEY_ENTRY_OFFSET(l2cp_key->l2cp_profile_egress, l2cp_key->da_mac_address_lsb);

  switch (handle_type) {
  case SOC_PPD_VSI_L2CP_HANDLE_TYPE_TUNNEL:
	  is_tunnel = 1;
	  is_peer = 0;
	  is_drop = 0;
	  break;
  case SOC_PPD_VSI_L2CP_HANDLE_TYPE_PEER:
	  is_tunnel = 0;
	  is_peer = 1;
	  is_drop = 0;
	  break;
  case SOC_PPD_VSI_L2CP_HANDLE_TYPE_DROP:
	  is_tunnel = 0;
	  is_peer = 0;
	  is_drop = 1;
	  break;
  case SOC_PPD_VSI_L2CP_HANDLE_TYPE_NORMAL:
	  is_tunnel = 0;
	  is_peer = 0;
	  is_drop = 0;
	  break;
  default:
	  SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
  }

  /* Ingress */
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MEF_L_2_CP_TRANSPARENT_BITMAPr(unit, REG_PORT_ANY, reg_above_64));
  SHR_BITCOPY_RANGE(reg_above_64, index_ingress, &is_tunnel, 0, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_MEF_L_2_CP_TRANSPARENT_BITMAPr(unit, REG_PORT_ANY, reg_above_64));
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, 0, reg_above_64));
  SHR_BITCOPY_RANGE(reg_above_64, index_ingress, &is_peer, 0, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, SOC_CORE_ALL, reg_above_64));
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, 0, reg_above_64));
  SHR_BITCOPY_RANGE(reg_above_64, index_ingress, &is_drop, 0, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, SOC_CORE_ALL, reg_above_64));

  /* Egress */
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_EPNI_MEF_L_2_CP_TRANSPARANT_BITMAPr(unit, REG_PORT_ANY, reg_above_64));
  SHR_BITCOPY_RANGE(reg_above_64, index_egress, &is_tunnel, 0, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_EPNI_MEF_L_2_CP_TRANSPARANT_BITMAPr(unit, REG_PORT_ANY, reg_above_64));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_l2cp_trap_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Gets Trap information for Layer 2 control protocol
 *     frames. Packet is an MEF layer 2 control protocol
 *     service frame When DA matches 01-80-c2-00-00-XX where XX
 * = 8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_vsi_l2cp_trap_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_OUT SOC_PPD_VSI_L2CP_HANDLE_TYPE               *handle_type
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    index_ingress;
  soc_reg_above_64_val_t  reg_above_64;
  uint32 is_tunnel=0;
  uint32 is_peer=0;
  uint32 is_drop=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);

  index_ingress = SOC_PPD_VSI_L2CP_KEY_ENTRY_OFFSET(l2cp_key->l2cp_profile_ingress, l2cp_key->da_mac_address_lsb);

  /* Ingress */
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MEF_L_2_CP_TRANSPARENT_BITMAPr(unit, REG_PORT_ANY, reg_above_64));
  SHR_BITCOPY_RANGE(&is_tunnel, 0, reg_above_64, index_ingress, 1);
  if (is_tunnel) {
	  *handle_type = SOC_PPD_VSI_L2CP_HANDLE_TYPE_TUNNEL;
  }
  else {
	  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, 0, reg_above_64));
	  SHR_BITCOPY_RANGE(&is_peer, 0, reg_above_64, index_ingress, 1);
	  if (is_peer) {
		  *handle_type = SOC_PPD_VSI_L2CP_HANDLE_TYPE_PEER;
	  }
	  else {
		  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
		  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, 0, reg_above_64));
		  SHR_BITCOPY_RANGE(&is_drop, 0, reg_above_64, index_ingress, 1);
		  if (is_peer) {
			  *handle_type = SOC_PPD_VSI_L2CP_HANDLE_TYPE_DROP;
		  }
		  else {
			  *handle_type = SOC_PPD_VSI_L2CP_HANDLE_TYPE_NORMAL;
		  }
	  }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_l2cp_trap_get_unsafe()", 0, 0);
}

uint32
  arad_pp_vsi_l2cp_trap_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_HANDLE_TYPE               handle_type
  )
{
 /* uint32
    res = SOC_SAND_OK;*/

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_L2CP_KEY, l2cp_key, 10, exit);*/ 
  SOC_SAND_ERR_IF_ABOVE_MAX(handle_type, SOC_PPD_VSI_L2CP_HANDLE_TYPES-1, 0 , 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_l2cp_trap_set_verify()", 0, 0);
}

uint32
  arad_pp_vsi_l2cp_trap_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_L2CP_KEY, l2cp_key, 10, exit);*/ 
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_l2cp_trap_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
*     arad_pp_api_vsi module.
*     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_vsi_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_vsi;
}

/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_vsi module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_vsi_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_vsi;
}

uint32
  ARAD_PP_VSI_INFO_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_PP_VSI_INFO *info,
    SOC_SAND_IN  ARAD_PP_VSI_ID    vsi_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mac_learn_profile_id, ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_MAX, ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  /* not checking info->default_forwarding */
  /* assume SOC_SAND_NUM2BOOL(vsi_info->enable_my_mac can hold any value to be translated to bool */

  if (vsi_ndx < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)) {
    /* handle the first 4K VSI IDs that support full configuration */
    if (info->fid_profile_id != ARAD_PP_VSI_FID_IS_VSID)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->fid_profile_id, ARAD_PP_VSI_FID_PROFILE_ID_MAX, ARAD_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR, 20, exit);
    }
    SOC_SAND_ERR_IF_ABOVE_NOF(info->stp_topology_id, SOC_DPP_DEFS_GET(unit, nof_topology_ids), ARAD_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR, 22, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->profile_ingress, ARAD_PP_VSI_PROFILE_ID_MAX, ARAD_PP_VSI_PROFILE_ID_OUT_OF_RANGE_ERR, 24, exit);
  } else {
    /* handle the remain remaining 28K VSI IDs */
    if (info->fid_profile_id != ARAD_PP_VSI_FID_IS_VSID) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR, 30, exit);
    }
    if (info->stp_topology_id) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR, 32, exit);
    }
    SOC_SAND_ERR_IF_ABOVE_MAX(info->profile_ingress, ARAD_PP_HIGH_VSI_PROFILE_ID_MAX, ARAD_PP_VSI_PROFILE_ID_OUT_OF_RANGE_ERR, 34, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_VSI_INFO_verify()",0,0);
}

#endif /* of #if defined(BCM_88650_A0) */
