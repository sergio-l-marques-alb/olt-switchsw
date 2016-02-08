/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_l2_lag.c
*
* Purpose: This file contains the functions to transform the trunk
*          components's driver requests. 
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: Shekhar Kalyanam 3/14/2001 
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/





/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_L2_LAG_GLOBALS              /* Enable global space   */    
#include "dtlinclude.h"
#include "dot3ad_exports.h"

L7_uint32 bcmLagHashModeMap[] = 
{
  FD_DOT3AD_HASH_MODE,
  L7_DOT3AD_HASH_SA_VLAN_ETYPE_INTF,
  L7_DOT3AD_HASH_DA_VLAN_ETYPE_INTF,
  L7_DOT3AD_HASH_SADA_VLAN_ETYPE_INTF,
  L7_DOT3AD_HASH_SIP_SPORT,
  L7_DOT3AD_HASH_DIP_DPORT,
  L7_DOT3AD_HASH_SIP_DIP_SPORT_DPORT
}; /* Hardware mapping hashMode */

#if DTLCTRL_COMPONENT_L2_LAG


/*
**********************************************************************
*                           PRIVATE FUNCTIONS PROTOTYPES 
**********************************************************************
*/


/*
**********************************************************************
*                           API FUNCTIONS 
**********************************************************************
*/

