/*~~~~~~~~~~~~~~~~~~~~~~~~~~Traditional Bridge: Port TPID~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_port_tpid.c,v 1.17 Broadcom SDK $
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
 * File: cint_port_tpid.c 
 * Purpose: Example of how to set the tag protocol identifiers (TPID) to identify in a given port 
 *  
 * Default settings:
 * It is assumed diag_init is executed:
 *  o    All ports set with TPID 0x8100 and all packet formats (tagged/untagged) are accepted.
 *  o    PVID of all ports set to VID = 1, thus untagged packet is associated to VLAN 1.
 *  o    By default, learning is managed by device without interference from the CPU.
 *  o    By default, aging is disabled.
 *  o    All VLANs are mapped to Default STG (0).
 *  o    STP state of all ports in default topology is set to forwarding.
 * 
 * Note: Without running diag_init, ports are not associated with any TPID,
 *       and thus all packets would be considered as untagged.
 *       Besides, only untagged packets are accepted.
 *  
 * Examples: 
 *  o    port_tpid_init(2,1,1): defines two TPIDs on port 2, 0x8100 as outer and 0x9100 as inner TPID.
 *  o    port_tpid_set(0): Commits the above setting on unit 0. 
 *  
 * Note: You can also use this cint on a remote device by calling the functions with the remote unit id.
 *  
 */

struct port_tpid_info_s {
    int port;
    int outer_tpids[2];
    int nof_outers; /*0, 1, 2*/
    int inner_tpids[2];
    int nof_inners; /*0, 1 */
    int discard_frames; /* BCM_PORT_DISCARD_NONE/ALL/UNTAG/TAG */
    uint32 tpid_class_flags; /* flags for bcm_port_tpid_class_set */
    int vlan_transation_profile_id; /* Default Ingress action command in case of no match in VTT L2 LIF lookup */
};

port_tpid_info_s port_tpid_info1;

/* Set to 1 so cints that only call port_tpid_set in advanced vlan translation mode would check, and turn it
   off if neccesary */
uint8 advanced_vlan_translation_mode = 1;


/* 
 *  nof_outers = 1,2
 *  nof_inners = 0,1
 *  nof_outers >= nof_inners
 *  nof_outers + nof_inners = 2
 */ 
void
port_tpid_init(int port, int nof_outers, int nof_inners){
    port_tpid_info1.port = port;
    port_tpid_info1.outer_tpids[0] = 0x8100;
    port_tpid_info1.outer_tpids[1] = 0x88a0;
    port_tpid_info1.inner_tpids[0] = 0x9100;
    port_tpid_info1.nof_outers = nof_outers;
    port_tpid_info1.nof_inners = nof_inners;
    port_tpid_info1.discard_frames = BCM_PORT_DISCARD_NONE;
    port_tpid_info1.tpid_class_flags = 0;
    port_tpid_info1.vlan_transation_profile_id = 0;

}

/* 
 *  nof_outers = 1,2
 *  nof_inners = 0,1
 *  nof_outers >= nof_inners
 *  nof_outers + nof_inners = 2
 */ 
void
port_tpid_init_cep_port_example(int port, int nof_outers, int nof_inners){
    port_tpid_info1.port = port;
    port_tpid_info1.outer_tpids[0] = 0x9100;
    port_tpid_info1.outer_tpids[1] = 0x88a0;
    port_tpid_info1.inner_tpids[0] = 0x8100;
    port_tpid_info1.nof_outers = nof_outers;
    port_tpid_info1.nof_inners = nof_inners;
    port_tpid_info1.discard_frames = BCM_PORT_DISCARD_NONE;
    port_tpid_info1.tpid_class_flags = 0;
    port_tpid_info1.vlan_transation_profile_id = 0;
}

int
init_vlan(int unit, int vlan)
{
    bcm_error_t rv;
    bcm_port_config_t c;
    bcm_pbmp_t p,u;
    rv = bcm_port_config_get(unit, &c);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_config_get\n");
        return rv;
    }
    BCM_PBMP_ASSIGN(p, c.e);
    
    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create, vlan=%d, \n", vlan);
        return rv;
    }
    
    rv = bcm_vlan_port_add(unit, vlan, p, u);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", vlan);
        return rv;
    }
    
    return rv;
}

int
port_tpid_set(int unit){
    return _port_tpid_set(unit, 0);
}

int
port_lif_tpid_set(int unit){
    return _port_tpid_set(unit, 1);
}

