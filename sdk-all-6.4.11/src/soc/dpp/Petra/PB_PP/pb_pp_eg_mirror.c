/* $Id: pb_pp_eg_mirror.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_eg_mirror.c
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
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define SOC_PB_PP_EG_MIRROR_VID_INDEX_INVALID (0)
#define SOC_PB_PP_EG_MIRROR_VID_DFLT_NDX      (7)

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
  Soc_pb_pp_procedure_desc_element_eg_mirror[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_VLAN_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_DFLT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_DFLT_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_DFLT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_DFLT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_DFLT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_DFLT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_DFLT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_PORT_DFLT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_MIRROR_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_eg_mirror[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR",
    "The parameter 'enable_mirror' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_MIRROR_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_MIRROR_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */



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


STATIC void
  soc_pb_pp_eg_mirror_enable_reg_param(
    SOC_SAND_IN uint32 internal_vid_ndx,
    SOC_SAND_IN uint32 port_ndx,
    SOC_SAND_OUT uint32 *reg_index,
    SOC_SAND_OUT uint32 *reg_offset
  )
{
  uint32
    enable_index = 0;

  enable_index |= (internal_vid_ndx << 6) | port_ndx;
  *reg_index  = enable_index / 32;
  *reg_offset = enable_index % 32;
}

STATIC void
  soc_pb_pp_eg_mirror_port_vlan_flds_get(
    SOC_SAND_OUT SOC_PETRA_REG_FIELD *vid_flds[SOC_PB_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES]
  )
{
  SOC_PB_PP_REGS
    *regs = soc_pb_pp_regs();

  /* array of vid regs, to make it easier to get field by internal_vid_ndx */
  vid_flds[0] = &(regs->epni.mirror_vid_reg0_reg.mirror_vid0);
  vid_flds[1] = &(regs->epni.mirror_vid_reg0_reg.mirror_vid1);
  vid_flds[2] = &(regs->epni.mirror_vid_reg1_reg.mirror_vid2);
  vid_flds[3] = &(regs->epni.mirror_vid_reg1_reg.mirror_vid3);
  vid_flds[4] = &(regs->epni.mirror_vid_reg2_reg.mirror_vid4);
  vid_flds[5] = &(regs->epni.mirror_vid_reg2_reg.mirror_vid5);
  vid_flds[6] = &(regs->epni.mirror_vid_reg3_reg.mirror_vid6);
}

