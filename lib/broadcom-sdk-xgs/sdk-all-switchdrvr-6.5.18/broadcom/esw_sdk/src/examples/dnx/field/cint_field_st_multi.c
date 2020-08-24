/*
 * Configuration example start:
 *  
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
 * cint ../../../src/examples/dnx/field/cint_field_st_multi.c
 * cint;
 * int unit = 0;
 * bcm_field_layer_type_t fwd_layer = bcmFieldLayerTypeEth;
 * cint_field_st_multi_main(unit, fwd_layer);
 *
 * Configuration example end
 *
 * This is an example how we can split the State Table memory between different State Table FGs
 *  by using constant values as prefixes to define a unique range per State Table FG.
 * This way, both added FGs won't have any shared entries,
 * allowing them to freely engage their range without colliding with one another.
 *
 * Configures the following scenario:
 * 1. Create 2 ST FGs.
 * 2. Create 2 different contexts with ST support (1 cascaded to the other).
 * 3. Attach the 2 ST FGs to the 2 IPMF1 contexts.
 * 4. Create 1 IPMF2 FG.
 * 5. Attach the IPMF2 FG to the context.
 * 6. Add 2 entries into the FG with different action value.
 *
 * main func:
 * cint_field_st_multi_main(unit, fwd_layer);
 *
 * destroy func:
 * cint_field_st_multi_destroy(unit);
 *
 */
/**
 * Nof of used contexts. (2 for ST. 1 for IPMF2 FG).
 */
int Cint_field_st_multi_nof_context = 2;
/**
 * Nof FGs. ( 2 ST and 1 IPMF2 TCAM)
 */
int Cint_field_st_multi_nof_fg = 3;
/**
 * Nof entries for IPMF2 TCAM.
 */
int Cint_field_st_multi_nof_entries = 2;
/**
 * Nof quals in the ST FGs.
 */
int Cint_field_st_multi_nof_quals = 4;
/**
 * Nof user defined quals.
 */
int Cint_field_st_multi_nof_udq = 2;
/**
 * Nof presels.
 */
int Cint_field_st_multi_nof_presels = 2;
/**
 * Array in which we are collecting all Qual IDs so we can delete them afterwards.
 */
bcm_field_qualify_t Cint_field_st_multi_qual_id[Cint_field_st_multi_nof_udq];
/**
 * Array in which we are collecting all FG IDs so we can delete them afterwards.
 */
bcm_field_group_t Cint_field_st_multi_fg_id[Cint_field_st_multi_nof_fg];
/**
 * Array in which we are collecting all Context IDs so we can delete them afterwards.
 */
bcm_field_context_t Cint_field_st_multi_context_id_arr[Cint_field_st_multi_nof_context];
/**
 * Array in which we are collecting all Entry IDs so we can delete them afterwards.
 */
bcm_field_entry_t Cint_field_st_multi_entry_id[Cint_field_st_multi_nof_entries];
/**
 * Variable in which we are collecting the Presel ID so we can delete it afterwards.
 */
bcm_field_presel_t Cint_field_st_multi_presel_id_arr[Cint_field_st_multi_nof_presels] = {22, 21};


/**
 * \brief
 * Function which creates a context for ST1 and ST2.
 * Second context is cascaded from the first one.
 *
 * \param [in]  unit               - Device ID
 * \param [in]  context_iter       - Shows if the presel is configured
 *                                for the 1 ST FG  or for the 2nd ST FG.
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_st_multi_context_create(
    int unit,
    int context_iter)
{
    bcm_field_context_info_t context_info;
    int rv = BCM_E_NONE;

    bcm_field_context_info_t_init(&context_info);
    context_info.state_table_enabled = TRUE;
    /**
     * Second context should be cascaded from the first one.
     */
    if(context_iter == 1)
    {
        context_info.cascaded_from = Cint_field_st_multi_context_id_arr[0];
    }
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &Cint_field_st_multi_context_id_arr[context_iter]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
    printf("Context with ID= %d was created! \n", Cint_field_st_multi_context_id_arr[context_iter]);
    return rv;
}

