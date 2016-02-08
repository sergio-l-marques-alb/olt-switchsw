/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_vlan_mac.c
*
* @purpose    General routine for USL for ipsubnet vlan
*
* @component  HAPI
*
* @comments   none
*
* @create     05/07/2005
*
* @author    tsrikanth,ryadagiri
* @end
*
**********************************************************************/

#include "l7_usl_bcm_vlan_mac.h"

/*********************************************************************
* @purpose  Adds the entry to the uslvlanmac table
*
* @param   mac     @b{(input)} Mac Address
* @param   vid     @b{(input)} vlan Id
* @param   prio    @b{(input)} priority field
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
bcm_error_t usl_bcmx_vlan_mac_add(bcm_mac_t mac, bcm_vlan_t vid, int prio)
{
    return bcmx_vlan_mac_add(mac, vid, prio);
}

/*********************************************************************
* @purpose  Deletes the entry from the uslvlanmac table
*
* @param   mac     @b{(input)} Mac Address
*
* @returns  bcm_error_t
*
* @end
*
*********************************************************************/
bcm_error_t usl_bcmx_vlan_mac_delete(bcm_mac_t mac)
{
    return bcmx_vlan_mac_delete(mac);
}
/*********************************************************************
* @purpose  sets the port to the uslvlanmac classfiertable
*
* @param   port    @b{(input)} Ethernet Port
* @param   type    @b{(input)} type field
* @param   arg     @b{(input)} arg field
*
* @returns  bcm_error_t
*
* @end
*
*********************************************************************/
bcm_error_t usl_mac_bcmx_vlan_control_port_set(bcmx_lport_t port,
                               bcm_vlan_control_port_t type, int arg)
{
   return(bcmx_vlan_control_port_set(port, type, arg));
}


