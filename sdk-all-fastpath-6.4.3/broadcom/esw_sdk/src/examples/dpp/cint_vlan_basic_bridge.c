/* $Id: cint_vlan_basic_bridge.c,v 1.6 Broadcom SDK $
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
* File: cint_vlan_basic_bridge.c
* Purpose: Example of Basic Bridging VLAN membership
*
* Calling sequence:
*  1. Create vlan and Add port for basic bridging, Add l2 address for unicast.
*        - Call bcm_vlan_create()
*        - Call bcm_vlan_port_add()
*        - Call bcm_l2_addr_add()
*  2. Remove port from vlan and Verify the traffic
*        - Call bcm_vlan_port_remove()
*  3. Create vlan and vsi, Create the vlan_port with vlan and vsi, Add port for bridging, Add l2 address for unicast.
*        - Call bcm_vlan_create()
*        - Call bcm_vlan_port_create()
*        - Call bcm_vlan_port_add()
*        - Call bcm_l2_addr_add()
*  4. Remove port from vlan and Verify the traffic
*        - Call bcm_vlan_port_remove()
*  5. Create and remove a specific number of vlan_port.
*        - Call vlan_basic_bridge_vlan_port_test()
*
* Traffic:
*  1. For case vlan=vsi=10.
*  unicast:
*      -   Send Ethernet packet  to port 1:
*          -   SA 00:00:00:00:00:05
*          -   DA 00:00:00:00:00:02
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is transmitted to port 2.
*
*  Flooding 
*      -   Send Ethernet packet to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is flooded to port 2(as DA is not known).
*
*  2. For case vlan=20 vsi=30.
*  unicast:
*      -   Send Ethernet packet  to port 1:
*          -   SA 00:00:00:00:00:05
*          -   DA 00:00:00:00:00:02
*          -   VLAN tag: VLAN tag type 0x8100, VID =20
*      -   Packet is transmitted to port 2.
*
*  Flooding 
*      -   Send Ethernet packet to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =20
*      -   Packet is flooded to port 2(as DA is not known).
*
* To Activate Above Settings Run: 
*      BCM> cint ../../../../src/examples/dpp/cint_port_tpid.c 
*      BCM> cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
*      BCM> cint ../../../../src/examples/dpp/internal/cint_vlan_basic_bridge.c
*      BCM> cint
*      cint> vlan_basic_bridge_run(unit, vid);
*      cint> vlan_basic_bridge_run2(unit, vid, vsi);
*/

struct vlan_bridge_info_s{
    int ports[10];
    bcm_port_t gport[10];
    int nof_ports;
};

vlan_bridge_info_s vlan_bridge_info;

int vlan_basic_bridge_init(int unit, int port1, int port2){
    vlan_bridge_info.ports[0] = port1;
    vlan_bridge_info.ports[1] = port2;
    vlan_bridge_info.nof_ports = 2;

    return BCM_E_NONE;
}

int vlan_basic_bridge_port_add(int unit, bcm_vlan_t vid, int port){
    int rv;
    bcm_pbmp_t pbmp;
    
    /* add port to vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_port_add(unit, vid, pbmp);    
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    return rv;
}

int vlan_basic_bridge_port_remove(int unit, bcm_vlan_t vid, int port){
    int rv;
    bcm_pbmp_t pbmp;
    bcm_mac_t mac;

    /* remove port from vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_port_remove(unit, vid, pbmp); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    return rv;
}

/*vsi == vlan*/
int vlan_basic_bridge_run(int unit, bcm_vlan_t vid){
    int rv;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    bcm_mac_t mac;
    int port_id;

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    /* add port to vlan */
    BCM_PBMP_CLEAR(pbmp);
    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        BCM_PBMP_PORT_ADD(pbmp, vlan_bridge_info.ports[port_id]);
    }
    BCM_PBMP_CLEAR(ubmp);
    rv = bcm_vlan_port_add(unit, vid, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
        mac[5] = 2;
        rv = vlan_basic_bridge_add_l2_addr_to_gport(unit, vlan_bridge_info.ports[port_id], mac, vid);
        if (rv != BCM_E_NONE) {
          printf("Error, vlan_bridge_add_l2_addr_to_gport\n");
          return rv;
        }
    }

    return rv;
}

