
/* global field configuration */
bcm_field_presel_t pfcdm_ipv4_hashing_presel_id = 119;
bcm_field_context_t pfcdm_ipv4_hashing_context_id = 0;

bcm_field_context_t pfcdm_udf_context_id = 0;

bcm_field_group_t   pfcdm_hash_udf_fg_id = 0;
bcm_field_group_t   pfcdm_hash_trap_code_to_udh_fg_id = 0;
bcm_field_group_t pfcdm_compare_fg_id = 0;
bcm_field_entry_t pfcdm_compare_entry_id1 = 0;
bcm_field_entry_t pfcdm_compare_entry_id2 = 0;

bcm_field_group_t  pfcdm_em_flow_fg_id = 0;
bcm_field_group_t  pfcdm_tcam_flow_fg_id = 0;

bcm_field_group_t   cint_pfcdm_hash_to_udh_base_ipmf2_const_fg_id = 0;
bcm_field_group_t   cint_pfcdm_trap_to_udh_base_ipmf2_const_fg_id = 0;

const int pfcdm_action_priority_high = 15;
const int pfcdm_action_priority_low = 7;

/* this group id is fixed */
bcm_field_group_t pfcdm_congest_mon_fg_id = 50;
 
bcm_field_group_t pfcdm_ipmf3_gen_fg_id = 8;
bcm_field_entry_t pfcdm_ipmf3_gen_entry_id1 = 0;
bcm_field_entry_t pfcdm_ipmf3_gen_entry_id2 = 0;
bcm_field_entry_t pfcdm_ipmf3_gen_entry_id3 = 0;
bcm_field_entry_t pfcdm_ipmf3_gen_entry_id4 = 0;

bcm_field_context_t pfcdm_ipmf3_gen_context_id = 0;
bcm_field_presel_t  pfcdm_ipmf3_gen_presel_id = 123;

bcm_field_context_t pfcdm_ipmf3_congest_mon_context_id = 0;
bcm_field_presel_t  pfcdm_ipmf3_congest_mon_presel_id = 124;

bcm_field_action_t pfcdm_action_drop_flag_id = 0;

pfcdm_pmf_group_action_list_t pfcdm_em_flow_action_list;
pfcdm_pmf_group_qualify_list_t pfcdm_em_flow_quailfy_list;

pfcdm_pmf_group_action_list_t pfcdm_tcam_flow_action_list;
pfcdm_pmf_group_qualify_list_t pfcdm_tcam_flow_quailfy_list;

pfcdm_pmf_group_action_list_t pfcdm_ipmf3_gen_action_list;
pfcdm_pmf_group_qualify_list_t pfcdm_ipmf3_gen_quailfy_list;

pfcdm_pmf_group_action_list_t pfcdm_congest_mon_action_list;
pfcdm_pmf_group_qualify_list_t pfcdm_congest_mon_quailfy_list;

bcm_field_qualify_t pfcdm_compare_const_qual_id = 0;

/* the LSB is 0 */
const int PFCDM_UDH_DROP_FLAG = BCM_FIELD_COLOR_BLACK; /* 0x888888; */

int pfcdm_field_ipv4_hash_ipmf1_init(int unit)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_hash_info_t hash_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
	
    bcm_field_context_compare_info_t compare_info;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;
    int rv = BCM_E_NONE;

    /*
     * Create a new context for first group, since two groups on same context can't share banks
     */
    printf("Creating new context for the hashing and compare\n");
    bcm_field_context_info_t_init(&context_info);
    context_info.hashing_enabled = TRUE;	
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &pfcdm_ipv4_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    printf("Context created %d!\n", pfcdm_ipv4_hashing_context_id);

    /*create the hash key and attach it to the context */
    bcm_field_context_hash_info_t_init(&hash_info);
    hash_info.hash_function = bcmFieldContextHashFunctionCrc16Bisync;
    hash_info.order = FALSE;
    hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
    hash_info.hash_config.action_key = bcmFieldContextHashActionKeyAdditionalLbKey;
    hash_info.key_info.nof_quals = 5;
    hash_info.key_info.qual_types[0] = bcmFieldQualifySrcIp;
    hash_info.key_info.qual_types[1] = bcmFieldQualifyDstIp;
    hash_info.key_info.qual_types[2] = bcmFieldQualifyIp4Protocol;
    hash_info.key_info.qual_types[3] = bcmFieldQualifyL4SrcPort;
    hash_info.key_info.qual_types[4] = bcmFieldQualifyL4DstPort;
    hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[0].input_arg = 1;
    hash_info.key_info.qual_info[0].offset = 0;

    hash_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[1].input_arg = 1;
    hash_info.key_info.qual_info[1].offset = 0;
    hash_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[2].input_arg = 1;
    hash_info.key_info.qual_info[2].offset = 0;
    hash_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[3].input_arg = 2;
    hash_info.key_info.qual_info[3].offset = 0;
    hash_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[4].input_arg = 2;
    hash_info.key_info.qual_info[4].offset = 0;

    rv = bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1, pfcdm_ipv4_hashing_context_id, &hash_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_hash_create\n", rv);
        return rv;
    }

    /* compare initialization, just 9 bits key */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 9;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "Comp_const", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &pfcdm_compare_const_qual_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create, pfcdm_compare_const_qual_id\n");
        return rv;
    }

	
    bcm_field_context_compare_info_t_init(&compare_info);
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = bcmFieldQualifyRxTrapCode;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    compare_info.second_key_info.nof_quals = 1;	
    compare_info.second_key_info.qual_types[0] = pfcdm_compare_const_qual_id;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    compare_info.second_key_info.qual_info[0].input_arg = PFCDM_HW_DO_NOTHING_TRAP_CODE;
    compare_info.second_key_info.qual_info[0].offset = 0;
	
    /**
     * Create the context compare mode.
     */
    rv = bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, pfcdm_ipv4_hashing_context_id, 0, &compare_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }

    p_id.presel_id = pfcdm_ipv4_hashing_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = pfcdm_ipv4_hashing_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    return rv;
}



