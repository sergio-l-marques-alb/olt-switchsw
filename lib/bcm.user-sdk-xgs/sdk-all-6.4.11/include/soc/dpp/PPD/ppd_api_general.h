/* $Id: ppd_api_general.h,v 1.23 Broadcom SDK $
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

#ifndef __SOC_PPD_API_GENERAL_INCLUDED__
/* { */
#define __SOC_PPD_API_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_general.h>

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_general.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Indicates that the RIF ID is not used and that RIF ID
 *     and RPF info are not updated.                           */
#define  SOC_PPD_RIF_NULL (SOC_PPC_RIF_NULL)

/*     Indicates that the EEP is not used.                     */
#define  SOC_PPD_EEP_NULL (SOC_PPC_EEP_NULL)

/*     Maximum Number of local ports in one device.            */
#define  SOC_PPD_MAX_NOF_LOCAL_PORTS (SOC_SAND_MAX(SOC_PPC_MAX_NOF_LOCAL_PORTS_PETRA,SOC_PPC_MAX_NOF_LOCAL_PORTS_ARAD))

/*     ignore given value     */
#define  SOC_PPD_IGNORE_VAL 0xFFFFFFFF

#define SOC_PPD_DEBUG           (SOC_PPC_DEBUG)
#define SOC_PPD_DEBUG_IS_LVL1   (SOC_PPC_DEBUG_IS_LVL1)
#define SOC_PPD_DEBUG_IS_LVL3   (SOC_PPC_DEBUG_IS_LVL3)

#define SOC_PPD_NOF_BITS_MPLS_LABEL                (20)
/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PPD_DO_NOTHING_AND_EXIT                    \
  do                                              \
  {                                               \
    SOC_SAND_IGNORE_UNUSED_VAR(res);                  \
    goto exit;                                    \
  } while(0)


#ifdef LINK_T20E_LIBRARIES
  #define SOC_T20E_DEVICE_CALL(func, params)          \
    do                                            \
    {                                             \
      res = t20e_##func params;                   \
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);      \
    } while(0)
#else
  #define SOC_T20E_DEVICE_CALL(func, params)          \
    do                                            \
    {                                             \
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit); \
    } while(0)
#endif

#ifdef LINK_PB_LIBRARIES
  #define SOC_PB_PP_DEVICE_CALL(func, params)            \
    do                                            \
    {                                             \
      res = soc_pb_pp_##func params;                     \
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);      \
    } while(0)
#else
#define SOC_PB_PP_DEVICE_CALL(func, params)              \
    do                                            \
    {                                             \
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit); \
    } while(0)
#endif

#ifdef LINK_ARAD_LIBRARIES
  #define ARAD_PP_DEVICE_CALL(func, params)            \
    do                                            \
    {                                             \
      res = arad_pp_##func params;                     \
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);      \
    } while(0)
#else
#define ARAD_PP_DEVICE_CALL(func, params)              \
    do                                            \
    {                                             \
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit); \
    } while(0)
#endif

#ifdef LINK_PCP_LIBRARIES
  #define SOC_PCP_DEVICE_CALL(func, params)            \
    do                                            \
    {                                             \
      res = pcp_##func params;                     \
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);      \
    } while(0)
#else
#define SOC_PCP_DEVICE_CALL(func, params)              \
    do                                            \
    {                                             \
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 30, exit); \
    } while(0)
#endif

#define SOC_PPD_ARAD_TMP_DEVICE_CALL(func,params)    \
  switch (SOC_SAND_DEVICE_TYPE_GET(unit))        \
  {                                               \
    case SOC_SAND_DEV_PB:                             \
      SOC_PB_PP_DEVICE_CALL(func, params);            \
      break;                                      \
  case SOC_SAND_DEV_ARAD:                             \
	ARAD_PP_DEVICE_CALL(func, params);            \
	break;                                      \
    default:                                      \
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit); \
      break; \
  }

#define SOC_PPD_ARAD_ONLY_DEVICE_CALL(func,params)    \
  switch (SOC_SAND_DEVICE_TYPE_GET(unit))        \
  {                                               \
  case SOC_SAND_DEV_ARAD:                             \
	ARAD_PP_DEVICE_CALL(func, params);            \
	break;                                      \
    default:                                      \
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit); \
      break; \
  }

#define SOC_PPD_ARAD_OR_JERICHO_ONLY_DEVICE_CALL(func,params) \
  do {\
  if (SOC_IS_JERICHO(unit)) {\
    soc_error_t res = soc_jer_pp_##func params ;\
    SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res,222,exit,333);\
  } else if (SOC_IS_ARAD(unit)) { \
      ARAD_PP_DEVICE_CALL(func, params);            \
  } else {\
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit); \
  }\
  } while (0)

#define SOC_PPD_JERICHO_ONLY_DEVICE_CALL(func,params) \
  do {\
      if (SOC_IS_JERICHO(unit)) {\
          soc_error_t res = soc_jer_pp_##func params ;\
          SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res,222,exit,333);\
      } else {\
          SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit); \
      }\
  } while (0)

#ifdef LINK_PB_PP_LIBRARIES
#define SOC_PB_PP_MACRO(macro,params, res) res = SOC_SAND_OK;\
    SOC_PB_PP_##macro params
#define SOC_PB_PP_FUNC(func,params) SOC_PB_PP_##func params
#else
#define SOC_PB_PP_MACRO(macro,params, res) res = SOC_PPD_INVALID_DEVICE_TYPE_ERR
#define SOC_PB_PP_FUNC(func,params) SOC_PPD_INVALID_DEVICE_TYPE_ERR
#endif

#ifdef LINK_ARAD_LIBRARIES
#define ARAD_MACRO(macro,params, res) res = SOC_SAND_OK;\
    ARAD_PP_##macro params
