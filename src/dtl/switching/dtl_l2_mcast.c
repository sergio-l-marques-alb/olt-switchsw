/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dtl_l2_mcast.c
*
* @purpose   This file contains the functions to transform layer 2
*            multicast components' requests into driver requests.
*
* @component Device Transformation Layer (DTL)
*
* @comments  none
*
* @create    05/11/2002
*
* @author    mfiorito
*
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#define DTLCTRL_L2_MCAST_GLOBALS              /* Enable global space */
#include "dtlinclude.h"

#if DTLCTRL_COMPONENT_L2_MCAST

/*********************************************************************
* @purpose  Enables IGMP/MLD Snooping
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlSnoopingEnable(L7_uchar8 family, L7_uint16 vlanId /* PTin added: IGMP snooping */) 
{
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  /* ddUsp must be a valid usp so set it to zeros */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.snoopConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.snoopConfig.enable = L7_TRUE;
  dapiCmd.cmdData.snoopConfig.family = family;
  /* PTin added: IGMP snooping */
  #if 1
  dapiCmd.cmdData.snoopConfig.CoS = L7_NULL;
  dapiCmd.cmdData.snoopConfig.vlanId = vlanId;
  #endif

  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_SNOOP_CONFIG, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Disables IGMP/MLD Snooping
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlSnoopingDisable(L7_uchar8 family, L7_uint16 vlanId /* PTin added: IGMP snooping */)
{
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  /* ddUsp must be a valid usp so set it to zeros */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.snoopConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.snoopConfig.enable = L7_FALSE;
  dapiCmd.cmdData.snoopConfig.family = family;
  /* PTin added: IGMP snooping */
  #if 1
  dapiCmd.cmdData.snoopConfig.CoS = L7_NULL;
  dapiCmd.cmdData.snoopConfig.vlanId = vlanId;
  #endif

  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_SNOOP_CONFIG, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}
/*********************************************************************
* @purpose  Notifies L3 about a change in the outport list
*
* @param    L7_uchar8       Multicast Mac Address of received group
* @param    L7_BOOL         L7_TRUE -> Upstream Routing interface intIfNum
*                           L7_FALSE  -> Downstream Routing interface intIfNum
* @param    L7_uint32       Upstream/Downstream Routing interface intIfNum
* @param    L7_INTF_MASK_t  Interface mask of member ports of the
*                           group
* @param    L7_BOOL         snoopVlanOperState -> L7_TRUE if snooping
*                           is operational on atleast one interface of the
*                           vlan rtrportNum.
*                           L7_FALSE otherwise.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlSnoopingL3Notify(L7_uchar8 *mcastMacAddr, 
                            L7_BOOL srcIntfFlag,
                            L7_uint32 outRtrPortNum, 
                            L7_INTF_MASK_t *outPortListMask,
                            L7_BOOL snoopVlanOperState)
{
  DAPI_USP_t                   ddUsp;
  nimUSP_t                     usp;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  L7_uint32                    outPortList[L7_MAX_INTERFACE_COUNT + 1];
  DAPI_USP_t                   outPortUspList[L7_MAX_INTERFACE_COUNT + 1];
  L7_RC_t                      dr;
  L7_uint32                    count, uintCtr;

  if (nimGetUnitSlotPort(outRtrPortNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddUsp.port = usp.port - 1; /* In or Out RTR Port */
  ddUsp.slot = usp.slot;
  ddUsp.unit = usp.unit;

   /* if there are any outgoing interfaces, create array of USPs, one for eact out interface */
   nimMaskToList( (NIM_INTF_MASK_t *) (outPortListMask), outPortList, &count);
   dapiCmd.cmdData.mcastModify.outUspCount = 0;
   for (uintCtr = 0; uintCtr < count; uintCtr++)
   {
      if (nimGetUnitSlotPort(outPortList[uintCtr], &usp) == L7_SUCCESS)
      {
        outPortUspList[uintCtr].unit = usp.unit;
        outPortUspList[uintCtr].slot = usp.slot;
        outPortUspList[uintCtr].port = usp.port - 1;
        dapiCmd.cmdData.mcastModify.outUspCount++;
      }
   }

   /* port List */
   dapiCmd.cmdData.mcastModify.outUspList = outPortUspList;
   dapiCmd.cmdData.mcastModify.getOrSet = DAPI_CMD_SET;

   memcpy(dapiCmd.cmdData.mcastModify.mcastMacAddr, mcastMacAddr, L7_MAC_ADDR_LEN);
   if (srcIntfFlag == L7_TRUE)
   {
     dapiCmd.cmdData.mcastModify.outIntfDirection = DAPI_SNOOP_L3_NOTIFY_INGRESS;
     dapiCmd.cmdData.mcastModify.snoopVlanOperState = snoopVlanOperState;
   }
   else
   {
     dapiCmd.cmdData.mcastModify.outIntfDirection = DAPI_SNOOP_L3_NOTIFY_EGRESS;
   }

   dr = dapiCtl(&ddUsp, DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY, &dapiCmd);
   if (dr == L7_SUCCESS)
   {
     return L7_SUCCESS;
   }
   else
   {
     return L7_FAILURE;
   }
}
#endif /*DTLCTRL_COMPONENT_L2_MCAST*/
