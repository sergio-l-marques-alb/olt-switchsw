/* $Id: cint_arp_traps.c,v 1.2 Broadcom SDK $
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
how to run:
 
cint;
cint_reset();
exit;
cint ../../../../src/examples/dpp/cint_trap_mgmt.c
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_arp_traps.c
cint
 
trap ARP reply on specific incomming router interface 
arp_reply_with_mymac(0,13,BCM_GPORT_LOCAL_CPU);
 
 Temp fixes 
mod IHB_FLP_PROGRAM_SELECTION_CAM 12 1 program=1
mod IHP_VTT_2ND_KEY_PROG_SEL_TCAM 21 1 PROGRAM=11 PACKET_FORMAT_QUALIFIER_2_MASK=0x7ff PACKET_FORMAT_QUALIFIER_1=0x500
mod IHP_VTT_2ND_LOOKUP_PROGRAM_1 11 1 PROCESSING_CODE=7
 
trap ARP request on specific incomming port
arp_request(0,13,BCM_GPORT_LOCAL_CPU,1);
 
trap ARP request with TPA = MY-IP 
arp_reply_request_with_my_ip(0,13,BCM_GPORT_LOCAL_CPU);
 
trap any ARP packet on specific incomming port
arp_request(0,13,BCM_GPORT_LOCAL_CPU,0);
 
 
*/ 


/* 
BCM API: 
 
RX-traps: 
    bcmRxTrapArpReply: ARP reply with My MAC
    bcmRxTrapArp: ARP request, if DA ignored then capture also ARP request
    bcmRxTrapArpMyIp: ARP request with TPA = My-IP
 
    Global switch controls

    bcmSwitchArpReplyMyStationL2ToCPU: ignore ARP trap, relevant for bcmRxTrapArp.
    bcmSwitchArpRequestMyStationIPToCPU: add IP address to be as considered my IP for ARP requests. relevant for bcmRxTrapArpMyIp.
    bcmSwitchArpRequestMyStationIPFwd: remove above IP .
    
    port control:
    bcmSwitchArpRequestToCpu: enable ARP trap on incomming port relevant for bcmRxTrapArp,bcmRxTrapArpMyIp.
 
 */

/* debug prints */



/* 
 * trap ARP reply to out-port: 
 *  identification:
 *      ARP-packet: Ethernet packet with Ether-Type = 0x806.
 *      Recieved on L3-interface with My-MAC i.e. has My-MAC on the resolved VSI.
 *  
 *  handling:
 *      - using rx-trap bcmRxTrapArpReply
 */
int arp_reply_with_mymac(int unit, int in_port, int out_port){
    int rv;
    int ing_intf_in; 
    int flags = 0;
    int in_vlan = 1;
    int vrf = 1;
    int open_vlan=1;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    int trap_strength = 6;/* set stronger than arp_request, as pkt may captured with arp_request() when only_bc_pkts = 0*/

    /*** create ingress router interface: identified according to <VLAN,MAC> ***/
    rv = create_l3_intf(unit,flags,open_vlan,in_port,in_vlan,vrf,mac_address, &ing_intf_in); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_port=%d, \n", in_port);
    }
    /* configure ARP reply trap to trap to out-port */
    rv = trap_type_set(unit,bcmRxTrapArpReply,trap_strength,out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, setting trap: bcmRxTrapArpReply, \n");
    }
   
    return rv;
}



/* 
 * trap ARP request to out-port: 
 *  identification:
 *      ARP-packet: Ethernet packet with Ether-Type = 0x806, 
 *      trap enabled in in_port.
 *      if only_bc_pkts = 1 --> DA = BC (all ones)
 *      if only_bc_pkts = 0 --> ANY DA (CAPTURE BOTH REQUEST AND REPLY)
 *  
 *  handling:
 *      - using rx-trap bcmRxTrapArpReply
 *  
 */
int arp_request(int unit, int in_port, int out_port, int only_bc_pkts){
    int rv;
    int trap_strength = 5;
    int enable = 1;
    
    /* enable ARP trap on incomming port */
    rv = bcm_switch_control_port_set( unit, in_port, bcmSwitchArpRequestToCpu, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_port_set \n");
    }

    /* configure ARP request to trap to out-port */
    rv = trap_type_set(unit,bcmRxTrapArp,trap_strength,out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, setting trap: bcmRxTrapArp, \n");
    }

    /* trap ARP packet regardless DA */
    rv = bcm_switch_control_set( unit, bcmSwitchArpReplyMyStationL2ToCPU, only_bc_pkts);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
    }


    return rv;
}


/* 
 * trap ARP request with TPA = myIP
 *  identification:
 *      ARP-packet: Ethernet packet with Ether-Type = 0x806, DA = BC (all ones)
 *      trap enabled in in_port (same enable as arp_reply_request)
 *      ARP.TPA = specific IP address
 *  handling:
 *      - using rx-trap bcmRxTrapArpMyIp
 */
int arp_reply_request_with_my_ip(int unit, int in_port, int out_port){
    int rv;
    int trap_strength = 5;
    int my_ip = 0x0a000505; /* 10.0.5.5 */
    int enable = 1;
    
    /* enable ARP request trap on incomming port */
    rv = bcm_switch_control_port_set( unit, in_port, bcmSwitchArpRequestToCpu, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_port_set \n");
    }

    /* enable ARP request trap on incomming port */
    rv = bcm_switch_control_set( unit, bcmSwitchArpRequestMyStationIPToCPU, my_ip);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_port_set \n");
    }

    /* configure ARP request to trap to out-port */
    rv = trap_type_set(unit,bcmRxTrapArpMyIp,trap_strength,out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, setting trap: bcmRxTrapArp, \n");
    }

    return rv;
}




