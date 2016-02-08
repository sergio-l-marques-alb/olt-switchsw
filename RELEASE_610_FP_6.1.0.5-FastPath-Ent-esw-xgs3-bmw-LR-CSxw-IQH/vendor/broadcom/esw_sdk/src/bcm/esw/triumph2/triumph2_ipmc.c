/*
 * $Id: triumph2_ipmc.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * File:        ipmc.c
 * Purpose:     Tracks and manages IPMC tables.
 */

#ifdef INCLUDE_L3

#include <soc/l3x.h>
#include <soc/bradley.h>

#include <bcm/error.h>
#include <bcm/ipmc.h>

#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/bradley.h>

#include <bcm_int/esw_dispatch.h>

#define REPL_PORT_CHECK(unit, port) \
    if (!IS_PORT(unit, port)) { return BCM_E_PARAM; }

typedef struct _tr2_repl_port_info_s {
    int32 *vlan_count;                        /* # VLANs the port repl to */
    uint32 emc_val;                           /* EGRMETERINGCONFIG value */
} _tr2_repl_port_info_t;

typedef struct _tr2_repl_info_s {
    int ipmc_size;
    uint32 intf_num;              /* Number of interfaces on this device */
    uint16 ipmc_vlan_total;       /* Keep track of total and */
    uint32 *bitmap_entries_used;  /* free entries of IPMC_VLAN table */
    _bcm_repl_list_info_t *repl_list_info;
    _tr2_repl_port_info_t *port_info[SOC_MAX_NUM_PORTS];
} _tr2_repl_info_t;

static _tr2_repl_info_t *_tr2_repl_info[BCM_MAX_NUM_UNITS];

#define IPMC_REPL_LOCK(_u_)                    \
    {                                          \
        soc_mem_lock(_u_, MMU_IPMC_VLAN_TBLm); \
    }
#define IPMC_REPL_UNLOCK(_u_)                    \
    {                                            \
        soc_mem_unlock(_u_, MMU_IPMC_VLAN_TBLm); \
    }
#define IPMC_REPL_ID(_u_, _id_) \
        if ((_id_ < 0) || (_id_ >= _tr2_repl_info[_u_]->ipmc_size)) \
            { return BCM_E_PARAM; }
#define IPMC_REPL_INIT(unit) \
        if (_tr2_repl_info[unit] == NULL) { return BCM_E_INIT; }
#define IPMC_REPL_TOTAL(_u_) \
        _tr2_repl_info[_u_]->ipmc_vlan_total
#define IPMC_REPL_INTF_TOTAL(_u_) \
        _tr2_repl_info[_u_]->intf_num
