/*
 * $Id: oam.c,v 1.148 Broadcom SDK $
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
 *
 * File:    oam.c
 * Purpose: Manages L3 interface table, forwarding table, routing table
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_OAM
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/common/sat.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/field_int.h>

#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/oam_sw_db.h>
#include <bcm_int/dpp/oam_hw_db.h>
#include <bcm_int/dpp/oam_resource.h>
#include <bcm_int/dpp/oam_dissect.h>
#include <bcm_int/dpp/bfd.h>

#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_eg_mirror.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/dpp_wb_engine.h>
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/dpp/JER/jer_intr.h>
#endif /*BCM_ARAD_SUPPORT*/

#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/oam.h>

#include <shared/gport.h>

#include <soc/drv.h>
#include <soc/enet.h>
#include <soc/defs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>



#include <soc/dpp/ARAD/arad_ports.h>
/***************************************************************/
/***************************************************************/
/*
 * Local defines
 * {
 */
#define OAM_HASHS_ACCESS  OAM_ACCESS.oam_hashs

#define ENDP_LST_ACCESS          OAM_ACCESS.endp_lst
#define ENDP_LST_ACCESS_DATA     OAM_ACCESS.endp_lst.endp_lsts_array
#define ENDP_LST_M_ACCESS        OAM_ACCESS.endp_lst_m
#define ENDP_LST_M_ACCESS_DATA   OAM_ACCESS.endp_lst_m.endp_lst_ms_array

#define _JER_PP_OAM_TC_NOF_BITS 3

#define _JER_PP_OAM_DP_NOF_BITS 2


/* } */
/***************************************************************/
/***************************************************************/

_bcm_dpp_oam_bfd_sw_db_lock_t _bcm_dpp_oam_bfd_rmep_info_db_lock[BCM_MAX_NUM_UNITS];
_bcm_dpp_oam_bfd_sw_db_lock_t _bcm_dpp_oam_bfd_mep_info_db_lock[BCM_MAX_NUM_UNITS];

/* Callbacks are not supported by warmboot. Need to re-register after warmboot */
/* OAM */
static bcm_oam_event_cb _g_oam_event_cb[BCM_MAX_NUM_UNITS][bcmOAMEventCount];
/* OAM LM/DM statistics reports handlers */
static bcm_oam_performance_event_cb _g_oam_performance_event_cb[BCM_MAX_NUM_UNITS][bcmOAMEventCount];
/* User data */
static void*            _g_oam_event_ud[BCM_MAX_NUM_UNITS][bcmOAMEventCount];
/* BFD */
static bcm_bfd_event_cb _g_bfd_event_cb[BCM_MAX_NUM_UNITS][bcmBFDEventCount];
/* User data */
static void*            _g_bfd_event_ud[BCM_MAX_NUM_UNITS][bcmBFDEventCount];





/* global variables used in bcm_petra_oam_(endpoint|group)_traverse( ) */
static bcm_oam_group_traverse_cb_w_param_t _group_cb_with_param;

/***************************************************************/
/***************************************************************/

/*
 * MACROS
 * 
 *
 */

#define _BCM_OAM_PPD_EVENT_PORT_BLOCKED(rmep_state) \
  (((rmep_state >> 3) & 3) == 1)

#define _BCM_OAM_PPD_EVENT_PORT_UP(rmep_state) \
  (((rmep_state >> 3) & 3) == 2)

#define _BCM_OAM_PPD_EVENT_INTERFACE_UP(rmep_state) \
  ((rmep_state & 7) == 1)

#define _BCM_OAM_PPD_EVENT_INTERFACE_DOWN(rmep_state) \
  ((rmep_state & 7) == 2)

#define _BCM_OAM_PPD_EVENT_INTERFACE_TESTING(rmep_state) \
  ((rmep_state & 7) == 3)

#define _BCM_OAM_PPD_EVENT_INTERFACE_UNKNOWN(rmep_state) \
  ((rmep_state & 7) == 4)

#define _BCM_OAM_PPD_EVENT_INTERFACE_DORMANT(rmep_state) \
  ((rmep_state & 7) == 5)

#define _BCM_OAM_PPD_EVENT_INTERFACE_NOT_PRESENT(rmep_state) \
  ((rmep_state & 7) == 6)

#define _BCM_OAM_PPD_EVENT_INTERFACE_LL_DOWN(rmep_state) \
  ((rmep_state & 7) == 7)



/* RMEP state event is handles separately */
#define _BCM_OAM_PPD_TO_BCM_EVENT_MEP(ppd_event, event_type, is_oam) \
  do {   \
	 switch (ppd_event) {   \
		case SOC_PPD_OAM_EVENT_LOC_SET:   \
			event_type = is_oam ? bcmOAMEventEndpointCCMTimeout : bcmBFDEventEndpointTimeout;   \
			break;   \
		case SOC_PPD_OAM_EVENT_ALMOST_LOC_SET:   \
			event_type = is_oam ? bcmOAMEventEndpointCCMTimeoutEarly : bcmBFDEventEndpointTimeoutEarly;   \
			break;   \
		case SOC_PPD_OAM_EVENT_LOC_CLR:   \
			event_type = is_oam ? bcmOAMEventEndpointCCMTimein : bcmBFDEventEndpointTimein;   \
			break;   \
		case SOC_PPD_OAM_EVENT_RDI_SET:   \
			event_type = is_oam ? bcmOAMEventEndpointRemote : bcmBFDEventEndpointRemote;   \
			break;   \
		case SOC_PPD_OAM_EVENT_RDI_CLR:   \
			event_type = is_oam ? bcmOAMEventEndpointRemoteUp : bcmBFDEventEndpointRemoteUp;  \
			break;   \
		case SOC_PPD_OAM_EVENT_RMEP_STATE_CHANGE:   \
			event_type = is_oam ? 0 : bcmBFDEventStateChange;  \
			break;   \
        case SOC_PPD_OAM_EVENT_REPORT_RX_LM: \
			event_type = is_oam ? bcmOAMEventEndpointLmStatistics : -1;  \
			break;   \
         case SOC_PPD_OAM_EVENT_REPORT_RX_DM: \
             event_type = is_oam ? bcmOAMEventEndpointDmStatistics : -1;  \
             break;   \
		default:   \
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Event ID is not supported")));   \
	}   \
  } while (0) 

#define _BCM_OAM_PPD_FROM_BCM_EVENT_MEP(soc_ppd_event_type, event_type, is_oam) \
  do {   \
     switch (event_type) {   \
            case bcmOAMEventEndpointCCMTimeout: soc_ppd_event_type = SOC_PPD_OAM_EVENT_LOC_SET; break;   \
	/*    case bcmOAMEventEndpointCCMTimeoutEarly: soc_ppd_event_type = SOC_PPD_OAM_EVENT_ALMOST_LOC_SET; break; Unsupported due to HW bug */ \
	    case bcmOAMEventEndpointCCMTimein: soc_ppd_event_type = SOC_PPD_OAM_EVENT_LOC_CLR; break;   \
	    case bcmOAMEventEndpointRemote: soc_ppd_event_type = SOC_PPD_OAM_EVENT_RDI_SET; break;   \
            case bcmOAMEventEndpointRemoteUp: soc_ppd_event_type = SOC_PPD_OAM_EVENT_RDI_CLR; break;   \
            case bcmOAMEventEndpointPortDown:   \
            case bcmOAMEventEndpointPortUp:   \
            case bcmOAMEventEndpointInterfaceDown:   \
            case bcmOAMEventEndpointInterfaceUp:   \
            case bcmOAMEventEndpointInterfaceTesting:   \
            case bcmOAMEventEndpointInterfaceUnkonwn:   \
            case bcmOAMEventEndpointInterfaceDormant:   \
            case bcmOAMEventEndpointInterfaceNotPresent:   \
            case bcmOAMEventEndpointInterfaceLLDown:   soc_ppd_event_type = SOC_PPD_OAM_EVENT_RMEP_STATE_CHANGE; break;   \
	    default:   \
		BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Event ID is not supported: %d"), event_i));   \
     }  \
  } while (0)

#define _BCM_BFD_PPD_FROM_BCM_EVENT_MEP(ppd_event, event_type, is_oam) \
  do {   \
     switch (event_type) {   \
        case bcmBFDEventEndpointTimeout: soc_ppd_event_type = SOC_PPD_OAM_EVENT_LOC_SET; break;   \
	case bcmBFDEventEndpointTimein: soc_ppd_event_type = SOC_PPD_OAM_EVENT_LOC_CLR; break;   \
        case bcmBFDEventEndpointTimeoutEarly: soc_ppd_event_type = SOC_PPD_OAM_EVENT_ALMOST_LOC_SET; break;   \
        case bcmBFDEventEndpointRemote: soc_ppd_event_type = SOC_PPD_OAM_EVENT_RDI_SET; break;   \
	case bcmBFDEventEndpointRemoteUp: soc_ppd_event_type = SOC_PPD_OAM_EVENT_RDI_CLR; break;   \
        case bcmBFDEventStateChange:   \
        case bcmBFDEventRemoteStateDiag:   \
        case bcmBFDEventEndpointFlagsChange: soc_ppd_event_type = SOC_PPD_OAM_EVENT_RMEP_STATE_CHANGE; break;   \
	default:   \
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Event ID is not supported: %d"), event_i));   \
     }  \
  } while (0) 



/* When upmep oam id is to be added to the FHEI, opcode is defined to include meta-data*/
/* Currently all opcodes include meta-data. This macro remains nonetheless */
#define _BCM_OAM_IS_OPCODE_WITH_META_DATA(opcode)           1                            



#define _BCM_OAM_MIRROR_PROFILE_DEALLOC(mirror_profile_dest)                                                   \
  do {																										   \
  int dont_care;\
	if (_bcm_dpp_am_template_mirror_action_profile_free(unit, mirror_profile_dest, &dont_care) != BCM_E_NONE) {                                    \
		BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Error: Failed to deallocate mirroring destination\n")));       \
	}																										   \
  } while (0)

#define _BCM_OAM_PROFILE_SINGLE_OPCODE_SET(profile_data, opcode, action, trap_code)	\
  do {																				\
    uint32 trap_id_err_level;														\
    uint32 tmp_bitmap[1];/* to avoid coverity defect */                             \
	if (BCM_OAM_ACTION_GET(*action, bcmOAMActionCountEnable)) {						\
        *tmp_bitmap = profile_data.counter_disable;                                 \
		SHR_BITCLR(tmp_bitmap, opcode);					    	                    \
        profile_data.counter_disable = *tmp_bitmap;                                 \
	}  														               		    \
	else {													               		    \
        *tmp_bitmap = profile_data.counter_disable;                                 \
        SHR_BITSET(tmp_bitmap, opcode);					    	                    \
        profile_data.counter_disable = *tmp_bitmap;                                 \
	}  														               		    \
	if (BCM_OAM_ACTION_GET(*action, bcmOAMActionMeterEnable)) {					 	\
        *tmp_bitmap = profile_data.meter_disable;                                   \
        SHR_BITCLR(tmp_bitmap, opcode);					    	                    \
        profile_data.meter_disable = *tmp_bitmap;                                   \
	}  														               		    \
	else {													               		    \
        *tmp_bitmap = profile_data.meter_disable;                                   \
        SHR_BITSET(tmp_bitmap, opcode);					    	                    \
        profile_data.meter_disable = *tmp_bitmap;                                   \
	}  														               		    \
	if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwd)) {			            	\
		profile_data.opcode_to_trap_code_unicast_map[opcode] = trap_code;	        \
	}  														               		    \
	if (BCM_OAM_ACTION_GET(*action, bcmOAMActionMcFwd)) {           				\
		profile_data.opcode_to_trap_code_multicast_map[opcode] = trap_code;	        \
	}  														               		    \
	if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcDrop)) {           				\
       rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_ERR_LEVEL, &trap_id_err_level);\
	    BCMDNX_IF_ERR_EXIT(rv);								               		    \
		profile_data.opcode_to_trap_code_unicast_map[opcode] = trap_id_err_level;	\
	}  														               		    \
	if (BCM_OAM_ACTION_GET(*action, bcmOAMActionMcDrop)) {           				\
       rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_ERR_LEVEL, &trap_id_err_level);\
	    BCMDNX_IF_ERR_EXIT(rv);								               		    \
		profile_data.opcode_to_trap_code_multicast_map[opcode] = trap_id_err_level;	\
	}  														               		    \
	if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwdAsData)) {		            	\
		profile_data.opcode_to_trap_code_unicast_map[opcode] = _BCM_PETRA_UD_DFLT_TRAP;\
	}  														               		    \
	if (BCM_OAM_ACTION_GET(*action, bcmOAMActionMcFwdAsData)) {        				\
		profile_data.opcode_to_trap_code_multicast_map[opcode] = _BCM_PETRA_UD_DFLT_TRAP;\
	}  														               		    \
  } while (0)

/* Find if there are meps on same lif & direction*/
#define _BCM_OAM_MEP_EXIST_ON_LIF_AND_DIRECTION(lif, is_upmep, find_mip, found, is_mip, profile)	                    \
  do {  														               		                                    \
	  uint8 __mdl, __found_profile, __is_mp_type_flexible_dummy;                  		                                \
	  for (__mdl=0; __mdl<=7; __mdl++) {			    		           		                                        \
			soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(unit,                          \
																						 lif,                           \
																						 __mdl,                         \
																						 is_upmep,                      \
																						 &found,                        \
                                                                                         &profile,                      \
																						 &__found_profile,              \
																						 &__is_mp_type_flexible_dummy,  \
																						 &is_mip);                      \
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);			               		                                            \
			if (!__found_profile) {			               		                                                        \
				break;			               		                                                                    \
            }			               		                                                                            \
			if (found && (!find_mip || is_mip)) {			               		                                        \
				break;									               		                                            \
			}  														               		                                \
	  }  														               		                                    \
  } while (0)

#define OAM_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW 1024

/**
 * Insert an event to the event hash table. 
 * If specific event has already been inserted, update the data 
 * to signify multiple events 
 */
#define  INSERT_EVENT_INTO_HTBL(htbl,eventt ) \
    do {\
        rv = shr_htb_insert(htbl,  (shr_htb_key_t)&eventt, (shr_htb_data_t) 0);\
        if (rv == _SHR_E_EXISTS) {\
            int count;\
            int count_enable;\
            rv = shr_htb_find(htbl, (shr_htb_key_t)&eventt,(shr_htb_data_t) &count, 1 /* remove */ );\
            BCMDNX_IF_ERR_EXIT(rv);\
            count_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_multiple_event_count_enable", 0);\
            if(count_enable)\
                count = (count >> 16) + 1;\
            else \
                count = 0;\
            rv = shr_htb_insert(htbl, (shr_htb_key_t)&eventt, INT_TO_PTR((count << 16) | BCM_OAM_EVENT_FLAGS_MULTIPLE));\
        }\
        BCMDNX_IF_ERR_EXIT(rv);\
} while (0)



#define GET_MSB_FROM_MAC_ADDR(mac_addr) \
	mac_addr[2] | ( mac_addr[1] << 8) | (mac_addr[0] << 16)

#define GET_LSB_FROM_MAC_ADDR(mac_addr) \
	mac_addr[5] | ( mac_addr[4] << 8) | (mac_addr[3] << 16)

/* Macro expects the LSBs and MSBs in uint32 form*/
#define SET_MAC_ADDRESS_BY_MSB_AND_LSB(mac_addr, lsb,msb)\
               mac_addr[5] = lsb & 0xff;\
               mac_addr[4] = (lsb & 0xff00 )>>8;\
               mac_addr[3] = (lsb & 0xff0000 )>>16;\
               mac_addr[2] = msb & 0xff;\
               mac_addr[1] = (msb & 0xff00 )>>8;\
               mac_addr[0] = (msb & 0xff0000 )>>16;

/* Macro used by diagnostics functions */
#define MAC_PRINT_FMT "%02x:%02x:%02x:%02x:%02x:%02x\n"
#define MAC_PRINT_ARG(mac_addr) mac_addr[0], mac_addr[1], \
        mac_addr[2], mac_addr[3],                         \
        mac_addr[4], mac_addr[5]

#define MULTIPLE_BY_3_5(_whole, _thousandth) /* multiple by 3.5 */ \
{                                                                  \
    uint32 _res_whole;                                             \
    uint32 _res_thousandth;                                        \
    _res_whole = ((_whole) * 35) / 10;                             \
    _res_thousandth = ((_thousandth) * 35) / 10;                   \
    if((((_whole) * 35) % 10) > 0) {                               \
        _res_thousandth += ((_whole) * 35) % 10;                   \
    }                                                              \
    if(_res_thousandth > 1000) {                                   \
        _res_whole += _res_thousandth / 1000;                      \
        _res_thousandth = _res_thousandth % 1000;                  \
    }                                                              \
    (_whole) = _res_whole;                                         \
    (_thousandth) = _res_thousandth;                               \
}
                     
/*
* Macro sends LM/DM packets to the destination in the action. (unicast forward). 
* when is_lm is set, the last parameter acts as is_piggy_backed, otherwise as is_1dm.                                                                             .
* All other properties in the classifier remain uncahnged.
*/
#define SET_CLASSIFIER_ACTION( mep_id, action, is_lm, is_1dm_or_piggy_back, mep_type ) \
    do {\
    int is_piggy_backed = is_1dm_or_piggy_back;\
    SOC_PPD_OAM_LIF_PROFILE_DATA  lif_profile;\
    uint32 meter_disable[1], counter_disable[1];\
    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, mep_id, &lif_profile);\
    BCMDNX_IF_ERR_EXIT(rv);\
    BCM_OAM_ACTION_CLEAR_ALL(action);\
    BCM_OAM_OPCODE_CLEAR_ALL(action);\
	action.flags = action.rx_trap = 0;\
    if (mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {\
		BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd); \
	}else{\
		BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd); \
	}\
    *counter_disable = lif_profile.mep_profile_data.counter_disable;\
    *meter_disable = lif_profile.mep_profile_data.meter_disable;\
    if (!is_lm) {\
        if (is_1dm_or_piggy_back) {\
            BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_1DM);\
        }\
		if (SOC_IS_ARADPLUS_A0(unit)) { /*Reply packets handled seperatly in Jericho*/\
                BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM);\
		}\
        BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMR);\
        if (!SHR_BITGET(counter_disable, SOC_PPD_OAM_OPCODE_MAP_DMM)) {\
            BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable); \
        }\
        if (!SHR_BITGET(meter_disable, SOC_PPD_OAM_OPCODE_MAP_DMM)) {\
        BCM_OAM_ACTION_SET(action, bcmOAMActionMeterEnable); \
        }\
        is_piggy_backed =0;\
    } else {\
	    int meter_counter_opcode = is_piggy_backed? SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM : SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM;\
        if (is_1dm_or_piggy_back) { \
            BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM);\
        } \
        else { \
            BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR);\
			if (SOC_IS_ARADPLUS_A0(unit)) {/*Reply packets handled seperatly in Jericho*/\
	                BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM);\
	        } \
		}\
        if (!SHR_BITGET(meter_disable, meter_counter_opcode)) {\
            BCM_OAM_ACTION_SET(action, bcmOAMActionMeterEnable); \
        }\
    }\
    rv = bcm_petra_oam_endpoint_action_set_internal(unit,mep_id, &action,is_piggy_backed);\
    BCMDNX_IF_ERR_EXIT(rv);\
} while (0); 

/*
*  Sets an action. Like the previous macro. Opcode in action should be set beforehand.
*  opcode_map should be on of the SOC_PPD_OAM_OPCODE_MAP_XXX enum (for meter enable/ counter enable).
*/
#define SET_CLASSIFIER_ACTION_GENERAL(mep_id, action, opcode_map, mep_type)\
do { \
    SOC_PPD_OAM_LIF_PROFILE_DATA  lif_profile;\
    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, mep_id, &lif_profile);\
	BCM_OAM_ACTION_CLEAR_ALL(action);\
	action.flags = action.rx_trap = 0;\
   if (mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {\
		BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd); \
	}else{\
		BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd); \
	}\
   if (!SHR_BITGET(&lif_profile.mep_profile_data.counter_disable, opcode_map)) {\
		BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable); \
	}\
	if (!SHR_BITGET(&lif_profile.mep_profile_data.meter_disable, opcode_map)) {\
		BCM_OAM_ACTION_SET(action, bcmOAMActionMeterEnable); \
	}\
   rv = bcm_petra_oam_endpoint_action_set_internal(unit,mep_id, &action,0); \
    BCMDNX_IF_ERR_EXIT(rv);\
} while (0); 

/**
 * Macro allocates profiles from the NIC/OUI tables and updates 
 * the mep_db_entry accordingly. 
 */


#define CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last, eth1731_profile_index, endpoint_id)\
    if (is_last) {\
        SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile_data;\
        SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile_data);\
        soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_id, 0, eth1731_profile_index, &eth1731_profile_data);\
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv); \
    }

#define FREE_NIC_AND_OUI_PROFILES(endpoint_id) \
do {\
	int nic_profile, oui_profile, is_last1, is_last2;\
		rv = _bcm_dpp_am_template_oam_lmm_nic_tables_free(unit, endpoint_id, &nic_profile, &is_last1);\
		BCMDNX_IF_ERR_EXIT(rv);\
		rv = _bcm_dpp_am_template_oam_lmm_oui_tables_free(unit, endpoint_id, &oui_profile, &is_last2);\
		BCMDNX_IF_ERR_EXIT(rv);\
        if (is_last1 && is_last2) {\
               soc_sand_rv = soc_ppd_oam_oamp_set_oui_nic_registers(unit,endpoint_id,0,0,oui_profile,nic_profile);\
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);\
        }\
} while(0)


#define UPDATE_NIC_AND_OUI_REFERENCE_COUNTER( _msb, _lsb, _nic_profile, _oui_profile)\
	do {\
		int dont_care;\
		rv = _bcm_dpp_am_template_oam_lmm_nic_tables_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &_lsb, &dont_care, &_nic_profile);\
		BCMDNX_IF_ERR_EXIT(rv);\
		rv = _bcm_dpp_am_template_oam_lmm_oui_tables_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &_msb, &dont_care, &_oui_profile);\
		BCMDNX_IF_ERR_EXIT(rv);\
	} while (0)


#define _BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(ptr, mep_id)\
	do {\
	uint32 lsb, msb;\
	int dont_care;\
		rv = _bcm_dpp_am_template_oam_lmm_nic_tables_get(unit, mep_id,  &dont_care, &lsb);\
		BCMDNX_IF_ERR_EXIT(rv); \
		rv = _bcm_dpp_am_template_oam_lmm_oui_tables_get(unit, mep_id,  &dont_care, &msb);\
		BCMDNX_IF_ERR_EXIT(rv); \
		SET_MAC_ADDRESS_BY_MSB_AND_LSB(ptr->peer_da_mac_address, lsb, msb);\
	} while (0)
			
			
			
#define _BCM_OAM_SET_RDI_ON_ETH1731_PROF_RDI_FIELD(rdi, rx_disable, scanner_disable) \
               rdi = ( (rx_disable)==0) <<1 | ((scanner_disable)==0)

#define _BCM_OAM_GET_RDI_STATUS_FROM_ETH1731_MEP_PROF_RDI_FIELD(rdi_field, is_rdi_from_rx, is_rdi_from_scanner) \
               do {\
                       is_rdi_from_rx = (rdi_field & 0x2)==0x2;\
                       is_rdi_from_scanner = (rdi_field & 0x1);\
               } while (0)


/** 
 * Offsets are calcualted in the following way: 
 * ETH: DA, SA addresss (12B) + num-tags (taken from mep db) * 
 * 4B, 
 * offset in OAM PDU (4B). 
 * For piggy backed CCMs an extra 54 bytes are added 
 * MPLS: GAL, GACH, MPLS/PWE (4B each), offset in OAM PDU
 * (4B). 
 * PWE: GACH, PWE label, offset in OAM PDU (4 byte each)
 * In case of piggy backed CCMs the offset increases by 54  
 * (sequence #) + 2 (MEP-ID) + 48 (MEG-ID)
 *  */ 
#define _OAM_SET_LMM_OFFSET(mep_db_entry, value, is_piggy_back, is_slm) \
    do {value = (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) ? mep_db_entry.tags_num *4 + 0x12 : \
     (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP)? 0x10 : 0x0c /*PWE offset */;\
	if (is_piggy_back) {value += 54 ;} \
    else if (is_slm) {value += 4 ;} }while (0)

/* For SLR the stamping is done 4 bytes after the SMM stamping, for LMR 8.*/
#define _OAM_SET_LMR_OFFSET(mep_db_entry, value,is_piggy_back, is_slm) \
      do {_OAM_SET_LMM_OFFSET(mep_db_entry, value, is_piggy_back,is_slm) ; value += 4 + !is_slm * 4; }while (0)
    

#define _OAM_SET_DMM_OFFSET(mep_db_entry, value) \
    _OAM_SET_LMM_OFFSET(mep_db_entry, value, 0,0)  

#define _OAM_SET_DMR_OFFSET(mep_db_entry, value) \
    do { _OAM_SET_DMM_OFFSET(mep_db_entry, value)  ; value +=0x10; } while (0)


/* Rate 1 means TX every other scan. Rate 2 means one every 6 scans. Higher rate acts as one per 24 scans.
   Zero gets you zero.*/
  /* Jericho only.*/
#define _OAM_RATE_TO_TX_PER_24_SCANS(rate) (((rate<=2)? (20 -8*rate) : 1) * (rate>0)) 

/* Condition for failure in Arad+ : one rate is at 1 and another rate is postive. */
/* Condition for failure in jericho:
   [ (number of packets to be sent with interval 1) * 3 +  (number of packets to be sent with interval 2) + (number of packets to be sent with interval >2) / 60  ] > 6
  In Jericho calculate the phase as well.
   Assumes Opcode_N_ use low transmission rates.
    The pahse is calculated in the following manner:
    (if CCM rate = 1, as an example):
    CCM --> phase 0
    highest of LM/DM --> phase 1
    Lowerst of LM/DM --> phase 3
    opcode_0  -->  331  (sufficiently high prime)
    opcode_1 -->  499   (sufficiently high prime)
   */
#define _OAM_OAMP_VERIFY_MAXIMAL_TX_PER_MEP(ccm_interval, existing_eth1731_profile, new_rate, new_eth1731_mep_profile, is_dm) \
    do {\
    if (SOC_IS_JERICHO(unit)) {\
               int tx_per_24_scans;\
               int old_rate = is_dm?existing_eth1731_profile.lmm_rate:existing_eth1731_profile.dmm_rate;\
               tx_per_24_scans = _OAM_RATE_TO_TX_PER_24_SCANS(ccm_interval);\
               tx_per_24_scans +=  _OAM_RATE_TO_TX_PER_24_SCANS(old_rate);\
               tx_per_24_scans +=  _OAM_RATE_TO_TX_PER_24_SCANS((new_rate));\
               tx_per_24_scans += _OAM_RATE_TO_TX_PER_24_SCANS(existing_eth1731_profile.opcode_0_rate);\
               tx_per_24_scans += _OAM_RATE_TO_TX_PER_24_SCANS(existing_eth1731_profile.opcode_1_rate);\
               if (tx_per_24_scans >24) {\
                   BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE , (_BSL_BCM_MSG("Unsupported number of packets requested to be transmitted from a single MEP.")));\
               }\
               (new_eth1731_mep_profile).ccm_cnt = 0;/* HW constraint: must always be 0.*/\
               /* Now set the phase*/\
				if (ccm_interval >=2) {\
				/* this case is easy*/\
					if (existing_eth1731_profile.dmm_rate == 1 || (new_eth1731_mep_profile).dmm_rate==1) {\
					    (new_eth1731_mep_profile).dmm_cnt =  1 ;\
						(new_eth1731_mep_profile).lmm_cnt = 3;\
					} else {\
				     (new_eth1731_mep_profile).dmm_cnt =  3;\
					 (new_eth1731_mep_profile).lmm_cnt = 1;\
					}\
				} else {\
                   if ((old_rate = 0 && is_dm) || ((new_rate) >= old_rate && is_dm && old_rate) || (old_rate > (new_rate) && !is_dm && old_rate)) { \
						  (new_eth1731_mep_profile).dmm_cnt = 1;\
				   }\
				   if ((old_rate=0 && !is_dm) || ((new_rate)>=old_rate && !is_dm && old_rate) || (old_rate>(new_rate) && is_dm && old_rate )) {\
					   (new_eth1731_mep_profile).lmm_cnt = 1;\
				   }\
				   if ((old_rate && is_dm && (new_rate)>=old_rate) || (old_rate && !is_dm && (new_rate)<old_rate) ) {\
					   (new_eth1731_mep_profile).lmm_cnt = 3;\
				   }\
				   if ((old_rate && !is_dm && (new_rate)>=old_rate) || (old_rate && is_dm && (new_rate)<old_rate) ) {\
					   (new_eth1731_mep_profile).dmm_cnt = 3;\
				   }\
			   }\
                (new_eth1731_mep_profile).op_1_cnt = 5; /*fifth slot modulo 6 is always available. */\
                (new_eth1731_mep_profile).op_0_cnt = 11;\
    } else {\
               if ((ccm_interval == 1 && (new_rate)) || ((new_rate) == 1 && ccm_interval)) {\
                     BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE , (_BSL_BCM_MSG("Unsupported number of packets requested to be transmitted from a single MEP.")));\
               }\
    }\
}while (0)

               

#define _BCM_OAM_SET_CCM_GROUP_DA_MAC_ADDRESS(_bcm_mac_addr, level) \
do {\
     bcm_mac_t ccm_group_da_mac_addr_msbs = {0x1,0x80,0xc2, 0x00, 0x00, 0x30 };\
	sal_memcpy(_bcm_mac_addr,ccm_group_da_mac_addr_msbs, 6 );\
	_bcm_mac_addr[5] |= level;\
} while(0)



#define _BCM_OAM_GET_OAMP_PENDINGEVENT_BY_DEVICE(unit) SOC_IS_JERICHO(unit)? JER_INT_OAMP_PENDING_EVENT : ARAD_INT_OAMP_PENDINGEVENT




/***************************************************************/
/***************************************************************/
/*
 * OAM Module Helper functions
 * 
 *
 */



int _bcm_dpp_oam_group_name_to_ma_name_struct_convert(int unit, uint8 *group_name, _bcm_oam_ma_name_t *ma_name_struct) {

    uint32 i;

    BCMDNX_INIT_FUNC_DEFS;

    switch (group_name[0]) { /* MD Format */
    case 0x01: /* ICC/Short */
        switch (group_name[1]) {
        case 3: /*BCM_OAM_MA_NAME_FORMAT_SHORT*/
            if (group_name[2] != 2) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Error in group name. Unsupported length: %d. Short name format supports only length 2.\n"), group_name[2]));
            }
            ma_name_struct->name_type = _BCM_OAM_MA_NAME_TYPE_SHORT;
            break;
        case 0x20: /*BCM_OAM_MA_NAME_FORMAT_ICC_BASED*/
            if (group_name[2] != 13) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Error in group name. Unsupported length: %d. Long name format supports only length 13.\n"), group_name[2]));
            }
            ma_name_struct->name_type = _BCM_OAM_MA_NAME_TYPE_ICC;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: Unsupported group name format: %d.\n"), group_name[0]));
            break;
        }
        for (i=0; i<group_name[2]; i++) {
            ma_name_struct->name[i] = group_name[i+3];
        }
        break;
    case 0x04: /* String based */
        if (soc_property_get(unit, spn_OAM_MAID_11_BYTES_ENABLE, 0) == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: Error in group name. String based 11b MAID is disabled.\n")));
        }
        if (group_name[1] > 5) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: Error in group name. Unsupported length: %d. MD Length supports up to 5.\n"), group_name[1]));
        }
        if (group_name[8] > 6) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: Error in group name. Unsupported length: %d. MA Length supports up to 6.\n"), group_name[8]));
        }
        ma_name_struct->name_type = _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES;
        /* Copy MD */
        for (i=0; i<6; i++) {
            ma_name_struct->name[i] = group_name[i+1];
        }
        /* Copy MA */
        for (i=0; i<7; i++) {
            ma_name_struct->name[i+6] = group_name[i+8];
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                 (_BSL_BCM_MSG("Error: Error in group name. Unsupported name: %d.\n"), group_name[0]));
        break;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_ma_name_struct_to_group_name_convert(int unit, _bcm_oam_ma_name_t *ma_name_struct, uint8 *group_name) {

    uint32 i, next;

    BCMDNX_INIT_FUNC_DEFS;

    if (ma_name_struct->name_type < _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES) {
        /* Long (ICC) or short MAID */

        group_name[0] = 1;
        if (ma_name_struct->name_type == _BCM_OAM_MA_NAME_TYPE_SHORT) {
            group_name[1]=3;
            group_name[2]=2;
        }
        else {
            group_name[1]=0x20;
            group_name[2]=13;
        }

        for (i=0; i<group_name[2]; i++) {
            group_name[i+3] = ma_name_struct->name[i];
        }
        i=i+3;

        while (i<BCM_OAM_GROUP_NAME_LENGTH) {
            group_name[i]=0;
            i++;
        }
    }
    else {
        /* 11 bytes string based MAID
         * Saved as {MD_Len[1B],MD[5B],MA_Len[1B],MA[6B]}
         * Should be extracted to:
         * {MD_Format=0x04,MD_Len,MD[5B],MA_Format=0x02,MA_Len,MA[6B]}
         */

        next = 0;

        /* Write MD Format (4) */
        group_name[next++] = 4;

        /* Copy MD */
        for (i = 0; i < 6; i++) {
            group_name[next++] = ma_name_struct->name[i];
        }

        /* Write MA Format (2) */
        group_name[next++] = 2;

        /* Copy MD */
        for (; i < 13; i++) {
            group_name[next++] = ma_name_struct->name[i];
        }

    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}




/**
 * Function sets the system port that goes on the ITMH for 
 * transmitted  packets from the OAMP. 
 * Shared by BFD and OAM Down MEP. 
 * 
 * @author sinai (29/04/2015)
 * 
 * @param unit 
 * @param gport 
 * @param sys_port 
 * 
 * @return int 
 */
int _bcm_dpp_oam_bfd_tx_gport_to_sys_port(int unit, bcm_gport_t tx_gport, uint32 * sys_port) {
    _bcm_dpp_gport_info_t gport_info;
    uint32 soc_sand_rv;
    int ret;
    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_GPORT_IS_TRUNK(tx_gport)) {
        /* The tx gport is a LAG group. extract the trunk ID and that goes on the ITMH*/

        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_logical_sys_id_build_with_device, (unit, TRUE, BCM_GPORT_TRUNK_GET(tx_gport), 0, 0,sys_port)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    } else {
        ret = _bcm_dpp_gport_to_phy_port(unit, tx_gport, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
        BCMDNX_IF_ERR_EXIT(ret);
        *sys_port = gport_info.sys_port;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Getting the destination from the ITMH in the OAMP, returning 
 * the tx_gport 
 * 
 * @author sinai (30/04/2015)
 * 
 * @param unit 
 * @param tx_gport 
 * @param sys_port 
 * 
 * @return int 
 */
int _bcm_dpp_oam_bfd_sys_port_to_tx_gport(int unit, bcm_gport_t * tx_gport, uint32 sys_port) {
    int ret;
    ARAD_DEST_INFO destination;
    uint8 is_lag_not_phys = 0;
    uint32  lag_id  = 0, lag_member_id, sys_phys_port_id,soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    arad_ARAD_DEST_INFO_clear(&destination);
    
    
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_logical_sys_id_parse, (unit, sys_port & 0xffff,&is_lag_not_phys,&lag_id,&lag_member_id,&sys_phys_port_id)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

    if (is_lag_not_phys) {
        destination.type = ARAD_DEST_TYPE_LAG;
        destination.id = lag_id;
        ret = _bcm_dpp_gport_from_tm_dest_info(unit, tx_gport, &destination);
        BCMDNX_IF_ERR_EXIT(ret); 
    } else {
        BCM_GPORT_SYSTEM_PORT_ID_SET(*tx_gport,sys_port );
    }



exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_oam_lif_table_set(int unit, SOC_PPD_LIF_ID lif_index, uint8 value) {

	uint32 soc_sand_rv; 
    int local_lif;
    int ret;
	SOC_PPD_LIF_ENTRY_INFO lif_entry_info;

	BCMDNX_INIT_FUNC_DEFS;

    if (lif_index == _BCM_OAM_INVALID_LIF) {
        /*Invalid lif. simply exit.*/
        BCM_EXIT;
    }
    /** lif index is a global lif. translate to local lif. Only
     *  ingress here. */
    ret = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, lif_index, &local_lif);
    BCMDNX_IF_ERR_EXIT(ret);

    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, local_lif, &lif_entry_info);

    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

	switch (lif_entry_info.type) {
	case SOC_PPD_LIF_ENTRY_TYPE_AC:
	case SOC_PPD_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
	case SOC_PPD_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
		lif_entry_info.value.ac.oam_valid = value;
		break;
	case SOC_PPD_LIF_ENTRY_TYPE_PWE:
		lif_entry_info.value.pwe.oam_valid = value;
		break;
	case SOC_PPD_LIF_ENTRY_TYPE_ISID:
		lif_entry_info.value.isid.oam_valid = value;
		break;
	case SOC_PPD_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
		lif_entry_info.value.ip_term_info.oam_valid = value;
		break;
	case SOC_PPD_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
		lif_entry_info.value.mpls_term_info.oam_valid = value;
		break;
    case SOC_PPD_LIF_ENTRY_TYPE_EXTENDER:
		lif_entry_info.value.extender.oam_valid = value;
		break;
	default:
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index %d (extracted from gport)."), local_lif));
	}

	soc_sand_rv = soc_ppd_lif_table_entry_update(unit, local_lif, &lif_entry_info);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_set_counter(int unit, int counter_base, uint8 is_pcp, uint32 lif) {
	uint32 soc_sand_rv; 
	uint32 counter_range_min, counter_range_max;
	uint32 rv;

	BCMDNX_INIT_FUNC_DEFS;

	rv = _bcm_dpp_oam_lif_table_set(unit, lif, 1);
	BCMDNX_IF_ERR_EXIT(rv);

	soc_sand_rv = soc_ppd_oam_counter_range_get(unit, &counter_range_min, &counter_range_max);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
	if (is_pcp) {
		if ((counter_range_min!=0) && (counter_range_min < counter_base) && (counter_range_max > counter_base)) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
					 (_BSL_BCM_MSG("Error: lm_counter_base_id %d should be outside the counter-singleton range.\n"), counter_base));
		}
	}
	else {
		if (counter_range_min==0) { /* First endpoint with counter */
			counter_range_min = counter_base;
			counter_range_max = counter_base;
		}
		else {
			if (counter_base > counter_range_max) {
				counter_range_max = counter_base;
			}
			else if (counter_base < counter_range_min) {
				counter_range_min = counter_base;
			}
		}
		soc_sand_rv = soc_ppd_oam_counter_range_set(unit, counter_range_min, counter_range_max);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Sets the relevant counters of an endpoint. 
 * LIF reperesents the global (in) LIF (passive side for Up MEP). 
 */
int _bcm_dpp_endpoint_counters_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_LIF_ID lif) {

    int rv;
    uint64 stat;
   
    soc_port_t crps_index; 
    uint8 enabled = 0;
    int src_core = 0;
    SOC_TMC_CNT_SRC_TYPE src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP;
    int command_id = 0;
    int command_base_offset = 0;
    SOC_TMC_CNT_MODE_EG_TYPE eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
    SOC_TMC_CNT_FORMAT format = SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES;
    SOC_TMC_CNT_COUNTER_WE_BITMAP we_val = 0;
    SOC_TMC_CNT_CUSTOM_MODE_PARAMS custom_mode_params;
    SOC_TMC_CNT_Q_SET_SIZE q_set_size = 0;
    uint32 stag_lsb = 0;
    int i;

    BCMDNX_INIT_FUNC_DEFS;
    sal_memset(&custom_mode_params, 0x0, sizeof(SOC_TMC_CNT_CUSTOM_MODE_PARAMS));

    if (endpoint_info->lm_counter_base_id > 0) {
        rv = _bcm_dpp_oam_set_counter(unit, endpoint_info->lm_counter_base_id, ((endpoint_info->lm_flags & BCM_OAM_LM_PCP)!=0), lif);
        BCMDNX_IF_ERR_EXIT(rv);

        COMPILER_64_ZERO(stat);
        for (crps_index = 0; crps_index < SOC_DPP_DEFS_GET(unit, nof_counter_processors); crps_index++) {           
            rv = bcm_dpp_counter_config_get(unit, crps_index, &enabled, &src_core, &src_type, &command_id, &command_base_offset, &eg_type, &format, &we_val, &custom_mode_params, &q_set_size, &stag_lsb);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!enabled) {
                continue;
            }
            if (we_val ==  endpoint_info->lm_counter_base_id  / (SOC_DPP_DEFS_GET(unit,counters_per_counter_processor) * ((format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) + 1))) {
                if ((endpoint_info->lm_flags & BCM_OAM_LM_PCP) != 0) {
                    for (i = 0; i < 8; i++) {
                        rv = bcm_dpp_counter_set(unit,
                                                 crps_index,
                                                 endpoint_info->lm_counter_base_id + i,
                                                 bcm_dpp_counter_pkts,
                                                 stat);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                } else {
                    rv = bcm_dpp_counter_set(unit,
                                             crps_index,
                                             endpoint_info->lm_counter_base_id,
                                             bcm_dpp_counter_pkts,
                                             stat);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
	} else if (SOC_IS_JERICHO(unit)) {
		/* Just set the OAM LIF*/
		rv = _bcm_dpp_oam_lif_table_set(unit, lif, 1);
		BCMDNX_IF_ERR_EXIT(rv); 
	}


exit:
    BCMDNX_FUNC_RETURN;
}

/***************************************************************/
/***************************************************************/

/*
 * Event handling
 *
 */

int bcm_petra_oam_event_register(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_event_cb cb, 
    void *user_data) {

    bcm_oam_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmOAMEventCount; event_i++) {
         if (BCM_OAM_EVENT_TYPE_GET(event_types, event_i)) {
             SOC_PPD_OAM_EVENT event_ppd; /* Used to verify the registered event.*/
             if (_g_oam_event_cb[unit][event_i] && (_g_oam_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is already registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("EVENT %d already has a registered callback"), event_i));
             }
             _BCM_OAM_PPD_FROM_BCM_EVENT_MEP(event_ppd, event_i, 1 /* is oam*/);
             (void) event_ppd; /* For compilation*/
             _g_oam_event_cb[unit][event_i] = cb;
             _g_oam_event_ud[unit][event_i] = user_data;
         }
    }
    
	    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Unregister a callback for handling OAM events */
int bcm_petra_oam_event_unregister(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_event_cb cb) {

    bcm_oam_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmOAMEventCount; event_i++) {
         if (BCM_OAM_EVENT_TYPE_GET(event_types, event_i)) {
             if (_g_oam_event_cb[unit][event_i] && (_g_oam_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("A different callback is registered for %d"), event_i));
             }
             _g_oam_event_cb[unit][event_i] = NULL;
             _g_oam_event_ud[unit][event_i] = NULL;

         }
    }
    
	    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_performance_event_register(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_performance_event_cb cb,
    void *user_data)
{
    bcm_oam_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmOAMEventCount; event_i++) {
         if (BCM_OAM_EVENT_TYPE_GET(event_types, event_i)) {
             if (_g_oam_performance_event_cb[unit][event_i] && (_g_oam_performance_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is already registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("EVENT %d already has a registered callback"), event_i));
             }
             _g_oam_performance_event_cb[unit][event_i] = cb;
             _g_oam_event_ud[unit][event_i] = user_data;
         }
    }


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_performance_event_unregister(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_performance_event_cb cb)
{
    bcm_oam_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmOAMEventCount; event_i++) {
         if (BCM_OAM_EVENT_TYPE_GET(event_types, event_i)) {
             if (_g_oam_performance_event_cb[unit][event_i] && (_g_oam_performance_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("A different callback is registered for %d"), event_i));
             }
             _g_oam_performance_event_cb[unit][event_i] = NULL;
             _g_oam_event_ud[unit][event_i] = NULL;

         }
    }


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_petra_bfd_event_register(
    int unit, 
    bcm_bfd_event_types_t event_types, 
    bcm_bfd_event_cb cb, 
    void *user_data) {

    bcm_bfd_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;


    for (event_i = 0; event_i < bcmBFDEventCount; event_i++) {
         if (BCM_BFD_EVENT_TYPE_GET(event_types, event_i)) {
             if (_g_bfd_event_cb[unit][event_i] && (_g_bfd_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is already registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("EVENT %d already has a registered callback"), event_i));
             }
             _g_bfd_event_cb[unit][event_i] = cb;
             _g_bfd_event_ud[unit][event_i] = user_data;
         }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Unregister a callback for handling BFD events */
int _bcm_petra_bfd_event_unregister(
    int unit, 
    bcm_bfd_event_types_t event_types, 
    bcm_bfd_event_cb cb) {

    bcm_bfd_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmBFDEventCount; event_i++) {
         if (BCM_BFD_EVENT_TYPE_GET(event_types, event_i)) {
             if (_g_bfd_event_cb[unit][event_i] && (_g_bfd_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("A different callback is registered for %d"), event_i));
             }
             _g_bfd_event_cb[unit][event_i] = NULL;
             _g_bfd_event_ud[unit][event_i] = NULL;

		 }
	}

    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}




