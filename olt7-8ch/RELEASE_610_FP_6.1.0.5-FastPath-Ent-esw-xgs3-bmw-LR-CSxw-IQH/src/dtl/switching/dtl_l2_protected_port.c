/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  
*
* @purpose   This file contains the entry points to the Layer 2 interface
*
* @component hapi
*
* @comments
*
* @create    
*
* @author    
*
* @end
*
**********************************************************************/

#include "dtlinclude.h"
#include "dtl_util.h"

static DAPI_USP_t portUsp[L7_MAX_INTERFACE_COUNT + 1]; 

/*************************************************************************
*
* @purpose Sets/Resets the interface list for protected port
*
* @param   cmd        @b{(input)} DAPI command
* @param   groupId    @b{(input)} groupId 
* @param   *portUsp   @b{(input)} Pointer to DAPI_USP_t array 
*
* @returns L7_SUCCESS 
* @returns L7_FAILURE 
*
* @notes none
*
* @end
************************************************************************/
L7_RC_t dtlProtectedPortConfig(L7_uint32 cmd, L7_uint32 groupId,DAPI_USP_t *portUsp)
{
    DAPI_USP_t ddUsp;
    DAPI_INTF_MGMT_CMD_t dapiCmd;
    L7_RC_t rc;

    /*
     * The lower-level HAPI code attempts to apply
     * settings on a specified set of ports.
     */
    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;

    dapiCmd.cmdData.protectedPortConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.protectedPortConfig.groupId = groupId;
    dapiCmd.cmdData.protectedPortConfig.ports = portUsp;

    rc = dapiCtl(&ddUsp, cmd, &dapiCmd);

    if (rc != L7_FAILURE)
  {
        rc = L7_SUCCESS;
  }

    return(rc);
}

/*************************************************************************
*
* @purpose Sets the interface list for protected port
*
* @param   groupId    @b{(input)} groupId 
* @param   portMask   @b{(input)} interface mask
*
* @returns L7_SUCCESS 
* @returns L7_FAILURE 
*
* @notes none
*
* @end
************************************************************************/
L7_RC_t dtlProtectedPortGroupCreate(L7_uint32 groupId, L7_INTF_MASK_t portMask)
{
    L7_RC_t rc;

    dtlIntfMaskToDapiUSPList(portMask,(DAPI_USP_t *)&portUsp);   

    rc = dtlProtectedPortConfig(DAPI_CMD_PROTECTED_PORT_ADD, groupId, portUsp);
    return(rc);
}

/*************************************************************************
*
* @purpose Delete the protected port
*
* @param   intIfNum   @b{(input)} interface number
*
* @returns L7_SUCCESS 
* @returns L7_FAILURE 
*
* @notes Since a port can only be protected in a single group, removing
*        a port from a group means that it can forward to all ports.
*
* @end
************************************************************************/
L7_RC_t dtlProtectedPortDelete( L7_uint32 intIfNum )
{
  DAPI_USP_t           ddUsp;
  nimUSP_t             usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return( L7_FAILURE );
  }

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port-1;

  dapiCmd.cmdData.protectedPortConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.protectedPortConfig.groupId  = 0; /* Dummy */
  dapiCmd.cmdData.protectedPortConfig.ports    = L7_NULL;

  return( dapiCtl( &ddUsp, DAPI_CMD_PROTECTED_PORT_DELETE, &dapiCmd ) );
}