int 
pfcdm_field_compare_stat_metata_set(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_action_info_t action_info;
    char *proc_name;
    void *dest_char;
	
    int rv = BCM_E_NONE;
    proc_name = "pfcdm_field_compare_stat_metata_ipmf2";

    /**
     * Init the fg_info structure.
     */
    bcm_field_group_info_t_init(&fg_info);
    /**
     * Fill the fg_info structure
     */
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    /**
     * Setting the qual to be bcmFieldQualifyCompareKeysResult0. So we can see the result from the compare.
     */
    fg_info.qual_types[0] = bcmFieldQualifyCompareKeysResult0;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStatMetaData;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "compare_stat_metata", sizeof(fg_info.name));

    /**
     * Create FG without ID
     */
    rv = bcm_field_group_add(unit, 0, &fg_info, &pfcdm_compare_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    /**
     * Init the attach_info.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    /**
     * Fill the attach_info structure
     */
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = bcmFieldQualifyCompareKeysResult0;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
	
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    /**
     * Attach the FG to the context.
     */
    rv = bcm_field_group_context_attach(unit, 0, pfcdm_compare_fg_id, pfcdm_ipv4_hashing_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
    /**
     * Add the first entry.
     * It will provide a signal if the compare pair is NOT equal.
     */
    /**
     * Init the entry_info structure.
     */
    bcm_field_entry_info_t_init(&entry_info);
    /**
     * Fill the entry_info structure
     */
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyCompareKeysResult0;
    entry_info.entry_qual[0].value[0] = 0x1;
    entry_info.entry_qual[0].mask[0] = 0x1;
    entry_info.entry_action[0].type = bcmFieldActionStatMetaData;
    entry_info.entry_action[0].value[0] = 0x1;
    /**
     * Add an entry to the FG
     */
    rv = bcm_field_entry_add(unit, 0, pfcdm_compare_fg_id, &entry_info, &pfcdm_compare_entry_id1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add entry_id(%d)\n", rv, pfcdm_compare_entry_id1);
        return rv;
    }
    printf("Compare entry #1 %d configured\n", pfcdm_compare_entry_id1);
	
    /**
     * Add the second entry.
     * It will provide a signal if the compare pair are equal.
     */
    /**
     * Init the entry_info structure.
     */
    bcm_field_entry_info_t_init(&entry_info);
    /**
     * Fill the entry_info structure
     */
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.priority = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyCompareKeysResult0;
    entry_info.entry_qual[0].value[0] = 0x0;
    entry_info.entry_qual[0].mask[0] = 0x1;
    entry_info.entry_action[0].type = bcmFieldActionStatMetaData;
    entry_info.entry_action[0].value[0] = 0x0;
    /**
     * Add an entry to the FG
     */
    rv = bcm_field_entry_add(unit, 0, pfcdm_compare_fg_id, &entry_info, &pfcdm_compare_entry_id2);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add entry_id(%d)\n", rv, pfcdm_compare_entry_id2);
        return rv;
    }

    printf("Compare entry #2 %d configured\n", pfcdm_compare_entry_id2);
    return rv;
}



int pfcdm_field_ipv4_flow_entry_em_add(int unit, int is_update, uint16_t hash_result, 
    uint16_t other_key, uint8_t sw_drop_reason, uint32_t stat_id)
{
    int flag = 0;
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_info_t_init(&entry_info);
	
    if (is_update)
    {
        /* flag = BCM_FIELD_FLAG_UPDATE; */
        rv = pfcdm_field_ipv4_flow_entry_em_del(unit, hash_result, other_key);
        if(rv != BCM_E_NONE)
        {
            printf("Error (%d), in update failure\n", rv);
            return rv;
        }
    }
	
    entry_info.nof_entry_quals = 2;
    entry_info.nof_entry_actions = 3;
    entry_info.entry_qual[0].type = pfcdm_em_flow_quailfy_list.qual_types[0]; /* bcmFieldQualifyHashValue; */
    entry_info.entry_qual[0].value[0] = hash_result;
    entry_info.entry_qual[1].type = pfcdm_em_flow_quailfy_list.qual_types[1]; /* pfcdm_em_flow_qual_16bits_dst_ip; */
    entry_info.entry_qual[1].value[0] = other_key;

    /* actions */
    entry_info.nof_entry_actions = 3;
    entry_info.entry_action[0].type = pfcdm_em_flow_action_list.action_types[0]; 
    entry_info.entry_action[0].value[0] = stat_id;
    entry_info.entry_action[1].type = pfcdm_em_flow_action_list.action_types[1]; 
    entry_info.entry_action[1].value[0] = 0;
    entry_info.entry_action[1].value[1] = 0;
    entry_info.entry_action[1].value[2] = PFCDM_COUNTER_TYPE_ID; /* type_id */
    entry_info.entry_action[1].value[3] = 1;
    entry_info.entry_action[2].type = pfcdm_em_flow_action_list.action_types[2]; 
    entry_info.entry_action[2].value[0] = sw_drop_reason;

    rv = bcm_field_entry_add(unit, flag, pfcdm_em_flow_fg_id, &entry_info, NULL);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
	
    return rv;
}


int pfcdm_field_ipv4_flow_entry_em_del(int unit, uint16_t hash_result, uint16_t other_key)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t entry_info;
	
    bcm_field_entry_info_t_init(&entry_info);

    entry_info.entry_qual[0].type = pfcdm_em_flow_quailfy_list.qual_types[0];;
    entry_info.entry_qual[0].value[0] = hash_result;
    entry_info.entry_qual[1].type = pfcdm_em_flow_quailfy_list.qual_types[1];;
    entry_info.entry_qual[1].value[0] = other_key;

    rv = bcm_field_entry_delete(unit, pfcdm_em_flow_fg_id, entry_info.entry_qual, 0);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
	
    return rv;
}





