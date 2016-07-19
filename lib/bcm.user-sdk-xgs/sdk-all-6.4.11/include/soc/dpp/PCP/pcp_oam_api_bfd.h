/* $Id: pcp_oam_api_bfd.h,v 1.3 Broadcom SDK $
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

#ifndef __SOC_PCP_OAM_API_BFD_INCLUDED__
/* { */
#define __SOC_PCP_OAM_API_BFD_INCLUDED__

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
 *  BFD COS profile identifier. Range: 0-7
 */
typedef uint32 PCP_OAM_BFD_COS_ID;


/*
 *  BFD TTL profile identifier. Range: 0-3
 */
typedef uint32 PCP_OAM_BFD_TTL_ID;


/*
 *  BFD IP profile identifier. Range: 0-3
 */
typedef uint32 PCP_OAM_BFD_IP_ID;


typedef enum
{
  /*
   *  Tunnel label, routable source IP, destination IP 127/8,
   *  protocol UDP, UDP port 3784, BFD PDU
   */
  PCP_OAM_BFD_TYPE_MPLS = 0,
  /*
   *  Tunnel label, PWE label, CW 0x21, routable source IP,
   *  destination IP, protocol UDP, UDP port 3784, BFD PDU
   */
  PCP_OAM_BFD_TYPE_MPLS_PWE = 1,
  /*
   *  Tunnel label, PWE label, CW 0x7, BFD PDU
   */
  PCP_OAM_BFD_TYPE_MPLS_PWE_WITH_IP = 2,
  /*
   *  Routable source IP, routable destination IP, protocol
   *  UDP, UDP port 3784, BFD PDU
   */
  PCP_OAM_BFD_TYPE_IP = 3,
  /*
   *  Number of types in PCP_OAM_BFD_TYPE
   */
  PCP_OAM_NOF_BFD_TYPES = 4
}PCP_OAM_BFD_TYPE;

typedef enum
{
  /*
   *  1 PPS (every 1000 ms)
   */
  PCP_OAM_BFD_TX_RATE_1 = 0,
  /*
   *  2 PPS (every 500 ms)
   */
  PCP_OAM_BFD_TX_RATE_2 = 1,
  /*
   *  5 PPS (every 200 ms)
   */
  PCP_OAM_BFD_TX_RATE_5 = 2,
  /*
   *  10 PPS (every 100 ms)
   */
  PCP_OAM_BFD_TX_RATE_10 = 3,
  /*
   *  20 PPS (every 50 ms)
   */
  PCP_OAM_BFD_TX_RATE_20 = 4,
  /*
   *  50 PPS (every 20 ms)
   */
  PCP_OAM_BFD_TX_RATE_50 = 5,
  /*
   *  100 PPS (every 10 ms)
   */
  PCP_OAM_BFD_TX_RATE_100 = 6,
  /*
   *  Number of types in PCP_OAM_BFD_TX_RATE
   */
  PCP_OAM_NOF_BFD_TX_RATES = 7
}PCP_OAM_BFD_TX_RATE;

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
   *  Tunnel EXP. Range: 0-7
   */
  uint32 tnl_exp;
  /*
   *  PWE label EXP. Range: 0-7
   */
  uint32 pwe_exp;

} PCP_OAM_BFD_COS_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Tunnel TTL. Range: 0-255
   */
  uint32 tnl_ttl;
  /*
   *  PWE TTL. Range: 0-255
   */
  uint32 pwe_ttl;

} PCP_OAM_BFD_TTL_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Source IPv4 address for BFD messages of types MPLS, PWE
   *  with IP, IP
   */
  SOC_SAND_PP_IPV4_ADDRESS ip;

} PCP_OAM_BFD_IP_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Detection time multiplier. The negotiated transmit
   *  interval, multiplied by this value, provides the
   *  Detection Time (timeout) for the BFD Monitoring process
   */
  uint32 detect_mult;
  /*
   *  One of eight possible rates. This is the minimum
   *  interval, in microseconds, between received BFD Control
   *  packets that this system is capable of supporting
   */
  PCP_OAM_BFD_TX_RATE min_rx_interval;
  /*
   *  One of eight possible rates. This is the minimum
   *  interval, in microseconds that the local system would
   *  like to use when transmitting BFD Control packets
   */
  PCP_OAM_BFD_TX_RATE min_tx_interval;

} PCP_OAM_BFD_NEGOTIATION_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  A pointer to the encapsulation information that should
   *  be appended to the packet by the egress packet processor
   */
  uint32 eep;
  /*
   *  CoS (Class of service) profile. One of eight
   *  preconfigured sets of values
   */
  PCP_OAM_BFD_COS_ID cos;
  /*
   *  TTL (time to live) profile. One of four preconfigured
   *  sets of values
   */
  PCP_OAM_BFD_TTL_ID ttl;

} PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  A pointer to the encapsulation information that should
   *  be appended to the packet by the egress packet processor
   */
  uint32 eep;
  /*
   *  PWE label to append to the packet. CW 0x21 is
   *  automatically appended
   */
  uint32 pwe;
  /*
   *  CoS (Class of service) profile. One of eight
   *  preconfigured sets of values
   */
  PCP_OAM_BFD_COS_ID cos;
  /*
   *  TTL (time to live) profile. One of four preconfigured
   *  sets of values
   */
  PCP_OAM_BFD_TTL_ID ttl;

} PCP_OAM_BFD_FWD_MPLS_PWE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  One of four preconfigured IPv4 source addresses
   */
  PCP_OAM_BFD_IP_ID src_ip;
  /*
   *  A pointer to the encapsulation information that should
   *  be appended to the packet by the egress packet processor
   */
  uint32 eep;
  /*
   *  PWE label to append to the packet. CW 0x7 is
   *  automatically appended
   */
  uint32 pwe;
  /*
   *  Class of service of transmitted packet. Mapped to
   *  traffic class and drop precedence in ITMH
   */
  uint32 cos;
  /*
   *  TTL (time to live) profile. One of four preconfigured
   *  sets of values
   */
  PCP_OAM_BFD_TTL_ID ttl;

} PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  One of four preconfigured IPv4 source addresses
   */
  uint32 src_ip;
  /*
   *  IPv4 destination address
   */
  SOC_SAND_PP_IPV4_ADDRESS dst_ip;
  /*
   *  Class of service of transmitted packet. Mapped to
   *  traffic class and drop precedence in ITMH
   */
  uint32 cos;
  /*
   *  TTL (time to live) profile. One of four preconfigured
   *  sets of values
   */
  PCP_OAM_BFD_TTL_ID ttl;

} PCP_OAM_BFD_FWD_IP_INFO;

typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Forwarding information for MPLS tunnel protection
   */
  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO mpls_tunnel_info;
  /*
   *  Forwarding information for PWE protection
   */
  PCP_OAM_BFD_FWD_MPLS_PWE_INFO mpls_pwe_info;
  /*
   *  Forwarding information for PWE protection (with IP)
   */
  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO mpls_pwe_with_ip_info;
  /*
   *  Forwarding information for IP protection
   */
  PCP_OAM_BFD_FWD_IP_INFO ip_info;

} PCP_OAM_BFD_FWD_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  T - Enable OAMP message generation for this sessionF -
   *  Disable OAMP message generation for this session
   */
  uint8 valid;
  /*
   *  100/50/20/10/5/2/1 PPS (every 10/20/50/100/200/500/1000
   *  ms)The scheduler scans the table every period. For 16K
   *  table and 5ns clock cycle this yields 0.5ms accuracy
   *  which is 1/20 of the interval
   */
  PCP_OAM_BFD_TX_RATE tx_rate;
  /*
   *  The BFD partner's discriminator
   */
  uint32 discriminator;
  /*
   *  One of four BFD packet types
   */
  PCP_OAM_BFD_TYPE bfd_type;
  /*
   *  Negotiation information
   */
  PCP_OAM_BFD_NEGOTIATION_INFO ng_info;
  /*
   *  Forwarding information
   */
  PCP_OAM_BFD_FWD_INFO fw_info;

} PCP_OAM_BFD_TX_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Calculated timeout for BFD session.
   */
  uint32 life_time;
  /*
   *  T - All BFD packets on this session are forwarded to the
   *  CPU. F - This BFD session is accelerated.
   */
  uint8 defect;

} PCP_OAM_BFD_RX_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Base entry in the table. Range: 0-8K
   */
  uint32 base;
  /*
   *  Start of discriminator range Range: 0-232-1
   */
  uint32 start;
  /*
   *  Start of discriminator range Range: 0-232-1
   */
  uint32 end;

} PCP_OAM_BFD_DISCRIMINATOR_RANGE;


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
 *   pcp_oam_bfd_ttl_mapping_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function maps 2 bit ttl_ndx to TTL information.
 *   This mapping is used to build BFD packets. Sets the TTL
 *   of the MPLS tunnel label, and in case PWE protection is
 *   applied, also sets the MPLS PWE label TTL.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx -
 *     TTL profile identifier. Range: 0-3
 *   SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO    *info -
 *     TTL (time to live) configuration information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_bfd_ttl_mapping_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO    *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_bfd_ttl_mapping_info_set" API.
 *     Refer to "pcp_oam_bfd_ttl_mapping_info_set" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_bfd_ttl_mapping_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_TTL_ID              ttl_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_TTL_MAPPING_INFO    *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_bfd_ip_mapping_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets one of four IPv4 possible source
 *   addresses. These addresses are used by the OAMP to
 *   generate BFD messages of types MPLS, PWE with IP, IP.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx -
 *     IP profile identifier. Range 0-3
 *   SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO     *info -
 *     IPv4/IPv6 address to configure
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_bfd_ip_mapping_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO     *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_bfd_ip_mapping_info_set" API.
 *     Refer to "pcp_oam_bfd_ip_mapping_info_set" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_bfd_ip_mapping_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_IP_ID               ip_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_IP_MAPPING_INFO     *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_bfd_my_discriminator_range_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets the range of the local accelerated
 *   discriminators. The value of disc_range.start is
 *   subtracted from the your_discriminator field of the
 *   incoming BFD message, to determine the entry offset to
 *   access from disc_range.base. The physical range is
 *   [disc_range.base, disc_range.base + disc_range.end -
 *   disc_range.start]. If the packet does not match the
 *   range, it is forwarded to CPU.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range -
 *     The range of accelerated discriminators.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_bfd_my_discriminator_range_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_bfd_my_discriminator_range_set" API.
 *     Refer to "pcp_oam_bfd_my_discriminator_range_set" API
 *     for details.
