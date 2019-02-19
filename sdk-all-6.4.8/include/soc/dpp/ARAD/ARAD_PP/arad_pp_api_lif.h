/* $Id: arad_pp_api_lif.h,v 1.21 Broadcom SDK $
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

#ifndef __ARAD_PP_API_LIF_INCLUDED__
/* { */
#define __ARAD_PP_API_LIF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_lif.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Used when the AC mapping is only according to the Port
 *     and outer VID, or according to the Port only.           */
#define  ARAD_PP_LIF_IGNORE_INNER_VID     (SOC_PPC_LIF_IGNORE_INNER_VID)

/*     Used when the AC mapping is only according to the port
 *     (port default).                                         */
#define  ARAD_PP_LIF_IGNORE_OUTER_VID     (SOC_PPC_LIF_IGNORE_OUTER_VID)

/*     Used when the AC mapping is only according to the port
 *     and outer VID and innver VID                                  */
#define  ARAD_PP_LIF_IGNORE_TUNNEL_VID     (SOC_PPC_LIF_IGNORE_TUNNEL_VID)

/*     Used when the AC mapping is only according to the port
 *     and outer VID and innver VID                                  */
#define  ARAD_PP_LIF_IGNORE_ETHER_TYPE     (SOC_PPC_LIF_IGNORE_ETHER_TYPE)

/*     Used when the AC mapping ignores Outer-PCP                   */
#define  ARAD_PP_LIF_IGNORE_OUTER_PCP     (SOC_PPC_LIF_IGNORE_OUTER_PCP)

/*     AC Group maximal size.                                  */
#define  ARAD_PP_LIF_AC_GROUP_MAX_SIZE    (SOC_PPC_LIF_AC_GROUP_MAX_SIZE)

/* VSI that should be used for P2P service */
#define ARAD_PP_VSI_P2P_SERVICE             (SOC_PPC_VSI_P2P_SERVICE_ARAD)

/*     VSI Equal to initial VID of the packet                 */
#define  ARAD_PP_VSI_EQ_IN_VID              (SOC_PPC_VSI_EQ_IN_VID)

#define ARAD_PP_l2_LIF_AC_LOCAL_SWITCHING_COS_PROFILE          SOC_PPC_LIF_AC_LOCAL_SWITCHING_COS_PROFILE


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

#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT                  	SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN             	SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN        	SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW                   	SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN              	SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN_VLAN         	SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN           SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_DEFAULT                  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_DEFAULT
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN        SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN            SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN_VLAN       SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL              SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN    SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN_COMP_VLAN    SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN_COMP_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN    SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_TLS_PORT_TUNNEL          SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TLS_PORT_TUNNEL
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_TRILL_DESIGNATED_VLAN    SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TRILL_DESIGNATED_VLAN
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID_VID SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID_VID
#define ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_TEST2                    SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TEST2
#define ARAD_PP_NOF_L2_LIF_AC_MAP_KEY_TYPES                  	SOC_PPC_NOF_L2_LIF_AC_MAP_KEY_TYPES_ARAD
typedef SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE                         ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE;

#define ARAD_PP_L2_LIF_DFLT_FRWRD_SRC_VSI                    SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI
#define ARAD_PP_L2_LIF_DFLT_FRWRD_SRC_LIF                    SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF
#define ARAD_PP_NOF_L2_LIF_DFLT_FRWRD_SRCS                   SOC_PPC_NOF_L2_LIF_DFLT_FRWRD_SRCS
typedef SOC_PPC_L2_LIF_DFLT_FRWRD_SRC                          ARAD_PP_L2_LIF_DFLT_FRWRD_SRC;

#define ARAD_PP_L2_LIF_AC_SERVICE_TYPE_MP                    SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP
#define ARAD_PP_L2_LIF_AC_SERVICE_TYPE_AC2AC                 SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC
#define ARAD_PP_L2_LIF_AC_SERVICE_TYPE_AC2PWE                SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE
#define ARAD_PP_L2_LIF_AC_SERVICE_TYPE_AC2PBB                SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB
#define ARAD_PP_NOF_L2_LIF_AC_SERVICE_TYPES                  SOC_PPC_NOF_L2_LIF_AC_SERVICE_TYPES
typedef SOC_PPC_L2_LIF_AC_SERVICE_TYPE                         ARAD_PP_L2_LIF_AC_SERVICE_TYPE;

#define ARAD_PP_L2_LIF_PWE_SERVICE_TYPE_MP                   SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP
#define ARAD_PP_L2_LIF_PWE_SERVICE_TYPE_P2P                  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P
#define ARAD_PP_L2_LIF_PWE_SERVICE_TYPE_P2P_CUSTOM           SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P_CUSTOM
#define ARAD_PP_L2_LIF_PWE_SERVICE_TYPE_VRF                  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_VRF
#define ARAD_PP_L2_LIF_PWE_SERVICE_TYPE_LSP                  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_LSP
#define ARAD_PP_NOF_L2_LIF_PWE_SERVICE_TYPES                 SOC_PPC_NOF_L2_LIF_PWE_SERVICE_TYPES
typedef SOC_PPC_L2_LIF_PWE_SERVICE_TYPE                        ARAD_PP_L2_LIF_PWE_SERVICE_TYPE;

#define ARAD_PP_L2_LIF_ISID_SERVICE_TYPE_MP                  SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_MP
#define ARAD_PP_L2_LIF_ISID_SERVICE_TYPE_P2P                 SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_P2P
#define ARAD_PP_NOF_L2_LIF_ISID_SERVICE_TYPES                SOC_PPC_NOF_L2_LIF_ISID_SERVICE_TYPES
typedef SOC_PPC_L2_LIF_ISID_SERVICE_TYPE                       ARAD_PP_L2_LIF_ISID_SERVICE_TYPE;

