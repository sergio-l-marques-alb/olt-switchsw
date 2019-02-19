/*
 * $Id: oam_resource.h,v 1.23 Broadcom SDK $
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
 *
 * OAM Resources module (traps, profiles, IDs, ...)
 */
#ifndef _BCM_INT_DPP_OAM_RESOURCE_H_
#define _BCM_INT_DPP_OAM_RESOURCE_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/oam.h>
#include <bcm/bfd.h>
#include <shared/hash_tbl.h>
#include <shared/swstate/sw_state_hash_tbl.h>

#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>

int _bcm_dpp_bfd_mep_id_alloc(int unit, uint32 flags, uint32 *mep_index);
int _bcm_dpp_oam_mep_id_alloc(int unit, uint32 flags, uint8 is_short, uint32 *mep_index);
int _bcm_dpp_oam_bfd_mep_id_is_alloced(int unit, uint32 mep_id);
int _bcm_dpp_oam_bfd_mep_id_dealloc(int unit, uint32 mep_id);

int _bcm_dpp_oam_mirror_data_free(int unit, uint32 trap_code);

int _bcm_dpp_oam_free_mirror_profile_find(int unit, uint32 * free_mirror_profile, uint32 cpu_trap_code, uint8 *found_match);

int _bcm_oam_trap_and_mirror_profile_set(int unit,
										 SOC_PPC_OAM_TRAP_ID trap_index,
										 SOC_PPC_OAM_UPMEP_TRAP_ID upmep_trap_index,
										 bcm_rx_trap_config_t * trap_config,
										 uint32 trap_code,
										 uint32 trap_code_upmep,
										 SOC_PPC_OAM_MIRROR_ID mirror_ndx,
										 uint8 set_upmep);

int _bcm_oam_trap_and_mirror_profile_up_lbm_set(int unit, bcm_rx_trap_config_t * trap_config);

int _bcm_oam_default_profile_get(int unit,
								  SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data,
								  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
								  uint8 is_accelerated,
								  uint8 is_default,
                                  uint8 is_upmep);

/* Gets the trap codes from the destination gports, Sets mirror profile if required, Handles advanced egress snooping,
   Sets OAMP RX trap if required
   IN: unit, id, action
   OUT: trap_code, trap_code2 */
int _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(int unit, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY* classifier_mep_entry,
                                                          bcm_gport_t destination, bcm_gport_t destination2,
                                                          uint32* trap_code, uint32* trap_code2);

/* OAMP errors trap allocation */
int _bcm_dpp_oam_error_trap_allocate(int unit, bcm_rx_trap_t trap_type, uint32 oamp_error_trap_id);
int _bcm_dpp_oam_error_trap_destroy(int unit, uint32 oamp_error_trap_id);
int _bcm_dpp_oam_error_trap_set(int unit, uint32 oamp_error_trap_id, bcm_gport_t dest_port);
int _bcm_dpp_oam_error_trap_get(int unit, uint32 oamp_error_trap_id, bcm_gport_t * dest_port);

#endif  /* _BCM_INT_DPP_OAM_RESOURCE_H_ */