/**
 * \brief
 * Function which sets a preselector.
 * For the first context the presel is with qual Forwarding type.
 * For the second context the presel is with qual VlanFormat.
 *
 * \param [in]  unit               - Device ID
 * \param [in]  fwd_layer          - The forwarding layer.
 * \param [in]  presel_index       - Shows if the presel is configured
 *                                for the 1 ST FG  or for the 2nd ST FG.
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_st_multi_presel_set(
    int unit,
    bcm_field_layer_type_t fwd_layer,
    int presel_index)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualify_t presel_qual[Cint_field_st_multi_nof_presels] = {bcmFieldQualifyForwardingType, bcmFieldQualifyVlanFormat};
    uint32 presel_qual_mask[Cint_field_st_multi_nof_presels] = {0x1F ,0xF};
    uint32 presel_qual_value[Cint_field_st_multi_nof_presels] = {fwd_layer, BCM_FIELD_VLAN_FORMAT_INNER_TAGGED};

    int rv = BCM_E_NONE;
    /**
     * Init the structures
     */
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);
    /**
     * Fill the structures
     */
    p_id.stage = bcmFieldStageIngressPMF1;
    p_id.presel_id = Cint_field_st_multi_presel_id_arr[presel_index];

    p_data.entry_valid = TRUE;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = presel_qual[presel_index];
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_mask = 0x1F;
    p_data.qual_data[0].qual_value = presel_qual_value[presel_index];
    p_data.context_id = Cint_field_st_multi_context_id_arr[presel_index];
    /**
     * Set the presel.
     */
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }
    printf("Presel (%d) was configured context(%d)\n",
            Cint_field_st_multi_presel_id_arr[presel_index], Cint_field_st_multi_context_id_arr[presel_index]);

    return rv;
}

/**
 * \brief
 * Config a ST with different address prefix.
 *
 * \param [in]  unit               - Device ID
 * \param [in]  fwd_layer          - The forwarding layer.
 * \param [in]  address_prefix     - The address prefix.
 *                    (First case is 0, second case is 1)
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_st_multi_cfg(
    int unit,
    bcm_field_layer_type_t fwd_layer,
    int address_prefix)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    int qual_iter;

    int rv = BCM_E_NONE;
    /**
     * Create context
     */
    rv = cint_field_st_multi_context_create(unit, address_prefix);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in func cint_field_st_multi_context_create \n", rv);
       return rv;
    }
    /**
     * Set a presel.
     */
    rv = cint_field_st_multi_presel_set(unit, fwd_layer, address_prefix);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_st_multi_presel_set\n", rv);
        return rv;
    }

    /*******************************************************************************
     *                                    STEP 1                                   *
     *******************************************************************************/
    printf("1.Create State Table FG Qualifiers \n");

    bcm_field_qualifier_info_create_t_init(&qual_info);

    for(qual_iter = 0; qual_iter < Cint_field_st_multi_nof_udq; qual_iter++)
    {
        if (qual_iter == 0)
        {
            qual_info.size = 2;
        }
        else
        {
            qual_info.size = 1;
        }
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_st_multi_qual_id[qual_iter]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_create\n", rv);
            return rv;
        }
    }
    /*******************************************************************************
     *                                    STEP 2                                   *
     *******************************************************************************/
    printf("2.Create State Table FG\n");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeStateTable;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = Cint_field_st_multi_nof_quals;
    /**
     * Address InPort (9bits) + 1constant qual (1bit)
     * The value of the Constant qual is the input parameter of the function.
     */
    fg_info.qual_types[0] = bcmFieldQualifyIp4Protocol;
    fg_info.qual_types[1] = Cint_field_st_multi_qual_id[0];
    /**
     * Data (8 bits)
     */
    fg_info.qual_types[2] = bcmFieldQualifyIp4Ttl;
    /**
     * do-write (always set to 1)
     */
    fg_info.qual_types[3] = Cint_field_st_multi_qual_id[1];

    rv = bcm_field_group_add(unit, 0, &fg_info, &Cint_field_st_multi_fg_id[address_prefix]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    /*******************************************************************************
     *                                    STEP 3                                   *
     *******************************************************************************/
    printf("3.Attaching State Table FG to the context.\n");
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = Cint_field_st_multi_nof_quals;
    /**
     * Set the prefix address to be 0 in the first ST FG and 1 in the second FG.
     */
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    /**
     * First ST FG will be with prefix 0.
     * Second ST FG will be with prefix 1.
     */
    attach_info.key_info.qual_info[1].input_arg = address_prefix;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 1;
    attach_info.key_info.qual_info[2].offset = 0;

    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = 1;
    attach_info.key_info.qual_info[3].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, Cint_field_st_multi_fg_id[address_prefix], Cint_field_st_multi_context_id_arr[address_prefix], &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    return rv;
}

