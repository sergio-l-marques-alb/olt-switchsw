/* $Id: cint_vlan_edit_port_default_lif.c,v 1.10 Broadcom SDK $
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
*/

/* 
* cint_vlan_edit_port_default_lif.c
*
* Set following setup:
* 
*    case 1: basic case,set vlan action for default lif
*     -   call vlan_edit_advanced_default_action(), 
*     -   Send single tag Ethernet traffic  to port_1:
*        -   VLAN tag: (0x8100,10)
*     -   Packet is transmitted to port port_2
*           -   Check the packets tag(0x8100,20) received on port_2 . If not, failed.
*          
*    case 1-1: In that  case,if you set another port default action, it will override the default action that you set before. 
*               Becasue all of ports are using the same default lif 
*     -   call overide_default_action_by_other_port(), 
*     -   Send single tag Ethernet traffic  to port_1:
*        -   VLAN tag: (0x8100,10)
*     -   Packet is transmitted to port port_2
*           -   Check the packets tag(0x8100,30 && 0x8100,10) received on port_2 . If not, failed.
*
*
*    case 2: different port can have different default action when they have different lif.
*      -   Send single tag Ethernet traffic  to port_1:
*          -   VLAN tag:  (0x8100,100)
*      -   Packet is transmitted to port port_2
*          -    Check the packets tag(0x8100,200 && 0x8100,100) received on port_2. If not, failed.
*
*      -   Send single tag Ethernet traffic  to port_2:
*          -   VLAN tag:  (0x8100,100)
*      -   Packet is transmitted to port_1
*          -    Check the packets tag(0x8100,300 && 0x8100,100) received on port_1. If not, failed.
*
*/
bcm_gport_t gport1;
bcm_gport_t gport2;

int vlan_edit_advanced_default_action(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    int port1 = port_1;
    int port2 = port_2;
    bcm_vlan_t vlan =10;
    bcm_pbmp_t pbmp,ubmp;
    BCM_PBMP_CLEAR(ubmp);
    
    rv = bcm_switch_control_set(unit,bcmSwitchPortVlanTranslationAdvanced,1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_switch_control_set  rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_vlan_create(unit,vlan);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create (vlan 100)rv %d\n", rv);
        return rv;
    }
	
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp,port1);
    BCM_PBMP_PORT_ADD(pbmp,port2);
    
    rv = bcm_vlan_port_add(unit,vlan,pbmp,ubmp);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_add (add to vlan 100) rv %d\n", rv);
        return rv;
    }
    
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t_init(&action);
    action.ot_outer = bcmVlanActionReplace;
    action.new_outer_vlan = 20;
    rv = bcm_vlan_port_default_action_set(unit,port1,&action);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_default_action_set ( add new_outer_vlan 100) ,rv %d\n", rv);
        return rv;
    }

    return rv;

}

int overide_default_action_by_other_port()
{
    int rv =BCM_E_NONE;
   bcm_vlan_action_set_t action;
   bcm_vlan_action_set_t_init(&action);
   action.ot_outer = bcmVlanActionAdd;
   action.new_outer_vlan = 30;
   rv = bcm_vlan_port_default_action_set(0,13,&action);/*using port 13 to overide port_1 default action*/
   if (rv != BCM_E_NONE){
       printf("Error, bcm_vlan_port_default_action_set ( add new_outer_vlan 100) ,rv %d\n", rv);
       return rv;
   }
   return rv;
}

