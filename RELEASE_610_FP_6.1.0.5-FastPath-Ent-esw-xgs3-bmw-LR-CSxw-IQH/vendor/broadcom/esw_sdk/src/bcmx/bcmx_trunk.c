/*
 * $Id: bcmx_trunk.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:    bcmx/trunk.c
 * Purpose: BCMX Trunk APIs
 *
 * BCMX trunking takes place on switch ports, not fabric ports.
 * Fabric port trunking may be controlled by the stack/toppology
 * code, so we are careful to skip any fabric devices and ports.
 */

#include <sal/core/libc.h>

#include <bcm/types.h>

#include <bcmx/trunk.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include <bcm_int/control.h>
#include "bcmx_int.h"


/*
 * Function:
 *      bcmx_trunk_add_info_t_init
 * Purpose:
 *      Initialize the bcmx_trunk_add_info_t structure.
 * Parameters:
 *      trunk_add_info - Pointer to structure to be initialized.
 * Returns:
 *      None
 * Notes:
 *      Memory is allocated for the port list member in this
 *      call.  The corresponding 'free' API needs to be called
 *      when structure is no longer needed.
 */
void
bcmx_trunk_add_info_t_init(bcmx_trunk_add_info_t *trunk_add_info)
{
    if (trunk_add_info != NULL) {
        sal_memset(trunk_add_info, 0, sizeof(bcmx_trunk_add_info_t));
        bcmx_lplist_t_init(&trunk_add_info->ports);
    }
}


/*
 * Function:
 *      bcmx_trunk_add_info_t_free
 * Purpose:
 *      Free memory allocated by bcmx_trunk_add_info_t_init().
 * Parameters:
 *      trunk_add_info - Pointer to structure to free memory.
 * Returns:
 *      None
 */
void
bcmx_trunk_add_info_t_free(bcmx_trunk_add_info_t *trunk_add_info)
{
    if (trunk_add_info != NULL) {
        bcmx_lplist_free(&trunk_add_info->ports);
    }
}

/*
 * Function:
 *      bcmx_trunk_init
 */

