/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename garp_cnfgr.c
*
* @purpose Contains definitions to support the configurator API
*
* @component GARP
*
* @comments none
*
* @create 08/03/2003
*
* @author djohnson
*
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "l7_common.h"
#include "usmdb_sim_api.h"
#include "osapi.h"
#include "sysapi.h"
#include "string.h"
#include "stdlib.h"
#include "log.h"
#include "nimapi.h"
#include "nvstoreapi.h"
#include "sysnet_api.h"
#include "garpapi.h"
#include "garpctlblk.h"
#include "garpcfg.h"
#include "default_cnfgr.h"
#include "dot3ad_api.h"

#include "dot1s_api.h"
#include "gipapi.h"
#include "dot1dgarp.h"
#include "dot1q_api.h"
#include "garpcfg.h"
#include "gmrp_api.h"
#include "garp_dot1q_util.h"
#include "garp_leaveall_timer.h"

L7_RC_t garpCnfgrInitPhase1Process(L7_CNFGR_CMD_DATA_t *pCmdData);
void garpCnfgrFiniPhase1Process(void);
L7_RC_t garpCnfgrInitPhase2Process(L7_CNFGR_CMD_DATA_t *pCmdData);
void garpCnfgrFiniPhase2Process(void);
L7_RC_t garpCnfgrInitPhase3Process(L7_CNFGR_CMD_DATA_t *pCmdData);
void garpCnfgrFiniPhase3Process(void);
L7_RC_t garpCnfgrNoopProcess(void);
void garpEnterSteadyState(L7_CNFGR_CMD_DATA_t *pCmdData);
void garpUnCfgPhase1(L7_CNFGR_CMD_DATA_t *pCmdData);
void garpUnCfgPhase2(L7_CNFGR_CMD_DATA_t *pCmdData);

extern multipleGIP_t *multipleGIP;
extern garpCfg_t  *garpCfg;
extern gvrpInfo_t *gvrpInfo;
extern gmrpInfo_t *gmrpInfo;
extern L7_uchar8 Garp_gvrp_mac_address[6];
extern L7_uchar8 Garp_gmrp_mac_address[6];
extern gvrpMac_t *gvrpMac;
extern gvrpMac_t *gmrpMac;
extern void *garpTaskSyncSema;
extern void *gmrpCfgSema;
extern gmrpInstance_t *gmrpInstance;
extern gmrpCfg_t *gmrpCfg;
extern gmrpOperCfg_t *gmrpOperCfg;
extern void *garpQueue;
extern void *garpPduQueue;
extern GarpInfo_t garpInfo;

L7_uint32 *gmrpMapTbl = L7_NULLPTR;

GARPCBptr GARPCB;

garpCnfgrState_t garpCnfgrState;
L7_uint32        *garpMapTbl;

osapiRWLock_t garpCfgRWLock;

/*********************************************************************
*
* @purpose  GARP configurator event handler
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
void garpApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
   GarpPacket msg;

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
      if (garpCnfgrInitPhase1Process(pCmdData) != L7_SUCCESS)
      {
          garpCnfgrFiniPhase1Process();
          /* respond to the cnfgr with error */
      }
    } else
    {
      memset(&msg, 0x00, (L7_uint32)sizeof(msg));
      memcpy(&msg.msgData.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
      msg.msgId = G_CNFGR;
      if (osapiMessageSend(garpQueue, &msg, (L7_uint32)sizeof(GarpPacket), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM)
          != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_GARP_COMPONENT_ID,
              "GARP: Failed to put configurator message in queue."
              " The garpQueue is full, logs specifics of the message"
              " content like internal interface number, type of message etc");
      }
      else
      {
          osapiSemaGive(garpInfo.msgQSema);
      }
    }

   return;
}



