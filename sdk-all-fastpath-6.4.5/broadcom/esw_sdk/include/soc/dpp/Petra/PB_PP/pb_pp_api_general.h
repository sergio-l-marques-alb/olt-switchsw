/* $Id: soc_pb_pp_api_general.h,v 1.10 Broadcom SDK $
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

#ifndef __SOC_PB_PP_API_GENERAL_INCLUDED__
/* { */
#define __SOC_PB_PP_API_GENERAL_INCLUDED__

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
#define  SOC_PB_PP_RIF_NULL (SOC_PPC_RIF_NULL)

/*     Indicates that the EEP is not used.                     */
#define  SOC_PB_PP_EEP_NULL (SOC_PPC_EEP_NULL)

/*     Maximum Number of local PP ports in one device.         */
#define  SOC_PB_PP_MAX_NOF_LOCAL_PORTS (SOC_PPC_MAX_NOF_LOCAL_PORTS_PETRA)

/*     ignore given value     */
#define  SOC_PB_PP_IGNORE_VAL 0xFFFFFFFF

#define SOC_PB_PP_DEBUG_IS_LVL1               (SOC_PPC_DEBUG_IS_LVL1)

#define SOC_PB_PP_VLAN_TAGS_MAX               (SOC_PPC_VLAN_TAGS_MAX)

/* $Id: soc_pb_pp_api_general.h,v 1.10 Broadcom SDK $
 *	Designates invalid/non-existing AC-id
 */
#define SOC_PB_PP_AC_ID_INVALID               (SOC_PPC_AC_ID_INVALID)

#define SOC_PB_PP_DP_MAX_VAL                  (SOC_PETRA_MAX_DROP_PRECEDENCE)

#define SOC_PB_PP_CUD_EEP_PREFIX   (0x0)
#define SOC_PB_PP_CUD_VSI_PREFIX   (0x4000)
#define SOC_PB_PP_CUD_AC_PREFIX    (0x8000)
/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PB_PP_DO_NOTHING_AND_EXIT                                         \
          SOC_SAND_IGNORE_UNUSED_VAR(res);                                    \
          goto exit

