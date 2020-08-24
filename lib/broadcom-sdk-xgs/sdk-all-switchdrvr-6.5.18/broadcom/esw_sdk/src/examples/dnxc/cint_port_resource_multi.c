/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        cint_port_resource_multi.c
 *
 * Purpose:
 *
 * Notes:
 *              1) Relevant functions for user:
 *                  - cint_port_resource_multi_speed_set
 *
 */

/*
 * Function:
 *      cint_port_resource_multi_set
 *
 * Purpose:
 *      Set port resource values to many ports at once.
 *      This is done by the following sequence:
 *
 *      1) Get default parameters for all port resources that are not specifically given
 *      2) Use bcm_port_resource_multi_set to configure all ports at once
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      ports_array                             (IN) - ports_array # array of ports which will be modified
 *      port_resources                          (IN) - port_resources # array of resources to be configured
 *      array_size                              (IN) - array_size # number of ports to be modified
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *
 *
 */
int
cint_port_resource_multi_set(
    int unit,
    bcm_port_t * ports_array,
    bcm_port_resource_t * port_resources,
    int array_size)
{
    int rv;
    int port_idx;
    uint32 flags = 0;
    bcm_port_t port;

    /*
     * Set all ports at once
     */
    rv = bcm_port_resource_multi_set(unit, array_size, port_resources);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_resource_multi_set failed.\n");
        return rv;
    }

    printf("cint_port_resource_multi_set: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_port_resource_multi_params_set
 *
 * Purpose:
 *      Set port resource SPEED values to many ports at once.
 *      This is done by the following sequence:
 *
 *      1) Get default parameters for all port resources for the speed provided.
 *      2) Use bcm_port_resource_multi_set to configure all ports at once
 *
 * Parameters:
 *      unit                                    (IN) - unit #
 *      ports_array                             (IN) - ports_array # array of ports which will be modified
 *      speeds_array                            (IN) - speeds_array # arrayof speeds for each port to be set
 *      array_size                              (IN) - array_size # number of ports to be modified
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *
 *
 */
int
cint_port_resource_multi_params_set(
    int unit,
    bcm_port_t * ports_array,
    int *speeds_array,
    bcm_port_phy_fec_t * fec_type_array,
    int *link_training_array,
    int array_size)
{
    int rv;
    int port_idx;
    bcm_port_resource_t port_resources[array_size];
    uint32 flags = 0;
    bcm_port_t port;

    for (port_idx = 0; port_idx < array_size; port_idx++)
    {
        bcm_port_resource_t_init(&port_resources[port_idx]);

        port_resources[port_idx].port = ports_array[port_idx];
        port_resources[port_idx].speed = speeds_array[port_idx];

        if (fec_type_array[port_idx] != BCM_PORT_RESOURCE_DEFAULT_REQUEST)
        {
            port_resources[port_idx].fec_type = fec_type_array[port_idx];
        }
        else
        {
            port_resources[port_idx].fec_type = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
        }

        if (link_training_array[port_idx] != BCM_PORT_RESOURCE_DEFAULT_REQUEST)
        {
            port_resources[port_idx].link_training = link_training_array[port_idx];
        }
        else
        {
            port_resources[port_idx].link_training = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
        }

        /*
         * For now phy_lane_config won't be implemented as control variable
         */
        port_resources[port_idx].phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;

        /*
         * Get default values for the ports gicing the speed passed
         */
        rv = bcm_port_resource_default_get(unit, ports_array[port_idx], flags, &port_resources[port_idx]);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_port_resource_default_get failed.\n");
            return rv;
        }
    }

    /*
     * Set all ports at once
     */
    rv = bcm_port_resource_multi_set(unit, array_size, port_resources);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_resource_multi_set failed.\n");
        return rv;
    }

    printf("cint_port_resource_multi_params_set: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}