/**
 * Function is intended for usage after inserting all the events
 * in the event hash table. Function reaches information on each 
 * stored event and calls the proper call back with that 
 * information. 
 * 
 * @author sinai (11/11/2013)
 * 
 * @param unit 
 * @param really_the_event_data 
 * @param really_the_flags 
 * 
 * @return int 
 */
int htbl_iteration_callback(int unit, shr_htb_key_t really_the_event_data,shr_htb_data_t really_the_flags ){
    _bcm_oam_event_and_rmep_info *event_info =  (_bcm_oam_event_and_rmep_info*) really_the_event_data;
    int rv, rmeb_db_ndx=0;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    SOC_PPD_OAM_RMEP_INFO_DATA rmep_info;
    bcm_bfd_event_types_t event_types;
    uint8 found;
    int flags = PTR_TO_INT(really_the_flags);
    BCMDNX_INIT_FUNC_DEFS;


    if (event_info->is_oam) {
        if (_g_oam_event_cb[unit][event_info->event_number]) {
            _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(event_info->mep_or_rmep_indx, rmeb_db_ndx);
            rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, rmeb_db_ndx,  &rmep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("This message should never appear. Something wrong with the hash table.")));
            }

            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, rmep_info.mep_index, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("This message should never appear. Something wrong with the hash table (mep db).")));
            }
            /*finally, the callback*/
            _g_oam_event_cb[unit][event_info->event_number](unit, flags, event_info->event_number, mep_info.ma_index, rmeb_db_ndx, _g_oam_event_ud[unit][event_info->event_number]);
        }
        else if (_g_oam_performance_event_cb[unit][event_info->event_number]) {
            bcm_oam_performance_event_data_t report_data;
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, event_info->mep_or_rmep_indx, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("This message should never appear. Something wrong with the hash table (mep db).")));
            }
            /* Fill a statistics report struct for the callback */
            bcm_oam_performance_event_data_t_init(&report_data);
            if (event_info->event_number == bcmOAMEventEndpointLmStatistics) { /* LM */
                report_data.delta_FCB = event_info->event_data[0];
                report_data.delta_FCf = event_info->event_data[1];
            }
            else { /* DM */
                COMPILER_64_SET(report_data.last_delay, event_info->event_data[1], event_info->event_data[0]);
            }
            /*finally, the callback*/
            _g_oam_performance_event_cb[unit][event_info->event_number](unit, event_info->event_number, mep_info.ma_index, event_info->mep_or_rmep_indx,
                                                                        &report_data, _g_oam_event_ud[unit][event_info->event_number]);
        }
    } else { /* bfd*/
        if (_g_bfd_event_cb[unit][event_info->event_number]) {
            _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(event_info->mep_or_rmep_indx, rmeb_db_ndx);
            rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, rmeb_db_ndx,  &rmep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("This message should never appear. Something wrong with the hash table.")));
            }

            BCM_BFD_EVENT_TYPE_CLEAR_ALL(event_types);
            BCM_BFD_EVENT_TYPE_SET(event_types, event_info->event_number);
            _g_bfd_event_cb[unit][event_info->event_number](unit, flags, event_types, rmep_info.mep_index, _g_bfd_event_ud[unit][event_info->event_number]);
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Function handling the interrupt. 
 * Should not be called directly, rather through callback in soc 
 * layer. 
 * 
 * @param unit 
 * @param event_type 
 * @param interrupt_data : "global" data used by soc layer. The 
 *                       reason this is not used as a global
 *                       variable is so that the interrupts will
 *                       be reentrable. The data is only global
 *                       per interrupt.
 * 
 * @return int 
 */
int _bcm_dpp_oam_fifo_interrupt_handler(int unit, JER_PP_OAM_API_DMA_EVENT_TYPE event_type, SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA * interrupt_data) {
	uint32 rmeb_db_ndx = 0; 
    uint32 rmeb_db_ndx_internal, valid, soc_sand_rv;
    uint32 event_id_bitmap[SOC_PPD_OAM_EVENT_COUNT];
    SOC_PPD_OAM_RMEP_INFO_DATA rmep_info;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    uint8 found;
    int rv;
    uint32 event_index;
    bcm_bfd_event_type_t bfd_event_type;
    bcm_oam_event_type_t oam_event_type;
    uint32 rmep_state;
    _bcm_oam_event_and_rmep_info event_found;

    shr_htb_hash_table_t event_htbl = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    
    sal_memset(&event_found, 0, sizeof(_bcm_oam_event_and_rmep_info));

	soc_sand_rv = soc_ppd_oam_event_fifo_read(unit, event_type, &rmeb_db_ndx_internal, event_id_bitmap, &valid, event_found.event_data, interrupt_data);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = shr_htb_create(&event_htbl,
                        OAM_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW,
                        sizeof(_bcm_oam_event_and_rmep_info),
                        "OAM event hash table.");
    BCMDNX_IF_ERR_EXIT(rv); 
    

    while (valid) {
        if (event_type == JER_PP_OAM_API_DMA_EVENT_TYPE_EVENT) {
            _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(rmeb_db_ndx_internal, rmeb_db_ndx);
            event_found.mep_or_rmep_indx = rmeb_db_ndx_internal;
            rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, rmeb_db_ndx,  &rmep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                /* The only reasonable way for an RMEP not to be found is that it was destroyed after the interrupt has occured. In this case there is nothing to do.*/
                LOG_WARN(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit, 
                                     "Warning: Processing event for MEP ID 0x%x failed. MEP not found. Continuing."),rmeb_db_ndx));
                soc_sand_rv = soc_ppd_oam_event_fifo_read(unit, event_type, &rmeb_db_ndx_internal, event_id_bitmap, &valid, event_found.event_data, interrupt_data);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
                continue;
            }
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, rmep_info.mep_index, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG("Internal error in FIFO event processing: MEP index matching RMEP index wasn't found. rmep_db_ndx=%d, mep_db_ndx=%d"),
                                  rmeb_db_ndx, rmep_info.mep_index));
            }
        } else {
            /* Other event types contain the OAM-ID as it arrives to the OAMP on the packet.
               The 14 lsb of this value is the MEP DB index so there's no need to retrieve from the RMEP DB */
            event_found.mep_or_rmep_indx = rmeb_db_ndx_internal & ((1<<14)-1);
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, event_found.mep_or_rmep_indx, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                /* The only reasonable way for an RMEP not to be found is that it was destroyed after the interrupt has occured. In this case there is nothing to do.*/
                LOG_WARN(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit, 
                                     "Warning: Processing event for MEP ID 0x%x failed. MEP not found. Continuing."),rmeb_db_ndx));
                soc_sand_rv = soc_ppd_oam_event_fifo_read(unit, event_type, &rmeb_db_ndx_internal, event_id_bitmap, &valid, event_found.event_data, interrupt_data);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
                continue;
            }
 
        }

        if ((mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) ||
            (mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
            (mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)) {
            event_found.is_oam =1;

            for (event_index = 0; event_index < SOC_PPD_OAM_EVENT_COUNT; event_index++) {
                if (event_id_bitmap[event_index]) {
                    int event_ind=-1;
                    _BCM_OAM_PPD_TO_BCM_EVENT_MEP(event_index, oam_event_type, 1/*is_oam*/);
                    rmep_state = event_found.event_data[0];
                    if (event_index == SOC_PPD_OAM_EVENT_RMEP_STATE_CHANGE) {
                        /* Check port status */
                        if (_BCM_OAM_PPD_EVENT_PORT_BLOCKED(rmep_state)) {
                            event_ind = bcmOAMEventEndpointPortDown;
                        }
                        else if (_BCM_OAM_PPD_EVENT_PORT_UP(rmep_state)) {
                            event_ind = bcmOAMEventEndpointPortUp;
                        }

                        /* If event found, insert to hash table */
                        if (event_ind !=-1) {
                            event_found.event_number = event_ind;
                            INSERT_EVENT_INTO_HTBL(event_htbl,event_found );
                            /* reset "flag" */
                            event_ind =-1;
                        }

                        /* Check interfase status */
                        if (_BCM_OAM_PPD_EVENT_INTERFACE_UP(rmep_state)) {
                            event_ind = bcmOAMEventEndpointInterfaceUp;
                        }
                        else if (_BCM_OAM_PPD_EVENT_INTERFACE_DOWN(rmep_state)) {
                            event_ind = bcmOAMEventEndpointInterfaceDown;
                        }
                        else if (_BCM_OAM_PPD_EVENT_INTERFACE_TESTING(rmep_state)) {
                            event_ind = bcmOAMEventEndpointInterfaceTesting;
                        }
                        else if (_BCM_OAM_PPD_EVENT_INTERFACE_UNKNOWN(rmep_state)) {
                            event_ind = bcmOAMEventEndpointInterfaceUnkonwn;
                        }
                        else if (_BCM_OAM_PPD_EVENT_INTERFACE_DORMANT(rmep_state)) {
                            event_ind = bcmOAMEventEndpointInterfaceDormant;
                        }
                        else if (_BCM_OAM_PPD_EVENT_INTERFACE_NOT_PRESENT(rmep_state)) {
                            event_ind = bcmOAMEventEndpointInterfaceNotPresent;
                        }
                        else if (_BCM_OAM_PPD_EVENT_INTERFACE_LL_DOWN(rmep_state)) {
                            event_ind = bcmOAMEventEndpointInterfaceLLDown;
                        }

                        /* If event found, insert to hash table */
                        if (event_ind !=-1) {
                            event_found.event_number = event_ind;
                            INSERT_EVENT_INTO_HTBL(event_htbl,event_found );
                        }
                    }
                    else {
                        event_found.event_number = oam_event_type;
                        INSERT_EVENT_INTO_HTBL(event_htbl,event_found );
                    }
                }
            }

        } else { /* BFD */
            event_found.is_oam =0;
            for (event_index = 0; event_index < SOC_PPD_OAM_EVENT_COUNT; event_index++) {
                if (event_id_bitmap[event_index]) {
                    _BCM_OAM_PPD_TO_BCM_EVENT_MEP(event_index, bfd_event_type, 0/*is_oam*/);
                    if (bfd_event_type != -1) {
                        event_found.event_number = (uint8)bfd_event_type;
                        INSERT_EVENT_INTO_HTBL(event_htbl,event_found );
                    }
                }
            }
        }

        soc_sand_rv = soc_ppd_oam_event_fifo_read(unit, event_type, &rmeb_db_ndx_internal, event_id_bitmap, &valid, event_found.event_data, interrupt_data);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    

    rv = shr_htb_iterate(unit,event_htbl, htbl_iteration_callback);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = shr_htb_destroy(&event_htbl, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    event_htbl = NULL;

    BCM_EXIT;
exit:
    if(event_htbl) {
        rv = shr_htb_destroy(&event_htbl, NULL);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "shr_htb_destroy failed\n")));
        }
    }
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_oamp_interrupts_init(int unit, uint8 enable_interrupts) {
	bcm_switch_event_control_t event_control;
    int rv,is_stat_event;

    BCMDNX_INIT_FUNC_DEFS;
		if (!SOC_WARM_BOOT(unit))
		{
		for (is_stat_event = 0; is_stat_event <= 1; ++is_stat_event) {
			uint32 flags;
			event_control.event_id =  _BCM_OAM_GET_OAMP_PENDINGEVENT_BY_DEVICE(unit);
			if (is_stat_event) {
				if (SOC_IS_JERICHO(unit)) {
					/* Enable Stat events in Jerichco */
					event_control.event_id = JER_INT_OAMP_STAT_PENDING_EVENT;
				} else {
					/* No such event in Arad*/
					break;
				}
			}

			rv = soc_interrupt_flags_get(unit, event_control.event_id, &flags);
			BCMDNX_IF_ERR_EXIT(rv);

			flags |= SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK;

			rv = soc_interrupt_flags_set(unit, event_control.event_id, flags);
			BCMDNX_IF_ERR_EXIT(rv); 

			event_control.index = 0;
			event_control.action = bcmSwitchEventMask;
			rv = bcm_petra_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_control, !enable_interrupts);
			BCMDNX_IF_ERR_EXIT(rv);

			if (enable_interrupts) {
				/* Disabling prints*/
				event_control.action = bcmSwitchEventLog;
				rv = bcm_petra_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_control, 0);
				BCMDNX_IF_ERR_EXIT(rv);
			}
		}
		}




exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * callback used for bcm_petra_oam_group_traverse()
 * 
*/
STATIC int
_bcm_petra_oam_group_traverse_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t user_data){
    int rv;
    bcm_oam_group_info_t group_info_for_cb;
    uint32 group_info_id = * (uint32*)key;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_oam_group_info_t_init(&group_info_for_cb); 

    /* user_data here is the name of the oam group*/
   rv = _bcm_dpp_oam_ma_name_struct_to_group_name_convert(unit, (_bcm_oam_ma_name_t*) user_data, (&group_info_for_cb)->name );
   BCMDNX_IF_ERR_EXIT(rv);
   group_info_for_cb.id = group_info_id;


   /*now call user callback. Everything but the name and key on the group_info _for_cb is 0*/
   rv = _group_cb_with_param.cb(unit, &group_info_for_cb, _group_cb_with_param.user_data);
   BCMDNX_IF_ERR_EXIT(rv);

	BCM_EXIT;
exit:
	BCMDNX_FUNC_RETURN;

}


/*
 * callbacks used for profile exchange
 * 
*/
STATIC int _bcm_oam_endpoint_exchange_profile_cb(
   int unit, 
   bcm_oam_endpoint_info_t *endpoint_info, 
   void *user_data)
{
	bcm_oam_endpoint_traverse_cb_profile_info_t * profile_info;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
	SOC_PPD_OAM_LIF_PROFILE_DATA profile_data_old;
	int old_profile, is_last, profile, is_allocated;
	uint32 lif;
	uint8 found;
	uint8 is_passive;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
	int rv; 

    BCMDNX_INIT_FUNC_DEFS;
	profile_info = (bcm_oam_endpoint_traverse_cb_profile_info_t*)user_data;

    rv = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info->gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS, &gport_hw_resources);
	BCMDNX_IF_ERR_EXIT(rv);

    lif = gport_hw_resources.global_in_lif;

	if (lif == profile_info->lif) {
		/* Save first endpoint id on this lif */
        if (profile_info->first_endpoint_id == BCM_OAM_ENDPOINT_INVALID) {
			profile_info->first_endpoint_id = endpoint_info->id;
        }
		
        if (profile_info->profile_data) {
            is_passive = (_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) == 0) ^ profile_info->is_ingress;

			if (is_passive && SOC_IS_JERICHO(unit)) {
				/* Passive entries in Jericho did not allocate a non accelerated profile.*/
				BCM_EXIT;   
			}
            
            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, endpoint_info->id, is_passive, &profile_data_old);
            BCMDNX_IF_ERR_EXIT(rv); 

            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_exchange(unit, 0/*flags*/,  endpoint_info->id, is_passive, profile_info->profile_data,
                                     &old_profile, &is_last, &profile, &is_allocated);
            BCMDNX_IF_ERR_EXIT(rv);
			profile_info->is_alloced = is_allocated;
			profile_info->is_last = is_last;

            if (profile != old_profile) {
                /* Replace existing profile in endpoint SW DB */
                rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info->id, &classifier_mep_entry, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (!found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: _bcm_oam_endpoint_exchange_profile_cb internal error.\n")));
                }
                if (is_passive) {
                    classifier_mep_entry.non_acc_profile_passive = profile;
                }
                else {
                    classifier_mep_entry.non_acc_profile = profile;
                }

                rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, endpoint_info->id, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (is_allocated) {
                rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &(profile_info->profile_data->mep_profile_data));
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &(profile_info->profile_data->mip_profile_data));
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (is_last) {
                rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &(profile_data_old.mep_profile_data), &classifier_mep_entry, 1/*is_mep*/);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &(profile_data_old.mip_profile_data), &classifier_mep_entry, 0/*is_mep*/);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            
        }
	}
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int _bcm_oam_endpoint_exchange_profile_group_cb(
   int unit, 
   bcm_oam_group_info_t *group_info,
   void *user_data)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = bcm_petra_oam_endpoint_traverse(unit, group_info->id, _bcm_oam_endpoint_exchange_profile_cb, user_data);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;   
exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Initialize snoop commands for MIP snooping. 
 * Snoop command 1 is set for down (ingress), 2 for up. Traps 
 * are also allocated. 
 * Function should only be called when the appropriate soc prop 
 * is set. 
 * 
 * @author sinai (29/01/2014)
 * 
 * @param unit 
 * @param snoop_destination - presumably the CPU port
 * 
 * @return int 
 */
int _bcm_oam_init_snoop_commands_for_advanced_egress_snooping(int unit, int snoop_destination) {
    int trap_code_up;
	bcm_rx_snoop_config_t snoop_config_cpu;
    int snoop_cmnd;
	bcm_rx_trap_config_t  trap_config_snoop;
    int trap_id_sw;
    int trap_code;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* start with the up-mep (ingress)*/
    snoop_cmnd =_BCM_OAM_MIP_SNOOP_CMND_UP;
    rv = bcm_petra_rx_snoop_create(unit, BCM_RX_SNOOP_WITH_ID, &snoop_cmnd);
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_rx_snoop_config_t_init(&snoop_config_cpu);
    snoop_config_cpu.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO);
    snoop_config_cpu.dest_port = snoop_destination;
    snoop_config_cpu.size = -1;
    snoop_config_cpu.probability = 100000;

    rv = bcm_petra_rx_snoop_set(unit, snoop_cmnd, &snoop_config_cpu);
    BCMDNX_IF_ERR_EXIT(rv);

    trap_code_up = SOC_PPD_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH;
    trap_id_sw = trap_code_up;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code));
    rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine,  &trap_code); /* this trap hasn't been allocated above with the others.*/
    BCMDNX_IF_ERR_EXIT(rv); 

    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = 0;
    trap_config_snoop.trap_strength = 0;
    trap_config_snoop.snoop_cmnd = snoop_cmnd; 
    trap_config_snoop.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
    trap_config_snoop.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
    rv = bcm_petra_rx_trap_set(unit, trap_code, &trap_config_snoop);
    BCMDNX_IF_ERR_EXIT(rv);
    /* Increasing trap code counter so that this trap will not to be deleted with another profile */
    rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_code_up);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * _bcm_oam_default_id_from_ep_id
 *
 * If an endpoint ID is one of the default BCM endpoint IDs, trnaslates the ID to a default endpoint ID for the soc layer.
 * If not a default BCM endpoint ID, returns a parameter error.
 */
int _bcm_oam_default_id_from_ep_id(int unit, int epid, ARAD_PP_OAM_DEFAULT_EP_ID *default_id)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        if (epid != -1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not supported in Arad.")));
        }
    }
    else {
        switch (epid) {
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS0:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_0;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS1:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_1;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS2:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_2;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS3:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_3;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS0:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_EGRESS_0;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS1:
            if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not supported in Arad+/Jericho.")));
            }
            *default_id = ARAD_PP_OAM_DEFAULT_EP_EGRESS_1;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS2:
            if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not supported in Arad+/Jericho.")));
            }
            *default_id = ARAD_PP_OAM_DEFAULT_EP_EGRESS_2;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS3:
            if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not supported in Arad+/Jericho.")));
            }
            *default_id = ARAD_PP_OAM_DEFAULT_EP_EGRESS_3;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not default.")));
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/***************************************************************/
/***************************************************************/

/*
 * Validity Functions
 *
 */

int _bcm_petra_oam_opcode_map_set_verify(int unit, int opcode, int profile) {

    BCMDNX_INIT_FUNC_DEFS;

    if (opcode > SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Opcode out of range.")));
    }

    if (profile > SOC_PPD_OAM_OPCODE_MAP_COUNT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Profile out of range.")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_oam_opcode_map_get_verify(int unit, int opcode) {

    BCMDNX_INIT_FUNC_DEFS;

    if (opcode > SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Opcode out of range.")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * _bcm_petra_oam_is_tlv_used
 *
 * Check if port/interface state TLV is used for the endpoint
 *
 * PARAMS:
 *  endpoint_id - Assumed to also be the MEP_ID entry
 *  used - (OUT) set to 0 if TLV is not used, 1 otherwise
 * RETURN:
 * 	BCM_E_NONE if no error occured
 */
int _bcm_petra_oam_is_tlv_used(int unit, int endpoint_id, uint8 *used) {

	SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  mep_db_entry;

	BCMDNX_INIT_FUNC_DEFS;

	BCMDNX_NULL_CHECK(used);

	soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_id, &mep_db_entry);
    if (mep_db_entry.interface_status_tlv_control || mep_db_entry.port_status_tlv_en) {
		*used = 1;
    }
	else {
		*used = 0;
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Retrieve ccm_period field from the MEP-DB saved ccm_interval */
int _bcm_dpp_oam_endpoint_ccm_period_get(int unit, int endpoint_id,
										 int *ccm_period){

	SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    uint32 ccm_period_ms, ccm_period_micro_s;

	BCMDNX_INIT_FUNC_DEFS;

	BCMDNX_NULL_CHECK(ccm_period);

	SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
	BCM_SAND_IF_ERR_EXIT(soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_id, &mep_db_entry));

	SOC_PPD_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, mep_db_entry.ccm_interval);
	MULTIPLE_BY_3_5(ccm_period_ms, ccm_period_micro_s);
	*ccm_period = ccm_period_ms;

	BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Wrapper function to use bcm_petra_oam_endpoint_get internally */
STATIC int _bcm_oam_endpoint_get_internal_wrapper(int unit, int endpoint_id,
												  bcm_oam_endpoint_info_t *endpoint_info){

    bcm_error_t rv;

	BCMDNX_INIT_FUNC_DEFS;

	BCMDNX_NULL_CHECK(endpoint_info);

	bcm_oam_endpoint_info_t_init(endpoint_info);
	rv = bcm_petra_oam_endpoint_get(unit, endpoint_id, endpoint_info);
	BCMDNX_IF_ERR_EXIT(rv);

	 /* if the use_hw_id soc property is enabled, then the trap_id is the HW
	   id and thus we need to convert it to the sw id becuase the oam logic uses
	   the SW id*/
	if (BCM_GPORT_IS_TRAP(endpoint_info->remote_gport)) {
		_BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, endpoint_info->remote_gport);
	}

	BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
/***************************************************************/
/***************************************************************/

/*
 * Begin BCM Functions
 *
 */

/* Initialize the OAM subsystem */
int bcm_petra_oam_init(int unit) {
	uint32 soc_sand_rv=0;
	bcm_error_t rv ;
	bcm_rx_trap_config_t trap_config_oamp, trap_config_cpu, trap_config_snoop, trap_config_recycle;
	bcm_rx_snoop_config_t snoop_config_cpu;
	bcm_port_config_t port_config;
	bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)], cpu_port, recycle_port, ethernet_port;
	bcm_gport_t recycle_port_iter;
	int count_erp;
	int snoop_cmnd;
	uint32 mirror_id_wb;
	uint32  oamp_local_port;
	uint8 oam_is_init, is_bfd_init;
	int trap_code_with_meta;
    uint32 trap_code_default_egress;
    int mirror_profile_default_egress;
    uint32 flags =0;
    _bcm_dpp_gport_info_t gport_info;
    int fake_destination;
    int core = SOC_CORE_INVALID;
	uint32		pp_port;
    uint8       is_allocated;
    uint32      trap_id;
    int      lb_trap_id;
    int      tst_trap_id;
#ifdef BCM_ARAD_SUPPORT
    SOC_TMC_PORT_HEADER_TYPE header_type_incoming, header_type_outgoing;
#endif /*BCM_ARAD_SUPPORT*/

    BCMDNX_INIT_FUNC_DEFS;

    if(!SOC_WARM_BOOT(unit)) {
        rv = OAM_ACCESS.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = OAM_ACCESS.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* We need also that bfd will be allocated */
        rv = sw_state_access[unit].dpp.bcm.bfd.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.bcm.bfd.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        {
           int soc_sand_rv ;
           soc_sand_rv = bcm_dpp_endp_lst_init(unit,MAX_NOF_ENDP_LSTS) ;
           SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv) ;
           soc_sand_rv = bcm_dpp_endp_lst_m_init(unit,MAX_NOF_ENDP_LST_MS) ;
           SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv) ;
        }
    }

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (oam_is_init) {

		if (!SOC_WARM_BOOT(unit))
		{
			BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: Oam already initialized.\n")));
		}
	}
	else {
		if (SOC_WARM_BOOT(unit)) {
			BCM_EXIT;
		}
	}

	_BCM_BFD_ENABLED_GET(is_bfd_init);

	if (SOC_WARM_BOOT(unit) || (!is_bfd_init)) {
		rv = _bcm_dpp_oam_dbs_init(unit, TRUE/*is_oam*/);
		BCMDNX_IF_ERR_EXIT(rv);
    } else if (SOC_IS_JERICHO(unit)) {
		rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_create(unit);
		BCMDNX_IF_ERR_EXIT(rv); 
	}
 

    rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &count_erp);
    BCMDNX_IF_ERR_EXIT(rv);
    if (count_erp < 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: Oamp port disabled, can't init OAM.\n")));
    }

    rv = bcm_petra_port_config_get(unit, &port_config);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!SOC_WARM_BOOT(unit)) {
        /* Allocate FTMH traps so that they won't be taken by somebody else */
        for (trap_code_with_meta = SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST;
             trap_code_with_meta < (SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST + SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM);
             trap_code_with_meta++) {


            int trap_id_sw;
            int trap_code;

            trap_id_sw = trap_code_with_meta;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit, trap_id_sw, &trap_code));
            rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        
        /* 
         * Configuration of trapping to CPU
         */

        /*get CPU port*/
        BCM_PBMP_ITER(port_config.cpu, cpu_port) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, cpu_port,&pp_port,&core)));

            break; /*get first CPU port*/
        }
        if (SOC_IS_JERICHO(unit)) {
            /*1. create SAT trap for TST */
            rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapSat0, &tst_trap_id);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = OAM_ACCESS.trap_info.trap_ids.set(unit, SOC_PPD_OAM_TRAP_ID_SAT0_TST, (uint32)tst_trap_id);
            BCMDNX_IF_ERR_EXIT(rv);
            /*2. create SAT trap for  for LB*/
            rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapSat1, &lb_trap_id);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = OAM_ACCESS.trap_info.trap_ids.set(unit, SOC_PPD_OAM_TRAP_ID_SAT0_LB, (uint32)lb_trap_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* set oam port profile to 1 */
        rv = _bcm_dpp_gport_to_phy_port(unit, cpu_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port,&pp_port,&core)));
        soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, 1);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_dpp_gport_to_phy_port(unit, oamp_port[0], _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port,&pp_port,&core)));
        oamp_local_port = pp_port;
        /*
         * 'trap_config_cpu' is only used 'if(!SOC_WARM_BOOT(unit))' but we initialize it here to
         * make coverity happy (hopefully).
         */
        bcm_rx_trap_config_t_init(&trap_config_cpu);

        soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, 1);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        trap_config_cpu.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
        trap_config_cpu.dest_port = cpu_port;
        rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                                  SOC_PPD_OAM_TRAP_ID_CPU,
                                                  SOC_PPD_OAM_UPMEP_TRAP_ID_CPU,
                                                  &trap_config_cpu,
                                                  SOC_PPD_TRAP_CODE_OAM_CPU,
                                                  SOC_PPD_TRAP_CODE_OAM_CPU_MIRROR,
                                                  SOC_PPD_OAM_MIRROR_ID_CPU,
                                                  1 /*set_upmep*/);
        BCMDNX_IF_ERR_EXIT(rv);
        /* 
         * Configuration of trapping to recycle port (used for LB)
         */

        /* Getting and checking recycle port */
        if (BCM_PBMP_IS_NULL(port_config.rcy)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Recycling port does not exist.\n")));
        }

        recycle_port = soc_property_get(unit, spn_OAM_RCY_PORT, -1);
        if (recycle_port != -1) {

            BCM_PBMP_ITER(port_config.rcy, recycle_port_iter) {
                if (recycle_port_iter == recycle_port) {
                    break;
                }
            }
            if (recycle_port_iter != recycle_port) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Network Interface of OAM recycling port should be configured as RCY (ucode_port_<Local-Port-Id>=RCY).\n")));
            }


            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, recycle_port,&pp_port,&core)));

            soc_sand_rv = arad_port_header_type_get(unit, core, pp_port, &header_type_incoming, &header_type_outgoing);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if ((header_type_incoming != SOC_TMC_PORT_HEADER_TYPE_TM) || (header_type_outgoing != SOC_TMC_PORT_HEADER_TYPE_ETH)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Recycle port network headers incorrect. Should be header_type_in=TM, header_type_out=ETH.\n")));
            }


            bcm_rx_trap_config_t_init(&trap_config_recycle);
            trap_config_recycle.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
            trap_config_recycle.dest_port = recycle_port;

            rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                                      SOC_PPD_OAM_TRAP_ID_RECYCLE,
                                                      0/*SOC_PPD_OAM_UPMEP_TRAP_ID_RECYCLE*/,
                                                      &trap_config_recycle,
                                                      SOC_PPD_TRAP_CODE_OAM_RECYCLE,
                                                      0/*SOC_PPD_TRAP_CODE_OAM_RECYCLE_MIRROR*/,
                                                      0/*SOC_PPD_OAM_MIRROR_ID_RECYCLE*/,
                                                      0 /*set_upmep*/);
            BCMDNX_IF_ERR_EXIT(rv);

        }

        rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPD_OAM_MIRROR_ID_CPU, &trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = OAM_ACCESS.trap_info.mirror_ids.set(unit, SOC_PPD_OAM_MIRROR_ID_RECYCLE, trap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPD_OAM_MIRROR_ID_CPU, &trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = OAM_ACCESS.trap_info.upmep_trap_ids.set(unit, SOC_PPD_OAM_UPMEP_TRAP_ID_RECYCLE, trap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         * Configuration of snooping to CPU (used for MIP LT)
         */


        /*creating snoop trap id*/
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)
            && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            /* In this case create a snoop_cmnd for down meps and up meps seperatley.*/
            rv = _bcm_oam_init_snoop_commands_for_advanced_egress_snooping(unit, cpu_port);
            BCMDNX_IF_ERR_EXIT(rv);

            snoop_cmnd = _BCM_OAM_MIP_SNOOP_CMND_DOWN;
            flags = BCM_RX_SNOOP_WITH_ID;
        }  else {
            flags = 0;
            snoop_cmnd = 0;
        }

        /* Now the other snoop cmnd for E.S.A., or the first one for other cases.*/
        rv = bcm_petra_rx_snoop_create(unit, flags, &snoop_cmnd);
        BCMDNX_IF_ERR_EXIT(rv);

        bcm_rx_snoop_config_t_init(&snoop_config_cpu);
        snoop_config_cpu.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO);
        snoop_config_cpu.dest_port = cpu_port;
        snoop_config_cpu.size = -1;
        snoop_config_cpu.probability = 100000;

        rv = bcm_petra_rx_snoop_set(unit, snoop_cmnd, &snoop_config_cpu);
        BCMDNX_IF_ERR_EXIT(rv);


        bcm_rx_trap_config_t_init(&trap_config_snoop);
        trap_config_snoop.flags = 0;
        trap_config_snoop.trap_strength = 0;
        trap_config_snoop.snoop_cmnd = snoop_cmnd;

        rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                                  SOC_PPD_OAM_TRAP_ID_SNOOP,
                                                  SOC_PPD_OAM_UPMEP_TRAP_ID_SNOOP,
                                                  &trap_config_snoop,
                                                  SOC_PPD_TRAP_CODE_OAM_CPU_SNOOP,
                                                  (SOC_IS_ARADPLUS_AND_BELOW(unit) ? SOC_PPD_TRAP_CODE_OAM_CPU_SNOOP : SOC_PPD_TRAP_CODE_OAM_CPU_SNOOP_UP),
                                                  SOC_PPD_OAM_MIRROR_ID_SNOOP,
                                                  1 /*set upmep - already set for advanced snooping.*/);
        BCMDNX_IF_ERR_EXIT(rv);



        /* Supporting mip LTM, LBM mirroring */
        rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPD_OAM_MIRROR_ID_CPU, &mirror_id_wb);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_CPU, &trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_id, mirror_id_wb);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_RECYCLE, &trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_id, mirror_id_wb);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         * Configuration of trapping to OAMP (used for accelerated packets)
         */
        bcm_rx_trap_config_t_init(&trap_config_oamp);
        trap_config_oamp.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
        trap_config_oamp.dest_port = oamp_port[0];
        rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                                  SOC_PPD_OAM_TRAP_ID_OAMP,
                                                  SOC_PPD_OAM_UPMEP_TRAP_ID_OAMP,
                                                  &trap_config_oamp,
                                                  SOC_PPD_TRAP_CODE_TRAP_ETH_OAM,
                                                  SOC_PPD_TRAP_CODE_OAM_OAMP_MIRROR,
                                                  SOC_PPD_OAM_MIRROR_ID_OAMP,
                                                  1 /*set_upmep*/);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         * Configuration of trapping to OAMP Y1731 MPLS (used for accelerated packets)
         */
        bcm_rx_trap_config_t_init(&trap_config_oamp);
        trap_config_oamp.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
        trap_config_oamp.dest_port = oamp_port[0];

        rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                                  SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_MPLS,
                                                  0,
                                                  &trap_config_oamp,
                                                  SOC_PPD_TRAP_CODE_TRAP_Y1731_O_MPLS_TP,
                                                  0,
                                                  0,
                                                  0 /*set_upmep*/);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         * Configuration of trapping to OAMP Y1731 PWE (used for accelerated packets)
         */
        bcm_rx_trap_config_t_init(&trap_config_oamp);
        trap_config_oamp.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
        trap_config_oamp.dest_port = oamp_port[0];

        rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                                  SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_PWE,
                                                  0,
                                                  &trap_config_oamp,
                                                  SOC_PPD_TRAP_CODE_TRAP_Y1731_O_PWE,
                                                  0,
                                                  0,
                                                  0 /*set_upmep*/);
        BCMDNX_IF_ERR_EXIT(rv);


    /* 
     * Configuration of trapping to CPU error packets
     */
        /* Creating traps for errors: Level error */
        rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                                  SOC_PPD_OAM_TRAP_ID_ERR_LEVEL,
                                                  SOC_PPD_OAM_UPMEP_TRAP_ID_ERR_LEVEL,
                                                  &trap_config_cpu,
                                                  SOC_PPD_TRAP_CODE_OAM_LEVEL,
                                                  SOC_PPD_TRAP_CODE_OAM_LEVEL,
                                                  SOC_PPD_OAM_MIRROR_ID_ERR_LEVEL,
                                                  1 /*set_upmep*/);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Creating traps for errors: Passive error */
        rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                                  SOC_PPD_OAM_TRAP_ID_ERR_PASSIVE,
                                                  SOC_PPD_OAM_UPMEP_TRAP_ID_ERR_PASSIVE,
                                                  &trap_config_cpu,
                                                  SOC_PPD_TRAP_CODE_OAM_PASSIVE,
                                                  SOC_PPD_TRAP_CODE_OAM_PASSIVE,
                                                  SOC_PPD_OAM_MIRROR_ID_ERR_PASSIVE,
                                                  1 /*set_upmep*/);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Enabling outbound mirroring on all ports */
        BCM_PBMP_ITER(port_config.e, ethernet_port) {
            if (!BCM_PBMP_MEMBER(port_config.rcy, ethernet_port)) {
                uint32 flags;
                BCM_SAND_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, ethernet_port, &flags));

                if (!SOC_PORT_IS_ELK_INTERFACE(flags) && !SOC_PORT_IS_STAT_INTERFACE(flags)) {
                    rv = bcm_petra_mirror_port_dest_add(unit, ethernet_port, BCM_MIRROR_PORT_EGRESS_ACL, 0);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
        soc_sand_rv = soc_ppd_oam_init(unit, FALSE/*not bfd*/);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        _BCM_OAM_ENABLED_SET(TRUE);

        /* Set FLP PTC profile for OAMP port */
        if (SOC_DPP_CONFIG(unit)->pp.oam_statistics == 1) {
            ARAD_PORTS_PROGRAMS_INFO port_prog_info;
            arad_ARAD_PORTS_PROGRAMS_INFO_clear(&port_prog_info);

            soc_sand_rv = arad_ports_programs_info_get_unsafe(unit, oamp_local_port, &port_prog_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            port_prog_info.ptc_flp_profile = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
            soc_sand_rv = arad_ports_programs_info_set_unsafe(unit, oamp_local_port, &port_prog_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        if (SOC_DPP_CONFIG(unit)->pp.oam_statistics == 1) {
            SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              success_vt;
            SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              success_tt;
            ARAD_PP_ISEM_ACCESS_KEY	sem_key;
            ARAD_PP_ISEM_ACCESS_ENTRY	sem_entry;

            /* add entry to tt stage - to indentify the tag format*/
            sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_OAM_STAT_TT;
            sem_key.key_info.oam_stat.cfm_eth_type = ARAD_PP_CFM_ETHER_TYPE;
            rv = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, &success_tt);
            BCMDNX_IF_ERR_EXIT(rv);

            /* add entry to vt stage - down mep*/
            sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_OAM_STAT_VT;
            sem_key.key_info.oam_stat.pph_type = 0x0;
            sem_key.key_info.oam_stat.opaque = 0x7;
            rv = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, &success_vt);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Configuring "fake" destination for recieving CCMs but not sending. The user can give determine the local port for the fake destination by soc property.
           If this is set configure this destination.*/

        fake_destination = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_local_port_for_fake_destination", -1);
        if (fake_destination != -1) {
            uint32 invalid_destination = _BCM_OAM_ILLEGAL_DESTINATION;
            int ignored;
            /* First: some validity checks:*/
            if (SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("soc property \"oam_local_port_for fake_destination\" not needed for Jericho")));

            }
            if (SOC_DPP_CONFIG(unit)->pp.oam_ccm_rx_wo_tx == 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("soc property \"oam_local_port_for fake_destination\" may only be set together with"
                                                               " \"custom_feature_oam_ccm_rx_wo_tx\" \n")));
            }
            if (fake_destination < 0 || fake_destination > 255) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("soc property \"oam_local_port_for fake_destination\" Must be set between 0 and 255"
                                                               " (this value will then be unusable as an LSB for src_mac_address in endpoint_create()")));
            }

            /* Finally, down to business*/
            rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, SHR_RES_ALLOC_WITH_ID, &invalid_destination, &ignored, &fake_destination);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* Following settings should be done in WB as well.*/

    rv = arad_pp_oam_register_dma_event_handler_callback(unit, _bcm_dpp_oam_fifo_interrupt_handler);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_oamp_interrupts_init(unit, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_ccm_two_fhei", 0) == 1) {
        int is_alloced;
        trap_code_default_egress = SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST + SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM - 1;
        rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, SHR_RES_ALLOC_WITH_ID, &trap_code_default_egress);
        BCMDNX_IF_ERR_EXIT(rv);

        mirror_profile_default_egress = SOC_DPP_CONFIG(unit)->pp.oam_ccm_2_fhei_eg_default_mirror_profile;
        rv = _bcm_dpp_am_template_mirror_action_profile_alloc(unit, SHR_RES_ALLOC_WITH_ID, &trap_code_default_egress, &is_alloced, &mirror_profile_default_egress);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit,
                                                                 mirror_profile_default_egress,
                                                                 trap_code_default_egress,
                                                                 3,
                                                                 7);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    }

    rv = _bcm_dpp_oam_sw_db_locks_init(unit);
    BCMDNX_IF_ERR_EXIT(rv); 


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Create or replace an OAM group object */
int bcm_petra_oam_group_create(int unit, bcm_oam_group_info_t *group_info) {

	int flags = 0;
	uint32 ma_index = 0;
    int legal_flags=0;
	int rv = 0;
	_bcm_oam_ma_name_t ma_name_struct;
	int i;
	uint8 new_id_alloced = FALSE;
	uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	/*init ma_name*/
	for (i=0; i<_BCM_OAM_MA_NAME_WITHOUT_HEADERS; i++) {
		ma_name_struct.name[i] = 0;
	}

    /*checking input parameters*/
    legal_flags |= BCM_OAM_GROUP_WITH_ID;
    if (group_info->flags & ~legal_flags)  {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Supported flags BCM_OAM_GROUP_WITH_IDs\n")));
	}

	if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
		rv = bcm_dpp_am_oam_ma_id_is_alloced(unit, group_info->id);
		if (rv==BCM_E_EXISTS) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
				 (_BSL_BCM_MSG("Error: Group with id %d exists\n"), group_info->id));
		}
		else if (rv != BCM_E_NOT_FOUND) {
			BCMDNX_IF_ERR_EXIT(rv);
		}
		flags = SHR_RES_ALLOC_WITH_ID;
		ma_index = group_info->id;
	}

	rv = _bcm_dpp_oam_group_name_to_ma_name_struct_convert(unit, group_info->name, &ma_name_struct);
	BCMDNX_IF_ERR_EXIT(rv);
	rv = bcm_dpp_am_oam_ma_id_alloc(unit, flags, &ma_index);
	BCMDNX_IF_ERR_EXIT(rv);
	new_id_alloced = TRUE;
	group_info->id = ma_index;
	LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: Creating group %d\n"),
                     ma_index));
	rv = _bcm_dpp_oam_ma_db_insert(unit, ma_index, &ma_name_struct);
	BCMDNX_IF_ERR_EXIT(rv);

	BCM_EXIT;
