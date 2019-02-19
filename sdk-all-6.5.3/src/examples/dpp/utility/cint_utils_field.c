/*
 * $id cint_utils_field.c $
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
 * Different field processor utility functions and constants
 *
 */


/*
 * Pre-defined action codes
 * These values are hard-coded and should not be changed
 */
int prge_action_code_bfd_c_bit_clear_wa        = 1;
int prge_action_code_reflector_1pass           = 2;
int prge_action_code_oam_end_tlv_wa            = 3;


/*
 * prge_action_create_field_group
 *
 * Allocates Egress PMF program and defines a key and action
 * Key: (DstPort,FheiSize,Fhei.TrapCode)
 * Action: Stat
 */
int field__prge_action_create_field_group(int unit, int group_priority, bcm_field_group_t grp_tcam, int qual_id)
{
    int result;
    int presel_id;
    bcm_field_aset_t aset;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_presel_set_t psset;
    bcm_field_group_config_t grp_conf;

    /* Cretae a presel entity */
    result = bcm_field_presel_create(unit, &presel_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_presel_create_id\n");
        result = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }

    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeRxReason);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    bcm_field_group_config_t_init(&grp_conf);
    grp_conf.group = grp_tcam;
    grp_conf.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp_conf.mode = bcmFieldGroupModeAuto;
    grp_conf.priority = group_priority;
    grp_conf.preselset = psset;

    BCM_FIELD_QSET_INIT(grp_conf.qset);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyDstPort);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyFheiSize);

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_WITH_ID | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.qual_id = qual_id;
    data_qual.offset = 0;
    data_qual.qualifier = bcmFieldQualifyFhei;
    data_qual.length = 8;
    data_qual.stage = bcmFieldStageEgress;
    result = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &grp_conf.qset, data_qual.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }

    result = bcm_field_group_config_create(unit, &grp_conf);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create for group %d\n", grp_conf.group);
        return result;
    }

    /* Define the ASET - use counter 0. */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

    /* Attach the action set */
    result = bcm_field_group_action_set(unit, grp_tcam, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set for group %d\n", grp_tcam);
      return result;
    }

    return result;
}

/*
 * prge_action_code_add_entry
 *
 * Adds an entry for the key and sets value=action_code for
 * the action created by field__prge_action_create_field_group
 */
