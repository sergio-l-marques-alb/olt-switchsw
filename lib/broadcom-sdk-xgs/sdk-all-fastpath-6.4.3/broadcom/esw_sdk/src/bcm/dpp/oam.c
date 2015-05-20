/*
 * $Id: oam.c,v 1.148 Broadcom SDK $
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
 *
 * File:    oam.c
 * Purpose: Manages L3 interface table, forwarding table, routing table
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_OAM
#include <shared/bsl.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/bfd.h>
#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/port.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_eg_mirror.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/dpp_wb_engine.h>
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_interrupts.h>
#endif /*BCM_ARAD_SUPPORT*/

#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/oam.h>

#include <shared/gport.h>
#include <shared/shr_resmgr.h>

#include <soc/drv.h>
#include <soc/enet.h>
#include <soc/defs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>

/***************************************************************/
/***************************************************************/

/*
 * Struct used to wrap callback for group together with its parameter. 
 * Used in  bcm_petra_oam_group_traverse( )
*/
typedef struct bcm_oam_group_traverse_cb_w_param_s {
    bcm_oam_group_traverse_cb cb;
    void* user_data;
}bcm_oam_group_traverse_cb_w_param_t;

/*
 * Struct used to wrap callback for endpoint together with its parameter. 
 * Used in  bcm_petra_oam_endpoint_traverse( )
*/
typedef struct bcm_oam_endpoint_traverse_cb_w_param_s {
    bcm_oam_endpoint_traverse_cb cb;
    bcm_oam_group_t group;
    void* user_data;
}bcm_oam_endpoint_traverse_cb_w_param_t;

/*
 * Struct used to wrap callback for endpoint together with its parameter. 
 * Used in  bcm_petra_oam_endpoint_create( )
*/
typedef struct bcm_oam_endpoint_traverse_cb_profile_info {
    uint32 lif;
	uint32 first_endpoint_id; /* first endpoint id of given lif which we meet during traverse */
    uint8 is_ingress;
    uint8 is_alloced; /* profile was allocated during the traverse */
    uint8 is_last; /* profile was freed during the traverse */
	SOC_PPD_OAM_LIF_PROFILE_DATA * profile_data;
}bcm_oam_endpoint_traverse_cb_profile_info_t;



/*struct should not exceed size 4 (in bytes). 
  Used to store data on all events. from the FIFO.*/
typedef struct {
    uint16 rmep_indx ; /* index in the RMEP DB (not the actual RMEP ID)*/
    uint8 is_oam;
    uint8 event_number ; /* number from the bcm_bfd/oam_event_type_t enum*/
} _bcm_oam_event_and_rmep_info;



/*
 * Global defines
 *
 */


/* hash tables */
shr_htb_hash_table_t _bcm_dpp_oam_ma_index_to_name_db[BCM_MAX_NUM_UNITS];
shr_htb_hash_table_t _bcm_dpp_oam_ma_to_mep_db[BCM_MAX_NUM_UNITS];
shr_htb_hash_table_t _bcm_dpp_oam_rmep_info_db[BCM_MAX_NUM_UNITS];
shr_htb_hash_table_t _bcm_dpp_oam_bfd_mep_info_db[BCM_MAX_NUM_UNITS];
shr_htb_hash_table_t _bcm_dpp_oam_bfd_mep_to_rmep_db[BCM_MAX_NUM_UNITS];
/* Used in Jericho only*/
shr_htb_hash_table_t _bcm_dpp_oam_mep_to_ais_id[BCM_MAX_NUM_UNITS];

/* Callbacks are not supported by warmboot. Need to re-register after warmboot */
/* OAM */
static bcm_oam_event_cb _g_oam_event_cb[BCM_MAX_NUM_UNITS][bcmOAMEventCount];
/* User data */
static void*            _g_oam_event_ud[BCM_MAX_NUM_UNITS][bcmOAMEventCount];
/* BFD */
static bcm_bfd_event_cb _g_bfd_event_cb[BCM_MAX_NUM_UNITS][bcmBFDEventCount];
/* User data */
static void*            _g_bfd_event_ud[BCM_MAX_NUM_UNITS][bcmBFDEventCount];




/* warmboot supported - use SOC_DPP_WB_ENGINE macros to SET variables */
SOC_PPD_OAM_INIT_TRAP_INFO _bcm_dpp_oam_trap_info[SOC_MAX_NUM_DEVICES]; /* Using only in init and detach (with _bcm_dpp_oam_trap_info_wb_add afterwards)*/
SOC_PPD_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP _bcm_oam_cpu_trap_code_to_mirror_profile_map[SOC_MAX_NUM_DEVICES]; /* Using only when sending to soc */

/* global variables used in bcm_petra_oam_(endpoint|group)_traverse( ) */
static bcm_oam_group_traverse_cb_w_param_t _group_cb_with_param;

/***************************************************************/
/***************************************************************/

/*
 * Local defines
 *
 */

#define _BCM_OAM_MA_NAME_WITHOUT_HEADERS 13
#define _BCM_OAM_RMEP_ID_MAX (1<<13)-1 /* rmep id is 13 bits */
#define _BCM_OAM_MEP_PER_MA_MAX_NUM 256

#define _BCM_OAM_PASSIVE_PROFILE 3

/**
 * Snoop commands used for advanced egress snooping. 
 */
#define _BCM_OAM_MIP_SNOOP_CMND_DOWN 1
#define _BCM_OAM_MIP_SNOOP_CMND_UP 2


typedef struct bcm_oam_ma_name_s {
	uint8 is_short;
	uint8 name[_BCM_OAM_MA_NAME_WITHOUT_HEADERS];
} bcm_oam_ma_name_t;

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

#define _BCM_OAM_RX_TRAP_TYPE_TO_PPD_TRAP_TYPE(trap_type, ppd_trap_type) \
  do {   \
	 switch (trap_type) {   \
		case bcmRxTrapOampTrapErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_TRAP_ERR;   \
			break;   \
		case bcmRxTrapOampTypeErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_TYPE_ERR;   \
			break;   \
		case bcmRxTrapOampRmepErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_RMEP_ERR;   \
			break;   \
		case bcmRxTrapOampMaidErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_MAID_ERR;   \
			break;   \
		case bcmRxTrapOampMdlErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_MDL_ERR;  \
			break;   \
		case bcmRxTrapOampCcmIntrvErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_CCM_INTERVAL_ERR;  \
			break;   \
		case bcmRxTrapOampMyDiscErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_MY_DISC_ERR;   \
			break;   \
		case bcmRxTrapOampSrcIpErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_SRC_IP_ERR;   \
			break;   \
		case bcmRxTrapOampYourDiscErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_YOUR_DISC_ERR;   \
			break;   \
		case bcmRxTrapOampSrcPortErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_SRC_PORT_ERR;   \
			break;   \
		case bcmRxTrapOampRmepStateChange:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_RMEP_STATE_CHANGE;  \
			break;   \
		case bcmRxTrapOampParityErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_PARITY_ERR;  \
			break;   \
		case bcmRxTrapOampTimestampErr:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_TIMESTAMP_ERR;  \
			break;   \
		case bcmRxTrapOampProtection:   \
			ppd_trap_type = SOC_PPD_OAM_OAMP_PROTECTION;  \
			break;   \
		default:   \
			BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OAMP trap type is not supported")));   \
	}   \
  } while (0) 


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
		default:   \
			BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Event ID is not supported")));   \
	}   \
  } while (0) 

#define _BCM_OAM_PPD_FROM_BCM_EVENT_MEP(ppd_event, event_type, is_oam) \
  do {   \
     switch (event_type) {   \
            case bcmOAMEventEndpointCCMTimeout: soc_ppd_event_type = SOC_PPD_OAM_EVENT_LOC_SET; break;   \
	    case bcmOAMEventEndpointCCMTimeoutEarly: soc_ppd_event_type = SOC_PPD_OAM_EVENT_ALMOST_LOC_SET; break;   \
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

#define BCM_OAM_OAMP_DESTINATION_TRAP_CODE_GET(is_upmep, mep_type, destination_trap_code) \
  do {   \
	if (is_upmep) { /* Can be only Eth. OAM */   \
			rv = SOC_DPP_WB_ENGINE_GET_ARR(unit,    \
									 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_UPMEP_TRAP_IDS,    \
									 &destination_trap_code,    \
									 SOC_PPD_OAM_UPMEP_TRAP_ID_OAMP);   \
			BCMDNX_IF_ERR_EXIT(rv);   \
    }   \
	else {   \
		if (mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {   \
			rv = SOC_DPP_WB_ENGINE_GET_ARR(unit,    \
									 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS,    \
									 &destination_trap_code,    \
									 SOC_PPD_OAM_TRAP_ID_OAMP);   \
			BCMDNX_IF_ERR_EXIT(rv);   \
		}   \
		else if (mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) {   \
			rv = SOC_DPP_WB_ENGINE_GET_ARR(unit,    \
									 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS,    \
									 &destination_trap_code,    \
									 SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_MPLS);   \
			BCMDNX_IF_ERR_EXIT(rv);   \
		}   \
		else /*(classifier_mep_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)*/ {   \
			rv = SOC_DPP_WB_ENGINE_GET_ARR(unit,    \
									 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS,    \
									 &destination_trap_code,    \
									 SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_PWE);   \
			BCMDNX_IF_ERR_EXIT(rv);   \
		}   \
	}   \
  } while (0) 

#define BCM_OAM_CPU_DESTINATION_TRAP_CODE_GET(is_upmep, meta_data, destination_trap_code) \
  do {   \
	if (is_upmep) { \
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit,    \
                                 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_UPMEP_TRAP_IDS,    \
                                 &destination_trap_code,    \
                                 SOC_PPD_OAM_UPMEP_TRAP_ID_CPU);   \
        BCMDNX_IF_ERR_EXIT(rv);   \
    }    \
	else { \
       rv = SOC_DPP_WB_ENGINE_GET_ARR(unit,    \
									 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS,    \
									 &destination_trap_code,    \
									 SOC_PPD_OAM_TRAP_ID_CPU);   \
	   BCMDNX_IF_ERR_EXIT(rv);   \
    }   \
  } while (0) 


/* When upmep oam id is to be added to the FHEI, opcode is defined to include meta-data*/
/* Currently all opcodes include meta-data. This macro remains nonetheless */
#define _BCM_OAM_IS_OPCODE_WITH_META_DATA(opcode)           1                            


#define _BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_id, cpu_trap_code)                                                                                      \
  do {																										                          \
	 int dont_care, mirror_id_int;\
    rv = _bcm_dpp_oam_free_mirror_profile_find(unit, &mirror_id, *(cpu_trap_code));											                          \
	BCMDNX_IF_ERR_EXIT(rv);										                                        	                          \
   mirror_id_int = mirror_id;\
	if (_bcm_dpp_am_template_mirror_action_profile_alloc(unit, SHR_RES_ALLOC_WITH_ID,cpu_trap_code,&dont_care,  &mirror_id_int) != BCM_E_NONE) {                                               \
		BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Error: Failed to allocate mirroring destination\n")));		                          \
	}																										                          \
  } while (0)

#define _BCM_OAM_MIRROR_PROFILE_DEALLOC(mirror_profile_dest)                                                   \
  do {																										   \
  int dont_care;\
	if (_bcm_dpp_am_template_mirror_action_profile_free(unit, mirror_profile_dest, &dont_care) != BCM_E_NONE) {                                    \
		BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Error: Failed to deallocate mirroring destination\n")));       \
	}																										   \
  } while (0)

#define _BCM_OAM_TRAP_CODE_UPMEP_FTMH(trap_code_upmep)      \
  ((trap_code_upmep == SOC_PPD_TRAP_CODE_OAM_CPU_MIRROR) || \
  (trap_code_upmep == SOC_PPD_TRAP_CODE_OAM_OAMP_MIRROR)  )

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
		rv = SOC_DPP_WB_ENGINE_GET_ARR(unit,                         				\
							 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS,\
							 &trap_id_err_level,                       				\
							 SOC_PPD_OAM_TRAP_ID_ERR_LEVEL);                      	\
	    BCMDNX_IF_ERR_EXIT(rv);								               		    \
		profile_data.opcode_to_trap_code_unicast_map[opcode] = trap_id_err_level;	\
	}  														               		    \
	if (BCM_OAM_ACTION_GET(*action, bcmOAMActionMcDrop)) {           				\
		rv = SOC_DPP_WB_ENGINE_GET_ARR(unit,                         				\
							 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS,\
							 &trap_id_err_level,                       				\
							 SOC_PPD_OAM_TRAP_ID_ERR_LEVEL);                      	\
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
 * Following 2 macros translate the field lm_counter_base_id to 
 * and from the counter base set on the O-EM1 entry (and in the 
 * classifier_mep_entry struct). The PCP flags are also handled.
 */
#define _BCM_OAM_COUNTER_BASE_ID_TO_CLASSIFIER_COUNTER(endpoint_info_ptr, classifier_mep_ptr)\
do {\
    if (endpoint_info_ptr->lm_counter_base_id) {\
        uint32 actual_counter= endpoint_info_ptr->lm_counter_base_id;\
        if ((endpoint_info_ptr->lm_flags & BCM_OAM_LM_PCP) != 0) {\
			classifier_mep_ptr->flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_PCP;\
            actual_counter >>=3; /* The HW shifts this back*/\
        }\
		_BCM_PETRA_PROC_CNTR_FROM_STAT(proc_dummy, classifier_mep_ptr->counter, actual_counter); \
    } else {\
        classifier_mep_ptr->counter=0;\
    }\
} while (0) 


#define _BCM_OAM_COUNTER_BASE_ID_FROM_CLASSIFIER_COUNTER(endpoint_info_ptr, classifier_mep_ptr)\
do {\
    if (classifier_mep_ptr->counter == 0) {\
        endpoint_info_ptr->lm_counter_base_id = 0;\
    }\
    else {\
        _BCM_PETRA_STAT_FROM_PROC_CNTR(endpoint_info_ptr->lm_counter_base_id, 0/*counter_proc (irrelevant)*/, classifier_mep_ptr->counter);\
        if ((classifier_mep_ptr->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_PCP) != 0) {\
            endpoint_info_ptr->lm_counter_base_id <<=3; /* The returned value in this case must be shifted in order to match the endpoint_create procedure.*/\
            endpoint_info_ptr->lm_flags = BCM_OAM_LM_PCP;\
        }\
    }\
} while (0) 


/**
 * Insert an event to the event hash table. 
 * If specific event has already been inserted, update the data 
 * to signify multiple events 
 */
#define  INSERT_EVENT_INTO_HTBL(htbl,eventt ) \
    do {\
        rv = shr_htb_insert(htbl,  (shr_htb_key_t)&eventt, (shr_htb_data_t) 0);\
        if (rv == _SHR_E_EXISTS) {\
            int dont_care;\
            rv = shr_htb_find(htbl, (shr_htb_key_t)&eventt,(shr_htb_data_t) &dont_care, 1 /* remove */ );\
            BCMDNX_IF_ERR_EXIT(rv);\
            rv = shr_htb_insert(htbl, (shr_htb_key_t)&eventt, (shr_htb_data_t)BCM_OAM_EVENT_FLAGS_MULTIPLE);\
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
                     
/**
 * Set the port_status, interface_status along with approriate 
 * flags in endpoint_info accirdingly to the remote_state. 
 * The mapping is {PortState[0:1], InterfaceState[2:0]} 
 */
#define _BCM_DPP_OAM_RMEP_STATE_TO_PORT_INTERFACE_STATUS(remote_state, endpoint_info)\
do {\
	if ((remote_state) & 0x7) {\
		(endpoint_info)->flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;\
		(endpoint_info)->interface_state = (remote_state) & 0x7;\
	}\
	if (((remote_state)>>3) & 0x3) {\
		(endpoint_info)->flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;\
		(endpoint_info)->port_state = ((remote_state)>>3) & 0x3;\
	}\
} while (0)

#define _BCM_DPP_OAM_RMEP_STATE_FROM_PORT_INTERFACE_STATUS(remote_state, endpoint_info)\
do {\
        (remote_state) = (((endpoint_info)->port_state << 3) | ((endpoint_info)->interface_state));\
} while (0)


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
        BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMR);\
        BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM);\
        if (!SHR_BITGET(counter_disable, SOC_PPD_OAM_OPCODE_MAP_DMM)) {\
            BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable); \
        }\
        if (!SHR_BITGET(meter_disable, SOC_PPD_OAM_OPCODE_MAP_DMM)) {\
        BCM_OAM_ACTION_SET(action, bcmOAMActionMeterEnable); \
        }\
        is_piggy_backed =0;\
    } else {\
	    int meter_counter_opcode = is_piggy_backed? SOC_PPD_OAM_OPCODE_MAP_CCM : SOC_PPD_OAM_OPCODE_MAP_LMM;\
        BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM);\
        BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR);\
        if (!SHR_BITGET(counter_disable, meter_counter_opcode)) {\
            BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable); \
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
		rv = _bcm_dpp_am_template_oam_lmm_nic_tables_alloc(unit, SHR_RES_ALLOC_WITH_ID, &_lsb, &dont_care, &_nic_profile);\
		BCMDNX_IF_ERR_EXIT(rv);\
		rv = _bcm_dpp_am_template_oam_lmm_oui_tables_alloc(unit, SHR_RES_ALLOC_WITH_ID, &_msb, &dont_care, &_oui_profile);\
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
 * MPLS/PWE: GAL, GACH, MPLS/PWE (4B each), offset in OAM PDU 
 * (4B). 
 * In case of piggy backed CCMs the offset increases by 54  
 * (sequence #) + 2 (MEP-ID) + 48 (MEG-ID)
 *  */ 
#define _OAM_SET_LMM_OFFSET(mep_db_entry, value, is_piggy_back, is_slm) \
    do {value = (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) ? mep_db_entry.tags_num *4 + 0x12 : \
     (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP)? 0x10 : 0x10 /*PWE offset */;\
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



/* May also return error*/
#define _BCM_OAM_BCM_MEP_TYPE_TO_MEP_DB_MEP_TYPE(bcm_mep_type, mep_db_mep_type) \
 	switch (bcm_mep_type) {\
	case bcmOAMEndpointTypeEthernet:\
		mep_db_mep_type = SOC_PPD_OAM_MEP_TYPE_ETH_OAM;\
		break;\
	case bcmOAMEndpointTypeBHHMPLS:\
		mep_db_mep_type = SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP ;\
		break;\
	case bcmOAMEndpointTypeBHHPwe:\
		mep_db_mep_type = SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE;\
		break;\
	default:\
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type.")));\
	}\

#define _BCM_OAM_IS_SERVER_CLIENT(endpoint_info) (endpoint_info->tx_gport==BCM_GPORT_INVALID && endpoint_info->remote_gport !=BCM_GPORT_INVALID)
#define _BCM_OAM_IS_SERVER_SERVER(endpoint_info) (endpoint_info->tx_gport!=BCM_GPORT_INVALID && endpoint_info->remote_gport !=BCM_GPORT_INVALID)
#define _BCM_OAM_IS_MEP_ID_DEFAULT(_unit, _epid)                                \
               (SOC_IS_ARAD_B1_AND_BELOW(_unit)                                 \
                  ? ((_epid) == -1)                                             \
                  : (SOC_IS_ARADPLUS_AND_BELOW(_unit)                           \
                      ? (((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS0)         \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS1)       \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS2)       \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS3)       \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS0))       \
/*Jericho and above*/ : (((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS0)         \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS1)      \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS2)      \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS3)      \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS0)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS1)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS2)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS3))))


/***************************************************************/
/***************************************************************/
/*
 * OAM Module Helper functions
 * 
 *
 */

/* CPU common trap to oam and bfd */
int _bcm_oam_bfd_common_cpu_trap_code_set(uint32 unit, uint32 trap_code) {
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
	SOC_PPD_OAM_INIT_TRAP_INFO_clear(&_bcm_dpp_oam_trap_info[unit]);
    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS, &trap_code, SOC_PPD_OAM_TRAP_ID_CPU);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_oam_bfd_common_cpu_trap_code_get(uint32 unit, uint32 * trap_code) {
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS, trap_code, SOC_PPD_OAM_TRAP_ID_CPU);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* traps_ref_counter functions */
int 
_bcm_dpp_oam_traps_ref_counter_increase(int unit, uint32 trap_code) {
	int traps_ref_counter;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
	rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAPS_REF_COUNTER, &traps_ref_counter, trap_code);
    BCMDNX_IF_ERR_EXIT(rv);
	traps_ref_counter++;
	rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAPS_REF_COUNTER, &traps_ref_counter, trap_code);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_oam_traps_ref_counter_decrease(int unit, uint32 trap_code) {
	int traps_ref_counter;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
	rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAPS_REF_COUNTER, &traps_ref_counter, trap_code);
    BCMDNX_IF_ERR_EXIT(rv);
	traps_ref_counter--;
	rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAPS_REF_COUNTER, &traps_ref_counter, trap_code);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_oam_traps_ref_counter_is_zero(int unit, uint32 trap_code, uint8 * is_zero) {
	int traps_ref_counter;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
	rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAPS_REF_COUNTER, &traps_ref_counter, trap_code);
    BCMDNX_IF_ERR_EXIT(rv);
	*is_zero = (traps_ref_counter == 0);
exit:
    BCMDNX_FUNC_RETURN;
}
/* endpoint linked list*/
void 
_bcm_dpp_oam_endpoint_list_init(endpoint_list_t * endpoint_list) {
	endpoint_list->first_member = NULL;
	endpoint_list->size = 0;
}

