/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  dvmrp_map.c
*
* @purpose   DVMRP Mapping system infrastructure
*
* @component DVMRP Mapping Layer
*
* @comments  none
*
* @create    02/18/2002
*
* @author    M Pavan K Chakravarthi
*
* @end
*
**********************************************************************/
#include "l7_common_l3.h"
#include "nvstoreapi.h"
#include "l3_mcast_commdefs.h"
#include "l7_mcast_api.h"
#include "l7_dvmrp_api.h"
#include "mcast_wrap.h"
#include "dvmrp_api.h"
#include "l7_ip_api.h"
#include "l3_mcast_defaultconfig.h"
#include "l3_mcast_default_cnfgr.h"
#include "dvmrp_cnfgr.h"
#include "dvmrp_config.h"
#include "dvmrp_map_util.h"
#include "dvmrp_map.h"
#include "dvmrp_vend_exten.h"
#include "dvmrp_map_debug.h"
#include "dvmrp_vend_ctrl.h"
#include "support_api.h"

/* DVMRP mapping component globals */
dvmrpGblVaribles_t dvmrpGblVar_g;
dvmrpDebugCfg_t dvmrpDebugCfg;

static void dvmrpMapMcastEventProcess(L7_uint32 intIfNum, L7_uint32 event,
                                      void *pData, ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
static void dvmrpMapRoutingEventProcess(L7_uint32 intIfNum,
                                        L7_uint32 event, void *pData,
                                        L7_BOOL asyncRespReq,
                                        ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
static void
dvmrpMapProtoEventChangeProcess(L7_uchar8 familyType,
                                L7_uint32 eventType,
                                void *pData);

/*********************************************************************
*
* @purpose task to handle all Dvmrp Mapping messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void dvmrpMapTask()
{
  dvmrpMapMsg_t msg;
  dvmrpAppTimerMsg_t appTimerMsg;
  dvmrpMapCtrlPktAndMfcMsg_t  pktAndMfcMsg;
  L7_uint32    status = L7_FAILURE;
  L7_uint32       QIndex=0;

  osapiTaskInitDone(L7_DVMRP_MAP_TASK_SYNC);

  dvmrpGblVar_g.dvmrpQueue[DVMRP_APP_TIMER_Q].QRecvBuffer =&appTimerMsg;
  dvmrpGblVar_g.dvmrpQueue[DVMRP_EVENT_Q].QRecvBuffer     =&msg;
  dvmrpGblVar_g.dvmrpQueue[DVMRP_CTRL_PKT_Q].QRecvBuffer  =&pktAndMfcMsg;


  do
  {
    if (osapiSemaTake(dvmrpGblVar_g.msgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
             "Failure taking DVMRP message queue semaphore.");
      continue;
    }

    for(QIndex=0;QIndex < DVMRP_MAX_Q;QIndex++)
    {
       status = osapiMessageReceive(dvmrpGblVar_g.dvmrpQueue[QIndex].QPointer,
                                    dvmrpGblVar_g.dvmrpQueue[QIndex].QRecvBuffer,
                                    dvmrpGblVar_g.dvmrpQueue[QIndex].QSize,
                                    L7_NO_WAIT);
       if(status == L7_SUCCESS)
       {
          dvmrpGblVar_g.dvmrpQueue[QIndex].QCurrentMsgCnt--;
          dvmrpGblVar_g.dvmrpQueue[QIndex].QRxSuccess++;
          break;
       }
    }

    if (status == L7_SUCCESS)
    {

      switch(QIndex)
      {
         case DVMRP_APP_TIMER_Q:

           dvmrpMapProtoEventChangeProcess(appTimerMsg.dvmrpMapAppTimerEventParms.familyType,
                            appTimerMsg.dvmrpMapAppTimerEventParms.event,
                            (void *)(appTimerMsg.dvmrpMapAppTimerEventParms.
                                      timerCtrlBlk));
           break;
         case DVMRP_EVENT_Q:

           switch (msg.msgId)
           {
             case (DVMRPMAP_CNFGR_MSG):
               dvmrpCnfgrCommandProcess(&msg.u.cnfgrCmdData);
               break;

             case (DVMRPMAP_ROUTING_EVENT_MSG):
               dvmrpMapRoutingEventProcess(msg.u.dvmrpMapRoutingEventParms.intIfNum,
                                      msg.u.dvmrpMapRoutingEventParms.event,
                                      msg.u.dvmrpMapRoutingEventParms.pData,
                                      msg.u.dvmrpMapRoutingEventParms.asyncResponseRequired,
                                      &(msg.u.dvmrpMapRoutingEventParms.eventInfo));
               break;
             case (DVMRPMAP_PROTOCOL_EVENT_MSG):
               dvmrpMapProtoEventChangeProcess(msg.u.dvmrpMapProtocolEventParms.familyType,
                            msg.u.dvmrpMapProtocolEventParms.event,
                            (void *)&(msg.u.dvmrpMapProtocolEventParms.
                                      msgData));
               break;

             case (DVMRPMAP_MCAST_EVENT_MSG):
               dvmrpMapMcastEventProcess(msg.u.dvmrpMapIgmpEventParms.intIfNum,
                                    msg.u.dvmrpMapIgmpEventParms.event,
                                    msg.u.dvmrpMapIgmpEventParms.pData,
                                    &(msg.u.dvmrpMapIgmpEventParms.eventInfo));
               break;

             case (DVMRPMAP_UI_EVENT_MSG):
               dvmrpMapUIEventChangeProcess (msg.u.dvmrpMapUiEventParms.event,
                                        (void *)&(msg.u.dvmrpMapUiEventParms));
               break;

             default:
               DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "\ninvalid message type %d\n",
                          msg.msgId);
               break;
           }
           break;

         case DVMRP_CTRL_PKT_Q:
           DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"Ctrl Pkt Q Event Entry\r\n");
           if((pktAndMfcMsg.dvmrpMapCtrlPktAndMfcParms.event == MCAST_MFC_NOCACHE_EVENT)||
              (pktAndMfcMsg.dvmrpMapCtrlPktAndMfcParms.event == MCAST_MFC_WRONGIF_EVENT)||
              (pktAndMfcMsg.dvmrpMapCtrlPktAndMfcParms.event == MCAST_MFC_ENTRY_EXPIRE_EVENT))
           {
             dvmrpMapCountersUpdate (DVMRP_MAP_NUM_MFC_EVENTS, DVMRP_MAP_COUNTERS_DECREMENT);
           }
           dvmrpMapProtoEventChangeProcess(pktAndMfcMsg.dvmrpMapCtrlPktAndMfcParms.familyType,
                            pktAndMfcMsg.dvmrpMapCtrlPktAndMfcParms.event,
                            (void *)&(pktAndMfcMsg.dvmrpMapCtrlPktAndMfcParms.u));
           break;

         default:
           DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "\ninvalid Queue type %d\n",
                          QIndex);
           break;
      }
    }
  } while (1);
}

/*********************************************************************
* @purpose  Save DVMRP user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpSave(void)
{
  if (dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    dvmrpGblVar_g.DvmrpMapCfgData.checkSum = nvStoreCrc32((L7_char8 *)
                                                          &dvmrpGblVar_g.DvmrpMapCfgData,
                                                          sizeof(L7_dvmrpMapCfg_t) - sizeof(dvmrpGblVar_g.DvmrpMapCfgData.checkSum));

    /* call NVStore save routine */
    if (sysapiCfgFileWrite(L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                           L7_DVMRP_CFG_FILENAME, (L7_char8 *)&dvmrpGblVar_g.DvmrpMapCfgData,
                           sizeof(L7_dvmrpMapCfg_t)) != L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                      "\nError during sysapiCfgFileWrite for config file %s\n",
                      L7_DVMRP_CFG_FILENAME);
      DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                       "Error during sysapiCfgFileWrite for config file\n");
      return L7_FAILURE;
    }
    dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged = L7_FALSE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restore DVMRP user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpRestore(void)
{
  L7_RC_t rc = L7_SUCCESS;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");

  if (dvmrpMapDvmrpAdminModeDisable(L7_TRUE)== L7_SUCCESS)
  {
    dvmrpBuildDefaultConfigData(L7_DVMRP_CFG_VER_CURRENT);
    dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged = L7_TRUE;
    rc = dvmrpApplyConfigData();
  }
  return rc;
}

