/*
 * Configuration:
 *

cint ../../../../src/examples/dnx/field/cint_field_evpn_esi.c
cint;
cint_field_evpn_ingress(0, 2);
cint_field_evpn_egress(0);
cint_field_evpn_egress_esi_port_entry_add(0, egress_context_id, egress_fg_id, &entry_handle_esi_port, 5, 2);
cint_field_evpn_egress_esi_port_entry_add(0, egress_context_id, egress_fg_id, &entry_handle_esi_port_1, 5, 9);
cint_field_evpn_egress_esi_entry_add(0, egress_context_id, egress_fg_id, &entry_handle_esi, 5, 2);

cint_field_evpn_destroy_all(0);
quit;

*/

/* Global Vars */
int evpn_esi_udh_base_3 = 3;

bcm_field_entry_t default_entry_handle;
int default_entry_priority = 50000;

bcm_field_entry_t entry_handle_esi_port;
bcm_field_entry_t entry_handle_esi;

bcm_field_entry_t entry_handle_esi_port_1;

bcm_field_group_t egress_fg_id;
bcm_field_group_t ingress_fg_id;

bcm_field_context_t egress_context_id;
bcm_field_context_t ingress_context_id;

bcm_field_presel_t ingress_presel_id = 61;
bcm_field_presel_t egress_presel_id = 65;

bcm_field_presel_entry_id_t ingress_presel_entry_id;
bcm_field_presel_entry_id_t egress_presel_entry_id;

bcm_field_qualify_t esi_qual = 0;
bcm_field_qualify_t esi_type_qual = 0;
bcm_field_action_t esi_action;


int cint_field_evpn_ingress_fg_add(int unit, bcm_field_group_t *fg_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    void *dest_char;
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_info_t fg_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    char *c_ptr;
    void *void_ptr;

    proc_name = "cint_field_evpn_ingress_fg_add";


    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "esi_type_qual", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &esi_type_qual);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_qualifier_create esi_type_qual\n", proc_name);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 20;
    void_ptr = qual_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "esi_qual", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &esi_qual);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_qualifier_create esi_qual\n", proc_name);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    /**Using the qual_idx so we can move qualifiers in key easy without modifying the index of qual*/
    qual_idx = 0;

    fg_info.qual_types[qual_idx] = esi_type_qual;
    qual_idx++;

    fg_info.qual_types[qual_idx] = esi_qual;
    qual_idx++;

    fg_info.nof_quals = qual_idx;


    /* Create UDA for a 20-bit ESI. UDA is required as the default size of the UDHData3 is 32bit. */
    bcm_field_action_info_t_init(&action_info);
    /**
    * Fill the structure needed for bcm_field_qualifier_create API
    */
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.action_type = bcmFieldActionUDHData3;
    action_info.size = 20;
    action_info.prefix_size=12;
    action_info.prefix_value=0;
    void_ptr = action_info.name;
    c_ptr = void_ptr;
    sal_strncpy_s(c_ptr, "UDHData3", sizeof(action_info.name));
    /**
    * Calling the API bcm_field_action_create for positive case
    */
    bcm_field_action_create(unit, 0, &action_info, &esi_action);


    /**Same for the actions to move them*/
    act_idx = 0;
    /** Order of actions should be the same as order of qualifiers*/
    fg_info.action_types[act_idx] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    fg_info.action_types[act_idx] = esi_action;
    fg_info.action_with_valid_bit[act_idx] = FALSE;
    act_idx++;

    fg_info.nof_actions = act_idx;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, fg_id_p);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_group_add\n", proc_name);
        return rv;
    }

    return rv;
}

int cint_field_evpn_ingress_fg_attach(int unit, bcm_field_context_t context_id, bcm_field_group_t fg_id)
{
    int rv = BCM_E_NONE;

    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_info_t fg_info;

    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_group_info_get( unit, fg_id, &fg_info);

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

    qual_idx = 0;
    /** UDH Data Type */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[qual_idx].input_arg = evpn_esi_udh_base_3;
    attach_info.key_info.qual_info[qual_idx].offset = 0;
    qual_idx++;

    /**Destination 20 bits from ESI (MPLS header) */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0;
    attach_info.key_info.qual_info[qual_idx].offset = -32;
    qual_idx++;

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", fg_id, context_id);
        return rv;
    }

    return rv;
}

