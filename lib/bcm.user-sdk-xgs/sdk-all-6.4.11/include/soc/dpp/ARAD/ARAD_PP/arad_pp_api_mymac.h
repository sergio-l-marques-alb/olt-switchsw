/* $Id: arad_pp_api_mymac.h,v 1.5 Broadcom SDK $
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

#ifndef __ARAD_PP_API_MYMAC_INCLUDED__
/* { */
#define __ARAD_PP_API_MYMAC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_mymac.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

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

#define ARAD_PP_MYMAC_VRRP_MODE_ALL_VSI_BASED                SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED
#define ARAD_PP_MYMAC_VRRP_MODE_256_VSI_BASED                SOC_PPC_MYMAC_VRRP_MODE_256_VSI_BASED
#define ARAD_PP_MYMAC_VRRP_MODE_512_VSI_BASED                SOC_PPC_MYMAC_VRRP_MODE_512_VSI_BASED
#define ARAD_PP_MYMAC_VRRP_MODE_1K_VSI_BASED                 SOC_PPC_MYMAC_VRRP_MODE_1K_VSI_BASED
#define ARAD_PP_MYMAC_VRRP_MODE_2K_VSI_BASED                 SOC_PPC_MYMAC_VRRP_MODE_2K_VSI_BASED
#define ARAD_PP_NOF_MYMAC_VRRP_MODES                         SOC_PPC_NOF_MYMAC_VRRP_MODES
typedef SOC_PPC_MYMAC_VRRP_MODE                                ARAD_PP_MYMAC_VRRP_MODE;

typedef SOC_PPC_MYMAC_VRRP_INFO                                ARAD_PP_MYMAC_VRRP_INFO;
typedef SOC_PPC_MYMAC_TRILL_INFO                               ARAD_PP_MYMAC_TRILL_INFO;

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
 *   arad_pp_mymac_msb_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the MSB of the MAC address of the device. Used for
 *   ingress termination and egress encapsulation.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_msb -
 *     The MSB of the MAC address of the device. The LSBs are
 *     according to VSI.
 * REMARKS:
 *   - Valid for Routing, LSR, VPLS and Trill. - Not valid
 *   for MIM. The B-MyMac is configured by
 *   soc_ppd_lif_my_bmac_msb_set() and
 *   soc_ppd_lif_my_bmac_port_lsb_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mymac_msb_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_msb
  );

/*********************************************************************
*     Gets the configuration set by the "arad_pp_mymac_msb_set"
 *     API.
 *     Refer to "arad_pp_mymac_msb_set" API for details.
*********************************************************************/
uint32
  arad_pp_mymac_msb_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *my_mac_msb
  );

/*********************************************************************
* NAME:
 *   arad_pp_mymac_vsi_lsb_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set MAC address LSB according to VSI.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx -
 *     VSI ID. Range: Arad-B: 0-4K. T20E: 0- 64K.
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb -
 *     The LSB of the MAC address. MSB is global.
 * REMARKS:
 *   - Ingress (termination): Packets run on this VSI with
 *   this MAC as DA will be forwarded to router/switch
 *   engine. - Egress (SA Encapsulation): Packets that exit
 *   the router/switch via this VSI will have this MAC
 *   address as the packet SA. - Not valid for MIM.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mymac_vsi_lsb_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mymac_vsi_lsb_set" API.
 *     Refer to "arad_pp_mymac_vsi_lsb_set" API for details.
*********************************************************************/
uint32
  arad_pp_mymac_vsi_lsb_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb
  );

