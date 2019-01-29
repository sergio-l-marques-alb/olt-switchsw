/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_debug.c
*
* @purpose   dot1x debug utilities
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*
**********************************************************************/

#include "dot1x_include.h"
#include "sysapi.h"
#include "support_api.h"
#include "log.h"
#include "dot1x_nsf.h"

#define DOT1X_DEBUG_PACKET_RX_FORMAT     "Pkt RX - Intf: %d/%d/%d(%d,%s),SrcMac: %s DestMac: %s Type: %s\n"
#define DOT1X_DEBUG_PACKET_RX_EAP_FORMAT "Pkt RX - Intf: %d/%d/%d(%d,%s),SrcMac: %s DestMac: %s Type: %s Code: %s Id:%d\n"
#define DOT1X_DEBUG_PACKET_TX_FORMAT     "Pkt TX - Intf: %d/%d/%d(%d,%s),SrcMac: %s DestMac: %s Type: %s Code: %s\n"
#define DOT1X_DEBUG_PACKET_TX_EAP_FORMAT "Pkt TX - Intf: %d/%d/%d(%d,%s),SrcMac: %s DestMac: %s Type: %s Code: %s Id:%d\n"

L7_BOOL dot1xDebugPacketTraceTxFlag = L7_FALSE;
L7_BOOL dot1xDebugPacketTraceRxFlag = L7_FALSE;
L7_uint32 dot1xDebugTraceFlag = 0;
L7_uint32 dot1xDebugTraceIntf = 0;

extern void *dot1xQueue;
extern dot1xDebugCfg_t dot1xDebugCfg;

/*  Function prototypes */
void dot1xBuildTestIntfConfigData(dot1xPortCfg_t *pCfg, L7_ushort16 seed);
void dot1xConfigDataTestShow(void);

extern L7_VLAN_MASK_t dot1xVlanMask;
extern L7_ushort16 *dot1xVlanIntfCount;
extern L7_VLAN_MASK_t dot1xGuestVlanMask;
extern L7_ushort16 *dot1xGuestVlanIntfCount;
extern L7_VLAN_MASK_t voiceVlanMask;
extern L7_ushort16 *voiceVlanIntfCount;