exit:
	if ((rv != BCM_E_NONE) && new_id_alloced) { 
		/* If error after new id was alloced we should free it */
		rv = bcm_dpp_am_oam_ma_id_dealloc(unit, group_info->id);
	}
    BCMDNX_FUNC_RETURN;
}

/* Get an OAM group object */
int bcm_petra_oam_group_get(
    int unit, 
    bcm_oam_group_t group, 
    bcm_oam_group_info_t *group_info) {

	int rv;
	uint8 found;
	_bcm_oam_ma_name_t ma_name_struct;
	uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	if (group < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Group id must be positive. Given id: %d\n"), group));
	}

	group_info->id = group;
    rv = _bcm_dpp_oam_ma_db_get(unit, (uint32)group, &ma_name_struct, &found);
	BCMDNX_IF_ERR_EXIT(rv);

	if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG("Error: Group %d is not valid\n"), group));
	}

	rv = _bcm_dpp_oam_ma_name_struct_to_group_name_convert(unit, &ma_name_struct, group_info->name);
	BCMDNX_IF_ERR_EXIT(rv);

	BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



/* Handles creation of a new local MEP/MIP. */
STATIC int _bcm_oam_endpoint_local_create(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl) {

    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;
    int flags = 0;
    SOC_PPD_OAM_ICC_MAP_DATA data;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    SOC_PPD_OAM_LIF_PROFILE_DATA *profile_data_pointer;
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data;
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data_passive;
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data_acc;
    _bcm_oam_ma_name_t ma_name_struct;
    uint8 allocate_icc_ndx = 0;
    uint8 found, found_profile;
    uint8 new_id_alloced = FALSE;
    int icc_ndx = 0;
    int is_allocated;
    int profile;
    uint32 profile_dummy;
    uint32 endpoint_ref_counter;
    uint8 is_mp_type_flexible;
    uint8 updated_mp_type = 0;
    uint8 is_accelerated = 0;
    uint8 is_upmep;
    uint8 is_passive;
    uint8 is_mip;
    uint8 found_mip;

    BCMDNX_INIT_FUNC_DEFS;

    is_accelerated = _BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info_lcl);
    is_upmep = _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info_lcl);
    is_mip = _BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info_lcl);

    if ((endpoint_info_lcl->gport == BCM_GPORT_INVALID)
        && (endpoint_info_lcl->tx_gport == BCM_GPORT_INVALID)
        && !_BCM_OAM_IS_SERVER_CLIENT(endpoint_info_lcl)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Both gport and"
                                          " tx_gport may not be invalid\n")));
    }

    if ((is_accelerated
         || _BCM_OAM_IS_SERVER_CLIENT(endpoint_info_lcl))
        && _BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info_lcl)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Intermediate endpoint"
                                          " can not be accelerated in HW.\n")));
    }

    /* Check that the group exist - needed for both accelerated and not accelerated */
    rv = _bcm_dpp_oam_ma_db_get(unit, (uint32)endpoint_info_lcl->group, &ma_name_struct, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: group %d does not exist.\n"), endpoint_info_lcl->group));
    }

    if (is_accelerated) { /* TX - OAMP */
        if ((endpoint_info_lcl->tx_gport == BCM_GPORT_INVALID) && (!is_upmep)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG
                                              ("Error: tx_gport invalid, can not add accelerated downward facing endpoint.\n")));
        }

        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: Accelerated endpoint create\n")));

        SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

        if (endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_WITH_ID) {
            rv = _bcm_dpp_oam_bfd_mep_id_is_alloced(unit, endpoint_info_lcl->id);
            if (rv == BCM_E_EXISTS) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                    (_BSL_BCM_MSG("Error: Local endpoint with id %d exists.\n"), endpoint_info_lcl->id));
            } else if (rv != BCM_E_NOT_FOUND) {
                BCMDNX_IF_ERR_EXIT(rv);
            }
            /* mep id given by the user */
            flags = SHR_RES_ALLOC_WITH_ID;
        }
        rv = _bcm_dpp_oam_mep_id_alloc(unit, flags, (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_SHORT),
                                       (uint32 *)(&endpoint_info_lcl->id));
        BCMDNX_IF_ERR_EXIT(rv);
        new_id_alloced = TRUE;

        /*icc index allocation*/
        if (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_SHORT) {
            icc_ndx = 15;
        } else { /*allocate a pointer to ICCMap register that holds Bytes 5:0 of SW.group_name_data_base(group).name. */
            SOC_PPD_OAM_GROUP_NAME_TO_ICC_MAP_DATA(ma_name_struct.name, data);
            rv = _bcm_dpp_am_template_oam_icc_map_ndx_alloc(unit, 0 /*flags*/, &data, &is_allocated, &icc_ndx);
            BCMDNX_IF_ERR_EXIT(rv);
            allocate_icc_ndx = is_allocated;
        }

        rv = _bcm_oam_mep_db_entry_struct_set(unit, endpoint_info_lcl, &mep_db_entry, icc_ndx,
                                              ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES);
        BCMDNX_IF_ERR_EXIT(rv);
        rv =  OAM_ACCESS.acc_ref_counter.get(unit, &endpoint_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
        endpoint_ref_counter++;
        rv = OAM_ACCESS.acc_ref_counter.set(unit, endpoint_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);



        if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info_lcl) && !is_upmep) {
            /* server down MEP. Set the traps.*/
            uint32 server_trap_ref_counter;

            rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.get(unit,
                                                            BCM_GPORT_TRAP_GET_ID(endpoint_info_lcl->remote_gport),
                                                            &server_trap_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            server_trap_ref_counter++;
            rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.set(unit,
                                                            BCM_GPORT_TRAP_GET_ID(endpoint_info_lcl->remote_gport),
                                                            server_trap_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);

            if (server_trap_ref_counter == 1) {
                /* This trap code hasen't been allocated yet */
                soc_sand_rv = soc_ppd_oam_oamp_rx_trap_codes_set(unit, mep_db_entry.mep_type, BCM_GPORT_TRAP_GET_ID(endpoint_info_lcl->remote_gport));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }

        /* At this stage we still may not configure the MEP-DB. The classifier must first be configured so that injected packets are not counted.*/
    }


    /*RX - classifier*/
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    if (endpoint_info_lcl->gport == BCM_GPORT_INVALID) {
        if (!is_accelerated)  {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                (_BSL_BCM_MSG("Error: TX only: endpoint must be accelerated.\n")));
        }
        LOG_WARN(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "OAM: Gport invalid - adding only TX.\n")));

        classifier_mep_entry.flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED;
        classifier_mep_entry.lif = _BCM_OAM_INVALID_LIF; /* Indication that RX does not exist */
        classifier_mep_entry.ma_index = endpoint_info_lcl->group;

        if (is_upmep == 0 && BCM_GPORT_IS_TRUNK(endpoint_info_lcl->tx_gport))  {
            classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER;
        }

        if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info_lcl)) {
            /* additional information needs to be stored. Shove it in other fields.*/
            if (is_upmep) {
                classifier_mep_entry.port = endpoint_info_lcl->tx_gport;
            } else {
                classifier_mep_entry.remote_gport = endpoint_info_lcl->remote_gport;
            }
        }

        /*update sw db of ma->mep & mep_info with new mep */
        rv = _bcm_dpp_oam_ma_to_mep_db_insert(unit, endpoint_info_lcl->group, endpoint_info_lcl->id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        uint32 profile_temp;
        rv = _bcm_oam_classifier_mep_entry_struct_set(unit, endpoint_info_lcl, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info_lcl)) {
            /* It has already been verified that _WITH_ID flag has been set. Update the ID.*/
            _BCM_OAM_SET_MEP_INDEX_FOR_SERVER_CLIENT_SIDE(endpoint_info_lcl->id, endpoint_info_lcl->id);
        } else if (!is_accelerated) { /* non-accelerated - allocate non-accelerated ID */
            endpoint_info_lcl->id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(classifier_mep_entry.lif, endpoint_info_lcl->level,
                                                                         _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry));
        }

        /* inserting an entry to the lem- the key is the in-lif the payload is the counter ID (mep ID)*/
        if (SOC_DPP_CONFIG(unit)->pp.oam_statistics == 1 && is_accelerated) {
            rv =  _bcm_oam_stat_lem_entry_add(unit, &classifier_mep_entry, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        }


        /* Configure two profile - one for actives side and one for passive side */
        for (is_passive = 0; is_passive <= 1; is_passive++) {
            SOC_PPC_LIF_ID lif =  is_passive ? classifier_mep_entry.passive_side_lif : classifier_mep_entry.lif;
            profile_data_pointer = is_passive ? &profile_data_passive : &profile_data;

            /* In Arad Mode Passive profile is fixed and configured separately.
               In Jericho mode there is no need to configure the Passive profile except for MIPs where the other side isn't really passive.*/
            if (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && is_passive
                && !(SOC_IS_JERICHO(unit) && is_mip)) {
                continue;
            }

            /* Validate that mep with same lif&mdlevel does not exist with same direction */
            soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(unit,
                                                                                         lif,
                                                                                         classifier_mep_entry.md_level,
                                                                                         is_passive ^ is_upmep,
                                                                                         &found,
                                                                                         &profile_temp,
                                                                                         &found_profile,
                                                                                         &is_mp_type_flexible,
                                                                                         &found_mip);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* Check if endpoint already exist (only in case of active side) */
            if (!is_passive && found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                    (_BSL_BCM_MSG("Error: Local endpoint with lif %d, mdlevel %d and direction %d(1-up,0-down) exists.\n"),
                                     lif, endpoint_info_lcl->level,_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry)));
            }

            profile = (int)profile_temp;
            if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit) ?
                found_profile == 1   /* In Jericho found_profile may equal 2 in which case only passive entries exist on OEM entry and  mp-profile may be overwriten.*/ :
                found_profile /* No such distinction yet in Arad*/) {

                if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                    /* get profile data existing on this lif */
                    rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
                        /* passive_active_mix and mep_mip mix configuration in Arad+ mode */
                        if (profile_data_pointer->mp_type_passive_active_mix && is_mip) {
                            /* Previous type was up/down, now trying to add mip */
                            if (is_mp_type_flexible) {
                                profile_data_pointer->mp_type_passive_active_mix = FALSE;
                                updated_mp_type = 1;
                            } else {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                    (_BSL_BCM_MSG("Error: Only two combinations of meps on same lif are possible - mips and meps (all up or down) OR only meps (up and down)."
                                                                  "Existing meps conflict with this constraint.\n")));
                            }
                        } else if (!profile_data_pointer->mp_type_passive_active_mix
                                   && !is_mip
                                   && !is_passive) {
                            /* Previous type was mep/mip, now trying to add mep */
                            /* Valid only in case the mep is on same direction as the existing mep, so looking for meps on the other direction */
                            /* (active side check only) */
                            _BCM_OAM_MEP_EXIST_ON_LIF_AND_DIRECTION(lif, !_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry),
                                                                    0/*find_mip*/, found, found_mip, profile_dummy);

                            if (found & !found_mip) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                    (_BSL_BCM_MSG("Error: Only two combinations of meps on same lif are possible - mips and meps (all up or down) OR only meps (up and down)."
                                                                  "Existing meps conflict with this constraint.\n")));

                            }
                        }
                    }
                } else if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                    /* Arad mode */
                    if (!is_mip) {
                        if (profile == _BCM_OAM_PASSIVE_PROFILE) {
                            /* Get a new profile */
                            SOC_PPD_OAM_LIF_PROFILE_DATA_clear(profile_data_pointer);
                            rv = _bcm_oam_default_profile_get(unit, profile_data_pointer, &classifier_mep_entry, 0 /* non accelerated */, 0 /* is default */, is_upmep);
                            BCMDNX_IF_ERR_EXIT(rv);
                        } else {
                            /* get profile data existing on this lif */
                            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    } else {
                        /* This is a mip and adopts the direction of the existing mep */
                        if (profile == _BCM_OAM_PASSIVE_PROFILE) {
                            /* The existing mep is upmep */
                            classifier_mep_entry.flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP;
                            /* Getting the active profile */
                            soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(
                               unit, lif, classifier_mep_entry.md_level,1/*is_upmep*/,
                               &found, &profile_temp, &found_profile, &is_mp_type_flexible, &found_mip);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                            profile = (int)profile_temp;
                        }
                        /* get profile data existing on this lif */
                        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                } else { /* Jericho mode*/
                    /* Simply get profile data existing on this lif */
                    rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            } else { /* Profile not found */
                SOC_PPD_OAM_LIF_PROFILE_DATA_clear(profile_data_pointer);
                rv = _bcm_oam_default_profile_get(unit, profile_data_pointer, &classifier_mep_entry, 0 /* non accelerated */, 0 /* is default */, is_passive ^ is_upmep);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* Allocate profile */
            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(unit, 0/*flags*/, profile_data_pointer, &is_allocated, &profile);
            BCMDNX_IF_ERR_EXIT(rv);

            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (updated_mp_type)) {
                /* Move all existing endpoints on this lif to the new profile */
                bcm_oam_endpoint_traverse_cb_profile_info_t profile_info;
                profile_info.first_endpoint_id = BCM_OAM_ENDPOINT_INVALID;
                profile_info.lif = lif;
                profile_info.profile_data = profile_data_pointer;
                profile_info.is_ingress = is_upmep ^ !is_passive;
                /* Iterate over all groups to find all the endpoints on same lif and switch their profile */
                rv = bcm_petra_oam_group_traverse(unit, _bcm_oam_endpoint_exchange_profile_group_cb, &profile_info);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (is_passive) {
                classifier_mep_entry.non_acc_profile_passive = (uint8)profile;
            } else {
                classifier_mep_entry.non_acc_profile = (uint8)profile;
            }

            if (is_allocated) {
                rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data_pointer->mep_profile_data);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data_pointer->mip_profile_data);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (updated_mp_type || !found_profile)) {
                soc_sand_rv = soc_ppd_oam_mep_passive_active_enable_set(unit, profile, profile_data_pointer->mp_type_passive_active_mix);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        } /* End of passive/active for loop*/

        if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (updated_mp_type)) {
            soc_sand_rv = soc_ppd_oam_classifier_oem_mep_profile_replace(unit, &classifier_mep_entry, 1 /*update mp-type*/);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data_acc);
        /* New Arad+ Classifier: in new mode this should always be done */
        /* When using a remote OAMP as a server OAM-ID must be stamped on the FHEI. This is taken from the O-EM2 table*/
        if ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && !is_mip) ||
            is_accelerated || _BCM_OAM_IS_SERVER_CLIENT(endpoint_info_lcl)) {
            rv = _bcm_oam_default_profile_get(unit, &profile_data_acc, &classifier_mep_entry, is_accelerated, 0 /* is default */, is_upmep);
            BCMDNX_IF_ERR_EXIT(rv);

            if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info_lcl)) {
                uint32 trap_code;
                rv = _bcm_dpp_oam_bfd_trap_code_from_remote_gport(unit, endpoint_info_lcl->remote_gport, &trap_code, TRUE /*META data*/, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);

                /* set the trap code for CCM m-cast to the above trap code.*/
                profile_data_acc.mep_profile_data.opcode_to_trap_code_multicast_map[SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM] = trap_code;
                classifier_mep_entry.remote_gport = endpoint_info_lcl->remote_gport; /* Also used by OAM-1-2 and for get APIs*/

                if (!is_upmep) {
                    int trap_id;
                    int trap_id_sw;
                    /* update the trap code to calculate the FWD header offset*/
                    bcm_rx_trap_config_t trap_config;
                    bcm_rx_trap_config_t_init(&trap_config);

                    trap_id_sw =  BCM_GPORT_TRAP_GET_ID(endpoint_info_lcl->remote_gport);
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit, trap_id_sw, &trap_id));
                    rv = bcm_petra_rx_trap_get(unit, trap_id, &trap_config);
                    BCMDNX_IF_ERR_EXIT(rv);
                    trap_config.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                    trap_config.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
                    rv = bcm_petra_rx_trap_set(unit, trap_id, &trap_config);
                    BCMDNX_IF_ERR_EXIT(rv);

                }
            }

            rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_alloc(unit, 0/*flags*/, &profile_data_acc, &is_allocated, &profile);
            BCMDNX_IF_ERR_EXIT(rv);

            classifier_mep_entry.acc_profile = (uint8)profile;

            if (is_allocated) {
                rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data_acc.mep_profile_data);
                BCMDNX_IF_ERR_EXIT(rv);
            }

        }

        rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info_lcl, is_upmep ?  classifier_mep_entry.passive_side_lif :  classifier_mep_entry.lif);
        BCMDNX_IF_ERR_EXIT(rv);


        /* Active profile OAM table configuration */
        soc_sand_rv = soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile(
           unit,
           SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint_info_lcl->id),
           &classifier_mep_entry,
           &profile_data,
           &profile_data_acc);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
            /* Passive profile OAM table configuration */
            soc_sand_rv = soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile(
               unit, 0, &classifier_mep_entry, &profile_data_passive, NULL);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        soc_sand_rv = soc_ppd_oam_classifier_oem_mep_add(unit, endpoint_info_lcl->id, &classifier_mep_entry, 0/*update*/);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /*update sw db of ma->mep & mep_info with new mep */
        rv = _bcm_dpp_oam_ma_to_mep_db_insert(unit, endpoint_info_lcl->group, endpoint_info_lcl->id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }


    if (is_accelerated) {
        /* Now that the classifier has been updated it is safe to transmit CCMs.*/
        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info_lcl->id, &mep_db_entry, allocate_icc_ndx, ma_name_struct.name);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (SOC_IS_ARADPLUS(unit)) {
            /* Get the RDI in the eth1731 profile.*/
            SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth_prof;
            SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth_prof);
            /* By default RDI is taken from scanner | RX */
            _BCM_OAM_SET_RDI_ON_ETH1731_PROF_RDI_FIELD(eth_prof.rdi_gen_method, endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE,
                                                       endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE);
            rv = _bcm_dpp_am_template_oam_eth_1731_mep_profile_RDI_alloc(unit, &eth_prof, &is_allocated, &profile);
            BCMDNX_IF_ERR_EXIT(rv);
            soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_info_lcl->id, !is_allocated, profile, &eth_prof);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

    }
    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM: Inserting local endpoint with id %d to group with id %d\n"),
                 endpoint_info_lcl->id, endpoint_info_lcl->group));
exit:
	if ((SOC_SAND_FAILURE(soc_sand_rv) || (BCM_FAILURE(rv))) && new_id_alloced) {
		/* If error after new id was alloced we should free it */
        if (is_accelerated) { /* TX - OAMP */
            /* There's no point in checking the return value here. This
               is already an error containment code */
            /* coverity[check_return:FALSE] */
            _bcm_dpp_oam_bfd_mep_id_dealloc(unit, endpoint_info_lcl->id);
            if (allocate_icc_ndx) {
                /* coverity[check_return:FALSE] */
                _bcm_dpp_am_template_oam_icc_map_ndx_free(unit, icc_ndx, &is_allocated/*dummy*/);
            }
            /* coverity[check_return:FALSE] */
            _bcm_dpp_oam_ma_to_mep_db_mep_delete(unit, endpoint_info_lcl->group, endpoint_info_lcl->id);
            /* coverity[check_return:FALSE] */
            _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint_info_lcl->id);
        }
	}
    BCMDNX_FUNC_RETURN;
}

/* Create new remote endpoint */
STATIC int _bcm_oam_endpoint_remote_create(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl) {

	uint32 soc_sand_rv = SOC_SAND_OK;
	int rv = 0;
	int flags = 0;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
	SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
	SOC_PPD_OAM_RMEP_INFO_DATA rmep_info;
	uint8 found;
	uint8 new_id_alloced = FALSE;
	uint32 rmep_id_internal;

    BCMDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM: Remote endpoint create.\n")));

    SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    if (endpoint_info_lcl->loc_clear_threshold < 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: loc_clear_threshold must be positive. Current value:%d\n"),
                                          endpoint_info_lcl->loc_clear_threshold));
    }

    /* find associated MEP and check if it's accelerated */
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->local_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                            (_BSL_BCM_MSG("Error: Endpoint with id %d does not exist.\n"), endpoint_info_lcl->local_id));
    }
    if (! _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Endpoint with id %d is not accelerated, remote endpoint can not be added\n"),
                                          endpoint_info_lcl->local_id));
    }

    if ((endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_WITH_ID)) {
        if (!_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint_info_lcl->id)) {
            _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(endpoint_info_lcl->id, endpoint_info_lcl->id);
        }
        rv = bcm_dpp_am_oam_rmep_id_is_alloced(unit, endpoint_info_lcl->id);
        if (rv == BCM_E_EXISTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                (_BSL_BCM_MSG("Error: Remote endpoint with id %d exists\n"), endpoint_info_lcl->id));
        } else if (rv != BCM_E_NOT_FOUND) {
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /* new rmep id given by the user */
        flags = SHR_RES_ALLOC_WITH_ID;
    }
    rv = bcm_dpp_am_oam_rmep_id_alloc(unit, flags, (uint32 *)&endpoint_info_lcl->id);
    BCMDNX_IF_ERR_EXIT(rv);
    new_id_alloced = TRUE;

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM: Inserting remote endpoint with id %d to mep with id %d\n"),
                 endpoint_info_lcl->id, endpoint_info_lcl->local_id));

    /*ccm_period - If not specified 3.5 times the transmitter MEP ccm_period*/
    if (endpoint_info_lcl->ccm_period == 0) {
        rv = _bcm_dpp_oam_endpoint_ccm_period_get(unit, endpoint_info_lcl->local_id,
                                                  &endpoint_info_lcl->ccm_period);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_oam_rmep_db_entry_struct_set(unit, endpoint_info_lcl, &rmep_db_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sw db of mep->rmep with the new rmep */
    rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(unit, endpoint_info_lcl->local_id, endpoint_info_lcl->id);
    BCMDNX_IF_ERR_EXIT(rv);
    SOC_PPD_OAM_RMEP_INFO_DATA_clear(&rmep_info);
    rmep_info.mep_index = endpoint_info_lcl->local_id;
    rmep_info.rmep_id = endpoint_info_lcl->name;
    rv = _bcm_dpp_oam_bfd_rmep_info_db_insert(unit, endpoint_info_lcl->id, &rmep_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update entry in rmep db and rmep index db*/
    _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint_info_lcl->id);
    soc_sand_rv = soc_ppd_oam_oamp_rmep_set(unit, rmep_id_internal,  endpoint_info_lcl->name,
                                            endpoint_info_lcl->local_id, SOC_PPD_OAM_MEP_TYPE_ETH_OAM, &rmep_db_entry, 0);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
	if ((SOC_SAND_FAILURE(soc_sand_rv) || (BCM_FAILURE(rv))) && new_id_alloced) {
		/* If error after new id was alloced we should free it */
		bcm_dpp_am_oam_rmep_id_dealloc(unit, endpoint_info_lcl->id);
		_bcm_dpp_oam_bfd_mep_to_rmep_db_rmep_delete(unit, endpoint_info_lcl->local_id, endpoint_info_lcl->id);
		_bcm_dpp_oam_bfd_rmep_info_db_delete(unit, endpoint_info_lcl->id);
	}
    BCMDNX_FUNC_RETURN;
}

/* Create new default endpoint */
STATIC int _bcm_oam_endpoint_default_create(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl,
                                            ARAD_PP_OAM_DEFAULT_EP_ID default_id) {
	uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;

	SOC_PPD_OAM_LIF_PROFILE_DATA profile_data;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
	int profile, is_allocated;
	uint8 is_upmep, found;

    BCMDNX_INIT_FUNC_DEFS;

	is_upmep = _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info_lcl);

	if (SOC_IS_ARAD_B1_AND_BELOW(unit)) { /* Arad default profile */

        SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
        SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data);

        classifier_mep_entry.mep_type = SOC_PPD_OAM_MEP_TYPE_ETH_OAM; /* Irrelevant to default. Setting to 0 */
        classifier_mep_entry.flags |= is_upmep ? SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP : 0;
        classifier_mep_entry.flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
        classifier_mep_entry.non_acc_profile = SOC_PPD_OAM_PROFILE_DEFAULT;
        rv = _bcm_oam_default_profile_get(unit, &profile_data, &classifier_mep_entry, 0 /* non accelerated */, 1 /* is default */, is_upmep);
        BCMDNX_IF_ERR_EXIT(rv);

        profile = 0;
        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(unit, SHR_TEMPLATE_MANAGE_SET_WITH_ID, &profile_data, &is_allocated, &profile);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_allocated) {
            rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data.mep_profile_data);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data.mip_profile_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, -1, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &classifier_mep_entry, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    } else if (SOC_IS_ARADPLUS(unit)) { /* Arad+/Jericho default profiles */

		rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                (_BSL_BCM_MSG("Error: Default endpoint already exists.\n")));
        }

        SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
        SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data);

        rv = _bcm_oam_classifier_default_mep_entry_struct_set(unit, endpoint_info_lcl, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_oam_default_profile_get(unit, &profile_data, &classifier_mep_entry,
                                          0 /* non accelerated */, 1 /* is default */, is_upmep);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(unit, 0/*flags*/, &profile_data, &is_allocated, &profile);
        BCMDNX_IF_ERR_EXIT(rv);

        classifier_mep_entry.non_acc_profile = (uint8)profile;

        if (is_allocated) {
            rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data.mep_profile_data);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data.mip_profile_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info_lcl, _BCM_OAM_INVALID_LIF);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_add,
                                          (unit, default_id,&classifier_mep_entry, 0/*update_action_only*/));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCM_DPP_UNIT_CHECK(unit);
        soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &classifier_mep_entry, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* The state of the internal registers determining outgoing packets may have been changed, In this case outgoing LBMs
   must reflect this change.*/
STATIC int _bcm_oam_endpoint_loopback_update_for_local_mep_replace(int unit, bcm_oam_endpoint_info_t *endpoint) {

    int rv = 0;
    bcm_oam_loopback_t lb = { 0 };
    bcm_oam_loopback_t lb_get = { 0 };
    uint32 lsbs, msbs;
    int dummy;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_oam_loopback_t_init(&lb);
    bcm_oam_loopback_t_init(&lb_get);

    /* bcm_petra_oam_loopback_get sets values to fields that
       are forbidden during set. Using a different object for
       the update copying only the relevant fields */
    rv = bcm_petra_oam_loopback_get(unit, &lb_get);
    BCMDNX_IF_ERR_EXIT(rv);

    lb.id = endpoint->id;
    lb.flags = lb_get.flags | BCM_OAM_LOOPBACK_UPDATE;
    lb.period = lb_get.period;
    if (endpoint->type == bcmOAMEndpointTypeEthernet) {
        rv = _bcm_dpp_am_template_oam_lmm_oui_tables_get(unit, endpoint->id, &dummy, &msbs);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_am_template_oam_lmm_nic_tables_get(unit, endpoint->id, &dummy, &lsbs);
        BCMDNX_IF_ERR_EXIT(rv);
        SET_MAC_ADDRESS_BY_MSB_AND_LSB(lb.peer_da_mac_address, lsbs, msbs);
    }

    if (lb_get.num_tlvs) {
        sal_memcpy(&lb.tlvs[0], &lb_get.tlvs[0], sizeof(bcm_oam_tlv_t) * lb_get.num_tlvs);
        lb.num_tlvs = lb_get.num_tlvs;
    }

    rv = bcm_petra_oam_loopback_add(unit, &lb);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Update local MEP/MIP */
STATIC int _bcm_oam_endpoint_local_replace(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl,
                                                 bcm_oam_endpoint_info_t *existing_endpoint_info,
                                                 SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {

    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    _bcm_oam_ma_name_t ma_name_struct;
    int is_last;
    int icc_ndx = 0;
    int is_allocated;
    int profile;
    uint8 is_accelerated = 0;
    uint8 is_upmep;
	uint8 found;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_oam_endpoint_local_replace_validity_check(unit, endpoint_info_lcl,
                                                        existing_endpoint_info,
                                                        classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check that the group exist - needed for both accelerated and not accelerated */
    rv = _bcm_dpp_oam_ma_db_get(unit, (uint32)endpoint_info_lcl->group, &ma_name_struct, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: group %d does not exist.\n"), endpoint_info_lcl->group));
    }

    is_upmep = _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info_lcl);
    is_accelerated = _BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info_lcl);

    if (is_accelerated) { /* TX - OAMP */
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: Accelerated endpoint update\n")));

        /* First, clear the initial data*/
        SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

        /*get entry of mep db*/
        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_info_lcl->id, &mep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* Down MEP, may be switching to "fake" destination. Save the original one. */
        classifier_mep_entry->dip_profile_ndx = existing_endpoint_info->tx_gport;
        classifier_mep_entry->src_mac_lsb = existing_endpoint_info->src_mac_address[5];
        rv = _bcm_oam_mep_db_entry_dealloc(unit, endpoint_info_lcl->id, &mep_db_entry, classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        icc_ndx = mep_db_entry.icc_ndx;

        SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

		rv = _bcm_oam_mep_db_entry_struct_set(unit, endpoint_info_lcl, &mep_db_entry, icc_ndx,
                                              ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES);
        BCMDNX_IF_ERR_EXIT(rv);
        /*update entry in mep db*/
        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info_lcl->id, &mep_db_entry, 0, ma_name_struct.name);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (SOC_IS_ARADPLUS(unit)) {
            SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY  old_prof;
            uint8 new_rdi;
            int new_profile;
            /* RDI may have been updated. */
            _BCM_OAM_SET_RDI_ON_ETH1731_PROF_RDI_FIELD(new_rdi, endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE,
                                                       endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE);
            rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, endpoint_info_lcl->id, &profile, &old_prof);
            BCMDNX_IF_ERR_EXIT(rv);
            if (old_prof.rdi_gen_method != new_rdi) {
                old_prof.rdi_gen_method = new_rdi;
                rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_exchange(unit, profile, &old_prof, &is_allocated, &is_last, &new_profile);
                BCMDNX_IF_ERR_EXIT(rv);

                if (profile != new_profile) {
                    CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last, profile, endpoint_info_lcl->id);
                }

                soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_info_lcl->id, !is_allocated, new_profile, &old_prof);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }



            if (classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK) {
                /* Make sure LBMs are also updated */
                rv = _bcm_oam_endpoint_loopback_update_for_local_mep_replace(unit, endpoint_info_lcl);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

    }
    /*RX - classifier*/

    if (endpoint_info_lcl->gport != BCM_GPORT_INVALID) {
        SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY new_classifier_mep_entry;
        SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&new_classifier_mep_entry);
        rv = _bcm_oam_classifier_mep_entry_struct_set(unit, endpoint_info_lcl, &new_classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        /* mep profiles weren't changed. Update the pointers from the old entry*/
        new_classifier_mep_entry.non_acc_profile = classifier_mep_entry->non_acc_profile;
        new_classifier_mep_entry.acc_profile = classifier_mep_entry->acc_profile;
        new_classifier_mep_entry.non_acc_profile_passive = classifier_mep_entry->non_acc_profile_passive;
        new_classifier_mep_entry.dip_profile_ndx = classifier_mep_entry->dip_profile_ndx;

        if (endpoint_info_lcl->lm_counter_base_id != classifier_mep_entry->counter) {
            if (endpoint_info_lcl->lm_counter_base_id > 0) {
                rv = _bcm_dpp_oam_set_counter(unit, endpoint_info_lcl->lm_counter_base_id,
                                              ((endpoint_info_lcl->lm_flags & BCM_OAM_LM_PCP) != 0),
                                              is_upmep ? new_classifier_mep_entry.passive_side_lif
                                                        : new_classifier_mep_entry.lif);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (!SOC_IS_JERICHO(unit)) { /* For Jericho the LIF must be set.*/
                rv = _bcm_dpp_oam_lif_table_set(unit,
                          is_upmep ? new_classifier_mep_entry.passive_side_lif : new_classifier_mep_entry.lif, 0);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            soc_sand_rv = soc_ppd_oam_classifier_oem_mep_add(unit, endpoint_info_lcl->id,
                                                             &new_classifier_mep_entry, 1/*update*/);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }


        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, endpoint_info_lcl->id, &new_classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Update Remote MEP */
STATIC int _bcm_oam_endpoint_remote_replace(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl) {

	uint32 soc_sand_rv = SOC_SAND_OK;
	int rv = 0;
	SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
	SOC_PPD_OAM_RMEP_INFO_DATA rmep_info;
	uint8 found;
	uint32 rmep_id_internal;

    BCMDNX_INIT_FUNC_DEFS;

	rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, endpoint_info_lcl->id, &rmep_info, &found);
	BCMDNX_IF_ERR_EXIT(rv);
	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
		   (_BSL_BCM_MSG("Error: Remote endpoint with id %d can not be found.\n"), endpoint_info_lcl->id));
	}

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit,
											"OAM: Remote endpoint replace.\n")));

	if (endpoint_info_lcl->loc_clear_threshold < 1) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: loc_clear_threshold must be positive. Current value:%d\n"),
								endpoint_info_lcl->loc_clear_threshold));
	}

	/* Check parameters that can not be changed */
	if (endpoint_info_lcl->local_id != rmep_info.mep_index) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: local id can not be changed. Current value:%d\n"),
								rmep_info.mep_index));
	}

	/*ccm_period*/
	if (endpoint_info_lcl->ccm_period==0) {
		rv = _bcm_dpp_oam_endpoint_ccm_period_get(unit, endpoint_info_lcl->local_id,
												  &endpoint_info_lcl->ccm_period);
		BCMDNX_IF_ERR_EXIT(rv);
	}

	SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);

	_BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint_info_lcl->id);

	rv = _bcm_dpp_oam_bfd_rmep_db_entry_dealloc(unit, rmep_id_internal);
	BCMDNX_IF_ERR_EXIT(rv);



	rv = _bcm_oam_rmep_db_entry_struct_set(unit, endpoint_info_lcl, &rmep_db_entry);
	BCMDNX_IF_ERR_EXIT(rv);

	/* Update sw db of rmep with the new rmep */
	rv = _bcm_dpp_oam_bfd_rmep_to_mep_db_update(unit, endpoint_info_lcl->id, &rmep_info);
	BCMDNX_IF_ERR_EXIT(rv);

	/* Update entry in rmep db and rmep index db*/
	_BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint_info_lcl->id);
	soc_sand_rv = soc_ppd_oam_oamp_rmep_set(unit, rmep_id_internal, endpoint_info_lcl->name,
											endpoint_info_lcl->local_id, endpoint_info_lcl->type,
											&rmep_db_entry, 1);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Update default MEP (LIF-Profile endpoint)*/
