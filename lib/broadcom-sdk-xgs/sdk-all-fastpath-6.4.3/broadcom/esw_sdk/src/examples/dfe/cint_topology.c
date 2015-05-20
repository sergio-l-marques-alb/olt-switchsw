/* 
 * $Id: $
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
 * DFE topology example example:
 * 
 * The example simulate:
 *  1. set topology for local module
 *  2. set topology for faps group
 *
 */


/*set topology for local module*/
int
local_mapping_set(int unit) {
    int rv;
    bcm_port_t links_array[3];
    bcm_module_t local_module;

    /*define local module id*/
    local_module = 1;
    BCM_FABRIC_LOCAL_MODID_SET(local_module);
    
    /*map local module id with module id*/ 
    rv = bcm_fabric_modid_local_mapping_set(unit, local_module, 512);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_fabric_modid_local_mapping_set, rv=%d, \n", rv);
        return rv;
    }

    /*select links*/
    links_array[0] = 1;
    links_array[1] = 2;
    links_array[2] = 3;

    /*set topology using module id*/
    rv = bcm_fabric_link_topology_set(unit, 512, 3, links_array); 
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_fabric_link_topology_set, rv=%d, \n", rv);
        return rv;
    }

    /*define second local module id*/
    local_module = 2;                                              
    BCM_FABRIC_LOCAL_MODID_SET(local_module);

    /*map local module id with module id*/ 
    rv = bcm_fabric_modid_local_mapping_set(unit, local_module, 25);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_fabric_modid_local_mapping_set, rv=%d, \n", rv);
        return rv;
    }

    /*select links*/
    links_array[0] = 4;
    links_array[1] = 5;
    links_array[2] = 6;

    /*set topology using local module id*/
    rv = bcm_fabric_link_topology_set(unit, local_module, 3, links_array); /*with local modid*/
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_fabric_link_topology_set, rv=%d, \n", rv);
        return rv;
    }

    printf("local_mapping_set: PASS\n");
    return BCM_E_NONE;
}

/*set topology for faps group*/
int
faps_group_set(int unit) {
    int rv;
    bcm_module_t modid_array[3];
    bcm_port_t links_array[3];
    bcm_module_t g;

    /*define faps group*/
    g = 1;
    BCM_FABRIC_GROUP_MODID_SET(g);

    /*select module ids*/
    modid_array[0] = 1;
    modid_array[1] = 2;
    modid_array[2] = 3;

    /*map faps groups with selected module ids*/
    rv = bcm_fabric_modid_group_set(unit,  g, 3, modid_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in  bcm_fabric_modid_group_set, rv=%d, \n", rv);
        return rv;
    }

    /*select links*/
    links_array[0] = 1;
    links_array[1] = 2;
    links_array[2] = 3;

    /*set faps group topology*/
    rv = bcm_fabric_link_topology_set(unit, g, 3, links_array); /*with local modid*/
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_fabric_link_topology_set, rv=%d, \n", rv);
        return rv;
    }
    
    printf("faps_group_set: PASS\n");
    return BCM_E_NONE;
}