/**
 * \brief
 * Configure an IPMF2 FG with 2 entries.
 * 1 entry is from the 1 ST FG.
 * 2 entry is from the 2 ST FG.
 *
 * \param [in]  unit               - Device ID.
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_st_multi_ipmf2_fg_cfg(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    int entry_iter;
    /**
     * The TTL value from the 1st ST FG (7) and for the 2nd ST FG (6).
     */
    uint32 qual_value[Cint_field_st_multi_nof_entries] = {0x7, 0x6};
    /**
     * The action values.
     * If we hit the 1st entry the TC value must be 0x2
     * If we hit the second entry the TC value must be 0x3
     */
    uint32 action_value[Cint_field_st_multi_nof_entries] = {0x2, 0x3};
    int rv = BCM_E_NONE;
    /*******************************************************************************
    *                                    STEP 1                                   *
    *******************************************************************************/
    printf("1. Create IPMF2 FG\n");

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyStateTableData;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_st_multi_fg_id[2]);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_field_group_add\n", rv);
      return rv;
    }

    /*******************************************************************************
     *                                    STEP 2                                   *
     *******************************************************************************/
    printf("2. Attach the FG to the context \n");
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, Cint_field_st_multi_fg_id[2], Cint_field_st_multi_context_id_arr[1], &attach_info);
    if (rv != BCM_E_NONE)
    {
      printf("Error (%d), in bcm_field_group_context_attach\n", rv);
      return rv;
    }
    /*******************************************************************************
     *                                    STEP 3                                   *
     *******************************************************************************/
    printf("3. Add 2 entries in IPMF2 FG. \n");
    /**
    * Fill the entry_info.
    */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].mask[0] = 0xFF;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    /**
     * Iterate over all entries and set their qual and action values.
     */
    for (entry_iter = 0; entry_iter < Cint_field_st_multi_nof_entries; entry_iter++)
    {
        entry_info.entry_qual[0].value[0] = qual_value[entry_iter];
        entry_info.entry_action[0].value[0] = action_value[entry_iter];
        /**
        * Add an entry to the FG
        */
        rv = bcm_field_entry_add(unit, 0, Cint_field_st_multi_fg_id[2], &entry_info, &Cint_field_st_multi_entry_id[entry_iter]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_add ipmf1 entry_id(%d)\n", rv, Cint_field_header_quals_entry_id);
            return rv;
        }
    }
    return rv;
}


