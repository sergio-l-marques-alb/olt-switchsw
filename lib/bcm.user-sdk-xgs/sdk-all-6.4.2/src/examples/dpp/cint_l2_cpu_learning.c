/* $Id: cint_l2_cpu_learning.c,v 1.4 Broadcom SDK $
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
 * File: cint_l2_cpu_learning.c
 * Purpose: Example of how to set l2 cpu learning with up to two devices.
 *          Example of how to enable cpu learn and handle learn and aged event.
 * 
 * Test1:
 *  Run: 
 *  cint cint_port_tpid.c 
 *  cint cint_l2_cpu_learning.c
 *  cint
 *  l2_cpu_learning_run(0, port);
 * 
 *  run packet 1:
 *      ethernet header with DA 33 and any SA
 *      vlan tag with vlan tag id 33
 *      from any port
 * 
 *  run packet 2:
 *      ethernet header with DA 44 and any SA
 *      vlan tag with vlan tag id 44
 *      from any port
 *  
 *  both packets will be sent to CPU
 *
 * Test2:
 *      BCM> examples/dpp/cint_l2_cpu_learning.c
 *      BCM> examples/dpp/cint_l2_mact.c
 *      BCM> m IPT_FORCE_LOCAL_OR_FABRIC FORCE_LOCAL=1 FORCE_FABRIC=0
 *      BCM> cint
 *      cint> print l2_entry_control_set(0, BCM_L2_INGRESS_CENT|BCM_L2_LEARN_CPU);
 *      cint> print l2_entry_event_handle_register(0);
 */
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MACT Cpu Learning:~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* SA and DA in learning msgs */
uint8 _src_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x12, 0x55};
uint8 _dest_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x44, 0x88};

/* DA in incomming packets */
uint8 _dest_mac_1[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
uint8 _dest_mac_2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};

struct l2_learning_info_s {
    bcm_gport_t cpu_port; /* cpu */
    bcm_vlan_t vlan_1;
    bcm_vlan_t vlan_2;
    int out_port;
};

l2_learning_info_s l2_learning_info;

int
l2_learning_info_init(int unit, int port){

    int count, rv = BCM_E_NONE;
    bcm_port_config_t port_config;
    bcm_gport_t cpu;

    /* get the cpu port */
    rv = bcm_port_config_get(unit, &port_config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_config_get\n");
        print rv;
        return rv;
    }
    /* get the first cpu port */
    BCM_PBMP_ITER(port_config.cpu, cpu) {
        BCM_GPORT_LOCAL_SET(l2_learning_info.cpu_port, cpu);
        break;
    }
    printf("cpu_port = 0x%x\n", l2_learning_info.cpu_port);

    l2_learning_info.vlan_1 = 33;
    l2_learning_info.vlan_2 = 44;
    l2_learning_info.out_port = port;

    return BCM_E_NONE;
}

/* set learning mode to egress_distributed (if distributed == 1) or egress_centralized (otherwise) */
int
learning_mode_set(int unit, int distributed){

    int rv, mode;

    if (distributed) {
        /* change learning mode, to be egress distributed, packet will be learned at egress 
         * note that if packet is dropped in ingress then it will not be learned 
         */
        mode = BCM_L2_EGRESS_DIST;
    }
    else {
        /* change learning mode, to be egress centralized */
        mode = BCM_L2_EGRESS_CENT;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, mode);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    return BCM_E_NONE;
}

