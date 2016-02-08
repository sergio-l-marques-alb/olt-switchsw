/*
 * $Id: triumph_mpls.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:    mpls.c
 * Purpose: Manages MPLS functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/triumph.h>

#include <bcm/error.h>
#include <bcm/mpls.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/virtual.h>
#ifdef BCM_TRIUMPH2_SUPPORT
#include <bcm_int/esw/triumph2.h>
#endif

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>



/* 
 * L3_IIF table is used to get the VRF & classid and is 
 * carved out as follows:
 *      0 - 4095 : 4K entries indexed by VLAN for non-tunnel/non-mpls packets
 *   4096 - 4607 : 512 entries directly indexed from L3_TUNNEL
 *   4608 - 6655 : 2K entries for MPLS pop & route packet (one for each VRF)
 *   6656 - 8191 : unused
 */
#define _BCM_TR_MPLS_L3_IIF_BASE        4608

/*
 * Index 63 of the ING_PRI_CNG_MAP table is a special value
 * in HW indicating no to trust the packet's 802.1p pri/cfi.
 * Here, we use index 62 as an identity mapping
 * (pkt_pri => int_pri) and (pkt_cfi => int_color)
 */
#define _BCM_TR_MPLS_PRI_CNG_MAP_IDENTITY   62
#define _BCM_TR_MPLS_PRI_CNG_MAP_NONE       63
#define _BCM_TR_MPLS_HASH_ELEMENTS 1024

/*
 * The SVP of an mpls port can be assigned by
 * VLAN_XLATE, MPLS_ENTRY or SOURCE_TRUNK_MAP_TABLE.
 * This structure is used to store the type of table 
 * and index into the table for quick retrieval.
 */
typedef struct _bcm_tr_match_index_s {
#define _BCM_TR_MATCH_TYPE_NONE         0x00
#define _BCM_TR_MATCH_TYPE_VLAN_XLATE   0x01
#define _BCM_TR_MATCH_TYPE_LABEL        0x02
#define _BCM_TR_MATCH_TYPE_PORT         0x04
#define _BCM_TR_MATCH_TYPE_TRUNK       0x08
    uint32     type:4;
    uint32     index:28;
    uint32     trunk_id;
} _bcm_tr_match_index_t;

/*
 * Software book keeping for MPLS related information
 */
typedef struct _bcm_tr_mpls_bookkeeping_s {
    int         initialized;        /* Set to TRUE when MPLS module initialized */
    SHR_BITDCL  *vrf_bitmap;        /* VRF bitmap */
    SHR_BITDCL  *vpws_bitmap;       /* VPWS usage bitmap (VP pairs) */
    SHR_BITDCL  *vc_c_bitmap;       /* VC_AND_LABEL index bitmap (1st 4K)*/
    SHR_BITDCL  *vc_nc_bitmap;      /* VC_AND_LABEL index bitmap (2nd 4K)*/
    SHR_BITDCL  *pw_term_bitmap;    /* ING_PW_TERM_COUNTERS index bitmap */
    SHR_BITDCL  *pw_init_bitmap;    /* ING_PW_TERM_COUNTERS index bitmap */
    SHR_BITDCL  *tnl_bitmap;        /* EGR_IP_TUNNEL_MPLS index bitmap */
    SHR_BITDCL  *ip_tnl_bitmap;     /* EGR_IP_TUNNEL index bitmap */
    SHR_BITDCL  *exp_map_bitmap;    /* EXP map usage bitmap */
    SHR_BITDCL  *ing_exp_map_bitmap;/* Ingress EXP map usage bitmap */
    sal_mutex_t    mpls_mutex;			  /* Protection mutex. */
    _bcm_tr_match_index_t *match_index; 
} _bcm_tr_mpls_bookkeeping_t;

STATIC _bcm_tr_mpls_bookkeeping_t  _bcm_tr_mpls_bk_info[BCM_MAX_NUM_UNITS] = {{ 0 }};
STATIC _bcm_mpls_egr_nhopList_t  *_bcm_tr_mpls_nhop_headPtr[_BCM_TR_MPLS_HASH_ELEMENTS];

#define MPLS_INFO(_unit_)   (&_bcm_tr_mpls_bk_info[_unit_])
#define L3_INFO(_unit_)   (&_bcm_l3_bk_info[_unit_])
#define MATCH_INDEX(_unit_, _vp_)  \
        (_bcm_tr_mpls_bk_info[_unit_].match_index[_vp_].index)
#define MATCH_TYPE(_unit_, _vp_)  \
        (_bcm_tr_mpls_bk_info[_unit_].match_index[_vp_].type)
#define MATCH_TRUNK(_unit_, _vp_)  \
        (_bcm_tr_mpls_bk_info[_unit_].match_index[_vp_].trunk_id)

/*
 * VRF table usage bitmap operations
 */
#define _BCM_MPLS_VRF_USED_GET(_u_, _vrf_) \
        SHR_BITGET(MPLS_INFO(_u_)->vrf_bitmap, (_vrf_))
#define _BCM_MPLS_VRF_USED_SET(_u_, _vrf_) \
        SHR_BITSET(MPLS_INFO((_u_))->vrf_bitmap, (_vrf_))
#define _BCM_MPLS_VRF_USED_CLR(_u_, _vrf_) \
        SHR_BITCLR(MPLS_INFO((_u_))->vrf_bitmap, (_vrf_))

/*
 * VPWS usage bitmap operations (num VPs / 2)
 */
#define _BCM_MPLS_VPWS_USED_GET(_u_, _i_) \
        SHR_BITGET(MPLS_INFO(_u_)->vpws_bitmap, (_i_))
#define _BCM_MPLS_VPWS_USED_SET(_u_, _i_) \
        SHR_BITSET(MPLS_INFO((_u_))->vpws_bitmap, (_i_))
#define _BCM_MPLS_VPWS_USED_CLR(_u_, _i_) \
        SHR_BITCLR(MPLS_INFO((_u_))->vpws_bitmap, (_i_))

/*
 * VC_AND_LABEL (1st 4K - pw_init counters) usage bitmap operations
 */
#define _BCM_MPLS_VC_COUNT_USED_GET(_u_, _vc_) \
        SHR_BITGET(MPLS_INFO(_u_)->vc_c_bitmap, (_vc_))
#define _BCM_MPLS_VC_COUNT_USED_SET(_u_, _vc_) \
        SHR_BITSET(MPLS_INFO((_u_))->vc_c_bitmap, (_vc_))
#define _BCM_MPLS_VC_COUNT_USED_CLR(_u_, _vc_) \
        SHR_BITCLR(MPLS_INFO((_u_))->vc_c_bitmap, (_vc_))

/*
 * VC_AND_LABEL (2nd 4K - no counters) usage bitmap operations
 */
#define _BCM_MPLS_VC_NON_COUNT_USED_GET(_u_, _vc_) \
        SHR_BITGET(MPLS_INFO(_u_)->vc_nc_bitmap, (_vc_))
#define _BCM_MPLS_VC_NON_COUNT_USED_SET(_u_, _vc_) \
        SHR_BITSET(MPLS_INFO((_u_))->vc_nc_bitmap, (_vc_))
#define _BCM_MPLS_VC_NON_COUNT_USED_CLR(_u_, _vc_) \
        SHR_BITCLR(MPLS_INFO((_u_))->vc_nc_bitmap, (_vc_))

/*
 * "Pseudo-wire termination counters" usage bitmap operations
 */
#define _BCM_MPLS_PW_TERM_USED_GET(_u_, _pt_) \
        SHR_BITGET(MPLS_INFO(_u_)->pw_term_bitmap, (_pt_))
#define _BCM_MPLS_PW_TERM_USED_SET(_u_, _pt_) \
        SHR_BITSET(MPLS_INFO((_u_))->pw_term_bitmap, (_pt_))
#define _BCM_MPLS_PW_TERM_USED_CLR(_u_, _pt_) \
        SHR_BITCLR(MPLS_INFO((_u_))->pw_term_bitmap, (_pt_))

/*
 * "Pseudo-wire Initiation counters" usage bitmap operations
 */
#define _BCM_MPLS_PW_INIT_USED_GET(_u_, _pt_) \
        SHR_BITGET(MPLS_INFO(_u_)->pw_init_bitmap, (_pt_))
#define _BCM_MPLS_PW_INIT_USED_SET(_u_, _pt_) \
        SHR_BITSET(MPLS_INFO((_u_))->pw_init_bitmap, (_pt_))
#define _BCM_MPLS_PW_INIT_USED_CLR(_u_, _pt_) \
        SHR_BITCLR(MPLS_INFO((_u_))->pw_init_bitmap, (_pt_))

/*
 * EGR_IP_TUNNEL_MPLS table usage bitmap operations
 * (there are 4 MPLS entries for each EGR_IP_TUNNEL_MPLS entry)
 */
#define _BCM_MPLS_TNL_USED_GET(_u_, _tnl_) \
        SHR_BITGET(MPLS_INFO(_u_)->tnl_bitmap, (_tnl_))
#define _BCM_MPLS_TNL_USED_SET(_u_, _tnl_) \
        SHR_BITSET(MPLS_INFO((_u_))->tnl_bitmap, (_tnl_))
#define _BCM_MPLS_TNL_USED_CLR(_u_, _tnl_) \
        SHR_BITCLR(MPLS_INFO((_u_))->tnl_bitmap, (_tnl_))

/*
 * EGR_IP_TUNNEL_IP table usage bitmap operations
 */
#define _BCM_MPLS_IP_TNL_USED_GET(_u_, _tnl_) \
        SHR_BITGET(MPLS_INFO(_u_)->ip_tnl_bitmap, (_tnl_))
#define _BCM_MPLS_IP_TNL_USED_SET(_u_, _tnl_) \
        SHR_BITSET(MPLS_INFO((_u_))->ip_tnl_bitmap, (_tnl_))
#define _BCM_MPLS_IP_TNL_USED_CLR(_u_, _tnl_) \
        SHR_BITCLR(MPLS_INFO((_u_))->ip_tnl_bitmap, (_tnl_))

/*
 * EXP map usage bitmap operations
 */
#define _BCM_MPLS_EXP_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(MPLS_INFO(_u_)->exp_map_bitmap, (_map_))
#define _BCM_MPLS_EXP_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(MPLS_INFO((_u_))->exp_map_bitmap, (_map_))
#define _BCM_MPLS_EXP_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(MPLS_INFO((_u_))->exp_map_bitmap, (_map_))

/*
 * Ingress EXP map usage bitmap operations
 */
#define _BCM_MPLS_ING_EXP_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(MPLS_INFO(_u_)->ing_exp_map_bitmap, (_map_))
#define _BCM_MPLS_ING_EXP_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(MPLS_INFO((_u_))->ing_exp_map_bitmap, (_map_))
#define _BCM_MPLS_ING_EXP_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(MPLS_INFO((_u_))->ing_exp_map_bitmap, (_map_))

#ifdef  BCM_TRIUMPH2_SUPPORT
#define _BCM_MPLS_FAILOVER_VALID_RANGE(_a_) \
	if ( ( (_a_) > 0)  &&  ( (_a_) < 1024) )
#endif

#define _BCM_MPLS_CLEANUP(_rv_) \
       if ( (_rv_) < 0) { \
	    goto cleanup; \
       }

#define _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_MASK    0x100
#define _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS 0x100
#define _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_EGRESS  0x000
#define _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK     0x0ff

STATIC void  _bcm_tr_mpls_cleanup_egrNhop (int unit);

/*
 * Function:
 *      bcm_mpls_enable
 * Purpose:
 *      Enable/disable MPLS function.
 * Parameters:
 *      unit - SOC unit number.
 *      enable - TRUE: enable MPLS support; FALSE: disable MPLS support.
 * Returns:
 *      BCM_E_XXX.
 */
STATIC int
bcm_tr_mpls_enable(int unit, int enable)
{
    int port, rv = BCM_E_NONE;
    PBMP_PORT_ITER(unit, port) {
        /* No mpls lookup on stacking ports. */
        if (IS_ST_PORT(unit, port)) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlMpls, 
                                      (enable) ? 1 : 0));
    }
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        soc_mem_t   mem;
        int         idx, idx_min, idx_max, mem_idx_max, buf_size, rv;
        int         tdma_size, vlan_mpls_chunks;
        int         *buffer, *entry;

        mem = VLAN_MPLSm;
        tdma_size = soc_property_get(unit, spn_TABLE_DMA_SIZE, 1024);
        vlan_mpls_chunks = tdma_size / 
                           (soc_mem_entry_words(unit, mem) * sizeof(uint32));
        if (vlan_mpls_chunks <= 0) {
            return BCM_E_PARAM;
        }        

        mem_idx_max = soc_mem_index_max(unit, mem);
        buf_size = soc_mem_entry_words(unit, mem) * sizeof(uint32) * 
                   vlan_mpls_chunks;
        buffer = soc_cm_salloc(unit, buf_size, "vlan_mpls_enable");
        if (buffer == NULL) {
            return BCM_E_MEMORY;
        }
        
        for (idx_min = 0; idx_min < mem_idx_max; idx_min+=vlan_mpls_chunks) {
            idx_max = idx_min + vlan_mpls_chunks - 1;
            if (idx_max > mem_idx_max) {
                idx_max = mem_idx_max;
            }
            sal_memset(buffer, 0, buf_size);
            rv = soc_mem_read_range(unit, mem, SOC_BLOCK_ANY, idx_min, 
                                    idx_max, buffer);
            if (rv != SOC_E_NONE) {
                break;
            }
            for (idx = 0; idx < vlan_mpls_chunks; idx++) {
                entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                     buffer, idx);
                soc_VLAN_MPLSm_field32_set(unit, entry, MPLS_ENABLEf, 1);
            }
            rv = soc_mem_write_range(unit, mem, SOC_BLOCK_ALL, idx_min, 
                                     idx_max, buffer);
            if (rv != SOC_E_NONE) {
                break;
            }
        }
        soc_cm_sfree(unit, buffer);
    }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_tr_mpls_free_resource
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_tr_mpls_free_resource(int unit, _bcm_tr_mpls_bookkeeping_t *mpls_info)
{
    if (!mpls_info) {
        return;
    }

    if (mpls_info->vrf_bitmap) {
        sal_free(mpls_info->vrf_bitmap);
        mpls_info->vrf_bitmap = NULL;
    }

    if (mpls_info->vpws_bitmap) {
        sal_free(mpls_info->vpws_bitmap);
        mpls_info->vpws_bitmap = NULL;
    }

    if (mpls_info->vc_c_bitmap) {
        sal_free(mpls_info->vc_c_bitmap);
        mpls_info->vc_c_bitmap = NULL;
    }

    if (mpls_info->vc_nc_bitmap) {
        sal_free(mpls_info->vc_nc_bitmap);
        mpls_info->vc_nc_bitmap = NULL;
    }

    if (mpls_info->pw_term_bitmap) {
        sal_free(mpls_info->pw_term_bitmap);
        mpls_info->pw_term_bitmap = NULL;
    }

    if (mpls_info->pw_init_bitmap) {
        sal_free(mpls_info->pw_init_bitmap);
        mpls_info->pw_init_bitmap = NULL;
    }

    if (mpls_info->match_index) {
        sal_free(mpls_info->match_index);
        mpls_info->match_index = NULL;
    }

    if (mpls_info->tnl_bitmap) {
        sal_free(mpls_info->tnl_bitmap);
        mpls_info->tnl_bitmap = NULL;
    }

    if (mpls_info->ip_tnl_bitmap) {
        sal_free(mpls_info->ip_tnl_bitmap);
        mpls_info->ip_tnl_bitmap = NULL;
    }

    if (mpls_info->exp_map_bitmap) {
        sal_free(mpls_info->exp_map_bitmap);
        mpls_info->exp_map_bitmap = NULL;
    }

    if (mpls_info->ing_exp_map_bitmap) {
        sal_free(mpls_info->ing_exp_map_bitmap);
        mpls_info->ing_exp_map_bitmap = NULL;
    }

    (void) _bcm_tr_mpls_cleanup_egrNhop(unit);
}

/*
 * Function:
 *      bcm_tr_mpls_check_init
 * Purpose:
 *      Check if MPLS is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int 
_bcm_tr_mpls_check_init (int unit)
{
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
            return BCM_E_UNIT;
        }
		
        if (!_bcm_tr_mpls_bk_info[unit].initialized) { 
            return BCM_E_INIT;
        } else {
             return BCM_E_NONE;
        }
}

/*
 * Function:
 *      bcm_tr_mpls_lock
 * Purpose:
 *      Take the MPLS software module Lock Sempahore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

 int 
 bcm_tr_mpls_lock (int unit)
{
   int rv;

   rv = _bcm_tr_mpls_check_init (unit);
   
   if ( rv == BCM_E_NONE ) {
           sal_mutex_take(MPLS_INFO((unit))->mpls_mutex, sal_mutex_FOREVER);
   }
   return rv; 
}


/*
 * Function:
 *      bcm_tr_mpls_unlock
 * Purpose:
 *      Release  the MPLS software module Lock Semaphore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

void
bcm_tr_mpls_unlock(int unit)
{
	int rv;
	
	rv = _bcm_tr_mpls_check_init (unit);
	
	if ( rv == BCM_E_NONE ) {
		sal_mutex_give(MPLS_INFO((unit))->mpls_mutex);
	}
}

/*
 * Function:
 *      bcm_tr_mpls_stat_clear
 * Purpose:
 *      Clear L2 MPLS PW Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 * Returns:
 *      BCM_E_XXX
 */     

STATIC int
_bcm_tr_mpls_stat_clear(int unit)
{
    int num_pw_term, num_pw_init;
    int index, rv=BCM_E_NONE;
    ing_pw_term_counters_entry_t cent;
    egr_pw_init_counters_entry_t pw_init_entry;

    num_pw_term = 0;
    if (SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {
        num_pw_term = soc_mem_index_count(unit, ING_PW_TERM_COUNTERSm);
    }

    num_pw_init = 0;
    if (SOC_MEM_IS_VALID(unit, EGR_PW_INIT_COUNTERSm)) {
        num_pw_init = soc_mem_index_count(unit, EGR_PW_INIT_COUNTERSm);
    }

    /* Clear the ING Counters */
   sal_memset(&cent, 0, sizeof(ing_pw_term_counters_entry_t));
   for (index=0; index<num_pw_term; index++) {
		  rv = WRITE_ING_PW_TERM_COUNTERSm(unit, MEM_BLOCK_ANY, index, &cent);
		  if (rv < 0) {
			   return rv;
		  }
   }

   /* Clear the EGR Counters */
   sal_memset(&pw_init_entry, 0, sizeof(egr_pw_init_counters_entry_t));
   for (index=0; index<num_pw_init; index++) {
		rv = WRITE_EGR_PW_INIT_COUNTERSm(unit, MEM_BLOCK_ANY, index, &pw_init_entry);
		if (rv < 0) {
			 return rv;
		}
   }
   return rv;
}

/*
 * Function:
 *      bcm_mpls_init
 * Purpose:
 *      Initialize the MPLS software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_init(int unit)
{
    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int rv=0, i, num_vfi, num_vp, num_vc, num_vrf;
    int num_pw_term, num_pw_init, num_ip_tnl, num_exp_map, num_ing_exp_map, pkt_cfi, pkt_pri;
    ing_pri_cng_map_entry_t pri_map;
    vfi_entry_t vfi_entry;
    _bcm_l3_ingress_intf_t iif;
    uint32 label;

    if (!L3_INFO(unit)->l3_initialized) {
        soc_cm_debug(DK_ERR, "L3 module must be initialized prior to MPLS_init\n");
	return BCM_E_CONFIG;
    }

    

    num_vfi = soc_mem_index_count(unit, VFIm);
    num_vrf = SOC_CONTROL(unit)->max_vrf_id + 1;
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
    num_ip_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);
    /* EGR_MPLS_EXP_MAPPING_1 profiles indexed by ((int_pri << 2) + cng) */
    num_exp_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) >> 6;
    num_ing_exp_map = soc_mem_index_count(unit, ING_MPLS_EXP_MAPPINGm) / 8;
    num_pw_term = 0;
    if (SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {
        num_pw_term = soc_mem_index_count(unit, ING_PW_TERM_COUNTERSm);
    }
    num_pw_init = 0;
    if (SOC_MEM_IS_VALID(unit, EGR_PW_INIT_COUNTERSm)) {
        num_pw_init = soc_mem_index_count(unit, EGR_PW_INIT_COUNTERSm);
    }

    /*
     * allocate resources
     */
    if (mpls_info->initialized) {
        rv = bcm_tr_mpls_cleanup(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    mpls_info->vrf_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_vrf), "vrf_bitmap");
    if (mpls_info->vrf_bitmap == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return BCM_E_MEMORY;
    }

    mpls_info->vpws_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_vp / 2), "vpws_bitmap");
    if (mpls_info->vpws_bitmap == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return BCM_E_MEMORY;
    }

    mpls_info->match_index =
        sal_alloc(sizeof(_bcm_tr_match_index_t) * num_vp, "match_index");
    if (mpls_info->match_index == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return BCM_E_MEMORY;
    }

    mpls_info->vc_c_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_vc), "vc_c_bitmap"); /* (num_vc / 2) TODO:Review */
    if (mpls_info->vc_c_bitmap == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return BCM_E_MEMORY;
    }

    mpls_info->vc_nc_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_vc), "vc_nc_bitmap"); /* (num_vc / 2) TODO:Review */
    if (mpls_info->vc_nc_bitmap == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return BCM_E_MEMORY;
    }

    mpls_info->pw_term_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_pw_term), "pw_term_bitmap");
    if (mpls_info->pw_term_bitmap == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return BCM_E_MEMORY;
    }

    mpls_info->pw_init_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_pw_init), "pw_init_bitmap");
    if (mpls_info->pw_init_bitmap == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return BCM_E_MEMORY;
    }

    mpls_info->exp_map_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_exp_map), "exp_map_bitmap");
    if (mpls_info->exp_map_bitmap == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return BCM_E_MEMORY;
    }

    mpls_info->ing_exp_map_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_ing_exp_map), "ing_exp_map_bitmap");
    if (mpls_info->ing_exp_map_bitmap == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return BCM_E_MEMORY;
    }

    /* 4 MPLS entries for each EGR_IP_TUNNEL_MPLS entry */
    mpls_info->tnl_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_ip_tnl * 4), "tnl_bitmap");
    if (mpls_info->tnl_bitmap == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
          return BCM_E_MEMORY;
    }

    mpls_info->ip_tnl_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_ip_tnl), "ip_tnl_bitmap");
    if (mpls_info->ip_tnl_bitmap == NULL) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
	   return BCM_E_MEMORY;
    }

    sal_memset(mpls_info->vrf_bitmap, 0, SHR_BITALLOCSIZE(num_vrf));
    sal_memset(mpls_info->vpws_bitmap, 0, SHR_BITALLOCSIZE(num_vp / 2));
    sal_memset(mpls_info->vc_c_bitmap, 0, SHR_BITALLOCSIZE(num_vc / 2));
    sal_memset(mpls_info->vc_nc_bitmap, 0, SHR_BITALLOCSIZE(num_vc / 2));
    sal_memset(mpls_info->pw_term_bitmap, 0, SHR_BITALLOCSIZE(num_pw_term));
    sal_memset(mpls_info->pw_init_bitmap, 0, SHR_BITALLOCSIZE(num_pw_init));	
    sal_memset(mpls_info->exp_map_bitmap, 0, SHR_BITALLOCSIZE(num_exp_map));
    sal_memset(mpls_info->ing_exp_map_bitmap, 0, SHR_BITALLOCSIZE(num_ing_exp_map));
    sal_memset(mpls_info->match_index, 0, sizeof(_bcm_tr_match_index_t) * num_vp);
    sal_memset(mpls_info->tnl_bitmap, 0, SHR_BITALLOCSIZE(num_ip_tnl * 4));
    sal_memset(mpls_info->ip_tnl_bitmap, 0, SHR_BITALLOCSIZE(num_ip_tnl));

    /*
     * Initialize the VFI table by setting L3MC_INDEX to 0 and
     * PFM to 3. The PFM==3 (invalid) indicates that the VFI entry is not used.
     * This can be used to recover SW state during warm reboot.
     */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
          sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));

          soc_VFIm_field32_set(unit, &vfi_entry, BC_INDEXf, 0);
          soc_VFIm_field32_set(unit, &vfi_entry, UUC_INDEXf, 0);
          soc_VFIm_field32_set(unit, &vfi_entry, UMC_INDEXf, 0);
          for (i = 0; i < num_vfi; i++) {
              rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, i, &vfi_entry);
              if (rv < 0) {
                  _bcm_tr_mpls_free_resource(unit, mpls_info);
                  return rv;
              }
          }
    } else
#endif /* BCM_TRIUMPH2_SUPPORT */
    {
        if (soc_mem_field_valid(unit, VFIm, PFMf)) { 
           sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
        
           soc_VFIm_field32_set(unit, &vfi_entry, L3MC_INDEXf, 0);
           soc_VFIm_field32_set(unit, &vfi_entry, PFMf, 3);
           for (i = 0; i < num_vfi; i++) {
                  rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, i, &vfi_entry);
                  if (rv < 0) {
                        _bcm_tr_mpls_free_resource(unit, mpls_info);
                        return rv;
                  }
           }
        }
    }
    /* 
     * Initialize L3_IIF entries starting from 
     * _BCM_TR_MPLS_L3_IIF_BASE for each valid VRF value.
     */
    sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
    for (i = 0; i < num_vrf; i++) {
        iif.intf_id = i + _BCM_TR_MPLS_L3_IIF_BASE;
        iif.vrf = i;
        rv = _bcm_tr_l3_ingress_interface_set(unit, &iif);
        if (rv < 0) {
            _bcm_tr_mpls_free_resource(unit, mpls_info);
            return rv;
        }
    }

    /* 
     * Reserve one mapping instance in ING_PRI_CNG_MAP for
     * identity mapping of packet pri/cfi to internal pri/color.
     */
    sal_memset(&pri_map, 0, sizeof(ing_pri_cng_map_entry_t));
    for (pkt_cfi = 0; pkt_cfi <= 1; pkt_cfi++) {
        for (pkt_pri = 0; pkt_pri <= 7; pkt_pri++) {
            /* ING_PRI_CNG_MAP table is indexed with
             * port[0:4] incoming priority[2:0] incoming CFI[0]
             */
            i = (_BCM_TR_MPLS_PRI_CNG_MAP_IDENTITY << 4) | (pkt_pri << 1) | pkt_cfi;
            soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map, PRIf,
                                pkt_pri);
            soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map, CNGf,
                                pkt_cfi);
            rv = WRITE_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ALL, i, &pri_map);
            if (rv < 0) {
                _bcm_tr_mpls_free_resource(unit, mpls_info);
                return rv;
            }
        }
    }

   /* Clear L2 PW Counters */
    rv = _bcm_tr_mpls_stat_clear(unit);
    if (rv<0) {
         return rv;
    }

    /* Initialize the entire label space "Port Independent"
      * Use Switch Controls to re-configure the Label Space 
      * for Port-Dependant and Port-Independant 
      */
 
    label = 0;
    rv = soc_reg_field32_modify(unit, GLOBAL_MPLS_RANGE_1_LOWERr,
                                REG_PORT_ANY, LABELf, label);
    if (rv < 0) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return rv;
    }
    rv = soc_reg_field32_modify(unit, GLOBAL_MPLS_RANGE_2_LOWERr,
                                REG_PORT_ANY, LABELf, label);
    if (rv < 0) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return rv;
    }

    label = (1 << 20) - 1;
    rv = soc_reg_field32_modify(unit, GLOBAL_MPLS_RANGE_1_UPPERr,
                                REG_PORT_ANY, LABELf, label);
    if (rv < 0) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return rv;
    }
    rv = soc_reg_field32_modify(unit, GLOBAL_MPLS_RANGE_2_UPPERr,
                                REG_PORT_ANY, LABELf, label);
    if (rv < 0) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return rv;
    }

    /* Create MPLS protection mutex. */
    MPLS_INFO((unit))->mpls_mutex = sal_mutex_create("mpls_mutex");
    if (!MPLS_INFO((unit))->mpls_mutex) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return rv;
    }
    /* Enable MPLS */
    rv = bcm_tr_mpls_enable(unit, TRUE);
    if (rv < 0) {
        _bcm_tr_mpls_free_resource(unit, mpls_info);
        return rv;
    }

    for(i=0; i < _BCM_TR_MPLS_HASH_ELEMENTS; ++i) {
         _bcm_tr_mpls_nhop_headPtr[i] = NULL;
    }

    /* Mark the state as initialized */
    mpls_info->initialized = TRUE;
    return rv;
}

/*
 * Function:
 *      _bcm_tr_mpls_hw_clear
 * Purpose:
 *     Perform hw tables clean up for mpls module. 
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_mpls_hw_clear(int unit)
{
    int rv, rv_error = BCM_E_NONE;

    rv = bcm_tr_mpls_enable(unit, FALSE);
    if (BCM_FAILURE(rv)) {
        rv_error = rv;
    }
	
    /* Clear L2 PW Counters */
    rv = _bcm_tr_mpls_stat_clear(unit);
    if (rv<0) {
       rv_error = rv;
    }

    /* Destroy all VPNs */
    rv = bcm_tr_mpls_vpn_id_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    /* Delete all L2 tunnel entries */
    rv = bcm_trx_metro_l2_tunnel_delete_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    /* Clear all MPLS tunnel initiators */
    rv = bcm_tr_mpls_tunnel_initiator_clear_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    /* Delete all MPLS switch entries */
    rv = bcm_tr_mpls_tunnel_switch_delete_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    /* Destroy all EXP maps */
    rv = bcm_tr_mpls_exp_map_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }

    return rv_error;
}

/*
 * Function:
 *      bcm_mpls_cleanup
 * Purpose:
 *      Detach the MPLS software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_cleanup(int unit)
{
    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int rv;

    if (FALSE == mpls_info->initialized) {
        return (BCM_E_NONE);
    } 

    rv = bcm_tr_mpls_lock (unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) { 
        rv = _bcm_tr_mpls_hw_clear(unit);
    }

    /* Mark the state as uninitialized */
    mpls_info->initialized = FALSE;

    /* Free software resources */
    (void) _bcm_tr_mpls_free_resource(unit, mpls_info);

    bcm_tr_mpls_unlock (unit);

    /* Destroy MPLS protection mutex. */
    sal_mutex_destroy(MPLS_INFO((unit))->mpls_mutex );

    return rv;
}

/*
 * Function:
 *      bcm_mpls_vpn_id_create
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      No hardware action
 */
int
bcm_tr_mpls_vpn_id_create(int unit, bcm_mpls_vpn_config_t *info)
{
    int vp, rv = BCM_E_PARAM;


    if (info->flags & BCM_MPLS_VPN_VPWS) {
        int num_vp;

        /*
         * For VPWS, allocate two VPs for each side of the 
         * point-to-point connection.
         */
        if (info->flags & BCM_MPLS_VPN_WITH_ID) {
            vp = _BCM_MPLS_VPN_ID_GET(info->vpn);
            if (_BCM_MPLS_VPWS_USED_GET(unit, vp / 2)) {
                /* Already used */
                return BCM_E_EXISTS;
            }
            /* Mark VP's as Used */
           BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, _bcmVpTypeMpls));
           BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, (vp+1), _bcmVpTypeMpls));
        } else {
            /* Allocate 2 consecutive VPs */
            num_vp = soc_mem_index_count(unit, SOURCE_VPm);
            rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 2, SOURCE_VPm, _bcmVpTypeMpls, &vp);
            BCM_IF_ERROR_RETURN(rv);
        }
        /* Keep track of VP pairs used for VPWS */
        _BCM_MPLS_VPWS_USED_SET(unit, vp / 2);

        /* Set the VPN id value returned to caller */
        _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_VPWS, vp);
        rv = BCM_E_NONE;
    } else if (info->flags & BCM_MPLS_VPN_VPLS) {
        vfi_entry_t vfi_entry;
        int vfi_index, mc_group, mc_group_type;
        bcm_vlan_mcast_flood_t mode;

        /* Check that the broadcast group is valid. Also check that
         * broadcast, unknown_unicast, and unknown_multicast groups
         * are the same (this device only supports on multicast group
         * for all 3 types of multicast traffic.
         */
        mc_group_type = _BCM_MULTICAST_TYPE_GET(info->broadcast_group);
        mc_group = _BCM_MULTICAST_ID_GET(info->broadcast_group);
        if ((mc_group_type != _BCM_MULTICAST_TYPE_VPLS) ||
            (mc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
            (info->broadcast_group != info->unknown_unicast_group) ||
            (info->broadcast_group != info->unknown_multicast_group)) {
            return BCM_E_PARAM;
        }
        /* Allocate a VFI */
        /* Lock the VFI table while allocating and writing to allow
         * proper operation with the service counters */
        soc_mem_lock(unit, VFIm);
        if (info->flags & BCM_MPLS_VPN_WITH_ID) {
            vfi_index = _BCM_MPLS_VPN_ID_GET(info->vpn);
            if (_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeMpls)) {
                soc_mem_unlock(unit, VFIm);
                return BCM_E_EXISTS;
            } else {
               rv = _bcm_vfi_alloc_with_id(unit, VFIm, _bcmVfiTypeMpls, vfi_index);
               if (rv < 0) {
                   soc_mem_unlock(unit, VFIm);
                   return rv;
               }
            }
        } else {
            rv = _bcm_vfi_alloc(unit, VFIm, _bcmVfiTypeMpls, &vfi_index);
            if (rv < 0) {
                soc_mem_unlock(unit, VFIm);
                return rv;
            }
        }

        /* Get the default flood mode */
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_flood_default_get(unit, &mode));
        switch (mode) {
            case BCM_VLAN_MCAST_FLOOD_ALL:
               mode = 0;
               break;
            case BCM_VLAN_MCAST_FLOOD_NONE:
               mode = 2;
               break;
            default:
               mode = 1;
               break;
        }

        /* Commit the entry to HW */
        sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            soc_VFIm_field32_set(unit, &vfi_entry, UMC_INDEXf, mc_group);
            soc_VFIm_field32_set(unit, &vfi_entry, UUC_INDEXf, mc_group);
            soc_VFIm_field32_set(unit, &vfi_entry, BC_INDEXf, mc_group);
        } else
