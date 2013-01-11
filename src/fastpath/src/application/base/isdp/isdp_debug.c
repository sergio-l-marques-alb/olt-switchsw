/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename isdp_debug.c
*
* @purpose isdp Debug File
*
* @component isdp
*
* @comments none
*
* @create 12/12/2007
*
* @author dgaryachy
*
* @end
*
**********************************************************************/

#include "isdp_include.h"
#include "isdp_api.h"
#include "isdp_debug_api.h"
#include "support_api.h"

extern isdpCfg_t          *isdpCfg;
extern isdpDebugCfg_t     isdpDebugCfg;
extern isdpIntfInfoData_t *isdpIntfTbl;
extern isdpStats_t        *isdpStats;

static L7_uint32 isdpDebugTraceCfg[ISDP_DBG_FLAG_LAST_ENTRY + 1];

#define ISDP_DEBUG_PACKET_RX_FORMAT "Pkt RX - Intf:%s, \
  Source_Mac:%02x:%02x:%02x:%02x:%02x:%02x, Version:%d"

#define ISDP_DEBUG_PACKET_TX_FORMAT "Pkt TX - Intf:%s, \
  Source_Mac:%02x:%02x:%02x:%02x:%02x:%02x, Version:%d"

#define ISDP_DEBUG_INTF_EVENT_FORMAT "Event - %s, ifName: %s, Nim event %s"

#define ISDP_DEBUG_EVENT_FORMAT "Event - %s"

static const char* isdpEventNames[ISDP_LAST_EVENT+1] =
{
    "UNKNOWN EVENT",
    "ISDP_PDU_EVENT",
    "ISDP_TIMER_EVENT",
    "ISDP_INTF_EVENT",
    "ISDP_CNFGR_EVENT",
    "ISDP_INTF_STARTUP_EVENT",
    "ISDP_LAST_EVENT"
};

L7_uint32 isdpDebugPacketTraceTxFlag = L7_DISABLE;
L7_uint32 isdpDebugPacketTraceRxFlag = L7_DISABLE;
L7_uint32 isdpDebugEventTraceFlag    = L7_DISABLE;

