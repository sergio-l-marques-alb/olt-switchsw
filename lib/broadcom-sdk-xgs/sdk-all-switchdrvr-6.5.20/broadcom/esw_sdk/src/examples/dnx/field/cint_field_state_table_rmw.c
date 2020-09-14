/*
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_state_table_rmw.c
 * int unit = 0;
 * bcm_field_presel_t presel_id = 50;
 * bcm_field_layer_type_t fwd_layer = bcmFieldLayerTypeEth;
 * field_state_table_rmw_main(unit, presel_id, fwd_layer);
 *
 * This is an example of configuring a state table field group in iPMF1 and using direct extraction in iPMF2
 * to collect the value read from the state table.
 * This example is for devices with atomic read modify write (J2CP and above)
 *
 */

bcm_field_group_t cint_field_state_table_rmw_fg_state_table_id = BCM_FIELD_ID_INVALID;
bcm_field_group_t cint_field_state_table_rmw_fg_de_id = BCM_FIELD_ID_INVALID;
bcm_field_context_t cint_field_state_table_rmw_context_id = BCM_FIELD_ID_INVALID;
bcm_field_qualify_t cint_field_state_table_rmw_qual_udq_zero = BCM_FIELD_ID_INVALID;
bcm_field_qualify_t cint_field_state_table_rmw_qual_udq_src = BCM_FIELD_ID_INVALID;
bcm_field_qualify_t cint_field_state_table_rmw_qual_udq_opcode = BCM_FIELD_ID_INVALID;
bcm_field_action_t cint_field_state_table_rmw_action_uda = BCM_FIELD_ID_INVALID;
bcm_field_qualify_t cint_field_state_table_rmw_address_8lsb = 5;

