/*
 * $Id: alloc_mngr.c,v 1.312 Broadcom SDK $
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
 * File:        alloc_mngr.c
 * Purpose:     Resource allocation manager for SOC_SAND chips.
 *
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>
#include <shared/swstate/sw_state_access.h>

/*#include <soc/error.h>*/
#include <bcm_int/common/debug.h>
#include <soc/debug.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/module.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <soc/dpp/dpp_config_defs.h>
#include <bcm/switch.h>
#include <soc/dpp/PPC/ppc_api_lif.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/dpp/wb_db_alloc.h>
#endif

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_trill.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>


/*
 * GLOBAL LIF DEFINES AND MACROS
 */
#define _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(_object_id) (_object_id / _BCM_DPP_AM_GLOBAL_LIF_NOF_ENTRIES_PER_BANK)

#define _BCM_DPP_AM_GLOBAL_LIF_BANK_IS_LEGAL_ALLOCATE_WITH_ID       0x0001
#define _BCM_DPP_AM_GLOBAL_LIF_BANK_IS_LEGAL_ALLOCATED_BANK_ONLY    0x0002

/* 
 * GLOBAL LIF UTILITIES DECLARATIONS. 
 */

STATIC uint32
_bcm_dpp_am_global_lif_bank_is_sync_bank(int unit, int bank_id, uint8 *is_sync);

STATIC uint32
_bcm_dpp_am_global_lif_bank_type_set(int unit, int bank_id, uint8 is_ingress, bcm_dpp_am_global_lif_bank_type_t new_bank_type);

STATIC uint32
_bcm_dpp_am_global_lif_is_legal_bank(int unit, uint32 sync_flags, uint32 bank_alloc_flags, int bank_id, uint8 *is_legal);

STATIC uint32
_bcm_dpp_am_global_lif_is_legal_bank_by_type(int unit, int bank_id, uint8 is_ingress, uint8 is_sync, uint32 bank_alloc_flags, uint8 *is_legal);

STATIC uint32
_bcm_dpp_am_global_lif_allocate_new_bank(int unit, uint32 sync_flags, int *bank_id, uint8 *is_success); 

STATIC uint32
_bcm_dpp_am_global_lif_update_bank_and_type(int unit, uint32 sync_flags, int bank_id); 

STATIC uint32
_bcm_dpp_am_global_lif_clear_bank_and_type(int unit, uint32 sync_flags, int bank_id);

STATIC uint32
_bcm_dpp_am_global_lif_valid_banks_get(int unit, uint32 sync_flags, int *nof_valid_banks, int *banks_ids);

STATIC uint32
_bcm_dpp_am_egress_bank_is_direct_bank(int unit, int bank_id, uint8 *is_direct_bank);

STATIC uint32
_bcm_dpp_am_global_lif_res_alloc(int unit, uint32 alloc_flags, uint32 sync_flags, int bank_id, int *global_lif);

STATIC uint32
_bcm_dpp_am_global_lif_internal_alloc_by_type(int unit, uint32 alloc_flags, uint32 sync_flags, int *global_lif, uint8 *alloc_success);

STATIC uint32
_bcm_dpp_am_global_lif_internal_alloc(int unit, uint32 alloc_flags, uint32 sync_flags, int *global_lif);

STATIC uint32
_bcm_dpp_am_global_lif_internal_dealloc(int unit, uint32 sync_flags, int global_lif);

STATIC uint32
_bcm_dpp_am_global_lif_internal_is_alloc(int unit, uint32 sync_flags, int global_lif_id);

STATIC uint32
_bcm_dpp_am_global_lif_update_ingress_count(int unit, int diff);

STATIC uint32
_bcm_dpp_am_global_lif_update_egress_count(int unit, int diff);

STATIC uint32
_bcm_dpp_am_global_lif_update_count(int unit, int (*get_func)(int, int*), int (*set_func)(int, int), int diff);

STATIC uint32 
_bcm_dpp_am_global_lif_ingress_alloc_verify(int unit);

STATIC uint32 
_bcm_dpp_am_global_lif_egress_alloc_verify(int unit);

STATIC uint32 
_bcm_dpp_am_global_lif_bank_is_legal(int unit, int bank_id, uint8 is_ingress, uint8 *is_legal);

/* 
 * GLOBAL LIF UTILITIES DECLARATIONS - END.
 */

/* Simple LIF Allocation print */
#define _DPP_AM_LIF_ALLOCATION_DEBUG_PRINT 0

#ifdef BCM_ARAD_SUPPORT
#define _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id) (object_id / _BCM_DPP_AM_EGRESS_ENCAP_NOF_ENTRIES_PER_BANK)

#define _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id) (object_id / _BCM_DPP_AM_INGRESS_LIF_NOF_ENTRIES_PER_BANK)
#define _BCM_DPP_AM_INGRESS_LIF_NOF_TABLES (4)
#define _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS_IN_TABLE (4)
#define _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(table_id) (table_id * _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS_IN_TABLE)
#define _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MAX_BANK_ID(table_id) (_BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(table_id) + _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS_IN_TABLE - 1)
#define _BCM_DPP_AM_INGRESS_LIF_BANK_ID_TO_TABLE_ID(bank_id) (bank_id / _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS_IN_TABLE)

#endif /*BCM_ARAD_SUPPORT*/

#ifdef BCM_ARAD_SUPPORT

/* Egress encapsulation end definiations */

/* Egress encapsulation functions { */
STATIC uint32
_bcm_dpp_am_egress_encap_internal_alloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int count, int* object);
STATIC uint32
_bcm_dpp_am_egress_encap_internal_alloc_align(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int align, int offset, int count, int* object);
STATIC uint32
_bcm_dpp_am_egress_encap_internal_dealloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, int count, int object);
/* Egress encapsulation functions } */

/* Ingress LIF end definiations */

/* Ingress LIF functions { */
STATIC uint32
_bcm_dpp_am_ingress_lif_internal_alloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int count, int* object);
STATIC uint32
_bcm_dpp_am_ingress_lif_internal_alloc_align(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int align, int offset, int count, int* object);
STATIC uint32
_bcm_dpp_am_ingress_lif_internal_dealloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint8 is_sync, int count, int object);
/* Ingress LIF functions } */

/* Sync LIF start definiations { */

#define _BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK (2)
#define _BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MIN (1)
#define _BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MAX (1)
#define _BCM_DPP_AM_SYNC_LIF_TYPE_NOF_BITS (_BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MAX-_BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MIN+1)
#define _BCM_DPP_AM_SYNC_LIF_NOF_BITS (_BCM_DPP_AM_LIF_NOF_BANKS*_BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK) 


/* Sync LIF end definiations } */

#endif /* BCM_ARAD_SUPPORT */

#ifdef BCM_ARAD_SUPPORT

/* Sync LIF util functions */

#define SYNC_LIF_CNTL(unit)  _dpp_am_sync_lif[(unit)]

/* Validate Sync LIF */
#define _BCM_DPP_AM_SYNC_LIF_VALID_CHECK \
    do {                                                                \
        if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not valid\n" ), (unit))); \
        } \
        if (!SOC_IS_ARAD(unit) || !SOC_DPP_PP_ENABLE(unit)) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not supported device for sync lif alloc\n" ), (unit))); \
        } \
        if (SYNC_LIF_CNTL(unit).init == FALSE) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_INIT,(_BSL_BCM_MSG("unit %d is not initialize sync lif alloc\n" ), (unit))); \
        } \
    } while (0);

STATIC uint32
_bcm_dpp_am_sync_lif_is_exist_bank(int unit, int bank_id, uint8 *is_exist)
{
    bcm_dpp_am_sync_lif_t *sync_lif_i;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;

    sync_lif_i = &SYNC_LIF_CNTL(unit);

    *is_exist = FALSE;
    if (SHR_BITGET(sync_lif_i->sync_lif_banks,bank_id*_BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK)) {
      *is_exist = TRUE;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Check is sync bank, i.e. valid bank and used of type for SYNC */
STATIC uint32
_bcm_dpp_am_sync_lif_is_sync_bank(int unit, int bank_id, uint8 *is_sync)
{
    uint32 rv;
    uint32 type = 0;
    uint8 is_exist = 0;
    bcm_dpp_am_sync_lif_t *sync_lif_i;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_sync);

    sync_lif_i = &SYNC_LIF_CNTL(unit);

    *is_sync = FALSE;

    rv = _bcm_dpp_am_sync_lif_is_exist_bank(unit,bank_id,&is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    SHR_BITCOPY_RANGE(&type, 0 , sync_lif_i->sync_lif_banks, (bank_id*_BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK+_BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MIN), (_BCM_DPP_AM_SYNC_LIF_TYPE_NOF_BITS));

    if (is_exist && type) {
      *is_sync = TRUE;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_sync_lif_set_new_bank(int unit, int bank_id, uint8 is_sync_bank)
{
    bcm_dpp_am_sync_lif_t *sync_lif_i;
    uint32 type;
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;

    sync_lif_i = &SYNC_LIF_CNTL(unit);

    /* Valid */
    SHR_BITSET(sync_lif_i->sync_lif_banks,bank_id * _BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK);
    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit,
                                   SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_SYNC_LIF_BANKS,
                                   sync_lif_i->sync_lif_banks);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set Type */
    type = (is_sync_bank) ? 1:0;
    SHR_BITCOPY_RANGE(sync_lif_i->sync_lif_banks, (bank_id*_BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK+_BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MIN), &type, 0, (_BCM_DPP_AM_SYNC_LIF_TYPE_NOF_BITS));    
    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit,
                                   SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_SYNC_LIF_BANKS,
                                   sync_lif_i->sync_lif_banks);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Ingress LIF DB - Start 
 * ARAD only 
 */

/* 
 * Each ingress lif bank (4K entries) consists of 5 bits: first bit indication of existence
 * 4 other bits relate to VTT LIF lookup results:
 * VT-Lookup-Res-0
 * VT-Lookup-Res-1
 * TT-Lookup-Res-0
 * TT-Lookup-Res-1
 * We Define phase i in ingress LIF, in case application can be resulted in VTT-Lookup result i (out of 4)
 * We Define table i in ingress LIF, for [i,i+3] consecutive banks. I.e. LIF-Table 0 Banks 0-3, LIF-Table 1 Banks 4-7...
 * HW Limitation: Two Applications cant be allocated within the same table i in case one application is located
 * in phase 0(2) and the second application is located in phase 1(3).
 */
#define _BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK (5)
#define _BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MIN (1)
#define _BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MAX (4)
#define _BCM_DPP_AM_INGRESS_LIF_PHASE_NOF_BITS (_BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MAX-_BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MIN+1)
#define _BCM_DPP_AM_INGRESS_LIF_NOF_BITS (_BCM_DPP_AM_LIF_NOF_BANKS*_BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK)


/* Validate Ingress LIF */
#define _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK \
    do {                                                                \
        if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not valid\n" ), (unit))); \
        } \
        if (!SOC_IS_ARAD(unit) || !SOC_DPP_PP_ENABLE(unit)) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not supported device for ingress lif alloc\n" ), (unit))); \
        } \
        if (INGRESS_LIF_CNTL(unit).init == FALSE) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_INIT,(_BSL_BCM_MSG("unit %d is not initialize ingress lif alloc\n" ), (unit))); \
        } \
    } while (0);

#define INGRESS_LIF_CNTL(unit)  _dpp_am_ingress_lif[(unit)]

#define _BCM_DPP_AM_INGRESS_LIF_IS_BANK_EXIST(ingress_lif_i,bank_id) \
  (SHR_BITGET(ingress_lif_i->ingress_lif_banks,bank_id*_BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK))




/* 
 *  Information on allocation manager for ingress lif:
 *  Ingress LIF includes DB support for following application types:
 *  MPLS tunnel, PWE, IPV4 tunnel, Trill, In-AC, MIM, FCoE
 *  16 Banks where each bank with 4K entries
 *  Allocation manager store for each bank its own management.
 *  Following APIs tries to manage between banks and find corresponding bank for each application.
 *  ID returns from allocation manager is directly the In-LIF ID which access the database.
 */
/* Check is existed bank */
STATIC uint32
_bcm_dpp_am_ingress_lif_is_exist_bank(int unit, int bank_id, uint8 *is_exist)
{
    bcm_dpp_am_ingress_lif_t *ingress_lif_i;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_exist);

    ingress_lif_i = &INGRESS_LIF_CNTL(unit);

    *is_exist = (_BCM_DPP_AM_INGRESS_LIF_IS_BANK_EXIST(ingress_lif_i,bank_id)) ? TRUE:FALSE;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Check is existed table */
STATIC uint32
_bcm_dpp_am_ingress_lif_is_exist_table(int unit, int table_id, uint8 *is_exist)
{
    int bank_id;
    int rv;
    uint8 is_exist_bank;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_exist);

    *is_exist = FALSE;
    for (bank_id = _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(table_id); 
          bank_id <= _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MAX_BANK_ID(table_id) ; ++bank_id) {
        
        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist_bank);
        BCMDNX_IF_ERR_EXIT(rv);
        
        if (is_exist_bank) {
            *is_exist = TRUE;
            break;
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Check if is valid table according to required phase numbers */
STATIC uint32
_bcm_dpp_am_ingress_lif_is_valid_table(int unit, int table_id, uint32 req_phase_numbers, uint8 *is_valid)
{
    uint32 used_phase_numbers = 0, tmp_phase_numbers = 0;
    bcm_dpp_am_ingress_lif_t *ingress_lif_i;
    int bank_id;
    int rv;
    uint8 is_exist;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_valid);

    ingress_lif_i = &INGRESS_LIF_CNTL(unit);



    /* Gather information regarding phase numbers for this required table */
    for (bank_id = _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(table_id); 
            bank_id <= _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MAX_BANK_ID(table_id) ; ++bank_id) {

        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist) {
            /* retreive information */
            SHR_BITCOPY_RANGE(&tmp_phase_numbers, 0, 
                        ingress_lif_i->ingress_lif_banks,bank_id*_BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK+_BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MIN, 
                        _BCM_DPP_AM_INGRESS_LIF_PHASE_NOF_BITS);
            used_phase_numbers |= tmp_phase_numbers;
        }
    }


    /* Decide according to information and given required phase numbers */
    *is_valid = TRUE;

    tmp_phase_numbers = used_phase_numbers;
    tmp_phase_numbers |= req_phase_numbers;
    /* Validate VT-Result-0 & 1 are not on */
    if ((SHR_BITGET(&tmp_phase_numbers,_dpp_am_ingress_lif_phase_vt_lookup_res_0)) && (SHR_BITGET(&tmp_phase_numbers,_dpp_am_ingress_lif_phase_vt_lookup_res_1))) {
        *is_valid = FALSE;
    }

    if ((SHR_BITGET(&tmp_phase_numbers,_dpp_am_ingress_lif_phase_tt_lookup_res_0)) && (SHR_BITGET(&tmp_phase_numbers,_dpp_am_ingress_lif_phase_tt_lookup_res_1))) {
        *is_valid = FALSE;
    }


exit:
  BCMDNX_FUNC_RETURN;
}


/* Retreive phase number. Returns error when bank doesnt exist */
STATIC uint32
_bcm_dpp_am_ingress_lif_bank_bitmap_phases_get(int unit, int bank_id, uint32 *bitmap_access_phase)
{ 
    uint8 is_exist;
    uint32 rv = BCM_E_NONE;
    bcm_dpp_am_ingress_lif_t *ingress_lif_i;
    uint32 bitmap_access_phase_lcl[1];

    BCMDNX_INIT_FUNC_DEFS;

    ingress_lif_i = &INGRESS_LIF_CNTL(unit);

    BCMDNX_NULL_CHECK(bitmap_access_phase);
    *bitmap_access_phase_lcl = 0;

    rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_exist == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bank id not exist")));
    }

    SHR_BITCOPY_RANGE(bitmap_access_phase_lcl, 0, 
                      ingress_lif_i->ingress_lif_banks, bank_id*_BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK+_BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MIN, 
                      _BCM_DPP_AM_INGRESS_LIF_PHASE_NOF_BITS);

    *bitmap_access_phase = *bitmap_access_phase_lcl;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* Allocate new bank according to requested phase numbers and flags */
