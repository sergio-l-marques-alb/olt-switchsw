/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename nim_debug.c
*
* @purpose NIM debug routines
*
* @component nim
*
* @comments none
*
* @create 08/30/2000
*
* @author mbaucom
*
* @end
*
**********************************************************************/


#include "l7_common.h"
#include "sysapi.h"
#include "nim_data.h"
#include "platform_config.h"
#include "log.h"
#include "l7utils_api.h"
#include "nim_events.h"
#include "nim_util.h"
#include "nim_startup.h"
#include "cnfgr_sid.h"

#include <string.h>

static  SYSAPI_HPC_PORT_DESCRIPTOR_t portData =
{
  L7_IANA_GIGABIT_ETHERNET,
  L7_PORTCTRL_PORTSPEED_FULL_10GSX,
  (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |
   L7_PHY_CAP_PORTSPEED_HALF_10   |
   L7_PHY_CAP_PORTSPEED_FULL_10   |
   L7_PHY_CAP_PORTSPEED_HALF_100  |
   L7_PHY_CAP_PORTSPEED_FULL_100  |
   L7_PHY_CAP_PORTSPEED_HALF_1000 |
   L7_PHY_CAP_PORTSPEED_FULL_1000 |
   L7_PHY_CAP_PORTSPEED_FULL_2500 |   /* PTin added (2.5G) */
   L7_PHY_CAP_PORTSPEED_FULL_10G),
  L7_MTRJ
};


extern L7_RC_t   nimCmgrNewIntfChangeCallback(L7_uint32 unit, L7_uint32 slot, L7_uint32 port,
                                              L7_uint32 cardType,L7_PORT_EVENTS_t event,
                                              SYSAPI_HPC_PORT_DESCRIPTOR_t *portData);

extern void nimEventDtlDebugCallback(NIM_NOTIFY_CB_INFO_t retVal);
extern L7_COMPONENT_IDS_t nimStartupEventInProcess(void);

void nimDebugCfgPort(nimCfgPort_t *configPort);
void nimDebugPortIntIfNum(nimIntf_t *port);
void nimDebugPortIntIfShow(L7_uint32 intIfNum);
void nimDebugPortUspShow(L7_uint32 unit,L7_uint32 slot,L7_uint32 port);
void nimBuildTestConfigData();
void nimConfigTestSet(L7_uint32 intIfNum);

