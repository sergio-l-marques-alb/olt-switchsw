/*
 * MDB DT test functions
 */
bcm_field_entry_t Cint_field_mdb_dt_entry_handle;
bcm_field_group_t Cint_field_mdb_dt_fg_id;
bcm_field_group_info_t Cint_field_mdb_dt_fg_info;

char Cint_field_mdb_dt_rand_name=0;

/**
 * \brief
 * Creates a simple MDB DT FG, with one qualifier and one action and attach it given context
 *
 * \param [in] unit          - Device id
 * \param [in] field_stage   - The Field stage to create the FG on
 * \param [in] context_id    - Context ID to attach FG to
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_mdb_dt_fg_cfg_main(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    int qual_iter,action_iter;

    bcm_field_group_info_t_init(&Cint_field_mdb_dt_fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    Cint_field_mdb_dt_fg_info.fg_type = bcmFieldGroupTypeDirectMdb;
    Cint_field_mdb_dt_fg_info.stage = field_stage;

    Cint_field_mdb_dt_fg_info.nof_actions = 1;
    Cint_field_mdb_dt_fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    Cint_field_mdb_dt_fg_info.action_with_valid_bit[0] = TRUE;

    Cint_field_mdb_dt_fg_info.nof_quals = 1;
    Cint_field_mdb_dt_fg_info.qual_types[0] = bcmFieldQualifyIp4Ttl;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    dest_char = &(Cint_field_mdb_dt_fg_info.name[0]);
    sal_strncpy(dest_char, "MDB DT Basic1", 13);

    if (field_stage == bcmFieldStageIngressPMF2)
    {
        Cint_field_mdb_dt_fg_info.qual_types[0] = bcmFieldQualifyIp4Tos;
        Cint_field_mdb_dt_fg_info.action_types[0] = bcmFieldActionDropPrecedence;

        sal_strncpy(dest_char, "MDB DT Basic2", 13);
    }
    else if (field_stage == bcmFieldStageIngressPMF3)
    {
        Cint_field_mdb_dt_fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsAbsolute;

        sal_strncpy(dest_char, "MDB DT Basic3", 13);
    }

    /**
     * Create the field group.
     */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &Cint_field_mdb_dt_fg_info,
                             &Cint_field_mdb_dt_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, Cint_field_mdb_dt_fg_id);
        return rv;
    }
    printf("Field Group ID %d was created. \n", Cint_field_mdb_dt_fg_id);

    attach_info.key_info.nof_quals = Cint_field_mdb_dt_fg_info.nof_quals;
    attach_info.payload_info.nof_actions = Cint_field_mdb_dt_fg_info.nof_actions;
    for (qual_iter = 0; qual_iter < Cint_field_mdb_dt_fg_info.nof_quals; qual_iter++)
    {
        attach_info.key_info.qual_types[qual_iter] = Cint_field_mdb_dt_fg_info.qual_types[qual_iter];
    }
    for (action_iter = 0; action_iter < Cint_field_mdb_dt_fg_info.nof_actions; action_iter++)
    {
        attach_info.payload_info.action_types[action_iter] = Cint_field_mdb_dt_fg_info.action_types[action_iter];
    }

    /**
     * Attach the created field group to the context.
     */
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_mdb_dt_fg_id, context_id,
                                        &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach on contex_id = %d \n", rv,
               context_id);
        return rv;
    }
    printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_mdb_dt_fg_id,
           context_id);

    return rv;
}

/**
 * \brief
 *  Function to create an entry for MDB DT FG.
 *
 * \param [in] unit         - Device id
 * \param [in] field_stage   - The Field stage to create the FG on
 * \param [in] fg_id        - The ID of the FG.
 * \param [in] qual_val      - The value of the Qual.
 * \param [in] act_value     - The value of the action.
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_mdb_dt_entry_cfg(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_group_t fg_id,
    uint32 qual_val,
    uint32 act_value)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    bcm_port_t port;
    int qual_index;
    bcm_field_entry_info_t entry_info;
    int action_iter;
    bcm_field_entry_info_t_init(&entry_info);

    entry_info.nof_entry_quals = Cint_field_mdb_dt_fg_info.nof_quals;
    entry_info.nof_entry_actions = Cint_field_mdb_dt_fg_info.nof_actions;

    for (qual_index = 0; qual_index < entry_info.nof_entry_quals; qual_index++)
    {
        entry_info.entry_qual[qual_index].type = Cint_field_mdb_dt_fg_info.qual_types[qual_index];
    }

    entry_info.entry_qual[0].value[0] = qual_val;

    for (action_iter = 0; action_iter < Cint_field_mdb_dt_fg_info.nof_actions; action_iter++)
    {
        entry_info.entry_action[action_iter].type = Cint_field_mdb_dt_fg_info.action_types[action_iter];
    }

    entry_info.entry_action[0].value[0] = act_value;

    if (field_stage == bcmFieldStageIngressPMF1)
    {
        entry_info.entry_qual[0].type = bcmFieldQualifyIp4Ttl;
        entry_info.entry_action[0].type = bcmFieldActionPrioIntNew;
    }

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &Cint_field_mdb_dt_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", Cint_field_mdb_dt_entry_handle,
           Cint_field_mdb_dt_entry_handle, fg_id);

    return rv;
}

/**
 * \brief
 *  Delete the entry.
 *
 * \param [in] unit                 - Device id
 * \param [in] field_stage   - The Field stage to create the FG on
 * \param [in] fg_id                - The FG ID.
 * \param [in] entry_id             - The entry id.
 * \param [in] qual_val              - The qual val.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_mdb_dt_entry_delete(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_group_t fg_id,
    bcm_field_entry_t entry_id,
    int qual_val)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int qual_index;

    bcm_field_entry_qual_t_init(entry_qual_info);

    for (qual_index = 0; qual_index < Cint_field_mdb_dt_fg_info.nof_quals; qual_index++)
    {
        entry_qual_info[qual_index].type = Cint_field_mdb_dt_fg_info.qual_types[qual_index];
    }

    entry_qual_info[0].value[0] = qual_val;

    if (field_stage == bcmFieldStageIngressPMF1)
    {
        entry_qual_info[0].type = bcmFieldQualifyIp4Ttl;
    }

    rv = bcm_field_entry_delete(unit, fg_id, entry_qual_info, entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
    return rv;
}

/**
 * \brief
 *  Destroys all configuration.
 *
 * \param [in] unit                 - Device id
 * \param [in] fg_id                - The FG ID.
 * \param [in] context_id           - Context ID FG is attached to.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_mdb_dt_destroy(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_group_context_detach(unit, fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    rv = bcm_field_group_delete(unit, fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }
    return rv;
}