/*********************************************************************
* @purpose  Check if DVMRP user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dvmrpHasDataChanged(void)
{
  return dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged;
}
void dvmrpResetDataChanged(void)
{
  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Build default DVMRP config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dvmrpBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 i;

  /*--------------------------*/
  /* build config file header */
  /*--------------------------*/

  memset((void *)&dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr, 0,
         sizeof(L7_fileHdr_t));
  strcpy(dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.filename,
         L7_DVMRP_CFG_FILENAME);
  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.version       = ver;
  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.componentID   =
  L7_FLEX_DVMRP_MAP_COMPONENT_ID;
  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.type          = L7_CFG_DATA;
  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.length      = sizeof(L7_dvmrpMapCfg_t);
  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged   = L7_FALSE;
  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.version       = ver;
  dvmrpGblVar_g.family                                 = L7_AF_INET;

  /*---------------------------*/
  /* build default config data */
  /*---------------------------*/

  /* generic DVMRP cfg */
  dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode = FD_DVMRP_DEFAULT_ADMIN_MODE;

  /* DVMRP router interface configuration parameters */
  /* we are preserving any previous configId fields in the table */
  for (i=1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
  {
    dvmrpMapIntfBuildDefaultCfg(&(dvmrpGblVar_g.DvmrpMapCfgData.ckt[i]));
  }
  dvmrpGblVar_g.DvmrpMapCfgData.checkSum = 0;
}



