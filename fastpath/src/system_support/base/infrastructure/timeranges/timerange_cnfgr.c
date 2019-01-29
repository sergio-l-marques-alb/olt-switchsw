
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename timerange_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component
*
* @comments
*
* @create 28/11/2009
*
* @author Siva Mannem
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "buff_api.h"
#include "timerange.h"
#include "timerange_api.h"
#include "nvstoreapi.h"
#include "timerange_cnfgr.h"
#include "timerange_cfg.h"
#include "timerange_control.h"

extern L7_BOOL                 timeRangeCompInitialized;
extern timeRangeCfgFileData_t  *timeRangeCfgFileData;
extern L7_uint32               timeRange_curr_entries_g;    /* total current time range entries */
extern avlTree_t               *pTimeRangeTree;

osapiRWLock_t                  timeRangeRwLock;
timeRangeCnfgrState_t          timeRangeCnfgrState   = TIMERANGE_PHASE_INIT_0;
void                           *timeRangeProcQueue   = L7_NULLPTR;
L7_int32                       timeRangeProcTaskId   = 0;
L7_uint32                      timeRangeBufferPoolId = 0;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for Time Range component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the timeRange comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void timeRangeApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  timeRangeMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;

  if (pCmdData == L7_NULLPTR)
  {
    cbData.correlator = 0;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    cbData.asyncResponse.rc = L7_ERROR;
    cnfgrApiCallback(&cbData);
    return;
  }
  /* If request is for Initialize Phase 1, call function to create
   * component's task, queues, etc.  All other command/requests get put on
   * component's queue to be processed in component's thread.
   */
  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    if (timeRangeInit(pCmdData) != L7_SUCCESS)
    {
      timeRangeInitUndo();
    }
  }
  else
  {
    memcpy(&msg.u.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.msgType = TIMERANGE_CNFGR_MSG;
    if (osapiMessageSend(timeRangeProcQueue,
                         &msg,
                         TIMERANGE_MSG_SIZE,
                         L7_NO_WAIT,
                         L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR,
             L7_TIMERANGES_COMPONENT_ID,
             "Failed to send message to Time Range Processing Message Queue");
    }
  }
  return;
}
/*********************************************************************
*
* @purpose  System Initialization for Time Range component
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
L7_RC_t timeRangeInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  timeRangeMsg_t     msg;
  L7_RC_t            rc = L7_SUCCESS;
  L7_CNFGR_CB_DATA_t cbData;

  timeRangeProcQueue = (void *)osapiMsgQueueCreate(TIMERANGE_PROC_QUEUE,
                                               TIMERANGE_MSG_COUNT,
                                               TIMERANGE_MSG_SIZE);
  if (timeRangeProcQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT,
           L7_TIMERANGES_COMPONENT_ID,
           "Failed to create Time Range Processing Message Queue");
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cnfgrApiCallback(&cbData);
    rc = L7_FAILURE;
  }

  if (timeRangeProcTaskStart() != L7_SUCCESS)
  {
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cnfgrApiCallback(&cbData);
    rc = L7_FAILURE;
  }

  memcpy(&msg.u.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgType = TIMERANGE_CNFGR_MSG;

  if (osapiMessageSend(timeRangeProcQueue,
                       &msg,
                       TIMERANGE_MSG_SIZE,
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT,
           L7_TIMERANGES_COMPONENT_ID,
           "Failed to send message to Time Range Processing Message Queue");
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_FATAL;
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cnfgrApiCallback(&cbData);
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  System Init Undo for Time Range component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void timeRangeInitUndo()
{
  if (timeRangeProcQueue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(timeRangeProcQueue);
  }

  if (timeRangeProcTaskId != L7_ERROR)
  {
    osapiTaskDelete(timeRangeProcTaskId);
  }
  timeRangeCnfgrState = TIMERANGE_PHASE_INIT_0;
}

/*********************************************************************
*
* @purpose  To parse the configurator commands
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void timeRangeCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             timeRangeRC = L7_ERROR;
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
                if ((timeRangeRC = timeRangeCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  timeRangeCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((timeRangeRC = timeRangeCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  timeRangeCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((timeRangeRC = timeRangeCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  timeRangeCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                timeRangeRC = timeRangeCnfgrNoopProccess( &response, &reason );
                timeRangeCnfgrState = TIMERANGE_PHASE_WMU;
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
                timeRangeCnfgrState = TIMERANGE_PHASE_EXECUTE;

                timeRangeRC  = L7_SUCCESS;
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
                timeRangeRC = timeRangeCnfgrNoopProccess( &response, &reason );
                timeRangeCnfgrState = TIMERANGE_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                timeRangeRC = timeRangeCnfgrUconfigPhase2( &response, &reason );
                timeRangeCnfgrState = TIMERANGE_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            timeRangeRC = timeRangeCnfgrNoopProccess( &response, &reason );
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

  }
  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;

  } /* check for command valid pointer */

  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = timeRangeRC;

  if (timeRangeRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason   = reason;
  }
  cnfgrApiCallback(&cbData);
  return;
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t timeRangeCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t   timeRangeRC;
  L7_uint32 bufferSize = (L7_uint32)sizeof(timeRangeEntryParms_t);
  L7_uint32 numBufs    =  L7_TIMERANGE_MAX_NUM * \
                          L7_TIMERANGE_MAX_ENTRIES_PER_TIMERANGE;


  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  timeRangeRC = L7_SUCCESS;


  if (osapiRWLockCreate(&timeRangeRwLock, OSAPI_RWLOCK_Q_PRIORITY) != L7_SUCCESS)
  {
    timeRangeRC = L7_FAILURE;
  }

  timeRangeCfgFileData  = osapiMalloc(L7_TIMERANGES_COMPONENT_ID, (L7_uint32)sizeof(timeRangeCfgFileData_t));
  if (timeRangeCfgFileData == L7_NULLPTR)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  memset(timeRangeCfgFileData, 0, sizeof(timeRangeCfgFileData_t));

  if (L7_SUCCESS != timeRangeCreateTree(L7_TIMERANGE_MAX_NUM))
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_TIMERANGES_COMPONENT_ID,
             "Failed to allocate memory for time range tree");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  /*Create a buffer pool for time entries in time ranges*/
  if (bufferPoolInit(numBufs, bufferSize, "Time Entry Bufs",
                     &timeRangeBufferPoolId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_TIMERANGES_COMPONENT_ID,
             "Failed to create buffer pool for time entries in time ranges");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  timeRange_curr_entries_g = 0;
  timeRangeCnfgrState      = TIMERANGE_PHASE_INIT_1;

  return timeRangeRC;
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t timeRangeCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t timeRangeRC;
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  timeRangeRC = L7_SUCCESS;

  memset(&notifyFunctionList, 0x00, sizeof(nvStoreFunctionList_t));

  notifyFunctionList.registrar_ID   = L7_TIMERANGES_COMPONENT_ID;
  notifyFunctionList.notifySave     = timeRangeSave;
  notifyFunctionList.hasDataChanged = timeRangeHasDataChanged;
  notifyFunctionList.resetDataChanged = timeRangeResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    timeRangeRC     = L7_ERROR;
    return timeRangeRC;
  }

  timeRangeCnfgrState = TIMERANGE_PHASE_INIT_2;
  return timeRangeRC;
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
L7_RC_t timeRangeCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t timeRangeRC;


  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  timeRangeRC   = L7_SUCCESS;

  timeRangeBuildDefaultConfigData(TIMERANGE_CFG_VER_CURRENT);
  /* TIME RANGE COMPONENT IS NOW INITIALIZED */
  timeRangeCompInitialized = L7_TRUE;

  if (timeRangeApplyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    timeRangeRC       = L7_ERROR;

    return timeRangeRC;
  }

  timeRangeTimerStart();
  timeRangeCfgFileData->cfgHdr.dataChanged = L7_FALSE;
  timeRangeCnfgrState = TIMERANGE_PHASE_INIT_3;

  return timeRangeRC;
}


