/* $Id: cint_sand_utils_tpid.c,
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * Utility functions for Switch/Port TPID in Jericho1 and Jericho2 devices.
 */

int
tpid__tpids_clear_all(
    int unit)
{
    int rv = 0;
    
    if (!is_device_or_above(unit, JERICHO2))
    {
        /*
         * In JR1 run over all ETH ports and run delete all tpids 
         */
        bcm_port_config_t port_config;
        bcm_port_config_t_init(&port_config);
        bcm_port_t port;
        rv = bcm_port_config_get(unit, &port_config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_config_get\n");
            return rv;
        }
        BCM_PBMP_ITER(port_config.e, port)
        {
            rv = bcm_port_tpid_delete_all(unit, port);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_port_tpid_add\n");
                return rv;
            }
        }
    }
    else
    {
       /*
        * In JR2 delete all global tpids 
        * Note: 
        * This was will delete all the default settings (see appl_dnx_vlan_init). 
        */
        rv = bcm_switch_tpid_delete_all(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_tpid_delete_all\n");
            return rv;
         }
    }

    return rv;
}

int
tpid__tpid_add(
    int unit,
    int tpid)
{
    int rv;

    if (!is_device_or_above(unit, JERICHO2))
    {
        /*
         * In JR1 run over all ETH ports and add TPID - up to 2 are avaiable 
         */
        bcm_port_config_t port_config;
        bcm_port_config_t_init(&port_config);
        bcm_port_t port;
        rv = bcm_port_config_get(unit, &port_config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_config_get\n");
            return rv;
        }
        BCM_PBMP_ITER(port_config.e, port)
        {
            rv = bcm_port_tpid_add(unit, port, tpid, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_port_tpid_add\n");
                return rv;
            }
        }
    }
    else
    {
        /*
         * In JR2 add TPID to global array 
         */
        bcm_switch_tpid_info_t tpid_info;
        int options = 0;
        tpid_info.tpid_value = tpid;
        rv = bcm_switch_tpid_add(unit, options, &tpid_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_tpid_add\n");
            return rv;
        }
    }

    return rv;
}
