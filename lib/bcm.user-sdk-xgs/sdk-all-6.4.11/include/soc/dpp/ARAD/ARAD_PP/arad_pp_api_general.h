/* $Id: arad_pp_api_general.h,v 1.17 Broadcom SDK $
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

#ifndef __ARAD_PP_API_GENERAL_INCLUDED__
/* { */
#define __ARAD_PP_API_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Indicates that the RIF ID is not used and that RIF ID
 *     and RPF info are not updated.                           */
#define  ARAD_PP_RIF_NULL (SOC_PPC_RIF_NULL)

/*     Indicates that the EEP is not used.                     */
#define  ARAD_PP_EEP_NULL (SOC_PPC_EEP_NULL)

/*     Maximum Number of local PP ports in one device.         */
#define  ARAD_PP_MAX_NOF_LOCAL_PORTS (SOC_PPC_MAX_NOF_LOCAL_PORTS_ARAD)

/*     ignore given value     */
#define  ARAD_PP_IGNORE_VAL 0xFFFFFFFF

#define ARAD_PP_DEBUG                       (SOC_PPC_DEBUG)
#define ARAD_PP_DEBUG_IS_LVL1               (SOC_PPC_DEBUG_IS_LVL1)
#define ARAD_PP_DEBUG_IS_LVL2               (SOC_PPC_DEBUG_IS_LVL2)
#define ARAD_PP_DEBUG_IS_LVL3               (SOC_PPC_DEBUG_IS_LVL3)

#define ARAD_PP_VLAN_TAGS_MAX               (SOC_PPC_VLAN_TAGS_MAX)

/*
 *	Designates invalid/non-existing AC-id & VRF
 */
#define ARAD_PP_AC_ID_INVALID               (SOC_PPC_AC_ID_INVALID)
#define ARAD_PP_VRF_INVALID                 (SOC_PPC_VRF_INVALID)

#define ARAD_PP_TC_MAX_VAL                  (ARAD_NOF_TRAFFIC_CLASSES - 1)
#define ARAD_PP_DP_MAX_VAL                  (ARAD_MAX_DROP_PRECEDENCE)

#define ARAD_PP_TPID_PROFILE_MAX            3
#define ARAD_PP_LIF_PROFILE_MAX             15

#define ARAD_PP_CUD_EEP_PREFIX   (0x0)
#define ARAD_PP_CUD_VSI_PREFIX   (0x0)
#define ARAD_PP_CUD_AC_PREFIX    (0x0)
/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define ARAD_PP_DO_NOTHING_AND_EXIT                                         \
          SOC_SAND_IGNORE_UNUSED_VAR(res);                                    \
          goto exit

#define ARAD_PP_STRUCT_VERIFY(type, name, exit_num, exit_place)             \
          do                                                              \
          {                                                               \
            res = type##_verify(                                          \
                    name                                                  \
                  );                                                      \
            SOC_SAND_CHECK_FUNC_RESULT(res, exit_num, exit_place);            \
          } while(0)

#define ARAD_PP_STRUCT_VERIFY_UNIT(type, unit, name, exit_num, exit_place)             \
          do                                                              \
          {                                                               \
            res = type##_verify(                                          \
                    unit,                                                 \
                    name                                                  \
                  );                                                      \
            SOC_SAND_CHECK_FUNC_RESULT(res, exit_num, exit_place);            \
          } while(0)

/************************************************************************/
/* forwarding decision Macros                                           */
/************************************************************************/

/*
 *  Destination is Drop. Set 'fwd_decision' to drop
 *  destination. Packet forwarded according to this
 *  'fwd_decision' is dropped.
 */
#define ARAD_PP_FRWRD_DECISION_DROP_SET(unit, fwd_decision)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_DROP;  \
  (fwd_decision)->dest_id = 0; \
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_NONE; \
  (fwd_decision)->additional_info.outlif.val = 0;

/*
 *  Destination is the local CPU. Set 'fwd_decision' to local
 *  CPU (i.e. local port 0). Packet forwarded according to
 *  this 'fwd_decision' is forwarded to CPU (not trapped,
 *  i.e., with no trap-code attached to it)
 */
#define ARAD_PP_FRWRD_DECISION_LOCAL_CPU_SET(unit, fwd_decision)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_UC_PORT; \
  (fwd_decision)->dest_id = ARAD_FRST_CPU_PORT_ID; \
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_NONE;

/*
 *  Destination is a physical system port. Set the
 *  'fwd_decision' to include the destination physical
 *  system port (0 to 4K-1).
 */
#define ARAD_PP_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, fwd_decision,phy_port)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_UC_PORT; \
  (fwd_decision)->dest_id = phy_port;    \
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_NONE;

/*
 *  Destination is a LAG. Set the 'fwd_decision' to include
 *  the LAG ID.
 */
#define ARAD_PP_FRWRD_DECISION_LAG_SET(unit, fwd_decision,lag_id)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_UC_LAG; \
  (fwd_decision)->dest_id = lag_id;  \
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_NONE;

/*
 *  Destination is a multicast group. Set the 'fwd_decision'
 *  to include MC-group ID
 */
#define ARAD_PP_FRWRD_DECISION_MC_GROUP_SET(unit, fwd_decision,mc_id)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_MC; \
  (fwd_decision)->dest_id = mc_id;\
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY;
/*
 *  Destination is a FEC-entry. Set the 'fwd_decision' to
 *  include a pointer to the FEC table
 */
