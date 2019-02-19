/* $Id: pb_pp_eg_ac.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_eg_ac.c
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
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_ac.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_esem_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_EG_AC_VLAN_DOMAIN_MAX                            (SOC_SAND_U32_MAX)
#define SOC_PB_PP_EG_AC_PEP_EDIT_PROFILE_MAX                       (7)
#define SOC_PB_PP_EG_AC_PCP_PROFILE_MAX                            (15)
#define SOC_PB_PP_EG_AC_EDIT_PROFILE_MAX                           (SOC_SAND_U32_MAX)
#define SOC_PB_PP_EG_AC_NOF_TAGS_MAX                               (SOC_PB_PP_VLAN_TAGS_MAX)

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
  Soc_pb_pp_procedure_desc_element_eg_ac[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_MP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_MP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_MP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_MP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_MP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_MP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_MP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_MP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_MAP_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_MAP_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_CVID_MAP_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_MAP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_MAP_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_MAP_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_PORT_VSI_MAP_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_AC_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_eg_ac[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_EG_AC_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_AC_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_AC_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_AC_VLAN_DOMAIN_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_domain' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_AC_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_AC_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'pep_edit_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_AC_PCP_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_AC_PCP_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_AC_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_AC_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'edit_profile' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_AC_NOF_TAGS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_AC_NOF_TAGS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_tags' is out of range. \n\r "
    "The range is: 2 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_AC_MAX_LEVEL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_AC_MAX_LEVEL_OUT_OF_RANGE_ERR",
    "The parameter 'max_level' is out of range. \n\r "
    "The range is: Soc_petra - B: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_ESEM_REMOVE_FAILED,
    "SOC_PB_PP_ESEM_REMOVE_FAILED",
    "Failed removing an existing ESEM entry\n\r ",
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
  soc_pb_pp_eg_ac_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    fld_val;
  SOC_PB_PP_REGS
    *pp_regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  pp_regs = soc_pb_pp_regs();

  /* enable ESEM */
  fld_val = 1;
  SOC_PB_PP_FLD_SET(pp_regs->epni.esem_management_unit_configuration_reg.esem_mngmnt_unit_enable,fld_val,10,exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets the editing information for packets
 *     associated with AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_ac_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_INFO                          *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_DATA
    sem_res_tbl;
  uint32
    vid_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_INFO_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);

  /*
   *	Egress VLAN Editing
   */
  sem_res_tbl.pcp_dei_profile = ac_info->edit_info.pcp_profile;
  sem_res_tbl.vlan_edit_profile = ac_info->edit_info.edit_profile;
  sem_res_tbl.vid[0] = 0;
  sem_res_tbl.vid[1] = 0;
  
  for (vid_idx = 0; vid_idx < ac_info->edit_info.nof_tags; vid_idx++)
  {
    sem_res_tbl.vid[vid_idx] = ac_info->edit_info.vlan_tags[vid_idx].vid;
  }
  
  res = soc_pb_pp_epni_small_em_result_memory_tbl_set_unsafe(
          unit,
          out_ac_ndx,
          &sem_res_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_info_set_unsafe()", out_ac_ndx, 0);
}

uint32
  soc_pb_pp_eg_ac_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_INFO                          *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_ac_ndx, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_INFO, ac_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_info_set_verify()", out_ac_ndx, 0);
}

uint32
  soc_pb_pp_eg_ac_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               out_ac_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_INFO_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_ac_ndx, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_info_get_verify()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the editing information for packets