STATIC uint32
_bcm_dpp_am_ingress_lif_allocate_new_bank(int unit, uint8 is_sync, uint32 flags, uint32 req_phase_numbers, int *bank_id, uint8 *is_success)
{
    uint32 rv;
    int new_bank = -1, tmp_bank_id, table_id = -1, tmp_table_id;
    uint8 is_valid, is_exist;
    bcm_dpp_am_ingress_lif_t *ingress_lif_i;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(bank_id);
    BCMDNX_NULL_CHECK(is_success);

    ingress_lif_i = &INGRESS_LIF_CNTL(unit);

    if (((req_phase_numbers >> _BCM_DPP_AM_INGRESS_LIF_PHASE_NOF_BITS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("phases %d are incorrect"), req_phase_numbers));
    }

    *is_success = TRUE;

    /* Find new bank */
    new_bank = -1;
    if (flags & SHR_RES_ALLOC_WITH_ID) {
        /* Allocate new given bank */
        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, *bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bank %d existed even if already suppose to be not existed"), *bank_id));
        } 
        
        /* is valid table */
        table_id = _BCM_DPP_AM_INGRESS_LIF_BANK_ID_TO_TABLE_ID(*bank_id);
        rv = _bcm_dpp_am_ingress_lif_is_valid_table(unit, table_id, req_phase_numbers, &is_valid);
        BCMDNX_IF_ERR_EXIT(rv);
        
        if (!is_valid) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("bank %d not existed but does not match"
                                       " table id %d requirements"),
                              *bank_id,
                              table_id));
        }
        
        new_bank = *bank_id;                             
    } else {
        /* 
         * Find arbitray new bank, according to limitations
         * 1. Find match table i (prefer existed which support phase number and then one that is not allocated)
         * 2. Get arbitray bank from table i
         */ 
        /* Find match table */   
        new_bank = -1;    
        for (tmp_table_id = 0; tmp_table_id < _BCM_DPP_AM_INGRESS_LIF_NOF_TABLES; ++tmp_table_id) {
            rv = _bcm_dpp_am_ingress_lif_is_exist_table(unit, tmp_table_id, &is_exist);
            BCMDNX_IF_ERR_EXIT(rv);
            
            if (is_exist) {
                /* match table */
                rv = _bcm_dpp_am_ingress_lif_is_valid_table(unit, tmp_table_id, req_phase_numbers, &is_valid);
                BCMDNX_IF_ERR_EXIT(rv);
                
                if (is_valid) {
                    for (tmp_bank_id = _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(tmp_table_id); 
                          tmp_bank_id <= _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MAX_BANK_ID(tmp_table_id) ; ++tmp_bank_id) {
                        
                        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, tmp_bank_id, &is_exist);
                        BCMDNX_IF_ERR_EXIT(rv);
                        
                        if (!is_exist) {
                            /* In case of sync, have one more constrain bank is not used by any other application at egress */
                            if (is_sync) {
                                rv = _bcm_dpp_am_sync_lif_is_exist_bank(unit, tmp_bank_id, &is_exist);
                                BCMDNX_IF_ERR_EXIT(rv);
                                if (is_exist) {
                                  continue;
                                }
                            }
                            table_id = tmp_table_id;
                            new_bank = tmp_bank_id;
                            break;
                        }
                    }
                    
                    if (new_bank != -1) {
                        /* Found new bank */
                        break;
                    }
                }
            }
        }
                
        if (new_bank == -1) {
            /* In case no valid bank, find from unexist table */
            for (tmp_table_id = 0; tmp_table_id < _BCM_DPP_AM_INGRESS_LIF_NOF_TABLES; ++tmp_table_id) {
                rv = _bcm_dpp_am_ingress_lif_is_exist_table(unit, tmp_table_id, &is_exist);
                BCMDNX_IF_ERR_EXIT(rv);
                
                if (!is_exist) {
                    table_id = tmp_table_id;

                    for (tmp_bank_id = _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(tmp_table_id); 
                          tmp_bank_id <= _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MAX_BANK_ID(tmp_table_id) ; ++tmp_bank_id) {
                        
                        /* In case of sync, have one more constrain bank is not used by any other application at egress */
                        if (is_sync) {
                            rv = _bcm_dpp_am_sync_lif_is_exist_bank(unit, tmp_bank_id, &is_exist);
                            BCMDNX_IF_ERR_EXIT(rv);
                            if (is_exist) {
                              continue;
                            }
                        }

                        new_bank = tmp_bank_id;
                        break;
                    }

                    if (new_bank != -1) {
                        /* Found new bank */
                        break;
                    }
                }
            }
        }
        
        if (table_id == -1) {
            /* Unavaiable table */
            /* No bank is avaiable with the following required phases */
            *is_success = FALSE;
            BCM_EXIT;            
        }
        
        /* Self check */
        if (new_bank != -1) {
            rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, new_bank, &is_exist);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_exist) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("found unexist table id %d, but first bank already allocated"), table_id));
            }
        }
        
        
    }
    

    if (new_bank == -1) { /* No new bank avaiable */
        *is_success = FALSE;
        BCM_EXIT;
    }

#if _DPP_AM_LIF_ALLOCATION_DEBUG_PRINT
    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            new_bank, req_phase_numbers)));
#endif   

    /* Set SW DB bank information */
    /* Valid */
    SHR_BITSET(ingress_lif_i->ingress_lif_banks,new_bank * _BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK);
    
    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit,
                                   SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_INGRESS_LIF_BANKS,
                                   ingress_lif_i->ingress_lif_banks);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Required Phase number */
    SHR_BITCOPY_RANGE(ingress_lif_i->ingress_lif_banks, (new_bank*_BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK+_BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MIN), &req_phase_numbers, 0, (_BCM_DPP_AM_INGRESS_LIF_PHASE_NOF_BITS));    
    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit,
                                   SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_INGRESS_LIF_BANKS,
                                   ingress_lif_i->ingress_lif_banks);
    BCMDNX_IF_ERR_EXIT(rv);


    /* Count */
    ingress_lif_i->ingress_lif_count++;
    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit,
                                   SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_INGRESS_LIF_COUNT,
                                   &(ingress_lif_i->ingress_lif_count));
    BCMDNX_IF_ERR_EXIT(rv);

    *bank_id = new_bank;

    /* 
     * Special case: Bank ID is 0, many application do not use InLIF 0. Allocate it so it won't be used 
     * We wont skip the allocation in case global lif index is 0. In this case, we can take advantage of that lif. 
     */
    if (new_bank == 0 && (SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple != 0 && SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop != 0)) {
        int object = 0;        
        rv = dpp_am_res_alloc(unit, new_bank, dpp_am_res_obs_inlif, SHR_RES_ALLOC_WITH_ID, 1, &object);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Convert allocation manager pool id to access phase number */
STATIC uint32
_bcm_dpp_am_ingress_lif_alloc_mngr_pool_id_to_bitmap_access_phase(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint32 *bitmap_phase_number)
{    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(allocation_info);

    *bitmap_phase_number = 0;

    switch (allocation_info->pool_id) {
    case dpp_am_res_lif_pwe: /* PWE termination */
      switch (allocation_info->application_type) {
      case bcm_dpp_am_ingress_lif_app_mpls_term:
        /* In case of Unindexed supported up to two terminations */
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
        break;
      /* MPLS Indexed Support */
      case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1:    
        if (SOC_DPP_CONFIG(unit)->pp.tunnel_termination_in_tt_only) {
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);  /* TT only lookups, tt lookup res 0 is MPLS_1 */         
        } else{
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);          
        }
        break;
      case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_2:          
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_1);          
        break;
      case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_3:          
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
        break; 
      case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_2:
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_1); 
        break;
      case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_3:
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);
        break;
      default:        
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported pool id %d and types %d"), allocation_info->pool_id, allocation_info->application_type));
      }      
      break;
    case dpp_am_res_lif_ip_tnl: /* IP tunnel termination */
      _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);
      break;
    case dpp_am_res_lif_dynamic: /* Main applications using Incoming LIF */
        switch (allocation_info->application_type) {
        case bcm_dpp_am_ingress_lif_app_ingress_ac:
        case bcm_dpp_am_ingress_lif_app_vpn_ac:
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_0);
          /* In case trill is enabled, make sure the ingress nick lookup does conflict with VD-vlan-vlan lookup */
          if (SOC_DPP_CONFIG(unit)->trill.mode) {
              _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);
          }
          break;
        case bcm_dpp_am_ingress_lif_app_ingress_isid:
        case bcm_dpp_am_ingress_lif_app_ip_term:
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
          break;
        case bcm_dpp_am_ingress_lif_app_mpls_term:
          /* In case of Unindexed supported up to two terminations */
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
          break;
        /* FRR */
        case bcm_dpp_am_ingress_lif_app_mpls_frr_term:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);          
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_explicit_null:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);          
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_mldp:
           _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);
         break;
        /* MPLS Indexed Support */
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1:          
          if (SOC_DPP_CONFIG(unit)->pp.tunnel_termination_in_tt_only) {
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
          } else {
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);          
          }
          break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_2:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_1);          
          break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_3:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
          break;        
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_2:
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_1);
          break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_3:
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
          break;
        case bcm_dpp_am_ingress_lif_app_trill_nick:
          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_1);
          break;
        default:
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported pool id %d and types %d"), allocation_info->pool_id, allocation_info->application_type));
        }
    break;
    default:
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported pool id %d in the ingress lif database"), allocation_info->pool_id));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Returns all existed banks with the corresponding access phases. Always returns with increasing order */
STATIC uint32
_bcm_dpp_am_ingress_lif_banks_by_bitmap_access_phase(int unit, uint8 is_sync, uint32 bitmap_phase_access, int* nof_valid_banks, int* banks_ids)
{
    uint32 bank_id, bank_bimap_phase_access, rv;
    uint8 is_exist, is_sync_bank;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(nof_valid_banks);
    BCMDNX_NULL_CHECK(banks_ids);

    *nof_valid_banks = 0;
    for (bank_id = 0; bank_id < _BCM_DPP_AM_LIF_NOF_BANKS; ++bank_id) {
        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist) {
            /* SYNC banks are low priority to handle */
            rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_sync != is_sync_bank) {
                continue;
            }

            /* Validate phase number */
            rv = _bcm_dpp_am_ingress_lif_bank_bitmap_phases_get(unit, bank_id, &bank_bimap_phase_access);
            BCMDNX_IF_ERR_EXIT(rv);

            if ((bitmap_phase_access & bank_bimap_phase_access) == bitmap_phase_access) {
                /* We have a match add it to the valid banks */
                banks_ids[(*nof_valid_banks)] = bank_id;
                (*nof_valid_banks)++;
            }
        }
    } 

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Initalize ingress lif allocation */
uint32
_bcm_dpp_am_ingress_lif_init(int unit)
{
    int alloc_size;    
    bcm_dpp_am_ingress_lif_t *ingress_lif_i;   

    BCMDNX_INIT_FUNC_DEFS;

    
    if (SOC_WARM_BOOT(unit))
    {
      
      /*BCM_EXIT;*/
    }

    ingress_lif_i = &INGRESS_LIF_CNTL(unit);
    
    if (ingress_lif_i->init) {
        /* Nothing to do ingress lif already initialized */
        BCM_EXIT;
    }

    /*
     * Allcoate ingress lif bitmap
     */
    ingress_lif_i->ingress_lif_count = 0;
    
    if (ingress_lif_i->ingress_lif_banks != NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("ingress lif banks are already initialized")));
    }

    alloc_size = SHR_BITALLOCSIZE(_BCM_DPP_AM_INGRESS_LIF_NOF_BITS);
    BCMDNX_ALLOC(ingress_lif_i->ingress_lif_banks, alloc_size, "ingress lif information bitmap");
    if (NULL == ingress_lif_i->ingress_lif_banks) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation ingress lif information bitmap")));
    }

    sal_memset(ingress_lif_i->ingress_lif_banks, 0, alloc_size); 

    /* Done initalize */
    ingress_lif_i->init = TRUE;

exit:
    BCMDNX_FUNC_RETURN;
}

/* De-Initalize ingress lif allocation */
uint32
_bcm_dpp_am_ingress_lif_deinit(int unit)
{
    bcm_dpp_am_ingress_lif_t *ingress_lif_i;   

    BCMDNX_INIT_FUNC_DEFS;

    
    if (SOC_WARM_BOOT(unit))
    {
      
      /*BCM_EXIT;*/
    }

    ingress_lif_i = &INGRESS_LIF_CNTL(unit);
    
    if (!(ingress_lif_i->init)) {
        /* Nothing to do ingress lif already de-initialized */
        BCM_EXIT;
    }

    /*
     * De-Allcoate ingress lif bitmap
     */
    BCM_FREE(ingress_lif_i->ingress_lif_banks);

    /* Done De-initalize */
    ingress_lif_i->init = FALSE;

exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_ingress_lif_alloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint32 flags, int count, int* object)
{
    uint32 rv;
    int bank_id, object_id;
    uint8 is_sync_bank;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    /* Input validation, make sure lif is within valid range. */
    if (flags & (SHR_RES_ALLOC_WITH_ID)) {
        if (*object >= SOC_DPP_DEFS_GET(unit, nof_local_lifs)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given outlif value is too high.")));
        }
    }

    /* Allocate object */
    rv = _bcm_dpp_am_ingress_lif_internal_alloc(unit,allocation_info, FALSE, flags,count,object);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sync in case of need */
    object_id = *object;
    bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit,bank_id,&is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync_bank) { /* allocated in synchronize bank, then needs to allocate also egress */
        bcm_dpp_am_sync_lif_alloc_info_t sync_info;

        sal_memset(&sync_info,0,sizeof(sync_info));

        sync_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_EGRESS; /* Sync egress */
        sync_info.ingress_lif.pool_id = allocation_info->pool_id;
        sync_info.ingress_lif.application_type = allocation_info->application_type;

        /* Always WITH_ID since we synchrnoize the other side */
        flags |= SHR_RES_ALLOC_WITH_ID;
        rv = _bcm_dpp_am_sync_lif_alloc(unit, &sync_info,flags,count,object);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Allocate entry according to pool id */
STATIC uint32
_bcm_dpp_am_ingress_lif_internal_alloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int count, int* object)
{
    uint32 rv;
    bcm_dpp_am_ingress_lif_t *ingress_lif_i;
    uint32 bitmap_access_phase;
    int *banks_ids = NULL;
    int nof_valid_banks, bank_id, new_bank;
    uint32 alloc_size;
    uint8 is_exist, is_success;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    ingress_lif_i = &INGRESS_LIF_CNTL(unit);

    rv = _bcm_dpp_am_ingress_lif_alloc_mngr_pool_id_to_bitmap_access_phase(unit, allocation_info, &bitmap_access_phase);
    BCMDNX_IF_ERR_EXIT(rv);

    alloc_size = sizeof(int) * ingress_lif_i->ingress_lif_count;
    BCMDNX_ALLOC(banks_ids, alloc_size, "allocate bank ids");
    if (NULL == banks_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation bank ids")));
    }
    sal_memset(banks_ids, 0, alloc_size); 

    /* Try to allocate for each bank id unless we are dealing with case of WITH_ID */
    if (flags & (SHR_RES_ALLOC_WITH_ID)) {
        int object_id;
        uint32 bank_bitmap_phase_access;

        object_id = *object;
        /* Find matched bank */
        bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
        /* Validate bank exist and with the correct phase */
        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist == 0) {
            uint32 alloc_bank_flags;

            alloc_bank_flags = SHR_RES_ALLOC_WITH_ID;
            /* Allocate new bank since it is not existed */
            rv = _bcm_dpp_am_ingress_lif_allocate_new_bank(unit, is_sync, alloc_bank_flags, bitmap_access_phase, &bank_id, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!is_success) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("ingress lif resources didnt find new bank avaiable")));
            }

            rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, bank_id, is_sync);
            BCMDNX_IF_ERR_EXIT(rv);
            
        }

        /* Validate phase number */
        rv = _bcm_dpp_am_ingress_lif_bank_bitmap_phases_get(unit, bank_id, &bank_bitmap_phase_access);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((bitmap_access_phase & bank_bitmap_phase_access) != bitmap_access_phase) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("given invalid object id %d, bank phase access %d, not matched to phase access %d"), object_id, bank_bitmap_phase_access, bitmap_access_phase));
        }

        rv = dpp_am_res_alloc(unit, bank_id, dpp_am_res_obs_inlif, flags, count, object);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        uint8 alloc_success;

        alloc_success = FALSE;

        rv = _bcm_dpp_am_ingress_lif_banks_by_bitmap_access_phase(unit, is_sync, bitmap_access_phase, &nof_valid_banks, banks_ids);
        BCMDNX_IF_ERR_EXIT(rv);

        for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
            rv = dpp_am_res_alloc(unit, banks_ids[bank_id], dpp_am_res_obs_inlif, flags, count, object);
            if (rv == BCM_E_NONE) {
                alloc_success = TRUE;
                /* Allocation succeded no need to find other banks - save to WB and continue */
                break;
            }
        }

        if (!alloc_success) {
            /* Allocate new bank since no success */
            rv = _bcm_dpp_am_ingress_lif_allocate_new_bank(unit, is_sync, 0, bitmap_access_phase, &new_bank, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_success) {
                rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, new_bank, is_sync);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Allocate object */
                rv = dpp_am_res_alloc(unit, new_bank, dpp_am_res_obs_inlif, flags, count, object);
                BCMDNX_IF_ERR_EXIT(rv);
                alloc_success = TRUE;
            } else {
                /* Retreive from sync banks as last option (no more bank to allocate) */
                rv = _bcm_dpp_am_ingress_lif_banks_by_bitmap_access_phase(unit, TRUE, bitmap_access_phase, &nof_valid_banks, banks_ids);
                BCMDNX_IF_ERR_EXIT(rv);

                for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
                    rv = dpp_am_res_alloc(unit, banks_ids[bank_id], dpp_am_res_obs_inlif, flags, count, object);
                    if (rv == BCM_E_NONE) {
                        alloc_success = TRUE;
                        /* Allocation succeded no need to find other banks */
                        break;
                    }
                }
            }
        }

        if (!alloc_success) {
            /* FAIL all full :( */
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("ingress lif resources didnt find new bank avaiable")));
        }
    }

    BCM_EXIT;