#define ARAD_FUNC(func,params) ARAD_PP_##func params
#else
#define ARAD_MACRO(macro,params, res) res = SOC_PPD_INVALID_DEVICE_TYPE_ERR
#define ARAD_FUNC(func,params) SOC_PPD_INVALID_DEVICE_TYPE_ERR
#endif


#ifdef LINK_PB_PP_LIBRARIES
#define SOC_PB_PP_MACRO_FUNC(func,params) SOC_PB_PP_##func params
#else
#define SOC_PB_PP_MACRO_FUNC(func,params) -1
#endif

#ifdef LINK_ARAD_LIBRARIES
#define ARAD_MACRO_FUNC(func,params) ARAD_PP_##func params
#else
#define ARAD_MACRO_FUNC(func,params) -1
#endif


#define SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, macro, params, res)    \
  switch (SOC_SAND_DEVICE_TYPE_GET(unit))        \
  {                                                   \
    case SOC_SAND_DEV_PB:                             \
      SOC_PB_PP_MACRO(macro, params, res);            \
      break;                                          \
  case SOC_SAND_DEV_ARAD:                             \
      ARAD_MACRO(macro, params, res);                 \
	  break;                                          \
    default:                                          \
      res =  SOC_PPD_INVALID_DEVICE_TYPE_ERR;         \
  }



#define SOC_PPD_ARAD_TMP_DEVICE_FUNC_CALL(unit, func, params, res)    \
  switch (SOC_SAND_DEVICE_TYPE_GET(unit))        \
  {                                                   \
    case SOC_SAND_DEV_PB:                             \
      res = SOC_PB_PP_FUNC(func, params);             \
      break;                                          \
  case SOC_SAND_DEV_ARAD:                             \
      res = ARAD_FUNC(func, params);                  \
	  break;                                          \
    default:                                          \
      res =  SOC_PPD_INVALID_DEVICE_TYPE_ERR;         \
  }


/* Used when macro returns a value */
#define SOC_PPD_ARAD_TMP_DEVICE_MACRO_VAL_CALL(unit, func, params)    \
	(SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_PB)  ? SOC_PB_PP_MACRO_FUNC(func, params) : \
	((SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_ARAD) ? ARAD_MACRO_FUNC(func, params) : -1 )

#define SOC_PPD_DEVICE_CALL(func,params)              \
  switch (SOC_SAND_DEVICE_TYPE_GET(unit))        \
  {                                               \
    case SOC_SAND_DEV_PB:                             \
      SOC_PB_PP_DEVICE_CALL(func, params);            \
      break;                                      \
    default:                                      \
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit); \
      break; \
  }

#ifdef LINK_PCP_LIBRARIES
#define SOC_PPD_FRWRD_DEVICE_CALL(func,params)              \
	switch (SOC_SAND_DEVICE_TYPE_GET(unit))        \
	{                                               \
	case SOC_SAND_DEV_PB:                             \
	  SOC_PB_PP_DEVICE_CALL(func, params);            \
	  break;                                      \
	case SOC_SAND_DEV_PCP:                             \
	  SOC_PCP_DEVICE_CALL(func, params);            \
	  break;                                      \
	default:                                      \
	  SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit); \
	  break; \
	}
#else
#define SOC_PPD_FRWRD_DEVICE_CALL(func,params) SOC_PPD_DEVICE_CALL(func,params)
#endif


#ifdef LINK_PCP_LIBRARIES
#define SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(func,params)              \
	switch (SOC_SAND_DEVICE_TYPE_GET(unit))        \
	{                                               \
	case SOC_SAND_DEV_PB:                             \
	  SOC_PB_PP_DEVICE_CALL(func, params);            \
	  break;                                      \
	case SOC_SAND_DEV_PCP:                             \
	  SOC_PCP_DEVICE_CALL(func, params);            \
	  break;                                      \
    case SOC_SAND_DEV_ARAD:                             \
      ARAD_PP_DEVICE_CALL(func, params);            \
      break;                                      \
	default:                                      \
	  SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit); \
	  break; \
	}
#else
#define SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(func,params) SOC_PPD_ARAD_TMP_DEVICE_CALL(func,params)
#endif

#if (SOC_PPD_DEBUG >= SOC_SAND_DBG_LVL3)
#define SOC_PPD_FUNC_PRINT(func,params)               \
    func params
#else
#define SOC_PPD_FUNC_PRINT(func,params)
#endif


/************************************************************************/
/* forwarding decision Macros                                           */
/************************************************************************/

/* $Id: ppd_api_general.h,v 1.23 Broadcom SDK $
 *  Destination is Drop. Set 'fwd_decision' to drop
 *  destination. Packet forwarded according to this
 *  'fwd_decision' is dropped.
 */
#define SOC_PPD_FRWRD_DECISION_DROP_SET(unit, fwd_decision, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_DROP_SET, (unit, fwd_decision), res);
/*
 *  Destination is the local CPU. Set 'fwd_decision' to local
 *  CPU (i.e. local port 0). Packet forwarded according to
 *  this 'fwd_decision' is forwarded to CPU (not trapped,
 *  i.e., with no trap-code attached to it)
 */
#define SOC_PPD_FRWRD_DECISION_LOCAL_CPU_SET(unit, fwd_decision, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_LOCAL_CPU_SET, (unit, fwd_decision), res);
/*
 *  Destination is a physical system port. Set the
 *  'fwd_decision' to include the destination physical
 *  system port (0 to 4K-1).
 */
#define SOC_PPD_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, fwd_decision, phy_port, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PHY_SYS_PORT_SET, (unit, fwd_decision, phy_port), res);
/*
 *  Destination is a LAG. Set the 'fwd_decision' to include
 *  the LAG ID.
 */
#define SOC_PPD_FRWRD_DECISION_LAG_SET(unit, fwd_decision, lag_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_LAG_SET, (unit, fwd_decision, lag_id), res);
/*
 *  Destination is a multicast group. Set the 'fwd_decision'
 *  to include MC-group ID
 */
