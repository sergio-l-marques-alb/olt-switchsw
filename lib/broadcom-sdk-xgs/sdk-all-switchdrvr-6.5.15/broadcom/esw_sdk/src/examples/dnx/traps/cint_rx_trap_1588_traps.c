/* $Id:
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File: cint_rx_trap_1588_traps.c
 * Purpose: Shows an example for configuration of 1588 traps.
 *
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_1588_traps.c
 *
 * Set Functions:
 *      cint_rx_trap_1588_traps_discard_set(unit, in_port);
 *      cint_rx_trap_1588_traps_accept_set(unit, in_port, out_port, fwd_strength); 
 * Clear Functions:
 *      cint_rx_trap_1588_traps_timesync_clear(unit, in_port);
 *      cint_rx_trap_1588_traps_accept_clear(unit, in_port);
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_1588_traps.c
 * cint
 * cint_rx_trap_1588_traps_discard_set(0, 13);
 * cint_rx_trap_1588_traps_timesync_clear(0, 13);
 */

/**
* \brief
*  Function that sets-up the bcmRxTrap1588Discard configuration.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] in_port       - in port
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_discard_set(
    int unit,
    bcm_port_t in_port)
{
    int rv = BCM_E_NONE, arr_len = 1;
    bcm_port_timesync_config_t timesync_config[arr_len];

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
    timesync_config.pkt_drop = 1;
    timesync_config.pkt_tocpu = 0;
    
    rv = bcm_port_timesync_config_set(unit, in_port, arr_len, &timesync_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_timesync_config_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Function that sets-up the bcmRxTrap1588Accepted configuration.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] in_port            - in port
*   \param [in] out_port          - out port
*   \param [in] fwd_strength    - trap forward strength
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_accept_set(
    int unit,
    bcm_port_t in_port,
    bcm_port_t out_port,
    int fwd_strength)
{
    int rv = BCM_E_NONE, arr_len = 1, trap_id = -1;
    bcm_port_timesync_config_t timesync_config[arr_len];
    bcm_rx_trap_config_t trap_config;

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;
    timesync_config.pkt_drop = 0;
    timesync_config.pkt_tocpu = 0;
    
    rv = bcm_port_timesync_config_set(unit, in_port, arr_len, &timesync_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_timesync_config_set\n");
        return rv;
    }

    /* Set the trap */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.trap_strength = fwd_strength;
    trap_config.dest_port = out_port;
    rv = cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrap1588Accepted, &trap_config, &trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    } 
    
    return rv;
}

/**
* \brief
*  Set either accept or discard 1588 traps.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] is_discard        - is discard 1588 trap (otherwise accept trap)
*   \param [in] in_port            - in port
*   \param [in] out_port          - out port
*   \param [in] fwd_strength    - trap forward strength
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_main(
    int unit,
    int is_discard,
    bcm_port_t in_port,
    bcm_port_t out_port,
    int fwd_strength)
{
    int rv = BCM_E_NONE;

    if (is_discard)
    {
        rv = cint_rx_trap_1588_traps_discard_set(unit, in_port);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in cint_rx_trap_1588_traps_discard_set \n");
            return rv;
        } 
    }
    else
    {
        rv = cint_rx_trap_1588_traps_accept_set(unit, in_port, out_port, fwd_strength);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in cint_rx_trap_1588_traps_accept_set \n");
            return rv;
        }     
    }
    
    return rv;
}

/**
* \brief
*  Function that clears timesync configuration.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] in_port            - in port
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_timesync_clear(
    int unit,
    bcm_port_t in_port)
{
    int rv = BCM_E_NONE, arr_len = 1;
    bcm_port_timesync_config_t timesync_config[arr_len];

    bcm_port_timesync_config_t_init(timesync_config);
    timesync_config.flags = BCM_PORT_TIMESYNC_DEFAULT;
    rv = bcm_port_timesync_config_set(unit, in_port, arr_len, &timesync_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_timesync_config_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Function that clears the bcmRxTrap1588Accepted configuration.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] in_port            - in port
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_accept_clear(
    int unit,
    bcm_port_t in_port)
{
    int rv = BCM_E_NONE, arr_len = 1, trap_id = -1;
    
    rv = cint_rx_trap_1588_traps_timesync_clear(unit, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_1588_traps_timesync_clear\n");
        return rv;
    }

    /* Destroy the trap */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrap1588Accepted, &trap_id);
    rv = bcm_rx_trap_type_destroy(unit, trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in bcm_rx_trap_type_destroy \n");
        return rv;
    } 
    
    return rv;
}

/**
* \brief
*  Destroy either accept or discard 1588 traps.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] is_discard        - is discard 1588 trap (otherwise accept trap)
*   \param [in] in_port            - in port
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_1588_traps_destroy(
    int unit,
    int is_discard,
    bcm_port_t in_port)
{
    int rv = BCM_E_NONE;

    if (is_discard)
    {
        rv = cint_rx_trap_1588_traps_timesync_clear(unit, in_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error in cint_rx_trap_1588_traps_timesync_clear\n");
            return rv;
        }
    }
    else
    {
        rv = cint_rx_trap_1588_traps_accept_clear(unit, in_port);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in cint_rx_trap_1588_traps_accept_clear \n");
            return rv;
        }     
    }
    
    return rv;
}

