/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename ospf_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component
*
* @comments For this release of the stacking feature, we make the following
*           assumptions:  If any component fails to complete a command and
*           returns an error to the configurator, the configurator will reset
*           the router. Thus, the code always expects an orderly sequence of
*           commands transitioning from state to state as follows:
*
*           INIT_0 -> INIT_1 -> INIT_2 -> WMU -> INIT_3 -> EXECUTE
*
*           Additionally, if the local unit resigns as the management unit or
*           if the user issues a "clear config" command, the component can
*           transition as follows:
*
*           EXECUTE -> UNCONFIG_1 -> UNCONFIG_2 -> WMU
*
*           An implication of this orderly state transition is that a component
*           need not check its current state to determine whether
*           the transition requested by a given command is valid.
*
*           The component's data and behavior in the WMU are exactly the same
*           whether it transitioned to the WMU state from the INIT_2 state
*           or the UNCONFIG_2 state.
*
*           Note that the set of legal transitions above do not include
*           transitions back to INIT_0, INIT_1, or INIT_2. Thus, there is no
*           code written to undo the initialization actions taken to enter
*           the INIT_1 or INIT_2 states.
*
*
* @create 07/22/2003
*
* @author markl
* @end
*
**********************************************************************/


#include "l7_ospf_api.h"             /* for ospfFuncTable_t */
#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "ospf_config.h"
#include "ospf_util.h"
#include "ospf_cnfgr.h"
#include "ospf_vend_ctrl.h"
#ifdef L7_NSF_PACKAGE
#include "ospf_ckpt.h"
#endif

static L7_BOOL traceConfigState = L7_FALSE;

ospfMapCnfgrState_t ospfMapCnfgrState = OSPFMAP_PHASE_INIT_0;
L7_int32 ospfProcTaskHandle = 0;

/* The OSPF processing thread reads events from two message queues. msgQSema
 * is a counting semaphore to indicate whether data is available in
 * one of the message queues. Ospf_Proc_Queue contains all events other than
 * redistribution events. Ospf_Redist_Queue contains the redistribution events.
 * There can be many redistribution events all at once, and we don't want
 * other events (interface, router, configuration events) to wait behind a
 * boatload of redistribution events. */
void *ospfMsgQSema = L7_NULLPTR;
void *Ospf_Proc_Queue = L7_NULLPTR;
void *Ospf_Redist_Queue = L7_NULLPTR;


/* Externs */
extern L7_ospfMapCfg_t     *pOspfMapCfgData;
extern ospfInfo_t          *pOspfInfo;
extern ospfAreaInfo_t      *pOspfAreaInfo;
extern ospfVlinkInfo_t     *pOspfVlinkInfo;
extern ospfIntfInfo_t      *pOspfIntfInfo;
extern ospfNetworkAreaInfo_t *pOspfNetworkAreaInfo;
extern ospfNetworkAreaInfo_t *networkAreaListHead_g, *networkAreaListFree_g;
extern L7_uint32           *ospfMapMapTbl;
extern ospfMapCnfgrState_t ospfMapCnfgrState;
extern ospfMapDeregister_t ospfMapDeregister;
extern rtoRouteChange_t *ospfRouteChangeBuf;

extern ospfMapDebugCfg_t ospfMapDebugCfg;    /* Debug Configuration File Overlay */
extern void ospfMapDebugRegister(void);
extern void ospfMapDebugCfgRead(void);

L7_char8 *ospfCfgPhaseNames[] = {"INIT 0", "INIT 1", "INIT 2", "WMU", "INIT 3",
                                 "EXECUTE", "UNCONFIG 1", "UNCONFIG 2"};


static void ospfCnfgrStateSet(ospfMapCnfgrState_t newState);
static L7_RC_t ospfMappingThreadCreate(void);
static void    ospfFuncTableInit(void);
static L7_RC_t ospfCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                          L7_BOOL warmRestart,
                                          L7_CNFGR_RESPONSE_t *response,
                                          L7_CNFGR_ERR_RC_t *reason);
static L7_RC_t ospfUnconfigRequestHandle(L7_CNFGR_RQST_t request,
                                         L7_CNFGR_RESPONSE_t *response,
                                         L7_CNFGR_ERR_RC_t *reason);