int pfcdm_field_ipv4_flow_entry_tcam_add(int unit, 
    int is_update,
    uint32_t sip, uint32_t dip, uint8_t protocol, uint16_t sport, uint16_t dport, 
    uint8_t sw_drop_reason, uint32_t stat_id, bcm_field_entry_t *entry_id_ptr)
{
    int flag = 0;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_handle = *entry_id_ptr;
    int rv = BCM_E_NONE;

    if (is_update)
    {
        flag = BCM_FIELD_FLAG_UPDATE;
        /* 
        rv = pfcdm_field_ipv4_flow_entry_tcam_del(unit, *entry_id_ptr);
        if(rv != BCM_E_NONE)
        {
            printf("Error (%d), in update failure\n", rv);
            return rv;
        }
        */
    }
	
    /* Add entry */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 1;

    entry_info.nof_entry_quals = 5;
    entry_info.entry_qual[0].type = bcmFieldQualifySrcIp;
    entry_info.entry_qual[0].value[0] = sip;
    entry_info.entry_qual[0].mask[0] = 0xFFFFFFFF;

    entry_info.entry_qual[1].type = bcmFieldQualifyDstIp;
    entry_info.entry_qual[1].value[0] = dip;
    entry_info.entry_qual[1].mask[0] = 0xFFFFFFFF;

    entry_info.entry_qual[2].type = bcmFieldQualifyIp4Protocol;
    entry_info.entry_qual[2].value[0] = protocol;
    entry_info.entry_qual[2].mask[0] = 0xFF;
	
    entry_info.entry_qual[3].type = bcmFieldQualifyL4SrcPort;
    entry_info.entry_qual[3].value[0] = sport;
    entry_info.entry_qual[3].mask[0] = 0xFFFF;

    entry_info.entry_qual[4].type = bcmFieldQualifyL4DstPort;
    entry_info.entry_qual[4].value[0] = dport;
    entry_info.entry_qual[4].mask[0] = 0xFFFF;    

    /* actions */
    entry_info.nof_entry_actions = 3;
    entry_info.entry_action[0].type = pfcdm_em_flow_action_list.action_types[0]; 
    entry_info.entry_action[0].value[0] = stat_id;
    entry_info.entry_action[1].type = pfcdm_em_flow_action_list.action_types[1]; 
    entry_info.entry_action[1].value[0] = 0;
    entry_info.entry_action[1].value[1] = 0;
    entry_info.entry_action[1].value[2] = PFCDM_COUNTER_TYPE_ID; /* type_id */
    entry_info.entry_action[1].value[3] = 1;
    entry_info.entry_action[2].type = pfcdm_em_flow_action_list.action_types[2]; 
    entry_info.entry_action[2].value[0] = sw_drop_reason;

    
    rv = bcm_field_entry_add(unit, flag, pfcdm_tcam_flow_fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    *entry_id_ptr = entry_handle;
    printf("Entry add: fg 0x%x, entry 0x%x\n", pfcdm_tcam_flow_fg_id, entry_handle);
    return rv;
}


int pfcdm_field_ipv4_flow_entry_tcam_del(int unit, bcm_field_entry_t entry_id)
{
    int rv = BCM_E_NONE;
	
    rv = bcm_field_entry_delete(unit, pfcdm_tcam_flow_fg_id, NULL, entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in pfcdm_field_ipv4_flow_entry_tcam_del\n", rv);
        return rv;
    }

    return rv;
}
	


int pfcdm_field_ipmf3_congest_mon_entry_add(int unit, int out_port, int priority, bcm_field_entry_t *entry_id_ptr)
{
    bcm_field_entry_info_t ent_info;
    int rv;

    /* Add entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 200;
    printf("Adding new iPMF3 TCAM Entry Id for packet snoop\n");

    ent_info.nof_entry_quals = 2;

    ent_info.entry_qual[0].type = bcmFieldQualifyContainer;
    ent_info.entry_qual[0].value[0] = 0; /* miss in flow table */
    ent_info.entry_qual[0].mask[0] = 0xFFFFFFFF;

/*
    ent_info.entry_qual[1].type = bcmFieldQualifyDstPort;
    ent_info.entry_qual[1].value[0] = out_port;
    ent_info.entry_qual[1].mask[0] = 0x1FFFFF;
*/

    ent_info.entry_qual[1].type = bcmFieldQualifyIntPriority;
    ent_info.entry_qual[1].value[0] = priority;
    ent_info.entry_qual[1].mask[0] = 0x7;

    /* just for trap code == 9 packets. */
    ent_info.entry_qual[2].type = bcmFieldQualifyRxTrapCode;
    ent_info.entry_qual[2].value[0] = bcmRxTrapForwardingIpv4Ttl0_hw_code;
/*    ent_info.entry_qual[3].value[0] = PFCDM_HW_DO_NOTHING_TRAP_CODE;  ... */
    ent_info.entry_qual[2].mask[0] = 0x1FF;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionMirrorIngress; /* bcmFieldActionSnoop; */
    ent_info.entry_action[0].value[0] = pfcdm_snoop_gport_id;

    rv = bcm_field_entry_add(unit, 0, pfcdm_congest_mon_fg_id, &ent_info, entry_id_ptr);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("iPMF3 TCAM Entry Id (%d) for packet snoop\n", *entry_id_ptr);

    return rv;
}


int pfcdm_field_ipmf3_congest_mon_entry_del(int unit, bcm_field_entry_t entry_id)
{
    int rv;
    rv = bcm_field_entry_delete(unit, pfcdm_congest_mon_fg_id, NULL, entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in pfcdm_field_ipmf3_congest_mon_entry_del\n", rv);
        return rv;
    }

    return rv;
}
	

void pfcdm_group_and_attach_info_set(
    pfcdm_pmf_group_qualify_list_t *qualfiy_list_ptr,
    pfcdm_pmf_group_action_list_t *action_list_ptr,
    bcm_field_group_info_t *fp_info_ptr, 
    bcm_field_group_attach_info_t *attach_ptr)
{
    int i = 0;

    /* qualify list */
    fp_info_ptr->nof_quals = qualfiy_list_ptr->nof_quals;
    attach_ptr->key_info.nof_quals = qualfiy_list_ptr->nof_quals;
    for (i = 0; i < qualfiy_list_ptr->nof_quals; i ++)
    {
        fp_info_ptr->qual_types[i] = qualfiy_list_ptr->qual_types[i];
        attach_ptr->key_info.qual_types[i] = qualfiy_list_ptr->qual_types[i];
        attach_ptr->key_info.qual_info[i].input_type = qualfiy_list_ptr->input_type[i];
        attach_ptr->key_info.qual_info[i].input_arg = qualfiy_list_ptr->input_arg[i];
        attach_ptr->key_info.qual_info[i].offset = qualfiy_list_ptr->offset[i];
    }

    /* action list */
    fp_info_ptr->nof_actions = action_list_ptr->nof_actions;
    attach_ptr->payload_info.nof_actions = action_list_ptr->nof_actions;
    for (i = 0; i < action_list_ptr->nof_actions; i ++)
    {
        fp_info_ptr->action_types[i] = action_list_ptr->action_types[i];
		attach_ptr->payload_info.action_types[i] = action_list_ptr->action_types[i];

        if (action_list_ptr->action_pri_en)
        {
            attach_ptr->payload_info.action_info[i].priority = action_list_ptr->action_info_pri;
        }
    }
}



int pfcdm_field_ipv4_ipmf2_em_init(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualifier_info_get_t qual_info_get;

    bcm_field_action_info_t action_info;
    void *dest_char;

    bcm_field_qualify_t pfcdm_em_flow_qual_16bits_dst_ip = 0;
    bcm_field_action_t pfcdm_flow_action_to_pfm3 = 0;

    int ii;
    int rv = BCM_E_NONE;
    int command_id = 9;
    int action_info_pri = 0;
    int dest_ip_offset = 0;

    /* qualifier for lower 16 bits of DIP */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 16;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &pfcdm_em_flow_qual_16bits_dst_ip);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create, action_prefix_qual_zero\n", rv);
        return rv;
    }

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionContainer;
    action_info.prefix_size = 24;
    action_info.prefix_value = 0;
    action_info.size = 8;
    action_info.stage = bcmFieldStageIngressPMF2;
    rv = bcm_field_action_create(unit, 0, &action_info, &pfcdm_flow_action_to_pfm3);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create trap_action\n", rv);
        return rv;
    }

    /* Note we can use bcm_field_qualifier_info_create_t_init before bcm_field_qualifier_info_get.*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyDstIp, bcmFieldStageIngressPMF2, &qual_info_get);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
	
    dest_ip_offset = qual_info_get.offset;
	
    /* priority */
    action_info_pri = BCM_FIELD_ACTION_PRIORITY(0, pfcdm_action_priority_low);
	
    /* generate the qualify list */
    sal_memset(&pfcdm_em_flow_quailfy_list, 0, sizeof(pfcdm_em_flow_quailfy_list));
    pfcdm_em_flow_quailfy_list.nof_quals = 2;
    pfcdm_em_flow_quailfy_list.qual_types[0] = bcmFieldQualifyHashValue; /* hash value */
    pfcdm_em_flow_quailfy_list.input_type[0] = bcmFieldInputTypeMetaData;
    pfcdm_em_flow_quailfy_list.qual_types[1] = pfcdm_em_flow_qual_16bits_dst_ip; /* Lower 16bits of DIP */	
    pfcdm_em_flow_quailfy_list.input_type[1] = bcmFieldInputTypeLayerAbsolute;
    pfcdm_em_flow_quailfy_list.input_arg[1] = 1;

    /** We take the 16 lsb, but header offsets are from the MSB "*/
    pfcdm_em_flow_quailfy_list.offset[1] = dest_ip_offset+(32-16);
	
    /* generate the action list */
    sal_memset(&pfcdm_em_flow_action_list, 0, sizeof(pfcdm_em_flow_action_list));
    pfcdm_em_flow_action_list.nof_actions = 3;
    pfcdm_em_flow_action_list.action_pri_en = 1;
    pfcdm_em_flow_action_list.action_info_pri = action_info_pri;
    pfcdm_em_flow_action_list.action_types[0] = (bcmFieldActionStatId0 + command_id);
    pfcdm_em_flow_action_list.action_types[1] = (bcmFieldActionStatProfile0 + command_id);
    pfcdm_em_flow_action_list.action_types[2] = pfcdm_flow_action_to_pfm3;
	
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeExactMatch;
    fg_info.stage = bcmFieldStageIngressPMF2;

    bcm_field_group_attach_info_t_init(&attach_info);

    /* generate group info and attach info*/
    pfcdm_group_and_attach_info_set(&pfcdm_em_flow_quailfy_list, &pfcdm_em_flow_action_list, &fg_info, &attach_info);

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ExEm_Flow_Table", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &pfcdm_em_flow_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_add\n", rv);
        return rv;
    }
 
    rv = bcm_field_group_context_attach(unit, 0, pfcdm_em_flow_fg_id, pfcdm_ipv4_hashing_context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_context_attach\n", rv);
        return rv;
    }
 
    printf ("EM group %d is created.\n", pfcdm_em_flow_fg_id);

    return rv;
}



