/* $Id: pb_pp_eg_vlan_edit.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_eg_vlan_edit.c
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

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_vlan_edit.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX                 (7)
#define SOC_PB_PP_EG_VLAN_EDIT_KEY_MAPPING_MAX                     (SOC_PB_PP_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEYS-1)
#define SOC_PB_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_MAX                (3)
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_FORMAT_MAX                      (SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1)
#define SOC_PB_PP_EG_VLAN_EDIT_EDIT_PROFILE_MAX                    (15)
#define SOC_PB_PP_EG_VLAN_EDIT_TPID_INDEX_MAX                      (SOC_SAND_U32_MAX)
#define SOC_PB_PP_EG_VLAN_EDIT_VID_SOURCE_MAX                      (SOC_PB_PP_NOF_EG_VLAN_EDIT_TAG_VID_SRCS-1)
#define SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_MAX                  (SOC_PB_PP_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_SRCS-1)
#define SOC_PB_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_MAX                  (2)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* Convert pcp-dei to/from field value { */
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_NONE 0
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_SRC_OUTER_TAG 1
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_SRC_INNER_TAG 2
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_SRC_MAP 3

#define SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, src_suffix) \
  case SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_##src_suffix: \
  fld_val = SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_##src_suffix; \
  break;

#define SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(src, fld_val) \
  switch (src) \
  { \
  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, NONE) \
  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, SRC_OUTER_TAG) \
  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, SRC_INNER_TAG) \
  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD_CASE(fld_val, SRC_MAP) \
    default: \
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_OUT_OF_RANGE_ERR, 555, exit); \
  }

#define SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, src_suffix) \
  case SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_##src_suffix: \
  src = SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_##src_suffix; \
  break;

#define SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(fld_val, src) \
  switch (fld_val) \
  { \
  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(fld_val, NONE) \
  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, SRC_OUTER_TAG) \
  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, SRC_INNER_TAG) \
  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC_CASE(src, SRC_MAP) \
  default: \
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_OUT_OF_RANGE_ERR, 556, exit); \
  }
/* } */

/* Convert vid to/from field value { */
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_EMPTY 0
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_OUTER_TAG 1
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_INNER_TAG 2
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_EDIT_INFO_0 3
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_EDIT_INFO_1 4
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_ENCAP_INFO 5

#define SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, src_suffix) \
  case SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_##src_suffix: \
  fld_val = SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_##src_suffix; \
  break;

#define SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD(src, fld_val) \
  switch (src) \
  { \
  SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, EMPTY) \
  SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, OUTER_TAG) \
  SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, INNER_TAG) \
  SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, EDIT_INFO_0) \
  SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, EDIT_INFO_1) \
  SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD_CASE(fld_val, ENCAP_INFO) \
  default: \
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_OUT_OF_RANGE_ERR, 557, exit); \
  }

#define SOC_PB_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, src_suffix) \
  case SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_##src_suffix: \
  src = SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_##src_suffix; \
  break;

