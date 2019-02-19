/*
 * $Id: bfd.c,v 1.58 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_BCM_BFD

#include <shared/bsl.h>
#include <shared/swstate/sw_state_access.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/bfd.h>
#include <bcm_int/dpp/bfd_uc.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>

#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/bfd.h>

#include <shared/gport.h>
#include <shared/shr_resmgr.h>

#include <soc/drv.h>
#include <soc/enet.h>
#include <soc/defs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/ARAD/arad_ports.h>

/***************************************************************/
/***************************************************************/

/*
 * Global defines
 *
 */

/* support wormboot (used only in init and deinit functions where _bcm_dpp_bfd_trap_info_wb_add is used) */
SOC_PPD_BFD_INIT_TRAP_INFO _bcm_dpp_bfd_trap_info[SOC_MAX_NUM_DEVICES];

/* No wormboot - reconstructed using other resources */
endpoint_list_t _bcm_bfd_endpoint_list[BCM_MAX_NUM_UNITS];

/***************************************************************/
/***************************************************************/

/*
 * Define validation
 *
 */

#if BCM_BFD_ENDPOINT_DEFAULT0 != BCM_OAM_ENDPOINT_DEFAULT_INGRESS0
#error "BFD and OAM default endpoint IDs must be compatible"
#endif
#if BCM_BFD_ENDPOINT_DEFAULT1 != BCM_OAM_ENDPOINT_DEFAULT_INGRESS1
#error "BFD and OAM default endpoint IDs must be compatible"
#endif
#if BCM_BFD_ENDPOINT_DEFAULT2 != BCM_OAM_ENDPOINT_DEFAULT_INGRESS2
#error "BFD and OAM default endpoint IDs must be compatible"
#endif
#if BCM_BFD_ENDPOINT_DEFAULT3 != BCM_OAM_ENDPOINT_DEFAULT_INGRESS3
#error "BFD and OAM default endpoint IDs must be compatible"
#endif

/***************************************************************/
/***************************************************************/

/*
 * Local defines
 *
 */

#define _BCM_BFD_UDP_SRC_PORT_MIN (49152)
#define _BCM_BFD_UDP_SRC_PORT_MAX (65535)

#define _BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(type) ((type == bcmBFDTunnelTypeUdp) || (SOC_IS_JERICHO(unit) &&  type==bcmBFDTunnelTypeMpls))

#define _BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(endpoint_info)  ( ((endpoint_info).flags & BCM_BFD_ENDPOINT_IN_HW) && \
             (((endpoint_info).remote_gport == BCM_GPORT_INVALID) || ((endpoint_info).flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET)) )

/* Macros used by diagnostics functions */
#define IPV4_PRINT_FMT "%u.%u.%u.%u"
#define IPV4_PRINT_ARG(addr) ((uint8*)&addr)[0], ((uint8*)&addr)[1],((uint8*)&addr)[2],((uint8*)&addr)[3]

/* Represents the maximum waiting time in miliseconds for a bfd endpoint to receive a message before assuming it's peer is dead */
#define _BCM_BFD_MAX_LIFETIME_IN_MS 2000000




/**
 * There is a 1 to 1 mapping between 3 bit flag profile indexes 
 * the the 6 bit content of each profile. The mapping is as 
 * such:  (P,F,D are 0/1)
 *  PF00D0 |---> PFD
 *  PFD    |--->  PF00D0
 *  
 *  In addition the flags PFD, may not be used together, so the
 *  addtional condition is
 *  111 |---> 000000
 *  110010 |---> 0
 *  
 *  Likewise the profie 0 is reserved for unsupported flag
 *  combinations. Thus profile_index 0 returns -1.
 *  Profiles 6,7 are used for Flags combination 0
 *  
 *  All input is assumed to be uint32/8.
 */
#define _BCM_BFD_FLAGS_PROFILE_INDEX_TO_FLAGS(profile_index, flags) \
	flags = (profile_index ==7)? 0 : (profile_index==0)? -1 : \
	 ((profile_index & 0x1) << 1 ) | ((profile_index & 0x2) << 3) | ((profile_index & 0x4) << 3)



#define _BCM_BFD_FLAGS_PROFILE_INDEX_FROM_FLAGS(profile_index, flags) \
	profile_index = (flags==0)? 7:  ((flags & 0x2) >>1) | ((flags & 0x10) >>3) | ((flags & 0x20) >>3)


/**
 * Only the flags P, F, D are supported. 
 * P, F, D may not be used together. 
 */
#define _BCM_BFD_FLAG_COMBINATION_IS_NOT_SUPPORTED(flags) \
	(flags & 0xd  ||  (flags == 0x32 ))



/**
 * The following macros are used to translate the RMEP_STATE
 * field in the RMEP DB to/from the remote state, remote diag. 
 * The mapping is 
 * rmep_state={FlagsProfile[2:0],sta[1:0],DiagProfile[3:0]}.
 */
#define _BCM_BFD_RMEP_STATE_TO_REMOTE_STATE_DIAG_AND_FLAGS(rmep_state, remote_state, remote_diag, flags) \
	do {\
	uint32 flags_prof_indx = (rmep_state & 0x1c0) >> 6; \
	remote_diag = rmep_state & 0xf;\
	remote_state = (rmep_state >>4) & 0x3;\
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0) && flags_prof_indx ) {\
        int rv = _bcm_dpp_am_template_bfd_flags_profile_data_get(unit,flags_prof_indx, &flags);\
            BCMDNX_IF_ERR_EXIT(rv);\
    } else {\
        _BCM_BFD_FLAGS_PROFILE_INDEX_TO_FLAGS(flags_prof_indx, flags);\
    }\
} while (0)


#define _BCM_BFD_RMEP_STATE_FROM_REMOTE_STATE_DIAG_AND_FLAGS(rmep_state, remote_state, remote_diag, flags)\
	do {\
		uint32 flags_prof_indx;\
       if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0)) {\
            /* remote flags need a profile as well*/\
            int flags_profile, is_allocated;\
            int rv = _bcm_dpp_am_template_bfd_flags_profile_alloc(unit, 0,&flags, &is_allocated, &flags_profile);\
            BCMDNX_IF_ERR_EXIT(rv);\
            flags_prof_indx = flags_profile;\
            if (is_allocated) {\
                uint32 soc_sand_rv = soc_ppd_oam_bfd_flags_profile_set(unit, flags_profile, flags);\
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);\
            }     \
        } else {\
		_BCM_BFD_FLAGS_PROFILE_INDEX_FROM_FLAGS(flags_prof_indx, (uint32)flags);\
        }\
        rmep_state = (flags_prof_indx <<6)  | ( (3 &remote_state) <<4 ) | (remote_diag & 0xf );\
	} while (0)


#define _BCM_BFD_IS_MEP_ID_DEFAULT(_unit, _epid) \
                (SOC_IS_ARAD_B1_AND_BELOW(_unit)\
                     ? ((_epid) == -1)\
                     : (((_epid) == BCM_BFD_ENDPOINT_DEFAULT0)\
                        ||((_epid) == BCM_BFD_ENDPOINT_DEFAULT1)\
                        ||((_epid) == BCM_BFD_ENDPOINT_DEFAULT2)\
                        ||((_epid) == BCM_BFD_ENDPOINT_DEFAULT3)))


/**
 * Convert IPv4 (in uint32 format) to IPv6 (in uint8[16] 
 * format.) 
 * result should be 0::FFFF:IPv4-DIP 
 *  
 */
#define _BCM_BFD_IPV4_ADDR_TO_DEFAULT_IPV6_EXTENSION_ADDR(ipv4_ip, ipv6_ip) \
    do {\
          soc_sand_SAND_PP_IPV6_ADDRESS_clear(&ipv6_ip);\
          ipv6_ip.address[0] = ipv4_ip;\
          ipv6_ip.address[1] = 0xffff;\
    } while (0)

/*OAM statistics*/
#define _BCM_BFD_CFM_ETHER_TYPE	(0x8902)

#define BFD_ACCESS                  sw_state_access[unit].dpp.bcm.bfd

/***************************************************************/
/***************************************************************/
/* in OAM statistics per mep functionality the counter Id is inserted into the LEM*/
int _bcm_bfd_stat_lem_entry_add(uint32 unit, bcm_bfd_endpoint_info_t *endpoint_info) {
	int rv;
	ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	ARAD_PP_LEM_ACCESS_PAYLOAD payload;
	BCMDNX_INIT_FUNC_DEFS;
	ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
	ARAD_PP_FP_QUAL_VAL_clear(qual_vals);
	payload.flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
	payload.dest = (uint16)endpoint_info->id;
	qual_vals[0].val.arr[0] = endpoint_info->local_discr;
    qual_vals[0].val.arr[1] = 0;
    qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
    switch (endpoint_info->type) {
    case bcmBFDTunnelTypeUdp:
        rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_STATISTICS, qual_vals, 0, &payload, &success);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmBFDTunnelTypeMpls:
        rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_MPLS_STATISTICS, qual_vals, 0, &payload, &success);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmBFDTunnelTypePweControlWord:
        rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_PWE_STATISTICS, qual_vals, 0, &payload, &success);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type with statistics feature")));
    }
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function manages BFD PDU static registers 
 * In Arad+ the register "BFD PDU Static Register" is used for all endpoint types. 
 * In Arad that register is used for all types except MPLS-TP CC endpoint types, which uses the 
 * "BFD CC Packet Static Register". 
 * All paramters should be self explanatory except is_updating. This should be 1 when endpoint_create() is called with 
 * endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE, 
 * 0 otherwise. 
*/
int _bcm_bfd_manage_static_registers(int unit, bcm_bfd_endpoint_info_t *endpoint_info, uint32 ref_counter, uint8 is_mplsCC, uint8 is_updating) {
    SOC_PPD_BFD_PDU_STATIC_REGISTER bfd_pdu;
    uint32 soc_sand_rv;
    SOC_PPD_BFD_PDU_STATIC_REGISTER *bfd_pdu_to_update;
    SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER bfd_cc_packet;
    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPD_BFD_PDU_STATIC_REGISTER_clear(&bfd_pdu);
    SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER_clear(&bfd_cc_packet);

    bfd_pdu_to_update = (SOC_IS_ARAD_B1_AND_BELOW(unit) && is_mplsCC) ? &(bfd_cc_packet.bfd_static_reg_fields) : &bfd_pdu;

    if (ref_counter == 0 || (ref_counter==1 && is_updating)) {
		/* Set the static registers in one of 2 cases:
		   1. This is the first endpoint (ref count is at 0)
		   2, Only one endpoint is using the static registers (ref count==1) and that endpoint is attempting to update these fields.*/
        if (is_updating && ref_counter==0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG("Something went wrong - internal counters mismanaged.")));
        }

        bfd_pdu_to_update->bfd_req_min_echo_rx_interval = endpoint_info->local_min_echo;
        bfd_pdu_to_update->bfd_vers = SOC_PPD_BFD_PDU_VERSION;
        bfd_pdu_to_update->bfd_length = SOC_PPD_BFD_PDU_LENGTH;
		if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            bfd_pdu_to_update->bfd_sta = endpoint_info->local_state;
            bfd_pdu_to_update->bfd_flags =  endpoint_info->local_flags ;
            bfd_pdu_to_update->bfd_diag = endpoint_info->local_diag;
            if (is_mplsCC) {
                bfd_cc_packet.bfd_your_discr = endpoint_info->remote_discr;
                bfd_cc_packet.bfd_my_discr = endpoint_info->local_discr;
                soc_sand_rv = soc_ppd_oam_bfd_cc_packet_static_register_set(unit, &bfd_cc_packet);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
        if (SOC_IS_ARADPLUS(unit) || !is_mplsCC) {  
            soc_sand_rv = soc_ppd_oam_bfd_pdu_static_register_set(unit, &bfd_pdu);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    } else {

		if (is_mplsCC && SOC_IS_ARAD_B1_AND_BELOW(unit)) {
			soc_sand_rv = soc_ppd_oam_bfd_cc_packet_static_register_get(unit, &bfd_cc_packet);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			if (bfd_cc_packet.bfd_my_discr != endpoint_info->local_discr) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
									(_BSL_BCM_MSG("Error: Supporting single local_discr value for device. Existing value: %d"),
									 bfd_cc_packet.bfd_my_discr));
			}
			if (bfd_cc_packet.bfd_your_discr != endpoint_info->remote_discr) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
									(_BSL_BCM_MSG("Error: Supporting single remote_discr value for device. Existing value: %d"),
									 bfd_cc_packet.bfd_your_discr));
			}
		} else {
			soc_sand_rv = soc_ppd_oam_bfd_pdu_static_register_get(unit, &bfd_pdu); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        if (bfd_pdu_to_update->bfd_req_min_echo_rx_interval != endpoint_info->local_min_echo) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: Supporting single local_min_echo value for device. Existing value: %d"),
                              bfd_pdu_to_update->bfd_req_min_echo_rx_interval));
        }
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            if (bfd_pdu_to_update->bfd_diag != endpoint_info->local_diag) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: Supporting single local_diag value for device. Existing value: %d"),
                                  bfd_pdu_to_update->bfd_diag));
            }
            if (bfd_pdu_to_update->bfd_sta != endpoint_info->local_state) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: Supporting single local_state value for device. Existing value: %d"), bfd_pdu_to_update->bfd_sta));
            }
            if (bfd_pdu_to_update->bfd_flags != (endpoint_info->local_flags)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: Supporting single C,A,D,M value for device. Existing value: C=%d, A=%d, D=%d, M=%d"),
                                  (bfd_pdu_to_update->bfd_flags & 0x1),(bfd_pdu_to_update->bfd_flags & 0x2),(bfd_pdu_to_update->bfd_flags & 0x4),(bfd_pdu_to_update->bfd_flags & 0x8)));

            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * OAM Module Helper functions
 * 
 *
 */