#define IPMC_REPL_VE_USED_GET(_u_, _i_) \
        SHR_BITGET(_tr2_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_REPL_VE_USED_SET(_u_, _i_) \
        SHR_BITSET(_tr2_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_REPL_VE_USED_CLR(_u_, _i_) \
        SHR_BITCLR(_tr2_repl_info[_u_]->bitmap_entries_used, _i_)

#define IPMC_REPL_LIST_INFO(_u_) \
	_tr2_repl_info[_u_]->repl_list_info
#define IPMC_REPL_PORT_INFO(_u_, _p_) \
        _tr2_repl_info[_u_]->port_info[_p_]
#define IPMC_REPL_PORT_VLAN_COUNT(_u_, _p_, _ipmc_id_) \
        _tr2_repl_info[_u_]->port_info[_p_]->vlan_count[_ipmc_id_]

typedef struct _rep_regs_s {
    soc_field_t port_ptr;
    soc_field_t last_ptr;
    soc_mem_t   mem;
}_rep_regs_t;

_rep_regs_t ch_rep_regs[55] = { {INVALIDf,       INVALIDf,    INVALIDm},
                                {PORT1_1STPTRf,  PORT1_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT2_1STPTRf,  PORT2_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT3_1STPTRf,  PORT3_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT4_1STPTRf,  PORT4_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT5_1STPTRf,  PORT5_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT6_1STPTRf,  PORT6_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT7_1STPTRf,  PORT7_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT8_1STPTRf,  PORT8_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT9_1STPTRf,  PORT9_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT10_1STPTRf,  PORT10_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT11_1STPTRf,  PORT11_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT12_1STPTRf,  PORT12_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT13_1STPTRf,  PORT13_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT14_1STPTRf,  PORT14_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT15_1STPTRf,  PORT15_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT16_1STPTRf,  PORT16_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT17_1STPTRf,  PORT17_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT18_1STPTRf,  PORT18_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT19_1STPTRf,  PORT19_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT20_1STPTRf,  PORT20_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT21_1STPTRf,  PORT21_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT22_1STPTRf,  PORT22_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT23_1STPTRf,  PORT23_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT24_1STPTRf,  PORT24_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT25_1STPTRf,  PORT25_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT26_1STPTRf,  PORT26_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT27_1STPTRf,  PORT27_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT28_1STPTRf,  PORT28_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT29_1STPTRf,  PORT29_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT30_1STPTRf,  PORT30_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT31_1STPTRf,  PORT31_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT32_1STPTRf,  PORT32_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT33_1STPTRf,  PORT33_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT34_1STPTRf,  PORT34_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT35_1STPTRf,  PORT35_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT36_1STPTRf,  PORT36_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT37_1STPTRf,  PORT37_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT38_1STPTRf,  PORT38_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT39_1STPTRf,  PORT39_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT40_1STPTRf,  PORT40_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT41_1STPTRf,  PORT41_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT42_1STPTRf,  PORT42_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT43_1STPTRf,  PORT43_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT44_1STPTRf,  PORT44_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT45_1STPTRf,  PORT45_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT46_1STPTRf,  PORT46_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT47_1STPTRf,  PORT47_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT48_1STPTRf,  PORT48_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT49_1STPTRf,  PORT49_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {PORT50_1STPTRf,  PORT50_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {PORT51_1STPTRf,  PORT51_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {PORT52_1STPTRf,  PORT52_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {PORT53_1STPTRf,  PORT53_LASTf, MMU_IPMC_GROUP_TBL8m},
};

STATIC int
_tr2_ipmc_vlan_ptr(int unit,int ipmc_id,
                  bcm_port_t port, int *vptr, int last, int set)
{
    soc_field_t  port_ptr = INVALIDf;
    soc_field_t  last_ptr = INVALIDf;
    soc_mem_t    mem = INVALIDm;
    mmu_ipmc_group_tbl0_entry_t gentry;
    int rv;
    _rep_regs_t     *rep_arr_ptr;

    rep_arr_ptr = ch_rep_regs;

    mem = rep_arr_ptr[port].mem;
    port_ptr = rep_arr_ptr[port].port_ptr;
    last_ptr = rep_arr_ptr[port].last_ptr;

    if ((INVALIDm == mem) || (INVALIDf == port_ptr)){
        return BCM_E_PARAM;
    }

    soc_mem_lock(unit, mem);
    if ((rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, 
                           ipmc_id, &gentry)) < 0) {
        soc_mem_unlock(unit, mem);
        return rv;
    }
    if (set) {
        soc_mem_field32_set(unit, mem, &gentry, port_ptr, *vptr);
        soc_mem_field32_set(unit, mem, &gentry, last_ptr,last ? 1 : 0);
        if ((rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, ipmc_id, 
                                &gentry)) < 0) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
    } else {
        *vptr =  soc_mem_field32_get(unit, mem, &gentry, port_ptr);
    }
    soc_mem_unlock(unit, mem);

    return BCM_E_NONE;
}


#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_tr2_ipmc_repl_reload
 * Purpose:
 *      Re-Initialize IPMC replication software to state consistent with
 *      hardware
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_ipmc_repl_reload(int unit)
{
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    uint32 ms_bit, ls_bits[2];
    bcm_port_t port;
    int ipmc_id, alloc_size, intf_alloc_size;
    int vlan_ptr, next_vlan_ptr, first_vlan_ptr;
    SHR_BITDCL          *intf_vec = NULL;
    _bcm_repl_list_info_t *rli_start, *rli_current;
    int list_mode_detected = FALSE;
    int rv;

    bcm_tr2_ipmc_repl_detach(unit);

    /* Allocate struct for IPMC replication booking keeping */
    alloc_size = sizeof(_tr2_repl_info_t);
    _tr2_repl_info[unit] = sal_alloc(alloc_size, "IPMC repl info");
    if (_tr2_repl_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_tr2_repl_info[unit], 0, alloc_size);

    IPMC_REPL_INTF_TOTAL(unit) = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm) +
                                 soc_mem_index_count(unit, EGR_L3_INTFm);

    IPMC_REPL_TOTAL(unit) = soc_mem_index_count(unit, MMU_IPMC_VLAN_TBLm);
    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_TOTAL(unit));
    _tr2_repl_info[unit]->bitmap_entries_used =
        sal_alloc(alloc_size, "IPMC repl entries used");
    if (_tr2_repl_info[unit]->bitmap_entries_used == NULL) {
        bcm_tr2_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_tr2_repl_info[unit]->bitmap_entries_used, 0, alloc_size);

    /* Always reserve slot 0 (because 0 means NULL pointer) */
    IPMC_REPL_VE_USED_SET(unit, 0);

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        alloc_size = sizeof(_tr2_repl_port_info_t);
        IPMC_REPL_PORT_INFO(unit, port) =
            sal_alloc(alloc_size, "IPMC repl port info");
        if (IPMC_REPL_PORT_INFO(unit, port) == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, port), 0, alloc_size);

        alloc_size = sizeof(int32) * IPMC_REPL_INTF_TOTAL(unit);
        IPMC_REPL_PORT_INFO(unit, port)->vlan_count =
            sal_alloc(alloc_size, "IPMC repl port vlan count");
        if (IPMC_REPL_PORT_INFO(unit, port)->vlan_count == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, port)->vlan_count,
                   0, alloc_size);
    }

    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(alloc_size, "IPMC repl interface vector");
    if (intf_vec == NULL) {
        bcm_tr2_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    intf_alloc_size = alloc_size;

    rli_start = IPMC_REPL_LIST_INFO(unit);

    /*
     * Read IPMC group and IPMC VLAN tables to build up software state
     */
    for (ipmc_id = soc_mem_index_min(unit, L3_IPMCm);
         ipmc_id <= soc_mem_index_max(unit, L3_IPMCm); ipmc_id++) {

        PBMP_ITER (PBMP_PORT_ALL(unit), port) {
            rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port, &first_vlan_ptr, 0, 0);
            if (BCM_FAILURE(rv)) {
                sal_free(intf_vec);
                bcm_tr2_ipmc_repl_detach(unit);
                return rv;
            }
            if (first_vlan_ptr == 0) {
                continue;
            }

            if (IPMC_REPL_VE_USED_GET(unit, first_vlan_ptr)) {
                /* We've already traversed this list, just note it */
                for (rli_current = rli_start; rli_current != NULL;
                     rli_current = rli_current->next) {
                    if (rli_current->index == first_vlan_ptr) {
                        (rli_current->refcount)++;
                        IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) = 
                            rli_current->list_size;
                        list_mode_detected = TRUE;
                        break;
                    }
                }
                if (rli_current == NULL) {
                    /* Table out of sync.  Not good. */
                    return BCM_E_INTERNAL;
                } else {
                    continue;
                }
            }

            sal_memset(intf_vec, 0, intf_alloc_size);
            next_vlan_ptr = first_vlan_ptr;
            vlan_ptr = 0;
            while (vlan_ptr != next_vlan_ptr) {
                vlan_ptr = next_vlan_ptr;
                READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                        vlan_ptr, &vlan_entry);
                soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                       LSB_VLAN_BMf, ls_bits);
                ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                       &vlan_entry, MSB_VLANf);
                intf_vec[2 * ms_bit + 0] = ls_bits[0];
                intf_vec[2 * ms_bit + 1] = ls_bits[1];
                IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) +=
                      _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
                IPMC_REPL_VE_USED_SET(unit, vlan_ptr);
                next_vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                  &vlan_entry, NEXTPTRf);
            }

            /* Create new list element */
            alloc_size = sizeof(_bcm_repl_list_info_t);
            rli_current = sal_alloc(alloc_size, "IPMC repl list info");
            if (rli_current == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(rli_current, 0, alloc_size);
            rli_current->index = first_vlan_ptr;
            rli_current->hash =
                _shr_crc32b(0, (uint8 *)intf_vec,
                            IPMC_REPL_INTF_TOTAL(unit));
            rli_current->next = rli_start;
            rli_current->list_size =
                IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id);
            IPMC_REPL_LIST_INFO(unit) = rli_current;
            rli_start = rli_current;
            (rli_current->refcount)++;
        }
    }

    if (list_mode_detected) {
        SOC_IPMCREPLSHR_SET(unit, 1);
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_tr2_ipmc_repl_init
 * Purpose:
 *      Initialize IPMC replication.
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_init(int unit)
{
    bcm_port_t          port;
    int                 alloc_size;

    bcm_tr2_ipmc_repl_detach(unit);

    /* Allocate struct for IPMC replication bookkeeping */
    alloc_size = sizeof(_tr2_repl_info_t);
    _tr2_repl_info[unit] = sal_alloc(alloc_size, "IPMC repl info");
    if (_tr2_repl_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_tr2_repl_info[unit], 0, alloc_size);
    _tr2_repl_info[unit]->ipmc_size = soc_mem_index_count(unit, L3_IPMCm);

    IPMC_REPL_INTF_TOTAL(unit) = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm) + 
                                 soc_mem_index_count(unit, EGR_L3_INTFm);

    IPMC_REPL_TOTAL(unit) = soc_mem_index_count(unit, MMU_IPMC_VLAN_TBLm);
    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_TOTAL(unit));
    _tr2_repl_info[unit]->bitmap_entries_used =
        sal_alloc(alloc_size, "IPMC repl entries used");
    if (_tr2_repl_info[unit]->bitmap_entries_used == NULL) {
        bcm_tr2_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_tr2_repl_info[unit]->bitmap_entries_used, 0, alloc_size);

    /* Always reserve slot 0 */
    IPMC_REPL_VE_USED_SET(unit, 0);

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        alloc_size = sizeof(_tr2_repl_port_info_t);
        IPMC_REPL_PORT_INFO(unit, port) =
            sal_alloc(alloc_size, "IPMC repl port info");
        if (IPMC_REPL_PORT_INFO(unit, port) == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, port), 0, alloc_size);
       
        alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
        IPMC_REPL_PORT_INFO(unit, port)->vlan_count =
            sal_alloc(alloc_size, "IPMC repl port vlan count");
        if (IPMC_REPL_PORT_INFO(unit, port)->vlan_count == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, port)->vlan_count,
                   0, alloc_size);
    }

    if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_BCMSIM) {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL0m, COPYNO_ALL, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL1m, COPYNO_ALL, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL2m, COPYNO_ALL, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL3m, COPYNO_ALL, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL4m, COPYNO_ALL, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL5m, COPYNO_ALL, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL6m, COPYNO_ALL, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL7m, COPYNO_ALL, FALSE)); 
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL8m, COPYNO_ALL, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_VLAN_TBLm, COPYNO_ALL, FALSE));
    }
    
    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_detach
 * Purpose:
 *      Initialize IPMC replication.
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_detach(int unit)
{
    bcm_port_t          port;
    _bcm_repl_list_info_t *rli_current, *rli_free;

    if (_tr2_repl_info[unit] != NULL) {
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            if (_tr2_repl_info[unit]->port_info[port] != NULL) {
                if (_tr2_repl_info[unit]->
                    port_info[port]->vlan_count != NULL) {
                    sal_free(_tr2_repl_info[unit]->
                             port_info[port]->vlan_count);
                }
                sal_free(_tr2_repl_info[unit]->port_info[port]);
            }
        }

        if (_tr2_repl_info[unit]->bitmap_entries_used != NULL) {
            sal_free(_tr2_repl_info[unit]->bitmap_entries_used);
        }

        if (_tr2_repl_info[unit]->repl_list_info != NULL) {
            rli_current = IPMC_REPL_LIST_INFO(unit);
            while (rli_current != NULL) {
                rli_free = rli_current;
                rli_current = rli_current->next;
                sal_free(rli_free);
            }
        }

        sal_free(_tr2_repl_info[unit]);
        _tr2_repl_info[unit] = NULL;
    }

    return BCM_E_NONE;
}