int pfcdm_field_ipv4_ipmf2_tcam_init(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_action_info_t action_info;
	void *dest_char;
	
    int ii = 0;
    int rv = BCM_E_NONE;
    int command_id = 8;
    int action_info_pri = 0;

    action_info_pri = BCM_FIELD_ACTION_PRIORITY(0, pfcdm_action_priority_high);
	
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
	
    fg_info.fg_type = bcmFieldGroupTypeTcam;

    /**Set which TCAM banks to access*/
    fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
    fg_info.tcam_info.nof_tcam_banks = 1;

    /**Select a big bank, 2K entries */
    fg_info.tcam_info.tcam_bank_ids[0] = 11;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /* quailfy */
    sal_memset(&pfcdm_tcam_flow_quailfy_list, 0, sizeof(pfcdm_tcam_flow_quailfy_list));
    pfcdm_tcam_flow_quailfy_list.nof_quals = 5;
    pfcdm_tcam_flow_quailfy_list.qual_types[0] = bcmFieldQualifySrcIp;
    pfcdm_tcam_flow_quailfy_list.qual_types[1] = bcmFieldQualifyDstIp;
    pfcdm_tcam_flow_quailfy_list.qual_types[2] = bcmFieldQualifyIp4Protocol;
    pfcdm_tcam_flow_quailfy_list.qual_types[3] = bcmFieldQualifyL4SrcPort;
    pfcdm_tcam_flow_quailfy_list.qual_types[4] = bcmFieldQualifyL4DstPort;	
	
    pfcdm_tcam_flow_quailfy_list.input_type[0] = bcmFieldInputTypeLayerAbsolute;
    pfcdm_tcam_flow_quailfy_list.input_arg[0] = 1; 
    pfcdm_tcam_flow_quailfy_list.offset[0] = 0;

    pfcdm_tcam_flow_quailfy_list.input_type[1] = bcmFieldInputTypeLayerAbsolute;
    pfcdm_tcam_flow_quailfy_list.input_arg[1] = 1; 
    pfcdm_tcam_flow_quailfy_list.offset[1] = 0;
	
    pfcdm_tcam_flow_quailfy_list.input_type[2] = bcmFieldInputTypeLayerAbsolute;
    pfcdm_tcam_flow_quailfy_list.input_arg[2] = 1; 
    pfcdm_tcam_flow_quailfy_list.offset[2] = 0;

    pfcdm_tcam_flow_quailfy_list.input_type[3] = bcmFieldInputTypeLayerAbsolute;
    pfcdm_tcam_flow_quailfy_list.input_arg[3] = 2; 
    pfcdm_tcam_flow_quailfy_list.offset[3] = 0;

    pfcdm_tcam_flow_quailfy_list.input_type[4] = bcmFieldInputTypeLayerAbsolute;
    pfcdm_tcam_flow_quailfy_list.input_arg[4] = 2; 
    pfcdm_tcam_flow_quailfy_list.offset[4] = 0;

    /* copy Ex-Em flow action list, but have the high priority  */
    sal_memset(&pfcdm_tcam_flow_action_list, 0, sizeof(pfcdm_tcam_flow_action_list));
    pfcdm_tcam_flow_action_list.nof_actions = pfcdm_em_flow_action_list.nof_actions;
    pfcdm_tcam_flow_action_list.action_pri_en = pfcdm_em_flow_action_list.action_pri_en;
    pfcdm_tcam_flow_action_list.action_info_pri = action_info_pri;
    for (ii = 0; ii < pfcdm_tcam_flow_action_list.nof_actions; ii ++)
    {
        pfcdm_tcam_flow_action_list.action_types[ii] = pfcdm_em_flow_action_list.action_types[ii];
    }

    /* generate group info and attach info*/
    pfcdm_group_and_attach_info_set(&pfcdm_tcam_flow_quailfy_list, &pfcdm_tcam_flow_action_list, &fg_info, &attach_info);
	
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "TCAM_Flow_Table", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &pfcdm_tcam_flow_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_add\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_attach(unit, 0, pfcdm_tcam_flow_fg_id, pfcdm_ipv4_hashing_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf ("Flow TCAM group %d is created.\n", pfcdm_tcam_flow_fg_id);
    return rv;
}



int pfcdm_field_hash_key_to_udh_data_init(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_action_info_t user_action_info;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_action_t udf_data_action_id = 0;
    bcm_field_action_t udf_base_action_id = 0;

    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int ii = 0;
    void *dest_char;

    int rv = BCM_E_NONE;

    /**
     * Init the fg_info structure.
     */
    bcm_field_group_info_t_init(&fg_info);
    /**
     * Fill the fg_info structure
     */
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = pfcdm_em_flow_quailfy_list.qual_types[0];
    fg_info.qual_types[1] = pfcdm_em_flow_quailfy_list.qual_types[1];

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionUDHData3;
    fg_info.action_with_valid_bit[0] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "DE_UDH_Flow_Table", sizeof(fg_info.name));
/*
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = udf_data_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    fg_info.action_types[1] = udf_base_action_id;
    fg_info.action_with_valid_bit[1] = FALSE;
*/	

    /**
     * Add the FG.
     */

    rv = bcm_field_group_add(unit, 0, &fg_info, &pfcdm_hash_udf_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }


    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = pfcdm_em_flow_quailfy_list.qual_types[ii];
        attach_info.key_info.qual_info[ii].input_type = pfcdm_em_flow_quailfy_list.input_type[ii];
		attach_info.key_info.qual_info[ii].input_arg = pfcdm_em_flow_quailfy_list.input_arg[ii];
        attach_info.key_info.qual_info[ii].offset = pfcdm_em_flow_quailfy_list.offset[ii];
    }

    for(ii=0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    rv = bcm_field_group_context_attach(unit, 0, pfcdm_hash_udf_fg_id, pfcdm_ipv4_hashing_context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    return rv;	
}





int pfcdm_field_udh_base_ipmf2_init(int unit, bcm_field_action_t action_type, bcm_field_group_t *fg_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    void *dest_char;

    bcm_field_context_t context_id;
	int action_value = 1;
    bcm_field_stage_t stage = bcmFieldStageIngressPMF2;
    bcm_field_action_t  cint_const_action = 0;

    int rv = BCM_E_NONE;

    rv = bcm_field_action_info_get(unit, action_type, stage,&action_info_get_size);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_info_get cint_const_action\n", rv);
        return rv;
    }

    /*
     * Create a user define action with size 0, value is fixed
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = action_type;
    action_info.prefix_size = action_info_get_size.size;
    action_info.prefix_value = action_value;
    action_info.size = 0;
    action_info.stage = stage;
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_const_action);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create cint_const_action\n", rv);
        return rv;
    }


    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = stage;

    /* Constant FG : number of qualifiers in key must be 0 (empty key) */
    fg_info.nof_quals = 0;

    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = cint_const_action;
    fg_info.action_with_valid_bit[0] = FALSE;


    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "udh_base_ipmf2_const", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    printf("FG created ID: %d \n", *fg_id);

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, *fg_id, pfcdm_ipv4_hashing_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    return 0;
}