/* Fill RMEP-entry struct */
int _bcm_bfd_rmep_db_entry_struct_set(int unit, bcm_bfd_endpoint_info_t *endpoint_info, SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY *rmep_db_entry) {

    SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA punt_profile_data;
	int is_allocated;
	int profile;
	uint32 soc_sand_rv;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    rmep_db_entry->ccm_period = endpoint_info->local_min_rx * endpoint_info->remote_detect_mult;
    rmep_db_entry->loc_clear_threshold = 0;
	rmep_db_entry->loc_clear_threshold = endpoint_info->loc_clear_threshold;

	if (SOC_IS_ARADPLUS(unit)) {
		SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA_clear(&punt_profile_data);
		punt_profile_data.punt_enable = (endpoint_info->sampling_ratio > 0) ? 1 : 0;
		punt_profile_data.punt_rate = endpoint_info->sampling_ratio;
		punt_profile_data.rx_state_update_enable =   (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
                                                     (endpoint_info->flags & BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE) ? 1 : 
			                                         (endpoint_info->flags & BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE) ? 3 : 2;
		punt_profile_data.scan_state_update_enable = (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
                                                     (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE) ? 1 : 
			                                         (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE ) ? 3 : 2;
		punt_profile_data.mep_rdi_update_loc_enable = (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_ON_LOC);
		punt_profile_data.mep_rdi_update_loc_clear_enable = (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR );
		punt_profile_data.mep_rdi_update_rx_enable = 0;
		rv = _bcm_dpp_am_template_oam_punt_event_hendling_profile_alloc(unit, 0 /*flags*/, &punt_profile_data, &is_allocated, &profile);
		BCMDNX_IF_ERR_EXIT(rv);
		if (is_allocated) {
			soc_sand_rv = soc_ppd_oam_oamp_punt_event_hendling_profile_set(unit, profile, &punt_profile_data);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
		}
		rmep_db_entry->punt_profile = profile;
		_BCM_BFD_RMEP_STATE_FROM_REMOTE_STATE_DIAG_AND_FLAGS(rmep_db_entry->rmep_state, endpoint_info->remote_state, endpoint_info->remote_diag, endpoint_info->remote_flags);
    }
	else {
		rmep_db_entry->is_state_auto_handle = ((endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE) != 0);
		rmep_db_entry->is_event_mask = ((endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE) != 0);
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Read RMEP-entry struct */
int _bcm_bfd_rmep_db_entry_struct_get(int unit, SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY *rmep_db_entry, bcm_bfd_endpoint_info_t *endpoint_info) {
	SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA punt_profile_data;
	uint32 soc_sand_rv;

	BCMDNX_INIT_FUNC_DEFS;

    endpoint_info->loc_clear_threshold = rmep_db_entry->loc_clear_threshold;
    if (endpoint_info->local_min_rx==0) {
        endpoint_info->remote_detect_mult = 0;
    }
    else {
        endpoint_info->remote_detect_mult = SOC_SAND_DIV_ROUND(rmep_db_entry->ccm_period, endpoint_info->local_min_rx);
    }

	if (SOC_IS_ARADPLUS(unit)) {
		SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA_clear(&punt_profile_data);
		soc_sand_rv = soc_ppd_oam_oamp_punt_event_hendling_profile_get(unit, rmep_db_entry->punt_profile, &punt_profile_data);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        endpoint_info->sampling_ratio = punt_profile_data.punt_rate;
        switch (punt_profile_data.rx_state_update_enable) {
        case 0:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE;
            break;
        case 1:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE;
            break;
        case 3:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE;
            break;
        default:
            break;
        }
        switch (punt_profile_data.scan_state_update_enable) {
        case 0:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE;
            break;
        case 1:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE;
            break;
        case 3:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE;
            break;
        default:
            break;
        }
        endpoint_info->flags |= punt_profile_data.mep_rdi_update_loc_enable ? BCM_BFD_ENDPOINT_RDI_ON_LOC : 0;
        endpoint_info->flags |= punt_profile_data.mep_rdi_update_loc_clear_enable ? BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR: 0;
        /*endpoint_info->flags |= punt_profile_data.mep_rdi_update_rx_enable ? 0 : 0; No such option for BFD */

		_BCM_BFD_RMEP_STATE_TO_REMOTE_STATE_DIAG_AND_FLAGS(rmep_db_entry->rmep_state, endpoint_info->remote_state, endpoint_info->remote_diag, endpoint_info->remote_flags);
	}
	else {
		endpoint_info->flags |= rmep_db_entry->is_state_auto_handle ? BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE : 0;
		endpoint_info->flags |= rmep_db_entry->is_event_mask ? BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE : 0;
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Fill MEP-entry struct */
int _bcm_bfd_mep_db_entry_struct_set(int unit, bcm_bfd_endpoint_info_t *endpoint_info, SOC_PPD_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry) {

    int is_allocated;
    uint32 soc_sand_rv;
    int rv;
    uint32 tx_rate;
    int temp_index;
    SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
    uint32 ipv4_multi_hop_acc_ref_counter;
    int priority;
    _bcm_dpp_gport_info_t gport_info;
	int ref_counter=0;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->tx_gport, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    mep_db_entry->system_port = gport_info.sys_port;

    if (_BCM_PETRA_L3_ITF_IS_FEC(endpoint_info->egress_if)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported egress_if.")));
    }
    mep_db_entry->egress_if = _BCM_PETRA_L3_ITF_VAL_GET(endpoint_info->egress_if);
    mep_db_entry->pbit = ((endpoint_info->flags & BCM_BFD_ENDPOINT_MPLS_TP_POLL_SEQUENCE_ENABLE) != 0);
    mep_db_entry->fbit = 0;

    switch (endpoint_info->type) {
    case bcmBFDTunnelTypeMpls:
        mep_db_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS;
        break;
    case bcmBFDTunnelTypeMplsTpCc:
    case bcmBFDTunnelTypePweGal:
        /* PWE -ACH should always be on in these cases.*/
        if (SOC_IS_ARADPLUS(unit)) {
            mep_db_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE; /* In ARAD+ supporting MPLSTPCC through PWE entry in mep DB */
            mep_db_entry->bfd_pwe_ach = 1; 
        }
        else {
            mep_db_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP;
        }
        break;
    case bcmBFDTunnelTypeMplsTpCcCv:
        mep_db_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFDCV_O_MPLSTP;
        break;
	case bcmBFDTunnelTypeUdp:
		if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
			mep_db_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP;
		} else {
			mep_db_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP;
			/*check if we are in bfd ipv4 single hop extended mode*/
			if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop == 1){
				if ((endpoint_info->type == bcmBFDTunnelTypeUdp) && !(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)){
					mep_db_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP;
				}
			}
		}
        break;
    case bcmBFDTunnelTypePweRouterAlert:
    case bcmBFDTunnelTypePweTtl:
        mep_db_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE;
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_ACH ) {
            mep_db_entry->bfd_pwe_ach = 1;
        }
        break; 
    case bcmBFDTunnelTypePweControlWord:
        mep_db_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE;
        mep_db_entry->bfd_pwe_ach = 1;
        break; 
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type.")));
    }

    if ((endpoint_info->type == bcmBFDTunnelTypePweGal || endpoint_info->type == bcmBFDTunnelTypeMplsTpCc) && SOC_IS_JERICHO(unit)) {
        mep_db_entry->bfd_pwe_gal = 1;/* For Arad+ there is a work around.*/
    }

    if (endpoint_info->type == bcmBFDTunnelTypePweRouterAlert) {
        mep_db_entry->bfd_pwe_router_alert = 1;
    }

    mep_db_entry->remote_discr = endpoint_info->remote_discr;
    if ((endpoint_info->type == bcmBFDTunnelTypeUdp) || (endpoint_info->type == bcmBFDTunnelTypeMpls)) {
        rv = _bcm_dpp_am_template_bfd_src_ip_profile_alloc(unit, 0/*flags*/, &(endpoint_info->src_ip_addr), &is_allocated, &temp_index);
        BCMDNX_IF_ERR_EXIT(rv);
        mep_db_entry->src_ip_add_ptr = (uint8)temp_index;
        if (is_allocated) {
            soc_sand_rv = soc_ppd_oam_bfd_ipv4_src_addr_select_set(unit, mep_db_entry->src_ip_add_ptr, endpoint_info->src_ip_addr);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP ||  (endpoint_info->type == bcmBFDTunnelTypeMpls) 
		|| (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop == 1) ) {
            SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA tos_ttl_data;

            SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(&tos_ttl_data);
            tos_ttl_data.tos = endpoint_info->ip_tos;
            tos_ttl_data.ttl = endpoint_info->ip_ttl;
	
			/*if we are in bfd ipv4 single hop extended mode allocate profile 15 
			if it is the first time profile 15 is used we set the tos value*/
			if ((SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop == 1) &&
                (endpoint_info->type == bcmBFDTunnelTypeUdp) && !(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
                is_allocated = 1;
                mep_db_entry->tos_ttl_profile = (uint8)_BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE;

                rv = BFD_ACCESS.s_hop.ref_counter.get(unit, &ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
                if (ref_counter == 0) {
                    int temp = endpoint_info->ip_tos;
                    rv = BFD_ACCESS.s_hop.tos_value.set(unit, temp);
                    BCMDNX_IF_ERR_EXIT(rv);
                } else {
                    int tos = 0;
                    rv = BFD_ACCESS.s_hop.tos_value.get(unit, &tos);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (tos != endpoint_info->ip_tos) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("Error: Supporting single IP TOS value for bcmBFDTunnelTypeUdp for single hop per device. Existing value: %d")
                                             , tos));
                    }
                }
                ref_counter++;
                rv = BFD_ACCESS.s_hop.ref_counter.set(unit, ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
				rv = _bcm_dpp_am_template_bfd_tos_ttl_profile_alloc(unit, 0/*flags*/, &tos_ttl_data, &is_allocated, &temp_index);
				BCMDNX_IF_ERR_EXIT(rv);
				mep_db_entry->tos_ttl_profile = (uint8)temp_index;
			}
            
            if (is_allocated) {
                soc_sand_rv = soc_ppd_oam_bfd_ipv4_tos_ttl_select_set(unit, mep_db_entry->tos_ttl_profile, &tos_ttl_data);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        } else if (endpoint_info->type == bcmBFDTunnelTypeUdp) { /* ONE HOP IPV4 */
            mep_db_entry->tunnel_is_mpls = (endpoint_info->ip_ttl == 1);
            mep_db_entry->ip_subnet_len =  endpoint_info->ip_subnet_length==32 ? 31 : endpoint_info->ip_subnet_length  ; /* value 31 in the HW indicates full check*/
        }
        /* Will be ignored by MPLS entries.*/
        mep_db_entry->dst_ip_add = endpoint_info->dst_ip_addr * ( SOC_IS_JERICHO(unit) || mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP);

    }
    mep_db_entry->local_detect_mult = endpoint_info->local_detect_mult;
    tx_rate = SOC_SAND_DIV_ROUND(endpoint_info->bfd_period * 100, 333); /* Tx Rate is in 3.33 ms units */
    rv = _bcm_dpp_am_template_bfd_tx_rate_profile_alloc(unit, 0/*flags*/, &tx_rate, &is_allocated, &temp_index);
    BCMDNX_IF_ERR_EXIT(rv);
    mep_db_entry->ccm_interval = (uint8)temp_index;
    if (is_allocated) {
        soc_sand_rv = soc_ppd_oam_bfd_tx_rate_set(unit, mep_db_entry->ccm_interval, tx_rate);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    rv = _bcm_dpp_am_template_bfd_req_interval_pointer_alloc(unit, 0/*flags*/, &(endpoint_info->local_min_rx), &is_allocated, &temp_index);
    BCMDNX_IF_ERR_EXIT(rv);
    mep_db_entry->min_rx_interval_ptr = (uint8)temp_index;
    if (is_allocated) {
        soc_sand_rv = soc_ppd_oam_bfd_req_interval_pointer_set(unit, mep_db_entry->min_rx_interval_ptr, endpoint_info->local_min_rx);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    rv = _bcm_dpp_am_template_bfd_req_interval_pointer_alloc(unit, 0/*flags*/, &(endpoint_info->local_min_tx), &is_allocated, &temp_index);
    BCMDNX_IF_ERR_EXIT(rv);
    mep_db_entry->min_tx_interval_ptr = (uint8)temp_index;
    if (is_allocated) {
        soc_sand_rv = soc_ppd_oam_bfd_req_interval_pointer_set(unit, mep_db_entry->min_tx_interval_ptr, endpoint_info->local_min_tx);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    if ((endpoint_info->type != bcmBFDTunnelTypeUdp)) {
        SOC_PPD_MPLS_PWE_PROFILE_DATA push_data;

        SOC_PPD_MPLS_PWE_PROFILE_DATA_clear(&push_data);
        /* In ARAD+ supporting MPLSTPCC through PWE entry in mep DB  */
        if (((endpoint_info->type == bcmBFDTunnelTypeMplsTpCc) ||  (endpoint_info->type == bcmBFDTunnelTypePweGal)) && SOC_IS_ARADPLUS_A0(unit)) {
            mep_db_entry->label =  SOC_PPD_MPLS_TERM_RESERVED_LABEL_GAL;
        }
        else {
            mep_db_entry->label =  endpoint_info->egress_label.label;
        }
        push_data.ttl = endpoint_info->egress_label.ttl;
        push_data.exp = endpoint_info->egress_label.exp;
        rv = _bcm_dpp_am_template_mpls_pwe_push_profile_alloc(unit, 0/*flags*/, &push_data, &is_allocated, &temp_index);
        BCMDNX_IF_ERR_EXIT(rv);
        mep_db_entry->push_profile = (uint8)temp_index;
        if (is_allocated) {
            soc_sand_rv = soc_ppd_oam_mpls_pwe_profile_set(unit, mep_db_entry->push_profile, &push_data);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    /* ITMH attributes profile allocate */
    SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
    /* int_pri:  ((COS & 0x7) << 2) + (DP & 0x3)) */
    tx_itmh_attributes.tc = ((uint32) (endpoint_info->int_pri & 0x1F)) >> 2;
    tx_itmh_attributes.dp = ((uint32) endpoint_info->int_pri) & 0x3;
    if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
        rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_MULTI_HOP_ACC_REF_COUNTER, &ipv4_multi_hop_acc_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
        if (ipv4_multi_hop_acc_ref_counter == 0) {
            soc_sand_rv = soc_ppd_oam_bfd_tx_ipv4_multi_hop_set(unit, &tx_itmh_attributes);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            soc_sand_rv = soc_ppd_oam_bfd_tx_ipv4_multi_hop_get(unit, &tx_itmh_attributes);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (tx_itmh_attributes.tc != (  ((uint32) endpoint_info->int_pri & 0x1F) >> 2) || (tx_itmh_attributes.dp != (((uint32)endpoint_info->int_pri) & 0x3 ))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: Supporting single int_pri value for bcmBFDTunnelTypeUdp multihop. Existing value: %d"),
                                  (((tx_itmh_attributes.tc & 0x7) << 2) + (tx_itmh_attributes.dp & 0x3))));
            }
        }
        ipv4_multi_hop_acc_ref_counter++;
        rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_MULTI_HOP_ACC_REF_COUNTER, &ipv4_multi_hop_acc_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = _bcm_dpp_am_template_oam_tx_priority_alloc(unit, 0/*flags*/, &tx_itmh_attributes, &is_allocated, &priority);
        BCMDNX_IF_ERR_EXIT(rv);
        mep_db_entry->priority = (uint8)priority;
        if (is_allocated) {
            soc_sand_rv = soc_ppd_oam_oamp_tx_priority_registers_set(unit, mep_db_entry->priority, &tx_itmh_attributes);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    if (SOC_IS_ARADPLUS(unit)) {
        mep_db_entry->sta = endpoint_info->local_state;
        mep_db_entry->diag_profile = endpoint_info->local_diag;
		if (endpoint_info->flags & BCM_BFD_ECHO) {
			mep_db_entry->diag_profile = 0xf;
		}
		if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0)) {
            int flags_profile;
            rv = _bcm_dpp_am_template_bfd_flags_profile_alloc(unit, 0/*flags*/, &(endpoint_info->local_flags), &is_allocated, &flags_profile);
            BCMDNX_IF_ERR_EXIT(rv);
            mep_db_entry->flags_profile = (uint8)flags_profile;
            if (is_allocated) {
                soc_sand_rv = soc_ppd_oam_bfd_flags_profile_set(unit, mep_db_entry->flags_profile, endpoint_info->local_flags);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        } else {
            _BCM_BFD_FLAGS_PROFILE_INDEX_FROM_FLAGS(mep_db_entry->flags_profile, endpoint_info->local_flags);
        }
    }

    if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
        int local_port, ignored;

        rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, 0, &(mep_db_entry->system_port), &ignored, &local_port);
        BCMDNX_IF_ERR_EXIT(rv);
        mep_db_entry->local_port = local_port; 
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Read MEP-entry struct */
int _bcm_bfd_mep_db_entry_struct_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info, SOC_PPD_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry) {
    int rv;
    SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA tos_ttl_data;
    SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
    uint32 soc_sand_rv;
    SOC_PPD_MPLS_PWE_PROFILE_DATA push_data;
    uint32 tx_rate;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, mep_db_entry->system_port);
    /* we use _bcm_dpp_gport_to_phy_port for set. Is it OK?
    ret = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->gport, 0, &pbmp_local_ports,
                                     &(mep_db_entry->system_port), &is_local_ports, &is_lag);*/

    if (mep_db_entry->egress_if < (SOC_DPP_CONFIG(unit))->l3.nof_rifs) {
        _BCM_PETRA_L3_ITF_SET(endpoint_info->egress_if, _BCM_PETRA_L3_ITF_RIF, mep_db_entry->egress_if);
    } else {
        _BCM_PETRA_L3_ITF_SET(endpoint_info->egress_if, _BCM_PETRA_L3_ITF_ENCAP, mep_db_entry->egress_if);
    }
    endpoint_info->flags |= mep_db_entry->pbit ? BCM_BFD_ENDPOINT_MPLS_TP_POLL_SEQUENCE_ENABLE : 0;
    switch (mep_db_entry->mep_type) {
    case SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP:
        endpoint_info->type = bcmBFDTunnelTypeMplsTpCc;
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFDCV_O_MPLSTP:
        endpoint_info->type = bcmBFDTunnelTypeMplsTpCcCv;
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS:
	case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:	

		/* check if we are in bfd ipv4 single hop*/
		if ((SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop == 1)&&
		mep_db_entry->tos_ttl_profile==_BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE){
			endpoint_info->type = bcmBFDTunnelTypeUdp ;
            endpoint_info->dst_ip_addr = mep_db_entry->dst_ip_add;
		}else{
			if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
            endpoint_info->type = bcmBFDTunnelTypeUdp ;
            endpoint_info->flags |= BCM_BFD_ENDPOINT_MULTIHOP;
            endpoint_info->dst_ip_addr = mep_db_entry->dst_ip_add;
			} else {
				endpoint_info->type = bcmBFDTunnelTypeMpls ;
			}
		}

		if (mep_db_entry->tos_ttl_profile == _BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE) {
			int temp;
			rv = BFD_ACCESS.s_hop.tos_value.get(unit, &temp);
			BCMDNX_IF_ERR_EXIT(rv);
			tos_ttl_data.ttl =255;
			tos_ttl_data.tos =temp;

		} else {
			SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(&tos_ttl_data); 
			rv = _bcm_dpp_am_template_bfd_tos_ttl_profile_data_get(unit, mep_db_entry->tos_ttl_profile, &tos_ttl_data);
			BCMDNX_IF_ERR_EXIT(rv);
		}
		
        endpoint_info->ip_tos = tos_ttl_data.tos;
        endpoint_info->ip_ttl = tos_ttl_data.ttl;
         
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
        endpoint_info->type = bcmBFDTunnelTypeUdp;
        endpoint_info->ip_ttl = mep_db_entry->tunnel_is_mpls ? 1 : 255;
        /* Following fields should be non zero only for Jericho*/
        endpoint_info->dst_ip_addr = mep_db_entry->dst_ip_add; 
        endpoint_info->ip_subnet_length = mep_db_entry->ip_subnet_len ==31? 32 : mep_db_entry->ip_subnet_len;  /* value 31 in the HW indicates full check*/
        break;
    case SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE:
        SOC_PPD_MPLS_PWE_PROFILE_DATA_clear(&push_data);
        rv = _bcm_dpp_am_template_mpls_pwe_push_profile_data_get(unit, mep_db_entry->push_profile, &push_data);
        BCMDNX_IF_ERR_EXIT(rv);

        if ( (SOC_IS_ARADPLUS(unit) && (mep_db_entry->label == SOC_PPD_MPLS_TERM_RESERVED_LABEL_GAL)) || (mep_db_entry->bfd_pwe_gal && SOC_IS_JERICHO(unit))) {
            endpoint_info->type = bcmBFDTunnelTypeMplsTpCc; /* In ARAD+ supporting MPLSTPCC through PWE entry in mep DB */
            /* This endpoint can also be of type bcmBFDTunnelTypePweGal, but the type field is rewritten in the classifier info get anyway */
        }
        else if (mep_db_entry->bfd_pwe_router_alert == 1) {
            endpoint_info->type = bcmBFDTunnelTypePweRouterAlert;
        }
        else if (push_data.ttl==1) {
            endpoint_info->type = bcmBFDTunnelTypePweTtl;
        }
        else {
            endpoint_info->type = bcmBFDTunnelTypePweControlWord;
        }

        if (mep_db_entry->bfd_pwe_ach == 1 && endpoint_info->type==bcmBFDTunnelTypePweRouterAlert ) {
            /* PWE ACH is optional only for RA types. This bit in the MEP DB must be set for all other types.*/
            endpoint_info->flags |= BCM_BFD_ENDPOINT_PWE_ACH;
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error: Error in mep_db_entry struct.")));
    }

    endpoint_info->remote_discr = mep_db_entry->remote_discr;
    if ((endpoint_info->type == bcmBFDTunnelTypeUdp) || (endpoint_info->type == bcmBFDTunnelTypeMpls)) {
        rv = _bcm_dpp_am_template_bfd_src_ip_profile_data_get(unit, mep_db_entry->src_ip_add_ptr, &(endpoint_info->src_ip_addr));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    endpoint_info->local_detect_mult = mep_db_entry->local_detect_mult;
    rv = _bcm_dpp_am_template_bfd_req_interval_pointer_data_get(unit, mep_db_entry->min_rx_interval_ptr, &(endpoint_info->local_min_rx));
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_am_template_bfd_req_interval_pointer_data_get(unit, mep_db_entry->min_tx_interval_ptr, &(endpoint_info->local_min_tx));
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_am_template_bfd_tx_rate_profile_data_get(unit, (int)mep_db_entry->ccm_interval, &tx_rate);
    BCMDNX_IF_ERR_EXIT(rv);
    endpoint_info->bfd_period = SOC_SAND_DIV_ROUND((tx_rate*333), 100); /* Tx Rate is in 3.33 ms units */

    if ((endpoint_info->type != bcmBFDTunnelTypeUdp)) {
        endpoint_info->egress_label.label = mep_db_entry->label;
        SOC_PPD_MPLS_PWE_PROFILE_DATA_clear(&push_data);
        rv = _bcm_dpp_am_template_mpls_pwe_push_profile_data_get(unit, mep_db_entry->push_profile, &push_data);
        BCMDNX_IF_ERR_EXIT(rv);
        endpoint_info->egress_label.ttl = push_data.ttl;
        endpoint_info->egress_label.exp = push_data.exp;
    }
    else {
        endpoint_info->egress_label.exp = 0xFF; /* default */
    }

    /* ITMH attributes profile get */
    SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
    if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
        soc_sand_rv = soc_ppd_oam_bfd_tx_ipv4_multi_hop_get(unit, &tx_itmh_attributes);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        rv = _bcm_dpp_am_template_oam_tx_priority_data_get(unit, mep_db_entry->priority, &tx_itmh_attributes);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    /* int_pri:  ((COS & 0x7) << 2) + (DP & 0x3)) */
    endpoint_info->int_pri = ((tx_itmh_attributes.tc & 0x7) << 2) + (tx_itmh_attributes.dp & 0x3);

    if (SOC_IS_ARADPLUS(unit)) {
        endpoint_info->local_state = mep_db_entry->sta;
		endpoint_info->local_diag = mep_db_entry->diag_profile;
        if ( soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0) ) {
            rv = _bcm_dpp_am_template_bfd_flags_profile_data_get(unit, mep_db_entry->flags_profile, &endpoint_info->local_flags);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            _BCM_BFD_FLAGS_PROFILE_INDEX_TO_FLAGS(mep_db_entry->flags_profile, endpoint_info->local_flags);
        }
		if (endpoint_info->local_diag == 0xf) {
			endpoint_info->flags |= BCM_BFD_ECHO;
			endpoint_info->local_diag = 0;
		}
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Free all MEP-entry struct relevant info */
int _bcm_mep_db_entry_dealloc(int unit, SOC_PPD_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry) {
    int is_last;
    uint32 soc_sand_rv;
    int rv;
    SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
    uint32 ipv4_multi_hop_acc_ref_counter;

    BCMDNX_INIT_FUNC_DEFS;

    if (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
        rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_MULTI_HOP_ACC_REF_COUNTER, &ipv4_multi_hop_acc_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
        ipv4_multi_hop_acc_ref_counter--;
        rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_MULTI_HOP_ACC_REF_COUNTER, &ipv4_multi_hop_acc_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
        if (ipv4_multi_hop_acc_ref_counter == 0) {
            SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
            soc_sand_rv = soc_ppd_oam_bfd_tx_ipv4_multi_hop_set(unit, &tx_itmh_attributes);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    } else {
        rv = _bcm_dpp_am_template_oam_tx_priority_free(unit, mep_db_entry->priority, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_last) {
            SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
            SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
            soc_sand_rv = soc_ppd_oam_oamp_tx_priority_registers_set(unit, mep_db_entry->priority, &tx_itmh_attributes);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    if (((mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) || (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS))
	&& !(mep_db_entry->tos_ttl_profile == _BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE)){
        rv = _bcm_dpp_am_template_bfd_tos_ttl_profile_free(unit, mep_db_entry->tos_ttl_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_last) {
            SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA tos_ttl_data;
            SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(&tos_ttl_data);
            soc_sand_rv = soc_ppd_oam_bfd_ipv4_tos_ttl_select_set(unit, mep_db_entry->tos_ttl_profile, &tos_ttl_data);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
	if (mep_db_entry->tos_ttl_profile == _BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE) {
		int tmp;
		rv = BFD_ACCESS.s_hop.ref_counter.get(unit, &tmp);
		BCMDNX_IF_ERR_EXIT(rv);
		if (tmp>0) {
			tmp--;
			rv = BFD_ACCESS.s_hop.ref_counter.set(unit, tmp);
			BCMDNX_IF_ERR_EXIT(rv);
		}
	}

    if ((mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) ||
        (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) ||
        (mep_db_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS)) {

        rv = _bcm_dpp_am_template_bfd_src_ip_profile_free(unit, mep_db_entry->src_ip_add_ptr, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_last) {
            soc_sand_rv = soc_ppd_oam_bfd_ipv4_src_addr_select_set(unit, mep_db_entry->src_ip_add_ptr, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    if ((mep_db_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) &&
        (mep_db_entry->mep_type != SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP)) {

        rv = _bcm_dpp_am_template_mpls_pwe_push_profile_free(unit, mep_db_entry->push_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_last) {
            SOC_PPD_MPLS_PWE_PROFILE_DATA push_data;
            SOC_PPD_MPLS_PWE_PROFILE_DATA_clear(&push_data);
            soc_sand_rv = soc_ppd_oam_mpls_pwe_profile_set(unit, mep_db_entry->push_profile, &push_data);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }


    rv = _bcm_dpp_am_template_bfd_tx_rate_profile_free(unit, mep_db_entry->ccm_interval, &is_last);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_last) {
        soc_sand_rv = soc_ppd_oam_bfd_tx_rate_set(unit, mep_db_entry->ccm_interval, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    rv = _bcm_dpp_am_template_bfd_req_interval_pointer_free(unit, mep_db_entry->min_rx_interval_ptr, &is_last);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_last) {
        soc_sand_rv = soc_ppd_oam_bfd_req_interval_pointer_set(unit, mep_db_entry->min_rx_interval_ptr, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    rv = _bcm_dpp_am_template_bfd_req_interval_pointer_free(unit, mep_db_entry->min_tx_interval_ptr, &is_last);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_last) {
        soc_sand_rv = soc_ppd_oam_bfd_req_interval_pointer_set(unit, mep_db_entry->min_tx_interval_ptr, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }


    if (SOC_IS_ARADPLUS(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0)) {
        rv = _bcm_dpp_am_template_bfd_flags_profile_free(unit, mep_db_entry->flags_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_last) {
            soc_sand_rv = soc_ppd_oam_bfd_flags_profile_set(unit, mep_db_entry->flags_profile, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Fill classifier entry struct */
int _bcm_bfd_classifier_mep_entry_struct_set(int unit, bcm_bfd_endpoint_info_t *endpoint_info, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry, int dip_ndx) {
    int is_local;
    uint32 ret;
    int fec_id;

    BCMDNX_INIT_FUNC_DEFS;

	classifier_mep_entry->ma_index = endpoint_info->local_discr; /* Used for IP single hop*/
    classifier_mep_entry->remote_gport = endpoint_info->remote_gport;
    classifier_mep_entry->flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
    classifier_mep_entry->flags |= ((endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) != 0) ? SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED : 0;
    classifier_mep_entry->your_discriminator = endpoint_info->local_discr & SOC_PPD_BFD_DISCRIMINATOR_TO_LIF_START_MASK;
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET) {
        classifier_mep_entry->flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER;
    }
    if ((!_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info->type)) && (endpoint_info->gport != BCM_GPORT_INVALID)) {
        ret = _bcm_dpp_gport_to_global_lif(unit, endpoint_info->gport, (int *)&classifier_mep_entry->lif, NULL, &fec_id, &is_local);
        BCMDNX_IF_ERR_EXIT(ret);
    }
    else {
         /* LSBs of your discriminator in this case are used for classification in the O-EM tables.*/
        classifier_mep_entry->lif = _BCM_OAM_INVALID_LIF;
    }

    switch (endpoint_info->type) {
    case bcmBFDTunnelTypeMpls:
        classifier_mep_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS;
        break;
    case bcmBFDTunnelTypeMplsTpCc:
        classifier_mep_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP;
        break;
    case bcmBFDTunnelTypeMplsTpCcCv:
        classifier_mep_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFDCV_O_MPLSTP;
        break;
    case bcmBFDTunnelTypePweGal:
        classifier_mep_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE_GAL;
        break;
    case bcmBFDTunnelTypeUdp:
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
            classifier_mep_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP;
            classifier_mep_entry->dip_profile_ndx = dip_ndx;
        } else {
            classifier_mep_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP;
        }
        break;
    case bcmBFDTunnelTypePweControlWord:
    case bcmBFDTunnelTypePweRouterAlert:
    case bcmBFDTunnelTypePweTtl:
        classifier_mep_entry->mep_type = SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Read classifier entry struct */
int _bcm_bfd_classifier_mep_entry_struct_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry, int *dip_ndx) {
    int ret;
    BCMDNX_INIT_FUNC_DEFS;


    endpoint_info->flags |= ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) ? BCM_BFD_ENDPOINT_IN_HW : 0;
    endpoint_info->remote_gport = classifier_mep_entry->remote_gport;

        switch (classifier_mep_entry->mep_type) {
        case SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS:
            endpoint_info->type = bcmBFDTunnelTypeMpls;
            break;
        case SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP:
            endpoint_info->type = bcmBFDTunnelTypeMplsTpCc;
            break;
        case SOC_PPD_OAM_MEP_TYPE_BFDCV_O_MPLSTP:
            endpoint_info->type = bcmBFDTunnelTypeMplsTpCcCv;
            break;
        case SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE_GAL:
            endpoint_info->type = bcmBFDTunnelTypePweGal;
            break;
        case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
            endpoint_info->type = bcmBFDTunnelTypeUdp;
            endpoint_info->flags |= BCM_BFD_ENDPOINT_MULTIHOP;
            break;
        case SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
            endpoint_info->type = bcmBFDTunnelTypeUdp;
            break;
        case SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE:
            endpoint_info->type = bcmBFDTunnelTypePweControlWord;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Internal error")));
        }
    if (classifier_mep_entry->mep_type==SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
        *dip_ndx = classifier_mep_entry->dip_profile_ndx; 
    }


    endpoint_info->local_discr = classifier_mep_entry->your_discriminator;
    if ((!_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info->type)) && (classifier_mep_entry->lif != _BCM_OAM_INVALID_LIF)) {
        int local_lif;
        int lif_to_gport_flags = _BCM_DPP_LIF_TO_GPORT_USE_SW_STATE | _BCM_DPP_LIF_TO_GPORT_INGRESS | _BCM_DPP_LIF_TO_GPORT_LOCAL_LIF;
        ret = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, classifier_mep_entry->lif, &local_lif);
        BCMDNX_IF_ERR_EXIT(ret);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_lif_to_gport(unit, local_lif, lif_to_gport_flags, &endpoint_info->gport));
    }
    else {
        endpoint_info->gport = BCM_GPORT_INVALID;
    }

    if (classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER) {
        endpoint_info->flags |= BCM_BFD_ENDPOINT_HW_ACCELERATION_SET;
    }

    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_bfd_trap_info_wb_add(int unit) {
    uint32 i;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    for (i = 0; i < SOC_PPC_BFD_TRAP_ID_COUNT; i++) {
        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_INIT_TRAP_INFO_TRAP_IDS, &_bcm_dpp_bfd_trap_info[unit].trap_ids[i], i);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_dpp_oam_bfd_mep_info_construct_bfd_mep_list_cb(int unit, shr_htb_key_t key, shr_htb_data_t data) {
    int rv = BCM_E_NONE;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *mep_entry_p;
    uint32 mep_index;

    BCMDNX_INIT_FUNC_DEFS;

    /*get the endpoint_info*/
    mep_entry_p = (SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *)data;
    mep_index = *(uint32 *)key;

    if (SOC_PPD_OAM_IS_MEP_TYPE_BFD(mep_entry_p->mep_type)) {
        rv = _bcm_dpp_oam_endpoint_list_member_add(unit, &_bcm_bfd_endpoint_list[unit], mep_index);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}
#endif /*BCM_WARM_BOOT_SUPPORT*/

/*
 * _bcm_bfd_default_id_from_ep_id
 *
 * If an endpoint ID is one of the default BCM endpoint IDs,
 * trnaslates the ID to a default endpoint ID in the soc layer.
 * If not a default BCM endpoint ID, returns a parameter error.
 */
int _bcm_bfd_default_id_from_ep_id(int unit, int endpoint, ARAD_PP_OAM_DEFAULT_EP_ID *default_id)
{

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(default_id);

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        if (endpoint != -1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not supported in Arad.")));
        }
    }
    else {
        switch (endpoint) {
        case BCM_BFD_ENDPOINT_DEFAULT0:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_0;
            break;
        case BCM_BFD_ENDPOINT_DEFAULT1:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_1;
            break;
        case BCM_BFD_ENDPOINT_DEFAULT2:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_2;
            break;
        case BCM_BFD_ENDPOINT_DEFAULT3:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_3;
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
 * Begin BCM Functions
 *
 */


/* Initialize the BFD subsystem */
int bcm_petra_bfd_init(int unit) {
    uint32 soc_sand_rv;
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config_oamp, trap_config_cpu;
    bcm_port_config_t port_config;
    bcm_gport_t oamp_port[1], cpu_port;
    int count_erp;
    uint8 oam_is_init, is_bfd_init;
	uint32 prof_ind, flags_value;
	int tmp=0;
	int bfd_ipv6_trap_port;
    int trap_code_converted;
    int trap_id_sw; 
 
    BCMDNX_INIT_FUNC_DEFS;

    rv = BFD_ACCESS.alloc(unit);
    BCMDNX_IF_ERR_EXIT(rv);
    
    _BCM_BFD_ENABLED_GET(is_bfd_init);

	if (is_bfd_init) {
#ifdef BCM_WARM_BOOT_SUPPORT
		if (!SOC_WARM_BOOT(unit))
#endif /*BCM_WARM_BOOT_SUPPORT*/
		{
			BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: BFD already initialized.\n")));
		}
	}
#ifdef BCM_WARM_BOOT_SUPPORT
	else {
		if (SOC_WARM_BOOT(unit)) {
			BCM_EXIT;
		}
	}
#endif /*BCM_WARM_BOOT_SUPPORT*/

	/*init global variables*/
	rv = BFD_ACCESS.s_hop.tos_value.set(unit, tmp);
	BCMDNX_IF_ERR_EXIT(rv);
				
	rv = BFD_ACCESS.s_hop.ref_counter.set(unit, tmp);
	BCMDNX_IF_ERR_EXIT(rv);

    _BCM_OAM_ENABLED_GET(oam_is_init);

/* In warmboot oam init is called first and inits DBs, so we don't need to init them from BFD */
	if (!oam_is_init) {
        rv = _bcm_dpp_oam_dbs_init(unit, FALSE/*is_oam*/);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_PP_OAM);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (SOC_IS_ARADPLUS(unit)) {
            soc_sand_rv = soc_ppd_oam_register_dma_event_handler_callback(unit, _bcm_dpp_oam_fifo_interrupt_handler);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    /* Init the uKernel BFD application */
    if (soc_property_get(unit, spn_MCS_LOAD_UC0, 0) && (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable)) {
        rv = bcm_petra_bfd_uc_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    _bcm_dpp_oam_endpoint_list_init(&_bcm_bfd_endpoint_list[unit]);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_dpp_bfd_get_hash_tables_from_wb_arrays(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    } 
    else 
#endif /*BCM_WARM_BOOT_SUPPORT*/    
    {
        rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &count_erp);
        BCMDNX_IF_ERR_EXIT(rv);
        if (count_erp < 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: Oamp port disabled, can't init OAM.")));
        }

        SOC_PPD_BFD_INIT_TRAP_INFO_clear(&_bcm_dpp_bfd_trap_info[unit]);
        bcm_rx_trap_config_t_init(&trap_config_oamp);
        bcm_rx_trap_config_t_init(&trap_config_cpu);

        /*get CPU port*/
        rv = bcm_petra_port_config_get(0, &port_config);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ITER(port_config.cpu, cpu_port) {
            break; /*get first CPU port*/
        }

        /*creating cpu trap id*/
        _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_CPU] = SOC_PPD_TRAP_CODE_OAM_CPU;
        _BCM_OAM_ENABLED_GET(oam_is_init);

        if (!oam_is_init) {

            int trap_id_sw;
            int trap_code;

            trap_id_sw = _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_CPU];


             BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,
                                  trap_id_sw, 
                                  &trap_code));

            rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int *)&trap_code);
            BCMDNX_IF_ERR_EXIT(rv);

            trap_config_cpu.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
            trap_config_cpu.dest_port = cpu_port;
            trap_config_cpu.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config_cpu.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
            rv = bcm_petra_rx_trap_set(unit, trap_code, &trap_config_cpu);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_oam_bfd_common_cpu_trap_code_set(unit, _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_CPU] /*transfering the SW id*/);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Increasing trap code counter in order to for this trap not to be deleted with another profile */
            rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_CPU]);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            rv = _bcm_oam_bfd_common_cpu_trap_code_get(unit, &_bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_CPU]);
            BCMDNX_IF_ERR_EXIT(rv);
        }


        /*creating oamp trap ids*/
        trap_config_oamp.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
        trap_config_oamp.dest_port = *oamp_port;

		/*configue bfd echo trap code*/ 
		if (soc_property_get(unit, spn_BFD_ECHO_ENABLED, 0) == 1) { 

            int trap_id_sw;

            trap_id_sw = SOC_PPC_TRAP_CODE_BFD_ECHO;

             BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,
                                  trap_id_sw, 
                                  &trap_code_converted));

            rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code_converted);
			BCMDNX_IF_ERR_EXIT(rv);
            trap_config_oamp.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config_oamp.forwarding_header = bcmRxTrapForwardingHeaderThirdHeader;
            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_oamp);
			BCMDNX_IF_ERR_EXIT(rv);

		} 

		/*configue bfd ipv4 single hop your discriminator not found trap code*/ 
		if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop == 1) { 
	
            int trap_id_sw;
            int trap_code_converted;

            trap_id_sw = SOC_PPC_TRAP_CODE_BFD_IPV4_YOUR_DISCR_NOT_FOUND;

            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,
                                  trap_id_sw, 
                                  &trap_code_converted));

			rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code_converted);
			BCMDNX_IF_ERR_EXIT(rv);
			trap_config_cpu.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config_cpu.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_cpu);
			BCMDNX_IF_ERR_EXIT(rv);

		} 


        _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_IPV4] = SOC_PPD_TRAP_CODE_TRAP_BFD_O_IPV4;
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamBfdIpv4, (int *)&trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
        trap_config_oamp.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
        trap_config_oamp.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
        rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_oamp);
        BCMDNX_IF_ERR_EXIT(rv);

        _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_MPLS] = SOC_PPD_TRAP_CODE_TRAP_BFD_O_MPLS;
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamBfdMpls, (int *)&trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_oamp);
        BCMDNX_IF_ERR_EXIT(rv);

        _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_PWE] = SOC_PPD_TRAP_CODE_TRAP_BFD_O_PWE;
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamBfdPwe, (int *)&trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_oamp);
        BCMDNX_IF_ERR_EXIT(rv);

        _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_CC_MPLS_TP] = SOC_PPD_TRAP_CODE_TRAP_BFDCC_O_MPLS_TP;
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamBfdCcMplsTp, (int *)&trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_oamp);
        BCMDNX_IF_ERR_EXIT(rv);

       bfd_ipv6_trap_port = soc_property_get(unit, spn_BFD_IPV6_TRAP_PORT, -1);
        if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable) {
            if (bfd_ipv6_trap_port != -1) {
                bcm_rx_trap_config_t trap_config_uc;

                bcm_rx_trap_config_t_init(&trap_config_uc);
                trap_config_uc.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
                trap_config_uc.dest_port = bfd_ipv6_trap_port;

                _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_UC_IPV6] = SOC_PPD_TRAP_CODE_TRAP_BFD_O_IPV6;

                trap_id_sw  = _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_UC_IPV6];

                BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,
                                                                        trap_id_sw,
                                                                        &trap_code_converted));

                rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int *)&trap_code_converted);
                BCMDNX_IF_ERR_EXIT(rv);
                trap_config_uc.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                trap_config_uc.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
                rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_uc);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BFD over IPv6 can't be enabled without BFD IPv6 trap port configuration")));
            }
        }

        rv = _bcm_dpp_bfd_trap_info_wb_add(unit);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_ppd_oam_init(unit, NULL, &_bcm_dpp_bfd_trap_info[unit], TRUE /*BFD*/);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        _BCM_BFD_ENABLED_SET(TRUE);

        rv = _bcm_dpp_oamp_interrupts_init(unit, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);

        if (SOC_IS_ARADPLUS(unit)) {
    		/* Set the diag values (1-15)*/
    		for (prof_ind = 1;  prof_ind < 16 /* 16 profiles */; ++prof_ind) {
    			soc_sand_rv = soc_ppd_oam_bfd_diag_profile_set(unit,prof_ind, prof_ind );
    			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    		}

    		/* Set the flags values (1-7)*/
    		for (prof_ind = 1;  prof_ind < 8 /* 7 profiles */; ++prof_ind) {
    			_BCM_BFD_FLAGS_PROFILE_INDEX_TO_FLAGS(prof_ind, flags_value);
    			soc_sand_rv = soc_ppd_oam_bfd_flags_profile_set(unit, prof_ind, flags_value);
    			BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
    		}
    	}

        /* Set FLP PTC profile for OAMP port */ 
        if (SOC_DPP_CONFIG(unit)->pp.oam_statistics == 1) {
            uint32 oamp_local_port;
            ARAD_PORTS_PROGRAMS_INFO port_prog_info;	
            _bcm_dpp_gport_info_t gport_info;
            arad_ARAD_PORTS_PROGRAMS_INFO_clear(&port_prog_info);
            soc_sand_rv = _bcm_dpp_gport_to_phy_port(unit, *oamp_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            oamp_local_port = gport_info.local_port;
            soc_sand_rv = arad_ports_programs_info_get_unsafe(unit, oamp_local_port, &port_prog_info); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            port_prog_info.ptc_flp_profile = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
            soc_sand_rv = arad_ports_programs_info_set_unsafe(unit, oamp_local_port, &port_prog_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        }

        /* inserting an entry to the SEM- For Identifying BFD packets for statistics feature*/
        if (SOC_DPP_CONFIG(unit)->pp.oam_statistics == 1) {
            SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              success_vt;
            SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              success_tt;
            ARAD_PP_ISEM_ACCESS_KEY	sem_key;
            ARAD_PP_ISEM_ACCESS_ENTRY	sem_entry;

            /* add entry to tt stage */
            sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_TT_ISA;
            sem_key.key_info.oam_stat.bfd_format = 0x1; /*pph fwd code ipv4*/
            rv = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, &success_tt);
            sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_TT_ISB;
            sem_key.key_info.oam_stat.bfd_format = 0x4; /*v4oLSP*/
            rv = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, &success_tt);
            BCMDNX_IF_ERR_EXIT(rv);
            /* add entry to vt stage opaque=5 bfd*/
            sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_VT;
            sem_key.key_info.oam_stat.pph_type = 0x4;
            sem_key.key_info.oam_stat.opaque = 0x7;
            rv = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, &success_vt);
            BCMDNX_IF_ERR_EXIT(rv);
        }

    }

	


    

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_tx_start
 * Purpose:
 *      Start all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to start TX which does not exist
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
int
bcm_petra_bfd_tx_start(int unit)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_tx_start(unit);
        BCMDNX_IF_ERR_EXIT(rv);            
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_tx_stop
 * Purpose:
 *      Stop all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to stop TX which does not exist
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
int
bcm_petra_bfd_tx_stop(int unit)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_tx_stop(unit);
        BCMDNX_IF_ERR_EXIT(rv);            
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_bfd_endpoint_create(int unit, bcm_bfd_endpoint_info_t *endpoint_info) {

    uint32 rmep_index = (uint32)(-1);
    uint32 rmep_index_internal;
    uint32 soc_sand_rv = 0;
    bcm_error_t rv = BCM_E_NONE;
    int flags = 0;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry_temp;
    uint8 found;
    SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
    SOC_PPD_OAM_RMEP_INFO_DATA rmep_info;
    endpoint_list_t *rmep_list_p = NULL;
    uint16 udp_sport;
    uint8 new_rmep_id_alloced = FALSE;
    uint32 new_disc_range, range;
    SOC_PPD_OAM_MA_NAME name = { 0 };
    int is_allocated;
    int dip_ndx = 0;
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data;
    /* These variables are used to call soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile api
       which is used in Ethernet OAM as well */
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data_acc_dummy;
    SOC_PPD_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP trap_code_to_mp_map_dummy;
    uint32 trap_code;
    int profile;
    bcm_bfd_endpoint_info_t existing_endpoint_info;
    uint32 mpls_udp_sport_ref_counter;
    uint32 ipv4_udp_sport_ref_counter;
    int ref_counter; /*used for static registers - bfd_pdu and bfd_cc_packet*/
    uint8 is_bfd_init;
    int correct_flags = 0;
    uint8 port_is_equal;
	uint8 found_profile;
	uint8 is_mp_type_flexible_dummy, is_mip_dimmy;
    uint8 internal_bfd_opcode;
    uint32 profile_temp;
    uint32 oem_key;
    uint32 server_trap_ref_counter;
    uint8 is_default;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
	uint8 is_1_hop_extended = 0;
    bcm_bfd_endpoint_info_t endpoint_info_lcl_t;
    bcm_bfd_endpoint_info_t *endpoint_info_lcl;

    BCMDNX_INIT_FUNC_DEFS;

    endpoint_info_lcl = &endpoint_info_lcl_t; /* If the GPORT contains a trap, since the trap id inside the GPORT can be either the HW id or the SW_id ( according to the use_hw_id SOC peroperty) , and this function 
    assumes that the trap id is the SW id, it will use a copy of the endpoint_info with the SW id */
    sal_memcpy(endpoint_info_lcl, endpoint_info, sizeof(bcm_bfd_endpoint_info_t));  /* from here till the end of the function, the function uses the copy of the endpoint */

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    /* if the use_hw_id soc property is enabled, then the trap_id is the HW id and thus we need to convert it to the sw id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP(endpoint_info_lcl->remote_gport))
    {
        _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, endpoint_info_lcl->remote_gport);
    }

    if (bcm_petra_bfd_uc_is_init(unit)) {
        if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IPV6) {
            bcm_bfd_endpoint_t id_internal;

            if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
                _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(id_internal, endpoint_info_lcl->id); 
                endpoint_info_lcl->id = id_internal;
            }
            rv = bcm_petra_bfd_uc_endpoint_create(unit, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
            _BCM_BFD_UC_MEP_INDEX_FROM_INTERNAL(endpoint_info_lcl->id, endpoint_info_lcl->id);

            BCM_EXIT;           
        }
    }

    is_default = _BCM_BFD_IS_MEP_ID_DEFAULT(unit, endpoint_info_lcl->id);

    if (is_default && SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Default endpoint not supported for device.")));
    }

    /*initial input check*/
    correct_flags |= BCM_BFD_ENDPOINT_REMOTE_WITH_ID;
    correct_flags |= BCM_BFD_ENDPOINT_UPDATE | BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MPLS_TP_POLL_SEQUENCE_ENABLE |
                     BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE | BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE | BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE |
                     BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE | BCM_BFD_ENDPOINT_RDI_ON_LOC | BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR | BCM_BFD_ENDPOINT_HW_ACCELERATION_SET |BCM_BFD_ECHO;

    if (endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) {
        correct_flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    }

	if ((endpoint_info_lcl->flags & BCM_BFD_ECHO) && (SOC_IS_ARAD_B1_AND_BELOW(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: BCM_BFD_ECHO is supported only on ARAD+ and above devices.")));
    }

	if ((endpoint_info_lcl->type != bcmBFDTunnelTypeUdp) && (endpoint_info_lcl->flags & BCM_BFD_ECHO) )
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: BFD ECHO suppoerted only on IPv4 type.")));
    }

	if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET) && (endpoint_info_lcl->flags & BCM_BFD_ECHO)) {
		 BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: BCM_BFD_ENDPOINT_HW_ACCELERATION_SET is not supported with BCM_BFD_ECHO")));
	}

	if ((endpoint_info_lcl->flags & BCM_BFD_ECHO) && ((!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW)) || (!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP)))) {
		 BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: BCM_BFD_ECHO flag must be set with BCM_BFD_ENDPOINT_IN_HW and BCM_BFD_ENDPOINT_MULTIHOP flags")));
	}

	if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET) && (SOC_IS_ARAD_B1_AND_BELOW(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: BCM_BFD_ENDPOINT_HW_ACCELERATION_SET is supported only on ARAD+ and above devices.")));

    }

    if ((endpoint_info_lcl->local_state != bcmBFDStateAdminDown) && (endpoint_info_lcl->local_state != bcmBFDStateDown) && (endpoint_info_lcl->local_state != bcmBFDStateUp)
        && (endpoint_info_lcl->local_state != bcmBFDStateInit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: local state incorrect. Allowed values are only bcmBFDStateAdminDown, bcmBFDStateDown, bcmBFDStateUp, bcmBFDStateInit.")));
    }
    if ((endpoint_info_lcl->type != bcmBFDTunnelTypeUdp) && (endpoint_info_lcl->type != bcmBFDTunnelTypeMpls) && (endpoint_info_lcl->type != bcmBFDTunnelTypePweControlWord)
        && (endpoint_info_lcl->type != bcmBFDTunnelTypeMplsTpCc) && (endpoint_info_lcl->type != bcmBFDTunnelTypeMplsTpCcCv) && (endpoint_info_lcl->type != bcmBFDTunnelTypePweGal)) {
        if (SOC_IS_ARAD_B1_AND_BELOW(unit) || (endpoint_info_lcl->type != bcmBFDTunnelTypePweRouterAlert)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: type incorrect. Allowed values are only bcmBFDTunnelTypeUdp, bcmBFDTunnelTypeMpls, bcmBFDTunnelTypePweControlWord, bcmBFDTunnelTypeMplsTpCc"
                                           "%s."),SOC_IS_JERICHO(unit)? ", bcmBFDTunnelTypePweGal, bcmBFDTunnelTypePweRouterAlert" : "" ));
        }
    }
    if (SOC_IS_JERICHO(unit) && ((endpoint_info_lcl->type == bcmBFDTunnelTypePweRouterAlert) || (endpoint_info_lcl->type == bcmBFDTunnelTypePweTtl))) {
        correct_flags |= BCM_BFD_ENDPOINT_PWE_ACH;
    }
    if (endpoint_info_lcl->flags & ~correct_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: invalid flag combination is set.")));
    }
    if (((endpoint_info_lcl->type != bcmBFDTunnelTypeUdp) && (endpoint_info_lcl->type != bcmBFDTunnelTypeMpls) )
        && (endpoint_info_lcl->src_ip_addr != 0 || endpoint_info_lcl->udp_src_port != 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: src_ip_addr and udp_src_port only supported for BFDoMPLS and BFDoIPV4.")));
    }

    if (endpoint_info_lcl->type == bcmBFDTunnelTypePweTtl) {
        if (endpoint_info_lcl->egress_label.ttl != 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: In PWE TTL type egress_label.ttl must be 1.")));
        }
    }

    if ( (endpoint_info_lcl-> flags & BCM_BFD_ENDPOINT_IN_HW) ==0 || (endpoint_info_lcl->type == bcmBFDTunnelTypeUdp)) {
        /* NON acc endpoint or IP endpoint. Either way TX MPLS fields must be 0. */
        if (endpoint_info_lcl->egress_label.ttl ||  endpoint_info_lcl->egress_label.flags || endpoint_info_lcl->egress_label.tunnel_id  || endpoint_info_lcl->egress_label.qos_map_id
             || endpoint_info_lcl->egress_label.l3_intf_id   || endpoint_info_lcl->egress_label.pkt_cfi  || endpoint_info_lcl->egress_label.label ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: egress_label must be set only for accelerated MPLS endpoints.")));
        }
    }
 

    if (!_BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(*endpoint_info_lcl)) {
        /* non accelerated EP. TX fields should be 0.*/
        if ( endpoint_info_lcl->int_pri || endpoint_info_lcl->local_diag || endpoint_info_lcl->remote_discr || endpoint_info_lcl->remote_flags || endpoint_info_lcl->local_flags || endpoint_info_lcl->loc_clear_threshold
			  || endpoint_info_lcl->remote_state || endpoint_info_lcl->remote_diag || endpoint_info_lcl->local_state || endpoint_info_lcl->local_min_echo || endpoint_info_lcl->local_min_tx || endpoint_info_lcl->local_detect_mult 
			  || endpoint_info_lcl->dst_ip_addr || endpoint_info_lcl->bfd_period || endpoint_info_lcl->remote_detect_mult ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: TX fields may not be set for non accelerated endpoint.")));
        }

        if (endpoint_info_lcl->tx_gport != BCM_GPORT_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: tx_gport may not be set for non accelerated endpoints.")));

        }

        if ((!_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info_lcl->type))
                && endpoint_info_lcl->local_discr) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: local discriminator is not used in this endpoint but a value was given.")));
        }
    } else {
        if (endpoint_info_lcl->tx_gport != BCM_GPORT_INVALID &&  !BCM_GPORT_IS_SYSTEM_PORT(endpoint_info_lcl->tx_gport) &&
            !BCM_GPORT_IS_MODPORT(endpoint_info_lcl->tx_gport) &&  !BCM_GPORT_IS_TRUNK(endpoint_info_lcl->tx_gport)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: tx-gport must be either invalid or a system port.")));
        }
		if (endpoint_info_lcl->int_pri >0x1f) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: int_pri field should be {traffic class[2:0], drop precedence[0:1]}")));
		}
        if (SOC_IS_ARAD_B1_AND_BELOW(unit) || soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0)) {
            if ( (endpoint_info_lcl->remote_flags& ~0x3f) || (endpoint_info_lcl->local_flags& ~0x3f) ) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                     (_BSL_BCM_MSG("Error: remote_flags, local_flags 6 bit fields.")));
            }
        } else {
            if (_BCM_BFD_FLAG_COMBINATION_IS_NOT_SUPPORTED(endpoint_info_lcl->remote_flags) || 
                _BCM_BFD_FLAG_COMBINATION_IS_NOT_SUPPORTED(endpoint_info_lcl->local_flags)) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: Only the flags P, F, D are supported. Given local_flags: %d, remote_flags: %d"),
                                  endpoint_info_lcl->local_flags , endpoint_info_lcl->remote_flags ));
            }
        }
		if (endpoint_info_lcl->local_diag > 16) {
						BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
							 (_BSL_BCM_MSG("Error: local_diag field must be between 0 and 16.")));
		}
		if (endpoint_info_lcl->remote_diag > 16) {
						BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
							 (_BSL_BCM_MSG("Error: remote_diag field must be between 0 and 16.")));
		}
        if (endpoint_info_lcl->loc_clear_threshold > 3) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                 (_BSL_BCM_MSG("Error: loc_clear_threshold may not be greater than 3")));
        }
		if (endpoint_info_lcl->local_min_rx * endpoint_info_lcl->remote_detect_mult > _BCM_BFD_MAX_LIFETIME_IN_MS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                 (_BSL_BCM_MSG("Error: local_min_rx * remote_detect_mult is too big")));
        }

        if (endpoint_info_lcl->bfd_period > 3406 /* floor(0x3ff * 3.33) is 3406 */) { /* writing on a 10 bit field, in 3.33 ms units*/
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: bfd rate may not be greater than %d"), 0x3ff * 3.33)); 
        }

        /*Other fields are verified depending on the type.*/
    }


    if ( (endpoint_info_lcl->type != bcmBFDTunnelTypeUdp && endpoint_info_lcl->type != bcmBFDTunnelTypeMpls ) && (endpoint_info_lcl->ip_ttl || endpoint_info_lcl->ip_tos)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: IP fields should only be set only for BFDoIP, BFDoIPoMPLS.")));

    }

	if (_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info_lcl->type) && endpoint_info_lcl->gport!=BCM_GPORT_INVALID) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
						 (_BSL_BCM_MSG("Error: For BFD endpoints of type UDP gport field must be set to BCM_GPORT_INVALID")));

	}

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        if (SOC_DPP_CONFIG(unit)->pp.bfd_mpls_lsp_support==0 && SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop==0) {
            if (endpoint_info->type ==bcmBFDTunnelTypeMpls ) {


                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: BFDoIPoMPLS not supported without suitable soc property."))); 
            }
        } 
    }

	/*check if we are in bfd ipv4 single hop extended mode*/
	if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop == 1){
		if ((endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) && !(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) && (is_default==0)
		&& (endpoint_info_lcl->flags &BCM_BFD_ENDPOINT_IN_HW)){
			is_1_hop_extended =1; /* 1= accelerated 2 = non accelerated*/
		}
		else if ((endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) && !(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) && (is_default==0)){
			is_1_hop_extended =2;
		}
	}

    /* get the internal bfd opcode */
    /* we use CCM opcode to signal BFD packets. This is not used in HW and isn't influence by opcode to internal opcode mapping*/
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_Y1731_OPCODE_TO_INTERNAL_OPCODE_MAP, &internal_bfd_opcode, SOC_PPD_OAM_ETHERNET_PDU_OPCODE_CCM);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Endpoint replace */
    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_UPDATE) {
		SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY new_classifier_entry;
        SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&new_classifier_entry);

        if (!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: BCM_BFD_ENDPOINT_REPLACE flag can be used only with BCM_BFD_ENDPOINT_WITH_ID specification.\n")));
        }

        if (is_default) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: Can not replace default mep.\n")));
        }

        bcm_bfd_endpoint_info_t_init(&existing_endpoint_info);
        rv = bcm_petra_bfd_endpoint_get(unit, endpoint_info_lcl->id, &existing_endpoint_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* if the use_hw_id soc property is enabled, then the trap_id is the HW id and thus we need to convert it to the sw id becuase the oam logic uses the SW id*/
        if (BCM_GPORT_IS_TRAP(existing_endpoint_info.remote_gport))
        {
            _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, existing_endpoint_info.remote_gport);
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Copy information from old data base. Whatever needs updating will be updated.*/
        new_classifier_entry = classifier_mep_entry;


		if (endpoint_info->gport != BCM_GPORT_INVALID || existing_endpoint_info.gport != BCM_GPORT_INVALID) {
			int new_lif_id, existing_lif_id, dont_care;
			rv = _bcm_dpp_gport_to_global_lif(unit, endpoint_info->gport, &new_lif_id, &dont_care, &dont_care, &dont_care);
			BCMDNX_IF_ERR_EXIT(rv); 
			rv = _bcm_dpp_gport_to_global_lif(unit, existing_endpoint_info.gport, &existing_lif_id, &dont_care, &dont_care, &dont_care);
			BCMDNX_IF_ERR_EXIT(rv); 

			if (new_lif_id != existing_lif_id) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: gport may not be updated.\n") ));
			}		           
        }
        if (endpoint_info_lcl->tx_gport != BCM_GPORT_INVALID || existing_endpoint_info.tx_gport != BCM_GPORT_INVALID ) {
            if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW)) {
                rv = _bcm_dpp_gport_compare(unit, endpoint_info_lcl->tx_gport, existing_endpoint_info.tx_gport, &port_is_equal);
                if (rv != BCM_E_NONE || !port_is_equal) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: tx_gport can not be updated.\n")));
                }
            }
        }
        if (endpoint_info_lcl->type != existing_endpoint_info.type) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: type can not be updated.\n")));
        }

        if (endpoint_info_lcl->remote_id != existing_endpoint_info.remote_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: remote_id can not be updated.\n")));
        }

        if (((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) != 0) != ((existing_endpoint_info.flags & BCM_BFD_ENDPOINT_IN_HW) != 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Acceleration in HW may not be added / removed.\n")));
        }

        if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) || _BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info_lcl->type)) { /* check discriminator */
            soc_sand_rv = soc_ppd_oam_bfd_discriminator_range_registers_get(unit, &range);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if ((endpoint_info_lcl->local_discr & SOC_PPD_BFD_DISCRIMINATOR_TO_LIF_START_MASK ) != (existing_endpoint_info.local_discr & SOC_PPD_BFD_DISCRIMINATOR_TO_LIF_START_MASK)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: LSBs of Local discriminator may not be updated.\n")));
            }
            if (range != endpoint_info_lcl->local_discr >> SOC_PPD_BFD_DISCRIMINATOR_RANGE_BIT_START) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: MSBs of Local discriminator may not be updated.\n")));
            }
        }
            
        if ((endpoint_info_lcl->udp_src_port) != (existing_endpoint_info.udp_src_port)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: udp_src_port may not be updated.\n")));
        }

        if ((endpoint_info_lcl->dst_ip_addr) != (existing_endpoint_info.dst_ip_addr)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: dst_ip_addr may not be updated.\n")));
        }

        if ((endpoint_info_lcl->int_pri) != (existing_endpoint_info.int_pri)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: int_pri can not be updated.\n")));
        }

        if ((endpoint_info_lcl->src_ip_addr) != (existing_endpoint_info.src_ip_addr)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: src_ip_addr can not be updated.\n")));
        }

        if ((endpoint_info_lcl->ip_tos) != (existing_endpoint_info.ip_tos)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: ip_tos can not be updated.\n")));
        }

        if ((endpoint_info_lcl->ip_ttl) != (existing_endpoint_info.ip_ttl)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: ip_ttl can not be updated.\n")));
        }

        if ((endpoint_info_lcl->egress_label.ttl) != (existing_endpoint_info.egress_label.ttl)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: egress_label.ttl can not be updated.\n")));
        }

        if ((endpoint_info_lcl->egress_label.exp) != (existing_endpoint_info.egress_label.exp)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: egress_label.exp can not be updated.\n")));
        }

        if ((endpoint_info_lcl->egress_label.label) != (existing_endpoint_info.egress_label.label)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: egress_label.label can not be updated.\n")));
        }

        if ((endpoint_info_lcl->egress_if) != (existing_endpoint_info.egress_if)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: egress_if can not be updated.\n")));
        }

        
        rv = _bcm_bfd_classifier_mep_entry_struct_set(unit, endpoint_info_lcl, &new_classifier_entry, classifier_mep_entry.dip_profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv); 

		

        if (_BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(*endpoint_info_lcl)) { 
            /* Accelerated - Inserting entries to OAMP dbs */
            SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;

            /*Setting the static registers.*/
            SOC_DPP_WB_ENGINE_VAR wb_eng_var = (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCc) ? SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_TP_CC_REF_COUNTER :
                                                                                                   SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_PDU_REF_COUNTER;
            rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, wb_eng_var, &ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_bfd_manage_static_registers(unit, endpoint_info_lcl, ref_counter, (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCc), 1);
            BCMDNX_IF_ERR_EXIT(rv);

            /*
            * MEP DB entry
            */
            LOG_VERBOSE(BSL_LS_BCM_BFD,
                        (BSL_META_U(unit,
                                    "BFD endpoint update: TX\n")));
            SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
            soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_info_lcl->id, &mep_db_entry);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = _bcm_mep_db_entry_dealloc(unit, &mep_db_entry);
            BCMDNX_IF_ERR_EXIT(rv);
            SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
            rv = _bcm_bfd_mep_db_entry_struct_set(unit, endpoint_info_lcl, &mep_db_entry);
            BCMDNX_IF_ERR_EXIT(rv);
            /*set entry in mep db*/
            soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info_lcl->id, &mep_db_entry, 0, name);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /*
            * RMEP DB entry
            */

            LOG_VERBOSE(BSL_LS_BCM_BFD,
                        (BSL_META_U(unit,
                                    "Updating remote entry.\n")));

            SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);
            rv = _bcm_bfd_rmep_db_entry_struct_set(unit, endpoint_info_lcl, &rmep_db_entry);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Update entry in rmep db and rmep index db*/
            _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_index_internal, endpoint_info_lcl->remote_id); 
            soc_sand_rv = soc_ppd_oam_oamp_rmep_set(unit, rmep_index_internal, 0, endpoint_info_lcl->id, mep_db_entry.mep_type, &rmep_db_entry, 1);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


        }

        /*update sw db mep_info with new mep */
        rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint_info_lcl->id);
        BCMDNX_IF_ERR_EXIT(rv);

		/*update sw db mep_info with new mep */
		rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &new_classifier_entry);
    	BCMDNX_IF_ERR_EXIT(rv);
    } /* END OF UPDATE */
    else {
        if (is_default) {
            /* Default endpoint */
            if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
                SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data);

                classifier_mep_entry.non_acc_profile = SOC_PPD_OAM_PROFILE_DEFAULT;
                classifier_mep_entry.flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;

                if (endpoint_info_lcl->remote_gport == BCM_GPORT_INVALID) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG("Error: BFD Default trap - remote_gport must be specified")));
                }

                classifier_mep_entry.remote_gport = endpoint_info_lcl->remote_gport;

                rv = _bcm_dpp_oam_bfd_trap_code_from_remote_gport(unit, endpoint_info_lcl->remote_gport, &trap_code, TRUE/*meta_data*/, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_code);
                BCMDNX_IF_ERR_EXIT(rv);

                profile_data.mep_profile_data.opcode_to_trap_code_multicast_map[internal_bfd_opcode] = trap_code; /* No unicast in BFD */
                profile_data.is_default = 1;

                profile = 0;

                rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_alloc(unit, SHR_TEMPLATE_MANAGE_SET_WITH_ID, &profile_data, &is_allocated, &profile);
                BCMDNX_IF_ERR_EXIT(rv);

                if (is_allocated) {
                    rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data.mep_profile_data);
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                /* Default BFD is mapped to endpoint -2 to avoid colision with oam default endpoint */
                rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, -2, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);

                soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &classifier_mep_entry, NULL, 1);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            else { /* Arad+ and above default endpoint create */
                rv = _bcm_bfd_default_id_from_ep_id(unit, endpoint_info_lcl->id, &default_id);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Check that this endpoint ID isn't occupied */
                rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                        (_BSL_BCM_MSG("Error: Default endpoint already exists.\n")));
                }

                /* Init classifier entry */
                SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
                classifier_mep_entry.flags |= SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
                classifier_mep_entry.mep_type = SOC_PPC_OAM_MEP_TYPE_COUNT; /* For diag callback only! This value is not used elsewhere */
                classifier_mep_entry.remote_gport = endpoint_info_lcl->remote_gport;
                if (endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
                    /* Get trap code */
                    rv = _bcm_dpp_oam_bfd_trap_code_from_remote_gport(unit, endpoint_info_lcl->remote_gport, &trap_code, TRUE/*meta_data*/, &classifier_mep_entry);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_code);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                else {
                    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_INIT_TRAP_INFO_TRAP_IDS, &trap_code, SOC_PPD_BFD_TRAP_ID_CPU);
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                /* Allocate profile */
                SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data);
                profile_data.mep_profile_data.opcode_to_trap_code_multicast_map[internal_bfd_opcode] = trap_code; /* No unicast in BFD */

                profile = 0;

                rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_alloc(unit, 0/*flags*/, &profile_data, &is_allocated, &profile);
                BCMDNX_IF_ERR_EXIT(rv);
                if (is_allocated) {
                    rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data.mep_profile_data);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                classifier_mep_entry.non_acc_profile = (uint8)profile;

                /* Load to HW */
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_add,
                                                  (unit, default_id,&classifier_mep_entry, 0/*update_action_only*/));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &classifier_mep_entry, NULL, 1);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /*update sw db mep_info with new mep */
                rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);

            }
        }
        else { /* Non-default new endpoint create */
            if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
                rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint_info_lcl->id, &rmep_list_p, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                /* Validate that mep with same id does not exist */
                if (found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                                     (_BSL_BCM_MSG("Error: Endpoint with id %d already exist."), endpoint_info_lcl->id));
                }

                /* Id should be equal to lowest 13 bits of your discriminator in case packet is accelerating to OAMP */
                if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) &&
                    ((endpoint_info_lcl->remote_gport == BCM_GPORT_INVALID) || (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET))) {
                    if (endpoint_info_lcl->id != (endpoint_info_lcl->local_discr & SOC_PPD_BFD_DISCRIMINATOR_TO_ACC_MEP_ID_START_MASK)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG("Error: Error in id %d: id should be equal to lowest %d bits of local_discr"), endpoint_info_lcl->id, (SOC_IS_JERICHO(unit)?14:13 )));
                    }
                }
            }

            /* If using YourDiscriminator, id should be taken from your discriminator LSBs and MSBs define a range that is checked in the identification */
            if (_BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(*endpoint_info_lcl) ||
                (_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info_lcl->type))) {
                /* This is the MSB of My Disc, that will be added to 16 bit MEP_DB_Index to get My discriminator on sent packets */
                new_disc_range = endpoint_info_lcl->local_discr >> SOC_PPD_BFD_DISCRIMINATOR_RANGE_BIT_START;
                rv = BFD_ACCESS.YourDiscriminator_ref_count.get(unit, &ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
                if (ref_counter > 0) {
                    /* Range should have already been set */
                    soc_sand_rv = soc_ppd_oam_bfd_discriminator_range_registers_get(unit, &range);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    if (!_BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(*endpoint_info_lcl)) {
                        /* If creating a non-accelerated endpoint, the relevant range for comparrison has less bits */
                        range          >>= (SOC_PPD_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START-SOC_PPD_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START);
                        new_disc_range >>= (SOC_PPD_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START-SOC_PPD_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START);
                    }
                    if (range != new_disc_range) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG("Error: BFD Your-Discriminator range should be constant for all endpoints. Existing value: %d"), range));
                    }
                }
                else {
                    soc_sand_rv = soc_ppd_oam_bfd_discriminator_range_registers_set(unit, new_disc_range);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
                /* Increase usage counter */
                ref_counter++;
                rv = BFD_ACCESS.YourDiscriminator_ref_count.set(unit, ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (_BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(*endpoint_info_lcl) &&
                ((endpoint_info_lcl->type == bcmBFDTunnelTypeMpls) || (endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) ||
                 (endpoint_info_lcl->type == bcmBFDTunnelTypePweControlWord) || (endpoint_info_lcl->type == bcmBFDTunnelTypePweTtl) ||
                 (endpoint_info_lcl->type == bcmBFDTunnelTypePweRouterAlert) ) ) {
                rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_PDU_REF_COUNTER, &ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_bfd_manage_static_registers(unit, endpoint_info_lcl, ref_counter, 0, 0);
                BCMDNX_IF_ERR_EXIT(rv);
                /*Update the counter only when we are sure an endpoint will be created*/
                if (endpoint_info_lcl->type == bcmBFDTunnelTypeMpls)  {
                    if ((endpoint_info_lcl->udp_src_port < _BCM_BFD_UDP_SRC_PORT_MIN) || (endpoint_info_lcl->udp_src_port > _BCM_BFD_UDP_SRC_PORT_MAX)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG("Error: udp_src_port value should be between %d and %d. Given value: %d"),
                                          _BCM_BFD_UDP_SRC_PORT_MIN, _BCM_BFD_UDP_SRC_PORT_MAX, endpoint_info_lcl->udp_src_port));
                    }

                    if ((endpoint_info_lcl->ip_ttl != 1)) {
                        /*TTL&TOS*/
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("Error: For BFD over MPLS supporting only ttl 1. Given value: %d"), endpoint_info_lcl->ip_ttl));
                    }

                    soc_sand_rv = soc_ppd_oam_bfd_mpls_udp_sport_get(unit, &udp_sport);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    if (udp_sport == 0) {
                        soc_sand_rv = soc_ppd_oam_bfd_mpls_udp_sport_set(unit, endpoint_info_lcl->udp_src_port);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    } else if (udp_sport != endpoint_info_lcl->udp_src_port) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG("Error: Supporting single MPLS udp_src_port value for device. Existing value: %d"), udp_sport));
                    }
                    rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_UDP_SPORT_REF_COUNTER, &mpls_udp_sport_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                    mpls_udp_sport_ref_counter++;
                    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_UDP_SPORT_REF_COUNTER, &mpls_udp_sport_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                } else if (endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) {
                    if ((endpoint_info_lcl->udp_src_port < _BCM_BFD_UDP_SRC_PORT_MIN) || (endpoint_info_lcl->udp_src_port > _BCM_BFD_UDP_SRC_PORT_MAX)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG("Error: udp_src_port value should be between %d and %d. Given value: %d"),
                                          _BCM_BFD_UDP_SRC_PORT_MIN, _BCM_BFD_UDP_SRC_PORT_MAX, endpoint_info_lcl->udp_src_port));
                    }
                    soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_get(unit, &udp_sport);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    if (udp_sport == 0) {
                        soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_set(unit, endpoint_info_lcl->udp_src_port);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    } else if (udp_sport != endpoint_info_lcl->udp_src_port) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG("Error: Supporting single IPv6 udp_src_port value for device. Existing value: %d"), udp_sport));
                    }
                    rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_UDP_SPORT_REF_COUNTER, &ipv4_udp_sport_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                    ipv4_udp_sport_ref_counter++;
                    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_UDP_SPORT_REF_COUNTER, &ipv4_udp_sport_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) == 0) {
                        if ((endpoint_info_lcl->ip_ttl != 255)) {
                            /*TTL&TOS*/
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                (_BSL_BCM_MSG("Error: For IP One Hop supporting only ttl 255. Given value: %d"), endpoint_info_lcl->ip_ttl));
                        }
						if (!is_1_hop_extended){
							if (endpoint_info_lcl->dst_ip_addr && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
									BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
													(_BSL_BCM_MSG("Error: For IP One Hop: dest ip address field should remain blank")));
							}
							if (SOC_IS_JERICHO(unit) &&  endpoint_info_lcl->ip_subnet_length >32) {
								BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
													(_BSL_BCM_MSG("Error: IP subnet length must be <=32"))); 
							} else if (SOC_IS_ARADPLUS_AND_BELOW(unit) &&  endpoint_info_lcl->ip_subnet_length ) {
								BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
													(_BSL_BCM_MSG("Error: IP subnet length unsopported."))); 
							}
						}
                    }
                }
                /*preumably after this an endpoint will be created, so we can now update the counter.*/
                ++ref_counter;
                rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_PDU_REF_COUNTER, &ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCc) {
                if (_BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(*endpoint_info_lcl)) {
                    rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_TP_CC_REF_COUNTER, &ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = _bcm_bfd_manage_static_registers(unit, endpoint_info_lcl, ref_counter, 1, 0);
                    BCMDNX_IF_ERR_EXIT(rv);
                    ref_counter++;
                    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_TP_CC_REF_COUNTER, &ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }

            LOG_VERBOSE(BSL_LS_BCM_BFD,
                        (BSL_META_U(unit,
                                    "BFD endpoint create: RX\n")));

            SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
            rv = _bcm_bfd_classifier_mep_entry_struct_set(unit, endpoint_info_lcl, &classifier_mep_entry, dip_ndx);
            BCMDNX_IF_ERR_EXIT(rv);

            if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET) {
                if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Device does not support remote OAMP")));
                }

                if (!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: In case of Server configuration BCM_BFD_ENDPOINT_IN_HW must be set.")));
                }

                /* Server: configuring only OAMP - add remote_gport trap code to OAMP traps list */
                if (!BCM_GPORT_IS_TRAP(endpoint_info_lcl->remote_gport)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: In case of Server configuration remote_gport must be valid trap code.")));
                }

                /* Allocating id:
                 *    If it's accelerated to the OAMP id should be taken from your discriminator LSBs
                 *    Else if it's accelerated to user destination it is allocated from the free range or can be set by the user,
                 *    Else, is taken from lif + indication bit that the endpoint is BFD
                 *    */
                flags = 0;
                if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
                    rv = _bcm_dpp_oam_bfd_mep_id_is_alloced(unit, endpoint_info_lcl->id);
                    if (rv == BCM_E_EXISTS) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                            (_BSL_BCM_MSG("Error: Local endpoint with id %d exists.\n"), endpoint_info_lcl->id));
                    } else if (rv != BCM_E_NOT_FOUND) {
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                    /* mep id given by the user */
                    flags = SHR_RES_ALLOC_WITH_ID;
                } else {
                    /* mep id callculated from the local_discr field */
                    endpoint_info_lcl->id = endpoint_info_lcl->local_discr & SOC_PPD_BFD_DISCRIMINATOR_TO_ACC_MEP_ID_START_MASK;

                    /* Checking same id does not collide with existig endpoints */
                    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry_temp);
                    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry_temp, &found);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (found) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error: Local endpoint with id %d exists."), endpoint_info_lcl->id));
                    }
                    flags = SHR_RES_ALLOC_WITH_ID;
                }
                rv = _bcm_dpp_bfd_mep_id_alloc(unit, flags, (uint32 *)&endpoint_info_lcl->id);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_SERVER_RX_TRAP, &server_trap_ref_counter, BCM_GPORT_TRAP_GET_ID(classifier_mep_entry.remote_gport));
                BCMDNX_IF_ERR_EXIT(rv);
                server_trap_ref_counter++;
                rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_SERVER_RX_TRAP, &server_trap_ref_counter, BCM_GPORT_TRAP_GET_ID(classifier_mep_entry.remote_gport));
                BCMDNX_IF_ERR_EXIT(rv);

                if (server_trap_ref_counter == 1) {
                    /* This trap code wasn't allocated yet */
                    soc_sand_rv = soc_ppd_oam_oamp_rx_trap_codes_set(unit, classifier_mep_entry.mep_type, BCM_GPORT_TRAP_GET_ID(endpoint_info_lcl->remote_gport));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }

            }
            else {
                /* Endpoint ID validity check */
                if (!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW)) {
                    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unaccelerated endpoint can not be created WITH_ID")));
                    }

                    /* Validate that mep with same lif does not exist */
                    oem_key = (classifier_mep_entry.lif==_BCM_OAM_INVALID_LIF) ? classifier_mep_entry.your_discriminator : classifier_mep_entry.lif;
                    soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(
                    unit, oem_key, 0/*mdlevel*/, 0/*is_upmep*/, &found, &profile_temp, &found_profile, &is_mp_type_flexible_dummy, &is_mip_dimmy);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    if (found) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                            (_BSL_BCM_MSG("Error: Local endpoint with lif/local_discr %d exists.\n"), oem_key));
                    }
                }

                /* Allocating id:
                 *    If it's accelerated to the OAMP id should be taken from your discriminator LSBs
                 *    Else if it's accelerated to user destination it is allocated from the free range or can be set by the user,
                 *    Else, is taken from lif + indication bit that the endpoint is BFD
                 *    */
                flags = 0;
                if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) {
                     if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
                        rv = _bcm_dpp_oam_bfd_mep_id_is_alloced(unit, endpoint_info_lcl->id);
                        if (rv == BCM_E_EXISTS) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                     (_BSL_BCM_MSG("Error: Local endpoint with id %d exists.\n"), endpoint_info_lcl->id));
                        }
                        else if (rv != BCM_E_NOT_FOUND) {
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                        /* mep id given by the user */
                        flags = SHR_RES_ALLOC_WITH_ID;
                    }
                    else {
                        if (endpoint_info_lcl->remote_gport == BCM_GPORT_INVALID) {
                            endpoint_info_lcl->id = endpoint_info_lcl->local_discr & SOC_PPD_BFD_DISCRIMINATOR_TO_ACC_MEP_ID_START_MASK;

                            /* Checking same id does not collide with existig endpoints */
                            SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry_temp);
                            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry_temp, &found);
                            BCMDNX_IF_ERR_EXIT(rv);
                            if (found) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error: Local endpoint with id %d exists."), endpoint_info_lcl->id));
                            }
                            /* mep id callculated from the local_discr field */
                            flags = SHR_RES_ALLOC_WITH_ID;
                        }
                    }
                    rv = _bcm_dpp_bfd_mep_id_alloc(unit, flags, (uint32*)&endpoint_info_lcl->id);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                else {
                    /* Can not be created WITH_ID - checked already */
                    if (classifier_mep_entry.lif==_BCM_OAM_INVALID_LIF) {
                        endpoint_info_lcl->id = _BCM_BFD_UNACCELERATED_MEP_INDEX_GET(classifier_mep_entry.your_discriminator);
                    }
                    else {
                        endpoint_info_lcl->id = _BCM_BFD_UNACCELERATED_MEP_INDEX_GET(classifier_mep_entry.lif);
                    }
                }

                /*in case of BFD echo or bfd ipv4 single hop extended mode we don't use The OAM classification the PMF does it*/
                if (!(endpoint_info_lcl->flags & BCM_BFD_ECHO) && (is_1_hop_extended ==0))
				 {
                    SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data);
                    SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data_acc_dummy);

                    if (BCM_GPORT_IS_TRAP(endpoint_info_lcl->remote_gport)) {
                        /* Use destination set to a pre defined trap code*/
                        rv = _bcm_dpp_oam_bfd_trap_code_from_remote_gport(unit,endpoint_info_lcl->remote_gport,&trap_code, TRUE /*META data*/, &classifier_mep_entry);
                        BCMDNX_IF_ERR_EXIT(rv);
                        LOG_VERBOSE(BSL_LS_BCM_BFD,
                                    (BSL_META_U(unit,
                                                "Creating BFD endpoint with user defined trap code %d \n"), trap_code));
                        /* trap set by user - no need to update the ref count.*/
                    } else  if (BCM_GPORT_IS_SYSTEM_PORT(endpoint_info_lcl->remote_gport)  || BCM_GPORT_IS_MODPORT(endpoint_info_lcl->remote_gport) ||
                                BCM_GPORT_IS_LOCAL(endpoint_info_lcl->remote_gport)) {
                        /* use user defined destination.*/
                        rv = _bcm_dpp_oam_bfd_trap_code_from_remote_gport(unit, endpoint_info_lcl->remote_gport, &trap_code, TRUE/*meta_data*/, &classifier_mep_entry);
                        BCMDNX_IF_ERR_EXIT(rv);
                        rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_code);
                        BCMDNX_IF_ERR_EXIT(rv);

                    } else if (endpoint_info_lcl->remote_gport == BCM_GPORT_INVALID) {
                        /* use default destinations*/
                        int trap_code_index;
                        if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) {
                            trap_code_index = (classifier_mep_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP ||
                                                   classifier_mep_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP)?  SOC_PPD_BFD_TRAP_ID_OAMP_IPV4 :
                                (classifier_mep_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS)? SOC_PPD_BFD_TRAP_ID_OAMP_MPLS :
                                (classifier_mep_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE)? SOC_PPD_BFD_TRAP_ID_OAMP_PWE :
                                (classifier_mep_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE_GAL)? SOC_PPD_BFD_TRAP_ID_OAMP_PWE :
                                /* Only conceivable option left (otherwise an error would have already been printed)*/ SOC_PPD_BFD_TRAP_ID_OAMP_CC_MPLS_TP ;
                                if (SOC_IS_ARADPLUS(unit) &&  (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP)) {
                                        /* Override with PWE (ARAD+ workaround)*/
                                        trap_code_index = SOC_PPD_BFD_TRAP_ID_OAMP_PWE;
                                }
                        } else {
                            /* default destination: CPU port.*/
                            trap_code_index = SOC_PPD_BFD_TRAP_ID_CPU;
                        }

                        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_INIT_TRAP_INFO_TRAP_IDS, &trap_code, trap_code_index);
                        BCMDNX_IF_ERR_EXIT(rv);
                    } else {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: remote_gport must be a system port, mod-port, invalid port, or trap code.")));
                    }
                    /* Trap_code is gotten above. Insert into mep_profile_data*/
                    profile_data.mep_profile_data.opcode_to_trap_code_multicast_map[internal_bfd_opcode] = trap_code; /* No unicast in BFD */


                    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) {
                        rv = _bcm_dpp_am_template_bfd_mep_profile_accelerated_alloc(unit, 0 /*flags*/, &profile_data, &is_allocated, &profile);
                        BCMDNX_IF_ERR_EXIT(rv);

                        classifier_mep_entry.acc_profile = profile;
                    }
                    else {
                        rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_alloc(unit, 0 /*flags*/, &profile_data, &is_allocated, &profile);
                        BCMDNX_IF_ERR_EXIT(rv);

                        classifier_mep_entry.non_acc_profile = (uint8)profile;
                    }
                    if (is_allocated) {
                        rv = _bcm_dpp_oam_mep_profile_trap_data_alloc(unit, &profile_data.mep_profile_data);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }

                    soc_sand_rv = soc_ppd_oam_classifier_oem_mep_add(unit, endpoint_info_lcl->id, &classifier_mep_entry, 0/*update*/);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    soc_sand_rv = soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile(
					unit,0, &classifier_mep_entry, &profile_data, &profile_data_acc_dummy, trap_code_to_mp_map_dummy);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
                        /* IP multi hop - set the DIP table.*/
                        /* BFD DIP table compares against packet.DIP, which should equal the endpoint's SIP.*/
                        /* BFD DIP table stores IPv6 addresses, which may be IPv4 addresses. convert IPv4 to default IPv6 extension.*/
                        SOC_SAND_PP_IPV6_ADDRESS ipv6_addr;

                        _BCM_BFD_IPV4_ADDR_TO_DEFAULT_IPV6_EXTENSION_ADDR(endpoint_info_lcl->src_ip_addr,ipv6_addr);
                        rv = _bcm_dpp_am_template_bfd_ip_dip_alloc(unit, 0 /*f  lags*/, &ipv6_addr, &is_allocated, &dip_ndx);
                        BCMDNX_IF_ERR_EXIT(rv);
                        if (is_allocated) {
                            soc_sand_rv = soc_ppd_oam_bfd_my_bfd_dip_ip_set(unit, dip_ndx, &ipv6_addr);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                    }

                    /* RX end */
				} else {
					/*if bfd ipv4 single hop extended we use the LEM instead of the classifier*/
					/*accelerated*/
					if ((is_1_hop_extended == 1) && (is_default == 0) && !(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_UPDATE)) {
						rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_bfd_one_hop_lem_entry_add, (unit, 0, endpoint_info_lcl->local_discr, endpoint_info_lcl->id, 1));
						BCMDNX_IF_ERR_EXIT(rv);
						classifier_mep_entry.ma_index = endpoint_info_lcl->local_discr; /*in single hop the ma_index will save the local discriminator. 
																					This will be used for deleting the entry from the LEM*/
					}
					/* non accelerated*/
					if ((is_1_hop_extended == 2) && (is_default == 0) && !(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_UPDATE)) {
						rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_bfd_one_hop_lem_entry_add, (unit, 0, endpoint_info_lcl->local_discr, endpoint_info_lcl->id, 0));
						BCMDNX_IF_ERR_EXIT(rv);
						classifier_mep_entry.ma_index = endpoint_info_lcl->local_discr; /*in single hop the ma_index will save the local discriminator. 
																					This will be used for deleting the entry from the LEM*/
					}
				}
            }

            if (_BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(*endpoint_info_lcl) ) {
                /* Accelerated to OAMP - Inserting entries to OAMP dbs */
                SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
                
                

                /*
                * MEP DB entry
                */
                LOG_VERBOSE(BSL_LS_BCM_BFD,
                                        (BSL_META_U(unit,
                                                    "BFD endpoint create: TX\n")));

                SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

                /* inserting an entry to the lem- the key is the my_discriminator the payload is the counter ID*/
                if (SOC_DPP_CONFIG(unit)->pp.oam_statistics == 1) {
                    rv =  _bcm_bfd_stat_lem_entry_add(unit, endpoint_info_lcl);
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                rv = _bcm_bfd_mep_db_entry_struct_set(unit, endpoint_info_lcl, &mep_db_entry);
                BCMDNX_IF_ERR_EXIT(rv);
                /*set entry in mep db*/
                soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info_lcl->id, &mep_db_entry, 0, name);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /*
                * RMEP DB entry
                */

                LOG_VERBOSE(BSL_LS_BCM_BFD,
                                        (BSL_META_U(unit,
                                                    "Inserting remote entry.\n")));

                SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);
                rv = _bcm_bfd_rmep_db_entry_struct_set(unit, endpoint_info_lcl, &rmep_db_entry);
                BCMDNX_IF_ERR_EXIT(rv);

                if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_REMOTE_WITH_ID) {
                    if (!_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint_info_lcl->remote_id)) {
                        _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(endpoint_info_lcl->remote_id, rmep_index);
                    }
                    else {
                        rmep_index = endpoint_info_lcl->remote_id;
                    }

                    rv = bcm_dpp_am_oam_rmep_id_is_alloced(unit, rmep_index);
                    if (rv == BCM_E_EXISTS) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                 (_BSL_BCM_MSG("Error: Remote endpoint with id %d exists.\n"), endpoint_info_lcl->remote_id));
                    }
                    else if (rv != BCM_E_NOT_FOUND) {
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                    /* mep id given by the user */
                    flags = SHR_RES_ALLOC_WITH_ID;
                }
                else {
                    flags = 0;
                }

                rv = bcm_dpp_am_oam_rmep_id_alloc(unit, flags, &rmep_index);
                BCMDNX_IF_ERR_EXIT(rv);
                new_rmep_id_alloced = TRUE;
                endpoint_info_lcl->remote_id = rmep_index;

                if ((endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCcCv)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Accelerated bcmBFDTunnelTypeMplsTpCcCv is not supported.")));
                }

                SOC_PPD_OAM_RMEP_INFO_DATA_clear(&rmep_info);
                rmep_info.mep_index = endpoint_info_lcl->id;
                rv = _bcm_dpp_oam_bfd_rmep_info_db_insert(unit, rmep_index, &rmep_info);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Update entry in rmep db and rmep index db*/
                _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_index_internal, rmep_index);
                soc_sand_rv = soc_ppd_oam_oamp_rmep_set(unit, rmep_index_internal, 0, endpoint_info_lcl->id, mep_db_entry.mep_type, &rmep_db_entry, 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            }
            else {
                /* in case there is no entry in RMEP DB we still want to indicate the existance of a mep*/
                rmep_index = (uint32)(-1);
            }
            /*update sw db of mep->rmep with new mep */
            rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(unit, endpoint_info_lcl->id, rmep_index);
            BCMDNX_IF_ERR_EXIT(rv);

            /*update sw db mep_info with new mep */
            rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_oam_endpoint_list_member_add(unit, &_bcm_bfd_endpoint_list[unit], endpoint_info_lcl->id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }



    BCM_EXIT;