exit:
    BCM_FREE(banks_ids);
    BCMDNX_FUNC_RETURN;

}

STATIC uint32
_bcm_dpp_am_ingress_lif_internal_alloc_align(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int align, int offset, int count, int* object)
{
    uint32 rv;
    uint32 bitmap_phase_access;
    bcm_dpp_am_ingress_lif_t *ingress_lif_i;
    int *banks_ids = NULL;
    int nof_valid_banks, bank_id, new_bank;
    uint32 alloc_size;
    uint8 is_exist, is_success;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    ingress_lif_i = &INGRESS_LIF_CNTL(unit);

    rv = _bcm_dpp_am_ingress_lif_alloc_mngr_pool_id_to_bitmap_access_phase(unit, allocation_info, &bitmap_phase_access);
    BCMDNX_IF_ERR_EXIT(rv);

    alloc_size = sizeof(int) * ingress_lif_i->ingress_lif_count;
    BCMDNX_ALLOC(banks_ids, alloc_size, "allocate bank ids");
    if (NULL == banks_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation bank ids")));
    }
    sal_memset(banks_ids, 0, alloc_size); 

    /* Try to allocate on each bank id unless we are dealing with case of WITH_ID */
    if (flags & (SHR_RES_ALLOC_WITH_ID)) {
        int object_id;
        uint32 bank_bitmap_phase_access;

        object_id = *object;
        /* Find matched bank */
        bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
        /* Validate bank exist and with the correct phase */
        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist == 0) {
            uint32 alloc_bank_flags;

            alloc_bank_flags = SHR_RES_ALLOC_WITH_ID;
            /* Allocate new bank since it is not existed */
            rv = _bcm_dpp_am_ingress_lif_allocate_new_bank(unit, is_sync, alloc_bank_flags, bitmap_phase_access, &bank_id, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!is_success) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("ingress lif resources didnt find new bank avaiable")));
            }

            rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, bank_id, is_sync);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Validate phase number */
        rv = _bcm_dpp_am_ingress_lif_bank_bitmap_phases_get(unit, bank_id, &bank_bitmap_phase_access);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((bitmap_phase_access & bank_bitmap_phase_access) != bitmap_phase_access) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("given invalid object id %d, bank phase access %d, not matched to phase access %d"), object_id, bank_bitmap_phase_access, bitmap_phase_access));
        }

        /* if replace flag is set and exist then we are done */
        if(flags & BCM_DPP_AM_FLAG_ALLOC_REPLACE) {
            rv = dpp_am_res_check(unit, bank_id, dpp_am_res_obs_inlif, offset, count);
            if(rv == BCM_E_EXISTS) {
                rv = BCM_E_NONE;
                BCM_EXIT;
            }
        }
        rv = dpp_am_res_alloc_align(unit, bank_id, dpp_am_res_obs_inlif, flags, align, offset, count, object);
        BCMDNX_IF_ERR_EXIT(rv);

    } else {
        uint8 alloc_success;

        if (flags & BCM_DPP_AM_FLAG_ALLOC_REPLACE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("cannot set replace when ID is not given")));
        }

        rv = _bcm_dpp_am_ingress_lif_banks_by_bitmap_access_phase(unit, is_sync, bitmap_phase_access, &nof_valid_banks, banks_ids);
        BCMDNX_IF_ERR_EXIT(rv);

        alloc_success = FALSE;
        for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
            rv = dpp_am_res_alloc_align(unit, banks_ids[bank_id], dpp_am_res_obs_inlif, flags, align, offset, count, object);
            if (rv == BCM_E_NONE) {
                alloc_success = TRUE;

                /* Allocation succeded no need to find other banks */
                break;
            }
        }

        if (!alloc_success) {
            /* Allocate new bank since no success */
            rv = _bcm_dpp_am_ingress_lif_allocate_new_bank(unit, is_sync, 0, bitmap_phase_access, &new_bank, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_success) {
                rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, new_bank, is_sync);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Allocate object */
                rv = dpp_am_res_alloc_align(unit, new_bank, dpp_am_res_obs_inlif, flags, align, offset, count, object);
                BCMDNX_IF_ERR_EXIT(rv);

                alloc_success = TRUE;

            } else {
                /* Retreive from sync banks as last option (no more bank to allocate) */
                rv = _bcm_dpp_am_ingress_lif_banks_by_bitmap_access_phase(unit, TRUE, bitmap_phase_access, &nof_valid_banks, banks_ids);
                BCMDNX_IF_ERR_EXIT(rv);

                for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
                    rv = dpp_am_res_alloc(unit, banks_ids[bank_id], dpp_am_res_obs_inlif, flags, count, object);
                    if (rv == BCM_E_NONE) {
                        alloc_success = TRUE;
                        /* Allocation succeded no need to find other banks */
                        break;
                    }
                }
            }
        }

        if (!alloc_success) {
            /* FAIL all full :( */
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("ingress lif resources didnt find new bank avaiable")));
        }
    }

    BCM_EXIT;
exit:
    BCM_FREE(banks_ids);
    BCMDNX_FUNC_RETURN;
}

/* Deallocate entry according to object id */
uint32
_bcm_dpp_am_ingress_lif_dealloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, int count, int object)
{
    uint32 rv;
    int bank_id;
    uint8 is_sync_bank;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

    /* Deallocate object */
    rv = _bcm_dpp_am_ingress_lif_internal_dealloc(unit,allocation_info, FALSE, count, object);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sync in case of need */
    bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object);
    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit,bank_id,&is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync_bank) { /* allocated in synchronize bank, then needs to allocate also egress */
        bcm_dpp_am_sync_lif_alloc_info_t sync_info;

        sal_memset(&sync_info,0,sizeof(sync_info));

        sync_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_EGRESS; /* Sync egress */
        sync_info.ingress_lif.pool_id = allocation_info->pool_id;
        sync_info.ingress_lif.application_type = allocation_info->application_type;

        rv = _bcm_dpp_am_sync_lif_dealloc(unit, &sync_info, count,object);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_ingress_lif_internal_dealloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint8 is_sync, int count, int object)
{
    uint32 rv;
    uint8 is_exist;
    int bank_id;
    uint32 flags = 0;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    

    /* Find matched bank */
    bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object);
    /* Validate bank exist and with the correct phase */
    rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_exist) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("given invalid object id %d"), object));
    }

    rv = dpp_am_res_free_and_status(unit, bank_id, dpp_am_res_obs_inlif, count, object, &flags);
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Is exist entry according to pool id */
int
_bcm_dpp_am_ingress_lif_is_allocated(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, int count, int object)
{
    uint32 rv;
    int bank_id;
    uint8 bank_is_exist;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

    /* Find matched bank */
    bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object);
    /* Validate bank exist and with the correct phase */
    rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &bank_is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!bank_is_exist) {
        BCM_RETURN_VAL_EXIT(BCM_E_NONE);
    }

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, bank_id, dpp_am_res_obs_inlif, count, object));
    
exit:
    BCMDNX_FUNC_RETURN;
}
/* 
 * Ingress LIF DB - End 
 * ARAD only 
 */
/* 
 * Egress encapsulation DB - Start 
 * ARAD only 
 */

/* Each egress encapsulation consists of 3 bits: first bit indication of exists 2 other bits of phase number (0-phase-1) */
#define _BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK (3)
#define _BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MIN (1)
#define _BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MAX (2)
#define _BCM_DPP_AM_EGRESS_ENCAP_PHASE_NOF_BITS (_BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MAX-_BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MIN+1)
#define _BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS (16*_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK)

/* Validate egress encap */
#define _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK \
    do {                                                                \
        if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not valid\n" ), (unit))); \
        } \
        if (!SOC_IS_ARAD(unit) || !SOC_DPP_PP_ENABLE(unit)) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not supported device for egress encap alloc\n" ), (unit))); \
        } \
        if (EGRESS_ENCAP_CNTL(unit).init == FALSE) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_INIT,(_BSL_BCM_MSG("unit %d is not initialize egress encap alloc\n" ), (unit))); \
        } \
    } while (0);

#define EGRESS_ENCAP_CNTL(unit)  _dpp_am_egress_encap[(unit)]

#define _BCM_DPP_AM_EGRESS_ENCAP_IS_BANK_EXIST(encap_i,bank_id) \
  (SHR_BITGET(encap_i->egress_encap_banks,bank_id*_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK))

/* 
 *  Information on allocation manager for egress encapsulation:
 *  Egress encapsulation includes DB support for following application types:
 *  MPLS tunnel, PWE, IPV4 tunnel, Trill, Out-AC, Out-RIF, LL 
 *  16 Banks where each bank with 2K/4K entries
 *  Allocation manager store for each bank its own management.
 *  Following APIs tries to manage between banks and find corresponding bank for each application.
 *  ID returns from allocation manager is directly the Out-LIF ID which access the database.
 */

/* Check is existed bank */
STATIC uint32
_bcm_dpp_am_egress_encap_is_exist_bank(int unit, int bank_id, uint8 *is_exist)
{
    bcm_dpp_am_egress_encap_t *encap_i;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_exist);

    encap_i = &EGRESS_ENCAP_CNTL(unit);

    *is_exist = (_BCM_DPP_AM_EGRESS_ENCAP_IS_BANK_EXIST(encap_i,bank_id)) ? TRUE:FALSE;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Retreive phase number. Returns error when bank doesnt exist */
STATIC uint32
_bcm_dpp_am_egress_encap_bank_phase_get(int unit, int bank_id, SOC_PPD_EG_ENCAP_ACCESS_PHASE *access_phase)
{ 
    uint8 is_exist;
    uint32 rv = BCM_E_NONE;
    uint32 phase_number = 0;
    bcm_dpp_am_egress_encap_t *encap_i; 

    BCMDNX_INIT_FUNC_DEFS;

    encap_i = &EGRESS_ENCAP_CNTL(unit);

    BCMDNX_NULL_CHECK(access_phase);

    rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_exist == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bank id not exist")));
    }

    SHR_BITCOPY_RANGE(&phase_number, 0, 
                      encap_i->egress_encap_banks,bank_id*_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK+_BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MIN, 
                      _BCM_DPP_AM_EGRESS_ENCAP_PHASE_NOF_BITS);

    if ((phase_number >= SOC_PPD_NOF_EG_ENCAP_ACCESS_PHASE_TYPES)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("phase number for bank id %d is incorrect, value got %d"), bank_id, phase_number));
    }

    /* 1:1 mapping between phase_number and enum types */
    *access_phase = phase_number;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Allocate new bank according to requested phase number and flags */
STATIC uint32
_bcm_dpp_am_egress_encap_allocate_new_bank(int unit, uint8 is_sync, uint32 flags, SOC_PPD_EG_ENCAP_ACCESS_PHASE phase_access, int *bank_id, uint8 *is_success)
{
    uint32 rv;
    int new_bank, tmp_bank_id;
    uint8 is_exist;
    bcm_dpp_am_egress_encap_t *encap_i;
    uint32 phase_number = phase_access;
    SOC_PPD_EG_ENCAP_RANGE_INFO encap_range_info;
    uint32 soc_sand_dev_id, soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(bank_id);
    BCMDNX_NULL_CHECK(is_success);

    *is_success = TRUE;

    encap_i = &EGRESS_ENCAP_CNTL(unit);

    soc_sand_dev_id = (unit);

    if ((phase_number >= SOC_PPD_NOF_EG_ENCAP_ACCESS_PHASE_TYPES)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("phase number %d is incorrect"), phase_number));
    }

    /* Find new bank */
    new_bank = -1;
    if (flags & SHR_RES_ALLOC_WITH_ID) {
        /* Allocate new given bank */
        rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, *bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("bank %d existed even if already suppose to be not existed"), *bank_id));
        } 
        new_bank = *bank_id;                             
    } else {
        /* Find arbitray new bank */        
        for (tmp_bank_id = 0; tmp_bank_id < SOC_PPD_EG_ENCAP_NOF_BANKS(unit); ++tmp_bank_id) {
            rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, tmp_bank_id, &is_exist);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_exist == 0) {
                if (is_sync) {
                    rv = _bcm_dpp_am_sync_lif_is_exist_bank(unit, tmp_bank_id, &is_exist);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (is_exist) {
                      continue;
                    }
                }
                /* New bank found. */
                new_bank = tmp_bank_id;
                break;
            }
        }
    }

    if (new_bank == -1) { /* No new bank avaiable */
        *is_success = FALSE;
        BCM_EXIT;
    }

#if _DPP_AM_LIF_ALLOCATION_DEBUG_PRINT
    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            new_bank, phase_access)));
