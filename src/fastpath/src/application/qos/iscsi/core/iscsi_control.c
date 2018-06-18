/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename iscsi_control.c
*
* @purpose ISCSI component functions
*
* @component ISCSI
*
* @comments none
*
* @create 04/18/2008
*
* @end
*
**********************************************************************/
#include "dtlapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "sysnet_api_ipv4.h"
#include "l7_packet.h"
#include "buff_api.h"
#include "iscsi.h"
#include "iscsi_db.h"
#include "iscsi_cnfgr.h"
#include "iscsi_api.h"
#include "iscsi_sid.h"
#include "iscsi_packet.h"
#include "dtl_iscsi.h"

extern void            *iscsiQueue;
extern L7_int32         iscsiTaskId;

L7_int32                  iscsiPacketBufferPoolId;
static osapiTimerDescr_t *iscsiTimer         = L7_NULLPTR;
static L7_uint32          iscsiIdleTimeMsecs = ISCSI_IDLE_TIMEOUT;

extern void iscsiImpSessionDelete(L7_uint32 sessionId);

L7_RC_t iscsiPduReceiveProcess(L7_uint32 intIfNum, L7_uint32 pkt_size, L7_uchar8 *iscsiBuf);
void iscsiTimerAction(void);
L7_uint32 iscsiIdleTimeMsecGet(void);

/*****************************************************************************
* @purpose  Callback function to process iSCSI control frames
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
SYSNET_PDU_RC_t iscsiPktIntercept(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc)
{
  L7_uchar8         *data;
  L7_uint32         pkt_size, offset, mode;
  L7_uint32         ipHdrLen, ipPktLen, tcpHdrLen, iscsiPktLen;
  iscsiMsg_t        msg;
  L7_ipHeader_t     *ip_header;
  L7_tcpHeader_t    *tcp_header;
  L7_uint32         dstAddr;
  L7_ushort16       dstPort;
  L7_uint32         index;

  ISCSI_TRACE(ISCSI_TRACE_PDU_INTERCEPT, "iscsiPktIntercept() running\n");

  if ((iscsiAdminModeGet(&mode) != L7_SUCCESS) || (mode == L7_DISABLE))
  {
    ISCSI_TRACE(ISCSI_TRACE_PDU_INTERCEPT, "iscsiPktIntercept() - returning after iscsiAdminModeGet(): mode == %d\n", mode);
    return SYSNET_PDU_RC_IGNORED;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  offset = sysNetDataOffsetGet(data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle,pkt_size);
  ip_header = (L7_ipHeader_t *)(data + offset);

  /* see if this is an interesting packet */
  if (ip_header->iph_prot == IP_PROT_TCP)
  {
    ipHdrLen = (ip_header->iph_versLen & 0xf)*4;
    tcp_header = (L7_tcpHeader_t *)(data + offset + ipHdrLen);

    dstPort = osapiNtohs(tcp_header->destPort);
    dstAddr = osapiNtohl(ip_header->iph_dst);

    ISCSI_TRACE(ISCSI_TRACE_PDU_INTERCEPT, "iscsiPktIntercept() - TCP frame received: dstAddr = %u.%u.%u.%u, dstPort = %d\n",
                ((dstAddr & 0xFF000000) >> 24),
                ((dstAddr & 0x00FF0000) >> 16),
                ((dstAddr & 0x0000FF00) >> 8),
                (dstAddr & 0x000000FF),
                dstPort);

    /* see if this packet is to a target port/address we are listening on */
    if (iscsiTargetTableLookUp(dstPort, dstAddr, &index) == L7_SUCCESS)
    {
      /* make sure there is possibly iSCSI protocol data in the packet (e.g. not just a TCP ACK or something) */
      ipPktLen = osapiNtohs(ip_header->iph_len);
      tcpHdrLen = TCP_HEADER_LENGTH(tcp_header);
      iscsiPktLen = ipPktLen - ipHdrLen - tcpHdrLen;

      ISCSI_TRACE(ISCSI_TRACE_PDU_INTERCEPT, "iscsiPktIntercept() - iscsiTargetTableLookUp sucessful: iscsiPktLen = %d\n", iscsiPktLen);

      if (iscsiPktLen > 0)
      {
        if (bufferPoolAllocate(iscsiPacketBufferPoolId, &msg.u.iscsiPduReceiveParms.iscsiPacketBuffer) == L7_SUCCESS)
        {
          if (ISCSI_PACKET_BUFFER_SIZE > pkt_size)
          {
            msg.msgId = ISCSI_MSG_TYPE_PDU_RECEIVED;
            msg.u.iscsiPduReceiveParms.intIfNum  = pduInfo->intIfNum;

            msg.u.iscsiPduReceiveParms.dataLength = pkt_size;

            memcpy(msg.u.iscsiPduReceiveParms.iscsiPacketBuffer, data, pkt_size);

            if (osapiMessageSend(iscsiQueue,
                                 &msg,
                                 ISCSI_MSG_SIZE,
                                 L7_NO_WAIT,
                                 L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
            {
              L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ISCSI_COMPONENT_ID, "iscsiPktIntercept(): message send failed");
              ISCSI_TRACE(ISCSI_TRACE_PDU_INTERCEPT, "iscsiPktIntercept() - message send failed\n");
            }
          }
          else
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ISCSI_COMPONENT_ID, "iscsiPktIntercept(): received frame larger than ISCSI_PACKET_BUFFER_SIZE");
            ISCSI_TRACE(ISCSI_TRACE_PDU_INTERCEPT, "iscsiPktIntercept(): received frame larger than ISCSI_PACKET_BUFFER_SIZE (== %d, pkt_size == %d)\n",
                        ISCSI_PACKET_BUFFER_SIZE, pkt_size);
          }
        }
        else
        {
          ISCSI_TRACE(ISCSI_TRACE_PDU_INTERCEPT, "iscsiPktIntercept() - bufferPoolAllocate failed\n");
        }
      }
      /* since this matched a target TCP port/IP address, assume it came up because of iSCSI... consume the packet */
      SYSAPI_NET_MBUF_FREE(bufHandle);
      return SYSNET_PDU_RC_CONSUMED;
    }
  }
  /* not ours */
  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose task to handle all iSCSI events
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
void iscsiTask()
{
  iscsiMsg_t msg;
  L7_RC_t rc;

  osapiTaskInitDone(L7_ISCSI_TASK_SYNC);

  do
  {
    rc = osapiMessageReceive(iscsiQueue, (void *)&msg, ISCSI_MSG_SIZE, L7_WAIT_FOREVER);
    switch (msg.msgId)
    {
    case ISCSI_MSG_TYPE_PDU_RECEIVED:
      iscsiPduReceiveProcess(msg.u.iscsiPduReceiveParms.intIfNum,
                             msg.u.iscsiPduReceiveParms.dataLength,
                             msg.u.iscsiPduReceiveParms.iscsiPacketBuffer);
      break;
    case ISCSI_MSG_TYPE_TIMER:
      iscsiTimerAction();
      break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
              "iscsiTask(): invalid message type:%d.", msg.msgId);
      ISCSI_TRACE(ISCSI_TRACE_TASK, "iscsiTask(): invalid message ID:%d.", msg.msgId);
      break;
    }
  } while (1);

}

