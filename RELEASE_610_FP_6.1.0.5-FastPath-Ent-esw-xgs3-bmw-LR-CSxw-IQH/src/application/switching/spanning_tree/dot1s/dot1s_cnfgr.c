/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1s_cnfgr.c
*
* @purpose   Multiple Spanning tree configurator file
*
* @component dot1s
*
* @comments 
*
* @create    8/5/2003
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/

#include "dot1s_include.h"
#include "buff_api.h"

dot1sCnfgrState_t dot1sCnfgrState = DOT1S_PHASE_INIT_0;
DOT1S_BRIDGE_t                          *dot1sBridge      = L7_NULLPTR;
DOT1S_INSTANCE_INFO_t           *dot1sInstance    = L7_NULLPTR;
DOT1S_PORT_COMMON_t                     *dot1sPort        = L7_NULLPTR;
DOT1S_INSTANCE_MAP_t            *dot1sInstanceMap = L7_NULLPTR;
DOT1S_INST_VLAN_MAP_t           *dot1sInstVlanMap = L7_NULLPTR;
L7_uint32                   *dot1sMapTbl      = L7_NULLPTR;
DOT1S_PORT_STATS_t                      *dot1sPortStats   = L7_NULLPTR;

dot1sCfg_t                  *dot1sCfg         = L7_NULLPTR;
dot1sDeregister_t dot1sDeregister = {L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE};

/* dot1s/driver async task parameters */
dot1sStateCirBuf_t 				*dot1sCB = L7_NULLPTR;
void * dot1sStateSetSema = L7_NULLPTR;
osapiTimerDescr_t        *dot1sStateSetTimer = L7_NULLPTR;

extern void *dot1s_queue;
extern  DOT1S_VLAN_MASK_t           dot1sVlanMask;
extern L7_uint32 dot1sBPDUBufferPoolId;
extern dot1sDebugCfg_t dot1sDebugCfg;    /* Debug Configuration File Overlay */