static void dot1xDebugVlanMaskPrint(L7_VLAN_MASK_t *temp, L7_ushort16 *intfCount);
/*********************************************************************
* @purpose  Do any initilization required for the debug utilities.
*
* @param
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xDebugInit()
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display the number of messages in the dot1x message queues
*
* @param
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1xDebugMsgQueue()
{
  L7_int32 num;

  if (osapiMsgQueueGetNumMsgs(dot1xQueue, &num) == L7_SUCCESS)
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "Dot1x Messages in queue: %d\n", num);
}

/*********************************************************************
* @purpose  Display the ID of the dot1x Trace Buffer
*
* @param
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1xDebugTraceIdGet()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Trace Id in use for dot1x is %d\n", dot1xInfo.traceId);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Use devshell traceBlockStart(traceId) and traceBlockStop(traceId)\n");
}

/*********************************************************************
* @purpose  Display the sizes of the dot1x structures
*
* @param
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1xDebugSizesShow()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DOT1X Data Structures:\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "----------------------\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1xInfo        = %d\r\n", sizeof(dot1xInfo));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1xPortInfo_t  = %d\r\n", sizeof(dot1xPortInfo_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1xPortStats_t = %d\r\n", sizeof(dot1xPortStats_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1xCfg_t       = %d\r\n", sizeof(dot1xCfg_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "L7_MAX_PORT_COUNT       = %d\r\n", L7_MAX_PORT_COUNT);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "L7_DOT1X_INTF_MAX_COUNT = %d\r\n", L7_DOT1X_INTF_MAX_COUNT);
}

/*********************************************************************
* @purpose  Display the config info for the specified port
*
* @param    intIfNum @b{(input)) internal interface number
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1xDebugPortCfgShow(L7_uint32 intIfNum)
{
  dot1xPortCfg_t *pCfg;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DOT1X AdminMode = %d", dot1xCfg->dot1xAdminMode);

  if (dot1xCfg->dot1xAdminMode == L7_ENABLE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " (Enabled)\r\n");
  }
  else if (dot1xCfg->dot1xAdminMode == L7_DISABLE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " (Disabled)\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " (Unknown mode)\r\n");
  }

  if (dot1xIsValidIntf(intIfNum) != L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "intIfNum(%d) is not a valid dot1x interface\r\n", intIfNum);
    return;
  }

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "intIfNum(%d) is not a valid dot1x interface\r\n", intIfNum);
    return;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DOT1X Cfg Info for port %d:\r\n", intIfNum);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "--------------------------\r\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "adminControlledDirections = %d", pCfg->adminControlledDirections);
  if (pCfg->adminControlledDirections == L7_DOT1X_PORT_DIRECTION_BOTH)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Both\r\n");
  }
  else if (pCfg->adminControlledDirections == L7_DOT1X_PORT_DIRECTION_IN)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " In\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "portControlMode           = %d", pCfg->portControlMode);
  switch (pCfg->portControlMode)
  {
  case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Force Unauthorized\r\n");}
    break;
  case L7_DOT1X_PORT_AUTO:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Auto\r\n");}
    break;
  case L7_DOT1X_PORT_FORCE_AUTHORIZED:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Force Authorized\r\n");}
    break;
  case L7_DOT1X_PORT_AUTO_MAC_BASED:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " MAC based\r\n");}
    break;
  default:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");}
    break;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "quietPeriod               = %d\r\n", pCfg->quietPeriod);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "txPeriod                  = %d\r\n", pCfg->txPeriod);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "suppTimeout               = %d\r\n", pCfg->suppTimeout);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "serverTimeout             = %d\r\n", pCfg->serverTimeout);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "maxReq                    = %d\r\n", pCfg->maxReq);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "reAuthPeriod              = %d\r\n", pCfg->reAuthPeriod);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "reAuthEnabled             = %d", pCfg->reAuthEnabled);
  if (dot1xCfg->dot1xPortCfg[intIfNum].reAuthEnabled == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (pCfg->reAuthEnabled == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "keyTxEnabled              = %d", pCfg->keyTxEnabled);
  if (pCfg->keyTxEnabled == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (pCfg->keyTxEnabled == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }
  
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Guest VLAN Id             = %d\r\n", pCfg->guestVlanId);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Guest VLAN Period         = %d\r\n",pCfg->guestVlanPeriod);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "PAE Capability            = %d %s\r\n", pCfg->paeCapabilities,
                (pCfg->paeCapabilities == L7_DOT1X_PAE_PORT_SUPP_CAPABLE) ? "Supplicant" : "Authenticator");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Mac-based Authentication Bypass (MAB) mode = %d", pCfg->mabEnabled);

  if (pCfg->mabEnabled == L7_ENABLE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " (Enabled)\r\n");
  }
  else if (pCfg->mabEnabled == L7_DISABLE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " (Disabled)\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " (Unknown mode)\r\n");
  }

}

/*********************************************************************
* @purpose  Display the status info for the specified port
*
* @param    lIntIfNum @b{(input)) Logical internal interface number
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1xDebugPortMacInfoShow(L7_uint32 lIntIfNum)
{
  L7_uchar8 buf[32];
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physPort;
  L7_uchar8 apmState[50]={0}, bamState[50]={0}, ktxState[50]={0},
            rtmState[50]={0}, krxState[50]={0};

  dot1xLogicalPortInfoTakeLock();
  logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  if (logicalPortInfo == L7_NULLPTR)
  {
    dot1xLogicalPortInfoGiveLock();
    printf("Cannot get logical port info for this log interface %d \n", lIntIfNum);
    return;
  }
  physPort = logicalPortInfo->physPort;


  if (dot1xIsValidIntf(physPort) != L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "intIfNum is not a valid dot1x interface(%d)\r\n", physPort);
    dot1xLogicalPortInfoGiveLock();
    return;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DOT1X Info for port %d Phy port(%d) :\r\n", lIntIfNum, physPort);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "------------------------------\r\n");

  if (logicalPortInfo->inUse == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                    "Port is in Use\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                    "Port Not in Use\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Port Status               = %d\n", logicalPortInfo->logicalPortStatus);


    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\n\rWhileTimer QuietWhileTimer SuppReauthTimer TxReqTimer GuestVlanTimer\n\r");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "---------- --------------- --------------- ---------- --------------\n\r");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-11d%-16d%-16d%-11d%-14d\n\r\n\r", logicalPortInfo->aWhile, logicalPortInfo->quietWhile,
             logicalPortInfo->reAuthWhen, logicalPortInfo->txWhenLP, logicalPortInfo->guestVlanTimer);

    switch (logicalPortInfo->apmState)
    {
      case APM_INITIALIZE:
          osapiSnprintf(apmState,sizeof(apmState), "Initialize");
          break;
      case APM_DISCONNECTED:
          osapiSnprintf(apmState,sizeof(apmState), "Disconnected");
          break;
      case APM_CONNECTING:
          osapiSnprintf(apmState,sizeof(apmState), "Connecting");
          break;
      case APM_AUTHENTICATING:
          osapiSnprintf(apmState,sizeof(apmState), "Authenticating");
           break;
      case APM_AUTHENTICATED:
          osapiSnprintf(apmState,sizeof(apmState), "Authenticated");
          break;
      case APM_ABORTING:
          osapiSnprintf(apmState,sizeof(apmState), "Aborting");
           break;
      case APM_HELD:
          osapiSnprintf(apmState,sizeof(apmState), "Held");
           break;
      case APM_FORCE_AUTH:
          osapiSnprintf(apmState,sizeof(apmState), "Force-Auth");
          break;
      case APM_FORCE_UNAUTH:
          osapiSnprintf(apmState,sizeof(apmState), "Force-Unauth");
          break;
      default:
          osapiSnprintf(apmState,sizeof(apmState), "Unknown");
          break;
     }

  switch (logicalPortInfo->ktxState)
  {
    case KTX_NO_KEY_TRANSMIT:
      osapiSnprintf(ktxState,sizeof(ktxState), "No-Key-Transmit");
      break;
    case KTX_KEY_TRANSMIT:
      osapiSnprintf(ktxState,sizeof(ktxState), "Key-Transmit");
      break;
    default:
      osapiSnprintf(ktxState,sizeof(ktxState), "Unknown");
      break;
  }

  switch (logicalPortInfo->rtmState)
  {
    case RTM_INITIALIZE:
      osapiSnprintf(rtmState,sizeof(rtmState), "Initialize");
      break;
    case RTM_REAUTHENTICATE:
      osapiSnprintf(rtmState,sizeof(rtmState), "Reauthenticate");
      break;
    default:
      osapiSnprintf(rtmState,sizeof(rtmState), "Unknown");
      break;
  }

  switch (logicalPortInfo->bamState)
  {
    case BAM_REQUEST:
      osapiSnprintf(bamState,sizeof(bamState), "Request");
      break;
    case BAM_RESPONSE:
      osapiSnprintf(bamState,sizeof(bamState), "Response");
      break;
    case BAM_SUCCESS:
      osapiSnprintf(bamState,sizeof(bamState), "Success");
      break;
    case BAM_FAIL:
      osapiSnprintf(bamState,sizeof(bamState), "Fail");
      break;
    case BAM_TIMEOUT:
      osapiSnprintf(bamState,sizeof(bamState), "Timeout");
      break;
    case BAM_IDLE:
      osapiSnprintf(bamState,sizeof(bamState), "Idle");
      break;
    case BAM_INITIALIZE:
      osapiSnprintf(bamState,sizeof(bamState), "Initialize");
      break;
    default:
      osapiSnprintf(bamState,sizeof(bamState), "Unknown");
      break;
  }

  switch (logicalPortInfo->krxState)
  {
    case KRX_NO_KEY_RECEIVE:
      osapiSnprintf(krxState,sizeof(krxState), "No-Key-Receive");
      break;
    case KRX_KEY_RECEIVE:
      osapiSnprintf(krxState,sizeof(krxState), "Key-Receive");
      break;
    default:
      osapiSnprintf(krxState,sizeof(krxState), "Unknown");
      break;
  }
  
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
      "\n\r%-15s%-16s%-12s%-15s%s\n\r", "APM_STATE","KTX_STATE","RTM_STATE","BAM_STATE","KRX_STATE");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "-------------- --------------- ----------- -------------- --------------\n\r");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
      "%-15s%-16s%-12s%-15s%s\n\r\n\r", apmState,ktxState,rtmState,bamState,krxState);


  if (logicalPortInfo->serverStateLen > 0)
  {
    L7_uint32 i;
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "serverState               = ");
    for (i = 0; i < logicalPortInfo->serverStateLen; i++)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "%02X", logicalPortInfo->serverState[i]);
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "serverState               = NULL\r\n");
  }

  if (logicalPortInfo->serverClassLen > 0)
  {
    L7_uint32 i;
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "serverClass               = ");
    for (i = 0; i < logicalPortInfo->serverClassLen; i++)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "%02X", logicalPortInfo->serverClass[i]);
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "serverClass               = NULL\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sessionTimeout            = %d\r\n", logicalPortInfo->sessionTimeout);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "terminationAction         = %d\r\n", logicalPortInfo->terminationAction);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "suppBufHandle             = 0x%08X\r\n", logicalPortInfo->suppBufHandle);

  bzero(buf, 32);
  osapiSnprintf((L7_char8 *)buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
          logicalPortInfo->suppMacAddr.addr[0],
          logicalPortInfo->suppMacAddr.addr[1],
          logicalPortInfo->suppMacAddr.addr[2],
          logicalPortInfo->suppMacAddr.addr[3],
          logicalPortInfo->suppMacAddr.addr[4],
          logicalPortInfo->suppMacAddr.addr[5]);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "suppMacAddr               = %s\r\n", buf);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "reAuthenticating          = %d", logicalPortInfo->reAuthenticating);
  if (logicalPortInfo->reAuthenticating == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (logicalPortInfo->reAuthenticating == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Reauthentication count    = %d\r\n",logicalPortInfo->reAuthCount);
  
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "suppRestarting            = %d", logicalPortInfo->suppRestarting);
  if (logicalPortInfo->suppRestarting == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (logicalPortInfo->suppRestarting == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authMethod                = %d", logicalPortInfo->authMethod);
  switch (logicalPortInfo->authMethod)
  {
  case L7_AUTH_METHOD_UNDEFINED:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Undefined\r\n");}
    break;
  case L7_AUTH_METHOD_LOCAL:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Local\r\n");}
    break;
  case L7_AUTH_METHOD_RADIUS:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Radius\r\n");}
    break;
  case L7_AUTH_METHOD_REJECT:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Reject\r\n");}
    break;
  default:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");}
    break;
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Vlan Assigned by Radius        = %d\r\n",logicalPortInfo->vlanAssigned);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Filter Id Assigned by Radius   = %s\r\n",logicalPortInfo->filterName);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Client Session Timeout         = %d\r\n",logicalPortInfo->clientTimeout);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Guest Vlan Id                  = %d\r\n",logicalPortInfo->guestVlanId);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Unauth Vlan Id                 = %d\r\n",logicalPortInfo->unauthVlan);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Voice Vlan Id                  = %d\r\n",logicalPortInfo->voiceVlanId);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Vlan Id                        = %d\r\n",logicalPortInfo->vlanId);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Blocked Vlan Id                = %d\r\n",logicalPortInfo->blockVlanId);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Guest Vlan Timer               = %d\r\n",logicalPortInfo->guestVlanTimer);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "MAB Client                     = %s\n\r", (logicalPortInfo->isMABClient==L7_TRUE)?"True":"False");
  
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Default Vlan                   = %d\n\r", logicalPortInfo->defaultVlanId);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Monitor Mode Client            = %s\n\r", (logicalPortInfo->isMonitorModeClient == L7_TRUE)?"True":"False");


  dot1xLogicalPortInfoGiveLock();
  return;
}



/*********************************************************************
* @purpose  Display the status info for the specified port
*
* @param    intIfNum @b{(input)) internal interface number
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1xDebugPortInfoShow(L7_uint32 intIfNum)
{
  if (dot1xIsValidIntf(intIfNum) != L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "intIfNum is not a valid dot1x interface(%d)\r\n", intIfNum);
    return;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DOT1X Status Info for port %d:\r\n", intIfNum);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "------------------------------\r\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "paeProtocolVersion        = %d\r\n", dot1xPortInfo[intIfNum].paeProtocolVersion);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "paeCapabilities           = 0x%02X", dot1xPortInfo[intIfNum].paeCapabilities);
  if (dot1xPortInfo[intIfNum].paeCapabilities & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Authenticator");
  }
  if (dot1xPortInfo[intIfNum].paeCapabilities & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Supplicant");
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\r\n");

  /*SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "txWhen                    = %d\r\n", dot1xPortInfo[intIfNum].txWhenPort);*/

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "currentId                 = %d\r\n", dot1xPortInfo[intIfNum].currentId);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "initialize                = %d", dot1xPortInfo[intIfNum].initialize);
  if (dot1xPortInfo[intIfNum].initialize == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (dot1xPortInfo[intIfNum].initialize == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "portControlMode           = %d", dot1xPortInfo[intIfNum].portControlMode);
  switch (dot1xPortInfo[intIfNum].portControlMode)
  {
  case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Force Unauthorized\r\n");}
    break;
  case L7_DOT1X_PORT_AUTO:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Auto\r\n");}
    break;
  case L7_DOT1X_PORT_FORCE_AUTHORIZED:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Force Authorized\r\n");}
    break;
  case L7_DOT1X_PORT_AUTO_MAC_BASED:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Mac-Based Authentication\r\n");}
    break;
  default:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");}
    break;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "portEnabled               = %d", dot1xPortInfo[intIfNum].portEnabled);
  if (dot1xPortInfo[intIfNum].portEnabled == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (dot1xPortInfo[intIfNum].portEnabled == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "portStatus                = %d", dot1xPortInfo[intIfNum].portStatus);
  if (dot1xPortInfo[intIfNum].portStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Authorized\r\n");
  }
  else if (dot1xPortInfo[intIfNum].portStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unauthorized\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "quietPeriod               = %d\r\n", dot1xPortInfo[intIfNum].quietPeriod);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "txPeriod                  = %d\r\n", dot1xPortInfo[intIfNum].txPeriod);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "keyAvailable              = %d", dot1xPortInfo[intIfNum].keyAvailable);
  if (dot1xPortInfo[intIfNum].keyAvailable == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (dot1xPortInfo[intIfNum].keyAvailable == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "keyTxEnabled              = %d", dot1xPortInfo[intIfNum].keyTxEnabled);
  if (dot1xPortInfo[intIfNum].keyTxEnabled == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (dot1xPortInfo[intIfNum].keyTxEnabled == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "reAuthPeriod              = %d\r\n", dot1xPortInfo[intIfNum].reAuthPeriod);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "reAuthEnabled             = %d", dot1xPortInfo[intIfNum].reAuthEnabled);
  if (dot1xPortInfo[intIfNum].reAuthEnabled == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (dot1xPortInfo[intIfNum].reAuthEnabled == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "suppTimeout               = %d\r\n", dot1xPortInfo[intIfNum].suppTimeout);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "serverTimeout             = %d\r\n", dot1xPortInfo[intIfNum].serverTimeout);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "adminControlledDirections = %d", dot1xPortInfo[intIfNum].adminControlledDirections);
  if (dot1xPortInfo[intIfNum].adminControlledDirections == L7_DOT1X_PORT_DIRECTION_BOTH)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Both\r\n");
  }
  else if (dot1xPortInfo[intIfNum].adminControlledDirections == L7_DOT1X_PORT_DIRECTION_IN)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " In\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "operControlledDirections  = %d", dot1xPortInfo[intIfNum].operControlledDirections);
  if (dot1xPortInfo[intIfNum].operControlledDirections == L7_DOT1X_PORT_DIRECTION_BOTH)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Both\r\n");
  }
  else if (dot1xPortInfo[intIfNum].operControlledDirections == L7_DOT1X_PORT_DIRECTION_IN)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " In\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "bridgeDetected            = %d", dot1xPortInfo[intIfNum].bridgeDetected);
  if (dot1xPortInfo[intIfNum].bridgeDetected == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (dot1xPortInfo[intIfNum].bridgeDetected == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "rxKey                     = %d", dot1xPortInfo[intIfNum].rxKey);
  if (dot1xPortInfo[intIfNum].rxKey == L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " True\r\n");
  }
  else if (dot1xPortInfo[intIfNum].rxKey == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " False\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authMethod                = %d", dot1xPortInfo[intIfNum].authMethod);
  switch (dot1xPortInfo[intIfNum].authMethod)
  {
  case L7_AUTH_METHOD_UNDEFINED:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Undefined\r\n");}
    break;
  case L7_AUTH_METHOD_LOCAL:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Local\r\n");}
    break;
  case L7_AUTH_METHOD_RADIUS:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Radius\r\n");}
    break;
  case L7_AUTH_METHOD_REJECT:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Reject\r\n");}
    break;
  default:
    {SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");}
    break;
  }
 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "MAB enabled            = %d", dot1xPortInfo[intIfNum].mabEnabled);
  if (dot1xPortInfo[intIfNum].mabEnabled == L7_ENABLE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Enabled\r\n");
  }
  else if (dot1xPortInfo[intIfNum].mabEnabled == L7_DISABLE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Disabled\r\n");
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " Unknown\r\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Number of Authorizations  = %d\r\n", dot1xPortInfo[intIfNum].authCount);


  return;
}