#define SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, fwd_decision, mc_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_MC_GROUP_SET, (unit, fwd_decision, mc_id), res);
/*
 *  Destination is a FEC-entry. Set the 'fwd_decision' to
 *  include a pointer to the FEC table
 */
#define SOC_PPD_FRWRD_DECISION_FEC_SET(unit, fwd_decision, fec_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_FEC_SET, (unit, fwd_decision, fec_id), res);
/*
 *  Destination with COS (i.e., explicit TM flow). Set the
 *  'fwd_decision' to include the explicit TM flow_id
 */
#define SOC_PPD_FRWRD_DECISION_EXPL_FLOW_SET(unit, fwd_decision, flow_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_EXPL_FLOW_SET, (unit, fwd_decision, flow_id), res);
/*
 *  Trap packet. Set the 'fwd_decision' to Trap the packet
 *  using the following attributes: - trap_code : 0-255;
 *  identifies the trap/snoop actions to be applied if the
 *  assigned strength is higher than the previously assigned
 *  strength.- fwd_strength: 0-7- snp_strenght: 0-3
 */
#define SOC_PPD_FRWRD_DECISION_TRAP_SET(unit, fwd_decision, _trap_code, frwrd_strength, snp_strength, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_TRAP_SET, (unit, fwd_decision, _trap_code, frwrd_strength,snp_strength), res);
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
#define SOC_PPD_FRWRD_DECISION_AC_SET(unit, fwd_decision, ac_id, is_lag, sys_port_id, res)  \
  SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_AC_SET, (unit, fwd_decision, ac_id, is_lag, sys_port_id), res);
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
#define SOC_PPD_FRWRD_DECISION_AC_WITH_COSQ_SET(unit, fwd_decision, ac_id,flow_id, res)  \
  SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_AC_WITH_COSQ_SET, (unit, fwd_decision, ac_id, flow_id), res);
/*
 *  Forward to access associated with AC-id with
 *  protection. Set the 'fwd_decision' to include AC-id with
 *  FEC-index. Packet forwarded according to this
 *  'fwd_decision' is forwarded according the FEC entry
 *  setting associated with the given (out) AC-id. This
 *  forwarding decision can be learned.
 */
#define SOC_PPD_FRWRD_DECISION_PROTECTED_AC_SET(unit, fwd_decision, ac_id, fec_index, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PROTECTED_AC_SET, (unit, fwd_decision, ac_id, fec_index), res);
/*
 *  VPLS access to core with no protection. Set the
 *  'fwd_decision' to include pwe-id and system-port. Packet
 *  forwarded according to this 'fwd_decision' is forwarded
 *  to sys_port encapsulated according to pwe_id setting.
 *  This forwarding decision can be learned as well.
 */
#define SOC_PPD_FRWRD_DECISION_PWE_SET(unit, fwd_decision, pwe_id, is_lag, sys_port_id, res)  \
  SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PWE_SET, (unit, fwd_decision, pwe_id, is_lag, sys_port_id), res);
/*
 *
 */
#define SOC_PPD_FRWRD_DECISION_TRILL_SET(unit, fwd_decision, nick, is_multi, fec_or_mc_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_TRILL_SET, (unit, fwd_decision, nick, is_multi, fec_or_mc_id), res);
/*
 *  VPLS access to core with no protection using an explicit TM flow ID.
 *  Set the fwd_decision' to include the destination flow-id and the PWE-ID.
 *  Notes 1. Packets forwarded according to this 'fwd_decision' are
 *  forwarded according the given TM flow-id 2. This forwarding decision can be
 *  dynamically learned, by setting it in the In-PWE's
 *  Learn-Record (see SOC_PPD_L2_LIF_PWE_INFO).
 */
#define SOC_PPD_FRWRD_DECISION_PWE_WITH_COSQ_SET(unit, fwd_decision, pwe_id, flow_id, res)  \
  SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PWE_WITH_COSQ_SET, (unit, fwd_decision, pwe_id, flow_id), res);

/*
 *  VPLS access to core with protection on tunnel only. Set
 *  the 'fwd_decision' to include fec-index VC-label. Packet
 *  forwarded according to this 'fwd_decision' is
 *  encapsulated with 'vc_label'. EXP,TTL is set according
 *  to 'push_profile' definition see. This forwarding
 *  decision can be learned as well.
 */
#define SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_SET(unit, fwd_decision, vc_label, push_profile, fec_index, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PWE_PROTECTED_TUNNEL_SET, (unit, fwd_decision, vc_label, push_profile, fec_index), res);

/*
 *  VPLS access to core with protection on tunnel only. Set
 *  the 'fwd_decision' to include fec-index and VPLS outlif. Packet
 *  forwarded according to this 'fwd_decision' is
 *  encapsulated as follows: PWE label is accoring to pwe_outlif entry, 
 *  tunnel labels are according to fec resolution. This forwarding
 *  decision can be learned as well.
 */
#define SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_WITH_OUTLIF_SET(unit, fwd_decision, fec_index, pwe_outlif, res) \
    ARAD_MACRO(FRWRD_DECISION_PWE_PROTECTED_TUNNEL_WITH_OUTLIF_SET, (unit, fwd_decision, fec_index, pwe_outlif), res); 

/*
 *  VPLS access to core with protection on PWE. Set the
 *  'fwd_decision' to include fec-index. Packet forwarded
 *  according to this 'fwd_decision' is forwarded according
 *  to FEC entry setting. By this setting the PWE can be
 *  protected. This forwarding decision can be learned as
 *  well.
 */
#define SOC_PPD_FRWRD_DECISION_PROTECTED_PWE_SET(unit, fwd_decision, fec_index, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_PROTECTED_PWE_SET, (unit, fwd_decision, fec_index), res);
/*
 *  ILM entry. Set the 'fwd_decision' to include swap-label
 *  and fec-index label. For Packets forwarded according to
 *  this 'fwd_decision' MPLS label is swappedAnd forwarded
 *  according to FEC entry setting
 */
