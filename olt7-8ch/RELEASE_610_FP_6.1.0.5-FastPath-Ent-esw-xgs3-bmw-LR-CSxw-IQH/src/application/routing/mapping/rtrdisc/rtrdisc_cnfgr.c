/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  rtrdisc_cnfgr.c
*
* @purpose   Implements the new stacking initialization sequence
*
* @component 
*
* @comments  The main configurator dictates what sequence of state changes
*            by using the apt command/request pair. The current initialization 
*            sequence for non-management units is: INIT_0, INIT_1, INIT_2 and WMU.
*            Management units have the sequence: INIT_0, INIT_1, INIT_2, WMU and INIT_3.
*            After the management unit (MU) has transitioned into INIT_3 the configurator 
*            issues a command to all units (MU and non-MU) to move into the EXECUTE phase.
*
*            When a "clear config" command is issued, the sequence of state changes
*            is: EXECUTE,UNCONFIG_1, UNCONFIG_2, WMU.
*            The MU then goes into INIT_3 and subsequently all units go into EXECUTE phase.
*
*            None the state changes are enforced by the DHCP relay component; the main 
*            configurator ensures this.
*
* @create    11/19/2001
*
* @author    ksdesai
*
* @end
*             
**********************************************************************/

/*************************************************************
        
*************************************************************/

#include "l7_common.h"
#include "l7_common_l3.h"
#include "l7_cnfgr_api.h"
#include "defaultconfig.h"
#include "nvstoreapi.h"
#include "sysnet_api_ipv4.h"
#include "nimapi.h"
#include "l7_ip_api.h"
#include "l3_default_cnfgr.h"
#include "rtrdisc.h"
#include "rtrdisc_config.h"
#include "rtrdisc_cnfgr.h"
#include "rtrdisc_util.h"
#include "l7_socket.h"


/* The current state of the unit. The main configurator maintains separate information
   about the component's current state. rtrDiscCnfgrState is used by the router discovery
   component alone to keep track of its current state. */
rtrDiscCnfgrState_t rtrDiscCnfgrState = RTR_DISC_PHASE_INIT_0;


extern rtrDiscCfgData_t *rtrDiscCfgData;
extern rtrDiscIntf_t *rtrDiscIntf;
extern L7_uint32 *rtrDiscMapTbl;
extern osapiTimerDescr_t *pRtrDiscTimerHolder;

extern osapiRWLock_t rtrDiscIntfRWLock;

extern rtrDiscInfo_t          *pRtrDiscInfo;
extern rtrDiscIntfInfo_t      *pRtrDiscIntfInfo;   

