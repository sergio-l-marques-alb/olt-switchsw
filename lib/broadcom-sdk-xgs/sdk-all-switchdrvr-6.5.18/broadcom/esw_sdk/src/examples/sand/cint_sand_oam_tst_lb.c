/*
 * $Id: cint_sand_oam_tst_lb.c, yw889499 Exp $
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 */
/*
 *
 * This cint give a example to add trap info for oam tst lb. 
 */
/**
 * Creating OAM egress trap for tst example
 *
 * @param unit
 * @return error code
 */
int dnx_oam_set_tst_trap(
    int unit, int is_upmep, bcm_gport_t sat_port, int lb_tst_flag, int mep_id)
{
    int rv;
    void *dest_char;
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t context_param;
    bcm_field_context_t context_id;
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_rx_trap_config_t trap_config;
    int count;
    uint32 flags = 0;
    int up_mep_ingress_trap_id;
    int up_mep_ingress_trap_id2;
    int tst_trap_id;
    int up_mep_stamp_trap;    
    int trap_id = 0;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_profile_t enc_ing_lif_profile_id, enc_egr_lif_profile_id;
    bcm_oam_profile_t ing_lif_profile_id, egr_lif_profile_id;
    bcm_oam_profile_t acc_ing_action_profile_id;
    bcm_gport_t gport;
    /*
     * Get SAT TST trap
     */
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapEgTxOamUpMEPDest2, &tst_trap_id);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("1(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    BCM_IF_ERROR_RETURN(bcm_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamUpMEPDest2, &tst_trap_id));
    trap_id = tst_trap_id & 0x1FF;
    /*
     * Create oam action profile
     */
    rv = bcm_oam_profile_create(unit, 0, bcmOAMProfileEgressLIF, &egr_lif_profile_id);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("1(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_oam_profile_create(unit, 0, bcmOAMProfileIngressLIF, &ing_lif_profile_id);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("1(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    bcm_oam_endpoint_get(unit, mep_id, &endpoint_info);
    gport = endpoint_info.gport;
    flags = 0;
    bcm_oam_action_key_t_init(&action_key);
    if (endpoint_info.type == bcmOAMEndpointTypeEthernet)
    {
        action_key.dest_mac_type = bcmOAMDestMacTypeMyCfmMac;
    }
    else if ((endpoint_info.type == bcmOAMEndpointTypeBHHMPLS) || (endpoint_info.type == bcmOAMEndpointTypeBHHPwe))
    {
        action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
    }
    else
    {
        printf("TST just support in ETH / MPLS/PWE");
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcm_oam_profile_id_get_by_type
                    (unit, ing_lif_profile_id, bcmOAMProfileIngressLIF, &flags, &enc_ing_lif_profile_id));
    BCM_IF_ERROR_RETURN(bcm_oam_profile_id_get_by_type
                    (unit, egr_lif_profile_id, bcmOAMProfileEgressLIF, &flags, &enc_egr_lif_profile_id));
    BCM_IF_ERROR_RETURN(bcm_oam_lif_profile_set(unit, 0, gport, enc_ing_lif_profile_id, enc_egr_lif_profile_id));

    acc_ing_action_profile_id = endpoint_info.acc_profile_id & 0xff;
    bcm_oam_profile_create(unit, BCM_OAM_PROFILE_WITH_ID, bcmOAMProfileIngressAcceleratedEndpoint,
                           &acc_ing_action_profile_id);
    acc_ing_action_profile_id = endpoint_info.acc_profile_id;

    /*
     * Prepare action to redirect TST to SAT (on OAMP)
     */
    action_key.flags = 0;
    action_key.inject = 0;
    if (lb_tst_flag & 0x1)
    {
        action_key.opcode = bcmOamOpcodeLBR;
    }
    else if (lb_tst_flag & 0x2)
    {
        action_key.opcode = bcmOamOpcodeTST;
    }
    else
    {
        printf("error lb_tst_flag\n");
        return BCM_E_PARAM;
    }
    action_key.endpoint_type = bcmOAMMatchTypeMEP;

    /*
     * Set action destination and set the action
     */
    bcm_oam_action_result_t_init(&action_result);
    BCM_GPORT_TRAP_SET(action_result.destination, trap_id, 15, 0);      /* Taken from default values */
    action_result.flags = 0;
    BCM_IF_ERROR_RETURN(bcm_oam_profile_action_set(unit, 0, acc_ing_action_profile_id, &action_key, &action_result));

    if (!is_upmep)
    {
        rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapEgTxOamUpMEPDest2, &up_mep_ingress_trap_id);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("1(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_rx_trap_type_get(unit, flags, bcmRxTrapEgTxOamUpMEPDest2, &up_mep_ingress_trap_id);
        if (rv != BCM_E_NONE) {
            printf("2(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        up_mep_ingress_trap_id = up_mep_ingress_trap_id&0x1ff;
        rv = bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &up_mep_ingress_trap_id);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("3(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        bcm_rx_trap_config_t_init(&trap_config);
        /*config dest port to SAT port.*/
        trap_config.dest_port = sat_port;
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
        rv = bcm_rx_trap_set(unit, up_mep_ingress_trap_id&0x1ff, trap_config);
        if (rv != BCM_E_NONE) {
            printf("4(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        printf("Done\n");
        return 0;
    }
    bcm_rx_trap_config_t_init(&trap_config);
    /*config dest port to SAT port.*/
    trap_config.dest_port = sat_port;
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &up_mep_ingress_trap_id);
    if (rv != BCM_E_NONE) {
        printf("3(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_rx_trap_set(unit, up_mep_ingress_trap_id, trap_config);
    if (rv != BCM_E_NONE) {
        printf("4(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    /** Ingress trap strength range 0-15 */
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, up_mep_ingress_trap_id, 15, 0);
    /** Trap stamped on FHEI header */
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapEgTxOamUpMEPDest2, &up_mep_ingress_trap_id2);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("1(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_rx_trap_type_get(unit, flags, bcmRxTrapEgTxOamUpMEPDest2, &up_mep_ingress_trap_id2);
    if (rv != BCM_E_NONE) {
        printf("5(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_rx_trap_set(unit, up_mep_ingress_trap_id2, trap_config);
    if (rv != BCM_E_NONE) {
        printf("6(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    /*set stamped_trap_code in FHEI*/
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapEgTxOamUpMEPDest1, &up_mep_stamp_trap);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("1(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_rx_trap_type_get(unit, flags, bcmRxTrapEgTxOamUpMEPDest1, &up_mep_stamp_trap);
    if (rv != BCM_E_NONE) {
        printf("5(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_rx_trap_get(unit, up_mep_stamp_trap, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("5(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    trap_config.stamped_trap_code =  up_mep_ingress_trap_id2 & 0x1FF;
    rv = bcm_rx_trap_set(unit, up_mep_stamp_trap, trap_config);
    if (rv != BCM_E_NONE) {
        printf("5(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "OAM_No_Header", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE) {
        printf("7(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /** Changing new context SYS HDR profile to None (No SYS HDRs are added) */
    context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    context_param.param_arg = 0;
    context_param.param_val = bcmFieldSystemHeaderProfileNone;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &context_param);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    context_param.param_arg = 0;
    context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset0, 0);
    /**Context that was created for iPMF1 is also created for iPMF2*/
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &context_param);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /** Adding trap_code preselector for the context */
    presel_entry_id.presel_id = 4;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    presel_data.entry_valid = TRUE;
    presel_data.context_id = context_id;
    presel_data.nof_qualifiers = 1;
    presel_data.qual_data[0].qual_type = bcmFieldQualifyRxTrapCode;
    presel_data.qual_data[0].qual_value = up_mep_ingress_trap_id;
    presel_data.qual_data[0].qual_mask = 0x1FF;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_data);
    if (rv != BCM_E_NONE) {
        printf("6(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Done\n");
    return rv;
}