int pfcdm_field_hash_key_to_udh_base_init(int unit)
{
    int rv = BCM_E_NONE;
	
    rv = pfcdm_field_udh_base_ipmf2_init(unit, bcmFieldActionUDHBase3, &cint_pfcdm_hash_to_udh_base_ipmf2_const_fg_id);
    return rv;
}



int pfcdm_field_trap_code_to_udh_data_init(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_action_info_t user_action_info;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_action_t udf_data_action_id = 0;
    bcm_field_action_t udf_base_action_id = 0;

    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
	
    bcm_field_stage_t stage = bcmFieldStageIngressPMF2;
    bcm_field_action_t action_type = bcmFieldActionUDHData2;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void *dest_char;
    int ii = 0;
	
    int rv = BCM_E_NONE;

    rv = bcm_field_action_info_get(unit, action_type, stage, &action_info_get_size);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_info_get udf_data_action_id\n", rv);
        return rv;
    }

    /*
     * Create a user define action with size 0, value is fixed
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = action_type;
    action_info.prefix_size = action_info_get_size.size - 9;
    action_info.prefix_value = 0;
    action_info.size = 9;
    action_info.stage = stage;
    rv = bcm_field_action_create(unit, 0, &action_info, &udf_data_action_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create udf_data_action_id\n", rv);
        return rv;
    }

	
    /**
     * Init the fg_info structure.
     */
    bcm_field_group_info_t_init(&fg_info);
    /**
     * Fill the fg_info structure
     */
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "trap_code_to_udh", sizeof(fg_info.name));

    fg_info.stage = stage;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyRxTrapCode; /* 9 bits */

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = udf_data_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;

    /**
     * Add the FG.
     */
    rv = bcm_field_group_add(unit, 0, &fg_info, &pfcdm_hash_trap_code_to_udh_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }


    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    for(ii=0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    rv = bcm_field_group_context_attach(unit, 0, pfcdm_hash_trap_code_to_udh_fg_id, pfcdm_ipv4_hashing_context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    return rv;	
}



int pfcdm_field_trap_code_to_udh_base_init(int unit)
{
    int rv = BCM_E_NONE;
	
    rv = pfcdm_field_udh_base_ipmf2_init(unit, bcmFieldActionUDHBase2, &cint_pfcdm_trap_to_udh_base_ipmf2_const_fg_id);
    return rv;
}


int pfcdm_field_egress_discard_to_udh_base_init(int unit)
{
    int rv = BCM_E_NONE;
	
    rv = pfcdm_field_udh_base_ipmf2_init(unit, bcmFieldActionUDHBase1);
    return rv;
}


int pfcdm_field_ipmf3_congest_mon_group_init(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void *dest_char;

    
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &pfcdm_ipmf3_congest_mon_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
    
    /**
     * Init the fg_info structure.
     */
    bcm_field_group_info_t_init(&fg_info);

    /**Set which TCAM banks to access*/
    fg_info.tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeSelect;
    fg_info.tcam_info.nof_tcam_banks = 1;

    /**Select a small bank, 256 entries. If half-wide is used, totally 512 entries */
    fg_info.tcam_info.tcam_bank_ids[0] = 15;
	
    fg_info.stage = bcmFieldStageIngressPMF3;
    fg_info.fg_type = bcmFieldGroupTypeTcam;

/*    fg_info.nof_quals = 4;*/
    fg_info.nof_quals = 3;

    /**Result of hit/miss in iPMF2*/
    fg_info.qual_types[0] = bcmFieldQualifyContainer;
/*    fg_info.qual_types[1] = bcmFieldQualifyDstPort;*/
    fg_info.qual_types[1] = bcmFieldQualifyIntPriority;
    fg_info.qual_types[2] = bcmFieldQualifyRxTrapCode;
    
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionMirrorIngress; /* bcmFieldActionSnoop; */

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Congest_Mon_Table", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_WITH_ID, &fg_info, &pfcdm_congest_mon_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
	
    /**Attach the context**/
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
/*    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];*/
	
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
/*    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;*/
    	
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];


    rv = bcm_field_group_context_attach(unit, 0, pfcdm_congest_mon_fg_id, pfcdm_ipmf3_gen_context_id /*pfcdm_ipmf3_congest_mon_context_id */, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached Congestion Monitor FG (%d) to context (%d)\n",pfcdm_congest_mon_fg_id, pfcdm_ipmf3_gen_context_id/*pfcdm_ipmf3_congest_mon_context_id*/);

/*    
    p_id.presel_id = pfcdm_ipmf3_congest_mon_presel_id;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = pfcdm_ipmf3_congest_mon_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = 0;
    p_data.qual_data[0].qual_mask = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }
*/
    return rv;
}



int pfcdm_field_action_drop_flag_create(int unit)
{
    bcm_field_action_info_t action_info;
    uint32 action_id=0;
    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionUDHData1;
    action_info.prefix_size = 31;
    /* set drop flag */
    action_info.prefix_value = (PFCDM_UDH_DROP_FLAG >> 1) & 0x7FFFFFFF;
    action_info.size = 1;
    action_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "action_drop_flag", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &pfcdm_action_drop_flag_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_create action_prefix_stat_action\n");
        return rv;
    }

    return 0;   
}