/*********************************************************************
* @purpose  Display the status for the specified port
*
* @param    intIfNum @b{(input)) internal interface number
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1xDebugPortStatsShow(L7_uint32 intIfNum)
{
  L7_uchar8 buf[32];

  if (dot1xIsValidIntf(intIfNum) != L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "intIfNum is not a valid interface(%d)\r\n", intIfNum);
    return;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DOT1X Stats for port %d:\r\n", intIfNum);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "-----------------------\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEapolFramesRx          = %d\r\n", dot1xPortStats[intIfNum].authEapolFramesRx);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEapolFramesTx          = %d\r\n", dot1xPortStats[intIfNum].authEapolFramesTx);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEapolStartFramesRx     = %d\r\n", dot1xPortStats[intIfNum].authEapolStartFramesRx);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEapolLogoffFramesRx    = %d\r\n", dot1xPortStats[intIfNum].authEapolLogoffFramesRx);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEapRespIdFramesRx      = %d\r\n", dot1xPortStats[intIfNum].authEapRespIdFramesRx);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEapResponseFramesRx    = %d\r\n", dot1xPortStats[intIfNum].authEapResponseFramesRx);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEapReqFramesTx         = %d\r\n", dot1xPortStats[intIfNum].authEapReqFramesTx);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authInvalidEapolFramesRx   = %d\r\n", dot1xPortStats[intIfNum].authInvalidEapolFramesRx);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEapLengthErrorFramesRx = %d\r\n", dot1xPortStats[intIfNum].authEapLengthErrorFramesRx);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authLastEapolFrameVersion  = %d\r\n", dot1xPortStats[intIfNum].authLastEapolFrameVersion);
  bzero(buf, 32);
  osapiSnprintf((L7_char8 *)buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
          dot1xPortStats[intIfNum].authLastEapolFrameSource.addr[0],
          dot1xPortStats[intIfNum].authLastEapolFrameSource.addr[1],
          dot1xPortStats[intIfNum].authLastEapolFrameSource.addr[2],
          dot1xPortStats[intIfNum].authLastEapolFrameSource.addr[3],
          dot1xPortStats[intIfNum].authLastEapolFrameSource.addr[4],
          dot1xPortStats[intIfNum].authLastEapolFrameSource.addr[5]);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authLastEapolFrameSource   = %s\r\n", buf);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\r\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DOT1X Debug Stats for port %d:\r\n", intIfNum);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "-----------------------------\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEntersConnecting                     = %d\r\n", dot1xPortStats[intIfNum].authEntersConnecting);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEapLogoffsWhileConnecting            = %d\r\n", dot1xPortStats[intIfNum].authEapLogoffsWhileConnecting);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authEntersAuthenticating                 = %d\r\n", dot1xPortStats[intIfNum].authEntersAuthenticating);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authAuthSuccessWhileAuthenticating       = %d\r\n", dot1xPortStats[intIfNum].authAuthSuccessWhileAuthenticating);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authAuthTimeoutsWhileAuthenticating      = %d\r\n", dot1xPortStats[intIfNum].authAuthTimeoutsWhileAuthenticating);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authAuthFailWhileAuthenticating          = %d\r\n", dot1xPortStats[intIfNum].authAuthFailWhileAuthenticating);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authAuthReauthsWhileAuthenticating       = %d\r\n", dot1xPortStats[intIfNum].authAuthReauthsWhileAuthenticating);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authAuthEapStartsWhileAuthenticating     = %d\r\n", dot1xPortStats[intIfNum].authAuthEapStartsWhileAuthenticating);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authAuthEapLogoffWhileAuthenticating     = %d\r\n", dot1xPortStats[intIfNum].authAuthEapLogoffWhileAuthenticating);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authAuthReauthsWhileAuthenticated        = %d\r\n", dot1xPortStats[intIfNum].authAuthReauthsWhileAuthenticated);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authAuthEapStartsWhileAuthenticated      = %d\r\n", dot1xPortStats[intIfNum].authAuthEapStartsWhileAuthenticated);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authAuthEapLogoffWhileAuthenticated      = %d\r\n", dot1xPortStats[intIfNum].authAuthEapLogoffWhileAuthenticated);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authBackendResponses                     = %d\r\n", dot1xPortStats[intIfNum].authBackendResponses);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authBackendAccessChallenges              = %d\r\n", dot1xPortStats[intIfNum].authBackendAccessChallenges);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authBackendOtherRequestsToSupplicant     = %d\r\n", dot1xPortStats[intIfNum].authBackendOtherRequestsToSupplicant);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authBackendNonNakResponsesFromSupplicant = %d\r\n", dot1xPortStats[intIfNum].authBackendNonNakResponsesFromSupplicant);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authBackendAuthSuccesses                 = %d\r\n", dot1xPortStats[intIfNum].authBackendAuthSuccesses);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "authBackendAuthFails                     = %d\r\n", dot1xPortStats[intIfNum].authBackendAuthFails);
}


/*********************************************************************
* @purpose  Set the dot1x log trace mode
*
* @param    mode     @b{(input)) log trace mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xDebugLogTraceModeSet(L7_uint32 mode)
{
  /* Validate input parm */
  if (mode != L7_ENABLE && mode != L7_DISABLE)
    return L7_FAILURE;

  /* If not changing mode, just return success */
  if (mode == dot1xCfg->dot1xLogTraceMode)
    return L7_SUCCESS;

  dot1xCfg->dot1xLogTraceMode = mode;

  dot1xCfg->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return the dot1x log trace mode
