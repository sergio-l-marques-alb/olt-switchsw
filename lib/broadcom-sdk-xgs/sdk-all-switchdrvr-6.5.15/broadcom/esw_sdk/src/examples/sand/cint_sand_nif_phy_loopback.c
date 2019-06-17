/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        cint_sand_nif_phy_loopback.c
 *
 * Purpose:  Configuring PHY loopback on one port will return failure when lane mapping for this port is
 *               not symmetric.
 *               For example, the following is not a symmetric mapping:
 *                +----------------------------------+
 *                |Rx Serdes 1   Lane 4   Tx Serdes 2|
 *                +----------------------------------+
 *               This Cint provides an example about how to configure PHY loopback on one NIF port.
 *
 *               The lane mapping is symmetric:
 *                       Use bcm_port_loopback_set directly.
 *               The lane mapping is not symmetric:
 *                       Align the lane map to symmetric lane map;
 *                       Use bcm_port_loopback_set to configure PHY loopback.
 *
 *
 * Notes:
 *              1) Configure sequence:
 *                  - cint_port_phy_loopback_set - Set PHY loopback for given port
 *                  - cint_port_phy_loopback_clear - Clear PHY loopback for given port
 *
 *              2) If the lane mapping for ILKN over fabric port is not symmetric, this Cint will return
 *                 error, as ILKN over fabric ports don't support DPP.
 *
 *              3) When changing the lane map, all the ports on the PortMacro should be removed
 *                 and added again.
 *
 *              4) Once closing the PHY loopback using this cint, the port cannot not be used for regular
 *                 connection with peer. It can only used for loopback.
 *                 Please call cint_port_phy_loopback_clear to clear the PHY loopback if a regular connection
 *                 with peer is needed.
 *
 * This Cint uses the following cint:
 *
 *     cint src/examples/sand/utility/cint_sand_utils_global.c
 *     cint src/examples/dnx/port/cint_dynamic_port_add_remove.c
 *
 *  Usage example:
       linkscan off
       nif sts
       c
       cint_reset();
       exit;
       cint ../../../../src/examples/dnx/port/cint_dynamic_port_add_remove.c
       cint ../../../../src/examples/sand/cint_sand_nif_phy_loopback.c

       c
       int unit;
       bcm_port_t port;
       uint32 flags;
       unit=0;
       port=13;
       print cint_port_phy_loopback_set (unit, port);
       exit;
       linkscan on
       p sts nif
       phy prbs set 13
       phy prbs get 13
 *
 */

static const int NOF_NIF_PHYS = 96;
static const int NOF_LANES_PER_PM = 8;
static const int MAX_NIF_PORTS = 256;

/*
 * Backup the orignal lane map info.
 */
static bcm_port_lane_to_serdes_map_t nif_serdes_map_bkup[NOF_NIF_PHYS];

/*
 * Port Info DB, used for storing the port info before removing the ports
 */
typedef struct port_info_db_s
{
    bcm_port_resource_t resource;
    int enable;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_tpid_class_t tpid_class;
    int loopback;
    dfe_mode_t dfe_mode;
    medium_type_t medium_type;
    int header_type_in;
    int header_type_out;
    int lane_swap_is_aligned;
};

static port_info_db_s port_info_db[MAX_NIF_PORTS];

