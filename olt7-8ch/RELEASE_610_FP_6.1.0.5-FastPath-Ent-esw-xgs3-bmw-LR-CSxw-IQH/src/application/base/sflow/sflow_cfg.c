/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_cfg.c
*
* @purpose   SFlow component configuration handling
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
#include "l7utils_inet_addr_api.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "support_api.h"

#include "sflow_debug.h"
#include "sflow_cfg.h"
#include "sflow_mib.h"
#include "sflow_ctrl.h"
#include "sflow_debug_api.h"

extern SFLOW_agent_t agent;

/*********************************************************************
* @purpose  Saves sFlow configuration  to NVStore
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t sFlowSave()
{
  L7_RC_t     rc = L7_SUCCESS;

  if (sFlowHasDataChanged() == L7_TRUE)
  {
      agent.sFlowCfg->cfgHdr.dataChanged = L7_FALSE;
      agent.sFlowCfg->checkSum =
                             nvStoreCrc32((L7_char8 *)agent.sFlowCfg,
                             (sizeof (SFLOW_cfgData_t) -
                              sizeof (agent.sFlowCfg->checkSum)));
      if (sysapiCfgFileWrite(L7_SFLOW_COMPONENT_ID,
                             agent.sFlowCfg->cfgHdr.filename,
                             (L7_char8 *)agent.sFlowCfg,
                             sizeof(SFLOW_cfgData_t)) != L7_SUCCESS)
      {
        LOG_MSG("sflowSave: Error on call to sysapiCfgFileWrite file %s\n",
                agent.sFlowCfg->cfgHdr.filename);
        rc = L7_FAILURE;
      }
  }

  return (rc);
}
/*********************************************************************
* @purpose  Checks if sFlow user config data has changed
*
* @param    none
*
* @returns  L7_TRUE    
* @returns  L7_FALSE
*
* @comments none
*           
*
* @end
*********************************************************************/
L7_BOOL sFlowHasDataChanged()
{
  return agent.sFlowCfg->cfgHdr.dataChanged;
}
void sFlowResetDataChanged(void)
{
   agent.sFlowCfg->cfgHdr.dataChanged = L7_FALSE;
   return;
}
/*********************************************************************
* @purpose  Apply Configuration Data for a specified sflowAgent
*
* @param    pSflowAgent       @b{(input)}  sFlow agent object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Receiver configuration is stored in the cfg structure
*
* @end
*********************************************************************/
L7_RC_t sFlowApplyConfigData()
{
  L7_uint32 idx;

  /* Apply global configuration - Randomizer seed, CPU priority to 
     hardware */
  if (sFlowGlobalCfgApply() != L7_SUCCESS)
  {
    LOG_MSG("sFlowApplyConfigData: Failed to apply global config");
    return L7_FAILURE;
  }

  /* Apply poller configuration */
  for (idx = 0; idx < SFLOW_MAX_POLLERS; idx++)
  {
    if (agent.sFlowCfg->pollerGroup[idx].sFlowCpDataSource == L7_NULL)
    {
      continue;
    }

    if (agent.sFlowCfg->pollerGroup[idx].sFlowCpReceiver)
    {
      /* create poller object */
      sFlowPollerReceiverApply(agent.sFlowCfg->pollerGroup[idx].sFlowCpDataSource,
                               agent.sFlowCfg->pollerGroup[idx].sFlowCpInstance,
                               agent.sFlowCfg->pollerGroup[idx].sFlowCpReceiver);
    }
  }

  /* Apply sampler configuration */
  for (idx = 0; idx < SFLOW_MAX_SAMPLERS; idx++)
  {
    if (agent.sFlowCfg->samplerGroup[idx].sFlowFsDataSource == L7_NULL)
    {
      continue;
    }

    if (agent.sFlowCfg->samplerGroup[idx].sFlowFsReceiver)
    {
      /* create sampler object */
      sFlowSamplerReceiverApply(agent.sFlowCfg->samplerGroup[idx].sFlowFsDataSource,
                               agent.sFlowCfg->samplerGroup[idx].sFlowFsInstance,
                               agent.sFlowCfg->samplerGroup[idx].sFlowFsReceiver);
    }
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Restores sFlow component user configuration to factory
*           defaults
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t sFlowRestoreProcess()
{
 L7_uint32 idx;
/* Default Receiver config */
  for (idx = 0; idx < SFLOW_MAX_RECEIVERS; idx++)
  {
    agent.sFlowCfg->receiverGroup[idx].sFlowRcvrTimeout = L7_NULL;
  }
  /* Delete all the poller and sampler objects */
  sFlowSamplerPollerInstancesCleanup();

  /* Build default configuration */
  sFlowBuildDefaultConfigData(SFLOW_CFG_VER_CURRENT);

  /* Apply configuration */
  sFlowApplyConfigData();
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Build default sFlow config data
*
* @param    ver       @b{(input)}  Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void sFlowBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 idx;
/* Default Agent config */
  agent.agentGroup.sFlowVersion = FD_SFLOW_PROTOCOL_VERSION;

/* Default Receiver config */
   for (idx = 1; idx <= SFLOW_MAX_RECEIVERS; idx++)
   {
     sFlowBuildDefaultReceiverConfigData(idx);
   }
/* Default Sampler config  */
   for (idx = 0; idx < SFLOW_MAX_SAMPLERS; idx++)
   {
      sFlowBuildDefaultSamplerConfigData(&agent.sFlowCfg->samplerGroup[idx]);
   }

/* Default poller config   */
   for (idx = 0; idx < SFLOW_MAX_POLLERS; idx++)
   {
      sFlowBuildDefaultPollerConfigData(&agent.sFlowCfg->pollerGroup[idx]);
   }

  /* sFlow config file name */
  strcpy(agent.sFlowCfg->cfgHdr.filename, SFLOW_CFG_FILENAME);
  agent.sFlowCfg->cfgHdr.version     = ver;
  agent.sFlowCfg->cfgHdr.componentID = L7_SFLOW_COMPONENT_ID;
  agent.sFlowCfg->cfgHdr.type        = L7_CFG_DATA;
  agent.sFlowCfg->cfgHdr.length      = sizeof(SFLOW_cfgData_t);
  agent.sFlowCfg->cfgHdr.dataChanged = L7_FALSE;
}
/*********************************************************************
* @purpose  Build default sFlow receiver config data
*
* @param    rcvrIndx   @b{(input)}  receiver index value = 
                                    (receiver arrayIndex + 1)
*
* @returns  void
*
* @comments rcvrIndx ranges from 1 to SFLOW_MAX_RECEIVERS
*
* @end
*********************************************************************/
void sFlowBuildDefaultReceiverConfigData(L7_uint32 rcvrIndx)
{
  agent.sFlowCfg->receiverGroup[rcvrIndx - 1].sFlowRcvrIndex = rcvrIndx;
  memset(agent.sFlowCfg->receiverGroup[rcvrIndx - 1].sFlowRcvrOwner, 0x00, L7_SFLOW_OWNER_STRING_LEN);
  agent.sFlowCfg->receiverGroup[rcvrIndx - 1].sFlowRcvrTimeout = FD_SFLOW_RCVR_TIMEOUT;
  agent.sFlowCfg->receiverGroup[rcvrIndx - 1].sFlowRcvrMaxDatagramSize = FD_SFLOW_MAX_DGRAM_SIZE;
  agent.sFlowCfg->receiverGroup[rcvrIndx - 1].sFlowRcvrAddressType = FD_SFLOW_RCVR_ADDR_TYPE;
  inetAddressZeroSet(FD_SFLOW_RCVR_ADDR_TYPE, &agent.sFlowCfg->receiverGroup[rcvrIndx - 1].sFlowRcvrAddress);
  agent.sFlowCfg->receiverGroup[rcvrIndx - 1].sFlowRcvrPort = FD_SFLOW_RCVR_PORT;
  agent.sFlowCfg->receiverGroup[rcvrIndx - 1].sFlowRcvrDatagramVersion = FD_SFLOW_PROTOCOL_VERSION;
#ifdef L7_SNMP_PACKAGE
  agent.sFlowCfg->receiverGroup[rcvrIndx - 1].sFlowRcvrSnmpMode = FD_SFLOW_RCVR_SNMP_MODE;
#else
  agent.sFlowCfg->receiverGroup[rcvrIndx - 1].sFlowRcvrSnmpMode = L7_DISABLE;
#endif

  /* Initialize the attached operational entry */
  memset(&agent.receiver[rcvrIndx - 1], 0x00, sizeof(SFLOW_receiver_t));
  agent.receiver[rcvrIndx - 1].rcvrCfg    = &agent.sFlowCfg->receiverGroup[rcvrIndx - 1];
  agent.receiver[rcvrIndx - 1].ptrPduBuff = agent.receiver[rcvrIndx - 1].pduBuff + 
                                            SFLOW_SAMPLE_DATAGRAM_IP6_LEN;
}
/*********************************************************************
* @purpose  Build default sFlow sampler config data
*
* @param    pSamplerCfg   @b{(input)}  sampler object
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void sFlowBuildDefaultSamplerConfigData(SFLOW_sampler_mib_t *pSamplerCfg)
{
   pSamplerCfg->sFlowFsDataSource = L7_NULL;
   pSamplerCfg->sFlowFsInstance   = FD_SFLOW_INSTANCE;
   pSamplerCfg->sFlowFsReceiver   = L7_NULL;
   pSamplerCfg->sFlowFsPacketSamplingRate = FD_SFLOW_SAMPLING_RATE;
   pSamplerCfg->sFlowFsMaximumHeaderSize  = FD_SFLOW_DEFAULT_HEADER_SIZE;
}
/*********************************************************************
* @purpose  Build default sFlow poller config data
*
* @param    pPollerCfg   @b{(input)}  poller object
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void sFlowBuildDefaultPollerConfigData(SFLOW_poller_mib_t *pPollerCfg)
{
  pPollerCfg->sFlowCpDataSource = L7_NULL;
  pPollerCfg->sFlowCpInstance   = FD_SFLOW_INSTANCE;
  pPollerCfg->sFlowCpReceiver   = L7_NULL;
  pPollerCfg->sFlowCpInterval   = FD_SFLOW_POLL_INTERVAL;
}
/*********************************************************************
* @purpose  Build default sFlow Debug config data
*
* @param    ver     @b{(input)}  Software version of debug Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void sFlowDebugBuildDefaultConfigData(L7_uint32 ver)
{
  /* setup debug file header */
  agent.sFlowDebugCfg->hdr.version = ver;
  agent.sFlowDebugCfg->hdr.componentID = L7_SFLOW_COMPONENT_ID;
  agent.sFlowDebugCfg->hdr.type = L7_CFG_DATA;
  agent.sFlowDebugCfg->hdr.length = (L7_uint32)sizeof(sFlowDebugCfg_t);
  strcpy((L7_char8 *)agent.sFlowDebugCfg->hdr.filename, SFLOW_DEBUG_CFG_FILENAME);
  agent.sFlowDebugCfg->hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&(agent.sFlowDebugCfg->cfg), 0, sizeof(agent.sFlowDebugCfg->cfg));
}
/*********************************************************************
* @purpose  Apply sFLow Debug config data for a specified
*           sFlow instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t sFlowApplyDebugConfigData(void)
{
  return sFlowDebugPacketTraceFlagSet(agent.sFlowDebugCfg->cfg.sFlowDebugPacketTraceTxFlag);
}
/*********************************************************************
* @purpose  Saves sFLow debug configuration for sFlow agent
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
L7_RC_t sFlowDebugSave(void)
{
   /* Store the config file */
  if (agent.sFlowDebugCfg->hdr.dataChanged == L7_TRUE)
  {
     /* Copy the operational states into the config file */
     sFlowDebugPacketTraceFlagSave();

    agent.sFlowDebugCfg->hdr.dataChanged = L7_FALSE;
    agent.sFlowDebugCfg->checkSum =
                           nvStoreCrc32((L7_uchar8 *)agent.sFlowDebugCfg,
                            (L7_uint32)(sizeof(sFlowDebugCfg_t) -
                                       sizeof(agent.sFlowDebugCfg->checkSum)));
    /* call save NVStore routine */
    if (sysapiSupportCfgFileWrite(L7_SFLOW_COMPONENT_ID,
                                       agent.sFlowDebugCfg->hdr.filename,
                                      (L7_char8 *)agent.sFlowDebugCfg,
                                      (L7_uint32)sizeof(sFlowDebugCfg_t))
                                    == L7_ERROR)
    {
      LOG_MSG("Error on call to sysapiSupportCfgFileWrite routine on config file %s\n",
              agent.sFlowDebugCfg->hdr.filename);
    }
  }/* config change check  */

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Checks if sFLow debug config data has changed for agent
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL sFlowDebugHasDataChanged(void)
{
  L7_BOOL     rc = L7_FALSE;

  if (agent.sFlowDebugCfg->hdr.dataChanged == L7_TRUE)
  {
    rc = L7_TRUE;
  }
  return rc;
}