int vlan_edit_default_action_for_different_port(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    int port1 = port_1;
    int port2 = port_2;
    bcm_vlan_t vlan =100;
    bcm_pbmp_t pbmp,ubmp;
    bcm_vlan_port_t vlan_port1;
    bcm_vlan_port_t vlan_port2;
    bcm_vswitch_cross_connect_t gports;
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t action2;
    bcm_vlan_action_set_t action_e;
    bcm_vlan_action_set_t action_e2;
    
    rv = bcm_vlan_create(unit,vlan);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create (vlan 100)rv %d\n", rv);
        return rv;
    }
    
    /* port1,2 add vlan 100 with tag mode*/ 
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp,port1);
    BCM_PBMP_PORT_ADD(pbmp,port2);
    
    rv = bcm_vlan_port_add(unit,vlan,pbmp,ubmp);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_add (add to vlan 100) rv %d\n", rv);
        return rv;
    }
    
    bcm_vlan_port_t_init(vlan_port1);
    vlan_port1.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port1.port =port1;
    bcm_vlan_port_create(unit,&vlan_port1);
    if (rv != BCM_E_NONE){
    printf("Error,bcm_vlan_port_create ( 1) ,rv %d\n", rv);
    return rv;
    }
    
    bcm_vlan_port_t_init(vlan_port2);
    vlan_port2.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port2.port =port2;
    bcm_vlan_port_create(unit,&vlan_port2);
    if (rv != BCM_E_NONE){
        printf("Error,bcm_vlan_port_create ( 2) ,rv %d\n", rv);
        return rv;
    }
    
    gport1 = vlan_port1.vlan_port_id;
    gport2 = vlan_port2.vlan_port_id;
    
    /* Cross connect the 2 LIFs */
    gports.port1 = vlan_port1.vlan_port_id;
    gports.port2 = vlan_port2.vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }
    
    bcm_vlan_action_set_t_init(&action);
    action.ot_outer = bcmVlanActionAdd;
    action.new_outer_vlan = 200;
    rv = bcm_vlan_port_default_action_set(unit,port1,&action);
    /* rv = bcm_vlan_translate_action_create(unit,vlan_port1.vlan_port_id, bcmVlanTranslateKeyPortOuter,BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);*/
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_default_action_set ( add new_outer_vlan 100) ,rv %d\n", rv);
        return rv;
    }
    
    bcm_vlan_action_set_t_init(&action_e);
    action_e.ot_outer = bcmVlanActionNone;
    rv = bcm_vlan_translate_egress_action_add(unit, vlan_port2.vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action_e);
    if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_egress_action_add %s\n", bcm_errmsg(rv));
    return rv;
    }
    
    bcm_vlan_action_set_t_init(&action2);
    action2.ot_outer = bcmVlanActionAdd;
    action2.new_outer_vlan = 300;
    /* rv = bcm_vlan_translate_action_create(unit,vlan_port2.vlan_port_id, bcmVlanTranslateKeyPortOuter,BCM_VLAN_INVALID, BCM_VLAN_NONE, &action2);*/
    rv = bcm_vlan_port_default_action_set(unit,port2,&action2);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_default_action_set ( add new_outer_vlan 100) ,rv %d\n", rv);
        return rv;
    }
    
    bcm_vlan_action_set_t_init(&action_e2);
    action_e2.ot_outer = bcmVlanActionNone;
    rv = bcm_vlan_translate_egress_action_add(unit, vlan_port1.vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action_e2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_translate_egress_action_add %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;

}

int  clean_vlan_edit_default_lif(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    int port1 = port_1;
    int port2 = port_2;
    bcm_vlan_t vlan1 =10;
    bcm_pbmp_t pbmp,ubmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp,port1);
    BCM_PBMP_PORT_ADD(pbmp,port2);

    rv = bcm_vlan_port_default_action_delete(unit, port1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_default_action_delete ( port1) ,rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_vlan_port_default_action_delete(unit, 13);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_default_action_delete ( 13) ,rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_vlan_port_remove(unit,vlan1,pbmp);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_vlan_destroy(unit, vlan1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_destroy ,rv %d\n", rv);
        return rv;
    }
    
    return rv;
}

int  clean_vlan_edit_default_lif_for_different_port(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    int port1 = port_1;
    int port2 = port_2;
    bcm_vlan_t vlan1 =100;
    bcm_pbmp_t pbmp,ubmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp,port1);
    BCM_PBMP_PORT_ADD(pbmp,port2);

    rv = bcm_vlan_port_default_action_delete(unit, port1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_default_action_delete ( port1) ,rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_vlan_port_default_action_delete(unit, port2);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_default_action_delete ( port1) ,rv %d\n", rv);
        return rv;
    }

    bcm_vlan_port_destroy(unit,gport1);
    bcm_vlan_port_destroy(unit,gport2);
    
    rv = bcm_vlan_port_remove(unit,vlan1,pbmp);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_vlan_destroy(unit, vlan1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_destroy ,rv %d\n", rv);
        return rv;
    }
    
    return rv;
}




