/* $Id: pcp_oam_api_mpls.h,v 1.3 Broadcom SDK $
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

#ifndef __SOC_PCP_OAM_API_MPLS_INCLUDED__
/* { */
#define __SOC_PCP_OAM_API_MPLS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/PCP/pcp_oam_api_general.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

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

/*
 *  MPLS OAM COS (Class Of Service) profile identifier.
 *  Range: 0-7
 */
typedef uint32 PCP_OAM_MPLS_COS_ID;


/*
 *  MPLS OAM TTL (Time To Live) profile identifier. Range:
 *  0-3
 */
typedef uint32 PCP_OAM_MPLS_TTL_ID;


/*
 *  MPLS OAM IP profile identifier. Range: 0-15
 */
typedef uint32 PCP_OAM_MPLS_IP_ID;


typedef enum
{
  /*
   *  Configurable rate
   */
  PCP_OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE = 0,
  /*
   *  Frame (FFD) each 10 ms
   */
  PCP_OAM_MPLS_CC_PKT_TYPE_FFD_10 = 1,
  /*
   *  Frame (FFD) each 20 ms
   */
  PCP_OAM_MPLS_CC_PKT_TYPE_FFD_20 = 2,
  /*
   *  Frame (FFD) each 50 ms
   */
  PCP_OAM_MPLS_CC_PKT_TYPE_FFD_50 = 3,
  /*
   *  Frame (FFD) each 100 ms
   */
  PCP_OAM_MPLS_CC_PKT_TYPE_FFD_100 = 4,
  /*
   *  Frame (FFD) each 200 ms
   */
  PCP_OAM_MPLS_CC_PKT_TYPE_FFD_200 = 5,
  /*
   *  Frame (FFD) each 500 ms
   */
  PCP_OAM_MPLS_CC_PKT_TYPE_FFD_500 = 6,
  /*
   *  Frame (FFD) each 1000 ms
   */
  PCP_OAM_MPLS_CC_PKT_TYPE_FFD_1000 = 7,
  /*
   *  Frame (CV) each 1000 ms
   */
  PCP_OAM_MPLS_CC_PKT_TYPE_CV = 8,
  /*
   *  Number of types in PCP_OAM_MPLS_CC_PKT_TYPE
   */
  PCP_OAM_NOF_MPLS_CC_PKT_TYPES = 9
}PCP_OAM_MPLS_CC_PKT_TYPE;

typedef enum
{
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_SERVER = 1,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_PEER_ME = 2,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_DLOCV = 3,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMATCH = 4,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMERGE = 5,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_EXCESS = 6,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_UNKNOWN = 7,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_NONE = 8,
  /*
   *  Number of types in PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE
   */
  PCP_OAM_NOF_MPLS_LSP_TX_DEFECT_TYPES = 8
}PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE;

typedef enum
{
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_SERVER = 1,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_PEER_ME = 2,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_DLOCV = 3,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_TTSI_MISMATCH = 4,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_TTSI_MISMERGE = 5,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_EXCESS = 6,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_UNKNOWN = 7,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_NONE = 8,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_DISABLE_DEFECT_SET = 14,
  /*
   *  As defined in ITU Y.1711
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_DISABLE_DEFECT_CLEAR = 15,
  /*
   *  Number of types in PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE
   */
  PCP_OAM_NOF_MPLS_LSP_RX_DEFECT_TYPES = 10
}PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  ITMH Traffic class. Range: 0-7
   */
  uint32 tc;
  /*
   *  ITMH drop precedence. Range: 0-3
   */
  uint32 dp;
  /*
   *  May be used as PWE/Tunnel label EXP, depending on the PP
   *  egress configuration. Range: 0-7
   */
  uint32 exp;

} PCP_OAM_MPLS_COS_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  May be used as PWE/Tunnel label TTL, depending on the PP
   *  egress configuration. Range: 0-255
   */
  uint32 ttl;

} PCP_OAM_MPLS_TTL_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  IPv4 address. Used as LSR ID when constructing MPLS OAM
   *  packet
   */
  SOC_SAND_PP_IPV4_ADDRESS ip;

} PCP_OAM_MPLS_IPV4_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  IPv6 address. Used as LSR ID when constructing MPLS OAM
   *  packet
   */
  SOC_SAND_PP_IPV6_ADDRESS ip;

} PCP_OAM_MPLS_IPV6_MAPPING_INFO;

typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  IPv4 address. Used as LSR ID when constructing MPLS OAM
   *  packet
   */
  PCP_OAM_MPLS_IPV4_MAPPING_INFO ipv4;
  /*
   *  IPv6 address. Used as LSR ID when constructing MPLS OAM
   *  packet
   */
  PCP_OAM_MPLS_IPV6_MAPPING_INFO ipv6;

} PCP_OAM_MPLS_IP_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Destination system port to stamp on the ITMH
   */
  uint32 system_port;
  /*
   *  CoS (Class of service) profile. One of eight
   *  preconfigured sets of values
   */
  PCP_OAM_MPLS_COS_ID cos;
  /*
   *  TTL (Time to live) profile. One of four preconfigured
   *  sets of values
   */
  PCP_OAM_MPLS_TTL_ID ttl;
  /*
   *  A pointer to the encapsulation information that should
   *  be appended to the packet by the egress packet processor
   */
  uint32 eep;
  /*
   *  One of 16 preconfigured IPv4/6 addresses
   */
  PCP_OAM_MPLS_IP_ID lsr_id;
  /*
   *  The 16 lsb of the LSP tunnel identifier (16 msb are
   *  global)
   */
  uint16 lsp_id;

} PCP_OAM_MPLS_LSP_TX_PKT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  One of eight possible defect types as defined in ITU-T
   *  Y.1711
   */
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE defect_type;
  /*
   *  The identity of the network in which the defect has been
   *  detected
   */
  uint32 defect_location;
  /*
   *  T - Enable sending of BDI messagesF - Disable sending of
   *  BDI messages
   */
  uint8 bdi_enable;
  /*
   *  T - Enable sending of FDI messagesF - Disable sending of
   *  FDI messages
   */
  uint8 fdi_enable;

} PCP_OAM_MPLS_LSP_TX_DEFECT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  T - Enable connectivity verification transmissionF -
   *  Disable connectivity verification transmission
   */
  uint8 valid;
  /*
   *  One of eight possible preconfigured types
   */
  PCP_OAM_MPLS_CC_PKT_TYPE type;
  /*
   *  Packet generation information
   */
  PCP_OAM_MPLS_LSP_TX_PKT_INFO packet_info;
  /*
   *  Defect indication configuration (enable/disable defect
   *  indications)
   */
  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO defect_info;

} PCP_OAM_MPLS_LSP_TX_INFO;

typedef uint32 PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  T - Entry is valid. Handles incoming CV/FFC messages F -
   *  Entry is invalid. Any incoming CV/FFD message to it is
   *  forwarded to the control-plane
   */
  uint8 valid;
  /*
   *  T - The connectivity verification message received is
   *  expected to be of FFD typeF - The connectivity
   *  verification message received is expected to be of CV
   *  type
   */
  uint8 is_ffd;
  /*
   *  Specified the message defect type to filter. OAM
   *  processor filters FDI message with defect type equals to
   *  the one specified for this field.
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_TYPE fdi_msg_type_to_filter;
  /*
   *  Specified the message defect type to filter. OAM
   *  processor filters BDI message with defect type equals to
   *  the one specified for this field.
   */
  PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_TYPE bdi_msg_type_to_filter;

} PCP_OAM_MPLS_LSP_RX_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Base entry in the table. Range: 0-16K
   */
  uint32 base;
  /*
   *  First label in the range. Range: 0-2^20-1
   */
  uint32 start;
  /*
   *  Last label in the range. Range: 0-2^20-1
   */
  uint32 end;

} PCP_OAM_MPLS_LABEL_RANGE;


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
 *   pcp_oam_mpls_ttl_mapping_info_set
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
  pcp_oam_mpls_ttl_mapping_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO   *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_ttl_mapping_info_set" API.
 *     Refer to "pcp_oam_mpls_ttl_mapping_info_set" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_mpls_ttl_mapping_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_ID             ttl_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_TTL_MAPPING_INFO   *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_ip_mapping_info_set
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
  pcp_oam_mpls_ip_mapping_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx,
    SOC_SAND_IN  uint8                       is_ipv6,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO    *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_ip_mapping_info_set" API.
 *     Refer to "pcp_oam_mpls_ip_mapping_info_set" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_mpls_ip_mapping_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_IP_ID              ip_ndx,
    SOC_SAND_OUT uint8                       *is_ipv6,
    SOC_SAND_OUT PCP_OAM_MPLS_IP_MAPPING_INFO    *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_label_ranges_set
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
  pcp_oam_mpls_label_ranges_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *tnl_range,
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE        *pwe_range
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_label_ranges_set" API.
 *     Refer to "pcp_oam_mpls_label_ranges_set" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_mpls_label_ranges_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_MPLS_LABEL_RANGE        *tnl_range,
    SOC_SAND_OUT PCP_OAM_MPLS_LABEL_RANGE        *pwe_range
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_configurable_ffd_rate_set
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
  pcp_oam_mpls_configurable_ffd_rate_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx,
    SOC_SAND_IN  uint32                       interval
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_configurable_ffd_rate_set" API.
 *     Refer to "pcp_oam_mpls_configurable_ffd_rate_set" API
 *     for details.
