/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_ctrl.c
*
* @purpose   SFlow
*
* @component sflow
*
* @comments  none
*
* @create 20-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#include <string.h>
#include "commdefs.h"
#include "l7_product.h"
#include "datatypes.h"
#include "osapi_support.h"
#include "osapi_sockdefs.h"
#include "dtl_sflow.h"

#include "sflow_cfg.h"
#include "sflow_mib.h"
#include "sflow_ctrl.h"
#include "sflow_cnfgr.h"
#include "sflow_outcalls.h"
#include "sflow_db.h"
#include "sflow_debug.h"

/* Interface Callback processing */
static void sFlowIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event,
                                   NIM_CORRELATOR_t correlator);
static L7_RC_t sFlowIntfAttach(L7_uint32 intIfNum);
static L7_RC_t sFlowIntfDetach(L7_uint32 intIfNum);
static L7_RC_t sFlowIntfDelete(L7_uint32 intIfNum);
static L7_RC_t sFlowIntfStatsReset(L7_uint32 intIfNum);

static L7_RC_t sFlowSamplingRateThrottle(L7_uint32 dsIndex, L7_uint32 instance);
/* Timer Process event */
static L7_RC_t sFlowTimerTickProcess(void);

/* Config changes apply routines */
static L7_RC_t sFlowReceiverOwnerStringApply(L7_uint32 rcvrIndex, 
                                             L7_uchar8 *ownerStr, 
                                             L7_uint32 timeout);
static L7_RC_t sFlowReceiverAddressApply(L7_uint32 rcvrIndex,
                                         L7_inet_addr_t *rcvrAddr);

/* sampler, poller object methods */
static L7_RC_t sFlowPollerDelete(SFLOW_poller_t *pPoller);
static L7_RC_t sFlowSamplerDelete(SFLOW_sampler_t *pSampler);
static L7_RC_t sFlowSamplerAdd(L7_uint32 dsIndex, L7_uint32 instance, 
                               L7_uint32 rcvrIndx);
static L7_RC_t sFlowPollerAdd(L7_uint32 dsIndex, L7_uint32 instance, 
                              L7_uint32 rcvrIndx);

extern SFLOW_agent_t agent;
/*********************************************************************
* @purpose   Task to handle all sFlow management & PDU messages
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
void sFlowTask(void)
{
  sFlowEventMsg_t eventMsg;
  sflowPDU_Msg_t  pduMsg;
  L7_uint32       msgId = L7_NULL;

  osapiTaskInitDone(L7_SFLOW_TASK_SYNC);
  do
  {
    osapiSemaTake(agent.sFlowMsgQSema, L7_WAIT_FOREVER);
    if ((osapiMessageReceive(agent.sFlowEventQueue, (void *)&eventMsg,
                             SFLOW_MSG_SIZE, L7_NO_WAIT))
                            != L7_SUCCESS)
    {
      if ((osapiMessageReceive(agent.sFlowPacketQueue, (void *)&pduMsg,
                             SFLOW_MSG_SIZE, L7_NO_WAIT))
                            != L7_SUCCESS)
      {
        LOG_MSG("sFlowTask(): Internal event msg receive failed");
      }
      else
      {
        msgId = pduMsg.msgId;
      }
    }
    else
    {
      msgId = eventMsg.msgId;
    }
    osapiWriteLockTake(agent.sFlowCfgRWLock, L7_WAIT_FOREVER);
    SFLOW_TRACE(SFLOW_DEBUG_EVENTS,"Event %d received by sFlow task", msgId);
    /* process the message */
    switch (msgId)
    {
      case (SFLOW_CNFGR_MSG):
        sFlowCnfgrHandle(&eventMsg.u.cmdData);
        break;
      case (SFLOW_INTF_EVENT):
      /* Process interface change event notfied by NIM */
        sFlowIntfChangeProcess(eventMsg.dataSource,
                               eventMsg.u.sflowIntfChangeParms.event,
                               eventMsg.u.sflowIntfChangeParms.correlator);
        break;
      case SFLOW_TIMER_TICK:
        sFlowTimerTickProcess();
        break;
      case SFLOW_SAMPLE_RX:
         sFlowSampleProcess(&pduMsg);
        break;
      case SFLOW_OWNER_STRING_TIMEOUT_SET:
        sFlowReceiverOwnerStringApply(eventMsg.receiverIndex,
                                      eventMsg.u.stringVal,
                                      eventMsg.uintVal);
        break;
      case SFLOW_SAMPLING_RATE_SET:
        sFlowSamplerRateApply(eventMsg.dataSource, eventMsg.sflowInstance,
                              eventMsg.uintVal);
        break;
      case SFLOW_POLL_INTERVAL_SET:
        sFlowPollerIntervalApply(eventMsg.dataSource, eventMsg.sflowInstance,
                                 eventMsg.uintVal);
        break;
      case SFLOW_COUNTER_RCVR_SET:
        sFlowPollerReceiverApply(eventMsg.dataSource, eventMsg.sflowInstance,
                                 eventMsg.receiverIndex);
        break;
      case SFLOW_SAMPLER_RCVR_SET:
        sFlowSamplerReceiverApply(eventMsg.dataSource, eventMsg.sflowInstance,
                                  eventMsg.receiverIndex);
        break;
      case SFLOW_RCVR_ADDR_SET:
        sFlowReceiverAddressApply(eventMsg.receiverIndex, &eventMsg.u.addrVal);
        break;
      case SFLOW_CHOKE:
        sFlowSamplingRateThrottle(eventMsg.dataSource, eventMsg.sflowInstance);
        break;
      default:
        LOG_MSG("sFlowTask(): invalid message type:%d. %s:%d\n",
                eventMsg.msgId, __FILE__, __LINE__);
        break;
    }
    osapiWriteLockGive(agent.sFlowCfgRWLock);
  } while (1); /* eternal loop */
}

