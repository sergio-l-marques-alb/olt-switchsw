/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  relaymap_cnfgr.c
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
#include "nimapi.h"
#include "relaymap_cnfgr.h"
#include "relay_config.h"
#include "relay_util.h"

extern dhcpRelayCfg_t        *pDhcpRelayCfgData;
dhcpRelayCfg_t *ihCfg = NULL;    /* alias for pDhcpRelayCfgData */
extern dhcpRelayInfo_t       *pDhcpRelayInfo;
dhcpRelayInfo_t *ihInfo = NULL;  /* alias for pDhcpRelayInfo */
pseudo_header_t *pseudo_packet = NULL;

/* The current state of the unit. The main configurator maintains separate information
   about the component's current state. dhcpRelayMapCnfgrState is used by the DHCP relay
   component alone to keep track of its current state. */
dhcpRelayMapCnfgrState_t dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_INIT_0;


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
void dhcpRelayApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
    /* set up variables and structures */
    L7_CNFGR_CMD_t        command;
    L7_CNFGR_RQST_t       request;
    L7_CNFGR_CB_DATA_t    cbData;
    L7_CNFGR_RESPONSE_t   response;
    L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

    udpRelayMsg_t        msg;

    L7_RC_t             dhcpRelayRC = L7_ERROR;
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
                if ((command == L7_CNFGR_CMD_INITIALIZE) && (request == L7_CNFGR_RQST_I_PHASE1_START))
                {
                    if ((dhcpRelayRC = dhcpRelayCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                    {
                        dhcpRelayCnfgrFiniPhase1Process();
                    }
                } else
                {
                    /*  Put a msg into the msgQ created in phase 1. The processing task 
                        created in Phase 1 should handle all cnfgr cllabcks other than the 
                        one from phase 1 */
                    memcpy(&msg.type.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
                    msg.msgId = DHCPRELAYMAP_CNFGR;
                    osapiMessageSend(dhcpRelayQueue, &msg, sizeof(udpRelayMsg_t),
                                     L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);
                    return; /* The processing task will take care of the returning the result 
                                of the command when it is executed */
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
    cbData.asyncResponse.rc = dhcpRelayRC;
    if (dhcpRelayRC == L7_SUCCESS)
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
L7_RC_t dhcpRelayCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t dhcpRelayRC = L7_FAILURE;

    /* use single-pass loop for exception control */
    do
    {
        /*---------------------------------------------------------------*/
        /* malloc space for the dhcp relay configuration data structure  */
        /*---------------------------------------------------------------*/

        pDhcpRelayCfgData = (dhcpRelayCfg_t *)osapiMalloc(L7_DHCP_RELAY_COMPONENT_ID, sizeof(dhcpRelayCfg_t));
        if (pDhcpRelayCfgData == L7_NULL)
        {
            L7_LOG(L7_LOG_SEVERITY_EMERGENCY, L7_DHCP_RELAY_COMPONENT_ID,
                   "Error allocating memory\n");
            break;
        }
        ihCfg = pDhcpRelayCfgData;

        /*----------------------------------------------*/
        /* malloc space for DHCP_RELAY  info structure */
        /*----------------------------------------------*/
        pDhcpRelayInfo = (dhcpRelayInfo_t *)osapiMalloc(L7_DHCP_RELAY_COMPONENT_ID, sizeof(dhcpRelayInfo_t));
        if (pDhcpRelayInfo == L7_NULL)
        {
            L7_LOG(L7_LOG_SEVERITY_EMERGENCY, L7_DHCP_RELAY_COMPONENT_ID,
                   "Error allocating IP helper info structure\n");
            break;
        }
        memset(pDhcpRelayInfo, 0, sizeof(dhcpRelayInfo_t));
        ihInfo = pDhcpRelayInfo;

        pDhcpRelayCfgData->relayEntryList.maxEntries = L7_IP_HELPER_ENTRIES_MAX;
        if (ihRelayEntryListCreate() != L7_SUCCESS)
        {
          L7_LOG(L7_LOG_SEVERITY_EMERGENCY, L7_DHCP_RELAY_COMPONENT_ID,
                 "Failed to create relay entry list.");
          return L7_FAILURE;
        }

        if (ihPacketPoolCreate() != L7_SUCCESS)
        {
          L7_LOG(L7_LOG_SEVERITY_EMERGENCY, L7_DHCP_RELAY_COMPONENT_ID,
                 "Failed to create IP helper packet pool.");
          return L7_FAILURE;
        }

        /* Allocate one global buffer to use as a scratch pad for configuring 
         * UDP checksums */
        pseudo_packet = (pseudo_header_t*) osapiMalloc(L7_DHCP_RELAY_COMPONENT_ID, DHCP_UDP_PSEUDO_PACKET_LEN);
        if (pseudo_packet == NULL)
        {
          L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DHCP_RELAY_COMPONENT_ID,
                  "Unable to allocate buffer for pseudo header.");
          return L7_FAILURE;
        }

        /* Binary semaphore to synchronize access to component data */
        ihInfo->ihLock = (void *)osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
        if (!ihInfo->ihLock)
        {
          L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DHCP_RELAY_COMPONENT_ID,
                  "Unable to create semaphore for IP Helper.");
          return L7_FAILURE;
        }
        
        /*--------------------------*/
        /* try to start application */
        /*--------------------------*/

        /* DHCP_RELAY_MAP_QUEUE. Queue used for both protocol events and 
           other events such as those from CNFGR */
        dhcpRelayQueue = (void *)osapiMsgQueueCreate(DHCP_RELAY_QUEUE,
                                                     DHCP_RELAY_MSG_COUNT,
                                                     sizeof(udpRelayMsg_t) );

        if (dhcpRelayQueue == L7_NULLPTR)
        {
            LOG_MSG("Unable to create IP helper message queue.\n");
            L7_assert(1);
        }

        /*now start dhcpRelay_task */
        pDhcpRelayInfo->dhcpRelayTaskId = osapiTaskCreate("IpHelperTask", udpRelay_Task, 0, 0,
                                                          L7_DEFAULT_STACK_SIZE,
                                                          FD_CNFGR_SIM_DEFAULT_STATS_TASK_PRI,
                                                          L7_DEFAULT_TASK_SLICE);

        if (pDhcpRelayInfo->dhcpRelayTaskId == L7_ERROR)
        {
            LOG_MSG("Failed to create IP helper task\n");
            return L7_FAILURE;
        }

        /* must wait here until the dhcpRelay_Task has reached a certain state
         * of initialization, or else things will fail when trying to
         * apply the configuration data for the DHCP_RELAY interfaces (due to
         * uninitialized pointers, etc.)
         */
        if (osapiWaitForTaskInit(L7_DHCP_RELAY_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
        {
            LOG_MSG("Unable to synchronize with IP helper task\n");
            return L7_FAILURE;
        }

        pDhcpRelayInfo->dataChanged = L7_FALSE;
        dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_INIT_1;
        return L7_SUCCESS;        /* everything worked */

    } while (0);

    if (pDhcpRelayCfgData != L7_NULL)
    {
      pDhcpRelayInfo->dataChanged = L7_FALSE;
    }

    return(dhcpRelayRC);
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
* @notes    DHCP relay does not require any interface information,hence
*           does not register either NIM or IP Map.
*
* @end
*********************************************************************/
L7_RC_t dhcpRelayCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t dhcpRelayRC = L7_SUCCESS;
    nvStoreFunctionList_t notifyFunctionList;

    /*----------------------*/
    /* nvStore registration */
    /*----------------------*/
    memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
    notifyFunctionList.registrar_ID   = L7_DHCP_RELAY_COMPONENT_ID;
    notifyFunctionList.notifySave     = dhcpRelaySave;
    notifyFunctionList.hasDataChanged = dhcpRelayHasDataChanged;
    notifyFunctionList.resetDataChanged = dhcpRelayResetDataChanged;

    dhcpRelayRC = nvStoreRegister(notifyFunctionList);

    /* callbacks need not be registered with IP Map and NIM  by DHCP_RELAY */

    if ( dhcpRelayRC != L7_SUCCESS )
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        dhcpRelayRC = L7_ERROR;

    }
    else if (nimRegisterIntfChange(L7_DHCP_RELAY_COMPONENT_ID, dhcpRelayIntfChangeCallback)
             != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        dhcpRelayRC = L7_ERROR;
    }
    else
    {
        *pResponse = L7_CNFGR_CMD_COMPLETE;
        *pReason   = 0;
        dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_INIT_2;
    }

    /* Return Value to caller */
    return(dhcpRelayRC);
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
L7_RC_t dhcpRelayCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason )
{
  dhcpRelayBuildDefaultConfigData(0);
  dhcpRelayApplyConfigData();
  dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_INIT_3;
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  return L7_SUCCESS;
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
L7_RC_t dhcpRelayCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t dhcpRelayRC = L7_SUCCESS;


    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    return(dhcpRelayRC);
}



/*********************************************************************
* @purpose  This function undoes dhcpRelayCnfgrInitPhase1Process.
*
* @param    none
*     
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
void dhcpRelayCnfgrFiniPhase1Process()
{

    if (pDhcpRelayCfgData != L7_NULLPTR)
    {
        osapiFree(L7_DHCP_RELAY_COMPONENT_ID, pDhcpRelayCfgData);
        pDhcpRelayCfgData = L7_NULLPTR;
    }

    if (pDhcpRelayInfo->dhcpRelayTaskId != L7_ERROR)
    {
        osapiTaskDelete(pDhcpRelayInfo->dhcpRelayTaskId);
    }

    if (pDhcpRelayInfo  != L7_NULLPTR)
    {
        osapiFree(L7_DHCP_RELAY_COMPONENT_ID, pDhcpRelayInfo);
        pDhcpRelayInfo = L7_NULLPTR;
    }

    if (dhcpRelayQueue != L7_NULLPTR)
    {
        osapiMsgQueueDelete(dhcpRelayQueue);
        dhcpRelayQueue = L7_NULLPTR;
    }

    ihRelayEntryListDelete();

    dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_INIT_0;
}
/*********************************************************************
* @purpose  This function undoes dhcpRelayCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpRelayCnfgrFiniPhase2Process()
{
    /* Return Value to caller */
    (void) nvStoreDeregister(L7_DHCP_RELAY_COMPONENT_ID);
    dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_INIT_1;
    return;
}
/*********************************************************************
* @purpose  This function undoes dhcpRelayCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    dhcpRelayCnfgrUnconfigPhase2 unapplies the configuration.
*           Hence, is reused to undo INIT_3
*
* @end
*********************************************************************/
void dhcpRelayCnfgrFiniPhase3Process()
{
    L7_CNFGR_RESPONSE_t response;
    L7_CNFGR_ERR_RC_t   reason;

    dhcpRelayCnfgrUnconfigPhase2(&response, &reason);
    /* dhcpRelayCnfgrUnconfigPhase2 places the component in WMU */
    
    return;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2. Mainly unapplies the current 
*           configuration 
*           
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
*           dhcpRelayRestoreProcess only builds default configuration and 
*           applies it, while UNCONFIG_2 requires configuration information 
*           to be reset. Hence, pDhcpRelayCfgData is reset to 0.
*        
*     
* @end
*********************************************************************/
L7_RC_t dhcpRelayCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{

    dhcpRelayRestoreProcess();

    dhcpRelayBuildDefaultConfigData(L7_DHCP_RELAY_CFG_VER_CURRENT);
    dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_WMU;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function parses the cnfgr command/pair object and handles 
*           the command/request. 
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
* @notes    Runs on the processing task.
*
* @notes    In current release (H), INIT_1 never runs on this thread,
*           hence is not handled here.
*     
* @end
*********************************************************************/
void dhcpRelayCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
    static const char *routine_name = "dhcpRelayCnfgrParse()";

    /* set up variables and structures */
    L7_CNFGR_CMD_t        command;
    L7_CNFGR_RQST_t       request;
    L7_CNFGR_CB_DATA_t    cbData;
    L7_CNFGR_RESPONSE_t   response;
    L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

    L7_RC_t             dhcpRelayRC = L7_ERROR;
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
                    case L7_CNFGR_RQST_I_PHASE2_START:
                        if ((dhcpRelayRC = dhcpRelayCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                        {
                            dhcpRelayCnfgrFiniPhase2Process();
                        }
                        break;
                    case L7_CNFGR_RQST_I_PHASE3_START:
                        if ((dhcpRelayRC = dhcpRelayCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                        {
                            dhcpRelayCnfgrFiniPhase3Process();
                        }
                        break;
                    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                        dhcpRelayRC = dhcpRelayCnfgrNoopProcess( &response, &reason );
                        dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_WMU;
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
                        dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_EXECUTE;
                        dhcpRelayRC = L7_SUCCESS;
                        response  = L7_CNFGR_CMD_COMPLETE;
                        reason    = 0;
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
                        dhcpRelayRC = dhcpRelayCnfgrNoopProcess( &response, &reason );
                        if (dhcpRelayRC == L7_SUCCESS)
                            dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_UNCONFIG_1;
                        break;

                    case L7_CNFGR_RQST_U_PHASE2_START:
                        dhcpRelayRC = dhcpRelayCnfgrUnconfigPhase2( &response, &reason );
                        if (dhcpRelayRC == L7_SUCCESS)
                            dhcpRelayMapCnfgrState = DHCPRELAYMAP_PHASE_UNCONFIG_2;
                        /* we need to do something with the stats in the future */
                        break;

                    default:
                        LOG_MSG("%s %d: %s: Invalid command/request pair\n",__FILE__, __LINE__,routine_name);
                        break;
                    }
                    break;

                case L7_CNFGR_CMD_TERMINATE:
                case L7_CNFGR_CMD_SUSPEND:
                    dhcpRelayRC = dhcpRelayCnfgrNoopProcess( &response, &reason );
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
    cbData.asyncResponse.rc = dhcpRelayRC;
    if (dhcpRelayRC == L7_SUCCESS)
        cbData.asyncResponse.u.response = response;
    else
        cbData.asyncResponse.u.reason   = reason;

    cnfgrApiCallback(&cbData);

    return;
}

L7_BOOL dhcpRelay_is_ready()
{
  return (DHCPRELAY_IS_READY);
}

