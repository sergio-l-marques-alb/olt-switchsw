/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_show_dhcp_snooping.c
*
* @purpose DHCP snooping show commands for the cli
*
* @component DHCP snooping
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
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_cli.h"
#include "datatypes.h"
#include "clicommands_card.h"
#include "clicommands_dhcp_snooping.h"
#include "usmdb_dhcp_snooping.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "dhcp_snooping_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include "usmdb_pml_api.h"


/*********************************************************************
*
* @purpose Displays the DHCP Snooping information
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes     Here we  display all DHCP Snooping information.
*
* @cmdsyntax    show ip dhcp snooping
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpDhcpSnooping(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_BOOL trust;
  L7_BOOL first = L7_TRUE;
  static L7_uint32 intIfNum;
  L7_uint32 globalConfig = L7_DISABLE;
  L7_BOOL verifyMac = L7_FALSE;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 logbuf[32];
  static L7_uint32 nextvid;
  L7_uint32 numArg=0;         
  L7_uint32 count = 0;
  L7_uint32 temp = 0,val=0;
  L7_uint32 u, s, p;
  static L7_BOOL interfaceCompleted = L7_FALSE;
  L7_uint32 vlanStart = 0, vlanEnd = 0;
  L7_uchar8 vlanStr[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL portEnabled = L7_FALSE;
  L7_BOOL log;
  L7_uint32 bytesWritten = 0;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_ShowDhcpSnooping);
  }

  cliCmdScrollSet( L7_FALSE);


  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    interfaceCompleted = L7_FALSE;

    /* Admin Mode */
    ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext, 
                            pStrInfo_base_DhcpSnoopAdminMode);
    if (usmDbDsAdminModeGet(&globalConfig) == L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, strUtilEnabledDisabledGet(globalConfig, 
                                                           pStrInfo_common_NotApplicable));
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
    }

    /* verify mac */
    ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext, 
                            pStrInfo_base_DhcpSnoopVerifyMac);
    if (usmDbDsVerifyMacGet(&verifyMac) == L7_SUCCESS)
    {
      if (verifyMac)
      {
        ewsTelnetWrite(ewsContext, "enabled");
      }
      else
      {
        ewsTelnetWrite(ewsContext, "disabled");
      }
    }

    /* List VLANs where DHCP snooping is enabled */
    ewsTelnetWriteAddBlanks(1, 1, 0, 1, L7_NULLPTR, ewsContext, 
                            "DHCP snooping is enabled on the following VLANs:");
    rc = usmDbDsNextEnabledVlanRangeGet(&vlanStart, &vlanEnd);
    while ((rc == L7_SUCCESS) && (vlanStart != 0))
    {
      if (!first)
      {
        ewsTelnetWrite(ewsContext, ", ");
      }
      first = L7_FALSE;
      if (vlanEnd != vlanStart)
        osapiSnprintf(vlanStr, L7_CLI_MAX_STRING_LENGTH, "%u - %u", vlanStart, vlanEnd);
      else
        osapiSnprintf(vlanStr, L7_CLI_MAX_STRING_LENGTH, "%u", vlanStart);

      /* Limit length of line of output, and avoid overrunning vlanStr. */
      if (bytesWritten > 70)
      {
        cliSyntaxBottom(ewsContext);
        bytesWritten = 0;
      }
      ewsTelnetWrite(ewsContext, vlanStr);
      bytesWritten += (strlen(vlanStr) + 2);   /* comma and space */
      rc = usmDbDsNextEnabledVlanRangeGet(&vlanStart, &vlanEnd);
    }

    cliSyntaxBottom(ewsContext);

    intIfNum = 0;
    nextvid = 1;
    temp = CLI_MAX_SCROLL_LINES-10;
  }
  else
  {
    if (L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
    temp = CLI_MAX_SCROLL_LINES-6;
  }

  if (interfaceCompleted == L7_FALSE)
  {
    /* Enabled Interfaces */
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,
                             pStrInfo_base_IntfTrusted);
    ewsTelnetWrite(ewsContext,
                   "\r\n-----------  ----------   ----------------\r\n");

    while ((usmDbValidIntIfNumNext(intIfNum, &intIfNum) == L7_SUCCESS) )
    {
      /* get unit slot and port for this interface*/
      if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ((usmDbDsPortEnabledGet(intIfNum, &portEnabled) == L7_SUCCESS) &&
          (portEnabled) &&
          (usmDbDsIntfTrustGet(intIfNum, &trust) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(u, s, p));
        memset (buf, 0,sizeof(buf));
        if (trust)
        {
          osapiSnprintf(buf, sizeof(buf), "%-11s  %-13s", stat, pStrInfo_common_Yes);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "%-11s  %-13s", stat, pStrInfo_common_No);
        }

        if (usmDbDsIntfLogInvalidGet(intIfNum, &log) == L7_SUCCESS)
        {
          osapiSnprintf(logbuf, sizeof(logbuf), "%-16s\r\n", 
                        log ? pStrInfo_common_Yes : pStrInfo_common_No);
          strncat(buf, logbuf, sizeof(logbuf));
        }

        ewsTelnetWrite(ewsContext, buf);
        /*hasInterfaces = L7_TRUE; */
        count++;

        if (count == temp)
        {
          cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
          cliSyntaxBottom(ewsContext);
          osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
          for (val=1; val<argc; val++)
          {
            OSAPI_STRNCAT(cmdBuf, " ");
            OSAPI_STRNCAT(cmdBuf, argv[val]);
          }
          cliAlternateCommandSet(cmdBuf);
          return pStrInfo_common_Name_2;
          /*cliAlternateCommandSet(pStrInfo_base_ShowIpDhcpFilterAlterCmd);
          return pStrInfo_common_Name_2;*/               /*--More-- or (q)uit */
        }
      }     
    }
  }
  interfaceCompleted = L7_TRUE;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