*     associated with AC.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_ac_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_AC_INFO                              *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_DATA
    sem_res_tbl;
  uint32
    vid_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_INFO_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_PB_PP_EG_AC_INFO_clear(ac_info);

  /*
   *	Egress VLAN Editing
   */
  res = soc_pb_pp_epni_small_em_result_memory_tbl_get_unsafe(
          unit,
          out_ac_ndx,
          &sem_res_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ac_info->edit_info.pcp_profile = sem_res_tbl.pcp_dei_profile;
  ac_info->edit_info.edit_profile = sem_res_tbl.vlan_edit_profile;
  
  for (vid_idx = 0; vid_idx < SOC_PB_PP_VLAN_TAGS_MAX; vid_idx++)
  {
    ac_info->edit_info.vlan_tags[vid_idx].vid = sem_res_tbl.vid[vid_idx];
  }
  /* always 2 in PB */
  ac_info->edit_info.nof_tags = 2;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_info_get_unsafe()", out_ac_ndx, 0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an outgoing Attachment-Circuit (port x VSI) and
 *     MD-level, and to determine the action to perform. If the
 *     MP is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_ac_mp_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_MP_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EGQ_CFM_TRAP_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_MP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  res = soc_pb_pp_egq_cfm_trap_tbl_get_unsafe(
          unit,
          out_ac_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  data.cfm_max_level = info->mp_level;
  data.cfm_trap_valid = info->is_valid;

  res = soc_pb_pp_egq_cfm_trap_tbl_set_unsafe(
          unit,
          out_ac_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_mp_info_set_unsafe()", out_ac_ndx, 0);
}

uint32
  soc_pb_pp_eg_ac_mp_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_MP_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_MP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_ac_ndx, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_MP_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_mp_info_set_verify()", out_ac_ndx, 0);
}

uint32
  soc_pb_pp_eg_ac_mp_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_MP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_ac_ndx, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_mp_info_get_verify()", out_ac_ndx, 0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an outgoing Attachment-Circuit (port x VSI) and
 *     MD-level, and to determine the action to perform. If the
 *     MP is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_ac_mp_info_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_AC_MP_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EGQ_CFM_TRAP_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_MP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_EG_AC_MP_INFO_clear(info);

  res = soc_pb_pp_egq_cfm_trap_tbl_get_unsafe(
          unit,
          out_ac_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->mp_level = data.cfm_max_level;
  info->is_valid = SOC_SAND_NUM2BOOL(data.cfm_trap_valid);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_mp_info_get_unsafe()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the editing information for packets NOT
*     associated with AC and to be transmitted from VBP port
*     (not CEP port).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_ac_port_vsi_info_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                                   out_ac_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_VBP_KEY                           *vbp_key,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_INFO                              *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ESEM_KEY
    esem_key;
  SOC_PB_PP_ESEM_ENTRY
    esem_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_VSI_INFO_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*
   *	ESEM Key
   */
  esem_key.key_type = SOC_PB_PP_ESEM_KEY_TYPE_VD_VSI;
  esem_key.key_info.vd_vsi.vd = vbp_key->vlan_domain;
  esem_key.key_info.vd_vsi.vsi = vbp_key->vsi;

  /*
   *	ESEM Entry value
   */
  esem_val.out_ac = out_ac_ndx;

  res = soc_pb_pp_esem_entry_add_unsafe(
          unit,
          &esem_key,
          &esem_val,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	OutAC Attributes
   */
  if(*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES)
  {
    res = soc_pb_pp_eg_ac_info_set_unsafe(
            unit,
            out_ac_ndx,
            ac_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_vsi_info_add_unsafe()", out_ac_ndx, 0);
}

uint32
  soc_pb_pp_eg_ac_port_vsi_info_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                                   out_ac_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_VBP_KEY                           *vbp_key,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_INFO                              *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_VSI_INFO_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_ac_ndx, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_VBP_KEY, vbp_key, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_INFO, ac_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_vsi_info_add_verify()", out_ac_ndx, 0);
}

/*********************************************************************
*     Removes editing information of packets NOT
*     associated with AC and to be transmitted from VBP port
*     (not CEP port).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_ac_port_vsi_info_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_VBP_KEY                           *vbp_key,
    SOC_SAND_OUT SOC_PB_PP_AC_ID                                   *out_ac
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ESEM_KEY
    esem_key;
  SOC_PB_PP_ESEM_ENTRY
    esem_val;
  uint8
    is_found,
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_VSI_INFO_REMOVE_UNSAFE);
  

  /*
   *	ESEM Key
   */
  esem_key.key_type = SOC_PB_PP_ESEM_KEY_TYPE_VD_VSI;
  esem_key.key_info.vd_vsi.vd = vbp_key->vlan_domain;
  esem_key.key_info.vd_vsi.vsi = vbp_key->vsi;

  res = soc_pb_pp_esem_entry_get_unsafe(
          unit,
          &esem_key,
          &esem_val,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (is_found)
  {
    *out_ac = esem_val.out_ac;
  
    res = soc_pb_pp_esem_entry_remove_unsafe(
            unit,
            &esem_key,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (!success)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ESEM_REMOVE_FAILED, 30, exit);
    }
  }
  else
  {
    *out_ac = SOC_PB_PP_AC_ID_INVALID;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_vsi_info_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_ac_port_vsi_info_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_VBP_KEY                           *vbp_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_VSI_INFO_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_VBP_KEY, vbp_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_vsi_info_remove_verify()", 0, 0);
}

/*********************************************************************
*     Gets the editing information for packets NOT
 *     associated with AC and to be transmitted from VBP port
 *     (not CEP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_ac_port_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_VBP_KEY                           *vbp_key,
    SOC_SAND_OUT SOC_PB_PP_AC_ID                                   *out_ac,
    SOC_SAND_OUT SOC_PB_PP_EG_AC_INFO                              *ac_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ESEM_KEY
    esem_key;
  SOC_PB_PP_ESEM_ENTRY
    esem_val;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_VSI_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vbp_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_EG_AC_INFO_clear(ac_info);

  /*
   *	ESEM Key
   */
  esem_key.key_type = SOC_PB_PP_ESEM_KEY_TYPE_VD_VSI;
  esem_key.key_info.vd_vsi.vd = vbp_key->vlan_domain;
  esem_key.key_info.vd_vsi.vsi = vbp_key->vsi;

  /*
   *	ESEM Entry value
   */
  res = soc_pb_pp_esem_entry_get_unsafe(
          unit,
          &esem_key,
          &esem_val,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  *out_ac = esem_val.out_ac;

  *found = is_found;

  if (is_found)
  {
    res = soc_pb_pp_eg_ac_info_get_unsafe(
            unit,
            esem_val.out_ac,
            ac_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_vsi_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_ac_port_vsi_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_VBP_KEY                           *vbp_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_VSI_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_VBP_KEY, vbp_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_vsi_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets the editing information for packets NOT
 *     associated with AC and to be transmitted from CEP port
 *     (not VBP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_ac_port_cvid_info_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ESEM_KEY
    esem_key;
  SOC_PB_PP_ESEM_ENTRY
    esem_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_CVID_INFO_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*
   *	ESEM Key
   */
  esem_key.key_type = SOC_PB_PP_ESEM_KEY_TYPE_VD_CVID;
  esem_key.key_info.vd_cvid.vd = cep_key->vlan_domain;
  esem_key.key_info.vd_cvid.cvid = cep_key->cvid;

  /*
   *	ESEM Entry value
   */
  esem_val.out_ac = out_ac_ndx;

  res = soc_pb_pp_esem_entry_add_unsafe(
          unit,
          &esem_key,
          &esem_val,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	OutAC Attributes
   */
  if(*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES)
  {
    res = soc_pb_pp_eg_ac_info_set_unsafe(
            unit,
            out_ac_ndx,
            ac_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_cvid_info_add_unsafe()", out_ac_ndx, 0);
}

uint32
  soc_pb_pp_eg_ac_port_cvid_info_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_INFO                          *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_CVID_INFO_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_ac_ndx, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_CEP_PORT_KEY, cep_key, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_INFO, ac_info, 30, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_cvid_info_add_verify()", out_ac_ndx, 0);
}

/*********************************************************************
*     Removes editing information of packets NOT
 *     associated with AC and to be transmitted from CEP port
 *     (not VBP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_ac_port_cvid_info_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PB_PP_AC_ID                               *out_ac
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ESEM_KEY
    esem_key;
  SOC_PB_PP_ESEM_ENTRY
    esem_val;
  uint8
    is_found,
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_CVID_INFO_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);

  /*
   *	ESEM Key
   */
  esem_key.key_type = SOC_PB_PP_ESEM_KEY_TYPE_VD_CVID;
  esem_key.key_info.vd_cvid.vd = cep_key->vlan_domain;
  esem_key.key_info.vd_cvid.cvid = cep_key->cvid;
  
  res = soc_pb_pp_esem_entry_get_unsafe(
          unit,
          &esem_key,
          &esem_val,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (is_found)
  {
    *out_ac = esem_val.out_ac;
  
    res = soc_pb_pp_esem_entry_remove_unsafe(
            unit,
            &esem_key,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (!success)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ESEM_REMOVE_FAILED, 30, exit);
    }
  }
  else
  {
    *out_ac = SOC_PB_PP_AC_ID_INVALID;
  }

  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_cvid_info_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_ac_port_cvid_info_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_CEP_PORT_KEY                  *cep_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_CVID_INFO_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_CEP_PORT_KEY, cep_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_cvid_info_remove_verify()", 0, 0);
}

/*********************************************************************
*     Gets the editing information for packets NOT
 *     associated with AC and to be transmitted from CEP port
 *     (not VBP port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_ac_port_cvid_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_CEP_PORT_KEY                  *cep_key,
    SOC_SAND_OUT SOC_PB_PP_AC_ID                               *out_ac,
    SOC_SAND_OUT SOC_PB_PP_EG_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ESEM_KEY
    esem_key;
  SOC_PB_PP_ESEM_ENTRY
    esem_val;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_CVID_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cep_key);
  SOC_SAND_CHECK_NULL_INPUT(out_ac);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_EG_AC_INFO_clear(ac_info);

  /*
   *	ESEM Key
   */
  esem_key.key_type = SOC_PB_PP_ESEM_KEY_TYPE_VD_CVID;
  esem_key.key_info.vd_cvid.vd = cep_key->vlan_domain;
  esem_key.key_info.vd_cvid.cvid = cep_key->cvid;

  /*
   *	ESEM Entry value
   */
  res = soc_pb_pp_esem_entry_get_unsafe(
          unit,
          &esem_key,
          &esem_val,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  *out_ac = esem_val.out_ac;
  *found = is_found;

  if (is_found)
  {
    res = soc_pb_pp_eg_ac_info_get_unsafe(
      unit,
      esem_val.out_ac,
      ac_info
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_cvid_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_ac_port_cvid_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_AC_CEP_PORT_KEY                  *cep_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_AC_PORT_CVID_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_CEP_PORT_KEY, cep_key, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_ac_port_cvid_info_get_verify()", 0, 0);
}


/*********************************************************************
*     Get the pointer to the list of procedures of the
*     soc_pb_pp_api_eg_ac module.
*     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_eg_ac_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_eg_ac;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
*     soc_pb_pp_api_eg_ac module.
*     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_eg_ac_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_eg_ac;
}
uint32
  SOC_PB_PP_EG_AC_VBP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_AC_VBP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan_domain, SOC_PB_PP_EG_AC_VLAN_DOMAIN_MAX, SOC_PB_PP_EG_AC_VLAN_DOMAIN_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_AC_VBP_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_AC_CEP_PORT_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_AC_CEP_PORT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan_domain, SOC_PB_PP_EG_AC_VLAN_DOMAIN_MAX, SOC_PB_PP_EG_AC_VLAN_DOMAIN_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cvid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pep_edit_profile, SOC_PB_PP_EG_AC_PEP_EDIT_PROFILE_MAX, SOC_PB_PP_EG_AC_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_AC_CEP_PORT_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_VLAN_EDIT_CEP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_CEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cvid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->up, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp_profile, SOC_PB_PP_EG_AC_PCP_PROFILE_MAX, SOC_PB_PP_EG_AC_PCP_PROFILE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->edit_profile, SOC_PB_PP_EG_AC_EDIT_PROFILE_MAX, SOC_PB_PP_EG_AC_EDIT_PROFILE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_VLAN_EDIT_CEP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_VLAN_EDIT_VLAN_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_VLAN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_VLAN_EDIT_VLAN_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_AC_VLAN_EDIT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_AC_VLAN_EDIT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < 2; ++ind)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_VLAN_EDIT_VLAN_INFO, &(info->vlan_tags[ind]), 10, exit);
  }

  /*
   *	In Soc_petra-B nof_tags is always 2
   */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_tags, SOC_PB_PP_EG_AC_NOF_TAGS_MAX, SOC_PB_PP_EG_AC_NOF_TAGS_MAX, SOC_PB_PP_EG_AC_NOF_TAGS_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->edit_profile, SOC_PB_PP_EG_AC_EDIT_PROFILE_MAX, SOC_PB_PP_EG_AC_EDIT_PROFILE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp_profile, SOC_PB_PP_EG_AC_PCP_PROFILE_MAX, SOC_PB_PP_EG_AC_PCP_PROFILE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_AC_VLAN_EDIT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_AC_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_AC_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_AC_VLAN_EDIT_INFO, &(info->edit_info), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_AC_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_AC_MP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mp_level, SOC_PB_PP_MP_LEVEL_MAX, SOC_PB_PP_MP_LEVEL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_AC_MP_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

