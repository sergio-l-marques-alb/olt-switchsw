/* $Id: pb_pp_llp_cos.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_llp_cos.c
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
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_cos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LLP_COS_MAPPING_TBL_NDX_MAX                      (SOC_PB_PP_NOF_LLP_COS_MAPPING_TABLES-1)
#define SOC_PB_PP_LLP_COS_TABLE_ID_NDX_MAX                         (SOC_PB_PP_NOF_LLP_COS_MAPPING_TABLES-1)
#define SOC_PB_PP_LLP_COS_RANGE_NDX_MAX                            (2)
#define SOC_PB_PP_LLP_COS_L4_PRTCL_TYPE_NDX_MAX                    (SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1)
#define SOC_PB_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_MAX             (SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1)
#define SOC_PB_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_MAX            (SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1)
#define SOC_PB_PP_LLP_COS_PROFILE_NDX_MAX                          (7)
#define SOC_PB_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_MAX             (3)
#define SOC_PB_PP_LLP_COS_TC_TO_UP_INDEX_MAX                       (3)
#define SOC_PB_PP_LLP_COS_UP_TO_DP_INDEX_MAX                       (1)
#define SOC_PB_PP_LLP_COS_TC_TO_DP_INDEX_MAX                       (0)
#define SOC_PB_PP_LLP_COS_IP_QOS_TO_TC_INDEX_MAX                   (1)
#define SOC_PB_PP_LLP_COS_VALUE1_MAX                               (7)
#define SOC_PB_PP_LLP_COS_VALUE2_MAX                               (7)

#define SOC_PB_PP_LLP_COS_IPV4_SUBNET_NDX_MAX (15)

#define SOC_PB_PP_LLP_COS_PCP_PROFILE_MAX    (3)
#define SOC_PB_PP_LLP_COS_TOS_INDEX_MAX      (1)
#define SOC_PB_PP_LLP_COS_UP_MAP_PROFILE_MAX (3)
#define SOC_PB_PP_LLP_COS_DP_PROFILE_MAX     (3)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* Map {PCP-profile(2b), Incoming. UP-PCP(3b)} to {DE(1b), TC(3b)} */
#define SOC_PB_PP_LLP_COS_PCP_DECODE_TBL_OFFSET(table_id_ndx, entry_ndx) \
  SOC_PB_PP_FLDS_TO_BUFF_2(table_id_ndx, 3, entry_ndx, 3)

/* {In-PP-Port. Incoming-UP-Map-Profile(2), Incoming-UP-PCP(3)} */
#define SOC_PB_PP_LLP_COS_INCOMING_UP_MAP_OFFSET(up_profile, up_pcp) \
  SOC_PB_PP_FLDS_TO_BUFF_2(up_profile, 2, up_pcp, 3)

/* {In-PP-Port. Cfg-Drop-Precedence-Profile(2), Incoming-UP-PCP (3)} */
#define SOC_PB_PP_LLP_COS_DROP_PRECEDENCE_MAP_PCP_OFFSET(dp_profile, up_pcp) \
  SOC_PB_PP_FLDS_TO_BUFF_2(dp_profile, 2, up_pcp, 3)

/* {Is-TCP(1b), Source-Port-Range(2b), Dest-Port-Range(2b)} */
#define SOC_PB_PP_LLP_COS_L4_PORT_RANGES_2_TC_TABLE_OFFSET(is_tcp, src_port_range, dest_port_range) \
  SOC_PB_PP_FLDS_TO_BUFF_3(is_tcp, 1, src_port_range, 2, dest_port_range, 2)


