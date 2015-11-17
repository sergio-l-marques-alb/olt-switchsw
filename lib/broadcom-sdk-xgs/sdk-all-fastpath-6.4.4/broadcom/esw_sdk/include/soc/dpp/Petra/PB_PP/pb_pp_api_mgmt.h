/* $Id: soc_pb_pp_api_mgmt.h,v 1.6 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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

#ifndef __SOC_PB_PP_API_MGMT_INCLUDED__
/* { */
#define __SOC_PB_PP_API_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PB_PP_MGMT_MPLS_NOF_ETHER_TYPES 2


/* $Id: soc_pb_pp_api_mgmt.h,v 1.6 Broadcom SDK $
 * whether the ipv4 module support caching operation
 * i.e. mode where user can add/remove many routes
 * to software shdaow and commit all changes in one call
 * see soc_ppd_frwrd_ip_routes_cache_mode_enable_set/
 *     soc_ppd_frwrd_ip_routes_cache_commit
 * used in SOC_PB_PP_MGMT_IPV4_INFO.flags
 */
#define  SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE (0x1)
/*
 * whether the ipv4 module support defragement operation
 * in Perta-B, IPv4 lpm includes dynamic memory management
 * This flag declare whether the module supports defragement
 * for these memories
 * see soc_ppd_frwrd_ipv4_mem_defrage
 * used in SOC_PB_PP_MGMT_IPV4_INFO.flags
 */
#define  SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG (0x2)


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
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /* VSI used for mac-in-mac identification,
   *  i.e. packet associated with this VSI will
   *  has Mac-in-MAC processing
   */
  uint32 mim_vsi;
  
} SOC_PB_PP_MGMT_P2P_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   *  If TRUE, then private-VLAN is enabled
   *  split_horizon_filter_enable and pvlan_enable cannot both set to TRUE
   */
  uint8 pvlan_enable;
 /*
  * number of supported VRFs
  * Range 1-256.
  * supported VRF IDs: 0-(nof_vrfs-1)
  */
  uint32 nof_vrfs;
 /*
  * maximum number of routes to support in LPM for IPv4 UC
  * for each VRF.
  * note: this is size to allocate for SW management.
  * maximum number of routes limited also by HW tables.
  */
  uint32 max_routes_in_vrf[SOC_DPP_NOF_VRFS_PETRAB];
 /*
  * if set to TRUE then EEP for host lookup is taken from
  * LEM payload and not from FEC (when FEC. EEP = SOC_PPD_EEP_NULL).
  * if FEC. EEP != SOC_PPD_EEP_NULL then EEP is taken from FEC entry.
  */
  uint8 ipv4_host_extend;
 /*
  * flags for ipv4 management
  * see SOC_PB_PP_MGMT_IPV4_OP_MODE
  */
  uint32 flags;

  
} SOC_PB_PP_MGMT_IPV4_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, then SA-authentication is enabled
   */
  uint8 authentication_enable;
  /*
   *  If TRUE, then the System-VSI is enabled
   *  PB: ignored.
   */
  uint8 system_vsi_enable;
  /*
   *  If TRUE, then the Hairpin is enabled
   *  PB: ignored.
   */
  uint8 hairpin_enable;
  /*
   *  If TRUE, then the split_horizon_filter for ACs is enabled
   *  when true then split-horizon is performed for in-AC vs out-AC
   *  if false then split-horizon is performed according to EEP/port.
   *  split_horizon_filter_enable and pvlan_enable cannot both set to TRUE
   */
  uint8 split_horizon_filter_enable;
  /*
   *  MPLS ether types
   */
  SOC_SAND_PP_ETHER_TYPE mpls_ether_types[SOC_PB_PP_MGMT_MPLS_NOF_ETHER_TYPES];
  /*
   *  IPV4 info
   */
  SOC_PB_PP_MGMT_IPV4_INFO ipv4_info;
  /*
   *  P2P init info
   */
  SOC_PB_PP_MGMT_P2P_INFO p2p_info;

}SOC_PB_PP_MGMT_OPERATION_MODE;

typedef enum
{
  /*
   *  External lookups are disabled.
   */
  SOC_PB_PP_MGMT_ELK_MODE_NONE = 0,
  /*
   *  External lookup is set to Soc_petra-B rev. A1 mode.
   */
  SOC_PB_PP_MGMT_ELK_MODE_NORMAL = 1,
  /*
   *  External lookup uses short records exclusively
   *  (available in Soc_petra-B rev. B0 only).
   */
  SOC_PB_PP_MGMT_ELK_MODE_B0_SHORT = 2,
  /*
   *  External lookup uses long records exclusively (available
   *  in Soc_petra-B rev. B0 only).
   */
  SOC_PB_PP_MGMT_ELK_MODE_B0_LONG = 3,
  /*
   *  External lookup uses both short and long records
   *  (available in Soc_petra-B rev. B0 only).
   */
  SOC_PB_PP_MGMT_ELK_MODE_B0_BOTH = 4,
  /*
   *  Number of types in SOC_PB_PP_MGMT_ELK_MODE
   */
  SOC_PB_PP_NOF_MGMT_ELK_MODES = 5
}SOC_PB_PP_MGMT_ELK_MODE;