*********************************************************************/
uint32
  pcp_oam_bfd_my_discriminator_range_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_BFD_DISCRIMINATOR_RANGE *disc_range
  );

/*********************************************************************
* NAME:
 *   pcp_oam_bfd_tx_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function configures/updates BFD message
 *   transmission information.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                        my_discriminator_ndx -
 *     Discriminator index. Must be in accelerated
 *     discriminator range (see
 *     pcp_oam_bfd_my_discriminator_range_set for more
 *     information)
 *   SOC_SAND_IN  PCP_OAM_BFD_TX_INFO             *info -
 *     BFD message transmission information for the specified
 *     discriminator index
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_bfd_tx_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_TX_INFO             *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_bfd_tx_info_set" API.
 *     Refer to "pcp_oam_bfd_tx_info_set" API for details.
*********************************************************************/
uint32
  pcp_oam_bfd_tx_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_TX_INFO             *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_bfd_rx_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function configures/updates BFD message reception
 *   and session monitoring information.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                        my_discriminator_ndx -
 *     Discriminator index. Must be in accelerated
 *     discriminator range (see
 *     pcp_oam_bfd_my_discriminator_range_set for more
 *     information)
 *   SOC_SAND_IN  PCP_OAM_BFD_RX_INFO             *info -
 *     BFD message reception and session monitoring information
 *     for the specified discriminator index
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_bfd_rx_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_IN  PCP_OAM_BFD_RX_INFO             *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_bfd_rx_info_set" API.
 *     Refer to "pcp_oam_bfd_rx_info_set" API for details.
*********************************************************************/
uint32
  pcp_oam_bfd_rx_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        my_discriminator_ndx,
    SOC_SAND_OUT PCP_OAM_BFD_RX_INFO             *info
  );

void
  PCP_OAM_BFD_COS_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_COS_MAPPING_INFO *info
  );

void
  PCP_OAM_BFD_TTL_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_TTL_MAPPING_INFO *info
  );

void
  PCP_OAM_BFD_IP_MAPPING_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_IP_MAPPING_INFO *info
  );

void
  PCP_OAM_BFD_NEGOTIATION_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_NEGOTIATION_INFO *info
  );

void
  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO *info
  );

void
  PCP_OAM_BFD_FWD_MPLS_PWE_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_FWD_MPLS_PWE_INFO *info
  );

void
  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO *info
  );

void
  PCP_OAM_BFD_FWD_IP_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_FWD_IP_INFO *info
  );

void
  PCP_OAM_BFD_FWD_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_FWD_INFO *info
  );

void
  PCP_OAM_BFD_TX_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_TX_INFO *info
  );

void
  PCP_OAM_BFD_RX_INFO_clear(
    SOC_SAND_OUT PCP_OAM_BFD_RX_INFO *info
  );

void
  PCP_OAM_BFD_DISCRIMINATOR_RANGE_clear(
    SOC_SAND_OUT PCP_OAM_BFD_DISCRIMINATOR_RANGE *info
  );

#if PCP_DEBUG_IS_LVL1

const char*
  PCP_OAM_BFD_TYPE_to_string(
    SOC_SAND_IN  PCP_OAM_BFD_TYPE enum_val
  );

const char*
  PCP_OAM_BFD_TX_RATE_to_string(
    SOC_SAND_IN  PCP_OAM_BFD_TX_RATE enum_val
  );

void
  PCP_OAM_BFD_COS_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_COS_MAPPING_INFO *info
  );

void
  PCP_OAM_BFD_TTL_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_TTL_MAPPING_INFO *info
  );

void
  PCP_OAM_BFD_IP_MAPPING_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_IP_MAPPING_INFO *info
  );

void
  PCP_OAM_BFD_NEGOTIATION_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_NEGOTIATION_INFO *info
  );

void
  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_TUNNEL_INFO *info
  );

void
  PCP_OAM_BFD_FWD_MPLS_PWE_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_PWE_INFO *info
  );

void
  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_MPLS_PWE_WITH_IP_INFO *info
  );

void
  PCP_OAM_BFD_FWD_IP_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_IP_INFO *info
  );

void
  PCP_OAM_BFD_FWD_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_FWD_INFO *info
  );

void
  PCP_OAM_BFD_TX_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_TX_INFO *info
  );

void
  PCP_OAM_BFD_RX_INFO_print(
    SOC_SAND_IN  PCP_OAM_BFD_RX_INFO *info
  );

void
  PCP_OAM_BFD_DISCRIMINATOR_RANGE_print(
    SOC_SAND_IN  PCP_OAM_BFD_DISCRIMINATOR_RANGE *info
  );

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_OAM_API_BFD_INCLUDED__*/
#endif

