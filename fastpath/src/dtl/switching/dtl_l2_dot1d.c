/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename:  dtl_l2_dot1d.c
*
* @purpose:   Contains the port related functions
*
* @component: Device Transformation Layer
*
* @comments:  none
*
* @author:    Shekhar Kalyanam 3/14/2001
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_L2_DOT1D_GLOBALS              /* Enable global space   */    
#include "dtlinclude.h"

#if DTLCTRL_COMPONENT_L2_DOT1D


static L7_uint32 portList[L7_MAX_INTERFACE_COUNT+1];
static DAPI_USP_t portUsp[L7_MAX_INTERFACE_COUNT+1];
static DAPI_GROUP_REG_STATE_t portState[L7_MAX_INTERFACE_COUNT+1];
extern void *dtlL2McastSema;

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
* @purpose  Enables GMRP for the interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1dGMRPEnable(L7_uint32 intIfNum)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_GARP_MGMT_CMD_t dapiCmd;
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

    dapiCmd.cmdData.portGmrp.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.portGmrp.enable = L7_TRUE;


    dr = dapiCtl(&ddUsp,DAPI_CMD_GARP_GMRP,&dapiCmd);
    if (dr == L7_SUCCESS)
      return L7_SUCCESS;
    else
      return L7_FAILURE;

  }


}

/*********************************************************************
* @purpose  Disables GMRP for the interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*           
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1dGMRPDisable(L7_uint32 intIfNum)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_GARP_MGMT_CMD_t dapiCmd;
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

    dapiCmd.cmdData.portGmrp.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.portGmrp.enable = L7_FALSE;


    dr = dapiCtl(&ddUsp,DAPI_CMD_GARP_GMRP,&dapiCmd);
    if (dr == L7_SUCCESS)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }


}
/*********************************************************************
* @purpose  Adds a Group Registration Entry in the Multicast Table
*
* @param	*grpEntry      		@b{(input)} Pointer to Group Entry  
*           
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    If the entry does not exist, a new entry is created.
*           if an entry exists the entry is replaced with this one.
*			This API assumes that the deafult behaviour is normal registration (for static) 
*           i.e. all the ports not in the fwd or filter mask will be assumed to be 
*           set to 'use dynamic information' Refer IEEE 802.1D section 7.9 for 
*           futher details. 
*			and filter for dynamic registration
*			i.e. for a dynamic entry type all ports not in forward mask will be
*           set to filter and the filter mask will be ig
*
*
* @end
*********************************************************************/
L7_RC_t dtlL2McastEntryAdd(DTL_L2_MCAST_ENTRY_t *grpEntry)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_GARP_MGMT_CMD_t dapiCmd;
  L7_RC_t dr,rc;
  L7_uint32 totalPorts=0, i, j, fwdPorts=0, filterPorts=0;
  
  rc = osapiSemaTake(dtlL2McastSema, L7_WAIT_FOREVER);

  /* NOTE: Due defect 34264 This section of code was modified. 
   * The design philosophy now is to let mfdb tell dtl/driver about 
   * all dst interfaces related to multicast addresses irrespective whether it is 
   * static (filter) or dynamic (IGMP Snoop or GMRP) in origination
   * The filter component will continue to tell dtl/driver about
   * static multicast src interfaces, unicast src and unicast dst interfaces
   */
  /*
  if (grpEntry->type == DTL_L2_MCAST_STATIC)
    return L7_SUCCESS;
   */
  /*ddUsp must be a valid usp set it to zeros*/
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  if(L7_SUCCESS != nimMaskToList(grpEntry->fwdMask,portList,&fwdPorts))
  {
	rc = osapiSemaGive(dtlL2McastSema);
    return L7_FAILURE;
  }
    
  for(i=0;i<fwdPorts;i++)
  {
    if(nimGetUnitSlotPort(portList[i], &usp) != L7_SUCCESS)
	{
	  rc = osapiSemaGive(dtlL2McastSema);
      return L7_FAILURE;
	}
      
    portUsp[i].unit = usp.unit;
    portUsp[i].slot = usp.slot;
    portUsp[i].port = usp.port - 1;
    portState[i] = DAPI_GROUP_REG_STATE_FORWARD;
  }

  j = fwdPorts;
  if(grpEntry->type == DTL_L2_MCAST_STATIC)
  {
    if(L7_SUCCESS != nimMaskToList(grpEntry->filterMask,portList,&filterPorts))
	{
	  rc = osapiSemaGive(dtlL2McastSema);
      return L7_FAILURE;
	}
      
    for(i=0;i<filterPorts;i++)
    {
      if(nimGetUnitSlotPort(portList[i], &usp) != L7_SUCCESS)
	  {
		rc = osapiSemaGive(dtlL2McastSema);
        return L7_FAILURE;
	  }
        
      portUsp[j].unit = usp.unit;
      portUsp[j].slot = usp.slot;
      portUsp[j].port = usp.port - 1;
      portState[j] = DAPI_GROUP_REG_STATE_FILTER;
      j++;
    }
  }
  totalPorts = fwdPorts + filterPorts;

  dapiCmd.cmdData.groupRegModify.getOrSet = DAPI_CMD_SET;
  
  if(grpEntry->type == DTL_L2_MCAST_STATIC)
    dapiCmd.cmdData.groupRegModify.isStatic = L7_TRUE;
  else
    dapiCmd.cmdData.groupRegModify.isStatic = L7_FALSE;

  dapiCmd.cmdData.groupRegModify.vlanId = grpEntry->vlanId;
  memcpy((void *)dapiCmd.cmdData.groupRegModify.grpMacAddr, 
		 (void *)grpEntry->macAddr, 
		 L7_MAC_ADDR_LEN);
  
  dapiCmd.cmdData.groupRegModify.numOfPorts = totalPorts;
  dapiCmd.cmdData.groupRegModify.ports = portUsp;
  dapiCmd.cmdData.groupRegModify.stateInfo = portState;
  
  dr = dapiCtl(&ddUsp,DAPI_CMD_GARP_GROUP_REG_MODIFY,&dapiCmd);

  rc = osapiSemaGive(dtlL2McastSema);

  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Deletes a Group Registration Entry from the Multicast table
