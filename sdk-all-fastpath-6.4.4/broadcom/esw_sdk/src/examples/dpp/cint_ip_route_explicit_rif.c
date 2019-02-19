/* $Id: cint_ip_route_explicit_rif.c,v 1.10 Broadcom SDK $
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


 
 
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */

struct l3_rif_s{
    int sys_port;
    int vlan;
    int vrf;
    int rif;
    bcm_mac_t my_lsb_mac;
    bcm_gport_t gport;
};


/*  Main Struct  */
struct ip_route_explicit_rif_s{
    l3_rif_s  rif[NUMBER_OF_PORTS];   /* rif[0] - used for l3 'in' interface, rif[1] - used for l3 'out' interface */
    l3_ipv4_route_entry_utils_s route_entry;
    bcm_mac_t global_mac;
    bcm_mac_t next_hop_mac;
};


/* Initialization of global struct*/
/*                                                   | sys_port | vlan | vrf | rif | my_lsb_mac(only 12 my-mac LSBs  are relevant   | gport |   */
ip_route_explicit_rif_s g_ip_route_explicit_rif = { {{     14,       10,    0,    5, { 0x00, 0x00, 0x00, 0x00, 0x01, 0x11 }      , -1    },   /* configuration of ingress port*/
                                                     {     13,      200,    0,   11, { 0x00, 0x00, 0x00, 0x00, 0x02, 0x22 }      , -1    }},  /* configuration of egress port*/
                                                        /* address   |   mask   */
                                                          {0x7fffff00, 0xfffffff0}, /* routing entry*/
                                                          g_l2_global_mac_utils, /* global mac taken from cint_utils_l2.c only 36 MSB bits are relevant*/
                                                          {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d} /* next hop mac*/};



/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */
/* Initialization of main struct
 * Function allow to re-write default values 
 *
 * INPUT: 
 *   params: new values for g_ip_route_explicit_rif
 */
int ip_route_explicit_rif_init(int unit, ip_route_explicit_rif_s *param){

    if (param != NULL) {
       sal_memcpy(&g_ip_route_explicit_rif, param, sizeof(g_ip_route_explicit_rif));
    }

    advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    if (!advanced_vlan_translation_mode ) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* 
 * Return explicit_rif information
 */
void ip_route_explicit_rif_struct_get(int unit, ip_route_explicit_rif_s *param){

    sal_memcpy( param, &g_ip_route_explicit_rif, sizeof(g_ip_route_explicit_rif));

    return;
}

/* This function runs the main test function with given ports
 *  
 * INPUT: unit     - unit
 *        in_port  - ingress port 
 *        out_port - egress port
 */
int ip_route_explicit_rif_with_ports__start_run(int unit,  int in_port, int out_port){
    int rv;

    ip_route_explicit_rif_s param;

    ip_route_explicit_rif_struct_get(unit, &param);

    param.rif[IN_PORT].sys_port = in_port;
    param.rif[OUT_PORT].sys_port = out_port;

    return ip_route_explicit_rif__start_run(unit, &param);

}

/*
 * Main function runs the excplicit rif example 
 *  
 * Main steps of configuration: 
 *    1. Initialize test parameters
 *    2. Setting VLAN domain for ingress/egress ports.
 *    3. Setting relevant TPID's for ingress/egress ports.
 *    4. Setting global MAC.
 *    5. Creating l3 routing interfaces for ingress/egress ports.
 *    6. Setting FEC and ARP entry for egress interface.
 *    7. Add routing  information.
 *    8. Setting vlan translation configuration for egress port.
 *    
 * INPUT: unit  - unit
 *        param - new values for explicit_rif(in case it's NULL default values will be used).
 */
int ip_route_explicit_rif__start_run(int unit,  ip_route_explicit_rif_s *param){
    int rv;
    int i;
    int fec = 0;
    int host;
    int encap_id = 0; 

    rv = ip_route_explicit_rif_init(unit, param);

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_default_mode_init\n");
        return rv;
    }


    /* set VLAN domain to each port */
    for (i=0; i< NUMBER_OF_PORTS; i++) {
        rv = port__vlan_domain__set(unit, g_ip_route_explicit_rif.rif[i].sys_port, g_ip_route_explicit_rif.rif[i].sys_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in port__vlan_domain__set, port=%d, \n", g_ip_route_explicit_rif.rif[i].sys_port);
            return rv;
        }
    }

    /* set TPIDs for both ports */
    for (i=0; i< NUMBER_OF_PORTS; i++) {
        port__default_tpid__set(unit, g_ip_route_explicit_rif.rif[i].sys_port);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }
    }

    /* set global MAC(36 MSB bits)*/
    rv = l2__global_mac__set(unit, g_ip_route_explicit_rif.global_mac);
    if (rv != BCM_E_NONE) {
         printf("Error, setting global my-MAC");
    }

    /*  create router interface for both ports */
    for (i=0; i< NUMBER_OF_PORTS; i++) {
        rv = ip_route_explicit_rif__l3_intf_create(unit, i); 
        if (rv != BCM_E_NONE) {
            printf("Error, creating interface: port=%d,  unit %d \n", g_ip_route_explicit_rif.rif[i].sys_port, unit);
        }
    }

    /* create egress object */
    /* Sets FEC and ARP entry for the egress L3 interface*/
    rv = l3__egress__create(unit ,0, g_ip_route_explicit_rif.rif[OUT_PORT].sys_port, 0, g_ip_route_explicit_rif.rif[OUT_PORT].rif, g_ip_route_explicit_rif.next_hop_mac, &fec, &encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_sysport=%d, in unit %d\n", out_sysport, unit);
    }
    

    /* add route point to FEC */
    rv = l3__ipv4_route__add(unit, g_ip_route_explicit_rif.route_entry, g_ip_route_explicit_rif.rif[IN_PORT].vrf, fec); 
    if (rv != BCM_E_NONE) {
        printf("Error, adding route, in_sysport=%d in unit %d, \n", l3_intrf[IN_PORT].sys_port, unit);
    }



    /* set port translation info*/
    rv = vlan__eve_default__set(unit, g_ip_route_explicit_rif.rif[OUT_PORT].gport, g_ip_route_explicit_rif.rif[OUT_PORT].vlan, 0, bcmVlanActionAdd, bcmVlanActionNone);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__eve_default__set in unit %d sys_port %d \n", unit, g_ip_route_explicit_rif.rif[OUT_PORT].sys_port);
    }
    return rv;
}


