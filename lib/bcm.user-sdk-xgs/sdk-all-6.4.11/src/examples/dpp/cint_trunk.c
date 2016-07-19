/*
 * $Id: cint_trunk.c,v 1.7 Broadcom SDK $
 *
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
 * File: cint_trunk.c
 * Purpose: The cint creates a Trunk containing several ports
 *
 *   default trunk example
 *  ----------------------------
 *   run:
 *   cint utility/cint_utils_global.c
 *   cint cint_trunk.c
 *   c
 *   run :
 *   print main_trunk(unit, is_hash_mode, double_lbk_rng_enable);
 *
 *      is_hashing_mode - use hashing lb-key or round-robin mode.
 *      double_lbk_rng_enable - supported only in ARAD+.
 *                              if set needs to config soc property: system_ftmh_load_balancing_ext_mod = STANDBY_MC_LB. 
 *
 * Notes: trunk_create - routine shows example how to create lag 
 * 
 */


int trunk_create(int unit, bcm_trunk_t tid, int in_local_port, int out_local_port_num, int *out_local_ports, int is_hash_mode, int double_lbk_rng_enable)
{
    int rv= BCM_E_NONE;
    bcm_trunk_member_t member_array[20];
    bcm_trunk_info_t trunk_info;
    int i;

    sal_memset(member_array, 0, sizeof(member_array));
    sal_memset(&trunk_info, 0, sizeof(trunk_info));
    /*
    * Creates #tid trunk (no memory allocation and no members inside)
    */
    rv =  bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);
    if(rv != BCM_E_NONE) {
        printf("Error: bcm_trunk_create(), rv=%d \n", rv);
        return rv;
    }
    printf("trunk creat: tid=%d\n", tid);

    /*
    * Prepare local ports to gports
    */ 
    for (i=0; i<out_local_port_num; i++) {
       BCM_GPORT_SYSTEM_PORT_ID_SET(member_array[i].gport, out_local_ports[i]);  
    }

    /*
    * two options are valid BCM_TRUNK_PSC_PORTFLOW or BCM_TRUNK_PSC_ROUND_ROBIN
    */
    if(is_hash_mode) {

        trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW;  

        rv = bcm_switch_control_port_set(unit, in_local_port, bcmSwitchTrunkHashPktHeaderCount, 0x2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set(), rv=%d.\n", rv);
            return rv;
        }

    } else {
        trunk_info.psc= BCM_TRUNK_PSC_ROUND_ROBIN;  
    }
 
    /*
     * Adds members (in member_array) to trunk and activates trunk mechanism
     */
    rv = bcm_trunk_set(unit, tid, &trunk_info, out_local_port_num, member_array);
    if(rv != BCM_E_NONE) {
        printf("Error: bcm_trunk_set(), rv=%d\n", rv);
        return rv;
    }

    printf("trunk set\n");

    if(double_lbk_rng_enable) {
        
        if(!is_device_or_above(unit,ARAD_PLUS)) {
            printf("set double_lbk_rng_enable is supported only in ARAD+ / Jericho");
            return BCM_E_PARAM;
        }
        
        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkIngressCommit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkEgressCommit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }
                
    }
    
    

    return rv;

}

int trunk_destroy(int unit, bcm_trunk_t tid, int double_lbk_rng_enable)
{
    int rv;
    
     rv = bcm_trunk_destroy(unit, tid);
     if(rv != BCM_E_NONE) {
        printf("Error: bcm_trunk_destroy(), rv=%d\n", rv);
        return rv;
     }   
        
    if(double_lbk_rng_enable) {
		
        if(!is_device_or_above(unit,ARAD_PLUS)) {
            printf("set double_lbk_rng_enable is supported only in ARAD+ / Jericho");
            return BCM_E_PARAM;
        }
        
        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkIngressCommit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkEgressCommit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }    
    }

    return rv;
}

int l2_addr_destroy(int unit, int is_mc)
{
    bcm_mac_t 
    da  = {0x01, 0x02, 0x03, 0x04, 0x05, 0x00};

    if(is_mc) {
        da[4] = 7;
    }

    bcm_l2_addr_delete(unit, da, 1);

    return 0;
}

