/* $Id: pb_pp_lif_cos.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP_/src/soc_pb_pp_lif_cos.c
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
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_cos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LIF_COS_COS_PROFILE_NDX_MIN                      (1)
#define SOC_PB_PP_LIF_COS_COS_PROFILE_NDX_MAX                      (15)
#define SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MIN                          (1)
#define SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MAX                          (15)
#define SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN                           (0)
#define SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX                           (2)
#define SOC_PB_PP_LIF_COS_OPCODE_TYPE_MAX                          (SOC_PB_PP_NOF_LIF_COS_OPCODE_TYPES-1)
#define SOC_PB_PP_LIF_COS_TAG_TYPE_NDX_MAX                         (3)
#define SOC_PB_PP_LIF_COS_TYPE_MAX                                 (SOC_PB_PP_NOF_LIF_COS_AC_PROFILE_TYPES-1)
#define SOC_PB_PP_LIF_COS_MAP_TABLE_MIN                            (1)
#define SOC_PB_PP_LIF_COS_MAP_TABLE_MAX                            (15)
#define SOC_PB_PP_LIF_COS_OUTER_TPID_MAX                           (3)
#define SOC_PB_PP_LIF_COS_AC_OFFSET_MAX                            (SOC_SAND_U8_MAX)

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
  Soc_pb_pp_procedure_desc_element_lif_cos[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TYPES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TYPES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TYPES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TYPES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TYPES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TYPES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TYPES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TYPES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_GET_UNSAFE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_lif_cos[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LIF_COS_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'profile_ndx' is out of range. \n\r "
    "The range is: 1 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_COS_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_COS_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'cos_profile_ndx' is out of range. \n\r "
    "The range is: 1 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'map_tbl_ndx' is out of range. \n\r "
    "The range is: 1 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'opcode_ndx' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_OPCODE_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_OPCODE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'opcode_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LIF_COS_OPCODE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_TAG_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_TAG_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'tag_type_ndx' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LIF_COS_AC_PROFILE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_MAP_TABLE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_MAP_TABLE_OUT_OF_RANGE_ERR",
    "The parameter 'map_table' is out of range. \n\r "
    "The range is: 1 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_IP_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_IP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'ip_type' is out of range. \n\r "
    "Must be equal to SOC_SAND_PP_IP_TYPE_IPV4/6.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_OUTER_TPID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_OUTER_TPID_OUT_OF_RANGE_ERR",
    "The parameter 'outer_tpid' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_AC_OFFSET_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_AC_OFFSET_OUT_OF_RANGE_ERR",
    "The parameter 'ac_offset' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_LIF_MAP_WHEN_IP_MPLS_MISMATCH_ERR,
    "SOC_PB_PP_LIF_MAP_WHEN_IP_MPLS_MISMATCH_ERR",
    "'map_when_ip' and 'map_when_mpls' should have the same value for Soc_petra-B.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_MAP_FROM_TC_DP_DISABLED_ERR,
    "SOC_PB_PP_LIF_COS_MAP_FROM_TC_DP_DISABLED_ERR",
    "Mapping from tc & dp is disabled. To enable, use soc_pb_pp_lif_cos_profile_info_set.\n\r",
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
  soc_pb_pp_lif_cos_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets COS information of AC COS Profile including (FORCE
 *     to const values, map field from the packet and select
 *     mapping table)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_ac_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_AC_PROFILE_INFO             *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_ac_profile_info_set_unsafe()", profile_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_ac_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_AC_PROFILE_INFO             *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_ac_profile_info_set_verify()", profile_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_ac_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_ac_profile_info_get_verify()", profile_ndx, 0);
}

/*********************************************************************
*     Sets COS information of AC COS Profile including (FORCE
 *     to const values, map field from the packet and select
 *     mapping table)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_ac_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_AC_PROFILE_INFO             *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_ac_profile_info_get_unsafe()", profile_ndx, 0);
}

/*********************************************************************
*     Sets COS Profile information (FORCE to const values, map
 *     field from the packet and select mapping table)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_pwe_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO            *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_pwe_profile_info_set_unsafe()", profile_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_pwe_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO            *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_pwe_profile_info_set_verify()", profile_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_pwe_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_pwe_profile_info_get_verify()", profile_ndx, 0);
}

/*********************************************************************
*     Sets COS Profile information (FORCE to const values, map
 *     field from the packet and select mapping table)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_pwe_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO            *profile_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_pwe_profile_info_get_unsafe()", profile_ndx, 0);
}

/*********************************************************************
*     Set CoS Profile attributes. Incoming LIFs are mapped to
 *     CoS Profile. CoS Profile '0' defined to keep the previous
 *     settings. The other CoS profiles are configured by this
 *     function.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  cos_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_INFO                *profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  SOC_PB_PP_REGS
    *regs = soc_pb_pp_regs();
  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY
    map_key;
  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY
    map_value;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  if (!profile_info->map_from_tc_dp)
  {
    /* No mapping from tc_dp, so we fill the tabled with the forced values */

    SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&map_key);
    SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&map_value);

    map_key.tc = 0;
    map_key.dp = 0;
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, (SOC_SAND_PP_TC_MAX + 1) * (SOC_SAND_PP_DP_MAX + 1));

    map_value.tc = profile_info->forced_tc;
    map_value.dp = profile_info->forced_dp;

    res = soc_pb_pp_lif_cos_profile_map_tc_dp_info_set_unsafe(
            unit,
            cos_profile_ndx,
            &map_key,
            &map_value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  res = soc_pb_pp_sw_db_lif_cos_map_from_tc_dp_set(unit, profile_info->map_from_tc_dp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Use L2 */
  SOC_PB_PP_FLD_GET(regs->ihp.cos_profile_usage_reg.cos_profile_use_l2, fld_val, 20, exit);
  SOC_SAND_SET_BIT(fld_val, SOC_SAND_BOOL2NUM(profile_info->map_when_l2), cos_profile_ndx);
  SOC_PB_PP_FLD_SET(regs->ihp.cos_profile_usage_reg.cos_profile_use_l2, fld_val, 30, exit);

  /* Assumption: profile_info->map_when_mpls was verified to be equal to profile_info->map_when_ip */
  /* Use L3 */
  SOC_PB_PP_FLD_GET(regs->ihp.cos_profile_usage_reg.cos_profile_use_l3, fld_val, 10, exit);
  SOC_SAND_SET_BIT(fld_val, SOC_SAND_BOOL2NUM(profile_info->map_when_ip), cos_profile_ndx);
  SOC_PB_PP_FLD_SET(regs->ihp.cos_profile_usage_reg.cos_profile_use_l3, fld_val, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_info_set_unsafe()", cos_profile_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  cos_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_INFO                *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(cos_profile_ndx, SOC_PB_PP_LIF_COS_COS_PROFILE_NDX_MIN, SOC_PB_PP_LIF_COS_COS_PROFILE_NDX_MAX, SOC_PB_PP_LIF_COS_COS_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_INFO, profile_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_info_set_verify()", cos_profile_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  cos_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(cos_profile_ndx, SOC_PB_PP_LIF_COS_COS_PROFILE_NDX_MIN, SOC_PB_PP_LIF_COS_COS_PROFILE_NDX_MAX, SOC_PB_PP_LIF_COS_COS_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_info_get_verify()", cos_profile_ndx, 0);
}

/*********************************************************************
*     Set CoS Profile attributes. Incoming LIFs are mapped to
 *     CoS Profile. CoS Profile '0' defined to keep the previous
 *     settings. The other CoS profiles are configured by this
 *     function.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  cos_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_PROFILE_INFO                *profile_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    bit_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY
    map_key;
  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY
    map_value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PB_PP_LIF_COS_PROFILE_INFO_clear(profile_info);

  regs = soc_pb_pp_regs();

  res = soc_pb_pp_sw_db_lif_cos_map_from_tc_dp_get(unit, &profile_info->map_from_tc_dp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  if (!profile_info->map_from_tc_dp)
  {
    /* No mapping from tc_dp. Get first entry (since all entries are the same) */

    SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&map_key);
    SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&map_value);

    map_key.tc = 0;
    map_key.dp = 0;

    res = soc_pb_pp_lif_cos_profile_map_tc_dp_info_get_unsafe(
            unit,
            cos_profile_ndx,
            &map_key,
            &map_value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    profile_info->forced_tc = map_value.tc;
    profile_info->forced_dp = map_value.dp;
  }

  /* Use L2 */
  SOC_PB_PP_FLD_GET(regs->ihp.cos_profile_usage_reg.cos_profile_use_l2, fld_val, 10, exit);
  bit_val = SOC_SAND_GET_BIT(fld_val, cos_profile_ndx);
  profile_info->map_when_l2 = SOC_SAND_BOOL2NUM(bit_val);

  /* Use L3 */
  SOC_PB_PP_FLD_GET(regs->ihp.cos_profile_usage_reg.cos_profile_use_l3, fld_val, 20, exit);
  bit_val = SOC_SAND_GET_BIT(fld_val, cos_profile_ndx);
  profile_info->map_when_mpls = profile_info->map_when_ip = SOC_SAND_BOOL2NUM(bit_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_info_get_unsafe()", cos_profile_ndx, 0);
}


/*********************************************************************
 *     Set entry in tc-dp map table. In order to write multiple entries,
 *     use soc_pb_pp_reps_for_tbl_set_unsafe
 ********************************************************************/
STATIC uint32
  soc_pb_pp_lif_cos_profile_map_tbl_entry_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  uint32                                  key_lsb,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_value);

  entry_offset = SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_ENTRY_OFFSET(
                   map_tbl_ndx,
                   key_lsb
                 );

  tbl_data.traffic_class = (uint32)map_value->tc;
  tbl_data.drop_precedence = (uint32)map_value->dp;

  res = soc_pb_pp_ihp_tc_dp_map_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_tbl_entry_set_unsafe()", map_tbl_ndx, key_lsb);
}