/*********************************************************************
*
* @purpose Trace isdp events
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   msg      @b{(input)} isdp message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t isdpDebugEventTrace(isdpMsg_t msg)
{
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  memset(ifName, 0, L7_NIM_IFNAME_SIZE + 1);

  if(isdpDebugEventTraceFlag == L7_DISABLE)
  {
    return L7_FAILURE;
  }

  if(msg.event < 0 || msg.event > ISDP_LAST_EVENT)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DebugGroup,
                   "%s: LINE %d: wrong event %d\n",
                   __FUNCTION__, __LINE__, msg.event);

    return L7_FAILURE;
  }

  if(msg.event == ISDP_INTF_EVENT)
  {
    nimGetIntfName(msg.intIfNum, L7_SYSNAME, ifName);

    ISDP_USER_TRACE(
        ISDP_DEBUG_INTF_EVENT_FORMAT,
        isdpEventNames[ISDP_INTF_EVENT],
        ifName,
        nimGetIntfEvent(msg.data.status.event));
  }
  else if(msg.event != ISDP_TIMER_EVENT)
  {
    ISDP_USER_TRACE(
        ISDP_DEBUG_EVENT_FORMAT,
        isdpEventNames[msg.event]);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Trace isdp packets transmitted
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   *pdu  @b{(input)} pointer to packet
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t isdpDebugPacketTxTrace(L7_uint32 intIfNum, isdpPacket_t *pdu)
{
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  if(pdu == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DebugGroup,
                   "%s: LINE %d: wrong parameters\n",
                   __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }

  memset(ifName, 0, L7_NIM_IFNAME_SIZE + 1);

  if(isdpDebugPacketTraceTxFlag == L7_DISABLE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DebugGroup,
                   "%s: LINE %d: tracing is disabled\n",
                   __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  ISDP_USER_TRACE(
      ISDP_DEBUG_PACKET_TX_FORMAT,
      ifName,
      pdu->ether.src.addr[0],
      pdu->ether.src.addr[1],
      pdu->ether.src.addr[2],
      pdu->ether.src.addr[3],
      pdu->ether.src.addr[4],
      pdu->ether.src.addr[5],
      pdu->isdp_header.version);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Trace isdp packets received
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   *pdu  @b{(input)} pointer to packet
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t isdpDebugPacketRxTrace(L7_uint32 intIfNum, isdpPacket_t *pdu)
{
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  if(pdu == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DebugGroup,
                   "%s: LINE %d: wrong parameters\n",
                   __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }

  memset(ifName, 0, L7_NIM_IFNAME_SIZE + 1);

  if(isdpDebugPacketTraceRxFlag == L7_DISABLE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DebugGroup,
                   "%s: LINE %d: tracing is disabled\n",
                   __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  ISDP_USER_TRACE(
      ISDP_DEBUG_PACKET_RX_FORMAT,
      ifName,
      pdu->ether.src.addr[0],
      pdu->ether.src.addr[1],
      pdu->ether.src.addr[2],
      pdu->ether.src.addr[3],
      pdu->ether.src.addr[4],
      pdu->ether.src.addr[5],
      pdu->isdp_header.version);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugCfgDump()
{
  L7_uint32         i         = 0;
  nimConfigID_t     cfgIdNull;
  isdpIntfCfgData_t *pCfg     = L7_NULLPTR;

  memset(&cfgIdNull, 0, sizeof(nimConfigID_t));

  sysapiPrintf("\n");
  sysapiPrintf("Isdp Configuration\n");
  sysapiPrintf("=====================\n\n");

  sysapiPrintf("Mode: %s\n", isdpCfg->cfg.globalCfgData.mode == L7_ENABLE ? "Enabled" : "Disabled");
  sysapiPrintf("Timer: %i\n", isdpCfg->cfg.globalCfgData.timer);
  sysapiPrintf("Holdtime: %i\n", isdpCfg->cfg.globalCfgData.holdTime);
  sysapiPrintf("V2Mode: %s\n", isdpCfg->cfg.globalCfgData.v2Mode == L7_ENABLE ? "Enabled" : "Disabled");

  sysapiPrintf("txTraceMode: %s\n", isdpDebugCfg.cfg.isdpDebugPacketTraceTxFlag   == L7_ENABLE ? "Enabled" : "Disabled");
  sysapiPrintf("rxTraceMode: %s\n", isdpDebugCfg.cfg.isdpDebugPacketTraceRxFlag   == L7_ENABLE ? "Enabled" : "Disabled");
  sysapiPrintf("eventsTraceMode: %s\n", isdpDebugCfg.cfg.isdpDebugEventTraceFlag == L7_ENABLE ? "Enabled" : "Disabled");

  sysapiPrintf("\nInterfaces Configured:\n");
  for (i = 1; i < L7_ISDP_INTF_MAX_COUNT; i++)
  {
    L7_uint32 intIfNum, extIfNum;

    if (NIM_CONFIG_ID_IS_EQUAL(&isdpCfg->cfg.intfCfgData[i].cfgId, &cfgIdNull))
    {
      continue;
    }

    if (nimIntIfFromConfigIDGet(&(isdpCfg->cfg.intfCfgData[i].cfgId),
                                &intIfNum) != L7_SUCCESS)
    {
      continue;
    }

    if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
      {
        sysapiPrintf("Interface: %d\n", extIfNum);
        sysapiPrintf("Mode: %s\n", pCfg->mode == L7_ENABLE ? "Enabled" : "Disabled");
      }
    }
  }
  sysapiPrintf("\n");

  sysapiPrintf("\n=====================\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current isdp interface status to serial port
*
* @param    L7_uint32    intIfNum   @b((input)) NIM internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugIntfDump(L7_uint32 intIfNum)
{
  L7_uint32         extIfNum = 0;
  isdpIntfCfgData_t *pCfg    = L7_NULLPTR;

  sysapiPrintf("\n");
  sysapiPrintf("Isdp Configuration\n");
  sysapiPrintf("=====================\n\n");

  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
    {
      sysapiPrintf("Interface: %d\n", extIfNum);
      sysapiPrintf("Link: %s\n", isdpIntfTbl[intIfNum].active ? "Up" : "Down");
    }
  }
  sysapiPrintf("\n");
  sysapiPrintf("\n=====================\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the current 802.1AB statistics to serial port
*
* @param    L7_uint32   intIfNum   @b((input)) NIM internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugStatsIntfDump(L7_uint32 intIfNum)
{
  L7_uint32         extIfNum = 0;
  isdpIntfCfgData_t *pCfg    = L7_NULLPTR;

  sysapiPrintf("\n");
  sysapiPrintf("Isdp Statistics\n");
  sysapiPrintf("=====================\n\n");

  sysapiPrintf("Neighbors Table Inserts last time: %i\n", isdpStats->lastChangeTime);
  sysapiPrintf("Neighbors Table Drops: %i\n", isdpStats->insertEntryDrops);

  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
    {
      sysapiPrintf("Interface Statistics\n");
      sysapiPrintf("-------------------------------------------------\n");

      sysapiPrintf("Interface: %d\n", extIfNum);

      sysapiPrintf("Tx Frames Total: %i\n", isdpStats->intfStats[intIfNum].pduTx);
      sysapiPrintf("Tx v1: %i\n", isdpStats->intfStats[intIfNum].v1PduTx);
      sysapiPrintf("Tx v2: %i\n", isdpStats->intfStats[intIfNum].v2PduTx);
      sysapiPrintf("Tx failures: %i\n", isdpStats->intfStats[intIfNum].failureTx);

      sysapiPrintf("Rx Frames Total: %i\n", isdpStats->intfStats[intIfNum].pduRx);
      sysapiPrintf("Rx Frames with invalid format: %i\n", isdpStats->intfStats[intIfNum].invalidFormatRx);
      sysapiPrintf("Rx Frames CheckSum Errors: %i\n", isdpStats->intfStats[intIfNum].chkSumErrorRx);
      sysapiPrintf("Rx with bad header: %i\n", isdpStats->intfStats[intIfNum].badHeaderRx);
      sysapiPrintf("Rx v1: %i\n", isdpStats->intfStats[intIfNum].v1PduRx);
      sysapiPrintf("Rx v2: %i\n", isdpStats->intfStats[intIfNum].v2PduRx);


    sysapiPrintf("-------------------------------------------------\n\n");
    }
  }

  sysapiPrintf("\n");
  sysapiPrintf("\n=====================\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check a ISDP event trace
*
* @param    event_flag  @b{(input)} one of ISDP_TRACE_* flags allowed in config
*
* @returns  L7_BOOL
*
* @notes
*
* @end
*********************************************************************/
static L7_BOOL isdpDebugIsTraceEvent(L7_uint32 event_flag)
{
    if(event_flag < ISDP_DBG_FLAG_FIRST_ENTRY ||
        event_flag > ISDP_DBG_FLAG_LAST_ENTRY)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "%s: Event out of range", __FUNCTION__);
      return L7_FALSE;
    }

    return L7_TRUE;
}