void nimDebugDataDumpShow()
{
  L7_uint32 component;
  L7_uint32 count = 0;
  L7_uint32 maxIntf;
  L7_uint32 intIfNum;
  L7_RC_t   rc;


  /*sysapiPrintf("semaId = %lx\n",nimCtlBlk_g->semaId);*/

  switch (nimCtlBlk_g->nimPhaseStatus)
  {
    case L7_CNFGR_STATE_P1:
      sysapiPrintf("phase is L7_CNFGR_STATE_P1\n");
      break;
    case L7_CNFGR_STATE_P2:
      sysapiPrintf("phase is L7_CNFGR_STATE_P2\n");
      break;
    case L7_CNFGR_STATE_P3:
      sysapiPrintf("phase is L7_CNFGR_STATE_P3\n");
      break;
    case L7_CNFGR_STATE_WMU:
      sysapiPrintf("phase is L7_CNFGR_STATE_WMU\n");
      break;
    case L7_CNFGR_STATE_E:
      sysapiPrintf("phase is L7_CNFGR_STATE_E\n");
      break;
    default:
      sysapiPrintf("phase is Not in P1,P2,P3,WMU, or E\n");
      break;
  }

  for (component = 0;component < L7_LAST_COMPONENT_ID;component++)
  {
    if (nimCtlBlk_g->nimNotifyList[component].notify_intf_change != L7_NULLPTR) count++;
  }

  sysapiPrintf("Number Components registered = %d \n",count);

  sysapiPrintf("Task ID of nimTask is %lx\n",nimCtlBlk_g->taskId);

  maxIntf = platIntfTotalMaxCountGet() + 1;
  maxIntf = (maxIntf / 32) + 1;
  for (count = 0; count < NIM_INTF_INDICES ;count++)
  {
    sysapiPrintf("LinkStateMask[%d] = %0.2x\n",count,nimCtlBlk_g->linkStateMask.value[count]);
  }

  for (count = 0; count < NIM_INTF_INDICES ;count++)
  {
    sysapiPrintf("forwardStateMask[%d] = %0.2x\n",count,nimCtlBlk_g->forwardStateMask.value[count]);
  }

  sysapiPrintf("Msg Queue is %lx\n",nimCtlBlk_g->nimMsgQueue);

  for (count = 0; count < nimCtlBlk_g->maxNumOfUnits;count++)
  {
    sysapiPrintf("The number of interfaces in unit %d = %d\n",count,nimCtlBlk_g->nimNumberOfPortsPerUnit[count]);

  }

  sysapiPrintf("Max units=%d , slots/unit=%d , phys ports/slot=%d\n",nimCtlBlk_g->maxNumOfUnits,
               nimCtlBlk_g->maxNumOfSlotsPerUnit,nimCtlBlk_g->maxNumOfPhysicalPortsPerSlot);

  sysapiPrintf("NIM printLog is set to %s\n", (nimCtlBlk_g->nimPrintLog > 0) ? "TRUE" : "FALSE");


  for (count = 0;count < NIM_INTF_INDICES;count++)
  {
    sysapiPrintf("createdMask[%d] = %0.2x\n",count,nimCtlBlk_g->createdMask.value[count]);
  }

  sysapiPrintf("\n");

  for (count = 0;count < NIM_INTF_INDICES;count++)
  {
    sysapiPrintf("presentMask[%d] = %0.2x\n",count,nimCtlBlk_g->presentMask.value[count]);
  }

  sysapiPrintf("\n");

  for (count = 0;count < NIM_INTF_INDICES;count++)
  {
    sysapiPrintf("physicalIntfMask[%d] = %0.2x\n",count,nimCtlBlk_g->physicalIntfMask.value[count]);
  }

  sysapiPrintf("\n");

  for (count = 0;count < NIM_INTF_INDICES;count++)
  {
    sysapiPrintf("cpuIntfMask[%d] = %0.2x\n",count,nimCtlBlk_g->cpuIntfMask.value[count]);
  }

  sysapiPrintf("\n");

  for (count = 0;count < NIM_INTF_INDICES;count++)
  {
    sysapiPrintf("lagIntfMask[%d] = %0.2x\n",count,nimCtlBlk_g->lagIntfMask.value[count]);
  }

  sysapiPrintf("\n");

  for (count = 0;count < NIM_INTF_INDICES;count++)
  {
    sysapiPrintf("vlanIntfMask[%d] = %0.2x\n",count,nimCtlBlk_g->vlanIntfMask.value[count]);
  }

  sysapiPrintf("\n");

  for (count = 0;count < NIM_INTF_INDICES;count++)
  {
    sysapiPrintf("l2tnnlIntfMask[%d] = %0.2x\n",count,nimCtlBlk_g->l2tnnlIntfMask.value[count]);
  }


  sysapiPrintf("********* Config data **********\n");


  sysapiPrintf("The Config Mask BITMAP of used indices\n");
  for (count = 0; count < NIM_INTF_INDICES ; count++)
  {
    sysapiPrintf("Bitmap[%d]= %0.2x\n",count,nimCtlBlk_g->nimConfigData->configMaskBitmap.value[count]);
  }


  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {

      sysapiPrintf("*********** INTIFNUM  %d **************\n", intIfNum);
      nimDebugCfgPort(&nimCtlBlk_g->nimPorts[intIfNum].configPort);
      sysapiPrintf("\n");
      /* Iterate */
      rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  } /* rc == L7_SUCCESS */


  return;

}

void nimDebugMsgQueueShow()
{
  L7_int32 numMsgs;
  L7_RC_t rc;
  extern L7_uint32 nimSidMsgCountGet();

  rc = osapiMsgQueueGetNumMsgs(nimCtlBlk_g->nimMsgQueue, &numMsgs);
  if (rc == L7_SUCCESS)
    sysapiPrintf("NIM Messages in queue: %d, max: %d\n", numMsgs, nimSidMsgCountGet());
  else
    sysapiPrintf("osapiMsgQueueGetNumMsgs failed\n");
}

int nimTestLogMsg (int val)
{
 NIM_LOG_MSG ("Val = %d\n", val);
 return 0;
}

/***
** Test the NIM_LOG_MSG routine.
***/
int nim_log_msg_test (int msg_size)
{
  unsigned char test_msg [LOG_MSG_MAX_MSG_SIZE * 2];
  int i;

  if (msg_size > sizeof (test_msg))
  {
    msg_size = sizeof (test_msg);
  }
  if (msg_size < 2)
  {
    msg_size = 0;
  }

  memset (test_msg, 0, sizeof (test_msg));
  for (i = 0; i < (msg_size - 2); i++)
  {
    test_msg[i] = 'a' + (char) (i % 26);
  }

  NIM_LOG_MSG ("%s", test_msg);

  return 0;
}