/*********************************************************************
* @purpose  Configure DVMRP router interfaces
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouterIntfConfigure(L7_uint32 intIfNum)
{
  dvmrpCfgCkt_t *pCfg;

  if (dvmrpMapIntfIsConfigurable(intIfNum, &pCfg))
  {
    if (dvmrpMapIntfIsOperational(intIfNum)== L7_TRUE)
    {
      dvmrpMapIntfMetricApply(intIfNum, pCfg->intfMetric);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Routing Event notifications to DVMRP Map task
*
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event type
* @param    pData   @b{(input)} Data
* @param    pEventInfo   @b{(input)} Event Info
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRoutingEventChangeCallBack(L7_uint32 intIfNum,
                                           L7_uint32 event,
                                           void *pData,
                                           ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  dvmrpMapMsg_t msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  /* Filter out the events that DVMRP Map is not interested in.
   */
  if (((event != L7_RTR_ENABLE) &&
       (event != L7_RTR_DISABLE_PENDING) &&
       (event != L7_RTR_INTF_CREATE) &&
       (event != L7_RTR_INTF_DELETE) &&
       (event != L7_RTR_INTF_ENABLE) &&
       (event != L7_RTR_INTF_DISABLE_PENDING)) ||
      ((intIfNum != 0) && (dvmrpIntfIsValid(intIfNum) != L7_TRUE)))
  {
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_DVMRP;
    if (pEventInfo != L7_NULLPTR)
    {
      completionData.correlator  = pEventInfo->correlator;
      completionData.handlerId   = pEventInfo->handlerId;
    }
    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  memset(&msg, 0, sizeof(dvmrpMapMsg_t));

  msg.msgId = DVMRPMAP_ROUTING_EVENT_MSG;

  msg.u.dvmrpMapRoutingEventParms.intIfNum = intIfNum;
  msg.u.dvmrpMapRoutingEventParms.event = event;
  msg.u.dvmrpMapRoutingEventParms.pData = pData;

  if (pEventInfo != L7_NULLPTR)
  {
    memcpy(&msg.u.dvmrpMapRoutingEventParms.eventInfo, pEventInfo,
           sizeof(ASYNC_EVENT_NOTIFY_INFO_t));
    msg.u.dvmrpMapRoutingEventParms.asyncResponseRequired = L7_TRUE;
  }
  else
  {
    msg.u.dvmrpMapRoutingEventParms.asyncResponseRequired = L7_FALSE;
  }

  if(dvmrpMessageSend(DVMRP_EVENT_Q,&msg) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to send message to Event Q.\n");
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_DVMRP;
    if (pEventInfo != L7_NULLPTR)
    {
      completionData.correlator  = pEventInfo->correlator;
      completionData.handlerId   = pEventInfo->handlerId;
    }
    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This routine is called when a routing event occurs that
*           may change the operational state of the DVMRP stack.
*
* @param    intIfnum    @b{(input)} internal interface number
* @param    event       @b{(input)} state event, as defined in
*                       L7_RTR_EVENT_CHANGE_t
* @param    pData       @b{(input)} data
* @param    asyncRespReq @b{(input)} Async response request
* @param    pEventInfo   @b{(input)} event info
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dvmrpMapRoutingEventProcess(L7_uint32 intIfNum,
                                 L7_uint32 event,
                                 void *pData,
                                 L7_BOOL asyncRespReq,
                                 ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_uint32     mode;
  L7_uint32      glblAdminMode = L7_NULL;
  dvmrpCfgCkt_t *pCfg;
  L7_RC_t       rc = L7_SUCCESS;

  if (dvmrpMapDvmrpAdminModeGet(&glblAdminMode) != L7_SUCCESS)
  {
    glblAdminMode = L7_DISABLE;
  }

  if ((intIfNum == L7_NULL) ||
      ((intIfNum != L7_NULL) && (dvmrpIntfIsValid(intIfNum)== L7_TRUE)))
  {
    switch (event)
    {
      case L7_RTR_ENABLE:
        if (glblAdminMode == L7_ENABLE)
          rc = dvmrpMapDvmrpAdminModeEnable(L7_FALSE);
        break;

      case L7_RTR_DISABLE_PENDING:
        if (glblAdminMode == L7_ENABLE)
          rc = dvmrpMapDvmrpAdminModeDisable(L7_FALSE);
        break;

      case L7_RTR_INTF_CREATE:
        dvmrpIntfCreate(intIfNum);
        break;

      case L7_RTR_INTF_DELETE:
        if (dvmrpMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
        {
          dvmrpMapIntfBuildDefaultCfg(pCfg);
          memset((void *)&(pCfg->configId), 0, sizeof(nimConfigID_t));
          dvmrpGblVar_g.DvmrpMapCfgMapTbl[intIfNum] = 0;
        }
        break;

      case L7_RTR_INTF_DISABLE_PENDING:
        if (glblAdminMode == L7_ENABLE)
        {
          if (dvmrpMapIntfIsOperational(intIfNum) == L7_TRUE)
          {
            rc = dvmrpMapIntfAdminModeApply(intIfNum, L7_DISABLE);
          }
        }
        break;

      case L7_RTR_INTF_ENABLE:
        if (glblAdminMode == L7_ENABLE)
        {
          if ((dvmrpMapIntfAdminModeGet(intIfNum,&mode) == L7_SUCCESS)&&
             (mode == L7_ENABLE))
          {
            if (dvmrpMapIntfIsOperational(intIfNum) != L7_TRUE)
            {
              rc = dvmrpMapIntfAdminModeApply(intIfNum,L7_ENABLE);
            }
          }
        }
        break;

      default:
        break;
    }
  }

  if (asyncRespReq == L7_TRUE)
  {
    ASYNC_EVENT_COMPLETE_INFO_t completionData;

    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_DVMRP;
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
    completionData.async_rc.rc = rc;
    if (rc == L7_SUCCESS)
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    }
    else
    {
      completionData.async_rc.reason=ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE;
    }
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }

  return;
}

/*********************************************************************
* @purpose  Propogate MCAST Event notifications to Dvmrp Map task
*
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event type
* @param    pData      @b{(input)} data
* @param    pEventInfo  @b{(input)} event info
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapMcastEventCallBack(L7_uint32 event,
                                   void *pData,
                                   ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  dvmrpMapMsg_t msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  /* Filter out the events that DVMRP Map is not interested in.
   */
  if ((event != MCAST_RTR_ADMIN_MODE_ENABLED) &&
      (event != MCAST_RTR_ADMIN_MODE_DISABLE_PENDING))
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "\nInvalid Input parameters.\n");
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId     = L7_MRP_DVMRP;
    completionData.correlator      = pEventInfo->correlator;
    completionData.handlerId       = pEventInfo->handlerId;
    completionData.async_rc.rc     = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;

  }

  memset(&msg, 0, sizeof(dvmrpMapMsg_t));

  msg.msgId = DVMRPMAP_MCAST_EVENT_MSG;

  msg.u.dvmrpMapIgmpEventParms.event = event;
  msg.u.dvmrpMapIgmpEventParms.pData = pData;
  memcpy(&msg.u.dvmrpMapIgmpEventParms.eventInfo, pEventInfo,
         sizeof(ASYNC_EVENT_NOTIFY_INFO_t));

  if(dvmrpMessageSend(DVMRP_EVENT_Q,&msg) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "\nMessae Send Failed.\n");
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId     = L7_MRP_DVMRP;
    completionData.correlator      = pEventInfo->correlator;
    completionData.handlerId       = pEventInfo->handlerId;
    completionData.async_rc.rc     = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send event to DVMRP Vendor (to register with MFC)
