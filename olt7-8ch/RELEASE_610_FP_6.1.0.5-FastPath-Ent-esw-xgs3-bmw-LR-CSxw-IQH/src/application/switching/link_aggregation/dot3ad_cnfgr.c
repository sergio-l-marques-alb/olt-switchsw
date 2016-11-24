
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3ad_cnfgr.c
*
* @purpose Provide the Configurator hooks for Initialization and Teardown
*
* @component dot3ad
*
* @comments
*
* @create 10/15/2003
*
* @author 
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_resources.h"
#include "nimapi.h"
#include "sysapi.h"
#include "platform_config.h"
#include "l7_cnfgr_api.h"
#include "dot3ad_include.h"
#include "dot3ad_sid.h"
#include "nvstoreapi.h"
#include "simapi.h"
#include "buff_api.h"

/*
 * Local prototypes
 */
L7_RC_t dot3adCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot3adCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot3adCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot3adCnfgrInitPhaseExecuteProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot3adCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot3adCnfgrUnconfigureProcess1( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason,
                                        L7_CNFGR_CORRELATOR_t correlator,
                                        L7_BOOL *performCallback);

L7_RC_t dot3adCnfgrUnconfigureProcess2( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason );

void dot3adNimUnconfigEventCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal);

L7_RC_t   dot3adPhaseOneInit(void);
void      dot3adPhaseOneFini(void);
L7_RC_t   dot3adPhaseTwoInit(void);
void      dot3adPhaseTwoFini(void);
L7_RC_t   dot3adPhaseThreeInit(void);
void      dot3adPhaseThreeFini(void);
L7_RC_t   dot3adPhaseExecuteInit(void);

/* The last phase that was completed */
static L7_CNFGR_STATE_t dot3adState = L7_CNFGR_STATE_NULL;

static L7_uint32 dot3ad_lac_task_id;
static L7_uint32 dot3ad_timer_task_id;
static NIM_NOTIFY_CB_INFO_t dot3adLastIntfDeleted;
static L7_CNFGR_CORRELATOR_t dot3adUnconfigureInProcess = L7_CNFGR_NO_HANDLE;

/* Global data */
extern void * dot3adTimerSyncSema;
extern void * dot3adTaskSyncSema;
extern void * dot3adAggIntfCreateSema;
extern void * dot3adCreateSyncSema;
/* The static mode has to be processed by the dot3ad queue
   before trying to add lag members based on this mode.
   This empty sync semaphore will block the caller on static mode
   set for the lag until it is processed by dot3ad queue.
*/

void * dot3adQueueSyncSema = L7_NULLPTR;

extern dot3adCfg_t dot3adCfg;    /* Configuration File Overlay */
extern L7_uint32   dot3adPortIdx[L7_MAX_PORT_COUNT + 1];
extern dot3ad_agg_t    dot3adAgg[L7_MAX_NUM_LAG_INTF];
extern L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT];
extern dot3ad_stats_t  dot3ad_stats[L7_MAX_INTERFACE_COUNT];
extern void *dot3ad_queue;  /* reference to the LAC message queue */
extern dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT + 1];
extern dot3ad_system_t dot3adSystem;
extern dot3ad_LagCnt_t dot3adLagCnt;
L7_uint32 dot3adBufferPoolId = 0;
extern dot3adDebugCfg_t dot3adDebugCfg;    /* Debug Configuration File Overlay */