static L7_RC_t ospfMapCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                              L7_CNFGR_ERR_RC_t   *pReason );
static L7_RC_t ospfMapCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                              L7_CNFGR_ERR_RC_t   *pReason );
static L7_RC_t ospfMapCnfgrInitPhase3Process(L7_BOOL warmRestart,
                                             L7_CNFGR_RESPONSE_t *pResponse,
                                             L7_CNFGR_ERR_RC_t   *pReason );
static void ospfMapCnfgrFiniPhase1Process(void);
static void ospfMapCnfgrFiniPhase2Process(void);
static void ospfMapCnfgrFiniPhase3Process(void);
static L7_RC_t ospfMapCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason );
static L7_RC_t ospfMapCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                          L7_CNFGR_ERR_RC_t   *pReason );
static L7_RC_t ospfMapInfoAllocate(void);
static L7_RC_t ospfMapInfoDeallocate(void);


/* Prevent the max number of OSPF areas to be greater than
 * the maximum number of interfaces.
 */
#if (L7_OSPF_MAX_AREAS > L7_MAX_INTERFACE_COUNT)
#error OSPF Build Error
#endif


/*********************************************************************
*
* @purpose  Enabling tracing of configuration state changes.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ospfCnfgrStateTrace(void)
{
    traceConfigState = L7_TRUE;
}

/*********************************************************************
*
* @purpose  Set the OSPF configuration state
*
* @param    newState - @b{(input)}  The configuration state that OSPF
*                                   is transitioning to.
*
* @returns  void
*
* @notes    Set traceConfigState to L7_TRUE to trace configuration state
*           transitions.
*
* @end
*********************************************************************/
static void ospfCnfgrStateSet(ospfMapCnfgrState_t newState)
{
    if (traceConfigState == L7_TRUE) {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                      "OSPF configuration state set to %s\n",
                      ospfCfgPhaseNames[newState]);
    }
    ospfMapCnfgrState = newState;
}