#endif 


    /* Set SW DB bank information */
    /* Valid */
    SHR_BITSET(encap_i->egress_encap_banks,new_bank * _BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK);
    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit,
                                   SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_EGRESS_ENCAP_BANKS,
                                   encap_i->egress_encap_banks);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Phase number */
    phase_number = phase_access;
    SHR_BITCOPY_RANGE(encap_i->egress_encap_banks, (new_bank*_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK+_BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MIN), &phase_number, 0, (_BCM_DPP_AM_EGRESS_ENCAP_PHASE_NOF_BITS));    
    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit,
                                   SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_EGRESS_ENCAP_BANKS,
                                   encap_i->egress_encap_banks);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Count */
    encap_i->egress_encap_count++;
    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit,
                                   SOC_DPP_WB_ENGINE_VAR_ALLOC_MNGR_EGRESS_ENCAP_COUNT,
                                   &(encap_i->egress_encap_count));
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set HW */
    soc_sand_rv = soc_ppd_eg_encap_range_info_get(soc_sand_dev_id, &encap_range_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    encap_range_info.bank_access_phase[new_bank] = phase_access;

    soc_sand_rv = soc_ppd_eg_encap_range_info_set(soc_sand_dev_id, &encap_range_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *bank_id = new_bank;

    /* Special case: Bank ID is 0, many application do not use OutLIF 0. Allocate it so it won't be used */
    if (new_bank == 0) {
        SOC_PPC_EG_ENCAP_DATA_INFO data_info;

        int object = 0;        
        rv = dpp_am_res_alloc(unit, new_bank, dpp_am_res_obs_eg_encap, SHR_RES_ALLOC_WITH_ID, 1, &object);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         * ARAD-A0 Workaround: outlif 0 must be DATA entry 
         */ 
        if (SOC_IS_ARAD_A0(unit)) {
          /* Null Data entry */
          SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
          
          soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id, object, &data_info, FALSE, 0);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
/* Convert allocation manager pool id to access phase number */
uint32
_bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, SOC_PPD_EG_ENCAP_ACCESS_PHASE *phase_number)
{    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(allocation_info);

    switch (allocation_info->pool_id) {
    case dpp_am_res_eep_global: /* Link layer */
      *phase_number = (allocation_info->application_type == bcm_dpp_am_egress_encap_app_linker_layer) ? SOC_PPD_EG_ENCAP_ACCESS_PHASE_TWO:SOC_PPD_EG_ENCAP_ACCESS_PHASE_THREE;
      break;
    case dpp_am_res_eep_ip_tnl: /* IP tunnel */
      *phase_number = (allocation_info->application_type == bcm_dpp_am_egress_encap_app_ip_tunnel_roo || allocation_info->application_type == bcm_dpp_am_egress_encap_app_l2_encap) ? SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE:SOC_PPD_EG_ENCAP_ACCESS_PHASE_TWO;
      break;
    case dpp_am_res_eep_mpls_tunnel: /* PWE or MPLS tunnel simple or MPLS tunnel second */
      *phase_number = (allocation_info->application_type == bcm_dpp_am_egress_encap_app_mpls_tunnel) ? SOC_PPD_EG_ENCAP_ACCESS_PHASE_TWO:SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE;      
      break;
    case dpp_am_res_eg_out_ac:
      /* PON 3 TAGS DATA Manipulation */
      if (allocation_info->application_type == bcm_dpp_am_egress_encap_app_3_tags_data) {
          *phase_number = SOC_PPD_EG_ENCAP_ACCESS_PHASE_THREE;
      } else { /* Normal out_ac */
          *phase_number = SOC_PPD_EG_ENCAP_ACCESS_PHASE_FOUR;
      }
      break;
    case dpp_am_res_eg_out_rif: /* Out-RIF */
      *phase_number = SOC_PPD_EG_ENCAP_ACCESS_PHASE_TWO;
      break;
    case dpp_am_res_lif_dynamic:
      /* PON 3 TAGS DATA Manipulation */
      if (allocation_info->application_type == bcm_dpp_am_egress_encap_app_3_tags_data) {
          *phase_number = SOC_PPD_EG_ENCAP_ACCESS_PHASE_THREE;
      } else { /* Normal lif */
          *phase_number = SOC_PPD_EG_ENCAP_ACCESS_PHASE_FOUR;
      }
      break;
    case dpp_am_res_eg_data_erspan: /* ERSPAN */
      *phase_number = SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE;
      break;
    case dpp_am_res_ipv6_tunnel: /* IPv6 tunnel using data entry */
      *phase_number = SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE;
      break;
    case dpp_am_res_eg_data_trill_invalid_entry: /* ARAD A0: Need an invalid entry for trill  */
      *phase_number = SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE;
      break;
    case dpp_am_res_eep_trill: /* Trill */
        *phase_number = (allocation_info->application_type == bcm_dpp_am_egress_encap_app_trill_roo) ? SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE:SOC_PPD_EG_ENCAP_ACCESS_PHASE_TWO; 
        break; 
    default:
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported pool id %d in the egress encapsulation database"), allocation_info->pool_id));
    }

    if ((SOC_DPP_CONFIG(unit)->pp.pon_application_enable) &&
        (allocation_info->application_type != bcm_dpp_am_egress_encap_app_3_tags_data) &&
        (allocation_info->application_type != bcm_dpp_am_egress_encap_app_3_tags_out_ac) &&
        SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        if (SOC_DPP_CONFIG(unit)->trill.mode){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Trill is Not supported, in case of PON application enable")));
        }

        /* 
         * application implemented in Phase One is not supported with PON 3 TAGs Manipulation,
         * such as, ERSPAN, ip tunnel and trill.
         * Since SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE is 0, (phase = phase -1) is used for checking.
         */
        *phase_number -= 1;
        if (*phase_number < 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported pool id %d in the egress encapsulation database, in case of PON application enable"), allocation_info->pool_id));
            }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Returns all existed banks with the corresponding access phase. Always returns with increasing order */
STATIC uint32
_bcm_dpp_am_egress_encap_banks_by_access_phase(int unit, uint8 is_sync, SOC_PPD_EG_ENCAP_ACCESS_PHASE phase_access, int* nof_valid_banks, int* banks_ids)
{
    uint32 bank_id, phase_number, bank_phase_number, rv;
    uint8 is_exist, is_sync_bank;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(nof_valid_banks);
    BCMDNX_NULL_CHECK(banks_ids);

    /* 1:1 mapping between phase_number and phase_access */
    phase_number = phase_access;

    *nof_valid_banks = 0;
    for (bank_id = 0; bank_id < SOC_PPD_EG_ENCAP_NOF_BANKS(unit); ++bank_id) {
        rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist) {
            rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_sync != is_sync_bank) {
                continue;
            }
           
            /* Validate phase number */
            rv = _bcm_dpp_am_egress_encap_bank_phase_get(unit, bank_id, &bank_phase_number);
            BCMDNX_IF_ERR_EXIT(rv);

            if (phase_number == bank_phase_number) {
                /* We have a matach add it to the valid banks */
                banks_ids[(*nof_valid_banks)] = bank_id;
                (*nof_valid_banks)++;
            }
        }
    } 

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_egress_encap_sw_state_init(int unit)
{
    int alloc_size;    
    bcm_dpp_am_egress_encap_t *encap_i;   

    BCMDNX_INIT_FUNC_DEFS;

    
    if (SOC_WARM_BOOT(unit))
    {
      
      /*BCM_EXIT;*/
    }

    encap_i = &EGRESS_ENCAP_CNTL(unit);
    
    if (encap_i->init) {
        /* Nothing to do egress encap already initialized */
        BCM_EXIT;
    }

    /*
     * Allcoate egress encap bitmap
     */
    encap_i->egress_encap_count = 0;
    
    if (encap_i->egress_encap_banks != NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Egress encap banks are already initialized")));
    }

    alloc_size = SHR_BITALLOCSIZE(_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS);
    BCMDNX_ALLOC(encap_i->egress_encap_banks, alloc_size, "egress encap information bitmap");
    if (NULL == encap_i->egress_encap_banks) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation egress encap information bitmap")));
    }

    sal_memset(encap_i->egress_encap_banks, 0, alloc_size); 

    /* Done initalize */
    encap_i->init = TRUE;

exit:
    BCMDNX_FUNC_RETURN;
}


/* Initalize egress encapsulation allocation */
uint32
_bcm_dpp_am_egress_encap_init(int unit)
{
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    uint32 rv;
    int elem;
    int bank_id = 0;
    int i = 0;
    uint8 is_sync = FALSE;
    SOC_PPD_EG_ENCAP_ACCESS_PHASE bank_phase = 0;
    uint8 is_success = 0;

    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit))
    {
        sal_memset(&allocation_info,0,sizeof(allocation_info));

        /* 
         * In case Routing is enabled Bank 0 must be reserved. 
         * mapping out-rif to VSI and remark-profile         .
         * Also Bank 0 is used in order to use entry 0 as a NULL entry.                                                  .                                                                                                              .
         * In that case Bank 0 must be not allocated for Out-AC.                                                                                                                                                                                                                             .
         */
        allocation_info.pool_id = dpp_am_res_eg_out_rif;
        rv = _bcm_dpp_am_egress_encap_alloc(unit,&allocation_info,0,4095,&elem);
        BCMDNX_IF_ERR_EXIT(rv);

        if (SOC_IS_ARAD(unit) && SOC_DPP_CONFIG(unit)->trill.mode && (SOC_IS_ARAD_A0(unit))) {
            /* 
             * In case Trill is enabled, for ARAD A0 we need to allocate entry SHR_RES_ALLOC_WITH_ID to be invalid
             */
            SOC_PPC_EG_ENCAP_DATA_INFO data_info;
            uint32 soc_sand_rv;
            uint32 soc_sand_dev_id;

            allocation_info.pool_id = dpp_am_res_eg_data_trill_invalid_entry;
            elem = SOC_DPP_PP_FRWRD_TRILL_EEDB_INVALID_ENTRY(unit);
            rv = _bcm_dpp_am_egress_encap_alloc(unit,&allocation_info,SHR_RES_ALLOC_WITH_ID,1,&elem);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Null Data entry */
            SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
            soc_sand_dev_id = (unit);
      
            soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id, SOC_DPP_PP_FRWRD_TRILL_EEDB_INVALID_ENTRY(unit), &data_info, FALSE, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        /* Allocate bank if Phase access isn't dynamic allocated */
        for (i = 0; i < SOC_PPD_EG_ENCAP_NOF_BANKS(unit); i++) {
            if (SOC_DPP_CONFIG(unit)->pp.egress_encap_bank_phase[i]) {
                switch (SOC_DPP_CONFIG(unit)->pp.egress_encap_bank_phase[i]) {
                case egress_encap_bank_phase_static_1:
                    bank_phase = SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE;
                    is_sync = FALSE;
                    break;
                case egress_encap_bank_phase_static_2:
                    bank_phase = SOC_PPD_EG_ENCAP_ACCESS_PHASE_TWO;
                    is_sync = FALSE;
                    break;
                case egress_encap_bank_phase_static_3:
                    bank_phase = SOC_PPD_EG_ENCAP_ACCESS_PHASE_THREE;
                    is_sync = FALSE;
                    break;
                case egress_encap_bank_phase_static_4:
                    bank_phase = SOC_PPD_EG_ENCAP_ACCESS_PHASE_FOUR;
                    is_sync = FALSE;
                    break;
                default:
                    BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
                }
        
                bank_id = i;
                rv = _bcm_dpp_am_egress_encap_allocate_new_bank(unit, is_sync, SHR_RES_ALLOC_WITH_ID, bank_phase, &bank_id, &is_success);
                BCMDNX_IF_ERR_EXIT(rv);
                if (!is_success) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("egres encap resources didnt find new bank avaiable")));
                }
        
                rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, bank_id, is_sync);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* De-Initalize egress encapsulation allocation */
uint32
_bcm_dpp_am_egress_encap_deinit(int unit)
{
    bcm_dpp_am_egress_encap_t *encap_i;

    BCMDNX_INIT_FUNC_DEFS;

    
    if (SOC_WARM_BOOT(unit))
    {
      
      /*BCM_EXIT;*/
    }

    encap_i = &EGRESS_ENCAP_CNTL(unit);
    
    if (!(encap_i->init)) {
        /* Nothing to do egress encap already de-initialized */
        BCM_EXIT;
    }

    /*
     * De-Allcoate egress encap bitmap
     */
    BCM_FREE(encap_i->egress_encap_banks);

    /* Done de-initalize */
    encap_i->init = FALSE;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Allocate entry according to pool id */
uint32
_bcm_dpp_am_egress_encap_alloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint32 flags, int count, int* object)
{
    uint32 rv;
    int bank_id, object_id;
    uint8 is_sync_bank;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    /* Input validation, make sure lif is within valid range. */
    if (flags & (SHR_RES_ALLOC_WITH_ID)) {
        if (*object >= SOC_DPP_DEFS_GET(unit, nof_out_lifs)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given outlif value is too high.")));
        }
    }

    /* Allocate object */
    rv = _bcm_dpp_am_egress_encap_internal_alloc(unit,allocation_info,FALSE,flags,count,object);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sync in case of need */
    object_id = *object;
    bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit,bank_id,&is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync_bank) { /* allocated in synchronize bank, then needs to allocate also ingress */
        bcm_dpp_am_sync_lif_alloc_info_t sync_info;

        sal_memset(&sync_info,0,sizeof(sync_info));

        sync_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS; /* Sync Ingress */
        sync_info.egress_lif.pool_id = allocation_info->pool_id;
        sync_info.egress_lif.application_type = allocation_info->application_type;

        /* Always WITH_ID since we synchrnoize the other side */
        flags |= SHR_RES_ALLOC_WITH_ID;
        rv = _bcm_dpp_am_sync_lif_alloc(unit, &sync_info,flags,count,object);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_egress_encap_internal_alloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int count, int* object)
{
    uint32 rv;
    SOC_PPD_EG_ENCAP_ACCESS_PHASE phase_access;
    bcm_dpp_am_egress_encap_t *encap_i;
    int *banks_ids = NULL;
    int nof_valid_banks, bank_id, new_bank;
    uint32 alloc_size;
    uint8 is_exist;
    uint8 is_success;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    encap_i = &EGRESS_ENCAP_CNTL(unit);

    rv = _bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(unit, allocation_info, &phase_access);
    BCMDNX_IF_ERR_EXIT(rv);

    alloc_size = sizeof(int) * encap_i->egress_encap_count;
    BCMDNX_ALLOC(banks_ids, alloc_size, "allocate bank ids");
    if (NULL == banks_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation bank ids")));
    }
    sal_memset(banks_ids, 0, alloc_size); 

    /* Try to allocate for each bank id unless we are dealing with case of WITH_ID */
    if (flags & (SHR_RES_ALLOC_WITH_ID)) {
        int object_id;
        SOC_PPD_EG_ENCAP_ACCESS_PHASE bank_phase_access;

        object_id = *object;
        /* Find matched bank */
        bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
        /* Validate bank exist and with the correct phase */
        rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist == 0) {
            uint32 alloc_bank_flags;

            alloc_bank_flags = SHR_RES_ALLOC_WITH_ID;
            /* Allocate new bank since it is not existed */
            rv = _bcm_dpp_am_egress_encap_allocate_new_bank(unit, is_sync, alloc_bank_flags, phase_access, &bank_id, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!is_success) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("egres encap resources didnt find new bank avaiable")));
            }

            rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, bank_id, is_sync);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Validate phase number */
        rv = _bcm_dpp_am_egress_encap_bank_phase_get(unit, bank_id, &bank_phase_access);
        BCMDNX_IF_ERR_EXIT(rv);

        if (bank_phase_access != phase_access) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("given invalid object id %d, bank phase access %d, not matched to phase access %d"), object_id, bank_phase_access, phase_access));
        }

        rv = dpp_am_res_alloc(unit, bank_id, dpp_am_res_obs_eg_encap, flags, count, object);
        BCMDNX_IF_ERR_EXIT(rv);

    } else {
        uint8 alloc_success;

        alloc_success = FALSE;

        rv = _bcm_dpp_am_egress_encap_banks_by_access_phase(unit, is_sync, phase_access, &nof_valid_banks, banks_ids);
        BCMDNX_IF_ERR_EXIT(rv);

        for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
            rv = dpp_am_res_alloc(unit, banks_ids[bank_id], dpp_am_res_obs_eg_encap, flags,count,object);
            if (rv == BCM_E_NONE) {
                alloc_success = TRUE;
                /* Allocation succeded no need to find other banks */

                break;
            }
        }

        if (!alloc_success) {
            /* Allocate new bank since no success */
            rv = _bcm_dpp_am_egress_encap_allocate_new_bank(unit, is_sync, 0, phase_access, &new_bank, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_success) {
                rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, new_bank, is_sync);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Allocate object */
                rv = dpp_am_res_alloc(unit, new_bank, dpp_am_res_obs_eg_encap, flags, count, object);
                BCMDNX_IF_ERR_EXIT(rv);

                alloc_success = TRUE;
            } else {
                /* Retreive from sync banks as last option (no more bank to allocate) */
                rv = _bcm_dpp_am_egress_encap_banks_by_access_phase(unit, TRUE, phase_access, &nof_valid_banks, banks_ids);
                BCMDNX_IF_ERR_EXIT(rv);

                for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
                    rv = dpp_am_res_alloc(unit, banks_ids[bank_id], dpp_am_res_obs_eg_encap, flags, count, object);
                    if (rv == BCM_E_NONE) {
                        alloc_success = TRUE;
                        /* Allocation succeded no need to find other banks */

                        break;
                    }
                }
            }
        }


        if (!alloc_success) {
            /* FAIL all full :( */
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("egres encap resources didnt find new bank avaiable")));
        }
    }

    BCM_EXIT;
exit:
    BCM_FREE(banks_ids);
    BCMDNX_FUNC_RETURN;
}

