/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  pimdm_map.c
*
* @purpose   PIMDM Mapping system infrastructure
*
* @component PIMDM Mapping Layer
*
* @comments  none
*
* @create    02/04/2006
*
* @author    gkiran
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "commdefs.h"
#include "l3_mcast_defaultconfig.h"
#include "l7_mcast_api.h"
#include "l7_pimdm_api.h"
#include "l7_ip6_api.h"
#include "nvstoreapi.h"
#include "mcast_wrap.h"
#include "pimdm_map.h"
#include "pimdm_map_debug.h"
#include "pimdm_map_v6_wrappers.h"
#include "pimdm_map_config.h"
#include "pimdm_main.h"
#include "support_api.h"

/* PIMDM mapping component globals */
pimdmGblVars_t pimdmGblVariables_g;
pimdmMapCB_t   *pimdmMapCB_g;

pimdmDebugCfg_t pimdmDebugCfg;

static void pimdmMapRoutingEventProcess (L7_uint32 intIfNum, L7_uint32 event, 
                                     void *pData, L7_BOOL asyncRespReq, 
                                     ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);
static void pimdm6MapRoutingEventProcess (L7_uint32 intIfNum, L7_uint32 event,
                                  void *pData, L7_BOOL asyncRespReq,
                                  ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

static void pimdmMapIntfChangeProcess(L7_uint32 intIfNum, 
                                L7_uint32 event, NIM_CORRELATOR_t correlator);
static void pimdmMapMcastEventProcess(L7_uint32 event,void *pData,
                                      ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

static void pimdmMapNimEventProcess(L7_uint32 intIfNum, L7_uint32 event,
                   NIM_CORRELATOR_t correlator, pimdmMapCB_t *pimdmMapCbPtr);
static void pimdmMapProtoEventChangeProcess(L7_uchar8 familyType, 
                   L7_uint32 event, void *pData);
static void pimdmCommonBuildDefaultConfigData(pimdmMapCB_t *pimdmMapCbPtr,
                                                            L7_uint32 ver);
static L7_BOOL pimdmMapIntfConfigEntryGet(L7_uint32 intIfNum, 
                            pimdmMapCB_t *pimdmMapCbPtr, pimdmCfgCkt_t **pCfg);

static void pimdmMapMcastCommonEventProcess(L7_uint32 event,
                                            pimdmMapCB_t *pimdmMapCbPtr);

static L7_RC_t pimdmIntfCreate(L7_uint32 intIfNum, 
                                            pimdmMapCB_t *pimdmMapCbPtr); 
/*********************************************************************
*
* @purpose task to handle all PIM-DM Mapping messages
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*
*********************************************************************/
void pimdmMapTask()
{
  pimdmMapMsg_t msg;
  pimdmMapAppTmrMsg_t appTmrMsg;
  pimdmMapCtrlPktAndMfcMsg_t pktAndMfcMsg;
  L7_RC_t      status = L7_FAILURE;
  L7_uint32 QIndex=0;

  pimdmGblVariables_g.pimdmQueue[PIMDM_APP_TIMER_Q].QRecvBuffer =  &appTmrMsg;
  pimdmGblVariables_g.pimdmQueue[PIMDM_EVENT_Q].QRecvBuffer =  &msg;
  pimdmGblVariables_g.pimdmQueue[PIMDM_CTRL_PKT_Q].QRecvBuffer =  &pktAndMfcMsg;


  osapiTaskInitDone(L7_PIMDM_MAP_TASK_SYNC);

  

  do
  {
    /* Since we are reading from multiple queues, we cannot wait forever 
     * on the message receive from each queue. Rather than sleep between
     * queue reads, use a semaphore to indicate whether any queue has
     * data. Wait until data is available. */

    if (osapiSemaTake(pimdmGblVariables_g.msgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failure taking PIMDM message queue semaphore");
      continue;
    }


    for(QIndex =0;QIndex < PIMDM_MAX_Q; QIndex++)
    {
      status = osapiMessageReceive(pimdmGblVariables_g.pimdmQueue[QIndex].QPointer, 
                                   pimdmGblVariables_g.pimdmQueue[QIndex].QRecvBuffer,
                                   pimdmGblVariables_g.pimdmQueue[QIndex].QSize, 
                                   L7_NO_WAIT);

      if(status == L7_SUCCESS)
      {
        break;
      }
    }

    if(status == L7_SUCCESS)
    {
      if (osapiSemaTake (pimdmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                      == L7_SUCCESS)
      {
        pimdmGblVariables_g.pimdmQueue[QIndex].QCurrentMsgCnt--;
        pimdmGblVariables_g.pimdmQueue[QIndex].QRxSuccess ++;
        osapiSemaGive (pimdmGblVariables_g.queueCountersSemId);
      }

      switch(QIndex)
      {
        case PIMDM_APP_TIMER_Q:
          pimdmMapProtoEventChangeProcess(appTmrMsg.pimdmMapAppTmrParms.familyType,
                                          appTmrMsg.pimdmMapAppTmrParms.event,
                                          L7_NULLPTR);
          break;

        case PIMDM_EVENT_Q:
         {
           switch (msg.msgId)
           {
             case (PIMDMMAP_CNFGR_MSG):
             pimdmCnfgrCommandProcess(&msg.u.cnfgrCmdData);
             break;

             case (PIMDMMAP_INTF_CHANGE_MSG):
             pimdmMapIntfChangeProcess(msg.u.pimdmMapIntfChangeParms.intIfNum, 
                                msg.u.pimdmMapIntfChangeParms.event, 
                                msg.u.pimdmMapIntfChangeParms.correlator);
             break;

             case (PIMDMMAP_ROUTING_EVENT_MSG):
             /* Processing Events from IpMap*/
             pimdmMapRoutingEventProcess(msg.u.pimdmMapRoutingEventParms.intIfNum, 
                                msg.u.pimdmMapRoutingEventParms.event, 
                                msg.u.pimdmMapRoutingEventParms.pData, 
                                msg.u.pimdmMapRoutingEventParms.
                                                 asyncResponseRequired, 
                                &(msg.u.pimdmMapRoutingEventParms.eventInfo));
             break;

             case (PIMDMMAP_RTR6_EVENT_MSG):
             /* Processing Events from Ip6Map*/
             pimdm6MapRoutingEventProcess(msg.u.pimdmMapRoutingEventParms.intIfNum, 
                                msg.u.pimdmMapRoutingEventParms.event, 
                                msg.u.pimdmMapRoutingEventParms.pData, 
                                msg.u.pimdmMapRoutingEventParms.
                                                 asyncResponseRequired, 
                                &(msg.u.pimdmMapRoutingEventParms.eventInfo));
             break;

             case (PIMDMMAP_MCAST_EVENT_MSG):
             pimdmMapMcastEventProcess(msg.u.pimdmMapMcastEventParms.event, 
                                msg.u.pimdmMapMcastEventParms.pData, 
                                &(msg.u.pimdmMapMcastEventParms.eventInfo));
             break;

             case (PIMDMMAP_PROTOCOL_EVENT_MSG):
             if (msg.u.pimdmMapProtocolEventParms.event == MCAST_EVENT_MGMD_GROUP_UPDATE)
             {
               pimdmMapCountersUpdate (PIMDM_MAP_NUM_MGMD_EVENTS, PIMDM_MAP_COUNTERS_DECREMENT);
             }                
             pimdmMapProtoEventChangeProcess(msg.u.pimdmMapProtocolEventParms.familyType,
                                      msg.u.pimdmMapProtocolEventParms.event, 
                                      (void *)&(msg.u.pimdmMapProtocolEventParms.msgData));
            
             break;

             case (PIMDMMAP_UI_EVENT_MSG):
             pimdmMapUIEventChangeProcess (msg.u.pimdmMapUiEventParms.cbHandle,
                                    msg.u.pimdmMapUiEventParms.event, 
                                    (void *)&(msg.u.pimdmMapUiEventParms));
             break;

             default:
             PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, 
                      "\n invalid message type:%d.\n", msg.msgId);
             break;
           }
         }
         break;
        case PIMDM_CTRL_PKT_Q:
          PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS,"Ctrl Pkt Q Event Entry\r\n");
          if((pktAndMfcMsg.pimdmMapCtrlPkAndMfctParms.event == MCAST_MFC_NOCACHE_EVENT)||
             (pktAndMfcMsg.pimdmMapCtrlPkAndMfctParms.event == MCAST_MFC_WRONGIF_EVENT)||
             (pktAndMfcMsg.pimdmMapCtrlPkAndMfctParms.event == MCAST_MFC_ENTRY_EXPIRE_EVENT))
          {
            pimdmMapCountersUpdate (PIMDM_MAP_NUM_MFC_EVENTS, PIMDM_MAP_COUNTERS_DECREMENT);
          }
          pimdmMapProtoEventChangeProcess(pktAndMfcMsg.pimdmMapCtrlPkAndMfctParms.familyType,
                               pktAndMfcMsg.pimdmMapCtrlPkAndMfctParms.event, 
                               (void *)&(pktAndMfcMsg.pimdmMapCtrlPkAndMfctParms.u));
          break;

        default:
          PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"invalid Queue type.\n");
         break;
      }

    }    
  } while (1);
}

/*********************************************************************
* @purpose  Save PIMDM user config file to NVStore
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmSave(void)
{
  L7_uint32    cbIndex = L7_NULL;
  L7_uint32    protoMax = L7_NULL;
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  protoMax =  pimdmMapProtocolsMaxGet();
  for (cbIndex = PIMDM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimdmMapCbPtr = &pimdmMapCB_g[cbIndex];
    if (pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged == L7_TRUE)
    {
      pimdmMapCbPtr->pPimdmMapCfgData->checkSum =
      nvStoreCrc32((L7_char8 *)pimdmMapCbPtr->pPimdmMapCfgData,
      sizeof(L7_pimdmMapCfg_t) - sizeof(pimdmMapCbPtr->pPimdmMapCfgData->checkSum));

      /* call NVStore save routine */
      if (sysapiCfgFileWrite(L7_FLEX_PIMDM_MAP_COMPONENT_ID, 
                             pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.filename, 
                             (L7_char8 *)pimdmMapCbPtr->pPimdmMapCfgData, 
                             sizeof(L7_pimdmMapCfg_t)) != L7_SUCCESS)
      {
        PIMDM_MAP_LOG_MSG (pimdmMapCbPtr->familyType, "PIMDM Config-Save Failed; File - %s",
                           pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.filename);
        return L7_FAILURE;
      }
      pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_FALSE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restore PIMDM user config file to factory defaults
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmRestore(void)
{
    L7_RC_t      rc = L7_SUCCESS;
    L7_uint32    cbIndex = L7_NULL;
    L7_uint32    protoMax = L7_NULL;
    pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

    protoMax =  pimdmMapProtocolsMaxGet();
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS, "\nFunction Entered\n");

    for (cbIndex = PIMDM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
    {
      pimdmMapCbPtr = &pimdmMapCB_g[cbIndex];
      if (pimdmMapPimdmAdminModeDisable (pimdmMapCbPtr, L7_TRUE) == L7_SUCCESS)
      {
          pimdmCommonBuildDefaultConfigData(pimdmMapCbPtr, 
                                            L7_PIMDM_CFG_VER_CURRENT);
          /* pimdmBuildDefaultConfigData(L7_PIMDM_CFG_VER_CURRENT);*/
          pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_TRUE;
          /* rc = pimdmApplyConfigData(pimdmMapCbPtr); */
      } else
      {
          PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, 
                            "Failed to restore PIM-DM configuration to \
                             factory defaults");
      } 
    }
    return rc;
}

