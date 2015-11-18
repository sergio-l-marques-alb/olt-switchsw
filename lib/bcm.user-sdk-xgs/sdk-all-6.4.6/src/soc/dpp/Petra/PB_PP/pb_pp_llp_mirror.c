/* $Id: pb_pp_llp_mirror.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_llp_mirror.c
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
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LLP_MIRROR_MIRROR_PROFILE_MAX                    (15)
#define SOC_PB_PP_LLP_MIRROR_TAGGED_DFLT_MAX                       (SOC_SAND_U32_MAX)
#define SOC_PB_PP_LLP_MIRROR_UNTAGGED_DFLT_MAX                     (SOC_SAND_U32_MAX)

#define SOC_PB_PP_LLP_MIRROR_TAGGED_NDX   (6)
#define SOC_PB_PP_LLP_MIRROR_UNTAGGED_NDX (7)

#define SOC_PB_PP_LLP_MIRROR_PROFILE_INVALID (0)

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
  Soc_pb_pp_procedure_desc_element_llp_mirror[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_MIRROR_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_llp_mirror[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LLP_MIRROR_MIRROR_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_MIRROR_MIRROR_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'mirror_profile' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_MIRROR_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_MIRROR_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_MIRROR_TAGGED_DFLT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_MIRROR_TAGGED_DFLT_OUT_OF_RANGE_ERR",
    "The parameter 'tagged_dflt' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_MIRROR_UNTAGGED_DFLT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_MIRROR_UNTAGGED_DFLT_OUT_OF_RANGE_ERR",
    "The parameter 'untagged_dflt' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

    {
      SOC_PB_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR,
      "SOC_PB_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR",
      "The parameter 'vid_ndx' is out of range. \n\r "
      "The range is: 1 - 4095.\n\r ",
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
  soc_pb_pp_llp_mirror_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_init_unsafe()", 0, 0);
}

STATIC void
  soc_pb_pp_llp_mirror_port_vlan_flds_get(
    SOC_SAND_OUT SOC_PETRA_REG_FIELD *vid_flds[SOC_PB_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES]
  )
{
  SOC_PB_PP_REGS
    *regs = soc_pb_pp_regs();

  /* array of vid regs, to make it easier to get field by internal_vid_ndx */
  vid_flds[0] = &(regs->ihp.ll_mirror_vid01_reg.ll_mirror_vid0);
  vid_flds[1] = &(regs->ihp.ll_mirror_vid01_reg.ll_mirror_vid1);
  vid_flds[2] = &(regs->ihp.ll_mirror_vid23_reg.ll_mirror_vid2);
  vid_flds[3] = &(regs->ihp.ll_mirror_vid23_reg.ll_mirror_vid3);
  vid_flds[4] = &(regs->ihp.ll_mirror_vid45_reg.ll_mirror_vid4);
  vid_flds[5] = &(regs->ihp.ll_mirror_vid45_reg.ll_mirror_vid5);
}