STATIC int _bcm_oam_endpoint_default_replace(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl,
                                             bcm_oam_endpoint_info_t *existing_endpoint_info,
                                             SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
											 ARAD_PP_OAM_DEFAULT_EP_ID default_id){
    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;

	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY new_classifier_mep_entry;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_oam_endpoint_local_replace_validity_check(unit, endpoint_info_lcl,
														existing_endpoint_info,
														classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&new_classifier_mep_entry);
    rv = _bcm_oam_classifier_default_mep_entry_struct_set(unit, endpoint_info_lcl,
                                                          &new_classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    /* mep profiles weren't changed. Update the pointers from the old entry*/
    new_classifier_mep_entry.non_acc_profile = classifier_mep_entry->non_acc_profile;

    if (endpoint_info_lcl->lm_counter_base_id != classifier_mep_entry->counter) {
        if (endpoint_info_lcl->lm_counter_base_id > 0) {
            rv = _bcm_dpp_oam_set_counter(unit, endpoint_info_lcl->lm_counter_base_id,
                                          ((endpoint_info_lcl->lm_flags & BCM_OAM_LM_PCP) != 0),
                                          _BCM_OAM_INVALID_LIF);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            rv = _bcm_dpp_oam_lif_table_set(unit, _BCM_OAM_INVALID_LIF, 0);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_add,
                                      (unit, default_id,&new_classifier_mep_entry, 0/*update_action_only*/));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_DPP_UNIT_CHECK(unit);

    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, endpoint_info_lcl->id, &new_classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_oam_endpoint_create(int unit, bcm_oam_endpoint_info_t *endpoint_info) {

	int rv = 0;

	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
	uint8 found;
    uint8 is_default = 0;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
	uint8 oam_is_init;
    bcm_oam_endpoint_info_t endpoint_info_lcl_t;
    bcm_oam_endpoint_info_t *endpoint_info_lcl = NULL;
	bcm_oam_endpoint_info_t existing_endpoint_info; /* For replacing */

    BCMDNX_INIT_FUNC_DEFS;

	BCMDNX_NULL_CHECK(endpoint_info);

	_BCM_OAM_ENABLED_GET(oam_is_init);
	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	/*
	 * If the GPORT contains a trap, since the trap id inside
	 * the GPORT can be either the HW id or the SW_id (according
	 * to the use_hw_id SOC peroperty), and this function assumes
	 * that the trap id is the SW id, it will use a copy of the
	 * endpoint_info with the SW id
	 */
    endpoint_info_lcl = &endpoint_info_lcl_t;
    sal_memcpy(endpoint_info_lcl, endpoint_info, sizeof(bcm_oam_endpoint_info_t));
    if (BCM_GPORT_IS_TRAP(endpoint_info_lcl->remote_gport)) {
        _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, endpoint_info_lcl->remote_gport);
    }

    is_default = _BCM_OAM_IS_MEP_ID_DEFAULT(unit, endpoint_info_lcl->id);

    /* preliminary validity check of endpoint_info. Relavent only when type==Eth */

    rv = _bcm_oam_endpoint_validity_checks_all(unit, endpoint_info_lcl);
    BCMDNX_IF_ERR_EXIT(rv);
	
    if (is_default) {
        /* Check flags limitations */
        rv = _bcm_oam_default_endpoint_validity_checks(unit, endpoint_info_lcl);
        BCMDNX_IF_ERR_EXIT(rv);
		rv = _bcm_oam_default_id_from_ep_id(unit, endpoint_info->id, &default_id);
		BCMDNX_IF_ERR_EXIT(rv);
    }
    else { /* Not default */

        rv = _bcm_oam_generic_endpoint_validity_checks(unit, endpoint_info_lcl);
        BCMDNX_IF_ERR_EXIT(rv);

        if (endpoint_info_lcl->type == bcmOAMEndpointTypeEthernet) {
            rv = _bcm_oam_ethernet_endpoint_validity_checks(unit, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHMPLS
				   || endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPwe) {
            rv = _bcm_oam_mpls_pwe_endpoint_validity_checks(unit, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv); 
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: Supporting only Ethernet, BHH-MPLS%stype.\n"),
												SOC_IS_JERICHO(unit) ? ", BHH-PWE " : " "));
        }
    }
	/* preliminary validity check complete */


   
	/* Endpoint replace */
	if (endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_REPLACE) {

		/* Retrieve the existing endpoint information */
		bcm_oam_endpoint_info_t_init(&existing_endpoint_info);
		rv = _bcm_oam_endpoint_get_internal_wrapper(unit, endpoint_info_lcl->id,
													&existing_endpoint_info);
		BCMDNX_IF_ERR_EXIT(rv);


		if (existing_endpoint_info.flags & BCM_OAM_ENDPOINT_REMOTE) {
			rv = _bcm_oam_endpoint_remote_replace(unit, endpoint_info_lcl);
			BCMDNX_IF_ERR_EXIT(rv);
		}
		else {
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id,
												  &classifier_mep_entry, &found);
			BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
				   (_BSL_BCM_MSG("Local endpoint with id %d can not be found.\n"),
					endpoint_info_lcl->id));
			}

            if (is_default) {
                rv = _bcm_oam_endpoint_default_replace(unit, endpoint_info_lcl,
                                                             &existing_endpoint_info,
                                                             &classifier_mep_entry,
															 default_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                rv = _bcm_oam_endpoint_local_replace(unit, endpoint_info_lcl,
                                                             &existing_endpoint_info,
                                                             &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
	}
	else { /* New endpoint */
		/* Default OAM trap */
        if (is_default) {
            rv = _bcm_oam_endpoint_default_create(unit, endpoint_info_lcl, default_id);
			BCMDNX_IF_ERR_EXIT(rv);
        }
        else { /* Non-default new endpoint */

            if (endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_REMOTE) {
				/* New remote MEP */
				rv = _bcm_oam_endpoint_remote_create(unit, endpoint_info_lcl);
                BCMDNX_IF_ERR_EXIT(rv);
			}
            else {
				/* New local MIP/MEP */
                rv = _bcm_oam_endpoint_local_create(unit, endpoint_info_lcl);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
	}

   

    BCM_EXIT;

exit:
    /* Both endpoint_info, endpoint_info_lcl will not be null at this stage. endpoint_info_lcl points to a local variable.*/
    /* coverity[var_deref_model : FALSE] */     
   sal_memcpy(endpoint_info, endpoint_info_lcl, sizeof(bcm_oam_endpoint_info_t));
	/* if the use_hw_id soc property is enabled, then the trap_id is the SW
	   id and thus we need to convert it to the HW id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP(endpoint_info->remote_gport)) {
        _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit, endpoint_info->remote_gport);
    }
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint, 
								bcm_oam_endpoint_info_t *endpoint_info)  
{
    uint32 soc_sand_rv=0;
    int rv;
    uint8 found;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 oam_is_init;
    uint8 is_default;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    is_default = _BCM_OAM_IS_MEP_ID_DEFAULT(unit, endpoint);

    if ((endpoint < 0) && !is_default) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Endpoint id must be positive or default. Given id: %d\n"), endpoint));
    }

    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    endpoint_info->id = endpoint;
    /* Default OAM trap */
    if (is_default) {
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Default profile not configured.\n")));
        }

        endpoint_info->flags |= _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry) ? BCM_OAM_ENDPOINT_UP_FACING : 0;
        if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) {
            endpoint_info->group = classifier_mep_entry.ma_index;
            endpoint_info->level = classifier_mep_entry.md_level;
            endpoint_info->lm_counter_base_id = classifier_mep_entry.counter;
        }

        BCM_EXIT;
    }

    if (_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint)) {
        /* remote endpoint */ 
        SOC_PPD_OAM_RMEP_INFO_DATA rmep_info;
        uint32 rmep_id_internal;
        SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;

        SOC_PPD_OAM_RMEP_INFO_DATA_clear(&rmep_info);
        SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);
        /* verify that rmep exists */
        rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, endpoint, &rmep_info, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
        }

        /*get entry from oamp */
        _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint);
        soc_sand_rv = soc_ppd_oam_oamp_rmep_get(unit, rmep_id_internal, &rmep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_oam_rmep_db_entry_struct_get(unit, &rmep_db_entry, rmep_info.rmep_id, rmep_info.mep_index, endpoint_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else { /* local endpoint */
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
        }
        /* RX - classifier: Must be first. Accelerated parts relay on fields in "endpoint_info" filled here.*/
        if (classifier_mep_entry.lif != _BCM_OAM_INVALID_LIF) { /* Indication that RX does not exist */
            rv = _bcm_oam_classifier_mep_entry_struct_get(unit, endpoint_info, &classifier_mep_entry);
            BCMDNX_IF_ERR_EXIT(rv);
            
            if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint)) {
                 /* In addition the remote gport field needs to be filled (This was set in the classifier struct accordingly)*/
               endpoint_info->remote_gport =  classifier_mep_entry.remote_gport ;
            }
        } else {
            endpoint_info->opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
            endpoint_info->group = classifier_mep_entry.ma_index;
            endpoint_info->gport = BCM_GPORT_INVALID;

        }
        /*TX - OAMP */
        if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
            SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
            /*get entry of mep db*/

            SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

            soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint, &mep_db_entry);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (classifier_mep_entry.lif == _BCM_OAM_INVALID_LIF) {
                /* Fill the OAMP server fields.*/
                if (classifier_mep_entry.port) {
                    /* Server - up mep.*/
                    uint32 system_port;
                    endpoint_info->tx_gport = classifier_mep_entry.port;
                    rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get(unit, mep_db_entry.local_port, &system_port);
                    BCMDNX_IF_ERR_EXIT(rv);
                    BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->remote_gport, system_port);
                    endpoint_info->flags |= BCM_OAM_ENDPOINT_UP_FACING;
                } else if (classifier_mep_entry.remote_gport != BCM_GPORT_INVALID) {
                    /*Server: down MEP*/
                    endpoint_info->remote_gport = classifier_mep_entry.remote_gport;
                }
            }

            rv = _bcm_oam_mep_db_entry_struct_get(unit, endpoint_info, &mep_db_entry, classifier_mep_entry.dip_profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);
            endpoint_info->src_mac_address[5] = classifier_mep_entry.src_mac_lsb;

            if ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER) != 0) {
                /* the tx gport is a TRUNK. override what was written in entry_struct_get()*/
                BCM_GPORT_TRUNK_SET(endpoint_info->tx_gport, (0x3FFF & mep_db_entry.system_port));
            }
            if (SOC_IS_ARADPLUS(unit)) {
                /* get the 2 RDI flags from the MEP professor.*/
                SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY  mep_prof;
                uint8 rdi_from_rx, rdi_from_scannner;
                int dont_care;
                rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, endpoint_info->id, &dont_care, &mep_prof);
                BCMDNX_IF_ERR_EXIT(rv);

                _BCM_OAM_GET_RDI_STATUS_FROM_ETH1731_MEP_PROF_RDI_FIELD(mep_prof.rdi_gen_method, rdi_from_rx, rdi_from_scannner);
                endpoint_info->flags2 |= rdi_from_rx ? 0 : BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE;
                endpoint_info->flags2 |= rdi_from_scannner ? 0 : BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE;
            }
        }
    }

    BCM_EXIT;
exit:
    /* if the use_hw_id soc property is enabled, then the trap_id is the SW id and thus we need to convert it to the HW id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP(endpoint_info->remote_gport))
    {
        _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit, endpoint_info->remote_gport);
    }
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_endpoint_destroy (int unit, bcm_oam_endpoint_t endpoint) {

    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv;
    _bcm_oam_ma_name_t ma_name_struct;
    ENDPOINT_LIST_PTR rmep_list_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR rmep_iter_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR member_next ;
    bcm_oam_endpoint_t current_endpoint;
    uint8 found;
    uint8 deallocate_icc_ndx = FALSE;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data;
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data_acc;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    int is_last, prof_ndx;
    uint32 rmep_id_internal;
    int endpoint_ref_counter;
    uint8 oam_is_init;
    uint8 is_passive;
    uint8 is_default;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
    uint8 is_mip_dummy;
    int profile;
    uint32 profile_u32, profile_dummy;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    is_default = _BCM_OAM_IS_MEP_ID_DEFAULT(unit, endpoint);

    if ((endpoint < 0) && !is_default) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Endpoint id must be positive or default. Given id: %d\n"), endpoint));
    }

    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&mep_info);

    /* Default OAM trap */
    if (is_default) {
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                    (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
            }

            if (mep_info.non_acc_profile != SOC_PPD_OAM_PROFILE_DEFAULT) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                    (_BSL_BCM_MSG("Error: Internal error in default profile configuration.\n")));
            }
            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_free(unit, mep_info.non_acc_profile, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);

            SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data);

            soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &mep_info, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, -1);
            BCMDNX_IF_ERR_EXIT(rv);

        }
        else if (SOC_IS_ARADPLUS(unit)) {

            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
            }

            /* Free profile */
            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, endpoint, 0 /*is_passive*/, &profile_data);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_free(unit, mep_info.non_acc_profile, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_last) {
                rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data.mep_profile_data, &mep_info, 1/*is_mep*/);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data.mip_profile_data, &mep_info, 0/*is_mep*/);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            rv = _bcm_oam_default_id_from_ep_id(unit, endpoint, &default_id);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_remove,
                                                                                    (unit, default_id));
            BCMDNX_IF_ERR_EXIT(rv);

            /* remove from mep info database */
            rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else { /* Destroy non-default endpoint */

        if (_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint)) {
            /* remote endpoint */
            SOC_PPD_OAM_RMEP_INFO_DATA rmep_info;

            /*update sw db of mep ->rmep: delete rmep; delete entry in rmep->mep db */
            SOC_PPD_OAM_RMEP_INFO_DATA_clear(&rmep_info);
            rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, endpoint, &rmep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                    (_BSL_BCM_MSG("Error: Remote endpoint %d not found.\n"), endpoint));
            }

            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, rmep_info.mep_index, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint);

            rv = _bcm_dpp_oam_bfd_rmep_db_entry_dealloc(unit, rmep_id_internal);
            BCMDNX_IF_ERR_EXIT(rv);

            soc_sand_rv = soc_ppd_oam_oamp_rmep_delete(unit, rmep_id_internal, rmep_info.rmep_id, rmep_info.mep_index, mep_info.mep_type);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_rmep_delete(unit, rmep_info.mep_index, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_dpp_oam_bfd_rmep_info_db_delete(unit, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = bcm_dpp_am_oam_rmep_id_dealloc(unit, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);

            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "OAM: Remote endpoint with id %d deleted\n"),
                                 endpoint));
        }

        /* local endpoint */
        else {
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
            }


            /*update sw db of ma->mep : delete mep */
            rv = _bcm_dpp_oam_ma_to_mep_db_mep_delete(unit, mep_info.ma_index, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);

            /*TX - OAMP clear */

            /*delete entry in mep db*/
            if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&mep_info)) {
                if (SOC_IS_ARADPLUS(unit)) {
                    bcm_oam_loss_t loss_obj;
                    bcm_oam_delay_t delay_obj;
                    int was_removed = 0;

                    LOG_DEBUG(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM: Attempting to delete loss/delay entries for endpoint %d\n"), endpoint));

                    bcm_oam_delay_t_init(&delay_obj);
                    bcm_oam_loss_t_init(&loss_obj);
                    loss_obj.id = endpoint;
                    rv = bcm_petra_oam_loss_delete(unit, &loss_obj);
                    if (rv != BCM_E_NOT_FOUND) {
                        BCMDNX_IF_ERR_EXIT(rv);
                        was_removed = 1;
                    }
                    delay_obj.id = endpoint;
                    rv = bcm_petra_oam_delay_delete(unit, &delay_obj);
                    if (rv != BCM_E_NOT_FOUND) {
                        BCMDNX_IF_ERR_EXIT(rv);
                        was_removed = 1;
                    }
                    if (mep_info.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) {
                        bcm_oam_loopback_t loopback = { 0 };
                        loopback.id = endpoint;
                        rv = bcm_petra_oam_loopback_delete(unit, &loopback);
                        BCMDNX_IF_ERR_EXIT(rv);
                        was_removed = 1;
                    }

                    if (was_removed) {
                        /* The mep_info may have been changed.*/
                        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &mep_info, &found);
                        BCMDNX_IF_ERR_EXIT(rv);
                        if (!found) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
                        }
                    }

                    if (SOC_IS_JERICHO(unit)) {
                        bcm_oam_ais_t ais;
                        int dont_care[1];
                        uint8 found;
                        ais.id = endpoint;
                        rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(unit, endpoint, dont_care, &found);
                        BCMDNX_IF_ERR_EXIT(rv);
                        if (found) {
                            rv = bcm_oam_ais_delete(unit, &ais);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }

                        if (mep_info.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER) {
                            bcm_oam_loopback_t loopback = { 0 };
                            loopback.id = endpoint;
                            rv = bcm_petra_oam_loopback_delete(unit, &loopback);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                        if (mep_info.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER) {
                            bcm_oam_tst_tx_t tst_tx = { 0 };
                            tst_tx.endpoint_id = endpoint;
                            rv = bcm_petra_oam_tst_tx_delete(unit, &tst_tx);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                        if (mep_info.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER) {
                            bcm_oam_tst_rx_t tst_rx = { 0 };
                            tst_rx.endpoint_id = endpoint;
                            rv = bcm_petra_oam_tst_rx_delete(unit, &tst_rx);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    }

                    /*free the mep profile entry.*/
                    rv =  _bcm_dpp_am_template_oam_eth1731_mep_profile_free(unit, endpoint,  &prof_ndx, &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);
                    CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last, prof_ndx, endpoint);


                }
                /*update sw db of mep->rmep : delete all rmeps */
                rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint, &rmep_list_p, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (found)
                {
                    /* this mep has rmeps */
                    /*Loop on all RMEPs associated with each MEP*/
                    rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, rmep_list_p, &rmep_iter_p) ;
                    BCMDNX_IF_ERR_EXIT(rv);

                    while (rmep_iter_p != 0)
                    {
                        uint32 member_index ;

                        rv = _bcm_dpp_oam_endpoint_list_get_member_index(unit, rmep_iter_p, &member_index) ;
                        BCMDNX_IF_ERR_EXIT(rv);
                        current_endpoint = (bcm_oam_endpoint_t)member_index ;
                        rv = _bcm_dpp_oam_endpoint_list_get_member_next(unit, rmep_iter_p, &member_next) ;
                        BCMDNX_IF_ERR_EXIT(rv);
                        rmep_iter_p = member_next ;
                        rv = bcm_petra_oam_endpoint_destroy(unit, current_endpoint);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }

                rv = _bcm_dpp_oam_ma_db_get(unit, mep_info.ma_index, &ma_name_struct, &found);
                BCMDNX_IF_ERR_EXIT(rv);

                /*get entry of mep db*/
                soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint, &mep_db_entry);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (ma_name_struct.name_type != _BCM_OAM_MA_NAME_TYPE_SHORT) {

                    /*Deallocate the pointer to ICCMap register*/

                    /* The MBCM_DPP_DRIVER_CALL initializing the required varible */
                    /* coverity[uninit_use_in_call:FALSE] */
                    rv = _bcm_dpp_am_template_oam_icc_map_ndx_free(unit, mep_db_entry.icc_ndx, &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (is_last) {
                        deallocate_icc_ndx = TRUE;
                    }
                }

                rv = _bcm_oam_mep_db_entry_dealloc(unit,endpoint,&mep_db_entry,&mep_info);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = OAM_ACCESS.acc_ref_counter.get(unit, (uint32 *)&endpoint_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
                endpoint_ref_counter--;
                rv =  OAM_ACCESS.acc_ref_counter.set(unit, (uint32)endpoint_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);


                if (BCM_GPORT_INVALID != mep_info.remote_gport && (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == 0) {
                    /* Server, down MEP. Free the trap.*/
                    uint32 server_trap_ref_counter;
                    rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.get(unit, BCM_GPORT_TRAP_GET_ID(mep_info.remote_gport), &server_trap_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                    server_trap_ref_counter--;
                    rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.set(unit, BCM_GPORT_TRAP_GET_ID(mep_info.remote_gport), server_trap_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (!server_trap_ref_counter) {
                        soc_sand_rv = soc_ppd_oam_oamp_rx_trap_codes_delete(unit, mep_info.mep_type, BCM_GPORT_TRAP_GET_ID(mep_info.remote_gport));
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }

                soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_delete(unit, endpoint, &mep_db_entry, deallocate_icc_ndx, (endpoint_ref_counter == 0));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, endpoint);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /*RX - classifier*/

            /* Free profiles  - Only if RX is set*/
            if (mep_info.lif != _BCM_OAM_INVALID_LIF) {

                /* accelerated profile */
            if ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)
                  && !_BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(&mep_info))
                || _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&mep_info)
                || _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint)) {
                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "Freeing accelerated profile for MEP %d\n"),endpoint));
                    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, endpoint, &profile_data_acc);
                    BCMDNX_IF_ERR_EXIT(rv);

                    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_free(unit, mep_info.acc_profile, &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (is_last) {
                        rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data_acc.mep_profile_data, &mep_info, 1/*is_mep*/);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }

                    /* for accelerated end point remove the entry from the Lem used for statistics*/
                    if (SOC_DPP_CONFIG(unit)->pp.oam_statistics == 1 && _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info)) {
                        
                         ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
                         SOC_SAND_SUCCESS_FAILURE success;
                         ARAD_PP_FP_QUAL_VAL_clear(qual_vals);
                        
                         qual_vals[0].val.arr[0] = mep_info.lif;
                         qual_vals[0].val.arr[1] = 0;
                         qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
                         qual_vals[1].val.arr[0] = SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM;
                         qual_vals[1].val.arr[1] = 0;
                         qual_vals[1].type = SOC_PPC_FP_QUAL_OAM_OPCODE;
                         qual_vals[2].val.arr[0] = ((mep_info.md_level)*2);
                         qual_vals[2].val.arr[1] = 0;
                         qual_vals[2].type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL;

                         rv = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS, qual_vals , &success);
                         BCMDNX_IF_ERR_EXIT(rv);
                     }
                }

                /* non-accelerated profile */
                for (is_passive = 0; is_passive <= 1; is_passive++) {

                    /* In Arad Mode Passive profile is fixed and configured separately */
                    if (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && is_passive) {
                        continue;
                    }

                    rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, endpoint, is_passive, &profile_data);
                    BCMDNX_IF_ERR_EXIT(rv);

                    rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_free(unit, (is_passive ? mep_info.non_acc_profile_passive : mep_info.non_acc_profile), &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (is_last) {
                        rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data.mep_profile_data, &mep_info, 1/*is_mep*/);
                        BCMDNX_IF_ERR_EXIT(rv);
                        rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data.mip_profile_data, &mep_info, 0/*is_mep*/);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }

                soc_sand_rv = soc_ppd_oam_classifier_mep_delete(unit, endpoint, &mep_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* Find if there are other meps on same lif */
                _BCM_OAM_MEP_EXIST_ON_LIF_AND_DIRECTION(mep_info.lif, _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info)
                                                        , 0/*find_mip*/, found, is_mip_dummy, profile_dummy);

                if (!found) { /* Serch meps on the other directions */
                    _BCM_OAM_MEP_EXIST_ON_LIF_AND_DIRECTION(mep_info.passive_side_lif, (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info))
                                                            , 0/*find_mip*/, found, is_mip_dummy, profile_dummy);
                }

                if (!found) { /* No more meps on this lif */
                    rv = _bcm_dpp_oam_lif_table_set(unit, _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info)? mep_info.passive_side_lif : mep_info.lif, 0);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && !SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
                    /* If this is the last MIP on this Lif, change profile to active_passive and add passive endpoint to passive bitmap */
                    if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(&mep_info)) {

                        /* Check if no more MIPs left on lif, direction (up/down) where looking for the MIP doesnt make difference */
                        _BCM_OAM_MEP_EXIST_ON_LIF_AND_DIRECTION(mep_info.lif, 0/*is_upmep*/, 1/*find_mip*/, found, is_mip_dummy, profile_u32);

                        if (!found) {
                            bcm_oam_endpoint_traverse_cb_profile_info_t profile_info;
                            SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry_dummy;
                            SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry_dummy);

                            /* If no more MIPs, we must have at least one MEP on any of the sides. */
                            /* If we have more then one they must be on the same side. */
                            /* Need to find which is the active side. */
                            _BCM_OAM_MEP_EXIST_ON_LIF_AND_DIRECTION(mep_info.lif, 0/*is_upmep*/, 0/*find_mip*/, found, is_mip_dummy, profile_u32);
                            if (!found) {
                                /* Remaining MEPs on the LIF are up directed. */
                                /* Finding the active profile */
                                _BCM_OAM_MEP_EXIST_ON_LIF_AND_DIRECTION(mep_info.lif, 1/*is_upmep*/, 0/*find_mip*/, found, is_mip_dummy, profile_u32);
                            }

                            /* Existing endpoints must return to active_passive mp_type */
                            for (is_passive = 0; is_passive<=1; is_passive++) {
                                if (!is_passive) {
                                    /* If active - fill active side on which MEP/s already exist, take profile from existing endpoint */
                                    profile = (int)profile_u32;
                                    rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, &profile_data);
                                    BCMDNX_IF_ERR_EXIT(rv);
                                    profile_data.mp_type_passive_active_mix = 1;

                                    /* Find first endpoint on this Lif */
                                    profile_info.first_endpoint_id = BCM_OAM_ENDPOINT_INVALID;
                                    profile_info.lif = mep_info.lif;
                                    profile_info.profile_data = NULL;
                                    /* Iterate over all groups to find all the endpoints on same lif to find the first */
                                    rv = bcm_petra_oam_group_traverse(unit, _bcm_oam_endpoint_exchange_profile_group_cb, &profile_info);
                                    BCMDNX_IF_ERR_EXIT(rv);

                                    /* get the existing endpoint */
                                    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, profile_info.first_endpoint_id, &classifier_mep_entry_dummy, &found);
                                    BCMDNX_IF_ERR_EXIT(rv);
                                }
                                else {
                                    /* Passive side - allocate NEW profile for passive side. mp_type_passive_active_mix will be set to 1 */
                                    rv = _bcm_oam_default_profile_get(unit, &profile_data, &classifier_mep_entry_dummy, 0 /* non accelerated */, 0 /* is default */, 
                                                                      (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry_dummy)));
                                    BCMDNX_IF_ERR_EXIT(rv);
                                }

                                /* Move all existing endpoints on this lif to the new active profile (only SW DB) */
                                profile_info.first_endpoint_id = BCM_OAM_ENDPOINT_INVALID;
                                profile_info.lif = mep_info.lif;
                                profile_info.profile_data = &profile_data;
                                profile_info.is_ingress = (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry_dummy)) ^ is_passive;
                                /* Iterate over all groups to find all the endpoints on same lif and switch their profile */
                                rv = bcm_petra_oam_group_traverse(unit, _bcm_oam_endpoint_exchange_profile_group_cb, &profile_info);
                                BCMDNX_IF_ERR_EXIT(rv);
                                
                                /* get the existing endpoint on active side */
                                rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, profile_info.first_endpoint_id, &classifier_mep_entry_dummy, &found);
                                BCMDNX_IF_ERR_EXIT(rv);

                                if (!found) {
                                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                                     (_BSL_BCM_MSG("Error: Internal error, endpoint not found. \n")));
                                }

                                if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry_dummy)) {
                                    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, profile_info.first_endpoint_id, &profile_data_acc);
                                    BCMDNX_IF_ERR_EXIT(rv);
                                }

                                /* Update HW */
                                soc_sand_rv = soc_ppd_oam_mep_passive_active_enable_set(unit, 
                                                                                        is_passive ? classifier_mep_entry_dummy.non_acc_profile_passive : classifier_mep_entry_dummy.non_acc_profile,
                                                                                        1/*enable active_passive_mix*/);
                                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                                soc_sand_rv = soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile(
                                   unit, 0, &classifier_mep_entry_dummy, &profile_data, is_passive ? NULL : &profile_data_acc);
                                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                            }

                            /* Update HW - OEM1 table*/
                            soc_sand_rv = soc_ppd_oam_classifier_oem_mep_profile_replace(unit, &classifier_mep_entry_dummy, 1 /*update mp-type*/);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                    }
                }
            }

            /* remove from mep info database */
            rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);

            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "Log: Local endpoint with id %d deleted\n"), endpoint));

        }
    }

    

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



/* Set OAM endpoint opcode actions */
int bcm_petra_oam_endpoint_action_set_internal(
    int unit, 
    bcm_oam_endpoint_t id, 
    bcm_oam_endpoint_action_t *action,
    int is_piggy_backed) {

    SOC_PPD_OAM_LIF_PROFILE_DATA * profile_data_pointer;
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data, old_profile_data;
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data_acc, old_profile_data_acc;
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data_passive, old_profile_data_passive;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   classifier_mep_entry;
    int opcode, profile,action_option;
    uint8 internal_opcode;
    int is_allocated, old_profile, old_profile_acc, is_last;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv = SOC_SAND_OK;
    uint32 trap_code=0;
    uint8 found;
    uint8 oam_is_init;
    uint8 profile_change = 0;
    uint8 is_accelerated;
    uint8 is_mip = 0;
    uint8 is_passive;
    uint8 is_default;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
    bcm_oam_endpoint_traverse_cb_profile_info_t profile_info; 
    int trap_id_sw;
    int trap_code_converted;
    bcm_oam_endpoint_action_t action_lcl_t;
    bcm_oam_endpoint_action_t *action_lcl;
    BCMDNX_INIT_FUNC_DEFS;

    action_lcl = &action_lcl_t; /* If the GPORT contains a trap, since the trap id inside the GPORT can be either the HW id or the SW_id ( according to the use_hw_id SOC peroperty) , and this function 
    assumes that the trap id is the SW id, it will use a copy of the action with the SW id */
    sal_memcpy(action_lcl, action, sizeof(bcm_oam_endpoint_action_t));   /* from here till the end of the function, the function uses the copy of the action */

    /* if the use_hw_id soc property is enabled, then the trap_id is the HW id and thus we need to convert it to the sw id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP(action_lcl->destination))
    {
        _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, action_lcl->destination);
    }

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    is_default = _BCM_OAM_IS_MEP_ID_DEFAULT(unit, id);
    if (is_default) {
        rv = _bcm_oam_default_id_from_ep_id(unit, id, &default_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if ((id < 0) && !is_default) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Endpoint id must be positive or default. Given id: %d\n"), id));
    }

    if ((BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionSLMEnable)) && (!SOC_IS_ARADPLUS(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("SLM is available only in Arad+.")));
    }

    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);


    /*verify that the opcode is  amongst the supported options. */
    for (action_option  = 0; action_option < bcmOAMActionCount; ++action_option) {
        if (action_option != bcmOAMActionMcFwd && action_option != bcmOAMActionUcFwd && action_option != bcmOAMActionMeterEnable
            && action_option != bcmOAMActionCountEnable && action_option != bcmOAMActionMcDrop && action_option != bcmOAMActionUcDrop
            && action_option != bcmOAMActionSLMEnable && action_option != bcmOAMActionUcFwdAsData && action_option != bcmOAMActionMcFwdAsData) {
            /* such an opcode is not allowed! verify that it isn't set*/
            if (BCM_OAM_ACTION_GET(*action_lcl, action_option)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported action: %d. Only bcmOAMActionMcFwd, bcmOAMActionUcFwd, bcmOAMActionMeterEnable,"
                                                        "bcmOAMActionMcFwdAsData, bcmOAMActionUcFwdAsData and bcmOAMActionCountEnable supported.\n"), action_option));
            }
        }
    }

    if (BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionSLMEnable) && (BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLM)==0 &&
                   BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLR)==0 ) ) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: bcmOAMActionSLMEnable may only be set with the SLM/R opcodes. ")));
    }

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Mep id %d does not exist.\n"), id));
    }

    if (classifier_mep_entry.mep_type !=SOC_PPD_OAM_MEP_TYPE_ETH_OAM && BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionUcFwd)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: for types other than Ethernet OAM select multicast.\n")));
    }

    is_mip = _BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(&classifier_mep_entry);

    if (BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionSLMEnable) && is_mip) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: SLM action is not supported for MIPs.\n")));
    }

	/* All opcodes include meta data.*/

	is_accelerated = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)
                        || _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(id); /* Classifier entries in the latter case act as accelerated entries.*/
	/* Get existing profile */
	SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data);
	rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, id, 0/*is_passive*/, &profile_data);
    BCMDNX_IF_ERR_EXIT(rv);
	SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&old_profile_data);
	rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, id,  0/*is_passive*/, &old_profile_data);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_mip) {
        /* 
         * In case of MIP, both up and down profiles should be updated.
         * In case of MEP, only the active profile should be updated.
         */
        SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data_passive);
        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, id, 1/*is_passive*/, &profile_data_passive);
        BCMDNX_IF_ERR_EXIT(rv);
        SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&old_profile_data);
        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, id,  1/*is_passive*/, &old_profile_data_passive);
        BCMDNX_IF_ERR_EXIT(rv);
    }

	SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data_acc);
    /* New Arad+ Classifier: in the new implementation this should always be done because OEM2 has entry for non-accelerated endpoints as well */
	if (_BCM_OAM_DISSECT_CLASSIFICATION_BY_OAM2(unit,&classifier_mep_entry,id) && !is_default) {
		rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, id, &profile_data_acc);
		BCMDNX_IF_ERR_EXIT(rv);
		SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&old_profile_data_acc);
		rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, id, &old_profile_data_acc);
		BCMDNX_IF_ERR_EXIT(rv);
	}

    if (action_lcl->destination != BCM_GPORT_INVALID) {
        if (BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionUcDrop) || BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionMcDrop) ||
            BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionUcFwdAsData) || BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionMcFwdAsData)) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Drop Fwd and as data actions supports only invalid gport in destination field.\n")));
        }
		rv = _bcm_dpp_oam_bfd_trap_code_from_remote_gport(unit, action_lcl->destination, &trap_code, 1 /* includes meta-data */, &classifier_mep_entry);
		BCMDNX_IF_ERR_EXIT(rv);

		/* Set return value trap code as gport */
		if ( soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)
             && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            BCM_GPORT_TRAP_SET(action_lcl->rx_trap, SOC_PPD_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH, 7, 0);
        } else if (!BCM_GPORT_IS_TRAP(action_lcl->destination)  && !SOC_PPD_TRAP_CODE_OAM_IS_FTMH_MIRROR(trap_code)){                                                                                                              
			BCM_GPORT_TRAP_SET(action_lcl->rx_trap, trap_code, 7, 0);
        }

    }
    else {
        if (BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionUcFwd) || BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionMcFwd)) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Fwd action supports only valid gport in destination field.\n")));
        }
    }

	/* Fill new profile */
    for (opcode = 0; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
         if (BCM_OAM_OPCODE_GET(*action_lcl, opcode)) {
             /* Translate to internal event */
             rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcode, &internal_opcode);
    		 BCMDNX_IF_ERR_EXIT(rv);
             LOG_DEBUG(BSL_LS_BCM_OAM,
                       (BSL_META_U(unit,
                                   "\n------------------------------ trap code is %d, internal trap code: %d, opcode: %d\n"), trap_code,
                        internal_opcode, opcode)); 

			 if (!is_mip) {
				 if (_BCM_OAM_DISSECT_CLASSIFICATION_BY_OAM2(unit,&classifier_mep_entry,id) && !is_default) {
					 _BCM_OAM_PROFILE_SINGLE_OPCODE_SET(profile_data_acc.mep_profile_data, internal_opcode, action_lcl, trap_code);
				 }
				 else { /* Modify only to non-acc profile */
					 _BCM_OAM_PROFILE_SINGLE_OPCODE_SET(profile_data.mep_profile_data, internal_opcode, action_lcl, trap_code);
				 }
			 }
			 else { /*MIP*/
                 /* In case of MIP passive profile should be modified as well */
				 _BCM_OAM_PROFILE_SINGLE_OPCODE_SET(profile_data.mip_profile_data, internal_opcode, action_lcl, trap_code);
				 _BCM_OAM_PROFILE_SINGLE_OPCODE_SET(profile_data_passive.mip_profile_data, internal_opcode, action_lcl, trap_code);
			 }
         }
    }

    /* SLM support - set profile */
    profile_data.is_slm = BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionSLMEnable) ? 1 : 0;

    if (is_accelerated && BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM)) {
        profile_data_acc.is_piggybacked = is_piggy_backed;
    }

    /* For MIP - update passive profile as well */
    for (is_passive=0; is_passive<=is_mip; is_passive++) {

        profile_data_pointer = is_passive ? &profile_data_passive : &profile_data;

        /* get the old profile */
        old_profile = is_passive ? classifier_mep_entry.non_acc_profile_passive : classifier_mep_entry.non_acc_profile;

        /* Move all existing endpoints on this lif to the new profile */
        profile_info.first_endpoint_id = BCM_OAM_ENDPOINT_INVALID;
        profile_info.lif = is_passive? classifier_mep_entry.passive_side_lif: classifier_mep_entry.lif;
        profile_info.profile_data = profile_data_pointer;
        profile_info.is_ingress = (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry)) ^ is_passive;
        profile_info.is_alloced = profile_info.is_last = 0;  
        /* Iterate over all groups to find all the endpoints on same lif and switch their profile */
        rv = bcm_petra_oam_group_traverse(unit, _bcm_oam_endpoint_exchange_profile_group_cb, &profile_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* get the new profile through reading it from the SW. It might have been changed inside the callback */
		rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, id, &classifier_mep_entry/*_dummy*/, &found);
		BCMDNX_IF_ERR_EXIT(rv);
		if (!found) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Internal Error: Mep id %d does not exist.\n"), id));
		}

        profile = is_passive ? classifier_mep_entry/*_dummy*/.non_acc_profile_passive : classifier_mep_entry/*_dummy*/.non_acc_profile;

        /* SLM support - disable/enable data counting */
        if (SOC_IS_ARADPLUS(unit)) {
            /* SLM support - disable data counting */
            if (profile_data.is_slm) {
                soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, profile, 1);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {
                soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, profile, 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            if ( BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLM) ||
                   BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLR)) {
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_slm_set, 
                                                  (unit, profile_data.is_slm));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
            }

            if (!is_mip && 
                (BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM) ||
                 BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR)|| 
                  BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLM) ||
                   BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLR))) {
                /* configure whether SLM/LMM and related packets are counted*/
                /* By default count none. If one is set to count then count all of them.*/
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_counter_increment_bitmap_set, 
                                                  (unit,BCM_OAM_ACTION_GET(*action, bcmOAMActionCountEnable)));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
            }




        }

        
        if (old_profile != profile) {
            profile_change = 1;

            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                /* Set HW passive_active for the new allocated profile */
                soc_sand_rv = soc_ppd_oam_mep_passive_active_enable_set(unit, profile, profile_data_pointer->mp_type_passive_active_mix);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }

        if (profile_info.is_last) {
            /* SLM support - enable data counting */
            if (SOC_IS_ARADPLUS(unit) && profile_change) {
                soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, old_profile, 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }

        /* Accelerated profile - relevant only for active side */
        /* New Arad+ Classifier: in the new implementation this should always be done */
        if (!is_passive) {
            if (_BCM_OAM_DISSECT_CLASSIFICATION_BY_OAM2(unit,&classifier_mep_entry,id) && !is_default) {
            rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_exchange(unit, 0/*flags*/, id, &profile_data_acc,
                                                         &old_profile_acc, &is_last, &profile, &is_allocated);
                BCMDNX_IF_ERR_EXIT(rv);
                if (classifier_mep_entry.acc_profile != profile) {
                    classifier_mep_entry.acc_profile = profile;
                    profile_change = 1;


					rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, id, &classifier_mep_entry);
					BCMDNX_IF_ERR_EXIT(rv);
                }

                if (is_allocated) {
                    rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data_acc.mep_profile_data);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                if (is_last) { 
                    rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &old_profile_data_acc.mep_profile_data, &classifier_mep_entry, 1/*is_mep*/);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }

		/* Insert into classifier OAM tables */
        if (is_default) {
            if (SOC_IS_ARADPLUS(unit) && (old_profile != classifier_mep_entry.non_acc_profile)) {
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_add,
                                                  (unit, default_id,&classifier_mep_entry, 1/*update_action_only*/));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, profile_data_pointer, &classifier_mep_entry, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            soc_sand_rv = soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile(
               unit,
               SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(id),
                &classifier_mep_entry, profile_data_pointer, 
               (is_passive ? NULL : &profile_data_acc));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    /* Insert into classifier OEM tables */
    if (profile_change) {
		if (id != -1) {
			soc_sand_rv = soc_ppd_oam_classifier_oem_mep_profile_replace(unit, &classifier_mep_entry, 0 /*update mp type*/);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
		}
    }

	BCM_EXIT;
exit:
     sal_memcpy(action, action_lcl, sizeof(bcm_oam_endpoint_action_t));
    /* if the use_hw_id soc property is enabled, then the trap_id is the SW id and thus we need to convert it to the HW id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP(action->destination))
    {
        _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit,action->destination);
    }
	if (SOC_SAND_FAILURE(soc_sand_rv) || (BCM_FAILURE(rv))) {
		/* Clear allocated trap codes */
		if (trap_code != 0) {

            trap_id_sw = trap_code; 
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
			rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
            if(BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "bcm_petra_rx_trap_type_destroy failed: %s\n"),
                           bcm_errmsg(rv)));
            }
			if (is_mip) {
				rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted+1);
                if(BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "bcm_petra_rx_trap_type_destroy failed: %s\n"),
                               bcm_errmsg(rv)));
                }
			}
		}
	}
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_oam_endpoint_action_set(
    int unit, 
    bcm_oam_endpoint_t id, 
    bcm_oam_endpoint_action_t *action) {


    return bcm_petra_oam_endpoint_action_set_internal(unit,id,action,0);
}


