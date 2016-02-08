/* 
 * $Id: bcmx_multicast.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:        bcmx/multicast.c
 * Purpose:     BCMX Multicast APIs
 *
 */

#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/lport.h>
#include <bcmx/multicast.h>

#include "bcmx_int.h"


/*
 * Function:
 *     bcmx_multicast_create
 * Notes:
 *     If group ID is not specified, the ID returned
 *     from the first successful 'create' is used for the remaining
 *     units.
 */
int
bcmx_multicast_create(uint32 flags, bcm_multicast_t *group)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(group);

    BCMX_UNIT_ITER(bcm_unit, i) {

        tmp_rv = bcm_multicast_create(bcm_unit, flags, group);
        /*
         * Use the ID from first successful 'create' if policer ID
         * is not specified.
         */
        if (!(flags & BCM_MULTICAST_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                flags |= BCM_MULTICAST_WITH_ID;
            }
        }

        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *    bcmx_multicast_destroy
 */
int
bcmx_multicast_destroy(bcm_multicast_t group)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_multicast_destroy(bcm_unit, group);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *     bcmx_multicast_l3_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_l3_encap_get(bcm_multicast_t group,
                            bcm_gport_t port, bcm_if_t intf, 
                            bcm_if_t *encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_l3_encap_get(bcm_unit, group, port, intf, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_l2_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_l2_encap_get(bcm_multicast_t group, 
                            bcm_gport_t port, bcm_vlan_t vlan, 
                            bcm_if_t *encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_l2_encap_get(bcm_unit, group, port, vlan, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_vpls_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_vpls_encap_get(bcm_multicast_t group, 
                              bcm_gport_t port, bcm_gport_t mpls_port_id, 
                              bcm_if_t *encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_vpls_encap_get(bcm_unit, group,
                                        port, mpls_port_id, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_subport_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_subport_encap_get(bcm_multicast_t group, 
                                 bcm_gport_t port, bcm_gport_t subport, 
                                 bcm_if_t *encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_subport_encap_get(bcm_unit, group,
                                           port, subport, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_mim_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_mim_encap_get(bcm_multicast_t group,
                             bcm_gport_t port, bcm_gport_t mim_port_id,
                             bcm_if_t *encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_mim_encap_get(bcm_unit, group,
                                       port, mim_port_id, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_wlan_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_wlan_encap_get(bcm_multicast_t group,
                              bcm_gport_t port, bcm_gport_t wlan_port_id,
                              bcm_if_t *encap_id)
{
    int  rv = BCM_E_NONE;
    int  i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(encap_id);

    if BCM_SUCCESS((_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT)))
    {
      return bcm_multicast_wlan_encap_get(bcm_unit, group,
                                          port, wlan_port_id, encap_id);
    }
    BCMX_UNIT_ITER(bcm_unit,i)
    {
      rv = bcm_multicast_wlan_encap_get(bcm_unit, group,
                                        port, wlan_port_id, encap_id);
      if (BCM_SUCCESS(rv)) {
          return rv;
      }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_multicast_egress_add
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_egress_add(bcm_multicast_t group,
                          bcm_gport_t port, bcm_if_t encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_egress_add(bcm_unit, group, port, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_egress_delete
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_egress_delete(bcm_multicast_t group, 
                             bcm_gport_t port, bcm_if_t encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_egress_delete(bcm_unit, group, port, encap_id);
}

/*
 * Function:
 *    bcmx_multicast_egress_delete_all
 */
int
bcmx_multicast_egress_delete_all(bcm_multicast_t group)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_multicast_egress_delete_all(bcm_unit, group);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *     bcmx_multicast_egress_set
 * Notes:
 *     Each gport 'port' in array is a physical local port.
 */
int
bcmx_multicast_egress_set(bcm_multicast_t group, int port_count, 
                          bcm_gport_t *port_array, bcm_if_t *encap_id_array)
{
    int          rv = BCM_E_NONE;
    int          tmp_rv;
    int          i, bcm_unit;
    int          j, unit;
    bcm_port_t   unit_port;
    int          unit_port_count;
    bcm_gport_t  *unit_port_array;
    bcm_if_t     *unit_encap_id_array;

    BCMX_PARAM_NULL_CHECK(port_array);
    BCMX_PARAM_NULL_CHECK(encap_id_array);
    if (port_count <= 0) {
        return BCM_E_NONE;
    }

    if ((unit_port_array = sal_alloc(port_count * sizeof(*port_array),
                                     "bcmx_multicast_egress_set")) == NULL) {
        return BCM_E_MEMORY;
    }

    if ((unit_encap_id_array = sal_alloc(port_count * sizeof(*encap_id_array),
                                         "bcmx_multicast_egress_set")) == NULL) {
        sal_free(unit_port_array);
        return BCM_E_MEMORY;
    }        

    /* Need to gather all ports in a unit, then do a 'set' */
    BCMX_UNIT_ITER(bcm_unit, i) {
        unit_port_count = 0;

        /* Get all ports that belong to a unit */
        for (j = 0; j < port_count; j++) {
            if (BCM_FAILURE
                (_bcmx_dest_to_unit_port(port_array[j],
                                         &unit, &unit_port,
                                         BCMX_DEST_CONVERT_DEFAULT))) {
                continue;
            }
            if (unit != bcm_unit) {
                continue;
            }

            unit_port_array[unit_port_count] = port_array[j];
            unit_encap_id_array[unit_port_count] = encap_id_array[j];
            unit_port_count++;
        }

        if (unit_port_count == 0) {
            continue;
        }

        tmp_rv = bcm_multicast_egress_set(bcm_unit, group, unit_port_count,
                                          unit_port_array, unit_encap_id_array);

        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    sal_free(unit_port_array);
    sal_free(unit_encap_id_array);

    return rv;
}

/*
 * Function:
 *     bcmx_multicast_egress_get
 * Notes:
 *     Each gport 'port' is a physical local port.
 */
int
bcmx_multicast_egress_get(bcm_multicast_t group, int port_max, 
                          bcm_gport_t *port_array, bcm_if_t *encap_id_array, 
                          int *port_count)
{
    int          rv = BCM_E_NONE;
    int          tmp_rv;
    int          i, bcm_unit;
    int          port_max_left;
    int          unit_port_count;
    bcm_gport_t  *unit_port_array;
    bcm_if_t     *unit_encap_id_array;

    BCMX_PARAM_NULL_CHECK(port_array);
    BCMX_PARAM_NULL_CHECK(encap_id_array);
    BCMX_PARAM_NULL_CHECK(port_count);

    *port_count   = 0;
    port_max_left = port_max;

    /* Need to gather egress ports from all units */
    BCMX_UNIT_ITER(bcm_unit, i) {

        if (port_max_left <= 0) {
            break;
        }

        unit_port_count = 0;
        unit_port_array = &port_array[*port_count];
        unit_encap_id_array = &encap_id_array[*port_count];

        tmp_rv = bcm_multicast_egress_get(bcm_unit, group, port_max_left,
                                          unit_port_array, unit_encap_id_array,
                                          &unit_port_count);
        if (BCM_SUCCESS(tmp_rv)) {
            *port_count   += unit_port_count;
            port_max_left -= unit_port_count;
        }

        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