int nimTestLogErr (int val)
{
 NIM_LOG_ERROR ("Val = %d\n", val);
 return 0;
}

int nimTestTraceMsg (int val)
{
 NIM_TRACE_MSG ("Val = %d\n", val);
 return 0;
}

/***
** Test the NIM_LOG_MSG routine.
***/
int nim_trace_msg_test (int msg_size)
{
  unsigned char test_msg [LOG_MSG_MAX_MSG_SIZE * 2];
  int i;

  if (msg_size > sizeof (test_msg))
  {
    msg_size = sizeof (test_msg);
  }
  if (msg_size < 2)
  {
    msg_size = 0;
  }

  memset (test_msg, 0, sizeof (test_msg));
  for (i = 0; i < (msg_size - 2); i++)
  {
    test_msg[i] = 'a' + (char) (i % 26);
  }

  NIM_TRACE_MSG ("%s", test_msg);

  return 0;
}

void nimLoggingSet(L7_uint32 value)
{
  nimCtlBlk_g->nimPrintLog = value;

  return;
}

void nim_log_msg_fmt(L7_char8 * file, L7_ulong32 line, L7_char8 * msg)
{
  L7_int32 len;
  L7_uchar8 log_buf[LOG_MSG_MAX_MSG_SIZE];

  l7utilsFilenameStrip(&file);

  bzero(log_buf,sizeof(log_buf));

  osapiSnprintf (log_buf, sizeof (log_buf), "%s:%d : ", file, (L7_int32)line);

  len = (L7_int32)strlen(log_buf);

  if (len < LOG_MSG_MAX_MSG_SIZE)
  {
    strncat(log_buf,msg,(size_t)(LOG_MSG_MAX_MSG_SIZE - len));
  }

  sysapiPrintf(log_buf);
}

L7_RC_t nimDebugPortCreate(L7_uint32 unit,L7_uint32 slot,L7_uint32 port)
{
  L7_RC_t   rc;

  rc = nimCmgrNewIntfChangeCallback(unit, slot, port, 0,L7_CREATE,
                                 &portData);
  return(rc);
}

L7_uchar8 *
nimDebugIntfTypeToString(L7_uint32 sysIntfType)
{
  L7_uchar8 *pName = "UNKNOWN";

  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
      pName = "L7_PHYSICAL_INTF";
      break;
    case L7_STACK_INTF:
      pName = "L7_STACK_INTF";
      break;
    case L7_CPU_INTF:
      pName = "L7_CPU_INTF";
      break;
    case L7_MAX_PHYS_INTF_VALUE:
      pName = "L7_MAX_PHYS_INTF_VALUE";
      break;
    case L7_LAG_INTF:
      pName = "L7_LAG_INTF";
      break;
    case L7_UNUSED1_INTF:
      pName = "L7_UNUSED1_INTF";
      break;
    case L7_LOGICAL_VLAN_INTF:
      pName = "L7_LOGICAL_VLAN_INTF";
      break;
    case L7_LOOPBACK_INTF:
      pName = "L7_LOOPBACK_INTF";
      break;
    case L7_TUNNEL_INTF:
      pName = "L7_TUNNEL_INTF";
      break;
    case L7_WIRELESS_INTF:
      pName = "L7_WIRELESS_INTF";
      break;
    case L7_CAPWAP_TUNNEL_INTF:
      pName = "L7_CAPWAP_TUNNEL_INTF";
      break;
    case L7_MAX_INTF_TYPE_VALUE:
      pName = "L7_MAX_INTF_TYPE_VALUE";
      break;
    default:
      break;
  }

  return pName;
}

