/*********************************************************************
 
 *(C) Copyright Broadcom Corporation 2000-2005
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/dhcp_snooping/cli_show_running_config_dhcp_snooping.c
 *
 * @purpose show running config commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  3/22/2007
 *
 * @author  rrice
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "dhcp_snooping_exports.h"
#include "usmdb_util_api.h"
#include "comm_mask.h"
#include "config_script_api.h"
#include "datatypes.h"
#ifndef _L7_OS_LINUX_
  #include <inetLib.h>    /* for converting from IP to L7_int32 */
#endif /* _L7_OS_LINUX_ */

#include "usmdb_dhcp_snooping.h"
#include "clicommands_dhcp_snooping.h"
#include "cli_show_running_config.h"

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
 * @purpose  To print the running configuration of IPSG
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 *
 * @notes
 *
 * @end
 **********************************************************************/
L7_RC_t cliRunningConfigIpsgInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intIfNum = 0;
  L7_ushort16 vlanId = 0;
  L7_uint32 ipAddr = 0;
  L7_uint32 intUnit = 0;
  L7_uint32 intSlot = 0;
  L7_uint32 intPort = 0;
  L7_enetMacAddr_t macAddr;
  L7_uint32 entryType = 0;
 
  L7_uchar8 ifName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 ipAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 macStr[L7_CLI_MAX_STRING_LENGTH]; 

 
  memset(&macAddr, 0, sizeof(macAddr));
  while (usmDbIpsgBindingGetNext(&intIfNum,
                                 &vlanId, 
                                 &ipAddr,
                                 &macAddr,
                                 &entryType) == L7_SUCCESS)
  {

    if (entryType == IPSG_ENTRY_DYNAMIC)
    {
      continue;
    } 
    memset (ifName, 0, sizeof(ifName));
    if (usmDbUnitSlotPortGet(intIfNum, &intUnit, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(ifName, sizeof(ifName),
                    cliDisplayInterfaceHelp(intUnit, intSlot, intPort));
    }
    else
    {
      osapiSnprintf(ifName, sizeof(ifName), pStrErr_common_Err);
      break;
    }


    osapiSnprintf(macStr, sizeof(macStr),"%02X:%02X:%02X:%02X:%02X:%02X",
                  macAddr.addr[0], macAddr.addr[1], macAddr.addr[2],
                  macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);   

    usmDbInetNtoa(ipAddr, ipAddrStr);
    
    osapiSnprintf(buf, sizeof(buf), "\r\n%s %s vlan %u %s interface %s",
                     pStrInfo_base_Ipsg,
                     macStr,
                     vlanId,
                     ipAddrStr,
                     ifName);
    EWSWRITEBUFFER (ewsContext, buf);

  }

  return L7_SUCCESS;

}
#endif

/*********************************************************************
 * @purpose  To print the running configuration of dhcp snooping
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 *
 * @notes
 *
 * @end
 **********************************************************************/
L7_RC_t cliRunningConfigDhcpSnoopingInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_uint32 vlanStart = 0;
  L7_uint32 vlanEnd = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 fileName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL verifyMac;
  L7_RC_t rc = L7_SUCCESS;
  dhcpSnoopBinding_t binding;
  L7_uchar8 ifName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 macStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32  intSlot, intPort;
  L7_uint32  first_time = 1;
  L7_char8  vlanStr[L7_CLI_MAX_STRING_LENGTH];
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  L7_uchar8 remoteIdStr[DS_MAX_REMOTE_ID_STRING]; 
  L7_uint32 vlanNext = 0;