/*********************************************************************
* @purpose  Start iSCSI Task
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
L7_RC_t iscsiStartTask()
{
  iscsiTaskId = osapiTaskCreate("iscsiTask", iscsiTask, 0, 0,
                                iscsiSidDefaultStackSize(),
                                iscsiSidDefaultTaskPriority(),
                                iscsiSidDefaultTaskSlice());

  if (iscsiTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
           "Could not create task iscsiTask");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(L7_ISCSI_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
           "Unable to initialize iscsiTask");
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
static void iscsiTimerCallback()
{
  iscsiMsg_t     msg;

  if (!iscsiQueue)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
           "iscsiTimerCallback(): iscsiQueue has not been created!");
  }

  memset(&msg, 0, sizeof(iscsiMsg_t));
  msg.msgId = ISCSI_MSG_TYPE_TIMER;

  if (osapiMessageSend(iscsiQueue,
                       &msg,
                       sizeof(iscsiMsg_t),
                       L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
           "iSCSI: timer event send failed.");
    ISCSI_TRACE(ISCSI_TRACE_TIMER_LOW, "iSCSI: timer event send failed\n");
  }
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
L7_RC_t iscsiTimerAdd(void)
{
  osapiTimerAdd((void *)iscsiTimerCallback, L7_NULL, L7_NULL,
                iscsiIdleTimeMsecGet(), &iscsiTimer);
  if (iscsiTimer == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
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
void iscsiTimerDelete(void)
{
  /* osapiTimerFree() checks for null pointer */
  osapiTimerFree(iscsiTimer);
  iscsiTimer = L7_NULLPTR;
  return;
}