extern void dot1sDebugRegister(void);
extern void dot1sDebugCfgRead(void);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for dot1s component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the dot1s comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void dot1sApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  DOT1S_MSG_t msg;

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
    if (dot1sInit(pCmdData) != L7_SUCCESS)
      dot1sInitUndo();
  } else
  {
    memset(&msg, 0x00, (L7_uint32)sizeof(msg));
        memcpy(&msg.data.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.event = dot1sCnfgr;
    osapiMessageSend(dot1s_queue, &msg, (L7_uint32)sizeof(DOT1S_MSG_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  }
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
L7_RC_t dot1sCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dot1sRC;
  
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  dot1sRC     = L7_SUCCESS;

  /* Malloc for all memory - cfg, user, intf etc */

                                
  dot1sBridge      = (DOT1S_BRIDGE_t *)osapiMalloc(L7_DOT1S_COMPONENT_ID, (L7_uint32)sizeof(DOT1S_BRIDGE_t));
  dot1sInstance    = (DOT1S_INSTANCE_INFO_t *)osapiMalloc(L7_DOT1S_COMPONENT_ID, (L7_uint32)sizeof(DOT1S_INSTANCE_INFO_t));
  dot1sPort        = (DOT1S_PORT_COMMON_t *)osapiMalloc(L7_DOT1S_COMPONENT_ID, (L7_uint32)sizeof(DOT1S_PORT_COMMON_t) 
                                                                                                        * L7_DOT1S_MAX_INTERFACE_COUNT);
  dot1sInstanceMap = (DOT1S_INSTANCE_MAP_t *)osapiMalloc(L7_DOT1S_COMPONENT_ID, (L7_uint32)sizeof(DOT1S_INSTANCE_MAP_t) 
                                                                                                                 * (L7_MAX_MULTIPLE_STP_INSTANCES+1));
  dot1sInstVlanMap = (DOT1S_INST_VLAN_MAP_t *)osapiMalloc(L7_DOT1S_COMPONENT_ID, (L7_uint32)sizeof(DOT1S_INST_VLAN_MAP_t) 
                                                                                                                  * (L7_MAX_VLAN_ID+2));
  dot1sPortStats   = (DOT1S_PORT_STATS_t *)osapiMalloc(L7_DOT1S_COMPONENT_ID, (L7_uint32)sizeof(DOT1S_PORT_STATS_t)
                                                                                                       * L7_DOT1S_MAX_INTERFACE_COUNT);
  dot1sCfg         = (dot1sCfg_t *)osapiMalloc(L7_DOT1S_COMPONENT_ID, (L7_uint32)sizeof(dot1sCfg_t));
  dot1sNotifyList  = (dot1sNotifyList_t *)osapiMalloc(L7_DOT1S_COMPONENT_ID, (L7_uint32)sizeof(dot1sNotifyList_t) 
                                                                                                          * L7_LAST_COMPONENT_ID);
  dot1sMapTbl      = (L7_uint32 *)osapiMalloc(L7_DOT1S_COMPONENT_ID, sizeof(L7_uint32) * L7_DOT1S_MAX_INTERFACE_COUNT);

  dot1sCB          = (dot1sStateCirBuf_t *)osapiMalloc(L7_DOT1S_COMPONENT_ID,  
													   ((L7_uint32)sizeof(dot1sStateCirBuf_t)*(DOT1S_STATE_SET_CB_SIZE)));
																						

  if ((dot1sBridge      == L7_NULLPTR) ||
      (dot1sInstance    == L7_NULLPTR) ||
      (dot1sPort        == L7_NULLPTR) ||
      (dot1sInstanceMap == L7_NULLPTR) ||
      (dot1sInstVlanMap == L7_NULLPTR) ||
      (dot1sPortStats   == L7_NULLPTR) ||
          (dot1sCfg         == L7_NULLPTR) ||
          (dot1sNotifyList  == L7_NULLPTR) ||
      (dot1sMapTbl      == L7_NULLPTR) ||
	  (dot1sCB 			== L7_NULLPTR))
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    dot1sRC   = L7_ERROR;
    return dot1sRC;
  }

  if (bufferPoolInit(DOT1S_MAX_BPDU_BUF_COUNT, DOT1S_MAX_BPDU_SIZE, "dot1s BPDU Bufs",
					 &dot1sBPDUBufferPoolId) != L7_SUCCESS)
  {
	LOG_MSG("%s: Error allocating large buffers\n",__FUNCTION__);
	*pResponse = 0;
	*pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
	dot1sRC   = L7_ERROR;
	return dot1sRC;
  }


  /* memset to initialize all data structures to zero */
  memset((void *)dot1sBridge, 0x00, (L7_uint32)sizeof(DOT1S_BRIDGE_t));
  memset((void *)dot1sInstance, 0x00, (L7_uint32)sizeof(DOT1S_INSTANCE_INFO_t));
  memset((void *)dot1sPort, 0x00, (L7_uint32)(sizeof(DOT1S_PORT_COMMON_t) * L7_DOT1S_MAX_INTERFACE_COUNT));
  memset((void *)dot1sInstanceMap, 0x00, ((L7_uint32)sizeof(DOT1S_INSTANCE_MAP_t) * (L7_MAX_MULTIPLE_STP_INSTANCES+1)));
  memset((void *)dot1sInstVlanMap, 0x00, ((L7_uint32)sizeof(DOT1S_INST_VLAN_MAP_t) * (L7_MAX_VLAN_ID+2)));
  memset((void *)dot1sPortStats, 0x00, ((L7_uint32)sizeof(DOT1S_PORT_STATS_t) * L7_DOT1S_MAX_INTERFACE_COUNT));
  memset((void *)dot1sCfg, 0x00, (L7_uint32)sizeof(dot1sCfg_t));
  memset((void *)dot1sNotifyList, 0x00, ((L7_uint32)sizeof(dot1sNotifyList_t) * L7_LAST_COMPONENT_ID));
  memset((void *)dot1sMapTbl, 0x00,(L7_uint32)(sizeof(L7_uint32) * L7_DOT1S_MAX_INTERFACE_COUNT));

  /* Create the Queue Sync semaphore */
  dot1sQueueSyncSemCreate();

  /*Create the state set semaphore*/
  dot1sStateSetSema = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dot1sStateSetSema == L7_NULLPTR)
  {
	LOG_MSG("%s: Error creating dot1sStateSetSema\n",__FUNCTION__);
	*pResponse = 0;
	*pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
	dot1sRC   = L7_ERROR;
	return dot1sRC;
  }

  
  dot1sCnfgrState = DOT1S_PHASE_INIT_1;

  /* Read and apply trace configuration at phase 1.
       This allows for tracing during system initialization and
       during clear config */
  dot1sDebugCfgRead();
  dot1sApplyDebugConfigData();

  /* the size of DOT1S_MST_CONFIG_ID_t is linked to DOT1S_BPDU_CONFIG_ID_LEN which is defined 
   * in the BPDU specification of IEEE 802.1Q. Thus it cannot change. 
   */
  if (DOT1S_BPDU_CONFIG_ID_LEN != (sizeof(DOT1S_MST_CONFIG_ID_t) -1))
  {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Error: The size of DOT1S_MST_CONFIG_ID_t"
                                           " struct has been changed \n");
  }

  return dot1sRC;
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
L7_RC_t dot1sCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dot1sRC, rc;
  nvStoreFunctionList_t dot1sNotifyFunctionList;
  L7_uint32 eventMask = 0;
  
  
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  dot1sRC     = L7_SUCCESS;


  memset((void *) &dot1sNotifyFunctionList, 0, sizeof(dot1sNotifyFunctionList));
  dot1sNotifyFunctionList.registrar_ID   = L7_DOT1S_COMPONENT_ID;
  dot1sNotifyFunctionList.notifySave     = dot1sSave;
  dot1sNotifyFunctionList.hasDataChanged = dot1sHasDataChanged;
  dot1sNotifyFunctionList.notifyConfigDump     = dot1sCfgDump;
  dot1sNotifyFunctionList.resetDataChanged = dot1sResetDataChanged;

  if (nvStoreRegister(dot1sNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1sRC     = L7_ERROR;

    return dot1sRC;

  }
  dot1sDeregister.dot1sSave                     = L7_FALSE;
  dot1sDeregister.dot1sRestore                  = L7_FALSE;
  dot1sDeregister.dot1sHasDataChanged           = L7_FALSE;
  dot1sDeregister.dot1sCfgDump                  = L7_FALSE;



  /* register callback with NIM for L7_UPs and L7_DOWNs */
  if (nimRegisterIntfChange(L7_DOT1S_COMPONENT_ID, dot1sIntfChangeCallBack) != L7_SUCCESS)
  {
    LOG_MSG("Unable to register for NIM Interface change callback!\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1sRC     = L7_ERROR;

    return dot1sRC;
  }
  dot1sDeregister.dot1sIntfChangeCallback       = L7_FALSE;
  eventMask = VLAN_ADD_NOTIFY | VLAN_DELETE_PENDING_NOTIFY | VLAN_DELETE_NOTIFY;
  /* register callback with DOT1Q for VLAN changes */
  if (vlanRegisterForChange(dot1sVlanChangeCallback, L7_DOT1S_COMPONENT_ID, eventMask) != L7_SUCCESS)
  {
    LOG_MSG("Unable to register for VLAN change callback!\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1sRC     = L7_ERROR;

    return dot1sRC;
  }
  dot1sDeregister.dot1sVlanChangeCallback       = L7_FALSE;

  /*register with dtl for the dot1s state query callback */
  if (dtlGenericRegistration(DTL_FAMILY_INTF_MGMT,
                                                         DTL_EVENT_PORT_DOT1S_STATE_QUERY,
                                                         (L7_FUNCPTR_t)dot1sInstStateQueryCallback) != L7_SUCCESS)
  {
        LOG_MSG("Unable to register for dot1s state callback with DTL!\n");
        *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1sRC     = L7_ERROR;

    return dot1sRC;
  }
  dot1sDeregister.dot1sInstStateQueryCallback = L7_FALSE;
  
  /*register with dtl for the dot1s state set response callback */
  if (dtlGenericRegistration(DTL_FAMILY_INTF_MGMT,
                             DTL_EVENT_PORT_DOT1S_STATE_SET_RESPONSE,
                             (L7_FUNCPTR_t)dot1sStateSetCallback) != L7_SUCCESS)
  {
        LOG_MSG("Unable to register for dot1s state callback with DTL!\n");
        *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1sRC     = L7_ERROR;

    return dot1sRC;
  }
  dot1sDeregister.dot1sStateSetResponseCallback = L7_FALSE;
  
  /*register function with sysnet to receive pdus*/
  rc = dot1sTxRxInit();
  if (rc != L7_SUCCESS)
  {
        LOG_MSG("Unable to register for dot1s state callback with Sysnet!\n");
        *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1sRC     = L7_ERROR;

    return dot1sRC;
  }
  dot1sDeregister.dot1sSysnetBpduRegister = L7_FALSE;
  
  
  /* register with stats manager */
  /*rc = dot1sStatsCreate();*/
  /* Create stats for each port as they are created
   */

  /*initilize debug*/
  (void)dot1sDebugInit();

  

  dot1sCnfgrState = DOT1S_PHASE_INIT_2;

  return dot1sRC;
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
L7_RC_t dot1sCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dot1sRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  dot1sRC     = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_DOT1S_COMPONENT_ID, DOT1S_CFG_FILENAME,
                   (L7_char8 *)dot1sCfg, sizeof(dot1sCfg_t),
                   &dot1sCfg->checkSum, DOT1S_CFG_VER_CURRENT,
                   dot1sBuildConfigData, dot1sMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1sRC     = L7_ERROR;

    return dot1sRC;
  }

  if (dot1sCopyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    dot1sRC     = L7_ERROR;

    return dot1sRC;
  }

  dot1sCnfgrState = DOT1S_PHASE_INIT_3;

  return dot1sRC;
}
/*********************************************************************
* @purpose  This function undoes dot1sCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dot1sCnfgrFiniPhase1Process()
{
  if (dot1sBridge != L7_NULLPTR)
  {
        osapiFree(L7_DOT1S_COMPONENT_ID, (void *) dot1sBridge);
  }
  
  if (dot1sInstance != L7_NULLPTR)
        osapiFree(L7_DOT1S_COMPONENT_ID, (void *)dot1sInstance);
  
  if (dot1sPort != L7_NULLPTR)
        osapiFree(L7_DOT1S_COMPONENT_ID, (void *)dot1sPort);
  
  if (dot1sInstanceMap != L7_NULLPTR)
        osapiFree(L7_DOT1S_COMPONENT_ID, (void *)dot1sInstanceMap);
  
  if (dot1sInstVlanMap != L7_NULLPTR)
        osapiFree(L7_DOT1S_COMPONENT_ID, (void *)dot1sInstVlanMap);
  
  if (dot1sPortStats != L7_NULLPTR)
        osapiFree(L7_DOT1S_COMPONENT_ID, (void *)dot1sPortStats);
  
  if (dot1sCfg != L7_NULLPTR)
        osapiFree(L7_DOT1S_COMPONENT_ID, (void *)dot1sCfg);
  
  if (dot1sNotifyList != L7_NULLPTR)
        osapiFree(L7_DOT1S_COMPONENT_ID, (void *)dot1sNotifyList);
  
  if (dot1sMapTbl != L7_NULLPTR)
    osapiFree(L7_DOT1S_COMPONENT_ID, (void *)dot1sMapTbl);
  
  if (dot1sBPDUBufferPoolId != 0)
  {
	bufferPoolDelete(dot1sBPDUBufferPoolId);
	dot1sBPDUBufferPoolId = 0;
  }

  
  dot1sQueueSyncSemDelete(); 

  dot1sInitUndo();

  dot1sCnfgrState = DOT1S_PHASE_INIT_0;

  return;
}

/*********************************************************************
* @purpose  This function undoes dot1sCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dot1sCnfgrFiniPhase2Process()
{
  /* 
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now
   * a temporary solution is set in place, where in the registered
   * function if we can execute the callback only if it's corresponding
   * member in the dot1sDeregister is set to L7_FALSE;
   */

  dot1sDeregister.dot1sSave                     = L7_TRUE;
  dot1sDeregister.dot1sRestore                  = L7_TRUE;
  dot1sDeregister.dot1sHasDataChanged           = L7_TRUE;
  dot1sDeregister.dot1sCfgDump                  = L7_TRUE;
  dot1sDeregister.dot1sIntfChangeCallback       = L7_TRUE;
  dot1sDeregister.dot1sVlanChangeCallback       = L7_TRUE;
  
  (void)dtlGenericDeregistration(DTL_FAMILY_INTF_MGMT,
                                                         DTL_EVENT_PORT_DOT1S_STATE_QUERY,
                                                         (L7_FUNCPTR_t)dot1sInstStateQueryCallback);

  (void)dtlGenericDeregistration(DTL_FAMILY_INTF_MGMT,
                                 DTL_EVENT_PORT_DOT1S_STATE_SET_RESPONSE,
                                 (L7_FUNCPTR_t)dot1sStateSetCallback);
  dot1sDeregister.dot1sSysnetBpduRegister = L7_TRUE;
  dot1sDeregister.dot1sInstStateQueryCallback   = L7_TRUE;
  dot1sDeregister.dot1sStateSetResponseCallback = L7_TRUE;

  dot1sCnfgrState = DOT1S_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes policyCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dot1sCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place policyCnfgrState to WMU */
  dot1sCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t dot1sCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dot1sRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(dot1sRC);
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

L7_RC_t dot1sCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dot1sRC, rc;
  L7_uint32 i;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  dot1sRC     = L7_SUCCESS;
  rc          = L7_SUCCESS;

  /* In Unconfig pahse 2 need to delete all the instances that have been
   * created. Interfaces will be taken care of by card manager and dot3ad
   * when they will be detached and deleted as a part of unconfigure phase 1
   * and the dot1sIhProcessIntfChange will do the unconfiguration for interfaces.
   * So dot1s is responsible only for the instances. Do not delete the CIST as 
   * the driver cannot delete the CIST 
   */ 
  for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    if (dot1sInstanceMap[i].inUse == L7_TRUE &&
                dot1sInstanceMap[i].instNumber != DOT1S_CIST_ID)
        {
          rc = dot1sInstanceDelete(dot1sInstanceMap[i].instNumber);
          if (rc != L7_SUCCESS)
          {
                break;
          }
        }
  }
  if (rc != L7_SUCCESS)
  {
        LOG_MSG("Unable to delete MSTP Instance %u in the hardware", i);
        *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
        dot1sRC = L7_ERROR;
  }
  
  memset(dot1sCfg, 0, sizeof(dot1sCfg_t));

  dot1sCnfgrState = DOT1S_PHASE_WMU;

  return dot1sRC;
}