*
* @param    mode     @b{(input)) log trace mode
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_uint32 dot1xDebugLogTraceModeGet()
{
  return dot1xCfg->dot1xLogTraceMode;
}



/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/




/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void dot1xBuildTestConfigData(void)
{

  L7_ushort16 cfgIndex;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/
   dot1xCfg->dot1xAdminMode    = L7_ENABLE;
   dot1xCfg->dot1xLogTraceMode = L7_ENABLE;

   for (cfgIndex = 1; cfgIndex < L7_DOT1X_INTF_MAX_COUNT; cfgIndex++)
     dot1xBuildTestIntfConfigData(&dot1xCfg->dot1xPortCfg[cfgIndex], cfgIndex );


 /* End of Component's Test Non-default configuration Data */

   /* Force write of config file */
   dot1xCfg->cfgHdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");


}


/*********************************************************************
* @purpose  Build non-default dot1x port config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void dot1xBuildTestIntfConfigData(dot1xPortCfg_t *pCfg, L7_ushort16 seed)
{

  pCfg->adminControlledDirections = L7_DOT1X_PORT_DIRECTION_IN;
  pCfg->portControlMode           = L7_DOT1X_PORT_FORCE_AUTHORIZED;
  pCfg->quietPeriod               = 70 + seed;
  pCfg->txPeriod                  = 40;
  pCfg->suppTimeout               = 50;
  pCfg->serverTimeout             = 60;
  pCfg->maxReq                    = 3 + seed;
  pCfg->reAuthPeriod              = 2400;
  pCfg->reAuthEnabled             = L7_TRUE;
  pCfg->keyTxEnabled              = L7_TRUE;
  pCfg->guestVlanId               = 1;
  pCfg->guestVlanPeriod           = 150;

}


/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void dot1xConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &dot1xCfg->cfgHdr;

    sysapiCfgFileHeaderDump (pFileHdr);


   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    dot1xCfgDump();


   /*-------------------------------*/
   /* Scaling Constants             */
   /*-------------------------------*/

    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_DOT1X_INTF_MAX_COUNT - %d\n",
                  L7_DOT1X_INTF_MAX_COUNT);


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("dot1xCfg->checkSum : %u\n", dot1xCfg->checkSum);


}