exit:
    sal_memcpy(endpoint_info, endpoint_info_lcl, sizeof(bcm_bfd_endpoint_info_t));
    /* if the use_hw_id soc property is enabled, then the trap_id is the SW id and thus we need to convert it to the HW id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP((endpoint_info->remote_gport)))
    {
        _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit, endpoint_info->remote_gport);
    }
    if ((SOC_SAND_FAILURE(soc_sand_rv) || (BCM_FAILURE(rv))) && new_rmep_id_alloced) {
        /* If error after new id was alloced we should free it */
        if (rmep_index != (uint32)(-1)) {
            rv = bcm_dpp_am_oam_rmep_id_dealloc(unit, rmep_index);
        }
    }

    
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_bfd_endpoint_get(
   int unit,
   bcm_bfd_endpoint_t endpoint,
   bcm_bfd_endpoint_info_t *endpoint_info) {
    uint32 soc_sand_rv;
    int rv;
    SOC_PPD_BFD_PDU_STATIC_REGISTER bfd_pdu;
    SOC_PPD_BFD_PDU_STATIC_REGISTER *bfd_pdu_to_update;
    SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER bfd_cc_packet;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
    uint8 found;
    uint16 udp_src_port;
    uint32 range;
    int dip_ndx;
    uint8 is_bfd_init;
    uint32 rmep_index_internal;
    endpoint_list_t *rmep_list_p = NULL;
    uint8 is_default;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        if (_BCM_BFD_MEP_INDEX_IS_UC(endpoint)) {
            bcm_bfd_endpoint_t id_internal;

            _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(id_internal, endpoint);
            rv = bcm_petra_bfd_uc_endpoint_get(unit, id_internal, endpoint_info);
            BCMDNX_IF_ERR_EXIT(rv);
            _BCM_BFD_UC_MEP_INDEX_FROM_INTERNAL(id_internal, endpoint_info->id);
            
            return rv;
        }
    }       

    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    endpoint_info->id = endpoint;

    is_default = _BCM_BFD_IS_MEP_ID_DEFAULT(unit, endpoint);

    /* Default BFD trap */
    if (is_default) {
        int sw_db_epid = endpoint;
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            /* Arad B1 and below create a default endpoint with id=-1 but save it in the SW DB
               with key (id) = -2 to differentiate from OAM default endpoint */
            sw_db_epid = -2;
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, sw_db_epid, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Default profile not configured.\n")));
        }

        /* Get remote_gport */
        endpoint_info->remote_gport = classifier_mep_entry.remote_gport;

        BCM_EXIT;
    }

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found."), endpoint));
    }

	SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

    if (((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) && 
        ((classifier_mep_entry.remote_gport == BCM_GPORT_INVALID) || (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER))) {
        /* OAMP MEP DB */
        /*get entry of mep db*/
        
        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint, &mep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_bfd_mep_db_entry_struct_get(unit, endpoint_info, &mep_db_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        /* get rmep id from sw db of mep->rmep */
        rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint, &rmep_list_p, &found);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found."), endpoint));
        }

        endpoint_info->remote_id = rmep_list_p->first_member->index;
        _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_index_internal, rmep_list_p->first_member->index);
		soc_sand_rv = soc_ppd_oam_oamp_rmep_get(unit, rmep_index_internal, &rmep_db_entry);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_bfd_rmep_db_entry_struct_get(unit, &rmep_db_entry, endpoint_info); 
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*RX - classifier*/
    rv = _bcm_bfd_classifier_mep_entry_struct_get(unit, endpoint_info, &classifier_mep_entry, &dip_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    if (_BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(*endpoint_info) ||
        (_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info->type))) {
        soc_sand_rv = soc_ppd_oam_bfd_discriminator_range_registers_get(unit, &range);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* When this line is reached, the value in endpoint_info->local_discr is taken from the sw db holding the
           classifier_mep_entry, which contains the 16 LSB of the local_discr, that the endpoint was created with.
           The range is composed of the 19 MSB of the same value (18 MSB in Jericho), so need to truncate accordingly. */
        range >>= (SOC_PPD_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START-SOC_PPD_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START);
        endpoint_info->local_discr |= range << SOC_PPD_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START;
    }

    if (endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW && (classifier_mep_entry.remote_gport == BCM_GPORT_INVALID))  {
        if ((endpoint_info->type == bcmBFDTunnelTypeMplsTpCc) && (SOC_IS_ARAD_B1_AND_BELOW(unit))) {
            /* If type is MPLSTP in ARAD+ it is set into OAMP as PWE */
            SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER_clear(&bfd_cc_packet);
            soc_sand_rv = soc_ppd_oam_bfd_cc_packet_static_register_get(unit, &bfd_cc_packet);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            endpoint_info->local_discr =  bfd_cc_packet.bfd_my_discr;
            endpoint_info->remote_discr = bfd_cc_packet.bfd_your_discr;
            bfd_pdu_to_update = &(bfd_cc_packet.bfd_static_reg_fields);
        } else {
            SOC_PPD_BFD_PDU_STATIC_REGISTER_clear(&bfd_pdu);
            soc_sand_rv = soc_ppd_oam_bfd_pdu_static_register_get(unit, &bfd_pdu);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            bfd_pdu_to_update = &(bfd_pdu);
        }

        endpoint_info->local_min_echo = bfd_pdu_to_update->bfd_req_min_echo_rx_interval;

		if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            endpoint_info->local_diag = bfd_pdu_to_update->bfd_diag;
            endpoint_info->local_flags = bfd_pdu_to_update->bfd_flags; /* These are only C,A,D,M flags. P and F are added from the mep db */
            endpoint_info->local_state = bfd_pdu_to_update->bfd_sta;
		}
    }

    if (endpoint_info->type == bcmBFDTunnelTypeMpls) {
        soc_sand_rv = soc_ppd_oam_bfd_mpls_udp_sport_get(unit, &udp_src_port);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        endpoint_info->udp_src_port = udp_src_port;
    } else if (endpoint_info->type == bcmBFDTunnelTypeUdp) {
        soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_get(unit, &udp_src_port);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        endpoint_info->udp_src_port = udp_src_port;

        if ((endpoint_info->flags & (BCM_BFD_ENDPOINT_MULTIHOP | BCM_BFD_ENDPOINT_HW_ACCELERATION_SET | BCM_BFD_ECHO))
              == BCM_BFD_ENDPOINT_MULTIHOP  ) {
            /* Strictly multi hop. multi hop*/
            SOC_SAND_PP_IPV6_ADDRESS ipv6_addr;

            rv = _bcm_dpp_am_template_bfd_ip_dip_data_get(unit, dip_ndx, &ipv6_addr);
            BCMDNX_IF_ERR_EXIT(rv);

            endpoint_info->src_ip_addr = ipv6_addr.address[0];
        }
    }

    BCM_EXIT;