#define SOC_PPD_FRWRD_DECISION_ILM_SWAP_SET(unit, fwd_decision, swap_label, fec_index, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_ILM_SWAP_SET, (unit, fwd_decision, swap_label, fec_index), res);
/*
 *  ILM Push entry. Set the 'fwd_decision' to include label
 *  and fec-index label. For Packets forwarded according to
 *  this 'fwd_decision' MPLS label is pushed And forwarded
 *  according to FEC entry setting
 */
#define SOC_PPD_FRWRD_DECISION_ILM_PUSH_SET(unit, fwd_decision,label, push_profile, fec_index, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_ILM_PUSH_SET, (unit, fwd_decision,label, push_profile, fec_index), res);
/*
 *  Mac in mac entry. Destination is a FEC-entry. Set the 'fwd_decision' to
 *  include a pointer to the FEC table. EEI is the isid_id.
 */
#define SOC_PPD_FRWRD_DECISION_MAC_IN_MAC_SET(unit, fwd_decision, isid_id, fec_id, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_CALL(unit, FRWRD_DECISION_MAC_IN_MAC_SET, (unit, fwd_decision, isid_id, fec_id), res);

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

/************************************************************************/
/* CUD macros                                                                     */
/************************************************************************/

/*
 * Returns CUD that includes EEP with value 'eep_ndx'
 */
#define SOC_PPD_CUD_EEP_GET(unit, eep_ndx)   \
    SOC_PPD_ARAD_TMP_DEVICE_MACRO_VAL_CALL(unit, CUD_EEP_GET, (unit, eep_ndx));

/*
 * Returns CUD that includes VSI with value 'vsi_ndx'
 */

#define SOC_PPD_CUD_VSI_GET(unit, vsi_ndx)   \
    SOC_PPD_ARAD_TMP_DEVICE_MACRO_VAL_CALL(unit, CUD_VSI_GET, (unit, vsi_ndx));
/*
 * Returns CUD that includes AC with value 'ac_ndx'
 */
#define SOC_PPD_CUD_AC_GET(unit, ac_ndx)   \
	SOC_PPD_ARAD_TMP_DEVICE_MACRO_VAL_CALL(unit, CUD_AC_GET, (unit, ac_ndx));


#define SOC_PPD_DEST_TRAP_VAL_SET(__dest_val, __trap_code,__fwd_strenght, __snoop_strength)   \
            __dest_val = (((__snoop_strength)<<19) | (__fwd_strenght)<<16)|((__trap_code))

#define SOC_PPD_DEST_TRAP_VAL_GET_TRAP_CODE(__dest_val)   \
            ((__dest_val) & 0xFFFF)

#define SOC_PPD_DEST_TRAP_VAL_GET_FWD_STRENGTH(__dest_val)   \
            (((__dest_val) >> 16) & 0x7)

#define SOC_PPD_DEST_TRAP_VAL_GET_SNP_STRENGTH(__dest_val)   \
            (((__dest_val) >> 19) & 0x3)

/* 
 * "identifier" encodes the action type of the mpls command.
 * identifier 0-7: push action. In this case identifier is also the push profile.
 * identifier 8: pop action.
 * identifier 9: swap action.
 */
#define SOC_PPD_EEI_ENCODING_MPLS_COMMAND(identifier,mpls_label)           ((identifier << SOC_PPD_NOF_BITS_MPLS_LABEL) | mpls_label)
#define SOC_PPD_MPLS_LABEL_FROM_EEI_COMMAND_ENCODING(eei)                  (eei & 0xfffff)
#define SOC_PPD_MPLS_IDENTIFIER_FROM_EEI_COMMAND_ENCODING(eei)           (eei >> SOC_PPD_NOF_BITS_MPLS_LABEL)

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
  SOC_PPD_GENERAL_GET_PROCS_PTR = SOC_PPD_PROC_DESC_BASE_GENERAL_FIRST,
  /*
   * } Auto generated. Do not edit previous section.
   */

   SOC_PPD_FWD_DECISION_TO_SAND_DEST,
   SOC_PPD_SAND_DEST_TO_FWD_DECISION,



  /*
   * Last element. Do no touch.
   */
  SOC_PPD_GENERAL_PROCEDURE_DESC_LAST
} SOC_PPD_GENERAL_PROCEDURE_DESC;

/*
 *  Local tm port. Range : 0 - 63.
 */
typedef SOC_PPC_TM_PORT                                        SOC_PPD_TM_PORT;


/*
 *  Local PP port. Range : 0 - 63.
 */
typedef SOC_PPC_PORT                                           SOC_PPD_PORT;


/*
 *  Filtering ID. Range: Soc_petraB: 0 - 16K-1. T20E: 0-64K-1.
 */
typedef SOC_PPC_FID                                            SOC_PPD_FID;


/*
 *  Virtual switch instance ID. Range: 0 - 16K-1.
 */
typedef SOC_PPC_VSI_ID                                         SOC_PPD_VSI_ID;


/*
 *  System VSI. Range: 0 - 64K-1.
 */
typedef SOC_PPC_SYS_VSI_ID                                     SOC_PPD_SYS_VSI_ID;


/*
 *  Forwarding Equivalence Class ID. Range: 0 - 16K-1.
 */
typedef SOC_PPC_FEC_ID                                         SOC_PPD_FEC_ID;


/*
 *  Virtual Router ID. Range: 1 - 255.
 */
typedef SOC_PPC_VRF_ID                                         SOC_PPD_VRF_ID;


/*
 *  Attachment Circuit ID. Range: Soc_petraB: 0 - 16K-1. T20E: 0
 *  - 64K-1.
 */
typedef SOC_PPC_AC_ID                                          SOC_PPD_AC_ID;


/*
 *  Router Interface ID. Range: 0 - 4K-1.
 */
typedef SOC_PPC_RIF_ID                                         SOC_PPD_RIF_ID;


