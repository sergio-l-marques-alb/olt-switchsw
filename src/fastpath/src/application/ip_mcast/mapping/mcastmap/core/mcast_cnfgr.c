/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename mcast_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component
*
* @comments
*
* @create 06/23/2003
*
* @author gkiran
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "l3_commdefs.h"
#include "l3_mcast_default_cnfgr.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "nvstoreapi.h"
#include "l7_ip_api.h"
#include "l7_mcast_api.h"
#include "dot1q_api.h"
#include "mcastmap_sid.h"
#include "l7_mgmd_api.h"
#include "mcast_map.h"
#include "mcast_util.h"
#include "mcast_packet.h"
#include "mcast_debug.h"
#include "rtmbuf.h"
#include "mfc_api.h"
#include "mfc_map.h"
#include "buff_api.h"
#include "pktrcvr.h"
#include "heap_api.h"
#include "sdm_api.h"

#ifdef L7_NSF_PACKAGE
#include "mcast_ckpt.h"
#endif


extern void mcastDebugRegister(void);
extern void mcastDebugCfgRead(void);

static L7_RC_t mcastMapTaskInit(L7_CNFGR_CMD_DATA_t *pCmdData);
static void    mcastMapTaskInitUndo(void);

static
L7_RC_t mcastCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason);

static
void mcastCnfgrFiniPhase1Process();

static
L7_RC_t mcastCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason );
static
void mcastCnfgrFiniPhase2Process();

static
L7_RC_t mcastCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
static
void mcastCnfgrFiniPhase3Process();

static
L7_RC_t mcastCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );

static
L7_RC_t mcastCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
*
* @purpose  CNFGR System Initialization for MCAST component
*
* @param    pCmdData  @b{(input)}  Data structure for this
*                                   CNFGR request
*
* @returns  none
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the mcast comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void mcastApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  mcastMapMsg_t        msg;