#define ARAD_PP_L2_LIF_AC_LEARN_DISABLE                      SOC_PPC_L2_LIF_AC_LEARN_DISABLE
#define ARAD_PP_L2_LIF_AC_LEARN_SYS_PORT                     SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT
#define ARAD_PP_L2_LIF_AC_LEARN_SYS_PORT_LIF                 SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT_LIF
#define ARAD_PP_L2_LIF_AC_LEARN_INFO                         SOC_PPC_L2_LIF_AC_LEARN_INFO
#define ARAD_PP_NOF_L2_LIF_AC_LEARN_TYPES                    SOC_PPC_NOF_L2_LIF_AC_LEARN_TYPES
typedef SOC_PPC_L2_LIF_AC_LEARN_TYPE                           ARAD_PP_L2_LIF_AC_LEARN_TYPE;

#define ARAD_PP_L2_LIF_L2CP_HANDLE_TYPE_NORMAL               SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_NORMAL
#define ARAD_PP_L2_LIF_L2CP_HANDLE_TYPE_TUNNEL               SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_TUNNEL
#define ARAD_PP_L2_LIF_L2CP_HANDLE_TYPE_PEER                 SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_PEER
#define ARAD_PP_L2_LIF_L2CP_HANDLE_TYPE_DROP                 SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_DROP
#define ARAD_PP_NOF_L2_LIF_L2CP_HANDLE_TYPES                 SOC_PPC_NOF_L2_LIF_L2CP_HANDLE_TYPES
typedef SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE                        ARAD_PP_L2_LIF_L2CP_HANDLE_TYPE;

typedef SOC_PPC_L2_LIF_AC_KEY_QUALIFIER                        ARAD_PP_L2_LIF_AC_KEY_QUALIFIER;
typedef SOC_PPC_L2_LIF_IN_VC_RANGE                             ARAD_PP_L2_LIF_IN_VC_RANGE;
typedef SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO                 ARAD_PP_L2_LIF_DEFAULT_FORWARDING_INFO;
typedef SOC_PPC_L2_LIF_PWE_LEARN_RECORD                        ARAD_PP_L2_LIF_PWE_LEARN_RECORD;
typedef SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO                     ARAD_PP_L2_LIF_PWE_ADDITIONAL_INFO;
typedef SOC_PPC_L2_LIF_PWE_INFO                                ARAD_PP_L2_LIF_PWE_INFO;

#define ARAD_PP_LIF_VLAN_RANGE_MAX_SIZE                        SOC_PPC_LIF_VLAN_RANGE_MAX_SIZE
typedef SOC_PPC_L2_VLAN_RANGE_KEY                              ARAD_PP_L2_VLAN_RANGE_KEY;
typedef SOC_PPC_L2_PORT_VLAN_RANGE_INFO                        ARAD_PP_L2_PORT_VLAN_RANGE_INFO;

typedef SOC_PPC_L2_LIF_AC_KEY                                  ARAD_PP_L2_LIF_AC_KEY;

typedef SOC_PPC_L2_LIF_AC_LEARN_RECORD                         ARAD_PP_L2_LIF_AC_LEARN_RECORD;
typedef SOC_PPC_L2_LIF_AC_ING_EDIT_INFO                        ARAD_PP_L2_LIF_AC_ING_EDIT_INFO;
typedef SOC_PPC_L2_LIF_AC_INFO                                 ARAD_PP_L2_LIF_AC_INFO;
typedef SOC_PPC_L2_LIF_AC_MP_INFO                              ARAD_PP_L2_LIF_AC_MP_INFO;
typedef SOC_PPC_L2_LIF_L2CP_KEY                                ARAD_PP_L2_LIF_L2CP_KEY;
typedef SOC_PPC_L2_LIF_AC_GROUP_INFO                           ARAD_PP_L2_LIF_AC_GROUP_INFO;
typedef SOC_PPC_L2_LIF_ISID_KEY                                ARAD_PP_L2_LIF_ISID_KEY;
typedef SOC_PPC_L2_LIF_ISID_INFO                               ARAD_PP_L2_LIF_ISID_INFO;
typedef SOC_PPC_L2_LIF_TRILL_INFO                              ARAD_PP_L2_LIF_TRILL_INFO;
typedef SOC_PPC_L2_LIF_GRE_KEY                                ARAD_PP_L2_LIF_GRE_KEY;
typedef SOC_PPC_L2_LIF_GRE_INFO                               ARAD_PP_L2_LIF_GRE_INFO;
typedef SOC_PPC_L2_LIF_VXLAN_KEY                                ARAD_PP_L2_LIF_VXLAN_KEY;
typedef SOC_PPC_L2_LIF_VXLAN_INFO                               ARAD_PP_L2_LIF_VXLAN_INFO;
typedef SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY                       ARAD_PP_L2_LIF_DEFAULT_FRWRD_KEY;
typedef SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION                    ARAD_PP_L2_LIF_DEFAULT_FRWRD_ACTION;
typedef SOC_PPC_L2_LIF_EXTENDER_KEY                             ARAD_PP_L2_LIF_EXTENDER_KEY;
typedef SOC_PPC_L2_LIF_EXTENDER_INFO                            ARAD_PP_L2_LIF_EXTENDER_INFO;