extern void dot3adDebugRegister(void);
extern void dot3adDebugCfgRead(void);
extern dot3adOperPort_t dot3adOperPort[L7_MAX_PORT_COUNT + 1];

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.       
*
* @returns  None
*
* @notes    This function completes synchronously. The return value 
*           is presented to the configurator by calling the cnfgrApiCallback().
*           The following are the possible return codes:   
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
void dot3adApiCnfgrCommandProcess( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  L7_BOOL   performCallback = L7_TRUE;

  L7_RC_t             rc    = L7_ERROR;
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
                if ((rc = dot3adCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  dot3adPhaseOneFini();
                }
                else
                {
                  dot3adState = L7_CNFGR_STATE_P1;
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((rc = dot3adCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  dot3adPhaseTwoFini();
                }
                else
                {
                  dot3adState = L7_CNFGR_STATE_P2;
                }
                break;

              case L7_CNFGR_RQST_I_PHASE3_START:  
                if ((rc = dot3adCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  dot3adPhaseThreeFini();
                }
                else
                {
                  dot3adState = L7_CNFGR_STATE_P3;
                }
                break;

              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rc = dot3adCnfgrNoopProccess( &response, &reason );
                dot3adState = L7_CNFGR_STATE_WMU;
                break;

              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            rc = dot3adCnfgrInitPhaseExecuteProcess( &response, &reason );
            dot3adState = L7_CNFGR_STATE_E;
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:

            if ( request == L7_CNFGR_RQST_U_PHASE1_START )
            {
              cbData.correlator       = correlator;
              rc = dot3adCnfgrUnconfigureProcess1(&response,&reason,correlator,&performCallback);
            }
            if ( request == L7_CNFGR_RQST_U_PHASE2_START )
            {
              if ((rc = dot3adCnfgrUnconfigureProcess2(&response,&reason)) == L7_SUCCESS)
              {
                dot3adState = L7_CNFGR_STATE_U2;
              }
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rc = dot3adCnfgrNoopProccess( &response, &reason );
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
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;

  } /* check for command valid pointer */

  /* return value to caller - 
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = rc;

  if (rc == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason   = reason;
  }

  if (performCallback == L7_TRUE)
  {
    cnfgrApiCallback(&cbData);
  }

  return;
}
/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.       
*
* @returns  None
*
* @notes    This function completes synchronously. The return value 
*           is presented to the configurator by calling the cnfgrApiCallback().
*           The following are the possible return codes:   
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
void dot3adApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator;
  dot3adMsg_t           msg;

  L7_RC_t             rc   ;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;

      if ((command == L7_CNFGR_CMD_INITIALIZE) &&
          (request == L7_CNFGR_RQST_I_PHASE1_START))
      {

        if ((rc = dot3adCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
        {
          dot3adPhaseOneFini();
        }
        else
        {
          dot3adState = L7_CNFGR_STATE_P1;
        }

        /* return value to caller - 
         * <prepare completion response>
         * <callback the configurator>
         */
        cbData.correlator       = correlator;
        cbData.asyncResponse.rc = rc;

        if (rc == L7_SUCCESS)
        {
          cbData.asyncResponse.u.response = response;
        }
        else
        {
          cbData.asyncResponse.u.reason   = reason;
        }

        cnfgrApiCallback(&cbData);
      } /* PHASE 1 Start */
      else
      {
        /* 
         * all events other than PHASE 1 are handled asyncronously 
         * Assumes that PHASE 1 has already happened or the message will not be 
         * sent.
         */
        msg.event = lacCnfgrChange;
        msg.intf  = 0;
        memcpy(&msg.intfData.cnfgrInfo,pCmdData,sizeof(L7_CNFGR_CMD_DATA_t));

        if (osapiMessageSend(dot3ad_queue, &msg, (L7_uint32)DOT3AD_MSG_SIZE, 
                             L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
        {
          sysapiPrintf("DOT3AD: error sending message for cnfgr\n");
          LOG_ERROR(666);
        }
      }

    }
  }


  return;
}

/*********************************************************************
* @purpose  This function returns the phase that dot3ad believes it has completed
*
* @param    void
*
* @returns  L7_CNFGR_RQST_t - the last phase completed
*
* @notes    Until dot3ad has completed phase 1, the value will be L7_CNFGR_RQST_FIRST
*
* @end
*********************************************************************/
L7_CNFGR_STATE_t dot3adCnfgrPhaseGet(void)
{
  return(dot3adState);
}

/*
 *********************************************************************
 *             dot3ad cnfgr Internal Function Calls
 *********************************************************************
*/ 

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
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t dot3adCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc;

  rc = dot3adPhaseOneInit();

  if (rc != L7_SUCCESS)
  {
    rc   = L7_ERROR;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  = 0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(rc);
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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t dot3adCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc;

  rc = dot3adPhaseTwoInit();

  if (rc != L7_SUCCESS)
  {
    rc = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(rc);
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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t dot3adCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc ;

  rc = dot3adPhaseThreeInit();

  if (rc != L7_SUCCESS)
  {
    rc = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(rc);
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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t dot3adCnfgrInitPhaseExecuteProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc ;

  rc = dot3adPhaseExecuteInit();

  if (rc != L7_SUCCESS)
  {
    rc = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(rc);
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
L7_RC_t dot3adCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(rc);
}

/*********************************************************************
* @purpose  Transitions the dot3ad to configurator state WMU from PHASE 3
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t dot3adCnfgrUnconfigureProcess1( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason,
                                        L7_CNFGR_CORRELATOR_t correlator,
                                        L7_BOOL *performCallback)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t handle;
  L7_uint32 maxLag,i; 

  maxLag = platIntfLagIntfMaxCountGet();
  *performCallback = L7_TRUE;

  memset((void *)&dot3adLastIntfDeleted, 0, sizeof(NIM_NOTIFY_CB_INFO_t) );
  memset((void *)&eventInfo, 0, sizeof(eventInfo) );
  dot3adUnconfigureInProcess  = correlator;

  /* we must delete the LAG interfaces */
  for (i = 0; i < maxLag ; i++)
  {
    if (dot3adAgg[i].inuse)
    {
      /* if we deconfigure at least one interface, don't callback cnfgr */
      *performCallback = L7_FALSE;

      /* successful event notification */
      eventInfo.component = L7_DOT3AD_COMPONENT_ID;
      eventInfo.intIfNum  = dot3adAgg[i].aggId;
      eventInfo.pCbFunc   = dot3adNimUnconfigEventCompletionCallback;
      eventInfo.event     = L7_DETACH;

      if ((rc = nimEventIntfNotify(eventInfo,&handle)) != L7_SUCCESS)
      {
        /* error path, tell the configurator */
        *performCallback = L7_TRUE;

        /* reset the data for the callback */
        memset((void *)&dot3adLastIntfDeleted, 0, sizeof(NIM_NOTIFY_CB_INFO_t) );
        dot3adUnconfigureInProcess  = 0;

        break;
      }
    }
  }

  if (*performCallback == L7_FALSE)
  {
    /* we must of had a successful notification */
    /* save the last intf deleted during the unconfigure2 */
    dot3adLastIntfDeleted.event     = eventInfo.event;
    dot3adLastIntfDeleted.handle    = handle;
    dot3adLastIntfDeleted.intIfNum  = eventInfo.intIfNum;
  }

  /* Return Value to caller */
  /* The callback to CNFGR will not happen until the deletes are complete */
  if (rc != L7_SUCCESS)
  {
    rc   = L7_ERROR;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  = 0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(rc);
}

/*********************************************************************
* @purpose  Transitions the dot3ad to configurator state WMU from PHASE 3
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t dot3adCnfgrUnconfigureProcess2( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  dot3adPhaseThreeFini();

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(rc);
}


/*********************************************************************
* @purpose  phase 1 to Initialize dot3ad
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t   dot3adPhaseOneInit(void)
{
  L7_RC_t rc ;

  /* initialize the global data to zero */
  memset((void*)&dot3ad_stats,0, sizeof(dot3ad_stats_t));
  /* clear operational data struture to keep track of number of lags in the system*/
  memset((void *)&dot3adLagCnt,0,sizeof(dot3ad_LagCnt_t));
  memset((void*)&dot3adAgg, 0 , L7_MAX_NUM_LAG_INTF * sizeof(dot3ad_agg_t));    
  memset((void*)&dot3adPort, 0 , sizeof(dot3ad_port_t) * (L7_MAX_PORT_COUNT + 1));
  memset((void*)&dot3adSystem, 0, sizeof(dot3ad_system_t)); 
  memset((void*)&dot3ad_stats, 0 , sizeof(dot3ad_stats_t) * L7_MAX_INTERFACE_COUNT);
  memset((void*)&dot3adAggIdx, 0 , sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);  
  memset((void*)&dot3adPortIdx, 0 , sizeof(L7_uint32) *  (L7_MAX_PORT_COUNT+1));
  memset((void*)&dot3adOperPort,  0,  sizeof(dot3adOperPort_t) * (L7_MAX_PORT_COUNT + 1));

  /* Initialize buffer pool for incoming PDUs*/
  if (bufferPoolInit(DOT3AD_NUM_BUFFERS, DOT3AD_BUFFER_SIZE, "Dot3ad Bufs",
                     &dot3adBufferPoolId) != L7_SUCCESS)
  {
    LOG_MSG("dot3adPhaseOneInit: Error allocating buffers\n");
    return L7_FAILURE;
  }
  
  /* create the semaphores */
  /*semaphore creation for task protection over the common data*/
  dot3adTaskSyncSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if (dot3adTaskSyncSema == L7_NULL)
  {
    LOG_MSG("Unable to create dot3ad task semaphore()\n");
    return(L7_FAILURE);
  }

  /*semaphore creation for timer protection*/
  dot3adTimerSyncSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dot3adTimerSyncSema == L7_NULL)
  {
    LOG_MSG("Unable to create dot3ad timer semaphore()\n");
    return(L7_FAILURE);
  }

  /*semaphore creation for create protection*/
  dot3adAggIntfCreateSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dot3adAggIntfCreateSema == L7_NULL)
  {
    LOG_MSG("Unable to create dot3ad Intf create semaphore()\n");
    return(L7_FAILURE);
  }

  dot3adCreateSyncSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);

  if (dot3adCreateSyncSema == L7_NULL)
  {
	LOG_MSG("Unable to create dot3ad task semaphore()\n");
	return(L7_FAILURE);
  }

  dot3adQueueSyncSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);

  if (dot3adQueueSyncSema == L7_NULL)
  {
    LOG_MSG("Unable to create dot3adQueueSync semaphore()\n");
    return(L7_FAILURE);
  }



  /* create the tasks */
  /* create dot3ad_lac_task - to service queue*/
  dot3ad_timer_task_id = (L7_uint32)osapiTaskCreate( "dot3ad_timer_task", (void *)dot3ad_timer_task, 0, 0,
                                                     dot3adSidTimerTaskStackSizeGet(), 
                                                     dot3adSidTimerTaskPriorityGet(),
                                                     dot3adSidTimerTaskSliceGet());

  if (osapiWaitForTaskInit (L7_DOT3AD_TIMER_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Unable to initialize dot3ad_timer_task()\n");
    return(L7_FAILURE);                                                   
  }

  dot3ad_lac_task_id = (L7_uint32)osapiTaskCreate( "dot3ad_lac_task", (void *)dot3ad_lac_task, 0, 0,
                                                   dot3adSidLacTaskStackSizeGet(), 
                                                   dot3adSidLacTaskPriorityGet(),
                                                   dot3adSidLacTaskSliceGet());

  if (osapiWaitForTaskInit (L7_DOT3AD_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Unable to initialize dot3ad_lac_task()\n");
    return(L7_FAILURE);                                                   
  }

  if (dot3ad_lac_task_id == L7_ERROR || dot3ad_timer_task_id == L7_ERROR)
  {
    LOG_MSG("Failed to create dot3ad tasks.\n");
    return(L7_FAILURE);
  }

  /*trace create for debugging*/
  /*this trace has 256 entries, each entry is 16 byte long*/
  /*actual user data available is 15 bytes as the first byte records*/
  /*the trace id always*/

  if ((rc = traceBlockCreate( DOT3AD_TRACE_ENTRY_MAX , DOT3AD_TRACE_ENTRY_SIZE_MAX,
       (L7_uchar8 *)"dot3adTrace",&dot3adSystem.traceId)) != L7_SUCCESS)
  {
    LOG_MSG("DOT3AD: trace block not created\n");
    return(rc);
  }

  /*enable tracing*/
  if ((rc = traceBlockStart(dot3adSystem.traceId)) != L7_SUCCESS)
  {
    LOG_MSG("DOT3AD: trace block not enabled\n");
    return(rc);
  }

  /* Read and apply trace configuration at phase 1.
       This allows for tracing during system initialization and
       during clear config */
  dot3adDebugCfgRead();
  dot3adApplyDebugConfigData();


  return(L7_SUCCESS); 
}

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none. 
*       
* @end
*********************************************************************/
void dot3adPhaseOneFini(void)
{
  if (dot3adTaskSyncSema != L7_NULL)
    osapiSemaDelete(dot3adTaskSyncSema);

  if (dot3adTimerSyncSema != L7_NULL)
    osapiSemaDelete(dot3adTimerSyncSema);

  if (dot3ad_lac_task_id != L7_ERROR)
    osapiTaskDelete(dot3ad_lac_task_id);

  if (dot3ad_timer_task_id != L7_ERROR)
    osapiTaskDelete(dot3ad_timer_task_id);

  if (dot3adAggIntfCreateSema != L7_NULL)
    osapiSemaDelete(dot3adAggIntfCreateSema); 
  if (dot3adBufferPoolId != 0)
  {
    bufferPoolDelete(dot3adBufferPoolId);
    dot3adBufferPoolId = 0;
  }

  if (dot3adCreateSyncSema != L7_NULL)
	osapiSemaDelete(dot3adCreateSyncSema); 

  if (dot3adQueueSyncSema != L7_NULL)
	osapiSemaDelete(dot3adQueueSyncSema); 

  return;
}

/*********************************************************************
* @purpose  phase 2 to Initialize dot3ad component   
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t   dot3adPhaseTwoInit(void)
{
  L7_RC_t rc ;
  nvStoreFunctionList_t notifyFunctionList;

  memset((void *) &notifyFunctionList, 0, sizeof(nvStoreFunctionList_t));
  notifyFunctionList.hasDataChanged = dot3adHasDataChanged;
  notifyFunctionList.notifySave     = dot3adSave;
  notifyFunctionList.registrar_ID   = L7_DOT3AD_COMPONENT_ID;
  notifyFunctionList.resetDataChanged = dot3adResetDataChanged;

  if ((rc = nvStoreRegister(notifyFunctionList)) != L7_SUCCESS)
  {
    LOG_MSG("DOT3AD: Failed to register NV routines\n");
  }
  /* register callback with NIM for L7_UPs and L7_DOWNs */
  else if ((rc = nimRegisterIntfChange(L7_DOT3AD_COMPONENT_ID, dot3adIntfChangeCallBack)) != L7_SUCCESS)
  {
    LOG_MSG("DOT3AD: failed to register to NIM\n");
  }
  /* register for PDUs */
  else if ((rc = dot3adLacpmInit()) != L7_SUCCESS)
  {
    LOG_MSG("DOT3AD: failed to register for PDUs\n");
  }
  /* TODO: Add registration for debug save/restore */
  else
    rc = L7_SUCCESS;

  /* register for debug */
  dot3adDebugRegister();

  return(rc);
}

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 2
*
* @param    void
*
* @returns  void
*
* @notes    none. 
*       
* @end
*********************************************************************/
void dot3adPhaseTwoFini(void)
{
  /* We don't want interface changes anymore */
  nimDeRegisterIntfChange(L7_DOT3AD_COMPONENT_ID); 

  /* need a mechanism to de-register for PDUs */

  return;
}

/*********************************************************************
* @purpose  phase 3 to Initialize dot3ad component   
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Read the configuration during phase 3
*       
* @end
*********************************************************************/
L7_RC_t   dot3adPhaseThreeInit(void)
{
  L7_RC_t rc ;

  /* initialize the global interface data to zero */
  memset((void*)&dot3adAgg, 0 , L7_MAX_NUM_LAG_INTF * sizeof(dot3ad_agg_t));    
  memset((void*)&dot3adPort, 0 , sizeof(dot3ad_port_t) * (L7_MAX_PORT_COUNT + 1));
  memset((void*)&dot3ad_stats, 0 , sizeof(dot3ad_stats_t) * L7_MAX_INTERFACE_COUNT);
  memset((void*)&dot3adAggIdx, 0 , sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);  
  memset((void*)&dot3adPortIdx, 0 , sizeof(L7_uint32) *  (L7_MAX_PORT_COUNT+1));
  memset((void*)&dot3adOperPort,  0,  sizeof(dot3adOperPort_t) * (L7_MAX_PORT_COUNT + 1));
  
  /* reset the lag counters*/
  memset((void*)&dot3adLagCnt, 0 ,sizeof(dot3ad_LagCnt_t));

   rc = sysapiCfgFileGet(L7_DOT3AD_COMPONENT_ID, DOT3AD_CFG_FILENAME, 
                        (L7_char8 *)&dot3adCfg, (L7_uint32)sizeof(dot3adCfg_t), 
                        &dot3adCfg.checkSum, DOT3AD_CFG_VER_CURRENT, 
                        dot3adBuildConfigData, dot3adMigrateConfigData /* Need a convert */);

  dot3adCfg.hdr.dataChanged = L7_FALSE;

  return(rc);
}


/*********************************************************************
* @purpose  Remove all resources acquired during Phase 3
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void dot3adPhaseThreeFini(void)
{
  /* erase volatile copy of the config */
  memset((void *)&dot3adCfg,0,sizeof(dot3adCfg_t));
  return;
}

/*********************************************************************
* @purpose  phase Execute to Initialize dot3ad component   
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Read the configuration during phase 3
*       
* @end
*********************************************************************/
L7_RC_t   dot3adPhaseExecuteInit(void)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 maxLagIntf,index;
  L7_uint32 tmpIntIfNum;
  L7_uint32 lagId;

  do
  {

  maxLagIntf = platIntfLagIntfMaxCountGet();

  dot3adSystem.actorSysPriority = dot3adCfg.cfg.dot3adSystem.actorSysPriority;
  dot3adSystem.hashMode = dot3adCfg.cfg.dot3adSystem.hashMode;

  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
  {
    simGetSystemIPBurnedInMac((L7_uchar8 *)&(dot3adSystem.actorSys));
  }
  else
  {                                  
    simGetSystemIPLocalAdminMac((L7_uchar8 *)&(dot3adSystem.actorSys));
  }

  for (index = 0; index < maxLagIntf ; index++)
  {
    if (dot3adCfg.cfg.dot3adAgg[index].inuse)
    {

      lagId = index + 1;

      dot3adPrivateLagCreate(lagId, dot3adCfg.cfg.dot3adAgg[index].name,
                             dot3adCfg.cfg.dot3adAgg[index].aggWaitSelectedPortList,
                             dot3adCfg.cfg.dot3adAgg[index].adminMode,
                             L7_ENABLE,
                             L7_TRUE,
                             dot3adCfg.cfg.dot3adAgg[index].hashMode,
                             &tmpIntIfNum
                             );
	  /* Since cfgCreated is TRUE, the linkTrapState does not matter as this is already in the nim saved
	   * configuration. In the dot3adPrivateLagCreate it is ignored. There is no need to store link trap state in dot3ad.
	   */

      /* there is no need to explicitly set the static mode as it is handled within
         dot3adPrivateLagCreate . The configuration is copied into the operation structure 
         and the members are added later. Hence the members added will follow the same steps
         as the adding a port to an already static lag */
      }
    }

  } while ( 0 );

  dot3adCfg.hdr.dataChanged = L7_FALSE;

 

  return(rc);
}


