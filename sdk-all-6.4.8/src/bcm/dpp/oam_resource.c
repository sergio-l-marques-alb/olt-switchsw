/*
 * $Id: oam_resource.c,v 1.148 Broadcom SDK $
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
 * File:    oam_resource.c
 * Purpose: OAM Resources module (traps, profiles, IDs, ...)
 */

#define _ERR_MSG_MODULE_NAME BSL_BCM_OAM
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/field_int.h>

#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/oam_hw_db.h>
#include <bcm_int/dpp/oam_resource.h>
#include <bcm_int/dpp/oam_dissect.h>
#include <bcm_int/dpp/oam_sw_db.h>
#include <bcm_int/dpp/bfd.h>

#include <bcm_int/dpp/alloc_mngr_glif.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_eg_mirror.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>

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

#define OAM_HASHS_ACCESS  OAM_ACCESS.oam_hashs

#define ENDP_LST_ACCESS          OAM_ACCESS.endp_lst
#define ENDP_LST_ACCESS_DATA     OAM_ACCESS.endp_lst.endp_lsts_array
#define ENDP_LST_M_ACCESS        OAM_ACCESS.endp_lst_m
#define ENDP_LST_M_ACCESS_DATA   OAM_ACCESS.endp_lst_m.endp_lst_ms_array



#define BCM_OAM_OAMP_DESTINATION_TRAP_CODE_GET(is_upmep, mep_type, destination_trap_code) \
  do {   \
	if (is_upmep) { /* Can be only Eth. OAM */   \
			rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPD_OAM_UPMEP_TRAP_ID_OAMP, &destination_trap_code);\
			BCMDNX_IF_ERR_EXIT(rv);   \
    }   \
	else {   \
		if (mep_type == SOC_PPD_OAM_MEP_TYPE_ETH_OAM) {   \
        	rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_OAMP, &destination_trap_code);\
			BCMDNX_IF_ERR_EXIT(rv);   \
		}   \
		else if (mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_MPLSTP) {   \
        	rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_MPLS, &destination_trap_code);\
			BCMDNX_IF_ERR_EXIT(rv);   \
		}   \
		else /*(classifier_mep_entry->mep_type == SOC_PPD_OAM_MEP_TYPE_Y1731_O_PWE)*/ {   \
        	rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_PWE, &destination_trap_code);\
			BCMDNX_IF_ERR_EXIT(rv);   \
		}   \
	}   \
  } while (0)

#define BCM_OAM_CPU_DESTINATION_TRAP_CODE_GET(is_upmep, meta_data, destination_trap_code) \
  do {   \
	if (is_upmep) { \
        rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPD_OAM_UPMEP_TRAP_ID_CPU, &destination_trap_code);\
        BCMDNX_IF_ERR_EXIT(rv);   \
    }    \
	else { \
       rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_CPU, &destination_trap_code);\
	   BCMDNX_IF_ERR_EXIT(rv);   \
    }   \
  } while (0)

#define BCM_OAM_RCY_DESTINATION_TRAP_CODE_GET(is_upmep, meta_data, destination_trap_code) \
  do {   \
	if (is_upmep) { \
        rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPD_OAM_UPMEP_TRAP_ID_RECYCLE, &destination_trap_code);\
        BCMDNX_IF_ERR_EXIT(rv);   \
    }    \
	else { \
       rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_RECYCLE, &destination_trap_code);\
	   BCMDNX_IF_ERR_EXIT(rv);   \
    }   \
  } while (0)