void nimDebugConfigIdPrint(nimConfigID_t *pCfgID)
{
  sysapiPrintf("configInterfaceId = type:%s, ",
                nimDebugIntfTypeToString(pCfgID->type));
  switch (pCfgID->type)
  {
    case L7_LOGICAL_VLAN_INTF:
      sysapiPrintf("vlanId:%u\n", pCfgID->configSpecifier.vlanId);
      break;
    case L7_LAG_INTF:
      sysapiPrintf("dot3adIntf:%u\n", pCfgID->configSpecifier.dot3adIntf);
      break;
    case L7_LOOPBACK_INTF:
      sysapiPrintf("loopbackId:%u\n", pCfgID->configSpecifier.loopbackId);
      break;
    case L7_TUNNEL_INTF:
      sysapiPrintf("tunnelId:%u\n", pCfgID->configSpecifier.tunnelId);
      break;
    case L7_WIRELESS_INTF:
      sysapiPrintf("wirelessNetId:%u\n", pCfgID->configSpecifier.wirelessNetId);
      break;
    case L7_CAPWAP_TUNNEL_INTF:
      sysapiPrintf("l2tunnelId:%u\n", pCfgID->configSpecifier.l2tunnelId);
      break;
    default:
      sysapiPrintf("usp:%d.%d.%d\n", pCfgID->configSpecifier.usp.unit,
                   pCfgID->configSpecifier.usp.slot,
                   pCfgID->configSpecifier.usp.port);
      break;
  }
}

void nimDebugCfgPort(nimCfgPort_t *configPort)
{
  L7_uint32 i;
  nimConfigID_t cfgID;
  nimUSP_t usp;
  if (configPort != L7_NULL)
  {

    sysapiPrintf("*********** Config Port **************\n");

    NIM_CONFIG_ID_COPY(&cfgID,&configPort->configInterfaceId);
    nimDebugConfigIdPrint(&cfgID);

    memset( (void *)&usp,0, sizeof(nimUSP_t));
    if ( nimUspFromConfigIDGet(&cfgID, &usp) == L7_SUCCESS)
    {
      sysapiPrintf("usp = %d.%d.%d\n", usp.unit, usp.slot, usp.port);
    }

    sysapiPrintf("configMaskOffset = %d\n",configPort->configIdMaskOffset);

    sysapiPrintf("LAA = ");
    for (i = 0;i < 5 ;i++)
    {
      sysapiPrintf("%0.2x:",configPort->cfgInfo.LAAMacAddr.addr[i]);
    }

    sysapiPrintf("%0.2x\n",configPort->cfgInfo.LAAMacAddr.addr[5]);

    sysapiPrintf("addrType = %s\n",(configPort->cfgInfo.addrType == L7_SYSMAC_BIA)?"L7_SYSMAC_BIA":"L7_SYSMAC_LAA");

    sysapiPrintf("ifAlias = %s\n", configPort->cfgInfo.ifAlias);

    sysapiPrintf("nameType = %s\n",(configPort->cfgInfo.nameType == L7_ALIASNAME)?"L7_ALIASNAME":"L7_SYSNAME");

    switch (configPort->cfgInfo.ifSpeed)
    {
      case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
        sysapiPrintf("ifSpeed = L7_PORTCTRL_PORTSPEED_AUTO_NEG\n");
        break;
      case L7_PORTCTRL_PORTSPEED_HALF_100TX:
        sysapiPrintf("ifSpeed =  L7_PORTCTRL_PORTSPEED_HALF_100TX\n");
        break;
      case L7_PORTCTRL_PORTSPEED_FULL_100TX:
        sysapiPrintf("ifSpeed =  L7_PORTCTRL_PORTSPEED_FULL_100TX\n");
        break;
      case L7_PORTCTRL_PORTSPEED_HALF_10T:
        sysapiPrintf("ifSpeed =  L7_PORTCTRL_PORTSPEED_HALF_10T\n");
        break;
      case L7_PORTCTRL_PORTSPEED_FULL_10T:
        sysapiPrintf("ifSpeed =  L7_PORTCTRL_PORTSPEED_FULL_10T\n");
        break;
      case L7_PORTCTRL_PORTSPEED_FULL_100FX:
        sysapiPrintf("ifSpeed =  L7_PORTCTRL_PORTSPEED_FULL_100FX\n");
        break;
      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
        sysapiPrintf("ifSpeed =  L7_PORTCTRL_PORTSPEED_FULL_1000SX\n");
        break;
      /* PTin added (2.5G) */
      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
        sysapiPrintf("ifSpeed =  L7_PORTCTRL_PORTSPEED_FULL_2500FX\n");
        break;
      /* PTin end */
      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
        sysapiPrintf("ifSpeed =  L7_PORTCTRL_PORTSPEED_FULL_10GSX\n");
        break;
      case L7_PORTCTRL_PORTSPEED_AAL5_155:
        sysapiPrintf("ifSpeed =  L7_PORTCTRL_PORTSPEED_AAL5_155\n");
        break;
      case L7_PORTCTRL_PORTSPEED_LAG:
        sysapiPrintf("ifSpeed =  L7_PORTCTRL_PORTSPEED_LAG\n");
        break;
      default:
        break;
    }

    sysapiPrintf("negoCapabilities = %s\n",(configPort->cfgInfo.negoCapabilities == 0)?"DISABLED":"ENABLED");

  sysapiPrintf("mgmtAdminState = %s\n",(configPort->cfgInfo.mgmtAdminState == L7_ENABLE)?"ENABLE":"DISABLED");
    sysapiPrintf("adminState = %s\n",(configPort->cfgInfo.adminState == L7_ENABLE)?"ENABLE":"DISABLED");

    sysapiPrintf("trapState = %s\n",(configPort->cfgInfo.trapState == L7_ENABLE)?"ENABLE":"DISABLED");

    sysapiPrintf("MTU = %d\n", configPort->cfgInfo.ipMtu);

    sysapiPrintf("Encapsulation type = %d\n", configPort->cfgInfo.encapsType);
    sysapiPrintf("Ptr to this nimCfgPort_t = 0x%0.8x\n",(L7_uint32)configPort);

  }
}

