/*~~~~~~~~~~~~~~~~~~~~~~~~~~NIF: Interface Configuration~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_dynamic_port_add_remove.c,v 1.1 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        cint_dynamic_port_add_remove.c
 * Purpose:     Example of adding and removing ports
 *
 * Example includes:
 *  o     Removing one port configuration
 *  o     Adding one port configuration
 *  o     Removing several ports and Adding several new ports configuration
 *  o     Changing number of lanes for a given port
 *
 * It is assumed diag_init is executed.
 *

 *
 * Usage Example:
 * The example below will remove port 13 and then will add it back as XE on link 88:
 *
    nif sts
    cd ../../../../regress/bcm
    c
    cint_reset();
    exit;
    c ../../src/examples/dnx/port/cint_dynamic_port_add_remove.c
    c ../../src/examples/sand/cint_sand_nif_phy_loopback.c


    c
    print "remove port 13";
    int unit;
    bcm_port_t port;
    uint32 flags;
    unit = 0;
    port = 13;
    flags = 0;
    print cint_dyn_port_remove_port_full_example(unit, port, flags);
    exit;
    nif sts

    c
    print "Add port 13 10G on XE88";
    int unit;
    bcm_port_t port;
    bcm_pbmp_t phy_pbmp;
    int rate_mhz;
    BCM_PBMP_PORT_SET(phy_pbmp, 88);
    unit = 0;
    port = 13;
    rate_mhz = 10000;
    print cint_dyn_port_add_ixia_port(unit, port , phy_pbmp, rate_mhz);
    exit;
    nif sts

    c
    print "Configure local snake";
    print bcm_port_force_forward_set(0,13,13,1);
    exit;

 * The example below will remove port 1 and then will add it back as CDG on links 0-7:
 *

    nif sts
    cd ../../../../regress/bcm
    c
    cint_reset();
    exit;
    c ../../src/examples/dnx/port/cint_dynamic_port_add_remove.c
    c ../../src/examples/sand/cint_sand_nif_phy_loopback.c

    c
    print "remove port 1";
    int unit;
    bcm_port_t port;
    uint32 flags;
    unit = 0;
    port = 1;
    flags = 0;
    print cint_dyn_port_remove_port_full_example(unit, port, flags);
    exit;
     nif sts

    c
    print "Add port 1 400G on CDG0";
    int unit;
    int rate_mhz;
    bcm_port_t port;
    bcm_pbmp_t phy_pbmp;
    BCM_PBMP_PORT_SET(phy_pbmp, 0);
    BCM_PBMP_PORT_ADD (phy_pbmp, 1);
    BCM_PBMP_PORT_ADD (phy_pbmp, 2);
    BCM_PBMP_PORT_ADD (phy_pbmp, 3);
    BCM_PBMP_PORT_ADD (phy_pbmp, 4);
    BCM_PBMP_PORT_ADD (phy_pbmp, 5);
    BCM_PBMP_PORT_ADD (phy_pbmp, 6);
    BCM_PBMP_PORT_ADD (phy_pbmp, 7);
    unit = 0;
    port = 1;
    rate_mhz = 400000;
    print cint_dyn_port_add_ixia_port(unit, port , phy_pbmp, rate_mhz);
    exit;
    nif sts

    c
    print "Configure local snake";
    print bcm_port_force_forward_set(0,1,1,1);
    exit;

 *
 */