/*********************************************************************
* @purpose  Check if PIMDM user config data has changed
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimdmHasDataChanged(void)
{
  return (pimdmV4HasDataChanged() || pimdmV6HasDataChanged());
}
void pimdmResetDataChanged(void)
{
  pimdmMapCB_t    *pimdmMapCbPtr = L7_NULLPTR;

  (void)pimdmMapCtrlBlockGet(L7_AF_INET, &pimdmMapCbPtr);
  pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  (void)pimdmMapCtrlBlockGet(L7_AF_INET6, &pimdmMapCbPtr);
  pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Check if PIMDM V4 user config data has changed
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimdmV4HasDataChanged(void)
{
  pimdmMapCB_t    *pimdmMapCbPtr = L7_NULLPTR;

  (void)pimdmMapCtrlBlockGet(L7_AF_INET, &pimdmMapCbPtr);

  return (pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged);
}

/*********************************************************************
* @purpose  Build default PIMDM config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmBuildDefaultConfigData(L7_uint32 ver)
{
  pimdmMapCB_t      *pimdmMapCbPtr = L7_NULLPTR;

  if (pimdmMapCtrlBlockGet(L7_AF_INET, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                     "Failed to get control block for family type %d\n",
                      L7_AF_INET);
    return;
  }
  pimdmCommonBuildDefaultConfigData(pimdmMapCbPtr, ver);
  return;

}
/*********************************************************************
* @purpose  Build default pimdm config data
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    ver              @b{(input)} Software version of Config Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmCommonBuildDefaultConfigData(pimdmMapCB_t *pimdmMapCbPtr,
                                                            L7_uint32 ver)
{
  L7_uint32 i = L7_NULL;

  /*--------------------------*/
  /* build config file header */
  /*--------------------------*/

  memset(&pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr, 0, sizeof(L7_fileHdr_t));
  pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.version       = ver;
  pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.componentID   = 
                          L7_FLEX_PIMDM_MAP_COMPONENT_ID;
  pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.type          = L7_CFG_DATA;
  pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.length        = 
                          sizeof(L7_pimdmMapCfg_t);
  pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged   = L7_FALSE;

  /*---------------------------*/
  /* build default config data */
  /*---------------------------*/

  /* generic PIMDM cfg */
  if (pimdmMapCbPtr->familyType == L7_AF_INET)
  {
    /* default values for Ipv4*/
    strcpy (pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.filename, 
            L7_PIMDM_CFG_FILENAME);
    pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode = FD_PIMDM_DEFAULT_ADMIN_MODE;
  }
  else
  {
    /* default values for Ipv6*/
    strcpy (pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.filename, 
            L7_PIMDM6_CFG_FILENAME);
    pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode = FD_PIMDM6_DEFAULT_ADMIN_MODE;
  }

  /* PIM router interface configuration parameters */
  for (i=1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
  {
    pimdmIntfBuildDefaultConfigData(&(pimdmMapCbPtr->pPimdmMapCfgData->ckt[i]));
  }

  pimdmMapCbPtr->pPimdmMapCfgData->checkSum = 0;
  /* memset(&pimdmMapCbPtr->pPimdmMapCfgData->checkSum, 0, sizeof(L7_uint32)); */
  return;
}