*
* @param	vlanId      		@b{(input)} VLAN ID for this entry 
* @param	macAddr     		@b{(input)} Pointer to the MAC Addr of this entry
* @param	type				@b{(input)} type of entry
*           
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlL2McastEntryDelete(L7_uint32 vlanId, 
					                  L7_uchar8 *macAddr,
									  DTL_L2_MCAST_TYPE_t type) 
{
  DAPI_USP_t ddUsp;
  DAPI_GARP_MGMT_CMD_t dapiCmd;
  L7_RC_t dr;

  /* NOTE: Due defect 34264 This section of code was modified. 
   * The design philosophy now is to let mfdb tell dtl/driver about 
   * all dst interfaces related to multicast addresses irrespective whether it is 
   * static (filter) or dynamic (IGMP Snoop or GMRP) in origination
   * The filter component will continue to tell dtl/driver about
   * static multicast src interfaces, unicast src and unicast dst interfaces
   */
  /*
  if (type == DTL_L2_MCAST_STATIC)
    return L7_SUCCESS;
  */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.groupRegDelete.getOrSet = DAPI_CMD_SET;
  if(type == DTL_L2_MCAST_STATIC)
	dapiCmd.cmdData.groupRegDelete.isStatic = L7_TRUE;
  else
	dapiCmd.cmdData.groupRegDelete.isStatic = L7_FALSE;

  dapiCmd.cmdData.groupRegDelete.vlanId = vlanId;
  memcpy((void *)dapiCmd.cmdData.groupRegDelete.grpMacAddr, (void *)macAddr, L7_MAC_ADDR_LEN);

  dr = dapiCtl(&ddUsp, DAPI_CMD_GARP_GROUP_REG_DELETE, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;


}


#if 0  /*just for testing*/
void dtlGmrpTest()
{
  L7_uint32 portListFwd[L7_MAX_INTERFACE_COUNT];
  L7_uint32 portListFilter[L7_MAX_INTERFACE_COUNT];
  L7_uint32 numFwd;
  L7_uint32 numFilter,i;
  L7_uint32 vlanId = 1;
  L7_uchar8 mac[6];

  mac[0] = 0x01;
  mac[1] = 0x02;
  mac[2] = 0x03;
  mac[3] = 0x04;
  mac[4] = 0x05;
  mac[5] = 0x06;

  for (i=0;i<L7_MAX_INTERFACE_COUNT;i++)
  {
	portListFwd[i]=0;
	portListFilter[i]=0;
  }

  portListFwd[1] = 1;
  portListFwd[2] = 2;
  numFwd = 2;

  portListFilter[0] = 2;
  portListFilter[1] = 3;
  portListFilter[2] = 4;
  numFilter = 3;

  dtlDot1dGMRPStaticGroupAdd(vlanId,mac,portListFwd,numFwd,portListFilter,numFilter);

  dtlDot1dGMRPGroupAdd(vlanId,mac,portListFwd,numFwd);

  dtlDot1dGMRPStaticGroupDelete(vlanId,mac);

  dtlDot1dGMRPGroupDelete(vlanId,mac);

  dtlDot1dGMRPFwdAllAdd(vlanId,portListFwd,numFwd);

  dtlDot1dGMRPFwdAllDelete(vlanId);

  dtlDot1dGMRPFwdUnregAdd(vlanId,portListFwd,numFwd);

  dtlDot1dGMRPFwdUnregDelete(vlanId);

  dtlDot1dGMRPStaticFwdAllAdd(vlanId,portListFwd,numFwd,portListFilter,numFilter);

  dtlDot1dGMRPStaticFwdAllDelete(vlanId);

  dtlDot1dGMRPStaticFwdUnregAdd(vlanId,portListFwd,numFwd,portListFilter,numFilter);

  dtlDot1dGMRPStaticFwdUnregDelete(vlanId);


}

#endif /*just for testing*/

/*********************************************************************
* @purpose  Sets the mcast flood mode for a VLAN
*
* @param	vlanId      		@b{(input)} VLAN ID for this entry 
* @param	mode                @b{(input)} flood mode
*           
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlL2McastVlanFloodModeSet(L7_uint32 vlanId, L7_FILTER_VLAN_FILTER_MODE_t mode)
{
  DAPI_USP_t ddUsp;
  DAPI_QVLAN_MGMT_CMD_t dapiCmd;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.mcastFloodModeSet.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.mcastFloodModeSet.mode = mode;
  dapiCmd.cmdData.mcastFloodModeSet.vlanId = vlanId;

  dr = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_MCAST_FLOOD_SET, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;


}



#endif /*DTLCTRL_COMPONENT_L2_DOT1D*/