/*
 * Example how to add port to an existing trunk tid
 */
int existing_trunk_member_add(int unit, bcm_trunk_t tid, int out_local_port, int double_lbk_rng_enable)
{
    int rv;
    bcm_trunk_member_t member_array;

    BCM_GPORT_SYSTEM_PORT_ID_SET(member_array.gport, out_local_port);
    /*
     *  bcm_trunk_member_add adds member_array to an existing tid.
     *  - multiple instances of a same port is allowed (to change ballance load)
     *  - same port cannot be member of different tid's
     */
    rv =  bcm_trunk_member_add(unit,tid,&member_array);
    if(rv != BCM_E_NONE) {
        printf("Error: bcm_trunk_member_add(), rv=%d\n", rv);
        return rv; 
    }
    
    if(double_lbk_rng_enable) {

        if(!is_device_or_above(unit,ARAD_PLUS)) {
            printf("set double_lbk_rng_enable is supported only in ARAD+ / Jericho");
            return BCM_E_PARAM;
        }
    
        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkIngressCommit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkEgressCommit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }    
    }
    
    return rv;
}


/*
 * Example how to delete port to an existing trunk tid
 */
int existing_trunk_member_delete(int unit, bcm_trunk_t tid, int out_local_port, int double_lbk_rng_enable)
{
    int rv;
    bcm_trunk_member_t member_array;
    
    BCM_GPORT_SYSTEM_PORT_ID_SET(member_array.gport, out_local_port);
    /*
     *  bcm_trunk_member_add adds member_array to an existing tid.
     *  - if multiple instances of a same port is in trunk, just one instance of it will be deleted
     */
    rv = bcm_trunk_member_delete(unit,tid,&member_array);
    if(rv != BCM_E_NONE) {
        printf("Error: bcm_trunk_member_delete(), rv=%d\n", rv);
        return rv; 
    }
        
    if(double_lbk_rng_enable) {
        
        if(!is_device_or_above(unit,ARAD_PLUS)) {
            printf("set double_lbk_rng_enable is supported only in ARAD+ / Jericho");
            return BCM_E_PARAM;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkIngressCommit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkEgressCommit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }    
    }
    
    return rv;
}


int l2_addr_config(int unit, bcm_trunk_t tid, int is_mc, int is_egr)
{
    int 
        rv = BCM_E_NONE,
        vlan = 0x1,
        mc_id,
        flags;
    bcm_gport_t
        gport;
    bcm_mac_t 
        da  = {0x01, 0x02, 0x03, 0x04, 0x05, 0x00};
    bcm_l2_addr_t l2addr;

    BCM_GPORT_TRUNK_SET(gport, tid);

    if(is_mc) {

        if(is_egr){
            mc_id = 0x7000;
        } else {
            mc_id = 0x7;
        }

        bcm_gport_t mc_gport_member;        
        
        da[4] = mc_id % 256;
        /*  open ingress multycast group */
        flags = BCM_MULTICAST_WITH_ID;
        
        if(is_egr){
            flags |= BCM_MULTICAST_EGRESS_GROUP;
        } else {
            flags |= BCM_MULTICAST_INGRESS_GROUP;
        } 

        rv = bcm_multicast_create(unit, flags, &mc_id);
        if (rv != BCM_E_NONE) {
             printf("Error, in _multicast_create, mc_id mc_id=%d\n", mc_id);
             return rv;
        }

        /* Add MC members */

        BCM_GPORT_TRUNK_SET(mc_gport_member, tid);        
        if(is_egr) {
            rv = bcm_multicast_egress_add(unit, mc_id, mc_gport_member, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_multicast_egress_add, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, mc_gport_member);
                return rv;
            }  
        } else {
            if (is_device_or_above(unit, ARAD_PLUS)) {
                rv = bcm_multicast_ingress_add(unit, mc_id, mc_gport_member, 0);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_multicast_ingress_add, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, mc_gport_member);
                    return rv;
                }
            } else {
                /*rv = bcm_multicast_ingress_add(unit, mc_id, mc_gport_member, 0);*/
                bcm_gport_t trunk_port_members[4];
                BCM_GPORT_LOCAL_SET(trunk_port_members[1], 14);
                BCM_GPORT_LOCAL_SET(trunk_port_members[2], 15);
                BCM_GPORT_LOCAL_SET(trunk_port_members[3], 16);
                BCM_GPORT_LOCAL_SET(trunk_port_members[0], 17);

                rv += bcm_multicast_ingress_add(unit, mc_id, trunk_port_members[0], 0);
                rv += bcm_multicast_ingress_add(unit, mc_id, trunk_port_members[1], 0);
                rv += bcm_multicast_ingress_add(unit, mc_id, trunk_port_members[2], 0);
                rv += bcm_multicast_ingress_add(unit, mc_id, trunk_port_members[3], 0);

                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_multicast_ingress_add, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, mc_gport_member);
                    return rv;
                }
            }
        }
    }

    /* Initiate l2 mac  */
    bcm_l2_addr_t_init(&l2addr, da, vlan);

    if(rv != BCM_E_NONE) {
        printf("Error: bcm_l2_addr_t_init() rv=%d \n", rv);
    }
    printf("l2 init, da = {0x1 0x2 0x3 0x4 0x5 0x%x}\n", da[5]);

    l2addr.flags = BCM_L2_STATIC; 
    
    if(is_mc) {
        l2addr.flags =  l2addr.flags | BCM_L2_MCAST;
        l2addr.l2mc_group = mc_id;
    } else {  
        l2addr.port = gport;
        l2addr.vid = vlan;
        l2addr.tgid = tid;  
    }
    
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add(), rv=%d.\n", rv);
        return rv;
    }
    printf("Created lb2: tid=%d is_mc=%d\n", tid, is_mc);

    return rv;

}