L7_RC_t rtrDiscCnfgrNoopPronDts( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t rtrDiscCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

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
* @notes    This function runs in the configurator's thread.
*
* @notes    Handles only Phase 1 initialization on the configurator thread. 
*           Enqueues all other command/request pairs for processing task to handle.
*       
* @end
*********************************************************************/
void rtrDiscApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
    
    /* set up variables and structures */
    L7_CNFGR_CMD_t        command;
    L7_CNFGR_RQST_t       request;
    L7_CNFGR_CB_DATA_t    cbData;
    L7_CNFGR_RESPONSE_t   response;
    L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

    L7_RC_t             rtrDiscRC = L7_ERROR;
    L7_CNFGR_ERR_RC_t   reason    = L7_CNFGR_ERR_RC_INVALID_PAIR;

    rtrDiscMsg_t        msg;

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
                if ( (command ==L7_CNFGR_CMD_INITIALIZE) && request == L7_CNFGR_RQST_I_PHASE1_START )
                {
                    if ((rtrDiscRC = rtrDiscCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                    {
                        rtrDiscCnfgrFiniPhase1Process();
                    }
                } else
                {
                    /*  Put a msg into the processing msgQ, created in phase 1. The task created in 
                    Phase 1 should handle all the cnfgr callbacks other than the one from phase 1 */
                    memcpy(&msg.type.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
                    msg.msgId = RTR_DISC_CNFGR;
                    osapiMessageSend(pRtrDiscInfo->rtrDiscProcessingQueue, &msg, sizeof(rtrDiscMsg_t),
                                     L7_NO_WAIT,L7_MSG_PRIORITY_NORM);
                    return; /* The processing task will take care of the reurning the value of 
                               the command when it is executed */

                }
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
    cbData.asyncResponse.rc = rtrDiscRC;
    if (rtrDiscRC == L7_SUCCESS)
        cbData.asyncResponse.u.response = response;
    else
        cbData.asyncResponse.u.reason   = reason;

    cnfgrApiCallback(&cbData);

    return;
}


/*********************************************************************
* @purpose  This function processes the configurator control commands/request
*           pair Init Phase 1. Init phase 1 mainly invloves allocation of memory,
*           creation of semaphores, message queues, tasks and other resources 
*           used by the protocol
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
* @notes    This function runs in the configurator's thread. 
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
    static const char *routine_name = "rtrDiscCnfgrInitPhase1Process()";
    L7_RC_t rtrDiscRC = L7_SUCCESS;

    rtrDiscIntf = L7_NULLPTR;

    pRtrDiscInfo = (rtrDiscInfo_t *)osapiMalloc(L7_RTR_DISC_COMPONENT_ID, sizeof(rtrDiscInfo_t));
    /* Malloc space for router discovery configuration data */
    rtrDiscCfgData = (rtrDiscCfgData_t *)osapiMalloc(L7_RTR_DISC_COMPONENT_ID, sizeof(rtrDiscCfgData_t));
    rtrDiscMapTbl  = osapiMalloc(L7_RTR_DISC_COMPONENT_ID, sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);
    pRtrDiscIntfInfo = osapiMalloc(L7_RTR_DISC_COMPONENT_ID, sizeof(rtrDiscIntfInfo_t) * L7_MAX_INTERFACE_COUNT);

    /* Make sure that allocation succeded */
    if ((rtrDiscCfgData == L7_NULL) || (rtrDiscMapTbl == L7_NULL) || 
        (pRtrDiscInfo == L7_NULLPTR) || (pRtrDiscIntfInfo == L7_NULLPTR))
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
        rtrDiscRC   = L7_ERROR;

        return rtrDiscRC;
    }
    memset(rtrDiscCfgData, 0, sizeof(rtrDiscCfgData_t));
    memset(rtrDiscMapTbl, 0, sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);
    memset(pRtrDiscIntfInfo, 0, sizeof(rtrDiscIntfInfo_t) * L7_MAX_INTERFACE_COUNT);

    /* Create a semaphore to protect the router discovery interface
       linked list
    */

    if (osapiRWLockCreate(&rtrDiscIntfRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
    {
        LOG_MSG("%s %d: %s: Unable to create semaphore for router discovery interface linked list\n",__FILE__, __LINE__,routine_name);
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        rtrDiscRC = L7_ERROR;

        return rtrDiscRC;
    } else
    {
        pRtrDiscInfo->rtrDiscProcessingTaskId = osapiTaskCreate("tRtrDiscProcessingTask", rtrDiscProcessingTask, 0, 0,
                                                                L7_DEFAULT_STACK_SIZE,
                                                                FD_CNFGR_SIM_DEFAULT_STATS_TASK_PRI,
                                                                L7_DEFAULT_TASK_SLICE);
        if (pRtrDiscInfo->rtrDiscProcessingTaskId == L7_ERROR)
        {
            LOG_MSG("%s: %d: %s: Failed to create Router Discovery processing task\n", __FILE__, __LINE__, routine_name);
            return L7_FAILURE;
        }

        if (osapiWaitForTaskInit(L7_RTR_DISC_PROCESSING_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
        {
            LOG_MSG("%s: %d: %s: Unable to synchronize Router Discovery processing task\n", __FILE__, __LINE__, routine_name);
            return L7_FAILURE;
        }

        pRtrDiscInfo->rtrDiscProcessingQueue = (void *)osapiMsgQueueCreate( RTR_DISC_PROCESS_QUEUE,
                                                                            RTR_DISC_PROC_MSG_COUNT,
                                                                            sizeof(rtrDiscMsg_t) );

        if (pRtrDiscInfo->rtrDiscProcessingQueue == L7_NULLPTR)
        {
            LOG_MSG("%s: %d: %s: Unable to create router discovery processing message queue \n", __FILE__, __LINE__, routine_name);
            return L7_FAILURE;
        } else
        {
            rtrDiscCfgData->cfgHdr.dataChanged = L7_FALSE;
            rtrDiscCnfgrState = RTR_DISC_PHASE_INIT_1;
            rtrDiscRC = L7_SUCCESS;
        }
    }


    return(rtrDiscRC);
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2. Phase 2 mainly involves callback registration.
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
* @notes    This function runs on the processing task.
*       
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t rtrDiscRC = L7_SUCCESS;
    nvStoreFunctionList_t notifyFunctionList;
    sysnetPduIntercept_t sysnetPduIntercept;

    memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
    notifyFunctionList.registrar_ID     = L7_RTR_DISC_COMPONENT_ID;
    notifyFunctionList.notifySave       = rtrDiscSave;
    notifyFunctionList.hasDataChanged   = rtrDiscHasDataChanged;
    notifyFunctionList.notifyConfigDump = rtrDiscConfigDump;
    notifyFunctionList.resetDataChanged = rtrDiscResetDataChanged;

    rtrDiscRC = nvStoreRegister(notifyFunctionList);

    if (rtrDiscRC == L7_FAILURE)
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        rtrDiscRC = L7_ERROR;
    } else if (nimRegisterIntfChange(L7_RTR_DISC_COMPONENT_ID, rtrDiscIntfChangeCallback) != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        rtrDiscRC = L7_ERROR;
    }
    /* Register router discovery  to receive a callback when an interface is enabled
       or disabled for routing or when the ip address of an interface changes*/
    else if (ipMapRegisterRoutingEventChange(L7_IPRT_RTR_DISC,"rtrDiscRoutingEventChangeCallBack", 
                                             rtrDiscRoutingEventChangeCallBack) != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        rtrDiscRC = L7_ERROR;
    } else
    {
        /* Inform NP to send router discovery multicast packets to CPU */
        /* ipMap has already informed DTL about multicast addresses
           224.0.0.1 and 224.0.0.2
        */
        sysnetPduIntercept.addressFamily = L7_AF_INET;
        sysnetPduIntercept.hookId = SYSNET_INET_VALID_IN;
        sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_RTR_DISC_PRECEDENCE;
        sysnetPduIntercept.interceptFunc = rtrDiscPktIntercept;
        strcpy(sysnetPduIntercept.interceptFuncName, "rtrDiscPktIntercept");
        /* Register with sysnet */
        if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
        {
            *pResponse = 0;
            *pReason   = L7_CNFGR_ERR_RC_FATAL;
            rtrDiscRC = L7_ERROR;
        }
    }

    if ( rtrDiscRC != L7_SUCCESS )
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        rtrDiscRC = L7_ERROR;

    } else
    {
        *pResponse = L7_CNFGR_CMD_COMPLETE;
        *pReason   = 0;
        rtrDiscCnfgrState = RTR_DISC_PHASE_INIT_2;
    }


    /* Return Value to caller */
    return(rtrDiscRC);
}




/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3. Phase 3 involves applying building and 
*           applying the configuration onto the hardware.
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
L7_RC_t rtrDiscCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t rtrDiscRC = L7_SUCCESS;

    rtrDiscRC = sysapiCfgFileGet(L7_RTR_DISC_COMPONENT_ID, L7_RTR_DISC_FILENAME, (L7_char8 *)rtrDiscCfgData,
                                 sizeof(rtrDiscCfgData_t), &rtrDiscCfgData->checkSum, L7_RTR_DISC_CFG_VER_CURRENT,
                                 rtrDiscBuildDefaultConfigData, rtrDiscMigrateConfigData);

    if ( rtrDiscRC != L7_SUCCESS )
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        rtrDiscRC = L7_ERROR;

    } else
    {
        *pResponse = L7_CNFGR_CMD_COMPLETE;
        *pReason   = 0;
    }

    pRtrDiscTimerHolder = L7_NULLPTR;

    osapiTimerAdd ( (void*)rtrDiscTimerTick,L7_NULL,L7_NULL,RTR_DISC_TASK_TIMER,&pRtrDiscTimerHolder);

    if (pRtrDiscTimerHolder == L7_NULLPTR)
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        rtrDiscRC = L7_ERROR;
    } else
    {
        if (rtrDiscApplyConfigData() != L7_SUCCESS)
        {
            *pResponse = 0;
            *pReason   = L7_CNFGR_ERR_RC_FATAL;
            rtrDiscRC = L7_ERROR;
        } else
        {
            rtrDiscCfgData->cfgHdr.dataChanged = L7_FALSE;

            /* Return Value to caller */
            *pResponse = L7_CNFGR_CMD_COMPLETE;
            *pReason   = 0;
            rtrDiscCnfgrState = RTR_DISC_PHASE_INIT_3;
        }
    }

    /* Return Value to caller */
    return(rtrDiscRC);
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
L7_RC_t rtrDiscCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t rtrDiscRC = L7_SUCCESS;


    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    return(rtrDiscRC);
}