/* Allocate align entry according to pool id */
uint32
_bcm_dpp_am_egress_encap_alloc_align(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint32 flags, int align, int offset, int count, int* object)
{
    uint32 rv;
    int bank_id, object_id;
    uint8 is_sync_bank;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    /* Input validation, make sure lif is within valid range. */
    if (flags & (SHR_RES_ALLOC_WITH_ID)) {
        if (*object >= SOC_DPP_DEFS_GET(unit, nof_out_lifs)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given outlif value is too high.")));
        }
    }

    /* Allocate object */
    rv = _bcm_dpp_am_egress_encap_internal_alloc_align(unit,allocation_info,FALSE,flags,align,offset,count,object);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sync in case of need */
    object_id = *object;
    bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit,bank_id,&is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync_bank) { /* allocated in synchronize bank, then needs to allocate also ingress */
        bcm_dpp_am_sync_lif_alloc_info_t sync_info;

        sal_memset(&sync_info,0,sizeof(sync_info));

        sync_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS; /* Sync Ingress */
        sync_info.egress_lif.pool_id = allocation_info->pool_id;
        sync_info.egress_lif.application_type = allocation_info->application_type;

        /* Always WITH_ID since we synchrnoize the other side */
        flags |= SHR_RES_ALLOC_WITH_ID;
        rv = _bcm_dpp_am_sync_lif_alloc_align(unit, &sync_info,flags,align,offset,count,object);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_egress_encap_internal_alloc_align(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int align, int offset, int count, int* object)
{
    uint32 rv;
    SOC_PPD_EG_ENCAP_ACCESS_PHASE phase_access;
    bcm_dpp_am_egress_encap_t *encap_i;
    int *banks_ids = NULL;
    int nof_valid_banks, bank_id, new_bank;
    uint32 alloc_size;
    uint8 is_exist;
    uint8 is_success;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    encap_i = &EGRESS_ENCAP_CNTL(unit);

    rv = _bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(unit, allocation_info, &phase_access);
    BCMDNX_IF_ERR_EXIT(rv);

    alloc_size = sizeof(int) * encap_i->egress_encap_count;
    BCMDNX_ALLOC(banks_ids, alloc_size, "allocate bank ids");
    if (NULL == banks_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation bank ids")));
    }
    sal_memset(banks_ids, 0, alloc_size); 

    /* Try to allocate on each bank id unless we are dealing with case of WITH_ID */
    if (flags & (SHR_RES_ALLOC_WITH_ID)) {
        int object_id;
        SOC_PPD_EG_ENCAP_ACCESS_PHASE bank_phase_access;

        object_id = *object;
        /* Find matched bank */
        bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
        /* Validate bank exist and with the correct phase */
        rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist == 0) {
            uint32 alloc_bank_flags;

            alloc_bank_flags = SHR_RES_ALLOC_WITH_ID;
            /* Allocate new bank since it is not existed */
            rv = _bcm_dpp_am_egress_encap_allocate_new_bank(unit, is_sync, alloc_bank_flags, phase_access, &bank_id, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!is_success) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("egres encap resources didnt find new bank avaiable")));
            }

            rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, bank_id, is_sync);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Validate phase number */
        rv = _bcm_dpp_am_egress_encap_bank_phase_get(unit, bank_id, &bank_phase_access);
        BCMDNX_IF_ERR_EXIT(rv);

        if (bank_phase_access != phase_access) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("given invalid object id %d, bank phase access %d, not matched to phase access %d"), object_id, bank_phase_access, phase_access));
        }

        /* if replace flag is set and exist then we are done */
        if(flags & BCM_DPP_AM_FLAG_ALLOC_REPLACE) {
            rv = dpp_am_res_check(unit, bank_id, dpp_am_res_obs_eg_encap, offset, count);
            if(rv == BCM_E_EXISTS) {
                rv = BCM_E_NONE;
                BCM_EXIT;
            }
        }
        rv = dpp_am_res_alloc_align(unit, bank_id, dpp_am_res_obs_eg_encap, flags, align, offset, count, object);
        BCMDNX_IF_ERR_EXIT(rv);

    } else {
        uint8 alloc_success;

        if (flags & BCM_DPP_AM_FLAG_ALLOC_REPLACE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("cannot set replace when ID is not given")));
        }

        rv = _bcm_dpp_am_egress_encap_banks_by_access_phase(unit, is_sync, phase_access, &nof_valid_banks, banks_ids);
        BCMDNX_IF_ERR_EXIT(rv);

        alloc_success = FALSE;
        for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
            rv = dpp_am_res_alloc_align(unit, banks_ids[bank_id], dpp_am_res_obs_eg_encap, flags, align, offset, count, object);
            if (rv == BCM_E_NONE) {
                alloc_success = TRUE;

                /* Allocation succeded no need to find other banks */
                break;
            }
        }

        if (!alloc_success) {
            /* Allocate new bank since no success */
            rv = _bcm_dpp_am_egress_encap_allocate_new_bank(unit, is_sync, 0, phase_access, &new_bank, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_success) {
                rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, new_bank, is_sync);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Allocate object */
                rv = dpp_am_res_alloc_align(unit, new_bank, dpp_am_res_obs_eg_encap, flags, align, offset, count, object);
                BCMDNX_IF_ERR_EXIT(rv);

                alloc_success = TRUE;

            } else {
                /* Retreive from sync banks as last option (no more bank to allocate) */
                rv = _bcm_dpp_am_egress_encap_banks_by_access_phase(unit, TRUE, phase_access, &nof_valid_banks, banks_ids);
                BCMDNX_IF_ERR_EXIT(rv);

                for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
                    rv = dpp_am_res_alloc(unit, banks_ids[bank_id], dpp_am_res_obs_eg_encap, flags, count, object);
                    if (rv == BCM_E_NONE) {
                        alloc_success = TRUE;

                        /* Allocation succeded no need to find other banks */
                        break;
                    }
                }
            }
        }


        if (!alloc_success) {
            /* FAIL all full :( */
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("egres encap resources didnt find new bank avaiable")));
        }
    }

    BCM_EXIT;
exit:
    BCM_FREE(banks_ids);
    BCMDNX_FUNC_RETURN;
}

/* Deallocate entry according to object */
uint32
_bcm_dpp_am_egress_encap_dealloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, int count, int object)
{
    uint32 rv;
    int bank_id;
    uint8 is_sync_bank;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

    /* Allocate object */
    rv = _bcm_dpp_am_egress_encap_internal_dealloc(unit,allocation_info,FALSE,count,object);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sync in case of need */
    bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object);
    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit,bank_id,&is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync_bank) { /* allocated in synchronize bank, then needs to allocate also ingress */
        bcm_dpp_am_sync_lif_alloc_info_t sync_info;

        sal_memset(&sync_info,0,sizeof(sync_info));

        sync_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS; /* Sync Ingress */
        sync_info.egress_lif.pool_id = allocation_info->pool_id;
        sync_info.egress_lif.application_type = allocation_info->application_type;

        rv = _bcm_dpp_am_sync_lif_dealloc(unit, &sync_info,count,object);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}
STATIC uint32
_bcm_dpp_am_egress_encap_internal_dealloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, int count, int object)
{
    uint32 rv;
    uint8 is_exist;
    int bank_id;
    uint32 flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

   

    /* Find matched bank */
    bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object);
    /* Validate bank exist and with the correct phase */
    rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_exist) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("given invalid object id %d"), object));
    }

    rv = dpp_am_res_free_and_status(unit, bank_id, dpp_am_res_obs_eg_encap, count, object, &flags);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Is exist entry according to pool id */
int
_bcm_dpp_am_egress_encap_is_allocated(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, int count, int object)
{
    uint32 rv;
    int bank_id;
    uint8 bank_is_exist;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

    /* Find matched bank */
    bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object);
    /* Validate bank exist and with the correct phase */
    rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &bank_is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!bank_is_exist) {
        BCM_RETURN_VAL_EXIT(BCM_E_NONE);
    }

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, bank_id, dpp_am_res_obs_eg_encap, count, object));
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Egress encapsulation DB - End
 */

/*
 * Sync LIF DB - Start 
 * ARAD only 
 * Synchornization LIF DB is needed in cases where application allocation requires both Ingress & Egress 
 * The handling and management is done by a set of APIs called _bcm_dpp_am_sync_lif_* 
 * APIs can be called directly from application view or indirect from ingress_lif or egress_encap set of APIs  
 */

/* Initalize ingress lif allocation */
uint32
_bcm_dpp_am_sync_lif_init(int unit)
{
    bcm_dpp_am_sync_lif_t *sync_lif_i;   
    int alloc_size;

    BCMDNX_INIT_FUNC_DEFS;

    
    if (SOC_WARM_BOOT(unit))
    {
      
      /*BCM_EXIT;*/
    }

    sync_lif_i = &SYNC_LIF_CNTL(unit);
    
    if (sync_lif_i->init) {
        /* Nothing to do ingress lif already initialized */
        BCM_EXIT;
    }

    /* Initialize database */
    alloc_size = SHR_BITALLOCSIZE(_BCM_DPP_AM_SYNC_LIF_NOF_BITS);
    BCMDNX_ALLOC(sync_lif_i->sync_lif_banks, alloc_size, "Sync lif information bitmap");
    if (NULL == sync_lif_i->sync_lif_banks) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation sync lif information bitmap")));
    }

    sal_memset(sync_lif_i->sync_lif_banks, 0, alloc_size); 

    /* Done initalize */
    sync_lif_i->init = TRUE;

exit:
    BCMDNX_FUNC_RETURN;
}

/* De-Initalize ingress lif allocation */
uint32
_bcm_dpp_am_sync_lif_deinit(int unit)
{
    bcm_dpp_am_sync_lif_t *sync_lif_i;   

    BCMDNX_INIT_FUNC_DEFS;

    
    if (SOC_WARM_BOOT(unit))
    {
      
      /*BCM_EXIT;*/
    }

    sync_lif_i = &SYNC_LIF_CNTL(unit);
    
    if (!(sync_lif_i->init)) {
        /* Nothing to do ingress lif already de-initialized */
        BCM_EXIT;
    }

    /* De-Initialize database */
    BCM_FREE(sync_lif_i->sync_lif_banks);

    /* Done De-initalize */
    sync_lif_i->init = FALSE;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Check is sync application */
STATIC uint32
_bcm_dpp_am_sync_lif_is_sync_application(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, uint8 *is_exist)
{
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(is_exist);

    *is_exist = FALSE;
    if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) { /* Information is located at ingress */
        switch (allocation_info->ingress_lif.pool_id) {
        case dpp_am_res_lif_dynamic:
          if (allocation_info->ingress_lif.application_type == bcm_dpp_am_ingress_lif_app_vpn_ac) {
              *is_exist = TRUE;
          }
          break;
        case dpp_am_res_lif_pwe:
          *is_exist = TRUE;
          break;
        default:
          break;
        }
    }

    if (*is_exist) {
        BCM_EXIT;
    }

    if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) { /* Information is located at egress */
        switch (allocation_info->egress_lif.pool_id) {
        case dpp_am_res_lif_dynamic: /* Only used for INOUT-AC */
          *is_exist = TRUE;
          break;
        case dpp_am_res_eep_mpls_tunnel:
          if (allocation_info->egress_lif.application_type == bcm_dpp_am_egress_encap_app_pwe) {
              *is_exist = TRUE;
          }
          break;
        default:
          break;
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * SYNC LIF allocation
 */
uint32
_bcm_dpp_am_sync_lif_alloc(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, uint32 flags, int count, int* object)
{
    uint32 rv;
    uint8 is_sync_application = FALSE, is_sync_bank = FALSE;
    int object_id = 0;
    int bank_id = 0;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    /* Check it is sync application */
    rv = _bcm_dpp_am_sync_lif_is_sync_application(unit, allocation_info, &is_sync_application);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check it is sync bank */
    /* 
     * Sync bank: request for allocation on synchornization bank in application
     * that doesnt require LIF ID from both sides. 
     * This might be set in case all other banks are full except synchronize banks 
     * In that case, well skip on validation and just allocate ID 
     */
    if (!is_sync_application && (flags & (SHR_RES_ALLOC_WITH_ID))) {
        object_id = *object;
        if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) {
          bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
        }

        if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) {
          bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
        }

        rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* in case of no need of synchrnoziation, well just quit */
    if (!is_sync_bank && !is_sync_application) {
        BCM_EXIT;
    }

    /* Sync bank management */
    if (is_sync_bank) {
      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS)) {
          bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
          rv = dpp_am_res_alloc(unit, bank_id, dpp_am_res_obs_inlif, flags, count, object);
          /* In case it is not sync application well we just wanted to allocate ID, in case it is already occupied just ignore */
          if (!is_sync_application && rv == BCM_E_EXISTS) { 
              rv = BCM_E_NONE;
          } else {

            BCMDNX_IF_ERR_EXIT(rv);

          }
  
      }

      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS)) {
          bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
          rv = dpp_am_res_alloc(unit, bank_id, dpp_am_res_obs_eg_encap, flags, count, object);
          /* In case it is not sync application well we just wanted to allocate ID, in case it is already occupied just ignore */
          if (!is_sync_application && rv == BCM_E_RESOURCE) { 
              rv = BCM_E_NONE;
          } else {
            BCMDNX_IF_ERR_EXIT(rv);
          }

      }
    }


    /* Sync application management */
    if (is_sync_application) {
      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS)) {
          rv = _bcm_dpp_am_ingress_lif_internal_alloc(unit,&(allocation_info->ingress_lif), TRUE, flags,count,object);
          BCMDNX_IF_ERR_EXIT(rv);
          flags |= SHR_RES_ALLOC_WITH_ID;
      }

      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS)) {
          /* We cant get into here without WITH ID flag */
          if (!(flags & (SHR_RES_ALLOC_WITH_ID))) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, 
             (_BSL_BCM_MSG("in case of synchrnozie application egress side is only with id, sync_flags %d alloc flags %d"), 
              allocation_info->sync_flags, flags));
          }
          rv = _bcm_dpp_am_egress_encap_internal_alloc(unit,&(allocation_info->egress_lif), TRUE, flags,count,object);
          BCMDNX_IF_ERR_EXIT(rv);
      }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_sync_lif_alloc_align(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, uint32 flags, int align, int offset, int count, int* object)
{
    uint32 rv;
    uint8 is_sync_application = FALSE, is_sync_bank = FALSE;
    int object_id = 0, bank_id = 0;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    /* Check it is sync application */
    rv = _bcm_dpp_am_sync_lif_is_sync_application(unit, allocation_info, &is_sync_application);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check it is sync bank */
    /* 
     * Sync bank: request for allocation on synchornization bank in application
     * that doesnt require LIF ID from both sides. 
     * This might be set in case all other banks are full except synchronize banks 
     * In that case, well skip on validation and just allocate ID 
     */
    if (!is_sync_application && (flags & (SHR_RES_ALLOC_WITH_ID))) {
        object_id = *object;
        if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) {
          bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
        }

        if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) {
          bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
        }

        rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* in case of no need of synchrnoziation, well just quit */
    if (!is_sync_bank && !is_sync_application) {
        BCM_EXIT;
    }

    /* Sync bank management */
    if (is_sync_bank) {
      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS)) {
          bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
          rv = dpp_am_res_alloc_align(unit, bank_id, dpp_am_res_obs_inlif, flags, align, offset, count, object);
          BCMDNX_IF_ERR_EXIT(rv);
      }

      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS)) {
          bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
          rv = dpp_am_res_alloc_align(unit, bank_id, dpp_am_res_obs_eg_encap, flags, align, offset, count, object);
          BCMDNX_IF_ERR_EXIT(rv);
      }
    }


    /* Sync application management */
    if (is_sync_application) {
      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS)) {
          rv = _bcm_dpp_am_ingress_lif_internal_alloc_align(unit,&(allocation_info->ingress_lif), TRUE, flags,align,offset,count,object);
          BCMDNX_IF_ERR_EXIT(rv);
          flags |= SHR_RES_ALLOC_WITH_ID;
      }

      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS)) {
          /* We cant get into here without WITH ID flag */
          if (SOC_IS_ARAD_B1_AND_BELOW(unit) && (!(flags & (SHR_RES_ALLOC_WITH_ID)))) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, 
             (_BSL_BCM_MSG("in case of synchrnozie application egress side is only with id, sync_flags %d alloc flags %d"), 
              allocation_info->sync_flags, flags));
          }
          rv = _bcm_dpp_am_egress_encap_internal_alloc_align(unit,&(allocation_info->egress_lif), TRUE, flags,align,offset,count,object);
          BCMDNX_IF_ERR_EXIT(rv);
      }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