int bcm_petra_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group)
{

    bcm_oam_endpoint_t current_endpoint;
    ENDPOINT_LIST_PTR mep_list_p ;
    ENDPOINT_LIST_MEMBER_PTR mep_iter_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR member_next ;
    uint8 found;
    int rv;
    uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    if (group < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Group id must be positive. Given id: %d\n"), group));
    }

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: All endpoints of group with id %d deleted\n"),
                     group));

    rv = _bcm_dpp_oam_ma_to_mep_db_get(unit, group, &mep_list_p, &found);
    BCMDNX_IF_ERR_EXIT(rv); 

    if (found)
    {
        /*Loop on all the MEPs of this MA*/
        rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, mep_list_p, &mep_iter_p) ;
        BCMDNX_IF_ERR_EXIT(rv);

        while (mep_iter_p != 0)
        {
            uint32 member_index ;

            rv = _bcm_dpp_oam_endpoint_list_get_member_index(unit, mep_iter_p, &member_index) ;
            BCMDNX_IF_ERR_EXIT(rv);
            current_endpoint = (bcm_oam_endpoint_t)member_index ;
            rv = _bcm_dpp_oam_endpoint_list_get_member_next(unit, mep_iter_p, &member_next) ;
            BCMDNX_IF_ERR_EXIT(rv);
            mep_iter_p = member_next ;

            rv = bcm_petra_oam_endpoint_destroy(unit, current_endpoint);
            BCMDNX_IF_ERR_EXIT(rv); 
        } 
    }
    else {
        LOG_WARN(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     " OAM Warning: Group %d has no endpoints.\n"), group));
    }

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: All endpoints of group with id %d deleted\n"), group));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Destroy an OAM group object.  All OAM endpoints associated with the
 * group will also be destroyed.
 */
int bcm_petra_oam_group_destroy(
    int unit, 
    bcm_oam_group_t group) {

    int rv;
    _bcm_oam_ma_name_t ma_name;
    uint8 found;
    uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;
	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}
	if (group < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Group id must be positive. Given id: %d\n"), group));
	}

	rv = _bcm_dpp_oam_ma_db_get(unit, group, &ma_name, &found);
	BCMDNX_IF_ERR_EXIT(rv); 
	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
			(_BSL_BCM_MSG("Error: Group %d not found.\n"), group));
	}

	rv = bcm_petra_oam_endpoint_destroy_all(unit, group);
	BCMDNX_IF_ERR_EXIT(rv); 

	/*delete MA name from SW db*/
	rv = _bcm_dpp_oam_ma_db_delete(unit, group);
	BCMDNX_IF_ERR_EXIT(rv); 

	rv = bcm_dpp_am_oam_ma_id_dealloc(unit, group);
	BCMDNX_IF_ERR_EXIT(rv);

	LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: Group with id %d deleted\n"),
                     group));

	BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function traverses the entire set of a OAM group, calling a given callbeack for each one. 
 */
int bcm_petra_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb, void* user_data ) {
    int rv;
    uint8 oam_is_init;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    /*filling the global data*/
    
    _group_cb_with_param.cb = cb;
    _group_cb_with_param.user_data = user_data;

    /*from here the iterate function takes over.*/
    rv = _bcm_dpp_sw_db_hash_oam_ma_name_iterate(unit,_bcm_petra_oam_group_traverse_cb);
       
    BCMDNX_IF_ERR_EXIT(rv);
        

	BCM_EXIT;
exit:
	BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_endpoint_traverse(int unit, bcm_oam_group_t group,bcm_oam_endpoint_traverse_cb cb,void *user_data)
{  
    int rv;
    ENDPOINT_LIST_PTR mep_list_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR mep_iter_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR member_next ;
    uint8 found;
    bcm_oam_endpoint_info_t current_endpoint;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_ma_to_mep_db_get(unit,group,&mep_list_p, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (found)
    {
        /*Go over the endpoints in mep_list_p, calling the user's callback on each one.*/
        rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, mep_list_p, &mep_iter_p) ;
        BCMDNX_IF_ERR_EXIT(rv);

        while (mep_iter_p)
        {
            uint32 member_index ;

            bcm_oam_endpoint_info_t_init(&current_endpoint);
            rv = _bcm_dpp_oam_endpoint_list_get_member_index(unit, mep_iter_p, &member_index) ;
            BCMDNX_IF_ERR_EXIT(rv);
            current_endpoint.id = member_index ;

            rv = bcm_petra_oam_endpoint_get(unit,current_endpoint.id, &current_endpoint);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = cb(unit,&current_endpoint, user_data);
            if (rv != 0)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("callback in bcm_oam_endpoint_traverse returned error message %s"),bcm_errmsg(rv)));
            }
            rv = _bcm_dpp_oam_endpoint_list_get_member_next(unit, mep_iter_p, &member_next) ;
            BCMDNX_IF_ERR_EXIT(rv);
            mep_iter_p = member_next ;
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_group_destroy_all(int unit) {

    int rv;
    uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    rv = _bcm_dpp_sw_db_hash_oam_ma_name_iterate(unit, _bcm_dpp_oam_ma_db_delete_ma_cb);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_sw_db_hash_oam_ma_name_destroy(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_sw_db_hash_oam_ma_name_create(unit);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Clear all OAM (used in debug) */
int _bcm_dpp_oam_clear(int unit) {

    int rv = BCM_E_NONE;
    bcm_port_config_t port_config;
    bcm_gport_t ethernet_port;
    uint32 soc_sand_rv;
    int trap_code;
    uint32 local_port;
    bcm_rx_trap_config_t trap_config;
    uint8 mirror_profile;
    uint8 oam_is_init, is_bfd_init;
    int trap_code_with_meta;
    SOC_PPD_PORT pp_port;
    int core = SOC_CORE_INVALID;
    uint32 trap_id_sw;
    int trap_code_converted;
    int i;
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_EXIT;
    } 

    rv = bcm_petra_oam_group_destroy_all(unit);
    BCMDNX_IF_ERR_EXIT(rv);
 
    for (trap_code_with_meta = SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST; 
          trap_code_with_meta<(SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST+SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM); 
          trap_code_with_meta++) {

        trap_id_sw = trap_code_with_meta;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted );
        BCMDNX_IF_ERR_EXIT(rv);
    }

    for (i = 0  ; i< 3; i++) 
    {
        if ( i == 0 ) {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_OAMP, &trap_id_sw);
        } else if ( i == 1 ) {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_MPLS, &trap_id_sw);
        } else {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_PWE, &trap_id_sw);
        }
        BCMDNX_IF_ERR_EXIT(rv);
      
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }
       
    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        rv = _bcm_dpp_oam_dbs_destroy(unit, TRUE/*is_oam*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_CPU, &trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted); 
        BCMDNX_IF_ERR_EXIT(rv);
    }
	
    rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPD_OAM_UPMEP_TRAP_ID_CPU, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
	rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(unit, trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get snoop command */
    bcm_rx_trap_config_t_init(&trap_config);

    rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_SNOOP, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_get(unit, trap_code_converted, &trap_config);
    BCMDNX_IF_ERR_EXIT(rv);

    /*destroy snoop trap id*/
    rv = bcm_petra_rx_snoop_destroy(unit, 0, trap_config.snoop_cmnd);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
    BCMDNX_IF_ERR_EXIT(rv);

    if (soc_property_get(unit, spn_OAM_RCY_PORT, -1)!=-1) {
        rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_RECYCLE, &trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPD_OAM_UPMEP_TRAP_ID_OAMP, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
	rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(unit, trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)
        && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* free all resources used for egress snooping*/
        rv = bcm_petra_rx_snoop_destroy(unit, 0,_BCM_OAM_MIP_SNOOP_CMND_UP);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_rx_snoop_destroy(unit, 0,_BCM_OAM_MIP_SNOOP_CMND_DOWN);
        BCMDNX_IF_ERR_EXIT(rv);

        trap_id_sw = SOC_PPD_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit,trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv); 
    }

#ifdef _BCM_OAM_LOOPBACK_UPMEP_SUPPORTED /*LoopBack upmep is not supported in ARAD*/

    rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPD_OAM_UPMEP_TRAP_ID_RECYCLE, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_type_destroy(unit,trap_code_converted);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /*LoopBack upmep is not supported in ARAD*/

    for (i = 0  ; i< 2; i++) 
    {
        if ( i == 0 ) {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_ERR_LEVEL, &trap_id_sw);
        } else {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_ERR_PASSIVE, &trap_id_sw);
        }
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    /*destroy all the mirror profiles*/
    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPD_OAM_MIRROR_ID_OAMP, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
	_BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPD_OAM_MIRROR_ID_CPU, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
	_BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

#ifdef _BCM_OAM_LOOPBACK_UPMEP_SUPPORTED /* LoopBack upmep not supported - trap to CPU using CPU trap id */
    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPD_OAM_MIRROR_ID_RECYCLE, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
	_BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
#endif

    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPD_OAM_MIRROR_ID_SNOOP, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
	_BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPD_OAM_MIRROR_ID_ERR_LEVEL, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
	_BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPD_OAM_MIRROR_ID_ERR_PASSIVE, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
	_BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* deinit _bcm_oam_cpu_trap_code_to_mirror_profile_map */
    for (trap_code = 1; trap_code < SOC_PPD_NOF_TRAP_CODES; trap_code++) {
        mirror_profile = 0;
        rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_code, mirror_profile);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Get Ethernet ports */
    rv = bcm_petra_port_config_get(unit, &port_config);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Creating mirror profiles for egress functionality */
    BCM_PBMP_ITER(port_config.e, ethernet_port) {
        bcm_petra_mirror_port_dest_delete(unit, ethernet_port, BCM_MIRROR_PORT_EGRESS_ACL, 0);
    }

    for (local_port = 0; local_port < 255; local_port++) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &pp_port, &core)));

        soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    _BCM_OAM_ENABLED_SET(FALSE);

    rv = _bcm_dpp_oamp_interrupts_init(unit, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_oam_deinit(unit, 0/*is_bfd*/, !is_bfd_init);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Internal function: used only by restart procedures.*/
int _bcm_petra_oam_detach(int unit) {

    int rv = BCM_E_NONE;
    uint8 oam_is_init, is_bfd_init;

	BCMDNX_INIT_FUNC_DEFS;
	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_EXIT;
	} 

	_BCM_BFD_ENABLED_GET(is_bfd_init);

	if (!is_bfd_init) {
		rv = _bcm_dpp_oam_dbs_destroy(unit, TRUE/*is_oam*/);
		BCMDNX_IF_ERR_EXIT(rv);
    } else if (SOC_IS_JERICHO(unit)) {
        rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_destroy(unit);
        BCMDNX_IF_ERR_EXIT(rv);
	}

	/* We do not deinit the oam on purpose - it might hurt the sync-WB */
	_BCM_OAM_ENABLED_SET(FALSE);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_opcode_map_set(int unit, int opcode, int profile) {

	int rv = BCM_E_NONE;
    uint8 internal_opcode;
	uint8 oam_is_init;
    uint32 soc_sand_rv;

	BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	} 

    rv = _bcm_petra_oam_opcode_map_set_verify(unit, opcode, profile);
	BCMDNX_IF_ERR_EXIT(rv);

    internal_opcode = (uint8)profile;
	rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.set(unit, opcode, internal_opcode);
	BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_oam_eth_oam_opcode_map_set(unit);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_opcode_map_get(int unit, int opcode, int *profile) {

	int rv = BCM_E_NONE;
    uint8 internal_opcode;
	uint8 oam_is_init;

	BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	} 

    rv = _bcm_petra_oam_opcode_map_get_verify(unit, opcode);
	BCMDNX_IF_ERR_EXIT(rv);

	rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcode, &internal_opcode);
	BCMDNX_IF_ERR_EXIT(rv);
    *profile = (uint32)internal_opcode;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/***************** LM, DM, and more (ARAD+ and above)*/



/**
 * Function manages the troika of profiles eth1731, NIC and OUI.
 * 
 * @author sinai (13/11/2013)
 * 
 * @param unit 
 * @param mep_id 
 * @param classifier_mep 
 * @param msb 
 * @param lsb 
 * @param eth_1731_profile - may be NULL when called by 
 *  					   loopback_add()
 * @param is_update set to 1 if loopback/delay/loss_add() was 
 *  				called with the update flag set
 * @param is_ok returns 1 if all checks were sucessfull, 0 
 *  			otherwise/
 * 
 * @return int 
 */
int _manage_DA_with_existing_entries_internal(int unit , int mep_id ,SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep, uint32 msb, uint32 lsb, 
											  SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY *eth_1731_profile, int is_update, uint8* is_ok, uint8 is_dm) {
	uint8  has_other_entry;
	uint32 soc_sand_rv, three_bytes_from_mac_addr;
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY old_eth_data;
	int only_piggy_back_exists=0, is_last, nic_profile, oui_profile, old_eth1731_profile;
	int rv, is_allocated, profile_indx;
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY *eth1731_profile_to_use= eth_1731_profile? eth_1731_profile: &old_eth_data;
	int (*alloc_function) (int, int,SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY * , int *, int *, int*, int*, int) = eth_1731_profile==NULL?  _bcm_oam_am_template_oam_eth_1731_mep_profile_loopback_alloc:
		(is_dm)? _bcm_dpp_am_template_oam_eth_1731_mep_profile_delay_alloc :
		_bcm_dpp_am_template_oam_eth_1731_mep_profile_loss_alloc;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    uint32 found_bitmap[1];
	int on_demand_dmm=0;


  uint32 user_header_size, user_header_0_size, user_header_1_size;
  uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

	BCMDNX_INIT_FUNC_DEFS;

	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&old_eth_data);

    if (is_dm && (classifier_mep->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE ) && eth_1731_profile &&
		eth_1731_profile->dmm_rate && is_update) {
		/* An on demand service is active for this mep, adding pro-active version. Turn off the flag*/
		classifier_mep->flags &= ~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;
    }

    /*First, see if any other entries are using the profiles*/
    soc_sand_rv = soc_ppd_oam_oamp_search_for_lm_dm(unit, mep_id, found_bitmap);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (eth_1731_profile==NULL) { /* go here only if this function was called by LB.*/
		has_other_entry = *found_bitmap > 0;
    } else {
		/* for LM, search for DM and vice-a-versa*/
        if ((classifier_mep->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY)|| 
			classifier_mep->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE) {
			/* in this case the LB is using the profiles. There definitely was another entry.*/
			has_other_entry=1;
        } else {
			/* There was only another entry if an LM/DM entry was found.*/
			has_other_entry= (is_dm)? ( SHR_BITGET(found_bitmap, SOC_PPD_OAM_MEP_TYPE_LM)  !=0) : SHR_BITGET(found_bitmap, SOC_PPD_OAM_MEP_TYPE_DM) !=0;
		}
        /* Update the offsets*/
        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, mep_id, &mep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

		soc_sand_rv = arad_pmf_db_fes_user_header_sizes_get(
		   unit,
		   &user_header_0_size,
		   &user_header_1_size,
		   &user_header_egress_pmf_offset_0_dummy,
		   &user_header_egress_pmf_offset_1_dummy
		 );
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

		user_header_size = (user_header_0_size + user_header_1_size) / 8; 

        if (is_dm) {
             _OAM_SET_DMM_OFFSET(mep_db_entry, eth_1731_profile->dmm_offset);
			 eth_1731_profile->dmm_offset = eth_1731_profile->dmm_offset + user_header_size;
             _OAM_SET_DMR_OFFSET(mep_db_entry, eth_1731_profile->dmr_offset);
			 eth_1731_profile->dmr_offset = eth_1731_profile->dmr_offset + user_header_size;
        } else {
             _OAM_SET_LMM_OFFSET(mep_db_entry,eth_1731_profile->lmm_offset, eth_1731_profile->piggy_back_lm, eth_1731_profile->slm_lm );
			 eth_1731_profile->lmm_offset = eth_1731_profile->lmm_offset + user_header_size;
             _OAM_SET_LMR_OFFSET(mep_db_entry,eth_1731_profile->lmr_offset,  eth_1731_profile->piggy_back_lm, eth_1731_profile->slm_lm);
			 eth_1731_profile->lmr_offset = eth_1731_profile->lmr_offset + user_header_size;
        }
	}

    rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, mep_id, &old_eth1731_profile, &old_eth_data);
    BCMDNX_IF_ERR_EXIT(rv); 

	/* Next, see if only an LM piggy back entry is using the profiles.*/
    if (has_other_entry) {
        if (old_eth_data.piggy_back_lm && (SHR_BITGET(found_bitmap, SOC_PPD_OAM_MEP_TYPE_DM))==0 &&
			!(classifier_mep->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) ) {
			only_piggy_back_exists =1;
        }

		/* Quick check that that piggy-back, slm status have not been changed through update*/
        if (is_update && eth_1731_profile && !is_dm ) {
			/* LM + update*/
            if (old_eth_data.piggy_back_lm != eth_1731_profile->piggy_back_lm) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Piggy back mode may not be changed.")));
            }
            if (old_eth_data.slm_lm != eth_1731_profile->slm_lm) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("SLM mode may not be changed.")));
            }
        }
    }

	
	/* It may be the case that this has been called for one shot on demand DMM */
    if (is_dm && eth_1731_profile->dmm_rate==0) {
		on_demand_dmm = 1; 
		/* Two cases: on demand on top of proactive or not */
		if ( SHR_BITGET(found_bitmap, SOC_PPD_OAM_MEP_TYPE_DM) ==0 && !(old_eth_data.report_mode && old_eth_data.dmm_rate ) )  {
			/* only on demand mode. turn on the flag.*/
			classifier_mep->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;
		} else {
			has_other_entry=1; /* At the very least there is a proactive dmm entry. Compare against it.*/
			eth_1731_profile->dmm_rate = old_eth_data.dmm_rate; /* So as not to change existing settings.*/
		}
	}




    if (eth_1731_profile!=NULL && (eth_1731_profile->lmm_rate + eth_1731_profile->dmm_rate) && !eth_1731_profile->piggy_back_lm &&
		on_demand_dmm==0 ) {
        /* verify amount of outgoing packets, but not if this function was called by looback_add() , one shot DMM, P-B*/
        SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY actual_transmission ={0}; /* It may be the case that the LM/DM rate is positive but no LMM/DMMs are actually being tranmsitted.
        This object represents the DMMs/LMMs actually transmitted*/
        if (old_eth_data.report_mode) {
			/* When report mode is used the above comment is untrue. In this case the tranmsmission rate stated in the MEP PROFILE is the actual transmission rate.*/
            actual_transmission = old_eth_data;
        } else {
            actual_transmission.opcode_0_rate  = old_eth_data.opcode_0_rate; 
            actual_transmission.opcode_1_rate  = old_eth_data.opcode_1_rate;
            actual_transmission.dmm_rate = ( SHR_BITGET(found_bitmap, SOC_PPD_OAM_MEP_TYPE_DM)  !=0) * old_eth_data.dmm_rate ;
            actual_transmission.lmm_rate = ( SHR_BITGET(found_bitmap, SOC_PPD_OAM_MEP_TYPE_LM)  !=0) * old_eth_data.lmm_rate ;
        }
        _OAM_OAMP_VERIFY_MAXIMAL_TX_PER_MEP(mep_db_entry.ccm_interval,actual_transmission , 
                                            eth_1731_profile->lmm_rate + eth_1731_profile->dmm_rate  /*The one that isn't used should be 0.*/, *eth_1731_profile, is_dm );
    }




	/* Now verify and manage the MAC address profiles.*/
    if (!has_other_entry || only_piggy_back_exists) {
		/* at the very least the DA is available*/
		*is_ok =1;
        if (classifier_mep->mep_type==SOC_PPD_OAM_MEP_TYPE_ETH_OAM &&  !(eth_1731_profile && eth_1731_profile->dmm_rate==0 && eth_1731_profile->piggy_back_lm==1 ) ) {
			/* update the DA registers only in the case of ETH-OAM and non piggy back entry*/
			if (is_update) {
				/* Free existing DA profiles for this MEP.*/
				FREE_NIC_AND_OUI_PROFILES(mep_id);
			}
			/* Now alloc.*/
			rv = _bcm_dpp_am_template_oam_lmm_nic_tables_alloc(unit, 0/* flags*/, &lsb, &is_allocated, &profile_indx);
			BCMDNX_IF_ERR_EXIT(rv);
			nic_profile = profile_indx ;

			rv = _bcm_dpp_am_template_oam_lmm_oui_tables_alloc(unit, 0/* flags*/, &msb, &is_allocated, &profile_indx);
			BCMDNX_IF_ERR_EXIT(rv);
			oui_profile = profile_indx;

			soc_sand_rv = soc_ppd_oam_oamp_set_oui_nic_registers(unit, mep_id, msb, lsb, oui_profile, nic_profile);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
            eth1731_profile_to_use->lmm_da_oui_prof = profile_indx; /* oui_profile may be -1*/
        } else {
			eth1731_profile_to_use->lmm_da_oui_prof =old_eth_data.lmm_da_oui_prof;
		}
	} else {
		/* Someone else is using the profiles. Verify that the DA addresses are OK and update the SW/HW*/
		/*First, verifying */
		if (classifier_mep->mep_type==SOC_PPD_OAM_MEP_TYPE_ETH_OAM && !(eth_1731_profile && eth_1731_profile->dmm_rate==0 && eth_1731_profile->piggy_back_lm==1 ) ) {
			/* update the DA registers only in the case of ETH-OAM and non piggy back entry*/
			rv = _bcm_dpp_am_template_oam_lmm_nic_tables_get(unit,mep_id, &nic_profile, &three_bytes_from_mac_addr);
			BCMDNX_IF_ERR_EXIT(rv);
			if (three_bytes_from_mac_addr != lsb) {
				LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "Cannot insert 0x%x as the LSB of the MAC address. Existing LSB: 0x%x \n"), lsb, three_bytes_from_mac_addr));
				*is_ok = 0;
				goto exit;
			}
			rv = _bcm_dpp_am_template_oam_lmm_oui_tables_get(unit,mep_id, &oui_profile, &three_bytes_from_mac_addr);
			BCMDNX_IF_ERR_EXIT(rv);
			if (three_bytes_from_mac_addr != msb) {
				LOG_ERROR(BSL_LS_BCM_OAM,
                                          (BSL_META_U(unit,
                                                      "Cannot insert 0x%x as the LSB of the MAC address. Existing LSB: 0x%x \n"), lsb, three_bytes_from_mac_addr));
				*is_ok = 0;
				goto exit;
			}
			eth1731_profile_to_use->lmm_da_oui_prof = oui_profile;
			/* Still alive. First update the reference counters for the DA registers...*/
			if (is_update==0) {
				/* ...but only in the case that update hasn't been selected (otherwise we already verified that these are alright.*/
				UPDATE_NIC_AND_OUI_REFERENCE_COUNTER(msb,lsb,nic_profile, oui_profile);
			}
		}
		eth1731_profile_to_use->lmm_da_oui_prof =old_eth_data.lmm_da_oui_prof; /* Use the existing lmm_da_oui profile.*/
		*is_ok = 1;
	}


    /*finally, the eth1731 profile*/
    rv = alloc_function(unit, mep_id, eth1731_profile_to_use, &is_allocated, &profile_indx, &old_eth1731_profile, &is_last, !has_other_entry);
    BCMDNX_IF_ERR_EXIT(rv);
    if (old_eth1731_profile != profile_indx) {
        CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last, old_eth1731_profile, mep_id);
    }
    soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, mep_id, !is_allocated, profile_indx, eth1731_profile_to_use);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

exit:
	BCMDNX_FUNC_RETURN;
}





/**
 * Function verifies that current 
 * DA settings for endpoint agree with given MAC address, if 
 * such exists. If all is well function allocates profiles and 
 * updates HW. 
 *  
 * Function should only be called when MEP_TPYE is Ethernet. 
 *  
 * @author sinai (30/10/2013)
 * 
 * @param unit 
 * @param mep_id 
 * @param classifier_mep 
 * @param msb 
 * @param lsb 
 * @param is_ok  returns 1 if incoming MAC address agrees with 
 *  			 existing MAC adress, 0 otherwise.
 * 
 * @return int 
 */
int  _manage_DA_with_existing_entries_loopback(int unit,int mep_id, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep, uint32 msb, uint32 lsb ,  uint8 * is_ok) {
	return _manage_DA_with_existing_entries_internal(unit,mep_id,classifier_mep,msb,lsb,NULL,
													 classifier_mep->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY,is_ok, 0);
}


/**
 * Function verifies that current 
 * DA settings for endpoint agree with given MAC address and 
 * then updates the SW, HW. 
 * 
 * @author sinai (13/11/2013)
 * 
 * @param unit 
 * @param mep_id 
 * @param classifier_mep 
 * @param msb 
 * @param lsb 
 * @param eth_1731_profile 
 * @param is_ok 
 * 
 * @return int 
 */
int  _manage_DA_with_existing_entries_loss(int unit, int mep_id,SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep, uint32 msb, uint32 lsb , 
										    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY * eth_1731_profile,int is_update, uint8 * is_ok) {
    return _manage_DA_with_existing_entries_internal(unit, mep_id, classifier_mep, msb, lsb, eth_1731_profile, is_update, is_ok, 
                                                     0/*is dm*/ );
}

/**
 * Function verifies that current 
 * DA settings for endpoint agree with given MAC address and 
 * then updates the SW, HW. 
 *  
 * When function is called in the on-demand mode 
 * classifier_mep->flags is updated if there is no  pro-active 
 * service active. 
 * 
 * @author sinai (13/11/2013)
 * 
 * @param unit 
 * @param mep_id 
 * @param classifier_mep 
 * @param msb 
 * @param lsb 
 * @param eth_1731_profile 
 * @param is_ok 
 * 
 * @return int 
 */
int  _manage_DA_with_existing_entries_delay(int unit, int mep_id,SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep, uint32 msb, uint32 lsb , 
										    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY * eth_1731_profile,int is_update, uint8 * is_ok) {
	return _manage_DA_with_existing_entries_internal(unit,mep_id,classifier_mep,msb,lsb,eth_1731_profile, is_update,is_ok, 
                                                     1/*is dm*/ );
}


/**
 * Function adds aditional entries in the MEP DB in the SW and 
 * HW. 
 * 
 * @author sinai (13/11/2013)
 * 
 * @param unit 
 * @param mep_db_entry 
 * 
 * @return int 
 */
int _bcm_oam_lm_dm_add_shared(int unit,SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry){
    uint8  entries_alloced=0,has_dm;
    uint32 entry = 0, soc_sand_rv;
    int rv=0;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_rv = soc_ppd_oam_oamp_next_index_get(unit, mep_db_entry->mep_id, &entry, &has_dm);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
    if ( !mep_db_entry->is_update) {
		if ( entry ==0 || entry==mep_db_entry->mep_id + 4) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("LM/DM can not be added.")));
		}
		mep_db_entry->allocated_id = entry;
		rv =  _bcm_dpp_oam_mep_id_alloc(unit,SHR_RES_ALLOC_WITH_ID ,0 /*ignored anyways*/ ,&entry); 
		BCMDNX_IF_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("request for entry %d in MEP DB denied."),entry) );
		++entries_alloced;
		LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "entry %d allocated\n"), entry));
		if (mep_db_entry->entry_type == SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM_STAT) { /*grab another one*/
			++entry ;
			rv = _bcm_dpp_oam_mep_id_alloc(unit, SHR_RES_ALLOC_WITH_ID,0 /*ignored anyways*/ , &entry);
			BCMDNX_IF_ERR_EXIT(rv);
			LOG_VERBOSE(BSL_LS_BCM_OAM,
                                    (BSL_META_U(unit,
                                                "extended statistics requested: entry %d allocated as well\n"),entry));
			++entries_alloced;
		}
    } else {
		/* in the LM case, see if EXTENDED STATISTICS has been changed */
        if (mep_db_entry->entry_type != SOC_PPD_OAM_LM_DM_ENTRY_TYPE_DM) {
			int num_entries = entry - mep_db_entry->mep_id -1; /* "entry" gives us the next available entry. This gives us the # of entries
			associated with this endpoint. */
			if (num_entries ==1 ) {
                if (mep_db_entry->entry_type == SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM_STAT) {
					/* From LM only to LM + EXTENDED*/
					entry = mep_db_entry->mep_id + 2;
					rv = _bcm_dpp_oam_mep_id_alloc(unit, SHR_RES_ALLOC_WITH_ID,0 /*ignored anyways*/ , &entry);
					BCMDNX_IF_ERR_EXIT(rv);
                }
            } else if (num_entries==2) {
                if (has_dm && mep_db_entry->entry_type==SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM_STAT) {
					/* From LM,DM to LM + EXTENDED, DM*/
                    entry = mep_db_entry->mep_id + 3;
                    rv = _bcm_dpp_oam_mep_id_alloc(unit, SHR_RES_ALLOC_WITH_ID, 0 /*ignored anyways*/, &entry);
			BCMDNX_IF_ERR_EXIT(rv);
                } else if (!has_dm && mep_db_entry->entry_type==SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM ) {
					/* From LM + EXTENDED to LM ONLY*/
					entry = mep_db_entry->mep_id + 2;
                    rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry);
			BCMDNX_IF_ERR_EXIT(rv);
		}
            } else if (num_entries==3) {
                if (has_dm && mep_db_entry->entry_type==SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM) {
					/* From LM+ EXTENDED , DM to LM, DM*/
					entry = mep_db_entry->mep_id + 3;
                    rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry);
			BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
	}

    /*Now let soc take over.*/
    soc_sand_rv = soc_ppd_oam_oamp_lm_dm_set(unit,mep_db_entry);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


exit:
    if ( (_rv!=0 || rv !=0 )  && entries_alloced) {/*the damage we can undo is free the mep-db's allocated.*/
	LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "Freeing unused MEP -DB entry %d \n"), entry ));
        rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry);
        if (rv !=0) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Error: couldn't free unused MEP -DB entry: %d\n "), entry ));
            return rv;
        }
        if (entries_alloced==2) {
            LOG_VERBOSE(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "Freeing second unused MEP -DB entry: %d \n"), entry-1 ));
            rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry-1);
            if (rv != 0) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Error: couldn't free second unused MEP -DB entry: %d \n"), entry));
                return rv;
            }
        }
	}
	    
    BCMDNX_FUNC_RETURN; 
}



int _bcm_oam_lm_dm_remove_shared(int unit,bcm_oam_endpoint_t endpoint_id , int is_lm, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry) {
    int rv,  is_removed, is_allocated;
    int  new_eth_1731_prof,eth1731_profile;
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY old_profile ={0};
    uint32 soc_sand_rv , removed_index;
    uint8 num_removed;
    bcm_port_config_t port_config;
    bcm_oam_endpoint_action_t action; 
    char l_or_d = is_lm? 'L':'D';
    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile_data;
    uint32 found[1];
	SOC_PPD_OAM_MEP_TYPE ccm_entry_type = classifier_mep_entry->mep_type;
	uint8 has_LBR = (classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) >0;
	uint8 only_on_demand_dmm = !is_lm &&  classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;
	uint8 exists_down_mep_piggy_back=0;

    BCMDNX_INIT_FUNC_DEFS;

	(void)l_or_d; /*for compilation*/


    if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint_id) == 0) {

        rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit,endpoint_id , &eth1731_profile, &old_profile);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!only_on_demand_dmm && !old_profile.report_mode) {
            soc_sand_rv = soc_ppd_oam_oamp_search_for_lm_dm(unit, endpoint_id, found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


            *found = is_lm ? SHR_BITGET(found, SOC_PPD_OAM_MEP_TYPE_LM) != 0  : SHR_BITGET(found, SOC_PPD_OAM_MEP_TYPE_DM) != 0;

            if (*found == 0) {
                LOG_DEBUG(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "No %cM entry to remove for endpoint %d.\n"),l_or_d, endpoint_id));
                BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
            }

            /*Now we know there is something to remove so we can delete in the HW and SW*/
            LOG_VERBOSE(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "About to remove %cM entry for endpoint %d.\n"), l_or_d, endpoint_id));

        }

        /*De-alloc from the mep_db, da_nic, da_oui and eth1731-profile tables.*/
        if ((!old_profile.piggy_back_lm || !is_lm) &&  ccm_entry_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {
            /*piggy back entries did not allocate entries from the DA MAC tables*/
            FREE_NIC_AND_OUI_PROFILES(endpoint_id);
        }
        if (old_profile.piggy_back_lm  && (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry)) && SOC_IS_ARADPLUS_A0(unit))  {
            /* In case we are removing a DM entry and the existing a piggybacked LM exists for an LM, and the direction is down, the MEP PE profile should be 0
               so as not to select the UP-MEP-MAC fix LSB program.*/
            exists_down_mep_piggy_back = 1;
        }
        SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile_data);

        /*Next, get a different profile. Fill all the fieldds from the old entry.*/
        eth1731_profile_data.rdi_gen_method = old_profile.rdi_gen_method;
        if (is_lm) {
            eth1731_profile_data.dmm_rate = old_profile.dmm_rate;
            eth1731_profile_data.dmm_offset = old_profile.dmm_offset;
            eth1731_profile_data.dmr_offset = old_profile.dmr_offset;
            if (old_profile.dmm_rate != 0) {
                /* Still using the report mode bit */
                eth1731_profile_data.report_mode = old_profile.report_mode;
            }
        } else {
            eth1731_profile_data.lmm_rate = old_profile.lmm_rate;
            eth1731_profile_data.lmm_offset = old_profile.lmm_offset;
            eth1731_profile_data.lmr_offset = old_profile.lmr_offset;
            eth1731_profile_data.piggy_back_lm = old_profile.piggy_back_lm;
            if ((old_profile.lmm_rate != 0) || old_profile.piggy_back_lm) {
                /* Still using the report mode bit */
                eth1731_profile_data.report_mode = old_profile.report_mode;
            }
        }
        if (has_LBR || eth1731_profile_data.dmm_rate || eth1731_profile_data.lmm_rate
            || (classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE)) {
            eth1731_profile_data.lmm_da_oui_prof = old_profile.lmm_da_oui_prof;
        }
        rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_exchange(unit, eth1731_profile, &eth1731_profile_data, &is_allocated, &is_removed, &new_eth_1731_prof);
        if (eth1731_profile != new_eth_1731_prof) {
            CLEAR_ETH_1731_PROF_IF_IS_LAST(is_removed, eth1731_profile, endpoint_id);
        }
        soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_id, !is_allocated, new_eth_1731_prof, &eth1731_profile_data);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (!only_on_demand_dmm && !old_profile.report_mode) {
            soc_sand_rv = soc_ppd_oam_oamp_lm_dm_remove(unit, endpoint_id, is_lm, exists_down_mep_piggy_back,   &num_removed, &removed_index);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* Now free the the mep db entries*/
            rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, removed_index);
            BCMDNX_IF_ERR_EXIT(rv);
            LOG_VERBOSE(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "entry %d freed\n"), removed_index));
            if (num_removed == 2) { /*Go here if we want to delete an LM entry and there is also a LM-STAT entry.*/
                rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, removed_index + 1);
                BCMDNX_IF_ERR_EXIT(rv);
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "entry %d freed as well\n"), removed_index + 1));
            }
        }
    }

    /*Have the classifier send LM/DM packets back to the CPU.*/

    rv = bcm_petra_port_config_get(unit, &port_config);
    BCMDNX_IF_ERR_EXIT(rv); 
    /*get CPU port*/
    BCM_PBMP_ITER(port_config.cpu,action.destination) {
       break; /*get first CPU port*/
    }

     
    if (classifier_mep_entry->lif != _BCM_OAM_INVALID_LIF) { /* OAM server: do not set the classifier*/
        /* fourth parameter piggy_back_or_1DM : for DM always 1, for LM 0 iff removed entry was P-B*/
        SET_CLASSIFIER_ACTION(endpoint_id,action,is_lm, (!is_lm || !(is_lm && old_profile.piggy_back_lm) ) ,  ccm_entry_type);
    }

    if (only_on_demand_dmm) {
        uint8 found;
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_id, classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found.\n"), endpoint_id));
        }

        
		classifier_mep_entry->flags &= ~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;

        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, endpoint_id, classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
	}