*
* @param    familyType           @b{(input)} Address Family type
* @param    eventType            @b{(input)} Event Type
* @param    msgLen               @b{(input)} Message Length
* @param    eventMsg             @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapEventChangeCallback(L7_uchar8 familyType,
                                    L7_uint32  eventType,
                                    L7_uint32  msgLen, void *eventMsg)
{
  L7_uint32 rtrIfNum = 0;
  L7_uint32 intIfNum = 0;
  L7_BOOL doIntfOperationalCheck = L7_FALSE;

  if ((eventType != MCAST_MFC_NOCACHE_EVENT) &&
      (eventType != MCAST_MFC_ENTRY_EXPIRE_EVENT) &&
      (eventType != MCAST_EVENT_MGMD_GROUP_UPDATE) &&
      (eventType != MCAST_DVMRP_TIMER_EXPIRY_EVENT) &&
      (eventType != MCAST_EVENT_DVMRP_CONTROL_PKT_RECV) &&
      (eventType != MCAST_EVENT_ADMINSCOPE_BOUNDARY))
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "\nInput parameters are not"
                    "valid for eventType:%d.\n", eventType);
    return L7_FAILURE;
  }


  if (dvmrpMapDvmrpIsOperational() != L7_TRUE)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                      "Received event when dvmrp is not operational");
    return L7_FAILURE;
  }
  /* Check if DVMRP is Operational on the interface to which the event is
   * addressed to.
   * As of now, we do it only for events from MFC and MGMD.
   */
  switch (eventType)
  {
    case MCAST_EVENT_MGMD_GROUP_UPDATE:
      rtrIfNum = ((mgmdMrpEventInfo_t*)eventMsg)->rtrIfNum;
      doIntfOperationalCheck = L7_TRUE;
      break;

    case MCAST_MFC_NOCACHE_EVENT:
    case MCAST_MFC_ENTRY_EXPIRE_EVENT:
      rtrIfNum = ((mfcEntry_t*)eventMsg)->iif;
      doIntfOperationalCheck = L7_TRUE;
      break;

    default:
      break;
  }

  if (doIntfOperationalCheck == L7_TRUE)
  {
    if (mcastIpMapRtrIntfToIntIfNum (familyType, rtrIfNum, &intIfNum) != L7_SUCCESS)
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                      "Failed to convert rtrIfNum (%d) to intIfNum for "
                      "familyType %d", rtrIfNum, familyType);
      return L7_FAILURE;
    }
    if (dvmrpMapIntfIsOperational (intIfNum) != L7_TRUE)
    {
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                      "DVMRP is not Operational on rtrIfNum - %d for Family - %d"
                      " for eventType - %d", rtrIfNum, familyType, eventType);
      return L7_FAILURE;
    }
  }

  if (dvmrpMapMessageQueueSend(familyType, eventType, msgLen,
                               eventMsg) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "MessageQueue Send Failure.\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Admin scope call back function (to register with MFC)
*
* @param    eventType            @b{(input)} Event Type
* @param    msgLen               @b{(input)} Message Length
* @param    eventMsg             @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapAdminScopeCallback(L7_uint32  eventType,
                                   L7_uint32  msgLen, void *eventMsg)
{

  if (dvmrpMapMessageQueueSend(L7_AF_INET, eventType, msgLen,
                               eventMsg) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "MessageQueue Send Failure.\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Process mcast-related events
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    event       @b{(input)} Event On the Interface
* @param    pData       @b{(input)} data
* @param    pEventInfo  @b{(input)} event info
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dvmrpMapMcastEventProcess(L7_uint32 intIfNum, L7_uint32 event,
                               void *pData, ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_RC_t rc = L7_SUCCESS;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  completionData.componentId = L7_MRP_DVMRP;
  completionData.correlator  = pEventInfo->correlator;
  completionData.handlerId   = pEventInfo->handlerId;
  completionData.async_rc.rc = L7_SUCCESS;

  if (intIfNum != L7_NULL)
  {
    if (dvmrpIntfIsValid(intIfNum) != L7_TRUE)
    {
      /* Return event completion status  */
      asyncEventCompleteTally(&completionData);

      return;
    }
  }

  switch (event)
  {
    case MCAST_RTR_ADMIN_MODE_ENABLED:
      if (dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode == L7_ENABLE)
      {
        rc = dvmrpMapDvmrpAdminModeEnable(L7_FALSE);
      }
      break;

    case MCAST_RTR_ADMIN_MODE_ENABLE_PENDING:
      /* do nothing here */
      break;

    case MCAST_RTR_ADMIN_MODE_DISABLE_PENDING:
      if (dvmrpGblVar_g.DvmrpMapCfgData.rtr.adminMode == L7_ENABLE)
      {
        DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                        "In dvmrpMapRoutingEventProcess function at the case"
                        "handling L7_RTR_IGMP_ADMIN_MODE_DISABLE_PENDING"
                        "dvmrpMapDvmrpAdminModeDisable callled\n");
        rc = dvmrpMapDvmrpAdminModeDisable(L7_FALSE);

      }
      break;

    case MCAST_RTR_ADMIN_MODE_DISABLED:
      /* do nothing here */
      break;

    default:
      break;
  }

  completionData.async_rc.rc = rc;
  if (rc == L7_SUCCESS)
  {
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
  }
  else
  {
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE;
  }
  /* Return event completion status  */

  asyncEventCompleteTally(&completionData);

  return;
}

/*********************************************************************
* @purpose  Sets the interface Configuration parameters to default
*
* @param    pCfg   @b{(input)} pointer to the DVMRP ckt configuration record
*
* @returns  Void
*
* @notes
*
* @end
*********************************************************************/

void dvmrpMapIntfBuildDefaultCfg(dvmrpCfgCkt_t *pCfg)
{
  if (pCfg == L7_NULLPTR)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "pCfg pointer is NULL.\n");
    return;
  }

  pCfg->intfAdminMode     = FD_DVMRP_INTF_DEFAULT_ADMIN_MODE;
  pCfg->intfMetric        = FD_DVMRP_INTF_DEFAULT_METRIC;
}