/*
 * create l3 interface - ingress/egress 
 *  
 * Main steps of configuration: 
 *  1. Set my mac (12 LSB bits).
 *  2. Create L2 logical port according <port,vlan>
 *  3. Create L3 Routing Interface
 *  
 *  INPUT: unit  -  unit
 *         rif index - index of RIF in explicit_rif(according to this index relevant values(port, vlan, rif, my_mac) will be taken.
 *  OUTPUT: gport - set L2 logical interface(gport field in explicit_rif)
 */
int ip_route_explicit_rif__l3_intf_create(int unit, int rif_index) {

    int rc;

    /* Set my_mac (12 LSB bits)*/
    rc = l2__my_mac_ipv4_vsi__set(unit, g_ip_route_explicit_rif.rif[rif_index].my_lsb_mac, 
                                        g_ip_route_explicit_rif.rif[rif_index].rif);
    if (rc != BCM_E_NONE) {
          printf("Fail  l2__my_mac_ipv4_vsi__set: port(%d) vlan(%d)", g_ip_route_explicit_rif.rif[rif_index].sys_port, g_ip_route_explicit_rif.rif[rif_index].vlan);
          return rc;
    }

    /* Create l2 logical interface*/
    rc = l2__port_vlan__create(unit,0,
                                     g_ip_route_explicit_rif.rif[rif_index].sys_port, 
                                     g_ip_route_explicit_rif.rif[rif_index].vlan, 
                                     g_ip_route_explicit_rif.rif[rif_index].rif, 
                                     &g_ip_route_explicit_rif.rif[rif_index].gport);
    if (rc != BCM_E_NONE) {
          printf("Fail  vlan__port_vlan__create: port(%d) vlan(%d)", g_ip_route_explicit_rif.rif[rif_index].sys_port, g_ip_route_explicit_rif.rif[rif_index].vlan);
          return rc;
    }
    
    /* Create L3 Routing Interface*/
    rc = l3__intf_rif_with_id__create(unit,g_ip_route_explicit_rif.global_mac,
                                      g_ip_route_explicit_rif.rif[rif_index].my_lsb_mac,
                                      g_ip_route_explicit_rif.rif[rif_index].rif);
    if (rc != BCM_E_NONE) {
          printf("Fail  l3__intf_rif_with_id__create: port(%d) ", g_ip_route_explicit_rif.rif[rif_index].sys_port);
          return rc;
    }

    return rc;
}