int pfcdm_field_ipmf3_gen_group_init(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void *dest_char;
	
    /**Attach Ethernet context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &pfcdm_ipmf3_gen_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /*
    rv = pfcdm_field_action_drop_flag_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in pfcdm_field_action_drop_flag_create\n", rv);
        return rv;
    }
    */
	
    /* Full for test */
    pfcdm_action_drop_flag_id = bcmFieldActionDropPrecedence; /* bcmFieldActionUDHData1; */
	
    /**
     * Init the fg_info structure.
     */
    bcm_field_group_info_t_init(&fg_info);
	
    fg_info.stage = bcmFieldStageIngressPMF3;
    fg_info.fg_type = bcmFieldGroupTypeTcam;

    sal_memset(&pfcdm_ipmf3_gen_quailfy_list, 0, sizeof(pfcdm_ipmf3_gen_quailfy_list));

    pfcdm_ipmf3_gen_quailfy_list.nof_quals = 4;

    /**Result in iPMF2*/
    pfcdm_ipmf3_gen_quailfy_list.qual_types[0] = bcmFieldQualifyContainer;

    pfcdm_ipmf3_gen_quailfy_list.qual_types[1] = bcmFieldQualifyRxTrapCode;
    pfcdm_ipmf3_gen_quailfy_list.qual_types[2] = bcmFieldQualifyDstPort;
    pfcdm_ipmf3_gen_quailfy_list.qual_types[3] = bcmFieldQualifySrcPort;
    /*
    pfcdm_ipmf3_gen_quailfy_list.qual_types[4] = bcmFieldQualifySrcMac;	
    pfcdm_ipmf3_gen_quailfy_list.qual_types[5] = bcmFieldQualifyDstMac;	*/
	
    pfcdm_ipmf3_gen_quailfy_list.input_type[0] = bcmFieldInputTypeMetaData;
    pfcdm_ipmf3_gen_quailfy_list.input_type[1] = bcmFieldInputTypeMetaData;
    pfcdm_ipmf3_gen_quailfy_list.input_type[2] = bcmFieldInputTypeMetaData;
    pfcdm_ipmf3_gen_quailfy_list.input_type[3] = bcmFieldInputTypeMetaData;

    sal_memset(&pfcdm_ipmf3_gen_action_list, 0, sizeof(pfcdm_ipmf3_gen_action_list));
    pfcdm_ipmf3_gen_action_list.nof_actions = 2;

    /* Use UDH Data 1 to discard the original packet in egress PMF */
    pfcdm_ipmf3_gen_action_list.action_types[0] = pfcdm_action_drop_flag_id; 
    pfcdm_ipmf3_gen_action_list.action_types[1] = bcmFieldActionStatSampling;

    /* generate group info and attach info*/
    pfcdm_group_and_attach_info_set(&pfcdm_ipmf3_gen_quailfy_list, &pfcdm_ipmf3_gen_action_list, &fg_info, &attach_info);
	
	
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "iPMF3_General", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &pfcdm_ipmf3_gen_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    
    rv = bcm_field_group_context_attach(unit, 0, pfcdm_ipmf3_gen_fg_id, pfcdm_ipmf3_gen_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached IPMF3 Gen FG (%d) to context (%d)\n",pfcdm_ipmf3_gen_fg_id, pfcdm_ipmf3_gen_context_id);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = pfcdm_ipmf3_gen_presel_id;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = pfcdm_ipmf3_gen_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = pfcdm_ipv4_hashing_context_id;
    p_data.qual_data[0].qual_mask = 0x3F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF3) context(%d)\n",
        pfcdm_ipmf3_gen_presel_id, pfcdm_ipmf3_gen_context_id);

    return rv;
}

int pfcdef_field_ipmf3_pp_drop_entry_install(int unit, uint16_t hw_code, uint8_t sw_code, bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;

    int rv = BCM_E_NONE;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 199;
	
    ent_info.nof_entry_quals = 2;

    ent_info.entry_qual[0].type = bcmFieldQualifyContainer;
    ent_info.entry_qual[0].value[0] = sw_code;
    ent_info.entry_qual[0].mask[0] = 0xFF;
	
    ent_info.entry_qual[1].type = bcmFieldQualifyRxTrapCode;
    ent_info.entry_qual[1].value[0] = hw_code;
    ent_info.entry_qual[1].mask[0] = 0x1FF;

    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = pfcdm_action_drop_flag_id; 
    ent_info.entry_action[0].value[0] = PFCDM_UDH_DROP_FLAG;  
	
    ent_info.entry_action[1].type = bcmFieldActionStatSampling;
    ent_info.entry_action[1].value[0] = sampling_mirror_destination;
    ent_info.entry_action[1].value[1] = 0x22; 
	
    rv = bcm_field_entry_add(unit, 0, pfcdm_ipmf3_gen_fg_id, &ent_info, entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }

    printf("iPMF3 TCAM Entry Id (0x%0x) for drop filtering (%d)\n", *entry_handle, sw_code);
    return rv;
}


