/*
 * $Id: cint_field_action_config.c v 1.0 31/03/2016 skoparan Exp $
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
 * File: cint_field_action_config.c
 * Purpose: Example Usage of egress PMF actions using ACE table
 * 
 * Fuctions:
 * Main function:
 *      stat_config_main()
 * Configuration functions:
 *  L2 config:
 *      create_vlan_and_ports() - to be used for all cases
 *      create_lif() - to be used for out-LIF redirection in addition to create_vlan_and_ports()
 *  PMF config:
 *      field_config()
 * 
 * Result verification functions:
 *      run_verify_redir() - for cases with port redirection
 *      run_verify_newlif() - for cases with out-LIF redirection
 * 
 * Cleanup function:
 *      field_destroy()
 * 
 */

int statId; /* Statistics ID for counting */
bcm_gport_t outlif; /* with out-LIF redirection, the new LIF to redirect to */

/* function to check if the device is Jericho and above */
int device_is_jer(int unit,
                  uint8* is_jer) {
    bcm_info_t info;
    char *proc_name;

    proc_name = "device_is_jer";
    printf("%s(): Checking if device is Jericho...\n", proc_name);
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_info_get() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" info.device = 0x%x...\n", info.device);
    *is_jer = ((info.device == 0x8375) || (info.device == 0x8675) || (info.device == 0x8470) || (info.device == 0x8680));
    if (*is_jer) {
        printf("%s(): Done. Device is Jericho or above.\n", proc_name);
    } else {
        printf("%s(): Done. Device is below Jericho.\n", proc_name);
    }
    return rv;
}
/* function to check if the device is Qumran AX */
int device_is_qax(int unit,
                  uint8* is_qax) {
    bcm_info_t info;
    char *proc_name;

    proc_name = "device_is_qax";
    printf("%s(): Checking if device is QAX...\n", proc_name);
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_info_get() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" info.device = 0x%x...\n", info.device);
    *is_qax = (info.device == 0x8470);
    if (*is_qax) {
        printf("%s(): Done. Device is QAX.\n", proc_name);
    } else {
        printf("%s(): Done. Device is not QAX.\n", proc_name);
    }
    return rv;
}


