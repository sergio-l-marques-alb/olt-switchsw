/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/switching/cli_config_protectedport.c
*
* @purpose  protected port config commands for the cli
*
* @component user interface
*
* @comments none
*
* @create  06/23/2005
*
* @author  ryadagiri
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
#include "dot1q_api.h"
#include "usmdb_protected_port_api.h"
#include "cli_web_exports.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  Configure an name for a protected group
*           'no' form - places the default name to the group
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes not well tested
*
* @cmdsyntax  switchport protected <groupid> name <name>
*
* @cmdhelp
*
* @cmddescript
*   Configure an name for protected group
*
* @end
*
*********************************************************************/
const L7_char8 * commandSwitchPortProtectedGroupName(EwsContext ewsContext, L7_uint32 argc,
                                                     const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 name[L7_PROTECTED_PORT_GROUP_NAME_SIZE];
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 indexName2 = 3;
  L7_uint32 argGroupId = 1;
  L7_uint32 groupId = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliSyntaxTop (ewsContext);

  unit = cliGetUnitId ();
  
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,
										                       ewsContext, pStrErr_common_UnitId_1);
  }
 	     /* verify if the specified string contains all the alpha-numeric characters */
#if 0
 
 if (cliIsAlphaNum(name) != L7_SUCCESS)
  {
    
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ProtectedPortAlphanum);
    
  }
#endif
  numArg = cliNumFunctionArgsGet ();
  

  if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
                         L7_PROTECTED_PORT_MAX_GROUPS) == L7_TRUE )
  {
    if (ewsContext->commType ==  CLI_NORMAL_CMD)
    {
      if(numArg != 3)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
												                 ewsContext, pStrErr_switching_ProtectedGrpName);
      }
      OSAPI_STRNCPY_SAFE(strGroupId, argv[index + argGroupId]);
      if (cliCheckIfInteger (strGroupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
												                       pStrErr_common_InvalidInteger);
      }
      groupId = atoi (strGroupId);
      memset(name, 0x00, sizeof(name));
      if(numArg == 3) /* if name is Given */
      {
         if (strlen(argv[index+indexName2]) > (L7_PROTECTED_PORT_GROUP_NAME_SIZE - 1))
        {
          return cliSyntaxReturnPrompt (ewsContext, pStrInfo_switching_NameMustBeCharsOrLess,
                 (L7_PROTECTED_PORT_GROUP_NAME_SIZE - 1));
        }
        OSAPI_STRNCPY_SAFE(name, argv[index+indexName2]);
      }
    }
    else if (ewsContext->commType ==  CLI_NO_CMD)
    {
      if(numArg != 2)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
							 ewsContext, pStrErr_switching_ProtectedGrpNameNo);
      }
      OSAPI_STRNCPY_SAFE(strGroupId, argv[index + argGroupId]);
      if (cliCheckIfInteger (strGroupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,
												   ewsContext, pStrErr_common_InvalidInteger);
      }
      groupId = atoi (strGroupId);
      OSAPI_STRNCPY_SAFE(name, FD_PROTECTED_PORT_GROUP_NAME);
    }
  }
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
  

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* Set the name */
      rc = usmdbProtectedPortGroupNameSet(unit,groupId,name);
	  

	  
      if (rc != L7_SUCCESS)
      {
        /* Failed to change the mode */
        return cliSyntaxReturnPromptAddBlanks (1, 2, 0, 0, L7_NULLPTR, ewsContext,
                                 pStrErr_switching_FailedProtectedPortNameChg);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* Set the name */
      rc = usmdbProtectedPortGroupNameSet(unit,groupId,name);
      if (rc != L7_SUCCESS)
      {
        /* Failed to change the mode */
        return cliSyntaxReturnPromptAddBlanks (1, 2, 0, 0, L7_NULLPTR, ewsContext,
                                 pStrErr_switching_FailedProtectedPortNameChg);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt (ewsContext);
}
/*********************************************************************
*
* @purpose  Configure an interface as protected.
*           'no' form - Remove the interface from protected state
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes not well tested
*
* @cmdsyntax  switchport protected  <groupid>
*
* @cmdhelp Configure an interface as protected.
*
* @cmddescript
*   Configure an interface as protected / not protected.
*
* @end
*
*********************************************************************/
const L7_char8 *
commandSwitchPortProtected(EwsContext ewsContext, L7_uint32 argc,
                           const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit, interface;
  L7_uint32 numArg;
  L7_RC_t rc;
  L7_uint32 argGroupId = 1;
  L7_uint32 groupId = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliSyntaxTop (ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported () == L7_TRUE)
  {
      unit = EWSUNIT (ewsContext);
  }
  else
  {
    unit = cliGetUnitId ();
  }

  if (unit == 0)
  {
     if (usmDbIntIfNumFromUSPGet(unit, EWSSLOT (ewsContext),
                 EWSPORT (ewsContext), &interface) != L7_SUCCESS)
     {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, EWSSLOT (ewsContext),
                                       EWSPORT (ewsContext));
     }
     if(usmDbDot3adIsConfigured(unit, interface) == L7_TRUE)
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
     }
  }

  numArg = cliNumFunctionArgsGet ();

  if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
                                 L7_PROTECTED_PORT_MAX_GROUPS) == L7_TRUE )
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ProtectedPort_1);
    }
    else if (ewsContext->commType ==  CLI_NORMAL_CMD)
    {
      OSAPI_STRNCPY_SAFE(strGroupId, argv[index + argGroupId]);
      if (cliCheckIfInteger (strGroupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
      }
      groupId = atoi (strGroupId);
    }
    else if (ewsContext->commType ==  CLI_NO_CMD)
    {
      OSAPI_STRNCPY_SAFE(strGroupId, argv[index + argGroupId]);
      if (cliCheckIfInteger (strGroupId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
      }
      groupId = atoi (strGroupId);
    }
  }
  else
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ProtectedPortNoArgs);
    }
    groupId = 0;
  }

  if (usmDbIntIfNumFromUSPGet(unit, EWSSLOT (ewsContext),
                   EWSPORT (ewsContext), &interface) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, EWSSLOT (ewsContext),
                                       EWSPORT (ewsContext));
  }

  if ((groupId < 0) || (groupId >= L7_PROTECTED_PORT_MAX_GROUPS))
  {
    return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_CfgureValIsOutOfRange);
  }

  if(usmdbProtectedPortIntfIsValid(unit,interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_switching_InvalidProtectedPort);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* Set Port mode to Protected */
      rc = usmdbProtectedPortGroupIntfAdd(unit, groupId, interface );
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_NOT_SUPPORTED)
        {
          /* Failed to change the mode */
          return cliSyntaxReturnPromptAddBlanks (1, 2, 0, 0, L7_NULLPTR, ewsContext,
                                 pStrErr_switching_ProtectedPortModeUnsupported);
        }
        else
        {
          /* Failed to change the mode */
          return cliSyntaxReturnPromptAddBlanks (1, 2, 0, 0, L7_NULLPTR, ewsContext,
                                 pStrErr_switching_FailedProtectedPortModeChg);
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /*PORT UNPROTECTED */
      rc = usmdbProtectedPortGroupIntfDelete(unit, groupId, interface);
      if (rc != L7_SUCCESS)
      {
        /* Failed to change the mode */
        return cliSyntaxReturnPromptAddBlanks (1, 2, 0, 0, L7_NULLPTR, ewsContext,
                                 pStrErr_switching_FailedProtectedPortModeChg);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt (ewsContext);
}
