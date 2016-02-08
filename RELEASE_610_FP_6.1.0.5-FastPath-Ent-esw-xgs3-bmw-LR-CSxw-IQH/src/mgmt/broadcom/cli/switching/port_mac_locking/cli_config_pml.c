/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_config_pml.c
 *
 * @purpose Port Mac Locking (port-security) commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  6/3/2004
 *
 * @author  kmans
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include <datatypes.h>                 /* for converting from IP to integer */
#include "ews.h"
#include "clicommands.h"
#include "clicommands_card.h"
#include "usmdb_pml_api.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  Enables/Disables port MAC Locking (Port Security)
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
* @cmdsyntax  port-security
*             no port-security
*
* @cmdhelp Configure port locking at the system (Global Config Mode) or port level (Interface Config Mode).
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandPortSecurity(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_SUCCESS;

  L7_uint32 slot, port;
  L7_uint32 interface = 0;
  L7_uint32 unit;
  L7_uint32 numArg;

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

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);
    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    interface = L7_ALL_INTERFACES;
  }
  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments */
  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_CfgPortSecurity);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_CfgPortSecurityNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    if (interface == L7_ALL_INTERFACES)
    {
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbPmlAdminModeSet(unit, L7_ENABLE);
      }
    }

    /* Called from Interface Config Mode */
    else
    {
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbPmlIntfModeSet(unit, interface, L7_ENABLE);
      }
    }
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_switching_CantEnblPortSecurity);
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {

    if (interface == L7_ALL_INTERFACES)
    {
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbPmlAdminModeSet(unit, L7_DISABLE);
      }
    }
    else
    {
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbPmlIntfModeSet(unit, interface, L7_DISABLE);
      }
    }

    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_switching_CantDsblPortSecurity);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  Sets port Port MAC Locking maximum dynamic limit for the specified interface
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
* @cmdsyntax  port-security max-dynamic <limit>
*             no port-security max-dynamic
*
* @cmdhelp Configure port locking maximum dynamic limit at port level (Interface Config Mode).
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandPortSecurityMaxDynamic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 slot, port;
  L7_uint32 interface = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 argmaxDynamic=1;
  L7_uint32 maxDynamic;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* determine if function called from interface config or global config */
  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments */
  if (numArg != 1 && ewsContext->commType == CLI_NORMAL_CMD)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgPortSecurityIfDyn, L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES);
  }
  else if (numArg != 0 && ewsContext->commType == CLI_NO_CMD)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_CfgPortSecurityIfDynNo);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (cliConvertTo32BitUnsignedInteger(argv[index+ argmaxDynamic],&maxDynamic) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidInteger);
    }
    if ((maxDynamic > L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES) || (maxDynamic < 0))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_CfgPortSecurityIfDynLimitRangeErr);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    maxDynamic = L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgPortSecurityIfDyn, L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES);
  }

/*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbPmlIntfDynamicLimitSet(unit, interface, maxDynamic);
    if (rc != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_switching_CfgPortSecurityIfDynLimitErr);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Sets port Port MAC Locking maximum static limit for the specified interface
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
* @cmdsyntax  port-security max-static <limit>
*             no port-security max-static
*
* @cmdhelp Configure port locking maximum static limit at port level (Interface Config Mode).
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandPortSecurityMaxStatic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 slot, port;
  L7_uint32 interface = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 argmaxStatic=1;
  L7_uint32 maxStatic;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments */
  if (numArg != 1 && ewsContext->commType == CLI_NORMAL_CMD)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgPortSecurityIfStatic, L7_MACLOCKING_MAX_STATIC_ADDRESSES);
  }
  else if (numArg != 0 && ewsContext->commType == CLI_NO_CMD)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_CfgPortSecurityIfStaticNo);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (cliConvertTo32BitUnsignedInteger(argv[index+ argmaxStatic],&maxStatic) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidInteger);
    }
    if ((maxStatic > L7_MACLOCKING_MAX_STATIC_ADDRESSES) || (maxStatic < 0))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_CfgPortSecurityIfStaticLimitRangeErr);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    maxStatic = L7_MACLOCKING_MAX_STATIC_ADDRESSES;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgPortSecurityIfStatic, L7_MACLOCKING_MAX_STATIC_ADDRESSES);
  }

/*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbPmlIntfStaticLimitSet(unit, interface, maxStatic);
    if (rc != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_switching_CfgPortSecurityIfStaticLimitErr);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Adds / deletes a statically locked MAC address for an interface
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
* @cmdsyntax  port-security mac-address <mac address> <vlan id>
*             no port-security mac-address <mac address> <vlan id>
*
* @end
*
*********************************************************************/