int 
_bcm_dpp_oam_endpoint_list_member_add(int unit, endpoint_list_t * endpoint_list, uint32 endpoint_index) {
	endpoint_list_member_t * endpoint_p = NULL;

    BCMDNX_INIT_FUNC_DEFS;	
	BCMDNX_ALLOC(endpoint_p, sizeof(endpoint_list_member_t), "oam endpoint list member");
	endpoint_p->index = endpoint_index;
	endpoint_p->next = endpoint_list->first_member;
	endpoint_list->first_member = endpoint_p;
	endpoint_list->size += 1;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_oam_endpoint_list_empty(int unit, endpoint_list_t * endpoint_list) {
	return (endpoint_list->first_member==NULL);
}

int 
_bcm_dpp_oam_endpoint_list_member_find(int unit, endpoint_list_t * endpoint_list, uint32 index, uint8 delete_endpoint, uint8 *found) {
	endpoint_list_member_t * endpoint_p;
	endpoint_list_member_t * endpoint_prev_p;

    BCMDNX_INIT_FUNC_DEFS;
	endpoint_prev_p = endpoint_list->first_member;
	if (endpoint_prev_p == NULL) {
		*found = FALSE;
		BCMDNX_FUNC_RETURN;
	}
	if (endpoint_prev_p->index == index) {
		*found = TRUE;
		if (delete_endpoint) {
			endpoint_list->first_member = endpoint_prev_p->next;
			BCM_FREE(endpoint_prev_p);
            endpoint_list->size -= 1;
		}
		BCMDNX_FUNC_RETURN;
	}
	endpoint_p = endpoint_prev_p->next;
	*found = FALSE;
	while (endpoint_p != NULL) {
		if (endpoint_p->index == index) {
			*found=TRUE;
			break;
		}
		endpoint_prev_p = endpoint_p;
		endpoint_p = endpoint_p->next;
	}
	if (*found==TRUE) {
		if (delete_endpoint) {
			endpoint_prev_p->next = endpoint_p->next;
			BCM_FREE(endpoint_p);
			endpoint_list->size -= 1;
		}
	}

    BCMDNX_FUNC_RETURN;
}

/* Free all list members but not list pointer itself */
int 
_bcm_dpp_oam_endpoint_list_destroy(int unit, endpoint_list_t * endpoint_list) {
	endpoint_list_member_t * endpoint_p;
	endpoint_list_member_t * endpoint_temp_p;

    BCMDNX_INIT_FUNC_DEFS;
	endpoint_p = endpoint_list->first_member;
	while (endpoint_p != NULL) {
		endpoint_temp_p = endpoint_p;
		endpoint_p = endpoint_p->next;
		BCM_FREE(endpoint_temp_p);
	}
    endpoint_list->first_member = 0;
    endpoint_list->size = 0;

    BCMDNX_FUNC_RETURN;
}

/* 
 * MA name db
 */

#ifdef BCM_WARM_BOOT_SUPPORT
/* fill saved_data_ptr with the data from orig_data (orig_data may be a complex data structure) */
int _bcm_dpp_hash_tbl_save_oam_ma_name(void *orig_data, uint8 *saved_data_ptr) {
	bcm_oam_ma_name_t * ma_name_p;

	ma_name_p = (bcm_oam_ma_name_t*)orig_data;
	sal_memcpy(saved_data_ptr, ma_name_p, sizeof(bcm_oam_ma_name_t));
	return BCM_E_NONE;
}

/* return pointer to the data to be saved to the hashed table based of the data saved in wb*/
int _bcm_dpp_hash_tbl_load_oam_ma_name(uint8 *wb_data_ptr, void **data_ptr) {
	bcm_oam_ma_name_t * ma_name_p = NULL;

    ma_name_p = sal_alloc(sizeof(bcm_oam_ma_name_t), "oam_ma_name");
	sal_memcpy(ma_name_p, wb_data_ptr, sizeof(bcm_oam_ma_name_t));

	*data_ptr = ma_name_p;

	return BCM_E_NONE;
}
#endif /*BCM_WARM_BOOT_SUPPORT*/

void _bcm_dpp_sw_db_hash_oam_ma_name_destroy_cb(shr_htb_data_t data)
{
	bcm_oam_ma_name_t * ma_name_p;

	ma_name_p = (bcm_oam_ma_name_t*)data;
	sal_free(ma_name_p);
}

int 
_bcm_dpp_sw_db_hash_oam_ma_name_create(int unit)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = shr_htb_create(&_bcm_dpp_oam_ma_index_to_name_db[unit],
                   SOC_PPD_OAM_MAX_NUMBER_OF_MAS(unit),
                   sizeof(uint32),
                   "OAM ma name DB _bcm_dpp_sw_db_hash_oam_ma_name_create");
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_init(unit, SOC_DPP_WB_HASH_TBLS_OAM_MA_NAME,
                                                     SOC_DPP_WB_ENGINE_VAR_OAM_MA_NAME, 
							  _bcm_dpp_oam_ma_index_to_name_db[unit], 
							  sizeof(uint32), 
							  SOC_PPD_OAM_MAX_NUMBER_OF_MAS(unit), 
							  sizeof(bcm_oam_ma_name_t), 
							  _bcm_dpp_hash_tbl_save_oam_ma_name, 
							  _bcm_dpp_hash_tbl_load_oam_ma_name);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/

	shr_htb_cast_key_func_set(_bcm_dpp_oam_ma_index_to_name_db[unit], (shr_htb_cast_key_f)_bcm_dpp_htb_cast_key_f);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_oam_ma_name_destroy(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv = BCM_E_NONE;
#endif /*BCM_WARM_BOOT_SUPPORT*/
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(shr_htb_destroy(&_bcm_dpp_oam_ma_index_to_name_db[unit], _bcm_dpp_sw_db_hash_oam_ma_name_destroy_cb));
#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_destroy(unit, SOC_DPP_WB_HASH_TBLS_OAM_MA_NAME);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_ma_name_iterate(int unit, shr_htb_cb_t restore_cb)
{
    int rv;

    rv = shr_htb_iterate(unit, _bcm_dpp_oam_ma_index_to_name_db[unit], restore_cb);

    return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_ma_name_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = shr_htb_find(_bcm_dpp_oam_ma_index_to_name_db[unit], key, data, remove);

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_oam_ma_name_insert(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(shr_htb_insert(_bcm_dpp_oam_ma_index_to_name_db[unit], key, data));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * MEP to MA db
 */

#ifdef BCM_WARM_BOOT_SUPPORT
/* fill saved_data_ptr with the data from orig_data (orig_data may be a complex data structure) */
int _bcm_dpp_hash_tbl_save_oam_bfd_mep_info(void *orig_data, uint8 *saved_data_ptr) {
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_info_p;

	mep_info_p = (SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY*)orig_data;
	sal_memcpy(saved_data_ptr, mep_info_p, sizeof(SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY));
	return BCM_E_NONE;
}

/* return pointer to the data to be saved to the hashed table based of the data saved in wb*/
int _bcm_dpp_hash_tbl_load_oam_bfd_mep_info(uint8 *wb_data_ptr, void **data_ptr) {
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_info_p = NULL;

    mep_info_p = sal_alloc(sizeof(SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY), "oam_mep_info");
	sal_memcpy(mep_info_p, wb_data_ptr, sizeof(SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY));

	*data_ptr = mep_info_p;

	return BCM_E_NONE;
}
#endif /*BCM_WARM_BOOT_SUPPORT*/

void _bcm_dpp_sw_db_hash_oam_bfd_mep_info_destroy_cb(shr_htb_data_t data)
{
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_info_p;

	mep_info_p = (SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY*)data;
	sal_free(mep_info_p);
}

int 
_bcm_dpp_sw_db_hash_oam_bfd_mep_info_create(int unit)
{
    int rv;
    int oam_max_meps;

    BCMDNX_INIT_FUNC_DEFS;

/* Maximal number of MEPS supported by HW is 8K x 7 MD-levels. However, this consumes proportional
   memory and persistant storage (~ number of meps x 35B).
   This value may be overriden via soc property. (Table size must be power of 2)
   If up and down mep aded on same lif and level this should be increased by 2. */
    oam_max_meps = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_mep_count", 0);
    if (oam_max_meps < 1) {
        oam_max_meps = (SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit) * 8);
    }

    rv = shr_htb_create(&_bcm_dpp_oam_bfd_mep_info_db[unit],
                   oam_max_meps,
                   sizeof(uint32),
                   "OAM bfd info DB _bcm_dpp_sw_db_hash_oam_bfd_mep_info_create");
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_init(unit, SOC_DPP_WB_HASH_TBLS_OAM_BFD_MEP_INFO, 
                              SOC_DPP_WB_ENGINE_VAR_OAM_BFD_MEP_INFO, 
							  _bcm_dpp_oam_bfd_mep_info_db[unit], 
							  sizeof(uint32), 
							  oam_max_meps, 
							  sizeof(SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY), 
							  _bcm_dpp_hash_tbl_save_oam_bfd_mep_info, 
							  _bcm_dpp_hash_tbl_load_oam_bfd_mep_info);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/

	shr_htb_cast_key_func_set(_bcm_dpp_oam_bfd_mep_info_db[unit], (shr_htb_cast_key_f)_bcm_dpp_htb_cast_key_f);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_oam_bfd_mep_info_destroy(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv;
#endif /*BCM_WARM_BOOT_SUPPORT*/

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(shr_htb_destroy(&_bcm_dpp_oam_bfd_mep_info_db[unit], _bcm_dpp_sw_db_hash_oam_bfd_mep_info_destroy_cb));
#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_destroy(unit, SOC_DPP_WB_HASH_TBLS_OAM_BFD_MEP_INFO);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_info_iterate(int unit, shr_htb_cb_t restore_cb)
{
    int rv;

    rv = shr_htb_iterate(unit, _bcm_dpp_oam_bfd_mep_info_db[unit], restore_cb);

    return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_info_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = shr_htb_find(_bcm_dpp_oam_bfd_mep_info_db[unit], key, data, remove);

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_oam_bfd_mep_info_insert(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(shr_htb_insert(_bcm_dpp_oam_bfd_mep_info_db[unit], key, data));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * MA to MEP db
 */

#ifdef BCM_WARM_BOOT_SUPPORT
/* fill saved_data_ptr with the data from orig_data (orig_data may be a complex data structure) */
int _bcm_dpp_hash_tbl_save_endpoint_linked_list(void *orig_data, uint8 *saved_data_ptr) {
	endpoint_list_t * mep_list_p = NULL;
	endpoint_list_member_t * mep_iter_p = NULL;
	uint32 last_index = (uint32)(-1); /*last index indication*/

	mep_list_p = (endpoint_list_t*)orig_data;
	mep_iter_p = mep_list_p->first_member;
	while (mep_iter_p!=NULL) {
		sal_memcpy(saved_data_ptr, &mep_iter_p->index, sizeof(uint32)); /* only rellevant data is the indices */
		saved_data_ptr += sizeof(uint32);

		mep_iter_p = mep_iter_p->next;
	}

	sal_memcpy(saved_data_ptr, &last_index, sizeof(uint32));
	return BCM_E_NONE;
}

/* return pointer to the data to be saved to the hashed table based of the data saved in wb*/
int _bcm_dpp_hash_tbl_load_endpoint_linked_list(uint8 *wb_data_ptr, void **data_ptr) {
	endpoint_list_t * mep_list_p = NULL;
	endpoint_list_member_t * mep_iter_p = NULL;
	endpoint_list_member_t * mep_iter_prev_p = NULL;
	uint32 last_index;

    mep_list_p = sal_alloc(sizeof(endpoint_list_t), "oam_rmep_list");
	mep_list_p->size = 0;
	sal_memcpy(&last_index, wb_data_ptr, sizeof(uint32));
	wb_data_ptr += sizeof(uint32);

	if (last_index == (uint32)(-1)) {
		mep_list_p->first_member = NULL;
	}
	else {
		/* First member */
		mep_iter_p = sal_alloc(sizeof(endpoint_list_member_t), "oam_rmep_list_member");
		mep_iter_p->index = last_index;
		mep_iter_prev_p = mep_iter_p;
		mep_list_p->first_member = mep_iter_p;
		mep_list_p->size += 1;

		sal_memcpy(&last_index, wb_data_ptr, sizeof(uint32));
		wb_data_ptr += sizeof(uint32);

		/* Other members */
		while (last_index != (uint32)(-1)) {
			mep_iter_p = sal_alloc(sizeof(endpoint_list_member_t), "oam_rmep_list_member");
			mep_iter_prev_p->next = mep_iter_p;
			mep_iter_p->index = last_index;
			mep_iter_prev_p = mep_iter_p;
			mep_list_p->size += 1;

			sal_memcpy(&last_index, wb_data_ptr, sizeof(uint32));
			wb_data_ptr += sizeof(uint32);
		}
		mep_iter_p->next = NULL;
	}

	*data_ptr = mep_list_p;

	return BCM_E_NONE;
}
#endif /*BCM_WARM_BOOT_SUPPORT*/

void _bcm_dpp_sw_db_hash_oam_endpoint_linked_list_destroy_cb(shr_htb_data_t data)
{
	endpoint_list_t * mep_list_p = NULL;
	endpoint_list_member_t * mep_iter_p = NULL;
	endpoint_list_member_t * mep_iter_next_p = NULL;

	mep_list_p = (endpoint_list_t*)data;
	mep_iter_p = mep_list_p->first_member;
	sal_free(mep_list_p);
	while (mep_iter_p!=NULL) {
		mep_iter_next_p = mep_iter_p->next;
		sal_free(mep_iter_p);
		mep_iter_p = mep_iter_next_p;
	}
}

int 
_bcm_dpp_sw_db_hash_oam_ma_to_mep_create(int unit)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = shr_htb_create(&_bcm_dpp_oam_ma_to_mep_db[unit],
                   SOC_PPD_OAM_MAX_NUMBER_OF_MAS(unit),
                   sizeof(uint32),
                   "OAM ma to mep DB _bcm_dpp_sw_db_hash_oam_ma_to_mep_create");
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_init(unit, SOC_DPP_WB_HASH_TBLS_OAM_MA_TO_MEP,
                                                     SOC_DPP_WB_ENGINE_VAR_OAM_MA_TO_MEP,
							  _bcm_dpp_oam_ma_to_mep_db[unit], 
							  sizeof(uint32), 
							  SOC_PPD_OAM_MAX_NUMBER_OF_MAS(unit), 
							  sizeof(endpoint_list_t) + (sizeof(endpoint_list_member_t)*_BCM_OAM_MEP_PER_MA_MAX_NUM), 
							  _bcm_dpp_hash_tbl_save_endpoint_linked_list, 
							  _bcm_dpp_hash_tbl_load_endpoint_linked_list);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/

	shr_htb_cast_key_func_set(_bcm_dpp_oam_ma_to_mep_db[unit], (shr_htb_cast_key_f)_bcm_dpp_htb_cast_key_f);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_oam_ma_to_mep_destroy(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv;
#endif /*BCM_WARM_BOOT_SUPPORT*/

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(shr_htb_destroy(&_bcm_dpp_oam_ma_to_mep_db[unit], _bcm_dpp_sw_db_hash_oam_endpoint_linked_list_destroy_cb));
#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_destroy(unit, SOC_DPP_WB_HASH_TBLS_OAM_MA_TO_MEP);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_ma_to_mep_iterate(int unit, shr_htb_cb_t restore_cb)
{
    int rv;

    rv = shr_htb_iterate(unit, _bcm_dpp_oam_ma_to_mep_db[unit], restore_cb);

    return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_ma_to_mep_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = shr_htb_find(_bcm_dpp_oam_ma_to_mep_db[unit], key, data, remove);

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_oam_ma_to_mep_insert(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(shr_htb_insert(_bcm_dpp_oam_ma_to_mep_db[unit], key, data));

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * MEP to RMEP db
 */

int 
_bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_create(int unit)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = shr_htb_create(&_bcm_dpp_oam_bfd_mep_to_rmep_db[unit],
                   SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit),
                   sizeof(uint32),
                   "OAM ma to mep DB _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_create");
    BCMDNX_IF_ERR_EXIT(rv);
#ifdef _BCM_OAM_ADD_OAM_BFD_MEP_TO_RMEP_HASH_TO_WB /* No warmboot for this db*/
#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_init(unit, SOC_DPP_WB_HASH_TBLS_OAM_BFD_MEP_TO_RMEP, 
							  _bcm_dpp_oam_bfd_mep_to_rmep_db[unit], 
							  sizeof(uint32), 
							  SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), 
							  sizeof(endpoint_list_t) + (sizeof(endpoint_list_member_t)*SOC_PPD_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit)), 
							  _bcm_dpp_hash_tbl_save_endpoint_linked_list, 
							  _bcm_dpp_hash_tbl_load_endpoint_linked_list);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/
#endif
	shr_htb_cast_key_func_set(_bcm_dpp_oam_bfd_mep_to_rmep_db[unit], (shr_htb_cast_key_f)_bcm_dpp_htb_cast_key_f);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_destroy(int unit)
{
#ifdef _BCM_OAM_ADD_OAM_BFD_MEP_TO_RMEP_HASH_TO_WB /* No warmboot for this db*/
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv;
#endif /*BCM_WARM_BOOT_SUPPORT*/
#endif

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(shr_htb_destroy(&_bcm_dpp_oam_bfd_mep_to_rmep_db[unit], _bcm_dpp_sw_db_hash_oam_endpoint_linked_list_destroy_cb));
#ifdef _BCM_OAM_ADD_OAM_BFD_MEP_TO_RMEP_HASH_TO_WB /* No warmboot for this db*/
#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_destroy(unit, SOC_DPP_WB_HASH_TBLS_OAM_BFD_MEP_TO_RMEP);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/
#endif

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_iterate(int unit, shr_htb_cb_t restore_cb)
{
    int rv;

    rv = shr_htb_iterate(unit, _bcm_dpp_oam_bfd_mep_to_rmep_db[unit], restore_cb);

    return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = shr_htb_find(_bcm_dpp_oam_bfd_mep_to_rmep_db[unit], key, data, remove);

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_insert(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(shr_htb_insert(_bcm_dpp_oam_bfd_mep_to_rmep_db[unit], key, data));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * RMEP info db
 */
#ifdef BCM_WARM_BOOT_SUPPORT
/* fill saved_data_ptr with the data from orig_data (orig_data may be a complex data structure) */
int _bcm_dpp_hash_tbl_save_oam_bfd_rmep_info(void *orig_data, uint8 *saved_data_ptr) {
	SOC_PPD_OAM_RMEP_INFO_DATA * rmep_info_p;

	rmep_info_p = (SOC_PPD_OAM_RMEP_INFO_DATA*)orig_data;
	sal_memcpy(saved_data_ptr, rmep_info_p, sizeof(SOC_PPD_OAM_RMEP_INFO_DATA));
	return BCM_E_NONE;
}

/* return pointer to the data to be saved to the hashed table based of the data saved in wb*/
int _bcm_dpp_hash_tbl_load_oam_bfd_rmep_info(uint8 *wb_data_ptr, void **data_ptr) {
	SOC_PPD_OAM_RMEP_INFO_DATA * rmep_info_p = NULL;

    rmep_info_p = sal_alloc(sizeof(SOC_PPD_OAM_RMEP_INFO_DATA), "oam_rmep_info");
	sal_memcpy(rmep_info_p, wb_data_ptr, sizeof(SOC_PPD_OAM_RMEP_INFO_DATA));

	*data_ptr = rmep_info_p;

	return BCM_E_NONE;
}
#endif /*BCM_WARM_BOOT_SUPPORT*/

void _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_destroy_cb(shr_htb_data_t data)
{
	SOC_PPD_OAM_RMEP_INFO_DATA * rmep_info_p;

	rmep_info_p = (SOC_PPD_OAM_RMEP_INFO_DATA*)data;
	sal_free(rmep_info_p);
}


int 
_bcm_dpp_sw_db_hash_oam_bfd_rmep_info_create(int unit)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = shr_htb_create(&_bcm_dpp_oam_rmep_info_db[unit],
                   SOC_PPD_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit),
                   sizeof(uint32),
                   "OAM ma to mep DB _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_create");
    BCMDNX_IF_ERR_EXIT(rv);
#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_init(unit, SOC_DPP_WB_HASH_TBLS_OAM_BFD_RMEP_INFO,
                                                     SOC_DPP_WB_ENGINE_VAR_OAM_BFD_RMEP_INFO,
							  _bcm_dpp_oam_rmep_info_db[unit], 
							  sizeof(uint32), 
							  SOC_PPD_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit), 
							  sizeof(SOC_PPD_OAM_RMEP_INFO_DATA), 
							  _bcm_dpp_hash_tbl_save_oam_bfd_rmep_info, 
							  _bcm_dpp_hash_tbl_load_oam_bfd_rmep_info);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/
	shr_htb_cast_key_func_set(_bcm_dpp_oam_rmep_info_db[unit], (shr_htb_cast_key_f)_bcm_dpp_htb_cast_key_f);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_oam_bfd_rmep_info_destroy(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv;
#endif /*BCM_WARM_BOOT_SUPPORT*/

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(shr_htb_destroy(&_bcm_dpp_oam_rmep_info_db[unit], _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_destroy_cb));
#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_destroy(unit, SOC_DPP_WB_HASH_TBLS_OAM_BFD_RMEP_INFO);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_rmep_info_iterate(int unit, shr_htb_cb_t restore_cb)
{
    int rv;

    rv = shr_htb_iterate(unit, _bcm_dpp_oam_rmep_info_db[unit], restore_cb);

    return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_bfd_rmep_info_find(int unit, shr_htb_key_t key, shr_htb_data_t *data, int remove)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = shr_htb_find(_bcm_dpp_oam_rmep_info_db[unit], key, data, remove);

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_sw_db_hash_oam_bfd_rmep_info_insert(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(shr_htb_insert(_bcm_dpp_oam_rmep_info_db[unit], key, data));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
* AIS DB functions.
*/
int 
_bcm_dpp_sw_db_hash_oam_mep_to_ais_id_create(int unit)
{
    int rv;
	BCMDNX_INIT_FUNC_DEFS;
	rv = shr_htb_create(&_bcm_dpp_oam_mep_to_ais_id[unit],
				   SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)/2,
				   sizeof(uint32),
				   "OAM MEP ID to AIS ID");
	BCMDNX_IF_ERR_EXIT(rv);
#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_init(unit, SOC_DPP_WB_HASH_TBLS_OAM_MEP_TO_AIS_ID,
                                                     SOC_DPP_WB_ENGINE_VAR_OAM_MEP_TO_AIS_ID,
							  _bcm_dpp_oam_mep_to_ais_id[unit], 
							  sizeof(uint32), 
							  SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)/2, 
							   sizeof(endpoint_list_t) + (sizeof(endpoint_list_member_t)*1), /* At most 7  (currently 1) AIS entries per endpoint, one per each level, excluding level 0.*/
							  _bcm_dpp_hash_tbl_save_endpoint_linked_list, 
							  _bcm_dpp_hash_tbl_load_endpoint_linked_list);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/

	shr_htb_cast_key_func_set(_bcm_dpp_oam_mep_to_ais_id[unit], (shr_htb_cast_key_f)_bcm_dpp_htb_cast_key_f);

exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Associate an AIS id with an endpoint.
 * 
 * @author sinai (14/07/2014)
 * 
 * @param unit 
 * @param endpoint_id 
 * @param ais_id 
 * 
 * @return int 
 */
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_insert(int unit, uint32 endpoint_id, int ais_id)
{
    int rv;
	endpoint_list_t * ais_list_p = NULL;
	uint8 found;
	BCMDNX_INIT_FUNC_DEFS;

  
	rv = shr_htb_find(_bcm_dpp_oam_mep_to_ais_id[unit],&endpoint_id, (shr_htb_data_t)&ais_list_p, 1 /*  remove*/  );
    if (rv == BCM_E_NOT_FOUND) {
		BCMDNX_ALLOC(ais_list_p, sizeof(endpoint_list_t), "oam endpoint list");
		if (ais_list_p == NULL) {        
			BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Error: Failed to allocate memory\n")));
		}
		_bcm_dpp_oam_endpoint_list_init(ais_list_p);
	}
	else {
		BCMDNX_IF_ERR_EXIT(rv);
        if (ais_list_p->size >7) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Maximum number of AIS frames per endpoint exceeded.\n")));
        }
    }

	rv = _bcm_dpp_oam_endpoint_list_member_find(unit, ais_list_p, ais_id, 0, &found);
	BCMDNX_IF_ERR_EXIT(rv); 

	if (!found) { /*if rmep index does not exist - add it */
		rv = _bcm_dpp_oam_endpoint_list_member_add(unit, ais_list_p, ais_id);
		BCMDNX_IF_ERR_EXIT(rv); 
	}

	rv = shr_htb_insert(_bcm_dpp_oam_mep_to_ais_id[unit],&endpoint_id, (shr_htb_data_t) ais_list_p);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Return an array of available AIS IDs
 * 
 * @author sinai (14/07/2014)
 * 
 * @param unit 
 * @param endpoint_id 
 * @param ais_id - num_found entries will be filled.
 * @param num_found - Number of AIS elements used by the 
 *  				endpoint
 * 
 * @return int 
 */
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(int unit, uint32 endpoint_id, int * ais_id, uint8 * num_found)
{
    int rv;
	endpoint_list_t * ais_list_p = NULL;
	endpoint_list_member_t * endpoint_p;
    BCMDNX_INIT_FUNC_DEFS;


	rv = shr_htb_find(_bcm_dpp_oam_mep_to_ais_id[unit],&endpoint_id, (shr_htb_data_t)&ais_list_p, 0 /*  remove*/  );    
	if (rv == BCM_E_NOT_FOUND) {
		*num_found = 0;
	}
	else {
		int i=0;
		BCMDNX_IF_ERR_EXIT(rv);
		*num_found = 0;
		endpoint_p = ais_list_p->first_member;
        while  (endpoint_p) {
			*(ais_id +i) = endpoint_p->index;
			endpoint_p = endpoint_p->next;
			*num_found +=1;
			++i;
            if (*num_found > 1) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Multiple AIS entries per MEP disallowed.")));
            }
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Remove an entry from the hash table.
 * 
 * @author sinai (14/07/2014)
 * 
 * @param uint 
 * @param mep_id 
 * 
 * @return int 
 */
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_delete_by_id(int unit, uint32 endpoint_id )
{
    int rv;
	endpoint_list_t * ais_list_p = NULL;
    BCMDNX_INIT_FUNC_DEFS;

    rv = shr_htb_find(_bcm_dpp_oam_mep_to_ais_id[unit], &endpoint_id, (shr_htb_data_t)&ais_list_p, 1 /*  remove*/);
	BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Destroy the hash table
 * 
 * @author sinai (14/07/2014)
 * 
 * @param uint 
 * 
 * @return int 
 */
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_destroy(int unit)
{
	int rv;
	BCMDNX_INIT_FUNC_DEFS;

	rv = shr_htb_destroy(&_bcm_dpp_oam_mep_to_ais_id[unit], _bcm_dpp_sw_db_hash_oam_endpoint_linked_list_destroy_cb);
	BCMDNX_IF_ERR_EXIT(rv);
#ifdef BCM_WARM_BOOT_SUPPORT
	rv = dpp_hash_tbl_wb_destroy(unit, SOC_DPP_WB_HASH_TBLS_OAM_MEP_TO_AIS_ID);
	BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
	BCMDNX_FUNC_RETURN;
}



/* 
 * End of DB functions
 */
  
#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_dpp_oam_bfd_mep_info_construct_mep_to_rmep_db_cb(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    int rv = BCM_E_NONE;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_entry_p;
	uint32 rmep_index, mep_index, rmep_id_internal;
	uint8 is_found;
	uint16 rmep_id;

	BCMDNX_INIT_FUNC_DEFS;

	/*get the endpoint_info*/
	mep_entry_p = (SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY*) data;
	mep_index = *(uint32*)key;

	if (((mep_entry_p->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) && (mep_entry_p->remote_gport == BCM_GPORT_INVALID)) {
        if (mep_entry_p->mep_type==SOC_PPC_OAM_MEP_TYPE_ETH_OAM || mep_entry_p->mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP ||
            mep_entry_p->mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE ) {
            for (rmep_id = 0; rmep_id < _BCM_OAM_RMEP_ID_MAX; rmep_id++) {
                SOC_DPP_ALLOW_WARMBOOT_WRITE(handle_sand_result(soc_ppd_oam_oamp_rmep_index_get(unit, rmep_id, mep_index, mep_entry_p->mep_type, &rmep_id_internal, &is_found)), rv);
                BCMDNX_IF_ERR_EXIT(rv);
                if (is_found) {
                    _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(rmep_id_internal, rmep_index);
                    rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(unit, mep_index, rmep_index);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        } else {
            /* BFD - no need to iterate over all possible names ("rmep_id"). This is ignored anyways*/
                SOC_DPP_ALLOW_WARMBOOT_WRITE(handle_sand_result(soc_ppd_oam_oamp_rmep_index_get(unit, 0, mep_index, mep_entry_p->mep_type, &rmep_id_internal, &is_found)), rv);
			BCMDNX_IF_ERR_EXIT(rv);
                if (is_found) {
                    _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(rmep_id_internal, rmep_index);
                    rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(unit, mep_index, rmep_index);
				BCMDNX_IF_ERR_EXIT(rv);
                }
        }
	}
    else if ((mep_entry_p->mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) || 
             (mep_entry_p->mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) || 
             (mep_entry_p->mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) || 
             (mep_entry_p->mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE) || 
             (mep_entry_p->mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE_GAL) || 
             (mep_entry_p->mep_type==SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP) || 
             (mep_entry_p->mep_type==SOC_PPC_OAM_MEP_TYPE_BFDCV_O_MPLSTP)) {

                rmep_index = (uint32)(-1);
				rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(unit, mep_index, rmep_index);
				BCMDNX_IF_ERR_EXIT(rv);
    }
	
	BCM_EXIT;
exit:
	BCMDNX_FUNC_RETURN;

}

int _bcm_dpp_oam_get_hash_table_reconstruct_oam_bfd_mep_to_rmep(int unit) {
	int rv;
    BCMDNX_INIT_FUNC_DEFS;

	/* Iterate on all the local meps */
	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_iterate(unit, _bcm_dpp_oam_bfd_mep_info_construct_mep_to_rmep_db_cb);
	BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_bfd_endpoint_list_reconstruct_oam_bfd_mep_info(int unit) {
	int rv;
    BCMDNX_INIT_FUNC_DEFS;

	/* Iterate on all the local meps */
	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_iterate(unit, _bcm_dpp_oam_bfd_mep_info_construct_bfd_mep_list_cb);
	BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_bfd_get_hash_tables_from_wb_arrays(int unit) {
	int rv;
	uint8 oam_is_init;
    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		rv = dpp_extract_hash_tbl_from_wb_arrays(unit, SOC_DPP_WB_HASH_TBLS_OAM_BFD_RMEP_INFO);
		BCMDNX_IF_ERR_EXIT(rv);

		rv = dpp_extract_hash_tbl_from_wb_arrays(unit, SOC_DPP_WB_HASH_TBLS_OAM_BFD_MEP_INFO);
		BCMDNX_IF_ERR_EXIT(rv);

		rv = _bcm_dpp_oam_get_hash_table_reconstruct_oam_bfd_mep_to_rmep(unit);
		BCMDNX_IF_ERR_EXIT(rv);
	}

	rv = _bcm_dpp_bfd_endpoint_list_reconstruct_oam_bfd_mep_info(unit);
	BCMDNX_IF_ERR_EXIT(rv);

	


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_get_hash_tables_from_wb_arrays(int unit) {
	int rv;
    BCMDNX_INIT_FUNC_DEFS;

	rv = dpp_extract_hash_tbl_from_wb_arrays(unit, SOC_DPP_WB_HASH_TBLS_OAM_BFD_RMEP_INFO);
	BCMDNX_IF_ERR_EXIT(rv);

	rv = dpp_extract_hash_tbl_from_wb_arrays(unit, SOC_DPP_WB_HASH_TBLS_OAM_BFD_MEP_INFO);
	BCMDNX_IF_ERR_EXIT(rv);

	rv = _bcm_dpp_oam_get_hash_table_reconstruct_oam_bfd_mep_to_rmep(unit);
	BCMDNX_IF_ERR_EXIT(rv);

	rv = dpp_extract_hash_tbl_from_wb_arrays(unit, SOC_DPP_WB_HASH_TBLS_OAM_MA_NAME);
	BCMDNX_IF_ERR_EXIT(rv);

	rv = dpp_extract_hash_tbl_from_wb_arrays(unit, SOC_DPP_WB_HASH_TBLS_OAM_MA_TO_MEP);
	BCMDNX_IF_ERR_EXIT(rv);

	


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /*BCM_WARM_BOOT_SUPPORT*/

int _bcm_dpp_oam_dbs_init(int unit, uint8 is_oam) {
	int rv;

    BCMDNX_INIT_FUNC_DEFS;

	rv = _bcm_dpp_sw_db_hash_oam_ma_name_create(unit);
	BCMDNX_IF_ERR_EXIT(rv);
	rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_create(unit);
	BCMDNX_IF_ERR_EXIT(rv);
	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_create(unit);
	BCMDNX_IF_ERR_EXIT(rv);
	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_create(unit);
	BCMDNX_IF_ERR_EXIT(rv);
	rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_create(unit);
	BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_JERICHO_A0(unit) && is_oam) {
		rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_create(unit);
		BCMDNX_IF_ERR_EXIT(rv);
    }


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_dbs_destroy(int unit, uint8 is_oam) {
	int rv;

    BCMDNX_INIT_FUNC_DEFS;

   	rv = _bcm_dpp_sw_db_hash_oam_ma_name_destroy(unit);
   	BCMDNX_IF_ERR_EXIT(rv);
   	rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_destroy(unit);
   	BCMDNX_IF_ERR_EXIT(rv);
   	rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_destroy(unit);
   	BCMDNX_IF_ERR_EXIT(rv);
   	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_destroy(unit);
   	BCMDNX_IF_ERR_EXIT(rv);
   	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_destroy(unit);
   	BCMDNX_IF_ERR_EXIT(rv);


    if (SOC_IS_JERICHO_A0(unit) && is_oam) {
		rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_destroy(unit);
		BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * MA name db
 */

int
_bcm_dpp_oam_ma_db_delete_ma_cb(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    int rv = BCM_E_NONE;
	uint32 * ma_index_p;

    BCMDNX_INIT_FUNC_DEFS;

	ma_index_p = (uint32 *)key;
	rv = bcm_petra_oam_endpoint_destroy_all(unit, *ma_index_p);
    BCMDNX_IF_ERR_EXIT(rv);

	rv = bcm_dpp_am_oam_ma_id_dealloc(unit, *ma_index_p);
	BCMDNX_IF_ERR_EXIT(rv);

	BCM_EXIT;
exit:
	BCMDNX_FUNC_RETURN;

}

int _bcm_dpp_oam_ma_db_insert(int unit, uint32 ma_index, bcm_oam_ma_name_t * ma_name) {
	int rv;
	bcm_oam_ma_name_t * ma_name_temp_p = NULL;

    BCMDNX_INIT_FUNC_DEFS;

	rv = _bcm_dpp_sw_db_hash_oam_ma_name_find(unit,
										&(ma_name),
										(shr_htb_data_t*)(void *)(&ma_name_temp_p),
										TRUE);        

    if (rv == BCM_E_NOT_FOUND) {
		BCMDNX_ALLOC(ma_name_temp_p, sizeof(bcm_oam_ma_name_t), "oam_ma_name");
		if (ma_name_temp_p == NULL) {        
			BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Error: Failed to allocate memory\n")));
		}

		*ma_name_temp_p = *ma_name;
		rv = _bcm_dpp_sw_db_hash_oam_ma_name_insert(unit, 
													(shr_htb_key_t)&(ma_index), 
													(shr_htb_data_t)(ma_name_temp_p)); 
		BCMDNX_IF_ERR_EXIT(rv);
	}
	else {
		if (rv == BCM_E_NONE) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,(_BSL_BCM_MSG("Error: Group %d exists\n"), ma_index));
		}
		else {
			BCMDNX_IF_ERR_EXIT(rv);
		}
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_ma_db_get(int unit, uint32 ma_index, bcm_oam_ma_name_t * ma_name, uint8 *found) {
	int rv;

	bcm_oam_ma_name_t * ma_name_temp = NULL;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_ma_name_find(unit,
										&(ma_index),
										(shr_htb_data_t*)(void *)&ma_name_temp,
    									FALSE);
	if (rv == BCM_E_NONE) {
		*found = TRUE;
	 	*ma_name = *ma_name_temp;
    }
    else if (rv == BCM_E_NOT_FOUND){
		*found = FALSE;
    }
	else {
		BCMDNX_IF_ERR_EXIT(rv);
	}


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_ma_db_delete(int unit, uint32 ma_index) {
	int rv;
	bcm_oam_ma_name_t * name_dummy = NULL;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_ma_name_find(unit,
										&(ma_index),
										(shr_htb_data_t*)(void *)&name_dummy,
										TRUE);        
	BCMDNX_IF_ERR_EXIT(rv);
	
	BCM_FREE(name_dummy); 

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * MA to MEP db
 */

int _bcm_dpp_oam_ma_to_mep_db_insert(int unit, uint32 ma_index, uint32 mep_index) {
	int rv;
	endpoint_list_t * mep_list_p = NULL;
	uint8 found;

    BCMDNX_INIT_FUNC_DEFS;

	BCMDNX_ALLOC(mep_list_p, sizeof(endpoint_list_t), "oam endpoint list");
	rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_find(unit,
										&(ma_index),
										(shr_htb_data_t*)(void *)(&mep_list_p),
										TRUE);        
    if (rv == BCM_E_NOT_FOUND) {
		_bcm_dpp_oam_endpoint_list_init(mep_list_p);
	}
	else {
		BCMDNX_IF_ERR_EXIT(rv);
	}

	rv = _bcm_dpp_oam_endpoint_list_member_find(unit, mep_list_p, mep_index, 0, &found);
	BCMDNX_IF_ERR_EXIT(rv);

	if (!found) { /* adding new mep_index if it does not exist already */
		rv = _bcm_dpp_oam_endpoint_list_member_add(unit, mep_list_p, mep_index);
		BCMDNX_IF_ERR_EXIT(rv);
	}

#ifdef BCM_WARM_BOOT_SUPPORT
	if (mep_list_p->size > _BCM_OAM_MEP_PER_MA_MAX_NUM) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Error: Endpoint per group limit reached. Warmboot supports up to %d endpoints per group.\n"), _BCM_OAM_MEP_PER_MA_MAX_NUM));
	}
#endif /*BCM_WARM_BOOT_SUPPORT*/

	rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_insert(unit, 
												(shr_htb_key_t)&(ma_index), 
												(shr_htb_data_t)(mep_list_p));
	BCMDNX_IF_ERR_EXIT(rv);

    goto exit_without_free;

exit:
    BCM_FREE(mep_list_p);
exit_without_free:
    BCMDNX_FUNC_RETURN;
}

/*The result of this get function is the member itself and not a copy*/
int _bcm_dpp_oam_ma_to_mep_db_get(int unit, uint32 ma_index, endpoint_list_t **mep_list, uint8* found) {
	int rv;

    BCMDNX_INIT_FUNC_DEFS; 
	rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_find(unit,
										&(ma_index),
										(shr_htb_data_t*)(void *)mep_list,
										FALSE);        
	if (rv == BCM_E_NONE) {
		*found = TRUE;
    }
    else if (rv == BCM_E_NOT_FOUND){
		*found = FALSE;
    }
	else {
		BCMDNX_IF_ERR_EXIT(rv);
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_ma_to_mep_db_ma_delete(int unit, uint32 ma_index) {
	int rv;
	endpoint_list_t * mep_list_p = NULL;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_find(unit,
										&(ma_index),
										(shr_htb_data_t*)(void *)&mep_list_p,
										TRUE);        
	BCMDNX_IF_ERR_EXIT(rv); 
	
	rv = _bcm_dpp_oam_endpoint_list_destroy(unit, mep_list_p);
	BCMDNX_IF_ERR_EXIT(rv);
	
	BCM_FREE(mep_list_p);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_ma_to_mep_db_mep_delete(int unit, uint32 ma_index, uint32 mep_index) {
	int rv;
	endpoint_list_t * mep_list_p = NULL;
	uint8 found;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_find(unit,
										&(ma_index),
										(shr_htb_data_t*)(void *)(&mep_list_p),
										TRUE);        
	BCMDNX_IF_ERR_EXIT(rv);
	rv = _bcm_dpp_oam_endpoint_list_member_find(unit, mep_list_p, mep_index, 1, &found);
	BCMDNX_IF_ERR_EXIT(rv);

	if (_bcm_dpp_oam_endpoint_list_empty(unit, mep_list_p)) {
		rv = _bcm_dpp_oam_endpoint_list_destroy(unit, mep_list_p);
		BCMDNX_IF_ERR_EXIT(rv);
		BCM_FREE(mep_list_p);
	}
	else {
		rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_insert(unit, &(ma_index), mep_list_p); 
		BCMDNX_IF_ERR_EXIT(rv);
	}
	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found\n"), mep_index));
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * MEP info db
 */

int _bcm_dpp_oam_bfd_mep_info_db_insert(int unit, uint32 mep_index, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_info) {
	int rv;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_info_temp_p = NULL;

    BCMDNX_INIT_FUNC_DEFS;

	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_find(unit,
										&(mep_index),
										(shr_htb_data_t*)(void *)(&mep_info_temp_p),
										FALSE);        

    if (rv == BCM_E_NOT_FOUND) {
		BCMDNX_ALLOC(mep_info_temp_p, sizeof(SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY), "oam mep info");
		if (mep_info_temp_p == NULL) {        
			BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Error: Failed to allocate memory\n")));
		}
		SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(mep_info_temp_p);
		*mep_info_temp_p = *mep_info;
		rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_insert(unit, 
												(shr_htb_key_t)&(mep_index), 
												(shr_htb_data_t)(mep_info_temp_p));
		BCMDNX_IF_ERR_EXIT(rv);
	}
	else {
		if (rv == BCM_E_NONE) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,(_BSL_BCM_MSG("Error: Endpoint %d exists\n"), mep_index));
		}
		else {
			BCMDNX_IF_ERR_EXIT(rv);
		}
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_bfd_mep_info_db_get(int unit, uint32 mep_index, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_info, uint8 *found) {
	int rv;

	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_info_temp_p;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_find(unit,
										&(mep_index),
										(shr_htb_data_t*)(void *)(&mep_info_temp_p),
										FALSE);        
	if (rv == BCM_E_NONE) {
		*found = TRUE;
	 	*mep_info = *mep_info_temp_p;
    }
    else if (rv == BCM_E_NOT_FOUND){
		*found = FALSE;
    }
	else {
		BCMDNX_IF_ERR_EXIT(rv);
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_bfd_mep_info_db_delete(int unit, uint32 mep_index) {
	int rv;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_info_dummy_p;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_find(unit,
										&(mep_index),
										(shr_htb_data_t*)(void *)(&mep_info_dummy_p),
										TRUE);        
	BCMDNX_IF_ERR_EXIT(rv); 
	
	BCM_FREE(mep_info_dummy_p); 	 

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 * MEP to RMEP db
 */

int _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(int unit, uint32 mep_index, uint32 rmep_index) {
	int rv;
	endpoint_list_t * rmep_list_p = NULL;
	uint8 found;

    BCMDNX_INIT_FUNC_DEFS;

	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(unit,
										&(mep_index),
										(shr_htb_data_t*)(void *)(&rmep_list_p),
										TRUE);        

    if (rv == BCM_E_NOT_FOUND) {
		BCMDNX_ALLOC(rmep_list_p, sizeof(endpoint_list_t), "oam endpoint list");
		if (rmep_list_p == NULL) {        
			BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Error: Failed to allocate memory\n")));
		}
		_bcm_dpp_oam_endpoint_list_init(rmep_list_p);
	}
	else {
		BCMDNX_IF_ERR_EXIT(rv);
	}
	rv = _bcm_dpp_oam_endpoint_list_member_find(unit, rmep_list_p, rmep_index, 0, &found);
	BCMDNX_IF_ERR_EXIT(rv); 

	if (!found) { /*if rmep index does not exist - add it */
		rv = _bcm_dpp_oam_endpoint_list_member_add(unit, rmep_list_p, rmep_index);
		BCMDNX_IF_ERR_EXIT(rv); 
	}
	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_insert(unit, 
											    (shr_htb_key_t)&(mep_index), 
												(shr_htb_data_t)(rmep_list_p));
	BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*The result of this get function is the member itself and not a copy*/
int _bcm_dpp_oam_bfd_mep_to_rmep_db_get(int unit, uint32 mep_index, endpoint_list_t **rmep_list, uint8* found) {
	int rv;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(unit,
										&(mep_index),
										(shr_htb_data_t*)(void *)rmep_list,
										FALSE);        
    if (rv == BCM_E_NOT_FOUND) {
		*found = FALSE;
	}
	else {
		BCMDNX_IF_ERR_EXIT(rv);
		*found = TRUE;
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_bfd_mep_to_rmep_db_mep_delete(int unit, uint32 mep_index) {
	int rv;
	endpoint_list_t * rmep_list_p = NULL;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(unit,
										&(mep_index),
										(shr_htb_data_t*)(void *)&rmep_list_p,
										TRUE);        
	BCMDNX_IF_ERR_EXIT(rv); 
	
	rv = _bcm_dpp_oam_endpoint_list_destroy(unit, rmep_list_p);
	BCMDNX_IF_ERR_EXIT(rv);		 
	BCM_FREE(rmep_list_p);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_bfd_mep_to_rmep_db_rmep_delete(int unit, uint32 mep_index, uint32 rmep_index) {
	int rv;
	endpoint_list_t * rmep_list_p = NULL;
	uint8 found;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(unit,
										&(mep_index),
										(shr_htb_data_t*)(void *)&rmep_list_p,
										TRUE);        
	BCMDNX_IF_ERR_EXIT(rv);

	rv = _bcm_dpp_oam_endpoint_list_member_find(unit, rmep_list_p, rmep_index, 1, &found);
	BCMDNX_IF_ERR_EXIT(rv);

	if (_bcm_dpp_oam_endpoint_list_empty(unit, rmep_list_p)) {
		rv = _bcm_dpp_oam_endpoint_list_destroy(unit, rmep_list_p);
		BCMDNX_IF_ERR_EXIT(rv);
		BCM_FREE(rmep_list_p);
	}
	else {
		rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_insert(unit, 
											    (shr_htb_key_t)&(mep_index), 
												(shr_htb_data_t)(rmep_list_p));
		BCMDNX_IF_ERR_EXIT(rv);
	}

	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,(_BSL_BCM_MSG("Error: Remote endpoint %d not found\n"), rmep_index));
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * RMEP to MEP db
 */

int _bcm_dpp_oam_bfd_rmep_info_db_insert(int unit, uint32 rmep_index, SOC_PPD_OAM_RMEP_INFO_DATA * rmep_info) {
	int rv;
	SOC_PPD_OAM_RMEP_INFO_DATA * rmep_info_p = NULL;

    BCMDNX_INIT_FUNC_DEFS;

	rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_find(unit,
										&(rmep_index),
										(shr_htb_data_t*)(void *)(&rmep_info_p),
										FALSE);        

    if (rv == BCM_E_NOT_FOUND) {
		BCMDNX_ALLOC(rmep_info_p, sizeof(SOC_PPD_OAM_RMEP_INFO_DATA), "oam rmep info");
		if (rmep_info_p == NULL) {        
			BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Error: Failed to allocate memory\n")));
		}
		*rmep_info_p = *rmep_info;
		rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_insert(unit, 
												(shr_htb_key_t)&(rmep_index), 
												(shr_htb_data_t)(rmep_info_p));
		BCMDNX_IF_ERR_EXIT(rv);
	}
	else {
		BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error: Remote endpoint %d exists\n"), rmep_index));
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_bfd_rmep_info_db_get(int unit, uint32 rmep_index,  SOC_PPD_OAM_RMEP_INFO_DATA * rmep_info, uint8 *found) {
	int rv;

	SOC_PPD_OAM_RMEP_INFO_DATA * rmep_info_temp_p = NULL;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_find(unit,
										&(rmep_index),
										(shr_htb_data_t*)(void *)&rmep_info_temp_p,
										FALSE);        
       
	if (rv == BCM_E_NONE) {
		*found = TRUE;
	 	*rmep_info = *rmep_info_temp_p;
    }
    else if (rv == BCM_E_NOT_FOUND){
		*found = FALSE;
    }
	else {
		BCMDNX_IF_ERR_EXIT(rv);
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_oam_bfd_rmep_info_db_delete(int unit, uint32 rmep_index) {
	int rv;
	SOC_PPD_OAM_RMEP_INFO_DATA * rmep_info_dummy_p = NULL;

    BCMDNX_INIT_FUNC_DEFS;
	rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_find(unit,
										&(rmep_index),
										(shr_htb_data_t*)(void *)(&rmep_info_dummy_p),
										TRUE);        
	BCMDNX_IF_ERR_EXIT(rv); 	
	
	BCM_FREE(rmep_info_dummy_p);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * End of DB functions
 */

int _bcm_dpp_oam_group_name_to_ma_name_struct_convert(int unit, uint8 *group_name, bcm_oam_ma_name_t *ma_name_struct) {

	uint32 i;

    BCMDNX_INIT_FUNC_DEFS;

	if (group_name[0]!=1) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                 (_BSL_BCM_MSG("Error: Error in group name. Unsupported name: %d.\n"), group_name[0]));
	}

	if (group_name[1]==3/*BCM_OAM_MA_NAME_FORMAT_SHORT*/) {
		ma_name_struct->is_short = 1;
		if (group_name[2]!=2) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                 (_BSL_BCM_MSG("Error: Error in group name. Unsupported length: %d. Short name format supports only length 2.\n"), group_name[2]));
		}	
	}
	else if (group_name[1]==0x20/*BCM_OAM_MA_NAME_FORMAT_ICC_BASED*/){
		ma_name_struct->is_short = 0;
		if (group_name[2]!=13) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                 (_BSL_BCM_MSG("Error: Error in group name. Unsupported length: %d. Long name format supports only length 13.\n"), group_name[2]));
		}
	}
	else {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                 (_BSL_BCM_MSG("Error: Unsupported group name format: %d.\n"), group_name[0]));
	}

	for (i=0; i<group_name[2]; i++) {
		ma_name_struct->name[i] = group_name[i+3];
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_ma_name_struct_to_group_name_convert(int unit, bcm_oam_ma_name_t *ma_name_struct, uint8 *group_name) {

	uint32 i;

	BCMDNX_INIT_FUNC_DEFS;

	group_name[0]=1;
	if (ma_name_struct->is_short == 1) {
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

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_bfd_mep_id_alloc(int unit, uint32 flags, uint32 *mep_index) {
	int rv;

	if (flags == SHR_RES_ALLOC_WITH_ID) {
		if (*mep_index < SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)/2) {
			rv = bcm_dpp_am_oam_mep_id_long_alloc(unit, flags, mep_index);
		}
		else {
			rv = bcm_dpp_am_oam_mep_id_short_alloc(unit, flags, mep_index);
		}
	}
	else {
		rv = _bcm_dpp_oam_mep_id_alloc(unit, flags, 1, mep_index);
	}

	return rv;
}

/* When WITH_ID flag is set, is_short parameter is ignored*/
int _bcm_dpp_oam_mep_id_alloc(int unit, uint32 flags, uint8 is_short, uint32 *mep_index) {
	int rv;

    if (flags == SHR_RES_ALLOC_WITH_ID) {
        return (*mep_index < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit))?
            bcm_dpp_am_oam_mep_id_long_alloc(unit, flags, mep_index) :  bcm_dpp_am_oam_mep_id_short_alloc(unit, flags, mep_index);
    }

	if (is_short) {
		rv = bcm_dpp_am_oam_mep_id_short_alloc(unit, flags, mep_index);
		if (rv == BCM_E_NONE) {
			return rv;
		}
	}
	return  bcm_dpp_am_oam_mep_id_long_alloc(unit, flags, mep_index);
}

int _bcm_dpp_oam_bfd_mep_id_is_alloced(int unit, uint32 mep_id) {
	int rv;

	rv = bcm_dpp_am_oam_mep_id_short_is_alloced(unit, mep_id);

	if ((rv == BCM_E_EXISTS) || (rv == BCM_E_NOT_FOUND)) {
		return rv;
	}
	return bcm_dpp_am_oam_mep_id_long_is_alloced(unit, mep_id);
}

int _bcm_dpp_oam_bfd_mep_id_dealloc(int unit, uint32 mep_id) {
	int rv;

	rv = bcm_dpp_am_oam_mep_id_short_is_alloced(unit, mep_id);
	if (rv == BCM_E_EXISTS) {
		return bcm_dpp_am_oam_mep_id_short_dealloc(unit, mep_id);
	}
	else {
		rv = bcm_dpp_am_oam_mep_id_long_is_alloced(unit, mep_id);
		if (rv == BCM_E_EXISTS) {
			return bcm_dpp_am_oam_mep_id_long_dealloc(unit, mep_id);
		}
		else {
			return BCM_E_PARAM;
		}
	}
}

/* Fill RMEP-entry struct */
int _bcm_oam_rmep_db_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY * rmep_db_entry) {
	int rv = 0;
	SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA punt_profile_data;
	int is_allocated;
	int profile;
	uint32 soc_sand_rv;

	BCMDNX_INIT_FUNC_DEFS;

	rmep_db_entry->ccm_period = endpoint_info->ccm_period;
	rmep_db_entry->loc_clear_threshold = endpoint_info->loc_clear_threshold;
	if (SOC_IS_ARADPLUS(unit)) {
		SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA_clear(&punt_profile_data);
		punt_profile_data.punt_enable = (endpoint_info->sampling_ratio > 0) ? 1 : 0;
		punt_profile_data.punt_rate = endpoint_info->sampling_ratio;
		punt_profile_data.rx_state_update_enable =   (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
                                                     (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RX_REMOTE_EVENT_DISABLE ) ? 1 : 
			                                         (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RX_REMOTE_DEFECT_AUTO_UPDATE ) ? 3 : 2;
		punt_profile_data.scan_state_update_enable = (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
                                                     (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE) ? 1 : 
			                                         (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE) ? 3 : 2;
		punt_profile_data.mep_rdi_update_loc_enable = (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RDI_ON_LOC) !=0;
		punt_profile_data.mep_rdi_update_loc_clear_enable = (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RDI_CLEAR_ON_LOC_CLEAR) !=0;
		punt_profile_data.mep_rdi_update_rx_enable = (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI) !=0;
		rv = _bcm_dpp_am_template_oam_punt_event_hendling_profile_alloc(unit, 0 /*flags*/, &punt_profile_data, &is_allocated, &profile);
		BCMDNX_IF_ERR_EXIT(rv);
		if (is_allocated) {
			soc_sand_rv = soc_ppd_oam_oamp_punt_event_hendling_profile_set(unit, profile, &punt_profile_data);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
		}
		rmep_db_entry->punt_profile = profile;
		if (SOC_IS_ARADPLUS(unit)) {
			_BCM_DPP_OAM_RMEP_STATE_FROM_PORT_INTERFACE_STATUS(rmep_db_entry->rmep_state, endpoint_info);
		}

	}
	else {
		rmep_db_entry->is_state_auto_handle = ((endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE) != 0);
		rmep_db_entry->is_event_mask = ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE) != 0);
	}


exit:
    BCMDNX_FUNC_RETURN;
}


/* Read RMEP-entry struct */
int _bcm_oam_rmep_db_entry_struct_get(int unit, SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY * rmep_db_entry, uint16 rmep_id, uint32 local_id, bcm_oam_endpoint_info_t *endpoint_info) {
	SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA punt_profile_data;
	uint32 soc_sand_rv;

	BCMDNX_INIT_FUNC_DEFS;
	endpoint_info->name = rmep_id;
	endpoint_info->local_id = local_id;
    endpoint_info->ccm_period = rmep_db_entry->ccm_period;
    endpoint_info->loc_clear_threshold = rmep_db_entry->loc_clear_threshold;
	endpoint_info->flags |= BCM_OAM_ENDPOINT_REMOTE;
	endpoint_info->faults |= rmep_db_entry->rdi_received ? BCM_OAM_ENDPOINT_FAULT_REMOTE : 0;
	endpoint_info->faults |= rmep_db_entry->loc ? BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT : 0;
	if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) {
		SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA_clear(&punt_profile_data);
		soc_sand_rv = soc_ppd_oam_oamp_punt_event_hendling_profile_get(unit, rmep_db_entry->punt_profile, &punt_profile_data);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        endpoint_info->sampling_ratio = punt_profile_data.punt_rate;
        switch (punt_profile_data.rx_state_update_enable) {
        case 0:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE;
            break;
        case 1:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT2_RX_REMOTE_EVENT_DISABLE;
            break;
        case 3:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT2_RX_REMOTE_DEFECT_AUTO_UPDATE;
            break;
        default:
            break;
        }
        switch (punt_profile_data.scan_state_update_enable) {
        case 0:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE;
            break;
        case 1:
            endpoint_info->flags |= BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE;
            break;
        case 3:
            endpoint_info->flags |= BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE;
            break;
        default:
			break;
        }

		if (SOC_IS_ARADPLUS(unit)) {
			_BCM_DPP_OAM_RMEP_STATE_TO_PORT_INTERFACE_STATUS(rmep_db_entry->rmep_state, endpoint_info);
		}

        endpoint_info->flags2 |= punt_profile_data.mep_rdi_update_loc_enable ? BCM_OAM_ENDPOINT2_RDI_ON_LOC : 0;
        endpoint_info->flags2 |= punt_profile_data.mep_rdi_update_loc_clear_enable ? BCM_OAM_ENDPOINT2_RDI_CLEAR_ON_LOC_CLEAR : 0;
        endpoint_info->flags2 |= punt_profile_data.mep_rdi_update_rx_enable ? BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI : 0;
	} else if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
		endpoint_info->flags |= rmep_db_entry->is_state_auto_handle ? BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE : 0;
		endpoint_info->flags |= rmep_db_entry->is_event_mask ? BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE : 0;
	} 

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Fill MEP-entry struct */
int _bcm_oam_mep_db_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPD_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry, int icc_ndx) {

	uint32 ret;
	SOC_PPD_LLP_PARSE_TPID_VALUES tpid_vals;
	uint32 soc_sand_rv;
	int tpid_index;
    uint32 ccm_period_ms;
	int is_allocated, push_profile;
	SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
	int priority;
    uint32 alloc_flags;
    _bcm_dpp_gport_info_t gport_info;
    int ignored, local_port;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_BCM_MEP_TYPE_TO_MEP_DB_MEP_TYPE(endpoint_info->type, mep_db_entry->mep_type);

    mep_db_entry->remote_recycle_port = 0;


    /* src mac handled differently for Arad and Jericho*/
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* common utility: always returns success */
        ret = _bcm_petra_mac_to_sand_mac(endpoint_info->src_mac_address, &(mep_db_entry->src_mac_address)); 
        BCMDNX_IF_ERR_EXIT(ret);
    } else {
        uint8 src_mac_address_msbs[5]={0};
        int profile;

        mep_db_entry->src_mac_lsb = endpoint_info->src_mac_address[5];
        sal_memcpy(src_mac_address_msbs,&(endpoint_info->src_mac_address),5 );
        ret = _bcm_dpp_am_template_oam_sa_mac_address_alloc(unit,0,src_mac_address_msbs,&is_allocated,&profile);
        BCMDNX_IF_ERR_EXIT_MSG(ret, (_BSL_BCM_MSG("Only two different values for the source MAC 5 MSBs allowed.")));

        if (is_allocated) {
            ret = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_sa_addr_msbs_set, (unit, profile, src_mac_address_msbs)); 
            BCMDNX_IF_ERR_EXIT(ret);
        }
    }

    mep_db_entry->icc_ndx = icc_ndx;
    /* Handle the local port*/

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) == 0 || _BCM_OAM_IS_SERVER_SERVER(endpoint_info)  ) {
        /* Down MEP: system port (taken from tx-gport) should go on the ITMH.*/
		/* Server: down MEP acts like normal down MEP, up MEP allocates an entry in this table as well for an ITMH built by the OAMP-PE.
		   In the latter case the destination is taken from the remote_gport field which should represent the remote device recycle port.*/
        uint32 sys_port=0;
        bcm_gport_t gport_to_use = (_BCM_OAM_IS_SERVER_SERVER(endpoint_info) && (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING))?
            endpoint_info->remote_gport :endpoint_info->tx_gport ;

        if (SOC_IS_ARAD_B0_AND_ABOVE(unit) &&  PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
            if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info)  && (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING)) {
                /* In this special case local port serves three purposes: 1) pointer to local_port_2_sys_port which will serve as the SSP on the ITMH
                   which should be the remote device recycle port and should be retreived from the remote_gport.
                   2) Pointer to the OAMP-GEN_MEM table from which the local port itself is taken.
                   This will be the SSP on the inner PTCH, and is retreived from the tx_gport.
                    Additionaly in Arad this serves as the LSB of the src mac address, which has already been verified to match the given value.*/
                uint32 local_port_for_real;
                ret = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->tx_gport, 0, &gport_info);
                BCMDNX_IF_ERR_EXIT(ret); 
                local_port_for_real  = gport_info.local_port; 
                local_port = endpoint_info->src_mac_address[5];

                ret = _bcm_dpp_gport_to_phy_port(unit, gport_to_use, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
                BCMDNX_IF_ERR_EXIT(ret);
                mep_db_entry->remote_recycle_port = gport_info.sys_port;
                ret = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, SHR_RES_ALLOC_WITH_ID, &gport_info.sys_port, &ignored, &local_port);
                BCMDNX_IF_ERR_EXIT(ret); 

                ret = _bcm_dpp_am_template_oam_oamp_pe_gen_mem_alloc(unit, SHR_RES_ALLOC_WITH_ID, &local_port_for_real, &ignored, &local_port);
                BCMDNX_IF_ERR_EXIT(ret); 
                
                ret = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, local_port, local_port_for_real));
                BCMDNX_IF_ERR_EXIT(ret); 

                mep_db_entry->local_port = local_port;
            } else {
                if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info) && BCM_GPORT_IS_TRUNK(endpoint_info->tx_gport)) {
                    /* Down-MEP, the tx gport is a LAG group. extract the trunk ID and that goes on the ITMH*/

                    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_logical_sys_id_build_with_device, (unit, TRUE, BCM_GPORT_TRUNK_GET(endpoint_info->tx_gport), 0, 0,&sys_port)));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

                } else {
                    ret = _bcm_dpp_gport_to_phy_port(unit, gport_to_use, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
                    BCMDNX_IF_ERR_EXIT(ret);
                    sys_port = gport_info.sys_port;
                }

				/* Normal case: Allocating for the LOCAL_PORT_2_SYS_PORT table, used only in down-MEPS, Arad B0 and above*/
                BCM_DPP_UNIT_CHECK(unit);/* not really needed. just to aviod coverity defect */
				alloc_flags = SOC_IS_JERICHO(unit) ? 0 : SHR_RES_ALLOC_WITH_ID; 
				local_port = endpoint_info->src_mac_address[5]; /* In Jericho this will be overriden.*/ 
				ret = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit,alloc_flags,&sys_port,&ignored,&local_port);
				BCMDNX_IF_ERR_EXIT(ret);
				mep_db_entry->local_port = local_port;
			}
        } else {
            /* Bug in Arad A0: system port taken directly from MEP DB.LOCAL_PORT.*/
            ret = _bcm_dpp_gport_to_phy_port(unit, gport_to_use, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
            BCMDNX_IF_ERR_EXIT(ret);
            sys_port = gport_info.sys_port;
            mep_db_entry->local_port = gport_info.sys_port; 
        }

        if (!_BCM_OAM_IS_SERVER_SERVER(endpoint_info)  || !(endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) ) {
			/* all cases except server - up mep.*/
			mep_db_entry->system_port = sys_port; 
        }
    } else {
        /* in this case local port is taken from the gport. set it. */
        _bcm_dpp_gport_info_t local_gport_info;
        ret = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &local_gport_info);
        BCMDNX_IF_ERR_EXIT(ret);
        mep_db_entry->local_port  = local_gport_info.local_port;
        if (!_BCM_OAM_IS_SERVER_SERVER(endpoint_info) && SOC_IS_ARADPLUS_A0(unit) ) {
			/* Up MEP case. The LSB of the MAC address will be set on the counter pointer (which is otherwise unused for up-MEPs)
			   and taken by the OAMP PE and set on the actual src MAC addr*/
			mep_db_entry->counter_pointer = endpoint_info->src_mac_address[5] <<1;  /* The leftmost bit cannot be used by the OAMP. shift by one.*/ 
        }
    }

	mep_db_entry->mdl = endpoint_info->level;
	mep_db_entry->mep_id = endpoint_info->name;     
	ccm_period_ms = endpoint_info->ccm_period;
	SOC_PPD_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(ccm_period_ms, 0, mep_db_entry->ccm_interval);
	mep_db_entry->rdi = ((endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_TX) != 0);  

	/* ITMH attributes profile allocate */
    SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
	/* int_pri:  ((COS & 0x7) << 2) + (DP & 0x3)) */
	tx_itmh_attributes.tc = (endpoint_info->int_pri & 0x1F) >> 2;
	tx_itmh_attributes.dp = endpoint_info->int_pri & 0x3;
	ret = _bcm_dpp_am_template_oam_tx_priority_alloc(unit, 0/*flags*/, &tx_itmh_attributes, &is_allocated, &priority);
	BCMDNX_IF_ERR_EXIT(ret);
	mep_db_entry->priority = (uint8)priority;
	if (is_allocated) {
		soc_sand_rv = soc_ppd_oam_oamp_tx_priority_registers_set(unit, mep_db_entry->priority, &tx_itmh_attributes);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
	}

	if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
		mep_db_entry->is_upmep = ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) != 0); 

		if (endpoint_info->outer_tpid == 0) {     /* when single tag - outer tag fields are used */
			if (endpoint_info->inner_tpid != 0) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Inner tpid can not be configured without an outer tpid.\n")));
			}        
			mep_db_entry->tags_num = 0;
		} else {
			soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			ret = _bcm_petra_vlan_translate_match_tpid_value_to_index(unit, endpoint_info->outer_tpid, &tpid_vals, &tpid_index); 
			BCMDNX_IF_ERR_EXIT(ret);
			mep_db_entry->outer_tpid = (uint8)tpid_index; 
			mep_db_entry->outer_vid_dei_pcp =  endpoint_info->vlan + (endpoint_info->pkt_pri << 12); /*pcp=3 dei=1 vlan=12*/
			if (endpoint_info->inner_tpid != 0) { /* double tag */
				mep_db_entry->tags_num = 2;
				ret = _bcm_petra_vlan_translate_match_tpid_value_to_index(unit, endpoint_info->inner_tpid, &tpid_vals, &tpid_index); 
				BCMDNX_IF_ERR_EXIT(ret);
				mep_db_entry->inner_tpid = (uint8)tpid_index;
				mep_db_entry->inner_vid_dei_pcp = endpoint_info->inner_vlan + (endpoint_info->inner_pkt_pri << 12); /*pcp=3 dei=1 vlan=12*/
			} else { /* double tag */
				mep_db_entry->tags_num = 1;
			}
		}
	}
    else {
		SOC_PPD_MPLS_PWE_PROFILE_DATA push_data;

		SOC_PPD_MPLS_PWE_PROFILE_DATA_clear(&push_data);
		mep_db_entry->label = endpoint_info->egress_label.label;
		push_data.ttl = endpoint_info->egress_label.ttl;
		push_data.exp = endpoint_info->egress_label.exp;
		ret = _bcm_dpp_am_template_mpls_pwe_push_profile_alloc(unit, 0/*flags*/, &push_data, &is_allocated, &push_profile);
		BCMDNX_IF_ERR_EXIT(ret);
		mep_db_entry->push_profile = (uint8)push_profile;
		if (is_allocated) {
			soc_sand_rv = soc_ppd_oam_mpls_pwe_profile_set(unit, mep_db_entry->push_profile, &push_data);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
		}

		mep_db_entry->egress_if = _BCM_PETRA_L3_ITF_VAL_GET(endpoint_info->intf_id);
	}
	if (SOC_IS_ARADPLUS(unit)) {
		int dont_care;
		(void) dont_care; /* for compilation*/
        if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) == 0) {
			_BCM_PETRA_PROC_CNTR_FROM_STAT(dont_care, mep_db_entry->counter_pointer, endpoint_info->lm_counter_base_id); 
        }

		if (SOC_IS_ARADPLUS(unit)) {
			if (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_UPDATE) {
				mep_db_entry->port_status_tlv_en = 1;
				mep_db_entry->port_status_tlv_val = endpoint_info->port_state - 1; /* see documentation on definition of this field.*/
			} else if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE) {
				mep_db_entry->interface_status_tlv_control = endpoint_info->interface_state;
			}
		}
	}

exit:
    BCMDNX_FUNC_RETURN;
}

/* Read MEP-entry struct */
int _bcm_oam_mep_db_entry_struct_get(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPD_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry) {

	uint32 ret, soc_sand_rv;
	SOC_PPD_LLP_PARSE_TPID_VALUES tpid_vals;
    uint32 ccm_period_ms, ccm_period_micro_s;
	SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;

    BCMDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(ccm_period_micro_s);

	switch (mep_db_entry->mep_type) {
	case SOC_PPD_OAM_MEP_TYPE_ETH_OAM:
		endpoint_info->type = bcmOAMEndpointTypeEthernet;
		break;
	case SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP:
		endpoint_info->type =  bcmOAMEndpointTypeBHHMPLS;
		break;
	case SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE:
		endpoint_info->type = bcmOAMEndpointTypeBHHPwe;
		break;
	default:
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type.")));
	}

    /* src mac: handled per device*/
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* common utility: always returns sucess */
        ret = _bcm_petra_mac_from_sand_mac(endpoint_info->src_mac_address, &(mep_db_entry->src_mac_address));
        if (mep_db_entry->is_upmep && SOC_IS_ARADPLUS_A0(unit)) {
            endpoint_info->src_mac_address[5] = mep_db_entry->counter_pointer >>1; /* The LSB was set on the counter pointer (without the leftmost bit) and applied via the OAMP-PE.*/
        } 
	} else {
        uint8 data[5];
        ret = _bcm_dpp_am_template_oam_sa_mac_address_get(unit,mep_db_entry->src_mac_msb_profile,data);
        BCMDNX_IF_ERR_EXIT(ret);
        sal_memcpy(endpoint_info->src_mac_address, data, 5);
        endpoint_info->src_mac_address[5] = mep_db_entry->src_mac_lsb;
    }


    if (!mep_db_entry->is_upmep) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, mep_db_entry->system_port); 
    } else {
        /* For Up-MEPs the tx gport is not used. */
        endpoint_info->tx_gport = BCM_GPORT_INVALID;
    }

    endpoint_info->level = mep_db_entry->mdl;
	endpoint_info->name = mep_db_entry->mep_id;
	SOC_PPD_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, mep_db_entry->ccm_interval);
	endpoint_info->ccm_period = ccm_period_ms;

	if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
		if (mep_db_entry->tags_num != 0) { /* not single tag */
			soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			ret = _bcm_petra_vlan_translate_match_tpid_index_to_value(unit, mep_db_entry->outer_tpid, &tpid_vals, &endpoint_info->outer_tpid);
			BCMDNX_IF_ERR_EXIT(ret);
			if (mep_db_entry->tags_num == 2) {
				ret = _bcm_petra_vlan_translate_match_tpid_index_to_value(unit, mep_db_entry->inner_tpid, &tpid_vals, &endpoint_info->inner_tpid);
				BCMDNX_IF_ERR_EXIT(ret);
			}
		}

		endpoint_info->flags |= mep_db_entry->is_upmep ? BCM_OAM_ENDPOINT_UP_FACING : 0;
		endpoint_info->inner_vlan = mep_db_entry->inner_vid_dei_pcp & 0xFFF;	   
		endpoint_info->inner_pkt_pri = mep_db_entry->inner_vid_dei_pcp >> 12 & 0xF;     
		endpoint_info->vlan = mep_db_entry->outer_vid_dei_pcp & 0xFFF;
		endpoint_info->pkt_pri = mep_db_entry->outer_vid_dei_pcp >> 12 & 0xF;
	}
    else {
		SOC_PPD_MPLS_PWE_PROFILE_DATA push_data;

		endpoint_info->egress_label.label = mep_db_entry->label;
		SOC_PPD_MPLS_PWE_PROFILE_DATA_clear(&push_data);
		ret = _bcm_dpp_am_template_mpls_pwe_push_profile_data_get(unit, mep_db_entry->push_profile, &push_data);
		BCMDNX_IF_ERR_EXIT(ret);
		endpoint_info->egress_label.ttl = push_data.ttl;
		endpoint_info->egress_label.exp = push_data.exp;

		_BCM_PETRA_L3_ITF_SET(endpoint_info->intf_id, _BCM_PETRA_L3_ITF_ENCAP, mep_db_entry->egress_if);
	}

	/* ITMH attributes profile get */
    SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
	ret = _bcm_dpp_am_template_oam_tx_priority_data_get(unit, mep_db_entry->priority, &tx_itmh_attributes);
	BCMDNX_IF_ERR_EXIT(ret);
	/* int_pri:  ((COS & 0x7) << 2) + (DP & 0x3)) */
	endpoint_info->int_pri = ((tx_itmh_attributes.tc & 0x7) << 2) + (tx_itmh_attributes.dp & 0x3);

	endpoint_info->flags |= (mep_db_entry->rdi )? BCM_OAM_ENDPOINT_RDI_TX : 0;
	if (SOC_IS_ARADPLUS(unit)) {
		if (mep_db_entry->interface_status_tlv_control) {
			endpoint_info->flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
			endpoint_info->interface_state = mep_db_entry->interface_status_tlv_control;
		} else if (mep_db_entry->port_status_tlv_en) {
			endpoint_info->flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
			endpoint_info->port_state = mep_db_entry->port_status_tlv_val +1 ; /* See struct definition for documentation.*/
		}
	}


	BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}




/* Fill classifier default entry struct */
int _bcm_oam_classifier_default_mep_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry) {

	uint32 proc_dummy;

    BCMDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(proc_dummy);

    classifier_mep_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_ETH_OAM; /* Irrelevant to default. Setting to 0 */
    classifier_mep_entry->flags |= ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) != 0) ? SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP : 0;
    classifier_mep_entry->flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
    classifier_mep_entry->md_level = endpoint_info->level;

    _BCM_OAM_COUNTER_BASE_ID_TO_CLASSIFIER_COUNTER(endpoint_info, classifier_mep_entry);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Fill classifier entry struct */
