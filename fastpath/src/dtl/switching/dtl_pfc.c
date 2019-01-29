/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2009
*
**********************************************************************
*
* @filename dtl_pfc.c
*
* @purpose Priority Flow Control definitions
*
* @component pfc
*
* @comments none
*
* @create 06/29/2009
*
* @author 
* @end
*
**********************************************************************/
#include "nimapi.h"
#include "datatypes.h"
#include "dapi.h"
#include "dtl_pfc.h"

/*********************************************************************
* @purpose  Set the PFC participation mode for an interface
*
* @param  intIfNum @b((input)) The internal interface
* @param  enable   @b((input)) The particpation mode 
* @param  nodrop-pri_bmp @b{(input)}  priority bitmap to set a nodrop
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
* @returns  L7_TABLE_IS_FULL  - resource issue
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPfcIntfConfig(L7_uint32 intIfNum, 
                         L7_BOOL enable, 
                         L7_uchar8 nodrop_pri_bmp)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t  cmd;
  L7_RC_t  rc;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.pfcConfig.enable = enable;
    cmd.cmdData.pfcConfig.priority_bmp = nodrop_pri_bmp;

    rc = dapiCtl(&ddUsp,DAPI_CMD_INTF_PFC_CONFIG,&cmd);
  }

  return rc;
}

static DAPI_PFC_STATS_t priority_map[] = {
  DAPI_STATS_PFC_COS0_RX,
  DAPI_STATS_PFC_COS1_RX,
  DAPI_STATS_PFC_COS2_RX,
  DAPI_STATS_PFC_COS3_RX,
  DAPI_STATS_PFC_COS4_RX,
  DAPI_STATS_PFC_COS5_RX,
  DAPI_STATS_PFC_COS6_RX,
  DAPI_STATS_PFC_COS7_RX,
};

/*********************************************************************
* @purpose  Get the count of pfc receives for an interface/priority
*
* @param  intIfNum  @b((input)) The internal interface
* @param  priority  @b((input)) The particpation mode 
* @param  stat      @b{(output)}  stat
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPfcRxPriorityStatGet(L7_uint32 intIfNum, 
                                L7_uint32 priority, 
                                L7_uint32 *stat)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t  cmd;
  L7_RC_t  rc;

  if (priority >= sizeof(priority_map)/sizeof(DAPI_PFC_STATS_t)) 
  {
    return L7_FAILURE;
  }

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.pfcStatGet.statType = priority_map[priority];
    cmd.cmdData.pfcStatGet.statCounter = 0;

    rc = dapiCtl(&ddUsp,DAPI_CMD_INTF_PFC_STATS_GET,&cmd);
  }

  if (rc == L7_SUCCESS) 
  {
    *stat = cmd.cmdData.pfcStatGet.statCounter;
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the count of pfc receives for an interface
*
* @param  intIfNum  @b((input)) The internal interface
* @param  stat      @b{(output)} stat
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPfcRxStatGet(L7_uint32 intIfNum, L7_uint32 *stat)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t  cmd;
  L7_RC_t  rc;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.pfcStatGet.statType = DAPI_STATS_PFC_RX;
    cmd.cmdData.pfcStatGet.statCounter = 0;

    rc = dapiCtl(&ddUsp,DAPI_CMD_INTF_PFC_STATS_GET,&cmd);
  }

  if (rc == L7_SUCCESS) 
  {
    *stat = cmd.cmdData.pfcStatGet.statCounter;
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the count of pfc transmits for an interface
*
* @param  intIfNum  @b((input)) The internal interface
* @param  stat      @b{(output)} stat
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPfcTxStatGet(L7_uint32 intIfNum, L7_uint32 *stat)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t  cmd;
  L7_RC_t  rc;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.pfcStatGet.statType = DAPI_STATS_PFC_TX;
    cmd.cmdData.pfcStatGet.statCounter = 0;

    rc = dapiCtl(&ddUsp,DAPI_CMD_INTF_PFC_STATS_GET,&cmd);
  }

  if (rc == L7_SUCCESS) 
  {
    *stat = cmd.cmdData.pfcStatGet.statCounter;
  }

  return rc;
}

/*********************************************************************
* @purpose Clear all PFC stats on an interface 
*
* @param  intIfNum  @b((input))  The internal interface
*
* @returns  L7_ERROR          - internal failure
* @returns  L7_NOT_SUPPORTED  - interface not supported
* @returns  L7_SUCCESS        - return is valid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPfcStatsClear(L7_uint32 intIfNum)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t  cmd;
  L7_RC_t  rc;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    rc = dapiCtl(&ddUsp,DAPI_CMD_INTF_PFC_STATS_CLEAR,&cmd);
  }

  return rc;
}
