/* $Id: Exp $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *  
 * File: cint_field_evpn.c 
 * Purpose: Example of how to FP for EVPN application. 
 *  
 * The application requires two DBs in ingress and one in egress. 
 * Ingress PMF: 
 * EVPN DB
 *  Use one ingress PMF entry to trigger the copy of ingress AC LIF(JR1) or ingress port(JR2) additional
 *  data (20-bit ESI label + 20-bit PE ID) into UDH-extension.
 * Pre-selector:  Forwarding type (L2) + EVPN inLIF profile
 * Key: EVPN inLIF profile
 * Action: copy inLIF/inPort.additional-data to UDH-extension
 *  
 * ESI DB
 *  Use N ingress PMF entries for ESI label lookup to trigger the copy of forbidden-out-port into learn-extension.
 * Pre-selector: Forwarding type (L2) + EVPN inLIF profile + TT termination found
 * Key: ESI label
 * Action: copy forbidden-out-port to learn-extension if match. Otherwise, drop the packet
 *  
 *  
 * Egress PMF: 
 * EVPN DB 
 *  Use 33 egress PMF entries in one DB to drop a bridge packet whose learn-extension (carried forbidden-out-port) equals to the destination system port
 * Pre-selector: bridge packet + learn-extension present
 * Key: 16-bit forbidden port in learn-extension-header + 16-bit destination system port
 * Action: Drop if forbidden port equals destination system port
 *  
 * This application can run either in advanced mode (user arranges preselection programs by herself/himself) or in
 * regular mode (SDK arranges these programs).
 * In order to work with advanced mode set field_presel_mgmt_advanced_mode = 1. For more examples for this mode see cint_field_presel_mgmt_advanced_mode.c.
 *
 * CINT Usage: 
 *  
 *  1. Run:
 *      cint cint_field_evpn.c
 *      Optional: Set field_presel_mgmt_advanced_mode = 1.
 *      field_processor_evpn_example(int unit);
 *  
 *
 */


struct cint_field_evpn_info_s {
    int inlif_profile_evpn; /* Inlif profile value for evpn */
    int inlif_profile_evpn_mask; /* Mask for evpn inlif profile */
    uint8 esi1_label[2]; /* First esi of application */
    uint8 esi2_label[2]; /* Second esi of application */
    uint8 esi_label_mask[2];/* Mask of esi label */
    int forbidden_out_port1; /* First forbidden port */
    int forbidden_out_port2; /* Second forbidden port */
    int ports[4];   /* Ports of the application */
    int advanced_mode; /* Indicates whether this is advanced user mode or not */
    int advanced_mode_presel_ids[4]; /* Array of presel ids, to be configured in case advaned_mode == 1 */


};

cint_field_evpn_info_s cint_field_evpn_info = 
/* inlif_profile_evpn | inlif_profile_evpn_mask */
{         0x1,                    0x1,
/* esi label 1 is 200. In order to insert it to the entry, we take 8 lsbs of 200 which are 200 and 8 msbs which is 0*/
/* esi label 2 is 300. In order to insert it to the entry, we take 8 lsbs of 300 which are 44 and 8 msbs which are 1*/
/* esi1_label | esi2_label */                
    {0,200} ,     {1,44}, 
/* esi_label_mask */ 
    {0xff,0xff}, 
/* forbidden_out_port1 | forbidden_out_port2 */
            0,                   0,
/*      ports    */
    {0, 0, 0, 0},
/*      advanced_mode    */
           0,
/* advanced_mode_presel_ids */
    {0, 0, 0, 0}
};

uint32 used_dnx_order = 0;

bcm_field_presel_t jr2_ing_presel_id = 61;
bcm_field_presel_t jr2_egr_presel_id = 65;
bcm_field_action_t uda0_action_id = bcmFieldActionCount;
bcm_field_action_t uda1_action_id = bcmFieldActionCount;
bcm_field_qualify_t qual_id_udh_base0 = bcmFieldQualifyCount;
bcm_field_qualify_t qual_id_udh_base1 = bcmFieldQualifyCount;


