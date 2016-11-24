/*
 * $Id: bcmx_l2cache.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * L2 Cache - Layer 2 BPDU and overflow address cache
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/l2.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"


STATIC INLINE void
_bcmx_l2_cache_addr_t_init(bcmx_l2_cache_addr_t *addr)
{
    sal_memset(addr, 0, sizeof (*addr));
}

STATIC INLINE int
_bcmx_l2_cache_addr_to_bcm(int bcm_unit,
                           bcm_l2_cache_addr_t *dest,
                           bcmx_l2_cache_addr_t *source)
{
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   to_bcm;
    _bcmx_dest_bcmx_t  from_bcmx;

    bcm_l2_cache_addr_t_init(dest);
    dest->flags = source->flags;
    sal_memcpy(dest->mac, source->mac, sizeof(bcm_mac_t));
    sal_memcpy(dest->mac_mask, source->mac_mask, sizeof(bcm_mac_t));
    dest->vlan = source->vlan;
    dest->vlan_mask = source->vlan_mask;
    
    dest->src_port = source->src_port;
    dest->src_port_mask = source->src_port_mask;
    dest->lookup_class = source->lookup_class;

    /*
     * Convert to destination port list to bitmap if destination
     * is not a trunk and DESTPORTS flag is set
     */
    if (!(source->flags & BCM_L2_CACHE_TRUNK) &&
        (source->flags & BCM_L2_CACHE_DESTPORTS)) {
        BCM_PBMP_CLEAR(dest->dest_ports);
        BCMX_LPLIST_TO_PBMP(source->dest_ports, bcm_unit, dest->dest_ports);

    } else {
        /*
         * Convert destination data
         */
        _bcmx_dest_bcmx_t_init(&from_bcmx);

        /* Set flags and data to convert */
        if (source->flags & BCM_L2_CACHE_TRUNK) {
            flags |= BCMX_DEST_TRUNK;
        }
        from_bcmx.port  = source->dest_lport;
        from_bcmx.trunk = source->dest_trunk;

        /* Convert */
        BCM_IF_ERROR_RETURN(_bcmx_dest_to_bcm(&from_bcmx, &to_bcm, &flags));

        /* Set converted flags and data */
        if (flags & BCMX_DEST_TRUNK) {
            dest->flags |= BCM_L2_CACHE_TRUNK;
        } else if (flags & BCMX_DEST_DISCARD) {
            dest->flags |= BCM_L2_CACHE_DISCARD;
        }

        dest->dest_modid = to_bcm.module_id;
        dest->dest_port  = to_bcm.module_port;
        dest->dest_trunk = to_bcm.trunk;
    }

    dest->prio = source->prio;

    return BCM_E_NONE;
}