#define ARAD_PP_FRWRD_DECISION_FEC_SET(unit, fwd_decision,fec_id)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_id;\
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY;

/*
 *  Destination with COS (i.e., explicit TM flow). Set the
 *  'fwd_decision' to include the explicit TM flow_id
 */
#define ARAD_PP_FRWRD_DECISION_EXPL_FLOW_SET(unit, fwd_decision,flow_id)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_UC_FLOW; \
  (fwd_decision)->dest_id = flow_id;\
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_NONE;

/*
 *  Trap packet. Set the 'fwd_decision' to Trap the packet
 *  using the following attributes: - trap_code : 0-255;
 *  identifies the trap/snoop actions to be applied if the
 *  assigned strength is higher than the previously assigned
 *  strength.- fwd_strength: 0-7- snp_strenght: 0-3
 */
#define ARAD_PP_FRWRD_DECISION_TRAP_SET(unit, fwd_decision,code,frwrd_strength,snp_strength)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_TRAP; \
  (fwd_decision)->dest_id = 0;              \
  (fwd_decision)->additional_info.trap_info.action_profile.trap_code = code;       \
  (fwd_decision)->additional_info.trap_info.action_profile.frwrd_action_strength = frwrd_strength;  \
  (fwd_decision)->additional_info.trap_info.action_profile.snoop_action_strength = snp_strength;

/*
 *  Forward to an Out-AC Logical Interface. Set the
 *  'fwd_decision' to include the destination system-port
 *  and the Out-AC ID. Notes 1. The system-port can either be
 *  a LAG port or a system physical port.2. Packets
 *  forwarded according to this 'fwd_decision' are forwarded
 *  to the given sys_port3. The outgoing VLAN editing
 *  information is configured according to the associated
 *  Out-AC.4. This forwarding decision can be dynamically
 *  learned, by setting it in the In-AC's Learn-Record (see
 *  SOC_PPD_L2_LIF_AC_INFO).
 */
#define ARAD_PP_FRWRD_DECISION_AC_SET(unit, fwd_decision, ac_id, is_lag, sys_port_id)  \
  (fwd_decision)->type = (is_lag)?ARAD_PP_FRWRD_DECISION_TYPE_UC_LAG:ARAD_PP_FRWRD_DECISION_TYPE_UC_PORT; \
  (fwd_decision)->dest_id = sys_port_id;  \
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_RAW; \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY; \
  (fwd_decision)->additional_info.outlif.val = ac_id;

/*
 *  Forward to an Out-AC Logical Interface, using an
 *  explicit TM flow ID. Set the 'fwd_decision' to include
 *  the destination flow-id and the Out-AC ID. Notes 1.
 *  Packets forwarded according to this 'fwd_decision' are
 *  forwarded according the given TM flow-id2. The outgoing
 *  VLAN editing information is configured according to the
 *  associated Out-AC. 3. This forwarding decision can be
 *  dynamically learned, by setting it in the In-AC's
 *  Learn-Record (see SOC_PPD_L2_LIF_AC_INFO).
 */
#define ARAD_PP_FRWRD_DECISION_AC_WITH_COSQ_SET(unit, fwd_decision,ac_id,flow_id)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_UC_FLOW; \
  (fwd_decision)->dest_id = flow_id;  \
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_RAW; \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY; \
  (fwd_decision)->additional_info.outlif.val = ac_id;

/*
 *  Forward to access associated with AC-id with
 *  protection. Set the 'fwd_decision' to include AC-id with
 *  FEC-index. Packet forwarded according to this
 *  'fwd_decision' is forwarded according the FEC entry
 *  setting associated with the given (out) AC-id. This
 *  forwarding decision can be learned.
 */
#define ARAD_PP_FRWRD_DECISION_PROTECTED_AC_SET(unit, fwd_decision, ac_id, fec_index)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY; \

/*
 *  VPLS access to core with no protection (neither in the PWE
 *  nor on the tunnel). Set the 'fwd_decision' to include pwe-id
 *  and system-port. Packet forwarded according to this
 *  'fwd_decision' is forwarded to sys_port encapsulated according
 *  to pwe_id setting. This forwarding decision can be learned as well.
 */
#define ARAD_PP_FRWRD_DECISION_PWE_SET(unit, fwd_decision, pwe_id, is_lag, sys_port_id)  \
  (fwd_decision)->type = (is_lag)?ARAD_PP_FRWRD_DECISION_TYPE_UC_LAG:ARAD_PP_FRWRD_DECISION_TYPE_UC_PORT; \
  (fwd_decision)->dest_id = sys_port_id;  \
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_RAW; \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY; \
  (fwd_decision)->additional_info.outlif.val = pwe_id;
/*
 *
 */
#define ARAD_PP_FRWRD_DECISION_TRILL_SET(unit, fwd_decision, nick, is_multi, destination_id)  \
  (fwd_decision)->type = (is_multi)?ARAD_PP_FRWRD_DECISION_TYPE_MC:ARAD_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = destination_id;  \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_TRILL;  \
  (fwd_decision)->additional_info.eei.val.trill_dest.dest_nick = nick; \
  (fwd_decision)->additional_info.eei.val.trill_dest.is_multicast = (uint8) is_multi;    