#endif
        {
            soc_VFIm_field32_set(unit, &vfi_entry, L3MC_INDEXf, mc_group);
            soc_VFIm_field32_set(unit, &vfi_entry, PFMf, mode);
        }
        rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry);
        if (rv < 0) {
            (void) _bcm_vfi_free(unit, _bcmVfiTypeMpls, vfi_index);
            soc_mem_unlock(unit, VFIm);
            return rv;
        }
        soc_mem_unlock(unit, VFIm);

        /* Set the returned VPN id */
        _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_VPLS, vfi_index);
    } else if (info->flags & BCM_MPLS_VPN_L3) {
        int vrf;

        /* 
         * The caller manages VRF space. An MPLS L3 VPN identifier
         * is simply the VRF value passed in by the caller (info->lookup_id)
         * added to a base offset. The VPN ID returned here can 
         * be used in the bcm_mpls_tunnel_switch_* APIs. 
         */
        if (info->flags & BCM_MPLS_VPN_WITH_ID) {
            vrf = _BCM_MPLS_VPN_ID_GET(info->vpn);
        } else {
            vrf = info->lookup_id;
        }

        if ((vrf < 0) || (vrf > SOC_VRF_MAX(unit))) {
            return BCM_E_PARAM;
        }
        if (_BCM_MPLS_VRF_USED_GET(unit, vrf)) {
            return BCM_E_EXISTS;
        }
        _BCM_MPLS_VRF_USED_SET(unit, vrf);
        _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_L3, vrf);
        rv = BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr_mpls_vpn_id_get
 * Purpose:
 *      Get VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vpn -  (IN)  VPN instance ID
 *      info  - (OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      No hardware action
 */

int
bcm_tr_mpls_vpn_id_get( int  unit,
                                                  bcm_vpn_t  vpn,
                                                  bcm_mpls_vpn_config_t *info)
{

    if (_BCM_MPLS_VPN_IS_VPWS(vpn)) {
         int vp;

         vp = _BCM_MPLS_VPN_ID_GET(vpn);

         if (!_BCM_MPLS_VPWS_USED_GET(unit, (vp / 2))) {
              return BCM_E_NOT_FOUND;
         }

          /* Set the VPN id value returned to caller */
          _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_VPWS, vp);
         info->flags |=  BCM_MPLS_VPN_VPWS;

    } else if (_BCM_MPLS_VPN_IS_VPLS(vpn)) {
         int vfi_index;
         vfi_entry_t vfi_entry;

         vfi_index = _BCM_MPLS_VPN_ID_GET(vpn);

         if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeMpls)) {
              return BCM_E_NOT_FOUND;
         }

         BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ALL, vfi_index, &vfi_entry));
#ifdef BCM_TRIUMPH2_SUPPORT
         if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
             SOC_IS_VALKYRIE2(unit)) {
              info->unknown_multicast_group = soc_VFIm_field32_get(unit, &vfi_entry, UMC_INDEXf);
              info->unknown_unicast_group = soc_VFIm_field32_get(unit, &vfi_entry, UUC_INDEXf);
              info->broadcast_group = soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf);
         } else
#endif
         {
              info->broadcast_group = soc_VFIm_field32_get(unit, &vfi_entry, L3MC_INDEXf);
              info->unknown_multicast_group = info->broadcast_group;
              info->unknown_unicast_group = info->broadcast_group;
         }
	 
         /* Set the returned VPN id */
         _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_VPLS, vfi_index);
         info->flags |=  BCM_MPLS_VPN_VPLS;

    } else if (_BCM_MPLS_VPN_IS_L3(vpn)) {
        int vrf;

        vrf = _BCM_MPLS_VPN_ID_GET(vpn);
        if ((vrf < 0) || (vrf > SOC_VRF_MAX(unit))) {
            return BCM_E_PARAM;
        }
        if (!_BCM_MPLS_VRF_USED_GET(unit, vrf)) {
            return BCM_E_NOT_FOUND;
        }
        _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_L3, vrf);
        info->flags |=	BCM_MPLS_VPN_L3;

    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_vpn_id_destroy
 * Purpose:
 *      Delete a VPN instance
 * Parameters:
 *      unit - Device Number
 *      vpn - VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_vpn_id_destroy (int unit, bcm_vpn_t vpn)
{
    int rv;


    if (_BCM_MPLS_VPN_IS_VPWS(vpn)) {
        int vp;

        vp = _BCM_MPLS_VPN_ID_GET(vpn);

        if (!_BCM_MPLS_VPWS_USED_GET(unit, (vp / 2))) {
            return BCM_E_NOT_FOUND;
        }

        /* Delete all the mpls ports on this VPN */
        rv = bcm_tr_mpls_port_delete_all(unit, vpn);
        BCM_IF_ERROR_RETURN(rv);

        /* Free the VP values */
        (void) _bcm_vp_free(unit, _bcmVpTypeMpls, 2, vp);

        /* Clear the  VPWS pair in-use status */
        _BCM_MPLS_VPWS_USED_CLR(unit, vp / 2);
    } else if (_BCM_MPLS_VPN_IS_VPLS(vpn)) {
        int vfi;
        vfi_entry_t vfi_entry;

        vfi = _BCM_MPLS_VPN_ID_GET(vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
            return BCM_E_NOT_FOUND;
        }

        /* Delete all the mpls ports on this VPN */
        rv = bcm_tr_mpls_port_delete_all(unit, vpn);
        BCM_IF_ERROR_RETURN(rv);

        /*
         * Clear the VFI table entry by setting L3MC_INDEX to 0 and
         * PFM to 3. The PFM==3 (invalid) indicates that the VFI entry is not used.
         * This can be used to recover SW state during warm reboot.
         */
        sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
             SOC_IS_VALKYRIE2(unit)) {
             soc_VFIm_field32_set(unit, &vfi_entry, BC_INDEXf, 0);
             soc_VFIm_field32_set(unit, &vfi_entry, UUC_INDEXf, 0);
             soc_VFIm_field32_set(unit, &vfi_entry, UMC_INDEXf, 0);
        } else
#endif
        {
  	     if (SOC_MEM_FIELD_VALID(unit, VFIm, PFMf)) {
	         soc_VFIm_field32_set(unit, &vfi_entry, PFMf, 3);
	     }
        }

        BCM_IF_ERROR_RETURN(WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry));
#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_gport_service_counters)) {
            /* Release Service counter, if any */
            _bcm_esw_flex_stat_handle_free(unit, _bcmFlexStatTypeService,
                                           vpn);
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit, EGR_VFIm, vfi,
                                                       SERVICE_CTR_IDXf, 0));
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
        (void) _bcm_vfi_free(unit, _bcmVfiTypeMpls, vfi);
    } else if (_BCM_MPLS_VPN_IS_L3(vpn)) {
        int vrf;

        vrf = _BCM_MPLS_VPN_ID_GET(vpn);
        if ((vrf < 0) || (vrf > SOC_VRF_MAX(unit))) {
            return BCM_E_PARAM;
        }
        if (!_BCM_MPLS_VRF_USED_GET(unit, vrf)) {
            return BCM_E_NOT_FOUND;
        }
        _BCM_MPLS_VRF_USED_CLR(unit, vrf);
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

int
bcm_tr_mpls_vpn_id_destroy_all (int unit)
{
    int num_vpws, num_vfi, num_vrf, i, rv, rv_error = BCM_E_NONE;
    bcm_vpn_t vpn;


    /* Destroy all VPWS VPNs */
    num_vpws = soc_mem_index_count(unit, SOURCE_VPm) / 2;
    for (i = 0; i < num_vpws; i++) {
        if (_BCM_MPLS_VPWS_USED_GET(unit, i)) {
            _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPWS, (i * 2));
            rv = bcm_tr_mpls_vpn_id_destroy(unit, vpn);
            if (rv < 0) {
                rv_error = rv;
            }
        }
    }

    /* Destroy all VPLS VPNs */
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (i = 0; i < num_vfi; i++) {
        if (_bcm_vfi_used_get(unit, i, _bcmVfiTypeMpls)) {
            _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_VPLS, i);
            rv = bcm_tr_mpls_vpn_id_destroy(unit, vpn);
            if (rv < 0) {
                rv_error = rv;
            }
        }
    }

    /* Destroy all L3 VPNs */
    num_vrf = SOC_VRF_MAX(unit) + 1;
    for (i = 0; i < num_vrf; i++) {
        if (_BCM_MPLS_VRF_USED_GET(unit, i)) {
            _BCM_MPLS_VPN_SET(vpn, _BCM_MPLS_VPN_TYPE_L3, i);
            rv = bcm_tr_mpls_vpn_id_destroy(unit, vpn);
            if (rv < 0) {
                rv_error = rv;
            }
        }
    }

    /* Destroy default  VP */
    if (_bcm_vp_used_get(unit, 0, _bcmVpTypeMpls)) {
         _bcm_vp_free(unit, _bcmVpTypeMpls, 1, 0);
    }
	
    return rv_error;
}

STATIC int
_bcm_tr_mpls_egress_intf_find(int unit, int nh_index, bcm_if_t *egress_if)
{
    bcm_l3_egress_t nh_info;
    egr_mac_da_profile_entry_t macda;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_l3_intf_entry_t egr_intf;
    int macda_idx;

    /* Read the HW entries */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &ing_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &egr_nh));

    sal_memset(&nh_info, 0, sizeof(bcm_l3_egress_t));
    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
        nh_info.flags = BCM_L3_TGID;
        nh_info.trunk = 
            soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
    } else {
        nh_info.module = 
            soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
        nh_info.port = 
            soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
    }
    nh_info.intf =
        soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, INTF_NUMf);
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_INTFm, MEM_BLOCK_ANY,
                                      nh_info.intf, &egr_intf));
    nh_info.vlan = soc_EGR_L3_INTFm_field32_get(unit, &egr_intf, VIDf);

    macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 MAC_DA_PROFILE_INDEXf);
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_MAC_DA_PROFILEm, MEM_BLOCK_ANY,
                                      macda_idx, &macda));
    soc_mem_mac_addr_get(unit, EGR_MAC_DA_PROFILEm,
                         &macda, MAC_ADDRESSf, nh_info.mac_addr);
    return bcm_xgs3_l3_egress_find(unit, &nh_info, egress_if);
}

typedef struct _bcm_tr_ing_nh_info_s {
    bcm_port_t      port;
    int      module;
    bcm_trunk_t      trunk;
    uint32   mtu;
} _bcm_tr_ing_nh_info_t;

typedef struct _bcm_tr_egr_nh_info_s {
    uint8    entry_type;
    uint8    dvp_is_network;
    uint8    sd_tag_action_present;
    uint8    sd_tag_action_not_present;
    int      dvp;
    int      intf_num;
    int      sd_tag_vlan;
    int      macda_index;
    int      vc_swap_index;
    int      tpid_index;
    int      pw_init_cnt;
} _bcm_tr_egr_nh_info_t;




STATIC int
_bcm_tr_mpls_INITIAL_ING_L3_NEXT_HOP_add(int unit,  _bcm_tr_ing_nh_info_t  *ing_nh_info, 
                                                                                              int nh_index, bcm_port_t *local_port, int *is_local)
{
    initial_ing_l3_next_hop_entry_t   initial_ing_nh;
    int rv;
    int modid_local=0;

	 sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
	 if (ing_nh_info->trunk == -1) {
		 soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
							 &initial_ing_nh, PORT_NUMf, ing_nh_info->port);
		 soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
							 &initial_ing_nh, MODULE_IDf, ing_nh_info->module);
		 BCM_IF_ERROR_RETURN( _bcm_esw_modid_is_local(unit, ing_nh_info->module, &modid_local));
		 
		 if (modid_local) {
			 /* Indicated to calling function that this is a local port */
			 *is_local = 1;
			 *local_port = ing_nh_info->port;
		 }
	 } else {
		 soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
							 &initial_ing_nh, Tf, 1);
		 soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
							 &initial_ing_nh, TGIDf, ing_nh_info->trunk);
		 *is_local = 1;		 
		 BCM_GPORT_TRUNK_SET(*local_port, ing_nh_info->trunk);
	 }

	 rv = soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
						MEM_BLOCK_ALL, nh_index, &initial_ing_nh);
	 
     return rv;
}


STATIC int
_bcm_tr_mpls_ING_L3_NEXT_HOP_add(int unit,  _bcm_tr_ing_nh_info_t  *ing_nh_info, 
                                                                              int nh_index, int  drop)
{
    ing_l3_next_hop_entry_t ing_nh;
    int rv;

	 sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
	 
	 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
	 if (ing_nh_info->trunk == -1) {
		 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
							 &ing_nh, PORT_NUMf, ing_nh_info->port);
		 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
							 &ing_nh, MODULE_IDf, ing_nh_info->module);
	 } else {	 
		 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
							 &ing_nh, Tf, 1);
		 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
							 &ing_nh, TGIDf, ing_nh_info->trunk);
	 }
	 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
						&ing_nh, ENTRY_TYPEf, 0x2); /* L2 DVP */
	 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
						&ing_nh, MTU_SIZEf, ing_nh_info->mtu);
	 rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
						MEM_BLOCK_ALL, nh_index, &ing_nh);


     return rv;
}


STATIC int
_bcm_tr_mpls_EGR_L3_NEXT_HOP_add(int unit,  _bcm_tr_egr_nh_info_t  *egr_nh_info, 
                                                                                uint32 flags, int nh_index, int *is_local, int is_vpws)
{
    egr_l3_next_hop_entry_t egr_nh;
    int rv;
	
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));

	

    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        ENTRY_TYPEf, egr_nh_info->entry_type);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        DVPf, egr_nh_info->dvp);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                        &egr_nh, DVP_IS_NETWORK_PORTf,
                        egr_nh_info->dvp_is_network);
    if (egr_nh_info->vc_swap_index != -1) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, VC_AND_SWAP_INDEXf,
                            egr_nh_info->vc_swap_index);
    }
    if (flags & BCM_MPLS_PORT_EGRESS_TUNNEL) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                           &egr_nh, INTF_NUMf, egr_nh_info->intf_num);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, MAC_DA_PROFILE_INDEXf,
                            egr_nh_info->macda_index);
    } else {
        if (egr_nh_info->sd_tag_vlan != -1) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, SD_TAG_VIDf, egr_nh_info->sd_tag_vlan);
        }
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            SD_TAG_ACTION_IF_PRESENTf,
                            egr_nh_info->sd_tag_action_present);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            SD_TAG_ACTION_IF_NOT_PRESENTf,
                            egr_nh_info->sd_tag_action_not_present);
        if (egr_nh_info->tpid_index != -1) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, SD_TAG_TPID_INDEXf,
                                egr_nh_info->tpid_index);
        }
    }
    if (egr_nh_info->pw_init_cnt) {
          soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                        PW_INIT_NUMf, egr_nh_info->pw_init_cnt);
    }
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        HG_LEARN_OVERRIDEf, is_vpws ? 1 : 0);
    /* FIX: For L2 MPLS, HG_MODIFY_ENABLE must be 0x0 for Ingress and Egress Chip */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                        HG_MODIFY_ENABLEf, 0x0);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, HG_HDR_SELf, 1);

    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, nh_index, &egr_nh);


     return rv;
}

/*
 * Function:
 *		_bcm_tr_mpls_compute_nhop_hash
 * Purpose:
 *		Compute the hash Index from Egress Tunnel If
 * Parameters:
 *		unit	- (IN) bcm device.
 *		egress_if	- (IN) Egress Tunnel If
 * Returns:
 *		BCM_E_XXX
 */

STATIC void
_bcm_tr_mpls_compute_nhop_hash (bcm_if_t egress_if, int *hash_idx)
{
    *hash_idx = egress_if % 1024;
}

/*
 * Function:
 *		_bcm_tr_mpls_add_vp_nh
 * Purpose:
 *		Add MPLS VP NextHop index into a list
 * Parameters:
 *		unit	- (IN) bcm device.
 *		mpls_port	- (IN) MPLS Port
 *		nh_index	 - (IN) VP NextHop Index
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_tr_mpls_add_vp_nh ( int unit, bcm_if_t  egr_if, int nh_index )
{
    _bcm_mpls_egr_nhopList_t  *nhop_cursorPtr;
    _bcm_mpls_vp_nh_list_t  *vp_cursorPtr;
    _bcm_mpls_vp_nh_list_t *newVpNode;
    int   hash_idx;

    (void) _bcm_tr_mpls_compute_nhop_hash (egr_if, &hash_idx);

   /* If Node is present, then insert the VP_index */
    for (nhop_cursorPtr = _bcm_tr_mpls_nhop_headPtr[hash_idx]; nhop_cursorPtr != NULL; nhop_cursorPtr = nhop_cursorPtr->link) {
	   if (nhop_cursorPtr->egr_if == egr_if ) {
		 for (vp_cursorPtr = nhop_cursorPtr->vp_head_ptr; vp_cursorPtr != NULL; vp_cursorPtr = vp_cursorPtr->link) {
		 	if (vp_cursorPtr->vp_nh_idx == nh_index) {
                          return BCM_E_NONE;
		 	}
		 }
		 newVpNode = (_bcm_mpls_vp_nh_list_t *) sal_alloc (sizeof (_bcm_mpls_vp_nh_list_t), "VP NH Index");
               if (NULL == newVpNode) {
                    return (BCM_E_MEMORY); 
               }	 
		 newVpNode->vp_nh_idx = nh_index;
		 newVpNode->link = nhop_cursorPtr->vp_head_ptr;
		 nhop_cursorPtr->vp_head_ptr = newVpNode;
		 return BCM_E_NONE;		 
          }
    }
    return BCM_E_NOT_FOUND;
}


/*
 * Function:
 *		bcm_tr_mpls_update_vp_nh
 * Purpose:
 *		Update NextHop Table indexed by VP NextHop index
 * Parameters:
 *		unit	- (IN) bcm device.
 *		egr_index - (IN) NextHop Index of Parent Egr Object
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr_mpls_update_vp_nh ( int unit, bcm_if_t  egr_if )
{
    _bcm_mpls_egr_nhopList_t  *nhop_cursorPtr;
    _bcm_mpls_vp_nh_list_t  *vp_cursorPtr;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    initial_ing_l3_next_hop_entry_t   initial_ing_nh;
    egr_mac_da_profile_entry_t macda;
    _bcm_tr_ing_nh_info_t ing_nh_info;
    _bcm_tr_egr_nh_info_t egr_nh_info;
    bcm_l3_egress_t nh_info;
    void *entries[1];
    int   vp_index;
    int  egr_index;
    int   hash_idx;

    (void) _bcm_tr_mpls_compute_nhop_hash (egr_if, &hash_idx);


    for (nhop_cursorPtr = _bcm_tr_mpls_nhop_headPtr[hash_idx]; nhop_cursorPtr != NULL; nhop_cursorPtr = nhop_cursorPtr->link) {
      if (nhop_cursorPtr->egr_if == egr_if) {
         for (vp_cursorPtr = nhop_cursorPtr->vp_head_ptr;  vp_cursorPtr != NULL; vp_cursorPtr = vp_cursorPtr->link) {
              vp_index = vp_cursorPtr->vp_nh_idx;

              /* Get the Egress Hext Hop Index from the Egress Object Id */
              BCM_IF_ERROR_RETURN(bcm_xgs3_get_nh_from_egress_object(unit, egr_if, 0, &egr_index));
 
              /* Read the egress next_hop entry pointed by EGRess-Object */	  
              BCM_IF_ERROR_RETURN(bcm_xgs3_nh_get(unit, egr_index, &nh_info));			  

              /* Obtain  the MPLS tunnel destination port information. */
              if (nh_info.flags & BCM_L3_TGID) {
                     ing_nh_info.port = -1;
                     ing_nh_info.module = -1;
                     ing_nh_info.trunk = nh_info.trunk; 
              } else {
                     ing_nh_info.port = nh_info.port;
                     ing_nh_info.module = nh_info.module;
                     ing_nh_info.trunk = -1; 
              }
              egr_nh_info.intf_num = nh_info.intf;
              egr_nh_info.entry_type = 0x1; /* MPLS_MACDA_PROFILE */
			  
              sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
              soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm, 
                                 &macda, MAC_ADDRESSf, nh_info.mac_addr);
              entries[0] = &macda;
               BCM_IF_ERROR_RETURN(_bcm_mac_da_profile_entry_add(unit, entries, 1,
                                              (uint32 *) &egr_nh_info.macda_index));

              /* Read the existing egress next_hop entry pointed by VP NextHop Index */
              BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL, 
								vp_index, &egr_nh));

              /* Modify  the Egress NextHop entry pointed by VP NextHop Index */
              soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                                  INTF_NUMf, egr_nh_info.intf_num);
              soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                                  ENTRY_TYPEf, egr_nh_info.entry_type);
              soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                                  MAC_DA_PROFILE_INDEXf, egr_nh_info.macda_index);

		 /* Update HW Table Memory */
		 BCM_IF_ERROR_RETURN (soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
							MEM_BLOCK_ALL, vp_index, &initial_ing_nh));

		 /* Read the existing Ingress next_hop entry pointed by VP NextHop Index */
		 BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ALL, 
						   vp_index, &ing_nh));
			  
		 /* Modify  the Ingress NextHop entry pointed by VP NextHop Index */
		 if (ing_nh_info.trunk == -1) {
			 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
								 &ing_nh, PORT_NUMf, ing_nh_info.port);
			 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
								 &ing_nh, MODULE_IDf, ing_nh_info.module);
		 } else {	 
			 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
								 &ing_nh, Tf, 1);
			 soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
								 &ing_nh, TGIDf, ing_nh_info.trunk);
		 }

		 /* Update HW Table Memory */
		 BCM_IF_ERROR_RETURN (soc_mem_write(unit, EGR_L3_NEXT_HOPm,
							MEM_BLOCK_ALL, vp_index, &egr_nh));
		 
		 /* Read the existing Initial-Ingress next_hop entry pointed by VP NextHop Index */
		 BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_ING_L3_NEXT_HOPm, MEM_BLOCK_ALL, 
						   vp_index, &initial_ing_nh));

		 /* Modify the Initial-Ingress NextHop entry pointed by VP NextHop Index */
		 if (ing_nh_info.trunk == -1) {
			 soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
								 &initial_ing_nh, PORT_NUMf, ing_nh_info.port);
			 soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
								 &initial_ing_nh, MODULE_IDf, ing_nh_info.module);
		 } else {
			 soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
								 &initial_ing_nh, Tf, 1);
			 soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
								 &initial_ing_nh, TGIDf, ing_nh_info.trunk);
		 }

		 /* Update HW Table Memory */		 
		 BCM_IF_ERROR_RETURN (soc_mem_write (unit, ING_L3_NEXT_HOPm,
							MEM_BLOCK_ALL, vp_index, &ing_nh));

         }	 
       }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *		bcm_dump_mpls_port
 * Purpose:
 *		Displays MPLS Port Structure Information
 * Parameters:
 *		mpls_port - (IN) MPLS Port Structure
 * Returns:
 *		None
 */

	
void
bcm_dump_mpls_port (bcm_mpls_port_t *mpls_port) 
{
	soc_cm_print("\n MPLS Port ID: %d", (bcm_gport_t) mpls_port->mpls_port_id);
	soc_cm_print("\n Egress Tunnel If: %d", (bcm_if_t) mpls_port->egress_tunnel_if);			
	soc_cm_print("\n flags: %X", mpls_port->flags);
	soc_cm_print("\n service_tpid: %d", mpls_port->service_tpid);
	soc_cm_print("\n match_vlan: %d", mpls_port->match_vlan);
	soc_cm_print("\n mpls_label: %d", (bcm_mpls_label_t) mpls_port->match_label);
	soc_cm_print("\n encap_id: %d", (bcm_if_t) mpls_port->encap_id);
}

/* 
 * Function:   * Function: 
 *              _bcm_tr_mpls_dump_egrNhop 
 * Purpose: 
 *              Dump Egr NextHop List
 * Parameters: 
 *              unit    - (IN) bcm device. 
 *              vp_idx   - (IN) VP NextHop Index
 * Returns: 
 *              BCM_E_XXX 
 */ 
     
void
 _bcm_tr_mpls_dump_egrNhop (void)
{ 
    _bcm_mpls_egr_nhopList_t  *nhop_cursorPtr; 
    _bcm_mpls_vp_nh_list_t *vp_cursorPtr; 
    int   index; 

    for(index=0; index < _BCM_TR_MPLS_HASH_ELEMENTS; ++index) {
         if (_bcm_tr_mpls_nhop_headPtr[index] != NULL) {
              for (nhop_cursorPtr = _bcm_tr_mpls_nhop_headPtr[index]; 
                                 nhop_cursorPtr != NULL; nhop_cursorPtr = nhop_cursorPtr->link) {
                   soc_cm_print("\n Egress Tunnel If: %d", (bcm_if_t) nhop_cursorPtr->egr_if);			 
                   for (vp_cursorPtr = nhop_cursorPtr->vp_head_ptr; 
                                 vp_cursorPtr != NULL; vp_cursorPtr = vp_cursorPtr->link) { 
                           soc_cm_print("\n VP Nhop Idx= %d", vp_cursorPtr->vp_nh_idx);
                   } 
              }
         }
    }
}


/*
 * Function:
 *		_bcm_tr_mpls_add_egrNhop
 * Purpose:
 *		Add Nhop information into a Hash-Table
 * Parameters:
 *		unit	- (IN) bcm device.
 *		egress_if	- (IN) Egress Tunnel If
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_tr_mpls_add_egrNhop ( int unit, bcm_if_t egress_if)
{
    _bcm_mpls_egr_nhopList_t  *newNhopNode;
    _bcm_mpls_egr_nhopList_t  *nhop_cursorPtr;
    int   hash_idx;

    (void) _bcm_tr_mpls_compute_nhop_hash (egress_if, &hash_idx);
    /* Does the Node exist? */
    for (nhop_cursorPtr = _bcm_tr_mpls_nhop_headPtr[hash_idx]; 
                               nhop_cursorPtr != NULL; nhop_cursorPtr = nhop_cursorPtr->link) {
        if (nhop_cursorPtr->egr_if == egress_if) {
            return (BCM_E_NONE);
        } 
    }

    newNhopNode = (_bcm_mpls_egr_nhopList_t *) 
		                   sal_alloc (sizeof (_bcm_mpls_egr_nhopList_t), "MPLS Egr Nhop");
    if (NULL == newNhopNode) {
        return (BCM_E_MEMORY); 
    }	  	
    newNhopNode->egr_if = (bcm_if_t) egress_if;
    newNhopNode->vp_head_ptr = ( _bcm_mpls_vp_nh_list_t *) NULL;
    newNhopNode->link = _bcm_tr_mpls_nhop_headPtr[hash_idx];
    _bcm_tr_mpls_nhop_headPtr[hash_idx] = newNhopNode;
    return BCM_E_NONE;
}


/*
 * Function:
 *		_bcm_tr_mpls_delete_egrNhop
 * Purpose:
 *		Delete MPLS Egress NHOP from Hash-table
 * Parameters:
 *		unit	- (IN) bcm device.
 *		egress_if	- (IN) Egress Tunnel If
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_tr_mpls_delete_egrNhop (int unit,  bcm_if_t egress_if )
{
    _bcm_mpls_egr_nhopList_t  *nhop_cursorPtr;
    _bcm_mpls_egr_nhopList_t  *removeNode;
    int   hash_idx;

    (void) _bcm_tr_mpls_compute_nhop_hash (egress_if, &hash_idx);

    removeNode = _bcm_tr_mpls_nhop_headPtr[hash_idx];
    if (removeNode != NULL)  {
       if (removeNode->egr_if ==  egress_if) {
	   	if (removeNode->vp_head_ptr == NULL) {
                _bcm_tr_mpls_nhop_headPtr[hash_idx] = removeNode->link;
	          sal_free (removeNode);
		   return BCM_E_NONE;			  
	   	} else {
                 return BCM_E_BUSY;
		}
        } else {
             for (nhop_cursorPtr = _bcm_tr_mpls_nhop_headPtr[hash_idx]; 
                                             nhop_cursorPtr->link != NULL; nhop_cursorPtr = nhop_cursorPtr->link) {
                 if (nhop_cursorPtr->link->egr_if ==  egress_if) {
			if (nhop_cursorPtr->link->vp_head_ptr == NULL) {
		          removeNode = nhop_cursorPtr->link;
		          nhop_cursorPtr->link = removeNode->link;
		          sal_free (removeNode);
		          return BCM_E_NONE;
			} else {
	                 return BCM_E_BUSY;
			}
	          }
	      }
        }
    }
    return BCM_E_NOT_FOUND;
} 

/*
 * Function:
 *		_bcm_tr_mpls_delete_vp_nh
 * Purpose:
 *		Delete VP NextHop information from List
 * Parameters:
 *		unit	- (IN) bcm device.
 *		vp_idx - (IN) VP NextHop Index
 *		egress_if	- (IN) Egress Tunnel If
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_tr_mpls_delete_vp_nh ( int unit,  int vp_idx, bcm_if_t egress_if)
{
    _bcm_mpls_egr_nhopList_t  *nhop_cursorPtr;
    _bcm_mpls_vp_nh_list_t  *vp_cursorPtr;
    _bcm_mpls_vp_nh_list_t  *vpRemoveNode;	
    int   hash_idx;

    (void) _bcm_tr_mpls_compute_nhop_hash (egress_if, &hash_idx);

    for (nhop_cursorPtr = _bcm_tr_mpls_nhop_headPtr[hash_idx]; 
                                      nhop_cursorPtr != NULL; nhop_cursorPtr = nhop_cursorPtr->link) {
        vp_cursorPtr = nhop_cursorPtr->vp_head_ptr;
	  if (vp_cursorPtr  != NULL) {
		if (vp_cursorPtr->vp_nh_idx == vp_idx)  {
			nhop_cursorPtr->vp_head_ptr = vp_cursorPtr->link;
			sal_free (vp_cursorPtr);
                     BCM_IF_ERROR_RETURN(
                             _bcm_tr_mpls_delete_egrNhop (unit, nhop_cursorPtr->egr_if));
			return BCM_E_NONE;
		} else {
			for ( ; vp_cursorPtr->link != NULL; vp_cursorPtr = vp_cursorPtr->link){
 			   if (vp_cursorPtr->link->vp_nh_idx == vp_idx) {
				vpRemoveNode = vp_cursorPtr->link;
				vp_cursorPtr->link = vpRemoveNode->link;
				sal_free (vpRemoveNode);
				return BCM_E_NONE;
			   }
			}
		} 					  
	   }
    }
    return BCM_E_NOT_FOUND;	
}

/*
 * Function:
 *		_bcm_tr_mpls_cleanup_egrNhop
 * Purpose:
 *		Cleanup MPLS Egress NHOP from Hash-table
 * Parameters:
 *		unit	- (IN) bcm device.
 * Returns:
 *		BCM_E_XXX
 */

STATIC void
_bcm_tr_mpls_cleanup_egrNhop (int unit)
{
    _bcm_mpls_egr_nhopList_t  *nhop_cursorPtr;
    _bcm_mpls_vp_nh_list_t	*vp_cursorPtr;
    int   index,rv;

    for(index=0; index < _BCM_TR_MPLS_HASH_ELEMENTS; ++index) {
         if (_bcm_tr_mpls_nhop_headPtr[index] != NULL) {
              nhop_cursorPtr = _bcm_tr_mpls_nhop_headPtr[index];
              while ( nhop_cursorPtr != NULL ) {
                   vp_cursorPtr = nhop_cursorPtr->vp_head_ptr;
                   while (vp_cursorPtr != NULL) {
                        rv = _bcm_tr_mpls_delete_vp_nh (unit, vp_cursorPtr->vp_nh_idx,
                                                                nhop_cursorPtr->egr_if);
                        nhop_cursorPtr = _bcm_tr_mpls_nhop_headPtr[index];
                        if (nhop_cursorPtr != NULL) {
                             vp_cursorPtr = nhop_cursorPtr->vp_head_ptr;
                        } else {
                             vp_cursorPtr = NULL;
                        }
                   }
              }
         }
    }
} 


/* 
 * Function:   * Function: 
 *              _bcm_tr_mpls_get_vp_nh 
 * Purpose: 
 *              Find whether  VP NextHop is present within list 
 * Parameters: 
 *              unit    - (IN) bcm device. 
 *              vp_idx   - (IN) VP NextHop Index
 *              egress_if - (OUT) Egr_If
 * Returns: 
 *              BCM_E_XXX 
 */ 
     
STATIC int 
 _bcm_tr_mpls_get_vp_nh (int unit, int vp_index, bcm_if_t *egress_if) 
{ 
    _bcm_mpls_egr_nhopList_t  *nhop_cursorPtr; 
    _bcm_mpls_vp_nh_list_t *vp_cursorPtr; 
    int   index; 

    for(index=0; index < _BCM_TR_MPLS_HASH_ELEMENTS; ++index) {
         if (_bcm_tr_mpls_nhop_headPtr[index] != NULL) {
              for (nhop_cursorPtr = _bcm_tr_mpls_nhop_headPtr[index]; 
                                 nhop_cursorPtr != NULL; nhop_cursorPtr = nhop_cursorPtr->link) { 
                   for (vp_cursorPtr = nhop_cursorPtr->vp_head_ptr; 
                                 vp_cursorPtr != NULL; vp_cursorPtr = vp_cursorPtr->link) { 
                        if (vp_index == vp_cursorPtr->vp_nh_idx) {
                             *egress_if = nhop_cursorPtr->egr_if;
                             return BCM_E_NONE; 
                        }
                   } 
              }
         }
    }
    return BCM_E_NOT_FOUND; 
}

/*
 * Function:
 *		_bcm_tr_mpls_get_vc_and_swap_table_index
 * Purpose:
 *		Obtain Index into  VC_AND_SWAP_TABLE
 * Parameters:
 *		IN :  Unit
 *           IN :  num_vc
 *           OUT : vc_swap_index
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_tr_mpls_get_vc_and_swap_table_index (int unit, int num_vc, int *vc_swap_index)
{
   int  i;
   
       /* Try getting index from 2nd half of MPLS_VC_AND_SWAP table */
       for (i = 0; i < num_vc; i++) {
		 if (!_BCM_MPLS_VC_NON_COUNT_USED_GET(unit, i)) {
			   break;
		 }
       }
	   
       if (i == num_vc) {
            /* Get index from 1st half of MPLS_VC_AND_SWAP table */
	     for (i = 0; i < num_vc; i++) {
			 if (!_BCM_MPLS_VC_COUNT_USED_GET(unit, i)) {
						break;
			 }
	     }
	     if (i == num_vc) {
			return  BCM_E_RESOURCE;
	     }
	     _BCM_MPLS_VC_COUNT_USED_SET(unit, i);
	     *vc_swap_index = i;
       } else {
	   _BCM_MPLS_VC_NON_COUNT_USED_SET(unit, i);
	   *vc_swap_index = i + num_vc;
       }
       return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_mpls_pw_init_counter_set
 * Purpose:
 *      Obtain the pw_init counter index, Set the EGR L3 NH entry parameter
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      egr_nh_info  - (IN/OUT) Egress NHOP Internal Structurel
 * Returns:
 *      BCM_E_XXX
 */     

