/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  auto_install_cnfgr.c
*
* @purpose   auto_install configurator API implementation
*
* @component auto_install
*
* @comments
*
* @create    28/12/2007
*
* @author    lkhedyk
*
* @end
*
**********************************************************************/

#include <string.h>
#include "log.h"
#include "auto_install_cnfgr.h"
#include "auto_install_control.h"
#include "auto_install_debug.h"
#include "sysapi.h"
#include "l7netapi.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"

#define AUTO_INSTALL_MSG_COUNT 32

/* current auto-install configuration phase */
autoInstallCnfgrState_t autoInstallCnfgrState  = AUTO_INSTALL_PHASE_INIT_0;

/* auto_istall message queue ID */
void * autoInstallMsgQueue = L7_NULLPTR;

/* auto_istall task queue ID */
L7_int32 autoInstallTaskId = 0;

/* auto-install run time variables/parameters */
extern autoInstallRunTimeData_t *autoInstallRunTimeData;

/* auto-install configuration lock */
void  *autoInstallCfgSemaphore = L7_NULLPTR;


/* forward declarations */
L7_RC_t autoInstallInit (L7_CNFGR_CMD_DATA_t *pCmdData);
void    autoInstallInitUndo ();
L7_RC_t autoInstallCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason );
/* for debug purposes */
extern L7_uint32 dhcpcDebugMsgLvlSet(L7_uint32 level);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for auto-install component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the isdp comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void autoInstallApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  autoInstallMsg_t msg;

  memset(&msg, 0x00, sizeof(msg));

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
    if (autoInstallInit(pCmdData) != L7_SUCCESS)
    {
      autoInstallInitUndo();
    }
  }
  else
  {
    memcpy(&msg.msgData.cnfgrData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.event = AUTO_INSTALL_CNFGR_EVENT;
    
    if (osapiMessageSend(autoInstallMsgQueue,
                         &msg,
                         sizeof(autoInstallMsg_t),
                         L7_NO_WAIT,
                         L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      LOG_MSG("autoInstallApiCnfgrCommand(): message send failed: event = %d\n", msg.event);
    }
  }
}

/*********************************************************************
*
* @purpose  System Initialization for auto_install
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
L7_RC_t autoInstallInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CB_DATA_t cbData;
  autoInstallMsg_t   msg;

  memset(&cbData, 0x00, sizeof(L7_CNFGR_CB_DATA_t));
  memset(&msg,    0x00, sizeof(autoInstallMsg_t));

#ifdef AUTO_INSTALL_DEBUG
  autoInstallDebugTraceSet(AUTO_INSTALL_DBG_FLAG_ALL_ENTRIES);
  dhcpcDebugMsgLvlSet(0xFF);
#endif

  /* create auto-install configuration lock */
  autoInstallCfgSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (autoInstallCfgSemaphore == L7_NULLPTR)
  {
    LOG_MSG("autoInstallInit: autoInstallCfgSemaphore creation error.\n");
    cbData.correlator             = pCmdData->correlator;
    cbData.asyncResponse.rc       = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }
  
  /* create auto-install task queue*/
  autoInstallMsgQueue = (void*)osapiMsgQueueCreate("autoInstallMsgQueue",
                                                   AUTO_INSTALL_MSG_COUNT,
                                                   sizeof(autoInstallMsg_t));

  if (autoInstallMsgQueue == L7_NULLPTR)
  {
    LOG_MSG("autoInstallInit: autoInstallMsgQueue creation error.\n");
    cbData.correlator             = pCmdData->correlator;
    cbData.asyncResponse.rc       = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }
  
  /* start auto-istall task */
  autoInstallTaskId = osapiTaskCreate("autoInstTask",
                                      (void *)autoInstallTask,
                                      0,
                                      0,
                                      AUTO_INSTALL_STACK_SIZE,
                                      L7_DEFAULT_TASK_PRIORITY,
                                      L7_DEFAULT_TASK_SLICE);
  if (autoInstallTaskId == L7_ERROR)
  {
    LOG_MSG("autoInstallInit: task creation error.\n");
    cbData.correlator             = pCmdData->correlator;
    cbData.asyncResponse.rc       = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  /* wait for task initialization */
  if (osapiWaitForTaskInit(L7_AUTO_INSTALL_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("autoInstallInit: Unable to initialize autoInstallTask()\n");
    cbData.correlator             = pCmdData->correlator;
    cbData.asyncResponse.rc       = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }
    
  /* send Configurator's message to the task*/
  memcpy(&msg.msgData.cnfgrData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.event = AUTO_INSTALL_CNFGR_EVENT;

  if (osapiMessageSend(autoInstallMsgQueue,
                       &msg,
                       sizeof(autoInstallMsg_t),
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("autoInstallInit: message send failed: event = %d\n", msg.event);
    return L7_FAILURE;
  }
  
  return L7_SUCCESS;
  
}

/*********************************************************************
*
* @purpose  System Init Undo for auto_install
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void autoInstallInitUndo()
{
  /* deleting task*/
  if (autoInstallTaskId != 0)
  {
    osapiTaskDelete(autoInstallTaskId);
  }
  
  /* deleting queue */
  if (autoInstallMsgQueue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(autoInstallMsgQueue);
  }
  
  autoInstallCnfgrState = AUTO_INSTALL_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function process the configurator control
*            commands/request pair Init Phase 1.
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
L7_RC_t autoInstallCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                           L7_CNFGR_ERR_RC_t   *pReason )
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  
  /* allocate memory for auto-install run time parameters */
  autoInstallRunTimeData = (autoInstallRunTimeData_t *)osapiMalloc(L7_AUTO_INSTALL_COMPONENT_ID,
                                                     (L7_uint32)sizeof(autoInstallRunTimeData_t));
  if (autoInstallRunTimeData == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    
    autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_CnfgrGroup,
                          "%s (%d): lack of resourses\n",
                          __FUNCTION__,
                          __LINE__);
    return L7_ERROR;
  }

  /* memset to initialize all data structures to zero */
  memset(autoInstallRunTimeData, 0, sizeof(autoInstallRunTimeData_t));

  autoInstallCnfgrState = AUTO_INSTALL_PHASE_INIT_1;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control 
*           commands/request pair Init Phase 2.
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
L7_RC_t autoInstallCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                           L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_uint32 requestedDhcpOptions;

  /*  The following are the requested options from DHCP client.
  BOOTP_DHCP_IP_ADDR  = 1,
  BOOTP_DHCP_NETMASK  = 2,
  BOOTP_DHCP_GATEWAY  = 4,
  BOOTP_DHCP_SIADDR   = 8,
  BOOTP_DHCP_BOOTFILE = 16,
  BOOTP_DHCP_SNAME    = 32,
  BOOTP_DHCP_OPTION_TFTP_SVR_ADDR  = 64,
  BOOTP_DHCP_OPTION_DNS_SVR_ADDR   = 128,
  BOOTP_DHCP_OPTION_BOOTFILE       = 256,
  BOOTP_DHCP_OPTION_TFTP_SVR_NAME  = 512,
  */

  requestedDhcpOptions = 0x3FF; /* This is ORed value of all the above options.*/

  /* register callback with DHCP to get notification for obtaining configuration parameters */
  if(dhcpNotificationRegister( L7_AUTO_INSTALL_COMPONENT_ID, requestedDhcpOptions,
                               autoInstallNotificationCallback ) != L7_SUCCESS)
  {
    LOG_MSG("autoInstallCnfgrInitPhase2Process: Unable to register for dhcp callback\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;

    return L7_ERROR;
  }

  /* register callback with BOOTP to get notification for obtaining configuration parameters */
  if(bootpNotificationRegister( L7_AUTO_INSTALL_COMPONENT_ID, autoInstallNotificationCallback ) != L7_SUCCESS)
  {
    LOG_MSG("autoInstallCnfgrInitPhase2Process: Unable to register for bootp callback\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;

    return L7_ERROR;
  }

#if 0
  /* register a callback for TFTP transfer status */
  if(transferStatusNotificationRegister(L7_FILE_TYPE_AUTO_INSTALL_SCRIPT,
                                        autoInstallTransferStatusNotificationHandler ) != L7_SUCCESS)
  {
    LOG_MSG("autoInstallCnfgrInitPhase2Process: Unable to register for TFTP callback\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;

    return L7_ERROR;
  }
#endif

  /* Bind state handler for each state */
  autoInstallBindStateHandlers();
    
  /* Bind event handler for each state */
  autoInstallBindEventHandlers();

  autoInstallCnfgrState = AUTO_INSTALL_PHASE_INIT_2;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control 
*           commands/request pair Init Phase 3.
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
L7_RC_t autoInstallCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                           L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_uint8 i  = L7_NULL;
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* init run time parameters */
  autoInstallRunTimeData->currentState               = AUTO_INSTALL_START_STATE;
  autoInstallRunTimeData->specificFileRetryCounter   = L7_NULL;
  autoInstallRunTimeData->predefinedFileRetryCounter = L7_NULL;
  autoInstallRunTimeData->startStop                  = FD_AUTO_INSTALL_START_STOP_DEFAULT;
  autoInstallRunTimeData->autoSave                   = FD_AUTO_INSTALL_AUTO_SAVE_DEFAULT;
  autoInstallRunTimeData->unicastRetryCnt            = FD_AUTO_INSTALL_UNICAST_RETRY_COUNT_DEFAULT;
  memset(autoInstallRunTimeData->bootFileName, 0, BOOTP_FILE_LEN + 1);

  for (i = 0; i < AUTO_INSTALL_MAX_EVENT; i++)
  {
    autoInstallRunTimeData->currentEvents[i] = L7_FALSE;
  }

  autoInstallCnfgrState = AUTO_INSTALL_PHASE_INIT_3;
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes autoInstallCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void autoInstallCnfgrFiniPhase1Process()
{
  /* free memory used by auto-install runtime parameters */
  if (autoInstallRunTimeData != L7_NULLPTR)
  {
    osapiFree(L7_AUTO_INSTALL_COMPONENT_ID, (void *)autoInstallRunTimeData);
    autoInstallRunTimeData = L7_NULLPTR;
  }

  autoInstallInitUndo();

  autoInstallCnfgrState = AUTO_INSTALL_PHASE_INIT_0;

  return;
}

/*********************************************************************
* @purpose  This function undoes autoInstallCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void autoInstallCnfgrFiniPhase2Process()
{
  dhcpNotificationDeregister(L7_AUTO_INSTALL_COMPONENT_ID);
  
  autoInstallCnfgrState = AUTO_INSTALL_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes autoInstallCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void autoInstallCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response = L7_CNFGR_CMD_COMPLETE;
  L7_CNFGR_ERR_RC_t   reason   = 0;
  
  autoInstallCnfgrUconfigPhase2(&response, &reason);
}

/*********************************************************************
* @purpose  This function process the configurator control 
*           commands/request pair Unconfigure Phase 2.
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

L7_RC_t autoInstallCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* clean up component's configuration */
  autoInstallRunTimeData->autoSave = FD_AUTO_INSTALL_START_STOP_DEFAULT;
  autoInstallRunTimeData->startStop = FD_AUTO_INSTALL_AUTO_SAVE_DEFAULT;
  autoInstallRunTimeData->unicastRetryCnt = FD_AUTO_INSTALL_UNICAST_RETRY_COUNT_DEFAULT;

  memset(&(autoInstallRunTimeData->bootConfigParams), 0,
         sizeof(autoInstallRunTimeData->bootConfigParams));

  autoInstallCnfgrState = AUTO_INSTALL_PHASE_WMU;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control
*           commands/request pair as a NOOP.
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
L7_RC_t autoInstallCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To parse the configurator commands sent to autoInstallTask
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void autoInstallCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_RC_t               rc         = L7_ERROR;
  L7_CNFGR_ERR_RC_t     reason     = L7_CNFGR_ERR_RC_INVALID_PAIR;

  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  L7_CNFGR_CMD_t        command    = L7_CNFGR_CMD_FIRST;
  L7_CNFGR_RQST_t       request    = L7_CNFGR_RQST_FIRST;
  L7_CNFGR_RESPONSE_t   response   = L7_CNFGR_CMD_COMPLETE;

  L7_CNFGR_CB_DATA_t    cbData;
  
  memset(&cbData, 0x00, sizeof(L7_CNFGR_CB_DATA_t));

  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      correlator = pCmdData->correlator;
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      
      if ( ( request > L7_CNFGR_RQST_FIRST ) && ( request < L7_CNFGR_RQST_LAST ) )
      {
        /* validate command/event pair */
        switch ( command )
        {
          case L7_CNFGR_CMD_INITIALIZE:
          {
            switch ( request )
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((rc = autoInstallCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  autoInstallCnfgrFiniPhase1Process();
                }
                break;
                
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((rc = autoInstallCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  autoInstallCnfgrFiniPhase2Process();
                }
                break;
                
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((rc = autoInstallCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  autoInstallCnfgrFiniPhase3Process();
                }
                break;
                
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rc = autoInstallCnfgrNoopProccess( &response, &reason );
                autoInstallCnfgrState = AUTO_INSTALL_PHASE_WMU;
                break;
                
              default:
                /* invalid command/request pair */
                break;
            } /* switch ( request ) */
          } /* case L7_CNFGR_CMD_INITIALIZE: */
          break;

          case L7_CNFGR_CMD_EXECUTE:
          {
            switch ( request )
            {
              case L7_CNFGR_RQST_E_START:
                autoInstallCnfgrState = AUTO_INSTALL_PHASE_EXECUTE;
                rc                    = L7_SUCCESS;
                response              = L7_CNFGR_CMD_COMPLETE;
                reason                = 0;
                /* Notify the state machine */
                autoInstallStateMachineEventProcess(AUTO_INSTALL_EXECUTION_EVENT);
                break;

              default:
                /* invalid command/request pair */
                break;
            }
          } /* case L7_CNFGR_CMD_EXECUTE */
          break;

          case L7_CNFGR_CMD_UNCONFIGURE:
          {
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                rc = autoInstallCnfgrNoopProccess( &response, &reason );
                autoInstallCnfgrState = AUTO_INSTALL_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                rc = autoInstallCnfgrUconfigPhase2( &response, &reason );
                autoInstallCnfgrState = AUTO_INSTALL_PHASE_UNCONFIG_2;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
          } /* case L7_CNFGR_CMD_UNCONFIGURE: */
          break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rc = autoInstallCnfgrNoopProccess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */
      } 
      else
      {
        /* Configurator's request is out of range */
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      } /* endif validate request */
    }
    else
    {
      /* Invalid command type*/
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
    } /* endif validate command type */
  }
  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;
  } /* check for command valid pointer */

  /* return value to caller -
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = rc;
  if (rc == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}