/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/

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
void dot1xDebugRegister(void)
{
  supportDebugDescr_t supportDebugDescr;

  memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

  supportDebugDescr.componentId = L7_DOT1X_COMPONENT_ID;

  /* Register User Control Parms */
  supportDebugDescr.userControl.notifySave = dot1xDebugSave;
  supportDebugDescr.userControl.hasDataChanged = dot1xDebugHasDataChanged;
  supportDebugDescr.userControl.clearFunc = dot1xDebugRestore;

  /* Register Internal Parms */

  /* Submit registration */

  (void) supportDebugRegister(supportDebugDescr);
}

/*********************************************************************
*
* @purpose Trace dot1x packets received
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   bufHandle  @b{(input)} packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot1xDebugPacketRxTrace(L7_uint32 intIfNum, L7_netBufHandle bufHandle)
{
  if (dot1xDebugPacketTraceRxFlag != L7_TRUE)
    return;

  dot1xDebugPacketTrace(intIfNum,bufHandle,L7_TRUE,L7_FALSE);
  return;
}

/*********************************************************************
*
* @purpose Trace dot1x packets transmitted
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   bufHandle  @b{(input)} packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot1xDebugPacketTxTrace(L7_uint32 intIfNum, L7_netBufHandle bufHandle)
{
  if (dot1xDebugPacketTraceTxFlag == L7_TRUE || (((dot1xDebugTraceFlag&DOT1X_TRACE_PDU) != 0) 
      && (intIfNum==dot1xDebugTraceIntf || dot1xDebugTraceIntf==0)))
  {
    dot1xDebugPacketTrace(intIfNum,bufHandle,L7_FALSE,L7_TRUE);
  }
  return;
}

/*********************************************************************
*
* @purpose Trace dot1x packets
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   bufHandle  @b{(input)} packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot1xDebugPacketTrace(L7_uint32 intIfNum, L7_netBufHandle bufHandle,L7_BOOL rxFlag,L7_BOOL txFlag)
{
  L7_enetHeader_t     *enetHdr;
  L7_char8 srcMac[20];
  L7_char8 destMac[20];
  L7_char8 type[25],code[25];
  L7_uint32 pduDataOffset;
  L7_uchar8 *data;
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  nimUSP_t usp;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  enetHdr = (L7_enetHeader_t *)(data);
  pduDataOffset = sysNetDataOffsetGet(data);
  eapolPkt = (L7_eapolPacket_t *)(data + pduDataOffset);
  eapPkt = (L7_eapPacket_t *)(data + pduDataOffset + sizeof(L7_eapolPacket_t));

  osapiSnprintf(srcMac, sizeof(srcMac), "%02x:%02x:%02x:%02x:%02x:%02x",enetHdr->src.addr[0],enetHdr->src.addr[1],
      enetHdr->src.addr[2],enetHdr->src.addr[3],enetHdr->src.addr[4],enetHdr->src.addr[5]);
  osapiSnprintf(destMac, sizeof(destMac), "%02x:%02x:%02x:%02x:%02x:%02x",enetHdr->dest.addr[0],enetHdr->dest.addr[1],
      enetHdr->dest.addr[2],enetHdr->dest.addr[3],enetHdr->dest.addr[4],enetHdr->dest.addr[5]);

  if (txFlag == L7_TRUE)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_PDU,intIfNum ,"\n%s:%d: EAPOL packet being sent to %02x:%02x:%02x:%02x:%02x:%02x type- %d \n",
                      __FUNCTION__,__LINE__,
                      enetHdr->dest.addr[0],
                      enetHdr->dest.addr[1],
                      enetHdr->dest.addr[2],
                      enetHdr->dest.addr[3],
                      enetHdr->dest.addr[4],
                      enetHdr->dest.addr[5],eapolPkt->packetType);
  }

  switch (eapolPkt->packetType)
  {
    case EAPOL_EAPPKT:
      osapiSnprintf(type, sizeof(type), "EAP Packet");
      switch(eapPkt->code)
      {
        case EAP_REQUEST:
          osapiSnprintf(code, sizeof(code), "EAP Request");
          break;

        case EAP_RESPONSE:
          osapiSnprintf(code, sizeof(code), "EAP Response");
          break;

        case EAP_SUCCESS:
          osapiSnprintf(code, sizeof(code), "EAP Success");
          break;

        case EAP_FAILURE:
          osapiSnprintf(code,sizeof(code), "EAP Failure");
          break;

        default:
          break;
      }
      if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        if((dot1xDebugPacketTraceRxFlag == L7_TRUE) && (rxFlag == L7_TRUE))
        {
          DOT1X_USER_TRACE_RX(DOT1X_DEBUG_PACKET_RX_EAP_FORMAT,
              (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port,
              intIfNum,ifName,srcMac, destMac, type,code,eapPkt->id);
        }
        else if ((dot1xDebugPacketTraceTxFlag == L7_TRUE) && (txFlag == L7_TRUE))
        {
          DOT1X_USER_TRACE_TX(DOT1X_DEBUG_PACKET_TX_EAP_FORMAT,
              (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port,
              intIfNum,ifName,srcMac, destMac, type,code,eapPkt->id)
        }
      }
      break;

    case EAPOL_START:
      osapiSnprintf(type, sizeof(type), "EAPOL-Start frame");
      break;

    case EAPOL_LOGOFF:
      osapiSnprintf(type, sizeof(type), "EAPOL-Logoff request frame");
      break;

    case EAPOL_KEY:
      osapiSnprintf(type, sizeof(type), "EAPOL-Key frame");
      break;

    case EAPOL_ENCASFALERT:
      osapiSnprintf(type, sizeof(type), "EAPOL-Encapsulated-ASF-Alert");
      break;

    default:
      break;

  }

  if(eapolPkt->packetType != EAPOL_EAPPKT)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      if ((dot1xDebugPacketTraceRxFlag == L7_TRUE) && (rxFlag == L7_TRUE))
      {
        DOT1X_USER_TRACE_RX(DOT1X_DEBUG_PACKET_RX_FORMAT,
                            (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port,
                            intIfNum,ifName,srcMac, destMac, type,code,eapPkt->id);
      }
      else if ((dot1xDebugPacketTraceTxFlag == L7_TRUE) && (txFlag == L7_TRUE))
      {
        DOT1X_USER_TRACE_TX(DOT1X_DEBUG_PACKET_TX_FORMAT,
                            (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port,
                            intIfNum,ifName,srcMac, destMac, type,code,eapPkt->id);
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  Turns on/off the displaying of dot1x packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1xDebugPacketTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag)
{
  dot1xDebugPacketTraceTxFlag = transmitFlag;
  dot1xDebugPacketTraceRxFlag = receiveFlag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
   */

  dot1xDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the current status of displaying dot1x packet debug info
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
void dot1xDebugPacketTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag)
{
  *transmitFlag = dot1xDebugPacketTraceTxFlag;
  *receiveFlag =  dot1xDebugPacketTraceRxFlag;
  return;
}