int random_lane_num;
int serdes_allocation_temp[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
int serdes_allocation_final[12];

typedef enum dfe_mode_t
{
    dfe_on,
    dfe_lp,
    dfe_none
};

typedef enum medium_type_t
{
    backplane,
    copper_cable,
    optic
};

typedef enum port_mapping_resource_t
{
    logical_ports,
    tm_ports,
    port_base_q_pairs,
    nif_phy_lanes
};


/* Function:
 *      cint_dyn_port_config_ports_properties
 * Purpose:
 *      Reconfigure NIF
 */

int
cint_dyn_port_resource_fill(
    int unit,
    bcm_gport_t port,
    dfe_mode_t dfe_mode,
    medium_type_t medium_type,
    bcm_port_resource_t * resource)
{
    int rv = BCM_E_NONE;

    /*
     * Set resource
     */
    rv = bcm_port_resource_default_get(unit, port, 0, resource);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_port_resource_default_get, unit=%d, port=%d\n", unit, port);
        return rv;
    }
    /*
     * Fill the lane_config word
     */
    switch (medium_type)
    {
        case backplane:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_CLEAR(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                         BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE);
            break;
        case copper_cable:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_CLEAR(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                         BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE);
            break;
        case optic:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_CLEAR(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                         BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS);
            break;
        default:
            printf("Unsupported medium type: $medium_type\n");
            return BCM_E_PARAM;
            break;
    }

    switch (dfe_mode)
    {
        case dfe_on:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
            break;
        case dfe_lp:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_SET(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
            break;
        case dfe_none:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_CLEAR(resource->phy_lane_config);
            break;
        default:
            printf("Unsupported dfe node: $dfe_mode\n");
            return BCM_E_PARAM;
            break;
    }

    return BCM_E_NONE;
}

/**
 * \brief - Take PP port soc-properties and call the right BCM PP port APIs accordingly.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

int
cint_dyn_port_dnx_port_dyanamic_add_packet_processing_properties_set(
    int unit,
    bcm_port_t port,
    int header_type_in,
    int header_type_out)
{
    uint32 class_id;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    uint32 flags;
    int rv = BCM_E_NONE;

    flags = BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD;
    class_id = port;

    /*
     * Set incoming header type 
     */
    key.type = bcmSwitchPortHeaderType;
    key.index = 1;      /* 0 = Both, 1 = direction IN, 2 = direction OUT */
    value.value = header_type_in;

    rv = bcm_switch_control_indexed_port_set(unit, port, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_switch_control_indexed_port_set when setting incoming header type\n");
        return rv;
    }

    /*
     * Set outgoing header type 
     */
    key.type = bcmSwitchPortHeaderType;
    key.index = 2;      /* 0 = Both, 1 = direction IN, 2 = direction OUT */
    value.value = header_type_out;

    rv = bcm_switch_control_indexed_port_set(unit, port, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_switch_control_indexed_port_set when setting outgoing header type\n");
        return rv;
    }

    /** Configure vlan domain per port*/
    rv = bcm_port_class_set(unit, port, bcmPortClassId, class_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set\n");
        return rv;
    }

    /** Set VLAN-membership-namespace per PP port */
    rv = bcm_port_class_set(unit, port, bcmPortClassVlanMember, class_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set\n");
        return rv;
    }

    /** Set vlan port membership */
    rv = bcm_vlan_gport_add(unit, BCM_VLAN_DEFAULT, port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    /** Configure inlif same interface*/
    rv = bcm_port_control_set(unit, port, bcmPortControlBridge, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_control_set\n");
        return rv;
    }

    /** Configure vlan for untag cases*/
    rv = bcm_port_untagged_vlan_set(unit, port, BCM_VLAN_DEFAULT);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
    }

    /** Enable learning */
    rv = bcm_port_learn_set(unit, port, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_learn_set\n");
        return rv;
    }

    /** Configure default stp state as FORWARD per ethernet port */
    rv = bcm_port_stp_set(unit, port, BCM_STG_STP_FORWARD);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_stp_set\n");
        return rv;
    }

    /** Set port default-LIF to simple bridge:  */
    rv = bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressMissDrop, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_control_port_set\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
