/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename dtl_l2_dot1s.c
*
* @purpose DTL interface
*
* @component DTL
*
* @comments none
*
* @create 10/14/2002
*
* @author skalyanam
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "log.h"
#include "nimapi.h"
#include "dot1s_api.h"
#include "dtl_l2_dot1s.h"
#include "dtlinclude.h"

L7_uint32 dtlDot1sDebugFlag = 0;
/*********************************************************************
* @purpose  Toggles debug flag 
*				
* @param    none  
*
* @returns  none
*
* @comments With debug flag set to 1 certain debug messages will be outputted
*
* @notes 
*
* @end
*********************************************************************/
void dtlDot1sFlag()
{
  if (dtlDot1sDebugFlag == 0)
  {
	dtlDot1sDebugFlag = 1;
  }
  else
  {
	dtlDot1sDebugFlag = 0;
  }
}
/*********************************************************************
* @purpose  Sets the MSTP state for the port in a particular instance 
*			of spanning tree	
*
* @param    instNumber @b((input)) user assigned instance number  
* @param    intIfNum @b((input)) user assigned interface number  
* @param    state @b((input)) user assigned dot1s forwarding state
*
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sStateSet(L7_uint32 instNumber, L7_uint32 intIfNum, L7_uint32 state, L7_uint32 appRef)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  L7_RC_t dd_rc;
  DAPI_INTF_MGMT_CMD_t cmd;
  L7_RC_t rc = L7_FAILURE;
  /*L7_BOOL brc;*/

  if (dot1sInstCheckInUse(instNumber) != L7_SUCCESS)
  {
    return rc;
  }

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    switch (state)
    {
    case L7_DOT1S_DISABLED:
      cmd.cmdData.dot1sState.state = DAPI_PORT_DOT1S_ADMIN_DISABLED;
      break;

    case L7_DOT1S_LEARNING:
      /*
	  brc = dot1sIsFwdInAnyOtherInst(instNumber, intIfNum);
	  if (brc == L7_TRUE)
	  {
		return L7_SUCCESS;
	  }
	  */
	  cmd.cmdData.dot1sState.state = DAPI_PORT_DOT1S_LEARNING;
      break;

    case L7_DOT1S_FORWARDING:
      cmd.cmdData.dot1sState.state = DAPI_PORT_DOT1S_FORWARDING;
      break;

    case L7_DOT1S_DISCARDING:
      /*
	  brc = dot1sIsFwdInAnyOtherInst(instNumber, intIfNum);
	  if (brc == L7_TRUE)
	  {
		return L7_SUCCESS;
	  }
	  */
	  cmd.cmdData.dot1sState.state = DAPI_PORT_DOT1S_DISCARDING;
      break;

    case L7_DOT1S_MANUAL_FWD:
      cmd.cmdData.dot1sState.state = DAPI_PORT_DOT1S_NOT_PARTICIPATING;
      break;

    default:
      LOG_MSG("received invalid MSTP state\n");
      return rc;
    }

    cmd.cmdData.dot1sState.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.dot1sState.instNumber = instNumber;
	cmd.cmdData.dot1sState.applicationReference = appRef;
    dd_rc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DOT1S_STATE, &cmd);
    if (dd_rc == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Creates an instance of spanning tree 
*				
* @param    instNumber	@b((input)) user assigned instance number  
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sInstanceCreate(L7_uint32 instNumber)
{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t dd_rc;
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t cmd;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  cmd.cmdData.dot1sInstanceCreate.getOrSet = DAPI_CMD_SET;
  cmd.cmdData.dot1sInstanceCreate.instNumber = instNumber;

  dd_rc = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE, &cmd);

  if (dd_rc == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Deletes an instance of spanning tree 
*			   
* @param    instNumber	@b((input)) user assigned instance number to be deleted  
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sInstanceDelete(L7_uint32 instNumber)
{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t dd_rc;
  DAPI_SYSTEM_CMD_t cmd;
  DAPI_USP_t ddUsp;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  cmd.cmdData.dot1sInstanceDelete.getOrSet = DAPI_CMD_SET;
  cmd.cmdData.dot1sInstanceDelete.instNumber = instNumber;

  dd_rc = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE, &cmd);

  if (dd_rc == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Add VLAN to an already created instance of spanning tree 
*          
* @param    instNumber	@b((input)) User assigned instance number
* @param	vlanId	    @b{(input)} vlan id
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sInstVlanIdAdd(L7_uint32 instNumber, L7_ushort16 vlanId)

{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t dd_rc;
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t cmd;

  bzero((char *)&cmd, (L7_int32)sizeof(cmd));

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  cmd.cmdData.dot1sInstanceVlanAdd.getOrSet = DAPI_CMD_SET;
  cmd.cmdData.dot1sInstanceVlanAdd.vlanId  = vlanId;
  cmd.cmdData.dot1sInstanceVlanAdd.instNumber = instNumber;

  if (dtlDot1sDebugFlag)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS, "\nFor instance %d adding VLAN %d\n",
				   instNumber, vlanId); 
  }

  dd_rc = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD, &cmd);

  if (dd_rc == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Remove VLAN from an existing instance of spanning tree 
*          
* @param    instNumber	@b((input)) User assigned instance number
* @param	vlanId	    @b{(input)} vlan id
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sInstVlanIdRemove(L7_uint32 instNumber, L7_ushort16 vlanId)

{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t dd_rc;
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t cmd;

  bzero((char *)&cmd, (L7_int32)sizeof(cmd));

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  cmd.cmdData.dot1sInstanceVlanRemove.getOrSet = DAPI_CMD_SET;
  cmd.cmdData.dot1sInstanceVlanRemove.vlanId  = vlanId;
  cmd.cmdData.dot1sInstanceVlanRemove.instNumber = instNumber;

  if (dtlDot1sDebugFlag)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS, "\nFor instance %d removing VLAN %d\n",
				   instNumber, vlanId); 
  }

  dd_rc = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE, &cmd);

  if (dd_rc == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

// PTin added
/*********************************************************************
* @purpose  Change Learn Mode
*			   
* @param    intIfNum	@b((input)) internal interface number
*           learnEnable => Learn mode: L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sLearnModeSet(L7_uint32 intIfNum, L7_BOOL learnEnable)
{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t dd_rc;
  DAPI_USP_t ddUsp;
  DAPI_ADDR_MGMT_CMD_t cmd;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
	
	/*issue the dapi cmd for flushing the entries learnt on this interface*/

	cmd.cmdData.portAddressSetLearnMode.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.portAddressSetLearnMode.learn_enabled = learnEnable;

	dd_rc = dapiCtl(&ddUsp, DAPI_CMD_ADDR_SET_LEARN_MODE, &cmd);

	if (dd_rc == L7_SUCCESS)
	{
	  rc = L7_SUCCESS;
	}
  }
  return rc;
}