/*********************************************************************
* @purpose  Record a ISDP event trace if trace configuration permits
*
* @param    event_flag  @b{(input)} one of ISDP_TRACE_* flags allowed in config
* @param    format      @b{(input)} format string
* @param    ...         @b{(input)} additional arguments (per format string)
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugTrace(L7_uint32 event_flag, L7_char8 * format, ...)
{
    va_list ap;

    if(isdpDebugIsTraceEvent(event_flag) == L7_FALSE)
    {
      return;
    }

    if (isdpDebugTraceCfg[event_flag] == L7_TRUE)
    {
      va_start(ap, format);
      (void)vprintf(format, ap);
      va_end(ap);
    }
}

/*********************************************************************
* @purpose  Turn on specified group of traces
*
* @param    event_flag  @b{(input)} one of isdp trace flags
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugTraceSet(L7_uint32 event_flag)
{
  L7_uint32 i = ISDP_DBG_FLAG_FIRST_ENTRY + 1;

  if (event_flag == 0xFF)
  {
    for(; i < ISDP_DBG_FLAG_LAST_ENTRY; i++)
    {
      isdpDebugTraceCfg[i] = L7_TRUE;
    }
    return;
  }

  if(isdpDebugIsTraceEvent(event_flag) == L7_TRUE)
  {
    isdpDebugTraceCfg[event_flag] = L7_TRUE;
  }
}

/*********************************************************************
* @purpose  Turn off specified group of traces
*
* @param    event_flag  @b{(input)} one of isdp trace flags
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugTraceClear(L7_uint32 event_flag)
{
  if (event_flag == 0xFF)
  {
    memset(isdpDebugTraceCfg, 0, sizeof(isdpDebugTraceCfg));
    return;
  }

  if(isdpDebugIsTraceEvent(event_flag) == L7_TRUE)
  {
    isdpDebugTraceCfg[event_flag] = L7_FALSE;
  }
}

/*********************************************************************
* @purpose  Return name of trace flag
*
* @param    event_flag  @b{(input)} one of isdp trace flags
*
* @returns  name of trace flag
*
* @notes    none
*
* @end
*********************************************************************/
static L7_char8* isdpDebugTraceFlagStringGet(L7_uint32 event_flag)
{
  switch (event_flag)
  {
  case ISDP_DBG_FLAG_FIRST_ENTRY:   return "ISDP_DBG_FLAG_FIRST_ENTRY";

  case ISDP_DBG_FLAG_ConfigGroup:   return "ISDP_DBG_FLAG_ConfigGroup";
  case ISDP_DBG_FLAG_DatabaseGroup: return "ISDP_DBG_FLAG_DatabaseGroup";
  case ISDP_DBG_FLAG_NimGroup:      return "ISDP_DBG_FLAG_NimGroup";
  case ISDP_DBG_FLAG_PduGroup:      return "ISDP_DBG_FLAG_PduGroup";
  case ISDP_DBG_FLAG_ControlGroup:  return "ISDP_DBG_FLAG_ControlGroup";
  case ISDP_DBG_FLAG_CnfgrGroup:    return "ISDP_DBG_FLAG_CnfgrGroup";
  case ISDP_DBG_FLAG_ApiGroup:      return "ISDP_DBG_FLAG_ApiGroup";

  case ISDP_DBG_FLAG_LAST_ENTRY:    return "ISDP_DBG_FLAG_LAST_ENTRY";
  default:return "Unknown trace flag";
  }
}