/* FLP use COS profile for anti-spoofing */
#define ARAD_PP_FLP_COS_PROFILE_ANTI_SPOOFING                 SOC_PPC_FLP_COS_PROFILE_ANTI_SPOOFING
/* FLP use COS profile for VMAC  */
#define ARAD_PP_FLP_COS_PROFILE_VMAC                          SOC_PPC_FLP_COS_PROFILE_VMAC


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
 *   arad_pp_l2_lif_pwe_map_range_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set MPLS labels that may be used as in-vc-labels
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range -
 *     First and Last MPLS Labels to be used as ingress VC
 *     labels. T20E Range Size: 0-896K
 * REMARKS:
 *   - T20E: Set the MPLS labels that may be used as
 *   in-vc-labels - Arad-B: Ignored
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_pwe_map_range_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_l2_lif_pwe_map_range_set" API.
 *     Refer to "arad_pp_l2_lif_pwe_map_range_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_l2_lif_pwe_map_range_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT ARAD_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_pwe_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable an in VC label as a PWE handle, and set the PWE
 *   attributes
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label -
 *     The In-MPLS label that is attached to the PWE
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index -
 *     LIF table index
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_PWE_INFO                     *pwe_info -
 *     PWE attributes. VSID, Out-VC, ...
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     Exact Match table
 * REMARKS:
 *   T20E: The lif_index is ignored. Must be set to
 *   T20E_LIF_DEFAULT_PWE_LIF. Arad-B: In multipoint services
 *   (pwe_info.service_type=MP), the default forwarding is
 *   set per VSI and NOT per PWE interface using this API.
 *   When Adding a PWE to the system, the user is expected
 *   to:- Find the Egress Editing pointer (EEP) that points
 *   to the PWE's tunnel and Link-Layer encapsulation.- Add a
 *   FEC, with the PWEs TM destination and the EEP that
 *   points to the tunnel. This FEC is used for learning.-
 *   For multipoint services, add the PWE to the multicast
 *   group that commits the flooding of the unknown MAC
 *   addresses in the VSI.- Call this function, when the
 *   above FEC is part of the learning information.
 *   ARAD: in case device is bring up with mpls indexed support
 *   in_vc_label consist both the label value and label index.
 *   to set in_vc_label call SOC_PPC_VC_LABEL_INDEXED_SET.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_pwe_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  ARAD_PP_L2_LIF_PWE_ADDITIONAL_INFO          *pwe_additional_info,
    SOC_SAND_IN  ARAD_PP_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_pwe_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get PWE attributes and the LIF-index according to
 *   in_vc_label.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label -
 *     The In-MPLS label that is attached to the PWE
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              lif_index -
 *     The index of the PWE in the LIF table
 *   SOC_SAND_OUT ARAD_PP_L2_LIF_PWE_INFO                     *pwe_info -
 *     PWE incoming attributes. VSID, Out-VC, ...
 *   SOC_SAND_OUT uint8                                 *found -
 *     TRUE: The entry was found, 'pwe_info' is validFALSE: The
 *     entry was not found, 'pwe_info' is invalid
 * REMARKS:
 *   T20E: The lif_index is ignored. Arad-B: In multipoint
 *   services, the default forwarding is according to the
 *   VSI. When Adding a PWE to the system, the user is
 *   expected to:- Find the Egress Editing pointer (EEP) that
 *   points to the PWE's tunnel and Link-Layer
 *   encapsulation.- Add a FEC, with the PWEs TM destination
 *   and the EEP that points to the tunnel. This FEC is used
 *   for learning.- For multipoint services, add the PWE to
 *   the multicast group that commits the flooding of the
 *   unknown MAC addresses in the VSI.- Call this function,
 *   when the above FEC is part of the learning
 *   information. In the T20E driver, the AC set API
 *   configures the VSI attributes as well.
 *   ARAD: in case device is bring up with mpls indexed support
 *   in_vc_label consist both the label value and label index.
 *   to set in_vc_label call SOC_PPC_VC_LABEL_INDEXED_SET.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_pwe_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT ARAD_PP_L2_LIF_PWE_ADDITIONAL_INFO              *pwe_additional_info,
    SOC_SAND_OUT ARAD_PP_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT uint8                                 *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_pwe_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remvoe the in-VC-label
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label -
 *     The In-MPLS label that is attached to the PWE
 *   SOC_SAND_IN  uint8                                remove_from_sem -
 *     Is set when entry should be removed from both lif table and SEM
 *   SOC_SAND_IN  uint8                                is_gal -
 *     Indication if lif is GAL entry    
 *   SOC_SAND_OUT uint32                                  *lif_index -
 *     LIF table index
 * REMARKS:
 *   - Unbind the mapping of the In-VC-Label to the LIF table
 *   from the SEM table- Invalidate the 'lif_index' entry in
 *   the LIF table- The 'lif_index' is returned to the user
 *   to enable management of the LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_pwe_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  uint8                                remove_from_sem,
    SOC_SAND_IN  uint8                                       is_gal,
    SOC_SAND_OUT uint32                                  *lif_index,
    SOC_SAND_IN  ARAD_PP_L2_LIF_PWE_ADDITIONAL_INFO          *pwe_additional_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_vlan_compression_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable VLAN compression when adding Attachment Circuits.
 *   Enables defining an AC according to VLAN domain and a
 *   range of VLANs, and not only according to port*VLAN
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_VLAN_RANGE_KEY                   *vlan_range_key -
 *     VLAN Range Key. When AC is added with the first VLAN in
 *     the range, the AC is defined for all the VLAN in the
 *     range. When the user adds an AC with VID in the middle of
 *     the range, the driver returns ERROR.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     VLAN ranges table
 * REMARKS:
 *   - Arad-B Only. In the T20E, there are no VLAN ranges,
 *   and an AC is specified per port * VLAN [* C-VLAN]- In
 *   the device, there is a global ranges table, and per port
 *   there is an option to use or not use the range. The
 *   driver manages the global table internally; therefore,
 *   the driver actually does the following:Upon Enable: Add
 *   the VLAN range mapping, unless it already exists, and
 *   enable for the ports in the VLAN domain. Upon Disable:
 *   Remove the VLAN range mapping, unless utilized by other
 *   ports, and the ports mapping in the VLAN domain.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_vlan_compression_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_VLAN_RANGE_KEY                   *vlan_range_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_vlan_compression_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove a VLAN range from the compressed VLAN ranges
 *   database
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_VLAN_RANGE_KEY                   *vlan_range_key -
 *     VLAN Range Key.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_vlan_compression_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_VLAN_RANGE_KEY                   *vlan_range_key
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_vlan_compression_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get all compressed VLAN ranges of a port
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 local_port_ndx -
 *     Identifier of the port to access.
 *   SOC_SAND_OUT  ARAD_PP_L2_PORT_VLAN_RANGE_INFO       *vlan_range_info -
 *     VLAN Range Info.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 arad_pp_l2_lif_vlan_compression_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_PORT                               local_port_ndx,
    SOC_SAND_OUT ARAD_PP_L2_PORT_VLAN_RANGE_INFO            *vlan_range_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_ac_map_key_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the packets attributes (in-port, VIDs) to consider
 *   when associating an incoming packet to in-AC.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY_QUALIFIER             *qual_key -
 *     The qualifier used to associate the packet into
 *     AC-key Includes port-profile and output from packet parsing
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE              key_mapping -
 *     AC map key, can be any of the following:- port- port x
 *     Vlan (outer)- port x Vlan x Vlan
 * REMARKS:
 *   - use SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_ANY as
 *   vlan_format_ndx to make a decision for ALL VLAN formats
 *   at once.- In order to assign a port profile to a local
 *   port, use: 1. ARAD_PP_port_info_set(local_port_ndx,
 *   port_profile)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_ac_map_key_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY_QUALIFIER             *qual_key,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE              key_mapping
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_l2_lif_ac_map_key_set" API.
 *     Refer to "arad_pp_l2_lif_ac_map_key_set" API for details.
