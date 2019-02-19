/* $Id: pb_pp_vsi.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP_/src/soc_pb_pp_vsi.c
*
* MODULE PREFIX:  soc_pb_pp_pp
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
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_vsi.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_svem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_trap_mgmt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_VSI_DEFAULT_FORWARD_PROFILE_MAX                  (SOC_SAND_U32_MAX)
#define SOC_PB_PP_VSI_STP_TOPOLOGY_ID_MAX                          (SOC_SAND_U32_MAX)
#define SOC_PB_PP_VSI_FID_PROFILE_ID_MAX                           (6)
#define SOC_PB_PP_VSI_MAC_LEARN_PROFILE_ID_MAX                     (8)
#define SOC_PB_PP_VSI_ORIENTATION_MAX                              (SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1)
#define SOC_PB_PP_VSI_DA_TYPE_MAX                                  (SOC_SAND_PP_NOF_ETHERNET_DA_TYPES-1)

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
  Soc_pb_pp_procedure_desc_element_vsi[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_MAP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_MAP_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_MAP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_SYS_TO_LOCAL_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_MAP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_MAP_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_MAP_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_MAP_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_MAP_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_VSI_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_vsi[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_VSI_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VSI_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_VSI_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VSI_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'default_forward_profile' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR",
    "The parameter 'stp_topology_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'fid_profile_id' is out of range. \n\r "
    "The range is: 0 - 6/ or SOC_PPD_VSI_FID_IS_VSID.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'mac_learn_profile_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_VSI_ORIENTATION_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VSI_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_VSI_DA_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VSI_DA_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'da_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_DA_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_VSI_TRAP_CODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VSI_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'action_profile.trap_code' is out of range. \n\r "
    "The range is: SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_0 - SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_VSI_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_VSI_OUT_OF_RANGE_ERR",
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
  soc_pb_pp_vsi_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    fld_val,
    indx;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_PP_IHP_VSI_ISID_TBL_DATA
    ihp_vsi_isid_tbl_data;
  SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA
    vsi_fid_class_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   * disable mapping system vsi to FID, as learning is according to FID - at ingress
   */
  regs = soc_pb_pp_regs();

  fld_val = 0x0;
  SOC_PB_PP_FLD_SET(regs->ihp.large_em_event_fifo_configuration_reg.large_em_event_fifo_access_vsi_db,fld_val,10,exit);

 /*
  * disable mapping system vsi to FID, as learning is according to FID - at egress
  */
  fld_val = 0x0;
  SOC_PB_PP_FLD_SET(regs->egq.bypass_system_vsi_em_reg.bypass_system_vsi_em,fld_val,20,exit);
  
 /*
  * init local to system VSI mapping :1 to 1 mapping
  */

  for (indx = 0; indx <= SOC_PB_PP_RIF_VSI_ID_MAX; ++indx)
  {
    ihp_vsi_isid_tbl_data.vsi_or_isid = indx;
    res = soc_pb_pp_ihp_vsi_isid_tbl_set_unsafe(
            unit,
            indx,
            &ihp_vsi_isid_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }


  res = soc_pb_pp_ihp_vsi_fid_class_tbl_get_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &vsi_fid_class_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);

  vsi_fid_class_tbl_data.fid_class[0] = 0x7;
  vsi_fid_class_tbl_data.fid_class[1] = 0x7;
  vsi_fid_class_tbl_data.fid_class[2] = 0x7;
  vsi_fid_class_tbl_data.fid_class[3] = 0x7;
  
  res = soc_pb_pp_ihp_vsi_fid_class_tbl_set_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &vsi_fid_class_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, 1);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Map VSI to sys-VSI and system VSI to egress VSI
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_vsi_map_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          sys_vsid,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              eg_local_vsid,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VSI_ISID_TBL_DATA
    vsi_isid_tbl_data;
  SOC_PB_PP_SVEM_ACCESS_KEY
    svem_key;
  uint32
    buffer[1];
