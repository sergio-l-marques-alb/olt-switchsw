/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_intf.c
*
* Purpose: This file contains the functions to transform the network
*          interface manager's driver requests. 
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: Shekhar Kalyanam 3/14/2001 
*
*********************************************************************/




/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_INTF_GLOBALS               /* Enable global space   */    
#include "dtlinclude.h"
#include "usmdb_util_api.h"
#include "dot1s_exports.h"

/*#include "nimapi.h"*/                    /* My parent to adapt    */

#if DTLCTRL_COMPONENT_INTF

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
* @purpose  Sets the speed of the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    intfSpeed   @b{(input)} Interface Speed,
*                       (@b{  L7_PORTCTRL_PORTSPEED_AUTO_NEG,
*                             L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                             L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                             L7_PORTCTRL_PORTSPEED_FULL_10T,
*                             L7_PORTCTRL_PORTSPEED_HALF_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                             L7_PORTCTRL_PORTSPEED_FULL_1000SX})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlIntfSpeedSet(L7_uint32 intIfNum, 
                        L7_uint32 intfSpeed)
{
  /* If interface is AAL5, no call to dapi needed */
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_uint32 speed = 0, duplex = 0;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {

    return L7_FAILURE;

  }
  else
  {

    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    /* explicitly hit all cases for clarity */
    switch (intfSpeed)
    {
      case L7_PORTCTRL_PORTSPEED_FULL_100TX:
        duplex = DAPI_PORT_DUPLEX_FULL;  speed = DAPI_PORT_SPEED_FE_100MBPS;
        break;

      case L7_PORTCTRL_PORTSPEED_HALF_100TX:
        duplex = DAPI_PORT_DUPLEX_HALF;  speed = DAPI_PORT_SPEED_FE_100MBPS;
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_10T:
        duplex = DAPI_PORT_DUPLEX_FULL;  speed = DAPI_PORT_SPEED_FE_10MBPS;
        break;

      case L7_PORTCTRL_PORTSPEED_HALF_10T:
        duplex = DAPI_PORT_DUPLEX_HALF;  speed = DAPI_PORT_SPEED_FE_10MBPS;
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_100FX:
        duplex = DAPI_PORT_DUPLEX_FULL;  speed = DAPI_PORT_SPEED_FE_100MBPS;
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
        duplex = DAPI_PORT_DUPLEX_FULL;  speed = DAPI_PORT_SPEED_GE_1GBPS;
        break;

      /* PTin added: Speed 2.5G */
      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
        duplex = DAPI_PORT_DUPLEX_FULL;  speed = DAPI_PORT_SPEED_GE_2G5BPS;
        break;
      /* PTin end */

      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
        duplex = DAPI_PORT_DUPLEX_FULL;  speed = DAPI_PORT_SPEED_GE_10GBPS;
        break;

      /* PTin added: Speed 40G */
      case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
        duplex = DAPI_PORT_DUPLEX_FULL;  speed = DAPI_PORT_SPEED_GE_40GBPS;
        break;

      /* PTin added: Speed 100G */
      case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
        duplex = DAPI_PORT_DUPLEX_FULL;  speed = DAPI_PORT_SPEED_GE_100GBPS;
        break;
      /* PTin end */

      case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
        duplex = DAPI_PORT_DUPLEX_FULL;  speed = DAPI_PORT_AUTO_NEGOTIATE;
        break;
    }

    dapiCmd.cmdData.portSpeedConfig.duplex = duplex;
    dapiCmd.cmdData.portSpeedConfig.speed = speed;

  }

  dapiCmd.cmdData.portSpeedConfig.getOrSet = DAPI_CMD_SET;

  dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_SPEED_CONFIG, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Gets the speed of the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *intfSpeed  @b{(output)} Pointer to Interface Speed,
*                       (@b{  L7_PORTCTRL_PORTSPEED_AUTO_NEG,
*                             L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                             L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                             L7_PORTCTRL_PORTSPEED_FULL_10T,
*                             L7_PORTCTRL_PORTSPEED_HALF_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                             L7_PORTCTRL_PORTSPEED_FULL_1000SX})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlIntfSpeedGet(L7_uint32 intIfNum, 
                        L7_uint32 *intfSpeed)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t rc;
  L7_uint32 duplex, speed;
  L7_uint32 linkState;
  L7_uint32 ifType;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    if (nimGetIntfLinkState(intIfNum, &linkState) != L7_SUCCESS)
    {
      return L7_FAILURE;   
    }
    if (linkState != L7_UP)/* check if port is up */
    {                                            /* if port is not up (down) port has */
      *intfSpeed = L7_PORTCTRL_PORTSPEED_UNKNOWN;/* "unknown" speed. operation        */
      rc = L7_SUCCESS;                           /* suceeded even though port is down */
    }
    else
    {
      ddUsp.unit = usp.unit;
      ddUsp.slot = usp.slot;
      ddUsp.port = usp.port - 1;

      dapiCmd.cmdData.portSpeedConfig.getOrSet = DAPI_CMD_GET;

      dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_SPEED_CONFIG, &dapiCmd);
      if (dr != L7_SUCCESS)
      {
        return L7_FAILURE;

      }
      rc = L7_SUCCESS;

      duplex = dapiCmd.cmdData.portSpeedConfig.duplex;
      speed = dapiCmd.cmdData.portSpeedConfig.speed;

      if (duplex == DAPI_PORT_DUPLEX_FULL)
      {
        switch (speed)
        {
          case DAPI_PORT_SPEED_FE_10MBPS:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_10T;
            break;

          case DAPI_PORT_SPEED_FE_100MBPS:
            rc = nimGetIfType(intIfNum, &ifType);
            if (ifType == L7_IANA_FAST_ETHERNET_FX)
              *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_100FX; /* FX! */
            else
              *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_100TX;
            break;

          case DAPI_PORT_SPEED_GE_1GBPS:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_1000SX;
            break;

          /* PTin added: Speed 2.5G */
          case DAPI_PORT_SPEED_GE_2G5BPS:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_2P5FX;
            break;
          /* PTin end */

          case DAPI_PORT_SPEED_GE_10GBPS:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_10GSX;
            break;

          /* PTin added: Speed 40G */
          case DAPI_PORT_SPEED_GE_40GBPS:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_40G_KR4;
            break;

          /* PTin added: Speed 100G */
          case DAPI_PORT_SPEED_GE_100GBPS:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_FULL_100G_BKP;
            break;
          /* PTin end */

          case DAPI_PORT_AUTO_NEGOTIATE:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_AUTO_NEG;
            break;

          default:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_UNKNOWN;
            break;
    
        }
      }
      else /* duplex == half */
      {
        switch (speed)
        {
          case DAPI_PORT_SPEED_FE_10MBPS:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_HALF_10T;
            break;

          case  DAPI_PORT_SPEED_FE_100MBPS:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_HALF_100TX;
            break;

          default:
            *intfSpeed = L7_PORTCTRL_PORTSPEED_UNKNOWN;
            break;

        }
      }
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Test the copper cable and retrieve test results.
*			The length values are reported in meters.
*
* @param    intIfNum    		 @b{(input)} Internal Interface Number
* @param    *cableStatus  		 @b{(output)} status of the copper cable.
* @param    *lengthKnown  		 @b{(output)} when L7_TRUE the length is known.
* @param    *shortestLength  	 @b{(output)} low end of the length estimate.
* @param    *longestLength  	 @b{(output)} high end of the length estimate.
* @param    *cableFailureLength  @b{(output)} failure location.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The function may take a couple of seconds to execute. 
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlintfCableStatusGet(L7_uint32 intIfNum, 
                                      L7_CABLE_STATUS_t *cableStatus,
                                      L7_BOOL   *lengthKnown,
                                      L7_uint32 *shortestLength,
                                      L7_uint32 *longestLength,
                                      L7_uint32 *cableFailureLength
                                     )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.cableStatusGet.getOrSet = DAPI_CMD_GET;

    dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_CABLE_STATUS_GET, &dapiCmd);
    if (dr != L7_SUCCESS)
    {
      return dr;
    }

    switch (dapiCmd.cmdData.cableStatusGet.cable_status)
    {
      case DAPI_CABLE_TEST_FAIL:
        *cableStatus = L7_CABLE_TEST_FAIL;
        break;

      case DAPI_CABLE_NORMAL:
        *cableStatus = L7_CABLE_NORMAL;
        break;

      case DAPI_CABLE_OPEN:
        if (dapiCmd.cmdData.cableStatusGet.cable_failure_length == 0)
        {
          *cableStatus = L7_CABLE_NOCABLE;
        }
        else
        {
          *cableStatus = L7_CABLE_OPEN;
        }
        break;

      case DAPI_CABLE_SHORT:
        *cableStatus = L7_CABLE_SHORT;
        break;

      case DAPI_CABLE_OPENSHORT:
        *cableStatus = L7_CABLE_OPENSHORT;
        break;

      case  DAPI_CABLE_CROSSTALK:
        *cableStatus = L7_CABLE_CROSSTALK;
        break;
    }

    if (dapiCmd.cmdData.cableStatusGet.length_known == L7_TRUE)
    {
      *lengthKnown = L7_TRUE;
    }
    else
    {
      *lengthKnown = L7_FALSE;
    }

    *shortestLength = dapiCmd.cmdData.cableStatusGet.shortest_length;
    *longestLength = dapiCmd.cmdData.cableStatusGet.longest_length;
    *cableFailureLength = dapiCmd.cmdData.cableStatusGet.cable_failure_length;
  };

  return rc;
}
/* ALT - HACK - Temporary test function.
*/
void test_CableStatusGet(L7_uint32 intIfNum)
{
  L7_CABLE_STATUS_t cableStatus;
  L7_BOOL   lengthKnown;
  L7_uint32 shortestLength;
  L7_uint32 longestLength;
  L7_uint32 cableFailureLength;
  L7_RC_t rc;

  rc =  dtlintfCableStatusGet(intIfNum, 
                              &cableStatus, 
                              &lengthKnown,
                              &shortestLength,
                              &longestLength,
                              &cableFailureLength);
  printf("rc = %d, cableStatus = %d, lengthKnown = %d\n", 
         rc, cableStatus, lengthKnown);

  printf("shortestLength = %d, longestLength = %d, cableFailureLength = %d\n",
         shortestLength, longestLength, cableFailureLength);

}

