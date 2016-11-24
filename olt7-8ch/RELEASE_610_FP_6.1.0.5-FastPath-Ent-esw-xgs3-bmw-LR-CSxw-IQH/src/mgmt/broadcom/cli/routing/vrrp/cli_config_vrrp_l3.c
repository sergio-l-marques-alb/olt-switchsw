/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/cli_config_vrrp_l3.c
 *
 * @purpose cli functions that implements the vrrp config functionality
 *
 * @component user interface
 *
 * @comments
 *
 * @create  18/11/2001
 *
 * @author  Samip Garg
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include <string.h>

#include "cliapi.h"
#include "datatypes.h"
#include "l7_vrrp_api.h"
#include "clicommands.h"
#include "ews.h"
#include "clicommands_l3.h"
#include "usmdb_mib_vrrp_api.h"
#include "cli_web_exports.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"
#include "clicommands_card.h"

/*********************************************************************
*
* @purpose  sets the  admin mode for vrrp
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
* @cmdsyntax ip vrrp / no ip vrrp
* @cmdhelp Set admin mode.
*
* @cmddescript
*   This command sets the admin mode to enable/disable. This command
*   will run in a global config mode
*
* @end
*
*********************************************************************/
const L7_char8 *commandIPVrrp(EwsContext ewsContext,
                              L7_uint32 argc,
                              const L7_char8 * * argv,
                              L7_uint32 index)
{
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  if ( cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpVrrp);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      usmDbVrrpOperAdminStateSet(unit, L7_TRUE);
    }
  }
  else
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      usmDbVrrpOperAdminStateSet(unit,L7_FALSE);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Add the virtual router configuration.
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
* @cmdsyntax ip vrrp <1-255>
*
* @cmdhelp Create virtual router.
*
* @cmddescript
*   This command create a virtual router configuration.This command
*   will run in interface config mode.
*
* @end
*
*********************************************************************/
const L7_char8 *commandVrrpInterfaceRouterid(EwsContext ewsContext,
                                             L7_uint32 argc,
                                             const L7_char8 * * argv,
                                             L7_uint32 index)
{
  L7_uchar8 routerID;
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 state, numArg;
  L7_uint32 argvrid = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpAdd);
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

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface)!= L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if (cliConvertRouterID(ewsContext,argv[index+argvrid],&routerID) != L7_SUCCESS)
  {
    cliSyntaxBottom(ewsContext);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InVal);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbVrrpRouterIdCreate (unit, routerID, iface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_CfgVrrpAddError);
      }
    }
  }
  else
  {
    if(usmDbVrrpOperVrrpModeGet(unit, routerID, iface, &state) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_routing_CfgVrrpRemoveCfgError);
    }
    else if (state == L7_VRRP_UP)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_routing_CfgVrrpRemoveError);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if(usmDbVrrpRouterIdDelete(unit, routerID, iface) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_routing_CfgVrrpRemoveCfgError);
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
* @purpose  sets the IP address for vrrp
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
* @cmdsyntax ip vrrp <vrID> ip <ipaddr> [secondary]
*
* @cmdhelp Set ipaddress.
*
* @cmddescript
*   The set the virtual router ip address.
*
* @end
*
*********************************************************************/