/*********************************************************************
* @purpose  Build default PIMDM config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdm6BuildDefaultConfigData(L7_uint32 ver)
{
  pimdmMapCB_t      *pimdm6MapCbPtr = L7_NULLPTR;

  if (pimdmMapCtrlBlockGet(L7_AF_INET6, &pimdm6MapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                     "Failed to get control block for family type %d\n",
                      L7_AF_INET6);
    return;
  }
  pimdmCommonBuildDefaultConfigData(pimdm6MapCbPtr, ver);
  return;
}

/*********************************************************************
* @purpose  Apply PIMDM config data
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS  Config data applied, or not a PIMDM interface
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmApplyConfigData(pimdmMapCB_t *pimdmMapCbPtr)
{
  L7_uint32      pimdmIfMode = L7_NULL;
  L7_uint32      intIfNum = L7_NULL;
  L7_uint32      adminMode = L7_NULL;
  L7_uint32      cfgIndex = L7_NULL;

  adminMode = mcastIpMapRtrAdminModeGet (pimdmMapCbPtr->familyType);

  if ((pimdmMapPimdmIsInitialized(pimdmMapCbPtr) != L7_TRUE) && 
      (adminMode == L7_DISABLE))
  {
    return L7_SUCCESS;
  }

  if (pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode == L7_ENABLE)
  {
    pimdmMapPimdmAdminModeEnable(pimdmMapCbPtr, L7_TRUE);
  }
  else if (pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode == L7_DISABLE)
  {
    pimdmMapPimdmAdminModeDisable(pimdmMapCbPtr, L7_TRUE);
  }

  /*-------------------------------------*/
  /* configure per-interface PIMDM parms */
  /*-------------------------------------*/

  /* Apply any interface config data for interfaces that exist at this time */
  if (PIMDM_IS_READY)
  {
    for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
    {
      if (nimIntIfFromConfigIDGet(&(pimdmMapCbPtr->pPimdmMapCfgData->
                                ckt[cfgIndex].configId), &intIfNum) == L7_SUCCESS)
      {
        pimdmCfgCkt_t   *pCfg = L7_NULLPTR;
  
        if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, &pCfg) == L7_TRUE)
        {
          if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum)== L7_TRUE)
          {
            /*Apply the interface configuration parameters*/
            pimdmMapIntfModeApply(intIfNum, pimdmMapCbPtr, pCfg->mode);
            pimdmMapIntfHelloIntervalApply (pimdmMapCbPtr, intIfNum,
                                            pCfg->pimInterfaceHelloInterval);
          }
        }
  
        if (pimdmMapIntfAdminModeGet(pimdmMapCbPtr->familyType, intIfNum,
                                     &pimdmIfMode)== L7_SUCCESS)
        {
          if (pimdmMapIntfModeApply(intIfNum, 
                                    pimdmMapCbPtr, pimdmIfMode) != L7_SUCCESS)
          {
            PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                             "Failed to configure PIM-DM on intIfNum(%d) \
                              for familyType %d\n",
                              intIfNum,pimdmMapCbPtr->familyType);
          }
        }
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Routing Event notifications to PIM-DM Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimdmMapRoutingEventChangeCallBack(L7_uint32 intIfNum, 
                                       L7_uint32 event, 
                                       void *pData, 
                                       ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{                          
  L7_uint32 gblAdminMode = L7_DISABLE;
  pimdmMapMsg_t msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  completionData.componentId = L7_IPRT_PIMDM;
  if (pEventInfo != L7_NULLPTR)
  {
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
  }
  completionData.async_rc.rc = L7_SUCCESS;
  completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;

  /* Filter out the events that PIM-DM Map is not interested in.
   */
  if (((event != L7_RTR_ENABLE) &&
       (event != L7_RTR_DISABLE) &&
       (event != L7_RTR_DISABLE_PENDING) &&
       (event != L7_RTR_INTF_ENABLE) &&
       (event != L7_RTR_INTF_DISABLE_PENDING) &&
       (event != L7_RTR_INTF_DISABLE) &&
       (event != L7_RTR_INTF_IPADDR_DELETE) &&
       (event != L7_RTR_INTF_IPADDR_DELETE_PENDING) &&
       (event != L7_RTR_INTF_IPADDR_ADD) &&
       (event != L7_RTR_INTF_SECONDARY_IP_ADDR_ADD) &&
       (event != L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE) &&
       (event != L7_RTR_INTF_CONFIG_CLEAR)) ||
      ((intIfNum != 0) && (pimdmIntfIsValid(intIfNum) != L7_TRUE)))
  {
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  /* Post the event if PIM-DM Map Global Admin mode is enabled.
   */
  if (pimdmMapPimdmAdminModeGet (L7_AF_INET, &gblAdminMode) != L7_SUCCESS)
  {
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }
  else
  {
    if (gblAdminMode != L7_ENABLE)
    {
      /* Return event completion status  */
      asyncEventCompleteTally(&completionData);
      return L7_SUCCESS;
    }
  }

  memset(&msg, 0, sizeof(pimdmMapMsg_t));

  msg.msgId = PIMDMMAP_ROUTING_EVENT_MSG;

  msg.u.pimdmMapRoutingEventParms.intIfNum = intIfNum;
  msg.u.pimdmMapRoutingEventParms.event = event;
  msg.u.pimdmMapRoutingEventParms.pData = pData;

  if (pEventInfo != L7_NULLPTR)
  {
    memcpy(&msg.u.pimdmMapRoutingEventParms.eventInfo, pEventInfo, 
           sizeof(ASYNC_EVENT_NOTIFY_INFO_t));
    msg.u.pimdmMapRoutingEventParms.asyncResponseRequired = L7_TRUE;
  }
  else
  {
    msg.u.pimdmMapRoutingEventParms.asyncResponseRequired = L7_FALSE;
  }

  if (pimdmMessageSend (PIMDM_EVENT_Q,(L7_VOIDPTR) &msg) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to post event(%d) to \
                       PIM-DM Map Queue",event);

    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Routing Event notifications to PIM-DM6 Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimdm6MapRoutingEventChangeCallBack(L7_uint32 intIfNum, 
                                         L7_uint32 event, 
                                         void *pData, 
                                         ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{                          
  L7_uint32 gblAdminMode = L7_DISABLE;
  pimdmMapMsg_t msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  completionData.componentId = L7_IPRT_PIMDM;
  if (pEventInfo != L7_NULLPTR)
  {
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
  }
  completionData.async_rc.rc = L7_SUCCESS;
  completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;

  /* Filter out the events that PIM-DM Map is not interested in.
   */

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS,"event is %d",event);

  if ((event != RTR6_EVENT_ENABLE) &&
      (event != RTR6_EVENT_DISABLE_PENDING) &&
      (event != RTR6_EVENT_INTF_ADDR_ADD) &&
      (event != RTR6_EVENT_INTF_ADDR_DEL) &&
      (event != RTR6_EVENT_INTF_ENABLE) &&
      (event != RTR6_EVENT_INTF_DISABLE_PENDING))
  {
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  /* Post the event if PIM-DM Map Global Admin mode is enabled.
   */
  if (pimdmMapPimdmAdminModeGet (L7_AF_INET6, &gblAdminMode) != L7_SUCCESS)
  {
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }
  else
  {
    if (gblAdminMode != L7_ENABLE)
    {
      /* Return event completion status  */
      asyncEventCompleteTally(&completionData);
      return L7_SUCCESS;
    }
  }

  memset(&msg, 0, sizeof(pimdmMapMsg_t));
    
  msg.msgId = PIMDMMAP_RTR6_EVENT_MSG;

  msg.u.pimdmMapRoutingEventParms.intIfNum = intIfNum;
  msg.u.pimdmMapRoutingEventParms.event = event;
  msg.u.pimdmMapRoutingEventParms.pData = pData;

  if (pEventInfo != L7_NULLPTR)
  {
    memcpy(&msg.u.pimdmMapRoutingEventParms.eventInfo, pEventInfo, 
           sizeof(ASYNC_EVENT_NOTIFY_INFO_t));
    msg.u.pimdmMapRoutingEventParms.asyncResponseRequired = L7_TRUE;
  }
  else
  {
    msg.u.pimdmMapRoutingEventParms.asyncResponseRequired = L7_FALSE;
  }
   
  if (pimdmMessageSend (PIMDM_EVENT_Q,(L7_VOIDPTR) &msg) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to post event(%d) to \
                       PIM-DM Map Queue",event);

    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This routine is called when a routing event occurs that
*           may change the operational state of the PIMDM stack.
*
* @param    intIfNum      @b{(input)} Interface number
* @param    event         @b{(input)} Event type
* @param    pData         @b{(input)} Data
* @param    asyncRespReq  @b{(input)} async Response Request.
* @param    pEventInfo    @b{(input)} Event Info.
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapRoutingEventProcess (L7_uint32 intIfNum,
                                  L7_uint32 event,
                                  void *pData,
                                  L7_BOOL asyncRespReq,
                                  ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_inet_addr_t  ipAddr;
  pimdmCfgCkt_t   *pCfg = L7_NULLPTR;
  pimdmMapCB_t    *pimdmMapCbPtr = L7_NULLPTR;
  L7_uint32       mode = L7_NULL;
  L7_RC_t         rc = L7_SUCCESS;

  if (pimdmMapCtrlBlockGet(L7_AF_INET, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                     "Failed to get control block for family type %d\n",
                      L7_AF_INET);
    return;
  }

  inetAddressZeroSet(pimdmMapCbPtr->familyType, &ipAddr);

  if ((intIfNum == 0) || ((intIfNum != 0) &&
      (pimdmIntfIsValid (intIfNum) == L7_TRUE)))
  {
    if((pimdmMapPimdmAdminModeGet(pimdmMapCbPtr->familyType, &mode)==L7_SUCCESS)
       && (mode == L7_ENABLE))
    {
      switch (event)
      {
      case L7_RTR_ENABLE:
        /* Enable PIMDM gloablly */
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_EVENTS, "L7_RTR_ENABLE Event received");
        rc = pimdmMapPimdmAdminModeEnable(pimdmMapCbPtr, L7_FALSE);
        break;

      case L7_RTR_DISABLE_PENDING:
        /* Disable PIMDM gloablly */
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_EVENTS, "L7_RTR_DISABLE_PENDING Event "
                         "received");
        rc = pimdmMapPimdmAdminModeDisable(pimdmMapCbPtr, L7_FALSE);
        break;

      case L7_RTR_DISABLE:
      case L7_RTR_INTF_DISABLE:
        /* do nothing */
        break;

      case L7_RTR_INTF_IPADDR_DELETE_PENDING:
      case L7_RTR_INTF_IPADDR_DELETE:
      case L7_RTR_INTF_IPADDR_ADD:
        /* deprecated */
        break;    

        case L7_RTR_INTF_DISABLE_PENDING:
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_EVENTS, "L7_RTR_INTF_DISABLE_PENDING Event "
                         "received");
        if ((pimdmMapIntfAdminModeGet(pimdmMapCbPtr->familyType, intIfNum, 
              &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
        {
          if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) == L7_TRUE)
          {
            rc = pimdmMapIntfModeApply(intIfNum, pimdmMapCbPtr, L7_DISABLE);
          }
        }
        break;    

      case L7_RTR_INTF_ENABLE:
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_EVENTS, "L7_RTR_INTF_ENABLE Event "
                         "received");
        if ((pimdmMapIntfAdminModeGet(pimdmMapCbPtr->familyType, intIfNum, 
              &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
        {
          if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) != L7_TRUE)
          {
            rc = pimdmMapIntfModeApply(intIfNum, pimdmMapCbPtr, L7_ENABLE);
          }
        }

        break;    

      case L7_RTR_INTF_SECONDARY_IP_ADDR_ADD:
        /* Nothing to do. Only primary ip add is handled */
        break;

      case L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE:
        /* Nothing to do. Only primary ip delete is handled */
        break;

      case L7_RTR_INTF_CONFIG_CLEAR:
        if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, &pCfg))
        {
          pimdmIntfBuildDefaultConfigData(pCfg);
        }
        break;

      default:
        break;
      }
    }
  }

  if (asyncRespReq == L7_TRUE)
  {
    ASYNC_EVENT_COMPLETE_INFO_t completionData;

    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_PIMDM;
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
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
  }

  return;
}
/*********************************************************************
* @purpose  This routine is called when a routing event occurs that
*           may change the operational state of the PIMDM stack.
*
* @param    intIfNum      @b{(input)} Interface number
* @param    event         @b{(input)} Event type
* @param    pData         @b{(input)} Data
* @param    asyncRespReq  @b{(input)} async Response Request.
* @param    pEventInfo    @b{(input)} Event Info.
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdm6MapRoutingEventProcess (L7_uint32 intIfNum,
                                  L7_uint32 event,
                                  void *pData,
                                  L7_BOOL asyncRespReq,
                                  ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_inet_addr_t   ipAddr;
  pimdmMapCB_t    *pimdm6MapCbPtr = L7_NULLPTR;
  L7_uint32        mode = L7_NULL;
  L7_RC_t          rc = L7_SUCCESS;

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS,"Entry");

  if (pimdmMapCtrlBlockGet(L7_AF_INET6, &pimdm6MapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                     "Failed to get control block for family type %d\n",
                      L7_AF_INET6);
    return;
  }

  inetAddressZeroSet(pimdm6MapCbPtr->familyType, &ipAddr);

  if ((intIfNum == 0) || ((intIfNum != 0) &&
      (pimdmIntfIsValid (intIfNum) == L7_TRUE)))
  {
    if ((pimdmMapPimdmAdminModeGet(pimdm6MapCbPtr->familyType,&mode) == L7_SUCCESS) 
        && (mode == L7_ENABLE))
    {
      switch (event)
      {
      case RTR6_EVENT_ENABLE:
        /*Enable PIMDM gloablly*/
        rc = pimdmMapPimdmAdminModeEnable(pimdm6MapCbPtr, L7_FALSE);
        break;

        case RTR6_EVENT_DISABLE:
         break;


        case RTR6_EVENT_DISABLE_PENDING:
          /*Disable PIMDM gloablly*/
        rc = pimdmMapPimdmAdminModeDisable(pimdm6MapCbPtr, L7_FALSE);
        break;    

      case RTR6_EVENT_INTF_ADDR_ADD:
        if (mcastIpMapRtrIntfIpAddressGet(pimdm6MapCbPtr->familyType, 
            intIfNum, &ipAddr) == L7_SUCCESS &&
            inetIsAddressZero(&ipAddr) != L7_TRUE)
        {
          if ((pimdmMapIntfAdminModeGet(pimdm6MapCbPtr->familyType, intIfNum, 
              &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
          {
            if (pimdmMapIntfIsOperational(pimdm6MapCbPtr, intIfNum) != L7_TRUE)
            {
              rc = pimdmMapIntfModeApply(intIfNum, pimdm6MapCbPtr, L7_ENABLE);
            }
          }
        }
        break;

      case RTR6_EVENT_INTF_ADDR_DEL:
        if ((pimdmMapIntfAdminModeGet(pimdm6MapCbPtr->familyType, intIfNum, 
                &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
        {
          if (pimdmMapIntfIsOperational(pimdm6MapCbPtr, intIfNum) == L7_TRUE)
          {
            rc = pimdmMapIntfModeApply(intIfNum, pimdm6MapCbPtr, mode);
          }
        }
        break;


      case RTR6_EVENT_INTF_ENABLE:
        if ((pimdmMapIntfAdminModeGet(pimdm6MapCbPtr->familyType, intIfNum, 
              &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
        {
          if (pimdmMapIntfIsOperational(pimdm6MapCbPtr, intIfNum) != L7_TRUE)
          {
            rc = pimdmMapIntfModeApply(intIfNum, pimdm6MapCbPtr, L7_ENABLE);
          }
        }
        break;

      case RTR6_EVENT_INTF_DISABLE_PENDING:
        if ((pimdmMapIntfAdminModeGet(pimdm6MapCbPtr->familyType, intIfNum, 
              &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
        {
          if (pimdmMapIntfIsOperational(pimdm6MapCbPtr, intIfNum) == L7_TRUE)
          {
            rc = pimdmMapIntfModeApply(intIfNum, pimdm6MapCbPtr, L7_DISABLE);
          }
        }
        break;

      default:
        break;
      }
    }
  }
  if (asyncRespReq == L7_TRUE)
  {
    ASYNC_EVENT_COMPLETE_INFO_t completionData;

    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_PIMDM;
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
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
  }
  return;
}
/*********************************************************************
* @purpose  Retrieve a pointer to an empty configuration data for a given
*           interface if found.
*
* @param    intIfNum        @b{(input)}  Internal Interface Number
* @param    pimdmMapCbPtr   @b{(input)}  Mapping Control Block.
* @param    pCfg            @b{(input)}  Output pointer location
*                           @b{(output)} Pointer to PIMDM interface 
*                                        config structure
*
* @returns  L7_TRUE     an empty interface config structure was found
* @returns  L7_FALSE    no empty interface config structure entry is available
*
* @comments Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the  
*           interface config data structure.
*
* @end
*********************************************************************/
L7_BOOL pimdmMapIntfConfigEntryGet(L7_uint32 intIfNum, 
                           pimdmMapCB_t *pimdmMapCbPtr, pimdmCfgCkt_t **pCfg)
{
  L7_uint32 i = L7_NULL;
  nimConfigID_t configIdNull;

  if (!(PIMDM_IS_READY))
  {
    *pCfg = L7_NULLPTR;
    return L7_FALSE;
  }

  memset((void *)&configIdNull, 0, sizeof(nimConfigID_t));

  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&(pimdmMapCbPtr->pPimdmMapCfgData->ckt[i].configId),
    	                          &configIdNull))
    {
      pimdmMapCbPtr->pPimdmMapCfgMapTbl[intIfNum] = i;
      *pCfg = &pimdmMapCbPtr->pPimdmMapCfgData->ckt[i];
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}
/*********************************************************************
* @purpose  To process the Callback for L7_CREATE
*
* @param    intIfNum        @b{(input)} internal interface number
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pimdmIntfCreate(L7_uint32 intIfNum, pimdmMapCB_t *pimdmMapCbPtr) 
{
  nimConfigID_t    configId;
  pimdmCfgCkt_t    *pCfg = L7_NULLPTR;
  L7_RC_t          pimdmRC = L7_SUCCESS;
  L7_uint32        i = L7_NULL;

  if (pimdmIntfIsValid(intIfNum)!= L7_TRUE)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, 
                       "intIfNum(%d) not valid",intIfNum);
    return(pimdmRC);
  }

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, 
                      "Failed to get configuration ID for intIfNum(%d)",
                      intIfNum);
    return(pimdmRC);
  }

  pCfg = L7_NULLPTR;

  /* scan for any preexisting config records that match this
   * configId; if found, update the config mapping table.
   */
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++ )
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&pimdmMapCbPtr->pPimdmMapCfgData->ckt[i].configId,
                               &configId))
    {
      pimdmMapCbPtr->pPimdmMapCfgMapTbl[intIfNum] = i;
      break;
    }
  }

  /* If an interface configuration entry is not already assigned
   * to the interface, assign one.
   */

  /* 
   * Note: pimdmMapIntfIsConfigurable() can return L7_FALSE for two reasons:
   * no matching config table entry was found, or the intIfNum is not valid 
   * for PIM-DM... the above call to pimdmIntfIsValid() assures that if we  
   * get L7_FALSE it is due to the first reason and we should try to assign 
   * a config table entry for this intf.
   */
  if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, &pCfg) == L7_FALSE)
  {
    if (pimdmMapIntfConfigEntryGet(intIfNum, pimdmMapCbPtr, &pCfg) == L7_TRUE)
    {
      /* successfully assigned a new config table entry for this intf,
       * so initialize the entry to defaults.
       * Update the configuration structure with the config id 
       */
      NIM_CONFIG_ID_COPY(&pCfg->configId, &configId);
      /* since this is a newly assigned configuration entry, initialize
       * it to default settings.
       */
      pimdmIntfBuildDefaultConfigData(pCfg);
      pimdmRC = L7_SUCCESS;
    }
    else
    {
      /* unsuccessful in assigning config table entry for this intf.
       * report failure.
       */
      pimdmRC = L7_FAILURE;
    }
  }
  else
  {
    /* we found an existing config table entry for this intf */
    pimdmRC = L7_SUCCESS;
  }

  return(pimdmRC);
}

/*********************************************************************
* @purpose  Propogate Interface notifications to PIM-DM Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    correlator  @b{(input)} Correlator for event
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
                                   NIM_CORRELATOR_t correlator)
{                          
  NIM_EVENT_COMPLETE_INFO_t status;    
  pimdmMapMsg_t msg;

  status.intIfNum     = intIfNum;
  status.component    = L7_FLEX_PIMDM_MAP_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.response.reason  = NIM_ERR_RC_UNUSED;
  status.event        = event;
  status.correlator   = correlator;

  /* Filter out the events that PIM-DM Map is not interested in.
   */
  if ((event != L7_CREATE) &&
      (event != L7_DELETE) &&
      (event != L7_ACTIVE) &&
      (event != L7_INACTIVE) &&
      (event != L7_LAG_ACQUIRE) &&
      (event != L7_PROBE_SETUP) &&
      (event != L7_LAG_RELEASE) &&
      (event != L7_PROBE_TEARDOWN))
  {
    nimEventStatusCallback (status);
    return L7_SUCCESS;
  }

  memset(&msg, 0, sizeof(pimdmMapMsg_t));
  msg.msgId = PIMDMMAP_INTF_CHANGE_MSG;

  msg.u.pimdmMapIntfChangeParms.intIfNum = intIfNum;
  msg.u.pimdmMapIntfChangeParms.event = event;
  msg.u.pimdmMapIntfChangeParms.correlator = correlator;

  if (pimdmMessageSend (PIMDM_EVENT_Q,(L7_VOIDPTR) &msg) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to post event(%d) to \
                       PIM-DM Map Queue",event);
    nimEventStatusCallback (status);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process interface-related events
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    event       @b{(input)} Event On the Interface
* @param    correlator  @b{(input)} Correlator
*
* @returns  none
*
* @end
*********************************************************************/
void pimdmMapIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, 
                               NIM_CORRELATOR_t correlator)
{
  L7_uint32    cbIndex = L7_NULL;
  L7_uint32    protoMax = L7_NULL;
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;
  NIM_EVENT_COMPLETE_INFO_t status;    

  protoMax =  pimdmMapProtocolsMaxGet();

  for (cbIndex = PIMDM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimdmMapCbPtr = &pimdmMapCB_g[cbIndex];
    pimdmMapNimEventProcess(intIfNum, event, correlator, pimdmMapCbPtr);
  }

  status.intIfNum     = intIfNum;
  status.component    = L7_FLEX_PIMDM_MAP_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.response.reason  = NIM_ERR_RC_UNUSED;
  status.event        = event;
  status.correlator   = correlator;

  nimEventStatusCallback(status);

  return;
}
/*********************************************************************
* @purpose  Process interface-related NIM Events per protocol.
*
*
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    event           @b{(input)} Event On the Interface
* @param    correlator      @b{(input)} Correlator for event
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapNimEventProcess(L7_uint32 intIfNum, L7_uint32 event,
                   NIM_CORRELATOR_t correlator, pimdmMapCB_t *pimdmMapCbPtr)
{

  L7_uint32                 mode/*, linkState*/;
  pimdmCfgCkt_t             *pCfg = L7_NULLPTR;
  /*L7_uint32                 rtrIfNum = L7_NULL;
  L7_uint32                 currAcquired, prevAcquired = L7_NULL;*/

  PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_APIS,"Event Type is %d,intfNum is %d",event,intIfNum);

  if ((intIfNum == 0) ||
      ((intIfNum != 0) &&
       (pimdmIntfIsValid (intIfNum) != L7_TRUE)))
  {
    /* If PIM-DM is not interested in this interface, inform 
     * event issuer that we have completed processing.
     */
    return;
  }

  switch (event)
  {
  case L7_UP:
  case L7_DOWN:
    break;

  case L7_CREATE:
    pimdmIntfCreate(intIfNum, pimdmMapCbPtr);
    break;

  case L7_DELETE:
    if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, &pCfg) == L7_TRUE)
    {
      pimdmIntfBuildDefaultConfigData(pCfg);
      memset((void *)&(pCfg->configId), 0, sizeof(nimConfigID_t));
      pimdmMapCbPtr->pPimdmMapCfgMapTbl[intIfNum] = 0;
    }
    break;

  case L7_ATTACH:
    break;

  case L7_DETACH:
    break;

  case L7_ACTIVE:
    if ((pimdmMapPimdmAdminModeGet(pimdmMapCbPtr->familyType, 
        &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
    {
      if ((pimdmMapIntfAdminModeGet(pimdmMapCbPtr->familyType, intIfNum, 
          &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
      {
        if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) != L7_TRUE)
          (void)pimdmMapIntfModeApply(intIfNum, pimdmMapCbPtr, L7_ENABLE);
      }
    }
    break;

    case L7_INACTIVE:
    if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) == L7_TRUE)
    {
      (void)pimdmMapIntfModeApply(intIfNum, pimdmMapCbPtr, L7_DISABLE);
    }
    break;


#if 0
    case L7_LAG_ACQUIRE:
    if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType,
                                         intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      /* check if previously acquired */
      COMPONENT_ACQ_NONZEROMASK(pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].
          acquiredList, prevAcquired);

      /* update aquired list */
      COMPONENT_ACQ_SETMASKBIT(pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].
          acquiredList, L7_DOT3AD_COMPONENT_ID);

      /* If the interface was not previously acquired by some component, cause
       * the acquisition to be reflected in the state of the spanning tree 
       */
      if (prevAcquired == L7_FALSE)
      {
        if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) == L7_TRUE)
        {
          (void)pimdmMapIntfModeApply(intIfNum, pimdmMapCbPtr, L7_DISABLE);
        }
      }
    }
    break;

  case L7_PROBE_SETUP:
    if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType,
                                         intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      /* check if previously acquired */
      COMPONENT_ACQ_NONZEROMASK(pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].
          acquiredList, prevAcquired);

      /* update aquired list */
      COMPONENT_ACQ_SETMASKBIT(pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].
          acquiredList, L7_PORT_MIRROR_COMPONENT_ID);

      /* If the interface is not previously acquired by some component, cause
       * the acquisition to be reflected in the state of the spanning tree.
       */
      if (prevAcquired == L7_FALSE)
      {
        if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) == L7_TRUE)
        {
          (void)pimdmMapIntfModeApply(intIfNum, pimdmMapCbPtr, L7_DISABLE);
        }
      }
    }
    break;

  case L7_LAG_RELEASE:
    if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType,
                                         intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      /* update aquired list */
      COMPONENT_ACQ_CLRMASKBIT(pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].
          acquiredList, L7_DOT3AD_COMPONENT_ID);

      /* check if currently acquired */
      COMPONENT_ACQ_NONZEROMASK(pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].
          acquiredList, currAcquired);

      /* If the interface is not currently acquired by some component, cause
       * the acquisition to be reflected in the state of the spanning tree 
       */

      if (currAcquired == L7_FALSE)
      {
        if (nimGetIntfLinkState(intIfNum, &linkState) == L7_SUCCESS)
        {
          if (linkState == L7_FORWARDING)
          {
            if ((pimdmMapPimdmAdminModeGet(pimdmMapCbPtr->familyType, 
                 &mode)== L7_SUCCESS) && (mode == L7_ENABLE))
            {
              if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, 
                  &pCfg) == L7_TRUE && pCfg->mode == L7_ENABLE)
              {
                if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) != L7_TRUE)
                {
                  (void)pimdmMapIntfModeApply(intIfNum, 
                                              pimdmMapCbPtr, L7_ENABLE);
                }
              }
            }
          }
        }
      }
    }
    break;

  case L7_PROBE_TEARDOWN:
    if (mcastIpMapIntIfNumToRtrIntf(pimdmMapCbPtr->familyType,
                                         intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      /* update aquired list */
      COMPONENT_ACQ_CLRMASKBIT(pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].
          acquiredList, L7_PORT_MIRROR_COMPONENT_ID);

      /* check if currently acquired */
      COMPONENT_ACQ_NONZEROMASK(pimdmMapCbPtr->pPimdmIntfInfo[rtrIfNum].
          acquiredList, currAcquired);

      /* If the interface is not currently acquired by some component, cause
       * the acquisition to be reflected in the state of the spanning tree.
       */
      if (currAcquired == L7_FALSE)
      {
        if (nimGetIntfLinkState(intIfNum, &linkState) == L7_SUCCESS)
        {
          if (linkState == L7_FORWARDING)
          {
            if ((pimdmMapPimdmAdminModeGet(pimdmMapCbPtr->familyType, 
                &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
            {
              if (pimdmMapIntfIsConfigurable(pimdmMapCbPtr, intIfNum, 
                  &pCfg) == L7_TRUE && pCfg->mode == L7_ENABLE)
              {
                if (pimdmMapIntfIsOperational(pimdmMapCbPtr, intIfNum) != L7_TRUE)
                {
                  (void)pimdmMapIntfModeApply(intIfNum,
                                              pimdmMapCbPtr, L7_ENABLE);
                }
              }
            }
          }
        }
      }
    }
    break;

 #endif
  default:
    break;
  }

  return;
}

/*********************************************************************
* @purpose  Propogate MCAST Event notifications to PIM-DM Map task
*
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info.
*
* @returns  L7_SUCCESS  
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimdmMapMcastEventCallBack(L7_uint32 event, 
                                   void *pData, 
                                   ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{                          
  ASYNC_EVENT_COMPLETE_INFO_t completionData;
  pimdmMapMsg_t msg;

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  completionData.componentId     = L7_MRP_PIMDM;
  completionData.correlator      = pEventInfo->correlator;
  completionData.handlerId       = pEventInfo->handlerId;
  completionData.async_rc.rc     = L7_SUCCESS;
  completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;

  /* Filter out the events that PIM-DM Map is not interested in.
   */
  if ((event != MCAST_RTR_ADMIN_MODE_ENABLED) &&
      (event != MCAST_RTR_ADMIN_MODE_DISABLE_PENDING))
  {
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  memset(&msg, 0, sizeof(pimdmMapMsg_t));

  msg.msgId = PIMDMMAP_MCAST_EVENT_MSG;

  msg.u.pimdmMapMcastEventParms.event = event;
  msg.u.pimdmMapMcastEventParms.pData = pData;
  memcpy (&msg.u.pimdmMapMcastEventParms.eventInfo, pEventInfo, 
          sizeof(ASYNC_EVENT_NOTIFY_INFO_t));

  if (pimdmMessageSend (PIMDM_EVENT_Q,(L7_VOIDPTR) &msg) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to post event (%d) to \
                       PIM-DM Map Queue.\n",event);

    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate MCAST Event notifications to PIM-DM Map task
*
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info.
*
* @returns  none
*
* @comments none    
*
* @end
*********************************************************************/
void pimdmMapMcastEventProcess (L7_uint32 event, 
                                void *pData, 
                                ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_uint32                   cbIndex = L7_NULL;
  L7_uint32                   protoMax = L7_NULL;
  pimdmMapCB_t                *pimdmMapCbPtr = L7_NULLPTR;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  protoMax =  pimdmMapProtocolsMaxGet();
  for (cbIndex = PIMDM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimdmMapCbPtr = &pimdmMapCB_g[cbIndex];

    /* process Mcast Events. */
    pimdmMapMcastCommonEventProcess(event, pimdmMapCbPtr);
  }

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  completionData.componentId     = L7_MRP_PIMDM;
  completionData.correlator      = pEventInfo->correlator;
  completionData.handlerId       = pEventInfo->handlerId;
  completionData.async_rc.rc     = L7_SUCCESS;
  completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;

  /* Return event completion status  */
  asyncEventCompleteTally(&completionData);

  return;
}

/*********************************************************************
* @purpose  Process Mcast related Events.
*
* @param    event            @b{(input)} Event On the Interface
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  None.
*
*
* @end
*********************************************************************/
void pimdmMapMcastCommonEventProcess (L7_uint32 event,
                                      pimdmMapCB_t *pimdmMapCbPtr)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (event)
  {
  case MCAST_RTR_ADMIN_MODE_ENABLED:
    if (pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode == L7_ENABLE)
    {
       rc = pimdmMapPimdmAdminModeEnable(pimdmMapCbPtr, L7_FALSE);
       if (rc != L7_SUCCESS)
       {
         PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                            "Failed to enable PIMDM routing functionality for \
                             for event %d for familyType %d\n",event,
                             pimdmMapCbPtr->familyType);
       }
    }
    break;

  case MCAST_RTR_ADMIN_MODE_ENABLE_PENDING:
    break;

  case MCAST_RTR_ADMIN_MODE_DISABLE_PENDING:
    if (pimdmMapCbPtr->pPimdmMapCfgData->rtr.adminMode == L7_ENABLE)
    {
      rc = pimdmMapPimdmAdminModeDisable(pimdmMapCbPtr, L7_FALSE);
      if (rc != L7_SUCCESS)
      {
         PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                            "Failed to disable PIMDM routing functionality for \
                             for event %d for familyType %d\n",event,
                             pimdmMapCbPtr->familyType);
      }
    }
    break;

  case MCAST_RTR_ADMIN_MODE_DISABLED:
    break;

  default:
    break;
  }
  return;
}