/*
 * Let all but PHASE 1 start fall through into an osapiMessageSend.
 * The mapping task will handle everything.
 */
  if (pCmdData == L7_NULLPTR)
  {
    L7_CNFGR_CB_DATA_t  cbData;

    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Input parameters for configure command "
             "not initialized.\n");
    /* game over... pCmdData is a null pointer */
    cbData.correlator             = 0;
    cbData.asyncResponse.rc       = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
  }
  else
  {
    if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
        (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
    {
      /* start the MCAST Map task to prepare for processing
       * configurator (and other) events.
       */
      if (mcastMapTaskInit(pCmdData) != L7_SUCCESS)
      {
        mcastMapTaskInitUndo();
      }
    }
    else
    {
      /* send the configurator request to the task thread for processing */
      memcpy(&msg.u.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
      msg.msgId = MCASTMAP_CNFGR_MSG;
      if (mcastMapMessageSend(MCASTMAP_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
      {
        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to send configuration event to mcastMap task\n");
        return;
      }
    }
  }
}

/*********************************************************************
*
* @purpose  Initialization for MCAST Mapping component task
*
* @param    pCmdData  @b{(input)}  Data structure for this
*                                  CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mcastMapTaskInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  mcastMapMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;
  L7_uint32 count=0;


  memset (&mcastGblVariables_g, 0, sizeof(mcastGblVar_t));
  mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_INIT_0;


  memcpy(mcastGblVariables_g.mcastMapQueue[MCASTMAP_APP_TIMER_Q].QName,
         MCASTMAP_APP_TMR_QUEUE,sizeof(MCASTMAP_APP_TMR_QUEUE));

  mcastGblVariables_g.mcastMapQueue[MCASTMAP_APP_TIMER_Q].QSize  = MCASTMAP_APP_TMR_MSG_SIZE;
  mcastGblVariables_g.mcastMapQueue[MCASTMAP_APP_TIMER_Q].QCount = MCASTMAP_APP_TMR_MSG_COUNT;

  memcpy(mcastGblVariables_g.mcastMapQueue[MCASTMAP_EVENT_Q].QName,
            MCASTMAP_MSG_QUEUE,sizeof(MCASTMAP_MSG_QUEUE));
  mcastGblVariables_g.mcastMapQueue[MCASTMAP_EVENT_Q].QSize      = MCASTMAP_MSG_SIZE;
  mcastGblVariables_g.mcastMapQueue[MCASTMAP_EVENT_Q].QCount     = MCASTMAP_MSG_COUNT;


  memcpy(mcastGblVariables_g.mcastMapQueue[MCASTMAP_CTRL_PKT_Q].QName,
            MCASTMAP_CTRL_PKT_QUEUE,sizeof(MCASTMAP_CTRL_PKT_QUEUE));
  mcastGblVariables_g.mcastMapQueue[MCASTMAP_CTRL_PKT_Q].QSize   = MCASTMAP_CTRL_PKT_MSG_SIZE;
  mcastGblVariables_g.mcastMapQueue[MCASTMAP_CTRL_PKT_Q].QCount  = MCASTMAP_CTRL_PKT_MSG_COUNT;


  for(count=0;count < MCASTMAP_MAX_Q;count++)
  {
    mcastGblVariables_g.mcastMapQueue[count].QPointer=
        (void *)osapiMsgQueueCreate(mcastGblVariables_g.mcastMapQueue[count].QName,
                                    mcastGblVariables_g.mcastMapQueue[count].QCount,
                                    mcastGblVariables_g.mcastMapQueue[count].QSize);

    if(mcastGblVariables_g.mcastMapQueue[count].QPointer == L7_NULLPTR)
    {
      cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);

      mcastMapQDelete();
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Mcast Map msgQueue creation error for Q %d\n",count);
      return L7_FAILURE;
    }
  }

  mcastGblVariables_g.mcastMapMsgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);
  if (mcastGblVariables_g.mcastMapMsgQSema == L7_NULL)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to create msgQueue semaphore\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    mcastMapQDelete();
    return L7_FAILURE;
  }

  /* Create the memory for the Handle List - both for IPv4 and IPv6 */
  mcastGblVariables_g.mcastHandleListMemHndl = (handle_member_t*)
                 osapiMalloc (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                              sizeof (handle_member_t) * MCAST_MAX_TIMERS);
  if (mcastGblVariables_g.mcastHandleListMemHndl == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "MCAST_MAP: Failed to Alloc Memory for Handle List.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    mcastMapQDelete();
    return L7_FAILURE;
  }

#ifdef L7_IPV6_PACKAGE
  mcastGblVariables_g.mcastV6HandleListMemHndl = (handle_member_t*)
                 osapiMalloc (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                              sizeof (handle_member_t) * MCAST_MAX_TIMERS);
  if (mcastGblVariables_g.mcastV6HandleListMemHndl == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "MCAST_MAP: Failed to Alloc Memory for V6 Handle List.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    mcastMapQDelete();
    return L7_FAILURE;
  }
#endif

  /* Create buffer pool for APP Timers */
  if (bufferPoolInit(MCAST_MAX_TIMERS, L7_APP_TMR_NODE_SIZE,
      "App Timer", &mcastGblVariables_g.mcastAppTimerBufPoolId) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: failed to create buffer pool for App Timers.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    mcastMapQDelete();
    return L7_FAILURE;
  }

  mcastGblVariables_g.mcastMapTaskId = osapiTaskCreate("mcastMapTask",
                                   mcastMapTask, 0, 0,
                                   mcastMapSidDefaultStackSize(),
                                   mcastMapSidDefaultTaskPriority(),
                                   mcastMapSidDefaultTaskSlice());

  if ((mcastGblVariables_g.mcastMapTaskId == L7_ERROR) ||
      (osapiWaitForTaskInit (L7_MCAST_MAP_TASK_SYNC,
                             L7_WAIT_FOREVER) != L7_SUCCESS))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to create mcast map Task \n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    mcastMapQDelete();
    return L7_FAILURE;
  }

#ifdef L7_IPV6_PACKAGE
  mcastGblVariables_g.pktRcvr = (L7_PktRcvr_t*)
      pktRcvrCreate(L7_FLEX_MCAST_MAP_COMPONENT_ID, L7_IPV6_PKT_RCVR_ID_MAX);

  if(mcastGblVariables_g.pktRcvr == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nPKT_RCVR: Failed to initialize Packet Receiver \n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    mcastMapQDelete();
    return L7_FAILURE;
  }

  mcastGblVariables_g.pktRcvrTaskId = osapiTaskCreate("pktRcvrTask",
                                   pktRcvrTask, 0, 0,
                                   mcastMapSidDefaultStackSize(),
                                   mcastMapSidDefaultTaskPriority(),
                                   mcastMapSidDefaultTaskSlice());

  if ((mcastGblVariables_g.pktRcvrTaskId == L7_ERROR) ||
      (osapiWaitForTaskInit (L7_PKT_RCVR_TASK_SYNC,
                             L7_WAIT_FOREVER) != L7_SUCCESS))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nPKT_RCVR: Failed to create Packet Receiver \n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    mcastMapQDelete();
    return L7_FAILURE;
  }
#endif

  memcpy(&msg.u.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = MCASTMAP_CNFGR_MSG;

  if (mcastMapMessageSend(MCASTMAP_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to send configure msg to mcast map task.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_BUSY;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for MCAST Mapping component
*
* @param    none
*
* @comments none
*
* @returns  none
*
* @end
*********************************************************************/
void mcastMapTaskInitUndo()
{
  mcastMapQDelete();

  if (mcastGblVariables_g.mcastMapTaskId != L7_ERROR)
  {
    osapiTaskDelete(mcastGblVariables_g.mcastMapTaskId);
  }


  if (mcastGblVariables_g.pktRcvr != L7_NULLPTR)
  {
    pktRcvrDestroy(&(mcastGblVariables_g.pktRcvr));
  }

  if (mcastGblVariables_g.pktRcvrTaskId != L7_ERROR)
  {
    osapiTaskDelete(mcastGblVariables_g.pktRcvrTaskId);
  }

  mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_INIT_0;
}

/*********************************************************************
*
* @purpose  CNFGR System Initialization for MCAST component
*
* @param    pCmdData    @b{(input)} Data structure for this
*                                    CNFGR request
*
* @returns  none
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the mcast comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void mcastCnfgrCommandProcess(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             mcastRC = L7_ERROR;
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
                if ((mcastRC = mcastCnfgrInitPhase1Process( &response,
                               &reason )) != L7_SUCCESS)
                {
                  mcastCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((mcastRC = mcastCnfgrInitPhase2Process( &response,
                               &reason )) != L7_SUCCESS)
                {
                  mcastCnfgrFiniPhase2Process();
                }
                break;
            case L7_CNFGR_RQST_I_PHASE3_START:
              /* Determine if this is a cold or warm restart */
                if ((pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM))
                {
                  mcastGblVariables_g.warmRestart = L7_TRUE;
                  mcastGblVariables_g.warmRestartInProgress = L7_TRUE;
                }
                else
                {
                  mcastGblVariables_g.warmRestart = L7_FALSE;
                  mcastGblVariables_g.warmRestartInProgress = L7_FALSE;
                }
                if ((mcastRC = mcastCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  mcastCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                mcastRC = mcastCnfgrNoopProcess( &response, &reason );
                mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_WMU;
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
                mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_EXECUTE;

                mcastRC  = L7_SUCCESS;
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
                mcastRC = mcastCnfgrNoopProcess( &response, &reason );
                mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                mcastRC = mcastCnfgrUconfigPhase2( &response, &reason );
                mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            mcastRC = mcastCnfgrNoopProcess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */

      }
      else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;

      } /* endif validate request */

    }
    else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;

    } /* endif validate command type */

  }
  else
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Invalid Command = %d in configure command process \n",
             pCmdData);
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;

  } /* check for command valid pointer */

  /* return value to caller -
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = mcastRC;
  if (mcastRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   @b{(output)}  Reason.
*
* @returns  L7_SUCCESS There were no errors. Response is available.
* @returns  L7_FAILURE There were failures. Reason code is available.
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*           The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*           This function runs in the configurator's thread.
*           This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_RC_t mcastCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t *pReason)
{
  L7_uint32 heapsize;
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;


  /***********************************************************
   * Initialize multicast configuration structures *
   ***********************************************************/
  memset(&mcastGblVariables_g.mcastMapCfgData, 0,
                                   sizeof(L7_mcastMapCfg_t));

  mcastGblVariables_g.pMcastMapCfgMapTbl =
    (L7_uint32 *)osapiMalloc(L7_FLEX_MCAST_MAP_COMPONENT_ID,
                             sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);
  if (mcastGblVariables_g.pMcastMapCfgMapTbl == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to allocate memory for interface config table\n");
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }
  memset((void *)mcastGblVariables_g.pMcastMapCfgMapTbl,
                 0, (sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT));

  /*******************************************************
   * Initialize general multicast routing info structure *
   *******************************************************/
  memset(&mcastGblVariables_g.mcastMapInfo, 0, sizeof(mcastMapInfo_t));


  /***********************************************************
   * Initialize Notify Lists for Admin mode and Admin scope  *
   **********************************************************/

  mcastGblVariables_g.mcastMapAdminScopeSema =
                      osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if (mcastGblVariables_g.mcastMapAdminScopeSema == L7_NULL)
  {
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }

  memset((void *)mcastGblVariables_g.notifyMcastModeChange, 0,
      sizeof(mcastGblVariables_g.notifyMcastModeChange));

  memset((void *)mcastGblVariables_g.notifyAdminScopeChange, 0,
      sizeof(mcastGblVariables_g.notifyAdminScopeChange));

  if ((mcastGblVariables_g.mcastMapInfo.eventCompleteSignal =
        osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY)) == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\n Failed to create"
                    "mcastMapInfo eventCompleteSignal\n");
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\n MCAST_MAP: Semaphore creation error.\n");
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }

  /* Initialize the event handler */
  memset( (void *)&(mcastGblVariables_g.mcastMapInfo.eventHandler),
                                     0, sizeof(asyncEventHandler_t) );
  mcastGblVariables_g.mcastMapInfo.eventHandler.componentId
                                     = L7_FLEX_MCAST_MAP_COMPONENT_ID;
  mcastGblVariables_g.mcastMapInfo.eventHandler.maxCorrelators
                                     = L7_MCAST_MAX_ASYNC_EVENTS;
  mcastGblVariables_g.mcastMapInfo.eventHandler.registeredMask
                 = &(mcastGblVariables_g.mcastMapInfo.registeredComponents);
  mcastGblVariables_g.mcastMapInfo.eventHandler.timeout
                                     = L7_MCAST_MAX_ASYNC_EVENT_TIMEOUT;
  mcastGblVariables_g.mcastMapInfo.eventHandler.complete_notify
                                     = (void *)mcastMapAsyncEventCompleteNotify;
  osapiStrncpySafe((L7_uchar8 *)&(mcastGblVariables_g.mcastMapInfo.eventHandler.name),
          "mcastAsyncEvents",(ASYNC_MAX_HANDLER_NAME-1));

  if(asyncEventHandlerCreate(&(mcastGblVariables_g.mcastMapInfo.eventHandler),
         &(mcastGblVariables_g.mcastMapInfo.eventHandlerId) ) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to create event handler for mcast map info\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
     return L7_FAILURE;
  }

  if(cnfgrBaseTechnologyTypeGet() == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4 ||
     cnfgrBaseTechnologyTypeGet() == L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS5)       /* PTin added: XGS5 family supported */
  { 
    if (sdmTemplateSupportsIpv6())
    {
      heapsize = MCAST_V4_HEAP_SIZE;
    }
    else
    { 
      /* This is special case for xgs4 family,for IPv6 build with IPv4 default or
       * data center templates as active.For the above mentioned case required heap 
       * size for IPv4 family is 60M, but MCAST_V4_HEAP_SIZE is 44.6M only.Increse
       * heap size for IPv4 default or IPv4 datecenter templates to 60M.*/
      heapsize = MCAST_V4_HEAP_SIZE + MCAST_V6_HEAP_SIZE;
    }
  }
  else
  {
    heapsize = MCAST_V4_HEAP_SIZE;
  }

  /* Create the Multicast Heap */
  mcastGblVariables_g.mcastV4HeapId = heapCreate(L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                                 heapsize);
  if(mcastGblVariables_g.mcastV4HeapId == L7_NULL)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: failed to create multicast heap.\n");
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }

