/* $Id: pb_pp_lif_ing_vlan_edit.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_lif_ing_vlan_edit.c
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
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_ing_vlan_edit.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_parse.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_cos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_MAX                (63)
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX            (15)
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_MAX                 (SOC_PB_PP_NOF_LIF_ING_VLAN_EDIT_TAG_VID_SRCS-1)
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_MAX             (SOC_PB_PP_NOF_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRCS-1)
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_MAX                 (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_MAX               (7)
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_MAX             (2)
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_MAX               (3)


/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_NONE 0
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_EMPTY 0
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_NEW 1
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_AC_EDIT_INFO 1
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_OUTER_TAG 2
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_INNER_TAG 3

/* Convert pcp-dei to/from field value { */
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, src_suffix) \
  case SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_##src_suffix: \
    fld_val = SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_##src_suffix; \
    break;

#define SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(src, fld_val) \
  switch (src) \
  { \
    SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, NONE) \
    SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, OUTER_TAG) \
    SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, INNER_TAG) \
    SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, NEW) \
    default: \
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR, 555, exit); \
  }

#define SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, src_suffix) \
  case SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_##src_suffix: \
    src = SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_##src_suffix; \
    break;

#define SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(fld_val, src) \
  switch (fld_val) \
  { \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, NONE) \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, OUTER_TAG) \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, INNER_TAG) \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, NEW) \
  default: \
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR, 555, exit); \
  }
/* } */

/* Convert vid to/from field value { */
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, src_suffix) \
  case SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_VID_SRC_##src_suffix: \
    fld_val = SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_##src_suffix; \
    break;

#define SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD(src, fld_val) \
  switch (src) \
  { \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, EMPTY) \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, OUTER_TAG) \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, INNER_TAG) \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, AC_EDIT_INFO) \
  default: \
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR, 555, exit); \
  }

#define SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, src_suffix) \
  case SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_VID_SRC_FLD_VAL_##src_suffix: \
    src = SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_VID_SRC_##src_suffix; \
    break;

#define SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC(fld_val, src) \
  switch (fld_val) \
  { \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, EMPTY) \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, OUTER_TAG) \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, INNER_TAG) \
  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, AC_EDIT_INFO) \
  default: \
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR, 555, exit); \
  }