/*********************************************************************
*
* @purpose  To parse the configurator commands send to dot1sTask
*
* @param    none
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void dot1sCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             dot1sRC = L7_ERROR;
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
                if ((dot1sRC = dot1sCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  dot1sCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((dot1sRC = dot1sCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  dot1sCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((dot1sRC = dot1sCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  dot1sCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                dot1sRC = dot1sCnfgrNoopProccess( &response, &reason );
                dot1sCnfgrState = DOT1S_PHASE_WMU;
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
                dot1sCnfgrState = DOT1S_PHASE_EXECUTE;
                                dot1sCnfgrInstApply();
                                dot1sRC  = L7_SUCCESS;
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
                dot1sRC = dot1sCnfgrNoopProccess( &response, &reason );
                dot1sCnfgrState = DOT1S_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                dot1sRC = dot1sCnfgrUconfigPhase2( &response, &reason );
                dot1sCnfgrState = DOT1S_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            dot1sRC = dot1sCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = dot1sRC;
  if (dot1sRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);


  return;
}
/*********************************************************************
*
* @purpose  To parse the configurator commands send to dot1sTask
*
* @param    none
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1sCfgDump()
{

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  To parse the configurator commands send to dot1sTask
*
* @param    none
*                            
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void dot1sCnfgrInstApply()
{
  L7_RC_t rc;
  L7_uint32 instIndex, index, vlanId, instId;
  
  /* Create all the configured instances and enable global spanning tree */
  dot1sInstanceMap[DOT1S_CIST_INDEX].inUse = L7_TRUE;
  dot1sInstanceMap[DOT1S_CIST_INDEX].instNumber = DOT1S_CIST_ID;
  rc = dot1sIhDoNotifyInstanceCreate(DOT1S_CIST_ID);
  for (vlanId = 1; vlanId < L7_DOT1Q_MAX_VLAN_ID + 2; vlanId++)
  {
    
        if (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask,vlanId) &&
                dot1sCfg->cfg.dot1sInstVlanMap[vlanId].instNumber == (L7_ushort16)DOT1S_CIST_ID)
    {
      rc = dot1sApplyCfgInstanceVlanAdd(DOT1S_CIST_ID, vlanId);
    }
  }
  /* Loop and create all configured instances and map the vlans. */
  for (instIndex = 1; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    if ((dot1sCfg->cfg.dot1sInstanceMap[instIndex].inUse == L7_TRUE) &&
        (dot1sInstanceAvailIndexFind(&index) == L7_SUCCESS))
    {
      /* Apply the stored configuration information for this instance into
       * the next available index.
       */
      
      instId = dot1sCfg->cfg.dot1sInstanceMap[instIndex].instNumber;
      rc = dot1sApplyCfgInstanceCreate(instId);
          
      for (vlanId = 1; vlanId < L7_DOT1Q_MAX_VLAN_ID + 2; vlanId++)
      {
        if (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask,vlanId) && 
                        dot1sCfg->cfg.dot1sInstVlanMap[vlanId].instNumber == (L7_ushort16)instId)
        {
          rc = dot1sApplyCfgInstanceVlanAdd(instId, vlanId);
                }
      }
    }
  }

  /* Request to regenerate the MSTP Configuration Identifier now that
   * the vlan map is populated with the configured values.
   */ 
  
  rc = dot1sMstConfigIdCompute();
  dot1sBridge->enabledPortCount = 0;
  /* Set mode to the opposite of the saved configured value 
   *  */
  if (dot1sCfg->cfg.dot1sBridge.Mode != L7_ENABLE)
  {
    dot1sCfg->cfg.dot1sBridge.Mode = !dot1sCfg->cfg.dot1sBridge.Mode;
        rc = dot1sSwitchModeEnableSet(L7_ENABLE);
  }
  else
  {
    dot1sCfg->cfg.dot1sBridge.Mode = !dot1sCfg->cfg.dot1sBridge.Mode;
        rc = dot1sSwitchModeDisableSet(L7_DISABLE);
  }

  /* Applied the configured values using routines that will set the dataChanged
   * variable to true, set it to false to ensure that a duplicate config is not
   * written out.
   */
  dot1sCfg->hdr.dataChanged = L7_FALSE;
}


/*********************************************************************
*
* @purpose  
*
* @param    none
*                            
* @returns  
*
* @notes    given a intIfNum return the cfg structure pointer 
*
* @end
*********************************************************************/
DOT1S_PORT_COMMON_CFG_t *dot1sIntfCfgEntryGet(L7_uint32 intIfNum)
{
  L7_uint32 i;
  nimConfigID_t nullConfigId;
  nimConfigID_t configId;

  memset((void *)&nullConfigId, 0x00, (L7_uint32)sizeof(nimConfigID_t));
  
  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
          return L7_NULLPTR;

  for (i = 1; i < L7_DOT1S_MAX_INTERFACE_COUNT; i++)
  {
        if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(dot1sCfg->cfg.dot1sCfgPort[i].configId)))
        {
          return &dot1sCfg->cfg.dot1sCfgPort[i];
        }
  }
  return L7_NULLPTR;
}