exit:
    BCMDNX_FUNC_RETURN;
}



/**
 * Function removes the Loopback reply mechanism by updating the
 * classifier and the DA registers (the latter in case of 
 * Ethernet OAM). Note that the function does not update the 
 * classifier_mep_entry for the given endpoint. 
 *  
 * It is the callers responsibility to ensure that these actions 
 * are safe (i.e. oam was initialized, is arad+, etc.). 
 * 
 * @author sinai (06/11/2013)
 * 
 * @param unit 
 * @param endpoint_id 
 * @param classifier_mep_entry 
 * 
 * @return int 
 */
int _remove_loopback_reply(int unit, int endpoint_id, const SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry ) {
	bcm_oam_endpoint_action_t action;
	bcm_port_config_t port_config;
    uint32  soc_sand_rv;
    int   rv;
    BCMDNX_INIT_FUNC_DEFS; 

    if (classifier_mep_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) { /* DA address profiles*/
		FREE_NIC_AND_OUI_PROFILES(endpoint_id) ;
		/* The eth1731 profile will remain as it is. */
	}

    if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry) ||(soc_property_get(unit, spn_OAM_RCY_PORT, -1)==-1) ) {
    /* Send LBRs to the to the CPU.*/
        rv = bcm_petra_port_config_get(unit, &port_config);
        BCMDNX_IF_ERR_EXIT(rv);
        /*get CPU port*/
        BCM_PBMP_ITER(port_config.cpu, action.destination) {
            break; /*get first CPU port*/
        }
    } else { /* send to the recycle port so that PRGE will answer LBMs with LBRs*/
        action.destination = soc_property_get(unit, spn_OAM_RCY_PORT, -1); 
    }

	BCM_OAM_OPCODE_CLEAR_ALL(action);
	BCM_OAM_OPCODE_SET(action, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM);
	SET_CLASSIFIER_ACTION_GENERAL(endpoint_id, action, SOC_PPD_OAM_OPCODE_MAP_LBM, classifier_mep_entry->mep_type);

exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Function removes the loopback mechanism from the HW by 
 * reseting the classifier and and clearing the TST/LB 
 * registers. 
 *  
 * 
 * @author sinai (06/11/2013)
 * 
 * @param unit 
 * @param mep_id 
 * @param mep_type 
 * 
 * @return int 
 */
int _remove_loopback(int unit, int mep_id,int mep_type) {
	bcm_oam_endpoint_action_t action;
	bcm_port_config_t port_config;
    int rv;
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS; 

    soc_sand_rv = soc_ppd_oam_oamp_loopback_remove(unit);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Send LBRs to the to the CPU.*/
    rv = bcm_petra_port_config_get(unit, &port_config);
    BCMDNX_IF_ERR_EXIT(rv);
    /*get CPU port*/
    BCM_PBMP_ITER(port_config.cpu, action.destination) {
        break; /*get first CPU port*/
    }

    BCM_OAM_OPCODE_CLEAR_ALL(action);
    BCM_OAM_OPCODE_SET(action, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBR);
    SET_CLASSIFIER_ACTION_GENERAL(mep_id, action, SOC_PPD_OAM_OPCODE_MAP_LBR, mep_type); 

exit:
    BCMDNX_FUNC_RETURN;
}



/**
 * Internal function for handling loss and delay add on the 
 * client side. 
 *  
 * @author sinai (05/01/2015)
 * 
 * @param unit 
 * @param endpoint_id 
 * @param is_single_ended_or_1dm 
 * @param is_lm 
 * 
 * @return STATIC int 
 */
STATIC int loss_delay_add_client_side(int unit,int endpoint_id, int is_single_ended_or_1dm, int is_lm, 
                                      SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {
    int rv;
    int is_single_ended = is_single_ended_or_1dm;
    bcm_oam_endpoint_action_t action ={0};
    SOC_PPD_OAM_LIF_PROFILE_DATA  lif_profile;
    uint32 opcode = is_lm? is_single_ended? SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM : SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM :
            is_single_ended_or_1dm? SOC_PPD_OAM_ETHERNET_PDU_OPCODE_1DM: SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM;
    uint32 meter_disable[1];
    BCMDNX_INIT_FUNC_DEFS;

    BCM_OAM_ACTION_CLEAR_ALL(action);
    BCM_OAM_OPCODE_CLEAR_ALL(action);

	rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, endpoint_id, &lif_profile);
	BCMDNX_IF_ERR_EXIT(rv); 

    action.destination = classifier_mep_entry->remote_gport; /* For down MEPs this will be encoded as a trap code. For Up MEPs as a system port. 
                                                                                            Either way the destination will be the remote OAMP.*/

	if (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
		BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd);
	} else {
		BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd);
	}

	if (!SHR_BITGET(&lif_profile.mep_profile_data.counter_disable, opcode)) {
        BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable); 
    }
    *meter_disable = lif_profile.mep_profile_data.meter_disable;
	if (!SHR_BITGET(meter_disable, opcode)) {
        BCM_OAM_ACTION_SET(action, bcmOAMActionMeterEnable); 
    }

    BCM_OAM_OPCODE_SET(action,opcode);

	if ( (is_single_ended_or_1dm && is_lm) || (!is_single_ended_or_1dm && !is_lm ) ) {
        BCM_OAM_OPCODE_SET(action,opcode -1); /* Response packet's OpCode is one less.*/
	}

    rv = bcm_petra_oam_endpoint_action_set_internal(unit,endpoint_id, &action,is_lm && !is_single_ended_or_1dm);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_oam_loss_add(int unit, bcm_oam_loss_t *loss_ptr) {
	uint8 oam_is_init,found, is_ok, tlv_used;
    uint32 correct_flags =0, lsb, msb;
    int dont_care; 
    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile, old_eth1731_profile;
    bcm_oam_endpoint_action_t action;
    int rv;
    uint32 soc_sand_rv;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY mep_db_entry;
    bcm_gport_t destinations[SOC_DPP_DEFS_MAX(NOF_CORES)];
    uint8 report_mode;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_EXIT;
    }
    

	SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(&mep_db_entry);
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile);

	rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loss_ptr->id, &classifier_mep_entry, &found);
	BCMDNX_IF_ERR_EXIT(rv);
	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loss_ptr->id));
	}

	if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(loss_ptr->id) != 0) {
		/* Redirect CCMs/LMMs to the remote OAMP and that's it.*/
		BCMDNX_IF_ERR_EXIT(loss_delay_add_client_side(unit, loss_ptr->id, (loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) != 0, 1, &classifier_mep_entry));

		BCM_EXIT;
	}


	if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

    report_mode = ((loss_ptr->flags & BCM_OAM_LOSS_REPORT_MODE) == BCM_OAM_LOSS_REPORT_MODE);

    /*First, verify that the parameters are correct and that the endpoint is indeed accelrated.*/
    correct_flags |= BCM_OAM_LOSS_SINGLE_ENDED | BCM_OAM_LOSS_STATISTICS_EXTENDED | BCM_OAM_LOSS_UPDATE;
    if (SOC_IS_JERICHO(unit)) {
        correct_flags |= BCM_OAM_LOSS_REPORT_MODE|BCM_OAM_LOSS_SLM;
        if ((loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED)
            && (loss_ptr->flags & BCM_OAM_LOSS_REPORT_MODE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_OAM_LOSS_REPORT_MODE and BCM_OAM_LOSS_STATISTICS_EXTENDED cannot be used together.")));
        }
    }

    if (loss_ptr->flags & ~correct_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal flag combination set.")));
    }

    if (loss_ptr->loss_farend ||  loss_ptr->loss_nearend || loss_ptr->gport || loss_ptr->int_pri || loss_ptr->loss_threshold ||
        loss_ptr->pkt_dp_bitmap || loss_ptr->pkt_pri || loss_ptr->rx_farend || loss_ptr->rx_nearend ||
        loss_ptr->rx_oam_packets || loss_ptr->tx_farend || loss_ptr->tx_nearend || loss_ptr->tx_oam_packets ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal field set.")));
    }

    if ( (loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED)==0 && !BCM_MAC_IS_ZERO(loss_ptr->peer_da_mac_address)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When LOSS_SINGLE_ENDED flag is set peer_da_mac_address must be zero.")));
    }
    if (loss_ptr->period==0 && (loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Period may not be set to 0.")));
    }

    if (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), loss_ptr->id));
    }
    if ( SOC_IS_ARADPLUS_A0(unit) && ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED)==0) ) {
        /* In Arad+, can't use piggy-backed LM together with port/interface status TLV (due to HW bug)
           Check that it's not used in the MEP_DB */
		rv = _bcm_petra_oam_is_tlv_used(unit, loss_ptr->id, &tlv_used);
		BCMDNX_IF_ERR_EXIT(rv);
        if (tlv_used) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
								(_BSL_BCM_MSG("Endpoint %d has status TLV so it can't use piggy backed LM."), loss_ptr->id));
        }
    }

    if (SOC_IS_JERICHO(unit)) {
        /* Jericho validations - Check compatability between the report mode in the MEP profile and the requested report mode */
        SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&old_eth1731_profile);
        rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, loss_ptr->id, &dont_care, &old_eth1731_profile);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((!report_mode) && old_eth1731_profile.report_mode) {
            /* This ep uses reports, but trying to add loss not in report mode */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses reports for LM/DM but loss added without reports flag"), loss_ptr->id));
        }

        if (report_mode && (old_eth1731_profile.report_mode == 0)) {
            uint32 found;
            /* Requested report mode but the profile is either no-report-mode or don't-care.
               Check if there's a shared entry for LM/DM or piggy-backed LM (which would mean No-Report mode) */
            if (old_eth1731_profile.piggy_back_lm) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses piggy-back LM but loss added with report flag"), loss_ptr->id));
            }

            /* Not Piggy-backed. Check for no-report LM/DM */
            soc_sand_rv = soc_ppd_oam_oamp_search_for_lm_dm(unit, loss_ptr->id, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (found != 0) { /* LM/DM entry found. MEP uses no-report mode */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses shared entry for LM/DM but loss added with report flag"), loss_ptr->id));
            }
        }
    }

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "passed initial tests")));


	/* First, the troika of profiles.*/
    lsb =  GET_LSB_FROM_MAC_ADDR(loss_ptr->peer_da_mac_address);
    msb =  GET_MSB_FROM_MAC_ADDR(loss_ptr->peer_da_mac_address); 
    if (loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) {
        SOC_PPD_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(loss_ptr->period, 0, eth1731_profile.lmm_rate);
    } else {
        eth1731_profile.lmm_rate = 0; /*the given rate is ignored, LMMs are not sent by the OAMP anyways.*/
        eth1731_profile.piggy_back_lm = 1;
	}
    eth1731_profile.slm_lm = (loss_ptr->flags & BCM_OAM_LOSS_SLM) ==BCM_OAM_LOSS_SLM;
    if (SOC_IS_JERICHO(unit) && report_mode) {
        eth1731_profile.report_mode = 1;
        if (old_eth1731_profile.report_mode == 0) {
            /* We previously validated that the report mode in the profile was don't care (for this case)
               beause no LM or DM was defined for the ep, so now the DMM rate needs to be set to 0 (since we're adding LM)*/
            eth1731_profile.dmm_rate = 0;
        }
    }

	rv = _manage_DA_with_existing_entries_loss(unit,loss_ptr->id, &classifier_mep_entry,msb,lsb,
											   &eth1731_profile, loss_ptr->flags &BCM_OAM_LOSS_UPDATE, &is_ok );
	BCMDNX_IF_ERR_EXIT(rv);
    if (!is_ok) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MAC adddress may not be changed")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit) || (!report_mode)) {
        mep_db_entry.entry_type = (loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED) ? SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM_STAT : SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM;
        mep_db_entry.mep_id = loss_ptr->id;
    mep_db_entry.is_update = ((loss_ptr->flags & BCM_OAM_LOSS_UPDATE) == BCM_OAM_LOSS_UPDATE);
    mep_db_entry.is_piggyback_down = ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED)==0) && 
            (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry)) ;

        rv = _bcm_oam_lm_dm_add_shared(unit,&mep_db_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }


	if (classifier_mep_entry.lif != _BCM_OAM_INVALID_LIF) { /*Have the classifier send LM packets to the OAMP, but only if the endpoint exists on this device.*/
		rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), destinations, &dont_care);
		action.destination = destinations[0];
		SET_CLASSIFIER_ACTION(loss_ptr->id, action, 1 /*is_lm */, (loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) == 0, classifier_mep_entry.mep_type);
	}

exit:
    BCMDNX_FUNC_RETURN;
}




int bcm_petra_oam_loss_get(int unit, bcm_oam_loss_t * loss_ptr) {
    uint8 oam_is_init, found, has_dm;
    uint32 soc_sand_rv, entry;
    SOC_PPD_OAM_OAMP_LM_INFO_GET lm_info;
    int rv, dont_care;
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile_data;
	uint32 ccm_period_ms, ccm_period_micro_s;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
	int num_entries ;


    BCMDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(ccm_period_micro_s);

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    }

	if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

	if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(loss_ptr->id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function may not be called for cleint side endpoint: Nothing to get")));
	}


    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loss_ptr->id, &classifier_mep_entry, &found);
			BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loss_ptr->id));
    }

    rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, loss_ptr->id, &dont_care, &eth1731_profile_data);
			BCMDNX_IF_ERR_EXIT(rv); 

    if (!eth1731_profile_data.lmm_rate && eth1731_profile_data.report_mode) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint with id %d has no loss measurement."), loss_ptr->id));
    }

    if (!eth1731_profile_data.report_mode) {

		SOC_PPD_OAM_OAMP_LM_INFO_GET_clear(&lm_info);
		lm_info.entry_id = loss_ptr->id;
		soc_sand_rv = soc_ppd_oam_oamp_lm_get(unit, &lm_info);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

		loss_ptr->rx_farend = lm_info.peer_rx;
		loss_ptr->tx_farend =  lm_info.peer_tx;
		loss_ptr->tx_nearend = lm_info.my_tx;
		loss_ptr->rx_nearend = lm_info.my_rx;
		if (lm_info.is_extended) {
			loss_ptr->loss_farend = (lm_info.my_tx == 0) ? 0 : (100 * lm_info.acc_lm_far) / lm_info.my_tx;
			loss_ptr->loss_nearend = (lm_info.peer_tx == 0) ? 0 : (100 * lm_info.acc_lm_near) / lm_info.peer_tx;
			loss_ptr->loss_nearend_max = lm_info.max_lm_near;
			loss_ptr->loss_nearend_acc = lm_info.acc_lm_near;
			loss_ptr->loss_farend_max = lm_info.max_lm_far;
			loss_ptr->loss_farend_acc = lm_info.acc_lm_far;
			}
		}

    /* Non statistics*/
	loss_ptr->flags = (eth1731_profile_data.report_mode ? BCM_OAM_LOSS_REPORT_MODE : 0);
    if (!eth1731_profile_data.piggy_back_lm) {
        loss_ptr->flags |= BCM_OAM_LOSS_SINGLE_ENDED;
        /* Get the rate and maybe the DA address */
        SOC_PPD_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, eth1731_profile_data.lmm_rate);
        loss_ptr->period = ccm_period_ms;
        if (classifier_mep_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_ETH_OAM ) {
			_BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(loss_ptr,loss_ptr->id );
        }
        loss_ptr->flags |= BCM_OAM_LOSS_SLM * eth1731_profile_data.slm_lm;
    }

    if (!eth1731_profile_data.report_mode) {
        soc_sand_rv = soc_ppd_oam_oamp_next_index_get(unit, loss_ptr->id, &entry, &has_dm);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        num_entries = entry - loss_ptr->id - 1;

        if ((num_entries == 2 && !has_dm) || num_entries == 3) {
            loss_ptr->flags |= BCM_OAM_LOSS_STATISTICS_EXTENDED;
        }
    }


exit:
    BCMDNX_FUNC_RETURN; 
}



int bcm_petra_oam_loss_delete(int unit, bcm_oam_loss_t *loss_ptr) {
	uint8 oam_is_init,found;
    int rv;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_EXIT;
    }

	if (!SOC_IS_ARADPLUS(unit) && !_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(loss_ptr->id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loss_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,(_BSL_BCM_MSG("Endpoint %d not found."),loss_ptr->id));
    }

    /*Do what we can in the shared function*/
    rv = _bcm_oam_lm_dm_remove_shared(unit,loss_ptr->id ,1/*is lm*/, &classifier_mep_entry );
    if (rv == BCM_E_NOT_FOUND) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    }
    BCMDNX_IF_ERR_EXIT(rv);

    
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_delay_add(int unit, bcm_oam_delay_t *delay_ptr) {
	uint8 oam_is_init,found, is_ok;
    uint32 soc_sand_rv;
    int rv;
    bcm_oam_endpoint_action_t action;
    int dont_care; 
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY mep_db_entry;
    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile, old_eth1731_profile;
	uint32 correct_flags=0,lsb,msb;
    bcm_gport_t destinations[SOC_DPP_DEFS_MAX(NOF_CORES)];
    uint8 report_mode;
    uint8 set_1dm_explicitly=1;
    uint8 is_1dm = (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) >0;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    } 

   SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(&mep_db_entry);
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry)  ;
    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, delay_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), delay_ptr->id));
    }


	if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(delay_ptr->id) != 0) {
		/* Redirect CCMs/LMMs to the remote OAMP and that's it.*/
		BCMDNX_IF_ERR_EXIT(loss_delay_add_client_side(unit, delay_ptr->id, (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) != 0, 0, &classifier_mep_entry));

		BCM_EXIT;
	}

    /*First, verify that the parameters are correct and that the endpoint is indeed accelrated.*/
	if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }
		
    report_mode = ((delay_ptr->flags & BCM_OAM_DELAY_REPORT_MODE) == BCM_OAM_DELAY_REPORT_MODE);

    /*First, verify that the parameters are correct and that the endpoint is indeed accelrated.*/

    correct_flags |= BCM_OAM_DELAY_ONE_WAY | BCM_OAM_DELAY_UPDATE;
    if (SOC_IS_JERICHO(unit)) {
        correct_flags |= BCM_OAM_DELAY_REPORT_MODE;
			}

    if (delay_ptr->flags & ~correct_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal flag combination set.")));
    }

    if (delay_ptr->timestamp_format !=bcmOAMTimestampFormatIEEE1588v1 ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only 1588 time stamp format supported for y1731.")));
    }

    if (delay_ptr->int_pri || delay_ptr->pkt_pri || delay_ptr->rx_oam_packets  || delay_ptr->tx_oam_packets ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal field set.")));
    }

    if (delay_ptr->period ==-1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("-1 period not supported.")));
    }

    if (delay_ptr->period == 0 && SOC_IS_ARADPLUS_A0(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Period may not be set to 0.")));
    }

    if (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), delay_ptr->id));
    }

    if (classifier_mep_entry.lif != _BCM_OAM_INVALID_LIF && ! (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not defined as 1588"), delay_ptr->id));
    }

    if (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) {
		/* 1 dm requires an OAMP_PE program in ARAD+, make sure it's available */
		rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_pe_use_1dm_check, (unit,delay_ptr->id));
		BCMDNX_IF_ERR_EXIT(rv);
    }


    if (SOC_IS_JERICHO(unit)) {
        /* Jericho validations - Check compatability between the report mode in the MEP profile and the
           requested report mode (In case this is not on-demand DMM) */
        SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&old_eth1731_profile);
        rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, delay_ptr->id, &dont_care, &old_eth1731_profile);
        BCMDNX_IF_ERR_EXIT(rv);

        if (delay_ptr->period) {
			if ((!report_mode) && old_eth1731_profile.report_mode) {
				/* This ep uses reports, but trying to add delay not in report mode */
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses reports for LM/DM but delay added without reports flag"), delay_ptr->id));
			}

			if (report_mode && (old_eth1731_profile.report_mode == 0)) {
				uint32 found;
				/* Requested report mode but the profile is either no-report-mode or don't-care.
				   Check if there's a shared entry for LM/DM or piggy-backed LM (which would mean No-Report mode) */
				if (old_eth1731_profile.piggy_back_lm) {
					BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses piggy-back LM but delay added with report flag"), delay_ptr->id));
				}

				/* Not Piggy-backed. Check for no-report LM/DM */
				soc_sand_rv = soc_ppd_oam_oamp_search_for_lm_dm(unit, delay_ptr->id, &found);
				BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
				if (found != 0) { /* LM/DM entry found. MEP uses no-report mode */
					BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses shared entry for LM/DM but delay added with report flag"), delay_ptr->id));
				}
			}
        }
    }

   LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "passed initial tests")));

	/* First, the troika of profiles.*/
    lsb =  GET_LSB_FROM_MAC_ADDR(delay_ptr->peer_da_mac_address);
    msb =  GET_MSB_FROM_MAC_ADDR(delay_ptr->peer_da_mac_address); 
    SOC_PPD_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(delay_ptr->period, 0 , eth1731_profile.dmm_rate);
    if (SOC_IS_JERICHO(unit) && report_mode && (delay_ptr->period)) {
        eth1731_profile.report_mode = 1;
        if (old_eth1731_profile.report_mode == 0) {
            /* We previously validated that the report mode in the profile was don't care (for this case)
               beause no LM or DM was defined for the ep, so now the LMM rate needs to be set to 0 (since we're adding DM)*/
            eth1731_profile.lmm_rate = 0;
        }
    }
	else if (SOC_IS_JERICHO(unit) && (delay_ptr->period == 0)) {
		/* On demand DM should not change the report_mode in the profile */
        eth1731_profile.report_mode = old_eth1731_profile.report_mode;
	}
	rv = _manage_DA_with_existing_entries_delay(unit,delay_ptr->id, &classifier_mep_entry,msb,lsb,
											   &eth1731_profile, delay_ptr->flags &BCM_OAM_DELAY_UPDATE, &is_ok );
	BCMDNX_IF_ERR_EXIT(rv);
    if (!is_ok) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MAC adddress may not be changed")));
    }


    if (delay_ptr->period) {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit) || (!report_mode)) {
            /*filling the mep_db_entry and having the shared function do its thing*/
            mep_db_entry.entry_type = SOC_PPD_OAM_LM_DM_ENTRY_TYPE_DM ;
            mep_db_entry.mep_id = delay_ptr->id;

            mep_db_entry.is_1DM =  (delay_ptr->flags& BCM_OAM_DELAY_ONE_WAY) / BCM_OAM_DELAY_ONE_WAY; /*1 iff the flag is set.*/

            mep_db_entry.is_update = ((delay_ptr->flags & BCM_OAM_DELAY_UPDATE) == BCM_OAM_DELAY_UPDATE);

            rv = _bcm_oam_lm_dm_add_shared(unit,&mep_db_entry );
            BCMDNX_IF_ERR_EXIT(rv);

            set_1dm_explicitly = 0; /* 1dm was set by _bcm_oam_lm_dm_add_shared */
        }
    }
    else {
		uint8 using_1dm =0;

        if (eth1731_profile.dmm_rate) {
			/* the DMM rate in the eth1731 profile will be positive if and only if there was found to be an proactive service as well for this MEP.*/
			rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_1dm_get, (unit,delay_ptr->id, &using_1dm));
			BCMDNX_IF_ERR_EXIT(rv);

			if (is_1dm != using_1dm) {
					LOG_WARN(BSL_LS_BCM_OAM,
											 (BSL_META_U(unit,
														 "Proactive session with differing 1DM/DMM status exists."
														 "BCM_OAM_DELAY_ONE_WAY flag ingnored \n")));
                    set_1dm_explicitly = 0;
            }
        }

    }

    if (set_1dm_explicitly) {
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_1dm_set, (unit,delay_ptr->id, is_1dm));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (delay_ptr->period == 0) {
        /* on demand DM */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_dm_trigger_set, (unit,delay_ptr->id));
        BCMDNX_IF_ERR_EXIT(rv);
    }

	if (classifier_mep_entry.lif != _BCM_OAM_INVALID_LIF) {
		/*Have the classifier send DM packets to the OAMP.*/
		rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), destinations, &dont_care);
		action.destination = destinations[0];
		SET_CLASSIFIER_ACTION(delay_ptr->id, action, 0 /*is_lm */, delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY, classifier_mep_entry.mep_type);
	}

    if (delay_ptr->period==0) {
        /* Update the classifier mep entry (The flags may have changed)*/
        uint8 only_on_demand = (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE) !=0;
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, delay_ptr->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), delay_ptr->id));
	}
        
        if (only_on_demand) {
			classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE; 
        } else {
			classifier_mep_entry.flags &= ~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;
	}


        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, delay_ptr->id, &classifier_mep_entry);
	BCMDNX_IF_ERR_EXIT(rv);
    }


exit:
    BCMDNX_FUNC_RETURN; 
}

int bcm_petra_oam_delay_get(int unit, bcm_oam_delay_t *delay_ptr) {
    uint8 oam_is_init, found, is_1dm=0;
    uint32 soc_sand_rv;
    SOC_PPD_OAM_OAMP_DM_INFO_GET dm_info;
    int rv, dont_care;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
	uint32 ccm_period_ms, ccm_period_micro_s;
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile_data;
    BCMDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(ccm_period_micro_s);

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
	}


	if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(delay_ptr->id)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function may not be called for cleint side endpoint (nothing to get).")));
	}


    if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

    SOC_PPD_OAM_OAMP_DM_INFO_GET_clear(&dm_info);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, delay_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), delay_ptr->id));
    }

    rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, delay_ptr->id, &dont_care, &eth1731_profile_data);
    BCMDNX_IF_ERR_EXIT(rv); 

    if (eth1731_profile_data.report_mode && !eth1731_profile_data.dmm_rate) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint with id %d has no delay measurement."), delay_ptr->id));
    }

    delay_ptr->flags = eth1731_profile_data.report_mode ? BCM_OAM_DELAY_REPORT_MODE : 0;

    if (eth1731_profile_data.dmm_rate) {
        if (!eth1731_profile_data.report_mode) {
			dm_info.entry_id = delay_ptr->id;
			soc_sand_rv = soc_ppd_oam_oamp_dm_get(unit, &dm_info, &is_1dm );
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

			COMPILER_64_SET(delay_ptr->delay.seconds,0,  dm_info.last_delay_second );
			delay_ptr->delay.nanoseconds = dm_info.last_delay_sub_seconds;

			COMPILER_64_SET(delay_ptr->delay_max.seconds,0,  dm_info.max_delay_second );
			delay_ptr->delay_max.nanoseconds = dm_info.max_delay_sub_seconds;


			COMPILER_64_SET(delay_ptr->delay_min.seconds,0,  dm_info.min_delay_second );
			delay_ptr->delay_min.nanoseconds = dm_info.min_delay_sub_seconds;
        }
        else {
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_1dm_get, (unit,delay_ptr->id, &is_1dm));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        SOC_PPD_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, eth1731_profile_data.dmm_rate);
        delay_ptr->period = ccm_period_ms; 
    } else {
		/* One shot DMM session is active*/
		delay_ptr->period=0;

        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_1dm_get, (unit,delay_ptr->id, &is_1dm)); 
        BCMDNX_IF_ERR_EXIT(rv);

	}

	/* Now the period and maybe the DA address.*/

    if (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {
        _BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(delay_ptr,delay_ptr->id);
    }
    if (is_1dm) {
		delay_ptr->flags |= BCM_OAM_DELAY_ONE_WAY;
    }

	delay_ptr->timestamp_format = bcmOAMTimestampFormatIEEE1588v1;

exit:
    BCMDNX_FUNC_RETURN; 
}


int bcm_petra_oam_delay_delete(int unit, bcm_oam_delay_t *delay_ptr) {
	uint8 oam_is_init,found;
    int rv;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_EXIT;
    }

    if (!SOC_IS_ARADPLUS(unit)  && !_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(delay_ptr->id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, delay_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,(_BSL_BCM_MSG("Endpoint %d not found."),delay_ptr->id));
    }

    /*Do what we can in the shared function*/
    rv = _bcm_oam_lm_dm_remove_shared(unit,delay_ptr->id ,0/*is lm*/, &classifier_mep_entry);
    if (rv == BCM_E_NOT_FOUND) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    }
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/** Accelerated loop-back */

/* Adds TLV extension to an accelerated loopback object */
int _bcm_oam_loopback_acc_packet_tlv_set(int unit, bcm_oam_loopback_t *loopback_ptr,
                                         ARAD_PP_OAM_LOOPBACK_INFO *soc_loopback_info) {

    bcm_oam_tlv_t *tlv_ptr = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    /* Validation */
    BCMDNX_NULL_CHECK(loopback_ptr);
    BCMDNX_NULL_CHECK(soc_loopback_info);
    tlv_ptr = &(loopback_ptr->tlvs[0]);
    /* Currently supporting only 1 TST-TLV */
    if (loopback_ptr->num_tlvs != 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Supporting only 1 TLV on loopback messages")));
    }
    if (tlv_ptr->tlv_length & (~(0xffff))) {
        /* Negative or too long */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Unsupported TLV Length."
                                          " Supporting only lengths 0-%u"),0xffff));
    }
    if ((tlv_ptr->tlv_length < 4)
        && ((tlv_ptr->tlv_type == bcmOamTlvTypeTestNullWithCRC)
            ||(tlv_ptr->tlv_type == bcmOamTlvTypeTestPrbsWithCRC))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("To use CRC in Test TLV,"
                                          " TLV-Length must be greater than 4")));
    }
    /* Validation over */

    /* Parse type */
    switch (tlv_ptr->tlv_type) {
    case bcmOamTlvTypeTestNullWithoutCRC:
        soc_loopback_info->tst_tlv_type = ARAD_PP_OAM_DATA_TST_TLV_NULL_NO_CRC;
        break;
    case bcmOamTlvTypeTestNullWithCRC:
        soc_loopback_info->tst_tlv_type = ARAD_PP_OAM_DATA_TST_TLV_NULL_WITH_CRC;
        break;
    case bcmOamTlvTypeTestPrbsWithoutCRC:
        soc_loopback_info->tst_tlv_type = ARAD_PP_OAM_DATA_TST_TLV_PRBS_NO_CRC;
        break;
    case bcmOamTlvTypeTestPrbsWithCRC:
        soc_loopback_info->tst_tlv_type = ARAD_PP_OAM_DATA_TST_TLV_PRBS_WITH_CRC;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Unsupported TLV type for loopback messages"
                                          " (Only Test type supported)")));
    }

    /* Parse length */
    soc_loopback_info->tst_tlv_len = tlv_ptr->tlv_length;

    /* Mark the TLV existance */
    soc_loopback_info->has_tst_tlv = 1;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Add Packet generation for a loopback object */
int _bcm_oam_loopback_acc_packet_generation_add(int unit, bcm_oam_loopback_t *loopback_ptr,
                                                SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
                                                uint32 *new_flags) {
    int rv = 0;
    uint32 soc_sand_rv;
    ARAD_PP_OAM_LOOPBACK_INFO soc_loopback_info = {0};

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(loopback_ptr);
    BCMDNX_NULL_CHECK(classifier_mep_entry);
    BCMDNX_NULL_CHECK(new_flags);

    /* Set SOC layer info */
    soc_loopback_info.endpoint_id = loopback_ptr->id;
    soc_loopback_info.tx_period = loopback_ptr->period;
    soc_loopback_info.is_period_in_kpps =
        ((loopback_ptr->flags & BCM_OAM_LOOPBACK_PERIOD_IN_KPPS) == BCM_OAM_LOOPBACK_PERIOD_IN_KPPS);
    soc_loopback_info.is_added =  (loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE) &&
        (classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK);
    /* DA Mac address */
    rv = _bcm_petra_mac_to_sand_mac(loopback_ptr->peer_da_mac_address, &soc_loopback_info.mac_address);
    BCMDNX_IF_ERR_EXIT(rv);
    /* Optional TLV */
    if (loopback_ptr->num_tlvs) {
        rv = _bcm_oam_loopback_acc_packet_tlv_set(unit, loopback_ptr, &soc_loopback_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        soc_loopback_info.has_tst_tlv = 0;
    }

    /*
       in the case of loopback update, the state of the HW is to be
       ignored only if the current endpoint previously set the LB
       mechanism. Otherwise, it may be the case that update has been
       called with the intention of adding the LB mechanism on top
       of the LBR mechanism. In this case, setting the LB registers
       is allowed only if no other endpoint has set the LB registers.
    */
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_loopback_set,(unit, &soc_loopback_info));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (soc_loopback_info.is_added == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Only one Loopback session allowed.")));
    }

    /* Flags to add to the classifier db - done outside this function */
    *new_flags |= (SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK
                   | (soc_loopback_info.is_period_in_kpps ?
                      SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_LOOPBACK_PERIOD_IN_KPPS : 0));

exit:
    BCMDNX_FUNC_RETURN;
}


int _insert_oam_data_tlv_head_for_tst_lb(uint8* buffer, int length, int byte_start_offset)
{
    int offset = byte_start_offset;

    buffer[offset++] = 0;    /* Resever 4bytes for sequence Number */
    buffer[offset++] = 0;    
    buffer[offset++] = 0;    
    buffer[offset++] = 0;    
    buffer[offset++] = 3;                      /* Data TLV Type */
    buffer[offset++] = (length>>8)&0xff;             /* Data TLV Length */
    buffer[offset++] = (length&0xff);                /* Data TLV Length */

    return offset;
}

int _insert_oam_test_tlv_head_for_tst_lb(uint8* buffer, int length,int patten_type, int byte_start_offset)
{
    int offset = byte_start_offset;
    offset += 4; 
    buffer[offset++] = 32;                      /* Test TLV Type */
    buffer[offset++] = (length>>8)&0xff;             /* Test TLV Length */
    buffer[offset++] = (length&0xff);                /* Test TLV Length */
    
    /* Test TLV patten_type */
    switch(patten_type)
    {
       case bcmOamTlvTypeTestPrbsWithCRC:
          buffer[offset++] = 3;                
          break;
       case bcmOamTlvTypeTestPrbsWithoutCRC:
           buffer[offset++] = 2;               
           break;
       case bcmOamTlvTypeTestNullWithCRC:
          buffer[offset++] = 1;                
          break;
       case bcmOamTlvTypeTestNullWithoutCRC:
           buffer[offset++] =0;                
           break;
       default:
          break;
    }
    return offset;
}