/*********************************************************************
* @purpose  Save configuration settings for dot1x trace data
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1xDebugPacketTraceFlagSave()
{
  dot1xDebugCfg.cfg.dot1xDebugPacketTraceTxFlag = dot1xDebugPacketTraceTxFlag;
  dot1xDebugCfg.cfg.dot1xDebugPacketTraceRxFlag = dot1xDebugPacketTraceRxFlag;
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
void dot1xDebugCfgUpdate(void)
{
  dot1xDebugPacketTraceFlagSave();
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
void dot1xDebugCfgRead(void)
{
  /* reset the debug flags*/
  memset((void*)&dot1xDebugCfg, 0 ,sizeof(dot1xDebugCfg_t));

  (void)sysapiSupportCfgFileGet(L7_DOT1X_COMPONENT_ID, DOT1X_DEBUG_CFG_FILENAME,
                                (L7_char8 *)&dot1xDebugCfg, (L7_uint32)sizeof(dot1xDebugCfg_t),
                                &dot1xDebugCfg.checkSum, DOT1X_DEBUG_CFG_VER_CURRENT,
                                dot1xDebugBuildDefaultConfigData, L7_NULL);

  dot1xDebugCfg.hdr.dataChanged = L7_FALSE;

}

void dot1xDebugVlanMask()
{
  sysapiPrintf( "dot1xVlanMask: \n");
  dot1xDebugVlanMaskPrint(&dot1xVlanMask, dot1xVlanIntfCount);
  sysapiPrintf("\ndot1xGuestVlanMask: \n");
  dot1xDebugVlanMaskPrint(&dot1xGuestVlanMask, dot1xGuestVlanIntfCount);
  sysapiPrintf("\nvoiceVlanMask: \n");
  dot1xDebugVlanMaskPrint(&voiceVlanMask, voiceVlanIntfCount);
}
void dot1xDebugVlanMaskPrint(L7_VLAN_MASK_t *temp, L7_ushort16 *intfCount)
{
  L7_uint32 i, j;

  for (i=1, j=0; i <= L7_VLAN_MAX_MASK_BIT; i++)
  {
      if (L7_VLAN_ISMASKBITSET_POINTER(temp, i) )
      {

          sysapiPrintf("v%d i%d, ",  i, intfCount[i]);

          /* Print no more than 10 ports on a line */

          j++;
          if (j%10 == 0 )    /* Sleepy hack*/
          {
              sysapiPrintf( "\n");
          }
      }
  }
}

