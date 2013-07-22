/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_cfg.c
*
* @purpose VOIP configuration functions
*
* @component VOIP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "nvstoreapi.h"
#include "nimapi.h"
#include "sysapi.h"
#include "l7_product.h"
#include "registry.h"
#include "log.h"
#include "defaultconfig.h"
#include "voip.h"
#include "voip_cnfgr.h"
#include "platform_config.h"
#include "voip_util.h"
#include "voip_sid.h"
#include "voip_sip.h"
#include "voip_parse.h"
#include "osapi_support.h"
#include "voip_control.h"
#include "voip_exports.h"
#include "dtl_voip.h"

void                   *voipQueue        = L7_NULLPTR;
L7_int32                voipTaskId;

#define VOIP_TIMER_INTERVAL   1000*60*3  /* wake up every 3 minutes (idle timeout) when there are pending requests */
#define VOIP_CALL_IDLE_TIMEOUT  VOIP_TIMER_INTERVAL
/* use one timer that wakes up every second, maintain a
   a reference count and delete the timer when we have no pending sessions */
static osapiTimerDescr_t *voipTimer       = L7_NULLPTR;
static L7_uint32          voipTimerCount  = 0;  /* reference count for voipTimer */

void *voipDbSema = L7_NULL;

/*********************************************************************
* @purpose  Take the semaphore for the VOIP DB
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void voipDbSemaTake()
{
  osapiSemaTake(voipDbSema, L7_WAIT_FOREVER);
}

/*********************************************************************
* @purpose  Give the semaphore for the VOIP DB
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void voipDbSemaGive()
{
  osapiSemaGive(voipDbSema);
}

/*****************************************************************************
* @purpose  Callback function to Copy and process VOIP control frames
*
* @param    hookId        @b{(input)} The hook location
* @param    bufHandle     @b{(input)} Handle to the frame to be processed
* @param    pduInfo       @b{(input)} Pointer to info about this frame
* @param    continueFunc  @b{(input)} Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  If frame has been consumed;
* @returns  SYSNET_PDU_RC_IGNORED  If frame has been ignored
*                                  continue processing it
*
* @notes    none
*
* @end
****************************************************************************/

SYSNET_PDU_RC_t voipPktIntercept(L7_uint32 hookId,
                                 L7_netBufHandle bufHandle,
                                 sysnet_pdu_info_t *pduInfo,
                                 L7_FUNCPTR_t continueFunc)