/*********************************************************************
*
* @purpose  Scans a buffer for the appearance of key text.  Returns the
*           offset in the buffer of the start of the key text if found.
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t iscsiFindKey(L7_uchar8 *iscsiDataSegment, L7_uint32 iscsiDataSegmentLength, L7_uchar8 *keyText, L7_uchar8 **foundString)
{
  L7_uchar8 *scanPtr;
  L7_uchar8 *keyPtr;
  L7_uchar8 *bufferEnd;
  L7_BOOL    found = L7_FALSE;

  scanPtr = iscsiDataSegment;
  bufferEnd = iscsiDataSegment + iscsiDataSegmentLength;

  do
  {
    while ((scanPtr < bufferEnd) && (*scanPtr == 0))
    {
      scanPtr++;
    }
    if ((keyPtr = strstr(scanPtr, keyText)) != L7_NULL)
    {
      /* found the keyText in string */
      *foundString = keyPtr;
      found = L7_TRUE;
    }
    else
    {
      /* key not found in previous string, move scanPtr to start of next string */
      while ((scanPtr < bufferEnd) && (*scanPtr != 0))
      {
        scanPtr++;
      }
      while ((scanPtr < bufferEnd) && (*scanPtr == 0))
      {
        scanPtr++;
      }
    }
  } while ((found != L7_TRUE) && (scanPtr < bufferEnd));

  if (found == L7_TRUE)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Process an incoming packet