/*
 *  Logical Interface ID. Range: Soc_petraB: 0 - 16K-1. T20E: 0
 *  - 64K-1.
 */
typedef SOC_PPC_LIF_ID                                         SOC_PPD_LIF_ID;


/*
 *  MP Level.
 */
typedef SOC_PPC_MP_LEVEL                                       SOC_PPD_MP_LEVEL;


#define SOC_PPD_EEI_TYPE_EMPTY                                SOC_PPC_EEI_TYPE_EMPTY
#define SOC_PPD_EEI_TYPE_TRILL                                SOC_PPC_EEI_TYPE_TRILL
#define SOC_PPD_EEI_TYPE_MPLS                                 SOC_PPC_EEI_TYPE_MPLS
#define SOC_PPD_EEI_TYPE_MIM                                  SOC_PPC_EEI_TYPE_MIM
#define SOC_PPD_EEI_TYPE_OUTLIF                               SOC_PPC_EEI_TYPE_OUTLIF
#define SOC_PPD_EEI_TYPE_RAW                                  SOC_PPC_EEI_TYPE_RAW
typedef SOC_PPC_EEI_TYPE                                      SOC_PPD_EEI_TYPE;

#define SOC_PPD_OUTLIF_ENCODE_TYPE_NONE                       SOC_PPC_OUTLIF_ENCODE_TYPE_NONE
#define SOC_PPD_OUTLIF_ENCODE_TYPE_RAW                        SOC_PPC_OUTLIF_ENCODE_TYPE_RAW
#define SOC_PPD_OUTLIF_ENCODE_TYPE_RAW_INVALID                SOC_PPC_OUTLIF_ENCODE_TYPE_RAW_INVALID
#define SOC_PPD_OUTLIF_ENCODE_TYPE_AC                         SOC_PPC_OUTLIF_ENCODE_TYPE_AC
#define SOC_PPD_OUTLIF_ENCODE_TYPE_EEP                        SOC_PPC_OUTLIF_ENCODE_TYPE_EEP
#define SOC_PPD_OUTLIF_ENCODE_TYPE_VSI                        SOC_PPC_OUTLIF_ENCODE_TYPE_VSI
typedef SOC_PPC_OUTLIF_ENCODE_TYPE                            SOC_PPD_OUTLIF_ENCODE_TYPE;

#define SOC_PPD_FRWRD_DECISION_TYPE_DROP                      SOC_PPC_FRWRD_DECISION_TYPE_DROP
#define SOC_PPD_FRWRD_DECISION_TYPE_UC_FLOW                   SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW
#define SOC_PPD_FRWRD_DECISION_TYPE_UC_LAG                    SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG
#define SOC_PPD_FRWRD_DECISION_TYPE_UC_PORT                   SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT
#define SOC_PPD_FRWRD_DECISION_TYPE_MC                        SOC_PPC_FRWRD_DECISION_TYPE_MC
#define SOC_PPD_FRWRD_DECISION_TYPE_FEC                       SOC_PPC_FRWRD_DECISION_TYPE_FEC
#define SOC_PPD_FRWRD_DECISION_TYPE_TRAP                      SOC_PPC_FRWRD_DECISION_TYPE_TRAP
#define SOC_PPD_NOF_FRWRD_DECISION_TYPES                      SOC_PPC_NOF_FRWRD_DECISION_TYPES
typedef SOC_PPC_FRWRD_DECISION_TYPE                           SOC_PPD_FRWRD_DECISION_TYPE;

#define SOC_PPD_MPLS_COMMAND_TYPE_PUSH                        SOC_PPC_MPLS_COMMAND_TYPE_PUSH
#define SOC_PPD_MPLS_COMMAND_TYPE_POP                         SOC_PPC_MPLS_COMMAND_TYPE_POP
#define SOC_PPD_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE          SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE
#define SOC_PPD_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM       SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM
#define SOC_PPD_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE          SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE
#define SOC_PPD_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM       SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM
#define SOC_PPD_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE          SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE
#define SOC_PPD_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM       SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM
#define SOC_PPD_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET           SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET
#define SOC_PPD_MPLS_COMMAND_TYPE_SWAP                        SOC_PPC_MPLS_COMMAND_TYPE_SWAP
typedef SOC_PPC_MPLS_COMMAND_TYPE                             SOC_PPD_MPLS_COMMAND_TYPE;

#define SOC_PPD_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC                 SOC_PPC_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC
#define SOC_PPD_L2_NEXT_PRTCL_TYPE_TRILL                      SOC_PPC_L2_NEXT_PRTCL_TYPE_TRILL
#define SOC_PPD_L2_NEXT_PRTCL_TYPE_IPV4                       SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV4
#define SOC_PPD_L2_NEXT_PRTCL_TYPE_IPV6                       SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV6
#define SOC_PPD_L2_NEXT_PRTCL_TYPE_ARP                        SOC_PPC_L2_NEXT_PRTCL_TYPE_ARP
#define SOC_PPD_L2_NEXT_PRTCL_TYPE_CFM                        SOC_PPC_L2_NEXT_PRTCL_TYPE_CFM
#define SOC_PPD_L2_NEXT_PRTCL_TYPE_MPLS                       SOC_PPC_L2_NEXT_PRTCL_TYPE_MPLS
#define SOC_PPD_L2_NEXT_PRTCL_TYPE_FC_ETH                     SOC_PPC_L2_NEXT_PRTCL_TYPE_FC_ETH
#define SOC_PPD_L2_NEXT_PRTCL_TYPE_OTHER                      SOC_PPC_L2_NEXT_PRTCL_TYPE_OTHER
typedef SOC_PPC_L2_NEXT_PRTCL_TYPE                            SOC_PPD_L2_NEXT_PRTCL_TYPE;