int _bcm_oam_classifier_mep_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry) {

	int fec_id, is_local;
	uint32 ret;
	uint32 proc_dummy;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;
    
    COMPILER_REFERENCE(proc_dummy);

	ret = _bcm_petra_mac_to_sand_mac(endpoint_info->dst_mac_address, &(classifier_mep_entry->dst_mac_address));
	BCMDNX_IF_ERR_EXIT(ret);

	_BCM_OAM_BCM_MEP_TYPE_TO_MEP_DB_MEP_TYPE(endpoint_info->type, classifier_mep_entry->mep_type)

	classifier_mep_entry->flags |= ((endpoint_info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) != 0) ? 0 : SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
	classifier_mep_entry->md_level = endpoint_info->level;
	classifier_mep_entry->flags |= ((endpoint_info->opcode_flags & BCM_OAM_OPCODE_CCM_IN_HW)!= 0) ? SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED : 0;
	classifier_mep_entry->flags |= ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) != 0) ? SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP : 0;


    _BCM_OAM_COUNTER_BASE_ID_TO_CLASSIFIER_COUNTER(endpoint_info, classifier_mep_entry);

    if (endpoint_info->gport != BCM_GPORT_INVALID) {
        ret = _bcm_dpp_gport_to_lif(unit, endpoint_info->gport, (int *)&classifier_mep_entry->lif, NULL, &fec_id, &is_local);
        BCMDNX_IF_ERR_EXIT(ret);
    }
    else {
        classifier_mep_entry->lif = _BCM_OAM_INVALID_LIF;
    }
	if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
		ret = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
		BCMDNX_IF_ERR_EXIT(ret);
        classifier_mep_entry->port = gport_info.local_port;
	}
	classifier_mep_entry->flags |= (endpoint_info->timestamp_format == bcmOAMTimestampFormatIEEE1588v1) ? SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588 : 0;
	classifier_mep_entry->ma_index = endpoint_info->group;
	
    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) == 0 && BCM_GPORT_IS_TRUNK(endpoint_info->tx_gport) )  {
        classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Read classifier entry struct */
int _bcm_oam_classifier_mep_entry_struct_get(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry) {
	uint32 ret;

    BCMDNX_INIT_FUNC_DEFS;

	switch (classifier_mep_entry->mep_type) {
	case SOC_PPD_OAM_MEP_TYPE_ETH_OAM:
		endpoint_info->type = bcmOAMEndpointTypeEthernet;
		break;
	case SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP:
		endpoint_info->type =  bcmOAMEndpointTypeBHHMPLS;
		break;
	case SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE:
		endpoint_info->type = bcmOAMEndpointTypeBHHPwe;
		break;
	default:
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type.")));
	}

	ret = _bcm_petra_mac_from_sand_mac(endpoint_info->dst_mac_address, &(classifier_mep_entry->dst_mac_address));
	BCMDNX_IF_ERR_EXIT(ret);

	endpoint_info->flags |= ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)== 0) ? BCM_OAM_ENDPOINT_INTERMEDIATE : 0;
	endpoint_info->opcode_flags = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)!= 0) ? BCM_OAM_OPCODE_CCM_IN_HW : 0;
	endpoint_info->flags |= ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP)!= 0)? BCM_OAM_ENDPOINT_UP_FACING : 0;
	endpoint_info->timestamp_format = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588) != 0) ? bcmOAMTimestampFormatIEEE1588v1 : bcmOAMTimestampFormatNTP;


    _BCM_OAM_COUNTER_BASE_ID_FROM_CLASSIFIER_COUNTER(endpoint_info, classifier_mep_entry);

	endpoint_info->level = classifier_mep_entry->md_level;
    if (classifier_mep_entry->lif != _BCM_OAM_INVALID_LIF) {
        endpoint_info->gport = _bcm_dpp_in_lif_match_gport_get(unit, classifier_mep_entry->lif);
    }
    else {
        endpoint_info->gport = BCM_GPORT_INVALID;
    }

	endpoint_info->group = classifier_mep_entry->ma_index;

	BCMDNX_IF_ERR_EXIT(ret);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_oam_default_profile_get(int unit, 
								  SOC_PPD_OAM_LIF_PROFILE_DATA *profile_data, 
								  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry, 
								  uint8 is_accelerated,
								  uint8 is_default,
                                  uint8 is_upmep) {
	uint32 opcode;
	uint32 upmep_trap_id_cpu;
	uint32 trap_id_cpu;
	uint32 trap_id_err_level, trap_id_snoop, trap_id_recycle;
    uint32 meter_disable_lcl[1];
    uint8 internal_opcode;
	int rv;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
        profile_data->is_default = is_default;
    }
	else {
		/* New Arad+ Classifier: in this case the mp_type_passive_active_mix will be always 0 unless default endpoint is confgured */
        if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
            profile_data->mp_type_passive_active_mix = 0;
        }
        else {
            profile_data->mp_type_passive_active_mix = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)!= 0) ? 1 : 0;
        }
	}

	profile_data->is_1588 = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588) != 0);

	rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, 
                                   SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_UPMEP_TRAP_IDS, 
                                   &upmep_trap_id_cpu, 
                                   SOC_PPD_OAM_UPMEP_TRAP_ID_CPU);
    BCMDNX_IF_ERR_EXIT(rv);
	rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, 
								 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS, 
								 &trap_id_cpu, 
								 SOC_PPD_OAM_TRAP_ID_CPU);
	BCMDNX_IF_ERR_EXIT(rv);
	rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, 
							 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS, 
							 &trap_id_err_level, 
							 SOC_PPD_OAM_TRAP_ID_ERR_LEVEL);
	BCMDNX_IF_ERR_EXIT(rv);

	rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, 
							 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS, 
							 &trap_id_snoop, 
							 SOC_PPD_OAM_TRAP_ID_SNOOP);
	BCMDNX_IF_ERR_EXIT(rv);
	rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, 
							 SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS, 
							 &trap_id_recycle, 
							 SOC_PPD_OAM_TRAP_ID_RECYCLE);
	BCMDNX_IF_ERR_EXIT(rv);

	/* configure non accelerated */
	for (opcode = 1; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
        BCMDNX_IF_ERR_EXIT(rv);
        /* configure counter disable */
        SHR_BITSET(&profile_data->mep_profile_data.counter_disable, internal_opcode);
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            /* Added because of a HW bug  (fixed in Arad+) - LMMs and related must be counted. */
            if ((opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMR) ||
                (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLR) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLM)) {
                SHR_BITCLR(&profile_data->mep_profile_data.counter_disable, internal_opcode);
            }
        }
        /* configure meter disable */
        *meter_disable_lcl = profile_data->mep_profile_data.meter_disable;
        SHR_BITSET(meter_disable_lcl, internal_opcode);
        profile_data->mep_profile_data.meter_disable = *meter_disable_lcl; 

		/* configure multicast behavior */
		if (classifier_mep_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) { 
			 if ((opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LTM) ||
				 (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LMM) ||
				 (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_AIS) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_TST) ||
				 (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LINEAR_APS) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_SLM) ||
				 (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CSF) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_DMM))   { /* These opcodes trap in multicast */
                 /* All supported multicast packets are trapped to the CPU.*/
                 if (is_upmep) {
                     profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = upmep_trap_id_cpu;
                 } else {
                     profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_cpu;
                 }
			 } else {
                 /* Non supported opcode: trap to CPU with error level.*/
                 profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_err_level;
			 }
		} else { /* MPLS and PWE: Only MC packets supported */
			 profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_cpu;
		}

		/* configure unicast behavior */
		if (classifier_mep_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) { 
            if (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM) {
				 if (is_upmep) {
					 profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = upmep_trap_id_cpu;
				 }
				 else {
					 /* recycle */
					 profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_recycle;
				 }
            }
            else if (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM) {
				 profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_err_level;
			 }
			 else {
                 if (is_upmep) {
                     
                     profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = upmep_trap_id_cpu;

                 } else {
                     profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_cpu; 
				 }
			 }

		}
		else { /* MPLS and PWE: Only MC packets supported */
			 profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_err_level;
		}

	   /* configure MIP: Only LBM and LTM are handled in MIP */
        if ((opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM) || (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LTM)) {
			SHR_BITSET(&profile_data->mip_profile_data.counter_disable, internal_opcode);
            *meter_disable_lcl = profile_data->mip_profile_data.meter_disable;
			SHR_BITSET(meter_disable_lcl, internal_opcode);
            profile_data->mip_profile_data.meter_disable = *meter_disable_lcl; 

            if (opcode == SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LTM) {
				profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_cpu;
				profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_snoop;
            }
            else {
				/* recycle */
				profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_recycle;
				profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = _BCM_PETRA_UD_DFLT_TRAP; /* Forward */
			}
		} else { /* MIPs are transparent to all but LTM, LBM. forward the M-C packets, drop  (trap with err level) the U-C paclkets*/
			SHR_BITSET(&profile_data->mip_profile_data.counter_disable, internal_opcode);
			profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = _BCM_PETRA_UD_DFLT_TRAP; /* Forward */
			profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_err_level;
		}
	}

	/* Configure trap OAMP destination if packet is accelerated (only multicast) */
	if (is_accelerated) {
        BCM_OAM_OAMP_DESTINATION_TRAP_CODE_GET(
           is_upmep, classifier_mep_entry->mep_type, profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM]);
	}

	BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Function finds a mirror profile, first searching for one 
 * where the CPU trap code matches the given paramter and then a 
 * new one. 
 * 
 * @param unit 
 * @param free_mirror_profile 
 * @param cpu_trap_code 
 * 
 * @return int 
 */