void field_evpn_info_init(int unit,
                    int port1,
                    int port2, 
                    int port3,
                    int port4,
                    int forbidden_out_port1,
                    int forbidden_out_port2
                    ) {
    cint_field_evpn_info.ports[0] = port1; 
    cint_field_evpn_info.ports[1] = port2; 
    cint_field_evpn_info.ports[2] = port3; 
    cint_field_evpn_info.ports[3] = port4;
    cint_field_evpn_info.forbidden_out_port1 = forbidden_out_port1;
    cint_field_evpn_info.forbidden_out_port2 = forbidden_out_port2;
    cint_field_evpn_info.advanced_mode = soc_property_get(unit , "field_presel_mgmt_advanced_mode",0) ? 1 : 0;
    if (cint_field_evpn_info.advanced_mode) {
        cint_field_evpn_info.advanced_mode_presel_ids[0] = 32;
        cint_field_evpn_info.advanced_mode_presel_ids[1] = 31;
        cint_field_evpn_info.advanced_mode_presel_ids[2] = 30;
        cint_field_evpn_info.advanced_mode_presel_ids[3] = 6;
    }
}

/*
 *  Example to ESI DB in ingress PMF. 
 *  
 *  Sequence:
 *  1. 	Define a preselection with TunnelTerminated, ForwardingType=L2 and inLif profile = EVPN as pre-selectors.
 *      Only EVPN packets will be processed.
 *      Qualifier is he ESI label, such that each ESI label points to a forbidden port that will be
 *      written on the Learn Extention 
 *  2. 	Create a Field Group and add 2 entries - for two different ESI labels.
 *  3. 	Create the connection between the Presel and Field Group.
 *  
 */

int field_evpn_esi_db(int unit, int group_priority) {
    bcm_field_presel_set_t psset;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t grp;
    bcm_field_entry_t ent1, ent2, ent3;
    bcm_gport_t forbidden_out_gport;
    int presel_id = 0;
    bcm_field_entry_t presel_flags = 0;
    int result;
    int auxRes;

    /********* PRESELECTOR *************/

    presel_flags = BCM_FIELD_QUALIFY_PRESEL;

    /* Cretae a presel entity */
    if (cint_field_evpn_info.advanced_mode) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        presel_id = cint_field_evpn_info.advanced_mode_presel_ids[0];
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS);
            return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id);
            return result;
        }
    }

    /* Configure the presel */
    result = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    result = bcm_field_qualify_InPort(unit, presel_id | presel_flags, cint_field_evpn_info.ports[2], 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_InPort\n");
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, presel_id | presel_flags, bcmFieldForwardingTypeL2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    result = bcm_field_qualify_InterfaceClassVPort(unit, presel_id | presel_flags, cint_field_evpn_info.inlif_profile_evpn, cint_field_evpn_info.inlif_profile_evpn_mask);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_InterfaceClassVPort\n");
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /********* QUALIFIERS *********/

    BCM_FIELD_QSET_INIT(qset);

    /* Create data qualifier ESI label */
    bcm_field_data_qualifier_t bcm_FieldQualifyEsi;

    /* the qualify inner dmac1 of user define */
    bcm_field_data_qualifier_t_init(&bcm_FieldQualifyEsi);
    /* the qualify of user define */
    bcm_FieldQualifyEsi.offset_base = bcmFieldDataOffsetBaseForwardingHeader;  /* the position within the packet starting at which the qualifier is extracted packet */
    bcm_FieldQualifyEsi.offset = (used_dnx_order ? 32 : 39);
    bcm_FieldQualifyEsi.length = 20;   /* Matched data byte length */
    bcm_FieldQualifyEsi.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE|BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES|BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES  ; /* Negative bit offset */

    if (used_dnx_order) {
        bcm_FieldQualifyEsi.flags |= BCM_FIELD_DATA_QUALIFIER_BIT_OFFSET_DNX_ORDER;
    }

    /* Create a data/offset-based qualifier */
    result = bcm_field_data_qualifier_create(unit, &bcm_FieldQualifyEsi);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_data_qualifier_create $result\n");
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &qset, bcm_FieldQualifyEsi.qual_id);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qset_data_qualifier_add $result\n");
        return result;
    }



    result = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }


    result = bcm_field_group_presel_set(unit, grp, &psset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    /********* ACTION *********/

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionLearnSrcPortNew);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    result = bcm_field_group_action_set(unit, grp, aset); 
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    /* Entries - one for each ESI label */
    result = bcm_field_entry_create(unit, grp, &ent1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_destroy(unit, ent1);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_entry_create(unit, grp, &ent2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_destroy(unit, ent2);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_entry_create(unit, grp, &ent3);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_destroy(unit, ent3);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }


    /* Add data field that the packet must match to trigger qualifier */

    /* ESI label 1 */
    result = bcm_field_qualify_data(unit, ent1, bcm_FieldQualifyEsi.qual_id, cint_field_evpn_info.esi1_label, cint_field_evpn_info.esi_label_mask/*label should be exact, other fields masked*/, 2);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_data $result\n");
        return result;
    }

    /* ESI label 2 */
    result = bcm_field_qualify_data(unit, ent2, bcm_FieldQualifyEsi.qual_id, cint_field_evpn_info.esi2_label, cint_field_evpn_info.esi_label_mask/*label should be exact, other fields masked*/, 2);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_data $result\n");
        return result;
    }

    /* Action: copy forbidden-out-port to learn-extension if match. */
    BCM_GPORT_LOCAL_SET(forbidden_out_gport, cint_field_evpn_info.forbidden_out_port1);
    result = bcm_field_action_add(unit, ent1, bcmFieldActionLearnSrcPortNew, forbidden_out_gport, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    /* Set priority for the entry */
    result = bcm_field_entry_prio_set(unit, ent1, 3);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_prio_set\n");
        return result;
    }

    BCM_GPORT_LOCAL_SET(forbidden_out_gport, cint_field_evpn_info.forbidden_out_port2);
    result = bcm_field_action_add(unit, ent2, bcmFieldActionLearnSrcPortNew, forbidden_out_gport, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    /* Set priority for the entry */
    result = bcm_field_entry_prio_set(unit, ent2, 2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_prio_set\n");
        return result;
    }

    /* Otherwise, drop the packet (no qualifier) */
    result = bcm_field_action_add(unit, ent3, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }
    
    /* Set priority for the entry - this entry must be lowest priority to act as default action */
    result = bcm_field_entry_prio_set(unit, ent3, 1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_prio_set\n");
        return result;
    }

    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_destroy(unit, 0);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    return result;
}


