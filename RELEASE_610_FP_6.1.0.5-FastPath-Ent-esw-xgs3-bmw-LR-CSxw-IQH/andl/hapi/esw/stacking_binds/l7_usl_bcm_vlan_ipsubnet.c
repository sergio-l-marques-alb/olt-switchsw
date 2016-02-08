/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_vlan_ipsubnet.c
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

#include "l7_usl_bcm_vlan_ipsubnet.h"

/*********************************************************************
* @purpose  Adds the entry to the uslipsubnetvlan table
*
* @param   ipaddr  @b{(input)} IP Address
* @param   netmask @b{(input)} net mask
* @param   vid     @b{(input)} vlan Id
* @param   prio    @b{(input)} priority field
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
bcm_error_t usl_bcmx_vlan_ip4_add(bcm_ip_t ipaddr, bcm_ip_t netmask, bcm_vlan_t vid, int prio)
{
    /* Call BCMX layer to add VLAN table entry in all units. */

    return bcmx_vlan_ip4_add(ipaddr, netmask, vid, prio);
}

/*********************************************************************
* @purpose  Deletes the entry from the uslipsubnetvlan table
*
* @param   ipaddr     @b{(input)} IP Address
* @param   netmask @b{(input)} net mask
*
* @returns  bcm_error_t
*
* @end
*
*********************************************************************/
bcm_error_t usl_bcmx_vlan_ip4_delete(bcm_ip_t ipaddr, bcm_ip_t netmask)
{
    /* Call BCMX layer to delete VLAN table entry in all units. */

    return bcmx_vlan_ip4_delete(ipaddr, netmask);
}


/*********************************************************************
* @purpose  Adds the entry to the uslipsubnetvlan table
*
* @param   ipaddr  @b{(input)} IP Address
* @param   netmask @b{(input)} net mask
* @param   vid     @b{(input)} vlan Id
* @param   prio    @b{(input)} priority field
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
bcm_error_t usl_ip_bcmx_vlan_control_port_set(bcmx_lport_t port,
                               bcm_vlan_control_port_t type, int arg)
{
   return(bcmx_vlan_control_port_set(port,type,arg));
}