/*
 *  VPLS access to core with no protection using an explicit TM flow ID.
 *  Set the fwd_decision' to include the destination flow-id and the PWE-ID.
 *  Notes 1. Packets forwarded according to this 'fwd_decision' are
 *  forwarded according the given TM flow-id 2. This forwarding decision can be
 *  dynamically learned, by setting it in the In-PWE's
 *  Learn-Record (see SOC_PPD_L2_LIF_PWE_INFO).
 */
#define ARAD_PP_FRWRD_DECISION_PWE_WITH_COSQ_SET(unit, fwd_decision, pwe_id,flow_id)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_UC_FLOW; \
  (fwd_decision)->dest_id = flow_id;  \
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_RAW; \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY; \
  (fwd_decision)->additional_info.outlif.val = pwe_id;

/*
 *  VPLS access to core with protection on tunnel only. Set
 *  the 'fwd_decision' to include fec-index VC-label. Packet
 *  forwarded according to this 'fwd_decision' is
 *  encapsulated with 'vc_label'. EXP,TTL is set according
 *  to 'push_profile' definition see. This forwarding
 *  decision can be learned as well.
 */
#define ARAD_PP_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_SET(unit, fwd_decision,vc_label, prm_push_profile,fec_index)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_MPLS; \
  (fwd_decision)->additional_info.eei.val.mpls_command.command = SOC_PPD_MPLS_COMMAND_TYPE_PUSH;  \
  (fwd_decision)->additional_info.eei.val.mpls_command.label = vc_label;    \
  (fwd_decision)->additional_info.eei.val.mpls_command.push_profile = prm_push_profile;

/*
 *  VPLS access to core with protection on tunnel only. Set
 *  the 'fwd_decision' to include fec-index and VPLS outlif. Packet
 *  forwarded according to this 'fwd_decision' is
 *  encapsulated as follows: PWE label is accoring to pwe_outlif entry, 
 *  tunnel labels are according to fec resolution. This forwarding
 *  decision can be learned as well.
 */
#define ARAD_PP_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_WITH_OUTLIF_SET(unit, fwd_decision, fec_index, pwe_outlif)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_RAW; \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY; \
  (fwd_decision)->additional_info.outlif.val = pwe_outlif;

/*
 *  VPLS access to core with protection on PWE. Set the
 *  'fwd_decision' to include fec-index. Packet forwarded
 *  according to this 'fwd_decision' is forwarded according
 *  to FEC entry setting. By this setting the PWE can be
 *  protected. This forwarding decision can be learned as
 *  well.
 */
#define ARAD_PP_FRWRD_DECISION_PROTECTED_PWE_SET(unit, fwd_decision, fec_index)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY;

/*
 *  ILM entry. Set the 'fwd_decision' to include swap-label
 *  and fec-index label. For Packets forwarded according to
 *  this 'fwd_decision' MPLS label is swappedAnd forwarded
 *  according to FEC entry setting
 */
#define ARAD_PP_FRWRD_DECISION_ILM_SWAP_SET(unit, fwd_decision,swap_label,fec_index)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_MPLS; \
  (fwd_decision)->additional_info.eei.val.mpls_command.command = SOC_PPD_MPLS_COMMAND_TYPE_SWAP;  \
  (fwd_decision)->additional_info.eei.val.mpls_command.label = swap_label;    \
  (fwd_decision)->additional_info.eei.val.mpls_command.push_profile = 0;

/*
 *  ILM Push entry. Set the 'fwd_decision' to include label
 *  and fec-index label. For Packets forwarded according to
 *  this 'fwd_decision' MPLS label is pushed And forwarded
 *  according to FEC entry setting
 */
#define ARAD_PP_FRWRD_DECISION_ILM_PUSH_SET(unit, fwd_decision,label, push_profile, fec_index)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_MPLS; \
  (fwd_decision)->additional_info.eei.val.mpls_command.command = SOC_PPD_MPLS_COMMAND_TYPE_PUSH;  \
  (fwd_decision)->additional_info.eei.val.mpls_command.label = label;    \
  (fwd_decision)->additional_info.eei.val.mpls_command.push_profile = push_profile;

/*
 *  Mac in mac entry. Destination is a FEC-entry. Set the 'fwd_decision' to
 *  include a pointer to the FEC table. EEI is the isid_id.
 */
#define ARAD_PP_FRWRD_DECISION_MAC_IN_MAC_SET(unit, fwd_decision,isid_id, fec_id)  \
  (fwd_decision)->type = ARAD_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_id;\
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_MIM;\
  (fwd_decision)->additional_info.eei.val.isid = isid_id;

#define ARAD_PP_FRWRD_DECISION_IP_TUNNEL_SET(_unit, _fwd_decision, _tunnel_id, _is_lag, _sys_port_id)  \
  (fwd_decision)->type = (is_lag)?ARAD_PP_FRWRD_DECISION_TYPE_UC_LAG:ARAD_PP_FRWRD_DECISION_TYPE_UC_PORT; \
  (fwd_decision)->dest_id = sys_port_id;  \
  (fwd_decision)->additional_info.outlif.type = ARAD_PP_OUTLIF_ENCODE_TYPE_RAW; \
  (fwd_decision)->additional_info.eei.type = ARAD_PP_EEI_TYPE_EMPTY; \
  (fwd_decision)->additional_info.outlif.val = ac_id;



/************************************************************************/
/* CUD macros                                                                     */
/************************************************************************/

/*
 * Returns CUD that includes EEP with value 'eep_ndx'
 */
#define ARAD_PP_CUD_EEP_GET(unit, eep_ndx)   \
  (ARAD_PP_CUD_EEP_PREFIX | (eep_ndx))