*********************************************************************/
uint32
  pcp_oam_mpls_configurable_ffd_rate_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE        rate_ndx,
    SOC_SAND_OUT uint32                       *interval
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_lsp_tx_info_set
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
  pcp_oam_mpls_lsp_tx_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO        *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_lsp_tx_info_set" API.
 *     Refer to "pcp_oam_mpls_lsp_tx_info_set" API for details.
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_tx_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mot_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_TX_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mpls_lsp_rx_info_set
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
  pcp_oam_mpls_lsp_rx_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx,
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO        *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mpls_lsp_rx_info_set" API.
 *     Refer to "pcp_oam_mpls_lsp_rx_info_set" API for details.
*********************************************************************/
uint32
  pcp_oam_mpls_lsp_rx_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        mor_ndx,
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_RX_INFO        *info
  );

void
  PCP_OAM_MPLS_COS_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_COS_MAPPING_INFO *info
  );

void
  PCP_OAM_MPLS_TTL_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_TTL_MAPPING_INFO *info
  );

void
  PCP_OAM_MPLS_IPV4_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_IPV4_MAPPING_INFO *info
  );

void
  PCP_OAM_MPLS_IPV6_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_IPV6_MAPPING_INFO *info
  );

void
  PCP_OAM_MPLS_IP_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_IP_MAPPING_INFO *info
  );

void
  PCP_OAM_MPLS_LSP_TX_PKT_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_TX_PKT_INFO *info
  );

void
  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_TX_DEFECT_INFO *info
  );

void
  PCP_OAM_MPLS_LSP_TX_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_TX_INFO *info
  );

void
  PCP_OAM_MPLS_LSP_RX_INFO_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_LSP_RX_INFO *info
  );

void
  PCP_OAM_MPLS_LABEL_RANGE_clear(
    SOC_SAND_OUT PCP_OAM_MPLS_LABEL_RANGE *info
  );

#if PCP_DEBUG_IS_LVL1

const char*
  PCP_OAM_MPLS_CC_PKT_TYPE_to_string(
    SOC_SAND_IN  PCP_OAM_MPLS_CC_PKT_TYPE enum_val
  );

const char*
  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_to_string(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE enum_val
  );

const char*
  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_to_string(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE enum_val
  );

void
  PCP_OAM_MPLS_COS_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_COS_MAPPING_INFO *info
  );

void
  PCP_OAM_MPLS_TTL_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_TTL_MAPPING_INFO *info
  );

void
  PCP_OAM_MPLS_IPV4_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_IPV4_MAPPING_INFO *info
  );

void
  PCP_OAM_MPLS_IPV6_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_IPV6_MAPPING_INFO *info
  );

void
  PCP_OAM_MPLS_IP_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_IP_MAPPING_INFO *info
  );

void
  PCP_OAM_MPLS_LSP_TX_PKT_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_PKT_INFO *info
  );

void
  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_DEFECT_INFO *info
  );

void
  PCP_OAM_MPLS_LSP_TX_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_TX_INFO *info
  );

void
  PCP_OAM_MPLS_LSP_RX_INFO_print(
    SOC_SAND_IN  PCP_OAM_MPLS_LSP_RX_INFO *info
  );

void
  PCP_OAM_MPLS_LABEL_RANGE_print(
    SOC_SAND_IN  PCP_OAM_MPLS_LABEL_RANGE *info
  );

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_OAM_API_MPLS_INCLUDED__*/
#endif