* Function:
*      cint_dyn_port_remove_port_full_example
* Purpose:
*      Reconfigure single port
* Parameters:
*      unit            - (IN)  Unit number.
*      port            - (IN)  Logical port # - must be a port which already defined by 'ucode_port' soc property.
*      flags           - (IN)  flags
* Returns:
*      BCM_E_NONE     No Error
*      BCM_E_XXX      Error occurred
*
*
* Assumes port is valid and initialized
* Stages ID is according to the ID when adding port:
* stage 6: disable the port
* stage 5: distroy application reference
* stage 3: configure egr and sch shapers back to zero
* stage 1: remove the port
*
*/
int
cint_dyn_port_remove_port_full_example(
    int unit,
    int port,
    uint32 flags)
{
    int rv = BCM_E_NONE;
    int commit_changes = 1;
    int is_remove = 1;
    int is_last_in_if = 1;
    int loopback;

    /*
     * stage 6, disable port
     */

    /*Disable port only if port is last_in_if (i.e. not sharing the interface with other channelized port*/
    rv = cint_dyn_port_is_last_in_if(unit, port, &is_last_in_if);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_dyn_port_is_last_in_if: unit=%d, port=%d\n", unit, port);
        return rv;
    }


    if (is_last_in_if)
    {
        /*If phy loopback is enabled - need to first open loopback and restore lane swap*/
        rv = bcm_port_loopback_get (unit, port, &loopback);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_port_loopback_get: unit=%d, port=%d\n", unit, port);
            return rv;
        }
        if (loopback == BCM_PORT_LOOPBACK_PHY)
        {
            /*open loopback and restore lane swap*/
            rv = cint_port_phy_loopback_clear(unit, port);
            if (rv != BCM_E_NONE)
            {
                printf("Error in cint_port_phy_loopback_clear: unit=%d, port=%d\n", unit, port);
                return rv;
            }
        }

        rv = bcm_port_enable_set(unit, port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_dnx_port_enable_set: unit=%d, port=%d\n", unit, port);
            return rv;
        }
    }

    /*
     * stage 5, application reference (JR2 and beyond)
     */
    rv = appl_dnx_dynamic_port_destroy(unit, port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in appl_dnx_dynamic_port_destroy: unit=%d, port=%d\n", unit, port);
        return rv;
    }

    /*
     * stage 3, before removing the port - set shapers rates to 0 
     */
    /*
     * setting egr shapers rates to zero 
     */
    rv = dnx_tune_egr_ofp_rate_set(unit, port, 0, commit_changes, is_remove);      /* egr expects speed in Kbits/sec */
    if (rv != BCM_E_NONE)
    {
        printf("Error, dnx_tune_egr_ofp_rate_set, unit=%d, port=%d\n", unit, port);
        return rv;
    }
    /*
     * setting scheduler rates back to zero
     */
    rv = dnx_tune_scheduler_port_rates_set(unit, port, 0, 0, commit_changes, is_remove);      /* sch expects speed in Kbits/sec */
    if (rv != BCM_E_NONE)
    {
        printf("Error, dnx_tune_scheduler_port_rates_set, unit=%d, port=%d\n", unit, port);
        return rv;
    }

    /*
     * stage 1, remove port
     */
    rv = bcm_port_remove(unit, port, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_remove\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_dyn_port_add_port_full_example
 * Purpose:
 *      Full example to add port and set relevant shapers
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      port            - (IN)  Logical port to add.
 *      mapping_info    - (IN)  Logical port mapping info
 *      interface_info  - (IN)  physical port info
 *      resource        - (IN)  NIF resource (speed, link training and FEC.
 *                              All other struct fields, including FW control word - are ignored
 *      dfe_mode        - (IN)  dfe mode: on, low power or off
 *      medium_type     - (IN)  medium type: backplane, copper cable or optic
 *      header_type_in  - (IN)  incomming header type
 *      header_type_out - (IN)  outgoing header type
 *      flags           - (IN)
 *
 * Returns:
 *      BCM_E_NONE     No Error
 *      BCM_E_XXX      Error occurred
 *
 * stage 1: Add new port
 * stage 2: Configure NIF
 * stage 3: configure egr and sch shapers
 * stage 4: Configure PP properties
 * stage 5: configure application reference
 * stage 6: enable the port
 *
 */

int
cint_dyn_port_add_port_full_example(
    int unit,
    bcm_port_t port,
    bcm_port_mapping_info_t mapping_info,
    bcm_port_interface_info_t interface_info,
    bcm_port_resource_t resource,
    dfe_mode_t dfe_mode,
    medium_type_t medium_type,
    int header_type_in,
    int header_type_out,
    uint32 flags)
{
    int port_total_rate_mhz, nof_lanes;
    int rv = BCM_E_NONE;
    bcm_gport_t egress_q_gport;
    bcm_port_phy_tx_t tx_taps;
    int commit_changes = 1;
    int is_remove = 0;

    /*
     * stage 1 - adding the port 
     */
    rv = bcm_port_add(unit, port, flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_add\n");
        return rv;
    }

    /*
     * stage 2 - configure NIF properties 
     */
    /*
     * Set resource
     */
    rv = cint_dyn_port_resource_fill(unit, port, dfe_mode, medium_type, &resource);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_dyn_port_resource_fill,unit $unit logical_port: %x, $rv\n", port);
        return rv;
    }

    rv = bcm_port_resource_set(unit, port, resource);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_port_resource_set, unit=%d, port=%d\n", unit, port);
        return rv;
    }

    /*
     * If no link training set tx taps
     */
    if (resource.link_training == 0)
    {
        /*
         * For NRZ - default Tx taps are OK.Only in PAM4 need to set Tx taps for the Ixia 400G setup
         */
        BCM_PBMP_COUNT(interface_info.phy_pbmp, nof_lanes);
        if (resource.speed/nof_lanes > 28125)
        {
            tx_taps.pre2 = 0;
            tx_taps.pre = -24;
            tx_taps.main = 132;
            tx_taps.post = -12;
            tx_taps.post2 = 0;
            tx_taps.post3 = 0;
            tx_taps.tx_tap_mode = bcmPortPhyTxTapMode6Tap;
            tx_taps.signalling_mode = bcmPortPhySignallingModePAM4;
            rv = bcm_port_phy_tx_set(unit, port, &tx_taps);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_port_phy_tx_set, unit=%d, port=%d\n", unit, port);
                return rv;
            }

            /* ER channels need to enable TX precoding */
            if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource.phy_lane_config))
            {
                rv = bcm_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE, 1);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, bcm_port_phy_control_set, unit=%d, port=%d\n", unit, port);
                    return rv;
                }

                rv = bcm_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE, 1);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, bcm_port_phy_control_set, unit=%d, port=%d\n", unit, port);
                    return rv;
                }
            }
        }
    }

    /*
     * stage 3 - configure egr and sch shapers 
     */
    switch (interface_info.interface)
    {
        case BCM_PORT_IF_NIF_ETH:
            port_total_rate_mhz = resource.speed;       /* ETH: rate is port rate */
            break;
        case BCM_PORT_IF_ILKN:
            port_total_rate_mhz = nof_lanes * (resource.speed); /* ILKN: rate is link rate */
            break;
        case BCM_PORT_IF_CPU:  /* 1G */
            port_total_rate_mhz = 1000;
            break;
        default:
            printf("Unsupported interface: $interface_info.interface\n");
            return BCM_E_PARAM;
            break;
    }

    /*
     * No need for speedup, as it s done inside dnx_tune_egr_ofp_rate_set 
     */
    rv = dnx_tune_egr_ofp_rate_set(unit, port, port_total_rate_mhz * 1000, commit_changes, is_remove); /* egr expects speed in
                                                                                                        * Kbits/sec */
    if (rv != BCM_E_NONE)
    {
        printf("Error, dnx_tune_egr_ofp_rate_set, unit=%d, port=%d\n", unit, port);
        return rv;
    }
    /*
     * No need for speedup, as it s done inside dnx_tune_scheduler_port_rates_set 
     */
    rv = dnx_tune_scheduler_port_rates_set(unit, port, 0, port_total_rate_mhz * 1000, commit_changes, is_remove);     /* sch expects
                                                                                                         * speed in
                                                                                                         * Kbits/sec */
    if (rv != BCM_E_NONE)
    {
        printf("Error, dnx_tune_scheduler_port_rates_set, unit=%d, port=%d\n", unit, port);
        return rv;
    }

    /*
     * stage 4 - PP properties
     */
    rv = cint_dyn_port_dnx_port_dyanamic_add_packet_processing_properties_set(unit, port, header_type_in,
                                                                              header_type_out);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_port_enable_set: unit=%d, port=%d\n", unit, port);
        return rv;
    }

    /*
     * stage 5 - application reference 
     */
    rv = appl_dnx_dynamic_port_create(unit, port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_port_enable_set: unit=%d, port=%d\n", unit, port);
        return rv;
    }

    /*
     * stage 6 - enable the port
     */
    rv = bcm_port_enable_set(unit, port, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_port_enable_set: unit=%d, port=%d\n", unit, port);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_dyn_port_add_ports_full_example
 * Purpose:
 *      Full example to add ports and set relevant shapers
 *      This example is used to configure port resouces for multi ports
 *      simultaneously using API bcm_port_resource_multi_set. This example
 *      can provide better port VCO assignment when configuring multi ports
 *      on one PM.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      nof_ports       - (IN)  Number of ports
 *      port_list       - (IN)  Ports array
 *      mapping_info    - (IN)  Logical port mapping info
 *      interface_info  - (IN)  physical port info
 *      resource        - (IN)  NIF resource (speed, link training and FEC.
 *                              All other struct fields, including FW control word - are ignored
 *      dfe_mode        - (IN)  dfe mode: on, low power or off
 *      medium_type     - (IN)  medium type: backplane, copper cable or optic
 *      header_type_in  - (IN)  incomming header type
 *      header_type_out - (IN)  outgoing header type
 *      flags           - (IN)  flags for bcm_port_add API
 *
 * Returns:
 *      BCM_E_NONE     No Error
 *      BCM_E_XXX      Error occurred
 *
 * stage 1: Add new ports
 * stage 2: Configure NIF resources
 * stage 3: configure egr and sch shapers
 * stage 4: Configure PP properties
 * stage 5: configure application reference
 * stage 6: enable the ports
 *
 */

int
cint_dyn_port_add_ports_full_example(
    int unit,
    int nof_ports,
    bcm_port_t *port_list,
    bcm_port_mapping_info_t *mapping_info,
    bcm_port_interface_info_t *interface_info,
    bcm_port_resource_t *resource,
    dfe_mode_t *dfe_mode,
    medium_type_t *medium_type,
    int *header_type_in,
    int *header_type_out,
    uint32 *flags)
{
    int port_total_rate_mhz, nof_lanes;
    int rv = BCM_E_NONE;
    bcm_gport_t egress_q_gport;
    bcm_port_phy_tx_t tx_taps;
    int commit_changes = 1, port_index;
    int is_remove = 0;

    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        /*
         * stage 1 - adding the port
         */
        rv = bcm_port_add(unit, port_list[port_index], flags[port_index],
                          &interface_info[port_index], &mapping_info[port_index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_add\n");
            return rv;
        }
        /*
         * stage 2 - configure NIF properties
         */
        /*
         * Set resource
         */
        rv = cint_dyn_port_resource_fill(unit, port_list[port_index],
                                         dfe_mode[port_index], medium_type[port_index], &resource[port_index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_dyn_port_resource_fill,unit $unit logical_port: %x, $rv\n", port_list[port_index]);
            return rv;
        }
    }

    rv = bcm_port_resource_multi_set(unit, nof_ports, resource);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_resource_multi_set, unit=%d, port=%d\n", unit, port_list[port_index]);
        return rv;
    }

    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        /*
         * If no link training set tx taps
         */
        if (resource[port_index].link_training == 0)
        {
            /*
             * For NRZ - default Tx taps are OK.Only in PAM4 need to set Tx taps for the Ixia 400G setup
             */
            BCM_PBMP_COUNT(interface_info[port_index].phy_pbmp, nof_lanes);
            if (resource[port_index].speed/nof_lanes > 28125)
            {
                tx_taps.pre2 = 0;
                tx_taps.pre = -24;
                tx_taps.main = 132;
                tx_taps.post = -12;
                tx_taps.post2 = 0;
                tx_taps.post3 = 0;
                tx_taps.tx_tap_mode = bcmPortPhyTxTapMode6Tap;
                tx_taps.signalling_mode = bcmPortPhySignallingModePAM4;
                rv = bcm_port_phy_tx_set(unit, port_list[port_index], &tx_taps);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, bcm_port_phy_tx_set, unit=%d, port=%d\n", unit, port_list[port_index]);
                    return rv;
                }

                /* ER channels need to enable TX precoding */
                if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource[port_index].phy_lane_config))
                {
                    rv = bcm_port_phy_control_set(unit, port_list[port_index], BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE, 1);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error, bcm_port_phy_control_set, unit=%d, port=%d\n", unit, port_list[port_index]);
                        return rv;
                    }

                    rv = bcm_port_phy_control_set(unit, port_list[port_index], BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE, 1);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error, bcm_port_phy_control_set, unit=%d, port=%d\n", unit, port_list[port_index]);
                        return rv;
                    }
                }
            }
        }

        /*
         * stage 3 - configure egr and sch shapers
         */
        switch (interface_info[port_index].interface)
        {
            case BCM_PORT_IF_NIF_ETH:
                /** ETH: rate is port rate */
                port_total_rate_mhz = resource[port_index].speed;
                break;
            case BCM_PORT_IF_ILKN:
                /** ILKN: rate is link rate */
                port_total_rate_mhz = nof_lanes * (resource[port_index].speed);
                break;
            case BCM_PORT_IF_CPU:
                /** 1G */
                port_total_rate_mhz = 1000;
                break;
            default:
                printf("Unsupported interface: $interface_info.interface\n");
                return BCM_E_PARAM;
                break;
        }

        /*
         * No need for speedup, as it s done inside dnx_tune_egr_ofp_rate_set
         */
        /* egr expects speed in Kbits/sec */
        rv = dnx_tune_egr_ofp_rate_set(unit, port_list[port_index], port_total_rate_mhz * 1000, commit_changes, is_remove);
        if (rv != BCM_E_NONE)
        {
            printf("Error, dnx_tune_egr_ofp_rate_set, unit=%d, port=%d\n", unit, port_list[port_index]);
            return rv;
        }
        /*
         * No need for speedup, as it s done inside dnx_tune_scheduler_port_rates_set
         */
        /* sch expects speed in Kbits/sec */
        rv = dnx_tune_scheduler_port_rates_set(unit, port_list[port_index], 0, port_total_rate_mhz * 1000, commit_changes, is_remove);
        if (rv != BCM_E_NONE)
        {
            printf("Error, dnx_tune_scheduler_port_rates_set, unit=%d, port=%d\n", unit, port_list[port_index]);
            return rv;
        }

        /*
         * stage 4 - PP properties
         */
        rv = cint_dyn_port_dnx_port_dyanamic_add_packet_processing_properties_set(unit, port_list[port_index], header_type_in[port_index],
                                                                                  header_type_out[port_index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_dnx_port_enable_set: unit=%d, port=%d\n", unit, port_list[port_index]);
            return rv;
        }

        /*
         * stage 5 - application reference
         */
        rv = appl_dnx_dynamic_port_create(unit, port_list[port_index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_dnx_port_enable_set: unit=%d, port=%d\n", unit, port_list[port_index]);
            return rv;
        }

        /*
         * stage 6 - enable the port
         */
        rv = bcm_port_enable_set(unit, port_list[port_index], 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_dnx_port_enable_set: unit=%d, port=%d\n", unit, port_list[port_index]);
            return rv;
        }
    }
    return BCM_E_NONE;
}

