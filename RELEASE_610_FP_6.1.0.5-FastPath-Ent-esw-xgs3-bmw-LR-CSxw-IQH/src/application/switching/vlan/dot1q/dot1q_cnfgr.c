/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q_cnfgr.c
*
* @purpose Contains definitions to support the configurator API
*
* @component dot1q
*
* @comments none
*
* @create 07/21/2003
*
* @author djohnson
*
* @end
*             
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "dot1q.h"
#include "dot1q_intf.h"
#include "dot1q_data.h"
#include "dot1q_control.h"
#include "dot1q_cfg.h"
#include "dot1q_cnfgr.h"
#include "dot1q_outcalls.h"
#include "dot1q_migrate.h"
#include "dot1q_sid.h"
#include "dot1q_debug.h"
#include "osapi.h"
#include "l7_product.h"

extern dot1qCfgData_t      *dot1qCfg;
extern dot1qIntfData_t     *dot1qQports;
extern dot1qInfo_t         *dot1qInfo;
extern vlanTree_t          *pVlanTree;
extern vlanTreeTables_t    *pVlanTreeHeap;
extern dot1q_vlan_dataEntry_t *pVlanDataHeap;
extern L7_uint32           *dot1qMapTbl;
extern L7_uint32           *dot1qVlanCfgMapTbl;
extern dot1qVlanIntf_t     dot1qVlanIntf[DOT1Q_MAX_VLAN_INTF + 1];
extern L7_uint32 dot1qEventsList[DOT1Q_MAX_EVENTS];

extern void * dot1qSem;
void * dot1qVlanCountSem = L7_NULLPTR;

extern dot1qVidList_t      *pVidList;


dot1qCnfgrState_t   dot1qCnfgrState; 
osapiRWLock_t       dot1qCfgRWLock;
void *dot1qMsgQueue = L7_NULLPTR;

static L7_uint32 dot1qTaskId = 0;


/*********************************************************************
*
* @purpose  Initialize the dot1q task and create message queue
*
* @param    None   
*               
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    
*           
*
* @end
*********************************************************************/
L7_RC_t dot1qInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
    DOT1Q_MSG_t msg;

    dot1qMsgQueue = (void *)osapiMsgQueueCreate("dot1qMsgQueue",DOT1Q_MSG_COUNT , (L7_uint32)sizeof(DOT1Q_MSG_t));
    if (dot1qMsgQueue == L7_NULLPTR)
    {
      LOG_MSG("dot1qInit: msgQueue creation error.\n");
      return L7_FAILURE;
    }

    /* create dot1qTask - to service dot1q message queue */
    dot1qTaskId = (L7_uint32)osapiTaskCreate("dot1qTask", (void *)dot1qTask, 0, 0,
                                           dot1qSidTaskStackSizeGet(),
                                           dot1qSidTaskPriorityGet(),
                                           dot1qSidTaskSliceGet());

    if (dot1qTaskId == L7_ERROR)
    {
      LOG_MSG("Failed to create dot1q task\n");
      return L7_FAILURE;
    }

	memset(dot1qEventsList,0x0,sizeof(dot1qEventsList));


    memcpy(&msg.data.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.event = dot1q_cnfgr;

    if (osapiWaitForTaskInit (L7_DOT1Q_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Unable to initialize dot1s_task()\n");
      /*cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      */
      return L7_FAILURE;
    }

    dot1qIssueCmd(&msg);


    return L7_SUCCESS;


} 

/*********************************************************************
*
* @purpose  System Init Undo for Dot1Q component
*
* @param    none
*                            
* @notes    none
*
* @end
*********************************************************************/
void dot1qInitUndo()
{
  if (dot1qMsgQueue != L7_NULLPTR)
    osapiMsgQueueDelete(dot1qMsgQueue);

  if (dot1qTaskId != L7_ERROR)
    osapiTaskDelete(dot1qTaskId);

  dot1qCnfgrState = DOT1Q_PHASE_INIT_0;
}

/*********************************************************************
*
* @purpose  VLAN configurator event handler
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the Card Manager.  This function is re-entrant.
*
* @end
*********************************************************************/
void dot1qApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
   DOT1Q_MSG_t msg;
   L7_RC_t rc;

   rc=L7_FAILURE; 
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
      if (dot1qInit(pCmdData) != L7_SUCCESS)
      {
          dot1qInitUndo();
          /* respond to the cnfgr with error */
      }
    } else
    {
      memset(&msg, 0x00, (L7_uint32)sizeof(msg));
          memcpy(&msg.data.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
      msg.event = dot1q_cnfgr;
      rc=osapiMessageSend(dot1qMsgQueue, &msg, (L7_uint32)sizeof(DOT1Q_MSG_t), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);
      if(rc!=L7_SUCCESS)
      {
        LOG_MSG("DOT1Q: Failed to put comamnd : %d configurator message in queue.",pCmdData->command);
      }
    }



     /*cnfgrApiCallback(&cbData);*/

  return;
}