/**
 * \brief - Get BMP boundaries, including upper boundary and
 *    lower boundary.
 *
 * \param [in] unit - chip unit id
 * \param [in] bmp - bitmap
 * \param [out] lower_bound - position of the first "1" from LSB
 * \param [out] upper_bound - position of the first "1" from MSB
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int
cint_port_phy_loopback_port_lane_boundary_get(
    int unit,
    bcm_port_t port,
    int *lower_bound,
    int *upper_bound)
{
    int rv = BCM_E_NONE;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    BCM_PBMP_ITER(interface_info.phy_pbmp, *lower_bound)
    {
        break;
    }
    for (*upper_bound = NOF_NIF_PHYS - 1; *upper_bound >= 0; --(*upper_bound))
    {
        if (BCM_PBMP_MEMBER(interface_info.phy_pbmp, *upper_bound))
        {
            break;
        }
    }

    return rv;
}

/**
 * \brief - Get the port list in the PortMacro
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port #
 * \param [out] pm_first_lane - the first lane id for the PM
 * \param [out] pm_last_lane - the last lane id for the PM
 * \param [out] nof_ports - port numbers in the PM
 * \param [out] port_list - port list in the PM
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_pm_ports_get(
    int unit,
    bcm_port_t port,
    uint32 *pm_first_lane,
    uint32 *pm_last_lane,
    int *nof_ports,
    int *port_list)
{
    int lane;
    int rv = BCM_E_NONE;
    int port_first_lane, port_last_lane;
    bcm_port_t nif_port;
    bcm_pbmp_t port_bmp;
    bcm_port_config_t config;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    /*
     * Get port and pm boundaries 
     */
    rv = cint_port_phy_loopback_port_lane_boundary_get(unit, port, &port_first_lane, &port_last_lane);
    if (BCM_FAILURE(rv))
    {
        printf("cint_port_phy_loopback_port_lane_boundary_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    *pm_first_lane = (port_first_lane / NOF_LANES_PER_PM) * NOF_LANES_PER_PM;
    *pm_last_lane = (port_last_lane / NOF_LANES_PER_PM + 1) * NOF_LANES_PER_PM - 1;

    /*
     * Not consider two ILKN ports share one PM scenario, as there is no such scenario for JER2 
     */
    *nof_ports = 0;
    BCM_PBMP_CLEAR(port_bmp);

    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    /*
     * Traverse all NIF ports to get the phy to port mapping
     */
    BCM_PBMP_ITER(config.nif, nif_port)
    {
        rv = cint_port_phy_loopback_port_lane_boundary_get(unit, nif_port, &port_first_lane, &port_last_lane);
        if (BCM_FAILURE(rv))
        {
            printf("cint_port_phy_loopback_port_lane_boundary_get failed for port %d - unit %d.\n", nif_port, unit);
            return rv;
        }
        rv = bcm_port_get(unit, nif_port, &flags, &interface_info, &mapping_info);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_port_interface_get failed for port %d - unit %d.\n", nif_port, unit);
            return rv;
        }
        /*
         * Check if the first lane and last lane are located in the PM.
         * Jericho2 supports Eth port and ILKN port sharing one PM,
         * so there may be parts of ILKN lanes are located in the PM.
         */
        if (((port_first_lane >= *pm_first_lane) && (port_last_lane <= *pm_last_lane)) ||
            ((interface_info.interface == BCM_PORT_IF_ILKN) && ((port_first_lane >= *pm_first_lane) || (port_last_lane <= *pm_last_lane))))
        {
            if (!BCM_PBMP_MEMBER(port_bmp, nif_port))
            {
                BCM_PBMP_PORT_ADD(port_bmp, nif_port);
                port_list[*nof_ports] = nif_port;
                (*nof_ports)++;
            }
        }
    }
    return rv;
}