/*********************************************************************
 *     Get entry from tc-dp map table
 ********************************************************************/
STATIC uint32
  soc_pb_pp_lif_cos_profile_map_tbl_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  uint32                                  key_lsb,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_value);

  entry_offset = SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_ENTRY_OFFSET(
                   map_tbl_ndx,
                   key_lsb
                 );

  res = soc_pb_pp_ihp_tc_dp_map_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  map_value->tc = (SOC_SAND_PP_TC)tbl_data.traffic_class;
  map_value->dp = (SOC_SAND_PP_DP)tbl_data.drop_precedence;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_tbl_entry_get_unsafe()", map_tbl_ndx, key_lsb);
}

/*********************************************************************
*     Set mapping from L2 VLAN Tag fields to DP and TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_profile_map_l2_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  key_lsb = SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2(
              map_key->outer_tpid,
              map_key->incoming_up,
              map_key->incoming_dei
            );

  res = soc_pb_pp_lif_cos_profile_map_tbl_entry_set_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_l2_info_set_unsafe()", map_tbl_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_profile_map_l2_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MIN, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MAX, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY, map_key, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY, map_value, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_l2_info_set_verify()", map_tbl_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_profile_map_l2_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MIN, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MAX, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY, map_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_l2_info_get_verify()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set mapping from L2 VLAN Tag fields to DP and TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_profile_map_l2_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(map_value);

  key_lsb = SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2(
              map_key->outer_tpid,
              map_key->incoming_up,
              map_key->incoming_dei
            );

  res = soc_pb_pp_lif_cos_profile_map_tbl_entry_get_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_l2_info_get_unsafe()", map_tbl_ndx, 0);
}


/*********************************************************************
*     Set mapping from IP header fields (TOS / DSCP) to DP and
 *     TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_profile_map_ip_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  /* Assumption: ip_type was verified to be only ipv4/6 */
  key_lsb = SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP(
              (map_key->ip_type == SOC_SAND_PP_IP_TYPE_IPV4 ?
               SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV4_PREFIX :
               SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV6_PREFIX),
              map_key->tos
            );

  res = soc_pb_pp_lif_cos_profile_map_tbl_entry_set_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_ip_info_set_unsafe()", map_tbl_ndx, 0);
}