/*
 *  Example to EVPN DB in ingress PMF in JR1. 
 *  
 *  Sequence:
 *  1.  Define a preselection with ForwardingType=L2 and inLif profile = EVPN as pre-selectors.
 *      Only EVPN packets will be processed.
 *      Qualifier is the inlif profile, such that it copies the additional data to UDH Extention.
 *  2.  Create a Field Group and add one entries - for copying additional data to UDH Extention.
 *  3.  Create the connection between the Presel and Field Group.
 *
 *  In Jericho, for the falling back to bridge case, the forwarding type will be bcmFieldForwardingTypeIp4Mcast
 *  instead of bcmFieldForwardingTypeL2. For this case, pass forwarding_type == bcmFieldForwardingTypeIp4Mcast.
 */
int field_evpn_evpn_db(int unit, int group_priority, bcm_field_ForwardingType_t forwarding_type) {
    bcm_field_presel_set_t psset;
    bcm_field_qset_t qset_lsb, qset_msb;
    bcm_field_aset_t aset;
    bcm_field_group_config_t grp_lsb;
    bcm_field_group_config_t grp_msb;
    bcm_field_entry_t ent1, ent2;
    int presel_id = 0;
    int result;
    int auxRes;
    bcm_field_entry_t presel_flags = 0;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_inlif;

    /********* PRESELECTOR *************/

    presel_flags = BCM_FIELD_QUALIFY_PRESEL;

    if (cint_field_evpn_info.advanced_mode) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        if (forwarding_type == bcmFieldForwardingTypeL2) {
            presel_id = cint_field_evpn_info.advanced_mode_presel_ids[1];
        } else if (forwarding_type == bcmFieldForwardingTypeIp4Mcast) {
            presel_id = cint_field_evpn_info.advanced_mode_presel_ids[2];
        }
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id | presel_flags);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return result;
        }
        
    } else {
        /* Cretae a presel entity */
        result = bcm_field_presel_create(unit, &presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id);
            return result;
        }
    }

    /* Configure the presel */
    result = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, presel_id | presel_flags, forwarding_type);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    result = bcm_field_qualify_InterfaceClassVPort(unit, presel_id | presel_flags, cint_field_evpn_info.inlif_profile_evpn, cint_field_evpn_info.inlif_profile_evpn_mask);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_InterfaceClassVPort\n");
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /********* QUALIFIERS *********/

    BCM_FIELD_QSET_INIT(qset_lsb);
    BCM_FIELD_QSET_INIT(qset_msb);

    /* Create data qualifier for inLif additional data */
    bcm_field_data_qualifier_t bcm_FieldQualifyAddtionalData_lsb;

    bcm_field_data_qualifier_t_init(&bcm_FieldQualifyAddtionalData_lsb);
    /* the qualify of user define */
    bcm_FieldQualifyAddtionalData_lsb.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;     
    bcm_FieldQualifyAddtionalData_lsb.offset = 0;   /* Packet bit offset */
    bcm_FieldQualifyAddtionalData_lsb.length = 16;   /* Matched data bit length */
    bcm_FieldQualifyAddtionalData_lsb.qualifier = bcmFieldQualifyInVPortWide;
    bcm_FieldQualifyAddtionalData_lsb.stage = bcmFieldStageIngress;

    /* Create data qualifier for inLif additional data */
    bcm_field_data_qualifier_t bcm_FieldQualifyAddtionalData_msb;

    bcm_field_data_qualifier_t_init(&bcm_FieldQualifyAddtionalData_msb);
    /* the qualify of user define */
    bcm_FieldQualifyAddtionalData_msb.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;     
    bcm_FieldQualifyAddtionalData_msb.offset = 16;   /* Packet bit offset */
    bcm_FieldQualifyAddtionalData_msb.length = 24;   /* Matched data bit length */
    bcm_FieldQualifyAddtionalData_msb.qualifier = bcmFieldQualifyInVPortWide;
    bcm_FieldQualifyAddtionalData_msb.stage = bcmFieldStageIngress;


    /* Create a data/offset-based qualifier */
    result = bcm_field_data_qualifier_create(unit, &bcm_FieldQualifyAddtionalData_lsb);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_data_qualifier_create $result\n");
        return result;
    }
    /* Create a data/offset-based qualifier */
    result = bcm_field_data_qualifier_create(unit, &bcm_FieldQualifyAddtionalData_msb);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_data_qualifier_create $result\n");
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &qset_lsb, bcm_FieldQualifyAddtionalData_lsb.qual_id);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qset_data_qualifier_add $result\n");
        return result;
    }
    result = bcm_field_qset_data_qualifier_add(unit, &qset_msb, bcm_FieldQualifyAddtionalData_msb.qual_id);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qset_data_qualifier_add $result\n");
        return result;
    }


    bcm_field_group_config_t_init(&grp_lsb);
    bcm_field_group_config_t_init(&grp_msb);
    /*  Create two Field groups with type Direct Extraction */
    /*  One group for LSB of the Additional Data and one for the MSB */
    grp_lsb.priority = group_priority;
    grp_lsb.qset = qset_lsb;
    grp_lsb.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET ;
    grp_lsb.mode = bcmFieldGroupModeDirectExtraction;

    BCM_FIELD_ASET_INIT(grp_lsb.aset);
    BCM_FIELD_ASET_ADD(grp_lsb.aset, bcmFieldActionClassSourceSet);

    result = bcm_field_group_config_create(unit, &grp_lsb);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }
    result = bcm_field_group_presel_set(unit, grp_lsb.group, &psset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp_lsb.group);
        return result;
    }

    grp_msb.priority = group_priority+1;
    grp_msb.qset = qset_msb;
    grp_msb.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET ;
    grp_msb.mode = bcmFieldGroupModeDirectExtraction;

    BCM_FIELD_ASET_INIT(grp_msb.aset);
    BCM_FIELD_ASET_ADD(grp_msb.aset, bcmFieldActionClassDestSet);

    result = bcm_field_group_config_create(unit, &grp_msb);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }

    result = bcm_field_group_presel_set(unit, grp_msb.group, &psset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp_msb.group);
        return result;
    }


    /* Create the Direct Extraction entries: two entries are required:
     *  One entry is required for the LSBs of the Additional Data
     *  One entry is required for the MSBs of the Additional Data 
     */

    /* Entry1: LSB*/
    result = bcm_field_entry_create(unit, grp_lsb.group, &ent1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_destroy(unit, ent1);
        auxRes = bcm_field_group_destroy(unit, grp_lsb.group);
        return result;
    }

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_inlif);

    /*UDH*/
    extract.action = bcmFieldActionClassSourceSet; 
    extract.bias = 0;

    /* extraction structure indicates to use InLif qualifier */
    ext_inlif.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD; 
    ext_inlif.bits  = 16;
    ext_inlif.lsb   = 0;
    ext_inlif.qualifier = bcm_FieldQualifyAddtionalData_lsb.qual_id;

    result = bcm_field_direct_extraction_action_add(unit,
                                                    ent1,
                                                    extract,
                                                    1 /* count */,
                                                    &ext_inlif);

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

     /* Write entry to HW */
      result = bcm_field_entry_install(unit, ent1);
    if (BCM_E_NONE != result) {
         printf("Error in bcm_field_entry_install\n");
        return result;
    }

    /* Entry2: MSB*/
    result = bcm_field_entry_create(unit, grp_msb.group, &ent2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_inlif);

    /*UDH*/
    extract.action = bcmFieldActionClassDestSet;
    extract.bias = 0;

    /* extraction structure indicates to use InLif qualifier */
    ext_inlif.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_inlif.bits  = 24;
    ext_inlif.lsb   = 0;
    ext_inlif.qualifier = bcm_FieldQualifyAddtionalData_msb.qual_id;
    result = bcm_field_direct_extraction_action_add(unit,
                                                    ent2,
                                                    extract,
                                                    1 /* count */,
                                                    &ext_inlif);

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    /* Write entry to HW */
      result = bcm_field_entry_install(unit, ent2);
    if (BCM_E_NONE != result) {
         printf("Error in bcm_field_entry_install\n");
        return result;
    }

    return result;
}