/*Create VLAN and add in_port and out_port to this VLAN.*/
int create_vlan_and_ports(int unit,
                          int vsi,
                          int in_port,
                          int out_port,
                          int redir_port,
                          int is_redir) {
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    bcm_mac_t sa_mac = {0x01,0x02,0x03,0x04,0x05,0x06};
    bcm_mac_t da_mac = {0x06,0x05,0x04,0x03,0x02,0x01};
    char *proc_name;

    proc_name = "create_vlan_and_ports";
    /* Create VSI */
    printf("%s(): Creating VSI on VLAN %d...", proc_name, vsi);
    rv = bcm_vlan_create(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_vlan_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    /* Configure in-port */
    printf("%s(): Adding inport %d to VLAN %d...", proc_name, in_port, vsi);
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN; 
    vlan_port.match_vlan = vsi;
    vlan_port.vsi = vsi;
    vlan_port.port = in_port;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_vlan_port_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");
    /* Configure a MAC address for the in-port, to not learn SA MAC. */
    printf("%s(): Adding MACT entry for in-port...", proc_name);
    l2__mact_properties_s mact_entry0 = {vlan_port.vlan_port_id,
                                        sa_mac,
                                        vlan_port.vsi};
    rv = l2__mact_entry_create(unit, &mact_entry0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: l2__mact_entry_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    /* Configure out-port */
    printf("%s(): Adding outport %d to VLAN %d...", proc_name, out_port, vsi);
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN; 
    vlan_port.match_vlan = vsi;
    vlan_port.vsi = vsi;
    vlan_port.port = out_port;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_vlan_port_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");
    /* Configure a MAC address for the out-port, to forward. */
    printf("%s(): Adding MACT entry for out-port...", proc_name);
    l2__mact_properties_s mact_entry1 = {vlan_port.vlan_port_id,
                                        da_mac,
                                        vlan_port.vsi};
    rv = l2__mact_entry_create(unit, &mact_entry1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in l2__mact_entry_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    if (is_redir) {
        /* Configure redirect port */
        printf("%s(): Adding redirect port %d to VLAN %d...", proc_name, redir_port, vsi);
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN; 
        vlan_port.match_vlan = vsi;
        vlan_port.vsi = vsi;
        vlan_port.port = redir_port;
        rv = bcm_vlan_port_create(unit, vlan_port);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, bcm_vlan_port_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf(" Done.\n");
    }

    printf("%s(): Done. Created VLAN %d with ports.\n", proc_name, vsi);
    return rv;
}

/*Create VLAN-port object (for out-LIF redirection)*/
int create_lif(int unit,
               int vsi,
               int redir_port,
               int *newlif) {
    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_gport_t redir_gport;
    bcm_gport_t local_outlif;
    char *proc_name;

    proc_name = "create_lif";
    /* Set gport */
    printf("%s(): Set gport for redirect port %d...", proc_name, redir_port);
    BCM_GPORT_SYSTEM_PORT_ID_SET(redir_gport, redir_port);
    printf(" Done. Gport = 0x%x\n", redir_gport);

    /* Create VLAN port */
    printf("%s(): Adding outport %d (gport=0x%x) to VLAN %d...", proc_name, redir_port, redir_gport, vsi);
    bcm_vlan_port_t_init (&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.vsi = vsi;
    vlan_port.port = redir_gport;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_vlan_port_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done. VLAN port ID = 0x%x\n", vlan_port.vlan_port_id);

    /* Get LIF value */
    printf("%s(): Getting LIF value from gport 0x%x...", proc_name, vlan_port.vlan_port_id);
    local_outlif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port.vlan_port_id);
    *newlif = local_outlif;
    printf(" Done. LIF = 0x%x\n", local_outlif);

    printf("%s(): Done. Created outlif %d.\n", proc_name, *newlif);
    return rv;
}

/* Function to configure Field Processor with ActionStat
 *  
 * Can be used in 4 combinations (cases): 
 *  1. To count only, pass arguments:
 *      - redir_port = any
 *      - is_count = 1
 *      - is_redir = 0
 *      - is_newlif = 0
 *  2. To count and redirect, pass arguments:
 *      - redir_port = redirection local port
 *      - is_count = 1
 *      - is_redir = 1
 *      - is_newlif = 0
 *  3. To count, redirect and change out-LIF, pass arguments:
 *      - redir_port = redirection local port
 *      - is_count = 1
 *      - is_redir = 1
 *      - is_newlif = 1
 *  4. To redirect only
 *      - redir_port = redirection local port
 *      - is_count = 0
 *      - is_redir = 1
 *      - is_newlif = 0
 *  
 */
int field_config(int unit, 
                 int core, 
                 int counter_id_0, 
                 int counter_proc, 
                 bcm_field_group_t group, 
                 int group_priority, 
                 bcm_port_t out_port,
                 bcm_port_t redir_port, /* redirection local port, pass any if not apply */
                 bcm_gport_t gport, /* out-lif gport id / modport, pass any if not apply */
                 int is_count,
                 int is_redir, 
                 int is_newlif) {
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    int action;
    bcm_gport_t out_gport, redir_gport;
    int modId = 0;
    int cores_num = 2;
    bcm_field_action_core_config_t core_config_arr[cores_num];
    int tcase = 0;
    int i = 0;
    int my_param0, my_param1, my_param2;
    unsigned long my_param0_as_long, my_param1_as_long, my_param2_as_long;
    uint8 is_jer, is_qax;
    char *proc_name;
    
    proc_name = "field_config";
    printf("%s(): Configure field processor.\n", proc_name);

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : device_is_jer() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    if(is_jer) {
        /* devices Jericho and above are using bcm_action_config_add API,
         * specifying number of cores: 2 for Jericho and 1 for QAX */
        rv = device_is_qax(unit, &is_qax);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : device_is_qax() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        if(is_qax) {
            cores_num = 1;
        } else {
            cores_num = 2;
        }
    }

    if (is_count) {
        action = bcmFieldActionStat;
        /* Set the Stat ID */
        BCM_FIELD_STAT_ID_SET(&statId, counter_proc, counter_id_0);
        printf("%s(): Counter processor = %d, Counter ID = %d, Stat ID = %d\n", proc_name, counter_proc, counter_id_0, statId);
        if(!is_redir) {
             if (!is_newlif) { /* case 1: count only */
                 tcase = 1;
             }
        } else {
            if(!is_newlif) { /* case 2: count+redirect */
                tcase = 2;
            } else { /* case 3: count+redirect+newlif */
                tcase = 3;
            }
        }
    } else { 
        if(is_redir) {
            if(!is_newlif) {/* case 4: redirect only */
                action = bcmFieldActionRedirect;
                tcase = 4;
            }
        }
    }

    /*Build the qset*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);

    /*Build the aset*/
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, action);

    /*Create group with given qset, id and priority*/
    rv = bcm_field_group_create_id(unit, qset, group_priority, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_create_id returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Group %d created.\n", proc_name, group);

    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_action_set returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): ASET attached.\n", proc_name);
    /*Create empty entry*/
    rv = bcm_field_entry_create(unit, group, &ent);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_entry_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Entry %d created.\n", proc_name, ent);
    /*Set the qualifier value for entry*/
    BCM_GPORT_LOCAL_SET(out_gport, out_port);
    rv = bcm_field_qualify_DstPort(unit, ent, 0, 0, out_gport, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_qualify_DstPort returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Added qualifier: bcm_field_qualify_DstPort.\n", proc_name);

    /*Set the action value for entry*/
    printf("%s(): Adding action...\n", proc_name);
    if(is_jer) {
        switch (tcase) {
        case 1:
            printf("%s(): Case '1', count only", proc_name);
            my_param0 = statId;
            my_param1 = BCM_ILLEGAL_ACTION_PARAMETER;
            my_param2 = BCM_ILLEGAL_ACTION_PARAMETER;
            break;
        case 2:
            printf("%s(): Case '2', count + redirect", proc_name);
            BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
            my_param0 = statId;
            my_param1 = redir_gport;
            my_param2 = BCM_ILLEGAL_ACTION_PARAMETER;
            break;
        case 3:
            printf("%s(): Case '3', count +redirect + change LIF", proc_name);
            BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
            my_param0 = statId;
            my_param1 = redir_gport;
            my_param2 = gport;
            break;
        case 4:
            printf("%s(): Case '4', redirect only", proc_name);
            BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
            my_param0 = modId;
            my_param1 = redir_gport;
            my_param2 = BCM_ILLEGAL_ACTION_PARAMETER;
            break;
        default:
            printf("%s(): this action combination is not supported\n", proc_name);
            return BCM_E_UNAVAIL;
        }
        my_param0_as_long = my_param0;
        my_param1_as_long = my_param1;
        my_param2_as_long = my_param2;
        printf("  param0 = 0x%08lX\n  param1 = 0x%08lX\n  param2 = 0x%08lX\n\r\n",
               my_param0_as_long, my_param1_as_long, my_param2);

        for(i = 0; i < cores_num; i++) {
            core_config_arr[i].param0 = my_param0;
            core_config_arr[i].param1 = my_param1;
            core_config_arr[i].param2 = my_param2;
        }

        rv = bcm_field_action_config_add(unit, ent, action, cores_num, &core_config_arr[0]);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_config_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): bcm_field_action_config_add() done.\n", proc_name);
    } else { /* for ARAD */
        switch (tcase) {
            case 2:
                printf("%s(): Case '2', count + redirect", proc_name);
                BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
                my_param0 = statId;
                my_param1 = redir_gport;
                break;
            case 4:
                printf("%s(): Case '4', redirect only", proc_name);
                BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
                modId = BCM_GPORT_MODPORT_MODID_GET(gport);
                my_param0 = modId;
                my_param1 = redir_gport;
                break;
            default:
                printf("%s(): this action combination is not supported\n", proc_name);
                return BCM_E_UNAVAIL;
        }
        my_param0_as_long = my_param0;
        my_param1_as_long = my_param1;
        printf("  param0 = 0x%08lX\n  param1 = 0x%08lX\n\r\n",my_param0_as_long, my_param1_as_long);

        rv = bcm_field_action_add(unit, ent, action, my_param0, my_param1);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_add() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv; 
        }
        printf("%s(): bcm_field_action_add() done.\n", proc_name);
    }

    /*Commit the entire group to hardware.*/
    printf("%s(): Commit group %d to hardware...", proc_name, group);
    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_install() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    printf("%s(): Done. Field processor configured.\n", proc_name);
    return rv;
}


/* FIELD CONFIG FOR ForwardingTypeNew */
int field_config_ftn(int unit,
                 int core,
                 bcm_field_group_t group,
                 int group_priority,
                 bcm_port_t in_port,
                 bcm_field_ForwardingType_t new_fwd_type,
                 int offset,
                 int fix) {
    int rv = BCM_E_NONE;
    uint8 is_jer;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_gport_t in_gport;
    int action = bcmFieldActionForwardingTypeNew;
    int cores_num = 1; /* Needs to be configured for exactly one core, all others take the same */
    int i;
    bcm_field_action_core_config_t core_config_arr[cores_num];
    char *proc_name;

    proc_name = "field_config_ftn";
    printf("%s(): Configure field processor to change Forwarding Type.\n", proc_name);

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : device_is_jer() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }

    /*Build the qset*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);

    /*Build the aset*/
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, action);

    /*Create group with given qset, id and priority*/
    rv = bcm_field_group_create_id(unit, qset, group_priority, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_create_id returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Group %d created.\n", proc_name, group);

    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_action_set returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): ASET attached.\n", proc_name);
    /*Create empty entry*/
    rv = bcm_field_entry_create(unit, group, &ent);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_entry_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Entry %d created.\n", proc_name, ent);
    /*Set the qualifier value for entry*/
    BCM_GPORT_LOCAL_SET(in_gport, in_port);
    rv = bcm_field_qualify_SrcPort(unit, ent, 0, 0, in_gport, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_qualify_SrcPort returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Added qualifier: bcm_field_qualify_SrcPort.\n", proc_name);

    /*Set the action value for entry*/
    printf("%s(): Adding action...\n", proc_name);
    if(is_jer) { /* Use config API for Jericho and above*/
        for(i = 0; i < cores_num; i++) {
            core_config_arr[i].param0 = new_fwd_type;
            core_config_arr[i].param1 = offset;
            core_config_arr[i].param2 = fix;
        }

        rv = bcm_field_action_config_add(unit, ent, action, cores_num, &core_config_arr[0]);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_config_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): bcm_field_action_config_add() done.\n", proc_name);
    } else { /* Use old API for Arad */
        rv = bcm_field_action_add(unit, ent, action, new_fwd_type, offset);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): bcm_field_action_add() done.\n", proc_name);
    }

    /*Commit the entire group to hardware.*/
    printf("%s(): Commit group %d to hardware...", proc_name, group);
    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_install() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    printf("%s(): Done. Field processor configured.\n", proc_name);
    return rv;
}