/*********************************************************************
* @purpose  Determine if the interface is valid for DVMRP configuration
*           and optionally output a pointer to the configuration structure
*           entry for this interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    pCfg       @b{(input)} Output pointer location, or L7_NULL if
*                       not needed
*                       @b{(output)} Pointer to DVMRP interface config structure
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the
*           interface config data structure.
*
* @end
*********************************************************************/

L7_BOOL dvmrpMapIntfIsConfigurable(L7_uint32 intIfNum, dvmrpCfgCkt_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if (!(DVMRP_IS_READY))
  {
    return L7_FALSE;
  }

  index = dvmrpGblVar_g.DvmrpMapCfgMapTbl[intIfNum];

  if (index == L7_NULL)
  {
    return L7_FALSE;
  }

  /* verify that the configId in the config data table entry matches
   * the configId that NIM maps to the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId,
                               &(dvmrpGblVar_g.DvmrpMapCfgData.ckt[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between
       * pIgmpMapCfgData and pIgmpMapCfgMapTbl or there is synchronization
       * issue between NIM and components w.r.t. interface creation/deletion
       */
      DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,
                      "Error accessing DVMRP config data for interface %d in"
                      "dvmrpMapIntfIsConfigurable.\n", intIfNum);
      return L7_FALSE;
    }
  }

  if (pCfg != L7_NULLPTR)
  {
    *pCfg = &(dvmrpGblVar_g.DvmrpMapCfgData.ckt[index]);
  }

  return L7_TRUE;
}
/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dvmrpIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  dvmrpCfgCkt_t *pCfg;
  L7_RC_t dvmrpRC = L7_SUCCESS;
  L7_uint32 i;

  if (dvmrpIntfIsValid(intIfNum) != L7_TRUE)
  {
    return(dvmrpRC);
  }

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
  {
    return(dvmrpRC);
  }

  pCfg = L7_NULLPTR;

  /* scan for any preexisting config records that match this **
  ** configId; if found, update the config mapping table     */
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&dvmrpGblVar_g.DvmrpMapCfgData.ckt[i].configId,
                               &configId))
    {
      dvmrpGblVar_g.DvmrpMapCfgMapTbl[intIfNum] = i;
      break;
    }
  }

  /* If an interface configuration entry is not already **
  ** assigned to the interface, assign one              */
  /*
  ** Note: dvmrpMapIntfIsConfigurable() can return L7_FALSE for     **
  ** two reasons: no matching config table entry was found,         **
  ** or the intIfNum is not valid for DVMRP... the above call to    **
  ** dvmrpIntfIsValid() assures that if we get L7_FAILURE it is due   **
  ** to the first reason and we should try to assign a config table **
  ** entry for this intf.                                           */
  if (dvmrpMapIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    if (dvmrpMapIntfConfigEntryGet(intIfNum, &pCfg) == L7_TRUE)
    {
      /* successfully assigned a new config table entry for this intf, **
      ** so initialize the entry to defaults                           */
      /* Update the configuration structure with the config id */
      NIM_CONFIG_ID_COPY(&pCfg->configId, &configId);
      /* since this is a newly assigned configuration entry, initialize **
      ** it to default settingsa                                        */
      dvmrpMapIntfBuildDefaultCfg(pCfg);
      dvmrpRC = L7_SUCCESS;
    }
    else
    {
      /* unsuccessful in assigning config table entry for this intf... **
      **  report failure                                               */
      dvmrpRC = L7_FAILURE;
      DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                "Unsuccessful in assigning config table entry for this intf\n");
    }
  }
  else
  {
    /* we found an existing config table entry for this intf */
    dvmrpRC = L7_SUCCESS;
  }
  return(dvmrpRC);
}