const L7_char8 *commandVrrpVrIDIp(EwsContext ewsContext,
                                  L7_uint32 argc,
                                  const L7_char8 * * argv,L7_uint32 index)
{
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 vrID;
  L7_uint32 intfcNum, s, p, ipAddr, primaryIpAddress;
  L7_uint32 intf_ip_address,mask;
  L7_uchar8 count;
  L7_uint32 unit;
  L7_uint32 argvrid = 2;
  L7_uint32 argIpaddr = 4;
  L7_uint32 argSecondary = 5;
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD && (argc < 5 || argc > 6))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpIpAddr);
  }
  else if (ewsContext->commType == CLI_NO_CMD && argc != 6)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpIpAddrNo);
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

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intfcNum) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( cliValidateRtrIntf(ewsContext, intfcNum) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if (cliConvertRouterID(ewsContext,argv[argvrid],&vrID)
      != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InVal);
  }
  if (strlen(argv[argIpaddr]) >= sizeof(strIpAddr))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_CfgVrrpIpAddrError);
  }
  OSAPI_STRNCPY_SAFE(strIpAddr, argv[argIpaddr]);
  if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS || ipAddr== 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_CfgVrrpIpAddrError);
  }

  if (((ewsContext->commType == CLI_NORMAL_CMD) && 
        (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT))  &&
      (usmDbVrrpOperIpAddrCountGet(unit, vrID, intfcNum, &count) != L7_SUCCESS ||
       count == L7_L3_NUM_IP_ADDRS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgFailMax);
  }

  usmDbIpRtrIntfIpAddressGet(unit, intfcNum, &intf_ip_address,&mask);

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {

      if (argc == 6 && strcmp (argv[argSecondary], pStrInfo_routing_Secondary_1) <= 0)
      {
        /* IP Address */
        if (usmDbVrrpIpAddressNextGet(unit, vrID, intfcNum, 0, &primaryIpAddress)!= L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_VrrpSecondaryCfgFail);
        }
        if (primaryIpAddress == ipAddr)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpSecondaryCfgFailDup);
        }
        else if (usmDbVrrpAssocSecondaryIpAddress(unit, vrID, intfcNum, ipAddr) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_VrrpSecondaryCfgFail);
        }
      }
      else
      {
        rc = usmDbVrrpAssocIpAddress(unit, vrID, intfcNum, ipAddr);
        if (rc == L7_ALREADY_CONFIGURED)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpSecondaryCfgFailDup);
        }
        else if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_VrrpPrimaryCfgFail);
        }
      }
   }    /* Normal */
   else     /* No */
   {

     if (strcmp (argv[argSecondary], pStrInfo_routing_Secondary_1) <= 0)
     {
        /* IP Address */
       if (usmDbVrrpDissocSecondaryIpAddress(unit, vrID, intfcNum, ipAddr) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_VrrpSecondaryCfgFailNo);
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
* @purpose  sets the priority level for vrrp
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
* @cmdsyntax ip vrrp <vrID> priority <priority>
*
* @cmdhelp Set priority.
*
* @cmddescript
*   The set the virtual router priority.
*
* @end
*
*********************************************************************/

const L7_char8 *commandVrrpVrIDPriority(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,L7_uint32 index)
{
  L7_char8 strPriority[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 vrID;
  L7_uchar8 oldPriority = 0;
  L7_uint32 priority = 0;
  L7_uint32 intfcNum, s, p;
  L7_uint32 tmpInd =0;
  L7_uint32 argvrid =2;
  L7_uint32 argpriority =4;
  L7_uint32 unit =0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if( (argc!=5 && ewsContext->commType == CLI_NORMAL_CMD) || (argc!=4 && ewsContext->commType == CLI_NO_CMD) )
  {
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpPri);
    }
    if( ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgNoVrrpPri);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  if (cliConvertRouterID(ewsContext,argv[tmpInd+argvrid],&vrID)
      != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InVal);
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

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intfcNum) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( cliValidateRtrIntf(ewsContext, intfcNum) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if( ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (strlen(argv[tmpInd+argpriority]) >= sizeof(strPriority))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpPri);
    }

    OSAPI_STRNCPY_SAFE(strPriority, argv[tmpInd+argpriority]);

    if ( cliConvertTo32BitUnsignedInteger(strPriority, &priority) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpPri);
    }
  }
  else
  {
    priority = FD_VRRP_DEFAULT_PRIORITY;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbVrrpOperPriorityGet(unit, vrID, intfcNum, &oldPriority) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
    }
    else if (oldPriority == L7_VRRP_INTF_PRIORITY_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpIpCfgMatch);
    }
  }
  if ( ( priority > L7_VRRP_INTF_PRIORITY_MIN ) && ( priority < L7_VRRP_INTF_PRIORITY_MAX ) )
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbVrrpOperPrioritySet(unit, vrID, intfcNum, priority)
          != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpPriError);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  sets the preempt mode for vrrp
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
* @cmdsyntax ip vrrp <1-255> preempt/ no ip vrrp <1-255> preempt
*
* @cmdhelp Set preempt mode
*
* @cmddescript
*   This command set the preempt mode.
*
* @end
*
*********************************************************************/
const L7_char8 *commandVrrpVrIDPreempt(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_RC_t rc;
  L7_uchar8 vrID;
  L7_uint32 intfcNum, s, p;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (argc!= 4)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpPre);
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

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intfcNum) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( cliValidateRtrIntf(ewsContext, intfcNum) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if( cliConvertRouterID(ewsContext,argv[2],&vrID)
     != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InVal);
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      rc = usmDbVrrpOperPreemptModeSet(unit, vrID, intfcNum, L7_ENABLE);
    }
    else
    {
      rc = usmDbVrrpOperPreemptModeSet(unit, vrID, intfcNum, L7_DISABLE);
    }

    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  set the Advertisement Interval for vrrp
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
* @cmdsyntax ip vrrp timers advertise <time in seconds>
*
* @cmdhelp Set advert interval.
*
* @cmddescript
*   This set the advertisement interval of a virtual router configuration.
*
* @end
*
*********************************************************************/
const L7_char8 *commandVrrpVrIDTimersAdvertise(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 * * argv,
                                               L7_uint32 index)
{
  L7_uint32 addInterval;
  L7_uchar8 vrID;
  L7_uint32 intfcNum, s, p;
  L7_uint32 unit;
  L7_uint32 argvrid = 2;
  L7_uint32 argsec = 5;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if( (argc!=6 && ewsContext->commType == CLI_NORMAL_CMD) || (argc!=5 && ewsContext->commType == CLI_NO_CMD) )
  {
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpAdvint);
    }
    if( ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgNoVrrpAdvint);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
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
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intfcNum) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( cliValidateRtrIntf(ewsContext, intfcNum) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if( cliConvertRouterID(ewsContext,argv[argvrid],&vrID) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InVal);
  }
  if( ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[argsec], &addInterval) == L7_SUCCESS )
    {
      if ( ( addInterval > L7_VRRP_INTF_PRIORITY_MIN ) && ( addInterval <= L7_VRRP_INTF_PRIORITY_MAX ) )
      {

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbVrrpOperAdvertisementIntervalSet(unit, vrID, intfcNum,
                                                    addInterval) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_routing_CfgVrrpAdvintError);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidInteger);
    }
  }
  else
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbVrrpOperAdvertisementIntervalSet(unit, vrID, intfcNum,
                                                FD_VRRP_DEFAULT_ADVERTISEMENT_INTERVAL) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose Track the interface for VRRP 
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
 * @cmdsyntax ip vrrp <vrid> track interface <u/s/p> [decrement <1-254>]
 *            no ip vrrp <vrid> track interface <u/s/p> [decrement]
 *
 * @cmdhelp This will add or delete a particular ip interfaces into the
            VRRP tracking interfaces list.
 *
 * @cmddescript
 *   This will causes the VRRP router priority to decrease based on the tracking 
     IP interfaces are Up/Down.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandVrrpVrIDTrackInterface(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 decPriority = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;
  L7_uchar8 vrID, tempPrio;
  L7_uint32 intfcNum, s, p,trackIntf, numArg;
  L7_uint32 unit,trackIntfUnit;
  L7_uint32 argvrid = 2;
  L7_uint32 argSlotPort=5;
  L7_uint32 argDecPriority = 7;
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg != 1) && (numArg != 3))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpTrackIntf);
    }
  }
  else
  {
    if( ewsContext->commType == CLI_NO_CMD)
    {
      if (numArg < 1 || numArg > 2)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpTrackIntfNo);
      }
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
    if (cliValidSpecificUSPCheck(argv[argSlotPort], &trackIntfUnit, &s, &p) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(trackIntfUnit, s, p, &trackIntf) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

  }
  else
  {
    unit = cliGetUnitId();
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    if (cliSlotPortToIntNum(ewsContext, argv[argSlotPort], &s, &p, &trackIntf) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intfcNum) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( cliValidateRtrIntf(ewsContext, intfcNum) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if( cliConvertRouterID(ewsContext,argv[argvrid],&vrID)
      != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InVal);
  }
  if (usmDbVrrpOperTrackIntfPrioGet(unit, vrID, intfcNum,trackIntf, &tempPrio)== L7_SUCCESS)
  {
     decPriority = tempPrio;
  }
  if (numArg > 1)
  {
    if (strcmp (argv[index+2], pStrInfo_common_Decrement ) != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpTrackIntf);   
    }

  }
  if (numArg == 3)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[argDecPriority], &decPriority) == L7_SUCCESS )
    {
      if ( ( decPriority < L7_VRRP_INTF_PRIORITY_MIN ) || ( decPriority > L7_VRRP_INTF_PRIORITY_MAX ) )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_routing_VrrpPriError);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidInteger);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( ewsContext->commType == CLI_NORMAL_CMD)
    {
      rc = usmDbVrrpOperTrackIntfAdd(unit, vrID, intfcNum,trackIntf,decPriority);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgTrackIntfTblFull);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
        }
      }

    }
    else
    {
      if ( ewsContext->commType == CLI_NO_CMD)
      {
        if (numArg == 1)
        {
          rc = usmDbVrrpOperTrackIntfDelete(unit, vrID, intfcNum,trackIntf);
          if (rc != L7_SUCCESS)
          {
            if (rc != L7_NOT_EXIST)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgTrackIntfNotFound);
            }
            else
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
            }
          }

        }

        if (numArg == 2)
        {
          rc = usmDbVrrpVrIdIntfTrackIntfGet (unit, vrID, intfcNum,trackIntf);
          if (rc == L7_SUCCESS)
          {
            if (usmDbVrrpOperTrackIntfAdd(unit, vrID, intfcNum,trackIntf,
                  FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
            }
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgTrackIntfNotFound);
          }
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
 * @purpose Track ip route for VRRP
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
 * @cmdsyntax ip vrrp <vrid> track ip route <u/s/p> [decrement <1-254>]
 *            no ip vrrp <vrid> track ip route <u/s/p> [decrement]
 *
 * @cmdhelp track ip route will decrement the vrrp router 
            priority based on route reachability.
 *
 q
 * @cmddescript
 *   This will add or delete a particular route into the VRRP track ip route list.
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandVrrpVrIDTrackIpRoute(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 * * argv,
    L7_uint32 index)
{
  L7_uint32 decPriority = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;
  L7_uchar8 vrID, tempPrio;
  L7_uint32 intfcNum, s, p;
  L7_uint32 unit,numArg;
  L7_uint32 argvrid = 2;
  L7_uint32 argRoute=6;
  L7_uint32 argDecPriority =8;
  L7_uint32 ipAddr=0;
  L7_uchar8 prefix[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 mask = 0;
  L7_uchar8 strIpRoute[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  memset(strIpRoute, 0, sizeof(strIpRoute));

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();


  if( ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg != 1) && (numArg != 3))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpTrackIPRoute);
    }
  }
  else
  {
    if( ewsContext->commType == CLI_NO_CMD)
    {
      if ((numArg < 1) || (numArg > 2))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpTrackIPRouteNo );
      }
    }
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

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intfcNum) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }
  if ( cliValidateRtrIntf(ewsContext, intfcNum) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if( cliConvertRouterID(ewsContext,argv[argvrid],&vrID)
      != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InVal);
  }
   
  if ( cliValidPrefixPrefixLenCheckIPV4(argv[argRoute], prefix , &mask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InvalidRouteIp);
  }
  if (usmDbInetAton(prefix, &ipAddr) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InvalidRouteIp);
  }
  
  if (usmDbVrrpOperTrackRoutePrioGet(unit, vrID, intfcNum, ipAddr, mask, &tempPrio)== L7_SUCCESS)
  {
     decPriority = tempPrio;
  }
  if (numArg > 1)
  {
    if (strcmp (argv[index+2], pStrInfo_common_Decrement ) != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpTrackIPRoute );
    }

  }
  if (numArg == 3)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[argDecPriority], &decPriority) == L7_SUCCESS )
    {
      if ( ( decPriority < L7_VRRP_INTF_PRIORITY_MIN ) || ( decPriority > L7_VRRP_INTF_PRIORITY_MAX ) )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_routing_VrrpPriError);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidInteger);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( ewsContext->commType == CLI_NORMAL_CMD)
    {
      rc = usmDbVrrpOperTrackRouteAdd(unit, vrID, intfcNum, ipAddr, mask, decPriority);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_InvalidVrrpTrackRoute);
        }
        else if (rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgTrackRoutTblFull);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
        }
      }

    }
    else
    {
      if ( ewsContext->commType == CLI_NO_CMD)
      {
        if (numArg == 1)
        {
          rc = usmDbVrrpOperTrackRouteDelete(unit, vrID, intfcNum, ipAddr, mask);
          if (rc  != L7_SUCCESS)
          {
            if (rc == L7_NOT_EXIST)
            {
              cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgTrackRoutNotFound);
            }
            else
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
            }
          }

        }

        if (numArg == 2)
        {
          if (usmDbVrrpVrIdIntfTrackRouteGet(unit, vrID, intfcNum, ipAddr, mask ) == L7_SUCCESS)
          {
            (void)usmDbVrrpOperTrackRouteAdd(unit, vrID, intfcNum, ipAddr, mask,
                                             FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT);
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgTrackRoutNotFound);
          }
        }
      } /* end if no command */
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;


   return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  sets the Auth Type & data for a virtual router
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
* @cmdsyntax ip vrrp <1-255> authentication [ <key> ]/no vrrp <vrID> authentication [ <key> ]
*
* @cmdhelp To set auth type and data
*
* @cmddescript
*   This set the Authentication Type and auth data for
*   a virtual router
*
* @end
*
*********************************************************************/
const L7_char8 *commandVrrpVrIDAuthentication(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 * * argv,
                                              L7_uint32 index)

