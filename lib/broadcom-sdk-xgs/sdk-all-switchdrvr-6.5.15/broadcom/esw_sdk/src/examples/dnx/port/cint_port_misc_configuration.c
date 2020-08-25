int add_mact_entry(
    int unit,
    int mac_id,
    uint16 vid,
    int phy_port)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;
    bcm_gport_t port;

    bcm_mac_t my_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    my_mac[5] = mac_id % 256;

    BCM_GPORT_SYSTEM_PORT_ID_SET(port, phy_port);

    bcm_l2_addr_t_init(&l2addr, my_mac, vid);

    l2addr.port = port;
    l2addr.vid = vid;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.tgid = 2;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add(), rv=%d.\n", rv);
        return rv;
    }

    return rv;
}

int disable_same_if_filter_all(
    int unit,
    int port)
{
    int rv = BCM_E_NONE;
    int flags = 0;
    int trap_id;
    bcm_rx_trap_config_t config;
    bcm_rx_trap_t trap = bcmRxTrapSameInterface;

    rv = bcm_port_control_set(unit, port, bcmPortControlBridge, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_control_set(), rv=%d.\n", rv);
        return rv;
    }

    bcm_rx_trap_config_t_init(&config);

    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.trap_strength = 0;

    rv = bcm_rx_trap_type_get(unit, flags, trap, &trap_id);
    if (rv != BCM_E_NONE && rv != BCM_E_CONFIG)
    {
        printf("Error, bcm_rx_trap_type_get(), rv=%d.\n", rv);
        return rv;
    }

    rv = bcm_rx_trap_set(unit, trap_id, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_set(), rv=%d.\n", rv);
        return rv;
    }

    return rv;
}

int port_set_raw_header(
    int unit,
    int port)
{
    int rv = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    key.index = 2;
    key.type = bcmSwitchPortHeaderType;
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_RAW;

    rv = bcm_switch_control_indexed_port_set(unit, port, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_indexed_port_set(), rv=%d.\n", rv);
        return rv;
    }

    return rv;
}


int pfc_port_control_set(
    int unit,
    int port)
{
    return bcm_port_control_set(unit, port, bcmPortControlPFCReceive, 1);
}

/**
* \brief - in order to have RX time-stamp of the packet, user must enable the timestamp in the port.
*          should be made while port is disabled.
*          assuming linkScan is enabled.
*/
int rx_time_stamp_enable(
    int unit,
    int in_port)    
{
    int rv = BCM_E_NONE;
    bcm_port_phy_timesync_config_t timesync_phy_config;

    bcm_port_phy_timesync_config_t_init(&timesync_phy_config);
    /** flag BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE is relevant only for 1588 feature. */
    /** If you don't use flag BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE, two step timestamping is enabled in the port. */
    /** If you only care about the Rx TS, you can only use flag BCM_PORT_PHY_TIMESYNC_ENABLE. */
    timesync_phy_config.flags = (BCM_PORT_PHY_TIMESYNC_ENABLE | BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE);
    rv = bcm_port_enable_set(unit, in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_enable_set\n");
        return rv;
    }    
    rv = bcm_port_phy_timesync_config_set(unit, in_port, &timesync_phy_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_phy_timesync_config_set\n");
        return rv;
    }    
    rv = bcm_port_enable_set(unit, in_port, 1);    
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_enable_set\n");
        return rv;
    }       

    return rv;
}