/*
 * Returns CUD that includes VSI with value 'vsi_ndx'
 */
#define ARAD_PP_CUD_VSI_GET(unit, vsi_ndx)   \
  (ARAD_PP_CUD_VSI_PREFIX | (vsi_ndx))
/*
 * Returns CUD that includes AC with value 'ac_ndx'
 */
#define ARAD_PP_CUD_AC_GET(unit, ac_ndx)   \
  (ARAD_PP_CUD_AC_PREFIX | (ac_ndx))
/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/*
 *  Local tm port. Range : 0 - 63.
 */
/*
 *  Local tm port. Range : 0 - 63.
 */
typedef SOC_PPC_TM_PORT                                        ARAD_PP_TM_PORT;

/*
 *  Local PP port. Range : 0 - 255.
 */
typedef SOC_PPC_PORT                                           ARAD_PP_PORT;

/*
 *  Filtering ID. Range: AradB: 0 - 16K-1. T20E: 0-64K-1.
 */
typedef SOC_PPC_FID                                            ARAD_PP_FID;

/*
 *  Virtual switch instance ID. Range: 0 - 16K-1.
 */
typedef SOC_PPC_VSI_ID                                         ARAD_PP_VSI_ID;

/*
 *  System VSI. Range: 0 - 64K-1.
 */
typedef SOC_PPC_SYS_VSI_ID                                     ARAD_PP_SYS_VSI_ID;

/*
 *  Forwarding Equivalence Class ID. Range: 0 - 16K-1.
 */
typedef SOC_PPC_FEC_ID                                         ARAD_PP_FEC_ID;

/*
 *  Virtual Router ID. Range: 1 - 255.
 */
typedef SOC_PPC_VRF_ID                                         ARAD_PP_VRF_ID;

/*
 *  Attachment Circuit ID. Range: AradB: 0 - 16K-1. T20E: 0
 *  - 64K-1.
 */
typedef SOC_PPC_AC_ID                                          ARAD_PP_AC_ID;

/*
 *  Router Interface ID. Range: 0 - 4K-1.
 */
typedef SOC_PPC_RIF_ID                                         ARAD_PP_RIF_ID;

/*
 *  Logical Interface ID. Range: AradB: 0 - 16K-1. T20E: 0
 *  - 64K-1.
 */
typedef SOC_PPC_LIF_ID                                         ARAD_PP_LIF_ID;


/*
 *  MP Level.
 */
typedef SOC_PPC_MP_LEVEL                                       ARAD_PP_MP_LEVEL;


#define ARAD_PP_EEI_TYPE_EMPTY                               SOC_PPC_EEI_TYPE_EMPTY
#define ARAD_PP_EEI_TYPE_TRILL                               SOC_PPC_EEI_TYPE_TRILL
#define ARAD_PP_EEI_TYPE_MPLS                                SOC_PPC_EEI_TYPE_MPLS
#define ARAD_PP_EEI_TYPE_MIM                                 SOC_PPC_EEI_TYPE_MIM
#define ARAD_PP_EEI_TYPE_OUTLIF                              SOC_PPC_EEI_TYPE_OUTLIF
#define ARAD_PP_EEI_TYPE_RAW                                 SOC_PPC_EEI_TYPE_RAW
#define ARAD_PP_NOF_EEI_TYPES                                SOC_PPC_NOF_EEI_TYPES_ARAD
typedef SOC_PPC_EEI_TYPE                                       ARAD_PP_EEI_TYPE;

#define ARAD_PP_OUTLIF_ENCODE_TYPE_NONE                      SOC_PPC_OUTLIF_ENCODE_TYPE_NONE
#define ARAD_PP_OUTLIF_ENCODE_TYPE_RAW                       SOC_PPC_OUTLIF_ENCODE_TYPE_RAW
#define ARAD_PP_OUTLIF_ENCODE_TYPE_RAW_INVALID               SOC_PPC_OUTLIF_ENCODE_TYPE_RAW_INVALID
#define ARAD_PP_OUTLIF_ENCODE_TYPE_AC                        SOC_PPC_OUTLIF_ENCODE_TYPE_AC
#define ARAD_PP_OUTLIF_ENCODE_TYPE_EEP                       SOC_PPC_OUTLIF_ENCODE_TYPE_EEP
#define ARAD_PP_OUTLIF_ENCODE_TYPE_VSI                       SOC_PPC_OUTLIF_ENCODE_TYPE_VSI
#define ARAD_PP_NOF_OUTLIF_ENCODE_TYPES                      SOC_PPC_NOF_OUTLIF_ENCODE_TYPES
typedef SOC_PPC_OUTLIF_ENCODE_TYPE                             ARAD_PP_OUTLIF_ENCODE_TYPE;

#define ARAD_PP_FRWRD_DECISION_TYPE_DROP                     SOC_PPC_FRWRD_DECISION_TYPE_DROP
#define ARAD_PP_FRWRD_DECISION_TYPE_UC_FLOW                  SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW
#define ARAD_PP_FRWRD_DECISION_TYPE_UC_LAG                   SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG
#define ARAD_PP_FRWRD_DECISION_TYPE_UC_PORT                  SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT
#define ARAD_PP_FRWRD_DECISION_TYPE_MC                       SOC_PPC_FRWRD_DECISION_TYPE_MC
#define ARAD_PP_FRWRD_DECISION_TYPE_FEC                      SOC_PPC_FRWRD_DECISION_TYPE_FEC
#define ARAD_PP_FRWRD_DECISION_TYPE_TRAP                     SOC_PPC_FRWRD_DECISION_TYPE_TRAP
#define ARAD_PP_NOF_FRWRD_DECISION_TYPES                     SOC_PPC_NOF_FRWRD_DECISION_TYPES
typedef SOC_PPC_FRWRD_DECISION_TYPE                            ARAD_PP_FRWRD_DECISION_TYPE;