STATIC int
_bcm_tr_mpls_pw_init_counter_set (int unit, _bcm_tr_egr_nh_info_t  *egr_nh_info)
{
    int  pw_init_cnt=-1;
    egr_pw_init_counters_entry_t pw_init_entry;
    int  vc_swap_index;
	
    vc_swap_index =  egr_nh_info->vc_swap_index;

    /* pw_init_counter index is the LSB 12-bits of vc_and_swap_index */
    pw_init_cnt = vc_swap_index & 0x0FFF;

    _BCM_MPLS_PW_INIT_USED_SET(unit, pw_init_cnt);
	
    egr_nh_info->pw_init_cnt = pw_init_cnt;
	
    sal_memset(&pw_init_entry, 0, sizeof(egr_pw_init_counters_entry_t));
    (void) WRITE_EGR_PW_INIT_COUNTERSm(unit, MEM_BLOCK_ALL, pw_init_cnt,
									   &pw_init_entry);
    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_tr_mpls_pw_init_counter_delete
 * Purpose:
 *      Get L2 MPLS PW Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 * Returns:
 *      BCM_E_XXX
 */     

STATIC int
_bcm_tr_mpls_pw_init_counter_delete (int unit, int nh_index)
{
    int  pw_init_cnt=-1,rv;
    egr_l3_next_hop_entry_t egr_nh;
    egr_pw_init_counters_entry_t pw_init_entry;

    rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
								 nh_index, &egr_nh);
    if (rv < 0) {
        return rv;
    }

    pw_init_cnt = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, PW_INIT_NUMf);

    sal_memset(&pw_init_entry, 0, sizeof(egr_pw_init_counters_entry_t));
    (void) WRITE_EGR_PW_INIT_COUNTERSm(unit, MEM_BLOCK_ALL, pw_init_cnt,
								   &pw_init_entry);
    _BCM_MPLS_PW_INIT_USED_CLR (unit, pw_init_cnt);
    return BCM_E_NONE;
}


STATIC int
_bcm_tr_mpls_l2_nh_info_add(int unit, bcm_mpls_port_t *mpls_port, int vp,
                            int drop, int is_vpws, int *nh_index, 
                            bcm_port_t *local_port, int *is_local)
{
    egr_l3_next_hop_entry_t egr_nh;
    egr_mac_da_profile_entry_t macda;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    _bcm_tr_ing_nh_info_t ing_nh_info;
    _bcm_tr_egr_nh_info_t egr_nh_info;
    bcm_if_t old_tunnel_if = 0;
    bcm_l3_egress_t nh_info;
    uint32 nh_flags;
    int gport_id, rv=0, temp_nh_index = -1, num_vc=0, i;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int action_present, action_not_present, tpid_index = -1;
    int old_tpid_idx = -1, old_macda_idx = -1, old_vc_swap_idx = -1;
    void *entries[1];
#if defined(BCM_TRIUMPH2_SUPPORT)
    int failover_vp, failover_nh_index;
    ing_dvp_table_entry_t failover_dvp;
#endif /* BCM_TRIUMPH2_SUPPORT  */


    /* Initialize values */
    *local_port = 0;
    *is_local = 0;
     if (mpls_port->mtu != 0) {
	 ing_nh_info.mtu = mpls_port->mtu;
     }  else {
       ing_nh_info.mtu = 0x3FFF;
     }
    ing_nh_info.port = -1;
    ing_nh_info.module = -1;
    ing_nh_info.trunk = -1;

    egr_nh_info.dvp = vp;
    egr_nh_info.dvp_is_network = (mpls_port->flags & BCM_MPLS_PORT_NETWORK) ? 1 : 0;
    egr_nh_info.entry_type = 0;
    egr_nh_info.sd_tag_action_present = 0;
    egr_nh_info.sd_tag_action_not_present = 0;
    egr_nh_info.intf_num = -1;
    egr_nh_info.sd_tag_vlan = -1;
    egr_nh_info.macda_index = 0;
    egr_nh_info.vc_swap_index = -1;
    egr_nh_info.tpid_index = -1;
    egr_nh_info.pw_init_cnt = 0;

    if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
        if ((*nh_index > soc_mem_index_max(unit, EGR_L3_NEXT_HOPm)) ||
            (*nh_index < soc_mem_index_min(unit, EGR_L3_NEXT_HOPm)))  {
            return BCM_E_PARAM;
        }
        /* Read the existing egress next_hop entry */
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                          *nh_index, &egr_nh);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /*
         * Allocate a next-hop entry. By calling bcm_xgs3_nh_add()
         * with _BCM_L3_SHR_WRITE_DISABLE flag, a next-hop index is
         * allocated but nothing is written to hardware. The "nh_info"
         * in this case is not used, so just set to all zeros.
         */
        sal_memset(&nh_info, 0, sizeof(bcm_l3_egress_t));
        nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
        BCM_IF_ERROR_RETURN(bcm_xgs3_nh_add(unit, nh_flags, &nh_info, nh_index));
	 rv = _bcm_tr_mpls_add_egrNhop (unit, mpls_port->egress_tunnel_if);
	 _BCM_MPLS_CLEANUP(rv);
        rv = _bcm_tr_mpls_add_vp_nh (unit, mpls_port->egress_tunnel_if, *nh_index);
        _BCM_MPLS_CLEANUP(rv);
    }

    /*
     * Indexes in the first half of EGR_MPLS_VC_AND_SWAP_LABEL_TABLE
     * also get used as the index into the EGR_PW_INIT_COUNTERS table.
     * Set num_vc to half the EGR_MPLS_VC_AND_SWAP_LABEL_TABLE size.
     */
    num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
    num_vc /= 2;

    if (mpls_port->flags & BCM_MPLS_PORT_EGRESS_TUNNEL) {
        /* Egressing into an MPLS tunnel */
        if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, mpls_port->egress_tunnel_if)) {
            /* invalid egress object */
            rv = BCM_E_PARAM;
            goto cleanup;
        } else if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
            /* Be sure that the existing entry is already setup to
             * egress into an MPLS tunnel. If not, return BCM_E_PARAM.
             */
            egr_nh_info.entry_type = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
            if (egr_nh_info.entry_type != 0x1) { /* != MPLS_MACDA_PROFILE */
                rv = BCM_E_PARAM;		
                goto cleanup;
            }
            /* Remember old MAC_DA_PROFILE_INDEX */
            old_macda_idx = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 MAC_DA_PROFILE_INDEXf);
            /* Remember old VC_AND_SWAP_INDEX */
            old_vc_swap_idx = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 VC_AND_SWAP_INDEXf);
            /* Remember old TPID index if it's used */
            rv = soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                              MEM_BLOCK_ALL, old_vc_swap_idx, &vc_entry);
            _BCM_MPLS_CLEANUP(rv);
            action_present = 
                soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit, 
                                                 &vc_entry,
                                                 SD_TAG_ACTION_IF_PRESENTf);
            action_not_present = 
                soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit, 
                                                 &vc_entry,
                                                 SD_TAG_ACTION_IF_NOT_PRESENTf);
            if ((action_not_present == 0x1) || (action_present == 0x1)) {
                /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
                 * index of the entry getting replaced is valid. Save
                 * the old tpid index to be deleted later.
                 */
                old_tpid_idx = 
                    soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit, 
                                                 &vc_entry, SD_TAG_TPID_INDEXf);
            }

            /* Remember old tunnel_if */
            rv = _bcm_tr_mpls_egress_intf_find(unit, *nh_index, &old_tunnel_if);
            _BCM_MPLS_CLEANUP(rv);
        }

        /* 
         * Get egress next-hop index from egress object and
         * increment egress object reference count. 
         */
        rv = bcm_xgs3_get_nh_from_egress_object(unit, mpls_port->egress_tunnel_if,
                                                0, &temp_nh_index);
        _BCM_MPLS_CLEANUP(rv);

        rv = bcm_xgs3_nh_get(unit, temp_nh_index, &nh_info);
        _BCM_MPLS_CLEANUP(rv);

        /* Resolve the gport */
        rv = _bcm_esw_gport_resolve(unit, mpls_port->port, &mod_out, 
                                                            &port_out, &trunk_id, &gport_id);
        BCM_IF_ERROR_RETURN(rv);
		
        /* Copy the MPLS tunnel destination port information. */
        if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
            ing_nh_info.port = -1;
            ing_nh_info.module = -1;
            ing_nh_info.trunk = trunk_id; 
        } else {
            ing_nh_info.port = port_out;
            ing_nh_info.module = mod_out;
            ing_nh_info.trunk = -1; 
        }
        egr_nh_info.intf_num = nh_info.intf;
        egr_nh_info.entry_type = 0x1; /* MPLS_MACDA_PROFILE */

        sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
        soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm, 
                             &macda, MAC_ADDRESSf, nh_info.mac_addr);
        entries[0] = &macda;
        rv = _bcm_mac_da_profile_entry_add(unit, entries, 1,
                                           (uint32 *) &egr_nh_info.macda_index);
        _BCM_MPLS_CLEANUP(rv);

        if ((mpls_port->flags & BCM_MPLS_PORT_COUNTED) ||
            (mpls_port->flags & BCM_MPLS_PORT_SEQUENCED)) {

            /* Get index from 1st half of MPLS_VC_AND_SWAP table */
            for (i = 0; i < num_vc; i++) {
                if (!_BCM_MPLS_VC_COUNT_USED_GET(unit, i)) {
                    break;
                }
            }
            if (i == num_vc) {
                rv = BCM_E_RESOURCE;
                goto cleanup;
            }
            _BCM_MPLS_VC_COUNT_USED_SET(unit, i);
            egr_nh_info.vc_swap_index = i;
            rv = _bcm_tr_mpls_pw_init_counter_set (unit, &egr_nh_info);
            _BCM_MPLS_CLEANUP(rv);
        } else {
            rv = _bcm_tr_mpls_get_vc_and_swap_table_index (unit, 
                                   num_vc,  &egr_nh_info.vc_swap_index);
            _BCM_MPLS_CLEANUP(rv);
        }

        /* Program the MPLS_VC_AND_SWAP table entry */
        sal_memset(&vc_entry, 0, 
                   sizeof(egr_mpls_vc_and_swap_label_table_entry_t));
        if (BCM_XGS3_L3_MPLS_LBL_VALID(mpls_port->egress_label.label)) {
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                &vc_entry, MPLS_LABELf, 
                                mpls_port->egress_label.label);
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                &vc_entry, MPLS_LABEL_ACTIONf, 
                                0x1); /* PUSH one label */
        } else {
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                &vc_entry, MPLS_LABEL_ACTIONf, 
                                0x0); /* NOOP */
        }
        if (mpls_port->egress_label.flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                &vc_entry, MPLS_TTLf,
                                mpls_port->egress_label.ttl);
        } else {
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                &vc_entry, MPLS_TTLf, 0x0);
        }
        if (mpls_port->flags & BCM_MPLS_PORT_SEQUENCED) {
            /* Append control word with sequence number */
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                &vc_entry, CW_INSERT_FLAGf, 0x2);
        } else if (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD) {
            /* Append control word with NO sequence number */
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                &vc_entry, CW_INSERT_FLAGf, 0x1);
        }
        if ((mpls_port->flags & BCM_MPLS_PORT_COUNTED) ||
            (mpls_port->flags & BCM_MPLS_PORT_SEQUENCED)) {
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, UPDATE_PW_INIT_COUNTERSf, 0x1);
        }
        if ((mpls_port->egress_label.flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
            (mpls_port->egress_label.flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) {

            if ((mpls_port->egress_label.flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) ||
                (mpls_port->egress_label.flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ||
                (mpls_port->egress_label.flags & BCM_MPLS_EGRESS_LABEL_PRI_REMARK)) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            /* Use the specified EXP, PRI and CFI */
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_SELECTf, 
                                0x0); /* USE_FIXED */
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXPf, 
                                mpls_port->egress_label.exp);
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, NEW_PRIf, 
                                mpls_port->egress_label.pkt_pri);
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, NEW_CFIf, 
                                mpls_port->egress_label.pkt_cfi);
        } else if (mpls_port->egress_label.flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
            if (mpls_port->egress_label.flags & BCM_MPLS_EGRESS_LABEL_PRI_SET) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            /* Use EXP-map for EXP, PRI and CFI */
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_SELECTf, 
                                0x1); /* USE_MAPPING */
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_MAPPING_PTRf,
                                mpls_port->egress_label.qos_map_id);
        } else { /* default BCM_MPLS_EGRESS_LABEL_EXP_COPY */
            /* Use EXP from incoming label. If there is no incoming label,
             * use the specified EXP value. Use EXP-map for PRI/CFI.
             */
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_SELECTf,
                                0x3); /* USE_SWAP */
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXPf,
                                mpls_port->egress_label.exp);

            /* Use EXP-map for PRI/CFI */
            soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_MAPPING_PTRf,
                                mpls_port->egress_label.qos_map_id);
        }
        if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_TAGGED) {
            if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_ADD) {
                if (!BCM_VLAN_VALID(mpls_port->egress_service_vlan)) {
                       rv = BCM_E_PARAM;
                       goto cleanup;
                }
                soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, SD_TAG_VIDf,
                                    mpls_port->egress_service_vlan);
                soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, SD_TAG_ACTION_IF_NOT_PRESENTf,
                                    0x1); /* ADD */
            }
            if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_TPID_REPLACE) {
                if (!BCM_VLAN_VALID(mpls_port->egress_service_vlan)) {
                           rv = BCM_E_PARAM;
                           goto cleanup;
                }
                soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, SD_TAG_VIDf,
                                    mpls_port->egress_service_vlan);
                soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, SD_TAG_ACTION_IF_PRESENTf,
                                    0x1); /* REPLACE_VID_TPID */
            } else if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_REPLACE) {
                if (!BCM_VLAN_VALID(mpls_port->egress_service_vlan)) {
                           rv = BCM_E_PARAM;
                           goto cleanup;
                }
                soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, SD_TAG_VIDf,
                                    mpls_port->egress_service_vlan);
                soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, SD_TAG_ACTION_IF_PRESENTf,
                                    0x2); /* REPLACE_VID_ONLY */
            } else if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_DELETE) {
                soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, SD_TAG_ACTION_IF_PRESENTf,
                                    0x3); /* DELETE */
            }

            if ((mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_ADD) ||
                (mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_TPID_REPLACE)) {
                /* TPID value is used */
                rv = _bcm_fb2_outer_tpid_entry_add(unit, mpls_port->service_tpid,
                                               &tpid_index);
                _BCM_MPLS_CLEANUP(rv);
                soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, SD_TAG_TPID_INDEXf, tpid_index);
            }
        }

        rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                           MEM_BLOCK_ALL, egr_nh_info.vc_swap_index,
                           &vc_entry);
        _BCM_MPLS_CLEANUP(rv);
    } else {
        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
            /* Be sure that the existing entry is not egressing into
             * an MPLS tunnel. If it is, return BCM_E_PARAM.
             */
            egr_nh_info.entry_type = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
            if (egr_nh_info.entry_type != 0x2) { /* != MPLS_SD_TAG_ACTIONS */
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            action_present = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 SD_TAG_ACTION_IF_PRESENTf);
            action_not_present = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 SD_TAG_ACTION_IF_NOT_PRESENTf);
            if ((action_not_present == 0x1) || (action_present == 0x1)) {
                /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
                 * index of the entry getting replaced is valid. Save
                 * the old tpid index to be deleted later.
                 */
                old_tpid_idx = 
                    soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                     SD_TAG_TPID_INDEXf);
            }
        }
        egr_nh_info.entry_type = 0x2; /* MPLS_SD_TAG_ACTIONS */

        rv = _bcm_esw_gport_resolve(unit, mpls_port->port, &mod_out, 
                                    &port_out, &trunk_id, &gport_id);
        _BCM_MPLS_CLEANUP(rv);

        if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
            ing_nh_info.module = -1;
            ing_nh_info.port = -1;
            ing_nh_info.trunk = trunk_id;
        } else {
            ing_nh_info.module = mod_out;
            ing_nh_info.port = port_out;
            ing_nh_info.trunk = -1;
        }

        if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_TAGGED) {
            if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_ADD) {
                if (!BCM_VLAN_VALID(mpls_port->egress_service_vlan)) {
                     rv = BCM_E_PARAM;
                     goto cleanup;
                }
                egr_nh_info.sd_tag_vlan = mpls_port->egress_service_vlan; 
                egr_nh_info.sd_tag_action_not_present = 0x1; /* ADD */
            }
            if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_TPID_REPLACE) {
                if (!BCM_VLAN_VALID(mpls_port->egress_service_vlan)) {
                     rv = BCM_E_PARAM;
                     goto cleanup;
                }
                /* REPLACE_VID_TPID */
                egr_nh_info.sd_tag_vlan = mpls_port->egress_service_vlan; 
                egr_nh_info.sd_tag_action_present = 0x1;
            } else if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_REPLACE) {
                if (!BCM_VLAN_VALID(mpls_port->egress_service_vlan)) {
                    rv = BCM_E_PARAM;
                    goto cleanup;
                }
                /* REPLACE_VID_ONLY */
                egr_nh_info.sd_tag_vlan = mpls_port->egress_service_vlan; 
                egr_nh_info.sd_tag_action_present = 0x2;
            } else if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_DELETE) {
                egr_nh_info.sd_tag_action_present = 0x3; /* DELETE */
            }

            if ((mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_ADD) ||
                (mpls_port->flags & BCM_MPLS_PORT_SERVICE_VLAN_TPID_REPLACE)) {
                /* TPID value is used */
                rv = _bcm_fb2_outer_tpid_entry_add(unit, mpls_port->service_tpid, 
                                                   &tpid_index);
                _BCM_MPLS_CLEANUP(rv);
                egr_nh_info.tpid_index = tpid_index;
            }
        }
    }

    /* Write INITIAL_ING_L3_NEXT_HOP entry */
    rv = _bcm_tr_mpls_INITIAL_ING_L3_NEXT_HOP_add(unit, &ing_nh_info, *nh_index, local_port, is_local);	
    _BCM_MPLS_CLEANUP(rv);

    /* Write EGR_L3_NEXT_HOP entry */
    rv = _bcm_tr_mpls_EGR_L3_NEXT_HOP_add(unit,  &egr_nh_info,  mpls_port->flags, *nh_index,  is_local,  is_vpws);
    _BCM_MPLS_CLEANUP(rv);

    /* Write ING_L3_NEXT_HOP entry */
    rv = _bcm_tr_mpls_ING_L3_NEXT_HOP_add(unit,	&ing_nh_info, *nh_index, drop);		
    _BCM_MPLS_CLEANUP(rv);

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_failover)) {
         _BCM_MPLS_FAILOVER_VALID_RANGE ( mpls_port->failover_id ) {
              /* Get egress next-hop index from Failover MPLS gport */
              failover_vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->failover_port_id);

              rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, failover_vp, &failover_dvp);
              _BCM_MPLS_CLEANUP(rv);

              failover_nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &failover_dvp,
                                                      NEXT_HOP_INDEXf);
              rv = bcm_tr2_failover_prot_nhi_create( 
                                                      unit, (uint32) *nh_index);
              _BCM_MPLS_CLEANUP(rv);

              rv = bcm_tr2_failover_prot_nhi_set(
                                                      unit, (uint32) *nh_index,
                                                      failover_nh_index, mpls_port->failover_id );
              _BCM_MPLS_CLEANUP(rv);
         }
    }
#endif /* BCM_TRIUMPH2_SUPPORT  */

    /* Delete old TPID, MAC and VC_SWAP indexes */
    if (old_macda_idx != -1) {
        rv = _bcm_mac_da_profile_entry_delete(unit, old_macda_idx);
        _BCM_MPLS_CLEANUP(rv);
    }
    if (old_vc_swap_idx != -1) {
        if (old_vc_swap_idx > num_vc) {
            _BCM_MPLS_VC_NON_COUNT_USED_CLR(unit, old_vc_swap_idx - num_vc);
        } else {
            _BCM_MPLS_VC_COUNT_USED_CLR(unit, old_vc_swap_idx);
        }
    } 
    if (old_tpid_idx != -1) {
        (void)_bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx);
    }
    if (old_tunnel_if != 0) {
        rv = bcm_xgs3_nh_del(unit, 0, 
                             (old_tunnel_if - BCM_XGS3_EGRESS_IDX_MIN));
        _BCM_MPLS_CLEANUP(rv);
    }
    return rv;

cleanup:
    if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);
    }
    if (temp_nh_index != -1) {
        (void) bcm_xgs3_nh_del(unit, 0, temp_nh_index);
    }
    if (tpid_index != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }
    if (egr_nh_info.macda_index != -1) {
        (void) _bcm_mac_da_profile_entry_delete(unit, egr_nh_info.macda_index);
    }
    if (egr_nh_info.vc_swap_index != -1) {
        if (egr_nh_info.vc_swap_index < num_vc) {
            _BCM_MPLS_VC_COUNT_USED_CLR(unit, 
                egr_nh_info.vc_swap_index);
        } else {
            _BCM_MPLS_VC_NON_COUNT_USED_CLR(unit, 
                egr_nh_info.vc_swap_index - num_vc);
        }
    }
    return rv;
}

STATIC int
_bcm_tr_mpls_l2_nh_info_delete(int unit, int nh_index)
{
    int rv, num_vc, old_macda_idx = -1, old_vc_swap_idx = -1;
    int action_present, action_not_present, old_tpid_idx = -1;
    bcm_if_t tunnel_if = 0;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &initial_ing_nh));

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf) == 0x1) {
        /* Egressing into an MPLS tunnel */
        old_macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                         MAC_DA_PROFILE_INDEXf);
        old_vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                           VC_AND_SWAP_INDEXf);

        /* Get old TPID index if it's used */
        rv = soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                          MEM_BLOCK_ALL, old_vc_swap_idx, &vc_entry);
        BCM_IF_ERROR_RETURN(rv);
        action_present =
            soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit,
                                             &vc_entry,
                                             SD_TAG_ACTION_IF_PRESENTf);
        action_not_present =
            soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit,
                                             &vc_entry,
                                             SD_TAG_ACTION_IF_NOT_PRESENTf);
        if ((action_not_present == 0x1) || (action_present == 0x1)) {
            /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
             * index of the entry is valid. Save
             * the old tpid index to be deleted later.
             */
            old_tpid_idx =
                soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit,
                                             &vc_entry, SD_TAG_TPID_INDEXf);
        }

        /* Find the tunnel_if */
        rv = _bcm_tr_mpls_get_vp_nh (unit, nh_index, &tunnel_if);
        if ( rv < 0) {
            return rv;
        }	

        rv = _bcm_tr_mpls_pw_init_counter_delete (unit, nh_index);
        if ( rv < 0) {
            return rv;
        }	
    } else if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                ENTRY_TYPEf) == 0x2) {
        /* egressing into a regular port */
        action_present =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             SD_TAG_ACTION_IF_PRESENTf);
        action_not_present =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             SD_TAG_ACTION_IF_NOT_PRESENTf);
        if ((action_not_present == 0x1) || (action_present == 0x1)) {
            /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
             * index of the entry is valid. Save
             * the old tpid index to be deleted later.
             */
            old_tpid_idx =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 SD_TAG_TPID_INDEXf);
        }
    } else {
        return BCM_E_NOT_FOUND;
    }

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mpls_failover)) {
              rv = bcm_tr2_failover_prot_nhi_cleanup  (unit, nh_index);
              if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
                    return rv;
              }
    }
#endif /* BCM_TRIUMPH2_SUPPORT  */

    /* Clear EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &egr_nh));

    /* Clear ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write (unit, ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &ing_nh));

    /* Clear INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &initial_ing_nh));

    /* Delete old TPID, MAC, tunnel next-hop and VC_SWAP indexes */
    if (old_vc_swap_idx != -1) {
        num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
        if (old_vc_swap_idx > num_vc) {
            _BCM_MPLS_VC_NON_COUNT_USED_CLR(unit, old_vc_swap_idx - num_vc);
        } else {
            _BCM_MPLS_VC_COUNT_USED_CLR(unit, old_vc_swap_idx);
        }
    }
    if (old_macda_idx != -1) {
        rv = _bcm_mac_da_profile_entry_delete(unit, old_macda_idx);
        BCM_IF_ERROR_RETURN(rv);
    }
    if (old_tpid_idx != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx);
    }
    if (tunnel_if != 0) {
        rv = bcm_xgs3_nh_del(unit, 0, 
                             (tunnel_if - BCM_XGS3_EGRESS_IDX_MIN));
        BCM_IF_ERROR_RETURN(rv);
    }

    BCM_IF_ERROR_RETURN(_bcm_tr_mpls_delete_vp_nh ( unit, nh_index, tunnel_if ));
	
    /* Free the next-hop entry. */
    rv = bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index);
    return rv;
}

STATIC int
_bcm_tr_mpls_l2_nh_info_get(int unit, bcm_mpls_port_t *mpls_port, int nh_index, int network_port_flag)
{
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    int rv, vc_swap_idx, action_present, action_not_present;
    int tpid_idx = -1;
    bcm_module_t mod_out, mod_in;
    bcm_port_t port_out, port_in;
    bcm_trunk_t trunk_id;
    int ttl = 0, entry_type;

    /* Read the HW entries */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &ing_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));
    entry_type = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf);

    if (entry_type == 0x2) {
        /* Egressing into an MPLS tunnel, find the tunnel_if */
        if (network_port_flag) {
              rv = _bcm_tr_mpls_egress_intf_find(unit, nh_index, 
                                           &mpls_port->egress_tunnel_if);
              BCM_IF_ERROR_RETURN (rv);
              mpls_port->flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
        }

        vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                       VC_AND_SWAP_INDEXf);
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                      MEM_BLOCK_ANY, vc_swap_idx, &vc_entry));

        if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_LABEL_ACTIONf)) {
            mpls_port->egress_label.label = 
                soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, MPLS_LABELf);
            ttl =  soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, MPLS_TTLf);
            if (ttl) {
                mpls_port->egress_label.ttl = ttl;
                mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            } else {
                mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            }
            if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_entry, MPLS_EXP_SELECTf) == 0x0) {
                /* Use the specified EXP, PRI and CFI */
                mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
                mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRI_SET;
                mpls_port->egress_label.exp =
                    soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                        &vc_entry, MPLS_EXPf);
                mpls_port->egress_label.pkt_pri =
                    soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                        &vc_entry, NEW_PRIf);
                mpls_port->egress_label.pkt_cfi =
                    soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                        &vc_entry, NEW_CFIf);
            } else if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                           &vc_entry, MPLS_EXP_SELECTf) == 0x1) {
                /* Use EXP-map for EXP, PRI and CFI */
                mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_REMARK;
                mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
                mpls_port->egress_label.qos_map_id =
                    soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                        &vc_entry, MPLS_EXP_MAPPING_PTRf);
            } else {
                /* Use EXP from incoming MPLS label. If no incoming label, use the specified
                 * EXP value. Use EXP-map for PRI/CFI.
                 */
                mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
                mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
                mpls_port->egress_label.exp =
                    soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                        &vc_entry, MPLS_EXPf);
                mpls_port->egress_label.qos_map_id =
                    soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                        &vc_entry, MPLS_EXP_MAPPING_PTRf);
            }
        }
        if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, CW_INSERT_FLAGf) == 0x2) {
            mpls_port->flags |= BCM_MPLS_PORT_SEQUENCED;
        } else if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                       &vc_entry, CW_INSERT_FLAGf) == 0x1) {
            mpls_port->flags |= BCM_MPLS_PORT_CONTROL_WORD;
        }
        if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, UPDATE_PW_INIT_COUNTERSf)) {
            mpls_port->flags |= BCM_MPLS_PORT_COUNTED;
        }

        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
            BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
        } else {
            mod_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            port_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);

            rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
        }

        action_present =
            soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit,
                                             &vc_entry,
                                             SD_TAG_ACTION_IF_PRESENTf);
        action_not_present =
            soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit,
                                             &vc_entry,
                                             SD_TAG_ACTION_IF_NOT_PRESENTf);
        if ((action_not_present == 0x1) || (action_present == 0x1)) {
            /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
             * index of the entry is valid. Get the tpid index for later.
             */
            tpid_idx =
                soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit,
                                             &vc_entry, SD_TAG_TPID_INDEXf);
            mpls_port->egress_service_vlan =
                soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit,
                                             &vc_entry, SD_TAG_VIDf);
            if (action_not_present) {
                mpls_port->flags |= BCM_MPLS_PORT_SERVICE_VLAN_ADD;
            }
            if (action_present) {
                mpls_port->flags |= BCM_MPLS_PORT_SERVICE_VLAN_TPID_REPLACE;
            }
        } else if (action_present == 0x2) { /* REPLACE_VID_ONLY */
            mpls_port->flags |= BCM_MPLS_PORT_SERVICE_VLAN_REPLACE;
            mpls_port->egress_service_vlan =
                soc_EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm_field32_get(unit,
                                             &vc_entry, SD_TAG_VIDf);
        } else if (action_present == 0x3) { /* DELETE */
            mpls_port->flags |= BCM_MPLS_PORT_SERVICE_VLAN_DELETE;
        }

        if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                          DVP_IS_NETWORK_PORTf)) {
              mpls_port->flags |= BCM_MPLS_PORT_NETWORK;
        }

        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, DROPf)) {
              mpls_port->flags |= BCM_MPLS_PORT_DROP;
        }

    } else {
        return BCM_E_NOT_FOUND;
    }

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         DVP_IS_NETWORK_PORTf)) {
        mpls_port->flags |= BCM_MPLS_PORT_NETWORK;
    }

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, DROPf)) {
        mpls_port->flags |= BCM_MPLS_PORT_DROP;
    }
	
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_mpls_match_trunk_add
 * Purpose:
 *      Assign SVP of an MPLS Trunk port
 * Parameters:
 *      unit    - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
 _bcm_tr_mpls_match_trunk_add(int unit, bcm_trunk_t tgid, int vp)
{
     bcm_trunk_add_info_t tinfo;
     int p;
     int modid_local = 0;
     int local_trunk_port[BCM_TRUNK_MAX_PORTCNT];
     int num_local_trunk_ports = 0;
     int rv = BCM_E_NONE;
     int i = 0;
     int num_bits_for_port = 0;
     bcm_module_t mod_out;
     bcm_port_t port_out;

     bcm_trunk_add_info_t_init(&tinfo);
     BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, tgid, &tinfo));

     num_bits_for_port =
             _shr_popcount((unsigned int)SOC_PORT_ADDR_MAX(unit));

     for (p = 0; p < tinfo.num_ports; p++) {
        BCM_IF_ERROR_RETURN(
               bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                        tinfo.tm[p], tinfo.tp[p], &mod_out, &port_out));
        BCM_IF_ERROR_RETURN(
              _bcm_esw_modid_is_local(unit, mod_out, &modid_local));
         if (!modid_local) {
             continue;
         }
        i = (mod_out << num_bits_for_port) | (port_out);
         rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                 i, SOURCE_VPf, vp);
         if (BCM_FAILURE(rv)) {
             goto trunk_cleanup;
         }
         rv = soc_mem_field32_modify(unit, PORT_TABm, port_out,
                                     PORT_OPERATIONf, 0x1); /* L2_SVP */
         if (BCM_FAILURE(rv)) {
             goto trunk_cleanup;
         }
         local_trunk_port[num_local_trunk_ports++] = i;
     }

     return BCM_E_NONE;

 trunk_cleanup:
     for (;p >= 0; p--) {
         rv = _bcm_esw_modid_is_local(unit, mod_out, &modid_local);
         if (!modid_local) {
             continue;
         }
         i = (mod_out << num_bits_for_port) | (port_out);
         rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                 i, SOURCE_VPf, 0);
         rv = soc_mem_field32_modify(unit, PORT_TABm, port_out,
                                     PORT_OPERATIONf, 0);
     }
     return rv;
}

/*
 * Function:
 *      _bcm_tr_mpls_match_trunk_delete
 * Purpose:
 *      Remove SVP of an MPLS Trunk port
 * Parameters:
 *      unit    - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
 _bcm_tr_mpls_match_trunk_delete(int unit, bcm_trunk_t tgid, int vp)
{
     bcm_trunk_add_info_t tinfo;
     int p;
     int modid_local = 0;
     int local_trunk_port[BCM_TRUNK_MAX_PORTCNT];
     int num_local_trunk_ports = 0;
     int rv = BCM_E_NONE;
     int i = 0;
     int num_bits_for_port = 0;
     bcm_module_t mod_out;
     bcm_port_t port_out;

     bcm_trunk_add_info_t_init(&tinfo);
     BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, tgid, &tinfo));

     num_bits_for_port =
         _shr_popcount((unsigned int)SOC_PORT_ADDR_MAX(unit));

     for (p = 0; p < tinfo.num_ports; p++) {
         BCM_IF_ERROR_RETURN(
                         bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                                     tinfo.tm[p], tinfo.tp[p], &mod_out, &port_out));
         BCM_IF_ERROR_RETURN(
                         _bcm_esw_modid_is_local(unit, mod_out, &modid_local));
         if (!modid_local) {
             continue;
         }
         i = (mod_out << num_bits_for_port) | (port_out);
         rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                 i, SOURCE_VPf, 0);
         if (BCM_FAILURE(rv)) {
             goto trunk_cleanup;
         }
         rv = soc_mem_field32_modify(unit, PORT_TABm, port_out,
                                     PORT_OPERATIONf, 0x0); /* L2_SVP */
         if (BCM_FAILURE(rv)) {
             goto trunk_cleanup;
         }
         local_trunk_port[num_local_trunk_ports++] = i;
     }

     return BCM_E_NONE;

 trunk_cleanup:
     for (;p >= 0; p--) {
         rv = _bcm_esw_modid_is_local(unit, mod_out, &modid_local);
         if (!modid_local) {
             continue;
         }
         i = (mod_out << num_bits_for_port) | (port_out);
         rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                 i, SOURCE_VPf, vp);
         rv = soc_mem_field32_modify(unit, PORT_TABm, port_out,
                                     PORT_OPERATIONf, 0x1);
     }
     return rv;
}