/*********************************************************************
*
* @purpose  VLAN configurator event handler
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the Card Manager.  This function is re-entrant.
*
* @end
*********************************************************************/
void dot1qParseCnfgr(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CB_DATA_t cbData;
  L7_CNFGR_ERR_RC_t  reason = 0;
  L7_RC_t rc;

  bzero((char *) &cbData, sizeof(cbData));
  rc = L7_SUCCESS;  /* claim victory until proven otherwise */
  
  if (pCmdData == L7_NULLPTR)
  {
    rc = L7_ERROR;
    reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
	return;
  }

  if (pCmdData->type == L7_CNFGR_RQST)  /* we only process configurator requests, anything else it an error */
  {
    switch (pCmdData->command)
    {
    case L7_CNFGR_CMD_INITIALIZE:
      switch (pCmdData->u.rqstData.request)
      {
        case L7_CNFGR_RQST_I_PHASE1_START:
          if ( (rc = dot1qCnfgrInitPhase1Process(pCmdData)) != L7_SUCCESS)
          {
            dot1qCnfgrFiniPhase1Process();
          }
          break;
        case L7_CNFGR_RQST_I_PHASE2_START:
          if ( (rc = dot1qCnfgrInitPhase2Process(pCmdData)) != L7_SUCCESS)
          {
            dot1qCnfgrFiniPhase2Process();
          }
          break;
        case L7_CNFGR_RQST_I_PHASE3_START:
          if ((rc = dot1qCnfgrInitPhase3Process(pCmdData)) != L7_SUCCESS)
          {
            dot1qCnfgrFiniPhase3Process();
          }
          break;
        case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
          dot1qCnfgrNoopProcess();
          dot1qCnfgrState = DOT1Q_PHASE_WMU;
          break;
        default:
          rc = L7_ERROR;
          reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
          break;
      }
      break;
    
    case L7_CNFGR_CMD_EXECUTE:
      switch (pCmdData->u.rqstData.request)
      {
      case L7_CNFGR_RQST_E_START:
        dot1qApplyConfigData();
        dot1qVlanEnterSteadyState(pCmdData);
          /* since at the begining of each session the data changed flag must be false */
          dot1qCfg->hdr.dataChanged = L7_FALSE;
          dot1qCnfgrState = DOT1Q_PHASE_EXECUTE;
          break;
        
        default:
          rc = L7_ERROR;
          reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
          break;
      }
      break;
    
    case L7_CNFGR_CMD_UNCONFIGURE:
      switch (pCmdData->u.rqstData.request) 
      {
        case L7_CNFGR_RQST_U_PHASE1_START:
          dot1qVlanUnCfgPhase1(pCmdData);
          break;

        case L7_CNFGR_RQST_U_PHASE2_START:
          rc = dot1qVlanUnCfgPhase2(pCmdData);
          break;

        default:
          rc = L7_ERROR;
          reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
          break;
      }
      break;

    case L7_CNFGR_CMD_TERMINATE:
      switch (pCmdData->u.rqstData.request)
      {
        case L7_CNFGR_RQST_T_START:
          dot1qVlanTerminate(pCmdData);
          break;
        
        default:
          rc = L7_ERROR;
          reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
          break;
      }
      break;

    case L7_CNFGR_CMD_SUSPEND:
      switch (pCmdData->u.rqstData.request)
      {
        case L7_CNFGR_RQST_S_START:
          dot1qVlanSuspend(pCmdData);
          break;

        default:
          rc = L7_ERROR;
          reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
          break;
      }
      break;
    
    case L7_CNFGR_CMD_RESUME:
      switch (pCmdData->u.rqstData.request)
      {
      case L7_CNFGR_RQST_R_START:
        dot1qVlanResume(pCmdData);
        break;

      default:
        rc = L7_ERROR;
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
        break;
      }
      break;
    
    default:
      rc = L7_ERROR;
      reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      break;
    }
  }
  else  /* api called with something other than a request */
  {
    rc = L7_ERROR;
    reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
  }
    
  cbData.correlator = pCmdData->correlator;
  cbData.asyncResponse.rc = rc;
  if (rc == L7_SUCCESS) 
  {
      cbData.asyncResponse.u.response = L7_CNFGR_CMD_COMPLETE;
  }
  else
  {
      cbData.asyncResponse.u.reason = reason;
  }

  cnfgrApiCallback(&cbData);

  return;
}



