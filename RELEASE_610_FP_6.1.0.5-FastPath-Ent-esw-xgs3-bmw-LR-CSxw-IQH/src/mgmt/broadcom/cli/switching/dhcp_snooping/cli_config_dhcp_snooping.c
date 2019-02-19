/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_config_dhcp_snooping.c
*
* @purpose DHCP Snooping config commands for the cli
*
* @component DHCP Snooping
*
* @comments none
*
* @create  03/17/2007
*
* @author  rrice
*
* @end
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "datatypes.h"
#include "clicommands_dhcp_snooping.h"
#include "usmdb_dhcp_snooping.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "dhcp_snooping_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include "clicommands_card.h"
#include "ews.h"



/*********************************************************************
*
* @purpose  Configure the DHCP Snooping Global Mode.
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
* @cmdsyntax    ip dhcp snooping
*
* @cmdhelp
*
* @cmddescript  This command enables and disables DHCP snooping.
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpSnooping(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDhcpSnoopingGlobal);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDsAdminModeSet(L7_ENABLE) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_IpDhcpSnoopEnableFail);
      }
      /* If the command is ip dhcp filtering then enable snooping on vlans */
     if (strcmp(argv[2],"filtering") == 0)
     {
       if(usmDbDsVlanConfigSet(1, L7_PLATFORM_MAX_VLAN_ID,L7_ENABLE)!=L7_SUCCESS)
       { 
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_IpDhcpSnoopEnableFail);
       }
     }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoDhcpSnooping);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDsAdminModeSet(L7_DISABLE) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_IpDhcpSnoopDisableFail);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgDhcpSnoopingGlobal);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure whether DHCP snooping sets up source MAC address filters.
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
* @cmdsyntax    ip dhcp snooping verify mac-address
*
* @cmdhelp
*
* @cmddescript  
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpSnoopingVerifyMac(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDhcpSnoopingVerifyMac);
    }
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDsVerifyMacSet(L7_TRUE) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_IpDhcpVerifyMacFail);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgNoDhcpSnoopingVerifyMac);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDsVerifyMacSet(L7_FALSE) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_IpDhcpVerifyMacFail);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgDhcpSnoopingVerifyMac);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set the host Ip and file name to store/restore the 
*           Dhcp Snooping database
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
* @cmdsyntax ip dhcp snooping binding databse <url> 
*            no ip dhcp snooping binding database
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpSnoopingDbUrl(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index)
{
   L7_uint32 numArg = cliNumFunctionArgsGet();
   L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 ipAddr=0;
   L7_char8 fileName[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 intPos=7;

  if ( (ewsContext->commType == CLI_NORMAL_CMD) &&
        (numArg != 1)
     )
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext,
     "Use 'ip dhcp snooping binding database <url>'.");

  }
  else if ( (numArg !=0) &&
            (ewsContext->commType == CLI_NO_CMD) )
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext,
     "Use 'no ip dhcp snooping binding database <url>'.");


  }
  if ( (ewsContext->commType == CLI_NORMAL_CMD) )
  {
     if ( strcmp(argv[index+1],"local") == 0)
     {
       if ( usmDbDsDbIpAddrSet(0) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext,
                                              "Failed to set Ip address.");
       }
       ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
       return cliPrompt(ewsContext);
     }

     if ( strlen( argv[index+1]) <=7)
     {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext,
                    "Use 'ip dhcp snooping binding database tftp://'.");

     }
     if ( strncmp (argv[index+1],"tftp://",7)!=0)
     {
       return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext,
                    "Use 'ip dhcp snooping binding database tftp://'.");
     }
     if ( strlen(argv[index+1]) > 50 )
     {
       return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext,
                    "Use 'ip dhcp snooping binding database tftp://'.");

     }
     memset (strIpAddr,'\0', sizeof(strIpAddr));
     while ( (argv[index+1][intPos]!= '\0') && 
             (argv[index+1][intPos]!= '/') )
     {
       strIpAddr[intPos-7] = argv[index+1][intPos];
       intPos++; 
     }
     if ( strlen(argv[index+1]) <= intPos)
     {
       return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext,
                    "Use 'ip dhcp snooping binding database tftp://'.");

     }
     if ( usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS )
     {
       return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext,
                    "Use 'ip dhcp snooping binding database tftp://'.");
     }
     strIpAddr[intPos-7]='\0';
     osapiStrncpy (fileName, argv[index+1]+intPos+1, L7_CLI_MAX_STRING_LENGTH);
     if ( usmDbDsDbFileNameSet(fileName) != L7_SUCCESS)
     {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext,
                                              "Failed to set File Name.");
     }
     if ( usmDbDsDbIpAddrSet(ipAddr) != L7_SUCCESS)
     {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext,
                                              "Failed to set Ip address.");
     }
      
  }  
  else if (  (ewsContext->commType == CLI_NO_CMD) ) 
  {
     if ( usmDbDsDbIpAddrSet(ipAddr) != L7_SUCCESS)
     {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext,
                                              "Failed to set Ip address.");
     }

  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Set the write delay interval to store the