/* Function to verify packet was redirected */
int run_verify_redir(int unit, int core, bcm_port_t port) {
    int rv = BCM_E_NONE;
    match_t match;
    signal_output_t signal_output;
    bcm_port_t local_port ;
    char *proc_name;

    proc_name = "run_verify_redir";
    match.block = "ETPP";
    printf("%s(): Enter to match expected port %d with actual out-port on %s\r\n", proc_name, port, match.block);
    match.from = "PRP";
    match.to = "Term";
    match.name = "Out_PP_Port";
    printf("%s(): Take the output, as given in BCM diagnostic command: \n diag pp sig block=%s from=%s to=%s name=%s\n",
           proc_name, match.block, match.from, match.to, match.name);
    rv = dpp_dsig_get(unit, core, &match, &signal_output);
    local_port = signal_output.value[0];
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: dpp_dsig_get() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        printf("%s(): Exiting with pass, unable to get signal value to verify\n", proc_name);
        return BCM_E_NONE;
    }
    local_port = signal_output.value[0];
    printf("Out_PP_Port value : %d (0x%x)\r\n", local_port, local_port);
    if (local_port != port) {
        printf("%s(): FAIL: Out_PP_Port is expected %d but received %d!\n", proc_name, port, local_port);
        return BCM_E_FAIL;
    }
    printf("%s(): Done. Verified redirection to port 0x%x\n", proc_name, local_port);

    return rv;
}