/*********************************************************************
*
* @purpose  Determine whether OSPF is in a configuration state where it
*           is ready to handle interface events.
*
* @param    void
*
* @returns  L7_TRUE if OSPF can handle interface events
* @returns  L7_FALSE otherwise
*
* @notes    OSPF can handle interface events in INIT_3, EXECUTE, or
*           UNCONFIG_1 states. INIT_3 is allowed because NIM may go
*           to EXECUTE and issue events before OSPF processes its
*           notification to transition to EXECUTE. UNCONFIG_1 is
*           allowed because a stack may transition through the
*           unconfig states when a unit leaves the stack and NIM
*           will issue events to delete the interfaces associated
*           with the lost unit during UNCONFIG_1.
*
* @end
*********************************************************************/
L7_BOOL ospfMapReadyForIntfEvents(void)
{
    if ((ospfMapCnfgrState == OSPFMAP_PHASE_INIT_3) ||
        (ospfMapCnfgrState == OSPFMAP_PHASE_EXECUTE) ||
        (ospfMapCnfgrState == OSPFMAP_PHASE_UNCONFIG_1))
        return L7_TRUE;
    else
        return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Handle initialization events from the configurator.
*
* @param    pCmdData - @b{(input)}  Indicates the command and request
*                                   from the configurator
*
* @returns  void
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the OSPF Map component.  This function is re-entrant.
*           Most actions are passed to the OSPF mapping thread instead of
*           being processed on the configurator's thread. Success or failure
*           of the command is returned to the configurator asynchronously.
*
* @end
*********************************************************************/
void ospfMapApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
    ospfMapMsg_t msg;           /* message to pass request to OSPF map thread */
    L7_CNFGR_CB_DATA_t cbData;  /* indicates response to correlator */

    /* validate command type */
    if ( pCmdData == L7_NULL )
    {
        cbData.correlator               = L7_NULL;
        cbData.asyncResponse.rc         = L7_ERROR;
        cbData.asyncResponse.u.reason   = L7_CNFGR_ERR_RC_INVALID_CMD;
        cnfgrApiCallback(&cbData);
        return;
    }

    /* Do minimum amount of work on configurator's thread. Pass other work
     * to OSPF mapping thread.
     */
    if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
        (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
    {
        /* Create the message queue and the OSPF mapping thread, then pass
         * a message via the queue to the mapping thread to complete
         * phase 1 initialization.
         */
        if (ospfMappingThreadCreate() != L7_SUCCESS) {
            cbData.correlator = L7_NULL;
            cbData.asyncResponse.rc = L7_ERROR;
            cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
            cnfgrApiCallback(&cbData);
            return;
        }
    }

    memcpy(&msg.type.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.msg = OSPF_CNFGR_INIT;
    if (osapiMessageSend(Ospf_Proc_Queue, &msg, sizeof(ospfMapMsg_t),
                           L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
    {
        osapiSemaGive(ospfMsgQSema);
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_OSPF_MAP_COMPONENT_ID,
               "Failure sending OSPF configurator init message.");
    }

    return;
}

/*********************************************************************
* @purpose  Create the OSPF mapping thread and the message queue used to
*           send work to the thread. The mapping layer thread performs
*           initialization tasks at the request of the configurator.
*
* @param    void
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes    This function runs in the configurator's thread and should
*           avoid blocking.
*
* @end
*********************************************************************/
static L7_RC_t ospfMappingThreadCreate(void)
{
    /* Counting semaphore. Given whenever a message is added to any message queue
     * for the OSPF processing task. Taken when a message is read. */
    ospfMsgQSema = osapiSemaCCreate (OSAPI_SEM_Q_FIFO, 0);
    if (ospfMsgQSema == L7_NULLPTR)
    {
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_OSPF_MAP_COMPONENT_ID,
               "Failure creating OSPF message queue semaphore.");
        return L7_FAILURE;
    }

    /* create semaphore used to synchronize access to mapping layer config data */
    ospfMapCtrl_g.cfgSema = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
    if (ospfMapCtrl_g.cfgSema == L7_NULLPTR)
    {
        OSPFMAP_ERROR("Error initializing OSPF configuration semaphore.\n");
        return L7_FAILURE;
    }

    /* create OSPF mapping layer thread queue */
    Ospf_Proc_Queue = osapiMsgQueueCreate(OSPF_PROC_QUEUE,
                                          OSPF_PROC_MSG_COUNT,
                                          sizeof(ospfMapMsg_t));

    if (Ospf_Proc_Queue == L7_NULLPTR) {
        OSPFMAP_ERROR("OSPF unable to create mapping thread message queue\n");
        return L7_FAILURE;
    }

    /* create OSPF redistribution queue */
    Ospf_Redist_Queue = osapiMsgQueueCreate(OSPF_REDIST_QUEUE,
                                            OSPF_REDIST_MSG_COUNT,
                                            sizeof(ospfMapMsg_t));

    if (Ospf_Redist_Queue == L7_NULLPTR) {
        OSPFMAP_ERROR("OSPF unable to create redistribution message queue\n");
        return L7_FAILURE;
    }

    /* Create mapping layer thread */
    ospfProcTaskHandle = osapiTaskCreate(OSPF_PROC_TASK,
                                         ospfProcTask,
                                         L7_NULL, L7_NULLPTR,
                                         L7_DEFAULT_STACK_SIZE,
                                         L7_DEFAULT_TASK_PRIORITY,
                                         L7_DEFAULT_TASK_SLICE);

    if (ospfProcTaskHandle == L7_ERROR)
    {
        OSPFMAP_ERROR("ERROR: Unable to create OSPF mapping thread\n");
        return L7_FAILURE;
    }
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Handles initialization messages from the configurator.
*
* @param    pCmdData - @b{(input)}  Indicates the command and request
*                                   from the configurator
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void ospfCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
{
    L7_CNFGR_CMD_t command;
    L7_CNFGR_RQST_t request;
    L7_CNFGR_CB_DATA_t cbData;
    L7_CNFGR_RESPONSE_t response = L7_CNFGR_INVALID_RESPONSE;
    L7_RC_t ospfMapRC = L7_ERROR;
    L7_CNFGR_ERR_RC_t reason = L7_CNFGR_ERR_RC_INVALID_PAIR;
    L7_BOOL warmRestart = L7_FALSE;

    if ((pCmdData == L7_NULLPTR) || (pCmdData->type != L7_CNFGR_RQST))
    {
        cbData.correlator = L7_NULL;
        cbData.asyncResponse.rc = L7_ERROR;
        cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
        cnfgrApiCallback(&cbData);
        return;
    }

    command = pCmdData->command;
    request = pCmdData->u.rqstData.request;
    cbData.correlator = pCmdData->correlator;
    if ((request <= L7_CNFGR_RQST_FIRST) || (request >= L7_CNFGR_RQST_LAST))
    {
        /* invalid request */
        cbData.asyncResponse.rc = L7_ERROR;
        cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
        cnfgrApiCallback(&cbData);
        return;
    }

    switch ( command )
    {
        case L7_CNFGR_CMD_INITIALIZE:
            if (request == L7_CNFGR_RQST_I_PHASE3_START)
            {
              warmRestart = (pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM);
            }
            ospfMapRC = ospfCnfgrInitRequestHandle(request, warmRestart, &response, &reason);
            break;

        case L7_CNFGR_CMD_EXECUTE:
            switch ( request )
            {
                case L7_CNFGR_RQST_E_START:
                    ospfCnfgrStateSet(OSPFMAP_PHASE_EXECUTE);
                    ospfMapRC  = L7_SUCCESS;
                    response  = L7_CNFGR_CMD_COMPLETE;
                    reason    = 0;
                    break;

                default:
                    /* invalid command/request pair */
                    reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
                    break;
            }
            break;

        case L7_CNFGR_CMD_UNCONFIGURE:
            ospfMapRC = ospfUnconfigRequestHandle(request, &response, &reason);
            break;

        case L7_CNFGR_CMD_TERMINATE:
#ifdef L7_NSF_PACKAGE
          ospfMapRC = o2TerminateProcess();
          response = L7_CNFGR_CMD_COMPLETE;
          reason   = 0;
          break;
#endif
        /* if not NSF, fall through and do no-op */
        case L7_CNFGR_CMD_SUSPEND:
        case L7_CNFGR_CMD_RESUME:
            ospfMapRC = ospfMapCnfgrNoopProcess( &response, &reason );
            break;

        default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
    } /* endswitch command/event pair */

    /* respond to configurator */
    cbData.asyncResponse.rc = ospfMapRC;
    if (ospfMapRC == L7_SUCCESS)
        cbData.asyncResponse.u.response = response;
    else
        cbData.asyncResponse.u.reason = reason;

    cnfgrApiCallback(&cbData);
    return;
}

/*********************************************************************
*
* @purpose  Initialize the function pointers in the function table
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void ospfFuncTableInit(void)
{
    ospfFuncTable.ospfMapCfgDataShow      = ospfMapCfgDataShow;
    ospfFuncTable.ospfMapExtenRTOShow     = ospfMapExtenRTOShow;
    ospfFuncTable.ospfMapExtenIFOShow     = ospfMapExtenIFOShow;
    ospfFuncTable.ospfMapExtenAROShow     = ospfMapExtenAROShow;
    ospfFuncTable.ospfMapExtenNBOShow     = ospfMapExtenNBOShow;
}

/*********************************************************************
*
* @purpose  Handles an initialization request from the configurator.
*
* @param @b{(input)} request - indicates the initialization phase to
*                              be executed
* @param @b{(output)} response - response if request successfully handled
* @param @b{(input)} reason - if an error, gives the reason
*
* @returns  L7_SUCCESS if request successfully handled
*           L7_ERROR if request is invalid
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t ospfCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                          L7_BOOL warmRestart,
                                          L7_CNFGR_RESPONSE_t *response,
                                          L7_CNFGR_ERR_RC_t *reason)
{
    L7_RC_t rc;

    switch ( request )
    {
        case L7_CNFGR_RQST_I_PHASE1_START:
            rc = ospfMapCnfgrInitPhase1Process(response, reason);
            if (rc != L7_SUCCESS)
            {
                ospfMapCnfgrFiniPhase1Process();
            }
            break;
        case L7_CNFGR_RQST_I_PHASE2_START:
            rc = ospfMapCnfgrInitPhase2Process(response, reason);
            if (rc != L7_SUCCESS)
            {
                ospfMapCnfgrFiniPhase2Process();
            }
            break;
        case L7_CNFGR_RQST_I_PHASE3_START:
            rc = ospfMapCnfgrInitPhase3Process(warmRestart, response, reason);
            if (rc != L7_SUCCESS)
            {
                ospfMapCnfgrFiniPhase3Process();
            }
            break;
        case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
            rc = ospfMapCnfgrNoopProcess(response, reason);
            ospfCnfgrStateSet(OSPFMAP_PHASE_WMU);
            break;
        default:
            /* invalid request */
            rc = L7_ERROR;
            *reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            break;
    }
    return rc;
}

/*********************************************************************
*
* @purpose  Handles an unconfigure request from the configurator.
*
* @param @b{(input)} request - indicates the level to which OSPF should
*                              be unconfigured
* @param @b{(output)} response - response if request successfully handled
* @param @b{(input)} reason - if an error, gives the reason
*
* @returns  L7_SUCCESS if request successfully handled
*           L7_ERROR if request is invalid
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t ospfUnconfigRequestHandle(L7_CNFGR_RQST_t request,
                                         L7_CNFGR_RESPONSE_t *response,
                                         L7_CNFGR_ERR_RC_t *reason)
{
    L7_RC_t rc;
    switch ( request )
    {
        case L7_CNFGR_RQST_U_PHASE1_START:
            rc = ospfMapCnfgrNoopProcess(response, reason);
            ospfCnfgrStateSet(OSPFMAP_PHASE_UNCONFIG_1);
            break;

        case L7_CNFGR_RQST_U_PHASE2_START:
            rc = ospfMapCnfgrUconfigPhase2(response, reason);
            /* we need to do something with the stats in the future */
            break;

        default:
            /* invalid command/request pair */
            rc = L7_ERROR;
            *reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            break;
    }
    return rc;
}