*           Dhcp Snooping database
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
* @cmdsyntax ip dhcp snooping binding databse write-delay <interval>
*            no ip dhcp snooping binding database write-delay
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpSnoopingDbStoreInterval(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index)
{

  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_uint32 storeInterval; 


  if ( (ewsContext->commType == CLI_NORMAL_CMD) &&
        (numArg != 1)
     )
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext,
     "Use 'ip dhcp snooping binding database write-delay <interval>'.");

  }
  else if ( (numArg !=0) &&
            (ewsContext->commType == CLI_NO_CMD) )
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext,
     "Use 'no ip dhcp snooping binding database write-delay <interval>'.");
  }

  if ( (ewsContext->commType == CLI_NORMAL_CMD))
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index + 1], &storeInterval) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                            ewsContext, "Invalid store interval");
    }

    if ( usmDbDsDbStoreIntervalSet(storeInterval) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                            ewsContext,
                                           "Failed to set store interval.");
    }
  }
  else if ( (ewsContext->commType == CLI_NO_CMD))
  {

    if ( usmDbDsDbStoreIntervalSet(L7_DS_DB_STORE_INTERVAL) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                            ewsContext,
                                           "Failed to set store interval.");
    }
  }
  
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}




/*********************************************************************
*
* @purpose  Create a static DHCP snooping binding.
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
* @cmdsyntax ip dhcp snooping binding <mac-address> vlan <vlan-id> <ip-address> interface <u/s/p>
*            no ip dhcp snooping binding <mac-address>
*
* @cmdhelp
*
* @cmddescript  
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpSnoopingBinding(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = cliNumFunctionArgsGet();        
  L7_uchar8 macAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_enetMacAddr_t macAddr;
  L7_uint32 vlanId = 0;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ipAddr;
  L7_uint32 u, s, p;
  L7_uint32 intIfNum = 0;      
  L7_RC_t rc;

  
  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD) 
  {
    if ((numArg != 6) || 
        (strcmp(argv[index+2], pStrInfo_base_VlanKeyword) != 0) ||
        (strcmp(argv[index+5], "interface") != 0))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, 
        "Use 'ip dhcp snooping binding <mac-address> vlan <vlan-id> <ip-address> interface <u/s/p>'.");
    }
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, 
                                          "Use 'no ip dhcp snooping binding <mac-address>'.");
  }

  /* Get MAC address */
  OSAPI_STRNCPY_SAFE(macAddrStr, argv[index + 1]);
  if (cliConvertMac(macAddrStr, macAddr.addr) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, "Invalid MAC address");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD) 
  {
    /* Get VLAN ID */
    if (cliConvertTo32BitUnsignedInteger(argv[index + 3], &vlanId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, "Invalid VLAN ID");
    }
  
    /* Get IP Address */
    OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + 4]);
    if (strlen(argv[index + 4]) >= sizeof(strIPaddr) || 
        (usmDbInetAton(strIPaddr, &ipAddr) != L7_SUCCESS))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, "Invalid IP address");
    }
  
    /* Get interface */
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[index + 6], &u, &s, &p) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                              pStrErr_common_InvalidSlotPort_1);
      }
  
      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(u, s, p, &intIfNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      u = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, argv[index + 6], &s, &p, 
                              &intIfNum) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD) 
    {
      rc = usmDbDsStaticBindingAdd(&macAddr, ipAddr, vlanId, 0, intIfNum);
      if (rc == L7_ERROR)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              "Invalid parameters.");
      }
      else if (rc == L7_TABLE_IS_FULL)
      {

        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              "MAX static DHCP Snooping entries are added already.");
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              "Failed to add static binding.");
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (usmDbDsStaticBindingRemove(&macAddr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              "Failed to remove static binding.");
      }
    }
  }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