/*
 * Function:
 *      _bcm_tr_mpls_port_independent_range
 * Purpose:
 *      Check if the incoming Label is within the Port Independent Range
 * Parameters:
 *      label - Label
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_mpls_port_independent_range (int unit, bcm_mpls_label_t label, bcm_gport_t port )
{
   uint32 reg_val;         /* Register value */
   uint32 label_1_low=0, label_1_high=0;
   uint32 label_2_low=0, label_2_high=0;


    if (SOC_REG_IS_VALID(unit, GLOBAL_MPLS_RANGE_1_LOWERr)) {
        BCM_IF_ERROR_RETURN(READ_GLOBAL_MPLS_RANGE_1_LOWERr(unit, &reg_val));
        label_1_low =
            soc_reg_field_get(unit, GLOBAL_MPLS_RANGE_1_LOWERr, reg_val, LABELf);
    }

    if (SOC_REG_IS_VALID(unit, GLOBAL_MPLS_RANGE_1_UPPERr)) {
        BCM_IF_ERROR_RETURN(READ_GLOBAL_MPLS_RANGE_1_UPPERr(unit, &reg_val));
        label_1_high =
            soc_reg_field_get(unit, GLOBAL_MPLS_RANGE_1_UPPERr, reg_val, LABELf);
    }

    if (SOC_REG_IS_VALID(unit, GLOBAL_MPLS_RANGE_2_LOWERr)) {
        BCM_IF_ERROR_RETURN(READ_GLOBAL_MPLS_RANGE_2_LOWERr(unit, &reg_val));
        label_2_low =
            soc_reg_field_get(unit, GLOBAL_MPLS_RANGE_2_LOWERr, reg_val, LABELf);
    }

    if (SOC_REG_IS_VALID(unit, GLOBAL_MPLS_RANGE_2_UPPERr)) {
        BCM_IF_ERROR_RETURN(READ_GLOBAL_MPLS_RANGE_2_UPPERr(unit, &reg_val));
        label_2_high =
            soc_reg_field_get(unit, GLOBAL_MPLS_RANGE_2_UPPERr, reg_val, LABELf);
    }

    if (  (( label >= label_1_low) && ( label < label_1_high)) || 
           ((  label >= label_2_low) && ( label < label_2_high)) ){
		if (port == BCM_GPORT_INVALID) {
                   return BCM_E_NONE;
              } else {
                   return BCM_E_CONFIG;
              }
    } else {
         if (port == BCM_GPORT_INVALID) {
              return BCM_E_CONFIG;
         } else {
              return BCM_E_NONE;
         }
    }
}

/*
 * Function:
 *      _bcm_tr_mpls_match_add
 * Purpose:
 *      Assign SVP of an MPLS port
 * Parameters:
 *      unit    - (IN) Device Number
 *      mpls_port - (IN) mpls port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr_mpls_match_add(int unit, bcm_mpls_port_t *mpls_port, int vp)
{
    int rv = BCM_E_NONE, num_bits_for_port, i, gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;


    rv = _bcm_esw_gport_resolve(unit, mpls_port->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN ||
        mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN_STACKED) {

        vlan_xlate_entry_t vent;

        sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
        soc_VLAN_XLATEm_field32_set(unit, &vent, VALIDf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &vent, MPLS_ACTIONf, 0x1); /* SVP */
        soc_VLAN_XLATEm_field32_set(unit, &vent, DISABLE_VLAN_CHECKSf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &vent, SOURCE_VPf, vp);
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN) {
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf, 
                                        TR_VLXLT_HASH_KEY_TYPE_OVID);
            soc_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, 
                                        mpls_port->match_vlan);
        } else {
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf, 
                                        TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
            soc_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, 
                                        mpls_port->match_vlan);
            soc_VLAN_XLATEm_field32_set(unit, &vent, IVIDf, 
                                        mpls_port->match_inner_vlan);
        }

        if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
            soc_VLAN_XLATEm_field32_set(unit, &vent, Tf, 1);
            soc_VLAN_XLATEm_field32_set(unit, &vent, TGIDf, trunk_id);
        } else {
            soc_VLAN_XLATEm_field32_set(unit, &vent, MODULE_IDf, mod_out);
            soc_VLAN_XLATEm_field32_set(unit, &vent, PORT_NUMf, port_out);
        }
        rv = soc_mem_insert(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &vent);
        BCM_IF_ERROR_RETURN(rv);
#if 1 
        {
            int index;
            rv = soc_mem_search(unit, VLAN_XLATEm, MEM_BLOCK_ANY,
                                &index, &vent, &vent, 0);
            if (rv >= 0) {
                MATCH_TYPE(unit, vp) = _BCM_TR_MATCH_TYPE_VLAN_XLATE;
                MATCH_INDEX(unit, vp) = index;
            }
        }
#endif
    } else if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT) {
        if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
             rv = _bcm_tr_mpls_match_trunk_add(unit, trunk_id, vp);
             if (rv >= 0) {
                 MATCH_TYPE(unit, vp) = _BCM_TR_MATCH_TYPE_TRUNK;
                 MATCH_TRUNK(unit, vp) = trunk_id;
             }
             BCM_IF_ERROR_RETURN(rv);       
        } else {
        
            num_bits_for_port = 
                    _shr_popcount((unsigned int)SOC_PORT_ADDR_MAX(unit));
            
            i = (mod_out << num_bits_for_port) | port_out;
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                        i, SOURCE_VPf, vp);
            BCM_IF_ERROR_RETURN(rv);

            /* Convert system ports to physical ports */ 
            if (soc_feature(unit, soc_feature_sysport_remap)) { 
                 BCM_XLATE_SYSPORT_S2P(unit, &port_out); 
            }

            rv = soc_mem_field32_modify(unit, PORT_TABm, port_out,
                                        PORT_OPERATIONf, 0x1); /* L2_SVP */
            BCM_IF_ERROR_RETURN(rv);

            MATCH_TYPE(unit, vp) = _BCM_TR_MATCH_TYPE_PORT;
            MATCH_INDEX(unit, vp) = i;
        }
    } else if ((mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL) ||
               (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_PORT)) {

        mpls_entry_entry_t ment; 
        int num_pw_term, pw_cnt = -1;

        sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_PORT) {
            /* Check for Port_independent Label mapping */
            rv = _bcm_tr_mpls_port_independent_range (unit, mpls_port->match_label, mpls_port->port);
            if (rv < 0) {
                return rv;
            }
            if (BCM_GPORT_IS_TRUNK(mpls_port->port)) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, Tf, 1);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, TGIDf, trunk_id);
            } else {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MODULE_IDf, mod_out);
                soc_MPLS_ENTRYm_field32_set(unit, &ment, PORT_NUMf, port_out);
            }
        } else {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MODULE_IDf, 0);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, PORT_NUMf, 0);
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_LABELf, 
                                    mpls_port->match_label);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, VALIDf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, SOURCE_VPf, vp);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf, 
                                    0x1); /* L2 SVP */
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_NOT_BOSf, 
                                    0x0); /* INVALID */
        if (mpls_port->flags & BCM_MPLS_PORT_SEQUENCED) {
            /* Control word is present, check sequence number */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, CW_CHECK_CTRLf, 
                                        0x3); /* CW_CHECK_LOOSE */
            

        } else if (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD) {
            /* Control word is present, but no sequence number check */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, CW_CHECK_CTRLf, 
                                        0x1); /* CW_NO_CHECK */
        }
        if (((mpls_port->flags & BCM_MPLS_PORT_COUNTED) ||
            (mpls_port->flags & BCM_MPLS_PORT_SEQUENCED)) &&
             SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {

            num_pw_term = soc_mem_index_count(unit, ING_PW_TERM_COUNTERSm);
            for (pw_cnt = 0; pw_cnt < num_pw_term; pw_cnt++) {
                if (!_BCM_MPLS_PW_TERM_USED_GET(unit, pw_cnt)) {
                    break;
                }
            }
            if (pw_cnt == num_pw_term) {
                return BCM_E_RESOURCE;
            }
            _BCM_MPLS_PW_TERM_USED_SET(unit, pw_cnt);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, PW_TERM_NUMf, pw_cnt);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, PW_TERM_NUM_VALIDf, 1);
        }

        if (mpls_port->flags & BCM_MPLS_PORT_INT_PRI_MAP) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_PRIf,
                                        0x1); /* MAP_PRI_AND_CNG */
            /* Use specified EXP-map to determine internal prio/color */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, EXP_MAPPING_PTRf,
                mpls_port->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
        } else if ((mpls_port->flags & BCM_MPLS_PORT_INT_PRI_SET) &&
                   (mpls_port->flags & BCM_MPLS_PORT_COLOR_MAP)) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_PRIf,
                                        0x2); /* NEW_PRI_MAP_CNG */
            /* Use the specified internal priority value */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, NEW_PRIf,
                                        mpls_port->int_pri);
            /* Use specified EXP-map to determine internal color */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, EXP_MAPPING_PTRf,
                mpls_port->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
        } else if (mpls_port->flags & BCM_MPLS_PORT_INT_PRI_SET) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_PRIf, 0x2);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, TRUST_OUTER_DOT1P_PTRf,
                                        _BCM_TR_MPLS_PRI_CNG_MAP_NONE);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, NEW_PRIf,
                                        mpls_port->int_pri);
        } else {
            /* Trust the encapsulated packet's 802.1p pri/cfi, 
             * and use the incoming ports mapping.
             */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_PRIf,
                                        0x0); /* NONE */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, TRUST_OUTER_DOT1P_PTRf,
                                        _BCM_TR_MPLS_PRI_CNG_MAP_IDENTITY);
        }

        rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &ment);
        if (rv < 0) {
            if (pw_cnt != -1) {
                _BCM_MPLS_PW_TERM_USED_CLR(unit, pw_cnt);
            }
            return rv;
        }
#if 1 
        {
            int index;
            rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY,
                                &index, &ment, &ment, 0);
            if (rv >= 0) {
                MATCH_TYPE(unit, vp) = _BCM_TR_MATCH_TYPE_LABEL;
                MATCH_INDEX(unit, vp) = index;
            }
        }
#endif
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr_mpls_match_delete
 * Purpose:
 *      Remove SVP of an MPLS port
 * Parameters:
 *      unit    - (IN) Device Number
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */


STATIC int
_bcm_tr_mpls_match_delete(int unit, int vp)
{
    ing_pw_term_counters_entry_t pw_cnt_entry;
    int source_vp, port, rv;
    bcm_trunk_t trunk_id;

    if (MATCH_TYPE(unit, vp) == _BCM_TR_MATCH_TYPE_VLAN_XLATE) {
        vlan_xlate_entry_t vent;

        
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, VLAN_XLATEm, MEM_BLOCK_ANY, 
                                          MATCH_INDEX(unit, vp), &vent));

        source_vp = soc_VLAN_XLATEm_field32_get(unit, &vent, SOURCE_VPf);
        if ((soc_VLAN_XLATEm_field32_get(unit, &vent, VALIDf) == 0) ||
            (source_vp != vp)) {
            return BCM_E_NOT_FOUND;
        }
        return soc_mem_delete(unit, VLAN_XLATEm, MEM_BLOCK_ANY, &vent);
    } else if (MATCH_TYPE(unit, vp) == _BCM_TR_MATCH_TYPE_PORT) {
         rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                  MATCH_INDEX(unit, vp), SOURCE_VPf, 0);
         BCM_IF_ERROR_RETURN(rv);

         port = MATCH_INDEX(unit, vp) & SOC_PORT_ADDR_MAX(unit);
         rv = soc_mem_field32_modify(unit, PORT_TABm, port,
                                       PORT_OPERATIONf, 0x0); /* NORMAL */
         BCM_IF_ERROR_RETURN(rv);
    }else if (MATCH_TYPE(unit, vp) == _BCM_TR_MATCH_TYPE_TRUNK) {
         trunk_id = MATCH_TRUNK(unit, vp);
         rv = _bcm_tr_mpls_match_trunk_delete(unit, trunk_id, vp);
         BCM_IF_ERROR_RETURN(rv);
    }else if (MATCH_TYPE(unit, vp) == _BCM_TR_MATCH_TYPE_LABEL) {
        mpls_entry_entry_t ment; 
        int pw_cnt;

        
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, 
                                          MATCH_INDEX(unit, vp), &ment));

        source_vp = soc_MPLS_ENTRYm_field32_get(unit, &ment, SOURCE_VPf);
        if ((soc_MPLS_ENTRYm_field32_get(unit, &ment, VALIDf) == 0) ||
            (source_vp != vp)) {
            return BCM_E_NOT_FOUND;
        }
        if (soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUM_VALIDf)) {
            pw_cnt = soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUMf);
            sal_memset(&pw_cnt_entry, 0, sizeof(ing_pw_term_counters_entry_t));
            (void) WRITE_ING_PW_TERM_COUNTERSm(unit, MEM_BLOCK_ALL, pw_cnt,
                                               &pw_cnt_entry);
            _BCM_MPLS_PW_TERM_USED_CLR(unit, pw_cnt);
        }
        return soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &ment);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_mpls_match_get
 * Purpose:
 *      Obtain match information of an MPLS port
 * Parameters:
 *      unit    - (IN) Device Number
 *      mpls_port - (OUT) mpls port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */


STATIC int
_bcm_tr_mpls_match_get(int unit, bcm_mpls_port_t *mpls_port, int vp)
{
    int num_bits_for_port, rv = BCM_E_NONE;
    uint32 port_mask;
    bcm_module_t mod_in, mod_out;
    bcm_port_t port_in, port_out;
    bcm_trunk_t trunk_id;
    int source_vp;

    if (MATCH_TYPE(unit, vp) == _BCM_TR_MATCH_TYPE_VLAN_XLATE) {
        vlan_xlate_entry_t vent;

        
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, VLAN_XLATEm, MEM_BLOCK_ANY,
                                          MATCH_INDEX(unit, vp), &vent));
        
        source_vp = soc_VLAN_XLATEm_field32_get(unit, &vent, SOURCE_VPf);
        if ((soc_VLAN_XLATEm_field32_get(unit, &vent, VALIDf) == 0) ||
            (source_vp != vp)) {
            return BCM_E_NOT_FOUND;
        }
        if (soc_VLAN_XLATEm_field32_get(unit, &vent, KEY_TYPEf) ==
            TR_VLXLT_HASH_KEY_TYPE_OVID) {
            mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT_VLAN;
            mpls_port->match_vlan = soc_VLAN_XLATEm_field32_get(unit, &vent, OVIDf);
        } else if (soc_VLAN_XLATEm_field32_get(unit, &vent, KEY_TYPEf) ==
                   TR_VLXLT_HASH_KEY_TYPE_IVID_OVID) {
            mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT_VLAN_STACKED;
            mpls_port->match_vlan = soc_VLAN_XLATEm_field32_get(unit, &vent, OVIDf);
            mpls_port->match_inner_vlan = 
                soc_VLAN_XLATEm_field32_get(unit, &vent, IVIDf);
        } else {
            return BCM_E_INTERNAL;
        }
        if (soc_VLAN_XLATEm_field32_get(unit, &vent, Tf) == 0x1) {
            trunk_id = soc_VLAN_XLATEm_field32_get(unit, &vent, TGIDf);
            BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
        } else {
            mod_in = soc_VLAN_XLATEm_field32_get(unit, &vent, MODULE_IDf);
            port_in = soc_VLAN_XLATEm_field32_get(unit, &vent, PORT_NUMf);

            rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
        }
    } else if (MATCH_TYPE(unit, vp) == _BCM_TR_MATCH_TYPE_PORT) {
         num_bits_for_port =
              _shr_popcount((unsigned int)SOC_PORT_ADDR_MAX(unit));
         port_mask = (1 << num_bits_for_port) - 1;
         mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT;
         port_in = MATCH_INDEX(unit, vp) & port_mask;
         mod_in = MATCH_INDEX(unit, vp) >> num_bits_for_port;

         rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                               mod_in, port_in, &mod_out, &port_out);
         BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
    }else if (MATCH_TYPE(unit, vp) == _BCM_TR_MATCH_TYPE_TRUNK) {
         trunk_id = MATCH_TRUNK(unit, vp);
         mpls_port->criteria = BCM_MPLS_PORT_MATCH_PORT;
         BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
    }	else if (MATCH_TYPE(unit, vp) == _BCM_TR_MATCH_TYPE_LABEL) {
        mpls_entry_entry_t ment;

        
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, MPLS_ENTRYm, MEM_BLOCK_ANY,
                                          MATCH_INDEX(unit, vp), &ment));

        source_vp = soc_MPLS_ENTRYm_field32_get(unit, &ment, SOURCE_VPf);
        if ((soc_MPLS_ENTRYm_field32_get(unit, &ment, VALIDf) == 0) ||
            (source_vp != vp)) {
            return BCM_E_NOT_FOUND;
        }
        if (soc_MPLS_ENTRYm_field32_get(unit, &ment, PORT_NUMf) == 0x3f) {
            mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL;
            mpls_port->port = BCM_GPORT_INVALID;
        } else {
            mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL_PORT;
            if (soc_MPLS_ENTRYm_field32_get(unit, &ment, Tf) == 0x1) {
                trunk_id = soc_MPLS_ENTRYm_field32_get(unit, &ment, TGIDf);
                BCM_GPORT_TRUNK_SET(mpls_port->port, trunk_id);
            } else {
                mod_in = soc_MPLS_ENTRYm_field32_get(unit, &ment, MODULE_IDf);
                port_in = soc_MPLS_ENTRYm_field32_get(unit, &ment, PORT_NUMf);

                rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                            mod_in, port_in, &mod_out, &port_out);
                BCM_GPORT_MODPORT_SET(mpls_port->port, mod_out, port_out);
            }
        }
        mpls_port->match_label = soc_MPLS_ENTRYm_field32_get(unit, &ment, MPLS_LABELf);
        if (soc_MPLS_ENTRYm_field32_get(unit, &ment, CW_CHECK_CTRLf) == 0x3) {
            
            /* Control word is present, and check sequence number */
            mpls_port->flags |= BCM_MPLS_PORT_SEQUENCED;
        } else if (soc_MPLS_ENTRYm_field32_get(unit, &ment, 
                                               CW_CHECK_CTRLf) == 0x1) {
            /* Control word is present, but no sequence number check */
            mpls_port->flags |= BCM_MPLS_PORT_CONTROL_WORD;
        }

        if (soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUM_VALIDf)) {
            mpls_port->flags |= BCM_MPLS_PORT_COUNTED;
        }

        if (soc_MPLS_ENTRYm_field32_get(unit, &ment, 
                                        PW_TERM_NUM_VALIDf) == 0x1) {
            /* MAP_PRI_AND_CNG */
            /* Use specified EXP-map to determine internal prio/color */
            mpls_port->exp_map = 
                soc_MPLS_ENTRYm_field32_get(unit, &ment, EXP_MAPPING_PTRf);
            mpls_port->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
            mpls_port->flags |= BCM_MPLS_PORT_INT_PRI_MAP;
        } else if (soc_MPLS_ENTRYm_field32_get(unit, &ment, 
                                               PW_TERM_NUM_VALIDf) == 0x2) {
            /* NEW_PRI_MAP_CNG */
            /* Use the specified internal priority value */
            mpls_port->int_pri = 
                soc_MPLS_ENTRYm_field32_get(unit, &ment, NEW_PRIf);
            mpls_port->flags |= BCM_MPLS_PORT_INT_PRI_SET;

            /* Use specified EXP-map to determine internal color */
            mpls_port->exp_map = 
                soc_MPLS_ENTRYm_field32_get(unit, &ment, EXP_MAPPING_PTRf);
            mpls_port->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
            mpls_port->flags |= BCM_MPLS_PORT_COLOR_MAP;
        } else if (soc_MPLS_ENTRYm_field32_get(unit, &ment, 
                                               TRUST_OUTER_DOT1P_PTRf) == 
                                               _BCM_TR_MPLS_PRI_CNG_MAP_NONE) {
            /*
             * Don't trust the encapsulated packet's 802.1p pri/cfi.
             * Use a fixed value for internal priority.
             */
            mpls_port->int_pri = 
                soc_MPLS_ENTRYm_field32_get(unit, &ment, NEW_PRIf);
            mpls_port->flags |= BCM_MPLS_PORT_INT_PRI_SET;
        }
    } else {
        mpls_port->criteria = BCM_MPLS_PORT_MATCH_NONE;
    }
    return rv;
}

STATIC int
_bcm_tr_mpls_nh_drop(int unit, int vp, int drop) 
{
    int nh_index;
    ing_dvp_table_entry_t dvp;

    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
    return soc_mem_field32_modify(unit, ING_L3_NEXT_HOPm, nh_index, DROPf, drop);
}

/*
 * Function:
 *      _bcm_tr_mpls_broadcast_replication_list_add
 * Purpose:
 *      Add an mpls port to a VFI replication list
 * Parameters:
 *      unit    - (IN) Device Number
 *      mpls_port - (IN/OUT) mpls port information (OUT : mpls_port_id)
 *      vfi     - (IN) VFI
 *      mc_group - (IN/OUT) Multicast_group
 *      nh_index  - (IN) Next Hop Index
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr_mpls_broadcast_replication_list_add (int unit, bcm_port_t local_port, int vfi,  bcm_multicast_t *mc_group, int nh_index )
{
    int rv=0, ipmc_id;
    vfi_entry_t vfi_entry;
    bcm_gport_t local_gport;

    /* Get IPMC index from HW */
    rv = READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry);
    if (rv < 0) {
         return rv;
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
         ipmc_id = soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf);
    } else
#endif
    {
         ipmc_id = soc_VFIm_field32_get(unit, &vfi_entry, L3MC_INDEXf);
    }
    _BCM_MULTICAST_GROUP_SET(*mc_group, _BCM_MULTICAST_TYPE_VPLS, ipmc_id);

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {

        if (BCM_GPORT_IS_TRUNK(local_port)) {
           local_gport = local_port;
        } else {
             /* Convert system local_port to physical local_port */ 
             if (soc_feature(unit, soc_feature_sysport_remap)) { 
                  BCM_XLATE_SYSPORT_S2P(unit, &local_port); 
             }
             (void) bcm_esw_port_gport_get(unit, local_port, &local_gport);
         }
         rv = bcm_tr2_multicast_egress_add(unit, *mc_group, 
                                      local_gport, nh_index 
                                      + BCM_XGS3_DVP_EGRESS_IDX_MIN);
    } else
#endif
    {        
         if (BCM_GPORT_IS_TRUNK(local_port)) {
              local_gport = local_port;
         } else {
              (void) bcm_esw_port_gport_get(unit, local_port, &local_gport);
         }
         rv = bcm_tr_multicast_egress_add(unit, *mc_group, 
                                       local_gport, nh_index);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr_mpls_broadcast_replication_list_remove
 * Purpose:
 *      Remove port from VFI replication list
 * Parameters:
 *      unit    - (IN) Device Number
 *      local_port     - (IN) Local_Port
 *      mc_group - (IN) Multicast_group
 *      nh_index  - (IN) Next Hop Index
 * Returns:
 *      BCM_E_XXX
 */

STATIC void
_bcm_tr_mpls_broadcast_replication_list_remove (int unit, bcm_port_t local_port, bcm_multicast_t mc_group, int nh_index )
{
    bcm_gport_t local_gport;
    
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {

        if (BCM_GPORT_IS_TRUNK(local_port)) {
           local_gport = local_port;
        } else {
           /* Convert system local_port to physical local_port */ 
           if (soc_feature(unit, soc_feature_sysport_remap)) { 
               BCM_XLATE_SYSPORT_S2P(unit, &local_port); 
           }
           (void) bcm_esw_port_gport_get(unit, local_port, &local_gport);
         }
         (void) bcm_tr2_multicast_egress_delete  (unit, mc_group, local_gport, nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN);
    } else
#endif
    {
       if (BCM_GPORT_IS_TRUNK(local_port)) {
           local_gport = local_port;
       } else {
         (void) bcm_esw_port_gport_get(unit, local_port, &local_gport);
       }
         (void) bcm_tr_multicast_egress_delete (unit, mc_group, local_gport, nh_index);
    }
}

/*
 * Function:
 *      _bcm_tr_mpls_vpws_port_add
 * Purpose:
 *      Add VPWS port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      mpls_port - (IN/OUT) mpls port information (OUT : mpls_port_id)
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr_mpls_vpws_port_add(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
        int active_vp = 0, vp1=0, vp2=0;
        source_vp_entry_t svp1, svp2;
        ing_dvp_table_entry_t dvp1, dvp2;
        uint8 vp_valid_flag = 0;
        bcm_port_t local_port;
        int old_tpid_enable = 0, tpid_enable = 0, tpid_index;
        int sdk_drop, customer_drop=0, is_local = 0, rv = BCM_E_PARAM, nh_index = 0; 
        int     vp3=0, num_vp;
        source_vp_entry_t  svp3;
        ing_dvp_table_entry_t     dvp3;
        ing_l3_next_hop_entry_t ing_nh;

        if ( vpn != BCM_MPLS_VPN_INVALID) {
            vp1 = _BCM_MPLS_VPN_ID_GET(vpn);
            vp2 = vp1 + 1;
		

            /* ---- Read in current table values for VP1 and VP2 ----- */
            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp1, &svp1);
            if (rv < 0) {
                return rv;
            }
            rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp1, &dvp1);
            if (rv < 0) {
                return rv;
            }
            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp2, &svp2);
            if (rv < 0) {
                return rv;
            }
            rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp2, &dvp2);
            if (rv < 0) {
                return rv;
            }

            if ((mpls_port->flags & BCM_MPLS_PORT_REPLACE) || (mpls_port->flags & BCM_MPLS_PORT_WITH_ID)) {
                active_vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
                /* Read in current table values for VP3 */
                if ((active_vp != vp1) && (active_vp != vp2)) {
                    vp3 = active_vp;
                    rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp3, &dvp3);
                    if (rv < 0) {
                        return rv;
                    }
                    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp3, NEXT_HOP_INDEXf);
                    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_index, &ing_nh));
                    if (soc_SOURCE_VPm_field32_get(unit, &svp3, ENTRY_TYPEf) != 0) {
                        vp_valid_flag |= 0x4;
                    } else if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, DROPf) == 0x1) {
                        vp_valid_flag |= 0x20;
                    }
                }
            }

            /* ---- See if the entry already exists - Set Valid flag --- */
            if (soc_SOURCE_VPm_field32_get(unit, &svp1, ENTRY_TYPEf) != 0) {
                vp_valid_flag |= 0x1;
            } else {
                nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp1, NEXT_HOP_INDEXf);
                BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_index, &ing_nh));
                if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, DROPf) == 0x1) {
                    vp_valid_flag |= 0x8;
                }
            }
            if (soc_SOURCE_VPm_field32_get(unit, &svp2, ENTRY_TYPEf) != 0) {
                vp_valid_flag |= 0x2;        /* -- VP2 Valid ----- */
            } else {
                nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp2, NEXT_HOP_INDEXf);
                BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_index, &ing_nh));
                if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, DROPf) == 0x1) {
                    vp_valid_flag |= 0x10;
                }
            }

           customer_drop = (mpls_port->flags & BCM_MPLS_PORT_DROP) ? 1:0;
        }else if (vpn == BCM_MPLS_VPN_INVALID) {

              if (mpls_port->flags & BCM_MPLS_PORT_WITH_ID) {
                   active_vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
                   vp_valid_flag = 0xFF;			   
 		     rv = soc_mem_field32_modify(unit, SOURCE_VPm, active_vp, ENTRY_TYPEf, 0x0);			 
                   if (rv >= 0) {
                        /* Set the DROP bit in the destination next-hop */
                        rv = _bcm_tr_mpls_nh_drop(unit, active_vp, 0x1);
                   }
                   return rv;
              } else {
                   /* VP3 Add */
                    vp_valid_flag = 0x4;
              }

        }
		
        switch (vp_valid_flag) {

              case 0x0: /* No VP is valid */
                             if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                                  return BCM_E_NOT_FOUND;
                             }
                             /* No entries are used, let's use VP1. */
                             active_vp = vp1;
                             vp_valid_flag = 1;
                             sal_memset(&svp1, 0, sizeof(source_vp_entry_t)); 		  
                             sal_memset(&svp2, 0, sizeof(source_vp_entry_t));
                             sal_memset(&dvp1, 0, sizeof(ing_dvp_table_entry_t));   
                             break;


              			
		case 0x1:    /* Only VP1 is valid */	
                             if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                                  if (active_vp != vp1) {
                                       return BCM_E_NOT_FOUND;
                                  }
                                  nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp1, 
                                                                         NEXT_HOP_INDEXf);
                                  if (soc_SOURCE_VPm_field32_get(unit, &svp1, SD_TAG_MODEf)) {
                                          /* SD-tag mode, save the old TPID enable bits */
                                         old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp1, 
                                                             TPID_ENABLEf);
                                  }
                             } else {
                                  /* Not replacing VP1, let's use the unused VP2 */
                                  active_vp = vp2;
                                  vp_valid_flag = 3;

                                   /* SDK cannot honor the Drop-Setting for VP2 */
                                  /* Obtain the Customer-Drop-Setting of VP1 from SOURCE_VP_2.CLASS_IDf */
                                  customer_drop = soc_SOURCE_VPm_field32_get(unit, &svp2, CLASS_IDf);
                                  sal_memset(&dvp2, 0, sizeof(ing_dvp_table_entry_t));
                             }
                             break;



		case 0x2: /* Only VP2 is valid */
                             if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                                  if (active_vp != vp2) {
                                       return BCM_E_NOT_FOUND;
                                  }
                                  nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp2,
                                                                          NEXT_HOP_INDEXf);
                                  if (soc_SOURCE_VPm_field32_get(unit, &svp2, SD_TAG_MODEf)) {
                                       /* SD-tag mode, save the old TPID enable bits */
                                       old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp2, 
                                                                          TPID_ENABLEf);
                                  }
                             } else {
                                  /* Not replacing VP2, let's use the unused VP1 */
                                  active_vp = vp1;
                                  vp_valid_flag = 3;
                                   sal_memset(&svp1, 0, sizeof(source_vp_entry_t));
                                   sal_memset(&dvp1, 0, sizeof(ing_dvp_table_entry_t));
                             }
                             break;


              
		case 0x3: /* VP1 and VP2 are valid */
#if defined(BCM_TRIUMPH2_SUPPORT)
                             if (soc_feature(unit, soc_feature_mpls_failover)) {
                                  _BCM_MPLS_FAILOVER_VALID_RANGE ( mpls_port->failover_id ) {
                                       /* allocate a new VP index */
                                       num_vp = soc_mem_index_count(unit, SOURCE_VPm);
                                       rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, _bcmVpTypeMpls, &vp3);
                                       if (rv < 0) {
                                            return rv;
                                       }
                                       sal_memset(&svp3, 0, sizeof(source_vp_entry_t));
                                       sal_memset(&dvp3, 0, sizeof(ing_dvp_table_entry_t));
                                       active_vp = vp3;
                                  }
                             } else 