int _bcm_dpp_oam_free_mirror_profile_find(int unit, uint32 * free_mirror_profile, uint32 cpu_trap_code) 
{
	int32 mirror_id;
	int first_free_prof, first_matching_prof;
	uint32 gotten_data, ref_count;
	int rv;
	BCMDNX_INIT_FUNC_DEFS;

	first_free_prof = first_matching_prof = DPP_MIRROR_ACTION_NDX_MAX+1;

	for (mirror_id = DPP_MIRROR_ACTION_NDX_MAX; mirror_id > 0; mirror_id--) {
		rv = _bcm_dpp_am_template_mirror_action_profile_ref_get(unit,mirror_id, &ref_count);
		BCMDNX_IF_ERR_EXIT(rv);
		if (ref_count==0) {
			first_free_prof = (first_free_prof== DPP_MIRROR_ACTION_NDX_MAX+1)? mirror_id  : first_free_prof;
		} else {
			rv = _bcm_dpp_am_template_mirror_action_profile_get(unit,mirror_id,&gotten_data);
			BCMDNX_IF_ERR_EXIT(rv);
			if (gotten_data== cpu_trap_code) {
				first_matching_prof = mirror_id;
				break;
			}
		}
	}

	if (first_matching_prof != (DPP_MIRROR_ACTION_NDX_MAX + 1)) {
		*free_mirror_profile = first_matching_prof;
	} else if (first_free_prof != (DPP_MIRROR_ACTION_NDX_MAX + 1)) {
		*free_mirror_profile = first_free_prof;
	} else {
		BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
							(_BSL_BCM_MSG("Error: No free mirror profile found\n")));
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_mep_profile_trap_data_free(int unit, SOC_PPD_OAM_MEP_PROFILE_DATA *profile_data, uint8 is_mep) {

    uint32 trap_codes[SOC_PPD_NOF_TRAP_CODES] = {0};
    uint32 trap_code;
	uint32 opcode;
    uint8  internal_opcode;
	uint32 mirror_id, mirror_trap_dest;
	uint32 rv;
	uint32 soc_sand_rv = SOC_SAND_OK;
	uint32 snoop_strength_dummy;
	bcm_rx_trap_config_t trap_config;
	uint32 local_port;
	uint8 wb_mirror_profile;
	uint8 no_traps;
    _bcm_dpp_gport_info_t gport_info;
    int core = SOC_CORE_INVALID;
	uint32		pp_port;

	BCMDNX_INIT_FUNC_DEFS;

	for (opcode = 1; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
		 /* Translate to internal opcode */
         rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
		 BCMDNX_IF_ERR_EXIT(rv);
		 trap_code = profile_data->opcode_to_trap_code_unicast_map[internal_opcode];
		 trap_codes[trap_code] = 1;
		 trap_code = profile_data->opcode_to_trap_code_multicast_map[internal_opcode];
		 trap_codes[trap_code] = 1;
	} 

	/* Starting from trap code 1 - 0 is not valid */
	for (trap_code = 1; trap_code < SOC_PPD_NOF_TRAP_CODES; trap_code++) {
		if (trap_codes[trap_code] && (trap_code != _BCM_PETRA_UD_DFLT_TRAP)) {
			rv = _bcm_dpp_oam_traps_ref_counter_decrease(unit, trap_code);
			BCMDNX_IF_ERR_EXIT(rv);
			rv = _bcm_dpp_oam_traps_ref_counter_is_zero(unit, trap_code, &no_traps);
			BCMDNX_IF_ERR_EXIT(rv);
			if (no_traps) {
				/* Get destination trap port */
				bcm_rx_trap_config_t_init(&trap_config);
				rv = bcm_petra_rx_trap_get(unit, trap_code, &trap_config);
				BCMDNX_IF_ERR_EXIT(rv);
                if (trap_config.snoop_cmnd > 0) {
					continue; /* Do not free snoop traps, they are configured by the user and should be freed by him */
                }
				if (trap_config.flags & BCM_RX_TRAP_UPDATE_DEST) {
					/* set oam port profile to 0 */
					rv = _bcm_dpp_gport_to_phy_port(unit, trap_config.dest_port, 0, &gport_info); 
                    BCMDNX_IF_ERR_EXIT(rv);
                    BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
                        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &pp_port, &core)));

                    	soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, 0);
	                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
				}

				rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloced_check(unit,trap_code);
				if (rv != BCM_E_EXISTS) {
					/* Free traps */
					rv = bcm_petra_rx_trap_type_destroy(unit, trap_code);
					BCMDNX_IF_ERR_EXIT(rv);
				}
                else {
                    /* No need to free meta traps - only mark that they can be reused */
                    rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(unit, trap_code);
					BCMDNX_IF_ERR_EXIT(rv);
                }
				

				if (!is_mep) {
					rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &wb_mirror_profile, trap_code);
					BCMDNX_IF_ERR_EXIT(rv);
					/* In this case the trap code in the mirror profile will presumably be an fthm trap code. If so, free it as well.*/
					soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_get(unit, wb_mirror_profile, &mirror_trap_dest, &snoop_strength_dummy);
					BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
					rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloced_check(unit, mirror_trap_dest);
					if (rv == BCM_E_EXISTS) {
						int is_last;
						/* No need to free meta traps - only mark that they can be reused */
						rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(unit, mirror_trap_dest);
						BCMDNX_IF_ERR_EXIT(rv);
						/* free the mirror profile as well*/
						rv = _bcm_dpp_am_template_mirror_action_profile_free(unit, wb_mirror_profile, &is_last);
						if (is_last) {
							soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, wb_mirror_profile, 0, 0, 7);
							BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
						}
					}
				} else {
					/* search for mirror profile that points to the same trap code */
					for (mirror_id = DPP_MIRROR_ACTION_NDX_MAX; mirror_id > 0; mirror_id--) {
						rv = _bcm_dpp_am_template_mirror_action_profile_get(unit, mirror_id, &mirror_trap_dest);
						if (rv != BCM_E_NOT_FOUND) {
							BCMDNX_IF_ERR_EXIT(rv);
						}
						if (mirror_trap_dest == trap_code) {
							int is_last;
							rv = _bcm_dpp_am_template_mirror_action_profile_free(unit, mirror_id, &is_last);
							if (is_last) {
								soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, mirror_id, 0, 0, 7);
								BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
							}
							break;
						}
					}
				}


				wb_mirror_profile = 0;
				rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &wb_mirror_profile, trap_code);
				BCMDNX_IF_ERR_EXIT(rv);
			}
		}
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_mep_profile_trap_data_alloc(int unit, SOC_PPD_OAM_MEP_PROFILE_DATA *profile_data) {

    uint32 trap_codes[SOC_PPD_NOF_TRAP_CODES] = {0};
    uint32 trap_code;
	uint32 opcode;
    uint8  internal_opcode;
	int rv;

	BCMDNX_INIT_FUNC_DEFS;

	for (opcode = 0; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
		 /* Translate to internal event */
         rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
		 BCMDNX_IF_ERR_EXIT(rv);
		 trap_code = profile_data->opcode_to_trap_code_unicast_map[internal_opcode];
		 trap_codes[trap_code] = 1;
		 trap_code = profile_data->opcode_to_trap_code_multicast_map[internal_opcode];
		 trap_codes[trap_code] = 1;
	} 

	for (trap_code = 1; trap_code < SOC_PPD_NOF_TRAP_CODES; trap_code++) {
		if (trap_codes[trap_code]) {
			rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_code);
			BCMDNX_IF_ERR_EXIT(rv);
		}
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_lif_table_set(int unit, SOC_PPD_LIF_ID lif_index, uint8 value) {

	uint32 soc_sand_rv; 
	SOC_PPD_LIF_ENTRY_INFO *lif_entry_info = NULL;

	BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPD_LIF_ENTRY_INFO), "_bcm_dpp_oam_lif_table_set.lif_entry_info");
    if (lif_entry_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, lif_index, lif_entry_info);

    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

	switch (lif_entry_info->type) {
	case SOC_PPD_LIF_ENTRY_TYPE_AC:
	case SOC_PPD_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
	case SOC_PPD_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
		lif_entry_info->value.ac.oam_valid = value;
		break;
	case SOC_PPD_LIF_ENTRY_TYPE_PWE:
		lif_entry_info->value.pwe.oam_valid = value;
		break;
	case SOC_PPD_LIF_ENTRY_TYPE_ISID:
		lif_entry_info->value.isid.oam_valid = value;
		break;
	case SOC_PPD_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
		lif_entry_info->value.ip_term_info.oam_valid = value;
		break;
	case SOC_PPD_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
		lif_entry_info->value.mpls_term_info.oam_valid = value;
		break;
	default:
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index %d (extracted from gport)."), lif_index));
	}

	soc_sand_rv = soc_ppd_lif_table_entry_update(unit, lif_index, lif_entry_info);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCM_FREE(lif_entry_info);
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
 * Sets the relevant counters of an endpoint
 */
int _bcm_dpp_endpoint_counters_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_LIF_ID lif) {

    int rv;
    uint64 stat;
    SOC_TMC_CNT_PROCESSOR_ID processor_ndx;
    SOC_TMC_CNT_COUNTERS_INFO processor_info;
    SOC_TMC_CNT_SRC_TYPE ing_cnt_type, egr_cnt_type;
    int i;

	BCMDNX_INIT_FUNC_DEFS;

    if (endpoint_info->lm_counter_base_id > 0) {
        rv = _bcm_dpp_oam_set_counter(unit, endpoint_info->lm_counter_base_id, ((endpoint_info->lm_flags & BCM_OAM_LM_PCP)!=0), lif);
        BCMDNX_IF_ERR_EXIT(rv);

        if (endpoint_info->lm_counter_base_id > SOC_DPP_DEFS_GET(unit, counters_per_counter_processor)) {
            ing_cnt_type = SOC_TMC_CNT_SRC_TYPE_OAM_ING_D;
            egr_cnt_type = SOC_TMC_CNT_SRC_TYPE_OAM_ING_B;
        } else {
            ing_cnt_type = SOC_TMC_CNT_SRC_TYPE_OAM_ING_C;
            egr_cnt_type = SOC_TMC_CNT_SRC_TYPE_OAM_ING_A;
        }

        COMPILER_64_ZERO(stat);
        SOC_TMC_CNT_COUNTERS_INFO_clear(&processor_info);
        for (processor_ndx=0; processor_ndx<SOC_DPP_DEFS_GET(unit, nof_counter_processors); processor_ndx++) {
            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_cnt_counters_get,(unit, processor_ndx, &processor_info)));
            if ((processor_info.src_type==ing_cnt_type) || (processor_info.src_type==egr_cnt_type)) {
                if ((endpoint_info->lm_flags & BCM_OAM_LM_PCP)!=0) {
                    for ( i = 0; i < 8; i++) {
                        rv = bcm_dpp_counter_set(unit,
                                                 processor_ndx,
                                                 endpoint_info->lm_counter_base_id + i,
                                                 bcm_dpp_counter_pkts,
                                                 stat);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
                else {
                    rv = bcm_dpp_counter_set(unit,
                                             processor_ndx,
                                             endpoint_info->lm_counter_base_id,
                                             bcm_dpp_counter_pkts,
                                             stat);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_bfd_trap_code_from_remote_gport(int unit, 
                                                 bcm_gport_t remote_gport, 
                                                 uint32 *trap_code, 
                                                 uint8 meta_data, 
                                                 SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry) {
	int rv;
	uint32 soc_sand_rv;
	uint32 trap_code_i, mirror_profile;
	bcm_rx_trap_config_t trap_config;
	uint8 is_destination_new = TRUE;
	uint32 flags = 0;
	uint8 is_equal;
	uint32 snoop_strength, trap_code_dummy;
	uint32 local_port;
	uint8 wb_mirror_profile;
	uint8 no_traps;
	int snoop_cmnd;
    uint8 is_upmep, is_mep, is_accelerated;
    bcm_gport_t oamp_port, cpu_port, recycle_port;
    int count_erp;
    int trap_code_up;
    bcm_port_config_t port_config;
    _bcm_dpp_gport_info_t gport_info;
    int core = SOC_CORE_INVALID;
	uint32		pp_port;
	BCMDNX_INIT_FUNC_DEFS;

    is_upmep = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
    is_mep = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)!= 0);
    is_accelerated = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)!= 0);

	/* Check if destination is trap gport with snoop destination */
    if (BCM_GPORT_IS_TRAP(remote_gport)) {
		*trap_code = BCM_GPORT_TRAP_GET_ID(remote_gport);
		bcm_rx_trap_config_t_init(&trap_config);
		rv = bcm_petra_rx_trap_get(unit, *trap_code, &trap_config);
		BCMDNX_IF_ERR_EXIT(rv);
        if (trap_config.snoop_cmnd>0) {
             if (!is_mep) {
                 if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0) ) {
                     /* In this case the user presumably set a trap for at the ingress (down). Setting a trap with the same configurations at the egress*/
                     bcm_rx_snoop_config_t snoop_config;
                     bcm_rx_snoop_config_t_init(&snoop_config);
                     rv = bcm_petra_rx_snoop_get(unit, _BCM_OAM_MIP_SNOOP_CMND_DOWN, &snoop_config);
                     BCMDNX_IF_ERR_EXIT(rv);

                     rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, SOC_PPD_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH);
                     BCMDNX_IF_ERR_EXIT(rv); 
                     rv =  bcm_petra_rx_snoop_set(unit, _BCM_OAM_MIP_SNOOP_CMND_UP, &snoop_config);
                     BCMDNX_IF_ERR_EXIT(rv);

                     trap_config.snoop_cmnd = _BCM_OAM_MIP_SNOOP_CMND_UP;
                     /* set the trap with the same values set by the user. In the other direction*/
                     /*use the same trap code that was used for default MIPs */
                     rv = bcm_petra_rx_trap_set(unit, SOC_PPD_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH, &trap_config);
                     BCMDNX_IF_ERR_EXIT(rv);

                    /* Use the same mirror profile as the one allocated by init. */
                    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_MIRROR_IDS, &wb_mirror_profile, SOC_PPD_OAM_MIRROR_ID_SNOOP);
                    BCMDNX_IF_ERR_EXIT(rv); 

                    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &wb_mirror_profile, *trap_code);
                    BCMDNX_IF_ERR_EXIT(rv); 
                 } else {
                    snoop_cmnd = trap_config.snoop_cmnd;
                    /* Check that if MIP 2 consecutive trap codes are allocated */
                    rv = bcm_petra_rx_trap_get(unit, (*trap_code+1), &trap_config);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (trap_config.snoop_cmnd != snoop_cmnd) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("   Error: For MIP two consecutive trap codes with same snoop command should be allocated.")));
                    }
                 }
            }
        }
        /* Create mirror profile and map to trap code */
        if (is_upmep || ((!is_mep) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0) == 0)) {
            /* Allocate upmep trap code and mirror profile. For advanced snooping This has been done above. */
            _BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_profile, trap_code); 
            soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_get(unit, mirror_profile, &trap_code_dummy, &snoop_strength);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            snoop_strength = soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 3);
            soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, mirror_profile,  *trap_code, snoop_strength, 7);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            wb_mirror_profile = (uint8)mirror_profile;
            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &wb_mirror_profile, *trap_code);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!is_mep) {
                rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &wb_mirror_profile, (*trap_code) + 1);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

		
		BCM_EXIT;
    }

    if (is_accelerated) { /* If destination is OAMP, trap code should be one of the specially allocated in init */
        rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 1, &oamp_port, &count_erp); 
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_gport_compare(unit, oamp_port, remote_gport, &is_equal);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_equal) {
             BCM_OAM_OAMP_DESTINATION_TRAP_CODE_GET(is_upmep, classifier_mep_entry->mep_type, (*trap_code));
             BCM_EXIT;
        }
    }

    /* If destination is first CPU port, we will use existing traps */
    rv = bcm_petra_port_config_get(unit, &port_config);
    BCMDNX_IF_ERR_EXIT(rv);

    /*get CPU port*/
    BCM_PBMP_ITER(port_config.cpu, cpu_port) {
       break; /*get first CPU port*/
    }

    rv = _bcm_dpp_gport_compare(unit, cpu_port, remote_gport, &is_equal);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_equal) {
         BCM_OAM_CPU_DESTINATION_TRAP_CODE_GET(is_upmep, meta_data, (*trap_code));
         BCM_EXIT;
    }

    /* If destination is recycle port, we will use existing traps */
    /*get recycle port*/
    recycle_port = soc_property_get(unit, spn_OAM_RCY_PORT, -1);

    rv = _bcm_dpp_gport_compare(unit, recycle_port, remote_gport, &is_equal);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_equal) {
         BCM_OAM_CPU_DESTINATION_TRAP_CODE_GET(is_upmep, meta_data, (*trap_code));
         BCM_EXIT;
    }


	/* Check if destination is new one */
	for (trap_code_i = 1; trap_code_i < SOC_PPD_NOF_TRAP_CODES; trap_code_i++) {
		rv = _bcm_dpp_oam_traps_ref_counter_is_zero(unit, trap_code_i, &no_traps);
        BCMDNX_IF_ERR_EXIT(rv);
		if ((!no_traps) && (trap_code_i != _BCM_PETRA_UD_DFLT_TRAP) && (trap_code_i !=SOC_PPD_TRAP_CODE_OAM_LEVEL)  && (trap_code_i !=SOC_PPD_TRAP_CODE_OAM_PASSIVE)) {
			bcm_rx_trap_config_t_init(&trap_config);
			rv = bcm_petra_rx_trap_get(unit, trap_code_i, &trap_config);
			BCMDNX_IF_ERR_EXIT(rv);
			is_equal = 0;
			if (trap_config.flags & BCM_RX_TRAP_UPDATE_DEST) {
				rv = _bcm_dpp_gport_compare(unit, trap_config.dest_port, remote_gport, &is_equal);
				BCMDNX_IF_ERR_EXIT(rv);
				if (is_equal) {
					rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &wb_mirror_profile, trap_code_i);
					BCMDNX_IF_ERR_EXIT(rv);
					if (is_upmep) {
						/* Check according to mirror profile if the trap code is used for up or down. If wb_mirror_profile == 1 it is up. */
						if ((wb_mirror_profile != 0)) {
                            int trap_code_i_is_for_meta_data  = (bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloced_check(unit, trap_code_i) == BCM_E_EXISTS);
							if (meta_data == trap_code_i_is_for_meta_data)  {
								is_destination_new = FALSE;
								break;
							}
						}
					}
					else {
						/* If it's a MIP 2 trap codes were created, each with the same destination, one for Up and one for Down. Search for the Down MIP trap. */
						if (!is_mep) {
                            int trap_code_i_is_for_meta_data  = (bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloced_check(unit, trap_code_i) == BCM_E_EXISTS);
							if (meta_data == trap_code_i_is_for_meta_data)  {
                                continue;
							}
                            /* Still here: found the Down MIP trap, now we are done.*/
                            is_destination_new = FALSE;
                            break;
						}
						else {
							/* If wb_mirror_profile == 0 it is down. */
							if (wb_mirror_profile == 0) {
								is_destination_new = FALSE;
								break;
							}
						}
					}
				}
			}
		}
	}


	/* If new destination, allocate new user_defined trap */
	if (is_destination_new) {
		/* set oam port profile to 1 */
        rv = _bcm_dpp_gport_to_phy_port(unit, remote_gport, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &pp_port, &core)));

           	soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, 1);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

		if (is_upmep && meta_data) {
			/* These trap codes are already allocated - no need to call trap_create */
			rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, 0/*flags*/, trap_code);
			BCMDNX_IF_ERR_EXIT(rv);
		}
		else {
			rv = bcm_petra_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, (int*)trap_code);
			BCMDNX_IF_ERR_EXIT(rv);
		}

		if (!is_mep) {
			/* In this case allocte a trap code for down and a ftmh trap code for direction up.*/
			rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, 0/*flags*/, (uint32 *)&trap_code_up);
			BCMDNX_IF_ERR_EXIT(rv);
		}
		else {
			trap_code_up = (int)*trap_code;
		}

		bcm_rx_trap_config_t_init(&trap_config);
		trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
		trap_config.dest_port = remote_gport;
		rv = _bcm_dpp_rx_trap_set(unit, *trap_code, &trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
		BCMDNX_IF_ERR_EXIT(rv);

		if (is_upmep || (!is_mep)) {
			/* Allocate upmep trap code and mirror profile */
			uint32 trap_code_up_int = trap_code_up;
			_BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_profile, &trap_code_up_int); 
			soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_get(unit, mirror_profile, &trap_code_dummy, &snoop_strength);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, mirror_profile, trap_code_up, snoop_strength,7);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			wb_mirror_profile = (uint8)mirror_profile;
			rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &wb_mirror_profile, (uint32)trap_code_up);
			BCMDNX_IF_ERR_EXIT(rv);

			if (!is_mep  ) {
				/* MIP - allocate both up and downmep trap codes, mirror profile should be mapped to both. */
				rv = _bcm_dpp_rx_trap_set(unit, trap_code_up, &trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
				BCMDNX_IF_ERR_EXIT(rv);
            	rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &wb_mirror_profile, *trap_code);
				BCMDNX_IF_ERR_EXIT(rv);
			}
		}
    }
	else {
		*trap_code = trap_code_i;
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_oam_trap_and_mirror_profile_set(int unit,
										 SOC_PPD_OAM_TRAP_ID trap_index, 
										 SOC_PPD_OAM_UPMEP_TRAP_ID upmep_trap_index, 
										 bcm_rx_trap_config_t * trap_config, 
										 uint32 trap_code, 
										 uint32 trap_code_upmep, 
										 SOC_PPD_OAM_MIRROR_ID mirror_ndx,
										 uint8 set_upmep){
	int rv;
	uint32 soc_sand_rv=0;
	uint32 snoop_strength = 0;
	uint32 forward_strength = 7;
	uint32 trap_id;
	uint32 mirror_id=0;
	uint8 mirror_id_wb;
	uint8 is_bfd_init;
    uint32 mirror_ind_uint32 = mirror_ndx;

	BCMDNX_INIT_FUNC_DEFS;

	rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS, &trap_id, trap_index);
	BCMDNX_IF_ERR_EXIT(rv);

	_BCM_BFD_ENABLED_GET(is_bfd_init);

	/*creating trap id*/
    switch (trap_index) {
	case SOC_PPD_OAM_TRAP_ID_OAMP:
		rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamEthAccelerated, (int*)&trap_id);
		BCMDNX_IF_ERR_EXIT(rv);
		rv = _bcm_dpp_rx_trap_set(unit, trap_id, trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
		BCMDNX_IF_ERR_EXIT(rv);
		break;
	case SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_MPLS:
		rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamY1731MplsTp, (int*)&trap_id);
		BCMDNX_IF_ERR_EXIT(rv);
		rv = _bcm_dpp_rx_trap_set(unit, trap_id, trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
		BCMDNX_IF_ERR_EXIT(rv);
		break;
	case SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_PWE:
		rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamY1731Pwe, (int*)&trap_id);
		BCMDNX_IF_ERR_EXIT(rv);
		rv = _bcm_dpp_rx_trap_set(unit, trap_id, trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
		BCMDNX_IF_ERR_EXIT(rv);
		break;
	case SOC_PPD_OAM_TRAP_ID_ERR_LEVEL:
		rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamLevel, (int*)&trap_id);
		BCMDNX_IF_ERR_EXIT(rv);
		rv = _bcm_dpp_rx_trap_set(unit, trap_id, trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
		BCMDNX_IF_ERR_EXIT(rv);
		break;
	case SOC_PPD_OAM_TRAP_ID_ERR_PASSIVE:
		rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamPassive, (int*)&trap_id);
		BCMDNX_IF_ERR_EXIT(rv);
		rv = _bcm_dpp_rx_trap_set(unit, trap_id, trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
		BCMDNX_IF_ERR_EXIT(rv);
		break;
    default:
        if ((trap_index != SOC_PPD_OAM_TRAP_ID_CPU) || (!is_bfd_init)) { /* bfd init already created cpu trap */
            trap_id = trap_code;
            rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int *)&trap_code);
            BCMDNX_IF_ERR_EXIT(rv);
            if ((trap_index == SOC_PPD_OAM_TRAP_ID_RECYCLE) || (trap_index == SOC_PPD_OAM_TRAP_ID_SNOOP)) { /* Snooped and recycled packets don't need OAM offset index */
                rv = _bcm_dpp_rx_trap_set(unit, trap_code, trap_config, 0);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                rv = _bcm_dpp_rx_trap_set(unit, trap_code, trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
	rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS, &trap_id, trap_index);
	BCMDNX_IF_ERR_EXIT(rv);

	/* Increasing trap code counter in order to for this trap not to be deleted with another profile */
	rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_id);
    BCMDNX_IF_ERR_EXIT(rv);

	if (set_upmep) {
		/*creating trap id for egress*/
		if (_BCM_OAM_TRAP_CODE_UPMEP_FTMH(trap_code_upmep)) {
			/* These trap codes are already allocated - no need to call trap_create, just to indicate that they are used */
			rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, SHR_RES_ALLOC_WITH_ID, &trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);
			rv = _bcm_dpp_rx_trap_set(unit, trap_code_upmep, trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
			BCMDNX_IF_ERR_EXIT(rv);
			/* Increasing trap code counter in order to for this trap not to be deleted with another profile */
			rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);			
		}
		else if (trap_code_upmep == SOC_PPD_TRAP_CODE_OAM_CPU_UP) {
			/* This is a new trap code we have to create */
			rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int*)&trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);
			rv = _bcm_dpp_rx_trap_set(unit, trap_code_upmep, trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
			BCMDNX_IF_ERR_EXIT(rv);
			/* Increasing trap code counter in order to for this trap not to be deleted with another profile */
			rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);
		}
		rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_UPMEP_TRAP_IDS, &trap_code_upmep, upmep_trap_index);
		BCMDNX_IF_ERR_EXIT(rv);

		/*creating mirror profile*/
		if (trap_code_upmep == SOC_PPD_TRAP_CODE_OAM_CPU_SNOOP) {
			snoop_strength = soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 3); 
		}
		else {
			snoop_strength = 0;
		}


		
		_BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_id, &trap_code_upmep); 
		mirror_id_wb = (uint8)mirror_id;
		rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_MIRROR_IDS, &mirror_id_wb, mirror_ind_uint32);
		BCMDNX_IF_ERR_EXIT(rv);


		rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &mirror_id_wb, trap_code_upmep);
		BCMDNX_IF_ERR_EXIT(rv);

        /*  */
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0) && trap_code_upmep == SOC_PPD_TRAP_CODE_OAM_CPU_SNOOP) {
            trap_code_upmep = SOC_PPD_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH;
            forward_strength = 3; /*Use weaker trap strength to allow forwarding.*/
        } 

		soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, 
																 mirror_id, 
																 trap_code_upmep,
																 snoop_strength,
                                                                 forward_strength);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

	}

	LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: traps were set: downmep trap=%d, upmep trap=%d, mirror profile=%d\n"), trap_id, trap_code_upmep, mirror_id));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_trap_info_wb_add(int unit)
{
	uint32 i;
	int rv;

	BCMDNX_INIT_FUNC_DEFS;

	for (i=0; i<SOC_PPC_OAM_TRAP_ID_COUNT; i++) {
		rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_TRAP_IDS, &_bcm_dpp_oam_trap_info[unit].trap_ids[i], i);
		BCMDNX_IF_ERR_EXIT(rv);
	}
	for (i=0; i<SOC_PPC_OAM_UPMEP_TRAP_ID_COUNT; i++) {
		rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_UPMEP_TRAP_IDS, &_bcm_dpp_oam_trap_info[unit].upmep_trap_ids[i], i);
		BCMDNX_IF_ERR_EXIT(rv);
	}
	for (i=0; i<SOC_PPC_OAM_MIRROR_ID_COUNT; i++) {
		rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_MIRROR_IDS, &_bcm_dpp_oam_trap_info[unit].mirror_ids[i], i);
		BCMDNX_IF_ERR_EXIT(rv);
	}

	BCM_EXIT;
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
             
             if (_g_oam_event_cb[unit][event_i] && (_g_oam_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is already registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("EVENT %d already has a registered callback"), event_i));
             }
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