/*********************************************************************
*
* @purpose  garp configurator event handler
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
void garpParseCnfgr(L7_CNFGR_CMD_DATA_t *pCmdData)
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

            /*  NOTE:  This code path does not follow the preferred model of invoking xxxInit on the
                       configurator thread and responding on the component task thread */

          /*
          if ( (rc = garpCnfgrInitPhase1Process(pCmdData)) != L7_SUCCESS)
          {
            garpCnfgrFiniPhase1Process();
          }

          */
          break;
        case L7_CNFGR_RQST_I_PHASE2_START:
          if ( (rc = garpCnfgrInitPhase2Process(pCmdData)) != L7_SUCCESS)
          {
            garpCnfgrFiniPhase2Process();
          }
          break;
        case L7_CNFGR_RQST_I_PHASE3_START:
          if ((rc = garpCnfgrInitPhase3Process(pCmdData)) != L7_SUCCESS)
          {
            garpCnfgrFiniPhase3Process();
          }
          break;
        case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
          garpCnfgrNoopProcess();
          garpCnfgrState = GARP_PHASE_WMU;
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
          garpEnterSteadyState(pCmdData);
          garpCnfgrState = GARP_PHASE_EXECUTE;
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
          garpUnCfgPhase1(pCmdData);
          break;

        case L7_CNFGR_RQST_U_PHASE2_START:
          garpUnCfgPhase2(pCmdData);
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
          garpCnfgrNoopProcess();
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
          garpCnfgrNoopProcess();
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
        garpCnfgrNoopProcess();
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
* @purpose  Perform all Phase 1 initialization for the GARP component.
*
* @param    pCmdData  pointer to a configurator request control block structure
*
* @returns  L7_SUCCESS, if GARP Phase 1 init was successful
* @returns  L7_FAILURE, if GARP Phase 1 was not successful
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t garpCnfgrInitPhase1Process(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit, i;
  GARPStatus returnVal;
  L7_CNFGR_CB_DATA_t cbData;

  unit = usmDbThisUnitGet();

  garpCfgRWLock.handle = (void *)L7_NULL;
  if (L7_FAILURE == osapiRWLockCreate(&garpCfgRWLock,
                                      OSAPI_RWLOCK_Q_PRIORITY))
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "garpCnfgrInitPhase1Process: Unable to create garpCfgRWLock\n");

      cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cbData.asyncResponse.u.response = L7_CNFGR_CMD_COMPLETE;         /*   TBD:  Not in other failure callbacks */
      cnfgrApiCallback(&cbData);
      return L7_FAILURE;

  }

  /* create semaphores */
  gmrpCfgSema = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY,OSAPI_SEM_FULL);
  if (gmrpCfgSema == L7_NULLPTR)
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "garpCnfgrInitPhase1Process: Unable to create gmrpCfgSema\n");

      cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cbData.asyncResponse.u.response = L7_CNFGR_CMD_COMPLETE;         /*   TBD:  Not in other failure callbacks */
      cnfgrApiCallback(&cbData);
      return L7_FAILURE;
  }



  /* malloc memory */

  garpCfg  = (garpCfg_t*)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(garpCfg_t) );
  gvrpInfo = (gvrpInfo_t*)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(gvrpInfo_t) );
  gmrpInfo = (gmrpInfo_t*)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(gmrpInfo_t) );
  GARPCB = (GARPCBptr)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(GARPControlBlock) );
  multipleGIP = (multipleGIP_t*)osapiMalloc( L7_GARP_COMPONENT_ID, (sizeof(multipleGIP_t)*L7_NUM_STP_INSTANCES) );
  gvrpMac = (gvrpMac_t*)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(gvrpMac_t)*L7_MAX_INTERFACE_COUNT );
  gmrpMac = (gvrpMac_t*)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(gvrpMac_t)*L7_MAX_INTERFACE_COUNT );
  garpMapTbl = (L7_uint32*)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(L7_uint32)*L7_GARP_MAX_INTF );
  gmrpMapTbl = (L7_uint32*)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(L7_uint32)*L7_GMRP_MAX_INTF );
  gmrpInstance = (gmrpInstance_t*)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(gmrpInstance_t)*(L7_MAX_VLAN_ID + 1));
  gmrpCfg = (gmrpCfg_t*)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(gmrpCfg_t) );
  gmrpOperCfg = (gmrpOperCfg_t*)osapiMalloc( L7_GARP_COMPONENT_ID, sizeof(gmrpOperCfg_t) );

  if ( (garpCfg == L7_NULLPTR) ||
       (gvrpInfo == L7_NULLPTR) ||
       (gmrpInfo == L7_NULLPTR) ||
       (GARPCB == L7_NULLPTR ) ||
       (multipleGIP == L7_NULLPTR) ||
       (gvrpMac == L7_NULLPTR) ||
       (gmrpMac == L7_NULLPTR) ||
       (garpMapTbl == L7_NULLPTR) ||
       (gmrpMapTbl == L7_NULLPTR) ||
       (gmrpCfg == L7_NULLPTR) ||
       (gmrpOperCfg == L7_NULLPTR) ||
       (gmrpInstance == L7_NULLPTR) )
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "garpCnfgrInitPhase1Process: Unable to allocate memory\n");

      cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cbData.asyncResponse.u.response = L7_CNFGR_CMD_COMPLETE;         /*   TBD:  Not in other failure callbacks */
      cnfgrApiCallback(&cbData);
      return L7_FAILURE;
  }

  memset( (void*)garpMapTbl, 0, sizeof(L7_uint32)*L7_GARP_MAX_INTF);
  memset( (void*)gmrpMapTbl, 0, sizeof(L7_uint32)*L7_GARP_MAX_INTF);
  memset( (void*)GARPCB, 0, sizeof(GARPControlBlock) );

  for (i=0; i<L7_MAX_VLANS; i++)
  {
    gmrpInstance[i].inuse = L7_FALSE;
    gmrpInstance[i].gmr = L7_NULLPTR;
  }

  /* Initialize GARP timer lists*/
  garpTimerListInit();


  /* allocate and initialise GARP timer structure */
  /* NOTE: This initialization is used for leaveall timers only.
           The naming convention is confusing */
  garpLeaveAllTimerStructInit();

  /* Initialize the Garp Control Structure */
  GARPCB->GarpCB_garpReady   = L7_FALSE;
  GARPCB->GarpCB_numPorts    = GARP_MAX_PORT_COUNT;
  GARPCB->GarpCB_gmrpMaxVars = GARP_MAX_GMRP_COUNT;
  GARPCB->GarpCB_gvrpMaxVars = GARP_MAX_VLAN_COUNT;

  /* Max GARP Instances: 1 GMRP instance for each VLAN + 1 GVRP instance  */
  GARPCB->GarpCB_garpNumInstances = GARP_MAX_VLAN_COUNT+1;

    /* init the m gip instances */
    gipInitMultipleGIP();


  /* PTin modified: it looks arm compiler does not handle with floating point numbers... */
  /* Initialize Timers values in milliseconds  */
  GARPCB->GarpCB_leaveAllTimeCeiling = (GARPCB->GarpCB_leaveAllTime*3)/2;

  /* Initialize Application Values    */
  GARPCB->GarpCB_gvrpSupported       = L7_FALSE;
  GARPCB->GarpCB_gmrpSupported       = L7_FALSE;

  /* Initialize Function Pointers   */
  GARPCB->GarpCB_garpInitDone        = GarpInitDone;

  /* Initialize GARP timer values */
  GARPCB->GarpCB_joinTime     = (L7_uint32)DEFAULT_GARPCONFIGJOINTIME;
  GARPCB->GarpCB_leaveTime    = (L7_uint32)DEFAULT_GARPCONFIGLEAVETIME;
  GARPCB->GarpCB_leaveAllTime = (L7_uint32)DEAFULT_GARPCONFIGLEAVEALLTIME;

  GARPCB->GarpCB_initSyncSem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);

  /* Instantiate Garp */
  GARPCB->GarpCB_create = GARP_CREATE_ENTRY;
  returnVal = (*GARPCB->GarpCB_create)((void *)GARPCB);

  /* If No Error And Garp is Not Ready  */
  if (returnVal == GARP_SUCCESS && (GarpIsReady() == L7_FALSE))
  {
    /*  Wait up for GARP to init its tasks */
    returnVal = GarpWaitInitDone((L7_int32)garp_init_timeout);
  }

  /*  NOTE:  This code path does not follow the preferred model of invoking xxxInit on the
             configurator thread and responding on the component task thread */
  cbData.correlator = pCmdData->correlator;
  cbData.asyncResponse.rc = rc;
  if (rc == L7_SUCCESS)
  {
      cbData.asyncResponse.u.response = L7_CNFGR_CMD_COMPLETE;
  }

  cnfgrApiCallback(&cbData);


  return rc;
}