/**
 * \brief
 * 1. Create 1st ST with action_prefix 0.
 * 2. Create 2nd ST with action_prefix 1.
 * 3. Create  IPMF2 FG with 2 entries:
 *    3.1. One for the 1st ST fg;
 *    3.2. One for the 2nd ST FG..
 *
 * \param [in]  unit               - Device ID
 * \param [in]  fwd_layer          - The forwarding layer.
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_st_multi_main(
    int unit,
    bcm_field_layer_type_t fwd_layer)
{
    int rv = BCM_E_NONE;
    /**
     * Create 1 ST with action_prefix 0.
     */
    rv = cint_field_st_multi_cfg(unit, fwd_layer, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_st_multi_cfg %d\n", rv);
        return rv;
    }
    /**
     * Create 2 ST with action_prefix 1.
     */
    rv = cint_field_st_multi_cfg(unit, fwd_layer, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_st_multi_cfg %d\n", rv);
        return rv;
    }
    /**
     * Create IPMF2 FG with 2 entries:
     *  1 for the 1st ST fg;
     *  2 for the 2nd ST FG.
     */
    rv = cint_field_st_multi_ipmf2_fg_cfg(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_st_multi_cfg %d\n", rv);
        return rv;
    }
    return rv;
}


/**
 * \brief
 *  Destroy the whole configuration.
 *
 * \param [in]  unit               - Device ID
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_st_multi_destroy(
    int unit)
{
    int context_iter, qual_iter, entry_iterator, fg_iter, presel_iter;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;
    int rv = BCM_E_NONE;

    printf("Destroying created preselector...\r\n");
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    p_id.stage = bcmFieldStageIngressPMF1;

    printf("Destroying created preselector...\r\n");
    /**
     * .Cleaning Presels
     */
    for (presel_iter = 0; presel_iter < Cint_field_st_multi_nof_entries; presel_iter++)
    {
        p_id.presel_id = Cint_field_st_multi_presel_id_arr[presel_iter];
        rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
        if(rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_presel_set\n");
            return rv;
        }
    }

    printf("Deleting created entries...\r\n");
    /**
     * Delete the created entries for IPMF2 FG.
     */
    for (entry_iterator = 0; entry_iterator < Cint_field_st_multi_nof_entries; entry_iterator++)
    {
        rv = bcm_field_entry_delete(unit, Cint_field_st_multi_fg_id[2], NULL, Cint_field_st_multi_entry_id[entry_iterator]);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_entry_delete entry \n", rv);
            return rv;
        }
    }

    printf("Detaching FGs from created contexts...\r\n");
    for (context_iter = 0; context_iter < Cint_field_st_multi_nof_context; context_iter++)
    {
        rv = bcm_field_group_context_detach(unit, Cint_field_st_multi_fg_id[context_iter], Cint_field_st_multi_context_id_arr[context_iter]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, Cint_field_st_multi_fg_id[context_iter], Cint_field_st_multi_context_id_arr[context_iter]);
            return rv;
        }
    }

    rv = bcm_field_group_context_detach(unit, Cint_field_st_multi_fg_id[2], Cint_field_st_multi_context_id_arr[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, Cint_field_st_multi_fg_id[context_iter], Cint_field_st_multi_context_id_arr[context_iter]);
        return rv;
    }

    printf("Deleting all created FGs...\r\n");
    for (fg_iter = 0; fg_iter < Cint_field_st_multi_nof_fg; fg_iter++)
    {
        rv = bcm_field_group_delete(unit, Cint_field_st_multi_fg_id[fg_iter]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, Cint_field_st_multi_fg_id[fg_iter]);
            return rv;
        }
    }

    printf("Destroying contexts...\r\n");
    for (context_iter = 0; context_iter < Cint_field_st_multi_nof_context; context_iter++)
    {
        rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_st_multi_context_id_arr[context_iter]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_context_destroy contex_id %d \n", rv, Cint_field_st_multi_context_id_arr[context_iter]);
            return rv;
        }
    }

    printf("Destroying the user defined quals...\r\n");
    for (qual_iter = 0; qual_iter < Cint_field_st_multi_nof_udq; qual_iter++)
    {
        rv = bcm_field_qualifier_destroy(unit, Cint_field_st_multi_qual_id[qual_iter]);
        if(rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_destroy, Const_qual\n", rv);
            return rv;
        }
    }
    return rv;
}