#define _BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_id, cpu_trap_code, jer_keep_orig_headers) \
  do { \
      bcm_mirror_destination_t mirror_dest={0};\
      uint8 found_match = 0;\
      rv = _bcm_dpp_oam_free_mirror_profile_find(unit, &mirror_id, *(cpu_trap_code), &found_match); \
      BCMDNX_IF_ERR_EXIT(rv); \
      if (!found_match) { \
          mirror_dest.sample_rate_dividend = mirror_dest.sample_rate_divisor = 1;\
          mirror_dest.flags = BCM_MIRROR_DEST_WITH_ID;\
          if (SOC_IS_JERICHO(unit)&&(jer_keep_orig_headers)){\
              mirror_dest.flags |= BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER;\
          }\
          BCM_GPORT_MIRROR_MIRROR_SET(mirror_dest.mirror_dest_id, mirror_id);\
          if (_bcm_petra_mirror_or_snoop_destination_create(unit,&mirror_dest,0)!=BCM_E_NONE) {\
              BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Error: Failed to allocate mirroring destination\n"))); \
          } \
      } \
  } while (0)

#define _BCM_OAM_TRAP_CODE_UPMEP_FTMH(trap_code_upmep)      \
  ((trap_code_upmep == SOC_PPD_TRAP_CODE_OAM_CPU_MIRROR) || \
  (trap_code_upmep == SOC_PPD_TRAP_CODE_OAM_OAMP_MIRROR)  )

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


/* traps_ref_counter functions */
int
_bcm_dpp_oam_traps_ref_counter_increase(int unit, uint32 trap_code) {
	uint32 traps_ref_counter;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;
    rv = OAM_ACCESS.traps_ref_counter.get(unit, trap_code, &traps_ref_counter);
    BCMDNX_IF_ERR_EXIT(rv);
    traps_ref_counter++;
    rv = OAM_ACCESS.traps_ref_counter.set(unit, trap_code, traps_ref_counter);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_oam_traps_ref_counter_decrease(int unit, uint32 trap_code) {
	uint32 traps_ref_counter;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;
    rv = OAM_ACCESS.traps_ref_counter.get(unit, trap_code, &traps_ref_counter);
    BCMDNX_IF_ERR_EXIT(rv);
    traps_ref_counter--;
    rv = OAM_ACCESS.traps_ref_counter.set(unit, trap_code, traps_ref_counter);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_oam_traps_ref_counter_is_zero(int unit, uint32 trap_code, uint8 * is_zero) {
    uint32 traps_ref_counter;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;
    rv = OAM_ACCESS.traps_ref_counter.get(unit, trap_code, &traps_ref_counter);
    BCMDNX_IF_ERR_EXIT(rv);
    *is_zero = (traps_ref_counter == 0);
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

    if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)
        || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
        profile_data->is_default = is_default;
    }
	else {
        if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
            /* New Arad+ Classifier: Setting the mp_type_passive_active_mix to 1 enables MIP Level filtering.
               Go by soc property*/
            profile_data->mp_type_passive_active_mix = SOC_DPP_CONFIG(unit)->pp.oam_mip_level_filtering;
        }
        else {
            profile_data->mp_type_passive_active_mix = (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(classifier_mep_entry)) ? 1 : 0;
        }
	}

	profile_data->is_1588 = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588) != 0);

	rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPD_OAM_UPMEP_TRAP_ID_CPU, &upmep_trap_id_cpu);
    BCMDNX_IF_ERR_EXIT(rv);
	rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_CPU, &trap_id_cpu);
	BCMDNX_IF_ERR_EXIT(rv);
	rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_ERR_LEVEL, &trap_id_err_level);
	BCMDNX_IF_ERR_EXIT(rv);

	rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_SNOOP, &trap_id_snoop);
	BCMDNX_IF_ERR_EXIT(rv);
	rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_RECYCLE, &trap_id_recycle);
	BCMDNX_IF_ERR_EXIT(rv);

    if (soc_property_get(unit, spn_OAM_RCY_PORT, -1)==-1) {
        /* Recycle port disabled, forward LBMs to CPU*/
        trap_id_recycle = trap_id_cpu;
    } else {
    	rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPD_OAM_TRAP_ID_RECYCLE, &trap_id_recycle);
        BCMDNX_IF_ERR_EXIT(rv);
    }

	/* configure non accelerated */
	for (opcode = 1; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
        rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcode, &internal_opcode);
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
 * @param found_match
 *
 * @return int
 */
