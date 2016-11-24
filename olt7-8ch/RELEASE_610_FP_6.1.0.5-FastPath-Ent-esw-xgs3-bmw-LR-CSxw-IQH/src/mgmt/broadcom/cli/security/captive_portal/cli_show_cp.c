/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/security/captive_portal/cli_show_cp.c
*
* @purpose Show commands for Captive portal 
*
* @component user interface
*
* @comments none
*
* @create  07/17/2007
*
* @author  rjain, rjindal
*
* @end
*
**********************************************************************/

#include "strlib_security_cli.h"
#include "strlib_security_web.h"
#include "cliapi.h"
#include "l7_common.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_wdm_client_api.h"
#include "ews.h"
#include "clicommands_cp.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpim_api.h"
#include "usmdb_cpdm_user_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "usmdb_cpdm_web_api.h"
#include "clicommands_card.h"
#include "captive_portal_defaultconfig.h"

#ifdef L7_WIRELESS_PACKAGE
 #include "usmdb_wdm_network_api.h"
 #include "usmdb_wdm_client_api.h"
 #include "usmdb_wdm_api.h"
#endif

/*********************************************************************
* @purpose  Display captive portal global information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal 
*
* @cmdhelp  Display captive portal global information.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 val = 0;
  L7_CP_MODE_REASON_t reason;
  L7_CP_MODE_STATUS_t status = L7_CP_MODE_DISABLED;
  L7_IP_ADDR_t ipAddr = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);

  if (cliNumFunctionArgsGet() != 0)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCaptivePortal);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
 
  /* admin mode */
  cliFormat(ewsContext, "Administrative Mode");
  if (usmDbCpdmGlobalModeGet(&val) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val, pStrInfo_common_Line));
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
  }   

  /* CP global status */
  cliFormat(ewsContext, "Operational Status");
  if (usmDbCpdmGlobalStatusModeGet(&status) == L7_SUCCESS)
  {
    switch (status)
    {
    case L7_CP_MODE_ENABLED:
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Enbld);
      break;
    case L7_CP_MODE_ENABLE_PENDING:
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_CPModeEnablePending);
      break;
    case L7_CP_MODE_DISABLED:
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Dsbld);
      break;
    case L7_CP_MODE_DISABLE_PENDING:
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_CPModeDisablePending);
      break;
    default:
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
      break;
    }
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  /* CP global disable reason */
  if ((status == L7_CP_MODE_DISABLED) || (status == L7_CP_MODE_DISABLE_PENDING))
  {
    cliFormat(ewsContext, "Disable Reason");
    if (usmDbCpdmGlobalCPDisableReasonGet(&reason) == L7_SUCCESS)
    {
      switch (reason)
      {
      case L7_CP_MODE_REASON_ADMIN:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_CPModeReasonAdmin);
        break;
      case L7_CP_MODE_REASON_NO_IP_ADDR:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_CPModeReasonNoIpAddr);
        break;
      case L7_CP_MODE_REASON_ROUTING_ENABLED_NO_IP_RT_INTF:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_CPModeReasonRoutingNoIpRoutingIntf);
        break;
      case L7_CP_MODE_REASON_ROUTING_DISABLED:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_CPModeReasonRoutingDisabled);
        break;
      default:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_CPModeReasonAdmin);
        break;
      }
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext,stat);
  }

  /* CP IP Address */
  cliFormat(ewsContext, "CP IP Address");
  memset(stat, 0, sizeof(stat));
  osapiStrncpy(stat, pStrInfo_common_0000, sizeof(stat)-1);
  if ((usmDbCpdmGlobalModeGet(&val) == L7_SUCCESS) && (val == L7_ENABLE))
  {
    if (usmDbCpdmGlobalCPIPAddressGet(&ipAddr) == L7_SUCCESS)
    {
      rc = usmDbInetNtoa(ipAddr, stat);
    }
  }
  ewsTelnetWrite(ewsContext, stat);

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Display captive portal global status
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal status
*
* @cmdhelp  Display captive portal global status.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPStatus(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 val = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);

  if (cliNumFunctionArgsGet() != 0)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCaptivePortalStatus);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /* HTTP Port */
  cliFormat(ewsContext, "Additional HTTP Port");
  if(usmDbCpdmGlobalHTTPPortGet(&val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

#ifdef L7_MGMT_SECURITY_PACKAGE
  /* HTTPS Port */
  cliFormat(ewsContext, "Additional HTTP Secure Port");
  if(usmDbCpdmGlobalHTTPSecurePort2Get(&val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

#endif

  /* statistics interval */
  if (usmDbCpdmClusterSupportGet() == L7_SUCCESS)
  {
    cliFormat(ewsContext, "Peer Switch Statistics Reporting Interval");
    if(usmDbCpdmGlobalStatusPeerSwStatsReportIntervalGet(&val) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat), "%d", val);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext,stat);
  }

  /* authentication timeout */
  cliFormat(ewsContext, "Authentication Timeout");
  if (usmDbCpdmGlobalStatusSessionTimeoutGet(&val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  /* Supported Captive Portals */
  cliFormat(ewsContext, "Supported Captive Portals");
  if (usmDbCpdmGlobalSupportedCPInstancesGet(&val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  /* Configured Captive Portals */
  cliFormat(ewsContext, "Configured Captive Portals");
  if(usmDbCpdmGlobalConfiguredCPInstancesGet(&val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  /* Active CP Instances */
  cliFormat(ewsContext, "Active Captive Portals");
  if(usmDbCpdmGlobalActiveCPInstancesGet(&val)== L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  /* Local supported users */
  cliFormat(ewsContext, "Local Supported Users");
  if(usmDbCpdmGlobalLocalUsersGet(&val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  /* Configured Local users */
  cliFormat(ewsContext, "Configured Local Users");
  if(usmDbCpdmUserEntryCountGet(&val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  /* System supported users */
  cliFormat(ewsContext, "System Supported Users");
  if(usmDbCpdmGlobalSupportedUsersGet(&val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  /* authenticated users */
  cliFormat(ewsContext, "Authenticated Users");
  if(usmDbCpdmGlobalAuthenticatedUsersGet(&val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Display operational status of a captive-portal configuration
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal configuration <cp-id>
*
* @cmdhelp  Display operational status of a captive-portal configuration.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPConfiguration(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 cpId32, val, cnt;
  webId_t webId;
  cpId_t cpId;
  L7_CP_INST_OPER_STATUS_t status;
  L7_CP_INST_DISABLE_REASON_t reason;
  L7_CP_INST_BLOCK_STATUS_t block_status;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];


  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);

  if (cliNumFunctionArgsGet() != 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPConfiguration);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index+1], &cpId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (cpId32 < CP_ID_MIN || cpId32 > CP_ID_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_Invalid_CPIdRange, CP_ID_MIN, CP_ID_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  cpId = (L7_ushort16)cpId32;

  if (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPIdNotFound);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /* CP ID */
  cliFormat(ewsContext, "CP ID");
  osapiSnprintf(stat,sizeof(stat), "%d", cpId);
  ewsTelnetWrite(ewsContext, stat);

  /* CP Name */
  cliFormat(ewsContext, "CP Name");
  memset(stat, 0x00, sizeof(stat));
  if (usmDbCpdmCPConfigNameGet(cpId, stat) != L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* operational status */
  cliFormat(ewsContext, "Operational Status");
  if (usmDbCpdmCPConfigOperStatusGet(cpId, &status) == L7_SUCCESS)
  {
    switch (status)
    {
    case L7_CP_INST_OPER_STATUS_ENABLED:
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Enbld);
      break;
    case L7_CP_INST_OPER_STATUS_DISABLED:
    default:
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Dsbld);
      break;
    }
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* disable reason */
  if (status == L7_CP_INST_OPER_STATUS_DISABLED)
  {
    cliFormat(ewsContext, "Disable Reason");
    if (usmDbCpdmCPConfigDisableReasonGet(cpId, &reason) == L7_SUCCESS)
    {
      switch (reason)
      {
      case L7_CP_INST_DISABLE_REASON_ADMIN:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_CPModeReasonAdmin);
        break;
      case L7_CP_INST_DISABLE_REASON_NO_RADIUS_SERVER:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_NoRadiusServer);
        break;
      case L7_CP_INST_DISABLE_REASON_NO_ACCT_SERVER:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_NoAcctServer);
        break;
      case L7_CP_INST_DISABLE_REASON_NOT_ASSOC_INTF:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_NoAssocWithIntf);
        break;
      case L7_CP_INST_DISABLE_REASON_NO_ACTIVE_INTF:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_NoValidActiveIntf);
        break;
      case L7_CP_INST_DISABLE_REASON_NO_VALID_CERT:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_NoValidCert);
        break;
      default:
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_CPModeReasonAdmin);
        break;
      }
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);
  }

  /* block status */
  cliFormat(ewsContext, "Block Status");
  if (usmDbCpdmCPConfigBlockedStatusGet(cpId, &block_status) == L7_SUCCESS)
  {
    if (block_status == L7_CP_INST_BLOCK_STATUS_BLOCKED_PENDING)
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_BlockedPending);
    }
    else if (block_status == L7_CP_INST_BLOCK_STATUS_BLOCKED)
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Blocked);
    }
    else if (block_status == L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED_PENDING)
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_NotBlockedPending);
    }
    else if (block_status == L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED)
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_security_NotBlocked);
    }
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* locales */
  cliFormat(ewsContext, "Configured Locales");
  cnt = 0;
  webId = 0;
  while (L7_SUCCESS == usmDbCpdmCPConfigWebIdNextGet(cpId,webId,&webId))
  {
    cnt++;
  }
  osapiSnprintf(stat,sizeof(stat), "%d", cnt);
  ewsTelnetWrite(ewsContext, stat);

  /* authenticated users */
  cliFormat(ewsContext, "Authenticated Users");
  if (usmDbCpdmCPConfigAuthenticatedUsersGet(cpId, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat), "%d", val);
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Display Captive Portal configuration client status
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal configuration [cp-id] client status
*
* @cmdhelp  Display Captive Portal configuration client status.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPConfigurationClientStatus(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 intf, u, s, p;
  L7_uint32 cpId32;
  L7_ushort16 cpId;
  L7_char8 name[CP_NAME_MAX+1];
  static L7_enetMacAddr_t macAddr;
  L7_IP_ADDR_t ipAddr;
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_char8 ipBuf[16];
  L7_char8 checkChar;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL all, first = L7_TRUE;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPCfgClientStatus);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (strcmp((L7_char8 *)argv[index-1], "configuration") == 0)
  {
    cpId32 = 0;
    all = L7_TRUE;
  }
  else
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index-1], &cpId32) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if (cpId32 < CP_ID_MIN || cpId32 > CP_ID_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_Invalid_CPIdRange, CP_ID_MIN, CP_ID_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    cpId = (L7_ushort16)cpId32;

    if (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPIdNotFound);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    all = L7_FALSE;
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    checkChar = cliCommon[cliUtil.handleNum].charInput;
    cliClearCharInput();
    cliConvertToLowerCase(&checkChar);
    if ((checkChar == 'q') || (checkChar == 26)) /*ctrl-z*/
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if (all == L7_TRUE)
    {
      cpId = 0;
      if (usmDbCpdmCPConfigNextGet(cpId, &cpId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPIdNotFound);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      ewsTelnetWrite(ewsContext,"\r\nCP ID        CP Name        Client MAC Address Client IP Address Interface");
      ewsTelnetWrite(ewsContext,"\r\n----- --------------------- ------------------ ----------------- ---------");

      while (L7_TRUE)
      {
        cliSyntaxTop(ewsContext);

        /* cp id */
        osapiSnprintf(stat, sizeof(stat), "%-6u", cpId);
        ewsTelnetWrite(ewsContext, stat);

        /* cp name */
        memset(name, 0x00, sizeof(name));
        if (usmDbCpdmCPConfigNameGet(cpId, name) == L7_SUCCESS)
        {
          cliStringTruncate(name, 21);
          osapiSnprintf(stat, sizeof(stat), "%-22s", name);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-22s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* client: mac + ip + intf */
        memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
        while (usmDbCpdmClientConnStatusCPIDClientNextGet(cpId, macAddr, &macAddr) == L7_SUCCESS)
        {
          if (first != L7_TRUE)
          {
            cliSyntaxTop(ewsContext);
            osapiSnprintf(stat, sizeof(stat), "%-28s", pStrInfo_common_EmptyString);
            ewsTelnetWrite(ewsContext, stat);
          }

          /* client mac address */
          osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", 
                        macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
          osapiSnprintf(stat, sizeof(stat), "%-19s", buf);
          ewsTelnetWrite(ewsContext, stat);

          /* client ip address */
          memset(&ipAddr, 0, sizeof(L7_IP_ADDR_t));
          memset(ipBuf, 0, sizeof(ipBuf));
          if ((usmDbCpdmClientConnStatusIpGet(&macAddr, &ipAddr) == L7_SUCCESS) && 
              (usmDbInetNtoa(ipAddr, ipBuf) == L7_SUCCESS))
          {
            osapiSnprintf(stat, sizeof(stat), "%-18s", ipBuf);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-18s", pStrInfo_common_EmptyString);
          }
          ewsTelnetWrite(ewsContext, stat);

          /* client interface */
          if (usmDbCpdmClientConnStatusIntfIdGet(&macAddr, &intf) == L7_SUCCESS)
          {
            if (usmDbUnitSlotPortGet(intf, &u, &s, &p) == L7_SUCCESS)
            {
              osapiSnprintf(stat, sizeof(stat), "%-9s", cliDisplayInterfaceHelp(u, s, p));
            }
            else
            {
              osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_EmptyString);
            }
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_EmptyString);
          }
          ewsTelnetWrite(ewsContext, stat);
          first = L7_FALSE;
        }

        if (usmDbCpdmCPConfigNextGet(cpId, &cpId) != L7_SUCCESS)
        {
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        first = L7_TRUE;
      }
    }
    else if (all != L7_TRUE)
    {
      /* cp id */
      cliFormat(ewsContext, "CP ID");
      osapiSnprintf(stat, sizeof(stat), "%u", cpId);
      ewsTelnetWrite(ewsContext, stat);

      /* cp name */
      cliFormat(ewsContext, "CP Name");
      memset(name, 0x00, sizeof(name));
      if (usmDbCpdmCPConfigNameGet(cpId, name) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", name);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      cliSyntaxTop(ewsContext);
      memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
      if (usmDbCpdmClientConnStatusCPIDClientNextGet(cpId, macAddr, &macAddr) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_NoCPClientsExist);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);

      ewsTelnetWrite(ewsContext,"\r\n     Client           Client                                                   ");
      ewsTelnetWrite(ewsContext,"\r\n   MAC Address      IP Address    Interface        Interface Description       ");
      ewsTelnetWrite(ewsContext,"\r\n----------------- --------------- --------- -----------------------------------");

      while (L7_TRUE)
      {
        cliSyntaxTop(ewsContext);

        /* client mac address */
        osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", 
                      macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
        osapiSnprintf(stat, sizeof(stat), "%-18s", buf);
        ewsTelnetWrite(ewsContext, stat);

        /* client ip address */
        memset(&ipAddr, 0, sizeof(L7_IP_ADDR_t));
        memset(ipBuf, 0, sizeof(ipBuf));
        if ((usmDbCpdmClientConnStatusIpGet(&macAddr, &ipAddr) == L7_SUCCESS) && 
            (usmDbInetNtoa(ipAddr, ipBuf) == L7_SUCCESS))
        {
          osapiSnprintf(stat, sizeof(stat), "%-16s", ipBuf);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* client interface */
        if (usmDbCpdmClientConnStatusIntfIdGet(&macAddr, &intf) == L7_SUCCESS)
        {
          if (usmDbUnitSlotPortGet(intf, &u, &s, &p) == L7_SUCCESS)
          {
            osapiSnprintf(stat, sizeof(stat), "%-10s", cliDisplayInterfaceHelp(u, s, p));
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_common_EmptyString);
          }
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* interface description */
        if (usmDbCpimIntfCapabilityDescriptionGet(intf, ifDescription) == L7_SUCCESS)
        {
          cliStringTruncate(ifDescription, 35);
          osapiSnprintf(stat, sizeof(stat), "%-35s", ifDescription);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-35s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        if (usmDbCpdmClientConnStatusCPIDClientNextGet(cpId, macAddr, &macAddr) != L7_SUCCESS)
        {
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  if (all == L7_TRUE)
  {
    cliAlternateCommandSet(pStrInfo_security_ShowCPCfgClientAllCmd);
  }
  else if (all != L7_TRUE)
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_security_ShowCPCfgClientCmd, cpId);
    cliAlternateCommandSet(stat);
  }
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*********************************************************************
* @purpose  Display Captive Portal configuration interface information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal configuration <cp-id> interface [intf]
*
* @cmdhelp  Display Captive Portal configuration interface information.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPConfigurationIntf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 cpId32, val, u=0, s=0, p=0, intf=0;
  cpId_t cpId, cpId_next;
  L7_BOOL status;
  L7_INTF_WHY_DISABLED_t reason;
  L7_CP_INST_BLOCK_STATUS_t blockStatus;
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);

  if (cliNumFunctionArgsGet() > 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPConfigurationIntf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &cpId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (cpId32 < CP_ID_MIN || cpId32 > CP_ID_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_Invalid_CPIdRange, CP_ID_MIN, CP_ID_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  cpId = (L7_ushort16)cpId32;

  if (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPIdNotFound);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliNumFunctionArgsGet() == 0)
  {
    /* CP ID */
    cliFormat(ewsContext, "CP ID");
    osapiSnprintf(stat, sizeof(stat), "%d", cpId);
    ewsTelnetWrite(ewsContext, stat);

    /* CP Name */
    cliFormat(ewsContext, "CP Name");
    memset (stat, 0, sizeof(stat));
    if (usmDbCpdmCPConfigNameGet(cpId, stat) != L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    cliSyntaxBottom(ewsContext);

    intf = 0;
    if ((usmDbCpdmCPConfigIntIfNumNextGet(cpId, intf, &cpId_next, &intf) != L7_SUCCESS) || (cpId != cpId_next))
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPNoIntf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    ewsTelnetWrite(ewsContext,"\r\n                                                   Operational    Block    ");
    ewsTelnetWrite(ewsContext,"\r\nInterface        Interface Description                Status      Status   ");
    ewsTelnetWrite(ewsContext,"\r\n--------- ---------------------------------------- ------------ -----------");

    do
    {
      cliSyntaxTop(ewsContext);

      /* interface */
      if (usmDbUnitSlotPortGet(intf, &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-10s", cliDisplayInterfaceHelp(u, s, p));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* interface description */
      if (usmDbCpimIntfCapabilityDescriptionGet(intf, ifDescription) == L7_SUCCESS)
      {
        cliStringTruncate(ifDescription, 40);
        osapiSnprintf(stat, sizeof(stat), "%-41s", ifDescription);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-41s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* operational status */
      if (usmDbCpdmIntfStatusGetIsEnabled(intf, &status) == L7_SUCCESS)
      {
        if (status == L7_TRUE)
        {
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_common_Enbld);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_common_Dsbld);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-13s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* block status */
      if (usmDbCpimIntfCapabilityBlockStatusGet(intf, &blockStatus) == L7_SUCCESS)
      {
        if (blockStatus == L7_CP_INST_BLOCK_STATUS_BLOCKED)
        {
          osapiSnprintf(stat, sizeof(stat), "%-11s", pStrInfo_security_Blocked);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-11s", pStrInfo_security_NotBlocked);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-11s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

    } while((usmDbCpdmCPConfigIntIfNumNextGet(cpId, intf, &cpId_next, &intf) == L7_SUCCESS) && (cpId == cpId_next));
  }
  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
       if ((rc = cliValidSpecificUSPCheck(argv[index+1], &u, &s, &p)) != L7_SUCCESS)
       {
         if (rc == L7_ERROR)
         {
           ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
           ewsTelnetPrintf (ewsContext, "%u/%u/%u", u, s, p);
         }
         else if (rc == L7_NOT_EXIST)
         {
           ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
         }
         else if (rc == L7_NOT_SUPPORTED)
         {
           ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
         }
         else
         {
           ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
         }

         return cliSyntaxReturnPrompt (ewsContext, "");
       }

       /* Get interface and check its validity */
       if (usmDbIntIfNumFromUSPGet(u, s, p, &intf) != L7_SUCCESS)
       {
         ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
         return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", u, s, p);
       }
    }
    else
    {
       /* NOTE: No need to check the unit value as ID of a standalone switch is always 'U_IDX' (=> 1) */
       if (cliSlotPortToIntNum(ewsContext, argv[index+1], &s, &p, &intf) != L7_SUCCESS)
       {
         return cliPrompt(ewsContext);
       }
    }

    if (usmDbCpimIntfGet(intf) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPIntfDoesNotExist);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (usmDbCpdmCPConfigIntIfNumGet(cpId, intf) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgInterfaceNotAssoc);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* CP ID */
    cliFormat(ewsContext, "CP ID");
    osapiSnprintf(stat, sizeof(stat), "%d", cpId);
    ewsTelnetWrite(ewsContext, stat);

    /* CP Name */
    cliFormat(ewsContext, "CP Name");
    memset (stat, 0, sizeof(stat));
    if (usmDbCpdmCPConfigNameGet(cpId, stat) != L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* interface */
    cliFormat(ewsContext, "Interface");
    osapiSnprintf(stat, sizeof(stat), "%s", cliDisplayInterfaceHelp(u, s, p));
    ewsTelnetWrite(ewsContext, stat);

    /* interface description */
    cliFormat(ewsContext, "Interface Description");
    if (usmDbCpimIntfCapabilityDescriptionGet(intf, ifDescription) == L7_SUCCESS)
    {
      cliStringTruncate(ifDescription, 32);
      osapiSnprintf(stat, sizeof(stat), "%s", ifDescription);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* operational status */
    cliFormat(ewsContext, "Operational Status");
    if (usmDbCpdmIntfStatusGetIsEnabled(intf, &status) == L7_SUCCESS)
    {
      if (status == L7_TRUE)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Enbld);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Dsbld);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* disable reason */
    if (status == L7_FALSE)
    {
      cliFormat(ewsContext, "Disable Reason");
      if (usmDbCpdmIntfStatusGetWhy(intf, &reason) == L7_SUCCESS)
      {
        switch (reason)
        {
        case L7_INTF_WHY_DISABLED_ADMIN:
          osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_CPModeReasonAdmin);
          break;
        case L7_INTF_WHY_DISABLED_UNATTACHED:
          osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_IntfNotAttached);
          break;
        default:
          osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_CPModeReasonAdmin);
          break;
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);
    }

    /* block status */
    cliFormat(ewsContext, "Block Status");
    if (usmDbCpimIntfCapabilityBlockStatusGet(intf, &blockStatus) == L7_SUCCESS)
    {
      if (blockStatus == L7_CP_INST_BLOCK_STATUS_BLOCKED)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Blocked);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_NotBlocked);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* authenticated users */
    cliFormat(ewsContext, "Authenticated Users");
    if (usmDbCpdmIntfStatusGetUsers(intf, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);
  }

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Display Captive Portal configuration status
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal configuration [cp-id] status
*
* @cmdhelp  Display Captive Portal configuration status.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPConfigurationStatus(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 cpId32=0, value;
  cpId_t cpId=0;
  L7_CP_MODE_STATUS_t mode;
  L7_LOGIN_TYPE_t protocol;
  CP_VERIFY_MODE_t verifyMode;
  gpId_t gId;
  L7_uchar8 val;
  L7_char8 name[L7_CLI_MAX_STRING_LENGTH+1];
  L7_char8 urlBuf[CP_WELCOME_URL_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);

  if (cliNumFunctionArgsGet() != 0)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPConfigurationStatus);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (strcmp((L7_char8 *)argv[index-1], "configuration") == 0)
  {
    ewsTelnetWrite(ewsContext,"\r\nCP ID       CP Name         Mode   Protocol Verification");
    ewsTelnetWrite(ewsContext,"\r\n----- -------------------- ------- -------- ------------");

    while (usmDbCpdmCPConfigNextGet(cpId, &cpId) == L7_SUCCESS)
    {
      cliSyntaxTop(ewsContext);

      /* CP ID */
      osapiSnprintf(stat,sizeof(stat), "%-6u", cpId);
      ewsTelnetWrite(ewsContext, stat);

      /* CP Name */
      memset(name, 0x00, sizeof(name));
      if (usmDbCpdmCPConfigNameGet(cpId, name) == L7_SUCCESS)
      {
        cliStringTruncate(name, 20);
        osapiSnprintf(stat, sizeof(stat), "%-21s", name);
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%-21s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* mode */
      if (usmDbCpdmCPConfigModeGet(cpId, &mode) == L7_SUCCESS)
      {
        if (mode == L7_CP_MODE_ENABLED)
        {
          osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_common_Enbl_1);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_common_Dsbl_1);
        }
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%-8s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* protocol */
      if (usmDbCpdmCPConfigProtocolModeGet(cpId, &protocol) == L7_SUCCESS)
      {
        if (protocol == L7_LOGIN_TYPE_HTTP)
        {
          osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_security_ProtocolHTTP);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_security_ProtocolHTTPS);
        }
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%-9s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* verification */
      if (usmDbCpdmCPConfigVerifyModeGet(cpId, &verifyMode) == L7_SUCCESS)
      {
        if (verifyMode == CP_VERIFY_MODE_GUEST)
        {
          osapiSnprintf(stat, sizeof(stat), "%-12s", pStrInfo_security_VerificationModeGuest);
        }
        else if (verifyMode == CP_VERIFY_MODE_LOCAL)
        {
          osapiSnprintf(stat, sizeof(stat), "%-12s", pStrInfo_security_VerificationModeLocal);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-12s", pStrInfo_security_VerificationModeRadius);
        }
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%-12s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);
    }
  }

  else
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index-1], &cpId32) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if (cpId32 < CP_ID_MIN || cpId32 > CP_ID_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_Invalid_CPIdRange, CP_ID_MIN, CP_ID_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    cpId = (L7_ushort16)cpId32;

    if (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPIdNotFound);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* CP ID */
    cliFormat(ewsContext, "CP ID");
    osapiSnprintf(stat, sizeof(stat), "%d", cpId);
    ewsTelnetWrite(ewsContext, stat);

    /* CP Name */
    cliFormat(ewsContext, "CP Name");
    memset(stat, 0x00, sizeof(stat));
    if (usmDbCpdmCPConfigNameGet(cpId, stat) == L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, stat);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
    }

    /* Captive Portal Mode */
    cliFormat(ewsContext, "CP Mode");
    if (usmDbCpdmCPConfigModeGet(cpId, &mode) == L7_SUCCESS)
    {
      if (mode == L7_CP_MODE_ENABLED)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Enbl_1);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Dsbl_1);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* protocol mode */
    cliFormat(ewsContext, "Protocol Mode");
    if (usmDbCpdmCPConfigProtocolModeGet(cpId, &protocol) == L7_SUCCESS)
    {
      if (protocol == L7_LOGIN_TYPE_HTTP)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_ProtocolHTTP);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_ProtocolHTTPS);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* verification */
    cliFormat(ewsContext, "Verification Mode");
    if (usmDbCpdmCPConfigVerifyModeGet(cpId, &verifyMode) == L7_SUCCESS)
    {
      if (verifyMode == CP_VERIFY_MODE_GUEST)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_VerificationModeGuest);
      }
      else if (verifyMode == CP_VERIFY_MODE_LOCAL)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_VerificationModeLocal);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_VerificationModeRadius);
      }
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* RADIUS authentication server */
    if (verifyMode == CP_VERIFY_MODE_RADIUS)
    {
      cliFormat(ewsContext, "RADIUS authentication server");
      memset(stat, 0x00, sizeof(stat));
      if (usmDbCpdmCPConfigRadiusAuthServerGet(cpId, stat) == L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, stat);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
      }
    }

    /* group */
    if (verifyMode == CP_VERIFY_MODE_LOCAL)
    {
      cliFormat(ewsContext, "Group ID");
      if (usmDbCpdmCPConfigGpIdGet(cpId, &gId) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%d", gId);
        ewsTelnetWrite(ewsContext, stat);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
      }

      cliFormat(ewsContext, "Group Name");
      memset(stat, 0x00, sizeof(stat));
      if (usmDbCpdmUserGroupEntryNameGet(gId, stat) == L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, stat);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
      }
    }

    /* user logout mode */
    cliFormat(ewsContext, "User Logout Mode");
    if (usmDbCpdmCPConfigUserLogoutModeGet(cpId, &val) == L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, strUtilEnableDisableGet(val, pStrInfo_common_Line));
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
    }   

    /* redirect mode */
    cliFormat(ewsContext, "URL Redirect Mode");
    if (usmDbCpdmCPConfigRedirectModeGet(cpId, &val) == L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, strUtilEnableDisableGet(val, pStrInfo_common_Line));
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
    }   

    /* redirect URL */
    if (val == L7_ENABLE)
    {
      cliFormat(ewsContext, "Redirect URL");
      memset(urlBuf, 0x00, sizeof(urlBuf));
      if (usmDbCpdmCPConfigRedirectURLGet(cpId, urlBuf) == L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, urlBuf);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
      }
    }

    /* session timeout */
    cliFormat(ewsContext, "Session Timeout");
    if (usmDbCpdmCPConfigSessionTimeoutGet(cpId, &value) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat), "%u", value);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* idle timeout */
    cliFormat(ewsContext, "Idle Timeout");
    if (usmDbCpdmCPConfigIdleTimeoutGet(cpId, &value) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat), "%d", value);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* max bandwidth up */
    cliFormat(ewsContext, "Max Bandwidth Up (bytes/sec)");
    if (usmDbCpdmCPConfigUserUpRateGet(cpId, &value) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat), "%u", value/8); /* convert from bits to bytes */
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* max bandwidth down */
    cliFormat(ewsContext, "Max Bandwidth Down (bytes/sec)");
    if (usmDbCpdmCPConfigUserDownRateGet(cpId, &value) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat), "%u", value/8); /* convert from bits to bytes */
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* max input octets */
    cliFormat(ewsContext, "Max Input Octets (bytes)");
    if (usmDbCpdmCPConfigMaxInputOctetsGet(cpId, &value) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat), "%u", value);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* max output octets */
    cliFormat(ewsContext, "Max Output Octets (bytes)");
    if (usmDbCpdmCPConfigMaxOutputOctetsGet(cpId, &value) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat), "%u", value);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%u", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* max total octets */
    cliFormat(ewsContext, "Max Total Octets (bytes)");
    if (usmDbCpdmCPConfigMaxTotalOctetsGet(cpId, &value) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat), "%u", value);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

  }

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Display Captive Portal configuration locales
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal configuration <cp-id> locales
*
* @cmdhelp  Display Captive Portal configuration locales.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPConfigurationLocales(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_char8 langCode[CP_LANG_CODE_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 cpId32=0;
  cpId_t cpId=0;
  webId_t webId;

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);

  if (cliNumFunctionArgsGet() != 0)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPConfigurationLocales);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &cpId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (cpId32 < CP_ID_MIN || cpId32 > CP_ID_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_Invalid_CPIdRange, CP_ID_MIN, CP_ID_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  cpId = (L7_ushort16)cpId32;

  if (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPIdNotFound);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  ewsTelnetWrite(ewsContext,"\r\nLocale Code    ");
  ewsTelnetWrite(ewsContext,"\r\n---------------");

  webId = 0;
  while (L7_SUCCESS == usmDbCpdmCPConfigWebIdNextGet(cpId,webId,&webId))
  {
    cliSyntaxTop(ewsContext);

    /* language code */
    memset(langCode, 0, sizeof(langCode));
    if (usmDbCpdmCPConfigWebLangCodeGet(cpId, webId, langCode) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%-15s", langCode);
    }  
    ewsTelnetWrite(ewsContext, stat);
  }

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Display Captive Portal client information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal client [macaddr] status
*
* @cmdhelp  Display Captive Portal client information.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPClientStatus(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 value, i, intf, u, s, p;
  L7_uint32 count;
  cpId_t cpId=0;
  static L7_enetMacAddr_t macAddr;
  L7_IP_ADDR_t ipAddr;
  L7_char8 ipBuf[16];
  L7_uchar8 userName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_LOGIN_TYPE_t protocol;
  CP_VERIFY_MODE_t verifyMode;
  L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL clientLocal = L7_FALSE;
  L7_BOOL fIsController = L7_FALSE;
#ifdef L7_WIRELESS_PACKAGE
  L7_BOOL widsInd = L7_FALSE;
#endif

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_security_ShowCPClientStatus);
  }

  if (strcmp((L7_char8 *)argv[index-1], "client") == 0)
  {
    cliCmdScrollSet(L7_FALSE);

    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth (ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt (ewsContext);
      }
    }
    else
    {
      memset(&macAddr, 0x00, sizeof(macAddr));
      if (usmDbCpdmClientConnStatusNextGet(&macAddr, &macAddr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                              pStrErr_security_NoCPClientsExist);
      }
    }

