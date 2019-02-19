
/* $Id: pcp_frwrd_ipv4.c,v 1.9 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */
 
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_frwrd_ipv4.h>
#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>
#include <soc/dpp/PCP/pcp_lem_access.h>
#include <soc/dpp/PCP/pcp_sw_db.h>
#include <soc/dpp/PCP/pcp_frwrd_ipv4_lpm_mngr.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_reg_access.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_FRWRD_IPV4_ROUTE_TYPES_MAX                       (SOC_PPC_NOF_FRWRD_IP_CACHE_MODES-1)
#define PCP_FRWRD_IPV4_MEM_NDX_MIN                           (1)
#define PCP_FRWRD_IPV4_MEM_NDX_MAX                           (5)
#define PCP_FRWRD_IPV4_ROUTE_VAL_MAX                         (SOC_SAND_U32_MAX)
#define PCP_FRWRD_IPV4_TYPE_MAX                              (PCP_NOF_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPES-1)
#define PCP_FRWRD_IPV4_MC_TABLE_RESOUCES_MAX                 (PCP_NOF_FRWRD_IPV4_MC_HOST_TABLE_RESOURCES-1)
#define PCP_FRWRD_IPV4_EEP_MAX                               (16*1024-1)
#define PCP_FRWRD_IPV4_EEP_MIN                               (1)

#define PCP_GENERAL_ENTRIES_TO_SCAN_MAX                      (SOC_SAND_U32_MAX)
#define PCP_GENERAL_ENTRIES_TO_ACT_MAX                       (SOC_SAND_U32_MAX)
#define PCP_GENERAL_TYPE_MAX                               (PCP_NOF_OUTLIF_ENCODE_TYPES-1)

/*
* build vrf config table index
* {VRF,0} : For IPv4-UC
* {VRF,1} : For IPv4-MC
*/

/* } */
/*************
 * MACROS    *
 *************/