uint32
  soc_pb_pp_eg_mirror_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* IMPLEMENTED */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Set outbound mirroring for out-port and VLAN, so all
 *     outgoing packets leave from the given port and with the
 *     given VID will be mirrored or not according to
 *     'enable_mirror'
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_mirror_port_vlan_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success,
    SOC_SAND_IN  dpp_outbound_mirror_config_t        *config

  )
{
  uint32
    res = SOC_SAND_OK,
    eq_mirror_enable_reg_index,
    eq_mirror_enable_reg_offset,
    fld_val,
    internal_vid_ndx;
  SOC_PETRA_REGS
    *regs = NULL;
  SOC_PETRA_REG_FIELD
    *vid_flds[SOC_PB_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES];
  uint8
    first_appear;
  SOC_SAND_SUCCESS_FAILURE
    multi_set_success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_MIRROR_PORT_VLAN_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

  regs = soc_petra_regs();

  *success = SOC_SAND_SUCCESS;

  /* array of vid regs, to make it easier to get field by internal_vid_ndx */
  soc_pb_pp_eg_mirror_port_vlan_flds_get(vid_flds);

  /* Add new vid_dnx to db */
  res = soc_pb_sw_db_multiset_add(
          unit,
          SOC_PB_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE,
          vid_ndx,
          &internal_vid_ndx,
          &first_appear,
          &multi_set_success
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

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

  /* Enable bit */
  soc_pb_pp_eg_mirror_enable_reg_param(
    internal_vid_ndx,
    out_port_ndx,
    &eq_mirror_enable_reg_index,
    &eq_mirror_enable_reg_offset
  );
   
  SOC_PB_FLD_GET(regs->epni.mirror_enable_reg[eq_mirror_enable_reg_index].mirror_enable , fld_val, 60, exit);
  SOC_SAND_SET_BIT(fld_val, config->mirror_command, eq_mirror_enable_reg_offset);
  SOC_PB_FLD_SET(regs->epni.mirror_enable_reg[eq_mirror_enable_reg_index].mirror_enable , fld_val, 70, exit);

  /* Add to SW */
  res = soc_pb_sw_db_outbound_mirror_port_vlan_is_exist_set(
          unit,
          out_port_ndx,
          internal_vid_ndx,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_port_vlan_add_unsafe()", out_port_ndx, vid_ndx);
}

uint32
  soc_pb_pp_eg_mirror_port_vlan_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  dpp_outbound_mirror_config_t        *config

  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_MIRROR_PORT_VLAN_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

  /* IMPLEMENTED */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_port_vlan_add_verify()", out_port_ndx, vid_ndx);
}

/*********************************************************************
*     Remove a mirroring for port and VLAN, upon this packet
 *     transmitted out this out_port_ndx and vid_ndx will be
 *     mirrored or not according to default configuration for
 *     out_port_ndx. see soc_ppd_eg_mirror_port_dflt_set()
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_mirror_port_vlan_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  uint32
    res = SOC_SAND_OK,
    eq_mirror_enable_reg_index,
    eq_mirror_enable_reg_offset,
    internal_vid_ndx,
    ref_count,
    fld_val,
    is_defult_port_mirror_enable;
  SOC_PETRA_REGS
    *regs = NULL;
  SOC_PETRA_REG_FIELD
    *vid_flds[SOC_PB_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES];
  uint8
    last_appear;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_MIRROR_PORT_VLAN_REMOVE_UNSAFE);

  regs = soc_petra_regs();

  /* array of vid regs, to make it easier to get field by internal_vid_ndx */
  soc_pb_pp_eg_mirror_port_vlan_flds_get(vid_flds);

  /* Search vid_ndx in db */
  res = soc_pb_sw_db_multiset_lookup(
    unit,
    SOC_PB_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE,
    vid_ndx,
    &internal_vid_ndx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count > 0)
  {
    /* Decrease count for this vid_ndx */
    res = soc_pb_sw_db_multiset_remove_by_index(
      unit,
      SOC_PB_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE,
      internal_vid_ndx,
      &last_appear
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (last_appear)
    {
      /* Remove from HW */
      SOC_PB_PP_IMPLICIT_FLD_SET(*vid_flds[internal_vid_ndx], SOC_PB_PP_EG_MIRROR_VID_INDEX_INVALID, 15, exit);
    }

    /* Remove port x internal vid from SW */
    res = soc_pb_sw_db_outbound_mirror_port_vlan_is_exist_set(
            unit,
            out_port_ndx,
            internal_vid_ndx,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    /* Enable/Disable mirror for the removed port x internal vid same as the default port. */
    soc_pb_pp_eg_mirror_enable_reg_param(
      SOC_PB_PP_EG_MIRROR_VID_DFLT_NDX,
      out_port_ndx,
      &eq_mirror_enable_reg_index,
      &eq_mirror_enable_reg_offset
      );

    SOC_PB_FLD_GET(regs->epni.mirror_enable_reg[eq_mirror_enable_reg_index].mirror_enable , fld_val, 60, exit);
    is_defult_port_mirror_enable = SOC_SAND_GET_BIT(fld_val, eq_mirror_enable_reg_offset);

    soc_pb_pp_eg_mirror_enable_reg_param(
      internal_vid_ndx,
      out_port_ndx,
      &eq_mirror_enable_reg_index,
      &eq_mirror_enable_reg_offset
    );

    SOC_PB_FLD_GET(regs->epni.mirror_enable_reg[eq_mirror_enable_reg_index].mirror_enable , fld_val, 60, exit);
    SOC_SAND_SET_BIT(fld_val, is_defult_port_mirror_enable, eq_mirror_enable_reg_offset);
    SOC_PB_FLD_SET(regs->epni.mirror_enable_reg[eq_mirror_enable_reg_index].mirror_enable , fld_val, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_port_vlan_remove_unsafe()", out_port_ndx, vid_ndx);
}

uint32
  soc_pb_pp_eg_mirror_port_vlan_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_MIRROR_PORT_VLAN_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

   /* IMPLEMENTED */

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_port_vlan_remove_verify()", out_port_ndx, vid_ndx);
}

/*********************************************************************
*     Get the assigned mirroring profile for port and VLAN.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_mirror_port_vlan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint8                                 *enable_mirror
  )
{
  uint32
    res = SOC_SAND_OK,
    eq_mirror_enable_reg_index,
    eq_mirror_enable_reg_offset,
    internal_vid_ndx,
    ref_count,
    fld_val;
  SOC_PETRA_REGS
    *regs = NULL;
  SOC_PETRA_REG_FIELD
    *vid_flds[SOC_PB_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES];
  uint8
    is_port_vlan_exists;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_MIRROR_PORT_VLAN_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable_mirror);

  regs = soc_petra_regs();

  *enable_mirror = FALSE;

  /* array of vid regs, to make it easier to get field by internal_vid_ndx */
  soc_pb_pp_eg_mirror_port_vlan_flds_get(vid_flds);

  /* Search vid_ndx in db */
  res = soc_pb_sw_db_multiset_lookup(
    unit,
    SOC_PB_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE,
    vid_ndx,
    &internal_vid_ndx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count != 0)
  {
    res = soc_pb_sw_db_outbound_mirror_port_vlan_is_exist_get(
            unit,
            out_port_ndx,
            internal_vid_ndx,
            &is_port_vlan_exists
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (is_port_vlan_exists == TRUE)
    {
      soc_pb_pp_eg_mirror_enable_reg_param(
        internal_vid_ndx,
        out_port_ndx,
        &eq_mirror_enable_reg_index,
        &eq_mirror_enable_reg_offset
      );

      SOC_PB_FLD_GET(regs->epni.mirror_enable_reg[eq_mirror_enable_reg_index].mirror_enable , fld_val, 60, exit);
      *enable_mirror = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BIT(fld_val, eq_mirror_enable_reg_offset));
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_port_vlan_get_unsafe()", out_port_ndx, vid_ndx);
}

uint32
  soc_pb_pp_eg_mirror_port_vlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_MIRROR_PORT_VLAN_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

  /* IMPLEMENTED */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_port_vlan_get_verify()", out_port_ndx, vid_ndx);
}

/*********************************************************************
*     Set default mirroring profiles for port
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_mirror_port_dflt_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_MIRROR_PORT_DFLT_INFO            *dflt_mirroring_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_MIRROR_PORT_DFLT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dflt_mirroring_info);
 
  res = soc_pb_ports_mirror_outbound_dflt_set_unsafe(
          unit,
          local_port_ndx,
          dflt_mirroring_info->enable_mirror
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_port_dflt_set_unsafe()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_eg_mirror_port_dflt_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_MIRROR_PORT_DFLT_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

   /* IMPLEMENTED */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_port_dflt_set_verify()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_eg_mirror_port_dflt_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_MIRROR_PORT_DFLT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

   /* IMPLEMENTED */
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_port_dflt_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Set default mirroring profiles for port
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_mirror_port_dflt_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT dpp_outbound_mirror_config_t            *config
  )
{
  uint32
    res = SOC_SAND_OK,
    eq_mirror_enable_reg_index,
    eq_mirror_enable_reg_offset,
    fld_val,
    internal_vid_ndx = SOC_PB_PP_EG_MIRROR_VID_DFLT_NDX;
  SOC_PETRA_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_MIRROR_PORT_DFLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(config);


  regs = soc_petra_regs();

  soc_pb_pp_eg_mirror_enable_reg_param(
    internal_vid_ndx,
    local_port_ndx,
    &eq_mirror_enable_reg_index,
    &eq_mirror_enable_reg_offset
  );

  SOC_PB_FLD_GET(regs->epni.mirror_enable_reg[eq_mirror_enable_reg_index].mirror_enable , fld_val, 60, exit);
  config->mirror_command = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BIT(fld_val, eq_mirror_enable_reg_offset));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_mirror_port_dflt_get_unsafe()", local_port_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_eg_mirror module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_eg_mirror_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_eg_mirror;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_eg_mirror module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_eg_mirror_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_eg_mirror;
}
uint32
  SOC_PB_PP_EG_MIRROR_PORT_DFLT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_MIRROR_PORT_DFLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_MIRROR_PORT_DFLT_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