void nimDebugPortIntIfNum(nimIntf_t *port)
{
  L7_uint32 i;
  nimConfigID_t cfgID;


  if (port != L7_NULL)
  {
    NIM_CONFIG_ID_COPY(&cfgID,&port->configInterfaceId);
    sysapiPrintf("Present = %d \n",port->present);

    sysapiPrintf("intfNo = %d\n",port->intfNo);

    sysapiPrintf("runTimeMaskId = %d \n",port->runTimeMaskId);

    nimDebugConfigIdPrint(&cfgID);

    sysapiPrintf("sysIntfType = %s\n",
                 nimDebugIntfTypeToString(port->sysIntfType));

    sysapiPrintf("ifIndex = %d \n", port->ifIndex);

    sysapiPrintf("ifDescr = %s \n",port->operInfo.ifDescr);

    switch (port->operInfo.ianaType)
    {
      case L7_IANA_OTHER:
        sysapiPrintf("ifType =  L7_IANA_OTHER\n");
        break;
      case L7_IANA_ETHERNET:
        sysapiPrintf("ifType =  L7_IANA_ETHERNET\n");
        break;
      case L7_IANA_AAL5:
        sysapiPrintf("ifType =  L7_IANA_AAL5\n");
        break;
      case L7_IANA_PROP_VIRTUAL:
        sysapiPrintf("ifType =  L7_IANA_PROP_VIRTUAL\n");
        break;
      case L7_IANA_FAST_ETHERNET:
        sysapiPrintf("ifType =  L7_IANA_FAST_ETHERNET\n");
        break;
      case L7_IANA_FAST_ETHERNET_FX:
        sysapiPrintf("ifType =  L7_IANA_FAST_ETHERNET_FX\n");
        break;
      case L7_IANA_GIGABIT_ETHERNET:
        sysapiPrintf("ifType =  L7_IANA_GIGABIT_ETHERNET\n");
        break;
      /* PTin added (2.5G) */
      case L7_IANA_2G5_ETHERNET:
        sysapiPrintf("ifType =  L7_IANA_2G5_ETHERNET\n");
        break;
      /* PTin end */
      case L7_IANA_10G_ETHERNET:
        sysapiPrintf("ifType =  L7_IANA_10G_ETHERNET\n");
        break;
      case L7_IANA_L2_VLAN:
        sysapiPrintf("ifType =  L7_IANA_L2_VLAN\n");
        break;
      case L7_IANA_LAG_DESC:
        sysapiPrintf("ifType =  L7_IANA_LAG_DESC\n");
        break;
      case L7_IANA_LOGICAL_DESC:
        sysapiPrintf("ifType =  L7_IANA_LOGICAL_DESC\n");
        break;
      case L7_IANA_SOFTWARE_LOOPBACK:
        sysapiPrintf("ifType =  L7_IANA_SOFTWARE_LOOPBACK\n");
        break;
      case L7_IANA_TUNNEL:
        sysapiPrintf("ifType =  L7_IANA_TUNNEL\n");
        break;
      case L7_IANA_CAPWAP_TUNNEL:
        sysapiPrintf("ifType =  L7_IANA_CAPWAP_TUNNEL\n");
        break;
      default:
        sysapiPrintf("ifType =  UNKNOWN %d \n", port->operInfo.ianaType);
        break;
    }

    sysapiPrintf("BIA = ");
    for (i = 0;i < 5 ;i++)
    {
      sysapiPrintf("%0.2x:",port->operInfo.macAddr.addr[i]);
    }

    sysapiPrintf("%0.2x\n",port->operInfo.macAddr.addr[5]);

    sysapiPrintf("resetTime = %d\n",port->resetTime);

    sysapiPrintf("linkChangeTime = %d\n",port->linkChangeTime);

    sysapiPrintf("ifName = %s\n", port->operInfo.ifName);

    sysapiPrintf("USP = %d.%d.%d\n",port->usp.unit,port->usp.slot,port->usp.port);

    sysapiPrintf("PHY CAP = ");
    if (port->operInfo.phyCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG)
      sysapiPrintf(" L7_PHY_CAP_PORTSPEED_AUTO_NEG |\n");
    if (port->operInfo.phyCapability & L7_PHY_CAP_PORTSPEED_HALF_10)
      sysapiPrintf(" L7_PHY_CAP_PORTSPEED_HALF_10 |\n");
    if (port->operInfo.phyCapability & L7_PHY_CAP_PORTSPEED_FULL_10)
      sysapiPrintf(" L7_PHY_CAP_PORTSPEED_FULL_10 |\n");
    if (port->operInfo.phyCapability & L7_PHY_CAP_PORTSPEED_HALF_100)
      sysapiPrintf(" L7_PHY_CAP_PORTSPEED_HALF_100 |\n");
    if (port->operInfo.phyCapability & L7_PHY_CAP_PORTSPEED_FULL_100)
      sysapiPrintf(" L7_PHY_CAP_PORTSPEED_FULL_100 |\n");
    if (port->operInfo.phyCapability & L7_PHY_CAP_PORTSPEED_HALF_1000)
      sysapiPrintf(" L7_PHY_CAP_PORTSPEED_HALF_1000 |\n");
    if (port->operInfo.phyCapability & L7_PHY_CAP_PORTSPEED_FULL_1000)
      sysapiPrintf(" L7_PHY_CAP_PORTSPEED_FULL_1000 |\n");
    /* PTin added (2.5G) */
    if (port->operInfo.phyCapability & L7_PHY_CAP_PORTSPEED_FULL_2500)
      sysapiPrintf(" L7_PHY_CAP_PORTSPEED_FULL_2500 |\n");
    /* PTin end */
    if (port->operInfo.phyCapability & L7_PHY_CAP_PORTSPEED_FULL_10G)
      sysapiPrintf(" L7_PHY_CAP_PORTSPEED_FULL_10G |\n");
    if (port->operInfo.phyCapability & L7_PHY_CAP_PORTSPEED_SFP)
      sysapiPrintf(" L7_PHY_CAP_PORTSPEED_SFP \n");
    if (port->operInfo.phyCapability & L7_PHY_CAP_POE_PSE)
      sysapiPrintf(" L7_PHY_CAP_POE_PSE \n");
    if (port->operInfo.phyCapability & L7_PHY_CAP_POE_PD)
      sysapiPrintf(" L7_PHY_CAP_POE_PD \n");

    switch (port->operInfo.connectorType)
    {
      case L7_CONNECTOR_NONE:
        sysapiPrintf("connectorType = L7_CONNECTOR_NONE\n");
        break;
      case L7_RJ45:
        sysapiPrintf("connectorType = L7_RJ45\n");
        break;
      case L7_MTRJ:
        sysapiPrintf("connectorType = L7_MTRJ\n");
        break;
      case L7_SCDUP:
        sysapiPrintf("connectorType = L7_SCDUP\n");
        break;
      case L7_XAUI:
        sysapiPrintf("connectorType = L7_XAUI\n");
        break;
      default:
        sysapiPrintf("connectorType = UNKNOWN\n");
        break;
    }

    sysapiPrintf("maxFrameSize = %d\n",port->operInfo.frameSize.largestFrameSize);

    switch(port->intfState)
    {
      case L7_INTF_UNINITIALIZED:
      sysapiPrintf("Intf State = L7_INTF_UNINITIALIZED\n");
      break;
      case L7_INTF_CREATING:
      sysapiPrintf("Intf State = L7_INTF_CREATING\n");
      break;
      case L7_INTF_CREATED:
      sysapiPrintf("Intf State = L7_INTF_CREATED\n");
      break;
      case L7_INTF_ATTACHING:
      sysapiPrintf("Intf State = L7_INTF_ATTACHING\n");
      break;
      case L7_INTF_ATTACHED:
      sysapiPrintf("Intf State = L7_INTF_ATTACHED\n");
      break;
      case L7_INTF_DETACHING:
      sysapiPrintf("Intf State = L7_INTF_DETACHING\n");
      break;
      case L7_INTF_DELETING:
      sysapiPrintf("Intf State = L7_INTF_DELETING\n");
      break;
      default:
        sysapiPrintf("Intf State = Unknown\n");
    }

    sysapiPrintf("Ptr for the nimIntf_t = 0x%0.8x\n",(L7_uint32)port);
  }

}

