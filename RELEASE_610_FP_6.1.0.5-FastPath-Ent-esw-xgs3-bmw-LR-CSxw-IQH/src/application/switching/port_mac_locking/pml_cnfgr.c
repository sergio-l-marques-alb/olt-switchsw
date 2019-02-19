/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pml_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component 
*
* @comments 
*
* @create 05/21/2004
*
* @author colinw
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "l7_product.h"
#include "nvstoreapi.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "pml.h"
#include "pml_util.h"

pmlCnfgrState_t          pmlCnfgrState = PML_PHASE_INIT_0;
extern void             *pmlQueue;
extern L7_uint32        *pmlMapTbl;
extern L7_int32          pmlTaskId;
extern pmlCfgData_t     *pmlCfgData;
extern pmlIntfOprData_t *pmlIntfOprData;
extern pmlIntfInfo_t    *pmlIntfInfo;


/*********************************************************************
*
* @purpose  CNFGR System Initialization for ACL component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the PML comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pmlApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  pmlMgmtMsg_t msg;

/*
 * Let all but PHASE 1 start fall through into an osapiMessageSend.
 * The application task will handle everything.
 * Phase 1 will do a osapiMessageSend after a few pre reqs have been
 * completed
 */

  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) && 
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* this function will invoke the message send for phase 1 */
    if (pmlInit(pCmdData) != L7_SUCCESS)
      pmlInitUndo();
  } else
  {
    memcpy(&msg.u.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.msgId = pmlCnfgr;
    osapiMessageSend(pmlQueue, &msg, PML_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  }
}


/*********************************************************************
*
* @purpose  System Initialization for Port MAC Locking component
*
* @param    none
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  pmlMgmtMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;

  pmlQueue = (void *)osapiMsgQueueCreate(PML_QUEUE, PML_MSG_COUNT, PML_MSG_SIZE);
  if (pmlQueue == L7_NULLPTR)
  {
    LOG_MSG("pmlInit: msgQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (pmlStartTask() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  memcpy(&msg.u.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.msgId = pmlCnfgr;

  osapiMessageSend(pmlQueue, &msg, PML_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for Port MAC Locking component
*
* @param    none
*                            
* @notes    none
*
* @end
*********************************************************************/
void pmlInitUndo()
{
  if (pmlQueue != L7_NULLPTR)
    osapiMsgQueueDelete(pmlQueue);


  if (pmlTaskId != L7_ERROR)
    osapiTaskDelete(pmlTaskId);

  pmlCnfgrState = PML_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t pmlCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t pmlRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  pmlRC     = L7_SUCCESS;


  pmlCfgData     = osapiMalloc(L7_PORT_MACLOCK_COMPONENT_ID, sizeof(pmlCfgData_t));
  pmlMapTbl      = osapiMalloc(L7_PORT_MACLOCK_COMPONENT_ID, sizeof(L7_uint32) * platIntfMaxCountGet());
  pmlIntfOprData = osapiMalloc(L7_PORT_MACLOCK_COMPONENT_ID, sizeof(pmlIntfOprData_t) * platIntfMaxCountGet());
  pmlIntfInfo    = osapiMalloc(L7_PORT_MACLOCK_COMPONENT_ID, sizeof(pmlIntfInfo_t) * platIntfMaxCountGet());

  if ((pmlCfgData     == L7_NULLPTR) ||
      (pmlMapTbl      == L7_NULLPTR) ||
      (pmlIntfOprData == L7_NULLPTR) ||
      (pmlIntfInfo    == L7_NULLPTR))
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    pmlRC   = L7_ERROR;

    return pmlRC;
  }

  /* Zero bitmasks to indicate no interfaces are enabled */
  memset((void *)pmlCfgData, 0, sizeof(pmlCfgData_t));
  memset((void *)pmlMapTbl, 0, sizeof(L7_uint32) * platIntfMaxCountGet());
  memset((void *)pmlIntfOprData, 0, sizeof(pmlIntfOprData_t) * platIntfMaxCountGet());
  memset((void *)pmlIntfInfo, 0, sizeof(pmlIntfInfo_t) * platIntfMaxCountGet());

  pmlCnfgrState = PML_PHASE_INIT_1;

  return pmlRC;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* 
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t pmlCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t pmlRC;
  nvStoreFunctionList_t pmlNotifyFunctionList;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  pmlRC     = L7_SUCCESS;


  memset((void *) &pmlNotifyFunctionList, 0, sizeof(pmlNotifyFunctionList));
  pmlNotifyFunctionList.registrar_ID   = L7_PORT_MACLOCK_COMPONENT_ID;
  pmlNotifyFunctionList.notifySave     = pmlSave;
  pmlNotifyFunctionList.hasDataChanged = pmlHasDataChanged;
  pmlNotifyFunctionList.notifyConfigDump     = pmlCfgDump;
  pmlNotifyFunctionList.resetDataChanged = pmlResetDataChanged;

  if (nvStoreRegister(pmlNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    pmlRC     = L7_ERROR;

    return pmlRC;

  }

  if (nimRegisterIntfChange(L7_PORT_MACLOCK_COMPONENT_ID, pmlIntfChangeCallback) != L7_SUCCESS)
  {
    LOG_MSG("Port MAC Locking: Unable to register with NIM\n");

    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    pmlRC     = L7_ERROR;

    return pmlRC;
  }

  pmlCnfgrState = PML_PHASE_INIT_2;

  return pmlRC;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t pmlCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t pmlRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  pmlRC     = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_PORT_MACLOCK_COMPONENT_ID, PML_CFG_FILENAME,
                   (L7_char8 *)pmlCfgData, sizeof(pmlCfgData_t),
                   &pmlCfgData->checkSum, PML_CFG_VER_CURRENT,
                   pmlBuildDefaultConfigData, pmlMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    pmlRC     = L7_ERROR;

    return pmlRC;
  }

  if (pmlApplyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    pmlRC     = L7_ERROR;

    return pmlRC;
  }

  pmlCnfgrState = PML_PHASE_INIT_3;

  return pmlRC;
}