int bcm_petra_bfd_event_register(
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
int bcm_petra_bfd_event_unregister(
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
    _bcm_oam_event_and_rmep_info event_info =  *(_bcm_oam_event_and_rmep_info*) really_the_event_data;
    int rv, rmeb_db_ndx;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    SOC_PPD_OAM_RMEP_INFO_DATA rmep_info;
    bcm_bfd_event_types_t event_types;
    uint8 found;
    int flags = PTR_TO_INT(really_the_flags);
    BCMDNX_INIT_FUNC_DEFS;



    _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(event_info.rmep_indx, rmeb_db_ndx);
    rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, rmeb_db_ndx,  &rmep_info, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("This message should never appear. Something wrong with the hash table.")));
    }


    if (event_info.is_oam) {
        if (_g_oam_event_cb[unit][event_info.event_number]) {
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, rmep_info.mep_index, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("This message should never appear. Something wrong with the hash table (mep db).")));
            }
            /*finally, the callback*/
            _g_oam_event_cb[unit][event_info.event_number](unit, flags, event_info.event_number, mep_info.ma_index, rmeb_db_ndx, _g_oam_event_ud[unit][event_info.event_number]);
        }
    } else { /* bfd*/
        if (_g_bfd_event_cb[unit][event_info.event_number]) {
            BCM_BFD_EVENT_TYPE_CLEAR_ALL(event_types);
            BCM_BFD_EVENT_TYPE_SET(event_types, event_info.event_number);
            _g_bfd_event_cb[unit][event_info.event_number](unit, flags, event_types, rmep_info.mep_index, _g_bfd_event_ud[unit][event_info.event_number]);
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_fifo_interrupt_handler(int unit) {
	uint32 rmeb_db_ndx; 
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
    

    soc_sand_rv = soc_ppd_oam_event_fifo_read(unit, &rmeb_db_ndx_internal, event_id_bitmap, &valid, &rmep_state);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = shr_htb_create(&event_htbl,
                        OAM_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW,
                        sizeof(_bcm_oam_event_and_rmep_info),
                        "OAM event hash table.");
    BCMDNX_IF_ERR_EXIT(rv); 
    

    while (valid) {
        _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(rmeb_db_ndx_internal, rmeb_db_ndx);
        event_found.rmep_indx = rmeb_db_ndx_internal;
        rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, rmeb_db_ndx,  &rmep_info, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG("Received event from FIFO with unknown RMEP index: rmep_db_ndx=%d"), rmeb_db_ndx));
        }
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, rmep_info.mep_index, &mep_info, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG("Internal error in FIFO event processing: MEP index matching RMEP index wasn't found. rmep_db_ndx=%d, mep_db_ndx=%d"),
                              rmeb_db_ndx, rmep_info.mep_index));
        }

        if ((mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) ||
            (mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
            (mep_info.mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)) {
            event_found.is_oam =1;

            for (event_index = 0; event_index < SOC_PPD_OAM_EVENT_COUNT; event_index++) {
                if (event_id_bitmap[event_index]) {
                    int event_ind=-1;
                    _BCM_OAM_PPD_TO_BCM_EVENT_MEP(event_index, oam_event_type, 1/*is_oam*/);
                    if (event_index == SOC_PPD_OAM_EVENT_RMEP_STATE_CHANGE) {
                        /* Check port status */
                        if (_BCM_OAM_PPD_EVENT_PORT_BLOCKED(rmep_state)) {
                            event_ind = bcmOAMEventEndpointPortDown;
                        }
                        else if (_BCM_OAM_PPD_EVENT_PORT_UP(rmep_state)) {
                            event_ind = bcmOAMEventEndpointPortUp;
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
                    event_found.event_number = (uint8) bfd_event_type;
                    INSERT_EVENT_INTO_HTBL(event_htbl,event_found );
                }
            }
        }

        soc_sand_rv = soc_ppd_oam_event_fifo_read(unit, &rmeb_db_ndx_internal, event_id_bitmap, &valid, &rmep_state);
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
	int rv;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_ARAD_SUPPORT
	if ((SOC_IS_ARAD(unit))) {
        uint32 flags;
		event_control.event_id = ARAD_INT_OAMP_PENDINGEVENT;

        rv = soc_interrupt_flags_get(unit,event_control.event_id ,&flags);
        BCMDNX_IF_ERR_EXIT(rv);
        
        flags |= SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK;

        rv = soc_interrupt_flags_set(unit,event_control.event_id, flags);
        BCMDNX_IF_ERR_EXIT(rv);
	} else
#endif /*BCM_ARAD_SUPPORT*/
	{
		BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.\n")));
	}
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

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * callback used for bcm_petra_oam_group_traverse()
 * 
*/
STATIC int
_bcm_petra_oam_group_traverse_cb(int unit, shr_htb_key_t key, shr_htb_data_t user_data){
    int rv;
    bcm_oam_group_info_t group_info_for_cb;
    uint32 group_info_id = * (uint32*)key;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_oam_group_info_t_init(&group_info_for_cb); 

    /* user_data here is the name of the oam group*/
   rv = _bcm_dpp_oam_ma_name_struct_to_group_name_convert(unit, (bcm_oam_ma_name_t*) user_data, (&group_info_for_cb)->name ); 
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
	int fec_id, is_local;
	uint32 lif;
	uint8 found;
	uint8 is_passive;
	int rv; 

    BCMDNX_INIT_FUNC_DEFS;
	profile_info = (bcm_oam_endpoint_traverse_cb_profile_info_t*)user_data;

	rv = _bcm_dpp_gport_to_lif(unit, endpoint_info->gport, (int*)&lif, NULL, &fec_id, &is_local);
	BCMDNX_IF_ERR_EXIT(rv);

	if (lif == profile_info->lif) {
		/* Save first endpoint id on this lif */
        if (profile_info->first_endpoint_id == BCM_OAM_ENDPOINT_INVALID) {
			profile_info->first_endpoint_id = endpoint_info->id;
        }
		
        if (profile_info->profile_data) {
            is_passive = ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) == 0) ^ profile_info->is_ingress;
            
            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, endpoint_info->id, is_passive, &profile_data_old);
            BCMDNX_IF_ERR_EXIT(rv); 

            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_exchange(unit, 0/*flags*/,  endpoint_info->id, is_passive, profile_info->profile_data,
                                     &old_profile, &is_last, &profile, &is_allocated);
            BCMDNX_IF_ERR_EXIT(rv);

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
                rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint_info->id);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info->id, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (is_allocated) {
                rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &(profile_info->profile_data->mep_profile_data));
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &(profile_info->profile_data->mip_profile_data));
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (is_last) {
                rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &(profile_data_old.mep_profile_data), 1/*is_mep*/);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &(profile_data_old.mip_profile_data), 0/*is_mep*/);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            
        }
	}
    BCM_EXIT;   
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

/* OAMP error trap allocation and setting functions */
int _bcm_dpp_oam_error_trap_allocate(int unit, bcm_rx_trap_t trap_type, uint32 oamp_error_trap_id)
{
    int rv; 
    uint32 ppd_trap_type;
	uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	if (!SOC_IS_ARADPLUS(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
	}

    _BCM_OAM_RX_TRAP_TYPE_TO_PPD_TRAP_TYPE(trap_type, ppd_trap_type);

    /* Mapping type to id */
	rv = SOC_DPP_WB_ENGINE_SET_ARR(
	  unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_ERROR_TYPE_TO_TRAP_ID_MAP, &oamp_error_trap_id, ppd_trap_type);
	BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;   
exit:
    BCMDNX_FUNC_RETURN;

}

int _bcm_dpp_oam_error_trap_destroy(int unit, uint32 oamp_error_trap_id)
{
    int rv; 
	uint32 soc_sand_rv=0;
    uint32 trap_type;
    uint32 trap_id;
	uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	if (!SOC_IS_ARADPLUS(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
	}

    /*Here should go over all the types mapped to this id and delete in soc*/
    for (trap_type=0; trap_type<SOC_PPD_OAM_OAMP_TRAP_TYPE_COUNT; trap_type++) {
        rv = SOC_DPP_WB_ENGINE_GET_ARR(
          unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_ERROR_TYPE_TO_TRAP_ID_MAP, &trap_id, trap_type);
        BCMDNX_IF_ERR_EXIT(rv);
        if (trap_id == oamp_error_trap_id) {
            /* Reset destination */
            soc_sand_rv = soc_ppd_oam_oamp_error_trap_id_and_destination_set(unit, trap_type, 0, 0);  
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }


    BCM_EXIT;   
exit:
    BCMDNX_FUNC_RETURN;

}

int _bcm_dpp_oam_error_trap_set(int unit, uint32 oamp_error_trap_id, bcm_gport_t dest_port)
{
    int rv; 
	uint32 soc_sand_rv=0;
    uint32 trap_type;
    uint32 trap_id;
	uint8 oam_is_init;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	if (!SOC_IS_ARADPLUS(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
	}

    rv = _bcm_dpp_gport_to_phy_port(unit, dest_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    /*Here should go over all types mapped to this is and set destination in soc*/
    for (trap_type=0; trap_type<SOC_PPD_OAM_OAMP_TRAP_TYPE_COUNT; trap_type++) {
        rv = SOC_DPP_WB_ENGINE_GET_ARR(
          unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_ERROR_TYPE_TO_TRAP_ID_MAP, &trap_id, trap_type);
        BCMDNX_IF_ERR_EXIT(rv);
        if (trap_id == oamp_error_trap_id) {
            /* Set destination */
            soc_sand_rv = soc_ppd_oam_oamp_error_trap_id_and_destination_set(unit, trap_type, trap_id, gport_info.sys_port);  
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

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

    rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine,  &trap_code_up); /* this trap hasn't been allocated above with the others.*/
    BCMDNX_IF_ERR_EXIT(rv); 

    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = 0;
    trap_config_snoop.trap_strength = 0;
    trap_config_snoop.snoop_cmnd = snoop_cmnd; 

    rv = _bcm_dpp_rx_trap_set(unit, trap_code_up, &trap_config_snoop, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
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
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not supported in Arad+.")));
            }
            *default_id = ARAD_PP_OAM_DEFAULT_EP_EGRESS_1;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS2:
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not supported in Arad+.")));
            }
            *default_id = ARAD_PP_OAM_DEFAULT_EP_EGRESS_2;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS3:
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not supported in Arad+.")));
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

/***************************************************************/
/***************************************************************/

/*
 * Begin BCM Functions
 *
 */

/* Initialize the OAM subsystem */
int bcm_petra_oam_init(int unit) {
	uint32 soc_sand_rv=0;
	int rv ;
	bcm_rx_trap_config_t trap_config_oamp, trap_config_cpu, trap_config_snoop, trap_config_recycle;
	bcm_rx_snoop_config_t snoop_config_cpu;
	bcm_port_config_t port_config;
	bcm_gport_t oamp_port, cpu_port, recycle_port, ethernet_port;
	bcm_gport_t recycle_port_iter;
	int count_erp;
	int snoop_cmnd;
	uint8 mirror_id_wb;
	uint32 cpu_local_port, oamp_local_port;
	uint8 oam_is_init, is_bfd_init;
	int trap_code_with_meta;
    uint32 trap_code_default_egress;
    int mirror_profile_default_egress;
    uint32 flags =0;
    _bcm_dpp_gport_info_t gport_info;
    int core = SOC_CORE_INVALID;
	uint32		pp_port;
#ifdef BCM_ARAD_SUPPORT
    SOC_TMC_PORT_HEADER_TYPE header_type_incoming, header_type_outgoing;
#endif /*BCM_ARAD_SUPPORT*/

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (oam_is_init) {
#ifdef BCM_WARM_BOOT_SUPPORT
		if (!SOC_WARM_BOOT(unit))
#endif /*BCM_WARM_BOOT_SUPPORT*/
		{
			BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: Oam already initialized.\n")));
		}
	}
#ifdef BCM_WARM_BOOT_SUPPORT
	else {
		if (SOC_WARM_BOOT(unit)) {
			BCM_EXIT;
		}
	}
#endif /*BCM_WARM_BOOT_SUPPORT*/

	_BCM_BFD_ENABLED_GET(is_bfd_init);

#ifdef BCM_WARM_BOOT_SUPPORT
	if (SOC_WARM_BOOT(unit) || (!is_bfd_init)) {
#else 
	if (!is_bfd_init) {
#endif /*BCM_WARM_BOOT_SUPPORT*/
		rv = _bcm_dpp_oam_dbs_init(unit, TRUE/*is_oam*/);
		BCMDNX_IF_ERR_EXIT(rv);
		soc_sand_rv = soc_dpp_wb_engine_init_buffer(unit,SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM);  
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
	}
 
#ifdef BCM_WARM_BOOT_SUPPORT
	if (SOC_WARM_BOOT(unit)) {
		rv = _bcm_dpp_oam_get_hash_tables_from_wb_arrays(unit);
		BCMDNX_IF_ERR_EXIT(rv);
        if (SOC_IS_ARADPLUS(unit)) {
			soc_sand_rv = soc_ppd_oam_dma_clear(unit);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
			soc_sand_rv = soc_ppd_oam_dma_reset(unit);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
        }
    }
	else 
#endif /*BCM_WARM_BOOT_SUPPORT*/
	{

		rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 1, &oamp_port, &count_erp); 
		BCMDNX_IF_ERR_EXIT(rv);
		if (count_erp<1) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: Oamp port disabled, can't init OAM.\n")));
		}

		rv = bcm_petra_port_config_get(unit, &port_config);
		BCMDNX_IF_ERR_EXIT(rv);

		if (!is_bfd_init) {
			SOC_PPD_OAM_INIT_TRAP_INFO_clear(&_bcm_dpp_oam_trap_info[unit]);
		}

		/* Allocate FTMH traps so that they won't be taken by somebody else */
		for (trap_code_with_meta = SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST; 
			  trap_code_with_meta<(SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST+SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM); 
			  trap_code_with_meta++) {

			rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code_with_meta);
			BCMDNX_IF_ERR_EXIT(rv);
		}

		/* 
		 * Configuration of trapping to CPU
		 */

		/*get CPU port*/
		BCM_PBMP_ITER(port_config.cpu, cpu_port) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, cpu_port, &pp_port, &core)));

		   break; /*get first CPU port*/
		}

        /* set oam port profile to 1 */
        rv = _bcm_dpp_gport_to_phy_port(unit, cpu_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);
        cpu_local_port = gport_info.local_port;
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, cpu_local_port, &pp_port, &core)));
		soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, 1);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = _bcm_dpp_gport_to_phy_port(unit, oamp_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);
        oamp_local_port = gport_info.local_port;
           BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, oamp_local_port, &pp_port, &core)));

		soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, 1);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

		bcm_rx_trap_config_t_init(&trap_config_cpu);
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
		if (recycle_port == -1) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: OAM recycling port is not configured.\n")));
		}

		BCM_PBMP_ITER(port_config.rcy, recycle_port_iter) {
		   if (recycle_port_iter == recycle_port) {
				break;
		   }
		}
		if (recycle_port_iter != recycle_port) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Network Interface of OAM recycling port should be configured as RCY (ucode_port_<Local-Port-Id>=RCY).\n")));
		}

	#ifdef BCM_ARAD_SUPPORT
		if ((SOC_IS_ARAD(unit))) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, recycle_port, &pp_port, &core)));

			soc_sand_rv = arad_port_header_type_get(unit, core, pp_port, &header_type_incoming, &header_type_outgoing); 
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

			if ((header_type_incoming != SOC_TMC_PORT_HEADER_TYPE_TM) || (header_type_outgoing != SOC_TMC_PORT_HEADER_TYPE_ETH)) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Recycle port network headers incorrect. Should be header_type_in=TM, header_type_out=ETH.\n")));
			}
		} else
	#endif /*BCM_ARAD_SUPPORT*/
		{
			BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.\n")));
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
		 
		_bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_RECYCLE] = _bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_CPU];
		_bcm_dpp_oam_trap_info[unit].upmep_trap_ids[SOC_PPD_OAM_UPMEP_TRAP_ID_RECYCLE] = _bcm_dpp_oam_trap_info[unit].upmep_trap_ids[SOC_PPD_OAM_MIRROR_ID_CPU];

		/* 
		 * Configuration of snooping to CPU (used for MIP LT)
		 */


		/*creating snoop trap id*/
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) {
            /* In this case create a snoop_cmnd for down meps and up meps seperatley.*/
            rv = _bcm_oam_init_snoop_commands_for_advanced_egress_snooping(unit, cpu_port);           
            BCMDNX_IF_ERR_EXIT(rv);

            snoop_cmnd =_BCM_OAM_MIP_SNOOP_CMND_DOWN;
            flags = BCM_RX_SNOOP_WITH_ID;
        }  else {
            flags =0;
            snoop_cmnd=0;
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
                                                  SOC_PPD_TRAP_CODE_OAM_CPU_SNOOP,
                                                  SOC_PPD_OAM_MIRROR_ID_SNOOP,
                                                  1 /*set upmep - already set for advanced snooping.*/);
            BCMDNX_IF_ERR_EXIT(rv);
        


            /* Supporting mip LTM, LBM mirroring */ 
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_INIT_TRAP_INFO_MIRROR_IDS, &mirror_id_wb, SOC_PPD_OAM_MIRROR_ID_CPU);
        BCMDNX_IF_ERR_EXIT(rv); 

		rv = SOC_DPP_WB_ENGINE_SET_ARR(
		  unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &mirror_id_wb, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_CPU]);
		BCMDNX_IF_ERR_EXIT(rv);

		rv = SOC_DPP_WB_ENGINE_SET_ARR(
		  unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &mirror_id_wb, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_RECYCLE]);
		BCMDNX_IF_ERR_EXIT(rv);


		/* 
		 * Configuration of trapping to OAMP (used for accelerated packets)
		 */
		bcm_rx_trap_config_t_init(&trap_config_oamp);
		trap_config_oamp.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
		trap_config_oamp.dest_port = oamp_port;
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
		trap_config_oamp.dest_port = oamp_port;
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
		trap_config_oamp.dest_port = oamp_port;
		rv = _bcm_oam_trap_and_mirror_profile_set(unit, 
												  SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_PWE, 
												  0, 
												  &trap_config_oamp, 
												  SOC_PPD_TRAP_CODE_TRAP_Y1731_O_PWE, 
												  0, 
												  0,
												  0 /*set_upmep*/);
		BCMDNX_IF_ERR_EXIT(rv);

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_ccm_two_fhei", 0) == 1) {
			int is_alloced;
            trap_code_default_egress = SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST + SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM - 1;
            rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, SHR_RES_ALLOC_WITH_ID, &trap_code_default_egress);
            BCMDNX_IF_ERR_EXIT(rv);

            mirror_profile_default_egress = SOC_DPP_CONFIG(unit)->pp.oam_ccm_2_fhei_eg_default_mirror_profile;
			rv= _bcm_dpp_am_template_mirror_action_profile_alloc(unit,SHR_RES_ALLOC_WITH_ID,&trap_code_default_egress,&is_alloced, &mirror_profile_default_egress );
            BCMDNX_IF_ERR_EXIT(rv);
            
            soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, 
                                                                     mirror_profile_default_egress, 
                                                                     trap_code_default_egress,
                                                                     3,
                                                                     7);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        }

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
				rv = bcm_petra_mirror_port_dest_add(unit, ethernet_port, BCM_MIRROR_PORT_EGRESS_ACL, 0);
				BCMDNX_IF_ERR_EXIT(rv);
			}
		}

        rv = _bcm_dpp_oam_trap_info_wb_add(unit);
		BCMDNX_IF_ERR_EXIT(rv);


		soc_sand_rv = soc_ppd_oam_init(unit, &_bcm_dpp_oam_trap_info[unit], NULL, FALSE/*not bfd*/);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


		_BCM_OAM_ENABLED_SET(TRUE);

		rv = _bcm_dpp_oamp_interrupts_init(unit, TRUE);
		BCMDNX_IF_ERR_EXIT(rv);
        
        if (SOC_IS_ARADPLUS(unit)) {
            soc_sand_rv = soc_ppd_oam_register_dma_event_handler_callback(unit, _bcm_dpp_oam_fifo_interrupt_handler);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        }
    }

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
	bcm_oam_ma_name_t ma_name_struct;
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
	bcm_oam_ma_name_t ma_name_struct;
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


/**
 * perform endpoint create validity check for default endpoint.
 * 
 * @author sinai (16/09/2014)
 * 
 * @param unit 
 * @param endpoint_info 
 * @param default_id 
 * 
 * @return STATIC int 
 */