void nimDebugPortIntIfShow(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  nimIntf_t port;
  L7_BOOL linkState;

  IS_INTIFNUM_PRESENT(intIfNum,rc);

  if (rc == L7_SUCCESS)
  {
    port = nimCtlBlk_g->nimPorts[intIfNum];
    nimDebugPortIntIfNum(&port);
    nimDebugCfgPort(&port.configPort);
    dtlIntfLinkStateGet(intIfNum,&linkState);
    sysapiPrintf("Driver believes link is %s\n",(linkState)?"up":"down");
  }


}

void nimDebugPortUspShow(L7_uint32 unit,L7_uint32 slot,L7_uint32 port)
{
  nimUSP_t usp;
  L7_uint32 intIfNum;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;

  if (nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
    nimDebugPortIntIfShow(intIfNum);

}

void nimDebugEventGenerator(char event[],L7_uint32 intIfNum)
{
  L7_uint32 eventNum = L7_LAST_PORT_EVENT;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t pHandle;


  if (strcmp(event,"L7_ATTACH") == 0)
  {
    eventNum = L7_ATTACH;
  }
  else if(strcmp(event,"L7_DETACH") == 0)
  {
    eventNum = L7_DETACH;
  }
  else if(strcmp(event,"L7_DELETE") == 0)
  {
    eventNum = L7_DELETE;
  }

  eventInfo.component = L7_NIM_COMPONENT_ID;
  eventInfo.intIfNum  = intIfNum;
  eventInfo.pCbFunc   = nimEventDtlDebugCallback;
  eventInfo.event     = eventNum;

  /* prefered method of event generation for all events */
  if (nimEventIntfNotify(eventInfo,&pHandle) != L7_SUCCESS)
  {
    sysapiPrintf("Error in the call to generate event\n");
  }
}

void nimDebugIfIndexNextGet(L7_uint32 extIfNum)
{
  L7_uint32 temp;
  L7_uint32 intIfNum;

  nimIfIndexNextGet(extIfNum,&temp,&intIfNum);

  sysapiPrintf("NExt is %d\n",temp);

  return;

}

void nimDebugUspIntfGet(L7_uint32 unit,L7_uint32 slot, L7_uint32 port)
{
  L7_uint32 intIfNum;
  nimUSP_t usp;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;

  if (nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
      sysapiPrintf("%d.%d.%d exists as %d\n",unit,slot,port,intIfNum);

  if (nimNextIntfOnSlotGet(&usp,&intIfNum ) == L7_SUCCESS)
    sysapiPrintf("Next On slot %d.%d.%d exists as %d\n",unit,slot,port,intIfNum);
  else
    sysapiPrintf("Nothing left on slot after %d.%d.%d exists as %d\n",unit,slot,port,intIfNum);

  if ( nimNextIntfOnUnitGet(&usp,&intIfNum ) == L7_SUCCESS)
    sysapiPrintf("Next On unit %d.%d.%d exists as %d\n",unit,slot,port,intIfNum);
  else
    sysapiPrintf("Nothing left on unit after %d.%d.%d exists as %d\n",unit,slot,port,intIfNum);

}




/*********************************************************************
* @purpose  Build test config data based upon the type of
*           physical card in present on slot
*
* @param    slot        Physical slot location
* @param    nimCfgVer   Software version of Config Data
* @param    cardIdRev   Config Data for Card ID/REV
* @param    pPr         Pointer to port registry data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void nimBuildTestConfigData()
{
  L7_uint32 intIfNum;
  L7_RC_t   rc;


  sysapiPrintf("The used nimCfgPort_t elements of the config file\n");


  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {

      nimConfigTestSet(intIfNum);
      /* Iterate */
      rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  } /* rc == L7_SUCCESS */


  sysapiPrintf("Built test config data\n");
}