uint32
  soc_pb_pp_lif_cos_profile_map_ip_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MIN, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MAX, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY, map_key, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY, map_value, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_ip_info_set_verify()", map_tbl_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_profile_map_ip_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MIN, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MAX, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY, map_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_ip_info_get_verify()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set mapping from IP header fields (TOS / DSCP) to DP and
 *     TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_profile_map_ip_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(map_value);

  /* Assumption: ip_type was verified to be only ipv4/6 */
  key_lsb = SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP(
              (map_key->ip_type == SOC_SAND_PP_IP_TYPE_IPV4 ?
               SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV4_PREFIX :
               SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV6_PREFIX),
              map_key->tos
            );

  res = soc_pb_pp_lif_cos_profile_map_tbl_entry_get_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_ip_info_get_unsafe()", map_tbl_ndx, 0);
}


/*********************************************************************
*     Set mapping from label fields (EXP) to DP and TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_profile_map_mpls_label_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  key_lsb = SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS(
              map_key->in_exp
            );

  res = soc_pb_pp_lif_cos_profile_map_tbl_entry_set_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_mpls_label_info_set_unsafe()", map_tbl_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_profile_map_mpls_label_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MIN, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MAX, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY, map_key, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY, map_value, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_mpls_label_info_set_verify()", map_tbl_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_profile_map_mpls_label_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MIN, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MAX, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY, map_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_mpls_label_info_get_verify()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set mapping from label fields (EXP) to DP and TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_profile_map_mpls_label_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(map_value);

  key_lsb = SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS(
              map_key->in_exp
            );

  res = soc_pb_pp_lif_cos_profile_map_tbl_entry_get_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_mpls_label_info_get_unsafe()", map_tbl_ndx, 0);
}


/*********************************************************************
*     Set remapping from TC and DP to TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_profile_map_tc_dp_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  key_lsb = SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP(
              map_key->tc,
              map_key->dp
            );

  res = soc_pb_pp_lif_cos_profile_map_tbl_entry_set_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_tc_dp_info_set_unsafe()", map_tbl_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_profile_map_tc_dp_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    map_from_tc_dp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MIN, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MAX, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY, map_key, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY, map_value, 30, exit);

  res = soc_pb_pp_sw_db_lif_cos_map_from_tc_dp_get(unit, &map_from_tc_dp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (!map_from_tc_dp)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_COS_MAP_FROM_TC_DP_DISABLED_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_tc_dp_info_set_verify()", map_tbl_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_profile_map_tc_dp_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(map_tbl_ndx, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MIN, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_MAX, SOC_PB_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY, map_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_tc_dp_info_get_verify()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set remapping from TC and DP to TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_profile_map_tc_dp_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  map_tbl_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK,
    key_lsb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(map_value);

  key_lsb = SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP(
              map_key->tc,
              map_key->dp
            );

  res = soc_pb_pp_lif_cos_profile_map_tbl_entry_get_unsafe(
          unit,
          map_tbl_ndx,
          key_lsb,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_profile_map_tc_dp_info_get_unsafe()", map_tbl_ndx, 0);
}


/*********************************************************************
*     Set the Opcode type. Set the mapping of Class of Service
 *     attributes to the AC-Offset.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_opcode_types_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_TYPE                 opcode_type
  )
  {
  uint32
    res = SOC_SAND_OK;
  uint32
    use_l3,
    use_tc;
  SOC_PB_PP_REGS
    *regs = soc_pb_pp_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_TYPES_SET_UNSAFE);

  if (opcode_type & SOC_PB_PP_LIF_COS_OPCODE_TYPE_TC_DP)
  {
    use_tc = 1;
  }
  else
  {
    use_tc = 0;
  }
  SOC_PB_PP_FLD_SET(regs->ihp.sem_opcode_usage_reg.sem_opcode_use_tc_dp[opcode_ndx], use_tc, 10, exit);

  if (opcode_type & SOC_PB_PP_LIF_COS_OPCODE_TYPE_L3)
  {
    use_l3 = 1;
  }
  else
  {
    use_l3 = 0;
  }
  SOC_PB_PP_FLD_SET(regs->ihp.sem_opcode_usage_reg.sem_opcode_use_l3[opcode_ndx], use_l3, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_types_set_unsafe()", opcode_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_opcode_types_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_TYPE                 opcode_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_TYPES_SET_VERIFY);

/*
 * COVERITY
 *
 * SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(opcode_ndx, SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN, SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX, SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(opcode_type, SOC_PB_PP_LIF_COS_OPCODE_TYPE_MAX, SOC_PB_PP_LIF_COS_OPCODE_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  if (((opcode_type & SOC_PB_PP_LIF_COS_OPCODE_TYPE_L2) ^ (opcode_type & SOC_PB_PP_LIF_COS_OPCODE_TYPE_TC_DP)) == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_MAP_WHEN_IP_MPLS_MISMATCH_ERR, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_types_set_verify()", opcode_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_opcode_types_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_TYPES_GET_VERIFY);

/*
 * COVERITY
 *
 * SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(opcode_ndx, SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN, SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX, SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_types_get_verify()", opcode_ndx, 0);
}

/*********************************************************************
*     Set the Opcode type. Set the mapping of Class of Service
 *     attributes to the AC-Offset.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_opcode_types_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_OPCODE_TYPE                 *opcode_type
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    use_l3,
    use_tc;
  SOC_PB_PP_REGS
    *regs = soc_pb_pp_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_TYPES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(opcode_type);

  *opcode_type = SOC_PB_PP_LIF_COS_OPCODE_TYPE_L2;

  SOC_PB_PP_FLD_GET(regs->ihp.sem_opcode_usage_reg.sem_opcode_use_tc_dp[opcode_ndx], use_tc, 10, exit);
  *opcode_type = use_tc ? SOC_PB_PP_LIF_COS_OPCODE_TYPE_TC_DP : *opcode_type;

  SOC_PB_PP_FLD_GET(regs->ihp.sem_opcode_usage_reg.sem_opcode_use_l3[opcode_ndx], use_l3, 10, exit);
  *opcode_type |= use_l3 ? SOC_PB_PP_LIF_COS_OPCODE_TYPE_L3 : *opcode_type;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_types_get_unsafe()", opcode_ndx, 0);
}

/*********************************************************************
*     Set the Opcode according to IPv6 TOS field
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_opcode_ipv6_tos_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, ipv6_tos_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);
  data.ipv6_sem_offset = action_info->ac_offset;
  data.ipv6_opcode_valid = action_info->is_packet_valid;
  data.ipv6_add_offset_to_base = !action_info->is_qos_only;
  res = soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_set_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, ipv6_tos_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_ipv6_tos_map_set_unsafe()", opcode_ndx, ipv6_tos_ndx);
}

uint32
  soc_pb_pp_lif_cos_opcode_ipv6_tos_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_VERIFY);

/*
 * COVERITY
 *
 * SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(opcode_ndx, SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN, SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX, SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(ipv6_tos_ndx, SOC_SAND_PP_IPV6_TC_MAX, SOC_SAND_PP_IPV6_TC_OUT_OF_RANGE_ERR, 20, exit); */
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO, action_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_ipv6_tos_map_set_verify()", opcode_ndx, ipv6_tos_ndx);
}