*
* @purpose  Create a static IPSG entry.
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
* @cmdsyntax ip verify binding <mac-address> vlan <vlan-id> <ip-address> interface <u/s/p>
*            no ip verify binding <mac-address> vlan <vlan-id> <ip-address> interface <u/s/p>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpsgEntry(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_uchar8 macAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_enetMacAddr_t macAddr;
  L7_uint32 vlanId = 0;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ipAddr;
  L7_uint32 u, s, p;
  L7_uint32 intIfNum = 0;
  L7_RC_t rc;


  cliSyntaxTop(ewsContext);

  if ((numArg != 6) ||
     (strcmp(argv[index+2], pStrInfo_base_VlanKeyword) != 0) ||
     (strcmp(argv[index+5], "interface") != 0))
    {
      if ( ewsContext->commType == CLI_NORMAL_CMD )
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                            ewsContext,
         "Use 'ip dhcp snooping binding <mac-address> vlan <vlan-id> <ip-address> interface <u/s/p>'.");
      }
      else if ( ewsContext->commType == CLI_NO_CMD )
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                            ewsContext,
         "Use 'no ip dhcp snooping binding <mac-address> vlan <vlan-id> <ip-address> interface <u/s/p>'.");

      }
    }


  /* Get MAC address */
  OSAPI_STRNCPY_SAFE(macAddrStr, argv[index + 1]);
  if (cliConvertMac(macAddrStr, macAddr.addr) != L7_TRUE)
  {
   return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext, "Invalid MAC address");
  }

  /* Get VLAN ID */
  if (cliConvertTo32BitUnsignedInteger(argv[index + 3], &vlanId) != L7_SUCCESS)
  {
     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                            ewsContext, "Invalid VLAN ID");
  }

  /* Get IP Address */
  OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + 4]);
  if (strlen(argv[index + 4]) >= sizeof(strIPaddr) ||
        (usmDbInetAton(strIPaddr, &ipAddr) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                            ewsContext, "Invalid IP address");
  }

  /* Get interface */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(argv[index + 6], &u, &s, &p) != L7_SUCCESS)
    {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                              pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
   if (usmDbIntIfNumFromUSPGet(u, s, p, &intIfNum) != L7_SUCCESS)
   {
     return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext,
                                              pStrErr_common_InvalidSlotPort_1);
    }
  }
  else
  {
    u = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index + 6], &s, &p,
                              &intIfNum) != L7_SUCCESS)
     {
       return cliPrompt(ewsContext);
     }
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      rc = usmDbIpsgStaticEntryAdd(intIfNum,vlanId, &macAddr, ipAddr);
      if (rc == L7_ERROR)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext,
                                              "Invalid parameters.");
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext,
                                              "Failed to add static binding.");      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (usmDbIpsgStaticEntryRemove(intIfNum,vlanId, &macAddr, ipAddr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext,
                                              "Failed to remove static binding.");
      }
    }
  }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
#endif