/*********************************************************************
* @purpose  Sets the administrative state of the specified interface to 
*           either enabled or disabled
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    adminState  @b{(input)} Admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlIntfAdminStateSet(L7_uint32 intIfNum, 
                             L7_uint32 adminState)
{
  /* Need to get design point on how to handle disabling of AAL5 intf   */
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;


  dapiCmd.cmdData.portIsolatePhyConfig.getOrSet = DAPI_CMD_SET;

  if (adminState == L7_ENABLE)
    dapiCmd.cmdData.portIsolatePhyConfig.enable = L7_FALSE;
  else
    dapiCmd.cmdData.portIsolatePhyConfig.enable = L7_TRUE; 

  if (dapiCtl(&ddUsp,DAPI_CMD_INTF_ISOLATE_PHY_CONFIG, &dapiCmd) != L7_SUCCESS)
    return L7_FAILURE;
  else
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Sets the (Layer 2) Maximum Framesize for the interface
*           based upon the type of interface and any submembers per port
*           
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    length      @b{(input)} Maximum frame size of the interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t dtlIntfMaxFrameSizeSet(L7_uint32 intIfNum, 
                               L7_uint32 length)
{

  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;
  DAPI_INTF_MGMT_CMD_t dapiCmd;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {

    return L7_FAILURE;

  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.frameSizeConfig.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.frameSizeConfig.maxFrameSize = length;

    dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG, &dapiCmd);
    if (dr == L7_SUCCESS)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }

}