uint32
  soc_pb_pp_lif_cos_opcode_ipv6_tos_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_VERIFY);

/*
 * COVERITY
 *
 * SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(opcode_ndx, SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN, SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX, SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(ipv6_tos_ndx, SOC_SAND_PP_IPV6_TC_MAX, SOC_SAND_PP_IPV6_TC_OUT_OF_RANGE_ERR, 20, exit); */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_ipv6_tos_map_get_verify()", opcode_ndx, ipv6_tos_ndx);
}

/*********************************************************************
*     Set the Opcode according to IPv6 TOS field
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_opcode_ipv6_tos_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO_clear(action_info);

  res = soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, ipv6_tos_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);
  action_info->ac_offset = (uint8)data.ipv6_sem_offset;
  action_info->is_packet_valid = SOC_SAND_NUM2BOOL(data.ipv6_opcode_valid);
  action_info->is_qos_only = SOC_SAND_NUM2BOOL_INVERSE(data.ipv6_add_offset_to_base != 0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_ipv6_tos_map_get_unsafe()", opcode_ndx, ipv6_tos_ndx);
}

/*********************************************************************
*     Set the Opcode according to IPv4 TOS field
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_opcode_ipv4_tos_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, ipv4_tos_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);
  data.ipv4_sem_offset = action_info->ac_offset;
  data.ipv4_opcode_valid = action_info->is_packet_valid;
  data.ipv4_add_offset_to_base = !action_info->is_qos_only;
  res = soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_set_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, ipv4_tos_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_ipv4_tos_map_set_unsafe()", opcode_ndx, ipv4_tos_ndx);
}

uint32
  soc_pb_pp_lif_cos_opcode_ipv4_tos_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_VERIFY);

/*
 * COVERITY
 *
 * SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(opcode_ndx, SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN, SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX, SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(ipv4_tos_ndx, SOC_SAND_PP_IPV4_TOS_MAX, SOC_SAND_PP_IPV4_TOS_OUT_OF_RANGE_ERR, 20, exit); */
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO, action_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_ipv4_tos_map_set_verify()", opcode_ndx, ipv4_tos_ndx);
}