/*********************************************************************
* @purpose  Receive the completion notification from NIM event notifications
*
* @param    intIfNum internal interface number  
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*            
*       
* @end
*********************************************************************/
void dot3adNimUnconfigEventCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal)
{
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t            handle;
  L7_CNFGR_CB_DATA_t    cbData;

  if (retVal.response.rc != L7_SUCCESS)
  {
    /* Failed to complete the request */
    LOG_MSG("DOT3AD: notification failed for event(%d), intf(%d), reason(%d)\n",
            retVal.event,retVal.intIfNum,retVal.response.reason);
  }
  else
  {
    /* successful event notification */
    eventInfo.component = L7_DOT3AD_COMPONENT_ID;
    eventInfo.intIfNum  = retVal.intIfNum;
    eventInfo.pCbFunc   = dot3adNimUnconfigEventCompletionCallback;

    switch (retVal.event)
    {
      case L7_DETACH:
        eventInfo.event     = L7_DELETE;
        nimEventIntfNotify(eventInfo,&handle); 

        if ((retVal.handle == dot3adLastIntfDeleted.handle) &&
            (retVal.intIfNum == dot3adLastIntfDeleted.intIfNum))
        {
          dot3adLastIntfDeleted.handle = handle;
          dot3adLastIntfDeleted.intIfNum = retVal.intIfNum;
          dot3adLastIntfDeleted.event = L7_DELETE;
        }
        break;

      case L7_DELETE:
        /* see if this is the last deleted interface in an unconfig */
        if ((dot3adLastIntfDeleted.handle == retVal.handle) &&
            (dot3adLastIntfDeleted.intIfNum == retVal.intIfNum))
        {
          cbData.correlator = dot3adUnconfigureInProcess;
          cbData.asyncResponse.rc = retVal.response.rc;
          cbData.asyncResponse.u.response =  L7_CNFGR_CMD_COMPLETE;

          dot3adState = L7_CNFGR_STATE_U1;

          cnfgrApiCallback(&cbData);
        }

      default:
        /* only care about create and detach for now */
        break;
    }
  }
}