exit:
    /* if the use_hw_id soc property is enabled, then the trap_id is the SW id and thus we need to convert it to the HW id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP(endpoint_info->remote_gport))
    {
        _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit,endpoint_info->remote_gport);
    }
    BCMDNX_FUNC_RETURN;

}

int bcm_petra_bfd_endpoint_destroy(
   int unit,
   bcm_bfd_endpoint_t endpoint) {

    uint32 soc_sand_rv;
    int rv;
    endpoint_list_t *rmep_list_p = NULL;
    uint8 found;
    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    int is_last;
    uint32 trap_code;
    uint32 rmep_index_internal;
    uint32 mpls_udp_sport_ref_counter;
    uint32 mpls_cc_ref_counter;
    uint32 ipv4_udp_sport_ref_counter;
    uint32 pdu_ref_counter;
    int ref_counter;
    uint8 is_bfd_init;
    uint8 no_traps;
    SOC_PPD_OAM_LIF_PROFILE_DATA profile_data;
    uint32 server_trap_ref_counter;
    uint8 is_default;
	int is_single_hop_extended=0;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
    int trap_code_converted;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    /* Destroy the endpoint that created on ukernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        if (_BCM_BFD_MEP_INDEX_IS_UC(endpoint)) {
            bcm_bfd_endpoint_t id_internal;

            _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(id_internal, endpoint);
            return bcm_petra_bfd_uc_endpoint_destroy(unit, id_internal);
        }
    }     

    is_default = _BCM_BFD_IS_MEP_ID_DEFAULT(unit, endpoint);
    SOC_PPD_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

    if (is_default) {
        /* Default BFD trap */
        int sw_db_epid = endpoint;
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            sw_db_epid = -2;
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, sw_db_epid, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                                (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
        }

        if ((SOC_IS_ARAD_B1_AND_BELOW(unit)) && (classifier_mep_entry.non_acc_profile != SOC_PPD_OAM_PROFILE_DEFAULT)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                (_BSL_BCM_MSG("Error: Internal error in default profile configuration.\n")));
        }

        /* Free profile */
        rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_data_get(unit, endpoint, &profile_data);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_free(unit, classifier_mep_entry.non_acc_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_last) {
            rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data.mep_profile_data, 1/*is_mep*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (classifier_mep_entry.remote_gport != BCM_GPORT_INVALID) {
            rv = _bcm_dpp_oam_bfd_trap_code_from_remote_gport(unit, classifier_mep_entry.remote_gport, &trap_code, TRUE/*meta_data*/, &classifier_mep_entry);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_oam_traps_ref_counter_decrease(unit, trap_code);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_dpp_oam_traps_ref_counter_is_zero(unit, trap_code, &no_traps);
            if (no_traps) { 

                BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,
                                      trap_code, 
                                      &trap_code_converted));

                rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        if (SOC_IS_ARADPLUS(unit)) {
            rv = _bcm_bfd_default_id_from_ep_id(unit, endpoint, &default_id);
            BCMDNX_IF_ERR_EXIT(rv);

            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_remove,
                                                                                    (unit, default_id));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, sw_db_epid);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);

	is_single_hop_extended =((SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop == 1) && classifier_mep_entry.mep_type==SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found."), endpoint));
    }

    if (((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) && 
        ((classifier_mep_entry.remote_gport == BCM_GPORT_INVALID) || (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER))) {
        /* delete OAMP MEP DB entry */

        /* deallocate all the allocated pointers */

        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint, &mep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry.mep_type))  {
            int ignored;
            rv =  _bcm_dpp_am_template_oam_local_port_2_sys_port_free(unit, endpoint, &ignored);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = _bcm_mep_db_entry_dealloc(unit, &mep_db_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_delete(unit, endpoint, 0, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* delete remote endpoint entry */
        rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint, &rmep_list_p, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found."), endpoint));
        }

        _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_index_internal, rmep_list_p->first_member->index);
        soc_sand_rv = soc_ppd_oam_oamp_rmep_delete(unit, rmep_index_internal, 0, endpoint, mep_db_entry.mep_type);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_dpp_oam_bfd_rmep_info_db_delete(unit, rmep_list_p->first_member->index);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_dpp_am_oam_rmep_id_dealloc(unit, rmep_list_p->first_member->index);
        BCMDNX_IF_ERR_EXIT(rv);

        if (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER) {
            /* Manage OAMP supported traps */
            rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_SERVER_RX_TRAP, &server_trap_ref_counter, BCM_GPORT_TRAP_GET_ID(classifier_mep_entry.remote_gport));
            BCMDNX_IF_ERR_EXIT(rv);
            server_trap_ref_counter--;
            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAMP_SERVER_RX_TRAP, &server_trap_ref_counter, BCM_GPORT_TRAP_GET_ID(classifier_mep_entry.remote_gport));
            BCMDNX_IF_ERR_EXIT(rv);

            if (!server_trap_ref_counter) {
                soc_sand_rv = soc_ppd_oam_oamp_rx_trap_codes_delete(unit, classifier_mep_entry.mep_type, BCM_GPORT_TRAP_GET_ID(classifier_mep_entry.remote_gport));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }

    }

    if ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) {
        rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, endpoint);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*RX - classifier*/

	if ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) && (classifier_mep_entry.remote_gport == BCM_GPORT_INVALID)) {
		if ((classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) ||
			(classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP)) {

			rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_UDP_SPORT_REF_COUNTER, &ipv4_udp_sport_ref_counter);
			BCMDNX_IF_ERR_EXIT(rv);
			ipv4_udp_sport_ref_counter--;
			rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_IPV4_UDP_SPORT_REF_COUNTER, &ipv4_udp_sport_ref_counter);
			BCMDNX_IF_ERR_EXIT(rv);
			if (ipv4_udp_sport_ref_counter == 0) {
				soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_set(unit, 0);
				BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			}

		} else if (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS) {
			rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_UDP_SPORT_REF_COUNTER, &mpls_udp_sport_ref_counter);
			BCMDNX_IF_ERR_EXIT(rv);
			mpls_udp_sport_ref_counter--;
			rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_UDP_SPORT_REF_COUNTER, &mpls_udp_sport_ref_counter);
			BCMDNX_IF_ERR_EXIT(rv);
			if (mpls_udp_sport_ref_counter == 0) {
				soc_sand_rv = soc_ppd_oam_bfd_mpls_udp_sport_set(unit, 0);
				BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			}
		} else if ((classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFDCC_O_MPLSTP)) {
			if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
				rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_TP_CC_REF_COUNTER, &mpls_cc_ref_counter);
				BCMDNX_IF_ERR_EXIT(rv);
				mpls_cc_ref_counter--;
				rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_MPLS_TP_CC_REF_COUNTER, &mpls_cc_ref_counter);
				BCMDNX_IF_ERR_EXIT(rv);
				if (mpls_cc_ref_counter == 0) {
					SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER bfd_cc_packet;
					SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER_clear(&bfd_cc_packet);
					soc_sand_rv = soc_ppd_oam_bfd_cc_packet_static_register_set(unit, &bfd_cc_packet);
				}
			}
		}
		if ((classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_MPLS) || (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) ||
			(classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) || (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE) ||
            (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_PWE_GAL)) {
			rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_PDU_REF_COUNTER, &pdu_ref_counter);
			BCMDNX_IF_ERR_EXIT(rv);
			pdu_ref_counter--;
			rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_BFD_PDU_REF_COUNTER, &pdu_ref_counter);
			BCMDNX_IF_ERR_EXIT(rv);
			if (pdu_ref_counter == 0) {
				SOC_PPD_BFD_PDU_STATIC_REGISTER bfd_pdu;
				SOC_PPD_BFD_PDU_STATIC_REGISTER_clear(&bfd_pdu);
				soc_sand_rv = soc_ppd_oam_bfd_pdu_static_register_get(unit, &bfd_pdu);
				BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

				bfd_pdu.bfd_diag = 0;
				bfd_pdu.bfd_flags = 1;
				bfd_pdu.bfd_req_min_echo_rx_interval = 0;
				bfd_pdu.bfd_sta = 3; /* UP */

				soc_sand_rv = soc_ppd_oam_bfd_pdu_static_register_set(unit, &bfd_pdu);
				BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			}
		}
	}

    if ((classifier_mep_entry.remote_gport != BCM_GPORT_INVALID) && !(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER)) {
        rv = _bcm_dpp_oam_bfd_trap_code_from_remote_gport(unit, classifier_mep_entry.remote_gport, &trap_code, TRUE/*meta_data*/, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_oam_traps_ref_counter_decrease(unit, trap_code);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_oam_traps_ref_counter_is_zero(unit, trap_code, &no_traps);
        if (no_traps) {

            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,
                                  trap_code, 
                                  &trap_code_converted));
            rv = bcm_petra_rx_trap_type_destroy(unit, trap_code);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }


    if ((!(classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER))
		&& (mep_db_entry.diag_profile!=0xf /* diag profile will be 0xf only for BFD ECHO.*/) 
		&& !is_single_hop_extended){

		/* Free profile */
		if ((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) == 0) {
			SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data);
			rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_data_get(unit, endpoint, &profile_data);
			BCMDNX_IF_ERR_EXIT(rv);

			rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_free(unit, classifier_mep_entry.non_acc_profile, &is_last);
			BCMDNX_IF_ERR_EXIT(rv);
		} else {
			SOC_PPD_OAM_LIF_PROFILE_DATA_clear(&profile_data);
			rv = _bcm_dpp_am_template_bfd_mep_profile_accelerated_data_get(unit, endpoint, &profile_data);
			BCMDNX_IF_ERR_EXIT(rv);

			rv = _bcm_dpp_am_template_bfd_mep_profile_accelerated_free(unit, classifier_mep_entry.acc_profile, &is_last);
			BCMDNX_IF_ERR_EXIT(rv);
		}
		if (is_last) {
			/* Accelerated endpoint profile should be freed only if destination is remote, otherwise it will reset hardcoded traps */
			if (((classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) == 0) ||
				(classifier_mep_entry.remote_gport != BCM_GPORT_INVALID)) {
				rv = _bcm_dpp_oam_mep_profile_trap_data_free(unit, &profile_data.mep_profile_data, 1/*is_mep*/);
				BCMDNX_IF_ERR_EXIT(rv);
			}
		}

		soc_sand_rv = soc_ppd_oam_classifier_mep_delete(unit, endpoint, &classifier_mep_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);



        /* Multicast DIP - supporting only 16 */
        if (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
            rv = _bcm_dpp_am_template_bfd_ip_dip_free(unit, classifier_mep_entry.dip_profile_ndx, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_last) {
                SOC_SAND_PP_IPV6_ADDRESS ipv6_addr;
                soc_sand_SAND_PP_IPV6_ADDRESS_clear(&ipv6_addr); 

                soc_sand_rv = soc_ppd_oam_bfd_my_bfd_dip_ip_set(unit, classifier_mep_entry.dip_profile_ndx, &ipv6_addr);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }

    }

    /* Range is set if _BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(*endpoint_info) ||
        _BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info->type)
       This is equivalent to the following condition over the classifier entry*/
    if ( ( (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)
           && ( (classifier_mep_entry.remote_gport == BCM_GPORT_INVALID)
                || (classifier_mep_entry.flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER) ) )
         || ( (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP)
              || (classifier_mep_entry.mep_type == SOC_PPD_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP)) )
 {
        rv = BFD_ACCESS.YourDiscriminator_ref_count.get(unit, &ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
        --ref_counter;
        rv = BFD_ACCESS.YourDiscriminator_ref_count.set(unit, ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_mep_delete(unit, endpoint);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint);
    BCMDNX_IF_ERR_EXIT(rv);


    rv = _bcm_dpp_oam_endpoint_list_member_find(unit, &_bcm_bfd_endpoint_list[unit], endpoint, 1, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (_bcm_dpp_oam_endpoint_list_empty(unit, &_bcm_bfd_endpoint_list[unit])) {
        soc_sand_rv = soc_ppd_oam_bfd_discriminator_range_registers_set(unit, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

	/*delete entry from LEM in case this is a bfd ipv4 single hop*/
	if (is_single_hop_extended)/*sigle hop extended mode*/ {
		rv= MBCM_PP_DRIVER_CALL(unit, mbcm_pp_bfd_one_hop_lem_entry_remove, (unit, classifier_mep_entry.ma_index)); 
		BCMDNX_IF_ERR_EXIT(rv);
	}

	
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_bfd_endpoint_destroy_all(int unit) {
    endpoint_list_member_t *mep_list_p = NULL;
    int rv;
    uint8 is_bfd_init;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_EXIT;
    }

    /* Destroy all the endpoint that created on ukernel on matter it is exists or not*/
    if (bcm_petra_bfd_uc_is_init(unit)) {   
        rv = bcm_petra_bfd_uc_endpoint_destroy_all(unit);   
        BCMDNX_IF_ERR_EXIT(rv);
    }    

    if (_bcm_dpp_oam_endpoint_list_empty(unit, &_bcm_bfd_endpoint_list[unit])) {
        BCM_EXIT;
    }

    mep_list_p = _bcm_bfd_endpoint_list[unit].first_member;

    while (mep_list_p != NULL) {
        rv = bcm_petra_bfd_endpoint_destroy(unit, mep_list_p->index);
        BCMDNX_IF_ERR_EXIT(rv);
        mep_list_p = mep_list_p->next;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_endpoint_poll
 * Purpose:
 *      Poll an BFD endpoint object.  Valid only for endpoint in Demand
 *      Mode and in state bcmBFDStateUp.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to poll.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND BFD Session ID not in use
 *      BCM_E_INTERNAL  Unable to read or write resource
 *      BCM_E_PARAM     Session not in Demand Mode/
 *                      Session not in UP state
 * Notes: The poll operation will be initiated.  Poll failure will be
 * signalled via asynchronous callback (aka session failure)
 */
int
bcm_petra_bfd_endpoint_poll(int unit, bcm_bfd_endpoint_t endpoint)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        if (_BCM_BFD_MEP_INDEX_IS_UC(endpoint)) {
            bcm_bfd_endpoint_t id_internal;

            _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(id_internal, endpoint);
            rv = bcm_petra_bfd_uc_endpoint_poll(unit, id_internal);
            BCMDNX_IF_ERR_EXIT(rv);
            
            return rv;
        }
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_event_register
 * Purpose:
 *      Register a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback will be invoked.
 *      cb          - (IN) A pointer to the callback function.
 *      user_data   - (IN) Pointer to user data to pass to the callback.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
int
bcm_petra_bfd_event_register(int unit,
                             bcm_bfd_event_types_t event_types,
                             bcm_bfd_event_cb cb, void *user_data)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Check whether to register the events to ukernel*/
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_event_register(unit, event_types, cb, user_data);
        BCMDNX_IF_ERR_EXIT(rv);  
    } 

    rv = _bcm_petra_bfd_event_register(unit, event_types, cb, user_data); 
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_event_unregister
 * Purpose:
 *      Unregister a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback should not be invoked.
 *      cb          - (IN) A pointer to the callback function.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
int
bcm_petra_bfd_event_unregister(int unit,
                               bcm_bfd_event_types_t event_types,
                               bcm_bfd_event_cb cb)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Check whether to register the events to ukernel*/
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_event_unregister(unit, event_types, cb);
        BCMDNX_IF_ERR_EXIT(rv);   

        return rv;
    } 

    rv = _bcm_petra_bfd_event_unregister(unit, event_types, cb);
    BCMDNX_IF_ERR_EXIT(rv); 
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_endpoint_stat_get
 * Purpose:
 *      Get BFD endpoint statistics.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get stats for.
 *      ctr_info - (IN/OUT) Pointer to endpoint count structure to receive
 *                 the data.
 *      clear    - (IN) If set, clear stats after read.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to counter information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_endpoint_stat_get(int unit,
                                bcm_bfd_endpoint_t endpoint,
                                bcm_bfd_endpoint_stat_t *ctr_info, uint8 clear)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        if (_BCM_BFD_MEP_INDEX_IS_UC(endpoint)) {
            bcm_bfd_endpoint_t id_internal;

            _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(id_internal, endpoint);
            rv = bcm_petra_bfd_uc_endpoint_stat_get(unit, id_internal, ctr_info, clear);
            BCMDNX_IF_ERR_EXIT(rv);            
        }
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_auth_sha1_set
 * Purpose:
 *      Set SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to configure.
 *      sha1  - (IN) Pointer to SHA1 info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_auth_sha1_set(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_auth_sha1_set(unit, index, sha1);
        BCMDNX_IF_ERR_EXIT(rv);        
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_bfd_auth_sha1_get
 * Purpose:
 *      Get SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to retrieve.
 *      sha1  - (IN/OUT) Pointer to SHA1 info structure to receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_auth_sha1_get(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_auth_sha1_get(unit, index, sha1);
        BCMDNX_IF_ERR_EXIT(rv);        
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_auth_simple_password_set
 * Purpose:
 *      Set Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to configure.
 *      sp    - (IN) Pointer to Simple Password info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_auth_simple_password_set(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_auth_simple_password_set(unit, index, sp);
        BCMDNX_IF_ERR_EXIT(rv);        
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_auth_simple_password_get
 * Purpose:
 *      Get Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to retrieve.
 *      sp    - (IN/OUT) Pointer to Simple Password info structure to
 *              receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_auth_simple_password_get(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_auth_simple_password_get(unit, index, sp);
        BCMDNX_IF_ERR_EXIT(rv);        
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* Clear all BFD (used in debug) */
int bcm_petra_bfd_clear(int unit) {

    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint8 oam_is_init, is_bfd_init;
	int diag_ind;
    int trap_code_converted;
    int trap_id_sw;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_EXIT;
    }
    rv = bcm_petra_bfd_endpoint_destroy_all(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        rv = _bcm_dpp_oam_dbs_destroy(unit, FALSE/*is_oam*/);
        BCMDNX_IF_ERR_EXIT(rv);

        trap_id_sw = _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_CPU];
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    trap_id_sw = _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_IPV4];   
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted );
    BCMDNX_IF_ERR_EXIT(rv);

    trap_id_sw = _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_MPLS];   
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
    BCMDNX_IF_ERR_EXIT(rv);

    trap_id_sw = _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_PWE];
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
    BCMDNX_IF_ERR_EXIT(rv);

    trap_id_sw = _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_OAMP_CC_MPLS_TP];
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted );
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable) {

        trap_id_sw = _bcm_dpp_bfd_trap_info[unit].trap_ids[SOC_PPD_BFD_TRAP_ID_UC_IPV6];
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    _BCM_BFD_ENABLED_SET(FALSE);

    rv = _bcm_dpp_oamp_interrupts_init(unit, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_bfd_trap_info_wb_add(unit);
    BCMDNX_IF_ERR_EXIT(rv);

	if (SOC_IS_ARADPLUS(unit)) {
		/* Set the diag values (1-8)*/
		for (diag_ind = 0;  diag_ind < 16 /* 16 profiles */; ++diag_ind) {
			soc_sand_rv = soc_ppd_oam_bfd_diag_profile_set(unit,diag_ind, 0 );
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
		}
	}


    soc_sand_rv = soc_ppd_oam_deinit(unit, 1/*is_bfd*/, !oam_is_init);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_bfd_detach(int unit) {

    bcm_error_t rv = BCM_E_NONE;
    uint8 oam_is_init, is_bfd_init;

    BCMDNX_INIT_FUNC_DEFS;
    
    BFD_ACCESS.free(unit);
    BCMDNX_IF_ERR_EXIT(rv);
    
    _BCM_BFD_ENABLED_GET(is_bfd_init);

	if (!is_bfd_init) {
		BCM_EXIT;
	} 

	if (bcm_petra_bfd_uc_is_init(unit)) {
	    rv = bcm_petra_bfd_uc_deinit(unit);
	    BCMDNX_IF_ERR_EXIT(rv);
	}

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		rv = _bcm_dpp_oam_dbs_destroy(unit, FALSE/*is_oam*/);
		BCMDNX_IF_ERR_EXIT(rv);
	}

    rv = _bcm_dpp_oam_endpoint_list_destroy(unit, &_bcm_bfd_endpoint_list[unit]);
    BCMDNX_IF_ERR_EXIT(rv);

	/* We do not deinit the bfd on purpose - it might hurt the sync-WB */
	_BCM_BFD_ENABLED_SET(FALSE);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* ***************************************************************/
/*******************ENDPOINT DIAGNOSTICS SECTION********************/
/* ***************************************************************/


void bcm_dpp_print_mpls_egress_label(bcm_mpls_egress_label_t *egress_label) {
    LOG_CLI((BSL_META("label: %u,  exp: %u, TTL: %u, Packet Priority: %d, "), egress_label->label, egress_label->exp, egress_label->ttl, egress_label->pkt_pri));
}




/*function simply prints information about the endpoint, no questions asked*/
void
bcm_dpp_bfd_endpoint_diag_print(bcm_bfd_endpoint_info_t *endpoint_info) {
    LOG_CLI((BSL_META("=====BFD endpoint ID: 0X%x \n\t***Properties:\n"), endpoint_info->id));

    LOG_CLI((BSL_META("\tType: ")));
    switch (endpoint_info->type) {
    case bcmBFDTunnelTypeUdp:
        LOG_CLI((BSL_META("BFD over IPV4/6"))); break;
    case bcmBFDTunnelTypeMpls:
        LOG_CLI((BSL_META("BFD over MPLS LSP"))); break;
    case bcmBFDTunnelTypePweControlWord:
        LOG_CLI((BSL_META("BFD over PWE control word (VCCV Type 1)"))); break;
    case bcmBFDTunnelTypeMplsTpCc:
        LOG_CLI((BSL_META("BFD over MPLS-TP proactive CC"))); break;
    case bcmBFDTunnelTypeMplsTpCcCv:
        LOG_CLI((BSL_META("BFD over MPLS-TP proactive CC&CV"))); break;
    case bcmBFDTunnelTypePweGal:
        LOG_CLI((BSL_META("BFD over PWE With GAL"))); break;
    case bcmBFDTunnelTypePweRouterAlert:
        LOG_CLI((BSL_META("BFD over PWE With Router Alert (VCCV Type 2)")));
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_ACH) {
            LOG_CLI((BSL_META(" including PWE-ACH")));
        }
        break; 
    default:
        LOG_CLI((BSL_META("INVALID TYPE")));
    }

    LOG_CLI((BSL_META("\n\tLocal state: ")));
    switch (endpoint_info->local_state) {
    case bcmBFDStateAdminDown:
        LOG_CLI((BSL_META("Admin down"))); break;
    case bcmBFDStateDown:
        LOG_CLI((BSL_META("down"))); break;
    case bcmBFDStateUp:
        LOG_CLI((BSL_META("up"))); break;
    case bcmBFDStateInit:
        LOG_CLI((BSL_META("Init"))); break;
    default:
        LOG_CLI((BSL_META("INVALID LOCAL STATE")));
    }

    if (endpoint_info->flags & BCM_BFD_ENDPOINT_MPLS_TP_POLL_SEQUENCE_ENABLE) {
        LOG_CLI((BSL_META("\n\tMPLS TP poll sequnce enabled.")));
    }
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) {
        LOG_CLI((BSL_META("\n\tBFD session is processed in OAMA (rather than CPU). TX gport: 0X%x"), endpoint_info->tx_gport));
    }
    if (endpoint_info->remote_gport != BCM_GPORT_INVALID) {
        LOG_CLI((BSL_META("\n\tRemote gport: 0X%x"), endpoint_info->remote_gport));
    } else {
        LOG_CLI((BSL_META("\n\tRemote gport disabled")));
    }

    if (endpoint_info->type == bcmBFDTunnelTypeUdp || endpoint_info->type == bcmBFDTunnelTypeMpls) {
        LOG_CLI((BSL_META("\n\tSource address: " IPV4_PRINT_FMT),
                 IPV4_PRINT_ARG(endpoint_info->src_ip_addr)));
        LOG_CLI((BSL_META(" UDP source port: %d"), endpoint_info->udp_src_port));
    }
    if (endpoint_info->type == bcmBFDTunnelTypeUdp) {
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
            LOG_CLI((BSL_META("\n\tIPV4 multi hop")));
            LOG_CLI((BSL_META("Destination IP address: " IPV4_PRINT_FMT),
                     IPV4_PRINT_ARG(endpoint_info->dst_ip_addr)));
            LOG_CLI((BSL_META("IP TTL: %u, IP TOS: %u"), endpoint_info->ip_ttl, endpoint_info->ip_tos));
        }
    } else {
        LOG_CLI((BSL_META("\n\tFirst MPLS label on MPLS stack: ")));
        bcm_dpp_print_mpls_egress_label(&(endpoint_info->egress_label));
    }
    LOG_CLI((BSL_META("\n\tQueing priority: %d"), endpoint_info->int_pri));
    LOG_CLI((BSL_META("\n\tBFD rate (ms): %d"), endpoint_info->bfd_period));
    LOG_CLI((BSL_META("\n\tDesired local min TX interval: %u, Required local RX interval: %u"), endpoint_info->local_min_tx, endpoint_info->local_min_rx));
    LOG_CLI((BSL_META("\n\tLocal discriminator: %u, Local detection multiplier: %u"), endpoint_info->local_discr, endpoint_info->local_detect_mult));
    LOG_CLI((BSL_META("\n\tRemote discriminator: %u, Remote detection multiplier: %u"), endpoint_info->remote_discr, endpoint_info->remote_detect_mult));
    LOG_CLI((BSL_META("\n")));

}