*********************************************************************/
uint32
  arad_pp_l2_lif_ac_map_key_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY_QUALIFIER             *qual_key,
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE              *key_mapping
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_ac_mp_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function is used to define a Maintenance Point (MP)
 *   on an incoming Attachment-Circuit (port x VLAN x VLAN)
 *   and MD-level, and to determine the action to perform. If
 *   the MP is one of the 4K accelerated MEPs, the function
 *   configures the related OAMP databases and associates the
 *   AC and MD-Level with a user-provided handle. This handle
 *   is later used by user to access OAMP database for this
 *   MEP.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                       lif_ndx -
 *     LIF table index
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_MP_INFO            *info -
 *     MP configuration information.
 * REMARKS:
 *   Should only be used if the MP is down MEP or MIP.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_ac_mp_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_PP_LIF_ID                       lif_ndx,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_MP_INFO            *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_l2_lif_ac_mp_info_set" API.
 *     Refer to "arad_pp_l2_lif_ac_mp_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_l2_lif_ac_mp_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_PP_LIF_ID                       lif_ndx,
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_MP_INFO            *info
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_ac_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable an Attachment Circuit, and set the AC attributes.
 *   Bind the ac-key to the LIF, and set the LIF attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key identifies the AC and how to map a packet to it
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index -
 *     Logical Interfaces table index. The LIF index is also the
 *     AC ID that the packet is learnt with.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_INFO                      *ac_info -
 *     AC Attributes. VSID, Ingress Editing attributes, ...
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space (in the
 *     Exact Match table)
 * REMARKS:
 *   - T20E: The VLAN domain is equal to the port ID- T20E:
 *   The concept of VLAN ranges is not applicable- Arad-B:
 *   In multipoint services, the default forwarding is
 *   according to the VSI- QinQ CEP port: o The VSI is the
 *   S-VIDo 'vid_ndx' is the C-VID tago This API registers
 *   C-VID * port to service o The egress editing module will
 *   map the VSI to the appended S-VIDo No need in AC
 *   learning, because the S-VID is the service, and the
 *   S-VID * port is sufficient for the editing.o Ingress
 *   editing may be used to append / replace / remove C-VID-
 *   MIM: This function is valid for customer facing ACs. In
 *   this case, the VSI holds the I-SID as the EEI- VPLS: The
 *   resolved VSI is the core VSI, and may be updated
 *   according to PWE- LIF index: Should be equal to
 *   'out_ac_ndx' when configuring the Out-AC via
 *   ARAD_PP_eg_ac_info_set()When Adding an AC to the system, the
 *   user is expected to:- Set the Out-AC Egress editing.-
 *   For multipoint services, when egress editing is
 *   according to AC, add the AC as a member in the flooding
 *   multicast ID. Otherwise, add the source port- Call this
 *   function
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_ac_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_INFO                      *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_ac_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an Attachment Circuit, according to AC key.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key identifies the AC and how to map a packet to it
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     The Logical Interfaces table index that the AC was found
 *     in.
 *   SOC_SAND_OUT ARAD_PP_L2_LIF_AC_INFO                      *ac_info -
 *     AC Attributes. VSID, Ingress Editing attributes, ...
 *   SOC_SAND_OUT uint8                                 *found -
 *     TRUE: The entry was found, 'ac_info' is validFALSE: The
 *     entry was not found, 'ac_info' is invalid
 * REMARKS:
 *   Get the AC info and index, according to the AC Key
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_ac_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_INFO                      *ac_info,
    SOC_SAND_OUT uint8                                 *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_ac_with_cos_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add Attachment Circuit (AC) group. Enable defining ACs,
 *   according to Quality of Service attributes, on top of
 *   the usual port * VID [*VID] identification
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key that identifies the ACs group.
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              base_lif_index -
 *     Logical Interfaces table base index. The AC occupies
 *     entries base_lif_index to (base_lif_index + ACs-group
 *     size - 1) in the Logical Interfaces table
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_GROUP_INFO                *acs_group_info -
 *     ACs Group Attributes. Contain an array of ACs, and
 *     pointer to the mapping attributes between the QoS
 *     information and the ACs in the group.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     Exact Match table
 * REMARKS:
 *   - See remarks of ARAD_PP_l2_lif_ac_add()- Map VLAN Domain
 *   and VID(s) to group of AC Logical interfaces, and set
 *   ACs information. The QoS attribute type and mapping is
 *   according to the opcode_id. - 'nof_lif_entries' is only
 *   used for driver validation. The Number of LIF entries is
 *   derived from the 'opcode_id'.- The opcode types are
 *   configured via the API ARAD_PP_lif_cos_opcode_types_set
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_ac_with_cos_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              base_lif_index,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_GROUP_INFO                *acs_group_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_ac_with_cos_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get Attachment Circuit (AC) group attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key identifies the ACs group and how to map a packet
 *     to it. Out of the ACs group, the specific AC is
 *     according to the QoS params and the Op-code.
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *base_lif_index -
 *     Logical Interfaces table base index. The LIF index of the
 *     first AC in the ACs array.
 *   SOC_SAND_OUT ARAD_PP_L2_LIF_AC_GROUP_INFO                *acs_group_info -
 *     ACs Group Attributes. Contain an array of ACs, and
 *     pointer to the mapping attributes between the QoS
 *     information and the ACs in the group.
 *   SOC_SAND_OUT uint8                                 *found -
 *     TRUE: The entry was found-'acs_group_info' is
 *     validFALSE: The entry was not found-'acs_group_info' is
 *     invalid
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_ac_with_cos_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *base_lif_index,
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_GROUP_INFO                *acs_group_info,
    SOC_SAND_OUT uint8                                 *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_ac_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an Attachment Circuit
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key identifies
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     Logical Interfaces table index
 * REMARKS:
 *   - Unbind the mapping of the AC to the LIF table from the
 *   SEM table- Invalidate the 'lif_index' entry in the LIF
 *   table- The 'lif_index' is returned to the user to enable
 *   management of the LIF table- Revert both
 *   ARAD_PP_l2_lif_ac_with_cos_add() and ARAD_PP_l2_lif_ac_add(). In
 *   this case, the driver invalidate all the lif table
 *   entries of the LIF group, and the returned 'lif_index'
 *   is the lif_base_index
 *   - For ARAD, in case lif_index = NULL, then remove only key without
 *   any modification on the LIF settings. 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_ac_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_l2cp_trap_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets Trap information for Layer 2 control protocol
 *   frames. Packet is an MEF layer 2 control protocol
 *   service frame When DA matches 01-80-c2-00-00-XX where XX
 *   = 8'b00xx_xxxx.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_L2CP_KEY                     *l2cp_key -
 *     Reserved Multicast key including Destination MAC address
 *     lsb (the msb are constant) and l2cp profile (set
 *     according to in-AC see ARAD_PP_l2_lif_ac_add()).
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_L2CP_HANDLE_TYPE             handle_type -
 *     How to handle the frame with the l2cp_key, drop,
 *     peer(trap), normal or tunneled (transparent)
 * REMARKS:
 *   - Arad-B only.- This applied only for L2 AC LIF.- in
 *   case L2CP packet to be dropped/trapped(peer) the packet
 *   is assigned Trap code
 *   ARAD_PP_TRAP_CODE_L2CP_DROP/ARAD_PP_TRAP_CODE_L2CP_PEER. - In
 *   order to set the forwarding/snooping action use
 *   ARAD_PP_trap_frwrd_profile_info_set()/ARAD_PP_trap_snoop_profile_info_set
 *   with the above trap codes
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_l2cp_trap_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_L2CP_KEY                     *l2cp_key,
    SOC_SAND_IN  ARAD_PP_L2_LIF_L2CP_HANDLE_TYPE             handle_type
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_l2_lif_l2cp_trap_set" API.
 *     Refer to "arad_pp_l2_lif_l2cp_trap_set" API for details.