/*********************************************************************
* @purpose  Change the DHCP snooping trust for an interface
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
* @cmdsyntax  ip dhcp snooping trust
*
* @cmdhelp Configure interface as trusted or untrusted for DHCP snooping.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpSnoopingTrust(EwsContext ewsContext, L7_uint32 argc, 
                                            const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0; 
  L7_BOOL trust;
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDhcpSnoopingTrust);
    }
    trust = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDhcpSnoopingTrustNo);
    }
    trust = L7_FALSE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_base_CfgDhcpSnoopingTrust);
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbDsIntfTrustSet(intIfNum, trust);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_IpDhcpSnoopingIntfBad);
      }
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_base_IpDhcpSnoopingTrustFail);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Enable or disable logging of invalid packets
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
* @cmdsyntax  ip dhcp snooping log-invalid
*
* @cmdhelp 
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpSnoopingLogInvalid(EwsContext ewsContext, L7_uint32 argc, 
                                                const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0; 
  L7_BOOL log;
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDhcpSnoopingLogInvalid);
    }
    log = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgDhcpSnoopingLogInvalidNo);
    }
    log = L7_FALSE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_base_CfgDhcpSnoopingLogInvalid);
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbDsIntfLogInvalidSet(intIfNum, log);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_IpDhcpSnoopingIntfBad);
      }
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_base_IpDhcpSnoopingTrustFail);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}





/*********************************************************************
* @purpose  Change the rate limit settings for an interface
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
* @cmdsyntax  ip dhcp snooping limit {rate <pps> [burst interval <seconds>] | none}
*
* @cmdhelp Configure rate limit and burst interval values
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpSnoopingIfRateLimit(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum = 0, rate, burst;
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg != 1) && (numArg != 2) && (numArg != 5))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                            ewsContext, pStrErr_base_CfgDsIfRateLimit);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                            ewsContext, pStrErr_base_CfgDsNoIfRateLimit);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext, pStrErr_base_CfgDsIfRateLimit);
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if(strcmp(argv[index+1], pStrInfo_base_LimitNone) == L7_SUCCESS)
      {
        rc = usmDbDsIntfRateLimitSet(intIfNum, L7_DS_RATE_LIMIT_NONE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                ewsContext, pStrErr_base_DsIfRateFail);
        }
      }
      else if(cliConvertTo32BitUnsignedInteger(argv[index+2], &rate) == L7_SUCCESS)
      {
        rc = usmDbDsIntfRateLimitSet(intIfNum, rate);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                ewsContext, pStrErr_base_DsIfRateFail);
        }
        if(numArg == 5)
        {
          if(cliConvertTo32BitUnsignedInteger(argv[index+5], &burst) == L7_SUCCESS)
          {
             rc = usmDbDsIntfBurstIntervalSet(intIfNum, burst); 
            if (rc != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                    ewsContext, pStrErr_base_DsIfBurstFail);
            }
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                                  ewsContext, pStrErr_base_CfgDsIfRateLimit);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                              ewsContext, pStrErr_base_CfgDsIfRateLimit);
      }
    }
  }
  else
  {
     if((usmDbDsIntfRateLimitSet(intIfNum, FD_DHCP_SNOOPING_RATE_LIMIT) != L7_SUCCESS) ||
       (usmDbDsIntfBurstIntervalSet(intIfNum, FD_DHCP_SNOOPING_BURST_INTERVAL) != L7_SUCCESS))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                            ewsContext, pStrErr_base_CfgDsIfRateLimit);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}


/*********************************************************************
* @purpose  Enable or disable DHCP snooping on a list of VLANs. 
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
* @cmdsyntax  ip dhcp snooping vlan <vlan-list> 
*
* @cmdhelp    User can specify a list of vlans using comma seprated
*             vlan ranges. A range is specified by two VLAN IDs
*             separated by a hyphen.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDhcpSnoopingVlan(EwsContext ewsContext, L7_uint32 argc, 
                                          const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 vlanStart = 0; 
  L7_uint32 vlanEnd = 0, vlanCount = 0;
  L7_uint32 vlanList[L7_MAX_VLANS];
  L7_uint32 arg1 = 1,i;
  L7_uint32 enable;        /* whether to enable or disable on these VLANS */
  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_char8 *strVlanList;
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];


  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  if ((numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgDhcpSnoopingVlan);
  }

  /* Get list of VLAN IDs */
  strVlanList = (L7_char8 *) argv[index + arg1];

  if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf),
                            pStrInfo_switching_VlanIdOutOfRange_1,
                            L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    ewsTelnetWrite(ewsContext, tempBuf);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
    
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    enable = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    enable = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_base_CfgDhcpSnoopingVlan);
  }

  if ( (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) &&
       (vlanCount))
  {
    vlanStart = vlanList[0];
    for(i = 0; i < vlanCount; i++)
    {
      if(1 == vlanCount)
      {
        vlanEnd = vlanStart;
      }
      else
      {
        if((i+1) == vlanCount)
        {
          vlanEnd = vlanList[i];
        }
        else if(vlanList[i+1] == vlanList[i]+1)
        {
          continue;
        }
        else
        {
          vlanEnd = vlanList[i];
        }
      }
      rc = usmDbDsVlanConfigSet(vlanStart, vlanEnd, enable);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              pStrErr_base_IpDhcpSnoopingInvalidVlanRange);
        }
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_base_IpDhcpSnoopingVlanFail);
      } 
      if((i+1) < vlanCount)
      {
        vlanStart = vlanList[i+1];
      }
   }

  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