/*********************************************************************
* @purpose  Retrieve a pointer to an empty configuration data for a given
*           interface if found.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    pCfg       @b{(input)} Output pointer location
*                       @b{(output)} Pointer to DVMRP interface config structure
*
* @returns  L7_TRUE     an empty interface config structure was found
* @returns  L7_FALSE    no empty interface config structure entry is available
*
* @notes    Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the
*           interface config data structure.
*
* @end
*********************************************************************/
L7_BOOL dvmrpMapIntfConfigEntryGet(L7_uint32 intIfNum, dvmrpCfgCkt_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configIdNull;

  if (!(DVMRP_IS_READY))
  {
    *pCfg = L7_NULLPTR;
    return L7_FALSE;
  }

  memset((void *)&configIdNull, 0, sizeof(nimConfigID_t));

  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&(dvmrpGblVar_g.DvmrpMapCfgData.
                                 ckt[i].configId), &configIdNull))
    {
      dvmrpGblVar_g.DvmrpMapCfgMapTbl[intIfNum] = i;
      *pCfg = &dvmrpGblVar_g.DvmrpMapCfgData.ckt[i];
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}
/*********************************************************************
* @purpose  Process UI events
*
* @param    addrFamily  @b{(input)} Address Family Identifier
* @param    event       @b{(input)} UI Event, one of DVMRPMAP_UI_EVENT_TYPE_t
* @param    pData       @b{(input)} UI Event Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapUIEventChangeProcess (DVMRPMAP_UI_EVENT_TYPE_t event,
                                      void *pData)
{
  dvmrpMapUIEventParms_t *uiEvent = (dvmrpMapUIEventParms_t*) pData;


  if (uiEvent == L7_NULLPTR)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "DVMRP MAP Dequeued Buffer is NULL for Event - %d.\n", event);
    return L7_FAILURE;
  }

  switch (event)
  {
    case DVMRPMAP_ADMIN_MODE_SET:
      {
        dvmrpMapUiAdminMode_t *adminMode = &(uiEvent->eventInfo.adminMode);

        if (adminMode->mode == L7_ENABLE)
        {
          dvmrpMapDvmrpAdminModeEnable (L7_TRUE);
        }
        else if (adminMode->mode == L7_DISABLE)
        {
          dvmrpMapDvmrpAdminModeDisable (L7_TRUE);
        }

        break;
      }

    case DVMRPMAP_INTF_MODE_SET:
      {
        dvmrpMapUiIntfMode_t *intfMode = &(uiEvent->eventInfo.intfMode);
        L7_uint32 dvmrpAdminMode;

        if (intfMode->mode == L7_ENABLE)
        {
          if ((dvmrpMapDvmrpAdminModeGet (&dvmrpAdminMode)
               == L7_SUCCESS) &&
              (dvmrpAdminMode == L7_ENABLE))
          {
            dvmrpMapIntfAdminModeApply (intfMode->intIfNum, L7_ENABLE);
          }
        }
        else if (intfMode->mode == L7_DISABLE)
        {
          dvmrpMapIntfAdminModeApply (intfMode->intIfNum, L7_DISABLE);
        }

        break;
      }

    case DVMRPMAP_INTF_METRIC_SET:
      {
        dvmrpMapUiIntfMetric_t *intfMetric = &(uiEvent->eventInfo.intfMetric);

        dvmrpMapIntfMetricApply (intfMetric->intIfNum,intfMetric->metric);

        break;
      }

    default:
      break;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Post UI event to the Mapping layer
*
* @param    addrFamily  @b{(input)} Address Family Identifier
* @param    event       @b{(input)} UI Event
* @param    pData       @b{(input)} UI Event Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapUIEventSend (DVMRPMAP_UI_EVENT_TYPE_t eventType,
                             L7_uint32 mode,
                             L7_uint32 intIfNum,
                             L7_uint32 metric)
{
  dvmrpMapMsg_t msg;

  memset(&msg, 0, sizeof(dvmrpMapMsg_t));

  msg.msgId = DVMRPMAP_UI_EVENT_MSG;

  msg.u.dvmrpMapUiEventParms.event = eventType;

  switch (eventType)
  {
    case DVMRPMAP_ADMIN_MODE_SET:
      msg.u.dvmrpMapUiEventParms.eventInfo.adminMode.mode = mode;
      break;

    case DVMRPMAP_INTF_MODE_SET:
      msg.u.dvmrpMapUiEventParms.eventInfo.intfMode.intIfNum = intIfNum;
      msg.u.dvmrpMapUiEventParms.eventInfo.intfMode.mode = mode;
      break;

    case DVMRPMAP_INTF_METRIC_SET:
      msg.u.dvmrpMapUiEventParms.eventInfo.intfMetric.intIfNum = intIfNum;
      msg.u.dvmrpMapUiEventParms.eventInfo.intfMetric.metric = metric;
      break;

    default:
      DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Invalid eventType:%d.\n",
                       eventType);
      return L7_FAILURE;
  }

  if(dvmrpMessageSend(DVMRP_EVENT_Q,&msg) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "DVMRP Map Event Post Failed for eventType:%d.\n", eventType);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To send the event to Message Queue
*
* @param    familyType  @b{(input)} address family
* @param    eventyType  @b{(input)} Event Type
* @param    msgLen      @b{(input)} Message Length.
* @param    pMsg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapMessageQueueSend(L7_uchar8 familyType, L7_uint32 eventType,
                                 L7_uint32 msgLen, L7_VOIDPTR pMsg)
{
  dvmrpMapMsg_t      msg;
  dvmrpAppTimerMsg_t appTimerMsg;
  dvmrpMapCtrlPktAndMfcMsg_t pktAndMfcMsg;
  DVMRP_QUEUE_ID_t   QId;
  void               *mesageBuf=L7_NULLPTR;

  if ((pMsg == L7_NULLPTR) ||
      ((eventType != MCAST_MFC_NOCACHE_EVENT) &&
       (eventType != MCAST_MFC_ENTRY_EXPIRE_EVENT) &&
       (eventType != MCAST_EVENT_MGMD_GROUP_UPDATE) &&
       (eventType != MCAST_DVMRP_TIMER_EXPIRY_EVENT) &&
       (eventType != MCAST_EVENT_DVMRP_CONTROL_PKT_RECV) &&
       (eventType != MCAST_EVENT_ADMINSCOPE_BOUNDARY)))
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "\nInput parameters are not"
                    "valid for eventType:%d.\n", eventType);
    return L7_FAILURE;
  }
  msg.msgId = DVMRPMAP_PROTOCOL_EVENT_MSG;

  msg.u.dvmrpMapProtocolEventParms.event = eventType;
  msg.u.dvmrpMapProtocolEventParms.familyType = familyType;
  memcpy(&(msg.u.dvmrpMapProtocolEventParms.msgData), pMsg, msgLen);

  if (eventType == MCAST_DVMRP_TIMER_EXPIRY_EVENT)
  {
    memset(&appTimerMsg, 0, sizeof(dvmrpAppTimerMsg_t));
    appTimerMsg.msgId = DVMRPMAP_PROTOCOL_EVENT_MSG;

    appTimerMsg.dvmrpMapAppTimerEventParms.event = eventType;
    appTimerMsg.dvmrpMapAppTimerEventParms.familyType = familyType;
    appTimerMsg.dvmrpMapAppTimerEventParms.timerCtrlBlk = pMsg;
    QId = DVMRP_APP_TIMER_Q;
    mesageBuf =&appTimerMsg;

  }
  else if ((eventType == MCAST_EVENT_DVMRP_CONTROL_PKT_RECV) ||
           (eventType == MCAST_MFC_NOCACHE_EVENT) ||
           (eventType == MCAST_MFC_WRONGIF_EVENT) ||
           (eventType == MCAST_MFC_ENTRY_EXPIRE_EVENT))
  {
    /* Reserve 60% of Ctrl Pkt Queue space for MFC Events */
    if((eventType == MCAST_MFC_NOCACHE_EVENT) ||
       (eventType == MCAST_MFC_WRONGIF_EVENT) ||
       (eventType == MCAST_MFC_ENTRY_EXPIRE_EVENT))
    {
      if (dvmrpMapCountersValueGet (DVMRP_MAP_NUM_MFC_EVENTS) >= DVMRPMAP_MFC_EVENTS_COUNT_LIMIT)
      {
        DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES,"Dropping the MFC Event as "
                         "there is not enough Event Queue space.\n");
        dvmrpMapCountersUpdate (DVMRP_MAP_NUM_MFC_OVERFLOW_EVENTS, DVMRP_MAP_COUNTERS_INCREMENT);
        if (osapiSemaTake (dvmrpGblVar_g.queueCountersSemId, L7_WAIT_FOREVER)
                        == L7_SUCCESS)
        {
          dvmrpGblVar_g.dvmrpQueue[DVMRP_CTRL_PKT_Q].QSendFailedCnt++;
          osapiSemaGive (dvmrpGblVar_g.queueCountersSemId);
        }
        return L7_FAILURE;
      }

      dvmrpMapCountersUpdate (DVMRP_MAP_NUM_MFC_EVENTS, DVMRP_MAP_COUNTERS_INCREMENT);
    }

    memset(&pktAndMfcMsg,0,sizeof(dvmrpMapCtrlPktAndMfcMsg_t));
    pktAndMfcMsg.msgId = DVMRPMAP_PROTOCOL_EVENT_MSG;

    pktAndMfcMsg.dvmrpMapCtrlPktAndMfcParms.event = eventType;
    pktAndMfcMsg.dvmrpMapCtrlPktAndMfcParms.familyType = familyType;

    if ((pMsg != L7_NULLPTR) && (msgLen > 0))
    {
      memcpy(&(pktAndMfcMsg.dvmrpMapCtrlPktAndMfcParms.u), pMsg, msgLen);
    }

    QId = DVMRP_CTRL_PKT_Q;
    mesageBuf =&pktAndMfcMsg;
  }
  else
  {
    memset(&msg, 0, sizeof(dvmrpMapMsg_t));
    msg.msgId = DVMRPMAP_PROTOCOL_EVENT_MSG;

    msg.u.dvmrpMapProtocolEventParms.event = eventType;
    msg.u.dvmrpMapProtocolEventParms.familyType = familyType;
    memcpy(&(msg.u.dvmrpMapProtocolEventParms.msgData), pMsg, msgLen);
    QId = DVMRP_EVENT_Q;
    mesageBuf =&msg;
  }

  if(dvmrpMessageSend(QId ,mesageBuf) != L7_SUCCESS)
  {
     DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "\nMessae Send Failed"
                    "for eventType:%d.\n", eventType);

     return L7_FAILURE;
  }

  return L7_SUCCESS;
}