/*********************************************************************
* @purpose Trace dot1x  events
*
* @param   0 - disable tracing
*          1 - enable port status events
*          2 - enable port events tracing
*
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot1xDebugTraceEvent(L7_uint32 debug,L7_uint32 intfNum)
{
  dot1xDebugTraceFlag = debug;
  dot1xDebugTraceIntf = intfNum;
}

/*********************************************************************
* @purpose show dot1x trace events help
*
* @param
*
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot1xDebugTraceEventHelp()
{

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n Use dot1xDebugTraceEvent(<debug>,<interface Number>) to trace various events");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n Specify internal interface number to trace events for specific interface or 0 for all interfaces.");

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n Trace Event Flags");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n-------------------");

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n Flag                            Description                                            Value");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n ------------------------------- -----------------------------------------------------  -------");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_PORT_STATUS         Traces port authorization events                        %u",DOT1X_TRACE_PORT_STATUS);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_EVENTS              Traces callback events                                  %u",DOT1X_TRACE_EVENTS);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_API_CALLS           Traces dot1x send packet events                         %u",DOT1X_TRACE_API_CALLS);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_FSM_EVENTS          Traces state machine events                             %u",DOT1X_TRACE_FSM_EVENTS);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_FAILURE             Traces failure events such as authentication failure    %u",DOT1X_TRACE_FAILURE);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_EXTERNAL            Traces evenst affecting external components viz 802.1Q  %u",DOT1X_TRACE_EXTERNAL);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_RADIUS              Traces RADIUS related events                            %u",DOT1X_TRACE_RADIUS);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_TIMER               Traces Dot1x Timer Events                               %u",DOT1X_TRACE_TIMER);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_PDU                 Traces EAPOL Packet recieve events                      %u",DOT1X_TRACE_PDU);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_MAC_ADDR_DB         Traces Mac Address Database related events              %u",DOT1X_TRACE_MAC_ADDR_DB);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n DOT1X_TRACE_MAB                 Traces MAB related events                               %u",DOT1X_TRACE_MAB);
}

/*********************************************************************
*
* @purpose  Debug to add sample Enable/Disable Voice Vlan Device authorization.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
* @param    L7_BOOL          flag       @b{(input)} TRUE - Enable, FALSE - DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*
* @end
*********************************************************************/
void dot1xDebugPortVoiceVlanAuthMode(L7_uint32 intIfNum,
                                     L7_ushort16 vlanId,
                                     L7_BOOL   flag)
{
  L7_enetMacAddr_t macAddr;
  macAddr.addr[0] = 0x00;
  macAddr.addr[1] = 0x01;
  macAddr.addr[2] = 0x02;
  macAddr.addr[3] = 0x03;
  macAddr.addr[4] = 0x04;
  macAddr.addr[5] = 0x05;
  dot1xPortVoiceVlanAuthMode(intIfNum,macAddr,vlanId,flag);
}


