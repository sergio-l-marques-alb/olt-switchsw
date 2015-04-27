/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_rpc_l2_mcast.h
*
* @purpose    New layer to handle directing driver calls to specified units
*
* @component  HAPI
*
* @comments   none
*
* @create     07/14/2006
*
* @author     mbaucom
* @end
*
**********************************************************************/
#include "l7_common.h"

#ifndef L7_RPC_L2_MCAST_H
#define L7_RPC_L2_MCAST_H

#include "hpc_hw_api.h"
#include "bcm/types.h"

#include "l7_usl_bcm_l2.h"
#include "l7_usl_bcm.h"

#define L7_RPC_L2MCAST_HANDLER       HPC_RPC_FUNCTION_ID (L7_DRIVER_COMPONENT_ID, 40)
 
typedef enum
{
 L7_RPC_CMD_MCAST_ADDR_UPDATE = 1,
 L7_RPC_CMD_MCAST_PORT_UPDATE

} l7RpcL2McastCmd_t;

/*********************************************************************
* @purpose  Initialize custom IP Multicast RPC calls.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_l2_mcast_rpc_init (void);


/*********************************************************************
* @purpose  Add/Remove a L2 Multicast MAC entry to the HW's Database
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
* @param    updateCmd    @{(input)} Add/Remove the multicast mac
* @param    numChips     @{(input)} The number of bcm unit IDs in the list
* @param    chips        @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int  l7_rpc_client_mcast_addr_update(usl_bcm_mcast_addr_t *mcAddr, USL_CMD_t updateCmd,
                                     L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Add/Remove ports to/from a L2 Multicast Group 
*
* @param    mcAddr       @{(input)} Multicast address information
* @param    updateCmd    @{(input)} Add/Remove the multicast mac
* @param    numChips     @{(input)} The number of bcm unit IDs in the list
* @param    chips        @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int  l7_rpc_client_mcast_update_ports(usl_bcm_mcast_addr_t *mcAddr, 
                                      USL_CMD_t updateCmd,
                                      L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  RPC client API to add a port to a list of L2 Multicast addresses
*
* @param    port              @{(input)} port being operated on
* @param    l2mc_index        @{(input)} array of L2MC indices
* @param    l2mc_index_count  @{(input)} count of L2MC indices
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_mcast_port_update_groups(bcm_gport_t port, int *l2mc_index, int l2mc_index_count, USL_CMD_t updateCmd);

#endif