uint32
  soc_pb_pp_lif_cos_opcode_ipv4_tos_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_VERIFY);

/*
 * COVERITY
 *
 * SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(opcode_ndx, SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN, SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX, SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_SAND_ERR_IF_ABOVE_MAX(ipv4_tos_ndx, SOC_SAND_PP_IPV4_TOS_MAX, SOC_SAND_PP_IPV4_TOS_OUT_OF_RANGE_ERR, 20, exit); */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_ipv4_tos_map_get_verify()", opcode_ndx, ipv4_tos_ndx);
}

/*********************************************************************
*     Set the Opcode according to IPv4 TOS field
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_opcode_ipv4_tos_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO_clear(action_info);

  res = soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, ipv4_tos_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);
  action_info->ac_offset = (uint8)data.ipv4_sem_offset;
  action_info->is_packet_valid = SOC_SAND_NUM2BOOL(data.ipv4_opcode_valid);
  action_info->is_qos_only = SOC_SAND_NUM2BOOL_INVERSE(data.ipv4_add_offset_to_base != 0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_ipv4_tos_map_get_unsafe()", opcode_ndx, ipv4_tos_ndx);
}

/*********************************************************************
*     Set the Opcode according to Traffic Class and Drop
 *     Precedence
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_opcode_tc_dp_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, tc_ndx, dp_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);
  data.tc_dp_sem_offset = action_info->ac_offset;
  data.tc_dp_opcode_valid = action_info->is_packet_valid;
  data.tc_dp_add_offset_to_base = !action_info->is_qos_only;
  res = soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_set_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, tc_ndx, dp_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_tc_dp_map_set_unsafe()", opcode_ndx, tc_ndx);
}

uint32
  soc_pb_pp_lif_cos_opcode_tc_dp_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_VERIFY);

/*
 * COVERITY
 *
 * SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(opcode_ndx, SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN, SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX, SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO, action_info, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_tc_dp_map_set_verify()", opcode_ndx, tc_ndx);
}

uint32
  soc_pb_pp_lif_cos_opcode_tc_dp_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_VERIFY);

/*
 * COVERITY
 *
 * SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(opcode_ndx, SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN, SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX, SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_tc_dp_map_get_verify()", opcode_ndx, tc_ndx);
}

/*********************************************************************
*     Set the Opcode according to Traffic Class and Drop
 *     Precedence
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_opcode_tc_dp_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO_clear(action_info);

  res = soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, tc_ndx, dp_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);
  action_info->ac_offset = (uint8)data.tc_dp_sem_offset;
  action_info->is_packet_valid = SOC_SAND_NUM2BOOL(data.tc_dp_opcode_valid);
  action_info->is_qos_only = SOC_SAND_NUM2BOOL_INVERSE(data.tc_dp_add_offset_to_base != 0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_tc_dp_map_get_unsafe()", opcode_ndx, tc_ndx);
}

/*********************************************************************
*     Set the Opcode according to VLAN Tag
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_opcode_vlan_tag_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, tag_type_ndx, pcp_ndx, dei_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);
  data.pcp_dei_sem_offset = action_info->ac_offset;
   data.pcp_dei_opcode_valid = action_info->is_packet_valid;
  data.pcp_dei_add_offset_to_base = !action_info->is_qos_only;
  res = soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_set_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, tag_type_ndx, pcp_ndx, dei_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_vlan_tag_map_set_unsafe()", opcode_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_opcode_vlan_tag_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_VERIFY);

/*
 * COVERITY
 *
 * SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(opcode_ndx, SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN, SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX, SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_type_ndx, SOC_PB_PP_LIF_COS_TAG_TYPE_NDX_MAX, SOC_PB_PP_LIF_COS_TAG_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dei_ndx, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 40, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO, action_info, 50, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_vlan_tag_map_set_verify()", opcode_ndx, 0);
}

uint32
  soc_pb_pp_lif_cos_opcode_vlan_tag_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_VERIFY);

/*
 * COVERITY
 *
 * SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(opcode_ndx, SOC_PB_PP_LIF_COS_OPCODE_NDX_MIN, SOC_PB_PP_LIF_COS_OPCODE_NDX_MAX, SOC_PB_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_type_ndx, SOC_PB_PP_LIF_COS_TAG_TYPE_NDX_MAX, SOC_PB_PP_LIF_COS_TAG_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pcp_ndx, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dei_ndx, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 40, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_vlan_tag_map_get_verify()", opcode_ndx, 0);
}

/*********************************************************************
*     Set the Opcode according to VLAN Tag
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_cos_opcode_vlan_tag_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO_clear(action_info);

  res = soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_KEY_ENTRY_OFFSET(opcode_ndx, tag_type_ndx, pcp_ndx, dei_ndx),
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);
  action_info->ac_offset = (uint8)data.pcp_dei_sem_offset;
  action_info->is_packet_valid = SOC_SAND_NUM2BOOL(data.pcp_dei_opcode_valid);
  action_info->is_qos_only = SOC_SAND_NUM2BOOL_INVERSE(data.pcp_dei_add_offset_to_base != 0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_cos_opcode_vlan_tag_map_get_unsafe()", opcode_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_lif_cos module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_lif_cos_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_lif_cos;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_lif_cos module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_lif_cos_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_lif_cos;
}

uint32
  SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_AC_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_LIF_COS_TYPE_MAX, SOC_PB_PP_LIF_COS_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->map_table, SOC_PB_PP_LIF_COS_MAP_TABLE_MIN, SOC_PB_PP_LIF_COS_MAP_TABLE_MAX, SOC_PB_PP_LIF_COS_MAP_TABLE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_COS_AC_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_LIF_COS_TYPE_MAX, SOC_PB_PP_LIF_COS_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->map_table, SOC_PB_PP_LIF_COS_MAP_TABLE_MAX, SOC_PB_PP_LIF_COS_MAP_TABLE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_COS_PWE_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_COS_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->forced_tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->forced_dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 15, exit);

  if (info->map_when_ip != info->map_when_mpls)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_MAP_WHEN_IP_MPLS_MISMATCH_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_COS_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if ((info->ip_type != SOC_SAND_PP_IP_TYPE_IPV4) &&
      (info->ip_type != SOC_SAND_PP_IP_TYPE_IPV6))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_COS_IP_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->tos, SOC_SAND_PP_IPV4_TOS_MAX, SOC_SAND_PP_IPV4_TOS_OUT_OF_RANGE_ERR, 11, exit); */

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->outer_tpid, SOC_PB_PP_LIF_COS_OUTER_TPID_MAX, SOC_PB_PP_LIF_COS_OUTER_TPID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->incoming_up, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->ac_offset, SOC_PB_PP_LIF_COS_AC_OFFSET_MAX, SOC_PB_PP_LIF_COS_AC_OFFSET_OUT_OF_RANGE_ERR, 12, exit); */

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