*********************************************************************/
uint32
  arad_pp_l2_lif_l2cp_trap_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_L2CP_KEY                     *l2cp_key,
    SOC_SAND_OUT ARAD_PP_L2_LIF_L2CP_HANDLE_TYPE             *handle_type
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_isid_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map an I-SID to VSI and vice versa. Set I-SID related
 *   attributes. Relevant when the packet arrives from the
 *   PBP
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          vsi_ndx -
 *     System VSID.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_KEY                     *isid_key -
 *     I-SID, as taken from the I-TAG, and optionally I-SID
 *     domain.
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index -
 *     Logical Interfaces Table Entry. I-SID mapping to VSI is
 *     done in the LIF table. Arad-B Range: 0-16KT20E Range:
 *     0-128K
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_INFO                    *isid_info -
 *     VSID and I-SID related attributes
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in
 *     Exact Match
 * REMARKS:
 *   The mapping from the VSI to I-SID is done in the VSID
 *   table. The mapping from the I-SID to VSI is done in the
 *   LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_isid_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_isid_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map an I-SID to VSI and vice versa. Set I-SID related
 *   attributes. Relevant when the packet arrives from the
 *   PBP
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_KEY                     *isid_key -
 *     I-SID, as taken from the I-TAG, and optionally I-SID
 *     domain.
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     Logical Interfaces Table Entry
 *   SOC_SAND_OUT ARAD_PP_SYS_VSI_ID                          *vsi_index -
 *     System VSID.
 *   SOC_SAND_OUT ARAD_PP_L2_LIF_ISID_INFO                    *isid_info -
 *     VSID and I-SID related attributes
 *   SOC_SAND_OUT uint8                                 *found -
 *     TRUE: ISID was foundFALSE: ISID was not found
 * REMARKS:
 *   The ISID mapping information is taken from the LIF
 *   table.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_isid_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT ARAD_PP_SYS_VSI_ID                          *vsi_index,
    SOC_SAND_OUT ARAD_PP_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_OUT uint8                                 *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_vsi_to_isid
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the I-SID related to the VSI
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          vsi_ndx -
 *     System VSID.
 *   SOC_SAND_OUT SOC_SAND_PP_ISID                              *isid_id -
 *     I-SID related to the VSID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_vsi_to_isid(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_ISID                      *isid_id
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_isid_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Unbind an I-SID from VSI and vice versa
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          vsi_ndx -
 *     System VSID.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_KEY                     *isid_key -
 *     I-SID, as taken from the I-TAG, and optionally I-SID
 *     domain.
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     Logical Interfaces Table Entry. I-SID mapping to VSI is
 *     done in the LIF table. Arad-B Range: 0-16KT20E Range:
 *     0-128K
 * REMARKS:
 *   - Unbind the mapping from the VSI to the I-SID in the
 *   VSI table- Unbind the mapping of the ISID-key to the LIF
 *   table from the SEM table- Invalidate the 'lif_index'
 *   entry in the LIF table- The 'lif_index' is returned to
 *   the user to enable management of the LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_isid_remove(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index
  );


/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_gre_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map an GRE-KEY to VSI and vice versa. Set GRE-KEY related
 *   attributes. Relevant when the packet arrives from the
 *   PBP
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          vsi_ndx -
 *     System VSID.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_GRE_KEY                     *gre_key -
 *     GRE-KEY, as taken from the GRE HEADER, and optionally GRE-KEY
 *     domain.
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index -
 *     Logical Interfaces Table Entry. GRE-KEY mapping to VSI is
 *     done in the LIF table. Petra-B Range: 0-16KT20E Range:
 *     0-128K
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_GRE_INFO                    *gre_info -
 *     VSID and GRE-KEY related attributes
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in
 *     Exact Match
 * REMARKS:
 *   The mapping from the VSI to GRE-KEY is done in the VSID
 *   table. The mapping from the GRE-KEY to VSI is done in the
 *   LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_gre_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  ARAD_PP_L2_LIF_GRE_INFO                    *gre_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_gre_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map an GRE-KEY to VSI and vice versa. Set GRE-KEY related
 *   attributes. Relevant when the packet arrives from the
 *   PBP
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_GRE_KEY                     *gre_key -
 *     GRE-KEY, as taken from the GRE HEADER, and optionally GRE-KEY
 *     domain.
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     Logical Interfaces Table Entry
 *   SOC_SAND_OUT ARAD_PP_L2_LIF_GRE_INFO                    *gre_info -
 *     VSID and GRE-KEY related attributes
 *   SOC_SAND_OUT uint8                               *found -
 *     TRUE: GRE was foundFALSE: GRE was not found
 * REMARKS:
 *   The GRE mapping information is taken from the LIF
 *   table.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_gre_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT ARAD_PP_L2_LIF_GRE_INFO                    *gre_info,
    SOC_SAND_OUT uint8                               *found
  );