/*********************************************************************
* @purpose  Callback function to propogate Interface notifications to
*           sFlow Task
*
* @param    intIfNum   @b{(input)}  Interface number
* @param    event      @b{(input)}  Event type
* @param    correlator @b{(input)}  Correlator for NIM event
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
                                NIM_CORRELATOR_t correlator)
{
  sFlowEventMsg_t           msg;
  L7_RC_t                   rc = L7_FAILURE;
  NIM_EVENT_COMPLETE_INFO_t status;
  L7_BOOL                   skip = L7_FALSE;

  status.intIfNum     = intIfNum;
  status.component    = L7_SFLOW_COMPONENT_ID;
  status.event        = event;
  status.correlator   = correlator;

  if (!(sFlowIsReady()))
  {
    LOG_MSG("sFlowIntfChangeCallback: Received an interface change callback while not ready to receive it");
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  /* Skip intfValidCheck fails*/
  if (sFlowIsValidIntf(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  switch (event)
  {
    case L7_ATTACH:
    case L7_DETACH:
    case L7_DELETE:
    case L7_PORT_STATS_RESET:
    case L7_LAG_ACQUIRE:
    case L7_LAG_RELEASE:
      skip = L7_FALSE;
      break;
    default:
      skip = L7_TRUE;
      break;
  }

  if (skip == L7_FALSE)
  {
     msg.msgId      = SFLOW_INTF_EVENT;
     msg.dataSource = SFLOW_VAL_TO_DS_INDEX(intIfNum,L7_SFLOW_DS_TYPE_IFINDEX);
     msg.sflowInstance = FD_SFLOW_INSTANCE;
     msg.u.sflowIntfChangeParms.event = event;
     msg.u.sflowIntfChangeParms.correlator = correlator;
    if (osapiMessageSend(agent.sFlowEventQueue, &msg,SFLOW_MSG_SIZE, L7_WAIT_FOREVER,
                           L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
    { 
      if (osapiSemaGive(agent.sFlowMsgQSema) != L7_SUCCESS)
      {
        LOG_MSG("sFlowIntfChangeCallback: Failed to give msgQueue semaphore\n");
      }
    }
  }
  else
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  return rc;
}
/*********************************************************************
* @purpose   This function is used to send timer events
*
* @param     timerCtrlBlk    @b{(input)}   Timer Control Block
* @param     ptrData         @b{(input)}   Ptr to passed data
*
* @returns   None
*
* @notes     None
* @end
*********************************************************************/
void sflowTimerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData)
{
  L7_int32         rc;
  sFlowEventMsg_t  msg;

  msg.msgId = SFLOW_TIMER_TICK;

  rc = osapiMessageSend(agent.sFlowEventQueue, &msg, SFLOW_MSG_SIZE, L7_NO_WAIT,
                        L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("sflowTimerExpiryHdlr(): Sflow timer tick send failed\n");
  }
  else
  {
    if (osapiSemaGive(agent.sFlowMsgQSema) != L7_SUCCESS)
    {
      LOG_MSG("sflowTimerExpiryHdlr: MsgQSema give failed");
    }
  }
}
/*********************************************************************
* @purpose  Process interface change notifications
*
* @param    intIfNum   @b{(input)}  Interface number
* @param    event      @b{(input)}  Event type
* @param    correlator @b{(output)}  Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static void sFlowIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event,
                                   NIM_CORRELATOR_t correlator)
{
  L7_RC_t                   rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;

  status.intIfNum        = intIfNum;
  status.component       = L7_SFLOW_COMPONENT_ID;
  status.event           = event;
  status.correlator      = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED ;

  if (sFlowIsReady() == L7_FALSE)
  {
    LOG_MSG("sFlowIntfChangeProcess: Received an interface change callback while not ready to receive it");
    rc = L7_FAILURE;
    status.response.rc  = rc;
    nimEventStatusCallback(status);
    return;
  }

  switch (event)
  {
    case L7_ATTACH:
    case L7_LAG_RELEASE:
         rc = sFlowIntfAttach(intIfNum);
         break;
    case L7_DETACH:
    case L7_LAG_ACQUIRE:
         rc = sFlowIntfDetach(intIfNum);
         break;
    case L7_DELETE:
         rc = sFlowIntfDelete(intIfNum);
         break;
    case L7_PORT_STATS_RESET:
         rc = sFlowIntfStatsReset(intIfNum);
         break;
    default:
    break;
  }

  status.response.rc  = rc;
  nimEventStatusCallback(status);
}
/*********************************************************************
* @purpose  To process the Callback for L7_PORT_STATS_RESET
*
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t sFlowIntfStatsReset(L7_uint32 intIfNum)
{
  L7_uint32        dsIndex, instance;
  SFLOW_poller_t  *pPoller = L7_NULLPTR;

  dsIndex  = SFLOW_VAL_TO_DS_INDEX(intIfNum,L7_SFLOW_DS_TYPE_IFINDEX);
  instance = FD_SFLOW_INSTANCE;
  /* check if a poller object exists and enable it operationally */
  if ((pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    pPoller->counterSampleSeqNo = 0;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To process the Callback for L7_ATTACH/L7_LAG_RELEASE
*
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t sFlowIntfAttach(L7_uint32 intIfNum)
{
  L7_uint32        dsIndex, instance;
  SFLOW_sampler_t *pSampler = L7_NULLPTR;
  SFLOW_poller_t  *pPoller = L7_NULLPTR;

  dsIndex  = SFLOW_VAL_TO_DS_INDEX(intIfNum,L7_SFLOW_DS_TYPE_IFINDEX);
  instance = FD_SFLOW_INSTANCE;
  /* check if a sampler object exists and enable it operationally */
  if ((pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (sFlowSamplerOperModeSet(pSampler, L7_ENABLE) != L7_SUCCESS)
    {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SFLOW_COMPONENT_ID,
          "sFlowIntfAttach: could not enable the sampler object for interface %u."
          " The configuration of new stack member ports added failed.",
              intIfNum);
    }
  }

  /* check if a poller object exists and enable it operationally */
  if ((pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (sFlowPollerOperModeSet(pPoller, L7_ENABLE) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SFLOW_COMPONENT_ID,
          "sFlowIntfAttach: could not enable the poller object for interface %u."
          " The configuration of new stack member ports added failed.",
              intIfNum);
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To process the Callback for L7_DETACH/L7_ACQUIRE
*
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t sFlowIntfDetach(L7_uint32 intIfNum)
{
  L7_uint32        dsIndex, instance;
  SFLOW_sampler_t *pSampler = L7_NULLPTR;
  SFLOW_poller_t  *pPoller = L7_NULLPTR;

  dsIndex  = SFLOW_VAL_TO_DS_INDEX(intIfNum,L7_SFLOW_DS_TYPE_IFINDEX);
  instance = FD_SFLOW_INSTANCE;
  /* check if a sampler object exists and disable it operationally */
  if ((pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (sFlowSamplerOperModeSet(pSampler, L7_DISABLE) != L7_SUCCESS)
    {
      LOG_MSG("sFlowIntfDetach: could not disable the sampler object for interface %u",
              intIfNum);
    }
  }

  /* check if a poller object exists and disable it operationally */
  if ((pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (sFlowPollerOperModeSet(pPoller, L7_DISABLE) != L7_SUCCESS)
    {
      LOG_MSG("sFlowIntfDetach: could not disable the poller object for interface %u",
              intIfNum);
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To process the Callback for L7_DELETE
*
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t sFlowIntfDelete(L7_uint32 intIfNum)
{
  L7_uint32        dsIndex, instance;
  SFLOW_sampler_t *pSampler = L7_NULLPTR;
  SFLOW_poller_t  *pPoller = L7_NULLPTR;
  
  dsIndex  = SFLOW_VAL_TO_DS_INDEX(intIfNum,L7_SFLOW_DS_TYPE_IFINDEX);
  instance = FD_SFLOW_INSTANCE;
  /* check if a sampler object exists and delete it */
  if ((pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (sFlowSamplerDelete(pSampler) != L7_SUCCESS)
    {
      LOG_MSG("sFlowIntfDelete: could not delete the sampler object for interface %u",
              intIfNum);
    }
  }

  /* check if a poller object exists and delete it */
  if ((pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (sFlowPollerDelete(pPoller) != L7_SUCCESS)
    {
      LOG_MSG("sFlowIntfDelete: could not delete the poller object for interface %u",
              intIfNum);
    }
    else
    {
      /* set flag to indicate configuration has changed */
      agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To assign owner string for a receiver
*
* @param    rcvrIndex @b{(input)} Receiver table index whose owner
*                                 string is to be modified
* @param    ownerStr  @b{(input)} Ownerstring set by the collector
* @param    timeout   @b{(input)} Timeout for the entry to be removed
*                                 by the agent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t sFlowReceiverOwnerStringApply(L7_uint32 rcvrIndex,
                                             L7_uchar8 *ownerStr,
                                             L7_uint32 timeout)
{
  SFLOW_receiver_t *pReceiver;

  pReceiver = &agent.receiver[rcvrIndex - 1];
  if (timeout == L7_NULL)
  {
    memset(pReceiver->rcvrCfg->sFlowRcvrOwner, 0x00, L7_SFLOW_OWNER_STRING_LEN);
  }

  if (osapiStrncmp(pReceiver->rcvrCfg->sFlowRcvrOwner, "" , 
                   L7_SFLOW_OWNER_STRING_LEN) == L7_NULL)
  {
    pReceiver->rcvrCfg->sFlowRcvrTimeout = L7_NULL;
    /* Build default receiver configuration */
    sFlowBuildDefaultReceiverConfigData(rcvrIndex);
    /* Receiver expired - Delete all the sampler and poller objects 
       for this receiver */
    sFlowSamplerPollerInstancesCleanup();
  }

  SFLOW_TRACE(SFLOW_DEBUG_INTERNAL, "Receiver %u ownerString %s rcvrTimeout %u", rcvrIndex,
              ownerStr, timeout);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To assign receiver ip address
*
* @param    rcvrIndex @b{(input)} Receiver table index whose owner
*                                 string is to be modified
* @param    rcvrAddr  @b{(input)} Ownerstring set by the collector
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If a null address is set, disable all the pollers, samplers
*           for this receiver operationally. Similarly, when a non-zero
*           ip address is set, re-enable all the pollers and samplers
*           for this receiver
*
* @end
*
*********************************************************************/
static L7_RC_t sFlowReceiverAddressApply(L7_uint32 rcvrIndex,
                                         L7_inet_addr_t *rcvrAddr)
{
  SFLOW_receiver_t *pReceiver;
  L7_uint32         mode;

  pReceiver = &agent.receiver[rcvrIndex - 1];
  /* Check if this receiver already expired */
  if (pReceiver->rcvrCfg->sFlowRcvrTimeout == 0)
  {
    return L7_FAILURE;
  }

/*
  If a null address is set, disable all the pollers, samplers
  for this receiver operationally. Similarly, when a non-zero
  ip address is set, re-enable all the pollers and samplers
  for this receiver
*/
  if (inetIsAddressZero(rcvrAddr) == L7_FALSE)
  {
    mode = L7_ENABLE;
  }
  else
  {
    mode = L7_DISABLE;
  }
  
  if (sFlowSamplerOperModeSet(L7_NULLPTR, mode) != L7_SUCCESS)
  {
    LOG_MSG("sFlowReceiverAddressApply: Failed to set one or more sampler objects");
    return L7_FAILURE;
  }
  if (sFlowPollerOperModeSet(L7_NULLPTR, mode) != L7_SUCCESS)
  {
    LOG_MSG("sFlowReceiverAddressApply: Failed to set one or more poller objects");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To apply polling interval for the specified datasource
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
* @param    pollInterval         @b{(input)} Poll interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t sFlowPollerIntervalApply(L7_uint32 dsIndex, L7_uint32 instance,
                                L7_uint32 pollInterval)
{
  SFLOW_poller_t  *pPoller = L7_NULLPTR;

  if ((pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    /* apply the changed configuration */
    if (sFlowPollerOperModeSet(pPoller, L7_ENABLE) != L7_SUCCESS)
    {
      LOG_MSG("sFlowPollerIntervalApply: Failed to enable poller for dsIndex %u instance %u",
              dsIndex, instance);
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To apply sampling rate for the specified datasource
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
* @param    samplingRate         @b{(input)} sampling rate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API can be cauled by application to modify the
*           user configured value in case of congestion
*
* @end
*
*********************************************************************/
L7_RC_t sFlowSamplerRateApply(L7_uint32 dsIndex, L7_uint32 instance,
                              L7_uint32 samplingRate)
{
  SFLOW_sampler_t  *pSampler = L7_NULLPTR;

  if ((pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    if (samplingRate != pSampler->samplerCfg->sFlowFsPacketSamplingRate)
    {
      /* set flag to indicate configuration has changed */
       agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
    }
    /* Update the configuration */
    pSampler->samplerCfg->sFlowFsPacketSamplingRate = samplingRate;
    /* apply the changed configuration */
    if (sFlowSamplerOperModeSet(pSampler, L7_ENABLE) != L7_SUCCESS)
    {
      LOG_MSG("sFlowSamplerRateApply: Failed to enable sampler for dsIndex %u instance %u",
              dsIndex, instance);
      return L7_FAILURE;
    }
    SFLOW_TRACE(SFLOW_DEBUG_SAMPLER, "Sampler %u  sampling rate set to %u", dsIndex,
                samplingRate);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To apply the newly assigned receiver index to the sampler
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
* @param    rcvrIndex            @b{(input)} Receiver table index
*
* @returns  L7_SUCCESS         On successful creation of sampler object
* @returns  L7_TABLE_IS_FULL   no more space for new sampler objects
* @returns  L7_ALREADY_CONFIGURED Trying to add an existing object
* @returns  L7_FAILURE    other failure
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t sFlowSamplerReceiverApply(L7_uint32 dsIndex, L7_uint32 instance,
                                  L7_uint32 rcvrIndex)
{
  L7_RC_t          rc = L7_SUCCESS;
  SFLOW_sampler_t *pSampler = L7_NULLPTR;

  if (rcvrIndex)
  {
    if (!agent.sFlowCfg->receiverGroup[rcvrIndex - 1].sFlowRcvrTimeout)
    {
      /* trying to assign a non-active receiver */
      return L7_FAILURE;
    }
    rc = sFlowSamplerAdd(dsIndex, instance, rcvrIndex);
  }
  else
  {
    if ((pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
    {
      if (sFlowSamplerDelete(pSampler) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}
/*********************************************************************
* @purpose  To apply the newly assigned receiver index to the poller
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
* @param    rcvrIndex            @b{(input)} Receiver table index
*
* @returns  L7_SUCCESS         On successful creation of poller object
* @returns  L7_TABLE_IS_FULL   no more space for new poller objects
* @returns  L7_ALREADY_CONFIGURED Trying to add an existing object
* @returns  L7_FAILURE    other failure
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t sFlowPollerReceiverApply(L7_uint32 dsIndex, L7_uint32 instance,
                                 L7_uint32 rcvrIndex)
{
  L7_RC_t         rc = L7_SUCCESS;
  SFLOW_poller_t *pPoller = L7_NULLPTR;

  if (rcvrIndex)
  {
    if (!agent.sFlowCfg->receiverGroup[rcvrIndex - 1].sFlowRcvrTimeout)
    {
      /* trying to assign a non-active receiver */
      return L7_FAILURE;
    }

    rc = sFlowPollerAdd(dsIndex, instance, rcvrIndex);
  }
  else
  {
    if ((pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
    {
      if (sFlowPollerDelete(pPoller) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Process timer event
*
* @param    none
*
* @returns  void
*
* @notes    This function is called every timer interval, which is 1 second
*
* @end
*********************************************************************/
static L7_RC_t sFlowTimerTickProcess(void)
{
  L7_uint32 idx;
  L7_BOOL   expiredReceivers = L7_FALSE;

  if (sFlowIsReady() == L7_TRUE)
  {
    /* send outstanding data and expire the timedout receivers*/
    for (idx = 0; idx < SFLOW_MAX_RECEIVERS; idx++)
    {
      /* Skip non-active receiver entries */
      if (!agent.sFlowCfg->receiverGroup[idx].sFlowRcvrTimeout)
      {
        continue;
      }

      /* flush the receiver if any data is present */
      if (agent.receiver[idx].pduLen)
      {
         SFLOW_TRACE(SFLOW_DEBUG_SAMPLER,"Flushing rcvr %u buffer pduLen %u",
                     idx+1, agent.receiver[idx].pduLen);
         if (sFlowReceiverDatagramSend(&agent.receiver[idx]) != L7_SUCCESS)
         {
           LOG_MSG("sFlowTimerTickProcess: Failed to flush the receiver data");
         }
      }

      /* time out the server when running in snmp mode*/
      if (agent.sFlowCfg->receiverGroup[idx].sFlowRcvrSnmpMode == L7_ENABLE)
      {
        if (agent.sFlowCfg->receiverGroup[idx].sFlowRcvrTimeout)
        {
          agent.sFlowCfg->receiverGroup[idx].sFlowRcvrTimeout--;
        }
      }

      /* check if receiver Timer expired */
      if (!agent.sFlowCfg->receiverGroup[idx].sFlowRcvrTimeout)
      {
        SFLOW_TRACE(SFLOW_DEBUG_TIMER, "Receiver %u  expired!", idx+1);
        /* Default the receiver data */
        sFlowBuildDefaultReceiverConfigData(agent.sFlowCfg->receiverGroup[idx].sFlowRcvrIndex);
        /* Mark as configuration changed */
        agent.sFlowCfg->cfgHdr.dataChanged = L7_TRUE;
        expiredReceivers = L7_TRUE;
      }
    } /* Iterate thru the receivers */

    /* default the poller and sampler objects with expired receivers */
    if (expiredReceivers == L7_TRUE)
    {
      sFlowSamplerPollerInstancesCleanup();
    }

    /* Decrement congestion alert timer if any congestion was reported */
    if (agent.congestionAlert)
    {
      agent.congestionAlert--;
    }
  }/* End of check if sFlow application is ready */
  /* Invoke all the expired poller timers here */
  appTimerProcess(agent.timerCB);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Delete the poller and sampler objects whose receivers
*           expired
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void sFlowSamplerPollerInstancesCleanup(void)
{
  SFLOW_poller_t  *pPoller = L7_NULLPTR;
  SFLOW_sampler_t *pSampler = L7_NULLPTR;
  L7_uint32        dsIndex, instance;
  
  /* Inspect Poller instances */
  dsIndex = instance = 0;
  while ((pPoller = sFlowPollerInstanceGet(dsIndex, instance, 
                                           L7_MATCH_GETNEXT)))
  {
    dsIndex  = pPoller->key.dsIndex;
    instance = pPoller->key.sFlowInstance;
    if (pPoller->pollerCfg->sFlowCpReceiver)
    {
      if (!agent.sFlowCfg->receiverGroup[pPoller->pollerCfg->sFlowCpReceiver - 1].sFlowRcvrTimeout)
      {
        if (sFlowPollerDelete(pPoller) != L7_SUCCESS)
        {
          LOG_MSG("sFlowSamplerPollerInstancesCleanup: could not delete poller for dsIndex %u instance %u",
                   dsIndex, instance);
        }
      } /* Check if the assigned receiver is active */
    }/* check if a valid receiver is configured */
  }/* iterate thru all the available poller objects */

  /* Inspect sampler instances */
  dsIndex = instance = 0;
  while ((pSampler = sFlowSamplerInstanceGet(dsIndex, instance,
                                             L7_MATCH_GETNEXT)))
  {
    dsIndex  = pSampler->key.dsIndex;
    instance = pSampler->key.sFlowInstance;
    if (pSampler->samplerCfg->sFlowFsReceiver)
    {
      if (!agent.sFlowCfg->receiverGroup[pSampler->samplerCfg->sFlowFsReceiver - 1].sFlowRcvrTimeout)
      {
        if (sFlowSamplerDelete(pSampler) != L7_SUCCESS)
        {
          LOG_MSG("sFlowSamplerPollerInstancesCleanup: could not delete sampler for dsIndex %u instance %u",
                   dsIndex, instance);
        }
      } /* Check if the assigned receiver is active */
    }/* check if a valid receiver is configured */
  }/* iterate thru all the available poller objects */
}
/*********************************************************************
* @purpose  Adds the specified poller object
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
* @param    rcvrIndx             @b{(input)} receiver table index
*
* @returns  L7_SUCCESS         On successful creation of poller object
* @returns  L7_TABLE_IS_FULL   no more space for new poller objects
* @returns  L7_ALREADY_CONFIGURED Trying to add an existing object
* @returns  L7_FAILURE    other failure
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t sFlowPollerAdd(L7_uint32 dsIndex, L7_uint32 instance,
                              L7_uint32 rcvrIndx)
{
  L7_uint32            idx, emptyIdx = SFLOW_MAX_POLLERS;
  L7_RC_t              rc = L7_SUCCESS;
  SFLOW_poller_t      *pPoller;

  /* get an empty row in the cfg table */
  for (idx = 0; idx < SFLOW_MAX_POLLERS; idx++)
  {
    if (emptyIdx == SFLOW_MAX_POLLERS &&
        !agent.sFlowCfg->pollerGroup[idx].sFlowCpDataSource)
    {
      emptyIdx = idx;
    }
    if (agent.sFlowCfg->pollerGroup[idx].sFlowCpDataSource == dsIndex &&
        agent.sFlowCfg->pollerGroup[idx].sFlowCpInstance == instance)
    {
      return L7_ALREADY_CONFIGURED;
    }
  }

  if (emptyIdx == SFLOW_MAX_POLLERS)
  {
    return L7_TABLE_IS_FULL;
  }

  agent.sFlowCfg->pollerGroup[emptyIdx].sFlowCpDataSource = dsIndex;
  agent.sFlowCfg->pollerGroup[emptyIdx].sFlowCpInstance   = instance;
  agent.sFlowCfg->pollerGroup[emptyIdx].sFlowCpReceiver   = rcvrIndx;

  /* Add poller object to datastore */
  rc = sFlowPollerInstanceAdd(dsIndex, instance, 
                              &agent.sFlowCfg->pollerGroup[emptyIdx]); 
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("sFlowPollerAdd: failed to add poller object for dsIndex %u instance %u",
           dsIndex, instance);
    /* remove the entry from cfg table */
    agent.sFlowCfg->pollerGroup[emptyIdx].sFlowCpDataSource = L7_NULL;
    return L7_FAILURE;
  }
 
  /* Poller successfully added */
  agent.numActivePollers++;
  SFLOW_TRACE(SFLOW_DEBUG_INTERNAL, "Poller dsIndex %u instance %u added!", dsIndex, instance);
  if ((pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)) 
                                        == L7_NULLPTR)
  {
    LOG_MSG("sFlowPollerAdd: failed to find the added poller object for dsIndex %u instance %u",
           dsIndex, instance);
    return L7_FAILURE;
  }

  /* enable poller operational functionality */
  if (sFlowPollerOperModeSet(pPoller, L7_ENABLE) != L7_SUCCESS)
  {
    LOG_MSG("sFlowPollerAdd: failed to enable poller for dsIndex %u instance %u", 
            dsIndex, instance);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Adds the specified sampler object
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
* @param    rcvrIndx             @b{(input)} receiver table index
*
* @returns  L7_SUCCESS         On successful creation of sampler object
* @returns  L7_TABLE_IS_FULL   no more space for new sampler objects
* @returns  L7_ALREADY_CONFIGURED Trying to add an existing object
* @returns  L7_FAILURE    other failure
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t sFlowSamplerAdd(L7_uint32 dsIndex, L7_uint32 instance,
                               L7_uint32 rcvrIndx)
{
  L7_uint32            idx, emptyIdx = SFLOW_MAX_SAMPLERS;
  L7_RC_t              rc = L7_SUCCESS;
  SFLOW_sampler_t     *pSampler;

  /* get an empty row in the cfg table */
  for (idx = 0; idx < SFLOW_MAX_SAMPLERS; idx++)
  {
    if (emptyIdx == SFLOW_MAX_SAMPLERS &&
        !agent.sFlowCfg->samplerGroup[idx].sFlowFsDataSource)
    {
      emptyIdx = idx;
    }
    if (agent.sFlowCfg->samplerGroup[idx].sFlowFsDataSource == dsIndex &&
        agent.sFlowCfg->samplerGroup[idx].sFlowFsInstance == instance)
    {
      return L7_ALREADY_CONFIGURED;
    }
  }

  if (emptyIdx == SFLOW_MAX_SAMPLERS)
  {
    return L7_TABLE_IS_FULL;
  }

  agent.sFlowCfg->samplerGroup[emptyIdx].sFlowFsDataSource = dsIndex;
  agent.sFlowCfg->samplerGroup[emptyIdx].sFlowFsInstance   = instance;
  agent.sFlowCfg->samplerGroup[emptyIdx].sFlowFsReceiver   = rcvrIndx;

  /* Add sampler object to datastore */
  rc = sFlowSamplerInstanceAdd(dsIndex, instance,
                              &agent.sFlowCfg->samplerGroup[emptyIdx]);
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("sFlowSamplerAdd: failed to add sampler object for dsIndex %u instance %u",
           dsIndex, instance);
    /* remove the entry from cfg table */
    agent.sFlowCfg->samplerGroup[emptyIdx].sFlowFsDataSource = L7_NULL;
    return L7_FAILURE;
  }
  /* Sampler successfully added */
  agent.numActiveSamplers++;
  SFLOW_TRACE(SFLOW_DEBUG_INTERNAL, "Sampler dsIndex %u instance %u added!", dsIndex, instance);
  if ((pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT))
                                        == L7_NULLPTR)
  {
    LOG_MSG("sFlowSamplerAdd: failed to find the added sampler object for dsIndex %u instance %u",
           dsIndex, instance);
    return L7_FAILURE;
  }

  /* enable sampler operational functionality */
  if (sFlowSamplerOperModeSet(pSampler, L7_ENABLE) != L7_SUCCESS)
  {
    LOG_MSG("sFlowSamplerAdd: failed to enable sampler for dsIndex %u instance %u",
            dsIndex, instance);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Delete the specified poller object
*
* @param    pPoller       @b{(input)} reference to the poller object
*
* @returns  L7_SUCCESS    On successful deletion
* @returns  L7_FAILURE    falure in action
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t sFlowPollerDelete(SFLOW_poller_t *pPoller)
{
  L7_RC_t              rc = L7_SUCCESS;
  SFLOW_poller_mib_t *pPollerCfg;

  /* disable poller operational functionality */
  if (sFlowPollerOperModeSet(pPoller, L7_DISABLE) != L7_SUCCESS)
  {
    LOG_MSG("sFlowPollerDelete: failed to disable poller for dsIndex %u instance %u",
            pPoller->key.dsIndex, pPoller->key.sFlowInstance);
    return L7_FAILURE;
  }
  pPollerCfg = pPoller->pollerCfg;
  /* delete the poller object from datastore */
  if ((rc = sFlowPollerInstanceDelete(pPoller->key.dsIndex, 
                                      pPoller->key.sFlowInstance))
                                      == L7_SUCCESS)
  {
    /* Poller successfully deleted */
    agent.numActivePollers--;
    SFLOW_TRACE(SFLOW_DEBUG_INTERNAL, "Poller dsIndex %u instance %u deleted!", 
                pPollerCfg->sFlowCpDataSource, pPollerCfg->sFlowCpInstance);
    /* default the configuration */
    sFlowBuildDefaultPollerConfigData(pPollerCfg);
  }
  return rc;
}
/*********************************************************************
* @purpose  Delete the specified sampler object
*
* @param    pSampler       @b{(input)} reference to the sampler object
*
* @returns  L7_SUCCESS    On successful deletion
* @returns  L7_FAILURE    falure in action
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t sFlowSamplerDelete(SFLOW_sampler_t *pSampler)
{
  L7_RC_t              rc = L7_SUCCESS;
  SFLOW_sampler_mib_t *pSamplerCfg;

  /* disable sampler operational functionality */
   if (sFlowSamplerOperModeSet(pSampler, L7_DISABLE) != L7_SUCCESS)
   {
     LOG_MSG("sFlowSamplerDelete: failed to disable sampler for dsIndex %u instance %u",
             pSampler->key.dsIndex, pSampler->key.sFlowInstance);
     return L7_FAILURE;
   }

  pSamplerCfg = pSampler->samplerCfg;
  /* delete the sampler object from datastore */
  if ((rc = sFlowSamplerInstanceDelete(pSampler->key.dsIndex, 
                                       pSampler->key.sFlowInstance))
                                       == L7_SUCCESS)
  {
    /* Sampler successfully deleted */
    agent.numActiveSamplers--;
    SFLOW_TRACE(SFLOW_DEBUG_INTERNAL, "Sampler dsIndex %u instance %u deleted!",
                pSamplerCfg->sFlowFsDataSource, pSamplerCfg->sFlowFsInstance);
    /* default the configuration */
    sFlowBuildDefaultSamplerConfigData(pSamplerCfg);
  }

  return rc;
}
/*********************************************************************
* @purpose  To Enable/Disable the sampler object operationally
*
* @param    pSampler      @b{(input)} reference to the sampler object
* @param    mode          @b{(input)} operational mode L7_ENABLE
*                                                      L7_DISABLE
*
* @returns  L7_SUCCESS    On success
* @returns  L7_FAILURE    falure in action
*
* @notes    if pSampler == L7_NULLPTR, we set the oeprational mode of 
*           all the sampler objects
*
* @end
*********************************************************************/
L7_RC_t sFlowSamplerOperModeSet(SFLOW_sampler_t *pSampler, L7_uint32 mode)
{
  SFLOW_receiver_t *pReceiver = L7_NULLPTR;
  SFLOW_sampler_t  *pTempSampler;
  L7_RC_t           rc = L7_SUCCESS;
  L7_uint32         val;

  pTempSampler = pSampler;
  if (!pTempSampler)
  {
    /* Get the first valid sampler object */
    pTempSampler = sFlowSamplerInstanceGet(L7_NULL, L7_NULL, L7_MATCH_GETNEXT);
  }

  while (pTempSampler)
  {
    pReceiver = &agent.receiver[pTempSampler->samplerCfg->sFlowFsReceiver - 1];
    if (mode == L7_ENABLE && pTempSampler->samplerCfg->sFlowFsPacketSamplingRate && 
        sFlowIsDataSourceReady(pTempSampler->key.dsIndex) == L7_TRUE)
    {
      if (sFlowIsReceiverReady(pReceiver) == L7_TRUE)
      {
        SFLOW_TRACE(SFLOW_DEBUG_INTERNAL, "Sampler dsIndex %u instance %u rate %u enabled!",
                    pTempSampler->key.dsIndex, pTempSampler->key.sFlowInstance, 
                    pTempSampler->samplerCfg->sFlowFsPacketSamplingRate);
        if (dtlsFlowIntfSamplingRateSet(SFLOW_DS_INDEX_TO_VAL(pTempSampler->key.dsIndex), 
                                        pTempSampler->samplerCfg->sFlowFsPacketSamplingRate) 
                                        != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SFLOW_COMPONENT_ID,
              "sFlowSamplerOperModeSet: Failed to set sampling rate in hardware for dsIndex %u."
              " Sampling rate set failed in driver", pTempSampler->key.dsIndex);
          if (rc != L7_SUCCESS)
          {
            rc = L7_FAILURE;
          }
        }
        else
        {
          /* Read the actual value set in the hardware and adjust the configured value */
          if (dtlsFlowIntfSamplingRateGet(SFLOW_DS_INDEX_TO_VAL(pTempSampler->key.dsIndex),
                                          &val) == L7_SUCCESS)
          {
            pTempSampler->samplerCfg->sFlowFsPacketSamplingRate = val;
          }
          else
          {
            LOG_MSG("sFlowSamplerOperModeSet: Failed to read sampling rate from hardware for dsIndex %u",
                  pTempSampler->key.dsIndex);
          }
        }
      }/* Receiver is ready */
    }
    else
    {
      SFLOW_TRACE(SFLOW_DEBUG_INTERNAL, "Sampler dsIndex %u instance %u disabled!",
                  pTempSampler->key.dsIndex, pTempSampler->key.sFlowInstance);
      if (dtlsFlowIntfSamplingRateSet(SFLOW_DS_INDEX_TO_VAL(pTempSampler->key.dsIndex),
                                      L7_NULL) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SFLOW_COMPONENT_ID,
              "sFlowSamplerOperModeSet: Failed to set sampling rate in hardware for dsIndex %u."
              " Sampling rate set failed in driver", pTempSampler->key.dsIndex);

        if (rc != L7_SUCCESS)
        {
          rc = L7_FAILURE;
        }
      }
    }

    if (pSampler)
    {
      /* opermode is to be set to the specified sampler object */
      break;
    }
    /* get next sampler object */
    pTempSampler = sFlowSamplerInstanceGet(pTempSampler->key.dsIndex, 
                                           pTempSampler->key.sFlowInstance, 
                                           L7_MATCH_GETNEXT);
  }/* Iterate thry all the sampler objects */
  return rc;
}
/*********************************************************************
* @purpose  To Enable/Disable the poller object operationally
*
* @param    pPoller       @b{(input)} reference to the poller object
* @param    mode          @b{(input)} operational mode L7_ENABLE
*                                                      L7_DISABLE
*
* @returns  L7_SUCCESS    On successful deletion
* @returns  L7_FAILURE    falure in action
*
* @notes    if pPoller == L7_NULLPTR, we set the oeprational mode of 
*           all the poller objects
*
* @end
*********************************************************************/
L7_RC_t sFlowPollerOperModeSet(SFLOW_poller_t *pPoller, L7_uint32 mode)
{
  SFLOW_receiver_t *pReceiver = L7_NULLPTR;
  SFLOW_poller_t   *pTempPoller;
  L7_RC_t           rc = L7_SUCCESS;

  pTempPoller = pPoller;
  if (!pTempPoller)
  {
    /* Get the first valid poller object */
    pTempPoller = sFlowPollerInstanceGet(L7_NULL, L7_NULL, L7_MATCH_GETNEXT);
  }

  while (pTempPoller)
  {
    pReceiver = &agent.receiver[pTempPoller->pollerCfg->sFlowCpReceiver - 1];
    if (mode == L7_ENABLE && pTempPoller->pollerCfg->sFlowCpInterval && 
        sFlowIsDataSourceReady(pTempPoller->key.dsIndex) == L7_TRUE)
    {
      if (sFlowIsReceiverReady(pReceiver) == L7_TRUE)
      {
        /* Start poller timer */
        SFLOW_TRACE(SFLOW_DEBUG_INTERNAL, "Poller dsIndex %u instance %u enabled!",
                  pTempPoller->key.dsIndex, pTempPoller->key.sFlowInstance);
        if (sFlowPollerTimerStart(pTempPoller, pTempPoller->pollerCfg->sFlowCpInterval) 
                                  != L7_SUCCESS)
        {
          LOG_MSG("sFlowPollerOperModeSet: Timer start failed for dsIndex %u instance %u",
                   pTempPoller->key.dsIndex, pTempPoller->key.sFlowInstance);
          if (rc == L7_SUCCESS)
          {
            rc = L7_FAILURE;
          }
        }
      }
    }
    else
    {
      SFLOW_TRACE(SFLOW_DEBUG_INTERNAL, "Poller dsIndex %u instance %u disabled!",
                  pTempPoller->key.dsIndex, pTempPoller->key.sFlowInstance);
      /* Stop poller timer */
      if (sFlowPollerTimerStop(pTempPoller) != L7_SUCCESS)
      {
        LOG_MSG("sFlowPollerOperModeSet: Timer stop failed for dsIndex %u instance %u",
                 pTempPoller->key.dsIndex, pTempPoller->key.sFlowInstance);
        if (rc == L7_SUCCESS)
        {
          rc = L7_FAILURE;
        }
      }
    }
    if (pPoller)
    {
      /* Mode set was called only for a specific poller */
      break;
    }
    /* get next valid poller object */
    pTempPoller = sFlowPollerInstanceGet(pTempPoller->key.dsIndex, 
                                         pTempPoller->key.sFlowInstance, 
                                         L7_MATCH_GETNEXT);
  } /* iterate thru all the available poller objects */
  return rc;
}
/*********************************************************************
* @purpose  Verify if the receiver object is ready to receive sFlow
*           agent samples
*
* @param    pReceiver    @b{(input)} reference to the receiver object
*
* @returns  L7_TRUE       If the receiver is ready to receive data
*                         from the sFlow agent
* @returns  L7_FALSE     otherwise
*
* @notes    A receiver to be redy - 1. sFlowRcvrTimeout > 0
*                                   2. sFlowRcvrAddress is not null
*                                   3. sFlowRcvrOwner is non empty
*          case 3 is taken care by the assumption that when timeout is
*          0, owner string is empty
*
* @end
*********************************************************************/
L7_BOOL sFlowIsReceiverReady(SFLOW_receiver_t *pReceiver)
{
  if (pReceiver->rcvrCfg->sFlowRcvrTimeout && 
      inetIsAddressZero(&pReceiver->rcvrCfg->sFlowRcvrAddress) == L7_FALSE)
  {
    return L7_TRUE;
  }
  else
  {
    return L7_FALSE;
  }
}
/*********************************************************************
* @purpose  To apply sampler randomizer seed value, priority value 
*           used while sampled packets are sent to CPU port
*
* @param    none 
*
* @returns  L7_SUCCESS    Successful applied in hardware
* @returns  L7_FAILURE    otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowGlobalCfgApply(void)
{
  if (dtlsFlowRandomSeedSet(FD_SFLOW_SAMPLE_RANDOM_SEED) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_SFLOW_COMPONENT_ID,
        "sFlowGlobalCfgApply: failed to apply sample random seed in hardware."
        " Driver returned error while setting random seed value");
    return L7_FAILURE;
  }
  
  if (dtlsFlowSamplePrioritySet(FD_SFLOW_CPU_SAMPLE_PRIORITY) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_SFLOW_COMPONENT_ID,
           "sFlowGlobalCfgApply: failed to apply sample priority in hardware."
           " Driver returned error while setting priority for sampled pdus");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To change sampling rate as there is congestion
*
* @param    dsIndex              @b{(input)} Data source Index
* @param    instance             @b{(input)} sFlow Instance
*
* @returns  L7_SUCCESS    Successful applied in hardware
* @returns  L7_FAILURE    otherwise
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t sFlowSamplingRateThrottle(L7_uint32 dsIndex, L7_uint32 instance)
{
  SFLOW_sampler_t    *pSampler = L7_NULLPTR;
  L7_uint32           samplerRate;

  if ((pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_EXACT)))
  {
    SFLOW_TRACE(SFLOW_DEBUG_INTERNAL,"Choke msg rcvd for dsIndex %d instance %d", dsIndex, instance);
    if (agent.congestionAlert == 0)
    {
      samplerRate = pSampler->samplerCfg->sFlowFsPacketSamplingRate * 2;
      if (samplerRate > L7_SFLOW_MAX_SAMPLING_RATE)
      {
        samplerRate = L7_SFLOW_MAX_SAMPLING_RATE;
      }
         
      if (samplerRate != pSampler->samplerCfg->sFlowFsPacketSamplingRate)
      {
        if (sFlowSamplerRateApply(dsIndex, instance, samplerRate) != L7_SUCCESS)
        {
          LOG_MSG("sFlowSamplingRateThrottle: Failed to modify sampling rate for dsIndex %d instance %d", 
                  dsIndex, instance);
        }
        else
        {
         SFLOW_TRACE(SFLOW_DEBUG_INTERNAL,"Sampling rate doubled for dsIndex %d instance %d", dsIndex, instance);
        }
        agent.congestionAlert = SFLOW_CONGESTION_ALERT_TIMEOUT;
      }
    }
  }
  return L7_SUCCESS;
}