/*********************************************************************
*
* @purpose  Perform all Phase 1 initialization for the DOT1Q component.
*
* @param    pCmdData  pointer to a configurator request control block structure
*
* @returns  L7_SUCCESS, if DOT1Q Phase 1 init was successful
* @returns  L7_FAILURE, if DOT1Q Phase 1 was not successful
*
* @comments none
*       
* @end
*
*********************************************************************/
L7_RC_t dot1qCnfgrInitPhase1Process(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc = L7_SUCCESS;

  /* create semaphores */

  /* controls write requests to the VLAN component */
  dot1qSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dot1qSem == L7_NULLPTR)
  {
      LOG_MSG("Error creating Dot1q Semaphore \n");
      return L7_FAILURE;
  }

  /* controls write requests to the current VLAN count variable */
  dot1qVlanCountSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dot1qVlanCountSem == L7_NULLPTR)
  {
      LOG_MSG("Error creating dot1qVlanCount Semaphore \n");
      return L7_FAILURE;
  }

  /* Read write lock for controlling API configuration */
  if (osapiRWLockCreate(&dot1qCfgRWLock,
                        OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
      LOG_MSG("Error creating dot1qCfgRWlock semaphore \n");
      return L7_FAILURE;
  }

  /* Create the Queue Sync semaphore */
  dot1qQueueSyncSemCreate();
  /* Vlan intf initialization */
   bzero((char *)&dot1qVlanIntf, (sizeof(dot1qVlanIntf_t) * (DOT1Q_MAX_VLAN_INTF + 1)));

  /* controls write requests to the VLAN component */
  
  /* allocate memory */
  pVlanTreeHeap = (vlanTreeTables_t*)osapiMalloc( L7_DOT1Q_COMPONENT_ID, sizeof(vlanTreeTables_t) * (L7_MAX_VLANS+1) );
  pVlanDataHeap = (dot1q_vlan_dataEntry_t*)osapiMalloc( L7_DOT1Q_COMPONENT_ID, sizeof(dot1q_vlan_dataEntry_t) * (L7_MAX_VLANS+1) );
  pVlanTree     = (vlanTree_t*)osapiMalloc( L7_DOT1Q_COMPONENT_ID, sizeof(vlanTree_t) );
  dot1qQports   = (dot1qIntfData_t*)osapiMalloc( L7_DOT1Q_COMPONENT_ID, sizeof(dot1qIntfData_t) * (L7_MAX_INTERFACE_COUNT+1) );
  pVidList      = (dot1qVidList_t*)osapiMalloc( L7_DOT1Q_COMPONENT_ID, (sizeof(dot1qVidList_t)*(L7_MAX_VLANS+1) ) );
  dot1qCfg      = (dot1qCfgData_t*)osapiMalloc( L7_DOT1Q_COMPONENT_ID, sizeof(dot1qCfgData_t) );
  dot1qInfo     = (dot1qInfo_t*)osapiMalloc( L7_DOT1Q_COMPONENT_ID, sizeof(dot1qInfo_t) );
  dot1qMapTbl   = (L7_uint32*)osapiMalloc( L7_DOT1Q_COMPONENT_ID, sizeof(L7_uint32)*L7_MAX_INTERFACE_COUNT );
  dot1qVlanCfgMapTbl = (L7_uint32*)osapiMalloc( L7_DOT1Q_COMPONENT_ID, sizeof(L7_uint32)*(L7_DOT1Q_MAX_VLAN_ID +1));
                                                 

  if ( (pVlanTreeHeap == L7_NULLPTR) || 
       (pVlanDataHeap == L7_NULLPTR) ||
       (pVlanTree == L7_NULLPTR) ||
       (dot1qQports == L7_NULLPTR) ||
       (pVidList == L7_NULLPTR) ||
       (dot1qCfg == L7_NULLPTR) ||
       (dot1qInfo == L7_NULLPTR) ||
       (dot1qVlanCfgMapTbl == L7_NULLPTR) ||
       (dot1qMapTbl == L7_NULLPTR) )
    return L7_FAILURE;
                         
  /* initialize memory */
  bzero((L7_uchar8 *)dot1qInfo,    sizeof(dot1qInfo_t));
  bzero((L7_uchar8 *)dot1qMapTbl,  sizeof(L7_uint32)*L7_MAX_INTERFACE_COUNT);
  bzero((L7_uchar8 *)dot1qVlanCfgMapTbl,  sizeof(L7_uint32)*(L7_DOT1Q_MAX_VLAN_ID+1));
  bzero((L7_uchar8 *)dot1qQports,  sizeof(dot1qQports)*L7_MAX_INTERFACE_COUNT);
  

/*
  pVlanTree->semId = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if (L7_NULLPTR == pVlanTree->semId) 
  {
      return L7_FAILURE;
  }
*/
  /* create tree for runtime VLAN data */
  vlanTreeCreate(pVlanTree, pVlanTreeHeap, pVlanDataHeap, L7_MAX_VLANS+1,
                   (L7_uint32)sizeof(dot1q_vlan_dataEntry_t), 0, (L7_uint32)sizeof(L7_uint32));
  (void)avlSetAvlTreeComparator(pVlanTree, avlCompareULong32);

  dot1qDebugInit();
  
  return rc;
}