/*********************************************************************
* @purpose  Process protocol-related events
*
* @param    familyType    @b{(input)} address family
* @param    event       @b{(input)} Event.
* @param    pData       @b{(input)} Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmMapProtoEventChangeProcess(L7_uchar8 familyType, L7_uint32 event, 
                                     void *pData)
{
  if (pimdmEventProcess(familyType, event, pData) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "PIMDM Event Process Failed for event %d",event);

    /* It might be possible that the MRP might be disabled after the MGMD
     * event is enqueued.  So, free the allocated MGMD Event buffers.
     */
    if (event == MCAST_EVENT_MGMD_GROUP_UPDATE)
    {
      /* Free the MCAST MGMD Events Source List Buffers */
      mcastMgmdEventsBufferPoolFree (familyType, (mgmdMrpEventInfo_t *) pData);
    }
    if (event == MCAST_EVENT_PIMDM_CONTROL_PKT_RECV ||
        event == MCAST_EVENT_IPv6_CONTROL_PKT_RECV)
    {
      mcastCtrlPktBufferPoolFree(familyType, ((mcastControlPkt_t *) pData)->payLoad);
    }
    return;
  }
  return;
}

/*********************************************************************
* @purpose  Build default PIMDM config data for an intf 
*
* @param    pCfg     @b{(output)} pointer to the PIMDM ckt 
*                                 configuration record.
*
* @returns  none
*
* @comments none 
*
* @end
*********************************************************************/
void pimdmIntfBuildDefaultConfigData(pimdmCfgCkt_t *pCfg)
{
  pCfg->mode                      = FD_PIMDM_INTF_DEFAULT_MODE;
  pCfg->pimInterfaceHelloInterval = FD_PIMDM_INTF_DEFAULT_HELLO_INTERVAL;
}