int main_trunk(int unit, int is_hash_mode, int double_lbk_rng_enable)
{
    int out_local_port_example[20] = {14,15,16,17};
    int out_local_port_num_example = 4;
    int in_local_port_example = 13;
    bcm_trunk_t tid_example = 2;
    int rv = BCM_E_NONE;
    printf("Trunk #%d create with %d ports\n", tid_example, out_local_port_num_example);

    rv = trunk_create(unit, tid_example, in_local_port_example, out_local_port_num_example, out_local_port_example, is_hash_mode, double_lbk_rng_enable);
    if (rv != BCM_E_NONE) {
        printf("Error, trunk_create(), rv=%d.\n", rv);
        return rv;
    }

/* 
 * configure lb2 for uc
 */
    rv = l2_addr_config(unit, tid_example, 0x0, 0); 
    if(rv != BCM_E_NONE){
         printf("Error: lb2_config() uc\n");
    }

/* 
 * configure lb2 for mc group (whene lag is one of the group)
 */
    rv = l2_addr_config(unit, tid_example, 0x1, 0); 
    if(rv != BCM_E_NONE){
         printf("Error: lb2_config() mc\n");
    }

/*
 * Example to add port to an existing tid
 */
    /*printf("Trunk #%d add member local port 14\n", tid_example);
    existing_trunk_member_add(unit, tid_example, 17); */


/*
 * Example to delete port to an existing tid
 */
    /*printf("Trunk #%d delete member local port 14\n", tid_example);
    existing_trunk_member_delete(unit, tid_example, 17);*/ 

/*
 * Example to delete all ports from an existing tid, but remain tid
 */
    /* existing_trunk_member_delete_all(unit, tid_example); */


/*
 * Example to delete all ports from an existing tid and remove tid
 */
    /* trunk_destroy(unit, tid_example); */

    return 0;

}

/*
 * This function is used to create a trunk with three ports (out_port_0, out_port_1, out_port_2) 
 * It is used for testing the trunk functionality without passing an array as parameter. 
 * This is useful in Cints' Dvapis scripts 
 */
int trunk_create_example_1 (int unit, int tid_num, int in_port, int out_port_0, int out_port_1, int out_port_2)
{
    bcm_trunk_t tid = tid_num;
    int ports[] = {out_port_0, out_port_1, out_port_2};
    trunk_create(unit, tid, in_port, 3, &ports, 0, 0);
}