#define SOC_PB_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC(fld_val, src) \
  switch (fld_val) \
  { \
  SOC_PB_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, EMPTY) \
  SOC_PB_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, OUTER_TAG) \
  SOC_PB_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, INNER_TAG) \
  SOC_PB_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, EDIT_INFO_0) \
  SOC_PB_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, EDIT_INFO_1) \
  SOC_PB_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC_CASE(src, ENCAP_INFO) \
  default: \
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_OUT_OF_RANGE_ERR, 558, exit); \
  }

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
  Soc_pb_pp_procedure_desc_element_eg_vlan_edit[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_TBL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_TBL_GET),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_eg_vlan_edit[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_EG_VLAN_EDIT_TRANSMIT_OUTER_TAG_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_TRANSMIT_OUTER_TAG_OUT_OF_RANGE_ERR",
    "The parameter 'transmit_outer_tag' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_VLAN_EDIT_KEY_MAPPING_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_KEY_MAPPING_OUT_OF_RANGE_ERR",
    "The parameter 'key_mapping' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEYS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'pep_edit_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR",
    "The parameter 'tag_format' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'edit_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_VLAN_EDIT_TPID_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_TPID_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR",
    "The parameter 'vid_source' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_VLAN_EDIT_TAG_VID_SRCS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR",
    "The parameter 'pcp_dei_source' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_SRCS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR",
    "The parameter 'tags_to_remove' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_OUT_OF_RANGE_ERR",
    "pcp-dei source is out of range. \n\r "
    "Allowed values are inner-tag, outer-tag or src-map (see \n\r"
    "SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_SRC).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*{
    SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_OUT_OF_RANGE_ERR",
    "Vid source is out of range. \n\r "
    "Allowed values are  (see \n\r"
    "SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_SRC).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },*/

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
  soc_pb_pp_eg_vlan_edit_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA
    epni_egress_edit_cmd_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* Init egress edit command table, since default value (for pcp_dei_source)
     are invalid */
  epni_egress_edit_cmd_tbl_data.inner_pcp_dei_source =
    SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_SRC_MAP;
  epni_egress_edit_cmd_tbl_data.inner_tpid_index = 0;
  epni_egress_edit_cmd_tbl_data.inner_vid_source =
    SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_EMPTY;

  epni_egress_edit_cmd_tbl_data.outer_pcp_dei_source =
    SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_FLD_VAL_SRC_MAP;
  epni_egress_edit_cmd_tbl_data.outer_tpid_index = 0;
  epni_egress_edit_cmd_tbl_data.outer_vid_source =
    SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_FLD_VAL_EMPTY;

  epni_egress_edit_cmd_tbl_data.tags_to_remove = 0;

  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
  res = soc_pb_pp_epni_egress_edit_cmd_tbl_set_unsafe(
          unit,
          0,
          &epni_egress_edit_cmd_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets the editing information for PEP port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pep_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                                 *pep_key,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO                   *pep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(pep_info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pep_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_vlan_edit_pep_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                                 *pep_key,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO                   *pep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_PEP_KEY, pep_key, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO, pep_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pep_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_eg_vlan_edit_pep_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                                 *pep_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_PEP_KEY, pep_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pep_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets the editing information for PEP port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pep_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                                 *pep_key,
    SOC_SAND_OUT SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO                   *pep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(pep_info);

  SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_clear(pep_info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pep_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the Edit command to perform over packets according
 *     to VLAN tags format and Edit profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_command_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY                *command_key,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO               *command_info
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_key);
  SOC_SAND_CHECK_NULL_INPUT(command_info);

  entry_offset =
    SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_INDX(command_key->tag_format, command_key->edit_profile);

  tbl_data.tags_to_remove = command_info->tags_to_remove;
  tbl_data.inner_tpid_index = command_info->inner_tag.tpid_index;
  tbl_data.outer_tpid_index = command_info->outer_tag.tpid_index;

  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(
    command_info->inner_tag.pcp_dei_source,
    tbl_data.inner_pcp_dei_source
    );

  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SRC_TO_FLD(
    command_info->outer_tag.pcp_dei_source,
    tbl_data.outer_pcp_dei_source
    );

  SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD(
    command_info->inner_tag.vid_source,
    tbl_data.inner_vid_source
    );

  SOC_PB_PP_EG_VLAN_EDIT_VID_SRC_TO_FLD(
    command_info->outer_tag.vid_source,
    tbl_data.outer_vid_source
    );

  res = soc_pb_pp_epni_egress_edit_cmd_tbl_set_unsafe(
    unit,
    entry_offset,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_command_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_eg_vlan_edit_command_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY                *command_key,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO               *command_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY, command_key, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO, command_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_command_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_eg_vlan_edit_command_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY                *command_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY, command_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_command_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the Edit command to perform over packets according
 *     to VLAN tags format and Edit profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_command_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY                *command_key,
    SOC_SAND_OUT SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO               *command_info
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(command_key);
  SOC_SAND_CHECK_NULL_INPUT(command_info);

  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_clear(command_info);

  entry_offset =
    SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_INDX(command_key->tag_format, command_key->edit_profile);

  res = soc_pb_pp_epni_egress_edit_cmd_tbl_get_unsafe(
    unit,
    entry_offset,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  command_info->tags_to_remove = (uint8)tbl_data.tags_to_remove;
  command_info->inner_tag.tpid_index = tbl_data.inner_tpid_index;
  command_info->outer_tag.tpid_index = tbl_data.outer_tpid_index;

  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(
    tbl_data.inner_pcp_dei_source,
    command_info->inner_tag.pcp_dei_source
    );

  SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_FLD_TO_SRC(
    tbl_data.outer_pcp_dei_source,
    command_info->outer_tag.pcp_dei_source
    );

  SOC_PB_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC(
    tbl_data.inner_vid_source,
    command_info->inner_tag.vid_source
    );

  SOC_PB_PP_EG_VLAN_EDIT_VID_FLD_TO_SRC(
    tbl_data.outer_vid_source,
    command_info->outer_tag.vid_source
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_command_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     For port x VLAN, set whether to transmit packets from a
 *     specific port tagged or not, according to the outer-tag
 *     VID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint8                                 transmit_outer_tag
  )
{
  uint32
    res = SOC_SAND_OK,
    bit_val;
  SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_UNSAFE);

  res = soc_pb_pp_epni_tx_tag_table_tbl_get_unsafe(
          unit,
          vid_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  bit_val = SOC_SAND_BOOL2NUM(transmit_outer_tag);
  res = soc_sand_bitstream_set_any_field(&bit_val, local_port_ndx, 1, (uint32*)&tbl_data.entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_epni_tx_tag_table_tbl_set_unsafe(
          unit,
          vid_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_unsafe()", local_port_ndx, vid_ndx);
}

uint32
  soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint8                                 transmit_outer_tag
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_VERIFY);

  res = soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_verify(
    unit,
    local_port_ndx,
    vid_ndx
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_verify()", local_port_ndx, vid_ndx);
}

uint32
  soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid_ndx, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_verify()", local_port_ndx, vid_ndx);
}

/*********************************************************************
*     For port x VLAN, set whether to transmit packets from a
 *     specific port tagged or not, according to the outer-tag
 *     VID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint8                                 *transmit_outer_tag
  )
{
  uint32
    res = SOC_SAND_OK,
    bit_val = 0;
  SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(transmit_outer_tag);

  res = soc_pb_pp_epni_tx_tag_table_tbl_get_unsafe(
          unit,
          vid_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field((uint32*)&tbl_data.entry, local_port_ndx, 1, &bit_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *transmit_outer_tag = SOC_SAND_NUM2BOOL(bit_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_unsafe()", local_port_ndx, vid_ndx);
}

/*********************************************************************
*     For PCP profile set the key used for mapping. When the
 *     Edit Command is set to determine the PCP-DEI value
 *     according to mapping, then the key used for mapping may
 *     be PCP-DEI, UP or DP and TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_profile_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY        key_mapping
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    bit_val;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_UNSAFE);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->epni.pcp_dei_profile_use_tc_dp_reg.pcp_dei_profile_use_tc_dp, fld_val, 10, exit);
  
  bit_val = SOC_SAND_BOOL2NUM(key_mapping == SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_TC_DP);
  
  res = soc_sand_bitstream_set(&fld_val, pcp_profile_ndx, bit_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  SOC_PB_PP_FLD_SET(regs->epni.pcp_dei_profile_use_tc_dp_reg.pcp_dei_profile_use_tc_dp, fld_val, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_profile_info_set_unsafe()", pcp_profile_ndx, 0);
}

uint32
  soc_pb_pp_eg_vlan_edit_pcp_profile_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY        key_mapping
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(key_mapping, SOC_PB_PP_EG_VLAN_EDIT_KEY_MAPPING_MAX, SOC_PB_PP_EG_VLAN_EDIT_KEY_MAPPING_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_profile_info_set_verify()", pcp_profile_ndx, 0);
}

uint32
  soc_pb_pp_eg_vlan_edit_pcp_profile_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_profile_info_get_verify()", pcp_profile_ndx, 0);
}

/*********************************************************************
*     For PCP profile set the key used for mapping. When the
 *     Edit Command is set to determine the PCP-DEI value
 *     according to mapping, then the key used for mapping may
 *     be PCP-DEI, UP or DP and TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_profile_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY        *key_mapping
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    bit_val;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(key_mapping);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->epni.pcp_dei_profile_use_tc_dp_reg.pcp_dei_profile_use_tc_dp, fld_val, 10, exit);

  bit_val = SOC_SAND_GET_BIT(fld_val, pcp_profile_ndx);

  *key_mapping = (bit_val ?
    SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_TC_DP :
    SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_PCP);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_profile_info_get_unsafe()", pcp_profile_ndx, 0);
}

STATIC uint32
  soc_pb_pp_eg_vlan_edit_pcp_dei_tbl_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      epni_entry_offset,
    SOC_SAND_IN  uint8                                       out_pcp,
    SOC_SAND_IN  uint8                                     out_dei
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_DATA
    epni_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_TBL_SET);

  epni_tbl_data.pcp_enc_table = SOC_PB_PP_PCP_DEI_TO_FLD_VAL(out_pcp, out_dei);

  res = soc_pb_pp_epni_pcp_dei_table_tbl_set_unsafe(
    unit,
    epni_entry_offset,
    &epni_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_dei_tbl_set_unsafe()", epni_entry_offset, epni_tbl_data.pcp_enc_table);
}

STATIC uint32
  soc_pb_pp_eg_vlan_edit_pcp_dei_tbl_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      epni_entry_offset,
    SOC_SAND_OUT uint8                                       *out_pcp,
    SOC_SAND_OUT uint8                                     *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_DATA
    epni_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_TBL_GET);

  res = soc_pb_pp_epni_pcp_dei_table_tbl_get_unsafe(
    unit,
    epni_entry_offset,
    &epni_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PB_PP_PCP_DEI_FROM_FLD_VAL(epni_tbl_data.pcp_enc_table, *out_pcp, *out_dei);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_dei_tbl_get_unsafe()", epni_entry_offset, 0);
}

/*********************************************************************
*     Set mapping from outer S-tag PCP and DEI to the PCP and
 *     DEI values to be set in the transmitted packet's tag.
 *     This is the mapping to be used when the incoming packet
 *     is S-tagged (outer Tag) and pcp profile set to use
 *     packet's attribute.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_stag_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_UNSAFE);

  res = soc_pb_pp_eg_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    SOC_PB_PP_EPNI_PCP_DEI_TBL_INDX_STAG(pcp_profile_ndx, tag_pcp_ndx, tag_dei_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_stag_set_unsafe()", pcp_profile_ndx, tag_pcp_ndx);
}

uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_stag_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_pcp_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_dei_ndx, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_stag_set_verify()", pcp_profile_ndx, tag_pcp_ndx);
}

uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_stag_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_pcp_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_dei_ndx, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_stag_get_verify()", pcp_profile_ndx, tag_pcp_ndx);
}

/*********************************************************************
*     Set mapping from outer S-tag PCP and DEI to the PCP and
 *     DEI values to be set in the transmitted packet's tag.
 *     This is the mapping to be used when the incoming packet
 *     is S-tagged (outer Tag) and pcp profile set to use
 *     packet's attribute.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_stag_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = soc_pb_pp_eg_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    SOC_PB_PP_EPNI_PCP_DEI_TBL_INDX_STAG(pcp_profile_ndx, tag_pcp_ndx, tag_dei_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_stag_get_unsafe()", pcp_profile_ndx, tag_pcp_ndx);
}

/*********************************************************************
*     Set mapping from outer C-tag UP to the PCP and DEI
 *     values to be set in the transmitted packet's tag. This
 *     is the mapping to be used when the incoming packet is
 *     C-tagged (outer Tag) and pcp profile set to use packet's
 *     attribute.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_ctag_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_UNSAFE);

  res = soc_pb_pp_eg_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    SOC_PB_PP_EPNI_PCP_DEI_TBL_INDX_CTAG(pcp_profile_ndx, tag_up_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_ctag_set_unsafe()", pcp_profile_ndx, tag_up_ndx);
}

uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_ctag_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  uint8                                       tag_up_ndx,
    SOC_SAND_IN  uint8                                       out_pcp,
    SOC_SAND_IN  uint8                                     out_dei
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_up_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_ctag_set_verify()", pcp_profile_ndx, tag_up_ndx);
}

uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_ctag_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_up_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_ctag_get_verify()", pcp_profile_ndx, tag_up_ndx);
}

/*********************************************************************
*     Set mapping from outer C-tag UP to the PCP and DEI
 *     values to be set in the transmitted packet's tag. This
 *     is the mapping to be used when the incoming packet is
 *     C-tagged (outer Tag) and pcp profile set to use packet's
 *     attribute.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_ctag_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = soc_pb_pp_eg_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    SOC_PB_PP_EPNI_PCP_DEI_TBL_INDX_CTAG(pcp_profile_ndx, tag_up_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_ctag_get_unsafe()", pcp_profile_ndx, tag_up_ndx);
}

/*********************************************************************
*     Set mapping from COS parameters (DP and TC) to the PCP
 *     and DEI values to be set in the transmitted packet's
 *     tag. This is the mapping to be used when the incoming
 *     packet has no tags or pcp profile is set to use TC and
 *     DP for the mapping.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_untagged_set_unsafe(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_UNSAFE);

  res = soc_pb_pp_eg_vlan_edit_pcp_dei_tbl_set_unsafe(
    unit,
    SOC_PB_PP_EPNI_PCP_DEI_TBL_INDX_UNTAGGED(pcp_profile_ndx, tc_ndx, dp_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_untagged_set_unsafe()", pcp_profile_ndx, tc_ndx);
}

uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_untagged_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_pcp, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(out_dei, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_untagged_set_verify()", pcp_profile_ndx, tc_ndx);
}

uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_untagged_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_profile_ndx, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_MAX, SOC_PB_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_untagged_get_verify()", pcp_profile_ndx, tc_ndx);
}

/*********************************************************************
*     Set mapping from COS parameters (DP and TC) to the PCP
 *     and DEI values to be set in the transmitted packet's
 *     tag. This is the mapping to be used when the incoming
 *     packet has no tags or pcp profile is set to use TC and
 *     DP for the mapping.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_untagged_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = soc_pb_pp_eg_vlan_edit_pcp_dei_tbl_get_unsafe(
    unit,
    SOC_PB_PP_EPNI_PCP_DEI_TBL_INDX_UNTAGGED(pcp_profile_ndx, tc_ndx, dp_ndx),
    out_pcp,
    out_dei
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_vlan_edit_pcp_map_untagged_get_unsafe()", pcp_profile_ndx, tc_ndx);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_eg_vlan_edit module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_eg_vlan_edit_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_eg_vlan_edit;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_eg_vlan_edit module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_eg_vlan_edit_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_eg_vlan_edit;
}

uint32
  SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->pvid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->up, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pep_edit_profile, SOC_PB_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_MAX, SOC_PB_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tag_format, SOC_PB_PP_EG_VLAN_EDIT_TAG_FORMAT_MAX, SOC_PB_PP_EG_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->edit_profile, SOC_PB_PP_EG_VLAN_EDIT_EDIT_PROFILE_MAX, SOC_PB_PP_EG_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_index, SOC_PB_PP_EG_VLAN_EDIT_TPID_INDEX_MAX, SOC_PB_PP_EG_VLAN_EDIT_TPID_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid_source, SOC_PB_PP_EG_VLAN_EDIT_VID_SOURCE_MAX, SOC_PB_PP_EG_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pcp_dei_source, SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_MAX, SOC_PB_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tags_to_remove, SOC_PB_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_MAX, SOC_PB_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO, &(info->inner_tag), 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO, &(info->outer_tag), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

