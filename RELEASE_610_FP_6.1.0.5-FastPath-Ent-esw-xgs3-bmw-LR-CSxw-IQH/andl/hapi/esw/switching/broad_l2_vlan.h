/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_l2_vlan.h
*
* @purpose   This file contains the entry points to the Layer 2 interface
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

#ifndef INCLUDE_BROAD_L2_VLAN_H
#define INCLUDE_BROAD_L2_VLAN_H

#include "l7_common.h"
#include "dapi.h"
#include "dapi_trace.h"

#define HAPI_BROAD_NUM_VLAN_STREAMS           128

/* bit positions for GVRP & GMRP in Global reg */
#define HAPI_BROAD_GC_GVRP_ENABLED_BIT        16
#define HAPI_BROAD_GC_GMRP_ENABLED_BIT        17


/*********************************************************************
*
* @purpose Add the specified port to the hardware VLAN configuration.
*
* @param hapiPortPtr - Port to add to the VLAN.
* @param vlanId - Requested VLAN id.
* @param is_tagged - Flag set to one if the VLAN is tagged on the port.
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL2VlanReInit (void);

/*********************************************************************
*
* @purpose Add the specified port to the hardware VLAN configuration.
*
* @param hapiPortPtr - Port to add to the VLAN.
* @param vlanId - Requested VLAN id.
* @param is_tagged - L7_TRUE: Vlan is tagged on this port
*                    L7_FALSE: Vlan is not tagged on this port
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL2VlanAddPortToVlanHw (DAPI_USP_t *usp,
                                    L7_ushort16 vlanId,
                                     L7_BOOL     is_tagged,
                                    DAPI_t      *dapi_g);

/*********************************************************************
*
* @purpose Add/Remove a port from the specified Vlan list.
*
* @param usp                  - Pointer to the port USP
* @param vlanCmd              - L7_TRUE:   Add port to list of vlans
*                               L7_FALSE:  Remove port from list of vlans
* @param vlanMemberSet        - List of vlans
* @param vlanTagSet           - List of tagged vlans 
* @param dapi_g               - Driver object
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL2VlanSetPortToVlanListHw (DAPI_USP_t      *usp,
                                         L7_BOOL          vlanCmd,
                                         L7_VLAN_MASK_t  *vlanMemberSet,
                                         L7_VLAN_MASK_t  *vlanTagSet,
                                         DAPI_t          *dapi_g);

/*********************************************************************
*
* @purpose Remove the specified port from the hardware VLAN configuration.
*
* @param hapiPortPtr - Port to remove from the VLAN.
* @param vlanId - Requested VLAN id.
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL2VlanRemovePortFromVlanHw (DAPI_USP_t *usp,
                                         L7_ushort16 vlanId,
                                         DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Add or remove specified port from hardware VLAN membership.
*
* @param usp
* @param port_cmd    - 1 - Add port to the hardware VLAN membership
*            0 - Remove port from the hardware VLAN membership.
* @param dapi_g
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadAddRemovePortFromVlans (DAPI_USP_t *usp,
                                      L7_uint32   port_cmd,
                                      DAPI_t     *dapi_g);

/*********************************************************************
*
* @purpose Hook in the Vlan functionality and create any data
*
* @param   DAPI_PORT_t *dapiPortPtr - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2VlanPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @purpose Takes the dapiCmd info modifies the NP's table accordingly.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_CREATE
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanCreate
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Takes the dapiCmd info modifies the NP's table accordingly.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_PORT_ADD
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanPortAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanPortAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Takes the dapiCmd info modifies the NP's table accordingly
*          so that a port is deleted from the Vlan.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_PORT_DELETE
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanPortDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanPortDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Takes the dapiCmd info modifies the NP's table accordingly
*          so that all entries in a Vlan table are deleted.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_PURGE
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanPurge
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanPurge(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose To configure vlan membership for a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.portVlanListSet
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   When DAPI_QVLAN_MGMT_CMD_t.cmdData.portVlanListSet.vlanCmd is L7_TRUE,
*          port is added to the vlans. When the vlanCmd is L7_FALSE, port is 
*          deleted from the vlans. 
*          This dapi command helps in vlan scaling by reducing the number of
*          RPC calls required for updating a port's vlan membership during
*          bootup time and clear config.
*           
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanPortVlanListSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose Takes the dapiCmd info and modifies the NP's Vlan table
*          accordingly.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_PORT_CONFIG
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.portConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanPortConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Takes the dapiCmd info and modifies the ports default priority
*		       for untagged frames 
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_PORT_PRIORITY
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.portPriority
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanPortPriority(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose 
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.portPriorityToTcMap
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanPortPriorityToTcMap(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Takes the dapiCmd info and enables GVRP on a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_GARP_GVRP
* @param   void       *data   - DAPI_GARP_MGMT_CMD_t.cmdData.portGvrp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGarpGvrp(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Associate a VLAN w/ a protocol on a port
*
* @param   DAPI_USP_t *usp
* @param   L7_ushort16 etherType
* @param   L7_ushort16 vlanId
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadProtocolVlanAdd(DAPI_USP_t *usp, L7_ushort16 etherType, L7_ushort16 vlanId, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Disassociate a VLAN w/ a protocol on a port
*
* @param   DAPI_USP_t *usp
* @param   L7_ushort16 etherType
* @param   L7_ushort16 vlanId
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadProtocolVlanRemove(DAPI_USP_t *usp, L7_ushort16 etherType, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Acquire the Drivers VLAN Database
*
* @param   void
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVlanCritSecEnter(void);

/*********************************************************************
*
* @purpose Release the Drivers VLAN Database
*
* @param   void
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVlanCritSecExit(void);

/*********************************************************************
*
* @purpose Determine if specified VLAN exists.
*
* @param   vid - Vlan ID.
*
* @returns L7_TRUE - Exists or L7_FALSE - Doesn't exist.
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadQvlanVlanExists(L7_uint32 vid);

/*********************************************************************
*
* @purpose Sets the untagged VLAN (PVID) for the specified port.
*
* @param   DAPI_USP_t   *usp
* @param   L7_ushort16   pvid
* @param   DAPI_t       *dapi_g
*
* @returns L7_RC_t result
*
* @notes   For Strata if the VLAN doesn't exist then a default
*          PVID is assigned. This is because Strata requires a
*          valid VLAN to be assigned to all ports.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortUntaggedVlanSet(DAPI_USP_t *usp, L7_ushort16 pvid, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Sets the acceptable frame type for the specified port.
*
* @param   DAPI_USP_t   *usp
* @param   L7_BOOL       acceptAll
* @param   DAPI_t       *dapi_g
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortAcceptableFrameTypeSet(DAPI_USP_t *usp, L7_uint32 acceptFrameType, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Create a list of vlans in the hardware.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_CREATE
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanCreate
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanListCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Purge a list of vlans
*         
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_PURGE
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanPurge
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanListPurge(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

#endif
