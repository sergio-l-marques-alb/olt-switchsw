/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_l2_std.h
*
* @purpose   This file includes all the messages for L2 and prototypes
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_L2_STD_H
#define INCLUDE_BROAD_L2_STD_H

#include "l7_common.h"
#include "l7_resources.h"
#include "dapi.h"

/* Number of messages on the dot1s async command queue. 
*/
#define HAPI_BROAD_DOT1S_ASYNC_CMD_QUEUE_SIZE DOT1S_STATE_CHANGE_QUEUE_SIZE

/* Number of messages on the L2 async command queue. This queue
** is used to signal pending work to L2 Async task.
*/
#define HAPI_BROAD_L2_ASYNC_CMD_QUEUE_SIZE  1   

/* Number of dot1s messages processed by L2 Async task before moving
*  to lag messages.
*/
#define HAPI_BROAD_L2_ASYNC_DOT1S_MSG_WEIGHT 120

/* Number of lag messages processed by L2 async task before moving
*  to dot1s messages.
*/
#define HAPI_BROAD_L2_ASYNC_LAG_MSG_WEIGHT   1

 /* Command format for Stp state set.
*/
typedef enum 
{
  HAPI_BROAD_DOT1S_STATE_SET = 1,
  HAPI_BROAD_DOT1S_ASYNC_DONE = 2
} hapi_broad_dot1s_async_cmd_t;


typedef struct 
{
  L7_uchar8           async_cmd;
  L7_uchar8           state; 
  DAPI_USP_t          usp;
  L7_ushort16         instNumber;
  L7_uint32           applicationReference;
  L7_uint64           enqueTime;
} hapi_broad_dot1s_async_msg_t;

typedef struct
{
  L7_uchar8 dummy;
} hapi_broad_l2_async_msg_t;

