/* $Id: cint_utils_roo.c,v 1.10 Broadcom SDK $
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
 *
 * This file provides ROO basic functionality 
 */


/*
 * Helps to migrate from Jericho version to QAX: 
 * Until Jericho, we build link layer using bcm_l2_egress_create API. 
 * From QAX, we use bcm_l3_egress_create instead. 
 * allocation_flags: For QAX only, additional flags we can't convert from l2_egress to l3_egress. 
 *          BCM_L3_KEEP_DSTMAC, BCM_L3_KEEP_VLAN 
 *  
 */
int roo__overlay_link_layer_create(int unit, 
                                   bcm_l2_egress_t* l2_egress, 
                                   int allocation_flags, 
                                   bcm_if_t* encap_id) {

    int rv = BCM_E_NONE; 

    /* For QAX, translate bcm_l2_egress_t to bcm_l3_egress_t  */
    if (is_device_or_above(unit,QUMRAN_AX)) {
        create_l3_egress_s l3_egress; 

        /* convert from l2_egress to l3_egress */
        l3_egress.next_hop_mac_addr = l2_egress->dest_mac; 
        l3_egress.encap_id = l2_egress->encap_id; 
        l3_egress.intf = l2_egress->l3_intf; 
        l3_egress.vlan = l2_egress->outer_vlan; 


        if (l2_egress->flags & BCM_L2_EGRESS_WITH_ID) {
            l3_egress.allocation_flags |= BCM_L3_WITH_ID; 
        }

        if (l2_egress->flags & BCM_L2_EGRESS_REPLACE) {
            l3_egress.allocation_flags |= BCM_L3_REPLACE; 
        }

        l3_egress.allocation_flags |= allocation_flags; 


        rv = l3__egress_only_encap__create(unit, &l3_egress); 
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_encap__create\n");
            return rv;
        }
        *encap_id = l3_egress.encap_id; 
    } 
    /* until jericho, use bcm_l3_egress_create */
    else {
        rv = bcm_l2_egress_create(unit, l2_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_egress_create\n");
            return rv;
        }
        *encap_id = l2_egress->encap_id; 
    }
    return rv; 
}


/*
 * Helps to migrate from Jericho version to QAX: 
 * Until Jericho, we build link layer using bcm_l2_egress_create API. 
 * From QAX, we use bcm_l3_egress_create instead. 
 *  
 * in l2_egress_create, up to 2 vlan can be configured. 
 * For l3_egress_create, if 1 vlan, vlan = vsi (from the tunnel.vlan), so no extra configuration required. 
 *                       if 2 vlans, we must perform EVE
 *  
 * incoming parameters: 
 * - l2_egress 
 * - port, vsi: used for EVE only. perform port x vsi -> outAC. 
 *  
 */
int roo__overlay_eve(int unit, 
                    bcm_l2_egress_t* l2_egress, 
                    bcm_port_t port, 
                    bcm_vlan_t vsi) {
    int rv = BCM_E_NONE; 

    if (is_device_or_above(unit,QUMRAN_AX)) {
        rv = vlan__eve__routing__set(unit,
                                port, vsi, /* perform port x vsi -> outAC.  */
                                l2_egress->outer_vlan, l2_egress->inner_vlan);  /* vlans to add to the packet. */
        if (rv != BCM_E_NONE) {
            printf("Error, vlan__eve__routing__set\n");
            return rv;
        }
    }

    return rv; 

}