/*********************************************************************
* @purpose  Perform actions for phase 1 initialization.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid responses:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
static L7_RC_t ospfMapCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                              L7_CNFGR_ERR_RC_t *pReason )
{
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;

    /*-----------------------------------------------------------------*/
    /*      Malloc space for the ospfMap configuration data structure  */
    /*-----------------------------------------------------------------*/
    pOspfMapCfgData = (L7_ospfMapCfg_t *)osapiMalloc(L7_OSPF_MAP_COMPONENT_ID, sizeof(L7_ospfMapCfg_t));
    if (pOspfMapCfgData == L7_NULLPTR)
    {
        OSPFMAP_ERROR("Error allocating memory for OSPF configuration data\n");
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
        return L7_ERROR;
    }
    memset(( void * )pOspfMapCfgData, 0, sizeof(L7_ospfMapCfg_t));

    /*---------------------------------------*/
    /*     Malloc space for Info structures  */
    /*---------------------------------------*/
    if (ospfMapInfoAllocate() != L7_SUCCESS) {
        (void)ospfMapInfoDeallocate();  /* no need to act on return code here */
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
        return L7_ERROR;
    }

    /* Create buffer for best route notification */
    ospfRouteChangeBuf = (rtoRouteChange_t*) osapiMalloc(L7_OSPF_MAP_COMPONENT_ID,
                                                         L7_OSPF_MAX_BEST_ROUTE_CHANGES *
                                                         sizeof(rtoRouteChange_t));
    if (ospfRouteChangeBuf == NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_OSPF_MAP_COMPONENT_ID,
             "Unable to allocate OSPF route change buffer.");
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      return L7_ERROR;
    }
    memset((void*) ospfRouteChangeBuf, 0,
           L7_OSPF_MAX_BEST_ROUTE_CHANGES * sizeof(rtoRouteChange_t));