#ifdef L7_IPV6_PACKAGE
  if (sdmTemplateSupportsIpv6())
  {
    mcastGblVariables_g.mcastV6HeapId = heapCreate(L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                                   MCAST_V6_HEAP_SIZE);
    if(mcastGblVariables_g.mcastV6HeapId == L7_NULL)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: failed to create multicast heap.\n");
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      return L7_FAILURE;
    }
  }
  else
  {
    mcastGblVariables_g.mcastV6HeapId = L7_NULL;
  }
#endif /* L7_IPV6_PACKAGE */

  /* Create buffer pool for IGMP and PIM control packets */
  if (bufferPoolInit((MCAST_PKT_BUF_COUNT), L7_MULTICAST_MAX_IP_MTU,
        "Mcast-Ctrl-Pkts Buf", &mcastGblVariables_g.mcastV4CtrlPktPoolId) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: failed to create buffer pool for mcast packets\n");
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }

   /* Note : This has to be watched out V4 builds only as the pktRecvr is created
             on V4 builds also but the buffer pools are created on V6 package
             inclusion only.
   */
#ifdef L7_IPV6_PACKAGE
  /* Create buffer pool for IPv6 MFC packets */
  if (bufferPoolInit(MCAST_PKT_BUF_COUNT, L7_MULTICAST_MAX_IP_MTU,
        "McastV6Data Bufs", &mcastGblVariables_g.mcastV6DataPktPoolId) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: failed to create buffer pool for IPv6 mcast packets\n");
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }

  /* Create buffer pool for IGMP and PIM control packets */
  if (bufferPoolInit((MCAST_PKT_BUF_COUNT), L7_MULTICAST_MAX_IP_MTU,
        "McastV6-Ctrl-Pkts Buf", &mcastGblVariables_g.mcastV6CtrlPktPoolId) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: failed to create buffer pool for V6 mcast packets\n");
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }
#endif

  /* Create buffer Pool for MGMD IPv4 Group Membership Events to MRPs */
  if (bufferPoolInit ((MCASTMAP_MGMD_EVENTS_COUNT_LIMIT * MGMD_MAX_QUERY_SOURCES),
                      sizeof (mrp_source_record_t), "MGMD_MRP_V4_Events_Pool",
                      &mcastGblVariables_g.mcastV4MgmdEventsPoolId)
                   != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Failed to Initialize Buffer Pool for MGMD V4 Events");
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }

#ifdef L7_IPV6_PACKAGE
  /* Create buffer Pool for MGMD IPv4 Group Membership Events to MRPs */
  if (bufferPoolInit ((MCASTMAP_MGMD_EVENTS_COUNT_LIMIT * MGMD_MAX_QUERY_SOURCES),
                      sizeof (mrp_source_record_t), "MGMD_MRP_V6_Events_Pool",
                      &mcastGblVariables_g.mcastV6MgmdEventsPoolId)
                   != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Failed to Initialize Buffer Pool for MGMD V6 Events");
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }
#endif

#ifdef L7_IPV6_PACKAGE
  /* Initialize the IPv6 Configuration Structure.
   * NOTE: This structure is "currently used" for storing IPv6 Static MRoutes
   * Information only.
   */
  if ((mcastGblVariables_g.mcastMapV6CfgData =
                (L7_mcastMapCfg_t*) osapiMalloc (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                                 sizeof(L7_mcastMapCfg_t)))
                                              == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Failed to Allocate Memory for "
                     "mcastMapV6CfgData");
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }

  memset(mcastGblVariables_g.mcastMapV6CfgData, 0, sizeof(L7_mcastMapCfg_t));
#endif

#ifdef L7_NSF_PACKAGE
  if (mcastCkptInit() != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Failure in allocating checkpoint resources");
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }
#endif

  /* Initialize the Phase 1 vendor code */
  if(mfcCnfgrPhase1Init() != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\n MCAST_MAP: Failed to initialize MFC Phase 1 \n");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }

  /* Read and apply trace configuration at phase 1.
   This allows for tracing during system initialization and
   during clear config */
  mcastDebugCfgRead();
  mcastApplyDebugConfigData();

  mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_INIT_1;

  return L7_SUCCESS;

}
/*********************************************************************
* @purpose  This function undoes mcastCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mcastCnfgrFiniPhase1Process()
{

  mcastGblVariables_g.mcastMapInfo.mcastV4Initialized = L7_FALSE;
  mcastGblVariables_g.mcastMapInfo.mcastV6Initialized = L7_FALSE;

  mfcCnfgrPhase1InitUndo();

  /* Deallocate anything that was allocated */
  if (mcastGblVariables_g.pMcastMapCfgMapTbl != L7_NULL)
  {
    osapiFree(L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                 mcastGblVariables_g.pMcastMapCfgMapTbl);
  }

  if (mcastGblVariables_g.mcastMapInfo.eventCompleteSignal != L7_NULL)
  {
    osapiSemaDelete(mcastGblVariables_g.mcastMapInfo.eventCompleteSignal);
  }

  if (mcastGblVariables_g.mcastMapAdminScopeSema != L7_NULL)
  {
    osapiSemaDelete(mcastGblVariables_g.mcastMapAdminScopeSema);
    mcastGblVariables_g.mcastMapAdminScopeSema = L7_NULL;
  }
  if (mcastGblVariables_g.mcastV4CtrlPktPoolId != 0)
  {
    bufferPoolDelete(mcastGblVariables_g.mcastV4CtrlPktPoolId);
    mcastGblVariables_g.mcastV4CtrlPktPoolId = 0;
  }

  if (mcastGblVariables_g.mcastHandleListMemHndl != 0)
  {
    osapiFree(L7_FLEX_MCAST_MAP_COMPONENT_ID, mcastGblVariables_g.mcastHandleListMemHndl);
    mcastGblVariables_g.mcastHandleListMemHndl = 0;
  }
  if (mcastGblVariables_g.mcastAppTimerBufPoolId != 0)
  {
    bufferPoolDelete(mcastGblVariables_g.mcastAppTimerBufPoolId);
    mcastGblVariables_g.mcastAppTimerBufPoolId = 0;
  }

  if (mcastGblVariables_g.mcastV4HeapId != L7_NULL)
  {
    heapDestroy(mcastGblVariables_g.mcastV4HeapId, L7_FALSE);
    mcastGblVariables_g.mcastV4HeapId = L7_NULL;
  }
  if (mcastGblVariables_g.mcastV4MgmdEventsPoolId != L7_NULL)
  {
    bufferPoolDelete(mcastGblVariables_g.mcastV4MgmdEventsPoolId);
    mcastGblVariables_g.mcastV4MgmdEventsPoolId = 0;
  }