* @purpose Display DHCP Snooping bindings
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes        Need to add scrolling
*
* @cmdsyntax    show ip dhcp snooping binding [vlan <vlan-id>] [interface <usp>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpDhcpSnoopingBinding(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 **argv, L7_uint32 index)
{
  static dhcpSnoopBinding_t binding;
  L7_uchar8 ifName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 ipAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 macStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit, intSlot, intPort;
  L7_uint32 u, s, p;
  L7_uint32 intIfNum = 0;           /* user specified interface */
  L7_uint32 vlanId = 0;             /* user specified VLAN ID */
  L7_BOOL   isStatic  = L7_TRUE;    /* user specified option staic option */
  L7_BOOL   isDynamic = L7_TRUE;    /* user specified option staic option */
  L7_uint32 numArg;
  L7_uint32 intfPos = 0;
  L7_uint32 vlanPos = 0;
  L7_uint32 filterPos = 0;          

  
  L7_uint32 val = 0;
  L7_uint32 count = 0;
  L7_RC_t rc;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];

  cliCmdScrollSet( L7_FALSE);
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      memset(&binding.key.macAddr, 0, L7_MAC_ADDR_LEN);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    memset(&binding.key.macAddr, 0, L7_MAC_ADDR_LEN);
  }

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet(); 
  if (numArg > 5)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_ShowDhcpSnoopingBinding);
  }
  
  if (numArg == 1)  /* Specifying only static or dynamic option */
  {
     if ( (strcmp(argv[index+1],"static")!=0) &&
          (strcmp(argv[index+1],"dynamic")!=0))
     {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                              ewsContext,
         "Use 'show ip dhcp binding [static | dynamic] [interface <u/s/p>] [vlan <vlan id>]'.");

     }
     if ( (strcmp(argv[index+1],"static") != 0))
     {
       isStatic = L7_FALSE;
     }
     if ( (strcmp(argv[index+1],"dynamic") != 0))
     {
       isDynamic = L7_FALSE;
     }
  }
 else if (numArg > 1) /* Display only selected interface bindings or on a given vlan*/
  {

     if ( numArg == 2)
     {

       if ( strcmp(argv[index+1],"interface") ==0 )
       {
         intfPos = index+2;
       }
       else if ( strcmp(argv[index+1],"vlan") ==0 )
       {
          vlanPos=index+2;
       }
     }
     else if ( numArg == 3)
     {
       if ( strcmp(argv[index+1],"interface") ==0 )
       {
         intfPos = index+2;
         filterPos = index+3;
       }
       else if ( strcmp(argv[index+1],"vlan") ==0 )
       {
          vlanPos=index+2;
          filterPos = index+3;
       }
       else if ( strcmp(argv[index+2],"interface") ==0)
       {
         intfPos = index+3;
         filterPos = index+1;
       }
       else if ( strcmp(argv[index+2],"vlan") ==0)
       {
         vlanPos  = index+3;
         filterPos = index+1;
       }

     }
     else if ( numArg == 4)
     {
       if ( strcmp(argv[index+1],"interface") ==0 )
       {
         intfPos = index+2;
         vlanPos = index+4;
       }
       else if ( strcmp(argv[index+3],"interface") ==0 )
       {
         intfPos = index+4;
         vlanPos = index+2;
       }

     }
    else if ( numArg == 5)
     {
       if ( strcmp(argv[index+1],"interface") ==0 )
       {
          intfPos = index+2;

          if ( strcmp(argv[index+3], "vlan") == 0)
          {
             vlanPos = index+4;
             filterPos = index +5;
          }
          else
          {
            filterPos = index+3;
            vlanPos =index+5;
          }

       }
       else if ( strcmp(argv[index+2],"interface") ==0)
       {

            filterPos = index+1;
            vlanPos =index+5;
            intfPos = index+3;

       }
       else if ( strcmp(argv[index+3],"interface") ==0)
       {
          intfPos = index+4;
          filterPos = index+5;
          vlanPos =index+2;

       }
       else if ( strcmp(argv[index+4],"interface") ==0)
       {

          intfPos = index+5;
          if ( strcmp(argv[index+2], "vlan") == 0)
          {
             vlanPos = index+3;
            filterPos = index+1;
          }
          else
          {
            filterPos = index+3;
            vlanPos =index+2;

          }


       }

     }
   }

  /* If user specified an interface, get it. */
  {
    if (intfPos)
    {
      if (cliIsStackingSupported() == L7_TRUE)
      {
        if (cliValidSpecificUSPCheck(argv[intfPos], &u, &s, &p) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                                pStrErr_common_InvalidSlotPort_1);
        }
  
        /* Get interface and check its validity */
        if (usmDbIntIfNumFromUSPGet(u, s, p, &intIfNum) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                                ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }
      else
      {
        u = cliGetUnitId();
        if (cliSlotPortToIntNum(ewsContext, argv[intfPos], &s, 
                                &p, &intIfNum) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
    }
  
    if (vlanPos)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[vlanPos], &vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, "Invalid VLAN ID");
      }
    }
    if ( filterPos )
    {
      if ( strcmp (argv[filterPos], "static")!=0)
      {
         isStatic = L7_FALSE;
      }
   
      if ( strcmp (argv[filterPos], "dynamic")!=0)
      {
         isDynamic = L7_FALSE;
      }

    }
  }

  cliSyntaxTop(ewsContext);
  osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH, 
                "Total number of bindings:  %u", usmDbDsBindingsCount());
  ewsTelnetWrite(ewsContext, buf);
  cliSyntaxTop(ewsContext);
  cliSyntaxTop(ewsContext);

  ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                          "   MAC Address       IP Address     VLAN   Interface    Type    Lease (Secs)");
  ewsTelnetWrite(ewsContext,
                 "\r\n-----------------  ---------------  ----  -----------  -------  -----------\r\n");
  count = 0;
  rc = usmDbDsBindingGet(&binding);
  if (rc != L7_SUCCESS)
  {
    rc = usmDbDsBindingGetNext(&binding);
  } 
  while ( (rc  == L7_SUCCESS) &&
          (count < CLI_MAX_SCROLL_LINES-6)
        )
  {
    /* skip tentative bindings */
    if (binding.bindingType == DS_BINDING_TENTATIVE)
    {
      rc =  usmDbDsBindingGetNext(&binding);
      continue;
    }

    if ( ((binding.bindingType == DS_BINDING_STATIC) &&  (isStatic == L7_FALSE)) ||
         ((binding.bindingType == DS_BINDING_DYNAMIC) && (isDynamic == L7_FALSE))
       )
    {
      rc =  usmDbDsBindingGetNext(&binding);
      continue;
    } 

    /* Restrict to interface and VLAN user specified */
    if ((intIfNum && (binding.intIfNum != intIfNum)) ||
        (vlanId && (binding.vlanId != vlanId)))
    {
      rc =  usmDbDsBindingGetNext(&binding);
      continue;
    }

    /* MAC address */
    osapiSnprintf(macStr, sizeof(macStr),"%02X:%02X:%02X:%02X:%02X:%02X",
                  binding.key.macAddr[0], binding.key.macAddr[1], binding.key.macAddr[2], 
                  binding.key.macAddr[3], binding.key.macAddr[4], binding.key.macAddr[5]);

    /* IP address */
    usmDbInetNtoa(binding.ipAddr, ipAddrStr);

    /* Interface */
    if (usmDbUnitSlotPortGet(binding.intIfNum, &unit, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(ifName, sizeof(ifName), 
                    cliDisplayInterfaceHelp(unit, intSlot, intPort));
    }
    else
    {
      osapiSnprintf(ifName, sizeof(ifName), pStrErr_common_Err);
    }

    if (binding.bindingType == DS_BINDING_DYNAMIC)
    {
      osapiSnprintf(buf, sizeof(buf), "%17s  %15s  %4u  %11s  %7s  %11u\r\n", 
                    macStr, ipAddrStr, binding.vlanId, ifName, 
                    dsBindingTypeNames[binding.bindingType], binding.remLease);
    }
    else if (binding.bindingType == DS_BINDING_STATIC)
    {
      /* don't print lease time for static entries */
      osapiSnprintf(buf, sizeof(buf), "%17s  %15s  %4u  %11s  %7s\r\n", 
                    macStr, ipAddrStr, binding.vlanId, ifName, 
                    dsBindingTypeNames[binding.bindingType]);
    }
    ewsTelnetWrite(ewsContext, buf);
    rc =  usmDbDsBindingGetNext(&binding);
    count = count +1;
  }
  if (rc != L7_SUCCESS)
  {
    memset(&binding.key.macAddr, 0, L7_MAC_ADDR_LEN);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

    for (val=1; val<argc; val++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[val]);
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;
  }

}

