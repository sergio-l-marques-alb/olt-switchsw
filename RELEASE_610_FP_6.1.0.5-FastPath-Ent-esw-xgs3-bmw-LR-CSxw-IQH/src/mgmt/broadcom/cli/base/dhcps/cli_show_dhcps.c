/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/cli_show_dhcps.c
*
* @purpose Dhcp server how commands for CLI
*
* @component user interface
*
* @comments none
*
* @create  05/03/2007
*
* @author  nshrivastav
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "dhcps_exports.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>

#include "usmdb_dhcps_api.h"
#include "clicommands_card.h"
#include "defaultconfig.h"

/*********************************************************************
*
* @purpose  To display address bindings on the DHCP server
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
* @cmdsyntax show ip dhcp binding  [address]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandShowDhcpsBinding(EwsContext ewsContext,
                                         L7_uint32 argc,
    const L7_char8 * * argv,
                                         L7_uint32 index)
{
   L7_uint32 intIpAddress;
   L7_uint32 argIpAddr = 1;
   L7_uint32 intLeaseIndex;
   L7_uint32 intState;
   L7_uint32 intMask;
   L7_uint32 intRemainingTime;
   L7_uint32 intLeaseTime = L7_NULL;
   L7_uint32 intPoolType, poolIndex;
   L7_uchar8 buffer[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  ipAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8  hwAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  clientId[L7_DHCPS_CLIENT_ID_MAXLEN+1];
   L7_char8  poolName[L7_DHCPS_POOL_NAME_MAXLEN];
   L7_uint32 unit;
   L7_uint32 numArg;
   L7_uint32 days=L7_NULL;
   L7_uint32 hours=L7_NULL;
   L7_uint32 minutes=L7_NULL;
   L7_RC_t   rc, rc_lease;

   cliSyntaxTop(ewsContext);

   unit = cliGetUnitId();
   numArg = cliNumFunctionArgsGet();

   if ((numArg < 0) || (numArg > 1))
   {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowIpDhcpBinding);

      return cliPrompt(ewsContext);
   }

   if (numArg == 1)
   {
    memset (ipAddr, 0,sizeof(ipAddr));
    if (strlen(argv[index+argIpAddr]) >= sizeof(ipAddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowIpDhcpBinding);
    }

    OSAPI_STRNCPY_SAFE(ipAddr, argv[index + argIpAddr]);

    /* Convert Ip address into 32 bit integer */
    if (usmDbInetAton(ipAddr, &intIpAddress) == L7_SUCCESS)
    {
      memset (hwAddr, 0,sizeof(hwAddr));
      if (usmDbDhcpsLeaseDataGet(unit, intIpAddress, &intMask, hwAddr, clientId, poolName, &intRemainingTime) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_BindingInfo);
      }

      intLeaseTime = intRemainingTime;

      /****converting the Remaining Time in days,hours,minutes format*****/

      intRemainingTime=intRemainingTime*SECONDS_PER_MINUTE;
      days=intRemainingTime/SECONDS_PER_DAY;
      intRemainingTime=intRemainingTime % SECONDS_PER_DAY;
      hours=intRemainingTime/SECONDS_PER_HOUR;
      intRemainingTime=intRemainingTime % SECONDS_PER_HOUR;
      minutes=intRemainingTime/SECONDS_PER_MINUTE;

      if (usmDbDhcpsPoolTypeGet(unit, poolName, &intPoolType) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }

      memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_IpAddrHardwareAddrLeaseExpirationType);
      ewsTelnetWrite( ewsContext, "\r\n-----------        -----------------    -----------------    -----");

      ewsTelnetPrintf (ewsContext, "\r\n%-19s",ipAddr);

      ewsTelnetPrintf (ewsContext, "%-21s",hwAddr);

      memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
      if (intLeaseTime > L7_DHCPS_LEASETIME_MAX)
      {
        osapiSnprintfAddBlanks (0, 0, 0, 8, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_base_Infinite);
      }
      else
      {
        osapiSnprintf(buffer,sizeof(buffer),"%02d:%02d:%02d        ",days,hours,minutes);
      }
      ewsTelnetWrite( ewsContext, buffer);

      memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
      if (intPoolType == L7_DHCPS_MANUAL_POOL)
      {
        osapiSnprintfAddBlanks (0, 0, 4, 0, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_common_Manual);
      }
      else if (intPoolType == L7_DHCPS_DYNAMIC_POOL)
      {
        osapiSnprintfAddBlanks (0, 0, 4, 0, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_base_Auto_1);
      }
      else
      {
        osapiSnprintfAddBlanks (0, 0, 4, 0, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_common_Unknown_1);
      }

      ewsTelnetWrite( ewsContext, buffer);
      return cliPrompt(ewsContext);
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_TacacsInValIp);
    }
  }

  memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_IpAddrHardwareAddrLeaseExpirationType);
   ewsTelnetWrite( ewsContext, "\r\n-----------        -----------------    -----------------    -----");

   rc = usmDbDhcpsPoolEntryFirst(unit,poolName,&poolIndex);

   while (rc == L7_SUCCESS)
   {
      rc_lease = usmDbDhcpsLeaseEntryFirst(unit,poolName, &intLeaseIndex, &intIpAddress, &intState);

      while (rc_lease == L7_SUCCESS)
      {
      memset (hwAddr, 0,sizeof(hwAddr));
         if (usmDbDhcpsLeaseDataGet(unit, intIpAddress, &intMask, hwAddr, clientId, poolName, &intRemainingTime) != L7_SUCCESS)
         {
            rc_lease = L7_FAILURE;
         }

         intLeaseTime = intRemainingTime;

         /****converting the Remaining Time in days,hours,minutes format*****/

         intRemainingTime=intRemainingTime*SECONDS_PER_MINUTE;
         days=intRemainingTime/SECONDS_PER_DAY;
         intRemainingTime=intRemainingTime % SECONDS_PER_DAY;
         hours=intRemainingTime/SECONDS_PER_HOUR;
         intRemainingTime=intRemainingTime % SECONDS_PER_HOUR;
         minutes=intRemainingTime/SECONDS_PER_MINUTE;

         if (usmDbInetNtoa( intIpAddress, ipAddr) != L7_SUCCESS)
         {
            rc_lease = L7_FAILURE;
         }

         if (usmDbDhcpsPoolTypeGet(unit, poolName, &intPoolType) != L7_SUCCESS)
         {
            rc_lease = L7_FAILURE;
         }

         if (rc_lease == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "\r\n%-19s",ipAddr);

        ewsTelnetPrintf (ewsContext, "%-21s",hwAddr);

        memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
        if (intLeaseTime > L7_DHCPS_LEASETIME_MAX)
        {
          osapiSnprintfAddBlanks (0, 0, 0, 8, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_base_Infinite);
        }
        else
        {
          osapiSnprintf(buffer,sizeof(buffer),"%02d:%02d:%02d        ",days,hours,minutes);
        }
        ewsTelnetWrite( ewsContext, buffer);

        memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
        if (intPoolType == L7_DHCPS_MANUAL_POOL)
        {
          osapiSnprintfAddBlanks (0, 0, 4, 0, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_common_Manual);
        }
        else if (intPoolType == L7_DHCPS_DYNAMIC_POOL)
        {
          osapiSnprintfAddBlanks (0, 0, 4, 0, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_base_Auto_1);
        }
        else
        {
          osapiSnprintfAddBlanks (0, 0, 4, 0, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_common_Unknown_1);
            }
            ewsTelnetWrite( ewsContext, buffer);
         }

         rc_lease = usmDbDhcpsLeaseEntryNext(unit,poolName, &intLeaseIndex, &intIpAddress, &intState);
      }

      rc = usmDbDhcpsPoolEntryNext(unit, poolName, &poolIndex);
   }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To display DHCP server statistics
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
* @cmdsyntax show ip dhcp server statistics
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandShowDhcpsStatistics(EwsContext ewsContext,
                                            L7_uint32 argc,
    const L7_char8 * * argv,
                                            L7_uint32 index)
{

   L7_uchar8 buffer[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 intActiveLeases;
   /*L7_uint32 intManualLeases;*/
   L7_uint32     intExpiredLeases;
   L7_uint32     intMalformedMessages;
   L7_uint32 intDhcpDiscover;
   L7_uint32 intDhcpRequest;
   L7_uint32     intDhcpDecline;
   L7_uint32     intDhcpRelease;
   L7_uint32 intDhcpInform;
   L7_uint32 intDhcpOffer;
   L7_uint32     intDhcpAck;
   L7_uint32     intDhcpNack;
   L7_uint32     unit;
   L7_uint32     numArg;

   cliSyntaxTop(ewsContext);

   unit = cliGetUnitId();
   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowIpDhcpSrvrStats);
   }

   if (usmDbDhcpsNoOfActiveLeasesGet(unit, &intActiveLeases) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetActiveLeases);
   }

   /*if(usmDbDhcpsNoOfManualLeasesGet(unit, &intManualLeases) != L7_SUCCESS)
   {
    return cliSyntaxReturnPrompt (ewsContext, CLIERROR_COULDNOTGETMANUALLEASES);
   }            */

   if (usmDbDhcpsNoOfExpiredLeasesGet(unit, &intExpiredLeases) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetExpiredLeases);
   }

   if (usmDbDhcpsMalformedGet(unit, &intMalformedMessages) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetMalformedMsgs);
   }

   if (usmDbDhcpsDiscoverReceivedGet(unit, &intDhcpDiscover) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetDhcpDiscover);
   }

   if (usmDbDhcpsRequestReceivedGet(unit, &intDhcpRequest) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetDhcpReq);
   }

   if (usmDbDhcpsDeclineReceivedGet(unit, &intDhcpDecline) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetDhcpDecline);
   }

   if (usmDbDhcpsReleaseReceivedGet(unit, &intDhcpRelease) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetDhcpRelease);
   }

   if (usmDbDhcpsInformReceivedGet(unit, &intDhcpInform) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetDhcpInform);
   }

   if (usmDbDhcpsOfferSentGet(unit, &intDhcpOffer) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetDhcpOffer);
   }

   if (usmDbDhcpsAckSentGet(unit, &intDhcpAck) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetDhcpAck);
   }

   if (usmDbDhcpsNackSentGet(unit, &intDhcpNack) != L7_SUCCESS)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantGetDhcpNack);
  }

  memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
  cliSyntaxTop(ewsContext);
  cliFormat(ewsContext, pStrInfo_base_AutoBindings);
  ewsTelnetPrintf (ewsContext, "%-3d", intActiveLeases);

   /*bzero(buffer,L7_CLI_MAX_STRING_LENGTH);
   cliFormat(ewsContext, CLI_MANUALBINDING);
    ewsTelnetPrintf (ewsContext, "%s%-3d", CLI_MANUALBINDING, intManualLeases);      */

  cliFormat(ewsContext, pStrInfo_base_ExpiredBindings);
  ewsTelnetPrintf (ewsContext, "%-3d", intExpiredLeases);

  cliFormat(ewsContext, pStrInfo_base_MalformedMsgs);
  ewsTelnetPrintf (ewsContext, "%-3d", intMalformedMessages);

  osapiSnprintfAddBlanks (2, 0, 0, 0, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_base_MsgsRcvd);
  ewsTelnetWrite( ewsContext, buffer);

  ewsTelnetPrintf (ewsContext, "\r\n----------                                 ----------");

  cliFormat(ewsContext, pStrInfo_base_DhcpDiscover);
  ewsTelnetPrintf (ewsContext, "%-3d", intDhcpDiscover);

  cliFormat(ewsContext, pStrInfo_base_DhcpReq);
  ewsTelnetPrintf (ewsContext, "%-3d", intDhcpRequest);

  cliFormat(ewsContext, pStrInfo_base_DhcpDecline);
  ewsTelnetPrintf (ewsContext, "%-3d", intDhcpDecline);

  cliFormat(ewsContext, pStrInfo_base_DhcpRelease);
  ewsTelnetPrintf (ewsContext, "%-3d", intDhcpRelease);

  cliFormat(ewsContext, pStrInfo_base_DhcpInform);
  ewsTelnetPrintf (ewsContext, "%-3d", intDhcpInform);

  osapiSnprintfAddBlanks (2, 0, 0, 0, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_base_MsgsSent);
  ewsTelnetWrite( ewsContext, buffer);

  ewsTelnetPrintf (ewsContext, "\r\n----------                                    ------");

  cliFormat(ewsContext, pStrInfo_base_DhcpOffer);
  ewsTelnetPrintf (ewsContext, "%-3d", intDhcpOffer);

  cliFormat(ewsContext, pStrInfo_base_DhcpAck);
  ewsTelnetPrintf (ewsContext, "%-3d", intDhcpAck);

  memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
  cliFormat(ewsContext, pStrInfo_base_DhcpNack);
  return cliSyntaxReturnPrompt (ewsContext,"%-3d", intDhcpNack);

}