#define SOC_PB_PP_STRUCT_VERIFY(type, name, exit_num, exit_place)             \
          do                                                              \
          {                                                               \
            res = type##_verify(                                          \
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
#define SOC_PB_PP_FRWRD_DECISION_DROP_SET(unit, fwd_decision)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_DROP;  \
  (fwd_decision)->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;
/*
 *  Destination is the local CPU. Set 'fwd_decision' to local
 *  CPU (i.e. local port 0). Packet forwarded according to
 *  this 'fwd_decision' is forwarded to CPU (not trapped,
 *  i.e., with no trap-code attached to it)
 */
#define SOC_PB_PP_FRWRD_DECISION_LOCAL_CPU_SET(unit, fwd_decision)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT; \
  (fwd_decision)->dest_id = SOC_PETRA_FRST_CPU_PORT_ID; \
  (fwd_decision)->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;

/*
 *  Destination is a physical system port. Set the
 *  'fwd_decision' to include the destination physical
 *  system port (0 to 4K-1).
 */
#define SOC_PB_PP_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, fwd_decision,phy_port)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT; \
  (fwd_decision)->dest_id = phy_port;    \
  (fwd_decision)->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;

/*
 *  Destination is a LAG. Set the 'fwd_decision' to include
 *  the LAG ID.
 */
#define SOC_PB_PP_FRWRD_DECISION_LAG_SET(unit, fwd_decision,lag_id)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG; \
  (fwd_decision)->dest_id = lag_id;  \
  (fwd_decision)->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;

/*
 *  Destination is a multicast group. Set the 'fwd_decision'
 *  to include MC-group ID
 */
#define SOC_PB_PP_FRWRD_DECISION_MC_GROUP_SET(unit, fwd_decision,mc_id)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_MC; \
  (fwd_decision)->dest_id = mc_id;\
  (fwd_decision)->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY;
/*
 *  Destination is a FEC-entry. Set the 'fwd_decision' to
 *  include a pointer to the FEC table
 */
#define SOC_PB_PP_FRWRD_DECISION_FEC_SET(unit, fwd_decision,fec_id)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_id;\
  (fwd_decision)->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY;

/*
 *  Destination with COS (i.e., explicit TM flow). Set the
 *  'fwd_decision' to include the explicit TM flow_id
 */
#define SOC_PB_PP_FRWRD_DECISION_EXPL_FLOW_SET(unit, fwd_decision,flow_id)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW; \
  (fwd_decision)->dest_id = flow_id;\
  (fwd_decision)->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;

/*
 *  Trap packet. Set the 'fwd_decision' to Trap the packet
 *  using the following attributes: - trap_code : 0-255;
 *  identifies the trap/snoop actions to be applied if the
 *  assigned strength is higher than the previously assigned
 *  strength.- fwd_strength: 0-7- snp_strenght: 0-3
 */
#define SOC_PB_PP_FRWRD_DECISION_TRAP_SET(unit, fwd_decision,_trap_code,frwrd_strength,snp_strength)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP; \
  (fwd_decision)->dest_id = 0;              \
  (fwd_decision)->additional_info.trap_info.action_profile.trap_code = _trap_code;       \
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
#define SOC_PB_PP_FRWRD_DECISION_AC_SET(unit, fwd_decision, ac_id, is_lag, sys_port_id)  \
  (fwd_decision)->type = (is_lag)?SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG:SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT; \
  (fwd_decision)->dest_id = sys_port_id;  \
  (fwd_decision)->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC; \
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
#define SOC_PB_PP_FRWRD_DECISION_AC_WITH_COSQ_SET(unit, fwd_decision,ac_id,flow_id)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW; \
  (fwd_decision)->dest_id = flow_id;  \
  (fwd_decision)->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC; \
  (fwd_decision)->additional_info.outlif.val = ac_id;

/*
 *  Forward to access associated with AC-id with
 *  protection. Set the 'fwd_decision' to include AC-id with
 *  FEC-index. Packet forwarded according to this
 *  'fwd_decision' is forwarded according the FEC entry
 *  setting associated with the given (out) AC-id. This
 *  forwarding decision can be learned.
 */
#define SOC_PB_PP_FRWRD_DECISION_PROTECTED_AC_SET(unit, fwd_decision, ac_id, fec_index)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY; \

/*
 *  VPLS access to core with no protection (neither in the PWE
 *  nor on the tunnel). Set the 'fwd_decision' to include pwe-id
 *  and system-port. Packet forwarded according to this
 *  'fwd_decision' is forwarded to sys_port encapsulated according
 *  to pwe_id setting. This forwarding decision can be learned as well.
 */
#define SOC_PB_PP_FRWRD_DECISION_PWE_SET(unit, fwd_decision, pwe_id, is_lag, sys_port_id)  \
  (fwd_decision)->type = (is_lag)?SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG:SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT; \
  (fwd_decision)->dest_id = sys_port_id;  \
  (fwd_decision)->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP; \
  (fwd_decision)->additional_info.outlif.val = pwe_id;
/*
 *
 */
#define SOC_PB_PP_FRWRD_DECISION_TRILL_SET(unit, fwd_decision, nick, is_multi, destination_id)  \
  (fwd_decision)->type = (is_multi)?SOC_PB_PP_FRWRD_DECISION_TYPE_MC:SOC_PB_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = destination_id;  \
  (fwd_decision)->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_TRILL;  \
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
#define SOC_PB_PP_FRWRD_DECISION_PWE_WITH_COSQ_SET(unit, fwd_decision, pwe_id,flow_id)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW; \
  (fwd_decision)->dest_id = flow_id;  \
  (fwd_decision)->additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP; \
  (fwd_decision)->additional_info.outlif.val = pwe_id;

/*
 *  VPLS access to core with protection on tunnel only. Set
 *  the 'fwd_decision' to include fec-index VC-label. Packet
 *  forwarded according to this 'fwd_decision' is
 *  encapsulated with 'vc_label'. EXP,TTL is set according
 *  to 'push_profile' definition see. This forwarding
 *  decision can be learned as well.
 */
#define SOC_PB_PP_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_SET(unit, fwd_decision,vc_label, prm_push_profile,fec_index)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_MPLS; \
  (fwd_decision)->additional_info.eei.val.mpls_command.command = SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH;  \
  (fwd_decision)->additional_info.eei.val.mpls_command.label = vc_label;    \
  (fwd_decision)->additional_info.eei.val.mpls_command.push_profile = prm_push_profile;

/*
 *  VPLS access to core with protection on PWE. Set the
 *  'fwd_decision' to include fec-index. Packet forwarded
 *  according to this 'fwd_decision' is forwarded according
 *  to FEC entry setting. By this setting the PWE can be
 *  protected. This forwarding decision can be learned as
 *  well.
 */
#define SOC_PB_PP_FRWRD_DECISION_PROTECTED_PWE_SET(unit, fwd_decision, fec_index)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY;

/*
 *  ILM entry. Set the 'fwd_decision' to include swap-label
 *  and fec-index label. For Packets forwarded according to
 *  this 'fwd_decision' MPLS label is swappedAnd forwarded
 *  according to FEC entry setting
 */
#define SOC_PB_PP_FRWRD_DECISION_ILM_SWAP_SET(unit, fwd_decision,swap_label,fec_index)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_MPLS; \
  (fwd_decision)->additional_info.eei.val.mpls_command.command = SOC_PB_PP_MPLS_COMMAND_TYPE_SWAP;  \
  (fwd_decision)->additional_info.eei.val.mpls_command.label = swap_label;    \
  (fwd_decision)->additional_info.eei.val.mpls_command.push_profile = 0;

/*
 *  ILM Push entry. Set the 'fwd_decision' to include label
 *  and fec-index label. For Packets forwarded according to
 *  this 'fwd_decision' MPLS label is pushed And forwarded
 *  according to FEC entry setting
 */
#define SOC_PB_PP_FRWRD_DECISION_ILM_PUSH_SET(unit, fwd_decision,label, push_profile, fec_index)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_index;  \
  (fwd_decision)->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_MPLS; \
  (fwd_decision)->additional_info.eei.val.mpls_command.command = SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH;  \
  (fwd_decision)->additional_info.eei.val.mpls_command.label = label;    \
  (fwd_decision)->additional_info.eei.val.mpls_command.push_profile = push_profile;

/*
 *  Mac in mac entry. Destination is a FEC-entry. Set the 'fwd_decision' to
 *  include a pointer to the FEC table. EEI is the isid_id.
 */
#define SOC_PB_PP_FRWRD_DECISION_MAC_IN_MAC_SET(unit, fwd_decision,isid_id, fec_id)  \
  (fwd_decision)->type = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC; \
  (fwd_decision)->dest_id = fec_id;\
  (fwd_decision)->additional_info.eei.type = SOC_PB_PP_EEI_TYPE_MIM;\
  (fwd_decision)->additional_info.eei.val.isid = isid_id;


/************************************************************************/
/* CUD macros                                                                     */
/************************************************************************/

/*
 * Returns CUD that includes EEP with value 'eep_ndx'
 */
#define SOC_PB_PP_CUD_EEP_GET(unit, eep_ndx)   \
  (SOC_PB_PP_CUD_EEP_PREFIX | (eep_ndx))
/*
 * Returns CUD that includes VSI with value 'vsi_ndx'
 */
#define SOC_PB_PP_CUD_VSI_GET(unit, vsi_ndx)   \
  (SOC_PB_PP_CUD_VSI_PREFIX | (vsi_ndx))
/*
 * Returns CUD that includes AC with value 'ac_ndx'
 */
#define SOC_PB_PP_CUD_AC_GET(unit, ac_ndx)   \
  (SOC_PB_PP_CUD_AC_PREFIX | (ac_ndx))
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
typedef SOC_PPC_TM_PORT                                        SOC_PB_PP_TM_PORT;

/*
 *  Local PP port. Range : 0 - 63.
 */
typedef SOC_PPC_PORT                                           SOC_PB_PP_PORT;

/*
 *  Filtering ID. Range: Soc_petraB: 0 - 16K-1. T20E: 0-64K-1.
 */
typedef SOC_PPC_FID                                            SOC_PB_PP_FID;

/*
 *  Virtual switch instance ID. Range: 0 - 16K-1.
 */
typedef SOC_PPC_VSI_ID                                         SOC_PB_PP_VSI_ID;

/*
 *  System VSI. Range: 0 - 64K-1.
 */
typedef SOC_PPC_SYS_VSI_ID                                     SOC_PB_PP_SYS_VSI_ID;

/*
 *  Forwarding Equivalence Class ID. Range: 0 - 16K-1.
 */
typedef SOC_PPC_FEC_ID                                         SOC_PB_PP_FEC_ID;

/*
 *  Virtual Router ID. Range: 1 - 255.
 */
typedef SOC_PPC_VRF_ID                                         SOC_PB_PP_VRF_ID;

/*
 *  Attachment Circuit ID. Range: Soc_petraB: 0 - 16K-1. T20E: 0
 *  - 64K-1.
 */
typedef SOC_PPC_AC_ID                                          SOC_PB_PP_AC_ID;

/*
 *  Router Interface ID. Range: 0 - 4K-1.
 */
typedef SOC_PPC_RIF_ID                                         SOC_PB_PP_RIF_ID;

/*
 *  Logical Interface ID. Range: Soc_petraB: 0 - 16K-1. T20E: 0
 *  - 64K-1.
 */
typedef SOC_PPC_LIF_ID                                         SOC_PB_PP_LIF_ID;


/*
 *  MP Level.
 */
typedef SOC_PPC_MP_LEVEL                                       SOC_PB_PP_MP_LEVEL;


#define SOC_PB_PP_EEI_TYPE_EMPTY                               SOC_PPC_EEI_TYPE_EMPTY
#define SOC_PB_PP_EEI_TYPE_TRILL                               SOC_PPC_EEI_TYPE_TRILL
#define SOC_PB_PP_EEI_TYPE_MPLS                                SOC_PPC_EEI_TYPE_MPLS
#define SOC_PB_PP_EEI_TYPE_MIM                                 SOC_PPC_EEI_TYPE_MIM
#define SOC_PB_PP_NOF_EEI_TYPES                                SOC_PPC_NOF_EEI_TYPES_PB
typedef SOC_PPC_EEI_TYPE                                       SOC_PB_PP_EEI_TYPE;

#define SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE                      SOC_PPC_OUTLIF_ENCODE_TYPE_NONE
#define SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC                        SOC_PPC_OUTLIF_ENCODE_TYPE_AC
#define SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP                       SOC_PPC_OUTLIF_ENCODE_TYPE_EEP
#define SOC_PB_PP_OUTLIF_ENCODE_TYPE_VSI                       SOC_PPC_OUTLIF_ENCODE_TYPE_VSI
#define SOC_PB_PP_NOF_OUTLIF_ENCODE_TYPES                      SOC_PPC_NOF_OUTLIF_ENCODE_TYPES
typedef SOC_PPC_OUTLIF_ENCODE_TYPE                             SOC_PB_PP_OUTLIF_ENCODE_TYPE;

#define SOC_PB_PP_FRWRD_DECISION_TYPE_DROP                     SOC_PPC_FRWRD_DECISION_TYPE_DROP
#define SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW                  SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW
#define SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG                   SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG
#define SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT                  SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT
#define SOC_PB_PP_FRWRD_DECISION_TYPE_MC                       SOC_PPC_FRWRD_DECISION_TYPE_MC
#define SOC_PB_PP_FRWRD_DECISION_TYPE_FEC                      SOC_PPC_FRWRD_DECISION_TYPE_FEC
#define SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP                     SOC_PPC_FRWRD_DECISION_TYPE_TRAP
#define SOC_PB_PP_NOF_FRWRD_DECISION_TYPES                     SOC_PPC_NOF_FRWRD_DECISION_TYPES
typedef SOC_PPC_FRWRD_DECISION_TYPE                            SOC_PB_PP_FRWRD_DECISION_TYPE;

#define SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH                       SOC_PPC_MPLS_COMMAND_TYPE_PUSH
#define SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE         SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE
#define SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM      SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM
#define SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE         SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE
#define SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM      SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM
#define SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE         SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE
#define SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM      SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM
#define SOC_PB_PP_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET          SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET
#define SOC_PB_PP_MPLS_COMMAND_TYPE_SWAP                       SOC_PPC_MPLS_COMMAND_TYPE_SWAP
#define SOC_PB_PP_NOF_MPLS_COMMAND_TYPES                       SOC_PPC_NOF_MPLS_COMMAND_TYPES
typedef SOC_PPC_MPLS_COMMAND_TYPE                              SOC_PB_PP_MPLS_COMMAND_TYPE;

#define SOC_PB_PP_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC                SOC_PPC_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC
#define SOC_PB_PP_L2_NEXT_PRTCL_TYPE_IPV4                      SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV4
#define SOC_PB_PP_L2_NEXT_PRTCL_TYPE_IPV6                      SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV6
#define SOC_PB_PP_L2_NEXT_PRTCL_TYPE_ARP                       SOC_PPC_L2_NEXT_PRTCL_TYPE_ARP
#define SOC_PB_PP_L2_NEXT_PRTCL_TYPE_CFM                       SOC_PPC_L2_NEXT_PRTCL_TYPE_CFM
#define SOC_PB_PP_L2_NEXT_PRTCL_TYPE_FC_ETH                    SOC_PPC_L2_NEXT_PRTCL_TYPE_FC_ETH
#define SOC_PB_PP_L2_NEXT_PRTCL_TYPE_OTHER                     SOC_PPC_L2_NEXT_PRTCL_TYPE_OTHER
#define SOC_PB_PP_NOF_L2_NEXT_PRTCL_TYPES                      SOC_PPC_NOF_L2_NEXT_PRTCL_TYPES
typedef SOC_PPC_L2_NEXT_PRTCL_TYPE                             SOC_PB_PP_L2_NEXT_PRTCL_TYPE;

#define SOC_PB_PP_L3_NEXT_PRTCL_TYPE_NONE                      SOC_PPC_L3_NEXT_PRTCL_TYPE_NONE
#define SOC_PB_PP_L3_NEXT_PRTCL_TYPE_TCP                       SOC_PPC_L3_NEXT_PRTCL_TYPE_TCP
#define SOC_PB_PP_L3_NEXT_PRTCL_TYPE_UDP                       SOC_PPC_L3_NEXT_PRTCL_TYPE_UDP
#define SOC_PB_PP_L3_NEXT_PRTCL_TYPE_IGMP                      SOC_PPC_L3_NEXT_PRTCL_TYPE_IGMP
#define SOC_PB_PP_L3_NEXT_PRTCL_TYPE_ICMP                      SOC_PPC_L3_NEXT_PRTCL_TYPE_ICMP
#define SOC_PB_PP_L3_NEXT_PRTCL_TYPE_ICMPV6                    SOC_PPC_L3_NEXT_PRTCL_TYPE_ICMPV6
#define SOC_PB_PP_L3_NEXT_PRTCL_TYPE_IPV4                      SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV4
#define SOC_PB_PP_L3_NEXT_PRTCL_TYPE_IPV6                      SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV6
#define SOC_PB_PP_L3_NEXT_PRTCL_TYPE_MPLS                      SOC_PPC_L3_NEXT_PRTCL_TYPE_MPLS
#define SOC_PB_PP_NOF_L3_NEXT_PRTCL_TYPES                      SOC_PPC_NOF_L3_NEXT_PRTCL_TYPES
typedef SOC_PPC_L3_NEXT_PRTCL_TYPE                             SOC_PB_PP_L3_NEXT_PRTCL_TYPE;

#define SOC_PB_PP_PKT_TERM_TYPE_MPLS_ETH                       SOC_PPC_PKT_TERM_TYPE_MPLS_ETH
#define SOC_PB_PP_PKT_TERM_TYPE_CW_MPLS_ETH                    SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH
#define SOC_PB_PP_PKT_TERM_TYPE_MPLS2_ETH                      SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH
#define SOC_PB_PP_PKT_TERM_TYPE_CW_MPLS2_ETH                   SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH
#define SOC_PB_PP_PKT_TERM_TYPE_MPLS3_ETH                      SOC_PPC_PKT_TERM_TYPE_MPLS3_ETH
#define SOC_PB_PP_PKT_TERM_TYPE_CW_MPLS3_ETH                   SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH
typedef SOC_PPC_PKT_TERM_TYPE                                  SOC_PB_PP_PKT_TERM_TYPE;

#define SOC_PB_PP_PKT_FRWRD_TYPE_BRIDGE                        SOC_PPC_PKT_FRWRD_TYPE_BRIDGE
#define SOC_PB_PP_PKT_FRWRD_TYPE_IPV4_UC                       SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC
#define SOC_PB_PP_PKT_FRWRD_TYPE_IPV4_MC                       SOC_PPC_PKT_FRWRD_TYPE_IPV4_MC
#define SOC_PB_PP_PKT_FRWRD_TYPE_IPV6_UC                       SOC_PPC_PKT_FRWRD_TYPE_IPV6_UC
#define SOC_PB_PP_PKT_FRWRD_TYPE_IPV6_MC                       SOC_PPC_PKT_FRWRD_TYPE_IPV6_MC
#define SOC_PB_PP_PKT_FRWRD_TYPE_MPLS                          SOC_PPC_PKT_FRWRD_TYPE_MPLS
#define SOC_PB_PP_PKT_FRWRD_TYPE_TRILL                         SOC_PPC_PKT_FRWRD_TYPE_TRILL
#define SOC_PB_PP_PKT_FRWRD_TYPE_CPU_TRAP                      SOC_PPC_PKT_FRWRD_TYPE_CPU_TRAP
#define SOC_PB_PP_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM             SOC_PPC_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM
#define SOC_PB_PP_NOF_PKT_FRWRD_TYPES                          SOC_PPC_NOF_PKT_FRWRD_TYPES
typedef SOC_PPC_PKT_FRWRD_TYPE                                 SOC_PB_PP_PKT_FRWRD_TYPE;

#define SOC_PB_PP_PKT_HDR_TYPE_NONE                            SOC_PPC_PKT_HDR_TYPE_NONE
#define SOC_PB_PP_PKT_HDR_TYPE_ETH                             SOC_PPC_PKT_HDR_TYPE_ETH
#define SOC_PB_PP_PKT_HDR_TYPE_IPV4                            SOC_PPC_PKT_HDR_TYPE_IPV4
#define SOC_PB_PP_PKT_HDR_TYPE_IPV6                            SOC_PPC_PKT_HDR_TYPE_IPV6
#define SOC_PB_PP_PKT_HDR_TYPE_MPLS                            SOC_PPC_PKT_HDR_TYPE_MPLS
#define SOC_PB_PP_NOF_PKT_HDR_TYPES                            SOC_PPC_NOF_PKT_HDR_TYPES
typedef SOC_PPC_PKT_HDR_TYPE                                   SOC_PB_PP_PKT_HDR_TYPE;

#define SOC_PB_PP_PKT_HDR_STK_TYPE_ETH                         SOC_PPC_PKT_HDR_STK_TYPE_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_ETH                     SOC_PPC_PKT_HDR_STK_TYPE_ETH_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_ETH                    SOC_PPC_PKT_HDR_STK_TYPE_IPV4_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_ETH                    SOC_PPC_PKT_HDR_STK_TYPE_IPV6_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_MPLS1_ETH                   SOC_PPC_PKT_HDR_STK_TYPE_MPLS1_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_MPLS2_ETH                   SOC_PPC_PKT_HDR_STK_TYPE_MPLS2_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_MPLS3_ETH                   SOC_PPC_PKT_HDR_STK_TYPE_MPLS3_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_TRILL_ETH               SOC_PPC_PKT_HDR_STK_TYPE_ETH_TRILL_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH               SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH               SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH               SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH               SOC_PPC_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH               SOC_PPC_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH
#define SOC_PB_PP_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH              SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH
typedef SOC_PPC_PKT_HDR_STK_TYPE                               SOC_PB_PP_PKT_HDR_STK_TYPE;

typedef SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE                     SOC_PB_PP_IP_ROUTING_TABLE_ITER_TYPE;

#define SOC_PB_PP_HASH_MASKS_MAC_SA                            SOC_PPC_HASH_MASKS_MAC_SA
#define SOC_PB_PP_HASH_MASKS_MAC_DA                            SOC_PPC_HASH_MASKS_MAC_DA
#define SOC_PB_PP_HASH_MASKS_VSI                               SOC_PPC_HASH_MASKS_VSI
#define SOC_PB_PP_HASH_MASKS_ETH_TYPE_CODE                     SOC_PPC_HASH_MASKS_ETH_TYPE_CODE
#define SOC_PB_PP_HASH_MASKS_MPLS_LABEL_1                      SOC_PPC_HASH_MASKS_MPLS_LABEL_1
#define SOC_PB_PP_HASH_MASKS_MPLS_LABEL_2                      SOC_PPC_HASH_MASKS_MPLS_LABEL_2
#define SOC_PB_PP_HASH_MASKS_MPLS_LABEL_3                      SOC_PPC_HASH_MASKS_MPLS_LABEL_3
#define SOC_PB_PP_HASH_MASKS_IPV4_SIP                          SOC_PPC_HASH_MASKS_IPV4_SIP
#define SOC_PB_PP_HASH_MASKS_IPV4_DIP                          SOC_PPC_HASH_MASKS_IPV4_DIP
#define SOC_PB_PP_HASH_MASKS_IPV4_PROTOCOL                     SOC_PPC_HASH_MASKS_IPV4_PROTOCOL
#define SOC_PB_PP_HASH_MASKS_IPV6_SIP                          SOC_PPC_HASH_MASKS_IPV6_SIP
#define SOC_PB_PP_HASH_MASKS_IPV6_DIP                          SOC_PPC_HASH_MASKS_IPV6_DIP
#define SOC_PB_PP_HASH_MASKS_L4_SRC_PORT                       SOC_PPC_HASH_MASKS_L4_SRC_PORT
#define SOC_PB_PP_HASH_MASKS_L4_DEST_PORT                      SOC_PPC_HASH_MASKS_L4_DEST_PORT
#define SOC_PB_PP_HASH_MASKS_FC_DEST_ID                        SOC_PPC_HASH_MASKS_FC_DEST_ID
#define SOC_PB_PP_HASH_MASKS_FC_SRC_ID                         SOC_PPC_HASH_MASKS_FC_SRC_ID
#define SOC_PB_PP_HASH_MASKS_FC_SEQ_ID                         SOC_PPC_HASH_MASKS_FC_SEQ_ID
#define SOC_PB_PP_HASH_MASKS_FC_ORG_EX_ID                      SOC_PPC_HASH_MASKS_FC_ORG_EX_ID
#define SOC_PB_PP_HASH_MASKS_FC_RES_EX_ID                      SOC_PPC_HASH_MASKS_FC_RES_EX_ID
#define SOC_PB_PP_NOF_HASH_MASKS                               SOC_PPC_NOF_HASH_MASKS_PB
typedef SOC_PPC_HASH_MASKS                                     SOC_PB_PP_HASH_MASKS;

typedef SOC_PPC_TRAP_INFO                                      SOC_PB_PP_TRAP_INFO;
typedef SOC_PPC_OUTLIF                                         SOC_PB_PP_OUTLIF;
typedef SOC_PPC_MPLS_COMMAND                                   SOC_PB_PP_MPLS_COMMAND;
typedef SOC_PPC_EEI_VAL                                        SOC_PB_PP_EEI_VAL;
typedef SOC_PPC_EEI                                            SOC_PB_PP_EEI;
typedef SOC_PPC_FRWRD_DECISION_TYPE_INFO                       SOC_PB_PP_FRWRD_DECISION_TYPE_INFO;
typedef SOC_PPC_FRWRD_DECISION_INFO                            SOC_PB_PP_FRWRD_DECISION_INFO;
typedef SOC_PPC_ACTION_PROFILE                                 SOC_PB_PP_ACTION_PROFILE;
typedef SOC_PPC_TPID_PROFILE                                   SOC_PB_PP_TPID_PROFILE;
typedef SOC_PPC_PEP_KEY                                        SOC_PB_PP_PEP_KEY;
typedef SOC_PPC_IP_ROUTING_TABLE_ITER                          SOC_PB_PP_IP_ROUTING_TABLE_ITER;
typedef SOC_PPC_IP_ROUTING_TABLE_RANGE                         SOC_PB_PP_IP_ROUTING_TABLE_RANGE;

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
  SOC_PB_PP_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_INFO *info
  );