L7_RC_t dvmrpMessageSend(L7_uint32 QIndex,void *msg)
{

  if (osapiMessageSend (dvmrpGblVar_g.dvmrpQueue[QIndex].QPointer, (L7_VOIDPTR) msg,
                        dvmrpGblVar_g.dvmrpQueue[QIndex].QSize, L7_NO_WAIT, L7_MSG_PRIORITY_NORM)
                     != L7_SUCCESS)
  {
    dvmrpGblVar_g.dvmrpQueue[QIndex].QSendFailedCnt++;

    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,"DVMRP Map Queue of queueIndex:%d"
                     " Failed while sending the message.\n", QIndex);
    return L7_FAILURE;
  }
  else
  {
     dvmrpGblVar_g.dvmrpQueue[QIndex].QCurrentMsgCnt++;

     if(dvmrpGblVar_g.dvmrpQueue[QIndex].QCurrentMsgCnt >
               dvmrpGblVar_g.dvmrpQueue[QIndex].QMaxRx)
     {
       dvmrpGblVar_g.dvmrpQueue[QIndex].QMaxRx =
                 dvmrpGblVar_g.dvmrpQueue[QIndex].QCurrentMsgCnt;
     }
     osapiSemaGive(dvmrpGblVar_g.msgQSema );
  }
  return L7_SUCCESS;
}

