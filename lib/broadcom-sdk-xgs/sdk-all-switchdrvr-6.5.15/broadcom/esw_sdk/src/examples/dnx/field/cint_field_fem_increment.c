 /*
 * Configuration:
 *  
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
 * cint ../../../src/examples/dnx/field/cint_field_fem_increment_index.c
 *
 * TRAP_BFD_O_IPV4 it the egress parsing index should be Fwd+2    trap_id = 227      
 */

int TRAP_BFD_O_IPV4_TRAP_CODE = 227;
bcm_field_group_t ipmf2_fg = 0;
bcm_field_context_t ipmf2_context_id=0;

bcm_field_presel_entry_id_t ipmf1_p_id;
/* bcm_field_fem_id_t fem_id = 0; #### REMOVE */
bcm_field_action_priority_t action_priority = {0, 0};


int field_fem_increment_index_config_ipmf1_presel(int unit)
{
    bcm_field_presel_entry_data_t ipmf1_p_data;    
    int rv=0;

    bcm_field_presel_entry_id_info_init(&ipmf1_p_id);
    bcm_field_presel_entry_data_info_init(&ipmf1_p_data);
    
    ipmf1_p_id.presel_id = 2;
    ipmf1_p_id.stage = bcmFieldStageIngressPMF1;
    ipmf1_p_data.entry_valid = TRUE;
    ipmf1_p_data.context_id = ipmf2_context_id;
    ipmf1_p_data.nof_qualifiers = 1;

    ipmf1_p_data.qual_data[0].qual_type = bcmFieldQualifyRxTrapCode;
    ipmf1_p_data.qual_data[0].qual_value = TRAP_BFD_O_IPV4_TRAP_CODE;
    ipmf1_p_data.qual_data[0].qual_mask = 0x1FF;

    rv = bcm_field_presel_set(unit, 0, &ipmf1_p_id, &ipmf1_p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set ipmf1 \n", rv);
        return rv;
    }
    return 0;
}

int field_fem_increment_index_config_ipmf2_de(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_field_fem_action_info_t  fem_action_info;
    int ii = 0 ;

    proc_name = "field_fem_increment_index_config_ipmf2_de";
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &ipmf2_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }


    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyForwardingLayerIndex;

    /* Set actions */
    fg_info.nof_actions = 0;
  
    fg_info.action_types[0] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_with_valid_bit[0] = FALSE

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &ipmf2_fg);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_add ipmf2_fg\n", proc_name, rv);
        return rv;
    }
    /**FEM CONFIG*/
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;  
    fem_action_info.condition_msb = 3;
    for(ii = 0 ; ii < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionEgressForwardingIndex;
    for(ii = 0 ; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[ii].offset = ii;
    }
    fem_action_info.fem_extraction[0].increment = 2;
    
    rv = bcm_field_fem_action_add(unit,0 ,ipmf2_fg,action_priority,&fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_fem_action_add ipmf2\n", proc_name, rv);
        return rv;
    }
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for(ii=0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }


    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    
    rv = bcm_field_group_context_attach(unit, 0, ipmf2_fg, ipmf2_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_group_context_attach ipmf2\n", proc_name, rv);
        return rv;
    }
    
    return 0;
}


int field_fem_increment_index_main(int unit)
{
    int rv = 0;
    
    rv = field_fem_increment_index_config_ipmf2_de(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_fem_increment_index_config_ipmf2_de\n", rv);
        return rv;
    }
    
    rv = field_fem_increment_index_config_ipmf1_presel(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_fem_increment_index_config_ipmf1_presel\n", rv);
        return rv;
    }


    return 0;
    
}

int field_fem_increment_index_destroy(int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &ipmf1_p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set ipmf1_p_id\n");
        return rv;
    }

    rv = bcm_field_group_context_detach(unit,ipmf2_fg,ipmf2_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach ipmf2_fg,ipmf2_context_id\n");
        return rv;
    }   
  
    rv = bcm_field_group_delete(unit, ipmf2_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete ipmf2_fg\n", rv);
        return rv;
    }
 
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, ipmf2_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy\n ipmf2_context_id", rv);
        return rv;
    }
 
    
    return rv;
}