int
bcmx_trunk_init(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_init(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_detach
 */

int
bcmx_trunk_detach(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_detach(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_create
 */

int
bcmx_trunk_create(bcm_trunk_t *tid)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_create(bcm_unit, tid);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_create_id
 */

int
bcmx_trunk_create_id(bcm_trunk_t tid)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_create_id(bcm_unit, tid);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_psc_set
 */

int
bcmx_trunk_psc_set(bcm_trunk_t tid, int psc)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_psc_set(bcm_unit, tid, psc);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_psc_get
 */

int
bcmx_trunk_psc_get(bcm_trunk_t tid, int *psc)
{
    int rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        rv = bcm_trunk_psc_get(bcm_unit, tid, psc);
        if (rv >= 0) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}


/*
 * Function:
 *      bcmx_trunk_chip_info_get
 */

int
bcmx_trunk_chip_info_get(bcm_trunk_chip_info_t *ta_info)
{
    int rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        rv = bcm_trunk_chip_info_get(bcm_unit, ta_info);
        if (rv >= 0) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_trunk_set
 */

int
bcmx_trunk_set(bcm_trunk_t tid, bcmx_trunk_add_info_t *add_info)
{
    bcm_trunk_add_info_t  badd;
    int                   rv, tmp_rv, i, bcm_unit, count;
    bcmx_lport_t          lport;

    sal_memset(&badd, 0, sizeof(bcm_trunk_add_info_t));
    badd.psc = add_info->psc;
    badd.dlf_index = -1;
    badd.mc_index = -1;
    badd.ipmc_index = -1;
    BCMX_LPLIST_ITER(add_info->ports, lport, count) {
        if (count >= BCM_TRUNK_MAX_PORTCNT) {
            return BCM_E_PARAM;
        }
        if (lport == add_info->dlf_port) {
            badd.dlf_index = count;
        }
        if (lport == add_info->mc_port) {
            badd.mc_index = count;
        }
        if (lport == add_info->ipmc_port) {
            badd.ipmc_index = count;
        }

        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_modid_port(lport,
                                      &badd.tm[count], &badd.tp[count],
                                      BCMX_DEST_CONVERT_DEFAULT));
    }
    badd.num_ports = count;
    
    rv = BCM_E_NONE;
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_set(bcm_unit, tid, &badd);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_destroy
 */

int
bcmx_trunk_destroy(bcm_trunk_t tid)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_destroy(bcm_unit, tid);
        if (tmp_rv == BCM_E_NOT_FOUND) {
            tmp_rv = BCM_E_UNAVAIL;
        }
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_get
 */

int
bcmx_trunk_get(bcm_trunk_t tid, bcmx_trunk_add_info_t *t_data)
{
    bcm_trunk_add_info_t  badd;
    int                   rv, i, bcm_unit;
    bcmx_lport_t          lport;

    rv = BCM_E_NONE;
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        rv = bcm_trunk_get(bcm_unit, tid, &badd);
        if (rv >= 0) {
            sal_memset(t_data, 0, sizeof(bcmx_trunk_add_info_t));
            bcmx_lplist_init(&t_data->ports, badd.num_ports, 0);
            t_data->psc = badd.psc;
            t_data->dlf_port = BCMX_LPORT_INVALID;
            t_data->mc_port = BCMX_LPORT_INVALID;
            t_data->ipmc_port = BCMX_LPORT_INVALID;

            for (i = 0; i < badd.num_ports && i < BCM_TRUNK_MAX_PORTCNT; i++) {
                /* Convert to lport */
                BCM_IF_ERROR_RETURN
                    (_bcmx_dest_from_modid_port(&lport,
                                                badd.tm[i], badd.tp[i],
                                                BCMX_DEST_CONVERT_DEFAULT));
                bcmx_lplist_add(&t_data->ports, lport);
                if (i == badd.dlf_index) {
                    t_data->dlf_port = lport;
                }
                if (i == badd.mc_index) {
                    t_data->mc_port = lport;
                }
                if (i == badd.ipmc_index) {
                    t_data->ipmc_port = lport;
                }
            }

            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}



/*
 * Function:
 *      bcmx_trunk_mcast_join
 */

int
bcmx_trunk_mcast_join(bcm_trunk_t tid,
                      bcm_vlan_t vid,
                      bcm_mac_t mac)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_mcast_join(bcm_unit, tid, vid, mac);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_egress_set
 */

int
bcmx_trunk_egress_set(bcm_trunk_t tid, bcmx_lplist_t lplist)
{
    bcm_pbmp_t  pbm;
    int         rv, tmp_rv, bcm_unit, i;

    rv = BCM_E_NONE;
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        BCM_PBMP_CLEAR(pbm);
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbm);
        tmp_rv = bcm_trunk_egress_set(bcm_unit, tid, pbm);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_egress_get
 */

int
bcmx_trunk_egress_get(bcm_trunk_t tid, bcmx_lplist_t *lplist)
{
    bcm_pbmp_t  pbm;
    int         rv, tmp_rv, bcm_unit, i;

    rv = BCM_E_NONE;
    bcmx_lplist_clear(lplist);
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_egress_get(bcm_unit, tid, &pbm);
        if (tmp_rv >= 0) {
            BCMX_LPLIST_PBMP_ADD(lplist, bcm_unit, pbm);
        }
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_trunk_find
 */
int
bcmx_trunk_find(bcmx_lport_t lport, bcm_trunk_t *tid)
{
    int rv = BCM_E_NONE, bcm_unit, i;
    bcm_module_t modid;
    bcm_port_t   modport;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(lport, &modid, &modport,
                                  BCMX_DEST_CONVERT_DEFAULT));

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_trunk_find(bcm_unit,
                            modid,
                            modport,
                            tid);

        if (BCM_SUCCESS(rv)) {
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trunk_failover_set
 * Purpose:
 *     Assign the failover port list for a specific trunk port.
 * Parameters:
 *     tid           - Trunk id
 *     failport      - Port in trunk for which to specify failover port list
 *     psc           - Port selection criteria for failover port list
 *     flags         - BCM_TRUNK_FLAG_FAILOVER_xxx
 *     count         - Number of ports in failover port list
 *     fail_to_array - Failover port list
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_trunk_failover_set(bcm_trunk_t tid, bcm_gport_t failport, 
                        int psc, uint32 flags, int count, 
                        bcm_gport_t *fail_to_array)
{
    int         rv = BCM_E_NONE, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(fail_to_array);

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_unit_port(failport, &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT));

    tmp_rv = bcm_trunk_failover_set(bcm_unit, tid, failport, 
                                    psc, flags, count, fail_to_array);
    BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);

    return rv;
}

/*
 * Function:
 *     bcmx_trunk_failover_get
 * Purpose:
 *     Retrieve the failover port list for a specific trunk port.
 * Parameters:
 *     tid           - Trunk id
 *     failport      - Port in trunk for which to retrieve failover port list
 *     psc           - (OUT) Port selection criteria for failover port list
 *     flags         - (OUT) BCM_TRUNK_FLAG_FAILOVER_xxx
 *     array_size    - Maximum number of ports in provided failover port list
 *     fail_to_array - (OUT) Failover port list
 *     array_count   - (OUT) Number of ports in returned failover port list
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_trunk_failover_get(bcm_trunk_t tid, bcm_gport_t failport, 
                        int *psc, uint32 *flags, int array_size, 
                        bcm_gport_t *fail_to_array, int *array_count)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(psc);
    BCMX_PARAM_NULL_CHECK(flags);
    BCMX_PARAM_NULL_CHECK(fail_to_array);
    BCMX_PARAM_NULL_CHECK(array_count);

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_unit_port(failport, &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT));

    return bcm_trunk_failover_get(bcm_unit, tid, failport, 
                                  psc, flags, array_size,
                                  fail_to_array, array_count);
}