#endif
  L7_char8  *dsVlanShow = "ip dhcp snooping vlan %s";


  /* ip dhcp snooping */
  if (usmDbDsAdminModeGet(&val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, val, FD_DHCP_SNOOP_ADMIN_MODE, 
                     pStrInfo_base_IpDhcpSnooping);
  }

  /* ip dhcp snooping vlan <vlan-range> */
  memset(vlanStr, 0, sizeof(vlanStr));
  rc = usmDbDsNextEnabledVlanRangeGet(&vlanStart, &vlanEnd);
  while ((rc == L7_SUCCESS) && vlanStart)
  {
    if (vlanEnd > vlanStart)
    {
      if (first_time)
      {
        osapiSnprintf(buf, sizeof(buf), "%d-%d", vlanStart, vlanEnd);
        first_time = 0;
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), ",%d-%d", vlanStart, vlanEnd);
      }

    }
    else
    {
      if (first_time)
      {
        osapiSnprintf(buf, sizeof(buf), "%d", vlanStart);
        first_time = 0;
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), ",%d", vlanStart);
      }
    }
    osapiStrncat(vlanStr, buf, strlen(buf));
    if ((strlen(vlanStr) + strlen(dsVlanShow) - 2) >  /* -2 is for %s */
        (CLI_CONSOLE_WIDTH - 11)) /* 11 is max number of chars for a range */
    {
      osapiSnprintf(buf, sizeof(buf), dsVlanShow, vlanStr);
      EWSWRITEBUFFER (ewsContext, buf);
      memset(vlanStr, 0, sizeof(vlanStr));
      first_time = 1;
    }

    rc = usmDbDsNextEnabledVlanRangeGet(&vlanStart, &vlanEnd);
  }

  if(vlanStr[0] != 0)
  {
    osapiSnprintf(buf, sizeof(buf), dsVlanShow, vlanStr);
    EWSWRITEBUFFER (ewsContext, buf);
    memset(vlanStr, 0, sizeof(vlanStr));
  }


  if ( (usmDbDsDbIpAddrGet(&val)== L7_SUCCESS) &&
       val > 0 )
  {
    usmDbInetNtoa(val, strIpAddr);
    if ( usmDbDsDbFileNameGet(fileName) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), "\r\n%s tftp://%s/%s",
                    pStrInfo_base_IpDhcpSnooping_DB,
                    strIpAddr,
                    fileName);
    }

    EWSWRITEBUFFER (ewsContext, buf);
  }
  if ( (usmDbDsDbStoreIntervalGet(&val)== L7_SUCCESS) &&
       val > 0 )
  {
    if ( val != 300)
    {
      osapiSnprintf(buf, sizeof(buf), "\r\n%s write-delay %d",
                  pStrInfo_base_IpDhcpSnooping_DB,
                  val);
       EWSWRITEBUFFER (ewsContext, buf);
    }
  }


  /* ip dhcp snooping verify mac-address */
  if (usmDbDsVerifyMacGet(&verifyMac) == L7_SUCCESS)
  {
    L7_char8 cmdStr[L7_CLI_MAX_STRING_LENGTH];
    osapiSnprintf(cmdStr, sizeof(cmdStr), "%s %s", 
                      pStrInfo_base_IpDhcpSnooping,
                      pStrInfo_base_IpDhcpSnoopingVerifyMac);
    cliShowCmdTrue(ewsContext, verifyMac, FD_DHCP_SNOOP_VERIFY_MAC, cmdStr);
  }


  memset(&binding.key.macAddr, 0, L7_MAC_ADDR_LEN);
  rc = usmDbDsBindingGetNext(&binding);

  while (rc  == L7_SUCCESS)
  {
    /* skip tentative bindings & Dynamic bindings */
    if ( (binding.bindingType == DS_BINDING_TENTATIVE) ||
         (binding.bindingType == DS_BINDING_DYNAMIC)
       )
    {
      rc =  usmDbDsBindingGetNext(&binding);
      continue;
    }
     /* MAC address */
    osapiSnprintf(macStr, sizeof(macStr),"%02X:%02X:%02X:%02X:%02X:%02X",
                  binding.key.macAddr[0], binding.key.macAddr[1], binding.key.macAddr[2],
                  binding.key.macAddr[3], binding.key.macAddr[4], binding.key.macAddr[5]);

    /* IP address */
    usmDbInetNtoa(binding.ipAddr, strIpAddr);
    
    /* Interface */
    if (usmDbUnitSlotPortGet(binding.intIfNum, &unit, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(ifName, sizeof(ifName),
                    cliDisplayInterfaceHelp(unit, intSlot, intPort));
    }
    else
    {
      break;
    }
    osapiSnprintf(buf, sizeof(buf), "\r\n%s binding  %s vlan %u %s interface %s",
                     pStrInfo_base_IpDhcpSnooping,
                     macStr,
                     binding.vlanId,
                     strIpAddr,
                     ifName);
    EWSWRITEBUFFER (ewsContext, buf);
    rc = usmDbDsBindingGetNext(&binding);
  }
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  /* The DHCP L2 relay Global Configuration.*/
  /* dhcp l2relay*/
  if (usmDbDsL2RelayAdminModeGet(&val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, val, FD_DHCP_L2RELAY_ADMIN_MODE, 
                     pStrInfo_switching_DhcpL2RelayModeShow);
  }
  /* dhcp l2relay vlan <vlan-list>.*/
  memset(vlanStr, 0, sizeof(vlanStr));
  first_time = 1;
  vlanStart = vlanEnd = 0;
  rc = usmDbDsL2RelayVlanRangeNextGet(&vlanStart, &vlanEnd);
  while ((rc == L7_SUCCESS) && vlanStart)
  {
    if (vlanEnd > vlanStart)
    {
      if (first_time)
      {
        osapiSnprintf(buf, sizeof(buf), "%d-%d", vlanStart, vlanEnd);
        first_time = 0;
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), ",%d-%d", vlanStart, vlanEnd);
      }

    }
    else
    {
      if (first_time)
      {
        osapiSnprintf(buf, sizeof(buf), "%d", vlanStart);
        first_time = 0;
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), ",%d", vlanStart);
      }
    }
    osapiStrncat(vlanStr, buf, strlen(buf));
    if ((strlen(vlanStr) + strlen(pStrInfo_switching_DhcpL2RelayVlanShow) - 2) >  /* -2 is for %s */
        (CLI_CONSOLE_WIDTH - 11)) /* 11 is max number of chars for a range */
    {
      osapiSnprintf(buf, sizeof(buf), pStrInfo_switching_DhcpL2RelayVlanShow, vlanStr);
      EWSWRITEBUFFER (ewsContext, buf);
      memset(vlanStr, 0, sizeof(vlanStr));
      first_time = 1;
    }

    rc = usmDbDsL2RelayVlanRangeNextGet(&vlanStart, &vlanEnd);
  }
  if(vlanStr[0] != 0)
  {
    osapiSnprintf(buf, sizeof(buf), pStrInfo_switching_DhcpL2RelayVlanShow, vlanStr);
    EWSWRITEBUFFER (ewsContext, buf);
    memset(vlanStr, 0, sizeof(vlanStr));
  }


  /* dhcp l2relay circuit-id vlan <vlan-list>.*/
  memset(vlanStr, 0, sizeof(vlanStr));
  first_time = 1;
  vlanStart = vlanEnd = 0;
  rc = usmDbDsL2RelayCircuitIdVlanRangeNextGet(&vlanStart, &vlanEnd);
  while ((rc == L7_SUCCESS) && vlanStart)
  {
    if (vlanEnd > vlanStart)
    {
      if (first_time)
      {
        osapiSnprintf(buf, sizeof(buf), "%d-%d", vlanStart, vlanEnd);
        first_time = 0;
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), ",%d-%d", vlanStart, vlanEnd);
      }

    }
    else
    {
      if (first_time)
      {
        osapiSnprintf(buf, sizeof(buf), "%d", vlanStart);
        first_time = 0;
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), ",%d", vlanStart);
      }
    }
    osapiStrncat(vlanStr, buf, strlen(buf));
    if ((strlen(vlanStr) + strlen(pStrInfo_switching_DhcpL2RelayCIdShow) - 2) >  /* -2 is for %s */
        (CLI_CONSOLE_WIDTH - 11)) /* 11 is max number of chars for a range */
    {
      osapiSnprintf(buf, sizeof(buf), pStrInfo_switching_DhcpL2RelayCIdShow, vlanStr);
      EWSWRITEBUFFER (ewsContext, buf);
      memset(vlanStr, 0, sizeof(vlanStr));
      first_time = 1;
    }

    rc = usmDbDsL2RelayCircuitIdVlanRangeNextGet(&vlanStart, &vlanEnd);
  }
  if(vlanStr[0] != 0)
  {
    osapiSnprintf(buf, sizeof(buf), pStrInfo_switching_DhcpL2RelayCIdShow, vlanStr);
    EWSWRITEBUFFER (ewsContext, buf);
    memset(vlanStr, 0, sizeof(vlanStr));
  }
  /* dhcp l2relay remote-id <string> vlan <vlan-list> */
  rc = usmDbDsL2RelayRemoteIdVlanNextGet(&vlanNext, remoteIdStr);
  while ((rc == L7_SUCCESS) && (vlanNext != 0 && vlanNext <= L7_DOT1Q_MAX_VLAN_ID))
  {
    osapiSnprintf(buf, sizeof(buf), pStrInfo_switching_DhcpL2RelayRIdShow, remoteIdStr, vlanNext); 
    EWSWRITEBUFFER (ewsContext, buf);
    rc = usmDbDsL2RelayRemoteIdVlanNextGet(&vlanNext, remoteIdStr);
  }
  #endif
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Print the running configuration of DHCP snooping for an interface
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32  intIfNum       internal interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 **********************************************************************/
L7_RC_t cliRunningConfigDhcpSnoopingInterfaceInfo(EwsContext ewsContext,
                                                  L7_uint32 intIfNum)
{
  L7_BOOL trust;
  L7_BOOL log;
  L7_int32 rate;
  L7_uint32 burst;
  L7_char8 cmdStr[L7_CLI_MAX_STRING_LENGTH];

#ifdef L7_DHCP_L2_RELAY_PACKAGE
  L7_uint32 mode;
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  L7_uint32 intIfNumTemp =  intIfNum -1;
  L7_uchar8 subscName[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX+1];
  L7_uchar8 remoteIdStr[DS_MAX_REMOTE_ID_STRING+1];
#endif
#endif
  if ((usmDbDsIntfTrustGet(intIfNum, &trust)) == L7_SUCCESS)
  {
    osapiSnprintf(cmdStr, sizeof(cmdStr), "%s trust", 
                  pStrInfo_base_IpDhcpSnooping);
    cliShowCmdTrue(ewsContext, trust, FD_DHCP_SNOOP_TRUST, cmdStr);
  }

  if ((usmDbDsIntfLogInvalidGet(intIfNum, &log)) == L7_SUCCESS)
  {
    osapiSnprintf(cmdStr, sizeof(cmdStr), "%s log-invalid", 
                  pStrInfo_base_IpDhcpSnooping);
    cliShowCmdTrue(ewsContext, log, FD_DHCP_SNOOP_LOG_INVALID_MSG, cmdStr);
  }
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  /* DHCP L2 relay interface configuration.*/
  if ((usmDbDsL2RelayIntfModeGet(intIfNum, &mode)) == L7_SUCCESS)
  {
    osapiSnprintf(cmdStr, sizeof(cmdStr), "%s", 
                  pStrInfo_switching_DhcpL2RelayModeShow);
    cliShowCmdTrue(ewsContext, mode, FD_DHCP_L2RELAY_INTERFACE_MODE, cmdStr);
  }
  if ((usmDbDsL2RelayIntfTrustGet(intIfNum, &trust)) == L7_SUCCESS)
  {
    osapiSnprintf(cmdStr, sizeof(cmdStr), "%s trust", 
                  pStrInfo_switching_DhcpL2RelayModeShow);
    cliShowCmdTrue(ewsContext, trust, FD_DHCP_L2RELAY_INTERFACE_TRUST_MODE, cmdStr);
  }

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  memset(subscName, 0, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX );
  while (usmdbDsL2RelayIntfSubscriptionNextGet(&intIfNumTemp, subscName) == L7_SUCCESS)
  {
    if (intIfNumTemp > intIfNum)
    {
      break;
    }
    else if (intIfNumTemp == intIfNum)
    {
      if (usmdbDsL2RelaySubscripionModeGet(intIfNumTemp, subscName, &mode) == L7_SUCCESS)
      {
        osapiSnprintf(cmdStr, sizeof(cmdStr), "dhcp l2relay subscription %s", 
                        subscName);
        cliShowCmdEnable(ewsContext,mode, FD_DHCP_L2RELAY_SUBSCRIPTION_MODE, cmdStr);
      }

      if (usmdbDsL2RelaySubscripionCircuitIdModeGet(intIfNumTemp, subscName, &mode) == L7_SUCCESS)
      {
        osapiSnprintf(cmdStr, sizeof(cmdStr), "dhcp l2relay circuit-id subscription %s", 
                        subscName);
        cliShowCmdEnable(ewsContext,mode, FD_DHCP_L2RELAY_CIRCUIT_ID_SUBSCRIPTION_MODE, cmdStr);
      }

      if (usmdbDsL2RelaySubscripionRemoteIdModeGet(intIfNumTemp, subscName, remoteIdStr) == L7_SUCCESS)
      {
        if (osapiStrncmp(remoteIdStr, FD_DHCP_L2RELAY_REMOTE_ID_SUBSCRIPTION_MODE, 
                     DS_MAX_REMOTE_ID_STRING) != L7_NULL)
        {
          osapiSnprintf(cmdStr, sizeof(cmdStr), "dhcp l2relay remote-id %s subscription %s", 
                          remoteIdStr, subscName);
          EWSWRITEBUFFER (ewsContext, cmdStr);
        }
      }
    }
  }
#endif
#endif

  if ((usmDbDsIntfRateLimitGet(intIfNum, &rate) == L7_SUCCESS) &&
      (usmDbDsIntfBurstIntervalGet(intIfNum, &burst) == L7_SUCCESS))
  {
    if((rate == FD_DHCP_SNOOPING_RATE_LIMIT) && (burst == FD_DHCP_SNOOPING_BURST_INTERVAL))
    {
      if(EWSSHOWALL (ewsContext) == L7_SHOW_DEFAULT)
      {
        osapiSnprintf(cmdStr, sizeof(cmdStr), "no ip dhcp snooping limit");
        EWSWRITEBUFFER (ewsContext, cmdStr);
      }
    }
    else if(rate == L7_DS_RATE_LIMIT_NONE)
    {
      osapiSnprintf(cmdStr, sizeof(cmdStr), "ip dhcp snooping limit none");
      EWSWRITEBUFFER (ewsContext, cmdStr);
    }
    else
    {
      osapiSnprintf(cmdStr, sizeof(cmdStr), "ip dhcp snooping limit rate %d burst interval %d",rate, burst);
      EWSWRITEBUFFER (ewsContext, cmdStr);
    }
  }
  return L7_SUCCESS;
}

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
 * @purpose  Print the running configuration of IP source guard for an interface
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32  intIfNum       internal interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 **********************************************************************/
L7_RC_t cliRunningConfigIpsgInterfaceInfo(EwsContext ewsContext, L7_uint32 intIfNum)
{
  L7_char8 cmdStr[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL verifyIp = L7_FALSE;
  L7_BOOL verifyMac = L7_FALSE;

  if ((usmDbIpsgVerifySourceGet(intIfNum, &verifyIp, &verifyMac)) == L7_SUCCESS)
  {
    if (verifyIp != FD_IPSG_VERIFY_IP || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      if (verifyIp)
      {
          osapiSnprintf(cmdStr, sizeof(cmdStr), "\r\n%s", pStrInfo_base_IpVerifySource);
          if (verifyMac)
            strncat(cmdStr, " port-security", sizeof(cmdStr));
      }
      else
      {
        osapiSnprintf(cmdStr, sizeof(cmdStr), "\r\nno %s", pStrInfo_base_IpVerifySource);
      }
      EWSWRITEBUFFER(ewsContext, cmdStr);
    }
  }
  return L7_SUCCESS;
}

#endif