#define ARAD_PP_MPLS_COMMAND_TYPE_PUSH                       SOC_PPC_MPLS_COMMAND_TYPE_PUSH
#define ARAD_PP_MPLS_COMMAND_TYPE_POP                        SOC_PPC_MPLS_COMMAND_TYPE_POP
#define ARAD_PP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE         SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE
#define ARAD_PP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM      SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM
#define ARAD_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE         SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE
#define ARAD_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM      SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM
#define ARAD_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE         SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE
#define ARAD_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM      SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM
#define ARAD_PP_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET          SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET
#define ARAD_PP_MPLS_COMMAND_TYPE_SWAP                       SOC_PPC_MPLS_COMMAND_TYPE_SWAP
#define ARAD_PP_NOF_MPLS_COMMAND_TYPES                       SOC_PPC_NOF_MPLS_COMMAND_TYPES
typedef SOC_PPC_MPLS_COMMAND_TYPE                              ARAD_PP_MPLS_COMMAND_TYPE;

#define ARAD_PP_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC                SOC_PPC_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC
#define ARAD_PP_L2_NEXT_PRTCL_TYPE_TRILL                     SOC_PPC_L2_NEXT_PRTCL_TYPE_TRILL
#define ARAD_PP_L2_NEXT_PRTCL_TYPE_IPV4                      SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV4
#define ARAD_PP_L2_NEXT_PRTCL_TYPE_IPV6                      SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV6
#define ARAD_PP_L2_NEXT_PRTCL_TYPE_ARP                       SOC_PPC_L2_NEXT_PRTCL_TYPE_ARP
#define ARAD_PP_L2_NEXT_PRTCL_TYPE_CFM                       SOC_PPC_L2_NEXT_PRTCL_TYPE_CFM
#define ARAD_PP_L2_NEXT_PRTCL_TYPE_MPLS                      SOC_PPC_L2_NEXT_PRTCL_TYPE_MPLS
#define ARAD_PP_L2_NEXT_PRTCL_TYPE_FC_ETH                    SOC_PPC_L2_NEXT_PRTCL_TYPE_FC_ETH
#define ARAD_PP_L2_NEXT_PRTCL_TYPE_OTHER                     SOC_PPC_L2_NEXT_PRTCL_TYPE_OTHER
#define ARAD_PP_NOF_L2_NEXT_PRTCL_TYPES                      SOC_PPC_NOF_L2_NEXT_PRTCL_TYPES
typedef SOC_PPC_L2_NEXT_PRTCL_TYPE                             ARAD_PP_L2_NEXT_PRTCL_TYPE;

#define ARAD_PP_L3_NEXT_PRTCL_TYPE_NONE                      SOC_PPC_L3_NEXT_PRTCL_TYPE_NONE
#define ARAD_PP_L3_NEXT_PRTCL_TYPE_TCP                       SOC_PPC_L3_NEXT_PRTCL_TYPE_TCP
#define ARAD_PP_L3_NEXT_PRTCL_TYPE_UDP                       SOC_PPC_L3_NEXT_PRTCL_TYPE_UDP
#define ARAD_PP_L3_NEXT_PRTCL_TYPE_IGMP                      SOC_PPC_L3_NEXT_PRTCL_TYPE_IGMP
#define ARAD_PP_L3_NEXT_PRTCL_TYPE_ICMP                      SOC_PPC_L3_NEXT_PRTCL_TYPE_ICMP
#define ARAD_PP_L3_NEXT_PRTCL_TYPE_ICMPV6                    SOC_PPC_L3_NEXT_PRTCL_TYPE_ICMPV6
#define ARAD_PP_L3_NEXT_PRTCL_TYPE_IPV4                      SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV4
#define ARAD_PP_L3_NEXT_PRTCL_TYPE_IPV6                      SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV6
#define ARAD_PP_L3_NEXT_PRTCL_TYPE_MPLS                      SOC_PPC_L3_NEXT_PRTCL_TYPE_MPLS
#define ARAD_PP_NOF_L3_NEXT_PRTCL_TYPES                      SOC_PPC_NOF_L3_NEXT_PRTCL_TYPES
typedef SOC_PPC_L3_NEXT_PRTCL_TYPE                             ARAD_PP_L3_NEXT_PRTCL_TYPE;