int cint_field_evpn_ingress_context_create(int unit, bcm_field_context_t * context_id_p, int inlif_profile)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    int rv= 0;


    bcm_field_context_info_t_init(&context_info);

    rv = bcm_field_context_create
                    (unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id_p);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_evpn_ingress_context_create: Error in bcm_field_context_create context %d\n", *context_id_p);
        return rv;
    }

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. Context created for iPMF1 is also created for iPMF2*/
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);

    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, *context_id_p, &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_context_param_set for context %d\n", *context_id_p);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&ingress_presel_entry_id);
    /**For iPMF2, iPMF1 presel must be configured*/
    ingress_presel_entry_id.presel_id = ingress_presel_id;
    ingress_presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyInVportClass;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = inlif_profile;
    presel_entry_data.qual_data[0].qual_mask = 0xF;
    rv = bcm_field_presel_set(unit, 0, &ingress_presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_evpn_context_create Error in bcm_field_presel_set  \n");
        return rv;
    }
    return rv;
}




int cint_field_evpn_ingress(int unit, int inlif_profile)
{
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_ingress";

    rv = cint_field_evpn_ingress_context_create(unit, &ingress_context_id, inlif_profile);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_context_create\n", proc_name);
        return rv;
    }


    rv = cint_field_evpn_ingress_fg_add(unit, &ingress_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_ingress_fg_add\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_ingress_fg_attach(unit, ingress_context_id, ingress_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_ingress_fg_attach\n", proc_name);
        return rv;
    }

    return rv;
}

int cint_field_evpn_egress_context_create(int unit, bcm_field_context_t *context_id_p)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    int rv= 0;


    bcm_field_context_info_t_init(&context_info);
    context_info.name[0] = 'E';
    context_info.name[1] = 'S';
    context_info.name[2] = 'I';
    context_info.name[3] = 0;
    rv = bcm_field_context_create
                    (unit, 0, bcmFieldStageEgress, &context_info, context_id_p);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_evpn_egress_context_create: Error in bcm_field_context_create\n");
        return rv;
    }

    /* Set the Presels: Current Layer Protocol and UDH Type*/
    bcm_field_presel_entry_id_info_init(&egress_presel_entry_id);
    egress_presel_entry_id.presel_id = egress_presel_id;
    egress_presel_entry_id.stage = bcmFieldStageEgress;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = *context_id_p;
    presel_entry_data.entry_valid = TRUE;


    /** UDH Data Type */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyUDHBase3;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = evpn_esi_udh_base_3;
    presel_entry_data.qual_data[0].qual_mask = 0x3;


    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = bcmFieldLayerTypeEth;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &egress_presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("cint_field_evpn_context_create Error in Egress bcm_field_presel_set for context %d\n", *context_id_p);
        return rv;
    }

    return rv;
}


int cint_field_evpn_egress_fg_add(int unit, bcm_field_group_t *fg_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int qual_idx = 0;
    bcm_field_group_info_t fg_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t bcm_action_id;

    proc_name = "cint_field_evpn_egress_fg_add";

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;

    fg_info.qual_types[0] = bcmFieldQualifyUDHData3;

    fg_info.qual_types[1] = bcmFieldQualifyOutPort;

    fg_info.nof_quals = 2;

    /* Action - Drop */
    fg_info.action_types[0] = bcmFieldActionDrop;
    fg_info.action_with_valid_bit[0] = TRUE;

    fg_info.nof_actions = 1;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, fg_id_p);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_group_add\n", proc_name);
        return rv;
    }

    return rv;
}


int cint_field_evpn_egress_fg_attach(int unit, bcm_field_context_t context_id, bcm_field_group_t fg_id)
{
    int rv = BCM_E_NONE;

    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_info_t fg_info;

    bcm_field_group_attach_info_t_init(&attach_info);
    rv = bcm_field_group_info_get( unit, fg_id, &fg_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_info_get fg_id %d context_id %d\n", fg_id, context_id);
        return rv;
    }

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

    qual_idx = 0;
    /** UDH Data Type */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    qual_idx++;

    /** Out Port */
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
    qual_idx++;

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", fg_id, context_id);
        return rv;
    }

    return rv;
}