/*********************************************************************
*
* @purpose  Perform all Phase 2 initialization for the DOT1Q component.
*
* @param    pCmdData  pointer to a configurator request control block structure
*
* @returns  L7_SUCCESS, if DOT1Q Phase 2 init was successful
* @returns  L7_FAILURE, if DOT1Q Phase 2 was not successful
*
* @comments Upon completion, ready to receive port create events
*       
* @end
*
*********************************************************************/
L7_RC_t dot1qCnfgrInitPhase2Process(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc = L7_SUCCESS;
  nvStoreFunctionList_t notifyFunctionList;
  L7_uint32 eventMask = 0;
  
  /* register with nvstore */
  notifyFunctionList.registrar_ID   = L7_DOT1Q_COMPONENT_ID;
  notifyFunctionList.notifySave     = dot1qSave;
  notifyFunctionList.hasDataChanged = dot1qHasDataChanged;
  notifyFunctionList.notifyConfigDump = L7_NULLPTR;
  notifyFunctionList.notifyDebugDump  = L7_NULLPTR;
  notifyFunctionList.resetDataChanged = dot1qResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
    return L7_FAILURE;
  
  /* register callback to support LAG changes */
  if (nimRegisterIntfChange(L7_DOT1Q_COMPONENT_ID, dot1qIntfChangeCallback) != L7_SUCCESS)
  {
    L7_assert(1);
    LOG_MSG("DOT1Q: Unable to register with NIM\n");
  }

  /* register with DTL for VLAN membership */
  if (dtlGenericRegistration(DTL_FAMILY_QVLAN_MGMT, DTL_EVENT_QVLAN_MEMBER_QUERY,
                                (L7_FUNCPTR_t)(dot1qVIDListGet)) != L7_SUCCESS)
  {
    L7_assert(1);
    LOG_MSG("DOT1Q: Unable to register with DTL\n");
  }
  eventMask = VLAN_ADD_NOTIFY | VLAN_DELETE_NOTIFY | VLAN_ADD_PORT_NOTIFY | VLAN_DELETE_PORT_NOTIFY;
  if (vlanRegisterForChange(dot1qVlanIntfVlanChange, L7_DOT1Q_COMPONENT_ID, eventMask) != L7_SUCCESS)
  {
    LOG_MSG("Unable to register for VLAN change callback!\n");
    rc     = L7_ERROR;

    return rc;
  }  
  dot1qCnfgrState = DOT1Q_PHASE_INIT_2;

  return rc;
}