#ifdef L7_WIRELESS_PACKAGE
    fIsController = ((usmDbWdmWidsControllerIndGet (&widsInd) == L7_SUCCESS) && (widsInd == L7_TRUE)) ? L7_TRUE : L7_FALSE;
#endif

    /* Heading for the client status */
    if (fIsController == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext,"\r\n      MAC Address                                         Verify             ");
      ewsTelnetWrite(ewsContext,"\r\n(*)Peer Authenticated   IP Address    User Name  Protocol  Mode  Session Time");
      ewsTelnetWrite(ewsContext,"\r\n--------------------- --------------- ---------- -------- ------ ------------");
    }
    else
    {
      ewsTelnetWrite(ewsContext,"\r\n                                                      Verify             ");
      ewsTelnetWrite(ewsContext,"\r\n   MAC Address      IP Address    User Name  Protocol  Mode  Session Time");
      ewsTelnetWrite(ewsContext,"\r\n----------------- --------------- ---------- -------- ------ ------------");
    }

    for (count = 0; count < CLI_MAX_SCROLL_LINES-4; count++)
    {
      clientLocal = L7_TRUE; /* Keep compiler quiet for non-wireless */

      cliSyntaxTop(ewsContext);

      /* client MAC Address */
      osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", 
                    macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], 
                    macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);


      if (fIsController == L7_TRUE)
      {
#ifdef L7_WIRELESS_PACKAGE
        if ((usmDbWdmAssocClientLocalGet (macAddr, &clientLocal) == L7_SUCCESS) && (clientLocal == L7_TRUE))
        {
          ewsTelnetPrintf (ewsContext, "%-1s", pStrInfo_common_Space);
        }
        else
#endif
        {
          ewsTelnetPrintf (ewsContext, "%-1s", pStrInfo_common_Asterix);
        }
        osapiSnprintf(stat, sizeof(stat), "%-21s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-18s", buf);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* client ip address */
      memset(&ipAddr, 0, sizeof(L7_IP_ADDR_t));
      memset(ipBuf, 0, sizeof(ipBuf));
      if ((usmDbCpdmClientConnStatusIpGet(&macAddr, &ipAddr) == L7_SUCCESS) && 
          (usmDbInetNtoa(ipAddr, ipBuf) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat), "%-16s", ipBuf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* user name */
      memset(userName, 0x00, sizeof(userName));
      if (usmDbCpdmClientConnStatusUserNameGet(&macAddr, userName) == L7_SUCCESS)
      {
        cliStringTruncate(userName, 10);
        osapiSnprintf(stat, sizeof(stat), "%-11s", userName);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-11s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* protocol */
      if (usmDbCpdmClientConnStatusProtocolModeGet(&macAddr, &protocol) == L7_SUCCESS)
      {
        if (protocol == L7_LOGIN_TYPE_HTTP)
        {
          osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_security_ProtocolHTTP);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_security_ProtocolHTTPS);
        }
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%-9s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* verify mode */
      if (usmDbCpdmClientConnStatusVerifyModeGet(&macAddr, &verifyMode) == L7_SUCCESS)
      {
        if (verifyMode == CP_VERIFY_MODE_GUEST)
        {
          osapiSnprintf(stat, sizeof(stat), "%-7s", pStrInfo_security_VerificationModeGuest);
        }
        else if (verifyMode == CP_VERIFY_MODE_LOCAL)
        {
          osapiSnprintf(stat, sizeof(stat), "%-7s", pStrInfo_security_VerificationModeLocal);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-7s", pStrInfo_security_VerificationModeRadius);
        }
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%-7s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* Session timeout */
      if (usmDbCpdmClientConnStatusSessionTimeGet(&macAddr, &value) == L7_SUCCESS)
      {
        memset(stat, 0x00, sizeof(stat));
        usmDbTimeToStringConvert(value, stat, sizeof(stat));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-12s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      if (usmDbCpdmClientConnStatusNextGet(&macAddr, &macAddr) != L7_SUCCESS)
      {
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    } /* endfor count */

    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), "%s", argv[0]);

    for (i=1; i<argc; i++)
    {
      osapiStrncat(cmdBuf, " ", sizeof(cmdBuf)-1-strlen(cmdBuf));
      osapiStrncat(cmdBuf, argv[i], sizeof(cmdBuf)-1-strlen(cmdBuf));
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
  }

  else
  {
    memset(&macAddr, 0x00, sizeof(macAddr));
    if (cliConvertMac((L7_char8 *)argv[index-1], macAddr.addr) != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidMacAddr);
    }

    if (usmDbCpdmClientConnStatusGet(&macAddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_security_CPClientDoesNotExist);
    }

    /* Client MAC Address */
    cliFormat(ewsContext, "Client MAC Address");
    osapiSnprintf(stat, sizeof(stat), "%02X:%02X:%02X:%02X:%02X:%02X", 
                  macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], 
                  macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
    ewsTelnetWrite(ewsContext, stat);

    /* IP Address */
    cliFormat(ewsContext, "Client IP Address");
    memset(ipBuf, 0x00, sizeof(ipBuf));
    memset(&ipAddr, 0, sizeof(L7_IP_ADDR_t));
    if ((usmDbCpdmClientConnStatusIpGet(&macAddr, &ipAddr) == L7_SUCCESS) && 
        (usmDbInetNtoa(ipAddr, ipBuf) == L7_SUCCESS))
    {
      ewsTelnetWrite(ewsContext, ipBuf);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
    }

    /* protocol */
    cliFormat(ewsContext, "Protocol Mode");
    if (usmDbCpdmClientConnStatusProtocolModeGet(&macAddr, &protocol) == L7_SUCCESS)
    {
      if (protocol == L7_LOGIN_TYPE_HTTP)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_ProtocolHTTP);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_ProtocolHTTPS);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* verification */
    cliFormat(ewsContext, "Verification Mode");
    if (usmDbCpdmClientConnStatusVerifyModeGet(&macAddr, &verifyMode) == L7_SUCCESS)
    {
      if (verifyMode == CP_VERIFY_MODE_GUEST)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_VerificationModeGuest);
      }
      else if (verifyMode == CP_VERIFY_MODE_LOCAL)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_VerificationModeLocal);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_VerificationModeRadius);
      }
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* CP ID */
    cliFormat(ewsContext, "CP ID");
    if (usmDbCpdmClientConnStatusCpIdGet(&macAddr, &cpId) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%u", cpId);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* CP Name */
    cliFormat(ewsContext, "CP Name");
    memset(buf, 0, sizeof(buf));
    if (usmDbCpdmCPConfigNameGet(cpId, buf) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%s", buf);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);
    
    /* interface */
    cliFormat(ewsContext, "Interface");
    if (usmDbCpdmClientConnStatusIntfIdGet(&macAddr, &intf) == L7_SUCCESS)
    {
      if (usmDbUnitSlotPortGet(intf, &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", cliDisplayInterfaceHelp(u, s, p));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* interface description */
    cliFormat(ewsContext, "Interface Description");
    if (usmDbCpimIntfCapabilityDescriptionGet(intf, ifDescription) == L7_SUCCESS)
    {
      cliStringTruncate(ifDescription, 32);
      osapiSnprintf(stat, sizeof(stat), "%s", ifDescription);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* user name */
    cliFormat(ewsContext, "User Name");
    if (usmDbCpdmClientConnStatusUserNameGet(&macAddr, userName) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%s", userName);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* session timeout */
    cliFormat(ewsContext, "Session Timeout");
    if (usmDbCpdmClientConnStatusSessionTimeGet(&macAddr, &value) == L7_SUCCESS)
    {
      memset(stat, 0x00, sizeof(stat));
      usmDbTimeToStringConvert(value, stat, sizeof(stat));
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    if (usmDbCpdmClusterSupportGet() == L7_SUCCESS)
    {
      L7_enetMacAddr_t swMac;
      cpdmAuthWhichSwitch_t swType;

      /* switch MAC address */
      cliFormat(ewsContext, "Switch MAC Address");
      memset(&swMac, 0x00, sizeof(swMac));
      if (usmDbCpdmClientConnStatusSwitchMacAddrGet(&macAddr, &swMac) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%02X:%02X:%02X:%02X:%02X:%02X", 
                      swMac.addr[0], swMac.addr[1], swMac.addr[2], 
                      swMac.addr[3], swMac.addr[4], swMac.addr[5]);
        ewsTelnetWrite(ewsContext, stat);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
      }

      /* switch IP address */
      cliFormat(ewsContext, "Switch IP Address");
      memset(ipBuf, 0x00, sizeof(ipBuf));
      memset(&ipAddr, 0, sizeof(L7_IP_ADDR_t));
      if ((usmDbCpdmClientConnStatusSwitchIpGet(&macAddr, &ipAddr) == L7_SUCCESS) && 
          (usmDbInetNtoa(ipAddr, ipBuf) == L7_SUCCESS))
      {
        ewsTelnetWrite(ewsContext, ipBuf);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
      }

      /* switch type */
      cliFormat(ewsContext, "Switch Type");
      if (usmDbCpdmClientConnStatusWhichSwitchGet(&macAddr, &swType) == L7_SUCCESS)
      {
        if (swType == CP_AUTH_ON_LOCAL_SWITCH)
        {
          ewsTelnetWrite(ewsContext, "Local");
        }
        else
        {
          ewsTelnetWrite(ewsContext, "Peer");
        }
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
      }
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
* @purpose  Display Captive Portal client statistics information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal client <macaddr> statistics
*
* @cmdhelp  Display Captive Portal client statistics information.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPClientStatistics(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_enetMacAddr_t macAddr;
  L7_uint64 bTx, bRx, pTx, pRx;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);

  if (cliNumFunctionArgsGet() != 0)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPClientStatsSyntax);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  memset(&macAddr, 0x00, sizeof(macAddr));
  if (cliConvertMac((L7_uchar8 *)argv[index-1], macAddr.addr) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidMacAddr);
  }

  if (usmDbCpdmClientConnStatusGet(&macAddr) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPClientNotFound);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /* Client MAC Address */
  cliFormat(ewsContext, "Client MAC Address");
  osapiSnprintf(stat, sizeof(stat), "%02X:%02X:%02X:%02X:%02X:%02X", 
                macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], 
                macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
  ewsTelnetWrite(ewsContext, stat);

  if (usmDbCpdmClientConnStatusStatisticsGet(&macAddr, &bTx, &bRx, &pTx, &pRx) == L7_SUCCESS)
  {
    /* bytes received */
    cliFormat(ewsContext, "Bytes Received");
    osapiSnprintf(stat, sizeof(stat), "%-20llu", bRx);
    ewsTelnetWrite(ewsContext,stat);

    /* bytes transmitted */
    cliFormat(ewsContext, "Bytes Transmitted");
    osapiSnprintf(stat, sizeof(stat), "%-20llu", bTx);
    ewsTelnetWrite(ewsContext,stat);

    /* packets received */
    cliFormat(ewsContext, "Packets Received");
    osapiSnprintf(stat, sizeof(stat), "%-20llu", pRx);
    ewsTelnetWrite(ewsContext,stat);

    /* packets transmitted */
    cliFormat(ewsContext, "Packets Transmitted");
    osapiSnprintf(stat, sizeof(stat), "%-20llu", pTx);
    ewsTelnetWrite(ewsContext,stat);
  }

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Display Captive Portal interface client status
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal interface [intf] client status
*
* @cmdhelp  Display Captive Portal client interface client status.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPIntfClientStatus(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 intIfNum, u, s, p;
  static L7_enetMacAddr_t macAddr;
  L7_IP_ADDR_t ipAddr;
  cpId_t cpId;
  L7_LOGIN_TYPE_t protocol;
  CP_VERIFY_MODE_t verifyMode;
  L7_char8 ipBuf[16];
  L7_char8 checkChar;
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL all, first = L7_TRUE;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPIntfClientStatus);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (strcmp((L7_char8 *)argv[index-1], "interface") == 0)
  {
    intIfNum = 0;
    all = L7_TRUE;
  }
  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
       if ((rc = cliValidSpecificUSPCheck(argv[index-1], &u, &s, &p)) != L7_SUCCESS)
       {
         if (rc == L7_ERROR)
         {
           ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
           ewsTelnetPrintf (ewsContext, "%u/%u/%u", u, s, p);
         }
         else if (rc == L7_NOT_EXIST)
         {
           ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
         }
         else if (rc == L7_NOT_SUPPORTED)
         {
           ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
         }
         else
         {
           ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
         }

         return cliSyntaxReturnPrompt (ewsContext, "");
       }

       /* Get interface and check its validity */
       if (usmDbIntIfNumFromUSPGet(u, s, p, &intIfNum) != L7_SUCCESS)
       {
         ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
         return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", u, s, p);
       }
    }
    else
    {
       /* NOTE: No need to check the unit value as ID of a standalone switch is always 'U_IDX' (=> 1) */
       if (cliSlotPortToIntNum(ewsContext, argv[index-1], &s, &p, &intIfNum) != L7_SUCCESS)
       {
         return cliPrompt(ewsContext);
       }
    }

    if (usmDbCpimIntfGet(intIfNum) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPIntfDoesNotExist);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    all = L7_FALSE;
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    checkChar = cliCommon[cliUtil.handleNum].charInput;
    cliClearCharInput();
    cliConvertToLowerCase(&checkChar);
    if ((checkChar == 'q') || (checkChar == 26)) /*ctrl-z*/
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if (all == L7_TRUE)
    {
      intIfNum = 0;
      if (usmDbCpimIntfNextGet(intIfNum, &intIfNum) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPIntfDoesNotExist);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      ewsTelnetWrite(ewsContext,"\r\n                                                    Client          Client     ");
      ewsTelnetWrite(ewsContext,"\r\nInterface        Interface Description           MAC Address      IP Address   ");
      ewsTelnetWrite(ewsContext,"\r\n--------- ----------------------------------- ----------------- ---------------");

      while (L7_TRUE)
      {
        cliSyntaxTop(ewsContext);

        /* interface */
        if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-10s", cliDisplayInterfaceHelp(u, s, p));
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* interface description */
        if (usmDbCpimIntfCapabilityDescriptionGet(intIfNum, ifDescription) == L7_SUCCESS)
        {
          cliStringTruncate(ifDescription, 35);
          osapiSnprintf(stat, sizeof(stat), "%-36s", ifDescription);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-36s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* client: mac + ip */
        memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
        while (usmDbCpdmClientConnStatusIntfClientNextGet(intIfNum, macAddr, &macAddr) == L7_SUCCESS)
        {
          if (first != L7_TRUE)
          {
            cliSyntaxTop(ewsContext);
            osapiSnprintf(stat, sizeof(stat), "%-46s", pStrInfo_common_EmptyString);
            ewsTelnetWrite(ewsContext, stat);
          }

          /* client mac address */
          osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", 
                        macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
          osapiSnprintf(stat, sizeof(stat), "%-18s", buf);
          ewsTelnetWrite(ewsContext, stat);

          /* client ip address */
          memset(&ipAddr, 0, sizeof(L7_IP_ADDR_t));
          memset(ipBuf, 0, sizeof(ipBuf));
          if ((usmDbCpdmClientConnStatusIpGet(&macAddr, &ipAddr) == L7_SUCCESS) && 
              (usmDbInetNtoa(ipAddr, ipBuf) == L7_SUCCESS))
          {
            osapiSnprintf(stat, sizeof(stat), "%-15s", ipBuf);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-15s", pStrInfo_common_EmptyString);
          }
          ewsTelnetWrite(ewsContext, stat);
          first = L7_FALSE;
        }

        if (usmDbCpimIntfNextGet(intIfNum, &intIfNum) != L7_SUCCESS)
        {
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        first = L7_TRUE;
      }
    }
    else if (all != L7_TRUE)
    {
      /* interface */
      cliFormat(ewsContext, "Interface");
      if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", cliDisplayInterfaceHelp(u, s, p));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* interface description */
      cliFormat(ewsContext, "Interface Description");
      if (usmDbCpimIntfCapabilityDescriptionGet(intIfNum, ifDescription) == L7_SUCCESS)
      {
        cliStringTruncate(ifDescription, 32);
        osapiSnprintf(stat, sizeof(stat), "%s", ifDescription);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      cliSyntaxTop(ewsContext);
      memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
      if (usmDbCpdmClientConnStatusIntfClientNextGet(intIfNum, macAddr, &macAddr) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_NoCPClientsExist);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);

      ewsTelnetWrite(ewsContext,"\r\n     Client           Client                                                   ");
      ewsTelnetWrite(ewsContext,"\r\n   MAC Address      IP Address    CP ID      CP Name      Protocol Verification");
      ewsTelnetWrite(ewsContext,"\r\n----------------- --------------- ----- ----------------- -------- ------------");

      while (L7_TRUE)
      {
        cliSyntaxTop(ewsContext);

        /* client mac address */
        osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", 
                      macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
        osapiSnprintf(stat, sizeof(stat), "%-18s", buf);
        ewsTelnetWrite(ewsContext, stat);

        /* client ip address */
        memset(&ipAddr, 0, sizeof(L7_IP_ADDR_t));
        memset(ipBuf, 0, sizeof(ipBuf));
        if ((usmDbCpdmClientConnStatusIpGet(&macAddr, &ipAddr) == L7_SUCCESS) && 
            (usmDbInetNtoa(ipAddr, ipBuf) == L7_SUCCESS))
        {
          osapiSnprintf(stat, sizeof(stat), "%-16s", ipBuf);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* cp id */
        if (usmDbCpdmClientConnStatusCpIdGet(&macAddr, &cpId) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-6u", cpId);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-6s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* cp name */
        memset(buf, 0x00, sizeof(buf));
        if (usmDbCpdmCPConfigNameGet(cpId, buf) == L7_SUCCESS)
        {
          cliStringTruncate(buf, 17);
          osapiSnprintf(stat, sizeof(stat), "%-18s", buf);
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat), "%-18s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* protocol */
        if (usmDbCpdmCPConfigProtocolModeGet(cpId, &protocol) == L7_SUCCESS)
        {
          if (protocol == L7_LOGIN_TYPE_HTTP)
          {
            osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_security_ProtocolHTTP);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_security_ProtocolHTTPS);
          }
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat), "%-9s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* verification */
        if (usmDbCpdmCPConfigVerifyModeGet(cpId, &verifyMode) == L7_SUCCESS)
        {
          if (verifyMode == CP_VERIFY_MODE_GUEST)
          {
            osapiSnprintf(stat, sizeof(stat), "%-12s", pStrInfo_security_VerificationModeGuest);
          }
          else if (verifyMode == CP_VERIFY_MODE_LOCAL)
          {
            osapiSnprintf(stat, sizeof(stat), "%-12s", pStrInfo_security_VerificationModeLocal);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-12s", pStrInfo_security_VerificationModeRadius);
          }
        }
        else
        {
          osapiSnprintf(stat,sizeof(stat), "%-12s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        if (usmDbCpdmClientConnStatusIntfClientNextGet(intIfNum, macAddr, &macAddr) != L7_SUCCESS)
        {
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  if (all == L7_TRUE)
  {
    cliAlternateCommandSet(pStrInfo_security_ShowCPIntfClientAllCmd);
  }
  else if (all != L7_TRUE)
  {
    if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), "%s", cliDisplayInterfaceHelp(u, s, p));
    }
    osapiSnprintf(stat, sizeof(stat), pStrInfo_security_ShowCPIntfClientCmd, buf);
    cliAlternateCommandSet(stat);
  }
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*********************************************************************
* @purpose  Display Captive Portal interface capability status
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal interface capability [intf]
*
* @cmdhelp  Display Captive Portal interface capability status.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPInterfaceCapability(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 i, u=0, s=0, p=0, intIfNum=0, intfType;
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_uchar8 value=L7_DISABLE;
  L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 checkChar;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);

  if (cliNumFunctionArgsGet() > 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPInterfaceCapabilitySyntax);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliNumFunctionArgsGet() == 0)
  {
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      checkChar = cliCommon[cliUtil.handleNum].charInput;
      cliClearCharInput();
      cliConvertToLowerCase (&checkChar);
      if ((checkChar == 'q') || (checkChar == 26)) /*ctrl-z*/
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      if (usmDbCpimIntfNextGet(intIfNum, &intIfNum) != L7_SUCCESS)      
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPNoCapableIntf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    ewsTelnetWrite(ewsContext,"\r\nInterface        Interface Description                Type   ");
    ewsTelnetWrite(ewsContext,"\r\n--------- ---------------------------------------- ----------");

    while (L7_TRUE)
    {
      cliSyntaxTop(ewsContext);

      /* interface */
      if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-10s", cliDisplayInterfaceHelp(u, s, p));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* interface description */
      if (usmDbCpimIntfCapabilityDescriptionGet(intIfNum, ifDescription) == L7_SUCCESS)
      {
        cliStringTruncate(ifDescription, 40);
        osapiSnprintf(stat, sizeof(stat), "%-41s", ifDescription);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-41s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* type */
      if (usmDbIntfTypeGet(intIfNum, &intfType) == L7_SUCCESS)
      {
        if (intfType == L7_WIRELESS_INTF)
        {
          osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_security_Wireless);
        }
        else if (intfType == L7_PHYSICAL_INTF)
        {
          osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_security_Wired);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_common_EmptyString);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, stat);

      if (usmDbCpimIntfNextGet(intIfNum, &intIfNum) != L7_SUCCESS)      
      {
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), "%s", argv[0]);

    for (i=1; i<argc; i++)
    {
      osapiStrncat(cmdBuf, " ", L7_CLI_MAX_STRING_LENGTH-1-strlen(cmdBuf));
      osapiStrncat(cmdBuf, argv[i], L7_CLI_MAX_STRING_LENGTH-1-strlen(cmdBuf));
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
  }

  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if ((rc = cliValidSpecificUSPCheck(argv[index+1], &u, &s, &p)) != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          ewsTelnetPrintf (ewsContext, "%u/%u/%u", u, s, p);
        }
        else if (rc == L7_NOT_EXIST)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
        }

        return cliSyntaxReturnPrompt (ewsContext, "");
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(u, s, p, &intIfNum) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", u, s, p);
      }
    }
    else
    {
       /* NOTE: No need to check the unit value as ID of a standalone switch is always 'U_IDX' (=> 1) */
       if (cliSlotPortToIntNum(ewsContext, argv[index+1], &s, &p, &intIfNum) != L7_SUCCESS)
       {
         return cliPrompt(ewsContext);
       }
    }

    if (usmDbCpimIntfGet(intIfNum) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPIntfDoesNotExist);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* interface */
    cliFormat(ewsContext, "Interface");
    osapiSnprintf(stat, sizeof(stat), "%s", cliDisplayInterfaceHelp(u, s, p));
    ewsTelnetWrite(ewsContext, stat);

    /* interface description */
    cliFormat(ewsContext, "Interface Description");
    if (usmDbCpimIntfCapabilityDescriptionGet(intIfNum, ifDescription) == L7_SUCCESS)
    {
      cliStringTruncate(ifDescription, 32);
      osapiSnprintf(stat, sizeof(stat), "%s", ifDescription);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* interface type */
    cliFormat(ewsContext, "Interface Type");
    if (usmDbIntfTypeGet(intIfNum, &intfType) == L7_SUCCESS)
    {
      if (intfType == L7_WIRELESS_INTF)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Wireless);
      }
      else if (intfType == L7_PHYSICAL_INTF)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Wired);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* session timeout */
    cliFormat(ewsContext, "Session Timeout");
    if (usmDbCpimIntfCapabilitySessionTimeoutGet(intIfNum, &value) == L7_SUCCESS)
    {
      if (value == L7_ENABLE)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Supported);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_NotSupported);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* idle timeout */
    cliFormat(ewsContext, "Idle Timeout");
    if (usmDbCpimIntfCapabilityIdleTimeoutGet(intIfNum, &value) == L7_SUCCESS)
    {
      if (value == L7_ENABLE)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Supported);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_NotSupported);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* bytes received counter */
    cliFormat(ewsContext, "Bytes Received Counter");
    if (usmDbCpimIntfCapabilityBytesReceivedCounterGet(intIfNum, &value) == L7_SUCCESS)
    {
      if (value == L7_ENABLE)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Supported);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_NotSupported);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* bytes transmitted counter */
    cliFormat(ewsContext, "Bytes Transmitted Counter");
    if (usmDbCpimIntfCapabilityBytesTransmittedCounterGet(intIfNum, &value) == L7_SUCCESS)
    {
      if (value == L7_ENABLE)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Supported);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_NotSupported);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* packets received counter */
    cliFormat(ewsContext, "Packets Received Counter");
    if (usmDbCpimIntfCapabilityPacketsReceivedCounterGet(intIfNum, &value) == L7_SUCCESS)
    {
      if (value == L7_ENABLE)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Supported);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_NotSupported);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* packets transmitted counter */
    cliFormat(ewsContext, "Packets Transmitted Counter");
    if (usmDbCpimIntfCapabilityPacketsTransmittedCounterGet(intIfNum, &value) == L7_SUCCESS)
    {
      if (value == L7_ENABLE)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Supported);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_NotSupported);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* roaming */
    cliFormat(ewsContext, "Roaming");
    if(usmDbCpimIntfCapabilityRoamingSupportGet(intIfNum, &value) == L7_SUCCESS)
    {
      if (value == L7_ENABLE)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_Supported);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_security_NotSupported);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
    }
    ewsTelnetWrite(ewsContext, stat);
  }

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Display captive-portal interface configuration information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal interface configuration [cp-id] status
*
* @cmdhelp  Display captive-portal interface configuration information.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPIntfConfigurationStatus(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 u, s, p, cpId32, intfType;
  static L7_uint32 intIfNum;
  static L7_ushort16 cpId, cpId_next;
  L7_ushort16 cpId_prev;
  L7_char8 name[CP_NAME_MAX+1];
  L7_uchar8 ifDescription[CP_INTF_DESCRIPTION_MAX_LEN+1];
  L7_char8 checkChar;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL all;

  cliSyntaxTop(ewsContext);

  if ((cliNumFunctionArgsGet() < 1) || (cliNumFunctionArgsGet() > 2))
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPIntfCfgStatus);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (strcmp((L7_char8 *)argv[index+1], "status") == 0)
  {
    cpId32 = 0;
    all = L7_TRUE;
  }
  else
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &cpId32) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if (cpId32 < CP_ID_MIN || cpId32 > CP_ID_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_Invalid_CPIdRange, CP_ID_MIN, CP_ID_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    cpId = (L7_ushort16)cpId32;

    if (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPIdNotFound);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    all = L7_FALSE;
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    checkChar = cliCommon[cliUtil.handleNum].charInput;
    cliClearCharInput();
    cliConvertToLowerCase(&checkChar);
    if ((checkChar == 'q') || (checkChar == 26)) /*ctrl-z*/
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if (all == L7_TRUE)
    {
      cpId_prev = 0;
      cpId = 0;
      intIfNum = 0;
      if (usmDbCpdmCPConfigIntIfNumNextGet(cpId, intIfNum, &cpId, &intIfNum) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPNoIntfCfgAssocExists);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      ewsTelnetWrite(ewsContext,"\r\nCP ID      CP Name       Interface        Interface Description          Type  ");
      ewsTelnetWrite(ewsContext,"\r\n----- ------------------ --------- ----------------------------------- --------");

      while (L7_TRUE)
      {
        cliSyntaxTop(ewsContext);

        /* cp id */
        if (cpId_prev != cpId)
        {
          osapiSnprintf(stat, sizeof(stat), "%-6u", cpId);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-6s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* cp name */
        if (cpId_prev != cpId)
        {
          memset(name, 0x00, sizeof(name));
          if (usmDbCpdmCPConfigNameGet(cpId, name) == L7_SUCCESS)
          {
            cliStringTruncate(name, 18);
            osapiSnprintf(stat, sizeof(stat), "%-19s", name);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-19s", pStrInfo_common_EmptyString);
          }
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-19s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* interface */
        if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-10s", cliDisplayInterfaceHelp(u, s, p));
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* interface description */
        if (usmDbCpimIntfCapabilityDescriptionGet(intIfNum, ifDescription) == L7_SUCCESS)
        {
          cliStringTruncate(ifDescription, 35);
          osapiSnprintf(stat, sizeof(stat), "%-36s", ifDescription);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-36s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* interface type */
        if (usmDbIntfTypeGet(intIfNum, &intfType) == L7_SUCCESS)
        {
          if (intfType == L7_WIRELESS_INTF)
          {
            osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_security_Wireless);
          }
          else if (intfType == L7_PHYSICAL_INTF)
          {
            osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_security_Wired);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_common_EmptyString);
          }
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        cpId_prev = cpId;
        if (usmDbCpdmCPConfigIntIfNumNextGet(cpId, intIfNum, &cpId, &intIfNum) != L7_SUCCESS)
        {
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
    else if (all != L7_TRUE)
    {
      /* cp id */
      cliFormat(ewsContext, "CP ID");
      osapiSnprintf(stat, sizeof(stat), "%u", cpId);
      ewsTelnetWrite(ewsContext, stat);

      /* cp name */
      cliFormat(ewsContext, "CP Name");
      memset(name, 0x00, sizeof(name));
      if (usmDbCpdmCPConfigNameGet(cpId, name) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", name);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      cliSyntaxTop(ewsContext);
      intIfNum = 0;
      if ((usmDbCpdmCPConfigIntIfNumNextGet(cpId, intIfNum, &cpId_next, &intIfNum) != L7_SUCCESS) || (cpId != cpId_next))
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPNoIntfCfgAssoc);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);

      ewsTelnetWrite(ewsContext,"\r\nInterface        Interface Description          Type  ");
      ewsTelnetWrite(ewsContext,"\r\n--------- ----------------------------------- --------");

      while (L7_TRUE)
      {

        cliSyntaxTop(ewsContext);

        /* interface */
        if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-10s", cliDisplayInterfaceHelp(u, s, p));
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-10s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* interface description */
        if (usmDbCpimIntfCapabilityDescriptionGet(intIfNum, ifDescription) == L7_SUCCESS)
        {
          cliStringTruncate(ifDescription, 35);
          osapiSnprintf(stat, sizeof(stat), "%-36s", ifDescription);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-36s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* interface type */
        if (usmDbIntfTypeGet(intIfNum, &intfType) == L7_SUCCESS)
        {
          if (intfType == L7_WIRELESS_INTF)
          {
            osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_security_Wireless);
          }
          else if (intfType == L7_PHYSICAL_INTF)
          {
            osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_security_Wired);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_common_EmptyString);
          }
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        if ((usmDbCpdmCPConfigIntIfNumNextGet(cpId, intIfNum, &cpId_next, &intIfNum) != L7_SUCCESS) || (cpId != cpId_next))
        {
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  if (all == L7_TRUE)
  {
    cliAlternateCommandSet(pStrInfo_security_ShowCPIntfCfgAllCmd);
  }
  else if (all != L7_TRUE)
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_security_ShowCPIntfCfgCmd, cpId);
    cliAlternateCommandSet(stat);
  }
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*********************************************************************
* @purpose  Display captive-portal trapflags
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal trapflags
*
* @cmdhelp  Display captive-portal trapflags.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPTrapFlags(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 mode;

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);

  if (cliNumFunctionArgsGet() != 0)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPTrapflags);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /* client authentication failure traps */
  cliFormat(ewsContext, "Client Authentication Failure Traps");
  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_AUTH_FAILURE, &mode) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(mode, pStrInfo_common_Line));
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
  }   

  /* client connection traps */
  cliFormat(ewsContext, "Client Connection Traps");
  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_CLIENT_CONNECTED, &mode) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(mode, pStrInfo_common_Line));
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
  }   

  /* client database full traps */
  cliFormat(ewsContext, "Client Database Full Traps");
  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_CONNECTION_DB_FULL, &mode) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(mode, pStrInfo_common_Line));
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
  }   

  /* client disconnection traps */
  cliFormat(ewsContext, "Client Disconnection Traps");
  if (usmDbCpdmGlobalTrapModeGet(CP_TRAP_CLIENT_DISCONNECTED, &mode) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(mode, pStrInfo_common_Line));
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
  }   

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Display captive-portal user information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal user [user-id]
*
* @cmdhelp  Display captive-portal user information.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPLocalUser(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 val=0;
  L7_uint32 uId32;
  L7_ushort16 uId, nextUId;
  static L7_ushort16 gId, nextGId;
  L7_char8 checkChar;
  L7_char8 name[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL first = L7_TRUE;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() > 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPUserSyntax);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliNumFunctionArgsGet() == 0)
  {
    uId32 = 0;
  }
  else if (cliNumFunctionArgsGet() == 1)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &uId32) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if (uId32 > FD_CP_LOCAL_USERS_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    uId = (L7_ushort16)uId32;

    if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_UserIdDoesNotExist);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    checkChar = cliCommon[cliUtil.handleNum].charInput;
    cliClearCharInput();
    cliConvertToLowerCase(&checkChar);
    if ((checkChar == 'q') || (checkChar == 26)) /*ctrl-z*/
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {

    if (cliNumFunctionArgsGet() == 0)
    {
      uId = 0;
      if (usmDbCpdmUserEntryNextGet(uId, &uId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPNoUsersExist);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      ewsTelnetWrite(ewsContext,"\r\n                              Session  Idle                                  ");
      ewsTelnetWrite(ewsContext,"\r\nUser ID       User Name       Timeout Timeout  Group ID      Group Name      ");
      ewsTelnetWrite(ewsContext,"\r\n------- --------------------- ------- -------- -------- ---------------------");

      while (L7_TRUE)
      {
        cliSyntaxTop(ewsContext);

        /* user id */
        osapiSnprintf(stat, sizeof(stat), "%-8u", uId);
        ewsTelnetWrite(ewsContext, stat);

        /* user name */
        memset(name, 0x00, sizeof(name));
        if (usmDbCpdmUserEntryLoginNameGet(uId, name) == L7_SUCCESS)
        {
          cliStringTruncate(name, 21);
          osapiSnprintf(stat, sizeof(stat), "%-22s", name);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-22s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* session timeout */
        if (usmDbCpdmUserEntrySessionTimeoutGet(uId, &val) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-8u", val);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-8s", pStrInfo_common_EmptyString);
        }    
        ewsTelnetWrite(ewsContext, stat);

        /* idle timeout */
        if (usmDbCpdmUserEntryIdleTimeoutGet(uId, &val) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-9u", val);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_EmptyString);
        }    
        ewsTelnetWrite(ewsContext, stat);

        /* group id + name */
        gId = 0;
        nextGId = 0;
        nextUId = 0;
        while (L7_SUCCESS==usmDbCpdmUserGroupAssocEntryNextGet(uId,gId,&nextUId,&nextGId))
        {
          if (uId == nextUId)
          {
            if (first != L7_TRUE)
            {
              cliSyntaxTop(ewsContext);
              osapiSnprintf(stat, sizeof(stat), "%-47s", pStrInfo_common_EmptyString);
              ewsTelnetWrite(ewsContext, stat);
            }

            osapiSnprintf(stat, sizeof(stat), "%-9u", nextGId);
            ewsTelnetWrite(ewsContext, stat);

            memset(name, 0x00, sizeof(name));
            if (usmDbCpdmUserGroupEntryNameGet(nextGId, name) == L7_SUCCESS)
            {
              cliStringTruncate(name, 21);
              osapiSnprintf(stat, sizeof(stat), "%-21s", name);
            }
            else
            {
              osapiSnprintf(stat, sizeof(stat), "%-21s", pStrInfo_common_EmptyString);
            }
            ewsTelnetWrite(ewsContext, stat);
            first = L7_FALSE;

            gId = nextGId;
            continue;
          }
          break;
        }

        if (usmDbCpdmUserEntryNextGet(uId, &uId) != L7_SUCCESS)
        {
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        first = L7_TRUE;
      }
    }
    else if (cliNumFunctionArgsGet() == 1)
    {
      /* user id */
      cliFormat(ewsContext, "User ID");
      osapiSnprintf(stat, sizeof(stat), "%u", uId);
      ewsTelnetWrite(ewsContext, stat);

      /* user name */
      cliFormat(ewsContext, "User Name");
      memset(name, 0x00, sizeof(name));
      if (usmDbCpdmUserEntryLoginNameGet(uId, name) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", name);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* password configured */
      cliFormat(ewsContext, "Password Configured");
      if (usmDbCpdmUserEntryPasswordGet(uId, stat) == L7_SUCCESS)
      {
        if (strcmp(stat, "") != 0)
        {
          osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Yes);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_No);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* session timeout */
      cliFormat(ewsContext, "Session Timeout");
      if (usmDbCpdmUserEntrySessionTimeoutGet(uId, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%u", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }    
      ewsTelnetWrite(ewsContext, stat);

      /* idle timeout */
      cliFormat(ewsContext, "Idle Timeout");
      if (usmDbCpdmUserEntryIdleTimeoutGet(uId, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%u", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }    
      ewsTelnetWrite(ewsContext, stat);

      /* max bandwidth up */
      cliFormat(ewsContext, "Max Bandwidth Up (bytes/sec)");
      if (usmDbCpdmUserEntryMaxBandwidthUpGet(uId, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat), "%u", val/8); /* convert from bits to bytes */
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* max bandwidth down */
      cliFormat(ewsContext, "Max Bandwidth Down (bytes/sec)");
      if (usmDbCpdmUserEntryMaxBandwidthDownGet(uId, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat), "%u", val/8); /* convert from bits to bytes */
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* max input octets */
      cliFormat(ewsContext, "Max Input Octets (bytes)");
      if (usmDbCpdmUserEntryMaxInputOctetsGet(uId, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat), "%u", val);
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* max output octets */
      cliFormat(ewsContext, "Max Output Octets (bytes)");
      if (usmDbCpdmUserEntryMaxOutputOctetsGet(uId, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat), "%u", val);
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* max total octets */
      cliFormat(ewsContext, "Max Total Octets (bytes)");
      if (usmDbCpdmUserEntryMaxTotalOctetsGet(uId, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat), "%u", val);
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      cliSyntaxTop(ewsContext);
      gId = 0;
      if (usmDbCpdmUserGroupAssocEntryNextGet(uId, gId, &nextUId, &gId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPNoGroupsExist);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);

      ewsTelnetWrite(ewsContext,"\r\nGroup ID            Group Name           ");
      ewsTelnetWrite(ewsContext,"\r\n-------- --------------------------------");

      while (L7_TRUE)
      {
        cliSyntaxTop(ewsContext);

        /* group id */
        osapiSnprintf(stat, sizeof(stat), "%-9u", gId);
        ewsTelnetWrite(ewsContext, stat);

        /* group name */
        memset(name, 0x00, sizeof(name));
        if (usmDbCpdmUserGroupEntryNameGet(gId, name) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-32s", name);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-32s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        if ((usmDbCpdmUserGroupAssocEntryNextGet(uId, gId, &nextUId, &gId) != L7_SUCCESS) ||
            (uId!=nextUId))
        {
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  if (cliNumFunctionArgsGet() == 0)
  {
    cliAlternateCommandSet(pStrInfo_security_ShowCPUserAllCmd);
  }
  else if (cliNumFunctionArgsGet() == 1)
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_security_ShowCPUserCmd, uId);
    cliAlternateCommandSet(stat);
  }


  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

/*********************************************************************
* @purpose  Display captive-portal user group information
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show captive-portal user group [group-id]
*
* @cmdhelp  Display captive-portal user group information.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandShowCPUserGroup(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 gId32;
  L7_ushort16 gId, tempGId, nextGId;
  static L7_ushort16 uId, tempUId;
  L7_char8 checkChar;
  L7_char8 name[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL first = L7_TRUE;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() > 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_ShowCPGroupSyntax);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliNumFunctionArgsGet() == 0)
  {
    gId32 = 0;
  }
  else if (cliNumFunctionArgsGet() == 1)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &gId32) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((gId32 < GP_ID_MIN) || (gId32 > GP_ID_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserGroupIDOutOfRange, GP_ID_MIN, GP_ID_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    gId = (L7_ushort16)gId32;

    if (usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_GroupIdDoesNotExist);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    checkChar = cliCommon[cliUtil.handleNum].charInput;
    cliClearCharInput();
    cliConvertToLowerCase(&checkChar);
    if ((checkChar == 'q') || (checkChar == 26)) /*ctrl-z*/
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if (cliNumFunctionArgsGet() == 0)
    {
      gId = 0;
      if (usmDbCpdmUserGroupEntryNextGet(gId, &gId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPNoGroupsExist);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      ewsTelnetWrite(ewsContext,"\r\nGroup ID      Group Name       User ID       User Name       ");
      ewsTelnetWrite(ewsContext,"\r\n-------- --------------------- ------- ----------------------");

      while (L7_TRUE)
      {
        cliSyntaxTop(ewsContext);

        /* group id */
        osapiSnprintf(stat, sizeof(stat), "%-9u", gId);
        ewsTelnetWrite(ewsContext, stat);

        /* group name */
        memset(name, 0x00, sizeof(name));
        if (usmDbCpdmUserGroupEntryNameGet(gId, name) == L7_SUCCESS)
        {
          cliStringTruncate(name, 21);
          osapiSnprintf(stat, sizeof(stat), "%-22s", name);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-22s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* user id + name */
        uId = 0;
        nextGId = 0;
        while (L7_SUCCESS==usmDbCpdmUserGroupAssocEntryNextGet(uId,nextGId,&uId,&nextGId))
        {
          if (gId == nextGId)
          {
            if (first != L7_TRUE)
            {
              cliSyntaxTop(ewsContext);
              osapiSnprintf(stat, sizeof(stat), "%-31s", pStrInfo_common_EmptyString);
              ewsTelnetWrite(ewsContext, stat);
            }

            osapiSnprintf(stat, sizeof(stat), "%-8u", uId);
            ewsTelnetWrite(ewsContext, stat);

            memset(name, 0x00, sizeof(name));
            if (usmDbCpdmUserEntryLoginNameGet(uId, name) == L7_SUCCESS)
            {
              cliStringTruncate(name, 22);
              osapiSnprintf(stat, sizeof(stat), "%-22s", name);
            }
            else
            {
              osapiSnprintf(stat, sizeof(stat), "%-22s", pStrInfo_common_EmptyString);
            }
            ewsTelnetWrite(ewsContext, stat);
            first = L7_FALSE;
          }
        }

        if (usmDbCpdmUserGroupEntryNextGet(gId, &gId) != L7_SUCCESS)
        {
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        first = L7_TRUE;
      }
    }
    else if (cliNumFunctionArgsGet() == 1)
    {
      /* group id */
      cliFormat(ewsContext, "Group ID");
      osapiSnprintf(stat, sizeof(stat), "%u", gId);
      ewsTelnetWrite(ewsContext, stat);

      /* group name */
      cliFormat(ewsContext, "Group Name");
      memset(name, 0, sizeof(name));
      if (usmDbCpdmUserGroupEntryNameGet(gId, name) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%s", name);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s", pStrInfo_common_Line);
      }
      ewsTelnetWrite(ewsContext, stat);

      cliSyntaxTop(ewsContext);
      uId = 0;
      if (usmDbCpdmUserGroupAssocEntryNextGet(uId, gId, &uId, &nextGId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPNoUsersAssocToGroup);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);

      ewsTelnetWrite(ewsContext,"\r\nUser ID            User Name            ");
      ewsTelnetWrite(ewsContext,"\r\n------- --------------------------------");

      while (L7_TRUE)
      {
        cliSyntaxTop(ewsContext);

        /* user id */
        osapiSnprintf(stat, sizeof(stat), "%-8u", uId);
        ewsTelnetWrite(ewsContext, stat);

        /* user name */
        memset(name, 0x00, sizeof(name));
        if (usmDbCpdmUserEntryLoginNameGet(uId, name) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-32s", name);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%-32s", pStrInfo_common_EmptyString);
        }
        ewsTelnetWrite(ewsContext, stat);

        /* Search for the gId */
        while (usmDbCpdmUserGroupAssocEntryNextGet(uId, nextGId, &uId, &nextGId) == L7_SUCCESS)
        {
          if (gId==nextGId)
          {
            break;
          }
        }
        if ((usmDbCpdmUserGroupAssocEntryNextGet(uId, gId, &tempUId, &tempGId) != L7_SUCCESS) ||
            (gId!=nextGId))
        {
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  if (cliNumFunctionArgsGet() == 0)
  {
    cliAlternateCommandSet(pStrInfo_security_ShowCPGroupAllCmd);
  }
  else if (cliNumFunctionArgsGet() == 1)
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_security_ShowCPGroupCmd, gId);
    cliAlternateCommandSet(stat);
  }
  return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
}

