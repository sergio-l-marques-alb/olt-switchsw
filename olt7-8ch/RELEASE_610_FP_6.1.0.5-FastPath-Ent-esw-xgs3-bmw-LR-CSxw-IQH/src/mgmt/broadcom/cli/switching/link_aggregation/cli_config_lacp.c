/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config_lacp.c
 *
 * @purpose LACP config commands for CLI
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  10/10/2005
 *
 * @author  Sunil Babu
 * @end
 *
 **********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>
#include "clicommands_card.h"
#include "clicommands_lacp.h"
#include "defaultconfig.h"
#include "commdefs.h"
#include "usmdb_dot3ad_api.h"

/*********************************************************************
*
* @purpose  To set LACP collector max delay
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lacp collector max-delay <delay>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandLACPCollectorMaxDelay(EwsContext ewsContext,
                                             L7_uint32 argc,
                                             const L7_char8 * * argv,
                                             L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uint32 itype;
  L7_uint32 maxDelay;

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_LAG_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_base_LagsNotValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgCollectorMaxDelay_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[index+1], &maxDelay) == L7_SUCCESS)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adAggCollectorMaxDelaySet(unit, iface, maxDelay) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgCollectorMaxDelay);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgCollectorMaxDelay_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgCollectorMaxDelayNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set the default value */
      if (usmDbDot3adAggCollectorMaxDelaySet(unit, iface, FD_DOT3AD_DEFAULT_LACP_MAXDELAY) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgCollectorMaxDelay);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgCollectorMaxDelay_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}
