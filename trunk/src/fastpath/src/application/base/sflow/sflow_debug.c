/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007-2006
*
**********************************************************************
* @filename  sFlow_debug.c
*
* @purpose    Contains definitions to debug APIs
*
* @component sFlow
*
* @comments
*
* @create    29-Nov-2007
*
* @author    drajendra
*
* @end
*
**********************************************************************/
#include "datatypes.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "support_api.h"
#include "buff_api.h"

#include "sflow_mib.h"
#include "sflow_ctrl.h"
#include "sflow_debug.h"
#include "sflow_db.h"
#include "sflow_cfg.h"

#define SFLOW_DEBUG_PACKET_TX_FORMAT "Pkt Tx - DestIP: %s Payload Length: %d SequenceNo: %d\n"
extern SFLOW_agent_t agent;
/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
static L7_uchar8 sFlowDebugFlags = 0x00;
static L7_BOOL sFlowIsDebugEnabled = L7_FALSE;
/*********************************************************************
* @purpose  Print the current sFlow config values to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowCfgDump(void)
{
  L7_uint32 idx;
  L7_uchar8 buff[128];

  sysapiPrintf("\nsFlow Receiver Table\n");
  sysapiPrintf("--------------------\n");
  sysapiPrintf("rcvrIdx  time        size  port   ver  Address                 Type  Owner String\n");
  sysapiPrintf("=======  ==========  ====  =====  ===  ======================= ====  ============\n");
  for (idx = 0; idx < SFLOW_MAX_RECEIVERS; idx++)
  {
   sysapiPrintf("%7d  %10d  %4d  %5d  %3d  %-24s   %4d  %s\n",
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrIndex,
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrTimeout,
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrMaxDatagramSize,
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrPort,
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrDatagramVersion,
                inetAddrPrint(&agent.sFlowCfg->receiverGroup[idx].sFlowRcvrAddress, buff),
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrAddressType,
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrOwner);
  }

  sysapiPrintf("\nsFlow Sampler Table\n");
  sysapiPrintf("-------------------\n");
  sysapiPrintf("dsIndex  instance  rcvr  rate    hdrSize\n");
  sysapiPrintf("=======  ========  ==== ======  =========\n");
  for (idx = 0; idx < SFLOW_MAX_SAMPLERS; idx++)
  {
   sysapiPrintf("%5d  %8d  %4d   %5d       %4d\n",
                agent.sFlowCfg->samplerGroup[idx].sFlowFsDataSource,
                agent.sFlowCfg->samplerGroup[idx].sFlowFsInstance,
                agent.sFlowCfg->samplerGroup[idx].sFlowFsReceiver,
                agent.sFlowCfg->samplerGroup[idx].sFlowFsPacketSamplingRate,
                agent.sFlowCfg->samplerGroup[idx].sFlowFsMaximumHeaderSize);
  }

  sysapiPrintf("\nsFlow Poller Table\n");
  sysapiPrintf("------------------\n");
  sysapiPrintf("dsIndex  instance  rcvr intrvl\n");
  sysapiPrintf("=======  ========  ==== ======\n");
  for (idx = 0; idx < SFLOW_MAX_POLLERS; idx++)
  {
   sysapiPrintf("%5d  %8d  %4d   %4d\n",
                agent.sFlowCfg->pollerGroup[idx].sFlowCpDataSource,
                agent.sFlowCfg->pollerGroup[idx].sFlowCpInstance,
                agent.sFlowCfg->pollerGroup[idx].sFlowCpReceiver,
                agent.sFlowCfg->pollerGroup[idx].sFlowCpInterval);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose   Displays all operation info for receiveres, pollers,
*            sampler objects
*
* @param     none
*
* @returns   none
*
* @comments  Used for debug
*
* @end
*
*********************************************************************/
void sFlowInfoShow()
{
  L7_uint32 idx, dsIndex,  instance;
  L7_uchar8 buff[128];
  SFLOW_poller_t *pPoller;
  SFLOW_sampler_t *pSampler;

  sysapiPrintf("\nsFlow Receiver Table\n");
  sysapiPrintf("--------------------\n");
  sysapiPrintf("rcvrIdx  time        size  sqno        ver  Address                 type  Owner String\n");
  sysapiPrintf("=======  ==========  ====  ==========  ===  ======================= ====  ============\n");
  for (idx = 0; idx < SFLOW_MAX_RECEIVERS; idx++)
  {
   sysapiPrintf("%7d  %10d  %4d  %10d  %3d  %-23s   %4d  %s\n",
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrIndex,
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrTimeout,
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrMaxDatagramSize,
                agent.receiver[idx].sFlowDatagram.sequence_number,
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrDatagramVersion,
                inetAddrPrint(&agent.sFlowCfg->receiverGroup[idx].sFlowRcvrAddress, buff),
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrAddressType,
                agent.sFlowCfg->receiverGroup[idx].sFlowRcvrOwner);
  }

  sysapiPrintf("\nsFlow Sampler Objects\n");
  sysapiPrintf("---------------------\n");
  sysapiPrintf("dsIndex  instance  rcvr  rate   total       drops       sqno\n");
  sysapiPrintf("=======  ========  ====  =====  ==========  ==========  ==========\n");
  dsIndex = instance = 0;
  while((pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_GETNEXT)))
  {
    dsIndex  = pSampler->key.dsIndex;
    instance = pSampler->key.sFlowInstance;
    sysapiPrintf("%5d  %8d  %4d  %5d  %10d  %10d  %10d\n",
                 pSampler->key.dsIndex,
                 pSampler->key.sFlowInstance,
                 pSampler->samplerCfg->sFlowFsReceiver,
                 pSampler->samplerCfg->sFlowFsPacketSamplingRate,
                 pSampler->sample_pool,
                 pSampler->drops,
                 pSampler->flowSampleSeqNo);
    pSampler = sFlowSamplerInstanceGet(dsIndex, instance, L7_MATCH_GETNEXT);
  }
  sysapiPrintf("Number of Samplers : %u\n", agent.numActiveSamplers);

  sysapiPrintf("\nsFlow Poller Objects\n");
  sysapiPrintf("--------------------\n");
  sysapiPrintf("dsIndex  instance  rcvr  intrvl      sqno\n");
  sysapiPrintf("=======  ========  ====  ==========  ==========\n");
  dsIndex = instance = 0;
  while((pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_GETNEXT)))
  {
    dsIndex  = pPoller->key.dsIndex;
    instance = pPoller->key.sFlowInstance;
    sysapiPrintf("%5d  %8d  %4d  %10d  %10d\n",
                 pPoller->key.dsIndex,
                 pPoller->key.sFlowInstance,
                 pPoller->pollerCfg->sFlowCpReceiver,
                 pPoller->pollerCfg->sFlowCpInterval,
                 pPoller->counterSampleSeqNo);
    pPoller = sFlowPollerInstanceGet(dsIndex, instance, L7_MATCH_GETNEXT);
  }
  sysapiPrintf("Number of Pollers : %u\n", agent.numActivePollers);
}
/*********************************************************************
* @purpose  Enable Debug Tracing in sFlow
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully enabled.
* @returns  L7_FAILURE  if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
sFlowDebugTraceEnable (void)
{
  if (sFlowIsDebugEnabled == L7_TRUE)
  {
    SFLOW_DEBUG_PRINTF ("SFLOW Debug Tracing is already Enabled.\n");
    return (L7_FAILURE);
  }

  sFlowIsDebugEnabled = L7_TRUE;
  SFLOW_DEBUG_PRINTF ("SFLOW Debug Tracing is Enabled.\n");

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Disable Debug Tracing in SFLOW
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully disabled.
* @returns  L7_FAILURE  if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
sFlowDebugTraceDisable (void)
{
  if (sFlowIsDebugEnabled != L7_TRUE)
  {
    SFLOW_DEBUG_PRINTF ("SFLOW Debug Tracing is already Disabled.\n");
    return (L7_FAILURE);
  }

  sFlowIsDebugEnabled = L7_FALSE;
  SFLOW_DEBUG_PRINTF ("SFLOW Debug Tracing is Disabled.\n");

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Shows the usage of the sFlow Debug Trace Utility
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void
sFlowDebugTraceHelp (void)
{
  SFLOW_DEBUG_PRINTF ("sFlowDebugTraceEnable()  - Enable Debug Tracing in sFlow.\n");
  SFLOW_DEBUG_PRINTF ("sFlowDebugTraceDisable() - Disable Debug Tracing in sFlow.\n");
  SFLOW_DEBUG_PRINTF ("sFlowDebugTraceAllFlagsReset()  - Disable Debug Tracing for All Events.\n");
  SFLOW_DEBUG_PRINTF ("sFlowDebugTraceAllFlagsSet()    - Enable Debug Tracing for All Events.\n");
  SFLOW_DEBUG_PRINTF ("sFlowDebugTraceFlagsShow()      - Show the status of Trace Flags.\n");
  SFLOW_DEBUG_PRINTF ("sFlowDebugTraceFlagsSet(flag)   - Enable Debug Tracing for the Specified Flag.\n");
  SFLOW_DEBUG_PRINTF ("sFlowDebugTraceFlagsReset(flag) - Disable Debug Tracing for the Specified Flag.\n");
  SFLOW_DEBUG_PRINTF ("     Flags  ....\n");
  SFLOW_DEBUG_PRINTF ("       0   -> To Trace SFLOW timer Events.\n");
  SFLOW_DEBUG_PRINTF ("       1   -> To Trace SFLOW Sampler Events.\n");
  SFLOW_DEBUG_PRINTF ("       2   -> To Trace SFLOW internal operations.\n");
  SFLOW_DEBUG_PRINTF ("       3   -> To Trace SFLOW task events.\n");

  return;
}
/*********************************************************************
* @purpose  Clear all trace flags
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void sFlowDebugTraceAllFlagsReset(void)
{
  sFlowDebugFlags = 0;
}
/*********************************************************************
* @purpose  Set all trace flags
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void sFlowDebugTraceAllFlagsSet(void)
{
  sFlowDebugFlags = 0xFF;
}

/*********************************************************************
* @purpose  Shows the Enabled/Disabled Trace flags
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void sFlowDebugTraceFlagsShow(void)
{
    SFLOW_DEBUG_PRINTF ("  Debug Trace Enabled                      ->   %s\n", (sFlowIsDebugEnabled) ? "Y" : "N");
    SFLOW_DEBUG_PRINTF ("  Trace SFLOW Timer Events                 ->   %s\n", (sFlowDebugFlags & 1) ? "Y" : "N");
    SFLOW_DEBUG_PRINTF ("  Trace SFLOW Sampler Events               ->   %s\n", (sFlowDebugFlags & 2) ? "Y" : "N");
    SFLOW_DEBUG_PRINTF ("  Trace the Protocol Control Packets Path  ->   %s\n", (sFlowDebugFlags & 4) ? "Y" : "N");
    SFLOW_DEBUG_PRINTF ("  Trace the SFLOW Internal Events Path     ->   %s\n", (sFlowDebugFlags & 8) ? "Y" : "N");
}
/*********************************************************************
* @purpose  Set a particulat tracelevel
*
* @param    flag   {(input)} Trace Level
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void sFlowDebugTraceFlagsSet(L7_uint32 flag)
{
  if (flag < SFLOW_DEBUG_FLAGS_BITS)
  {
    sFlowDebugFlags |= (1 << flag);
  }
}
/*********************************************************************
* @purpose  Reset a particulat tracelevel
*
* @param    flag   {(input)} Trace Level
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void sFlowDebugTraceFlagsReset(L7_uint32 flag)
{
  if (flag < SFLOW_DEBUG_FLAGS_BITS)
  {
    sFlowDebugFlags &= ~(1 << flag);
  }
}
/*********************************************************************
*
* @purpose
*
* @param    flag   {(input)} Trace Level
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL
sFlowDebugTraceFlagCheck (L7_uint32 traceFlag)
{
  if (sFlowIsDebugEnabled != L7_TRUE)
  {
   return L7_FALSE;
  }

  if (sFlowDebugFlags & (1 << traceFlag))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}
/*********************************************************************
* @purpose Trace sFlow packets transmitted
*
* @param   rcvrIp         @b{(input)} destination receiver ip address
* @param   payloadLen     @b{(input)} sFlow datagram length
* @param   dgramSqN       @b{(input)} sFlow datagram sequence number
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void sFlowDebugPacketTxTrace(L7_inet_addr_t *rcvrIp, L7_uint32 payloadLen,
                             L7_uint32 dgramSqNo)
{
  L7_uchar8   addrBuff[IPV6_DISP_ADDR_LEN];

  if (agent.sFlowDebugPacketTraceTxFlag != L7_TRUE)
  {
    return;
  }

  if (inetAddrPrint(rcvrIp, addrBuff) == L7_NULLPTR)
  {
    memset(addrBuff, 0x00, IPV6_DISP_ADDR_LEN);
  }

  SFLOW_USER_TRACE_TX(SFLOW_DEBUG_PACKET_TX_FORMAT, addrBuff,payloadLen, dgramSqNo);
}
/************************************************************************
* @purpose  Get the current status of displaying sFlow packet debug info
*
* @param    transmitFlag  @b{(output)}  Tx Debug Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
************************************************************************/
L7_RC_t sFlowDebugPacketTraceFlagGet(L7_BOOL *transmitFlag)
{
  *transmitFlag = agent.sFlowDebugPacketTraceTxFlag;
  return L7_SUCCESS;
}
/*****************************************************************************
* @purpose  Turns on/off the displaying of sFlow packet debug info of
*           a sflow agent
*
* @param    transmitFlag  @b{(input)}  Tx Debug Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t sFlowDebugPacketTraceFlagSet(L7_BOOL transmitFlag)
{
  agent.sFlowDebugPacketTraceTxFlag = transmitFlag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
  */

  agent.sFlowDebugCfg->hdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Save configuration settings for sFlow trace data of a
