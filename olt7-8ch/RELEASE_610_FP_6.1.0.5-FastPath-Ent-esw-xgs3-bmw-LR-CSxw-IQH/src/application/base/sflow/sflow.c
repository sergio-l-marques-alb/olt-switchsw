/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow.c
*
* @purpose   SFlow sample processing and datagram packing functions
*
* @component sflow
*
* @comments  none
*
* @create 26-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#include <string.h>
#include "commdefs.h"
#include "sysnet_api.h"
#include "buff_api.h"
#include "ipv6_commdefs.h"
#include "osapi_support.h"
#include "usmdb_ip_base_api.h"

#include "sflow_mib.h"
#include "sflow_ctrl.h"
#include "sflow_db.h"
#include "sflow_debug.h"
#include "sflow_outcalls.h"

extern SFLOW_agent_t agent;
static SFLOW_sample_data_t  sample;
static void sFlowReceiverFlowSampleWrite(SFLOW_receiver_t *pReceiver,
                                         SFLOW_flow_sample_t *pFlowSample);
static void sFlowReceiverCounterSampleWrite(SFLOW_receiver_t *pReceiver,
                                            SFLOW_counters_sample_t *pCounterSample);
static void sFlowReceiverSampleDatagramHeaderWrite(SFLOW_receiver_t *pReceiver);
/*********************************************************************
* @purpose  Receives sampled packet (1 in Nth packet)
*
* @param    netBufHandle    @b{(input)} Handle to buffer
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
*
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
* @notes    When in congestion, the drop count increases. If we see
*           SFLOW_CHOKE_THRESHOLD drops in SFLOW_CONGESTION_ALERT_TIMEOUT
*           seconds, we send a choke message to sFlow task to double the
*           sampling rate of the latest sampled data source
*
* @end
*********************************************************************/
L7_RC_t sFlowPduReceive(L7_netBufHandle netBufHandle,
                        sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8        *data;
  L7_uint32         dsIndex, instance = FD_SFLOW_INSTANCE, dataLength;
  SFLOW_sampler_t  *pSampler = L7_NULLPTR;
  sflowPDU_Msg_t    msg;
  sFlowEventMsg_t   chokeMsg;
  static L7_uint32  congestionCount = 0;
  L7_BOOL           dropFlag = L7_FALSE;

  SFLOW_TRACE(SFLOW_DEBUG_SAMPLER,"Sample received on intIfNum %u", pduInfo->intIfNum);
  dsIndex = SFLOW_VAL_TO_DS_INDEX(pduInfo->intIfNum,L7_SFLOW_DS_TYPE_IFINDEX);
  osapiSemaTake(agent.samplerAvlTree.semId, L7_WAIT_FOREVER);
  /* get the sampler object responsible to handle the sampled pkt*/
  if ((pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT))
                                         != L7_NULLPTR)
  {
    pSampler->sample_pool++;
    /* obtain a buffer from the bufferpool */
    msg.msgId      = SFLOW_SAMPLE_RX;
    msg.dsIndex    = dsIndex;
    msg.instance   = FD_SFLOW_INSTANCE;
    msg.direction  = ((SYSAPI_NET_MBUF_HEADER_t *)netBufHandle)->rxCode;
    /* Get start and length of incoming frame */
    SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
    SYSAPI_NET_MBUF_GET_DATALENGTH(netBufHandle, dataLength);


    msg.dataLength = dataLength;
    if (bufferPoolAllocate(agent.sflowBufferPool, &(msg.sflowSampleBuffer)) 
                           == L7_SUCCESS)
    {
       /* Copy the sampled pkt header into the buffer */
       if (dataLength > SFLOW_MAX_HEADER_SIZE)
       {
         memcpy(msg.sflowSampleBuffer, data, SFLOW_MAX_HEADER_SIZE);
       }
       else
       {
         memcpy(msg.sflowSampleBuffer, data, dataLength);
       }

       if (osapiMessageSend(agent.sFlowPacketQueue,
                            &msg, SFLOW_PDU_MSG_SIZE, L7_NO_WAIT,
                            L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
       {
         /* Free the allocated buffer */
         bufferPoolFree(agent.sflowBufferPool, msg.sflowSampleBuffer);
         dropFlag = L7_TRUE;
       }
       else
       {
         if (osapiSemaGive(agent.sFlowMsgQSema) != L7_SUCCESS)
         {
           LOG_MSG("sFlowPduReceive: Failed to give msgQueue semaphore\n");
         }
         if (congestionCount)
         {
           congestionCount--;
         }
       }
    } /* check if a buffer is available */
    else
    { 
       dropFlag = L7_TRUE;
    }

    /* Re-init congestion count as the congestionAlert timer is on */
    if (agent.congestionAlert != 0)
    {
      congestionCount = 0;
    }

    if (dropFlag == L7_TRUE)
    {
      congestionCount++;
      pSampler->drops++;
      /* Check if congestion is already detected */
      if (agent.congestionAlert == 0 && 
          congestionCount > SFLOW_CHOKE_THRESHOLD)
      {
        congestionCount = 0;
        /* throttle the sampling rate */
        /* send choke signal only if congestion alert is not flagged*/
        chokeMsg.msgId = SFLOW_CHOKE;
        chokeMsg.dataSource  = msg.dsIndex;
        chokeMsg.sflowInstance   = FD_SFLOW_INSTANCE;
        if (osapiMessageSend(agent.sFlowEventQueue, &chokeMsg,SFLOW_MSG_SIZE, L7_NO_WAIT,
                             L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
        {
          if (osapiSemaGive(agent.sFlowMsgQSema) != L7_SUCCESS)
          {
            LOG_MSG("sFlowPduReceive: Failed to give msgQueue semaphore\n");
          }
        }
      }
    }
  } /* check if a sampler exists */
  else
  {
    SFLOW_TRACE(SFLOW_DEBUG_SAMPLER,"Sampler not found for dataSource %u", dsIndex);
  }
  osapiSemaGive(agent.samplerAvlTree.semId);

  /* Always return success indicating frame is
     consumed by sFlow. Free the mbuf handle */
  SYSAPI_NET_MBUF_FREE (netBufHandle);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Fill counter record sample with interface counters
*
* @param   pPoller       @b{(input)}  Pointer to poller object
* @param   if_counters   @b{(input)}  if_counter object to be filled
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void sFlowIfCountersGet(SFLOW_poller_t *pPoller,
                        SFLOW_if_counters_t *if_counters)
{
  L7_uint32  uintVal, intIfNum;
  L7_uint64  uint64Val = 0;

  intIfNum = SFLOW_DS_INDEX_TO_VAL(pPoller->key.dsIndex);
  
  sFlowCounterValGet(intIfNum, SFLOW_IF_TYPE, &if_counters->ifType, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_SPEED, &uintVal, &if_counters->ifSpeed);
  sFlowCounterValGet(intIfNum, SFLOW_IF_DIRECTION, &if_counters->ifDirection, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_STATUS, &if_counters->ifStatus, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_IN_OCTETS, &uintVal, &if_counters->ifInOctets);
  sFlowCounterValGet(intIfNum, SFLOW_IF_IN_UCAST_PKTS, &if_counters->ifInUcastPkts, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_IN_MCAST_PKTS, &if_counters->ifInMulticastPkts, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_IN_BCAST_PKTS, &if_counters->ifInBroadcastPkts, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_IN_DISCARDS, &if_counters->ifInDiscards, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_IN_ERRORS, &if_counters->ifInErrors, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_IN_UNKNWN_PROTOS, &if_counters->ifInUnknownProtos, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_OUT_OCTETS, &uintVal, &if_counters->ifOutOctets);
  sFlowCounterValGet(intIfNum, SFLOW_IF_OUT_UCAST_PKTS, &if_counters->ifOutUcastPkts, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_OUT_MCAST_PKTS, &if_counters->ifOutMulticastPkts, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_OUT_BCAST_PKTS, &if_counters->ifOutBroadcastPkts, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_OUT_DISCARDS, &if_counters->ifOutDiscards, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_OUT_ERRORS, &if_counters->ifOutErrors, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_IF_PROMISCOUS_MODE, &if_counters->ifPromiscuousMode, &uint64Val);
}
/*********************************************************************
* @purpose Fill counter record sample with ethernet counters
*
* @param   pPoller       @b{(input)}  Pointer to poller object
* @param   eth_counters  @b{(input)}  eth_counter object to be filled
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void sFlowEthCountersGet(SFLOW_poller_t *pPoller,
                         SFLOW_ethernet_counters_t *eth_counters)
{
  L7_uint32  intIfNum;
  L7_uint64  uint64Val = 0;

  intIfNum = SFLOW_DS_INDEX_TO_VAL(pPoller->key.dsIndex);

  sFlowCounterValGet(intIfNum, SFLOW_IF_PROMISCOUS_MODE, 
                     &eth_counters->dot3StatsAlignmentErrors, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_FCS_ERRORS, 
                     &eth_counters->dot3StatsFCSErrors, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_SINGLE_COLLISION_FRAMES, 
                     &eth_counters->dot3StatsSingleCollisionFrames, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_MULTIPLE_COLLISION_FRAMES, 
                     &eth_counters->dot3StatsMultipleCollisionFrames, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_SQE_TEST_ERRORS, 
                     &eth_counters->dot3StatsSQETestErrors, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_DEFERRED_TX, 
                     &eth_counters->dot3StatsDeferredTransmissions, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_LATE_COLLISIONS, 
                     &eth_counters->dot3StatsLateCollisions, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_EXCESSIVE_COLLISIONS, 
                     &eth_counters->dot3StatsExcessiveCollisions, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_INTERNAL_MAC_TX_ERRORS, 
                     &eth_counters->dot3StatsInternalMacTransmitErrors, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_CARIER_SENSE_ERRORS, 
                     &eth_counters->dot3StatsCarrierSenseErrors, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_FRAME_TOO_LONGS, 
                     &eth_counters->dot3StatsFrameTooLongs, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_INTERNAL_MAC_RX_ERRORS, 
                     &eth_counters->dot3StatsInternalMacReceiveErrors, &uint64Val);
  sFlowCounterValGet(intIfNum, SFLOW_ETH_SYMBOL_ERRORS, 
                     &eth_counters->dot3StatsSymbolErrors, &uint64Val);
  return;
}
/*********************************************************************
* @purpose Process incoming counter sample
*
* @param   pPoller       @b{(input)}  Pointer to poller object
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void sFlowCounterProcess(SFLOW_poller_t *pPoller)
{
#define SFLOW_IF_COUNTER_RECORD  0
#define SFLOW_ETH_COUNTER_RECORD 1
  L7_uint32                       dataLength = 0, idx, ifIdx;
  SFLOW_counters_sample_t        *counter_sample;
  SFLOW_counters_sample_record_t *counter_sample_record;
  SFLOW_if_counters_t            *if_counters;
  SFLOW_ethernet_counters_t      *eth_counters;
  
  if (pPoller->pollerCfg->sFlowCpReceiver == L7_NULL)
  {
    /* No valid receiver object is registered with this poller object */
    return;
  }

  if (nimGetIntfIfIndex(SFLOW_DS_INDEX_TO_VAL(pPoller->key.dsIndex), &ifIdx)
                        != L7_SUCCESS)
  {
    ifIdx = pPoller->key.dsIndex;
  }

  memset(&sample, 0x00, sizeof(SFLOW_sample_data_t));
  counter_sample = &(sample.sample_data.counter_sample);
  counter_sample->sequence_number = (pPoller->counterSampleSeqNo)++;
  counter_sample->source_id       = SFLOW_VAL_TO_DS_INDEX(ifIdx,L7_SFLOW_DS_TYPE_IFINDEX);
  counter_sample->num_records     = SFLOW_COUNTER_RECORDS_PER_SAMPLE;

  dataLength += 3 * 4;

  for (idx = 0; idx < SFLOW_COUNTER_RECORDS_PER_SAMPLE; idx++)
  {
    counter_sample_record = &counter_sample->counter_records[idx];
    if (idx == SFLOW_IF_COUNTER_RECORD)
    {
      counter_sample_record->tag    = SFLOW_COUNTERS_GENERIC;
      counter_sample_record->length = SFLOW_IF_COUNTERS_LEN;
      dataLength += 2 * 4;
      dataLength += SFLOW_IF_COUNTERS_LEN;
      if_counters = &counter_sample_record->counter_data.generic;
      if_counters->ifIndex = SFLOW_DS_INDEX_TO_VAL(pPoller->key.dsIndex);
      sFlowIfCountersGet(pPoller, if_counters);
    }
    else if (idx == SFLOW_ETH_COUNTER_RECORD)
    {
      counter_sample_record->tag    = SFLOW_COUNTERS_ETHERNET;
      counter_sample_record->length = SFLOW_ETH_COUNTERS_LEN;
      dataLength += 2 * 4;
      dataLength += SFLOW_ETH_COUNTERS_LEN;
      eth_counters = &counter_sample_record->counter_data.ethernet;
      sFlowEthCountersGet(pPoller, eth_counters);
    }
  }/* fill up the supported counter records */

  sample.data_format = SFLOW_COUNTERS_SAMPLE;
  sample.length      =  dataLength;
  sFlowReceiverSampleAdd(&agent.receiver[pPoller->pollerCfg->sFlowCpReceiver - 1], &sample);
}
/*********************************************************************
* @purpose Process incoming flow samples
*
* @param   msg       @b{(input)}  Pointer to received sflow Pdu
*                                 message
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void sFlowSampleProcess(sflowPDU_Msg_t *pduMsg)
{
  SFLOW_sampled_header_t     *sampledHeader;
  SFLOW_flow_sample_record_t *flow_sample_record;
  SFLOW_flow_sample_t        *flow_sample;
  SFLOW_sampler_t            *pSampler = L7_NULLPTR;
  L7_uint32                   ifIdx;
 

  memset(&sample, 0x00, sizeof(SFLOW_sample_data_t));
  flow_sample   = &(sample.sample_data.flow_sample);
  sampledHeader = &flow_sample->flow_records[0].flow_data.header;
  sampledHeader->header_protocol = SFLOW_HDR_ETHERNET_ISO8023;
  /* FCS is subtracted from the dataLength in driver */
  sampledHeader->frame_length    = pduMsg->dataLength + 
                                   SFLOW_SAMPLE_FRAME_FCS_LEN;
  sampledHeader->stripped        = SFLOW_SAMPLE_FRAME_FCS_LEN; 
  
  /* get the corresponding sampler object */
  if ((pSampler =  sFlowSamplerInstanceGet(pduMsg->dsIndex, pduMsg->instance, 
                                           L7_MATCH_EXACT)) == L7_NULLPTR)
  {
   /* an unwanted sample has come. This might happen momentarily when sampling
      is disabled when there are samples pending for processing in the 
      queue */
    bufferPoolFree(agent.sflowBufferPool, pduMsg->sflowSampleBuffer);
    return;
  }

  if (nimGetIntfIfIndex(SFLOW_DS_INDEX_TO_VAL(pSampler->key.dsIndex), &ifIdx)
                        != L7_SUCCESS)
  {
    ifIdx = pSampler->key.dsIndex;
  }

  if (pSampler->samplerCfg->sFlowFsReceiver == L7_NULL)
  {
    /* No valid receiver object is registered with this sampler object */
    bufferPoolFree(agent.sflowBufferPool, pduMsg->sflowSampleBuffer);
    return;
  }

  if (pSampler->samplerCfg->sFlowFsMaximumHeaderSize > pduMsg->dataLength)
  {
    sampledHeader->header_length = pduMsg->dataLength;
  }
  else
  {
    sampledHeader->header_length = pSampler->samplerCfg->sFlowFsMaximumHeaderSize;
  }

  sample.data_format = SFLOW_FLOW_SAMPLE;
  sample.length      = SFLOW_FLOW_HEADER_SAMPLE_OVERHEAD + 
                       sampledHeader->header_length;

  /* Make a local copy of the sample and release the buffer pool */
  memcpy(sampledHeader->header_bytes, pduMsg->sflowSampleBuffer , 
         sampledHeader->header_length);
  bufferPoolFree(agent.sflowBufferPool, pduMsg->sflowSampleBuffer);

  flow_sample_record = &flow_sample->flow_records[0];
 /* Build flow sample records. Only one record per flow sample is supported */
  flow_sample_record->tag    = SFLOW_FLOW_HEADER;
  flow_sample_record->length = SFLOW_FLOW_RECORD_SAMPLED_HEADER_OVERHEAD + sampledHeader->header_length;

  /* Build flow sample */
  flow_sample->sequence_number = (pSampler->flowSampleSeqNo)++;
  flow_sample->source_id  = SFLOW_VAL_TO_DS_INDEX(ifIdx,L7_SFLOW_DS_TYPE_IFINDEX);
  flow_sample->sampling_rate   = pSampler->samplerCfg->sFlowFsPacketSamplingRate;
  flow_sample->sample_pool     = pSampler->sample_pool;
  flow_sample->drops           = pSampler->drops;
  flow_sample->input           = (pduMsg->direction & L7_MBUF_RX_SAMPLE_SOURCE) 
                                 ? ifIdx : L7_NULL;
  flow_sample->output          = (pduMsg->direction & L7_MBUF_RX_SAMPLE_DEST)
                                 ? ifIdx : L7_NULL;
  flow_sample->num_records     = SFLOW_FLOW_RECORDS_PER_SAMPLE;
  sFlowReceiverSampleAdd(&agent.receiver[pSampler->samplerCfg->sFlowFsReceiver - 1], &sample);
}
/*********************************************************************
* @purpose Add a flow or counter sample to receiver buffer
*
* @param   pReceiver     @b{(input)}  Pointer to receiver 
* @param   pSample       @b{(input)}  Pointer to constructed
*                                      sample object
*                                 
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t sFlowReceiverSampleAdd(SFLOW_receiver_t *pReceiver,
                               SFLOW_sample_data_t *pSample)
{
  L7_uint32         sampleLength;
  
  sampleLength =  pSample->length + 8; /* sizeof(data_format) + sizeof(length) */

  if (sampleLength + pReceiver->pduLen + SFLOW_SAMPLE_DATAGRAM_IP6_LEN >  
      pReceiver->rcvrCfg->sFlowRcvrMaxDatagramSize)
  {
    SFLOW_TRACE(SFLOW_DEBUG_SAMPLER,"No place in rcvr %u buffer pduLen %u",
                 pReceiver->rcvrCfg->sFlowRcvrIndex, pReceiver->pduLen);
    if (sFlowReceiverDatagramSend(pReceiver) != L7_SUCCESS)
    {
      LOG_MSG("sFlowReceiverFlowSampleAdd: Failed to send out datagram");
    }
  }

  if (pReceiver->pduLen + sampleLength > pReceiver->rcvrCfg->sFlowRcvrMaxDatagramSize)
  {
    LOG_MSG("sFlowReceiverFlowSampleAdd: sample length is overshooting max datagram size");
    return L7_FAILURE;
  }

  pReceiver->pduLen += sampleLength;
  SFLOW_PUT_32(pSample->data_format, pReceiver->ptrPduBuff);/* Data_format */
  SFLOW_PUT_32(pSample->length, pReceiver->ptrPduBuff);                       /* sample length */
 
  /* Add the sample to receiver buff */
  switch (pSample->data_format)
  {
    case SFLOW_FLOW_SAMPLE:
         sFlowReceiverFlowSampleWrite(pReceiver, &pSample->sample_data.flow_sample);
         break;
    case SFLOW_COUNTERS_SAMPLE:
         sFlowReceiverCounterSampleWrite(pReceiver, &pSample->sample_data.counter_sample);
         break;
    default:
         LOG_MSG("sFlowReceiverSampleAdd: data_format %u not supported ", pSample->data_format);
         return L7_FAILURE;
         break;
  }
  pReceiver->sFlowDatagram.num_records++;

  /* Check if the buffer is enough full to send it to collector.  
     8 = sizeof(data_format) + sizeof(length)*/
  if (pReceiver->pduLen + SFLOW_SAMPLE_DATAGRAM_IP6_LEN < SFLOW_FLOW_HEADER_SAMPLE_OVERHEAD + 8)
  {
    if (sFlowReceiverDatagramSend(pReceiver) != L7_SUCCESS)
    {
      LOG_MSG("sFlowReceiverFlowSampleAdd: Failed to send out datagram");
      return L7_FAILURE;
    }
  }
  else
  { 
    SFLOW_TRACE(SFLOW_DEBUG_SAMPLER,"Delaying sFlow dgram send as rcvr %u buffer not full pduLen %u",
                 pReceiver->rcvrCfg->sFlowRcvrIndex, pReceiver->pduLen);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Write flow sample to receiver buffer
*
* @param   pSampler       @b{(input)}  Pointer to source sampler
*                                      object
* @param   pFlowSample    @b{(input)}  Pointer to flow sample to be
*                                      written in the receiver buff
*
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
static void sFlowReceiverFlowSampleWrite(SFLOW_receiver_t *pReceiver,
                                         SFLOW_flow_sample_t *pFlowSample)
{
   L7_uint32  idx;

   SFLOW_flow_sample_record_t *pFlowSampleRecord;

   SFLOW_PUT_32(pFlowSample->sequence_number, pReceiver->ptrPduBuff);
   SFLOW_PUT_32(pFlowSample->source_id, pReceiver->ptrPduBuff);
   SFLOW_PUT_32(pFlowSample->sampling_rate, pReceiver->ptrPduBuff);
   SFLOW_PUT_32(pFlowSample->sample_pool, pReceiver->ptrPduBuff);
   SFLOW_PUT_32(pFlowSample->drops, pReceiver->ptrPduBuff);
   SFLOW_PUT_32(pFlowSample->input, pReceiver->ptrPduBuff);
   SFLOW_PUT_32(pFlowSample->output, pReceiver->ptrPduBuff);
   SFLOW_PUT_32(pFlowSample->num_records, pReceiver->ptrPduBuff);

   for (idx = 0 ; idx < pFlowSample->num_records; idx++)
   {
     pFlowSampleRecord = &pFlowSample->flow_records[idx];
     SFLOW_PUT_32(pFlowSampleRecord->tag, pReceiver->ptrPduBuff);
     SFLOW_PUT_32(pFlowSampleRecord->length, pReceiver->ptrPduBuff);
     switch(pFlowSampleRecord->tag)
     {
        case SFLOW_FLOW_HEADER:
        {
           SFLOW_sampled_header_t *pFlowData = &pFlowSampleRecord->flow_data.header;

           SFLOW_PUT_32(pFlowData->header_protocol, pReceiver->ptrPduBuff);
           SFLOW_PUT_32(pFlowData->frame_length, pReceiver->ptrPduBuff);
           SFLOW_PUT_32(pFlowData->stripped, pReceiver->ptrPduBuff);
           SFLOW_PUT_32(pFlowData->header_length, pReceiver->ptrPduBuff);
           SFLOW_PUT_DATA(pFlowData->header_bytes, pFlowData->header_length, pReceiver->ptrPduBuff);
        }
           break;
        default:
           LOG_MSG("sFlowReceiverFlowSampleWrite: flow record type %u not supported ", pFlowSampleRecord->tag);
            break;
     }
   }
}
/*********************************************************************
* @purpose Write counter sample to receiver buffer
*
* @param   pSampler       @b{(input)}  Pointer to source sampler
*                                      object
* @param   pFlowSample    @b{(input)}  Pointer to counter sample to be
*                                      written in the receiver buff
*
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
static void sFlowReceiverCounterSampleWrite(SFLOW_receiver_t *pReceiver,
                                            SFLOW_counters_sample_t *pCounterSample)
{
   L7_uint32     idx;
   SFLOW_counters_sample_record_t *pCounterSampleRecord;

   SFLOW_PUT_32(pCounterSample->sequence_number, pReceiver->ptrPduBuff);
   SFLOW_PUT_32(pCounterSample->source_id, pReceiver->ptrPduBuff);
   SFLOW_PUT_32(pCounterSample->num_records, pReceiver->ptrPduBuff);

   for (idx = 0; idx < pCounterSample->num_records; idx++)
   {
     pCounterSampleRecord = &pCounterSample->counter_records[idx];
     SFLOW_PUT_32(pCounterSampleRecord->tag, pReceiver->ptrPduBuff);
     SFLOW_PUT_32(pCounterSampleRecord->length, pReceiver->ptrPduBuff);
     switch (pCounterSampleRecord->tag)
     {
       case SFLOW_COUNTERS_GENERIC:
       {
            SFLOW_if_counters_t          *pCounterData;
            pCounterData = &pCounterSampleRecord->counter_data.generic;
            SFLOW_PUT_32(pCounterData->ifIndex,            pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifType,             pReceiver->ptrPduBuff);
            SFLOW_PUT_64(pCounterData->ifSpeed,            pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifDirection,        pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifStatus,           pReceiver->ptrPduBuff);
            SFLOW_PUT_64(pCounterData->ifInOctets,         pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifInUcastPkts,      pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifInMulticastPkts,  pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifInBroadcastPkts,  pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifInDiscards,       pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifInErrors,         pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifInUnknownProtos,  pReceiver->ptrPduBuff);
            SFLOW_PUT_64(pCounterData->ifOutOctets,        pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifOutUcastPkts,     pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifOutMulticastPkts, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifOutBroadcastPkts, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifOutDiscards,      pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifOutErrors,        pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->ifPromiscuousMode,  pReceiver->ptrPduBuff);
       }
            break;
       case SFLOW_COUNTERS_ETHERNET:
       {
            SFLOW_ethernet_counters_t    *pCounterData;
            pCounterData = &pCounterSampleRecord->counter_data.ethernet;
            SFLOW_PUT_32(pCounterData->dot3StatsAlignmentErrors, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsFCSErrors, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsSingleCollisionFrames, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsMultipleCollisionFrames, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsSQETestErrors, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsDeferredTransmissions, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsLateCollisions, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsExcessiveCollisions, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsInternalMacTransmitErrors, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsCarrierSenseErrors, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsFrameTooLongs, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsInternalMacReceiveErrors, pReceiver->ptrPduBuff);
            SFLOW_PUT_32(pCounterData->dot3StatsSymbolErrors, pReceiver->ptrPduBuff);
        }
            break;
       default:
          LOG_MSG("sFlowReceiverCounterSampleWrite: flow record type %u not supported ", pCounterSampleRecord->tag);
          break;
     }
   }
}
/*********************************************************************
* @purpose Write counter sample to receiver buffer
*
* @param   pSampler       @b{(input)}  Pointer to source sampler
*                                      object
* @param   pFlowSample    @b{(input)}  Pointer to counter sample to be
*                                      written in the receiver buff
*
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
static void sFlowReceiverSampleDatagramHeaderWrite(SFLOW_receiver_t *pReceiver)
{
  L7_uint32       uintVal, startOffset = 0;
  
  L7_inet_addr_t  ipAddr;
  /* Populate the datagram header fields */
  /* Version */
   pReceiver->sFlowDatagram.datagram_version = FD_SFLOW_PROTOCOL_VERSION;
  /* Agent address */
  if (pReceiver->rcvrCfg->sFlowRcvrAddressType == L7_AF_INET)
  {
    startOffset = L7_IP6_ADDR_LEN - L7_IP_ADDR_LEN;
    if (sFlowAgentAddrGet(&ipAddr) != L7_SUCCESS)
    {
      ipAddr.addr.ipv4.s_addr = 0;
      LOG_MSG("sFlowReceiverSampleDatagramHeaderWrite: IPv4 agent address get failed");
    }
    pReceiver->sFlowDatagram.agent_address = ipAddr;
  }
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  else if (pReceiver->rcvrCfg->sFlowRcvrAddressType == L7_AF_INET6)
  {
    if (sFlowAgentAddrGet(&ipAddr) != L7_SUCCESS)
    {
      memset((L7_uchar8 *)&ipAddr.addr.ipv6, L7_NULL, sizeof(ipAddr.addr.ipv6));
      LOG_MSG("sFlowReceiverSampleDatagramHeaderWrite: IPv6 agent address get failed");
    }
    pReceiver->sFlowDatagram.agent_address = ipAddr;
  }
#endif

  /* uptime */
  pReceiver->sFlowDatagram.uptime  = osapiTimeMillisecondsGet();

  /* Write the datagram header into the pduBuffer */
  pReceiver->ptrPduBuff = &pReceiver->pduBuff[startOffset];
  /* Version */
  SFLOW_PUT_32(pReceiver->sFlowDatagram.datagram_version, pReceiver->ptrPduBuff);
  /* Agent Address type */
  uintVal = pReceiver->sFlowDatagram.agent_address.family;
  SFLOW_PUT_32(uintVal, pReceiver->ptrPduBuff);
  if (pReceiver->sFlowDatagram.agent_address.family == L7_AF_INET6)
  {
   /* Agent Address */
    SFLOW_PUT_DATA(&pReceiver->sFlowDatagram.agent_address.addr.ipv6.in6.addr8, 
                    L7_IP6_ADDR_LEN, pReceiver->ptrPduBuff);
  }
  else
  {
    inetAddressGet(L7_AF_INET,&pReceiver->sFlowDatagram.agent_address,&uintVal);
   /* Agent Address */
    SFLOW_PUT_32(uintVal, pReceiver->ptrPduBuff);
  }
  /* Sub-Agent ID */
  SFLOW_PUT_32(pReceiver->sFlowDatagram.sub_agent_id, pReceiver->ptrPduBuff);
  /* sequence number */
  SFLOW_PUT_32(pReceiver->sFlowDatagram.sequence_number, pReceiver->ptrPduBuff);
  /* uptime */
  SFLOW_PUT_32(pReceiver->sFlowDatagram.uptime, pReceiver->ptrPduBuff);
  /* num records */
  SFLOW_PUT_32(pReceiver->sFlowDatagram.num_records, pReceiver->ptrPduBuff);
}
/*********************************************************************
* @purpose Send out a sFlow datagram and re-init structures
*
* @param   pReceiver       @b{(input)}  Pointer to receiver object
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t sFlowReceiverDatagramSend(SFLOW_receiver_t *pReceiver)
{
  L7_uint32 bytesSent, uintVal;
  L7_uint32 startOffset = 0;
  L7_RC_t   rc = L7_SUCCESS;

  if (pReceiver)
  {
    SFLOW_TRACE(SFLOW_DEBUG_SAMPLER,"Datagram Send request for receiver %u",
                pReceiver->rcvrCfg->sFlowRcvrIndex);
    if (inetIsAddressZero(&pReceiver->rcvrCfg->sFlowRcvrAddress) == L7_FALSE)
    {
      sFlowReceiverSampleDatagramHeaderWrite(pReceiver);

      /* check what is the address type */
      if (pReceiver->rcvrCfg->sFlowRcvrAddressType == L7_AF_INET)
      {
        L7_sockaddr_in_t saddr;

        startOffset = L7_IP6_ADDR_LEN - L7_IP_ADDR_LEN;
        saddr.sin_family = L7_AF_INET;
        saddr.sin_port   = osapiHtons(pReceiver->rcvrCfg->sFlowRcvrPort);
        if (inetAddressGet(L7_AF_INET, &pReceiver->rcvrCfg->sFlowRcvrAddress, &uintVal) == L7_SUCCESS)
        {
          saddr.sin_addr.s_addr = osapiHtonl(uintVal);
        }
        else
        {
          saddr.sin_addr.s_addr = 0;
        }
        if (osapiSocketSendto(agent.sFlowAgentv4Socket,(pReceiver->pduBuff + startOffset), 
                              SFLOW_SAMPLE_DATAGRAM_IP4_LEN + pReceiver->pduLen, 0, 
                              (L7_sockaddr_t *)&saddr, sizeof(saddr), &bytesSent) 
                              != L7_SUCCESS)
        {
          SFLOW_TRACE(SFLOW_DEBUG_SAMPLER, "Failed to send v4 datagram errno: %d", osapiErrnoGet());
        }
        else
        {
          sFlowDebugPacketTxTrace(&pReceiver->rcvrCfg->sFlowRcvrAddress, bytesSent, 
                                   pReceiver->sFlowDatagram.sequence_number);
          SFLOW_TRACE(SFLOW_DEBUG_SAMPLER,"IPv4 Datagram Send by receiver %u seqNo %u numRecords %u pduLen %u \
sent %u",
                 pReceiver->rcvrCfg->sFlowRcvrIndex, pReceiver->sFlowDatagram.sequence_number,
                 pReceiver->sFlowDatagram.num_records, pReceiver->pduLen + SFLOW_SAMPLE_DATAGRAM_IP4_LEN, 
                 bytesSent);
        }
      }
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
      else if (pReceiver->rcvrCfg->sFlowRcvrAddressType == L7_AF_INET6)
      {
        L7_sockaddr_in6_t saddr;

        saddr.sin6_family = L7_AF_INET6;
        saddr.sin6_port = osapiHtons(pReceiver->rcvrCfg->sFlowRcvrPort);
        saddr.sin6_addr = pReceiver->rcvrCfg->sFlowRcvrAddress.addr.ipv6;
        saddr.sin6_scope_id = 0;
        if (osapiSocketSendto(agent.sFlowAgentv6Socket , pReceiver->pduBuff, 
                              SFLOW_SAMPLE_DATAGRAM_IP6_LEN + pReceiver->pduLen, 0,
                              (L7_sockaddr_t *)&saddr,sizeof(saddr), &bytesSent) 
                              != L7_SUCCESS)
        {
          SFLOW_TRACE(SFLOW_DEBUG_SAMPLER, "Failed to send v6 datagram errno: %d", osapiErrnoGet());
        }
        else
        {
          sFlowDebugPacketTxTrace(&pReceiver->rcvrCfg->sFlowRcvrAddress, bytesSent, 
                                   pReceiver->sFlowDatagram.sequence_number);
          SFLOW_TRACE(SFLOW_DEBUG_SAMPLER,"IPv6 Datagram Send by receiver %u seqNo %u numRecords %u pduLen %u sent %u",
                 pReceiver->rcvrCfg->sFlowRcvrIndex, pReceiver->sFlowDatagram.sequence_number,
                 pReceiver->sFlowDatagram.num_records, pReceiver->pduLen, bytesSent);
        }
      }
#endif
    }/* End of address is zero check */
   
    /* Initialize the receiver buffer */
    pReceiver->sFlowDatagram.sequence_number++;
    pReceiver->pduLen = 0;
    pReceiver->sFlowDatagram.num_records = 0;
    pReceiver->ptrPduBuff = pReceiver->pduBuff + SFLOW_SAMPLE_DATAGRAM_IP6_LEN;
    memset(pReceiver->pduBuff, 0x00, SFLOW_MAX_DATAGRAM_SIZE);
  }
  return rc;
}