/*********************************************************************
* @purpose dump contents of given packet
*
* @param
*
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot1xDebugPacketDump(L7_uint32 flag,L7_uint32 physPort,L7_uint32 intIfNum,L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_uint32 datalen,i,rowcnt = 0;


  DOT1X_EVENT_TRACE(flag,physPort,"\n Recieved packet on interface:%d",intIfNum);
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, datalen);

  DOT1X_EVENT_TRACE(flag,physPort,"\n ===============================================================================\n");
  DOT1X_EVENT_TRACE(flag,physPort,"\n");
  for(i=0;i<datalen;i++)
  {
     DOT1X_EVENT_TRACE(flag,physPort,"%02X ",data[i]);
     rowcnt+=2;
     if(rowcnt == 40)
     {
       DOT1X_EVENT_TRACE(flag,physPort,"\n");
       rowcnt =0;
     }

  }

  DOT1X_EVENT_TRACE(flag,physPort,"\n ===============================================================================\n");
  DOT1X_EVENT_TRACE(flag,physPort,"\n");
}

/*********************************************************************
* @purpose dump contents of string
*
* @param
*
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot1xDebugDataDump(L7_uint32 flag, L7_uint32 physPort,L7_uchar8  *data,L7_uint32 len)
{
  L7_uint32 i=0,rowcnt=0;

  DOT1X_EVENT_TRACE(flag,physPort,"\n Data : \n");
  DOT1X_EVENT_TRACE(flag,physPort,"\n ===============================================================================\n");
  DOT1X_EVENT_TRACE(flag,physPort,"\n");
  for(i=0;i<len;i++)
  {
      DOT1X_EVENT_TRACE(flag,physPort,"%02X ",data[i]);
     rowcnt+=2;
     if(rowcnt == 40)
     {
       DOT1X_EVENT_TRACE(flag,physPort,"\n");
       rowcnt =0;
     }

  }

  DOT1X_EVENT_TRACE(flag,physPort,"\n ===============================================================================\n");
  DOT1X_EVENT_TRACE(flag,physPort,"\n");

}

/*********************************************************************
* @purpose  Display the status info for the specified port
*
* @param    lIntIfNum @b{(input)) Logical internal interface number
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1xDebugLogicalPortInfoShow(L7_uint32 intIfNum, L7_uint32 lIntIfNum)
{
  dot1xLogicalPortInfo_t *logicalPortInfo;
  L7_uint32 physPort;
  L7_BOOL all=L7_FALSE;
#if 0
  L7_uchar8 buf[32];
  L7_uchar8 apmState[50]={0}, bamState[50]={0}, ktxState[50]={0},
            rtmState[50]={0}, krxState[50]={0};
#endif
 
              
  if(lIntIfNum == 0)
  {
    all = L7_TRUE;
    logicalPortInfo = dot1xLogicalPortInfoFirstGet(intIfNum, &lIntIfNum);
  }
  else
  {
    dot1xLogicalPortInfoTakeLock();
    logicalPortInfo = dot1xLogicalPortInfoGet(lIntIfNum);
  }

  if (logicalPortInfo == L7_NULLPTR)
  {
    if(all == L7_FALSE)
      dot1xLogicalPortInfoGiveLock();
    printf("Cannot get logical port info for this log interface %d \n", lIntIfNum);
    return;
  }
   
  physPort = logicalPortInfo->physPort;
  if (dot1xIsValidIntf(physPort) != L7_TRUE)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "intIfNum is not a valid dot1x interface(%d)\r\n", physPort);
    if(all == L7_FALSE)
      dot1xLogicalPortInfoGiveLock();
    return;
  }

  if(intIfNum != physPort)
  {
    sysapiPrintf("Error! LogicalPort[%d] not belongs to Port[%d] \n\r",logicalPortInfo->logicalPortNumber, intIfNum);
    if(all == L7_FALSE)
      dot1xLogicalPortInfoGiveLock();
    return;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Port Control Mode         = %s\n\n\r", (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO)?"Auto":"Mac-Based");

  while(lIntIfNum != 0)
  {
    if(all == L7_FALSE)
      dot1xLogicalPortInfoGiveLock();

    dot1xDebugPortMacInfoShow(lIntIfNum);
    
    if(all == L7_TRUE)
    {
       logicalPortInfo = dot1xLogicalPortInfoGetNextNode(intIfNum, &lIntIfNum);
       if(logicalPortInfo == L7_NULLPTR)
       {
         lIntIfNum = 0;
       }
    }
    else
    {
      lIntIfNum = 0;
    }
  }/*While */
}
   
 

