/* $Id: pcp_oam_mpls.c,v 1.6 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PCP/pcp_general.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_oam_mpls.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_OAM_MPLS_RATE_NDX_MAX                              (PCP_OAM_NOF_MPLS_CC_PKT_TYPES-1)
#define PCP_OAM_MPLS_INTERVAL_MAX                              (PCP_OAM_NOF_MPLS_CC_PKT_TYPES-1)
#define PCP_OAM_MPLS_MOT_NDX_MAX                               (SOC_SAND_U32_MAX)
#define PCP_OAM_MPLS_MOR_NDX_MAX                               (SOC_SAND_U32_MAX)
#define PCP_OAM_MPLS_TC_MAX                                    (SOC_SAND_UINT_MAX)
#define PCP_OAM_MPLS_DP_MAX                                    (SOC_SAND_UINT_MAX)
#define PCP_OAM_MPLS_EXP_MAX                                   (SOC_SAND_UINT_MAX)
#define PCP_OAM_MPLS_TTL_MAX                                   (SOC_SAND_UINT_MAX)
#define PCP_OAM_MPLS_SYSTEM_PORT_MAX                           (SOC_SAND_UINT_MAX)
#define PCP_OAM_MPLS_EEP_MAX                                   (SOC_SAND_U32_MAX)
#define PCP_OAM_MPLS_DEFECT_TYPE_MAX                           (PCP_OAM_NOF_MPLS_LSP_TX_DEFECT_TYPES-1)
#define PCP_OAM_MPLS_DEFECT_LOCATION_MAX                       (SOC_SAND_U32_MAX)
#define PCP_OAM_MPLS_TYPE_MAX                                  (PCP_OAM_NOF_MPLS_CC_PKT_TYPES-1)
#define PCP_OAM_MPLS_BASE_MAX                                  (SOC_SAND_U32_MAX)
#define PCP_OAM_MPLS_START_MAX                                 (SOC_SAND_U32_MAX)
#define PCP_OAM_MPLS_END_MAX                                   (SOC_SAND_U32_MAX)

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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_oam_mpls[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_TTL_MAPPING_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_TTL_MAPPING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_IP_MAPPING_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_IP_MAPPING_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_IP_MAPPING_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_IP_MAPPING_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_IP_MAPPING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_IP_MAPPING_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_IP_MAPPING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_IP_MAPPING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LABEL_RANGES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LABEL_RANGES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LABEL_RANGES_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LABEL_RANGES_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LABEL_RANGES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LABEL_RANGES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LABEL_RANGES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LABEL_RANGES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_TX_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_TX_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_TX_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_TX_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_TX_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_TX_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_TX_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_TX_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_RX_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_RX_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_RX_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_RX_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_RX_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_RX_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_RX_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_LSP_RX_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MPLS_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Pcp_error_desc_element_oam_mpls[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_OAM_MPLS_COS_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_COS_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_MPLS_COS_ID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_TTL_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_TTL_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_MPLS_TTL_ID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_IP_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_IP_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'PCP_OAM_MPLS_IP_ID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_IS_IPV6_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_IS_IPV6_OUT_OF_RANGE_ERR",
    "The parameter 'is_ipv6' is out of range. \n\r "
    "The range is: 0 - No max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_RATE_NDX_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_RATE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'rate_ndx' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_MPLS_CC_PKT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_INTERVAL_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_INTERVAL_OUT_OF_RANGE_ERR",
    "The parameter 'interval' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_MPLS_CC_PKT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_MOT_NDX_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_MOT_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mot_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_MOR_NDX_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_MOR_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mor_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_TC_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_TC_OUT_OF_RANGE_ERR",
    "The parameter 'tc' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_DP_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_DP_OUT_OF_RANGE_ERR",
    "The parameter 'dp' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_EXP_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_EXP_OUT_OF_RANGE_ERR",
    "The parameter 'exp' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_TTL_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_TTL_OUT_OF_RANGE_ERR",
    "The parameter 'ttl' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_SYSTEM_PORT_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_SYSTEM_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'system_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_EEP_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'eep' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_LSP_ID_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_LSP_ID_OUT_OF_RANGE_ERR",
    "The parameter 'lsp_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_DEFECT_TYPE_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_DEFECT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'defect_type' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_MPLS_LSP_TX_DEFECT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_DEFECT_LOCATION_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_DEFECT_LOCATION_OUT_OF_RANGE_ERR",
    "The parameter 'defect_location' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_TYPE_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_MPLS_CC_PKT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_BASE_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_BASE_OUT_OF_RANGE_ERR",
    "The parameter 'base' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_START_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_START_OUT_OF_RANGE_ERR",
    "The parameter 'start' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_MPLS_END_OUT_OF_RANGE_ERR,
    "PCP_OAM_MPLS_END_OUT_OF_RANGE_ERR",
    "The parameter 'end' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
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

/*********************************************************************
*     This function maps 2 bit ttl_ndx to TTL information.
 *     This mapping is used to build MPLS OAM packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_ttl_mapping_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ttl_mapping_info_set_unsafe()", ttl_ndx, 0);
}

uint32
  pcp_oam_mpls_ttl_mapping_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ttl_ndx, PCP_OAM_MPLS_TTL_ID_MAX, PCP_OAM_MPLS_TTL_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_MPLS_TTL_MAPPING_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ttl_mapping_info_set_verify()", ttl_ndx, 0);
}

uint32
  pcp_oam_mpls_ttl_mapping_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ttl_ndx, PCP_OAM_MPLS_TTL_ID_MAX, PCP_OAM_MPLS_TTL_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ttl_mapping_info_get_verify()", ttl_ndx, 0);
}

/*********************************************************************
*     This function maps 2 bit ttl_ndx to TTL information.
 *     This mapping is used to build MPLS OAM packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_ttl_mapping_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_TTL_MAPPING_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_MPLS_TTL_MAPPING_INFO_clear(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ttl_mapping_info_get_unsafe()", ttl_ndx, 0);
}

/*********************************************************************
*     This function maps 4 bit ip_ndx to IPv4/IPv6
 *     information. This mapping is used to determine the LSR
 *     identifier of the constructed MPLS OAM packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_ip_mapping_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx,
    SOC_SAND_IN  uint8                       is_ipv6,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_IP_MAPPING_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ip_mapping_info_set_unsafe()", ip_ndx, 0);
}

uint32
  pcp_oam_mpls_ip_mapping_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx,
    SOC_SAND_IN  uint8                       is_ipv6,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_IP_MAPPING_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ip_ndx, PCP_OAM_MPLS_IP_ID_MAX, PCP_OAM_MPLS_IP_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_MPLS_IP_MAPPING_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ip_mapping_info_set_verify()", ip_ndx, 0);
}

uint32
  pcp_oam_mpls_ip_mapping_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_IP_MAPPING_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(ip_ndx, PCP_OAM_MPLS_IP_ID_MAX, PCP_OAM_MPLS_IP_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ip_mapping_info_get_verify()", ip_ndx, 0);
}

/*********************************************************************
*     This function maps 4 bit ip_ndx to IPv4/IPv6
 *     information. This mapping is used to determine the LSR
 *     identifier of the constructed MPLS OAM packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_ip_mapping_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx,
    SOC_SAND_OUT uint8                       *is_ipv6,
    SOC_SAND_OUT PCP_OAM_MPLS_IP_MAPPING_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_IP_MAPPING_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_ipv6);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_MPLS_IP_MAPPING_INFO_clear(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_ip_mapping_info_get_unsafe()", ip_ndx, 0);
}

/*********************************************************************
*     This function sets label ranges for MPLS OAM. An
 *     incoming MPLS OAM packet is accelerated if in_label (the
 *     first label after the MPLS OAM label, 14) fulfils one of
 *     the following conditions: 1. in_label is between
 *     tnl_range.start and tnl_range.end.2. in_label is between
 *     pwe_range.start and pwe_range.end. pwe_range.base and
 *     tnl_range.base set the physical ranges associated with
 *     pwe_range and tnl_range respectively. The physical range
 *     is [range.base, range.base + range.end - range.start].if
 *     the packet does not fulfill either conditions, it is
 *     forwarded to CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_label_ranges_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *tnl_range,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *pwe_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LABEL_RANGES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tnl_range);
  SOC_SAND_CHECK_NULL_INPUT(pwe_range);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_label_ranges_set_unsafe()", 0, 0);
}

uint32
  pcp_oam_mpls_label_ranges_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *tnl_range,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *pwe_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LABEL_RANGES_SET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_OAM_MPLS_LABEL_RANGE, tnl_range, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_MPLS_LABEL_RANGE, pwe_range, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_label_ranges_set_verify()", 0, 0);
}

uint32
  pcp_oam_mpls_label_ranges_get_verify(
    SOC_SAND_IN  int                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LABEL_RANGES_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_label_ranges_get_verify()", 0, 0);
}

/*********************************************************************
*     This function sets label ranges for MPLS OAM. An
 *     incoming MPLS OAM packet is accelerated if in_label (the
 *     first label after the MPLS OAM label, 14) fulfils one of
 *     the following conditions: 1. in_label is between
 *     tnl_range.start and tnl_range.end.2. in_label is between
 *     pwe_range.start and pwe_range.end. pwe_range.base and
 *     tnl_range.base set the physical ranges associated with
 *     pwe_range and tnl_range respectively. The physical range
 *     is [range.base, range.base + range.end - range.start].if
 *     the packet does not fulfill either conditions, it is
 *     forwarded to CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_label_ranges_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_MPLS_LABEL_RANGE        *tnl_range,
    SOC_SAND_OUT PCP_OAM_MPLS_LABEL_RANGE        *pwe_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LABEL_RANGES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tnl_range);
  SOC_SAND_CHECK_NULL_INPUT(pwe_range);

  PCP_OAM_MPLS_LABEL_RANGE_clear(tnl_range);
  PCP_OAM_MPLS_LABEL_RANGE_clear(pwe_range);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_label_ranges_get_unsafe()", 0, 0);
}

/*********************************************************************
*     This function sets the configurable FFD rate. Rate is
 *     given by the interval (milliseconds) between two
 *     consecutive frames. Range: 10 - 1270 (denominations of
 *     10ms)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_configurable_ffd_rate_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx,
    SOC_SAND_IN  uint32                       interval
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_UNSAFE);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_configurable_ffd_rate_set_unsafe()", 0, 0);
}

uint32
  pcp_oam_mpls_configurable_ffd_rate_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx,
    SOC_SAND_IN  uint32                       interval
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rate_ndx, PCP_OAM_MPLS_RATE_NDX_MAX, PCP_OAM_MPLS_RATE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(interval, PCP_OAM_MPLS_INTERVAL_MAX, PCP_OAM_MPLS_INTERVAL_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_configurable_ffd_rate_set_verify()", 0, 0);
}

uint32
  pcp_oam_mpls_configurable_ffd_rate_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rate_ndx, PCP_OAM_MPLS_RATE_NDX_MAX, PCP_OAM_MPLS_RATE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_configurable_ffd_rate_get_verify()", 0, 0);
}

/*********************************************************************
*     This function sets the configurable FFD rate. Rate is
 *     given by the interval (milliseconds) between two
 *     consecutive frames. Range: 10 - 1270 (denominations of
 *     10ms)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_configurable_ffd_rate_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx,
    SOC_SAND_OUT uint32                       *interval
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(interval);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_configurable_ffd_rate_get_unsafe()", 0, 0);
}

/*********************************************************************
*     This function configures LSP-source message generator
 *     (CV/FFD/BDI/FDI)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_tx_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_TX_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_tx_info_set_unsafe()", mot_ndx, 0);
}

uint32
  pcp_oam_mpls_lsp_tx_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_TX_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mot_ndx, PCP_OAM_MPLS_MOT_NDX_MAX, PCP_OAM_MPLS_MOT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_MPLS_LSP_TX_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_tx_info_set_verify()", mot_ndx, 0);
}

uint32
  pcp_oam_mpls_lsp_tx_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_TX_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mot_ndx, PCP_OAM_MPLS_MOT_NDX_MAX, PCP_OAM_MPLS_MOT_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_tx_info_get_verify()", mot_ndx, 0);
}

/*********************************************************************
*     This function configures LSP-source message generator
 *     (CV/FFD/BDI/FDI)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_tx_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_TX_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_TX_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_MPLS_LSP_TX_INFO_clear(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_tx_info_get_unsafe()", mot_ndx, 0);
}

/*********************************************************************
*     This function configures LSP-sink
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_rx_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_RX_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_rx_info_set_unsafe()", mor_ndx, 0);
}

uint32
  pcp_oam_mpls_lsp_rx_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_RX_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mor_ndx, PCP_OAM_MPLS_MOR_NDX_MAX, PCP_OAM_MPLS_MOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_MPLS_LSP_RX_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_rx_info_set_verify()", mor_ndx, 0);
}

uint32
  pcp_oam_mpls_lsp_rx_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_RX_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mor_ndx, PCP_OAM_MPLS_MOR_NDX_MAX, PCP_OAM_MPLS_MOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_rx_info_get_verify()", mor_ndx, 0);
}

/*********************************************************************
*     This function configures LSP-sink
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_rx_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_RX_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MPLS_LSP_RX_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_MPLS_LSP_RX_INFO_clear(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_mpls_lsp_rx_info_get_unsafe()", mor_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_oam_api_mpls module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_oam_mpls_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_oam_mpls;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_oam_api_mpls module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_oam_mpls_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_oam_mpls;
}
uint32
  PCP_OAM_MPLS_COS_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_COS_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, PCP_OAM_MPLS_TC_MAX, PCP_OAM_MPLS_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, PCP_OAM_MPLS_DP_MAX, PCP_OAM_MPLS_DP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->exp, PCP_OAM_MPLS_EXP_MAX, PCP_OAM_MPLS_EXP_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MPLS_COS_MAPPING_INFO_verify()",0,0);
}

uint32
  PCP_OAM_MPLS_TTL_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl, PCP_OAM_MPLS_TTL_MAX, PCP_OAM_MPLS_TTL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MPLS_TTL_MAPPING_INFO_verify()",0,0);
}

uint32
  PCP_OAM_MPLS_IPV4_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_IPV4_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MPLS_IPV4_MAPPING_INFO_verify()",0,0);
}

uint32
  PCP_OAM_MPLS_IPV6_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_IPV6_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MPLS_IPV6_MAPPING_INFO_verify()",0,0);
}

uint32
  PCP_OAM_MPLS_IP_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_OAM_MPLS_IPV4_MAPPING_INFO, &(info->ipv4), 10, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_MPLS_IPV6_MAPPING_INFO, &(info->ipv6), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MPLS_IP_MAPPING_INFO_verify()",0,0);
}

uint32
  PCP_OAM_MPLS_LSP_TX_PKT_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_PKT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->system_port, PCP_OAM_MPLS_SYSTEM_PORT_MAX, PCP_OAM_MPLS_SYSTEM_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos, PCP_OAM_MPLS_COS_ID_MAX, PCP_OAM_MPLS_COS_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl, PCP_OAM_MPLS_TTL_ID_MAX, PCP_OAM_MPLS_TTL_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->eep, PCP_OAM_MPLS_EEP_MAX, PCP_OAM_MPLS_EEP_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->lsr_id, PCP_OAM_MPLS_IP_ID_MAX, PCP_OAM_MPLS_IP_ID_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MPLS_LSP_TX_PKT_INFO_verify()",0,0);
}

uint32
  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->defect_type, PCP_OAM_MPLS_DEFECT_TYPE_MAX, PCP_OAM_MPLS_DEFECT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->defect_location, PCP_OAM_MPLS_DEFECT_LOCATION_MAX, PCP_OAM_MPLS_DEFECT_LOCATION_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MPLS_LSP_TX_DEFECT_INFO_verify()",0,0);
}

uint32
  PCP_OAM_MPLS_LSP_TX_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, PCP_OAM_MPLS_TYPE_MAX, PCP_OAM_MPLS_TYPE_OUT_OF_RANGE_ERR, 11, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_MPLS_LSP_TX_PKT_INFO, &(info->packet_info), 12, exit);
  PCP_STRUCT_VERIFY(PCP_OAM_MPLS_LSP_TX_DEFECT_INFO, &(info->defect_info), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MPLS_LSP_TX_INFO_verify()",0,0);
}

uint32
  PCP_OAM_MPLS_LSP_RX_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MPLS_LSP_RX_INFO_verify()",0,0);
}

uint32
  PCP_OAM_MPLS_LABEL_RANGE_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->base, PCP_OAM_MPLS_BASE_MAX, PCP_OAM_MPLS_BASE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->start, PCP_OAM_MPLS_START_MAX, PCP_OAM_MPLS_START_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->end, PCP_OAM_MPLS_END_MAX, PCP_OAM_MPLS_END_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MPLS_LABEL_RANGE_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