#ifdef L7_NSF_PACKAGE
    if (o2CkptTableCreate() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_OSPF_MAP_COMPONENT_ID,
             "Unable to allocate OSPF checkpoint table.");
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      return L7_ERROR;
    }
#endif

    /*---------------------------------------------------------------*/
    /*      Initialize MIB support                                   */
    /*---------------------------------------------------------------*/
    if (ospfMapMibSupportInit() != L7_SUCCESS)
    {
        OSPFMAP_ERROR("Error initializing OSPF MIB support.\n");
        *pResponse = 0;
        *pReason   =  L7_CNFGR_ERR_RC_FATAL;
        return L7_ERROR;
    }

    /* Create the redistribution list, which will keep track of the routes
     * OSPF is currently redistributing.
     */
    if (ospfMapRedistListCreate() != L7_SUCCESS)
    {
        OSPFMAP_ERROR("Error creating OSPF redistribution list.\n");
        *pResponse = 0;
        *pReason   =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
        return L7_ERROR;
    }

    /*------------------------*/
    /*     Start Application  */
    /*------------------------*/
    if (ospfStartTasks() != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        pOspfMapCfgData->cfgHdr.dataChanged = L7_FALSE;
        return L7_ERROR;
    }

      /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
    ospfMapDebugCfgRead();
    ospfMapApplyDebugConfigData();

    ospfCnfgrStateSet(OSPFMAP_PHASE_INIT_1);

    /* Initialize the function pointers in the OSPF function table */
    ospfFuncTableInit();

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Allocate space for OSPF info structures.
*
* @param    void
*
* @returns  L7_SUCCESS if space successfully allocated
*           L7_FAILURE if memory not available
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t ospfMapInfoAllocate(void)
{
    pOspfInfo = (ospfInfo_t *)osapiMalloc(L7_OSPF_MAP_COMPONENT_ID, sizeof(ospfInfo_t));
    if (pOspfInfo == L7_NULLPTR)
    {
        OSPFMAP_ERROR("Error allocating memory for OSPF Info structure.\n");
        return L7_FAILURE;
    }
    memset(( void * )pOspfInfo, 0, sizeof( ospfInfo_t));

    pOspfAreaInfo = (ospfAreaInfo_t *)osapiMalloc(L7_OSPF_MAP_COMPONENT_ID, sizeof(ospfAreaInfo_t)* L7_OSPF_MAX_AREAS);
    if (pOspfAreaInfo == L7_NULLPTR)
    {
        OSPFMAP_ERROR("Error allocating memory for area info structure\n");
        return L7_FAILURE;
    }
    memset(( void * )pOspfAreaInfo, 0, sizeof( ospfAreaInfo_t) * L7_OSPF_MAX_AREAS);

    pOspfVlinkInfo = (ospfVlinkInfo_t *)osapiMalloc(L7_OSPF_MAP_COMPONENT_ID, sizeof(ospfVlinkInfo_t) *
                                                    L7_OSPF_MAX_VIRT_NBRS);
    if (pOspfVlinkInfo == L7_NULLPTR)
    {
        OSPFMAP_ERROR("Error allocating memory for OSPF virtual link info structure.\n");
        return L7_FAILURE;
    }
    memset(( void * )pOspfVlinkInfo, 0,
           sizeof( ospfVlinkInfo_t) * L7_OSPF_MAX_VIRT_NBRS);


    pOspfIntfInfo = (ospfIntfInfo_t *)osapiMalloc(L7_OSPF_MAP_COMPONENT_ID, sizeof(ospfIntfInfo_t) *
                                                  (L7_MAX_INTERFACE_COUNT + 1));
    if (pOspfIntfInfo == L7_NULLPTR)
    {
        OSPFMAP_ERROR("Error allocating memory for OSPF interface info structure.\n");
        return L7_FAILURE;
    }
    memset(( void * )pOspfIntfInfo, 0,
           sizeof( ospfIntfInfo_t) * (L7_MAX_INTERFACE_COUNT + 1));

    ospfMapMapTbl = osapiMalloc(L7_OSPF_MAP_COMPONENT_ID, sizeof(L7_uint32) * platIntfMaxCountGet());
    if (ospfMapMapTbl == L7_NULLPTR)
    {
        OSPFMAP_ERROR("Error allocating memory for OSPF map table.\n");
        return L7_FAILURE;
    }
    memset(ospfMapMapTbl, 0, sizeof(L7_uint32)* platIntfMaxCountGet());

    pOspfNetworkAreaInfo = (ospfNetworkAreaInfo_t *)
                           osapiMalloc(L7_OSPF_MAP_COMPONENT_ID,
                           sizeof(ospfNetworkAreaInfo_t) * NUM_NETWORK_AREA_CMDS);
    if (pOspfNetworkAreaInfo == L7_NULLPTR)
    {
        OSPFMAP_ERROR("Error allocating memory for OSPF network area structure.\n");
        return L7_FAILURE;
    }
    memset(( void * )pOspfNetworkAreaInfo, 0,
           sizeof(ospfNetworkAreaInfo_t) * NUM_NETWORK_AREA_CMDS);
    networkAreaListHead_g = L7_NULLPTR;
    networkAreaListFree_g = pOspfNetworkAreaInfo;

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Deallocate space for OSPF info structures.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t ospfMapInfoDeallocate(void)
{
    if (pOspfInfo != L7_NULLPTR)
    {
        osapiFree(L7_OSPF_MAP_COMPONENT_ID, pOspfInfo);
        pOspfInfo = L7_NULLPTR;
    }

    if (pOspfAreaInfo != L7_NULLPTR)
    {
        osapiFree(L7_OSPF_MAP_COMPONENT_ID, pOspfAreaInfo);
        pOspfAreaInfo = L7_NULLPTR;
    }

    if (pOspfVlinkInfo != L7_NULLPTR)
    {
        osapiFree(L7_OSPF_MAP_COMPONENT_ID, pOspfVlinkInfo);
        pOspfVlinkInfo = L7_NULLPTR;
    }

    if (pOspfIntfInfo != L7_NULLPTR)
    {
        osapiFree(L7_OSPF_MAP_COMPONENT_ID, pOspfIntfInfo);
        pOspfIntfInfo = L7_NULLPTR;
    }

    if (ospfMapMapTbl != L7_NULLPTR)
    {
        osapiFree(L7_OSPF_MAP_COMPONENT_ID, ospfMapMapTbl);
        ospfMapMapTbl = L7_NULLPTR;
    }

    if (pOspfNetworkAreaInfo != L7_NULLPTR)
    {
        osapiFree(L7_OSPF_MAP_COMPONENT_ID, pOspfNetworkAreaInfo);
        pOspfNetworkAreaInfo = L7_NULLPTR;
        networkAreaListHead_g = networkAreaListFree_g = L7_NULLPTR;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Perform actions for phase 2 initialization. Phase 2
*           is when components register with other components for
*           notification of interesting events.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    OSPF registers with RTO and with ACL when OSPF is enabled.
*
* @end
*********************************************************************/
static L7_RC_t ospfMapCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                              L7_CNFGR_ERR_RC_t   *pReason )
{
    L7_RC_t ospfMapRC;
    nvStoreFunctionList_t notifyFunctionList;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    ospfMapRC  = L7_SUCCESS;

    /*---------------------------*/
    /*     nvStore Registration  */
    /*---------------------------*/
    memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
    notifyFunctionList.registrar_ID   = L7_OSPF_MAP_COMPONENT_ID;
    notifyFunctionList.notifySave     = ospfSave;
    notifyFunctionList.hasDataChanged = ospfHasDataChanged;
    notifyFunctionList.resetDataChanged = ospfResetDataChanged;

    if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
    {
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        return L7_ERROR;
    }
    ospfMapDeregister.ospfMapSave = L7_FALSE;
    ospfMapDeregister.ospfMapRestore = L7_FALSE;
    ospfMapDeregister.ospfMapHasDataChanged = L7_FALSE;

    /* Register OSPF to receive notification of routing events. */
    if (ipMapRegisterRoutingEventChange(L7_IPRT_OSPF,
                                        "ospfMapRoutingEventChangeCallBack",
                                        ospfMapRoutingEventChangeCallBack) != L7_SUCCESS)
    {
        OSPFMAP_ERROR("Error registering OSPF for routing events.\n");
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        return L7_ERROR;
    }

#ifdef L7_NSF_PACKAGE
  if (ospfCkptCallbacksRegister() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_OSPF_MAP_COMPONENT_ID,
           "OSPFv2 failed to register for checkpoint service callbacks.");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }
#endif

     /* register for debug */
    ospfMapDebugRegister();

    ospfCnfgrStateSet(OSPFMAP_PHASE_INIT_2);

    return ospfMapRC;
}

/*********************************************************************
* @purpose  Perform actions for phase 3 initialization. In phase 3,
*           OSPF reads and applies its configuration.
*
* @param    warmRestart @b{(input)}   L7_TRUE if restart is warm
* @param    pResponse   @b{(output)}  Response if L7_SUCCESS.
* @param    pReason     @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    In a stack, phase 3 initialization is only done on the master unit.
*
* @end
*********************************************************************/
static L7_RC_t ospfMapCnfgrInitPhase3Process(L7_BOOL warmRestart,
                                             L7_CNFGR_RESPONSE_t *pResponse,
                                             L7_CNFGR_ERR_RC_t   *pReason )
{
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;

    pOspfInfo->lastSystemRestartWarm = warmRestart;
    if (warmRestart)
    {
      /* Tell IP MAP that OSPF intends to provide routes. */
      ipMapNsfRouteSource(NSF_PROTOCOL_OSPF);
    }

    ospfBuildDefaultConfigData(L7_OSPF_CFG_VER_CURRENT);

    /* Update the operational "network area" commands info
     * from the configuration
     * This function is also called from ospfApplyConfigData().
     *
     * We are calling this here to allow for reading the
     * preconfigured "network area" commands before
     * "router ospf" is enabled
     */
    ospfMapNetworkAreaConfigRead();

    /* defer call to ospfMapAppsInit() until after configuration has been
     * pushed and router startup events are done */

    pOspfInfo->p3InitTime = osapiUpTimeRaw();
    ospfCnfgrStateSet(OSPFMAP_PHASE_INIT_3);

    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This function undoes the actions taken during
*           phase 1 initialization.
*
* @param    void
*
* @returns  none
*
* @notes    Not used in this release.
*
* @end
*********************************************************************/
static void ospfMapCnfgrFiniPhase1Process(void)
{
   /* NOTE: Configurator will reset router, so no need to take any action.
    *       Original code retained for reference.
    *
    * ospfEndTasks();
    * ospfMapMibSupportFini();
    *
    * if (pOspfMapCfgData != L7_NULLPTR)
    * {
    *     osapiFree(L7_OSPF_MAP_COMPONENT_ID, pOspfMapCfgData);
    *     pOspfMapCfgData = L7_NULLPTR;
    * }
    * ospfMapInfoDeallocate();
    *
    * osapiSemaDelete(&ospfMapCtrl_g.cfgSema);
    *
    * ospfMapRedistListDestroy();
    *
    * ospfCnfgrStateSet(OSPFMAP_PHASE_INIT_0);
    */
}

/*********************************************************************
* @purpose  This function undoes ospfMapCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    Not used in this release.
*
* @end
*********************************************************************/
static void ospfMapCnfgrFiniPhase2Process(void)
{
   /*
    * Currently there is no real way of deregistering with some
    * components. This will be a work item for the future. For now
    * a temporary solution is to set deregister flags. In each
    * callback function, we do nothing if the deregister flag is set.
    */

   /* NOTE: Configurator will reset router, so no need to take any action.
    *       Original code retained for reference.
    *
    * ospfMapDeregister.ospfMapSave = L7_TRUE;
    * ospfMapDeregister.ospfMapRestore = L7_TRUE;
    * ospfMapDeregister.ospfMapHasDataChanged = L7_TRUE;
    *
    * nimRegisterIntfChange(L7_OSPF_MAP_COMPONENT_ID, L7_NULLPTR);
    *
    * ** We can deregister from RTO and ACL **
    * rtoCallbackDeregister(ospfMapExtenRouteCallback, RTO_BEST_ROUTE);
    * ospfMapAclCallbackUnregister(ospfMapAclCallback);
    *
    * ospfCnfgrStateSet(OSPFMAP_PHASE_INIT_1);
    */
}

/*********************************************************************
* @purpose  This function undoes ospfMapCnfgrInitPhase3Process
*
* @param    void
*
* @returns  none
*
* @notes    Not used in this release.
*
* @end
*********************************************************************/
static void ospfMapCnfgrFiniPhase3Process(void)
{
   /* NOTE: Configurator will reset router, so no need to take any action.
    *       Original code retained for reference.
    *
    * L7_CNFGR_RESPONSE_t response;
    * L7_CNFGR_ERR_RC_t   reason;
    *
    * ** this func will place ospfMapCnfgrState to WMU **
    * ospfMapCnfgrUconfigPhase2(&response, &reason);
    * ospfCnfgrStateSet(OSPFMAP_PHASE_WMU);
    */
}


/*********************************************************************
* @purpose  Serves as a no-op for configurator stages that require
*           no action for OSPF. Simply sets pResponse to L7_CNFGR_CMD_COMPLETE
*           and returns L7_SUCCESS;
*
* @param    pResponse - @b{(output)}  Always set to L7_CNFGR_CMD_COMPLETE
*
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           None.
*
* @end
*********************************************************************/
static L7_RC_t ospfMapCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t *pReason )
{
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Takes actions in response to an unconfigure phase 2 command
*           from the configurator.
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
* @notes    Disable OSPF and zero its configuration structure.
*
* @end
*********************************************************************/
static L7_RC_t ospfMapCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                          L7_CNFGR_ERR_RC_t *pReason )
{
    /* undo ospfApplyConfigData() and disable OSPF */
    ospfResetCleanUp();

    if (pOspfInfo->ospfInitialized == L7_TRUE) {
        /* Uninitialize the OSPF protocol engine */
        ospfMapOspfVendorUninit();
        pOspfInfo->ospfInitialized = L7_FALSE;
    }

    /* Zero the configuration data structure */
    memset((void*) pOspfMapCfgData, 0, sizeof(L7_ospfMapCfg_t));

    ospfCnfgrStateSet(OSPFMAP_PHASE_UNCONFIG_2);
    pOspfInfo->startupDone = L7_FALSE;
    pOspfInfo->backupElected = L7_FALSE;

    *pResponse  = L7_CNFGR_CMD_COMPLETE;
    *pReason    = 0;
    return L7_SUCCESS;
}