/*********************************************************************
* @purpose  Build default PIMDM config data for an intf 
*
* @param    timerCtrlBlk @b{(input)}  pointer to the PIMDM ckt 
*                                      configuration record.
* @param    param        @b{(input)}  pointer  to param
*
* @returns  void
*
* @comments none 
*
* @end
*********************************************************************/
void pimdmMapAppTimerExpiryHandler (L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, 
                                                          void *param)
{
  appTimerProcess (timerCtrlBlk);
}

/*********************************************************************
* @purpose  Send event to PIMDM Vendor
*
* @param    familyType   @b{(input)} Address Family type
* @param    eventType    @b{(input)} Event Type 
* @param    msgLen       @b{(input)} Message Length
* @param    eventMsg     @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapEventChangeCallback (L7_uchar8 familyType,
                                     L7_uint32 eventType, 
                                     L7_uint32 msgLen, 
                                     void *eventMsg)
{
  L7_uint32 rtrIfNum = 0;
  L7_uint32 intIfNum = 0;
  L7_BOOL doIntfOperationalCheck = L7_FALSE;

  if ((eventType != MCAST_EVENT_PIMDM_CONTROL_PKT_RECV) &&
      (eventType != MCAST_EVENT_IPv6_CONTROL_PKT_RECV) &&
      (eventType != MCAST_MFC_NOCACHE_EVENT) &&
      (eventType != MCAST_MFC_ENTRY_EXPIRE_EVENT) &&
      (eventType != MCAST_MFC_WRONGIF_EVENT) &&
      (eventType != MCAST_EVENT_MGMD_GROUP_UPDATE) &&
      (eventType != MCAST_EVENT_ADMINSCOPE_BOUNDARY) &&
      (eventType != MCAST_EVENT_STATIC_MROUTE_CHANGE) &&
      (eventType != MCAST_EVENT_RTO_BEST_ROUTE_CHANGE) &&
      (eventType != MCAST_EVENT_PIMDM_TIMER_EXPIRY))
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, "\nInput parameters are not"
                    "valid for eventType:%d.\n", eventType);
    return L7_FAILURE;
  }

  if (pimdmMapPimDmIsOperational(familyType) != L7_TRUE)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                      "Received event when pimdm is not operational");
    return L7_FAILURE;
  }
  /* Check if PIM-DM is Operational on the interface to which the event is
   * addressed to.
   * As of now, we do it only for events from MFC and MGMD.
   */
  switch (eventType)
  {
    case MCAST_EVENT_MGMD_GROUP_UPDATE:
      rtrIfNum = ((mgmdMrpEventInfo_t*)eventMsg)->rtrIfNum;
      doIntfOperationalCheck = L7_TRUE;
      break;

    case MCAST_EVENT_ADMINSCOPE_BOUNDARY:
      rtrIfNum = ((mcastAdminMsgInfo_t *)eventMsg)->intIfNum;
      doIntfOperationalCheck = L7_TRUE;
      break;

    case MCAST_EVENT_STATIC_MROUTE_CHANGE:
      rtrIfNum = ((mcastRPFInfo_t *)eventMsg)->rpfIfIndex;
      doIntfOperationalCheck = L7_TRUE;
      break;

    case MCAST_MFC_NOCACHE_EVENT:
    case MCAST_MFC_WRONGIF_EVENT:
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
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                      "Failed to convert rtrIfNum (%d) to intIfNum for "
                      "familyType %d", rtrIfNum, familyType);
      return L7_FAILURE;
    }
    if (pimdmMapInterfaceIsOperational (familyType, intIfNum) != L7_TRUE)
    {
      PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                      "PIM-DM is not Operational on rtrIfNum - %d for Family - %d"
                      " for eventType - %d", rtrIfNum, familyType, eventType);
      return L7_FAILURE;
    }
  }

  if (pimdmMapMessageQueueSend(familyType, eventType, 
                               msgLen, eventMsg) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to post event(%d) to "
                     "the PIMDM Map Message Queue for familyType %d",eventType,
                     familyType);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send AdminScope Boundary event to PIMDM Vendor