/*
 *  Example to EVPN DB in ingress PMF in JR2.
 *
 *  Sequence:
 *  1.  Define a context with ForwardingType=ETH (configurable) and AC.inLif_profile = EVPN.
 *      Define a context with ForwardingType=IP4 (configurable) and AC.inLif_profile = EVPN.
 *      Only EVPN packets will be processed.
 *  2.  Create user define actions on UDH0(24b) and UDH1(16b) for carrying ESI(20b)+PE-ID(20).
 *  3.  Create UDH0 type to indicate size of UDH0 and UDH1 type to indicate size of UDH1
 *  4.  Add filed group for PMF2 with EXTRACT field group type:
 *      Qualifier: 1) UDH0 type, 2) UDH1 type, 3) port.additional-data
 *      Actions: 1) created UDH0, 2) created UDH1, 3) created UDH0 type, 4) created UDH1 type.
 *  5.  Attach the context and the field group.
 *
 *  Since the field group extracts the action value from qualifiers, no field entries are needed.
 *  Then the field group will copy UDH0 type and UDH1 type to UDH0 base and UDH1 base, and the
 *  port.additional-data(40b) to UDH0(24b) and UDH1(16b).
 */
int field_jr2_comp_evpn_db(int unit, int presel_id, bcm_field_layer_type_t forwarding_type)
{
    int rv= 0;
    bcm_field_context_t context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;

    /********* Create PRESELECTOR *************/
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_evpn_ingress_context_create: Error in bcm_field_context_create context %d\n", jr2_pmf2_context_id);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    /**For iPMF2, iPMF1 presel must be configured*/
    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyInVportClass;
    presel_entry_data.qual_data[0].qual_arg = (forwarding_type == bcmFieldLayerTypeEth)? 0 : 1;
    presel_entry_data.qual_data[0].qual_value = cint_field_evpn_info.inlif_profile_evpn;
    presel_entry_data.qual_data[0].qual_mask = cint_field_evpn_info.inlif_profile_evpn_mask;
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = forwarding_type;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_evpn_context_create Error in bcm_field_presel_set  \n");
        return rv;
    }

    /********* Add QUALIFIERS *********/
    bcm_field_group_info_t fg_info;
    int qual_and_action_index;
    char *proc_name;
    bcm_field_group_t ingress_fg_id;
    proc_name = "field_dir_extract_esi_pe_id_to_udh";

    /** Init the fg_info structure. */
    bcm_field_group_info_t_init(&fg_info);

    /*
     * Create user define action with UDA0 for(ESI+PE-ID)[39:16]
     */
    if (uda0_action_id == bcmFieldActionCount)
    {
        bcm_field_action_info_t action_info;
        bcm_field_action_info_t_init(&action_info);

        /**Fill the structure needed for bcm_field_qualifier_create API */
        action_info.stage = bcmFieldStageIngressPMF2;
        action_info.action_type = bcmFieldActionUDHData0;
        action_info.size = 24;
        action_info.prefix_size=8;
        action_info.prefix_value=0x0;
        sal_memcpy(action_info.name, "UDHData0", sizeof(action_info.name));
        rv = bcm_field_action_create(unit, 0, &action_info, &uda0_action_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_action_create\n", rv);
            return rv;
        }
    }

    /*
     * Create user define action with UDA1 for(ESI+PE-ID)[15:0]
     */
    
    if (uda1_action_id == bcmFieldActionCount)
    {
        bcm_field_action_info_t action_info;
        bcm_field_action_info_t_init(&action_info);
        
        action_info.stage = bcmFieldStageIngressPMF2;
        action_info.action_type = bcmFieldActionUDHData1;
        action_info.size = 16;
        action_info.prefix_size=16;
        action_info.prefix_value=0x0;
        sal_memcpy(action_info.name, "UDHData1", sizeof(action_info.name));
        rv = bcm_field_action_create(unit, 0, &action_info, &uda1_action_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_action_create\n", rv);
            return rv;
        }
    }

    /*
     * Create user define qualifier for UDHBASE0[1:0], UDHBASE1[1:0]
     */
    if (qual_id_udh_base0 == bcmFieldQualifyCount)
    {
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 2;
        sal_memcpy(qual_info.name, "UDHBASE0", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id_udh_base0);
        if(rv != BCM_E_NONE)
        {
            printf("%s Error in bcm_field_qualifier_create esi_type_qual\n", proc_name);
            return rv;
        }
    }

    if (qual_id_udh_base1 == bcmFieldQualifyCount)
    {
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 2;
        sal_memcpy(qual_info.name, "UDHBASE1", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id_udh_base1);
        if(rv != BCM_E_NONE)
        {
            printf("%s Error in bcm_field_qualifier_create esi_type_qual\n", proc_name);
            return rv;
        }
    }

    /*
     *  Fill the fg_info structure, Add the FG. 
     */
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = bcmFieldQualifyPortClassPacketProcessingGeneralData;
    fg_info.qual_types[1] = qual_id_udh_base0;
    fg_info.qual_types[2] = qual_id_udh_base1;

    fg_info.nof_actions = 4;
    fg_info.action_types[0] = uda1_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = uda0_action_id;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionUDHBase0;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionUDHBase1;
    fg_info.action_with_valid_bit[3] = FALSE;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &ingress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }

    /*
     * Attach the field group ID to the context.
     */
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_info_get(unit, ingress_fg_id, &fg_info);

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for(qual_idx=0; qual_idx< fg_info.nof_quals; qual_idx++)
    {
        attach_info.key_info.qual_types[qual_idx] = fg_info.qual_types[qual_idx];
    }
    for(act_idx=0; act_idx<fg_info.nof_actions; act_idx++)
    {
        attach_info.payload_info.action_types[act_idx] = fg_info.action_types[act_idx];
    }

    /** UDH Data Type */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = 2; /** Length of UDH1 is 3Byte*/
    attach_info.key_info.qual_info[1].offset = 0;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 1; /** Length of UDH2 is 2Byte*/
    attach_info.key_info.qual_info[2].offset = 0;
    rv = bcm_field_group_context_attach(unit, 0, ingress_fg_id, context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", ingress_fg_id, context_id);
        return rv;
    }
    return rv;
}


