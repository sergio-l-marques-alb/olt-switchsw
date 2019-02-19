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

#ifndef L7_USL_BCM_IPMCAST_H
#define L7_USL_BCM_IPMCAST_H
 
#include "l7_common.h"

#ifdef L7_MCAST_PACKAGE

#include "osapi.h"
#include "l7_usl_bcm.h"
#include "bcmx/ipmc.h"
#include "avl_api.h"
#include "platform_config.h"
#include "l7_usl_bcm.h"


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
extern L7_RC_t usl_ipmc_init(); 

/*********************************************************************
* @purpose  Deallocate the IPMC tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*       
* @end
*********************************************************************/
extern L7_RC_t usl_ipmc_fini();

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
extern int usl_bcmx_ipmc_add_l2_ports(bcm_ip_t s_ip_addr,
                                      bcm_ip_t mc_ip_addr,
                                      bcm_vlan_t vid,
                                      bcmx_lplist_t lplist, 
                                      bcmx_lplist_t ut_lplist);

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
extern int usl_bcmx_ipmc_delete_l2_ports(bcm_ip_t s_ip_addr,
                                  bcm_ip_t mc_ip_addr,
                                  bcm_vlan_t vid,
                                  bcmx_lport_t port);

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
extern int usl_bcmx_ipmc_add_l3_ports(bcm_ip_t s_ip_addr,
                                      bcm_ip_t mc_ip_addr, 
                                      bcm_vlan_t vid,
                                      bcmx_lplist_t lplist);

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
extern int usl_bcmx_ipmc_delete_l3_port(bcm_ip_t s_ip_addr,
                                        bcm_ip_t mc_ip_addr,
                                        bcm_vlan_t vid, 
                                        bcmx_lport_t port);

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
extern int usl_bcmx_ipmc_delete(bcm_ip_t s_ip_addr,
                                bcm_ip_t mc_ip_addr, 
                                bcm_vlan_t vid,
                                int keep);
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
extern int usl_bcmx_ipmc_remove(bcmx_ipmc_addr_t *data, int keep);

/*********************************************************************
* @purpose  Add a IP Mcast entry
*
* @param    data     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_ipmc_add(bcmx_ipmc_addr_t *data);

/*********************************************************************
* @purpose  Enable IPMC in the chip
*
* @param    enabled     @{(input)} enable(1) or disable(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_ipmc_enable(int enabled);

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
extern int usl_bcmx_ipmc_repl_add(int ipmc_index,
                                  bcmx_lplist_t lplist,
                                  bcm_vlan_t vid);
  
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
extern int usl_bcmx_ipmc_repl_delete(int ipmc_index,
                                     bcmx_lplist_t lplist,
                                     bcm_vlan_t vid);

/*********************************************************************
* @purpose  Modify the entry in the table
*
* @param    *ipmc         @{(input)} pointer to the IPMC entry.
* @param    port_tgid     @{(input)} The port or Trunk group ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_port_tgid_set(bcmx_ipmc_addr_t *ipmc, int port_tgid);

/*********************************************************************
* @purpose  Clear all entries from the L3 databases
*
* @param    void
*
* @returns  L7_SUCCESS or L7_ERROR 
*
* @notes    Use this function to reset all the tables to the default, no 
*           resources will be deallocated
*
* @end
*********************************************************************/
extern L7_RC_t usl_ipmc_database_invalidate(void);

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
extern L7_BOOL usl_ipmc_inuse_get(bcmx_ipmc_addr_t *bcm_ipmc);

#endif /* L7_MCAST_PACKAGE */
#endif
