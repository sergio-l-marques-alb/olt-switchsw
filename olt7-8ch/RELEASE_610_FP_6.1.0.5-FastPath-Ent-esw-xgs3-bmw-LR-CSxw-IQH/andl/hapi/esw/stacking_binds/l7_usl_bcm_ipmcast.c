/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l2.c
*
* @purpose    Handle synchronization responsibilities for Address,Trunk,VLAN, STG
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"

#ifdef L7_MCAST_PACKAGE

#include "osapi.h"
#include "l7_usl_bcm.h"
#include "bcmx/ipmc.h"
#include "bcm_int/control.h"
#include "avl_api.h"
#include "platform_config.h"
#include "l7_usl_bcm.h"
#include "broad_common.h"

/*********************************************************************
* @purpose  Initialize the IPMC tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    On error, all resources will be released
*       
* @end
*********************************************************************/
L7_RC_t usl_ipmc_init()
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deallocate the IPMC tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    If the system has been operational for a while, the caller should
*           have called the usl_ipmc_database_invalidate before calling this
*           in order to insure that the lplists have been properly freed 
*       
* @end
*********************************************************************/
L7_RC_t usl_ipmc_fini()
{
  return L7_SUCCESS;
}
 
/*********************************************************************
* @purpose  Add L2 ports to an ip multicast group
*
* @param    s_ip_addr     @{(input)} The Source IP address
* @param    mc_ip_addr    @{(input)} The Multicast IP address
* @param    vid           @{(input)} The VLAN associated
* @param    lplist        @{(input)} Ports in the group
* @param    ut_lplist     @{(input)} Untagged ports
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_add_l2_ports(bcm_ip_t s_ip_addr,
                               bcm_ip_t mc_ip_addr,
                               bcm_vlan_t vid,
                               bcmx_lplist_t lplist, 
                               bcmx_lplist_t ut_lplist)
{
  return bcmx_ipmc_add_l2_ports(s_ip_addr, mc_ip_addr, vid, lplist, ut_lplist);
}

/*********************************************************************
* @purpose  Remove L2 ports to an ip multicast group
*
* @param    s_ip_addr     @{(input)} The Source IP address
* @param    mc_ip_addr    @{(input)} The Multicast IP address
* @param    vid           @{(input)} The VLAN associated
* @param    port          @{(input)} The port being deleted
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_delete_l2_ports(bcm_ip_t s_ip_addr,
                                  bcm_ip_t mc_ip_addr,
                                  bcm_vlan_t vid,
                                  bcmx_lport_t port)
{
  return bcmx_ipmc_delete_l2_ports(s_ip_addr, mc_ip_addr, vid, port);
}
/*********************************************************************
* @purpose  Add L3 ports to an ip multicast group
*
* @param    s_ip_addr     @{(input)} The Source IP address
* @param    mc_ip_addr    @{(input)} The Multicast IP address
* @param    vid           @{(input)} The VLAN associated
* @param    lplist        @{(input)} Ports in the group
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_add_l3_ports(bcm_ip_t s_ip_addr,
                               bcm_ip_t mc_ip_addr, 
                               bcm_vlan_t vid,
                               bcmx_lplist_t lplist)
{
  return bcmx_ipmc_add_l3_ports(s_ip_addr, mc_ip_addr, vid, lplist);
}

/*********************************************************************
* @purpose  Delete a L3 port from the Mcast group 
*
* @param    s_ip_addr     @{(input)} The Source IP address
* @param    mc_ip_addr    @{(input)} The Multicast IP address
* @param    vid           @{(input)} The VLAN associated
* @param    port          @{(input)} The port being deleted
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_delete_l3_port(bcm_ip_t s_ip_addr,
                                 bcm_ip_t mc_ip_addr,
                                 bcm_vlan_t vid, 
                                 bcmx_lport_t port)
{
  return bcmx_ipmc_delete_l3_port(s_ip_addr, mc_ip_addr, vid, port);
}

/*********************************************************************
* @purpose  Configure the IP Multicast egress properties
*
* @param    port          @{(input)} The port to configure
* @param    mac           @{(input)} The MAC address
* @param    untag         @{(input)} Transmit as tagged (0) or untagged (1)
* @param    vid           @{(input)} The VLAN to associate with
* @param    ttl_threshold @{(input)} Drop IPMC packets if TTL of packet <= ttl_thresh
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_egress_port_set(bcmx_lport_t port,
                                  const bcm_mac_t mac,
                                  int untag,
                                  bcm_vlan_t vid,
                                  int ttl_threshold)
{
  return bcmx_ipmc_egress_port_set(port, mac, untag, vid, ttl_threshold);
}

/*********************************************************************
* @purpose  Delete a IP Mcast entry
*
* @param    s_ip_addr     @{(input)} The Source IP address
* @param    mc_ip_addr    @{(input)} The Multicast IP address
* @param    vid           @{(input)} The VLAN associated
* @param    keep          @{(input)} Keep the entry in Table(1) or remove entry(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_delete(bcm_ip_t s_ip_addr,
                         bcm_ip_t mc_ip_addr, 
                         bcm_vlan_t vid,
                         int keep)
{
  return bcmx_ipmc_delete(s_ip_addr, mc_ip_addr, vid, keep);
}

/*********************************************************************
* @purpose  Delete a IP Mcast entry
*
* @param    data          @{(input)} The ipmc entry
* @param    keep          @{(input)} Keep the entry in Table(1) or remove entry(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_remove(bcmx_ipmc_addr_t *data, int keep)
{
  if (keep)
  {
    data->flags |= BCM_IPMC_KEEP_ENTRY;
  }
  else
  {
    data->flags &= ~BCM_IPMC_KEEP_ENTRY;
  }
  return  bcmx_ipmc_remove(data);
}

/*********************************************************************
* @purpose  Add a IP Mcast entry
*
* @param    data     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_add(bcmx_ipmc_addr_t *data)
{
  L7_uint32 rv, tmp_rv;

  rv = tmp_rv = BCM_E_NONE;

  rv = bcmx_ipmc_add(data);

  return rv;
}

/*********************************************************************
* @purpose  Modify the entry in the table
*
* @param    ipmc_index    @{(input)} Index of the IPMC Entry.
* @param    s_ip_addr     @{(input)} The Source IP address
* @param    mc_ip_addr    @{(input)} The Multicast IP address
* @param    vid           @{(input)} The VLAN associated
* @param    ts            @{(input)} Trunk (1) or Port(0) the traffic should arrive on
* @param    port_tgid     @{(input)} The port or Trunk group ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_port_tgid_set(bcmx_ipmc_addr_t *ipmc, int port_tgid)
{
  L7_uint32 rv = BCM_E_NONE;
  L7_uint32 bcm_unit;
  bcm_ipmc_addr_t new_ipmc;

  /* We are given srcaddr, groupip, vid, index, and tgid. For all other 
     fields, use what exists in the entry. srcaddr, groupip, and index 
     won't be different. */
  for (bcm_unit = 0; bcm_unit < soc_ndev ; bcm_unit++) {
    if (BCM_IS_FABRIC(bcm_unit)) {
      continue;
    }
    
    memset(&new_ipmc, 0, sizeof(new_ipmc));
    if (mbcm_driver[bcm_unit]->mbcm_ipmc_get(bcm_unit, ipmc->ipmc_index, &new_ipmc) != BCM_E_NONE) {
      continue;
    }

    new_ipmc.vid = ipmc->vid;
    new_ipmc.port_tgid = port_tgid;
    new_ipmc.ts = ipmc->ts;
    new_ipmc.flags |= (BCM_IPMC_REPLACE | BCM_IPMC_USE_IPMC_INDEX);
    BCM_IF_ERROR_RETURN(bcm_ipmc_add(bcm_unit, &new_ipmc));
  }

  return rv;
}