int
_bcm_tr2_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                             int id)
{
    int *if_array = NULL;
    int intf_num, intf_max, alloc_size, rv = BCM_E_NONE;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(int);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &intf_num);
    if (BCM_SUCCESS(rv)) {
        if (intf_num < intf_max) {
            if_array[intf_num++] = id;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             intf_num, if_array);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array); 
    return rv;
}

int
_bcm_tr2_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                                int if_max, int id)
{
    int  *if_array = NULL;
    int  alloc_size, if_count, if_cur, match, rv = BCM_E_NONE;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    } else if ((if_max <= 0) || ((uint32)if_max > IPMC_REPL_INTF_TOTAL(unit))) {
        return BCM_E_PARAM;
    }
    alloc_size = if_max * sizeof(int);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, if_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array);
        } else { 
            rv = BCM_E_NOT_FOUND;
        }
    }
    
    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *	_bcm_tr2_repl_list_compare
 * Description:
 *	Compare HW list starting at vlan_index to the VLAN list contained
 *      in vlan_vec.
 */

STATIC int
_bcm_tr2_repl_list_compare(int unit, int vlan_index,
                          SHR_BITDCL *intf_vec)
{
    uint32		ms_bit, hw_ms_bit, ms_max;
    uint32		ls_bits[2], hw_ls_bits[2];
    mmu_memories1_ipmc_vlan_tbl_entry_t vlan_entry;
    int	                vlan_ptr, last_vlan_ptr;

    last_vlan_ptr = -1;
    vlan_ptr = vlan_index;
    ms_max = _SHR_BITDCLSIZE(IPMC_REPL_INTF_TOTAL(unit)) / 2; /* 32 -> 64 */

    for (ms_bit = 0; ms_bit < ms_max; ms_bit++) {
        ls_bits[0] = intf_vec[2 * ms_bit + 0];
        ls_bits[1] = intf_vec[2 * ms_bit + 1];
        if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
            if (vlan_ptr == last_vlan_ptr) { /* HW list end, not app list */
                return BCM_E_NOT_FOUND;
            }
            SOC_IF_ERROR_RETURN
                (READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                                                   vlan_ptr, &vlan_entry));
            hw_ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                       &vlan_entry, MSB_VLANf);
            soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                                   LSB_VLAN_BMf, hw_ls_bits);
            if ((hw_ms_bit != ms_bit) || (ls_bits[0] != hw_ls_bits[0]) ||
                (ls_bits[1] != hw_ls_bits[1])) {
                return BCM_E_NOT_FOUND;
            }
            last_vlan_ptr = vlan_ptr;
            vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                          &vlan_entry, NEXTPTRf);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_tr2_repl_list_free
 * Description:
 *	Release the IPMC_VLAN entries in the HW list starting at start_ptr.
 */