/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_gre_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Unbind an GRE-KEY from VSI and vice versa
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_GRE_KEY                     *gre_key -
 *     GRE-KEY, as taken from the GRE HEADER, and optionally GRE-KEY
 *     domain.
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     Logical Interfaces Table Entry. GRE-KEY mapping to VSI is
 *     done in the LIF table. Petra-B Range: 0-16KT20E Range:
 *     0-128K
 * REMARKS:
 *   - Unbind the mapping from the VSI to the GRE-KEY in the
 *   VSI table- Unbind the mapping of the GRE-key to the LIF
 *   table from the SEM table- Invalidate the 'lif_index'
 *   entry in the LIF table- The 'lif_index' is returned to
 *   the user to enable management of the LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_gre_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_GRE_KEY                   *gre_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                           *lif_index
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_vxlan_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map an VXLAN-KEY to VSI and vice versa. Set VXLAN-KEY related
 *   attributes. Relevant when the packet arrives from the
 *   PBP
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_SYS_VSI_ID                          vsi_ndx -
 *     System VSID.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_VXLAN_KEY                     *vxlan_key -
 *     VXLAN-KEY, as taken from the VXLAN HEADER, and optionally VXLAN-KEY
 *     domain.
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index -
 *     Logical Interfaces Table Entry. VXLAN-KEY mapping to VSI is
 *     done in the LIF table. Petra-B Range: 0-16KT20E Range:
 *     0-128K
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_VXLAN_INFO                    *vxlan_info -
 *     VSID and VXLAN-KEY related attributes
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in
 *     Exact Match
 * REMARKS:
 *   The mapping from the VSI to VXLAN-KEY is done in the VSID
 *   table. The mapping from the VXLAN-KEY to VSI is done in the
 *   LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_vxlan_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  ARAD_PP_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_vxlan_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map an VXLAN-KEY to VSI and vice versa. Set VXLAN-KEY related
 *   attributes. Relevant when the packet arrives from the
 *   PBP
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_VXLAN_KEY                     *vxlan_key -
 *     VXLAN-KEY, as taken from the VXLAN HEADER, and optionally VXLAN-KEY
 *     domain.
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     Logical Interfaces Table Entry
 *   SOC_SAND_OUT ARAD_PP_L2_LIF_VXLAN_INFO                    *vxlan_info -
 *     VSID and VXLAN-KEY related attributes
 *   SOC_SAND_OUT uint8                               *found -
 *     TRUE: VXLAN was foundFALSE: VXLAN was not found
 * REMARKS:
 *   The VXLAN mapping information is taken from the LIF
 *   table.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_vxlan_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT ARAD_PP_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_OUT uint8                               *found
  );