#define SOC_PPD_L3_NEXT_PRTCL_TYPE_NONE                        SOC_PPC_L3_NEXT_PRTCL_TYPE_NONE
#define SOC_PPD_L3_NEXT_PRTCL_TYPE_TCP                         SOC_PPC_L3_NEXT_PRTCL_TYPE_TCP
#define SOC_PPD_L3_NEXT_PRTCL_TYPE_UDP                         SOC_PPC_L3_NEXT_PRTCL_TYPE_UDP
#define SOC_PPD_L3_NEXT_PRTCL_TYPE_IGMP                        SOC_PPC_L3_NEXT_PRTCL_TYPE_IGMP
#define SOC_PPD_L3_NEXT_PRTCL_TYPE_ICMP                        SOC_PPC_L3_NEXT_PRTCL_TYPE_ICMP
#define SOC_PPD_L3_NEXT_PRTCL_TYPE_ICMPV6                      SOC_PPC_L3_NEXT_PRTCL_TYPE_ICMPV6
#define SOC_PPD_L3_NEXT_PRTCL_TYPE_IPV4                        SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV4
#define SOC_PPD_L3_NEXT_PRTCL_TYPE_IPV6                        SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV6
#define SOC_PPD_L3_NEXT_PRTCL_TYPE_MPLS                        SOC_PPC_L3_NEXT_PRTCL_TYPE_MPLS
typedef SOC_PPC_L3_NEXT_PRTCL_TYPE                             SOC_PPD_L3_NEXT_PRTCL_TYPE;

#define SOC_PPD_PKT_TERM_TYPE_NONE                             SOC_PPC_PKT_TERM_TYPE_NONE
#define SOC_PPD_PKT_TERM_TYPE_ETH                              SOC_PPC_PKT_TERM_TYPE_ETH
#define SOC_PPD_PKT_TERM_TYPE_IPV4_ETH                         SOC_PPC_PKT_TERM_TYPE_IPV4_ETH
#define SOC_PPD_PKT_TERM_TYPE_IPV6_ETH                         SOC_PPC_PKT_TERM_TYPE_IPV6_ETH
#define SOC_PPD_PKT_TERM_TYPE_MPLS_ETH                         SOC_PPC_PKT_TERM_TYPE_MPLS_ETH
#define SOC_PPD_PKT_TERM_TYPE_CW_MPLS_ETH                      SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH
#define SOC_PPD_PKT_TERM_TYPE_MPLS2_ETH                        SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH
#define SOC_PPD_PKT_TERM_TYPE_CW_MPLS2_ETH                     SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH
#define SOC_PPD_PKT_TERM_TYPE_MPLS3_ETH                        SOC_PPC_PKT_TERM_TYPE_MPLS3_ETH
#define SOC_PPD_PKT_TERM_TYPE_CW_MPLS3_ETH                     SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH
#define SOC_PPD_PKT_TERM_TYPE_TRILL                            SOC_PPC_PKT_TERM_TYPE_TRILL
#define SOC_PPD_NOF_PKT_TERM_TYPES                             SOC_PPC_NOF_PKT_TERM_TYPES
typedef SOC_PPC_PKT_TERM_TYPE                                  SOC_PPD_PKT_TERM_TYPE;

#define SOC_PPD_PKT_FRWRD_TYPE_BRIDGE                          SOC_PPC_PKT_FRWRD_TYPE_BRIDGE
#define SOC_PPD_PKT_FRWRD_TYPE_IPV4_UC                         SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC
#define SOC_PPD_PKT_FRWRD_TYPE_IPV4_MC                         SOC_PPC_PKT_FRWRD_TYPE_IPV4_MC
#define SOC_PPD_PKT_FRWRD_TYPE_IPV6_UC                         SOC_PPC_PKT_FRWRD_TYPE_IPV6_UC
#define SOC_PPD_PKT_FRWRD_TYPE_IPV6_MC                         SOC_PPC_PKT_FRWRD_TYPE_IPV6_MC
#define SOC_PPD_PKT_FRWRD_TYPE_MPLS                            SOC_PPC_PKT_FRWRD_TYPE_MPLS
#define SOC_PPD_PKT_FRWRD_TYPE_TRILL                           SOC_PPC_PKT_FRWRD_TYPE_TRILL
#define SOC_PPD_PKT_FRWRD_TYPE_CPU_TRAP                        SOC_PPC_PKT_FRWRD_TYPE_CPU_TRAP
#define SOC_PPD_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM               SOC_PPC_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM
#define SOC_PPD_PKT_FRWRD_TYPE_TM                              SOC_PPC_PKT_FRWRD_TYPE_TM
typedef SOC_PPC_PKT_FRWRD_TYPE                                 SOC_PPD_PKT_FRWRD_TYPE;

#define SOC_PPD_PKT_HDR_TYPE_ETH                               SOC_PPC_PKT_HDR_TYPE_ETH
#define SOC_PPD_PKT_HDR_TYPE_IPV4                              SOC_PPC_PKT_HDR_TYPE_IPV4
#define SOC_PPD_PKT_HDR_TYPE_IPV6                              SOC_PPC_PKT_HDR_TYPE_IPV6
#define SOC_PPD_PKT_HDR_TYPE_MPLS                              SOC_PPC_PKT_HDR_TYPE_MPLS
#define SOC_PPD_PKT_HDR_TYPE_TRILL                             SOC_PPC_PKT_HDR_TYPE_TRILL
typedef SOC_PPC_PKT_HDR_TYPE                                   SOC_PPD_PKT_HDR_TYPE;