{
  L7_uint32         protocol =0;
  L7_uchar8         *data;
  L7_uint32         pkt_size,offset;
  voipMgmtMsg_t     msg;
  L7_ipHeader_t     *ip_header;
  L7_udp_header_t   *udp_header;
  L7_uint32          srcPort,dstPort;

  msg.msgId = voipMsgPduReceive;
  msg.u.voipPduReceiveParms.intIfNum  = pduInfo->intIfNum;


  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  offset = sysNetDataOffsetGet(data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle,pkt_size);
  ip_header = (L7_ipHeader_t *)(data + offset);

  if ((ip_header->iph_prot == IP_PROT_UDP)||(ip_header->iph_prot == IP_PROT_TCP))
  {
    /*  UDP and TCP headers are the same wrt source and dest ports
          */

    udp_header = (L7_udp_header_t *)(data + offset + L7_IP_HDR_LEN);

    dstPort= osapiNtohs(udp_header->destPort);
    srcPort = osapiNtohs(udp_header->sourcePort);

    if ((srcPort == L7_SIP_L4_PORT) || (dstPort == L7_SIP_L4_PORT))
        protocol = L7_QOS_VOIP_PROTOCOL_SIP;
    else if ((srcPort == L7_MGCP_L4_PORT) || (dstPort == L7_MGCP_L4_PORT))
        protocol = L7_QOS_VOIP_PROTOCOL_MGCP;
    else if ((srcPort == L7_SCCP_L4_PORT) || (dstPort == L7_SCCP_L4_PORT))
        protocol = L7_QOS_VOIP_PROTOCOL_SCCP;
    else if ((srcPort == L7_H323_L4_PORT) || (dstPort == L7_H323_L4_PORT))
       protocol = L7_QOS_VOIP_PROTOCOL_H323;

    if (protocol == 0)
      return SYSNET_PDU_RC_IGNORED;

    msg.u.voipPduReceiveParms.voipBuf = (L7_uchar8 *)osapiMalloc (L7_FLEX_QOS_VOIP_COMPONENT_ID,pkt_size);
    msg.u.voipPduReceiveParms.protocol  = protocol;
    msg.u.voipPduReceiveParms.dataLength = pkt_size;

    if (msg.u.voipPduReceiveParms.voipBuf == L7_NULLPTR)
    {
        return SYSNET_PDU_RC_IGNORED;
    }
    else
    {
     memcpy(msg.u.voipPduReceiveParms.voipBuf, data,pkt_size);

     if (osapiMessageSend(voipQueue,
                          &msg,
                          VOIP_MSG_SIZE,
                          L7_NO_WAIT,
                          L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                "VoipPktIntercept(): message send failed\n");
        return SYSNET_PDU_RC_IGNORED;
     }

      return SYSNET_PDU_RC_COPIED;
   }
 }
  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  Start VOIP TASk
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipStartTask()
{
  voipTaskId = osapiTaskCreate("voipTask", voipTask, 0, 0,
                           voipSidDefaultStackSize(),
                           voipSidDefaultTaskPriority(),
                           voipSidDefaultTaskSlice());

  if (voipTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "Could not create task voipTask\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit (L7_VOIP_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "Unable to initialize voipTask()\n");
    return(L7_FAILURE);
  }
  return(L7_SUCCESS);
}

/*********************************************************************
*
* @purpose callback to handle the request timer events
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
static void voipTimerCallback()
{
  voipMgmtMsg_t     msg;

  memset(&msg, 0, sizeof(voipMgmtMsg_t));
  if (!voipQueue)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "voipTimerCallback(): voipQueue has not been created!\n");
  }

  msg.msgId = voipMsgTimer;
  if (osapiMessageSend(voipQueue,
                       &msg,
                       sizeof(voipMgmtMsg_t),
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "VOIP: timer event send failed.\n");
  }
  osapiTimerAdd((void *)voipTimerCallback, L7_NULL, L7_NULL,
                VOIP_CALL_IDLE_TIMEOUT, &voipTimer);
}

/*********************************************************************
*
* @purpose  Maintain one timer resource for the component
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void voipTimerAdd(void)
{

  if (voipTimerCount == 0)
  {
    osapiTimerAdd((void *)voipTimerCallback, L7_NULL, L7_NULL,
                  VOIP_CALL_IDLE_TIMEOUT, &voipTimer);
  }
  voipTimerCount++;

  return;
}

/*********************************************************************
*
* @purpose  Maintain one timer resource for the component
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void voipTimerDelete(void)
{

  voipTimerCount--;
  if (voipTimerCount == 0)
  {
    osapiTimerFree(voipTimer);
    voipTimer = L7_NULLPTR;
  }
  return;
}
/*********************************************************************
*
* @purpose Process timer events, handle connection timeouts.
*
* @param   none
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
static void voipTimerAction(void)
{
  voipSipTimerAction();
  voipSccpTimerAction();
  voipH323TimerAction();
  return;
}


/*********************************************************************
* @purpose task to handle all Port VOIP management messages
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
void voipTask()
{
  voipMgmtMsg_t msg;
  L7_RC_t rc;

  osapiTaskInitDone(L7_VOIP_TASK_SYNC);

  do
  {
    rc = osapiMessageReceive(voipQueue, (void *)&msg, VOIP_MSG_SIZE, L7_WAIT_FOREVER);
    voipDbSemaTake();
    switch(msg.msgId)
    {
      case voipMsgIntfChange:
           voipIntfChangeProcess(msg.u.voipIntfChangeParms.intIfNum, msg.u.voipIntfChangeParms.event,
                                 msg.u.voipIntfChangeParms.correlator);
           break;
      case voipMsgStartupNotify:
           voipStartupNotifyProcess(msg.u.voipStartupNotifyParms.startupPhase);
           break;
      case voipMsgPduReceive:
           voipPduReceiveProcess(msg.u.voipPduReceiveParms.intIfNum,
                                 msg.u.voipPduReceiveParms.voipBuf,
                                 msg.u.voipPduReceiveParms.protocol,
                                 msg.u.voipPduReceiveParms.dataLength);
           break;
      case voipMsgTimer:
           voipTimerAction();
           break;
      default:
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                   "voipTask(): invalid message type:%d. %s:%d\n",
           msg.msgId, __FILE__, __LINE__);
        break;
    }
    voipDbSemaGive();
  } while (1);

}
