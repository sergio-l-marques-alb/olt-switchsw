/*********************************************************************

 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config.c
 *
 * @purpose config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  08/09/2000
 *
 * @update 07/01/2003
 *
 * @author  Forrest Samuels, Deepa
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_ip_mcast_cli.h"

#include "osapi_support.h"
#include "datatypes.h"                 /* for converting from IP to integer */
#include "cli_web_util.h"
#include "clicommands_card.h"
#include "log_api.h"
#include "log_exports.h"
#include "usmdb_log_api.h"
#include "dot1q_exports.h"
#include "dot1x_exports.h"
#include "fdb_exports.h"
#include "snmp_exports.h"
#include "sntp_exports.h"
#include "filter_exports.h"
#include "usmdb_1213_api.h"
#include "usmdb_cnfgr_api.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_mirror_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_policy_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_snmp_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_sntp_api.h"
#include "usmdb_telnet_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_util_api.h"
#include "ews.h"
#include "l7_product.h"
#include "dot3ad_api.h"

#include "usmdb_garp.h"
#include "usmdb_cmd_logger_api.h"
#include "usmdb_keying_api.h"
#include "usmdb_traceroute_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_sdm.h"
#include "cli_web_include.h"
#include "cli_web_mgr_api.h"
#include "cli_web_user_mgmt.h"
#include "session.h"
#include "usmdb_dns_client_api.h"
#include "usmdb_user_mgmt_api.h"
#include "garp_exports.h"
#include "dot3ad_exports.h"
#include "traceroute_exports.h"

extern L7_RC_t usmDbDot3adCfgPortListValidate(L7_uint32 unitNum,
    L7_uint32 trunkIntIfNum,
    L7_uint32 * portList,
    L7_uint32 memCount);

extern L7_RC_t usmDbSwPortMonitorCfgValidate(L7_uint32 unit,
    L7_uint32 sessionNum,
    L7_uint32 monPort,
    L7_uint32 * srcIntfList,
    L7_uint32 count);

extern cliWebCfgData_t cliWebCfgData;

/*********************************************************************
 *
 * @purpose  Delete the Cli Banner file
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8
 *
 * @returns cliPrompt(ewsContext)
 *
 * @notes To delete the banner displayed when user logs in
 *
 *
 * @cmdsyntax no clibanner
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8  *commandNoCliBanner(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 rc;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(cliNumFunctionArgsGet() != 0 || ewsContext->commType == CLI_NORMAL_CMD)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,pStrErr_base_NoBanner);
  }

  /*Clear the clibanner structure*/
  memset(cliWebCfgData.cliBanner, 0, sizeof(cliWebCfgData.cliBanner));
  /*Delete the clibanner file*/
  rc = osapiFsDeleteFile(CLI_BANNER_FILE_NAME);
  if(rc != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_FailedToDelBanner);
  }

  return cliPrompt(ewsContext);

}

/*********************************************************************
 *
 * @purpose  Enable or disable java mode
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8
 *
 * @returns cliPrompt(ewsContext)
 *
 * @notes To configure the javamode for the box. Either Enable or disable
 *
 *
 * @cmdsyntax network javamode
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8  *commandNetworkJavaMode(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 mode = 0;
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNwJavaMode_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoNwJavaMode);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbWebJavaModeSet(unit,mode)!= L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  configure the switch to have a specific user defined mac address
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes not set up yet to store the MAC
 * @notes add range/error checking, remove confirm
 *
 * @cmdsyntax  network mac-address <mac-addr>
 *
 * @cmdhelp Enter the locally administered MAC address.
 *
 * @cmddescript
 *   This is an additional parameter that you can configure. The
 *   following rules apply:
 *       Bit 6 of byte 0 (called the U/L bit) indicates whether the address
 *           is universally administered (B'0') or locally administered (B'1').
 *       Bit 7 of byte 0 (called the I/G bit) indicates whether the destination
 *           address is an individual address (B'0') or a group address (B'1').
 *       A locally administered address must have bit 6 On (B'1') and bit 7 Off (B'0').
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandNetworkMACAddress(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uchar8 mac_address[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg, unit;
  L7_uint32 argMac = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /*check to see if anything other than 1 argument was passed */
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgNwMac);
  }

  if (strlen(argv[index+argMac]) >= sizeof(mac_address))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_common_NwMacInvalidParm);
  }

  OSAPI_STRNCPY_SAFE(mac_address, argv[index+argMac]);

  memset(mac, 0, sizeof(mac));
  if (cliConvertMac(mac_address, mac) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_UsrInputInvalidClientMacAddr,ewsContext, pStrErr_base_CfgNwMac);
  }

  if ((mac[0] & 0x03) != 0x02)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgNwMac);
  }
  else
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbSwDevCtrlLocalAdminAddrSet(unit, mac) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_NwMacFailedToSet);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  to set the switch to use either the locally administered or burnedin MAC address
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  network mac-type {local | burnedin}
 *
 * @cmdhelp Select the locally administered or burnedin MAC address.
 *
 * @cmddescript
 *   Specifies if the burnedin MAC address or the locally-administered
 *   MAC address should be used. The burnedin MAC address is the
 *   default MAC address type.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandNetworkMACType(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uchar8 nullMac[L7_MAC_ADDR_LEN];
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 argType = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNwMacType);
    }

    if (strcmp(argv[index+argType], pStrInfo_base_Burnedin) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlMacAddrTypeSet(unit, L7_SYSMAC_BIA);
      }
    }
    else if (strcmp(argv[index+argType], pStrInfo_common_LocalAuth) == 0)
    {
      memset(( void * )nullMac, 0, L7_MAC_ADDR_LEN);
      usmDbSwDevCtrlLocalAdminAddrGet(unit, mac);
      if (strcmp(mac, nullMac) == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_CfgNwMacError);
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlMacAddrTypeSet(unit, L7_SYSMAC_LAA);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNwMacType);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoNwMacType);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSwDevCtrlMacAddrTypeSet(unit, L7_SYSMAC_BIA);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
 *
 * @purpose  Configure the network ip, netmask and gateway.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8
 *
 * @returns cliPrompt(ewsContext)
 *
 * @notes  To configure the switch network mask.  The subnet mask for the LAN.
 *         To configure the default gateway.  Identifies the address of
 *         the default router if the switch is a node outside the IP range of the LAN.
 *         To configure the switch ip.   Unique IP address of your switch.
 *         Each IP parameter is made up of four decimal numbers.
 *         The numbers range from 0 to 255. The default for
 *         all IP parameters consists of "0"s (that is, 0.0.0.0).
 *
 *
 * @cmdsyntax  network parms { [<ipaddress> <netmask> [<gateway>]] | none }
 *
 * @cmdhelp
 *
 * @cmddescript  check for valid ip addr and valid subnet mask.
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8  *commandNetworkParams(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 argNetMask = 2;
  L7_uint32 argGateway = 3;
  L7_uint32 ipAddr, oldIpAddr;
  L7_uint32 netMask, oldNetMask;
  L7_uint32 gateway, oldGateway;
  L7_uint32 val, errorNum;
  L7_uint32 numArg;        /* New variable Added */

  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strNetMask[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strGateway[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* initialize values */
  ipAddr = 0;
  netMask = 0;
  gateway = 0;

  /* If we have less than 2 or more than 3 arguments - error */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* if more than 3 or less than 1 arguments, display syntax */
  if (numArg < 1  ||  numArg > 3)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNwParams);
  }

  if (strcmp(argv[index + 1], "none") == 0)
  {
    usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(unit, &val);
    if (val != L7_SYSCONFIG_MODE_NONE)
    {
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_CfgNwIp_1);
    }
    usmDbAgentIpIfDefaultRouterSet(unit, 0); 
    usmDbSystemIPAndNetMaskSet(unit, 0, 0, &errorNum);
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  if ((strlen(argv[index+argIPaddr]) >= sizeof(strIpAddr)) ||
      (strlen(argv[index+argNetMask]) >= sizeof(strNetMask)))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNwParams);
  }

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(unit,&val);
  if (val != L7_SYSCONFIG_MODE_NONE)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_CfgNwIp_1);
  }

  /* Verify if the specified ip address is valid */
  OSAPI_STRNCPY_SAFE(strIpAddr, argv[index + argIPaddr]);
  if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
  }

  /* Verify if the specified network mask is valid */
  OSAPI_STRNCPY_SAFE(strNetMask, argv[index + argNetMask]);
  if (usmDbInetAton(strNetMask, &netMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwMask);
  }
  if (usmDbNetmaskValidate(netMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSubnetMask);
  }
  if (usmDbIpAddressValidate(ipAddr, netMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidIpCombo);
  }

  /* Verify if the specified network mask is valid if supplied, defaults to 0.0.0.0 */
  if (numArg > 2)
  {
    if (strlen(argv[index+argGateway]) >= sizeof(strGateway))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNwParams);
    }
    OSAPI_STRNCPY_SAFE(strGateway, argv[index + argGateway]);
    if (usmDbInetAton(strGateway, &gateway) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNwGatewayInvalid);
    }
  }

  /* Get the old ip address & subnet mask, and gateway in case something fails */
  usmDbAgentIpIfAddressGet(unit, &oldIpAddr);
  usmDbAgentIpIfNetMaskGet(unit, &oldNetMask);
  usmDbAgentIpIfDefaultRouterGet(unit, &oldGateway);

  /* If the current configuration is already the same as specified by the user exit */
  if ((ipAddr == oldIpAddr) && (oldNetMask == netMask) && (oldGateway == gateway))
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  /* Verify the validity of the ip address, subnet mask & gateway together */
  if (usmDbIpInfoValidate(ipAddr, netMask, gateway) != L7_SUCCESS)
  {
    /* If the triad is invalid return without doing anything */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgGatewayError);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* If the triad is valid reset gateway, ip address, and netmask */
    usmDbAgentIpIfDefaultRouterSet(unit, 0);            /* CLEAR GATEWAY */

    /* only if ip or netmask changed */
    if ((ipAddr != oldIpAddr) || (oldNetMask != netMask))
    {
      /* then set up the netmask, ip addr with user specified values */
      if (usmDbSystemIPAndNetMaskSet(unit, ipAddr, netMask, &errorNum) != L7_SUCCESS)  /* SET IP/NETMASK */
      {
        /* If we could not set the IP and NetMask, reset the gateway, and return with the given error message */
        usmDbAgentIpIfDefaultRouterSet(unit, oldGateway);                              /* CLEAR GATEWAY */

        switch(errorNum)
        {
          case 7101:
            ewsTelnetWrite(ewsContext, pStrInfo_base_ConflictingIpAndMaskSwitch);
            break;
          case 7102:
            ewsTelnetWrite(ewsContext, pStrInfo_base_ConflictingIpAndMaskServicePort);
            break;
          case 7103:
            ewsTelnetWrite(ewsContext, pStrInfo_common_ConflictingIpAndMaskRtrIntf);
            break;
        }
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }

    /* if gateway is not 0, and if setting the gateway fails */
    if (gateway != 0 && usmDbAgentIpIfDefaultRouterSet(unit, gateway) != L7_SUCCESS) /* SET GATEWAY */
    {
      /* Revert back changes and display reason for failure, does not check validity */
      usmDbAgentIpIfDefaultRouterSet(unit, oldGateway);                              /* RESET GATEWAY */

      /* only if ip or netmask changed */
      if ((ipAddr != oldIpAddr) || (oldNetMask != netMask))
      {
        usmDbSystemIPAndNetMaskSet(unit, oldIpAddr, oldNetMask, &errorNum);
      }

      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_base_CfgNwGateway);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  to set the switch to use either BootP, DHCP or manual entered network settings
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  network protocol {none | bootp | dhcp}
 *
 * @cmdhelp Select BootP, DHCP or None as the network config protcol.
 *
 * @cmddescript
 *   When you select BootP or DHCP (the default), the switch
 *   periodically sends requests to a BootP or DHCP server until
 *   a response is received. You must specify None, if you want
 *   to manually configure the switch with the appropriate IP
 *   information. When this value is modified, you need to issue
 *   a Save and then reset the switch in order for the new value
 *   to take effect.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandNetworkProtocol(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 servPortVal, oldVal;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 argProtocol = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNwProto_1);
  }

  if (strlen(argv[index+argProtocol]) >= sizeof(buf))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNwProto_1);
  }

  if ( (strcmp(argv[index+argProtocol], pStrInfo_common_None_3) == 0) ||
      (strcmp(argv[index+argProtocol], pStrInfo_base_Bootp_1) == 0) ||
      (strcmp(argv[index+argProtocol], pStrInfo_common_Dhcp_1) == 0) )
  {
    if ( cliGetCharInputID() == CLI_INPUT_EMPTY && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
    {
      cliSyntaxTop(ewsContext);
      cliSetCharInputID(1, ewsContext, argv);

      osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_base_NwProto, argv[index+argProtocol]);
      cliAlternateCommandSet(buf);
      return pStrInfo_base_VerifyChangingProtoMode;
    }

    if ( tolower(cliGetCharInput()) != 'y' && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt(ewsContext, pStrInfo_base_ProtoModeNotChgd );
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

      usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(unit,&oldVal);

      if (strcmp(argv[index+argProtocol], pStrInfo_common_None_3) == 0)
      {
        usmDbAgentBasicConfigProtocolDesiredSet(unit, L7_SYSCONFIG_MODE_NONE);
      }
      else if (strcmp(argv[index+argProtocol], pStrInfo_base_Bootp_1) == 0)
      {

        usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(unit,&servPortVal);
        if (servPortVal == L7_SYSCONFIG_MODE_NONE)
        {
          usmDbAgentBasicConfigProtocolDesiredSet(unit,L7_SYSCONFIG_MODE_BOOTP);
        }
        else
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_CfgNwProto);
        }

      }
      else if (strcmp(argv[index+argProtocol], pStrInfo_common_Dhcp_1) == 0)
      {
        usmDbAgentBasicConfigProtocolDesiredSet(unit,L7_SYSCONFIG_MODE_DHCP);
      }

      if (oldVal == L7_SYSCONFIG_MODE_BOOTP)
      {
        usmDbBootpTaskReInit(unit);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNwProto_1);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  configure the management vlan id
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  network mgmt_vlan {1-4094}
 *
 * @cmdhelp Enable/Disable the Web interface.
 *
 * @cmddescript
 *   Used to configure the management vlan id. The default value of
 *   the vlan id is 1
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandNetworkMgmtVlan(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 vlanid=1;
  L7_uint32 argVlanID=1;
  L7_uint32 unit;
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
          pStrErr_base_CfgNwMgmtVlan, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    }

    sscanf(argv[index+argVlanID], "%d", &vlanid);
    /* port VLAN ID doesn't have to exist yet, just be in range */
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoNwMgmtVlan);
    }
    /* Default VlanId */
    vlanid = FD_SIM_DEFAULT_MGMT_VLAN_ID;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbMgmtVlanIdSet(unit, vlanid);
    if (rc == L7_ERROR)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,
                                             ewsContext, 
                                             "VLAN %u is configured as a routing VLAN. "
                                             "Cannot set the management VLAN to a VLAN used for routing.",
                                             vlanid);
    }
    else if (rc == L7_ALREADY_CONFIGURED)
    {
      return cliSyntaxReturnPromptAddBlanks(0, 1, 0, 0, pStrErr_common_Error, ewsContext, 
                                            "VLAN ID %u is used internally. "
                                            "Use 'show vlan internal usage' to display how "
                                            "this VLAN ID is being used.",
                                            vlanid);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, 
                                             ewsContext, pStrErr_base_MgmtVlanNotSet);
    }
    else
    {
      /* Do Nothing */
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  configures the switch ip, gateway and subnet for the service port.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8
 *
 * @returns cliPrompt(ewsContext)
 *
 * @notes  To configure the switch ip.  Enter the switch's IP address.
 *         Unique IP address of your switch. Each IP parameter is made up
 *         of four decimal numbers. The numbers range from 0 to 255. The default for
 *         all IP parameters consists of "0"s (that is, 0.0.0.0).
 *         To configure the servicePort network mask, The subnet mask for the LAN.
 *         To configure the default gateway.  Identifies the address of the
 *         default router if the switch is a node outside the IP range of the LAN.
 *
 *
 * @cmdsyntax  serviceport { [ip <ipaddr> <netmask> [<gateway>]] | none }
 *
 * @cmdhelp
 *
 * @cmddescript  check for valid ip addr and valid subnet mask.
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8  *commandSetServiceportIp(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 argNetMask = 2;
  L7_uint32 argGateway = 3;
  L7_uint32 ipAddr, oldIpAddr;
  L7_uint32 netMask, oldNetMask;
  L7_uint32 gateway, oldGateway;
  L7_uint32 val, errorNum;
  L7_uint32 numArg;

  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strNetMask[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strGateway[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;


  /* initialize values */
  ipAddr = 0;
  netMask = 0;
  gateway = 0;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* if more than 3 or less than 1 arguments, display syntax */
  if (numArg < 1  ||  numArg > 3)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SetIpRouteParams);
  }

  if (strcmp(argv[index + 1], "none") == 0)
  {
    usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(unit, &val);
    if (val != L7_SYSCONFIG_MODE_NONE)
    {
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_CfgServPortIp_1);
    }
    usmDbServicePortGatewaySet(unit, 0);                                    
    usmDbServicePortIPAndNetMaskSet(unit, 0, 0, &errorNum);
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  if ((strlen(argv[index+argIPaddr]) >= sizeof(strIpAddr)) ||
      (strlen(argv[index+argNetMask]) >= sizeof(strIpAddr)))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SetIpRouteParams);
  }

  /* Verify if the specified ip address is valid */
  OSAPI_STRNCPY_SAFE(strIpAddr, argv[index + argIPaddr]);
  if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_CfgServPortIp);
  }

  /* Verify if the specified network mask is valid */
  OSAPI_STRNCPY_SAFE(strNetMask, argv[index + argNetMask]);
  if (usmDbInetAton(strNetMask, &netMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgServPortMask);
  }
  if (usmDbNetmaskValidate(netMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSubnetMask);
  }
  if (usmDbIpAddressValidate(ipAddr, netMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidIpCombo);
  }

  /* Verify if the specified network mask is valid if supplied, defaults to 0.0.0.0 */
  if (numArg > 2)
  {
    if (strlen(argv[index+argGateway]) >= sizeof(strIpAddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_base_NetmaskErr);

    }
    OSAPI_STRNCPY_SAFE(strGateway, argv[index + argGateway]);
    if (usmDbInetAton(strGateway, &gateway) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgNwGateway);
    }
  }


  /* Verify the validity of the ip address, subnet mask & gateway together */
  if (usmDbIpInfoValidate(ipAddr, netMask, gateway) != L7_SUCCESS)
  {
    /* If the triad is invalid return without doing anything */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgGatewayError);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
  
     /* protocol must be set to none to be configured */
    usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(unit,&val);
    if (val != L7_SYSCONFIG_MODE_NONE)
    {
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_CfgServPortIp_1);
    }
  
    /* Get the current ip address, subnet mask, and gateway */
    usmDbServicePortConfiguredIpAddrGet(&oldIpAddr);
    usmDbServicePortConfiguredNetMaskGet(&oldNetMask);
    usmDbServicePortConfiguredGatewayGet(&oldGateway);

    /* If the current configuration is already the same as specified by the user exit */
    if ((ipAddr == oldIpAddr) && (oldNetMask == netMask) && (oldGateway == gateway))
    {
      return cliPrompt(ewsContext);
    }

    /* Default gateway is optional. Set to 0, and if specified, reset below.  */
    usmDbServicePortGatewaySet(unit, 0);                                               

    /* only if ip or netmask changed */
    if ((ipAddr != oldIpAddr) || (oldNetMask != netMask))
    {
      /* set up the netmask, ip addr with user specified values */
      if (usmDbServicePortIPAndNetMaskSet(unit, ipAddr, netMask, &errorNum) != L7_SUCCESS)  
      {
        /* If we could not set the IP and NetMask, reset the gateway, and return with the given error message */
        usmDbServicePortGatewaySet(unit, oldGateway);                                  /* RESET GATEWAY */

        switch(errorNum)
        {
          case 7101:
            ewsTelnetWrite(ewsContext, pStrInfo_base_ConflictingIpAndMaskSwitch);
            break;
          case 7102:
            ewsTelnetWrite(ewsContext, pStrInfo_base_ConflictingIpAndMaskServicePort);
            break;
          case 7103:
            ewsTelnetWrite(ewsContext, pStrInfo_common_ConflictingIpAndMaskRtrIntf);
            break;
        }

        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }

    if (gateway != 0 && usmDbServicePortGatewaySet(unit, gateway) != L7_SUCCESS)       /* SET GATEWAY */
    {
      /* Revert back changes and display reason for failure, does not check validity */
      usmDbServicePortGatewaySet(unit, oldGateway);                                    /* RESET GATEWAY */

      /* only if ip or netmask changed */
      if ((ipAddr != oldIpAddr) || (oldNetMask != netMask))
      {
        usmDbServicePortIPAndNetMaskSet(unit, oldIpAddr, oldNetMask, &errorNum);
      }

      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_base_CfgNwGateway);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  to set the servicePort to use either BootP, DHCP or manual entered network settings
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  serviceport protocol <none, bootp, dhcp>
 *
 * @cmdhelp Select BootP, DHCP  or None as the network config protcol.
 *
 * @cmddescript
 *   When you select BootP or DHCP (the default), the servicePort
 *   periodically sends requests to a BootP or DHCP server until
 *   a response is received. You must specify None, if you want
 *   to manually configure the switch with the appropriate IP
 *   information. When this value is modified, you need to issue
 *   a Save and then reset the switch in order for the new value
 *   to take effect.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetServiceportProtocol(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 networkVal, oldVal;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 argProtocol = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgServPortProto_1);
  }

  if (strlen(argv[index+argProtocol]) >= sizeof(buf))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgServPortProto_1);
  }

  if ( (strcmp(argv[index+argProtocol], pStrInfo_common_None_3) == 0) ||
      (strcmp(argv[index+argProtocol], pStrInfo_base_Bootp_1) == 0) ||
      (strcmp(argv[index+argProtocol], pStrInfo_common_Dhcp_1) == 0) )
  {
    if ( cliGetCharInputID() == CLI_INPUT_EMPTY && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
    {
      cliSyntaxTop(ewsContext);
      cliSetCharInputID(1, ewsContext, argv);

      osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_base_ServiceportProto, argv[index+argProtocol]);
      cliAlternateCommandSet(buf);

      return pStrInfo_base_VerifyChangingProtoMode;
    }

    if ( tolower(cliGetCharInput()) != 'y' && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_base_ProtoModeNotChgd);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

      usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(unit,&oldVal);

      if (strcmp(argv[index+argProtocol], pStrInfo_common_None_3) == 0)
      {
        usmDbAgentBasicConfigServPortProtocolDesiredSet(unit,L7_SYSCONFIG_MODE_NONE);
      }
      else if (strcmp(argv[index+argProtocol], pStrInfo_base_Bootp_1) == 0)
      {
        usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(unit,&networkVal);
        if (networkVal == L7_SYSCONFIG_MODE_NONE)
        {
          usmDbAgentBasicConfigServPortProtocolDesiredSet(unit,L7_SYSCONFIG_MODE_BOOTP);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_CfgServPortProto);
        }
      }
      else if (strcmp(argv[index+argProtocol], pStrInfo_common_Dhcp_1) == 0)
      {
        usmDbAgentBasicConfigServPortProtocolDesiredSet(unit,L7_SYSCONFIG_MODE_DHCP);
      }
      else
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPromptAddBlanks(1,1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgServPortProto_1);
      }

      if (oldVal == L7_SYSCONFIG_MODE_BOOTP)
      {
        usmDbBootpTaskReInit(unit);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgServPortProto_1);
  }

  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  to set the prompt for the switch
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes this value is only saved in ram currently and any settings are lost accross a reset
 * @notes on other boxes this is really setting the hostname
 *
 * @cmdsyntax  set prompt <prompt-string>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   This commands is used to change the prompt for the switch.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetPrompt(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_char8 temp[L7_PROMPT_SIZE];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 arg1 = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPrompt_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (strlen(argv[index+arg1]) >= sizeof(temp))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPrompt);
  }

  OSAPI_STRNCPY_SAFE(temp, (L7_char8 *)argv[index+arg1]);

  if (strlen((L7_char8 *)argv[index+arg1]) >= L7_PROMPT_SIZE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgPrompt_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    osapiSnprintf(cliUtil.systemPrompt, sizeof(cliUtil.systemPrompt), "\r\n(%s) ", temp);

    rc = usmDbCommandPromptSet(unit, temp);
    osapiSnprintf(cliCommon[cliUtil.handleNum].prompt, sizeof(cliCommon[cliUtil.handleNum].prompt), "%s%s",cliUtil.systemPrompt, pStrInfo_common_PriviledgeUsrExecModePrompt);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  configure the baudrate for the serial connection
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  serial baudrate {1200|2400|4800|9600|19200|38400|57600|115200}
 *
 * @cmdhelp Configure the serial port baud rate.
 *
 * @cmddescript
 *   Specifies the communication rate of the terminal interface.
 *   Values can be 1200, 2400, 4800, 9600, 19200, 38400, 57600, or 115200.
 *   The default value is 9600.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetSerialBaudrate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 val=0;
  L7_uint32 unit;
  L7_uint32 inputParm;
  L7_uint32 numArg;
  L7_uint32 argSpeed = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /**** If the command is of type 'normal'the 'if' condition is executed
    otherwise 'else-if' condition is excuted******/

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgSerialBaudRate);
    }
    else
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+argSpeed],&inputParm)!=L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }

      if (inputParm == 1200)
      {
        val = L7_BAUDRATE_1200;
      }
      else if (inputParm == 2400)
      {
        val = L7_BAUDRATE_2400;
      }
      else if (inputParm == 4800)
      {
        val = L7_BAUDRATE_4800;
      }
      else if (inputParm == 9600)
      {
        val = L7_BAUDRATE_9600;
      }
      else if (inputParm == 19200)
      {
        val = L7_BAUDRATE_19200;
      }
      else if (inputParm == 38400)
      {
        val = L7_BAUDRATE_38400;
      }
      else if (inputParm == 57600)
      {
        val = L7_BAUDRATE_57600;
      }
      else if (inputParm == 115200)
      {
        val = L7_BAUDRATE_115200;
      }
    }

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoSerialBaudRate);
    }

    val = FD_SIM_DEFAULT_SYSTEM_BAUD_RATE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbAgentBasicConfigSerialBaudRateSet(unit, val);

    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, pStrErr_common_Error,  ewsContext,pStrErr_base_CfgSerialBaudRateErr );
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  configure the timeout in minutes for the serial connection
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  serial timeout <0-160>
 *
 * @cmdhelp Configure the serial port login inactivity timeout.
 *
 * @cmddescript
 *   Specifies the maximum connect time without console activity.
 *   The value is in a range from 0 to 160 minutes. A value of 0
 *   indicates that a console can be connected indefinitely.
 *   The default value is 5 minutes.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetSerialTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 tempInt = 0;
  L7_uint32 argTimeout = 1;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgSerialTimeout);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argTimeout],&tempInt) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoSerialTimeout);
    }

    tempInt = FD_SIM_DEFAULT_SERIAL_PORT_TIMEOUT;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbSerialTimeOutSet(unit, tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_CfgSerialTimeoutErr);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose to set mode to read/write or read only for an SNMP community
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax snmp-server community {ro | rw} <community-name>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   This value can be read-only or read/write. A community with a
 *   read-only access allows for switch information to be displayed.
 *   A community with a read/write access allows for configuration
 *   changes to be made and for information to be displayed.
 *
 *   A community name with read-only access is restricted from viewing
 *   SNMP community and SNMP trap receiver information.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpServerCommunityRo(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index )
{
  L7_uchar8 name[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 commIndex;
  L7_RC_t rc2 = L7_SUCCESS;
  L7_RC_t rc;
  L7_uint32 currStatus;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 argName = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseSnmpCommNameRoRwName);
  }

  if ( (strlen(argv[index+argName]) >= sizeof(name)) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseSnmpCommNameRoRwName);
  }

  OSAPI_STRNCPY_SAFE(name, argv[index+argName]);

  /* if correct input, execute this section of code */
  commIndex = cliFindSNMPCommunityIndex(name);
  if (commIndex == CLI_INDEX_NOT_FOUND)
  {
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_CommunityDoesntExist);
    }
  }
  else
  {
    rc = usmDbSnmpCommunityStatusGet(unit, commIndex, &currStatus);

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnmpCommunityStatusSet(unit, commIndex, L7_SNMP_COMMUNITY_STATUS_CONFIG);
      rc2 = usmDbSnmpCommunityAccessLevelSet(unit, commIndex, L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY);
      rc = usmDbSnmpCommunityStatusSet(unit, commIndex, currStatus);
      if (rc2 != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetCommunityMode);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose to set mode to read/write or read only for an SNMP community
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax snmp-server community {ro | rw} <community-name>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   This value can be read-only or read/write. A community with a
 *   read-only access allows for switch information to be displayed.
 *   A community with a read/write access allows for configuration
 *   changes to be made and for information to be displayed.
 *
 *   A community name with read-only access is restricted from viewing
 *   SNMP community and SNMP trap receiver information.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpServerCommunityRw(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index )
{
  L7_uchar8 name[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 commIndex;
  L7_RC_t rc2 = L7_SUCCESS;
  L7_RC_t rc;
  L7_uint32 currStatus;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 argName = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseSnmpCommNameRoRwName);
  }

  if ( (strlen(argv[index+argName]) >= sizeof(name)) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseSnmpCommNameRoRwName);
  }

  OSAPI_STRNCPY_SAFE(name, argv[index+argName]);

  /* if correct input, execute this section of code */
  commIndex = cliFindSNMPCommunityIndex(name);
  if (commIndex == CLI_INDEX_NOT_FOUND)
  {
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_CommunityDoesntExist);
    }
  }
  else
  {
    rc = usmDbSnmpCommunityStatusGet(unit, commIndex, &currStatus);

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnmpCommunityStatusSet(unit, commIndex, L7_SNMP_COMMUNITY_STATUS_CONFIG);
      rc2 = usmDbSnmpCommunityAccessLevelSet(unit, commIndex, L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE);
      rc = usmDbSnmpCommunityStatusSet(unit, commIndex, currStatus);
      if (rc2 != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetCommunityMode);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose to set the IP for accepting SNMP packets
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax snmp-server community ipaddr <ipAddr> <name>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   This attribute is an IP address (or portion thereof) from which
 *   this device will accept SNMP packets with the associated community.
 *   The requesting entity's IP address is logical-ANDed with the Client
 *   IP Mask and the result must match the Client IP Address.
 *   The default value is 0.0.0.0.
 *       Note: If the Client IP Mask is set to 0.0.0.0, a Client IP
 *       Address of 0.0.0.0 matches all IP addresses.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpServerCommunityIpAddress(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index )
{
  L7_uchar8 name[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 ipString[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 inetint;
  L7_int32 commIndex;
  L7_RC_t rc;
  L7_uint32 currStatus;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 arg1 = 1;
  L7_uint32 arg2 = 2;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseIpAddrIpAddrName);
    }

    if ((strlen(argv[index+arg1]) >= sizeof(ipString)) ||
        (strlen(argv[index+arg2]) >= sizeof(name)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseIpAddrIpAddrName);
    }

    OSAPI_STRNCPY_SAFE(ipString, argv[index+arg1]);
    OSAPI_STRNCPY_SAFE(name, argv[index+arg2]);

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseIpAddrNoIpAddrName);
    }

    if(strlen(argv[index+arg1]) >= sizeof(name))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseIpAddrNoIpAddrName);
    }
    OSAPI_STRNCPY_SAFE(name, argv[index+arg1]);

    OSAPI_STRNCPY_SAFE(ipString,pStrInfo_common_0000);
  }

  commIndex = cliFindSNMPCommunityIndex(name);
  if (commIndex == CLI_INDEX_NOT_FOUND)
  {
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_CommunityDoesntExist);
    }
  }
  else
  {
    rc = usmDbInetAton(ipString, &inetint);

    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_TacacsInValIp);
    }

    rc = usmDbSnmpCommunityStatusGet(unit, commIndex, &currStatus);

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnmpCommunityStatusSet(unit, commIndex, L7_SNMP_COMMUNITY_STATUS_CONFIG);
      rc = usmDbSnmpCommunityIpAddrSet(unit, commIndex, inetint);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetCommunityIpAddr);
      }
      rc = usmDbSnmpCommunityStatusSet(unit, commIndex, currStatus);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose to set the IP Mask for accepting SNMP packets
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes mask is not retained accross a reset even after a 'save flash'
 *
 * @cmdsyntax snmp-server community ipmask <ipmask> <name>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   This attribute is a mask to be logical-ANDed with the requesting
 *   entity's IP address before comparison with the Client IP Address.
 *   If the result matches with Client IP Address then the address is
 *   an authenticated IP address. For example, if the Client IP Address
 *   is 9.47.128.0 and the corresponding Client IP Mask is 255.255.255.0,
 *   a range of incoming IP addresses would match, that is, the incoming
 *   IP addresses could be a value in the following range: 9.47.128.0
 *   to 9.47.128.255.
 *
 *   To have a specific IP address be the only authenticated IP address,
 *   set the Client IP Address to the required IP address and set the
 *   Client IP Mask to 255.255.255.255. The default for the Client IP
 *   Mask is 0.0.0.0.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpServerCommunityIPMask(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index )
{
  L7_uchar8 name[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 netmaskString[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 commIndex;
  L7_uint32 inetint;
  L7_RC_t rc;
  L7_uint32 currStatus;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 arg1 = 1;
  L7_uint32 arg2 = 2;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseIpMaskIpMaskName);
    }

    if ((strlen(argv[index+arg1]) >= sizeof(netmaskString)) ||
        (strlen(argv[index+arg2]) >= sizeof(name)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseIpMaskIpMaskName);
    }

    OSAPI_STRNCPY_SAFE(netmaskString, argv[index+arg1]);
    OSAPI_STRNCPY_SAFE(name, argv[index+arg2]);

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)  /* <community name> */
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseIpMaskNoIpMaskName);
    }

    if(strlen(argv[index+arg1]) >= sizeof(name))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseIpMaskNoIpMaskName);
    }
    OSAPI_STRNCPY_SAFE(name, argv[index+arg1]);

    OSAPI_STRNCPY_SAFE(netmaskString,pStrInfo_common_0000);

  }

  /* if correct input, execute this section of code */
  commIndex = cliFindSNMPCommunityIndex(name);
  if (commIndex == CLI_INDEX_NOT_FOUND)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_CommunityDoesntExist);
  }
  else
  {
    rc = usmDbInetAton(netmaskString, &inetint);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidIpMask);
    }
    rc = usmDbSnmpCommunityStatusGet(unit, commIndex, &currStatus);
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnmpCommunityStatusSet(unit, commIndex, L7_SNMP_COMMUNITY_STATUS_CONFIG);
      rc = usmDbSnmpCommunityIpMaskSet(unit, commIndex, inetint);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetCommunityIpMask);
      }
      rc = usmDbSnmpCommunityStatusSet(unit, commIndex, currStatus);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose Enable / disable the authentication flag
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  [no] snmp-server enable traps
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpServerEnableTraps(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 mode=0;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTrapFlagsAuth);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoTrapFlagsAuth);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode= L7_DISABLE;
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbTrapAuthSwSet(unit, mode)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose Enable/disable Link up/down traps
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  snmp-server enable traps linkmode
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandEnableTrapsLinkMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 mode=0;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTrapFlagsLinksTatus_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoTrapFlagsLinksTatus);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if(usmDbTrapLinkStatusSwSet(unit, mode)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose Enable/disable multiple user traps
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  snmp-server enable traps multiusers
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpServerEnableTrapsMultiusers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 mode=0;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTrapFlagsMultiUsrs);
    }
    else if ((ewsContext->commType == CLI_NO_CMD))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoTrapFlagsMultiUsrs);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if(usmDbTrapMultiUsersSwSet(unit, mode)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose Enable/disable sending of new root traps
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  [no] snmp-server enable traps stpmode
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpServerEnableTrapsSTPMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 mode=0;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTrapFlagsStp_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoTrapFlagsStp);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if(usmDbTrapSpanningTreeSwSet(unit, mode)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose to change an SNMP community trap ip based on its community name
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  snmptrap ipddr <ipaddrold> <name> <ipaddrnew|hostname>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   Each IP address parameter is four decimal numbers. The numbers
 *   range from 0 to 255. The default IP address is 0.0.0.0.
 *
 *   This is the IP to send captured SNMP traps to.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpTrapIPAddr(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index )
{
  L7_uchar8 commName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strIpAddrNew[L7_CLI_MAX_STRING_LENGTH];
  L7_int32  trapIndex;
  L7_uint32 currentConfig;
  L7_RC_t rc;
  L7_uint32 ipAddr, ipAddrNew;
  L7_uint32 unit;
  L7_uint32 numArg,ipAddrType=0;
/*  L7_inet_addr_t inetAddr;*/
  L7_inet_addr_t inetAddrNew;
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_char8 domainName[L7_DNS_DOMAIN_NAME_SIZE_MAX];  
  /*L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;*/
  L7_IP_ADDRESS_TYPE_t addrTypeNew = L7_IP_ADDRESS_TYPE_UNKNOWN;
    /*#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    L7_in6_addr_t ipv6_address,ipv6_address_new;
    memset(&ipv6_address,0x00,sizeof(L7_in6_addr_t));
    memset(&ipv6_address_new,0x00,sizeof(L7_in6_addr_t));
#endif
L7_uchar8     str_addr[64];*/

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 3)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseIpIpAddrOldNameIpAddrNew);
  }

  if ((strlen(argv[index+2]) >= sizeof(strIpAddr)) ||
      (strlen(argv[index+1]) >= sizeof(commName)) ||
      (strlen(argv[index+3]) >= sizeof(strIpAddrNew)))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseIpIpAddrOldNameIpAddrNew);
  }

  OSAPI_STRNCPY_SAFE(strIpAddr, argv[index+2]);
  OSAPI_STRNCPY_SAFE(commName, argv[index+1]);
  OSAPI_STRNCPY_SAFE(strIpAddrNew, argv[index+3]);

  /* #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
     if(strcmp(argv[index],"ip6addr") == 0)
     {
     ipAddrType=6;
     if((osapiInetPton(L7_AF_INET6, strIpAddr, (L7_uchar8 *)&ipv6_address)!=L7_SUCCESS) ||
     (osapiInetPton(L7_AF_INET6, strIpAddrNew, (L7_uchar8 *)&ipv6_address_new)!=L7_SUCCESS))

     {
     return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGNETWORKIP);
     }
     }
#endif
   */
  if(strcmp(argv[index],pStrInfo_base_Ipaddr_2) == 0)
  {
    ipAddrType=4;
    
    /* validate the ip address as ipv4*/
    if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS || (ipAddr == 0))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
    }
    else if(cliIPHostAddressValidate(ewsContext, strIpAddrNew, &ipAddrNew, &addrTypeNew) != L7_SUCCESS)
    {
      /* Invalid Host Address */
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
    }

    if (addrTypeNew == L7_IP_ADDRESS_TYPE_IPV4)
    {
      if (usmDbInetAton(strIpAddrNew, &ipAddrNew) != L7_SUCCESS || (ipAddrNew == 0))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
      }
    }
    else if(addrTypeNew == L7_IP_ADDRESS_TYPE_DNS)
    {
      inetAddressReset(&inetAddrNew);
      rc = usmDbDNSClientInetNameLookup(L7_AF_INET, strIpAddrNew, &status, domainName, &inetAddrNew);
      /*  If the DNS lookup fails */
      if (rc == L7_SUCCESS)
      {
        inetAddrHtop(&inetAddrNew, strIpAddrNew);
        if (usmDbInetAton(strIpAddrNew, &ipAddrNew) != L7_SUCCESS || (ipAddrNew == 0))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_base_DnsLookupFailed);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
    } 
  }
  /*trapIndex = cliFindSNMPTrapCommunityIndex(commName, ipAddr, ipv6_address, ipAddrType);*/
  if(ipAddrType==4)
  {
    trapIndex = cliFindSNMPTrapCommunityIndex(commName, ipAddr);
  }
  else
  {
    trapIndex = cliFindSNMPTrapCommunityV6Index(commName, strIpAddr);
  }

  if (trapIndex == CLI_INDEX_NOT_FOUND)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SnmpTrapDoesntExist);
  }
  else
  {
    usmDbTrapAuthSwGet(trapIndex, &currentConfig); /*save the config*/

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbTrapManagerStatusIpSet(unit, trapIndex, L7_SNMP_TRAP_MGR_STATUS_CONFIG);
      /*set it to config mode*/
      if (rc == L7_FAILURE)
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FailedToSetCfgForChg);
      }
      /*#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
        if(ipAddrType==6)
        rc = usmDbTrapManagerIPv6AddrSet(unit, trapIndex, &ipv6_address);
#endif
       */
      if(ipAddrType==4)
      {
        rc = usmDbTrapManagerIpAddrSet(unit, trapIndex, strIpAddrNew);
      }
      /*set the address*/

      if (rc == L7_FAILURE)
      {
        ewsTelnetWrite( ewsContext, pStrErr_base_FailedToSetNewIp);
        usmDbTrapManagerStatusIpSet(unit, trapIndex, currentConfig);
        return cliSyntaxReturnPrompt (ewsContext, "");
      }

      rc = usmDbTrapManagerStatusIpSet(unit, trapIndex, currentConfig);
      /*reset the config*/
      if (rc == L7_FAILURE)
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FailedToResetCfg);
      }
    }

  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose to change an SNMP community trap version based on its community name
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  snmptrap snmpversion <name> <ipaddr|hostname> <snmpversion>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpTrapVersion(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index )
{
  L7_uchar8 commName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_char8 domainName[L7_DNS_DOMAIN_NAME_SIZE_MAX];
  L7_inet_addr_t inetAddr;
  L7_uchar8 trapVersion[L7_CLI_MAX_STRING_LENGTH];
  L7_int32  trapIndex;
  L7_uint32 currentConfig;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 numArg, ipAddr = 0;
  snmpTrapVer_t version;
  L7_BOOL validInterface  = L7_FALSE;
  L7_uint32 ipAddrType=0;
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  L7_in6_addr_t ipv6_address;
  L7_inet_addr_t destinationIP;

  memset(&ipv6_address, 0, sizeof(L7_in6_addr_t));
#endif
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 3)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrInfo_base_IncorrectInputUseTrapVerNew);
  }

  if ((strlen(argv[index+1]) >= sizeof(commName)) ||
      (strlen(argv[index+2]) >= sizeof(strIpAddr)) ||
      (strlen(argv[index+3]) >= sizeof(trapVersion)))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseTrapVerNew);
  }

  OSAPI_STRNCPY_SAFE(commName, argv[index+1]);
  /* check the length of the name to be 16 or fewer */
  if ( strlen(commName) > L7_SNMP_NAME_SIZE )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_NameMustBe16OrFew);
  }

  OSAPI_STRNCPY_SAFE(strIpAddr, argv[index+2]);
  
  /* Verify if the specified ip address is valid */
  if (usmDbInetAton(strIpAddr, &ipAddr) == L7_SUCCESS || (ipAddr != 0))
  {
    ipAddrType = 4;
    validInterface  = L7_TRUE;
  }
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  else if(osapiInetPton(L7_AF_INET6, strIpAddr, (L7_uchar8 *)&ipv6_address) ==L7_SUCCESS)
  {
    ipAddrType =6;
    memset(strIpAddr, 0, sizeof(strIpAddr));
    osapiInetNtop(L7_AF_INET6, (char *)&ipv6_address, strIpAddr, sizeof(strIpAddr));
    validInterface  = L7_TRUE;
  }
#endif
 
/*look for DNS  */ 
  else if (usmDbDNSClientInetNameLookup(L7_AF_INET, strIpAddr, &status, domainName, &inetAddr) == L7_SUCCESS)
  {
    ipAddrType = 4;
    inetAddrHtop(&inetAddr, strIpAddr);
    if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS || (ipAddr == 0))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
    }
    validInterface  = L7_TRUE;
  }
  #if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  else if(usmDbDNSClientInetNameLookup(L7_AF_INET6, strIpAddr, &status, domainName, &destinationIP) == L7_SUCCESS)
  {
    ipAddrType =6;
    if (inet6AddressGet(&destinationIP, &ipv6_address) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
    }
    osapiInetNtop(L7_AF_INET6, (char *)&ipv6_address, strIpAddr, sizeof(strIpAddr));
    validInterface  = L7_TRUE;
  }
  #endif
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_base_DnsLookupFailed);
  }
  
  OSAPI_STRNCPY_SAFE(trapVersion, argv[index+3]);

  if(strcmp(trapVersion,pStrInfo_base_SnmpVer1)== 0)
  {
    version = L7_SNMP_TRAP_VER_SNMPV1;
  }
  else if(strcmp(trapVersion,pStrInfo_base_SnmpVer2)==0)
  {
    version = L7_SNMP_TRAP_VER_SNMPV2C;
  }
  else
  {
    version = L7_SNMP_TRAP_VER_SNMPV2C;
  }

  if(ipAddrType==4)
  {
    trapIndex = cliFindSNMPTrapCommunityIndex(commName, ipAddr);
  }
  else
  {
    trapIndex = cliFindSNMPTrapCommunityV6Index(commName, strIpAddr);
  }

  if (trapIndex == CLI_INDEX_NOT_FOUND)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SnmpTrapDoesntExist);
    }
  }
  else
  {
    usmDbTrapAuthSwGet(trapIndex, &currentConfig); /*save the config*/
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbTrapManagerStatusIpSet(unit, trapIndex, L7_SNMP_TRAP_MGR_STATUS_CONFIG);
      /*set it to config mode*/
      if (rc == L7_FAILURE)
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FailedToSetCfgForChg);
      }

      rc = usmDbTrapManagerVersionSet(unit, trapIndex, version);
      /*set the trap version*/
      if (rc == L7_FAILURE)
      {
        ewsTelnetWrite( ewsContext, pStrErr_base_FailedToSetTrapVer);
        usmDbTrapManagerStatusIpSet(unit, trapIndex, currentConfig);
        return cliSyntaxReturnPrompt (ewsContext, "");
      }

      rc = usmDbTrapManagerStatusIpSet(unit, trapIndex, currentConfig);
      /*reset the config*/
      if (rc == L7_FAILURE)
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FailedToResetCfg);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
 *
 * @purpose to enable or disable an SNMP Community Trap
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax snmptrap mode <name> <ipaddress|hostname>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   Trap receivers with Enabled status are active and the SNMP agent
 *   sends traps to them.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpTrapMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index )
{
  L7_uchar8 commName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_char8 domainName[L7_DNS_DOMAIN_NAME_SIZE_MAX];
  L7_inet_addr_t inetAddr;
  L7_int32 trapIndex = 0;
  L7_RC_t rc;
  L7_uint32 ipAddr;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 arg1 = 1;
  L7_uint32 arg2 = 2;
  L7_BOOL validAddress = L7_FALSE;

  L7_uint32 ipAddrType=0;
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  L7_in6_addr_t ipv6_address;
  L7_inet_addr_t destinationIP;

  memset(&ipv6_address, 0, sizeof(L7_in6_addr_t));
#endif
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 2)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseModeEnblDsblNameIpAddr);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseNoModeEnblDsblNameIpAddr);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if ((strlen(argv[index+arg1]) >= sizeof(commName)) ||
      (strlen(argv[index+arg2]) >= sizeof(strIpAddr)))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseModeEnblDsblNameIpAddr);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseNoModeEnblDsblNameIpAddr);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  OSAPI_STRNCPY_SAFE(commName, argv[index+arg1]);
  OSAPI_STRNCPY_SAFE(strIpAddr, argv[index+arg2]);

  /* Verify if the specified ip address is valid */

  if (usmDbInetAton(strIpAddr, &ipAddr) == L7_SUCCESS || (ipAddr != 0))
  {
    ipAddrType=4;
    validAddress = L7_TRUE;
  }

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  else if(osapiInetPton(L7_AF_INET6, strIpAddr, (L7_uchar8 *)&ipv6_address) ==L7_SUCCESS)
  {
    ipAddrType=6;
    memset(strIpAddr, 0, sizeof(strIpAddr));
    osapiInetNtop(L7_AF_INET6, (char *)&ipv6_address, strIpAddr, sizeof(strIpAddr));
    validAddress = L7_TRUE;
  }
#endif
  /*look for DNS  */ 
  else if (usmDbDNSClientInetNameLookup(L7_AF_INET, strIpAddr, &status, domainName, &inetAddr) == L7_SUCCESS)
  {
    ipAddrType = 4;
    inetAddrHtop(&inetAddr, strIpAddr);
    if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS || (ipAddr == 0))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
    }
    validAddress = L7_TRUE;
  }
  #if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  else if(usmDbDNSClientInetNameLookup(L7_AF_INET6, strIpAddr, &status, domainName, &destinationIP) == L7_SUCCESS)
  {
    ipAddrType =6;
    if (inet6AddressGet(&destinationIP, &ipv6_address) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
    }
    osapiInetNtop(L7_AF_INET6, (char *)&ipv6_address, strIpAddr, sizeof(strIpAddr));
    validAddress = L7_TRUE;
  }
  #endif
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_base_DnsLookupFailed);
  }


  /* check if the snmp trap already exist */
  if(ipAddrType==4)
  {
    trapIndex = cliFindSNMPTrapCommunityIndex(commName, ipAddr);
  }
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  else
    trapIndex = cliFindSNMPTrapCommunityV6Index(commName, strIpAddr);
#endif
  if (trapIndex == CLI_INDEX_NOT_FOUND)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SnmpTrapDoesntExist);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      usmDbTrapManagerStatusIpSet(unit, trapIndex, L7_SNMP_COMMUNITY_STATUS_VALID);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      rc = usmDbTrapManagerStatusIpSet(unit, trapIndex, L7_SNMP_COMMUNITY_STATUS_INVALID);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable or disable new telnet connections
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes number of arguments checking
 *
 * @cmdsyntax  transport input telnet
 *
 * @cmdhelp Allow or disallow new telnet sessions.
 *
 * @cmddescript
 *   Enable means that new Telnet sessions can be established until
 *   there are no more sessions available. Any already established
 *   session remains active until the session is ended or an abnormal
 *   network error ends it. The default value is Enable.
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandTelnetCon(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 mode=0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTelnetStatus_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoTelnetStatus);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if(usmDbAgentTelnetNewSessionsSet(unit, mode)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  configure the max telnet sessions
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  telnetcon maxsessions <0-5>
 *
 * @cmdhelp Configure the number of telnet sessions allowed.
 *
 * @cmddescript
 *   Specify a decimal value from 0 to 5. If the value is 0, no Telnet
 *   session can be established. The default value is 5.
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandTelnetConMaxSessions(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 tempInt = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 arg1 = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgRemoteConMaxSession);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+arg1],  &tempInt)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoRemoteConMaxSession);
    }
    tempInt = FD_CLI_WEB_DEFAULT_NUM_SESSIONS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbAgentTelnetNumSessionsSet(unit, tempInt);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

const L7_char8 *commandRemoteConMaxSessions(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 tempInt = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 arg1 = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgRemoteConMaxSession);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+arg1],  &tempInt)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoRemoteConMaxSession);
    }
    tempInt = FD_CLI_WEB_DEFAULT_NUM_SESSIONS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbAgentTelnetNumSessionsSet(unit, tempInt);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable/disable admin mode for one port
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  shutdown
 *
 * @cmdhelp Enable/Disable the port's administrative mode.
 *
 * @cmddescript
 *   This is a configurable value and indicates if the port is
 *   enabled or disabled. The default for all ports is Enabled.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandShutdown(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 interface;
  L7_uint32 val=0;
  L7_uint32 numArg;
  L7_uint32 unit, s, p;
  L7_RC_t status=L7_SUCCESS;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_Shutdown_1);
    }
    val = L7_DISABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShutdownNo);
    }
    val = L7_ENABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_Shutdown_1);
  }

  unit = cliGetUnitId();

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        rc = usmDbIfAdminStateSet(unit, interface, val);
        if (rc == L7_NOT_SUPPORTED)
        {
          memset(buf, 0, sizeof(buf));
          sprintfAddBlanks(1,1, 0, 0, L7_NULLPTR, buf, pStrInfo_base_FeatNotSupported, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWrite(ewsContext, buf);
          status = L7_ERROR;
        }
        else if (rc != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_Shutdown_1);
          status = L7_ERROR;
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  if(status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable/disable admin mode for  all ports
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  shutdown all
 *
 * @cmdhelp Enable/Disable the port's administrative mode.
 *
 * @cmddescript
 *   This is a configurable value and indicates if the port is
 *   enabled or disabled. The default for all ports is Enabled.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandShutdownAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface, nextInterface;
  L7_uint32 val=0;
  L7_uint32 numArg;

  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShutdownAll_1);
    }
    val = L7_DISABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShutdownAllNo);
    }
    val = L7_ENABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShutdownAll_1);
  }

  if (L7_SUCCESS != usmDbValidIntIfNumFirstGet(&interface))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox_1);
  }

  while (interface)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbIfAdminStateSet(unit, interface, val);
    }

    if (L7_SUCCESS != usmDbValidIntIfNumNext(interface, &nextInterface))
    {
      interface = 0;
    }
    else
    {
      interface = nextInterface;
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable/disable auto negotiate mode for a ports
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  auto-negotiate
 *
 * @cmdhelp Enable/Disable the port's auto negotiate mode.
 *
 * @cmddescript
 *   This is a configurable value and indicates if the port's
 *   auto negotiate mode is enabled or disabled. The default
 *   for all ports is Enabled.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandAutoNegotiate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 interface; /* nextInterface;*/
  L7_uint32 val=0, intIfType;

  L7_uint32 phyCapability;
  /*L7_BOOL portChannelHasMembers = L7_FALSE;*/
  L7_uint32 unit, s, p;

  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_Duplex_1);
    }
    val = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DuplexNo);
    }
    val = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_Duplex_1);
  }

  unit = cliGetUnitId();


  for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
  {
    if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
    {
      if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      usmDbIntfPhyCapabilityGet(interface, &phyCapability);
      if (usmDbDot3adIsMember(unit, interface) == L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_Cant,  ewsContext, pStrInfo_base_AutoNegotiate_1);
        status = L7_ERROR;
        continue;
      }

      rc = usmDbIfTypeGet(unit, interface, &intIfType);
      if ( intIfType == L7_IANA_GIGABIT_ETHERNET &&
           (phyCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG) != L7_TRUE)
      {
        ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_OperationHasNoEffectOnGigabitPorts);
        status = L7_ERROR;
        continue;
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbIfAutoNegoStatusCapabilitiesSet(interface, val ? L7_PORT_NEGO_CAPABILITY_ALL : 0);
        if (rc != L7_SUCCESS)
        {
          /* port selected cannot perform this function */
          ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          status = L7_ERROR;
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  if(status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable/disable auto negotiate mode for all ports
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  auto-negotiate all
 *
 * @cmdhelp Enable/Disable the port's auto negotiate mode.
 *
 * @cmddescript
 *   This is a configurable value and indicates if the port's
 *   auto negotiate mode is enabled or disabled. The default
 *   for all ports is Enabled.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandAutoNegotiateAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface, nextInterface;
  L7_uint32 val=0; /* intIfType;*/
  /* L7_uint32 phyCapability;*/
  L7_BOOL portChannelHasMembers = L7_FALSE;
  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DuplexAll_1);
    }
    val = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DuplexAllNo);
    }
    val = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_DuplexAll_1);
  }

  if (L7_SUCCESS != usmDbValidIntIfNumFirstGet(&interface))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox_1);
  }

  while (interface)
  {
    if (usmDbDot3adIsMember(unit, interface) == L7_SUCCESS)
    {
      portChannelHasMembers = L7_TRUE;
    }

    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbIfAutoNegoStatusCapabilitiesSet(interface, val ? L7_PORT_NEGO_CAPABILITY_ALL : 0);
      }
    }
    if (L7_SUCCESS != usmDbValidIntIfNumNext(interface, &nextInterface))
    {
      interface = 0;
    }
    else
    {
      interface = nextInterface;
    }
  }

  if (portChannelHasMembers == L7_TRUE)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_base_SetAutoNegotiate);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable/disable the lacpmode for a port
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  port lacpmode
 *
 * @cmdhelp Enable/Disable LACP mode for this port.
 *
 * @cmddescript
 *   This is a configurable value and can be Enabled or Disabled. It
 *   allows you to enable or disable the Link Aggregation Control
 *   Protocol (LACP) mode by port. The default value of this
 *   parameter is disabled.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortLacpMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 interface; /*nextInterface;*/
  L7_uint32 s, p, itype;
  L7_uint32 val=0;

  L7_uint32 unit;
  L7_uint32 numArg;

  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpMode_1);
    }
    val = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpModeNo);
    }
    val = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpMode_1);
  }

  for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
  {
    if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
    {
      if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( usmDbIntfTypeGet(interface, &itype) == L7_SUCCESS )
      {
        if(itype == L7_LAG_INTF)
        {
          ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_LagCantEnblDsbl);
          status = L7_FAILURE;
          continue;
        }
      }
    
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbDot3adAggPortLacpModeSet(unit, interface, val);
        if (rc == L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          status = L7_FAILURE;
        }
        else if (rc != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_PortLacpMode_1);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable/disable the lacpmode for all ports
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  port lacpmode all
 *
 * @cmdhelp Enable/Disable LACP mode for all ports.
 *
 * @cmddescript
 *   This is a configurable value and can be Enabled or Disabled. It
 *   allows you to enable or disable the Link Aggregation Control
 *   Protocol (LACP) mode by port. The default value of this
 *   parameter is disabled.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortLacpModeAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface, nextInterface;
  L7_uint32 val=0;

  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpModeAll_1);
    }
    val = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpModeAllNo);
    }
    val = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpModeAll_1);
  }

  /* if interface entered was 'all' */
  if (L7_SUCCESS != usmDbValidIntIfNumFirstGet(&interface))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox_1);
  }

  while (interface)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbDot3adAggPortLacpModeSet(unit, interface, val);
    }
    if (L7_SUCCESS != usmDbValidIntIfNumNext(interface, &nextInterface))
    {
      interface = 0;
    }
    else
    {
      interface = nextInterface;
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable or disable broadcast storm recovery globally
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  storm-control broadcast [all] [level <0-x>]
 *
 * @cmdhelp Enable/Disable broadcast storm recovery.
 *
 * @cmddescript
 *   When you specify Enable for Broadcast Storm Recovery and the broadcast
 *   traffic on any Ethernet port exceeds the configured rate level, the
 *   switch blocks (discards) the broadcast traffic until the broadcast
 *   traffic returns to below this value.
 *
 *   When you specify Disable for Broadcast Recovery Mode, then the switch
 *   will not block any broadcast traffic on any Ethernet port.
 *   The default is Disable.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandStormControlBcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 threshold;
  L7_char8 * ptmp;
  L7_char8 tmpMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmpMsg1[L7_CLI_MAX_STRING_LENGTH];
  L7_RATE_UNIT_t rate_unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg == 0)       /* setting storm-control mode only */
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlBcastStormModeSet(unit, L7_ENABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcast_1);
        }
      }
    }
    else if (numArg == 2)       
    {
      if (strcmp(argv[index+1], "level") == 0)
      {   /* setting storm-control threshold as percentage */
        ptmp = strstr(argv[index+2], ".");   /* check if decimal value was entered */
        if (ptmp != L7_NULL)
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], min((strlen(argv[index+2])-strlen(ptmp)+1), sizeof(tmpMsg)));
          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          threshold = atoi(tmpMsg);

          strncpy(tmpMsg1, ptmp+1, sizeof(tmpMsg1));
          if(cliCheckIfInteger(tmpMsg1) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          if (atoi(ptmp+1) >= 5)      /* round-up if greater than 0.5 decimal */
          {
            threshold++;
          }

        }
        else
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], sizeof(tmpMsg));

          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);
          }

          threshold = atoi(argv[index+2]);
        }

        /* check if threshold value is within valid range */
        if (threshold < L7_STORMCONTROL_LEVEL_MIN || threshold > L7_STORMCONTROL_LEVEL_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN, L7_STORMCONTROL_LEVEL_MAX);
        }
        rate_unit = L7_RATE_UNIT_PERCENT;
      }
      else if (strcmp(argv[index+1], "rate") == 0) 
      {
        threshold = atoi(argv[index+2]);

        /* check if threshold value is within valid range */
        if ((threshold < L7_STORMCONTROL_RATE_MIN) || (threshold > L7_STORMCONTROL_RATE_MAX)) 
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlRate, L7_STORMCONTROL_RATE_MIN, L7_STORMCONTROL_RATE_MAX);
        }
        rate_unit = L7_RATE_UNIT_PPS;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlBcast);
      }
    
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlBcastStormThresholdSet(unit, threshold,rate_unit);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcastLvl_1);
        }
        rc = usmDbSwDevCtrlBcastStormModeSet(unit, L7_ENABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcast_1);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlBcast);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlBcastStormModeSet(unit, L7_DISABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcast_1);
        }
      }

    }
    else if (numArg == 1)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlBcastStormThresholdSet(unit, FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD,
                                                  FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD_UNIT);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcastLvl_1);
        }
        rc = usmDbSwDevCtrlBcastStormModeSet(unit, L7_DISABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcast_1);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlBcastNo);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlBcast);

  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable or disable multicast storm recovery globally
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  storm-control multicast [all] [level <0-x>]
 *
 * @cmdhelp Enable/Disable multicast storm recovery.
 *
 * @cmddescript
 *   When you specify Enable for multicast Storm Recovery and the multicast
 *   traffic on any Ethernet port exceeds the configured rate level, the
 *   switch blocks (discards) the multicast traffic until the multicast
 *   traffic returns to below this value.
 *
 *   When you specify Disable for multicast Recovery Mode, then the switch
 *   will not block any multicast traffic on any Ethernet port.
 *   The default is Disable.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandStormControlMcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit;
  L7_uint32 threshold;
  L7_uint32 numArg;
  L7_char8 * ptmp;
  L7_char8 tmpMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmpMsg1[L7_CLI_MAX_STRING_LENGTH];
  L7_RATE_UNIT_t rate_unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg == 0)       /* setting storm-control mode only */
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlMcastStormModeSet(unit, L7_ENABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcast_1);
        }

      }
    }
    else if (numArg == 2)     
    {
      if (strcmp(argv[index+1], "level") == 0)
      {   /* setting storm-control threshold as percentage */
        ptmp = strstr(argv[index+2], ".");   /* check if decimal value was entered */
        if (ptmp != L7_NULL)
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], min((strlen(argv[index+2])-strlen(ptmp)+1), sizeof(tmpMsg)));
          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          threshold = atoi(tmpMsg);

          strncpy(tmpMsg1, ptmp+1, sizeof(tmpMsg1));
          if(cliCheckIfInteger(tmpMsg1) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          if (atoi(ptmp+1) >= 5)      /* round-up if greater than 0.5 decimal */
          {
            threshold++;
          }

        }
        else
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], sizeof(tmpMsg));

          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);
          }

          threshold = atoi(argv[index+2]);
        }

        if (threshold < L7_STORMCONTROL_LEVEL_MIN || threshold > L7_STORMCONTROL_LEVEL_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN, L7_STORMCONTROL_LEVEL_MAX);
        }
        rate_unit = L7_RATE_UNIT_PERCENT;
        }
          else if (strcmp(argv[index+1], "rate") == 0) 
        {
        threshold = atoi(argv[index+2]);

        /* check if threshold value is within valid range */
        if ((threshold < L7_STORMCONTROL_RATE_MIN) || (threshold > L7_STORMCONTROL_RATE_MAX)) 
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlRate, L7_STORMCONTROL_RATE_MIN, L7_STORMCONTROL_RATE_MAX);
        }
        rate_unit = L7_RATE_UNIT_PPS;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlMcast);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlMcastStormThresholdSet(unit, threshold, rate_unit);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcastLvl_1);
        }
        rc = usmDbSwDevCtrlMcastStormModeSet(unit, L7_ENABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcast_1);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlMcast);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlMcastStormModeSet(unit, L7_DISABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcast_1);
        }
      }
    }
    else if (numArg == 1)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlMcastStormThresholdSet(unit, FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD,
                                                  FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD_UNIT);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcastLvl_1);
        }
        rc = usmDbSwDevCtrlMcastStormModeSet(unit, L7_DISABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcast_1);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlMcastNo);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlMcast);

  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable or disable destination lookup failure storm recovery globally
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  storm-control unicast [all] [level <0-x>]
 *
 * @cmdhelp Enable/Disable destination lookup failure storm recovery.
 *
 * @cmddescript
 *   When you specify Enable for unicast Storm Recovery and the unknown unicast
 *   traffic on any Ethernet port exceeds the configured rate level, the
 *   switch blocks (discards) the unicast traffic until the unicast
 *   traffic returns to below this value.
 *
 *   When you specify Disable for unicast Recovery Mode, then the switch
 *   will not block any unicast traffic on any Ethernet port.
 *   The default is Disable.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandStormControlUcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit;
  L7_uint32 threshold;
  L7_uint32 numArg;
  L7_char8 * ptmp;
  L7_char8 tmpMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmpMsg1[L7_CLI_MAX_STRING_LENGTH];
  L7_RATE_UNIT_t rate_unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg == 0)     /* setting storm-control mode only */
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlUcastStormModeSet(unit, L7_ENABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcast_1);
        }
      }
    }
    else if (numArg == 2)    
    {
      if (strcmp(argv[index+1], "level") == 0)
      {   /* setting storm-control threshold as percentage */
        ptmp = strstr(argv[index+2], ".");   /* check if decimal value was entered */
        if (ptmp != L7_NULL)
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], min((strlen(argv[index+2])-strlen(ptmp)+1), sizeof(tmpMsg)));
          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          threshold = atoi(tmpMsg);

          strncpy(tmpMsg1, ptmp+1, sizeof(tmpMsg1));
          if(cliCheckIfInteger(tmpMsg1) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          if (atoi(ptmp+1) >= 5)      /* round-up if greater than 0.5 decimal */
          {
            threshold++;
          }

        }
        else
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], sizeof(tmpMsg));

          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);
          }

          threshold = atoi(argv[index+2]);
        }

        if (threshold < L7_STORMCONTROL_LEVEL_MIN || threshold > L7_STORMCONTROL_LEVEL_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN, L7_STORMCONTROL_LEVEL_MAX);
        }
        rate_unit = L7_RATE_UNIT_PERCENT;
      } 
      else if (strcmp(argv[index+1], "rate") == 0) 
      {
        threshold = atoi(argv[index+2]);

        /* check if threshold value is within valid range */
        if ((threshold < L7_STORMCONTROL_RATE_MIN) || (threshold > L7_STORMCONTROL_RATE_MAX)) 
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlRate, L7_STORMCONTROL_RATE_MIN, L7_STORMCONTROL_RATE_MAX);
        }
        rate_unit = L7_RATE_UNIT_PPS;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlUcast);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlUcastStormThresholdSet(unit, threshold, rate_unit);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcastLvl_1);
        }
        rc = usmDbSwDevCtrlUcastStormModeSet(unit, L7_ENABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcast_1);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlUcast);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlUcastStormModeSet(unit, L7_DISABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcast_1);
        }
      }
    }
    else if (numArg == 1)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwDevCtrlUcastStormThresholdSet(unit, FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD,
                                                  FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD_UNIT);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcastLvl_1);
        }
        rc = usmDbSwDevCtrlUcastStormModeSet(unit, L7_DISABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcast_1);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlUcastNo);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlUcast);

  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable or disable broadcast storm recovery for an interface
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  storm-control broadcast [level <0-x>]
 *
 * @cmdhelp Enable/Disable broadcast storm recovery.
 *
 * @cmddescript
 *   When you specify Enable for Broadcast Storm Recovery and the broadcast
 *   traffic on any Ethernet port exceeds the configured rate level, the
 *   switch blocks (discards) the broadcast traffic until the broadcast
 *   traffic returns to below this value.
 *
 *   When you specify Disable for Broadcast Recovery Mode, then the switch
 *   will not block any broadcast traffic on any Ethernet port.
 *   The default is Disable.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandStormControlIntfBcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit;
  L7_uint32 s,p;
  L7_uint32 interface;
  L7_uint32 threshold;
  L7_uint32 numArg;
  L7_char8 * ptmp;
  L7_char8 tmpMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmpMsg1[L7_CLI_MAX_STRING_LENGTH];
  L7_RATE_UNIT_t rate_unit;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_StormCntrlBcast_2);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg == 0)             /* setting storm-control mode only */
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            rc = usmDbSwDevCtrlBcastStormModeIntfSet(interface, L7_ENABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcast_1);
              status = L7_FAILURE;
            }
          }
        }
      }
    }
    else if (numArg == 2)  
    {
      if (strcmp(argv[index+1], "level") == 0)
      {   /* setting storm-control threshold as percentage */
        ptmp = strstr(argv[index+2], ".");   /* check if decimal value was entered */
        if (ptmp != L7_NULL)
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], min((strlen(argv[index+2])-strlen(ptmp)+1), sizeof(tmpMsg)));
          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          threshold = atoi(tmpMsg);

          strncpy(tmpMsg1, ptmp+1, sizeof(tmpMsg1));
          if(cliCheckIfInteger(tmpMsg1) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          if (atoi(ptmp+1) >= 5)      /* round-up if greater than 0.5 decimal */
          {
            threshold++;
          }

        }
        else
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], sizeof(tmpMsg));

          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);
          }

          threshold = atoi(argv[index+2]);
        }

        if (threshold < L7_STORMCONTROL_LEVEL_MIN || threshold > L7_STORMCONTROL_LEVEL_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN, L7_STORMCONTROL_LEVEL_MAX);
        }
        rate_unit = L7_RATE_UNIT_PERCENT;
      } 
      else if (strcmp(argv[index+1], "rate") == 0) 
      {
        threshold = atoi(argv[index+2]);

        /* check if threshold value is within valid range */
        if ((threshold < L7_STORMCONTROL_RATE_MIN) || (threshold > L7_STORMCONTROL_RATE_MAX)) 
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlRate, L7_STORMCONTROL_RATE_MIN, L7_STORMCONTROL_RATE_MAX);
        }
        rate_unit = L7_RATE_UNIT_PPS;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlBcast);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            rc = usmDbSwDevCtrlBcastStormThresholdIntfSet(interface, threshold, rate_unit);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcastLvl_1);
              status = L7_FAILURE;
              continue;
            }
            rc = usmDbSwDevCtrlBcastStormModeIntfSet(interface, L7_ENABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcast_1);
              status = L7_FAILURE;
            }
          }
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlBcast);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            rc = usmDbSwDevCtrlBcastStormModeIntfSet(interface, L7_DISABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcast_1);
              status = L7_FAILURE;
            }
          }
        }
      }
    }
    else if (numArg == 1)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            rc = usmDbSwDevCtrlBcastStormThresholdIntfSet(interface, FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD,
                                                          FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD_UNIT);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcastLvl_1);
              status = L7_FAILURE;
              continue;
            }
            rc = usmDbSwDevCtrlBcastStormModeIntfSet(interface, L7_DISABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlBcast_1);
              status = L7_FAILURE;
            }
          }
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlBcastNo);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlBcast);

  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable or disable multicast storm recovery for an interface
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  storm-control multicast [level <0-x>]
 *
 * @cmdhelp Enable/Disable multicast storm recovery.
 *
 * @cmddescript
 *   When you specify Enable for multicast Storm Recovery and the multicast
 *   traffic on any Ethernet port exceeds the configured rate level, the
 *   switch blocks (discards) the multicast traffic until the multicast
 *   traffic returns to below this value.
 *
 *   When you specify Disable for multicast Recovery Mode, then the switch
 *   will not block any multicast traffic on any Ethernet port.
 *   The default is Disable.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandStormControlIntfMcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit;
  L7_uint32 s,p;
  L7_uint32 interface;
  L7_uint32 threshold;
  L7_uint32 numArg;
  L7_char8 * ptmp;
  L7_char8 tmpMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmpMsg1[L7_CLI_MAX_STRING_LENGTH];
  L7_RATE_UNIT_t rate_unit;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg == 0)           /* setting storm-control mode only */
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            rc = usmDbSwDevCtrlMcastStormModeIntfSet(interface, L7_ENABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcast_1);
              status = L7_FAILURE;
              continue;
            }
          }
        }
      }
    }
    else if (numArg == 2)    
    {
      if (strcmp(argv[index+1], "level") == 0)
      {   /* setting storm-control threshold as percentage */
        ptmp = strstr(argv[index+2], ".");   /* check if decimal value was entered */
        if (ptmp != L7_NULL)
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], min((strlen(argv[index+2])-strlen(ptmp)+1), sizeof(tmpMsg)));
          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          threshold = atoi(tmpMsg);

          strncpy(tmpMsg1, ptmp+1, sizeof(tmpMsg1));
          if(cliCheckIfInteger(tmpMsg1) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          if (atoi(ptmp+1) >= 5)      /* round-up if greater than 0.5 decimal */
          {
            threshold++;
          }

        }
        else
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], sizeof(tmpMsg));

          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);
          }

          threshold = atoi(argv[index+2]);
        }

        if (threshold < L7_STORMCONTROL_LEVEL_MIN || threshold > L7_STORMCONTROL_LEVEL_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN, L7_STORMCONTROL_LEVEL_MAX);
        }
        rate_unit = L7_RATE_UNIT_PERCENT;
      } 
      else if (strcmp(argv[index+1], "rate") == 0) 
      {
        threshold = atoi(argv[index+2]);

        /* check if threshold value is within valid range */
        if ((threshold < L7_STORMCONTROL_RATE_MIN) || (threshold > L7_STORMCONTROL_RATE_MAX)) 
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlRate, L7_STORMCONTROL_RATE_MIN, L7_STORMCONTROL_RATE_MAX);
        }
        rate_unit = L7_RATE_UNIT_PPS;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlMcast);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            rc = usmDbSwDevCtrlMcastStormThresholdIntfSet(interface, threshold, rate_unit);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcastLvl_1);
              status = L7_FAILURE;
              continue;
            }
            rc = usmDbSwDevCtrlMcastStormModeIntfSet(interface, L7_ENABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcast_1);
              status = L7_FAILURE;
            }
          }
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlMcast);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            rc = usmDbSwDevCtrlMcastStormModeIntfSet(interface, L7_DISABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcast_1);
              status = L7_FAILURE;
              continue;
            }
          }
        }
      }
    }
    else if (numArg == 1)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            rc = usmDbSwDevCtrlMcastStormThresholdIntfSet(interface, FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD,
                                                          FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD_UNIT);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcastLvl_1);
              status = L7_FAILURE;
              continue;
            }
            rc = usmDbSwDevCtrlMcastStormModeIntfSet(interface, L7_DISABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlMcast_1);
              status = L7_FAILURE;
            }
          }
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlMcastNo);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlMcast);

  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable or disable destination lookup failure storm recovery for an interface
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  storm-control unicast [level <0-x>]
 *
 * @cmdhelp Enable/Disable destination lookup failure storm recovery.
 *
 * @cmddescript
 *   When you specify Enable for unicast Storm Recovery and the unknown unicast
 *   traffic on any Ethernet port exceeds the configured rate level, the
 *   switch blocks (discards) the unicast traffic until the unicast
 *   traffic returns to below this value.
 *
 *   When you specify Disable for unicast Recovery Mode, then the switch
 *   will not block any unicast traffic on any Ethernet port.
 *   The default is Disable.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandStormControlIntfUcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit;
  L7_uint32 s,p;
  L7_uint32 interface;
  L7_uint32 threshold;
  L7_uint32 numArg;
  L7_char8 * ptmp;
  L7_char8 tmpMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmpMsg1[L7_CLI_MAX_STRING_LENGTH];
  L7_RATE_UNIT_t rate_unit;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg == 0)         /* setting storm-control mode only */
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }
            rc = usmDbSwDevCtrlUcastStormModeIntfSet(interface, L7_ENABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcast_1);
              status = L7_FAILURE;
            }
          }
        }
      }
    }
    else if (numArg == 2)   
    {
      if (strcmp(argv[index+1], "level") == 0)
      {   /* setting storm-control threshold as percentage */
        ptmp = strstr(argv[index+2], ".");   /* check if decimal value was entered */
        if (ptmp != L7_NULL)
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], min((strlen(argv[index+2])-strlen(ptmp)+1), sizeof(tmpMsg)));
          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          threshold = atoi(tmpMsg);

          strncpy(tmpMsg1, ptmp+1, sizeof(tmpMsg1));
          if(cliCheckIfInteger(tmpMsg1) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);

          }
          if (atoi(ptmp+1) >= 5)      /* round-up if greater than 0.5 decimal */
          {
            threshold++;
          }

        }
        else
        {
          osapiStrncpySafe(tmpMsg, argv[index+2], sizeof(tmpMsg));

          if(cliCheckIfInteger(tmpMsg) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                                   pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN,
                                                   L7_STORMCONTROL_LEVEL_MAX);
          }

          threshold = atoi(argv[index+2]);
        }

        if (threshold < L7_STORMCONTROL_LEVEL_MIN || threshold > L7_STORMCONTROL_LEVEL_MAX)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlLvl, L7_STORMCONTROL_LEVEL_MIN, L7_STORMCONTROL_LEVEL_MAX);
        }
         rate_unit = L7_RATE_UNIT_PERCENT;
      } 
      else if (strcmp(argv[index+1], "rate") == 0) 
      {
        threshold = atoi(argv[index+2]);

        /* check if threshold value is within valid range */
        if ((threshold < L7_STORMCONTROL_RATE_MIN) || (threshold > L7_STORMCONTROL_RATE_MAX)) 
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_StormCntrlRate, L7_STORMCONTROL_RATE_MIN, L7_STORMCONTROL_RATE_MAX);
        }
        rate_unit = L7_RATE_UNIT_PPS;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlUcast);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }
            rc = usmDbSwDevCtrlUcastStormThresholdIntfSet(interface, threshold, rate_unit);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcastLvl_1);
              status = L7_FAILURE;
              continue;
            }
            rc = usmDbSwDevCtrlUcastStormModeIntfSet(interface, L7_ENABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcast_1);
              status = L7_FAILURE;
            }
          }
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlUcast);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }
            rc = usmDbSwDevCtrlUcastStormModeIntfSet(interface, L7_DISABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcast_1);
              status = L7_FAILURE;
            }
          }
        }
      }
    }
    else if (numArg == 1)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
        {
          if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
          {
            if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }
            rc = usmDbSwDevCtrlUcastStormThresholdIntfSet(interface, FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD,
                                                          FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD_UNIT);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcastLvl_1);
              status = L7_FAILURE;
              continue;
            }
            rc = usmDbSwDevCtrlUcastStormModeIntfSet(interface, L7_DISABLE);
            if (rc != L7_SUCCESS)
            {
              ewsTelnetWrite(ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlUcast_1);
              status = L7_FAILURE;
            }
          }
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlUcastNo);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_StormCntrlUcast);

  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable or disable flow control
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  storm-control flowcontrol
 *
 * @cmdhelp Enable/Disable 802.3x flow control.
 *
 * @cmddescript
 *   Indicates if 802.3x flow control is enabled for the switch. The default
 *   is Disable. This value applies to only full-duplex mode ports.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandStormControlFlowControl(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit;

  L7_uint32 numArg;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SetPortFlowCntrl);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSwDevCtrlFlowControlModeSet(unit, L7_ENABLE);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlFlow);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SetPortFlowCntrlNo);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSwDevCtrlFlowControlModeSet(unit, L7_DISABLE);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_base_StormCntrlFlow);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SetPortFlowCntrl);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Creates/configures a system portChannel
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax - creation: port-channel <name>
 * @cmdsyntax - deletion in stacking env: no port-channel <logical unit/slot/port | all>
 * @cmdsyntax - deletion in non-stacking env: no port-channel <logical slot/port | all>
 *
 * @cmdhelp
 *
 * @cmddescript  Note: the first 5 parameter (or a place holder for them '$'
 *               are required then the slot/port member are optional (range 8 or 0)
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandInterfacePortChannel(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argName = 1;
  L7_uint32 intIfNum;
  static L7_uint32 lastCreatedPortChannelIfNum=0;
  L7_RC_t rc;
  L7_RC_t lrc;
  L7_uint32 currentIntIfNum;
  L7_uint32 currentExtIfNum;
  L7_uchar8 bufIndex[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 argSlotPort = 1;
  L7_uint32 logicalSlot;
  L7_uint32 logicalPort;
  L7_uint32 adminMode;
  L7_uint32 numPortChannelIf = 0;
  L7_char8 portChannelName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 member[L7_MAX_MEMBERS_PER_LAG]; /* Internal interface numbers of portChannel members */
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit, u;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IntfLag_1);
    }
    if (ewsContext->commType == CLI_NO_CMD)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf),
          pStrErr_base_IntfLagNo, cliSyntaxLogInterfaceHelp());
      ewsTelnetWrite(ewsContext, buf);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (cnfgrIsFeaturePresent(L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_LAG_PRECREATE_FEATURE_ID) ) {
    if (lastCreatedPortChannelIfNum == 0) {
      if (usmDbDot3adIfIndexGetFirst(unit, &lastCreatedPortChannelIfNum) != L7_SUCCESS) {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LagNotCreated);
      }
    } 
    else {
      if (usmDbDot3adIfIndexGetNext(unit, lastCreatedPortChannelIfNum, &lastCreatedPortChannelIfNum) != L7_SUCCESS) {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LagNotCreated);
      }
    }

    if (strlen(argv[index+argName]) >= sizeof(portChannelName))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IntfLag_1);
    }

    /* getting the name param 1 */
    OSAPI_STRNCPY_SAFE(portChannelName, argv[index+argName]);
    if (strlen(portChannelName) >= L7_LAG_NAME_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LagName_1);
    }

    if (cliIsAlphaNum(portChannelName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_LagNameSyntax);
    }

    /* Set the port-channel's name */
    if (usmDbDot3adNameSet(unit, lastCreatedPortChannelIfNum, portChannelName) != L7_SUCCESS) {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LagNotCreated);
    }

    /* make it dynamic */
    if (usmDbDot3adLagStaticModeSet(unit, lastCreatedPortChannelIfNum, L7_FALSE) != L7_SUCCESS) {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LagNotCreated);
    }
  }
  else {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (strlen(argv[index+argName]) >= sizeof(portChannelName))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IntfLag_1);
      }
  
      /* getting the name param 1 */
      OSAPI_STRNCPY_SAFE(portChannelName, argv[index+argName]);
      if (strlen(portChannelName) >= L7_LAG_NAME_SIZE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LagName_1);
      }
  
      if (cliIsAlphaNum(portChannelName) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_LagNameSyntax);
      }
  
      /* Set all member values to zero, allowing us to create a portChannel without any members */
      /* Members will be created at a later stage using the add portChannel members command */
      memset ((L7_char8 *)member, 0, sizeof(L7_uint32)*L7_MAX_MEMBERS_PER_LAG);
      /* Verify that we haven't already created the maximum number of port-channels (lags). */
  
      rc=usmDbDot3adLagCountGet(&numPortChannelIf);
  
      if((rc==L7_SUCCESS)&&(numPortChannelIf >= L7_MAX_NUM_LAG_INTF))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_CouldNot,ewsContext, pStrInfo_base_MaxLagsReached, L7_MAX_NUM_LAG_INTF);
      }
      else
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          /* just call the function to create it w/ correct default parameters */
          if ( usmDbDot3adCreateSet(unit, portChannelName, FD_DOT3AD_ADMIN_MODE,FD_DOT3AD_LINK_TRAP_MODE,
                FD_DOT3AD_MEMBER_UNIT, FD_DOT3AD_HASH_MODE, member, &intIfNum) != L7_SUCCESS )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LagNotCreated);
          }
        }
      }
    }
  
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      if (strlen(argv[index+argSlotPort]) >= sizeof(bufIndex))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_IntfLagNo, cliSyntaxLogInterfaceHelp());
      }
  
      OSAPI_STRNCPY_SAFE(bufIndex, argv[index+argSlotPort]);
      cliConvertToLowerCase(bufIndex);
      if (strcmp(bufIndex,pStrInfo_common_All) == 0)
      {
        /* Get ifIndex for the first portChannel member */
        if ((lrc = usmDbDot3adIfIndexGetFirst(unit, &currentExtIfNum)) != L7_SUCCESS)
          /* if there is no ifIndex no portChannels exist */
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoLags);
        }
        while (lrc == L7_SUCCESS)                                             /* there is a portChannel. */
        {
          /* Check validity of portChannel and don't care if it is configured */
  
          usmDbIntIfNumFromExtIfNum(currentExtIfNum, &currentIntIfNum);
          if (usmDbDot3adIsConfigured(unit, currentIntIfNum) == L7_TRUE)
            /* checking to see if the ifIndex is valid and has been configured/created */
          {
            /*******Check if the Flag is Set for Execution*************/
            if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              rc = usmDbDot3adRemoveSet(unit, currentIntIfNum);
              if (rc != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LagNotValidToDel);
              }
            }
          }
          lrc = usmDbDot3adIfIndexGetNext(unit, currentExtIfNum, &currentExtIfNum);  /* get the next ifIndex */
        }
  
        /*************Set Flag for Script Successful******/
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
        return cliPrompt(ewsContext);
      }                               /* end of if the configuration set applies to all */
  
      if (cliIsStackingSupported() == L7_TRUE)
      {
        if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &u, &logicalSlot, &logicalPort)) != L7_SUCCESS)
        {
          if (rc == L7_ERROR)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            ewsTelnetPrintf (ewsContext, "%d/%d/%d", u, logicalSlot, logicalPort);
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
        rc = usmDbIntIfNumFromUSPGet(u, logicalSlot, logicalPort, &intIfNum);
      }
      else
      {
        u = unit;
        rc = cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort],
            &logicalSlot, &logicalPort, &intIfNum);
        if (rc != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
  
      /* Check if the portChannel is configured */
      if ((rc != L7_SUCCESS) || (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
      {
        osapiSnprintf(bufIndex, sizeof(bufIndex), "\r\n%s ", cliDisplayInterfaceHelp(u, logicalSlot, logicalPort));
        OSAPI_STRNCAT(bufIndex, pStrErr_base_IsNotACfguredLag);   /*  is not a configured portChannel.   */
        return cliSyntaxReturnPrompt (ewsContext, bufIndex);
      }
  
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adRemoveSet(unit, intIfNum) != L7_SUCCESS)
        {
          rc = usmDbDot3adAdminModeGet(unit, intIfNum, &adminMode);
          if (adminMode == L7_DISABLE)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DsbldLagsNotDeld);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_base_LagDel);
          }
  
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  make a port part of a portChannel
 *
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
 * @cmdsyntax  addport <logical slot/port>
 *
 * @cmdhelp Configure ports included in this portChannel.
 *
 * @cmddescript checks whether the port is included in any portChannel.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandInterfaceAddPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc, lrc;
  L7_uint32 s,p;

  L7_uint32 interface;
  L7_uint32 val;
  L7_uint32 itype;
  L7_uint32 lacpmode = L7_DISABLE;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 argSlotPort = 1;
  L7_uint32 logicalSlot;
  L7_uint32 logicalPort;
  L7_uint32 intIfNum, adminMode;
  L7_uint32 unit, unitNum;

  L7_uint32 numArg;
  L7_uchar8 paeCap;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* if numarg is less than 1, display help */
  if (numArg < 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrInfo_base_IncorrectInputAddPort, cliSyntaxLogInterfaceHelp());
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &logicalSlot, &logicalPort)) != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, logicalSlot, logicalPort);
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
      if (usmDbIntIfNumFromUSPGet(unit, logicalSlot, logicalPort, &intIfNum) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, logicalSlot, logicalPort);
      }
    }
    else
    {
      /* NOTE: No need to check the value of `unit` as
       *       ID of a standalone switch is always `U_IDX` (=> 1).
       */
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &logicalSlot, &logicalPort, &intIfNum) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }

    /* Get slot/port from the user and Map slot/port to an internal interface */
    /* Also, check if the portChannel is configured */
    if (usmDbDot3adIsConfigured(unit, intIfNum) == L7_FALSE)/* checking to see if the ifIndex is valid and has been configured/created */
    {
      osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
      OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);  /* is not a configured portChannel.   */
      return cliSyntaxReturnPrompt (ewsContext, buf);

    }
  }/* check if execution flag is set */

  for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
  {
    if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
    {
      if(usmDbUnitSlotPortGet(interface, &unitNum, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (usmDbDot3adIsValidIntf(unitNum, interface) != L7_TRUE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unitNum, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        status = L7_FAILURE;
        continue;
      }
     
       paeCap = 0x00;
       if (usmDbDot1xPortPaeCapabilitiesGet(unit,interface, &paeCap) == L7_SUCCESS)
       {
         if (paeCap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
         {
           ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unitNum, s, p));
           ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                    pStrErr_common_InvalidSlotPort_1);
           status = L7_FAILURE;
           continue;
         }
       }
    
      if ( usmDbIntfTypeGet(interface, &itype) != L7_SUCCESS )
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unitNum, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        status = L7_FAILURE;
        continue;
      }
    
      /* Don't allow PORTCHANNELS to be added to other PORTCHANNELs as member ports. */
      if (itype == L7_LAG_INTF)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unitNum, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_base_LagsCantBeAddedToOtherLagsAsMbrPorts);
        status = L7_FAILURE;
        continue;
      }
    
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if(usmDbDot3adIsStaticLag(unit, intIfNum, &val) == L7_SUCCESS)
        {
          /* if the port-channel is static, then no need to check LACP is enabled or not on inteface */
          if(val != L7_TRUE)
          {
            /* if the user attempts to add a port that doesn't have LACP enabled */
            rc = usmDbDot3adAggPortLacpModeGet(unitNum, interface, &lacpmode);
            if ((rc != L7_SUCCESS) || (lacpmode == L7_DISABLE))
            {
              osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unitNum, s, p));
              OSAPI_STRNCAT(buf, pStrInfo_base_LacpModeMustBeEnbld);    /*     LACP Mode must be enabled on member ports before they can be added to a PORTCHANNEL   */
              ewsTelnetWrite (ewsContext, buf);
              status = L7_FAILURE;
              continue;
            }
          }
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unitNum, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_LagStaticCheckingFailed);
          status = L7_FAILURE;
          continue;
        }
      }/* check if execution flag is set */
    
      /* if the user attempts to re-add a member port */
      if (usmDbDot3adIsMember(unitNum, interface) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unitNum, s, p));
        OSAPI_STRNCAT(buf, pStrInfo_base_SlotPortWithoutUnitIsPartOfALag);    /*     interface is part of a portChannel.   */
        ewsTelnetWrite( ewsContext, buf);
    
        /*it's a warning, don't set status to failure*/
        continue;
      }
    
      /* make sure the port is not a mirror or probe */
      if (usmDbSwPortMonitorIsDestConfigured(unitNum, interface,L7_NULLPTR) == L7_TRUE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unitNum, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_MirroredOrProbePortsCantBeLagMbrs);
        status = L7_FAILURE;
        continue;
      }
      if (usmDbSwPortMonitorIsSrcConfigured(unitNum, interface) == L7_TRUE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unitNum, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_MirroredOrProbePortsCantBeLagMbrs);
        status = L7_FAILURE;
        continue;
      }
    
    
      /* check to find the speed of the interface about to join a portChannel  */
      rc = usmDbIfSpeedGet(unitNum, interface, &val);
    
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* since everything seems fine, adding an interface should be possible */
        rc = usmDbDot3adMemberAddSet(unit, intIfNum, interface);
    
        /* if a logical port has eight member-ports already */
        if (rc != L7_SUCCESS)
        {
          status = L7_FAILURE;
          if (rc == L7_NOT_SUPPORTED) /* LAG on NNI port configuration */
          {
            osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unitNum, s, p));
            OSAPI_STRNCAT(buf, pStrInfo_base_LAG_ON_NNI);
            ewsTelnetWrite (ewsContext, buf);
            continue;
          }
    
          lrc = usmDbDot3adAdminModeGet(unit, intIfNum, &adminMode);
          if (adminMode == L7_DISABLE)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DsbldLags);
          }
          else
          {
            if (rc == L7_REQUEST_DENIED)
            {
              osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unitNum, s, p));
              OSAPI_STRNCAT(buf, pStrInfo_base_ErrorIntfMacFilterMember);  /* Interface could not be added. Interface is a member of a mac filter.*/
            }
            else
            {    
              osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unitNum, s, p));
              OSAPI_STRNCAT(buf, pStrInfo_base_ReachedMaxMbrPortLimit);    /* interface could not be added. Logical port reached max member port limit.*/
            } 
          }
          ewsTelnetWrite (ewsContext, buf);
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose  Remove a member port from the portChannel
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes this command needs work, it is very ineffecient (only 1 port at a time)
 *
 * @mode Interface Config
 *
 * @cmdsyntax in stacking env: deleteport <logical unit/slot/port>
 * @cmdsyntax in non-stacking env: deleteport <logical slot/port>
 *
 * @cmdhelp Delete a member port included in this portChannel.
 *
 * @cmddescript Removes member port(s) included in the portChannel.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandInterfaceDeletePort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t   rc;
  L7_uint32 argSlotPort = 1;
  L7_uint32 logicalSlot;
  L7_uint32 logicalPort;
  L7_uint32 intIfNum, lag_intf;
  L7_uint32 interface, adminMode;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit, unitNum, s, p;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_IntfDelPort_1, cliSyntaxLogInterfaceHelp());
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &logicalSlot, &logicalPort)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, logicalSlot, logicalPort);
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
    if (usmDbIntIfNumFromUSPGet(unit, logicalSlot, logicalPort, &intIfNum) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, logicalSlot, logicalPort);
    }
  }
  else
  {
    if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &logicalSlot, &logicalPort, &intIfNum) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  if (usmDbDot3adIsConfigured(unit, intIfNum) == L7_FALSE)      /* checking to see if the ifIndex is valid and has been configured/created */
  {
    osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
    OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);  /*     is not a configured portChannel.   */
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
  {
    if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
    {
      if (usmDbUnitSlotPortGet(interface, &unitNum, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (usmDbDot3adIsValidIntf(unitNum, interface) != L7_TRUE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unitNum, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        status = L7_FAILURE;
        continue;
      }
    
      /* check if `interface` is member of the logical port `intIfNum` */
      if ((usmDbDot3adIntfIsMemberGet(unit, interface, &lag_intf) != L7_SUCCESS) ||
          (lag_intf != intIfNum))
      {
        osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unitNum, s, p));
        osapiSnprintf(stat, sizeof(stat), pStrErr_base_DelMbrLag, cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
        OSAPI_STRNCAT(buf, stat);
        ewsTelnetWrite (ewsContext, buf);
        status = L7_FAILURE;
        continue;
      }
    
      if (usmDbDot3adMemberDeleteSet(unit, intIfNum, interface) != L7_SUCCESS)
      {
        rc = usmDbDot3adAdminModeGet(unit, intIfNum, &adminMode);
        if (adminMode == L7_DISABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unitNum, s, p));
          osapiSnprintfAddBlanks (1, 1, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_PortsCantBeRemovedFromDsbldLags);
        }
    
        ewsTelnetWrite (ewsContext, buf);
        status = L7_FAILURE;
        continue;
    
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose  Remove all member ports from a portChannel
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @mode Global Config
 *
 * @cmdsyntax in stacking env: deleteport <logical unit/slot/port> all
 * @cmdsyntax in non-stacking env: deleteport <logical slot/port> all
 *
 * @cmdhelp Delete all member ports from a port-channel.
 *
 * @cmddescript Removes all member ports from a configured portChannel.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandGlobalDeletePortAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 argSlotPort = 1;
  L7_uint32 logicalSlot;
  L7_uint32 logicalPort;
  L7_uint32 intIfNum;
  L7_uint32 argAll = 2;
  L7_uint32 numOfArg = 1;
  L7_uint32 counter;
  L7_uint32 listMember[L7_MAX_MEMBERS_PER_LAG];  /* internal interfaces of portChannel members */
  L7_uchar8 strAll[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32 unit, u;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArg = cliNumFunctionArgsGet();

  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_GlobalDelPortAll, cliSyntaxLogInterfaceHelp());
  }

  if (strlen(argv[index+argAll]) >= sizeof(strAll))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_GlobalDelPortAll, cliSyntaxLogInterfaceHelp());
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &u, &logicalSlot, &logicalPort)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%d/%d/%d", u, logicalSlot, logicalPort);
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
    rc = usmDbIntIfNumFromUSPGet(u, logicalSlot, logicalPort, &intIfNum);
  }

  else
  {
    u = unit;
    rc = cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort],
        &logicalSlot, &logicalPort, &intIfNum);
  }

  /* Check if the portChannel is configured */
  if ((rc != L7_SUCCESS) || (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_common_InvalidLag, cliDisplayInterfaceHelp(u, logicalSlot, logicalPort));
  }

  OSAPI_STRNCPY_SAFE(strAll,argv[index+ argAll]);
  cliConvertToLowerCase(strAll);
  if ( (numArg == 2)  && (strcmp(strAll, pStrInfo_common_All) == 0) )
  {                                                                       /* delete all the slot/ports from this portChannel, */
    /* get all ports into a list and go through the list and delete them all */
    numOfArg = L7_MAX_MEMBERS_PER_LAG;
    rc = usmDbDot3adMemberListGet(unit, intIfNum, &numOfArg,  listMember);
    for (counter=0; counter < numOfArg; counter++)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ( usmDbDot3adMemberDeleteSet(unit, intIfNum, listMember[counter]) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LagsDeld);
        }
      }
    }

  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Set all portChannel hashMode
 * @in non-stacking env: reset all portChannel hashMode to default value
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @mode Global Config
 *
  * @cmdsyntax in stacking env: port-channel load-balance <1-6> {<logical unit/slot/port> | all}
 * @cmdsyntax in non-stacking env: port-channel load-balance <1-6> {<logical slot/port> | all}* 
 *
 * @cmdhelp Set all portChannel hashMode.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortChannelLoadBalanceGlobalLAG(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 argSelIntf = 2;
  L7_uint32 argHashMode = 1;
  L7_uint32 hashMode;
  L7_uchar8 strHashMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strSelIntf[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 logicalSlot;
  L7_uint32 logicalPort;
  L7_uint32 lagIntIfNum;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    OSAPI_STRNCPY_SAFE(strHashMode, argv[index+argHashMode]);
    OSAPI_STRNCPY_SAFE(strSelIntf,argv[index+ argSelIntf]);
    cliConvertToLowerCase(strSelIntf);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    OSAPI_STRNCPY_SAFE(strSelIntf,argv[index+ numArg]);
    cliConvertToLowerCase(strSelIntf);
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_GlobalLoadBalanceAll, cliSyntaxLogInterfaceHelp());
  }

  if (strcmp(strSelIntf, pStrInfo_common_All) == 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        hashMode=atoi(strHashMode);
        rc = usmDbDot3adSystemHashModeSet(unit, hashMode);
      }
    }
    else
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        hashMode=FD_DOT3AD_HASH_MODE;
        rc = usmDbDot3adSystemHashModeSet(unit, hashMode);
      }
    }

    if ((ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) && (rc != L7_SUCCESS))
    {
      status = L7_FAILURE;
    }
  }
  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      rc = cliValidSpecificUSPCheck(argv[index+argSelIntf], &unit, &logicalSlot, &logicalPort);

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, logicalSlot, logicalPort, &lagIntIfNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      if (cliSlotPortToIntNum(ewsContext, argv[index+argSelIntf], &logicalSlot, &logicalPort, &lagIntIfNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }

    /* Check if the portChannel is configured */
    if (usmDbDot3adIsConfigured(unit, lagIntIfNum) != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_common_InvalidLag, cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
    }

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        hashMode=atoi(strHashMode);
        rc = usmDbDot3adLagHashModeSet(unit, lagIntIfNum, hashMode);
      }
    }
    else
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        hashMode=FD_DOT3AD_HASH_MODE;
        rc = usmDbDot3adLagHashModeSet(unit, lagIntIfNum, hashMode);
      }
    }

    if ((ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) && (rc != L7_SUCCESS))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
      if (rc == L7_DEPENDENCY_NOT_MET)
      {
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_LagHashModeAdminDown);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_LagReturn);
      }
      status = L7_FAILURE;
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  set the admin mode of portChannels
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  adminmode
 *
 * @cmdhelp Enable/Disable the interface related port channel's administrative mode.
 *
 * @cmddescript
 *   Indicates whether the portChannel is Enabled or Disabled.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandInterfaceAdminMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 adminMode=0;

  L7_uint32 unit;
  L7_uint32 interface; /*nextInterface;*/
  L7_uint32 s, p;

  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IntfAdminMode);
    }
    adminMode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IntfAdminModeNo);
    }
    adminMode = L7_DISABLE;
  }

  for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
  {
    if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
    {
      if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      /* Check validity of portChannel meaning is it configured */
    
      if (usmDbDot3adIsConfigured(unit, interface) == L7_FALSE)      /* checking to see if the ifIndex is valid and has been configured/created */
      {
        osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unit, s, p));
        OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);  /*     is not a configured portChannel.   */
        ewsTelnetWrite (ewsContext, buf);
        status = L7_FAILURE;
        continue;
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbDot3adAdminModeSet(unit, interface, adminMode);
        if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LagAdminModeNotSet);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Set the administrative mode of all configured portChannels
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  adminmode all
 *
 * @cmdhelp Enable/Disable admin mode of all configured portChannels.
 *
 * @cmddescript Indicates whether portChannels are Enabled or Disabled.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortChannelAdminMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_RC_t lrc;
  L7_uchar8 bufIndex[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 adminMode=0;

  L7_uint32 argSlotPort = 1;
  L7_uint32 intIfNum;
  L7_uint32 extIfNum;
  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_LagAdminMode);
    }
    if (strlen(argv[index+argSlotPort]) >= sizeof(bufIndex))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_LagAdminMode);
    }

    adminMode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_LagAdminModeNo);
    }
    if (strlen(argv[index+argSlotPort]) >= sizeof(bufIndex))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_LagAdminMode);
    }

    adminMode = L7_DISABLE;
  }

  OSAPI_STRNCPY_SAFE(bufIndex, argv[index+argSlotPort]);
  cliConvertToLowerCase(bufIndex);

  if (strcmp(bufIndex, pStrInfo_common_All) == 0)
  {
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    /* Get ifIndex for the first portChannel member */
    lrc = usmDbDot3adIfIndexGetFirst(unit, &extIfNum);
    if (lrc != L7_SUCCESS)                                                /* if there is no ifIndex no portChannels exist */
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoLags );
    }

    while (lrc == L7_SUCCESS)                                             /* there is a portChannel. */
    {
      usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum);

      if (usmDbDot3adIsConfigured(unit, intIfNum) == L7_TRUE)          /* checking to see if the ifIndex is valid and has been configured/created */
      {
        rc = usmDbDot3adAdminModeSet(unit, intIfNum, adminMode);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LagAdminModeNotSet);
        }
      }
      lrc = usmDbDot3adIfIndexGetNext(unit, extIfNum, &extIfNum);  /* get the next ifIndex */
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  port-channel system priority <priority>
 *
 * @cmdhelp .
 *
 * @cmddescript .
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortChannelSystemPriority(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 priorityValue = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 2 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_LagsSysPri_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[index+2], &priorityValue) == L7_SUCCESS)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adPortChannelSystemPrioritySet(priorityValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_LagsSysPri);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_LagsSysPri_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_LagsSysPriNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set the default value */
      if (usmDbDot3adPortChannelSystemPrioritySet( FD_DOT3AD_ACTOR_DEFAULT_SYS_PRIORITY) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_LagsSysPri);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_LagsSysPri_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable or disable link traps for a specific portChannel
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  linktrap {<logical slotport> | all}
 *
 * @cmdhelp Enable/Disable Link Up/Down traps for this port.
 *
 * @cmddescript Indicates whether link traps are Enabled or Disabled; Enable is the default.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandInterfaceLinktrap(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_RC_t lrc;
  L7_uchar8 bufIndex[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 linkTrapMode=0;

  L7_uint32 argSlotPort = 1;
  L7_uint32 logicalSlot;
  L7_uint32 logicalPort;
  L7_uint32 intIfNum;
  L7_uint32 extIfNum;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_IntfLinkTrapNo, cliSyntaxLogInterfaceHelp());
    }
    if (strlen(argv[index+argSlotPort]) >= sizeof(bufIndex))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_IntfLinkTrapNo, cliSyntaxLogInterfaceHelp());
    }
    linkTrapMode = L7_DISABLE;
  }
  else if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_IntfLinkTrap, cliSyntaxLogInterfaceHelp());
    }
    if (strlen(argv[index+argSlotPort]) >= sizeof(bufIndex))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_IntfLinkTrap, cliSyntaxLogInterfaceHelp());
    }
    linkTrapMode = L7_ENABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_IntfLinkTrap, cliSyntaxLogInterfaceHelp());
  }

  /* if is not for all portChannels then it has to be a valid ifIndex */
  OSAPI_STRNCPY_SAFE(bufIndex, argv[index+argSlotPort]);
  cliConvertToLowerCase(bufIndex);
  if (strcmp(bufIndex,pStrInfo_common_All) == 0)
  {
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    /* Get ifIndex for the first portChannel member */
    if ((lrc = usmDbDot3adIfIndexGetFirst(unit, &extIfNum)) != L7_SUCCESS)  /* if there is no ifIndex no portChannels exist */
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoLags);
    }
    while (lrc == L7_SUCCESS)                                             /* there is a portChannel. */
    {

      usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum);
      if (usmDbDot3adIsConfigured(unit, intIfNum) == L7_TRUE)          /* checking to see if the ifIndex is valid and has been configured/created */
      {

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbDot3adLinkTrapSet(unit, intIfNum, linkTrapMode);
          if (rc != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LagLinkTrap);
          }
        }

      }
      lrc = usmDbDot3adIfIndexGetNext(unit, extIfNum, &extIfNum);  /* get the next ifIndex */
    }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }                                                                       /* end of if the configuration set applies to all */

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &logicalSlot, &logicalPort)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, logicalSlot, logicalPort);
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
    if (usmDbIntIfNumFromUSPGet(unit, logicalSlot, logicalPort, &intIfNum) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, logicalSlot, logicalPort);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &logicalSlot, &logicalPort, &intIfNum) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  /* Check validity of portChannel meaning is it configured */
  if (usmDbDot3adIsConfigured(unit, intIfNum) == L7_FALSE)      /* checking to see if the ifIndex is valid and has been configured/created */
  {
    osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
    OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);  /*     is not a configured portChannel.   */
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbDot3adLinkTrapSet(unit, intIfNum, linkTrapMode);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LagLinkTrap);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  set the name of portChannels
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   name {<logical slot/port | all} <new portChannel name>
 *
 * @cmdhelp Enter the Name for this portChannel.
 *
 * @cmddescript
 *   Defines the unique name for the portChannel. You can use up to 15
 *   alphanumeric characters. There is no default portChannel name.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortChannelName(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_RC_t lrc;
  L7_uint32 intIfNum, adminMode;
  L7_uint32 extIfNum;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 bufIndex[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 argSlotPort = 1;
  L7_uint32 logicalSlot;
  L7_uint32 logicalPort;
  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_IntfName_1, cliSyntaxLogInterfaceHelp());
  }

  if ((strlen(argv[index+argSlotPort]) >= sizeof(bufIndex)) ||
      (strlen(argv[index+2]) >= sizeof(buf)))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_IntfName_1, cliSyntaxLogInterfaceHelp());
  }

  OSAPI_STRNCPY_SAFE(buf, argv[index+2]);
  if (strlen(buf) >= L7_LAG_NAME_SIZE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LagNameTooLong);
  }

  /* if is not for all portChannels then it has to be a valid ifIndex */
  OSAPI_STRNCPY_SAFE(bufIndex, argv[index+argSlotPort]);
  cliConvertToLowerCase(bufIndex);

  if (strcmp(bufIndex,pStrInfo_common_All) == 0)
  {
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    /* Get ifIndex for the first portChannel member */
    if ((lrc = usmDbDot3adIfIndexGetFirst(unit, &extIfNum)) != L7_SUCCESS)  /* if there is no ifIndex no portChannels exist */
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoLags);
    }

    while (lrc == L7_SUCCESS)                                             /* there is a portChannel. */
    {

      usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum);
      if (usmDbDot3adIsConfigured(unit, intIfNum) == L7_TRUE)          /* checking to see if the ifIndex is valid and has been configured/created */
      {

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbDot3adNameSet(unit, intIfNum, buf);
          if (rc != L7_SUCCESS)
          {
            rc = usmDbDot3adAdminModeGet(unit, intIfNum, &adminMode);
            if (adminMode == L7_DISABLE)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LagsNotRenamed);
            }
            else
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LagNameNotSet);
            }

            return cliSyntaxReturnPrompt (ewsContext, "");
          }
        }
      }
      lrc = usmDbDot3adIfIndexGetNext(unit, extIfNum, &extIfNum);  /* get the next ifIndex */
    }                                                                     /* end of if the configuration set applies to all */
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }                                                                       /* end of if the configuration set applies to all */

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &logicalSlot, &logicalPort)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, logicalSlot, logicalPort);
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
    if (usmDbIntIfNumFromUSPGet(unit, logicalSlot, logicalPort, &intIfNum) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, logicalSlot, logicalPort);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &logicalSlot, &logicalPort, &intIfNum) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  /* Check validity of portChannel meaning is it configured */
  if (usmDbDot3adIsConfigured(unit, intIfNum) == L7_FALSE)      /* checking to see if the ifIndex is valid and has been configured/created */
  {
    osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
    OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);  /*     is not a configured portChannel.   */
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbDot3adNameSet(unit, intIfNum, buf);
    if (rc != L7_SUCCESS)
    {
      rc = usmDbDot3adAdminModeGet(unit, intIfNum, &adminMode);
      if (adminMode == L7_DISABLE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LagsNotRenamed);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LagNameNotSet);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Sets GARP GMRP Admin mode to Enable or Disable for the switch
 *
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
 * @cmdsyntax  set gmrp adminmode
 *
 * @cmdhelp
 *
 * @cmddescript  where mode is enable or disable.
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetGMRPAdminMode(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_SetGmrpAdminMode);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbGarpGmrpEnableSet(unit, L7_ENABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotEnblGmrp);
      }
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_SetGmrpAdminModeNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbGarpGmrpEnableSet(unit, L7_DISABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotDsblGmrp);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Sets GVRP Admin mode to Enable or Disable for the switch
 *
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
 * @cmdsyntax  set gvrp adminmode
 *
 * @cmdhelp
 *
 * @cmddescript  where mode is enable or disable.
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetGVRPAdminMode(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_SetGvrpAdminMode);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbGarpGvrpEnableSet(unit, L7_ENABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotEnblGvrp);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_SetGvrpAdminModeNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      cliWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, pStrInfo_base_DisablingGvrp);          /*UnBuffered Write*/
      if ( usmDbGarpGvrpEnableSet(unit, L7_DISABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotDsblGvrp);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Configure GMRP on a per port basis
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index*
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes
 *
 * @cmdsyntax  set gmrp interfacemode
 *
 * @cmdhelp     Enable or Disable port GVRP. Valid when switch GVRP is enabled
 *
 * @cmddescript    Configure GVRP on a per port basis
 *    none
 * @end
 *
 *********************************************************************/
const L7_char8  *commandSetGMRPInterfaceMode(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;

  L7_uint32 interface = 0;
  L7_uint32 s, p;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SetGmrpIntfMode_1);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
      {
        if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
        {
          if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
          {
            continue;
          }

          if (usmDbGarpIsValidIntf(unit, interface) != L7_TRUE)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
            status = L7_FAILURE;
            continue;
          }

          rc = usmDbGarpGmrpPortEnableSet(unit, interface, L7_ENABLE);
          if (rc == L7_NOT_SUPPORTED)
          {
            /* port selected cannot perform this function */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
            status = L7_FAILURE;
          }
          else if (rc != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotEnblGmrp);
            status = L7_FAILURE;
          }
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SetGmrpIntfModeNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
      {
        if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
        {
          if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
          {
            continue;
          }

          if (usmDbGarpIsValidIntf(unit, interface) != L7_TRUE)
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            status = L7_FAILURE;
            continue;
          }

          rc = usmDbGarpGmrpPortEnableSet(unit, interface, L7_DISABLE);
          if (rc == L7_NOT_SUPPORTED)
          {
            /* port selected cannot perform this function */
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
            status = L7_FAILURE;
          }
          else if (rc != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotDsblGmrp);
            status = L7_FAILURE;
          }
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Configure GVRP on a per port basis
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index*
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes
 *
 * @cmdsyntax  set gmrp interfacemode all
 *
 * @cmdhelp     Enable or Disable port GVRP. Valid when switch GVRP is enabled
 *
 * @cmddescript    Configure GVRP on a per port basis
 *    none
 * @end
 *
 *********************************************************************/
const L7_char8  *commandSetGMRPInterfaceModeAll(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;

  L7_uint32 interface = 0;
  /* L7_uint32 itype;*/

  L7_uint32 unit;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* For all the ports */
  /* apply to all interfaces */

  interface = L7_ALL_INTERFACES;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SetGmrpIntfMode_1);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbGarpGmrpPortEnableSet(unit, interface, L7_ENABLE);
      if (rc == L7_NOT_SUPPORTED)
      {
        /* port selected cannot perform this function */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotEnblGmrp);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SetGmrpIntfModeNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbGarpGmrpPortEnableSet(unit, interface, L7_DISABLE);
      if (rc == L7_NOT_SUPPORTED)
      {
        /* port selected cannot perform this function */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotDsblGmrp);
      }

    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Configures port mirroring sessions
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8
 *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  monitor session <session-id> { {mode} | ( { source | destination }
 *             interface <slot/port>) }
 *
 * @cmdhelp   Add/Remove interfaces to mirror sessions. Also controls enable/disable
 *            status of the session.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortMirroring(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  static L7_uint32 listSrcPorts[L7_FILTER_MAX_INTF];
  L7_uint32      numPorts;
  L7_uint32 argSessionNum = 2;
  L7_uint32 sessionNum;
  L7_uint32 argThirdParam = 3;
  L7_uint32 argTypeParam = 6;
  L7_uchar8 strTypeParam[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strThirdParam[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 5, unit, slot, port;
  L7_uint32 numArg;
  L7_uint32  intIfNum;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 bufTemp[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t   rc = L7_SUCCESS;
  L7_BOOL   failStatus = L7_TRUE;
  L7_uint32      destIntfNum;
  L7_INTF_MASK_t srcIntfMask;
  L7_MIRROR_DIRECTION_t type;
  L7_uchar8 paeCap;


  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  memset(strTypeParam,0,sizeof(strTypeParam));
  cliSyntaxTop(ewsContext);

  type = L7_MIRROR_BIDIRECTIONAL;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (cliIsStackingSupported() == L7_TRUE)
  {
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, bufTemp, sizeof(bufTemp), pStrErr_common_AclIntfsStacking);
  }
  else
  {
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, bufTemp, sizeof(bufTemp), pStrErr_common_AclIntfs);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 3 && numArg != 5 && numArg !=6) /* change is occured  add  6 case */
    {
      if((cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID,
              L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID)) == L7_TRUE)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_base_CfgMirrorCreateDirection, bufTemp,bufTemp);
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_base_CfgMirroringCreate,bufTemp,bufTemp);
      }

      return cliSyntaxReturnPrompt (ewsContext,buf);
    }
  }
  else   /* "no" form of the command */
  {
    if (numArg != 0 && numArg != 2 && numArg != 3 && numArg != 4 && numArg != 5)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgMirroringDel,bufTemp);
    }
  }

  if(numArg != 0)  /* numArg is 2, 3, 4 or 5 */
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index + argSessionNum],&sessionNum) != L7_SUCCESS)
    {
      /* Error is displayed by parser */
      return cliPrompt(ewsContext);
    }
  }
  else /* This is "no" form of command and have no argument (no monitor)*/
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSwPortMonitorConfigRemove(unit);
      if(rc == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_base_CfgMirroringDataRemoveFailure);
      }
    }
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  if(numArg >= 3)
  {
    OSAPI_STRNCPY_SAFE(strThirdParam, argv[index + argThirdParam]);
    cliConvertToLowerCase(strThirdParam); /* To avoid duplicate calculations store type of interface here */
    if(numArg == 3 && strcmp(strThirdParam,pStrInfo_common_ApShowRunningMode) != 0)
    {
      if(ewsContext->commType == CLI_NORMAL_CMD)
      {
        if((cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID,
                L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID)) == L7_TRUE)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_base_CfgMirrorCreateDirection, bufTemp,bufTemp);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_base_CfgMirroringCreate,bufTemp,bufTemp);
        }
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_base_CfgMirroringDel,bufTemp);
      }
      return cliSyntaxReturnPrompt (ewsContext,buf);
    }
  }
  if(numArg == 5 || numArg == 6)  /* source/destination interface add/remove command but not 'remove destination' */ /*change is occured */
  {

    if ((cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port) != L7_SUCCESS) || (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS))
    {
      return cliPrompt(ewsContext);
    }

    /* check for portChannel members */
    if (usmDbDot3adIsMember(unit, intIfNum) == L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_PortMirroringIsNotAllowedOnLagMbrs);
    }
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg == 3)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwPortMonitorModeSet(unit, sessionNum, L7_ENABLE);
        if(rc == L7_NOT_EXIST)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgMirroringInvalidSessionId, L7_MIRRORING_MAX_SESSIONS,
              L7_MIRRORING_MAX_SESSIONS);
        }
      }
      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      /* if above "if" condition fails then also we need to return cli prompt */
      return cliPrompt(ewsContext);
    }
    else if(numArg == 5 || numArg ==6)
    {
      if(numArg == 6)
      {
        OSAPI_STRNCPY_SAFE(strTypeParam, argv[index + argTypeParam]);
        cliConvertToLowerCase(strTypeParam);
        if( strcmp(strTypeParam,pStrInfo_base_Egress) == 0)
        {
          type =  L7_MIRROR_EGRESS;
        }
        else if( strcmp(strTypeParam,pStrInfo_base_Ingress) == 0)
        {
          type =  L7_MIRROR_INGRESS;
        }
      }

      if (strcmp(strThirdParam,pStrInfo_common_SrcPortMirror) == 0)
      {
        rc = usmDbSwPortMonitorDestPortGet(unit, sessionNum, &destIntfNum);
        usmDbSwPortMonitorSourcePortsGet(unit, sessionNum, &srcIntfMask);

        /* listSrcPorts' list is not needed to be created for the CLI,
         * but is being used in order to use the same routine for the CLI & Web validations.
         */
        rc = usmDbConvertMaskToList(&srcIntfMask, listSrcPorts, &numPorts);
        listSrcPorts[numPorts++] = intIfNum;

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbSwPortMonitorSourcePortAdd(unit, sessionNum, intIfNum, type);
        }
      }
      else
      {
        usmDbSwPortMonitorSourcePortsGet(unit, sessionNum, &srcIntfMask);

        /* 'listSrcPorts' list is not needed to be created for the CLI,
         * but is being used in order to use the same routine for the CLI & Web validations.
         */
        rc = usmDbConvertMaskToList(&srcIntfMask, listSrcPorts, &numPorts);
        paeCap = 0x00;
        if (usmDbDot1xPortPaeCapabilitiesGet(unit,intIfNum, &paeCap) == L7_SUCCESS)
        {
          if (paeCap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
          {
             return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                pStrErr_base_PortMirroringIsNotAllowedOnSuppMbrs);
          }
        }
        
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbSwPortMonitorDestPortSet(unit, sessionNum, intIfNum);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        switch(rc)
        {
          case L7_NOT_EXIST:
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_CfgMirroringInvalidSessionId, L7_MIRRORING_MAX_SESSIONS,
                L7_MIRRORING_MAX_SESSIONS);
            break;
          case L7_FAILURE:
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_CfgMirroringInvalidIntf,strThirdParam);
            break;
          case L7_ALREADY_CONFIGURED:  /* this case is valid only for destination interface */
            OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, buf, pStrErr_base_CfgMirroringProbeIsAlreadyCfgured);
            break;
          case L7_REQUEST_DENIED:     /* valid only for source interface */
            OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, buf, pStrErr_base_CfgMirroringMaxSrcPortLimitReached);
            break;
          default:
            failStatus = L7_FALSE;
            break;
        } /* switch(rc) */
        if(failStatus == L7_TRUE)
        {
          return cliSyntaxReturnPrompt (ewsContext,buf);
        }
      }
    } /*else if(numArg == 5)*/
  }      /* if (ewsContext->commType == CLI_NORMAL_CMD) */
  else   /* ewsContext->commType == CLI_NO_CMD */
  {
    if(numArg == 2)  /* no monitor session {session-id} */
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwPortMonitorSessionRemove(unit, sessionNum);
        if(rc == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_base_CfgMirroringDataRemoveFailure);
        }
        else if (rc == L7_NOT_EXIST)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgMirroringInvalidSessionId, L7_MIRRORING_MAX_SESSIONS,
              L7_MIRRORING_MAX_SESSIONS);
        }
      }
      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliPrompt(ewsContext);
    }
    else if(numArg == 3) /* no monitor session  {session-id} mode */
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSwPortMonitorModeSet(unit, sessionNum, L7_DISABLE);

        if(rc == L7_NOT_EXIST)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgMirroringInvalidSessionId, L7_MIRRORING_MAX_SESSIONS,
              L7_MIRRORING_MAX_SESSIONS);
        }
      }
      /*************Set Flag for Script Successful******/
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliPrompt(ewsContext);
    }
    else     /* numArg == 4  or numArg == 5*/
    {
      if (strcmp(strThirdParam, pStrInfo_common_SrcPortMirror) == 0 && numArg == 5)
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbSwPortMonitorSourcePortRemove(unit, sessionNum, intIfNum);
        }
      }
      else if(strcmp(strThirdParam, pStrInfo_common_Ipv6DhcpRelayDest) == 0 && numArg == 4)  /* for destination only 4 arguments are valid */
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbSwPortMonitorDestPortRemove(unit, sessionNum);
        }
      }
      else     /* This is error */
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgMirroringDel,bufTemp);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        switch(rc)
        {
          case L7_NOT_EXIST:
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_CfgMirroringInvalidSessionId, L7_MIRRORING_MAX_SESSIONS,
                L7_MIRRORING_MAX_SESSIONS);
            break;
          case L7_FAILURE:
            OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, buf, pStrErr_base_CfgMirroringPortIsNotSet);
            break;
          default:
            failStatus = L7_FALSE;
            break;
        } /* switch(rc) */

        if(failStatus == L7_TRUE)
        {
          return cliSyntaxReturnPrompt (ewsContext,buf);
        }
      }
    }   /* numArg == 4 || numArg == 5*/
  }    /* ewsContext->commType == CLI_NO_CMD */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Group Membership Interval for
 *           a specified interface.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  set igmp groupmembership-interval <1-3600>
 *
 * @cmdhelp
 *
 * @cmddescript  This command sets the Group Membership Interval value for
 *               IGMP Snooping.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPInterfaceGroupMembershipInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argVal = 1;
  L7_uint32 value = 0;
  L7_uint32 responseTime = 0;
  L7_uint32 s, p, intIfNum=0;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uchar8 family = L7_AF_INET;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGGROUPMEMBERSHIPINTERVAL(family));
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+ argVal],&value) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    else if (usmDbSnoopIntfResponseTimeGet(unit, intIfNum, &responseTime, family) == L7_SUCCESS)
    {
      if (value <= responseTime)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_IgmpGrpMbrsHopIncorrect);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGGROUPMEMBERSHIPINTERVAL_NO(family));
    }

    value = FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;
    if (usmDbSnoopIntfResponseTimeGet(unit, intIfNum, &responseTime, family) == L7_SUCCESS)
    {
      if (value <= responseTime)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_IgmpGrpMbrsHopIncorrect);
      }
    }
  }

  for (intIfNum=1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
  {
    if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIfNum))
    {
      if(usmDbUnitSlotPortGet(intIfNum, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }
      /* don't allow if the ifIndex is a PORTCHANNEL member */
      if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
          && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        osapiSnprintf(buf, sizeof(buf), "\r\n%d/%d ", s, p);
        OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);
        ewsTelnetWrite (ewsContext, buf);
        status = L7_FAILURE;
        continue;
      }
    
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbSnoopIntfGroupMembershipIntervalSet(unit, intIfNum, value, family) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetGrpMbrshipIntvl);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Group Membership Interval
 *           for all interfaces.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  set igmp groupmembership-interval all <1-3600>
 *
 * @cmdhelp
 *
 * @cmddescript  This command sets the Group Membership Interval value
 *               for IGMP Snooping.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPInterfaceGroupMembershipIntervalAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argVal = 1;
  L7_uint32 value = 0;
  L7_uint32 responseTime = 0;
  L7_uint32 intIfNum, next;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_RC_t rc  = L7_SUCCESS;
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGGROUPMEMBERSHIPINTERVAL_ALL(family));
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+ argVal],&value) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    /* For all the ports */
    intIfNum=0;

    while (usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &next) == L7_SUCCESS)
    {
      intIfNum = next;
      /* don't allow if the ifIndex is a PORTCHANNEL member */
      if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
          && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
      {
        continue;
      }

      if (usmDbSnoopIntfResponseTimeGet(unit, intIfNum, &responseTime, family) == L7_SUCCESS)
      {
        if (value <= responseTime)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_IgmpGrpMbrsHopIncorrect);
        }
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSnoopIntfGroupMembershipIntervalSet(unit, intIfNum, value, family);
      }
    } /* end of while */
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGGROUPMEMBERSHIPINTERVAL_ALL_NO(family));
    }

    value = FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;

    /* For all the ports */
    intIfNum=0;

    while (usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &next) == L7_SUCCESS)
    {
      intIfNum = next;
      if (usmDbSnoopIntfResponseTimeGet(unit, intIfNum, &responseTime, family) == L7_SUCCESS)
      {
        if (value <= responseTime)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_IgmpGrpMbrsHopIncorrect);
        }
      }

      /* don't allow if the ifIndex is a PORTCHANNEL member */
      if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
          && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
      {
        continue;
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSnoopIntfGroupMembershipIntervalSet(unit, intIfNum, value, family);
      }
    }
  }

  if (rc != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetGrpMbrshipIntvl);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Maximum Response
 *           for a specified interface.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none   config igmpsnooping maxresponse
 *
 * @cmdsyntax  set igmp maxresponse <1-3600>
 *
 * @cmdhelp
 *
 * @cmddescript  This command sets the Maximum Response value for IGMP Snooping.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPInterfaceMaxResponse(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argVal = 1;
  L7_uint32 value = 0;
  L7_uint32 groupMembershipInterval = 0;
  L7_uint32 s, p, intIfNum=0;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uchar8 family = L7_AF_INET;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGMAXRESPONSE(family));
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+ argVal],&value) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    if (usmDbSnoopIntfGroupMembershipIntervalGet(unit, intIfNum, &groupMembershipInterval, family) == L7_SUCCESS)
    {
      if (value >= groupMembershipInterval)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_MaxRespTimeTooLarge);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGMAXRESPONSE_NO(family));
    }

    value = FD_IGMP_SNOOPING_MAX_RESPONSE_TIME;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (intIfNum=1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIfNum))
      {
        if(usmDbUnitSlotPortGet(intIfNum, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        /* don't allow if the ifIndex is a PORTCHANNEL member */
        if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
            && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          osapiSnprintf(buf, sizeof(buf), "\r\n%d/%d ", s, p);
          OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);
          ewsTelnetWrite(ewsContext, buf);
          status = L7_FAILURE;
          continue;
        }
    
        if (usmDbSnoopIntfResponseTimeSet(unit, intIfNum, value, family) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetMaxRespTime);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Maximum Response for all interfaces.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none   config igmpsnooping maxresponse
 *
 * @cmdsyntax  set igmp maxresponse all <1-3600>
 *
 * @cmdhelp
 *
 * @cmddescript  This command sets the Maximum Response value for IGMP Snooping.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPInterfaceMaxResponseAll(EwsContext ewsContext, L7_uint32 argc,  const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argVal = 1;
  L7_uint32 value = 0;
  L7_uint32 groupMembershipInterval = 0;
  L7_uint32 intIfNum, next;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_RC_t rc  = L7_SUCCESS;
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGMAXRESPONSE_ALL(family));
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+ argVal],&value) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    /* For all the ports */
    intIfNum=0;

    while (usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &next) == L7_SUCCESS)
    {
      intIfNum = next;
      /* don't allow if the ifIndex is a PORTCHANNEL member */
      if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
          && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
      {
        continue;
      }

      if (usmDbSnoopIntfGroupMembershipIntervalGet(unit, intIfNum, &groupMembershipInterval, family) ==  L7_SUCCESS)
      {
        if (value >= groupMembershipInterval)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_MaxRespTimeTooLarge);
        }
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSnoopIntfResponseTimeSet(unit, intIfNum, value, family);
      }
    }/* end of while */
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGMAXRESPONSE_ALL_NO(family));
    }

    value = FD_IGMP_SNOOPING_MAX_RESPONSE_TIME;

    /* For all the ports */
    intIfNum=0;

    while (usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &next) == L7_SUCCESS)
    {
      intIfNum = next;
      /* don't allow if the ifIndex is a PORTCHANNEL member */
      if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
          && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
      {
        continue;
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSnoopIntfResponseTimeSet(unit, intIfNum, value, family);
      }
    }
  }

  if (rc != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetMaxRespTime);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Multicast Router Present
 *           Expiration time for a specified interface.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none   config igmpsnooping mcrtrpresent
 *
 * @cmdsyntax  set igmp mcrtrexpirytime <0-3600>
 *
 * @cmdhelp
 *
 * @cmddescript  This command sets the Mulitcast Router Present Expiration
 *               value for IGMP Snooping.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPInterfaceMcRtrExpireTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argVal = 1;
  L7_uint32 value = 0;
  L7_uint32 s, p, intIfNum=0;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uchar8 family = L7_AF_INET;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is executed */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGMCRTREXPIRETIME(family));
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+ argVal],&value) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGMCRTREXPIRETIME_NO(family));
    }

    value = FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (intIfNum=1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIfNum))
      {
        if(usmDbUnitSlotPortGet(intIfNum, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        /* don't allow if the ifIndex is a PORTCHANNEL member */
        if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
            && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          osapiSnprintf(buf, sizeof(buf), "\r\n%d/%d ", s, p);
          OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);
          ewsTelnetWrite (ewsContext, buf);
          status = L7_FAILURE;
          continue;
        }
    
        if (usmDbSnoopIntfMcastRtrExpiryTimeSet(unit, intIfNum,value, family) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetMcastRtrPresentExpirationTime);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Multicast Router Present
 *           Expiration time for all interfaces.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none   config igmpsnooping mcrtrpresent
 *
 * @cmdsyntax  set igmp mcrtrexpirytime all <0-3600>
 *
 * @cmdhelp
 *
 * @cmddescript  This command sets the Mulitcast Router Present Expiration
 *               value for IGMP Snooping.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPInterfaceMcRtrExpireTimeAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv,  L7_uint32 index)
{
  L7_uint32 argVal = 1;
  L7_uint32 value = 0;
  L7_uint32 intIfNum, next;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGMCRTREXPIRETIME_ALL(family));
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+ argVal],&value) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    /* For all the ports */
    intIfNum=0;

    while (usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &next) == L7_SUCCESS)
    {
      intIfNum = next;
      /* don't allow if the ifIndex is a PORTCHANNEL member */
      if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
          && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
      {
        continue;
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSnoopIntfMcastRtrExpiryTimeSet(unit, intIfNum,value, family);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGMCRTREXPIRETIME_ALL_NO(family));
    }

    value = FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;

    /* For all the ports */
    intIfNum=0;

    while (usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &next) == L7_SUCCESS)
    {
      intIfNum = next;
      /* don't allow if the ifIndex is a PORTCHANNEL member */
      if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
          && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
      {
        continue;
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSnoopIntfMcastRtrExpiryTimeSet(unit, intIfNum,value, family);
      }
    }
  }

  if (rc != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetMcastRtrPresentExpirationTime);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Set GARP Join Timer
 *
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
 * @cmdsyntax set garp timer join <10-100>
 *
 * @cmdhelp
 *
 * @cmddescript  <time> is a number in centiseconds
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetGarpTimerJoin(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argTime = 1;
  L7_int32 interface = 0;
  L7_int32 intSlot;
  L7_int32 intPort;
  L7_int32 intTime = L7_GARP_DEF_JOIN_TIME;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_RC_t rc, status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpJoinTime_1);
    }

    (void) cliConvertTo32BitUnsignedInteger(argv[index+ argTime],&intTime);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpJoinTimeAllNo);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &intSlot, &intPort) != L7_SUCCESS)
        {
          continue;
        }
        if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) == L7_SUCCESS)
        {
          if (usmDbGarpIsValidIntf(unit, interface) != L7_TRUE)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, intSlot, intPort));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
            status = L7_FAILURE;
            continue;
          }
        }
    
        if ((rc=usmDbGarpJoinTimeSet(unit, interface, intTime)) != L7_SUCCESS)
        {
          if (rc == L7_ERROR)
          {
            return cliSyntaxReturnPromptAddBlanks(1,1,0,0, pStrErr_common_Error, ewsContext, pStrErr_base_GarpJoinTimerValueInvalid);
          }

          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, intSlot, intPort));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_JoinTimerBeSet);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Set GARP Join Timer
 *
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
 * @cmdsyntax   set garp timer join all <10-100>
 *
 * @cmdhelp
 *
 * @cmddescript  <time> is a number in centiseconds
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetGarpTimerJoinAll(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argTime = 1;
  L7_int32 intTime = L7_GARP_DEF_JOIN_TIME;
  L7_uint32 next, current,rc;
  L7_uint32 unit, s, p;
  L7_uint32 numArg;        /* New variable Added */
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpJoinTime_1);
    }

    (void) cliConvertTo32BitUnsignedInteger(argv[index+ argTime],&intTime);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpJoinTimeAllNo);
    }
  }

  rc = usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &current);
  if ( rc != L7_SUCCESS )
  {
    return cliPrompt(ewsContext);
  }
  else
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( (rc=usmDbGarpJoinTimeSet(unit, current, intTime)) != L7_SUCCESS )
      {
        if (rc == L7_ERROR)
        {
          memset(buf, 0, sizeof(buf)); /* in case usmDb call below fails */
          if (usmDbUnitSlotPortGet(current, &unit, &s, &p) == L7_SUCCESS)
          {
            osapiSnprintf(buf, sizeof(buf), pStrErr_base_GarpGlobalTimerValueInvalid, cliDisplayInterfaceHelp(unit, s, p));
          }
          return cliSyntaxReturnPromptAddBlanks(1,1,0,0, buf, ewsContext, pStrErr_base_GarpJoinTimerValueInvalid);
        }

        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_JoinTimerBeSet);
      }
    }
  }
  while (1)
  {
    rc = usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, current, &next);
    if (rc != L7_SUCCESS)
    {
      break;
    }
    current = next;

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ((rc=usmDbGarpJoinTimeSet(unit, current, intTime)) != L7_SUCCESS )
      {
        if (rc == L7_ERROR)
        {
          memset(buf, 0, sizeof(buf)); /* in case usmDb call below fails */
          if (usmDbUnitSlotPortGet(current, &unit, &s, &p) == L7_SUCCESS)
          {
            osapiSnprintf(buf, sizeof(buf), pStrErr_base_GarpGlobalTimerValueInvalid, cliDisplayInterfaceHelp(unit, s, p));
          }
          return cliSyntaxReturnPromptAddBlanks(1,1,0,0, buf, ewsContext, pStrErr_base_GarpJoinTimerValueInvalid);
        }

        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_JoinTimerBeSet);
      }
    }
  }  /* end while(1) */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  sets the garp leave timer
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax set garp timer leave <20-600>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetGarpTimerLeave(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argTime = 1;
  L7_int32 interface = 0;
  L7_int32 intSlot;
  L7_int32 intPort;
  L7_int32 intTime = L7_GARP_DEF_LEAVE_TIME;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_RC_t rc, status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpLeaveTime_1);
    }

    (void) cliConvertTo32BitUnsignedInteger(argv[index+ argTime],&intTime);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpLeaveTimeNo);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &intSlot, &intPort) != L7_SUCCESS)
        {
          continue;
        }
        if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) == L7_SUCCESS)
        {
          if (usmDbGarpIsValidIntf(unit, interface) != L7_TRUE)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, intSlot, intPort));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
            status = L7_FAILURE;
            continue;
          }
        }
    
        /* examine the argument that represent the time in centiseconds */
        if ( (rc=usmDbGarpLeaveTimeSet(unit, interface, intTime)) != L7_SUCCESS )
        {
          if (rc == L7_REQUEST_DENIED)
          {
            return cliSyntaxReturnPromptAddBlanks(1,1,0,0, pStrErr_common_Error, ewsContext, pStrErr_base_GarpLeaveTimerValueInvalid);
          }

          if (rc == L7_ERROR)
          {
            return cliSyntaxReturnPromptAddBlanks(1,1,0,0, pStrErr_common_Error, ewsContext, pStrErr_base_GarpJoinTimerValueInvalid);
          }

          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, intSlot, intPort));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_LeaveTimerBeSet);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  sets the garp leave timer
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax set garp timer leave all <20-600>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetGarpTimerLeaveAll(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argTime = 1;
  L7_uint32 unit, s, p;
  L7_uint32 current, next;
  L7_RC_t rc;
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 intTime = L7_GARP_DEF_LEAVE_TIME;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpLeaveTime_1);
    }

    (void) cliConvertTo32BitUnsignedInteger(argv[index+ argTime],&intTime);

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpLeaveTimeAllNo);
    }
  }

  rc = usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &current);
  if ( rc != L7_SUCCESS )
  {
    return cliPrompt(ewsContext);
  }
  else
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( (rc=usmDbGarpLeaveTimeSet(unit, current, intTime)) != L7_SUCCESS )
      {
        memset(buf, 0, sizeof(buf)); /* in case usmDb call below fails */
        if (usmDbUnitSlotPortGet(current, &unit, &s, &p) == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf), pStrErr_base_GarpGlobalTimerValueInvalid, cliDisplayInterfaceHelp(unit, s, p));
        }

        if (rc == L7_REQUEST_DENIED)
        {
          return cliSyntaxReturnPromptAddBlanks(1,1,0,0, buf, ewsContext, pStrErr_base_GarpLeaveTimerValueInvalid);
        }

        if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks(1,1,0,0, buf, ewsContext, pStrErr_base_GarpJoinTimerValueInvalid);
        }

        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_LeaveTimerBeSet);
      }
    }
  }
  while (1)
  {
    rc = usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, current, &next);
    if (rc != L7_SUCCESS)
    {
      break;
    }
    current = next;

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( (rc=usmDbGarpLeaveTimeSet(unit, current, intTime)) != L7_SUCCESS )
      {
        memset(buf, 0, sizeof(buf)); /* in case usmDb call below fails */
        if (usmDbUnitSlotPortGet(current, &unit, &s, &p) == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf), pStrErr_base_GarpGlobalTimerValueInvalid, cliDisplayInterfaceHelp(unit, s, p));
        }

        if (rc == L7_REQUEST_DENIED)
        {
          return cliSyntaxReturnPromptAddBlanks(1,1,0,0, buf, ewsContext, pStrErr_base_GarpLeaveTimerValueInvalid);
        }

        if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks(1,1,0,0, buf, ewsContext, pStrErr_base_GarpJoinTimerValueInvalid);
        }

        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_LeaveTimerBeSet);
      }
    }
  }  /* end while(1) */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  sets the garp leave all timer
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax set garp timer leaveall <200-6000>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetGarpTimerLeaveAllTime(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argTime = 1;
  L7_int32 interface = 0;
  L7_int32 intSlot;
  L7_int32 intPort;
  L7_int32 intTime = L7_GARP_DEF_LEAVE_ALL_TIME;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_RC_t rc, status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpLeaveAllTime);
    }

    (void) cliConvertTo32BitUnsignedInteger(argv[index+ argTime],&intTime);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpLeaveAllTimeAllNo);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &intSlot, &intPort) != L7_SUCCESS)
        {
          continue;
        }
        if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) == L7_SUCCESS)
        {
          if (usmDbGarpIsValidIntf(unit, interface) != L7_TRUE)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, intSlot, intPort));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
            status = L7_FAILURE;
            continue;
          }
        }
        /* examine the argument that represent the time in centiseconds */
        if ( (rc=usmDbGarpLeaveAllTimeSet(unit, interface, intTime)) != L7_SUCCESS )
        {
          if (rc == L7_ERROR)
          {
            return cliSyntaxReturnPromptAddBlanks(1,1,0,0, pStrErr_common_Error, ewsContext, pStrErr_base_GarpLeaveTimerValueInvalid);
          }

          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, intSlot, intPort));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_LeaveAllTimerBeSet);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  sets  the garp leave all timer
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax set garp timer leaveall all <200-6000>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetGarpTimerLeaveAllTimeAll(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argTime = 1;
  L7_int32 intTime = L7_GARP_DEF_LEAVE_ALL_TIME;
  L7_uint32 next, current,rc;
  L7_uint32 unit, s, p;
  L7_uint32 numArg;        /* New variable Added */
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpLeaveAllTime);
    }

    (void) cliConvertTo32BitUnsignedInteger(argv[index+ argTime],&intTime);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgGarpLeaveAllTimeAllNo);
    }
  }

  rc = usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &current);
  if ( rc != L7_SUCCESS )
  {
    return cliPrompt(ewsContext);
  }
  else
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( (rc=usmDbGarpLeaveAllTimeSet(unit, current, intTime)) != L7_SUCCESS )
      {
        if (rc == L7_ERROR)
        {
          memset(buf, 0, sizeof(buf)); /* in case usmDb call below fails */
          if (usmDbUnitSlotPortGet(current, &unit, &s, &p) == L7_SUCCESS)
          {
            osapiSnprintf(buf, sizeof(buf), pStrErr_base_GarpGlobalTimerValueInvalid, cliDisplayInterfaceHelp(unit, s, p));
          }
          return cliSyntaxReturnPromptAddBlanks(1,1,0,0, buf, ewsContext, pStrErr_base_GarpLeaveTimerValueInvalid);
        }

        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_LeaveAllTimerBeSet);
      }
    }
  }
  while (1)
  {
    rc = usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, current, &next);
    if (rc != L7_SUCCESS)
    {
      break;
    }
    current = next;

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( (rc=usmDbGarpLeaveAllTimeSet(unit, current, intTime)) != L7_SUCCESS )
      {
        if (rc == L7_ERROR)
        {
          memset(buf, 0, sizeof(buf)); /* in case usmDb call below fails */
          if (usmDbUnitSlotPortGet(current, &unit, &s, &p) == L7_SUCCESS)
          {
            osapiSnprintf(buf, sizeof(buf), pStrErr_base_GarpGlobalTimerValueInvalid, cliDisplayInterfaceHelp(unit, s, p));
          }
          return cliSyntaxReturnPromptAddBlanks(1,1,0,0, buf, ewsContext, pStrErr_base_GarpLeaveTimerValueInvalid);
        }

        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_LeaveAllTimerBeSet);
      }
    }
  }  /* end while(1) */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Configure GVRP on a per port basis
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index*
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes
 *
 * @cmdsyntax  set gvrp interfacemode
 *
 * @cmdhelp     Enable or Disable port GVRP. Valid when switch GVRP is enabled
 *
 * @cmddescript    Configure GVRP on a per port basis
 *    none
 * @end
 *
 *********************************************************************/
const L7_char8  *commandSetGVRPInterfaceMode(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface = 0;
  L7_uint32 slot, port;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 value = L7_ENABLE;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgGarpGvrpIntfMode_1);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgGarpGvrpIntfModeNo);
    }
    value = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &slot, &port) != L7_SUCCESS)
        {
          continue;
        }
        if (usmDbGarpIsValidIntf(unit, interface) != L7_TRUE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          status = L7_FAILURE;
          continue;
        }
        rc = usmDbQportsEnableGVRPSet(unit, interface, value);
    
        if (rc == L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          status = L7_FAILURE;
        }
        else if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotEnblGvrp);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Configure GVRP for all Interfaces
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index*
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes
 *
 * @cmdsyntax set gvrp interfacemode all
 *
 * @cmdhelp     Enable or Disable port GVRP. Valid when switch GVRP is enabled
 *
 * @cmddescript    Configure GVRP on a per port basis
 *    none
 * @end
 *
 *********************************************************************/
const L7_char8  *commandSetGVRPInterfaceModeAll(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 interface = 0;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* For all the ports */
  interface = L7_ALL_INTERFACES;

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgGarpGvrpIntfMode_1);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbQportsEnableGVRPSet(unit, interface, L7_ENABLE);
      if (rc == L7_NOT_SUPPORTED)
      {
        /* port selected cannot perform this function */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotEnblGvrp);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgGarpGvrpIntfModeNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbQportsEnableGVRPSet(unit, interface, L7_DISABLE);
      if (rc == L7_NOT_SUPPORTED)
      {
        /* port selected cannot perform this function */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_NotDsblGvrp);
      }
    }
  }

  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgGarpGvrpIntfMode_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Administrative Mode.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax    set igmp
 *
 * @cmdhelp
 *
 * @cmddescript  This command enables and disables IGMP Snooping.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPGlobal(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGADMINMODE(family));
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopAdminModeSet(unit, L7_ENABLE, family);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGADMINMODE_NO(family));
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopAdminModeSet(unit, L7_DISABLE, family);
    }
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGADMINMODE(family));
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc != L7_SUCCESS)
    {
      cliSyntaxNewLine( ewsContext);
      return cliSyntaxReturnPrompt (ewsContext, CLIERROR_SET_IGMP_SNOOPING_ADMIN_MODE(family));
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}
/*********************************************************************
*
* @purpose  Sets whether the optional Router Alert field is required.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  set igmp router-alert-check
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpIgmpSnoopingRouterAlert(EwsContext ewsContext,
                                                 L7_uint32 argc, const L7_char8 **argv,
                                                 L7_uint32 index)
{
  L7_BOOL mode = L7_FALSE;
  L7_uint32 unit;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_FALSE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if ( L7_EXECUTE_SCRIPT == ewsContext->scriptActionFlag )
  {
    if ( L7_SUCCESS != usmDbSnoopRouterAlertMandatorySet(mode, L7_AF_INET))
    {
      cliSyntaxBottom( ewsContext );
      ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrInfo_ipmcast_IgmpRouterAlertCheck);
      return( cliPrompt( ewsContext ) );
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return( cliPrompt(ewsContext) );
}


/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Interface Mode.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   set igmp
 *
 * @cmdhelp
 *
 * @cmddescript  This command enables and disables IGMP Snooping on an interface.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPInterface(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 s, p, intIfNum=0;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uchar8 family = L7_AF_INET;
  L7_RC_t status = L7_SUCCESS;
  L7_uint32 value = L7_ENABLE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext,CLISYNTAX_CONFIGIGMPSNOOPINGADMINMODE(family));
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext,CLISYNTAX_CONFIGIGMPSNOOPINGADMINMODE_NO(family));
    }
    value = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGINTERFACEMODE(family));
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (intIfNum=1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIfNum))
      {
        if(usmDbUnitSlotPortGet(intIfNum, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }
        /* don't allow if the ifIndex is a PORTCHANNEL member */
        if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
            && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          osapiSnprintf(buf, sizeof(buf), "\r\n%s ", cliDisplayInterfaceHelp(unit, s, p));
          OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);
          ewsTelnetWrite (ewsContext, buf);
          status = L7_FAILURE;
          continue;
        }
    
        rc = usmDbSnoopIntfModeSet(unit, intIfNum, value, family);
        if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetIntf);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);

}
/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Interface Mode.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none   set igmp interfacemode all
 *
 * @cmdsyntax  config igmpsnooping interfacemode <slot/port / all> <enable/disable>
 *
 * @cmdhelp
 *
 * @cmddescript  This command enables and disables IGMP Snooping on an interface.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPInterfaceModeAll(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32  intIfNum, next;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGINTERFACEMODE(family));
    }
    /* For all the ports */
    intIfNum=0;

    while (usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &next) == L7_SUCCESS)
    {
      intIfNum = next;
      /* don't allow if the ifIndex is a PORTCHANNEL member */
      if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
          && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
      {
        continue;
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSnoopIntfModeSet(unit, intIfNum, L7_ENABLE, family);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGINTERFACEMODE_NO(family));
    }
    /* For all the ports */
    intIfNum=0;

    while (usmDbIntIfNumTypeNextGet(unit, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, intIfNum, &next) == L7_SUCCESS)
    {
      intIfNum = next;
      /* don't allow if the ifIndex is a PORTCHANNEL member */
      if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
          && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
      {
        continue;
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSnoopIntfModeSet(unit, intIfNum, L7_DISABLE, family);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGINTERFACEMODE(family));
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetIntf);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}
/*********************************************************************
 *
 * @purpose  Configures the Fast-Leave Admin Mode for a specified interface.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   set igmp fast-leave
 *
 * @cmdhelp
 *
 * @cmddescript  This command enables and disables IGMP Snooping on an interface.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPInterfaceFastLeave(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 s, p, intIfNum=0;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uchar8 family = L7_AF_INET;
  L7_uint32 value = L7_ENABLE;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext,CLISYNTAX_CONFIGIGMPSNOOPINGFASTLEAVEADMINMODE(family));
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext,CLISYNTAX_CONFIGIGMPSNOOPINGFASTLEAVEADMINMODE_NO(family));
    }
    value = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGIGMPSNOOPINGFASTLEAVEADMINMODE(family));
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (intIfNum=1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIfNum))
      {
        if(usmDbUnitSlotPortGet(intIfNum, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }
        /* don't allow if the ifIndex is a PORTCHANNEL member */
        if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
            && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          osapiSnprintf(buf, sizeof(buf), "\r\n%d/%d ", s, p);
          OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);
          ewsTelnetWrite (ewsContext, buf);
          status = L7_FAILURE;
          continue;
        }
    
        rc = usmDbSnoopIntfFastLeaveAdminModeSet(unit, intIfNum, value, family);
        if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (0, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetIntfFastLeave);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);

}
/*********************************************************************
 *
 * @purpose  Configures the Mrouter Attached Mode for a specified interface.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   set igmp mrouter interface
 *
 * @cmdhelp
 *
 * @cmddescript  This command enables and disables Mrouter Attached Mode on an interface.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPMrouterInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 s, p, intIfNum=0;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 argInterface=1;
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 vlanId;
  L7_uchar8 family = L7_AF_INET;
  L7_RC_t status = L7_SUCCESS;
  L7_BOOL useVlan = L7_FALSE;
  L7_uint32 value = L7_ENABLE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SETIGMPSNOOPINGMROUTERINTERFACE(family));
    }

    if(cliConvertTo32BitUnsignedInteger(argv[index+argInterface], &vlanId) == L7_SUCCESS)
    {
      useVlan = L7_TRUE;
    }
    else
    {
      if (strcmp(argv[index+argInterface], pStrInfo_common_Ipv6DhcpRelayIntf_1) != 0)
      {
        return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SETIGMPSNOOPINGMROUTERINTERFACE(family));
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SETIGMPSNOOPINGMROUTERINTERFACE_NO(family));
    }
    if(cliConvertTo32BitUnsignedInteger(argv[index+argInterface], &vlanId) == L7_SUCCESS)
    {
      useVlan = L7_TRUE;
    }
    else
    {
      if (strcmp(argv[index+argInterface], pStrInfo_common_Ipv6DhcpRelayIntf_1) != 0)
      {
        return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SETIGMPSNOOPINGMROUTERINTERFACE_NO(family));
      }
    }
    value = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SETIGMPSNOOPINGMROUTERINTERFACE(family));
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (intIfNum=1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIfNum))
      {
        if(usmDbUnitSlotPortGet(intIfNum, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }
        /* don't allow if the ifIndex is a PORTCHANNEL member */
        if ((usmDbDot3adValidIntfCheck(unit, intIfNum) == L7_TRUE)
            && (usmDbDot3adIsConfigured(unit, intIfNum) != L7_TRUE))
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          osapiSnprintf(buf, sizeof(buf), "\r\n%d/%d ", s, p);
          OSAPI_STRNCAT(buf, pStrErr_base_IsNotACfguredLag);
          ewsTelnetWrite (ewsContext, buf);
          status = L7_FAILURE;
          continue;
        }
    
        if (useVlan == L7_TRUE)
        {
          rc = usmDbsnoopIntfApiVlanStaticMcastRtrSet(unit, intIfNum, vlanId, value, family);
        }
        else
        {
          rc = usmDbSnoopIntfMrouterSet(unit, intIfNum, value, family);
        }
        if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetIntf);
          status = L7_FAILURE;
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}
/***********Code Addition TM2 **************/

/*********************************************************************
 *
 * @purpose  enable/disable the linktrap for the port
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  snmp trap link-status
 *
 * @cmdhelp Enable/Disable Link Up/Down traps for this port.
 *
 * @cmddescript
 *   This is a configurable value and can be Enabled or Disabled. It
 *   allows you to enable or disable link status traps by port. This
 *   parameter is only valid when Link Up/Down Flag is enabled on the
 *   Trap Flags Configuration Menu.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpTrapLinkStatus(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface;
  L7_uint32 unit;
  L7_uint32 slot,port;
  L7_uint32 numArg;      /* New variable Added  */
  L7_RC_t status = L7_SUCCESS;
  L7_uint32 value = L7_ENABLE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg =  cliNumFunctionArgsGet();

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments  */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SnmpTrapLinksTatus);
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments  */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SnmpTrapLinksTatusNo);
    }
    value = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &slot, &port) != L7_SUCCESS)
        {
          continue;
        }
        rc = usmDbIfLinkUpDownTrapEnableSet(unit, interface, value);
        if (rc == L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          status = L7_FAILURE;
          continue;
    
        }
        else if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SnmpTrapLinksTatus);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable/disable the linktrap for all ports
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  snmp trap link status all
 *
 * @cmdhelp Enable/Disable Link Up/Down traps for all ports.
 *
 * @cmddescript
 *   This is a configurable value and can be Enabled or Disabled. It
 *   allows you to enable or disable link status traps by port. This
 *   parameter is only valid when Link Up/Down Flag is enabled on the
 *   Trap Flags Configuration Menu.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpTrapLinkStatusAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface, nextInterface;
  L7_uint32 unit;
  L7_uint32 numArg;      /* New variable Added  */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg =  cliNumFunctionArgsGet();

  if (L7_SUCCESS != usmDbValidIntIfNumFirstGet(&interface))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox_1);
  }

  while (interface)
  {
    if (ewsContext->commType ==  CLI_NORMAL_CMD)
    {
      /* Error Checking for Number of Arguments  */
      if (numArg != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SnmpTrapLinksTatusAll);
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbIfLinkUpDownTrapEnableSet(unit,  interface, L7_ENABLE);
      }
    }
    else if (ewsContext->commType ==  CLI_NO_CMD)
    {
      /* Error Checking for Number of Arguments  */
      if (numArg != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SnmpTrapLinksTatusAllNo);
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbIfLinkUpDownTrapEnableSet(unit,  interface, L7_DISABLE);
      }
    }

    if (L7_SUCCESS != usmDbValidIntIfNumNext(interface, &nextInterface))
    {
      interface = 0;
    }
    else
    {
      interface = nextInterface;
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  set the port speed and duplex setting
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  speed {{100 | 10} {half-duplex|full-duplex} | 1000 full-duplex}
 *
 * @cmdhelp Configure port speed and duplex setting.
 *
 * @cmddescript
 *   This is a configurable value and indicates the speed and duplex
 *   setting for the port. The value of Auto (autodetect) is valid
 *   only for 10/100BASE-TX ports.
 *
 *       100 Half: 100BASE-T half-duplex
 *       100 Full: 100BASE-T full duplex
 *       10 Half: 10BASE-T half duplex
 *       10 Full: 100BASE-T full duplex
 *       100FX Half: 100BASE-FX half duplex
 *       100FX Full: 100BASE-FX full duplex
 *       1000SX Full: 1000BASE-SX full duplex
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSpeed(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index){
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface;
  L7_uint32 val;
  L7_uint32 portCapability;
  L7_uint32 unit;
  L7_uint32 s,p;
  L7_uint32 numArg;      /* New variable Added  */
  L7_uint32 argSpeed = 1, argDuplex = 2;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 2)
  {
    /*check to see if anything other than 2 arguments were passed */

    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_Speed_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }
        if (usmDbDot3adIsMember(unit, interface) == L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_Cant,  ewsContext, pStrErr_base_CantChgSpeedOfALagMbr);
          status = L7_FAILURE;
          continue;
        }
      
        if (usmDbIfAutoNegAdminStatusGet(unit, interface, &val) == L7_SUCCESS)
        {
          if (val == L7_ENABLE)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 2, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DsblAutoNeg);
            status = L7_FAILURE;
            continue;
          }
        }

        usmDbIntfPhyCapabilityGet(interface, &portCapability);
        /* use config port autoneg
           if (strcmp(argv[index+argMode], "auto") == 0)
           if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
           rc = usmDbIfSpeedSet(unit, interface, L7_PORTCTRL_PORTSPEED_AUTO_NEG);*/
      
        if(strcmp(argv[index+argSpeed], pStrInfo_base_Num100) == 0 && (strcmp(argv[index+argDuplex], pStrInfo_base_HalfDuplex)) == 0  &&
            (portCapability & L7_PHY_CAP_PORTSPEED_HALF_100))
        {
          rc = usmDbIfSpeedSet(unit, interface, L7_PORTCTRL_PORTSPEED_HALF_100TX);
        }
        else if (strcmp(argv[index+argSpeed], pStrInfo_base_Num100) == 0 && (strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex)) == 0  &&
            (portCapability & L7_PHY_CAP_PORTSPEED_FULL_100))
        {
          rc = usmDbIfSpeedSet(unit, interface, L7_PORTCTRL_PORTSPEED_FULL_100TX);
        }
        else if (strcmp(argv[index+argSpeed], pStrInfo_base_Num10) == 0 && (strcmp(argv[index+argDuplex], pStrInfo_base_HalfDuplex)) == 0  &&
            (portCapability & L7_PHY_CAP_PORTSPEED_HALF_10))
        {
          rc = usmDbIfSpeedSet(unit, interface, L7_PORTCTRL_PORTSPEED_HALF_10T);
        }
        else if (strcmp(argv[index+argSpeed], pStrInfo_base_Num10) == 0 && (strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex)) == 0  &&
            (portCapability & L7_PHY_CAP_PORTSPEED_FULL_10))
        {
          rc = usmDbIfSpeedSet(unit, interface, L7_PORTCTRL_PORTSPEED_FULL_10T);
        }
        /* PTin added: Speed 2.5G */
        else if (strcmp(argv[index+argSpeed], pStrInfo_base_Num2500) == 0 && (strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex)) == 0  &&
            (portCapability & L7_PHY_CAP_PORTSPEED_FULL_2500))
        {
          rc = usmDbIfSpeedSet(unit, interface, L7_PORTCTRL_PORTSPEED_FULL_2P5FX);
        }
        /* PTin end */
        else if (strcmp(argv[index+argSpeed], pStrInfo_base_10g) == 0 && (strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex)) == 0  &&
            (portCapability & L7_PHY_CAP_PORTSPEED_FULL_10G))
        {
          rc = usmDbIfSpeedSet(unit, interface, L7_PORTCTRL_PORTSPEED_FULL_10GSX);
        }
        /* PTin added: Speed 40G */
        else if (strcmp(argv[index+argSpeed], pStrInfo_base_40g) == 0 && (strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex)) == 0  &&
            (portCapability & L7_PHY_CAP_PORTSPEED_FULL_40G))
        {
          rc = usmDbIfSpeedSet(unit, interface, L7_PORTCTRL_PORTSPEED_FULL_40G_KR4);
        }
        /* PTin added: Speed 100G */
        else if (strcmp(argv[index+argSpeed], pStrInfo_base_100g) == 0 && (strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex)) == 0  &&
            (portCapability & L7_PHY_CAP_PORTSPEED_FULL_100G))
        {
          rc = usmDbIfSpeedSet(unit, interface, L7_PORTCTRL_PORTSPEED_FULL_100G_BKP);
        }
        /* PTin end */
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_PhyModeForPort);
          status = L7_FAILURE;
          continue;
        }

        if (rc == L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          status = L7_FAILURE;
        }
        else if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_Speed_1);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  set the port speed and duplex setting for all ports
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  speed all {{100|10} {half-duplex|full-duplex} | 1000 full-duplex}
 *
 * @cmdhelp Configure port speed and duplex setting.
 *
 * @cmddescript
 *   This is a configurable value and indicates the speed and duplex
 *   setting for the port. The value of Auto (autodetect) is valid
 *   only for 10/100BASE-TX ports.
 *
 *       100 Half: 100BASE-T half-duplex
 *       100 Full: 100BASE-T full duplex
 *       10 Half: 10BASE-T half duplex
 *       10 Full: 100BASE-T full duplex
 *       100FX Half: 100BASE-FX half duplex
 *       100FX Full: 100BASE-FX full duplex
 *       1000SX Full: 1000BASE-SX full duplex
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSpeedAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 intIfnum, val2;
  L7_uint32 portCapability;
  L7_uint32 portChannelHasMembers = L7_FALSE;
  L7_BOOL flag = L7_FALSE;
  L7_BOOL portFail = L7_FALSE;
  L7_uint32 unit;
  L7_uint32 numArg;      /* New variable Added  */
  L7_uint32 argSpeed = 1, argDuplex = 2;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 2)
  {                                   /*check to see if anything other than 2 arguments were passed */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SpeedAll_1);
  }

  intIfnum = 0;
  while(usmDbGetNextPhysicalIntIfNumber(intIfnum, &intIfnum) == L7_SUCCESS)
  {
    usmDbIntfPhyCapabilityGet(intIfnum, &portCapability);
    if (usmDbDot3adIsMember(unit, intIfnum) == L7_SUCCESS)
    {
      portChannelHasMembers=L7_TRUE;
      /*  break; */
    }
    else
    {
      if (usmDbIfAutoNegAdminStatusGet(unit, intIfnum, &val2) == L7_SUCCESS)
      {
        if (val2 == L7_ENABLE)
        {
          if ((portCapability & L7_PHY_CAP_PORTSPEED_FULL_10G) == L7_FALSE)
          {
            flag = L7_TRUE;
          }
        }
      }
      if ( (strcmp(argv[index+argSpeed], pStrInfo_base_Num100) == 0) && (strcmp(argv[index+argDuplex], pStrInfo_base_HalfDuplex) == 0) &&
          (portCapability & L7_PHY_CAP_PORTSPEED_HALF_100))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbIfSpeedSet(unit, intIfnum, L7_PORTCTRL_PORTSPEED_HALF_100TX);
        }
      }
      else if ((strcmp(argv[index+argSpeed], pStrInfo_base_Num100) == 0) && (strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex) == 0) &&
          (portCapability & L7_PHY_CAP_PORTSPEED_HALF_100))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbIfSpeedSet(unit, intIfnum, L7_PORTCTRL_PORTSPEED_FULL_100TX);
        }
      }
      else if ((strcmp(argv[index+argSpeed], pStrInfo_base_Num10) == 0) && (strcmp(argv[index+argDuplex], pStrInfo_base_HalfDuplex) == 0)  &&
          (portCapability & L7_PHY_CAP_PORTSPEED_HALF_10))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbIfSpeedSet(unit, intIfnum, L7_PORTCTRL_PORTSPEED_HALF_10T);
        }
      }
      else if((strcmp(argv[index+argSpeed], pStrInfo_base_Num10) == 0) && (strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex) == 0)  &&
          (portCapability & L7_PHY_CAP_PORTSPEED_FULL_10))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbIfSpeedSet(unit, intIfnum, L7_PORTCTRL_PORTSPEED_FULL_10T);
        }
      }
#if 1   /* PTin changed 0 -> 1 */
      else if((strcmp(argv[index+argSpeed], pStrInfo_base_Num1000) == 0) &&(strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex) == 0)  &&
          (portCapability & L7_PHY_CAP_PORTSPEED_FULL_1000))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbIfSpeedSet(unit, intIfnum, L7_PORTCTRL_PORTSPEED_FULL_1000SX);
        }
      }
#endif
      /* PTin added: Speed 2.5G */
      else if((strcmp(argv[index+argSpeed], pStrInfo_base_Num2500) == 0) && (strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex) == 0)  &&
          (portCapability & L7_PHY_CAP_PORTSPEED_FULL_2500))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbIfSpeedSet(unit, intIfnum, L7_PORTCTRL_PORTSPEED_FULL_2P5FX);
        }
      }
      /* PTin end */
      else if((strcmp(argv[index+argSpeed], pStrInfo_base_10g) == 0) &&(strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex) == 0)  &&
          (portCapability & L7_PHY_CAP_PORTSPEED_FULL_10G))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbIfSpeedSet(unit, intIfnum, L7_PORTCTRL_PORTSPEED_FULL_10GSX);
        }
      }
      /* PTin added: Speed 40G */
      else if((strcmp(argv[index+argSpeed], pStrInfo_base_40g) == 0) &&(strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex) == 0)  &&
          (portCapability & L7_PHY_CAP_PORTSPEED_FULL_40G))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbIfSpeedSet(unit, intIfnum, L7_PORTCTRL_PORTSPEED_FULL_40G_KR4);
        }
      }
      /* PTin added: Speed 100G */
      else if((strcmp(argv[index+argSpeed], pStrInfo_base_100g) == 0) &&(strcmp(argv[index+argDuplex], pStrInfo_base_FullDuplex) == 0)  &&
          (portCapability & L7_PHY_CAP_PORTSPEED_FULL_100G))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbIfSpeedSet(unit, intIfnum, L7_PORTCTRL_PORTSPEED_FULL_100G_BKP);
        }
      }
      /* PTin end */
      else
      {
        /* 10G ports do not support fixed speeds. Do not fail */
        if ((portCapability & L7_PHY_CAP_PORTSPEED_FULL_10G) == L7_FALSE)
        {
          portFail = L7_TRUE;
        }
      }
    }

  } /* endwhile */

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc == L7_NOT_SUPPORTED)
    {
      /* port selected cannot perform this function */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_SpeedAll_1);
    }

    if (portFail == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SetToPhyMode);
      cliSyntaxBottom(ewsContext);
    }
    if (flag == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DsblAutoNeg);
      cliSyntaxBottom(ewsContext);
    }
    if (portChannelHasMembers == L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_RequiresDeletingLag);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  set the system contact, system name and system location
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes add argument checking
 * @notes if "" are not used, only the first string is accepted
 *
 * @cmdsyntax  snmp-server {sysname <name> | location <loc> | contact <con>}
 *
 * @cmdhelp Enter system contact (Max 31 alpha-numeric char8acters).
 *
 * @cmddescript Identifies the person responsible for your network (for example, you network admin
 istrator) Specify up to 31 alphanumeric char8acters. The default is blank.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_char8 temp[L7_SYS_SIZE];
  L7_char8 strSysAttr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSysAttrToken = 1;/* New variable Introduced for the Token on the Command Line */
  L7_uint32 argSysAttrValue = 2;/* New variable Introduced for the value of System Attribute */
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* convert System Attribute to lower case */
  if (strlen(argv[index+argSysAttrToken]) <= L7_CLI_MAX_STRING_LENGTH)
  {
    OSAPI_STRNCPY_SAFE(strSysAttr, argv[index+argSysAttrToken]);
    cliConvertToLowerCase(strSysAttr);
  }
  memset (temp, 0, sizeof(temp));
  if (strcmp (strSysAttr,pStrInfo_base_Sysname) == 0)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgSysname_1);
    }
    if( strlen(argv[index+argSysAttrValue]) >= L7_SYS_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgName_1);
    }
    /* Error Checking for Number of Arguments */
    OSAPI_STRNCPY_SAFE(temp, (L7_char8 *)argv[index+argSysAttrValue]);

    if (temp[0] != '"' && strlen((L7_char8 *)argv[index+argSysAttrValue]) < L7_SYS_SIZE)
    {
      if (cliIsAlphaNum(temp) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgCharInput);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgName_1);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDb1213SysNameSet(unit, temp) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgSysname_1);
      }
    }
  }
  else if (strcmp (strSysAttr,pStrInfo_common_ApShowRunningLocation) == 0)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgSysLocation_1);
    }
    if( strlen(argv[index+argSysAttrValue]) >= L7_SYS_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgName_1);
    }

    OSAPI_STRNCPY_SAFE(temp, (L7_char8 *)argv[index+argSysAttrValue]);
    if (cliIsLocation(temp) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgCharInput);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDb1213SysLocationSet(unit, temp) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgSysLocation_1);
      }
    }
  }
  else if (strcmp (strSysAttr,pStrInfo_base_Contact) == 0)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgSysContact_1);
    }

    if (strlen(argv[index+argSysAttrValue]) >= sizeof(temp))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgSysContact_1);
    }

    OSAPI_STRNCPY_SAFE(temp, (L7_char8 *)argv[index+argSysAttrValue]);

    if (cliIsValidSysContact(temp) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgCharInput);

    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDb1213SysContactSet(unit, temp) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgSysContact_1);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (2, 2, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/* SSH TELNET */

/*********************************************************************
 *
 * @purpose  configure the telnet session timeout in minutes
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  telnetcon timeout <0-160>
 *
 * @cmdhelp Configure the telnet login inactivity timeout.
 *
 * @cmddescript
 *   A session is active as long as the session has not remained idle
 *   for the value set. Specify a decimal value from 0 to 160 minutes.
 *   A value of 0 indicates that a Telnet session remains active
 *   indefinitely. The default is 5 minutes.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandTelnetConTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 tempInt = FD_CLI_WEB_DEFAULT_REMOTECON_TIMEOUT;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgRemoteConTimeout);
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1],  &tempInt)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoRemoteConTimeout);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbAgentTelnetTimeoutSet(unit, tempInt);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/* SSH TELNET */

/*********************************************************************
 *
 * @purpose  configure the telnet session timeout in minutes
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  telnetcon timeout <0-160>
 *
 * @cmdhelp Configure the telnet login inactivity timeout.
 *
 * @cmddescript
 *   A session is active as long as the session has not remained idle
 *   for the value set. Specify a decimal value from 0 to 160 minutes.
 *   A value of 0 indicates that a Telnet session remains active
 *   indefinitely. The default is 5 minutes.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandRemoteConTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 tempInt = FD_CLI_WEB_DEFAULT_REMOTECON_TIMEOUT;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgRemoteConTimeout);
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1],  &tempInt)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoRemoteConTimeout);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbAgentTelnetTimeoutSet(unit, tempInt);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  set aging timeout for forwarding databases
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  bridge aging-time <10-1000000> [fdbid | all]
 *
 * @cmdhelp Configure the aging timeout for dynamically learned info.
 *
 * @cmddescript
 *   Indicates the timeout period (in seconds) for aging out dynamically
 *   learned forwarding information. The range is 10 to 1 million (seconds).
 *   The default is 300 (seconds).
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandBridgeAgingTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_FDB_TYPE_t fdb_type = L7_SVL;
  L7_uint32 argTimeout = 1;
  L7_uint32 argFdbId = 2;
  L7_uint32 argNoFdbId = 2;

  L7_uint32 numArgs;
  L7_uint32 fdbId;
  L7_uint32 seconds = 0;

  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 all = L7_FALSE;
  L7_uint32 vlanFeature = L7_FALSE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();

  usmDbFDBTypeOfVLGet(unit, &fdb_type);

  /* Check to see if VLAN Feature is supported */
  vlanFeature = usmDbFeaturePresentCheck(unit,
      L7_FDB_COMPONENT_ID, L7_FDB_AGETIME_PER_VLAN_FEATURE_ID);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Normal form of the command */
    if ((numArgs < 1) || (numArgs > 2))
    {
      if (vlanFeature == L7_FALSE)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_base_CfgFdbAgeTimeSvl, L7_FDB_MIN_AGING_TIMEOUT, L7_FDB_MAX_AGING_TIMEOUT);
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_base_CfgFdbAgeTime, L7_FDB_MIN_AGING_TIMEOUT, L7_FDB_MAX_AGING_TIMEOUT,
            L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
      }
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }
    else
    {
      if (numArgs == 2)
      {
        if (strcmp(argv[index+argFdbId], pStrInfo_common_All) == 0)
        {
          all = L7_TRUE;
        }
      }
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argTimeout], &seconds) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    if (fdb_type == L7_SVL)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot1dTpAgingTimeSet(unit, seconds) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgFdbAgeTimeSet);
        }
      }
    }
    else
    {  /* fdb_type == L7_IVL */
      if (usmDbFeaturePresentCheck(unit, L7_FDB_COMPONENT_ID, L7_FDB_AGETIME_PER_VLAN_FEATURE_ID) == L7_TRUE  )
      {
        if (all != L7_TRUE )
        {
          sscanf(argv[index+argFdbId], "%d", &fdbId);
          if (L7_SUCCESS != usmDbFdbIdGet(unit, fdbId))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_CfgFdbAgeTimeInvalidFdbId);
          }

          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if (usmDbFDBAddressAgingTimeoutSet(unit, fdbId, seconds) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgFdbAgeTimeSet);
            }
          }
        }
        else
        {
          fdbId = 0;
          while (L7_SUCCESS == usmDbNextVlanGet(unit, fdbId, &fdbId))
          {
            /*******Check if the Flag is Set for Execution*************/
            if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              if (usmDbFDBAddressAgingTimeoutSet(unit, fdbId, seconds) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgFdbAgeTimeSet);
              }
            }
          }
        }
      }
      else
      {
          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if (usmDbFDBAddressAgingTimeoutSet(unit, FD_SIM_DEFAULT_MGMT_VLAN_ID, seconds) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgFdbAgeTimeSet);
            }
          }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* The NO form of the command */
    if ((vlanFeature == L7_FALSE) && (numArgs != 0))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_Cfg_4);
    }
    if ((vlanFeature ==  L7_TRUE  && (numArgs != 1)))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_Cfg_5, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    }

    if (numArgs == 1)
    {
      if (strcmp(argv[index+argNoFdbId], pStrInfo_common_All) == 0)
      {
        all = L7_TRUE;
      }
    }

    seconds = FD_FDB_DEFAULT_AGING_TIMEOUT;

    if (fdb_type == L7_SVL)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* fdb_type == L7_SVL */
        if (usmDbDot1dTpAgingTimeSet(unit, seconds) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgFdbAgeTimeSet);
        }
      }
    }
    else
    {
      /* fdb_type == L7_IVL */

      if (usmDbFeaturePresentCheck(unit, L7_FDB_COMPONENT_ID, L7_FDB_AGETIME_PER_VLAN_FEATURE_ID) == L7_TRUE  )
      {
        if (all != L7_TRUE )
        {
          sscanf(argv[index+argFdbId], "%d", &fdbId);
          if (L7_SUCCESS != usmDbFdbIdGet(unit, fdbId))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_CfgFdbAgeTimeInvalidFdbId);
          }

          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if (usmDbFDBAddressAgingTimeoutSet(unit, fdbId, seconds) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgFdbAgeTimeSet);
            }
          }
        }
        else
        {
          fdbId = 0;
          while (L7_SUCCESS == usmDbNextVlanGet(unit, fdbId, &fdbId))
          {
            /*******Check if the Flag is Set for Execution*************/
            if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
            {
              if (usmDbFDBAddressAgingTimeoutSet(unit, fdbId, seconds) != L7_SUCCESS)
              {
                return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgFdbAgeTimeSet);
              }
            }
          }
        }
      }
      else
      {
          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if (usmDbFDBAddressAgingTimeoutSet(unit, FD_SIM_DEFAULT_MGMT_VLAN_ID, seconds) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgFdbAgeTimeSet);
            }
          }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose to activate or de-activate an SNMP community
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax snmp-server community mode <community name>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   A community status of Enable/Active means that the community is
 *   active, allowing SNMP managers associated with this community to
 *   manage the switch according to its access right.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpserverCommunityMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index )
{
  L7_char8 name[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 commIndex;
  L7_uint32 argName = 1;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* validity chekcing access and arguments */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliNumFunctionArgsGet() != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseModeEnblDsblName);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseModeNoEnblDsblName);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (strlen(argv[index+argName]) >= sizeof(name))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseModeEnblDsblName);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseModeNoEnblDsblName);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  OSAPI_STRNCPY_SAFE(name, argv[index+argName]);

  commIndex = cliFindSNMPCommunityIndex(name);

  if (commIndex == CLI_INDEX_NOT_FOUND)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_CommunityDoesntExist);
  }
  else
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        rc = usmDbSnmpCommunityStatusSet(unit, commIndex, L7_SNMP_COMMUNITY_STATUS_VALID);
      }
      else if (ewsContext->commType == CLI_NO_CMD)
      {
        rc = usmDbSnmpCommunityStatusSet(unit, commIndex, L7_SNMP_COMMUNITY_STATUS_INVALID);
      }

      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetCommunityStatus);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose to add/delete a SNMP community trap
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8
 * @returns cliPrompt(ewsContext)
 *
 * @notes apply() will not save without IP
 *
 * @cmdsyntax snmptrap <name> <ipaddr|hostname>  snmpversion <snmpversion>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   This is the SNMP community name of the remote network manager;
 *   the name can be up to 16 characters, and is case-sensitive. The
 *   default value for the 6 undefined community names is Delete.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpTrap(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index )
{
  L7_uchar8 commName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 domainName[L7_DNS_DOMAIN_NAME_SIZE_MAX];
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_inet_addr_t inetAddr;
  L7_uchar8 trapVersion[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ipAddr = 0;
  L7_int32 emptyTrapCommIndex;
  L7_int32 trapCommIndex = 0;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 argName = 1;
  L7_uint32 argIpType=2,argIpadd = 3;
  L7_uint32 argNoIpadd = 3;
  L7_uint32 argVer = 5;
  L7_BOOL validAddress = L7_FALSE;
  L7_uint32 numArgs,ipAddrType=0;
  snmpTrapVer_t version = L7_SNMP_TRAP_VER_SNMPV2C;
  L7_uint32 currStatus;
  L7_uchar8 strAddrType[L7_CLI_MAX_STRING_LENGTH];
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  L7_in6_addr_t ipv6_address;
  L7_inet_addr_t destinationIP;
  memset(&ipv6_address, 0, sizeof(L7_in6_addr_t));
#endif
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArgs != 3) && (numArgs != 5))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseCreateSnmpTrap);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else
  {
    if ((numArgs != 3))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseNoCreateNameIpAddr);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArgs == 3) || (numArgs == 5))
    {

    if ((strlen(argv[index+argName]) >= sizeof(commName)) ||
        (strlen(argv[index+argIpadd]) >= sizeof(strIpAddr)))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseCreateNameIpAddr);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    OSAPI_STRNCPY_SAFE(commName, argv[index+argName]);

    /* check the length of the name to be 16 or fewer */
    if ( strlen(commName) > L7_SNMP_NAME_SIZE )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_NameMustBe16OrFew);
    }

    /* Verify if the specified ip address is valid */
    OSAPI_STRNCPY_SAFE(strIpAddr, argv[index+argIpadd]);
    strncpy(strAddrType, argv[index+argIpType], L7_CLI_MAX_STRING_LENGTH);
    if(strcmp(strAddrType,pStrInfo_base_Ipaddr_2)==0)
    {
      ipAddrType=4;

      /* validate the ip address as ipv4*/
      if (cliIPHostAddressValidate(ewsContext, strIpAddr, &ipAddr, &addrType) != L7_SUCCESS)
      {
        /* Invalid Host Address*/
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
      }

      if (addrType == L7_IP_ADDRESS_TYPE_IPV4)
      {
        if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS || (ipAddr == 0))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
        }
      }
      else if(addrType == L7_IP_ADDRESS_TYPE_DNS)
      {
        inetAddressReset(&inetAddr);
        rc = usmDbDNSClientInetNameLookup(L7_AF_INET, strIpAddr, &status, domainName, &inetAddr);
        /*  If the DNS lookup fails */
        if (rc == L7_SUCCESS)
        {
          inetAddrHtop(&inetAddr, strIpAddr);
          if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS || (ipAddr == 0))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_base_DnsLookupFailed);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
      }
    }
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
    else if(strcmp(strAddrType,pStrInfo_base_Ip6addr_1)==0)
    {
      ipAddrType=6;
  
      if (usmDbIPv6HostAddressValidate(strIpAddr, &destinationIP, &addrType) == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgNwIp);
      }
      if (addrType == L7_IP_ADDRESS_TYPE_DNS)
      {
        inetAddressZeroSet(L7_AF_INET6, &destinationIP);
        if (usmDbDNSClientInetNameLookup(L7_AF_INET6, strIpAddr, &status,
                                         domainName, &destinationIP) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,
                 ewsContext, pStrErr_base_DnsLookupFailed);
        }
        if (inet6AddressGet(&destinationIP, &ipv6_address) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
        }
        osapiInetNtop(L7_AF_INET6, (char *)&ipv6_address, strIpAddr, sizeof(strIpAddr));
      }
      if (addrType == L7_IP_ADDRESS_TYPE_IPV6)
      {
        if(osapiInetPton(L7_AF_INET6, strIpAddr, (L7_uchar8 *)&ipv6_address) !=L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
        } 
      }
    }
#endif
  }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ((strlen(argv[index+argName]) >= sizeof(commName)) ||
        (strlen(argv[index+argNoIpadd]) >= sizeof(strIpAddr)))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseNoCreateNameIpAddr);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    OSAPI_STRNCPY_SAFE(commName, argv[index+argName]);

    /* check the length of the name to be 16 or fewer */
    if ( strlen(commName) > L7_SNMP_NAME_SIZE )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_NameMustBe16OrFew);
    }

    /* Verify if the specified ip address is valid */
    OSAPI_STRNCPY_SAFE(strIpAddr, argv[index+argNoIpadd]);
    OSAPI_STRNCPY_SAFE(strAddrType, argv[index+argNoIpadd-1]);

    if(strcmp(strAddrType,pStrInfo_base_Ipaddr_2)==0)
    {
      ipAddrType=4;

      if (cliIPHostAddressValidate(ewsContext, strIpAddr, &ipAddr, &addrType) != L7_SUCCESS)
      {
        /* Invalid Host Address*/
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
      }
      if(addrType == L7_IP_ADDRESS_TYPE_DNS)
      {
        inetAddressReset(&inetAddr);
        rc = usmDbDNSClientInetNameLookup(L7_AF_INET, strIpAddr, &status, domainName, &inetAddr);
        /*  If the DNS lookup fails */
        if (rc == L7_SUCCESS)
        {
          inetAddrHtop(&inetAddr, strIpAddr);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_base_DnsLookupFailed);
        }
      }
      if (usmDbInetAton(strIpAddr, &ipAddr) == L7_SUCCESS || (ipAddr != 0))
      {
        validAddress = L7_TRUE;
      }
    }

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
    if(strcmp(strAddrType,pStrInfo_base_Ip6addr_1)==0)
    {
      ipAddrType=6;

      if (usmDbIPv6HostAddressValidate(strIpAddr, &destinationIP, &addrType) == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgNwIp);
      }
      if (addrType == L7_IP_ADDRESS_TYPE_DNS)
      {
        inetAddressZeroSet(L7_AF_INET6, &destinationIP);
        if (usmDbDNSClientInetNameLookup(L7_AF_INET6, strIpAddr, &status,
                                         domainName, &destinationIP) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,
                 ewsContext, pStrErr_base_DnsLookupFailed);
        }
        if (inet6AddressGet(&destinationIP, &ipv6_address) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
        }
        osapiInetNtop(L7_AF_INET6, (char *)&ipv6_address, strIpAddr, sizeof(strIpAddr));
      }
      if (addrType == L7_IP_ADDRESS_TYPE_IPV6)
      {
        if(osapiInetPton(L7_AF_INET6, strIpAddr, (L7_uchar8 *)&ipv6_address) ==L7_SUCCESS)
        {
          memset(strIpAddr, 0, sizeof(strIpAddr));
          osapiInetNtop(L7_AF_INET6, (char *)&ipv6_address, strIpAddr, sizeof(strIpAddr));         
        }
      }
      validAddress = L7_TRUE;
    }
#endif

    if(validAddress != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
    }
  }      

  if (numArgs == 5)
  {
    if(strlen(argv[index+argVer]) >= sizeof(trapVersion))
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseCreateNameTrap);
      }
      else if (ewsContext->commType == CLI_NO_CMD)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputUseNoCreateNameIpAddr);
      }
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    OSAPI_STRNCPY_SAFE(trapVersion, argv[index+argVer]);

    if(strcmp(trapVersion,pStrInfo_base_SnmpVer1)==0)
    {
      version = L7_SNMP_TRAP_VER_SNMPV1;
    }
    else if(strcmp(trapVersion,pStrInfo_base_SnmpVer2)==0)
    {
      version = L7_SNMP_TRAP_VER_SNMPV2C;
    }
    else
    {
      version = L7_SNMP_TRAP_VER_SNMPV2C;
    }
  }

  /*trapCommIndex = cliFindSNMPTrapCommunityIndex(commName, ipAddr, ipv6_address, ipAddrType);*/
  if(ipAddrType==4)
  {
    trapCommIndex = cliFindSNMPTrapCommunityIndex(commName, ipAddr);
  }
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  else
    trapCommIndex = cliFindSNMPTrapCommunityV6Index(commName, strIpAddr);
#endif


  if (trapCommIndex == CLI_INDEX_NOT_FOUND)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /* check if the snmp trap table is full */
      emptyTrapCommIndex = cliFindEmptySNMPTrapCommunityIndex();
      if (emptyTrapCommIndex == CLI_INDEX_NOT_FOUND)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SnmpTrapTblFull);
      }
      else
      {

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbTrapManagerStatusIpGet (unit, emptyTrapCommIndex, &currStatus);
          rc = usmDbTrapManagerStatusIpSet(unit, emptyTrapCommIndex, L7_SNMP_TRAP_MGR_STATUS_CONFIG);
          rc = usmDbTrapManagerCommIpSet(unit, emptyTrapCommIndex, commName);

          if(ipAddrType == 4)
          {
            if (usmDbTrapManagerIpAddrSet(unit, emptyTrapCommIndex, strIpAddr) != L7_SUCCESS)
            {
              if (currStatus == L7_SNMP_TRAP_MGR_STATUS_DELETE)
              {
                usmDbTrapManagerStatusIpSet(unit, emptyTrapCommIndex, L7_SNMP_TRAP_MGR_STATUS_DELETE);
              }
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_CfgNwIp);
            }
          }
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
          if(ipAddrType ==6)
          {
            if (usmDbTrapManagerIPv6AddrSet(unit, emptyTrapCommIndex, &ipv6_address) != L7_SUCCESS)
            {
              if (currStatus == L7_SNMP_TRAP_MGR_STATUS_DELETE)
              {
                usmDbTrapManagerStatusIpSet(unit, emptyTrapCommIndex, L7_SNMP_TRAP_MGR_STATUS_DELETE);
              }
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_CfgNwIp);
            }
          }
#endif

          rc = usmDbTrapManagerVersionSet(unit, emptyTrapCommIndex, version);
          rc = usmDbTrapManagerStatusIpSet(unit, emptyTrapCommIndex, L7_SNMP_TRAP_MGR_STATUS_VALID);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SnmpTrapAdded);
          cliSyntaxBottom(ewsContext);
        }
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SnmpTrapDoesntExist);
    }
  }
  else
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_SnmpTrapAlreadyExists);
        cliSyntaxBottom(ewsContext);
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if((strcmp(strIpAddr,"0.0.0.0") != 0 ) && (strcmp(strIpAddr,"::")!= 0))
        {
        rc = usmDbTrapManagerStatusIpSet(unit, trapCommIndex, L7_SNMP_COMMUNITY_STATUS_INVALID);
        if (rc !=L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_DelCommunity);
        }
        }
        rc = usmDbTrapManagerStatusIpSet(unit, trapCommIndex, L7_SNMP_TRAP_MGR_STATUS_DELETE);
        if (rc !=L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_DelCommunity);
        }
      }

    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 *
 * @purpose to add a new snmp community name if space permits
 *
 *
 * @param EwsContext ewsContext
 * @paam L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax snmp-server community <name>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   This name identifies each SNMP community; the name can be up to
 *   16 characters, and it is case-sensitive. A public community means
 *   users have read only access. A private community is for users who
 *   have read/write access. Two communities have default values. The
 *   default names are Public and Private. You can replace these
 *   default community names with unique identifiers for each
 *   community. The default values for the remaining four community
 *   names are blank.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSnmpServerCommunityName(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index )
{
  L7_uchar8 name[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 emptyCommIndex;
  L7_int32 commIndex;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 argName = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliNumFunctionArgsGet() != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrInfo_base_IncorrectInputUseCreateName);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrInfo_base_IncorrectInputUseNoCreateName);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (strlen(argv[index+argName]) >= sizeof(name))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrInfo_base_IncorrectInputUseCreateName);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrInfo_base_IncorrectInputUseNoCreateName);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  OSAPI_STRNCPY_SAFE(name, argv[index+argName]);

  /* check the length of the name to be 16 chars or less */
  if ( strlen(name) > L7_SNMP_NAME_SIZE )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_NameMustBe16OrFew);
  }

  /* if correct input, execute this section of code */
  commIndex = cliFindSNMPCommunityIndex(name);
  emptyCommIndex = cliFindEmptySNMPCommunityIndex();

  if (commIndex == CLI_INDEX_NOT_FOUND)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (emptyCommIndex == CLI_INDEX_NOT_FOUND)                /* check if table is not full */
      {
        /* else no more room  because the table is full*/
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CommunityTblFull);
      }
      else
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbSnmpCommunityStatusSet(unit, emptyCommIndex, L7_SNMP_COMMUNITY_STATUS_CONFIG);
          rc = usmDbSnmpCommunityNameSet(unit, emptyCommIndex, name);
          if (rc != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_base_CreateCommunity);
          }
          rc = usmDbSnmpCommunityStatusSet(unit, emptyCommIndex, L7_SNMP_COMMUNITY_STATUS_VALID);
        }
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_CommunityDoesntExist);
    }
  }
  else
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_CommunityAlreadyExists);
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSnmpCommunityStatusSet(unit, commIndex, L7_SNMP_COMMUNITY_STATUS_DELETE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_CantDelCommunity);
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Sets the port Max Frame size.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8
 *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @Mode  Interface Config
 *
 * @cmdsyntax for normal command: mtu <1522-max platform supports>
 *
 * @cmdsyntax for no command: no mtu
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMaxFrameSize(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMaxFrameSize  = 1;
  L7_uint32 interface, s, p;
  L7_int32 maxFrameSize = L7_MIN_FRAME_SIZE;
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop (ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 1, 0, pStrErr_common_IncorrectInput,ewsContext, pStrInfo_base_UseMtu,
        L7_MIN_FRAME_SIZE, L7_MAX_FRAME_SIZE);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (( cliConvertTo32BitUnsignedInteger(argv[index+argMaxFrameSize], &maxFrameSize) != L7_SUCCESS))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CfgMaxFrameSize);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if (usmDbIntfParmCanSet(interface, L7_INTF_PARM_FRAMESIZE) != L7_TRUE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          status = L7_FAILURE;
          continue;
        }

        if(usmDbIfConfigMaxFrameSizeSet(interface, maxFrameSize) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CfgMaxFrameSize);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  used for diffserv, Converts a log severity keyword string to the
 *           associated value, if one is available.  If the string holds
 *           an integer value, the integer value is returned.
 *
 * @param    dscpVal         value to return
 * @param    dscpString      string to parse
 *
 * @returns rc
 *
 * @notes none
 *
 * @end
 *********************************************************************/

L7_RC_t cliLogConvertSeverityLevelToVal(L7_char8 * severityLevelString, L7_uint32 * severityLevel)
{

  if (usmDbStringCaseInsensitiveCompare(severityLevelString, pStrInfo_base_LogSeverityEmergency) == L7_SUCCESS)
  {
    *severityLevel = L7_LOG_SEVERITY_EMERGENCY;
  }

  else if (usmDbStringCaseInsensitiveCompare(severityLevelString, pStrInfo_base_LogSeverityAlert) == L7_SUCCESS)
  {
    *severityLevel =  L7_LOG_SEVERITY_ALERT;
  }

  else if (usmDbStringCaseInsensitiveCompare(severityLevelString, pStrInfo_common_LogSeverityCritical) == L7_SUCCESS)
  {
    *severityLevel =  L7_LOG_SEVERITY_CRITICAL;
  }
  else if (usmDbStringCaseInsensitiveCompare(severityLevelString, pStrInfo_common_Error_5) == L7_SUCCESS)
  {
    *severityLevel = L7_LOG_SEVERITY_ERROR;
  }
  else if (usmDbStringCaseInsensitiveCompare(severityLevelString, pStrWarn_base_LogSeverityWarning) == L7_SUCCESS)
  {
    *severityLevel =  L7_LOG_SEVERITY_WARNING;
  }
  else if (usmDbStringCaseInsensitiveCompare(severityLevelString, pStrInfo_base_LogSeverityNotice) == L7_SUCCESS)
  {
    *severityLevel = L7_LOG_SEVERITY_NOTICE;
  }
  else if (usmDbStringCaseInsensitiveCompare(severityLevelString, pStrInfo_base_LogSeverityInfo) == L7_SUCCESS)
  {
    *severityLevel =  L7_LOG_SEVERITY_INFO;
  }
  else if (usmDbStringCaseInsensitiveCompare(severityLevelString, pStrInfo_common_LogSeverityDebug) == L7_SUCCESS)
  {
    *severityLevel = L7_LOG_SEVERITY_DEBUG;
  }
  else
  {
    /* verify if the specified  argument is an integer */
    if ( cliCheckIfInteger(severityLevelString) != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    /* check for overflow of integer value */
    if ( cliConvertTo32BitUnsignedInteger(severityLevelString, severityLevel) != L7_SUCCESS)
    {
      return L7_ERROR;
    }
    if(*severityLevel < L7_LOG_SEVERITY_EMERGENCY || *severityLevel > L7_LOG_SEVERITY_DEBUG)
    {
      return L7_ERROR;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  used for diffserv, Converts a log severity keyword string to the
 *           associated value, if one is available.  If the string holds
 *           an integer value, the integer value is returned.
 *
 * @param    dscpVal         value to return
 * @param    dscpString      string to parse
 *
 * @returns rc
 *
 * @notes none
 *
 * @end
 *********************************************************************/

L7_RC_t cliLogConvertFacilityToVal(L7_char8 * facilityString, L7_LOG_FACILITY_t * facility)
{

  if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityKernel) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_KERNEL;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_common_LogFacilityUsr) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_USER;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityMail) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_MAIL;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_BootSys) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_SYSTEM;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_common_LogFacilitySecurity) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_SECURITY;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilitySyslog) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_SYSLOG;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityLpr) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_LPR;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityNntp) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_NNTP;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityUucp) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_UUCP;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityCron) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_CRON;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_common_LogFacilityAuth ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_AUTH ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_common_Ftp ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_FTP ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityNtp ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_NTP ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityAudit ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_AUDIT ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogSeverityAlert ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_ALERT ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityClock ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_CLOCK ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityLocal ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_LOCAL0 ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityLocal_1 ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_LOCAL1 ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityLocal2 ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_LOCAL2 ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityLocal3 ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_LOCAL3 ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityLocal_2 ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_LOCAL4 ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityLocal_3 ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_LOCAL5 ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityLocal_4 ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_LOCAL6 ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_base_LogFacilityLocal_4 ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_LOCAL7 ;
  }

  else if (usmDbStringCaseInsensitiveCompare(facilityString, pStrInfo_common_All ) == L7_SUCCESS)
  {
    *facility = L7_LOG_FACILITY_ALL;
  }

  else
  {
    /* verify if the specified  argument is an integer */
    if ( cliCheckIfInteger(facilityString) != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    /* check for overflow of integer value */
    if ( cliConvertTo32BitUnsignedInteger(facilityString, facility) != L7_SUCCESS)
    {
      return L7_ERROR;
    }
    if(*facility < L7_LOG_FACILITY_KERNEL || *facility > L7_LOG_FACILITY_ALL)
    {
      return L7_ERROR;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging persistent [<severitylevel>]
 *         no logging persistent
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes persistent logging.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingPersistent(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSeverityLevel = 1;
  L7_uint32 numArg;
  L7_LOG_SEVERITY_t severityLevel;
  L7_char8 strSeverityLevel[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    numArg = cliNumFunctionArgsGet();
    if (numArg > 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingPersistent);
    }
    else if (numArg == 1)
    {
      OSAPI_STRNCPY_SAFE(strSeverityLevel,argv[index+ argSeverityLevel]);

      if ( cliLogConvertSeverityLevelToVal(strSeverityLevel, &severityLevel) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidSeverityVal);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogPersistentSeverityFilterSet(unit, severityLevel) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingEnbl);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogPersistentAdminStatusSet(unit, L7_ADMIN_MODE_ENABLE) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingSeveritySet);
        }
      }
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogPersistentAdminStatusSet(unit, L7_ADMIN_MODE_ENABLE) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingEnbl);
        }
      }
    }

  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingPersistentNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogPersistentAdminStatusSet(unit, L7_ADMIN_MODE_DISABLE) == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingDsbl);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging buffered [<severitylevel>]
 *         logging buffered component <componentid> <hidden>
 *         no logging buffered
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes in-memory (buffered) logging.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingBuffered(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSeverityLevel = 1;
  L7_uint32 numArg;
  L7_LOG_SEVERITY_t severityLevel;
  L7_char8 strSeverityLevel[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 argComponent = 2;
  L7_uint32 argComponentToken = 1;
  L7_COMPONENT_IDS_t componentId;
  L7_char8 componentMnemonic[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 componentToken[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  OSAPI_STRNCPY_SAFE(strMode,argv[argc-1]);
  cliConvertToLowerCase(strMode);

  if ( strcmp(strMode, pStrInfo_common_Lvl7Clr) != 0 )
  {
    if ( ewsContext->commType == CLI_NORMAL_CMD )
    {

      numArg = cliNumFunctionArgsGet();
      if ( numArg > 1 )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingBufed);
      }
      else if ( numArg == 1 )
      {
        OSAPI_STRNCPY_SAFE(strSeverityLevel,argv[index+ argSeverityLevel]);

        if ( cliLogConvertSeverityLevelToVal(strSeverityLevel, &severityLevel) != L7_SUCCESS )
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidSeverityVal);
        }

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ( usmDbLogInMemorySeverityFilterSet(unit, severityLevel) == L7_FAILURE )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingEnbl);
          }
        }

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ( usmDbLogInMemoryAdminStatusSet(unit, L7_ADMIN_MODE_ENABLE) == L7_FAILURE )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingSeveritySet);
          }
        }
      }
      else
      {

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ( usmDbLogInMemoryAdminStatusSet(unit, L7_ADMIN_MODE_ENABLE) == L7_FAILURE )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingEnbl);
          }
        }
      }

    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (cliNumFunctionArgsGet() != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingBufedNo);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogInMemoryAdminStatusSet(unit, L7_ADMIN_MODE_DISABLE) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingDsbl);
        }
      }
    }
  }
  else   /*Hidden command */
  {
    if ( ewsContext->commType == CLI_NORMAL_CMD )
    {
      numArg = cliNumFunctionArgsGet();
      numArg--; /*to take care last argument 'lvl7clear'*/

      if ( numArg == 0 )
      {
        /* put usmdb to clear logging */

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ( usmDbLogInMemoryLogClear(unit) != L7_SUCCESS )
          {
            return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_LoggingWithErrs);
          }
          else
          {
            ewsTelnetWrite( ewsContext, pStrErr_base_LoggingWithoutErrs);
            cliSyntaxBottom(ewsContext);

            /*************Set Flag for Script Successful******/
            ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

            return cliPrompt(ewsContext);
          }
        }
      }
      else if ( numArg == 2 )
      {
        /*check that the token is 'component'*/

        OSAPI_STRNCPY_SAFE(componentToken,argv[index+ argComponentToken]);

        if(strcmp(componentToken,pStrInfo_base_Comp_2) == 0)
        {
          OSAPI_STRNCPY_SAFE(componentMnemonic,argv[index+ argComponent]);

          if ( usmDbComponentIdFromMnemonicGet(componentMnemonic, &componentId) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidCompVal);
          }

          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if (usmDbLogInMemoryComponentFilterSet(unit, componentId) == L7_FAILURE)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingCompSet);
            }
          }

          /*************Set Flag for Script Successful******/
          ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

          return cliPrompt(ewsContext);
        }
      }

      /*if it gets here - syntax was not correct*/
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingBufed);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {

      numArg = cliNumFunctionArgsGet();
      numArg--; /*to take care last argument 'lvl7clear'*/
      if ( numArg == 1 )
      {
        OSAPI_STRNCPY_SAFE(componentToken,argv[index+ argComponentToken]);

        if(strcmp(componentToken,pStrInfo_base_Comp_2) == 0)
        {
          /*check that the token is 'component'*/

          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if (usmDbLogInMemoryComponentFilterSet(unit, L7_ALL_COMPONENTS) == L7_FAILURE)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingCompSet);
            }
          }

          /*************Set Flag for Script Successful******/
          ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

          return cliPrompt(ewsContext);

        }
      }

      /*if it gets here - syntax was not correct*/
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingBufedNo);

    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging buffered component <component>
 *         no logging buffered component
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes in-memory logging component.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingBufferedComponent(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argComponent = 1;
  L7_uint32  numArg;
  L7_COMPONENT_IDS_t componentId;
  L7_char8 componentMnemonic[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    numArg = cliNumFunctionArgsGet();
    if (numArg == 1)
    {
      OSAPI_STRNCPY_SAFE(componentMnemonic,argv[index+ argComponent]);

      if ( usmDbComponentIdFromMnemonicGet(componentMnemonic, &componentId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidCompVal);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogInMemoryComponentFilterSet(unit, componentId) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingCompSet);
        }
      }

    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingBufedComp);
    }

  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingBufedCompNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogInMemoryComponentFilterSet(unit, L7_ALL_COMPONENTS) == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingCompSet);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging syslog port <portid>
 *         no logging syslog port
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes logging port.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingSyslogPort(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argPort = 1;
  L7_uint32 numArg;
  L7_uint32 port;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    numArg = cliNumFunctionArgsGet();
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingSyslogPort);
    }
    else
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index + argPort], &port)!= L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogLocalPortSet (unit, (L7_ushort16) port) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingPortSet);
        }
      }

    }

  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingSyslogPortNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogLocalPortSet (unit, 514) == L7_FAILURE)
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FailedToSetPort);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging buffered wrap
 *         no logging buffered wrap
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes in-memory logging behavior.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingBufferedWrap(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    numArg = cliNumFunctionArgsGet();
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingBufedWrap);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogInMemoryBehaviorSet (unit, L7_LOG_WRAP_BEHAVIOR) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingBufedWrapSet);
        }
      }
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingBufedWrapNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogInMemoryBehaviorSet (unit, L7_LOG_STOP_ON_FULL_BEHAVIOR) == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingBufedWrapSetNo);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging syslog [<facility>]
 *         no logging syslog
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes syslog logging.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingSyslog(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argFacilityToken = 1;
  L7_uint32 argFacilityLevel = 2;
  L7_uint32 numArg;
  L7_LOG_FACILITY_t facilityLevel;
  L7_char8 strFacilityLevel[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strFacilityToken[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    OSAPI_STRNCPY_SAFE(strMode,argv[argc-1]);
    cliConvertToLowerCase(strMode);
    numArg = cliNumFunctionArgsGet();
    if (numArg == 3 && strcmp(strMode, pStrInfo_common_Lvl7Clr) == 0)
    {
      OSAPI_STRNCPY_SAFE(strFacilityToken,argv[index+ argFacilityToken]);

      if(strcmp(strFacilityToken, pStrInfo_base_Facility_1) == 0)
      {
        OSAPI_STRNCPY_SAFE(strFacilityLevel,argv[index+ argFacilityLevel]);
        if ( cliLogConvertFacilityToVal(strFacilityLevel, &facilityLevel) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidFacilityVal);
        }

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbLogSyslogDefaultFacilitySet(unit, facilityLevel) == L7_FAILURE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingFacilitySet);
          }
        }

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbLogSyslogAdminStatusSet(unit, L7_ADMIN_MODE_ENABLE) == L7_FAILURE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingSyslogEnbl);
          }
        }
      }
    }
    else if (numArg > 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGLOGGINGSYSLOG);
    }
    else
    {

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogSyslogAdminStatusSet(unit, L7_ADMIN_MODE_ENABLE) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingSyslogEnbl);
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    OSAPI_STRNCPY_SAFE(strMode,argv[argc-1]);
    cliConvertToLowerCase(strMode);
    numArg = cliNumFunctionArgsGet();

    if(strcmp(strMode, pStrInfo_common_Lvl7Clr) != 0)
    {
      if (numArg != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingSyslogNo);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogSyslogAdminStatusSet(unit, L7_ADMIN_MODE_DISABLE) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingSyslogDsbl);
        }
      }
    }
    else
    {
      if (numArg != 2)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingSyslogNo);
      }

      OSAPI_STRNCPY_SAFE(strFacilityToken,argv[index+ argFacilityToken]);

      if(strcmp(strFacilityToken, pStrInfo_base_Facility_1) == 0)
      {

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbLogSyslogDefaultFacilitySet(unit, FD_LOG_DEFAULT_LOG_FACILITY) == L7_FAILURE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingFacilitySet_1);
          }
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}




/*********************************************************************
 * @purpose  Configure email Alert log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging email 
 *         no logging email
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes Email Alerting.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlert(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_char8 strSeverity[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argMetric = 1;  
  L7_uint32 severity = FD_EMAIL_ALERT_DEFAULT_NON_URGENT_SEVERITY;  
	

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg != 0)&&(numArg != 1))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgEmailAlerting);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
	   if(numArg  == 0)
	   {
	      if (usmDbLogEmailAlertStatusSet(USMDB_UNIT_CURRENT,L7_ENABLE) != L7_SUCCESS)
	      {
	        cliSyntaxNewLine(ewsContext);
	        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                              ewsContext, pStrErr_base_emailAlertingEnableFail);
	      }
	   }
		if(numArg  == 1)
		{
		    OSAPI_STRNCPY_SAFE(strSeverity,argv[index+ argMetric]);
	        if ( cliLogConvertSeverityLevelToVal(strSeverity, &severity) != L7_SUCCESS)
	        {
	          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidSeverityVal);
	        }

			if (( severity >= L7_LOG_EMAILALERT_SEVERITY_MIN ) && ( severity <= L7_LOG_EMAILALERT_SEVERITY_MAX ))
			{
			  /*******Check if the Flag is Set for Execution*************/
			  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
			  {
				 L7_uint32 urgentSeverity;  
		        if (usmDbLogEmailAlertUrgentSeverityGet(USMDB_UNIT_CURRENT, &urgentSeverity)!= L7_SUCCESS )
		        {
		          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetEmailAlertSeverity );
		        }
				 if(severity < urgentSeverity)		
				 {
				    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_base_EmailAlertnonUrgentSeverityMustBeGreaterThanNonUrgentSeverity);
				 }

			      if (usmDbLogEmailAlertNonUrgentSeveritySet(USMDB_UNIT_CURRENT, severity ) != L7_SUCCESS )
			      {
			        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetEmailAlertSeverity );
			      }

			  }
			}
			else
			{
			    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_base_EmailAlertSeverityMustBeBetweenAnd ,L7_LOG_EMAILALERT_SEVERITY_MIN, L7_LOG_EMAILALERT_SEVERITY_MAX);			
			}
		}
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ((numArg != 0)&&(numArg != 1))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoEmailAlerting);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
	  if(numArg  == 0)
	  {
		   L7_uint32 urgentSeverity;  
			 
	      if (usmDbLogEmailAlertStatusSet(USMDB_UNIT_CURRENT,L7_DISABLE) != L7_SUCCESS)
	      {
	        cliSyntaxNewLine(ewsContext);
	        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                              ewsContext, pStrErr_base_emailAlertingDisableFail);
	      }

        if (usmDbLogEmailAlertUrgentSeverityGet(USMDB_UNIT_CURRENT, &urgentSeverity)!= L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetEmailAlertSeverity );
        }
		 if(FD_EMAIL_ALERT_DEFAULT_NON_URGENT_SEVERITY <= urgentSeverity)		
		 {
		    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_base_EmailAlertnonUrgentSeverityMustBeGreaterThanNonUrgentSeverity);
		 }

      if (usmDbLogEmailAlertNonUrgentSeveritySet(USMDB_UNIT_CURRENT, FD_EMAIL_ALERT_DEFAULT_NON_URGENT_SEVERITY) != L7_SUCCESS )
      {
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetEmailAlertSeverity );
      }
			
	  }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgEmailAlerting);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}


/*********************************************************************
 * @purpose  Configure to Address of the email
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging email msgtype <msgtype> toAddr <toAddr>
 *         no logging  email msgtype <msgtype> toAddr  <toAddr>

 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes toAddress
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlertToAddr(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_char8 toAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argindex = 1;  
  L7_uint32 msgType;	
  L7_uint32 tmpMsgType;		
  L7_char8 strMsgType[L7_CLI_MAX_STRING_LENGTH];		
  L7_LOG_EMAIL_ALERT_ERROR_t errorValue;			
		
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingToAddress);
    }

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoEmailAlertingToAddress);
    }

  }

   OSAPI_STRNCPY_SAFE(strMsgType,argv[index -1]);

	if(osapiStrCaseCmp(strMsgType, "urgent",strlen("urgent"))==0)
	{
		msgType= L7_LOG_EMAIL_ALERT_URGENT; 
	}
	else if(osapiStrCaseCmp(strMsgType, "non-urgent",strlen("non-urgent"))==0)
	{
		msgType= L7_LOG_EMAIL_ALERT_NON_URGENT; 
	}
	else if(osapiStrCaseCmp(strMsgType, "both",strlen("both"))==0)
	{
		msgType= L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES; 
	}
	else
	{
     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingInvalidMsgType);
	}

	if((  msgType != L7_LOG_EMAIL_ALERT_URGENT	) &&(msgType != L7_LOG_EMAIL_ALERT_NON_URGENT)&&(msgType != L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES))
	{
     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingInvalidMsgType);
	}
		
 OSAPI_STRNCPY_SAFE(toAddr,argv[index+argindex]);
	

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
	   if(msgType == L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES)
	   {
			for(tmpMsgType= L7_LOG_EMAIL_ALERT_URGENT	; tmpMsgType < L7_LOG_EMAIL_ALERT_MAX_MSG_TYPES; tmpMsgType++)
			{
		      if (usmDbLogEmailAlertToAddrSet(USMDB_UNIT_CURRENT,tmpMsgType,toAddr, &errorValue) != L7_SUCCESS)
		      {
		        cliSyntaxNewLine(ewsContext);

				if(errorValue == L7_LOG_EMAIL_ALERT_EMAIL_ADDRESS_INVALID)
				{
				 	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
				                                        ewsContext, "Invalid email address");
				}
				else if(errorValue == L7_LOG_EMAIL_ALERT_EMAIL_ADDRESS_INVALID_LENGTH)
				{
				 	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
				                                        ewsContext, "Email Address length is not Valid");

				}
				else if(errorValue == L7_LOG_EMAIL_ALERT_MAX_EMAIL_ADDRESSES_CONFIGURED)
				{
				 	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
				                                        ewsContext, "Maximum number of Email Addresses are configured!");
				}
				else
				{
				 	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
				                                        ewsContext, pStrErr_base_CfgEmailAlertingToAddressAddFail);
				}

		      }
			}

	   }
		else
		{
	      if (usmDbLogEmailAlertToAddrSet(USMDB_UNIT_CURRENT,msgType,toAddr, &errorValue) != L7_SUCCESS)
	      {
	        cliSyntaxNewLine(ewsContext);

			if(errorValue == L7_LOG_EMAIL_ALERT_EMAIL_ADDRESS_INVALID)
			{
			 	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
			                                        ewsContext, "Invalid email address");
			}
			else if(errorValue == L7_LOG_EMAIL_ALERT_EMAIL_ADDRESS_INVALID_LENGTH)
			{
			 	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
			                                        ewsContext, "Email Address length is not Valid");

			}
			else if(errorValue == L7_LOG_EMAIL_ALERT_MAX_EMAIL_ADDRESSES_CONFIGURED)
			{
			 	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
			                                        ewsContext, "Maximum number of Email Addresses are configured!");
			}
			else
			{
			 	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
			                                        ewsContext, pStrErr_base_CfgEmailAlertingToAddressAddFail);
			}

	      }
		}
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
	   if(msgType == L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES)
	   {
			for(tmpMsgType= L7_LOG_EMAIL_ALERT_URGENT	; tmpMsgType < L7_LOG_EMAIL_ALERT_MAX_MSG_TYPES; tmpMsgType++)
			{
			      if (usmDbLogEmailAlertToAddrRemove(USMDB_UNIT_CURRENT,tmpMsgType,toAddr) != L7_SUCCESS)
			      {
			        cliSyntaxNewLine(ewsContext);
			        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
			                                              ewsContext, pStrErr_base_CfgEmailAlertingToAddressRemoveFail);
			      }
			}

	   	}

	  else 
	  {
	      if (usmDbLogEmailAlertToAddrRemove(USMDB_UNIT_CURRENT,msgType,toAddr) != L7_SUCCESS)
	      {
	        cliSyntaxNewLine(ewsContext);
	        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                              ewsContext, pStrErr_base_CfgEmailAlertingToAddressRemoveFail);
	      }
	  }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgEmailAlertingToAddress);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  Configure the subject of the email
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging email msgtype <msgtype> subject  <toAddr>
 *         no logging email msgtype <msgtype> subject  <toAddr>
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes toAddress
 *
 * @end
 *********************************************************************/

const L7_char8 *commandLoggingEmailAlertSubject(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_int32 tempIndex;
  L7_char8 ch;
  L7_uint32 numArg = 0;        
  L7_char8 subject[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argindex = 1;  
  L7_uint32 msgType;	
  L7_uint32 tmpMsgType;		
  L7_char8 strMsgType[L7_CLI_MAX_STRING_LENGTH];		
	
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingSubject);
    }

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoEmailAlertingSubject);
    }
  }

   OSAPI_STRNCPY_SAFE(strMsgType,argv[index -1]);

	if(osapiStrCaseCmp(strMsgType, "urgent",strlen("urgent"))==0)
	{
		msgType= L7_LOG_EMAIL_ALERT_URGENT; 
	}
	else if(osapiStrCaseCmp(strMsgType, "non-urgent",strlen("non-urgent"))==0)
	{
		msgType= L7_LOG_EMAIL_ALERT_NON_URGENT; 
	}
	else if(osapiStrCaseCmp(strMsgType, "both",strlen("both"))==0)
	{
		msgType= L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES; 
	}
	else
	{
     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingInvalidMsgType);
	}

	if((  msgType != L7_LOG_EMAIL_ALERT_URGENT	) &&(msgType != L7_LOG_EMAIL_ALERT_NON_URGENT)&&(msgType != L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES))
	{
     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingInvalidMsgType);
	}
		
    OSAPI_STRNCPY_SAFE(subject,argv[index+argindex]);

	

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
	   for(tempIndex=0; tempIndex < strlen(subject);tempIndex++)
	   {
		   ch = subject[tempIndex];
			if((isalnum(ch)==0) &&(ch != ' ') &&(ch != '-')&&(ch != '_')&&(ch != '.')&&(ch != '!'))
		 	{
		        cliSyntaxNewLine(ewsContext);
		        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CfgEmailAlertingSubjectInvalid);
		 	}
	   }

	   if(msgType == L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES)
	   {
			for(tmpMsgType= L7_LOG_EMAIL_ALERT_URGENT	; tmpMsgType < L7_LOG_EMAIL_ALERT_MAX_MSG_TYPES; tmpMsgType++)
			{

		      if (usmDbLogEmailAlertSubjectSet(USMDB_UNIT_CURRENT,tmpMsgType,subject) != L7_SUCCESS)
		      {
		        cliSyntaxNewLine(ewsContext);
		        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
		                                              ewsContext, pStrErr_base_CfgEmailAlertingSubjectAddFail);
		      }

			}

	   	}
		else 
		{
	      if (usmDbLogEmailAlertSubjectSet(USMDB_UNIT_CURRENT,msgType,subject) != L7_SUCCESS)
	      {
	        cliSyntaxNewLine(ewsContext);
	        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                              ewsContext, pStrErr_base_CfgEmailAlertingSubjectAddFail);
	      }
		}
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
	   if(msgType == L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES)
	   {
			for(tmpMsgType= L7_LOG_EMAIL_ALERT_URGENT	; tmpMsgType < L7_LOG_EMAIL_ALERT_MAX_MSG_TYPES; tmpMsgType++)
			{
				  if(tmpMsgType == L7_LOG_EMAIL_ALERT_URGENT)	
				  {
				      if (usmDbLogEmailAlertSubjectSet(USMDB_UNIT_CURRENT,tmpMsgType,"Urgent Log Messages") != L7_SUCCESS)
				      {
				        cliSyntaxNewLine(ewsContext);
				        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
				                                              ewsContext, pStrErr_base_CfgEmailAlertingSubjectRemoveFail);
				      }
				  }
				  else if(tmpMsgType == L7_LOG_EMAIL_ALERT_NON_URGENT)	
				  {
				      if (usmDbLogEmailAlertSubjectSet(USMDB_UNIT_CURRENT,tmpMsgType,"Non Urgent Log Messages") != L7_SUCCESS)
				      {
				        cliSyntaxNewLine(ewsContext);
				        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
				                                              ewsContext, pStrErr_base_CfgEmailAlertingSubjectRemoveFail);
				      }
				  }
			}
	   	}

			
	  if(msgType == L7_LOG_EMAIL_ALERT_URGENT)	
	  {
	      if (usmDbLogEmailAlertSubjectSet(USMDB_UNIT_CURRENT,msgType,"Urgent Log Messages") != L7_SUCCESS)
	      {
	        cliSyntaxNewLine(ewsContext);
	        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                              ewsContext, pStrErr_base_CfgEmailAlertingSubjectRemoveFail);
	      }
	  }
	  else if(msgType == L7_LOG_EMAIL_ALERT_NON_URGENT)	
	  	{
	      if (usmDbLogEmailAlertSubjectSet(USMDB_UNIT_CURRENT,msgType,"Non Urgent Log Messages") != L7_SUCCESS)
	      {
	        cliSyntaxNewLine(ewsContext);
	        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                              ewsContext, pStrErr_base_CfgEmailAlertingSubjectRemoveFail);
	      }
	  	}
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgEmailAlertingSubject);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  Configure from Address of the email
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging email fromAddr  <fromAddr>
 *         no logging  email fromAddr
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes fromAddress
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlertFromAddr(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_char8 fromAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argMetric = 1;  
  L7_LOG_EMAIL_ALERT_ERROR_t errorValue;		
	
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingFromAddress);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

      OSAPI_STRNCPY_SAFE(fromAddr,argv[index+ argMetric]);


      if (usmDbLogEmailAlertFromAddrSet(USMDB_UNIT_CURRENT,fromAddr,&errorValue) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);

		if(errorValue == L7_LOG_EMAIL_ALERT_EMAIL_ADDRESS_INVALID)
		{
       	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, "Invalid email address");
		}
		else if(errorValue == L7_LOG_EMAIL_ALERT_EMAIL_ADDRESS_INVALID_LENGTH)
		{
       	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, "Email Address length is not Valid");

		}
		else
		{
       	 return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_CfgEmailAlertingFromAddressAddFail);

		}
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoEmailAlertingFromAddress);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogEmailAlertFromAddrSet(USMDB_UNIT_CURRENT,FD_EMAIL_ALERT_DEFAULT_FROM_ADDRESS,L7_NULL) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_CfgEmailAlertingFromAddressRemoveFail);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgEmailAlertingFromAddress);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  Configure SMTP server port
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         mail-server port  <port>

 *         no mail-server port  
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes SMTP server port
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlertSMTPServerPort(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_uint32  smtpPort = 0;        
  L7_uint32 argMetric = 1;  
  L7_char8  mailServerAddr[L7_EMAIL_ALERT_SERVER_ADDRESS_SIZE];
  L7_inet_addr_t serverAddr;	
  L7_uint32 ipAddr = L7_NULL;	
  L7_RC_t 	rc=L7_FAILURE;
  L7_IP_ADDRESS_TYPE_t   type = L7_IP_ADDRESS_TYPE_UNKNOWN;	
	
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get the current Mail Server Address */
  OSAPI_STRNCPY_SAFE(mailServerAddr, EWSMAILSERVERHOST(ewsContext));
  /* Validate & Get the address type for Host Address */


  rc = usmDbIPHostAddressValidate(mailServerAddr,&ipAddr,&type);
  if(rc == L7_ERROR)
  {
    /* Invalid Ip Address */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
			  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }
  else if( rc == L7_FAILURE)
  {
    /* It can be either a invalid host name or can be a IPV6 address */
    /* Convert the IP Address to Inet Address */
    if(usmDbIPv6HostAddressValidate(mailServerAddr, &serverAddr,&type) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,
  	    ewsContext, pStrErr_common_MailServerInValIp);
    }
    if( (type != L7_IP_ADDRESS_TYPE_IPV6)&&(type != L7_IP_ADDRESS_TYPE_DNS))
    {
        /* Invalid Host Name*/
        cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
  		    ewsContext, pStrErr_base_InvalidHostName);
    }
  }

 

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingMailServerPort);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

	   if (cliConvertTo32BitUnsignedInteger(argv[index + argMetric], &smtpPort) != L7_SUCCESS)
	   {
	     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                            ewsContext, pStrErr_base_CfgEmailAlertingInvalidPort);
	   }


      if (usmDbLogEmailAlertMailServerPortSet(USMDB_UNIT_CURRENT,mailServerAddr,smtpPort) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_CfgEmailAlertingMailServerPortAddFail);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoEmailAlertingMailServerPort);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
		L7_uint32 securityProtocol;
		L7_uint32 port;	

	    if(usmDbLogEmailAlertMailServerSecurityGet(USMDB_UNIT_CURRENT, mailServerAddr,&securityProtocol) != L7_FAILURE)
	    {
			if(securityProtocol == L7_LOG_EMAIL_ALERT_TLSV1)
			{
				port = L7_EMAIL_ALERT_SMTP_TLS_PORT; 
			}
			else
			{
				port = L7_EMAIL_ALERT_SMTP_NORMAL_PORT; 
			}

			if (usmDbLogEmailAlertMailServerPortSet(USMDB_UNIT_CURRENT,mailServerAddr,port) != L7_SUCCESS)
			{
			  cliSyntaxNewLine(ewsContext);
			  return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
			                                        ewsContext, pStrErr_base_CfgEmailAlertingMailServerPortRemoveFail);
			}

	    }
    
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgEmailAlertingMailServerPort);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  Configure SMTP server security
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         mail-server security  <tlsv1|none>
 *         no mail-server security
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes SMTP security protocol
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlertSMTPServerSecurity(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_uchar8 securityProtocolString[L7_CLI_MAX_SECURITY_PROTOCOL_STRING];	
  L7_uint32 argMetric = 1;  
  L7_char8  mailServerAddr[L7_EMAIL_ALERT_SERVER_ADDRESS_SIZE];
  L7_inet_addr_t serverAddr;	
  L7_uint32 ipAddr = L7_NULL;	
  L7_RC_t 	rc=L7_FAILURE;
  L7_IP_ADDRESS_TYPE_t   type = L7_IP_ADDRESS_TYPE_UNKNOWN;	
	
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get the current Mail Server Address */
  OSAPI_STRNCPY_SAFE(mailServerAddr, EWSMAILSERVERHOST(ewsContext));
  /* Validate & Get the address type for Host Address */
  rc = usmDbIPHostAddressValidate(mailServerAddr,&ipAddr,&type);
  if(rc == L7_ERROR)
  {
    /* Invalid Ip Address */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
			  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }
  else if( rc == L7_FAILURE)
  {
    /* It can be either a invalid host name or can be a IPV6 address */
    /* Convert the IP Address to Inet Address */
    if(usmDbIPv6HostAddressValidate(mailServerAddr, &serverAddr,&type) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,
  	    ewsContext, pStrErr_common_MailServerInValIp);
    }
    if( (type != L7_IP_ADDRESS_TYPE_IPV6)&&(type != L7_IP_ADDRESS_TYPE_DNS))
    {
        /* Invalid Host Name*/
        cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
  		    ewsContext, pStrErr_base_InvalidHostName);
    }
  }
	

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurity);
    }
		

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {


      OSAPI_STRNCPY_SAFE(securityProtocolString,argv[index+ argMetric]);		 

		if(osapiStrCaseCmp(securityProtocolString, "tlsv1",strlen("tlsv1"))==0)
		{
	      if (usmDbLogEmailAlertMailServerSecuritySet(USMDB_UNIT_CURRENT, mailServerAddr,L7_LOG_EMAIL_ALERT_TLSV1) != L7_SUCCESS)
	      {
	        cliSyntaxNewLine(ewsContext);
	        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                              ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityAddFail);
	      }
		
		}

		else if (osapiStrCaseCmp(securityProtocolString, "none",strlen("none"))==0)
		{
	      if (usmDbLogEmailAlertMailServerSecuritySet(USMDB_UNIT_CURRENT, mailServerAddr,L7_LOG_EMAIL_ALERT_NONE) != L7_SUCCESS)
	      {
	        cliSyntaxNewLine(ewsContext);
	        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                              ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityAddFail);
	      }
		}
		else 
		{
	        cliSyntaxNewLine(ewsContext);
	        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                              ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurity);

		}
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoEmailAlertingMailServerSecurity);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogEmailAlertMailServerSecuritySet(USMDB_UNIT_CURRENT,mailServerAddr,
																	FD_LOG_EMAILALERT_DEFAULT_SECURITY_PROTOCOL) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityRemoveFail);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurity);
  }

	/*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  Configure username required for authentication
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         mail-server username <username>
 *         no mail-server username 
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configure/remove username required for authentication
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlertusername(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_char8 userid[L7_EMAIL_ALERT_USERNAME_STRING_SIZE];
  L7_uint32 argMetric = 1;  
  L7_char8  mailServerAddr[L7_EMAIL_ALERT_SERVER_ADDRESS_SIZE];
  L7_inet_addr_t serverAddr;		
  L7_uint32 ipAddr = L7_NULL;	
  L7_RC_t 	rc=L7_FAILURE;
  L7_IP_ADDRESS_TYPE_t   type = L7_IP_ADDRESS_TYPE_UNKNOWN;	
	
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get the current Mail Server Address */
  OSAPI_STRNCPY_SAFE(mailServerAddr, EWSMAILSERVERHOST(ewsContext));
  /* Validate & Get the address type for Host Address */

  rc = usmDbIPHostAddressValidate(mailServerAddr,&ipAddr,&type);
  if(rc == L7_ERROR)
  {
    /* Invalid Ip Address */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
			  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }
  else if( rc == L7_FAILURE)
  {
    /* It can be either a invalid host name or can be a IPV6 address */
    /* Convert the IP Address to Inet Address */
    if(usmDbIPv6HostAddressValidate(mailServerAddr, &serverAddr,&type) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,
  	    ewsContext, pStrErr_common_MailServerInValIp);
    }
    if( (type != L7_IP_ADDRESS_TYPE_IPV6)&&(type != L7_IP_ADDRESS_TYPE_DNS))
    {
        /* Invalid Host Name*/
        cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
  		    ewsContext, pStrErr_base_InvalidHostName);
    }
  }
 

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityUsername );
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

      OSAPI_STRNCPY_SAFE(userid,argv[index+ argMetric]);
  
      if (usmDbLogEmailAlertMailServerUsernameSet(USMDB_UNIT_CURRENT,mailServerAddr,userid) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityUsernameAddFail);
      }
      
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoEmailAlertingMailServerSecurityUsername );
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogEmailAlertMailServerUsernameSet(USMDB_UNIT_CURRENT,mailServerAddr,FD_EMAIL_ALERT_SMTP_DEFAULT_USERID) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityUsernameRemoveFail );
      }
      
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityUsername );
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  Configure password required for authentication
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         mail-server password <password>
 *         no mail-server password
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configure/Remove password required for authentication
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlertpassword(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_char8 passwd[L7_EMAIL_ALERT_PASSWD_STRING_SIZE];
  L7_uint32 argMetric = 1;  
  L7_char8  mailServerAddr[L7_EMAIL_ALERT_SERVER_ADDRESS_SIZE];
  L7_inet_addr_t serverAddr;			
  L7_uint32 ipAddr = L7_NULL;	
  L7_RC_t 	rc=L7_FAILURE;
  L7_IP_ADDRESS_TYPE_t   type = L7_IP_ADDRESS_TYPE_UNKNOWN;	
	
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();


  /* get the current Mail Server Address */
  OSAPI_STRNCPY_SAFE(mailServerAddr, EWSMAILSERVERHOST(ewsContext));

  rc = usmDbIPHostAddressValidate(mailServerAddr,&ipAddr,&type);
  if(rc == L7_ERROR)
  {
    /* Invalid Ip Address */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
			  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }
  else if( rc == L7_FAILURE)
  {
    /* It can be either a invalid host name or can be a IPV6 address */
    /* Convert the IP Address to Inet Address */
    if(usmDbIPv6HostAddressValidate(mailServerAddr, &serverAddr,&type) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,
  	    ewsContext, pStrErr_common_MailServerInValIp);
    }
    if( (type != L7_IP_ADDRESS_TYPE_IPV6)&&(type != L7_IP_ADDRESS_TYPE_DNS))
    {
        /* Invalid Host Name*/
        cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
  		    ewsContext, pStrErr_base_InvalidHostName);
    }
  }
 

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityPassword);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

      OSAPI_STRNCPY_SAFE(passwd,argv[index+ argMetric]);

      if (usmDbLogEmailAlertMailServerPasswdSet(USMDB_UNIT_CURRENT,mailServerAddr,passwd) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityPasswordAddFail);
      }
      
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoEmailAlertingMailServerSecurityPassword);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

      if (usmDbLogEmailAlertMailServerPasswdSet(USMDB_UNIT_CURRENT,mailServerAddr,FD_EMAIL_ALERT_SMTP_DEFAULT_PASSWD) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityPasswordRemoveFail);
      }
      
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgEmailAlertingMailServerSecurityPassword);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  Test email Alert functionality
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging email test msgtype <msgtype> msgbody <msgbody>
 *         
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Tests email Alert functionality
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlertTest(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_char8 logbuffer[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argindex = 2;  
  L7_uint32 msgType;	
  L7_char8 strMsgType[L7_CLI_MAX_STRING_LENGTH];			

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 4)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgEmailAlertingTest);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

	   OSAPI_STRNCPY_SAFE(strMsgType,argv[index +argindex]);

		if(osapiStrCaseCmp(strMsgType, "urgent",strlen("urgent"))==0)
		{
			msgType= L7_LOG_EMAIL_ALERT_URGENT; 
		}
		else if(osapiStrCaseCmp(strMsgType, "non-urgent",strlen("non-urgent"))==0)
		{
			msgType= L7_LOG_EMAIL_ALERT_NON_URGENT; 
		}
		else if(osapiStrCaseCmp(strMsgType, "both",strlen("both"))==0)
		{
			msgType= L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES; 
		}
		else
		{
	     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                            ewsContext, pStrErr_base_CfgEmailAlertingInvalidMsgType);
		}

		if((  msgType != L7_LOG_EMAIL_ALERT_URGENT	) &&(msgType != L7_LOG_EMAIL_ALERT_NON_URGENT)&&
																							(msgType != L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES))
		{
	     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
	                                            ewsContext, pStrErr_base_CfgEmailAlertingInvalidMsgType);

		}
		 

      OSAPI_STRNCPY_SAFE(logbuffer,argv[index+ argindex+2]);

	   if(strlen(argv[index+ argindex+2])  < L7_CLI_MAX_STRING_LENGTH)	
	   	{
			logbuffer[strlen(argv[index+ argindex+2])]= '\0';
	   	}
		 else
		 {
	   		logbuffer[L7_CLI_MAX_STRING_LENGTH -1] = '\0';
		 }

		if(msgType ==L7_LOG_EMAIL_ALERT_NON_URGENT )
		{
    		L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_CLI_WEB_COMPONENT_ID,"%s",
           				 logbuffer);
		}
		if(msgType == L7_LOG_EMAIL_ALERT_URGENT)
		{
    		L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_CLI_WEB_COMPONENT_ID,"%s",
           				 logbuffer);
		}
		if(msgType == L7_LOG_EMAIL_ALERT_BOTH_MSGTYPES)
		{
    		L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_CLI_WEB_COMPONENT_ID,"%s",
           				 logbuffer);
    		L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_CLI_WEB_COMPONENT_ID,"%s",
           				 logbuffer);
		}


    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  Configure Email Alert Log duration
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging email logtime  <logtime>
 *         no logging email logtime 
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures Email alert Log Duration
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlertLogDuration(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_char8 strDuration[L7_CLI_MAX_STRING_DURATION];
  L7_uint32 argMetric = 1;  
  L7_int32 duration = FD_LOG_EMAILALERT_DEFAULT_DURATION;  
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();


  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArg != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
																					ewsContext, pStrErr_base_CfgEmailAlertingLogDuration);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArg != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
																					ewsContext, pStrErr_base_CfgNoEmailAlertingLogDuration);
  }

  if(ewsContext->commType == CLI_NO_CMD)
  {
    duration = FD_LOG_EMAILALERT_DEFAULT_DURATION;
  }
  else if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    OSAPI_STRNCPY_SAFE(strDuration,argv[index+ argMetric]);
    if ( cliCheckIfInteger(strDuration) == L7_SUCCESS)
    {
      (void)cliConvertTo32BitSignedInteger(strDuration, &duration);
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_base_CfgEmailAlertingLogDurationInvalid);
    }

  }

  if (( duration >= L7_LOG_EMAILALERT_DURATION_IN_MINUTES_MIN ) && ( duration <= L7_LOG_EMAILALERT_DURATION_IN_MINUTES_MAX))
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        if (usmDbLogEmailAlerLogTimeSet(USMDB_UNIT_CURRENT, (L7_uint32) duration ) != L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CfgEmailAlertingLogDurationSetFail);
        }

    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_base_EmailAlertLogDurationMustBeBetweenAnd ,
																					L7_LOG_EMAILALERT_DURATION_IN_MINUTES_MIN, L7_LOG_EMAILALERT_DURATION_IN_MINUTES_MAX);
  }


  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  Configure Email Alert urgent  severity
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *          logging email CriticalSeverity  <severity>
 *			  no 	logging email CriticalSeverity 

 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configure Email Alert urgent  severity
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlertUrgentSeverity(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_char8 strSeverity[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argMetric = 1;  
  L7_uint32 severity = FD_LOG_EMAILALERT_DEFAULT_SEVERITY;  
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();


  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArg != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgEmailAlertingCriticalSeverity);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArg != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoEmailAlertingCriticalSeverity);
  }

  if(ewsContext->commType == CLI_NO_CMD)
  {
    severity = FD_EMAIL_ALERT_DEFAULT_URGENT_SEVERITY;
  }
  else if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    OSAPI_STRNCPY_SAFE(strSeverity,argv[index+ argMetric]);
	 if(osapiStrCaseCmp(strSeverity, "none",strlen("none")) == 0)
	 {
		severity = -1;
	 }
    else if ( cliLogConvertSeverityLevelToVal(strSeverity, &severity) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidSeverityVal);
    }

  }

  if (( severity >= L7_LOG_EMAILALERT_SEVERITY_MIN ) && ( severity <= L7_LOG_EMAILALERT_SEVERITY_MAX ))
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
		 L7_uint32 nonurgentSeverity;  
        if (usmDbLogEmailAlertNonUrgentSeverityGet(USMDB_UNIT_CURRENT, &nonurgentSeverity)!= L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetEmailAlertSeverity );
        }
		 if(severity > nonurgentSeverity)		
		 {
		    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_base_EmailAlertUrgentSeverityMustBeLessThanNonUrgentSeverity);
		 }
        if (usmDbLogEmailAlertUrgentSeveritySet(USMDB_UNIT_CURRENT, severity ) != L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetEmailAlertSeverity );
        }

    }
  }
  else if(severity == -1)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        if (usmDbLogEmailAlertUrgentSeveritySet(USMDB_UNIT_CURRENT, severity ) != L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetEmailAlertSeverity );
        }

    }

  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_base_EmailAlertSeverityMustBeBetweenAnd ,L7_LOG_EMAILALERT_SEVERITY_MIN, L7_LOG_EMAILALERT_SEVERITY_MAX);
  }


  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);




}

/*********************************************************************
 * @purpose  Configure Email Alert Trap  severity
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *          logging email CriticalSeverity  <severity>
 *			  no 	logging email CriticalSeverity 

 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configure Email Alert urgent  severity
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingEmailAlertTrapSeverity(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        
  L7_char8 strSeverity[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argMetric = 1;  
  L7_uint32 severity = FD_LOG_EMAILALERT_DEFAULT_SEVERITY;  
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();


  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArg != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgEmailAlertingTrapSeverity);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArg != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoEmailAlertingTrapSeverity);
  }

  if(ewsContext->commType == CLI_NO_CMD)
  {
    severity = FD_EMAIL_ALERT_DEFAULT_TRAP_SEVERITY;
  }
  else if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    OSAPI_STRNCPY_SAFE(strSeverity,argv[index+ argMetric]);
    if ( cliLogConvertSeverityLevelToVal(strSeverity, &severity) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidSeverityVal);
    }
  }

  if (( severity >= L7_LOG_EMAILALERT_SEVERITY_MIN ) && ( severity <= L7_LOG_EMAILALERT_SEVERITY_MAX ))
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
        if (usmDbLogEmailAlertTrapSeveritySet(USMDB_UNIT_CURRENT, severity ) != L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetEmailAlertSeverity );
        }

    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_base_EmailAlertSeverityMustBeBetweenAnd ,L7_LOG_EMAILALERT_SEVERITY_MIN, L7_LOG_EMAILALERT_SEVERITY_MAX);
  }


  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging console [<severitylevel>]
 *         no logging console
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes console logging.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingConsole(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 argSeverityLevel = 1;
  L7_uint32 numArg;
  L7_LOG_SEVERITY_t severityLevel;
  L7_char8 strSeverityLevel[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 argComponent = 2;
  L7_uint32 argComponentToken = 1;
  L7_COMPONENT_IDS_t componentId;
  L7_char8 componentMnemonic[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 componentToken[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  OSAPI_STRNCPY_SAFE(strMode,argv[argc-1]);
  cliConvertToLowerCase(strMode);

  if ( strcmp(strMode, pStrInfo_common_Lvl7Clr) != 0 )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      numArg = cliNumFunctionArgsGet();
      if (numArg > 1)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingConsole);
      }
      else if (numArg == 1)
      {
        OSAPI_STRNCPY_SAFE(strSeverityLevel,argv[index+ argSeverityLevel]);

        if ( cliLogConvertSeverityLevelToVal(strSeverityLevel, &severityLevel) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidSeverityVal);
        }

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbLogConsoleSeverityFilterSet(unit, severityLevel) == L7_FAILURE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingEnbl);
          }
        }

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbLogConsoleAdminStatusSet(unit, L7_ADMIN_MODE_ENABLE) == L7_FAILURE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingSeveritySet);
          }
        }
      }
      else
      {

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbLogConsoleAdminStatusSet(unit, L7_ADMIN_MODE_ENABLE) == L7_FAILURE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingEnbl);
          }
        }
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (cliNumFunctionArgsGet() != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingConsoleNo);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogConsoleAdminStatusSet(unit, L7_ADMIN_MODE_DISABLE) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingDsbl);
        }
      }
    }
  }
  else
  {
    if ( ewsContext->commType == CLI_NORMAL_CMD )
    {
      numArg = cliNumFunctionArgsGet();
      numArg--; /*to take care last argument 'lvl7clear'*/

      if ( numArg == 2 )
      {
        /*check that the token is 'component'*/

        OSAPI_STRNCPY_SAFE(componentToken,argv[index+ argComponentToken]);

        if(strcmp(componentToken,pStrInfo_base_Comp_2) == 0)
        {
          OSAPI_STRNCPY_SAFE(componentMnemonic,argv[index+ argComponent]);

          if ( usmDbComponentIdFromMnemonicGet(componentMnemonic, &componentId) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidCompVal);
          }

          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if (usmDbLogConsoleComponentFilterSet(unit, componentId) == L7_FAILURE)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingCompSet);
            }
          }

          /*************Set Flag for Script Successful******/
          ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

          return cliPrompt(ewsContext);

        }
      }

      /*if it gets here - syntax was not correct*/
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingConsole);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {

      numArg = cliNumFunctionArgsGet();
      numArg--; /*to take care last argument 'lvl7clear'*/
      if ( numArg == 1 )
      {
        OSAPI_STRNCPY_SAFE(componentToken,argv[index+ argComponentToken]);

        if(strcmp(componentToken,pStrInfo_base_Comp_2) == 0)
        {
          /*check that the token is 'component'*/

          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if (usmDbLogConsoleComponentFilterSet(unit, L7_ALL_COMPONENTS) == L7_FAILURE)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingCompSet);
            }
          }

          /*************Set Flag for Script Successful******/
          ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

          return cliPrompt(ewsContext);

        }
      }

      /*if it gets here - syntax was not correct*/
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingConsoleNo);

    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging console component <component>
 *         no logging console component
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures or removes console logging component.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingConsoleComponent(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argComponent = 1;
  L7_uint32  numArg;
  L7_COMPONENT_IDS_t componentId;
  L7_char8 componentMnemonic[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    numArg = cliNumFunctionArgsGet();
    if (numArg == 1)
    {
      OSAPI_STRNCPY_SAFE(componentMnemonic,argv[index+ argComponent]);

      if ( usmDbComponentIdFromMnemonicGet(componentMnemonic, &componentId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidCompVal);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogConsoleComponentFilterSet(unit, componentId) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingCompSet);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingConsoleComp);
    }

  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingConsoleCompNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogConsoleComponentFilterSet(unit, L7_ALL_COMPONENTS) == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingCompSet);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging host <hostaddress|hostname> <addrtype> [<port>]
 *                      [<severitylevel>][<component>]
 *         no logging host
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures logging hosts.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingHost(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg, maxNum;
  L7_LOG_SEVERITY_t severityLevel;
  L7_RC_t   rc;
  L7_char8 strComponent[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSeverityLevel[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strAddressType[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 argHost = 1;
  L7_uint32 ipAddr = L7_NULL;
  L7_char8  strHost[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  address[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 unit;
  L7_uint32 rowIndex, status, logIndex;
  L7_uint32 port;
  L7_IP_ADDRESS_TYPE_t addressType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_COMPONENT_IDS_t component;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg < 2)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGLOGGINGHOST);
  }
  else
  {

    /* ip address */
    if (strlen(argv[index+argHost]) >= sizeof(strHost))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
    }
    OSAPI_STRNCPY_SAFE(strHost, argv[index + argHost]);
    if (cliIPHostAddressValidate(ewsContext, strHost, &ipAddr, &addressType) != L7_SUCCESS)
    {
      /* Invalid Host Address*/
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
    }
    memset(strAddressType, 0, sizeof(strAddressType));
    OSAPI_STRNCPY_SAFE(strAddressType, argv[index + 2]);
    if (usmDbStringCaseInsensitiveCompare(strAddressType,
                                  pStrInfo_common_Diffserv_4) == L7_SUCCESS)
    {
      addressType = L7_IP_ADDRESS_TYPE_IPV4;
    }
    else if (usmDbStringCaseInsensitiveCompare(strAddressType,
                                         pStrInfo_base_Dns_1) == L7_SUCCESS)
    {
       addressType = L7_IP_ADDRESS_TYPE_DNS;
    }
    else
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGLOGGINGHOST);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (numArg >= 3)
    {
      /* port */
      if (cliConvertTo32BitUnsignedInteger(argv[index+3], &port)!= L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }

      if (numArg >= 4)
      {
        /* Severity */
        OSAPI_STRNCPY_SAFE(strSeverityLevel,argv[index+ 4]);

        if (cliLogConvertSeverityLevelToVal(strSeverityLevel, &severityLevel) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidSeverityVal);
        }

        if (numArg >= 5)
        {
          /* Component */
          OSAPI_STRNCPY_SAFE(strComponent,argv[index+ 5]);

          if (usmDbComponentIdFromMnemonicGet(strComponent, &component) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidCompVal);
          }
        }
      }
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

      rc = usmDbLogHostAdd (unit, strHost, addressType, &rowIndex);

      if (rc == L7_ERROR)
      {
        logIndex = 0;

        while (usmDbLogHostTableRowStatusGetNext(unit, logIndex, &status, &rowIndex) == L7_SUCCESS)
        {
          memset(address, 0, sizeof(address));
          rc = usmDbLogHostTableIpAddressGet(unit, rowIndex, address);

          if (logIndex == rowIndex || (strcmp(strHost, address) == 0))
          {
            break;
          }
          else
          {
            logIndex = rowIndex;
          }
        }

        if (logIndex == rowIndex)
        {
          rc = L7_FAILURE;
        }
      }

      if (rc == L7_SUCCESS)
      {
        if (numArg >= 3)
        {

          rc = usmDbLogHostTablePortSet(unit, rowIndex, port);

          if (rc == L7_FAILURE)
          {
            return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FailedToSetPort);
          }

          if (numArg >= 4)
          {
            rc = usmDbLogHostTableSeveritySet(unit, rowIndex, severityLevel);
            if (rc == L7_FAILURE)
            {
              return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FailedToSetSeverity);
            }

            if (numArg >= 5)
            {
              rc = usmDbLogHostTableComponentSet(unit, rowIndex, component);
              if (rc == L7_FAILURE)
              {
                return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FailedToSetComp);
              }
            }
          }
        }

      }

      else if (rc == L7_TABLE_IS_FULL)
      {
        rc = usmDbLogMaxSyslogHostsGet(unit, &maxNum);
        if (rc == L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_ErrCouldNot,ewsContext, pStrErr_base_FailedToAddHostMaxReached, maxNum);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_FailedToAddHost);
        }
      }

      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_FailedToAddHost);
      }
    }

  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging host
 *         no logging host
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures logging hosts.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingHostRemove(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArg;
  L7_uint32 argIndex = 1;
  L7_uint32 status;
  L7_uint32 indx;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingHostRemove);
  }
  else
  {
    indx = atoi(argv[index+ argIndex]);
    if (usmDbLogHostTableRowStatusGet(unit, indx, &status) == L7_NOT_EXIST)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_FailedHostNonExistent, indx);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogHostTableRowStatusSet(unit, indx, L7_ROW_STATUS_DESTROY) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_FailedToRemoveHost);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  Change the IP Address of log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging host reconfigure <index> <hostaddress|hostname>
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures logging hosts.
 *
 * @end
 *********************************************************************/
const L7_char8 *commandLoggingHostReconfigure(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArg;
  L7_uint32 argIndex = 1;
  L7_uint32 argHost = 2;
  L7_uint32 ipAddr;
  L7_uint32 status;
  L7_char8 strHost[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 oldIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 indx;
  dnsClientLookupStatus_t dnsStatus =  DNS_LOOKUP_STATUS_FAILURE;
  L7_char8  hostFQHN[L7_DNS_DOMAIN_NAME_SIZE_MAX];


  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingHostReconfigure);
  }
  else
  {
    indx = atoi(argv[index+ argIndex]);
    if (usmDbLogHostTableRowStatusGet(unit, indx, &status) == L7_NOT_EXIST)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_FailedHostNonExistent, indx);
    }

    /* ip address */
    if (strlen(argv[index+argHost]) >= sizeof(strHost))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLoggingHostReconfigure);
    }
    OSAPI_STRNCPY_SAFE(strHost, argv[index + argHost]);
    if (usmDbInetAton(strHost, &ipAddr) != L7_SUCCESS)
    {
       if (usmDbDNSClientNameLookup(strHost, &dnsStatus,
                                    hostFQHN, &ipAddr) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
                ewsContext, pStrErr_base_DnsLookupFailed);
       }
    }

    memset ( oldIpAddr, 0, sizeof(oldIpAddr));
    if (usmDbLogHostTableIpAddressGet(unit, indx, oldIpAddr) == L7_SUCCESS &&
        strcmp(oldIpAddr, strHost) == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_FailedToReconfigureHostDup);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogHostTableHostSet(unit, indx, strHost) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_FailedToReconfigureHost);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  Configure log information
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  argc        @b{(input)) the argument count
 * @param  *argv       @b{(input)) pointer to argument
 * @param  index       @b{(input)) the index
 *
 * @returntype  const L7_char8  *
 * @returns  cliPrompt(ewsContext)
 *
 * @comments
 *
 * @mode  Global Config
 *
 * @cmdsyntax:
 *         logging syslog facility <facility>
 *         no logging syslog facility
 *
 * @cmdhelp  Configure logs.
 *
 * @cmddescript  Configures facility for syslog.
 *
 * @end
 *********************************************************************/

const L7_char8 *commandLoggingSyslogFacility(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argComponent = 1;
  L7_uint32  numArg;
  L7_LOG_FACILITY_t facilityId;
  L7_char8 valString[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    numArg = cliNumFunctionArgsGet();
    if (numArg == 1)
    {
      OSAPI_STRNCPY_SAFE(valString,argv[index+ argComponent]);

      if ( cliLogConvertFacilityToVal(valString, &facilityId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidFacilityVal);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbLogSyslogDefaultFacilitySet(unit, facilityId) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingFacilitySet_1);
        }
      }

    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingSyslogFacility);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingSyslogFacilityNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLogSyslogDefaultFacilitySet(unit, FD_LOG_DEFAULT_LOG_FACILITY) == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingFacilitySet_1);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Enables/Disables the functionality based on the key entered
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8
 *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @Mode  Privileged Exec Mode
 *
 * @cmdsyntax for normal command: license advanced <key>
 *
 * @cmdsyntax for no command: no license advanced <key>
 *
 * @cmdhelp
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandLicenseAdvanced(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32
    index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 mode = L7_DISABLE;
  L7_uchar8 keyString[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argKey = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLicenseAdvanced);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoLicenseAdvanced);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  unit = cliGetUnitId();

  OSAPI_STRNCPY_SAFE(keyString, argv[index+argKey]);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbFeatureKeyLicenseValidate(unit, keyString, mode) == L7_FAILURE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidKey);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

#ifdef L7_OUTBOUND_TELNET_PACKAGE
/*********************************************************************
 *
 * @purpose      configure the outbound telnet max sessions
 *
 * @param        EwsContext ewsContext
 * @param        L7_uint32 argc
 * @param        const L7_char8 **argv
 * @param        L7_uint32 index
 *
 * @returntype   const L7_char8  *
 * @returns      cliPrompt(ewsContext)
 *
 * @notes        none
 *
 * @cmdsyntax    session-limit <0-5>
 *
 * @cmdhelp      Configure the number of telnet sessions allowed.
 *
 * @cmddescript
 *   Specify a decimal value from 0 to 5. If the value is 0, no
 *   Outbound  Telnet session can be established. The default
 *   value is 5.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandTelnetMaxSessions(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 tempInt = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 arg1 = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTelnetMaxSession);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+arg1],  &tempInt)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoTelnetMaxSession);
    }
    tempInt = L7_TELNET_DEFAULT_MAX_SESSIONS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbTelnetMaxSessionsSet(unit, tempInt);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  configure the outbound telnet session timeout in minutes
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  session-timeout <0-160>
 *
 * @cmdhelp Configure the telnet login inactivity timeout.
 *
 * @cmddescript
 *   A session is active as long as the session has not remained idle
 *   for the value set. Specify a decimal value from 0 to 160 minutes.
 *   A value of 0 indicates that a OutboundTelnet session remains
 *   active indefinitely. The default is 0 minutes.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandTelnetTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 tempInt = L7_TELNET_DEFAULT_MAX_SESSION_TIMEOUT;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTelnetTimeout);
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1],  &tempInt)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoTelnetTimeout);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbTelnetTimeoutSet(unit, tempInt);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose     Enable or Disable new Outbound Telnet connections
 *
 * @param       EwsContext ewsContext
 * @param       L7_uint32 argc
 * @param       const L7_char8 **argv
 * @param       L7_uint32 index
 *
 * @returntype  const L7_char8  *
 * @returns     cliPrompt(ewsContext)
 *
 * @notes       Number of arguments checking
 *
 * @cmdsyntax   transport output telnet
 *
 * @cmdhelp     Allow or disallow new Outbound Telnet sessions.
 *
 * @cmddescript
 *   Enable means that new Outbound Telnet sessions can be established
 *   until there are no more sessions available. Any already established
 *   session remains active until the session is ended or an abnormal
 *   network error ends it. The default value is Enable.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandOutboundTelnet(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 mode=0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgOutboundTelnetStatus);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoOutboundTelnetStatus);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if(usmDbTelnetAdminModeSet(unit, mode)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose     Telnet to a remote host
 *
 * @param       EwsContext ewsContext
 * @param       L7_uint32 argc
 * @param       const L7_char8 **argv
 * @param       L7_uint32 index
 *
 * @returntype  const L7_char8  *
 * @returns     cliPrompt(ewsContext)
 *
 * @notes       none
 *
 * @cmdsyntax   telnet <ip-address|hostname> [port] [debug] [line] [noecho]
 *
 * @cmdhelp     Telnet to a remote host.
 *
 * @cmddescript
 *   This command is used to create a remote telnet session.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandRemoteTelnet(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 val;
  L7_uint32 tempInt;
  L7_uint32 ipAddr;
  L7_uint32 activeSessions;/* no of active OBT sessions */
  L7_uint32 maxSessions;   /* max no of OBT sessions allowed */
  L7_uint32 argHost   = 1; /* hostname or ipaddress */
  L7_uint32 argSecond = 2; /* port no / debug / line / noecho */
  L7_uint32 argThird  = 3; /* debug / line / noecho */
  L7_uint32 argFourth = 4; /* line / noecho */
  L7_uint32 argFifth  = 5; /* noecho */
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_char8  hostname[L7_DNS_DOMAIN_NAME_SIZE_MAX];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uchar8 family = L7_AF_INET | L7_AF_INET6;
  L7_inet_addr_t   inetAddresses[2];


  L7_RC_t   rc;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_int32  telnetSocketS; 	 /* server socket */
  L7_char8  strHostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 * cp;
  L7_BOOL tmpBool;

  telnetParams_t telnetParams = {{L7_FALSE, L7_FALSE, L7_TRUE}, 0, L7_TELNET_DEFAULT_PORT, L7_NULLPTR, 0};

  /* If we have less than 1 or more than 5 arguments - error */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  /* if more than 5 or less than 1 arguments, display syntax */
  if (numArg < 1 || numArg > 5)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTelnetRemoteConnect_1);
  }
  else
  {
    switch(numArg)
    {
      case 5:  /* localecho */
        tmpBool = L7_FALSE;
        cp = (L7_char8 *)argv[index+argFifth];
        if(cp[0] == ' ')
        {
          tmpBool = L7_TRUE;
          cp++;
        }

        if ((strcmp(cp, pStrInfo_base_Localecho) != 0))
        {
          if(tmpBool == L7_FALSE)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTelnetRemoteConnect_1);
          }
          telnetParams.modes[2] = L7_FALSE;
        }
        /* PASSTHROUGH */
      case 4:  /* line / localecho */
        if ((strcmp(argv[index+argFourth], pStrInfo_base_Line_2)) && (strcmp(argv[index+argFourth], pStrInfo_base_Localecho)) != 0)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTelnetRemoteConnect_1);
        }
        else
        {
          if (strcmp(argv[index+argFourth], pStrInfo_base_Line_2) == 0)
          {
            telnetParams.modes[1] = L7_TRUE;
          }
          if ((strcmp(argv[index+argFourth], pStrInfo_base_Localecho) == 0))
          {
            telnetParams.modes[2] = L7_FALSE;
          }
        }
        /* PASSTHROUGH */
      case 3:  /* debug / line / localecho */
        if ((strcmp(argv[index+argThird], pStrInfo_common_LogSeverityDebug)) && (strcmp(argv[index+argThird], pStrInfo_base_Line_2)) &&
            (strcmp(argv[index+argThird], pStrInfo_base_Localecho)) != 0)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTelnetRemoteConnect_1);
        }
        else
        {
          if (strcmp(argv[index+argThird], pStrInfo_common_LogSeverityDebug) == 0)
          {
            telnetParams.modes[0] = L7_TRUE;
          }
          if (strcmp(argv[index+argThird], pStrInfo_base_Line_2) == 0)
          {
            telnetParams.modes[1] = L7_TRUE;
          }
          if ((strcmp(argv[index+argThird], pStrInfo_base_Localecho) == 0))
          {
            telnetParams.modes[2] = L7_FALSE;
          }
        }
        /* PASSTHROUGH */
      case 2:  /* port no / debug / line / localecho */
        if (cliConvertTo32BitUnsignedInteger(argv[index+argSecond], &tempInt) != L7_SUCCESS)
        {
          if ((strcmp(argv[index+argSecond], pStrInfo_common_LogSeverityDebug)) && (strcmp(argv[index+argSecond], pStrInfo_base_Line_2)) &&
              (strcmp(argv[index+argSecond], pStrInfo_base_Localecho)) != 0)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTelnetRemoteConnect_1);
          }
          else
          {
            if (strcmp(argv[index+argSecond], pStrInfo_common_LogSeverityDebug) == 0)
            {
              telnetParams.modes[0] = L7_TRUE;
            }
            if (strcmp(argv[index+argSecond], pStrInfo_base_Line_2) == 0)
            {
              telnetParams.modes[1] = L7_TRUE;
            }
            if ((strcmp(argv[index+argSecond], pStrInfo_base_Localecho) == 0))
            {
              telnetParams.modes[2] = L7_FALSE;
            }
          }
        }
        else
        {
          if (tempInt > L7_TELNET_MAX_PORT_NO)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_OutboundTelnetBadPortNo);
          }
          else
          {
            telnetParams.port = tempInt;
          }
        }
        /* PASSTHROUGH */
      case 1:  /* ipaddress */
        if ((strlen(argv[index+argHost]) >= L7_TELNET_MAX_HOST_LENGTH))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
        }

        /* Verify if the specified ip address is valid */
        OSAPI_STRNCPY_SAFE(strHostAddr, argv[index+argHost]);
        if (cliIPHostAddressValidate(ewsContext, strHostAddr, &ipAddr, &addrType) != L7_SUCCESS)
        {
          /* Invalid Host Address*/
          return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
        }
        if (addrType == L7_IP_ADDRESS_TYPE_DNS)
        {
          rc = usmDbDNSClientInetNameLookup(family, strHostAddr, &status,
                                            hostname, inetAddresses);
          /* If the DNS lookup fails */
          if (rc == L7_SUCCESS)
          {
            inetAddressGet(L7_AF_INET, &inetAddresses[0], &ipAddr);
            usmDbInetNtoa(ipAddr, strHostAddr);
          }
          else
          {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
              ewsContext, pStrErr_base_DnsLookupFailed);
          }
        }

        /* IP address should be between 0.0.0.1 and 255.255.255.255 */
        if ((ipAddr < 1) || (ipAddr > 0xFFFFFFFF))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
        }
        telnetParams.ipAddr = (L7_char8 *) strHostAddr;
        break;
    }
  }

  telnetParams.sock = cliCurrentSocketGet();  /* get client socket no */

  if (cliCurrentHandleGet() == CLI_SERIAL_HANDLE_NUM)
  {
    telnetParams.sockout = cliWebIORedirectStdOutGet();  /* get actual serial fd */
  }
  else
  {
    telnetParams.sockout = telnetParams.sock;
  }

  rc = usmDbTelnetAdminModeGet(unit, &val);
  switch (val)  /* val = Enable / Disable */
  {
    case L7_DISABLE:
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_OutboundTelnetNewSessionDsbld);
      break;
    default:
      break;
  }

  usmDbTelnetMaxSessionsGet(unit, &maxSessions);  /* get maximum no of OBT sessions configured on the switch */
  usmDbTelnetNoOfActiveSessionsGet(unit, &activeSessions);  /* get no of active OBT sessions */

  if (activeSessions >= maxSessions)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_OutboundTelnetMaxSessionsReached);
  }

  telnetSocketS = usmDbTelnetNewCon(unit, &telnetParams);  /* server socket */
  if (telnetSocketS == L7_ERROR)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_CfgTelnetRemoteConnect);
  }
  else
  {
    ewsContext->obtActive = L7_TRUE;
    ewsContext->obtSocket = telnetSocketS;
    usmDbObtLoginSessionStart(cliLoginSessionIndexGet());
  }

  return pStrInfo_common_CrLf;
}
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
/*********************************************************************
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8
 *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @Mode  Interface Config
 *
 * @cmdsyntax for normal command: sntp client mode
 *
 * @cmdsyntax for no command: no sntp client mode
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSntpClientMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 modeName[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit;
  L7_SNTP_CLIENT_MODE_t mode = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* validity chekcing access and arguments */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1 || strlen(argv[index+1]) >= sizeof(modeName))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputSntpClientMode);
    }

    else
    {
      OSAPI_STRNCPY_SAFE (modeName, argv[index+1]);

      if (usmDbStringCaseInsensitiveCompare (modeName, pStrInfo_common_Ucast_1) == L7_SUCCESS)
      {
        mode = L7_SNTP_CLIENT_UNICAST;
      }
      else if (usmDbStringCaseInsensitiveCompare (modeName, pStrInfo_common_Bcast_1) == L7_SUCCESS)
      {
        mode = L7_SNTP_CLIENT_BROADCAST;
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpClientModeSet(unit, mode);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpClientMode);
        }
      }
    }
  }

  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputNoSntpClientMode);
    }
    mode = L7_SNTP_CLIENT_DISABLED;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSntpClientModeSet(unit, mode);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpClientMode);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  To configure SNTP client port
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax sntp client port
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSntpClientPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit;
  L7_uint32 port;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* validity chekcing access and arguments */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrInfo_base_IncorrectInputSntpClientPort, SNTP_MIN_CLIENT_PORT, SNTP_MAX_CLIENT_PORT);
    }

    port = atoi(argv[index+1]);
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSntpClientPortSet(unit, port);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpPort);
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrInfo_base_IncorrectInputNoSntpClientPort);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSntpClientPortDefaultSet(unit);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpPort);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  To configure SNTP unicast client poll-interval
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax sntp client unicast poll-interval
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const L7_char8 *commandSntpClientUnicastPollInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit;
  L7_int32 val;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* validity chekcing access and arguments */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (cliNumFunctionArgsGet() != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgSntpClientUcastPollIntvl, SNTP_MIN_POLL_INTERVAL, SNTP_MAX_POLL_INTERVAL);
    }
    else
    {
      val = atoi(argv[index+1]);
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpClientUnicastPollIntervalSet(unit, val);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpPollIntvl);
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoSntpClientUcastPollIntvl);
    }
    else
    {   /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpClientUnicastPollIntervalSet(unit, L7_SNTP_DEFAULT_POLL_INTERVAL);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpPollIntvl);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  To configure SNTP unicast client poll-timeout
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax sntp client unicast poll-timeout
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSntpClientUnicastPollTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit;
  L7_int32 val;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* validity chekcing access and arguments */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgSntpClientUcastPollTimeout, SNTP_MIN_POLL_TIMEOUT, SNTP_MAX_POLL_TIMEOUT);
    }
    else
    {
      val = atoi(argv[index+1]);
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpClientUnicastPollTimeoutSet(unit, val);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpPollTimeout);
        }
      }
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoSntpClientUcastPollTimeout);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpClientUnicastPollTimeoutSet(unit, L7_SNTP_DEFAULT_POLL_TIMEOUT);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpPollTimeout);
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  To configure SNTP unicast client poll-retry
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax sntp client unicast poll-retry
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSntpClientUnicastPollRetry(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit;
  L7_int32 val;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* validity chekcing access and arguments */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (cliNumFunctionArgsGet() != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgSntpClientUcastPollRetry, SNTP_MIN_POLL_RETRY, SNTP_MAX_POLL_RETRY);
    }
    else
    {
      val = atoi(argv[index+1]);
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpClientUnicastPollRetrySet(unit, val);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpPollRetry);
        }
      }
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoSntpClientUcastPollRetry);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpClientUnicastPollRetrySet(unit, L7_SNTP_DEFAULT_POLL_RETRY);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpPollRetry);
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  To configure SNTP broadcast client poll-interval
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax sntp client broadcast poll-interval
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSntpClientBroadcastPollInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit;
  L7_int32 val;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* validity chekcing access and arguments */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgSntpClientBcastPollIntvl, SNTP_MIN_POLL_INTERVAL, SNTP_MAX_POLL_INTERVAL);
    }
    else
    {
      if ((cliConvertTo32BitUnsignedInteger(argv[index+1], &val)!= L7_SUCCESS))
      {
        return cliPrompt(ewsContext);
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpClientBroadcastPollIntervalSet(unit, val);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpPollIntvl);
        }
      }
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoSntpClientBcastPollIntvl);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpClientBroadcastPollIntervalSet(unit, L7_SNTP_DEFAULT_POLL_INTERVAL);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpPollIntvl);
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  To configure SNTP server entry
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax sntp server <ipaddress | hostname> [<priority>] [<version>]
 *                        [<port>]
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSntpServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32  numArg, num, maxNum;
  L7_char8 strServHost[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_SNTP_ADDRESS_TYPE_t addressType = L7_SNTP_ADDRESS_UNKNOWN;
  L7_inet_addr_t serverAddr;
  L7_IP_ADDRESS_TYPE_t   type = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uint32 ipAddr = L7_NULL;
  L7_RC_t   rc;
  L7_uint32 unit;
  L7_uint32 valPriority, valVersion, valPort;
  L7_uint32 rowIndex;

  L7_uint32 argServAddr = 1;

  inetAddressReset(&serverAddr);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

      /* ip address*/
      if (strlen(argv[index+argServAddr]) >= sizeof(strServHost))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
      }
  OSAPI_STRNCPY_SAFE(strServHost, argv[index + argServAddr]);


      /* type */
  rc = usmDbIPHostAddressValidate(strServHost,&ipAddr,&type);
  if(rc == L7_SUCCESS)
  {
    if (type == L7_IP_ADDRESS_TYPE_DNS)
    {
	    addressType = L7_SNTP_ADDRESS_DNS;
    }
    else if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
	    addressType = L7_SNTP_ADDRESS_IPV4;
    }
  }
  else if(rc == L7_ERROR)
  {
    /* Invalid Ip Address */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
			  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  else if( rc == L7_FAILURE)
  {
    /* It can be either a invalid host name or can be a IPV6 address */
    /* Convert the IP Address to Inet Address */
    if(usmDbIPv6HostAddressValidate(strServHost, &serverAddr,&type) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,
  	    ewsContext, pStrErr_common_TacacsInValIp);
    }
    if (type == L7_AF_INET6)
    {
        addressType = L7_SNTP_ADDRESS_IPV6;
    }
    else if (type == L7_IP_ADDRESS_TYPE_DNS)
    {
        addressType = L7_SNTP_ADDRESS_DNS;
    }
    else
    {
        /* Invalid Host Name*/
        cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
  		    ewsContext, pStrErr_base_InvalidHostName);
    }
  }
#endif
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (numArg < 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgSntpSrvr);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpServerCurrentEntriesGet(unit, &num);
        if (rc == L7_SUCCESS)
        {
          rc = usmDbSntpServerMaxEntriesGet(unit, &maxNum);
          if (rc == L7_SUCCESS)
          {
            if (num == maxNum)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, buf, sizeof(buf), pStrInfo_base_MaxSntpSrvrs, maxNum);
              ewsTelnetWrite( ewsContext, buf);
              return cliPrompt(ewsContext);
            }
          }
        }
      }

      /* priority */
      if ( numArg >= 2 )
      {
        if (cliConvertTo32BitUnsignedInteger(argv[index+2], &valPriority) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }

      if ( numArg >= 3 )
      {
        if (cliConvertTo32BitUnsignedInteger(argv[index+3], &valVersion) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
      /* port */
      if ( numArg >= 4 )
      {
        if (cliConvertTo32BitUnsignedInteger(argv[index+4], &valPort) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpServerAdd (unit, strServHost, addressType, &rowIndex);
        if (rc == L7_ALREADY_CONFIGURED)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 2, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_SntpSrvrAlreadyExists);
        }
        else if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 2, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_base_SntpSrvrAdd);
        }

      if (numArg >= 2)
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbSntpServerTablePrioritySet(unit, rowIndex, valPriority);
          if (rc != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpSrvrPri);
          }
        }
      }

      /* version */
      if (numArg >= 3)
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbSntpServerTableVersionSet(unit, rowIndex, valVersion);
          if (rc != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_SetSntpSrvrVer);
          }
        }
      }

      /* port */
      if (numArg >= 4)
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbSntpServerTablePortSet(unit, rowIndex, valPort);
          if (rc != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_base_SetSntpSrvrPort);
          }
        }
      }
    }
  }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {

    if (numArg < 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgSntpSrvrNo);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSntpServerDelete (unit, strServHost, addressType);
        if (rc != L7_SUCCESS)
        {
          if (rc == L7_NOT_EXIST)
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_SntpSrvrNotExist);
            /*************Set Flag for Script Successful******/
            ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_base_SntpSrvrDel);
          }

          return cliSyntaxReturnPrompt (ewsContext, "");
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Sets the port Max Frame size for all interfaces.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @Mode  Global Config
 *
 * @cmdsyntax  mtu <size>
 * @cmdsyntax for no command: no mtu
 *
 * @cmdhelp Enable/Disable the port's administrative mode.
 *
 * @cmddescript
 *   This is a configurable value and indicates if the port is
 *   enabled or disabled. The default for all ports is Enabled.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandGlobalMaxFrameSize(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argMaxFrameSize = 1;
  L7_uint32 interface, nextInterface;
  L7_int32  maxFrameSize = L7_MIN_FRAME_SIZE;
  L7_uint32 numArg;
  L7_uint32 u, s, p;
  L7_uint32 unit;
  L7_char8  tmp[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_BOOL   first = L7_TRUE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if ( unit == 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrInfo_base_UseMtu, L7_MIN_FRAME_SIZE, L7_MAX_FRAME_SIZE);
    }

    if ((cliConvertTo32BitUnsignedInteger(argv[index+argMaxFrameSize], &maxFrameSize) != L7_SUCCESS))
    {
      return cliPrompt(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrInfo_base_UseNoMtu);
    }
  }

  if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_NoValidPortsInBox_1);
  }

  memset(tmp, 0, sizeof(tmp));
  while (interface)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIntfParmCanSet(interface, L7_INTF_PARM_FRAMESIZE) == L7_TRUE &&
          usmDbIfConfigMaxFrameSizeSet(interface, maxFrameSize) != L7_SUCCESS)
      {
        if (usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS)
        {
          if (first == L7_TRUE)
          {
            osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u,s,p));
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), ", %s", cliDisplayInterfaceHelp(u,s,p));
          }
          OSAPI_STRNCAT(tmp, stat);
          first = L7_FALSE;
        }
      }
    }

    if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
    {
      interface = nextInterface;
    }
    else
    {
      interface = 0;
    }

  } /* endwhile interface */

  if (first != L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_CouldNot, ewsContext, pStrErr_base_CfgMaxFrameSizeOnPorts);
    ewsTelnetWrite(ewsContext, tmp);
    ewsTelnetWrite(ewsContext, pStrInfo_common_Period);
    cliSyntaxBottom(ewsContext);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
/*********************************************************************
 *
 * @purpose  Callback for the traceroute
 *
 * @param L7_uint32 probePerHop
 * @param L7_ushort16 handle
 * @param L7_uint32 ttl
 * @param L7_uint32 ipda
 * @param L7_uint16 hopCount
 * @param L7_uint16 probeCount
 * @param L7_uint32 rtt
 * @param L7_char8  errSym
 *
 * @returns L7_SUCCESS
 *
 * @notes
 *       Prints response for one probe
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t traceRouteCallbackFn(void * param, L7_ushort16 handle, L7_uint32 ttl,
                                    L7_uint32 ipDa, L7_ushort16 hopCount, L7_ushort16 probeCount,
                                    L7_uint32 rtt, L7_char8 errSym )
{

  static L7_uint32 lastIpAddr = 0;
  L7_uchar8  buff[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8  ipBuff[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8  tmpBuff[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32  probePerHop = (L7_uint32)param;
  
  buff[0] = '\0';
  if(probeCount == 1)
  {
    lastIpAddr = ipDa;
    osapiInetNtoa( ipDa, tmpBuff );
    if(ttl < 10)
    {
      sprintf(buff, "%-2d %-16s  ", ttl, tmpBuff);
    }
    else
    {
      sprintf(buff, "%d %-16s  ", ttl, tmpBuff);
    }
  }

  if(ipDa != lastIpAddr)
  {
    osapiInetNtoa( ipDa, tmpBuff );
    sprintf( ipBuff,"[%-16.15s] %s", tmpBuff, pStrInfo_base_Reports);
    strcat( buff, ipBuff );
  }

  if (errSym == ' ')
  {
    if(rtt/TRACEROUTE_RTT_MULTIPLIER != 0)
    {
      osapiSnprintf(tmpBuff, sizeof(tmpBuff)-1, " %-3d ms %c ", rtt/TRACEROUTE_RTT_MULTIPLIER, errSym);
    }
    else
    {
      sprintfAddBlanks (0, 0, 0, 0, L7_NULLPTR,  tmpBuff, pStrInfo_base_Msec2, errSym );
    }
  }
  else
  {
    switch (errSym)
    {
      case 'P':
        sprintfAddBlanks (0, 0, 0, 3, L7_NULLPTR,  tmpBuff, "%s", pStrInfo_base_DestProtoUnreachable );
        break;
      case 'N':
        sprintfAddBlanks (0, 0, 0, 3, L7_NULLPTR,  tmpBuff, "%s", pStrInfo_base_DestNetUnreachable );
        break;
      case 'H':
        sprintfAddBlanks (0, 0, 0, 3, L7_NULLPTR,  tmpBuff, "%s", pStrInfo_base_DestHostUnreachable );
        break;
      case 'F':
        sprintfAddBlanks (0, 0, 0, 3, L7_NULLPTR,  tmpBuff, "%s", pStrInfo_base_IpDFbitset);
        break;
      case 'S':
        sprintfAddBlanks (0, 0, 0, 3, L7_NULLPTR,  tmpBuff, "%s", pStrInfo_base_SrcRouteFail);
        break;
      case 'A':
        sprintfAddBlanks (0, 0, 0, 3, L7_NULLPTR,  tmpBuff, "%s", pStrInfo_base_DestNetAdminProhibit );
        break;
      case 'C':
        sprintfAddBlanks (0, 0, 0, 3, L7_NULLPTR,  tmpBuff, "%s", pStrInfo_base_DestHosttAdminProhibit );
        break;
      case '*':
        sprintfAddBlanks (0, 0, 0, 0, L7_NULLPTR,  tmpBuff, " %-9c", errSym );
        break;
      default :
        sprintfAddBlanks (0, 0, 0, 3, L7_NULLPTR,  tmpBuff, pStrInfo_base_Msec, rtt/TRACEROUTE_RTT_MULTIPLIER, errSym );
        break;
    }
  }

  strcat( buff, tmpBuff );
  if ( probePerHop == 0 )
  {
    probePerHop = L7_TRACEROUTE_NUM_PROBE_PACKETS;
  }
  if ( probeCount == probePerHop )
  {
    strcat( buff, pStrInfo_common_CrLf );
  }

  cliWrite( buff );
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose    Trace the route for the specified IP address.
 *
 * @param      EwsContext ewsContext
 * @param      L7_uint32 argc
 * @param      const L7_char8 **argv
 * @param      L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns    cliPrompt(ewsContext)
 *
 * @notes      none
 *
 * @Mode       Privileged EXEC
 *
 * @cmdsyntax  traceroute <ipaddr|hostname> [initTtl <initTtl>]
 * [maxTtl <maxTtl>]
 * [maxFail <maxFail>] [interval <interval>] [count <count>] [port <port>]
 * [size <size>]
 *
 * @cmdhelp    Trace the route for the specified IP address.
 *
 * @cmddescript
 *   This command is is used to discover the routes that packets actually
 *   take when traveling to their destination through the network on a
 *   hop-by-hop basis.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandTraceRoute(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 ipAddr_check;
  L7_char8 str[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 hostAddressStr[L7_DNS_HOST_NAME_LEN_MAX + 1];
  L7_ushort16 handle;
  L7_uint32 probePerHop = TRACEROUTE_DEFAULT_PROBE_PER_HOP;
  L7_ushort16 probeInterval = TRACEROUTE_DEFAULT_PROBE_INTERVAL;
  L7_ushort16 probeSize = TRACEROUTE_DEFAULT_PROBE_SIZE;
  L7_uint32   port = TRACEROUTE_DEFAULT_PORT;
  L7_ushort16 maxTtl = TRACEROUTE_DEFAULT_MAX_TTL;
  L7_ushort16 initTtl = TRACEROUTE_DEFAULT_INIT_TTL;
  L7_ushort16 maxFail = TRACEROUTE_DEFAULT_MAX_FAIL;
  L7_ushort16 ttl;
  L7_ushort16 currHopCount, currProbeCount, testAttempts, testSuccess;
  L7_uint32   vrfId = L7_VALID_VRID;
  L7_uint32 unit, numArgs;
  L7_BOOL operStatus;
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_char8    domainName[L7_DNS_DOMAIN_NAME_SIZE_MAX];
  L7_RC_t     rc = L7_FAILURE;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_inet_addr_t inetAddr;


  /* check the validity */
  cliSyntaxTop(ewsContext);

  if( ewsContext->commType == CLI_NO_CMD)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_common_CmdTypeNormal );
  }

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs > 14 || ((numArgs % 2) != 0) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_TraceRoute_1);
  }

  osapiStrncpySafe(hostAddressStr, argv[index], sizeof(hostAddressStr));

  while ( numArgs > 0 )
  {
    strcpy( buf, argv[ index + 1 ] );
    if (cliCheckIfInteger((L7_char8 *)argv[index+2]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ( strcmp( buf, pStrInfo_base_Count_1 ) == 0 )
    {
      probePerHop = atoi((L7_char8 *)argv[index+2]);
    }

    if ( strcmp( buf, pStrInfo_common_WsInputIntvl ) == 0 )
    {
      probeInterval = atoi((L7_char8 *)argv[index+2]);
    }

    if ( strcmp( buf, pStrInfo_common_Size_1 ) == 0 )
    {
      probeSize = atoi((L7_char8 *)argv[index+2]);
    }

    if ( strcmp( buf, pStrInfo_common_Port_4 ) == 0 )
    {
      port = atoi((L7_char8 *)argv[index+2]);
    }

    if ( strcmp( buf, pStrInfo_base_Maxttl_1 ) == 0 )
    {
      maxTtl = atoi((L7_char8 *)argv[index+2]);
    }

    if ( strcmp( buf, pStrInfo_base_Initttl_1 ) == 0 )
    {
      initTtl = atoi((L7_char8 *)argv[index+2]);
    }

    if ( strcmp( buf, pStrInfo_base_Maxfail_1 ) == 0 )
    {
      maxFail = atoi((L7_char8 *)argv[index+2]);
    }

    numArgs -= 2;
    index += 2;
  }

  if ( vrfId == L7_VR_NO_VRID )
  {
    sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, buf, pStrErr_base_VirtualRtrContext );
    ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
    return cliSyntaxReturnPrompt (ewsContext, buf );
  }

  /* validate the ip address as ipv4*/
  if (cliIPHostAddressValidate(ewsContext, hostAddressStr, &ipAddr_check, &addrType) != L7_SUCCESS)
  {
    /* Invalid Host Address*/
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }

  if (addrType == L7_IP_ADDRESS_TYPE_IPV4)
  {
    usmDbInetNtoa(ipAddr_check, buf);
  }
  else
  {
    inetAddressReset(&inetAddr);
    rc = usmDbDNSClientInetNameLookup(L7_AF_INET, hostAddressStr, &status,
                                      domainName, &inetAddr);
    /*  If the DNS lookup fails */
    if (rc == L7_SUCCESS)
    {
      inetAddrHtop(&inetAddr, buf);
      (void) inetAddressGet(L7_AF_INET, &inetAddr, &ipAddr_check);
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
             ewsContext, pStrErr_base_DnsLookupFailed);
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( probeSize > TRACEROUTE_MAX_PROBE_SIZE )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR,  buf, pStrErr_base_PingSize, TRACEROUTE_MIN_PROBE_SIZE,
        TRACEROUTE_MAX_PROBE_SIZE );
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if ( probePerHop > TRACEROUTE_MAX_PROBE_PER_HOP ||
      probePerHop < TRACEROUTE_MIN_PROBE_PER_HOP )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR,  buf, pStrErr_base_TraceRouteProbePerHop,
        TRACEROUTE_MIN_PROBE_PER_HOP, TRACEROUTE_MAX_PROBE_PER_HOP );
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if ( probeInterval > TRACEROUTE_MAX_INTERVAL || probeInterval < TRACEROUTE_MIN_INTERVAL )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR,  buf, pStrErr_base_PingIntvl_1, TRACEROUTE_MIN_INTERVAL,
        TRACEROUTE_MAX_INTERVAL );
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if ( maxTtl > TRACEROUTE_MAX_TTL || maxTtl < TRACEROUTE_MIN_TTL )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR,  buf, pStrErr_base_TraceRouteMaxTtl, TRACEROUTE_MIN_TTL,
        TRACEROUTE_MAX_TTL );
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if ( initTtl > TRACEROUTE_MAX_INIT_TTL  )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR,  buf, pStrErr_base_TraceRouteInitTtl_1, TRACEROUTE_MIN_INIT_TTL,
        TRACEROUTE_MAX_INIT_TTL );
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if ( initTtl > maxTtl )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_base_TraceRouteTtl);
  }

  if ( maxFail > TRACEROUTE_MAX_FAIL  )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR,  buf, pStrErr_base_TraceRouteMaxFail, TRACEROUTE_MIN_FAIL,
        TRACEROUTE_MAX_FAIL );
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if ( port > TRACEROUTE_MAX_PORT || port < TRACEROUTE_MIN_PORT )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR,  buf, pStrErr_base_TraceRoutePort, TRACEROUTE_MIN_PORT,
        TRACEROUTE_MAX_PORT );
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }
  sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR, str,pStrInfo_base_TracerouteToHopsMaxBytePkts, hostAddressStr, maxTtl,probeSize);
  cliWrite(str);
  memset(str,0,sizeof(str));
  cliWrite(pStrInfo_common_CrLf);

  if ( usmDbTraceRoute( "", "", L7_TRUE, vrfId, ipAddr_check,
        probeSize, (L7_ushort16)probePerHop, probeInterval, L7_FALSE,
        port, maxTtl, initTtl, maxFail, traceRouteCallbackFn,
        (void *)probePerHop, &handle ) != L7_SUCCESS )
  {
    sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, str, pStrInfo_base_TracerouteFailed_1 );
  }
  else
  {
    if ( usmDbTraceRouteQuery( handle, &operStatus, &ttl, &currHopCount, &currProbeCount, &testAttempts, &testSuccess ) != L7_SUCCESS )
    {
      sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, str, pStrInfo_base_TracerouteFailed_1 );
    }
    else
    {
      sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR,  str,pStrInfo_base_HopCountLastTtlTestAttemptTestSuccess, currHopCount, ttl, testAttempts, testSuccess );
    }
    usmDbTraceRouteFree( handle );
  }

  return cliSyntaxReturnPrompt (ewsContext, str);
}

/*********************************************************************
 *
 * @purpose  Enables/Disables the CLI command logging admin mode
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8
 *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @Mode  Global Config Mode
 *
 * @cmdsyntax for normal command: logging cli-command
 *
 * @cmdsyntax for no command: no logging cli-command
 *
 * @cmdhelp
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandLoggingCliCommand(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 mode;
  L7_RC_t rc;
  L7_uint32 persistentLogSeverity;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLogging );
    }

    mode = L7_ENABLE;
    if( usmDbCmdLoggerAdminModeSet(mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingSet );
    }

    rc = usmDbLogPersistentAdminStatusSet(unit, L7_ADMIN_MODE_ENABLE);
    if(rc == L7_FAILURE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingEnbl);
    }
    if(rc == L7_NOT_SUPPORTED)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingEnbl);
    }

    rc = usmDbLogPersistentSeverityFilterGet(unit, &persistentLogSeverity);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else if (persistentLogSeverity > L7_LOG_SEVERITY_NOTICE)
    {
      /* If persistent log severity is already above NOTICE do nothing */
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /*
     *  Now persistent log severity is below NOTICE
     *  so set it to NOTICE
     */
    rc = usmDbLogPersistentSeverityFilterSet(unit, L7_LOG_SEVERITY_NOTICE);
    if(rc == L7_FAILURE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingSeveritySet);
    }
    if(rc == L7_NOT_SUPPORTED)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingEnbl);
    }
  } /* END OF NORMAL COMMAND */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if( numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLoggingNo );
    }

    mode = L7_DISABLE;
    if( usmDbCmdLoggerAdminModeSet(mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LoggingSet );
    }
  } /* END OF NO COMMAND */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");

} /* END OF FUNCTION */

/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Vlan mode.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   set igmp
 *
 * @cmdhelp
 *
 * @cmddescript  This command enables and disables IGMP Snooping on an vlan.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPVlan(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32  argVlan;
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */

    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext,CLISYNTAX_SNOOPIGMP_SETIGMP(family));
    }
    if( cliConvertTo32BitUnsignedInteger(argv[index + 1],&argVlan) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbSnoopVlanModeSet(unit, argVlan, L7_ENABLE, family) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, CLIERROR_SNOOPIGMP_VLANMODESET(family));
      }
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext,CLISYNTAX_SNOOPIGMP_NOSETIGMP(family));
    }

    if( cliConvertTo32BitUnsignedInteger(argv[index + 1],&argVlan) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbSnoopVlanModeSet(unit, argVlan, L7_DISABLE, family) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, CLIERROR_SNOOPIGMP_VLANMODESET(family));
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping fast leave mode for the vlan
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   set igmp fast-leave <vlanid>
 *
 * @cmdhelp
 *
 * @cmddescript  Configure Fast Leaving for the Vlan
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPVlanFastLeave(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_uint32 vlanId;
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPIGMP_SETFASTLEAVE(family));
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &vlanId)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbSnoopVlanFastLeaveModeSet(unit, vlanId, L7_ENABLE, family) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpFastLeaveSet);
      }
    }
  }/* END OF NORMAL COMMAND */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPIGMP_NOSETFASTLEAVE(family));
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &vlanId)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbSnoopVlanFastLeaveModeSet(unit, vlanId, L7_DISABLE, family) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpFastLeaveSet);
      }
    }
  }/* END OF NO COMMAND */
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Group Membership Interval for the vlan
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   set igmp groupmembership-interval <vlanid>
 *
 * @cmdhelp
 *
 * @cmddescript  Configure the Group Membership Interval for the Vlan
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPVlanGroupMembershipInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_uint32 vlanId;
  L7_ushort16 grpmembershipint;
  L7_ushort16  maxresponsetime;
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 2)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPIGMP_SETGMI(family));
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &vlanId)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    grpmembershipint = atoi(argv[index+2]);

    if(usmDbSnoopVlanMaximumResponseTimeGet(unit, vlanId, &maxresponsetime, family) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpMaxRespTimeGet);
    }
    if(maxresponsetime >= grpmembershipint)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpGmiLesser);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbSnoopVlanGroupMembershipIntervalSet(unit, vlanId, grpmembershipint, family)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpGmiSet);
      }
    }

  } /* END OF NORMAL COMMAND */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPIGMP_NOSETGMI(family));
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &vlanId)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    grpmembershipint = FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;

    if(usmDbSnoopVlanMaximumResponseTimeGet(unit, vlanId, &maxresponsetime, family) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpMaxRespTimeGet);
    }
    if(maxresponsetime >= grpmembershipint)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpGmiLesser);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbSnoopVlanGroupMembershipIntervalSet(unit, vlanId, grpmembershipint, family)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpGmiSet);
      }
    }
  } /* END OF NO COMMAND */
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Maximum Response Time for the vlan
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   set igmp maxresponse <vlanid>
 *
 * @cmdhelp
 *
 * @cmddescript  Configure the aximum Response Time for the Vlan
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPVlanMaxResponseTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_uint32 vlanId;
  L7_ushort16 maxresponsetime;
  L7_ushort16 groupmembershipinterval;
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 2)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPIGMP_SETMAXRESPONSETIME(family));
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &vlanId)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    maxresponsetime = atoi(argv[index+2]);

    if(usmDbSnoopVlanGroupMembershipIntervalGet(unit, vlanId, &groupmembershipinterval, family) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpGmiGet);
    }
    if(maxresponsetime >= groupmembershipinterval)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpRespTimeGreater);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbSnoopVlanMaximumResponseTimeSet(unit, vlanId, maxresponsetime, family)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpMaxRespTimeSet);
      }
    }
  } /* END OF NORMAL COMMAND */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPIGMP_NOSETMAXRESPONSETIME(family));
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &vlanId)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    maxresponsetime = FD_IGMP_SNOOPING_MAX_RESPONSE_TIME;
    if(usmDbSnoopVlanGroupMembershipIntervalGet(unit, vlanId, &groupmembershipinterval, family) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpGmiGet);
    }
    if(maxresponsetime >= groupmembershipinterval)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpRespTimeGreater);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbSnoopVlanMaximumResponseTimeSet(unit, vlanId, maxresponsetime, family)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpMaxRespTimeSet);
      }
    }
  } /* END OF NO COMMAND */
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose  Configures the IGMP Snooping Multicast Router Expiry Time for the vlan
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   set igmp mcrtrexpirytime <vlanid>
 *
 * @cmdhelp
 *
 * @cmddescript  Configure the Multicast Router Expiry Time for the Vlan
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetIGMPVlanMulticastRouterExpiryTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_uint32 vlanId;
  L7_uint32 temp;
  L7_ushort16 mcrtrexpirytime;
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 2)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPIGMP_SETMCASTRTREXPIRYTIME(family));
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &vlanId)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    temp = atoi(argv[index+2]);
    mcrtrexpirytime = (L7_ushort16)temp;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbSnoopVlanMcastRtrExpiryTimeSet(unit, vlanId, mcrtrexpirytime, family)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpMcastRtrExpiryTimeSet);
      }
    }
  }/* END OF NORMAL COMMAND */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPIGMP_NOSETMCASTRTREXPIRYTIME(family));
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &vlanId)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    mcrtrexpirytime = FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if( usmDbSnoopVlanMcastRtrExpiryTimeSet(unit, vlanId, mcrtrexpirytime, family)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_SnoopIgmpMcastRtrExpiryTimeSet);
      }
    }
  }/* END OF NO COMMAND */
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose  Configure the Port-channel as static/Dynamic
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   port-channel static
 *
 * @cmdhelp
 *
 * @cmddescript  Configure the Port-channel as static/Dynamic
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortChannelStaticLAG(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, s, p, interface;
  L7_uint32 itype;
  L7_BOOL mode=L7_FALSE;
  L7_uint32 numArg;
  L7_RC_t rc=L7_FAILURE;
  L7_RC_t status = L7_SUCCESS;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_LagsTatic_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_NoLagsTatic);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* normal command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  /* no form */
  else
  {
    mode = L7_FALSE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        /* Only LAGS are allowed */
        if( usmDbIntfTypeGet(interface, &itype) != L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          status = L7_FAILURE;
          continue;
        }
        /* Interface should be LAG interface */
        if (itype != L7_LAG_INTF)
        {
          osapiSnprintf(buf, sizeof(buf), pStrErr_common_InvalidLag, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, buf);
          status = L7_FAILURE;
          continue;
        }
    
        /* Sets the static mode */
        rc=usmDbDot3adLagStaticModeSet(unit, interface, mode);
        if (rc != L7_SUCCESS)
        {
          if (rc==L7_TABLE_IS_FULL)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            osapiSnprintf(buf, sizeof(buf), pStrInfo_base_LagsTaticDynLagFull,platIntfDynamicLagIntfMaxCountGet());
            ewsTelnetWriteAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, buf);
            status = L7_FAILURE;
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_LagsTatic);
            status = L7_FAILURE;
          }
        }
      }
    }
  }
  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Configure the Port-channel Load Balance
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax   port-channel load-balance <hashMode>
 *
 * @cmdhelp
 *
 * @cmddescript  Configure the Port-channel Load Balance
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortChannelLoadBalanceLAG(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argHashMode = 1;
  L7_uint32 unit, s, p, interface;
  L7_uint32 itype;
  L7_uint32 numArg;
  L7_uint32 hashMode;
  L7_RC_t   rc=L7_FAILURE;
  L7_uchar8 strHashMode[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t status = L7_SUCCESS;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* getting the param 1 */
    OSAPI_STRNCPY_SAFE(strHashMode, argv[index+argHashMode]);

    if (cliCheckIfInteger(strHashMode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_LagLoadBalance);
    }

    hashMode=atoi(strHashMode);
  }
  else
  {
    hashMode=FD_DOT3AD_HASH_MODE;
  }

  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        /* Only LAGS are allowed */
        if( usmDbIntfTypeGet(interface, &itype) != L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          status = L7_FAILURE;
          continue;
        }
        /* Interface should be LAG interface */
        if (itype != L7_LAG_INTF)
        {
          osapiSnprintf(buf, sizeof(buf), pStrErr_common_InvalidLag, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, buf);
          status = L7_FAILURE;
          continue;
        }
      
        rc=usmDbDot3adLagHashModeSet(unit, interface, hashMode);
      
        if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          if (rc == L7_DEPENDENCY_NOT_MET)
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_LagHashModeAdminDown);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_LagReturn);
          }
          status = L7_FAILURE;
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Add description to an interface.
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  description <string>
 *
 * @cmdhelp Add description to an interface configuration.
 *
 * @author Gunavardhani
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8  *commandIntfDescription(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface;
  L7_uint32 unit;
  L7_uint32 s,p;
  L7_uint32 numArg;
  L7_uint32 argDescription = 1;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package
     unit = cliGetUnitId(); */

  numArg = cliNumFunctionArgsGet();

  /* Error Checking for command Type */
  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      /*check to see if anything other than 1 argument was passed */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IntfDescr_1);
    }

    if(strlen(argv[index+argDescription]) > L7_NIM_IF_ALIAS_SIZE)
    {
      /* port selected cannot perform this function */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_IntfDescrLen);

    }
    OSAPI_STRNCPY_SAFE(buf, argv[index+argDescription]);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      /*check to see if any argument was passed */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IntfNoDescr);
    }

    OSAPI_STRNCPY_SAFE(buf, "");
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }
        rc = usmDbIfAliasSet(unit, interface, buf);
    
        if (rc == L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          status = L7_FAILURE;
        }
        else if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_IntfDecsrSetting);
          status = L7_FAILURE;
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  to set the hostname/prompt for the switch
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes this value is only saved in ram currently and any settings are lost accross a reset
 * @notes on other boxes this is really setting the hostname
 *
 * @cmdsyntax  hostname <hostname>
 *
 * @cmdhelp
 *
 * @cmddescript
 *   This commands is used to change the prompt for the switch.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandHostname(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_char8 temp[L7_PROMPT_SIZE];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 arg1 = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_HostName_1);
    }
    if (strlen(argv[index+arg1]) >= sizeof(temp))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPrompt);
    }

    OSAPI_STRNCPY_SAFE(temp, (L7_char8 *)argv[index+arg1]);
    if (strlen((L7_char8 *)argv[index+arg1]) >= L7_PROMPT_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_HostName_1);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_HostName_1);
    }
    /*copy the default command prompt into temp*/
    OSAPI_STRNCPY_SAFE(temp, FD_CLI_WEB_COMMAND_PROMPT);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    osapiSnprintf(cliUtil.systemPrompt, sizeof(cliUtil.systemPrompt), "\r\n(%s) ", temp);
    rc = usmDbCommandPromptSet(unit, temp);
    osapiSnprintf(cliCommon[cliUtil.handleNum].prompt, sizeof(cliCommon[cliUtil.handleNum].prompt), "%s%s",cliUtil.systemPrompt, pStrInfo_common_PriviledgeUsrExecModePrompt);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable/disbale the telnet server
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  (no) ip telnet server enable
 *
 * @cmdhelp Configure the number of telnet sessions
 *
 * @cmddescript
 *   Enabling the telnet server set the max sessions for telnet to 5
 *   No command set the telnet server max sessions for telnet to 0.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandIpTelnetServerEnable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 mode = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 i;
  L7_uint32 sessionType;
  L7_BOOL   validLogin = L7_FALSE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IpTelnetSrvrEnbl);
    }
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IpTelnetSrvrDsbl);
    }
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbAgentTelnetAdminModeSet(unit, mode);
    if( rc == L7_SUCCESS)
    {
      if( ewsContext->commType == CLI_NO_CMD )  /* Flag all telnet connections to be disconnected on a no command */
      {
        for (i = 0; i < FD_CLI_DEFAULT_MAX_CONNECTIONS; i++)
        {
          usmDbLoginSessionValidEntry(unit, i, &validLogin);
          if( validLogin == L7_TRUE)
          {
            rc = usmDbLoginSessionTypeGet(unit, i, &sessionType);
            if( rc == L7_SUCCESS)
            {
              if( sessionType == L7_LOGIN_TYPE_TELNET)
              {
                cliWebLoginSessionResetConnectionSet(i);
              }
            }
          }
        }
      }
    }
    else
      if(rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_UnableToSet);
      }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
 *
 * @purpose  Sets the terminal length for scrolling in show running-config
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  [no]terminal length <5-48>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandSetTerminalLength(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 termLength;
  L7_uint32 numArg;
  L7_uint32 argLength = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTerminalLen_1);
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+argLength], &termLength) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTerminalLen_1);
    }
    if(termLength != 0)
    {
      if ((termLength < L7_MIN_TERMINAL_LENGTH) || (termLength > L7_MAX_TERMINAL_LENGTH))
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_FailedToSet, ewsContext, pStrErr_base_SetTerminalLen);
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_TerminalLenMustBeInRangeOfTo,
            L7_MIN_TERMINAL_LENGTH,
            L7_MAX_TERMINAL_LENGTH);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgTerminalLenNo);
    }
    termLength = FD_CLI_LINES_FOR_PAGINATION;
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if(usmDbTerminalLineSet(unit,termLength) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrErr_base_SetTerminalLen);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}
/*********************************************************************
 *
 * @purpose  enable/disable the lacpmode for a port
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  [no] lacp timeout {actor | partner} {short | long}
 *
 * @cmdhelp Set the LACP timeout for this port.
 *
 * @cmddescript
 *   This is a configurable value and can be set to "short" (default)
 *   (3 seconds) or "long" (90 seconds). The default value of this
 *   parameter is "short".
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortLacpTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t argDest = 1;
  L7_RC_t argLength = 2;
  L7_uint32 interface, itype; /*nextInterface;*/
  L7_uint32 s, p;
  L7_uchar8 val = 0, new_val = 0;
  L7_uchar8 currentVal;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 dest[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 length[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeout_1);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeoutNo);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeout_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    osapiStrncpySafe(dest, argv[index+argDest], sizeof(dest));
    osapiStrncpySafe(length, argv[index+argLength], sizeof(length));

    if ((cliNoCaseCompare(dest, pStrInfo_base_Actor) == L7_FALSE &&
          cliNoCaseCompare(dest, pStrInfo_base_Partner) == L7_FALSE) ||
        (cliNoCaseCompare(length, pStrInfo_base_Short_1) == L7_FALSE &&
         cliNoCaseCompare(length, pStrInfo_base_Long) == L7_FALSE))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeout_1);
    }
    if (cliNoCaseCompare(length, pStrInfo_base_Short_1) == L7_TRUE)
    {
      new_val = DOT3AD_STATE_LACP_TIMEOUT;
    }                                  /*  set to 0x2 if short */
  }
  else
  {

    osapiStrncpySafe(dest, argv[index+argDest], sizeof(dest));

    if (cliNoCaseCompare(dest, pStrInfo_base_Actor) == L7_FALSE &&
        cliNoCaseCompare(dest, pStrInfo_base_Partner) == L7_FALSE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeoutNo);
    }
    new_val = DOT3AD_STATE_LACP_TIMEOUT; /*  set to default value of 0x2 if short*/
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if (usmDbIntfTypeGet(interface, &itype) != L7_SUCCESS || itype != L7_PHYSICAL_INTF)
        {
          memset (buf, 0, sizeof(buf));
          osapiSnprintf(buf, sizeof(buf), " %s ", cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          ewsTelnetWrite( ewsContext, buf);
          status = L7_FAILURE;
          continue;
        }

        val = new_val;

        if (cliNoCaseCompare(dest, pStrInfo_base_Actor) == L7_TRUE)
        {
          rc = usmDbDot3adAggPortActorAdminStateGet(unit, interface, (L7_uchar8 *) &currentVal);
          if (val == (L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT)
          {
            val = (L7_uchar8) (currentVal | val);
          }
          else
          {
            val = (L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT;
            val = (L7_uchar8) (currentVal & (~val));
          }

          rc = usmDbDot3adAggPortActorAdminStateSet(unit, interface, (L7_uchar8 *) &val);
        }
        if (cliNoCaseCompare(dest, pStrInfo_base_Partner) == L7_TRUE)
        {
          rc = usmDbDot3adAggPortPartnerAdminStateGet(unit, interface, (L7_uchar8 *) &currentVal);
          if (val == (L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT)
          {
            val = (L7_uchar8) (currentVal | val);
          }
          else
          {
            val = (L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT;
            val = (L7_uchar8) (currentVal & (~val));
          }
          rc = usmDbDot3adAggPortPartnerAdminStateSet(unit, interface, (L7_uchar8 *) &val);
        }

        if (rc == L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          memset (buf, 0, sizeof(buf));
          osapiSnprintf(buf, sizeof(buf), " %s ", cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          ewsTelnetWrite( ewsContext, buf);
          status = L7_FAILURE;
          continue;
        }
        else if (rc != L7_SUCCESS)
        {
          memset (buf, 0, sizeof(buf));
          osapiSnprintf(buf, sizeof(buf), " %s ", cliDisplayInterfaceHelp(unit, s, p));

          if (ewsContext->commType == CLI_NORMAL_CMD)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeout_1);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeoutNo);
          }
          ewsTelnetWrite( ewsContext, buf);
          status = L7_FAILURE;
        }
      }
    }
  }

  if (status == L7_SUCCESS)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  enable/disable the lacpmode for all ports
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  port lacpmode all
 *
 * @cmdhelp Enable/Disable LACP mode for all ports.
 *
 * @cmddescript
 *   This is a configurable value and can be Enabled or Disabled. It
 *   allows you to enable or disable the Link Aggregation Control
 *   Protocol (LACP) mode by port. The default value of this
 *   parameter is disabled.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandPortLacpTimeoutAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_RC_t argDest = 1;
  L7_RC_t argLength = 2;
  L7_uint32 interface, nextInterface;
  L7_uchar8 currentVal;
  L7_uchar8 val = 0, originalVal = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_char8 dest[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 length[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 itype;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeout_1);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeoutNo);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeout_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    osapiStrncpySafe(dest, argv[index+argDest], sizeof(dest));
    osapiStrncpySafe(length, argv[index+argLength], sizeof(length));

    if ((cliNoCaseCompare(dest, pStrInfo_base_Actor) == L7_FALSE && cliNoCaseCompare(dest, pStrInfo_base_Partner) == L7_FALSE) ||
        (cliNoCaseCompare(length, pStrInfo_base_Short_1) == L7_FALSE && cliNoCaseCompare(length, pStrInfo_base_Long) == L7_FALSE))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeout_1);
    }
    if (cliNoCaseCompare(length, pStrInfo_base_Short_1) == L7_TRUE)
    {
      val = DOT3AD_STATE_LACP_TIMEOUT; /*  set to 0x2 if short, defaults to 0x0 if long */
      originalVal = val;
    }

  }
  else
  {
    osapiStrncpySafe(dest, argv[index+argDest], sizeof(dest));

    if (cliNoCaseCompare(dest, pStrInfo_base_Actor) == L7_FALSE &&
        cliNoCaseCompare(dest, pStrInfo_base_Partner) == L7_FALSE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_PortLacpTimeoutNo);
    }
    val = DOT3AD_STATE_LACP_TIMEOUT; /*  set to default value of 0x2 if short*/
    originalVal = val;
  }

  /* if interface entered was 'all' */
  if (L7_SUCCESS != usmDbValidIntIfNumFirstGet(&interface))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox_1);
  }

  while (interface)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIntfTypeGet(interface, &itype) == L7_SUCCESS &&
          (itype == L7_PHYSICAL_INTF))
      {
        if (cliNoCaseCompare(dest, pStrInfo_base_Actor) == L7_TRUE)
        {
          rc = usmDbDot3adAggPortActorAdminStateGet(unit, interface, (L7_uchar8 *) &currentVal);
          if (val == (L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT)
          {
            val = (L7_uchar8) (currentVal | val);
          }
          else
          {
            val = (L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT;
            val = (L7_uchar8) (currentVal & (~val));
          }
          rc = usmDbDot3adAggPortActorAdminStateSet(unit, interface, (L7_uchar8 *) &val);
        }
        if (cliNoCaseCompare(dest, pStrInfo_base_Partner) == L7_TRUE)
        {
          rc = usmDbDot3adAggPortPartnerAdminStateGet(unit, interface, (L7_uchar8 *) &currentVal);
          if (val == (L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT)
          {
            val = (L7_uchar8) (currentVal | val);
          }
          else
          {
            val = (L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT;
            val = (L7_uchar8) (currentVal & (~val));
          }
          rc = usmDbDot3adAggPortPartnerAdminStateSet(unit, interface, (L7_uchar8 *) &val);
        }
      }
    }
    if (L7_SUCCESS != usmDbValidIntIfNumNext(interface, &nextInterface))
    {
      interface = 0;
    }
    else
    {
      interface = nextInterface;
      val = originalVal;
    }

  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  To enable the IP Domain Naming System (DNS)-based host name-to-address translation.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes  To enable or disable the DNS
* @cmdsyntax  ip domain lookup
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDomainLookup(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
  L7_BOOL enable = L7_FALSE;
  L7_uint32 numArg;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  numArg = cliNumFunctionArgsGet();
  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_IpDomainLookup);
  }

  /********* set the flag to enable or disable the DNS*******/
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    enable = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    enable = L7_FALSE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbDNSClientAdminModeSet(enable) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
             pStrErr_common_Error,  ewsContext, 
             pStrErr_base_UnableEnDisDns);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To define a default domain names.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  ip domain name <name>
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDomainName(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 argName = 1;
  L7_uint32 unit;
  L7_char8  name[DNS_DOMAIN_NAME_SIZE_MAX+1];
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg != 1) ||
        (strlen(argv[index + argName]) > DNS_DOMAIN_NAME_SIZE_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
             pStrErr_common_Error,  ewsContext, 
             pStrErr_base_UnableDefineDDname);
    }

    bzero(name, sizeof(name));
    osapiStrncpySafe(name, argv[index + argName], DNS_DOMAIN_NAME_SIZE_MAX + 1);     /*******Check if the Flag is Set for Execution*************/

    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbDNSClientDefaultDomainSet(name);
      if (rc == L7_NOT_SUPPORTED)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                               ewsContext, pStrErr_base_InvalidDomainName_space);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                               ewsContext, pStrErr_base_UnableDefineDDname);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
             pStrErr_common_IncorrectInput,  ewsContext, 
             pStrErr_base_NoIpDomainName);
    }
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbDNSClientDefaultDomainClear() != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
               pStrErr_common_IncorrectInput,  ewsContext, 
               pStrErr_base_NoIpDomainName);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Add a name server for the DNS client
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax for normal command:  ip name server server-address1 [server-address2.....server-address8]
*
* @cmdsyntax for no command: no ip name server [server-address1.....server-address8]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpNameServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_inet_addr_t serverAddr;
  L7_char8  strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg;
  L7_uint32 intCounter;
  L7_uint32 unit;
  L7_uint32 intIPaddr = 0;
  L7_RC_t rc;

  inetAddressReset(&serverAddr);
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();
  numArg = cliNumFunctionArgsGet();
   if (((numArg > 8) || (numArg < 1)) && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_IpNameServer);
  }

  /* If arguments are provided in 'no' mode*/
  if ((numArg > 8) && (ewsContext->commType == CLI_NO_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_NoIpNameServer);
  }

  if ((numArg == 0) && (ewsContext->commType == CLI_NO_CMD))
  {
    for (intCounter = 0; intCounter < L7_DNS_NAME_SERVER_ENTRIES; intCounter++)
    {
      /** for getting the first and succesive name server entries **/
      inetAddressReset(&serverAddr);
      rc = usmDbDNSClientNameServerEntryNextGet(&serverAddr);
      if (rc == L7_SUCCESS)
      {
        if (usmDbDNSClientNameServerEntryRemove(&serverAddr) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0,
                 pStrErr_common_Error,  ewsContext,
                 pStrErr_base_UnableRemoveServerEntry);
        }
      }
      else
      {
        /*************Set Flag for Script Successful******/
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
  }
   if (numArg > 0)
  {
    for (intCounter = 0; intCounter < numArg ; intCounter++)
    {
      if (strlen(argv[index+intCounter+1]) >= sizeof(strIPaddr))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
               ewsContext, pStrErr_base_InvalidIpAddr_1);
      }

      bzero( strIPaddr, sizeof(strIPaddr) );
      osapiStrncpySafe(strIPaddr, argv[index+intCounter+1], sizeof(strIPaddr));
      /* Convert the IP address into an inet Address */
      if (usmDbParseInetAddrFromStr(strIPaddr, &serverAddr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
               ewsContext, pStrErr_common_TacacsInValIp);
      }

#if !defined(L7_IPV6_PACKAGE) && !defined(L7_IPV6_MGMT_PACKAGE)
      if (L7_INET_GET_FAMILY(&serverAddr) == L7_AF_INET6)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
               ewsContext, pStrErr_common_TacacsInValIp);
      }
#endif
       /* IP address should be between 0.0.0.1 and 255.255.255.255 */
      if (L7_INET_GET_FAMILY(&serverAddr) == L7_AF_INET)
      {
        inetAddressGet(L7_AF_INET, &serverAddr, &intIPaddr);
        if ((intIPaddr < 1) || (intIPaddr > 0xFFFFFFFF))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
                 ewsContext, pStrErr_base_InvalidIpAddr_1);
        }
      }

      /* Set the DNS Servers*/
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (ewsContext->commType == CLI_NORMAL_CMD)
        {
          rc = usmDbDNSClientNameServerEntryAdd(&serverAddr);
          if (rc == L7_TABLE_IS_FULL)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0,
                   pStrErr_common_Error,  ewsContext,
                   pStrErr_base_NameServerEntryFull);
          }
          else if (rc != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0,
                   pStrErr_common_Error,  ewsContext,
                   pStrErr_base_UnableAddServerEntry);
          }
        }
        /* Reset the DNS Servers*/
        else if (ewsContext->commType == CLI_NO_CMD)
        {
          if (usmDbDNSClientNameServerEntryRemove(&serverAddr) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0,
                   pStrErr_common_Error,  ewsContext,
                   pStrErr_base_UnableRemoveServerEntry);
          }
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To define a host name to address mapping in the host cache
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax for normal command:  ip host <name> <IPaddress>
*
* @cmdsyntax for no command: no ip host <name>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpHostNameAddress(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_char8  strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  hostName[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_uint32 ipAddr = 0;
  L7_RC_t   rc = L7_FAILURE;
  L7_inet_addr_t inetAddr;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();
  numArg = cliNumFunctionArgsGet();
  inetAddressReset(&inetAddr);

  if (((numArg != 2)) && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_IpHostHostName);
  }
  /* If arguments are provided in 'no' mode*/
  if (((numArg != 1)) && (ewsContext->commType == CLI_NO_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_NoIpHostHostName);
  }

  if ((strlen(argv[index + 1]) > (DNS_DOMAIN_NAME_SIZE_MAX)) || (usmDbHostNameValidateWithSpace((L7_uchar8 *)argv[index + 1]) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                           ewsContext, pStrErr_base_InvalidHostName_space);
  }
  

  bzero(hostName, sizeof(hostName));
  osapiStrncpySafe(hostName, argv[index+1], sizeof(hostName));
  rc = usmDbDnsIpAddressValidate(hostName, &ipAddr);
  if (rc != L7_FAILURE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_InvalidHostName_space);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index + 2]) > sizeof(strIPaddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
             ewsContext, pStrErr_base_InvalidIpAddr_1);
    }
    bzero( strIPaddr, sizeof(strIPaddr) );
    osapiStrncpySafe(strIPaddr, argv[index+2], sizeof(strIPaddr));
    /* Convert the IP address into an inet Address */
    if (usmDbParseInetAddrFromStr(strIPaddr, &inetAddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
             ewsContext, pStrErr_common_TacacsInValIp);
    }
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbDNSClientStaticHostEntryAdd(hostName, &inetAddr);
      if (rc == L7_TABLE_IS_FULL)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, 
               pStrErr_common_Error,  ewsContext, 
               pStrErr_base_StaticCacheFull);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, 
               pStrErr_common_Error,  ewsContext, 
               pStrErr_base_UnableSetHostName);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbDNSClientStaticHostEntryRemove(hostName, L7_AF_INET) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0,
               pStrErr_common_Error,  ewsContext,
               pStrErr_base_UnableRemoveEntry);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To define the number of times of retry sending Domain Name
*           System queries.
*
* @param EwsContext     ewsContext
* @param L7_uint32      argc
* @param const L7_char8 **argv
* @param L7_uint32      index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  ip domain retry <number> (Global config)
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDomainRetry(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 argRetry = 1;
  L7_uint32 unit;
  L7_uint32 retries = L7_NULL;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  numArg = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 1))

  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_IpDomainRetry);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_NoIpDomainRetry);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argRetry],
                                         &retries) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
             ewsContext, pStrErr_base_InvalidRetryNumber);
    }

    if ((retries < L7_DNS_DOMAIN_RETRY_NUMBER_MIN) ||
        (retries > L7_DNS_DOMAIN_RETRY_NUMBER_MAX))
    {
      sprintf(buf, pStrErr_base_RetryOutOfRange, L7_DNS_DOMAIN_RETRY_NUMBER_MIN,
                   L7_DNS_DOMAIN_RETRY_NUMBER_MAX);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                             ewsContext, buf);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    retries = FD_DNS_NO_OF_RETRIES;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbDNSClientRetransmitsSet(retries) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
             ewsContext, pStrErr_base_UnableToSetRetryNum);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To define the amount of time to wait for a response to a DNS
*           query.
*
* @param EwsContext     ewsContext
* @param L7_uint32      argc
* @param const L7_char8 **argv
* @param L7_uint32      index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  ip domain timeout <seconds> (Global config)
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDomainTimeout(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 argTimeout = 1;
  L7_uint32 unit;
  L7_uint32 timeout = L7_NULL;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  numArg = cliNumFunctionArgsGet();

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 1))

  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_IpDomainTimeout);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_IpDomainTimeout);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argTimeout],
                                         &timeout) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
             ewsContext, pStrErr_base_InvalidDomainTimeout);
    }

    if ((timeout < L7_DNS_DOMAIN_TIMEOUT_MIN) ||
        (timeout > L7_DNS_DOMAIN_TIMEOUT_MAX))
    {
      sprintf(buf, pStrErr_base_TimeoutOutOfRange, L7_DNS_DOMAIN_TIMEOUT_MIN,
                   L7_DNS_DOMAIN_TIMEOUT_MAX);
      ewsTelnetWrite( ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    timeout = FD_DNS_CLIENT_QUERY_TIMEOUT;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbDNSClientQueryTimeoutSet(timeout) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
             ewsContext, pStrErr_base_UnableToSetTimeout);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To define a default domain name list.
*
* @param EwsContext     ewsContext
* @param L7_uint32      argc
* @param const L7_char8 **argv
* @param L7_uint32      index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  ip domain list (Global Config Mode).
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDomainList(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 argNameList = 1;
  L7_uint32 unit;
  L7_char8  nameList[L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX+1];
  L7_RC_t   rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  numArg = cliNumFunctionArgsGet();

  if ((numArg != 1) ||
      (strlen(argv[index + argNameList]) > L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX))
  {

    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_IpDomainList);
  }
  bzero(nameList, sizeof(nameList));
  osapiStrncpySafe(nameList, argv[index + argNameList],
                         L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX + 1);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbDNSClientDomainNameListSet(nameList);
      if (rc == L7_TABLE_IS_FULL)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
               ewsContext, pStrErr_base_DomainListTableFull);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_InvalidDomainName_space);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
               ewsContext, pStrErr_base_UnableToSetDoaminListName);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbDNSClientDomainNameListRemove(nameList) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
               ewsContext, pStrErr_base_UnableClearDomainList);
      }
    }
  }
/*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To trigger the ip address conflict detection.
*
* @param EwsContext     ewsContext
* @param L7_uint32      argc
* @param const L7_char8 **argv
* @param L7_uint32      index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  ip address-conflict-detect run (Global Config Mode).
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpAddrConflictDetect(EwsContext ewsContext, L7_uint32 argc,
                                            const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* 'ip address-conflict-detect run' command is issued */
      if (L7_SUCCESS != usmDbIPAddrConflictDetectRun())
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
               ewsContext, pStrErr_base_IpAddrConflictDetectError);
      }
    }
  }

/*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)

/*********************************************************************
*
* @purpose  To define a host name to ipv6 address mapping in the host cache
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *

* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax for normal command: ipv6 host <name> <ipv6 address>
*
* @cmdsyntax for no command:  no ipv6 host <name>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpv6HostNameAddress(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index)
{
  L7_char8  strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  hostName[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_inet_addr_t inetV6Addr;
  L7_RC_t   rc = L7_FAILURE;
  L7_inet_addr_t inetAddr;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();
  numArg = cliNumFunctionArgsGet();
  inetAddressReset(&inetAddr);
  inetAddressReset(&inetV6Addr);

  if (((numArg != 2)) && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_Ipv6HostHostName);
  }
  /* If arguments are provided in 'no' mode*/
  if (((numArg != 1)) && (ewsContext->commType == CLI_NO_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
           pStrErr_common_IncorrectInput,  ewsContext, 
           pStrErr_base_NoIpv6HostHostName);
  }

  if ((strlen(argv[index + 1]) > (DNS_DOMAIN_NAME_SIZE_MAX)) || (usmDbHostNameValidateWithSpace((L7_uchar8 *)argv[index + 1]) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_InvalidHostName_space);
  }

  bzero(hostName, sizeof(hostName));
  osapiStrncpySafe(hostName, argv[index+1], sizeof(hostName));
  rc = osapiInetPton(L7_AF_INET6, hostName, (L7_uchar8 *)&inetV6Addr);
  if (rc != L7_FAILURE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_InvalidHostName_space);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index + 2]) > sizeof(strIPaddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
             ewsContext, pStrErr_common_CfgIpv6InvalidPrefix);
    }
    bzero( strIPaddr, sizeof(strIPaddr) );
    osapiStrncpySafe(strIPaddr, argv[index+2], sizeof(strIPaddr));
    /* Convert the v6 address string into an inet Address */
    if (usmDbParseInetAddrFromStr(strIPaddr, &inetAddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, 
             ewsContext, pStrErr_common_CfgIpv6InvalidPrefix);
    }
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbDNSClientStaticHostEntryAdd(hostName, &inetAddr);
      if (rc == L7_TABLE_IS_FULL)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, 
               pStrErr_common_Error,  ewsContext, 
               pStrErr_base_StaticCacheFull);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, 
               pStrErr_common_Error,  ewsContext, 
               pStrErr_base_UnableSetHostName);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbDNSClientStaticHostEntryRemove(hostName, L7_AF_INET6) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0,
               pStrErr_common_Error,  ewsContext,
               pStrErr_base_UnableRemoveEntry);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}
#endif

#ifdef L7_ROUTING_PACKAGE
/*************************************************************************
 * @purpose to set the switch to release Ip given by Dhcp server for a 
 * specified interface
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  release dhcp <unit/slot/port>
 *
 * @cmdhelp Enter Interface in unit/slot/port format.
 * 
 * @cmddescript
 * The DHCP client sends a DHCP RELEASE message telling the DHCP server that
 * it no longer needs the IP address, and that the IP address can be
 * reassigned to another client. If the IPv4 address on the interface was not
 * assigned by DHCP, then the command fails and displays the following error
 * message:
 *      Interface does not have a DHCP originated address
 *  
 * @end
 ***************************************************************************/
const L7_char8 *commandReleaseDhcp(EwsContext ewsContext,
                                   L7_uint32 argc,
                                   const L7_char8 * * argv,
                                   L7_uint32 index)
{
  L7_uint32 argInterface = 1;
  L7_uint32 unit, numArgs, intIfNum;
  L7_uint32 u, s, p;
  L7_char8 strArg[L7_CLI_MAX_STRING_LENGTH];
  L7_INTF_IP_ADDR_METHOD_t method;

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();
   /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,
                                           ewsContext, pStrErr_common_UnitId_1);
  }

  if (numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                           ewsContext, pStrErr_base_CfgDhcpRel_1);
  }

  memset(strArg,0,sizeof(strArg));
  OSAPI_STRNCPY_SAFE(strArg, argv[index + argInterface]);

  if (strlen(argv[index+argInterface]) >= sizeof(strArg))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                           ewsContext, pStrErr_base_CfgDhcpRel_1);
  }

  if (numArgs == 1)
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(strArg, &u, &s, &p) == L7_SUCCESS)
      {
        if (usmDbIntIfNumFromUSPGet(u, s, p, &intIfNum) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, 
                                                 pStrErr_common_InvalidSlotPort_1);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 1, pStrErr_common_Error, ewsContext,
                                               pStrErr_common_InvalidSlotPort_1);
      }
    }
      /* check if its nonstacking interface */

    else if (cliSlotPortToInterface(strArg, &u, &s, &p, &intIfNum)!= L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 1, pStrErr_common_Error,ewsContext,
                                             pStrErr_common_InvalidSlotPort_1);
    }

    if (usmDbIpIntfExists(unit, intIfNum) != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 1, pStrErr_common_Error,ewsContext,
                                             pStrErr_common_InvalidRoutingIntf_1);
    }

    if(usmDbIpRtrIntfIpAddressMethodGet(intIfNum, &method) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                             ewsContext, pStrErr_base_IntfDsntDhcpAdd);
    }

    if(method == L7_INTF_IP_ADDR_METHOD_DHCP)
    {
     /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if(usmDbIntfIpAddressMethodSet (intIfNum, L7_INTF_IP_ADDR_RELEASE,
                                        L7_MGMT_IPPORT, L7_FALSE)
                                     != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                                 ewsContext, pStrErr_base_DhcpRelease);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                             ewsContext, pStrErr_base_IntfDsntDhcpAdd);
    }
  }

 /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
#endif /* L7_ROUTING_PACKAGE */

/*****************************************************************************************
 * @purpose to set the switch to renew Ip given by Dhcp server for a
 * specified interface
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax  renew dhcp {<unit/slot/port>|network-port|service-port}
 *
 * @cmdhelp Enter Interface in unit/slot/port format.
 *
 * @cmddescript
 * If the interface has a leased IPv4 address when this command is issued, the DHCP client 
 * sends a DHCP REQUEST message telling the DHCP server that it wants to continue using 
 * the IP address. If DHCP is enabled on the interface, but the interface does not 
 * currently have an IPv4 address (for example, if the address was previously released),
 * then the DHCP client sends a DISCOVER to acquire a new address. If DHCP is not enabled 
 * on the interface, then the command fails and displays the following error message:
 *                
 *                   DHCP is not enabled on this interface
 * @end
 ******************************************************************************************/
 const L7_char8 *commandRenewDhcp(EwsContext ewsContext,
                                  L7_uint32 argc,
                                  const L7_char8 * * argv,
                                  L7_uint32 index)
 {
   L7_uint32 argIntfPort = 1;
   L7_uint32 unit, numArgs;
   L7_uint32 intIfNum = 0;
   L7_uint32 oldval = 0;
 #ifdef L7_ROUTING_PACKAGE
   L7_uint32 u, s, p;
   L7_INTF_IP_ADDR_METHOD_t method;
 #endif
   L7_char8 strArg[L7_CLI_MAX_STRING_LENGTH];
   L7_MGMT_PORT_TYPE_t mgmtPortType;

   cliSyntaxTop(ewsContext);   
   numArgs = cliNumFunctionArgsGet();
   /*************Set Flag for Script Failed******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,
                                            ewsContext, pStrErr_common_UnitId_1);
   }

   if(numArgs != 1)
   {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                            ewsContext, pStrErr_base_CfgDhcpRenew_1);
   }

   if (strlen(argv[index+argIntfPort]) >= sizeof(strArg))
   {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                            ewsContext, pStrErr_base_CfgDhcpRenew_1);
   }

   if (strcmp(argv[index+argIntfPort], pStrInfo_common_NetworkPort) == 0)
   {
     if (usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(unit, &oldval) != L7_SUCCESS)
     {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                              ewsContext, pStrErr_base_DhcpnotEnabled);
     }
     if (oldval == L7_SYSCONFIG_MODE_DHCP)
     {
       mgmtPortType = L7_MGMT_NETWORKPORT;
     }
     else
     {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                            ewsContext, pStrErr_base_DhcpnotEnabled);
     }
   }
   else if (strcmp(argv[index+argIntfPort], pStrInfo_common_ServicePort) == 0)
   {
     if (usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(unit, &oldval) != L7_SUCCESS)
     {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                            ewsContext, pStrErr_base_DhcpnotEnabled);
     }
     if (oldval == L7_SYSCONFIG_MODE_DHCP)
     {
       mgmtPortType = L7_MGMT_SERVICEPORT;
     }
     else
     {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                            ewsContext, pStrErr_base_DhcpnotEnabled);
     }
   }
#ifdef L7_ROUTING_PACKAGE
   else
   {
     memset(strArg,0,sizeof(strArg));
     OSAPI_STRNCPY_SAFE(strArg, argv[index + argIntfPort]);

     if (cliIsStackingSupported() == L7_TRUE)
     {
       if (cliValidSpecificUSPCheck(strArg, &u, &s, &p) == L7_SUCCESS)
       {
         if (usmDbIntIfNumFromUSPGet(u, s, p, &intIfNum) != L7_SUCCESS)
         {
           return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                                 pStrErr_common_InvalidSlotPort_1);
         }
       }
       else
       {
         return cliSyntaxReturnPromptAddBlanks(1,1, 0, 1, pStrErr_common_Error,
                                               ewsContext, pStrErr_common_InvalidSlotPort_1);
       }
     }
     /* check if its nonstacking interface */
     else if (cliSlotPortToInterface(strArg, &u, &s, &p, &intIfNum)!= L7_SUCCESS)
     {
       return cliSyntaxReturnPromptAddBlanks(1,1, 0, 1, pStrErr_common_Error,ewsContext,
                                             pStrErr_common_InvalidSlotPort_1);
     }
     else
     {
       /* Do Nothing */
     }
     if (usmDbIpIntfExists(unit, intIfNum) != L7_TRUE)
     {
       return cliSyntaxReturnPromptAddBlanks (1,1, 0, 1, pStrErr_common_Error,ewsContext,
                                              pStrErr_common_InvalidRoutingIntf_1);
     }
     if(usmDbIpRtrIntfIpAddressMethodGet(intIfNum, &method) != L7_SUCCESS)
     {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                              ewsContext, pStrErr_base_DhcpnotEnabled);
     }
     if (method == L7_INTF_IP_ADDR_METHOD_DHCP)
     {
       mgmtPortType = L7_MGMT_IPPORT;
     }
     else
     {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                              ewsContext, pStrErr_base_DhcpnotEnabled);
     }
   }
#else
   else
   {
     return cliSyntaxReturnPromptAddBlanks(1,1, 0, 1, pStrErr_common_Error,ewsContext,
                                           pStrErr_common_InvalidSlotPort_1);
   }
#endif /* L7_ROUTING_PACKAGE */

   /*******Check if the Flag is Set for Execution*************/
   if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
   {
     if (usmDbIntfIpAddressMethodSet (intIfNum, L7_INTF_IP_ADDR_RENEW,
                                      mgmtPortType, L7_FALSE)
                                   != L7_SUCCESS)
     {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                              ewsContext, pStrErr_base_DhcpRenew);
     }
   }
   /*************Set Flag for Script Successful******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
   return cliPrompt(ewsContext);
 }

/*********************************************************************
 *
 * @purpose  Sets the CPU Memory monitoring threshold
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @Mode  Global Config
 *
 * @cmdsyntax  memory free low-watermark processor <value>
 * @cmdsyntax for no command: no memory free low-watermark processor
 *
 * @cmdhelp Set the cpu free memory monitoring threshold
 *
 * @cmddescript Setting a value of 0 for threshold disables the memory 
 *              monitoring (same effect as running "no" command).
 *  
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandGlobalCpuMemoryThreshold(EwsContext ewsContext, L7_uint32 argc,
                                                const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 threshold = 0, numArg;
  L7_uint32 argThreshold = 4, unit;
  L7_RC_t   rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if ( unit == 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, 
                                           ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 4)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0,
                                             pStrErr_common_IncorrectInput,
                                             ewsContext, 
                                             pStrInfo_base_UseCpuMemoryThreshold);
    }

    if ((cliConvertTo32BitUnsignedInteger(argv[index+argThreshold], &threshold) != L7_SUCCESS))
    {
      return cliPrompt(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 3)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,
                                             ewsContext, pStrInfo_base_UseNoCpuMemoryThreshold);
    }
    
    threshold = 0;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0,
                                           pStrErr_common_IncorrectInput,
                                           ewsContext, 
                                           pStrInfo_base_UseCpuMemoryThreshold);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmdbCpuFreeMemoryThresholdSet(unit, threshold);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_REQUEST_DENIED)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot, 
                                               ewsContext, pStrErr_base_MemoryThreshold_BiggerTotalMem);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot, 
                                               ewsContext, pStrErr_base_SetMemoryThreshold);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}


/*********************************************************************
 *
 * @purpose  Sets the CPU Utilization monitoring thresholds
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @Mode  Global Config
 *
 * @cmdsyntax  process cpu threshold type total rising <val> interval <val> [falling 
 *             <val> interval <val>]
 * @cmdsyntax for no command: no process cpu threshold type total
 *
 * @cmdhelp 
 *
 * @cmddescript 
 *  
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandGlobalCpuUtilizationThreshold(EwsContext ewsContext, 
                                                     L7_uint32 argc,
                                                     const L7_char8 **argv, 
                                                     L7_uint32 index)
{
  L7_uint32 risingThr, risingPeriod, fallingThr, fallingPeriod, numArg;
  L7_uint32 argRisingThr = 6, argRisingPeriod = 8, argFallingThr = 10;
  L7_uint32 argFallingPeriod = 12, unit;
  L7_RC_t   rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if ( unit == 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, 
                                           ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg != 8) && (numArg != 12))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0,
                                             pStrErr_common_IncorrectInput,
                                             ewsContext, 
                                             pStrInfo_base_UseCpuUtilThreshold);
    }

    if ((cliConvertTo32BitUnsignedInteger(argv[index+argRisingThr], &risingThr) != L7_SUCCESS))
    {
      return cliPrompt(ewsContext);
    }

    if ((cliConvertTo32BitUnsignedInteger(argv[index+argRisingPeriod], &risingPeriod) != L7_SUCCESS))
    {
      return cliPrompt(ewsContext);
    }

    if ((risingPeriod % FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD) != 0)
    {
        
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0,
                                             pStrErr_common_IncorrectInput,
                                             ewsContext, 
                                             pStrErr_base_CpuUtilPeriod,
                                             FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD);
    }

    if (numArg == 12)
    {
      if ((cliConvertTo32BitUnsignedInteger(argv[index+argFallingThr], &fallingThr) != L7_SUCCESS))
      {
        return cliPrompt(ewsContext);
      }

      if ((cliConvertTo32BitUnsignedInteger(argv[index+argFallingPeriod], &fallingPeriod) != L7_SUCCESS))
      {
        return cliPrompt(ewsContext);
      }

      if ((fallingPeriod % FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD) != 0)
      {
        
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0,
                                               pStrErr_common_IncorrectInput,
                                               ewsContext, 
                                               pStrErr_base_CpuUtilPeriod,
                                               FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD);
      }

      if (risingThr < fallingThr)
      {
        
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0,
                                               pStrErr_common_IncorrectInput,
                                               ewsContext, 
                                               pStrErr_base_CpuUtilThr);
      }

    }
    else
    {
      fallingThr = risingThr;
      fallingPeriod = risingPeriod;
    }
    
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 4)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, 
                                             pStrErr_common_IncorrectInput,
                                             ewsContext, 
                                             pStrInfo_base_UseNoCpuUtilThreshold);
    }
    
    risingThr = 0;
    risingPeriod = 0;
    fallingThr = 0;
    fallingPeriod = 0;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0,
                                           pStrErr_common_IncorrectInput,
                                           ewsContext, 
                                           pStrInfo_base_UseCpuUtilThreshold);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmdbCpuUtilMonitorParamSet(unit, 
                                     SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM,
                                     fallingPeriod);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot, 
                                             ewsContext, pStrErr_base_SetCpuUtilThreshold);
    }

    rc = usmdbCpuUtilMonitorParamSet(unit, 
                                     SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM,
                                     fallingThr);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot, 
                                             ewsContext, pStrErr_base_SetCpuUtilThreshold);
    }

    rc = usmdbCpuUtilMonitorParamSet(unit, 
                                     SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM,
                                     risingThr);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot, 
                                             ewsContext, pStrErr_base_SetCpuUtilThreshold);
    }

    rc = usmdbCpuUtilMonitorParamSet(unit, 
                                     SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM,
                                     risingPeriod);

    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, pStrErr_common_CouldNot, 
                                             ewsContext, pStrErr_base_SetCpuUtilThreshold);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose    Set the SDM template to be used on the next reboot.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  sdm prefer { dual-ipv4-and-ipv6 default | ipv4-routing { default | data-center } }
*
* @cmdsyntax for no command:      no sdm prefer
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandSdmPrefer(EwsContext ewsContext, L7_uint32 argc, 
                                 const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 sdmGroupPos = 1;     /* index of keyword that selects template group */
  L7_uint32 sdmTemplatePos = 2;  /* index of keyword that specifies template within group */
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg;
  L7_uchar8 *syntax = "Use 'sdm prefer {dual-ipv4-and-ipv6 default | ipv4-routing {default | data-center}}'.";
  L7_uchar8 *noSyntax = "Use 'no sdm prefer'.";
  L7_uchar8 *failureMsg = "Failed to set the SDM template.";
  sdmTemplateId_t templateId = SDM_TEMPLATE_NONE;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Validate number of arguments */
  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 2))
  {
    osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), syntax);
    ewsTelnetWrite(ewsContext, buf);
    return cliPrompt(ewsContext);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), noSyntax);
    ewsTelnetWrite(ewsContext, buf);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strcmp(argv[index + sdmGroupPos], "dual-ipv4-and-ipv6") == 0)
    {
      if (strcmp(argv[index + sdmTemplatePos], "default") == 0)
      {
        templateId = SDM_TEMPLATE_DUAL_DEFAULT;
      }
      else
      {
        osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), syntax);
        ewsTelnetWrite(ewsContext, buf);
        return cliPrompt(ewsContext);
      }
    }
    else if (strcmp(argv[index + sdmGroupPos], "ipv4-routing") == 0)
    {
      if (strcmp(argv[index + sdmTemplatePos], "default") == 0)
      {
        templateId = SDM_TEMPLATE_V4_DEFAULT;
      }
      else if (strcmp(argv[index + sdmTemplatePos], "data-center") == 0)
      {
        templateId = SDM_TEMPLATE_V4_DATA_CENTER;
      }
      else
      {
        osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), syntax);
        ewsTelnetWrite(ewsContext, buf);
        return cliPrompt(ewsContext);
      }
    }
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbSdmNextActiveTemplateSet(templateId) == L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, "Changes to the running SDM preferences have been stored, "
                       "but cannot take effect until the next reload.");
        cliSyntaxBottom(ewsContext);
        ewsTelnetWrite(ewsContext, "Use 'show sdm prefer' to see what SDM preference is currently active.");
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      else
      {
        ewsTelnetWrite(ewsContext, failureMsg);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
  }  
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbSdmNextActiveTemplateSet(SDM_TEMPLATE_NONE) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, "Failed to clear the next active SDM template.");
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