STATIC int _bcm_oam_default_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info, ARAD_PP_OAM_DEFAULT_EP_ID * default_id) {
    int rv = 0;
    BCMDNX_INIT_FUNC_DEFS;
    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint is defined as default (id == %d) but BCM_OAM_ENDPOINT_WITH_ID flag not set.\n"),
                             (int)(endpoint_info->id)));
    }
    if (SOC_IS_ARADPLUS(unit)) {
        if (endpoint_info->flags & ~(BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_REPLACE | BCM_OAM_ENDPOINT_UP_FACING)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: endpoint is defined as default (id == %d) but forbidden flags were used.\n"),
                                 (int)(endpoint_info->id)));
        }
        if (endpoint_info->id == BCM_OAM_ENDPOINT_DEFAULT_EGRESS0) {
            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) == 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: default egress endpoint must face up\n")));
            }
        } else if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: default ingress endpoint must face down\n")));
        }
    }
    if ((SOC_IS_ARAD_B1_AND_BELOW(unit))
        && (endpoint_info->flags != BCM_OAM_ENDPOINT_WITH_ID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint is defined as default (id == %d) but forbidden flags were used.\n"),
                             (int)(endpoint_info->id)));
    }

    /* check field limitations */
    if (endpoint_info->ccm_period || endpoint_info->int_pri || endpoint_info->vlan || endpoint_info->inner_pkt_pri
        || endpoint_info->inner_tpid || endpoint_info->inner_vlan || endpoint_info->outer_tpid ||   !BCM_MAC_IS_ZERO(endpoint_info->src_mac_address)
        || !BCM_MAC_IS_ZERO(endpoint_info->dst_mac_address) ||  endpoint_info->name || endpoint_info->type || endpoint_info->group
        || endpoint_info->opcode_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint is defined as default (id == %d) but other fields are set.\n"),(int)(endpoint_info->id)));
    }
    /* Extra limitations for Arad */
    else if (SOC_IS_ARAD_B1_AND_BELOW(unit) && (endpoint_info->level)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: endpoint is defined as default (id == -1) but other fields are set.\n")));
    }
    rv = _bcm_oam_default_id_from_ep_id(unit, endpoint_info->id, default_id);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int _bcm_oam_generic_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info) {
    int rv = 0;
	int correct_flags=0, correct_flags2=0;
	uint8 is_accelerated = 0;
    uint8 is_upmep;

    BCMDNX_INIT_FUNC_DEFS;

    correct_flags =  BCM_OAM_ENDPOINT_REPLACE | BCM_OAM_ENDPOINT_REMOTE;
    is_accelerated = ((endpoint_info->opcode_flags & BCM_OAM_OPCODE_CCM_IN_HW) != 0);
    is_upmep = ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) != 0);



    if (endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE) {
        correct_flags |= BCM_OAM_ENDPOINT_WITH_ID;
    }


    if (((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) || is_accelerated)
        && SOC_IS_ARADPLUS(unit) && endpoint_info->type == bcmOAMEndpointTypeEthernet ) {
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_UPDATE) {
            if ((endpoint_info->port_state > (BCM_OAM_PORT_TLV_UP
                                             )) || (endpoint_info->port_state < BCM_OAM_PORT_TLV_BLOCKED)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: flag status BCM_OAM_ENDPOINT_PORT_STATE_UPDATE does not match port_state field.")));
            }
        }
        else if (endpoint_info->port_state) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: flag status BCM_OAM_ENDPOINT_PORT_STATE_UPDATE does not match port_state field.")));
        }
        if ((endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE) ?
            (endpoint_info->interface_state < BCM_OAM_INTERFACE_TLV_UP || endpoint_info->interface_state > BCM_OAM_INTERFACE_TLV_LLDOWN) :
            endpoint_info->interface_state) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: flag status BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE does not match interface_state field.")));
        }
        if ((endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE) && (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_UPDATE)
            && !(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: only one of the flags BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE, BCM_OAM_ENDPOINT_PORT_STATE_UPDATE may be set")));
        }
        correct_flags |=  BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE | BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
    } else {
        if (endpoint_info->port_state || endpoint_info->interface_state) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: port_state, interface_state only available in Jericho and for accelerate/remote endpoints.")));

        }
    }

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
        /*remote endpoint. additional flags allowed.*/
        correct_flags |=   BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE | BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE | BCM_OAM_ENDPOINT_WITH_ID ;
        if (SOC_IS_ARADPLUS(unit)) {
            correct_flags2 |=  (BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE | BCM_OAM_ENDPOINT2_RX_REMOTE_EVENT_DISABLE
                                | BCM_OAM_ENDPOINT2_RX_REMOTE_DEFECT_AUTO_UPDATE | BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI |
                                   BCM_OAM_ENDPOINT2_RDI_CLEAR_ON_LOC_CLEAR | BCM_OAM_ENDPOINT2_RDI_ON_LOC);
            if  (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE &&
                 ((endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RX_REMOTE_EVENT_DISABLE) || (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RX_REMOTE_DEFECT_AUTO_UPDATE))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: when BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE flag is set BCM_OAM_ENDPOINT2_RX_REMOTE_EVENT_DISABLE,"
                                           " BCM_OAM_ENDPOINT2_RX_REMOTE_DEFECT_AUTO_UPDATE flags may not be set")));
            }

            if  (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE &&
                 ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE) || (endpoint_info->flags2 & BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: when BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE flag is set BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE,"
                                           " BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE flags may not be set")));
            }

            if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE) && (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE) ) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                               (_BSL_BCM_MSG("Error: when BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE is set, BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE flag may not be set.\n")));

            }

            if ((endpoint_info->flags & BCM_OAM_ENDPOINT2_RX_REMOTE_EVENT_DISABLE) && (endpoint_info->flags & BCM_OAM_ENDPOINT2_RX_REMOTE_DEFECT_AUTO_UPDATE) ) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                               (_BSL_BCM_MSG("Error: when BCM_OAM_ENDPOINT2_RX_REMOTE_EVENT_DISABLE is set, BCM_OAM_ENDPOINT2_RX_REMOTE_DEFECT_AUTO_UPDATE flag may not be set.\n")));

            }

        }

    }
    else {
        /*local endpoint*/	
        correct_flags |= BCM_OAM_ENDPOINT_INTERMEDIATE | BCM_OAM_ENDPOINT_UP_FACING;

        if (is_upmep && endpoint_info->type != bcmOAMEndpointTypeEthernet) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                       (_BSL_BCM_MSG("Error: Up MEP supported only for type Ethernet OAM.\n")));
        }

        if (is_accelerated) {
            /*Accelerated endpoint. TX fields must be correct as well */
            if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
                if ((endpoint_info->inner_vlan == 0) && (endpoint_info->inner_pkt_pri | endpoint_info->inner_tpid)) {
                      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                           (_BSL_BCM_MSG("Error: inner_vlan is not set but inner_pkt_pri or inner_tpid are set\n")));
                }
            } else {
                /* presumably MPLS or PWE. */
                if (endpoint_info->intf_id==0 || endpoint_info->egress_label.label==0 || endpoint_info->egress_label.ttl==0 ) {
                      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                           (_BSL_BCM_MSG("Error: MPLS label fields must be filled.\n")));
                }
            }


            correct_flags |= BCM_OAM_ENDPOINT_WITH_ID ;  
            correct_flags |= BCM_OAM_ENDPOINT_RDI_TX  * SOC_IS_ARAD_B1_AND_BELOW(unit) ;  
            correct_flags2 |= ( BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE |BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE) * SOC_IS_ARADPLUS(unit);
            /* handling the gport, tx-gport.*/
            if (is_upmep) {
                /*For up MEPs the tx-gport is not used. Should be invalid.*/
                if (endpoint_info->tx_gport != BCM_GPORT_INVALID && !_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("For accelerated up-MEPs tx_gport must be invalid")));
                }
                if (endpoint_info->gport == BCM_GPORT_INVALID && !_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("For accelerated up-MEPs gport may not be invalid.")));
                }
                if (SOC_IS_ARAD_B1_AND_BELOW(unit) && (endpoint_info->gport!=BCM_GPORT_INVALID )) {
                    /* In Arad the MEP_DB.local_port goes on the PTCH and acts as the LSB of the src mac address. Verify that these values are in fact identical */
                    /* For server the value on the tx_gport represents the SSP on the inner PTCH, which should be SSP in remote device*/
                    _bcm_dpp_gport_info_t gport_info;
                    rv = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (gport_info.local_port != endpoint_info->src_mac_address[5]) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("Error: local port does not match LSB of src MAC address. Local port is: 0x%x, LSB of MAC address is 0x%x\n"), gport_info.local_port,  endpoint_info->src_mac_address[5]));
                    }

                }

            } else if (endpoint_info->tx_gport != BCM_GPORT_INVALID  && SOC_IS_ARAD_A0(unit) && endpoint_info->type == bcmOAMEndpointTypeEthernet) {
                       /* In Arad A0 the system port goes on the MEP_DB.local_port, from which the LSB the the src mac address is taken.*/
                       _bcm_dpp_gport_info_t gport_info;
                       rv = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->tx_gport, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
                       BCMDNX_IF_ERR_EXIT(rv);
                       if (( 0xff & gport_info.sys_port) != endpoint_info->src_mac_address[5]) {
                           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                               (_BSL_BCM_MSG("Error: system port does not match LSB of src MAC address. System port is: 0x%x, LSB of MAC address is 0x%x\n"), 
                                                0xff & gport_info.sys_port,  endpoint_info->src_mac_address[5]));
                       }
            }

            if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: OAMP server - client side. Endpoint may not be accelerated."))); 
            }

            if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {

                if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server, Only supported for Arad+ and above.")));
                }
                if (endpoint_info->gport != BCM_GPORT_INVALID) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server - gport must be set to BCM_GPORT_INVALID.")));
                }
                if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) == 0 && !BCM_GPORT_IS_TRAP(endpoint_info->remote_gport)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server, down MEP - remote_gport must be set to the trap configured on client device (trap destination should be server OAMP).")));
                }
                if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) && !BCM_GPORT_IS_MODPORT(endpoint_info->remote_gport) && 
                    !BCM_GPORT_IS_SYSTEM_PORT(endpoint_info->remote_gport) ) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server, up MEP - remote_gport must be a MODPORT (remote device's recycle port)."))); 
                }
            }
        }
        else {
            /*Non accelerated endpoint. Only RX fields should be filled.*/
            if (  endpoint_info->ccm_period || endpoint_info->int_pri || endpoint_info->vlan || endpoint_info->inner_pkt_pri || endpoint_info->pkt_pri || endpoint_info->name ||
                 endpoint_info->intf_id || endpoint_info->egress_label.label || endpoint_info->egress_label.ttl || endpoint_info->egress_label.exp ||
                                    endpoint_info->inner_tpid || endpoint_info->inner_vlan || endpoint_info->outer_tpid || !BCM_MAC_IS_ZERO(endpoint_info->src_mac_address)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                 (_BSL_BCM_MSG("Error: Non acceleration in HW marked but TX fields non zero.\n")));

            }
            if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: OAMP server. Endpoint must be accelerated.")));
            }
            if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) {
                if (endpoint_info->tx_gport != BCM_GPORT_INVALID) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server -client side. tx_port must be set to BCM_GPORT_INVALID.")));
                }
                if ((endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) ==0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: OAMP server -client side must be set with the _WITH_ID flag, ID being that of the server.")));
                }
                correct_flags |= BCM_OAM_ENDPOINT_WITH_ID;
            }

        }


        if (endpoint_info->opcode_flags & ~BCM_OAM_OPCODE_CCM_IN_HW) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
            (_BSL_BCM_MSG("Error: invalid opcode flag is set.\n")));
        }
        if ((_BCM_OAM_IS_SERVER_SERVER(endpoint_info) || _BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) && SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: OAMP server supported only for Arad+ and above.")));
        }
    }

    if (endpoint_info->flags & ~correct_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: invalid flag combination is set.\n")));
    }
    if (endpoint_info->flags2 & ~correct_flags2) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: invalid flag2 combination is set.\n")));
    }

    if (SOC_IS_ARAD_A0(unit) && endpoint_info->timestamp_format == bcmOAMTimestampFormatIEEE1588v1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: 1588 timestamp format supported only for Arad B0 and above.")));
    }

    if (endpoint_info->int_pri > 0xFF) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
             (_BSL_BCM_MSG("Error: Supporting range for int_pri is 0-255.\n")));
    }

    if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info) && !is_upmep) {
        if (!BCM_GPORT_IS_SYSTEM_PORT(endpoint_info->tx_gport) &&  !BCM_GPORT_IS_TRUNK(endpoint_info->tx_gport)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: server, down MEP: tx-gport must be either trunk or a system port.\n")));
        }
    } else if (endpoint_info->tx_gport != BCM_GPORT_INVALID && !BCM_GPORT_IS_SYSTEM_PORT(endpoint_info->tx_gport)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: tx-gport must be either invalid or a system port.\n")));
    }


exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int _bcm_oam_ethernet_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info) {
    BCMDNX_INIT_FUNC_DEFS;

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)==0 ) {
        if (BCM_MAC_IS_ZERO(endpoint_info->dst_mac_address) && endpoint_info->gport != BCM_GPORT_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: destination mac address may not be zero.\n")));
        }
        
        /* Arad+ Advanced node does not support level 0. Arad+ new classifier mode and Arad simple does support level 0.*/
        if ((endpoint_info->level > 7) || ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) &&
                                            !SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) &&
                                            (endpoint_info->level <= 0)) ||
                                           (endpoint_info->level < 0))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: invalid level (must be in range %d to 7.\n"), 
								 (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && !SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit))));
        }
    }
    
    if ((endpoint_info->outer_tpid != 0 && endpoint_info->vlan == 0) || (endpoint_info->inner_tpid != 0 && endpoint_info->inner_vlan == 0) ||
        (endpoint_info->outer_tpid == 0 && endpoint_info->vlan != 0) || (endpoint_info->inner_tpid == 0 && endpoint_info->inner_vlan != 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: VLAN fields inconsistent.\n")));
    } 

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int _bcm_oam_mpls_pwe_endpoint_validity_checks(int unit, bcm_oam_endpoint_info_t *endpoint_info) {
    BCMDNX_INIT_FUNC_DEFS;

    
    if ((!BCM_MAC_IS_ZERO(endpoint_info->src_mac_address)) || (!BCM_MAC_IS_ZERO(endpoint_info->dst_mac_address))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: src, dst MAC adresses must be zero in case of BHH type.\n")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && endpoint_info->level !=7 && (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) ==0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Only level 7 supported for Y.1731 endpoints over MPLS/PWE.\n")));
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Validity checks used by default and non default endpoints 
 * alike. 
 * 
 * @author sinai (14/10/2014)
 * 
 * @param unit 
 * @param endpoint_info 
 * 
 * @return STATIC int 
 */
STATIC int _bcm_oam_endpoint_validity_checks_all(int unit, bcm_oam_endpoint_info_t *endpoint_info) {
    BCMDNX_INIT_FUNC_DEFS;

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) ==0) {
        /* Local endpoint*/

        /* For ARAD and ARAD+ support only 0-16K counters ids (do not support pcp)*/
            /* An extra 3 bits in Jericho are used representing the OAM-PCP*/
            if ( (endpoint_info->lm_counter_base_id >= 0x4000  << (( (endpoint_info->lm_flags & BCM_OAM_LM_PCP) !=0)*3 )) || (endpoint_info->lm_counter_base_id < 0) ) {
                /*value must be  must be in the range 0 - (16K-1)*/
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                     (_BSL_BCM_MSG("Error: invalid lm_counter_base_id (must be in range 0 to %dK-1).\n"), 16 + ((endpoint_info->lm_flags & BCM_OAM_LM_PCP) !=0)));

            }

            if (endpoint_info->lm_flags & BCM_OAM_LM_PCP) {
                if (SOC_IS_JERICHO(unit)) {
                    if (endpoint_info->lm_counter_base_id & 0x7) {
                        /* Three LSBs in this case must be off.*/
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("When using LM PCP 3 LSBs of lm_counter_base_id must be off."))); 
                    }
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
                                        (_BSL_BCM_MSG("LM PCP unavalable."))); 
                }
            }
    }

exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_oam_endpoint_create(int unit, bcm_oam_endpoint_info_t *endpoint_info) {

	uint32 soc_sand_rv = SOC_SAND_OK;
	int rv = 0;
	int flags = 0;
	SOC_PPD_OAM_ICC_MAP_DATA data;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
	SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
	SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
	SOC_PPD_OAM_LIF_PROFILE_DATA * profile_data_pointer;
	SOC_PPD_OAM_LIF_PROFILE_DATA profile_data;
	SOC_PPD_OAM_LIF_PROFILE_DATA profile_data_passive;
	SOC_PPD_OAM_LIF_PROFILE_DATA profile_data_acc;
	SOC_PPD_OAM_RMEP_INFO_DATA rmep_info;
	bcm_oam_ma_name_t ma_name_struct;
	uint8 allocate_icc_ndx = 0;
	uint8 found, found_profile;
	uint8 new_id_alloced = FALSE;
	int is_last;
	int icc_ndx=0;
	int is_allocated;
	int profile;
    uint32 profile_dummy;
    uint8 is_default = 0;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
    uint32 ccm_period_ms, ccm_period_micro_s;
	uint8 update_counter;
	uint32 rmep_id_internal;
	bcm_oam_endpoint_info_t existing_endpoint_info;
	int endpoint_ref_counter;
	uint8 oam_is_init;
    uint8 port_is_equal;
	uint8 is_mp_type_flexible;
	uint8 updated_mp_type = 0;
	uint8 is_accelerated = 0;
    uint8 is_upmep;
    uint8 is_passive;
	uint8 is_mip;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	is_accelerated = ((endpoint_info->opcode_flags & BCM_OAM_OPCODE_CCM_IN_HW) != 0);
    is_upmep = ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) != 0);

    is_default = _BCM_OAM_IS_MEP_ID_DEFAULT(unit, endpoint_info->id);

    /* preliminary validity check of endpoint_info. Relavent only when type==Eth and id != -1*/
	/*Note that if endpoint_info is an invalid pointer then the result will presumably be a segmentation fault.*/

    rv = _bcm_oam_endpoint_validity_checks_all(unit, endpoint_info);
    BCMDNX_IF_ERR_EXIT(rv);
	
    if (is_default) {
        /* Check flags limitations */
        rv = _bcm_oam_default_endpoint_validity_checks(unit, endpoint_info, &default_id);
        BCMDNX_IF_ERR_EXIT(rv);

    }
    else { /* Not default */

        rv = _bcm_oam_generic_endpoint_validity_checks(unit, endpoint_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
            rv = _bcm_oam_ethernet_endpoint_validity_checks(unit, endpoint_info);
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (endpoint_info->type == bcmOAMEndpointTypeBHHMPLS || endpoint_info->type == bcmOAMEndpointTypeBHHPwe) {
            rv = _bcm_oam_mpls_pwe_endpoint_validity_checks(unit, endpoint_info);
            BCMDNX_IF_ERR_EXIT(rv); 
        } else {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
			 (_BSL_BCM_MSG("Error: Supporting only Ethernet, BHH-MPLS%stype.\n"), SOC_IS_JERICHO(unit)? ", BHH-PWE " : " " ));
        }

		/* Checking that group exists */
		if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
			rv = _bcm_dpp_oam_ma_db_get(unit, (uint32)endpoint_info->group, &ma_name_struct, &found);
			BCMDNX_IF_ERR_EXIT(rv); 
			if (!found) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
					 (_BSL_BCM_MSG("Error: group %d does not exist.\n"), endpoint_info->group));
			}
		}
    }
	/*preliminary validity check complete*/

	/* Endpoint replace */
	if (endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE) {
		if(!(endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID)) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
			 (_BSL_BCM_MSG("Error: BCM_OAM_ENDPOINT_REPLACE flag can be used only with BCM_OAM_ENDPOINT_WITH_ID specification.\n")));
		}

		if (endpoint_info->id == -1 && SOC_IS_ARAD_B1_AND_BELOW(unit)) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
			 (_BSL_BCM_MSG("Error: Can not replace default mep.\n")));
		}

		bcm_oam_endpoint_info_t_init(&existing_endpoint_info);
		rv = bcm_petra_oam_endpoint_get(unit, endpoint_info->id, &existing_endpoint_info);
		BCMDNX_IF_ERR_EXIT(rv);

		if (existing_endpoint_info.flags & BCM_OAM_ENDPOINT_REMOTE) {
			rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, endpoint_info->id, &rmep_info, &found);
			BCMDNX_IF_ERR_EXIT(rv);
			if (!found) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
				   (_BSL_BCM_MSG("Error: Remote endpoint with id %d can not be found.\n"), endpoint_info->id));
			}

			LOG_VERBOSE(BSL_LS_BCM_OAM,
                                    (BSL_META_U(unit,
                                                "OAM: Remote endpoint replace.\n")));
			if (endpoint_info->loc_clear_threshold < 1) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: loc_clear_threshold must be positive. Current value:%d\n"), 
										endpoint_info->loc_clear_threshold));
			}

			/* Check parameters that can not be changed */
			if (endpoint_info->local_id != rmep_info.mep_index) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: local id can not be changed. Current value:%d\n"), 
										rmep_info.mep_index));
			}

			/*ccm_period*/
		    if (endpoint_info->ccm_period==0) {
				SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
				SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
				soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_info->local_id, &mep_db_entry);
				BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
				SOC_PPD_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, mep_db_entry.ccm_interval);
                MULTIPLE_BY_3_5(ccm_period_ms, ccm_period_micro_s);
				endpoint_info->ccm_period = ccm_period_ms;
			}
            SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);

			rv = _bcm_oam_rmep_db_entry_struct_set(unit, endpoint_info, &rmep_db_entry);
			BCMDNX_IF_ERR_EXIT(rv);

		    /* Update sw db of rmep with the new rmep */ 
			rv = _bcm_dpp_oam_bfd_rmep_info_db_delete(unit, endpoint_info->id);
			BCMDNX_IF_ERR_EXIT(rv);
			rv = _bcm_dpp_oam_bfd_rmep_info_db_insert(unit, endpoint_info->id, &rmep_info);
			BCMDNX_IF_ERR_EXIT(rv);

		    /* Update entry in rmep db and rmep index db*/
			_BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint_info->id);
		    soc_sand_rv = soc_ppd_oam_oamp_rmep_set(unit, rmep_id_internal, endpoint_info->name, 
												endpoint_info->local_id, endpoint_info->type, &rmep_db_entry, 1);
		    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
		}
		else {

            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info->id, &classifier_mep_entry, &found);
			BCMDNX_IF_ERR_EXIT(rv);
			if (!found) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
				   (_BSL_BCM_MSG("Local endpoint with id %d can not be found.\n"), endpoint_info->id));
			}
            if ( (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) != (existing_endpoint_info.flags & BCM_OAM_ENDPOINT_INTERMEDIATE) ) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: intermediate can not be updated.\n")));
            }
            if ( (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) != (existing_endpoint_info.flags & BCM_OAM_ENDPOINT_REMOTE) ) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: endpoint remote can not be updated.\n")));
            }
            if (endpoint_info->type != existing_endpoint_info.type){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: type can not be changed\n")));
            }
            if (((endpoint_info->opcode_flags & BCM_OAM_ENDPOINT_INTERMEDIATE)!=0) != ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)== 0)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Endpoint can not be updated from MEP to MIP or vise versa.\n")));
            }
			if (((endpoint_info->opcode_flags & BCM_OAM_OPCODE_CCM_IN_HW)!=0) != ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)!= 0)) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Acceleration in HW can not be added / removed.\n")));
			}
            if (is_accelerated){ 
                if (endpoint_info->tx_gport != BCM_GPORT_INVALID || existing_endpoint_info.tx_gport != BCM_GPORT_INVALID ) {
                    rv = _bcm_dpp_gport_compare(unit, endpoint_info->tx_gport, existing_endpoint_info.tx_gport, &port_is_equal);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (!port_is_equal) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: tx gport can not be updated.\n")));
                    }
                }

                if  ((endpoint_info->vlan !=existing_endpoint_info.vlan) ||(endpoint_info->outer_tpid !=existing_endpoint_info.outer_tpid) 
                     ||  (endpoint_info->pkt_pri !=existing_endpoint_info.pkt_pri) || (endpoint_info->inner_tpid !=existing_endpoint_info.inner_tpid)  ||
                      endpoint_info->intf_id !=existing_endpoint_info.intf_id 
                     || (endpoint_info->inner_pkt_pri !=existing_endpoint_info.inner_pkt_pri) || (endpoint_info->inner_vlan !=existing_endpoint_info.inner_vlan) )   {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: encapsulation fields can not be updated.\n")));
                }
                if (endpoint_info->name > SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)) {
					BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Endpoint name may not be greater than the max number of local MEPs.\n")));
                }

            }
            if (endpoint_info->gport != BCM_GPORT_INVALID || existing_endpoint_info.gport != BCM_GPORT_INVALID) {
                if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
                    rv = _bcm_dpp_gport_compare(unit, endpoint_info->gport, existing_endpoint_info.gport, &port_is_equal); 
                    BCMDNX_IF_ERR_EXIT(rv);
                } else {
                    /* gport_compare will not suffice for gports returned by MPLS APIs*/
                    int lif1, lif2,dont,care;
                    rv = _bcm_dpp_gport_to_lif(unit, endpoint_info->gport,&lif1, NULL, &dont, &care);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = _bcm_dpp_gport_to_lif(unit, existing_endpoint_info.gport, &lif2, NULL, &dont, &care);
                    BCMDNX_IF_ERR_EXIT(rv);
                    port_is_equal = (lif1 == lif2);
                }
                if ( !port_is_equal) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Gport can not be updated.\n")));
                }
            }
            if (endpoint_info->name != existing_endpoint_info.name){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: name can not be changed\n")));
            }


			if (endpoint_info->group != classifier_mep_entry.ma_index) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: MA id can not be updated.\n")));
			}

			if ((endpoint_info->level != classifier_mep_entry.md_level) && (!is_default)) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: level can not be updated.\n")));
			}

			if (((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING)!=0) != ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0)) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: MEP direction can not be updated.\n")));
			}

			if (endpoint_info->lm_counter_base_id != classifier_mep_entry.counter) {
				update_counter = 1;
			}
			else {
				update_counter = 0;
			}

			if (is_accelerated) { /* TX - OAMP */
				LOG_VERBOSE(BSL_LS_BCM_OAM,
                                            (BSL_META_U(unit,
                                                        "OAM: Accelerated endpoint update\n")));

				SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
				/*icc index allocation*/
				if (ma_name_struct.is_short) {
						icc_ndx = 15;
				}
				else { /*allocate a pointer to ICCMap register that holds Bytes 5:0 of SW.group_name_data_base(group).name. */
						SOC_PPD_OAM_GROUP_NAME_TO_ICC_MAP_DATA(ma_name_struct.name, data);
						rv = _bcm_dpp_am_template_oam_icc_map_ndx_alloc(unit, 0 /*flags*/, &data, &is_allocated, &icc_ndx);
						BCMDNX_IF_ERR_EXIT(rv);
				}

				rv = _bcm_oam_mep_db_entry_struct_set(unit, endpoint_info, &mep_db_entry, icc_ndx);
				BCMDNX_IF_ERR_EXIT(rv);
				/*update entry in mep db*/
				soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info->id, &mep_db_entry, 0, ma_name_struct.name);
				BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                if (SOC_IS_ARADPLUS(unit)) {
                    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY  old_prof;
                    uint8 new_rdi;
                    int new_profile;
                    /* RDI may have been updated. */
                    _BCM_OAM_SET_RDI_ON_ETH1731_PROF_RDI_FIELD(new_rdi,endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE,
                                                     endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE  );
                    rv= _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit ,endpoint_info->id , &profile,&old_prof);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (old_prof.rdi_gen_method != new_rdi) {
                        old_prof.rdi_gen_method = new_rdi;
                        rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_exchange(unit,profile,&old_prof,&is_allocated, &is_last,&new_profile );
                        BCMDNX_IF_ERR_EXIT(rv);

                        if (profile != new_profile) {
                            CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last,profile, endpoint_info->id);
                        }

                        soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_info->id, !is_allocated, new_profile, &old_prof);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
                    }



                    if (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK ) {
                        /* The state of the internal registers determining outgoing packets may have been changed, In this case outgoing LBMs
                           must reflect this change.*/
                        bcm_oam_loopback_t lb = { 0 };
                        uint32 period, lsbs, msbs;
                        int nic_profile, oui_profile;
                        soc_sand_rv = soc_ppd_oam_oamp_loopback_get_period(unit, &period);
                        lb.period = period;
                        lb.id = endpoint_info->id;
                        if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
                            rv = _bcm_dpp_am_template_oam_lmm_oui_tables_get(unit, endpoint_info->id, &oui_profile, &msbs);
                            BCMDNX_IF_ERR_EXIT(rv);
                            rv = _bcm_dpp_am_template_oam_lmm_nic_tables_get(unit, endpoint_info->id, &nic_profile, &lsbs);
                            BCMDNX_IF_ERR_EXIT(rv);
                            SET_MAC_ADDRESS_BY_MSB_AND_LSB(lb.peer_da_mac_address, lsbs, msbs);
                        }
                        lb.flags = BCM_OAM_LOOPBACK_UPDATE;
                        bcm_oam_loopback_add(unit,&lb);

                    }
                }
                
			}
			/*RX - classifier*/

			if (endpoint_info->gport != BCM_GPORT_INVALID) {
				SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY new_classifier_mep_entry;
				SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&new_classifier_mep_entry);
                rv = _bcm_oam_classifier_mep_entry_struct_set(unit, endpoint_info, &new_classifier_mep_entry);
				BCMDNX_IF_ERR_EXIT(rv);

				/* mep profiles weren't changed. Update the pointers from the old entry*/
				new_classifier_mep_entry.non_acc_profile = classifier_mep_entry.non_acc_profile;
				new_classifier_mep_entry.acc_profile = classifier_mep_entry.acc_profile;

				if (update_counter) {
					if (endpoint_info->lm_counter_base_id > 0) {
						rv = _bcm_dpp_oam_set_counter(unit, endpoint_info->lm_counter_base_id, ((endpoint_info->lm_flags & BCM_OAM_LM_PCP)!=0), new_classifier_mep_entry.lif);
						BCMDNX_IF_ERR_EXIT(rv);
					}
					else {
						rv = _bcm_dpp_oam_lif_table_set(unit, new_classifier_mep_entry.lif, 0);
						BCMDNX_IF_ERR_EXIT(rv);
					}
                    soc_sand_rv = soc_ppd_oam_classifier_oem_mep_add(unit, endpoint_info->id, &new_classifier_mep_entry, 1/*update*/);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
				}

				rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint_info->id);
				BCMDNX_IF_ERR_EXIT(rv);
				rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info->id, &new_classifier_mep_entry);
				BCMDNX_IF_ERR_EXIT(rv);
			}
            else if (is_default) {
				SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY new_classifier_mep_entry;
				SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&new_classifier_mep_entry);
                rv = _bcm_oam_classifier_default_mep_entry_struct_set(unit, endpoint_info, &new_classifier_mep_entry);
				BCMDNX_IF_ERR_EXIT(rv);

                /* mep profiles weren't changed. Update the pointers from the old entry*/
				new_classifier_mep_entry.non_acc_profile = classifier_mep_entry.non_acc_profile;

				if (update_counter) {
					if (endpoint_info->lm_counter_base_id > 0) {
						rv = _bcm_dpp_oam_set_counter(unit, endpoint_info->lm_counter_base_id, ((endpoint_info->lm_flags & BCM_OAM_LM_PCP)!=0), new_classifier_mep_entry.lif);
						BCMDNX_IF_ERR_EXIT(rv);
					}
					else {
						rv = _bcm_dpp_oam_lif_table_set(unit, new_classifier_mep_entry.lif, 0);
						BCMDNX_IF_ERR_EXIT(rv);
					}
                }
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_add,
                                                  (unit, default_id,&new_classifier_mep_entry, 0/*update_action_only*/));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

				rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint_info->id);
				BCMDNX_IF_ERR_EXIT(rv);
				rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info->id, &new_classifier_mep_entry);
				BCMDNX_IF_ERR_EXIT(rv);
            }
        }
	}
	else { /* New endpoint */
		/* Default OAM trap */
        if (is_default) {
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

                soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &classifier_mep_entry,
                                                                                         _bcm_oam_cpu_trap_code_to_mirror_profile_map[unit], 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            } else if (SOC_IS_ARADPLUS(unit)) { /* Arad+ default profiles */

                rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info->id, &classifier_mep_entry, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                         (_BSL_BCM_MSG("Error: Default endpoint already exists.\n")));
                }

                SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
                SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data);

                rv = _bcm_oam_classifier_default_mep_entry_struct_set(unit, endpoint_info, &classifier_mep_entry);
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

                rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info, classifier_mep_entry.lif);
                BCMDNX_IF_ERR_EXIT(rv);

                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_add,
                                                  (unit, default_id,&classifier_mep_entry, 0/*update_action_only*/));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &classifier_mep_entry,
                                                                                         _bcm_oam_cpu_trap_code_to_mirror_profile_map[unit], 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info->id, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);

            }
        }
        else { /* Non-default new endpoint */

            if (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) { /* Remote endpoint */
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                                        (BSL_META_U(unit,
                                                    "OAM: Remote endpoint create.\n")));

                SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);
                SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

                if (endpoint_info->loc_clear_threshold < 1) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: loc_clear_threshold must be positive. Current value:%d\n"),
                                                endpoint_info->loc_clear_threshold));
                }

                /* find associated MEP and check if it's accelerated */
                rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info->local_id, &classifier_mep_entry, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (!found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG("Error: Endpoint with id %d does not exist.\n"), endpoint_info->local_id));
                }
                if ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)== 0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Endpoint with id %d is not accelerated, remote endpoint can not be added\n"),
                                                endpoint_info->local_id));
                }

                if ((endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID)) {
                    if (!_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint_info->id)) {
                        _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(endpoint_info->id, endpoint_info->id);
                    }
                    rv = bcm_dpp_am_oam_rmep_id_is_alloced(unit, endpoint_info->id);
                    if (rv==BCM_E_EXISTS) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                 (_BSL_BCM_MSG("Error: Remote endpoint with id %d exists\n"), endpoint_info->id));
                    }
                    else if (rv != BCM_E_NOT_FOUND) {
                            BCMDNX_IF_ERR_EXIT(rv);
                    }
                    /* new rmep id given by the user */
                    flags = SHR_RES_ALLOC_WITH_ID;
                }
                rv = bcm_dpp_am_oam_rmep_id_alloc(unit, flags, (uint32*)&endpoint_info->id);
                BCMDNX_IF_ERR_EXIT(rv);
                new_id_alloced = TRUE;

                LOG_VERBOSE(BSL_LS_BCM_OAM,
                                        (BSL_META_U(unit,
                                                    "OAM: Inserting remote endpoint with id %d to mep with id %d\n"),
                                         endpoint_info->id, endpoint_info->local_id));

                /*ccm_period - If not specified 3.5 times the transmitter MEP ccm_period*/
                if (endpoint_info->ccm_period==0) {
                    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
                    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
                    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_info->local_id, &mep_db_entry);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    SOC_PPD_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, mep_db_entry.ccm_interval);
                    MULTIPLE_BY_3_5(ccm_period_ms, ccm_period_micro_s);
                    endpoint_info->ccm_period = ccm_period_ms;
                }

                rv = _bcm_oam_rmep_db_entry_struct_set(unit, endpoint_info, &rmep_db_entry);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Update sw db of mep->rmep with the new rmep */
                rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(unit, endpoint_info->local_id, endpoint_info->id);
                BCMDNX_IF_ERR_EXIT(rv);
                SOC_PPD_OAM_RMEP_INFO_DATA_clear(&rmep_info);
                rmep_info.mep_index = endpoint_info->local_id;
                rmep_info.rmep_id = endpoint_info->name;
                rv = _bcm_dpp_oam_bfd_rmep_info_db_insert(unit, endpoint_info->id, &rmep_info);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Update entry in rmep db and rmep index db*/
                _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint_info->id);
                soc_sand_rv = soc_ppd_oam_oamp_rmep_set(unit, rmep_id_internal,  endpoint_info->name,
                                                        endpoint_info->local_id, SOC_PPD_OAM_MEP_TYPE_ETH_OAM, &rmep_db_entry, 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            else { /* Local endpoint */
                if ((endpoint_info->gport == BCM_GPORT_INVALID) && (endpoint_info->tx_gport == BCM_GPORT_INVALID) &&
                    !_BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Both gport and tx_gport may not be invalid\n")));
                }

                if ((is_accelerated || _BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) && (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Intermediate endpoint can not be accelerated in HW.\n")));
                }

                if (is_accelerated) { /* TX - OAMP */
                    if ((endpoint_info->tx_gport == BCM_GPORT_INVALID) && (!is_upmep)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG
                                                          ("Error: tx_gport invalid, can not add accelerated downward facing endpoint.\n")));
                    }

                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "OAM: Accelerated endpoint create\n")));

                    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
                    rv = _bcm_dpp_oam_ma_db_get(unit, (uint32)endpoint_info->group, &ma_name_struct, &found);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) {
                        rv = _bcm_dpp_oam_bfd_mep_id_is_alloced(unit, endpoint_info->id);
                        if (rv == BCM_E_EXISTS) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                                (_BSL_BCM_MSG("Error: Local endpoint with id %d exists.\n"), endpoint_info->id));
                        } else if (rv != BCM_E_NOT_FOUND) {
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                        /* mep id given by the user */
                        flags = SHR_RES_ALLOC_WITH_ID;
                    }
                    rv = _bcm_dpp_oam_mep_id_alloc(unit, flags, ma_name_struct.is_short, (uint32 * )(&endpoint_info->id));
                    BCMDNX_IF_ERR_EXIT(rv);
                    new_id_alloced = TRUE;

                    /*icc index allocation*/
                    if (ma_name_struct.is_short) {
                        icc_ndx = 15;
                    } else { /*allocate a pointer to ICCMap register that holds Bytes 5:0 of SW.group_name_data_base(group).name. */
                        SOC_PPD_OAM_GROUP_NAME_TO_ICC_MAP_DATA(ma_name_struct.name, data);
                        rv = _bcm_dpp_am_template_oam_icc_map_ndx_alloc(unit, 0 /*flags*/, &data, &is_allocated, &icc_ndx);
                        BCMDNX_IF_ERR_EXIT(rv);
                        allocate_icc_ndx = is_allocated;
                    }

                    rv = _bcm_oam_mep_db_entry_struct_set(unit, endpoint_info, &mep_db_entry, icc_ndx);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_ACC_ENDPOINT_REF_COUNTER, &endpoint_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                    endpoint_ref_counter++;
                    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_ACC_ENDPOINT_REF_COUNTER, &endpoint_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);

                    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info->id, &mep_db_entry, allocate_icc_ndx, ma_name_struct.name);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    if (SOC_IS_ARADPLUS(unit)) {
                        /* Get the RDI in the eth1731 profile.*/
                        SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth_prof;
                        SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth_prof);
                        /* By default RDI is taken from scanner | RX */
                        _BCM_OAM_SET_RDI_ON_ETH1731_PROF_RDI_FIELD(eth_prof.rdi_gen_method, endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE,
                                                                   endpoint_info->flags2 & BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE);
                        rv = _bcm_dpp_am_template_oam_eth_1731_mep_profile_RDI_alloc(unit, &eth_prof, &is_allocated, &profile);
                        BCMDNX_IF_ERR_EXIT(rv);
                        soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_info->id, !is_allocated, profile, &eth_prof);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }

                    if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info) && !is_upmep) {
                        /* server down MEP. Set the traps.*/
                        uint32 server_trap_ref_counter;
                        SOC_PPC_OAM_MEP_TYPE mep_type;

                        _BCM_OAM_BCM_MEP_TYPE_TO_MEP_DB_MEP_TYPE(endpoint_info->type, mep_type);

                        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_SERVER_RX_TRAP, &server_trap_ref_counter,
                                                       BCM_GPORT_TRAP_GET_ID(endpoint_info->remote_gport));
                        BCMDNX_IF_ERR_EXIT(rv);
                        server_trap_ref_counter++;
                        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_SERVER_RX_TRAP, &server_trap_ref_counter, BCM_GPORT_TRAP_GET_ID(endpoint_info->remote_gport));
                        BCMDNX_IF_ERR_EXIT(rv);

                        if (server_trap_ref_counter == 1) {
                            /* This trap code hasen't been allocated yet */
                            soc_sand_rv = soc_ppd_oam_oamp_rx_trap_codes_set(unit, mep_type, BCM_GPORT_TRAP_GET_ID(endpoint_info->remote_gport));
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                    }

                }


                /*RX - classifier*/
                SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

                if (endpoint_info->gport == BCM_GPORT_INVALID) {
                    if (!is_accelerated)  {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                            (_BSL_BCM_MSG("Error: TX only: endpoint must be accelerated.\n")));
                    }
                    LOG_WARN(BSL_LS_BCM_OAM,
                             (BSL_META_U(unit,
                                         "OAM: Gport invalid - adding only TX.\n")));

                    classifier_mep_entry.flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED;
                    classifier_mep_entry.lif = _BCM_OAM_INVALID_LIF; /* Indication that RX does not exist */
                    classifier_mep_entry.ma_index = endpoint_info->group;

                    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) == 0 && BCM_GPORT_IS_TRUNK(endpoint_info->tx_gport))  {
                        classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER;
                    }

                    if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
                        /* additional information needs to be stored. Shove it in other fields.*/
                        if (is_upmep) {
                            classifier_mep_entry.port = endpoint_info->tx_gport;
                        } else {
                            classifier_mep_entry.remote_gport = endpoint_info->remote_gport;
                        }
                    }

                    /*update sw db of ma->mep & mep_info with new mep */
                    rv = _bcm_dpp_oam_ma_to_mep_db_insert(unit, endpoint_info->group, endpoint_info->id);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info->id, &classifier_mep_entry);
                    BCMDNX_IF_ERR_EXIT(rv);
                } else {
                    uint32 profile_temp;
                    rv = _bcm_oam_classifier_mep_entry_struct_set(unit, endpoint_info, &classifier_mep_entry);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) {
                        /* It has already been verified that _WITH_ID flag has been set. Update the ID.*/
                        _BCM_OAM_SET_MEP_INDEX_FOR_SERVER_CLIENT_SIDE(endpoint_info->id, endpoint_info->id);
                    } else if (!is_accelerated) { /* non-accelerated - allocate non-accelerated ID */
                        endpoint_info->id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(classifier_mep_entry.lif, endpoint_info->level,
                                                                                 ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0));
                    }


                    /* Configure two profile - one for actives side and one for passive side */
                    for (is_passive = 0; is_passive <= 1; is_passive++) {

                        profile_data_pointer = is_passive ? &profile_data_passive : &profile_data;

                        /* In Arad Mode Passive profile is fixed and configured separately */
                        if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && is_passive) {
                            continue;
                        }

                        /* Validate that mep with same lif&mdlevel does not exist with same direction */
                        soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(unit,
                                                                                                     classifier_mep_entry.lif,
                                                                                                     classifier_mep_entry.md_level,
                                                                                                     is_passive ^ is_upmep,
                                                                                                     &found,
                                                                                                     &profile_temp,
                                                                                                     &found_profile,
                                                                                                     &is_mp_type_flexible,
                                                                                                     &is_mip);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        /* Check if endpoint already exist (only in case of active side) */
                        if (!is_passive && found) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                                (_BSL_BCM_MSG("Error: Local endpoint with lif %d, mdlevel %d and direction %d(1-up,0-down) exists.\n"),
                                                 classifier_mep_entry.lif, endpoint_info->level,((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0)));
                        }

                        profile = (int)profile_temp;
                        if (found_profile) {
                            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {

                                /* get profile data existing on this lif */
                                rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                                BCMDNX_IF_ERR_EXIT(rv);

                                if (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
                                    /* passive_active_mix and mep_mip mix configuration in Arad+ mode */
                                    if (profile_data_pointer->mp_type_passive_active_mix && ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT) == 0)) {
                                        /* Previous type was up/down, now trying to add mip */
                                        if (is_mp_type_flexible) {
                                            profile_data_pointer->mp_type_passive_active_mix = FALSE;
                                            updated_mp_type = 1;
                                        } else {
                                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                                (_BSL_BCM_MSG("Error: Only two combinations of meps on same lif are possible - mips and meps (all up or down) OR only meps (up and down)."
                                                                              "Existing meps conflict with this constraint.\n")));
                                        }
                                    } else if (!profile_data_pointer->mp_type_passive_active_mix &&
                                               ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT) != 0)
                                               && !is_passive) {
                                        /* Previous type was mep/mip, now trying to add mep */
                                        /* Valid only in case the mep is on same direction as the existing mep, so looking for meps on the other direction */
                                        /* (active side check only) */
                                        _BCM_OAM_MEP_EXIST_ON_LIF_AND_DIRECTION(classifier_mep_entry.lif,
                                                                                ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == 0), 0/*find_mip*/, found, is_mip, profile_dummy);

                                        if (found & !is_mip) {
                                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                                (_BSL_BCM_MSG("Error: Only two combinations of meps on same lif are possible - mips and meps (all up or down) OR only meps (up and down)."
                                                                              "Existing meps conflict with this constraint.\n")));

                                        }
                                    }
                                }
                            } else {
                                /* Arad mode */
                                if ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT) != 0) {
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
                                            unit, classifier_mep_entry.lif, classifier_mep_entry.md_level,
                                            ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0),
                                            &found, &profile_temp, &found_profile, &is_mp_type_flexible, &is_mip);
                                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                                        profile = (int)profile_temp;
                                    }
                                    /* get profile data existing on this lif */
                                    rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                                    BCMDNX_IF_ERR_EXIT(rv);
                                }
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
                            profile_info.lif = classifier_mep_entry.lif;
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
                    }

                    if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (updated_mp_type)) {
                        soc_sand_rv = soc_ppd_oam_classifier_oem_mep_profile_replace(unit, &classifier_mep_entry, 1 /*update mp-type*/);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }

                    SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data_acc);
                    /* New Arad+ Classifier: in new mode this should always be done */
                    /* When using a remote OAMP as a server OAM-ID must be stamped on the FHEI. This is taken from the O-EM2 table*/
                    if ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && !is_mip) ||
                        is_accelerated || _BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) {
                        rv = _bcm_oam_default_profile_get(unit, &profile_data_acc, &classifier_mep_entry, is_accelerated, 0 /* is default */, is_upmep);
                        BCMDNX_IF_ERR_EXIT(rv);

                        if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) {
                            uint32 trap_code;
                            rv = _bcm_dpp_oam_bfd_trap_code_from_remote_gport(unit, endpoint_info->remote_gport, &trap_code, TRUE /*META data*/, &classifier_mep_entry);
                            BCMDNX_IF_ERR_EXIT(rv);

                            /* set the trap code for CCM m-cast to the above trap code.*/
                            profile_data_acc.mep_profile_data.opcode_to_trap_code_multicast_map[SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM] = trap_code;
                            classifier_mep_entry.remote_gport = endpoint_info->remote_gport; /* Also used by OAM-1-2 and for get APIs*/

                            if (!is_upmep) {
                                /* update the trap code to calculate the FWD header offset*/
                                bcm_rx_trap_config_t trap_config;
                                bcm_rx_trap_config_t_init(&trap_config);
                                rv = bcm_petra_rx_trap_get(unit, BCM_GPORT_TRAP_GET_ID(endpoint_info->remote_gport), &trap_config);
                                BCMDNX_IF_ERR_EXIT(rv);
                                trap_config.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_DEST;
                                rv = _bcm_dpp_rx_trap_set(unit, BCM_GPORT_TRAP_GET_ID(endpoint_info->remote_gport), &trap_config, _BCM_OAM_TRAP_FWD_OFFSET_INDEX);
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

                    rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info, classifier_mep_entry.lif);
                    BCMDNX_IF_ERR_EXIT(rv);

                    soc_sand_rv = soc_ppd_oam_classifier_oem_mep_add(unit, endpoint_info->id, &classifier_mep_entry, 0/*update*/);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    /* Active profile OAM table configuration */
                    soc_sand_rv = soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile(
                        unit,
                        SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint_info->id),
                        &classifier_mep_entry,
                        &profile_data,
                        &profile_data_acc,
                        _bcm_oam_cpu_trap_code_to_mirror_profile_map[unit]);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    if (!SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                        /* Passive profile OAM table configuration */
                        soc_sand_rv = soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile(
                            unit, 0, &classifier_mep_entry, &profile_data_passive, NULL, _bcm_oam_cpu_trap_code_to_mirror_profile_map[unit]);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }

                    /*update sw db of ma->mep & mep_info with new mep */
                    rv = _bcm_dpp_oam_ma_to_mep_db_insert(unit, endpoint_info->group, endpoint_info->id);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info->id, &classifier_mep_entry);
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM: Inserting local endpoint with id %d to group with id %d\n"),
                             endpoint_info->id, endpoint_info->group));
            }
        }
	}

    BCM_EXIT;