/*********************************************************************
* @purpose  Creates a logical interface in the driver
*
* @param    intIfNum    @b{(input)} Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This routine can only be used to create a logical interface
*
* @end
*********************************************************************/
L7_RC_t dtlIntfLogicalIntfCreate(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;
  DAPI_LOGICAL_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t   usp;
  DAPI_USP_t ddUsp;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FAILURE;

  switch (sysIntfType) 
  {
    case L7_LOGICAL_VLAN_INTF:
      dapiCmd.cmdData.logicalIntfCreate.type = L7_IANA_L2_VLAN;
      break;

    case L7_TUNNEL_INTF:
      dapiCmd.cmdData.logicalIntfCreate.type = L7_IANA_TUNNEL;
      break;

    default:
      return L7_FAILURE;
  }

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.logicalIntfCreate.getOrSet = DAPI_CMD_SET;

  if (dapiCtl(&ddUsp, DAPI_CMD_LOGICAL_INTF_CREATE, &dapiCmd) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Deletes a logical interface in the driver
*
* @param    intIfNum    @b{(input)} Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This routine can only be used to delete a logical interface
*
* @end
*********************************************************************/
L7_RC_t dtlIntfLogicalIntfDelete(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;
  DAPI_LOGICAL_INTF_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FAILURE;

  if (sysIntfType != L7_LOGICAL_VLAN_INTF && sysIntfType != L7_TUNNEL_INTF)
    return L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.logicalIntfDelete.getOrSet = DAPI_CMD_SET;

  if (dapiCtl(&ddUsp, DAPI_CMD_LOGICAL_INTF_DELETE, &dapiCmd) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the interface in loopback mode.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} Link state
*                       (@b{  L7_PORTCTRL_LOOPBACK_MAC,
*                             L7_PORTCTRL_LOOPBACK_PHY,
*                             L7_PORTCTRL_LOOPBACK_NONE }}
* @param    usp         @b{(input)} NIM usp structure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t dtlIntfLoopbackModeSet(L7_uint32 intIfNum, 
                               L7_uint32 mode, 
                               nimUSP_t usp)
{

  DAPI_INTF_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

 /* Loopback mode if MAC-PHY */
  dapiCmd.cmdData.portLoopbackConfig.loopMode = mode;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.portLoopbackConfig.getOrSet = DAPI_CMD_SET;
  dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_LOOPBACK_CONFIG, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}
/*********************************************************************
* @purpose  Puts the specified interface into the equivalent of STP
*           Disabled mode
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    "Soft Disable" is a quiescent state whereby an interface
*           discards all incoming and outgoing packets.  It is
*           operationally equivalent to STP Disabled mode.  When issued
*           to a macroport interface, the device driver puts all 
*           macroport members into "Soft Disabled".
*
* @notes    There is not a separate DTL command to release an interface
*           from "Soft Disable".  This happens naturally when commands
*           are issued to the interface during normal course of operation,
*           such as when told to go into forwarding mode.
*
* @end
*********************************************************************/
L7_RC_t dtlIntfSoftDisable(L7_uint32 intIfNum)
{
  L7_uint32 instIndex;
  
  /* IEEE 802.1s Support for Multiple Spanning Tree */

  for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
	dtlDot1sStateSet(instIndex, intIfNum, L7_DOT1S_DISABLED, L7_NULL);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configure mirroring session configuration data
*
* @param   *dtlMirrorCmd  @b{(input)} Mirroring configuration data
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes none
*
* @end
*
*********************************************************************/

L7_RC_t dtlMirrorSessionConfig(DTL_MIRROR_t *dtlMirrorCmd)
{
  DAPI_SYSTEM_CMD_t  dapiCmd;
  nimUSP_t           usp;
  DAPI_USP_t         ddUsp;
  DAPI_USP_PROBE_t   srcProbeTypeUSP[L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION];
  dtlMirrorSrcPortInfo_t *srcPortInfoList;
  L7_uint32          numOfSrcPortEntries;
  L7_uint32          i;
  L7_RC_t            dr;
  
  /*
   * Fill dapi structure for probe port USP
  */
  dapiCmd.cmdData.mirroring.getOrSet = DAPI_CMD_SET;

  dapiCmd.cmdData.mirroring.enable = dtlMirrorCmd->cmdType.config.enable;
  dapiCmd.cmdData.mirroring.probeSpecified = dtlMirrorCmd->cmdType.config.probeSpecified;

  if (dtlMirrorCmd->cmdType.config.probeSpecified == L7_TRUE)
  {
    if (nimGetUnitSlotPort(dtlMirrorCmd->cmdType.config.destPort,&usp) != L7_SUCCESS)
        return L7_FAILURE;
    
    dapiCmd.cmdData.mirroring.probeUsp.unit = usp.unit;
    dapiCmd.cmdData.mirroring.probeUsp.slot = usp.slot;
    dapiCmd.cmdData.mirroring.probeUsp.port = usp.port - 1;
  }

  srcPortInfoList = dtlMirrorCmd->cmdType.config.srcPortInfo;
  
  numOfSrcPortEntries = dtlMirrorCmd->cmdType.config.numOfSrcPortEntries;
  for(i = 0; i < numOfSrcPortEntries; i++)
  {
    if (nimGetUnitSlotPort(srcPortInfoList[i].intIfNum, &usp) != L7_SUCCESS)
         return L7_FAILURE;

    srcProbeTypeUSP[i].srcUsp.unit = usp.unit; 
    srcProbeTypeUSP[i].srcUsp.slot = usp.slot;
    srcProbeTypeUSP[i].srcUsp.port = usp.port-1;
   /* NOTE: Following left side uses type DAPI_MIRROR_DIRECTION_t whereas the
           right side uses type L7_MIRROR_DIRECTION_t. Make sure those types 
           are in sync. */
    /* explicit conversion between L7_MIRROR_DIRECTION_t and DAPI_MIRROR_DIRECTION_t*/

      if(dtlMirrorCmd->cmdType.config.srcPortInfo[i].probeType == L7_MIRROR_BIDIRECTIONAL)
      {
        srcProbeTypeUSP[i].probeType = DAPI_MIRROR_BIDIRECTIONAL;
      }
      else if (dtlMirrorCmd->cmdType.config.srcPortInfo[i].probeType == L7_MIRROR_INGRESS)
      {
        srcProbeTypeUSP[i].probeType = DAPI_MIRROR_INGRESS;
      }
      else if (dtlMirrorCmd->cmdType.config.srcPortInfo[i].probeType == L7_MIRROR_EGRESS )
      {
        srcProbeTypeUSP[i].probeType = DAPI_MIRROR_EGRESS;
      }
   }

  dapiCmd.cmdData.mirroring.mirrorProbeUsp = srcProbeTypeUSP;
  dapiCmd.cmdData.mirroring.numMirrors = numOfSrcPortEntries;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_MIRRORING, &dapiCmd);

  if (dr == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Configure mirroring session configuration data
*
* @param   *dtlMirrorCmd  @b{(input)} Mirroring configuration data
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes none
*
* @end
*
*********************************************************************/

L7_RC_t dtlMirrorPortModify(DTL_MIRROR_t *dtlMirrorCmd)
{
  DAPI_SYSTEM_CMD_t  dapiCmd;
  nimUSP_t           usp;
  DAPI_USP_t         ddUsp;
  L7_RC_t            dr;
  
  /*
   * Fill dapi structure for probe port USP
  */
  dapiCmd.cmdData.mirroringPortModify.getOrSet = DAPI_CMD_SET;

  dapiCmd.cmdData.mirroringPortModify.add = dtlMirrorCmd->cmdType.modify.add;

  if (nimGetUnitSlotPort(dtlMirrorCmd->cmdType.modify.srcPort,&usp) != L7_SUCCESS)
      return L7_FAILURE;
  
  dapiCmd.cmdData.mirroringPortModify.mirrorUsp.unit = usp.unit;
  dapiCmd.cmdData.mirroringPortModify.mirrorUsp.slot = usp.slot;
  dapiCmd.cmdData.mirroringPortModify.mirrorUsp.port = usp.port - 1;
  /* NOTE: Following left side uses type DAPI_MIRROR_DIRECTION_t whereas the
           right side uses type L7_MIRROR_DIRECTION_t. Make sure those types 
           are in sync. */
  dapiCmd.cmdData.mirroringPortModify.probeType = 
                  (DAPI_MIRROR_DIRECTION_t)dtlMirrorCmd->cmdType.modify.probeType;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_MIRRORING_PORT_MODIFY, &dapiCmd);

  if (dr == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}


/*********************************************************************
* @purpose  Gets the Maximum Transfer Unit for the interface
*          
* @param        intIfNum  @b{(input)} Internal Interface Number
* @param        *mtuSize  @b{(output)} MTU of the interface
*   
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlIntfMtuSizeGet(L7_uint32 intIfNum, 
                          L7_uint32 *mtuSize)
{
#if 0
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &nimusp) == L7_FAILURE)
    return L7_FAILURE;

  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port -1;

  dapiCmd.cmdData.frameSizeConfig.getOrSet = DAPI_CMD_GET;

  dr = dapiCtl(&ddusp,DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG,&dapiCmd);
  if (dr == L7_SUCCESS)
  {
    *mtuSize = dapiCmd.cmdData.frameSizeConfig.maxFrameSize;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

#else
  /* colinw: The above code has been disabled since the driver doesn't support 
             the operation */
  return L7_SUCCESS;
#endif

}

L7_RC_t dtlIntfMacAddrTypeSet(L7_uint32 intIfNum, L7_uint32 addrType)
{
  return L7_NOT_IMPLEMENTED_YET;

}

/*********************************************************************
* @purpose  Test the copper cable and retrieve test results.
*			The length values are reported in meters.
*
* @param    intIfNum    		 @b{(input)} Internal Interface Number
* @param    *sfpLinkUp		     @b{(output)} sfp Link on interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlintfSfpLinkUpGet(L7_uint32 intIfNum, L7_BOOL *sfpLinkUp)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.portSpeedConfig.getOrSet = DAPI_CMD_GET;

    dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_SPEED_CONFIG, &dapiCmd);
    if (dr== L7_FAILURE)
    {
      return L7_FAILURE;

    }
    *sfpLinkUp = dapiCmd.cmdData.portSpeedConfig.isSfpLinkUp; 
  }
  return rc;
}

/*********************************************************************
* @purpose  Gets the Link State of the specified interface
*
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    *isLinkUp   @b{(output)}  The link state of the interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlIntfLinkStateGet(L7_uint32 intIfNum, L7_BOOL *isLinkUp)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t rc = L7_SUCCESS;

  if ((rc = nimGetUnitSlotPort(intIfNum, &usp)) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.portSpeedConfig.getOrSet = DAPI_CMD_GET;

    if (dapiCtl(&ddUsp, DAPI_CMD_INTF_SPEED_CONFIG, &dapiCmd) == L7_SUCCESS)
    {
      *isLinkUp = dapiCmd.cmdData.portSpeedConfig.isEnabled &&  /* PTin added: consider also admin configuration */
                  dapiCmd.cmdData.portSpeedConfig.isLinkUp;
      rc = L7_SUCCESS;
    }
    else
      rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Gets the Rx and Tx pause State of the specified interface
*
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    *isRxPauseAgreed   @b{(output)}  L7_TRUE if the link partners
*                                     have agreed to use RX Pause
* @param    *isTxPauseAgreed   @b{(output)}  L7_TRUE if the link partners
*                                     have agreed to use TX Pause
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlIntfPauseStateGet(L7_uint32 intIfNum, L7_BOOL *isRxPauseAgreed, L7_BOOL *isTxPauseAgreed)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t rc = L7_SUCCESS;

  if ((rc = nimGetUnitSlotPort(intIfNum, &usp)) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.portSpeedConfig.getOrSet = DAPI_CMD_GET;

    if (dapiCtl(&ddUsp, DAPI_CMD_INTF_SPEED_CONFIG, &dapiCmd) == L7_SUCCESS)
    {
      *isRxPauseAgreed = dapiCmd.cmdData.portSpeedConfig.isRxPauseAgreed;
      *isTxPauseAgreed = dapiCmd.cmdData.portSpeedConfig.isTxPauseAgreed;
      rc = L7_SUCCESS;
    }
    else
      rc = L7_FAILURE;
  }

  return rc;
}

/*******************************************************************************
* @purpose  Enable/disable auto-negotiation and set the negotiation capabilities
*           of the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    negoCapabilities  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************************/
L7_RC_t dtlIntfNegotiationCapabilitiesSet(L7_uint32 intIfNum, L7_uint32 negoCapabilities)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t dr;
  L7_uint32 phyCapability;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.autoNegotiateConfig.abilityMask = 0;

  /* Convert the application level capabilities mask to DAPI level mask. 
     If the ability mask is zero, it means the auto-negotiation should be
     turned off. */
  if (negoCapabilities == 0)
  {
    dapiCmd.cmdData.autoNegotiateConfig.abilityMask = 0;
  }
  else if (negoCapabilities & L7_PORT_NEGO_CAPABILITY_ALL)
  { 
    nimGetIntfPhyCapability(intIfNum, &phyCapability);

    dapiCmd.cmdData.autoNegotiateConfig.abilityMask = 0;

    if (phyCapability & L7_PHY_CAP_PORTSPEED_HALF_10)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_10MB_HD;
    }
    if (phyCapability & L7_PHY_CAP_PORTSPEED_FULL_10)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_10MB_FD;
    }
    if (phyCapability & L7_PHY_CAP_PORTSPEED_HALF_100)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_100MB_HD;
    }
    if (phyCapability & L7_PHY_CAP_PORTSPEED_FULL_100)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_100MB_FD;
    }
    if (phyCapability & L7_PHY_CAP_PORTSPEED_FULL_1000)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_1000MB_FD;
    }
    /* PTin added: Speed 2.5G */
    if (phyCapability & L7_PHY_CAP_PORTSPEED_FULL_2500)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_2500MB_FD;
    }
    /* PTin end */
    if (phyCapability & L7_PHY_CAP_PORTSPEED_FULL_10G)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_10GB_FD;
    }
    /* PTin added: Speed 40G */
    if (phyCapability & L7_PHY_CAP_PORTSPEED_FULL_40G)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_40GB_FD;
    }
    /* PTin added: Speed 100G */
    if (phyCapability & L7_PHY_CAP_PORTSPEED_FULL_100G)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_100GB_FD;
    }
    /* PTin end */
  }
  else 
  {
    if (negoCapabilities & L7_PORT_NEGO_CAPABILITY_HALF_10)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_10MB_HD;
    }
    if (negoCapabilities & L7_PORT_NEGO_CAPABILITY_FULL_10)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_10MB_FD;
    }
    if (negoCapabilities & L7_PORT_NEGO_CAPABILITY_HALF_100)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_100MB_HD;
    }
    if (negoCapabilities & L7_PORT_NEGO_CAPABILITY_FULL_100)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_100MB_FD;
    }
    if (negoCapabilities & L7_PORT_NEGO_CAPABILITY_FULL_1000)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_1000MB_FD;
    }
    /* PTin added: Speed 2.5G */
    if (negoCapabilities & L7_PORT_NEGO_CAPABILITY_FULL_2500)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_2500MB_FD;
    }
    /* PTin end */
    if (negoCapabilities & L7_PORT_NEGO_CAPABILITY_FULL_10G)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_10GB_FD;
    }
    /* PTin added: Speed 40G */
    if (negoCapabilities & L7_PORT_NEGO_CAPABILITY_FULL_40G)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_40GB_FD;
    }
    /* PTin added: Speed 100G */
    if (negoCapabilities & L7_PORT_NEGO_CAPABILITY_FULL_100G)
    {
      dapiCmd.cmdData.autoNegotiateConfig.abilityMask |= DAPI_NEG_100GB_FD;
    }
    /* PTin end */
  }

  dapiCmd.cmdData.autoNegotiateConfig.getOrSet = DAPI_CMD_SET;
  dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG, &dapiCmd);
  if (dr == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

#if L7_FEAT_DUAL_PHY_COMBO
/*******************************************************************************
* @purpose  Get auto-negotiation and the negotiation capabilities
*           of the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    negoCapabilities
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************************/
L7_RC_t dtlIntfNegotiationCapabilitiesGet(L7_uint32 intIfNum, L7_uint32 *negoCapabilities)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_uint32 abilityMask;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.autoNegotiateConfig.abilityMask = 0;

  dapiCmd.cmdData.autoNegotiateConfig.getOrSet = DAPI_CMD_GET;
  if(dapiCtl(&ddUsp, DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG, &dapiCmd) == L7_FAILURE)
  {
    return L7_FAILURE;
  }

  abilityMask = dapiCmd.cmdData.autoNegotiateConfig.abilityMask;

  if (abilityMask == 0)
  {
    *negoCapabilities = 0;
  }
  else
  {
    if (abilityMask & DAPI_NEG_10MB_HD)
    {
      *negoCapabilities |= L7_PORT_NEGO_CAPABILITY_HALF_10;
    }
    if (abilityMask & DAPI_NEG_10MB_FD)
    {
      *negoCapabilities |= L7_PORT_NEGO_CAPABILITY_FULL_10;
    }
    if (abilityMask & DAPI_NEG_100MB_HD)
    {
      *negoCapabilities |= L7_PORT_NEGO_CAPABILITY_HALF_100;
    }
    if (abilityMask & DAPI_NEG_100MB_FD)
    {
      *negoCapabilities |= L7_PORT_NEGO_CAPABILITY_FULL_100;
    }
    if (abilityMask & DAPI_NEG_1000MB_FD)
    {
      *negoCapabilities |= L7_PORT_NEGO_CAPABILITY_FULL_1000;
    }
    if (abilityMask & DAPI_NEG_10GB_FD)
    {
      *negoCapabilities |= L7_PORT_NEGO_CAPABILITY_FULL_10G;
    }
  }

  return L7_SUCCESS;
}
#endif