uint32
_bcm_dpp_am_sync_lif_dealloc(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, int count, int object)
{
    uint32 rv;
    uint8 is_sync_bank = FALSE;
    int bank_id = 0;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

    if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) {
      bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object);
    }

    if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) {
      bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object);
    }

    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    /* in case of no need of synchrnoziation, well just quit */
    if (!is_sync_bank) {
        BCM_EXIT;
    }

    if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS)) {
        rv = _bcm_dpp_am_ingress_lif_internal_dealloc(unit,&(allocation_info->ingress_lif), TRUE, count,object);
        if (rv == BCM_E_NOT_FOUND) { /* there might be a case of request for dealloc twice */
          rv = BCM_E_NONE;
        }
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS)) {
        rv = _bcm_dpp_am_egress_encap_internal_dealloc(unit,&(allocation_info->egress_lif), TRUE, count,object);
        if (rv == BCM_E_NOT_FOUND) {
          rv = BCM_E_NONE;
        }
        BCMDNX_IF_ERR_EXIT(rv);
    }


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
/* Sync LIF DB - End */

/* Global LIF DB - Start */


/*
 * GLOBAL LIF INTERNAL FUNCTIONS - START
 */


/*
 * Function:
 *     _bcm_dpp_am_global_lif_init
 * Purpose:
 *     Init the global lif variables - the bank types and lif counters.
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32 
_bcm_dpp_am_global_lif_init(int unit){
    uint32 rv;
    int bank_id_iter, count;
    bcm_dpp_am_global_lif_bank_type_t bank_type;
    int nof_rif_banks;
    int illegal_egress_lif;
    BCMDNX_INIT_FUNC_DEFS;

    /* Find out the number of rif banks so we can set them to illegal banks. */
    nof_rif_banks = _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(SOC_DPP_CONFIG(unit)->l3.nof_rifs);

    /* Set banks types. All banks should be set to "none" type, other than those permanently invalid. */
    for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_NOF_GLOBAL_LIF_BANKS(unit) ; bank_id_iter++) {

        /* 
         *  Set egress bank.
         */

        if (bank_id_iter < nof_rif_banks) {
            /* The only permanent constraint, for now, is for egress rif banks. */
            bank_type = bcm_dpp_am_global_lif_bank_type_invalid;
        } else {
            bank_type = bcm_dpp_am_global_lif_bank_type_none;
        }

        rv = _bcm_dpp_am_global_lif_bank_type_set(unit, bank_id_iter, FALSE, bank_type);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         *  Set ingress bank.
         */
        bank_type = bcm_dpp_am_global_lif_bank_type_none;

        rv = _bcm_dpp_am_global_lif_bank_type_set(unit, bank_id_iter, TRUE, bank_type);
        BCMDNX_IF_ERR_EXIT(rv);
    }


    /* Reset global lif counters. */

    count = 0;
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_egress_count.set(unit, count);
    BCMDNX_IF_ERR_EXIT(rv);
    
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_ingress_count.set(unit, count);
    BCMDNX_IF_ERR_EXIT(rv);


    /* Allocate the illegal egress global lif */
    illegal_egress_lif = SOC_PPC_INVALID_GLOBAL_LIF;
    rv = _bcm_dpp_am_global_lif_internal_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, _BCM_DPP_AM_SYNC_LIF_EGRESS, &illegal_egress_lif);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_lif_id_is_global_lif_id
 * Purpose:
 *     Given a global lif id and a direction, returns whether it's a mapped lif, or a direct lif,
 *      according to HW setting.
 *      Not that in ARAD, lifs are always direct.
 * Parameters:
 *     unit       - Device number
 *     is_ingress - TRUE if ingress lif id, FALSE if egress.
 *     global_lif - Global lif id to be checked.
 *     is_mapped  - Will be filled with TRUE if lif is mapped and FALSE is lif is direct.
 * Returns:
 *     BCM_E_*      - If there was an error.
 *     BCM_E_NONE   - Otherwise.
 */
int
_bcm_dpp_lif_id_is_mapped(int unit, uint8 is_ingress, int global_lif, uint8 *is_global){
    int rv = BCM_E_NONE;
    uint8 is_mapped_bank; 
    BCMDNX_INIT_FUNC_DEFS;


    *is_global = FALSE;

    /* Aradplus and below don't use global lifs. */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCM_EXIT;
    }

    /* 
     *  If the bank is dynamically legal, then it can contain only mapped lifs.
     */
    rv = _bcm_dpp_am_global_lif_bank_is_legal(unit, _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(global_lif), is_ingress, &is_mapped_bank);
    BCMDNX_IF_ERR_EXIT(rv); 
    if (!is_mapped_bank){ 
        BCM_EXIT;
    } 

    /* If we made it here, it means the lif is a global lif. */
    *is_global = TRUE;
    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_id_is_sync
 * Purpose:
 *     Given a global lif id, returns whether it's a sync global lif id, or if the ingress and egress global lifs are unrelated.
 * Parameters:
 *     unit       - Device number
 *     global_lif - Global lif id to be checked.
 *     is_sync    - Will be filled with TRUE if lif is sync and FALSE is lif is not sync.
 * Returns:
 *     BCM_E_*      - If there was an error.
 *     BCM_E_NONE   - Otherwise.
 */