const L7_char8 *commandPortSecurityMacAddress(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 argMacAddr = 1;
  L7_uint32 argVlanId = 2;
  L7_uchar8 strMacAddr[L7_ENET_MAC_ADDR_LEN];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 errMsg[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 slot, port;
  L7_uint32 interface = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_enetMacAddr_t macAddr;
  L7_uint32 vlanID = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
  }
  else
  {

    return cliPrompt(ewsContext);
  }

  osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, errMsg, sizeof(errMsg), pStrErr_switching_CfgPortSecurityIfMacAdd, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID );

  numArg = cliNumFunctionArgsGet();

  if (numArg != 2)
  {
    return cliSyntaxReturnPrompt (ewsContext, errMsg);
  }
  else
  {
    if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
    {
      return cliSyntaxReturnPrompt (ewsContext, errMsg);
    }
    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      cliSyntaxTop(ewsContext);
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, pStrErr_common_UserInput,  ewsContext,pStrErr_common_UsrInputInvalidClientMacAddr);
      return cliPrompt(ewsContext);
    }
    if (cliIsRestrictedFilterMac(strMacAddr) == L7_TRUE)
    {
      cliSyntaxTop(ewsContext);
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_MacAddr_1);
      return cliPrompt(ewsContext);
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, errMsg);
    }
    else
    {
      if( (vlanID < L7_DOT1Q_MIN_VLAN_ID) || (vlanID > L7_DOT1Q_MAX_VLAN_ID) )
      {
        return cliSyntaxReturnPrompt (ewsContext, errMsg);
      }

      memcpy(&macAddr.addr, &strMacAddr, L7_ENET_MAC_ADDR_LEN);

      rc = cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID);

      if(rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, errMsg);
      }
      /* For normal command */
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbPmlIntfStaticEntryAdd(unit, interface, (L7_enetMacAddr_t)macAddr, (L7_ushort16)vlanID);
          if (rc != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_switching_CfgPortSecurityIfMacAddAddErr);
          }      /* end if Execution Flag */
        }
      }
      /* 'no' command */
      else
      {
        if (ewsContext->commType == CLI_NO_CMD)
        {
          rc = usmDbPmlIntfStaticEntryDelete(unit, interface, (L7_enetMacAddr_t)macAddr, (L7_ushort16) vlanID);
          if (rc != L7_SUCCESS)
          {
            cliSyntaxTop(ewsContext);
            osapiSnprintfAddBlanks (0, 1, 0, 1, pStrErr_common_UserInput, errMsg, sizeof(errMsg), pStrErr_switching_UsrInputInvalidMacVlanCombination);
            ewsTelnetWrite( ewsContext, errMsg);
            return cliPrompt(ewsContext);
          }
        }
      }      /* end if else regular/no command */
    }    /* end if else integer check */
  }  /* end if else numarg() check */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  Moves a dynamically locked entry to a statically locked entry
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
* @cmdsyntax  port-security mac-address move <vlan id> <mac address>
*
* @end
*
*********************************************************************/

const L7_char8 *commandPortSecurityMacAddressMove(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 slot, port;
  L7_uint32 interface = 0;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 staticLimit;
  L7_enetMacAddr_t macAddr;
  L7_ushort16 vlanId;
  L7_uint32 count = 0;
  L7_uchar8 errMsg[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    return cliPrompt(ewsContext);
  }

  numArg = cliNumFunctionArgsGet();
  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_CfgPortSecurityIfMacAddMov);
    }
    else
    if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_CfgPortSecurityIfMacAddMov);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbPmlIntfStaticLimitGet(unit, interface, &staticLimit);
      if (staticLimit == 0)
      {
        osapiSnprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrErr_switching_CfgPortSecurityIfMacAddMovStaticLimitReachedErr, staticLimit);
        ewsTelnetWrite( ewsContext, errMsg);
      }
      else
      {
        rc = usmDbPmlIntfStaticEntryGetFirst(unit, interface,&macAddr, &vlanId);
        while (rc == L7_SUCCESS)
        {
          rc = usmDbPmlIntfStaticEntryGetNext(unit, interface, &macAddr, &vlanId);
          count++;
        }
        if( count >= staticLimit)
        {
          osapiSnprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrErr_switching_CfgPortSecurityIfMacAddMovStaticLimitReachedErr, staticLimit);
          ewsTelnetWrite( ewsContext, errMsg);
        }
        else
        {
          rc = usmDbPmlIntfDynamicToStaticMove(unit, interface);
          if (rc != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 1, 0, pStrErr_common_Error,  ewsContext, pStrErr_switching_CfgPortSecurityIfMacAddMoveErr);
          }
        }
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
* @purpose  Enables/disables snmp violation traps for port mac locking
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
* @cmdsyntax  snmp-server enable traps violation
*
* @end
*
*********************************************************************/
const L7_char8 *commandSnmpServerEnableTrapsViolationMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 slot, port;
  L7_uint32 interface = 0;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);
    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments */
  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_CfgPortSecurity);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_CfgPortSecurityNo);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbPmlIntfViolationTrapModeSet(unit, interface, L7_ENABLE);
    }
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_switching_CantEnblPortSecurity);
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbPmlIntfViolationTrapModeSet(unit, interface, L7_DISABLE);
    }
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_switching_CantDsblPortSecurity);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