/* The below cint function is an example for adding Ixia port
 * Port description:
 *  - Logical port, phys and rate - user decision (input arguments)
 *  - Ixia requirments:
 *     - Link training: 'off'
 *     - DFE: 'Off' for 10G. 'On' for 100G and 400G
 *     - medium type: 'optic' for 10G. 'copper' for 100G and 400G
 *     - NIF interface: ETH
 *     - Header type (in and out): ETH
 *  - FEC: default according to speed
 *  - TM port: same id as the logical port
 *  - Priorities: 2
 *
 * */
int
cint_dyn_port_add_ixia_port(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phy_pbmp,
    int rate_mhz)
{

    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    int first_phy;
    int nof_links_per_core;
    uint32 nof_cores, nof_links;
    bcm_port_resource_t resource;
    dfe_mode_t dfe_mode;
    medium_type_t medium_type;
    int header_type_in, header_type_out;
    uint32 flags = 0;
    int rv = 0;

    /** init vars */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    bcm_port_mapping_info_t_init(&mapping_info);
    bcm_port_resource_t_init(resource);

    /** Configuring  interface_info*/
    interface_info.interface = BCM_PORT_IF_NIF_ETH;
    BCM_PBMP_ASSIGN(interface_info.phy_pbmp, phy_pbmp);

    /** Configuring  mapping_info*/
    BCM_PBMP_ITER(phy_pbmp, first_phy)
    {
        break;
    }
    nof_links = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
    nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    nof_links_per_core = nof_links / nof_cores;
    mapping_info.core = first_phy / nof_links_per_core;
    mapping_info.channel = 0;
    mapping_info.tm_port = port;
    mapping_info.num_priorities = 2;

    /** Configuring NIF resource*/
    resource.port = port;
    resource.speed = rate_mhz;
    resource.fec_type = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    resource.link_training = 0;
    resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    /*
     * We can't pass default inside phy_lane_config so we pass 2 of them (dfe_mode and medium type) - specifically
     */
    if (rate_mhz == 10 * 1000)  /* Ixia requirments for 10G: Optic, no DFE, no link training */
    {
        dfe_mode = dfe_none;
        medium_type = optic;
    }
    else if ((rate_mhz == 100 * 1000) || (rate_mhz == 400 * 1000))      /* Ixia requirments for 100G or 400G: DFE, no
                                                                         * link training */
    {
        dfe_mode = dfe_on;
        medium_type = copper_cable;
    }

    /*
     * Configuring PP properties
     */
    header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;

    /*
     * Add the port
     */
    rv = cint_dyn_port_add_port_full_example(unit, port, mapping_info, interface_info, resource,
                                             dfe_mode, medium_type, header_type_in, header_type_out, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_dyn_port_add_port_full_example,unit $unit logical_port: %x, $rv\n", logical_port);
        return rv;
    }
    return BCM_E_NONE;

}

