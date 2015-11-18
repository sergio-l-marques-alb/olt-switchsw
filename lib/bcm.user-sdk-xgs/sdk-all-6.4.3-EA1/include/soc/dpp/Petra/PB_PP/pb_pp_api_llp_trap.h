/* $Id: pb_pp_api_llp_trap.h,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_api_llp_trap.h
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

#ifndef __SOC_PB_PP_API_LLP_TRAP_INCLUDED__
/* { */
#define __SOC_PB_PP_API_LLP_TRAP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_llp_trap.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Number of my-IP addresses, in usage to trap ARP
 *     requests.                                               */
#define  SOC_PB_PP_TRAP_NOF_MY_IPS (SOC_PPC_TRAP_NOF_MY_IPS)

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

#define SOC_PB_PP_LLP_TRAP_PORT_ENABLE_ARP                     SOC_PPC_LLP_TRAP_PORT_ENABLE_ARP
#define SOC_PB_PP_LLP_TRAP_PORT_ENABLE_IGMP                    SOC_PPC_LLP_TRAP_PORT_ENABLE_IGMP
#define SOC_PB_PP_LLP_TRAP_PORT_ENABLE_MLD                     SOC_PPC_LLP_TRAP_PORT_ENABLE_MLD
#define SOC_PB_PP_LLP_TRAP_PORT_ENABLE_DHCP                    SOC_PPC_LLP_TRAP_PORT_ENABLE_DHCP
#define SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0             SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0
#define SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_1             SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_1
#define SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_2             SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_2
#define SOC_PB_PP_LLP_TRAP_PORT_ENABLE_PROG_TRAP_3             SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_3
#define SOC_PB_PP_LLP_TRAP_PORT_ENABLE_ALL                     SOC_PPC_LLP_TRAP_PORT_ENABLE_ALL
typedef SOC_PPC_LLP_TRAP_PORT_ENABLE                           SOC_PB_PP_LLP_TRAP_PORT_ENABLE;

#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_DA            SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA
#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE    SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE
#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE      SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE
#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL      SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL
#define SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS      SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS
typedef SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT                 SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT;

typedef SOC_PPC_LLP_TRAP_PORT_INFO                             SOC_PB_PP_LLP_TRAP_PORT_INFO;
typedef SOC_PPC_LLP_TRAP_ARP_INFO                              SOC_PB_PP_LLP_TRAP_ARP_INFO;
typedef SOC_PPC_LLP_TRAP_RESERVED_MC_KEY                       SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY;
typedef SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO                     SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO;
typedef SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO                     SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO;
typedef SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO                     SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO;
typedef SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER                   SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER;

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
 *   soc_pb_pp_llp_trap_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets port information for Link Layer Traps, including
 *   which reserved Multicast profile and which Traps are
 *   enabled...
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_INFO                  *port_info -
 *     Port information for Link-layer trapping.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_INFO                  *port_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_trap_port_info_set" API.
 *     Refer to "soc_pb_pp_llp_trap_port_info_set" API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PORT_INFO                  *port_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_trap_arp_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets information for ARP trapping, including My-IP
 *   addresses (used to Trap ARP Requests)
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_ARP_INFO                   *arp_info -
 *     ARP trap information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_arp_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_ARP_INFO                   *arp_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_trap_arp_info_set" API.
 *     Refer to "soc_pb_pp_llp_trap_arp_info_set" API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_arp_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_ARP_INFO                   *arp_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_trap_reserved_mc_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets Trap information for IEEE reserved multicast
 *   (Ethernet Header. DA matches 01-80-c2-00-00-XX where XX =
 *   8'b00xx_xxxx.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key -
 *     Reserved Multicast key including Destination MAC address
 *     lsb (the msb are constant) and profile.
 *   SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *trap_action -
 *     Trap information including snoop/forwarding action.
 *     trap_code range: SOC_PPD_TRAP_CODE_RESERVED_MC_0-SOC_PPD_TRAP_CODE_RESERVED_MC_7.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_reserved_mc_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *trap_action
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_trap_reserved_mc_info_set" API.
 *     Refer to "soc_pb_pp_llp_trap_reserved_mc_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_reserved_mc_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY            *reserved_mc_key,
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE                      *trap_action
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_trap_prog_trap_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets a programmable trap, a trap that may be set to
 *   packets according to L2/L3/L4 attributes.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  prog_trap_ndx -
 *     Trap ID. Range: 0 - 3.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual -
 *     L2/L3/L4 attributes of the packets including (packets
 *     MAC-DA, Ether-Type, IP protocol, L4 ports).
 * REMARKS:
 *   - User can specify per port which programmable traps are
 *   enabled on the port - See soc_ppd_llp_trap_port_info_set().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_prog_trap_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  prog_trap_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_trap_prog_trap_info_set" API.
 *     Refer to "soc_pb_pp_llp_trap_prog_trap_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_llp_trap_prog_trap_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  prog_trap_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER        *prog_trap_qual
  );

void
  SOC_PB_PP_LLP_TRAP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PORT_INFO *info
  );

void
  SOC_PB_PP_LLP_TRAP_ARP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_ARP_INFO *info
  );

void
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY *info
  );

void
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO *info
  );

void
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO *info
  );

void
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO *info
  );

void
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_LLP_TRAP_PORT_ENABLE_to_string(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_ENABLE enum_val
  );

const char*
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT_to_string(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_COND_SELECT enum_val
  );

void
  SOC_PB_PP_LLP_TRAP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PORT_INFO *info
  );

void
  SOC_PB_PP_LLP_TRAP_ARP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_ARP_INFO *info
  );

void
  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_RESERVED_MC_KEY *info
  );

void
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L2_INFO *info
  );

void
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L3_INFO *info
  );

void
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_L4_INFO *info
  );

void
  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_LLP_TRAP_INCLUDED__*/
#endif