STATIC int
_bcm_tr2_repl_list_free(int unit, int start_ptr)
{
    mmu_memories1_ipmc_vlan_tbl_entry_t vlan_entry;
    int	                vlan_ptr, last_vlan_ptr;

    last_vlan_ptr = -1;
    vlan_ptr = start_ptr;

    while (vlan_ptr != last_vlan_ptr) {
        SOC_IF_ERROR_RETURN
            (READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                                               vlan_ptr, &vlan_entry));
        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                          &vlan_entry, NEXTPTRf);
        IPMC_REPL_VE_USED_CLR(unit, last_vlan_ptr);
    }

    return BCM_E_NONE;
}

STATIC int
_tr2_ipmc_repl_next_free_ptr(int unit)
{
    int                 ix, bit;
    SHR_BITDCL          not_ptrs;

    for (ix = 0; ix < _SHR_BITDCLSIZE(IPMC_REPL_TOTAL(unit)); ix++) {
        not_ptrs = ~_tr2_repl_info[unit]->bitmap_entries_used[ix];
        if (not_ptrs) {
            for (bit = 0; bit < SHR_BITWID; bit++) {
                if (not_ptrs & (1 << bit)) {
                    return (ix * SHR_BITWID) + bit;
                }
            }
        }
    }

    return -1;
}