/*********************************************************************
* @purpose Display DHCP Snooping statistics
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes        
*
* @cmdsyntax    show ip dhcp snooping statistics
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpDhcpSnoopingStats(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 intIfNum = 0;
  dhcpSnoopIntfStats_t intfStats;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 ifName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 u, s, p;
  L7_BOOL trust;
  L7_BOOL portEnabled;
  L7_uint32 numArg = cliNumFunctionArgsGet();   

  cliSyntaxTop(ewsContext);

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_ShowDhcpSnoopingStats);
  }

  ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                          " Interface    MAC Verify   Client Ifc   DHCP Server");
  ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                          "               Failures     Mismatch    Msgs Rec'd ");
  ewsTelnetWrite(ewsContext,
                 "\r\n-----------   ----------   ----------   -----------\r\n");

  while ((usmDbValidIntIfNumNext(intIfNum, &intIfNum) == L7_SUCCESS))
  {
    /* get unit slot and port for this interface*/
    if (usmDbUnitSlotPortGet(intIfNum, &u, &s, &p) != L7_SUCCESS)
    {
      continue;
    }
    osapiSnprintf(ifName, sizeof(ifName), cliDisplayInterfaceHelp(u, s, p));

    /* Only show stats for interfaces where DHCP snooping is enabled and 
     * the port is untrusted. */
    if ((usmDbDsPortEnabledGet(intIfNum, &portEnabled) == L7_SUCCESS) &&
        (portEnabled) &&
        (usmDbDsIntfTrustGet(intIfNum, &trust) == L7_SUCCESS) && !trust)
    {
      if (usmDbDsIntfStatsGet(intIfNum, &intfStats) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%-11s   %10u   %10u    %10u\r\n", 
                      ifName, intfStats.macVerify, intfStats.intfMismatch, 
                      intfStats.untrustedSvrMsg);
        ewsTelnetWrite(ewsContext, buf);
      }
    }
  }
  return cliSyntaxReturnPrompt(ewsContext, "");
}

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
*
* @purpose Displays IP Source Guard configuration 
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes        Lists the IPSG configuration on IPSG valid interfaces.
*
* @cmdsyntax    show ip verify 
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpVerify(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
  static L7_uint32 intIfNum = 0;
  L7_uchar8 ifName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 filterTypeStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit, intSlot, intPort;
  L7_BOOL verifyIp = L7_FALSE;
  L7_BOOL verifyMac = L7_FALSE;
  L7_uint32 count = 0;
  L7_uint32 cfgIntf = 0;
  L7_uint32 numArg=0;
  L7_uint32 val;
  L7_RC_t rc = L7_FAILURE;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[4*L7_CLI_MAX_STRING_LENGTH];

  cliCmdScrollSet( L7_FALSE);
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      intIfNum = 0;
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    intIfNum = 0;
    cliSyntaxTop(ewsContext);
    /* using a pstring for this is a waste. It's NEVER going to get reused. */
    ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                            "Interface    Filter Type ");
    ewsTelnetWrite(ewsContext,
                 "\r\n-----------  -----------");
  }

  numArg = cliNumFunctionArgsGet();
  if (numArg != 0 && numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext, pStrErr_base_ShowIpVerify);
  }
  if (numArg == 2)
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[index+2], &unit, &intSlot, &intPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                              pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &cfgIntf) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      if (cliSlotPortToIntNum(ewsContext, argv[index+2], &intSlot,
                              &intPort, &cfgIntf) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }

  }

  while ((rc = usmDbValidIntIfNumNext (intIfNum, &intIfNum)) == L7_SUCCESS)
  {
    if (numArg == 2)
    {
      intIfNum = cfgIntf;
    }
    if (usmDbDsIntfIsValid(intIfNum) != L7_TRUE)
    {
      if (numArg == 2)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      continue;
    }
   /* Interface */
    memset (ifName, 0, sizeof(ifName));
    if (usmDbUnitSlotPortGet(intIfNum, &unit, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(ifName, sizeof(ifName),
                    cliDisplayInterfaceHelp(unit, intSlot, intPort));
    }
    else
    {
      osapiSnprintf(ifName, sizeof(ifName), pStrErr_common_Err);
    }

    /* filter type */
    if (usmDbIpsgVerifySourceGet(intIfNum, &verifyIp, &verifyMac) == L7_SUCCESS)
    {
      if (verifyMac)
      {
        osapiSnprintf(filterTypeStr, sizeof(filterTypeStr), "ip-mac");
      }
      else if(verifyIp)
      {
        osapiSnprintf(filterTypeStr, sizeof(filterTypeStr), "ip");
      }
      else
      {
        osapiSnprintf(filterTypeStr, sizeof(filterTypeStr), pStrInfo_common_NotApplicable);
      }
    }
    else
    {
      osapiSnprintf(filterTypeStr, sizeof(filterTypeStr), pStrErr_common_Err);
    }

    osapiSnprintf(buf, sizeof(buf), "\r\n%-11s  %-11s",
                  ifName, filterTypeStr);
    ewsTelnetWrite(ewsContext, buf);
    
    count++;
    if (numArg == 2 || count >= CLI_MAX_SCROLL_LINES-6)
    {
      break;
    }
  }

  if (rc == L7_SUCCESS && count >= CLI_MAX_SCROLL_LINES-6)
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

    for (val=1; val<argc; val++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[val]);
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;
  }
  else
  {
   intIfNum = 0;
   return cliSyntaxReturnPrompt (ewsContext, "");
  }
}

/*********************************************************************
*
* @purpose Displays IP Source Guard data
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes        Lists the IPSG entries on only interfaces where
*               IPSG is enabled. 
*
* @cmdsyntax    show ip verify source [interface <u/s/p>] [static | dynamic]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpVerifySource(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
  static L7_uint32 intIfNum = 0;
  L7_uint32 cfgIntf = 0;    /* intIfNum configured by user */
  L7_uchar8 ifName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 filterTypeStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 ipAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 macStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[4*L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg=0;         
  L7_uint32 unit, intSlot, intPort;
  L7_BOOL isIntfCfg = L7_FALSE;
  static L7_ushort16 vlanId = 0;
  static L7_uint32 ipAddr = 0;
  static L7_uint32 entryType = 0;
  static L7_enetMacAddr_t macAddr;
  L7_BOOL verifyIp = L7_FALSE;
  L7_BOOL verifyMac = L7_FALSE;
  L7_uint32 pmlIntfMode, pmlAdminMode;
  L7_uint32 count = 0;
  L7_uint32 val = 0;
  L7_RC_t rc;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  

  cliCmdScrollSet( L7_FALSE);
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      intIfNum = 0;
      vlanId = 0;
      ipAddr = 0;
      memset ( &macAddr, 0, sizeof(L7_enetMacAddr_t));

      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    intIfNum = 0;
    vlanId = 0;
    ipAddr = 0;
    memset ( &macAddr, 0, sizeof(L7_enetMacAddr_t));
  }
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ((numArg != 0) && (numArg != 2))
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_base_ShowDhcpSnooping);
  }
  if (numArg == 2) /* Display only selected interface bindings */
  {
    /* Get interface user specified */
    isIntfCfg = L7_TRUE;
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[index+2], &unit, &intSlot, &intPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                              pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &cfgIntf) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, argv[index+2], &intSlot, 
                              &intPort, &cfgIntf) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }


  }

  count = 0;
  rc = usmDbIpsgBindingGet(&intIfNum,
                           &vlanId, &ipAddr,
                           &macAddr, &entryType);
  if (rc != L7_SUCCESS)
  {
      rc = usmDbIpsgBindingGetNext(&intIfNum,
                           &vlanId, &ipAddr,
                           &macAddr, &entryType);
  }
  
  
  /* using a pstring for this is a waste. It's NEVER going to get reused. */
  ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                           "Interface    Filter Type    IP Address       MAC Address      VLAN");
  ewsTelnetWrite(ewsContext,
                 "\r\n-----------  -----------  ---------------  -----------------  -----\r\n");

  /* Iterate through set of IPSG bindings. Key is {intIfNum, vlanId, ipAddr, macAddr} */
  while ( (rc == L7_SUCCESS) &&
          (count < CLI_MAX_SCROLL_LINES-6) )
  {
    if (isIntfCfg && (intIfNum != cfgIntf))
    {
      rc = usmDbIpsgBindingGetNext(&intIfNum,
                           &vlanId, &ipAddr,
                           &macAddr, &entryType);
      continue;
    }

    /* Interface */
    memset (ifName, 0, sizeof(ifName));
    memset (macStr, 0, sizeof(macStr));
    if (usmDbUnitSlotPortGet(intIfNum, &unit, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(ifName, sizeof(ifName), 
                    cliDisplayInterfaceHelp(unit, intSlot, intPort));
    }
    else
    {
      osapiSnprintf(ifName, sizeof(ifName), pStrErr_common_Err);
    }

    /* filter type */
    if (usmDbIpsgVerifySourceGet(intIfNum, &verifyIp, &verifyMac) == L7_SUCCESS)
    {
      if (verifyMac)
        osapiSnprintf(filterTypeStr, sizeof(filterTypeStr), "ip-mac");
      else
        osapiSnprintf(filterTypeStr, sizeof(filterTypeStr), "ip");
    }
    else
    {
      osapiSnprintf(filterTypeStr, sizeof(filterTypeStr), pStrErr_common_Err);
    }

    /* IP address*/
    usmDbInetNtoa(ipAddr, ipAddrStr);

    /* MAC address */
    if (verifyMac)
    {
      /* If port-security is disabled on the interface, print "permit-all" */
      if ((usmDbPmlAdminModeGet(1, &pmlAdminMode) == L7_SUCCESS) &&
          (pmlAdminMode == L7_ENABLE) &&
          (usmDbPmlIntfModeGet(1, intIfNum, &pmlIntfMode) == L7_SUCCESS) &&
          (pmlIntfMode == L7_ENABLE))
      {
        osapiSnprintf(macStr, sizeof(macStr),"%02X:%02X:%02X:%02X:%02X:%02X",
                      macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], 
                      macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
      }
      else
      {
        osapiSnprintf(macStr, sizeof(macStr), "permit-all");
      }
    }

    osapiSnprintf(buf, sizeof(buf), "%-11s  %-11s  %15s  %17s  %4u\r\n", 
                  ifName, filterTypeStr, ipAddrStr, macStr, vlanId);
    ewsTelnetWrite(ewsContext, buf);
    count = count +1;
    rc = usmDbIpsgBindingGetNext(&intIfNum,
                           &vlanId, &ipAddr,
                           &macAddr, &entryType);
    cliSyntaxBottom(ewsContext);
  }
  if (rc != L7_SUCCESS)
  {
    intIfNum = 0;
    vlanId = 0;
    ipAddr = 0;
    memset ( &macAddr, 0, sizeof(L7_enetMacAddr_t));
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

    for (val=1; val<argc; val++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[val]);
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;
  }
     
}
#endif