int 
_bcm_dpp_global_lif_id_is_sync(int unit, int global_lif, uint8 *is_sync){
    int rv = BCM_E_NONE;
    int bank_id;
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("_bcm_dpp_lif_id_is_sync is unavailable for ARADPLUS and below")));
    }

    bank_id = _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(global_lif);
    
    rv = _bcm_dpp_am_global_lif_bank_is_sync_bank(unit, bank_id, is_sync);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_global_sync_lif_internal_alloc
 * Purpose:
 *     See description of bcm_dpp_am_global_sync_lif_alloc
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32
_bcm_dpp_am_global_sync_lif_internal_alloc(int unit, uint32 flags, int *global_lif){
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* Verify that there are enough ingress and egress global lifs available. */
    rv = _bcm_dpp_am_global_lif_egress_alloc_verify(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_am_global_lif_ingress_alloc_verify(unit);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* Allocate */
    rv = _bcm_dpp_am_global_lif_internal_alloc(unit, flags, _BCM_DPP_AM_SYNC_LIF_EGRESS | _BCM_DPP_AM_SYNC_LIF_INGRESS, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update lif counts. */
    rv = _bcm_dpp_am_global_lif_update_egress_count(unit, 1);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_am_global_lif_update_ingress_count(unit, 1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_global_sync_lif_internal_dealloc
 * Purpose:
 *     See description of bcm_dpp_am_global_sync_lif_dealloc
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32
_bcm_dpp_am_global_sync_lif_internal_dealloc(int unit, int global_lif){
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* Dellocate */
    rv = _bcm_dpp_am_global_lif_internal_dealloc(unit, _BCM_DPP_AM_SYNC_LIF_EGRESS | _BCM_DPP_AM_SYNC_LIF_INGRESS, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update lif counts. */
    rv = _bcm_dpp_am_global_lif_update_egress_count(unit, -1);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_am_global_lif_update_ingress_count(unit, -1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_egress_lif_internal_alloc
 * Purpose:
 *     See description of bcm_dpp_am_global_egress_lif_alloc
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32
_bcm_dpp_am_global_egress_lif_internal_alloc(int unit, uint32 flags, int *global_lif){
    uint32 rv;
    int bank_id;
    uint8 is_sync;

    BCMDNX_INIT_FUNC_DEFS;

    /* Verify that there are enough ingress and egress global lifs available. */
    rv = _bcm_dpp_am_global_lif_egress_alloc_verify(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Allocate */
    rv = _bcm_dpp_am_global_lif_internal_alloc(unit, flags, _BCM_DPP_AM_SYNC_LIF_EGRESS, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update lif counts. */
    rv = _bcm_dpp_am_global_lif_update_egress_count(unit, 1);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     *  If the lif was allocated on a sync bank, allocate ingress as well.
     */

    /* Check bank. */
    bank_id = _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(*global_lif);
    rv = _bcm_dpp_am_global_lif_bank_is_sync_bank(unit, bank_id, &is_sync);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync) {
        flags |= SHR_RES_ALLOC_WITH_ID;

        /* Allocate. No need to allocate count, because we don't actually use this lif. */
        rv = _bcm_dpp_am_global_lif_internal_alloc(unit, flags, _BCM_DPP_AM_SYNC_LIF_INGRESS, global_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_egress_lif_internal_is_alloc
 * Purpose:
 *     See description of bcm_dpp_am_global_egress_lif_is_alloc
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32             
_bcm_dpp_am_global_egress_lif_internal_is_alloced(int unit, int global_lif){
    uint32 rv;
    uint8 is_mapped = TRUE;

    BCMDNX_INIT_FUNC_DEFS;

    /* First, check if it's a global lif, or local (direct) lif */
    rv = _bcm_dpp_lif_id_is_mapped(unit, 0, global_lif, &is_mapped);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_mapped) {
        rv = _bcm_dpp_am_local_outlif_is_alloc(unit, global_lif);
        BCM_RETURN_VAL_EXIT(rv);
    } else {
        rv = _bcm_dpp_am_global_lif_internal_is_alloc(unit, _BCM_DPP_AM_SYNC_LIF_EGRESS, global_lif);
        BCM_RETURN_VAL_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_egress_lif_internal_dealloc
 * Purpose:
 *     See description of bcm_dpp_am_global_egress_lif_dealloc
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32             
_bcm_dpp_am_global_egress_lif_internal_dealloc(int unit, int global_lif){
    uint32 rv;
    uint32 sync_flags;
    uint8 is_sync;
    int bank_id;
    BCMDNX_INIT_FUNC_DEFS;

    /* 
     * If it's a sync bank, deallocate ingress lif as well. 
     */

    /* Check if bank is sync bank. */
    bank_id = _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(global_lif);
    rv = _bcm_dpp_am_global_lif_bank_is_sync_bank(unit, bank_id, &is_sync);
    BCMDNX_IF_ERR_EXIT(rv);

    /* If sync, deallocate ingress. */
    sync_flags = _BCM_DPP_AM_SYNC_LIF_EGRESS;
    sync_flags |= (is_sync) ? _BCM_DPP_AM_SYNC_LIF_INGRESS : 0;


    /* Dellocate */
    rv = _bcm_dpp_am_global_lif_internal_dealloc(unit, sync_flags, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update egress lif count. */
    rv = _bcm_dpp_am_global_lif_update_egress_count(unit, -1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_ingress_lif_internal_alloc
 * Purpose:
 *     See description of bcm_dpp_am_global_ingress_lif_alloc
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32
_bcm_dpp_am_global_ingress_lif_internal_alloc(int unit, uint32 flags, int *global_lif){
    uint32 rv;
    int bank_id;
    uint8 is_sync;

    BCMDNX_INIT_FUNC_DEFS;

    /* Verify that there are enough ingress and egress global lifs available. */
    rv = _bcm_dpp_am_global_lif_ingress_alloc_verify(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Allocate */
    rv = _bcm_dpp_am_global_lif_internal_alloc(unit, flags, _BCM_DPP_AM_SYNC_LIF_INGRESS, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update lif counts. */
    rv = _bcm_dpp_am_global_lif_update_ingress_count(unit, 1);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     *  If the lif was allocated on a sync bank, allocate ingress as well.
     */

    /* Check bank */
    bank_id = _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(*global_lif);
    rv = _bcm_dpp_am_global_lif_bank_is_sync_bank(unit, bank_id, &is_sync);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync) {
        flags |= SHR_RES_ALLOC_WITH_ID;

        /* Allocate. No need to allocate count, because we don't actually use this lif. */
        rv = _bcm_dpp_am_global_lif_internal_alloc(unit, flags, _BCM_DPP_AM_SYNC_LIF_EGRESS, global_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_ingress_lif_internal_is_alloc
 * Purpose:
 *     See description of bcm_dpp_am_global_ingress_lif_is_alloc
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32             
_bcm_dpp_am_global_ingress_lif_internal_is_alloced(int unit, int global_lif){
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_global_lif_internal_is_alloc(unit, _BCM_DPP_AM_SYNC_LIF_INGRESS, global_lif);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_ingress_lif_internal_dealloc
 * Purpose:
 *     See description of bcm_dpp_am_global_ingress_lif_dealloc
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32             
_bcm_dpp_am_global_ingress_lif_internal_dealloc(int unit, int global_lif){
    uint32 rv;
    uint32 sync_flags;
    uint8 is_sync;
    int bank_id;

    BCMDNX_INIT_FUNC_DEFS;

    /* 
     * If it's a sync bank, deallocate egress lif as well. 
     */

    /* Check if bank is sync bank. */
    bank_id = _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(global_lif);
    rv = _bcm_dpp_am_global_lif_bank_is_sync_bank(unit, bank_id, &is_sync);
    BCMDNX_IF_ERR_EXIT(rv);

    /* If sync, deallocate ingress. */
    sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS;
    sync_flags |= (is_sync) ? _BCM_DPP_AM_SYNC_LIF_EGRESS : 0;


    /* Dellocate */
    rv = _bcm_dpp_am_global_lif_internal_dealloc(unit, sync_flags, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update ingress lif count. */
    rv = _bcm_dpp_am_global_lif_update_ingress_count(unit, -1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * GLOBAL LIF GENERIC INTERNAL FUNCTIONS 
 */

/** 
  * The global lif allocation mechanism is as following: 
  *  
  * Environment: 
  * - The global lif allocation range is divided into banks, representing the mapped-lif vs. direct-lif 
  *     bank division in the egress lif table.
  * - There are GLOBAL_LIF_MAX_NUMBER / NOF_LIFS_PER_BANK banks for both egress and ingress, each with NOF_LIFS_PER_BANK entries. 
  * - The first banks, which are reserved for rifs, can't be used to allocate egress global lifs.  
  *  
  * - A bank can be either sync or non-sync. 
  *     - Sync banks always allocate and deallocate both ingress and egress entries.
  *     - Non-sync banks allocate and deallocate ingress and egress seperatly.
  * 
  * - All banks start as "none" banks, except for invalid banks. 
  * - The bank type is dynamically set at runtime, and can be changed. 
  *  
  * - There are several constraints on what can be allocated in each bank: 
  *     - Direct-lif banks can't be used to allocate egress lifs (HW constraint).
  *     - Direct-lif banks can be used to allocate ingress lifs, but only WITH_ID (API constraint).
  *     - Sync entries can't be allocated in non-sync banks (SW DB contarint).
  *     - Non-sync entries can be allocated in sync banks. The symmetric entry will be allocated as well, but not used.
  *  
  * - It is better to use a bank with a higher ID range, because there can't be direct-lifs in this range.
  *  
  * Allocation algorithm: 
  *  
  * 1. Make sure there the global lif amount did not exceed the maximum (The Glem's size for egress, or the number of ingress lifs 
  *     for ingress).
  *  
  * 2.1 If the allocation is WITH_ID: 
  *     2.1.1. Make sure that the entry can be allocated in this bank (see constaraints).
  *     2.1.2. Allocate.
  *  
  * 2.2 Otherwise, allocate a new ID. 
  *     2.2.1. Get all sync / non-sync banks according to the entry's type, from highest to lowest.
  *     2.2.2. Try to allocate on each bank, until the allocation is successful.
  *     2.2.3. If the allocation failed, get a "none" bank, and allocate 
  *     2.2.4. If there are no "none" banks (Probably impossible, because the ID range is much bigger than the number of
  *             lifs that can be allocated):
  *         2.2.4.1 If it's a non-sync entry: allocate in a sync bank
  *         2.2.4.2 If it's a sync entry, the allocation fails.
  *  
  * 3. If that's the first allocated entry in this bank update the bank's type.
  *  
  * 4. Update the counter for the allocated direction (ingress/ egress/ both).
  *  
  * 5. If the entry is a non-sync entry that was allocated in a sync bank, allocate the symmetric entry, but don't update the counter, 
  *     because it's not a real entry.
  *  
  *  
  * Deallocation algorithm: 
  *  
  * 1. Make sure the id is legal, and that it's not a sync entry in a non-sync bank. 
  * 2. Deallocate the entry from the bank, and update the counter. 
  * 3. If the entry is a non-sync entry in a sync bank, deallocate the symmetric entry, but don't update the counter.
  * 4. If that's the last entry in this bank, update the bank's type (back to none, or to ingress/ egress only).
  *  
  */

/*
 * Function:
 *     _bcm_dpp_am_global_lif_internal_alloc
 * Purpose:
 *     Allocate either a global ingress, egress or sync lif, according to the sync_flags.
 *     See the algorithm above.
 *  
 * Parameters:
 *     unit         - (IN) Device number
 *     alloc_flags  - (IN) Resource manager flags.
 *     sync_flags   - (IN) _BCM_DPP_AM_SYNC_LIF_INGRESS, _BCM_DPP_AM_SYNC_LIF_EGRESS or both for sync entry.
 *     global_lif   - (INOUT) Will be filled with the global lif's id. If WITH_ID alloc flag is provided, Id must be provided.
 * Returns: 
 *     BCM_E_NONE       - If allocation is successful. 
 *     BCM_E_PARAM      - If WITH_ID flag is set and given global lif is illegal.
 *     BCM_E_RESOURCE   - If lif banks are full.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_global_lif_internal_alloc(int unit, uint32 alloc_flags, uint32 sync_flags, int *global_lif){
    uint32 rv;
    int bank_id = 0;
    uint8 is_legal;
    uint8 is_success;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(global_lif);

    /* If with id, make sure the bank is legal, and allocate. */
    if (alloc_flags & (SHR_RES_ALLOC_WITH_ID)) {
        int object_id;

        if (!_BCM_DPP_AM_GLOBAL_LIF_ID_IS_LEGAL(unit, *global_lif)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given lif id is illegal.")));
        }

        object_id = *global_lif;
        /* Find matched bank */
        bank_id = _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(object_id);
        /* Validate bank exists and matches type */
        rv = _bcm_dpp_am_global_lif_is_legal_bank(unit, sync_flags, _BCM_DPP_AM_GLOBAL_LIF_BANK_IS_LEGAL_ALLOCATE_WITH_ID, bank_id, &is_legal);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_legal) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given lif id can't be allocated.")));
        }

        /* Add resource. */
        rv = _bcm_dpp_am_global_lif_res_alloc(unit, alloc_flags, sync_flags, bank_id, global_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Update bank type if necessary. */
        rv = _bcm_dpp_am_global_lif_update_bank_and_type(unit, sync_flags, bank_id);
        BCMDNX_IF_ERR_EXIT(rv);

    } else { /* Allocate without ID. */
        uint8 alloc_success;

        rv = _bcm_dpp_am_global_lif_internal_alloc_by_type(unit, alloc_flags, sync_flags, global_lif, &alloc_success);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!alloc_success) {
            /* Current banks are full, so allocate a new bank. */
            rv = _bcm_dpp_am_global_lif_allocate_new_bank(unit, sync_flags, &bank_id, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_success) {
                /* Allocate object */
                rv = _bcm_dpp_am_global_lif_res_alloc(unit, alloc_flags, sync_flags, bank_id, global_lif);
                BCMDNX_IF_ERR_EXIT(rv);
                alloc_success = TRUE;
            } else if (!((sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) && (sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS))) {
               /* If all asymmetric banks are full, use a symmetric bank.  */
               rv = _bcm_dpp_am_global_lif_internal_alloc_by_type(unit, alloc_flags, _BCM_DPP_AM_SYNC_LIF_EGRESS | _BCM_DPP_AM_SYNC_LIF_INGRESS, 
                                                                  global_lif, &alloc_success);
               BCMDNX_IF_ERR_EXIT(rv);
               alloc_success = TRUE;
            } 

            if (!alloc_success) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("All global lif banks are full.")));
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_internal_dealloc
 * Purpose:
 *     Deallocate either a global ingress, egress or sync lif, according to the sync_flags.
 *     See the algorithm above.
 *  
 * Parameters:
 *     unit         - (IN) Device number
 *     sync_flags   - (IN) _BCM_DPP_AM_SYNC_LIF_INGRESS, _BCM_DPP_AM_SYNC_LIF_EGRESS or both for sync entry. 
 *     global_lif_id- (IN) The global lif to be deallocated.
 * Returns: 
 *     BCM_E_NONE       - If deallocation is successful. 
 *     BCM_E_PARAM      - If the lif id is illegal, or sync flags don't fit the bank type. 
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_global_lif_internal_dealloc(int unit, uint32 sync_flags, int global_lif_id){
    uint32 rv;
    uint8 is_legal, clear_bank;
    int bank_id;
    uint32 flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* Input validation. */
    if (!_BCM_DPP_AM_GLOBAL_LIF_ID_IS_LEGAL(unit, global_lif_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given lif id is illegal.")));
    }

    /* Find matched bank */
    bank_id = _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(global_lif_id);

    /* Validate bank exists and matches type */
    rv = _bcm_dpp_am_global_lif_is_legal_bank(unit, sync_flags, _BCM_DPP_AM_GLOBAL_LIF_BANK_IS_LEGAL_ALLOCATED_BANK_ONLY, bank_id, &is_legal);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_legal) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given lif doesn't exist. %d"), global_lif_id));
    }

    clear_bank = FALSE;

    /* Deallocate egress. */
    if (sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) {
        rv = dpp_am_res_free_and_status(unit, bank_id, dpp_am_res_global_outlif, 1, global_lif_id, &flags);
        BCMDNX_IF_ERR_EXIT(rv);

        clear_bank = clear_bank || (flags & (SHR_RES_FREED_TYPE_LAST_ELEM | SHR_RES_FREED_POOL_LAST_ELEM));
    }
    
    /* Deallocate ingress. */
    if (sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) {
        rv = dpp_am_res_free_and_status(unit, bank_id, dpp_am_res_global_inlif, 1, global_lif_id, &flags);
        BCMDNX_IF_ERR_EXIT(rv);

        clear_bank = clear_bank || (flags & (SHR_RES_FREED_TYPE_LAST_ELEM | SHR_RES_FREED_POOL_LAST_ELEM));
    }

    /* Update bank type if necessary. */
    if (clear_bank) {
        rv = _bcm_dpp_am_global_lif_clear_bank_and_type(unit, sync_flags, bank_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }    

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_internal_is_alloc
 * Purpose:
 *     Check if a lif exists. 
 *  
 * Parameters:
 *     unit         - (IN) Device number
 *     sync_flags   - (IN) _BCM_DPP_AM_SYNC_LIF_INGRESS xor _BCM_DPP_AM_SYNC_LIF_EGRESS.
 *     global_lif_id- (IN) The global lif to check.
 * Returns: 
 *     BCM_E_EXISTS     - If lif exists.
 *     BCM_E_NOT_FOUND  - If the lif doesn't exist.
 *     BCM_E_PARAM      - If the lif id is illegal.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_global_lif_internal_is_alloc(int unit, uint32 sync_flags, int global_lif_id){
    uint32 rv;
    int bank_id, res_id;

    BCMDNX_INIT_FUNC_DEFS;

    if (!_BCM_DPP_AM_GLOBAL_LIF_ID_IS_LEGAL(unit, global_lif_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given lif id is illegal.")));
    }

    bank_id = _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(global_lif_id);

    /* Find matched bank */
    res_id = (sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) ? dpp_am_res_global_outlif : dpp_am_res_global_inlif;

    rv = dpp_am_res_check(unit, bank_id, res_id, 1, global_lif_id);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 * GLOBAL LIF INTERNAL FUNCTIONS - END
 * GLOBAL LIF BANK MANAGEMENT FUNCTIONS - START
 */

/*
 * Function:
 *     _bcm_dpp_am_global_lif_bank_is_sync_bank
 * Purpose:
 *     Given a bank id, returns whether the bank is a sync bank or a none sync bank.
 *  
 * Parameters:
 *     unit         - (IN) Device number
 *     bank_id      - (IN) Bank id to be checked.
 *     is_sync      - (OUT) Will be TRUE if the bank is sync, or FALSE if it's not.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_bank_is_sync_bank(int unit, int bank_id, uint8 *is_sync){
    uint32 rv;
    bcm_dpp_am_global_lif_bank_type_t bank_type;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get bank type from ingress. We could have asked for egress as well since it's symmetric. */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_ingress_bank_types.get(unit, bank_id, &bank_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Sync banks use the "sync" bank type. */
    *is_sync = (bank_type == bcm_dpp_am_global_lif_bank_type_sync);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_bank_is_legal
 * Purpose:
 *      Check whether the given ingress/egress bank can currently be allocated.
 *      Note that this is for constraints that change dynamically during the device's run. Permanent constraints are
 *      set at device bringup and make the bank type invalid.
 *  
 *      See the allocation algorithm above for full details.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     bank_id          - (IN) Bank id to be checked.
 *     is_ingress       - (IN) Whether to check ingress bank (FALSE means egress bank).
 *     is_legal         - (OUT)Whether the bank can be currently allocated.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32 
_bcm_dpp_am_global_lif_bank_is_legal(int unit, int bank_id, uint8 is_ingress, uint8 *is_legal){
    uint32 rv;
    uint8 is_direct_bank;
    BCMDNX_INIT_FUNC_DEFS;

    if (is_ingress) {
        /* In the future, add dynamic ingress constraints here. */
        *is_legal = TRUE;
    } else {
        /* Egress global lifs can't be allocated in direct mapped banks. */
        rv = _bcm_dpp_am_egress_bank_is_direct_bank(unit, bank_id, &is_direct_bank);
        BCMDNX_IF_ERR_EXIT(rv);
        *is_legal = !is_direct_bank;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_is_legal_bank_by_type
 * Purpose:
 *      Check whether the given ingress/egress bank's type matches the sync/none sync entry. Can be used to check whether an entry can be
 *      allocated or deallocated from this bank.
 *  
 *      See the allocation algorithm above for full details.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     bank_id          - (IN) Bank id to be checked.
 *     is_ingress       - (IN) Whether to check ingress bank (FALSE means egress bank).
 *     is_sync          - (IN) Whether the entry is sync or none-sync.
 *     allocated_bank_only (IN)Will cause none-allocated bank to return FALSE.
 *     is_legal         - (OUT)Whether the bank can be used for the entry type.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_is_legal_bank_by_type(int unit, int bank_id, uint8 is_ingress, uint8 is_sync, uint32 bank_alloc_flags, uint8 *is_legal) {
    bcm_dpp_am_global_lif_bank_type_t bank_type;
    uint32 rv;
    uint8 with_id, allocated_bank_only;
    BCMDNX_INIT_FUNC_DEFS;

    with_id = bank_alloc_flags & _BCM_DPP_AM_GLOBAL_LIF_BANK_IS_LEGAL_ALLOCATE_WITH_ID;

    allocated_bank_only = bank_alloc_flags & _BCM_DPP_AM_GLOBAL_LIF_BANK_IS_LEGAL_ALLOCATED_BANK_ONLY;


    
    /* Get bank type. */
    if (is_ingress) {
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_ingress_bank_types.get(unit, bank_id, &bank_type);
    } else {
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_egress_bank_types.get(unit, bank_id, &bank_type);
    }
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check legality by bank type. */
    switch (bank_type) {
    case bcm_dpp_am_global_lif_bank_type_invalid:
        /* No action can be done in an illegal bank. */
        *is_legal = FALSE;
        break;
    case bcm_dpp_am_global_lif_bank_type_sync:
        /* Only sync entries or with_id entries can be allocated in sync banks. */
        *is_legal = is_sync || with_id;
        break;
    case bcm_dpp_am_global_lif_bank_type_allocated:
        /* Only non sync entries can be allocated in non sync banks. */
        *is_legal = !is_sync;
        break;
    case bcm_dpp_am_global_lif_bank_type_none:
        if (allocated_bank_only) {
            /* None allocated bank must be false for this flag. */
            *is_legal = FALSE;
        } else {
            /* Check if this bank can be allocated. */
            rv = _bcm_dpp_am_global_lif_bank_is_legal(unit, bank_id, is_ingress, is_legal);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unkonwn bank type.")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_is_legal_bank
 * Purpose:
 *      Check whether the given bank's type matches the given sync flags. Can be used to check whether an entry can be
 *      allocated or deallocated from this bank.
 *  
 *      See the allocation algorithm above for full details.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     sync_flags       - (IN) Sync flags for the entry. _BCM_DPP_AM_SYNC_LIF_INGRESS, _BCM_DPP_AM_SYNC_LIF_EGRESS or both for sync entry. 
 *     bank_alloc_flags - (IN) _BCM_DPP_AM_GLOBAL_LIF_BANK_IS_LEGAL_ALLOCATED_BANK_ONLY to only return TRUE if the bank
 *                                  is already allocated (not NONE type).
 *                             _BCM_DPP_AM_GLOBAL_LIF_BANK_IS_LEGAL_ALLOCATE_WITH_ID to return TRUE for non-sync entries allocated in
 *                                  sync banks.
 *     bank_id          - (IN) Bank id to be checked.
 *     is_legal         - (OUT)Whether the bank can be used for the entry type.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_is_legal_bank(int unit, uint32 sync_flags, uint32 bank_alloc_flags, int bank_id, uint8 *is_legal){
    uint32 rv;
    uint8 is_ingress, is_egress, is_sync;
    uint8 is_egress_legal, is_ingress_legal;
    BCMDNX_INIT_FUNC_DEFS;

    /* Fill booleans. */
    is_ingress = sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS;
    is_egress = sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS;
    is_sync = is_ingress && is_egress;

    /* Entry must be ingress or egress. */
    if (!is_ingress && !is_egress) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Given sync flags must be either ingress or egress.")));
    }

    /* If only one direction is not checked, the other shouldn't be illegal */
    is_egress_legal = TRUE; 
    is_ingress_legal = TRUE;


    /* Check if ingress bank is legal. */
    if (is_ingress) {
        rv = _bcm_dpp_am_global_lif_is_legal_bank_by_type(unit, bank_id, TRUE, is_sync, bank_alloc_flags, &is_ingress_legal);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Check if egress bank is legal. */
    if (is_egress) {
        rv = _bcm_dpp_am_global_lif_is_legal_bank_by_type(unit, bank_id, FALSE, is_sync, bank_alloc_flags, &is_egress_legal);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    *is_legal = is_ingress_legal && is_egress_legal;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_egress_bank_is_direct_bank
 * Purpose:
 *      Checks whether the given egress bank is direct mapped. Egress global lif can't be allocated in these banks.
 *  
 *      NOT IMPLEMENTED YET - Always returns FALSE.
 *  
 *      See the allocation algorithm above for full details.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     bank_id          - (IN) Egress bank id to be checked.
 *     is_direct_bank   - (OUT)Will be TRUE if direct mapped, false otherwise.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_egress_bank_is_direct_bank(int unit, int bank_id, uint8 *is_direct_bank) {
    int rv;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;
    BCMDNX_INIT_FUNC_DEFS;

    if (bank_id < _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS) {
        /* If the bank is in the range of real EEDB lines, then it's mode should be checked. */
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id);
        BCMDNX_IF_ERR_EXIT(rv);

        *is_direct_bank = (eedb_bank_info.type == BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT);
    } else {
        /* If the bank is above the range, then it can't be direct lif, because it's not a legal EEDB line. */
        *is_direct_bank = FALSE;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_update_bank_and_type
 * Purpose:
 *      To be called after an entry was allocatd. Updates the bank type according to the allocated entry.
 *      The function assumes that _bcm_dpp_am_global_lif_is_legal_bank was called before this function, so
 *      no sanity check is performed.
 *  
 *      The new bank type will be bcm_dpp_am_global_lif_bank_type_sync for sync entries, and bcm_dpp_am_global_lif_bank_type_allocated
 *      for none sync entries.
 *  
 *      See the allocation algorithm above for full details.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     sync_flags       - (IN) Sync flags of the allocated entry. _BCM_DPP_AM_SYNC_LIF_INGRESS, _BCM_DPP_AM_SYNC_LIF_EGRESS or both for sync entry. 
 *     bank_id          - (IN) Bank id to be updated.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_update_bank_and_type(int unit, uint32 sync_flags, int bank_id){
    uint32 rv;
    bcm_dpp_am_global_lif_bank_type_t new_bank_type;
    uint8 is_ingress, is_egress;
    BCMDNX_INIT_FUNC_DEFS;

    is_ingress = sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS;
    is_egress = sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS;

    if (!is_ingress && !is_egress) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Given sync flags must be either ingress or egress.")));
    }

    /* Set the new bank type. */
    new_bank_type = (is_ingress && is_egress) ? bcm_dpp_am_global_lif_bank_type_sync
                                               : bcm_dpp_am_global_lif_bank_type_allocated;
    /* Update ingress bank. */
    if (is_ingress) {
        rv = _bcm_dpp_am_global_lif_bank_type_set(unit, bank_id, TRUE, new_bank_type);
        BCMDNX_IF_ERR_EXIT(rv);
        LOG_VERBOSE(BSL_LS_BCM_COMMON, (BSL_META_U(unit, "unit %d, Ingress global lif bank #%d has been allocated as a %s bank.\n"), 
                                     unit, bank_id, (is_egress) ? "sync" : "non-sync"));
    }

    /* Update egress bank*/
    if (is_egress) {
        rv = _bcm_dpp_am_global_lif_bank_type_set(unit, bank_id, FALSE, new_bank_type);
        BCMDNX_IF_ERR_EXIT(rv);
        LOG_VERBOSE(BSL_LS_BCM_COMMON, (BSL_META_U(unit, "unit %d, Egress global lif bank #%d has been allocated as a %s bank.\n"), 
                                     unit, bank_id, (is_ingress) ? "sync" : "non-sync"));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_clear_bank_and_type
 * Purpose:
 *      To be called after an entry was deallocatd. Updates the bank type according to the deallocated entry.
 *  
 *      The bank type will be changed to bcm_dpp_am_global_lif_bank_type_none
 *  
 *      See the allocation algorithm above for full details.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     sync_flags       - (IN) Sync flags of the deallocated entry. _BCM_DPP_AM_SYNC_LIF_INGRESS, _BCM_DPP_AM_SYNC_LIF_EGRESS or both for sync entry. 
 *     bank_id          - (IN) Bank id to be updated.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_clear_bank_and_type(int unit, uint32 sync_flags, int bank_id) {
    uint32 rv;
    uint8 is_ingress, is_egress;
    BCMDNX_INIT_FUNC_DEFS;

    is_ingress = sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS;
    is_egress = sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS;

    if (!is_ingress && !is_egress) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Given sync flags must be either ingress or egress.")));
    }

    /* Update ingress bank. */
    if (is_ingress) {
        rv = _bcm_dpp_am_global_lif_bank_type_set(unit, bank_id, TRUE, bcm_dpp_am_global_lif_bank_type_none);
        BCMDNX_IF_ERR_EXIT(rv);
        LOG_VERBOSE(BSL_LS_BCM_COMMON, (BSL_META_U(unit, "unit %d, Ingress global lif bank #%d has been deallocated. \n"), 
                                     unit, bank_id));
    }

    /* Update egress bank*/
    if (is_egress) {
        rv = _bcm_dpp_am_global_lif_bank_type_set(unit, bank_id, FALSE, bcm_dpp_am_global_lif_bank_type_none);
        BCMDNX_IF_ERR_EXIT(rv);
        LOG_VERBOSE(BSL_LS_BCM_COMMON, (BSL_META_U(unit, "unit %d, Egress global lif bank #%d has been deallocated. \n"), 
                                     unit, bank_id));
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_allocate_new_bank
 * Purpose:
 *      To be called when trying to allocate an entry and all current banks are full. Not relevant for WITH_ID entries.
 *      Will search for a "none" bank, change its type to match the sync flags and return its id.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     sync_flags       - (IN) Sync flags of the allocated entry. _BCM_DPP_AM_SYNC_LIF_INGRESS, _BCM_DPP_AM_SYNC_LIF_EGRESS or both for sync entry. 
 *     bank_id          - (OUT)Found bank_id.
 *     is_success       - (OUT)Will be FALSE if no free bank was found.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_allocate_new_bank(int unit, uint32 sync_flags, int *bank_id, uint8 *is_success) {
    uint32 rv;
    bcm_dpp_am_global_lif_bank_type_t egress_bank_type, ingress_bank_type;
    int bank_id_iter;
    uint8 is_ingress, is_egress;
    uint8 is_ingress_legal, is_egress_legal;
    BCMDNX_INIT_FUNC_DEFS;

    is_ingress = sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS;
    is_egress = sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS;

    if (!is_ingress && !is_egress) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Given sync flags must be either ingress or egress.")));
    }

    /* If we finished iterating and didn't change this variable, it means the allocation failed. */
    *is_success = FALSE;

    /* Set both bank types to none, so in case of non sync entry, they don't interrupt allocating the other bank. */
    egress_bank_type = bcm_dpp_am_global_lif_bank_type_none;
    ingress_bank_type = bcm_dpp_am_global_lif_bank_type_none;

    /* Iterate over the banks from highest to lowest. We use the high banks first because there can be no direct lifs in
       these ranges, so there won't be any constraints. */
    for (bank_id_iter = _BCM_DPP_AM_NOF_GLOBAL_LIF_BANKS(unit) - 1 ; bank_id_iter >= 0 ; bank_id_iter--) {
        if (is_ingress) {
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_ingress_bank_types.get(unit, bank_id_iter, &ingress_bank_type);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (is_egress) {
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_egress_bank_types.get(unit, bank_id_iter, &egress_bank_type);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Bank is free, see if it can be allocated. */
        if (ingress_bank_type == bcm_dpp_am_global_lif_bank_type_none
            && egress_bank_type == bcm_dpp_am_global_lif_bank_type_none) {
            /* Don't allocate banks with constraints unless user indicates WITH_ID. */
            rv = _bcm_dpp_am_global_lif_bank_is_legal(unit, bank_id_iter, FALSE, &is_egress_legal);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_global_lif_bank_is_legal(unit, bank_id_iter, TRUE, &is_ingress_legal);
            BCMDNX_IF_ERR_EXIT(rv);
            
            if (is_ingress_legal && is_egress_legal) {
                /* We can use this bank, break. */
                *is_success = TRUE;
                break;
            }
        }
    }


    /* If we found a valid bank, update its type. */
    if (*is_success) {
        rv = _bcm_dpp_am_global_lif_update_bank_and_type(unit, sync_flags, bank_id_iter);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Update bank id. */
        *bank_id = bank_id_iter;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_valid_banks_get
 * Purpose:
 *      To be called when trying to allocate an entry without the WITH_ID flag.
 *      Will return all allocated banks matching the entry's type.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     sync_flags       - (IN) Sync flags of the allocated entry. _BCM_DPP_AM_SYNC_LIF_INGRESS, _BCM_DPP_AM_SYNC_LIF_EGRESS or both for sync entry. 
 *     nof_valid_banks  - (OUT)Number of valid banks returned.
 *     banks_ids        - (OUT)An array of the existing banks.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_valid_banks_get(int unit, uint32 sync_flags, int *nof_valid_banks, int banks_ids[]) {
    uint32 rv;
    int bank_id_iter;
    uint8 is_ingress, is_egress, is_legal;
    BCMDNX_INIT_FUNC_DEFS;

    is_ingress = sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS;
    is_egress = sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS;

    if (!is_ingress && !is_egress) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Given sync flags must be either ingress or egress.")));
    }

    *nof_valid_banks = 0;

    /* Iterate over the banks from highest to lowest.
       For each bank, check if the bank can be allocated. If so, add it to the bank list.
       We use the high banks first because we prefer to use the direct mapped banks last. */
    for (bank_id_iter = _BCM_DPP_AM_NOF_GLOBAL_LIF_BANKS(unit) - 1 ; bank_id_iter >= 0 ; bank_id_iter--) {

        /* Check legality of the bank. */
        rv = _bcm_dpp_am_global_lif_is_legal_bank(unit, sync_flags, _BCM_DPP_AM_GLOBAL_LIF_BANK_IS_LEGAL_ALLOCATED_BANK_ONLY, bank_id_iter, &is_legal);
        BCMDNX_IF_ERR_EXIT(rv);

        /* If the bank fits, add it to the array. */
        if (is_legal) {
            banks_ids[(*nof_valid_banks)++] = bank_id_iter;
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_global_lif_bank_type_set
 * Purpose:
 *      Set ingress/ egress bank type in the SW DB.
 *        
 *      Invalid bank type is set at device bringup and can't be dynamically changed. An attempt to change an invalid
 *      bank will return an error.
 *  
 *      Other than that, the function doesn't check the logical legality of the new bank type, only sets it.
 *      
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     bank_id          - (IN) The bank id to set.
 *     is_ingress       - (IN) Wether to get the ingress bank (FALSE means get the egress bank).
 *     new_bank_type    - (IN) The bank's new type.
 * Returns: 
 *     BCM_E_NONE       - If the bank type was set succesfuly.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_bank_type_set(int unit, int bank_id, uint8 is_ingress, bcm_dpp_am_global_lif_bank_type_t new_bank_type){
    uint32 rv;
    bcm_dpp_am_global_lif_bank_type_t old_bank_type;
    BCMDNX_INIT_FUNC_DEFS;

    if (is_ingress) {
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_ingress_bank_types.get(unit, bank_id, &old_bank_type);
    } else {
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_egress_bank_types.get(unit, bank_id, &old_bank_type);
    }
    BCMDNX_IF_ERR_EXIT(rv);

    /* Invalid bank type can't be changed during the device's run. */
    if (old_bank_type == bcm_dpp_am_global_lif_bank_type_invalid) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Attempting to set an illegal bank type.")));
    }

    if (old_bank_type == new_bank_type) {
        /* Nothing to do here. */
        BCM_EXIT;
    }

    if (is_ingress) {
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_ingress_bank_types.set(unit, bank_id, new_bank_type);
    } else {
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_egress_bank_types.set(unit, bank_id, new_bank_type);
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_global_lif_res_alloc
 * Purpose:
 *      Attemps to allocate an entry in the given bank_id. 
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     alloc_flags      - (IN) SHR_RES_* flags.
 *     sync_flags       - (IN) Sync flags of the allocated entry. _BCM_DPP_AM_SYNC_LIF_INGRESS, _BCM_DPP_AM_SYNC_LIF_EGRESS or both for sync entry. 
 *     bank_id          - (IN) The bank in which the entry should be allocated.
 *     global_lif       - (INOUT)If WITH_ID flag is given - should have the required global lif id. Otherwise, will be filled with the allocated id.
 * Returns: 
 *     BCM_E_NONE       - If the allocation was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_res_alloc(int unit, uint32 alloc_flags, uint32 sync_flags, int bank_id, int *global_lif){
    uint32 rv;
    uint8 is_ingress, is_egress;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(global_lif);

    is_egress = sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS;
    is_ingress = sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS;

    /* Allocate from egress bank. */
    if (is_egress) {
        rv = dpp_am_res_alloc(unit, bank_id, dpp_am_res_global_outlif, alloc_flags, 1, global_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        /* If ingress allocation is needed, allocate the same id. */
        if (is_ingress) {
            alloc_flags |= SHR_RES_ALLOC_WITH_ID;
        }
    }

    /* Allocate from ingress bank. */
    if (is_ingress) {
        rv = dpp_am_res_alloc(unit, bank_id, dpp_am_res_global_inlif, alloc_flags, 1, global_lif);
        
        /* If sync bank and allocation failed, undo egress allocation. */
        if (rv != BCM_E_NONE && is_egress) {
            int rc;
            LOG_WARN(BSL_LS_BCM_COMMON, (BSL_META_U(unit, "unit %d, global lif sync bank #%d is out of sync\n"), unit, bank_id));
            rc = dpp_am_res_free(unit, bank_id, dpp_am_res_global_outlif, 1, *global_lif);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_internal_alloc_by_type
 * Purpose:
 *      Allocate entries with sync flags and without the WITH_ID flag.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     alloc_flags      - (IN) SHR_RES_* flags.
 *     sync_flags       - (IN) Sync flags of the allocated entry. _BCM_DPP_AM_SYNC_LIF_INGRESS, _BCM_DPP_AM_SYNC_LIF_EGRESS or both for sync entry. 
 *     global_lif       - (OUT)If WITH_ID flag is given - should have the required global lif id. Otherwise, will be filled with the allocated id.
 *     alloc_success    - (OUT)Whether the entry was allocated succesfully or not.
 * Returns: 
 *     BCM_E_NONE       - If the allocation was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_internal_alloc_by_type(int unit, uint32 alloc_flags, uint32 sync_flags, int *global_lif, uint8 *alloc_success) {
    uint32 rv;
    int banks_ids[_BCM_DPP_AM_MAX_GLOBAL_LIF_BANKS];
    int nof_valid_banks, bank_arr_indx;
    BCMDNX_INIT_FUNC_DEFS;


    *alloc_success = FALSE;

    rv = _bcm_dpp_am_global_lif_valid_banks_get(unit, sync_flags, &nof_valid_banks, banks_ids);
    BCMDNX_IF_ERR_EXIT(rv);

    for (bank_arr_indx = 0; bank_arr_indx < nof_valid_banks; ++bank_arr_indx) {
        rv = _bcm_dpp_am_global_lif_res_alloc(unit, alloc_flags, sync_flags, banks_ids[bank_arr_indx], global_lif);
        if (rv == BCM_E_NONE) {
            *alloc_success = TRUE;
            /* Allocation succeded no need to search in other banks */
            break;
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 * GLOBAL LIF BANK MANAGEMENT FUNCTIONS - END
 * GLOBAL LIF COUNTER FUNCTIONS - START
 */
    
/*
 * Function:
 *     _bcm_dpp_am_global_lif_ingress_alloc_verify
 *     _bcm_dpp_am_global_lif_egress_alloc_verify
 * Purpose:
 *      Checks if the maximum number of global lifs (ingress or egress) was not exceeded in the device.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 * Returns: 
 *     BCM_E_NONE       - If the allocation was succesful.
 *     BCM_E_RESOURCE   - If no more global lifs can be allocated. 
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32 
_bcm_dpp_am_global_lif_ingress_alloc_verify(int unit){
    uint32 rv;
    int count;
    BCMDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_ingress_count.get(unit, &count);
    BCMDNX_IF_ERR_EXIT(rv);

    if (count >= SOC_DPP_DEFS_GET(unit, nof_local_lifs)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("No more ingress global lifs available.")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* See description above. */
STATIC uint32 
_bcm_dpp_am_global_lif_egress_alloc_verify(int unit){
    bcm_error_t rv;
    int count;
    BCMDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_egress_count.get(unit, &count);
    BCMDNX_IF_ERR_EXIT(rv);

    if (count >= SOC_DPP_DEFS_GET(unit, nof_glem_lines)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("No more egress global lifs available.")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_global_lif_update_ingress_count
 *     _bcm_dpp_am_global_lif_update_egress_count
 *     _bcm_dpp_am_global_lif_update_count
 * Purpose:
 *      Updates the global ingress/egress lif counter. 
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     diff             - (IN) The number of lifs allocated / deallocated. 
 * Returns: 
 *     BCM_E_NONE       - If the count was updated successfully.
 *     BCM_E_INTERNAL   - If the new count is less than 0.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_global_lif_update_ingress_count(int unit, int diff){
    uint32 rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_global_lif_update_count(unit,
                                             sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_ingress_count.get,
                                             sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_ingress_count.set,
                                             diff);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_global_lif_update_egress_count(int unit, int diff){
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_global_lif_update_count(unit,
                                             sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_egress_count.get,
                                             sw_state_access[unit].dpp.bcm.alloc_mngr_glif.global_lif_egress_count.set,
                                             diff);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_global_lif_update_count(int unit, int (*get_func)(int, int*), int (*set_func)(int, int), int diff) {
    uint32 rv;
    int count;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get old count. */
    rv = get_func(unit, &count);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Calculate new count. */
    count += diff;

    /* If new count is less than 0, return error. */
    if (count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("New global lif count is less than 0.")));
    }

    rv = set_func(unit, count);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * GLOBAL LIF COUNTER FUNCTIONS - END 
 * GLOBAL LIF SW DB - END 
 */

#endif /* BCM_ARAD_SUPPORT */