/*********************************************************************
* @purpose  Get Learn Mode
*			   
* @param    intIfNum	@b((input)) internal interface number
*           learnEnable => Learn mode: L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sLearnModeGet(L7_uint32 intIfNum, L7_BOOL *learnEnable)
{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t dd_rc;
  DAPI_USP_t ddUsp;
  DAPI_ADDR_MGMT_CMD_t cmd;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
	
	/*issue the dapi cmd for flushing the entries learnt on this interface*/

	cmd.cmdData.portAddressSetLearnMode.getOrSet = DAPI_CMD_GET;
    cmd.cmdData.portAddressSetLearnMode.learn_enabled = 0;

	dd_rc = dapiCtl(&ddUsp, DAPI_CMD_ADDR_SET_LEARN_MODE, &cmd);

	if (dd_rc == L7_SUCCESS)
	{
      // Output
      if (learnEnable!=L7_NULLPTR)
        *learnEnable = cmd.cmdData.portAddressSetLearnMode.learn_enabled;
	  rc = L7_SUCCESS;
	}
  }
  return rc;
}
// PTin end

/*********************************************************************
* @purpose  Flushes all entries in fdb learnt on this interface 
*			   
* @param    intIfNum	@b((input)) internal interface number  
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sFlush(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t dd_rc;
  DAPI_USP_t ddUsp;
  DAPI_ADDR_MGMT_CMD_t cmd;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
	
	/*issue the dapi cmd for flushing the entries learnt on this interface*/

	cmd.cmdData.portAddressFlush.getOrSet = DAPI_CMD_SET;
	dd_rc = dapiCtl(&ddUsp, DAPI_CMD_ADDR_FLUSH, &cmd);

	if (dd_rc == L7_SUCCESS)
	{
	  rc = L7_SUCCESS;
	}
  }
  return rc;
}

/*********************************************************************
* @purpose  Set the BPDU filter for a port
*			   
* @param    bpduFilter	@b((input)) Filter value to be set  
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sBpduFilterSet(L7_uint32 intIfNum, L7_BOOL bpduFilter)
{
  DAPI_USP_t ddUsp;
  L7_RC_t ddRc;
  DAPI_INTF_MGMT_CMD_t  dapiCmd;
  L7_RC_t rc = L7_FAILURE;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.bpduFiltering.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.bpduFiltering.enable = bpduFilter;

    ddRc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DOT1S_BPDU_FILTERING, &dapiCmd);
    if (ddRc == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Set the BPDU flood for a port
*			   
* @param    intIfNum    @b((input)) internal interface number
* @param    bpduFlood	@b((input)) Filter value to be set  
*
* @returns  L7_SUCCESS	on a successful operation 
* @returns  L7_FAILURE	for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sBpduFloodSet(L7_uint32 intIfNum, L7_BOOL bpduFlood)
{
  DAPI_USP_t ddUsp;
  L7_RC_t ddRc;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t rc = L7_FAILURE;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    dapiCmd.cmdData.bpduFlood.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.bpduFlood.enable = bpduFlood;

    ddRc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DOT1S_BPDU_FLOOD, &dapiCmd);
    if (ddRc == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Flushes all learned entries in the FDB
*
* @param    none
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  for any error
*
* @comments
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlDot1sFlushAll(void)
{
  DAPI_SYSTEM_CMD_t cmd;
  DAPI_USP_t ddUsp;
  /* no interface for this command */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  cmd.cmdData.l2FlushAll.getOrSet = DAPI_CMD_SET;
  return dapiCtl(&ddUsp, DAPI_CMD_ADDR_FLUSH_ALL, &cmd);
}