/*********************************************************************
* @purpose  This function undoes rtrDiscCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void rtrDiscCnfgrFiniPhase1Process()
{
    if (rtrDiscCfgData != L7_NULLPTR)
        osapiFree(L7_RTR_DISC_COMPONENT_ID, rtrDiscCfgData);
    rtrDiscCfgData = L7_NULLPTR;

    if (rtrDiscMapTbl != L7_NULLPTR)
        osapiFree(L7_RTR_DISC_COMPONENT_ID, rtrDiscMapTbl);
    rtrDiscMapTbl = L7_NULLPTR;

    if(pRtrDiscIntfInfo != L7_NULLPTR)
    {
        osapiFree(L7_RTR_DISC_COMPONENT_ID, pRtrDiscIntfInfo);
        pRtrDiscIntfInfo = L7_NULLPTR;
    }


    osapiRWLockDelete(rtrDiscIntfRWLock);

    if (pRtrDiscInfo->rtrDiscProcessingTaskId != L7_ERROR)
    {
        osapiTaskDelete(pRtrDiscInfo->rtrDiscProcessingTaskId);
    }

    if (pRtrDiscInfo->rtrDiscProcessingQueue != L7_NULLPTR)
    {
        osapiMsgQueueDelete(pRtrDiscInfo->rtrDiscProcessingQueue);
        pRtrDiscInfo->rtrDiscProcessingQueue = L7_NULLPTR;
    }

    if(pRtrDiscInfo != L7_NULLPTR)
    {
        osapiFree(L7_RTR_DISC_COMPONENT_ID, pRtrDiscInfo);
        pRtrDiscInfo = L7_NULLPTR;
    }
}
/*********************************************************************
* @purpose  This function undoes rtrDiscCnfgrInitPhase2Process, i.e. 
*           deregister from all callbacks.
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void rtrDiscCnfgrFiniPhase2Process()
{


    sysnetPduIntercept_t sysnetPduIntercept;

    (void)nimDeRegisterIntfChange(L7_RTR_DISC_COMPONENT_ID);
    (void)ipMapDeregisterRoutingEventChange( L7_IPRT_RTR_DISC );
    (void) nvStoreDeregister(L7_RTR_DISC_COMPONENT_ID);

    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_VALID_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_RTR_DISC_PRECEDENCE;
    sysnetPduIntercept.interceptFunc = rtrDiscPktIntercept;
    strcpy(sysnetPduIntercept.interceptFuncName, "rtrDiscPktIntercept");

    /* De-register with sysnet */
    sysNetPduInterceptDeregister(&sysnetPduIntercept);

    return;
}
/*********************************************************************
* @purpose  This function undoes rtrDiscCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    rtrDiscCnfgrUnconfigPhase2 unapplies the configuration.
*           Hence, is reused to undo INIT_3
*
* @end
*********************************************************************/
void rtrDiscCnfgrFiniPhase3Process()
{
    L7_CNFGR_RESPONSE_t response;
    L7_CNFGR_ERR_RC_t   reason;

    rtrDiscCnfgrUnconfigPhase2(&response, &reason);
    /* rtrDiscCnfgrUnconfigPhase2() places the component in WMU */

    if (pRtrDiscTimerHolder != L7_NULLPTR )
        osapiTimerFree(pRtrDiscTimerHolder);
    pRtrDiscTimerHolder = L7_NULLPTR;

    return;
}