* @purpose  Configure IP Source Guard on an interface
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
* @cmdsyntax  ip verify source
*
* @cmdhelp 
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpVerifySource(EwsContext ewsContext, L7_uint32 argc, 
                                      const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0; 
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;
  L7_BOOL verifyIp = L7_FALSE;
  L7_BOOL verifyMac = L7_FALSE;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgIpVerifySource);
    }
    verifyIp = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_base_CfgIpVerifySourceNo);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_base_CfgIpVerifySource);
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbIpsgVerifySourceSet(intIfNum, verifyIp, verifyMac);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_IpsgIntfBad);
      }
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_base_IpsgFail);
    }
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure IP Source Guard source MAC filtering on an interface
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
* @cmdsyntax  ip verify source port-security
*
* @cmdhelp 
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpVerifySourcePortSecurity(EwsContext ewsContext, L7_uint32 argc, 
                                                  const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0; 
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;
  L7_BOOL verifyIp = L7_TRUE;
  L7_BOOL verifyMac = L7_TRUE;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_CfgIpVerifySource);
  }
    
  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbIpsgVerifySourceSet(intIfNum, verifyIp, verifyMac);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_base_IpsgIntfBad);
      }
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_base_IpsgFail);
    }
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
#endif

#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
*
* @purpose  Configure the DHCP L2 Relay Global Mode.
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
* @cmdsyntax   [no] dhcp l2relay
*
* @cmdhelp
*
* @cmddescript  This command enables and disables DHCP L2 relay globally.
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpL2RelayGlobalMode(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = 0;        

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_switching_CfgDhcpL2Relay);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDsL2RelayAdminModeSet(L7_ENABLE) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_switching_DhcpL2RelayEnableFail);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_switching_CfgNoDhcpL2Relay);
    }
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDsL2RelayAdminModeSet(L7_DISABLE) != L7_SUCCESS)
      {
        cliSyntaxNewLine(ewsContext);
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_switching_DhcpL2RelayDisableFail);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_switching_CfgDhcpL2Relay);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}
/*********************************************************************
*
* @purpose  Configure the DHCP L2 Relay  for an interface
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
* @cmdsyntax    [no] dhcp l2relay 
*
* @cmdhelp
*
* @cmddescript  This command enables and disables DHCP L2 relay for an interface.
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpL2RelayIntfMode(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0; 
  L7_uint32 intfMode;
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_switching_CfgDhcpL2Relay);
    }
    intfMode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_switching_CfgNoDhcpL2Relay);
    }
    intfMode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_base_CfgDhcpSnoopingTrust);
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbDsL2RelayIntfModeSet(intIfNum, intfMode);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_switching_DhcpL2RelayIntfInvalid);
      }
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_switching_DhcpL2RelayIntfL2RelayFail);
    }
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}



/*********************************************************************
* @purpose  Change the DHCP snooping trust for an interface
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
* @cmdsyntax  [no] dhcp l2relay trust
*
* @cmdhelp Configure interface as trusted or untrusted for DHCP snooping.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpL2RelayIntfTrust(EwsContext ewsContext, L7_uint32 argc, 
                                        const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0; 
  L7_BOOL trust;
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_switching_CfgDhcpL2RelayTrust);
    }
    trust = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_switching_CfgNoDhcpL2RelayTrust);
    }
    trust = L7_FALSE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_switching_CfgDhcpL2RelayTrust);
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbDsL2RelayIntfTrustSet(intIfNum, trust);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_switching_DhcpL2RelayIntfInvalid);
      }
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_switching_DhcpL2RelayEnableTrustFail);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure the DHCP L2 Relay  for Vlan range
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
* @cmdsyntax    [no] dhcp l2relay vlan <vlan-range>
*
* @cmdhelp
*
* @cmddescript  This command enables and disables DHCP L2 relay for a vlan.
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpL2RelayVlan(EwsContext ewsContext, L7_uint32 argc, 
                                         const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 vlanStart = 0; 
  L7_uint32 vlanEnd = 0, vlanCount = 0;
  L7_uint32 vlanList[L7_MAX_VLANS];
  L7_uint32 arg1 = 1,i;
  L7_uint32 enable;        /* whether to enable or disable on these VLANS */
  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_char8 *strVlanList;
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];


  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  if ((numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_switching_CfgDhcpL2RelayVlan);
  }

  /* Get list of VLAN IDs */
  strVlanList = (L7_char8 *) argv[index + arg1];

  if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf),
                            pStrInfo_switching_VlanIdOutOfRange_1,
                            L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    ewsTelnetWrite(ewsContext, tempBuf);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    enable = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    enable = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_switching_CfgDhcpL2RelayVlan);
  }

  if ( (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) &&
       (vlanCount))
  {
    vlanStart = vlanList[0];
    for(i = 0; i < vlanCount; i++)
    {
      if ((vlanList[i] < L7_DOT1Q_MIN_VLAN_ID) ||
          (vlanList[i] > L7_DOT1Q_MAX_VLAN_ID))
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_base_IpDhcpSnoopingInvalidVlanRange);
      }
      if(1 == vlanCount)
      {
        vlanEnd = vlanStart;
      }
      else
      {
        if((i+1) == vlanCount)
        {
          vlanEnd = vlanList[i];
        }
        else if(vlanList[i+1] == vlanList[i]+1)
        {
          continue;
        }
        else
        {
          vlanEnd = vlanList[i];
        }
      }
      rc = usmDbDsL2RelayVlanModeSet(vlanStart, vlanEnd, enable);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              pStrErr_base_IpDhcpSnoopingInvalidVlanRange);
        }
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_switching_DhcpL2RelayVlanFail);
      } 
      if((i+1) < vlanCount)
      {
        vlanStart = vlanList[i+1];
      }
   }

  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  Configure the DHCP L2 Relay circuit-id for Vlan range
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
* @cmdsyntax   [no] dhcp l2relay vlan <vlan-range> circuit-id
*
* @cmdhelp
*
* @cmddescript  This command enables and disables DHCP circuit-id for a vlan.
*
* @end
*
*********************************************************************/