/*********************************************************************
*
* @purpose  To set LACP actor system priority
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lacp actor system priority <sysPrio>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandLACPActorSysPriority(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 * * argv,
                                            L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uint32 itype;
  L7_uint32 priorityValue;

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_PHYSICAL_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgActorSysPri_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[index+1], &priorityValue) == L7_SUCCESS)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adAggPortActorSystemPrioritySet(unit, iface, priorityValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgActorSysPri);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgActorSysPri_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgActorSysPriNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set the default value */
      if (usmDbDot3adAggPortActorSystemPrioritySet(unit, iface, FD_DOT3AD_ACTOR_DEFAULT_SYS_PRIORITY) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgActorSysPri);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgActorSysPri_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  To set LACP actor admin key
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lacp actor admin key <key>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandLACPActorAdminKey(EwsContext ewsContext,
                                         L7_uint32 argc,
                                         const L7_char8 * * argv,
                                         L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uint32 itype;
  L7_uint32 keyValue;

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_PHYSICAL_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgActorAdminKey_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[index+1], &keyValue) == L7_SUCCESS)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adAggPortActorAdminKeySet(unit, iface, keyValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgActorAdminKey);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgActorAdminKey_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgActorAdminKeyNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set the default value */

      if (usmDbDot3adAggPortActorAdminKeySet(unit, iface, iface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgActorAdminKey);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgActorAdminKey_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  To set LACP actor port priority
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lacp actor port priority <portPrio>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandLACPActorPortPriority(EwsContext ewsContext,
                                             L7_uint32 argc,
                                             const L7_char8 * * argv,
                                             L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uint32 itype;
  L7_uint32 priorityValue;

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_PHYSICAL_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgActorPortPri_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[index+1], &priorityValue) == L7_SUCCESS)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adAggPortActorPortPrioritySet(unit, iface, priorityValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgActorPortPri);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgActorPortPri_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgActorPortPriNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set the default value */
      /*FD_DOT3AD_DEFAULT_PORT_PRIORITY  ??? */
      if (usmDbDot3adAggPortActorPortPrioritySet(unit, iface, FD_DOT3AD_ACTOR_DEFAULT_PORT_PRIORITY) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgActorPortPri);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgActorPortPri_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  To set LACP partner system priority
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lacp partner system priority <sysPrio>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandLACPPartnerSysPriority(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 * * argv,
                                              L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uint32 itype;
  L7_uint32 priorityValue;

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_PHYSICAL_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerSysPri_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[index+1], &priorityValue) == L7_SUCCESS)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adAggPortPartnerAdminSystemPrioritySet(unit, iface, priorityValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgPartnerSysPri);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerSysPri_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerSysPriNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set the default value */
      if (usmDbDot3adAggPortPartnerAdminSystemPrioritySet(unit, iface, FD_DOT3AD_PARTNER_DEFAULT_PORT_PRIORITY) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgPartnerSysPri);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgPartnerSysPri_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  To set LACP partner admin key
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lacp partner admin key <key>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandLACPPartnerAdminKey(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uint32 itype;
  L7_uint32 keyValue;

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_PHYSICAL_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerAdminKey_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[index+1], &keyValue) == L7_SUCCESS)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adAggPortPartnerAdminKeySet(unit, iface, keyValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgPartnerAdminKey);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerAdminKey_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerAdminKeyNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set the default value */
      if (usmDbDot3adAggPortPartnerAdminKeySet(unit, iface, FD_DOT3AD_PARTNER_DEFAULT_ADMINKEY) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgPartnerAdminKey);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgPartnerAdminKey_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  To set LACP partner system ID
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lacp partner system id <sysID>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandLACPPartnerSysID(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uint32 itype;

  L7_uchar8 mac_address[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argMac = 1;

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_PHYSICAL_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerSysId_1);
    }

    /* Read the string argumment and validate it */

    osapiStrncpySafe(mac_address, argv[index+argMac], L7_CLI_MAX_STRING_LENGTH);

    memset(mac, 0, L7_CLI_MAX_STRING_LENGTH);
    if (cliConvertMac(mac_address, mac) != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLacpMac);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDot3adAggPortPartnerAdminSystemIDSet(unit, iface, mac) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgPartnerSysId);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerSysId_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerSysIdNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set the default value   */

      osapiStrncpySafe(mac_address, pStrInfo_common_000000000000, L7_CLI_MAX_STRING_LENGTH);

      memset(mac, 0, L7_CLI_MAX_STRING_LENGTH);
      cliConvertMac(mac_address, mac);

      if (usmDbDot3adAggPortPartnerAdminSystemIDSet(unit, iface, mac) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgPartnerSysId);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgPartnerSysId_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  To set LACP partner port id
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lacp partne port id <portID>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandLACPPartnerPortID(EwsContext ewsContext,
                                         L7_uint32 argc,
                                         const L7_char8 * * argv,
                                         L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uint32 itype;
  L7_uint32 portID;

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_PHYSICAL_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerPortId_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[index+1], &portID) == L7_SUCCESS)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adAggPortPartnerAdminPortSet(unit, iface, portID) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgPartnerPortId);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerPortId_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerPortIdNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set the default value */
      if (usmDbDot3adAggPortPartnerAdminPortSet(unit, iface, FD_DOT3AD_PARTNER_DEFAULT_PORTID) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgPartnerPortId);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgPartnerPortId_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  To set LACP partner port priority
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lacp partner port priority <portPrio>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandLACPPartnerPortPriority(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 * * argv,
                                               L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uint32 itype;
  L7_uint32 portPriority;

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_PHYSICAL_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerPortPri_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[index+1], &portPriority) == L7_SUCCESS)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adAggPortPartnerAdminPortPrioritySet(unit, iface, portPriority) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgPartnerPortPri);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerPortPri_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgPartnerPortPriNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* Set the default value */
      if (usmDbDot3adAggPortPartnerAdminPortPrioritySet(unit, iface, FD_DOT3AD_PARTNER_DEFAULT_PORT_PRIORITY) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgPartnerPortPri);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgPartnerPortPri_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set LACP admin key parameters
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax [no] lacp admin key <key>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandLACPAdminKey(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 * * argv,
                                    L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uint32 itype;
  L7_uint32 keyValue;

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_LAG_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_base_LagsNotValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLacpAdminKey_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[index+1], &keyValue) == L7_SUCCESS)
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot3adAggActorAdminKeySet(unit, iface, keyValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgLacpAdminKey);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLacpAdminKey_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLacpAdminKeyNo);
    }

    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbDot3adAggActorAdminKeySet(unit, iface, iface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgLacpAdminKey);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgLacpAdminKey_1);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set LACP actor admin state parameters
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax [no] lacp actor admin state (passive|individual| longtimeout|
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandLACPActorAdminState(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index)
{
  L7_uint32 s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uchar8 adminStateTemp;
  L7_uchar8 adminState;

  L7_uint32 iface;
  L7_uint32 itype;
  L7_RC_t rc;

  L7_uchar8 strOption[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if(numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLacpActorAdminState_1);
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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_PHYSICAL_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
  }

  adminStateTemp = 0;
  adminState = 0;

  rc = usmDbDot3adAggPortActorAdminStateGet(unit, iface, &adminStateTemp);

  osapiStrncpySafe(strOption,argv[index+ 1], sizeof(strOption));
  cliConvertToLowerCase(strOption);

  if ((strcmp(strOption, pStrInfo_base_LacpIn_1) == 0) )
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      adminState = (L7_uchar8)DOT3AD_STATE_AGGREGATION;
      adminState = (L7_uchar8)(adminStateTemp &
                               (~((L7_uchar8)DOT3AD_STATE_AGGREGATION)));
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      adminState = (L7_uchar8) (adminStateTemp | DOT3AD_STATE_AGGREGATION);
    }
  }
  else if((strcmp(strOption, pStrInfo_base_Pass_3) == 0) )
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      adminState = (L7_uchar8)DOT3AD_STATE_LACP_ACTIVITY;
      adminState = (L7_uchar8)(adminStateTemp &
                               (~((L7_uchar8)DOT3AD_STATE_LACP_ACTIVITY)));
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      adminState = (L7_uchar8) (adminStateTemp | DOT3AD_STATE_LACP_ACTIVITY);
    }
  }
  else if((strcmp(strOption, pStrInfo_base_LacpLongTimeoutString) == 0) )
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      adminState = (L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT;
      adminState = (L7_uchar8)(adminStateTemp &
                               (~((L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT)));
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      adminState = (L7_uchar8) (adminStateTemp | DOT3AD_STATE_LACP_TIMEOUT);
    }
  }
  else
  {
    adminState = (L7_uchar8)(FD_DOT3AD_ACTOR_ADMIN_PORT_STATE);
  }

  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDot3adAggPortActorAdminStateSet(unit, iface, &adminState) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgLacpActorAdminState);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set LACP partner state parameters
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lacp partner admin state (passive|individual| longtimeout|
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandLACPPartnerAdminState(EwsContext ewsContext,
                                             L7_uint32 argc,
                                             const L7_char8 * * argv,
                                             L7_uint32 index)
{
  L7_uint32 s, p;
  L7_uint32 unit;
  L7_uint32 numArg = 0;
  L7_uchar8 adminStateTemp;
  L7_uchar8 adminState;

  L7_uint32 iface;
  L7_uint32 itype;
  L7_RC_t rc;

  L7_uchar8 strOption[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if(numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLacpPartnerAdminState_1);
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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( usmDbIntfTypeGet(iface, &itype) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if (itype != L7_PHYSICAL_INTF)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
  }

  adminStateTemp = 0;
  adminState = 0;
  rc = usmDbDot3adAggPortPartnerAdminStateGet(unit, iface, &adminStateTemp);

  osapiStrncpySafe(strOption,argv[index+ 1], sizeof(strOption));
  cliConvertToLowerCase(strOption);

  if ((strcmp(strOption, pStrInfo_base_LacpIn_1) == 0) )
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      adminState = (L7_uchar8)DOT3AD_STATE_AGGREGATION;
      adminState = (L7_uchar8)(adminStateTemp &
                               (~((L7_uchar8)DOT3AD_STATE_AGGREGATION)));
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      adminState = (L7_uchar8) (adminStateTemp | DOT3AD_STATE_AGGREGATION);
    }
  }
  else if((strcmp(strOption, pStrInfo_base_Pass_3) == 0) )
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      adminState = (L7_uchar8)DOT3AD_STATE_LACP_ACTIVITY;
      adminState = (L7_uchar8)(adminStateTemp &
                               (~((L7_uchar8)DOT3AD_STATE_LACP_ACTIVITY)));
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      adminState = (L7_uchar8) (adminStateTemp | DOT3AD_STATE_LACP_ACTIVITY);
    }
  }
  else if((strcmp(strOption, pStrInfo_base_LacpLongTimeoutString) == 0) )
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      adminState = (L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT;
      adminState = (L7_uchar8)(adminStateTemp &
                               (~((L7_uchar8)DOT3AD_STATE_LACP_TIMEOUT)));
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      adminState = (L7_uchar8) (adminStateTemp | DOT3AD_STATE_LACP_TIMEOUT);
    }
  }
  else
  {
    adminState = (L7_uchar8)(FD_DOT3AD_PARTNER_ADMIN_PORT_STATE);
  }

  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbDot3adAggPortPartnerAdminStateSet(unit, iface, &adminState) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgLacpPartnerAdminState);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}