*
* @param    intIfNum   @b((input)) NIM internal interface number
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    NONE
*
* @end
*********************************************************************/
L7_RC_t iscsiPduReceiveProcess(L7_uint32 intIfNum, L7_uint32 pkt_size, L7_uchar8 *iscsiBuf)
{
  L7_uint32         offset=0;
  L7_ipHeader_t     *ip_header;
  L7_tcpHeader_t    *tcp_header;
  L7_iscsi_basic_header_segment_t *iscsiPacket;
  L7_uint32         ipHdrLen, ipPktLen, tcpHdrLen, totalAhsLength, iscsiDataSegmentLength;
  L7_int32          iscsiPktLen;
  L7_uchar8         *iscsiDataSegment, *iscsiDataSegmentEnd, *initiatorName, *targetName;
  L7_uint32         dstAddr, srcAddr;
  L7_ushort16       dstPort, srcPort, cid;
  L7_int32          connectionId;
  L7_int32          sessionId;
  L7_uint32         targetTableIndex;
  L7_uint32         iscsiOpCode;
  L7_uchar8         isid[6];


  ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() running\n");

  if (iscsiBuf == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_FLEX_QOS_ISCSI_COMPONENT_ID, "Called with NULL iscsiBuf pointer.");
    ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() Called with NULL iscsiBuf pointer\n");
    return(L7_ERROR);
  }

  offset = sysNetDataOffsetGet(iscsiBuf);
  ip_header = (L7_ipHeader_t *)(iscsiBuf + offset);

  /* see if this is an interesting packet */
  if (ip_header->iph_prot == IP_PROT_TCP)
  {
    ipHdrLen = (ip_header->iph_versLen & 0xf)*4;
    tcp_header = (L7_tcpHeader_t *)(iscsiBuf + offset + ipHdrLen);

    /* parse protocol packet for information needed to set up iSCSI session QoS  */
    dstPort = osapiNtohs(tcp_header->destPort);
    dstAddr = osapiNtohl(ip_header->iph_dst);

    /* see if this packet is to a target port/address we are listening on */

    if (iscsiTargetTableLookUp(dstPort, dstAddr, &targetTableIndex) == L7_SUCCESS)
    {
      ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() call to iscsiTargetTableLookUp successful: targetTableIndex = %d\n", targetTableIndex);

      ipPktLen = osapiNtohs(ip_header->iph_len);
      tcpHdrLen = TCP_HEADER_LENGTH(tcp_header);
      iscsiPktLen = ipPktLen - ipHdrLen - tcpHdrLen;

      /* if frame does not have enough payload to at least hold iSCSI BHS, no need to process further */
      if (iscsiPktLen >= ISCSI_BHS_LENGTH)
      {
        srcPort = osapiNtohs(tcp_header->srcPort);
        srcAddr = osapiNtohl(ip_header->iph_src);

        iscsiPacket = (L7_iscsi_basic_header_segment_t *)((L7_char8 *)tcp_header + tcpHdrLen);

        iscsiOpCode = ISCSI_OPCODE_MASK(iscsiPacket->opcode);
        totalAhsLength = ISCSI_TOTAL_AHS_LENGTH(iscsiPacket);

        /* make sure this packet is long enough to contain the entire data segment */
        iscsiDataSegmentLength = ISCSI_DATA_LENGTH(iscsiPacket);

        ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() treating as iSCSI PDU: iscsiPktLen = %d\n", iscsiPktLen);

        /* only process if the IP frame payload is large enough to contain the entire iSCSI PDU */
        if ((iscsiDataSegmentLength + totalAhsLength + ISCSI_BHS_LENGTH) <= iscsiPktLen)
        {
          switch (iscsiOpCode)
          {
          case ISCSI_OPCODE_LOGIN_REQ:
            /* interpret and process login request */

            ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() found Login Request opcode\n");

            /* parse DataSegment for Target and Initiator Name keys and values */
            iscsiDataSegment = iscsiBuf + offset + ipHdrLen + tcpHdrLen + ISCSI_BHS_LENGTH + totalAhsLength;

            /* insert a terminating NULL to assure iscsiFindKey() does not scan past the end of the message buffer */
            if (ISCSI_PACKET_BUFFER_SIZE > pkt_size)
            {
              iscsiDataSegmentEnd = iscsiDataSegment + iscsiDataSegmentLength;
              *iscsiDataSegmentEnd = L7_EOS;

              if ((iscsiFindKey(iscsiDataSegment, iscsiDataSegmentLength, L7_ISCSI_TARGET_NAME_KEY, &targetName) == L7_SUCCESS) &&
                  (iscsiFindKey(iscsiDataSegment, iscsiDataSegmentLength, L7_ISCSI_INITIATOR_NAME_KEY, &initiatorName) == L7_SUCCESS))
              {
                /* set up connection, not taking action on return code here since an error/failure may just indicate the tables are full */

                ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() creating connection\n");

                targetName += L7_ISCSI_TARGET_NAME_KEY_LENGTH;
                initiatorName += L7_ISCSI_INITIATOR_NAME_KEY_LENGTH;
                cid = osapiNtohs(iscsiPacket->cid);
                memcpy(isid, &(iscsiPacket->isid), sizeof(isid));

                if (iscsiConnectionCreate(targetName, initiatorName, isid, dstAddr, dstPort, srcAddr, srcPort, cid, targetTableIndex) != L7_SUCCESS)
                {
                  ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() failed creating connection\n");
                }
              }
            }
            else
            {
              ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() insufficient space in message buffer to insert terminating NULL\n");
            }
            break;
          case ISCSI_OPCODE_LOGOUT_REQ:
            /* interpret and process logout request */

            ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() found Logout Request opcode\n");

            /* per RFC 3720 section 10.14.2 for Logout Requests, both totalAhsLength and iscsiDataSegmentLength must be 0*/
            if ((totalAhsLength != 0) || (iscsiDataSegmentLength != 0))
            {
              ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() opcode byte matches Logout Request but either TotalAHSLength or DataSegmentLength not 0\n");
            }
            else if (iscsiPktLen != ISCSI_BHS_LENGTH && iscsiPktLen != ISCSI_BHS_WITH_HDR_DIGEST_LENGTH)
            {
              ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() iscsiPktLen not as expected for Logout Request (%d)\n", iscsiPktLen);
            }
            else
            {
            if ((connectionId = iscsiConnectionLookup(dstAddr, dstPort, srcAddr, srcPort)) >= 0)
            {
              if (ISCSI_LOGOUT_REQ_REASON_CODE_MASK(iscsiPacket->opcodeSpecFields[0]) == L7_ISCSI_LOGOUT_REQ_REASON_CLOSE_SESSION)
              {
                /* close the entire session */
                if ((sessionId = iscsiDbConnectionSessionIdGet(connectionId)) >= 0)
                {
                  ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() closing session for sessionId = %d, connectionId = %d\n", sessionId, connectionId);
                  iscsiSessionDelete(sessionId);
                }
              }
              else if ((ISCSI_LOGOUT_REQ_REASON_CODE_MASK(iscsiPacket->opcodeSpecFields[0]) == L7_ISCSI_LOGOUT_REQ_REASON_CLOSE_CONNECTION) ||
                       (ISCSI_LOGOUT_REQ_REASON_CODE_MASK(iscsiPacket->opcodeSpecFields[0]) == L7_ISCSI_LOGOUT_REQ_REASON_CLOSE_CONNECTION_FOR_RECOVERY))
              {
                /* close the connection only */
                ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() closing connection for connectionId = %d\n", connectionId);
                iscsiConnectionDelete(connectionId);
              }
              else
              {
                ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() unrecognized Logout Req reason code: %x\n", iscsiPacket->opcodeSpecFields[0]);
              }
            }
            else
            {
              ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() failed to find connection matching Logout Request\n");
            }
            }
            break;
          default:
            ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() did not find interesting opcode\n");
            break;
          }
        }
        else
        {
          ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() - packet not large enough to contain iSCSI PDU:",
                      "\n  iscsiDataSegmentLength = %d",
                      "\n  totalAhsLength = %d",
                      "\n  ISCSI_BHS_LENGTH = %d",
                      "\n  iscsiPktLen = %d\n", 
                      iscsiDataSegmentLength, totalAhsLength, ISCSI_BHS_LENGTH, iscsiPktLen);
        }
      }
      else
      {
        ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() packet too short to be iSCSI PDU : iscsiPktLen = %d\n", iscsiPktLen);
      }
    }
    else
    {
      ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() called with PDU containing data not found in target port table\n");
    }
  }
  else
  {
    ISCSI_TRACE(ISCSI_TRACE_PDU_RX, "iscsiPduReceiveProcess() called with PDU not identified as a TCP packet\n");
  }

  bufferPoolFree(iscsiPacketBufferPoolId, iscsiBuf);
  return L7_SUCCESS;
}