/*
 *  Example to EVPN DB in egress PMF.
 *  
 *  Drop a bridge packet whose learn-extension (carried forbidden-out-port) equals to the destination system port
 *  
 *  Sequence:
 *  1. 	Define a preselection with ForwardingType=L2 and learn extention present (!!!!).
 *      Only EVPN packets with learn extention will be processed.
 *      Qualifier is the 16-bit forbidden port in learn-extension-header + 16-bit destination system port,
 *      such that if forbidden port equals destination system port packet is dropped
 *  2. 	Create a Field Group and add 3 entries - for three different forbidden ports.
 *  3. 	Create the connection between the Presel and Field Group.
 *  
 */

int field_evpn_evpn_egress_filter_db(int unit, int group_priority) {
    bcm_field_presel_set_t psset;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t grp;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp_conf;
    int presel_id;
    bcm_field_entry_t presel_flags = 0;
    int result;
    int auxRes;
    int cur_port;
    bcm_gport_t learn_src_port = 0;
    int base_modid, data_modid, data_tm_port, core;
    int rv;

    /********* PRESELECTOR *************/

    /* Cretae a presel entity */

    presel_flags |= BCM_FIELD_QUALIFY_PRESEL;

    if (cint_field_evpn_info.advanced_mode) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS;
        presel_id = cint_field_evpn_info.advanced_mode_presel_ids[3];
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageEgress, presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return result;
        }
    } else {
        result = bcm_field_presel_create(unit, &presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id);
            return result;
        }
    }

    /* Configure the presel */
    result = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageEgress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }


    result = bcm_field_qualify_ForwardingType(unit, presel_id | presel_flags, bcmFieldForwardingTypeL2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /********* QUALIFIERS *********/

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyLearnSrcPort);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    bcm_field_group_config_t_init(&grp_conf);
    grp_conf.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    grp_conf.mode = bcmFieldGroupModeAuto;
    grp_conf.qset = qset;
    grp_conf.aset = aset;
    grp_conf.priority = group_priority;
    grp_conf.preselset = psset;