int
_port_tpid_set(int unit, int is_lif){
    int rv;
    int indx;
    int color_set=0;/* has to be zero */

    advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    if (advanced_vlan_translation_mode) {
        if (port_tpid_info1.nof_outers == 1 && port_tpid_info1.nof_inners == 1) {
            return port_dt_tpid_set(unit, port_tpid_info1.port, port_tpid_info1.outer_tpids[0], port_tpid_info1.inner_tpids[0], is_lif, port_tpid_info1.vlan_transation_profile_id);
        } else if (port_tpid_info1.nof_outers == 1 && port_tpid_info1.nof_inners == 0) {
            return port_outer_tpid_set(unit, port_tpid_info1.port, port_tpid_info1.outer_tpids[0], is_lif, port_tpid_info1.vlan_transation_profile_id);
        }
    }
    
    /* assume port_tpid_init was called with legal params*/
    
    /* remove old tpids on port */
    rv = bcm_port_tpid_delete_all(unit,port_tpid_info1.port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_delete_all, port=%d, \n", port_tpid_info1.port);
        return rv;
    }
    
    /* set outer tpids of the port */
    for(indx = 0; indx < port_tpid_info1.nof_outers; ++indx){
        rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.outer_tpids[indx], color_set);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add, tpid=%d, \n", port_tpid_info1.outer_tpids[indx]);
            return rv;
        }
    }
    
    /* set inner tpids of the port */
    for(indx = 0; indx < port_tpid_info1.nof_inners; ++indx){
        rv = bcm_port_inner_tpid_set(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[indx]);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[indx]);
            return rv;
        }
    }
    
    /* Only relevant for physical port */    
    if(!is_lif) {    
        /* set discard frames on port */
        rv = bcm_port_discard_set(unit, port_tpid_info1.port, port_tpid_info1.discard_frames);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_discard_set, discard-type=%d, \n", port_tpid_info1.discard_frames);
            return rv;
        }
    }
    
    return rv;
}

int
add_mac_and_vlan(int unit, int mac_lsb, int vlan,int port){
    
    bcm_l2_addr_t l2_addr;
    bcm_mac_t mac_a;
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t dest_gport;
    bcm_pbmp_t pbmp;
    
    int i;
    
    uint8 incoming_mac[6] = {0,0,0,0,0,0}; 
    incoming_mac[5] = mac_lsb;
    
    bcm_l2_addr_t_init(&l2_addr, incoming_mac, vlan);
    l2_addr.flags |= BCM_L2_STATIC;
    BCM_GPORT_LOCAL_SET(dest_gport, port);
    l2_addr.port = dest_gport;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }
    
    bcm_l2_addr_dump(&l2_addr);
    rv = bcm_vlan_create(unit,vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create\n");
        return rv;
    }
    
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_add(unit, vlan, pbmp, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }
    
    return rv;
}

int port_tpid_example(int unit, int mac_lsb, int vlan, int port){
    port_tpid_init(1,1,0);
    port_tpid_set(unit);
    add_mac_and_vlan (unit, mac_lsb, vlan, port);
    /* cint_reset(); */
    return 0;
}

/*  These two functions present a second example of using port_tpid to control packet routing.
 *  In stage1:
 *  We delete all port tpids for both in_port and out_port, causing any packet
 *  tagged with any tpid other than 0x8100 to be untagged during ingress.
 *  
 *  We then add an entry to the mac_table:
 *  Vlan id:            22
 *  Destination MAC:    55
 *  
 *  We then(in dvapi) send a packet
 *  tagged with 0x9100, and the above settings.
 *  Result: The packet will be untagged and will be flooded to all ports.
 *  
 *  In stage2:
 *  We add the tpid 0x9100 to both in_port and out_port.
 *  We then(in dvapi) send a second packet
 *  tagged with 0x9100, and the above settings.
 *  Result: The packet will remain tagged and arrive at out_port.
 *  See test: AT_Cint_port_tpid for additional details.
 */

int run_with_dvapi_port_tpid_stage_1(int unit, int in_port, int out_port){
    int rv = 0;
    /*mac table entry settings*/
    int vlan = 22;
    int mac_lsb = 55;
    /*initializing port_tpid*/
    port_tpid_init(in_port,1,0);
    port_tpid_init(out_port,1,0);
    rv = bcm_port_tpid_delete_all(unit,in_port);

    /*Deleting port tpids for in_port*/
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_delete_all, port=%d, \n", in_port);
        return rv;
    }
    /*Deleting port tpids for out_port*/
    rv = bcm_port_tpid_delete_all(unit,out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_delete_all, port=%d, \n", out_port);
        return rv;
    }
    /*Adding entra to mac table for vlan 22 and dmac 55*/
    add_mac_and_vlan (unit, mac_lsb, vlan, out_port);
    return 0;
}