#ifndef USING_CHIP_SIM
  SOC_PB_REGS
    *regs = soc_pb_regs();
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_MAP_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

  *success = TRUE;

  res = soc_pb_pp_ihp_vsi_isid_tbl_get_unsafe(
          unit,
          local_vsi_ndx,
          &vsi_isid_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  vsi_isid_tbl_data.vsi_or_isid = sys_vsid;
  res = soc_pb_pp_ihp_vsi_isid_tbl_set_unsafe(
          unit,
          local_vsi_ndx,
          &vsi_isid_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  svem_key.key[0] = sys_vsid;
  *buffer = eg_local_vsid;
  res = soc_pb_pp_svem_access_entry_add_unsafe(
          unit,
          &svem_key,
          buffer,
          sizeof(buffer),
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifndef USING_CHIP_SIM
  do
  {
    SOC_PB_IMPLICIT_FLD_GET(regs->egq.interrupt_reg.svem_management_completed, *buffer, 10, exit);
  } while (*buffer == 0);
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_map_add_unsafe()", local_vsi_ndx, 0);
}


/*********************************************************************
*     Map VSI to sys-VSI and system VSI to egress VSI
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_vsi_sys_to_local_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          sys_vsid,
    SOC_SAND_OUT  SOC_PB_PP_VSI_ID                             *eg_local_vsid,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_SVEM_ACCESS_KEY
    svem_key;
  uint32
    buffer=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_SYS_TO_LOCAL_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(found);
  SOC_SAND_CHECK_NULL_INPUT(eg_local_vsid);

  *found = FALSE;


  svem_key.key[0] = sys_vsid;
  
  res = soc_pb_pp_svem_entry_get_unsafe(
          unit,
          &svem_key,
          &buffer,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*found)
  {
    *eg_local_vsid = buffer;
  }
  else
  {
    *eg_local_vsid = sys_vsid;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_map_get_unsafe()", sys_vsid, 0);
}

uint32
  soc_pb_pp_vsi_map_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          sys_vsid,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              eg_local_vsid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_MAP_ADD_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_vsi_ndx, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(sys_vsid, SOC_PB_PP_SYS_VSI_ID_MAX, SOC_PB_PP_SYS_VSI_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(eg_local_vsid, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_map_add_verify()", local_vsi_ndx, 0);
}

/*********************************************************************
*     Remove mapping of local VSI to system VSI, and system
 *     VSI to local VSI.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_vsi_map_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          sys_vsid
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VSI_ISID_TBL_DATA
    vsi_isid_tbl_data;
  SOC_PB_PP_SVEM_ACCESS_KEY
    svem_key;
  uint32
    buffer;
  SOC_PB_PP_REGS
    *regs = soc_pb_pp_regs();
  SOC_SAND_SUCCESS_FAILURE
    success = SOC_SAND_SUCCESS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_MAP_REMOVE_UNSAFE);

  res = soc_pb_pp_ihp_vsi_isid_tbl_get_unsafe(
          unit,
          local_vsi_ndx,
          &vsi_isid_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  vsi_isid_tbl_data.vsi_or_isid = sys_vsid;
  res = soc_pb_pp_ihp_vsi_isid_tbl_set_unsafe(
          unit,
          local_vsi_ndx,
          &vsi_isid_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_PP_REG_GET(regs->egq.bypass_system_vsi_em_reg, buffer, 10, exit);
  if (buffer)
  {
    svem_key.key[0] = sys_vsid;
    res = soc_pb_pp_svem_access_entry_remove_unsafe(
            unit,
            &svem_key,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_map_remove_unsafe()", local_vsi_ndx, 0);
}

uint32
  soc_pb_pp_vsi_map_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          sys_vsid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_MAP_REMOVE_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_vsi_ndx, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(sys_vsid, SOC_PB_PP_SYS_VSI_ID_MAX, SOC_PB_PP_SYS_VSI_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_map_remove_verify()", local_vsi_ndx, 0);
}

/*********************************************************************
*     Set the action profile (forwarding/snooping) to assign
 *     for packets upon failure lookup in the MACT (MACT
 *     default forwarding).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_vsi_default_frwrd_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA
    unknown_da_action_profiles_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dflt_frwrd_key);
  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  res = soc_pb_pp_ihb_unknown_da_action_profiles_tbl_get_unsafe(
          unit,
          SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ENTRY_OFFSET(dflt_frwrd_key),
          &unknown_da_action_profiles_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  unknown_da_action_profiles_tbl_data.fwd = action_profile->frwrd_action_strength;
  unknown_da_action_profiles_tbl_data.snp = action_profile->snoop_action_strength;
  unknown_da_action_profiles_tbl_data.cpu_trap_code_lsb = (action_profile->trap_code - SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_0);
  res = soc_pb_pp_ihb_unknown_da_action_profiles_tbl_set_unsafe(
          unit,
          SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ENTRY_OFFSET(dflt_frwrd_key),
          &unknown_da_action_profiles_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_default_frwrd_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_vsi_default_frwrd_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY, dflt_frwrd_key, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_ACTION_PROFILE, action_profile, 20, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_profile->trap_code, SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_0, SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_7, SOC_PB_PP_VSI_TRAP_CODE_OUT_OF_RANGE_ERR, 11, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_default_frwrd_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_vsi_default_frwrd_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY, dflt_frwrd_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_default_frwrd_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the action profile (forwarding/snooping) to assign
 *     for packets upon failure lookup in the MACT (MACT
 *     default forwarding).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_vsi_default_frwrd_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key,
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA
    unknown_da_action_profiles_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_DEFAULT_FRWRD_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dflt_frwrd_key);
  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_PB_PP_ACTION_PROFILE_clear(action_profile);

  res = soc_pb_pp_ihb_unknown_da_action_profiles_tbl_get_unsafe(
          unit,
          SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_ENTRY_OFFSET(dflt_frwrd_key),
          &unknown_da_action_profiles_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  action_profile->frwrd_action_strength = unknown_da_action_profiles_tbl_data.fwd;
  action_profile->snoop_action_strength = unknown_da_action_profiles_tbl_data.snp;
  action_profile->trap_code = (unknown_da_action_profiles_tbl_data.cpu_trap_code_lsb + SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_0);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_default_frwrd_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the Virtual Switch Instance information. After
 *     setting the VSI, the user may attach L2 Logical
 *     Interfaces to it: ACs; PWEs
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_vsi_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_VSI_INFO                            *vsi_info
  )
{
  uint32
    fid_val;
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_DATA
    vsi_da_not_found_tbl_data;
  SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA
    vsi_general_cfg_tbl_data;
  SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA
    vsi_fid_class_tbl_data;
  SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_DATA
    vsi_topology_id_tbl_data;
  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_DATA
    mact_fid_counter_db_tbl_data;
  SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA
    fid_class_2_fid_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  res = soc_pb_pp_ihp_vsi_da_not_found_tbl_get_unsafe(
          unit,
          vsi_ndx / 4,
          &vsi_da_not_found_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  vsi_da_not_found_tbl_data.profile_index[vsi_ndx % 4] = vsi_info->default_forward_profile;
  res = soc_pb_pp_ihp_vsi_da_not_found_tbl_set_unsafe(
          unit,
          vsi_ndx / 4,
          &vsi_da_not_found_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (vsi_ndx <= SOC_PB_PP_RIF_VSI_ID_MAX)
  {
    res = soc_pb_pp_ihp_vsi_my_mac_tbl_get_unsafe(
            unit,
            vsi_ndx,
            &vsi_general_cfg_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    vsi_general_cfg_tbl_data.enable_my_mac = vsi_info->enable_my_mac;
    res = soc_pb_pp_ihp_vsi_my_mac_tbl_set_unsafe(
            unit,
            vsi_ndx,
            &vsi_general_cfg_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* set FID class */
    res = soc_pb_pp_ihp_vsi_fid_class_tbl_get_unsafe(
            unit,
            vsi_ndx / 4,
            &vsi_fid_class_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    vsi_fid_class_tbl_data.fid_class[vsi_ndx % 4] = (vsi_info->fid_profile_id != SOC_PB_PP_VSI_FID_IS_VSID) ? vsi_info->fid_profile_id : 0x7;
    res = soc_pb_pp_ihp_vsi_fid_class_tbl_set_unsafe(
            unit,
            vsi_ndx / 4,
            &vsi_fid_class_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   /* map VSI to FID */
    if (vsi_info->fid_profile_id == SOC_PB_PP_VSI_FID_IS_VSID)
    {
      fid_val = vsi_ndx;
    }
    else
    {
      res = soc_pb_pp_ihp_fid_class_2_fid_tbl_get_unsafe(
              unit,
              vsi_info->fid_profile_id,
              &fid_class_2_fid_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      fid_val = fid_class_2_fid_tbl_data.fid;
    }


    res = soc_pb_pp_ihp_large_em_fid_counter_db_tbl_get_unsafe(
            unit,
            fid_val,
            &mact_fid_counter_db_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    mact_fid_counter_db_tbl_data.profile_pointer = vsi_info->mac_learn_profile_id;
    res = soc_pb_pp_ihp_large_em_fid_counter_db_tbl_set_unsafe(
            unit,
            fid_val,
            &mact_fid_counter_db_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_pb_pp_ihp_vsi_topology_id_tbl_get_unsafe(
            unit,
            vsi_ndx,
            &vsi_topology_id_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    vsi_topology_id_tbl_data.vsi_topology_id = vsi_info->stp_topology_id;
    res = soc_pb_pp_ihp_vsi_topology_id_tbl_set_unsafe(
            unit,
            vsi_ndx,
            &vsi_topology_id_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  { /* for VSI > 4K, FID = VSI, yet can assign FID profile for MAC limit and VSI learning handling */
      fid_val = vsi_ndx;
      res = soc_pb_pp_ihp_large_em_fid_counter_db_tbl_get_unsafe(
              unit,
              fid_val,
              &mact_fid_counter_db_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      mact_fid_counter_db_tbl_data.profile_pointer = vsi_info->mac_learn_profile_id;
      res = soc_pb_pp_ihp_large_em_fid_counter_db_tbl_set_unsafe(
              unit,
              fid_val,
              &mact_fid_counter_db_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_info_set_unsafe()", vsi_ndx, 0);
}

uint32
  soc_pb_pp_vsi_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_VSI_INFO                            *vsi_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_VSI_INFO, vsi_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_info_set_verify()", vsi_ndx, 0);
}

uint32
  soc_pb_pp_vsi_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsi_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_INFO_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_info_get_verify()", vsi_ndx, 0);
}

/*********************************************************************
*     Set the Virtual Switch Instance information. After
 *     setting the VSI, the user may attach L2 Logical
 *     Interfaces to it: ACs; PWEs
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_PB_PP_VSI_INFO                            *vsi_info
  )
{
  SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_DATA
    vsi_da_not_found_tbl_data;
  SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA
    vsi_general_cfg_tbl_data;
  SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA
    vsi_fid_class_tbl_data;
  SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_DATA
    vsi_topology_id_tbl_data;
  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_DATA
    mact_fid_counter_db_tbl_data;
  SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA
    fid_class_2_fid_tbl_data;
  uint32
    fid_val;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_VSI_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  SOC_PB_PP_VSI_INFO_clear(vsi_info);

  res = soc_pb_pp_ihp_vsi_da_not_found_tbl_get_unsafe(
          unit,
          vsi_ndx / 4,
          &vsi_da_not_found_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  vsi_info->default_forward_profile = vsi_da_not_found_tbl_data.profile_index[vsi_ndx % 4];

  if (vsi_ndx <= SOC_PB_PP_RIF_VSI_ID_MAX)
  {
    res = soc_pb_pp_ihp_vsi_my_mac_tbl_get_unsafe(
            unit,
            vsi_ndx,
            &vsi_general_cfg_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    vsi_info->enable_my_mac = SOC_SAND_NUM2BOOL(vsi_general_cfg_tbl_data.enable_my_mac);

    res = soc_pb_pp_ihp_vsi_fid_class_tbl_get_unsafe(
            unit,
            vsi_ndx / 4,
            &vsi_fid_class_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    vsi_info->fid_profile_id = (vsi_fid_class_tbl_data.fid_class[vsi_ndx % 4] != 0x7) ? vsi_fid_class_tbl_data.fid_class[vsi_ndx % 4] : SOC_PB_PP_VSI_FID_IS_VSID;

    /* map VSI to FID */
    if (vsi_info->fid_profile_id == SOC_PB_PP_VSI_FID_IS_VSID)
    {
      fid_val = vsi_ndx;
    }
    else
    {
      res = soc_pb_pp_ihp_fid_class_2_fid_tbl_get_unsafe(
              unit,
              vsi_info->fid_profile_id,
              &fid_class_2_fid_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      fid_val = fid_class_2_fid_tbl_data.fid;
    }

    res = soc_pb_pp_ihp_large_em_fid_counter_db_tbl_get_unsafe(
            unit,
            fid_val,
            &mact_fid_counter_db_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    vsi_info->mac_learn_profile_id = mact_fid_counter_db_tbl_data.profile_pointer;

    res = soc_pb_pp_ihp_vsi_topology_id_tbl_get_unsafe(
            unit,
            vsi_ndx,
            &vsi_topology_id_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    vsi_info->stp_topology_id = vsi_topology_id_tbl_data.vsi_topology_id;
  }

  else{ /* VSI > 4K */
    vsi_info->fid_profile_id = SOC_PB_PP_VSI_FID_IS_VSID;

    /* vsi = FID */
    res = soc_pb_pp_ihp_large_em_fid_counter_db_tbl_get_unsafe(
            unit,
            vsi_ndx,
            &mact_fid_counter_db_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    vsi_info->mac_learn_profile_id = mact_fid_counter_db_tbl_data.profile_pointer;

  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_vsi_info_get_unsafe()", vsi_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
*     soc_pb_pp_api_vsi module.
*     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_vsi_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_vsi;
}

/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_vsi module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_vsi_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_vsi;
}

uint32
  SOC_PB_PP_VSI_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_VSI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_forward_profile, SOC_PB_PP_VSI_DEFAULT_FORWARD_PROFILE_MAX, SOC_PB_PP_VSI_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->stp_topology_id, SOC_PB_PP_VSI_STP_TOPOLOGY_ID_MAX, SOC_PB_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR, 12, exit);
  if (info->fid_profile_id != SOC_PB_PP_VSI_FID_IS_VSID)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->fid_profile_id, SOC_PB_PP_VSI_FID_PROFILE_ID_MAX, SOC_PB_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR, 14, exit);
  }
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mac_learn_profile_id, SOC_PB_PP_VSI_MAC_LEARN_PROFILE_ID_MAX, SOC_PB_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_VSI_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, SOC_PB_PP_VSI_ORIENTATION_MAX, SOC_PB_PP_VSI_ORIENTATION_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_type, SOC_PB_PP_VSI_DA_TYPE_MAX, SOC_PB_PP_VSI_DA_TYPE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_forward_profile, SOC_PB_PP_VSI_DEFAULT_FORWARD_PROFILE_MAX, SOC_PB_PP_VSI_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_VSI_DEFAULT_FRWRD_KEY_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