/* Function to verify packet was redirected to given outLIF */
int run_verify_newlif(int unit, int core) {
    int rv = BCM_E_NONE;
    bcm_gport_t local_outlif;
    match_t match;
    signal_output_t signal_output;
    char *proc_name;

    proc_name = "run_verify_newlif";
    match.block = "ETPP";
    printf("%s(): Enter to match expected outlif %d with actual outlif on %s\r\n", proc_name, outlif, match.block);
    match.from = "Term";
    match.to = "Fwd";
    match.name = "Out_LIF";
    printf("%s(): Take the output, as given in BCM diagnostic command: \n diag pp sig block=%s from=%s to=%s name=%s\n",
           proc_name, match.block, match.from, match.to, match.name);
    rv = dpp_dsig_get(unit, core, &match, &signal_output);
    local_outlif = signal_output.value[0] ;
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in dpp_dsig_get() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        printf("%s(): Exit without fail, because the signal value could not be obtained.\n", proc_name);
        return BCM_E_NONE;
    }
    printf("Out_LIF value : %d (0x%x)\r\n", local_outlif, local_outlif) ;
    if (local_outlif != outlif) {
        printf("%s(): FAIL: Out_LIF is expected %d but received %d!\n", proc_name, outlif, local_outlif);
        return BCM_E_FAIL;
    }

    printf("%s(): Done. Verified Out_LIF change to 0x%x.\n", proc_name, local_outlif);
    return rv;
}
/* Function to verify ForwardingTypeNew action took effect */
int run_verify_ftn(int unit, int core, bcm_field_ForwardingType_t new_fwd_type, int offset, int fix) {
    int rv = BCM_E_NONE;
    uint8 is_jer = 0;
    char *signal_name;
    char *block = "IRPP";
    char *from;
    char *to;
    uint32 signal_value = -1;
    int expected = -1;
    char *proc_name;

    proc_name = "run_verify_ftn";

    printf("%s(): Enter to verify Forwarding type change\n", proc_name);

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : device_is_jer() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }

    if(!is_jer) { /* For Arad and below, the index signal is single */
        printf("%s(): 1. Match Forwarding Offset Index\n", proc_name);
        /* Match expected forwarding offset index */
        signal_name = "Fwd_Offset_Index";
        expected = offset;
        from = "FER";
        to = "LBP";
    } else { /* For Jericho and above, the fix value is changed */
        printf("%s(): Match Forwarding Offset Fix\n", proc_name);
        /* Match expected offset local fix */
        signal_name = "Fwd_Offset_Fix";
        expected = fix;
        from = "PMF";
        to = "FER";
    }

    rv =  dpp_dsig_read(unit, core, block, from, to, signal_name, &signal_value, 1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in dpp_dsig_get() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        printf("%s(): Exit without fail, because the signal value could not be obtained.\n", proc_name);
        return BCM_E_NONE;
    }
    printf("%s(): diag pp sig block=%s from=%s to=%s name=%s ", proc_name, block, from, to, signal_name);
    printf("==> value = %d \n", signal_value);
    if (signal_value != expected) {
        printf("%s(): FAIL: %s is expected %d but received %d!\n", proc_name, signal_name, expected, signal_value);
        return BCM_E_FAIL;
    }
    printf("%s(): Done. Verified %s change to 0x%x.\n", proc_name, signal_name, signal_value);

    return rv;
}