/*********************************************************************
* @purpose  Set test config data for the interface
*
* @param    intfDescr   @b{(input)}   A description of the interface being created
* @param    defaultCfg  @b{(output)}  The created config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    This routine is based on nimConfigDefaultGet
*
* @end
*********************************************************************/
void nimConfigTestSet(L7_uint32 intIfNum)
{
  nimCfgPort_t    *configPort;

  configPort    = &nimCtlBlk_g->nimPorts[intIfNum].configPort;


  /* Ensure at least one entry in each array has a unique value to validate
     accurate migration */


  memset( (void *)configPort->cfgInfo.LAAMacAddr.addr, (0xbb + intIfNum), 6);

  configPort->cfgInfo.addrType    = L7_SYSMAC_LAA;

  strcpy(configPort->cfgInfo.ifAlias, "TestAlias");

  configPort->cfgInfo.trapState   = L7_DISABLE;

  configPort->cfgInfo.encapsType  = L7_ENCAP_802;

  configPort->cfgInfo.mgmtAdminState = L7_ENABLE;
  configPort->cfgInfo.adminState  = L7_DISABLE;

  configPort->cfgInfo.nameType    = L7_ALIASNAME;

  configPort->cfgInfo.ipMtu = L7_PORT_SNAP_ENCAP_MAX_MTU;

  configPort->cfgInfo.cfgMaxFrameSize = 1800 + intIfNum;

  configPort->cfgInfo.negoCapabilities = L7_PORT_NEGO_CAPABILITY_ALL;

  configPort->cfgInfo.ifSpeed = L7_PORTCTRL_PORTSPEED_AAL5_155;
}