/**
* \brief
*  Configures the following scenario:
*  1) Creates a new State Table FG.in iPMF1
*     * Qual1-UDQ: IPV4-source (8 lsb) - 8b Argument
*     * Qual2:     TTL (8b)      - 8 lsb Address
*     * Qual3-CONST-UDQ: 0 (5b)  - 5 msb Address
*     * Qual4-CONST-UDQ: Opcode 12, write operation (5 lsb) - 5b opcode
*  2) Creates a new context for iPMF1/iPMF2
*     * State Table support
*  3) Attaches the State Table FG on the newly created context
*  4) Creates a new DE FG in iPMF2
*     * Key: StateTableData
*     * Action: InterfaceClassVPort (IN_LIF_PROFILE_0)
*  5) Attaches the DE FG on the same context
*
* \remark
*/
int field_state_table_rmw_main(int unit, bcm_field_presel_t presel_id, bcm_field_layer_type_t fwd_layer)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_entry_info_t entry_info;
    int ip_src_lsb_size_bits = 8;
    int fwd_layer_internal;
    void *dest_char;

    int rv = BCM_E_NONE;

    printf("Creating State Table FG Qualifier...\r\n");

    /* New qualifier to take 8b out of IPv4 src address*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = ip_src_lsb_size_bits;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "arg_src_ip_udq", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_state_table_rmw_qual_udq_src);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /* New qualifier to pad the 5msb of the state table address with 0.*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "address_pad_udq", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_state_table_rmw_qual_udq_zero);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /* New qualifier to write to the opcode of the modify operation.*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "opcode_udq", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_state_table_rmw_qual_udq_opcode);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /* New void action to serve as placeholder for the state table's payload.*/
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = bcmFieldStageIngressPMF1;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 8;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "st_uda", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_state_table_rmw_action_uda);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    printf("Creating a context that supports State Table...\r\n");

    bcm_field_context_info_t_init(&context_info);
    context_info.state_table_enabled = TRUE;
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "state_table_test", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_state_table_rmw_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);
    
    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_state_table_rmw_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = fwd_layer;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    fwd_layer_internal = fwd_layer;
    printf("Presel (%d) was configured context(%d) fwd_layer(%d) \n",
        presel_id, cint_field_state_table_rmw_context_id, fwd_layer_internal);

    printf("Creating State Table FG...\r\n");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeStateTable;
    fg_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "state_table_test_fg", sizeof(fg_info.name));
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = cint_field_state_table_rmw_qual_udq_src;
    fg_info.qual_types[1] = bcmFieldQualifyIp4Ttl;
    fg_info.qual_types[2] = cint_field_state_table_rmw_qual_udq_zero;
    fg_info.qual_types[3] = cint_field_state_table_rmw_qual_udq_opcode;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = cint_field_state_table_rmw_action_uda;
    fg_info.action_with_valid_bit[0] = FALSE;
    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_state_table_rmw_fg_state_table_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    printf("Attaching State Table FG to the created context...\r\n");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 128 - ip_src_lsb_size_bits; /* LSB of SIP */

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 0;

    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = 12; /* Write argument operation */

    rv = bcm_field_group_context_attach(unit, 0, cint_field_state_table_rmw_fg_state_table_id, cint_field_state_table_rmw_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /*
     * We will add an entry to have an initial value different from zero.
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0xab;
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_info.entry_qual[1].value[0] = cint_field_state_table_rmw_address_8lsb;
    entry_info.entry_qual[2].type = fg_info.qual_types[2];
    entry_info.entry_qual[2].value[0] = 0;
    entry_info.entry_qual[3].type = fg_info.qual_types[3];
    entry_info.entry_qual[3].value[0] = 0;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = 0;
    rv = bcm_field_entry_add(unit, 0, cint_field_state_table_rmw_fg_state_table_id, &entry_info, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Creating iPMF2 DE FG to execute actions on State Table result...\r\n");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "state_table_test_const_fg", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyStateTableData;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionInVportClass0;
    fg_info.action_with_valid_bit[0] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_state_table_rmw_fg_de_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
    }

    printf("Attaching DE FG to the created context...\r\n");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_state_table_rmw_fg_de_id, cint_field_state_table_rmw_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    return rv;
}

int field_state_table_rmw_destroy(int unit, bcm_field_presel_t presel_id)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;
    int rv = BCM_E_NONE;

    printf("Destroying created preselector...\r\n");
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_id.presel_id = presel_id;
    rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
    if(rv != BCM_E_NONE)

    printf("Detaching State Table FG from created context...\r\n");
    rv = bcm_field_group_context_detach(unit, cint_field_state_table_rmw_fg_state_table_id, cint_field_state_table_rmw_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_field_state_table_rmw_fg_state_table_id, cint_field_state_table_rmw_context_id);
        return rv;
    }

    printf("Detaching DE FG from created context...\r\n");
    rv = bcm_field_group_context_detach(unit, cint_field_state_table_rmw_fg_de_id, cint_field_state_table_rmw_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_field_state_table_rmw_fg_de_id, cint_field_state_table_rmw_context_id);
        return rv;
    }

    printf("Deleting State Table FG...\r\n");
    rv = bcm_field_group_delete(unit, cint_field_state_table_rmw_fg_state_table_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_field_state_table_rmw_fg_state_table_id);
        return rv;
    }

    printf("Deleting DE FG...\r\n");
    rv = bcm_field_group_delete(unit, cint_field_state_table_rmw_fg_de_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_field_state_table_rmw_fg_de_id);
        return rv;
    }

    printf("Destroying created context...\r\n");
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, cint_field_state_table_rmw_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy contex_id %d \n", rv, cint_field_state_table_rmw_context_id);
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, cint_field_state_table_rmw_qual_udq_zero);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy qualifier %d \n", rv, cint_field_state_table_rmw_qual_udq_zero);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_state_table_rmw_qual_udq_src);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy qualifier %d \n", rv, cint_field_state_table_rmw_qual_udq_src);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_state_table_rmw_qual_udq_opcode);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy qualifier %d \n", rv, cint_field_state_table_rmw_qual_udq_opcode);
        return rv;
    }

    rv = bcm_field_action_destroy(unit, cint_field_state_table_rmw_action_uda);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_destroy action %d \n", rv, cint_field_state_table_rmw_action_uda);
        return rv;
    }

    return rv;
}