/*********************************************************************
*
* @purpose  Perform all Phase 3 initialization for the DOT1Q component.
*
* @param    pCmdData  pointer to a configurator request control block structure
*
* @returns  L7_SUCCESS, if DOT1Q Phase 3 init was successful
* @returns  L7_FAILURE, if DOT1Q Phase 3 was not successful
*
* @comments none
*       
* @end
*
*********************************************************************/
L7_RC_t dot1qCnfgrInitPhase3Process(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc;


  /* Initialize operational data structures on each entry into P3 init.
     This ensures that the state of dot1q is the same after a clear config
     as it is upon system reboot */

  memset((L7_uchar8 *)dot1qInfo,  0,  sizeof(dot1qInfo_t));
  memset((L7_uchar8 *)dot1qMapTbl, 0, sizeof(L7_uint32)*L7_MAX_INTERFACE_COUNT);
  memset((L7_uchar8 *)dot1qQports, 0, sizeof(dot1qQports)*L7_MAX_INTERFACE_COUNT);

  /*  An uninitialized value of a dot1qVlanCfgMapTbl entry must be outside of the
      range of 0-L7_MAX_VLANS, since 0 can be a valid VLAN cfg index */
  memset((L7_uchar8 *)dot1qVlanCfgMapTbl, 0xFFFFFFFF, sizeof(L7_uint32)*(L7_DOT1Q_MAX_VLAN_ID+1));

  dot1qCnfgrState = DOT1Q_PHASE_INIT_3;


  /* nvstore, register, */

  rc = sysapiCfgFileGet(L7_DOT1Q_COMPONENT_ID, DOT1Q_CFG_FILENAME, (L7_char8*)dot1qCfg, (L7_uint32)sizeof(dot1qCfgData_t),
                        &dot1qCfg->checkSum, DOT1Q_CFG_VER_CURRENT, dot1qBuildDefaultConfigData, dot1qMigrateConfigData);
  

  dot1qCfg->hdr.dataChanged = L7_FALSE;

  /* Initialize operational data */

  dot1qInfo->MaxVLANID          = L7_DOT1Q_MAX_VLAN_ID;
  dot1qInfo->MaxNumVlans        = L7_MAX_VLANS;
  dot1qInfo->deviceCapabilities = 0x72;
  dot1qInfo->version            = 1;
  dot1qInfo->TrafficClassesEnabled = L7_TRUE;
  dot1qInfo->initialized        = L7_TRUE;

  

  
  /* register stats */
  dot1qStatsSystemCreate();

  dot1qCnfgrState = DOT1Q_PHASE_INIT_3;
  
  return rc;
}

