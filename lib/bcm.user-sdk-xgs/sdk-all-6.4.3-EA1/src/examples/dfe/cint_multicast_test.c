/*
 * $Id: cint_multicast_test.c,v 1.3 Broadcom SDK $ 
 *
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
 * Multicast test application
 * Cint shows two examples of multicast applications: direct and indirect
 * Direct: set multicast table.
 * Indirect: set multicast table and create a static topology. An example of such multicast mode will be
 * to work with module ids that higher than 127.
 * 
 * Focal funtions:
 * o    multicast_test__fabric_direct_mode_set()
 * o    multicast_test__fabric_indirect_mode_set()
 */


/* 
 * Create multicast id, and set fabric to it.
 */
int multicast_test__fabric_set(int fe_unit, int fabric_multicast_id, bcm_module_t *modid_array, int nof_modules)
{
    int rv = BCM_E_NONE;

    rv = bcm_multicast_create(fe_unit, BCM_MULTICAST_WITH_ID, &fabric_multicast_id);
    if(rv != 0) {
        printf("Error, in bcm_multicast_create, rv=%d, \n", rv);
        return rv;
    }

    rv = bcm_fabric_multicast_set(fe_unit, fabric_multicast_id, 0, nof_modules, modid_array);
    if(rv != 0) {
        printf("Error, in bcm_fabric_multicast_set, rv=%d, \n", rv);
        return rv;
    }

    return rv;
}

/* 
 * Create multicast id, and set egress of unit to it.
 */ 
int multicast_test__egress_set(int unit, int mc_id, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    bcm_gport_t local_gports;
    bcm_if_t encap;

    rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create (%d)\n", unit);
        return rv;
    }
    BCM_GPORT_LOCAL_SET(&(local_gports), port);
    rv = bcm_multicast_egress_set(unit, mc_id, 1, &local_gports, &encap);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_egress_set (%d)\n", unit);
        return rv;
    }

    return rv;
}

/* 
 * Set multicast direct application. 
 * Given multicast id, create and replicate multicast id over all requested devices. 
 */
int multicast_test__fabric_direct_mode_set(int fe_unit, int fabric_multicast_id, bcm_module_t* modid_array, int nof_modules)
{
    int rv = BCM_E_NONE;
    int tmp,local_modid;

    /* map local modid with modid */
    for (local_modid = 0; local_modid < nof_modules; local_modid++)
    {
        tmp = local_modid;
        BCM_FABRIC_LOCAL_MODID_SET(tmp);
        
        rv = bcm_fabric_modid_local_mapping_set(fe_unit, tmp, modid_array[local_modid]);
        if(rv != 0) {
            printf("Error, in bcm_fabric_modid_local_mapping_set, rv=%d, \n", rv);
            return rv;
        }
    }

    rv = multicast_test__fabric_set(fe_unit, fabric_multicast_id, modid_array, nof_modules);
    if(rv != 0) {
        printf("Error, in multicast_test__fabric_set, rv=%d, \n", rv);
        return rv;
    }

    return rv;
}