#define SOC_PPD_PKT_HDR_STK_TYPE_ETH                           SOC_PPC_PKT_HDR_STK_TYPE_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_ETH_ETH                       SOC_PPC_PKT_HDR_STK_TYPE_ETH_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_IPV4_ETH                      SOC_PPC_PKT_HDR_STK_TYPE_IPV4_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_IPV6_ETH                      SOC_PPC_PKT_HDR_STK_TYPE_IPV6_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_MPLS1_ETH                     SOC_PPC_PKT_HDR_STK_TYPE_MPLS1_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_MPLS2_ETH                     SOC_PPC_PKT_HDR_STK_TYPE_MPLS2_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_MPLS3_ETH                     SOC_PPC_PKT_HDR_STK_TYPE_MPLS3_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_ETH_TRILL_ETH                 SOC_PPC_PKT_HDR_STK_TYPE_ETH_TRILL_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH                 SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH                 SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH                 SOC_PPC_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH                 SOC_PPC_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH                SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH                SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH                SOC_PPC_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH                 SOC_PPC_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH                SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH                SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH
#define SOC_PPD_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH                SOC_PPC_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH
typedef SOC_PPC_PKT_HDR_STK_TYPE                               SOC_PPD_PKT_HDR_STK_TYPE;

#define SOC_PPD_IP_ROUTING_TABLE_ITER_TYPE_FAST                SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_FAST
#define SOC_PPD_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED   SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED
#define SOC_PPD_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED   SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED
typedef SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE                     SOC_PPD_IP_ROUTING_TABLE_ITER_TYPE;

#define SOC_PPD_HASH_MASKS_MAC_SA                              SOC_PPC_HASH_MASKS_MAC_SA
#define SOC_PPD_HASH_MASKS_MAC_DA                              SOC_PPC_HASH_MASKS_MAC_DA
#define SOC_PPD_HASH_MASKS_VSI                                 SOC_PPC_HASH_MASKS_VSI
#define SOC_PPD_HASH_MASKS_ETH_TYPE_CODE                       SOC_PPC_HASH_MASKS_ETH_TYPE_CODE
#define SOC_PPD_HASH_MASKS_MPLS_LABEL_1                        SOC_PPC_HASH_MASKS_MPLS_LABEL_1
#define SOC_PPD_HASH_MASKS_MPLS_LABEL_2                        SOC_PPC_HASH_MASKS_MPLS_LABEL_2
#define SOC_PPD_HASH_MASKS_MPLS_LABEL_3                        SOC_PPC_HASH_MASKS_MPLS_LABEL_3
#define SOC_PPD_HASH_MASKS_MPLS_LABEL_4                        SOC_PPC_HASH_MASKS_MPLS_LABEL_4
#define SOC_PPD_HASH_MASKS_MPLS_LABEL_5                        SOC_PPC_HASH_MASKS_MPLS_LABEL_5
#define SOC_PPD_HASH_MASKS_IPV4_SIP                            SOC_PPC_HASH_MASKS_IPV4_SIP
#define SOC_PPD_HASH_MASKS_IPV4_DIP                            SOC_PPC_HASH_MASKS_IPV4_DIP
#define SOC_PPD_HASH_MASKS_IPV4_PROTOCOL                       SOC_PPC_HASH_MASKS_IPV4_PROTOCOL
#define SOC_PPD_HASH_MASKS_IPV6_SIP                            SOC_PPC_HASH_MASKS_IPV6_SIP
#define SOC_PPD_HASH_MASKS_IPV6_DIP                            SOC_PPC_HASH_MASKS_IPV6_DIP
#define SOC_PPD_HASH_MASKS_IPV6_PROTOCOL                       SOC_PPC_HASH_MASKS_IPV6_PROTOCOL
#define SOC_PPD_HASH_MASKS_L4_SRC_PORT                         SOC_PPC_HASH_MASKS_L4_SRC_PORT
#define SOC_PPD_HASH_MASKS_L4_DEST_PORT                        SOC_PPC_HASH_MASKS_L4_DEST_PORT
#define SOC_PPD_HASH_MASKS_FC_DEST_ID                          SOC_PPC_HASH_MASKS_FC_DEST_ID
#define SOC_PPD_HASH_MASKS_FC_SRC_ID                           SOC_PPC_HASH_MASKS_FC_SRC_ID
#define SOC_PPD_HASH_MASKS_FC_SEQ_ID                           SOC_PPC_HASH_MASKS_FC_SEQ_ID
#define SOC_PPD_HASH_MASKS_FC_ORG_EX_ID                        SOC_PPC_HASH_MASKS_FC_ORG_EX_ID
#define SOC_PPD_HASH_MASKS_FC_RES_EX_ID                        SOC_PPC_HASH_MASKS_FC_RES_EX_ID
#define SOC_PPD_HASH_MASKS_TRILL_EG_NICK                       SOC_PPC_HASH_MASKS_TRILL_EG_NICK
#define SOC_PPD_HASH_MASKS_FC_VFI                              SOC_PPC_HASH_MASKS_FC_VFI
typedef SOC_PPC_HASH_MASKS                                     SOC_PPD_HASH_MASKS;

typedef SOC_PPC_ACTION_PROFILE                                 SOC_PPD_ACTION_PROFILE;
typedef SOC_PPC_TRAP_INFO                                      SOC_PPD_TRAP_INFO;
typedef SOC_PPC_OUTLIF                                         SOC_PPD_OUTLIF;
typedef SOC_PPC_MPLS_COMMAND                                   SOC_PPD_MPLS_COMMAND;
typedef SOC_PPC_EEI_VAL                                        SOC_PPD_EEI_VAL;
typedef SOC_PPC_EEI                                            SOC_PPD_EEI;
typedef SOC_PPC_FRWRD_DECISION_TYPE_INFO                       SOC_PPD_FRWRD_DECISION_TYPE_INFO;
typedef SOC_PPC_FRWRD_DECISION_INFO                            SOC_PPD_FRWRD_DECISION_INFO;
typedef SOC_PPC_TPID_PROFILE                                   SOC_PPD_TPID_PROFILE;
typedef SOC_PPC_PEP_KEY                                        SOC_PPD_PEP_KEY;
typedef SOC_PPC_IP_ROUTING_TABLE_ITER                          SOC_PPD_IP_ROUTING_TABLE_ITER;
typedef SOC_PPC_IP_ROUTING_TABLE_RANGE                         SOC_PPD_IP_ROUTING_TABLE_RANGE;
typedef SOC_PPC_ADDITIONAL_TPID_VALUES                         SOC_PPD_ADDITIONAL_TPID_VALUES;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
uint32
  soc_ppd_fwd_decision_to_sand_dest(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN SOC_PPD_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID *dest_id
  );