/**
 * \brief - Get the original Medium type and DFE mode
 *
 * \param [in] unit - chip unit id
 * \param [in] phy_lane_config - phy lane config
 * \param [out] medium_type - Medium type
 * \param [out] dfe_mode - DFE mode
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_medium_type_and_dfe_mode_get(
    int unit,
    int phy_lane_config,
    medium_type_t *medium_type,
    dfe_mode_t *dfe_mode)
{
    int rv = BCM_E_NONE;

    switch (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_GET(phy_lane_config))
    {
        case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE:
            *medium_type = backplane;
            break;
        case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE:
            *medium_type = copper_cable;
            break;
        case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS:
            *medium_type = optic;
            break;
        default:
            rv = BCM_E_PARAM;
            printf("Unsupported medium type\n");
            return rv;
    }

    if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_GET(phy_lane_config) &&
        !BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(phy_lane_config))
    {
        *dfe_mode = dfe_lp;
    }
    else if (!BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_GET(phy_lane_config) &&
             BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(phy_lane_config))
    {
        *dfe_mode = dfe_on;
    }
    else if (!BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_GET(phy_lane_config) &&
             !BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(phy_lane_config))
    {
        *dfe_mode = dfe_none;
    }
    else
    {
        rv = BCM_E_PARAM;
        printf("Unsupported dfe mode!\n");
        return rv;
    }

    return rv;
}

/**
 * \brief - Get all the port info, including port resource,
 *    flags, interface info, mapping info, enabe status and
 *    loopback mode.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port #
 * \param [out] port_info - port info DB
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_port_info_get(
    int unit,
    bcm_port_t port)
{
    int rv = BCM_E_NONE;
    rv = bcm_port_get(unit, port, &port_info_db[port].flags, &port_info_db[port].interface_info,
                      &port_info_db[port].mapping_info);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }

    rv = bcm_port_resource_get(unit, port, &port_info_db[port].resource);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_resource_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }

    bcm_port_tpid_class_t_init(&port_info_db[port].tpid_class);
    port_info_db[port].tpid_class.port = port;
    port_info_db[port].tpid_class.tpid1 = 0x8100;
    port_info_db[port].tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_info_db[port].tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY|BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    rv = bcm_port_tpid_class_get(unit, &port_info_db[port].tpid_class);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_tpid_class_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }

    rv = bcm_port_enable_get(unit, port, &port_info_db[port].enable);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_enable_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    rv = cint_port_phy_loopback_medium_type_and_dfe_mode_get(unit, port_info_db[port].resource.phy_lane_config,
                                                             &port_info_db[port].medium_type, &port_info_db[port].dfe_mode);
    if (BCM_FAILURE(rv))
    {
        printf("cint_port_phy_loopback_medium_type_and_dfe_mode_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    port_info_db[port].header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    port_info_db[port].header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;

    return rv;
}

/**
 * \brief - Set all the port info, including port resource,
 *    enabe status and loopback mode.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port #
 * \param [in] port_info - port info DB
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_port_info_set(
    int unit,
    bcm_port_t port)
{
    int rv = BCM_E_NONE;

    rv = bcm_port_tpid_class_set(unit, &port_info_db[port].tpid_class);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_tpid_class_set failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    rv = bcm_port_enable_set(unit, port, port_info_db[port].enable);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_enable_set failed for port %d- unit %d.\n", port, unit);
        return rv;
    }
    return rv;
}

int
cint_dyn_port_properties_construct(
    int unit,
    int nof_ports,
    int *port_list,
    bcm_port_mapping_info_t *mapping_info,
    bcm_port_interface_info_t *interface_info,
    bcm_port_resource_t *resource,
    dfe_mode_t *dfe_mode,
    medium_type_t *medium_type,
    int *header_type_in,
    int *header_type_out,
    uint32 *flags)
{
    int rv = BCM_E_NONE;
    int port_index;
    bcm_port_t tmp_port;

    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        tmp_port = port_list[port_index];
        mapping_info[port_index] = port_info_db[tmp_port].mapping_info;
        mapping_info[port_index].pp_port = -1;
        interface_info[port_index] = port_info_db[tmp_port].interface_info;
        resource[port_index] = port_info_db[tmp_port].resource;
        dfe_mode[port_index] = port_info_db[tmp_port].dfe_mode;
        medium_type[port_index] = port_info_db[tmp_port].medium_type;
        header_type_in[port_index] = port_info_db[tmp_port].header_type_in;
        header_type_out[port_index] = port_info_db[tmp_port].header_type_out;
        flags[port_index] = port_info_db[tmp_port].flags;
    }
    return rv;
}



/**
 * \brief - Align the lane map to symmetric one.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port #
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_lane_swap_align(
    int unit,
    bcm_port_t port)
{
    int rv = BCM_E_NONE;
    uint32 pm_first_lane, pm_last_lane;
    int lane, port_index;
    bcm_port_t port_list[NOF_LANES_PER_PM] = { 0 }, tmp_port;
    int nof_ports = 0;
    bcm_port_lane_to_serdes_map_t serdes_map[NOF_NIF_PHYS];
    dfe_mode_t dfe_mode[NOF_LANES_PER_PM];
    medium_type_t medium_type[NOF_LANES_PER_PM];
    bcm_port_mapping_info_t mapping_info[NOF_LANES_PER_PM];
    bcm_port_interface_info_t interface_info[NOF_LANES_PER_PM];
    bcm_port_resource_t resource[NOF_LANES_PER_PM];
    int header_type_in[NOF_LANES_PER_PM];
    int header_type_out[NOF_LANES_PER_PM];
    uint32 flags[NOF_LANES_PER_PM];

    /*
     * Get all the ports which share the same PM with the loopback port
     */
    rv = cint_port_phy_loopback_pm_ports_get(unit, port, &pm_first_lane, &pm_last_lane, &nof_ports, port_list);
    if (BCM_FAILURE(rv))
    {
        printf("cint_port_phy_loopback_pm_ports_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    rv = bcm_port_lane_to_serdes_map_get(unit, 0, NOF_NIF_PHYS, serdes_map);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_lane_to_serdes_map_get failed - unit %d.\n", unit);
        return rv;
    }
    for (lane = pm_first_lane; lane <= pm_last_lane; ++lane)
    {
        /*
         * Store the default mapping 
         */
        nif_serdes_map_bkup[lane].serdes_rx_id = serdes_map[lane].serdes_rx_id;
        nif_serdes_map_bkup[lane].serdes_tx_id = serdes_map[lane].serdes_tx_id;
        /*
         * Assign one to one mapping, skip the NOT_MAPPED lane
         */
        if (serdes_map[lane].serdes_rx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            serdes_map[lane].serdes_rx_id = lane;
            serdes_map[lane].serdes_tx_id = lane;
        }
    }

    /*
     * Change the lane mapping to one to one.
     * This procedure will remove all the ports, adjust the lane map,
     * and then add all the ports back.
     */

    /*
     * Get the orignal port status and then remove the port 
     */
    for (port_index = 0; port_index < nof_ports; ++port_index)

    {
        /*
         * Get the original port info 
         */
        tmp_port = port_list[port_index];
        rv = cint_port_phy_loopback_port_info_get(unit, tmp_port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_port_phy_loopback_port_info_get failed for port %d - unit %d.\n", tmp_port, unit);
            return rv;
        }

        rv = cint_dyn_port_remove_port_full_example(unit, tmp_port, 0);
        if (BCM_FAILURE(rv))
        {
            printf("cint_dyn_port_remove_port_full_example failed for port %d - unit %d.\n", tmp_port, unit);
            return rv;
        }
    }
    /*
     * Configure Lane Map 
     */
    rv = bcm_port_lane_to_serdes_map_set(unit, 0, NOF_NIF_PHYS, serdes_map);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_lane_to_serdes_map_t failed - unit %d.\n", unit);
        return rv;
    }
    rv = cint_dyn_port_properties_construct(unit, nof_ports, port_list, mapping_info, interface_info, resource,
                                            dfe_mode, medium_type, header_type_in, header_type_out, flags);
    if (BCM_FAILURE(rv))
    {
        printf("cint_dyn_port_properties_construct failed - unit %d.\n", unit);
        return rv;
    }
    /*
     * Add the ports back 
     */
    rv = cint_dyn_port_add_ports_full_example(unit, nof_ports, port_list, mapping_info, interface_info, resource,
                                              dfe_mode, medium_type, header_type_in, header_type_out, flags);
    if (BCM_FAILURE(rv))
    {
        printf("cint_dyn_port_add_ports_full_example failed - unit %d.\n", unit);
        return rv;
    }
    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        /*
         * Restore the original port info which is not covered by dynamic port CINT
         */
        rv = cint_port_phy_loopback_port_info_set(unit, tmp_port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_port_phy_loopback_port_info_set failed for port %d - unit %d.\n", tmp_port, unit);
            return rv;
        }
        /*
         * Mark the lane_swap is aligned for this port
         */
        port_info_db[tmp_port].lane_swap_is_aligned = 1;
    }
    return rv;
}