int sat_loopback_add_rx(int unit, bcm_oam_loopback_t *loopback_ptr) 
{
    uint8 found=0;
    int rv = BCM_E_NONE;
    int trap_id=0;
    uint32 lb_trap_id=0;
    int trap_index=0;
    int trap_data = 0;
    bcm_sat_ctf_t ctf_id = 0;
    uint32 soc_sand_rv=0;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    bcm_sat_ctf_packet_info_t packet_info;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_SAT_CTF_ENTRY ctf_info;
    int index =0;
    int index_2 =0;
    bcm_sat_ctf_identifier_t identifier;
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)];
    bcm_oam_endpoint_action_t action;
    int dont_care; 
    int session_id=0;
    int tc = 0;
    int dp = 0; 
    int tc_max = 7;
    int dp_max =3;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
    SOC_PPD_OAM_SAT_CTF_ENTRY_clear(&ctf_info);
    
    /*get entry of mep db*/
    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, loopback_ptr->id, &mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
    }
    if ((!(loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE))&&(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d has already add LB TX&RX objext."), loopback_ptr->id));
    } 
    if((loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE)&&(!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER))){
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d  lb objext don't exist ."), loopback_ptr->id));
    }

    if((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == 0)/* Down mep */
    {
        
        /******  Down MEP Classification    *****/
        rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_SAT0_LB, &lb_trap_id);
        trap_id = (int)lb_trap_id;
        BCMDNX_IF_ERR_EXIT(rv);
        
        bcm_rx_trap_config_t_init(&trap_config); 
        
        /*2. Set the destination in the trap to be the OAMP*/
        rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &dont_care);
        BCMDNX_IF_ERR_EXIT(rv);
        /*And then*/
        trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
        trap_config.dest_port = oamp_port[0];
        
        /*3.  Update the FWD header:*/
        trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
        trap_config.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
        
        rv = bcm_petra_rx_trap_set(unit, trap_id, &trap_config);
        BCMDNX_IF_ERR_EXIT(rv);
        
        BCM_OAM_ACTION_CLEAR_ALL(action);
        BCM_OAM_OPCODE_CLEAR_ALL(action);
        
        /*action.destination = oamp_port[0];*/
        
        /*4 Call the API endpoint_action_set.The destination should be set set to the trap code above:*/
        BCM_GPORT_TRAP_SET(action.destination, trap_id, 7, 0); /*Taken from default values*/
        /*Action should be:*/
        if(mep_db_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM){
            BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd);
        }else if((mep_db_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP)
                    ||(mep_db_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)) {
            BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently Jericho LB just support in ETH / MPLS/PWE ")));
        }
        
        /*Opcode should be LBR*/
        BCM_OAM_OPCODE_SET(action, 2); /* 2-LBR*/
        
        /*After calling bcm_oam_endpoint_action_set with the above sequence, Down MEP packets will be redirected to the SAT.*/
        rv = bcm_petra_oam_endpoint_action_set(unit,loopback_ptr->id,&action);
        BCMDNX_IF_ERR_EXIT(rv);
        
        /******  CTF configuration.    *****/
        if(loopback_ptr->flags & BCM_OAM_LOOPBACK_WITH_CTF_ID){  /* if flag is update, user should give the cft_id*/
            ctf_id = loopback_ptr->ctf_id;
            /* Create a CTF with ID*/
            rv = bcm_common_sat_ctf_create(unit, BCM_SAT_CTF_WITH_ID, &ctf_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else if(loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE){ /* flag is update, don't need to create a new Ctf*/
            ctf_id = loopback_ptr->ctf_id;
            rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, ctf_id, &ctf_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ctf_id %d not found."), ctf_id));
            }
        }
        else{
            /* Create a CTF */
            rv = bcm_common_sat_ctf_create(unit, 0, &ctf_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        
        loopback_ptr->ctf_id = ctf_id; /* save gtf_id */
        ctf_info.flag = 1;/*1:lb enalbe / 2:tst enable */
        ctf_info.mepid= loopback_ptr->id;
        
        if(!(loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE)){        
          rv = bcm_common_sat_ctf_trap_add(unit, trap_id);
          BCMDNX_IF_ERR_EXIT(rv);
             
          trap_data = loopback_ptr->id;
        
          /* map OAM-ID + trap code to CTF ID */
          /* map trap data to session ID */

          for(session_id=0;session_id <16;session_id++){
               rv = bcm_common_sat_ctf_trap_data_to_session_map(unit, trap_data, 0xff, session_id);/*OAM-ID is endpoint ID and session ID*/
               if((rv == BCM_E_NONE)||(rv == BCM_E_EXISTS))
               {
                   break;
               }
               else
               {
                   BCMDNX_IF_ERR_EXIT(rv);
               }
          }
          classifier_mep_entry.session_map_flag++;
          
          rv = bcm_common_sat_trap_idx_get(unit, trap_id, &trap_index);
          BCMDNX_IF_ERR_EXIT(rv);
          
          /*(2)  Map identification(four content) to specific flow-id */
          bcm_sat_ctf_identifier_t_init(&identifier); 
          identifier.session_id = session_id;  /*oam ID*/
          identifier.trap_id = trap_index; 
          for(dp = 0; dp <= dp_max; dp++)
          {
              for(tc = 0; tc <= tc_max; tc++)
                {
                  identifier.color = dp; 
                  identifier.tc =tc;   
                  rv = bcm_common_sat_ctf_identifier_map(unit, &identifier, ctf_id);
                  BCMDNX_IF_ERR_EXIT(rv);
              }
          }
          /*save identifier for tst*/
          ctf_info.identifier_session_id = session_id;  /*oam ID*/
          ctf_info.identifier_trap_id = trap_index; 
        }
        /* Set the recieved packet information */
        bcm_sat_ctf_packet_info_t_init(&packet_info);
        packet_info.sat_header_type = bcmSatHeaderY1731;
        
        /*  Seq no offset set 4, should be the same with GTF send packet */
        packet_info.offsets.seq_number_offset = 4;
        
       /* LB just have one of Data/Test TLV*/
       for(index =0; index<BCM_OAM_MAX_NUM_TLVS_FOR_LBM; index++ ){
          if(loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeData){
             packet_info.offsets.payload_offset = 11;
           }
          else {
             packet_info.offsets.payload_offset = 12; /* for test tlv*/
          }
          
          /* Test TLV */
          if((loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
              || (loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)){
              packet_info.payload.payload_type = bcmSatPayloadPRBS;
          }
          else if((loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestNullWithCRC)
             ||(loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestNullWithoutCRC)){
             packet_info.payload.payload_type = bcmSatPayloadConstant4Bytes; 
             for (index_2 = 0; index_2 < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index_2++){
                packet_info.payload.payload_pattern[index_2]=0; 
             }
          }
    
           /* Data TLV */
           if(loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeData){
              packet_info.payload.payload_type = bcmSatPayloadConstant4Bytes;
              packet_info.payload.payload_pattern[0] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern&0xFF000000)>>24;
              packet_info.payload.payload_pattern[1] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern&0xFF0000)>>16;
              packet_info.payload.payload_pattern[2] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern&0xFF00)>>8;
              packet_info.payload.payload_pattern[3] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern&0xFF);
              packet_info.payload.payload_pattern[4] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern&0xFF000000)>>24;
              packet_info.payload.payload_pattern[5] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern&0xFF0000)>>16;
              packet_info.payload.payload_pattern[6] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern&0xFF00)>>8;
              packet_info.payload.payload_pattern[7] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern&0xFF);
          }
    
          /*End TLV and CRC*/
           if((loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
              || (loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestNullWithCRC)){
                packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
            }
           else
            {
                 packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
           }
      }
     
      rv = bcm_common_sat_ctf_packet_config(unit, ctf_id, &packet_info);
      BCMDNX_IF_ERR_EXIT(rv);
      }
       else 
       {
           
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("currently just support Down MEP")));
       }
        
       /*update sw db mep_info with new mep */
       rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, loopback_ptr->id, &classifier_mep_entry);
       BCMDNX_IF_ERR_EXIT(rv); 
       if(loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE) {
           rv = _bcm_dpp_oam_sat_ctf_info_db_update(unit, ctf_id, &ctf_info);
           BCMDNX_IF_ERR_EXIT(rv);
       }
       else{
           rv = _bcm_dpp_oam_sat_ctf_info_db_insert(unit, ctf_id, &ctf_info);
           BCMDNX_IF_ERR_EXIT(rv);
       }
       
exit:
    BCMDNX_FUNC_RETURN;
}


int sat_loopback_add_tx(int unit, bcm_oam_loopback_t *loopback_ptr) 
{
    uint8 found=0;
    int rv = BCM_E_NONE;
    bcm_sat_gtf_t gtf_id = 0;
    bcm_sat_gtf_packet_config_t config;
    bcm_pkt_t *sat_header = NULL;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_SAT_GTF_ENTRY gtf_info;
    uint32  gtf_seq_number_offset;
    int offset = 0;
    int flag = 0;
    int is_mem_free = 0;
    int i = 0, priority = 0;
    int i2 =0;
    uint32 gtf_packet_length_pattern[BCM_SAT_GTF_NUM_OF_PRIORITIES][BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7}};
        
    BCMDNX_INIT_FUNC_DEFS;
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
    }

   if (loopback_ptr->flags & BCM_OAM_LOOPBACK_WITH_GTF_ID){  /* user request a specify gtf_id*/
      gtf_id = loopback_ptr->gtf_id ; 
      rv = bcm_common_sat_gtf_create(unit, BCM_SAT_GTF_WITH_ID,&gtf_id);
      BCMDNX_IF_ERR_EXIT(rv);
   }
   else if(loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE){
       gtf_id = loopback_ptr->gtf_id ; 
       rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, gtf_id, &gtf_info, &found);
       BCMDNX_IF_ERR_EXIT(rv);
       if (!found) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ctf_id %d not found."), gtf_id));
       }
   }
   else {
       rv = bcm_common_sat_gtf_create(unit, 0,&gtf_id);
       BCMDNX_IF_ERR_EXIT(rv);
   }
   /*loopback_ptr->gtf_id = gtf_id; */
   loopback_ptr->gtf_id = gtf_id; /* save gtf_id */
   gtf_info.flag = 1;/*1:lb enalbe / 2:tst enable */
   gtf_info.mepid= loopback_ptr->id;
   
    /* Configure header info need to free this malloc memery*/
    rv = bcm_pkt_alloc(unit, 127, 0, &sat_header); 
    if(rv == BCM_E_NONE){
        is_mem_free = 1;
    }else{
        BCMDNX_IF_ERR_EXIT(rv);
    }

   
    /* flag 1:LBM,0:TST*/
    flag = 1;
    /*build tst header*/
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_lb_tst_header_set, ( unit, sat_header->pkt_data->data,loopback_ptr->id,loopback_ptr->peer_da_mac_address,flag,&offset));
    BCMDNX_IF_ERR_EXIT(rv);
    
    gtf_seq_number_offset = offset;
    
    /* Configure packet */
    bcm_sat_gtf_packet_config_t_init(&config);
    
    config.header_info.pkt_data = sat_header->pkt_data;    
    config.header_info.blk_count = 1;
    /*Configure header type*/
    config.sat_header_type = bcmSatHeaderY1731;
    
    /* add sequence no*/
    config.offsets.seq_number_offset = gtf_seq_number_offset; /* offset =internal header+OAM header */
    
    /*Loop for support 2 TLV in the future , currently LB just have one TLV, either Data TLV or Test TLV*/
    /*BCM_OAM_MAX_NUM_TLVS_FOR_LBM*/
    for(i=0; i< loopback_ptr->num_tlvs ; i++){
        /* insert data TLV header*/
        
        if(loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeData)
         {
            offset= _insert_oam_data_tlv_head_for_tst_lb(sat_header->pkt_data->data, loopback_ptr->tlvs[i].tlv_length, offset);
        }
        /* insert test TLV header*/
        else if((loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
             || (loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)
             || (loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithCRC)
             || (loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
         {
            offset= _insert_oam_test_tlv_head_for_tst_lb(sat_header->pkt_data->data, loopback_ptr->tlvs[i].tlv_length,loopback_ptr->tlvs[i].tlv_type, offset);
        }
        else{
            continue;
        }

        sat_header->pkt_data->len = offset;  /*whole packet( from internal header -to- TLV length and pattern type)- before TLV patten*/
        
          /* configure Test TLV payload type*/
          if((loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
              ||(loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC))
          {
              config.payload.payload_type = bcmSatPayloadPRBS; 
             /* pattern will generate by system,don't need to fill it */ 
          }
          else if((loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithCRC)
              ||(loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
          {
              config.payload.payload_type = bcmSatPayloadConstant4Bytes; /*default value?*/
              for (i = 0; i < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; i++) {
              config.payload.payload_pattern[i]=0; 
              }
          }
          
          /* configure Data TLV payload type and pattern */
          if((loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeData))
          {
              config.payload.payload_type = bcmSatPayloadConstant4Bytes; /*tst_ptr->tlv.four_byte_repeatable_pattern*/
              config.payload.payload_pattern[0] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern&0xFF000000)>>24;
              config.payload.payload_pattern[1] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern&0xFF0000)>>16;
              config.payload.payload_pattern[2] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern&0xFF00)>>8;
              config.payload.payload_pattern[3] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern&0xFF);
              config.payload.payload_pattern[4] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern&0xFF000000)>>24;
              config.payload.payload_pattern[5] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern&0xFF0000)>>16;
              config.payload.payload_pattern[6] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern&0xFF00)>>8;
              config.payload.payload_pattern[7] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern&0xFF);
          }
          gtf_info.tlv.tlv_type =loopback_ptr->tlvs[i].tlv_type;
          gtf_info.tlv.tlv_length=loopback_ptr->tlvs[i].tlv_length;
       }      
	   
       gtf_info.lb_tlv_num=loopback_ptr->num_tlvs;
       
       i = 0;
        /*config Whole packet length, all the packets have the same length*/
       for (priority = 0; priority < BCM_SAT_GTF_NUM_OF_PRIORITIES; priority++)
       {
           for (i2 = 0; i2 < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i2++) {
               config.packet_edit[priority].packet_length[i] =  offset+  loopback_ptr->tlvs[i].tlv_length -4;/*don't include CRC*/
           }
           
           for (i2 = 0; i2 < BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH; i2++) {
               config.packet_edit[priority].packet_length_pattern[i2] =  gtf_packet_length_pattern[0][i2];
           }
           
           config.packet_edit[priority].pattern_length = 1; 
           config.packet_edit[priority].number_of_ctfs = 1;
           /*add end tlv and CRC*/
           if((loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
           || (loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithCRC)){
               
               config.packet_edit[priority].flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
           }else{
              config.packet_edit[priority].flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
           }
       }

    rv = bcm_common_sat_gtf_packet_config(unit, gtf_id, &config);
    BCMDNX_IF_ERR_EXIT(rv);

    /*if ctf and gtf set successfully, need to set LB flag*/
    classifier_mep_entry.flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER;

    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, loopback_ptr->id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 
    
    if(loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE) {
        rv = _bcm_dpp_oam_sat_gtf_info_db_update(unit, gtf_id, &gtf_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else{
        rv = _bcm_dpp_oam_sat_gtf_info_db_insert(unit, gtf_id, &gtf_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
exit:
      if(1 == is_mem_free){
          bcm_pkt_free(unit, sat_header); 
      }

      BCMDNX_FUNC_RETURN;
}
/* Add an OAM loopback object */
int bcm_petra_oam_loopback_add(int unit, bcm_oam_loopback_t *loopback_ptr) {
    uint8 found;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 is_ok,  is_period_in_kpps;
    int dont_care;
    bcm_oam_endpoint_action_t action;
    int rv=0;
    uint32 msb_to_oui;
    uint32 lsb_to_nic;
    uint32 new_flags=0, remove_flags = 0;
    bcm_gport_t destinations[SOC_DPP_DEFS_MAX(NOF_CORES)];

    BCMDNX_INIT_FUNC_DEFS;

    /* Validation of input + classifier retrieval */
    rv = _bcm_oam_loopback_validity_check(unit,loopback_ptr,&classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Jericho SAT usage mode */
    if(SOC_IS_JERICHO(unit)){
         /* Step 1, create sat_CTF for RX  */
        rv = sat_loopback_add_rx(unit, loopback_ptr);
        BCMDNX_IF_ERR_EXIT(rv);
         
        /* Step 2, create sat_GTF for TX  */
       rv = sat_loopback_add_tx(unit, loopback_ptr);
       BCMDNX_IF_ERR_EXIT(rv);
    }
    else{
	/* LBR responses to LBMs*/
    if (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) { /* DA address profiles*/
        msb_to_oui = GET_MSB_FROM_MAC_ADDR(loopback_ptr->peer_da_mac_address);
        lsb_to_nic = GET_LSB_FROM_MAC_ADDR(loopback_ptr->peer_da_mac_address);

        rv = _manage_DA_with_existing_entries_loopback(unit, loopback_ptr->id, &classifier_mep_entry, msb_to_oui, lsb_to_nic, &is_ok);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!is_ok) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MAC adddress may not be changed")));
        }
    }

	/* Have the classifier redirect LBMs to the OAMP.*/    
	rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), destinations, &dont_care); 
	BCM_OAM_OPCODE_CLEAR_ALL(action);
    action.destination = destinations[0];
	BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM);
	SET_CLASSIFIER_ACTION_GENERAL(loopback_ptr->id, action,SOC_PPD_OAM_OPCODE_MAP_LBM ,classifier_mep_entry.mep_type );

    is_period_in_kpps = ((loopback_ptr->flags & BCM_OAM_LOOPBACK_PERIOD_IN_KPPS) == BCM_OAM_LOOPBACK_PERIOD_IN_KPPS);

	if (loopback_ptr->period != 0) { /* Use LBM functionality as well*/

        rv = _bcm_oam_loopback_acc_packet_generation_add(unit, loopback_ptr, &classifier_mep_entry, &new_flags);
        BCMDNX_IF_ERR_EXIT(rv);

		/* Have the classifier redirect LBRs to the OAMP.*/    
		BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBR);
		SET_CLASSIFIER_ACTION_GENERAL(loopback_ptr->id, action,SOC_PPD_OAM_OPCODE_MAP_LBM ,classifier_mep_entry.mep_type );

	} else  {
        if (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK) {
            /* This is a special case of UPDATE where the loopback mechanism is to be removed while the LBR mechanism is
               to remain.*/
            rv = _remove_loopback(unit,loopback_ptr->id, classifier_mep_entry.mep_type);
            BCMDNX_IF_ERR_EXIT(rv);
            remove_flags |= (SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK
                             | (is_period_in_kpps ? SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_LOOPBACK_PERIOD_IN_KPPS : 0));
        }
    }
    new_flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY;

	/* The classifier may have been updated in action_set(). get the latest version and then update.*/
	rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
	BCMDNX_IF_ERR_EXIT(rv);
	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
	}

	classifier_mep_entry.flags =  classifier_mep_entry.flags | (new_flags & ~remove_flags); /*stupid compiler doesn't know what associative means*/
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, loopback_ptr->id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 

 }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Gets TLV extension from an accelerated loopback object */
int _bcm_oam_loopback_acc_packet_tlv_get(int unit, bcm_oam_loopback_t *loopback_ptr,
                                         ARAD_PP_OAM_LOOPBACK_INFO *soc_loopback_info) {

    bcm_oam_tlv_t *tlv_ptr = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(loopback_ptr);
    BCMDNX_NULL_CHECK(soc_loopback_info);
    tlv_ptr = &(loopback_ptr->tlvs[0]);

    /* Retrieve Length */
    tlv_ptr->tlv_length = soc_loopback_info->tst_tlv_len;

    /* Retrieve type */
    switch (soc_loopback_info->tst_tlv_type) {
    case ARAD_PP_OAM_DATA_TST_TLV_NULL_NO_CRC:
        tlv_ptr->tlv_type = bcmOamTlvTypeTestNullWithoutCRC;
        break;
    case ARAD_PP_OAM_DATA_TST_TLV_NULL_WITH_CRC:
        tlv_ptr->tlv_type = bcmOamTlvTypeTestNullWithCRC;
        break;
    case ARAD_PP_OAM_DATA_TST_TLV_PRBS_NO_CRC:
        tlv_ptr->tlv_type = bcmOamTlvTypeTestPrbsWithoutCRC;
        break;
    case ARAD_PP_OAM_DATA_TST_TLV_PRBS_WITH_CRC:
        tlv_ptr->tlv_type = bcmOamTlvTypeTestPrbsWithCRC;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                            (_BSL_BCM_MSG("Shouldn't get here. Something is wrong")));
    }

    /*Set num_tlvs to the only supported value */
    loopback_ptr->num_tlvs = 1;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Gets the configuration and status of an accelerated loopback object */
int _bcm_oam_loopback_acc_packet_generation_get(int unit, bcm_oam_loopback_t *loopback_ptr,
                                                SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {

    int rv = 0;
    uint32 soc_sand_rv = SOC_E_NONE;
    ARAD_PP_OAM_LOOPBACK_INFO soc_loopback_info = {0};

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(loopback_ptr);
    BCMDNX_NULL_CHECK(classifier_mep_entry);

    soc_loopback_info.is_period_in_kpps =
        ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_LOOPBACK_PERIOD_IN_KPPS) != 0);

    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_loopback_get,(unit, &soc_loopback_info));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    loopback_ptr->flags = soc_loopback_info.is_period_in_kpps ? BCM_OAM_LOOPBACK_PERIOD_IN_KPPS : 0;
    loopback_ptr->period = soc_loopback_info.tx_period;
    loopback_ptr->rx_count = soc_loopback_info.rx_packet_count;
    loopback_ptr->tx_count = soc_loopback_info.tx_packet_count;
    loopback_ptr->drop_count = soc_loopback_info.discard_count;

    if (soc_loopback_info.has_tst_tlv) {
        rv = _bcm_oam_loopback_acc_packet_tlv_get(unit, loopback_ptr, &soc_loopback_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int sat_loopback_get(int unit, bcm_oam_loopback_t *loopback_ptr) 
{
     uint8 oam_is_init=0;
     uint8 found=0;
     int priority = 0;
     int rv = BCM_E_NONE;
     SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
     SOC_PPD_OAM_SAT_GTF_ENTRY gtf_info;
     SOC_PPD_OAM_SAT_CTF_ENTRY ctf_info;
     bcm_sat_ctf_stat_t stat;
     uint64 gtf_tx_count[2];
     uint64 total_tx;
     int index=0;
     BCMDNX_INIT_FUNC_DEFS;
     _BCM_OAM_ENABLED_GET(oam_is_init);
     
     if (!oam_is_init) {
          BCM_EXIT;
     }
     if (!SOC_IS_JERICHO(unit)) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
     }
    
    bcm_sat_ctf_stat_t_init(& stat);
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
    }
    
    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add LB TX&RX objext."), loopback_ptr->id));
    } 
    
    /*get ctf info from ctf DB*/
    for(index =0; index < 32; index++){
        rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, index, &ctf_info, &found);
        if(!found){
              continue;
        }
        else if(found && (ctf_info.flag == 1)){
            if(ctf_info.mepid== loopback_ptr->id){/* find it*/
                 loopback_ptr->ctf_id = index;
                 break;
            }
            else{/* other mep enable lb*/
                continue;
            }
        }
        else{
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    

    rv = bcm_common_sat_ctf_stat_get (unit, loopback_ptr->ctf_id , BCM_SAT_CTF_STAT_DO_NOT_CLR_ON_READ, &stat);
    BCMDNX_IF_ERR_EXIT(rv);
    COMPILER_64_ZERO(total_tx);
    COMPILER_64_TO_32_LO(loopback_ptr->rx_count,stat.received_packet_cnt);
    COMPILER_64_TO_32_LO(loopback_ptr->invalid_tlv_count,stat.err_packet_cnt);
    COMPILER_64_TO_32_LO(loopback_ptr->out_of_sequence,stat.out_of_order_packet_cnt);
    
    /*get gtf info from gtf DB*/
    for(index =0; index < 8; index++){
        rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, index, &gtf_info, &found);
        if(!found){
              continue;
        }
        else if(found && (gtf_info.flag == 1)){
            if(gtf_info.mepid== loopback_ptr->id){/* find it*/
                 loopback_ptr->gtf_id = index;
                 break;
            }
            else{/* other mep enable lb*/
                continue;
            }
        }
        else{
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    for (priority = 0; priority < 2; priority++) {
         rv = bcm_common_sat_gtf_stat_get (unit,
                  loopback_ptr->gtf_id,
                  priority,
                  0,
                  bcmSatGtfStatPacketCount,
                  &gtf_tx_count[priority]);
         BCMDNX_IF_ERR_EXIT(rv);
         COMPILER_64_ADD_64(total_tx,gtf_tx_count[priority]);
    }

    COMPILER_64_TO_32_LO(loopback_ptr->tx_count,total_tx);
    
    if( loopback_ptr->rx_count > loopback_ptr->tx_count){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("tx should lager than rx , so error must be happend for endpoint id %d."), loopback_ptr->id));
    }
    
    loopback_ptr->drop_count = loopback_ptr->tx_count -  loopback_ptr->rx_count; 
    
    loopback_ptr->num_tlvs = gtf_info.lb_tlv_num;
    if(loopback_ptr->num_tlvs == 1){
        loopback_ptr->tlvs[0].tlv_length = gtf_info.tlv.tlv_length;
        loopback_ptr->tlvs[0].tlv_type= gtf_info.tlv.tlv_type;
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}



/* Get an OAM loopback object */
int bcm_petra_oam_loopback_get(int unit, bcm_oam_loopback_t *loopback_ptr) {
    int rv=0;
	uint8 oam_is_init,found;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    BCMDNX_INIT_FUNC_DEFS;
    
    if ((!SOC_IS_ARADPLUS_A0(unit))&&(!SOC_IS_JERICHO(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+ and Jericho.")));
    }
	_BCM_OAM_ENABLED_GET(oam_is_init);
	if (!oam_is_init) {
		BCM_EXIT;
	}
     if(SOC_IS_JERICHO(unit)){
        rv = sat_loopback_get(unit, loopback_ptr);
        BCMDNX_IF_ERR_EXIT(rv);
     }
     else{
	rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
	BCMDNX_IF_ERR_EXIT(rv);
	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
	}

	if ( (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) ==0 ){
		BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Loopback session not associated with endpoint %d."), loopback_ptr->id));
	}

    if (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK) {
        rv = _bcm_oam_loopback_acc_packet_generation_get(unit, loopback_ptr, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }

	/* Now the DA address and the period (when applicable)*/
    if (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {
        _BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(loopback_ptr, loopback_ptr->id);
    }

    BCM_EXIT;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int sat_loopback_delete(int unit, bcm_oam_loopback_t *loopback_ptr) 
{
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    bcm_sat_ctf_t ctf_id = 0;
    bcm_sat_gtf_t gtf_id = 0;
    int trap_id=0;
    int trap_data=0;
    uint32 lb_trap_id=0;
    bcm_rx_trap_config_t trap_config;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_SAT_CTF_ENTRY ctf_info;
    SOC_PPD_OAM_SAT_GTF_ENTRY gtf_info;
    bcm_sat_ctf_identifier_t identifier;
    int tc = 0;
    int dp = 0; 
    int tc_max = 7;
    int dp_max =3;
    int index = 0;
	
     BCMDNX_INIT_FUNC_DEFS;
     _BCM_OAM_ENABLED_GET(oam_is_init);

     if (!oam_is_init) {
          BCM_EXIT;
     }
     
     if (!SOC_IS_JERICHO(unit)) {
     BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
     }
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
    }
    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add LB TX&RX objext."), loopback_ptr->id));
    } 

    /*clear trap config for LB*/
    rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_SAT0_LB, &lb_trap_id);
    trap_id= (int)lb_trap_id;
    
    BCMDNX_IF_ERR_EXIT(rv);
    bcm_rx_trap_config_t_init(&trap_config); 
    rv = bcm_petra_rx_trap_set(unit, trap_id, &trap_config);
    BCMDNX_IF_ERR_EXIT(rv);

    
   /*get ctf info from ctf DB*/
   for(index =0; index < 32; index++){
       rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, index, &ctf_info, &found);
       if(!found){
             continue;
       }
       else if(found && (ctf_info.flag == 1)){
           if(ctf_info.mepid== loopback_ptr->id){/* find it*/
                ctf_id = index;
                break;
           }
           else{/* other mep enable lb*/
               continue;
           }
       }
       else{
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }
    rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, ctf_id, &ctf_info, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ctf id %d information can't be found."), ctf_id));
    }
    /*get identifier for lb*/
    identifier.session_id = ctf_info.identifier_session_id;  /*oam ID*/
    identifier.trap_id = ctf_info.identifier_trap_id; 
    /* clean up */
    /* Set the CTF flow as invalid  */
    for(dp = 0; dp <= dp_max; dp++)
    {
        for(tc = 0; tc <= tc_max; tc++)
          {
            identifier.color = dp; 
            identifier.tc =tc;   
            rv = bcm_common_sat_ctf_identifier_unmap(unit, &identifier);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    rv = bcm_common_sat_ctf_trap_remove(unit, trap_id);
    BCMDNX_IF_ERR_EXIT(rv);
    
    classifier_mep_entry.session_map_flag--;
    if(classifier_mep_entry.session_map_flag == 0)/* no one using this map, so delete it*/
    {
        trap_data = loopback_ptr->id;   /*MEP_DB Index <= OAM_ID(14) <= PPH.Trap_Qualifier*/
        rv = bcm_common_sat_ctf_trap_data_to_session_unmap(unit,trap_data,0xff);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Destroy the CTF flow */
    rv = bcm_common_sat_ctf_destroy(unit, ctf_id);
    BCMDNX_IF_ERR_EXIT(rv);
    
    for(index =0; index < 8; index++){
        rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, index, &gtf_info, &found);
        if(!found){
              continue;
        }
        else if(found && (gtf_info.flag == 1)){
            if(gtf_info.mepid== loopback_ptr->id){/* find it*/
                 gtf_id = index;
                 break;
            }
            else{/* other mep enable lb*/
                continue;
            }
        }
        else{
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    /* Destroy the GTF Object */
    rv = bcm_sat_gtf_destroy(unit, gtf_id);
    BCMDNX_IF_ERR_EXIT(rv);
    
    
    /*if ctf and gtf delet successfully, need to clear LB flag*/
    classifier_mep_entry.flags &= (~SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER);

    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, loopback_ptr->id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 
    
    /*delete ctf db */
     rv =  _bcm_dpp_oam_sat_ctf_info_db_delete(unit, ctf_id);
     BCMDNX_IF_ERR_EXIT(rv); 
     
     /*delete gtf db */
     rv = _bcm_dpp_oam_sat_gtf_info_db_delete(unit, gtf_id);
     BCMDNX_IF_ERR_EXIT(rv); 
exit:
        BCMDNX_FUNC_RETURN;
}


/* Delete an OAM loopback object */
int bcm_petra_oam_loopback_delete(int unit, bcm_oam_loopback_t *loopback_ptr) {
	uint8 oam_is_init,found;
	int rv;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
	uint8 remove_flags =0;
    BCMDNX_INIT_FUNC_DEFS; 


	_BCM_OAM_ENABLED_GET(oam_is_init);
	if (!oam_is_init) {
		BCM_EXIT;
	}

    if ((!SOC_IS_ARADPLUS_A0(unit))&&(!SOC_IS_JERICHO(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+ and Jericho.")));
    }
    
   if(SOC_IS_JERICHO(unit)){
       rv = sat_loopback_delete(unit, loopback_ptr);
       BCMDNX_IF_ERR_EXIT(rv);
   }
   else{

	rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
	BCMDNX_IF_ERR_EXIT(rv);
	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
	}

	if  ( (classifier_mep_entry.flags &SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) ==0 ) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint with id %d does not have loopback session."), loopback_ptr->id));
	}


    rv = _remove_loopback_reply(unit, loopback_ptr->id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 

    if (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK) {
        rv = _remove_loopback(unit, loopback_ptr->id, classifier_mep_entry.mep_type);
        BCMDNX_IF_ERR_EXIT(rv); 
		remove_flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK;

    }
	remove_flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY;

	/*get the latest version and then update.*/
	rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
	BCMDNX_IF_ERR_EXIT(rv);
	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
	}
	classifier_mep_entry.flags &= ~remove_flags;

	rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, loopback_ptr->id, &classifier_mep_entry);
	BCMDNX_IF_ERR_EXIT(rv); 
   }
    
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_oam_ais_add(int unit, bcm_oam_ais_t * ais_ptr)
{
	uint8 oam_is_init,found;
	int rv;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
	uint32 correct_flags=0,lsb,msb, soc_sand_rv;
	int used_ais_id[1];
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile;
	SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
	int profile_indx, is_allocated;
	int eth1731_prof_indx, eth1731_was_previously_allocated;
	uint8 nic_profile, oui_profile;
	SOC_PPD_OAM_MA_NAME name={0};

	BCMDNX_INIT_FUNC_DEFS; 
    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }

    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile);
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, ais_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), ais_ptr->id));
    }

    if (classifier_mep_entry.mep_type != SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS defined only for Ethernet OAM.")));
    }

    if (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), ais_ptr->id));
    }

	rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(unit, ais_ptr->id, used_ais_id, &found);
    BCMDNX_IF_ERR_EXIT(rv);

	correct_flags |=  BCM_OAM_AIS_MULTICAST|BCM_OAM_AIS_WITH_AIS_ID |BCM_OAM_AIS_UPDATE;

    if (found && (ais_ptr->flags & BCM_OAM_AIS_UPDATE)==0 ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS entry exists. ais_add() may only be called with the UPDATE flag.")));
	}

    if (ais_ptr->flags & BCM_OAM_AIS_MULTICAST && !BCM_MAC_IS_ZERO(ais_ptr->peer_da_mac_address)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When MULTICAST flag is set peer_da_mac_address field may not be filled.")));
    }

    if (ais_ptr->level <classifier_mep_entry.md_level || ais_ptr->level >7) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS level must be greateer than the endpoint level and less than 8.")));
		}

	if (ais_ptr->period != BCM_OAM_ENDPOINT_CCM_PERIOD_1S &&  ais_ptr->period != BCM_OAM_ENDPOINT_CCM_PERIOD_1M) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS period must be 1 frame per second (1000) or one frame per minute (60000).")));
    }


    if ((ais_ptr->flags & BCM_OAM_AIS_WITH_AIS_ID) && ais_ptr->ais_id> SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When _WITH_AIS_ID flag is set id must be less than %d."), SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)));
		}


    if (ais_ptr->flags & ~correct_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("llegal flag combination set."))); 
    }
	/* prelimanery checks complete.*/

	if (ais_ptr->flags & BCM_OAM_AIS_UPDATE) {
		/* Destroy and reset, and nothing is lost.*/
		ais_ptr->flags &= ~BCM_OAM_AIS_UPDATE;
		rv = bcm_petra_oam_ais_delete(unit,ais_ptr);
		BCMDNX_IF_ERR_EXIT(rv);

		rv = bcm_petra_oam_ais_add(unit,ais_ptr);
        BCMDNX_IF_ERR_EXIT(rv); 

		ais_ptr->flags |= BCM_OAM_AIS_UPDATE;
    }

    if (ais_ptr->flags & BCM_OAM_AIS_MULTICAST) {
		/* set mac address*/
		_BCM_OAM_SET_CCM_GROUP_DA_MAC_ADDRESS(ais_ptr->peer_da_mac_address, ais_ptr->level);
    }

	/* Step 1: MEP profile and friends*/
    if (ais_ptr->period==BCM_OAM_ENDPOINT_CCM_PERIOD_1S) {
		/* period 1s uses opcode 0*/
		SOC_PPD_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(ais_ptr->period, 0 , eth1731_profile.opcode_0_rate);
	} else {
		/* period 1m uses opcode 1*/
		SOC_PPD_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(ais_ptr->period, 0 , eth1731_profile.opcode_1_rate);
		}

    lsb =  GET_LSB_FROM_MAC_ADDR(ais_ptr->peer_da_mac_address);
    msb =  GET_MSB_FROM_MAC_ADDR(ais_ptr->peer_da_mac_address); 

    rv = _bcm_dpp_am_template_oam_lmm_nic_tables_alloc(unit, 0/* flags*/, &lsb, &is_allocated, &profile_indx);
    BCMDNX_IF_ERR_EXIT(rv);
    nic_profile = profile_indx;

    rv = _bcm_dpp_am_template_oam_lmm_oui_tables_alloc(unit, 0/* flags*/, &msb, &is_allocated, &profile_indx);
    BCMDNX_IF_ERR_EXIT(rv);
    oui_profile =  profile_indx ; 

	eth1731_profile.lmm_da_oui_prof = profile_indx; /* oui_profile may be -1.*/

	rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_alloc(unit,&eth1731_profile,0 /*flags*/,&is_allocated,&eth1731_prof_indx);
    BCMDNX_IF_ERR_EXIT(rv);
	eth1731_was_previously_allocated = !is_allocated;

	/* Step 2: MEP DB*/
    /*allocate new entry*/
    if (ais_ptr->flags & BCM_OAM_AIS_WITH_AIS_ID) {
        rv = _bcm_dpp_bfd_mep_id_alloc(unit, SHR_RES_ALLOC_WITH_ID, (uint32 *)&(ais_ptr->ais_id));
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* search look for a short mep ID and if that fails try a long one.*/
        rv = bcm_dpp_am_oam_mep_id_short_alloc(unit, 0, (uint32 *)&(ais_ptr->ais_id));
        if (rv == BCM_E_RESOURCE) {
            /* No short mep ID, look for a long one*/
            rv = bcm_dpp_am_oam_mep_id_long_alloc(unit, 0, (uint32 *)&(ais_ptr->ais_id));
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, ais_ptr->id, &mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    mep_db_entry.ccm_interval = 0;
	mep_db_entry.mdl = ais_ptr->level;
    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, ais_ptr->ais_id, &mep_db_entry, 0 /* allocate ICC index - no*/, name /*name - affects only CCMs*/);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Set troika of profiles in the HW*/
    soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, ais_ptr->ais_id, eth1731_was_previously_allocated, eth1731_prof_indx, &eth1731_profile);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = soc_ppd_oam_oamp_set_oui_nic_registers(unit, ais_ptr->ais_id, msb, lsb, oui_profile, nic_profile);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

		
	/* Update the SW and we are done!*/
	rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_insert(unit, ais_ptr->id, ais_ptr->ais_id);
	BCMDNX_IF_ERR_EXIT(rv); 


exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_oam_ais_get(int unit, bcm_oam_ais_t * ais_ptr)
{
	int rv;
	int used_ais_id[1];
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile;
	SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
	int mep_prof;
	bcm_mac_t mcast_mac;
	uint8 found;
	uint32 soc_sand_rv;
	BCMDNX_INIT_FUNC_DEFS; 


	rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(unit, ais_ptr->id, used_ais_id, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS entry does not exist.")));
    }

	ais_ptr->ais_id = *used_ais_id;

	SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile);

    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, ais_ptr->ais_id, &mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

	ais_ptr->level = mep_db_entry.mdl;

	rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit,ais_ptr->ais_id,&mep_prof,&eth1731_profile);
			BCMDNX_IF_ERR_EXIT(rv);

    if (eth1731_profile.opcode_0_rate) {
		ais_ptr->period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    } else {
		ais_ptr->period = BCM_OAM_ENDPOINT_CCM_PERIOD_1M;
		}

	_BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(ais_ptr, ais_ptr->ais_id);

	_BCM_OAM_SET_CCM_GROUP_DA_MAC_ADDRESS(mcast_mac, ais_ptr->level);
    if (sal_memcmp(mcast_mac, ais_ptr->peer_da_mac_address,6)==0) {
		sal_memset(ais_ptr->peer_da_mac_address, 0, 6);
		ais_ptr->flags = BCM_OAM_AIS_MULTICAST;
    }


exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_ais_delete(int unit, bcm_oam_ais_t * ais_ptr)
{
	int rv;
	int used_ais_id[1];
	int mep_prof, is_last;
	uint8 found;
	uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS; 

	rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(unit, ais_ptr->id, used_ais_id, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS entry does not exist.")));
    }

	FREE_NIC_AND_OUI_PROFILES(*used_ais_id);
	    
    rv =  _bcm_dpp_am_template_oam_eth1731_mep_profile_free(unit, *used_ais_id,  &mep_prof, &is_last);
    BCMDNX_IF_ERR_EXIT(rv);
    CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last, mep_prof, *used_ais_id); 

    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_delete(unit, *used_ais_id, NULL /*mep_db_entry*/, 0 /* dealloc icc index */, 0 /*is last*/);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, *used_ais_id);
    BCMDNX_IF_ERR_EXIT(rv); 

	rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_delete_by_id(unit, ais_ptr->id);
    BCMDNX_IF_ERR_EXIT(rv); 