/* { */
#define PCP_FRWRD_IPV4_FEC_TO_DEST(_fec_id_) (0x8000|(_fec_id_)) 
#define PCP_FRWRD_IPV4_FEC_FROM_DEST(_fec_id_) (0x7FFF&(_fec_id_)) 
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
  pcp_procedure_desc_element_frwrd_ipv4[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_COMMIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_COMMIT_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_COMMIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IP_ROUTES_CACHE_COMMIT_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_HOST_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_IS_SUPPORTED_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_MEM_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_MEM_STATUS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_MEM_STATUS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_MEM_DEFRAGE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_MEM_DEFRAGE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_MEM_DEFRAGE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_DEF_ACTION_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_L3_VPN_DEFAULT_ROUTING_ENABLE_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_L3_VPN_DEFAULT_ROUTING_ENABLE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_LPM_ROUTE_ADD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_LPM_ROUTE_IS_EXIST),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_LPM_ROUTE_CLEAR),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_LPM_ROUTE_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_LPM_ROUTE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_LPM_ROUTE_GET_BLOCK),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_LEM_ROUTE_ADD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_LEM_ROUTE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_LEM_ROUTE_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_OR_VPN_ROUTE_ADD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET_BLOCK),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_OR_VPN_ROUTE_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FRWRD_IPV4_UC_OR_VPN_ROUTING_TABLE_CLEAR),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  pcp_error_desc_element_frwrd_ipv4[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_FRWRD_IPV4_ROUTE_TYPES_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_ROUTE_TYPES_OUT_OF_RANGE_ERR",
    "The parameter 'route_types' is out of range. \n\r "
    "The range is: NONE or LPM .\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_SUCCESS_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_ROUTE_STATUS_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_ROUTE_STATUS_OUT_OF_RANGE_ERR",
    "The parameter 'route_status' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_IP_ROUTE_STATUSS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_LOCATION_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_LOCATION_OUT_OF_RANGE_ERR",
    "The parameter 'location' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_FOUND_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_FOUND_OUT_OF_RANGE_ERR",
    "The parameter 'found' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_ROUTES_STATUS_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_ROUTES_STATUS_OUT_OF_RANGE_ERR",
    "The parameter 'routes_status' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_IP_ROUTE_STATUSS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_ROUTES_LOCATION_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_ROUTES_LOCATION_OUT_OF_RANGE_ERR",
    "The parameter 'routes_location' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_EXACT_MATCH_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_EXACT_MATCH_OUT_OF_RANGE_ERR",
    "The parameter 'exact_match' is out of range. \n\r "
    "The range is: 1 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_MEM_NDX_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_MEM_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mem_ndx' is out of range. \n\r "
    "The range is: 0 - 4.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_ROUTE_VAL_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_ROUTE_VAL_OUT_OF_RANGE_ERR",
    "The parameter 'route_val' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_UC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_UC_TABLE_RESOUCES_OUT_OF_RANGE_ERR",
    "The parameter 'uc_table_resouces' is out of range. \n\r "
    "for Soc_petra-B it has to be LPM only, to add to host use host APIs.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_MC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_MC_TABLE_RESOUCES_OUT_OF_RANGE_ERR",
    "The parameter 'mc_table_resouces' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_IPV4_MC_HOST_TABLE_RESOURCES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IP_CACHE_MODE_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IP_CACHE_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'mc_table_resouces' is out of range. \n\r "
    "The range is: 0 - PCP_NOF_FRWRD_IP_CACHE_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_GROUP_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_GROUP_OUT_OF_RANGE_ERR",
    "The parameter 'group' is out of range. \n\r "
    "The range is: 224.0.0.0 - 239.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
   {
    PCP_FRWRD_IPV4_EEP_OUT_OF_RANGE_ERR,
    "PCP_FRWRD_IPV4_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'eep' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,
    "PCP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR",
    "Destination in IPv4 MC routing info can be \n\r"
    "FEC-ptr or MC-group only\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_MUST_ALL_VRF_ERR,
    "PCP_FRWRD_IPV4_MUST_ALL_VRF_ERR",
    "operation can be done for all-VRFs and not one VRF \n\r"
    "set vrf_ndx = PCP_FRWRD_IP_ALL_VRFS_ID\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_CACHE_NOT_SUPPORTED_ERR,
    "PCP_FRWRD_IPV4_CACHE_NOT_SUPPORTED_ERR",
    "caching is not supported (supplied in operation mode)\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_FRWRD_IPV4_DEFRAGE_NOT_SUPPORTED_ERR,
    "PCP_FRWRD_IPV4_DEFRAGE_NOT_SUPPORTED_ERR",
    "defragment is not supported (supplied in operation mode)\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR",
    "The parameter 'entries_to_scan' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR,
    "PCP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR",
    "The parameter 'entries_to_act' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
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
  pcp_frwrd_ipv4_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK,
    vrf;
  PCP_FRWRD_IPV4_GLBL_INFO
    glbl_info;
  PCP_FRWRD_IPV4_VRF_INFO
    vrf_info;
  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint32
    fld_val;
  PCP_REGS
    *regs;
  uint32
    nof_vrfs;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = pcp_sw_db_ipv4_nof_vrfs_get(
          unit,
          &nof_vrfs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);
  if (nof_vrfs == 0)
  {
    PCP_DO_NOTHING_AND_EXIT;
  }

  PCP_FRWRD_IPV4_GLBL_INFO_clear(&glbl_info);
  glbl_info.mc_table_resouces = PCP_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE_TCAM_ONLY;
  glbl_info.uc_table_resouces = PCP_FRWRD_IPV4_HOST_TABLE_RESOURCE_LPM_ONLY;
  glbl_info.router_info.uc_default_action.type = PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC;
  glbl_info.router_info.uc_default_action.value.route_val = 0;
  glbl_info.router_info.mc_default_action.type = PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC;
  glbl_info.router_info.mc_default_action.value.route_val = 0;

  res = pcp_frwrd_ipv4_glbl_info_set_unsafe(
          unit,
          &glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  PCP_FRWRD_IPV4_VRF_INFO_clear(&vrf_info);
  vrf_info.use_dflt_non_vrf_routing = FALSE;
  vrf_info.router_info.uc_default_action.type = PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC;
  vrf_info.router_info.uc_default_action.value.route_val = 0;
  vrf_info.router_info.mc_default_action.type = PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC;
  vrf_info.router_info.mc_default_action.value.route_val = 0;
  for(vrf = 1; vrf <= nof_vrfs; vrf++)
  {
    res = pcp_frwrd_ipv4_vrf_info_set_unsafe(
            unit,
            vrf,
            &vrf_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  for(vrf = 0; vrf < nof_vrfs; ++vrf)
  {
    pcp_ipv4_lpm_mngr_vrf_init(
      &lpm_mngr,
      vrf,
      PCP_FRWRD_IPV4_FEC_TO_DEST(glbl_info.router_info.uc_default_action.value.route_val)
    );
  }

  /* set LPM regs how many bits per bank */
  regs = pcp_regs();

  fld_val = lpm_mngr.init_info.nof_vrf_bits;
  PCP_FLD_SET(regs->elk.lpm_lkp_conf_reg.lpm_key_vrf_size, fld_val , 31, exit);

  fld_val = lpm_mngr.init_info.nof_bits_per_bank[1];
  PCP_FLD_SET(regs->elk.lpm_lkp_conf_reg.lpm_sec_lkp_size, fld_val , 41, exit);

  fld_val = lpm_mngr.init_info.nof_bits_per_bank[2];
  PCP_FLD_SET(regs->elk.lpm_lkp_conf_reg.lpm_thrd_lkp_size, fld_val , 51, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_init_unsafe()", 0, 0);
}

STATIC void
  pcp_ipv4_unicast_mask_ip(
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET       *route_key,
    SOC_SAND_OUT  SOC_SAND_PP_IPV4_SUBNET       *masked_route_key
  )
{
  masked_route_key->prefix_len = route_key->prefix_len;
  if (route_key->prefix_len != 0)
  {

    masked_route_key->ip_address = route_key->ip_address &
      SOC_SAND_BITS_MASK(
        SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN - 1,
        SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN - route_key->prefix_len
      );
  }
  else
  {
    masked_route_key->ip_address = 0;
  }
  return;
}

STATIC
  uint32
    pcp_frwrd_fec_to_em_dest(
      SOC_SAND_IN PCP_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
      SOC_SAND_OUT uint32 *dest,
      SOC_SAND_OUT uint32 *add_info
    )
{
  PCP_FRWRD_DECISION_INFO
    em_dest;
  uint32
    val;
  uint32
    add_info_lcl[1];
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  PCP_FRWRD_DECISION_INFO_clear(&em_dest);
  *add_info = 0;

  em_dest.type = PCP_FRWRD_DECISION_TYPE_FEC;
  em_dest.dest_id = routing_info->fec_id;
  em_dest.additional_info.eei.type = PCP_EEI_TYPE_EMPTY;

  res = pcp_fwd_decision_in_buffer_build(
          PCP_FRWRD_DECISION_APPLICATION_TYPE_IP,
          &em_dest,
          FALSE,
          dest,
          add_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /* set EEP inside the ASD if valid*/
  if (routing_info->eep != PCP_EEP_NULL)
  {
    *add_info_lcl = routing_info->eep;

    /*  identifier is zero */
    /*  asd is outlif */
    val = 3;
    res = soc_sand_bitstream_set_any_field(
            &(val),
            18,
            2,
            add_info_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    /* outlif is EEP */
    val = 0;
    res = soc_sand_bitstream_set_any_field(
            &(val),
            14,
            2,
            add_info_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    *add_info = *add_info_lcl;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_fec_to_em_dest()",0,0);
}

uint32
    pcp_frwrd_ipv4_sand_dest_to_em_dest(
      SOC_SAND_IN int              unit,
      SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *dest_id,
      SOC_SAND_OUT uint32 *dest
    )
{
  PCP_FRWRD_DECISION_INFO
    em_dest;
  uint32
    res,
    add_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  PCP_FRWRD_DECISION_INFO_clear(&em_dest);

  if (dest_id->dest_type == SOC_SAND_PP_DEST_FEC)
  {
    em_dest.type = PCP_FRWRD_DECISION_TYPE_FEC;
    em_dest.dest_id = dest_id->dest_val;
    em_dest.additional_info.eei.type = PCP_EEI_TYPE_EMPTY;
  }
  else if (dest_id->dest_type == SOC_SAND_PP_DEST_MULTICAST)
  {
    em_dest.type = PCP_FRWRD_DECISION_TYPE_MC;
    em_dest.dest_id = dest_id->dest_val;
    em_dest.additional_info.eei.type = PCP_EEI_TYPE_EMPTY;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
  }
  
  res = pcp_fwd_decision_in_buffer_build(
          PCP_FRWRD_DECISION_APPLICATION_TYPE_IP,
          &em_dest,
          FALSE,
          dest,
          &add_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_sand_dest_to_em_dest()",0,0);
}

uint32
    pcp_frwrd_em_dest_to_fec(
      SOC_SAND_IN uint32 dest,
      SOC_SAND_IN  uint32                  asd_buffer,
      SOC_SAND_OUT PCP_FRWRD_IPV4_HOST_ROUTE_INFO *route_info
    )
{
  PCP_FRWRD_DECISION_INFO
    em_dest;
  uint8
    is_sa_drop;
  uint32
    outlif_val=0,
    outlif_type=0;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  PCP_FRWRD_DECISION_INFO_clear(&em_dest);

  res = pcp_fwd_decision_in_buffer_parse(
          PCP_FRWRD_DECISION_APPLICATION_TYPE_IP,
          dest,
          asd_buffer,
          &em_dest,
          &is_sa_drop
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  route_info->fec_id = em_dest.dest_id;
  res = soc_sand_bitstream_get_any_field(
          &(asd_buffer),
          0,
          14,
          &outlif_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(
          &(asd_buffer),
          14,
          10,
          &outlif_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* outlif type in bits: (11)-outlif, 00-reserved, 10-eep */
  if (outlif_type == 0x30 && outlif_val)
  {
    route_info->eep = outlif_val;
  }
  else
  {
    route_info->eep = PCP_EEP_NULL;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_em_dest_to_fec()",0,0);
}


uint32
    pcp_frwrd_ipv4_em_dest_to_sand_dest(
      SOC_SAND_IN int               unit,
      SOC_SAND_IN uint32                dest,
      SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID *dest_id
    )
{
  PCP_FRWRD_DECISION_INFO
    em_dest;
  uint32
    res;
  uint8
    is_sa_drop;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  PCP_FRWRD_DECISION_INFO_clear(&em_dest);

  res = pcp_fwd_decision_in_buffer_parse(
          PCP_FRWRD_DECISION_APPLICATION_TYPE_IP,
          dest,
          0,
          &em_dest,
          &is_sa_drop
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (em_dest.type == PCP_FRWRD_DECISION_TYPE_FEC)
  {
    dest_id->dest_type = SOC_SAND_PP_DEST_FEC;
    dest_id->dest_val = em_dest.dest_id;
  }
  else if (em_dest.type == PCP_FRWRD_DECISION_TYPE_MC)
  {
    dest_id->dest_type = SOC_SAND_PP_DEST_MULTICAST;
    dest_id->dest_val = em_dest.dest_id;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_em_dest_to_sand_dest()",0,0);
}

/* build lem access key for IpV4 host address */
STATIC
  void
    pcp_frwrd_ipv4_host_lem_request_key_build(
      SOC_SAND_IN PCP_LEM_ACCESS_KEY_TYPE type,
      SOC_SAND_IN uint32 vrf_ndx,
      SOC_SAND_IN uint32 dip,
      SOC_SAND_OUT PCP_LEM_ACCESS_KEY *key
    )
{
  uint32
    num_bits;

  PCP_LEM_ACCESS_KEY_clear(key);
  
  key->type = type;

  key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IP_HOST;

  num_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IP_HOST;
  key->param[0].nof_bits = (uint8)num_bits;
  key->param[0].value[0] = dip;
  
  num_bits = PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IP_HOST;
  key->param[1].nof_bits = (uint8)num_bits;
  key->param[1].value[0] = vrf_ndx;

  key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST;
  key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_IP_HOST;
}

/* build lem access key for IpV4 host address */
STATIC
  void
    pcp_frwrd_ipv4_host_lem_request_key_parse(
      SOC_SAND_IN PCP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT PCP_FRWRD_IPV4_HOST_KEY *host_key
    )
{
  host_key->ip_address = key->param[0].value[0];
  host_key->vrf_ndx = key->param[1].value[0];
}

/* build lem access payload for IpV4 host address */
STATIC
  void
    pcp_frwrd_ipv4_host_lem_payload_build(
      SOC_SAND_IN PCP_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
      SOC_SAND_OUT PCP_LEM_ACCESS_PAYLOAD     *payload
    )
{
  PCP_LEM_ACCESS_PAYLOAD_clear(payload);
   
  pcp_frwrd_fec_to_em_dest(
    routing_info,
    &payload->dest,
    &payload->asd
  );
}

/* parse lem access payload for IpV4 host address

  void
    pcp_frwrd_ipv4_host_lem_payload_parse(
      SOC_SAND_IN PCP_LEM_ACCESS_PAYLOAD *payload,
      SOC_SAND_OUT uint32 *fec
    )
{
  pcp_frwrd_em_dest_to_fec(
    payload->dest,
    fec);
}*/



STATIC
  uint32
    pcp_frwrd_ipv4_lpm_route_add(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_IN  uint32                                      fec_id,
      SOC_SAND_IN  uint8                                     is_pending_op,
      SOC_SAND_IN  PCP_FRWRD_IP_ROUTE_LOCATION                 hw_target_type,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
    )
{
  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_SAND_PP_IPV4_SUBNET
    masked_route_key;
  uint32
    res;
  uint8
    success_bool;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_LPM_ROUTE_ADD);

  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  pcp_ipv4_unicast_mask_ip(
    route_key,
    &masked_route_key
  );

  res = pcp_ipv4_lpm_mngr_prefix_add(
          &lpm_mngr,
          vrf_ndx,
          &masked_route_key,
          PCP_FRWRD_IPV4_FEC_TO_DEST(fec_id),
          is_pending_op,
          hw_target_type,
          &success_bool
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  *success = (success_bool == TRUE) ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_lpm_route_add()",0,0);
}

STATIC
  uint32
    pcp_frwrd_ipv4_lpm_vrf_clear(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx
    )
{
  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint8
    pending_op;
  uint32
    res,
    default_fec;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_LPM_ROUTE_CLEAR);
  
  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = pcp_sw_db_ipv4_default_fec_get(
          unit,
          &default_fec
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = pcp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          PCP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &pending_op
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = pcp_ipv4_lpm_mngr_vrf_clear(
          &lpm_mngr,
          vrf_ndx,
          PCP_FRWRD_IPV4_FEC_TO_DEST(default_fec),
          pending_op
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_lpm_vrf_clear()",0,0);
}

STATIC
  uint32
    pcp_frwrd_ipv4_lpm_all_vrfs_clear(
      SOC_SAND_IN  int                                     unit
    )
{
  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint32
    nof_vrfs;
  uint32
    res,
    default_fec;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_LPM_ROUTE_CLEAR);
  
  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = pcp_sw_db_ipv4_default_fec_get(
          unit,
          &default_fec
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = pcp_sw_db_ipv4_nof_vrfs_get(
          unit,
          &nof_vrfs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  res = pcp_ipv4_lpm_mngr_all_vrfs_clear(
          &lpm_mngr,
          nof_vrfs,
          default_fec
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_lpm_all_vrfs_clear()",0,0);
}

STATIC
  uint32
    pcp_frwrd_ipv4_lpm_route_remove(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_IN  uint8                                     is_pending_op,
      SOC_SAND_OUT uint8                                     *success
    )
{
  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_SAND_PP_IPV4_SUBNET
    masked_route_key;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_LPM_ROUTE_REMOVE);
  
  /*
  * cannot remove default.
  */
  if (route_key->prefix_len == 0)
  {
    goto exit;
  }

  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  pcp_ipv4_unicast_mask_ip(
    route_key,
    &masked_route_key
  );

  pcp_ipv4_lpm_mngr_prefix_remove(
          &lpm_mngr,
          vrf_ndx,
          &masked_route_key,
          is_pending_op,
          success
        );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_lpm_route_remove()",0,0);
}


STATIC
  uint32
    pcp_frwrd_ipv4_lpm_route_get(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_IN  uint8                                     exact_match,
      SOC_SAND_OUT uint32                                      *fec_id,
      SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_STATUS         *pending_type,
      SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_LOCATION          *hw_target,
      SOC_SAND_OUT uint8                                     *found
    )
{
  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_SAND_PP_IPV4_SUBNET
    masked_route_key;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_LPM_ROUTE_GET);

  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  pcp_ipv4_unicast_mask_ip(
    route_key,
    &masked_route_key
  );

  res = pcp_ipv4_lpm_mngr_sys_fec_get(
          &lpm_mngr,
          vrf_ndx,
          &masked_route_key,
          exact_match,
          fec_id,
          pending_type,
          hw_target,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  *fec_id = PCP_FRWRD_IPV4_FEC_FROM_DEST(*fec_id);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_lpm_route_get()",0,0);
}

STATIC
  uint32
    pcp_frwrd_ipv4_lpm_route_get_block(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_INOUT  PCP_IP_ROUTING_TABLE_RANGE               *block_range,
      SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_OUT uint32                                      *fec_id,
      SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_STATUS                   *pending_type,
      SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_LOCATION                 *hw_target,
      SOC_SAND_OUT uint32                                      *nof_entries
    )
{

  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_LPM_ROUTE_GET_BLOCK);
  
  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = pcp_ipv4_lpm_mngr_get_block(
          &lpm_mngr,
          vrf_ndx,
          block_range->start.type,
          &block_range->start.payload,
          block_range->entries_to_scan,
          block_range->entries_to_act,
          route_key,
          fec_id,
          pending_type,
          hw_target,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_lpm_route_get_block()",0,0);
}




STATIC
  uint32
    pcp_frwrd_ipv4_lem_route_add(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_ROUTE_INFO              *routing_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
    )
{
  uint32
    res;
  PCP_LEM_ACCESS_REQUEST
    request;
  PCP_LEM_ACCESS_PAYLOAD
    payload;
  PCP_LEM_ACCESS_ACK_STATUS
    ack;


  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_LEM_ROUTE_ADD);
  
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

#ifdef SOC_SAND_DEBUG
  {
     uint8 lem_add_fail = FALSE;
     pcp_sw_db_ipv4_lem_add_fail_get(unit, &lem_add_fail);
     if(lem_add_fail)
    {
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      goto exit;
    }
  }
#endif

  PCP_LEM_ACCESS_REQUEST_clear(&request);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload);
  PCP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = PCP_LEM_ACCESS_CMD_INSERT;
  pcp_frwrd_ipv4_host_lem_request_key_build(
    PCP_LEM_ACCESS_KEY_TYPE_IP_HOST,
    vrf_ndx,
    route_key->ip_address,
    &request.key
  );
  
  
  pcp_frwrd_ipv4_host_lem_payload_build(
    routing_info,
    &payload
  );

  res = pcp_lem_access_entry_add_unsafe(
          unit,
          &request,
          &payload,
          &ack);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
 
  if(ack.is_success)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_lem_route_add()",0,0);

}


STATIC
  uint32
    pcp_frwrd_ipv4_lem_route_get(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_KEY                     *host_key,
      SOC_SAND_OUT  PCP_FRWRD_IPV4_HOST_ROUTE_INFO             *routing_info,
      SOC_SAND_OUT uint8                                     *found
    )
{
  uint32
    res;
  PCP_LEM_ACCESS_REQUEST
    request;
  PCP_LEM_ACCESS_PAYLOAD
    payload;
  PCP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_LEM_ROUTE_GET);
  
  SOC_SAND_CHECK_NULL_INPUT(host_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  PCP_LEM_ACCESS_REQUEST_clear(&request);
  PCP_LEM_ACCESS_PAYLOAD_clear(&payload);
  PCP_LEM_ACCESS_ACK_STATUS_clear(&ack);


  pcp_frwrd_ipv4_host_lem_request_key_build(
    PCP_LEM_ACCESS_KEY_TYPE_IP_HOST,
    host_key->vrf_ndx,
    host_key->ip_address,
    &request.key
  );

  res = pcp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!(*found))
  {
    goto exit;
  }

  res = pcp_frwrd_em_dest_to_fec(
          payload.dest,
          payload.asd,
          routing_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_lem_route_get()",0,0);

}

STATIC
  uint32
    pcp_frwrd_ipv4_lem_route_remove_unsafe(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key
    )
{
  uint32
    res;
  PCP_LEM_ACCESS_REQUEST
    request;
  PCP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_LEM_ROUTE_REMOVE);
  
  SOC_SAND_CHECK_NULL_INPUT(route_key);

  PCP_LEM_ACCESS_REQUEST_clear(&request);
  PCP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = PCP_LEM_ACCESS_CMD_DELETE;
  pcp_frwrd_ipv4_host_lem_request_key_build(
    PCP_LEM_ACCESS_KEY_TYPE_IP_HOST,
    vrf_ndx,
    route_key->ip_address,
    &request.key
  );
  
  res = pcp_lem_access_entry_remove_unsafe(
          unit,
          &request,
          &ack);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_frwrd_ipv4_lem_route_remove()",0,0);

}

STATIC uint32
  pcp_frwrd_ipv4_uc_or_vpn_route_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_IN  uint32                                      fec_id,
    SOC_SAND_IN  uint8                                     is_pending_op,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{

  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_OR_VPN_ROUTE_ADD);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = pcp_frwrd_ipv4_lpm_route_add(
          unit,
          vrf_ndx,
          route_key,
          PCP_FRWRD_IPV4_FEC_TO_DEST(fec_id),
          is_pending_op,
          PCP_FRWRD_IP_ROUTE_LOCATION_LPM,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*success == SOC_SAND_SUCCESS)
  {
    res = pcp_sw_db_ipv4_cache_vrf_modified_set(unit,vrf_ndx,TRUE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_or_vpn_route_add()", 0, 0);
}


STATIC uint32
  pcp_frwrd_ipv4_uc_or_vpn_route_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_IN  uint8                                     exact_match,
    SOC_SAND_OUT uint32                                      *fec_id,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_STATUS         *pending_type,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  SOC_SAND_CHECK_NULL_INPUT(found);
  

  res = pcp_frwrd_ipv4_lpm_route_get(
          unit,
          vrf_ndx,
          route_key,
          exact_match,
          fec_id,
          pending_type,
          location,
          found);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

     
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_or_vpn_route_get()", 0, 0);
}


STATIC uint32
  pcp_frwrd_ipv4_uc_or_vpn_route_get_block_unsafe(
  SOC_SAND_IN  int                                     unit,
  SOC_SAND_IN  uint32                                      vrf_ndx,
    SOC_SAND_INOUT  PCP_IP_ROUTING_TABLE_RANGE                  *block_range,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_OUT uint32                                      *fec_id,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_STATUS         *pending_type,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint32                                      *nof_entries
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET_BLOCK);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = pcp_frwrd_ipv4_lpm_route_get_block(
          unit,
          vrf_ndx,
          block_range,
          route_key,
          fec_id,
          pending_type,
          location,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_or_vpn_route_get_block()", 0, 0);
}


STATIC uint32
  pcp_frwrd_ipv4_uc_or_vpn_route_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_IN  uint8                                     is_pending_op,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    success_bool = TRUE;



  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_OR_VPN_ROUTE_REMOVE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  
  /* non-host route */
  res = pcp_frwrd_ipv4_lpm_route_remove(
          unit,
          vrf_ndx,
          route_key,
          is_pending_op,
          &success_bool
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (success_bool)
  {
    res = pcp_sw_db_ipv4_cache_vrf_modified_set(unit,vrf_ndx,TRUE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  *success = (success_bool == TRUE) ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_or_vpn_route_remove()", 0, 0);
}



STATIC
  uint32
    pcp_frwrd_ipv4_uc_or_vpn_routing_table_clear(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_OR_VPN_ROUTING_TABLE_CLEAR);

  
  res = pcp_frwrd_ipv4_lpm_vrf_clear(
          unit,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_or_vpn_routing_table_clear()", 0, 0);
}


STATIC uint32
  pcp_frwrd_ipv4_vrf_modified_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT  uint8                                 *modified
  )
{
  uint32
    vrf_used_bmp[PCP_SW_IPV4_VRF_BITMAP_SIZE];
  uint32
    tmp,
    indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = pcp_sw_db_ipv4_cache_modified_bitmap_get(
          unit,
          vrf_used_bmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* status of all VRFs*/
  if (vrf_ndx == PCP_FRWRD_IP_ALL_VRFS_ID)
  {
    for (indx = 0 ; indx < PCP_SW_IPV4_VRF_BITMAP_SIZE; ++indx)
    {
      if (vrf_used_bmp[indx]!= 0)
      {
        *modified = TRUE;
        goto exit;
      }
    }
    *modified = FALSE;
  }
  else
  {
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(vrf_used_bmp,vrf_ndx,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    *modified = (uint8)tmp;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_modified_get()", vrf_ndx, 0);
}



/*********************************************************************
*     Set for which IP routes (IPv4/6 UC/MC) to enable caching
 *     by SW
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ip_routes_cache_mode_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                  route_types
  )
{
  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint32
    vrf_used_bmp[PCP_SW_IPV4_VRF_BITMAP_SIZE];
  uint8
    cache;
  uint32
    nof_vrfs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_UNSAFE);

  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!(lpm_mngr.init_info.flags & PCP_LPV4_LPM_SUPPORT_CACHE))
  {
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_IPV4_CACHE_NOT_SUPPORTED_ERR, SOC_SAND_NULL_POINTER_ERR, exit);
  }

  if (vrf_ndx == PCP_FRWRD_IP_ALL_VRFS_ID)
  {
    res = pcp_sw_db_ipv4_nof_vrfs_get(
            unit,
            &nof_vrfs
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    nof_vrfs = 1;
  }

  cache = (uint8)(route_types & PCP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM);
  res = pcp_ipv4_lpm_mngr_cache_set(&lpm_mngr,vrf_ndx,nof_vrfs,cache);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* disable caching will roleback all changes*/
  if (!cache)
  {
    PCP_CLEAR(vrf_used_bmp,uint32,PCP_SW_IPV4_VRF_BITMAP_SIZE);
    res = pcp_sw_db_ipv4_cache_modified_bitmap_set(
            unit,
            vrf_used_bmp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  /* now everything committed update SW-DB */
  res = pcp_sw_db_ipv4_cache_mode_set(unit,route_types);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ip_routes_cache_mode_enable_set_unsafe()", vrf_ndx, 0);
}

uint32
  pcp_frwrd_ip_routes_cache_mode_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                  route_types
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_VERIFY);

  if (vrf_ndx != PCP_FRWRD_IP_ALL_VRFS_ID)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_IPV4_MUST_ALL_VRF_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(route_types, PCP_FRWRD_IPV4_ROUTE_TYPES_MAX, PCP_FRWRD_IPV4_ROUTE_TYPES_OUT_OF_RANGE_ERR, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ip_routes_cache_mode_enable_set_verify()", vrf_ndx, 0);
}

uint32
  pcp_frwrd_ip_routes_cache_mode_enable_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_VERIFY);

  if (vrf_ndx != PCP_FRWRD_IP_ALL_VRFS_ID)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(vrf_ndx, PCP_VRF_ID_MAX, PCP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ip_routes_cache_mode_enable_get_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Set for which IP routes (IPv4/6 UC/MC) to enable caching
 *     by SW
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ip_routes_cache_mode_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT uint32                                  *route_types
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_types);

  res = pcp_sw_db_ipv4_cache_mode_get(unit,route_types);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ip_routes_cache_mode_enable_get_unsafe()", vrf_ndx, 0);
}

/*********************************************************************
*     Synchronize the routing table in the Data plane (HW)
 *     with the routing table in the control plane (SW)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ip_routes_cache_commit_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  route_types,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint8
    changed=0;
  uint32
    vrf_id,
    nof_vrfs,
    vrf_used_bmp[PCP_SW_IPV4_VRF_BITMAP_SIZE];
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IP_ROUTES_CACHE_COMMIT_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);
 /*
  * check if there was any changes
  */
  res = pcp_frwrd_ipv4_vrf_modified_get(
          unit,
          vrf_ndx,
          &changed
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (!changed)
  {
    *success = SOC_SAND_SUCCESS;
    goto exit;
  }
  
  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
    res = pcp_sw_db_ipv4_cache_modified_bitmap_get(
            unit,
            vrf_used_bmp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (vrf_ndx == PCP_FRWRD_IP_ALL_VRFS_ID)
  {
    res = pcp_sw_db_ipv4_nof_vrfs_get(
            unit,
            &nof_vrfs
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
   /*
    * if no change then ignore
    */
    vrf_id = 0;
  }
  else
  {
    nof_vrfs = 1;
    vrf_id = vrf_ndx;
  }

  /* commit all, or commit specific */
  res = pcp_ipv4_lpm_mngr_sync(
          &lpm_mngr,
          vrf_id,
          vrf_used_bmp,
          nof_vrfs,
          TRUE,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (*success!=SOC_SAND_SUCCESS)
  {
    goto exit;
  }
  /* update synced VRFs*/
  if (vrf_ndx == PCP_FRWRD_IP_ALL_VRFS_ID)
  {
    PCP_CLEAR(vrf_used_bmp,uint32,PCP_SW_IPV4_VRF_BITMAP_SIZE);
    
    res = pcp_sw_db_ipv4_cache_modified_bitmap_set(
            unit,
            vrf_used_bmp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  else
  {
    res = pcp_sw_db_ipv4_cache_vrf_modified_set(unit,vrf_ndx,FALSE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ip_routes_cache_commit_unsafe()", vrf_ndx, 0);
}

STATIC uint32
  pcp_frwrd_ipv4_vrf_is_supported_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint8                                 all_permitted
  )
{
  uint32
    nof_vrfs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_IS_SUPPORTED_VERIFY);

  res = pcp_sw_db_ipv4_nof_vrfs_get(
          unit,
          &nof_vrfs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* if permit all VRF-ID, and it's all VRF-IDs then it's OK */
  if (all_permitted && vrf_ndx == PCP_FRWRD_IP_ALL_VRFS_ID)
  {
    goto exit;
  }

  if (vrf_ndx >= nof_vrfs)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_VRF_ID_OUT_OF_RANGE_ERR,101,exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_is_supported_verify()", vrf_ndx, 0);
}


uint32
  pcp_frwrd_ip_routes_cache_commit_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  route_types,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IP_ROUTES_CACHE_COMMIT_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(route_types, PCP_FRWRD_IPV4_ROUTE_TYPES_MAX, PCP_FRWRD_IPV4_ROUTE_TYPES_OUT_OF_RANGE_ERR, 10, exit);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ip_routes_cache_commit_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Setting global information of the IP routing (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_GLBL_INFO                *glbl_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_glbl_info_set_unsafe()", 0, 0);
}



uint32
  pcp_frwrd_ipv4_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_GLBL_INFO_SET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_GLBL_INFO, glbl_info, 10, exit);

  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_glbl_info_set_verify()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_GLBL_INFO_GET_VERIFY);

  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_glbl_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Setting global information of the IP routing (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_IPV4_GLBL_INFO                *glbl_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  PCP_FRWRD_IPV4_GLBL_INFO_clear(glbl_info);

  glbl_info->uc_table_resouces = PCP_FRWRD_IPV4_HOST_TABLE_RESOURCE_LPM_ONLY;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_glbl_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Add IPv4 route entry to the routing table. Binds between
 *     Ipv4 Unicast route key (IPv4-address/prefix) and a FEC
 *     entry identified by fec_id for a given router. As a
 *     result of this operation, Unicast Ipv4 packets
 *     designated to the IP address matching the given key (as
 *     long there is no more-specific route key) will be routed
 *     according to the information in the FEC entry identified
 *     by fec_id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_uc_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  PCP_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    pending_op;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_ROUTE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);
  
  res = pcp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          PCP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &pending_op);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = pcp_frwrd_ipv4_uc_or_vpn_route_add_unsafe(
          unit,
          0,
          &route_key->subnet,
          fec_id,
          pending_op,
          success
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_route_add_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_uc_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  PCP_FEC_ID                              fec_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_ROUTE_ADD_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_UC_ROUTE_KEY, route_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fec_id, PCP_FEC_ID_MAX, PCP_FEC_ID_OUT_OF_RANGE_ERR, 20, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
    &route_key->subnet
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_route_add_verify()", 0, 0);
}

/*********************************************************************
*     Gets the routing information (system-fec-id) associated
 *     with the given route key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_uc_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT PCP_FEC_ID                              *fec_id,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  
  SOC_SAND_CHECK_NULL_INPUT(found);
  
  res = pcp_frwrd_ipv4_uc_or_vpn_route_get_unsafe(
          unit,
          0,
          &route_key->subnet,
          exact_match,
          fec_id,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_route_get_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_uc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_ROUTE_GET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_UC_ROUTE_KEY, route_key, 10, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
          &route_key->subnet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_route_get_verify()", 0, 0);
}

/*********************************************************************
*     Gets the Ipv4 UC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_uc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT PCP_IP_ROUTING_TABLE_RANGE              *block_range,
    SOC_SAND_OUT PCP_FRWRD_IPV4_UC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT PCP_FEC_ID                              *fec_ids,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    i;
  SOC_SAND_PP_IPV4_SUBNET
    *subnets = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(fec_ids);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  subnets = soc_sand_os_malloc(sizeof(SOC_SAND_PP_IPV4_SUBNET) * block_range->entries_to_act,"subnets ipv4_uc_route_get");
  if (subnets == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit);
  }
  soc_sand_os_memset(subnets, 0, sizeof(SOC_SAND_PP_IPV4_SUBNET) * block_range->entries_to_act);

  res = pcp_frwrd_ipv4_uc_or_vpn_route_get_block_unsafe(
          unit,
          0,
          block_range,
          subnets,
          fec_ids,
          routes_status,
          routes_location,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  for(i = 0; i < *nof_entries; i++)
  {
    PCP_FRWRD_IPV4_UC_ROUTE_KEY_clear(&route_keys[i]);
    soc_sand_os_memcpy(&(route_keys[i].subnet), &(subnets[i]), sizeof(SOC_SAND_PP_IPV4_SUBNET));
  }

exit:
  if (subnets)
  {
    soc_sand_os_free(subnets);
  }
  
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_route_get_block_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_uc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT PCP_IP_ROUTING_TABLE_RANGE              *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_VERIFY);

  PCP_STRUCT_VERIFY(PCP_IP_ROUTING_TABLE_RANGE, block_range, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_route_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Remove entry from the routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_uc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    pending_op;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_ROUTE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);
  
  res = pcp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          PCP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &pending_op
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = pcp_frwrd_ipv4_uc_or_vpn_route_remove_unsafe(
          unit,
          0,
          &route_key->subnet,
          pending_op,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_route_remove_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_uc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_ROUTE_REMOVE_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_UC_ROUTE_KEY, route_key, 10, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
          &route_key->subnet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_route_remove_verify()", 0, 0);
}

/*********************************************************************
*     Clear the IPv4 UC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_uc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_UNSAFE);

  res = pcp_frwrd_ipv4_uc_or_vpn_routing_table_clear(
          unit,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_routing_table_clear_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_uc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_VERIFY);

  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_uc_routing_table_clear_verify()", 0, 0);
}

/*********************************************************************
*     Add IPv4 entry to the Host table. Binds between Host and
 *     next hop information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_host_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  )
{
  SOC_SAND_PP_IPV4_SUBNET
    route_key;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_HOST_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(host_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  soc_sand_SAND_PP_IPV4_SUBNET_clear(&route_key);
  route_key.ip_address = host_key->ip_address;
  route_key.prefix_len = 32;

  res = pcp_frwrd_ipv4_lem_route_add(
          unit,
          host_key->vrf_ndx,
          &route_key,
          routing_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_host_add_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_host_add_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_HOST_ADD_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_HOST_KEY, host_key, 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_HOST_ROUTE_INFO, routing_info, 20, exit);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          host_key->vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_host_add_verify()", 0, 0);
}

/*********************************************************************
*     Gets the routing information associated with the given
 *     route key on VRF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_host_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_OUT PCP_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_HOST_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(host_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(found);

  PCP_FRWRD_IPV4_HOST_ROUTE_INFO_clear(routing_info);

  res = pcp_frwrd_ipv4_lem_route_get(
          unit,
          host_key,
          routing_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* for host this is always the status*/
  *route_status = PCP_FRWRD_IP_ROUTE_STATUS_COMMITED;
  *location = PCP_FRWRD_IP_ROUTE_LOCATION_HOST;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_host_get_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_host_get_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_KEY             *host_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_HOST_GET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_HOST_KEY, host_key, 10, exit);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          host_key->vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_host_get_verify()", 0, 0);
}

/*********************************************************************
*     Gets the host table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_host_get_block_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range,
    SOC_SAND_OUT PCP_FRWRD_IPV4_HOST_KEY             *host_keys,
    SOC_SAND_OUT PCP_FRWRD_IPV4_HOST_ROUTE_INFO      *routes_info,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                              *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    num_scanned = 0;
  uint8
    is_found,
    is_valid;
  PCP_LEM_ACCESS_KEY
    key;
  PCP_LEM_ACCESS_PAYLOAD
    payload;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_HOST_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(host_keys);
  SOC_SAND_CHECK_NULL_INPUT(routes_info);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  *nof_entries = 0;


  /*
   *	Loop on the entry index
   */
  for ( num_scanned = 0;
        (num_scanned < block_range->entries_to_scan) &&
         (*nof_entries < block_range->entries_to_act) &&
         (block_range->iter < PCP_LEM_NOF_ENTRIES);
        ++block_range->iter, ++num_scanned)
  {
    PCP_LEM_ACCESS_KEY_clear(&key);
    PCP_LEM_ACCESS_PAYLOAD_clear(&payload);

    res = pcp_lem_access_entry_by_index_get_unsafe(
            unit,
            block_range->iter,
            &key,
            &is_valid
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if ((is_valid == TRUE) && (key.type == PCP_LEM_ACCESS_KEY_TYPE_IP_HOST))
    {
      pcp_frwrd_ipv4_host_lem_request_key_parse(
        &key,
        &host_keys[*nof_entries]
      );

      res = pcp_lem_access_entry_by_key_get_unsafe(
              unit,
              &key,
              &payload,
              &is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

     res = pcp_frwrd_em_dest_to_fec(
              payload.dest,
              payload.asd,
              &routes_info[*nof_entries]
            );
     SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

     routes_status[*nof_entries] = PCP_FRWRD_IP_ROUTE_STATUS_COMMITED;

      (*nof_entries)++;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_host_get_block_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_host_get_block_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_HOST_GET_BLOCK_VERIFY);


  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_host_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Remove IPv4 route entry from the routing table of a
 *     virtual router (VRF).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_host_remove_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_KEY             *host_key
  )
{
  SOC_SAND_PP_IPV4_SUBNET
    route_key;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_HOST_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(host_key);

  soc_sand_SAND_PP_IPV4_SUBNET_clear(&route_key);
  route_key.ip_address = host_key->ip_address;
  route_key.prefix_len = 32;

  res = pcp_frwrd_ipv4_lem_route_remove_unsafe(
          unit,
          host_key->vrf_ndx,
          &route_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_host_remove_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_host_remove_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_KEY             *host_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_HOST_REMOVE_VERIFY);

  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_HOST_KEY, host_key, 10, exit);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          host_key->vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_host_remove_verify()", 0, 0);
}


/*********************************************************************
*     Setting global information of the VRF including
 *     (defaults forwarding).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_vrf_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  PCP_FRWRD_IPV4_VRF_INFO                 *vrf_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vrf_info);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_info_set_unsafe()", vrf_ndx, 0);
}

uint32
  pcp_frwrd_ipv4_vrf_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  PCP_FRWRD_IPV4_VRF_INFO                 *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, PCP_VRF_ID_MIN, PCP_VRF_ID_MAX, PCP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_VRF_INFO, vrf_info, 20, exit);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_info_set_verify()", vrf_ndx, 0);
}

uint32
  pcp_frwrd_ipv4_vrf_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, PCP_VRF_ID_MIN, PCP_VRF_ID_MAX, PCP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_info_get_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Setting global information of the VRF including
 *     (defaults forwarding).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_vrf_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT PCP_FRWRD_IPV4_VRF_INFO                 *vrf_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vrf_info);

  PCP_FRWRD_IPV4_VRF_INFO_clear(vrf_info);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_info_get_unsafe()", vrf_ndx, 0);
}

/*********************************************************************
*     Add IPv4 route entry to the virtual routing table (VRF).
 *     Binds between Ipv4 route key (UC/MC IPv4-address\prefix)
 *     and a FEC entry identified by fec_id for a given virtual
 *     router. As a result of this operation, Unicast Ipv4
 *     packets designated to IP address matching the given key
 *     (as long there is no more-specific route key) will be
 *     routed according to the information in the FEC entry
 *     identified by fec_id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_vrf_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  PCP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  PCP_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    pending_op;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ROUTE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = pcp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          PCP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &pending_op
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = pcp_frwrd_ipv4_uc_or_vpn_route_add_unsafe(
          unit,
          vrf_ndx,
          &route_key->subnet,
          fec_id,
          pending_op,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_route_add_unsafe()", vrf_ndx, 0);
}


uint32
  pcp_frwrd_ipv4_vrf_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  PCP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  PCP_FEC_ID                              fec_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ROUTE_ADD_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, PCP_VRF_ID_MIN, PCP_VRF_ID_MAX, PCP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_VPN_ROUTE_KEY, route_key, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fec_id, PCP_FEC_ID_MAX, PCP_FEC_ID_OUT_OF_RANGE_ERR, 30, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
          &route_key->subnet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_route_add_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Gets the routing information (system-fec-id) associated
 *     with the given route key on VRF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_vrf_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  PCP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT PCP_FEC_ID                              *fec_id,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = pcp_frwrd_ipv4_uc_or_vpn_route_get_unsafe(
          unit,
          vrf_ndx,
          &route_key->subnet,
          exact_match,
          fec_id,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_route_get_unsafe()", vrf_ndx, 0);
}

uint32
  pcp_frwrd_ipv4_vrf_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  PCP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ROUTE_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, PCP_VRF_ID_MIN, PCP_VRF_ID_MAX, PCP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_VPN_ROUTE_KEY, route_key, 20, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
         &route_key->subnet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_route_get_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Gets the routing table of a virtual router (VRF).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_vrf_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT PCP_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT PCP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT PCP_FEC_ID                              *fec_ids,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT PCP_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_PP_IPV4_SUBNET
    *subnets = NULL;
  uint32
    i = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(fec_ids);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  subnets = soc_sand_os_malloc(sizeof(SOC_SAND_PP_IPV4_SUBNET) * block_range_key->entries_to_act,"subnets ipv4_vrf_route");
  if (subnets == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit);
  }
  soc_sand_os_memset(subnets, 0, sizeof(SOC_SAND_PP_IPV4_SUBNET) * block_range_key->entries_to_act);

  res = pcp_frwrd_ipv4_uc_or_vpn_route_get_block_unsafe(
          unit,
          vrf_ndx,
          block_range_key,
          subnets,
          fec_ids,
          routes_status,
          routes_location,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  for(i = 0; i < *nof_entries; i++)
  {
    PCP_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&route_keys[i]);
    soc_sand_os_memcpy(&(route_keys[i].subnet), &(subnets[i]), sizeof(SOC_SAND_PP_IPV4_SUBNET));
  }

exit:
  soc_sand_os_free(subnets);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_route_get_block_unsafe()", vrf_ndx, 0);
}

uint32
  pcp_frwrd_ipv4_vrf_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT PCP_IP_ROUTING_TABLE_RANGE              *block_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, PCP_VRF_ID_MIN, PCP_VRF_ID_MAX, PCP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_IP_ROUTING_TABLE_RANGE, block_range_key, 20, exit);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_route_get_block_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Remove IPv4 route entry from the routing table of a
 *     virtual router (VRF).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_vrf_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  PCP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    pending_op;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ROUTE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = pcp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          PCP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &pending_op
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = pcp_frwrd_ipv4_uc_or_vpn_route_remove_unsafe(
          unit,
          vrf_ndx,
          &route_key->subnet,
          pending_op,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_route_remove_unsafe()", vrf_ndx, 0);
}

uint32
  pcp_frwrd_ipv4_vrf_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  PCP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ROUTE_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, PCP_VRF_ID_MIN, PCP_VRF_ID_MAX, PCP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_VPN_ROUTE_KEY, route_key, 20, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
    &route_key->subnet
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_route_remove_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Clear IPv4 routing table of VRF
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_vrf_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_UNSAFE);

  res = pcp_frwrd_ipv4_uc_or_vpn_routing_table_clear(
          unit,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_routing_table_clear_unsafe()", vrf_ndx, 0);
}

uint32
  pcp_frwrd_ipv4_vrf_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VRF_ID                              vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_VERIFY);

  res = pcp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_routing_table_clear_verify()", vrf_ndx, 0);
}

/*********************************************************************
*     Clear IPv4 routing tables for all VRFs.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_vrf_all_routing_tables_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_UNSAFE);

  res = pcp_frwrd_ipv4_lpm_all_vrfs_clear(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_all_routing_tables_clear_unsafe()", 0, 0);
}

uint32
  pcp_frwrd_ipv4_vrf_all_routing_tables_clear_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_VERIFY);

  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_vrf_all_routing_tables_clear_verify()", 0, 0);
}

/*********************************************************************
*     Returns the status and usage of memory.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_mem_status_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_OUT PCP_FRWRD_IPV4_MEM_STATUS      *mem_status
  )
{
  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  PCP_ARR_MEM_ALLOCATOR_MEM_STATUS      
    pcp_mem_status;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_MEM_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mem_status);

  PCP_ARR_MEM_ALLOCATOR_MEM_STATUS_clear(&pcp_mem_status);
  PCP_FRWRD_IPV4_MEM_STATUS_clear(mem_status);


  PCP_FRWRD_IPV4_MEM_STATUS_clear(mem_status);

  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = pcp_arr_mem_allocator_mem_status_get(&(lpm_mngr.init_info.mem_allocators[mem_ndx]),&pcp_mem_status);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  mem_status->mem_stat.max_free_block_size = pcp_mem_status.max_free_block_size;
  mem_status->mem_stat.nof_fragments = pcp_mem_status.max_free_block_size;
  mem_status->mem_stat.total_free = pcp_mem_status.total_free;
  mem_status->mem_stat.total_size = pcp_mem_status.total_size;

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_mem_status_get_unsafe()", mem_ndx, 0);
}

uint32
  pcp_frwrd_ipv4_mem_status_get_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_MEM_STATUS_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(mem_ndx, PCP_FRWRD_IPV4_MEM_NDX_MIN, PCP_FRWRD_IPV4_MEM_NDX_MAX, PCP_FRWRD_IPV4_MEM_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_mem_status_get_verify()", mem_ndx, 0);
}

/*********************************************************************
*     Returns the status and usage of memory.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_ipv4_mem_defrage_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_IN  PCP_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  )
{
  PCP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  PCP_ARR_MEM_ALLOCATOR_DEFRAG_INFO
    mem_defrag_info;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_MEM_DEFRAGE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(defrag_info);

  res = pcp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  PCP_ARR_MEM_ALLOCATOR_DEFRAG_INFO_clear(&mem_defrag_info);
  PCP_COPY(&mem_defrag_info.block_range,&defrag_info->range,sizeof(SOC_SAND_TABLE_BLOCK_RANGE),1);

  if (!lpm_mngr.init_info.mem_allocators[mem_ndx].support_defragment)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_IPV4_DEFRAGE_NOT_SUPPORTED_ERR, 5, exit);
  }

  res = pcp_arr_mem_allocator_defrag(&(lpm_mngr.init_info.mem_allocators[mem_ndx]),&mem_defrag_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_mem_defrage_unsafe()", mem_ndx, 0);
}

uint32
  pcp_frwrd_ipv4_mem_defrage_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_IN  PCP_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_IPV4_MEM_DEFRAGE_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(mem_ndx, PCP_FRWRD_IPV4_MEM_NDX_MIN, PCP_FRWRD_IPV4_MEM_NDX_MAX, PCP_FRWRD_IPV4_MEM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_MEM_DEFRAG_INFO, defrag_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_ipv4_mem_defrage_verify()", mem_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_api_frwrd_ipv4 module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_frwrd_ipv4_get_procs_ptr(
  )
{
  return pcp_procedure_desc_element_frwrd_ipv4;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_api_frwrd_ipv4 module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_frwrd_ipv4_get_errs_ptr(
  )
{
  return pcp_error_desc_element_frwrd_ipv4;
}
uint32
  PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_verify(
    SOC_SAND_IN  PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->route_val, PCP_FRWRD_IPV4_ROUTE_VAL_MAX, PCP_FRWRD_IPV4_ROUTE_VAL_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_ACTION_PROFILE, &(info->action_profile), 11, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_verify()",0,0);
}

uint32
  PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_verify(
    SOC_SAND_IN  PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, PCP_FRWRD_IPV4_TYPE_MAX, PCP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL, &(info->value), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_verify()",0,0);
}

uint32
  PCP_FRWRD_IPV4_ROUTER_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_IPV4_ROUTER_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION, &(info->uc_default_action), 10, exit);
  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION, &(info->mc_default_action), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_IPV4_ROUTER_INFO_verify()",0,0);
}

uint32
  PCP_FRWRD_IPV4_VRF_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_IPV4_VRF_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_ROUTER_INFO, &(info->router_info), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_IPV4_VRF_INFO_verify()",0,0);
}

uint32
  PCP_FRWRD_IPV4_GLBL_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_IPV4_GLBL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_FRWRD_IPV4_ROUTER_INFO, &(info->router_info), 10, exit);
  if (info->uc_table_resouces != PCP_FRWRD_IPV4_HOST_TABLE_RESOURCE_LPM_ONLY)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_FRWRD_IPV4_UC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,11,exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mc_table_resouces, PCP_FRWRD_IPV4_MC_TABLE_RESOUCES_MAX, PCP_FRWRD_IPV4_MC_TABLE_RESOUCES_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_IPV4_GLBL_INFO_verify()",0,0);
}

uint32
  PCP_FRWRD_IPV4_UC_ROUTE_KEY_verify(
    SOC_SAND_IN  PCP_FRWRD_IPV4_UC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_IPV4_UC_ROUTE_KEY_verify()",0,0);
}


uint32
  PCP_FRWRD_IPV4_VPN_ROUTE_KEY_verify(
    SOC_SAND_IN  PCP_FRWRD_IPV4_VPN_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_IPV4_VPN_ROUTE_KEY_verify()",0,0);
}

uint32
  PCP_FRWRD_IPV4_HOST_KEY_verify(
    SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vrf_ndx, PCP_VRF_ID_MAX, PCP_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_IPV4_HOST_KEY_verify()",info->vrf_ndx,0);
}

uint32
  PCP_FRWRD_IPV4_HOST_ROUTE_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_IPV4_HOST_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fec_id, PCP_FEC_ID_MAX, PCP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  if (info->eep != PCP_EEP_NULL)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->eep, PCP_FRWRD_IPV4_EEP_MIN, PCP_FRWRD_IPV4_EEP_MAX, PCP_FRWRD_IPV4_EEP_OUT_OF_RANGE_ERR, 11, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_IPV4_HOST_ROUTE_INFO_verify()",0,0);
}


uint32
  PCP_FRWRD_IPV4_MEM_DEFRAG_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_IPV4_MEM_DEFRAG_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
 
  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_FRWRD_IPV4_MEM_DEFRAG_INFO_verify()",0,0);
}


uint32
  PCP_IP_ROUTING_TABLE_RANGE_verify(
    SOC_SAND_IN  PCP_IP_ROUTING_TABLE_RANGE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_IP_ROUTING_TABLE_ITER, &(info->start), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entries_to_scan, PCP_GENERAL_ENTRIES_TO_SCAN_MAX, PCP_GENERAL_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entries_to_act, PCP_GENERAL_ENTRIES_TO_ACT_MAX, PCP_GENERAL_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_IP_ROUTING_TABLE_RANGE_verify()",0,0);
}




uint32
  PCP_IP_ROUTING_TABLE_ITER_verify(
    SOC_SAND_IN  PCP_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, PCP_GENERAL_TYPE_MAX, PCP_GENERAL_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_IP_ROUTING_TABLE_ITER_verify()",0,0);
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