/*********************************************************************
*
* @purpose  Perform all Phase 2 initialization for the GARP component.
*
* @param    pCmdData  pointer to a configurator request control block structure
*
* @returns  L7_SUCCESS, if GARP Phase 2 init was successful
* @returns  L7_FAILURE, if GARP Phase 2 was not successful
*
* @comments Upon completion, ready to receive port create events
*
* @end
*
*********************************************************************/
L7_RC_t garpCnfgrInitPhase2Process(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc = L7_SUCCESS;
  nvStoreFunctionList_t notifyFunctionList;
  L7_uint32 eventMask = 0;

  /* Used to register gvrp & gmrp pdu receive with sysnet utility */
  sysnetNotifyEntry_t snEntry;

  /* register with nvstore */
  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_GARP_COMPONENT_ID;
  notifyFunctionList.notifySave     = garpSave;
  notifyFunctionList.hasDataChanged = garpHasDataChanged;
  notifyFunctionList.notifyConfigDump = GarpConfigDump;
  notifyFunctionList.resetDataChanged = garpResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
    return L7_FAILURE;

  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_GMRP_COMPONENT_ID;
  notifyFunctionList.notifySave     = gmrpSave;
  notifyFunctionList.hasDataChanged = gmrpHasDataChanged;
  notifyFunctionList.resetDataChanged = gmrpResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
    return L7_FAILURE;

  /* register w/ portmgr so that link state changes are reported */
  if (nimRegisterIntfChange(L7_GARP_COMPONENT_ID, garpIntfChangeCallback,
                            garpIntfStartupCallback, NIM_STARTUP_PRIO_DEFAULT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
            "Unable to register port manager callback\n");
  }
  else
  {
    rc = L7_SUCCESS;
  }


  if ((dot1sRegisterIntfChange(L7_GARP_COMPONENT_ID, garpDot1sChangeCallBack) != L7_SUCCESS))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
            "Unable to register MSTP callback\n");
  }

  eventMask = VLAN_ADD_NOTIFY | VLAN_ADD_PORT_NOTIFY | VLAN_DELETE_PENDING_NOTIFY |
              VLAN_DELETE_PORT_NOTIFY | VLAN_RESTORE_NOTIFY;
  /*register with vlan to get notifications for vlan create/delete and port added/deleted*/
  rc = vlanRegisterForChange(garpVlanChangeCallback, L7_GARP_COMPONENT_ID, eventMask);

  strcpy(snEntry.funcName, "GvrpRecvPkt");
  snEntry.notify_pdu_receive = GarpRecvPkt;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, Garp_gvrp_mac_address, L7_MAC_ADDR_LEN);
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
            "Unable to register Gvrp pdu receive callback\n");
    return L7_FAILURE;
  }
  strcpy(snEntry.funcName, "GmrpRecvPkt");
  snEntry.notify_pdu_receive = GarpRecvPkt;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, Garp_gmrp_mac_address, L7_MAC_ADDR_LEN);
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
            "Unable to register Gmrp pdu receive callback\n");
    return L7_FAILURE;
  }

  garpCnfgrState = GARP_PHASE_INIT_2;
  return rc;
}