*           sFlow agent
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowDebugPacketTraceFlagSave(void)
{
  agent.sFlowDebugCfg->cfg.sFlowDebugPacketTraceTxFlag =
  agent.sFlowDebugPacketTraceTxFlag;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Copy the trace settings to the debug config file
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void sFlowpDebugCfgUpdate(void)
{
  (void)sFlowDebugPacketTraceFlagSave();
}
/*********************************************************************
* @purpose  Read and apply the debug config of a sFlow agent
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void sFlowDebugCfgRead(void)
{
  /* reset the debug cfg data */
  memset((void*)agent.sFlowDebugCfg, 0x00 ,sizeof(sFlowDebugCfg_t));

    (void)sysapiSupportCfgFileGet(L7_SFLOW_COMPONENT_ID,
                                  SFLOW_DEBUG_CFG_FILENAME,
                                 (L7_char8 *)agent.sFlowDebugCfg,
                                 (L7_uint32)sizeof(sFlowDebugCfg_t),
                                &(agent.sFlowDebugCfg->checkSum),
                                 SFLOW_DEBUG_CFG_VER_CURRENT,
                                 sFlowDebugBuildDefaultConfigData, L7_NULL);
  agent.sFlowDebugCfg->hdr.dataChanged = L7_FALSE;
}
/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void sFlowDebugRegister(void)
{
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_SFLOW_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave     = sFlowDebugSave;
    supportDebugDescr.userControl.hasDataChanged = sFlowDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc      = sFlowDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */
    (void) supportDebugRegister(supportDebugDescr);
}
/*********************************************************************
* @purpose  Restores sFlow debug configuration of sFlow agent
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
L7_RC_t sFlowDebugRestore(void)
{
  sFlowDebugBuildDefaultConfigData(SFLOW_DEBUG_CFG_VER_CURRENT);
  agent.sFlowDebugCfg->hdr.dataChanged = L7_TRUE;
  if (sFlowApplyDebugConfigData() != L7_SUCCESS)
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SFLOW_COMPONENT_ID,
             "sFlowDebugRestore: sFlow Debug config restore failed");
   }
   return L7_SUCCESS;
}

/* Debug routines */
void sFlowDebugAddReceiver(L7_uint32 rcvrIdx, L7_uint32 timeout)
{
  L7_uint32 val;

  osapiInetPton(L7_AF_INET, "10.131.12.178", (L7_uchar8 *)&val);
  if (rcvrIdx < SFLOW_MAX_RECEIVERS)
  {
    agent.sFlowCfg->receiverGroup[rcvrIdx - 1].sFlowRcvrTimeout = timeout;
    sprintf(agent.sFlowCfg->receiverGroup[rcvrIdx - 1].sFlowRcvrOwner,
            "%s%d","receiver",rcvrIdx);
    inetAddressSet(L7_AF_INET, &val,&agent.sFlowCfg->receiverGroup[rcvrIdx - 1].sFlowRcvrAddress);
    agent.sFlowCfg->receiverGroup[rcvrIdx - 1].sFlowRcvrAddressType = L7_AF_INET;
  }
}