/********************** DVMRP DEBUG TRACE ROUTINES *******************/

/*********************************************************************
 * @purpose  Saves dvmrp debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    dvmrpDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpDebugSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Copy the operational states into the config file */
  dvmrpDebugCfgUpdate();

  if (dvmrpDebugHasDataChanged() == L7_TRUE)
  {
    dvmrpDebugCfg.hdr.dataChanged = L7_FALSE;
    dvmrpDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&dvmrpDebugCfg,
        (L7_uint32)(sizeof(dvmrpDebugCfg) - sizeof(dvmrpDebugCfg.checkSum)));
    /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_FLEX_DVMRP_MAP_COMPONENT_ID, DVMRP_DEBUG_CFG_FILENAME ,
            (L7_char8 *)&dvmrpDebugCfg, (L7_uint32)sizeof(dvmrpDebugCfg_t))) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
             "DVMRP Config-Save Failed; File - %s"
             " DVMRP current running configuration write to file/memory Failed.", DVMRP_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
 * @purpose  Restores dvmrp debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    dvmrpDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpDebugRestore(void)
{
  L7_RC_t rc;

  dvmrpDebugBuildDefaultConfigData(DVMRP_DEBUG_CFG_VER_CURRENT);

  dvmrpDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = dvmrpApplyDebugConfigData();

  return rc;
}
/*********************************************************************
 * @purpose  Checks if dvmrp debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL dvmrpDebugHasDataChanged(void)
{
  return dvmrpDebugCfg.hdr.dataChanged;
}

/*********************************************************************
 * @purpose  Build default dvmrp config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void dvmrpDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  dvmrpDebugCfg.hdr.version = ver;
  dvmrpDebugCfg.hdr.componentID = L7_FLEX_DVMRP_MAP_COMPONENT_ID;
  dvmrpDebugCfg.hdr.type = L7_CFG_DATA;
  dvmrpDebugCfg.hdr.length = (L7_uint32)sizeof(dvmrpDebugCfg);
  strcpy((L7_char8 *)dvmrpDebugCfg.hdr.filename, DVMRP_DEBUG_CFG_FILENAME);
  dvmrpDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&dvmrpDebugCfg.cfg, 0, sizeof(dvmrpDebugCfg.cfg));
}

/*********************************************************************
 * @purpose  Apply dvmrp debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t dvmrpApplyDebugConfigData(void)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 flagIndex;

  dvmrpDebugTraceFlags_t *dvmrpDebugTraceFlags =
           &(dvmrpDebugCfg.cfg.dvmrpDebugTraceFlag);
  for (flagIndex = 0;  flagIndex < DVMRP_DEBUG_LAST_TRACE ; flagIndex ++)
  {
    if (((*(dvmrpDebugTraceFlags[flagIndex/DVMRP_DEBUG_TRACE_FLAG_BITS_MAX])) &
         (DVMRP_DEBUG_TRACE_FLAG_VALUE << (flagIndex % DVMRP_DEBUG_TRACE_FLAG_BITS_MAX))) != 0)
    {
      rc = dvmrpDebugTraceFlagSet(flagIndex, L7_TRUE);
    }
    else
    {
      rc = dvmrpDebugTraceFlagSet(flagIndex, L7_FALSE);
    }
  }
  return rc;
}


/*********************************************************************
* @purpose  Process protocol-related events
*
* @param    familyType  @b{(input)} address family
* @param    eventType   @b{(input)} Event.
* @param    pData       @b{(input)} Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
static void
dvmrpMapProtoEventChangeProcess (L7_uchar8 familyType,
                                 L7_uint32 eventType,
                                 void *pData)
{
  if (dvmrpEventProcess (familyType, eventType, pData) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "DVMRP Event Process Failed for event %d", eventType);

    /* It might be possible that the MRP might be disabled after the MGMD
     * event is enqueued.  So, free the allocated MGMD Event buffers.
     */
    if (eventType == MCAST_EVENT_MGMD_GROUP_UPDATE)
    {
      /* Free the MCAST MGMD Events Source List Buffers */
      mcastMgmdEventsBufferPoolFree (familyType, (mgmdMrpEventInfo_t *) pData);
    }
    if (eventType == MCAST_EVENT_DVMRP_CONTROL_PKT_RECV)
    {
      mcastCtrlPktBufferPoolFree(familyType, ((mcastControlPkt_t *) pData)->payLoad);
    }
    return;
  }

  return;
}