int pfcdm_field_ipmf3_entry_init(int unit)
{
    bcm_field_entry_info_t ent_info;
    int cpu_gport = 0;
    int rv = BCM_E_NONE;

    /* Entry #1: monitor control to avoid the loop */
/*
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 200;
	
    ent_info.nof_entry_quals = 2;

	BCM_GPORT_LOCAL_SET(cpu_gport, local_host_cpu);
	
    ent_info.entry_qual[0].type = bcmFieldQualifyDstPort;
    ent_info.entry_qual[0].value[0] = cpu_gport;
    ent_info.entry_qual[0].mask[0] = 0x1FFFFF;
	
    ent_info.entry_qual[1].type = bcmFieldQualifySrcPort;
    ent_info.entry_qual[1].value[0] = cpu_gport;
    ent_info.entry_qual[1].mask[0] = 0x1FFFFF;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = pfcdm_action_drop_flag_id;
    ent_info.entry_action[0].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, pfcdm_ipmf3_gen_fg_id, &ent_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("iPMF3 TCAM Entry Id (%d) to avoid the loop\n", entry_handle);
*/

    /* Entry #2, 3: PP Drop filter */
    /* Entry #2, 3: PP Drop filter */
    pfcdef_field_ipmf3_pp_drop_entry_install(unit, bcmRxTrapForwardingIpv4Ttl0_hw_code, bcmRxTrapForwardingIpv4Ttl0_sw_drop_reason, &pfcdm_ipmf3_gen_entry_id1);

    pfcdef_field_ipmf3_pp_drop_entry_install(unit, bcmRxTrapUcLooseRpfFail_hw_code, bcmRxTrapUcLooseRpfFail_sw_drop_reason, &pfcdm_ipmf3_gen_entry_id2);

    pfcdef_field_ipmf3_pp_drop_entry_install(unit, bcmRxTrapUnknowDest_hw_code, bcmRxTrapUnknowDest_sw_drop_reason, &pfcdm_ipmf3_gen_entry_id3);
	
   /* Entry #4: PP Drop filter for congestion sampling */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 198;
	
    ent_info.nof_entry_quals = 2;

    ent_info.entry_qual[0].type = bcmFieldQualifyContainer;
    ent_info.entry_qual[0].value[0] = pfcdm_congest_sw_drop_reason;
    ent_info.entry_qual[0].mask[0] = 0xFF;
    ent_info.entry_qual[1].type = bcmFieldQualifyRxTrapCode;
    ent_info.entry_qual[1].value[0] = PFCDM_HW_DO_NOTHING_TRAP_CODE;
    ent_info.entry_qual[1].mask[0] = 0x1FF;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionStatSampling;
    ent_info.entry_action[0].value[0] = sampling_mirror_destination_2;
    ent_info.entry_action[0].value[1] = 0x33;
    rv = bcm_field_entry_add(unit, 0, pfcdm_ipmf3_gen_fg_id, &ent_info, &pfcdm_ipmf3_gen_entry_id4);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("iPMF3 TCAM Entry Id (0x%0x) for congestion sampling.\n", pfcdm_ipmf3_gen_entry_id4);

    return 0;
}




bcm_field_group_t pfcdm_egress_fg_id = 0;
	
bcm_field_context_t pfcdm_egress_context_id = 0;
	
int pfcdm_field_epmf_group_init(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void *dest_char;

/*
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &pfcdm_egress_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
*/
  
    /**
     * Init the fg_info structure.
     */
    bcm_field_group_info_t_init(&fg_info);

    fg_info.stage = bcmFieldStageEgress;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 2;

    /** Discard original trap packets in some cases */
    fg_info.qual_types[0] = bcmFieldQualifyUDHData0; /*bcmFieldQualifyForwardCopy;*/
    fg_info.qual_types[1] = bcmFieldQualifyUDHData1;
    
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionDrop;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Trap Discard Table", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &pfcdm_egress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
	
    /**Attach the context**/
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1]; 
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
	
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    rv = bcm_field_group_context_attach(unit, 0, pfcdm_egress_fg_id, pfcdm_egress_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached Trap Drop FG (%d) to context (%d)\n",pfcdm_egress_fg_id, pfcdm_egress_context_id);

    return rv;
}


int pfcdm_field_epmf_entry_init(int unit)
{
    bcm_field_entry_t entry_handle;
    bcm_field_entry_info_t ent_info;
    int cpu_gport = 0;
    int rv = BCM_E_NONE;


    /* PP Drop filter */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 199;
	
    ent_info.nof_entry_quals = 2;

    ent_info.entry_qual[0].type = bcmFieldQualifyUDHData0;/*bcmFieldQualifyForwardCopy;*/
    ent_info.entry_qual[0].value[0] = 0;
    ent_info.entry_qual[0].mask[0] = 0x1;
    ent_info.entry_qual[1].type = bcmFieldQualifyUDHData1;
    ent_info.entry_qual[1].value[0] = PFCDM_UDH_DROP_FLAG;
    ent_info.entry_qual[1].mask[0] = 0xFFFFFFFF;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionDrop;
    ent_info.entry_action[0].value[0] = 1; /* 1; */ 
	
    rv = bcm_field_entry_add(unit, 0, pfcdm_egress_fg_id, &ent_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("iPMF3 TCAM Entry Id (0x%08x) for drop filtering\n", entry_handle);
    return rv;
}



int pfcdm_field_ipv4_flow_init(int unit)
{
    bcm_field_action_info_t user_action_info;
    int rv = BCM_E_NONE;
	
    /* create Hash for IPv4 5-tuples hash */
    rv = pfcdm_field_ipv4_hash_ipmf1_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_field_ipv4_hash_ipmf1_init\n");
        return rv;
    }

    /* S-EXEM3 table init  */
    rv = pfcdm_field_ipv4_ipmf2_em_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_field_ipv4_ipmf2_em_init\n");
        return rv;
    }	

    /* TCAM in iPMF2 init  */
    rv = pfcdm_field_ipv4_ipmf2_tcam_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_field_ipv4_ipmf2_tcam_init\n");
        return rv;
    }
	
    /* carry hash value into UDH header, data, configure after S-EXEM3 table init */
    rv = pfcdm_field_hash_key_to_udh_data_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_field_hash_key_to_udh_data_init\n");
        return rv;
    }

    /* carry hash value into UDH header, base header  */
    rv = pfcdm_field_hash_key_to_udh_base_init(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error in pfcdm_field_hash_key_to_udh_base_init\n");
       return rv;
    }

    /* carry trap code into UDH header, data, for sampling packet */
    rv = pfcdm_field_trap_code_to_udh_data_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_field_trap_code_to_udh_data_init\n");
        return rv;
    }

    /* carry trap code into UDH header, header, for sampling packet */
    rv = pfcdm_field_trap_code_to_udh_base_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_field_trap_code_to_udh_base_init\n");
        return rv;
    }

    /* set stat metata */
    rv = pfcdm_field_compare_stat_metata_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_field_compare_stat_metata_set\n");
        return rv;
    }
	
    rv = pfcdm_sampling_mirror_cfg(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error in pfcdm_sampling_mirror_cfg\n");
       return rv;
    }

    rv = pfcdm_congest_sampling_mirror_cfg(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_congest_sampling_mirror_cfg\n");
        return rv;
    }

    rv = pfcdm_field_ipmf3_gen_group_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_field_ipmf3_gen_group_init\n");
        return rv;
    }