#endif /* BCM_TRIUMPH2_SUPPORT  */
                             {
                                  if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
                                       return BCM_E_FULL;
                                  }
                             }
							 
                             if (active_vp == vp1) {
                                  /* Replacing VP1 */
                                       nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp1,
                                                                                         NEXT_HOP_INDEXf);
                                       if (soc_SOURCE_VPm_field32_get(unit, &svp1, SD_TAG_MODEf)) {
                                            /* SD-tag mode, save the old TPID enable bits */
                                            old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp1, 
                                                                                         TPID_ENABLEf);
                                       }
                             }

                             else if (active_vp == vp2) {
                                  /* Replacing VP2 */
                                  nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp2,
                                                                                         NEXT_HOP_INDEXf);
                                  if (soc_SOURCE_VPm_field32_get(unit, &svp2, SD_TAG_MODEf)) {
                                       /* SD-tag mode, save the old TPID enable bits */
                                       old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp2, 
                                                                                         TPID_ENABLEf);
                                  }
                             }

                             else if (active_vp ==	vp3) {
                                  nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp3,
                                                                                         NEXT_HOP_INDEXf);
                                  if (soc_SOURCE_VPm_field32_get(unit, &svp2, SD_TAG_MODEf)) {
                                       /* SD-tag mode, save the old TPID enable bits */
                                       old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp3, 
                                                                                         TPID_ENABLEf);
                                  }
                             }
                             break;


              
		case 0x4: /* Only VP3 is valid - For Triumph SW based Failover purposes */
                            if (!soc_feature(unit, soc_feature_mpls_failover)) {
                                  /* allocate a new VP index */
                                  num_vp = soc_mem_index_count(unit, SOURCE_VPm);
                                  rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, _bcmVpTypeMpls, &vp3);
                                  if (rv < 0) {
                                       return rv;
                                  }
                                  sal_memset(&svp3, 0, sizeof(source_vp_entry_t));
                                  sal_memset(&dvp3, 0, sizeof(ing_dvp_table_entry_t));
                                  active_vp = vp3;
                             }
                             break;

              case 0x11:
              case 0x31:
                             break;

		default:
                             return BCM_E_CONFIG;
       }


	/* Set the MPLS port ID */
	 BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id, active_vp);
	 mpls_port->encap_id = 0;

        switch (vp_valid_flag) {

              case 0x11:
              case 0x31:
                             break;

              default:
                            /* If (vp_valid_flag != 3), set the "DROP" bit in next hop entry */
                            sdk_drop = ((vp_valid_flag != 3) || customer_drop) ? 1 : 0;
                            rv = _bcm_tr_mpls_l2_nh_info_add(unit, mpls_port, active_vp, sdk_drop, 1,
                                         &nh_index, &local_port, &is_local);
                            if (rv < 0) {
                                return rv;
                            }

                            if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_TAGGED) {
                                    rv = _bcm_fb2_outer_tpid_entry_add(unit, mpls_port->service_tpid, 
                                                                   &tpid_index);
                                    if (rv < 0) {
                                        goto vpws_cleanup;
                                    }
                                    tpid_enable = (1 << tpid_index);
                            }
                            break;
        }


              if (active_vp == vp1) {
                        /* Set SOURCE_VP */
                        soc_SOURCE_VPm_field32_set(unit, &svp1, CLASS_IDf,
                                                                                    mpls_port->if_class);
                        if (customer_drop) {
                             soc_SOURCE_VPm_field32_set(unit, &svp2, CLASS_IDf, 0x1);
                        } else {
                             soc_SOURCE_VPm_field32_set(unit, &svp2, CLASS_IDf, 0x0);
                        }
                        soc_SOURCE_VPm_field32_set(unit, &svp1, NETWORK_PORTf,
                                            (mpls_port->flags & BCM_MPLS_PORT_NETWORK) ? 1 : 0);
                        if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_TAGGED) {
                             soc_SOURCE_VPm_field32_set(unit, &svp1, SD_TAG_MODEf, 1);
                             soc_SOURCE_VPm_field32_set(unit, &svp1, TPID_ENABLEf, tpid_enable);
                        } else {
                             soc_SOURCE_VPm_field32_set(unit, &svp1, SD_TAG_MODEf, 0);
                        }

                        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                             rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp1, &svp1);
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }
                        } else {
							 
                             /* Link in the newly allocated next-hop entry to DVP */
                             soc_ING_DVP_TABLEm_field32_set(unit, &dvp1, NEXT_HOP_INDEXf,
                                                                                         nh_index);
                             rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, active_vp, &dvp1);
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }
                             if (vpn == BCM_MPLS_VPN_INVALID) {
                                  /* Set Drop */
                                  rv = soc_mem_field32_modify(unit, ING_L3_NEXT_HOPm, nh_index, DROPf, 0x1);
                             }
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }

                             /* Tie the SVP to DVP. If only one port is valid, point to itself */
                             soc_SOURCE_VPm_field32_set(unit, &svp1, 
                                                                               ENTRY_TYPEf, 0x2); /* VPWS */
                             soc_SOURCE_VPm_field32_set(unit, &svp1, DVPf,
                                                                               (vp_valid_flag == 3) ? vp2 : vp1);

                             rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp1, &svp1);
                             if ((rv >= 0) && (vp_valid_flag == 3)) {
                                  soc_SOURCE_VPm_field32_set(unit, &svp2, DVPf, vp1);
                                  rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp2, &svp2);
                                  if (rv >= 0) {
                                       /* Clear the DROP bit in the destination next-hop */
                                       rv = _bcm_tr_mpls_nh_drop(unit, vp2, 0);
                                  }
                             } else if ((rv >= 0) && (vp_valid_flag == 1)) {
                                  rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp2, &svp2);
                             }
                        }
              }



              else if (active_vp == vp2) {
                        /* Set SOURCE_VP */
                        soc_SOURCE_VPm_field32_set(unit, &svp2, CLASS_IDf,
                             mpls_port->if_class);
                        soc_SOURCE_VPm_field32_set(unit, &svp2, NETWORK_PORTf,
                             (mpls_port->flags & BCM_MPLS_PORT_NETWORK) ? 1 : 0);
                        if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_TAGGED) {
                             soc_SOURCE_VPm_field32_set(unit, &svp2, SD_TAG_MODEf, 1);
                              soc_SOURCE_VPm_field32_set(unit, &svp2, TPID_ENABLEf, tpid_enable);
                        } else {
                             soc_SOURCE_VPm_field32_set(unit, &svp2, SD_TAG_MODEf, 0);
                        }

                        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                             rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp2, &svp2);
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }
                        } else {

                             /* Link in the newly allocated next-hop entry to DVP */
                             soc_ING_DVP_TABLEm_field32_set(unit, &dvp2, NEXT_HOP_INDEXf,
                                                                                           nh_index);
                             rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, active_vp, &dvp2);
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }

                             if (vpn == BCM_MPLS_VPN_INVALID) {
                                  /* Set Drop */
                                  rv = soc_mem_field32_modify(unit, ING_L3_NEXT_HOPm, nh_index, DROPf, 0x1);
                             } else {
                                 /* Reset Drop */
                                 rv = soc_mem_field32_modify(unit, ING_L3_NEXT_HOPm, nh_index, DROPf, 0x0);
                             }
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }

                             /* Tie the SVP to DVP. If only one port is valid, point to itself */
                             soc_SOURCE_VPm_field32_set(unit, &svp2, 
                                                                               ENTRY_TYPEf, 0x2); /* VPWS */
                             soc_SOURCE_VPm_field32_set(unit, &svp2, DVPf,
                                                                               ((vp_valid_flag == 0x3) || (vp_valid_flag == 0x11)) ? vp1 : vp2);
				  
                             rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, &svp2);
                             if ((rv >= 0) && ((vp_valid_flag == 0x3) || (vp_valid_flag == 0x11))) {
                                  soc_SOURCE_VPm_field32_set(unit, &svp1, DVPf, vp2);
                                  rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp1, &svp1);
                                  if (rv >= 0) {
                                       /* Clear the DROP bit in the destination next-hop */
                                       if (customer_drop == 0) {
                                            rv = _bcm_tr_mpls_nh_drop(unit, vp1, 0);
                                       }
                                  }
                             }
                        }
              }



              else if (active_vp == vp3) {
                        /* Set SOURCE_VP */
                        soc_SOURCE_VPm_field32_set(unit, &svp3, CLASS_IDf,
                                                           mpls_port->if_class);
                        soc_SOURCE_VPm_field32_set(unit, &svp3, NETWORK_PORTf,
                                                           (mpls_port->flags & BCM_MPLS_PORT_NETWORK) ? 1 : 0);
                        if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_TAGGED) {
                             soc_SOURCE_VPm_field32_set(unit, &svp3, SD_TAG_MODEf, 1);
                             soc_SOURCE_VPm_field32_set(unit, &svp3, TPID_ENABLEf, tpid_enable);
                        } else {
                             soc_SOURCE_VPm_field32_set(unit, &svp3, SD_TAG_MODEf, 0);
                        }
                        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
                             rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp3, &svp3);
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }
                        } else {

				  /* Link in the newly allocated next-hop entry to DVP */
#if defined(BCM_TRIUMPH2_SUPPORT)
                             if (soc_feature(unit, soc_feature_mpls_failover)){
                                  soc_ING_DVP_TABLEm_field32_set(unit, &dvp3, NEXT_HOP_INDEXf,
                                                                                         nh_index);
                                  rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, active_vp, &dvp3);
                             }
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }
#endif /* BCM_TRIUMPH2_SUPPORT  */		
                             if (!soc_feature(unit, soc_feature_mpls_failover)){
                                  soc_ING_DVP_TABLEm_field32_set(unit, &dvp3, NEXT_HOP_INDEXf, nh_index);
                                  rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, active_vp, &dvp3);
                                  if (rv < 0) {
                                       goto vpws_cleanup;
                                  }
                                  if (vpn == BCM_MPLS_VPN_INVALID) {
                                      /* Set Drop */
                                      rv = soc_mem_field32_modify(unit, ING_L3_NEXT_HOPm, nh_index, DROPf, 0x1);
                                  } else {
                                      /* Reset Drop */
                                      rv = soc_mem_field32_modify(unit, ING_L3_NEXT_HOPm, nh_index, DROPf, 0x0);
                                  }
                             }
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }


				  /* Tie the SVP to DVP. If only one port is valid, point to itself */
#if defined(BCM_TRIUMPH2_SUPPORT)
                             if (soc_feature(unit, soc_feature_mpls_failover)){
                                  soc_SOURCE_VPm_field32_set(unit, &svp3, 
                                                                ENTRY_TYPEf, 0x2); /* VPWS */
                                  soc_SOURCE_VPm_field32_set(unit, &svp3, DVPf,
                                                                (vp_valid_flag == 0x3) ? vp1 : vp3);


                                  rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, &svp3);
                                  if ((rv >= 0) && (vp_valid_flag == 3)) {
                                       soc_SOURCE_VPm_field32_set(unit, &svp1, DVPf, vp3);
                                       rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp1, &svp1);
                                       if (rv >= 0) {
                                          /* Clear the DROP bit in the destination next-hop */
                                          rv = _bcm_tr_mpls_nh_drop(unit, vp1, 0);
                                       }
                                  }
                             }
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }
#endif /* BCM_TRIUMPH2_SUPPORT  */

                             if (!soc_feature(unit, soc_feature_mpls_failover)){
                                  if (vpn == BCM_MPLS_VPN_INVALID) {
                                       soc_SOURCE_VPm_field32_set(unit, &svp3, ENTRY_TYPEf, 0x0); /* INVALID */
                                       soc_SOURCE_VPm_field32_set(unit, &svp3, DVPf, vp3);
                                  } else {
                                       soc_SOURCE_VPm_field32_set(unit, &svp3, 
                                                                ENTRY_TYPEf, 0x2); /* VPWS */
                                       soc_SOURCE_VPm_field32_set(unit, &svp3, DVPf,
                                                                (vp_valid_flag == 0x31) ? vp1 : vp3);
                                  }			  

                                  rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, active_vp, &svp3);
                                  if ((rv >= 0) && (vp_valid_flag == 0x31)) {
                                       soc_SOURCE_VPm_field32_set(unit, &svp1, DVPf, vp3);
                                       rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp1, &svp1);
                                  }
                             }    
                             if (rv < 0) {
                                  goto vpws_cleanup;
                             }
                        }
              }
			  
              if (mpls_port->flags & BCM_MPLS_PORT_WITH_ID) {
                  goto vpws_cleanup;
              }

         if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {

#if defined(BCM_TRIUMPH2_SUPPORT)
            if (soc_feature(unit, soc_feature_mpls_failover)) {
                   int failover_vp = 0;
                   bcm_mpls_port_t	failover_mpls_port;
			
                   _BCM_MPLS_FAILOVER_VALID_RANGE ( mpls_port->failover_id ) {
                        failover_vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->failover_port_id);
                        if (!_bcm_vp_used_get(unit, failover_vp, _bcmVpTypeMpls)) {
                             rv = BCM_E_NOT_FOUND;
                        }
                        rv = _bcm_tr_mpls_match_get(unit,  &failover_mpls_port, failover_vp);
                        if ( rv < 0 ) {
                             if (rv != BCM_E_NOT_FOUND) {
                                  goto vpws_cleanup;
                             }
                        }
                        if (mpls_port->match_label == failover_mpls_port.match_label) {
                             /* Replace failover_vp with Primary vp */
                             rv = _bcm_tr_mpls_match_delete(unit, failover_vp);
                        }
                   }
              }
#endif /* BCM_TRIUMPH2_SUPPORT  */

            /*
             * Match entries cannot be replaced, instead, callers
             * need to delete the existing entry and re-add with the
             * new match parameters.
             */
            rv = _bcm_tr_mpls_match_add(unit, mpls_port, active_vp);

            /* Set the MPLS port ID */
            BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id, active_vp);
            mpls_port->encap_id = 0;
         }

vpws_cleanup:
        if (rv < 0) {
            if (tpid_enable) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            }
            if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
                (void) _bcm_tr_mpls_l2_nh_info_delete(unit, nh_index);
            }
            return rv;
        }
	return rv;
}

/*
* Function:
*		_bcm_tr_mpls_vpls_port_add
* Purpose:
*		Add VPLS port to a VPN
* Parameters:
*		unit	- (IN) Device Number
*		vpn 	- (IN) VPN instance ID
*		mpls_port - (IN/OUT) mpls port information (OUT : mpls_port_id)
* Returns:
*		BCM_E_XXX
*/
		
STATIC int
_bcm_tr_mpls_vpls_port_add(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
        int vp, num_vp, vfi;
        source_vp_entry_t svp;
        ing_dvp_table_entry_t dvp;
        ing_l3_next_hop_entry_t ing_nh;
        bcm_multicast_t mc_group = 0;
        int modid_local=0, chip_drop_bit=0, module_id, is_trunk=0;
        bcm_port_t local_port;
        int old_tpid_enable = 0, tpid_enable = 0, tpid_index;
        int drop, i, is_local = 0, rv = BCM_E_PARAM, nh_index = 0; 

        if ( vpn != BCM_MPLS_VPN_INVALID) {
              vfi = _BCM_MPLS_VPN_ID_GET(vpn);
              if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
                  return BCM_E_NOT_FOUND;
              }
        } else {
              vfi = 0x3FF;
        }

        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
            vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                return BCM_E_NOT_FOUND;
            }
            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
            if (rv < 0) {
                return rv;
            }
            rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
            if (rv < 0) {
                return rv;
            }
            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, 
                                                      NEXT_HOP_INDEXf);

            /* Get Drop-bit from Port Update */
            drop = (mpls_port->flags & BCM_MPLS_PORT_DROP) ? 1 : 0;

            /* Read Drop-bit of Associated ING-Nhop */
            BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_index, &ing_nh));
            chip_drop_bit = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf);
            module_id = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, MODULE_IDf);			
            is_trunk = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, Tf);
            if (!is_trunk) {
                BCM_IF_ERROR_RETURN( _bcm_esw_modid_is_local(unit, module_id, &modid_local));
            }
            if (modid_local) {
                 local_port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, PORT_NUMf);
            }	 else if (is_trunk) {
                 local_port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, TGIDf);
            }

            /* DeActivate Drop-bit */
            if ( !drop &&  chip_drop_bit ) {
                   soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
                   rv = soc_mem_write (unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ALL, nh_index, &ing_nh);
                   if (modid_local || is_trunk) {
                        rv = _bcm_tr_mpls_broadcast_replication_list_add (unit, local_port, vfi, &mc_group, nh_index );  
                        if (rv < 0) {
                             return rv;
                        }
                   }
            }


            /* Activate Drop-bit */
           if ( drop &&  !chip_drop_bit ) {
                   soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
                   rv = soc_mem_write (unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ALL, nh_index, &ing_nh);
                   if (modid_local || is_trunk) {
                        (void) _bcm_tr_mpls_broadcast_replication_list_remove (unit, local_port, mc_group, nh_index );
                   }
           }

 
            if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
                /* SD-tag mode, save the old TPID enable bits */
                old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp,
                                                             TPID_ENABLEf);
            }
        } else if (mpls_port->flags & BCM_MPLS_PORT_WITH_ID ) {
              if (!BCM_GPORT_IS_MPLS_PORT(mpls_port->mpls_port_id)) {
                   return (BCM_E_BADID);
              }

              num_vp = soc_mem_index_count(unit, SOURCE_VPm);
              vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
              if (vp >= num_vp) {
                   return (BCM_E_BADID);
              }

    /* ------------  SW Failover - Applicable for TRIUMPH/VL only ---- */
    if (!soc_feature(unit, soc_feature_mpls_failover)) {
         int ipmc_id=0;			 
         vfi_entry_t vfi_entry;

         rv = READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry);
         if (rv < 0) {
            return rv;
         }
 	  rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
	  if (rv < 0) {
              return rv;
	  }
	  rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
	  if (rv < 0) {
		  return rv;
	  }
	  nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
         BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_index, &ing_nh));
         module_id = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, MODULE_IDf);			 
         is_trunk = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, Tf);
         if (!is_trunk) {
              BCM_IF_ERROR_RETURN( _bcm_esw_modid_is_local(unit, module_id, &modid_local));
         }
         if (modid_local) {
              local_port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, PORT_NUMf);
         } else if (is_trunk) {
              local_port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, TGIDf);
         }

         /* VP Detach */
         if ((soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) == 0x1) && (vpn == BCM_MPLS_VPN_INVALID)){
		 soc_SOURCE_VPm_field32_set(unit, &svp, ENTRY_TYPEf, 0x0);		 
               ipmc_id = soc_VFIm_field32_get(unit, &vfi_entry, L3MC_INDEXf);
               _BCM_MULTICAST_GROUP_SET(mc_group, _BCM_MULTICAST_TYPE_VPLS, ipmc_id);
               if (modid_local) {
                   (void) _bcm_tr_mpls_broadcast_replication_list_remove (unit, local_port, mc_group, nh_index );
               }
               soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, 0x1); /* Set Drop */
               BCM_IF_ERROR_RETURN(soc_mem_write (unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ALL, nh_index, &ing_nh));
		 rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
               return rv;
         }
         
          /* VP Attach */
          if ((soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) == 0x0) && (vpn != BCM_MPLS_VPN_INVALID)){
		soc_SOURCE_VPm_field32_set(unit, &svp, ENTRY_TYPEf, 0x1);
              if (modid_local) {
                   BCM_IF_ERROR_RETURN(_bcm_tr_mpls_broadcast_replication_list_add (unit, local_port, vfi, &mc_group, nh_index ));
              }
              soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, 0x0); /* Reset Drop */
              BCM_IF_ERROR_RETURN(soc_mem_write (unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ALL, nh_index, &ing_nh));
              rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
              return rv;
          }
    }

              rv = _bcm_vp_if_used(unit, vp);
              if (rv < 0) {
                   return (BCM_E_EXISTS);
              }

              BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, _bcmVpTypeMpls));

              sal_memset(&svp, 0, sizeof(source_vp_entry_t));
              sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
        } else {
            /* allocate a new VP index */
            num_vp = soc_mem_index_count(unit, SOURCE_VPm);
            rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, _bcmVpTypeMpls, &vp);
            if (rv < 0) {
                return rv;
            }
            sal_memset(&svp, 0, sizeof(source_vp_entry_t));
            sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
        }

        drop = (mpls_port->flags & BCM_MPLS_PORT_DROP) ? 1 : 0;
        rv = _bcm_tr_mpls_l2_nh_info_add(unit, mpls_port, vp, drop, 0,
                                         &nh_index, &local_port, &is_local);
        if (rv < 0) {
            return rv;
        }

        if (mpls_port->flags & BCM_MPLS_PORT_SERVICE_TAGGED) {
            rv = _bcm_fb2_outer_tpid_entry_add(unit, mpls_port->service_tpid,
                                               &tpid_index);
            if (rv < 0) {
                goto vpls_cleanup;
            }
            tpid_enable = (1 << tpid_index);
            soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 1);
            soc_SOURCE_VPm_field32_set(unit, &svp, TPID_ENABLEf, tpid_enable);
        } else {
            soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 0);
        }
        soc_SOURCE_VPm_field32_set(unit, &svp, CLASS_IDf, 
                                   mpls_port->if_class);
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf,
            (mpls_port->flags & BCM_MPLS_PORT_NETWORK) ? 1 : 0);

        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
            rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        } else {
            /* Link in the newly allocated next-hop entry */
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NEXT_HOP_INDEXf,
                                           nh_index);
            rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
            if (rv < 0) {
                goto vpls_cleanup;
            }

            if (vpn == BCM_MPLS_VPN_INVALID) {
                   soc_SOURCE_VPm_field32_set(unit, &svp, 
                                       ENTRY_TYPEf, 0x0); /* INVALID */			   
                   rv = soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_index, &ing_nh);
                   if (rv < 0) {
                        goto vpls_cleanup;
                   }
                   soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, 0x1); /* Set Drop */
                   rv = soc_mem_write (unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ALL, nh_index, &ing_nh);	   
                   if (rv < 0) {
                        goto vpls_cleanup;
                   }
            } else {
                   /* Initialize the SVP parameters */
                   soc_SOURCE_VPm_field32_set(unit, &svp, 
                                       ENTRY_TYPEf, 0x1); /* VPLS */
            }
            soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, vfi);

            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, 0x8);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, 0x8);
            rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
            if (rv < 0) {
                goto vpls_cleanup;
            }

            /* Add the port to the VFI broadcast replication list, only_if drop=0 */
            if (is_local && !drop && (vpn != BCM_MPLS_VPN_INVALID)) {
		 rv = _bcm_tr_mpls_broadcast_replication_list_add (unit, local_port, vfi, &mc_group, nh_index );		 
               if (rv < 0) {
                    goto vpls_cleanup;
               }
            }
#if defined(BCM_TRIUMPH2_SUPPORT)
            if (soc_feature(unit, soc_feature_mpls_failover)) {
                   int failover_vp = 0;
                   bcm_mpls_port_t  failover_mpls_port;

                   _BCM_MPLS_FAILOVER_VALID_RANGE ( mpls_port->failover_id ) {
                        failover_vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->failover_port_id);
                        if (!_bcm_vp_used_get(unit, failover_vp, _bcmVpTypeMpls)) {
                              rv = BCM_E_NOT_FOUND;
                        }
                        rv = _bcm_tr_mpls_match_get(unit,  &failover_mpls_port, failover_vp);
                        if ( rv < 0 ) {
                             if (rv != BCM_E_NOT_FOUND) {
                                  goto vpls_cleanup;
                             }
                        }
                        if (mpls_port->match_label == failover_mpls_port.match_label) {
                             /* Replace failover_vp with Primary vp */
                             rv = _bcm_tr_mpls_match_delete(unit, failover_vp);
                        }
                   }
            }
#endif /* BCM_TRIUMPH2_SUPPORT  */

             /*
             * Match entries cannot be replaced, instead, callers
             * need to delete the existing entry and re-add with the
             * new match parameters.
             */
            rv = _bcm_tr_mpls_match_add(unit, mpls_port, vp);
            if (rv < 0) {
                if (is_local &&  !drop && (vpn != BCM_MPLS_VPN_INVALID)) {
                   (void) _bcm_tr_mpls_broadcast_replication_list_remove (unit, local_port, mc_group, nh_index );
                }
            } else {
                /* Set the MPLS port ID */
                BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id, vp);
                mpls_port->encap_id = nh_index;
            }
        }

vpls_cleanup:
        if (rv < 0) {
            if (tpid_enable) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            }
            if (!(mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
                (void) _bcm_vp_free(unit, _bcmVpTypeMpls, 1, vp);
                _bcm_tr_mpls_l2_nh_info_delete(unit, nh_index);
            }
            return rv;
        }
    if (old_tpid_enable) {
        for (i = 0; i < 4; i++) {
            if (old_tpid_enable & (1 << i)) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, i);
                break;
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_mpls_port_add
 * Purpose:
 *      Add an mpls port to a VPN
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN instance ID
 *      mpls_port - (IN/OUT) mpls port information (OUT : mpls_port_id)
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr_mpls_port_add(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
    int mode, rv = BCM_E_PARAM; 

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        soc_cm_debug(DK_L3, "L3 egress mode must be set first\n");
        return BCM_E_DISABLED;
    }

    if (!_BCM_MPLS_VPN_IS_VPLS(vpn) && !_BCM_MPLS_VPN_IS_VPWS(vpn)) {
        return BCM_E_PARAM;
    }

    if ((mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_VLAN) ||
        (mpls_port->criteria == BCM_MPLS_PORT_MATCH_INVALID)) {
        return BCM_E_PARAM;
    }

    if (_BCM_MPLS_VPN_IS_VPWS(vpn)) {
         rv = _bcm_tr_mpls_vpws_port_add(unit, vpn, mpls_port);
    } else if (_BCM_MPLS_VPN_IS_VPLS(vpn)) {
         rv = _bcm_tr_mpls_vpls_port_add(unit, vpn, mpls_port);
    }

    return rv;
}

int
_bcm_tr_mpls_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    int rv = BCM_E_NONE;

    if (_BCM_MPLS_VPN_IS_VPWS(vpn)) {
        int nh_index = 0, vp1, vp2;
        source_vp_entry_t svp1, svp2;
        ing_dvp_table_entry_t dvp1, dvp2;
        uint8 valid = 0;

        vp1 = _BCM_MPLS_VPN_ID_GET(vpn);
	 vp2 = vp1 + 1;

	 if ( vp == vp1) {
              rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp1, &svp1);
              if (rv < 0) {
                  return rv;
              }
              if (soc_SOURCE_VPm_field32_get(unit, &svp1, ENTRY_TYPEf) != 0) {
                   valid |= 1;
                   rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp1, &dvp1);
                   if (rv < 0) {
                       return rv;
                   }
              }
	 }
	
       if (vp == vp2) {
              rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp2, &svp2);
              if (rv < 0) {
                 return rv;
             }
             if (soc_SOURCE_VPm_field32_get(unit, &svp2, ENTRY_TYPEf) != 0) {
                 valid |= 2;
                 rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp2, &dvp2);
                 if (rv < 0) {
                     return rv;
                 }
             }
       }

        if (vp == vp1) {
            if (!(valid & 1)) {
		return BCM_E_NOT_FOUND;
            }
            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp1, 
                                                      NEXT_HOP_INDEXf);
        } else if (vp == vp2) {
            if (!(valid & 2)) {
	return BCM_E_NOT_FOUND;
            }
            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp2, 
                                                      NEXT_HOP_INDEXf);
        } else {
            return BCM_E_NOT_FOUND;
        }

        rv = _bcm_tr_mpls_match_delete(unit, vp);
        if ( rv < 0 ) {
            if (rv != BCM_E_NOT_FOUND) {
                     return rv;
            } else {
                rv = BCM_E_NONE;
            	}
        }

        /* If the other port is valid, point it to itself */
        if (valid == 3) { 
            if (vp == vp1) {
                /* Set the DROP bit in the destination next-hop */
                rv = _bcm_tr_mpls_nh_drop(unit, vp2, 1);
                if (rv >= 0) {
                    rv = soc_mem_field32_modify(unit, SOURCE_VPm, 
                                                vp2, DVPf, vp2);
                }
            } else {
                /* Set the DROP bit in the destination next-hop */
                rv = _bcm_tr_mpls_nh_drop(unit, vp1, 1);
                if (rv >= 0) {
                    rv = soc_mem_field32_modify(unit, SOURCE_VPm, 
                                                vp1, DVPf, vp1);
                }
            }
        }
        if (rv >= 0) {
            /* Invalidate the VP being deleted */
            if (vp == vp1) {
                sal_memset(&svp1, 0, sizeof(source_vp_entry_t));
                rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp1, &svp1);
                if (rv >= 0) {
                    rv = soc_mem_field32_modify(unit, ING_DVP_TABLEm, 
                                                vp1, NEXT_HOP_INDEXf, 0);
                }
            } else {
                sal_memset(&svp2, 0, sizeof(source_vp_entry_t));
                rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp2, &svp2);
                if (rv >= 0) {
                    rv = soc_mem_field32_modify(unit, ING_DVP_TABLEm, 
                                                vp2, NEXT_HOP_INDEXf, 0);
                }
            }

            /* Delete the next-hop info */
            if (rv >= 0) {
                rv = _bcm_tr_mpls_l2_nh_info_delete(unit, nh_index);
            }
        }
    } else if (_BCM_MPLS_VPN_IS_VPLS(vpn)) {
        int nh_index = 0, vfi, ipmc_id, isModLocal;
        source_vp_entry_t svp;
        ing_dvp_table_entry_t dvp;
        ing_l3_next_hop_entry_t ing_nh;
        vfi_entry_t vfi_entry;
        bcm_module_t modid, mod_out;
        bcm_port_t port, port_out;
        bcm_multicast_t mc_group = 0;
        bcm_gport_t local_gport;

        vfi = _BCM_MPLS_VPN_ID_GET(vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
            return BCM_E_NOT_FOUND;
        }
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            return BCM_E_NOT_FOUND;
        }
        rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
        if (rv < 0) {
            return rv;
        }
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

        rv = _bcm_tr_mpls_match_delete(unit, vp);
        if ( rv < 0 ) {
            if (rv != BCM_E_NOT_FOUND) {
                     return rv;
            }
        }

        /* Check if this port is on the local unit */
        rv = soc_mem_read(unit, ING_L3_NEXT_HOPm,
                          MEM_BLOCK_ANY, nh_index, &ing_nh);
        if (rv < 0) {
            return rv;
        }
        if (!soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
            /* If this port is on the local unit, remove from replication list */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, modid, &isModLocal)); 
            if (TRUE == isModLocal) {
                /* Get IPMC index from HW */
                rv = READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry);
                if (rv < 0) {
                    return rv;
                }
#ifdef BCM_TRIUMPH2_SUPPORT
                if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                    SOC_IS_VALKYRIE2(unit)) {
                    ipmc_id = soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf);
                } else
#endif
                {
                    ipmc_id = soc_VFIm_field32_get(unit, &vfi_entry, 
                                                   L3MC_INDEXf);
                }
                _BCM_MULTICAST_GROUP_SET(mc_group, _BCM_MULTICAST_TYPE_VPLS, 
                                         ipmc_id);

                if ((port == 0) && (modid == 0)) {
                     /* Global label, mod/port not part of lookup key */
                     local_gport = BCM_GPORT_INVALID;
                     return BCM_E_PORT;
                } else {
                     BCM_IF_ERROR_RETURN
                          (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                       modid, port, &mod_out, &port_out));
                     BCM_GPORT_MODPORT_SET(local_gport, mod_out, port_out);
                }

#ifdef BCM_TRIUMPH2_SUPPORT
                if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                    SOC_IS_VALKYRIE2(unit)) {
                    rv = bcm_tr2_multicast_egress_delete
                             (unit, mc_group, local_gport, nh_index 
                              + BCM_XGS3_DVP_EGRESS_IDX_MIN);
                } else
#endif
                {
                    rv = bcm_tr_multicast_egress_delete
                             (unit, mc_group, local_gport, nh_index);
                }
                if (rv < 0) {
                    return rv;
                }
            }
        }

        /* Clear the SVP and DVP table entries */
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        if (rv < 0) {
            return rv;
        }

        sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
        rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
        if (rv < 0) {
            return rv;
        }

        /* Clear the next-hop table entries */
        rv = _bcm_tr_mpls_l2_nh_info_delete(unit, nh_index);
        if (rv < 0) {
            return rv;
        }

        /* Free the VP */
        (void) _bcm_vp_free(unit, _bcmVpTypeMpls, 1, vp);
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        bcm_gport_t gport;

        /* Release Service counter, if any */
        BCM_GPORT_MPLS_PORT_ID_SET(gport, vp);
        _bcm_esw_flex_stat_handle_free(unit, _bcmFlexStatTypeGport, gport);
    }
#endif
       return rv;
}

/*
 * Function:
 *      bcm_mpls_port_delete
 * Purpose:
 *      Delete an mpls port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      mpls_port_id - (IN) mpls port information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mpls_port_id)
{
    int vp;


    if (!_BCM_MPLS_VPN_IS_VPLS(vpn) && !_BCM_MPLS_VPN_IS_VPWS(vpn)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
        return BCM_E_NOT_FOUND;
    }
    return (_bcm_tr_mpls_port_delete(unit, vpn, vp));
}

/*
 * Function:
 *      bcm_mpls_port_delete_all
 * Purpose:
 *      Delete all mpls ports from a VPN
 * Parameters:
 *      unit - Device Number
 *      vpn - VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_port_delete_all(int unit, bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;


    if (!_BCM_MPLS_VPN_IS_VPLS(vpn) && !_BCM_MPLS_VPN_IS_VPWS(vpn)) {
        return BCM_E_PARAM;
    }

    if (_BCM_MPLS_VPN_IS_VPWS(vpn)) {
        int vp1, vp2;
        source_vp_entry_t svp1, svp2;
        ing_dvp_table_entry_t dvp1, dvp2;

        vp1 = _BCM_MPLS_VPN_ID_GET(vpn);
        vp2 = vp1 + 1;

        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp1, &svp1);
        if (rv < 0) {
            goto done;
        }
        rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp1, &dvp1);
        if (rv < 0) {
            goto done;
        }

        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp2, &svp2);
        if (rv < 0) {
            goto done;
        }
        rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp2, &dvp2);
        if (rv < 0) {
            goto done;
        }

        /* See if the entry already exists */
        if (soc_SOURCE_VPm_field32_get(unit, &svp1, ENTRY_TYPEf) != 0) {
            rv = _bcm_tr_mpls_port_delete(unit, vpn, vp1);
            if (rv < 0) {
                goto done;
            }
        }
        if (soc_SOURCE_VPm_field32_get(unit, &svp2, ENTRY_TYPEf) != 0) {
            rv = _bcm_tr_mpls_port_delete(unit, vpn, vp2);
            if (rv < 0) {
                goto done;
            }
        }

#if defined(BCM_TRIUMPH2_SUPPORT)
        if (soc_feature(unit, soc_feature_mpls_failover)) {
              initial_prot_nhi_table_entry_t	 prot_nhi_entry;
              egr_l3_next_hop_entry_t       egr_nh;
              int  failover_nh_index, num_entry, rv, vp3;
              int  primary_nh_index;
              source_vp_entry_t           svp3;
              ing_dvp_table_entry_t      dvp3;

              /* Obtain Failover_NHI from DVP2 */
              failover_nh_index = soc_mem_field32_get(unit, ING_DVP_TABLEm, &dvp2, NEXT_HOP_INDEXf);
	       num_entry = soc_mem_index_count (unit, INITIAL_PROT_NHI_TABLEm);

              /* Obtain Primary_NHI from PROT_NHI */
		for (primary_nh_index=0; primary_nh_index < num_entry; primary_nh_index++) {
			  rv = soc_mem_read(unit, INITIAL_PROT_NHI_TABLEm, 
									   MEM_BLOCK_ANY, primary_nh_index, &prot_nhi_entry);
			  if (rv != BCM_E_NONE) {
			  	continue;
			  }
			  if (failover_nh_index  == soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLEm,
										&prot_nhi_entry, PROT_NEXT_HOP_INDEXf) ) {
                              /* Read the egress next_hop entry */
                              rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                                         primary_nh_index, &egr_nh);

                              /* Obtain vp from EGR_NH */
				  vp3 = soc_mem_field32_get (unit, EGR_L3_NEXT_HOPm, &egr_nh, DVPf);
				  rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp3, &svp3);
				  if (rv < 0) {
					  goto done;
				  }
				  
				  /* See if the entry already exists */
				  if (soc_SOURCE_VPm_field32_get(unit, &svp3, ENTRY_TYPEf) != 0) {
					  rv = _bcm_tr_mpls_match_delete(unit, vp3);
					  if ( rv < 0 ) {
						  if (rv != BCM_E_NOT_FOUND) {
                                                      goto done;
						  }
					  }
					  /* Set the DROP bit in the destination next-hop */
					  rv = _bcm_tr_mpls_nh_drop(unit, vp3, 1);
					  if (rv >= 0) {
						  rv = soc_mem_field32_modify(unit, SOURCE_VPm, 
													  vp3, DVPf, vp3);
                                     }

                                     sal_memset(&svp3, 0, sizeof(source_vp_entry_t));
                                     rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp3, &svp3);
                                     if (rv >= 0) {
                                          rv = soc_mem_field32_modify(unit, ING_DVP_TABLEm, 
                                                                                vp3, NEXT_HOP_INDEXf, 0);
                                     }

                             
					  /* Delete the next-hop info */
					  if (rv >= 0) {
						  rv = _bcm_tr_mpls_l2_nh_info_delete(unit, primary_nh_index);
					  }
					  
                                     sal_memset(&dvp3, 0, sizeof(ing_dvp_table_entry_t));
					  rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp3, &dvp3);					  
					  break;
				   }				  
			  	}else {
			  	         continue;
                            }
              }
        }
#endif /* BCM_TRIUMPH2_SUPPORT  */
    } else if (_BCM_MPLS_VPN_IS_VPLS(vpn)) {
        uint32 vfi, vp, num_vp;
        source_vp_entry_t svp;

        vfi = _BCM_MPLS_VPN_ID_GET(vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
            rv =  BCM_E_NOT_FOUND;
            goto done;
        }
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        for (vp = 0; vp < num_vp; vp++) {
            /* Check for the validity of the VP */
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {	
                 continue;
            }
            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
            if (rv < 0) {
                goto done;
            }
            if ((soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) == 1) &&
                (vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf))) {
                rv = _bcm_tr_mpls_port_delete(unit, vpn, vp);
                if (rv < 0) {
                    goto done;
                }
            }
        }
    }
done:
    return rv;
}

