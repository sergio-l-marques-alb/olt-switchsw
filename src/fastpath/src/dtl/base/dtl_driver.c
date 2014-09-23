/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_driver.c
*
* Purpose: This file contains the functions to initialize the drivers
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

#define DTLCTRL_DRIVER_GLOBALS              /* Enable global space   */
#include "dtlinclude.h"

/* PTin added: logger */
#include "logger.h"

#if DTLCTRL_COMPONENT_DRIVER             /* Classifier support        */

L7_BOOL dtlNetInitDone;

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
* @purpose  Initialize the Driver
*
* @param    cpuBoardID   @b{(input)} CPU board ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlDriverInit(L7_ulong32 cpuBoardID)
{


  L7_ulong32 stackUnitID;
  L7_RC_t rc = L7_FAILURE;

  stackUnitID = simGetThisUnit();

  dtlNetInitDone = L7_FALSE;

  if (dapiInit(cpuBoardID) == L7_SUCCESS)
  {
    rc = dtlCallbackRegistration(DTL_FAMILY_INTF_MGMT,dtlLinkChangeCallback);
    if (rc==L7_FAILURE)
    {
      return rc;
    }

    rc = dtlCallbackRegistration(DTL_FAMILY_FRAME,dtlPduReceiveCallback);
    if (rc==L7_FAILURE)
    {
      return rc;
    }

    rc = dtlCallbackRegistration(DTL_FAMILY_QVLAN_MGMT,dtlGenericCallback);
    if (rc == L7_FAILURE)
    {
      return rc;
    }

    rc = dtlCallbackRegistration(DTL_FAMILY_SERVICES, dtlGenericCallback);
    if (rc == L7_FAILURE)
    {
      return rc;
    }

    return L7_SUCCESS;
  }
  else
    return rc;
}

/*********************************************************************
* @purpose  Initializes the card
*
* @param    unitNum     @b{(input)} Unit number
* @param    slotNum     @b{(input)} Slot number
* @param    *pSR        @b{(input)} Pointer to the slot registry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlCardCmd(L7_ushort16    unitNum,
                   L7_ushort16    slotNum,
                   DTL_CARD_CMD_t cardCmd,
                   L7_ulong32     cardTypeID)
{
  DAPI_CARD_MGMT_CMD_t dapiCardMgmt;
  DAPI_USP_t           usp;

  usp.unit = unitNum;
  usp.slot = slotNum;
  usp.port = 0;

  switch (cardCmd)
  {
  case DTL_CARD_INSERT:

    dapiCardMgmt.cardCmd = CARD_CMD_INSERT;
    dapiCardMgmt.cmdData.cardInsert.cardId = cardTypeID;

    LOG_TRACE(LOG_CTX_STARTUP,"CARD_CMD_INSERT event");
    if (dapiCtl(&usp,DAPI_CMD_CARD_INSERT,&dapiCardMgmt) == L7_FAILURE)
      return L7_FAILURE;
    break;

  case DTL_CARD_REMOVE:
    dapiCardMgmt.cardCmd = CARD_CMD_REMOVE;
    LOG_TRACE(LOG_CTX_STARTUP,"CARD_CMD_REMOVE event");
    if (dapiCtl(&usp,DAPI_CMD_CARD_REMOVE,&dapiCardMgmt) == L7_FAILURE)
      return L7_FAILURE;
    break;

  case DTL_CARD_PLUGIN_START:
    dapiCardMgmt.cardCmd = CARD_CMD_PLUGIN_START;
    LOG_TRACE(LOG_CTX_STARTUP,"CARD_CMD_PLUGIN_START event");
    if (dapiCtl(&usp,DAPI_CMD_CARD_PLUGIN,&dapiCardMgmt) == L7_FAILURE)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
              "cardCmd %d failed for unit/slot %d/%d\n", cardCmd, unitNum, slotNum);
    break;

  case DTL_CARD_PLUGIN_FINISH:
    dapiCardMgmt.cardCmd = CARD_CMD_PLUGIN_FINISH;
    LOG_TRACE(LOG_CTX_STARTUP,"CARD_CMD_PLUGIN_FINISH event");
    if (dapiCtl(&usp,DAPI_CMD_CARD_PLUGIN,&dapiCardMgmt) == L7_FAILURE)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
              "cardCmd %d failed for unit/slot %d/%d\n", cardCmd, unitNum, slotNum);
    break;

  case DTL_CARD_UNPLUG_START:
    dapiCardMgmt.cardCmd = CARD_CMD_UNPLUG_START;
    LOG_TRACE(LOG_CTX_STARTUP,"CARD_CMD_UNPLUG_START event");
    if (dapiCtl(&usp,DAPI_CMD_CARD_UNPLUG,&dapiCardMgmt) == L7_FAILURE)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
              "cardCmd %d failed for unit/slot %d/%d\n", cardCmd, unitNum, slotNum);
    break;

  case DTL_CARD_UNPLUG_FINISH:
    dapiCardMgmt.cardCmd = CARD_CMD_UNPLUG_FINISH;
    LOG_TRACE(LOG_CTX_STARTUP,"CARD_CMD_UNPLUG_FINISH event");
    if (dapiCtl(&usp,DAPI_CMD_CARD_UNPLUG,&dapiCardMgmt) == L7_FAILURE)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
              "cardCmd %d failed for unit/slot %d/%d\n", cardCmd, unitNum, slotNum);
    break;

  default:
    LOG_ERR(LOG_CTX_STARTUP,"Unknown event");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Start the driver based on the startupReason code
*
*
* @params  systemStartupreason {(input)} Type of startup
* @params  mgrFailover         {(input)} Indicates that the unit is
*                                        becoming mgr after a failover
*          lastMgrUnitId       {(input)} Unit number of the last mgr
*          lastMgrKey          {(input)} Key of the last mgr
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Driver translates various reason codes into either:
*            - warm start
*            - cold start
*
*            If warm start was requested but could not be performed
*            then cold start is initiated.
*
* @end
*********************************************************************/
L7_RC_t dtlDriverStart(L7_LAST_STARTUP_REASON_t systemStartupReason,
                       L7_BOOL                  mgrFailover,
                       L7_uint32                lastMgrUnitId,
                       L7_enetMacAddr_t         lastMgrKey)
{
  DAPI_CONTROL_GENERIC_CMD_t controlCmd;
  DAPI_USP_t                 usp;
  L7_RC_t                    rc;

  /* Dummy parameter */
  usp.unit = 0;
  usp.slot = 0;
  usp.port = 0;

  controlCmd.cmdData.startup.startupReason = systemStartupReason;
  controlCmd.cmdData.startup.mgrFailover = mgrFailover;
  controlCmd.cmdData.startup.lastMgrUnitId = lastMgrUnitId;
  memcpy(&(controlCmd.cmdData.startup.lastMgrKey), &lastMgrKey,
         sizeof(lastMgrKey));

  rc = dapiCtl(&usp, DAPI_CMD_CONTROL_DRIVER_START, (void *)&controlCmd);

  return rc;
}

