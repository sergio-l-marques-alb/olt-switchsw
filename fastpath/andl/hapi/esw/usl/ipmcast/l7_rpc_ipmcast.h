/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_rpc_ipmcast.h
*
* @purpose    New layer to handle directing driver calls to specified units
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

#ifndef L7_RPC_IPMCAST_H
#define L7_RPC_IPMCAST_H

#include "l7_common.h"

#ifdef L7_MCAST_PACKAGE

#include "broad_common.h"
#include "hpc_hw_api.h"
#include "l7_usl_bcm_ipmcast.h"


#define L7_RPC_IPMCAST_IPMC_ADD      HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 30)
#define L7_RPC_IPMCAST_IPMC_DELETE   HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 31)
#define L7_RPC_IPMCAST_RPF_SET       HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 32)
#define L7_RPC_IPMCAST_L2_PORT_SET   HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 34)


/*********************************************************************
* @purpose  Initialize custom IP Multicast RPC calls.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
void l7_custom_rpc_ipmcast_init (void);

/*********************************************************************
* @purpose  Add a IP Mcast entry
*
* @param    data     @{(input)} IPMC BCM data
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_ipmc_add(usl_bcm_ipmc_addr_t *data,
                           L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Delete a IP Mcast entry
*
* @param    *data         @{(input)} The ipmc entry 
* @param    keep          @{(input)} Keep the entry in Table(1) or remove entry(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_ipmc_remove(usl_bcm_ipmc_addr_t *data, int keep,
                              L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Modify the entry in the table
*
* @param    ipmc     @{(input)} IPMC BCM data
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_ipmc_port_tgid_set(usl_bcm_ipmc_addr_t *ipmc,
                                     L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Enable IPMC in the chip
*
* @param    enabled     @{(input)} enable(1) or disable(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_ipmc_enable(int enabled,
                              L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Set L2 ports for an ip multicast group
*
* @param    ipmc_addr    @{(input)} IPMC Address info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_ipmc_set_l2_ports(usl_bcm_ipmc_addr_t *ipmc_addr, 
                                    L7_uint32 numChips, L7_uint32 *chips);


/*********************************************************************
*
* @purpose Add L2 ports to the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
* @param   tagged - Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_add_l2_port_groups (bcmx_lport_t     port,
                                       L7_uint32        *ipmc_index,
                                       L7_uint32         num_groups,
                                       L7_uint32         vlan_id,
                                       L7_uint32         tagged);

/*********************************************************************
*
* @purpose Delete L2 ports from the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_delete_l2_port_groups(bcmx_lport_t port,
                                         L7_uint32    *ipmc_index,
                                         L7_uint32    num_groups,
                                         L7_uint32    vlan_id);

/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
* @param   tagged - Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_add_l3_port_groups (bcmx_lport_t port,
                                       L7_uint32    *ipmc_index,
                                       L7_uint32    num_groups,
                                       L7_uint32    vlan_id,
                                       L7_uint32    tagged,
                                       L7_uchar8    *mac,
                                       L7_uint32    ttl);

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_delete_l3_port_groups (bcmx_lport_t port,
                                          L7_uint32    *ipmc_index,
                                          L7_uint32    num_groups,
                                          L7_uint32    vlan_id);


/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   encap_id - encap_id associated with the port.
*
* @returns BCMX Error Code
*
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_egress_port_add (bcmx_lport_t port,
                                    L7_uint32    *ipmc_index,
                                    L7_uint32    encap_id);

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   encap_id - encap_id associated with the port.
*
* @returns BCMX Error Code
*
*
* @end
*
*********************************************************************/
int
l7_rpc_client_ipmc_egress_port_delete (bcmx_lport_t port,
                                       L7_uint32    *ipmc_index,
                                       L7_uint32    encap_id);

/*********************************************************************
*
* @purpose Handle custom bcmx commands for port configuration 
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} config data
* @param    rv            @{(output)} return value if the command
*                                     was executed
*
*
* @returns L7_TRUE: If the command was handled
*          L7_FALSE: If the command was not handled
*
* @notes 
*
* @end
*
*********************************************************************/
L7_BOOL l7_rpc_server_ipmc_handler(int unit, bcm_port_t port, int setget,
                                   int type, uint32 *args, int *rv);

#endif /* L7_MCAST_PACKAGE */
#endif