/*********************************************************************
*     Set a mirroring for port and VLAN, so all incoming
 *     packets enter from the given port and identified with
 *     the given VID will be associated with Mirror command
 *     (Enables mirroring (copying) the packets to additional
 *     destination.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_mirror_port_vlan_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                      mirror_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK,
    ll_mirror_profile_tbl_offset,
    internal_vid_ndx;
  SOC_PETRA_REG_FIELD
    *vid_flds[SOC_PB_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES];
  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ll_mirror_profile_tbl_data;
  uint8
    first_appear;
  SOC_SAND_SUCCESS_FAILURE
    multi_set_success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

  *success = SOC_SAND_SUCCESS;

  /* array of vid regs, to make it easier to get field by internal_vid_ndx */
  soc_pb_pp_llp_mirror_port_vlan_flds_get(vid_flds);

  /* Add new vid_dnx to db */
  res = soc_pb_sw_db_multiset_add(
    unit,
    SOC_PB_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,
    vid_ndx,
    &internal_vid_ndx,
    &first_appear,
    &multi_set_success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (multi_set_success != SOC_SAND_SUCCESS)
  {
    *success = multi_set_success;
    goto exit;
  }

  if (first_appear)
  {
    /* Add to HW */
    SOC_PB_PP_IMPLICIT_FLD_SET(*vid_flds[internal_vid_ndx], vid_ndx, 15, exit);
  }

  ll_mirror_profile_tbl_offset =
    SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(internal_vid_ndx, local_port_ndx);

  ll_mirror_profile_tbl_data.ll_mirror_profile = mirror_profile;

  res = soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
    unit,
    ll_mirror_profile_tbl_offset,
    &ll_mirror_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Add to SW */
  res = soc_pb_pp_sw_db_llp_mirror_port_vlan_is_exist_set(
          unit,
          local_port_ndx,
          internal_vid_ndx,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_port_vlan_add_unsafe()", local_port_ndx, vid_ndx);
}

uint32
  soc_pb_pp_llp_mirror_port_vlan_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                      mirror_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(mirror_profile, SOC_PB_PP_LLP_MIRROR_MIRROR_PROFILE_MAX, SOC_PB_PP_LLP_MIRROR_MIRROR_PROFILE_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    vid_ndx,
    1,
    SOC_SAND_PP_VLAN_ID_MAX,
    SOC_PB_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR,
    20,
    exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(mirror_profile, SOC_PB_PP_LLP_MIRROR_MIRROR_PROFILE_MAX, SOC_PB_PP_LLP_MIRROR_MIRROR_PROFILE_OUT_OF_RANGE_ERR, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_port_vlan_add_verify()", local_port_ndx, vid_ndx);
}

/*********************************************************************
*     Remove a mirroring for port and VLAN
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_mirror_port_vlan_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  uint32
    res = SOC_SAND_OK,
    ll_mirror_profile_tbl_offset,
    internal_vid_ndx,
    ref_count;
  SOC_PETRA_REG_FIELD
    *vid_flds[SOC_PB_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES];
  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ll_mirror_profile_tbl_data;
  uint8
    last_appear;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_REMOVE_UNSAFE);

  /* array of vid regs, to make it easier to get field by internal_vid_ndx */
  soc_pb_pp_llp_mirror_port_vlan_flds_get(vid_flds);

  /* Search vid_ndx in db */
  res = soc_pb_sw_db_multiset_lookup(
    unit,
    SOC_PB_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,
    vid_ndx,
    &internal_vid_ndx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count > 0)
  {
    /* Someone is using this vid_ndx. If this port has a valid mirror profile,
       remove it */

    ll_mirror_profile_tbl_offset =
      SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(internal_vid_ndx, local_port_ndx);

    res = soc_pb_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
      unit,
      ll_mirror_profile_tbl_offset,
      &ll_mirror_profile_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (ll_mirror_profile_tbl_data.ll_mirror_profile != SOC_PB_PP_LLP_MIRROR_PROFILE_INVALID)
    {
      /* Set table entry to invalid */
      ll_mirror_profile_tbl_data.ll_mirror_profile =
        SOC_PB_PP_LLP_MIRROR_PROFILE_INVALID;

      res = soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
        unit,
        ll_mirror_profile_tbl_offset,
        &ll_mirror_profile_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      /* Decrease count for this vid_ndx */
      res = soc_pb_sw_db_multiset_remove_by_index(
        unit,
        SOC_PB_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,
        internal_vid_ndx,
        &last_appear
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      /* Remove from SW */
      res = soc_pb_pp_sw_db_llp_mirror_port_vlan_is_exist_set(
              unit,
              local_port_ndx,
              internal_vid_ndx,
              FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      /* Enable for the removed port x vlan same profile as the default tagged packets. */
      ll_mirror_profile_tbl_offset =
        SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(SOC_PB_PP_LLP_MIRROR_TAGGED_NDX, local_port_ndx);

      res = soc_pb_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
        unit,
        ll_mirror_profile_tbl_offset,
        &ll_mirror_profile_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      ll_mirror_profile_tbl_offset =
        SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(internal_vid_ndx, local_port_ndx);

      res = soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
        unit,
        ll_mirror_profile_tbl_offset,
        &ll_mirror_profile_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (last_appear)
      {
        /* Remove from HW */
        SOC_PB_PP_IMPLICIT_FLD_SET(*vid_flds[internal_vid_ndx], 0, 15, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_port_vlan_remove_unsafe()", local_port_ndx, vid_ndx);
}

uint32
  soc_pb_pp_llp_mirror_port_vlan_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    vid_ndx,
    1,
    SOC_SAND_PP_VLAN_ID_MAX,
    SOC_PB_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR,
    20,
    exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_port_vlan_remove_verify()", local_port_ndx, vid_ndx);
}

/*********************************************************************
*     Get the assigned mirroring profile for port and VLAN.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_mirror_port_vlan_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint32                                      *mirror_profile
  )
{
  uint32
    res = SOC_SAND_OK,
    ll_mirror_profile_tbl_offset,
    internal_vid_ndx,
    ref_count;
  SOC_PETRA_REG_FIELD
    *vid_flds[SOC_PB_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES];
  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ll_mirror_profile_tbl_data;
  uint8
    is_port_vlan_exists;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mirror_profile);

  *mirror_profile = 0;

  /* array of vid regs, to make it easier to get field by internal_vid_ndx */
  soc_pb_pp_llp_mirror_port_vlan_flds_get(vid_flds);

  /* Search vid_ndx in db */
  res = soc_pb_sw_db_multiset_lookup(
    unit,
    SOC_PB_PP_SW_DB_MULTI_SET_LLP_MIRROR_PROFILE,
    vid_ndx,
    &internal_vid_ndx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* internal vid exists */
  if (ref_count != 0)
  {
    res = soc_pb_pp_sw_db_llp_mirror_port_vlan_is_exist_get(
            unit,
            local_port_ndx,
            internal_vid_ndx,
            &is_port_vlan_exists
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (is_port_vlan_exists == TRUE)
    {
      /* port x internal vid exists */
      ll_mirror_profile_tbl_offset =
        SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(internal_vid_ndx, local_port_ndx);

      res = soc_pb_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
              unit,
              ll_mirror_profile_tbl_offset,
              &ll_mirror_profile_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      *mirror_profile = ll_mirror_profile_tbl_data.ll_mirror_profile;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_port_vlan_get_unsafe()", local_port_ndx, vid_ndx);
}

uint32
  soc_pb_pp_llp_mirror_port_vlan_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_MIRROR_PORT_VLAN_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    vid_ndx,
    1,
    SOC_SAND_PP_VLAN_ID_MAX,
    SOC_PB_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR,
    20,
    exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_port_vlan_get_verify()", local_port_ndx, vid_ndx);
}

/*********************************************************************
*     Set default mirroring profiles for port
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_mirror_port_dflt_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                    local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO               *dflt_mirroring_info
  )
{
  uint32
    res = SOC_SAND_OK,
    ll_mirror_profile_tbl_offset,
    internal_vid_ndx;
  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ll_mirror_profile_tbl_data;
  uint8
    is_port_vlan_exist;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dflt_mirroring_info);

  /* Tagged */
  ll_mirror_profile_tbl_offset =
    SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(SOC_PB_PP_LLP_MIRROR_TAGGED_NDX, local_port_ndx);

  ll_mirror_profile_tbl_data.ll_mirror_profile = dflt_mirroring_info->tagged_dflt;

  res = soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
          unit,
          ll_mirror_profile_tbl_offset,
          &ll_mirror_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Enable for all port x internal_vids that are disable the same profile as default tagged. */
  for (internal_vid_ndx = 0; internal_vid_ndx < SOC_PB_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES; internal_vid_ndx++)
  {
    res = soc_pb_pp_sw_db_llp_mirror_port_vlan_is_exist_get(
            unit,
            local_port_ndx,
            internal_vid_ndx,
            &is_port_vlan_exist
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    if (is_port_vlan_exist == FALSE)
    {
      ll_mirror_profile_tbl_offset =
        SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(internal_vid_ndx, local_port_ndx);
      
      res = soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
        unit,
        ll_mirror_profile_tbl_offset,
        &ll_mirror_profile_tbl_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    }
  }
  
  /* Untagged */
  ll_mirror_profile_tbl_offset =
    SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(SOC_PB_PP_LLP_MIRROR_UNTAGGED_NDX, local_port_ndx);

  ll_mirror_profile_tbl_data.ll_mirror_profile = dflt_mirroring_info->untagged_dflt;

  res = soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
    unit,
    ll_mirror_profile_tbl_offset,
    &ll_mirror_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_port_dflt_set_unsafe()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_llp_mirror_port_dflt_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO               *dflt_mirroring_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO, dflt_mirroring_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_port_dflt_set_verify()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_llp_mirror_port_dflt_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_port_dflt_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Set default mirroring profiles for port
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_mirror_port_dflt_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO               *dflt_mirroring_info
  )
{
  uint32
    res = SOC_SAND_OK,
    ll_mirror_profile_tbl_offset;
  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ll_mirror_profile_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_MIRROR_PORT_DFLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dflt_mirroring_info);

  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO_clear(dflt_mirroring_info);

  /* Tagged */
  ll_mirror_profile_tbl_offset =
    SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(SOC_PB_PP_LLP_MIRROR_TAGGED_NDX, local_port_ndx);

  res = soc_pb_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
    unit,
    ll_mirror_profile_tbl_offset,
    &ll_mirror_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  dflt_mirroring_info->tagged_dflt = ll_mirror_profile_tbl_data.ll_mirror_profile;

  /* Untagged */
  ll_mirror_profile_tbl_offset =
    SOC_PB_PP_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(SOC_PB_PP_LLP_MIRROR_UNTAGGED_NDX, local_port_ndx);

  res = soc_pb_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
    unit,
    ll_mirror_profile_tbl_offset,
    &ll_mirror_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  dflt_mirroring_info->untagged_dflt = ll_mirror_profile_tbl_data.ll_mirror_profile;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_mirror_port_dflt_get_unsafe()", local_port_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_llp_mirror module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_llp_mirror_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_llp_mirror;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_llp_mirror module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_llp_mirror_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_llp_mirror;
}

uint32
  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tagged_dflt, SOC_PB_PP_LLP_MIRROR_TAGGED_DFLT_MAX, SOC_PB_PP_LLP_MIRROR_TAGGED_DFLT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->untagged_dflt, SOC_PB_PP_LLP_MIRROR_UNTAGGED_DFLT_MAX, SOC_PB_PP_LLP_MIRROR_UNTAGGED_DFLT_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