/*
Function to destroy the configured fieldgroup and entry
*/
int field_destroy(int unit, bcm_field_group_t group) {
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "field_destroy";
    /*destroy group and entry*/
    rv = bcm_field_group_flush(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_flush(), returned %d\n", proc_name, rv);
        return rv;
    }

    printf("%s(): Done. Destroyed field group %d with its entries.\n", proc_name, group);
    return rv;
}

/* Main function to configure working L2 and field */
int stat_config_main (int unit, 
                      int core, 
                      int counter_id_0, 
                      int counter_proc, 
                      bcm_field_group_t group, 
                      int group_priority,
                      bcm_vlan_t vsi,
                      bcm_port_t in_port,
                      bcm_port_t out_port,
                      bcm_port_t redir_port, /* redirection local port, pass any if not apply */
                      bcm_gport_t gport, /*new out-LIF or modport; pass any if not apply */
                      int is_count,
                      int is_redir, 
                      int is_newlif) {
    int rv = BCM_E_NONE;
    bcm_gport_t newlif;
    char *proc_name;

    proc_name = "stat_config_main";
    rv = create_vlan_and_ports(unit, vsi, in_port, out_port, redir_port, is_redir);
    if (rv != BCM_E_NONE) {
        printf("%s():Error in function create_vlan_and_ports(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): VLAN AND PORTS CREATED.\n", proc_name);
    if(is_newlif) {
        rv = create_lif(unit, vsi, redir_port, &newlif);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in function create_lif(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): OUTLIF CREATED. newlif = 0x%x\n", proc_name, newlif);
        outlif = newlif; /* Fill file-global variable to later use for verify function */
        gport = newlif; /* Substitute the gport value to use for out-LIF redirection */
    }
    
    rv = field_config(unit, core, counter_id_0, counter_proc, group, group_priority, out_port, redir_port, gport, is_count, is_redir, is_newlif);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in function field_config(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): FIELD CONFIGURATION CREATED.\n", proc_name);

    return rv;
}

int main_ftn (int unit, 
                      int core, 
                      bcm_field_group_t group, 
                      int group_priority,
                      bcm_vlan_t vsi,
                      bcm_port_t in_port,
                      bcm_port_t out_port,
                      bcm_field_ForwardingType_t new_fwd_type,
                      int offset,
                      int fix) {
    int rv = BCM_E_NONE;
    bcm_gport_t newlif;
    char *proc_name;

    proc_name = "stat_config_ftn";
    rv = create_vlan_and_ports(unit, vsi, in_port, out_port, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("%s():Error in function create_vlan_and_ports(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): VLAN AND PORTS CREATED.\n", proc_name);

    rv = field_config_ftn(unit, core, group, group_priority, in_port, new_fwd_type, offset, fix);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in function field_config_ftn(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): FIELD CONFIGURATION CREATED.\n", proc_name);

    return rv;
}

int main_in_modport_drop(int unit,
                         bcm_field_group_t group,
                         int group_priority,
                         bcm_gport_t modport) {
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    int action = bcmFieldActionDrop;
    char *proc_name;

    proc_name = "main_in_modport_drop";
    printf("%s(): Qualify on InPort given as ModPort and Drop.\n", proc_name);

    /*Build the qset*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    /*Build the aset*/
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, action);

    /*Create group with given qset, id and priority*/
    rv = bcm_field_group_create_id(unit, qset, group_priority, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_create_id returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Group %d created.\n", proc_name, group);

    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_action_set returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): ASET attached.\n", proc_name);
    /*Create empty entry*/
    rv = bcm_field_entry_create(unit, group, &ent);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_entry_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Entry %d created.\n", proc_name, ent);
    /*Set the qualifier value for entry*/
    rv = bcm_field_qualify_InPort(unit, ent, modport, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_qualify_SrcPort returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Added qualifier: bcm_field_qualify_InPort.\n", proc_name);

    /*Set the action value for entry*/
    printf("%s(): Adding action...\n", proc_name);
    rv = bcm_field_action_add(unit, ent, action, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_action_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): bcm_field_action_add() done.\n", proc_name);

    /*Commit the entire group to hardware.*/
    printf("%s(): Commit group %d to hardware...", proc_name, group);
    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_install() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    printf("%s(): Done. Field processor configured.\n", proc_name);

    return rv;
}