STATIC int
_bcm_tr_mpls_port_get(int unit, bcm_vpn_t vpn, int vp,
                        bcm_mpls_port_t *mpls_port)
{
    int i, nh_index, tpid_enable = 0, rv = BCM_E_NONE;
    ing_dvp_table_entry_t dvp;
    source_vp_entry_t svp;
    int network_port_flag=0;

    /* Initialize the structure */
    bcm_mpls_port_t_init(mpls_port);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id, vp);

    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

    /* Get the match parameters */
    rv = _bcm_tr_mpls_match_get(unit, mpls_port, vp);
    BCM_IF_ERROR_RETURN(rv);

   /* Check for Network-Port */
    BCM_IF_ERROR_RETURN (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
    network_port_flag = soc_SOURCE_VPm_field32_get(unit, &svp, NETWORK_PORTf);

    /* Get the next-hop parameters */
    rv = _bcm_tr_mpls_l2_nh_info_get(unit, mpls_port, nh_index, network_port_flag);
    BCM_IF_ERROR_RETURN(rv);

    /* Fill in SVP parameters */
    mpls_port->if_class = soc_SOURCE_VPm_field32_get(unit, &svp, CLASS_IDf);
    if (network_port_flag) {
        mpls_port->flags |= BCM_MPLS_PORT_NETWORK;
    }
    if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
        tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp, TPID_ENABLEf);
        if (tpid_enable) {
            mpls_port->flags |= BCM_MPLS_PORT_SERVICE_TAGGED;
            for (i = 0; i < 4; i++) {
                if (tpid_enable & (1 << i)) {
                    _bcm_fb2_outer_tpid_entry_get(unit, &mpls_port->service_tpid, i);
                }
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_mpls_port_get
 * Purpose:
 *      Get an mpls port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      mpls_port  - (IN/OUT) mpls port information
 */
int
bcm_tr_mpls_port_get(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
    int vp;


    if (!_BCM_MPLS_VPN_IS_VPLS(vpn) && !_BCM_MPLS_VPN_IS_VPWS(vpn)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
        return BCM_E_NOT_FOUND;
    }
    return _bcm_tr_mpls_port_get(unit, vpn, vp, mpls_port);
}

/*
 * Function:
 *      bcm_mpls_port_get_all
 * Purpose:
 *      Get all mpls ports from a VPN
 * Parameters:
 *      unit     - (IN) Device Number
 *      vpn      - (IN) VPN instance ID
 *      port_max   - (IN) Maximum number of interfaces in array
 *      port_array - (OUT) Array of mpls ports
 *      port_count - (OUT) Number of interfaces returned in array
 *
 */
int
bcm_tr_mpls_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                           bcm_mpls_port_t *port_array, int *port_count)
{
    int vp, rv = BCM_E_NONE;


    if (!_BCM_MPLS_VPN_IS_VPLS(vpn) && !_BCM_MPLS_VPN_IS_VPWS(vpn)) {
        return BCM_E_PARAM;
    }

    *port_count = 0;

    if (_BCM_MPLS_VPN_IS_VPWS(vpn)) {
        vp = _BCM_MPLS_VPN_ID_GET(vpn);
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            if (*port_count < port_max) {
                rv = _bcm_tr_mpls_port_get(unit, vpn, vp, 
                                           &port_array[*port_count]);
                if (rv < 0) {
                    goto done;
                }
                (*port_count)++;
            }
        }

        vp += 1;
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            if (*port_count < port_max) {
                rv = _bcm_tr_mpls_port_get(unit, vpn, vp, 
                                           &port_array[*port_count]);
                if (rv < 0) {
                    goto done;
                }
                (*port_count)++;
            }
        }
    } else if (_BCM_MPLS_VPN_IS_VPLS(vpn)) {
        uint32 vfi, num_vp, entry_type;
        source_vp_entry_t svp;

        vfi = _BCM_MPLS_VPN_ID_GET(vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
            rv = BCM_E_NOT_FOUND;
            goto done;
        }
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        for (vp = 0; vp < num_vp; vp++) {
            if (*port_count == port_max) {
                break;
            }
            rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
            if (rv < 0) {
                goto done;
            }
            entry_type = soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf);

            if (vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf) && entry_type == 0x1) {
                rv = _bcm_tr_mpls_port_get(unit, vpn, vp, 
                                           &port_array[*port_count]);
                if (rv < 0) {
                    goto done;
                }
                (*port_count)++;
            }
        }
    }
done:
    return rv;
}

/*
 * Function:
 *      _bcm_tr_mpls_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a MPLS port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_mpls_port_resolve(int unit, bcm_gport_t mpls_port_id, 
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE, nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;

    rv = _bcm_tr_mpls_check_init (unit);
    if (rv < 0) {
        return rv;
     }

    if (!BCM_GPORT_IS_MPLS_PORT(mpls_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, 
                                              NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &ing_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf) != 0x2) {
        /* Entry type is not L2 DVP */
        return BCM_E_NOT_FOUND;
    }
    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
        *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
    } else {
        /* Only add this to replication set if destination is local */
        *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
        *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
    }
    *id = vp;
    return rv;
}

static soc_field_t _tnl_label_f[] =
    { MPLS_LABEL_0f, MPLS_LABEL_1f, MPLS_LABEL_2f, MPLS_LABEL_3f };
static soc_field_t _tnl_push_action_f[] =
    { MPLS_PUSH_ACTION_0f, MPLS_PUSH_ACTION_1f,
      MPLS_PUSH_ACTION_2f, MPLS_PUSH_ACTION_3f };
static soc_field_t _tnl_exp_select_f[] =
    { MPLS_EXP_SELECT_0f, MPLS_EXP_SELECT_1f, 
      MPLS_EXP_SELECT_2f, MPLS_EXP_SELECT_3f };
static soc_field_t _tnl_exp_ptr_f[] =   
    { MPLS_EXP_MAPPING_PTR_0f, MPLS_EXP_MAPPING_PTR_1f, 
      MPLS_EXP_MAPPING_PTR_2f, MPLS_EXP_MAPPING_PTR_3f };
static soc_field_t _tnl_exp_f[] =    
    { MPLS_EXP_0f, MPLS_EXP_1f, MPLS_EXP_2f, MPLS_EXP_3f };
static soc_field_t _tnl_ttl_f[] =
    { MPLS_TTL_0f, MPLS_TTL_1f, MPLS_TTL_2f, MPLS_TTL_3f };
static soc_field_t _tnl_pri_f[] = 
    { NEW_PRI_0f, NEW_PRI_1f, NEW_PRI_2f, NEW_PRI_3f };
static soc_field_t _tnl_cfi_f[] = 
    { NEW_CFI_0f, NEW_CFI_1f, NEW_CFI_2f, NEW_CFI_3f };

/*  
 * Function:
 *      bcm_mpls_tunnel_initiator_set
 * Purpose:
 *      Set MPLS Tunnel initiator
 * Parameters:
 *      unit - Device Number
 *      intf - The egress L3 interface
 *      num_labels  - Number of labels in the array
 *      label_array - Array of MPLS label and header information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_tunnel_initiator_set(int unit, bcm_if_t intf, int num_labels,
                                 bcm_mpls_egress_label_t *label_array)
{
    egr_ip_tunnel_mpls_entry_t tnl_entry;
    egr_l3_intf_entry_t if_entry;
    int rv, i, num_exp_map, num_ip_tnl, push_action=0, offset;
    int found = 0, tnl_index = 0, mpls_index = 0;
    bcm_tunnel_initiator_t tnl_init;
    uint32 tnl_flags;
    int old_push_act_value;

    if ((num_labels < 0) || (num_labels > 2)  ||
         (intf >= L3_INFO(unit)->l3_intf_table_size)) {
        return BCM_E_PARAM;
    }

    if (num_labels == 2) {
        push_action = 0x2; /* push 2 labels */
    } else if (num_labels == 1) {
        push_action = 0x1; /* push 1 label */
    } 

    /* Param checking */
    num_exp_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) >> 6;
    for (i = 0; i < num_labels; i++) {
        if ((label_array[i].label > 0xfffff) ||
            (label_array[i].qos_map_id >= num_exp_map) ||
            (label_array[i].exp > 7) || (label_array[i].pkt_pri > 7) ||
            (label_array[i].pkt_cfi > 1)) {
            return BCM_E_PARAM;
        }
    }

    if (!BCM_L3_INTF_USED_GET(unit, intf)) {
        soc_cm_debug(DK_L3, "L3 interface not created\n");
        return BCM_E_NOT_FOUND;
    }

    /* L3 interface info */
    rv = READ_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, intf, &if_entry);
    if (rv < 0) {
        return rv;
    }

    tnl_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry, 
                                             TUNNEL_INDEXf);
    mpls_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry, 
                                              MPLS_TUNNEL_INDEXf);

    if ((mpls_index != 0) && (_BCM_MPLS_TNL_USED_GET(unit, mpls_index))) {
        rv = READ_EGR_IP_TUNNEL_MPLSm(unit, MEM_BLOCK_ANY, 
                                      tnl_index, &tnl_entry);
        if (rv < 0) {
            return rv;
        }
        if (soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                ENTRY_TYPEf) != 3) {
            return BCM_E_INTERNAL;
        }
        offset = mpls_index & 0x3;
        old_push_act_value = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm,
                                   &tnl_entry, _tnl_push_action_f[offset]);
        if (old_push_act_value != push_action) {
            /* Changing push action is not permitted, need to clear/set */
            return BCM_E_PARAM;
        }
    } else {
        /* allocate an unused EGR_IP_TUNNEL_MPLS_ENTRY */
        num_ip_tnl = soc_mem_index_count(unit, EGR_IP_TUNNELm);

        /*
         * First try finding an allocated tunnel entry with 
         * unused MPLS entries.
         */
        for (tnl_index = 0; tnl_index < num_ip_tnl; tnl_index++) {
            if (!_BCM_MPLS_IP_TNL_USED_GET(unit, tnl_index)) {
                continue;
            }
            mpls_index = tnl_index * 4;
            for (i = 0; i < 4; i++) {
                if ((push_action == 0x2) && (i == 3)) {
                    /* Pushing 2 labels, need 2 consecutive entries. */
                    break;
                }
                if (!(_BCM_MPLS_TNL_USED_GET(unit, mpls_index))) {
                    if (push_action == 0x2) {
                        /* Pushing 2 labels, need 2 consecutive entries */
                        if (!(_BCM_MPLS_TNL_USED_GET(unit, mpls_index + 1))) {
                            found = 1;
                            break;
                        }
                    } else {
                        found = 1;
                        break;
                    }
                }
                mpls_index++;
            }
            if (found) {
                break;
            }
        }
        if (found) {
            /* Mark entry as used */
            _BCM_MPLS_TNL_USED_SET(unit, mpls_index);

            /* Read existing entry */
            rv = READ_EGR_IP_TUNNEL_MPLSm(unit, MEM_BLOCK_ANY, 
                                          tnl_index, &tnl_entry);
            if (rv < 0) {
                goto cleanup;
            }
        } else {
            /*
             * Alloc an egr_ip_tunnel entry. By calling bcm_xgs3_tnl_init_add
             * with _BCM_L3_SHR_WRITE_DISABLE flag, a tunnel index is
             * allocated but nothing is written to hardware. The "tnl_init"
             * information is not used, set to all zero.
             */
            sal_memset(&tnl_init, 0, sizeof(bcm_tunnel_initiator_t));
            tnl_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE |
                        _BCM_L3_SHR_SKIP_INDEX_ZERO;
            rv = bcm_xgs3_tnl_init_add(unit, tnl_flags, &tnl_init, &tnl_index);
            if (rv < 0) {
                return rv;
            }
            mpls_index = tnl_index * 4;

            /* Mark entries as used */
            _BCM_MPLS_IP_TNL_USED_SET(unit, tnl_index);
            _BCM_MPLS_TNL_USED_SET(unit, mpls_index);
            if (push_action == 0x2) {
                _BCM_MPLS_TNL_USED_SET(unit, mpls_index + 1);
            }
            /* Clear and set ENTRY_TYPE */
            sal_memset(&tnl_entry, 0, sizeof(egr_ip_tunnel_mpls_entry_t));
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                ENTRY_TYPEf, 0x3);
        } 
    }

    offset = mpls_index & 0x3;
    for (i = 0; i < num_labels; i++) {
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                            _tnl_push_action_f[offset], push_action);
        if (push_action > 0) {
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                _tnl_label_f[offset], label_array[i].label);
        }
        if ((label_array[i].flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
            (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) {

            /* Use the specified EXP, PRI and CFI */
            if ((label_array[i].flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) ||
                (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ||
                (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_PRI_REMARK)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                _tnl_exp_select_f[offset], 0x0); /* USE_FIXED */
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                _tnl_exp_f[offset], label_array[i].exp);
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                _tnl_pri_f[offset], label_array[i].pkt_pri);
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                _tnl_cfi_f[offset], label_array[i].pkt_cfi);
        } else if (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
            if (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_PRI_SET) {
                return BCM_E_PARAM;
            }
            /* Use EXP-map for EXP, PRI and CFI */
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                _tnl_exp_select_f[offset], 0x1); /* USE_MAPPING */
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                _tnl_exp_ptr_f[offset], label_array[i].qos_map_id);
        } else { /* BCM_MPLS_EGRESS_LABEL_EXP_COPY */
            /* Use EXP from inner label. If there is no inner label,
             * use the specified EXP value. Use EXP-map for PRI/CFI.
             */
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                _tnl_exp_select_f[offset], 0x2); /* USE_INNER */
    
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                _tnl_exp_f[offset], label_array[i].exp);
    
            /* Use EXP-map for PRI/CFI */
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                _tnl_exp_ptr_f[offset], label_array[i].qos_map_id);
        }
    
        if (label_array[i].flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
            /* Use specified TTL */
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                _tnl_ttl_f[offset], label_array[i].ttl);
        } else {
            soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry, 
                                _tnl_ttl_f[offset], 0);
        }
        offset++;
    }

    /* Commit the values to HW */
    rv = WRITE_EGR_IP_TUNNEL_MPLSm(unit, MEM_BLOCK_ANY, 
                                   tnl_index, &tnl_entry);
    if (rv < 0) {
        goto cleanup;
    }

    /* Update the EGR_L3_INTF to point to the MPLS tunnel entry */
    rv = soc_mem_field32_modify(unit, EGR_L3_INTFm, intf, 
                                MPLS_TUNNEL_INDEXf, mpls_index);
    return rv;

cleanup:
    if (!found) {
        /* Free the tunnel entry */
        sal_memset(&tnl_entry, 0, sizeof(egr_ip_tunnel_mpls_entry_t));
        tnl_flags = _BCM_L3_SHR_WRITE_DISABLE;
        (void) bcm_xgs3_tnl_init_del(unit, tnl_flags, tnl_index);

        /* Clear "in-use" for the IP tunnel entry */
        _BCM_MPLS_IP_TNL_USED_CLR(unit, tnl_index);
    }
    /* Clear "in-use" for the MPLS tunnel entry(s) */
    _BCM_MPLS_TNL_USED_CLR(unit, mpls_index);
    if (push_action == 0x2) {
        _BCM_MPLS_TNL_USED_CLR(unit, mpls_index + 1);
    }
    return rv;
}

/*  
 * Function:
 *      _bcm_tr_mpls_tunnel_initiator_clear
 * Purpose:
 *      Clear MPLS Tunnel initiator
 * Parameters:
 *      unit - Device Number
 *      intf - The egress L3 interface
 *      num_labels  - Number of labels in the array
 *      label_array - Array of MPLS label and header information
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr_mpls_tunnel_initiator_clear(int unit, int intf_id)
{
    egr_ip_tunnel_mpls_entry_t tnl_entry;
    egr_l3_intf_entry_t if_entry;
    int rv, offset, push_action;
    int tnl_index = 0, mpls_index = 0;
    uint32 tnl_flags;

    /* L3 interface info */
    rv = READ_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, intf_id, &if_entry);
    if (rv < 0) {
        return rv;
    }
    tnl_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry,
                                             TUNNEL_INDEXf);
    mpls_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry,
                                              MPLS_TUNNEL_INDEXf);

    if (!_BCM_MPLS_TNL_USED_GET(unit, mpls_index)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_EGR_IP_TUNNEL_MPLSm(unit, MEM_BLOCK_ANY,
                                  tnl_index, &tnl_entry);
    if (rv < 0) {
        return rv;
    }
    if (soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                            ENTRY_TYPEf) != 3) {
        return BCM_E_NOT_FOUND;
    }
    offset = mpls_index & 0x3;
    push_action = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                      _tnl_push_action_f[offset]);

    /* Update the EGR_L3_INTF to no longer point to the MPLS tunnel entry */
    rv = soc_mem_field32_modify(unit, EGR_L3_INTFm, intf_id,
                                MPLS_TUNNEL_INDEXf, 0);
    if (rv < 0) {
        return rv;
    }

    /* Clear the MPLS tunnel entry(s) */
    rv = READ_EGR_IP_TUNNEL_MPLSm(unit, MEM_BLOCK_ANY, 
                                  tnl_index, &tnl_entry);
    if (rv < 0) {
        return rv;
    }

    soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                        _tnl_label_f[offset], 0);
    soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                        _tnl_push_action_f[offset], 0);
    soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                        _tnl_exp_select_f[offset], 0);
    soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                        _tnl_exp_ptr_f[offset], 0);
    soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                        _tnl_exp_f[offset], 0);
    soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                        _tnl_ttl_f[offset], 0);
    if (push_action == 0x2) {
        offset++;
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                            _tnl_label_f[offset], 0);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                            _tnl_push_action_f[offset], 0);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                            _tnl_exp_select_f[offset], 0);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                            _tnl_exp_ptr_f[offset], 0);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                            _tnl_exp_f[offset], 0);
        soc_mem_field32_set(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                            _tnl_ttl_f[offset], 0);
    }
    rv = WRITE_EGR_IP_TUNNEL_MPLSm(unit, MEM_BLOCK_ANY, 
                                   tnl_index, &tnl_entry);
    if (rv < 0) {
        return rv;
    }

    /* Clear "in-use" for the MPLS tunnel entry(s) */
    _BCM_MPLS_TNL_USED_CLR(unit, mpls_index);
    if (push_action == 0x2) {
        _BCM_MPLS_TNL_USED_CLR(unit, mpls_index + 1);
    }

    /* See if we can free the IP tunnel base entry */
    mpls_index = tnl_index * 4;
    if (!(_BCM_MPLS_TNL_USED_GET(unit, mpls_index)) &&
        !(_BCM_MPLS_TNL_USED_GET(unit, mpls_index + 1)) &&
        !(_BCM_MPLS_TNL_USED_GET(unit, mpls_index + 2)) &&
        !(_BCM_MPLS_TNL_USED_GET(unit, mpls_index + 3))) {
        /* None of the 4 entries are used, free base entry */
        tnl_flags = _BCM_L3_SHR_WRITE_DISABLE;
        (void) bcm_xgs3_tnl_init_del(unit, tnl_flags, tnl_index);

        /* Clear "in-use" for the IP tunnel entry */
        _BCM_MPLS_IP_TNL_USED_CLR(unit, tnl_index);
    }
	
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_tunnel_initiator_clear
 * Purpose:
 *      Clear MPLS Tunnel Initiator
 * Parameters:
 *      unit - Device Number
 *      intf - The egress L3 interface
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_tunnel_initiator_clear(int unit, bcm_if_t intf)
{
    if (intf >= L3_INFO(unit)->l3_intf_table_size) {
        return BCM_E_PARAM;
    }

    if (!BCM_L3_INTF_USED_GET(unit, intf)) {
        soc_cm_debug(DK_L3, "L3 interface not created\n");
        return BCM_E_NOT_FOUND;
    }
    return _bcm_tr_mpls_tunnel_initiator_clear(unit, intf);
}

/*
 * Function:
 *      bcm_mpls_tunnel_initiator_get
 * Purpose:
 *      Get MPLS Tunnel Initiator info
 * Parameters:
 *      unit        - (IN) Device Number
 *      intf        - (IN) The egress L3 interface
 *      label_max   - (IN) Number of entries in label_array
 *      label_array - (OUT) MPLS header information
 *      label_count - (OUT) Actual number of labels returned
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_tunnel_initiator_get(int unit, bcm_if_t intf, int label_max,
                                 bcm_mpls_egress_label_t *label_array,
                                 int *label_count)
{
    egr_ip_tunnel_mpls_entry_t tnl_entry;
    egr_l3_intf_entry_t if_entry;
    int rv, i, push_action, offset;
    int tnl_index = 0, mpls_index = 0;

    if ((label_array == NULL) ||
        (intf >= L3_INFO(unit)->l3_intf_table_size)) {
        return BCM_E_PARAM;
    }

    if (!BCM_L3_INTF_USED_GET(unit, intf)) {
        soc_cm_debug(DK_L3, "L3 interface not created\n");
        return BCM_E_NOT_FOUND;
    }

    /* L3 interface info */
    rv = READ_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, intf, &if_entry);
    if (rv < 0) {
        return rv;
    }
    tnl_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry, 
                                             TUNNEL_INDEXf);
    mpls_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry, 
                                              MPLS_TUNNEL_INDEXf);

    if (!_BCM_MPLS_TNL_USED_GET(unit, mpls_index)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_EGR_IP_TUNNEL_MPLSm(unit, MEM_BLOCK_ANY, 
                                  tnl_index, &tnl_entry);
    if (rv < 0) {
        return rv;
    }
    if (soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                            ENTRY_TYPEf) != 3) {
        return BCM_E_NOT_FOUND;
    }

    *label_count = 0;
    sal_memset(label_array, 0, sizeof(bcm_mpls_egress_label_t) * label_max);
    offset = mpls_index & 0x3;
    push_action = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                      _tnl_push_action_f[offset]);
    if (push_action == 0x1) {
        *label_count = 1;
    } else if (push_action == 0x2) {
        *label_count = 2;
    }
    if (label_max < *label_count) {
        *label_count = label_max;
    }
    for (i = 0; i < *label_count; i++) {
        label_array[i].label =
            soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm,
                                &tnl_entry, _tnl_label_f[offset]);
        if (soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                _tnl_exp_select_f[offset]) == 0x0) {
            /* Use the specified EXP, PRI and CFI */
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_PRI_SET;
            label_array[i].exp = 
                soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm,
                                    &tnl_entry, _tnl_exp_f[offset]);
            label_array[i].pkt_pri = 
                soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm,
                                    &tnl_entry, _tnl_pri_f[offset]);
            label_array[i].pkt_cfi = 
                soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm,
                                    &tnl_entry, _tnl_cfi_f[offset]);
        } else if (soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                       _tnl_exp_select_f[offset]) == 0x1) {
            /* Use EXP-map for EXP, PRI and CFI */
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_EXP_REMARK;
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;

            label_array[i].qos_map_id = 
                soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm,
                                    &tnl_entry, _tnl_exp_ptr_f[offset]);
        } else {
            /* Use EXP from incoming label. If there is no incoming label,
             * use the specified EXP value.
             */
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
            label_array[i].exp = 
                soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm,
                                    &tnl_entry, _tnl_exp_f[offset]);

            /* Use EXP-map for PRI/CFI */
            label_array[i].qos_map_id = 
                soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm,
                                    &tnl_entry, _tnl_exp_ptr_f[offset]);
        }
        if (soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                _tnl_ttl_f[offset])) {
            /* Use specified TTL value */
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            label_array[i].ttl = 
                soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm,
                                    &tnl_entry, _tnl_ttl_f[offset]);
        } else {
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        }
        offset++;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_tunnel_initiator_clear_all
 * Purpose:
 *      Clear all MPLS Tunnel Initiators
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_tunnel_initiator_clear_all(int unit)
{
    egr_l3_intf_entry_t if_entry;
    int mpls_index, i, rv;


    for (i = 0; i < L3_INFO(unit)->l3_intf_table_size; i++) {
        if (!BCM_L3_INTF_USED_GET(unit, i)) {
            continue;
        }
        rv = READ_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, i, &if_entry);
        if (rv < 0) {
            return rv;
        }
        mpls_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry,
                                                  MPLS_TUNNEL_INDEXf);
        if (!_BCM_MPLS_TNL_USED_GET(unit, mpls_index)) {
            continue;
        }
        rv = _bcm_tr_mpls_tunnel_initiator_clear(unit, i);
        if (rv < 0) {
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *		bcm_tr_mpls_set_entry_type
 * Purpose:
 *		To set Entry_Type  of  EGR_L3_NEXT_HOP Entry 
 * Parameters:
 *		IN :  Unit
 *           IN :  Egress Interface
 * Returns:
 *		BCM_E_XXX
 */

int 
bcm_tr_mpls_set_entry_type(int unit, int nh_index, bcm_if_t intf)
{
  int rv=BCM_E_UNAVAIL;
  int tnl_index = 0, mpls_index = 0, tunnel_type;
  egr_ip_tunnel_mpls_entry_t tnl_entry;
  egr_l3_intf_entry_t if_entry;
  egr_l3_next_hop_entry_t egr_nh;
  egr_mac_da_profile_entry_t macda;
  uint32 macda_index=-1;
  void *entries[1];
  bcm_l3_egress_t nh_info;

    BCM_IF_ERROR_RETURN(READ_EGR_L3_INTFm(unit, MEM_BLOCK_ANY, 
                                      intf, &if_entry));

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    tnl_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry, 
                                              TUNNEL_INDEXf);
    mpls_index = soc_EGR_L3_INTFm_field32_get(unit, &if_entry, 
                                              MPLS_TUNNEL_INDEXf);

    if ((mpls_index != 0) && (_BCM_MPLS_TNL_USED_GET(unit, mpls_index))) {
         BCM_IF_ERROR_RETURN(READ_EGR_IP_TUNNEL_MPLSm(unit, MEM_BLOCK_ANY, 
                                              tnl_index, &tnl_entry));

         tunnel_type = soc_mem_field32_get(unit, EGR_IP_TUNNEL_MPLSm, &tnl_entry,
                                              ENTRY_TYPEf);

         if (tunnel_type == 0x3) {
              BCM_IF_ERROR_RETURN(bcm_xgs3_nh_get(unit, nh_index, &nh_info));
              sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));			  
              soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm, 
                                              &macda, MAC_ADDRESSf, nh_info.mac_addr);
              entries[0] = &macda;
              rv = _bcm_mac_da_profile_entry_add(unit, entries, 1, &macda_index);
              soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                               &egr_nh, MAC_DA_PROFILE_INDEXf,
                                               macda_index);
              soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                              ENTRY_TYPEf, 0x1);
              soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                              HG_MODIFY_ENABLEf, 0x1);			  
         } else {
              soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                              ENTRY_TYPEf, 0x0);
              soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                              HG_MODIFY_ENABLEf, 0x0);
         }
         rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                             MEM_BLOCK_ALL, nh_index, &egr_nh);
    }
    return rv;	
}

/*
 * Function:
 *		bcm_tr_mpls_get_entry_type
 * Purpose:
 *		To get Entry_type of  EGR_L3_NEXT_HOP Entry  for a specific EGR_NH Index
 * Parameters:
 *		IN :  Unit
 *           IN :  mpls_label
 *           IN :  nh_index
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr_mpls_get_entry_type (int unit, int nh_index, uint8 *entry_type )
{
    egr_l3_next_hop_entry_t egr_nh;
    int rv=BCM_E_NONE;

    /* Retrieve EGR L3 NHOP Entry */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    *entry_type = 
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);

    return rv;
}

/*
 * Function:
 *		bcm_tr_mpls_l3_label_add
 * Purpose:
 *		Updates  Inner_LABEL within VC_AND_SWAP_TABLE for a specific EGR_NH Index
 * Parameters:
 *		IN :  Unit
 *           IN :  mpls_label
 *           IN :  nh_index
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr_mpls_l3_label_add (int unit, bcm_mpls_label_t  mpls_label, uint8 mpls_exp, int nh_index, uint32 flags)
{
    int  num_vc, rv=BCM_E_NONE;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    int      vc_swap_index=-1;
    egr_l3_next_hop_entry_t egr_nh;
    uint8    entry_type;

    /* Retrieve EGR L3 NHOP Entry */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

     /*
     * Indexes in the first half of EGR_MPLS_VC_AND_SWAP_LABEL_TABLE
     * also get used as the index into the EGR_PW_INIT_COUNTERS table.
     * Set num_vc to half the EGR_MPLS_VC_AND_SWAP_LABEL_TABLE size.
     * Choose from the non-counted section (second half) so the counted
     * section is available for VPWS.
     */

    num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
    num_vc /= 2;
    BCM_IF_ERROR_RETURN(bcm_tr_mpls_get_entry_type(unit, nh_index, &entry_type));
    if ((entry_type == 1) && (mpls_label != 0) && (flags & BCM_L3_REPLACE)) {
		/* Be sure that the existing entry is already setup to
		 * egress into an MPLS tunnel. If not, return BCM_E_PARAM.
		 */
		entry_type = 
			soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
		
		if (entry_type != 0x1) { /* != MPLS_MACDA_PROFILE */
			return BCM_E_PARAM;
		}
				
		/* Retrieve VC_AND_SWAP_INDEX */
		vc_swap_index  = 
			soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
											 VC_AND_SWAP_INDEXf);

		/* Retrieve VC_AND_SWAP_TABLE entry */
		BCM_IF_ERROR_RETURN( 
		               soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
						  MEM_BLOCK_ALL, vc_swap_index, &vc_entry));
    }else if ((entry_type == 1) && (mpls_label ==0) && (flags  & BCM_L3_REPLACE)) {
              rv = bcm_tr_mpls_l3_label_delete (unit, nh_index);
              return rv;
    } else {

		BCM_IF_ERROR_RETURN(
			_bcm_tr_mpls_get_vc_and_swap_table_index (unit, 
			                                         num_vc,  &vc_swap_index));
						
              /* Init the MPLS_VC_AND_SWAP table entry */
              sal_memset(&vc_entry, 0, 
                       sizeof(egr_mpls_vc_and_swap_label_table_entry_t));			  

    }

    if (BCM_XGS3_L3_MPLS_LBL_VALID(mpls_label)) {
	         soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
						&vc_entry, MPLS_LABELf, 
						mpls_label);
	         soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
						&vc_entry, MPLS_LABEL_ACTIONf, 
						0x1); /* PUSH one label */
    } else {
	         soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
						&vc_entry, MPLS_LABEL_ACTIONf, 
						0x0); /* NOOP */
    }

    /* For L3 VPN inner-label: Use TTL of the IP Payload */
    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
						&vc_entry, MPLS_TTLf, 0x0);

    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
						&vc_entry, CW_INSERT_FLAGf, 0x0);

    /* Use Fixed - User configured value for MPLS_EXP  */
    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
						&vc_entry, MPLS_EXP_SELECTf, 
						0x0); /* USE_FIXED */
    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
						&vc_entry, MPLS_EXPf, 
						mpls_exp);
    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
						&vc_entry, NEW_PRIf, 
						0x0);
    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
						&vc_entry, NEW_CFIf, 
						0x0);
		
    rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
				   MEM_BLOCK_ALL, vc_swap_index,
				   &vc_entry);
    if (rv < 0){
       goto cleanup;
    }


    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, 
                        &egr_nh, ENTRY_TYPEf, 
                        entry_type);

    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                        &egr_nh, VC_AND_SWAP_INDEXf,
                        vc_swap_index);

    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                        MEM_BLOCK_ALL, nh_index, &egr_nh);

    if (rv < 0) {
       goto cleanup;
    }

    return rv;

cleanup:
    if (vc_swap_index != -1) {
        if (vc_swap_index < num_vc) {
           _BCM_MPLS_VC_COUNT_USED_CLR(unit, vc_swap_index);
        } else {
           _BCM_MPLS_VC_NON_COUNT_USED_CLR(unit, (vc_swap_index - num_vc));
        }
    }
    return rv; 
}

/*
 * Function:
 *		bcm_tr_mpls_l3_label_get
 * Purpose:
 *		Retrieves the  Inner_LABEL within VC_AND_SWAP_TABLE for a specific EGR_NH Index
 * Parameters:
 *		IN :  Unit
 *           IN :  mpls_label
 *           IN :  nh_index
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr_mpls_l3_label_get (int unit, int index, bcm_l3_egress_t *egr)
{
   int rv=BCM_E_NONE;
   int		vc_swap_index=-1;
   egr_l3_next_hop_entry_t egr_nh;
   egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
   uint32	entry_type;
   uint32  label_action;

   /* Retrieve EGR L3 NHOP Entry */
   BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
									 index, &egr_nh));

   entry_type = 
	   soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);

   if (entry_type != 0x1) { /* != MPLS_MACDA_PROFILE */
	  egr->mpls_label = 0;
         return rv;
   }
  
   /* Retrieve VC_AND_SWAP_INDEX */
   vc_swap_index  = 
	   soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
										VC_AND_SWAP_INDEXf);   
   if (vc_swap_index != -1) {
        /* Retrieve VC_AND_SWAP_TABLE entry */
        BCM_IF_ERROR_RETURN( 
				  soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
					 MEM_BLOCK_ALL, vc_swap_index, &vc_entry));
        label_action = 
                soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                                    &vc_entry, MPLS_LABEL_ACTIONf);

       if (label_action == 0x1) { /* PUSH */
	      egr->flags |= BCM_L3_ROUTE_LABEL;
             egr->mpls_label = 
		   soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
							   &vc_entry, MPLS_LABELf);
       } else {
             egr->mpls_label = 0;
       }
   } else {
        egr->mpls_label = 0;
   }
   
   return rv;
}

/*
 * Function:
 *		bcm_tr_mpls_l3_label_delete
 * Purpose:
 *		Deletes  Inner_LABEL within VC_AND_SWAP_TABLE for a specific EGR_NH Index
 * Parameters:
 *		IN :  Unit
 *           IN :  nh_index
 * Returns:
 *		BCM_E_XXX
 */


int 
bcm_tr_mpls_l3_label_delete (int unit, int index)
{
    int num_vc, rv=BCM_E_NONE;
    int  vc_swap_index=-1;
    egr_l3_next_hop_entry_t egr_nh;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    uint8  entry_type;
    int  macda_index=-1;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
									  index, &egr_nh));
	
    /* Retrieve  and Verify  ENTRY_TYPE */
    entry_type = 
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
	
    if (entry_type != 0x1) { /* != MPLS_MACDA_PROFILE */
          return BCM_E_PARAM;
    }

    /* Set Entry_Type to Normal */
    entry_type = 0; /* Normal */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        ENTRY_TYPEf, entry_type);

    /* Delete MAC_DA_PROFILE_INDEX */
    macda_index = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                                          MAC_DA_PROFILE_INDEXf);

    if (macda_index != -1) {
        _bcm_mac_da_profile_entry_delete(unit, macda_index);
    }
	
    /* Retrieve VC_AND_SWAP_INDEX */
    vc_swap_index  = 
		soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
										 VC_AND_SWAP_INDEXf);

	
    /* Init the MPLS_VC_AND_SWAP table entry */
    sal_memset(&vc_entry, 0, 
                  sizeof(egr_mpls_vc_and_swap_label_table_entry_t));

	
    rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
				   MEM_BLOCK_ALL, vc_swap_index,
				   &vc_entry);

    /* Free VC_AND_SWAP_LABEL entry */
    num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
    if (vc_swap_index > num_vc) {
        _BCM_MPLS_VC_NON_COUNT_USED_CLR(unit, vc_swap_index - num_vc);
    } else {
        _BCM_MPLS_VC_COUNT_USED_CLR(unit, vc_swap_index);
    }

    vc_swap_index = 0;
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
						&egr_nh, VC_AND_SWAP_INDEXf,
						vc_swap_index);

    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                        MEM_BLOCK_ALL, index, &egr_nh);

    return rv;
}

