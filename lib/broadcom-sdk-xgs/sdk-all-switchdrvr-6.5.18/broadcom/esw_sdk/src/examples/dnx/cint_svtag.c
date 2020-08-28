/*
 * $Id: cint_svtag_swap.c
 */
/*
 * svtag swap usage examples.
 */




/*
 * Call back function that used for the traverse example that increases an entry SVTAG with the requested amount.
 * see bcm_switch_svtag_egress_traverse_cb API description.
 */
int increase_signature_value(int unit,
uint32 flags,
bcm_gport_t gport,
bcm_switch_svtag_egress_info_t *svtag_info,
void *user_data)
{
    int rv = BCM_E_NONE;
    int *increase_value = user_data;

    rv = bcm_switch_svtag_egress_entry_get(unit,0, gport, svtag_info);
    if (rv != BCM_E_NONE) {
         printf("Error, bcm_switch_svtag_egress_entry_get Failed, gport value 0x%x\n", gport);
         return rv;
    }

    svtag_info->signature += (*increase_value);

    rv = bcm_switch_svtag_egress_entry_add(unit,BCM_SWITCH_SVTAG_EGRESS_REPLACE, gport, svtag_info);
    if (rv != BCM_E_NONE) {
         printf("Error, bcm_switch_svtag_egress_entry_add Failed, gport value 0x%x\n", gport);
         return rv;
    }

    return rv;
}

/*
 * This is a traverse call example that goes over all the SVTAG entries and increases the signature value byt the
 * requested amount
 * increase_value - the number to increase the signature value
 */
int dnx_svtag_traverse_increase_signature(int unit,int increase_value)
{
    int rv = BCM_E_NONE;

    rv = bcm_switch_svtag_egress_entry_traverse( unit,0, increase_signature_value, &increase_value);

    if (rv != BCM_E_NONE) {
         printf("Error, bcm_switch_svtag_egress_entry_traverse Failed");
         return rv;
    }

    return rv;
}



int dnx_svtag_swap(
    int unit,
    int in_core,
    int in_port)
{
    int rv;
    int inst_id = -1;
    /* the offset to the SVTAG including the PTCH */
    uint32 svtag_offset = 14;
    /* 0 means 4 bytes */
    uint32 svtag_swap_size = 0;
    uint32 entry_handle_id;
    uint32 port = in_port;
    uint32 core = in_core;


    /*
     * Configure global swap
     */
    dnx_dbal_entry_handle_take(unit, "SIT_INGRESS_GENERAL_CONFIGURATION", &entry_handle_id);
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "GLOBAL_SWAP_TPID_OFFSET", inst_id, &svtag_offset);
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "GLOBAL_SWAP_TPID_SIZE", inst_id, &svtag_swap_size);
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");


   /*
    * Enable global swap
    */
   dnx_dbal_entry_handle_take(unit, "INGRESS_PTC_PORT", &entry_handle_id);
   dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "PTC", &port);
   dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "CORE_ID", &core);

   uint32 tag_swap_enable = 1; /* enable */
   dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "TAG_SWAP_ENABLE", inst_id, &tag_swap_enable);
   uint32 tag_swap_mode = 0; /* DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_GLOBAL_TRIGGER */
   dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "TAG_SWAP_OP_MODE", inst_id, &tag_swap_mode);
   dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");



    return rv;
}
