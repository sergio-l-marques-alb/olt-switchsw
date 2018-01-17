/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config_dhcps.c
 *
 * @purpose show running config commands for dhcp server
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  03/05/2007
 *
 * @author  Amit Kulkarni
 *
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "datatypes.h"
#include "commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_switching_cli.h"
#include "strlib_base_cli.h"

#include "usmdb_counters_api.h"
#include "dhcps_exports.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dhcps_api.h"
#include "defaultconfig.h"


#include "ews.h"
#include "ews_cli.h"

#include "cliutil.h"
#include "cli_show_running_config.h"
#include "clicommands_card.h"


/*********************************************************************
 * @purpose  To print the DHCP running configuration
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 * @end
 *********************************************************************/

L7_RC_t cliRunningConfigDhcps(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 mode, ipAddr, ipMask, leaseTime ;
  L7_uchar8 buffer[(L7_DHCPS_CLIENT_ID_MAXLEN*3) + L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 tempBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMask[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 poolName[L7_DHCPS_POOL_NAME_MAXLEN+1];
  L7_char8 strClientName[L7_DHCPS_HOST_NAME_MAXLEN+1];
  L7_char8 strClientId[L7_DHCPS_CLIENT_ID_MAXLEN+1];
  L7_uchar8 hwAddr[L7_DHCPS_HARDWARE_ADDR_MAXLEN];
  L7_uint32 hwType;
  L7_uint32 poolType;
  L7_uint32 intClientIdLen = 0;
  L7_uint32 hostAddr, hostMask;
  L7_char8 strHostIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strHostMask[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strFromIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strToIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;
  L7_uint32 poolIndex, noPingPackets;
  L7_uint32 dnsSvrs[L7_DHCPS_DNS_SERVER_MAX];
  L7_uint32 defaultRouters[L7_DHCPS_DEFAULT_ROUTER_MAX], i;
  L7_uint32 days, hours;
  L7_uint32 fromIpAddr;
  L7_uint32 toIpAddr;
  L7_uint32 rangeIndex;
  L7_uint32 counter, nonzero = 0;
  L7_RC_t rc_option;
  L7_BOOL conflictLogging, bootpAutoMode;
  L7_char8 strDomainName[L7_DHCPS_DOMAIN_NAME_MAXLEN+1];
  L7_char8 strBootFile[L7_DHCPS_BOOT_FILE_NAME_MAXLEN+1];
  L7_char8 strNextServerIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 nextServerIpAddr;
  L7_char8 nodeType;
  L7_char8 strNodeType[8];
  L7_uint32 nbNameServerAddress[L7_DHCPS_NETBIOS_NAME_SERVER_MAX];
  L7_char8 strNBNameServerIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strData[3*L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 optionBuffer[3*L7_DHCPS_POOL_OPTION_LENGTH];
  L7_char8 temp[4];
  L7_uint32 firstChar, lastChar;
  L7_uchar8 optionCode, nextOptionCode;
  L7_uchar8 optionData[L7_DHCPS_POOL_OPTION_LENGTH+1];
  L7_uint32 optionDataLen;
  L7_uchar8 optionDataFormat;
  L7_char8 strOptionDataFormat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 optionStatus;
  L7_uint32 noOfIpAddresses;

  if (usmDbDhcpsAdminModeGet(unit, &mode) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, mode,FD_DHCPS_DEFAULT_ADMIN_MODE,pStrInfo_base_ServiceDhcp_1);
  }
  if (usmDbDhcpsNoOfPingPktGet(unit, &noPingPackets) == L7_SUCCESS)
  {
    if ((noPingPackets != FD_DHCPS_DEFAULT_PINGPKTNO)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      if (noPingPackets == 0)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_NoIpDhcpPingPkts);
        EWSWRITEBUFFER(ewsContext, buffer);
      }
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_IpDhcpPingPkts, noPingPackets);
      EWSWRITEBUFFER(ewsContext, buffer);
    }
  }

  rc = usmDbDhcpsExcludedAddressRangeEntryFirst(unit, &fromIpAddr, &toIpAddr, &rangeIndex);

  while (rc == L7_SUCCESS)
  {
    rc = usmDbInetNtoa(fromIpAddr,strFromIpAddr);
    rc = usmDbInetNtoa(toIpAddr,strToIpAddr);
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_IpDhcpExcldAddr, strFromIpAddr, strToIpAddr);
    EWSWRITEBUFFER(ewsContext, buffer);

    rc = usmDbDhcpsExcludedAddressRangeEntryNext(unit, &fromIpAddr, &toIpAddr, &rangeIndex);
  }

  if (usmDbDhcpsConflictLoggingGet(unit, &conflictLogging) == L7_SUCCESS)
  {
    cliShowCmdTrue(ewsContext,conflictLogging,FD_DHCPS_DEFAULT_CONFLICTLOGGING,pStrInfo_base_IpDhcpConflictLogging);
  }
  if (usmDbDhcpsBootpAutomaticGet(unit, &bootpAutoMode) == L7_SUCCESS)
  {
    cliShowCmdTrue(ewsContext,bootpAutoMode,FD_DHCPS_DEFAULT_BOOTPAUTO,pStrInfo_base_IpDhcpBootpAuto);
  }

  rc = usmDbDhcpsPoolEntryFirst(unit,poolName,&poolIndex);

  while (rc == L7_SUCCESS)
  {
    osapiSnprintfAddBlanks (2, 0, 0, 0, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_IpDhcpPool, poolName);
    EWSWRITEBUFFER(ewsContext, buffer);

    if (usmDbDhcpsLeaseTimeGet(unit,&leaseTime,poolName) == L7_SUCCESS)
    {
      if ((leaseTime != FD_DHCPS_DEFAULT_POOLCFG_LEASETIME)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        days = leaseTime/1440;
        leaseTime = leaseTime%1440;
        hours = leaseTime/60;
        leaseTime = leaseTime%60;
        if(days == 60) /* infinite */
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_LeaseInfinite);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_Lease_1, days, hours, leaseTime);
        }
        EWSWRITEBUFFER(ewsContext, buffer);
      }
    }

    if (usmDbDhcpsDnsServersGet(unit, poolName, dnsSvrs) == L7_SUCCESS)
    {
      if ((dnsSvrs[0] != FD_DHCPS_DEFAULT_DNS_SERVER)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_common_DnsSrvr_1);
        for (i=0; i<L7_DHCPS_DNS_SERVER_MAX ; i++)
        {
          if (dnsSvrs[i] !=0 )
          {
            if (usmDbInetNtoa(dnsSvrs[i],strIpAddr) == L7_SUCCESS)
            {
              OSAPI_STRNCAT(buffer,strIpAddr);
              OSAPI_STRNCAT(buffer," ");
            }
          }
        }
        EWSWRITEBUFFER(ewsContext, buffer);
      }
    }

    if ((usmDbDhcpsRoutersGet(unit, poolName, defaultRouters) == L7_SUCCESS)&&
        ((defaultRouters[0] != FD_DHCPS_DEFAULT_ROUTER)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_DeflRtr);
      for (i=0; i<L7_DHCPS_DEFAULT_ROUTER_MAX ; i++)
      {
        if (defaultRouters[i] !=0 )
        {
          if (usmDbInetNtoa(defaultRouters[i],strIpAddr) == L7_SUCCESS)
          {
            OSAPI_STRNCAT(buffer,strIpAddr);
            OSAPI_STRNCAT(buffer," ");
          }
        }
      }
      EWSWRITEBUFFER(ewsContext, buffer);
    }

    rc = usmDbDhcpsPoolTypeGet(unit, poolName, &poolType);
    if (poolType == L7_DHCPS_DYNAMIC_POOL)
    {
      if (usmDbDhcpsNetworkGet(unit, poolName, &ipAddr, &ipMask) == L7_SUCCESS)
      {
        if ((ipAddr != FD_DHCPS_DEFAULT_NETWORKIP_ADDR)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          rc = usmDbInetNtoa(ipAddr,strIpAddr);
          rc = usmDbInetNtoa(ipMask,strMask);
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_Nw_1, strIpAddr, strMask);
          EWSWRITEBUFFER(ewsContext, buffer);
        }
      }
    }

    memset (strClientName, 0, sizeof(strClientName));
    if (usmDbDhcpsClientNameGet(unit, poolName, strClientName) == L7_SUCCESS)
    {
      if ((strcmp(strClientName,FD_DHCPS_DEFAULT_CLIENT_NAME)!=0)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_ClientName_3, strClientName);
        EWSWRITEBUFFER(ewsContext, buffer);
      }
    }

    if ((usmDbDhcpsClientIdGet(unit, poolName, strClientId, &intClientIdLen) == L7_SUCCESS))
    {
      if ( (strcmp(strClientId, FD_DHCPS_DEFAULT_CLIENT_ID)!=0) ||
          (intClientIdLen > 0) ||
          (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT) )
      {
        memset (buffer, 0, sizeof(buffer));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, buffer, pStrInfo_base_ClientId_2);

        for (counter = 0; counter < intClientIdLen; counter++ )
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf), "%02x", strClientId[counter]);
          OSAPI_STRNCAT(buffer, tempBuf);

          if (counter != intClientIdLen-1)
          {
            OSAPI_STRNCAT(buffer, ":");
          }
        }

        EWSWRITEBUFFER(ewsContext, buffer);
      }
    }

    if (usmDbDhcpsHwAddressGet(unit, poolName, hwAddr, &hwType) == L7_SUCCESS)
    {
      for (i=0; i<=5; i++)
      {
        if (hwAddr[i] != 0)
        {
          nonzero++;
        }
      }

      if (nonzero != 0)
      {
        memset (buffer, 0, sizeof(buffer));
        if (hwType == FD_DHCPS_DEFAULT_HARDWARE_TYPE)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_HardwareAddr_3,
              hwAddr[0], hwAddr[1], hwAddr[2], hwAddr[3], hwAddr[4], hwAddr[5]);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_HardwareAddr_4,
              hwAddr[0], hwAddr[1], hwAddr[2], hwAddr[3], hwAddr[4], hwAddr[5], hwType);
        }
        EWSWRITEBUFFER(ewsContext, buffer);
      }
    }

    if (poolType == L7_DHCPS_MANUAL_POOL)
    {
      if (usmDbDhcpsHostGet(unit, poolName, &hostAddr, &hostMask) == L7_SUCCESS)
      {
        if ((hostAddr != FD_DHCPS_DEFAULT_HOST_ADDRESS)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          memset (buffer, 0, sizeof(buffer));
          rc = usmDbInetNtoa(hostAddr,strHostIpAddr);
          rc = usmDbInetNtoa(hostMask,strHostMask);
          osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_Host_3, strHostIpAddr, strHostMask);
          EWSWRITEBUFFER(ewsContext, buffer);
        }
      }
    }

    memset (strDomainName, 0, sizeof(strDomainName));
    if ((usmDbDhcpsDomainNameGet(unit, poolName, strDomainName) == L7_SUCCESS) &&
        ((strcmp(strDomainName, FD_DHCPS_DEFAULT_DOMAIN_NAME)!=0)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_common_ShowRunDhcpv6DoMainName, strDomainName);
      EWSWRITEBUFFER(ewsContext, buffer);
    }

    memset (strBootFile, 0, sizeof(strBootFile));
    if ((usmDbDhcpsBootfileGet(unit, poolName, strBootFile) == L7_SUCCESS)&&
        ((strcmp(strBootFile, FD_DHCPS_DEFAULT_BOOTFILE_NAME)!=0)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_Bootfile_1, strBootFile);
      EWSWRITEBUFFER(ewsContext, buffer);
    }

    if ((usmDbDhcpsNextServerGet(unit, poolName, &nextServerIpAddr) == L7_SUCCESS) &&
        ((nextServerIpAddr != FD_DHCPS_DEFAULT_NEXTSERVERIP_ADDR)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
    {
      rc = usmDbInetNtoa(nextServerIpAddr, strNextServerIpAddr);
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_NextSrvr_2, strNextServerIpAddr);
      EWSWRITEBUFFER(ewsContext, buffer);
    }

    if ((usmDbDhcpsNetbiosNodeTypeGet(unit, poolName, &nodeType) == L7_SUCCESS)&&
        ((nodeType != FD_DHCPS_DEFAULT_NBNODETYPE)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
    {
      memset (strNodeType, 0, 8);
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

      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_NetbiosNodeType_3, strNodeType);
      EWSWRITEBUFFER(ewsContext, buffer);
    }

    if ((usmDbDhcpsNetbiosNameServerAddressGet(unit, poolName, nbNameServerAddress) == L7_SUCCESS)&&
        ((nbNameServerAddress[0] != FD_DHCPS_DEFAULT_NBNAMESERVERIP_ADDR)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buffer, sizeof(buffer),pStrInfo_base_NetbiosNameSrvr_1);
      for (i=0; i<L7_DHCPS_NETBIOS_NAME_SERVER_MAX ; i++)
      {
        if (nbNameServerAddress[i] !=0 )
        {
          if (usmDbInetNtoa(nbNameServerAddress[i], strNBNameServerIpAddr) == L7_SUCCESS)
          {
            OSAPI_STRNCAT(buffer, strNBNameServerIpAddr);
            OSAPI_STRNCAT(buffer," ");
          }
        }
      }
      EWSWRITEBUFFER(ewsContext, buffer);
    }

    rc_option = usmDbDhcpsOptionGetFirst(unit, poolName, &optionCode);

    while (rc_option == L7_SUCCESS)
    {
      if (usmDbDhcpsOptionGet(unit, poolName, optionCode, optionData, &optionDataLen,                                                                                                                   &optionDataFormat, &optionStatus) == L7_SUCCESS)
      {
        if (optionDataFormat == L7_DHCPS_ASCII)
        {
          OSAPI_STRNCPY_SAFE(strOptionDataFormat, pStrInfo_common_AsCiiOption);

          optionData[optionDataLen] = 0;
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, optionBuffer, sizeof(optionBuffer), pStrInfo_base_Option_2, optionCode, strOptionDataFormat, optionData);
          EWSWRITEBUFFER( ewsContext, optionBuffer);

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
            osapiSnprintf(temp, sizeof(temp), "%x%x", firstChar, lastChar);

            if ((counter%2) == 1)
            {
              if (counter != (optionDataLen-1))
              {
                OSAPI_STRNCAT(temp,".");
              }
            }

            OSAPI_STRNCAT(strData, temp);
          }

          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, optionBuffer, sizeof(optionBuffer), pStrInfo_base_Option_2, optionCode, strOptionDataFormat, strData);
          EWSWRITEBUFFER( ewsContext, optionBuffer);

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
              OSAPI_STRNCAT(strData, " ");
              noOfIpAddresses++;
            }

            counter = counter + 4;

            if ((noOfIpAddresses == 8) || (counter == optionDataLen))
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buffer, sizeof(buffer), pStrInfo_base_Option_2, optionCode, strOptionDataFormat, strData);
              EWSWRITEBUFFER( ewsContext, buffer);

              memset(strIpAddr, 0, sizeof(strIpAddr));
              memset(strData, 0, sizeof(strData));
              noOfIpAddresses = 0;
            }
          }

        }

      }

      rc_option = usmDbDhcpsOptionGetNext(unit, poolName, optionCode, &nextOptionCode);
      optionCode = nextOptionCode;
    }

    nonzero = 0;
    rc = usmDbDhcpsPoolEntryNext(unit, poolName, &poolIndex);
    EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_Exit); /*exit from 'dhcp pool config' mode */
  }

  return L7_SUCCESS;
}