void
  SOC_PB_PP_OUTLIF_clear(
    SOC_SAND_OUT SOC_PB_PP_OUTLIF *info
  );

void
  SOC_PB_PP_MPLS_COMMAND_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_COMMAND *info
  );

void
  SOC_PB_PP_EEI_VAL_clear(
    SOC_SAND_OUT SOC_PB_PP_EEI_VAL *info
  );

void
  SOC_PB_PP_EEI_clear(
    SOC_SAND_OUT SOC_PB_PP_EEI *info
  );

void
  SOC_PB_PP_FRWRD_DECISION_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_DECISION_TYPE_INFO *info
  );

void
  SOC_PB_PP_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_DECISION_INFO *info
  );

void
  SOC_PB_PP_ACTION_PROFILE_clear(
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE *info
  );

void
  SOC_PB_PP_TPID_PROFILE_clear(
    SOC_SAND_OUT SOC_PB_PP_TPID_PROFILE *info
  );

void
  SOC_PB_PP_PEP_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_PEP_KEY *info
  );

void
  SOC_PB_PP_IP_ROUTING_TABLE_ITER_clear(
    SOC_SAND_OUT SOC_PB_PP_IP_ROUTING_TABLE_ITER *info
  );

void
  SOC_PB_PP_IP_ROUTING_TABLE_RANGE_clear(
    SOC_SAND_OUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_EEI_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EEI_TYPE enum_val
  );