/*********************************************************************
*
* @purpose Displays the DHCP Snooping database configuration
*          information
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
*
* @cmdsyntax    show ip dhcp snooping database
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const char *commandShowIpDhcpSnoopingDatabase(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
   L7_RC_t   rc;
   L7_uint32 ipAddr;
   L7_uint32 val;
   L7_uchar8 ipAddrStr[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 fileName [L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

    /* pStrInfo_base_DhcpSnoopAdminMode */
   ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                            pStrInfo_base_DhcpSnoopStorageLocation);

   if ( ((rc=usmDbDsDbIpAddrGet(&ipAddr)) == L7_SUCCESS) &&
        (ipAddr == 0) )
   {
         ewsTelnetWrite(ewsContext, "local\r\n");
   }

   if ( (rc == L7_SUCCESS) && (ipAddr > 0))
   {

     usmDbInetNtoa(ipAddr, ipAddrStr);
     if ( (rc=usmDbDsDbFileNameGet(fileName)) == L7_SUCCESS)
     {
       osapiSnprintf(buf, sizeof(buf), "/%s:/%s\r\n",
                    ipAddrStr,fileName);
     }
     ewsTelnetWrite(ewsContext, buf);
   }

   if ( (rc=usmDbDsDbStoreIntervalGet(&val)) == L7_SUCCESS)
   {
    ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                          pStrInfo_base_DhcpSnoopStorageInterval);

    osapiSnprintf(buf, sizeof(buf), "%d\r\n",
                    val);
    ewsTelnetWrite(ewsContext, buf);
   }

  return cliSyntaxReturnPrompt(ewsContext, "");
}

#ifdef L7_IPSG_PACKAGE

/*********************************************************************
*
* @purpose Displays IP Source Guard Bindings
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes        Lists the IPSG bindings 
*
* @cmdsyntax    show ip source binding [dhcp-snooping | static]
*                                      [inteface interface-id]
*                                      [vlan vlan-id]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpsgBinding(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
  static L7_uint32 intIfNum = 0;
  L7_uint32 cfgIntf = 0;    /* intIfNum configured by user */
  L7_uchar8 ifName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 filterTypeStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 ipAddrStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 macStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[4*L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg=0;
  L7_uint32 unit, intSlot, intPort;
  L7_BOOL isIntfCfg = L7_FALSE;
  static L7_ushort16 vlanId = 0;
  static L7_uint32 ipAddr = 0;
  static L7_uint32 entryType = 0;
  static L7_enetMacAddr_t macAddr;
  L7_BOOL isStatic = L7_TRUE;
  L7_BOOL isDynamic = L7_TRUE;
  L7_uint32 count = 0;
  L7_uint32 cfgVlanId = 0;
  L7_uint32 val = 0;
  L7_RC_t rc;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 vlanPos=0, interfacePos=0, filterPos=0;


  cliCmdScrollSet( L7_FALSE);
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      intIfNum = 0;
      vlanId = 0;
      ipAddr = 0;
      memset ( &macAddr, 0, sizeof(L7_enetMacAddr_t));

      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    intIfNum = 0;
    vlanId = 0;
    ipAddr = 0;
    memset ( &macAddr, 0, sizeof(L7_enetMacAddr_t));
  }
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ((numArg != 0) && (numArg > 5))
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                          ewsContext, pStrErr_base_ShowDhcpSnooping);
  }
  if (numArg == 1)  /* Specifying only static or dynamic option */
  {
     if ( (strcmp(argv[index+1],"static")!=0) &&
          (strcmp(argv[index+1],"dhcp-snooping")!=0))
     {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                              ewsContext,
         "Use 'show ip source binding [static | dynamic] [interface <u/s/p>] [vlan <vlan id>]'.");

     }
     if ( (strcmp(argv[index+1],"static") != 0))
     {
       isStatic = L7_FALSE;
     }
     if ( (strcmp(argv[index+1],"dhcp-snooping") != 0))
     {
       isDynamic = L7_FALSE;
     }
  }
  else if (numArg > 1) /* Display only selected interface bindings or on a given vlan*/
  {
   
     if ( numArg == 2)
     {        
      
       if ( strcmp(argv[index+1],"interface") ==0 )
       {
         interfacePos = index+2;
       }
       else if ( strcmp(argv[index+1],"vlan") ==0 )
       {
          vlanPos=index+2;
       }
     }
     else if ( numArg == 3)
     {
       if ( strcmp(argv[index+1],"interface") ==0 )
       {
         interfacePos = index+2;
         filterPos = index+3;
       }
       else if ( strcmp(argv[index+1],"vlan") ==0 )
       {
          vlanPos=index+2;
          filterPos = index+3;
       }
       else if ( strcmp(argv[index+2],"interface") ==0)
       {
         interfacePos = index+3; 
         filterPos = index+1;
       } 
       else if ( strcmp(argv[index+2],"vlan") ==0)
       {
         vlanPos  = index+3; 
         filterPos = index+1;
       } 

     }
     else if ( numArg == 4)
     {
       if ( strcmp(argv[index+1],"interface") ==0 )
       {
         interfacePos = index+2;
         vlanPos = index+4;
       }
       else if ( strcmp(argv[index+3],"interface") ==0 )
       {
         interfacePos = index+4;
         vlanPos = index+2;
       }

     }
     else if ( numArg == 5)
     {

       if ( strcmp(argv[index+1],"interface") ==0 )
       {
          interfacePos = index+2;
          
          if ( strcmp(argv[index+3], "vlan") == 0)
          {
             vlanPos = index+4;
             filterPos = index +5;
          }
          else
          {
            filterPos = index+3;
            vlanPos =index+5;

          }

       }
       else if ( strcmp(argv[index+2],"interface") ==0)
       {
        
            filterPos = index+1;
            vlanPos =index+5;
            interfacePos = index+3;

       }
       else if ( strcmp(argv[index+3],"interface") ==0)
       {
        
            filterPos = index+5;
            vlanPos =index+2;
            interfacePos = index+4;

       }
       else if ( strcmp(argv[index+4],"interface") ==0)
       {
          
          interfacePos = index+5;
          if ( strcmp(argv[index+2], "vlan") == 0)
          {
             vlanPos = index+3;
             filterPos = index+1;
          }
          else
          {
            filterPos = index+3;
            vlanPos =index+2;

          }
          

       }
       
     }
   }
       
   if ( interfacePos )
   { 
     /* Get interface user specified */
     isIntfCfg = L7_TRUE;
     if (cliIsStackingSupported() == L7_TRUE)
     {
       if (cliValidSpecificUSPCheck(argv[interfacePos], &unit, &intSlot, &intPort) != L7_SUCCESS)
       {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                            pStrErr_common_InvalidSlotPort_1);
       }
       /* Get interface and check its validity */
       if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &cfgIntf) != L7_SUCCESS)
       {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                            ewsContext, pStrErr_common_InvalidSlotPort_1);
       }
     }
     else
     {
       unit = cliGetUnitId();
       if (cliSlotPortToIntNum(ewsContext, argv[interfacePos], &intSlot,
                              &intPort, &cfgIntf) != L7_SUCCESS)
       {
         return cliPrompt(ewsContext);
       }
     }
   }
   if ( vlanPos )
   {
      if (cliConvertTo32BitUnsignedInteger(argv[vlanPos], (L7_uint32 *)&cfgVlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext, "Invalid VLAN ID");
      }
   }
   if ( filterPos )
   {
       if ( (strcmp(argv[filterPos],"static") != 0))
       {
         isStatic = L7_FALSE;
       }
       if ( (strcmp(argv[filterPos],"dhcp-snooping") != 0)) /* dhcp-snooping case */
       {
         isDynamic = L7_FALSE;
       }
  }

  count = 0;
  rc = usmDbIpsgBindingGet(&intIfNum,
                           &vlanId, &ipAddr,
                           &macAddr, &entryType);
  if (rc != L7_SUCCESS)
  {
      rc = usmDbIpsgBindingGetNext(&intIfNum,
                           &vlanId, &ipAddr,
                           &macAddr, &entryType);
  }


  /* using a pstring for this is a waste. It's NEVER going to get reused. */
  ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                      "   MAC Address       IP Address       Type         VLAN     Interface");
  ewsTelnetWrite(ewsContext,
                 "\r\n-----------------  ---------------  -------------  -----  -------------\r\n");


  /* Iterate through set of IPSG bindings. Key is {intIfNum, vlanId, ipAddr, macAddr} */
  while ( (rc == L7_SUCCESS) &&
          (count < CLI_MAX_SCROLL_LINES-6) )
  {
    if (isIntfCfg && (intIfNum != cfgIntf))
    {
      rc = usmDbIpsgBindingGetNext(&intIfNum,
                           &vlanId, &ipAddr,
                           &macAddr, &entryType);
      continue;
    }
    if ( cfgVlanId &&
        (cfgVlanId != vlanId ))
    {
      rc = usmDbIpsgBindingGetNext(&intIfNum,
                          &vlanId, &ipAddr,
                         &macAddr, &entryType);
      continue;

    }
    if  ( ( (entryType == IPSG_ENTRY_STATIC) && (isStatic == L7_FALSE)) ||
          ( (entryType == IPSG_ENTRY_DYNAMIC)&& (isDynamic == L7_FALSE)) )
    {
     rc = usmDbIpsgBindingGetNext(&intIfNum,
                           &vlanId, &ipAddr,
                           &macAddr, &entryType);
     continue;
    }

    /* Interface */
    memset (ifName, 0, sizeof(ifName));
    if (usmDbUnitSlotPortGet(intIfNum, &unit, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(ifName, sizeof(ifName),
                    cliDisplayInterfaceHelp(unit, intSlot, intPort));
    }
    else
    {
      osapiSnprintf(ifName, sizeof(ifName), pStrErr_common_Err);
    }
    if (entryType == IPSG_ENTRY_STATIC) 
    {
         osapiSnprintf(filterTypeStr, sizeof(filterTypeStr), "static");
    }
    else
    {
        osapiSnprintf(filterTypeStr, sizeof(filterTypeStr), "dhcp-snooping");
    }
    /* IP address*/
    usmDbInetNtoa(ipAddr, ipAddrStr);

    /* MAC address */
      osapiSnprintf(macStr, sizeof(macStr),"%02X:%02X:%02X:%02X:%02X:%02X",
                      macAddr.addr[0], macAddr.addr[1], macAddr.addr[2],
                      macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);

 
    osapiSnprintf(buf, sizeof(buf), "%17s  %15s  %13s  %4u  %11s\r\n",
                  macStr, ipAddrStr, filterTypeStr, vlanId, ifName);
    ewsTelnetWrite(ewsContext, buf);
    count = count +1;
    rc = usmDbIpsgBindingGetNext(&intIfNum,
                           &vlanId, &ipAddr,
                           &macAddr, &entryType);
    cliSyntaxBottom(ewsContext);
  }
  if (rc != L7_SUCCESS)
  {
    intIfNum = 0;
    vlanId = 0;
    ipAddr = 0;
    memset ( &macAddr, 0, sizeof(L7_enetMacAddr_t));
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

    for (val=1; val<argc; val++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[val]);
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;
  }

}
#endif