/*
 * Function:
 *	_bcm_tr2_repl_list_write
 * Description:
 *	Write the VLAN list contained in vlan_vec into the HW table.
 *      Return the start_index and total VLAN count.
 */

STATIC int
_bcm_tr2_repl_list_write(int unit, int *start_index, int *count,
                        SHR_BITDCL *intf_vec)
{
    uint32		ms_bit, ms_max, msb;
    uint32		ls_bits[2];
    mmu_memories1_ipmc_vlan_tbl_entry_t vlan_entry;
    int	                vlan_ptr, last_vlan_ptr, last_write_ptr;
    int                 vlan_count = 0;
    int                 repl_type;
    int                 if_cnt, nh_cnt, offset;
    
    if_cnt = soc_mem_index_count(unit, EGR_L3_INTFm);
    nh_cnt = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    last_vlan_ptr = -1;
    last_write_ptr = -1;

    /* Iterate over the two replication types - interfaces and next hops */
    for (repl_type = 0; repl_type < 2; repl_type++) {
        if (repl_type == 0) {
            ms_max = _SHR_BITDCLSIZE(if_cnt) / 2; /* 32 -> 64 */
        } else {
            ms_max = _SHR_BITDCLSIZE(nh_cnt) / 2; /* 32 -> 64 */
        }
        for (ms_bit = 0; ms_bit < ms_max; ms_bit++) {
            offset = (repl_type == 0) ? 0 : _SHR_BITDCLSIZE(if_cnt);
            ls_bits[0] = intf_vec[offset + 2 * ms_bit + 0];
            ls_bits[1] = intf_vec[offset + 2 * ms_bit + 1];
            if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
                vlan_ptr = _tr2_ipmc_repl_next_free_ptr(unit);
                if (last_vlan_ptr > 0) {
                    /* Write previous non-zero entry */
                    soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                            &vlan_entry, NEXTPTRf,
                            (vlan_ptr > 0) ? vlan_ptr : last_vlan_ptr);
                    /* If vlan_ptr <= 0, terminate so
                     * later cleanup can remove the chain. */
                    SOC_IF_ERROR_RETURN
                        (WRITE_MMU_IPMC_VLAN_TBLm(unit,
                            MEM_BLOCK_ALL, last_vlan_ptr, &vlan_entry));
                    last_write_ptr = last_vlan_ptr;
                    if (vlan_ptr < 0) {
                        _bcm_tr2_repl_list_free(unit, *start_index);
                        return BCM_E_RESOURCE;
                    }
                } else {
                    if (vlan_ptr < 0) {
                        return BCM_E_RESOURCE;
                    }
                    *start_index = vlan_ptr;
                }
                sal_memset(&vlan_entry, 0, sizeof(vlan_entry));
                if (repl_type == 1) {
                    /* Replication is over next hops */
                    msb = ms_bit | (1 << 8);
                } else {
                    /* Replication is over interfaces */
                    msb = ms_bit;
                }
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                                   MSB_VLANf, msb);
                soc_MMU_IPMC_VLAN_TBLm_field_set(unit, &vlan_entry,
                                                 LSB_VLAN_BMf, ls_bits);
                IPMC_REPL_VE_USED_SET(unit, vlan_ptr);
                vlan_count +=
                    _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
                last_vlan_ptr = vlan_ptr;
            }
        }
    }

    if (last_vlan_ptr > 0) {
        /* Write final entry */
        soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                          &vlan_entry, NEXTPTRf, last_vlan_ptr);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_IPMC_VLAN_TBLm(unit,
                   MEM_BLOCK_ALL, last_vlan_ptr, &vlan_entry));
        
        if (last_write_ptr > 0) {
            soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                             LSB_VLAN_BMf, ls_bits);
            if ((_shr_popcount(ls_bits[0]) +
                 _shr_popcount(ls_bits[1])) == 1) {
                /* Mark previous link with "last" flag */
                SOC_IF_ERROR_RETURN
                    (READ_MMU_IPMC_VLAN_TBLm(unit,
                          MEM_BLOCK_ANY, last_write_ptr, &vlan_entry));
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                                   LASTf, 1);
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_IPMC_VLAN_TBLm(unit,
                          MEM_BLOCK_ALL, last_write_ptr, &vlan_entry));
            }
        }
    }

    *count = vlan_count;
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_tr2_ipmc_egress_intf_set
 * Purpose:
 *	Assign set of egress interfaces to port's replication list for chosen
 *	IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	ipmc_id  - The index number.
 *	port     - port to list.
 *	if_count - number of interfaces in replication list.
 *      if_array - (IN) List of interface numbers.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_tr2_ipmc_egress_intf_set(int unit, int ipmc_id, bcm_port_t port,
                            int if_count, bcm_if_t *if_array)
{
    int			rv = BCM_E_NONE;
    SHR_BITDCL          *intf_vec = NULL;
    int			list_start_ptr, prev_start_ptr;
    int                 alloc_size, repl_hash, vlan_count, last_flag;
    int                 if_num, partition;
    bcm_l3_intf_t       l3_intf;
    pbmp_t              pbmp, ubmp;
    _bcm_repl_list_info_t *rli_start, *rli_current, *rli_prev;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if ((uint32)if_count > IPMC_REPL_INTF_TOTAL(unit)) {
        return BCM_E_PARAM;
    }

    /* Partition between L3 interfaces and NHIs in the intf_vec
     * intf_vec = [L3 interfaces, NHIs] */
    partition = soc_mem_index_count(unit, EGR_L3_INTFm);

    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(alloc_size, "IPMC repl interface vector");
    if (intf_vec == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(intf_vec, 0, alloc_size);

    IPMC_REPL_LOCK(unit);

    /* Interface validation and vector construction */
    for (if_num = 0; if_num < if_count; if_num++) {
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num]) && 
            (uint32)(if_array[if_num]) > partition) {
            rv = BCM_E_PARAM;
            goto intf_set_done;
        }
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num])) {
            /* L3 interface is used */
            bcm_l3_intf_t_init(&l3_intf);
            l3_intf.l3a_intf_id = if_array[if_num];
            if ((rv = bcm_esw_l3_intf_get(unit, &l3_intf)) < 0) {
                goto intf_set_done;
            }
            if ((rv = bcm_esw_vlan_port_get(unit, l3_intf.l3a_vid,
                                            &pbmp, &ubmp)) < 0) {
                goto intf_set_done;
            }
            if (!BCM_PBMP_MEMBER(pbmp, port)) {
                rv = BCM_E_PARAM;
                goto intf_set_done;
            }
            SHR_BITSET(intf_vec, if_array[if_num]);
        } else {
            /* Next hop is used */
            SHR_BITSET(intf_vec, partition + if_array[if_num] - 
                       BCM_XGS3_DVP_EGRESS_IDX_MIN);
        }
    }

    /* Check previous group pointer */
    if ((rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port,
                                &prev_start_ptr, 0, 0)) < 0) {
        goto intf_set_done;
    }

    /* Search for list already in table */
    rli_start = IPMC_REPL_LIST_INFO(unit);

    repl_hash =
        _shr_crc32b(0, (uint8 *)intf_vec, IPMC_REPL_INTF_TOTAL(unit));

    for (rli_current = rli_start; rli_current != NULL;
         rli_current = rli_current->next) {
        if (repl_hash == rli_current->hash) {
            rv = _bcm_tr2_repl_list_compare(unit, rli_current->index,
                                           intf_vec);
            if (rv == BCM_E_NOT_FOUND) {
                continue; /* Not a match */
            } else if (rv < 0) {
                goto intf_set_done; /* Access error */
            } else {
                break; /* Match */
            }
        }
    }

    if (rli_current != NULL) {
        /* Found a match, point to here and increase reference count */
        if (prev_start_ptr == rli_current->index) {
            /* We're already pointing to this list, so done */
            rv = BCM_E_NONE;
            goto intf_set_done;
        } else {
            list_start_ptr = rli_current->index;
            vlan_count = rli_current->list_size;
        }
    } else {
        /* Not a match, make a new chain */
        if ((rv = _bcm_tr2_repl_list_write(unit, &list_start_ptr,
                                         &vlan_count, intf_vec)) < 0) {
            goto intf_set_done;
        }

        if (vlan_count > 0) {
            /* Update data structures */
            alloc_size = sizeof(_bcm_repl_list_info_t);
            rli_current = sal_alloc(alloc_size, "IPMC repl list info");
            if (rli_current == NULL) {
                /* Release list */
                _bcm_tr2_repl_list_free(unit, list_start_ptr);
                rv = BCM_E_MEMORY;
                goto intf_set_done;
            }
            sal_memset(rli_current, 0, alloc_size);
            rli_current->index = list_start_ptr;
            rli_current->hash = repl_hash;
            rli_current->next = rli_start;
            rli_current->list_size = vlan_count;
            IPMC_REPL_LIST_INFO(unit) = rli_current;
            rli_start = rli_current;
        }
    }

    last_flag = (vlan_count == 1);

    if (vlan_count > 0) {
        if ((rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port,
                                    &list_start_ptr, last_flag, 1)) < 0) {
            if (rli_current->refcount == 0) {
                /* This was new */
                _bcm_tr2_repl_list_free(unit, list_start_ptr);
                IPMC_REPL_LIST_INFO(unit) = rli_current->next;
                sal_free(rli_current);
            }
            goto intf_set_done;
        }

        (rli_current->refcount)++;
        /* we don't need this rli_current anymore */
    } else if (prev_start_ptr != 0) {
        list_start_ptr = 0;
        if ((rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port,
                                    &list_start_ptr, last_flag, 1)) < 0) {
            goto intf_set_done;
        }
    }
    IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) = vlan_count;

    if (prev_start_ptr != 0) {
        rli_prev = NULL;
        for (rli_current = rli_start; rli_current != NULL;
             rli_current = rli_current->next) {
            if (prev_start_ptr == rli_current->index) {
                (rli_current->refcount)--;
                if (rli_current->refcount == 0) {
                    /* Free these linked list entries */
                    rv = _bcm_tr2_repl_list_free(unit, prev_start_ptr);
                    /* If we have an error, we'll fall out anyway */
                    if (rli_prev == NULL) {
                        IPMC_REPL_LIST_INFO(unit) = rli_current->next;
                    } else {
                        rli_prev->next = rli_current->next;
                    }
                    sal_free(rli_current);
                }
                break;
            }
            rli_prev = rli_current;
        }
    }

 intf_set_done:
    IPMC_REPL_UNLOCK(unit);
    if (intf_vec != NULL) {
        sal_free(intf_vec);
    }
    return rv;
}