{
  L7_AUTH_TYPES_t authType;
  L7_uchar8 vrID;
  L7_uint32 intfcNum, s, p;
  L7_char8 authKey[VRRP_AUTH_DATA_LEN +1];
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 tmpInd = 0;
  L7_uint32 argvrid = 2;
  L7_uint32 argAuthTyp = 4;
  L7_uint32 argKey = 5;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if( ((argc<5 || argc >6) && ewsContext->commType == CLI_NORMAL_CMD) || (argc!=4 && ewsContext->commType == CLI_NO_CMD) )
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpAuthType);
    }
    if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpNoAuthType);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
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
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intfcNum) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( cliValidateRtrIntf(ewsContext, intfcNum) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if( cliConvertRouterID(ewsContext,argv[tmpInd+argvrid],&vrID)
     != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InVal);
  }

  if(ewsContext->commType == CLI_NO_CMD)
  {
    authType = L7_AUTH_TYPE_NONE;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbVrrpOperAuthTypeSet(unit, vrID, intfcNum, authType)
          != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
      }
    }

  }
  else
  {
    if (strlen(argv[tmpInd+argAuthTyp]) >= sizeof(strMode))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpAuthType);
    }
    OSAPI_STRNCPY_SAFE(strMode,argv[tmpInd+argAuthTyp]);
    cliConvertToLowerCase(strMode);

    if (strcmp(strMode, pStrInfo_common_None_3) == 0)
    {
      if(argc!=5)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpAuthType);
      }
      authType = L7_AUTH_TYPE_NONE;

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbVrrpOperAuthTypeSet(unit, vrID, intfcNum, authType)
            != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
        }
      }
    }

    else if (strcmp(strMode, pStrInfo_common_Simple_2) == 0)
    {
      if(argc!=6)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpAuthType);
      }
      authType = L7_AUTH_TYPE_SIMPLE_PASSWORD;

      if(strlen( argv[tmpInd+argKey] ) > VRRP_AUTH_DATA_LEN)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpAuthDataError);
      }