const L7_char8 *commandDhcpL2RelayVlanCircuitId(EwsContext ewsContext, L7_uint32 argc, 
                                                  const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 vlanStart = 0; 
  L7_uint32 vlanEnd = 0, vlanCount = 0;
  L7_uint32 vlanList[L7_MAX_VLANS];
  L7_uint32 arg1 = 1,i;
  L7_uint32 enable;        /* whether to enable or disable on these VLANS */
  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_char8 *strVlanList;
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];


  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  if ((numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_switching_CfgDhcpL2RelayCircuitIdVlan);
  }

  /* Get list of VLAN IDs */
  strVlanList = (L7_char8 *) argv[index + arg1];

  if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf),
                            pStrInfo_switching_VlanIdOutOfRange_1,
                            L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    ewsTelnetWrite(ewsContext, tempBuf);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    enable = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    enable = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_switching_CfgDhcpL2RelayCircuitIdVlan);
  }

  if ( (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) &&
       (vlanCount))
  {
    vlanStart = vlanList[0];
    for(i = 0; i < vlanCount; i++)
    {
      if ((vlanList[i] < L7_DOT1Q_MIN_VLAN_ID) ||
          (vlanList[i] > L7_DOT1Q_MAX_VLAN_ID))
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_base_IpDhcpSnoopingInvalidVlanRange);
      }
      if(1 == vlanCount)
      {
        vlanEnd = vlanStart;
      }
      else
      {
        if((i+1) == vlanCount)
        {
          vlanEnd = vlanList[i];
        }
        else if(vlanList[i+1] == vlanList[i]+1)
        {
          continue;
        }
        else
        {
          vlanEnd = vlanList[i];
        }
      }
      rc = usmDbDsL2RelayCircuitIdSet(vlanStart, vlanEnd, enable);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              pStrErr_base_IpDhcpSnoopingInvalidVlanRange);
        }
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_switching_DhcpL2RelayCircuitIdVlanFail);
      } 
      if((i+1) < vlanCount)
      {
        vlanStart = vlanList[i+1];
      }
   }

  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  Configure the DHCP L2 Relay remote-id for Vlan range
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
* @cmdsyntax    [no] dhcp l2relay vlan <vlan-range> remote-id
*
* @cmdhelp
*
* @cmddescript  This command enables and disables DHCP remote-id for a vlan.
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpL2RelayVlanRemoteId(EwsContext ewsContext, L7_uint32 argc, 
                                                 const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 vlanStart = 0; 
  L7_uint32 vlanEnd = 0, vlanCount = 0;
  L7_uint32 vlanList[L7_MAX_VLANS];
  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_uint32 argRemoteId = 1, argVlanRange = numArg, i;
  L7_char8 *strVlanList, remoteId[DS_MAX_REMOTE_ID_STRING];
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uchar8 endOfStr = FD_DHCP_L2RELAY_REMOTE_ID_VLAN_MODE;


  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 3)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_switching_CfgDhcpL2RelayRemoteIdVlan);
    }
    else
    {
      osapiStrncpySafe(remoteId, (L7_char8 *) (argv[index + argRemoteId]), DS_MAX_REMOTE_ID_STRING);
    }
    argVlanRange = 3;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_switching_CfgDhcpL2RelayRemoteIdVlan);
    }
    else
    {
      osapiStrncpySafe(remoteId, &endOfStr, sizeof(endOfStr));
    }
    argVlanRange = 2;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, pStrErr_switching_CfgDhcpL2RelayRemoteIdVlan);
  }

  /* Get list of VLAN IDs */
  strVlanList = (L7_char8 *) argv[index + argVlanRange]; 

  if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf),
                            pStrInfo_switching_VlanIdOutOfRange_1,
                            L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    ewsTelnetWrite(ewsContext, tempBuf);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }


  if ( (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) &&
       (vlanCount))
  {
    vlanStart = vlanList[0];
    for(i = 0; i < vlanCount; i++)
    {
      if ((vlanList[i] < L7_DOT1Q_MIN_VLAN_ID) ||
          (vlanList[i] > L7_DOT1Q_MAX_VLAN_ID))
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_base_IpDhcpSnoopingInvalidVlanRange);
      }
      if(1 == vlanCount)
      {
        vlanEnd = vlanStart;
      }
      else
      {
        if((i+1) == vlanCount)
        {
          vlanEnd = vlanList[i];
        }
        else if(vlanList[i+1] == vlanList[i]+1)
        {
          continue;
        }
        else
        {
          vlanEnd = vlanList[i];
        }
      }
      rc = usmDbDsL2RelayRemoteIdSet(vlanStart, vlanEnd, remoteId);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              pStrErr_base_IpDhcpSnoopingInvalidVlanRange);
        }
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_switching_DhcpL2RelayRemoteIdVlanFail);
      } 
      if((i+1) < vlanCount)
      {
        vlanStart = vlanList[i+1];
      }
   }

  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)