int field__prge_action_code_add_entry(int unit, bcm_field_group_t grp_tcam,
                                      uint8 trap_code, int dest_port, int qual_id,
                                      int action_code)
{
    int result;
    bcm_field_entry_t ent_tcam;
    bcm_gport_t local_gport;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int modid;
    int ace_var, statId;
    uint8 dqData[1], dqMask[1];

    char *proc_name ;
    bcm_field_action_core_config_t core_config_arr[SOC_DPP_DEFS_MAX_NOF_CORES()] ;
    int core_config_arr_len ;
    unsigned long local_gport_as_long ;
    unsigned long statId_as_long ;
    int action_stat ;
    int cores_num, ii ;

    proc_name = "field__prge_action_code_add_entry" ;
    printf("%s(): Enter\r\n",proc_name) ;
    
    /* Get the actual device cores number */
    cores_num = SOC_DPP_DEFS_GET_NOF_CORES(unit);

    result = bcm_stk_modid_get(unit,&modid);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_stk_modid_get\n");
        return result;
    }

    result = bcm_port_get(unit, dest_port, &flags, &interface_info, &mapping_info);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_port_get\n");
        return result;
    }

    result = bcm_field_entry_create(unit, grp_tcam, &ent_tcam);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    /* Entry qualifier values */
    result = bcm_field_qualify_DstPort(unit, ent_tcam,
                                       modid + mapping_info.core, 0xffffffff,
                                       mapping_info.tm_port, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_DstPort\n");
        return result;
    }

    result = bcm_field_qualify_FheiSize(unit, ent_tcam, 3, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_FheiSize\n");
        return result;
    }

    dqData[0] = trap_code;
    dqMask[0] = 0xFF;
    result = bcm_field_qualify_data(unit, ent_tcam, qual_id, &(dqData[0]), &(dqMask[0]), 1 /* len in bytes */);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_data\n");
        return result;
    }

    /* Entry Action values */
    BCM_GPORT_LOCAL_SET(local_gport, dest_port);
    ace_var = (action_code << 8);
    statId = (ace_var << 19); /* Set internal data */

    action_stat = bcmFieldActionStat ;
    local_gport_as_long = local_gport ;

    if (is_device_or_above(unit, JERICHO)) {
        /*
         * Enter for Jericho and up
         */

        /*
         * Set the counter at the same location as in Arad (since the LS 19 bits are '0'):
         * For Arad, when the LS 19 bits are zero, on statId, selected counter will be as the destination port on the
         * last 256 entries of the Ace table (which is 4K entries long).
         */
        statId |= (4*1024 - 256 - 1 + dest_port) ; 
        statId_as_long = statId ;

        printf("%s(): JERICHO: statId %d (0x%08lX) local_gport %d (0x%08lX) dest_port %d\r\n",proc_name,statId,statId_as_long,local_gport,local_gport_as_long,dest_port) ;

        for (ii = 0 ; ii < cores_num ; ii++) {
          core_config_arr[ii].param0 = statId ;
          core_config_arr[ii].param1 = local_gport ;
          core_config_arr[ii].param2 = BCM_ILLEGAL_ACTION_PARAMETER ;
        }

        printf("%s(): Call bcm_field_action_config_add(ent_tcam %d bcmFieldActionStat %d statId %d local_gport %d)\r\n",proc_name,ent_tcam,action_stat,statId,local_gport) ;

        /*
         * Setting 'core_config_arr_len' to '1' results in setting all cores the same way.
         */
        core_config_arr_len = cores_num ;
        result = bcm_field_action_config_add(unit, ent_tcam, bcmFieldActionStat, core_config_arr_len, &core_config_arr[0]) ;
        if (BCM_E_NONE != result) {
            printf("%s(): Error in bcm_field_action_config_add\n",proc_name);
            return result;
        }
    } else {
        statId_as_long = statId ;

        printf("%s(): ARAD: statId %d (0x%08lX) local_gport %d (0x%08lX) dest_port %d\r\n",proc_name,statId,statId_as_long,local_gport,local_gport_as_long,dest_port) ;

        result = bcm_field_action_add(unit, ent_tcam, bcmFieldActionStat, statId, local_gport);
        if (BCM_E_NONE != result) {
            printf("%s(): Error in bcm_field_action_add\n",proc_name);
            return result;
        }
    }

    result = bcm_field_entry_install(unit, ent_tcam);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in bcm_field_entry_install\n",proc_name);
        return result;
    }

    printf("%s(): Exit OK\r\n",proc_name) ;
    return result;
}

/*
 * Create the ingress field processor rule
 * Key: SrcPort+OuterVLAN
 * Action: Redirect to DstPort and trap packet
 */
int field__redirect_trap_ingress_rule_create(int unit,
                                             bcm_port_t src_port, int vlan,
                                             bcm_port_t dst_port, int trap_code) {

    int rv = BCM_E_NONE;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_gport_t trap_gport, src_gport, dst_gport;

    BCM_GPORT_TRAP_SET(trap_gport, trap_code, 7, 0);
    BCM_GPORT_LOCAL_SET(src_gport, src_port);
    BCM_GPORT_LOCAL_SET(dst_gport, dst_port);

    bcm_field_group_config_t_init(&grp);

    /*
     * Define QSET - Key = VLAN + Source port
     */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifySrcPort);

    /*
     * Define ASET - Trap and redirect
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);

    /*
     * Create the Field group with type Direct Extraction
     */
    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create\n");
        return rv;
    }
    printf("Created group with id: 0x%08x\n", grp.group);

    /*  Attach the action set */
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set\n");
        return rv;
    }

    /*
     * Create relevant entry
     */
    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Qualifier value */
    rv = bcm_field_qualify_SrcPort(unit, ent, 0, 0, src_gport, -1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_SrcPort (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_qualify_OuterVlan(unit, ent, vlan, -1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_OuterVlan (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Actions */
    rv = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, 0, dst_gport);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_action_add (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_action_add(unit, ent, bcmFieldActionTrap, trap_gport, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_action_add (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Install entry */
    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_install (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("Reflection was set\n");

    return BCM_E_NONE;
}