/* } */

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
  Soc_pb_pp_procedure_desc_element_lif_ing_vlan_edit[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_TBL_GET),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_lif_ing_vlan_edit[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR",
    "The parameter 'command_id' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'command_ndx' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR",
    "The parameter 'vid_source' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LIF_ING_VLAN_EDIT_TAG_VID_SRCS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_dei_source' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRCS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR",
    "The parameter 'tag_format' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'edit_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR",
    "The parameter 'tags_to_remove' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR",
    "pcp-dei/vid source is out of range. \n\r "
    "The range is: 0 to 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_INCONSISTANT_TBLS_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_INCONSISTANT_TBLS_ERR",
    "Relevant data should be the same in two tables. \n\r ",
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
  soc_pb_pp_lif_ing_vlan_edit_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    egq_ingress_vlan_edit_command_map_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_CLEAR(&egq_ingress_vlan_edit_command_map_tbl_data, SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA, 1);
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
  res = soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_set_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &egq_ingress_vlan_edit_command_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the Ingress VLAN Edit command to perform over
 *     packets according to incoming VLAN tags format and
 *     required VLAN edit profile (set according to AC).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_ing_vlan_edit_command_id_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_KEY           *command_key,
    SOC_SAND_IN  uint32                                      command_id
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA
    ingress_vlan_edit_command_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_key);

  entry_offset =
    SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_KEY_ENTRY_OFFSET(
      FALSE,
      command_key->tag_format,
      command_key->edit_profile);

  ingress_vlan_edit_command_table_tbl_data.ivec = command_id;

  res = soc_pb_pp_ihp_ingress_vlan_edit_command_table_tbl_set_unsafe(
    unit,
    entry_offset,
    &ingress_vlan_edit_command_table_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_command_id_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_lif_ing_vlan_edit_command_id_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_KEY           *command_key,
    SOC_SAND_IN  uint32                                      command_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_KEY, command_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(command_id, SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_command_id_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_lif_ing_vlan_edit_command_id_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_KEY           *command_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_KEY, command_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_command_id_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the Ingress VLAN Edit command to perform over
 *     packets according to incoming VLAN tags format and
 *     required VLAN edit profile (set according to AC).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_ing_vlan_edit_command_id_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_KEY           *command_key,
    SOC_SAND_OUT uint32                                      *command_id
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA
    ingress_vlan_edit_command_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_key);
  SOC_SAND_CHECK_NULL_INPUT(command_id);

  entry_offset =
    SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_KEY_ENTRY_OFFSET(
    FALSE,
    command_key->tag_format,
    command_key->edit_profile);

  res = soc_pb_pp_ihp_ingress_vlan_edit_command_table_tbl_get_unsafe(
    unit,
    entry_offset,
    &ingress_vlan_edit_command_table_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *command_id = ingress_vlan_edit_command_table_tbl_data.ivec;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_command_id_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the Edit command to perform over packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_ing_vlan_edit_command_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      command_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO          *command_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    epni_tbl_data;
  SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    egq_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_info);

  res = soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_get_unsafe(
          unit,
          command_ndx,
          &egq_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  epni_tbl_data.tpid_profile_link =
    command_info->tpid_profile;
  epni_tbl_data.inner_tpid_ndx =
    command_info->inner_tag.tpid_index;
  epni_tbl_data.outer_tpid_ndx =
    command_info->outer_tag.tpid_index;

  epni_tbl_data.edit_command_bytes_to_remove =
    command_info->tags_to_remove;

  SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(
    command_info->inner_tag.pcp_dei_source,
    epni_tbl_data.edit_command_inner_pcp_dei_source
    );
  
  SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(
    command_info->outer_tag.pcp_dei_source,
    epni_tbl_data.edit_command_outer_pcp_dei_source
    );
  
  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD(
    command_info->inner_tag.vid_source,
    epni_tbl_data.edit_command_inner_vid_source
    );
  
  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SRC_TO_FLD(
    command_info->outer_tag.vid_source,
    epni_tbl_data.edit_command_outer_vid_source
    );
  
  /* Copy same data to EGQ table */
  egq_tbl_data.edit_command_bytes_to_remove =
    epni_tbl_data.edit_command_bytes_to_remove;
  egq_tbl_data.edit_command_inner_pcp_dei_source =
    epni_tbl_data.edit_command_inner_pcp_dei_source;
  egq_tbl_data.edit_command_outer_pcp_dei_source =
    epni_tbl_data.edit_command_outer_pcp_dei_source;
  egq_tbl_data.edit_command_inner_vid_source =
    epni_tbl_data.edit_command_inner_vid_source;
  egq_tbl_data.edit_command_outer_vid_source =
    epni_tbl_data.edit_command_outer_vid_source;
  egq_tbl_data.tpid_profile_link =
    epni_tbl_data.tpid_profile_link;
  egq_tbl_data.inner_tpid_ndx=
    epni_tbl_data.inner_tpid_ndx;
  egq_tbl_data.outer_tpid_ndx =
    epni_tbl_data.outer_tpid_ndx;

  res = soc_pb_pp_epni_ingress_vlan_edit_command_map_tbl_set_unsafe(
          unit,
          command_ndx,
          &epni_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_set_unsafe(
          unit,
          command_ndx,
          &egq_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_command_info_set_unsafe()", command_ndx, 0);
}

uint32
  soc_pb_pp_lif_ing_vlan_edit_command_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      command_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO          *command_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(command_ndx, SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO, command_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_command_info_set_verify()", command_ndx, 0);
}

uint32
  soc_pb_pp_lif_ing_vlan_edit_command_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      command_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(command_ndx, SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_command_info_get_verify()", command_ndx, 0);
}

/*********************************************************************
*     Set the Edit command to perform over packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_ing_vlan_edit_command_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      command_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO          *command_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    epni_tbl_data;
  SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    egq_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_info);

  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(command_info);

  res = soc_pb_pp_epni_ingress_vlan_edit_command_map_tbl_get_unsafe(
    unit,
    command_ndx,
    &epni_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_get_unsafe(
    unit,
    command_ndx,
    &egq_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Validate that EGQ and EPN tables are the same */
  if ((egq_tbl_data.edit_command_bytes_to_remove != epni_tbl_data.edit_command_bytes_to_remove) ||
      (egq_tbl_data.edit_command_inner_pcp_dei_source != epni_tbl_data.edit_command_inner_pcp_dei_source) ||
      (egq_tbl_data.edit_command_outer_pcp_dei_source != epni_tbl_data.edit_command_outer_pcp_dei_source) ||
      (egq_tbl_data.edit_command_inner_vid_source != epni_tbl_data.edit_command_inner_vid_source) ||
      (egq_tbl_data.edit_command_outer_vid_source != epni_tbl_data.edit_command_outer_vid_source) ||
      (egq_tbl_data.tpid_profile_link != epni_tbl_data.tpid_profile_link) ||
      (egq_tbl_data.inner_tpid_ndx != epni_tbl_data.inner_tpid_ndx) ||
      (egq_tbl_data.outer_tpid_ndx != epni_tbl_data.outer_tpid_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_ING_VLAN_EDIT_INCONSISTANT_TBLS_ERR, 30, exit);
  }

  command_info->tpid_profile =
    epni_tbl_data.tpid_profile_link;
  command_info->inner_tag.tpid_index =
    epni_tbl_data.inner_tpid_ndx;
  command_info->outer_tag.tpid_index =
    epni_tbl_data.outer_tpid_ndx;

  command_info->tags_to_remove =
    (uint8)epni_tbl_data.edit_command_bytes_to_remove;

  SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(
    epni_tbl_data.edit_command_inner_pcp_dei_source,
    command_info->inner_tag.pcp_dei_source
    );

  SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(
    epni_tbl_data.edit_command_outer_pcp_dei_source,
    command_info->outer_tag.pcp_dei_source
    );

  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC(
    epni_tbl_data.edit_command_inner_vid_source,
    command_info->inner_tag.vid_source
    );

  SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_FLD_TO_SRC(
    epni_tbl_data.edit_command_outer_vid_source,
    command_info->outer_tag.vid_source
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_command_info_get_unsafe()", command_ndx, 0);
}

STATIC uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_dei_tbl_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      ihp_entry_offset,
    SOC_SAND_IN  uint8                                       out_pcp,
    SOC_SAND_IN  uint8                                     out_dei
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA
    ihp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_TBL_SET);

  ihp_tbl_data.pcp_dei = SOC_PB_PP_PCP_DEI_TO_FLD_VAL(out_pcp, out_dei);

  res = soc_pb_pp_ihp_vlan_edit_pcp_dei_map_tbl_set_unsafe(
    unit,
    ihp_entry_offset,
    &ihp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_dei_tbl_set_unsafe()", ihp_entry_offset, 0);
}

STATIC uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_dei_tbl_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      ihp_entry_offset,
    SOC_SAND_OUT uint8                                       *out_pcp,
    SOC_SAND_OUT uint8                                     *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA
    ihp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_TBL_GET);

  res = soc_pb_pp_ihp_vlan_edit_pcp_dei_map_tbl_get_unsafe(
    unit,
    ihp_entry_offset,
    &ihp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_PP_PCP_DEI_FROM_FLD_VAL(ihp_tbl_data.pcp_dei, *out_pcp, *out_dei);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_dei_tbl_get_unsafe()", ihp_entry_offset, 0);
}

/*********************************************************************
*     Set mapping from outer S-tag PCP and DEI to the PCP and
 *     DEI values to be set in the transmitted packet's tag.
 *     This is the mapping to be used when the incoming packet
 *     is S-tagged (outer Tag).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_stag_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  uint8                                       tag_pcp_ndx,
    SOC_SAND_IN  uint8                                     tag_dei_ndx,
    SOC_SAND_IN  uint8                                       out_pcp,
    SOC_SAND_IN  uint8                                     out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_UNSAFE);

  res = soc_pb_pp_lif_ing_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_STAG(pcp_profile_ndx, tag_pcp_ndx, tag_dei_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_stag_set_unsafe()", pcp_profile_ndx, tag_pcp_ndx);
}

uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_stag_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_pcp_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_dei_ndx, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_stag_set_verify()", pcp_profile_ndx, tag_pcp_ndx);
}

uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_stag_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_pcp_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_dei_ndx, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_stag_get_verify()", pcp_profile_ndx, tag_pcp_ndx);
}

/*********************************************************************
*     Set mapping from outer S-tag PCP and DEI to the PCP and
 *     DEI values to be set in the transmitted packet's tag.
 *     This is the mapping to be used when the incoming packet
 *     is S-tagged (outer Tag).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_stag_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = soc_pb_pp_lif_ing_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_STAG(pcp_profile_ndx, tag_pcp_ndx, tag_dei_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_stag_get_unsafe()", pcp_profile_ndx, tag_pcp_ndx);
}

/*********************************************************************
*     Set mapping from outer C-tag UP to the PCP and DEI
 *     values to be set in the transmitted packet's tag. This
 *     is the mapping to be used when the incoming packet is
 *     C-tagged (outer Tag.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_ctag_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_UNSAFE);

  res = soc_pb_pp_lif_ing_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_CTAG(pcp_profile_ndx, tag_up_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_ctag_set_unsafe()", pcp_profile_ndx, tag_up_ndx);
}

uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_ctag_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_up_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_ctag_set_verify()", pcp_profile_ndx, tag_up_ndx);
}

uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_ctag_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_up_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_ctag_get_verify()", pcp_profile_ndx, tag_up_ndx);
}

/*********************************************************************
*     Set mapping from outer C-tag UP to the PCP and DEI
 *     values to be set in the transmitted packet's tag. This
 *     is the mapping to be used when the incoming packet is
 *     C-tagged (outer Tag.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_ctag_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = soc_pb_pp_lif_ing_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_CTAG(pcp_profile_ndx, tag_up_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_ctag_get_unsafe()", pcp_profile_ndx, tag_up_ndx);
}

/*********************************************************************
*     Set mapping from COS parameters (DP and TC) to the PCP
 *     and DEI values to be set in the transmitted packet's
 *     tag. This is the mapping to be used when the incoming
 *     packet has no tags.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_untagged_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_UNSAFE);

  res = soc_pb_pp_lif_ing_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_UNTAGGED(pcp_profile_ndx, tc_ndx, dp_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_untagged_set_unsafe()", pcp_profile_ndx, tc_ndx);
}

uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_untagged_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 50, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_untagged_set_verify()", pcp_profile_ndx, tc_ndx);
}

uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_untagged_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_untagged_get_verify()", pcp_profile_ndx, tc_ndx);
}

/*********************************************************************
*     Set mapping from COS parameters (DP and TC) to the PCP
 *     and DEI values to be set in the transmitted packet's
 *     tag. This is the mapping to be used when the incoming
 *     packet has no tags.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_ing_vlan_edit_pcp_map_untagged_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = soc_pb_pp_lif_ing_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_UNTAGGED(pcp_profile_ndx, tc_ndx, dp_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_ing_vlan_edit_pcp_map_untagged_get_unsafe()", pcp_profile_ndx, tc_ndx);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_lif_ing_vlan_edit module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_lif_ing_vlan_edit_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_lif_ing_vlan_edit;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_lif_ing_vlan_edit module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_lif_ing_vlan_edit_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_lif_ing_vlan_edit;
}

uint32
  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_index, SOC_PB_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_OUT_OF_RANGE_ERR, 9, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid_source, SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp_dei_source, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tag_format, SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->edit_profile, SOC_PB_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tags_to_remove, SOC_PB_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile, SOC_PB_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO, &(info->inner_tag), 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO, &(info->outer_tag), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_ING_VLAN_EDIT_COMMAND_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