/*********************************************************************
* NAME:
 *   arad_pp_mymac_vrrp_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set My-MAC according to Virtual Router Redundancy
 *   Protocol.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_MYMAC_VRRP_INFO                     *vrrp_info -
 *     VRRP information, including according to which interface
 *     to set the My-MAC lsb (port/VSI)
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mymac_vrrp_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MYMAC_VRRP_INFO                     *vrrp_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mymac_vrrp_info_set" API.
 *     Refer to "arad_pp_mymac_vrrp_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_mymac_vrrp_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_MYMAC_VRRP_INFO                     *vrrp_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_mymac_vrrp_mac_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable / Disable MyMac/MyVRID according to VRRP-ID and
 *   Mac Address LSB.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  vrrp_id_ndx -
 *     VRRP-ID. The port or VSI, according to which to
 *     configure the MAC. Depending on the VRRP_MODE.
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *vrrp_mac_lsb_key -
 *     The LSB of the MAC address. MSB is global.
 *   SOC_SAND_IN  uint8                                 enable -
 *     Whether to enable or disable the given MAC for the given
 *     vrrp_id
 * REMARKS:
 *   - The MAC address MSB is according to the value set by
 *   soc_ppd_my_mac_msb_set()- The MAC address is identified as
 *   MyMAC, either according to
 *   'soc_ppd_my_mac_vrrp_glbl_info_set' or
 *   'soc_ppd_my_mac_vrrp_mac_set'.- vrrp-id type and
 *   vrrp_mac_lsb range according to vrrp-mode set by
 *   soc_ppd_mymac_vrrp_info_set(): - Per Port: VRRP-ID is
 *   port-ID, and MAC address LSBs are the bits [7:0] - Per
 *   VSI: VRRP-ID is the VSID, and MAC address LSBs are the
 *   bits [0:0] - Per 256 VSIs: VRRP-ID is the VSID[7:0], and
 *   MAC address LSBs are the bits [4:0]
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mymac_vrrp_mac_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *vrrp_mac_lsb_key,
    SOC_SAND_IN  uint8                                 enable
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mymac_vrrp_mac_set" API.
 *     Refer to "arad_pp_mymac_vrrp_mac_set" API for details.
*********************************************************************/
uint32
  arad_pp_mymac_vrrp_mac_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *vrrp_mac_lsb_key,
    SOC_SAND_OUT uint8                                 *enable
  );

/*********************************************************************
* NAME:
 *   arad_pp_mymac_trill_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set TRILL My-Nick-Name and reserved Nick-Name.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_MYMAC_TRILL_INFO                    *trill_info -
 *     Trill information.
 * REMARKS:
 *   - More TRILL encapsulation settings are under
 *   soc_ppd_eg_encap_trill_info_set() - MyMAC is according to:
 *   soc_ppd_my_mac_lsb_vsi_info_set(); soc_ppd_mymac_msb_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mymac_trill_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MYMAC_TRILL_INFO                    *trill_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mymac_trill_info_set" API.
 *     Refer to "arad_pp_mymac_trill_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_mymac_trill_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_MYMAC_TRILL_INFO                    *trill_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_my_bmac_msb_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the MSB of the My-B-MAC. My-B-MAC forwards the
 *   packets to the I Component, and is added as the SA when
 *   sending toward the backbone network.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_bmac_msb -
 *     The MSB of the MAC address of the device. The LSBs are
 *     according to VSI.
 * REMARKS:
 *   - Relevant Only for PBP ports.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_my_bmac_msb_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_bmac_msb
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_my_bmac_msb_set" API.
 *     Refer to "arad_pp_lif_my_bmac_msb_set" API for details.
*********************************************************************/
uint32
  arad_pp_lif_my_bmac_msb_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *my_bmac_msb
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_my_bmac_port_lsb_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the My-B-MAC LSB according to the source system
 *   port.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx -
 *     Physical system port.
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb -
 *     The LSB of the MAC address. MSB is global according to
 *     soc_ppd_lif_my_bmac_msb_set().
 *   SOC_SAND_IN  uint8                                 enable -
 *     When negated, the MAC address LSB is disabled for the
 *     system port
 * REMARKS:
 *   - Encapsulation: MAC-address is the packet B-SA,
 *   according to source system port ID - Termination -
 *   Enable = TRUE: When packet arrives with B-DA ==
 *   MAC-address, the backbone header is terminated and the
 *   packet is assigned to a VSI according to the I-SID. -
 *   Enable = FALSE: When the MAC address is attached to zero
 *   source system ports, the MAC address is not terminated,
 *   and forwarding is according B-MACT.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_my_bmac_port_lsb_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb,
    SOC_SAND_IN  uint8                                 enable
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_my_bmac_port_lsb_set" API.
 *     Refer to "arad_pp_lif_my_bmac_port_lsb_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_my_bmac_port_lsb_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_INOUT SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb,
    SOC_SAND_OUT uint8                                 *enable
  );

void
  ARAD_PP_MYMAC_VRRP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MYMAC_VRRP_INFO *info
  );

void
  ARAD_PP_MYMAC_TRILL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MYMAC_TRILL_INFO *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_MYMAC_VRRP_MODE_to_string(
    SOC_SAND_IN  ARAD_PP_MYMAC_VRRP_MODE enum_val
  );

void
  ARAD_PP_MYMAC_VRRP_INFO_print(
    SOC_SAND_IN  ARAD_PP_MYMAC_VRRP_INFO *info
  );

void
  ARAD_PP_MYMAC_TRILL_INFO_print(
    SOC_SAND_IN  ARAD_PP_MYMAC_TRILL_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_MYMAC_INCLUDED__*/
#endif