#define ARAD_PP_PKT_TERM_TYPE_NONE                           SOC_PPC_PKT_TERM_TYPE_NONE
#define ARAD_PP_PKT_TERM_TYPE_ETH                            SOC_PPC_PKT_TERM_TYPE_ETH
#define ARAD_PP_PKT_TERM_TYPE_IPV4_ETH                       SOC_PPC_PKT_TERM_TYPE_IPV4_ETH
#define ARAD_PP_PKT_TERM_TYPE_MPLS_ETH                       SOC_PPC_PKT_TERM_TYPE_MPLS_ETH
#define ARAD_PP_PKT_TERM_TYPE_CW_MPLS_ETH                    SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH
#define ARAD_PP_PKT_TERM_TYPE_MPLS2_ETH                      SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH
#define ARAD_PP_PKT_TERM_TYPE_CW_MPLS2_ETH                   SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH
#define ARAD_PP_PKT_TERM_TYPE_MPLS3_ETH                      SOC_PPC_PKT_TERM_TYPE_MPLS3_ETH
#define ARAD_PP_PKT_TERM_TYPE_CW_MPLS3_ETH                   SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH
#define ARAD_PP_PKT_TERM_TYPE_TRILL                          SOC_PPC_PKT_TERM_TYPE_TRILL
#define ARAD_PP_NOF_PKT_TERM_TYPES                           SOC_PPC_NOF_PKT_TERM_TYPES
typedef SOC_PPC_PKT_TERM_TYPE                                  ARAD_PP_PKT_TERM_TYPE;

#define ARAD_PP_PKT_FRWRD_TYPE_BRIDGE                        SOC_PPC_PKT_FRWRD_TYPE_BRIDGE
#define ARAD_PP_PKT_FRWRD_TYPE_IPV4_UC                       SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC
#define ARAD_PP_PKT_FRWRD_TYPE_IPV4_MC                       SOC_PPC_PKT_FRWRD_TYPE_IPV4_MC
#define ARAD_PP_PKT_FRWRD_TYPE_IPV6_UC                       SOC_PPC_PKT_FRWRD_TYPE_IPV6_UC
#define ARAD_PP_PKT_FRWRD_TYPE_IPV6_MC                       SOC_PPC_PKT_FRWRD_TYPE_IPV6_MC
#define ARAD_PP_PKT_FRWRD_TYPE_MPLS                          SOC_PPC_PKT_FRWRD_TYPE_MPLS
#define ARAD_PP_PKT_FRWRD_TYPE_TRILL                         SOC_PPC_PKT_FRWRD_TYPE_TRILL
#define ARAD_PP_PKT_FRWRD_TYPE_CPU_TRAP                      SOC_PPC_PKT_FRWRD_TYPE_CPU_TRAP
#define ARAD_PP_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM             SOC_PPC_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM
#define ARAD_PP_PKT_FRWRD_TYPE_CUSTOM1                       SOC_PPC_PKT_FRWRD_TYPE_CUSTOM1
#define ARAD_PP_PKT_FRWRD_TYPE_TM                            SOC_PPC_PKT_FRWRD_TYPE_TM
#define ARAD_PP_NOF_PKT_FRWRD_TYPES                          SOC_PPC_NOF_PKT_FRWRD_TYPES
typedef SOC_PPC_PKT_FRWRD_TYPE                                 ARAD_PP_PKT_FRWRD_TYPE;

#define ARAD_PP_PKT_HDR_TYPE_NONE                            SOC_PPC_PKT_HDR_TYPE_NONE
#define ARAD_PP_PKT_HDR_TYPE_ETH                             SOC_PPC_PKT_HDR_TYPE_ETH
#define ARAD_PP_PKT_HDR_TYPE_IPV4                            SOC_PPC_PKT_HDR_TYPE_IPV4
#define ARAD_PP_PKT_HDR_TYPE_IPV6                            SOC_PPC_PKT_HDR_TYPE_IPV6
#define ARAD_PP_PKT_HDR_TYPE_MPLS                            SOC_PPC_PKT_HDR_TYPE_MPLS
#define ARAD_PP_PKT_HDR_TYPE_TRILL                           SOC_PPC_PKT_HDR_TYPE_TRILL
#define ARAD_PP_NOF_PKT_HDR_TYPES                            SOC_PPC_NOF_PKT_HDR_TYPES
typedef SOC_PPC_PKT_HDR_TYPE                                   ARAD_PP_PKT_HDR_TYPE;

#define ARAD_PP_PKT_HDR_STK_TYPE_ETH                         SOC_PPC_PKT_HDR_STK_TYPE_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_ETH_ETH                     SOC_PPC_PKT_HDR_STK_TYPE_ETH_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_IPV4_ETH                    SOC_PPC_PKT_HDR_STK_TYPE_IPV4_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_ETH_IPV4_ETH                SOC_PPC_PKT_HDR_STK_TYPE_ETH_IPV4_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_IPV6_ETH                    SOC_PPC_PKT_HDR_STK_TYPE_IPV6_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_MPLS1_ETH                   SOC_PPC_PKT_HDR_STK_TYPE_MPLS1_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_FC_ENCAP_ETH               SOC_PPC_PKT_HDR_STK_TYPE_FC_ENCAP_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_FC_STD_ETH                 SOC_PPC_PKT_HDR_STK_TYPE_FC_STD_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_MPLS2_ETH                   SOC_PPC_PKT_HDR_STK_TYPE_MPLS2_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_MPLS3_ETH                   SOC_PPC_PKT_HDR_STK_TYPE_MPLS3_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_ETH_TRILL_ETH               SOC_PPC_PKT_HDR_STK_TYPE_ETH_TRILL_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH               SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH               SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH               SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH               SOC_PPC_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH               SOC_PPC_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH
#define ARAD_PP_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH
#define ARAD_PP_NOF_PKT_HDR_STK_TYPES                        SOC_PPC_NOF_PKT_HDR_STK_TYPES
typedef SOC_PPC_PKT_HDR_STK_TYPE                               ARAD_PP_PKT_HDR_STK_TYPE;