/**
 * \brief - Judge if port has lane swap.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port #
 *
 * \return
 *   int - need_to_align_swap
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_has_lane_swap(
    int unit,
    bcm_port_t port)
{
    int rv = BCM_E_NONE;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_lane_to_serdes_map_t serdes_map[NOF_NIF_PHYS];
    int need_to_align_swap = 0;
    uint32 flags;
    int lane_id;

    rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    rv = bcm_port_lane_to_serdes_map_get(unit, 0, NOF_NIF_PHYS, serdes_map);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_lane_to_serdes_map_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    /*
     * Check if the lane map is symmetric
     */
    BCM_PBMP_ITER(interface_info.phy_pbmp, lane_id)
    {
        if (serdes_map[lane_id].serdes_rx_id != serdes_map[lane_id].serdes_tx_id)
        {
            need_to_align_swap = 1;
            break;
        }
    }
    return need_to_align_swap;
}

/**
 * \brief - Recover the original lane map.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port #
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_lane_swap_recover(
    int unit,
    int port)
{
    int rv = BCM_E_NONE;
    int port_first_lane, port_last_lane;
    uint32 pm_first_lane, pm_last_lane;
    int lane, port_index;
    bcm_port_t port_list[NOF_LANES_PER_PM] = { 0 }, tmp_port;
    int nof_ports = 0;
    int need_recover_lane_map, loopback;
    bcm_port_lane_to_serdes_map_t serdes_map[NOF_NIF_PHYS];
    dfe_mode_t dfe_mode[NOF_LANES_PER_PM];
    medium_type_t medium_type[NOF_LANES_PER_PM];
    bcm_port_mapping_info_t mapping_info[NOF_LANES_PER_PM];
    bcm_port_interface_info_t interface_info[NOF_LANES_PER_PM];
    bcm_port_resource_t resource[NOF_LANES_PER_PM];
    int header_type_in[NOF_LANES_PER_PM];
    int header_type_out[NOF_LANES_PER_PM];
    uint32 flags[NOF_LANES_PER_PM];

    /*
     * Check if the lane swap is aligned for this port.
     */
    if (!port_info_db[port].lane_swap_is_aligned)
    {
        return rv;
    }
    /*
     * Get the PM port list 
     */
    rv = cint_port_phy_loopback_pm_ports_get(unit, port, &pm_first_lane, &pm_last_lane, &nof_ports, port_list);
    if (BCM_FAILURE(rv))
    {
        printf("cint_port_phy_loopback_pm_ports_get failed for port %d- unit %d.\n", port, unit);
        return rv;
    }
    /*
     * Check if there is any other PHY loopback port. If yes, we should not recover the original lane mapping 
     */
    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        if (port == port_list[port_index])
        {
            continue;
        }
        rv = bcm_port_loopback_get(unit, port_list[port_index], &loopback);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_port_loopback_get failed for port %d- unit %d.\n", port_list[port_index], unit);
            return rv;
        }
        if (loopback == BCM_PORT_LOOPBACK_PHY)
        {
            return rv;
        }
    }
    rv = bcm_port_lane_to_serdes_map_get(unit, 0, NOF_NIF_PHYS, serdes_map);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_lane_to_serdes_map_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    /*
     * Restore Lane Mapping
     */
    for (lane = pm_first_lane; lane <= pm_last_lane; ++lane)
    {
        /*
         * Restore the default mapping 
         */
        serdes_map[lane].serdes_rx_id = nif_serdes_map_bkup[lane].serdes_rx_id;
        serdes_map[lane].serdes_tx_id = nif_serdes_map_bkup[lane].serdes_tx_id;
        /*
         * Clear the default lane mapping DB
         */
        nif_serdes_map_bkup[lane].serdes_rx_id = 0;
        nif_serdes_map_bkup[lane].serdes_tx_id = 0;
    }

    /*
     * Get the orignal port status and then remove the port 
     */
    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        /*
         * Get the original port info 
         */
        tmp_port = port_list[port_index];
        rv = cint_port_phy_loopback_port_info_get(unit, tmp_port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_port_phy_loopback_port_info_get failed for port %d - unit %d.\n", tmp_port, unit);
            return rv;
        }
        rv = cint_dyn_port_remove_port_full_example(unit, tmp_port, 0);
        if (BCM_FAILURE(rv))
        {
            printf("cint_dyn_port_remove_port_full_example failed for port %d - unit %d.\n", tmp_port, unit);
            return rv;
        }
    }
    /*
     * Configure Lane Map 
     */
    rv = bcm_port_lane_to_serdes_map_set(unit, 0, NOF_NIF_PHYS, serdes_map);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_lane_to_serdes_map_t failed - unit %d.\n", unit);
        return rv;
    }
    rv = cint_dyn_port_properties_construct(unit, nof_ports, port_list, mapping_info, interface_info, resource,
                                            dfe_mode, medium_type, header_type_in, header_type_out, flags);
    if (BCM_FAILURE(rv))
    {
        printf("cint_dyn_port_properties_construct failed - unit %d.\n", unit);
        return rv;
    }
    /*
     * Add the ports back 
     */
    rv = cint_dyn_port_add_ports_full_example(unit, nof_ports, port_list, mapping_info, interface_info, resource,
                                              dfe_mode, medium_type, header_type_in, header_type_out, flags);
    if (BCM_FAILURE(rv))
    {
        printf("cint_dyn_port_add_ports_full_example failed - unit %d.\n", unit);
        return rv;
    }
    /*
     * Add the ports back 
     */
    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        /*
         * Restore the original port info which is not covered by dynamic port CINT
         */
        rv = cint_port_phy_loopback_port_info_set(unit, tmp_port);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_port_resource_set failed for port %d- unit %d.\n", tmp_port, unit);
            return rv;
        }
        /*
         * Clear lane swap aligned flag
         */
        port_info_db[tmp_port].lane_swap_is_aligned = 0;
    }
    return rv;
}

