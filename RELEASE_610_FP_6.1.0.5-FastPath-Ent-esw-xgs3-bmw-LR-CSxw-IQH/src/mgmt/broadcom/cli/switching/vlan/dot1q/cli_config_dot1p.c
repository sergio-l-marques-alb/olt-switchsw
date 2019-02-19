/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_config_dot1p.c
 *
 * @purpose Dot1p config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  09/16/2003
 *
 * @author  Jason Shaw
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "clicommands_dot1p.h"

#include "clicommands_card.h"

/*********************************************************************
*
* @purpose  Configure 802.1p Priority Mapping
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
* @mode  Interface Config
*
* @cmdsyntax  classofservice dot1p-mapping <0-7> <0-3>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandClassofserviceDot1pmapping(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 argPriority=1;
  L7_uint32 argPriorityQueue=2;
  L7_uint32 unit, slot, port;
  L7_uint32 priority;
  L7_uint32 queue, interface, def_tc;
  L7_int32 maxTrafficClasses;

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
  if(mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
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
    interface = L7_ALL_INTERFACES;      /* use this for all dot1p USMDB calls */
  }

  usmDbDot1dPortNumTrafficClassesGet(unit, interface, &maxTrafficClasses);

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgCosDot1pMapPing,
                              L7_DOT1P_MIN_PRIORITY, L7_DOT1P_MAX_PRIORITY,
                              L7_DOT1P_MIN_TRAFFIC_CLASS, (maxTrafficClasses-1) );
    }

    if (cliCheckIfInteger((L7_char8 *)argv[index+argPriority]) == L7_SUCCESS)
    {
      sscanf(argv[index+argPriority], "%d", &priority);
      if (  (priority < L7_DOT1P_MIN_PRIORITY) || (priority > L7_DOT1P_MAX_PRIORITY)  )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_switching_Dot1pPriOutOfRange);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if (cliCheckIfInteger((L7_char8 *)argv[index+argPriorityQueue]) == L7_SUCCESS)
    {
      sscanf(argv[index+argPriorityQueue], "%d", &queue);
      if (  (queue < L7_DOT1P_MIN_TRAFFIC_CLASS) || (queue > (maxTrafficClasses-1))  )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_switching_TrafficClassOutOfRange);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbDot1dTrafficClassSet(unit, interface, priority, queue);

      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_switching_Set8021pMapping);
      }
    }

  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgCosDot1pMapPingNo);
    }

    for (priority = L7_DOT1P_MIN_PRIORITY; priority <= L7_DOT1P_MAX_PRIORITY; priority++)
    {
      rc = usmDbDot1dDefaultTrafficClassGet(unit, interface, priority, &def_tc);
      if (rc == L7_SUCCESS)
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          /* NOTE:  Called function will check existing value against new value
           *        to cover case where global config change affects some
           *        interfaces but not all.
           */
          rc = usmDbDot1dTrafficClassSet(unit, interface, priority, def_tc);
        }
      }

      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_switching_Set8021pMapping);
      }
    } /* endfor */
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure vlan port priority for an interface
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
* @cmdsyntax  vlan port priority <0-7>
*
* @cmdhelp Configure ingress filtering
*
* @cmddescript
*    none
* @end
*
*********************************************************************/
const L7_char8 *commandVlanPortPriority(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface, slot, port;
  L7_uint32 argPriority=1;
  L7_uint32 unit;
  L7_int32 portPriority;
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

  numArg =  cliNumFunctionArgsGet();

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgVlanPortPri_1, L7_DOT1P_MIN_PRIORITY, L7_DOT1P_MAX_PRIORITY);
  }
  else
  {
    if (cliCheckIfInteger((L7_char8 *)argv[index+argPriority]) == L7_SUCCESS)
    {
      sscanf(argv[index+argPriority], "%d", &portPriority);
      if (  (portPriority < L7_DOT1P_MIN_PRIORITY) || (portPriority > L7_DOT1P_MAX_PRIORITY)  )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_switching_PortPriOutOfRange);
      }
      else                                                                  /* apply to single interface */
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          rc = usmDbDot1dPortDefaultUserPrioritySet(unit, interface, portPriority);
        }
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        /* port selected cannot perform this function */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_switching_AnInvalidSlotPortHasBeenUsedForFunc);

      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_switching_CfgurePortPri);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure vlan port priority for an interface
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
* @cmdsyntax  vlan port priority all <0-7>
*
* @cmdhelp Configure ingress filtering
*
* @cmddescript
*    none
* @end
*
*********************************************************************/
const L7_char8 *commandVlanPortPriorityAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 argPriority=1;
  L7_uint32 unit;
  L7_int32 portPriority;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  unit = cliGetUnitId();

  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_CfgVlanPortPriAll, L7_DOT1P_MIN_PRIORITY, L7_DOT1P_MAX_PRIORITY);
  }
  else
  {
    if (cliCheckIfInteger((L7_char8 *)argv[index+argPriority]) == L7_SUCCESS)
    {
      sscanf(argv[index+argPriority], "%d", &portPriority);
      if (  (portPriority < L7_DOT1P_MIN_PRIORITY) || (portPriority > L7_DOT1P_MAX_PRIORITY)  )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_switching_PortPriOutOfRange);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_PortPriSpecified);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbDot1dPortDefaultUserPrioritySet(unit, L7_ALL_INTERFACES, portPriority);

      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_switching_CfgurePortPri);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