/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_vxlan_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Unbind an VXLAN-KEY from VSI and vice versa
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_VXLAN_KEY                     *vxlan_key -
 *     VXLAN-KEY, as taken from the VXLAN HEADER, and optionally VXLAN-KEY
 *     domain.
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     Logical Interfaces Table Entry. VXLAN-KEY mapping to VSI is
 *     done in the LIF table. Petra-B Range: 0-16KT20E Range:
 *     0-128K
 * REMARKS:
 *   - Unbind the mapping from the VSI to the VXLAN-KEY in the
 *   VSI table- Unbind the mapping of the VXLAN-key to the LIF
 *   table from the SEM table- Invalidate the 'lif_index'
 *   entry in the LIF table- The 'lif_index' is returned to
 *   the user to enable management of the LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_vxlan_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_VXLAN_KEY                   *vxlan_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                           *lif_index
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_extender_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable a Port-Extender and set its attributes.
 *   Bind the Port-Extender key to an In-LIF, and set the In-LIF attributes.
 * INPUT:
 *   SOC_SAND_IN  int                                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_EXTENDER_KEY           *extender_key -
 *     Port Extender key - ame-Space x E-CID x C-VID
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                        lif_index -
 *     Logical Interfaces Table Entry. Port-Extender
 *     mapping to VSI is done in the LIF table.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_EXTENDER_INFO          *extender_info -
 *     VSID and I-SID related attributes
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is
 *     no space in Exact Match
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 arad_pp_l2_lif_extender_add(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_EXTENDER_KEY            *extender_key,
    SOC_SAND_IN  ARAD_PP_LIF_ID                         lif_index,
    SOC_SAND_IN  ARAD_PP_L2_LIF_EXTENDER_INFO           *extender_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success);



/*********************************************************************
* NAME:
 *   arad_pp_l2_lif_default_frwrd_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the action info (forwarding/snooping) to assign
 *   for packets upon failure lookup in the MACT (MACT
 *   default forwarding).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_DEFAULT_FRWRD_KEY           *dflt_frwrd_key -
 *     Set of parameters that default forwarding may be
 *     assigned to. Includes Port-DA-not-found profile,
 *     LIF-default-profile and DA-type (UC/MC/BC).
 *   SOC_SAND_IN  ARAD_PP_L2_LIF_DEFAULT_FRWRD_ACTION        *action_info -
 *     Default Action information including snoop/forwarding action and add_vsi base.
 * REMARKS:
 *  Used to set default
 *   forwarding for LIF/Profile.- By arad_pp_port_info_set() and
 *   arad_pp_l2_lif_xx_info_set() the user
 *   supplies the default forwarding profiles for the LIF and Port; 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_lif_default_frwrd_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key,
    SOC_SAND_IN  ARAD_PP_L2_LIF_DEFAULT_FRWRD_ACTION     *action_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_l2_lif_default_frwrd_info_set" API.
 *     Refer to "arad_pp_l2_lif_default_frwrd_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_l2_lif_default_frwrd_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key,
    SOC_SAND_OUT ARAD_PP_L2_LIF_DEFAULT_FRWRD_ACTION     *action_info
  );

/**
 * See the soc_ppd versions for docing.
 * 
 */
void
  ARAD_PP_L2_LIF_AC_KEY_QUALIFIER_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_KEY_QUALIFIER *info
  );

void
  ARAD_PP_L2_LIF_IN_VC_RANGE_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_IN_VC_RANGE *info
  );

void
  ARAD_PP_L2_LIF_DEFAULT_FORWARDING_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_DEFAULT_FORWARDING_INFO *info
  );

void
  ARAD_PP_L2_LIF_PWE_LEARN_RECORD_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_PWE_LEARN_RECORD *info
  );

void
  ARAD_PP_L2_LIF_PWE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_PWE_INFO *info
  );

void
  ARAD_PP_L2_VLAN_RANGE_KEY_clear(
    SOC_SAND_OUT ARAD_PP_L2_VLAN_RANGE_KEY *info
  );

void
  ARAD_PP_L2_PORT_VLAN_RANGE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_PORT_VLAN_RANGE_INFO *info
  );

void
  ARAD_PP_L2_LIF_AC_KEY_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_KEY *info
  );

void
  ARAD_PP_L2_LIF_AC_LEARN_RECORD_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_LEARN_RECORD *info
  );