int _bcm_dpp_oam_free_mirror_profile_find(int unit, uint32 * free_mirror_profile, uint32 cpu_trap_code, uint8 *found_match)
{
	int32 mirror_id;
	int first_free_prof, first_matching_prof;
	uint32 gotten_data, ref_count;
	int rv;
	uint32 soc_sand_rv = SOC_SAND_OK;

    BCMDNX_INIT_FUNC_DEFS;

	first_free_prof = first_matching_prof = DPP_MIRROR_ACTION_NDX_MAX+1;

	for (mirror_id = DPP_MIRROR_ACTION_NDX_MAX; mirror_id > 0; mirror_id--) {
		rv = _bcm_dpp_am_template_mirror_action_profile_ref_get(unit,mirror_id, &ref_count);
		BCMDNX_IF_ERR_EXIT(rv);
		if (ref_count==0) {
			first_free_prof = (first_free_prof== DPP_MIRROR_ACTION_NDX_MAX+1)? mirror_id  : first_free_prof;
		} else {
            soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_get(unit, mirror_id, &gotten_data, &ref_count/*dummy*/);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

			if (gotten_data== cpu_trap_code) {
				first_matching_prof = mirror_id;
				break;
			}
		}
	}

	if (first_matching_prof != (DPP_MIRROR_ACTION_NDX_MAX + 1)) {
		*free_mirror_profile = first_matching_prof;
        *found_match = 1;
	} else if (first_free_prof != (DPP_MIRROR_ACTION_NDX_MAX + 1)) {
		*free_mirror_profile = first_free_prof;
        *found_match = 0;
	} else {
		BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
							(_BSL_BCM_MSG("Error: No free mirror profile found\n")));
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_mep_profile_trap_data_free(int unit, SOC_PPD_OAM_MEP_PROFILE_DATA *profile_data, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry, uint8 is_mep) {

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
	uint8 trap_count_is_zero, is_internal_trap;
    _bcm_dpp_gport_info_t gport_info;
    int core = SOC_CORE_INVALID;
	uint32		pp_port;
    int trap_id_sw;
    int trap_code_converted;

	BCMDNX_INIT_FUNC_DEFS;

	for (opcode = 1; opcode < SOC_PPD_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
		 /* Translate to internal opcode */
         rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcode, &internal_opcode);
		 BCMDNX_IF_ERR_EXIT(rv);
		 trap_code = profile_data->opcode_to_trap_code_unicast_map[internal_opcode];
		 trap_codes[trap_code] = 1;
		 trap_code = profile_data->opcode_to_trap_code_multicast_map[internal_opcode];
		 trap_codes[trap_code] = 1;
	}

	/* Starting from trap code 1 - 0 is not valid */
	for (trap_code = 1; trap_code < SOC_PPD_NOF_TRAP_CODES; trap_code++) {
		if (trap_codes[trap_code] && (trap_code != _BCM_PETRA_UD_DFLT_TRAP)) {
            uint8 is_ftmh_trap;
            rv = _bcm_dpp_oam_traps_ref_counter_decrease(unit, trap_code);
			BCMDNX_IF_ERR_EXIT(rv);
			rv = _bcm_dpp_oam_traps_ref_counter_is_zero(unit, trap_code, &trap_count_is_zero);
			BCMDNX_IF_ERR_EXIT(rv);
            if (!trap_count_is_zero) {
                /* Trap is still being used. Nothing more to do.*/
                /* Internally allocated traps needs freeing/destroying
                   when no more references to them exist */
                continue;
            }

            
			rv = OAM_ACCESS.is_internally_allocated_trap.get(unit, trap_code, &is_internal_trap);
			BCMDNX_IF_ERR_EXIT(rv);
            if (is_internal_trap) {
                is_ftmh_trap = (bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloced_check(unit, trap_code) == BCM_E_EXISTS);

                /* Get destination trap port */
                bcm_rx_trap_config_t_init(&trap_config);

                trap_id_sw = trap_code;
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit, trap_id_sw, &trap_code_converted));
                rv = bcm_petra_rx_trap_get(unit, trap_code_converted, &trap_config);
                BCMDNX_IF_ERR_EXIT(rv);
                if ((trap_config.snoop_cmnd > 0) && (!is_ftmh_trap)) {
                    continue; /* Do not free snoop traps, they are configured by the user and should be freed by him (except ftmh traps) */
                }
                if (trap_config.flags & BCM_RX_TRAP_UPDATE_DEST) {
                    /* set oam port profile to 0 */
                    rv = _bcm_dpp_gport_to_phy_port(unit, trap_config.dest_port, 0, &gport_info);
                    BCMDNX_IF_ERR_EXIT(rv);
                    BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
                        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port,&pp_port,&core)));

                        soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, 0);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }

                if (!is_ftmh_trap) {
                    /* Free traps */
                    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
                    BCMDNX_IF_ERR_EXIT(rv);
                    /* This trap is destroyed and may be re-allocated externally to the oam
                       Unmark it as internal */
                    rv = OAM_ACCESS.is_internally_allocated_trap.set(unit, trap_code, 0);
                    BCMDNX_IF_ERR_EXIT(rv);
                } else {
                    /* No need to free meta traps - only mark that they can be reused */
                    rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(unit, trap_code);
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                if (SOC_IS_ARADPLUS(unit)) {
                    /* If this trap is used by oamp_rx_trap_codes[1-3] delete it from there */
                    soc_sand_rv = soc_ppd_oam_oamp_rx_trap_codes_delete(unit, classifier_mep_entry->mep_type, trap_code);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }


                if (!is_mep) {
                    rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.get(unit, trap_code, &wb_mirror_profile);
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
                    } else {
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
            }

            if (is_mep && _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry)) {
                /* search for mirror profile that points to the same trap code */
                for (mirror_id = DPP_MIRROR_ACTION_NDX_MAX; mirror_id > 0; mirror_id--) {
                    rv = _bcm_dpp_am_template_mirror_action_profile_get(unit, mirror_id, &mirror_trap_dest);
                    if (rv != BCM_E_NOT_FOUND) {
                        BCMDNX_IF_ERR_EXIT(rv);
                        soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_get(unit, mirror_id, &mirror_trap_dest, &snoop_strength_dummy);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
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
                } /* end of for loop over mirror profiles*/
            } /* end of "if is_upmep*/

            if (is_internal_trap) {
                wb_mirror_profile = 0;
                rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_code, wb_mirror_profile);
                BCMDNX_IF_ERR_EXIT(rv);
            }

        } 
    } /* End of for loop over trap codes */

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
         rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcode, &internal_opcode);
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
	uint32 wb_mirror_profile;
    uint8  u8_wb_mirror_profile;
	uint8 no_traps;
	int snoop_cmnd;
    uint8 is_upmep, is_mep, is_accelerated;
    bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)], cpu_port, recycle_port;
    int count_erp;
    int trap_code_up;
    bcm_port_config_t port_config;
    _bcm_dpp_gport_info_t gport_info;
    int core = SOC_CORE_INVALID;
	uint32		pp_port;
    int trap_id_sw;
    int trap_code_converted;
	BCMDNX_INIT_FUNC_DEFS;

    is_upmep = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry);
    is_mep = (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(classifier_mep_entry));
    is_accelerated = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(classifier_mep_entry);

	/* Check if destination is trap gport with snoop destination */
    if (BCM_GPORT_IS_TRAP(remote_gport)) {
        *trap_code = BCM_GPORT_TRAP_GET_ID(remote_gport);
        bcm_rx_trap_config_t_init(&trap_config);
        trap_id_sw = *trap_code;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
		rv = bcm_petra_rx_trap_get(unit, trap_code_converted, &trap_config);
		BCMDNX_IF_ERR_EXIT(rv);

        if (trap_config.snoop_cmnd>0) {
             if (!is_mep) {
                 if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)
                     && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                     int trap_id_sw;
                     int trap_id;
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
                     trap_id_sw = SOC_PPD_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH;
                     BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_id));
                     rv = bcm_petra_rx_trap_set(unit, trap_id, &trap_config);
                     BCMDNX_IF_ERR_EXIT(rv);

                    /* Use the same mirror profile as the one allocated by init. */
                    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPD_OAM_MIRROR_ID_SNOOP, &wb_mirror_profile);
                    BCMDNX_IF_ERR_EXIT(rv);

                    rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, *trap_code, wb_mirror_profile);
                    BCMDNX_IF_ERR_EXIT(rv);
                 } else if (SOC_IS_JERICHO(unit)) {
                     uint32 trap_code_i;
                     uint32 trap_code_upmep;
                     uint32 mirror_id=0;
                     bcm_rx_trap_config_t ftmh_trap_config;

                     for (trap_code_i = SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST;
                          trap_code_i < SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST + SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM;
                          trap_code_i++) {
                         if (bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloced_check(unit, trap_code_i) == BCM_E_EXISTS) {
                             BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_code_i, &trap_code_converted));
                             rv = bcm_petra_rx_trap_get(unit, trap_code_converted, &ftmh_trap_config);
                             BCMDNX_IF_ERR_EXIT(rv);
                             if (ftmh_trap_config.snoop_cmnd == trap_config.snoop_cmnd) {
                                 /* Use the same mirror profile as the one allocated by init. */
                                 BCM_EXIT;
                             }
                         }

                     }

                     /* If executing this code, then the user had created a new trap-code for the down-direction.
                        Need to allocate a ftmh trap-code for the up-direction and connect it to a mirror profile */

                     /* Allocate trap code */
                     rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, 0/*flags*/, &trap_code_upmep);
                     BCMDNX_IF_ERR_EXIT(rv);
                     /* Convert it to HW trap ID */
                     BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,(int)trap_code_upmep, &trap_code_converted));

                     trap_config.flags &= ~BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                     trap_config.forwarding_header = 0;

                     rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config);
                     BCMDNX_IF_ERR_EXIT(rv);

                     /* Allocate mirror profile */
                     _BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_id, &trap_code_upmep, 1);
                     soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_get(unit, mirror_id, &trap_code_dummy, &snoop_strength);
                     BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                     snoop_strength = soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 3);
                     soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, mirror_id,  trap_code_upmep, snoop_strength, 7);
                     BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                     wb_mirror_profile = mirror_id;
                     rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, *trap_code, wb_mirror_profile);
                     BCMDNX_IF_ERR_EXIT(rv);
                 }
                 else {
                    snoop_cmnd = trap_config.snoop_cmnd;
                    /* Check that if MIP 2 consecutive trap codes are allocated */
                    rv = bcm_petra_rx_trap_get(unit, (*trap_code+1), &trap_config);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (trap_config.snoop_cmnd != snoop_cmnd) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("   Error: For MIP two consecutive trap codes with same snoop command should be allocated.")));
                    }
                 }
             }
             else if (SOC_IS_ARADPLUS(unit)) { /* MEP */
                 if (is_upmep) {
                     /* In this case, an 'ftmh' trap code need to be used to prevent a new set of system headers from being created. */
                     bcm_rx_trap_config_t trap_i_config;
                     no_traps=1;

                     /* First - Search for one that's already allocated */
                     for (trap_code_i = SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST;
                           trap_code_i < SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST + SOC_PPD_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM;
                           trap_code_i++) {
                         if (bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloced_check(unit, trap_code_i) == BCM_E_EXISTS) {
                             BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_code_i, &trap_code_converted));
                             rv = bcm_petra_rx_trap_get(unit, trap_code_converted, &trap_i_config);
                             BCMDNX_IF_ERR_EXIT(rv);
                             rv = _bcm_dpp_gport_compare(unit, trap_config.dest_port, trap_i_config.dest_port, _bcmDppGportCompareTypeSystemPort, &is_equal);
                             BCMDNX_IF_ERR_EXIT(rv);
                             if (is_equal && (trap_i_config.snoop_cmnd == trap_config.snoop_cmnd)) {
                                 /* allocated trap code found */
                                 *trap_code = trap_code_i;
                                 no_traps = 0;
                                 break;
                             }
                         }
                     }

                     if (no_traps) {
                         /* No suitable traps found. Allocate a new 'ftmh' trap */
                         rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, 0/*flags*/, trap_code);
                         BCMDNX_IF_ERR_EXIT(rv);
                         /* And set the trap config for it */
                         trap_id_sw = *trap_code;
                         BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
                         trap_config.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu; 
                         rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config);
                         BCMDNX_IF_ERR_EXIT(rv);

                     }
                 }
                 else { /* Down-MEP */
                     /* In this case, Just need to add the trap code to the OAMP rx trap code collection */
                     soc_sand_rv = soc_ppd_oam_oamp_rx_trap_codes_set(unit, classifier_mep_entry->mep_type, *trap_code);
                     BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                 }
             }
        }
        /* Create mirror profile and map to trap code */
        if (is_upmep
            || ((!is_mep) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0) == 0
                && SOC_IS_ARADPLUS_AND_BELOW(unit))) {
            /* Allocate upmep trap code and mirror profile. For advanced snooping or Jericho, This has been done above. */
            _BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_profile, trap_code, 0);
            soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_get(unit, mirror_profile, &trap_code_dummy, &snoop_strength);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            snoop_strength = soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 3);
            soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, mirror_profile,  *trap_code, snoop_strength, 7);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            wb_mirror_profile = mirror_profile;
            rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, *trap_code, wb_mirror_profile);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!is_mep) {
                rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, (*trap_code) + 1, wb_mirror_profile);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }


		BCM_EXIT;
    }

    if (is_accelerated) { /* If destination is OAMP, trap code should be one of the specially allocated in init */
        rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &count_erp);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_gport_compare(unit, oamp_port[0], remote_gport, _bcmDppGportCompareTypeSystemPort, &is_equal);
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

    rv = _bcm_dpp_gport_compare(unit, cpu_port, remote_gport, _bcmDppGportCompareTypeSystemPort, &is_equal);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_equal) {
         BCM_OAM_CPU_DESTINATION_TRAP_CODE_GET(is_upmep, meta_data, (*trap_code));
         BCM_EXIT;
    }

    /* If destination is recycle port, we will use existing traps */
    /*get recycle port*/
    recycle_port = soc_property_get(unit, spn_OAM_RCY_PORT, -1);
    if (recycle_port != -1) {
        rv = _bcm_dpp_gport_compare(unit, recycle_port, remote_gport, _bcmDppGportCompareTypeSystemPort, &is_equal);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_equal) {
            BCM_OAM_RCY_DESTINATION_TRAP_CODE_GET(is_upmep, meta_data, (*trap_code));
            BCM_EXIT;
        }
    }


	/* Check if destination is new one */
	for (trap_code_i = 1; trap_code_i < SOC_PPD_NOF_TRAP_CODES; trap_code_i++) {
		rv = _bcm_dpp_oam_traps_ref_counter_is_zero(unit, trap_code_i, &no_traps);
        BCMDNX_IF_ERR_EXIT(rv);
		if ((!no_traps) && (trap_code_i != _BCM_PETRA_UD_DFLT_TRAP) && (trap_code_i !=SOC_PPD_TRAP_CODE_OAM_LEVEL)  && (trap_code_i !=SOC_PPD_TRAP_CODE_OAM_PASSIVE)) {
			bcm_rx_trap_config_t_init(&trap_config);

            trap_id_sw = trap_code_i;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
			rv = bcm_petra_rx_trap_get(unit, trap_code_converted, &trap_config);
			BCMDNX_IF_ERR_EXIT(rv);
			is_equal = 0;
			if (trap_config.flags & BCM_RX_TRAP_UPDATE_DEST) {
				rv = _bcm_dpp_gport_compare(unit, trap_config.dest_port, remote_gport, _bcmDppGportCompareTypeSystemPort, &is_equal);
				BCMDNX_IF_ERR_EXIT(rv);
				if (is_equal) {
					rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.get(unit, trap_code_i, &u8_wb_mirror_profile);
					BCMDNX_IF_ERR_EXIT(rv);
					if (is_upmep) {
						/* Check according to mirror profile if the trap code is used for up or down. If wb_mirror_profile == 1 it is up. */
						if ((u8_wb_mirror_profile != 0)) {
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
							if (u8_wb_mirror_profile == 0) {
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
            /* trap code is always the software id*/
		}
		else {

            /* flags is zero - no need to convert to HW trap id */
			rv = bcm_petra_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, (int*)trap_code);
			BCMDNX_IF_ERR_EXIT(rv);
            /* we got the trap code from bcm rx trap type create which returns the HW id-> we need to convert it to the SW id*/
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_sw_id(unit,*trap_code,(int *)trap_code));
    		/* This trap is allocated internally and should be freed (destroyed)
    		   when it is no longer in use by OAM. Mark it as such */
    		rv = OAM_ACCESS.is_internally_allocated_trap.set(unit, *trap_code, 1);
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

        trap_id_sw = *trap_code;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
        trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
        trap_config.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
        rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config);
		BCMDNX_IF_ERR_EXIT(rv);

		if (is_upmep || (!is_mep)) {
			/* Allocate upmep trap code and mirror profile */
			uint32 trap_code_up_int = trap_code_up;
			_BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_profile, &trap_code_up_int, _BCM_OAM_TRAP_CODE_CPU_KEEP_ORIG_HEADERS(trap_code_up));
			soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_get(unit, mirror_profile, &trap_code_dummy, &snoop_strength);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, mirror_profile, trap_code_up, snoop_strength,7);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			wb_mirror_profile = mirror_profile;
			rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_code_up, wb_mirror_profile);
			BCMDNX_IF_ERR_EXIT(rv);

            if (!is_mep) {
			   /* MIP - allocate both up and downmep trap codes, mirror profile should be mapped to both. */
               trap_id_sw = trap_code_up;
               BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit, trap_id_sw, &trap_code_converted));
               trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
               trap_config.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
               rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config);
               BCMDNX_IF_ERR_EXIT(rv);
               rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, *trap_code, wb_mirror_profile);
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
    int trap_id_sw;
    uint8 got_into_switch_flag;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_ACCESS.trap_info.trap_ids.get(unit, trap_index, &trap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    _BCM_BFD_ENABLED_GET(is_bfd_init);

     got_into_switch_flag = 0;
     trap_config->flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
     trap_config->forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
    /*creating trap id*/
        switch (trap_index) {
    case SOC_PPD_OAM_TRAP_ID_OAMP:
		rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamEthAccelerated, (int*)&trap_id);
		BCMDNX_IF_ERR_EXIT(rv);
		rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
		BCMDNX_IF_ERR_EXIT(rv);
        got_into_switch_flag = 1;
		break;
	case SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_MPLS:
		rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamY1731MplsTp, (int*)&trap_id);
		BCMDNX_IF_ERR_EXIT(rv);
		rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
		BCMDNX_IF_ERR_EXIT(rv);
        got_into_switch_flag = 1;
		break;
	case SOC_PPD_OAM_TRAP_ID_OAMP_Y1731_PWE:
		rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamY1731Pwe, (int*)&trap_id);
		BCMDNX_IF_ERR_EXIT(rv);
		rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
		BCMDNX_IF_ERR_EXIT(rv);
        got_into_switch_flag = 1;
        break;
	case SOC_PPD_OAM_TRAP_ID_ERR_LEVEL:
		rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamLevel, (int*)&trap_id);
		BCMDNX_IF_ERR_EXIT(rv);
		rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
        BCMDNX_IF_ERR_EXIT(rv);
        got_into_switch_flag = 1;
        break;
	case SOC_PPD_OAM_TRAP_ID_ERR_PASSIVE:
		rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamPassive, (int*)&trap_id);
		BCMDNX_IF_ERR_EXIT(rv);
		rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
		BCMDNX_IF_ERR_EXIT(rv);
        got_into_switch_flag = 1;
        break;
    default:
        if ((trap_index != SOC_PPD_OAM_TRAP_ID_CPU) || (!is_bfd_init)) { /* bfd init already created cpu trap */

            int trap_id_sw;
            int trap_code_converted;

            trap_id_sw = trap_code;
            got_into_switch_flag = 1;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
            trap_id = trap_code_converted;
            rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int *)&trap_code_converted);
            BCMDNX_IF_ERR_EXIT(rv);
            if ((trap_index == SOC_PPD_OAM_TRAP_ID_RECYCLE) || (trap_index == SOC_PPD_OAM_TRAP_ID_SNOOP)) { /* Snooped and recycled packets don't need OAM offset index */
                trap_config->flags &= ~BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                trap_config->forwarding_header = 0;
                rv = bcm_petra_rx_trap_set(unit, trap_code_converted, trap_config);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {

                trap_config->flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                trap_config->forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
                rv = bcm_petra_rx_trap_set(unit, trap_code_converted, trap_config);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
    /* In this case the trap id holds the HW id - we need to convert it to SW id.*/
    if( got_into_switch_flag == 1 )
    {
            /* we got the trap id from bcm rx trap type create which returns the HW id-> we need to convert it to the SW id*/
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_sw_id(unit,trap_id,  &trap_id_sw));
    }
    else
    {
          trap_id_sw = trap_id;
    }
    trap_id = trap_id_sw;
	rv = OAM_ACCESS.trap_info.trap_ids.set(unit, trap_index, trap_id);
	BCMDNX_IF_ERR_EXIT(rv);

	/* Increasing trap code counter in order to for this trap not to be deleted with another profile */
	rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_id);
    BCMDNX_IF_ERR_EXIT(rv);

	if (set_upmep) {
		/*creating trap id for egress*/
		if (_BCM_OAM_TRAP_CODE_UPMEP_FTMH(trap_code_upmep)) {
            int trap_id_sw;
            int trap_code_converted;
			/* These trap codes are already allocated - no need to call trap_create, just to indicate that they are used */
			rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, SHR_RES_ALLOC_WITH_ID, &trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);

            trap_id_sw = trap_code_upmep;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
            trap_config->flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config->forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, trap_config);
			BCMDNX_IF_ERR_EXIT(rv);
			/* Increasing trap code counter in order to for this trap not to be deleted with another profile */
			rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);
		}
		else if (trap_code_upmep == SOC_PPD_TRAP_CODE_OAM_CPU_UP) {

            int trap_id_sw;
            int trap_code_converted;

            trap_id_sw = trap_code_upmep;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,trap_id_sw, &trap_code_converted));
			/* This is a new trap code we have to create */
			rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int*)&trap_code_converted);
			BCMDNX_IF_ERR_EXIT(rv);
            trap_config->flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config->forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, trap_config);
			BCMDNX_IF_ERR_EXIT(rv);
			/* Increasing trap code counter in order to for this trap not to be deleted with another profile */
			rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);
		}
        else if (SOC_IS_JERICHO(unit) && _BCM_OAM_TRAP_CODE_CPU_KEEP_ORIG_HEADERS(trap_code_upmep)) {
            /* Using a PMF program to prevent the outer FTMH from being created. Need to use a trap code that's suitable
               for this program's selection */

            int trap_code_converted;

            /* Get suitable trap code */
			rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, SHR_RES_ALLOC_WITH_ID, &trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);
            /* Convert it to HW trap ID */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_get_converted_trap_hw_id(unit,(int)trap_code_upmep, &trap_code_converted));

            trap_config->flags &= ~BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config->forwarding_header = 0;

            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, trap_config);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Increasing trap code counter in order to for this trap not to be deleted with another profile */
			rv = _bcm_dpp_oam_traps_ref_counter_increase(unit, trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = OAM_ACCESS.trap_info.upmep_trap_ids.set(unit, upmep_trap_index, trap_code_upmep);
		BCMDNX_IF_ERR_EXIT(rv);

		/*creating mirror profile*/
		if (trap_code_upmep == SOC_PPD_TRAP_CODE_OAM_CPU_SNOOP
            || (SOC_IS_JERICHO(unit) && _BCM_OAM_TRAP_CODE_CPU_KEEP_ORIG_HEADERS(trap_code_upmep))) {
			snoop_strength = soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 3);
		}
		else {
			snoop_strength = 0;
		}


		
		_BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_id, &trap_code_upmep, _BCM_OAM_TRAP_CODE_CPU_KEEP_ORIG_HEADERS(trap_code_upmep));
		mirror_id_wb = (uint8)mirror_id;
		rv = OAM_ACCESS.trap_info.mirror_ids.set(unit, mirror_ind_uint32, mirror_id_wb);
		BCMDNX_IF_ERR_EXIT(rv);


		rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_code_upmep, mirror_id_wb);
		BCMDNX_IF_ERR_EXIT(rv);

        /* Advanced snooping in Arad/Arad+ */
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0) && trap_code_upmep == SOC_PPD_TRAP_CODE_OAM_CPU_SNOOP
            && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
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
	rv = OAM_ACCESS.error_type_to_trap_id_map.set(unit, ppd_trap_type, oamp_error_trap_id);
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
    SOC_TMC_DEST_INFO dest;
    int cpu_port;
    bcm_port_config_t port_config;

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
        rv = OAM_ACCESS.error_type_to_trap_id_map.get(unit, trap_type, &trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        if (trap_id == oamp_error_trap_id) {
            /* Reset destination */

            /*get CPU port*/
            rv = bcm_petra_port_config_get(unit, &port_config);
            BCMDNX_IF_ERR_EXIT(rv);
            BCM_PBMP_ITER(port_config.cpu, cpu_port) {
               break; /*get first CPU port*/
            }

            SOC_TMC_DEST_INFO_clear(&dest);
            dest.id = cpu_port;
            dest.type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;

            soc_sand_rv = soc_ppd_oam_oamp_error_trap_id_and_destination_set(unit, trap_type, 0, dest);
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
    SOC_TMC_DEST_INFO dest;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);
    SOC_TMC_DEST_INFO_clear(&dest);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	if (!SOC_IS_ARADPLUS(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
	}

    rv = _bcm_dpp_gport_to_tm_dest_info(unit, dest_port, &dest);
    BCMDNX_IF_ERR_EXIT(rv);


    /*Here should go over all types mapped to this id and set destination in soc*/
    for (trap_type=0; trap_type<SOC_PPD_OAM_OAMP_TRAP_TYPE_COUNT; trap_type++) {
        rv = OAM_ACCESS.error_type_to_trap_id_map.get(unit, trap_type, &trap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        if (trap_id == oamp_error_trap_id) {
            /* Set destination */
            soc_sand_rv = soc_ppd_oam_oamp_error_trap_id_and_destination_set(unit, trap_type, trap_id, dest);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}


int _bcm_dpp_oam_error_trap_get(int unit, uint32 oamp_error_trap_id, bcm_gport_t * dest_port)
{
    int rv;
	uint32 soc_sand_rv=0;
    uint32 trap_type;
    uint32 trap_id;
	uint8 oam_is_init;
    SOC_TMC_DEST_INFO dest;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	if (!SOC_IS_ARADPLUS(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
	}

    /*Here should go over all types mapped to this id and get destination from soc*/
    for (trap_type=0; trap_type<SOC_PPD_OAM_OAMP_TRAP_TYPE_COUNT; trap_type++) {
        rv = OAM_ACCESS.error_type_to_trap_id_map.get(unit, trap_type, &trap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        if (trap_id == oamp_error_trap_id) {
            /* Get destination */
            soc_sand_rv = soc_ppd_oam_oamp_error_trap_id_and_destination_get(unit, trap_type, &trap_id, &dest);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    rv = _bcm_dpp_gport_from_tm_dest_info(unit, dest_port, &dest);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}