typedef enum
{
  /*
   *  Transparent P2P service.
   */
  SOC_PB_PP_MGMT_LKP_TYPE_P2P = 0,
  /*
   *  Ethernet bridging.
   */
  SOC_PB_PP_MGMT_LKP_TYPE_ETH = 1,
  /*
   *  TRILL unicast forwarding.
   */
  SOC_PB_PP_MGMT_LKP_TYPE_TRILL_UC = 2,
  /*
   *  TRILL multicast forwarding.
   */
  SOC_PB_PP_MGMT_LKP_TYPE_TRILL_MC = 3,
  /*
   *  IPv4 unicast forwarding.
   */
  SOC_PB_PP_MGMT_LKP_TYPE_IPV4_UC = 4,
  /*
   *  IPv4 multicast forwarding.
   */
  SOC_PB_PP_MGMT_LKP_TYPE_IPV4_MC = 5,
  /*
   *  IPv6 unicast forwarding.
   */
  SOC_PB_PP_MGMT_LKP_TYPE_IPV6_UC = 6,
  /*
   *  IPv6 multicast forwarding.
   */
  SOC_PB_PP_MGMT_LKP_TYPE_IPV6_MC = 7,
  /*
   *  MPLS forwarding.
   */
  SOC_PB_PP_MGMT_LKP_TYPE_LSR = 8,
  /*
   *  Number of types in SOC_PB_PP_MGMT_LKP_TYPE
   */
  SOC_PB_PP_NOF_MGMT_LKP_TYPES = 9
}SOC_PB_PP_MGMT_LKP_TYPE;

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
*     soc_pb_pp_mgmt_operation_mode_set
* TYPE:
*   PROC
* FUNCTION:
*     Set soc_petra_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE *op_mode -
*     soc_petra_pp device operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mgmt_operation_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE *op_mode
  );

/*********************************************************************
* NAME:
*     soc_pb_pp_mgmt_operation_mode_get
* TYPE:
*   PROC
* FUNCTION:
*     Set soc_petra_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PB_PP_MGMT_OPERATION_MODE *op_mode -
*     soc_petra_pp device operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mgmt_operation_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PB_PP_MGMT_OPERATION_MODE *op_mode
  );

/*********************************************************************
* NAME:
*     soc_pb_pp_mgmt_device_close
* TYPE:
*   PROC
* FUNCTION:
*     Close the Device, and clean HW and SW.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to close.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mgmt_device_close(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mgmt_elk_mode_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the ELK interface mode.
 * INPUT:
 *   SOC_SAND_IN  int           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_MGMT_ELK_MODE elk_mode -
 *     The requested operation mode for the ELK interface.
 *   SOC_SAND_OUT uint32           *ingress_pkt_rate -
 *     The effective processing rate of the ingress device in
 *     packets per second.
 * REMARKS:
 *   1. The ELK's physical interface has to be configured
 *   before calling this function. See soc_pb_nif_elk_set().2.
 *   Soc_petra-B rev. B0 modes are not available on earlier
 *   revisions.3. The ingress device's rate is limited by the
 *   processing rate of the ELK interface, affecting all
 *   packets entering the device.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mgmt_elk_mode_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_ELK_MODE elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mgmt_elk_mode_set" API.
 *     Refer to "soc_pb_pp_mgmt_elk_mode_set" API for details.
*********************************************************************/
uint32
  soc_pb_pp_mgmt_elk_mode_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT SOC_PB_PP_MGMT_ELK_MODE *elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mgmt_use_elk_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Determine whether the specified lookup is externalized
 *   or not.
 * INPUT:
 *   SOC_SAND_IN  int           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_MGMT_LKP_TYPE lkp_type -
 *     The lookup type in question.
 *   SOC_SAND_IN  uint8           use_elk -
 *     Whether to use the ELK for that lookup or not.
 * REMARKS:
 *   IPv4 multicast and IPv6 lookups can only be performed
 *   externally in Soc_petra-B rev. B0 modes.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mgmt_use_elk_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mgmt_use_elk_set" API.
 *     Refer to "soc_pb_pp_mgmt_use_elk_set" API for details.
*********************************************************************/
uint32
  soc_pb_pp_mgmt_use_elk_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_OUT uint8           *use_elk
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mgmt_mac_in_mac_enable
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Mac-in-Mac TPID profile. This also shows Mac-in_mac is
 *   enbled on the device.
 * INPUT:
 *   SOC_SAND_IN  int           unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   none.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mgmt_mac_in_mac_enable(
    SOC_SAND_IN  int           unit
  );

void
  SOC_PB_PP_MGMT_OPERATION_MODE_clear(
    SOC_SAND_OUT SOC_PB_PP_MGMT_OPERATION_MODE *info
  );

void
  SOC_PB_PP_MGMT_P2P_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_MGMT_P2P_INFO *info
  );
void
  SOC_PB_PP_MGMT_IPV4_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_MGMT_IPV4_INFO *info
  );


/* } */


#if SOC_PB_PP_DEBUG_IS_LVL1
void
  SOC_PB_PP_MGMT_OPERATION_MODE_print(
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE *info
  );

void
  SOC_PB_PP_MGMT_P2P_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MGMT_P2P_INFO *info
  );

void
  SOC_PB_PP_MGMT_IPV4_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MGMT_IPV4_INFO *info
  );

const char*
  SOC_PB_PP_MGMT_ELK_MODE_to_string(
    SOC_SAND_IN  SOC_PB_PP_MGMT_ELK_MODE enum_val
  );

const char*
  SOC_PB_PP_MGMT_LKP_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_MGMT_LKP_TYPE enum_val
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_MGMT_INCLUDED__*/
#endif