exit:
	if ((SOC_SAND_FAILURE(soc_sand_rv) || (BCM_FAILURE(rv))) && new_id_alloced) { 
		/* If error after new id was alloced we should free it */
		if (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
			rv = bcm_dpp_am_oam_rmep_id_dealloc(unit, endpoint_info->id);
			rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_rmep_delete(unit, endpoint_info->local_id, endpoint_info->id);
			rv = _bcm_dpp_oam_bfd_rmep_info_db_delete(unit, endpoint_info->id);
		}
		else {
			if (!is_accelerated) { /* TX - OAMP */
				rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, endpoint_info->id);
				if (allocate_icc_ndx) {
					rv = _bcm_dpp_am_template_oam_icc_map_ndx_free(unit, icc_ndx, &is_last);
				}
			}
			else {
				rv = _bcm_dpp_oam_ma_to_mep_db_mep_delete(unit, endpoint_info->group, endpoint_info->id);
				rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint_info->id);
			}
		}
	}
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint, 
								bcm_oam_endpoint_info_t *endpoint_info)
{
	uint32 soc_sand_rv=0;
	int rv;
	uint8 found;
	SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
	SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
	uint32 rmep_id_internal;
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

	SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);
	SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

	endpoint_info->id = endpoint;
	/* Default OAM trap */
	if (is_default) {
		rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &classifier_mep_entry, &found);
		BCMDNX_IF_ERR_EXIT(rv);

		if (!found) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Default profile not configured.\n")));
		}

	    endpoint_info->flags |= ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0) ? BCM_OAM_ENDPOINT_UP_FACING : 0;
        if (SOC_IS_ARADPLUS(unit)) {
            endpoint_info->group = classifier_mep_entry.ma_index;
            endpoint_info->level = classifier_mep_entry.md_level;
            endpoint_info->lm_counter_base_id = classifier_mep_entry.counter;
        }

		BCM_EXIT;
	}

	if (_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint)) {
	    /* remote endpoint */ 
		SOC_PPD_OAM_RMEP_INFO_DATA rmep_info;

		/* verify that rmep exists */
		SOC_PPD_OAM_RMEP_INFO_DATA_clear(&rmep_info);
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

		/*TX - OAMP */
		if ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)!= 0) {
			/*get entry of mep db*/
			soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint, &mep_db_entry);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

			rv = _bcm_oam_mep_db_entry_struct_get(unit, endpoint_info, &mep_db_entry);
			BCMDNX_IF_ERR_EXIT(rv);

            if ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER) != 0) {
                /* the tx gport is a TRUNK. override what was written in entry_struct_get()*/
                BCM_GPORT_TRUNK_SET(endpoint_info->tx_gport, (0x3FFF &mep_db_entry.system_port));
            }
            if (SOC_IS_ARADPLUS(unit)) {
                /* get the 2 RDI flags from the MEP professor.*/
                SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY  mep_prof;
                uint8 rdi_from_rx, rdi_from_scannner;
                int dont_care;
                rv= _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit ,endpoint_info->id , &dont_care,&mep_prof);
                BCMDNX_IF_ERR_EXIT(rv);

                _BCM_OAM_GET_RDI_STATUS_FROM_ETH1731_MEP_PROF_RDI_FIELD(mep_prof.rdi_gen_method, rdi_from_rx, rdi_from_scannner);
                endpoint_info->flags2 |= rdi_from_rx? 0 : BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE;
                endpoint_info->flags2 |= rdi_from_scannner? 0 : BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE;
            }
		}
	    /* RX - classifier*/
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
            if (classifier_mep_entry.port) {
                /* Server - up mep.*/
                uint32 system_port;
                endpoint_info->tx_gport = classifier_mep_entry.port;
                rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get(unit, mep_db_entry.local_port, &system_port);
                BCMDNX_IF_ERR_EXIT(rv);
                BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->remote_gport, system_port);
            } else if (classifier_mep_entry.remote_gport != BCM_GPORT_INVALID) {
                /*Server: down MEP*/
                endpoint_info->remote_gport = classifier_mep_entry.remote_gport;
            }

        }
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_oam_endpoint_destroy (int unit, bcm_oam_endpoint_t endpoint) {

	uint32 soc_sand_rv = SOC_SAND_OK;
	int rv;
	bcm_oam_ma_name_t ma_name_struct;
	endpoint_list_t * rmep_list_p = NULL;
	endpoint_list_member_t * rmep_iter_p = NULL;
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

            soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &mep_info,
                                                                                     _bcm_oam_cpu_trap_code_to_mirror_profile_map[unit], 0);
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
                rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data.mep_profile_data, 1/*is_mep*/);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data.mip_profile_data, 0/*is_mep*/);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            rv = _bcm_oam_default_id_from_ep_id(unit, endpoint, &default_id);
            BCMDNX_IF_ERR_EXIT(rv);

            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_remove,
                                                                                    (unit, default_id));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

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

            if (SOC_IS_ARADPLUS(unit)) {
                SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
                SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA punt_profile;

                soc_sand_rv = soc_ppd_oam_oamp_rmep_get(unit, rmep_id_internal, &rmep_db_entry);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                /* rmep_db_entry is initialized by soc_ppd_oam_oamp_rmep_get */
                /* coverity[uninit_use_in_call:FALSE] */
                rv = _bcm_dpp_am_template_oam_punt_event_hendling_profile_free(unit, rmep_db_entry.punt_profile, &is_last);
                BCMDNX_IF_ERR_EXIT(rv);
                if (is_last) {
                    SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA_clear(&punt_profile);
                    soc_sand_rv = soc_ppd_oam_oamp_punt_event_hendling_profile_set(unit, rmep_db_entry.punt_profile, &punt_profile);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }

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
            if ((mep_info.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)!=0 ) {
                if (SOC_IS_ARADPLUS(unit)) {
                    bcm_oam_loss_t loss_obj;
                    bcm_oam_delay_t delay_obj;
                    int was_removed=0;

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
                        bcm_oam_loopback_t loopback = {0};
                        loopback.id = endpoint;
                        rv = bcm_petra_oam_loopback_delete(unit,&loopback);
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
                    }

                    /*free the mep profile entry.*/
                    rv =  _bcm_dpp_am_template_oam_eth1731_mep_profile_free(unit,endpoint,  &prof_ndx, &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);
                    CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last,prof_ndx, endpoint);


                }
                /*update sw db of mep->rmep : delete all rmeps */
                rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint, &rmep_list_p, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (found) { /* this mep has rmeps */
                    /*Loop on all RMEPs associated with each MEP*/
                    rmep_iter_p = rmep_list_p->first_member;
                    while (rmep_iter_p!=NULL) {
                        current_endpoint = rmep_iter_p->index;
                        rmep_iter_p = rmep_iter_p->next;
                        rv = bcm_petra_oam_endpoint_destroy(unit, current_endpoint);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }

                rv = _bcm_dpp_oam_ma_db_get(unit, mep_info.ma_index, &ma_name_struct, &found);
                BCMDNX_IF_ERR_EXIT(rv);

                /*get entry of mep db*/
                soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint, &mep_db_entry);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (!(ma_name_struct.is_short)) {

                    /*Deallocate the pointer to ICCMap register*/

                    /* The MBCM_DPP_DRIVER_CALL initializing the required varible */
                    /* coverity[uninit_use_in_call:FALSE] */
                    rv = _bcm_dpp_am_template_oam_icc_map_ndx_free(unit, mep_db_entry.icc_ndx, &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (is_last) {
                        deallocate_icc_ndx = TRUE;
                    }
                }

                if (mep_db_entry.mep_type != SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {
                    rv = _bcm_dpp_am_template_mpls_pwe_push_profile_free(unit, mep_db_entry.push_profile, &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (is_last) {
                        SOC_PPD_MPLS_PWE_PROFILE_DATA push_data;
                        SOC_PPD_MPLS_PWE_PROFILE_DATA_clear(&push_data);
                        soc_sand_rv = soc_ppd_oam_mpls_pwe_profile_set(unit, mep_db_entry.push_profile, &push_data);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                } else if (SOC_IS_JERICHO(unit)){
                    int dont_care;
                    rv = _bcm_dpp_am_template_oam_sa_mac_address_free(unit,mep_db_entry.src_mac_msb_profile,&dont_care);
                    BCMDNX_IF_ERR_EXIT(rv);

                }

                rv = _bcm_dpp_am_template_oam_tx_priority_free(unit, mep_db_entry.priority, &is_last);
                BCMDNX_IF_ERR_EXIT(rv);
                if (is_last) {
                    SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
                    SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
                    soc_sand_rv = soc_ppd_oam_oamp_tx_priority_registers_set(unit, mep_db_entry.priority, &tx_itmh_attributes);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }

                rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_ACC_ENDPOINT_REF_COUNTER, &endpoint_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
                endpoint_ref_counter--;
                rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_ACC_ENDPOINT_REF_COUNTER, &endpoint_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);

            if ((SOC_IS_ARAD_B0_AND_ABOVE(unit) && PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry.mep_type) && 
                !(mep_info.flags &SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) ) || (mep_info.lif== _BCM_OAM_INVALID_LIF && mep_info.port)  ) {
				/* This table has been set by down MEPs and server up MEPs. */
                    int ignored;
                    rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_free(unit,endpoint,&ignored);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if ((mep_info.flags &SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) && (mep_info.lif== _BCM_OAM_INVALID_LIF && mep_info.port) ) {
                        /* Free the entry in the OAMP-PE gen memory entry*/
                        rv = _bcm_dpp_am_template_oam_oamp_pe_gen_mem_free(unit, endpoint, &ignored);
                        BCMDNX_IF_ERR_EXIT(rv); 
                    }
                }

            if ( BCM_GPORT_INVALID!= mep_info.remote_gport && (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == 0) {
                /* Server, down MEP. Free the trap.*/
				uint32 server_trap_ref_counter;
				rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_SERVER_RX_TRAP, &server_trap_ref_counter, BCM_GPORT_TRAP_GET_ID(mep_info.remote_gport));
                BCMDNX_IF_ERR_EXIT(rv);
                server_trap_ref_counter--;
                rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_SERVER_RX_TRAP, &server_trap_ref_counter, BCM_GPORT_TRAP_GET_ID(mep_info.remote_gport));
                BCMDNX_IF_ERR_EXIT(rv);

                if (!server_trap_ref_counter) {
                    soc_sand_rv = soc_ppd_oam_oamp_rx_trap_codes_delete(unit, mep_info.mep_type, BCM_GPORT_TRAP_GET_ID(mep_info.remote_gport));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
                }

                soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_delete(unit, endpoint, deallocate_icc_ndx, (endpoint_ref_counter==0));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, endpoint);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /*RX - classifier*/

            /* Free profiles  - Only if RX is set*/
            if (mep_info.lif != _BCM_OAM_INVALID_LIF) {

                /* accelerated profile */
            if ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && (mep_info.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT) != 0) || 
				((mep_info.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) ||
				_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint)) {
                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "Freeing accelerated profile for MEP %d\n"),endpoint));
                    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, endpoint, &profile_data_acc);
                    BCMDNX_IF_ERR_EXIT(rv);

                    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_free(unit, mep_info.acc_profile, &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (is_last) {
                        rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data_acc.mep_profile_data, 1/*is_mep*/);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }

                /* non-accelerated profile */
                for (is_passive = 0; is_passive <= 1; is_passive++) {

                    /* In Arad Mode Passive profile is fixed and configured separately */
                    if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && is_passive) {
                        continue;
                    }

                    rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, endpoint, is_passive, &profile_data);
                    BCMDNX_IF_ERR_EXIT(rv);

                    rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_free(unit, (is_passive ? mep_info.non_acc_profile_passive : mep_info.non_acc_profile), &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (is_last) {
                        rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data.mep_profile_data, 1/*is_mep*/);
                        BCMDNX_IF_ERR_EXIT(rv);
                        rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data.mip_profile_data, 0/*is_mep*/);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }

                soc_sand_rv = soc_ppd_oam_classifier_mep_delete(unit, endpoint, &mep_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* Find if there are other meps on same lif */
                _BCM_OAM_MEP_EXIST_ON_LIF_AND_DIRECTION(mep_info.lif, TRUE /*is_upmep*/, 0/*find_mip*/, found, is_mip_dummy, profile_dummy);

                if (!found) { /* Serch meps on the other directions */
                    _BCM_OAM_MEP_EXIST_ON_LIF_AND_DIRECTION(mep_info.lif, FALSE/*is_upmep*/, 0/*find_mip*/, found, is_mip_dummy, profile_dummy);
                }

                if (!found) { /* No more meps on this lif */
                    rv = _bcm_dpp_oam_lif_table_set(unit, mep_info.lif, 0);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && !SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
                    /* If this is the last MIP on this Lif, change profile to active_passive and add passive endpoint to passive bitmap */
                    if ((mep_info.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT) == 0) {

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
                                                                      ((classifier_mep_entry_dummy.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == 0));
                                    BCMDNX_IF_ERR_EXIT(rv);
                                }

                                /* Move all existing endpoints on this lif to the new active profile (only SW DB) */
                                profile_info.first_endpoint_id = BCM_OAM_ENDPOINT_INVALID;
                                profile_info.lif = mep_info.lif;
                                profile_info.profile_data = &profile_data;
                                profile_info.is_ingress = ((classifier_mep_entry_dummy.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == 0) ^ is_passive;
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

                                if (classifier_mep_entry_dummy.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) {
                                    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, profile_info.first_endpoint_id, &profile_data_acc);
                                    BCMDNX_IF_ERR_EXIT(rv);
                                }

                                /* Update HW */
                                soc_sand_rv = soc_ppd_oam_mep_passive_active_enable_set(unit, 
                                                                                        is_passive ? classifier_mep_entry_dummy.non_acc_profile_passive : classifier_mep_entry_dummy.non_acc_profile,
                                                                                        1/*enable active_passive_mix*/);
                                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                                soc_sand_rv = soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile(
                                   unit, 0, &classifier_mep_entry_dummy, &profile_data, is_passive ? NULL : &profile_data_acc, _bcm_oam_cpu_trap_code_to_mirror_profile_map[unit]);
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

    BCMDNX_INIT_FUNC_DEFS;

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

    if ((BCM_OAM_ACTION_GET(*action, bcmOAMActionSLMEnable)) && (!SOC_IS_ARADPLUS(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("SLM is available only in Arad+.")));
    }

	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);


    /*verify that the opcode is  amongst the supported options. */
    for (action_option  = 0; action_option < bcmOAMActionCount; ++action_option) {
        if (action_option != bcmOAMActionMcFwd && action_option != bcmOAMActionUcFwd && action_option != bcmOAMActionMeterEnable
            && action_option != bcmOAMActionCountEnable && action_option != bcmOAMActionMcDrop && action_option != bcmOAMActionUcDrop
            && action_option != bcmOAMActionSLMEnable && action_option != bcmOAMActionUcFwdAsData && action_option != bcmOAMActionMcFwdAsData) {
            /* such an opcode is not allowed! verify that it isn't set*/
            if (BCM_OAM_ACTION_GET(*action, action_option)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported action: %d. Only bcmOAMActionMcFwd, bcmOAMActionUcFwd, bcmOAMActionMeterEnable,"
                                                        "bcmOAMActionMcFwdAsData, bcmOAMActionUcFwdAsData and bcmOAMActionCountEnable supported.\n"), action_option));
            }
        }
    }

	rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, id, &classifier_mep_entry, &found);
	BCMDNX_IF_ERR_EXIT(rv);
	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Mep id %d does not exist.\n"), id));
	}

    if (classifier_mep_entry.mep_type !=SOC_PPD_OAM_MEP_TYPE_ETH_OAM && BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwd)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: for types other than Ethernet OAM select multicast.\n")));
	}

    is_mip = ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)== 0);

    if (BCM_OAM_ACTION_GET(*action, bcmOAMActionSLMEnable) && is_mip) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: SLM action is not supported for MIPs.\n")));
    }

	/* All opcodes include meta data.*/

	is_accelerated = ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)!= 0  || 
					  _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(id)); /* Classifier entries in the latter case act as accelerated entries.*/
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
	if (((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && !is_mip) || is_accelerated ||
		 (SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(id))) && !is_default) {
		rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, id, &profile_data_acc);
		BCMDNX_IF_ERR_EXIT(rv);
		SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&old_profile_data_acc);
		rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, id, &old_profile_data_acc);
		BCMDNX_IF_ERR_EXIT(rv);
	}

    if (action->destination != BCM_GPORT_INVALID) {
        if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcDrop) || BCM_OAM_ACTION_GET(*action, bcmOAMActionMcDrop) ||
            BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwdAsData) || BCM_OAM_ACTION_GET(*action, bcmOAMActionMcFwdAsData)) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Drop Fwd and as data actions supports only invalid gport in destination field.\n")));
        }
		rv = _bcm_dpp_oam_bfd_trap_code_from_remote_gport(unit, action->destination, &trap_code, 1 /* includes meta-data */, &classifier_mep_entry);
		BCMDNX_IF_ERR_EXIT(rv);


		/* Set return value trap code as gport */
		if ( soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) {
            BCM_GPORT_TRAP_SET(action->rx_trap, SOC_PPD_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH, 7, 0);
        } else if (!BCM_GPORT_IS_TRAP(action->destination)  && !SOC_PPD_TRAP_CODE_OAM_IS_FTMH_MIRROR(trap_code)){
			BCM_GPORT_TRAP_SET(action->rx_trap, trap_code, 7, 0);
        }

    }
    else {
        if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwd) || BCM_OAM_ACTION_GET(*action, bcmOAMActionMcFwd)) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Fwd action supports only valid gport in destination field.\n")));
        }
    }

	/* Fill new profile */
    for (opcode = 0; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
         if (BCM_OAM_OPCODE_GET(*action, opcode)) {
             /* Translate to internal event */
             rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
    		 BCMDNX_IF_ERR_EXIT(rv);
             LOG_DEBUG(BSL_LS_BCM_OAM,
                       (BSL_META_U(unit,
                                   "\n------------------------------ trap code is %d, internal trap code: %d, opcode: %d\n"), trap_code,
                        internal_opcode, opcode)); 

			 if (!is_mip) {
				 if ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) || is_accelerated ||
					  SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(id)) && !is_default) {
					 _BCM_OAM_PROFILE_SINGLE_OPCODE_SET(profile_data_acc.mep_profile_data, internal_opcode, action, trap_code);
				 }
				 else { /* Modify only to non-acc profile */
					 _BCM_OAM_PROFILE_SINGLE_OPCODE_SET(profile_data.mep_profile_data, internal_opcode, action, trap_code);
				 }
			 }
			 else { /*MIP*/
                 /* In case of MIP passive profile should be modified as well */
				 _BCM_OAM_PROFILE_SINGLE_OPCODE_SET(profile_data.mip_profile_data, internal_opcode, action, trap_code);
				 _BCM_OAM_PROFILE_SINGLE_OPCODE_SET(profile_data_passive.mip_profile_data, internal_opcode, action, trap_code);
			 }
         }
    }

    /* SLM support - set profile */
    profile_data.is_slm = BCM_OAM_ACTION_GET(*action, bcmOAMActionSLMEnable) ? 1 : 0;

    if (is_accelerated) {
        profile_data_acc.is_piggybacked = is_piggy_backed;
    }

    /* For MIP - update passive profile as well */
    for (is_passive=0; is_passive<=is_mip; is_passive++) {

        profile_data_pointer = is_passive ? &profile_data_passive : &profile_data;

        /* get the old profile */
        old_profile = is_passive ? classifier_mep_entry.non_acc_profile_passive : classifier_mep_entry.non_acc_profile;

        /* Move all existing endpoints on this lif to the new profile */
        profile_info.first_endpoint_id = BCM_OAM_ENDPOINT_INVALID;
        profile_info.lif = classifier_mep_entry.lif;
        profile_info.profile_data = profile_data_pointer;
        profile_info.is_ingress = ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP)== 0) ^ is_passive;
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
        if (profile_info.is_alloced) {           
            if (SOC_IS_ARADPLUS(unit)) {
                /* SLM support - disable data counting */
                if (profile_data.is_slm) {
                    soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, profile, 1);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
                else {
                    soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, profile, 0);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }
        if (profile_info.is_last) { 
            /* SLM support - enable data counting */
            if (SOC_IS_ARADPLUS(unit) && profile_change) {
                soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, old_profile, 0);
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

        /* Accelerated profile - relevant only for active side */
        /* New Arad+ Classifier: in the new implementation this should always be done */
        if (!is_passive) {
            if (((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && !is_mip) || is_accelerated ||
				 SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(id)) && !is_default) {
            rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_exchange(unit, 0/*flags*/, id, &profile_data_acc,
                                                         &old_profile_acc, &is_last, &profile, &is_allocated);
                BCMDNX_IF_ERR_EXIT(rv);
                if (classifier_mep_entry.acc_profile != profile) {
                    classifier_mep_entry.acc_profile = profile;
                    profile_change = 1;

					rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, id);
					BCMDNX_IF_ERR_EXIT(rv);
					rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, id, &classifier_mep_entry);
					BCMDNX_IF_ERR_EXIT(rv);
                }

                if (is_allocated) {
                    rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data_acc.mep_profile_data);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                if (is_last) { 
                    rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &old_profile_data_acc.mep_profile_data, 1/*is_mep*/);
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
            soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, profile_data_pointer, &classifier_mep_entry,
                                                                                     _bcm_oam_cpu_trap_code_to_mirror_profile_map[unit], 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            soc_sand_rv = soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile(
               unit,
               SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(id),
                &classifier_mep_entry, profile_data_pointer, 
               (is_passive ? NULL : &profile_data_acc),
                _bcm_oam_cpu_trap_code_to_mirror_profile_map[unit]);
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
	if (SOC_SAND_FAILURE(soc_sand_rv) || (BCM_FAILURE(rv))) {
		/* Clear allocated trap codes */
		if (trap_code != 0) {
			rv = bcm_petra_rx_trap_type_destroy(unit, trap_code);
            if(BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "bcm_petra_rx_trap_type_destroy failed: %s\n"),
                           bcm_errmsg(rv)));
            }
			if (is_mip) {
				rv = bcm_petra_rx_trap_type_destroy(unit, trap_code+1);
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


int bcm_petra_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group) {

	bcm_oam_endpoint_t current_endpoint;
	endpoint_list_t * mep_list_p;
	endpoint_list_member_t * mep_iter_p = NULL;
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

	if (found) {
		/*Loop on all the MEPs of this MA*/
		mep_iter_p = mep_list_p->first_member;
		while (mep_iter_p!=NULL) {
			current_endpoint = mep_iter_p->index;
			mep_iter_p = mep_iter_p->next;
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
	bcm_oam_ma_name_t ma_name;
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


int bcm_petra_oam_endpoint_traverse(int unit, bcm_oam_group_t group,bcm_oam_endpoint_traverse_cb cb,void *user_data){  
     int rv;
    endpoint_list_t* mep_list_p = NULL;
    endpoint_list_member_t* mep_iter_p = NULL;
    uint8 found;
    bcm_oam_endpoint_info_t current_endpoint;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_ma_to_mep_db_get(unit,group,&mep_list_p, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (found) {/*Go over the endpoints in mep_list_p, calling the user's callback on each one.*/
        mep_iter_p = mep_list_p->first_member;
        while (mep_iter_p) {
            bcm_oam_endpoint_info_t_init(&current_endpoint);
            current_endpoint.id = mep_iter_p->index;
            rv = bcm_petra_oam_endpoint_get(unit,current_endpoint.id, &current_endpoint);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = cb(unit,&current_endpoint, user_data);
             if (rv != 0) {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("callback in bcm_oam_endpoint_traverse returned error message %s"),bcm_errmsg(rv)));
             }

            mep_iter_p = mep_iter_p->next;
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

		rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_with_meta);
		BCMDNX_IF_ERR_EXIT(rv);
	}

	rv = bcm_petra_rx_trap_type_destroy(unit, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_OAMP]);
    BCMDNX_IF_ERR_EXIT(rv);

	rv = bcm_petra_rx_trap_type_destroy(unit, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_MPLS]);
    BCMDNX_IF_ERR_EXIT(rv);

	rv = bcm_petra_rx_trap_type_destroy(unit, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_PWE]);
    BCMDNX_IF_ERR_EXIT(rv);

	_BCM_BFD_ENABLED_GET(is_bfd_init);

	if (!is_bfd_init) {
		rv = _bcm_dpp_oam_dbs_destroy(unit, TRUE/*is_oam*/);
		BCMDNX_IF_ERR_EXIT(rv);

		rv = bcm_petra_rx_trap_type_destroy(unit, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_CPU]); 
		BCMDNX_IF_ERR_EXIT(rv);
	}
	
	rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(unit, _bcm_dpp_oam_trap_info[unit].upmep_trap_ids[SOC_PPD_OAM_UPMEP_TRAP_ID_CPU]);
	BCMDNX_IF_ERR_EXIT(rv);

    /* Get snoop command */
    bcm_rx_trap_config_t_init(&trap_config);
    rv = bcm_petra_rx_trap_get(unit, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_SNOOP], &trap_config);
    BCMDNX_IF_ERR_EXIT(rv);

    /*destroy snoop trap id*/
	rv = bcm_petra_rx_snoop_destroy(unit, 0, trap_config.snoop_cmnd);
	BCMDNX_IF_ERR_EXIT(rv);

	rv = bcm_petra_rx_trap_type_destroy(unit, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_SNOOP]);
    BCMDNX_IF_ERR_EXIT(rv);

	rv = bcm_petra_rx_trap_type_destroy(unit, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_RECYCLE]);
    BCMDNX_IF_ERR_EXIT(rv);

	rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(unit, _bcm_dpp_oam_trap_info[unit].upmep_trap_ids[SOC_PPD_OAM_UPMEP_TRAP_ID_OAMP]);
	BCMDNX_IF_ERR_EXIT(rv);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) {
        /* free all resources used for egress snooping*/
        rv = bcm_petra_rx_snoop_destroy(unit, 0,_BCM_OAM_MIP_SNOOP_CMND_UP);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_rx_snoop_destroy(unit, 0,_BCM_OAM_MIP_SNOOP_CMND_DOWN);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_rx_trap_type_destroy(unit,SOC_PPD_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH);
        BCMDNX_IF_ERR_EXIT(rv); 
    }