STATIC INLINE int
_bcmx_l2_cache_addr_from_bcm(bcmx_l2_cache_addr_t *dest,
                             bcm_l2_cache_addr_t *source)
{
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   from_bcm;
    _bcmx_dest_bcmx_t  to_bcmx;

    _bcmx_l2_cache_addr_t_init(dest);
    dest->flags = source->flags;
    sal_memcpy(dest->mac, source->mac, sizeof(bcm_mac_t));
    sal_memcpy(dest->mac_mask, source->mac_mask, sizeof(bcm_mac_t));
    dest->vlan = source->vlan;
    dest->vlan_mask = source->vlan_mask;
    
    dest->src_port = source->src_port;
    dest->src_port_mask = source->src_port_mask;
    dest->lookup_class = source->lookup_class;

    /*
     * Convert bitmap to destination port list if destination
     * is not a trunk and DESTPORTS flag is set
     */
    if (!(source->flags & BCM_L2_CACHE_TRUNK) &&
        (source->flags & BCM_L2_CACHE_DESTPORTS)) {
        bcm_port_t port;
        bcmx_lport_t lport;

        bcmx_lplist_clear(&dest->dest_ports);
        BCM_PBMP_ITER(source->dest_ports, port) {
            BCM_IF_ERROR_RETURN
                (_bcmx_dest_from_modid_port(&lport, source->dest_modid, port,
                                            BCMX_DEST_CONVERT_DEFAULT));
            BCMX_LPLIST_ADD(&dest->dest_ports, lport); 
        }

    } else {
        /*
         * Convert destination data
         */
        _bcmx_dest_bcm_t_init(&from_bcm);

        /* Set flags and data to convert */
        if (source->flags & BCM_L2_CACHE_TRUNK) {
            flags |= BCMX_DEST_TRUNK;
        }
        from_bcm.module_id   = source->dest_modid;
        from_bcm.module_port = source->dest_port;
        from_bcm.trunk       = source->dest_trunk;

        /* Convert */
        BCM_IF_ERROR_RETURN(_bcmx_dest_from_bcm(&to_bcmx, &from_bcm, &flags));

        /* Set converted flags and data */
        if (flags & BCMX_DEST_TRUNK) {
            dest->flags |= BCM_L2_CACHE_TRUNK;
        } else if (flags & BCMX_DEST_DISCARD) {
            dest->flags |= BCM_L2_CACHE_DISCARD;
        }

        dest->dest_lport = to_bcmx.port;
        dest->dest_trunk = to_bcmx.trunk;
    }

    dest->prio = source->prio;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmx_l2_cache_init
 */

int
bcmx_l2_cache_init(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_cache_init(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_cache_addr_t_init
 */

void
bcmx_l2_cache_addr_t_init(bcmx_l2_cache_addr_t *addr)
{
    if (addr != NULL) {
        _bcmx_l2_cache_addr_t_init(addr);
        bcmx_lplist_t_init(&addr->dest_ports);
    }
}

/*
 * Function:
 *      bcmx_l2_cache_addr_t_free
 * Purpose:
 *      Free memory allocated to bcmx_l2_cache_addr_t struct
 */
void
bcmx_l2_cache_addr_t_free(bcmx_l2_cache_addr_t *addr)
{
    if (addr != NULL) {
        bcmx_lplist_free(&addr->dest_ports);
    }
}
  

/*
 * Function:
 *      bcmx_l2_cache_size_get
 */

int
bcmx_l2_cache_size_get(int *size)
{
    int rv = BCM_E_NONE;
    int i, bcm_unit;
    int tmp_size, count = 0;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l2_cache_size_get(bcm_unit, &tmp_size);
        if ((BCM_SUCCESS(rv)) && (tmp_size > 0)) {
            if (count == 0) {
                count = tmp_size;
            }
            if (tmp_size < count) {
                count = tmp_size;
            }
        }
    }
    *size = tmp_size;

    return rv;
}


/*
 * Function:
 *      bcmx_l2_cache_set
 */

int
bcmx_l2_cache_set(int index, bcmx_l2_cache_addr_t *addr, int *index_used)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;
    bcm_l2_cache_addr_t bcm_l2;
    int call_made = FALSE;
    int map_local_cpu;  /* Is local CPU port mapping required? */
    bcmx_lport_t cpu_lport;

    /* Auto inser is not support across chips of different types */
    if (index < 0) {
        return BCM_E_PARAM;
    }

    map_local_cpu = ((!(addr->flags & BCM_L2_CACHE_TRUNK)) &&
                     (addr->dest_lport == BCMX_LPORT_LOCAL_CPU));

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (_bcmx_l2_cache_addr_to_bcm(bcm_unit, &bcm_l2, addr) < 0) {
            BCMX_DEBUG(BCMX_DBG_WARN, ("BCMX L2 CACHE WARN:  "
                                       "Failed to convert "
                                       "L2 Cache address to BCM\n"));
        } else {
            if (map_local_cpu) {
                cpu_lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);
                if (BCM_FAILURE
                    (_bcmx_dest_to_modid_port(cpu_lport,
                                              &bcm_l2.dest_modid,
                                              &bcm_l2.dest_port,
                                              BCMX_DEST_CONVERT_DEFAULT))) {
                    /* Skip if can't find local CPU port */
                    continue;
                }
            }
            tmp_rv = bcm_l2_cache_set(bcm_unit, index, &bcm_l2, index_used);
            if (tmp_rv == BCM_E_NONE) {   /* Note that a call has succeeded */
                call_made = TRUE;
            }

            BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
        }
    }

    if (!call_made) {  /* Never succeeded */
        return BCM_E_FAIL;
    }

    *index_used = index;

    return rv;
}


/*
 * Function:
 *      bcmx_l2_cache_get
 */

int 
bcmx_l2_cache_get(int index, bcmx_l2_cache_addr_t *addr)
{
    int rv =  BCM_E_NOT_FOUND, tmp_rv;
    int i, bcm_unit;
    bcm_l2_cache_addr_t bcm_l2;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_cache_get(bcm_unit, index, &bcm_l2);
        if (BCM_SUCCESS(tmp_rv)) {
            tmp_rv = _bcmx_l2_cache_addr_from_bcm(addr, &bcm_l2);
            if (BCM_SUCCESS(tmp_rv)) {
                return tmp_rv;
            }
        }

        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_cache_delete
 */

int
bcmx_l2_cache_delete(int index)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_cache_delete(bcm_unit, index);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_cache_delete_all
 */

int
bcmx_l2_cache_delete_all(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_cache_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}
