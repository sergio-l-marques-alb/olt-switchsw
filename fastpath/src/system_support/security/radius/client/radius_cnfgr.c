/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename radius_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component 
*
* @comments 
*
* @create 08/06/2003
*
* @author msmith
* @end
*
**********************************************************************/
#include "radius_include.h"

radiusCnfgrState_t radiusCnfgrState = RADIUS_PHASE_INIT_0;

extern void * radius_queue;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for Radius component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                            
* @notes    This API is provided to allow the Configurator to issue a
*           request to the radius comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void radiusApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  radiusMsg_t   msg;
  L7_CNFGR_CB_DATA_t    cbData;

/*
 * Let all but PHASE 1 start fall through into an osapiMessageSend.
 * The application task will handle everything.
 * Phase 1 will do a osapiMessageSend after a few pre reqs have been
 * completed
 */

  if ( pCmdData == L7_NULL)
  {
    cbData.correlator               = L7_NULL;
    cbData.asyncResponse.rc         = L7_ERROR; 
    cbData.asyncResponse.u.reason   = L7_CNFGR_ERR_RC_INVALID_CMD;

    cnfgrApiCallback(&cbData);
    return;
  }

  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) && 
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* this function will invoke the message send for phase 1 */
    if (radiusInit(pCmdData) != L7_SUCCESS)
      radiusInitUndo();
  }
  else
  {
    memcpy(&msg.data.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.event = RADIUS_CNFGR_INIT;
    (void)osapiMessageSend(radius_queue, &msg, sizeof(radiusMsg_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM); 
  }

  return;
}