#ifdef L7_IPV6_PACKAGE
  if (mcastGblVariables_g.mcastV6CtrlPktPoolId != 0)
  {
    bufferPoolDelete(mcastGblVariables_g.mcastV6CtrlPktPoolId);
    mcastGblVariables_g.mcastV6CtrlPktPoolId = 0;
  }
  if (mcastGblVariables_g.mcastV6DataPktPoolId != 0)
  {
    bufferPoolDelete(mcastGblVariables_g.mcastV6DataPktPoolId);
    mcastGblVariables_g.mcastV6DataPktPoolId = 0;
  }
  if (mcastGblVariables_g.mcastV6HandleListMemHndl != 0)
  {
    osapiFree(L7_FLEX_MCAST_MAP_COMPONENT_ID, mcastGblVariables_g.mcastV6HandleListMemHndl);
    mcastGblVariables_g.mcastV6HandleListMemHndl = 0;
  }
  if (mcastGblVariables_g.mcastV6HeapId != L7_NULL)
  {
    heapDestroy(mcastGblVariables_g.mcastV6HeapId, L7_FALSE);
    mcastGblVariables_g.mcastV6HeapId = L7_NULL;
  }
  if (mcastGblVariables_g.mcastV6MgmdEventsPoolId != L7_NULL)
  {
    bufferPoolDelete(mcastGblVariables_g.mcastV6MgmdEventsPoolId);
    mcastGblVariables_g.mcastV6MgmdEventsPoolId = 0;
  }