/*********************************************************************
* @purpose  Test the fiber cable and retrieve test results.
*
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    *cableType       @b((output)) copper or fiber.
* @param    *temperature     @b{(output)} degrees C
* @param    *voltage         @b{(output)} milliVolts
* @param    *current         @b{(output)} milliAmps
* @param    *powerOut        @b{(output)} microWatts
* @param    *powerIn         @b{(output)} microWatts
* @param    *txFault         @b((output)) true or false
* @param    *LOS             @b((output)) true or false
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The function may take a couple of seconds to execute.
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIntfCableFiberDiagGet( L7_uint32              intIfNum,
                                          L7_PORT_CABLE_MEDIUM_t *cableType,
                                          L7_int32               *temperature,
                                          L7_uint32              *voltage,
                                          L7_uint32              *current,
                                          L7_uint32              *powerOut,
                                          L7_uint32              *powerIn,
                                          L7_BOOL                *txFault,
                                          L7_BOOL                *los )
{
  DAPI_USP_t           ddUsp;
  nimUSP_t             usp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t              rc = L7_FAILURE;
  L7_RC_t              dapi_rc;

  if ( L7_SUCCESS == nimGetUnitSlotPort( intIfNum, &usp ) )
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.cableFiberDiag.getOrSet = DAPI_CMD_GET;

    dapi_rc = dapiCtl( &ddUsp, DAPI_CMD_INTF_FIBER_DIAG_GET, &dapiCmd );
    if ( L7_FAILURE == dapi_rc )
    {
      return( rc );
    }
    rc = L7_SUCCESS;

    *temperature = dapiCmd.cmdData.cableFiberDiag.temperature;
    *voltage     = dapiCmd.cmdData.cableFiberDiag.voltage;
    *current     = dapiCmd.cmdData.cableFiberDiag.current;
    *powerOut    = dapiCmd.cmdData.cableFiberDiag.powerOut;
    *powerIn     = dapiCmd.cmdData.cableFiberDiag.powerIn;
    *txFault     = (L7_BOOL)dapiCmd.cmdData.cableFiberDiag.txFault;
    *los         = (L7_BOOL)dapiCmd.cmdData.cableFiberDiag.los;
  }

  return( rc );
}

/*********************************************************************
* @purpose  Enables /disables blinking the ports.
*
* @param    enable       @b{(input)} L7_TRUE - for enabling
*                                    L7_FALSE - for disabling      
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This API to blink all port when an error 
*           situation arises.
*
* @end
*********************************************************************/
L7_RC_t dtlLedBlinkSet(L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t  dapiCmd;
  DAPI_USP_t         ddUsp;
  L7_RC_t            dr;

  dapiCmd.cmdData.ledConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.ledConfig.blink = enable;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_BLINK_SET, &dapiCmd);

  if (dr == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Enables /disables ISDP the ports.
*
* @param    enable       @b{(input)} L7_TRUE - for enabling
*                                    L7_FALSE - for disabling      
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlIsdpPortStatusSet(L7_uint32 intIfNum, L7_BOOL enable)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t cmd;

  L7_RC_t rc = L7_FAILURE;
  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    memset( &(cmd.cmdData.isdpStatus), 0, sizeof(cmd.cmdData.isdpStatus));

    cmd.cmdData.isdpStatus.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.isdpStatus.enable = enable;
    rc = dapiCtl(&ddUsp, DAPI_CMD_ISDP_INTF_STATUS_SET, &cmd);
  }
  return rc;
}

#endif /*DTLCTRL_COMPONENT_INTF*/