int run_with_dvapi_port_tpid_stage_2(int unit, int in_port, int out_port){
    int rv = 0;
    int color_set=0;/* has to be zero */

    advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    if (advanced_vlan_translation_mode) {
        rv = port_outer_tpid_set(unit, in_port, port_tpid_info1.inner_tpids[0],0,port_tpid_info1.vlan_transation_profile_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in port_outer_tpid_set, port=%d, tpid=%d, \n", in_port, port_tpid_info1.inner_tpids[0]);
            return rv;
        }
        rv = port_outer_tpid_set(unit, out_port, port_tpid_info1.inner_tpids[0],0,port_tpid_info1.vlan_transation_profile_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in port_outer_tpid_set, port=%d, tpid=%d, \n", out_port, port_tpid_info1.inner_tpids[0]);
            return rv;
        }
    } else {
        /*Adding tpid 0x9100 back to in_port*/
        rv = bcm_port_tpid_add(unit, in_port, port_tpid_info1.inner_tpids[0], color_set);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add, tpid=%d, \n", port_tpid_info1.inner_tpids[0]);
            return rv;
        }
        /*Adding tpid 0x9100 back to out_port*/
        rv = bcm_port_tpid_add(unit, out_port, port_tpid_info1.inner_tpids[0], color_set);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add, tpid=%d, \n", port_tpid_info1.inner_tpids[0]);
            return rv;
        }
    }

    return BCM_E_NONE;
}


/* advance mode vlan translation mode
  According to Soc property: bcm886xx_vlan_translate_mode
  Two separate configurations: 
1.	TPID  values recognized on port using bcm_port_tpid_add
2.	For port and TPID combination: set tag-format bcm_port_tpid_class_set
*/

/* set port with one tpid as outer */
int port_outer_tpid_set(int unit, int port, int tpid1, int is_lif, int vlan_transation_profile_id){
    int rv;
    bcm_port_tpid_class_t port_tpid_class;

    /* remove old tpids on port */
    rv = bcm_port_tpid_delete_all(unit,port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_delete_all, port=%d, \n", port);
        return rv;
    }

    /* identify TPID on port */
    rv = bcm_port_tpid_add(unit, port, tpid1,0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_add, port=%d, \n", port);
        return rv;
    }

    /* when TPID found as outer, then consider as tag and accepted format */

    if(!is_lif) {
        bcm_port_tpid_class_t_init(&port_tpid_class);
        sal_memset(&port_tpid_class, 0, sizeof (port_tpid_class));

        port_tpid_class.port = port;
        port_tpid_class.tpid1 = tpid1;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        port_tpid_class.tag_format_class_id = 2;  /* can be any number !=0, 2 is backward for s-tag */
        port_tpid_class.flags = 0;
        port_tpid_class.vlan_translation_action_id = vlan_transation_profile_id;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
            return rv;
        }
    }
    return rv;
}



/* set port with one tpid as outer */
int port_dt_tpid_set(int unit, int port, int tpid1, int tpid2, int is_lif, int vlan_transation_profile_id){
    int rv;
    bcm_port_tpid_class_t port_tpid_class;
    uint32 advanced_vlan_editing;

    advanced_vlan_editing = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    /* remove old tpids on port */
    rv = bcm_port_tpid_delete_all(unit,port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_delete_all, port=%d, \n", port);
        return rv;
    }

    /* identify TPID on port */
    if (!advanced_vlan_editing) {
        rv = bcm_port_tpid_add(unit, port, tpid1,0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add1, port=%d, \n", port);
            return rv;
        }
    

        /* identify TPID on port */
        rv = bcm_port_tpid_add(unit, port, tpid2,0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add2, port=%d, \n", port);
            return rv;
        }
    } else { 
        rv = bcm_port_tpid_set(unit, port, tpid1); /* AVT mode does not support bcm_port_tpid_add(); instead we do set */
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add1, port=%d, \n", port);
            return rv;
        }
    

        /* identify TPID on port */
        rv = bcm_port_inner_tpid_set(unit, port, tpid2);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add2, port=%d, \n", port);
            return rv;
        }

    }

    if(!is_lif){
        /* outer: TPID1, inner: ANY ==> S_tag */
        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = port;
        port_tpid_class.tpid1 = tpid1;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        port_tpid_class.tag_format_class_id = 2;  /* can be any number !=0, 2 is backward for s-tag */
        port_tpid_class.flags = port_tpid_info1.tpid_class_flags;
        port_tpid_class.vlan_translation_action_id = vlan_transation_profile_id;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
            return rv;
        }
       /* outer: TPID1, inner: TPID2 ==> S_c_tag */
        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = port;
        port_tpid_class.tpid1 = tpid1;
        port_tpid_class.tpid2 = tpid2;
        port_tpid_class.tag_format_class_id = 6;  /* can be any number !=0, 2 is backward for s-c-tag */
        port_tpid_class.flags = port_tpid_info1.tpid_class_flags;
        port_tpid_class.vlan_translation_action_id = vlan_transation_profile_id;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
            return rv;
        }
        /* outer: TPID2, inner: ANY ==> C_tag */
        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = port;
        port_tpid_class.tpid1 = tpid2;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        port_tpid_class.tag_format_class_id = 3;  /* can be any number !=0, 3 is backward for c-tag */
        port_tpid_class.flags = port_tpid_info1.tpid_class_flags;
        port_tpid_class.vlan_translation_action_id = 0;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
            return rv;
        }

    }

    return rv;
}