/*********************************************************************
* @purpose  Enables or disables LACP PDU and Marker Response PDU
*           to be sent to the CPU from an interface 
*           
*
* @param    intIfNum  @b{(input)} Internal Interface Number of the interface
* @param    mode      @b{(input)} L7_ENABLE to enable LACP PDU and Marker 
*                                 Response PDU to be SENT TO THE cpu
*                                 L7_DISABLE to drop these PDUs 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlDot3adLacpSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  DAPI_LAG_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {

    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port-1;

    dapiCmd.cmdData.portADTrunk.getOrSet = DAPI_CMD_SET;
    if (mode == L7_ENABLE)
      dapiCmd.cmdData.portADTrunk.enable = L7_TRUE;
    else
      dapiCmd.cmdData.portADTrunk.enable = L7_FALSE;
    dr = dapiCtl(&ddUsp,DAPI_CMD_AD_TRUNK_MODE_SET,&dapiCmd);
    if (dr == L7_SUCCESS)
      return L7_SUCCESS;
    else
      return L7_FAILURE;

  }

}

// PTin added
L7_RC_t dtlDot3adPendingConfigsOnHW(L7_uint8 *num)
{
  DAPI_USP_t ddUsp;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dr = dapiCtl(&ddUsp,DAPI_CMD_LAG_PENDING_CONFIGS,num);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}
// PTin end

/*********************************************************************
* @purpose  Creates a new LAG  
*           
*
* @param    lagIfNum  @b{(input)} Internal Interface Number of the lag interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlDot3adCreate(DTL_USP_t *lagUsp)
{
  DAPI_LAG_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t dr;



  ddUsp.unit = lagUsp->unit;
  ddUsp.slot = lagUsp->slot;
  ddUsp.port = lagUsp->port - 1;

  dapiCmd.cmdData.lagCreate.getOrSet = DAPI_CMD_SET;

  dr = dapiCtl(&ddUsp,DAPI_CMD_LAG_CREATE,&dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}
/*********************************************************************
* @purpose  
*           
*
* @param    lagIfNum  @b{(input)} Internal Interface Number of the lag interface
* @param    numPort   @b{(input)} Total number of ports to be added
* @param    portIntf  @b{(input)} Internal Interface Number of the port to be added
* @param    hashMode  @b{(input)} Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlDot3adPortAdd(L7_uint32 lagIfNum,
                         L7_uint32 numPort, 
                         L7_uint32 portIntf[],
                         L7_uint32 hashMode)
{
  DAPI_LAG_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  DAPI_USP_t addUSP[L7_MAX_MEMBERS_PER_LAG];
  nimUSP_t usp;
  L7_RC_t rc;
  L7_uint32 i,maxFrameSize;

  if (nimGetUnitSlotPort(lagIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  /* get lag max frame size */
  nimGetIntfConfigMaxFrameSize(lagIfNum, &maxFrameSize);

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  for (i = 0; i < numPort; i++)
  {
    if (nimGetUnitSlotPort(portIntf[i], &usp) != L7_SUCCESS)
      return L7_FAILURE;

    addUSP[i].unit = usp.unit;
    addUSP[i].slot = usp.slot;
    addUSP[i].port = usp.port-1;
  }

  dapiCmd.cmdData.lagPortAdd.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.lagPortAdd.numOfMembers = numPort;
  dapiCmd.cmdData.lagPortAdd.maxFrameSize = maxFrameSize;
  dapiCmd.cmdData.lagPortAdd.hashMode = bcmLagHashModeMap[hashMode];
  dapiCmd.cmdData.lagPortAdd.memberSet = 
  (numPort != 0) ? addUSP:(DAPI_USP_t *)L7_NULLPTR;

  rc = dapiCtl(&ddUsp,DAPI_CMD_LAG_PORT_ADD,&dapiCmd);

  return rc;
  
}
/*********************************************************************
* @purpose  Enables or disables LACP PDU and Marker Response PDU
*           to be sent to the CPU from an interface 
*           
*
* @param    intIfNum  @b{(input)} Internal Interface Number of the interface
* @param    mode      @b{(input)} L7_ENABLE to enable LACP PDU and Marker 
*                                 Response PDU to be SENT TO THE cpu
*                                 L7_DISABLE to drop these PDUs 
* @param    portIntf  @b{(input)} Internal Interface Number of the port to be added
* @param    hashMode  @b{(input)} Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlDot3adPortDelete(L7_uint32 lagIfNum, 
                            L7_uint32 numPort, 
                            L7_uint32 portIntf[],
                            L7_uint32 hashMode)
{
  DAPI_LAG_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  DAPI_USP_t deleteUSP[L7_MAX_MEMBERS_PER_LAG];
  nimUSP_t usp;
  L7_RC_t rc;
  L7_uint32 i, maxFrameSize;

  if (nimGetUnitSlotPort(lagIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  for (i = 0; i < numPort; i++)
  {
    if (nimGetUnitSlotPort(portIntf[i], &usp) != L7_SUCCESS)
      return L7_FAILURE;

    deleteUSP[i].unit = usp.unit;
    deleteUSP[i].slot = usp.slot;
    deleteUSP[i].port = usp.port-1;
    /* get port max frame size */
    nimGetIntfConfigMaxFrameSize(portIntf[i], &maxFrameSize);
  }

  dapiCmd.cmdData.lagPortDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.lagPortDelete.numOfMembers = numPort;
  dapiCmd.cmdData.lagPortDelete.maxFrameSize = maxFrameSize; /* rhelbaoui */
  dapiCmd.cmdData.lagPortDelete.hashMode = bcmLagHashModeMap[hashMode];
  dapiCmd.cmdData.lagPortDelete.memberSet = 
  (numPort != 0) ? deleteUSP:(DAPI_USP_t *)L7_NULLPTR;

  rc = dapiCtl(&ddUsp,DAPI_CMD_LAG_PORT_DELETE,&dapiCmd);

  return rc;
}
/*********************************************************************
* @purpose  Enables or disables LACP PDU and Marker Response PDU
*           to be sent to the CPU from an interface 
*           
*
* @param    intIfNum  @b{(input)} Internal Interface Number of the interface
* @param    mode      @b{(input)} L7_ENABLE to enable LACP PDU and Marker 
*                                 Response PDU to be SENT TO THE cpu
*                                 L7_DISABLE to drop these PDUs 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlDot3adLagDelete(L7_uint32 lagIfNum)
{
  DAPI_LAG_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(lagIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.lagDelete.getOrSet = DAPI_CMD_SET;

  dr = dapiCtl(&ddUsp,DAPI_CMD_LAG_DELETE,&dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Hash Mode for a LAG
*
*
* @param    intIfNum  @b{(input)} Internal Interface Number of the interface
* @param    hashMode  @b{(input)} Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlDot3adHashModeSet (L7_uint32 lagIfNum, L7_uint32 hashMode)
{
  DAPI_LAG_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;
  L7_uint32 newHashMode;

  memset(&dapiCmd, 0, sizeof(DAPI_LAG_MGMT_CMD_t));
  memset(&ddUsp, 0, sizeof(DAPI_USP_t));
  memset(&usp, 0, sizeof(nimUSP_t));

  if (nimGetUnitSlotPort(lagIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  newHashMode = bcmLagHashModeMap[hashMode];
  dapiCmd.cmdData.lagHashMode.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.lagHashMode.hashMode = newHashMode;
  dr = dapiCtl(&ddUsp,DAPI_CMD_LAG_HASHMODE_SET,&dapiCmd);
  if (dr == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

/*
**********************************************************************
*                           PRIVATE FUNCTIONS 
**********************************************************************
*/


#endif /*DTLCTRL_COMPONENT_L2_LAG*/