void sFlowDebugAddSampler(L7_uint32 rcvrIdx, L7_uint32 dsIndex, L7_uint32 rate)
{
  if (rcvrIdx < SFLOW_MAX_RECEIVERS)
  {
    sFlowSamplerReceiverApply(dsIndex, 1, rcvrIdx);
    sFlowSamplerRateApply(dsIndex, 1, rate);
  }
}

void sFlowDebugAddPoller(L7_uint32 rcvrIdx, L7_uint32 dsIndex, L7_uint32 intrvl)
{
  if (rcvrIdx < SFLOW_MAX_RECEIVERS)
  {
    sFlowPollerReceiverApply(dsIndex, 1, rcvrIdx);
    sFlowPollerIntervalApply(dsIndex, 1, intrvl);
  }
}

/* Reduce the number of packet buffers so that sample drops can be seen.
   Use flag = 0 to restore it to default value */
void sFlowDebugCongestion(L7_uint32 flag)
{
  L7_uint32 count = 10;
  if (flag == 0)
  {
    count = SFLOW_PACKETQ_MSG_COUNT;
  }
  /* Buffer pool */
  if (bufferPoolSizeSet(agent.sflowBufferPool, count) != L7_SUCCESS)
  {
    sysapiPrintf("\nError allocating buffers");
    return;
  }
}