/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX_NONE = 0,
  SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX_MPLS = 8,
  SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX_MAC_IN_MAC = 9,
  SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX_ARP = 10,
  SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX_CFM = 11,
  SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX_IPV4 = 12,
  SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX_IPV6 = 13,
  SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX_FCOE = 14,
  SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX_TRILL = 15,
  SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX_USER_DEFINED = 0xff
} SOC_PB_PP_LLP_COS_L2_NEXT_PRTCL_NDX;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_llp_cos[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_ETHER_TYPE_PROCESS),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_llp_cos[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LLP_COS_MAPPING_TBL_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_MAPPING_TBL_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mapping_tbl_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LLP_COS_MAPPING_TABLES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_TABLE_ID_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_TABLE_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'table_id_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LLP_COS_MAPPING_TABLES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_ENTRY_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_ENTRY_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'entry_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LLP_COS_MAPPING_TABLES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_RANGE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_RANGE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'range_ndx' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_L4_PRTCL_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_L4_PRTCL_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'l4_prtcl_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'src_port_match_range_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'dest_port_match_range_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'profile_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'in_up_to_tc_and_de_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_TC_TO_UP_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_TC_TO_UP_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tc_to_up_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_UP_TO_DP_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_UP_TO_DP_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'up_to_dp_index' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_TC_TO_DP_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_TC_TO_DP_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tc_to_dp_index' is out of range. \n\r "
    "Must be 0 for Soc_petraB.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_IP_QOS_TO_TC_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_IP_QOS_TO_TC_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'ip_qos_to_tc_index' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_VALUE1_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_VALUE1_OUT_OF_RANGE_ERR",
    "The parameter 'value1' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_VALUE2_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_VALUE2_OUT_OF_RANGE_ERR",
    "The parameter 'value2' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_START_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_START_OUT_OF_RANGE_ERR",
    "The parameter 'start' is out of range. \n\r "
    "The range is: 0 - 65535.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_END_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_END_OUT_OF_RANGE_ERR",
    "The parameter 'end' is out of range. \n\r "
    "The range is: 0 - 65535.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR,
    "SOC_PB_PP_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR",
    "Table is not supported in Soc_petraB. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
   
  {
    SOC_PB_PP_LLP_COS_MAPPING_TBL_NDX_RANGE_MISMATCH_ERR,
    "SOC_PB_PP_LLP_COS_MAPPING_TBL_NDX_RANGE_MISMATCH_ERR",
    "Table index value is not in range for this table. \n\r "
    "Allowed ranges:\n\r "
    "up_to_de_tc table: 0 - 3\n\r "
    "ipv4_tos_to_dp_tc_valid table: 0 - 1\n\r "
    "ipv6_tc_to_dp_tc_valid table: 0 - 1\n\r "
    "incoming_up_to_up table: 0 - 3\n\r "
    "up_to_dp table: 0 - 3\n\r "
    "Other tables: 0 - 0\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_IPV4_SUBNET_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LLP_COS_IPV4_SUBNET_NDX_OUT_OF_RANGE_ERR",
    "'entry_ndx' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LLP_COS_ETHER_TYPE_NOT_EXIT_ERR,
    "SOC_PB_PP_LLP_COS_ETHER_TYPE_NOT_EXIT_ERR",
    "Ether-type is not one of the predefined ether-types, and was not. \n\r "
    "configured as a user-defined ether-type.\n\r ",
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
  soc_pb_pp_llp_cos_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LLP_COS_MAPPING_TABLE
    mapping_tbl_ndx;
  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO
    entry;
  uint32
    table_id_ndx = 0,
    entry_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&entry);

  /* Mapping UP to DE,TC */
  mapping_tbl_ndx = SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC;

  for (table_id_ndx=0; table_id_ndx<SOC_PB_PP_LLP_COS_NOF_UP_TO_TC_TABLES; ++table_id_ndx)
  {
    for (entry_ndx=0; entry_ndx<(SOC_PB_PP_LLP_COS_MAX_UP + 1); ++entry_ndx)
    {
      /*
       *  DE value
       *  Map UP 0-3 to DE 1
       *  Map UP 4-7 to DE 0
       */
      entry.value1 = SOC_PB_PP_LLP_COS_MAX_DE - entry_ndx/4;
      /*
       *  TC value
       *  UP i to TC i
       */
      entry.value2 = entry_ndx;
      
      res = soc_pb_pp_llp_cos_mapping_table_entry_set_unsafe(
        unit,
        mapping_tbl_ndx,
        table_id_ndx,
        entry_ndx,
        &entry
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

  /* Mapping UP to DP */
  mapping_tbl_ndx = SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DP;

  for (table_id_ndx=0; table_id_ndx<SOC_PB_PP_LLP_COS_NOF_UP_TO_DP_TABLES; ++table_id_ndx)
  {
    for (entry_ndx=0; entry_ndx<(SOC_PB_PP_LLP_COS_MAX_UP + 1); ++entry_ndx)
    {
      /*
       *  Map UP i to DP 7-i/2
       */
      entry.value1 = SOC_PB_PP_LLP_COS_MAX_DP - entry_ndx/2;
      res = soc_pb_pp_llp_cos_mapping_table_entry_set_unsafe(
        unit,
        mapping_tbl_ndx,
        table_id_ndx,
        entry_ndx,
        &entry
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

  /* Mapping DE to UP */
  mapping_tbl_ndx = SOC_PB_PP_LLP_COS_MAPPING_TABLE_DE_TO_DP;

  for (table_id_ndx=0; table_id_ndx<SOC_PB_PP_LLP_COS_NOF_DE_TO_DP_TABLES; ++table_id_ndx)
  {
    for (entry_ndx=0; entry_ndx<(SOC_PB_PP_LLP_COS_MAX_DE + 1); ++entry_ndx)
    {
      /*
       *  Map DE i to DP i
       */
      entry.value1 = entry_ndx;
      
      res = soc_pb_pp_llp_cos_mapping_table_entry_set_unsafe(
        unit,
        mapping_tbl_ndx,
        table_id_ndx,
        entry_ndx,
        &entry
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_init_unsafe()", table_id_ndx, entry_ndx);
}

/*********************************************************************
*     Sets global information for COS resolution, including
 *     default drop precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_GLBL_INFO                   *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_SET(
    regs->ihp.de_to_dp_map_reg.default_drop_precedence,
    glbl_info->default_dp,
    10,
    exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_glbl_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_llp_cos_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_GLBL_INFO                   *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_GLBL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_GLBL_INFO, glbl_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_glbl_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_llp_cos_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_GLBL_INFO_GET_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_glbl_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets global information for COS resolution, including
 *     default drop precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_GLBL_INFO                   *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PB_PP_LLP_COS_GLBL_INFO_clear(glbl_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(
    regs->ihp.de_to_dp_map_reg.default_drop_precedence,
    fld_val,
    10,
    exit);

  glbl_info->default_dp = (SOC_SAND_PP_DP)fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_glbl_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets port information for COS resolution, including
 *     which mechanisms to perform, table instance to use for
 *     mapping,...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_INFO                   *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
    unit,
    local_port_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Default
   */
  tbl_data.default_traffic_class = port_info->default_tc;

  /*
   * L2
   */
  tbl_data.pcp_profile = port_info->l2_info.tbls_select.in_up_to_tc_and_de_index;
  tbl_data.incoming_up_map_profile = port_info->l2_info.tbls_select.tc_to_up_index;
  tbl_data.drop_precedence_profile = port_info->l2_info.tbls_select.up_to_dp_index;

  tbl_data.ignore_incoming_up = SOC_SAND_BOOL2NUM(!port_info->l2_info.up_use.use_for_tc);
  tbl_data.use_dei = SOC_SAND_BOOL2NUM(port_info->l2_info.use_dei);
  tbl_data.tc_port_protocol_enable = SOC_SAND_BOOL2NUM(port_info->l2_info.use_l2_protocol);
  tbl_data.ignore_incoming_pcp = SOC_SAND_BOOL2NUM(port_info->l2_info.ignore_pkt_pcp_for_tc);

  /*
   * L3
   */
  tbl_data.tc_dp_tos_index = port_info->l3_info.ip_qos_to_tc_index;
  tbl_data.tc_dp_tos_enable = SOC_SAND_BOOL2NUM(port_info->l3_info.use_ip_qos);
  tbl_data.tc_subnet_enable = SOC_SAND_BOOL2NUM(port_info->l3_info.use_ip_subnet);

  /*
   * L4
   */
  tbl_data.tc_l4_protocol_enable = port_info->l4_info.use_l4_prtcl;

  res = soc_pb_pp_ihp_pinfo_llr_tbl_set_unsafe(
          unit,
          local_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_port_info_set_unsafe()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_llp_cos_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_INFO                   *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_PORT_INFO, port_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_llp_cos_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_port_info_get_verify()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets port information for COS resolution, including
 *     which mechanisms to perform, table instance to use for
 *     mapping,...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_PORT_INFO                   *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PB_PP_LLP_COS_PORT_INFO_clear(port_info);

  res = soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
    unit,
    local_port_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Default
   */
  port_info->default_tc = (SOC_SAND_PP_TC)tbl_data.default_traffic_class;

  /*
  * L2
  */
  port_info->l2_info.tbls_select.in_up_to_tc_and_de_index = tbl_data.pcp_profile;
  port_info->l2_info.tbls_select.tc_to_up_index = tbl_data.incoming_up_map_profile;
  port_info->l2_info.tbls_select.up_to_dp_index = tbl_data.drop_precedence_profile;

  port_info->l2_info.up_use.use_for_tc = SOC_SAND_NUM2BOOL(!tbl_data.ignore_incoming_up);
  port_info->l2_info.use_dei = SOC_SAND_NUM2BOOL(tbl_data.use_dei);
  port_info->l2_info.use_l2_protocol = SOC_SAND_NUM2BOOL(tbl_data.tc_port_protocol_enable);
  port_info->l2_info.ignore_pkt_pcp_for_tc = SOC_SAND_NUM2BOOL(tbl_data.ignore_incoming_pcp);

  /*
  * L3
  */
  port_info->l3_info.ip_qos_to_tc_index = tbl_data.tc_dp_tos_index;
  port_info->l3_info.use_ip_qos = (uint8)tbl_data.tc_dp_tos_enable;
  port_info->l3_info.use_ip_subnet = (uint8)tbl_data.tc_subnet_enable;

  /*
  * L4
  */
  port_info->l4_info.use_l4_prtcl = SOC_SAND_NUM2BOOL(tbl_data.tc_l4_protocol_enable);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_port_info_get_unsafe()", local_port_ndx, 0);
}


/*********************************************************************
*     Set mapping from packet fields to TC/DP, includes the
 *     following mappings:- UP to DE and TC.- IPv4 TOS to DP
 *     and TC.- IPv6 TC to DP and TC.- DE to DP- UP to DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_mapping_table_entry_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_offset,
    bit_offset,
    offset,
    reg_val,
    fld_val,
    is_ipv4;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_PP_IHP_TOS_2_COS_TBL_DATA
    tos_2_cos_tbl_data;
  SOC_PETRA_REG_FIELD
    *de_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry);

  regs = soc_pb_pp_regs();
 
  /* Default value for IPV6_TC_TO_DP_TC_VALID if case is entered not via
     ipv4 case */
  is_ipv4 = 0;

  switch (mapping_tbl_ndx)
  {
  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC:
    /*
     * Each 32 bit register holds 8 values of 4b DE + TC.
     * E.g: entry in offset 9 in table should be in 2nd register, bits 7:4.
     */
    offset = SOC_PB_PP_LLP_COS_PCP_DECODE_TBL_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 8;
    bit_offset = (offset % 8) * 4;
    fld_val = ((entry->value1 << 3) + entry->value2);

    SOC_PB_PP_REG_GET(regs->ihp.pcp_decoding_table_reg[reg_offset], reg_val, 10, exit);
    
    res = soc_sand_bitstream_set_field(
            &reg_val,
            bit_offset,
            4 /* DE + TC */,
            fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_PB_PP_REG_SET(regs->ihp.pcp_decoding_table_reg[reg_offset], reg_val, 30, exit);

  	break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID:
    is_ipv4 = 1;
    /* Fall-through on purpose. Both cases are handled the same way, with different
       is_ipv4 value */

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID:
    offset = SOC_PB_PP_IHP_TOS_2_COS_KEY_ENTRY_OFFSET(is_ipv4, table_id_ndx, entry_ndx);

    res = soc_pb_pp_ihp_tos_2_cos_tbl_get_unsafe(
      unit,
      offset,
      &tos_2_cos_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    tos_2_cos_tbl_data.dp = entry->value1;
    tos_2_cos_tbl_data.tc = entry->value2;
    tos_2_cos_tbl_data.valid = SOC_SAND_BOOL2NUM(entry->valid);

    res = soc_pb_pp_ihp_tos_2_cos_tbl_set_unsafe(
      unit,
      offset,
      &tos_2_cos_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  	break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP:
    /*
    * Each 32 bit register holds 8 values of 3b UP.
    * E.g: entry in offset 9 in table should be in 2nd register, bits 5:3.
    */
    offset = SOC_PB_PP_LLP_COS_INCOMING_UP_MAP_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 8;
    bit_offset = (offset % 8) * 3;
    fld_val = entry->value1;

    SOC_PB_PP_REG_GET(regs->ihp.incoming_up_map_reg[reg_offset], reg_val, 60, exit);

    res = soc_sand_bitstream_set_field(
      &reg_val,
      bit_offset,
      3 /* UP */,
      fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    SOC_PB_PP_REG_SET(regs->ihp.incoming_up_map_reg[reg_offset], reg_val, 80, exit);

    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_TC_TO_UP:

    /*
     * Maps 3b TC to 3b UP: one register, bit offset is TC * 3
     */
    bit_offset = entry_ndx * 3;
    fld_val = entry->value1;
    SOC_PB_PP_REG_GET(regs->ihp.traffic_class_to_user_priority_reg, reg_val, 90, exit);

    res = soc_sand_bitstream_set_field(
      &reg_val,
      bit_offset,
      3 /* UP */,
      fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    SOC_PB_PP_REG_SET(regs->ihp.traffic_class_to_user_priority_reg, reg_val, 110, exit);

  	break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_DE_TO_DP:
    de_fld =
      (entry_ndx ?
       &regs->ihp.de_to_dp_map_reg.dp_when_de_is_1 :
       &regs->ihp.de_to_dp_map_reg.dp_when_de_is_0);
    
    SOC_PB_PP_FLD_SET(
      (*de_fld),
      entry->value1,
      120,
      exit);

    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DP:

    /*
    * Each 32 bit register holds 16 values of 2b DP.
    * E.g: entry in offset 17 in table should be in 2nd register, bits 3:2.
    */
    offset = SOC_PB_PP_LLP_COS_DROP_PRECEDENCE_MAP_PCP_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 16;
    bit_offset = (offset % 16) * 2;
    fld_val = entry->value1;

    SOC_PB_PP_REG_GET(regs->ihp.drop_precedence_map_pcp_reg[reg_offset], reg_val, 120, exit);

    res = soc_sand_bitstream_set_field(
      &reg_val,
      bit_offset,
      2 /* UP */,
      fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    SOC_PB_PP_REG_SET(regs->ihp.drop_precedence_map_pcp_reg[reg_offset], reg_val, 130, exit);

    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR, 140, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_mapping_table_entry_set_unsafe()", mapping_tbl_ndx, table_id_ndx);
}

uint32
  soc_pb_pp_llp_cos_mapping_table_entry_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    up = 0,
    de = 0,
    dp = 0,
    tc = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_SET_VERIFY);

  res = soc_pb_pp_llp_cos_mapping_table_entry_get_verify(
    unit,
    mapping_tbl_ndx,
    table_id_ndx,
    entry_ndx
    );
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO, entry, 40, exit);

  switch (mapping_tbl_ndx)
  {
  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC:
    de = entry->value1;
    tc = entry->value2;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID:
    dp = entry->value1;
    tc = entry->value2;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID:
    dp = entry->value1;
    tc = entry->value2;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP:
    up = entry->value1;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_TC_TO_UP:
    up = entry->value1;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_DE_TO_DP:
    dp = entry->value1;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DP:
    dp = entry->value1;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR, 50, exit);
    break;
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(up, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 60, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(de, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 80, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 90, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_mapping_table_entry_set_verify()", 0, table_id_ndx);
}

uint32
  soc_pb_pp_llp_cos_mapping_table_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{
  uint32
    up = 0,
    de = 0,
    tc = 0,
    ipv4_tos = 0,
    ipv6_tc = 0,
    tbl_id_ndx_max = 0;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mapping_tbl_ndx, SOC_PB_PP_LLP_COS_MAPPING_TBL_NDX_MAX, SOC_PB_PP_LLP_COS_MAPPING_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(table_id_ndx, SOC_PB_PP_LLP_COS_TABLE_ID_NDX_MAX, SOC_PB_PP_LLP_COS_TABLE_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

  switch (mapping_tbl_ndx)
  {
  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC:
    up = entry_ndx;
    tbl_id_ndx_max = SOC_PB_PP_LLP_COS_PCP_PROFILE_MAX;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID:
    ipv4_tos = entry_ndx;
    tbl_id_ndx_max = SOC_PB_PP_LLP_COS_TOS_INDEX_MAX;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID:
    ipv6_tc = entry_ndx;
    tbl_id_ndx_max = SOC_PB_PP_LLP_COS_TOS_INDEX_MAX;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP:
    up = entry_ndx;
    tbl_id_ndx_max = SOC_PB_PP_LLP_COS_UP_MAP_PROFILE_MAX;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_TC_TO_UP:
    tc = entry_ndx;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_DE_TO_DP:
    de = entry_ndx;
    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DP:
    up = entry_ndx;
    tbl_id_ndx_max = SOC_PB_PP_LLP_COS_DP_PROFILE_MAX;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR, 50, exit);
    break;
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(up, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 60, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(de, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 80, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 100, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ipv4_tos, SOC_SAND_PP_IPV4_TOS_MAX, SOC_SAND_PP_IPV4_TOS_OUT_OF_RANGE_ERR, 110, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ipv6_tc, SOC_SAND_PP_IPV6_TC_MAX, SOC_SAND_PP_IPV6_TC_OUT_OF_RANGE_ERR, 120, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(table_id_ndx, tbl_id_ndx_max, SOC_PB_PP_LLP_COS_MAPPING_TBL_NDX_RANGE_MISMATCH_ERR, 130, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_mapping_table_entry_get_verify()", 0, table_id_ndx);
}

/*********************************************************************
*     Set mapping from packet fields to TC/DP, includes the
 *     following mappings:- UP to DE and TC.- IPv4 TOS to DP
 *     and TC.- IPv6 TC to DP and TC.- DE to DP- UP to DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_mapping_table_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_offset,
    bit_offset,
    offset,
    reg_val[1],
    fld_val,
    is_ipv4;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_PP_IHP_TOS_2_COS_TBL_DATA
    tos_2_cos_tbl_data;
  SOC_PETRA_REG_FIELD
    *de_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry);

  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(entry);

  regs = soc_pb_pp_regs();

  /* Default value for IPV6_TC_TO_DP_TC_VALID if case is entered not via
  ipv4 case */
  is_ipv4 = 0;

  switch (mapping_tbl_ndx)
  {
  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC:
    /*
    * Each 32 bit register holds 8 values of 4b DE + TC.
    * E.g: entry in offset 9 in table should be in 2nd register, bits 7:4.
    */
    offset = SOC_PB_PP_LLP_COS_PCP_DECODE_TBL_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 8;
    bit_offset = (offset % 8) * 4;

    SOC_PB_PP_REG_GET(regs->ihp.pcp_decoding_table_reg[reg_offset], *reg_val, 10, exit);

    res = soc_sand_bitstream_get_field(
      reg_val,
      bit_offset,
      4 /* DE + TC */,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    entry->value2 = SOC_SAND_GET_BITS_RANGE(fld_val, 2, 0);
    entry->value1 = SOC_SAND_GET_BIT(fld_val, 3);
    
    SOC_PB_PP_REG_SET(regs->ihp.pcp_decoding_table_reg[reg_offset], *reg_val, 30, exit);

    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID:
    is_ipv4 = 1;
    /* Fall-through on purpose. Both cases are handled the same way, with different
    is_ipv4 value */

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID:
    offset = SOC_PB_PP_IHP_TOS_2_COS_KEY_ENTRY_OFFSET(is_ipv4, table_id_ndx, entry_ndx);

    res = soc_pb_pp_ihp_tos_2_cos_tbl_get_unsafe(
      unit,
      offset,
      &tos_2_cos_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    entry->value1 = tos_2_cos_tbl_data.dp;
    entry->value2 = tos_2_cos_tbl_data.tc;
    entry->valid = SOC_SAND_NUM2BOOL(tos_2_cos_tbl_data.valid);

    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP:
    /*
    * Each 32 bit register holds 8 values of 3b UP.
    * E.g: entry in offset 9 in table should be in 2nd register, bits 5:3.
    */
    offset = SOC_PB_PP_LLP_COS_INCOMING_UP_MAP_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 8;
    bit_offset = (offset % 8) * 3;

    SOC_PB_PP_REG_GET(regs->ihp.incoming_up_map_reg[reg_offset], *reg_val, 60, exit);

    res = soc_sand_bitstream_get_field(
      reg_val,
      bit_offset,
      3 /* UP */,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    entry->value1 = fld_val;

    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_TC_TO_UP:

    /*
    * Maps 3b TC to 3b UP: one register, bit offset is TC * 3
    */
    bit_offset = entry_ndx * 3;
    SOC_PB_PP_REG_GET(regs->ihp.traffic_class_to_user_priority_reg, *reg_val, 90, exit);

    res = soc_sand_bitstream_get_field(
      reg_val,
      bit_offset,
      3 /* UP */,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    entry->value1 = fld_val;

    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_DE_TO_DP:
    de_fld =
      (entry_ndx ?
      &regs->ihp.de_to_dp_map_reg.dp_when_de_is_1 :
    &regs->ihp.de_to_dp_map_reg.dp_when_de_is_0);

    SOC_PB_PP_FLD_GET(
      (*de_fld),
      fld_val,
      120,
      exit);

    entry->value1 = fld_val;

    break;

  case SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DP:

    /*
    * Each 32 bit register holds 16 values of 2b DP.
    * E.g: entry in offset 17 in table should be in 2nd register, bits 3:2.
    */
    offset = SOC_PB_PP_LLP_COS_DROP_PRECEDENCE_MAP_PCP_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 16;
    bit_offset = (offset % 16) * 2;

    SOC_PB_PP_REG_GET(regs->ihp.drop_precedence_map_pcp_reg[reg_offset], *reg_val, 120, exit);

    res = soc_sand_bitstream_get_field(
      reg_val,
      bit_offset,
      2 /* UP */,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    entry->value1 = fld_val;

    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR, 140, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_mapping_table_entry_get_unsafe()", mapping_tbl_ndx, table_id_ndx);
}

/*********************************************************************
*     Sets range of L4 ports. Packets L4 ports will be
 *     compared to this range in order to set TC value
 *     correspondingly.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_l4_port_range_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val,
    fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(regs->ihp.traffic_class_l4_range_reg[range_ndx], reg_val, 10, exit);

  fld_val = range_info->start;
  SOC_PB_PP_FLD_TO_REG(
    regs->ihp.traffic_class_l4_range_reg[range_ndx].tc_l4_range_min,
    fld_val,
    reg_val,
    20,
    exit
    );

  fld_val = range_info->end;
  SOC_PB_PP_FLD_TO_REG(
    regs->ihp.traffic_class_l4_range_reg[range_ndx].tc_l4_range_max,
    fld_val,
    reg_val,
    30,
    exit
    );

  SOC_PB_PP_REG_SET(regs->ihp.traffic_class_l4_range_reg[range_ndx], reg_val, 40, exit);

  fld_val = SOC_SAND_BOOL2NUM_INVERSE(range_info->in_range);
  SOC_PB_PP_FLD_SET(
    regs->ihp.traffic_class_l4_range_cfg_reg.tc_l4_range_outside[range_ndx],
    fld_val,
    40,
    exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_l4_port_range_info_set_unsafe()", range_ndx, 0);
}

uint32
  soc_pb_pp_llp_cos_l4_port_range_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(range_ndx, SOC_PB_PP_LLP_COS_RANGE_NDX_MAX, SOC_PB_PP_LLP_COS_RANGE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO, range_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_l4_port_range_info_set_verify()", range_ndx, 0);
}

uint32
  soc_pb_pp_llp_cos_l4_port_range_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(range_ndx, SOC_PB_PP_LLP_COS_RANGE_NDX_MAX, SOC_PB_PP_LLP_COS_RANGE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_l4_port_range_info_get_verify()", range_ndx, 0);
}

/*********************************************************************
*     Sets range of L4 ports. Packets L4 ports will be
 *     compared to this range in order to set TC value
 *     correspondingly.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_l4_port_range_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val,
    fld_val;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_clear(range_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(regs->ihp.traffic_class_l4_range_reg[range_ndx], reg_val, 10, exit);

  SOC_PB_PP_FLD_FROM_REG(
    regs->ihp.traffic_class_l4_range_reg[range_ndx].tc_l4_range_min,
    fld_val,
    reg_val,
    20,
    exit
    );
  range_info->start = (uint16)fld_val;

  SOC_PB_PP_FLD_FROM_REG(
    regs->ihp.traffic_class_l4_range_reg[range_ndx].tc_l4_range_max,
    fld_val,
    reg_val,
    30,
    exit
    );
  range_info->end = (uint16)fld_val;

  SOC_PB_PP_FLD_GET(
    regs->ihp.traffic_class_l4_range_cfg_reg.tc_l4_range_outside[range_ndx],
    fld_val,
    40,
    exit);
  range_info->in_range = SOC_SAND_NUM2BOOL_INVERSE(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_l4_port_range_info_set_unsafe()", range_ndx, 0);
}

/*********************************************************************
*     Set mapping to Traffic class according to L4 information
 *     includingSource/Destination and protocol type (TCP or
 *     UDP). Packet L4 destination and source ports are
 *     compared to the ranges set by
 *     soc_ppd_llp_cos_l4_port_range_info_set(). Then according to
 *     comparison result: - 0-3 for source port (where 3 means
 *     was not match) - 0-3 for destination port (where 3 means
 *     was not match)and according to protocol type (UDP/TCP),
 *     this API is used to determine the TC value to set to the
 *     packet
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_l4_port_range_to_tc_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_TC_INFO                     *tc_info
  )
{
  uint32
    res = SOC_SAND_OK,
    offset,
    reg_offset,
    bit_offset,
    reg_val,
    fld_val;
  uint8
    is_tcp,
    is_udp;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tc_info);

  regs = soc_pb_pp_regs();

  is_tcp = (uint8)(l4_prtcl_type_ndx != SOC_SAND_PP_L4_PRTCL_TYPE_UDP);
  is_udp = (uint8)(l4_prtcl_type_ndx != SOC_SAND_PP_L4_PRTCL_TYPE_TCP);

  if (is_tcp)
  {
    offset = SOC_PB_PP_LLP_COS_L4_PORT_RANGES_2_TC_TABLE_OFFSET(
      is_tcp,
      src_port_match_range_ndx,
      dest_port_match_range_ndx);

    /* 8 values of 4b (valid(1b), TC(3b)) per register */
    reg_offset = offset / 8;
    bit_offset = offset % 8 * 4;
    fld_val = (((SOC_SAND_BOOL2NUM(tc_info->valid)) << 3) + tc_info->tc);

    SOC_PB_PP_REG_GET(regs->ihp.l4_port_ranges_2_tc_table_reg[reg_offset], reg_val, 10, exit);

    res = soc_sand_bitstream_set_field(
      &reg_val,
      bit_offset,
      4 /* Valid(1b) + TC(3b) */,
      fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_PB_PP_REG_SET(regs->ihp.l4_port_ranges_2_tc_table_reg[reg_offset], reg_val, 30, exit);
  }

  if (is_udp)
  {
    offset = SOC_PB_PP_LLP_COS_L4_PORT_RANGES_2_TC_TABLE_OFFSET(
      (SOC_SAND_BOOL2NUM(!is_udp)),
      src_port_match_range_ndx,
      dest_port_match_range_ndx);

    /* 8 values of 4b (valid(1b), TC(3b)) per register */
    reg_offset = offset / 8;
    bit_offset = offset % 8 * 4;
    fld_val = (((SOC_SAND_BOOL2NUM(tc_info->valid)) << 3) + tc_info->tc);

    SOC_PB_PP_REG_GET(regs->ihp.l4_port_ranges_2_tc_table_reg[reg_offset], reg_val, 10, exit);

    res = soc_sand_bitstream_set_field(
      &reg_val,
      bit_offset,
      4 /* Valid(1b) + TC(3b) */,
      fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_PB_PP_REG_SET(regs->ihp.l4_port_ranges_2_tc_table_reg[reg_offset], reg_val, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_l4_port_range_to_tc_info_set_unsafe()", 0, src_port_match_range_ndx);
}

uint32
  soc_pb_pp_llp_cos_l4_port_range_to_tc_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_TC_INFO                     *tc_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(l4_prtcl_type_ndx, SOC_PB_PP_LLP_COS_L4_PRTCL_TYPE_NDX_MAX, SOC_PB_PP_LLP_COS_L4_PRTCL_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(src_port_match_range_ndx, SOC_PB_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_MAX, SOC_PB_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dest_port_match_range_ndx, SOC_PB_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_MAX, SOC_PB_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR, 30, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_TC_INFO, tc_info, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_l4_port_range_to_tc_info_set_verify()", 0, src_port_match_range_ndx);
}

uint32
  soc_pb_pp_llp_cos_l4_port_range_to_tc_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_VERIFY);

  if ((l4_prtcl_type_ndx != SOC_SAND_PP_L4_PRTCL_TYPE_TCP) &&
      (l4_prtcl_type_ndx != SOC_SAND_PP_L4_PRTCL_TYPE_UDP))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_COS_L4_PRTCL_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(src_port_match_range_ndx, SOC_PB_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_MAX, SOC_PB_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dest_port_match_range_ndx, SOC_PB_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_MAX, SOC_PB_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_l4_port_range_to_tc_info_get_verify()", 0, src_port_match_range_ndx);
}

/*********************************************************************
*     Set mapping to Traffic class according to L4 information
 *     includingSource/Destination and protocol type (TCP or
 *     UDP). Packet L4 destination and source ports are
 *     compared to the ranges set by
 *     soc_ppd_llp_cos_l4_port_range_info_set(). Then according to
 *     comparison result: - 0-3 for source port (where 3 means
 *     was not match) - 0-3 for destination port (where 3 means
 *     was not match)and according to protocol type (UDP/TCP),
 *     this API is used to determine the TC value to set to the
 *     packet
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_l4_port_range_to_tc_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_TC_INFO                     *tc_info
  )
{
  uint32
    res = SOC_SAND_OK,
    offset,
    reg_offset,
    bit_offset,
    reg_val[1],
    fld_val;
  uint8
    is_tcp;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tc_info);

  SOC_PB_PP_LLP_COS_TC_INFO_clear(tc_info);

  regs = soc_pb_pp_regs();

  is_tcp = (uint8)(l4_prtcl_type_ndx == SOC_SAND_PP_L4_PRTCL_TYPE_TCP);

  offset = SOC_PB_PP_LLP_COS_L4_PORT_RANGES_2_TC_TABLE_OFFSET(
    is_tcp,
    src_port_match_range_ndx,
    dest_port_match_range_ndx);

  /* 8 values of 4b (valid(1b), TC(3b)) per register */
  reg_offset = offset / 8;
  bit_offset = offset % 8 * 4;

  SOC_PB_PP_REG_GET(regs->ihp.l4_port_ranges_2_tc_table_reg[reg_offset], *reg_val, 10, exit);

  res = soc_sand_bitstream_get_field(
    reg_val,
    bit_offset,
    4 /* Valid(1b) + TC(3b) */,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tc_info->valid = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BIT(fld_val, 3));
  tc_info->tc = (SOC_SAND_PP_TC)SOC_SAND_GET_BITS_RANGE(fld_val, 2, 0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_l4_port_range_to_tc_info_get_unsafe()", 0, src_port_match_range_ndx);
}

/*********************************************************************
*     Set COS parameters based on source IPv4 subnet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_ipv4_subnet_based_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  res = soc_pb_pp_ihp_subnet_classify_tbl_get_unsafe(
    unit,
    entry_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Override subnet only if vid is valid, in order not to ruin the vid assign entry (if valid) */
  if (subnet_based_info->tc_is_valid)
  {
    tbl_data.ipv4_subnet = subnet->ip_address;
    tbl_data.ipv4_subnet_mask = (32 - subnet->prefix_len);
  }
  tbl_data.ipv4_subnet_tc = subnet_based_info->tc;
  tbl_data.ipv4_subnet_tc_valid = SOC_SAND_BOOL2NUM(subnet_based_info->tc_is_valid);

  res = soc_pb_pp_ihp_subnet_classify_tbl_set_unsafe(
    unit,
    entry_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_ipv4_subnet_based_set_unsafe()", entry_ndx, 0);
}

uint32
  soc_pb_pp_llp_cos_ipv4_subnet_based_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_LLP_COS_IPV4_SUBNET_NDX_MAX, SOC_PB_PP_LLP_COS_IPV4_SUBNET_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
  res = soc_sand_pp_ipv4_subnet_verify(subnet);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO, subnet_based_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_ipv4_subnet_based_set_verify()", entry_ndx, 0);
}

uint32
  soc_pb_pp_llp_cos_ipv4_subnet_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_LLP_COS_IPV4_SUBNET_NDX_MAX, SOC_PB_PP_LLP_COS_IPV4_SUBNET_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_ipv4_subnet_based_get_verify()", entry_ndx, 0);
}

/*********************************************************************
*     Set COS parameters based on source IPv4 subnet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_ipv4_subnet_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_IPV4_SUBNET_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  soc_sand_SAND_PP_IPV4_SUBNET_clear(subnet);
  SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO_clear(subnet_based_info);

  res = soc_pb_pp_ihp_subnet_classify_tbl_get_unsafe(
    unit,
    entry_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  subnet->ip_address = tbl_data.ipv4_subnet;
  subnet->prefix_len = (uint8)(32 - tbl_data.ipv4_subnet_mask);
  subnet_based_info->tc = (SOC_SAND_PP_TC)tbl_data.ipv4_subnet_tc;
  subnet_based_info->tc_is_valid = SOC_SAND_NUM2BOOL(tbl_data.ipv4_subnet_tc_valid);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_ipv4_subnet_based_set_unsafe()", entry_ndx, 0);
}

/*********************************************************************
*     Sets mapping from Ethernet Type and profile to COS
 *     parameters.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_protocol_based_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PRTCL_INFO                  *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK,
    tbl_offset,
    ether_type_internal;
  uint8
    update_hw,
    alloc_ether_type,
    found;
  SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA
    ihp_port_protocol_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  tbl_offset = 0;
  *success = SOC_SAND_SUCCESS;
  alloc_ether_type = TRUE;
  update_hw = TRUE;

  res = soc_pb_pp_l2_next_prtcl_type_find(
          unit,
          ether_type_ndx,
          &ether_type_internal,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (found)
  {
    /* Ethertype already allocated. Let's get the table entry */
    tbl_offset =
      SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(ether_type_internal, profile_ndx);

    res = soc_pb_pp_ihp_port_protocol_tbl_get_unsafe(
            unit,
            tbl_offset,
            &ihp_port_protocol_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (ihp_port_protocol_tbl_data.traffic_class_valid)
    {
      if (prtcl_assign_info->tc_is_valid)
      {
        if (ihp_port_protocol_tbl_data.traffic_class == prtcl_assign_info->tc)
        {
          /* Same data is already configured. Nothing to do. */
          update_hw = FALSE;
          alloc_ether_type = FALSE;
        }
        else /* (ihp_port_protocol_tbl_data.traffic_class != prtcl_assign_info->tc) */
        {
          /* Entry will be later update to new value */
        }
      }
      else /* !prtcl_assign_info->tc_is_valid */
      {
        /* Entry is currently valid, but will be configured to be invalid */
        res = soc_pb_pp_l2_next_prtcl_type_deallocate(
                unit,
                ether_type_ndx
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        alloc_ether_type = FALSE;
      }
    }
  }
  
  if (alloc_ether_type)
  {
    /* Allocate ether type */
    res = soc_pb_pp_l2_next_prtcl_type_allocate(
            unit,
            ether_type_ndx,
            &ether_type_internal,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (*success == SOC_SAND_SUCCESS)
  {
    if (!found)
    {
      tbl_offset =
        SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(ether_type_internal, profile_ndx);

      res = soc_pb_pp_ihp_port_protocol_tbl_get_unsafe(
              unit,
              tbl_offset,
              &ihp_port_protocol_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

    if (update_hw)
    {
      ihp_port_protocol_tbl_data.traffic_class = prtcl_assign_info->tc;
      ihp_port_protocol_tbl_data.traffic_class_valid = SOC_SAND_BOOL2NUM(prtcl_assign_info->tc_is_valid);

      res = soc_pb_pp_ihp_port_protocol_tbl_set_unsafe(
              unit,
              tbl_offset,
              &ihp_port_protocol_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_protocol_based_set_unsafe()", profile_ndx, ether_type_ndx);
}


uint32
  soc_pb_pp_llp_cos_protocol_based_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PB_PP_LLP_COS_PROFILE_NDX_MAX, SOC_PB_PP_LLP_COS_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_PRTCL_INFO, prtcl_assign_info, 30, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_protocol_based_set_verify()", profile_ndx, ether_type_ndx);
}

uint32
  soc_pb_pp_llp_cos_protocol_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PB_PP_LLP_COS_PROFILE_NDX_MAX, SOC_PB_PP_LLP_COS_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_protocol_based_get_verify()", profile_ndx, ether_type_ndx);
}

/*********************************************************************
*     Sets mapping from Ethernet Type and profile to COS
 *     parameters.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_protocol_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  )
{
  uint32
    res = SOC_SAND_OK,
    tbl_offset,
    ether_type_internal;
  uint8
    found;
  SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA
    ihp_port_protocol_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_COS_PROTOCOL_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);

  SOC_PB_PP_LLP_COS_PRTCL_INFO_clear(prtcl_assign_info);

  res = soc_pb_pp_l2_next_prtcl_type_find(
          unit,
          ether_type_ndx,
          &ether_type_internal,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (found)
  {
    tbl_offset =
      SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(ether_type_internal, profile_ndx);

    res = soc_pb_pp_ihp_port_protocol_tbl_get_unsafe(
      unit,
      tbl_offset,
      &ihp_port_protocol_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    prtcl_assign_info->tc_is_valid =
      SOC_SAND_NUM2BOOL(ihp_port_protocol_tbl_data.traffic_class_valid);
    prtcl_assign_info->tc = (SOC_SAND_PP_TC)ihp_port_protocol_tbl_data.traffic_class;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LLP_COS_ETHER_TYPE_NOT_EXIT_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_cos_protocol_based_set_unsafe()", profile_ndx, ether_type_ndx);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_llp_cos module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_llp_cos_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_llp_cos;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_llp_cos module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_llp_cos_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_llp_cos;
}

uint32
  SOC_PB_PP_LLP_COS_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_TC_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_TC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_TC_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_UP_USE_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_UP_USE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_UP_USE_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_PORT_L2_TABLE_INDEXES_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_L2_TABLE_INDEXES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_up_to_tc_and_de_index, SOC_PB_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_MAX, SOC_PB_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc_to_up_index, SOC_PB_PP_LLP_COS_TC_TO_UP_INDEX_MAX, SOC_PB_PP_LLP_COS_TC_TO_UP_INDEX_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->up_to_dp_index, SOC_PB_PP_LLP_COS_UP_TO_DP_INDEX_MAX, SOC_PB_PP_LLP_COS_UP_TO_DP_INDEX_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc_to_dp_index, SOC_PB_PP_LLP_COS_TC_TO_DP_INDEX_MAX, SOC_PB_PP_LLP_COS_TC_TO_DP_INDEX_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_PORT_L2_TABLE_INDEXES_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_PORT_L2_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_L2_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_UP_USE, &(info->up_use), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_PORT_L2_TABLE_INDEXES, &(info->tbls_select), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_PORT_L2_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_PORT_L3_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ip_qos_to_tc_index, SOC_PB_PP_LLP_COS_IP_QOS_TO_TC_INDEX_MAX, SOC_PB_PP_LLP_COS_IP_QOS_TO_TC_INDEX_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_PORT_L3_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_PORT_L4_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_PORT_L4_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_PORT_L2_INFO, &(info->l2_info), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_PORT_L3_INFO, &(info->l3_info), 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_COS_PORT_L4_INFO, &(info->l4_info), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_PORT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->value1, SOC_PB_PP_LLP_COS_VALUE1_MAX, SOC_PB_PP_LLP_COS_VALUE1_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->value2, SOC_PB_PP_LLP_COS_VALUE2_MAX, SOC_PB_PP_LLP_COS_VALUE2_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_PB_PP_LLP_COS_TC_MAX, SOC_PB_PP_LLP_COS_TC_OUT_OF_RANGE_ERR, 10, exit); */

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_PRTCL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_PRTCL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