/*********************************************************************
*
* @purpose Displays the DHCP Snooping information of the interface(s).
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes     Here we  display the interface specific DHCP Snooping info.
*
* @cmdsyntax    show ip dhcp snooping interfaces [<u/s/p>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpDhcpSnoopingIf(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index)
{
  L7_BOOL trust;
  static L7_uint32 intIfNum;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg=0, unit, intSlot, intPort;
  L7_uint32 count = 0, temp = 0, cfgIntf = 0;
  L7_uint32 rate, burst, val;
  L7_BOOL portEnabled = L7_FALSE;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                           ewsContext, pStrErr_base_ShowDaiIf);
  }
  cliCmdScrollSet(L7_FALSE);

  temp = CLI_MAX_SCROLL_LINES-6;

  if (numArg == 1)
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[index+1], &unit, &intSlot, &intPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                              pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &cfgIntf) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, argv[index+1], &intSlot,
                              &intPort, &cfgIntf) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
    ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DsShowIfHeader);
    ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DsShowIfUnits);
    ewsTelnetWrite(ewsContext, pStrInfo_DsShowIfHeaderLine);
  }
  else
  {
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DsShowIfHeader);
      ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DsShowIfUnits);
      ewsTelnetWrite(ewsContext, pStrInfo_DsShowIfHeaderLine);
      intIfNum = 0;
    }
    else
    {
      if (L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
  }
  while((cfgIntf != 0) || (usmDbValidIntIfNumNext(intIfNum, &intIfNum) == L7_SUCCESS))
  {
    if(cfgIntf == 0)
    {
      /* get unit slot and port for this interface*/
      if (usmDbUnitSlotPortGet(intIfNum, &unit, &intSlot, &intPort) != L7_SUCCESS)
      {
        continue;
      }
    }
    else
    {
      /* show ip dhcp snooping interfaces <u/s/p> */
      intIfNum = cfgIntf;
    }

    /* In the display of all interfaces information, we shall show only
     * those interfaces enabled for DHCP Snooping (i.e., with atleast one
     * participating vlan enabled for DHCP Snooping)
     * In the display of specific interface information, we shall show its
     * information irrespective of DHCP enabled for it */
    if (((cfgIntf != 0) ||
         ((usmDbDsPortEnabledGet(intIfNum, &portEnabled) == L7_SUCCESS) &&
          (portEnabled))) &&
         (usmDbDsIntfTrustGet(intIfNum, &trust) == L7_SUCCESS))
    {
      osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(unit, intSlot, intPort));
      memset (buf, 0,sizeof(buf));
      if (trust)
      {
        osapiSnprintf(buf, sizeof(buf), "%-11s  %-13s", stat, pStrInfo_common_Yes);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "%-11s  %-13s", stat, pStrInfo_common_No);
      }

      if (usmDbDsIntfRateLimitGet(intIfNum, &rate) == L7_SUCCESS)
      {
        if(rate == L7_DS_RATE_LIMIT_NONE)
        {
          osapiSnprintf(stat, sizeof(stat), "   %-13s", pStrInfo_base_LimitNone_1);
        }
        else
        {
        {
          osapiSnprintf(stat, sizeof(stat), "   %-13d", rate);
        }
        osapiStrncat(buf, stat, sizeof(buf));
      }
      if (usmDbDsIntfBurstIntervalGet(intIfNum, &burst) == L7_SUCCESS)
      {
        if(rate == L7_DAI_RATE_LIMIT_NONE)
        {
          osapiSnprintf(stat, sizeof(stat), "   %-15s\r\n", pStrInfo_base_NA);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "   %-15d\r\n", burst);
        }
        osapiStrncat(buf, stat, sizeof(buf));
      }
      count++;
      ewsTelnetWrite(ewsContext, buf);
    }

    if(cfgIntf != 0)
      break;

    if (count >= temp)
    {
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliSyntaxBottom(ewsContext);

      osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
      for (val=1; val<argc; val++)
      {
        OSAPI_STRNCAT(cmdBuf, " ");
        OSAPI_STRNCAT(cmdBuf, argv[val]);
      }
      cliAlternateCommandSet(cmdBuf);
      return pStrInfo_common_Name_2;               /*--More-- or (q)uit */
    }
   }
 }
  return cliSyntaxReturnPrompt (ewsContext, "");
}
#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
*
* @purpose Displays the DHCP L2 Relay information.
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes      All options of this show command are included in this function
*
* @cmdsyntax  show dhcp l2relay { all | interfaces [<u/s/p>] | 
                                  vlan <vlan-list> |
                                  circuit-id vlan <vlan-list> | 
                                  remote-id vlan <vlan-list> }
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowDhcpL2Relay(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index)
{
  L7_uchar8 argKeyWord = 1, *showOption;
  L7_BOOL   vlanOption = L7_FALSE, intfOption = L7_FALSE;
  L7_BOOL   cIdOption = L7_FALSE, rIdOption = L7_FALSE;
  L7_uint32 unit,slot,port, vlanStart1 =0, vlanEnd1 = 0;
  L7_uint32 intIfNum = 0, numArg=0;
  L7_uchar8 *vlanStr1, val = 0;
  L7_uint32 vlanList[L7_MAX_VLANS], vlanCount = 0;
  L7_uint32 gblConfig = L7_DISABLE;
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH], logBuf[15];
  L7_uchar8 vlanStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 remoteIdStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 lineCnt = 0;
  L7_uint32 vlanStart = 0, vlanEnd = 0, relayMode, trustMode;
  L7_uint32 bytesWritten = 0, l2RelayMode, cIdMode, rIdMode = L7_DISABLE;
  L7_BOOL first = L7_TRUE, intfSpecific = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 endOfStr = FD_DHCP_L2RELAY_REMOTE_ID_VLAN_MODE;
  static L7_BOOL pagenated = L7_FALSE, intfPagenated = L7_FALSE;
  static L7_uint32 intIfNext = 0, vlanNext = 0;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg < 1 || numArg > 3)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_switching_DhcpL2RelayNumArgs);
  }

  cliCmdScrollSet( L7_FALSE);
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      pagenated = intfPagenated = L7_FALSE;
      intIfNext = L7_NULL;
      vlanNext = L7_NULL;
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
      pagenated = intfPagenated = L7_FALSE;
      intIfNext = L7_NULL;
      vlanNext = L7_NULL;
  }


  showOption = (L7_uchar8 *)(argv[index + argKeyWord]);

  
  /* Do all validations for each of the options in show command.*/
  if (osapiStrncmp(showOption, pStrInfo_base_all_1, sizeof(pStrInfo_base_all_1)) == L7_NULL)
  {
    /* The command is 'show dhcp l2relay all' */
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                             ewsContext, pStrErr_switching_DhcpL2RelayNumArgs);
    }
    intfOption = vlanOption = cIdOption = rIdOption = L7_TRUE;
  }
  else if (osapiStrncmp(showOption, pStrInfo_base_interface, sizeof(pStrInfo_base_interface)) == L7_NULL)
  {
    if (numArg > 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                             ewsContext, pStrErr_switching_DhcpL2RelayNumArgs);
    }  
    if (osapiStrncmp(argv[index+ argKeyWord + 1], pStrInfo_base_all_1, sizeof(pStrInfo_base_all_1)) == L7_NULL)
    {
      /* interface all.*/
      intIfNum = L7_NULL;
    }
    else
    {
      if (cliIsStackingSupported() != L7_TRUE)
      {
        unit = cliGetUnitId();
        if (cliSlotPortToIntNum(ewsContext, argv[index+ argKeyWord + 1], &slot,
                                &port, &intIfNum) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
      else
      {
        if (cliValidSpecificUSPCheck(argv[index + argKeyWord + 1], &unit, &slot, &port) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                                pStrErr_common_InvalidSlotPort_1);
        }
        /* Get interface and check its validity */
        if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }
      intfSpecific = L7_TRUE;
      intIfNum = intIfNum - 1;
    }
    intfOption = L7_TRUE;
  }
  else if (osapiStrncmp(showOption, pStrInfo_base_VlanKeyword, 
                        sizeof(pStrInfo_base_VlanKeyword)) == L7_NULL)
  {
    /* The command is 'show dhcp l2relay vlan [<vlan-list>]' */
    vlanOption = L7_TRUE;
  }
  else if (osapiStrncmp(showOption, pStrInfo_switching_DhcpL2RelayCircuitId, 
                        sizeof(pStrInfo_switching_DhcpL2RelayCircuitId)) == L7_NULL)
  {
    /* The command is 'show dhcp l2relay circuit-id [<vlan-list>]' */
    cIdOption = L7_TRUE;
  }
  else if (osapiStrncmp(showOption, pStrInfo_switching_DhcpL2RelayRemoteId, 
                        sizeof(pStrInfo_switching_DhcpL2RelayRemoteId)) == L7_NULL)
  {
    /* The command is 'show dhcp l2relay remote-id [<vlan-list>]' */
    rIdOption = L7_TRUE;
  }
  else if (osapiStrncmp(showOption, pStrInfo_switching_DhcpL2RelayRelayAgentInfo, 
                        sizeof(pStrInfo_switching_DhcpL2RelayRelayAgentInfo)) == L7_NULL)
  {
    /* The command is 'show dhcp l2relay agent-option [<vlan-list>]' */
    cIdOption = rIdOption = L7_TRUE;
  }

  if (vlanOption == L7_TRUE && cIdOption == L7_TRUE && rIdOption == L7_TRUE)
  {
    /* this is for 'all' option.*/
    vlanStart = L7_DOT1Q_MIN_VLAN_ID;
    vlanEnd = L7_DOT1Q_MAX_VLAN_ID;
  }
  /* The validations for all vlan commands is same, hence do it together.*/
  else if (vlanOption == L7_TRUE ||
           cIdOption == L7_TRUE || rIdOption == L7_TRUE)
  {
    /* Get list of VLAN IDs */
    if (vlanOption != L7_TRUE)
    {
      vlanStr1 = (L7_char8 *) argv[index + argKeyWord + 2];
    }
    else
    {
      vlanStr1 = (L7_char8 *) argv[index + argKeyWord + 1];
    }

    if (L7_SUCCESS != cliParseRangeInput(vlanStr1, &vlanCount, vlanList, L7_MAX_VLANS))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, buf, sizeof(buf),
                              pStrInfo_switching_VlanIdOutOfRange_1,
                              L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
      ewsTelnetWrite(ewsContext, buf);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    vlanStart = vlanList[0];
    vlanEnd = vlanList[vlanCount - 1];

  }
  if (pagenated == L7_FALSE && intfPagenated == L7_FALSE)
  {
    /* Now, Display the information based on show options.*/
    /* Admin Mode */
    ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext, 
                            pStrInfo_switching_DhcpL2RelayAdminMode);
    if (usmDbDsL2RelayAdminModeGet(&gblConfig) == L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, strUtilEnabledDisabledGet(gblConfig, 
                                                           pStrInfo_common_NotApplicable));
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
    }
    lineCnt++;
    ewsTelnetWrite(ewsContext,".\n");
  }

  /* Display interface-specific information.*/
  if (pagenated == L7_FALSE)
  {
    if (intfOption == L7_TRUE)
    {
      /* Enabled Interfaces */
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,
                               pStrInfo_switching_DhcpL2RelayIntfModeTrusted);
      ewsTelnetWrite(ewsContext,
                     "\r\n----------  -----------  --------------\r");
  
      if (intfPagenated != L7_TRUE)
      {
        intIfNext = intIfNum; /* Start from first.*/
      }
      while (usmDbValidIntIfNumNext(intIfNext, &intIfNext) == L7_SUCCESS)
      {
        if (usmDbDsIntfIsValid(intIfNext) != L7_TRUE)
        {
          continue;
        }
        /* get unit slot and port for this interface*/
        if (usmDbUnitSlotPortGet(intIfNext, &unit, &slot, &port) != L7_SUCCESS)
        {
          continue;
        }
  
        if (usmDbDsL2RelayIntfModeGet(intIfNext, &relayMode) != L7_SUCCESS)
        {
          continue;
        }
        if (usmDbDsL2RelayIntfTrustGet(intIfNext, &trustMode) != L7_SUCCESS)
        {
          continue;
        }
  
        if ((relayMode == L7_ENABLE) || (trustMode == L7_TRUE))
        {
          osapiSnprintf(stat, sizeof(stat),"\n %s", cliDisplayInterfaceHelp(unit, slot, port));
          memset (buf, 0,sizeof(buf));
          if (relayMode == L7_ENABLE)
          {
            osapiSnprintf(buf, sizeof(buf), "%-11s  %-13s", stat, pStrInfo_base_Enabled);
          }
          else if (relayMode == L7_DISABLE)
          {
            osapiSnprintf(buf, sizeof(buf), "%-11s  %-13s", stat, pStrInfo_base_Disabled);
          }
  
          osapiSnprintf(logBuf, sizeof(logBuf), "%-16s\r\n", 
                        trustMode ? pStrInfo_switching_DhcpL2RelayTrusted : pStrInfo_switching_DhcpL2RelayUntrusted);
          strncat(buf, logBuf, sizeof(logBuf));
  
          ewsTelnetWrite(ewsContext, buf);
          /*hasInterfaces = L7_TRUE; */
          lineCnt++;
  
          if (lineCnt == CLI_MAX_LINES_IN_PAGE)
          {
            intfPagenated = L7_TRUE;
            cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
            cliSyntaxBottom(ewsContext);
            osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
            for (val=1; val<argc; val++)
            {
              OSAPI_STRNCAT(cmdBuf, " ");
              OSAPI_STRNCAT(cmdBuf, argv[val]);
            }
            cliAlternateCommandSet(cmdBuf);
            return pStrInfo_common_Name_2;
          }
        }    
        if (intfSpecific == L7_TRUE)
        {
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
      }
    }
  }

  /* Display vlan-specific information.*/
  if (vlanOption == L7_TRUE && (cIdOption == L7_FALSE && rIdOption == L7_FALSE))
  {
    /* List VLANs where DHCP L2 relay is enabled */
    ewsTelnetWriteAddBlanks(1, 1, 0, 1, L7_NULLPTR, ewsContext, 
                            "DHCP L2 Relay is enabled on the following VLANs:");
    vlanStart1 = vlanStart;
    rc = usmDbDsL2RelayVlanRangeNextGet(&vlanStart1, &vlanEnd1);
    while ((rc == L7_SUCCESS) && (vlanStart1 != 0))
    {
      if (vlanStart1 > vlanEnd)
      {
        break;
      }
      if (vlanEnd1 != L7_NULL && vlanEnd1 < vlanStart)
      {
        rc = usmDbDsL2RelayVlanRangeNextGet(&vlanStart1, &vlanEnd1);
        continue;
      }
      if (vlanEnd1 >= vlanEnd)
      {
        vlanEnd1 = vlanEnd;
      }
      if (vlanStart1 <= vlanStart)
      {
        vlanStart1 = vlanStart;
      }
      if (!first)
      {
        ewsTelnetWrite(ewsContext, ", ");
      }
      first = L7_FALSE;
      if (vlanEnd1 != vlanStart1)
        osapiSnprintf(vlanStr, L7_CLI_MAX_STRING_LENGTH, "%u - %u", vlanStart1, vlanEnd1);
      else
        osapiSnprintf(vlanStr, L7_CLI_MAX_STRING_LENGTH, "%u", vlanStart1);
      /* Limit length of line of output, and avoid overrunning vlanStr. */
      if (bytesWritten > 70)
      {
        cliSyntaxBottom(ewsContext);
        bytesWritten = 0;
      }
      ewsTelnetWrite(ewsContext, vlanStr);
      if (vlanEnd1 >= vlanEnd)
      {
        break;
      }
      bytesWritten += (strlen(vlanStr) + 2);   /* comma and space */
      rc = usmDbDsL2RelayVlanRangeNextGet(&vlanStart1, &vlanEnd1);
    }

    cliSyntaxBottom(ewsContext);
  }

  /* Display circuit-Id specific information.*/
  if (cIdOption == L7_TRUE && (vlanOption == L7_FALSE && rIdOption == L7_FALSE))
  {
    /* List VLANs where DHCP L2 relay is enabled */
    ewsTelnetWriteAddBlanks(1, 1, 0, 1, L7_NULLPTR, ewsContext, 
                            "DHCP Circuit-Id option is enabled on the following VLANs:");
    vlanStart1 = vlanStart;
    rc = usmDbDsL2RelayCircuitIdVlanRangeNextGet(&vlanStart1, &vlanEnd1);
    while ((rc == L7_SUCCESS) && (vlanStart1 != 0))
    {
      if (vlanStart1 > vlanEnd)
      {
        break;
      }
      if (vlanEnd1 != L7_NULL && vlanEnd1 < vlanStart)
      {
        rc = usmDbDsL2RelayCircuitIdVlanRangeNextGet(&vlanStart1, &vlanEnd1);
        continue;
      }
      if (vlanEnd1 >= vlanEnd)
      {
        vlanEnd1 = vlanEnd;
      }
      if (vlanStart1 <= vlanStart)
      {
        vlanStart1 = vlanStart;
      }
      if (!first)
      {
        ewsTelnetWrite(ewsContext, ", ");
      }
      first = L7_FALSE;
      if (vlanEnd1 != vlanStart1)
        osapiSnprintf(vlanStr, L7_CLI_MAX_STRING_LENGTH, "%u - %u", vlanStart1, vlanEnd1);
      else
        osapiSnprintf(vlanStr, L7_CLI_MAX_STRING_LENGTH, "%u", vlanStart1);

      /* Limit length of line of output, and avoid overrunning vlanStr. */
      if (bytesWritten > 70)
      {
        cliSyntaxBottom(ewsContext);
        bytesWritten = 0;
      }
      ewsTelnetWrite(ewsContext, vlanStr);
      if (vlanEnd1 >= vlanEnd)
      {
        break;
      }
      bytesWritten += (strlen(vlanStr) + 2);   /* comma and space */
      rc = usmDbDsL2RelayCircuitIdVlanRangeNextGet(&vlanStart1, &vlanEnd1);
    }

    cliSyntaxBottom(ewsContext);
  }

   /* Display remote-Id specific information.*/
  if (rIdOption == L7_TRUE && (vlanOption == L7_FALSE && cIdOption == L7_FALSE))
  {
    /* List VLANs where DHCP remote-Id is enabled */
    ewsTelnetWriteAddBlanks(1, 1, 0, 1, L7_NULLPTR, ewsContext, 
                            pStrInfo_switching_DhcpL2RelayVlanNumRemoteIdMode);
    ewsTelnetWrite(ewsContext,"---------  -------------");
    if (pagenated != L7_TRUE)
    {
      vlanNext = vlanStart - 1;
    }
    else
    {
      vlanNext = vlanNext - 1;
    }
    rc = usmDbDsL2RelayRemoteIdVlanNextGet(&vlanNext, remoteIdStr);

    while ((rc == L7_SUCCESS) && (vlanNext != 0))
    {
      if (vlanNext > vlanEnd)
      {
        break;
      }
      lineCnt++;
      if (lineCnt >= 21)
      {
        pagenated = L7_TRUE;
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliSyntaxBottom(ewsContext);
        osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
        for (val=1; val<argc; val++)
        {
          OSAPI_STRNCAT(cmdBuf, " ");
          OSAPI_STRNCAT(cmdBuf, argv[val]);
        }
        cliAlternateCommandSet(cmdBuf);
        return pStrInfo_common_Name_2;
      }
      osapiSnprintf(buf, sizeof(buf), "\n%-10d %-25s ",vlanNext, remoteIdStr); 
      ewsTelnetWrite(ewsContext, buf);
      rc = usmDbDsL2RelayRemoteIdVlanNextGet(&vlanNext, remoteIdStr);
    }
    cliSyntaxBottom(ewsContext);
  }

  if ((vlanOption == L7_TRUE && cIdOption == L7_TRUE) ||
      (cIdOption == L7_TRUE && rIdOption == L7_TRUE) ||
      (vlanOption == L7_TRUE && rIdOption == L7_TRUE))
  {
    /* List VLANs where DHCP L2 relay is enabled */
    ewsTelnetWrite(ewsContext, "\n");
    ewsTelnetWriteAddBlanks(1, 1, 0, 1, L7_NULLPTR, ewsContext, 
                            pStrInfo_switching_DhcpL2RelayVlanNumL2RelayCidRIdMode);
    ewsTelnetWrite(ewsContext,"--------- ----------  ----------- ------------");
    if (pagenated != L7_TRUE)
    {
      vlanNext = L7_DOT1Q_MIN_VLAN_ID;
    }
    else
    {
      vlanNext = vlanNext + 1;
    }
    for (; vlanNext <= L7_DOT1Q_MAX_VLAN_ID; vlanNext++)
    { 
      if (usmDbDsL2RelayVlanModeGet(vlanNext, &l2RelayMode) != L7_SUCCESS)
      {
        continue;
      }
      if (usmDbDsL2RelayCircuitIdGet(vlanNext, &cIdMode) != L7_SUCCESS)
      {
        continue;
      }
      if (usmDbDsL2RelayRemoteIdGet(vlanNext, remoteIdStr) != L7_SUCCESS)
      {
        continue;
      }
      else
      {
        if (osapiStrncmp(remoteIdStr, &endOfStr, sizeof(endOfStr)) != L7_NULL)
        {
          rIdMode = L7_ENABLE;
        }
        else
        {
          rIdMode = L7_DISABLE;
          osapiStrncpy(remoteIdStr, "--NULL--", sizeof(remoteIdStr));
        }
      }
      if (l2RelayMode != L7_ENABLE && cIdMode != L7_ENABLE && rIdMode != L7_ENABLE)
      {
        continue;
      }

      {
        osapiSnprintf(buf, sizeof(buf), "\r\n %-10d  %-10s  %-10s %-25s",
                      vlanNext,strUtilEnabledDisabledGet(l2RelayMode, pStrInfo_common_NotApplicable),
                      strUtilEnabledDisabledGet(cIdMode, pStrInfo_common_NotApplicable),
                      remoteIdStr);
        ewsTelnetWrite(ewsContext, buf);
        lineCnt++;
        if (lineCnt >= 21)
        {
          pagenated = L7_TRUE;
          cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
          cliSyntaxBottom(ewsContext);
          osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
          for (val=1; val<argc; val++)
          {
            OSAPI_STRNCAT(cmdBuf, " ");
            OSAPI_STRNCAT(cmdBuf, argv[val]);
          }
          cliAlternateCommandSet(cmdBuf);
          return pStrInfo_common_Name_2;
        }
      }
    }
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}


