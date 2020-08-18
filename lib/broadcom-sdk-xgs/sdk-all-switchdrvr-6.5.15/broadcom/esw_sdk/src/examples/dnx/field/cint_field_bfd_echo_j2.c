 /*
 * Configuration:
 *  
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
 * cint ../../../src/examples/dnx/field/cint_field_bfd_echo.c
 * cint_field_bfd_echo_main(0,0);
 *
 *
 * 
 *    
 */


bcm_field_group_t cint_field_echo_bfd_fg_id=0;

bcm_field_qualify_t cint_field_bfd_echo_desc_qual_id;
int cint_field_echo_trap_id;



/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] context_id_bfd_echo  -  Context id to which attach the info to
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_bfd_echo_main(int unit,bcm_field_context_t context_id_bfd_echo)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t qual_id;
    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_qualifier_info_create_t_init(&qual_info);

    qual_info.size = 16;

    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "Discriminator", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_bfd_echo_desc_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = bcmFieldQualifyDstIp;
    fg_info.qual_types[1] = bcmFieldQualifyL4DstPort;
    fg_info.qual_types[2] = cint_field_bfd_echo_desc_qual_id;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionTrap;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "OAM_BFD_ECHO", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_echo_bfd_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_echo_bfd_fg_id);



    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    /**Packet assumed to be Forwarded by IPv4 Layer*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    /**Layer above IPv4 is UDP*/
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;

    /*Discriminator taken from PDU at offset 10*/
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[2].input_arg = 2;
    attach_info.key_info.qual_info[2].offset = 80;


    
    rv = bcm_field_group_context_attach(unit, 0, cint_field_echo_bfd_fg_id, context_id_bfd_echo, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",cint_field_echo_bfd_fg_id,context_id_bfd_echo);


    rv = bcm_rx_trap_type_get(unit,0, bcmRxTrapOamBfdIpv4, &cint_field_echo_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_type_get\n", rv);
        return rv;
    }

    return 0;
}


/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] dest_ip      -  Dest ip value to set in entry add
* \param [in] local_discr  -  Discriminator value to set in entry
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_bfd_echo_entry_add(int unit,uint32 dest_ip, uint16 local_discr)
{
    bcm_field_entry_info_t ent_info;
    bcm_field_entry_t cint_bfd_echo_entry_id;
    int trap_action_value = 0;
    int max_trap_str = 15;
    bcm_gport_t trap_gport;
    int rv = BCM_E_NONE;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = local_discr;
ent_info.nof_entry_quals = 3;
    ent_info.entry_qual[0].type = bcmFieldQualifyDstIp;
    ent_info.entry_qual[0].value[0] = dest_ip;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.entry_qual[1].type = bcmFieldQualifyL4DstPort;
    ent_info.entry_qual[1].value[0] = 0x0ec9;
    ent_info.entry_qual[1].mask[0] = 0xffff;

    ent_info.entry_qual[2].type = cint_field_bfd_echo_desc_qual_id;
    ent_info.entry_qual[2].value[0] = local_discr;
    ent_info.entry_qual[2].mask[0] = 0xffff;


    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionTrap;
    BCM_GPORT_TRAP_SET(trap_gport, cint_field_echo_trap_id, max_trap_str, 0);
    /*trap_action_value = (local_discr << 13) + (max_trap_str << 9) + cint_field_echo_trap_id;*/
    
    ent_info.entry_action[0].value[0] = trap_gport;
    ent_info.entry_action[0].value[1] = local_discr;

    rv = bcm_field_entry_add(unit, 0, cint_field_echo_bfd_fg_id, &ent_info, &(cint_bfd_echo_entry_id));
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  dest_ip:(0x%x) L4Port:(%d) Desc(%d) Trap_Action (%d) \n",cint_bfd_echo_entry_id,
                                                                ent_info.entry_qual[0].value[0],
                                                                ent_info.entry_qual[1].value[0],
                                                                ent_info.entry_qual[2].value[0],
                                                                ent_info.entry_action[0].value[0] );

    return 0;
}