#define ARAD_PP_IP_ROUTING_TABLE_ITER_TYPE_FAST              SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_FAST
#define ARAD_PP_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED
#define ARAD_PP_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED
#define ARAD_PP_NOF_IP_ROUTING_TABLE_ITER_TYPES              SOC_PPC_NOF_IP_ROUTING_TABLE_ITER_TYPES
typedef SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE                     ARAD_PP_IP_ROUTING_TABLE_ITER_TYPE;

#define ARAD_PP_HASH_MASKS_MAC_SA                            SOC_PPC_HASH_MASKS_MAC_SA
#define ARAD_PP_HASH_MASKS_MAC_DA                            SOC_PPC_HASH_MASKS_MAC_DA
#define ARAD_PP_HASH_MASKS_VSI                               SOC_PPC_HASH_MASKS_VSI
#define ARAD_PP_HASH_MASKS_ETH_TYPE_CODE                     SOC_PPC_HASH_MASKS_ETH_TYPE_CODE
#define ARAD_PP_HASH_MASKS_MPLS_LABEL_1                      SOC_PPC_HASH_MASKS_MPLS_LABEL_1
#define ARAD_PP_HASH_MASKS_MPLS_LABEL_2                      SOC_PPC_HASH_MASKS_MPLS_LABEL_2
#define ARAD_PP_HASH_MASKS_MPLS_LABEL_3                      SOC_PPC_HASH_MASKS_MPLS_LABEL_3
#define ARAD_PP_HASH_MASKS_IPV4_SIP                          SOC_PPC_HASH_MASKS_IPV4_SIP
#define ARAD_PP_HASH_MASKS_IPV4_DIP                          SOC_PPC_HASH_MASKS_IPV4_DIP
#define ARAD_PP_HASH_MASKS_IPV4_PROTOCOL                     SOC_PPC_HASH_MASKS_IPV4_PROTOCOL
#define ARAD_PP_HASH_MASKS_IPV6_SIP                          SOC_PPC_HASH_MASKS_IPV6_SIP
#define ARAD_PP_HASH_MASKS_IPV6_DIP                          SOC_PPC_HASH_MASKS_IPV6_DIP
#define ARAD_PP_HASH_MASKS_IPV6_PROTOCOL                     SOC_PPC_HASH_MASKS_IPV6_PROTOCOL
#define ARAD_PP_HASH_MASKS_L4_SRC_PORT                       SOC_PPC_HASH_MASKS_L4_SRC_PORT
#define ARAD_PP_HASH_MASKS_L4_DEST_PORT                      SOC_PPC_HASH_MASKS_L4_DEST_PORT
#define ARAD_PP_HASH_MASKS_FC_DEST_ID                        SOC_PPC_HASH_MASKS_FC_DEST_ID
#define ARAD_PP_HASH_MASKS_FC_SRC_ID                         SOC_PPC_HASH_MASKS_FC_SRC_ID
#define ARAD_PP_HASH_MASKS_FC_SEQ_ID                         SOC_PPC_HASH_MASKS_FC_SEQ_ID
#define ARAD_PP_HASH_MASKS_FC_ORG_EX_ID                      SOC_PPC_HASH_MASKS_FC_ORG_EX_ID
#define ARAD_PP_HASH_MASKS_FC_RES_EX_ID                      SOC_PPC_HASH_MASKS_FC_RES_EX_ID
#define ARAD_PP_HASH_MASKS_FC_RES_EX_ID                      SOC_PPC_HASH_MASKS_FC_RES_EX_ID
#define ARAD_PP_HASH_MASKS_FC_VFI                            SOC_PPC_HASH_MASKS_FC_VFI
#define ARAD_PP_HASH_MASKS_TRILL_EG_NICK                     SOC_PPC_HASH_MASKS_TRILL_EG_NICK
#define ARAD_PP_HASH_MASKS_MPLS_LABEL_4                      SOC_PPC_HASH_MASKS_MPLS_LABEL_4
#define ARAD_PP_HASH_MASKS_MPLS_LABEL_5                      SOC_PPC_HASH_MASKS_MPLS_LABEL_5
#define ARAD_PP_NOF_HASH_MASKS                               SOC_PPC_NOF_HASH_MASKS
typedef SOC_PPC_HASH_MASKS                                     ARAD_PP_HASH_MASKS;

typedef SOC_PPC_TRAP_INFO                                      ARAD_PP_TRAP_INFO;
typedef SOC_PPC_OUTLIF                                         ARAD_PP_OUTLIF;
typedef SOC_PPC_MPLS_COMMAND                                   ARAD_PP_MPLS_COMMAND;
typedef SOC_PPC_EEI_VAL                                        ARAD_PP_EEI_VAL;
typedef SOC_PPC_EEI                                            ARAD_PP_EEI;
typedef SOC_PPC_FRWRD_DECISION_TYPE_INFO                       ARAD_PP_FRWRD_DECISION_TYPE_INFO;
typedef SOC_PPC_FRWRD_DECISION_INFO                            ARAD_PP_FRWRD_DECISION_INFO;
typedef SOC_PPC_ACTION_PROFILE                                 ARAD_PP_ACTION_PROFILE;
typedef SOC_PPC_TPID_PROFILE                                   ARAD_PP_TPID_PROFILE;
typedef SOC_PPC_PEP_KEY                                        ARAD_PP_PEP_KEY;
typedef SOC_PPC_IP_ROUTING_TABLE_ITER                          ARAD_PP_IP_ROUTING_TABLE_ITER;
typedef SOC_PPC_IP_ROUTING_TABLE_RANGE                         ARAD_PP_IP_ROUTING_TABLE_RANGE;

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