/*********************************************************************
*
* @purpose Displays the DHCP L2 Relay per-interface statistics.
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes      All options of this show command displays for all interfaces.
*
* @cmdsyntax  show dhcp l2relay  interfaces stats {all | <u/s/p> }
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowDhcpL2RelayIntfStats(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 intIfNum = 0;
  dsL2RelayIntfStats_t intfStats;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 ifName[L7_CLI_MAX_STRING_LENGTH], *showOption;
  L7_uint32 unit, slot, port, givenIntf = 0;
  L7_BOOL intfOption = L7_FALSE;
  L7_uint32 numArg = cliNumFunctionArgsGet(),arg = 1;   

  cliSyntaxTop(ewsContext);

  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_switching_ShowDhcpL2RelayStats);
  }

  showOption = (L7_uchar8 *)(argv[index + arg]);
  /* Do all validations for each of the options in show command.*/
  if (osapiStrncmp(showOption, pStrInfo_base_all_1, sizeof(pStrInfo_base_all_1)) != L7_NULL)
  {
    if (cliIsStackingSupported() != L7_TRUE)
    {
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, showOption, &slot,
                              &port, &givenIntf) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      if (cliValidSpecificUSPCheck(showOption, &unit, &slot, &port) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                              pStrErr_common_InvalidSlotPort_1);
      }
      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &givenIntf) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    intfOption = L7_TRUE;
    intIfNum = givenIntf - 1; /* So this will adjust for the getNext call.*/
  }



  ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                          " Interface  UntrustedServer    UntrustedClient  TrustedServer     TrustedClient");
  ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                          "            MsgsWithOpt82    MsgsWithOpt82    MsgsWithoutOpt82  MsgsWithoutOpt82");
  ewsTelnetWrite(ewsContext,
                 "\r\n-----------  --------------- -----------------  ----------------- -----------------\n");

  while ((usmDbValidIntIfNumNext(intIfNum, &intIfNum) == L7_SUCCESS))
  {
    if (usmDbDsIntfIsValid(intIfNum) != L7_TRUE)
    {
      continue;
    }
    /* get unit slot and port for this interface*/
    if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) != L7_SUCCESS)
    {
      continue;
    }
    osapiSnprintf(ifName, sizeof(ifName), cliDisplayInterfaceHelp(unit, slot, port));

    if (usmDbDsL2RelayIntfStatsGet(intIfNum, &intfStats) == L7_SUCCESS)
    {
     osapiSnprintf(buf, sizeof(buf), " %-11s   %-15u   %-15u   %-15u  %-15u\r\n", 
                   ifName, intfStats.untrustedSrvMsgsWithOpt82, intfStats.untrustedCliMsgsWithOpt82, 
                   intfStats.trustedSrvMsgsWithoutOpt82, intfStats.trustedCliMsgsWithoutOpt82);
     ewsTelnetWrite(ewsContext, buf);
    }
    if ((intfOption == L7_TRUE) && (intIfNum == givenIntf))
    {
      break;  
    }
  }
  return cliSyntaxReturnPrompt(ewsContext, "");
}

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
/*********************************************************************
*
* @purpose Displays the DHCP L2 Relay subscription information.
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes      All options of this show command are included in this function
*
* @cmdsyntax  show dhcp l2relay subscription interfaces {all | <u/s/p> }
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowDhcpL2RelaySubscription(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 intIfNum = 0;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 ifName[L7_CLI_MAX_STRING_LENGTH], *showOption;
  L7_uint32 unit, slot, port, givenIntf = 0;
  L7_BOOL intfOption = L7_FALSE;
  L7_uchar8 endOfStr = FD_DHCP_L2RELAY_REMOTE_ID_VLAN_MODE;
  L7_uint32 numArg = cliNumFunctionArgsGet(),arg = 1;   
  L7_uint32 l2RelayMode = L7_DISABLE, cIdMode = L7_DISABLE;
  L7_uchar8 subscriptionName[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX];
  L7_uchar8 remoteIdStr[DS_MAX_REMOTE_ID_STRING];

  cliSyntaxTop(ewsContext);

  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, pStrErr_switching_ShowDhcpL2RelaySubscription);
  }

  showOption = (L7_uchar8 *)(argv[index + arg]);
  /* Do all validations for each of the options in show command.*/
  if (osapiStrncmp(showOption, pStrInfo_base_all_1, sizeof(pStrInfo_base_all_1)) != L7_NULL)
  {
    if (cliIsStackingSupported() != L7_TRUE)
    {
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, showOption, &slot,
                              &port, &givenIntf) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      if (cliValidSpecificUSPCheck(showOption, &unit, &slot, &port) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                              pStrErr_common_InvalidSlotPort_1);
      }
      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &givenIntf) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    intfOption = L7_TRUE;
    intIfNum = givenIntf; /* So this will adjust for the getNext call.*/
  }



  ewsTelnetWriteAddBlanks(1, 0, 0, 1, L7_NULLPTR, ewsContext,
                          " Interface   SubscriptionName    L2Relay mode  Circuit-Id mode  Remote-Id mode");
  ewsTelnetWrite(ewsContext,
                 "\r\n-----------  ----------------   -------------- "
                 "---------------  ------------------ \n");

  memset(subscriptionName, 0 , L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
  while ((usmdbDsL2RelayIntfSubscriptionNextGet(&intIfNum, subscriptionName) == L7_SUCCESS))
  {
    /* get unit slot and port for this interface*/
    if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) != L7_SUCCESS)
    {
      continue;
    }
    osapiSnprintf(ifName, sizeof(ifName), cliDisplayInterfaceHelp(unit, slot, port));

    if ((intfOption == L7_FALSE) ||
       ((intfOption == L7_TRUE) && (intIfNum == givenIntf)))
    {
      usmdbDsL2RelaySubscripionModeGet(intIfNum, subscriptionName, &l2RelayMode);
      usmdbDsL2RelaySubscripionCircuitIdModeGet(intIfNum, subscriptionName, &cIdMode);
      usmdbDsL2RelaySubscripionRemoteIdModeGet(intIfNum, subscriptionName, remoteIdStr);

      if (osapiStrncmp(remoteIdStr, &endOfStr, sizeof(L7_uchar8)) == L7_NULL) 
      {
        osapiStrncpy(remoteIdStr, "--NULL--", DS_MAX_REMOTE_ID_STRING);
      }

      osapiSnprintf(buf, sizeof(buf), " %-11s   %-15s   %-15s   %-15s  %-15s \r\n", 
                    ifName, subscriptionName, 
                    ((l2RelayMode == L7_ENABLE) ? "Enabled" : "Disabled"),
                    ((cIdMode== L7_ENABLE) ? "Enabled" : "Disabled"),
                    remoteIdStr);
      ewsTelnetWrite(ewsContext, buf);
    }
  }
  return cliSyntaxReturnPrompt(ewsContext, "");
}
#endif
#endif
