/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/ip_mcast/cli_config_dvmrp.c
 *
 * @purpose config commands for the dvmrp cli functionality
 *
 * @component user interface
 *
 * @comments
 *
 * @create
 *
 * @author srikrishnas
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_ip_mcast_common.h"
#include "strlib_ip_mcast_cli.h"
#include <errno.h>
#include "l3_mcast_commdefs.h"
#include "l3_mcast_defaultconfig.h"

#include "l7_common.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_dvmrp_api.h"
#include "usmdb_mib_mcast_api.h"
#endif


#include <osapi.h>
#include <nimapi.h>
#include <clicommands_l3.h>
#include <clicommands_mcast.h>
#include <usmdb_mib_dvmrp_api.h>
#include <l3_commdefs.h>
#include "clicommands_card.h"
#include <cli_config_script.h>

/*********************************************************************
*
* @purpose  Sets the administrative mode of DVMRP to active or inactive
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip dvmrp
*
* @cmdhelp
*
* @cmddescript  enable = TRUE: DVMRP is active
*               disable = FALSE: DVMRP is inactive on all interfaces;
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandIpDvmrp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 mode, argMode = 1;
  L7_uint32 unit, numArgs, protocol;
  L7_uint32 intMode = FD_DVMRP_DEFAULT_ADMIN_MODE;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 protoStat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc = L7_FAILURE;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  if((ewsContext->commType == CLI_NORMAL_CMD) && (( numArgs != 0 ) && (numArgs != 1)))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpDvmrp);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpDvmrpNo);
  }

  else if((ewsContext->commType == CLI_NO_CMD) && (numArgs == 0))
  {
    intMode = FD_DVMRP_DEFAULT_ADMIN_MODE;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDvmrpAdminModeSet(unit, intMode) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_ipmcast_DsblDvmrpAdminMode);
      }
    }
  }
  else if((ewsContext->commType == CLI_NORMAL_CMD) && (numArgs == 0))
  {
    intMode = L7_ENABLE;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc =  usmDbDvmrpAdminModeSet(unit, intMode);
      if (rc != L7_SUCCESS )
      {
        if (rc == L7_ERROR)
        {
          usmDbMcastIpProtocolGet(unit, L7_AF_INET, &protocol);
          cliMcastProtocolStrPrint(protocol, stat, sizeof(stat), "%s");
          osapiSnprintf(protoStat,sizeof(protoStat),pStrInfo_ipmcast_McastProtoCfgured, stat); 
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, protoStat);
        }
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_ipmcast_SetDvmrpAdminModeOtherMcastProtoMightBeEnbld);
      }
    }
  }
  else if((ewsContext->commType == CLI_NORMAL_CMD) && (numArgs == 1))
  {
    OSAPI_STRNCPY_SAFE(strMode,argv[index+ argMode]);
    cliConvertToLowerCase(strMode);
    if (strcmp(strMode, pStrInfo_common_Lvl7Clr) == 0)
    {
      /*  lvl7clear is a method of clearing all dvmrp routes
       *  this is temporary thats why no mapping layer is provided for
       *  the test team.
       */
      if ((usmDbDvmrpAdminModeGet(unit, &mode) == L7_SUCCESS ) &&
          (mode == L7_ENABLE))
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_Successful);
        cliSyntaxBottom(ewsContext);

      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_ipmcast_UnSuccessfulAdminModeNotSet);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the interface mode of DVMRP to enable or disable
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip dvmrp
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandIpDvmrpUnicastRouting(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs, s, p;
  L7_uint32 intMode = FD_DVMRP_INTF_DEFAULT_ADMIN_MODE;
  L7_uint32 iface;
  L7_RC_t rc;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpDvmrp);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpDvmrpNo);
  }

  if(ewsContext->commType == CLI_NO_CMD)
  {
    intMode = FD_DVMRP_INTF_DEFAULT_ADMIN_MODE;
  }
  else if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode = L7_ENABLE;
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        continue;
      }
      if ( (rc = usmDbDvmrpIntfAdminModeSet(unit, iface, intMode)) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_ipmcast_SetIntfMode_1);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the dvmrp metric for the specified interface
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip dvmrp metric <metric>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDvmrpMetric(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMetric = 1;
  L7_uint32 unit, numArgs, s, p;
  L7_char8 strMetric[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 metric = FD_DVMRP_INTF_DEFAULT_METRIC, iface;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpDvmrpMetric);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpDvmrpMetricNo);
  }

  if(ewsContext->commType == CLI_NO_CMD)
  {
    metric = FD_DVMRP_INTF_DEFAULT_METRIC;
  }
  else if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    OSAPI_STRNCPY_SAFE(strMetric,argv[index+ argMetric]);
    if ( cliCheckIfInteger(strMetric) == L7_SUCCESS)
    {
      (void)cliConvertTo32BitUnsignedInteger(strMetric, &metric);
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrInfo_ipmcast_EnterAValidDvmrpMetricValMetricMustBeBetweenAnd,L7_DVMRP_INTERFACE_METRIC_MIN, L7_DVMRP_INTERFACE_METRIC_MAX);
    }

  }

  if (( metric >= L7_DVMRP_INTERFACE_METRIC_MIN ) && ( metric <= L7_DVMRP_INTERFACE_METRIC_MAX ))
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          continue;
        }
        if (usmDbDvmrpIntfMetricSet(unit, iface, metric) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_ipmcast_SetIntfMetric);
        }
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_ipmcast_DvmrpMetricValMetricMustBeBetweenAnd,L7_DVMRP_INTERFACE_METRIC_MIN, L7_DVMRP_INTERFACE_METRIC_MAX);
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Enable/Disable DVMRP trap mode.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  ip dvmrp trapflags
*
* @cmdhelp
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpDvmrpTrapflags(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArgs;
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();

  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpDvmrpTrapFlags);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpDvmrpTrapFlagsNo);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDvmrpTrapModeSet(unit, L7_ENABLE, L7_DVMRP_TRAP_ALL) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_ipmcast_PimDmTrapModeFailure);
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbDvmrpTrapModeSet(unit, L7_DISABLE, L7_DVMRP_TRAP_ALL) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_ipmcast_PimDmTrapModeFailure);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