void
  ARAD_PP_TRAP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_INFO *info
  );

void
  ARAD_PP_OUTLIF_clear(
    SOC_SAND_OUT ARAD_PP_OUTLIF *info
  );

void
  ARAD_PP_MPLS_COMMAND_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_COMMAND *info
  );

void
  ARAD_PP_EEI_VAL_clear(
    SOC_SAND_OUT ARAD_PP_EEI_VAL *info
  );

void
  ARAD_PP_EEI_clear(
    SOC_SAND_OUT ARAD_PP_EEI *info
  );

void
  ARAD_PP_FRWRD_DECISION_TYPE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_DECISION_TYPE_INFO *info
  );

void
  ARAD_PP_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_DECISION_INFO *info
  );

void
  ARAD_PP_ACTION_PROFILE_clear(
    SOC_SAND_OUT ARAD_PP_ACTION_PROFILE *info
  );

void
  ARAD_PP_TPID_PROFILE_clear(
    SOC_SAND_OUT ARAD_PP_TPID_PROFILE *info
  );

void
  ARAD_PP_PEP_KEY_clear(
    SOC_SAND_OUT ARAD_PP_PEP_KEY *info
  );

void
  ARAD_PP_IP_ROUTING_TABLE_ITER_clear(
    SOC_SAND_OUT ARAD_PP_IP_ROUTING_TABLE_ITER *info
  );

void
  ARAD_PP_IP_ROUTING_TABLE_RANGE_clear(
    SOC_SAND_OUT ARAD_PP_IP_ROUTING_TABLE_RANGE *info
  );

/*********************************************************************
* NAME:
 *   arad_pp_l2_next_prtcl_type_allocate_test
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Checks whether l2_next_prtcl_type can be successfully allocated.
 * INPUT:
 *   SOC_SAND_IN  int                                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                               l2_next_prtcl_type -
 *     Ethernet Type.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                             *success -
 *     Whether the set operation succeeded. Operation may fail
 *     if there are no available resources to support the given
 *     (new) Ethernet Type.
 * REMARKS:
 *   Arad-only.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_l2_next_prtcl_type_allocate_test(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               l2_next_prtcl_type,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE            *success
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_EEI_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_EEI_TYPE enum_val
  );

const char*
  ARAD_PP_OUTLIF_ENCODE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_OUTLIF_ENCODE_TYPE enum_val
  );

const char*
  ARAD_PP_FRWRD_DECISION_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_TYPE enum_val
  );

const char*
  ARAD_PP_MPLS_COMMAND_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_MPLS_COMMAND_TYPE enum_val
  );

const char*
  ARAD_PP_L2_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_L2_NEXT_PRTCL_TYPE enum_val
  );

const char*
  ARAD_PP_L3_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_L3_NEXT_PRTCL_TYPE enum_val
  );

const char*
  ARAD_PP_PKT_TERM_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_PKT_TERM_TYPE enum_val
  );

const char*
  ARAD_PP_PKT_FRWRD_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_PKT_FRWRD_TYPE enum_val
  );

const char*
  ARAD_PP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_PKT_HDR_TYPE enum_val
  );

const char*
  ARAD_PP_PKT_HDR_STK_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_PKT_HDR_STK_TYPE enum_val
  );

const char*
  ARAD_PP_IP_ROUTING_TABLE_ITER_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_IP_ROUTING_TABLE_ITER_TYPE enum_val
  );

const char*
  ARAD_PP_HASH_MASKS_to_string(
    SOC_SAND_IN  ARAD_PP_HASH_MASKS enum_val
  );

void
  ARAD_PP_TRAP_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_INFO *info
  );

void
  ARAD_PP_OUTLIF_print(
    SOC_SAND_IN  ARAD_PP_OUTLIF *info
  );

void
  ARAD_PP_MPLS_COMMAND_print(
    SOC_SAND_IN  ARAD_PP_MPLS_COMMAND *info
  );

void
  ARAD_PP_EEI_VAL_print(
    SOC_SAND_IN  ARAD_PP_EEI_VAL *info
  );

void
  ARAD_PP_EEI_print(
    SOC_SAND_IN  ARAD_PP_EEI *info
  );

void
  ARAD_PP_FRWRD_DECISION_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE      frwrd_type,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_TYPE_INFO *info
  );

void
  ARAD_PP_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_INFO *info
  );

void
  ARAD_PP_ACTION_PROFILE_print(
    SOC_SAND_IN  ARAD_PP_ACTION_PROFILE *info
  );

void
  ARAD_PP_TPID_PROFILE_print(
    SOC_SAND_IN  ARAD_PP_TPID_PROFILE *info
  );

void
  ARAD_PP_PEP_KEY_print(
    SOC_SAND_IN  ARAD_PP_PEP_KEY *info
  );

void
  ARAD_PP_IP_ROUTING_TABLE_ITER_print(
    SOC_SAND_IN  ARAD_PP_IP_ROUTING_TABLE_ITER *info
  );

void
  ARAD_PP_IP_ROUTING_TABLE_RANGE_print(
    SOC_SAND_IN  ARAD_PP_IP_ROUTING_TABLE_RANGE *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */




/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_GENERAL_INCLUDED__*/
#endif


