/* $Id: cint_rate_policer_example.c,v 1.4 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * create an Ethernet Policer for each traffic type: UC, unknown UC, MC, unknown MC and BC.
 * get each entry, to see the rates and bursts that were actually set (might be a little different).
 * send traffic and check that rate matches the Policer configuration.
 * Note- these interfaces are not supported for Jericho at the moment 
 *  
 * run:
 * cint examples/cint_rate_policer_example.c
 * cint
 * print traffic_example(0, <in_port>, <out_port>); 
 * 
 * traffic to run: 
 * 1) UC: run ethernet packet with DA 1 and vlan tag id 1 from in_port
 *    traffic will arrive at out_port with 250Mbit rate
 *  
 * 2) MC: run ethernet packet with DA ff:0:0:0:0:1 and vlan tag id 1 from in_port
 *    traffic will arrive at out_port with 500Mbit rate
 *  
 * 3) BC: run ethernet packet with DA ff:ff:ff:ff:ff:ff and vlan tag id 1 from in_port
 *    traffic will arrive at out_port with 100Mbit rate
 * 
 * Distinguishing meter red from Ethernet policer red: 
 * When a packet arrives red at a meter, the meter will output red as well. 
 * In this situation there is no way to distinguish whether the meter decided 
 * to drop the packet (e.g. the packet arrived green, but the meter decided 
 * to drop it) or if it arrived red at the meter (in which case the meter 
 * will keep the red color). 
 * If the SOC property policer_color_resolution_mode=1 then: 
 * If a packet arrives red at the meter, the final DP will be 3. 
 * If the packet does not arrive red, but the meter decides to drop it, 
 * then the DP will be 2. 
 */
int 
traffic_example(int unit, int in_port, int out_port) {

    int rv;
    uint32 kbits_sec, kbits_burst;
    bcm_l2_addr_t l2addr;
    bcm_mac_t uc_mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};
    bcm_mac_t mc_mac = {0xff, 0x0, 0x0, 0x0, 0x0, 0x1};
    bcm_mac_t bc_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	printf("Set the device to drop all red packets...\n");
	rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_cosq_discard_set for BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK.\n");
		return rv;
	}

	rv = bcm_port_vlan_priority_map_set(unit, in_port, 0 /* priority */, 0 /* cfi */, 
											  0 /* internal priority */, bcmColorGreen /* color */);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_port_vlan_priority_map_set with prio 4\n");
		print rv;
		return rv;
	}

	/* set 1800MB rate for unknown unicast traffic */
	rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_DLF, 1800000, 5500);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_rate_bandwidth_set for BCM_RATE_DLF\n");
		return rv;
	}
	rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_DLF, &kbits_sec, &kbits_burst);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_rate_bandwidth_get for BCM_RATE_DLF\n");
		return rv;
	}
	printf("for BCM_RATE_DLF, set rate %d and burst %d\n", kbits_sec, kbits_burst);

	/* set 250MB rate for unicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UCAST, 250000, 300);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_UCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_UCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_UCAST\n");
        return rv;
    }
    printf("for BCM_RATE_UCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

    /* set 2300MB rate for unknown multicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UNKNOWN_MCAST, 2300000, 7000);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_UNKNOWN_MCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_UNKNOWN_MCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_UNKNOWN_MCAST\n");
        return rv;
    }
    printf("for BCM_RATE_UNKNOWN_MCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

    /* set 500MB rate for multicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_MCAST, 500000, 400);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_MCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_MCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_MCAST\n");
        return rv;
    }
    printf("for BCM_RATE_MCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

	/* set 100MB rate for broadcast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_BCAST, 100000, 200);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_BCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_BCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_BCAST\n");
        return rv;
    }
    printf("for BCM_RATE_BCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

    /* add UC, MC and BC entries (with Vlan 1) and send to out_port */
    bcm_l2_addr_t_init(&l2addr, uc_mac, 1);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for UC mac\n");
        return rv;
    }

    sal_memcpy(l2addr.mac, mc_mac, 6);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for MC mac\n");
        return rv;
    }

    sal_memcpy(l2addr.mac, bc_mac, 6);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for BC mac\n");
        return rv;
    }
    
    return rv;
}

/* disable all the Policers set at traffic_example() */
int 
policer_reset(int unit, int in_port) {

    int rv;

    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_BCAST, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set with 0 for BCM_RATE_BCAST\n");
        return rv;
    }

    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_MCAST, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set with 0 for BCM_RATE_MCAST\n");
        return rv;
    }

    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UCAST, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set with 0 for BCM_RATE_UCAST\n");
        return rv;
    }

    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_DLF, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set with 0 for BCM_RATE_DLF\n");
        return rv;
    }

    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UNKNOWN_MCAST, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set with 0 for BCM_RATE_UNKNOWN_MCAST\n");
        return rv;
    }
    
    return rv;
}

