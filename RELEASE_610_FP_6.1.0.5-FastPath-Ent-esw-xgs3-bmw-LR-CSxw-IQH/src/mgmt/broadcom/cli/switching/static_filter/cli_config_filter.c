/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config_filter.c
 *
 * @purpose static filtering config commands for CLI
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  4/20/2008
 *
 * @author  akulkarn
 * @end
 *
 **********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>
#include "clicommands_card.h"
#include "clicommands_filter.h"
#include "defaultconfig.h"
#include "commdefs.h"
#include "usmdb_filter_api.h"
#include "filter_exports.h"

/*********************************************************************
 *
 * @purpose  create a static MAC filter
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
 * @cmdsyntax  macfilter <macaddr> <vlan>
 *
 * @cmdhelp Create a Static MAC Filter.
 *
 * @cmddescript
 *   This command adds a static MAC filter entry fo the MAC address
 *   <macaddr> on the VLAN <vlan>.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMacFilter(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 argMacAddr = 1;
  L7_uint32 argVlanId = 2;
  L7_uint32 vlanID = 0;
  L7_uint32 count = 0;
  L7_uchar8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_BOOL   entryMulticast;

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
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgMacFilterCreate);
    }

    if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgMacFilterCreate);
    }

    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr );
      return cliPrompt(ewsContext);
    }
    if (cliIsRestrictedFilterMac(strMacAddr) == L7_TRUE)
    {
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_MacAddr_1);
      return cliPrompt(ewsContext);
    }

    entryMulticast = L7_FALSE;
    entryMulticast = usmDbFilterCheckMacAddrTypeMulticast(strMacAddr);
    if (usmdbFilterCountGet(unit, &count) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_MacAddr_1);
    }
    else
    {
      if((entryMulticast==L7_FALSE)&& (count >= L7_MAX_FDB_STATIC_FILTER_ENTRIES))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_MaxMacFilters);
      }

      if((entryMulticast==L7_TRUE)&& (count >= L7_MAX_GROUP_REGISTRATION_ENTRIES))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_MaxMacFilters);
      }

    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbVlanIDGet(unit, vlanID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbFilterCreate(unit, strMacAddr, vlanID);
      if (rc == L7_ALREADY_CONFIGURED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterExists);
        cliSyntaxBottom(ewsContext);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_MacFilterNotCreated);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgMacFilterRemove);
    }
    if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgMacFilterRemove);
    }

    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, pStrErr_common_UserInput,  ewsContext,  pStrErr_common_UsrInputInvalidClientMacAddr);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],&vlanID)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    if (usmDbFilterIsConfigured(unit, strMacAddr, vlanID) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbFilterRemove(unit, strMacAddr, vlanID) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_MacFilterNotRemoved);
      }
    }

  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Adds a destination port to a static MAC filter
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
 * @cmdsyntax  macfilter adddest <macaddr> <vlanid>
 *
 * @cmdhelp Adds a destination port to a Static MAC Filter.
 *
 * @cmddescript
 *   This command adds a destination port to a static MAC filter entry for the MAC address
 *   <macaddr> on the VLAN <vlan>.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMacFilterAddDest(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMacAddr = 1;
  L7_uint32 argVlanId = 2;
  L7_uint32 vlanID = 0;
  L7_uint32 slot = 0;
  L7_uint32 port = 0;
  L7_uint32 interface = 0;
  L7_uchar8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  /*L7_uchar8 allPortsMask[FILTER_INTF_INDICES];  */
  L7_uint32 portChannelIf;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Fetching the port informtion from ewsContext */
  slot = EWSSLOT(ewsContext);
  port =EWSPORT(ewsContext);

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgMacFilterAddDest_1);
    }

    if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgMacFilterAddDest_1);
    }
    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext,pStrErr_common_UsrInputInvalidClientMacAddr  );
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbVlanIDGet(unit, vlanID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
    }

    if (usmDbFilterIsConfigured(unit, strMacAddr, vlanID) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
    }

    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");

    }
    else if (usmDbDot3adIntfIsMemberGet(unit, interface, &portChannelIf) == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_base_AddPort);
      ewsTelnetPrintf (ewsContext, " %s %s", cliDisplayInterfaceHelp(unit, slot, port),
          pStrInfo_base_SlotPortWithoutUnitIsPartOfALag);
      cliSyntaxBottom(ewsContext);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmdbFilterDstIntfAdd(unit, strMacAddr, vlanID, interface);
        if(rc == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext,pStrErr_common_UsrInputInvalidMcastMacAddr);
        }        

        if (rc== L7_NOT_IMPLEMENTED_YET)
        {
          /* This feature not supported on this platform */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_CfguringDstPortsWithUcastMacAddrNotSupportedOnPlatform);

        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
        else if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PortNotAdded);
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgMacFilterDelDest);
    }

    /* Validating the parameters from the command line */
    if ((strlen(argv[index+argMacAddr]) >= sizeof(buf)) ||
        (strlen(argv[index+argVlanId]) >= sizeof(buf)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgMacFilterDelDest);
    }

    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr );
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbVlanIDGet(unit, vlanID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
    }

    if (usmDbFilterIsConfigured(unit, strMacAddr, vlanID) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
    }

    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");

    }
    else
    {

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmdbFilterDstIntfDelete(unit, strMacAddr, vlanID, interface);
        if(rc == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext,pStrErr_common_UsrInputInvalidMcastMacAddr);
        }

        if (rc== L7_NOT_IMPLEMENTED_YET)
        {
          /* This feature not supported on this platform */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_CfguringDstPortsWithUcastMacAddrNotSupportedOnPlatform);

        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
        else if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PortNotDeld);
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
 * @purpose  Adds a destination port to a static MAC filter
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
 * @cmdsyntax  macfilter adddest all  <macaddr> <vlanid>
 *
 * @cmdhelp Adds a destination port to a Static MAC Filter.
 *
 * @cmddescript
 *   This command adds a destination port to a static MAC filter entry for the MAC address
 *   <macaddr> on the VLAN <vlan>.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMacFilterAddDestAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMacAddr = 1;
  L7_uint32 argVlanId = 2;
  L7_uint32 vlanID = 0;
  L7_uint32 interface = 0;
  L7_uchar8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 allPortsMask[FILTER_INTF_INDICES];
  L7_uint32 portChannelIf, nextInterface;
  L7_RC_t rc=L7_FAILURE;
  L7_uint32 error = 0;
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

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (cliNumFunctionArgsGet() != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgMacFilterAddDestAll);
    }
    if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgMacFilterAddDestAll);
    }

    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext,pStrErr_common_UsrInputInvalidClientMacAddr  );
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbVlanIDGet(unit, vlanID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
    }

    if (usmDbFilterIsConfigured(unit, strMacAddr, vlanID) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
    }

    /* Applies to all interfaces */
    while (usmDbGetNextPhysicalIntIfNumber(interface, &nextInterface) == L7_SUCCESS)
    {
      interface = nextInterface;
      if (usmDbDot3adIntfIsMemberGet(unit, interface, &portChannelIf) == L7_SUCCESS)
      {
        continue;
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmdbFilterDstIntfAdd(unit, strMacAddr, vlanID, interface);
        if ( rc == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext,pStrErr_common_UsrInputInvalidMcastMacAddr);
        }
        else if (rc==L7_NOT_IMPLEMENTED_YET)
        {
          error=L7_NOT_IMPLEMENTED_YET;
        }
        else if (rc==L7_NOT_SUPPORTED)
        {
          error=L7_NOT_SUPPORTED;
        }
        else if (rc == L7_ALREADY_CONFIGURED)
        {                                        /*if the port already exists then no need for an error*/
          continue;
        }
      }
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (error != 0)
      {
        if (error == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_CantAddPort);
        }
        else if (error==L7_NOT_IMPLEMENTED_YET)
        {
          /* This feature not supported on this platform */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_CfguringDstPortsWithUcastMacAddrNotSupportedOnPlatform);
        }
        else if (error==L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgMacFilterDelDest);
    }

    /* Validaing parameters from the command line */
    if ((strlen(argv[index+argMacAddr]) >= sizeof(buf)) ||
        (strlen(argv[index+argVlanId]) >= sizeof(buf)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgMacFilterDelDest);
    }
    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext,pStrErr_common_UsrInputInvalidClientMacAddr  );
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID)!=L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbVlanIDGet(unit, vlanID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
    }

    if (usmDbFilterIsConfigured(unit, strMacAddr, vlanID) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
    }
    memset (allPortsMask, 0, sizeof(allPortsMask));
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc= usmDbFilterDstIntfMaskSet(unit, strMacAddr, vlanID, allPortsMask);
    }
    if(rc == L7_FAILURE)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext,pStrErr_common_UsrInputInvalidMcastMacAddr);
    } 

    if (rc==L7_NOT_SUPPORTED)
    {
      /* This feature not supported on this platform */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_CfguringDstPortsWithUcastMacAddrNotSupportedOnPlatform);
    }
    else
    {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);

    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Adds a source port to a static MAC filter
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
 * @cmdsyntax  macfilter addsrc <macaddr> <vlan>
 *
 * @cmdhelp Adds a source port to a Static MAC Filter.
 *
 * @cmddescript
 *   This command adds a source port to a static MAC filter entry for the MAC address
 *   <macaddr> on the VLAN <vlan>.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMacFilterAddSrc(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMacAddr = 1;
  L7_uint32 argVlanId = 2;
  L7_uint32 vlanID = 0;
  L7_uint32 slot = 0;
  L7_uint32 port = 0;
  L7_uint32 interface = 0;
  L7_uchar8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  /* L7_uchar8 allPortsMask[FILTER_INTF_INDICES]; */
  L7_uint32 portChannelIf;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Fetching the port information from ewsContext */
  slot = EWSSLOT(ewsContext);
  port =EWSPORT(ewsContext);

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgMacFilterAddSrc_1);
    }

    /* Validating parameters from the command line */
    if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgMacFilterAddSrc_1);
    }

    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext,pStrErr_common_UsrInputInvalidClientMacAddr  );
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbVlanIDGet(unit, vlanID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
    }
    if (usmDbFilterIsConfigured(unit, strMacAddr, vlanID) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
    }
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else if (usmDbDot3adIntfIsMemberGet(unit, interface, &portChannelIf) == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_base_AddPort);
      return cliSyntaxReturnPrompt (ewsContext," %s %s", cliDisplayInterfaceHelp(unit, slot, port),
          pStrInfo_base_SlotPortWithoutUnitIsPartOfALag);
    }
    else
    {

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmdbFilterSrcIntfAdd(unit, strMacAddr, vlanID, interface);
        if (rc == L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
        else if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SrcPortNotAdded);
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgMacFilterDelSrc);
    }

    if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgMacFilterDelSrc);
    }

    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr );
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbVlanIDGet(unit, vlanID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
    }

    if (usmDbFilterIsConfigured(unit, strMacAddr, vlanID) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
    }
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else
    {

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmdbFilterSrcIntfDelete(unit, strMacAddr, vlanID, interface);
        if (rc == L7_NOT_SUPPORTED)
        {
          /* port selected cannot perform this function */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
        else if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SrcPortNotDeld);
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
 * @purpose  Adds a source port to a static MAC filter
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
 * @cmdsyntax  macfilter addsrc all <macaddr> <vlan>
 *
 * @cmdhelp Adds a source port to a Static MAC Filter.
 *
 * @cmddescript
 *   This command adds a source port to a static MAC filter entry for the MAC address
 *   <macaddr> on the VLAN <vlan>.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandMacFilterAddSrcAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMacAddr = 1;
  L7_uint32 argVlanId = 2;
  L7_uint32 vlanID = 0;
  L7_uint32 interface = 0;
  L7_uchar8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 allPortsMask[FILTER_INTF_INDICES];
  L7_uint32 portChannelIf, nextInterface;
  L7_BOOL error = L7_FALSE;
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

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (cliNumFunctionArgsGet() != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgMacFilterAddSrcAll);
    }

    if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgMacFilterAddSrcAll);
    }

    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr );
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbVlanIDGet(unit, vlanID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
    }

    if (usmDbFilterIsConfigured(unit, strMacAddr, vlanID) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
    }

    while (usmDbGetNextPhysicalIntIfNumber(interface, &nextInterface) == L7_SUCCESS)
    {
      interface = nextInterface;
      if (usmDbDot3adIntfIsMemberGet(unit, interface, &portChannelIf) == L7_SUCCESS)
      {
        continue;
      }

      if (usmdbFilterSrcIntfAdd(unit, strMacAddr, vlanID, interface) != L7_SUCCESS)
      {
        error = L7_TRUE;
      }
    }
    if (error == L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_PortNotAddedToMask);
    }
    cliSyntaxBottom(ewsContext);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgMacFilterDelSrcAll);
    }

    if (strlen(argv[index+argMacAddr]) >= sizeof(buf))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_base_CfgMacFilterDelSrcAll);
    }
    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset ( strMacAddr, 0, sizeof(strMacAddr) );
    if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
    {
      return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext,pStrErr_common_UsrInputInvalidClientMacAddr );
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argVlanId],  &vlanID) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (usmDbVlanIDGet(unit, vlanID) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
    }

    if (usmDbFilterIsConfigured(unit, strMacAddr, vlanID) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_MacFilterNotExists);
    }

    OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddr]);

    memset (allPortsMask, 0, sizeof(allPortsMask));
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbFilterSrcIntfMaskSet(unit, strMacAddr, vlanID, allPortsMask) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}