/*********************************************************************
* @purpose  Notify the driver that Hw updates are complete for a phase
*
* @param    phase {(input)} Phase for which Hw Update is complete
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
* @end
*********************************************************************/

L7_RC_t dtlDriverHwApplyNotify(L7_CNFGR_HW_APPLY_t phase)
{
  DAPI_CONTROL_GENERIC_CMD_t controlCmd;
  DAPI_USP_t                 usp;
  L7_RC_t                    rc;

  /* Dummy parameter */
  usp.unit = 0;
  usp.slot = 0;
  usp.port = 0;

  controlCmd.cmdData.hwApplyNotification.phase = phase;

  rc = dapiCtl(&usp, DAPI_CMD_CONTROL_HW_APPLY_NOTIFY, (void *)&controlCmd);

  return rc;
}

/*********************************************************************
* @purpose  Notify the driver that a unit has been disconnected from
*           the stack
*
* @param    unit {(input)} unit number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
* @end
*********************************************************************/

L7_RC_t dtlDriverUnitDisconnectNotify(L7_uint32 unit)
{
  DAPI_CONTROL_GENERIC_CMD_t controlCmd;
  DAPI_USP_t                 usp;
  L7_RC_t                    rc;

  /* Dummy parameter */
  usp.unit = 0;
  usp.slot = 0;
  usp.port = 0;

  controlCmd.cmdData.unitStatus.unit = unit;
  controlCmd.cmdData.unitStatus.flag = L7_FALSE; /* Unit is disconnected */

  rc = dapiCtl(&usp, DAPI_CMD_CONTROL_UNIT_STATUS_NOTIFY, (void *)&controlCmd);

  return rc;
}


/*********************************************************************
* @purpose  Execute commands on a driver shell
*
* @param    str    string needed to be executed on the debug shell
*
* @returns
*
* @notes
*
* @end
*********************************************************************/
void dtlDriverShell(const L7_uchar8 str[])
{
  DAPI_USP_t           usp;

  usp.unit = -1;
  usp.slot = -1;
  usp.port = -1;

  dapiCtl(&usp,DAPI_CMD_DEBUG_SHELL,(void *)str);

  return;

}

#endif /* DTLCTRL_COMPONENT_DRIVER */

