/* $Id: pcp_oam_mpls.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_PCP_OAM_MPLS_INCLUDED__
/* { */
#define __SOC_PCP_OAM_MPLS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PCP/pcp_oam_api_mpls.h>
#include <soc/dpp/PCP/pcp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define PCP_OAM_MPLS_COS_ID_MAX                                (SOC_SAND_U32_MAX)
#define PCP_OAM_MPLS_TTL_ID_MAX                                (SOC_SAND_U32_MAX)
#define PCP_OAM_MPLS_IP_ID_MAX                                 (SOC_SAND_U32_MAX)

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

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_OAM_MPLS_TTL_MAPPING_INFO_SET = PCP_PROC_DESC_BASE_OAM_MPLS_FIRST,
  PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_PRINT,
  PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_UNSAFE,
  PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_VERIFY,
  PCP_OAM_MPLS_TTL_MAPPING_INFO_GET,
  PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_PRINT,
  PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_VERIFY,
  PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_UNSAFE,
  PCP_OAM_MPLS_IP_MAPPING_INFO_SET,
  PCP_OAM_MPLS_IP_MAPPING_INFO_SET_PRINT,
  PCP_OAM_MPLS_IP_MAPPING_INFO_SET_UNSAFE,
  PCP_OAM_MPLS_IP_MAPPING_INFO_SET_VERIFY,
  PCP_OAM_MPLS_IP_MAPPING_INFO_GET,
  PCP_OAM_MPLS_IP_MAPPING_INFO_GET_PRINT,
  PCP_OAM_MPLS_IP_MAPPING_INFO_GET_VERIFY,
  PCP_OAM_MPLS_IP_MAPPING_INFO_GET_UNSAFE,
  PCP_OAM_MPLS_LABEL_RANGES_SET,
  PCP_OAM_MPLS_LABEL_RANGES_SET_PRINT,
  PCP_OAM_MPLS_LABEL_RANGES_SET_UNSAFE,
  PCP_OAM_MPLS_LABEL_RANGES_SET_VERIFY,
  PCP_OAM_MPLS_LABEL_RANGES_GET,
  PCP_OAM_MPLS_LABEL_RANGES_GET_PRINT,
  PCP_OAM_MPLS_LABEL_RANGES_GET_VERIFY,
  PCP_OAM_MPLS_LABEL_RANGES_GET_UNSAFE,
  PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET,
  PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_PRINT,
  PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_UNSAFE,
  PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_VERIFY,
  PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET,
  PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_PRINT,
  PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_VERIFY,
  PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_UNSAFE,
  PCP_OAM_MPLS_LSP_TX_INFO_SET,
  PCP_OAM_MPLS_LSP_TX_INFO_SET_PRINT,
  PCP_OAM_MPLS_LSP_TX_INFO_SET_UNSAFE,
  PCP_OAM_MPLS_LSP_TX_INFO_SET_VERIFY,
  PCP_OAM_MPLS_LSP_TX_INFO_GET,
  PCP_OAM_MPLS_LSP_TX_INFO_GET_PRINT,
  PCP_OAM_MPLS_LSP_TX_INFO_GET_VERIFY,
  PCP_OAM_MPLS_LSP_TX_INFO_GET_UNSAFE,
  PCP_OAM_MPLS_LSP_RX_INFO_SET,
  PCP_OAM_MPLS_LSP_RX_INFO_SET_PRINT,
  PCP_OAM_MPLS_LSP_RX_INFO_SET_UNSAFE,
  PCP_OAM_MPLS_LSP_RX_INFO_SET_VERIFY,
  PCP_OAM_MPLS_LSP_RX_INFO_GET,
  PCP_OAM_MPLS_LSP_RX_INFO_GET_PRINT,
  PCP_OAM_MPLS_LSP_RX_INFO_GET_VERIFY,
  PCP_OAM_MPLS_LSP_RX_INFO_GET_UNSAFE,
  PCP_OAM_MPLS_GET_PROCS_PTR,
  PCP_OAM_MPLS_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  PCP_OAM_MPLS_PROCEDURE_DESC_LAST
} PCP_OAM_MPLS_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_OAM_MPLS_COS_ID_OUT_OF_RANGE_ERR = PCP_ERR_DESC_BASE_OAM_MPLS_FIRST,
  PCP_OAM_MPLS_TTL_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_IP_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_IS_IPV6_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_RATE_NDX_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_INTERVAL_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_MOT_NDX_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_MOR_NDX_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_TC_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_DP_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_EXP_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_TTL_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_SYSTEM_PORT_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_EEP_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_LSP_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_DEFECT_TYPE_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_DEFECT_LOCATION_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_TYPE_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_BASE_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_START_OUT_OF_RANGE_ERR,
  PCP_OAM_MPLS_END_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  PCP_OAM_MPLS_ERR_LAST
} PCP_OAM_MPLS_ERR;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_ttl_mapping_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function maps 2 bit ttl_ndx to TTL information.
 *   This mapping is used to build MPLS OAM packets
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx -
 *     TTL profile identifier. Range: 0-3
 *   SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO   *info -
 *     TTL (time to live) configuration information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_mpls_ttl_mapping_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO   *info
  );

uint32
  pcp_oam_mpls_ttl_mapping_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO   *info
  );