/*********************************************************************
* @purpose  This function undoes timeRangeCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void timeRangeCnfgrFiniPhase1Process()
{
  if (timeRangeProcTaskId != L7_ERROR)
  {
    osapiTaskDelete(timeRangeProcTaskId);
  }

  timeRangeDeleteTree();
  (void)osapiSemaDelete(pTimeRangeTree->semId);

  timeRange_curr_entries_g = 0;

  if (timeRangeCfgFileData != L7_NULLPTR)
  {
    osapiFree(L7_TIMERANGES_COMPONENT_ID, timeRangeCfgFileData);
    timeRangeCfgFileData = L7_NULLPTR;
  }
  if (timeRangeBufferPoolId != 0)
  {
    bufferPoolTerminate(timeRangeBufferPoolId);
    timeRangeBufferPoolId = 0;
  }
}


/*********************************************************************
* @purpose  This function undoes timeRangeCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void timeRangeCnfgrFiniPhase2Process()
{
  nvStoreDeregister(L7_TIMERANGES_COMPONENT_ID);
  timeRangeCnfgrState = TIMERANGE_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes timeRangeCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void timeRangeCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place timeRangeCnfgrState to WMU */
  (void)timeRangeCnfgrUconfigPhase2(&response, &reason);
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
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t timeRangeCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t timeRangeRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(timeRangeRC);
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
* @end
*********************************************************************/

L7_RC_t timeRangeCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t timeRangeRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  timeRangeRC       = L7_SUCCESS;

  /* clear out the applied configuration */
  (void)timeRangeRestore();
  (void)timeRangeTimerStop();

  if (timeRangeCfgFileData != L7_NULLPTR)
  {
    memset(timeRangeCfgFileData, 0, sizeof(timeRangeCfgFileData_t));
  }
  timeRangeCnfgrState = TIMERANGE_PHASE_WMU;

  return timeRangeRC;
}