/*******************************************************************************
*
* @purpose Re-Initialize L2 functions.
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
void hapiBroadL2ReInit(void);

/*******************************************************************************
*
* @purpose Initialize the non port specific Layer 2 functionality
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL2Init(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Hook in the L2 functionality
*
* @param   DAPI_PORT_t *dapiPortPtr - generic driver port
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2StdPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @purpose Create a dot1s spanning tree instance
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sInstanceCreate
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDot1sInstanceCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Delete a dot1s spanning tree instance
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sInstanceDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDot1sInstanceDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Add VLAN to a particular dot1s spanning tree
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sInstanceVlanAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDot1sInstanceVlanAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Remove VLAN from a particular dot1s spanning tree
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sInstanceVlanRemove
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDot1sInstanceVlanRemove(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Set the port state for this usp in the specified spanning tree
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_DOT1S_STATE
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dot1sState
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1sState(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Used to enable/disable Double Vlan Tagging on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.doubleVlanTagConfig 
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDoubleVlanTagConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Used to enable/disable Double Vlan Tagging and set the ethertype
*           on the ports.
*
* @param   *lag_usp       @b{(input)} The USP of the port that is to be acted upon
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    added         @b{(input)} L7_TRUE if port added, L7_FALSE if port removed
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadDoubleVlanLagNotify(DAPI_USP_t *lag_usp, 
                                     DAPI_USP_t *usp, 
                                     L7_BOOL added,
                                     DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose  Used to configure the learning mode on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    flags         @b{(input)} flags to define the learning mode
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  void
*
* @end
*
*********************************************************************/
void hapiBroadLearnSet(DAPI_USP_t *usp, L7_uint32 flags, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Used to configure MAC locking on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_MAC_LOCK_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.macLockConfig 
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfMacLockConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Used to configure LLDP on a physical port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_LLDP_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.lldpConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfLldpConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Copy the dot1s port states from the src USP to dest USP
*
* @param   DAPI_USP_t *destUsp
* @param   DAPI_USP_t *srcUsp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadDot1sPortStateCopy(DAPI_USP_t *destUsp, DAPI_USP_t *srcUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Set the stp state for all groups on a port
*
* @param   DAPI_USP_t *usp
* @param   bcm_stg_stp_t state - the stp state to set the port to
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The destination port must be a physical port.
*
* @end
*
*********************************************************************/
void hapiBroadDot1sPortAllGroupsStateSet(DAPI_USP_t *usp, 
                                         bcm_stg_stp_t state,
                                         DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Set the mapping between instNumber and the Broadcom STG
*
* @param   DAPI_USP_t *destUsp
* @param   DAPI_USP_t *srcUsp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The destination port must be a physical port.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDot1sBcmStgMapSet(L7_int32 instNumber, bcm_stg_t stg);

/*********************************************************************
*
* @purpose Get the mapping between instNumber and the Broadcom STG
*
* @param   DAPI_USP_t *destUsp
* @param   DAPI_USP_t *srcUsp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The destination port must be a physical port.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDot1sBcmStgMapGet(L7_uint32 instNumber, bcm_stg_t *stg, L7_uint32 *stg_index);

/******************************************************************************
* @purpose  Get the list of all spanning tree instances
*
* @param    pStgList   - @b{(output)}  Pointer to an array that stores stg id's
*           pStgCount  - @b{(output)}  Number of stp instances
*
* @returns  none
*
* @notes    Memory should be allocated in the calling function for bcm_stg_t*
*
* @end
******************************************************************************/
void hapiBroadDot1sBcmStgListGet(bcm_stg_t* pStgList, L7_int32* pStgCount);

/* Access routine to support debug routines for iterating */
L7_RC_t hapiBroadDebugDot1sIndexMapGet(L7_uint32 stg_index, L7_int32 *instNumber, bcm_stg_t *stg);

/*********************************************************************
*
* @purpose Clear the mapping between instNumber and the Broadcom STG
*
* @param   DAPI_USP_t *destUsp
* @param   DAPI_USP_t *srcUsp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The destination port must be a physical port.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDot1sBcmStgMapClear(L7_int32 instNumber);

/*********************************************************************
*
* @purpose This routine get the port bit map of ports that are not participating
*          in STP
*
* @param   L7_int32    unit
* @param   DAPI_USP_t *usp
* @param   L7_uint32  *pbmp
* @param   DAPI_t     *dapi_g
*
* @returns NONE
*
* @notes   This routine is for use on reception... it includes all ports
*          w/ link up that are not participating in STP, but excludes the port
*          that originally received the pkt.
*
* @end
*
*********************************************************************/
void hapiBroadGetStpNotParticipatingPbmp(DAPI_USP_t *usp, bcmx_lplist_t *taggedLplist, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Add a MAC address to the network processor's table
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.macAddressEntryAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacAddressEntryAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Add a MAC address to the network processor's table
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.macAddressEntryModify
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacAddressEntryModify(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Add a MAC address to the network processor's table
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.macAddressEntryDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacAddressEntryDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Set the aging time
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_AGING_TIME
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.agingTime
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrAgingTime(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Flush the forwarding entries for the port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_AGING_TIME
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.agingTime
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrFlush(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Flush the VLAN specific MAC Table dynamic entries.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_FLUSH
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.agingTime
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrFlushByVlan(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Flush the MAC specific MAC Table dynamic entries.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_FLUSH
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.agingTime
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrFlushByMac(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Flush all MAC address for all Ports
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_FLUSH_ALL_NAME
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.l2FlushAll
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrFlushAll(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Synchronize all the dynamic L2 entries w/ the FDB application
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_SYNC
* @param   void       *data   - not used
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrSync(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Adds a static filter to the NP
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_MAC_FILTER_ADD
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.macFilterAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*         
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacFilterAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Removes a static filter from the NP
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_ADDR_MAC_FILTER_DELETE
* @param   void       *data   - DAPI_ADDR_MGMT_CMD_t.cmdData.macFilterDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacFilterDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Apply the MAC filter configuration
*
* @param   void
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrMacFilterConfigApply(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notify MAC filters of a LAG modification
*
* @param   void
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadMacFilterLagModifiedNotify(DAPI_USP_t *usp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notifies the DHCP Snooping component that a physical port has been
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDhcpSnoopingLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notifies the DHCP Snooping component that a physical port has been
*          removed from a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadDhcpSnoopingLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose Create a logical interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LOGICAL_INTF_CREATE
* @param   void       *data   - DAPI_LOGICAL_INTF_MGMT_CMD_t.cmdData.logicalIntfCreate
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The implementation of this routine currently only supports 
* @notes	 the creation of a VLAN interface that is used for routing.
* @notes	 FUTURE_FUNC : Likely to be enhanced in the future 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrLogicalIntfCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Delete a logical interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LOGICAL_INTF_DELETE
* @param   void       *data   - DAPI_LOGICAL_INTF_MGMT_CMD_t.cmdData.logicalIntfDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadAddrLogicalIntfDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Create a message queue and task to handle requests to flush
*          L2 address in the hardware either for a port or a lag
*
* @param   DAPI_t *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   Since dynamic L2 addr flushing takes a good 2 sec on 5690
*          we are running that in a different thread
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2AddrFlushInit (DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Flushes the L2 Address for the messsages received
*
* @param   L7_uint32        numArgs  - number of arguments
* @param   DAPI_t          *dapi_g   - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadL2AddrFlushTask(L7_uint32 numArgs, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose Create a ip-sbunet based vlan
*
* @param   *usp    @b{(input)} [unit/slot/port]
* @param   cmd     @b{(input)} [DAPI_CMD_QVLAN_IPVLAN_CONFIG]
* @param   *data   @b{(input)} [Data for command operation]
* @param   *dapi_g @b{(input)} [The driver object]
*
* @returns L7_RC_t result
*
* @notes   The implementation of this routine currently only supports 
* @notes	 the creation of a VLAN interface that is used for routing.
* @notes	 FUTURE_FUNC : Likely to be enhanced in the future 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2VlanIpSubnetEntryAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Delete a ip-sbunet based vlan
*
* @param   *usp    @b{(input)} [unit/slot/port]
* @param   cmd     @b{(input)} [DAPI_CMD_QVLAN_IPVLAN_CONFIG]
* @param   *data   @b{(input)} [Data for command operation]
* @param   *dapi_g @b{(input)} [The driver object]
*
* @returns L7_RC_t result
*
* @notes   The implementation of this routine currently only supports 
* @notes	 the creation of a VLAN interface that is used for routing.
* @notes	 FUTURE_FUNC : Likely to be enhanced in the future 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2VlanIpSubnetEntryDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
/*********************************************************************** 
@purpose  Used to configure DHCP Snooping on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DHCP_FILTER_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dhcpFilterConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
**********************************************************************/
L7_RC_t hapiBroadIntfDhcpSnoopingConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Create a Mac based vlan
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_MAC_VLAN_CREATE
* @param   void       *data   - DAPI_LOGICAL_INTF_MGMT_CMD_t.cmdData.logicalIntfCreate
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The implementation of this routine currently only supports 
* @notes	 the creation of a VLAN interface that is used for routing.
* @notes	 FUTURE_FUNC : Likely to be enhanced in the future 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2VlanMacEntryAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Delete a Mac based vlan
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_MAC_VLAN_DELETE
* @param   void       *data   - DAPI_LOGICAL_INTF_MGMT_CMD_t.cmdData.logicalIntfCreate
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   The implementation of this routine currently only supports 
* @notes	 the creation of a VLAN interface that is used for routing.
* @notes	 FUTURE_FUNC : Likely to be enhanced in the future 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2VlanMacEntryDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

#ifndef L7_STACKING_PACKAGE
/*********************************************************************
*
* @purpose Create a task to handle mac table synchronization
*
* @param   DAPI_t *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes Currently mac-sync task syncs addresses learned on trunk
*        between units on a multiple unit configuration.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2AddrMacSyncInit (DAPI_t *dapi_g);
#endif

/*********************************************************************
*
* @purpose  Configure a group of protected ports
*
* @param   *usp    @b{(input)}  Dummy USP
* @param   cmd     @b{(input)}  DAPI_CMD_PROTECTED_PORT_ADD
* @param   *data   @b{(input)}  DAPI_INTF_MGMT_CMD_t.cmdData.protectedPortConfig
* @param   *dapi_g @b{(input)}  The driver object
*
* @returns L7_RC_t result
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2ProtectedGroupConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Remove a port from a protected group
*
* @param   *usp    @b{(input)}  USP of the port to be removed
* @param   cmd     @b{(input)}  DAPI_CMD_PROTECTED_PORT_DELETE
* @param   *data   @b{(input)}  DAPI_INTF_MGMT_CMD_t.cmdData.protectedPortConfig
* @param   *dapi_g @b{(input)}  The driver object
*
* @returns L7_RC_t result
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2ProtectedPortDelete( DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Create message queues and task to handle spanning-tree state
*          sets and Lag port management asynchronously.
*
* @param   DAPI_t *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   Since Protocol threads get blocked for an unacceptable amount
*          of time for RPCs to finish, we need to handle these requests
*          asynchronously in the driver. For spanning-tree state sets,
*          application expects a callback indicating whether the request
*          was successfully processed or not. For Lags, there is no such
*          mechanism.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2AsyncInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Wait for Dot1s async work to finish.
*
* @param none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadDot1sAsyncDoneWait(void);

/*********************************************************************
*
* @purpose Wakeup the L2 Async task.
*
* @param none
*
* @returns L7_RC_t
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2AsyncTaskSignal(void);

/*********************************************************************
*
* @purpose To override the default Cos on a voice vlan port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_VOICE_VLAN_COS_OVERRIDE
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   We do not support specification of the destination ports
*          on Broadcom.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVoiceVlanCosOverride(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Set Dynamic ARP Inspection trust mode for a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDynamicArpInspectionConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notifies the DynamicArpInspection component that a physical port has been
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDynamicArpInspectionLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notifies the DynamicArpInspection component that a physical port has been
*          removed from a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDynamicArpInspectionLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);



/*********************************************************************
*
* @purpose  To get cable status corrosponding to port
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_CABLE_STATUS_GET
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.cableStatusGet
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Sets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortCableDiag(DAPI_USP_t *usp,DAPI_CMD_t cmd,
                               void *data,DAPI_t *dapi_g);


/*********************************************************************
 *
 * @purpose Installs the DHCP policies for DHCP snooping and elevating
 *          DHCP packet priority
 *
 * @param *dapi_g
 *
 * @returns L7_RC_t
 *
 * @notes   This function is called from hapiBroadSystemPolicyInstall()
            and hapiBroadSystemPolicyInstallRaptor().
 *
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadDhcpPolicyInstall(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Configure the Captive Portal parameters on a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfCaptivePortalConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose  Used to Configure an OUTER TPID  on a port.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.doubleVlanTagConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDoubleVlanTagMultiTpidConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, 
                                                  void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Used to configure Given outer TPID on the port
*           accordingly.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.doubleVlanTagConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDoubleVlanTagDefaultTpidConfig(DAPI_USP_t *usp,
                         DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose  Used to Configure a Default TPID per system.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.doubleVlanTagConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDoubleVlanTagMultiTpidConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd,
                                                  void *data, DAPI_t *dapi_g);



#endif
