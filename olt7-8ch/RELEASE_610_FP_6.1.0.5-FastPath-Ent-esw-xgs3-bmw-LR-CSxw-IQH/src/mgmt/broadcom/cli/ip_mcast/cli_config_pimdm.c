/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/ip_mcast/cli_config_pimdm.c
 *
 * @purpose config commands for the pimdm cli functionality
 *
 * @component user interface
 *
 * @comments
 *
 * @create
 *
 * @author  srikrishnas
 * @end
 *
 **********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_ip_mcast_common.h"
#include "strlib_ip_mcast_cli.h"
#include <errno.h>
#include "l3_mcast_commdefs.h"
#include "l3_mcast_defaultconfig.h"
#include "cliapi.h"

#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_mcast_api.h"
#include "usmdb_mib_pim_api.h"
#endif


#include <osapi.h>
#include <nimapi.h>
#include <usmdb_ip_api.h>
#include <clicommands_l3.h>
#include <clicommands_mcast.h>
#include <usmdb_mib_pim_api.h>
#include <l3_commdefs.h>
#include "clicommands_card.h"
#include <cli_config_script.h>

/*********************************************************************
*
* @purpose    Configures the PIM-DM administrative mode of
* the router to enable or disable.
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
* @cmdsyntax  ip pimdm
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimdm(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 intMode = L7_DISABLE;
  L7_uint32 unit, numArgs, protocol;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 protoStat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc = L7_FAILURE;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimDmNo);
  }
  else if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimDm);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  if(ewsContext->commType == CLI_NO_CMD)
  {
    intMode=FD_PIMDM_DEFAULT_ADMIN_MODE;
  }
  else if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode=L7_ENABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc =usmDbPimRouterAdminModeSet(unit, L7_AF_INET, intMode);
    if (rc != L7_SUCCESS)
    {
      if ((intMode == L7_ENABLE) && (rc == L7_ERROR))
      {
        usmDbMcastIpProtocolGet(unit, L7_AF_INET, &protocol);
        cliMcastProtocolStrPrint(protocol, stat, sizeof(stat), "%s");
        osapiSnprintf(protoStat,sizeof(protoStat),pStrInfo_ipmcast_McastProtoCfgured, stat); 
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, protoStat);
      }
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_PimDm);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configures the  mode of
* PIMDM interface in the router to enable or disable.

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
* @cmdsyntax  ip pimdm
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimdmMode(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 intMode = L7_DISABLE;
  L7_uint32 iface;
  L7_uint32 unit, numArgs, slot, port;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimDmMode_1);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimDmModeNo);
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

  slot=EWSSLOT(ewsContext);
  port=EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode=L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    intMode=FD_PIMDM_INTF_DEFAULT_MODE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (( rc = usmDbPimInterfaceModeSet(unit, L7_AF_INET, iface,
                                    intMode)) != L7_SUCCESS )
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_common_CantSetIfMode);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_OnlyLogicalIntfsAreSupported);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_IntfAdminModeFailure);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose    Enable/Disable PIM trap mode.
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
* @cmdsyntax  ip pim-trapflags
*
* @cmdhelp
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimTrapflags(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpPimTrapFlags);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpPimTrapFlagsNo);
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
      if ( usmDbPimTrapModeSet(unit, L7_ENABLE, L7_PIM_TRAP_ALL) != L7_SUCCESS )
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
      if ( usmDbPimTrapModeSet(unit, L7_DISABLE, L7_PIM_TRAP_ALL) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_ipmcast_PimDmTrapModeFailure);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configures the hello interval of
* PIMDM interface in the router to enable or disable.

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
* @cmdsyntax  ip pimdm hello-interval <seconds>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpPimdmHelloInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argHelloInt = 1;
  L7_uint32 intHelloInt = FD_PIMDM_INTF_DEFAULT_HELLO_INTERVAL;
  L7_uint32 iface;
  L7_uint32 unit, numArgs, slot, port;
  L7_char8 strHelloInt[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimDmQueryIntvl);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimDmQueryIntvlNo);
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

  slot=EWSSLOT(ewsContext);
  port=EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs == 1 ))
  {
    /*     check for valid hello interval  */
    OSAPI_STRNCPY_SAFE(strHelloInt,argv[index+ argHelloInt]);
    if ( cliCheckIfInteger(strHelloInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpPimDmQueryIntvl);
    }
    else
    {
      (void)cliConvertTo32BitUnsignedInteger(strHelloInt, &intHelloInt);
    }
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs == 0 ))
  {
    intHelloInt = FD_PIMDM_INTF_DEFAULT_HELLO_INTERVAL;
  }

  /* Hello interval should be between 10 and 3600 */
  if ( (intHelloInt >= L7_PIMDM_HELLOINTERVAL_MIN) && (intHelloInt <= L7_PIMDM_HELLOINTERVAL_MAX) )
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbPimIntfHelloIntervalSet(unit, L7_AF_INET, iface,
                                             intHelloInt) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_PimDmHelloInt);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_CfgureValIsOutOfRangeSeeCmdHelpForSyntax);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
