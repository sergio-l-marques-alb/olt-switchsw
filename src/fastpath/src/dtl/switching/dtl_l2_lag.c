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

#include "ptin_globaldefs.h"

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

/*********************************************************************
* @purpose  Get the driver to finish its processing before application 
*           tells the rest of the system.
*           
* @param    timeout {(input)} Time (secs) to wait for driver to finish
*                             pending operations 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlDot3adSynchronize(L7_int32 timeout)
{
  DAPI_LAG_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  dapiCmd.cmdData.lagsSync.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.lagsSync.timeout = timeout;
    
  return dapiCtl(&ddUsp, DAPI_CMD_LAGS_SYNCHRONIZE, &dapiCmd);
    

}
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
  dapiCmd.cmdData.lagPortAdd.hashMode = hashMode;
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
  dapiCmd.cmdData.lagPortDelete.hashMode = hashMode;
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

  dapiCmd.cmdData.lagHashMode.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.lagHashMode.hashMode = hashMode;
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










































/* PTin added */
#if ( LAG_DIRECT_CONTROL_FEATURE )
//#include "dapi_struct.h"
//extern DAPI_t *dapi_g;
//Please check dtlDot3adPort*() in "dtl_l2_lag.c"
L7_RC_t dtlDot3adInternalPortAdd(L7_uint32 lagIfNum,
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
  dapiCmd.cmdData.lagPortAdd.hashMode = hashMode;
  dapiCmd.cmdData.lagPortAdd.memberSet = 
  (numPort != 0) ? addUSP:(DAPI_USP_t *)L7_NULLPTR;

  rc = dapiCtl(&ddUsp,DAPI_CMD_INTERNAL_LAG_PORT_ADD,&dapiCmd); //hapiBroadLagPortAsyncAdd(&ddUsp, DAPI_CMD_INTERNAL_LAG_PORT_ADD, &dapiCmd, dapi_g);  //dapiCtl(&ddUsp,DAPI_CMD_INTERNAL_LAG_PORT_ADD,&dapiCmd);

  return rc;
  
}
L7_RC_t dtlDot3adInternalPortDelete(L7_uint32 lagIfNum, 
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
  dapiCmd.cmdData.lagPortDelete.hashMode = hashMode;
  dapiCmd.cmdData.lagPortDelete.memberSet = 
  (numPort != 0) ? deleteUSP:(DAPI_USP_t *)L7_NULLPTR;

  rc = dapiCtl(&ddUsp,DAPI_CMD_INTERNAL_LAG_PORT_DELETE,&dapiCmd); //hapiBroadLagPortAsyncDelete(&ddUsp, DAPI_CMD_INTERNAL_LAG_PORT_DELETE, &dapiCmd, dapi_g);  //dapiCtl(&ddUsp,DAPI_CMD_INTERNAL_LAG_PORT_DELETE,&dapiCmd);

  return rc;
}
#endif  //( LAG_DIRECT_CONTROL_FEATURE )

/*
**********************************************************************
*                           PRIVATE FUNCTIONS 
**********************************************************************
*/


#endif /*DTLCTRL_COMPONENT_L2_LAG*/