const char*
  SOC_PB_PP_OUTLIF_ENCODE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_OUTLIF_ENCODE_TYPE enum_val
  );

const char*
  SOC_PB_PP_FRWRD_DECISION_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_TYPE enum_val
  );

const char*
  SOC_PB_PP_MPLS_COMMAND_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_MPLS_COMMAND_TYPE enum_val
  );

const char*
  SOC_PB_PP_L2_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_L2_NEXT_PRTCL_TYPE enum_val
  );

const char*
  SOC_PB_PP_L3_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_L3_NEXT_PRTCL_TYPE enum_val
  );

const char*
  SOC_PB_PP_PKT_TERM_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_PKT_TERM_TYPE enum_val
  );

const char*
  SOC_PB_PP_PKT_FRWRD_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_PKT_FRWRD_TYPE enum_val
  );

const char*
  SOC_PB_PP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_PKT_HDR_TYPE enum_val
  );

const char*
  SOC_PB_PP_PKT_HDR_STK_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_PKT_HDR_STK_TYPE enum_val
  );

const char*
  SOC_PB_PP_IP_ROUTING_TABLE_ITER_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_IP_ROUTING_TABLE_ITER_TYPE enum_val
  );

const char*
  SOC_PB_PP_HASH_MASKS_to_string(
    SOC_SAND_IN  SOC_PB_PP_HASH_MASKS enum_val
  );