/*      if (strlen(argv[tmpInd+argAuthTyp]) >= sizeof(authKey))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpAuthDataError);
      }*/
      OSAPI_STRNCPY_SAFE(authKey, argv[tmpInd+argKey]);

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbVrrpOperAuthTypeSet(unit, vrID, intfcNum, authType)
            != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
        }
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      usmDbVrrpOperAuthKeySet (unit, vrID, intfcNum, authKey);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set tbe vrrp status up/down
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
* @cmdsyntax ip vrrp <1-255> mode /no ip vrrp <1-255> mode
*
* @cmdhelp Set virtual router status up or down
*
* @cmddescript
*   This command set the status up/down.
*
* @end
*
*********************************************************************/

const L7_char8 *commandIPVrrpID (EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 * * argv,
                                 L7_uint32 index)
{

  L7_RC_t rc;
  L7_uchar8 vrID;
  L7_uint32 intfcNum, s, p;
  L7_uint32 unit;
  L7_uint32 argvrid = 2;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgVrrpStart);
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
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intfcNum) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( cliValidateRtrIntf(ewsContext, intfcNum) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if( cliConvertRouterID(ewsContext,argv[argvrid],&vrID)
     != L7_SUCCESS)
  {
    cliSyntaxBottom(ewsContext);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InVal);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      rc = usmDbVrrpOperVrrpModeSet(unit, vrID, intfcNum, L7_VRRP_UP);
    }
    else
    {
      rc = usmDbVrrpOperVrrpModeSet(unit, vrID, intfcNum, L7_VRRP_DOWN);
    }

    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_VrrpCfgNotFound);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}