uint32
  soc_ppd_sand_dest_to_fwd_decision(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *dest_id,
    SOC_SAND_OUT SOC_PPD_FRWRD_DECISION_INFO *fwd_decision
  );

/*********************************************************************
* NAME:
 *   soc_ppd_l2_next_prtcl_type_allocate_test
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
  soc_ppd_l2_next_prtcl_type_allocate_test(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               l2_next_prtcl_type,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE            *success
  );

/* { */
/*********************************************************************
* NAME:
 *   soc_ppd_general_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_ppd_api_general module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_general_get_procs_ptr(void);

void
  SOC_PPD_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_TRAP_INFO *info
  );

void
  SOC_PPD_OUTLIF_clear(
    SOC_SAND_OUT SOC_PPD_OUTLIF *info
  );

void
  SOC_PPD_MPLS_COMMAND_clear(
    SOC_SAND_OUT SOC_PPD_MPLS_COMMAND *info
  );

void
  SOC_PPD_EEI_VAL_clear(
    SOC_SAND_OUT SOC_PPD_EEI_VAL *info
  );

void
  SOC_PPD_EEI_clear(
    SOC_SAND_OUT SOC_PPD_EEI *info
  );

void
  SOC_PPD_FRWRD_DECISION_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_DECISION_TYPE_INFO *info
  );

void
  SOC_PPD_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_DECISION_INFO *info
  );

void
  SOC_PPD_ACTION_PROFILE_clear(
    SOC_SAND_OUT SOC_PPD_ACTION_PROFILE *info
  );

void
  SOC_PPD_TPID_PROFILE_clear(
    SOC_SAND_OUT SOC_PPD_TPID_PROFILE *info
  );

void
  SOC_PPD_PEP_KEY_clear(
    SOC_SAND_OUT SOC_PPD_PEP_KEY *info
  );

void
  SOC_PPD_IP_ROUTING_TABLE_ITER_clear(
    SOC_SAND_OUT SOC_PPD_IP_ROUTING_TABLE_ITER *info
  );

void
  SOC_PPD_IP_ROUTING_TABLE_RANGE_clear(
    SOC_SAND_OUT SOC_PPD_IP_ROUTING_TABLE_RANGE *info
  );

void
  SOC_PPD_ADDITIONAL_TPID_VALUES_clear(
    SOC_SAND_OUT SOC_PPD_ADDITIONAL_TPID_VALUES *info
  );

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_EEI_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_EEI_TYPE enum_val
  );

const char*
  SOC_PPD_OUTLIF_ENCODE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_OUTLIF_ENCODE_TYPE enum_val
  );

const char*
  SOC_PPD_FRWRD_DECISION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_DECISION_TYPE enum_val
  );

const char*
  SOC_PPD_MPLS_COMMAND_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_MPLS_COMMAND_TYPE enum_val
  );

const char*
  SOC_PPD_L2_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_L2_NEXT_PRTCL_TYPE enum_val
  );

const char*
  SOC_PPD_L3_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_L3_NEXT_PRTCL_TYPE enum_val
  );

const char*
  SOC_PPD_PKT_TERM_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_PKT_TERM_TYPE enum_val
  );

const char*
  SOC_PPD_PKT_FRWRD_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_PKT_FRWRD_TYPE enum_val
  );

const char*
  SOC_PPD_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_PKT_HDR_TYPE enum_val
  );

const char*
  SOC_PPD_PKT_HDR_STK_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_PKT_HDR_STK_TYPE enum_val
  );

const char*
  SOC_PPD_IP_ROUTING_TABLE_ITER_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_IP_ROUTING_TABLE_ITER_TYPE enum_val
  );

const char*
  SOC_PPD_HASH_MASKS_to_string(
    SOC_SAND_IN  SOC_PPD_HASH_MASKS enum_val
  );

void
  SOC_PPD_TRAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_TRAP_INFO *info
  );

void
  SOC_PPD_OUTLIF_print(
    SOC_SAND_IN  SOC_PPD_OUTLIF *info
  );

void
  SOC_PPD_MPLS_COMMAND_print(
    SOC_SAND_IN  SOC_PPD_MPLS_COMMAND *info
  );

void
  SOC_PPD_EEI_VAL_print(
    SOC_SAND_IN  SOC_PPD_EEI_VAL *info
  );

void
  SOC_PPD_EEI_print(
    SOC_SAND_IN  SOC_PPD_EEI *info
  );

void
  SOC_PPD_FRWRD_DECISION_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_DECISION_TYPE      frwrd_type,
    SOC_SAND_IN  SOC_PPD_FRWRD_DECISION_TYPE_INFO *info
  );

void
  SOC_PPD_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_DECISION_INFO *info
  );

void
  SOC_PPD_ACTION_PROFILE_print(
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE *info
  );

void
  SOC_PPD_TPID_PROFILE_print(
    SOC_SAND_IN  SOC_PPD_TPID_PROFILE *info
  );

void
  SOC_PPD_PEP_KEY_print(
    SOC_SAND_IN  SOC_PPD_PEP_KEY *info
  );

void
  SOC_PPD_IP_ROUTING_TABLE_ITER_print(
    SOC_SAND_IN  SOC_PPD_IP_ROUTING_TABLE_ITER *info
  );

void
  SOC_PPD_IP_ROUTING_TABLE_RANGE_print(
    SOC_SAND_IN  SOC_PPD_IP_ROUTING_TABLE_RANGE *info
  );

void
  SOC_PPD_ADDITIONAL_TPID_VALUES_print(
    SOC_SAND_IN  SOC_PPD_ADDITIONAL_TPID_VALUES *info
  );

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_GENERAL_INCLUDED__*/
#endif