void
  SOC_PB_PP_TRAP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_INFO *info
  );

void
  SOC_PB_PP_OUTLIF_print(
    SOC_SAND_IN  SOC_PB_PP_OUTLIF *info
  );

void
  SOC_PB_PP_MPLS_COMMAND_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_COMMAND *info
  );

void
  SOC_PB_PP_EEI_VAL_print(
    SOC_SAND_IN  SOC_PB_PP_EEI_VAL *info
  );

void
  SOC_PB_PP_EEI_print(
    SOC_SAND_IN  SOC_PB_PP_EEI *info
  );

void
  SOC_PB_PP_FRWRD_DECISION_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE      frwrd_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_TYPE_INFO *info
  );

void
  SOC_PB_PP_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO *info
  );

void
  SOC_PB_PP_ACTION_PROFILE_print(
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE *info
  );

void
  SOC_PB_PP_TPID_PROFILE_print(
    SOC_SAND_IN  SOC_PB_PP_TPID_PROFILE *info
  );

void
  SOC_PB_PP_PEP_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY *info
  );

void
  SOC_PB_PP_IP_ROUTING_TABLE_ITER_print(
    SOC_SAND_IN  SOC_PB_PP_IP_ROUTING_TABLE_ITER *info
  );

void
  SOC_PB_PP_IP_ROUTING_TABLE_RANGE_print(
    SOC_SAND_IN  SOC_PB_PP_IP_ROUTING_TABLE_RANGE *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */




/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_GENERAL_INCLUDED__*/
#endif