void
  ARAD_PP_L2_LIF_AC_ING_EDIT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_ING_EDIT_INFO *info
  );

void
  ARAD_PP_L2_LIF_AC_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_INFO *info
  );

void
  ARAD_PP_L2_LIF_AC_MP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_MP_INFO *info
  );

void
  ARAD_PP_L2_LIF_L2CP_KEY_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_L2CP_KEY *info
  );

void
  ARAD_PP_L2_LIF_AC_GROUP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_AC_GROUP_INFO *info
  );

void
  ARAD_PP_L2_LIF_ISID_KEY_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_ISID_KEY *info
  );

void
  ARAD_PP_L2_LIF_ISID_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_ISID_INFO *info
  );

void
  ARAD_PP_L2_LIF_TRILL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_TRILL_INFO *info
  );

void
  ARAD_PP_L2_LIF_GRE_KEY_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_GRE_KEY *info
  );

void
  ARAD_PP_L2_LIF_GRE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_GRE_INFO *info
  );

void
  ARAD_PP_L2_LIF_VXLAN_KEY_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_VXLAN_KEY *info
  );

void
  ARAD_PP_L2_LIF_VXLAN_INFO_clear(
    SOC_SAND_OUT ARAD_PP_L2_LIF_VXLAN_INFO *info
  );

void
  ARAD_PP_L2_LIF_DEFAULT_FRWRD_KEY_clear(
    SOC_SAND_OUT  ARAD_PP_L2_LIF_DEFAULT_FRWRD_KEY *info
  );

void
  ARAD_PP_L2_LIF_DEFAULT_FRWRD_ACTION_clear(
    SOC_SAND_OUT  ARAD_PP_L2_LIF_DEFAULT_FRWRD_ACTION *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_MAP_KEY_TYPE enum_val
  );

const char*
  ARAD_PP_L2_LIF_DFLT_FRWRD_SRC_to_string(
    SOC_SAND_IN  ARAD_PP_L2_LIF_DFLT_FRWRD_SRC enum_val
  );

const char*
  ARAD_PP_L2_LIF_AC_SERVICE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_SERVICE_TYPE enum_val
  );

const char*
  ARAD_PP_L2_LIF_PWE_SERVICE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_L2_LIF_PWE_SERVICE_TYPE enum_val
  );

const char*
  ARAD_PP_L2_LIF_ISID_SERVICE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_SERVICE_TYPE enum_val
  );

const char*
  ARAD_PP_L2_LIF_AC_LEARN_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_LEARN_TYPE enum_val
  );

const char*
  ARAD_PP_L2_LIF_L2CP_HANDLE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_L2_LIF_L2CP_HANDLE_TYPE enum_val
  );

void
  ARAD_PP_L2_LIF_AC_KEY_QUALIFIER_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY_QUALIFIER *info
  );

void
  ARAD_PP_L2_LIF_IN_VC_RANGE_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_IN_VC_RANGE *info
  );

void
  ARAD_PP_L2_LIF_DEFAULT_FORWARDING_INFO_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_DEFAULT_FORWARDING_INFO *info
  );

void
  ARAD_PP_L2_LIF_PWE_LEARN_RECORD_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_PWE_LEARN_RECORD *info
  );

void
  ARAD_PP_L2_LIF_PWE_INFO_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_PWE_INFO *info
  );

void
  ARAD_PP_L2_VLAN_RANGE_KEY_print(
    SOC_SAND_IN  ARAD_PP_L2_VLAN_RANGE_KEY *info
  );

void
  ARAD_PP_L2_PORT_VLAN_RANGE_INFO_print(
    SOC_SAND_IN  ARAD_PP_L2_PORT_VLAN_RANGE_INFO *info
  );

void
  ARAD_PP_L2_LIF_AC_KEY_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY *info
  );

void
  ARAD_PP_L2_LIF_AC_LEARN_RECORD_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_LEARN_RECORD *info
  );

void
  ARAD_PP_L2_LIF_AC_ING_EDIT_INFO_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_ING_EDIT_INFO *info
  );

void
  ARAD_PP_L2_LIF_AC_INFO_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_INFO *info
  );

void
  ARAD_PP_L2_LIF_AC_MP_INFO_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_MP_INFO *info
  );

void
  ARAD_PP_L2_LIF_L2CP_KEY_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_L2CP_KEY *info
  );

void
  ARAD_PP_L2_LIF_AC_GROUP_INFO_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_GROUP_INFO *info
  );

void
  ARAD_PP_L2_LIF_ISID_KEY_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_KEY *info
  );

void
  ARAD_PP_L2_LIF_ISID_INFO_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_ISID_INFO *info
  );

void
  ARAD_PP_L2_LIF_GRE_KEY_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_GRE_KEY *info
  );

void
  ARAD_PP_L2_LIF_GRE_INFO_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_GRE_INFO *info
  );

void
  ARAD_PP_L2_LIF_VXLAN_KEY_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_VXLAN_KEY *info
  );

void
  ARAD_PP_L2_LIF_VXLAN_INFO_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_VXLAN_INFO *info
  );

void
  ARAD_PP_L2_LIF_DEFAULT_FRWRD_KEY_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_DEFAULT_FRWRD_KEY *info
  );

void
  ARAD_PP_L2_LIF_DEFAULT_FRWRD_ACTION_print(
    SOC_SAND_IN  ARAD_PP_L2_LIF_DEFAULT_FRWRD_ACTION *info
  );


#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_LIF_INCLUDED__*/
#endif 

