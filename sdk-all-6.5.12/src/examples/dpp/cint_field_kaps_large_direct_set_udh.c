/*
 *
 * $Id: cint_field_kaps_large_direct_set_udh.c,v 1.15 2017/08/18 08:43:00 Lingyong Chen Exp $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * 
 * File: cint_field_kaps_large_direct_set_udhc
 * Purpose: use pmf large direct to set the UDH header .
 * IPv6
 *  	set UDH0 with {Dest-VTEP-IPv4-Address[32b]}
 *    set UDH1 with {Reserved[4b], TOR-ID[28]}
 * IPv6
 *  	set UDH0 with {Dest-VTEP-IPv4-Address[32b]}
 *    set UDH1 with {Reserved[16b], IP-Cksum-Fix[16]}
 *
 * Usage:
 *    soc property pmf_kaps_large_db_size  to set the PMF large direct size.
 * 
 */
/********** 
  functions
 ********** */
int field_kaps_large_direct_set_udh(int unit, int entry_num, uint32 *dest_vtep_info_ptr, uint32 *udh0, uint32 *udh1) {
    bcm_field_data_qualifier_t data_qual;
    int presel_id = 0;
    int presel_flags = 0;
    bcm_field_stage_t stage; 
    bcm_field_presel_set_t presel_set;    
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t action_entry;
    int result = 0;
    uint8 data[3], mask[3]; 
    int i;
    
 
    result = bcm_field_control_set(unit, bcmFieldControlLargeDirectLuKeyLength, 14);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_control_set with Err %x\n", result);
        return result;
    }
 
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.qualifier = bcmFieldQualifyL2DestValue;
    data_qual.length = 14;
    data_qual.offset = 20;
 
    result = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create with Err %x\n", result);
        return result;
    } 
    
    /*Define preselector*/
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            return result;
        } else {
            printf("bcm_field_presel_create_id: %x\n", presel_id);
        }
    } else {
        result = bcm_field_presel_create(unit, &presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            return result;
        } else {
            printf("bcm_field_presel_create_id: %x\n", presel_id);
        }
    }
    BCM_FIELD_PRESEL_INIT(presel_set);
    BCM_FIELD_PRESEL_ADD(presel_set, presel_id);
 
    stage = bcmFieldStageIngress;
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage);
 
    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp);
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_LARGE | BCM_FIELD_GROUP_CREATE_WITH_ID;
    grp.mode = bcmFieldGroupModeDirect;
    grp.group = 20; 
    grp.preselset = presel_set;
 
    /*add qualifiers*/
    BCM_FIELD_QSET_INIT(grp.qset);
    
    result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (BCM_E_NONE != result) {
        printf("bcm_field_qset_data_qualifier_add %x\n", result);
        return result;
    }
    /* create field group */
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create with Err %x\n", result);
        return result;
    }
 
    BCM_FIELD_ASET_INIT(aset); 
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassSourceSet); 
 
    /* Attached the action to the field group */ 
    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set Err %x\n", result);
        return result;
    }

    for (i = 0; i < entry_num; i++) {
        /*create an entry*/
        result = bcm_field_entry_create(unit, grp.group, &action_entry);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_entry_create Err %x\n", result);
            return result;
        }
        /* Fill the key and mask for the data qualifier. */
        data[0] = (dest_vtep_info_ptr[i] >> 16) & 0xFF;
        data[1] = (dest_vtep_info_ptr[i] >> 8) & 0xFF;
        data[2] = dest_vtep_info_ptr[i] & 0xFF;
        mask[0] = 0x00;
        mask[1] = 0x3F;
        mask[2] = 0xFF;
     
        result = bcm_field_qualify_data(unit, action_entry, data_qual.qual_id, &data, &mask, 3);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_data\n");
            return result;
        }
        result = bcm_field_action_add(unit, action_entry, bcmFieldActionClassDestSet, udh0[i], 0);
        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_action_add User-Header0:%x\n", udh0[i]);
            return result;
        }
     
        result = bcm_field_action_add(unit, action_entry, bcmFieldActionClassSourceSet, udh1[i], 0);
        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_action_add User-Header1:%x\n", udh1[i]);
            return result;
        }
    }
    
    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
       return result;
    }

    return result;
} 