/* The below cint function is an example for adding normal eth port
 *  Port description:
 *  - Logical port, phys and rate - user decision (input arguments)
 *  - SerDes configuration to suite simple loopback or p2p cable connected port:
 *     - Link training: 'on'
 *     - DFE: 'on'
 *     - medium type: 'copper'
 *  - NIF interface: ETH
 *  - Header type (in and out): ETH
 *  - FEC: default according to speed
 *  - TM port: same id as the logical port
 *  - Priorities: 2
 *
 * */
int
cint_dyn_port_add_eth_port(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phy_pbmp,
    int rate_mhz)
{

    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    int first_phy;
    int nof_links_per_core;
    uint32 nof_cores, nof_links;
    bcm_port_resource_t resource;
    dfe_mode_t dfe_mode;
    medium_type_t medium_type;
    int header_type_in, header_type_out;
    uint32 flags = 0;
    int rv = 0;

    /** init vars */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    bcm_port_mapping_info_t_init(&mapping_info);
    bcm_port_resource_t_init(resource);

    /** Configuring  interface_info*/
    interface_info.interface = BCM_PORT_IF_NIF_ETH;
    BCM_PBMP_ASSIGN(interface_info.phy_pbmp, phy_pbmp);

    /** Configuring  mapping_info*/
    BCM_PBMP_ITER(phy_pbmp, first_phy)
    {
        break;
    }
    nof_links = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
    nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    nof_links_per_core = nof_links / nof_cores;
    mapping_info.core = first_phy / nof_links_per_core;
    mapping_info.channel = 0;
    mapping_info.tm_port = port;
    mapping_info.num_priorities = 2;

    /** Configuring NIF resource*/
    resource.port = port;
    resource.speed = rate_mhz;
    resource.fec_type = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    resource.link_training = 1;
    resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    /*
     * We can't pass default inside phy_lane_config so we pass 2 of them (dfe_mode and medium type) - specifically
     */
    dfe_mode = dfe_on;
    medium_type = copper_cable;

    /*
     * Configuring PP properties
     */
    header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;

    /*
     * Add the port
     */
    rv = cint_dyn_port_add_port_full_example(unit, port, mapping_info, interface_info, resource,
                                             dfe_mode, medium_type, header_type_in, header_type_out, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_dyn_port_add_port_full_example,unit $unit logical_port: %x, $rv\n", logical_port);
        return rv;
    }
    return BCM_E_NONE;

}