/*********************************************************************
* @purpose  This function undoes pmlCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pmlCnfgrFiniPhase1Process()
{

  if (pmlCfgData != L7_NULLPTR)
  {
    osapiFree(L7_PORT_MACLOCK_COMPONENT_ID, pmlCfgData);
    pmlCfgData = L7_NULLPTR;
  }

  if (pmlMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_PORT_MACLOCK_COMPONENT_ID, pmlMapTbl);
    pmlMapTbl = L7_NULLPTR;
  }

  if (pmlIntfOprData != L7_NULLPTR)
  {
    osapiFree(L7_PORT_MACLOCK_COMPONENT_ID, pmlIntfOprData);
    pmlIntfOprData = L7_NULLPTR;
  }

  if (pmlIntfInfo != L7_NULLPTR)
  {
    osapiFree(L7_PORT_MACLOCK_COMPONENT_ID, pmlIntfInfo);
    pmlIntfInfo = L7_NULLPTR;
  }

  pmlInitUndo();

  pmlCnfgrState = PML_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes pmlCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pmlCnfgrFiniPhase2Process()
{
  (void)nimDeRegisterIntfChange(L7_PORT_MACLOCK_COMPONENT_ID);
  (void)nvStoreDeregister(L7_PORT_MACLOCK_COMPONENT_ID);

  pmlCnfgrState = PML_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes pmlCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pmlCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place pmlCnfgrState to WMU */
  pmlCnfgrUconfigPhase2(&response, &reason);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse - @b{(output)}  Response always command complete.
*     
* @param    pReason   - @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           None.
*
* @end
*********************************************************************/
L7_RC_t pmlCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t pmlRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(pmlRC);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @end
*********************************************************************/

L7_RC_t pmlCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t pmlRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  pmlRC     = L7_SUCCESS;

  if (pmlAdminModeApply(L7_DISABLE) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    pmlRC     = L7_ERROR;

    return pmlRC;
  }

  memset(pmlCfgData, 0, sizeof(pmlCfgData));

  pmlCnfgrState = PML_PHASE_WMU;

  return pmlRC;
}


/*********************************************************************
*
* @purpose  To parse the configurator commands send to pmlTask
*
* @param    none
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void pmlCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             pmlRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if ( request > L7_CNFGR_RQST_FIRST && 
           request < L7_CNFGR_RQST_LAST )
      {
        /* validate command/event pair */
        switch ( command )
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch ( request )
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((pmlRC = pmlCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  pmlCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((pmlRC = pmlCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  pmlCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((pmlRC = pmlCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  pmlCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                pmlRC = pmlCnfgrNoopProccess( &response, &reason );
                pmlCnfgrState = PML_PHASE_WMU;
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            switch ( request )
            {
              case L7_CNFGR_RQST_E_START:
                pmlCnfgrState = PML_PHASE_EXECUTE;

                pmlRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = 0;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                pmlRC = pmlCnfgrNoopProccess( &response, &reason );
                pmlCnfgrState = PML_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                pmlRC = pmlCnfgrUconfigPhase2( &response, &reason );
                pmlCnfgrState = PML_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            pmlRC = pmlCnfgrNoopProccess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */


      } else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;


      } /* endif validate request */


    } else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;


    } /* endif validate command type */


  } else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;


  } /* check for command valid pointer */

  /* return value to caller - 
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = pmlRC;
  if (pmlRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);


  return;
}

