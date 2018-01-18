/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcmx_ipmc.h
*
* @purpose    USL BCMX API's for IP Multicast
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
#ifndef L7_USL_BCMX_IPMCAST_H
#define L7_USL_BCMX_IPMCAST_H

#include "l7_common.h"

#ifdef L7_MCAST_PACKAGE

#include "l7_usl_bcm_ipmcast.h"

/*********************************************************************
* @purpose  Add a IP Mcast entry
*
* @param    data     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_add(usl_bcm_ipmc_addr_t *data);


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
int usl_bcmx_ipmc_remove(usl_bcm_ipmc_addr_t *data, int keep);



/*********************************************************************
* @purpose  Modify the port_tgid for the entry
*
* @param    *ipmc         @{(input)} pointer to the IPMC entry.
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_port_tgid_set(usl_bcm_ipmc_addr_t *ipmc);

/*********************************************************************
* @purpose  Set L2 ports for a IPMC group
*
* @params   ipMcAddress {(input)} IPMC group info
*
* @returns  Defined by the Broadcom driver
*
* @notes    The ports in L2 bitmap are replaces the existing L2 ports 
*           for the group.
* @end
*********************************************************************/
int usl_bcmx_ipmc_set_l2_ports(usl_bcm_ipmc_addr_t *ipmcAddress);

/*********************************************************************
* @purpose  Set a port in L2 bitmap for a number of groups
*
* @param   port        {(input)}  BCMX Lport
* @param   *index      {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups  {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id     {(input)} VLAN affected by this call.
* @param   tagged      {(input)} Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_add_l2_port_groups (bcmx_lport_t port,
                                      L7_uint32    *ipmc_index,
                                      L7_uint32    num_groups,
                                      L7_uint32    vlan_id,
                                      L7_uint32    tagged);

/*********************************************************************
* @purpose  Delete a port from L2 bitmap for a number of groups
*
* @param   port        {(input)}  BCMX Lport
* @param   *index      {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups  {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id     {(input)} VLAN affected by this call. 
* @param   tagged      {(input)}  not used
*
* @returns BCMX Error Code
*
* @notes This function removes L2 ports from the specified groups. 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_delete_l2_port_groups (bcmx_lport_t port,
                                         L7_uint32    *ipmc_index,
                                         L7_uint32    num_groups,
                                         L7_uint32    vlan_id,
                                         L7_uint32    tagged);
                                  

/*********************************************************************
* @purpose  Set a port in L3 bitmap for a number of groups
*
* @param   port        {(input)}  BCMX Lport
* @param   *index      {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups  {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id     {(input)} VLAN affected by this call.
* @param   tagged      {(input)} Flag indicating whether this VLAN should be tagged.
* @param   mac         {(input)} Mac-address
* @param   ttl         {(input)} ttl
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int
usl_bcmx_ipmc_add_l3_port_groups (bcmx_lport_t port,
                                  L7_uint32    *ipmc_index,
                                  L7_uint32    num_groups,
                                  L7_uint32    vlan_id,
                                  L7_uint32    tagged,
                                  L7_uchar8    *mac,
                                  L7_uint32    ttl);

/*********************************************************************
* @purpose  Remove a port from L3 bitmap for a number of groups
*
* @param   port        {(input)}  BCMX Lport
* @param   *index      {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups  {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id     {(input)} VLAN affected by this call.
* @param   tagged      {(input)} Flag indicating whether this VLAN should be tagged.
* @param   mac         {(input)} Mac-address (not used)
* @param   ttl         {(input)} ttl (not used)
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int
usl_bcmx_ipmc_delete_l3_port_groups (bcmx_lport_t port,
                                     L7_uint32    *ipmc_index,
                                     L7_uint32    num_groups,
                                     L7_uint32    vlan_id,
                                     L7_uint32    tagged,
                                     L7_uchar8    *mac,
                                     L7_uint32    ttl);


/*********************************************************************
* @purpose  Get whether an IPMC Group is in use
*
* @param    ipmc      @{(input)} pointer to the IPMC entry.
* @param    inuse     @{(output)} L7_TRUE: Group is in use
*                                 L7_FALSE: Group is not in use
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_inuse_get(usl_bcm_ipmc_addr_t *ipmc, L7_BOOL *inuse);

/*********************************************************************
* @purpose  Add a WLAN virtual port to an IP multicast group
*
* @param    index       @{(input)} ipmc hardware table index
* @param    l3a_intf_id @{(input)} L3 if id of vlan routing interface
* @param    vlan_id     @{(input)} vlan routing interface vlan  id
* @param    port        @{(input)} wlan gport
* @param    phys_port   @{(input)} physical port where wlan vp is terminated
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_wlan_l3_port_add(L7_int32 index, L7_int32 l3a_intf_id,
                                   L7_uint32 vlan_id, bcm_port_t port,
                                   bcm_gport_t phys_port);

/*********************************************************************
* @purpose  Delete a WLAN virtual port from an IP multicast group
*
* @param    index       @{(input)} ipmc hardware table index
* @param    l3a_intf_id @{(input)} L3 if id of vlan routing interface
* @param    vlan_id     @{(input)} vlan routing interface vlan  id
* @param    port        @{(input)} wlan gport
* @param    phys_port   @{(input)} physical port where wlan vp is terminated
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_wlan_l3_port_delete(L7_int32 index, L7_int32 l3a_intf_id,
                                      L7_uint32 vlan_id, bcm_port_t port,
                                      bcm_gport_t phys_port);
#endif /* L7_MCAST_PACKAGE */
#endif 