/* select the serdes lane randomly */
/* serdes_allocation_final is the final selected serdes array and the length is random_lane_num */
int
cint_dyn_port_select_random_serdes(
    int unit)
{
    int count = 12;
    int random_lane_index;
    int i, j, k;

    random_lane_num = sal_rand() % 12;
    if (random_lane_num == 0)
    {
        random_lane_num = 1;
    }
    printf("The random_lane_num %d is \n", random_lane_num);

    for (i = 0; i < random_lane_num; i++)
    {
        random_lane_index = sal_rand() % count;
        printf("The random_lane_index %d is selected\n", random_lane_index);

        serdes_allocation_final[i] = serdes_allocation_temp[random_lane_index];
        for (j = random_lane_index; j < count - 1; j++)
        {
            serdes_allocation_temp[j] = serdes_allocation_temp[j + 1];
        }

        count--;
    }

    for (k = 0; k < random_lane_num; k++)
    {
        printf("Each serdes in serdes_allocation_final is %d\n", serdes_allocation_final[k]);
    }

    return BCM_E_NONE;
}

/* add the channelized ILKN port */
int
cint_dyn_port_add_channelized_ilkn_port(
    int unit,
    int core_id,
    int phy_id,
    int local_port,
    int tm_port,
    int interface_id,
    int channel_num,
    int num_priorities,
    int num_lanes,
    int is_phy_pbmp)
{
    int rv = 0;
    int i, flag = 0, logical_port;
    int is_first = 1;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_pbmp_t pbmp_add;
    bcm_pbmp_t pbmp;
    int rate_mhz;

    BCM_PBMP_CLEAR(pbmp);
    for (i = 0; i < num_lanes; i++)
    {
        BCM_PBMP_PORT_ADD(pbmp, serdes_allocation_final[i]);
    }

    for (i = 0; i <= (channel_num - 1); i++)
    {
        logical_port = local_port + i;

        mapping_info.channel = i;
        mapping_info.core = core_id;
        mapping_info.num_priorities = num_priorities;
        mapping_info.tm_port = logical_port;

        interface_info.interface = BCM_PORT_IF_ILKN;
        interface_info.interface_id = interface_id;

        if (is_phy_pbmp)
        {
            interface_info.phy_pbmp = pbmp;
            flag = BCM_PORT_ADD_CONFIG_CHANNELIZED | BCM_PORT_ADD_USE_PHY_PBMP;
        }
        else
        {
            interface_info.phy_port = phy_id;
            interface_info.num_lanes = num_lanes;
            flag = BCM_PORT_ADD_CONFIG_CHANNELIZED;
        }

        rv = cint_dyn_port_add_port_full_example(unit, logical_port, mapping_info, interface_info, flag, rate_mhz);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_dyn_port_add_port_full_example,unit $unit logical_port: %x, $rv\n", logical_port);
            return rv;
        }

        /*
         * stage 2 - rates
         */
        BCM_PBMP_CLEAR(pbmp_add);
        BCM_PBMP_PORT_ADD(pbmp_add, logical_port);
        rate_mhz = (is_first ? 125 * 100 : 1 * 1000);

        rv = config_ports_rate(unit, pbmp_add, rate_mhz, BCM_SWITCH_PORT_HEADER_TYPE_ETH, TRUE);        /* After
                                                                                                         * reconfiguring 
                                                                                                         * * the port:
                                                                                                         * set the
                                                                                                         * added port
                                                                                                         * rate to its
                                                                                                         * speed */
        if (rv != BCM_E_NONE)
        {
            printf("Error, config_ports_rate, rate: $rate_mhz\n");
            return rv;
        }
        is_first = 0;
    }

    return rv;
}