/*  
    rv = pfcdm_ingress_mirror_gport_get(unit);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
*/
    rv = pfcdm_field_ipmf3_congest_mon_group_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_field_ipmf3_congest_mon_group_init\n");
        return rv;
    }

    rv = pfcdm_field_ipmf3_entry_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in pfcdm_field_ipmf3_entry_init\n");
        return rv;
    }

/*
    rv = pfcdm_field_egress_discard_to_udh_base_init(unit);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
*/

    /* drop packets */
/*
    rv = pfcdm_field_epmf_group_init(unit);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

    rv = pfcdm_field_epmf_entry_init(unit);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
*/

    return rv;
}

int pfcdm_ipv4_flow_cleanup(int unit)
{
    int rv;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    rv = bcm_field_group_context_detach(unit, pfcdm_compare_fg_id, pfcdm_ipv4_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, pfcdm_compare_fg_id, pfcdm_ipv4_hashing_context_id);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, pfcdm_em_flow_fg_id, pfcdm_ipv4_hashing_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, pfcdm_em_flow_fg_id, pfcdm_ipv4_hashing_context_id);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, pfcdm_tcam_flow_fg_id, pfcdm_ipv4_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, pfcdm_tcam_flow_fg_id, pfcdm_ipv4_hashing_context_id);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, pfcdm_hash_udf_fg_id, pfcdm_ipv4_hashing_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, pfcdm_hash_udf_fg_id, pfcdm_ipv4_hashing_context_id);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, cint_pfcdm_hash_to_udh_base_ipmf2_const_fg_id, pfcdm_ipv4_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_pfcdm_hash_to_udh_base_ipmf2_const_fg_id, pfcdm_ipv4_hashing_context_id);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, cint_pfcdm_trap_to_udh_base_ipmf2_const_fg_id, pfcdm_ipv4_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_pfcdm_trap_to_udh_base_ipmf2_const_fg_id, pfcdm_ipv4_hashing_context_id);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, pfcdm_hash_trap_code_to_udh_fg_id, pfcdm_ipv4_hashing_context_id);
     if(rv != BCM_E_NONE)
     {
         printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, pfcdm_hash_trap_code_to_udh_fg_id, pfcdm_ipv4_hashing_context_id);
         return rv;
     }

    rv = bcm_field_entry_delete(unit, pfcdm_compare_fg_id, NULL, pfcdm_compare_entry_id1);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, pfcdm_compare_fg_id, NULL, pfcdm_compare_entry_id2);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, pfcdm_compare_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_delete pfcdm_compare_fg_id group %d\n", rv, pfcdm_compare_fg_id);
        return rv;
    }
    rv = bcm_field_group_delete(unit, pfcdm_em_flow_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_delete pfcdm_em_flow_fg_id group %d\n", rv, pfcdm_em_flow_fg_id);
        return rv;
    }
    rv = bcm_field_group_delete(unit, pfcdm_tcam_flow_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_delete pfcdm_tcam_flow_fg_id group %d\n", rv, pfcdm_tcam_flow_fg_id);
        return rv;
    }
    rv = bcm_field_group_delete(unit, pfcdm_hash_udf_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_delete group pfcdm_hash_udf_fg_id %d\n", rv, pfcdm_hash_udf_fg_id);
        return rv;
    }
    rv = bcm_field_group_delete(unit, cint_pfcdm_hash_to_udh_base_ipmf2_const_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_delete cint_pfcdm_udh_base_ipmf2_const_fg_id group %d\n", rv, cint_pfcdm_hash_to_udh_base_ipmf2_const_fg_id);
        return rv;
    }
    rv = bcm_field_group_delete(unit, cint_pfcdm_trap_to_udh_base_ipmf2_const_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_delete cint_pfcdm_trap_to_udh_base_ipmf2_const_fg_id group %d\n", rv, cint_pfcdm_trap_to_udh_base_ipmf2_const_fg_id);
        return rv;
    }

    rv = bcm_field_group_delete(unit, pfcdm_hash_trap_code_to_udh_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_delete pfcdm_hash_trap_code_to_udh_fg_id group %d\n", rv, pfcdm_hash_trap_code_to_udh_fg_id);
        return rv;
    }

    rv = bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, pfcdm_ipv4_hashing_context_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_compare_destroy\n", rv);
        return rv;
    }

    rv = bcm_field_context_hash_destroy(unit, bcmFieldStageIngressPMF1, pfcdm_ipv4_hashing_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_hash_destroy\n", rv);
        return rv;
    }

    p_id.presel_id = pfcdm_ipv4_hashing_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = FALSE;
    p_data.context_id = pfcdm_ipv4_hashing_context_id;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, pfcdm_ipv4_hashing_context_id);
     if (rv != BCM_E_NONE)
     {
         printf("Error (%d), in bcm_field_context_destroy\n", rv);
         return rv;
     }
    return rv;

}

int pfcdm_field_ipmf3_congest_mon_group_destroy(int unit)
{
    int rv;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    rv = bcm_field_group_context_detach(unit, pfcdm_congest_mon_fg_id, pfcdm_ipmf3_gen_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, pfcdm_congest_mon_fg_id, pfcdm_ipmf3_gen_context_id);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, pfcdm_ipmf3_gen_fg_id, NULL, pfcdm_ipmf3_gen_entry_id1);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, pfcdm_ipmf3_gen_fg_id, NULL, pfcdm_ipmf3_gen_entry_id2);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, pfcdm_ipmf3_gen_fg_id, NULL, pfcdm_ipmf3_gen_entry_id3);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, pfcdm_ipmf3_gen_fg_id, NULL, pfcdm_ipmf3_gen_entry_id4);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, pfcdm_congest_mon_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete pfcdm_congest_mon_fg_id %d\n", rv, pfcdm_congest_mon_fg_id);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, pfcdm_ipmf3_gen_fg_id, pfcdm_ipmf3_gen_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, pfcdm_ipmf3_gen_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_destroy pfcdm_ipmf3_gen_fg_id %d\n", rv, pfcdm_ipmf3_gen_fg_id);
        return rv;
    }

    p_id.presel_id = pfcdm_ipmf3_gen_presel_id;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = FALSE;
    p_data.context_id = pfcdm_ipmf3_gen_context_id;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF3, pfcdm_ipmf3_gen_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy pfcdm_ipmf3_gen_context_id %d\n", rv, pfcdm_ipmf3_gen_context_id);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF3, pfcdm_ipmf3_congest_mon_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy pfcdm_ipmf3_congest_mon_context_id %d\n", rv, pfcdm_ipmf3_congest_mon_context_id);
        return rv;
    }
    return rv;
}