/* *********************************     */
    result = bcm_field_group_config_create(unit, &grp_conf);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    
    grp = grp_conf.group;
/* *********************************     */
    /* Entries */
    for (cur_port = 0; cur_port < 3; cur_port++) {
        result = bcm_field_entry_create(unit, grp, &ent);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_entry_create\n");
            auxRes = bcm_field_entry_destroy(unit, ent);
            auxRes = bcm_field_group_destroy(unit, grp);
            return result;
        }

        BCM_GPORT_LOCAL_SET(learn_src_port, cint_field_evpn_info.ports[cur_port]);

        rv = bcm_stk_modid_get(unit, &base_modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_get\n");
            return rv;
        }

        data_modid = 0;
        data_tm_port = cint_field_evpn_info.ports[cur_port];

        if (is_device_or_above(unit,JERICHO)) {
            rv = get_core_and_tm_port_from_port(unit, cint_field_evpn_info.ports[cur_port], &core, &data_tm_port);
            if (rv != BCM_E_NONE) {
                printf("Error, in get_core_and_tm_port_from_port\n");
                return rv;
            }
            data_modid = base_modid + core;

        }
        result = bcm_field_qualify_DstPort(unit, ent, data_modid, 0, data_tm_port, -1);
        if (result != BCM_E_NONE) {
            printf("Error, bcm_field_qualify_DstPort $result\n");
            return result;
        }

        /* Learn Extention */
        result = bcm_field_qualify_LearnSrcPort(unit, ent, learn_src_port);
        if (result != BCM_E_NONE) {
            printf("Error, bcm_field_qualify_data $result\n");
            return result;
        }


        result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_action_add\n");
            return result;
        }

    }

    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    return result;
}


