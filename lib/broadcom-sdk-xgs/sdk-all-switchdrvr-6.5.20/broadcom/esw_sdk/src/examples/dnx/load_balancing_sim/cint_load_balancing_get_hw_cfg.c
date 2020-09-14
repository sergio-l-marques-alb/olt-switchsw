/*
 * $Id: cint_load_balancing_get_hw_cfg.c
 */
/*
 * This cint is used to get the relevant load balancing HW configuration from a device to be used by the offline load balancing keys simulation.
 * Note: the load_blancing_sim_hw_cfg_t structure and some other values which are placed inside the cint_load_balancing_sim.c file are used in the cint below.
 */

/*
 * Return the polynomial value given the polynomial enumeration value.
 */
uint32
load_balancing_sim_hw_cfg_polynomial_value_get(bcm_switch_hash_config_t poly_enum)
{
    switch(poly_enum)
    {
        case BCM_HASH_CONFIG_CRC16_0x1015d:
            return 0x015d;
        case BCM_HASH_CONFIG_CRC16_0x100d7:
            return 0x00d7;
        case BCM_HASH_CONFIG_CRC16_0x10039:
            return 0x0039;
        case BCM_HASH_CONFIG_CRC16_0x10ac5:
            return 0x0ac5;
        case BCM_HASH_CONFIG_CRC16_0x109e7:
            return 0x09e7;
        case BCM_HASH_CONFIG_CRC16_0x10939:
            return 0x0939;
        case BCM_HASH_CONFIG_CRC16_0x12105:
            return 0x2105;
        case BCM_HASH_CONFIG_CRC16_0x1203d:
            return 0x203d;
        default:
            return 0;
    }
}

/**
 * \brief
 *   Fills a structure that contains relevant HW fields that are used by the offline load balancing simulator to
 *   generate load balancing keys.
 *   This cint should be run on a device
 * \param [in] unit -
 *   The unit number.
 * \param [out] hw_cfg -
 *   returns all the load balancing relevant HW values that are configured in the device.
 * \retval Zero if no error was detected
 * \retval Negative if error was detected.
 */
int
load_balancing_sim_hw_cfg_get(
        int unit,
        load_blancing_sim_hw_cfg_t *hw_cfg)
{
    int rv = BCM_E_NONE;
    /*
     * LB keys client list
     */
    int lb_clients[5] = {bcmSwitchNwkHashConfig,  bcmSwitchTrunkHashConfig, bcmSwitchECMPHashConfig, bcmSwitchECMPSecondHierHashConfig, bcmSwitchECMPThirdHierHashConfig };
    int val;
    int client_iter;
    bcm_switch_control_key_t control_key;
    bcm_switch_control_info_t control_info;

    /*
     * Update the device type
     */
    if (is_device_or_above(unit, JERICHO2P))
    {
        hw_cfg->device = DEVICE_TYPE_JERICHO2P;
    }
    else if (is_device_or_above(unit, QUMRAN2A_B0))
    {
        hw_cfg->device = DEVICE_TYPE_QUMRAN2_B0;
    }
    else if (is_device_or_above(unit, QUMRAN2A))
    {
        hw_cfg->device = DEVICE_TYPE_QUMRAN2_A0;
    }
    else if (is_device_or_above(unit, JERICHO2C))
    {
        hw_cfg->device = DEVICE_TYPE_JERICHO2C;
    }
    else if (is_device_or_above(unit,JERICHO2_B0))
    {
        hw_cfg->device = DEVICE_TYPE_JERICHO2_B0;
    }
    else
    {
        hw_cfg->device  = DEVICE_TYPE_JERICHO2_A0;
    }

    /*
     * Update parser seed
     */
    rv = bcm_switch_control_get(unit,bcmSwitchParserHashSeed,&val);
    if(rv != BCM_E_NONE)
    {
        printf("bcm_switch_control_get bcmSwitchParserHashSeed failed \n");
        return rv;
    }
    hw_cfg->parser_seed = val;

    /*
     * Update MPLS hash function seed
     */
    rv = bcm_switch_control_get(unit,bcmSwitchMplsStack0HashSeed,&val);
    if(rv != BCM_E_NONE)
    {
        printf("bcm_switch_control_get bcmSwitchMplsStack0HashSeed failed \n");
        return rv;
    }
    hw_cfg->mpls_seed[0] = val;
    /*
     * Second MPLS stack isn't supported and remain in the defult value of 0
     */
    hw_cfg->mpls_seed[1] = 0;

    /*
     * Update clients LB configuration
     */
    for(client_iter = 0; client_iter < NOF_CLIENTS; client_iter++)
    {
        /*
         * Update client CRC function
         */
        rv = bcm_switch_control_get(unit,lb_clients[client_iter],&val);
        if(rv != BCM_E_NONE)
        {
            printf("bcm_switch_control_get bcmSwitch-Config client %d failed \n",client_iter);
            return rv;
        }
        hw_cfg->clients_polynomial[client_iter] = load_balancing_sim_hw_cfg_polynomial_value_get(val);

        /*
         * Update client CRC function seed
         */
        control_key.type = bcmSwitchHashSeed;
        control_key.index = val;
        rv = bcm_switch_control_indexed_get(unit,control_key, &control_info);
        if(rv != BCM_E_NONE)
        {
            printf("bcm_switch_control_indexed_get bcmSwitchHashSeed client %d failed \n",client_iter);
            return rv;
        }
        hw_cfg->clients_polynomial_seeds[client_iter] = control_info.value;

        /*
         * Update client CRC function input selection
         */
        control_key.type = bcmSwitchLayerRecordModeSelection;
        control_key.index = lb_clients[client_iter];
        rv = bcm_switch_control_indexed_get(unit,control_key, &control_info);
        if(rv != BCM_E_NONE)
        {
            printf("bcm_switch_control_indexed_get bcmSwitchHashSeed client %d failed \n",client_iter);
            return rv;
        }
        hw_cfg->clients_16_crc_selection[client_iter] = control_info.value;
    }

    return rv;

}
