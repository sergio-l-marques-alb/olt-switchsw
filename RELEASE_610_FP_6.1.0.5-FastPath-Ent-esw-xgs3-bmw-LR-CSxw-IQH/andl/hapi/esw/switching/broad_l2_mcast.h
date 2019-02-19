/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename broad_mcast.h
*
* @purpose Layer 2 Multicast group management.
*
* @component hapi_broad
*
* @comments none
*
* @create 6/26/02
*
* @author Andrey Tsigler
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_L2_MCAST_H
#define INCLUDE_BROAD_L2_MCAST_H

#include "dapi.h"
#include "broad_common.h"

/* Number of L2 multicast entries in the hardware.
*/
#define BROAD_L2_MCAST_TABLE_SIZE (L7_MFDB_MAX_MAC_ENTRIES)

/*********************************************************************
*
* @purpose Re-Init L2 Mcast software
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadL2McastReInit(void);

/*********************************************************************
*
* @purpose Init L2 Mcast software
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2McastInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Hook in the Mcast functionality and create any data
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
L7_RC_t hapiBroadL2McastPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @purpose Takes the dapiCmd info and enables GMRP on a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_GARP_GMRP
* @param   void       *data   - DAPI_GARP_MGMT_CMD_t.cmdData.portGmrp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGarpGmrp(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Enable GMRP on the system
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_GVRP_GMRP_CONFIG
* @param   void       *data   - DAPI_GARP_MGMT_CMD_t.cmdData.gvrpGmrpConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGvrpGmrpConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Add Group Reg entry
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_GARP_GROUP_REG_MODIFY
* @param   void       *data   - DAPI_GARP_MGMT_CMD_t.cmdData.groupRegModify
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGarpGroupRegModify(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Delete Group Registration entry
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_GARP_GROUP_REG_DELETE
* @param   void       *data   - DAPI_GARP_MGMT_CMD_t.cmdData.groupRegDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGarpGroupRegDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
* @purpose  Notify MGM that a port or LAG was added to VLAN in hardware.
*
* @param    *usp
* @param    vlan_id
* @param    *dapi_g
*
* @returns  none
*
* @notes    The function must NOT be called for LAG members.
*
*  @end
*********************************************************************/
void hapiBroadMgmPortVlanAddNotify (DAPI_USP_t *usp, L7_ushort16 vlan_id, DAPI_t *dapi_g);

/*********************************************************************
* @purpose  Notify MGM that a port or LAG was removed from VLAN hardware.
*
* @param    *usp
* @param    vlan_id
* @param    *dapi_g
*
* @returns  none
*
* @notes    The function must NOT be called for LAG members.
*
* @end
*********************************************************************/
void hapiBroadMgmPortVlanRemoveNotify (DAPI_USP_t *usp, L7_ushort16 vlan_id, DAPI_t *dapi_g);

/*********************************************************************
* @purpose  Notify MGM that a port was added to a LAG.
*
* @param    *memberUsp
* @param    *lagUsp
* @param    *dapi_g
*
* @returns  none
*
*  @end
*********************************************************************/
void hapiBroadMgmLagMemberAddNotify (DAPI_USP_t *memberUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
* @purpose  Notify MGM that a port was removed from a LAG
*
* @param    *memberUsp
* @param    *lagUsp
* @param    *dapi_g
*
* @returns  none
*
* @end
*********************************************************************/
void hapiBroadMgmLagMemberRemoveNotify (DAPI_USP_t *memberUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
* @purpose  Notify MGM to add this wlan port
*
* @param    *usp
* @param     vlanId
* @param    *dapi_g
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void hapiBroadMgmL2TunnelAdd(DAPI_USP_t *usp, L7_ushort16 vlanId, DAPI_t *dapi_g);

/*********************************************************************
* @purpose  Notify MGM to remove this wlan port
*
* @param    *usp
* @param     vlanId
* @param    *dapi_g
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void hapiBroadMgmL2TunnelRemove(DAPI_USP_t *usp, L7_ushort16 vlanId, DAPI_t *dapi_g);

/*********************************************************************
* @purpose  Notify MGM that this port/vlan is tagging
*
* @param    *usp
* @param     vlanId
* @param    *dapi_g
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void hapiBroadMgmTaggingEnable(DAPI_USP_t *usp, L7_uint32 vlanId, DAPI_t *dapi_g);

/*********************************************************************
* @purpose  Notify MGM that this port/vlan is not tagging
*
* @param    *usp
* @param     vlanId
* @param    *dapi_g
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void hapiBroadMgmTaggingDisable(DAPI_USP_t *usp, L7_uint32 vlanId, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Enters a critical section for L2MC.
*
* @returns 
*
* @end
*
*********************************************************************/
void hapiBroadL2McastCritSecEnter(void);

/*********************************************************************
*
* @purpose Exits a critical section for L2MC.
*
* @returns 
*
* @end
*
*********************************************************************/
void hapiBroadL2McastCritSecExit(void);

#endif
