/*
 * $Id:$
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
/* This funciton is the main function. It configures the HW for egress MTU violation and for IPv4 packet routing. Then, it creates the bcmRxTrapEgMtuFilter
   trap and uses the bcm_field_qualify_RxTrapCode in order to check the PMF handling of MTU violation */
int egr_mtu_run(int unit, int in_port, int out_port){
    int rv;
    int mtu_value;
    int gport;

    /* This function will enable the packet to be routed from in_port to in-port */
    rv = basic_example(&unit, 1, in_port,in_port) ;
    if (rv != BCM_E_NONE)
    {
	printf("Error in basic_example\n");
	return rv;
    }

    /* This function set the MTU violation profile to "catch" IPv4 packets */
    rv = mtu_check_header_code_enable(0, bcmForwardingTypeIp4Ucast);
    if (rv != BCM_E_NONE)
    {
        printf("Error in basic_example\n");
        return rv;
    }

    /* The use for drop*/
    /* rv = egr_mtu_trap(unit,BCM_GPORT_BLACK_HOLE, in_port)p -rn ; */
    
     /* the packet is supposed to get routed through in_port and we want to redirect it to out_port*/
    rv = egr_mtu_trap(unit,out_port, in_port);
    if (rv != BCM_E_NONE)
    {
       printf("Error in egr_mtu_trap\n");
        return rv;
    }

    /* Testing the rx_trap_code_qualifier */
    /* 
    rv = egr_mtu_init_group(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in egr_mtu_init_group\n");
        return rv;
    }
    rv = egr_mtu_add_filter(unit,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in egr_mtu_add_filter\n");
        return rv;
    } 
    */
    return rv;
}

int egr_mtu_trap(int unit, bcm_gport_t destination, int out_port)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int trap_id;
    int mtu_value;
    int mtu_limit = 200;

    /* setting the mtu value on the out_port*/
    rv = bcm_port_l3_mtu_set(0, out_port, mtu_limit);
    if (rv != BCM_E_NONE) {
	printf("Error, bcm_port_l3_mtu_set : 15\n");
	return rv;
    }

    rv = bcm_port_l3_mtu_get(0, out_port, &mtu_value);     
    printf(" out_port %d  mtu = %d\n",out_port, mtu_value);

    /* setting the Out-lif (just to get an encap id - needs more configuration process in order to change the Vlan id for example) */
    bcm_vlan_port_t vlan_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.flags = 0;
    vlan_port.port = destination ;
    vlan_port.egress_vlan = 500;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_create\n");
        print rv;
    }
    else{
        printf("Created vlan port %d\n", vlan_port.vlan_port_id);
    }

    /*Defining the trap */

    sal_memset(&config, 0, sizeof(config));
    config.flags |= BCM_RX_TRAP_UPDATE_DEST |BCM_RX_TRAP_UPDATE_PRIO | BCM_RX_TRAP_UPDATE_ENCAP_ID;
    config.encap_id = vlan_port.encap_id;
    config.trap_strength = 5;
    config.dest_port = destination;  
    config.prio=7;

    /* other options */
    /*
      bcm_gport_t local_gport;
      BCM_GPORT_LOCAL_SET(local_gport, 0);
      config.dest_port = local_gport; 
      config.dest_port= BCM_GPORT_BLACK_HOLE;  
    */ 

    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapEgMtuFilter, &trap_id);

    printf("mtu trap id is %d \n", trap_id);
    if (rv != BCM_E_NONE) {
	printf("Error, in trap create, trap id %d;  \n", trap_id);
	return rv;
    }

    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
	printf("Error, in trap set \n");
	return rv;
    }
    
    return rv;
}


int egr_mtu_init_group(int unit)
{
    int rc;
    int priority_group = 3;
    bcm_field_qset_t qset;
    bcm_field_group_t grp;
    bcm_field_aset_t aset;

    /* Define the QSET */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyRxTrapCode);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
    /*  Create the Field group */
    rc = bcm_field_group_create(unit, qset, priority_group, &grp);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_group_create\n");
        bcm_field_group_destroy(unit, grp);
        return -1;
    }

      printf("grp is %d",grp);
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);   
    /*  Attach the action set */
    rc = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_group_action_set\n");
        bcm_field_group_destroy(unit, grp);
        return -1;
    }

    rc = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_group_install\n");
        bcm_field_group_destroy(unit, grp);
        return -1;
    }        /* Define the QSET */

    printf("Init MTU Group successful, group:%d\n", grp);
    return 0;
}

int egr_mtu_add_filter(int unit, int grp)
{
    int rc;
    bcm_field_entry_t entry;
    bcm_rx_trap_t data;
    
    rc = bcm_field_entry_create(unit, grp, &entry);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_entry_create\n");
        return -1;
    }
    /* VSI */
    rc = bcm_field_qualify_RxTrapCode(unit, entry, bcmRxTrapEgMtuFilter);
    if (BCM_E_NONE != rc) {
       printf("Error in bcm_field_qualify_RxTrapCode\n");
      return rc;
    }

    /* checking that RxtrapCode get works */
    rc =  bcm_field_qualify_RxTrapCode_get( unit,  entry, &data);
    if (data != bcmRxTrapEgMtuFilter) {
       printf("Error in bcm_field_qualify_RxTrapCode_get\n");
      return rc;
    } 
   
    /* Drop the packet */
    rc = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_action_add\n");
        return rc;
    }

    rc = bcm_field_entry_install(unit, entry);
    if (BCM_E_NONE != rc) {
        printf("Error in bcm_field_entry_install\n");
        return rc;
    }

    printf("Add MTU Filter successful");
    return 0;
}