/*********************************************************************
* @purpose  Dump a summary table of created interfaces
*
* @param    void        void
*
* @returns  void
*
* @notes    This routine is based on nimConfigDefaultGet
*
* @end
*********************************************************************/
void nimDebugPortTableShow(void)
{
  L7_uint32 intIfNum;
  nimUSP_t  usp;
  L7_INTF_TYPES_t sysIntfType;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_RC_t   rc;

  sysapiPrintf("Total number of interfaces supported = %d\n", platIntfTotalMaxCountGet() );
  sysapiPrintf("Total number of interfaces created = %d\n", nimCtlBlk_g->nimConfigData->numOfInterfaces);
  sysapiPrintf("Total number of NIM indices = %d\n",NIM_INTF_INDICES);


  sysapiPrintf( "\n-----------------------------------------------");
  sysapiPrintf( "\nintIf   USP      ifType              ifName    ");
  sysapiPrintf( "\nNum                                            ");
  sysapiPrintf( "\n-----------------------------------------------");


  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {
    memset((void *)&usp, 0, sizeof(nimUSP_t));

    if ((nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)      &&
        (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)  &&
        (nimGetIntfName(intIfNum, L7_SYSNAME, ifName) == L7_SUCCESS))
    {
      sysapiPrintf("\n");
      sysapiPrintf( "%d  ", intIfNum);
      sysapiPrintf(  "\t%d/%d/%d", usp.unit, usp.slot, usp.port);
      sysapiPrintf("\t %s", nimDebugIntfTypeToString(sysIntfType));
      sysapiPrintf("\t%s", ifName);
    }


    /* Iterate */
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  } /* rc == L7_SUCCESS */

  return;

}


void nimDebugStartupDump(void)
{
  L7_RC_t              rc;
  nimStartUpTreeData_t startupData;
  L7_COMPONENT_IDS_t compInProcess;

  rc = nimStartUpFirstGet(&startupData);

  if (rc == L7_SUCCESS)
  {
    sysapiPrintf("component:  priority:  startup function\n");
  }

  while (rc == L7_SUCCESS)
  {
    sysapiPrintf("%-10s   %6d       %8p\n",
                  cnfgrSidComponentNameGet(startupData.componentId),
                  startupData.priority, startupData.startupFunction);
    rc = nimStartUpNextGet(&startupData);
  }

  compInProcess = nimStartupEventInProcess();
  if (compInProcess != L7_FIRST_COMPONENT_ID)
  {
    sysapiPrintf("\nStartup callback in process:  %s", 
                 cnfgrSidComponentNameGet(compInProcess));
  }
}