/* delete all ports */
int
cint_dyn_port_remove_all(
    int unit)
{
    int rv = 0;
    int port, flag = 0;
    bcm_port_config_t config;

    rv = bcm_port_config_get(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_config_get\n");
        return rv;
    }

    BCM_PBMP_ITER(config.nif, port)
    {
        rv = cint_dyn_port_remove_port_full_example(unit, port, flag);
        if (rv)
        {
            printf("remove port fail port=%d \n", port);
            return rv;
        }
    }

    return rv;
}


/* *
 * The below cint function is an example for checking if port is last in if.
 * is_last_in_if is relevant only for channelized port. Still, the cint will work also for non-channelized ports
 * */
int
cint_dyn_port_is_last_in_if(
    int unit,
    bcm_port_t port,
    int * is_last_in_if)
{
    int rv = 0;
    unsigned int inquired_port_first_phy, other_port_first_phy;
    bcm_port_config_t config;
    bcm_port_t logical_port;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags = 0;

    *is_last_in_if = 1;

    /* Get the first phy of the inquired port */
    rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_get\n");
        return rv;
    }
    inquired_port_first_phy = interface_info.phy_port;

    /* Get all NIF ports and remove the inquired port from all-nif-ports list*/
    rv = bcm_port_config_get(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_config_get\n");
        return rv;
    }
    BCM_PBMP_PORT_REMOVE(config.nif, port);

    BCM_PBMP_ITER(config.nif, logical_port)
    {
        /* Get the first phy of the other port */
        rv = bcm_port_get(unit, logical_port, &flags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_get\n");
            return rv;
        }
        other_port_first_phy = interface_info.phy_port;

        /* Check if port is last_in_if */
        if (other_port_first_phy == inquired_port_first_phy)
        {
            /* Port is not last in if */
            *is_last_in_if = 0;
            break;
        }
    }
    return rv;
}