int
l2_addr_add(int unit, bcm_l2_addr_t *l2addr, bcm_mac_t mac, bcm_gport_t port, uint16 vid) {

    int rv;

    sal_memcpy(l2addr->mac, mac, 6);
    l2addr->port = port;
    l2addr->vid = vid;
    l2addr->flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

int 
l2_cpu_learning_run(int unit, int port){

    int rv;
    bcm_l2_learn_msgs_config_t learn_msgs;
    bcm_l2_addr_distribute_t distribution;
    int age_seconds;
    bcm_l2_addr_t l2addr_1, l2addr_2;

    l2_learning_info_init(unit, port);

    /* create 2 vlans - 33, 44 */
    rv = init_vlan(unit, l2_learning_info.vlan_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in init_vlan with vlan %d\n", l2_learning_info.vlan_1);
        print rv;
        return rv;
    }

    rv = init_vlan(unit, l2_learning_info.vlan_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in init_vlan with vlan %d\n", l2_learning_info.vlan_2);
        print rv;
        return rv;
    }

    /* enter the DA's of the arriving packets and send them to port 4 */
    rv = l2_addr_add(unit, &l2addr_1, _dest_mac_1, l2_learning_info.out_port , l2_learning_info.vlan_1);
    if (BCM_FAILURE(rv)) {
        printf("Error, in l2_addr_add\n");
        return rv;
    }

    rv = l2_addr_add(unit, &l2addr_2, _dest_mac_2, l2_learning_info.out_port , l2_learning_info.vlan_2);
    if (BCM_FAILURE(rv)) {
        printf("Error, in l2_addr_add\n");
        return rv;
    }

    /* set learning through cpu (shadow) */

    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    learn_msgs.flags = BCM_L2_LEARN_MSG_LEARNING;
    /* follow attriburtes set the destination port/group shadow messages are distributed to */
    /*learn_msgs.flags |= BCM_L2_LEARN_MSG_DEST_MULTICAST; - not needed as dest is port*/
    learn_msgs.dest_port = l2_learning_info.cpu_port;

    /* follow attributes set the encapsulation of the learning messages */
    /* learning message encapsulated with ethernet header */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0xab00;
    sal_memcpy(learn_msgs.src_mac_addr, _src_mac_address, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, _dest_mac_address, 6);
    /* default value of learn_msgs.vlan indicates packet is not tagged */
    /*learn_msgs.vlan/tpid/vlan_prio: not relevant as packet is not tagged*/
    rv = bcm_l2_learn_msgs_config_set(unit, &learn_msgs);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set \n");
        return rv;
    }

    /* set aging, so there'll be refresh */
    age_seconds = 10;
    rv = bcm_l2_age_timer_set(unit, age_seconds);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, bcm_l2_age_timer_set \n");
        return rv;
    }

    /* set learning mode to egress centralized */
    rv = learning_mode_set(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, learning_mode_set \n");
        return rv;
    }

    return BCM_E_NONE;
}

void l2_entry_event_handle(int unit,
	     bcm_l2_addr_t *l2addr,
	     int operation,
	     void *userdata)
{
    int rv = BCM_E_NONE;

    if (operation == BCM_L2_CALLBACK_LEARN_EVENT) {
        printf("BCM_L2_CALLBACK_LEARN_EVENT handle\n");
        l2_addr_print_entry(l2addr);
        rv = bcm_l2_addr_add(unit, l2addr);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_addr_add\n");
            return rv;
        }
    } else if (operation == BCM_L2_CALLBACK_AGE_EVENT) {
        printf("BCM_L2_CALLBACK_AGE_EVENT handle\n");
        l2_addr_print_entry(l2addr);
        rv = bcm_l2_addr_delete(unit, l2addr->mac, l2addr->vid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_addr_add\n");
            return rv;
        }
    }
}

int l2_entry_control_set(int unit, int flags) {
    int rv = BCM_E_NONE;
    int age_seconds = 60;
    bcm_switch_event_control_t event_control;

    rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_set\n");
        return rv;
    }

    /* Enabling aging to 60 sec */
    rv = bcm_l2_age_timer_set(unit, age_seconds);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_age_timer_set\n");
        return rv;
    }

    /* Diactivate log prints ARAD_INT_IHP_MACTEVENTREADY event */
    /*
    event_control.event_id = 439;
	event_control.index = 0;
	event_control.action = bcmSwitchEventLog; 
    rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_control, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_event_control_set\n");
        return rv;
    }
    */
    return rv;

}

int l2_entry_event_handle_register(int unit) {
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    rv = bcm_l2_addr_register(unit,l2_entry_event_handle, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_register \n");
        return rv;
    }

    return rv;
}
int l2_entry_event_handle_unregister(int unit) {
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    rv = bcm_l2_addr_unregister(unit,l2_entry_event_handle, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_register \n");
        return rv;
    }

    return rv;
}