/**
 * \brief - Set PHY loopback.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port #
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_set(
    int unit,
    bcm_port_t port)
{
    int rv = BCM_E_NONE;
    int is_dnx;
    int need_to_align_swap = 0;
    bcm_port_config_t config;

    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    if (!BCM_PBMP_MEMBER(config.nif, port))
    {
        printf("Port % is not supported by this CINT - unit %d.\n", port, unit);
        return BCM_E_PARAM;
    }
    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_device_member_get failed to get is_dnx\n", unit);
        return rv;
    }
    if (is_dnx)
    {
        need_to_align_swap = cint_port_phy_has_lane_swap(unit, port);
        if (need_to_align_swap)
        {
            rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE);
            if (BCM_FAILURE(rv))
            {
                printf("bcm_port_loopback_set failed for port %d - unit %d.\n", port, unit);
            }
            rv = cint_port_phy_loopback_lane_swap_align(unit, port);
            if (BCM_FAILURE(rv))
            {
                printf("cint_port_phy_loopback_set_with_lane_swap_align failed for port %d - unit %d.\n", port, unit);
                return rv;
            }
        }
    }
    rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_loopback_set failed for port %d - unit %d.\n", port, unit);
        return rv;
    }
    return BCM_E_NONE;
}

/**
 * \brief - Clear PHY loopback for the given port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port #
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_clear(
    int unit,
    bcm_port_t port)
{
    int rv = BCM_E_NONE;
    int is_dnx;
    bcm_port_config_t config;

    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed - unit %d.\n", unit);
        return rv;
    }
    if (!BCM_PBMP_MEMBER(config.nif, port))
    {
        printf("Port % is not supported by this CINT - unit %d.\n", port, unit);
        return BCM_E_PARAM;
    }
    /*
     * Clear loopback status 
     */
    rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_loopback_set failed for port %d - unit %d.\n", port, unit);
        return rv;
    }

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_device_member_get failed to get is_dnx\n", unit);
        return rv;
    }

    /*
     * Recover the original lane map if needed
     */
    if (is_dnx)
    {
        rv = cint_port_phy_loopback_lane_swap_recover(unit, port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_port_phy_loopback_lane_swap_recover failed for port %d - unit %d.\n", port, unit);
            return rv;
        }
    }
    return rv;
}

