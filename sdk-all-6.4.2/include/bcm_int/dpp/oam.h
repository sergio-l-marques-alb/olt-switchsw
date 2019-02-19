/*
 * $Id: oam.h,v 1.23 Broadcom SDK $ 
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
 * IPMC Internal header
 */

#ifndef _BCM_INT_DPP_OAM_H_
#define _BCM_INT_DPP_OAM_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/oam.h>
#include <shared/hash_tbl.h>
#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPC/ppc_api_oam.h>

#ifdef BCM_ARAD_SUPPORT
#define _BCM_OAM_ENABLED_GET(oam_is_init)                     \
  do {		                                                  \
    if (SOC_IS_PETRAB(unit)) {  \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.")));     \
    } else if (SOC_IS_ARAD(unit) && (!SOC_DPP_PP_ENABLE(unit))) {    \
        oam_is_init = 0;                                      \
    } else {                                                  \
        rv = arad_pp_sw_db_oam_enable_get(unit, &oam_is_init);\
        BCMDNX_IF_ERR_EXIT(rv);                                  \
    }                                                         \
  } while (0)

#define _BCM_OAM_ENABLED_SET(oam_is_init)                     \
  do {		                                                  \
	rv = arad_pp_sw_db_oam_enable_set(unit, oam_is_init);     \
	BCMDNX_IF_ERR_EXIT(rv);                                      \
  } while (0)

#define _BCM_BFD_ENABLED_GET(is_bfd_init)                     \
  do {		                                                  \
    if (SOC_IS_ARAD(unit) && (!SOC_DPP_PP_ENABLE(unit))) {    \
      is_bfd_init = 0;                                        \
    } else {                                                  \
        rv = arad_pp_sw_db_bfd_enable_get(unit, &is_bfd_init);\
        BCMDNX_IF_ERR_EXIT(rv);                                  \
    }                                                         \
  } while (0)

#define _BCM_BFD_ENABLED_SET(is_bfd_init)                     \
  do {		                                                  \
	rv = arad_pp_sw_db_bfd_enable_set(unit, is_bfd_init);     \
	BCMDNX_IF_ERR_EXIT(rv);                                      \
  } while (0)

#else
#define _BCM_OAM_ENABLED_GET(oam_is_init)                         \
  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.\n")));

#define _BCM_BFD_ENABLED_GET(is_bfd_init)                         \
  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.\n")));

#define _BCM_OAM_ENABLED_SET(oam_is_init)                         \
  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.\n")));

#define _BCM_BFD_ENABLED_SET(is_bfd_init)                         \
  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.\n")));
#endif /*BCM_ARAD_SUPPORT*/

#define _BCM_OAM_REMOTE_MEP_INDEX_BIT 25
#define _BCM_OAM_BFD_MEP_INDEX_BIT 26
#define _BCM_OAM_UNACCELERATED_MEP_UPMEP_INDEX_BIT 27
#define _BCM_OAM_UNACCELERATED_MEP_MDL_INDEX_BIT 28 /* Uses 3 bits*/
#define _BCM_OAM_SERVER_CLIENT_SIDE_BIT SOC_PPD_OAM_SERVER_CLIENT_SIDE_BIT


#define _BCM_OAM_MEP_ID_MASK (SOC_PPD_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit) -1)

/* unaccelerated mep index calculation */
#define _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(lif, mdl, is_up)  \
	(lif + (mdl << _BCM_OAM_UNACCELERATED_MEP_MDL_INDEX_BIT) + (is_up << _BCM_OAM_UNACCELERATED_MEP_UPMEP_INDEX_BIT))

/* unaccelerated mep index calculation */
#define _BCM_BFD_UNACCELERATED_MEP_INDEX_GET(lif)  \
	(lif + (1 << _BCM_OAM_BFD_MEP_INDEX_BIT))

#define _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(rmep_id_internal, endpoint)  \
	(endpoint = rmep_id_internal + (1 << _BCM_OAM_REMOTE_MEP_INDEX_BIT))

#define _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint)  \
	(rmep_id_internal = endpoint & 0xFFFFFF)

#define _BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint)  \
	(((endpoint >> _BCM_OAM_REMOTE_MEP_INDEX_BIT) & 1) == 1)


#define _BCM_OAM_SET_MEP_INDEX_FOR_SERVER_CLIENT_SIDE(endpoint_id, server_endpoint_id) \
	endpoint_id = (server_endpoint_id & _BCM_OAM_MEP_ID_MASK) + (1 << _BCM_OAM_SERVER_CLIENT_SIDE_BIT)

#define _BCM_OAM_GET_MEP_INDEX_FOR_SERVER_CLIENT_SIDE(endpoint_id)  (endpoint_id & _BCM_OAM_MEP_ID_MASK)

#define _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE  SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE

#define _BCM_OAM_TRAP_FWD_OFFSET_INDEX 7 /* 3b -1 */
#define _BCM_OAM_INVALID_LIF ((uint32)-1)

/* endpoint list struct */
typedef struct endpoint_list_member_s {
	uint32 index;
	struct endpoint_list_member_s * next;
} endpoint_list_member_t;

typedef struct endpoint_list_s {
	endpoint_list_member_t * first_member;
	uint32 size;
} endpoint_list_t;

extern uint8 _bcm_dpp_cb_ref_counter[BCM_MAX_NUM_UNITS][SOC_PPD_OAM_EVENT_COUNT];
extern shr_htb_hash_table_t _bcm_dpp_oam_ma_index_to_name_db[BCM_MAX_NUM_UNITS];
extern shr_htb_hash_table_t _bcm_dpp_oam_ma_to_mep_db[BCM_MAX_NUM_UNITS];
extern shr_htb_hash_table_t _bcm_dpp_oam_rmep_info_db[BCM_MAX_NUM_UNITS];
extern shr_htb_hash_table_t _bcm_dpp_oam_bfd_mep_info_db[BCM_MAX_NUM_UNITS];
extern shr_htb_hash_table_t _bcm_dpp_oam_bfd_mep_to_rmep_db[BCM_MAX_NUM_UNITS];
/* Jericho only*/
extern shr_htb_hash_table_t _bcm_dpp_oam_mep_to_ais_id[BCM_MAX_NUM_UNITS];


int _bcm_dpp_oam_dbs_init(int unit, uint8 is_oam);
int _bcm_dpp_oam_dbs_destroy(int unit, uint8 is_oam);

int _bcm_dpp_oam_mep_profile_trap_data_alloc(int unit, SOC_PPD_OAM_MEP_PROFILE_DATA *profile_data);
int _bcm_dpp_oam_mep_profile_trap_data_free(int unit, SOC_PPD_OAM_MEP_PROFILE_DATA *profile_data, uint8 is_mep);

int _bcm_dpp_bfd_get_hash_tables_from_wb_arrays(int unit);
int _bcm_dpp_oam_bfd_mep_info_construct_bfd_mep_list_cb(int unit, shr_htb_key_t key, shr_htb_data_t data);

int _bcm_oam_bfd_common_cpu_trap_code_set(uint32 unit, uint32 trap_code);
int _bcm_oam_bfd_common_cpu_trap_code_get(uint32 unit, uint32* trap_code);

int _bcm_dpp_oamp_interrupts_init(int unit, uint8 enable_interrupts);
int _bcm_dpp_oam_bfd_trap_code_from_remote_gport(int unit, 
                                                 bcm_gport_t remote_gport, 
                                                 uint32 *trap_code, 
                                                 uint8 meta_data, 
                                                 SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry);

int _bcm_dpp_oam_traps_ref_counter_increase(int unit, uint32 trap_code);
int _bcm_dpp_oam_traps_ref_counter_decrease(int unit, uint32 trap_code);
int _bcm_dpp_oam_traps_ref_counter_is_zero(int unit, uint32 trap_code, uint8 * is_zero);

int _bcm_dpp_sw_db_hash_oam_bfd_mep_info_create(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_create(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_create(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_info_destroy(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_destroy(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_destroy(int unit);

int _bcm_dpp_bfd_mep_id_alloc(int unit, uint32 flags, uint32 *mep_index);
int _bcm_dpp_oam_mep_id_alloc(int unit, uint32 flags, uint8 is_short, uint32 *mep_index);
int _bcm_dpp_oam_bfd_mep_id_is_alloced(int unit, uint32 mep_id);
int _bcm_dpp_oam_bfd_mep_id_dealloc(int unit, uint32 mep_id);

int _bcm_dpp_oam_bfd_rmep_info_db_insert(int unit, uint32 rmep_index, SOC_PPD_OAM_RMEP_INFO_DATA * rmep_info);
int _bcm_dpp_oam_bfd_rmep_info_db_delete(int unit, uint32 rmep_index);

int _bcm_dpp_oam_bfd_mep_info_db_insert(int unit, uint32 mep_index, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_info);
int _bcm_dpp_oam_bfd_mep_info_db_get(int unit, uint32 mep_index, SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY * mep_info, uint8 *found);
int _bcm_dpp_oam_bfd_mep_info_db_delete(int unit, uint32 mep_index);
int _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(int unit, uint32 mep_index, uint32 rmep_index);
int _bcm_dpp_oam_bfd_mep_to_rmep_db_get(int unit, uint32 mep_index, endpoint_list_t **rmep_list, uint8* found);
int _bcm_dpp_oam_bfd_mep_to_rmep_db_mep_delete(int unit, uint32 mep_index);
int _bcm_dpp_oam_bfd_mep_to_rmep_db_rmep_delete(int unit, uint32 mep_index, uint32 rmep_index);

void _bcm_dpp_oam_endpoint_list_init(endpoint_list_t * endpoint_list);
int _bcm_dpp_oam_endpoint_list_member_add(int unit, endpoint_list_t * endpoint_list, uint32 endpoint_index);
int _bcm_dpp_oam_endpoint_list_empty(int unit, endpoint_list_t * endpoint_list);
int _bcm_dpp_oam_endpoint_list_member_find(int unit, endpoint_list_t * endpoint_list, uint32 index, uint8 delete_endpoint, uint8 *found);
int _bcm_dpp_oam_endpoint_list_destroy(int unit, endpoint_list_t * endpoint_list);

/* OAMP errors trap allocation */
int _bcm_dpp_oam_error_trap_allocate(int unit, bcm_rx_trap_t trap_type, uint32 oamp_error_trap_id);
int _bcm_dpp_oam_error_trap_destroy(int unit, uint32 oamp_error_trap_id);
int _bcm_dpp_oam_error_trap_set(int unit, uint32 oamp_error_trap_id, bcm_gport_t dest_port);


/*diagnostics functions*/
int _bcm_dpp_oam_bfd_diagnostics_endpoints(int unit);
int _bcm_dpp_oam_bfd_diagnostics_endpoint_by_id(int unit, int endpoint_id);
int _bcm_dpp_oam_bfd_diagnostics_LM_counters(int unit, int endpoint_id);


int _bcm_dpp_oam_fifo_interrupt_handler(int unit);


int bcm_petra_oam_deinit(int unit);

#endif /* _BCM_INT_DPP_OAM_H_ */