void iscsiTimerAction(void)
{
  L7_int32 sessionId = -1;
  L7_uint32 silentTime, silentTimeConfig;
  L7_ulong64 countData, currentCount;

  memset(&countData, 0, sizeof(countData));
  memset(&currentCount, 0, sizeof(currentCount));

  /* retrieve the configured silent time */
  (void)iscsiTimeOutIntervalGet(&silentTimeConfig);

  iscsiSemaTake();

  ISCSI_TRACE(ISCSI_TRACE_TIMER_LOW, "iscsiTimerAction() running\n");

  /* for each session */
  while (iscsiDbSessionIterate(sessionId, &sessionId) == L7_SUCCESS)
  {
    /* update counter data and adjust silent time values */
    if (dtlIscsiConnectionCounterGet(sessionId, &countData) == L7_SUCCESS)
    {
      if (iscsiDbSessionCounterGet(sessionId, &currentCount) == L7_SUCCESS)
      {
        if ((countData.low != currentCount.low) || (countData.high != currentCount.high))
        {
          iscsiDbSessionCounterSet(sessionId, &countData);
          (void)iscsiDbSessionActivityTimeUpdate(sessionId);
          ISCSI_TRACE(ISCSI_TRACE_TIMER_HIGH, "iscsiTimerAction() resetting silent time data for sessionId = %d\n", sessionId);
        }
      }
    }
    else
    {
      ISCSI_TRACE(ISCSI_TRACE_TIMER_HIGH, "iscsiTimerAction() error returned from dtlIscsiConnectionCounterGet() for sessionId = %d\n", sessionId);
    }
    /* examine the silent time value to determine if expired */
    if ((iscsiDbSessionSilentTimeGet(sessionId, &silentTime) == L7_SUCCESS) &&
        (silentTime > silentTimeConfig))
    {
      iscsiImpSessionDelete(sessionId);
      ISCSI_TRACE(ISCSI_TRACE_TIMER_HIGH, "iscsiTimerAction() timing out sessionId = %d\n", sessionId);
    }
  }

  /* restart the timer */
  (void)iscsiTimerAdd();

  iscsiSemaGive();
  ISCSI_TRACE(ISCSI_TRACE_TIMER_LOW, "iscsiTimerAction() returning\n");
  return;
}

L7_uint32 iscsiIdleTimeMsecGet(void)
{
  return iscsiIdleTimeMsecs;
}

void iscsiIdleTimeSecondsSet(L7_uint32 seconds)
{
  iscsiIdleTimeMsecs = seconds * 1000;
  return;
}