/**
 * \brief - Enable/disable PHY loopback according to 'enable' argument.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port #
 * \param [in] en   - enable/disable control
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_enable_set(
    int unit,
    bcm_port_t port,
    int en)
{
    int rv = BCM_E_NONE;
    if (en)
    {
        rv = cint_port_phy_loopback_set(unit, port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_port_phy_loopback_set failed for port %d - unit %d.\n", port, unit);
            return rv;
        }
    }
    else
    {
        rv = cint_port_phy_loopback_clear(unit, port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_port_phy_loopback_clear failed for port %d - unit %d.\n", port, unit);
            return rv;
        }
    }

    return rv;
}

/**
 * \brief - Set PHY loopback for all the NIF ports.
 *
 * \param [in] unit - chip unit id
 * \param [in] pbmp - logical port bitmap
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_set_all(
    int unit)
{
    int rv = 0;
    bcm_port_t port;
    bcm_port_config_t config;

    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed for unit %d.\n", unit);
        return rv;
    }

    BCM_PBMP_ITER(config.nif, port)
    {
        rv = cint_port_phy_loopback_set(unit, port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_port_phy_loopback_set failed for port %d - unit %d.\n", port, unit);
            return rv;
        }
    }
    return rv;
}

/**
 * \brief - Clear PHY loopback for all the NIF ports.
 *
 * \param [in] unit - chip unit id
 * \param [in] pbmp - logical port bitmap
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
cint_port_phy_loopback_clear_all(
    int unit)
{
    int rv = 0;
    bcm_port_t port;
    bcm_port_config_t config;

    rv = bcm_port_config_get(unit, &config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed for unit %d.\n", unit);
        return rv;
    }

    BCM_PBMP_ITER(config.nif, port)
    {
        rv = cint_port_phy_loopback_clear(unit, port);
        if (BCM_FAILURE(rv))
        {
            printf("cint_port_phy_loopback_clear failed for port %d - unit %d.\n", port, unit);
            return rv;
        }
    }
    return rv;
}