/*
 * Function:
 *	bcm_tr2_ipmc_egress_intf_get
 * Purpose:
 *	Retreieve set of egress interfaces in port's replication list
 *      for chosen IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	ipmc_id  - The index number.
 *	port     - port to list.
 *	if_max   - maximum number of interfaces in replication list.
 *      if_array - (OUT) List of interface numbers.
 *	if_count - (OUT) number of interfaces returned in replication list.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_tr2_ipmc_egress_intf_get(int unit, int ipmc_id, bcm_port_t port,
                            int if_max, bcm_if_t *if_array, int *if_count)
{
    int			rv = BCM_E_NONE;
    uint32              ms_bit;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int                 vlan_ptr, last_vlan_ptr;
    uint32              vlan_count;
    int                 ls_pos;
    int                 rvth = BCM_E_NONE;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (if_max <= 0) {
        return BCM_E_PARAM;
    }

    IPMC_REPL_LOCK(unit);
    if (IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) == 0) {
        *if_count = 0;
        IPMC_REPL_UNLOCK(unit);
        return rvth;
    }

    if ((rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port, &vlan_ptr, 0, 0)) < 0) {
        goto intf_get_done;
    }
    last_vlan_ptr = -1;
    vlan_count = 0;
    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                                          vlan_ptr, &vlan_entry)) < 0) {
            goto intf_get_done;
        }
        ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                    &vlan_entry, MSB_VLANf);
        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                         LSB_VLAN_BMf, ls_bits);
        /* Using MSB, check if L3 interface or next hop */
        
        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                /* Using MSB, check if L3 interface or next hop */
                if (ms_bit & (1 << 8)) {
                    /* Entry contains next hops */
                    if_array[vlan_count++] = ((ms_bit & 0xFF) * 64) + ls_pos +
                                              BCM_XGS3_DVP_EGRESS_IDX_MIN;
                } else {
                    /* Entry contains interfaces */
                    if_array[vlan_count++] = (ms_bit * 64) + ls_pos;
                }
                if (vlan_count == (uint32)if_max) {
                    *if_count = vlan_count;
                    goto intf_get_done;
                }
            }
        }
        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                      &vlan_entry, NEXTPTRf);

        if (vlan_count >= IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
            break;
        }
    }

    *if_count = vlan_count;

 intf_get_done:
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_add
 * Purpose:
 *      Add VLAN to selected ports' replication list for chosen
 *      IPMC group.
 *Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to add.
 *      vlan     - VLAN to replicate.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_ipmc_repl_add(int unit, int ipmc_id, bcm_port_t port,
                     bcm_vlan_t vlan)
{
    int alloc_size, intf_max, if_count, rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    pbmp_t pbmp, ubmp;
    bcm_l3_intf_t l3_intf;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    /* Check if port belongs to this VLAN */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_get(unit, vlan, &pbmp, &ubmp));
    if (!SOC_PBMP_MEMBER(pbmp, port)) {
        return BCM_E_PARAM;
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    if (bcm_esw_l3_intf_find_vlan(unit, &l3_intf) < 0) {
        return BCM_E_PARAM;
    }

    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        if (if_count < intf_max) {
            if_array[if_count++] = l3_intf.l3a_intf_id;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_delete
 * Purpose:
 *      Remove VLAN from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to remove.
 *      vlan     - VLAN to delete from replication.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_delete(int unit, int ipmc_id, bcm_port_t port,
                        bcm_vlan_t vlan)
{
    int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    bcm_l3_intf_t       l3_intf;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    if (bcm_esw_l3_intf_find_vlan(unit, &l3_intf) < 0) {
        return BCM_E_PARAM;
    }


    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == l3_intf.l3a_intf_id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }
    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_get
 * Purpose:
 *      Return set of VLANs selected for port's replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to list.
 *      vlan_vec - (OUT) vector of replicated VLANs common to selected ports.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_get(int unit, int ipmc_id, bcm_port_t port,
                        bcm_vlan_vector_t vlan_vec)
{
    int                 rv = BCM_E_NONE;
    uint32              ms_bit;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int                 vlan_ptr, last_vlan_ptr;
    uint32              vlan_count;
    int                 rvth = BCM_E_NONE;

    IPMC_REPL_INIT(unit);

    IPMC_REPL_ID(unit, ipmc_id);

    REPL_PORT_CHECK(unit, port);

    BCM_VLAN_VEC_ZERO(vlan_vec);

    IPMC_REPL_LOCK(unit);
    if (IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) == 0) {
        IPMC_REPL_UNLOCK(unit);
        return rvth;
    }

    if ((rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port, &vlan_ptr, 0, 0)) < 0) {
        goto get_done;
    }
    last_vlan_ptr = -1;
    vlan_count = 0;
    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                          vlan_ptr, &vlan_entry)) < 0) {
            goto get_done;
        }
        ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                    &vlan_entry, MSB_VLANf);
        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                         LSB_VLAN_BMf, ls_bits);
        vlan_vec[2 * ms_bit + 0] = ls_bits[0];
        vlan_vec[2 * ms_bit + 1] = ls_bits[1];
        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                      &vlan_entry, NEXTPTRf);

        vlan_count +=
            _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
        if (vlan_count >= IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
            break;
        }
    }

 get_done:
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_delete_all
 * Purpose:
 *      Remove all VLANs from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The MC index number.
 *      port     - port from which to remove VLANs.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_delete_all(int unit, int ipmc_id, bcm_port_t port)
{
    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        /* Nothing to do */
        return BCM_E_NONE;
    }

    return bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port, 0, NULL);
}

/*
 * Function:
 *      bcm_tr2_ipmc_egress_intf_add
 * Purpose:
 *      Add L3 interface to selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to add.
 *      l3_intf  - L3 interface to replicate.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                            bcm_l3_intf_t *l3_intf)
{
    bcm_if_t *if_array = NULL;
    int  intf_num, intf_max, alloc_size, rv = BCM_E_NONE;
    pbmp_t pbmp, ubmp;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    /* Check if port belongs to this VLAN */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_get(unit, l3_intf->l3a_vid, &pbmp, &ubmp));
    if (!SOC_PBMP_MEMBER(pbmp, port)) {
        return BCM_E_PARAM;
    }

    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &intf_num);
    if (BCM_SUCCESS(rv)) {
        if (intf_num < intf_max) {
            if_array[intf_num++] = l3_intf->l3a_intf_id;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             intf_num, if_array);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_egress_intf_delete
 * Purpose:
 *      Remove L3 interface from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to remove.
 *      l3_intf  - L3 interface to delete from replication.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                               bcm_l3_intf_t *l3_intf)
{
    bcm_if_t *if_array = NULL;
    int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }


    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == l3_intf->l3a_intf_id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}
#endif /* INCLUDE_L3 */
