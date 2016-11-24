
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    pimsm_vend_ctrl.c
*
* @purpose     PIM-SM vendor-specific internal definitions
*
* @component   PIM-SM Mapping Layer
*
* @comments    none
*
* @create      03/08/2002
*
* @author      Ratnakar
*
* @end
*
**********************************************************************/
#include "l7_pimsminclude.h"
#include "pim_defs.h"
#include "pimsm_map.h"
#include "l7_mcast_api.h"
#include "pimsm_map_util.h"
#include "pimsm_v6_wrappers.h"
extern pimsmGblVariables_t pimsmGblVariables_g;
extern pimsmMapCB_t        *pimsmMapCB;

/*********************************************************************
* @purpose  To get the vendor CB handle based on family Type
*
* @param    familyType   @b{(input)}   Address Family type.
*
* @returns  cbHandle     return cbHandle, 
* @returns  L7_NULLPTR   on failure
*
* @comments none
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t pimsmMapProtocolCtrlBlockGet(L7_uchar8 familyType)
{
  pimsmMapCB_t    *pimsmMapCbPtr = L7_NULLPTR;

  if (pimsmMapCtrlBlockGet(familyType, &pimsmMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES,
                     "Invalid Mapping CB for Family - %d", familyType);
    return L7_NULLPTR;
  }

  if ((pimsmMapCbPtr != L7_NULLPTR) &&
      (pimsmMapCbPtr->cbHandle != L7_NULLPTR))
  {
    return pimsmMapCbPtr->cbHandle;
  }

  PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Invalid Vendor CB for Family - %d",
                   familyType);
  return L7_NULLPTR;
}


/*********************************************************************
* @purpose  To send the event to Message Queue
*
* @param    cbHandle    @b{(input)} pimdm CB Handle.
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
L7_RC_t pimsmMapMessageQueueSend(L7_uchar8 familyType, L7_uint32 eventType,
                                 L7_uint32 msgLen, L7_VOIDPTR pMsg)
{
  pimsmMapMsg_t msg;
  pimsmMapCtrlPktAndMfcMsg_t pktAndMfcMsg;
  pimsmMapAppTmrMsg_t  appTmrMsg;
  pimsmMapDataPktMsg_t dataMsg;
  
  L7_RC_t rc;
  L7_uint32 QIndex =PIMSM_MAX_Q;
  L7_VOIDPTR  sendBuffer = L7_NULLPTR;
  L7_char8 val;
  L7_uint32 type, version;

  if ((eventType != MCAST_MFC_WRONGIF_EVENT) &&
      (eventType != MCAST_MFC_NOCACHE_EVENT) &&
      (eventType != MCAST_MFC_WHOLEPKT_EVENT) &&
      (eventType != MCAST_MFC_ENTRY_EXPIRE_EVENT) &&      
      (eventType != MCAST_EVENT_MGMD_GROUP_UPDATE) &&
      (eventType != MCAST_EVENT_RTO_BEST_ROUTE_CHANGE) &&
      (eventType != MCAST_EVENT_PIMSM_CONTROL_PKT_RECV) &&
      (eventType != MCAST_EVENT_PIMSM_TIMER_EXPIRY) &&
      (eventType != MCAST_EVENT_IPv6_CONTROL_PKT_RECV) &&
      (eventType != MCAST_EVENT_STATIC_MROUTE_CHANGE) &&
      (eventType != MCAST_EVENT_ADMINSCOPE_BOUNDARY))
  {
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Invalid event Type (%d)",eventType);
    return L7_FAILURE;
  }

  if((eventType == MCAST_EVENT_PIMSM_CONTROL_PKT_RECV) ||
     (eventType == MCAST_EVENT_IPv6_CONTROL_PKT_RECV)||
     (eventType == MCAST_MFC_NOCACHE_EVENT )||
     (eventType == MCAST_MFC_WRONGIF_EVENT)||
     (eventType == MCAST_MFC_ENTRY_EXPIRE_EVENT))
  {
    if ((pMsg != L7_NULLPTR) && (msgLen > 0))
    {
      mcastControlPkt_t     *pMcastPkt;
      L7_char8 *pPimHeader;

      if((eventType == MCAST_MFC_NOCACHE_EVENT)||
         (eventType == MCAST_MFC_WRONGIF_EVENT) ||
         (eventType == MCAST_MFC_ENTRY_EXPIRE_EVENT))
      {
        if(pimsmMapCountersValueGet (PIMSM_MAP_NUM_MFC_EVENTS) >= PIMSMMAP_MFC_EVENTS_COUNT_LIMIT)
        {
          pimsmMapCountersUpdate (PIMSM_MAP_NUM_MFC_OVERFLOW_EVENTS, PIMSM_MAP_COUNTERS_INCREMENT);
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Dropping the MFC Event as "
                           "there is not enough Event Queue space.\n");
          if (osapiSemaTake (pimsmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                          == L7_SUCCESS)
          {
            pimsmGblVariables_g.pimsmQueue[PIMSM_CTRL_PKT_Q].QSendFailedCnt++;
            osapiSemaGive (pimsmGblVariables_g.queueCountersSemId);
          }
          return L7_FAILURE;
        }
        pimsmMapCountersUpdate (PIMSM_MAP_NUM_MFC_EVENTS, PIMSM_MAP_COUNTERS_INCREMENT);
        memset(&pktAndMfcMsg,0,sizeof(pimsmMapCtrlPktAndMfcMsg_t));
        pktAndMfcMsg.msgId = PIMSMMAP_PROTOCOL_EVENT_MSG;
    
        pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.event = eventType;
        pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.familyType = familyType;
        
        memcpy(&(pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.u), pMsg, msgLen);
        QIndex =  PIMSM_CTRL_PKT_Q;
        sendBuffer = (L7_VOIDPTR)&pktAndMfcMsg;
      }
      else
      {
      pMcastPkt = (mcastControlPkt_t *) pMsg;
      pPimHeader = pMcastPkt->payLoad;
      val = *(L7_uchar8 *)pPimHeader;
      version = val >> 4;
      type = val& 0x0f;
      PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS,"PIM Version (%d) & Type (%d)",version, type);

      /* Enqueue the PIM Register packets into the Data Packet Queue.
       * All the remaining PIM Control Packets will be enqueued to the
       * Control Pakcet Queue.
       */
      if ((version == PIM_PROTOCOL_VERSION) && (type == PIM_PDU_REGISTER))
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS,"PIM-Regsiter/Stop pkt Type ");
        memset(&dataMsg, 0, sizeof(pimsmMapDataPktMsg_t));
        dataMsg.msgId = PIMSMMAP_PROTOCOL_EVENT_MSG;
    
        dataMsg.pimsmMapDataPktEventParms.event = eventType;
        dataMsg.pimsmMapDataPktEventParms.familyType = familyType;
        memcpy(&(dataMsg.pimsmMapDataPktEventParms.u), pMsg, msgLen);
        QIndex =  PIMSM_DATA_PKT_Q;
        sendBuffer = (L7_VOIDPTR)&dataMsg;
      }
      else 
      {
          memset(&pktAndMfcMsg,0,sizeof(pimsmMapCtrlPktAndMfcMsg_t));
          pktAndMfcMsg.msgId = PIMSMMAP_PROTOCOL_EVENT_MSG;
    
          pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.event = eventType;
          pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.familyType = familyType;
        
          memcpy(&(pktAndMfcMsg.pimsmMapCtrlPktAndMfcParms.u), pMsg, msgLen);
        QIndex =  PIMSM_CTRL_PKT_Q;
          sendBuffer = (L7_VOIDPTR)&pktAndMfcMsg;
        }
      }
    }

  }
  else if(eventType == MCAST_EVENT_PIMSM_TIMER_EXPIRY)
  {
    memset(&appTmrMsg, 0, sizeof(pimsmMapAppTmrMsg_t));
    appTmrMsg.msgId = PIMSMMAP_PROTOCOL_EVENT_MSG;
    appTmrMsg.pimsmMapAppTmrParms.event = eventType;
    appTmrMsg.pimsmMapAppTmrParms.familyType = familyType;

    QIndex =  PIMSM_APP_TIMER_Q;
    sendBuffer = (L7_VOIDPTR)&appTmrMsg;
  }
  else
  {
    if (eventType == MCAST_MFC_WHOLEPKT_EVENT)
    {
      memset(&dataMsg, 0, sizeof(pimsmMapDataPktMsg_t));
      dataMsg.msgId = PIMSMMAP_PROTOCOL_EVENT_MSG;
    
      dataMsg.pimsmMapDataPktEventParms.event = eventType;
      dataMsg.pimsmMapDataPktEventParms.familyType = familyType;
      if ((pMsg != L7_NULLPTR) && (msgLen > 0))
      {
       memcpy(&(dataMsg.pimsmMapDataPktEventParms.u), pMsg, msgLen);
      }
      sendBuffer = (L7_VOIDPTR)&dataMsg;
      QIndex =  PIMSM_DATA_PKT_Q;
    }
    else
    {
      /* Regulate the MGMD Events */
      if (eventType == MCAST_EVENT_MGMD_GROUP_UPDATE)
      {
        if (pimsmMapCountersValueGet (PIMSM_MAP_NUM_MGMD_EVENTS) >= PIMSMMAP_MGMD_EVENTS_COUNT_LIMIT)
        {
          PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Dropping the MGMD Event as "
                           "there is not enough Event Queue space.\n");
          pimsmMapCountersUpdate (PIMSM_MAP_NUM_MGMD_OVERFLOW_EVENTS, PIMSM_MAP_COUNTERS_INCREMENT);
                   
          if (osapiSemaTake (pimsmGblVariables_g.queueCountersSemId, L7_WAIT_FOREVER)
                          == L7_SUCCESS)
          {
            pimsmGblVariables_g.pimsmQueue[PIMSM_EVENT_Q].QSendFailedCnt++;
            osapiSemaGive (pimsmGblVariables_g.queueCountersSemId);
          }
          return L7_FAILURE;
        }
        pimsmMapCountersUpdate (PIMSM_MAP_NUM_MGMD_EVENTS, PIMSM_MAP_COUNTERS_INCREMENT);
      }

      memset(&msg, 0, sizeof(pimsmMapMsg_t));
      msg.msgId = PIMSMMAP_PROTOCOL_EVENT_MSG;
  
      msg.u.pimsmMapProtocolEventParms.event = eventType;
      msg.u.pimsmMapProtocolEventParms.familyType = familyType;
      if ((pMsg != L7_NULLPTR) && (msgLen > 0))
      {
        memcpy(&(msg.u.pimsmMapProtocolEventParms.msgData), pMsg, msgLen);
      }
      sendBuffer = (L7_VOIDPTR)&msg;      
      QIndex =  PIMSM_EVENT_Q;
    }
  }
  rc = pimsmMessageSend(QIndex,(L7_VOIDPTR)sendBuffer);

  if(rc != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Failed to send Msg to PIMSM Queue for event(%d)",eventType);

    /* Roll-back the Queue-limiting counters, if any */
    if ((eventType == MCAST_MFC_NOCACHE_EVENT)||
        (eventType == MCAST_MFC_WRONGIF_EVENT) ||
        (eventType == MCAST_MFC_ENTRY_EXPIRE_EVENT))
    {
      pimsmMapCountersUpdate (PIMSM_MAP_NUM_MFC_EVENTS, PIMSM_MAP_COUNTERS_DECREMENT);
    }
    else if (eventType == MCAST_EVENT_MGMD_GROUP_UPDATE)
    {
      pimsmMapCountersUpdate (PIMSM_MAP_NUM_MGMD_EVENTS, PIMSM_MAP_COUNTERS_DECREMENT);
    }
    else
    {
      /* Do Nothing */
    }

    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To get the mapping layer CB based on family Type
*
* @param    familyType    @b{(input)} Address Family type.
* @param    pimsmMapCbPtr @b{(inout)} Mapping Control Block.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCtrlBlockGet(L7_uchar8 familyType, 
                                     pimsmMapCB_t **pimsmMapCbPtr)
{
  switch (familyType)
  {
    case L7_AF_INET:
      *pimsmMapCbPtr = &pimsmMapCB[PIMSM_MAP_IPV4_CB];
      break;
    case L7_AF_INET6:
      if (pimsmMapV6CtrlBlockGet(pimsmMapCbPtr) != L7_SUCCESS) 
      {
        *pimsmMapCbPtr = L7_NULLPTR;
        PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Family Type %d Not Supported",familyType);
        return L7_FAILURE;
      }
      break;
    default:
      *pimsmMapCbPtr = L7_NULLPTR;
      PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Invlaid Family Type %d",familyType);
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