/**
 * Purpose: To get all free port mapping resource of a provided type.
 * Possible types are as in the enum port_mapping_resource_t:
 *   - logical_ports,
 *   - tm_ports,
 *   - port_base_q_pairs,
 *   - nif_phy_lanes
 */
int cint_dyn_port_mapping_free_resources_get (
    int unit,
    port_mapping_resource_t free_resource_type_requested,
    int *free_resources_array,
    int *nof_free_resources
)
{
    int rv = BCM_E_NONE;
    uint32 array_size, flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_config_t port_config;
    bcm_pbmp_t pbmp, phy_pbmp;
    int logical_port, phy, is_found = 0;
    int is_ilkn_over_fabric = 0;
    int resource, occupied_resource, free_resource_index = 0;

    switch (free_resource_type_requested) {
        case logical_ports:
            array_size = *(dnxc_data_get(unit, "port", "general", "nof_tm_ports", NULL));
            printf(" Get free LOGICAL ports \n");
            break;
        case tm_ports:
            array_size = *(dnxc_data_get(unit, "port", "general", "nof_tm_ports", NULL));
            printf(" Get free TM ports \n");
            break;
        case port_base_q_pairs:
            array_size = *(dnxc_data_get(unit, "egr_queuing", "params", "nof_q_pairs", NULL));
            printf(" Get free Base Q_Pairs \n");
            break;
        case nif_phy_lanes:
            array_size = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
            printf(" Get free NIF PHYs \n");
            break;
        default:
            printf("Invalid resource type required %d \n", res);
            return  BCM_E_PARAM;
    }

    int occupied_resource_array[array_size];

    /**
     * Get device port bit maps
     */
    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_config_get: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    BCM_PBMP_ASSIGN(pbmp , port_config.all);
    BCM_PBMP_REMOVE(pbmp, port_config.sfi);

    /**
     * Go over all TM ports
     */
    int resource_index = 0;
    BCM_PBMP_ITER(pbmp, logical_port) {

        rv = bcm_port_get(unit, logical_port, &flags, &interface_info, &mapping_info);
        if (BCM_FAILURE(rv)) {
            printf("bcm_port_get: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
            return rv;
        }
        switch (free_resource_type_requested) {
            case logical_ports:
                occupied_resource_array[resource_index] =        logical_port;
                resource_index++;
                break;
            case tm_ports:
                occupied_resource_array[resource_index] =        mapping_info.tm_port;
                resource_index++;
                break;
            case port_base_q_pairs:
                occupied_resource_array[resource_index] =        mapping_info.base_q_pair;
                resource_index++;
                break;
            case nif_phy_lanes:
                if (flags & BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT) {
                    continue;
                }

                BCM_PBMP_ITER(interface_info.phy_pbmp, phy) {
                    occupied_resource_array[resource_index] = phy;
                    resource_index++;
                }
                break;
            default:
                printf("Invalid resource type required %d \n", free_resource_type_requested);
                return  BCM_E_PARAM;
        }

    }

    /**
     * There is a reserved TM port
     */
    if (free_resource_type_requested == tm_ports) {
        uint32 reserved_tm_port = *(dnxc_data_get(unit, "port", "general", "reserved_tm_port", NULL));
        occupied_resource_array[resource_index] = reserved_tm_port;
        resource_index++;
    }

    int nof_occupied_resources = resource_index;
    /**
     * Go over all possible resources
     * If not found in the list of occupied resources THAN
     * add to the list of free resources
     */
    for(resource = 0; resource < array_size; resource++) {
        is_found = 0;
        for(occupied_resource = 0; occupied_resource < nof_occupied_resources; occupied_resource++) {
            if (resource == occupied_resource_array[occupied_resource]) {
                is_found = 1;
                break;
            }
        }

        if(!is_found) {
            free_resources_array[free_resource_index] = resource;
            free_resource_index++;
        }
    }

    *nof_free_resources = free_resource_index;

    return BCM_E_NONE;
}


