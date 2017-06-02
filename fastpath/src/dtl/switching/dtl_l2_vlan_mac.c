/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2005-2007
 *
 **********************************************************************
 *
 * @filename  dtl_l2_vlan_mac.c
 *
 * @purpose   This file contains the entry points to the Layer 2 interface
 *
 * @component hapi vlanMac
 *
 * @comments
 *
 * @create   05/20/2005 
 *
 * @author   tsrikanth 
 *
 * @end
 *
 **********************************************************************/

#define DTLCTRL_L2_DOT1Q_GLOBALS              /* Enable global space   */
#include "dtlinclude.h"


/**********************************************************************
 * @purpose Apply a configuration for IP VLAN
 *
 * @param   cmd        @b{(input)} Command
 * @param   subnet     @b{(input)} IP subnet
 * @param   netmask    @b{(input)} network mask
 * @param   vlanid     @b{(input)} Vlan identifier
 *
 * @returns L7_SUCCESS if success
 * @returns L7_FAILURE if failure
 *
 * @notes none
 *
 * @end
 **********************************************************************/
L7_RC_t dtlVlanMacVlanConfig(L7_uint32 cmd,L7_enetMacAddr_t mac, L7_uint32 vlanid)
{
    DAPI_USP_t ddUsp;
    DAPI_QVLAN_MGMT_CMD_t dapiCmd;
    L7_RC_t rc = L7_FAILURE;

    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;

    dapiCmd.cmdData.macVlanConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.macVlanConfig.vlanId = vlanid;
    dapiCmd.cmdData.macVlanConfig.mac = mac;

    rc = dapiCtl(&ddUsp, cmd, &dapiCmd);

    if (rc != L7_FAILURE)
        rc = L7_SUCCESS;
    return(rc);
}

/*****************************************************************
 * @purpose Create an IP VLAN
 *
 * @param   vlanid     @b{(input)} VLAN ID
 * @param   mac        @b{(input)} IP mac
 * @param   netmask    @b{(input)} network mask
 *
 * @returns L7_SUCCESS if success
 * @returns L7_FAILURE if failure
 *
 * @notes none
 *
 * @end
 ****************************************************************/
L7_RC_t dtlVlanMacEntryCreate(L7_enetMacAddr_t mac,
                             L7_uint32 vlanid)
{
    L7_RC_t rc;

    rc = dtlVlanMacVlanConfig(DAPI_CMD_MAC_VLAN_CREATE,mac, vlanid);
    return(rc);
}

/*********************************************************************
 * @purpose Delete an IP VLAN
 *
 * @param   vlanid     @b{(input)} VLAN ID
 * @param   mac        @b{(input)} IP mac
 * @param   netmask    @b{(input)} network mask
 *
 * @returns L7_SUCCESS if success
 * @returns L7_FAILURE if failure
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t dtlVlanMacEntryDelete(L7_enetMacAddr_t mac,L7_uint32 vlanid)
{
    L7_RC_t rc;

    rc = dtlVlanMacVlanConfig(DAPI_CMD_MAC_VLAN_DELETE,mac, vlanid);
    return(rc);
}