int vlan_basic_bridge_revert(int unit, bcm_vlan_t vid){
    int rv;
    int port_id;
    bcm_mac_t mac;

    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
        mac[5] = vlan_bridge_info.ports[port_id];
        rv = bcm_l2_addr_delete(unit, mac, vid);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
          printf("Error, bcm_l2_addr_delete rv %d\n", rv);
          return rv;
        }
    }

    rv = bcm_vlan_destroy(unit, vid);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)){
        printf("Error, bcm_vlan_destroy unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    return rv;
}

/*
 * add L2 entry points to the given gport
 */
int
vlan_basic_bridge_add_l2_addr_to_gport(int unit, int gport_id, bcm_mac_t mac1, bcm_vlan_t v1){
    int rv;
    bcm_gport_t g1;
    /*bcm_mac_t mac1;*/
    bcm_l2_addr_t l2_addr1;
    /*bcm_vlan_t v1;*/

    /* local: MACT, add entry points to local-port */
    bcm_l2_addr_t_init(&l2_addr1,mac1,v1); 
    l2_addr1.flags = 0x00000020; /* static entry */
    g1 = gport_id;
    l2_addr1.port = g1;
    rv = bcm_l2_addr_add(0,&l2_addr1);

    return rv;
}

int vlan_basic_bridge_vlan_port_create(int unit, bcm_port_t port, bcm_vlan_t vid, 
    bcm_vlan_t vsi, bcm_gport_t *gport){
    int rv = 0; 
    bcm_vlan_port_t vlan_port;

    if (gport == NULL) {
        return BCM_E_PARAM;
    }
    
    vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.match_vlan  = vid;
    vlan_port.egress_vlan = vid;
    vlan_port.vsi  = vsi;
    vlan_port.port = port;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create unit %d, vid %d, vsi %d, rv %d\n", unit, vid, vsi, rv);
        return rv;
    }

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_vlan_port_create_to_translation\n");
            return rv;
        }
    }

    *gport = vlan_port.vlan_port_id;

    return rv;
}

int vlan_basic_bridge_port_remove2(int unit, bcm_vlan_t vid, bcm_vlan_t vsi, int port){
    int rv;
    bcm_pbmp_t pbmp;
    bcm_mac_t mac;

    /* remove port from vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_port_remove(unit, vid, pbmp); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    return rv;
}

/*vsi != vlan*/
int vlan_basic_bridge_run2(int unit, bcm_vlan_t vid, bcm_vlan_t vsi){
    int rv;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    int port_id;
    bcm_gport_t gport;
    bcm_mac_t mac;
    bcm_port_t port;

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    rv = bcm_vlan_create(unit, vsi);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vsi, rv);
        return rv;
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
       are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
            port = vlan_bridge_info.ports[port_id];
            port_tpid_init(port, 1, 1);
            rv = port_tpid_set(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, port_tpid_set\n");
                print rv;
                return rv;
            }
        }

        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        port = vlan_bridge_info.ports[port_id];
        rv = vlan_basic_bridge_vlan_port_create(unit, port, vid, vsi, &gport);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_basic_bridge_vlan_port_create failed, rv %d\n", rv);
        }

        vlan_bridge_info.gport[port_id] = gport;

        /* add port to vlan */
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, port);
        BCM_PBMP_CLEAR(ubmp);
        rv = bcm_vlan_port_add(unit, vid, pbmp, ubmp);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
            return rv;
        }

        mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
        mac[5] = 2;
        rv = vlan_basic_bridge_add_l2_addr_to_gport(unit, port, mac, vsi);
        if (rv != BCM_E_NONE) {
          printf("Error, vlan_bridge_add_l2_addr_to_gport\n");
          return rv;
        }

    }

    return rv;
}

int vlan_basic_bridge_revert2(int unit, bcm_vlan_t vid, bcm_vlan_t vsi){
    int rv;
    int port_id;
    bcm_mac_t mac;

    for (port_id = 0; port_id < vlan_bridge_info.nof_ports; port_id++) {
        rv = bcm_vlan_port_destroy(unit, vlan_bridge_info.gport[port_id]);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
            printf("Error, bcm_vlan_port_destroy unit %d, gport 0x%x, rv %d\n", unit, vlan_bridge_info.gport[port_id], rv);
            return rv;
        }

        mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
        mac[5] = vlan_bridge_info.ports[port_id];
        rv = bcm_l2_addr_delete(unit, mac, vsi);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
          printf("Error, bcm_l2_addr_delete rv %d\n", rv);
          return rv;
        }
    }

    rv = bcm_vlan_destroy(unit, vid);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
        printf("Error, bcm_vlan_destroy unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    rv = bcm_vlan_destroy(unit, vsi);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
        printf("Error, bcm_vlan_destroy unit %d, vsi %d, rv %d\n", unit, vsi, rv);
        return rv;
    }

    return rv;
}