/*********************************************************************
* @purpose  Enable IPMC in the chip
*
* @param    enabled     @{(input)} enable(1) or disable(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_enable(int enabled)
{
  return bcmx_ipmc_enable(enabled);
}

/*********************************************************************
* @purpose  Add a VLAN to selected ports' replication list for chosen IPMC group
*
* @param    ipmc_index    @{(input)} The Multicast IP address index
* @param    lplist        @{(input)} Ports in the group
* @param    vid           @{(input)} The VLAN associated
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_repl_add(int ipmc_index,
                           bcmx_lplist_t lplist,
                           bcm_vlan_t vid)
{
  return bcmx_ipmc_repl_add(ipmc_index, lplist, vid);
}
  
/*********************************************************************
* @purpose  Remove VLAN from selected ports' replication list for chosen
*
* @param    ipmc_index    @{(input)} The Multicast IP address index
* @param    lplist        @{(input)} Ports in the group
* @param    vid           @{(input)} The VLAN associated
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_repl_delete(int ipmc_index,
                              bcmx_lplist_t lplist,
                              bcm_vlan_t vid)
{
  return bcmx_ipmc_repl_delete(ipmc_index, lplist, vid);
}

/*********************************************************************
* @purpose  Test whether an IPMC group is in use
*
* @param    bcm_ipmc      @{(input)}  IPMC entry to be used
*
* @returns  L7_TRUE if in use
*
* @notes    DOES NOT ADD DOWNSTREAM PORTS
*
* @end
*********************************************************************/
L7_BOOL usl_ipmc_inuse_get(bcmx_ipmc_addr_t *bcm_ipmc)
{
  L7_BOOL rc = L7_FALSE;
  L7_uint32 rv = BCM_E_NONE;

  bcm_ipmc->flags |= BCM_IPMC_HIT_CLEAR;

  rv = bcmx_ipmc_find(bcm_ipmc);

  if ((L7_BCMX_OK(rv) == L7_TRUE) && (bcm_ipmc->flags & BCM_IPMC_HIT))
  {
    rc = L7_TRUE;
  }

  return rc;
}
#endif /* L7_MCAST_PACKAGE */