*
* @param    eventType    @b{(input)} Event Type 
* @param    msgLen       @b{(input)} Message Length
* @param    eventMsg     @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is an IPv4 specific function
*
* @end
*********************************************************************/
L7_RC_t pimdmMapAdminScopeEventChangeCallback (L7_uint32 eventType, 
                                               L7_uint32 msgLen, 
                                               void *eventMsg)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;
  L7_uint32 gblAdminMode = L7_DISABLE;

  /* Get the interface CB based on family */
  if (pimdmMapCtrlBlockGet(L7_AF_INET, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                       "Failed to get control block for family type %d\n",
                        L7_AF_INET);
    return L7_FAILURE;
  }

  /* Post the event if PIM-DM Map Global Admin mode is enabled.
   */
  if (pimdmMapPimdmAdminModeGet (L7_AF_INET, &gblAdminMode) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "PIMDM Global Admin Mode not enabled");
    return L7_FAILURE;
  }
  else
  {
    if (gblAdminMode != L7_ENABLE)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "PIMDM Global Admin Mode not enabled");
      return L7_FAILURE;
    }
  }

  if (pimdmMapMessageQueueSend(L7_AF_INET, eventType, 
                               msgLen, eventMsg) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,"Failed to post event(%d) to \
                     the PIMDM Map Message Queue for familyType %d",eventType,
                     L7_AF_INET);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process UI events
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
L7_RC_t pimdmMapUIEventChangeProcess (MCAST_CB_HNDL_t cbHandle,
                                      PIMDMMAP_UI_EVENT_TYPE_t event, 
                                      void *pData)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;
  pimdmMapUIEventParms_t *uiEvent = (pimdmMapUIEventParms_t*) pData;

  if ((pimdmMapCbPtr = (pimdmMapCB_t*) cbHandle) == L7_NULLPTR)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"Invalid PIM-DM MAP CB "
                    "Handle(NULL)");
    return L7_FAILURE;
  }

  if (uiEvent == L7_NULLPTR)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,"Dequeued Buffer is NULL "
                     "for Event %d\n",event);
    return L7_FAILURE;
  }

  switch (event)
  {
    case PIMDMMAP_ADMIN_MODE_SET:
    {
      pimdmMapUiAdminMode_t *adminMode = &(uiEvent->eventInfo.adminMode);

      if (adminMode->mode == L7_ENABLE)
      {
        pimdmMapPimdmAdminModeEnable (pimdmMapCbPtr, L7_TRUE);
      }
      else if (adminMode->mode == L7_DISABLE)
      {
        pimdmMapPimdmAdminModeDisable (pimdmMapCbPtr, L7_TRUE);
      }

      break;
    }

    case PIMDMMAP_INTF_MODE_SET:
    {
      pimdmMapUiIntfMode_t *intfMode = &(uiEvent->eventInfo.intfMode);
      L7_uint32 pimAdminMode;

      if (intfMode->mode == L7_ENABLE)
      {
        if ((pimdmMapPimdmAdminModeGet (pimdmMapCbPtr->familyType, &pimAdminMode)
        	                            == L7_SUCCESS) &&
        	   (pimAdminMode == L7_ENABLE))
        {
          pimdmMapIntfModeApply (intfMode->intIfNum, pimdmMapCbPtr, L7_ENABLE);
        }
      }
      else if (intfMode->mode == L7_DISABLE)
      {
        pimdmMapIntfModeApply (intfMode->intIfNum, pimdmMapCbPtr, L7_DISABLE);
      }

      break;
    }

    case PIMDMMAP_HELLO_INTVL_SET:
    {
      pimdmMapUiHelloIntvl_t *helloIntvl = &(uiEvent->eventInfo.helloIntvl);

      pimdmMapIntfHelloIntervalApply (pimdmMapCbPtr, helloIntvl->intIfNum,
                                      helloIntvl->interval);

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
L7_RC_t pimdmMapUIEventSend (pimdmMapCB_t *pimdmMapCbPtr,
                             PIMDMMAP_UI_EVENT_TYPE_t eventType, 
                             L7_uint32 mode,
                             L7_uint32 intIfNum,
                             L7_uint32 helloIntvl)
{
  pimdmMapMsg_t msg;

  memset(&msg, 0, sizeof(pimdmMapMsg_t));

  if ((eventType != PIMDMMAP_ADMIN_MODE_SET) &&
      (eventType != PIMDMMAP_INTF_MODE_SET) &&
      (eventType != PIMDMMAP_HELLO_INTVL_SET))
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Invalid eventType:%d for \
                     familyType %d\n",eventType,pimdmMapCbPtr->familyType);
    return L7_FAILURE;
  }

  msg.msgId = PIMDMMAP_UI_EVENT_MSG;

  msg.u.pimdmMapUiEventParms.event = eventType;
  msg.u.pimdmMapUiEventParms.cbHandle = (MCAST_CB_HNDL_t) pimdmMapCbPtr;

  switch (eventType)
  {
    case PIMDMMAP_ADMIN_MODE_SET:
      msg.u.pimdmMapUiEventParms.eventInfo.adminMode.mode = mode;
      break;

    case PIMDMMAP_INTF_MODE_SET:
      msg.u.pimdmMapUiEventParms.eventInfo.intfMode.intIfNum = intIfNum;
      msg.u.pimdmMapUiEventParms.eventInfo.intfMode.mode = mode;
      break;

    case PIMDMMAP_HELLO_INTVL_SET:
      msg.u.pimdmMapUiEventParms.eventInfo.helloIntvl.intIfNum = intIfNum;
      msg.u.pimdmMapUiEventParms.eventInfo.helloIntvl.interval = helloIntvl;
      break;

    default:
      break;
  }

  if (pimdmMessageSend (PIMDM_EVENT_Q,(L7_VOIDPTR) &msg) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, 
                      "Event Post Failed for eventType(%d)",eventType);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To get the vendor(protocol) CB handle based on family Type
*
* @param    familyType    @b{(input)} Address Family type.
*
* @returns  cbHandle return cbHandle, else L7_NULLPTR
*
* @comments none
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t pimdmMapProtocolCtrlBlockGet(L7_uchar8 familyType)
{
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;

  if (pimdmMapCtrlBlockGet(familyType, &pimdmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                     "Invalid Mapping CB for Family - %d", familyType);
    return L7_NULLPTR;
  }

  if ((pimdmMapCbPtr != L7_NULLPTR) &&
      (pimdmMapCbPtr->cbHandle != L7_NULLPTR))
  {
    return pimdmMapCbPtr->cbHandle;
  }

  PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Invalid Vendor CB for Family - %d",
                   familyType);
  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  To send the event to Message Queue
*
* @param    familyType    @b{(input)} address family
* @param    eventType  @b{(input)} Event Type
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
L7_RC_t pimdmMapMessageQueueSend(L7_uchar8 familyType, L7_uint32 eventType,
                                 L7_uint32 msgLen, L7_VOIDPTR pMsg)
{
  pimdmMapMsg_t msg;
  pimdmMapCtrlPktAndMfcMsg_t pktAndMfcMsg;
  pimdmMapAppTmrMsg_t appTmrMsg;
  L7_RC_t rc;
  L7_uint32 QIndex=PIMDM_MAX_Q;
  L7_VOIDPTR sendBuffer=L7_NULLPTR;

  if ((eventType != MCAST_EVENT_PIMDM_CONTROL_PKT_RECV) &&
      (eventType != MCAST_EVENT_IPv6_CONTROL_PKT_RECV) &&
      (eventType != MCAST_MFC_NOCACHE_EVENT) &&
      (eventType != MCAST_MFC_WRONGIF_EVENT) &&
      (eventType != MCAST_MFC_ENTRY_EXPIRE_EVENT) &&
      (eventType != MCAST_EVENT_MGMD_GROUP_UPDATE) &&
      (eventType != MCAST_EVENT_ADMINSCOPE_BOUNDARY) &&
      (eventType != MCAST_EVENT_STATIC_MROUTE_CHANGE) &&
      (eventType != MCAST_EVENT_RTO_BEST_ROUTE_CHANGE) &&
      (eventType != MCAST_EVENT_PIMDM_TIMER_EXPIRY))
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES, "\nInput parameters are not"
                    "valid for eventType:%d.\n", eventType);
    return L7_FAILURE;
  }

  memset(&msg, 0, sizeof(pimdmMapMsg_t));

  if((eventType == MCAST_EVENT_PIMDM_CONTROL_PKT_RECV) ||
     (eventType == MCAST_EVENT_IPv6_CONTROL_PKT_RECV) ||
     (eventType == MCAST_MFC_NOCACHE_EVENT) ||
     (eventType == MCAST_MFC_WRONGIF_EVENT) ||
     (eventType == MCAST_MFC_ENTRY_EXPIRE_EVENT))
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_EVENTS,"CtrlPkt/MFC Event Received\n");

    if((eventType == MCAST_MFC_NOCACHE_EVENT) ||
       (eventType == MCAST_MFC_WRONGIF_EVENT) ||
       (eventType == MCAST_MFC_ENTRY_EXPIRE_EVENT))
    {
      if (pimdmMapCountersValueGet (PIMDM_MAP_NUM_MFC_EVENTS) >= PIMDMMAP_MFC_EVENTS_COUNT_LIMIT)
      {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"Dropping the MFC Event as "
                         "there is not enough Event Queue space.\n");
        pimdmMapCountersUpdate (PIMDM_MAP_NUM_MFC_OVERFLOW_EVENTS, PIMDM_MAP_COUNTERS_INCREMENT);        
        if (osapiSemaTake (pimdmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                        == L7_SUCCESS)
        {
          pimdmGblVariables_g.pimdmQueue[PIMDM_CTRL_PKT_Q].QSendFailedCnt++;
          osapiSemaGive (pimdmGblVariables_g.queueCountersSemId);
        }
        return L7_FAILURE;
      }

      pimdmMapCountersUpdate (PIMDM_MAP_NUM_MFC_EVENTS, PIMDM_MAP_COUNTERS_INCREMENT);
    }

    memset(&pktAndMfcMsg,0,sizeof(pimdmMapCtrlPktAndMfcMsg_t));
    pktAndMfcMsg.msgId = PIMDMMAP_PROTOCOL_EVENT_MSG;

    pktAndMfcMsg.pimdmMapCtrlPkAndMfctParms.event = eventType;
    pktAndMfcMsg.pimdmMapCtrlPkAndMfctParms.familyType = familyType;

    if ((pMsg != L7_NULLPTR) && (msgLen > 0))
    {
      memcpy(&(pktAndMfcMsg.pimdmMapCtrlPkAndMfctParms.u), pMsg, msgLen);
    }

    QIndex =  PIMDM_CTRL_PKT_Q;
    sendBuffer = (L7_VOIDPTR)&pktAndMfcMsg;

  }
  else if(eventType == MCAST_EVENT_PIMDM_TIMER_EXPIRY)
  {
    memset(&appTmrMsg, 0, sizeof(pimdmMapAppTmrMsg_t));
    appTmrMsg.msgId = PIMDMMAP_PROTOCOL_EVENT_MSG;
    appTmrMsg.pimdmMapAppTmrParms.event = eventType;
    appTmrMsg.pimdmMapAppTmrParms.familyType = familyType;

    QIndex =  PIMDM_APP_TIMER_Q;
    sendBuffer = (L7_VOIDPTR)&appTmrMsg;
  }
  else
  {
    /* Regulate the MGMD Events */
    if (eventType == MCAST_EVENT_MGMD_GROUP_UPDATE)
    {
      if (pimdmMapCountersValueGet (PIMDM_MAP_NUM_MGMD_EVENTS) >= PIMDMMAP_MGMD_EVENTS_COUNT_LIMIT)
      {
        PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,"Dropping the MGMD Event as "
                         "there is not enough Event Queue space.\n");
        pimdmMapCountersUpdate (PIMDM_MAP_NUM_MGMD_OVERFLOW_EVENTS, PIMDM_MAP_COUNTERS_INCREMENT);
                 
        if (osapiSemaTake (pimdmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                        == L7_SUCCESS)
        {
          pimdmGblVariables_g.pimdmQueue[PIMDM_EVENT_Q].QSendFailedCnt++;
          osapiSemaGive (pimdmGblVariables_g.queueCountersSemId);
        }
        return L7_FAILURE;
      }
      pimdmMapCountersUpdate (PIMDM_MAP_NUM_MGMD_EVENTS, PIMDM_MAP_COUNTERS_INCREMENT);
    }  
    msg.msgId = PIMDMMAP_PROTOCOL_EVENT_MSG;

    msg.u.pimdmMapProtocolEventParms.event = eventType;
    msg.u.pimdmMapProtocolEventParms.familyType = familyType;
    if ((pMsg != L7_NULLPTR) && (msgLen > 0))
    {
      memcpy(&(msg.u.pimdmMapProtocolEventParms.msgData), pMsg, msgLen);
    }
    QIndex =  PIMDM_EVENT_Q;
    sendBuffer = (L7_VOIDPTR)&msg;

  }
  rc = pimdmMessageSend(QIndex,(L7_VOIDPTR)sendBuffer);

  if(rc != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG(PIMDM_MAP_DEBUG_FAILURES,
                    "Failed to send Msg to PIMDM Queue for event(%d)",eventType);

    /* Roll-back the Queue-limiting counters, if any */
    if ((eventType == MCAST_MFC_NOCACHE_EVENT)||
        (eventType == MCAST_MFC_WRONGIF_EVENT) ||
        (eventType == MCAST_MFC_ENTRY_EXPIRE_EVENT))
    {
      pimdmMapCountersUpdate (PIMDM_MAP_NUM_MFC_EVENTS, PIMDM_MAP_COUNTERS_DECREMENT);
    }
    else if (eventType == MCAST_EVENT_MGMD_GROUP_UPDATE)
    {
      pimdmMapCountersUpdate (PIMDM_MAP_NUM_MGMD_EVENTS, PIMDM_MAP_COUNTERS_DECREMENT);
    }

    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Wrapper function to send the the message on the appropriate Q
*
* @param    QIndex    @b{(input)} Index to the Queue on which message is to be sent.
* @param    msg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
**********************************************************************/

L7_RC_t pimdmMessageSend(PIMDM_QUEUE_ID_t QIndex,void *msg)
{
  if (QIndex >= PIMDM_MAX_Q)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "PIMDM Map wrong queueIndex:%d \n", QIndex);
    return L7_FAILURE;
  }

  if (osapiMessageSend (pimdmGblVariables_g.pimdmQueue[QIndex].QPointer, (L7_VOIDPTR) msg,
                        pimdmGblVariables_g.pimdmQueue[QIndex].QSize, L7_NO_WAIT, L7_MSG_PRIORITY_NORM)
                     != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "PIMDM Map Queue of queueIndex:%d  Failed while "
                      "sending the message", QIndex);
    if (osapiSemaTake (pimdmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                    == L7_SUCCESS)
    {
      pimdmGblVariables_g.pimdmQueue[QIndex].QSendFailedCnt++;
      osapiSemaGive (pimdmGblVariables_g.queueCountersSemId);
    }
    return L7_FAILURE;
  }

  if (osapiSemaTake (pimdmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                  == L7_SUCCESS)
  {
    pimdmGblVariables_g.pimdmQueue[QIndex].QCurrentMsgCnt++;
    if (pimdmGblVariables_g.pimdmQueue[QIndex].QCurrentMsgCnt >
                pimdmGblVariables_g.pimdmQueue[QIndex].QMaxRx)
    {
      pimdmGblVariables_g.pimdmQueue[QIndex].QMaxRx =
              pimdmGblVariables_g.pimdmQueue[QIndex].QCurrentMsgCnt;
    }
    osapiSemaGive (pimdmGblVariables_g.queueCountersSemId);
  }

  osapiSemaGive (pimdmGblVariables_g.msgQSema);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Function to delete the Queue
*
* @param    None
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
**********************************************************************/

L7_RC_t pimdmMapQDelete(void)
{
  L7_uint32 count;

  for(count =0; count <PIMDM_MAX_Q;count++)
  {
    if(pimdmGblVariables_g.pimdmQueue[count].QPointer != L7_NULLPTR)
    {
      osapiMsgQueueDelete(pimdmGblVariables_g.pimdmQueue[count].QPointer);
    }
  }
  return L7_SUCCESS;
}


/********************** PIMDM DEBUG TRACE ROUTINES *******************/

/*********************************************************************
 * @purpose  Saves pimdm debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    pimdmDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t pimdmDebugSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Copy the operational states into the config file */
  pimdmDebugCfgUpdate();

  if (pimdmDebugHasDataChanged() == L7_TRUE)
  {
    pimdmDebugCfg.hdr.dataChanged = L7_FALSE;
    pimdmDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&pimdmDebugCfg,
        (L7_uint32)(sizeof(pimdmDebugCfg) - sizeof(pimdmDebugCfg.checkSum)));
    /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_FLEX_PIMDM_MAP_COMPONENT_ID, PIMDM_DEBUG_CFG_FILENAME ,
            (L7_char8 *)&pimdmDebugCfg, (L7_uint32)sizeof(pimdmDebugCfg_t))) == L7_ERROR)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error on call to osapiFsWrite routine on config file %s\n",PIMDM_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
 * @purpose  Restores pimdm debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    pimdmDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t pimdmDebugRestore(void)
{
  L7_RC_t rc;

  pimdmDebugBuildDefaultConfigData(PIMDM_DEBUG_CFG_VER_CURRENT);

  pimdmDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = pimdmApplyDebugConfigData();

  return rc;
}
/*********************************************************************
 * @purpose  Checks if pimdm debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL pimdmDebugHasDataChanged(void)
{
  return pimdmDebugCfg.hdr.dataChanged;
}

/*********************************************************************
 * @purpose  Build default pimdm config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void pimdmDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  pimdmDebugCfg.hdr.version = ver;
  pimdmDebugCfg.hdr.componentID = L7_FLEX_PIMDM_MAP_COMPONENT_ID;
  pimdmDebugCfg.hdr.type = L7_CFG_DATA;
  pimdmDebugCfg.hdr.length = (L7_uint32)sizeof(pimdmDebugCfg);
  strcpy((L7_char8 *)pimdmDebugCfg.hdr.filename, PIMDM_DEBUG_CFG_FILENAME);
  pimdmDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&pimdmDebugCfg.cfg, 0, sizeof(pimdmDebugCfg.cfg));
}

/*********************************************************************
 * @purpose  Apply pimdm debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t pimdmApplyDebugConfigData(void)
{
  L7_uchar8 familyIndex = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 flagIndex;

  for (familyIndex = 0; familyIndex < PIMDM_MAP_CB_MAX; 
       familyIndex++)
  {
    pimdmDebugTraceFlags_t *pimdmDebugTraceFlags = 
       &(pimdmDebugCfg.cfg.pimdmDebugTraceFlag[familyIndex]);
    for(flagIndex = 0;  flagIndex < PIMDM_DEBUG_LAST_TRACE ; flagIndex ++)
    { 
       if(((*(pimdmDebugTraceFlags[flagIndex/PIMDM_DEBUG_TRACE_FLAG_BITS_MAX])) & 
          (PIMDM_DEBUG_TRACE_FLAG_VALUE << (flagIndex % PIMDM_DEBUG_TRACE_FLAG_BITS_MAX))) != 0)
       {
         pimdmDebugTraceFlagSet(familyIndex+1, flagIndex, L7_TRUE);
       }
       else
       {
         pimdmDebugTraceFlagSet(familyIndex+1, flagIndex, L7_FALSE);
       }
    }
  }
  return rc;
}