/*********************************************************************
*
* @purpose  Configure the DHCP L2 Relay  for a subscription
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
* @cmdsyntax    [no] dhcp l2relay subscription <subscription-name>
*
* @cmdhelp
*
* @cmddescript  This command enables and disables DHCP L2 relay for a subscription
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpL2RelaySubscription(EwsContext ewsContext, L7_uint32 argc, 
                                         const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0, mode = L7_DISABLE; 
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;
  L7_uchar8 subscriptionName[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX +1];

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, 
                                            pStrErr_switching_DhcpL2RelayCfgModeSubscription);
    }
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, 
                                            pStrErr_switching_DhcpL2RelayCfgModeSubscriptionNo);
    }
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, 
                                          pStrErr_switching_DhcpL2RelayCfgModeSubscription);
  }

  if (strlen(argv[index + 1]) > L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, 
                                          pStrErr_switching_DhcpL2RelayCfgSubscriptionLen);
  }
  memcpy(subscriptionName, (L7_char8 *) argv[index + 1], L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmdbDsL2RelaySubscripionModeSet(intIfNum, subscriptionName,
                                          mode);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_NOT_EXIST)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              pStrErr_switching_DhcpL2RelaySubscriptionNotExistFail);
      }
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_switching_DhcpL2RelayModeSubscriptionFail);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure the DHCP Circuit-id  for a subscription
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
* @cmdsyntax    [no] dhcp l2relay circuit-id subscription <subscription-name>
*
* @cmdhelp
*
* @cmddescript  
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpL2RelayCircuitIdSubscription(EwsContext ewsContext,
                                                         L7_uint32 argc, 
                                         const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0, mode = L7_DISABLE; 
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;
  L7_uchar8 subscriptionName[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX +1];

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, 
                                            pStrErr_switching_DhcpL2RelayCfgCircuitIdModeSubscription);
    }
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, 
                                            pStrErr_switching_DhcpL2RelayCfgCircuitIdModeSubscriptionNo);
    }
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, 
                                          pStrErr_switching_DhcpL2RelayCfgCircuitIdModeSubscription);
  }

  if (strlen(argv[index + 1]) > L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, 
                                          pStrErr_switching_DhcpL2RelayCfgSubscriptionLen);
  }
  memcpy(subscriptionName, (L7_char8 *) argv[index + 1], L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmdbDsL2RelaySubscripionCircuitIdModeSet(intIfNum, subscriptionName,
                                          mode);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_NOT_EXIST)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              pStrErr_switching_DhcpL2RelaySubscriptionNotExistFail);
      }
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, pStrErr_switching_DhcpL2RelayCircuitIdSubscriptionFail);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configure the DHCP Remote-id for a subscription
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
* @cmdsyntax    [no] dhcp l2relay remote-id <emoteId-string> subscription <subscription-name>
*
* @cmdhelp
*
* @cmddescript  
*
* @end
*
*********************************************************************/
const L7_char8 *commandDhcpL2RelayRemoteIdSubscription(EwsContext ewsContext,
                                                         L7_uint32 argc, 
                                         const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 intIfNum = 0; 
  L7_uint32 numArg = 0;
  L7_uint32 unit = 1, s, p;
  L7_uchar8 subscriptionName[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX +1];
  L7_uchar8 remoteIdStr[DS_MAX_REMOTE_ID_STRING];

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 3)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, 
                                            pStrErr_switching_DhcpL2RelayCfgRemoteIdModeSubscription);
    }
    osapiStrncpySafe(remoteIdStr, (L7_char8 *) (argv[index + 1]), DS_MAX_REMOTE_ID_STRING-1);
    osapiStrncpySafe(subscriptionName, (L7_char8 *) (argv[index + 3]), L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, 
                                            pStrErr_switching_DhcpL2RelayCfgRemoteIdModeSubscriptionNo);
    }
   else
   {
    osapiStrncpySafe(subscriptionName, (L7_char8 *) (argv[index + 2]), L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
   }
   memset(remoteIdStr, 0, DS_MAX_REMOTE_ID_STRING);
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, 
                                          pStrErr_switching_DhcpL2RelayCfgRemoteIdModeSubscription);
  }

  if (strlen(subscriptionName) > L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                          ewsContext, 
                                          pStrErr_switching_DhcpL2RelayCfgSubscriptionLen);
  }
  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmdbDsL2RelaySubscripionRemoteIdModeSet(intIfNum, subscriptionName,
                                          remoteIdStr);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_NOT_EXIST)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, 
                                              pStrErr_switching_DhcpL2RelaySubscriptionNotExistFail);
      }
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                            ewsContext, 
                                            pStrErr_switching_DhcpL2RelayRemoteIdSubscriptionFail);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}
#endif
#endif