/*
 * Function:
 *		bcm_tr_mpls_swap_nh_info_add
 * Purpose:
 *		Add  SWAP_LABEL within VC_AND_SWAP_TABLE for a specific EGR_NH Index
 * Parameters:
 *		IN :  Unit
 *           IN : Egress object Pointer
 *           IN :  nh_index
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr_mpls_swap_nh_info_add (int unit, bcm_l3_egress_t *egr, int nh_index)
{
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    int vc_swap_index=-1;
    int rv, num_vc;

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(egr->mpls_label)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &ing_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    /*
     * Indexes in the first half of EGR_MPLS_VC_AND_SWAP_LABEL_TABLE
     * also get used as the index into the EGR_PW_INIT_COUNTERS table.
     * Set num_vc to half the EGR_MPLS_VC_AND_SWAP_LABEL_TABLE size.
     * Choose from the non-counted section (second half) so the counted
     * section is available for VPWS.
     */
    num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
    num_vc /= 2;

    rv =  _bcm_tr_mpls_get_vc_and_swap_table_index (unit, 
                        num_vc,  &vc_swap_index);
    _BCM_MPLS_CLEANUP(rv);

    /* Program the MPLS_VC_AND_SWAP table entry */
    sal_memset(&vc_entry, 0, sizeof(egr_mpls_vc_and_swap_label_table_entry_t));
    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                        &vc_entry, MPLS_LABELf, 
                        egr->mpls_label);
    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                        &vc_entry, MPLS_LABEL_ACTIONf, 
                        0x2); /* SWAP */
    if (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_TTLf, egr->mpls_ttl); 
    }
    if ((egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
        (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) {

        /* Use the specified EXP, PRI and CFI */
        if ((egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) ||
            (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ||
            (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_REMARK)) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }

        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_EXP_SELECTf, 0x0); /* USE_FIXED */
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_EXPf, egr->mpls_exp); 
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, NEW_PRIf, egr->mpls_pkt_pri); 
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, NEW_CFIf, egr->mpls_pkt_cfi); 
    } else if (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
        if (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        /* Use EXP-map for EXP, PRI and CFI */
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_EXP_SELECTf, 0x1); /* USE_MAPPING */
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, &vc_entry,
                            MPLS_EXP_MAPPING_PTRf, egr->mpls_qos_map_id); 
    } else { /* default BCM_MPLS_EGRESS_LABEL_EXP_COPY */
        /* Use EXP from incoming MPLS label. If no incoming label, use the specified
         * EXP value. Use EXP-map for PRI/CFI.
         */
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_EXP_SELECTf, 0x3); /* USE_SWAP */
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_EXPf, egr->mpls_exp); 
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, &vc_entry,
                            MPLS_EXP_MAPPING_PTRf, egr->mpls_qos_map_id); 
    }
    rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                       MEM_BLOCK_ALL, vc_swap_index,
                       &vc_entry);
    _BCM_MPLS_CLEANUP(rv);


    /* Write EGR_L3_NEXT_HOP entry */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                        &egr_nh, VC_AND_SWAP_INDEXf,
                        vc_swap_index);

    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, nh_index, &egr_nh);
    _BCM_MPLS_CLEANUP(rv);

    /* Write ING_L3_NEXT_HOP entry */
    
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                       &ing_nh, ENTRY_TYPEf, 0x1); /* MPLS L3_OIF */
    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, nh_index, &ing_nh);
    _BCM_MPLS_CLEANUP(rv);

    return rv;

cleanup:

    if (vc_swap_index != -1) {
        if (vc_swap_index < num_vc) {
            _BCM_MPLS_VC_COUNT_USED_CLR(unit, vc_swap_index);
        } else {
            _BCM_MPLS_VC_NON_COUNT_USED_CLR(unit, 
                                     vc_swap_index - num_vc);
        }
    }
    return rv;
}

/*
 * Function:
 *		bcm_tr_mpls_swap_nh_info_delete
 * Purpose:
 *		Delete  SWAP_LABEL within VC_AND_SWAP_TABLE for a specific EGR_NH Index
 * Parameters:
 *		IN   :  Unit
 *           IN    :  nh_index
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr_mpls_swap_nh_info_delete(int unit, int nh_index)
{
    int rv, num_vc, old_macda_idx = -1, old_vc_swap_idx = -1;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf) != 0x1) {
        return BCM_E_NOT_FOUND;
    }
    old_macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                     MAC_DA_PROFILE_INDEXf);
    old_vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                       VC_AND_SWAP_INDEXf);


    /* Clear EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &egr_nh));

    /* Clear ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write (unit, ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &ing_nh));

    /* Free VC_AND_SWAP_LABEL entry */
    num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
    num_vc /= 2;

    if (old_vc_swap_idx >= num_vc) {
        _BCM_MPLS_VC_NON_COUNT_USED_CLR(unit, old_vc_swap_idx - num_vc);
    } else {
        _BCM_MPLS_VC_COUNT_USED_CLR(unit, old_vc_swap_idx);
    }
	
    /* clear VC_AND_SWAP_LABEL entry per customer's requirements */
    sal_memset(&vc_entry, 0, sizeof(egr_mpls_vc_and_swap_label_table_entry_t));
    rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                  MEM_BLOCK_ALL, old_vc_swap_idx,
                                  &vc_entry);

    /* Delete old MAC profile reference */
    rv = _bcm_mac_da_profile_entry_delete(unit, old_macda_idx);

    return rv;
}

/*
 * Function:
 *		bcm_tr_mpls_swap_nh_info_get
 * Purpose:
 *		Get  Label_Swap info within Egress Object for a specific EGR_NH Index
 * Parameters:
 *		IN :  Unit
 *           OUT : Egress object Pointer
 *           IN :  nh_index
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr_mpls_swap_nh_info_get(int unit, bcm_l3_egress_t *egr, int nh_index)
{
    egr_l3_next_hop_entry_t egr_nh;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    int vc_swap_idx;
    uint32	 label_action;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                   VC_AND_SWAP_INDEXf);

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                  MEM_BLOCK_ANY, vc_swap_idx, &vc_entry));

    label_action =  soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                  &vc_entry, MPLS_LABEL_ACTIONf);

    if (label_action == 0x2) { /* SWAP */
         egr->mpls_label = 
                 soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                            &vc_entry, MPLS_LABELf);
         if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                            &vc_entry, MPLS_TTLf)) {
              egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
         } else {
              egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
         }
         if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                            &vc_entry, MPLS_EXP_SELECTf) == 0x0) {
              /* Use the specified EXP, PRI and CFI */
              egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
              egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRI_SET;
              egr->mpls_exp = 
                   soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXPf);
              egr->mpls_pkt_pri = 
              soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, NEW_PRIf);
              egr->mpls_pkt_cfi = 
                   soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, NEW_CFIf);
         } else if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                   &vc_entry, MPLS_EXP_SELECTf) == 0x1) {
              /* Use EXP-map for EXP, PRI and CFI */
              egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_REMARK;
              egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
              egr->mpls_qos_map_id = 
                   soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_MAPPING_PTRf);
         } else if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                   &vc_entry, MPLS_EXP_SELECTf) == 0x2) {
              /* Use EXP from inner MPLS label. If no inner label, use the specified
                * EXP value. Use EXP-map for PRI/CFI.
                */
              egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
              egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
              egr->mpls_exp = 
                   soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXPf);
              egr->mpls_qos_map_id = 
                   soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_MAPPING_PTRf);
         } else {
              /* Use EXP from incoming MPLS label. If no incoming label, use the specified
                * EXP value. Use EXP-map for PRI/CFI.
                */
              egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
              egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
              egr->mpls_exp = 
                   soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXPf);
              egr->mpls_qos_map_id = 
                   soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_MAPPING_PTRf);
         }
    } else {
         egr->mpls_label = 0;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_tr_mpls_l3_nh_info_add(int unit, bcm_mpls_tunnel_switch_t *info, int *nh_index)
{
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_mac_da_profile_entry_t macda;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    _bcm_tr_ing_nh_info_t ing_nh_info;
    _bcm_tr_egr_nh_info_t egr_nh_info;
    bcm_l3_egress_t nh_info, nh_obj;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    uint32 nh_flags, macda_index;
    int rv, temp_nh_index = -1, num_vc=0;
    void *entries[1];

    /* Initialize values */
    if (info->mtu != 0) {
        ing_nh_info.mtu = info->mtu;
    }  else {
        ing_nh_info.mtu = 0x3FFF;
    }
	
    ing_nh_info.port = -1;
    ing_nh_info.module = -1;
    ing_nh_info.trunk = -1;

    egr_nh_info.dvp = 0;
    egr_nh_info.dvp_is_network = 0;
    egr_nh_info.entry_type = 0x1; /* MPLS_MACDA_PROFILE */
    egr_nh_info.sd_tag_action_present = 0;
    egr_nh_info.sd_tag_action_not_present = 0;
    egr_nh_info.intf_num = -1;
    egr_nh_info.sd_tag_vlan = -1;
    egr_nh_info.macda_index = -1;
    egr_nh_info.vc_swap_index = -1;
    egr_nh_info.pw_init_cnt = 0;

    if (!BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label)) {
        return BCM_E_PARAM;
    }

    /* 
     * Get egress next-hop index from egress object and
     * increment egress object reference count. 
     */
    rv = bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                            0, &temp_nh_index);
    _BCM_MPLS_CLEANUP(rv);
    sal_memset(&nh_info, 0, sizeof(bcm_l3_egress_t));

    /* Get egress next-hop info from the egress object */
    rv = bcm_xgs3_nh_get(unit, temp_nh_index, &nh_info);
    _BCM_MPLS_CLEANUP(rv);

    /* Check if Egress Object contains a valid SWAP Label */
    if (nh_info.mpls_label != 0) {
         return BCM_E_CONFIG;
    }

    /*
     * Indexes in the first half of EGR_MPLS_VC_AND_SWAP_LABEL_TABLE
     * also get used as the index into the EGR_PW_INIT_COUNTERS table.
     * Set num_vc to half the EGR_MPLS_VC_AND_SWAP_LABEL_TABLE size.
     * Choose from the non-counted section (second half) so the counted
     * section is available for VPWS.
     */
    num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
    num_vc /= 2;

    /*
     * For SWAP operation, we need to allocate a unique next-hop
     * index and populate the swap label info. Copy the destination
     * port information from the egress object.
     * 
     * Allocate a next-hop entry. By calling bcm_xgs3_nh_add()
     * with _BCM_L3_SHR_WRITE_DISABLE flag, a next-hop index is
     * allocated but nothing is written to hardware. The "nh_info"
     * in this case is not used, so just set to all zeros.
     */
    sal_memset(&nh_obj, 0, sizeof(bcm_l3_egress_t));
    nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
    BCM_IF_ERROR_RETURN(bcm_xgs3_nh_add(unit, nh_flags, &nh_obj, nh_index));
	
    rv = _bcm_tr_mpls_add_egrNhop (unit, info->egress_if);
    _BCM_MPLS_CLEANUP(rv);

    rv = _bcm_tr_mpls_add_vp_nh (unit, info->egress_if, *nh_index);
    _BCM_MPLS_CLEANUP(rv);

    /* Copy the destination port information. */
    if (nh_info.flags & BCM_L3_TGID) {
        ing_nh_info.port = -1;
        ing_nh_info.module = -1;
        ing_nh_info.trunk = nh_info.trunk; 
    } else {
        BCM_IF_ERROR_RETURN
              (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, nh_info.module, nh_info.port, 
                                                                         &mod_out, &port_out));
        ing_nh_info.port = port_out;
        ing_nh_info.module = mod_out;
        ing_nh_info.trunk = -1; 
    }
    egr_nh_info.intf_num = nh_info.intf;

    sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
    soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm, 
                         &macda, MAC_ADDRESSf, nh_info.mac_addr);
    entries[0] = &macda;
    rv = _bcm_mac_da_profile_entry_add(unit, entries, 1, &macda_index);
    egr_nh_info.macda_index = macda_index;
    _BCM_MPLS_CLEANUP(rv);

    rv =  _bcm_tr_mpls_get_vc_and_swap_table_index (unit, 
                          num_vc,  &egr_nh_info.vc_swap_index);
    _BCM_MPLS_CLEANUP(rv);

    

    /* Program the MPLS_VC_AND_SWAP table entry */
    sal_memset(&vc_entry, 0, sizeof(egr_mpls_vc_and_swap_label_table_entry_t));
    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                        &vc_entry, MPLS_LABELf, 
                        info->egress_label.label);
    soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                        &vc_entry, MPLS_LABEL_ACTIONf, 
                        0x2); /* SWAP */
    if (info->egress_label.flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_TTLf, info->egress_label.ttl); 
    }
    if ((info->egress_label.flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
        (info->egress_label.flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) {

        /* Use the specified EXP, PRI and CFI */
        if ((info->egress_label.flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) ||
            (info->egress_label.flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ||
            (info->egress_label.flags & BCM_MPLS_EGRESS_LABEL_PRI_REMARK)) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }

        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_EXP_SELECTf, 0x0); /* USE_FIXED */
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_EXPf, info->egress_label.exp); 
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, NEW_PRIf, info->egress_label.pkt_pri); 
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, NEW_CFIf, info->egress_label.pkt_cfi); 
    } else if (info->egress_label.flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
        if (info->egress_label.flags & BCM_MPLS_EGRESS_LABEL_PRI_SET) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        /* Use EXP-map for EXP, PRI and CFI */
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_EXP_SELECTf, 0x1); /* USE_MAPPING */
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, &vc_entry,
                            MPLS_EXP_MAPPING_PTRf, info->egress_label.qos_map_id); 
    } else { /* default BCM_MPLS_EGRESS_LABEL_EXP_COPY */
        /* Use EXP from incoming MPLS label. If no incoming label, use the specified
         * EXP value. Use EXP-map for PRI/CFI.
         */
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_EXP_SELECTf, 0x3); /* USE_SWAP */
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                            &vc_entry, MPLS_EXPf, info->egress_label.exp); 
        soc_mem_field32_set(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, &vc_entry,
                            MPLS_EXP_MAPPING_PTRf, info->egress_label.qos_map_id); 
    }
    rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                       MEM_BLOCK_ALL, egr_nh_info.vc_swap_index,
                       &vc_entry);
    _BCM_MPLS_CLEANUP(rv);

    /* Write EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                       &egr_nh, INTF_NUMf, egr_nh_info.intf_num);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                        &egr_nh, VC_AND_SWAP_INDEXf,
                        egr_nh_info.vc_swap_index);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                        &egr_nh, MAC_DA_PROFILE_INDEXf,
                        egr_nh_info.macda_index);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        ENTRY_TYPEf, egr_nh_info.entry_type);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        HG_MODIFY_ENABLEf, 0x1);
#if 0 
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        DVPf, egr_nh_info.dvp);
#endif
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &egr_nh);
    _BCM_MPLS_CLEANUP(rv);

    /* Write ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    if (ing_nh_info.trunk == -1) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, PORT_NUMf, ing_nh_info.port);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, MODULE_IDf, ing_nh_info.module);
    } else {    
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, Tf, 1);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, TGIDf, ing_nh_info.trunk);
    }
    
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                       &ing_nh, ENTRY_TYPEf, 0x1); /* MPLS L3_OIF */
    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &ing_nh);
    _BCM_MPLS_CLEANUP(rv);

    /* Write INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    if (ing_nh_info.trunk == -1) {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, PORT_NUMf, ing_nh_info.port);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, MODULE_IDf, ing_nh_info.module);
    } else {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, Tf, 1);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, TGIDf, ing_nh_info.trunk);
    }
    rv = soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &initial_ing_nh);
    _BCM_MPLS_CLEANUP(rv);

    return rv;

cleanup:
    (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);

    if (temp_nh_index != -1) {
        (void) bcm_xgs3_nh_del(unit, 0, temp_nh_index);
    }
    if (egr_nh_info.macda_index != -1) {
        _bcm_mac_da_profile_entry_delete(unit, egr_nh_info.macda_index);
    }
    if (egr_nh_info.vc_swap_index != -1) {
        if (egr_nh_info.vc_swap_index < num_vc) {
            _BCM_MPLS_VC_COUNT_USED_CLR(unit, 
                egr_nh_info.vc_swap_index);
        } else {
            _BCM_MPLS_VC_NON_COUNT_USED_CLR(unit, 
                egr_nh_info.vc_swap_index - num_vc);
        }
    }
    return rv;
}

STATIC int
_bcm_tr_mpls_l3_nh_info_delete(int unit, int nh_index)
{
    int rv, num_vc, old_macda_idx = -1, old_vc_swap_idx = -1;
    bcm_if_t old_egress_if = 0;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &initial_ing_nh));

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf) != 0x1) {
        return BCM_E_NOT_FOUND;
    }
    old_macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                     MAC_DA_PROFILE_INDEXf);
    old_vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                       VC_AND_SWAP_INDEXf);

    /* Find the tunnel_if */
    rv = _bcm_tr_mpls_egress_intf_find(unit, nh_index, &old_egress_if);
    BCM_IF_ERROR_RETURN (rv);

    /* Clear EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &egr_nh));

    /* Clear ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write (unit, ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &ing_nh));

    /* Clear INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &initial_ing_nh));

    /* Free VC_AND_SWAP_LABEL entry */
    num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
    num_vc /= 2;

    if (old_vc_swap_idx >= num_vc) {
        _BCM_MPLS_VC_NON_COUNT_USED_CLR(unit, old_vc_swap_idx - num_vc);
    } else {
        _BCM_MPLS_VC_COUNT_USED_CLR(unit, old_vc_swap_idx);
    }
	
    /* clear VC_AND_SWAP_LABEL entry per customer's requirements */
    sal_memset(&vc_entry, 0, sizeof(egr_mpls_vc_and_swap_label_table_entry_t));
    rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                  MEM_BLOCK_ALL, old_vc_swap_idx,
                                  &vc_entry);

    /* Delete old MAC profile reference */
    rv = _bcm_mac_da_profile_entry_delete(unit, old_macda_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Delete egress_if next-hop reference */
    rv = bcm_xgs3_nh_del(unit, 0, (old_egress_if - BCM_XGS3_EGRESS_IDX_MIN));
    BCM_IF_ERROR_RETURN(rv);

    BCM_IF_ERROR_RETURN(_bcm_tr_mpls_delete_vp_nh ( unit, nh_index, old_egress_if ));

    /* Free the next-hop entry. */
    rv = bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index);
    return rv;
}

STATIC int
_bcm_tr_mpls_l3_nh_info_get(int unit, bcm_mpls_tunnel_switch_t *info, int nh_index)
{
    egr_l3_next_hop_entry_t egr_nh;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    int rv, vc_swap_idx;

    rv = _bcm_tr_mpls_egress_intf_find(unit, nh_index, &info->egress_if);
    BCM_IF_ERROR_RETURN(rv);

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                   VC_AND_SWAP_INDEXf);
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                  MEM_BLOCK_ANY, vc_swap_idx, &vc_entry));

    info->egress_label.label = 
        soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                            &vc_entry, MPLS_LABELf);
    if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                            &vc_entry, MPLS_TTLf)) {
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    } else {
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    }
    if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                            &vc_entry, MPLS_EXP_SELECTf) == 0x0) {
        /* Use the specified EXP, PRI and CFI */
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRI_SET;
        info->egress_label.exp = 
            soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXPf);
        info->egress_label.pkt_pri = 
            soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, NEW_PRIf);
        info->egress_label.pkt_cfi = 
            soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, NEW_CFIf);
    } else if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                   &vc_entry, MPLS_EXP_SELECTf) == 0x1) {
        /* Use EXP-map for EXP, PRI and CFI */
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_REMARK;
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
        info->egress_label.qos_map_id = 
            soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_MAPPING_PTRf);
    } else if (soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                   &vc_entry, MPLS_EXP_SELECTf) == 0x2) {
        /* Use EXP from inner MPLS label. If no inner label, use the specified
         * EXP value. Use EXP-map for PRI/CFI.
         */
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
        info->egress_label.exp = 
            soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXPf);
        info->egress_label.qos_map_id = 
            soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_MAPPING_PTRf);
    } else {
        /* Use EXP from incoming MPLS label. If no incoming label, use the specified
         * EXP value. Use EXP-map for PRI/CFI.
         */
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
        info->egress_label.exp = 
            soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXPf);
        info->egress_label.qos_map_id = 
            soc_mem_field32_get(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_entry, MPLS_EXP_MAPPING_PTRf);
    }
    return BCM_E_NONE;
}

/* Convert key part of application format to HW entry. */
STATIC int
_bcm_tr_mpls_entry_set_key(int unit, bcm_mpls_tunnel_switch_t *info,
                           mpls_entry_entry_t *ment)
{
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int rv, gport_id;

    rv = _bcm_esw_gport_resolve(unit, info->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(ment, 0, sizeof(mpls_entry_entry_t));
    if (info->port == BCM_GPORT_INVALID) {
        /* Global label, mod/port not part of lookup key */
        soc_MPLS_ENTRYm_field32_set(unit, ment, MODULE_IDf, 0);
        soc_MPLS_ENTRYm_field32_set(unit, ment, PORT_NUMf, 0);
    } else if (BCM_GPORT_IS_TRUNK(info->port)) {
        soc_MPLS_ENTRYm_field32_set(unit, ment, Tf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, ment, TGIDf, trunk_id);
    } else {
        soc_MPLS_ENTRYm_field32_set(unit, ment, MODULE_IDf, mod_out);
        soc_MPLS_ENTRYm_field32_set(unit, ment, PORT_NUMf, port_out);
    }
    soc_MPLS_ENTRYm_field32_set(unit, ment, MPLS_LABELf, info->label);
    soc_MPLS_ENTRYm_field32_set(unit, ment, VALIDf, 1);
    
    return BCM_E_NONE;
}

/* Convert key part of HW entry to application format. */
STATIC int
_bcm_tr_mpls_entry_get_key(int unit, mpls_entry_entry_t *ment,
                           bcm_mpls_tunnel_switch_t *info)
{
    bcm_port_t port_in, port_out;
    bcm_module_t mod_in, mod_out;
    bcm_trunk_t trunk_id;

    port_in = soc_MPLS_ENTRYm_field32_get(unit, ment, PORT_NUMf);
    mod_in = soc_MPLS_ENTRYm_field32_get(unit, ment, MODULE_IDf);
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, Tf)) {
        trunk_id = soc_MPLS_ENTRYm_field32_get(unit, ment, TGIDf);
        BCM_GPORT_TRUNK_SET(info->port, trunk_id);
    } else if ((port_in == 0) && (mod_in == 0)) {
        /* Global label, mod/port not part of lookup key */
        info->port = BCM_GPORT_INVALID;
    } else {
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out));
        BCM_GPORT_MODPORT_SET(info->port, mod_out, port_out);
    } 
    info->label = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS_LABELf);

    return BCM_E_NONE;
}

/* Convert data part of HW entry to application format. */
STATIC int
_bcm_tr_mpls_entry_get_data(int unit, mpls_entry_entry_t *ment,
                            bcm_mpls_tunnel_switch_t *info)
{
    int rv, action, nh_index, vrf;
    bcm_if_t egress_if=0;

    action = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS_ACTION_IF_BOSf);
    switch(action) {
        case 0x2:
            info->action = BCM_MPLS_SWITCH_ACTION_POP;
            vrf = soc_MPLS_ENTRYm_field32_get(unit, ment, L3_IIFf);
            vrf -= _BCM_TR_MPLS_L3_IIF_BASE;
            _BCM_MPLS_VPN_SET(info->vpn, _BCM_MPLS_VPN_TYPE_L3, vrf);
            break;
        case 0x3:
            info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
            nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                                   NEXT_HOP_INDEXf);
            rv = _bcm_tr_mpls_get_vp_nh (unit, nh_index,&egress_if);
            if (rv == BCM_E_NONE) {
                rv = _bcm_tr_mpls_l3_nh_info_get(unit, info, nh_index);
            } else {
                rv = _bcm_tr_mpls_egress_intf_find(unit, nh_index, &info->egress_if);
                info->egress_label.label = 0;
            }
            BCM_IF_ERROR_RETURN(rv);
            break;
        case 0x4:
            info->action = BCM_MPLS_SWITCH_ACTION_PHP;
            nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                                   NEXT_HOP_INDEXf);
            info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN;
        case 0x5:
            info->action = BCM_MPLS_SWITCH_ACTION_PHP;
            nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment,
                                                   ECMP_PTRf);
            info->egress_if = nh_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN;
            break;
        default:
            return BCM_E_INTERNAL;
            break;
    }
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, PW_TERM_NUM_VALIDf)) {
        info->flags |= BCM_MPLS_SWITCH_COUNTED;
    }
    if (!soc_MPLS_ENTRYm_field32_get(unit, ment, DECAP_USE_TTLf)) {
        info->flags |= BCM_MPLS_SWITCH_INNER_TTL;
    }
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, DECAP_USE_EXP_FOR_INNERf)) {
        info->flags |= BCM_MPLS_SWITCH_INNER_EXP;
    }
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                    DECAP_USE_EXP_FOR_PRIf) == 0x1) {

        /* Use specified EXP-map to determine internal prio/color */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_MAP;
        info->exp_map = 
            soc_MPLS_ENTRYm_field32_get(unit, ment, EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    } else if (soc_MPLS_ENTRYm_field32_get(unit, ment, 
                                           DECAP_USE_EXP_FOR_PRIf) == 0x2) {

        /* Use the specified internal priority value */
        info->flags |= BCM_MPLS_SWITCH_INT_PRI_SET;
        info->int_pri =
            soc_MPLS_ENTRYm_field32_get(unit, ment, NEW_PRIf);

        /* Use specified EXP-map to determine internal color */
        info->flags |= BCM_MPLS_SWITCH_COLOR_MAP;
        info->exp_map = 
            soc_MPLS_ENTRYm_field32_get(unit, ment, EXP_MAPPING_PTRf);
        info->exp_map |= _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_add
 * Purpose:
 *      Add an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_tunnel_switch_add(int unit, bcm_mpls_tunnel_switch_t *info)
{
    mpls_entry_entry_t ment;
    int mode, nh_index = -1, vrf, rv, num_pw_term, old_pw_cnt = -1, pw_cnt = -1;
    int multipath = 0, index, action = -1, old_nh_index = -1, old_ecmp_index = -1;
    int  tunnel_switch_update=0;

    rv = bcm_xgs3_l3_egress_mode_get(unit, &mode);
    BCM_IF_ERROR_RETURN(rv);
    if (!mode) {
        soc_cm_debug(DK_L3, "L3 egress mode must be set first\n");
        return BCM_E_DISABLED;
    }

    /* Check for Port_independent Label mapping */
    rv = _bcm_tr_mpls_port_independent_range (unit, info->label, info->port);
    if (rv < 0) {
         return rv;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr_mpls_entry_set_key(unit, info, &ment));

    /* See if entry already exists */
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);

    if (rv == SOC_E_NONE) {
        /* Entry exists, save old info */
	 tunnel_switch_update = 1;
        action = soc_MPLS_ENTRYm_field32_get(unit, &ment, MPLS_ACTION_IF_BOSf);
        if ((action == 0x3) || (action == 0x4)) {
            /* SWAP_NHI or L3_NHI */
            old_nh_index = soc_MPLS_ENTRYm_field32_get(unit, &ment, NEXT_HOP_INDEXf);
        } else if (action == 0x5) {
            /* L3_ECMP */
            old_ecmp_index = soc_MPLS_ENTRYm_field32_get(unit, &ment, ECMP_PTRf);
        }

        if (soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUM_VALIDf)) {
            old_pw_cnt = soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUMf);
        } 
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    }

    switch(info->action) {
        case BCM_MPLS_SWITCH_ACTION_SWAP:
            if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
                return BCM_E_PARAM;
            }
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_NOT_BOSf,
                                        0x3); /* SWAP_NHI */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf,
                                        0x3); /* SWAP NHI */
            break;
        case BCM_MPLS_SWITCH_ACTION_POP:
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_NOT_BOSf,
                                        0x1); /* POP */
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf,
                                        0x2); /* L3_IIF */ 
            break;
        case BCM_MPLS_SWITCH_ACTION_PHP:
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_NOT_BOSf,
                                        0x2); /* PHP_NHI */
            if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, info->egress_if)) {
                multipath = 1;
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf,
                                            0x5); /* L3_ECMP */ 
            } else if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egress_if)) {
                soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf,
                                            0x4); /* L3_NHI */ 
            } else {
                return BCM_E_PARAM;
            }
            break;
        default:
            return BCM_E_PARAM;
            break;
    }
    if (info->action == BCM_MPLS_SWITCH_ACTION_POP) {
        if (_BCM_MPLS_VPN_IS_L3(info->vpn)) {
            vrf = _BCM_MPLS_VPN_ID_GET(info->vpn);
            if (!_BCM_MPLS_VRF_USED_GET(unit, vrf)) {
                return BCM_E_PARAM;
            }
            soc_MPLS_ENTRYm_field32_set(unit, &ment, L3_IIFf, 
                                        _BCM_TR_MPLS_L3_IIF_BASE + vrf);
        }
    } else if (info->action == BCM_MPLS_SWITCH_ACTION_PHP) {
        bcm_l3_egress_t nh_info;
        egr_l3_next_hop_entry_t egr_nh;
        egr_mac_da_profile_entry_t macda;
        uint32 macda_index=-1;
        void *entries[1];

       /*
         * Get egress next-hop index from egress object and
         * increment egress object reference count.
         */

        BCM_IF_ERROR_RETURN(bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                                0, &nh_index));

        /* Fix: Entry_Type = 1, for PHP Packets with more than 1 Label */
        /* Read the egress next_hop entry pointed by Egress-Object */   
        BCM_IF_ERROR_RETURN(bcm_xgs3_nh_get(unit, nh_index, &nh_info));
        sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
        soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm,
                             &macda, MAC_ADDRESSf, nh_info.mac_addr);
        entries[0] = &macda;
        BCM_IF_ERROR_RETURN(_bcm_mac_da_profile_entry_add(unit, entries, 1,
                                                                 (uint32 *) &macda_index));
		
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                                                                 nh_index, &egr_nh);
        if (rv < 0) {
            if (macda_index != -1) {
                 (void) _bcm_mac_da_profile_entry_delete(unit, macda_index);
	     }
        }
	   
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             MAC_DA_PROFILE_INDEXf, macda_index);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                             ENTRY_TYPEf, 1);

        rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                             MEM_BLOCK_ALL, nh_index, &egr_nh);
        if (rv < 0) {
            if (macda_index != -1) {
                 (void) _bcm_mac_da_profile_entry_delete(unit, macda_index);
	     }
        }

        if (multipath) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, ECMP_PTRf, nh_index);
        } else {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, NEXT_HOP_INDEXf, nh_index);
        }
    } else { /* BCM_MPLS_SWITCH_ACTION_SWAP */
         if (BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label)) {         
              rv = _bcm_tr_mpls_l3_nh_info_add(unit, info, &nh_index);
              if (rv < 0) {
                   goto cleanup;
              }
         } else {
              rv = bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                       0, &nh_index);
              if (rv < 0) {
                   return rv;
              }
         }
         soc_MPLS_ENTRYm_field32_set(unit, &ment, NEXT_HOP_INDEXf, nh_index);
    }/* BCM_MPLS_SWITCH_ACTION_SWAP */

    
    soc_MPLS_ENTRYm_field32_set(unit, &ment, V4_ENABLEf, 1);
    soc_MPLS_ENTRYm_field32_set(unit, &ment, V6_ENABLEf, 1);
    if (info->flags & BCM_MPLS_SWITCH_INNER_TTL) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_TTLf, 0);
    } else {
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_TTLf, 1);
    }
    if (info->flags & BCM_MPLS_SWITCH_INNER_EXP) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_INNERf, 0);
    } else {
        /* For SWAP, Do-not PUSH EXP */
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
             soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_INNERf, 0);
        } else {
             soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_INNERf, 1);
        }
    }
    if (info->flags & BCM_MPLS_SWITCH_INT_PRI_MAP) {
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_PRIf, 
                                    0x1); /* MAP_PRI_AND_CNG */
        /* Use specified EXP-map to determine internal prio/color */
        soc_MPLS_ENTRYm_field32_set(unit, &ment, EXP_MAPPING_PTRf, 
            info->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
    } else if (info->flags & BCM_MPLS_SWITCH_INT_PRI_SET) {
        soc_MPLS_ENTRYm_field32_set(unit, &ment, DECAP_USE_EXP_FOR_PRIf, 
                                    0x2); /* NEW_PRI_MAP_CNG */
        /* Use the specified internal priority value */
        soc_MPLS_ENTRYm_field32_set(unit, &ment, NEW_PRIf, 
                                    info->int_pri);
        /* Use specified EXP-map to determine internal color */
        soc_MPLS_ENTRYm_field32_set(unit, &ment, EXP_MAPPING_PTRf, 
            info->exp_map & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
    }
    if ((info->flags & BCM_MPLS_SWITCH_COUNTED) && 
        SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {
        if (old_pw_cnt == -1) {
            num_pw_term = soc_mem_index_count(unit, ING_PW_TERM_COUNTERSm);
            for (pw_cnt = 0; pw_cnt < num_pw_term; pw_cnt++) {
                if (!_BCM_MPLS_PW_TERM_USED_GET(unit, pw_cnt)) {
                    break;
                }
            }
            if (pw_cnt == num_pw_term) {
                rv = BCM_E_RESOURCE;
                goto cleanup;
            }
            _BCM_MPLS_PW_TERM_USED_SET(unit, pw_cnt);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, PW_TERM_NUMf, pw_cnt);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, PW_TERM_NUM_VALIDf, 1);
        }
    }

   if (!tunnel_switch_update) {
      rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &ment);
   } else {
      rv = soc_mem_write(unit, MPLS_ENTRYm,
						  MEM_BLOCK_ALL, index,
						  &ment);
   }
	
    if (rv < 0) {
         goto cleanup;
    }
 	 
    /* Clean up old next-hop and counter info if entry was replaced */
    if ((old_pw_cnt != -1) && !(info->flags & BCM_MPLS_SWITCH_COUNTED)) {
        _BCM_MPLS_PW_TERM_USED_CLR(unit, old_pw_cnt);
    }
    if (action ==  0x3) { /* SWAP_NHI */
         if (BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label)) {
              rv = _bcm_tr_mpls_l3_nh_info_delete(unit, old_nh_index);
         }
    } else if (action == 0x4) {
        /* L3_NHI */
        rv = bcm_xgs3_nh_del(unit, 0, old_nh_index);
    } else if (action == 0x5) {
        /* L3_ECMP */
        rv = bcm_xgs3_ecmp_group_del(unit, old_ecmp_index);
    }
    if (rv < 0) {
         goto cleanup;
    }

    return rv;