/*********************************************************************
*
* @purpose  Perform all Phase 3 initialization for the GARP component.
*
* @param    pCmdData  pointer to a configurator request control block structure
*
* @returns  L7_SUCCESS, if GARP Phase 3 init was successful
* @returns  L7_FAILURE, if GARP Phase 3 was not successful
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t garpCnfgrInitPhase3Process(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc;

  /* Clear the NIM port event mask until ready to receive events */
  memset(&GARPCB->nimEventMask, 0, sizeof(GARPCB->nimEventMask));
  nimRegisterIntfEvents(L7_GARP_COMPONENT_ID, GARPCB->nimEventMask);

  rc = sysapiCfgFileGet(L7_GARP_COMPONENT_ID, GARP_CFG_FILENAME, (L7_char8*)garpCfg, (L7_uint32)sizeof(garpCfg_t),
                        &garpCfg->checkSum, GARP_CFG_VER_CURRENT, garpBuildDefaultConfigData, L7_NULL);


  /* sysapiCfgFileGet() should have already set the parms back to default config values.
     However, it does not hurt to do so again */
  if (rc == L7_FAILURE)
  {
      garpBuildDefaultConfigData (GARP_CFG_VER_CURRENT);
  }

  garpCfg->hdr.dataChanged = L7_FALSE;


  /* sysapiCfgFileGet() should have already set the parms back to default config values.
     However, it does not hurt to do so again */
  rc = sysapiCfgFileGet(L7_GMRP_COMPONENT_ID, GMRP_CFG_FILENAME, (L7_char8*)gmrpCfg, (L7_uint32)sizeof(gmrpCfg_t),
                        &gmrpCfg->checkSum, GMRP_CFG_VER_CURRENT, gmrpBuildDefaultConfigData, L7_NULL);

  if (rc == L7_FAILURE)
  {
      gmrpBuildDefaultConfigData(GARP_CFG_VER_CURRENT);
  }

  GARP_GVRP_IS_ENABLED = L7_FALSE;
  GARP_GMRP_IS_ENABLED = L7_FALSE;

  /* Reset key status structures */
  memset(&garpInfo.infoCounters,0,sizeof(garpInfo.infoCounters));

  rc = garpApplyConfigData();

  garpCnfgrState = GARP_PHASE_INIT_3;
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
void garpCnfgrFiniPhase1Process(void)
{
  if (L7_NULL != garpCfgRWLock.handle)
      osapiRWLockDelete(garpCfgRWLock);

  if (GARPCB->GarpCB_garpCCPTask != L7_ERROR)
    osapiTaskDelete(GARPCB->GarpCB_garpCCPTask);

  if (garpCfg != L7_NULLPTR)
    osapiFree(L7_GARP_COMPONENT_ID, garpCfg);

  if (gvrpInfo != L7_NULLPTR)
    osapiFree(L7_GARP_COMPONENT_ID, gvrpInfo);

  if (gmrpInfo != L7_NULLPTR)
    osapiFree(L7_GARP_COMPONENT_ID, gmrpInfo);

  if (GARPCB != L7_NULLPTR)
    osapiFree(L7_GARP_COMPONENT_ID, GARPCB);

  if (multipleGIP != L7_NULLPTR)
    osapiFree(L7_GARP_COMPONENT_ID, multipleGIP);

  if (gvrpMac != L7_NULLPTR)
    osapiFree(L7_GARP_COMPONENT_ID, gvrpMac);

  if (gmrpMac != L7_NULLPTR)
    osapiFree(L7_GARP_COMPONENT_ID, gmrpMac);

  if (garpMapTbl != L7_NULLPTR)
    osapiFree(L7_GARP_COMPONENT_ID, garpMapTbl);

  if (garpTaskSyncSema != L7_NULLPTR)
    osapiSemaDelete(garpTaskSyncSema);

  if (gmrpCfgSema != L7_NULLPTR)
    osapiSemaDelete(gmrpCfgSema);

  garpTimerListFini();

  garpCnfgrState = GARP_PHASE_INIT_0;

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
void garpCnfgrFiniPhase2Process(void)
{
  nvStoreFunctionList_t notifyFunctionList;

  (void)memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));

  (void)nvStoreRegister(notifyFunctionList);

  (void)nimRegisterIntfChange(L7_GARP_COMPONENT_ID, L7_NULLPTR, NULL, 500);

  garpCnfgrState = GARP_PHASE_INIT_1;
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
void garpCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_CMD_DATA_t cmdData;

  garpUnCfgPhase2(&cmdData);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 1.
*
* @param    pCmdData command data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void garpUnCfgPhase1(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  garpCnfgrState = GARP_PHASE_UNCONFIG_1;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pCmdData command data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void garpUnCfgPhase2(L7_CNFGR_CMD_DATA_t *pCmdData)
{
    garpRestore();

    garpCnfgrState = GARP_PHASE_UNCONFIG_2;

}
/*********************************************************************
* @purpose  NOOP
*
* @param    void
*
* @returns  L7_SUCCESS, always
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t garpCnfgrNoopProcess(void)
{
  return L7_SUCCESS;
}

void garpEnterSteadyState(L7_CNFGR_CMD_DATA_t *pCmdData)
{

}