#endif

  mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_INIT_0;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS There were no errors. Response is available.
* @returns  L7_FAILURE There were errors. Reason code is available.
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*           The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*           This function runs in the configurator's thread.
*           This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_RC_t mcastCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t *pReason )
{
  sysnetPduIntercept_t sysnetPduIntercept;
  nvStoreFunctionList_t      notifyFunctionList;
#if 0
  L7_uint32 eventMask = 0;
#endif
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  /************************
   * nvStore registration *
   ************************/
  notifyFunctionList.registrar_ID    = L7_FLEX_MCAST_MAP_COMPONENT_ID;
  notifyFunctionList.notifySave      = mcastMapSave;
  notifyFunctionList.hasDataChanged  = mcastMapHasDataChanged;
  notifyFunctionList.notifyConfigDump = L7_NULLPTR;
  notifyFunctionList.notifyDebugDump = L7_NULLPTR;
  notifyFunctionList.resetDataChanged = mcastMapResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Error registering with NVStore");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }

  /**********************************************/
  /* Register Mcast to receive a callback when routing is enabled*/
  /**********************************************/

  if (ipMapRegisterRoutingEventChange(L7_IPRT_MCAST,
                       "mcastMapRoutingEventChangeCallBack",
                       mcastMapRoutingChangeCallback) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: EFailed to register for RouteEvent change callback.\n");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }

  if (mcastMapV6Register(L7_IPRT_MCAST,
                         "mcastMapRouting6EventChangeCallBack",
                         mcastMapRouting6ChangeCallback) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: EFailed to register for RouteEvent change callback.\n");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }
#if 0
  /**********************************************/
  /* Register Mcast to receive a callback when vlan changes  */
  /**********************************************/
  /* Currently there is no underlying hapi code that makes use of these notification.
   * to avoid unneccessary processing we have commented this section out.
   * If and when the need arises for these notification the if def can be removed.
   */
  eventMask = VLAN_ADD_PORT_NOTIFY | VLAN_DELETE_PORT_NOTIFY | VLAN_START_TAGGING_PORT_NOTIFY | VLAN_STOP_TAGGING_PORT_NOTIFY;
  if (vlanRegisterForChange(mcastMapVlanChangeCallback,
                            L7_FLEX_MCAST_MAP_COMPONENT_ID, eventMask) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to register for VLAN change callback.\n");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }
#endif
  /**************************************/
  /* Register with sysnet to receive multicast  packets */
  /**************************************/
  sysnetPduIntercept.addressFamily = L7_AF_INET;
  sysnetPduIntercept.hookId = SYSNET_INET_IN;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_MCAST_PRECEDENCE;
  sysnetPduIntercept.interceptFunc = mcastMapIntercept;
  strcpy(sysnetPduIntercept.interceptFuncName, "mcastMapIntercept");
  if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to register inetercept with sysnet "
             " to receive multicast packets\n");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }

#ifdef L7_NSF_PACKAGE
  if (mcastCkptCallbacksRegister() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_MCAST_MAP_COMPONENT_ID,
           "IP Multicast failed to register for checkpoint service callbacks.");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }
#endif

  /**************************************/
  /* Initialize the Phase 2 vendor code */
  /**************************************/


  if(mfcCnfgrPhase2Init() != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to initialize MFC Phase 2 \n");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }

  /* Register with support*/
  (void)mcastDebugRegister();
  mcastMapDebugMulticastInfoDumpRegister();

  mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_INIT_2;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes mcastpCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mcastCnfgrFiniPhase2Process()
{
   sysnetPduIntercept_t sysnetPduIntercept;

  (void)vlanRegisterForChange(L7_NULLPTR, L7_FLEX_MCAST_MAP_COMPONENT_ID,L7_NULL);
  (void)ipMapDeregisterRoutingEventChange(L7_IPRT_MCAST);
  (void)mcastMapV6Deregister(L7_IPRT_MCAST);
  (void)nimDeRegisterIntfChange(L7_FLEX_MCAST_MAP_COMPONENT_ID);
  (void)nvStoreDeregister(L7_FLEX_MCAST_MAP_COMPONENT_ID);

  /* Deregister with sysnet */
  sysnetPduIntercept.addressFamily = L7_AF_INET;
  sysnetPduIntercept.hookId = SYSNET_INET_IN;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_MCAST_PRECEDENCE;
  (void)sysNetPduInterceptDeregister(&sysnetPduIntercept);

  mfcCnfgrPhase2InitUndo();
 (void)mcastDebugDeRegister();
  mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_INIT_1;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   @b{(output)}  Reason.
*
* @returns  L7_SUCCESS There were no errors. Response is available.
* @returns  L7_FAILURE There were errors. Reason code is available.
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*           The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*           This function runs in the configurator's thread.
*           This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_RC_t mcastCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t *pReason )
{
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  /*******************************************************
   * Initialize the Phase 3 vendor code and the hardware *
   *******************************************************/
  if(mfcCnfgrPhase3Init() != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to initialize MFC Phase 3 \n");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }

  mcastMapBuildDefaultConfigData(L7_MCAST_CFG_VER_CURRENT);

  if (mcastMapApplyConfigData() != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to apply config data \n");
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }
  mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged = L7_FALSE;

  mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_INIT_3;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function undoes mcastCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mcastCnfgrFiniPhase3Process()
{
  mfcCnfgrPhase3InitUndo();

  if (mcastMapRestore() == L7_SUCCESS)
  {
    mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_WMU;
  }

  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_FAILURE - There were failures. Reason code is available.
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*           The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t mcastCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  if (mcastMapRestore() != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: mcastMapRestore Failed.\n");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }
  else
  {
    mcastGblVariables_g.mcastCnfgrState = MCAST_PHASE_WMU;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse @b{(output)}  Response always command complete.
* @param    pReason   @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*           The following are valid error reason code:
*           None.
*           This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t mcastCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason )
{
  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return L7_SUCCESS;
}