/*********************************************************************
*
* @purpose  To display global config parameters
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
* @cmdsyntax show ip dhcp global config
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowDhcpsGlobalConfig(EwsContext ewsContext,
                                             L7_uint32 argc,
    const L7_char8 * * argv,
                                             L7_uint32 index)
{
   L7_uint32 unit;
   L7_uint32 numArg;
   L7_uint32 mode;
   L7_uint32 noPingPackets;
   L7_uint32 fromIpAddr;
   L7_uint32 toIpAddr;
   L7_uint32 rangeIndex;
   L7_RC_t   rc;
   L7_uchar8 buffer[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  strFromIpAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  strToIpAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_BOOL   isFirstExcludedAddress = L7_TRUE;
   L7_BOOL   conflictLogging, bootpAutoMode;

   cliSyntaxTop(ewsContext);

   unit = cliGetUnitId();
   numArg = cliNumFunctionArgsGet();

   if (numArg != 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowIpDhcpGlobalCfg);
   }

   if (usmDbDhcpsAdminModeGet(unit, &mode) == L7_SUCCESS)
   {
    memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
    cliFormat(ewsContext, pStrInfo_base_ServiceDhcp);

      if (mode == L7_ENABLE)
      {
      ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
    }
    else
    {
      ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
   }

   if (usmDbDhcpsNoOfPingPktGet(unit, &noPingPackets) == L7_SUCCESS)
   {
    cliFormat(ewsContext, pStrInfo_base_PingPkts);
    ewsTelnetPrintf (ewsContext, "%-2d", noPingPackets);
   }

   rc = usmDbDhcpsExcludedAddressRangeEntryFirst(unit, &fromIpAddr, &toIpAddr, &rangeIndex);

   while (rc == L7_SUCCESS)
   {
      if (isFirstExcludedAddress == L7_TRUE)
      {
      memset (buffer, 0, L7_CLI_MAX_STRING_LENGTH);
      cliFormat(ewsContext, pStrInfo_base_ExcldAddres);
      }
      else
      {
         ewsTelnetWrite(ewsContext, "\r\n                                                ");
      }
      rc = usmDbInetNtoa(fromIpAddr,strFromIpAddr);
      rc = usmDbInetNtoa(toIpAddr,strToIpAddr);

    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_To_1, strFromIpAddr, strToIpAddr);

      ewsTelnetWrite(ewsContext, buffer);

    memset (buffer, 0, L7_CLI_MAX_STRING_LENGTH);
      rc = usmDbDhcpsExcludedAddressRangeEntryNext(unit, &fromIpAddr, &toIpAddr, &rangeIndex);
      isFirstExcludedAddress = L7_FALSE;
   }

   if (usmDbDhcpsConflictLoggingGet(unit, &conflictLogging) == L7_SUCCESS)
   {
    memset (buffer, 0, sizeof(buffer));
    cliFormat(ewsContext, pStrInfo_base_ConflictLogging);
      if (conflictLogging == L7_TRUE)
      {
      ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else if (conflictLogging == L7_FALSE)
      {
      ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
   }

   if (usmDbDhcpsBootpAutomaticGet(unit, &bootpAutoMode) == L7_SUCCESS)
   {
    memset (buffer, 0, sizeof(buffer));
    cliFormat(ewsContext, pStrInfo_base_BootpAuto);
      if (bootpAutoMode == L7_TRUE)
      {
      ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else if (bootpAutoMode == L7_FALSE)
      {
      ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  To display pool config parameters
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
* @cmdsyntax show ip dhcp pool config <name|all>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowDhcpsPoolConfig(EwsContext ewsContext,
                                           L7_uint32 argc,
    const L7_char8 * * argv,
                                           L7_uint32 index)
{
   L7_uint32 argShowPool = 1;
   L7_uint32 ipAddr, ipMask, leaseTime ;
   L7_uchar8 buffer[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 clientIdBuffer[(L7_DHCPS_CLIENT_ID_MAXLEN*3)+1];
   L7_uchar8 tempBuf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  strIpAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  strMask[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  poolName[L7_DHCPS_POOL_NAME_MAXLEN];
   L7_char8  strClientName[L7_DHCPS_HOST_NAME_MAXLEN+1];
   L7_char8  strClientId[L7_DHCPS_CLIENT_ID_MAXLEN+1];
   L7_uchar8  hwAddr[L7_DHCPS_HARDWARE_ADDR_MAXLEN];
   L7_uint32 hwType;
   L7_uint32 poolType;
   L7_uint32 intClientIdLen;
   L7_uint32 hostAddr, hostMask;
   L7_char8  strHostIpAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  strHostMask[L7_CLI_MAX_STRING_LENGTH];
   L7_RC_t   rc;
   L7_uint32 poolIndex;
   L7_uint32 dnsSvrs[L7_DHCPS_DNS_SERVER_MAX];
   L7_uint32 defaultRouters[L7_DHCPS_DEFAULT_ROUTER_MAX];
   L7_uint32 days, hours;
   L7_uint32 counter, i, nonzero = 0;
   L7_BOOL   showFlag;
   L7_uint32 unit, numArg;
   L7_char8  strDomainName[L7_DHCPS_DOMAIN_NAME_MAXLEN+1];
   L7_char8  strBootFile[L7_DHCPS_BOOT_FILE_NAME_MAXLEN+1];
   L7_char8  strNextServerIpAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 nextServerIpAddr;
   L7_char8  nodeType;
   L7_char8  strNodeType[8];
   L7_uint32 nbNameServerAddress[L7_DHCPS_NETBIOS_NAME_SERVER_MAX];
   L7_char8  strNBNameServerIpAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 optionCode, nextOptionCode;
   L7_uchar8 optionData[L7_DHCPS_POOL_OPTION_LENGTH+1];
   L7_uint32 optionDataLen;
   L7_uchar8 optionDataFormat;
   L7_uchar8 optionStatus;
   L7_char8  temp[4];
   L7_uint32 firstChar, lastChar;
   L7_char8  strData[3*L7_DHCPS_POOL_OPTION_LENGTH];
   L7_char8  strOptionDataFormat[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 noOfIpAddresses;
   L7_RC_t   rc_option;
   L7_BOOL   isFirstOption = L7_TRUE;

   cliSyntaxTop(ewsContext);

   unit = cliGetUnitId();
   numArg = cliNumFunctionArgsGet();

   if (numArg != 1)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowIpDhcpPoolCfg);
  }

  if (strcmp(argv[index+argShowPool], pStrInfo_common_All) != 0)
   {
      if (strlen(argv[index+argShowPool]) >= sizeof(poolName))
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidPoolName);
    }
    OSAPI_STRNCPY_SAFE(poolName, argv[index + argShowPool]);

      showFlag = L7_TRUE;
   }
   else
   {
    OSAPI_STRNCPY_SAFE(poolName, "");
      showFlag = L7_FALSE;
   }

   if (showFlag == L7_FALSE)
   {
     /* Display all pools */
      rc = usmDbDhcpsPoolEntryFirst(unit,poolName,&poolIndex);
      if (rc != L7_SUCCESS)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DhcpNoPoolIPv4);
      }
   }
   else
   {
      rc = usmDbDhcpsPoolTypeGet(unit, poolName, &poolType);
      if (rc != L7_SUCCESS)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidPoolName);
      }
   }

   while (rc == L7_SUCCESS)
   {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_Pool_1, poolName);
    ewsTelnetWrite(ewsContext, buffer);
    memset (buffer, 0, sizeof(buffer));
      rc = usmDbDhcpsPoolTypeGet(unit, poolName, &poolType);
      if (poolType == L7_DHCPS_DYNAMIC_POOL)
      {
      memset (buffer, 0, L7_CLI_MAX_STRING_LENGTH);
      cliFormat(ewsContext, pStrInfo_base_PoolType);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Dyn_1);

         if ((usmDbDhcpsNetworkGet(unit, poolName, &ipAddr, &ipMask) == L7_SUCCESS)
             && (ipAddr != FD_DHCPS_DEFAULT_NETWORKIP_ADDR))
         {
            rc = usmDbInetNtoa(ipAddr,strIpAddr);
            rc = usmDbInetNtoa(ipMask,strMask);
        cliFormat(ewsContext,pStrInfo_common_Nw);
        ewsTelnetPrintf (ewsContext, "%s %s ", strIpAddr, strMask);
         }
      }
      else if (poolType == L7_DHCPS_MANUAL_POOL)
      {
      cliFormat(ewsContext, pStrInfo_base_PoolType);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Manual);
      memset (strClientName, 0, sizeof(strClientName));
         if ((usmDbDhcpsClientNameGet(unit, poolName, strClientName) == L7_SUCCESS)
             && (strcmp(strClientName,FD_DHCPS_DEFAULT_CLIENT_NAME)!=0))
         {
        cliFormat(ewsContext,pStrInfo_base_ClientName_1);
        ewsTelnetPrintf (ewsContext, "%s ", strClientName);
      }
      memset (strClientId, 0, sizeof(strClientId));
         if ((usmDbDhcpsClientIdGet(unit, poolName, strClientId, &intClientIdLen) == L7_SUCCESS)
          && (strcmp(strClientId, FD_DHCPS_DEFAULT_CLIENT_ID)!=0))
      {
        memset (clientIdBuffer, 0, sizeof(clientIdBuffer));
        cliFormat(ewsContext,pStrInfo_base_ClientId_1);
            for (counter = 0; counter < intClientIdLen; counter++)
            {
          osapiSnprintf(tempBuf, sizeof(tempBuf), "%02x", strClientId[counter]);
          OSAPI_STRNCAT(clientIdBuffer, tempBuf);

               if (counter != intClientIdLen-1)
               {
            OSAPI_STRNCAT(clientIdBuffer, ":");
               }
            }
            ewsTelnetWrite(ewsContext, clientIdBuffer);
         }

         if (usmDbDhcpsHwAddressGet(unit, poolName, hwAddr, &hwType) == L7_SUCCESS)
         {
        for (counter=0; counter<=5; counter++)
        {
               if (hwAddr[counter] != 0)
          {
                  nonzero++;
          }
        }

            if (nonzero != 0)
            {
          cliFormat(ewsContext,pStrInfo_base_HardwareAddr_1);

          ewsTelnetPrintf (ewsContext, "%02x:%02x:%02x:%02x:%02x:%02x",
                       hwAddr[0], hwAddr[1], hwAddr[2], hwAddr[3], hwAddr[4], hwAddr[5]);
               if (hwType == L7_DHCPS_ETHERNET)
               {
            cliFormat(ewsContext,pStrInfo_base_HardwareAddrType);
            ewsTelnetWrite(ewsContext,pStrInfo_common_DhcpEtherNet);
               }
               else if (hwType == L7_DHCPS_IEEE_802)
               {
            cliFormat(ewsContext,pStrInfo_base_HardwareAddrType);
            ewsTelnetWrite(ewsContext, pStrInfo_base_DhcpIeee);
               }
            }
            nonzero = 0;
         }

         if ((usmDbDhcpsHostGet(unit, poolName, &hostAddr, &hostMask) == L7_SUCCESS)
             && (hostAddr != FD_DHCPS_DEFAULT_HOST_ADDRESS))
         {
        memset (buffer, 0, sizeof(buffer));
        memset (strHostIpAddr, 0, sizeof(strHostIpAddr));
        memset (strHostMask, 0, sizeof(strHostMask));
        cliFormat(ewsContext,pStrInfo_base_Host_1);
            rc = usmDbInetNtoa(hostAddr,strHostIpAddr);
            rc = usmDbInetNtoa(hostMask,strHostMask);
        ewsTelnetPrintf (ewsContext, "%s %s ", strHostIpAddr, strHostMask);
      }
    }
    else
    {
      memset (buffer, 0, L7_CLI_MAX_STRING_LENGTH);
      cliFormat(ewsContext, pStrInfo_base_PoolType);
      ewsTelnetWrite(ewsContext, pStrInfo_base_Auto_1);
      }
      if (usmDbDhcpsLeaseTimeGet(unit,&leaseTime,poolName) == L7_SUCCESS)
      {
         if (leaseTime >= L7_DHCPS_LEASETIME_MAX)
         {
        cliFormat(ewsContext, pStrInfo_base_LeaseTime);
        ewsTelnetPrintf (ewsContext, pStrInfo_common_DhcpsLeaseInfinite_1);
         }
         else
         {
            days = leaseTime/1440;
            leaseTime = leaseTime%1440;
            hours = leaseTime/60;
            leaseTime = leaseTime%60;

        cliFormat(ewsContext, pStrInfo_base_LeaseTime);
        ewsTelnetPrintf (ewsContext, pStrInfo_base_DaysHrsMins, days, hours, leaseTime);
         }
      }

      if (usmDbDhcpsDnsServersGet(unit, poolName, dnsSvrs) == L7_SUCCESS)
      {
      memset (buffer, 0, sizeof(buffer));
         for (i=0; i<L7_DHCPS_DNS_SERVER_MAX ; i++)
         {
            if (dnsSvrs[i] !=0)
            {
               if (usmDbInetNtoa(dnsSvrs[i],strIpAddr) == L7_SUCCESS)
               {
                  if (nonzero == 0)
                  {
              cliFormat(ewsContext, pStrInfo_base_DnsSrvr);
                     nonzero =1;
                  }
                  else
                  {
                     ewsTelnetWrite(ewsContext, "\r\n                                                ");
                  }
            ewsTelnetPrintf (ewsContext, "%s ", strIpAddr);
            memset (buffer, 0, sizeof(buffer));
               }
            }
         }
         nonzero = 0;
      }
      if (usmDbDhcpsRoutersGet(unit, poolName, defaultRouters) == L7_SUCCESS)
      {

      memset (buffer, 0, sizeof(buffer));
         for (i=0; i<L7_DHCPS_DEFAULT_ROUTER_MAX ; i++)
         {
            if (defaultRouters[i] !=0)
            {
               if (usmDbInetNtoa(defaultRouters[i],strIpAddr) == L7_SUCCESS)
               {
                  if (nonzero == 0)
                  {
              cliFormat(ewsContext, pStrInfo_base_Rtr);
                     nonzero =1;
                  }
                  else
                  {
                     ewsTelnetWrite(ewsContext, "\r\n                                                ");
                  }
            ewsTelnetPrintf (ewsContext, "%s ", strIpAddr);
            memset (buffer, 0, sizeof(buffer));
               }
            }
         }
         nonzero = 0;
      }

      if ((usmDbDhcpsDomainNameGet(unit, poolName, strDomainName) == L7_SUCCESS)
          && (strcmp(strDomainName, FD_DHCPS_DEFAULT_DOMAIN_NAME)!=0))
      {
      cliFormat(ewsContext, pStrInfo_common_DoMainName);
      ewsTelnetPrintf (ewsContext, "%s ", strDomainName);
      }

      if ((usmDbDhcpsBootfileGet(unit, poolName, strBootFile) == L7_SUCCESS)
          && (strcmp(strBootFile, FD_DHCPS_DEFAULT_BOOTFILE_NAME)!=0))
      {
      cliFormat(ewsContext, pStrInfo_base_BootFile_1);
      ewsTelnetPrintf (ewsContext, "%s ", strBootFile);
      }

      if ((usmDbDhcpsNextServerGet(unit, poolName, &nextServerIpAddr) == L7_SUCCESS)
          && (nextServerIpAddr != FD_DHCPS_DEFAULT_NEXTSERVERIP_ADDR))
      {
         rc = usmDbInetNtoa(nextServerIpAddr, strNextServerIpAddr);
      cliFormat(ewsContext, pStrInfo_base_NextSrvr);
      ewsTelnetPrintf (ewsContext, "%s ", strNextServerIpAddr);
      }

      if ((usmDbDhcpsNetbiosNodeTypeGet(unit, poolName, &nodeType) == L7_SUCCESS)
          && (nodeType != FD_DHCPS_DEFAULT_NBNODETYPE))
      {
      memset (strNodeType, 0, sizeof(strNodeType));
         if (nodeType == L7_DHCPS_B_NODE)
      {
        OSAPI_STRNCPY_SAFE(strNodeType, pStrInfo_base_BNode);
      }
         else if (nodeType == L7_DHCPS_P_NODE)
      {
        OSAPI_STRNCPY_SAFE(strNodeType, pStrInfo_base_PNode);
      }
         else if (nodeType == L7_DHCPS_M_NODE)
      {
        OSAPI_STRNCPY_SAFE(strNodeType, pStrInfo_base_MNode);
      }
         else if (nodeType == L7_DHCPS_H_NODE)
      {
        OSAPI_STRNCPY_SAFE(strNodeType, pStrInfo_base_HNode);
      }

      cliFormat(ewsContext, pStrInfo_base_NetbiosNodeType_1);
      ewsTelnetPrintf (ewsContext, strNodeType);
      }

      if ((usmDbDhcpsNetbiosNameServerAddressGet(unit, poolName, nbNameServerAddress) == L7_SUCCESS)
          && (nbNameServerAddress[0] != FD_DHCPS_DEFAULT_NBNAMESERVERIP_ADDR))
      {

      memset (buffer, 0, sizeof(buffer));
         for (i=0; i<L7_DHCPS_NETBIOS_NAME_SERVER_MAX ; i++)
         {
            if (nbNameServerAddress[i] !=0)
            {
               if (usmDbInetNtoa(nbNameServerAddress[i], strNBNameServerIpAddr) == L7_SUCCESS)
               {
                  if (nonzero == 0)
                  {
              cliFormat(ewsContext, pStrInfo_base_NetbiosNameSrvr);
                     nonzero = 1;
                  }
                  else
                  {
                     ewsTelnetWrite(ewsContext, "\r\n                                                ");
                  }
            ewsTelnetPrintf (ewsContext, "%s ", strNBNameServerIpAddr);
            memset (buffer, 0, sizeof(buffer));
               }
            }
         }
         nonzero = 0;
      }

      rc_option = usmDbDhcpsOptionGetFirst(unit, poolName, &optionCode);

      while (rc_option == L7_SUCCESS)
      {
         if (isFirstOption == L7_TRUE)
         {
        memset (buffer, 0, L7_CLI_MAX_STRING_LENGTH);
        cliFormat(ewsContext, pStrInfo_base_Option);
         }
         else
         {
            ewsTelnetWrite(ewsContext, "\r\n                                                ");
         }

         if (usmDbDhcpsOptionGet(unit, poolName, optionCode, optionData, &optionDataLen, &optionDataFormat, &optionStatus) == L7_SUCCESS)
         {
            if (optionDataFormat == L7_DHCPS_ASCII)
            {
          OSAPI_STRNCPY_SAFE(strOptionDataFormat, pStrInfo_common_AsCiiOption);

               optionData[optionDataLen] = 0;
          ewsTelnetPrintf (ewsContext, "%d %s %s", optionCode, strOptionDataFormat, optionData);
            }
            else if (optionDataFormat == L7_DHCPS_HEX)
            {
          OSAPI_STRNCPY_SAFE(strOptionDataFormat, pStrInfo_common_HexOption);

          memset(strData, 0, sizeof(strData));
               for (counter=0 ; counter < optionDataLen ; counter++)
               {
            memset(temp, 0, sizeof(temp));
                  firstChar = (optionData[counter] & 0xF0) >> 4;
                  lastChar  = optionData[counter] & 0x0F;
            osapiSnprintf(temp,sizeof(temp),"%x%x", firstChar, lastChar);

                  if ((counter%2) == 1)
            {
                     if (counter != (optionDataLen-1))
              {
                OSAPI_STRNCAT(temp,".");
              }
               }

            OSAPI_STRNCAT(strData, temp);
          }

          ewsTelnetPrintf (ewsContext, "%d %s %s", optionCode, strOptionDataFormat, strData);
            }
            else if (optionDataFormat == L7_DHCPS_IP)
            {
          OSAPI_STRNCPY_SAFE(strOptionDataFormat, pStrInfo_common_IpOption);

          memset(strData, 0, sizeof(strData));
          memset(strIpAddr, 0, sizeof(strIpAddr));
               counter = 0;
               noOfIpAddresses = 0;

               while (counter < optionDataLen)
               {
                  ipAddr = 0;

                  memcpy(&ipAddr, optionData + counter, 4);

                  rc = usmDbInetNtoa(ipAddr,  strIpAddr);
                  if (rc == L7_SUCCESS)
                  {
              OSAPI_STRNCAT(strData, strIpAddr);
              OSAPI_STRNCAT(strData," ");
                     noOfIpAddresses++;
                  }

                  counter = counter + 4;

                  if ((noOfIpAddresses == 8) || (counter == optionDataLen))
                  {
              ewsTelnetPrintf (ewsContext, "%d %s %s", optionCode, strOptionDataFormat, strData);

              memset(strIpAddr, 0, sizeof(strIpAddr));
              memset(strData, 0, sizeof(strData));
                     noOfIpAddresses = 0;
                  }
               }
            }
         }

         rc_option = usmDbDhcpsOptionGetNext(unit, poolName, optionCode, &nextOptionCode);
         optionCode = nextOptionCode;
         isFirstOption = L7_FALSE;
      }

      nonzero = 0;

      if (showFlag == L7_FALSE)
      {
         rc = usmDbDhcpsPoolEntryNext(unit, poolName, &poolIndex);
      }
      else
      {
         rc = L7_FAILURE;
      }
      cliSyntaxBottom(ewsContext);

   }

   return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To display address conflicts found by DHCP server
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
* @cmdsyntax show ip dhcp conflict [ip-address]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowDhcpsConflict(EwsContext ewsContext,
                                         L7_uint32 argc,
    const L7_char8 * * argv,
                                         L7_uint32 index)
{
   L7_RC_t   rc;
   L7_uint32 unit, numArg;
   L7_uint32 argIpAddr = 1;
   L7_char8  ipAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 buffer[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 intIpAddress;
   L7_uint32 intNextIpAddress;
   L7_uint32 intDetectionTime;
   L7_uint32 intDetectionMethod;
   L7_uint32 intDay, intHours, intMinutes;

   cliSyntaxTop(ewsContext);

   unit = cliGetUnitId();
   numArg = cliNumFunctionArgsGet();

   if ((numArg < 0) || (numArg > 1))
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ShowIpDhcpConflict);
   }

   if (numArg == 1)
   {
    memset (ipAddr, 0,sizeof(ipAddr));
    if (strlen(argv[index+argIpAddr]) >= sizeof(ipAddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }

    OSAPI_STRNCPY_SAFE(ipAddr, argv[index + argIpAddr]);

    /* Convert Ip address into 32 bit integer */
    if (usmDbInetAton(ipAddr, &intIpAddress) == L7_SUCCESS)
    {
      if (usmDbDhcpsConflictGet(unit, intIpAddress, &intDetectionTime, &intDetectionMethod) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_NoConflict);
      }

      memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_IpAddrDetectionMethodDetectionTime);
      ewsTelnetWrite( ewsContext, "\r\n-----------        -----------------        ---------------");

      ewsTelnetPrintf (ewsContext, "\r\n%-19s",ipAddr);

      memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
      if (intDetectionMethod == L7_DHCPS_PING)
      {
        osapiSnprintfAddBlanks (0, 0, 0, 21, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_common_Ping);
      }
      else if (intDetectionMethod == L7_DHCPS_GRATUITOUS_ARP)
      {
        osapiSnprintfAddBlanks (0, 0, 0, 11, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_base_GratuitousArp);
      }

      ewsTelnetWrite( ewsContext, buffer);

      intDay = intDetectionTime/SECONDS_PER_DAY;
      intDetectionTime = intDetectionTime%SECONDS_PER_DAY;
      intHours = intDetectionTime/SECONDS_PER_HOUR;
      intDetectionTime = intDetectionTime%SECONDS_PER_HOUR;
      intMinutes = intDetectionTime/SECONDS_PER_MINUTE;
      intDetectionTime = intDetectionTime%SECONDS_PER_MINUTE;

      ewsTelnetPrintf (ewsContext, pStrInfo_base_DaysHMS_1, intDay, intHours, intMinutes, intDetectionTime);

      return cliPrompt(ewsContext);

    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
         return cliPrompt(ewsContext);
      }
   }

   rc = usmDbDhcpsConflictGetFirst(unit, &intIpAddress);

  memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_IpAddrDetectionMethodDetectionTime);
   ewsTelnetWrite( ewsContext, "\r\n-----------        -----------------        ---------------");

   while (rc == L7_SUCCESS)
   {
      if (usmDbDhcpsConflictGet(unit, intIpAddress, &intDetectionTime, &intDetectionMethod) != L7_SUCCESS)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_NoConflict);
      }

      if (usmDbInetNtoa( intIpAddress, ipAddr) != L7_SUCCESS)
      {
         rc = L7_FAILURE;
      }
      else
      {
      ewsTelnetPrintf (ewsContext, "\r\n%-19s",ipAddr);

      memset (buffer, 0,L7_CLI_MAX_STRING_LENGTH);
      if (intDetectionMethod == L7_DHCPS_PING)
      {
        osapiSnprintfAddBlanks (0, 0, 0, 21, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_common_Ping);
      }
      else if (intDetectionMethod == L7_DHCPS_GRATUITOUS_ARP)
      {
        osapiSnprintfAddBlanks (0, 0, 0, 11, L7_NULLPTR, buffer,sizeof(buffer),pStrInfo_base_GratuitousArp);
      }

      ewsTelnetWrite( ewsContext, buffer);

      intDay = intDetectionTime/SECONDS_PER_DAY;
      intDetectionTime = intDetectionTime%SECONDS_PER_DAY;
      intHours = intDetectionTime/SECONDS_PER_HOUR;
      intDetectionTime = intDetectionTime%SECONDS_PER_HOUR;
      intMinutes = intDetectionTime/SECONDS_PER_MINUTE;
      intDetectionTime = intDetectionTime%SECONDS_PER_MINUTE;

      ewsTelnetPrintf (ewsContext, pStrInfo_base_DaysHMS_1, intDay, intHours, intMinutes, intDetectionTime);

      }

      rc = usmDbDhcpsConflictGetNext(unit, intIpAddress, &intNextIpAddress);
      intIpAddress = intNextIpAddress;
   }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

