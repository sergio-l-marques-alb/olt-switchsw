/*
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_state_table.c
 * int unit = 0;
 * bcm_field_presel_t presel_id = 50;
 * bcm_field_layer_type_t fwd_layer = bcmFieldLayerTypeEth;
 * field_state_table_main(unit, presel_id, fwd_layer);
 *
 * Configuration example end
 *
 */

bcm_field_group_t state_table_fg_state_table_id = 0;
bcm_field_group_t state_table_fg_de_id = 0;
bcm_field_context_t state_table_context_id = 0;

/**
* \brief
*  Configures the following scenario:
*  1) Creates a new State Table FG.in iPMF1
*     * Qual1:     TTL (8b)      - 8 lsb Address
*     * Qual2-UDF: IPV4 (11 lsb) - 2 msb Address + 8b Data + 1b Do-write
*  2) Creates a new context for iPMF1/iPMF2
*     * State Table support
*  3) Attaches the State Table FG on the newly created context
*  4) Creates a new DE FG in iPMF2
*     * Key: StateTableData
*     * Action: InterfaceClassVPort (IN_LIF_PROFILE_0)
*  5) Attaches the DE FG on the same context
*
* \remark
*  The TCL test for this cint sends two packets:
*    1) First packet has TTL and IPV4 values that triggers a write operation (do-write=1) with data=0xc0 to address 0x5
*    2) Second packet has TTL and IPV4 values that trigger only a read operatoin (do-write=0) from address 0x5
*  We expect that data read after the second packet is sent reaches the DE FG and therefore updating the
*  IN_LIF_PROFILE_0 to 0xc0 value
*/
int field_state_table_main(int unit, bcm_field_presel_t presel_id, bcm_field_layer_type_t fwd_layer)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t qual_id;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    int fwd_layer_internal;

    int rv = BCM_E_NONE;

    printf("Creating State Table FG Qualifier...\r\n");

    /* New qualifier to take 11b out of IPv4 src address*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 11;

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    printf("Creating context that supports State Table...\r\n");

    bcm_field_context_info_t_init(&context_info);
    context_info.state_table_enabled = TRUE;
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &state_table_context_id);
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
    p_data.context_id = state_table_context_id;
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
        presel_id, state_table_context_id, fwd_layer_internal);

    printf("Creating State Table FG...\r\n");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeStateTable;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyIp4Ttl;
    fg_info.qual_types[1] = qual_id;

    rv = bcm_field_group_add(unit, 0, &fg_info, &state_table_fg_state_table_id);
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

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 117; /* SIP */

    rv = bcm_field_group_context_attach(unit, 0, state_table_fg_state_table_id, state_table_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    /*
     * We don't add entries since the test first sends a packet that has do-write as 1
     * therefore changing the data of the state table, then sends another packet that
     * only reads the data in state table in order to be parsed by the iPMF2 FG that
     * we're gonna add next
     */

    printf("Creating iPMF2 DE FG to execute actions on State Table result...\r\n");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyStateTableData;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionInVportClass0;
    fg_info.action_with_valid_bit[0] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &state_table_fg_de_id);
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

    rv = bcm_field_group_context_attach(unit, 0, state_table_fg_de_id, state_table_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    return rv;
}

int field_state_table_destroy(int unit, bcm_field_presel_t presel_id)
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
    rv = bcm_field_group_context_detach(unit, state_table_fg_state_table_id, state_table_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, state_table_fg_state_table_id, state_table_context_id);
        return rv;
    }

    printf("Detaching DE FG from created context...\r\n");
    rv = bcm_field_group_context_detach(unit, state_table_fg_de_id, state_table_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, state_table_fg_de_id, state_table_context_id);
        return rv;
    }

    printf("Deleting State Table FG...\r\n");
    rv = bcm_field_group_delete(unit, state_table_fg_state_table_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, state_table_fg_state_table_id);
        return rv;
    }

    printf("Deleting DE FG...\r\n");
    rv = bcm_field_group_delete(unit, state_table_fg_de_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, state_table_fg_de_id);
        return rv;
    }

    printf("Destroying created context...\r\n");
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, state_table_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy contex_id %d \n", rv, state_table_context_id);
        return rv;
    }

    return rv;
}