/* Parameters:
   int unit
   int port1-4: ports of the application
   forbidden_out_port1-2: forbidden out ports to be used in esi_db
*/
int field_evpn_field_processor_example(/* in */ int unit, int port1, int port2, int port3, int port4, int forbidden_out_port1,int forbidden_out_port2)
{
    int group_priority = 10;
    int result=BCM_E_NONE;


    field_evpn_info_init(unit,port1, port2, port3, port4, forbidden_out_port1, forbidden_out_port2);

    /* Prioirity of esi db should be higher than evpn db, because the former's program selection's criteria are more specific*/
    result = field_evpn_esi_db(unit, 5/*group_priority*/);
        if (BCM_E_NONE != result) {
          printf("Error in esi_db\n");
            return result;
        }

    
    result = field_evpn_evpn_db(unit, 3/*group_priority*/, bcmFieldForwardingTypeL2);
        if (BCM_E_NONE != result) {
            printf("Error in evpn_db with ethernet\n");
            return result;
        }

    /* In Jericho, for the falling back to bridge case, the forwarding type will be bcmFieldForwardingTypeIp4Mcast
       instead of bcmFieldForwardingTypeL2. For this case, pass forwarding_type == bcmFieldForwardingTypeIp4Mcast.*/
    result = field_evpn_evpn_db(unit, 1/*group_priority*/, bcmFieldForwardingTypeIp4Mcast);
        if (BCM_E_NONE != result) {
            printf("Error in evpn_db with multicast\n");
            return result;
        }
    result = field_evpn_evpn_egress_filter_db(unit, 6/*group_priority*/);
        if (BCM_E_NONE != result) {
            printf("Error in evpn_egress_filter_db\n");
            return result;
        }

    return result;
}