/*********************************************************************
* @purpose  This function processes the configurator control commands/request
*           pair Unconfigure Phase 2. Mainly unapplies the current 
*           configuration
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
*           rtrDiscRestoreProcess only builds default configuration and 
*           applies it, while UNCONFIG_2 requires configuration information 
*           to be reset. Hence, rtrDiscCfgData is reset to 0.
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
    osapiTimerFree(pRtrDiscTimerHolder);
    rtrDiscRestoreProcess();

    memset((void *)rtrDiscCfgData, 0, sizeof(rtrDiscCfgData_t));
    rtrDiscCnfgrState = RTR_DISC_PHASE_WMU;

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function parses the cnfgr command/pair object and handles 
*           the command/request.          
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
* @notes    This function runs in the router discovery processing task thread.. 
*           
* @notes    In current release (H), INIT_1 never runs on this thread,
*           hence is not handled here.

* @end
*********************************************************************/

void rtrDiscCnfgrParse( L7_CNFGR_CMD_DATA_t *pCmdData )
{
    static const char *routine_name = "rtrDiscCnfgrParse";
    /* set up variables and structures */
    L7_CNFGR_CMD_t        command;
    L7_CNFGR_RQST_t       request;
    L7_CNFGR_CB_DATA_t    cbData;
    L7_CNFGR_RESPONSE_t   response;
    L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

    L7_RC_t             rtrDiscRC = L7_ERROR;
    L7_CNFGR_ERR_RC_t   reason    = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                    case L7_CNFGR_RQST_I_PHASE2_START:
                        if ((rtrDiscRC = rtrDiscCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                        {
                            rtrDiscCnfgrFiniPhase2Process();
                        }
                        break;
                    case L7_CNFGR_RQST_I_PHASE3_START:
                        if ((rtrDiscRC = rtrDiscCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                        {
                            rtrDiscCnfgrFiniPhase3Process();
                        }
                        break;
                    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                        rtrDiscRC = rtrDiscCnfgrNoopProcess( &response, &reason );
                        rtrDiscCnfgrState = RTR_DISC_PHASE_WMU;
                        break;
                    default:
                        /* invalid command/request pair */
                        LOG_MSG("%s %d: %s: Invalid command/request pair\n",__FILE__, __LINE__,routine_name);
                        break;
                    } /* endswitch initialize requests */
                    break;

                case L7_CNFGR_CMD_EXECUTE:
                    switch ( request )
                    {
                    case L7_CNFGR_RQST_E_START:
                        rtrDiscRC = L7_SUCCESS;
                        response  = L7_CNFGR_CMD_COMPLETE;
                        reason    = 0;
                        rtrDiscCnfgrState = RTR_DISC_PHASE_EXECUTE;
                        break;

                    default:
                        /* invalid command/request pair */
                        LOG_MSG("%s %d: %s: Invalid command/request pair\n",__FILE__, __LINE__,routine_name);
                        break;
                    }
                    break;

                case L7_CNFGR_CMD_UNCONFIGURE:
                    switch ( request )
                    {
                    case L7_CNFGR_RQST_U_PHASE1_START:
                        rtrDiscRC = rtrDiscCnfgrNoopProcess( &response, &reason );
                        rtrDiscCnfgrState = RTR_DISC_PHASE_UNCONFIG_1;
                        break;

                    case L7_CNFGR_RQST_U_PHASE2_START:
                        rtrDiscRC = rtrDiscCnfgrUnconfigPhase2( &response, &reason );
                        rtrDiscCnfgrState = RTR_DISC_PHASE_UNCONFIG_2;
                        /* we need to do something with the stats in the future */
                        break;

                    default:
                        /* invalid command/request pair */
                        LOG_MSG("%s %d: %s: Invalid command/request pair\n",__FILE__, __LINE__,routine_name);
                        break;
                    }
                    break;

                case L7_CNFGR_CMD_TERMINATE:
                case L7_CNFGR_CMD_SUSPEND:
                    rtrDiscRC = rtrDiscCnfgrNoopProcess( &response, &reason );
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
    cbData.asyncResponse.rc = rtrDiscRC;
    if (rtrDiscRC == L7_SUCCESS)
        cbData.asyncResponse.u.response = response;
    else
        cbData.asyncResponse.u.reason   = reason;

    cnfgrApiCallback(&cbData);

    return;
}