uint32
  pcp_oam_mpls_ttl_mapping_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_ttl_mapping_info_set_unsafe" API.
 *     Refer to "pcp_oam_mpls_ttl_mapping_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  pcp_oam_mpls_ttl_mapping_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_TTL_MAPPING_INFO   *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_ip_mapping_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function maps 4 bit ip_ndx to IPv4/IPv6
 *   information. This mapping is used to determine the LSR
 *   identifier of the constructed MPLS OAM packets
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx -
 *     IP profile identifier. Range: 0-15
 *   SOC_SAND_IN  uint8                       is_ipv6 -
 *     T - The source is IPv6F - The source is IPv4
 *   SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO    *info -
 *     IPv4/IPv6 configuration information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_mpls_ip_mapping_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx,
    SOC_SAND_IN  uint8                       is_ipv6,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO    *info
  );

uint32
  pcp_oam_mpls_ip_mapping_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx,
    SOC_SAND_IN  uint8                       is_ipv6,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO    *info
  );

uint32
  pcp_oam_mpls_ip_mapping_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_ip_mapping_info_set_unsafe" API.
 *     Refer to "pcp_oam_mpls_ip_mapping_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  pcp_oam_mpls_ip_mapping_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx,
    SOC_SAND_OUT uint8                       *is_ipv6,
    SOC_SAND_OUT PCP_OAM_MPLS_IP_MAPPING_INFO    *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_label_ranges_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets label ranges for MPLS OAM. An
 *   incoming MPLS OAM packet is accelerated if in_label (the
 *   first label after the MPLS OAM label, 14) fulfils one of
 *   the following conditions: 1. in_label is between
 *   tnl_range.start and tnl_range.end.2. in_label is between
 *   pwe_range.start and pwe_range.end. pwe_range.base and
 *   tnl_range.base set the physical ranges associated with
 *   pwe_range and tnl_range respectively. The physical range
 *   is [range.base, range.base + range.end - range.start].if
 *   the packet does not fulfill either conditions, it is
 *   forwarded to CPU.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *tnl_range -
 *     Label range.
 *   SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *pwe_range -
 *     Label range.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_mpls_label_ranges_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *tnl_range,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *pwe_range
  );

uint32
  pcp_oam_mpls_label_ranges_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *tnl_range,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *pwe_range
  );

uint32
  pcp_oam_mpls_label_ranges_get_verify(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_label_ranges_set_unsafe" API.
 *     Refer to "pcp_oam_mpls_label_ranges_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_mpls_label_ranges_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_MPLS_LABEL_RANGE        *tnl_range,
    SOC_SAND_OUT PCP_OAM_MPLS_LABEL_RANGE        *pwe_range
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_configurable_ffd_rate_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets the configurable FFD rate. Rate is
 *   given by the interval (milliseconds) between two
 *   consecutive frames. Range: 10 - 1270 (denominations of
 *   10ms)
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx -
 *     The rate index to configure. Currently
 *     PCP_OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE is the only
 *     configurable message type, thus is also the only allowed
 *     index to pass to this function
 *   SOC_SAND_IN  uint32                       interval -
 *     Interval between two consecutive frames in denomination
 *     of 10 ms
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_mpls_configurable_ffd_rate_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx,
    SOC_SAND_IN  uint32                       interval
  );

uint32
  pcp_oam_mpls_configurable_ffd_rate_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx,
    SOC_SAND_IN  uint32                       interval
  );

uint32
  pcp_oam_mpls_configurable_ffd_rate_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_configurable_ffd_rate_set_unsafe" API.
 *     Refer to "pcp_oam_mpls_configurable_ffd_rate_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  pcp_oam_mpls_configurable_ffd_rate_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx,
    SOC_SAND_OUT uint32                       *interval
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_lsp_tx_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function configures LSP-source message generator
 *   (CV/FFD/BDI/FDI)
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                        mot_ndx -
 *     Transmitter index. Range: 0-16K
 *   SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO        *info -
 *     Configuration data
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_tx_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO        *info
  );

uint32
  pcp_oam_mpls_lsp_tx_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO        *info
  );

uint32
  pcp_oam_mpls_lsp_tx_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_lsp_tx_info_set_unsafe" API.
 *     Refer to "pcp_oam_mpls_lsp_tx_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_tx_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_TX_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_lsp_rx_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function configures LSP-sink
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                        mor_ndx -
 *     Offset of the incoming MPLS Label from the base of the
 *     MPLS-labels-range as defined in
 *     pcp_oam_mpls_label_ranges_set(). Range: 0-16K
 *   SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO        *info -
 *     Configuration data
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_rx_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO        *info
  );

uint32
  pcp_oam_mpls_lsp_rx_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO        *info
  );

uint32
  pcp_oam_mpls_lsp_rx_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_lsp_rx_info_set_unsafe" API.
 *     Refer to "pcp_oam_mpls_lsp_rx_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_rx_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_RX_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   pcp_oam_api_mpls module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_oam_mpls_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   pcp_oam_api_mpls module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_oam_mpls_get_errs_ptr(void);

uint32
  PCP_OAM_MPLS_COS_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_COS_MAPPING_INFO *info
  );

uint32
  PCP_OAM_MPLS_TTL_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO *info
  );

uint32
  PCP_OAM_MPLS_IPV4_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_IPV4_MAPPING_INFO *info
  );

uint32
  PCP_OAM_MPLS_IPV6_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_IPV6_MAPPING_INFO *info
  );

uint32
  PCP_OAM_MPLS_IP_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO *info
  );

uint32
  PCP_OAM_MPLS_LSP_TX_PKT_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_PKT_INFO *info
  );

uint32
  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO *info
  );

uint32
  PCP_OAM_MPLS_LSP_TX_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO *info
  );

uint32
  PCP_OAM_MPLS_LSP_RX_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO *info
  );

uint32
  PCP_OAM_MPLS_LABEL_RANGE_verify(
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_OAM_MPLS_INCLUDED__*/
#endif