int cint_field_evpn_egress_default_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_p)
{
    char *proc_name;

    bcm_field_entry_info_t entry_in_info;
    bcm_field_group_info_t fg_info;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_egress_default_entry_add";
    bcm_field_group_info_get( unit, fg_id, &fg_info);

    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = 0;
    entry_in_info.entry_qual[0].mask[0] = 0;

    entry_in_info.entry_action[0].type = fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = 1;

    entry_in_info.priority = default_entry_priority;
    entry_in_info.nof_entry_quals = 1;
    entry_in_info.nof_entry_actions = 1;
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add for context %d, fg_id %d\n", proc_name, rv, context_id, fg_id);
        return rv;
    }
    return rv;
}

int cint_field_evpn_egress_esi_port_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_esi_port_p,
        int esi_label,
        int port)
{
    char *proc_name;

    bcm_field_entry_info_t entry_in_info;
    bcm_field_group_info_t fg_info;
    bcm_gport_t gport;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_egress_default_entry_add";
    bcm_field_group_info_get( unit, fg_id, &fg_info);

    if (BCM_GPORT_IS_SET(port))
    {
        gport = port;
    }
    else
    {
        BCM_GPORT_LOCAL_SET(gport, port);
    }

    /* Add the Entry (ESI + Port) --> Drop */
    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = esi_label;
    entry_in_info.entry_qual[0].mask[0] = 0xfffff;

    entry_in_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_in_info.entry_qual[1].value[0] = gport;
    entry_in_info.entry_qual[1].mask[0] = 0xff;

    entry_in_info.entry_action[0].type = fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = 1;

    entry_in_info.priority = 10;
    entry_in_info.nof_entry_quals = 2;
    entry_in_info.nof_entry_actions = 1;
    if (BCM_GPORT_IS_TRUNK(gport))
    {
        entry_in_info.core = 0;
    }
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_esi_port_p);
    if (BCM_GPORT_IS_TRUNK(gport))
    {
        entry_in_info.core = 1;
        rv = bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_esi_port_p);
    }

    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add for context %d, fg_id %d\n", proc_name, rv, context_id, fg_id);
        return rv;
    }

    return rv;
}

int cint_field_evpn_egress_esi_entry_add(
        int unit,
        bcm_field_context_t context_id,
        bcm_field_group_t fg_id,
        bcm_field_entry_t *entry_handle_esi_p,
        int esi_label)
{
    char *proc_name;

    bcm_field_entry_info_t entry_in_info;
    bcm_field_group_info_t fg_info;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_egress_default_entry_add";
    bcm_field_group_info_get( unit, fg_id, &fg_info);

    /*
     * Add the Entry (ESI) --> Do Nothing.
     */
    bcm_field_entry_info_t_init(&entry_in_info);

    entry_in_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_in_info.entry_qual[0].value[0] = esi_label;
    entry_in_info.entry_qual[0].mask[0] = 0xfffff;

    entry_in_info.entry_action[0].type = fg_info.action_types[0];
    entry_in_info.entry_action[0].value[0] = 0;

    entry_in_info.priority = 15;
    entry_in_info.nof_entry_quals = 1;
    entry_in_info.nof_entry_actions = 1;
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_in_info, entry_handle_esi_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_add for context %d, fg_id %d\n", proc_name, rv, context_id, fg_id);
        return rv;
    }

    return rv;
}