#ifdef _BCM_OAM_LOOPBACK_UPMEP_SUPPORTED /*LoopBack upmep is not supported in ARAD*/
	rv = bcm_petra_rx_trap_type_destroy(unit, _bcm_dpp_oam_trap_info[unit].upmep_trap_ids[SOC_PPD_OAM_UPMEP_TRAP_ID_RECYCLE]);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /*LoopBack upmep is not supported in ARAD*/

	rv = bcm_petra_rx_trap_type_destroy(unit, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_ERR_LEVEL]);
    BCMDNX_IF_ERR_EXIT(rv);

	rv = bcm_petra_rx_trap_type_destroy(unit, _bcm_dpp_oam_trap_info[unit].trap_ids[SOC_PPD_OAM_TRAP_ID_ERR_PASSIVE]);
    BCMDNX_IF_ERR_EXIT(rv);

	/*destroy all the mirror profiles*/
	_BCM_OAM_MIRROR_PROFILE_DEALLOC(_bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_OAMP]);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, _bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_OAMP], 0, 0, 7);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

	_BCM_OAM_MIRROR_PROFILE_DEALLOC(_bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_CPU]);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, _bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_CPU], 0, 0, 7);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

#ifdef _BCM_OAM_LOOPBACK_UPMEP_SUPPORTED /* LoopBack upmep not supported - trap to CPU using CPU trap id */
	_BCM_OAM_MIRROR_PROFILE_DEALLOC(_bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_OAMP]);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, _bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_RECYCLE], 0,7);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
#endif

	_BCM_OAM_MIRROR_PROFILE_DEALLOC(_bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_SNOOP]);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, _bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_SNOOP], 0, 0,7);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

	_BCM_OAM_MIRROR_PROFILE_DEALLOC(_bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_ERR_LEVEL]);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, _bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_ERR_LEVEL], 0, 0, 7);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

	_BCM_OAM_MIRROR_PROFILE_DEALLOC(_bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_ERR_PASSIVE]);
	soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, _bcm_dpp_oam_trap_info[unit].mirror_ids[SOC_PPD_OAM_MIRROR_ID_ERR_PASSIVE], 0, 0, 7);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

	/* deinit _bcm_oam_cpu_trap_code_to_mirror_profile_map */
	for (trap_code = 1; trap_code < SOC_PPD_NOF_TRAP_CODES; trap_code++) {
		mirror_profile = 0;
		rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &mirror_profile, trap_code);
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

	rv = _bcm_dpp_oam_trap_info_wb_add(unit);
	BCMDNX_IF_ERR_EXIT(rv);

	_BCM_OAM_ENABLED_SET(FALSE);

	rv = _bcm_dpp_oamp_interrupts_init(unit, FALSE);
	BCMDNX_IF_ERR_EXIT(rv);

	soc_sand_rv = soc_ppd_oam_deinit(unit, 0/*is_bfd*/, !is_bfd_init);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_detach(int unit) {

	int rv = BCM_E_NONE;
	int trap_code;
	uint8 mirror_profile;
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
	}

	/* deinit _bcm_oam_cpu_trap_code_to_mirror_profile_map */
	for (trap_code = 1; trap_code < SOC_PPD_NOF_TRAP_CODES; trap_code++) {
		mirror_profile = 0;
		rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TO_MIRROR_PROFILE_MAP, &mirror_profile, trap_code);
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
	rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
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

	rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_opcode, opcode);
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

        if (is_dm) {
             _OAM_SET_DMM_OFFSET(mep_db_entry, eth_1731_profile->dmm_offset);
             _OAM_SET_DMR_OFFSET(mep_db_entry, eth_1731_profile->dmr_offset);
        } else {
             _OAM_SET_LMM_OFFSET(mep_db_entry,eth_1731_profile->lmm_offset, eth_1731_profile->piggy_back_lm, eth_1731_profile->slm_lm );
             _OAM_SET_LMR_OFFSET(mep_db_entry,eth_1731_profile->lmr_offset,  eth_1731_profile->piggy_back_lm, eth_1731_profile->slm_lm);
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


	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&old_eth_data);


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
			eth1731_profile_to_use->lmm_da_oui_prof =0;
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
	SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY old_profile;
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
    BCMDNX_INIT_FUNC_DEFS;

	(void)l_or_d; /*for compilation*/

    if (!only_on_demand_dmm) {
		soc_sand_rv = soc_ppd_oam_oamp_search_for_lm_dm(unit, endpoint_id, found); 
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


		*found = is_lm? SHR_BITGET(found, SOC_PPD_OAM_MEP_TYPE_LM)!=0  : SHR_BITGET(found, SOC_PPD_OAM_MEP_TYPE_DM) !=0; 

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
	rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit,endpoint_id , &eth1731_profile, &old_profile);
    BCMDNX_IF_ERR_EXIT(rv); 

    /*De-alloc from the mep_db, da_nic, da_oui and eth1731-profile tables.*/ 
    if ( (!old_profile.piggy_back_lm || !is_lm ) &&  ccm_entry_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) { 
		/*piggy back entries did not allocate entries from the DA MAC tables*/
		FREE_NIC_AND_OUI_PROFILES(endpoint_id);
    }

    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile_data);
    
    /*Next, get a different profile. Fill all the fieldds from the old entry.*/
	eth1731_profile_data.rdi_gen_method = old_profile.rdi_gen_method;
	if (is_lm) {
		eth1731_profile_data.dmm_rate = old_profile.dmm_rate;
		eth1731_profile_data.dmm_offset = old_profile.dmm_offset;
		eth1731_profile_data.dmr_offset = old_profile.dmr_offset;
	} else {
		eth1731_profile_data.lmm_rate = old_profile.lmm_rate;
		eth1731_profile_data.lmm_offset = old_profile.lmm_offset;
		eth1731_profile_data.lmr_offset = old_profile.lmr_offset;
		eth1731_profile_data.piggy_back_lm = old_profile.piggy_back_lm;
	} if (has_LBR || eth1731_profile_data.dmm_rate || eth1731_profile_data.lmm_rate ) {
		eth1731_profile_data.lmm_da_oui_prof = old_profile.lmm_da_oui_prof;
	}
	rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_exchange(unit, eth1731_profile, &eth1731_profile_data, &is_allocated, &is_removed, &new_eth_1731_prof);
	if (eth1731_profile != new_eth_1731_prof) {
		CLEAR_ETH_1731_PROF_IF_IS_LAST(is_removed, eth1731_profile, endpoint_id);
	}
	soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_id, !is_allocated, new_eth_1731_prof, &eth1731_profile_data);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

    if (!only_on_demand_dmm) {
		soc_sand_rv = soc_ppd_oam_oamp_lm_dm_remove(unit, endpoint_id, is_lm,   &num_removed, &removed_index); 
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


		/* Now free the the mep db entries*/
		rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, removed_index);
		BCMDNX_IF_ERR_EXIT(rv); 
		LOG_VERBOSE(BSL_LS_BCM_OAM,
					(BSL_META_U(unit,
								"entry %d freed\n"), removed_index));
		if (num_removed ==2) {/*Go here if we want to delete an LM entry and there is also a LM-STAT entry.*/
			rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, removed_index+1);
			BCMDNX_IF_ERR_EXIT(rv); 
			LOG_VERBOSE(BSL_LS_BCM_OAM,
						(BSL_META_U(unit,
									"entry %d freed as well\n"), removed_index+1));
		}
    }

    /*Have the classifier send LM/DM packets back to the CPU.*/

    rv = bcm_petra_port_config_get(unit, &port_config);
    BCMDNX_IF_ERR_EXIT(rv); 
    /*get CPU port*/
    BCM_PBMP_ITER(port_config.cpu,action.destination) {
       break; /*get first CPU port*/
    }
	/* fourth parameter piggy_back_or_1DM : for DM always 1, for LM 0 iff removed entry was P-B*/
    SET_CLASSIFIER_ACTION(endpoint_id,action,is_lm, (!is_lm || !(is_lm && old_profile.piggy_back_lm) ) ,  ccm_entry_type);

    if (only_on_demand_dmm) {
        uint8 found;
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_id, classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found.\n"), endpoint_id));
        }
        rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint_id);
        BCMDNX_IF_ERR_EXIT(rv);
        
		classifier_mep_entry->flags &= ~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;

        rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_id, classifier_mep_entry);
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

    if (classifier_mep_entry->flags &SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) {
    /* Send LBRs to the to the CPU.*/
        rv = bcm_petra_port_config_get(unit, &port_config);
        BCMDNX_IF_ERR_EXIT(rv);
        /*get CPU port*/
        BCM_PBMP_ITER(port_config.cpu, action.destination) {
            break; /*get first CPU port*/
        }
    } else { /* send to the recycle port so that PRGE will answer LBMs with LBRs*/
         action.destination = soc_property_get(unit, spn_OAM_RCY_PORT, -1);
        if ( action.destination == -1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: OAM recycling port is not configured.\n")));
        }
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



int bcm_petra_oam_loss_add(int unit, bcm_oam_loss_t *loss_ptr) {
	uint8 oam_is_init,found, is_ok, tlv_used;
    uint32 correct_flags =0, lsb, msb;
    int dont_care; 
    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile;
    bcm_oam_endpoint_action_t action;
    int rv;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY mep_db_entry;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_EXIT;
    }
    
	if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

    /*First, verify that the parameters are correct and that the endpoint is indeed accelrated.*/
    correct_flags |= BCM_OAM_LOSS_SINGLE_ENDED | BCM_OAM_LOSS_STATISTICS_EXTENDED | BCM_OAM_LOSS_UPDATE | (BCM_OAM_LOSS_SLM * SOC_IS_JERICHO(unit)) ;



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

    SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(&mep_db_entry);
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loss_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loss_ptr->id));
    }
    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)) {
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

	rv = _manage_DA_with_existing_entries_loss(unit,loss_ptr->id, &classifier_mep_entry,msb,lsb,
											   &eth1731_profile, loss_ptr->flags &BCM_OAM_LOSS_UPDATE, &is_ok );
	BCMDNX_IF_ERR_EXIT(rv);
    if (!is_ok) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MAC adddress may not be changed")));
    }

    mep_db_entry.entry_type = (loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED)? SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM_STAT : SOC_PPD_OAM_LM_DM_ENTRY_TYPE_LM ;
    mep_db_entry.mep_id = loss_ptr->id;
	mep_db_entry.is_update = ((loss_ptr->flags & BCM_OAM_LOSS_UPDATE) == BCM_OAM_LOSS_UPDATE);

    rv = _bcm_oam_lm_dm_add_shared(unit,&mep_db_entry);
    BCMDNX_IF_ERR_EXIT(rv);


    /*Have the classifier send LM packets to the OAMP.*/
    rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 1, &(action.destination), &dont_care); 
    SET_CLASSIFIER_ACTION(mep_db_entry.mep_id,action,1 /*is_lm */, (loss_ptr->flags &BCM_OAM_LOSS_SINGLE_ENDED)==0 , classifier_mep_entry.mep_type)    ;

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

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loss_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loss_ptr->id));
    }

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

	/* Non statistics*/
	loss_ptr->flags = 0;
    rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, loss_ptr->id, &dont_care, &eth1731_profile_data);
    BCMDNX_IF_ERR_EXIT(rv); 
    if (!eth1731_profile_data.piggy_back_lm) {
		loss_ptr->flags = BCM_OAM_LOSS_SINGLE_ENDED;
		/* Get the rate and maybe the DA address */
        SOC_PPD_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, eth1731_profile_data.lmm_rate);
        loss_ptr->period = ccm_period_ms;
        if (classifier_mep_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_ETH_OAM ) {
			_BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(loss_ptr,loss_ptr->id );
        }
        loss_ptr->flags |= BCM_OAM_LOSS_SLM * eth1731_profile_data.slm_lm;
    }


	soc_sand_rv = soc_ppd_oam_oamp_next_index_get(unit, loss_ptr->id, &entry, &has_dm);
	BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
	    num_entries = entry - loss_ptr->id -1; 

    if ( (num_entries==2 && !has_dm)  || num_entries==3 ) {
		loss_ptr->flags |= BCM_OAM_LOSS_STATISTICS_EXTENDED;
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

	if (!SOC_IS_ARADPLUS(unit)) {
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
    int rv;
    bcm_oam_endpoint_action_t action;
    int dont_care; 
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY mep_db_entry;
    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile;
	uint32 correct_flags=0,lsb,msb;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    } 

	if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

    /*First, verify that the parameters are correct and that the endpoint is indeed accelrated.*/

    correct_flags |= BCM_OAM_DELAY_ONE_WAY | BCM_OAM_DELAY_UPDATE;

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
    SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(&mep_db_entry);
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry)  ;
    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, delay_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), delay_ptr->id));
    }
    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), delay_ptr->id));
    }

    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not defined as 1588"), delay_ptr->id));
    }

    if (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) {
		/* 1 dm requires an OAMP_PE program in ARAD+, make sure it's available */
		rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_pe_use_1dm_check, (unit,delay_ptr->id));
		BCMDNX_IF_ERR_EXIT(rv);
    }

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "passed initial tests")));

	/* First, the troika of profiles.*/
    lsb =  GET_LSB_FROM_MAC_ADDR(delay_ptr->peer_da_mac_address);
    msb =  GET_MSB_FROM_MAC_ADDR(delay_ptr->peer_da_mac_address); 
    SOC_PPD_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(delay_ptr->period, 0 , eth1731_profile.dmm_rate);
	rv = _manage_DA_with_existing_entries_delay(unit,delay_ptr->id, &classifier_mep_entry,msb,lsb,
											   &eth1731_profile, delay_ptr->flags &BCM_OAM_DELAY_UPDATE, &is_ok );
	BCMDNX_IF_ERR_EXIT(rv);
    if (!is_ok) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MAC adddress may not be changed")));
    }


    if (delay_ptr->period) {
        /*filling the mep_db_entry and having the shared function do its thing*/
        mep_db_entry.entry_type = SOC_PPD_OAM_LM_DM_ENTRY_TYPE_DM ;
        mep_db_entry.mep_id = delay_ptr->id;

        mep_db_entry.is_1DM =  (delay_ptr->flags& BCM_OAM_DELAY_ONE_WAY) / BCM_OAM_DELAY_ONE_WAY; /*1 iff the flag is set.*/

        mep_db_entry.is_update = ((delay_ptr->flags & BCM_OAM_DELAY_UPDATE) == BCM_OAM_DELAY_UPDATE);

        rv = _bcm_oam_lm_dm_add_shared(unit,&mep_db_entry );
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        uint8 is_1dm = (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) >0;
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
			} else {
				rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_1dm_set, (unit,delay_ptr->id,is_1dm));
				BCMDNX_IF_ERR_EXIT(rv);
			}
        } else {
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_1dm_set, (unit,delay_ptr->id, is_1dm));
            BCMDNX_IF_ERR_EXIT(rv); 
		}
        /* on demand DM */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_dm_trigger_set, (unit,delay_ptr->id));
        BCMDNX_IF_ERR_EXIT(rv);

    }

    /*Have the classifier send DM packets to the OAMP.*/
    rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 1, &(action.destination), &dont_care);
    SET_CLASSIFIER_ACTION(delay_ptr->id,action,0 /*is_lm */, delay_ptr->flags& BCM_OAM_DELAY_ONE_WAY,classifier_mep_entry.mep_type );    

    if (delay_ptr->period==0) {
        /* Update the classifier mep entry (The flags may have changed)*/
        uint8 only_on_demand = (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE) !=0;
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, delay_ptr->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), delay_ptr->id));
        }
        rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, delay_ptr->id);
        BCMDNX_IF_ERR_EXIT(rv);
        
        if (only_on_demand) {
			classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE; 
        } else {
			classifier_mep_entry.flags &= ~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;
		}


        rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, delay_ptr->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }


exit:
    BCMDNX_FUNC_RETURN; 
}

int bcm_petra_oam_delay_get(int unit, bcm_oam_delay_t *delay_ptr) {
    uint8 oam_is_init, found, is_1dm;
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

    if (eth1731_profile_data.dmm_rate) {
		dm_info.entry_id = delay_ptr->id; 
		soc_sand_rv = soc_ppd_oam_oamp_dm_get(unit, &dm_info, &is_1dm );
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

		COMPILER_64_SET(delay_ptr->delay.seconds,0,  dm_info.last_delay_second );
		delay_ptr->delay.nanoseconds = dm_info.last_delay_sub_seconds;

		COMPILER_64_SET(delay_ptr->delay_max.seconds,0,  dm_info.max_delay_second );
		delay_ptr->delay_max.nanoseconds = dm_info.max_delay_sub_seconds;


		COMPILER_64_SET(delay_ptr->delay_min.seconds,0,  dm_info.min_delay_second );
		delay_ptr->delay_min.nanoseconds = dm_info.min_delay_sub_seconds;

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
		delay_ptr->flags = BCM_OAM_DELAY_ONE_WAY;
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

	if (!SOC_IS_ARADPLUS(unit)) {
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

/* Add an OAM loopback object */
int bcm_petra_oam_loopback_add(int unit, bcm_oam_loopback_t *loopback_ptr) {
	uint8 oam_is_init,found;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 is_added,  is_ok;
	int dont_care;
	bcm_oam_endpoint_action_t action;
    int rv=0;
    uint32 soc_sand_rv;
	uint32 msb_to_oui;
	uint32 lsb_to_nic;
	uint8 new_flags=0, remove_flags = 0;

	SOC_SAND_PP_MAC_ADDRESS dest_addr;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    }
    
    if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
    }
    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), loopback_ptr->id));
    }


    if (loopback_ptr->tx_count || loopback_ptr->rx_count || loopback_ptr->drop_count|| loopback_ptr->invalid_tlv_offset || loopback_ptr->invalid_mep_tlv_subtype || loopback_ptr->invalid_target_mep_tlv
		|| loopback_ptr->remote_mipid_missmatch || loopback_ptr->out_of_sequence || loopback_ptr->unexpected_response) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid fields set")));
    }

    if ( loopback_ptr->flags & ~BCM_OAM_LOOPBACK_UPDATE ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flag set")));
    }

	/* Done with initial tests.*/


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
	rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 1, &(action.destination), &dont_care); 
	BCM_OAM_OPCODE_CLEAR_ALL(action);
	BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBM);
	SET_CLASSIFIER_ACTION_GENERAL(loopback_ptr->id, action,SOC_PPD_OAM_OPCODE_MAP_LBM ,classifier_mep_entry.mep_type );

	if (loopback_ptr->period != 0) { /* Use LBM functionality as well*/
		is_added =  (loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE) &&  
            (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK); 
        /* in the case of loopback update, the state of the HW is to be ignored only if the current endpoint previously set
           the LB mechanism. Otherwise, it may be the case that update has been called with the intention of adding the LB mechanism
           on top of the LBR mechanism. In this case, setting the LB registyers is allowed only if no other endpoint has set the LB registers.*/
		_bcm_petra_mac_to_sand_mac(loopback_ptr->peer_da_mac_address, &dest_addr);
		soc_sand_rv = soc_ppd_oam_oamp_loopback_set(unit,loopback_ptr->id, loopback_ptr->period,&dest_addr,&is_added);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
		if (is_added ==0) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Only one Loopback session allowed.")));
		}

		/* Have the classifier redirect LBRs to the OAMP.*/    
		BCM_OAM_OPCODE_SET(action,SOC_PPD_OAM_ETHERNET_PDU_OPCODE_LBR);
		SET_CLASSIFIER_ACTION_GENERAL(loopback_ptr->id, action,SOC_PPD_OAM_OPCODE_MAP_LBM ,classifier_mep_entry.mep_type );

		new_flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK;
	} else  {
        if (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK) {
            /* This is a special case of UPDATE where the loopback mechanism is to be removed while the LBR mechanism is
               to remain.*/
            rv = _remove_loopback(unit,loopback_ptr->id, classifier_mep_entry.mep_type);
            BCMDNX_IF_ERR_EXIT(rv);
            remove_flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK;
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
    rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, loopback_ptr->id);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, loopback_ptr->id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}

/* Get an OAM loopback object */
int bcm_petra_oam_loopback_get(int unit, bcm_oam_loopback_t *loopback_ptr) {
    int rv=0;
	uint8 oam_is_init,found;
	SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint32 soc_sand_rv, dont_care;
	uint32 period;
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }
	_BCM_OAM_ENABLED_GET(oam_is_init);
	if (!oam_is_init) {
		BCM_EXIT;
	}

	rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
	BCMDNX_IF_ERR_EXIT(rv);
	if (!found) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
	}

	if ( (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) ==0 ){
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Loopback session not associated with endpoint %d."), loopback_ptr->id));
	}

    if (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK) {
		soc_sand_rv = soc_ppd_oam_oamp_loopback_get(unit,&loopback_ptr->rx_count,&loopback_ptr->tx_count,
													&loopback_ptr->drop_count,&dont_care);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        soc_sand_rv = soc_ppd_oam_oamp_loopback_get_period(unit, &period);
        loopback_ptr->period = period; 
    }

	/* Now the DA address and the period (when applicable)*/
    if (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {
        _BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(loopback_ptr, loopback_ptr->id);
    }

    BCM_EXIT;
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

    if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

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

	rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, loopback_ptr->id);
	BCMDNX_IF_ERR_EXIT(rv);
	rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, loopback_ptr->id, &classifier_mep_entry);
	BCMDNX_IF_ERR_EXIT(rv); 
	

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

    if (!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)) {
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

    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_delete(unit, *used_ais_id, 0 /* dealloc icc index */, 0 /*is last*/);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, *used_ais_id);
    BCMDNX_IF_ERR_EXIT(rv); 

	rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_delete_by_id(unit, ais_ptr->id);
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

int bcm_oam_endpoint_info_diag_print(int unit, bcm_oam_endpoint_info_t *endpoint_info) {
    endpoint_list_t *rmep_list_p = NULL;
    endpoint_list_member_t *cur_member = NULL;
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


    if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tMaintenence entity is intermediate\n")));
    } else if (endpoint_info->type == bcmOAMEndpointTypeEthernet ) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tEndpoint direction: ")));
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "up facing\n")));
        } else {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "down facing\n")));
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
                         "\n\tEndpoint level: %d"), endpoint_info->level));

    if (endpoint_info->lm_flags & BCM_OAM_LM_PCP) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "\tAn 8-tuple of LM counters are set. ")));
    }
    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\n\tLoss management counter base id: %d\n"), endpoint_info->lm_counter_base_id));



    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\tGport 0X%x"), endpoint_info->gport));
    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "\tDestination mac address: " MAC_PRINT_FMT),
              MAC_PRINT_ARG(endpoint_info->dst_mac_address)));

    if (endpoint_info->opcode_flags & BCM_OAM_OPCODE_CCM_IN_HW) {
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
    if (found) {
        cur_member = rmep_list_p->first_member;
        while (cur_member) {
            bcm_oam_endpoint_info_t_init(&rmep_info);
            rmep_info.id = cur_member->index;
            bcm_oam_endpoint_get(unit, rmep_info.id, &rmep_info);
            bcm_oam_remote_endpoint_print(unit, &rmep_info, endpoint_info);

            cur_member = cur_member->next;
        }
    } else {
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
_bcm_oam_diag_iterate_cb(int unit, shr_htb_key_t key, shr_htb_data_t user_data) {
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
    SOC_TMC_CNT_MODE_EG_TYPE dont_care; 
 	SOC_TMC_CNT_SRC_TYPE src_type;

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

 
	for (soc_port =0; soc_port< SOC_DPP_DEFS_GET(unit, nof_counter_processors) ; ++soc_port) {
		rv = _soc_dpp_str_prop_parse_counter_source(unit, soc_port, &src_type, &dont_care); 
		BCMDNX_IF_ERR_EXIT(rv);
        if (src_type == SOC_TMC_CNT_SRC_TYPE_OAM_ING_A) {
			/*soc_port is an egress oam port*/
			egress_index = soc_port;
        } else if (src_type == SOC_TMC_CNT_SRC_TYPE_OAM_ING_C) {
			/*soc_port is an ingress oam port*/
			ingress_index = soc_port;
        }
    }

	if ( (ingress_index == -1) || (egress_index == -1)) {
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