/*********************************************************************
* @purpose  Dump trace flags
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugTraceDump(void)
{
  L7_uint32 flag = ISDP_DBG_FLAG_FIRST_ENTRY + 1;

  for(; flag < ISDP_DBG_FLAG_LAST_ENTRY; flag++)
  {
    sysapiPrintf(" %s = %d\n", isdpDebugTraceFlagStringGet(flag),
        isdpDebugTraceCfg[flag]);
  }
}

/*********************************************************************
* @purpose  Turn on all traces
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugTraceLinkerAssist(void)
{
    isdpDebugTraceDump();
    isdpDebugTraceSet(0xFF);
    return;
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
void isdpDebugRegister(void)
{
  supportDebugDescr_t supportDebugDescr;

  memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

  supportDebugDescr.componentId = L7_ISDP_COMPONENT_ID;

  /* Register User Control Parms */
  supportDebugDescr.userControl.notifySave     = isdpDebugSave;
  supportDebugDescr.userControl.hasDataChanged = isdpDebugHasDataChanged;
  supportDebugDescr.userControl.clearFunc      = isdpDebugRestore;

  /* Register Internal Parms */

  /* Submit registration */

  supportDebugRegister(supportDebugDescr);
}

/*********************************************************************
* @purpose  Read and apply the debug config
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugCfgRead(void)
{
  /* reset the debug flags*/
  memset((void*)&isdpDebugCfg, 0 ,sizeof(isdpDebugCfg_t));

  sysapiSupportCfgFileGet(L7_ISDP_COMPONENT_ID, ISDP_DEBUG_CFG_FILENAME,
                          (L7_char8 *)&isdpDebugCfg, sizeof(isdpDebugCfg_t),
                          &isdpDebugCfg.checkSum, ISDP_DEBUG_CFG_VER_CURRENT,
                          isdpDebugBuildDefaultConfigData, L7_NULL);

  isdpDebugCfg.hdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Turns on/off the displaying of isdp packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugPacketTraceTxFlagSet(L7_uint32 flag)
{
  isdpDebugPacketTraceTxFlag = flag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
   */
  isdpDebugCfg.hdr.dataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Turns on/off the displaying of isdp packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugPacketTraceRxFlagSet(L7_uint32 flag)
{
  isdpDebugPacketTraceRxFlag = flag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
   */
  isdpDebugCfg.hdr.dataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Turns on/off the displaying of isdp event debug info
*
* @param    flag         new value of the Event Debug flag
*
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugEventTraceFlagSet(L7_uint32 flag)
{
  isdpDebugEventTraceFlag = flag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
   */
  isdpDebugCfg.hdr.dataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Get the current status of displaying isdp packet debug info
*
* @param    none
*
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 isdpDebugPacketTraceRxFlagGet(void)
{
  return isdpDebugPacketTraceRxFlag;
}

/*********************************************************************
* @purpose  Get the current status of displaying isdp packet debug info
*
* @param    none
*
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 isdpDebugPacketTraceTxFlagGet(void)
{
  return isdpDebugPacketTraceTxFlag;
}

/*********************************************************************
* @purpose  Get the current status of displaying isdp event debug info
*
* @param    none
*
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 isdpDebugEventTraceFlagGet(void)
{
  return isdpDebugEventTraceFlag;
}

/*********************************************************************
* @purpose  Save configuration settings for isdp trace data
*
* @param    void
*
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t isdpDebugPacketTraceFlagSave()
{
  isdpDebugCfg.cfg.isdpDebugPacketTraceTxFlag = isdpDebugPacketTraceTxFlag;
  isdpDebugCfg.cfg.isdpDebugPacketTraceRxFlag = isdpDebugPacketTraceRxFlag;
  isdpDebugCfg.cfg.isdpDebugEventTraceFlag    = isdpDebugEventTraceFlag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Copy the trace settings to the debug config file
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugCfgUpdate(void)
{
  isdpDebugPacketTraceFlagSave();
}

