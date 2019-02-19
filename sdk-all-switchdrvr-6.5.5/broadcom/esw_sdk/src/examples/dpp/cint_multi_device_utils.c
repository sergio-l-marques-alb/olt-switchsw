/* $Id: cint_multi_device_utils.c,v 1.6 Broadcom SDK $
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
 */

/*
 * This cint holds all the general purpose functions for running cints on multi device.
 * Load it before you load any other cint using multi device
 *
*/



/* Use this function before performing for loops API calls.
   In most of these calls, you would want to configure the local device first, to assign the resource,
   and then configure the rest of the devices with the relevant _WITH_ID flag.
 
   This function accepts the array of units used by the cint, its length, and the system port to be used locally.
   The function places the unit whose local port is attached to the system port in index 0 of the array.
   The unit that was in index 0 is placed in the local unit's previous last index.
   */
int 
units_array_make_local_first(int *units_ids, int nof_units, int sysport){
    int i, tmp, rv = BCM_E_NONE;
    int first_modid, unit, gport_modid, num_modids;
    int gport;

    if (nof_units < 2){ /* Nothing to do here*/
        return BCM_E_NONE;
    }

    rv = bcm_stk_sysport_gport_get(units_ids[0], sysport, &gport);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_sysport_gport_get\n");
        return rv;
    }
    printf("Sysport's gport is: 0x%x\n", gport);
    gport_modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    printf("Required modid is: 0x%x\n", gport_modid);

    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_stk_modid_get(unit, &first_modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_get\n");
        }

        rv = bcm_stk_modid_count(unit, &num_modids);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_count\n");
        }

        /* By default, modids configured on a device are always consecutive, and the modid returned from bcm_stk_modid_get is always the lowest. */
        if ((gport_modid < first_modid + num_modids)
            && (gport_modid >= first_modid)){
            break;
        }
    }

    if (i == nof_units){
        printf("sysport 0x%x is not a local port to any of the units\n", sysport);
        return BCM_E_NOT_FOUND;
    } 

    printf("sysport 0x%x is a local port for unit %d\n", sysport, units_ids[i]);

    /* Arrange the local unit to be first*/
    tmp = units_ids[0];
    units_ids[0] = units_ids[i];
    units_ids[i] = tmp;
    return rv;
}




/*  When we define egress multicasting for a single unit, it doesn't matter if we don't multicast in ingress. However,
*   when working with several units, if one egress multicast port is configured on a different unit than another egress multicast port,
*   one of them will not get the packet to multicast, because it was sent only to one unit. To avoid that, we need to multicast on the ingress
*   as well, so all ports on all units will get the packet to multicast on egress.
*  
*   In the units_ids array, the ingress unit must be first, because the multicast will be defined in the first unit.
*/
int
device_level_multicast(int *units_ids, int nof_units, int egress_multicast_id, int *ingress_multicast_id){
    int rv, i, unit;
    int erp_port, erp_sysport;
    int flags = 0;
    int count_erp;
    int modid;
    int ingress_unit = units_ids[0];

    flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
    /* Only the local unit is using this multicast, it must be first */
    rv = bcm_multicast_create(ingress_unit, flags, ingress_multicast_id);
    if (rv != BCM_E_NONE){
        printf("Unit %d: Error, in bcm_multicast_create\n", unit);
        return rv;
    }


    flags = BCM_PORT_INTERNAL_EGRESS_REPLICATION;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = bcm_port_internal_get(unit, flags, 1, &erp_port, &count_erp);
        if (rv != BCM_E_NONE) {
            printf("Unit %d: Error, in bcm_port_internal_get\n", unit);
            return rv;
        }

        port_to_system_port(unit, erp_port, &erp_sysport);

        rv = bcm_multicast_ingress_add(ingress_unit, *ingress_multicast_id, erp_sysport, egress_multicast_id);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                 printf("Unit %d: Error, in bcm_multicast_ingress_add\n", unit);
                 return rv;
        }         
    }

    printf("Unit %d: Done confiugring ingress multicast group %d\n", ingress_unit, egress_multicast_id);
    return rv; 
}

int
units_array_make_local_last(int *units_ids, int nof_units, int sysport){
    int tmp;

    units_array_make_local_first(units_ids, nof_units, sysport);
 
    if (nof_units > 1) {
        tmp = units_ids[0];
        units_ids[0] = units_ids[1];
        units_ids[1] = tmp;
    }

    return BCM_E_NONE;
}
