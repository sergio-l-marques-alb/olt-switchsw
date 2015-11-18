/*~~~~~~~~~~~~~~~~~~~~~~~Bridge Router: IPMC, Multi Device~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_ipmc_example_multi_device.c,v 1.6 Broadcom SDK $
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
 * 
 * File: cint_ipmc_example_multi_device.c
 * Purpose: Example demonstartes the MPLS label-switching router configuration, 
 *          including label swap in ILM and egress object pointing to the L3 interface
 *          for a two-device system.
 *
 * More details about the configuration below can be found in cint_ipmc_example.c
 * The same configurations described will be executed on two units, 
 * where unit 0 is the ingress unit and unit 1 is the egress unit.
 *
 * Send L3 multicast traffic between two devices
 * Configuration: 
 *   Destination IP address (MC): 224.224.224.1
 *   Src IP address: 192.128.1.1
 *   Destination mac: 00:00:00:00:ab:1d
 *   Source port is unit 0, port 1
 * Expected output: 
 *   Traffic on unit 1, port 1
 *
 * Note: You can specify different parameters for these examples including: 
 *       DA, SA IPV4, multicast group ID, and destination port.
 */

/* Add an IPv4 MC entry
 *  This entry contains the MC IP, SRC IP and IPMC Group Index.
 *  You need to create this group before you call this function.
 */
int add_ip4mc_entry(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int vlan, bcm_multicast_t mc_id){
    bcm_error_t rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    int flags =  BCM_IPMC_SOURCE_PORT_NOCHECK | 
                        BCM_IPMC_REPLACE;

    /* Init the ipmc_addr structure and fill the requiered fields. */
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.s_ip_addr = src_ip;
    data.vid = vlan;
    data.flags = flags;
    data.group = mc_id;

    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, in ipmc_add, mc_ip $mc_ip src_ip $src_ip\n");
        return rv;
    }

    return rv;
}


/* Creates an IP MC Group
 *  Here we create the actual MC group.
 *  We must run this function before creating the IP MC entry
 *  The creation of IPv4 and IPv6 MC Groups is identical
 */
bcm_multicast_t create_ingress_ip_mc_group(int unit, int ipmc_index, int vlan){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_info_t gport_info;
    bcm_cosq_gport_type_t gport_type;
    bcm_gport_t dest_gport;
    bcm_l3_intf_t intf;
    int cud;
    int port;
    bcm_gport_t dest_erp;
    bcm_gport_t temp;
    int count_erp;
    gport_type = bcmCosqGportTypeLocalPort; 
    
    bcm_multicast_t mc_id = ipmc_index;
    /* The TYPE_L3 Flag will make this group an IPMC group */
    int flags = BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP;

    /* Create the group */
    rv = bcm_multicast_create(unit, flags, &mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_create, mc_id $mc_id flags $flags\n");
        return rv;
    }

    /* CUD on multicast ingress must be the multicast egress group id */
    cud = 6000;
    
    /* Call ERP system port */      
    rv = bcm_port_internal_get(/*unit*/ 1,BCM_PORT_INTERNAL_EGRESS_REPLICATION,1,&temp,&count_erp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    } 
    
    if (count_erp == 0) {
        printf("ERP is not enable, Multicast creation failed \n");
        bcm_multicast_destroy(unit,ipmc_index);
        return BCM_E_PARAM;
    }
    
    if (!BCM_GPORT_IS_LOCAL(temp)){
        printf("erp is not a local port\n");
        return -1;
    }
    else
    {
        port = BCM_GPORT_LOCAL_GET(temp);
        print port;
        BCM_GPORT_MODPORT_SET(dest_erp,/*module*/ 1,port);    
    }
    
    rv = bcm_multicast_ingress_add(unit,ipmc_index,dest_erp,cud);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    } 

    return mc_id;
}


/* Creates an IP MC Group
 *  Here we create the actual MC group.
 *  We must run this function before creating the IP MC entry
 *  The creation of IPv4 and IPv6 MC Groups is identical
 */
bcm_multicast_t create_egress_ip_mc_group(int unit, int ipmc_index, int dest_local_port_id, int vlan){
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_info_t gport_info;
    bcm_cosq_gport_type_t gport_type;
    bcm_gport_t dest_gport;
    bcm_if_t cud;
    gport_type = bcmCosqGportTypeLocalPort; 
    
    bcm_multicast_t mc_id = ipmc_index;
    /* The TYPE_L3 Flag will make this group an IPMC group */
    int flags = BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_TYPE_L3;

    /* Create the group */
    rv = bcm_multicast_create(unit, flags, &mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast_create, mc_id $mc_id flags $flags\n");
        return rv;
    }

    /* Now we will add a destination to the created group */
    BCM_GPORT_LOCAL_SET(dest_gport,dest_local_port_id); 
    rv = bcm_multicast_l3_encap_get(unit, mc_id, dest_gport, vlan,&cud);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_petra_multicast_l3_encap_get, mc_group mc_group \n");
        return rv;
    }
    rv = bcm_multicast_egress_add(unit,mc_id,dest_gport,cud);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_egress_add, ipmc_index $ipmc_index dest_gport $dest_gport \n");
        return rv;
    }

    return mc_id;
}


/* Main IPv4 MC Example */
int ipv4_mc_example(){
    /* mc ip address 224.224.224.1 */
    /* src ip address 192.128.1.1 */
    /* Destination is port 2 */
    bcm_error_t rv = BCM_E_NONE;
    int ingress_intf;
    bcm_gport_t temp;

    /* Set parameters: User can set different parameters.= */
    bcm_ip_t mc_ip = 3772833793;
    bcm_ip_t src_ip = 3229614337;
    int vlan = 1;
    int ingress_ipmc_index = 12;
    int egress_ipmc_index = 6000;
    
    int dest_local_port_id = 1;
    
    int source_local_port_id = 1;
    bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};

    /* Create the ingress IP MC Group */
    bcm_multicast_destroy(0,ingress_ipmc_index);
    bcm_multicast_t in_mc_id = create_ingress_ip_mc_group(0, ingress_ipmc_index, vlan);

    /* Create the egress IP MC Group */
    bcm_multicast_destroy(1,egress_ipmc_index);
    bcm_multicast_t eg_mc_id = create_egress_ip_mc_group(1, egress_ipmc_index, dest_local_port_id, vlan);

    /* Create ingress intf, for ethernet termination */
    /* create_l3_intf(0, source_local_port_id, vlan, mac_l3_ingress, &ingress_intf); */
    create_l3_intf_s intf;
    intf.vsi = vlan;
    intf.my_global_mac = mac_l3_ingress;
    intf.my_lsb_mac = mac_l3_ingress;
    intf.skip_mymac = 1;
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }


    /* Create the IP MC entry */
/*   
    int cud = 1;                             
    BCM_GPORT_LOCAL_SET(temp,dest_local_port_id);
    rv = bcm_multicast_egress_add(1,eg_mc_id,temp,cud);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }      
*/
    rv = add_ip4mc_entry(1, mc_ip, src_ip, vlan, eg_mc_id);
    if (rv != BCM_E_NONE) return rv;
    rv = add_ip4mc_entry(0, mc_ip, src_ip, vlan, in_mc_id);
    if (rv != BCM_E_NONE) return rv;

    return rv;
}


