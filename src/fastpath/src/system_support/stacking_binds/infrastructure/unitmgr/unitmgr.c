/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename unitmgr.c
*
* @purpose Unit manager main implementation
*
* @component unitmgr
*
* @comments none
*
* @create 07/15/2003
*
* @author atsigler
*
* @end
*
**********************************************************************/

#include <string.h>

#include "l7_common.h"
#include "log.h"
#include "unitmgr.h"
#include "unitmgr_api.h"

#include "sysapi_hpc.h"
#include "cardmgr_api.h"

/* Reason why the Management unit software was started last */
static L7_LAST_STARTUP_REASON_t umSystemStartupReason = L7_STARTUP_POWER_ON;

L7_int32 umRunCfgDlTaskId = 0;

/*********************************************************************
* @purpose  Register a routine to be called when a unit state changes.
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
* @param    *notify        pointer to a routine to be invoked for unit state
*                          changes.  Each routine has the following parameters:
*                          (unit number, event).
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitChangeRegister(L7_COMPONENT_IDS_t registrar_ID, unitNotify notify)
{
  return L7_SUCCESS;	/* Need to return Success otherwise non-stacking builds will crash */
}

/*********************************************************************
* @purpose  De-Register a routine to be called when a unit state changes.
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t unitMgrUnitChangeDeRegister(L7_COMPONENT_IDS_t registrar_ID)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  CNFGR System Initialization for Unit Manager
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the Unit Manager
*
* @end
*********************************************************************/
L7_RC_t unitMgrApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_uint32 rc;
  L7_CNFGR_CB_DATA_t cbData;
  L7_CNFGR_ERR_RC_t reason;

  bzero((char *) &cbData, sizeof(cbData));
  rc = L7_SUCCESS;  /* claim victory until proven otherwise */
  reason = 0;

  if (pCmdData == L7_NULLPTR)
  {
    rc = L7_ERROR;
    reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
  }
  else if (pCmdData->type == L7_CNFGR_RQST)  /* we only process configurator requests, anything else it an error */
  {
    switch (pCmdData->command)
    {
      case L7_CNFGR_CMD_INITIALIZE:
        switch (pCmdData->u.rqstData.request)
        {
          case L7_CNFGR_RQST_I_PHASE1_START:
            PT_LOG_INFO(LOG_CTX_STARTUP,"PHASE 1");
            /* nothing to do for this Configurator request */
            break;

          case L7_CNFGR_RQST_I_PHASE2_START:
            PT_LOG_INFO(LOG_CTX_STARTUP,"PHASE 2");
            /* nothing to do for this Configurator request */
            break;

          case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
            { 
              L7_enetMacAddr_t lastMgrKey;

              PT_LOG_INFO(LOG_CTX_STARTUP,"WAIT");

              memset(&lastMgrKey, 0, sizeof(lastMgrKey));

              /* Tell card manager that this unit is no longer the management unit.
              ** This will remove logical cards from the driver so they may be re-plugged.
              */
              cmgrLocalUnitIsNotManager ();
              /* Tell card manager that this unit is about to become the management unit.
               */

              cmgrLocalUnitIsManager (umSystemStartupReason, L7_FALSE,
                                      0, lastMgrKey);

            }
            break;

          case L7_CNFGR_RQST_I_PHASE3_START:
            PT_LOG_INFO(LOG_CTX_STARTUP,"PHASE 3");
            /* nothing to do for this Configurator request */
            break;

          default:
            rc = L7_ERROR;
            reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            break;
        }
        break;
      case L7_CNFGR_CMD_EXECUTE:
        PT_LOG_INFO(LOG_CTX_STARTUP,"EXECUTE");
        switch (pCmdData->u.rqstData.request)
        {
          case L7_CNFGR_RQST_E_START:
            /* nothing to do for this Configurator request */
            break;

          default:
            rc = L7_ERROR;
            reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            break;
        }
        break;
      case L7_CNFGR_CMD_UNCONFIGURE:
        switch (pCmdData->u.rqstData.request)
        {
          case L7_CNFGR_RQST_U_PHASE1_START:
            /* nothing to do for this Configurator request */
            break;

          case L7_CNFGR_RQST_U_PHASE2_START:
            /* nothing to do for this Configurator request */
            break;

          default:
            rc = L7_ERROR;
            reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            break;
        }
        break;
      case L7_CNFGR_CMD_TERMINATE:
        switch (pCmdData->u.rqstData.request)
        {
          case L7_CNFGR_RQST_T_START:
            /* nothing to do for this Configurator request */
            break;

          default:
            rc = L7_ERROR;
            reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            break;
        }
        break;
      case L7_CNFGR_CMD_SUSPEND:
        switch (pCmdData->u.rqstData.request)
        {
          case L7_CNFGR_RQST_S_START:
            /* nothing to do for this Configurator request */
            break;

          default:
            rc = L7_ERROR;
            reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            break;
        }
        break;
      case L7_CNFGR_CMD_RESUME:
        switch (pCmdData->u.rqstData.request)
        {
          case L7_CNFGR_RQST_R_START:
            /* nothing to do for this Configurator request */
            break;

          default:
            rc = L7_ERROR;
            reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            break;
        }
        break;
      default:
        rc = L7_ERROR;
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
        break;
    }
  }
  else  /* api called with something other than a request */
  {
    rc = L7_ERROR;
    reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
  }

  cbData.correlator = pCmdData->correlator;
  cbData.asyncResponse.rc = rc;
  cbData.asyncResponse.u.reason = reason;
  cnfgrApiCallback(&cbData);

  return(rc);
}

/*********************************************************************
* @purpose  Set the reason for starting the management software
*
* @param    startupReason {(input)} This is set by CM/Driver when
*                                   they fail to do warm start
*
* @returns  none
*
* @notes   
*
* @end
*********************************************************************/

void unitMgrSystemStartupReasonSet(L7_LAST_STARTUP_REASON_t startupReason)
{
  umSystemStartupReason = startupReason;
}