/*When same_interface_filter is diabled, two packets are returned when flood occurs(unknowd DA) 
 *When the filter is enables, the packet sent to the tx port is dropped(and you recieve 1 packet)
 *We disabled the filter so we now receive two packets during flood and one during unicast to a known DA.
 *This makes the cint's Dvapi test more informative"
 */
int set_same_interface_filter(int unit, int port, int enabled){
    int rv = 0;
    
    bcm_port_control_set(unit, port, bcmPortControlBridge, enabled);
    if ((rv != BCM_E_NONE)) {
        printf("Error, bcm_port_control_set failed: %d\n", rv);
        return rv;
    } else if (enabled){
        printf("Same interface filter is enabled successfully\n");
    } else {
        printf("Same interface filter is disabled successfully\n");   
    }
    return rv;
}

/* 
 * Verify a specific number of AC-LIFs can be successfully created and removed.
 * Example: vlan_basic_bridge_vlan_port_test(0, 15, 1, 4001, 1, 9);
 *          This case create and remove 32,000(4000*8) AC-LIFs.
 */
int 
vlan_basic_bridge_vlan_port_test(int unit, bcm_port_t port, int outerLoVlan, int outerHighVlan, int innerLoVlan, int innerHighVlan)
{
    int rv1 = 0, rv2 = 0;
    bcm_vlan_t outervlan, innervlan;
    bcm_vlan_port_t vlan_port;
    int vpreq = 0, vpaddcount = 0, vp = 0;
    bcm_gport_t *pvpgport = NULL;

    if (outerLoVlan >= outerHighVlan || innerLoVlan >= innerHighVlan)
        return BCM_E_PARAM;
    vpreq = (outerHighVlan - outerLoVlan) * (innerHighVlan - innerLoVlan);
    if (vpreq > 32768) {
        printf("Please test vlan port less than 32768!\n");
        return BCM_E_PARAM;
    }
    
    pvpgport = sal_alloc(4*vpreq, "Cint: vlan_basic_bridge_vlan_port_test");
    if (NULL == pvpgport) {
        printf("No memory for allocation pvpgport buffer!");
        return BCM_E_MEMORY;
    }    
		
    printf("Start test vlan port\n");
    /*
     * Create vlan port.
     */
    for (outervlan = outerLoVlan; outervlan < outerHighVlan; outervlan++) {
        for (innervlan = innerLoVlan; innervlan < innerHighVlan; innervlan++) {
            bcm_vlan_port_t_init(&vlan_port);
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            vlan_port.port = port;		
            vlan_port.match_vlan = outervlan;
            vlan_port.match_inner_vlan = innervlan;
            rv1 = bcm_vlan_port_create(unit, &vlan_port);
            if(rv1 != BCM_E_NONE) {
                printf("Error, bcm_vlan_port_create unit %d, port %d, ovid %d, ivid %d, vpaddcount %d, rv %d\n", unit, port, outervlan, innervlan, vpaddcount, rv1);
                break;
            }
            pvpgport[vpaddcount] = vlan_port.vlan_port_id;
            vpaddcount++;
            if ((vpaddcount%1000) == 0)
                printf(".");
        }
        if (rv1 != BCM_E_NONE)
            break;
    }

    /* 
     * Remove vlan port.
     */
    for (vp = 0; vp < vpaddcount; vp++) {
        rv2 = bcm_vlan_port_destroy(unit, pvpgport[vp]);
        if(rv2 != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy unit %d, vpdelcount %d, rv %d\n", unit, vp, rv2);
            break;
        }
        if ((vp%1000) == 0)
            printf(".");
    }

    printf("\nvlan port create %d, destroy %d\n", vpaddcount, vp);

    sal_free(pvpgport);
    	
    return rv1|rv2;     
}