/*********************************************************************
* @purpose  Release all resources collected during phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dot1qCnfgrFiniPhase1Process(void)
{
  if (pVlanTreeHeap != L7_NULLPTR)
    osapiFree(L7_DOT1Q_COMPONENT_ID, pVlanTreeHeap);

  if (pVlanDataHeap != L7_NULLPTR)
    osapiFree(L7_DOT1Q_COMPONENT_ID, pVlanDataHeap);
/*
  if (pVlanTree->semId != L7_NULLPTR)
    osapiSemaDelete(pVlanTree->semId);
*/
  if (pVlanTree != L7_NULLPTR)
    osapiFree(L7_DOT1Q_COMPONENT_ID, pVlanTree);
  
  
  if (dot1qQports != L7_NULLPTR)
    osapiFree(L7_DOT1Q_COMPONENT_ID, dot1qQports);
  
  if (pVidList != L7_NULLPTR)
    osapiFree(L7_DOT1Q_COMPONENT_ID, pVidList);
  
  if (dot1qCfg != L7_NULLPTR)
    osapiFree(L7_DOT1Q_COMPONENT_ID, dot1qCfg);
  
  
  if (dot1qInfo != L7_NULLPTR)
    osapiFree(L7_DOT1Q_COMPONENT_ID, dot1qInfo);

  
  if (dot1qMapTbl != L7_NULLPTR)
    osapiFree(L7_DOT1Q_COMPONENT_ID, dot1qMapTbl);

  
  if (dot1qVlanCfgMapTbl != L7_NULLPTR)
    osapiFree(L7_DOT1Q_COMPONENT_ID, dot1qVlanCfgMapTbl);

  /* delete dotqQueueSync semaphore*/
  dot1qQueueSyncSemDelete();


  (void)osapiRWLockDelete(dot1qCfgRWLock);

  if (dot1qSem != L7_NULLPTR)
    osapiSemaDelete(dot1qSem);

  if (dot1qVlanCountSem != L7_NULLPTR)
    osapiSemaDelete(dot1qVlanCountSem);


  dot1qCnfgrState = DOT1Q_PHASE_INIT_0;

  return;
}

/*********************************************************************
* @purpose  Release all resources collected during phase 2
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dot1qCnfgrFiniPhase2Process(void)
{
  dot1qCnfgrState = DOT1Q_PHASE_INIT_1;
  return;
}

/*********************************************************************
* @purpose  Release all resources collected during phase 3
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dot1qCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_CMD_DATA_t cmdData;
  
  dot1qVlanUnCfgPhase2(&cmdData);
}

/* ready to receive interface creates */
void dot1qVlanEnterSteadyState(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  dot1qNotifyData_t vlanData;

  vlanData.numVlans = 1;
  vlanData.data.vlanId = DOT1Q_NULL_VLAN;

  dot1qVlanIntfCreateInterfaces();
  vlanNotifyRegisteredUsers(&vlanData, L7_NULL, VLAN_INITIALIZED_NOTIFY);
  
  
}

void dot1qVlanTerminate(L7_CNFGR_CMD_DATA_t *pCmdData)
{

}

void dot1qVlanSuspend(L7_CNFGR_CMD_DATA_t *pCmdData)
{

}

void dot1qVlanResume(L7_CNFGR_CMD_DATA_t *pCmdData)
{

}

/*********************************************************************
* @purpose  Unconfig phase 1
*
* @param    void
*
* @returns  void
*
* @notes    In this phase, interfaces are detached and deleted
*
* @end
*********************************************************************/
void dot1qVlanUnCfgPhase1(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* Delete any configured VLAN interfaces */
  dot1qVlanIntfDeleteInterfaces();

  dot1qCnfgrState = DOT1Q_PHASE_UNCONFIG_1;
}

/*********************************************************************
* @purpose  Unconfig phase 2
*
* @param    void
*
* @returns  void
*
* @notes    In this phase, the non-interface operational config is
*           cleared and default config applied
*
* @end
*********************************************************************/
L7_RC_t dot1qVlanUnCfgPhase2(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc;
  
  /* this special state tells dot1qCnfgrStateCheck() that U1
     has completed _system wide_, restricting all NIM interface calls.
     
     dot1q can and does make nim calls after U1 has completed locally,
     but not system wide (while processing detaches on dot1qIntfChangeCallback */
  
  dot1qCnfgrState = DOT1Q_PHASE_UNCONFIG_2_IN_PROGRESS;
  
  rc = dot1qRestore();
  
  dot1qCnfgrState = DOT1Q_PHASE_UNCONFIG_2;
  
  return rc;
}

L7_RC_t dot1qCnfgrNoopProcess(void)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check whether dot1q is ready.
*
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL dot1qCnfgrStateCheck (void)
{
  if ((dot1qCnfgrState == DOT1Q_PHASE_EXECUTE) ||
	  (dot1qCnfgrState == DOT1Q_PHASE_INIT_3) ||
	  (dot1qCnfgrState == DOT1Q_PHASE_UNCONFIG_1)) 
  {
	  return L7_TRUE;
  }

  return L7_FALSE;
}