cleanup:
    if (pw_cnt != -1) {
        _BCM_MPLS_PW_TERM_USED_CLR(unit, pw_cnt);
    }
    if (nh_index != -1) {
        if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {
              if (BCM_XGS3_L3_MPLS_LBL_VALID(info->egress_label.label)) {
                   (void) _bcm_tr_mpls_l3_nh_info_delete(unit, nh_index);
              }
        } else if (info->action == BCM_MPLS_SWITCH_ACTION_PHP) {
            (void) bcm_xgs3_nh_del(unit, 0, nh_index);
        }
    }
    return rv;
}

STATIC int
_bcm_tr_mpls_entry_delete(int unit, mpls_entry_entry_t *ment)
{   
    ing_pw_term_counters_entry_t pw_cnt_entry;
    int rv, action, ecmp_index = -1, nh_index = -1, pw_cnt = -1;
    bcm_if_t  egress_if=0;
	
    if (soc_MPLS_ENTRYm_field32_get(unit, ment, PW_TERM_NUM_VALIDf)) {
        pw_cnt = soc_MPLS_ENTRYm_field32_get(unit, ment, PW_TERM_NUMf);
    }

    action = soc_MPLS_ENTRYm_field32_get(unit, ment, MPLS_ACTION_IF_BOSf);
    if ((action == 0x3) || (action == 0x4)) {
        /* SWAP_NHI or L3_NHI */
        nh_index = soc_MPLS_ENTRYm_field32_get(unit, ment, NEXT_HOP_INDEXf);
    } else if (action == 0x5) {
        /* L3_ECMP */
        ecmp_index = soc_MPLS_ENTRYm_field32_get(unit, ment, ECMP_PTRf);
    }
        
    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, ment);
    if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
	return rv;
    }

    if (pw_cnt != -1) {
        sal_memset(&pw_cnt_entry, 0, sizeof(ing_pw_term_counters_entry_t));
        (void) WRITE_ING_PW_TERM_COUNTERSm(unit, MEM_BLOCK_ALL, pw_cnt,
                                           &pw_cnt_entry);
        _BCM_MPLS_PW_TERM_USED_CLR(unit, pw_cnt);
    }

    if (action == 0x3) {
        /* SWAP_NHI */
       /* Check if tunnel_switch.egress_label mode is being used */
       rv = _bcm_tr_mpls_get_vp_nh (unit, (bcm_if_t) nh_index, &egress_if);
        if (rv == BCM_E_NONE) {
            rv = _bcm_tr_mpls_l3_nh_info_delete(unit, nh_index);
        } else {
            rv = BCM_E_NONE;
        }
    } else if (action == 0x4) {
        /* L3_NHI */
        rv = bcm_xgs3_nh_del(unit, 0, nh_index);
    } else if (action == 0x5) {
        /* L3_ECMP */
        rv = bcm_xgs3_ecmp_group_del(unit, ecmp_index);
    }
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_delete
 * Purpose:
 *      Delete an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_tunnel_switch_delete(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int rv, index;
    mpls_entry_entry_t ment;


    rv = _bcm_tr_mpls_entry_set_key(unit, info, &ment);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);
    if (rv < 0) {
        return rv;
    }
    rv = _bcm_tr_mpls_entry_delete(unit, &ment);
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_delete_all
 * Purpose:
 *      Delete all MPLS label entries.
 * Parameters:
 *      unit   - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_tunnel_switch_delete_all(int unit)
{
    int rv, i, num_entries;
    mpls_entry_entry_t ment;


    

    num_entries = soc_mem_index_count(unit, MPLS_ENTRYm);
    for (i = 0; i < num_entries; i++) {
        rv = READ_MPLS_ENTRYm(unit, MEM_BLOCK_ANY, i, &ment);
        if (rv < 0) {
            return rv;
        }
        if (!soc_MPLS_ENTRYm_field32_get(unit, &ment, VALIDf)) {
            continue;
        }
        if (soc_MPLS_ENTRYm_field32_get(unit, &ment,
                                        MPLS_ACTION_IF_BOSf) == 0x2) {
            /* L2_SVP */
            continue;
        }
        rv = _bcm_tr_mpls_entry_delete(unit, &ment);
        if (rv < 0) {
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_get
 * Purpose:
 *      Get an MPLS label entry.
 * Parameters:
 *      unit - Device Number
 *      info - Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_tunnel_switch_get(int unit, bcm_mpls_tunnel_switch_t *info)
{
    int rv, index;
    mpls_entry_entry_t ment;

    rv = _bcm_tr_mpls_entry_set_key(unit, info, &ment);


    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);

    if (rv < 0) {
        return rv;
    }
    rv = _bcm_tr_mpls_entry_get_data(unit, &ment, info);
	
    return rv;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_traverse
 * Purpose:
 *      Traverse all valid MPLS label entries an call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per MPLS entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_tunnel_switch_traverse(int unit, 
                                   bcm_mpls_tunnel_switch_traverse_cb cb,
                                   void *user_data)
{
    int rv, i, num_entries;
    mpls_entry_entry_t ment;
    bcm_mpls_tunnel_switch_t info;


    

    num_entries = soc_mem_index_count(unit, MPLS_ENTRYm);
    for (i = 0; i < num_entries; i++) {
        BCM_IF_ERROR_RETURN (READ_MPLS_ENTRYm(unit, MEM_BLOCK_ANY, i, &ment));
        if (!soc_MPLS_ENTRYm_field32_get(unit, &ment, VALIDf)) {
            continue;
        }
        if (soc_MPLS_ENTRYm_field32_get(unit, &ment,
                                        MPLS_ACTION_IF_BOSf) == 0x2) {
            /* L2_SVP */
            continue;
        }
        sal_memset(&info, 0, sizeof(bcm_mpls_tunnel_switch_t));
        rv = _bcm_tr_mpls_entry_get_key(unit, &ment, &info);
        if (rv < 0) {
            return rv;
        }
        rv = _bcm_tr_mpls_entry_get_data(unit, &ment, &info);
        if (rv < 0) {
            return rv;
        }
        cb(unit, &info, user_data);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_exp_map_create
 * Purpose:
 *      Create an MPLS EXP map instance.
 * Parameters:
 *      unit        - (IN)  SOC unit #
 *      flags       - (IN)  MPLS flags
 *      exp_map_id  - (OUT) Allocated EXP map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_exp_map_create(int unit, uint32 flags, int *exp_map_id)
{
    int i, table_num, num_exp_map;

    /* Check for unsupported Flag */
    if (flags & (~(BCM_MPLS_EXP_MAP_WITH_ID |
                   BCM_MPLS_EXP_MAP_INGRESS |
                   BCM_MPLS_EXP_MAP_EGRESS ))) {
        return BCM_E_PARAM;
    }

    /* Input parameters check. */
    if (NULL == exp_map_id) {
         return BCM_E_PARAM;
    }

    if (flags & BCM_MPLS_EXP_MAP_INGRESS) {
        /* ING_MPLS_EXP_MAPPING profiles indexed by EXP */
        num_exp_map = soc_mem_index_count(unit, ING_MPLS_EXP_MAPPINGm) / 8;

        if (flags & BCM_MPLS_EXP_MAP_WITH_ID) {
            table_num = (*exp_map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
            if (table_num > num_exp_map) {
                return BCM_E_PARAM;
            }
            if (_BCM_MPLS_ING_EXP_MAP_USED_GET(unit, table_num) &&
                !(flags & BCM_MPLS_UPDATE)) {
                return BCM_E_EXISTS;
            }
            _BCM_MPLS_ING_EXP_MAP_USED_SET(unit, table_num);
            return BCM_E_NONE;
        }

        for (i = 0; i < num_exp_map; i++) {
            if (!_BCM_MPLS_ING_EXP_MAP_USED_GET(unit, i)) {
                _BCM_MPLS_ING_EXP_MAP_USED_SET(unit, i);
                *exp_map_id = (i | _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS);
                return BCM_E_NONE;
            }
        }

    } else if (flags & BCM_MPLS_EXP_MAP_EGRESS) {
        /* EGR_MPLS_EXP_MAPPING_1 profiles indexed by ((int_pri << 2) + cng) */
        num_exp_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) >> 6;

        if (flags & BCM_MPLS_EXP_MAP_WITH_ID) {
            table_num = (*exp_map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK);
            if (table_num > num_exp_map) {
                return BCM_E_PARAM;
            }
            if (_BCM_MPLS_EXP_MAP_USED_GET(unit, table_num) &&
                !(flags & BCM_MPLS_UPDATE)) {
                return BCM_E_EXISTS;
            }
            _BCM_MPLS_EXP_MAP_USED_SET(unit, table_num);
            return BCM_E_NONE;
        }

        for (i = 0; i < num_exp_map; i++) {
            if (!_BCM_MPLS_EXP_MAP_USED_GET(unit, i)) {
                _BCM_MPLS_EXP_MAP_USED_SET(unit, i);
                *exp_map_id = (i | _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_EGRESS);
                return BCM_E_NONE;
            }
        }
    }else {
           return BCM_E_PARAM;
    }
    return BCM_E_FULL;
}

/*
 * Function:
 *      bcm_mpls_exp_map_destroy
 * Purpose:
 *      Destroy an existing MPLS EXP map instance.
 * Parameters:
 *      unit       - (IN) SOC unit #
 *      exp_map_id - (IN) EXP map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_exp_map_destroy(int unit, int exp_map_id)
{
    int num_exp_map, table_num;


    if (exp_map_id < 0) {
        return BCM_E_PARAM;
    }
    table_num = exp_map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK;

    if ((exp_map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_MASK) == 
        _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {

        /* ING_MPLS_EXP_MAPPING profiles indexed by EXP */
        num_exp_map = soc_mem_index_count(unit, ING_MPLS_EXP_MAPPINGm) / 8;
        if (table_num >= num_exp_map) {
            return BCM_E_PARAM;
        }
        if (!_BCM_MPLS_ING_EXP_MAP_USED_GET(unit, table_num)) {
            return BCM_E_NOT_FOUND;
        }
        _BCM_MPLS_ING_EXP_MAP_USED_CLR(unit, table_num);
    } else if ((exp_map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_MASK) == 
        _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_EGRESS) {
        /* EGR_MPLS_EXP_MAPPING_1 profiles indexed by ((int_pri << 2) + cng) */
        num_exp_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) >> 6;

        if (table_num >= num_exp_map) {
            return BCM_E_PARAM;
        }
        if (!_BCM_MPLS_EXP_MAP_USED_GET(unit, table_num)) {
            return BCM_E_NOT_FOUND;
        }
        _BCM_MPLS_EXP_MAP_USED_CLR(unit, table_num);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_exp_map_destroy_all
 * Purpose:
 *      Destroy all existing MPLS EXP map instances.
 * Parameters:
 *      unit       - (IN) SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_exp_map_destroy_all(int unit)
{
    int i, num_exp_map;


    /* ING_MPLS_EXP_MAPPING profiles indexed by EXP */
    num_exp_map = soc_mem_index_count(unit, ING_MPLS_EXP_MAPPINGm) / 8;
    for (i = 0; i < num_exp_map; i++) {
        if (_BCM_MPLS_ING_EXP_MAP_USED_GET(unit, i)) {
            _BCM_MPLS_ING_EXP_MAP_USED_CLR(unit, i);
        }
    }

    /* EGR_MPLS_EXP_MAPPING_1 profiles indexed by ((int_pri << 2) + cng) */
    num_exp_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) >> 6;

    for (i = 0; i < num_exp_map; i++) {
        if (_BCM_MPLS_EXP_MAP_USED_GET(unit, i)) {
            _BCM_MPLS_EXP_MAP_USED_CLR(unit, i);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mpls_exp_map_set
 * Purpose:
 *      Set the mapping of { internal priority, color }
 *      to a EXP value for MPLS headers
 *      in the specified EXP map instance.
 * Parameters:
 *      unit        - (IN) SOC unit #
 *      exp_map_id  - (IN) EXP map ID
 *      map         - (IN) EXP map config structure 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_exp_map_set(int unit, int exp_map_id,
                        bcm_mpls_exp_map_t *map)
{
    int table_num, num_exp_map, cng, rv, index;
    egr_mpls_pri_mapping_entry_t pri_entry;
    egr_mpls_exp_pri_mapping_entry_t exp_pri_entry;
    ing_mpls_exp_mapping_entry_t ing_pri_entry;

    /* Check validity of exp_map_id */
    if (exp_map_id < 0) {
        return BCM_E_PARAM;
    }

    if (!map) {
        /* invalid pointer to map data */
        return BCM_E_PARAM;
    }

    table_num = exp_map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK;

    if ((exp_map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_MASK) == 
        _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {

        /* ING_MPLS_EXP_MAPPING profiles indexed by EXP */
        num_exp_map = soc_mem_index_count(unit, ING_MPLS_EXP_MAPPINGm) / 8;

        if (!_BCM_MPLS_ING_EXP_MAP_USED_GET(unit, table_num)) {
            return BCM_E_NOT_FOUND;
        }
 
        cng = _BCM_COLOR_ENCODING(unit, map->color);
 
        /* Get the base index for this EXP map */
        index = (table_num * 8) + map->exp; 
 
        /* Commit to HW */
        BCM_IF_ERROR_RETURN(READ_ING_MPLS_EXP_MAPPINGm(unit, MEM_BLOCK_ANY,
                                                       index, &ing_pri_entry));
        soc_ING_MPLS_EXP_MAPPINGm_field32_set(unit, &ing_pri_entry, 
                                              PRIf, map->priority);
        soc_ING_MPLS_EXP_MAPPINGm_field32_set(unit, &ing_pri_entry, 
                                              CNGf, cng);
        BCM_IF_ERROR_RETURN(WRITE_ING_MPLS_EXP_MAPPINGm(unit, MEM_BLOCK_ALL, 
                                                        index, &ing_pri_entry));
    	} else if 
            ((exp_map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_MASK) == 
        _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_EGRESS) {
        /* EGR_MPLS_EXP_MAPPING_1 profiles indexed by ((int_pri << 2) + cng) */
        num_exp_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) >> 6;

        if ((map == NULL) || (table_num >= num_exp_map) ||
            (map->priority < BCM_PRIO_MIN) || (map->priority > BCM_PRIO_MAX) ||
            (map->exp > 7) || (map->pkt_pri > 7) || (map->pkt_cfi > 1)) {
            return BCM_E_PARAM;
        }

        if (!_BCM_MPLS_EXP_MAP_USED_GET(unit, table_num)) {
            return BCM_E_NOT_FOUND;
        }

        cng = _BCM_COLOR_ENCODING(unit, map->color);

        /* Get the base index for this EXP map */
        index = (table_num << 6);

        /* Add the offset based on priority and color values */
        index += ((map->priority << 2) | (cng & 3));

        /* Commit to HW */
        rv = soc_mem_field32_modify(unit, EGR_MPLS_EXP_MAPPING_1m, 
                                    index, MPLS_EXPf, map->exp);
        BCM_IF_ERROR_RETURN(rv);
    
        rv = soc_mem_field32_modify(unit, EGR_MPLS_EXP_MAPPING_2m, 
                                    index, MPLS_EXPf, map->exp);
        BCM_IF_ERROR_RETURN(rv);

        BCM_IF_ERROR_RETURN(READ_EGR_MPLS_PRI_MAPPINGm(unit, MEM_BLOCK_ANY,
                                                       index, &pri_entry));
        soc_EGR_MPLS_PRI_MAPPINGm_field32_set(unit, &pri_entry, 
                                              NEW_PRIf, map->pkt_pri);
        soc_EGR_MPLS_PRI_MAPPINGm_field32_set(unit, &pri_entry, 
                                              NEW_CFIf, map->pkt_cfi);
        BCM_IF_ERROR_RETURN(WRITE_EGR_MPLS_PRI_MAPPINGm(unit, MEM_BLOCK_ALL, 
                                                        index, &pri_entry));

        BCM_IF_ERROR_RETURN(READ_EGR_MPLS_EXP_PRI_MAPPINGm(unit, MEM_BLOCK_ANY,
                                                        index, &exp_pri_entry));
        soc_EGR_MPLS_PRI_MAPPINGm_field32_set(unit, &exp_pri_entry, 
                                                        NEW_PRIf, map->pkt_pri);
        soc_EGR_MPLS_PRI_MAPPINGm_field32_set(unit, &exp_pri_entry, 
                                                        NEW_CFIf, map->pkt_cfi);
        BCM_IF_ERROR_RETURN(WRITE_EGR_MPLS_EXP_PRI_MAPPINGm(unit, MEM_BLOCK_ALL, 
                                                        index, &exp_pri_entry));

    }else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}
    
/*
 * Function:
 *      bcm_mpls_exp_map_get
 * Purpose:
 *      Get the mapping of { internal priority, color }
 *      to a EXP value for MPLS headers
 *      in the specified EXP map instance.
 * Parameters:
 *      unit        - (IN)  SOC unit #
 *      exp_map_id  - (IN)  EXP map ID
 *      map         - (OUT) EXP map config structure 
 * Returns:
 *      BCM_E_XXX
 */     
int     
bcm_tr_mpls_exp_map_get(int unit, int exp_map_id,
                        bcm_mpls_exp_map_t *map)
{
    int num_exp_map, cng, index, table_num;
    egr_mpls_pri_mapping_entry_t pri_entry;
    egr_mpls_exp_mapping_1_entry_t exp_entry;
    ing_mpls_exp_mapping_entry_t ing_pri_entry;


    if (exp_map_id < 0) {
        return BCM_E_PARAM;
    }

    table_num = exp_map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK;

    if ((exp_map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_MASK) == 
        _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {

        /* ING_MPLS_EXP_MAPPING profiles indexed by EXP */
        num_exp_map = soc_mem_index_count(unit, ING_MPLS_EXP_MAPPINGm) / 8;

        if ((map == NULL) || (table_num >= num_exp_map)) {
            return BCM_E_PARAM;
        }
        if (!_BCM_MPLS_ING_EXP_MAP_USED_GET(unit, table_num)) {
            return BCM_E_NOT_FOUND;
        }

        /* Get the base index for this EXP map */
        index = (table_num * 8) + map->exp; 

        /* Commit to HW */
        BCM_IF_ERROR_RETURN(READ_ING_MPLS_EXP_MAPPINGm(unit, MEM_BLOCK_ANY,
                                                       index, &ing_pri_entry));
        map->priority = soc_mem_field32_get(unit, ING_MPLS_EXP_MAPPINGm, 
                                            &ing_pri_entry, PRIf);
        cng = soc_mem_field32_get(unit, ING_MPLS_EXP_MAPPINGm,
                                  &ing_pri_entry, CNGf);
        map->color = _BCM_COLOR_DECODING(unit, cng);
    } else {
        /* EGR_MPLS_EXP_MAPPING_1 profiles indexed by ((int_pri << 2) + cng) */
        num_exp_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) >> 6;
    
        if ((map == NULL) || (table_num >= num_exp_map) ||
            (map->priority < BCM_PRIO_MIN) || (map->priority > BCM_PRIO_MAX)) {
            return BCM_E_PARAM;
        }

        if (!_BCM_MPLS_EXP_MAP_USED_GET(unit, table_num)) {
            return BCM_E_NOT_FOUND;
        }
        cng = _BCM_COLOR_ENCODING(unit, map->color);

        /* Get the base index for this EXP map */
        index = (table_num << 6);

        /* Add the offset based on priority and color values */
        index += ((map->priority << 2) | (cng & 3));

        /* Get the values from HW */
        BCM_IF_ERROR_RETURN(READ_EGR_MPLS_EXP_MAPPING_1m(unit, MEM_BLOCK_ANY,
                                                         index, &exp_entry));
        map->exp = soc_mem_field32_get(unit, EGR_MPLS_EXP_MAPPING_1m, 
                                       &exp_entry, MPLS_EXPf);
    
        BCM_IF_ERROR_RETURN(READ_EGR_MPLS_PRI_MAPPINGm(unit, MEM_BLOCK_ANY,
                                                       index, &pri_entry));
        map->pkt_pri = soc_mem_field32_get(unit, EGR_MPLS_PRI_MAPPINGm, 
                                           &pri_entry, NEW_PRIf);
        map->pkt_cfi = soc_mem_field32_get(unit, EGR_MPLS_PRI_MAPPINGm, 
                                           &pri_entry, NEW_CFIf);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr_mpls_label_stat_get
 * Purpose:
 *      Get L2 MPLS PW Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 *      val    - (OUT) 64-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */     

int
bcm_tr_mpls_label_stat_get(int unit, bcm_mpls_label_t label, bcm_gport_t port,
                           bcm_mpls_stat_t stat, uint64 *val)
{
    int rv=BCM_E_UNAVAIL, pw_term_cnt, pw_init_cnt, index, val_high, val_low,seq_num,pkt_counter;
    bcm_mpls_tunnel_switch_t info;
    mpls_entry_entry_t ment;
    ing_pw_term_counters_entry_t cent;
    egr_pw_init_counters_entry_t pw_init_entry;
    egr_l3_next_hop_entry_t egr_nh;
    ing_dvp_table_entry_t dvp;
    int vp, nh_index;
    uint32  hw_val[2];

   /* Based on Switch Type */
  if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit)) {
       bcm_port_stat_t flex_stat=0;
	
       COMPILER_64_ZERO(*val);
       switch (stat) {
           case bcmMplsInPkts:
                   flex_stat = bcmPortStatIngressPackets;
                   break;
           case bcmMplsInBytes:
                   flex_stat = bcmPortStatIngressBytes;
                   break;
           case bcmMplsOutPkts:
                   flex_stat = bcmPortStatEgressPackets;
                   break;
           case bcmMplsOutBytes:
                   flex_stat = bcmPortStatEgressBytes;
                   break;
       }

	rv = bcm_esw_port_stat_get(unit, port, flex_stat, val);
  } else if (SOC_IS_TR_VL(unit)) {
   
    if ((stat == bcmMplsInBytes) || (stat == bcmMplsInPkts)) {
         sal_memset(&info, 0, sizeof(bcm_mpls_tunnel_switch_t));
         info.port = port;
         info.label = label;

         rv = _bcm_tr_mpls_entry_set_key(unit, &info, &ment);
         BCM_IF_ERROR_RETURN(rv);

         rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);
	
         if (rv == SOC_E_NONE) {
              if (!soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUM_VALIDf)) {
                        return BCM_E_CONFIG;
              }
              pw_term_cnt = soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUMf);
			  
              if (SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {
                   rv = READ_ING_PW_TERM_COUNTERSm(unit, MEM_BLOCK_ANY, pw_term_cnt, &cent);
                   if (rv < 0) {
                        return rv;
                   }
              } else {
                        return BCM_E_UNAVAIL;
              }
              COMPILER_64_ZERO(*val);
              switch (stat) {
                  case bcmMplsInBytes:
                      if(SOC_IS_ENDURO(unit)) {
                          hw_val[0] = hw_val[1] = 0;
                          soc_mem_field_get(unit, ING_PW_TERM_COUNTERSm, 
                                            (uint32 *) &cent, BYTE_COUNTERf, hw_val);
                          COMPILER_64_SET(*val,  hw_val[1],hw_val[0]);
                      }
                      else {
                          val_high = soc_mem_field32_get(unit, ING_PW_TERM_COUNTERSm, 
                                                          &cent, BYTE_COUNTER_HIf);
                          val_low  = soc_mem_field32_get(unit, ING_PW_TERM_COUNTERSm, 
                                                         &cent, BYTE_COUNTER_LOf);
                          COMPILER_64_SET(*val, val_high, val_low);
                      }
                      break;

                  case bcmMplsInPkts:
                      val_low  = soc_mem_field32_get(unit, ING_PW_TERM_COUNTERSm, 
                                                     &cent, PACKET_COUNTERf);
                      COMPILER_64_SET(*val, 0, val_low);
                      break;

                  default:
                      break;
              }
         }
    }else if ((stat == bcmMplsOutBytes) || (stat == bcmMplsOutPkts) ) {

         /* Get egress next-hop index from MPLS GPORT */
         vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
		 
         rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
         if (rv < 0) {
	     return rv;
         }
         nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, 
                                       NEXT_HOP_INDEXf);
     
         /* Retrieve EGR L3 NHOP Entry */
         BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

         /* Retrieve PW_INIT Index from EGR L3 NHOP Entry */
         pw_init_cnt = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, PW_INIT_NUMf);

        /* Retrieve the Counter Value */
         sal_memset(&pw_init_entry, 0, sizeof(egr_pw_init_counters_entry_t));
         if (SOC_MEM_IS_VALID(unit, EGR_PW_INIT_COUNTERSm)) {	
              rv = READ_EGR_PW_INIT_COUNTERSm(unit, MEM_BLOCK_ANY, pw_init_cnt, &pw_init_entry);
              if (rv < 0) {
                   return rv;
              }
         } else {
               return BCM_E_UNAVAIL;
         }
        COMPILER_64_ZERO(*val);
         switch (stat) {
              case bcmMplsOutBytes:
				  hw_val[0] = hw_val[1] = 0;
				  soc_mem_field_get(unit, EGR_PW_INIT_COUNTERSm, 
                                                           (uint32 *) &pw_init_entry, BYTE_COUNTERf, hw_val);
                             COMPILER_64_SET(*val,  hw_val[1],hw_val[0]);
                             break;

              case bcmMplsOutPkts:
                             pkt_counter  = soc_mem_field32_get(unit, EGR_PW_INIT_COUNTERSm, 
                                                           &pw_init_entry, PACKET_COUNTERf); 
                             seq_num  = soc_mem_field32_get(unit, EGR_PW_INIT_COUNTERSm, 
                                                           &pw_init_entry, SEQ_NUMf);
                             val_low = (pkt_counter - (pkt_counter/65535));
                             COMPILER_64_SET(*val, 0, val_low);
                             break;
              default:
                             break;
         }
    }
  }
  return rv;
}

int
bcm_tr_mpls_label_stat_get32(int unit, bcm_mpls_label_t label, bcm_gport_t port,
                             bcm_mpls_stat_t stat, uint32 *val)
{
    int rv;
    uint64 val64;


    rv = bcm_tr_mpls_label_stat_get(unit, label, port, stat, &val64);
    if (rv == BCM_E_NONE) {
        *val = COMPILER_64_LO(val64);
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr_mpls_label_stat_clear
 * Purpose:
 *      Clear L2 MPLS PW Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      label  - (IN) MPLS label
 *      port   - (IN) MPLS gport
 *      stat   - (IN)  specify the Stat type
 * Returns:
 *      BCM_E_XXX
 */     

int
bcm_tr_mpls_label_stat_clear(int unit, bcm_mpls_label_t label, bcm_gport_t port,
                           bcm_mpls_stat_t stat)
{
    int rv=BCM_E_UNAVAIL, pw_term_cnt, pw_init_cnt, index;
    bcm_mpls_tunnel_switch_t info;
    mpls_entry_entry_t ment;
    ing_pw_term_counters_entry_t cent;
    egr_pw_init_counters_entry_t pw_init_entry;
    egr_l3_next_hop_entry_t egr_nh;
    ing_dvp_table_entry_t dvp;
    int vp, nh_index;

  /* Based on Switch Type */
  if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit)) {
       rv = BCM_E_NONE;
  } else if (SOC_IS_TR_VL(unit)) {

    if ((stat == bcmMplsInBytes) || (stat == bcmMplsInPkts)) {
         sal_memset(&info, 0, sizeof(bcm_mpls_tunnel_switch_t));
         info.port = port;
         info.label = label;

         rv = _bcm_tr_mpls_entry_set_key(unit, &info, &ment);
         BCM_IF_ERROR_RETURN(rv);

         rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);
	
         if (rv == SOC_E_NONE) {
              if (!soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUM_VALIDf)) {
                 return BCM_E_CONFIG;
              }

              /* Retrieve PW_TERM Index from MPLS_Entry */
              pw_term_cnt = soc_MPLS_ENTRYm_field32_get(unit, &ment, PW_TERM_NUMf);

              /* Clear the Counter Value */
              sal_memset(&cent, 0, sizeof(ing_pw_term_counters_entry_t));
              rv = WRITE_ING_PW_TERM_COUNTERSm(unit, MEM_BLOCK_ANY, pw_term_cnt, &cent);
              if (rv < 0) {
                   return rv;
              }
         }
    }else if ((stat == bcmMplsOutBytes) || (stat == bcmMplsOutPkts) ) {

         /* Get egress next-hop index from MPLS GPORT */
         vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
		 
         rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
         if (rv < 0) {
	     return rv;
         }
         nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, 
                                       NEXT_HOP_INDEXf);
     
         /* Retrieve EGR L3 NHOP Entry */
         BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

         /* Retrieve PW_INIT Index from EGR L3 NHOP Entry */
         pw_init_cnt = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, PW_INIT_NUMf);

        /* Clear the Counter Value */
         sal_memset(&pw_init_entry, 0, sizeof(egr_pw_init_counters_entry_t));
         rv = WRITE_EGR_PW_INIT_COUNTERSm(unit, MEM_BLOCK_ANY, pw_init_cnt, &pw_init_entry);
         if (rv < 0) {
	     return rv;
         }
    }
  }
  return rv;
}

/*
 * Function:
 *      bcm_tr_mpls_port_learn_set
 * Purpose:
 *      Set the CML bits for an mpls port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_port_learn_set(int unit, bcm_gport_t mpls_port_id, uint32 flags)
{
    int vp, cml = 0, rv = BCM_E_NONE;
    source_vp_entry_t svp;

    rv = _bcm_tr_mpls_check_init (unit);
    if (rv < 0) {
        return rv;
     }

    cml = 0;
    if (!(flags & BCM_PORT_LEARN_FWD)) {
       cml |= (1 << 0);
    }
    if (flags & BCM_PORT_LEARN_CPU) {
       cml |= (1 << 1);
    }
    if (flags & BCM_PORT_LEARN_PENDING) {
       cml |= (1 << 2);
    }
    if (flags & BCM_PORT_LEARN_ARL) {
       cml |= (1 << 3);
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id);

    /* Be sure the entry is used and is set for VPLS */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        return rv;
    }
    if (soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) != 1) { /* VPLS */
        return BCM_E_NOT_FOUND;
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml);
    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml);
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    return rv;
}

/*
 * Function:    
 *      bcm_tr_mpls_port_learn_get
 * Purpose:
 *      Get the CML bits for an mpls port
 * Returns: 
 *      BCM_E_XXX
 */     
int     
bcm_tr_mpls_port_learn_get(int unit, bcm_gport_t mpls_port_id, uint32 *flags)
{
    int rv, vp, cml = 0;
    source_vp_entry_t svp;
    
    rv = _bcm_tr_mpls_check_init (unit);
    if (rv < 0) {
        return rv;
     }
    
    /* Get the VP index from the gport */
    vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id);

    /* Be sure the entry is used and is set for VPLS */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        return rv;
    }
    if (soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) != 1) { /* VPLS */
        return BCM_E_NOT_FOUND;
    }
    cml = soc_SOURCE_VPm_field32_get(unit, &svp, CML_FLAGS_NEWf);
    
    *flags = 0;
    if (!(cml & (1 << 0))) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (cml & (1 << 1)) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (cml & (1 << 2)) {
       *flags |= BCM_PORT_LEARN_PENDING;
    }
    if (cml & (1 << 3)) {
       *flags |= BCM_PORT_LEARN_ARL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr_mpls_mcast_flood_set
 * Purpose:
 *      Set the PFM for a VPLS VFI.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_mpls_mcast_flood_set(int unit, bcm_vlan_t vlan,
                            bcm_vlan_mcast_flood_t mode)
{
    int vfi, pfm = 0, rv = BCM_E_NONE;

    rv = _bcm_tr_mpls_check_init (unit);
    if (rv < 0) {
        return rv;
     }

    vfi = _BCM_MPLS_VPN_ID_GET(vlan);
    switch (mode) {
        case BCM_VLAN_MCAST_FLOOD_ALL:
           pfm = 0;
           break;
        case BCM_VLAN_MCAST_FLOOD_NONE:
           pfm = 2;
           break;
        default:
           pfm = 1;
           break;
    }

    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
        return BCM_E_NOT_FOUND;
    }
    /* Update the HW entry */
    rv = soc_mem_field32_modify(unit, VFIm, vfi, PFMf, pfm);
    return rv;
}

/*
 * Function:
 *      bcm_tr_mpls_mcast_flood_get
 * Purpose:
 *      Get the PFM for a VPLS VFI.
 * Returns: 
 *      BCM_E_XXX
 */     
int
bcm_tr_mpls_mcast_flood_get(int unit, bcm_vlan_t vlan,
                            bcm_vlan_mcast_flood_t *mode)
{
    int vfi, pfm = 0, rv = BCM_E_NONE;
    vfi_entry_t vfi_entry;

    rv = _bcm_tr_mpls_check_init (unit);
    if (rv < 0) {
        return rv;
     }

    vfi = _BCM_MPLS_VPN_ID_GET(vlan);

    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry);
    if (rv < 0) {
        return rv;
    }
    pfm = soc_VFIm_field32_get(unit, &vfi_entry, PFMf);

    switch (pfm) {
        case 0:
           *mode = BCM_VLAN_MCAST_FLOOD_ALL;
           break;
        case 2:
           *mode = BCM_VLAN_MCAST_FLOOD_NONE;
           break;
        default:
           *mode = BCM_VLAN_MCAST_FLOOD_UNKNOWN;
           break;
    }
    return rv;
}

#ifdef BCM_TRIUMPH2_SUPPORT
int
_bcm_esw_mpls_flex_stat_index_set(int unit, bcm_gport_t port, int fs_idx)
{
    int rv, vp, nh_index;
    ing_dvp_table_entry_t dvp;

    vp = BCM_GPORT_MPLS_PORT_ID_GET(port);

    rv = bcm_tr_mpls_lock(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
        bcm_tr_mpls_unlock(unit);
        return BCM_E_NOT_FOUND;
    } else {
        /* Ingress side */
        rv = soc_mem_field32_modify(unit, SOURCE_VPm, vp, VINTF_CTR_IDXf, 
                                    fs_idx);

        if (BCM_SUCCESS(rv)) {
            rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
            /* Egress side */
            if (BCM_SUCCESS(rv)) {
                rv = soc_mem_field32_modify(unit, EGR_L3_NEXT_HOPm, nh_index,
                                            VINTF_CTR_IDXf, fs_idx);
            }
        }
    }

    bcm_tr_mpls_unlock(unit);
    return rv;
}

int
_bcm_tr2_mpls_svp_field_set(int unit, bcm_gport_t vp, 
                           soc_field_t field, int value)
{
    int rv = BCM_E_NONE;
    source_vp_entry_t svp;

    rv = bcm_tr_mpls_lock(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        bcm_tr_mpls_unlock(unit);
        return rv;
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, field, value);
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    bcm_tr_mpls_unlock(unit);
    return rv;
}
#endif

#else /* BCM_TRIUMPH_SUPPORT && BCM_MPLS_SUPPORT && INCLUDE_L3 */
int bcm_tr_mpls_not_empty;
#endif /* BCM_TRIUMPH_SUPPORT && BCM_MPLS_SUPPORT && INCLUDE_L3 */