int cint_field_evpn_egress(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_egress";

    rv = cint_field_evpn_egress_context_create(unit, &egress_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_egress_context_create\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_egress_fg_add(unit, &egress_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_egress_fg_add\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_egress_fg_attach(unit, egress_context_id, egress_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_egress_fg_attach\n", proc_name);
        return rv;
    }

    rv = cint_field_evpn_egress_default_entry_add(unit, egress_context_id, egress_fg_id, &default_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in cint_field_evpn_egress_default_entry_add\n", proc_name);
        return rv;
    }

    return rv;
}



int cint_field_evpn_destroy_all_entries(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_destroy_all_entries";

    /* Destroy the default entry */
    rv = bcm_field_entry_delete(unit,egress_fg_id, NULL, default_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete for egress for fg_id %d for default entry %d\n", proc_name, rv, egress_fg_id, default_entry_handle);
        return rv;
    }

    if(entry_handle_esi_port != 0)
    {
        /* Destroy the entry */
        rv = bcm_field_entry_delete(unit,egress_fg_id, NULL, entry_handle_esi_port);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_entry_delete for egress for fg_id %d for entry %d\n", proc_name, rv, egress_fg_id, entry_handle_esi_port);
            return rv;
        }
    }
    if(entry_handle_esi != 0)
    {
        /* Destroy the entry */
        rv = bcm_field_entry_delete(unit,egress_fg_id, NULL, entry_handle_esi);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_entry_delete for egress for fg_id %d for entry %d\n", proc_name, rv, egress_fg_id, entry_handle_esi);
            return rv;
        }
    }
    if(entry_handle_esi_port_1 != 0)
    {
        /* Destroy the entry */
        rv = bcm_field_entry_delete(unit,egress_fg_id, NULL, entry_handle_esi_port_1);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_entry_delete for egress for fg_id %d for entry %d\n", proc_name, rv, egress_fg_id, entry_handle_esi_port_1);
            return rv;
        }
    }
    return rv;
}

int cint_field_evpn_destroy_ugq_and_uda(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_evpn_destroy_ugq_and_uda";

    rv = bcm_field_qualifier_destroy(unit, esi_type_qual);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_qualifier_destroy esi_type_qual %d\n", proc_name, esi_type_qual);
        return rv;
    }


    rv = bcm_field_qualifier_destroy(unit, esi_qual);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_qualifier_destroy esi_qual %d\n", proc_name, esi_qual);
        return rv;
    }

    rv = bcm_field_action_destroy(unit, esi_action);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_action_destroy esi_action %d\n", proc_name, esi_action);
        return rv;
    }

    return rv;
}

int cint_field_evpn_destroy_all(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t presel_entry_data;

    proc_name = "cint_field_evpn_destroy_all";

    /* Destroy Egress configuration */
    rv = cint_field_evpn_destroy_all_entries(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_destroy_all_entries for egress for fg_id %d for all entries\n", proc_name, rv, egress_fg_id);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, egress_fg_id, egress_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_group_context_detach for egress fg_id %d context_id %d\n", proc_name, egress_fg_id, egress_context_id);
        return rv;
    }

    rv = bcm_field_group_delete(unit, egress_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_group_delete for egress fg_id \n", proc_name, egress_fg_id);
        return rv;
    }

    /* Destroy the preselector */
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &egress_presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_presel_destroy for egress context %d presel_id %d \n", proc_name, egress_context_id, egress_presel_id);
        return rv;
    }


    rv = bcm_field_context_destroy
                    (unit, bcmFieldStageEgress, egress_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_context_destroy for egress context %d\n",proc_name, egress_context_id);
        return rv;
    }


    /* Destroy Ingress configuration */

    rv = bcm_field_group_context_detach(unit, ingress_fg_id, ingress_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach for ingress fg_id %d context_id %d\n", ingress_fg_id, ingress_context_id);
        return rv;
    }


    rv = bcm_field_group_delete(unit, ingress_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_group_delete for ingress fg_id \n", proc_name, egress_fg_id);
        return rv;
    }

    /* Destroy UDQ & UDA */
    rv = cint_field_evpn_destroy_ugq_and_uda(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_evpn_destroy_ugq_and_uda for ingress for fg_id %d for udq and uda\n", proc_name, rv, ingress_fg_id);
        return rv;
    }


    rv = bcm_field_presel_set(unit, 0, &ingress_presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_presel_destroy for ingress context %d presel %d \n", proc_name, ingress_context_id, ingress_presel_id);
        return rv;
    }

    rv = bcm_field_context_destroy
                    (unit, bcmFieldStageIngressPMF1, ingress_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error in bcm_field_context_destroy for ingress context %d\n",proc_name, ingress_context_id);
        return rv;
    }

    return rv;
}