/*
 * PMF cint example for multi-dev case
 * It configure the same DB as field_evpn_field_processor_example but in different
 * unit per the DB location.
 *
 * Parameters:
 * int ingress_unit: ingress device unit-id.
 * int egress_unit: egress device unit-id.
 * int port1-4: ports of the application
 * forbidden_out_port1-2: forbidden out ports to be used in esi_db
 */
int multi_dev_field_evpn_field_processor_example(/* in */ int ingress_unit, int egress_unit, int port1, int port2, int port3, int port4, int forbidden_out_port1,int forbidden_out_port2)
{
    int group_priority = 10;
    int result=BCM_E_NONE;

    field_evpn_info_init(ingress_unit, port1, port2, port3, port4, forbidden_out_port1, forbidden_out_port2);

    if (!is_device_or_above(ingress_unit, JERICHO2)) {
        /* Prioirity of esi db should be higher than evpn db, because the former's program selection's criteria are more specific*/
        result = field_evpn_esi_db(ingress_unit, 5/*group_priority*/);
        if (BCM_E_NONE != result) {
          printf("Erro,unit: %d, r in esi_db\n", ingress_unit);
            return result;
        }

        result = field_evpn_evpn_db(ingress_unit, 3/*group_priority*/, bcmFieldForwardingTypeL2);
        if (BCM_E_NONE != result) {
            printf("Error,unit: %d,  in evpn_db with ethernet\n", ingress_unit);
            return result;
        }

        /* In Jericho, for the falling back to bridge case, the forwarding type will be bcmFieldForwardingTypeIp4Mcast
           instead of bcmFieldForwardingTypeL2. For this case, pass forwarding_type == bcmFieldForwardingTypeIp4Mcast.*/
        result = field_evpn_evpn_db(ingress_unit, 1/*group_priority*/, bcmFieldForwardingTypeIp4Mcast);
        if (BCM_E_NONE != result) {
            printf("Error,unit: %d,  in evpn_db with multicast\n", ingress_unit);
            return result;
        }
    } else {
        
        printf("The PMF ESI filter example won't be ready until all related qualifers and actions are ready!!!\n");

        result = field_jr2_comp_evpn_db(ingress_unit, jr2_ing_presel_id/*presel_id*/, bcmFieldLayerTypeEth);
        if (BCM_E_NONE != result) {
            printf("Error,unit: %d,  in evpn_db with ethernet\n", ingress_unit);
            return result;
        }

        result = field_jr2_comp_evpn_db(ingress_unit,(jr2_ing_presel_id+1)/*presel_id*/, bcmFieldLayerTypeIp4);
        if (BCM_E_NONE != result) {
            printf("Error,unit: %d,  in evpn_db with ethernet\n", ingress_unit);
            return result;
        }
    }

    if (!is_device_or_above(egress_unit, JERICHO2)) {
        result = field_evpn_evpn_egress_filter_db(egress_unit, 6/*group_priority*/);
        if (BCM_E_NONE != result) {
            printf("Error,unit: %d,  in evpn_egress_filter_db\n", egress_unit);
            return result;
        }
    } else {
        
        printf("The PMF ESI filter example won't be ready until all related qualifers and actions are ready!!!\n");
    }

    return result;
}