exit:
    BCMDNX_FUNC_RETURN;
}
/* Add an OAM TST RX object */ 
int bcm_petra_oam_tst_rx_add(int unit,bcm_oam_tst_rx_t *tst_ptr)
{
    uint8 oam_is_init,found;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv=0;
    int trap_id=0;
    uint32 tst_trap_id = 0;
    int trap_index = 0;
    int trap_data = 0;
    bcm_sat_ctf_t ctf_id = 0;
    bcm_sat_ctf_packet_info_t packet_info;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_SAT_CTF_ENTRY ctf_info;
    bcm_oam_endpoint_action_t action;
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)];
    int dont_care; 
    int index =0;
    bcm_sat_ctf_identifier_t identifier;
    int session_id=0;
    int tc = 0;
    int dp = 0; 
    int tc_max = 7;
    int dp_max =3;


    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
         BCM_EXIT;
    }
    if (!SOC_IS_JERICHO(unit)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }

    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
   
    /*get entry of mep db*/
    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, tst_ptr->endpoint_id, &mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
    }
    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), tst_ptr->endpoint_id));
    }

    if ((!(tst_ptr->flags & BCM_OAM_TST_RX_UPDATE))&&(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Endpoint %d has already added tst RX objext."), tst_ptr->endpoint_id));
    } 
    if((tst_ptr->flags & BCM_OAM_TST_RX_UPDATE)&&(!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER))){
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d  tst RX objext don't exist ."), tst_ptr->endpoint_id));
    }

    if((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == 0)/* Down mep */
    {

       /******  Down MEP Classification    *****/
       rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_SAT0_TST, &tst_trap_id);
       BCMDNX_IF_ERR_EXIT(rv);
       trap_id= (int)tst_trap_id;

       bcm_rx_trap_config_t_init(&trap_config); 
       
       /*2. Set the destination in the trap to be the OAMP*/
       rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &dont_care);
       BCMDNX_IF_ERR_EXIT(rv);
       /*And then*/
       trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
       trap_config.dest_port = oamp_port[0];
       /*3.  Update the FWD header:*/
       trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
       trap_config.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
       
       rv = bcm_petra_rx_trap_set(unit, trap_id, &trap_config);
       BCMDNX_IF_ERR_EXIT(rv);
       
       BCM_OAM_ACTION_CLEAR_ALL(action);
       BCM_OAM_OPCODE_CLEAR_ALL(action);

       /*action.destination = oamp_port[0];*/
       
       
       /*4 Call the API endpoint_action_set.The destination should be set set to the trap code above:*/
       BCM_GPORT_TRAP_SET(action.destination, trap_id, 7, 0); /*Taken from default values*/
       
       /*Action should be:*/
       if(mep_db_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM){
           BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd);
       }else if((mep_db_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP)
                   ||(mep_db_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)) {
           BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd);
       } else {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("TST just support in ETH / MPLS/PWE")));
       }
       /*Opcode should be LBR or TST */
       BCM_OAM_OPCODE_SET(action, 37); /* 37-TST*/
       /*After calling bcm_oam_endpoint_action_set with the above sequence, Down MEP packets will be redirected to the SAT.*/
       rv = bcm_petra_oam_endpoint_action_set(unit,tst_ptr->endpoint_id,&action);
       BCMDNX_IF_ERR_EXIT(rv);
    }
    else 
    {
       
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("currently just support Down MEP")));
    }


       /******  CTF configuration.    *****/
       if(tst_ptr->flags & BCM_OAM_TST_RX_WITH_ID)  /* if flag is update, user should give the cft_id*/
        {
           ctf_id = tst_ptr->ctf_id;
           /* Create a CTF */
           rv = bcm_common_sat_ctf_create(unit, BCM_SAT_CTF_WITH_ID, &ctf_id);
           BCMDNX_IF_ERR_EXIT(rv);
       }
       else if(tst_ptr->flags & BCM_OAM_TST_RX_UPDATE) /* flag is update, don't need to create a new Ctf*/
       {
           ctf_id = tst_ptr->ctf_id;
           rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, ctf_id, &ctf_info, &found);
           BCMDNX_IF_ERR_EXIT(rv);
           if (!found) {
               BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ctf_id %d not found."), ctf_id));
           }
       }
       else
       {
           /* Create a CTF */
           rv = bcm_common_sat_ctf_create(unit, 0, &ctf_id);
           BCMDNX_IF_ERR_EXIT(rv);
       }
       
       tst_ptr->ctf_id= ctf_id; /* save ctf_id */
       ctf_info.flag = 2;  /* 2:tst enable */
       ctf_info.mepid= tst_ptr->endpoint_id;
       
       if(!(tst_ptr->flags & BCM_OAM_TST_RX_UPDATE)){        
         rv = bcm_common_sat_ctf_trap_add(unit, trap_id);
         BCMDNX_IF_ERR_EXIT(rv);

         trap_data = tst_ptr->endpoint_id;/*o   MEP_DB Index <= OAM_ID(14) <= PPH.Trap_Qualifier*/

         /*(1) map trap qualifier and mask to session ID */
         for(session_id=0;session_id <16;session_id++){
              rv = bcm_common_sat_ctf_trap_data_to_session_map(unit, trap_data, 0xff, session_id/*tst_ptr->endpoint_id*/);/*OAM-ID is endpoint ID and session ID*/
              if((rv == BCM_E_NONE)||(rv == BCM_E_EXISTS))
              {
                  break;
              }
              else
              {
                  BCMDNX_IF_ERR_EXIT(rv);
              }
         }
         classifier_mep_entry.session_map_flag++;
         
         rv = bcm_common_sat_trap_idx_get(unit, trap_id, &trap_index);
         BCMDNX_IF_ERR_EXIT(rv);
         
         /*(2)  Map identification(four content) to specific flow-id */
         
         bcm_sat_ctf_identifier_t_init(&identifier); 
         for(dp = 0; dp <= dp_max; dp++)
         {
             for(tc = 0; tc <= tc_max; tc++)
               {
                 identifier.session_id = session_id;  
                 identifier.trap_id = trap_index; 
                 identifier.color = dp; 
                 identifier.tc =tc;   
                 rv = bcm_common_sat_ctf_identifier_map(unit, &identifier, ctf_id);
                 BCMDNX_IF_ERR_EXIT(rv);
             }
         }
         /*save identifier for tst*/
         ctf_info.identifier_session_id = session_id;  /*oam ID*/
         ctf_info.identifier_trap_id = trap_index; /* trap idx 1-3 */
         
       }       
       
       /* Set the recieved packet information */
       bcm_sat_ctf_packet_info_t_init(&packet_info);
       packet_info.sat_header_type = bcmSatHeaderY1731;
       
       /*  Seq no offset set 4, should be the same with GTF send packet */
       packet_info.offsets.seq_number_offset = 4;
       
       if(tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeData){
           packet_info.offsets.payload_offset = 11;
       }else {
           packet_info.offsets.payload_offset = 12; /* for test tlv*/
       }
       
       /* Test TLV */
       if((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
           || (tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)){
             packet_info.payload.payload_type = bcmSatPayloadPRBS;
       }else if((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)
           ||(tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestNullWithoutCRC)) {
             packet_info.payload.payload_type = bcmSatPayloadConstant4Bytes; 
          for (index = 0; index < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index++) {
               packet_info.payload.payload_pattern[index]=0; 
           }
       }
       
       /* Check End TLV & CRC */
       if((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
         || (tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)){
           packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
       }
       else
       {
           packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
       }
       rv = bcm_common_sat_ctf_packet_config(unit, ctf_id, &packet_info);
       BCMDNX_IF_ERR_EXIT(rv);

       
       /*if ctf set successfully, need to set TST RX flag*/
       classifier_mep_entry.flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER;
       ctf_info.tlv.tlv_type =tst_ptr->expected_tlv.tlv_type;
       ctf_info.tlv.tlv_length=tst_ptr->expected_tlv.tlv_length;
       
      /*update sw db mep_info with new mep */
      rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, tst_ptr->endpoint_id, &classifier_mep_entry);
      BCMDNX_IF_ERR_EXIT(rv); 

      if(tst_ptr->flags & BCM_OAM_TST_RX_UPDATE) {
          rv = _bcm_dpp_oam_sat_ctf_info_db_update(unit, ctf_id, &ctf_info);
          BCMDNX_IF_ERR_EXIT(rv);
      }
      else{
          rv = _bcm_dpp_oam_sat_ctf_info_db_insert(unit, ctf_id, &ctf_info);
          BCMDNX_IF_ERR_EXIT(rv);
      }
exit:
    BCMDNX_FUNC_RETURN;
}


/* Get an OAM TST RX object */ 
int bcm_petra_oam_tst_rx_get(int unit,bcm_oam_tst_rx_t *tst_ptr)
{
     uint8 oam_is_init=0;
     uint8 found=0;
     int rv = BCM_E_NONE;
     SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
     SOC_PPD_OAM_SAT_CTF_ENTRY ctf_info;
     bcm_sat_ctf_stat_t stat;
     int index = 0;
     
     BCMDNX_INIT_FUNC_DEFS;
     _BCM_OAM_ENABLED_GET(oam_is_init);
     
     if (!oam_is_init) {
          BCM_EXIT;
     }
     if (!SOC_IS_JERICHO(unit)) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
     }
    
    bcm_sat_ctf_stat_t_init(& stat);
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
    }
    
    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add tst rx objext."), tst_ptr->endpoint_id));
    } 
    
    /*get ctf info from ctf DB*/
    for(index =0; index < 32; index++){
       rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, index, &ctf_info, &found);
       if(!found){
             continue;
       }
       else if(found && (ctf_info.flag == 2)){
           if(ctf_info.mepid== tst_ptr->endpoint_id){/* find it*/
                tst_ptr->ctf_id = index;
                break;
           }
           else{/* other mep enable lb*/
               continue;
           }
       }
       else{
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }
    tst_ptr->expected_tlv.tlv_type = ctf_info.tlv.tlv_type;
    tst_ptr->expected_tlv.tlv_length = ctf_info.tlv.tlv_length;
    
    rv = bcm_sat_ctf_stat_get (unit, tst_ptr->ctf_id , BCM_SAT_CTF_STAT_DO_NOT_CLR_ON_READ, &stat);
    BCMDNX_IF_ERR_EXIT(rv);
    
    tst_ptr->rx_count = stat.received_packet_cnt; 
    tst_ptr->invalid_tlv_count =  stat.err_packet_cnt;
    tst_ptr->out_of_sequence = stat.out_of_order_packet_cnt;    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Delete an OAM TST RX object */ 
int bcm_petra_oam_tst_rx_delete(int unit, bcm_oam_tst_rx_t *tst_ptr)
{
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    int trap_id=0;
    int trap_data = 0;
    uint32 tst_trap_id = 0;
    bcm_rx_trap_config_t trap_config;
    bcm_sat_ctf_t ctf_id = 0;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_SAT_CTF_ENTRY ctf_info;
    bcm_sat_ctf_identifier_t identifier;
    int tc = 0;
    int dp = 0; 
    int tc_max = 7;
    int dp_max =3;
    int index = 0;
    
     BCMDNX_INIT_FUNC_DEFS;
     _BCM_OAM_ENABLED_GET(oam_is_init);

     if (!oam_is_init) {
          BCM_EXIT;
     }
     
     if (!SOC_IS_JERICHO(unit)) {
     BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
     }
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
    }
    
    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't enable tst rx objext."), tst_ptr->endpoint_id));
    } 
    
    /*clear trap config for tst*/
    rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_SAT0_TST, &tst_trap_id);
    BCMDNX_IF_ERR_EXIT(rv);
    
    trap_id= (int)tst_trap_id;
    
    bcm_rx_trap_config_t_init(&trap_config); 
    rv = bcm_petra_rx_trap_set(unit, trap_id, &trap_config);
    BCMDNX_IF_ERR_EXIT(rv);

    classifier_mep_entry.session_map_flag--;
    if(classifier_mep_entry.session_map_flag == 0)/* no one using this map*/
    {
        trap_data = tst_ptr->endpoint_id;   /*MEP_DB Index <= OAM_ID(14) <= PPH.Trap_Qualifier*/
        rv = bcm_common_sat_ctf_trap_data_to_session_unmap(unit,trap_data,0xff);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    /*get ctf info from ctf DB*/
    for(index =0; index < 32; index++){
       rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, index, &ctf_info, &found);
       if(!found){
             continue;
       }
       else if(found && (ctf_info.flag == 2)){
           if(ctf_info.mepid== tst_ptr->endpoint_id){/* find it*/
                ctf_id = index;
                break;
           }
           else{/* other mep enable lb*/
               continue;
           }
       }
       else{
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }
    
    /*get identifier for tst*/
    identifier.session_id = ctf_info.identifier_session_id;  /*oam ID*/
    identifier.trap_id = ctf_info.identifier_trap_id; 

    
    
    /* clean up */
    /* Set the CTF flow as invalid  */
    for(dp = 0; dp <= dp_max; dp++)
    {
        for(tc = 0; tc <= tc_max; tc++)
        {
            identifier.color = dp; 
            identifier.tc =tc;   
            rv = bcm_common_sat_ctf_identifier_unmap(unit, &identifier);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);

    /* Destroy the CTF flow */
    rv = bcm_common_sat_ctf_destroy(unit, ctf_id);
    BCMDNX_IF_ERR_EXIT(rv);
    
    rv = bcm_common_sat_ctf_trap_remove(unit, trap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /*if ctf and ctf delet successfully, need to clear tst rx flag*/
    classifier_mep_entry.flags &= (~SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER);
    
    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, tst_ptr->endpoint_id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 
    /*delete ctf db */
    rv =  _bcm_dpp_oam_sat_ctf_info_db_delete(unit, ctf_id);
    BCMDNX_IF_ERR_EXIT(rv); 
exit:
    BCMDNX_FUNC_RETURN;
}


/* Add an OAM TST TX object */ 
int bcm_petra_oam_tst_tx_add(int unit, bcm_oam_tst_tx_t *tst_ptr)
{
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    bcm_sat_gtf_t gtf_id = 0;
    bcm_sat_gtf_packet_config_t config;
    bcm_pkt_t *sat_header = NULL;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_SAT_GTF_ENTRY gtf_info;
    uint32  gtf_seq_number_offset;
    int offset = 0;
    int i = 0,priority=0;
    int is_mem_free = 0;
    int flag = 0;
    uint32 gtf_packet_length_pattern[BCM_SAT_GTF_NUM_OF_PRIORITIES][BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7}};
        
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
         BCM_EXIT;
    }
    
    if (!SOC_IS_JERICHO(unit)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }

   /* Configure header info*/
   rv = bcm_pkt_alloc(unit, 127, 0, &sat_header); 
   if(rv == BCM_E_NONE){
       is_mem_free = 1;
   }else{
       BCMDNX_IF_ERR_EXIT(rv);
   }

   rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
   BCMDNX_IF_ERR_EXIT(rv);
   
   if (!found) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
   }
   if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), tst_ptr->endpoint_id));
   }
   if ((!(tst_ptr->flags & BCM_OAM_TST_TX_UPDATE))&&(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Endpoint %d has already add tst Tx objext."), tst_ptr->endpoint_id));
   } 
   if((tst_ptr->flags & BCM_OAM_TST_TX_UPDATE)&&(!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER))){
       BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d  tst Tx objext don't exist ."), tst_ptr->endpoint_id));
   }

   if(tst_ptr->flags & BCM_OAM_TST_TX_WITH_ID){  /* user request a specify gtf_id*/
       gtf_id = tst_ptr->gtf_id;
       rv = bcm_common_sat_gtf_create(unit, BCM_SAT_GTF_WITH_ID,&gtf_id);
       BCMDNX_IF_ERR_EXIT(rv);
   }
   else if(tst_ptr->flags & BCM_OAM_TST_TX_UPDATE){/*flag is update, don't need to create a new gtf*/
       gtf_id = tst_ptr->gtf_id;
       rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, gtf_id, &gtf_info, &found);
       BCMDNX_IF_ERR_EXIT(rv);
       if (!found) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("gtf_id %d not found."), gtf_id));
       }
    }
   else {
        rv = bcm_common_sat_gtf_create(unit, 0,&gtf_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
   
    /*tst_ptr->gtf_id = gtf_id;*/
    tst_ptr->gtf_id = gtf_id; /* save gtf_id */
    
    gtf_info.flag = 2;  /* 2:tst enable */
    gtf_info.mepid= tst_ptr->endpoint_id;
    
    /* flag 1:LBM,0:TST*/
    flag = 0;
    /*build tst header*/
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_lb_tst_header_set, ( unit, sat_header->pkt_data->data,tst_ptr->endpoint_id,tst_ptr->peer_da_mac_address,flag,&offset));
    BCMDNX_IF_ERR_EXIT(rv);
    
    gtf_seq_number_offset = offset;
    /* Configure packet */
    bcm_sat_gtf_packet_config_t_init(&config);
    
    config.header_info.pkt_data = sat_header->pkt_data;    
    config.header_info.blk_count = 1; /* ? */

    /*Configure header type*/
    config.sat_header_type = bcmSatHeaderY1731;

    /* add sequence no*/
    config.offsets.seq_number_offset = gtf_seq_number_offset; /* offset =internal header+OAM header */
    
    /* insert data TLV header*/
    if(tst_ptr->tlv.tlv_type == bcmOamTlvTypeData)
    {
        offset= _insert_oam_data_tlv_head_for_tst_lb(sat_header->pkt_data->data, tst_ptr->tlv.tlv_length, offset);
    }
    /* insert test TLV header*/
    else if((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
    || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)
    || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)
    || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
    {
        offset= _insert_oam_test_tlv_head_for_tst_lb(sat_header->pkt_data->data, tst_ptr->tlv.tlv_length,tst_ptr->tlv.tlv_type, offset);
    }
    
    sat_header->pkt_data->len = offset;  /*whole packet( from internal header -to- TLV length and pattern type)- before TLV patten*/

  
    /* configure Test TLV payload type*/
    if((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
        ||(tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC))
    {
        config.payload.payload_type = bcmSatPayloadPRBS; 
       /* pattern will generate by system,don't need to fill it */ 
    }
    else if((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)
        ||(tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
    {
        config.payload.payload_type = bcmSatPayloadConstant4Bytes; /*default value?*/
        for (i = 0; i < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; i++) {
        config.payload.payload_pattern[i]=0; 
        }
    }
    else /* TST can only have Test TLV */
    {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d ,the TLV bype of  tst Tx was wrong ."), tst_ptr->endpoint_id));
    }
    
    /*config Whole packet length, all the packets have the same length*/
    for (priority = 0; priority < BCM_SAT_GTF_NUM_OF_PRIORITIES; priority++)
    {
        /*config Whole packet length, all the packets have the same length*/
        for (i = 0; i < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++) {
           config.packet_edit[priority].packet_length[i] =  offset+  tst_ptr->tlv.tlv_length -4 ; /*don't include CRC*/
        }
        for (i = 0; i < BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH; i++) {
           config.packet_edit[priority].packet_length_pattern[i] =  gtf_packet_length_pattern[0][i];;
        }
        config.packet_edit[priority].pattern_length = 1; /*1*/
        config.packet_edit[priority].number_of_ctfs = 1;
        /*add end tlv and CRC*/
        if((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
        || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)){
            
            config.packet_edit[priority].flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
        }else{
            config.packet_edit[priority].flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
        }
    }
    rv = bcm_sat_gtf_packet_config(unit, gtf_id, &config);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /*if gtf set successfully, need to set TST TX flag*/
    classifier_mep_entry.flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER;
    gtf_info.tlv.tlv_type =tst_ptr->tlv.tlv_type;
    gtf_info.tlv.tlv_length=tst_ptr->tlv.tlv_length;
    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, tst_ptr->endpoint_id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 
    
    if(tst_ptr->flags & BCM_OAM_TST_TX_UPDATE) {
        rv = _bcm_dpp_oam_sat_gtf_info_db_update(unit, gtf_id, &gtf_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else{
        rv = _bcm_dpp_oam_sat_gtf_info_db_insert(unit, gtf_id, &gtf_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
exit:
    if(1 == is_mem_free){
        bcm_pkt_free(unit, sat_header); 
    }
    BCMDNX_FUNC_RETURN;
}
/* Get an OAM TST TX object */ 
int bcm_petra_oam_tst_tx_get(int unit,bcm_oam_tst_tx_t *tst_ptr) 
{
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_SAT_GTF_ENTRY gtf_info;
    int index = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
        BCM_EXIT;
    }
    if (!SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
    }
    
    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add tst tx objext."), tst_ptr->endpoint_id));
    } 
    
    /*get ctf info from ctf DB*/
    for(index =0; index < 8; index++){
       rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, index, &gtf_info, &found);
       if(!found){
             continue;
       }
       else if(found && (gtf_info.flag == 2)){
           if(gtf_info.mepid== tst_ptr->endpoint_id){/* find it*/
                tst_ptr->gtf_id = index;
                break;
           }
           else{/* other mep enable lb*/
               continue;
           }
       }
       else{
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }
    tst_ptr->tlv.tlv_type = gtf_info.tlv.tlv_type;
    tst_ptr->tlv.tlv_length = gtf_info.tlv.tlv_length;

exit:
   BCMDNX_FUNC_RETURN;

}


/* Delete an OAM TST TX object */ 
int bcm_petra_oam_tst_tx_delete(int unit,bcm_oam_tst_tx_t *tst_ptr) 
{
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    bcm_sat_gtf_t gtf_id = 0;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_SAT_GTF_ENTRY gtf_info;
    int index = 0;
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
        BCM_EXIT;
    }
    if (!SOC_IS_JERICHO(unit)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
    }
    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't enable tst Tx objext."), tst_ptr->endpoint_id));
    } 
    
    /*get gtf info from gtf DB*/
    for(index =0; index < 8; index++){
       rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, index, &gtf_info, &found);
       if(!found){
             continue;
       }
       else if(found && (gtf_info.flag == 2)){
           if(gtf_info.mepid== tst_ptr->endpoint_id){/* find it*/
                gtf_id = index;
                break;
           }
           else{/* other mep enable lb*/
               continue;
           }
       }
       else{
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }
    /* Destroy the GTF Object */
    rv = bcm_sat_gtf_destroy(unit, gtf_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /*if gtf delet successfully, need to clear tst tx flag*/
    classifier_mep_entry.flags &= (~SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER);

    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, tst_ptr->endpoint_id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 

   /*delete gtf db */
    rv = _bcm_dpp_oam_sat_gtf_info_db_delete(unit, gtf_id);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;

}


/****************************************************
  OAM Control
 ****************************************************/

int
  _bcm_dpp_oam_control_get(
    int unit,
    bcm_oam_control_type_t type,
    uint64 *arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    switch (type) {
    case bcmOamControlReportMode:
        {
            if (SOC_IS_JERICHO(unit)) {
                SOC_PPC_OAM_REPORT_MODE mode;
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_report_mode_get, (unit, &mode));
                BCMDNX_IF_ERR_EXIT(rv);

                switch (mode) {
                case SOC_PPC_OAM_REPORT_MODE_COMPACT:
                    COMPILER_64_SET(*arg, 0, bcmOamReportModeTypeCompact);
                    break;
                case SOC_PPC_OAM_REPORT_MODE_NORMAL:
                    COMPILER_64_SET(*arg, 0, bcmOamReportModeTypeNormal);
                    break;
                case SOC_PPC_OAM_REPORT_MODE_RAW:
                    COMPILER_64_SET(*arg, 0, bcmOamReportModeTypeRaw);
                    break;
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Internal error")));
                }
            }
            else
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm oam control type")));

        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm oam control type")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_control_get(
    int unit,
    bcm_oam_control_type_t type,
    uint64 *arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_control_get(unit, type, arg);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_dpp_oam_control_set(
    int unit,
    bcm_oam_control_type_t type,
    uint64 arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    switch (type) {
    case bcmOamControlReportMode:
        {
            if (SOC_IS_JERICHO(unit)) {
                SOC_PPC_OAM_REPORT_MODE mode;
                uint32 arg_lw = COMPILER_64_LO(arg);
                switch (arg_lw) {
                case bcmOamReportModeTypeCompact:
                    mode = SOC_PPC_OAM_REPORT_MODE_COMPACT;
                    break;
                case bcmOamReportModeTypeNormal:
                    mode = SOC_PPC_OAM_REPORT_MODE_NORMAL;
                    break;
                case bcmOamReportModeTypeRaw:
                    /*mode = SOC_PPC_OAM_REPORT_MODE_RAW;
                    break;*/ /* Not supported */
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported bcm oam report mode")));
                }
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_report_mode_set, (unit, mode));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm oam control type")));

        }
        break;
    case bcmOamControl1588ToD:
        {
            uint32 soc_sand_rv;
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_tod_set, (unit, 0, arg));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);         }
        break;
    case bcmOamControlNtpToD:
        {
            uint32 soc_sand_rv;
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_tod_set, (unit, 1, arg));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;

    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm oam control type")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_control_set(
    int unit,
    bcm_oam_control_type_t type,
    uint64 arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_control_set(unit, type, arg);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/* ***************************************************************/
/********************OAM DIGNOSTICS SECTION************************/
/* **************************************************************/



/* Function recieves a endpoint info presumed to be remote and prints it information.
   Second parameter is the local endpoint. If none is given function will search for the local endpoint.*/
void bcm_oam_remote_endpoint_print(int unit, bcm_oam_endpoint_info_t *rmep_info, bcm_oam_endpoint_info_t *local_endpoint) {
    uint32 ccm_ms, ccm_micro_s;
    BCMDNX_INIT_FUNC_DEFS;
    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\t\t***Remote endpoint. ID: 0X%x name: %d \n"), rmep_info->id, rmep_info->name));

    if (rmep_info->ccm_period == 0) {
        if (local_endpoint == NULL) {
            SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
            SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
            BCM_SAND_IF_ERR_EXIT(soc_ppd_oam_oamp_mep_db_entry_get(unit, rmep_info->local_id, &mep_db_entry));
            SOC_PPD_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_ms, ccm_micro_s, mep_db_entry.ccm_interval);
            MULTIPLE_BY_3_5(ccm_ms, ccm_micro_s);
        } else {
            ccm_ms = local_endpoint->ccm_period;
            ccm_micro_s = 0;
            MULTIPLE_BY_3_5(ccm_ms, ccm_micro_s);
        }
    } else {
        ccm_ms =  rmep_info->ccm_period;
        ccm_micro_s = 0;
    }
    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\t\tCCM period (in MS): %d.%.2u "), ccm_ms, ccm_micro_s));
    if (rmep_info->loc_clear_threshold) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "Loss of continuity clear threshold: %d \n"), rmep_info->loc_clear_threshold));
    } else {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\t\tAutomatic clearing of of loss of continuity disabled.\n")));
    }
    if (rmep_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_UPDATE) {
        if (rmep_info->port_state & BCM_OAM_PORT_TLV_BLOCKED) {
            LOG_CLI((BSL_META("\t\tPort state - blocked\n")));
        }
        else if (rmep_info->port_state & BCM_OAM_PORT_TLV_UP) {
            LOG_CLI((BSL_META("\t\ttPort state - up\n")));
        }
    }
    if (rmep_info->faults & BCM_OAM_ENDPOINT_FAULT_REMOTE) {
           LOG_CLI((BSL_META("\t\tAn RDI has been received\n")));
    }
    else if (rmep_info->faults & BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT) {
        LOG_CLI((BSL_META("\t\tRMEP is in LOC state\n")));
    }
    if (rmep_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE) {
        if (rmep_info->interface_state  & BCM_OAM_INTERFACE_TLV_UP) {
            LOG_CLI((BSL_META("\t\tInterface is up\n")));
        }
        else if (rmep_info->interface_state  & BCM_OAM_INTERFACE_TLV_DOWN) {
            LOG_CLI((BSL_META("\t\tInterface is down\n")));
        }
        else if (rmep_info->interface_state  & BCM_OAM_INTERFACE_TLV_DORMANT) {
            LOG_CLI((BSL_META("\t\tInterface is dormant\n")));
        }
        else if (rmep_info->interface_state  & BCM_OAM_INTERFACE_TLV_TESTING) {
            LOG_CLI((BSL_META("\t\tInterface is testing\n")));
        }
        else if (rmep_info->interface_state  & BCM_OAM_INTERFACE_TLV_UNKNOWN) {
            LOG_CLI((BSL_META("\t\tInterface is unknown\n")));
        }
        else if (rmep_info->interface_state  & BCM_OAM_INTERFACE_TLV_NOTPRESENT) {
            LOG_CLI((BSL_META("\t\tInterface is not present\n")));
        }
        else if (rmep_info->interface_state  & BCM_OAM_INTERFACE_TLV_LLDOWN) {
            LOG_CLI((BSL_META("\t\tInterface is LL down\n")));
        }
    }
    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\t\t-----------\n")));
exit:
    BCMDNX_FUNC_RETURN_VOID
}

/*Function prints fields that are relevant to oam over mpls only*/
void print_oamOmpls_information(bcm_oam_endpoint_info_t *endpoint_info) {
    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META("\n\t Interface ID: %d\n\t"), endpoint_info->intf_id));
    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META("Innermost MPLS label encapsulating the OAM BPDU: ")));
    bcm_dpp_print_mpls_egress_label(&(endpoint_info->egress_label));
}


int bcm_oam_endpoint_info_diag_print(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    ENDPOINT_LIST_PTR rmep_list_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR cur_member = 0 ;
    ENDPOINT_LIST_MEMBER_PTR member_next ;
    bcm_oam_endpoint_info_t rmep_info;
    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;

    /*first, print the information about this endpoint*/
    if (endpoint_info->id == -1) { /*default mep*/
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "=====Default endpoint. ID: -1\n\t***Properties:\n")));
    } else {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "=====Endpoint ID: 0X%x Group: %d \n\t***Properties:\n"), endpoint_info->id,  endpoint_info->group));
    }


    if (_BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info)) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tMaintenence entity is intermediate\n")));
    } else if (endpoint_info->type == bcmOAMEndpointTypeEthernet ) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tEndpoint direction: ")));
        if (_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "Up facing\n")));
        } else {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "Down facing\n")));
        }
    }
    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\tType: ")));
    switch (endpoint_info->type) {
    case bcmOAMEndpointTypeEthernet:
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "OAM over Ethernet"))); break;
    case bcmOAMEndpointTypeBHHMPLS:
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "OAM type BHH MPLS")));
        print_oamOmpls_information(endpoint_info);
        break;
    case bcmOAMEndpointTypeBHHPwe:
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "OAM type BHH - PWE")));
        print_oamOmpls_information(endpoint_info);
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type.")));
    }


    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\n\tEndpoint Level: %d"), endpoint_info->level));

    if (endpoint_info->lm_flags & BCM_OAM_LM_PCP) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tAn 8-tuple of LM counters are set. ")));
    }
    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\n\tLoss management counter base id: %d\n"), endpoint_info->lm_counter_base_id));

    if(!(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE))
    {
        bcm_oam_group_info_t group_info;
        int rv, i;
        char group_name[BCM_OAM_GROUP_NAME_LENGTH*2 + 1];

        rv = bcm_oam_group_get(unit, endpoint_info->group, &group_info);
        BCMDNX_IF_ERR_EXIT(rv);

        for(i=0; i<BCM_OAM_GROUP_NAME_LENGTH; i++) {
            sprintf(group_name, "%02x", group_info.name[i]);
        }
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tGroup id: %d Group name: %s\n"), endpoint_info->group, group_name));
    }

    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\tGport 0X%x"), endpoint_info->gport));
    if (endpoint_info->mpls_out_gport!=BCM_GPORT_INVALID) {
    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\tMPLS out gport 0X%x"), endpoint_info->mpls_out_gport));
    }

    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\tDestination mac address: " MAC_PRINT_FMT),
              MAC_PRINT_ARG(endpoint_info->dst_mac_address)));

    if (_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tEndpoint is accelerated in HW. TX fields:\n")));
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tName: %d, TX gport: 0X%x, "), endpoint_info->name, endpoint_info->tx_gport));
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             MAC_PRINT_FMT),
                  MAC_PRINT_ARG(endpoint_info->src_mac_address)));
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_TX) {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "\tUpdate and transmit RDI")));
        }
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tCCM period is %d. COS of outgoing CCMs: %d\n"), endpoint_info->ccm_period, endpoint_info->int_pri));
        if (endpoint_info->vlan) {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "\tVlan information: vlan: %d, packet priority: %d, TPID: 0x%x\n"), endpoint_info->vlan, endpoint_info->pkt_pri,
                                 endpoint_info->outer_tpid));

            if (endpoint_info->inner_vlan) {
                LOG_INFO(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "\tInner vlan information: vlan: %d, packet priority: %d, tpid: %d\n"), endpoint_info->inner_vlan,
                                     endpoint_info->inner_pkt_pri, endpoint_info->inner_tpid));
            }
        }
        if (SOC_IS_ARADPLUS(unit)){
            /* look for additional entries in the MEP-DB*/
            uint8  has_dm;
            uint32 entry = 0, soc_sand_rv;
            soc_sand_rv = soc_ppd_oam_oamp_next_index_get(unit, endpoint_info->id, &entry, &has_dm);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
            if ((entry == endpoint_info->id + 2 && !has_dm) || (entry == endpoint_info->id + 3 && has_dm) ) {
                LOG_CLI((BSL_META_U(unit,
                                                    "MEP has additional LM entry.\n")));
            } else if ( (entry == endpoint_info->id + 3 && !has_dm) || entry == endpoint_info->id + 4 ) {
                LOG_CLI((BSL_META_U(unit,
                                                    "MEP has additional LM, LM STAT entry.\n")));
            } 
            if (has_dm) {
                LOG_CLI((BSL_META_U(unit,
                                                    "MEP has additional DM entry.\n")));
            }

            if (SOC_IS_JERICHO(unit)) {
                SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
                int rv;
                int dont_care[1];
                uint8 found;

                rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info->id, &classifier_mep_entry, &found);
                BCMDNX_IF_ERR_EXIT(rv); 

                if (!found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), endpoint_info->id));
                }
            
                if ((classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE) !=0)  {
                LOG_CLI((BSL_META_U(unit,
                                                    "On demand DM session active.\n")));
                }

                rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(unit, endpoint_info->id, dont_care, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (found) {
                    bcm_oam_ais_t ais;
                    ais.id = endpoint_info->id;
                    rv = bcm_oam_ais_get(unit, &ais); 
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (ais.flags & BCM_OAM_AIS_MULTICAST) {
                        LOG_CLI((BSL_META_U(unit, "Multicast ")));
                    }
                    LOG_CLI((BSL_META_U(unit,"AIS frames are being transmitted with period one frame per %s.\n"), (ais.period == 1000)? "second" : "minute" )); 
                }

            }
        }

    } else {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tEndpoint is not accelerated . TX fields unavailable.\n")));
    }

    /*now getting the endpoint's rmeps and printing them*/
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint_info->id, &rmep_list_p, &found));
    if (found)
    {
        int rv ;

        rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, rmep_list_p, &cur_member) ;
        BCMDNX_IF_ERR_EXIT(rv);

        while (cur_member)
        {
            uint32 member_index ;

            bcm_oam_endpoint_info_t_init(&rmep_info) ;
            rv = _bcm_dpp_oam_endpoint_list_get_member_index(unit, cur_member, &member_index) ;
            BCMDNX_IF_ERR_EXIT(rv) ;
            rmep_info.id = (bcm_oam_endpoint_t)member_index ;
            bcm_oam_endpoint_get(unit, rmep_info.id, &rmep_info);
            bcm_oam_remote_endpoint_print(unit, &rmep_info, endpoint_info);

            rv = _bcm_dpp_oam_endpoint_list_get_member_next(unit, cur_member, &member_next) ;
            BCMDNX_IF_ERR_EXIT(rv) ;
            cur_member = member_next ;
        }
    }
    else
    {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\t\tEndpoint has no associated remote MEPS")));
    }


    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\n")));
exit:
    BCMDNX_FUNC_RETURN
}
/*
 * callback used for diagnostics. Find an endpoint, determine if it is bfd or oam, 
 * then call proper function to print information. 
 * 
 */
int
_bcm_oam_diag_iterate_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t user_data) {
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_bfd_endpoint_info_t bfd_endpoint;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *mep_entry =  (SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *)user_data;
    BCMDNX_INIT_FUNC_DEFS;

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint);
    bcm_oam_endpoint_info_t_init(&endpoint_info);

    /*get the endpoint_info*/
    if ( (*(int*)key != -2  ) && ((mep_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) || (mep_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
        (mep_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE) ) ) { /*found an oam endpoint*/
        endpoint_info.id = *(int *)key;
        BCMDNX_IF_ERR_EXIT(bcm_oam_endpoint_get(unit, endpoint_info.id, &endpoint_info));
        BCMDNX_IF_ERR_EXIT(bcm_oam_endpoint_info_diag_print(unit, &endpoint_info));
    } else { /*try looking for bfd endpoints*/
        bfd_endpoint.id = (*(int *)key == -2) ? -1: *(int *)key;
        BCMDNX_IF_ERR_EXIT(bcm_bfd_endpoint_get(unit, bfd_endpoint.id, &bfd_endpoint));
        bcm_dpp_bfd_endpoint_diag_print(&bfd_endpoint);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/************ Diagnostics functions ********************/

/* "BCM.0>diag oam endpoints" initiates the following function.
   Function prints information on all endpoints and their rmep.*/
int _bcm_dpp_oam_bfd_diagnostics_endpoints(int unit) {
    uint8 oam_is_init, bfd_is_init;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    (void)rv; /*rv is used in macros under some compilations*/

    _BCM_OAM_ENABLED_GET(oam_is_init);
    _BCM_BFD_ENABLED_GET(bfd_is_init);
    if (!oam_is_init && !bfd_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "===================================\n")));
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_sw_db_hash_oam_bfd_mep_info_iterate(unit, _bcm_oam_diag_iterate_cb));

exit:
    BCMDNX_FUNC_RETURN;
}


/* "BCM.0>diag oam endpoints id=<endpoint_id>" initiates the following function.
   Function prints information endpoint_ID and its rmeps if applicable.*/
int _bcm_dpp_oam_bfd_diagnostics_endpoint_by_id(int unit, int endpoint_id) {
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_bfd_endpoint_info_t bfd_endpoint;
    uint8 found;
    uint8 oam_is_init, bfd_is_init; 
    int rv;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    BCMDNX_INIT_FUNC_DEFS;


    _BCM_OAM_ENABLED_GET(oam_is_init);
    _BCM_BFD_ENABLED_GET(bfd_is_init);
    if (!oam_is_init && !bfd_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    bcm_oam_endpoint_info_t_init(&endpoint_info);
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_id, &mep_info, &found));
    if (!found) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
    }

    if ((mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) || (mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
        (mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)) { /*found an oam endpoint*/
        rv = bcm_petra_oam_endpoint_get(unit, endpoint_id, &endpoint_info);
        BCMDNX_IF_ERR_EXIT(rv);
        if (endpoint_info.flags & BCM_OAM_ENDPOINT_REMOTE) {
            bcm_oam_remote_endpoint_print(unit, &endpoint_info, NULL);
        } else {
            BCMDNX_IF_ERR_EXIT(bcm_oam_endpoint_info_diag_print(unit, &endpoint_info));
        }
    } else { /*found a bfd endpoint*/
        rv = bcm_petra_bfd_endpoint_get(unit, endpoint_id, &bfd_endpoint);
        BCMDNX_IF_ERR_EXIT(rv);
        bcm_dpp_bfd_endpoint_diag_print(&bfd_endpoint);
    }

exit:
    BCMDNX_FUNC_RETURN;
}





int _bcm_dpp_oam_bfd_diagnostics_LM_counters(int unit, int endpoint_id) {
    uint8 found;
    int rv;
    uint32 eg_val, ing_val; 
	int ingress_index = -1, egress_index=-1;
    uint8 oam_is_init, bfd_is_init;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    int soc_port;
    SOC_TMC_CNT_SRC_TYPE src_type;
    unsigned int command_id;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    _BCM_BFD_ENABLED_GET(bfd_is_init);
    if (!oam_is_init && !bfd_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_id, &mep_info, &found);
    if ( (!found) || (!((mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) || (mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
        (mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE))) ) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
    }

 
    for (soc_port =0; soc_port < SOC_DPP_DEFS_GET(unit, nof_counter_processors) ; ++soc_port) {
        rv = bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_source, soc_port, &src_type);
        BCMDNX_IF_ERR_EXIT(rv);

        if (src_type == SOC_TMC_CNT_SRC_TYPE_OAM) {
            rv = bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_command_id, soc_port, &command_id);
            BCMDNX_IF_ERR_EXIT(rv);

            if (command_id == 0) {
                /*soc_port is an egress oam port*/
                egress_index = soc_port;
            } else if (command_id == 1) {
                /*soc_port is an ingress oam port*/
                ingress_index = soc_port;
            }
        }
    }

    if ((ingress_index == -1) || (egress_index == -1)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("soc pretperty get failed")));
    }

    rv = soc_ppd_oam_get_crps_counter(unit, egress_index, mep_info.counter, &eg_val);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = soc_ppd_oam_get_crps_counter(unit, ingress_index, mep_info.counter, &ing_val);
    BCMDNX_IF_ERR_EXIT(rv); 

    if (eg_val) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "Egress counter: %d.\t"), eg_val));
    } else {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "Egress counter inactive.\t")));
    }
    if (ing_val) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "Ingress counter: %d.\n"), ing_val));
    } else {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "Ingress counter inactive.\n")));
    }

      


exit:
    BCMDNX_FUNC_RETURN;
}
