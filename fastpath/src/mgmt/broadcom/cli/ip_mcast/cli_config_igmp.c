/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/ip_mcast/cli_config_igmp.c
*
* @purpose config commands for the IGMP cli functionality
*
* @component user interface
*
* @comments
*
* @create
*
* @author   srikrishnas
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
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_igmp_api.h"
#include "usmdb_mib_igmp_api.h"
#include "usmdb_mib_mcast_api.h"
#endif


#include <osapi.h>
#include <nimapi.h>
#include <usmdb_ip_api.h>
#include <clicommands_l3.h>
#include <clicommands_mcast.h>
#include <usmdb_mib_igmp_api.h>
#include <usmdb_igmp_api.h>
#include <l3_commdefs.h>
#include "clicommands_card.h"
#include <cli_config_script.h>


/*********************************************************************
*
* @purpose  Sets the administrative mode of IGMP to active or inactive
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
* @cmdsyntax  ip igmp
*
* @cmdhelp
*
* @cmddescript  enable = TRUE: IGMP is active
*               disable = FALSE: IGMP is inactive on all interfaces;
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpIgmp(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs;
  L7_uint32 intMode = FD_IGMP_DEFAULT_ADMIN_MODE;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

        /* get switch ID based on presence/absence of STACKING package */
        if (unit == 0)
        {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
        }

  numArgs = cliNumFunctionArgsGet();

  if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpIgmpGlobalNo);
  }
  else if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpIgmpGlobal);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode = L7_ENABLE;
  }
  if(ewsContext->commType == CLI_NO_CMD)
  {
    intMode = FD_IGMP_DEFAULT_ADMIN_MODE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbMgmdAdminModeSet(unit, L7_AF_INET, intMode) != L7_SUCCESS )
    {
      cliSyntaxBottom(ewsContext);
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR,  ewsContext, pStrInfo_common_Igmp_1);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the interface mode of IGMP to enable or disable
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
* @cmdsyntax  ip igmp
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpIgmpInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 unit, numArgs, s, p;
    L7_uint32 iface;
    L7_RC_t rc;
    L7_uint32 intMode = FD_IGMP_INTF_DEFAULT_ADMIN_MODE;
  L7_MGMD_ERROR_CODE_t errCode;
  L7_RC_t status = L7_SUCCESS;

    cliSyntaxTop(ewsContext);

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    numArgs = cliNumFunctionArgsGet();
    if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpIgmpGlobalNo);
  }
  else if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpIgmpGlobal);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode = L7_ENABLE;

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

        if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }

        if ( (rc = usmDbMgmdInterfaceModeSet(unit, L7_AF_INET, iface,
                                             intMode,MGMD_ROUTER_INTERFACE, &errCode)) != L7_SUCCESS)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));

          switch(errCode)
          {
          case L7_MGMD_ERR_INVALID_INPUT:
            /* ERROR: Invalid Input.*/
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_Input_4);
            break;
  
          case L7_MGMD_ERR_REQUEST_FAILED:
            /* ERROR: Operation failed. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_common_OperationFailed_1);
            break;
  
          case L7_MGMD_ERR_PROXY_INTF:
              /* FAILED: IGMP Proxy configured on this interface. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_IgmpProxyCfguredOnIntf);
              break;
  
            default:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_common_OperationFailed_1);
              break;
          }/*End-of-Switch*/
        }
      }
      }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    intMode = FD_IGMP_INTF_DEFAULT_ADMIN_MODE;
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

          if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
            continue;
          }

        if ( (rc = usmDbMgmdInterfaceModeSet(unit, L7_AF_INET, iface,
                                             intMode, MGMD_ROUTER_INTERFACE, &errCode)) != L7_SUCCESS )
        {
          switch(errCode)
          {
          case L7_MGMD_ERR_INVALID_INPUT:
              /* ERROR: Invalid Input.*/
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_Input_4);
              break;
  
          case L7_MGMD_ERR_NOT_MGMD_INTF:
            /* ERROR: Given interface is not an MGMD interface.*/
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_common_GivenIntfIsNotAnIgmpIntf);
            ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
            break;
  
          case L7_MGMD_ERR_REQUEST_FAILED:
            /* ERROR: Operation failed. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_common_OperationFailed_1);
            break;
  
          default:
            /* ERROR: Operation failed. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_common_OperationFailed_1);
            break;
          }/*End-of-Switch*/
        }
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
* @purpose  Sets the interface mode of IGMP Proxy to enable or disable
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip igmp-proxy
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpIgmpProxyInterface(EwsContext ewsContext,
                                             L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs, s, p;
  L7_uint32 iface, protocol;
  L7_RC_t rc;
  L7_uint32 intMode = FD_IGMP_INTF_DEFAULT_ADMIN_MODE;
  L7_MGMD_ERROR_CODE_t errCode;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 protoStat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpIgmpProxyIntfNo);
  }
  else if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpIgmpProxyIntf);
  }


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode = L7_ENABLE;

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
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
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
        if ((rc = usmDbMgmdInterfaceModeSet(unit, L7_AF_INET, iface, intMode,
                                            MGMD_PROXY_INTERFACE, &errCode)) != L7_SUCCESS)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));

          switch (errCode)
          {
          case L7_MGMD_ERR_INVALID_INPUT:
            /* ERROR: Invalid Input.*/
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                     pStrErr_common_Input_4);
            break;

          case L7_MGMD_PROXY_ALREADY_CONFIGURED:
            /* FAILED: MGMD Proxy already configured on some other interface. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     pStrInfo_ipmcast_IgmpProxyCfguredAlready);
            break;

          case L7_MGMD_ERR_REQUEST_FAILED:
            /* ERROR: Operation failed. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                     pStrInfo_common_OperationFailed_1);
            break;

          case L7_MGMD_ERR_MGMD_INTF:
            /* FAILED: MGMD configured on this interface. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     pStrInfo_ipmcast_IgmpIntf);
            break;

          case L7_MGMD_ERR_OTHER_MCAST_ROUTING_PROTOCOL_CONFIGURED:
            /* FAILED: Some another Multicast Routing Protocol already configured. */
            usmDbMcastIpProtocolGet(unit, L7_AF_INET, &protocol);
            cliMcastProtocolStrPrint(protocol, stat, sizeof(stat), "%s");
            osapiSnprintf(protoStat,sizeof(protoStat),pStrInfo_ipmcast_McastProtoCfgured, stat);  
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     protoStat);
            break;

          case L7_MGMD_ERR_MCAST_FWD_DISABLED:
            /* FAILED: Multicast Forwarding not enabled.*/
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     pStrInfo_ipmcast_McastFwdNotEnbld);
            break;

          case L7_MGMD_ERR_MGMD_DISABLED_ON_SYSTEM:
            /* FAILED: MGMD not enabled on system. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     pStrInfo_ipmcast_IgmpNotEnbld);
            break;

          case L7_MGMD_ERR_ROUTING_NOT_ENABLED:
            /* FAILED: Routing not enabled on this interface. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     pStrInfo_ipmcast_RoutingNotEnbld);
            break;

          default:
            /* ERROR: Operation failed. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                     pStrInfo_common_OperationFailed_1);
            break;
          }/*End-of-Switch*/
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    intMode = FD_IGMP_INTF_DEFAULT_ADMIN_MODE;
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
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
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
        if ( (rc = usmDbMgmdInterfaceModeSet(unit, L7_AF_INET, iface, intMode,
                                             MGMD_PROXY_INTERFACE, &errCode)) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          switch (errCode)
          {
          case L7_MGMD_ERR_INVALID_INPUT:
            /* ERROR: Invalid Input.*/
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_Input_4);
            break;

          case L7_MGMD_ERR_NOT_PROXY_INTF:
            /* ERROR: Given interface is not an MGMD Proxy interface. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                     pStrInfo_ipmcast_NotIgmpProxyIntf);
            break;

          case L7_MGMD_ERR_REQUEST_FAILED:
            /* ERROR: Operation failed. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                     pStrInfo_common_OperationFailed_1);
            break;

          default:
            /* ERROR: Operation failed. */
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext,
                                     pStrInfo_common_OperationFailed_1);
            break;
          }/*End-of-Switch*/
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  Sets whether the optional Router Alert field is required.
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
* @cmdsyntax  ip igmp router-alert-required
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpIgmpRouterAlert(EwsContext ewsContext, L7_uint32 argc,const L7_char8 **argv, L7_uint32 index)
{
  L7_BOOL mode = L7_FALSE;
  L7_uint32 unit;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = FD_IGMP_DEFAULT_ROUTER_ALERT_CHECK;
  }

  /*******Check if the Flag is Set for Execution*************/
  if ( L7_EXECUTE_SCRIPT == ewsContext->scriptActionFlag )
  {
    if ( L7_SUCCESS != usmDbMgmdRouterAlertMandatorySet(unit, L7_AF_INET, mode))
    {
      cliSyntaxBottom( ewsContext );
      ewsTelnetWrite( ewsContext, "Failed to set Router Alert requirement." );
      return( cliPrompt( ewsContext ) );
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return( cliPrompt(ewsContext) );
}
/*********************************************************************
*
* @purpose  Sets the interface startup query interval of IGMP
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
* @cmdsyntax  ip igmp startup-query-interval <seconds>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandIpIgmpStartupQueryInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argInterval = 1;
  L7_uint32 iface;
  L7_uint32 interval=0;
  L7_uint32 unit, numArgs, s, p;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpStartupQueryIntvl_1);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpStartupQueryIntvlNo);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    interval=FD_IGMP_STARTUP_QUERY_INTERVAL;
  }
  else if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if ( (cliConvertTo32BitUnsignedInteger(argv[index+argInterval], &interval) != L7_SUCCESS) ||(interval < L7_IGMP_STARTUP_QUERY_INTERVAL_MIN) ||(interval > L7_IGMP_STARTUP_QUERY_INTERVAL_MAX) )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_ipmcast_OutsideValidRangeForStartupQueryIntvl); /* value outside range */
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrInfo_ipmcast_InputOutOfRange,L7_IGMP_STARTUP_QUERY_INTERVAL_MIN,L7_IGMP_STARTUP_QUERY_INTERVAL_MAX );
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
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
      if (usmDbMgmdInterfaceStartupQueryIntervalSet(unit, L7_AF_INET, iface, interval) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_ipmcast_SetStartupQueryIntvl);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the interface unsolicited report interval of IGMP Proxy
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip igmp-proxy  unsolicit-rprt-interval <seconds>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandIpIgmpProxyUnsolicitedReportInterval(EwsContext
                                                            ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 iface;
  L7_uint32 interval=0;
  L7_uint32 unit, numArgs, s, p;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argInterval = 1;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();

  osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf),  pStrErr_ipmcast_IpIgmpProxyUnSolicitedIntvl_1,
                          L7_IGMP_UNSOLICITED_REPORT_INTERVAL_MIN,
                          L7_IGMP_UNSOLICITED_REPORT_INTERVAL_MAX);

  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpProxyUnSolicitedIntvlNo);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    interval = FD_IGMP_DEFAULT_UNSOLICITED_REPORT_INTERVAL ;
  }
  else if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if ( (cliConvertTo32BitUnsignedInteger(argv[index+argInterval], &interval)
          != L7_SUCCESS) ||(interval < L7_IGMP_UNSOLICITED_REPORT_INTERVAL_MIN )
         ||(interval > L7_IGMP_UNSOLICITED_REPORT_INTERVAL_MAX ) )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_MgmdProxyReportIntvl); /* value outside range */
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrInfo_ipmcast_InputOutOfRange,
                                             L7_IGMP_UNSOLICITED_REPORT_INTERVAL_MIN,
                                             L7_IGMP_UNSOLICITED_REPORT_INTERVAL_MAX );
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
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

      if (usmDbMgmdProxyInterfaceUnsolicitedIntervalSet(unit, L7_AF_INET, iface,
                                                        interval) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_ipmcast_CantSetSolicitedIntvl);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the  interface startup query count of IGMP
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
* @cmdsyntax  ip igmp startup-query-count <count>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandIpIgmpStartupQueryCount(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argCount = 1;
  L7_uint32 iface;
  L7_uint32 count=0;
  L7_uint32 unit, numArgs, s, p;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpStartupQueryCount_1);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpStartupQueryCountNo);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    count= FD_IGMP_STARTUP_QUERY_COUNT;
  }
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argCount], &count) == L7_SUCCESS)
    {
      if (( count < L7_IGMP_STARTUP_QUERY_COUNT_MIN) || ( count > L7_IGMP_STARTUP_QUERY_COUNT_MAX ))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_ipmcast_OutsideValidRangeForStartupQueryCountChooseAValFrom120);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_ipmcast_StartupQueryCountMustBeAnIntegerChooseAValFrom120);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        continue;
      }
      if (usmDbMgmdInterfaceStartupQueryCountSet(unit, L7_AF_INET, iface, count) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_ipmcast_SetStartupQueryCount);
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
* @purpose  Sets the interface last member query count of IGMP
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
* @cmdsyntax  ip igmp last-member-query-count <count>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpIgmpLastMemberQueryCount(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argCount = 1;
  L7_uint32 iface;
  L7_uint32 count=0;
  L7_uint32 unit, numArgs, s, p;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpLastMbrQueryCount_1);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpLastMbrQueryCountNo);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    count = FD_IGMP_LAST_MEMBER_QUERY_COUNT;
  }
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argCount], &count) == L7_SUCCESS)
    {
      if (( count < L7_IGMP_LAST_MEM_QUERY_COUNT_MIN ) || ( count > L7_IGMP_LAST_MEM_QUERY_COUNT_MAX ))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_common_OutsideValidRangeForLastMbrQueryCountChooseAValFrom120);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_common_LastMbrQueryCountMustBeAnIntegerChooseAValFrom120);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        continue;
      }

      if (usmDbMgmdLastMembQueryCountSet(unit, L7_AF_INET, iface,
                                         count) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_common_SetLastMbrQueryCount);
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
* @purpose  Sets the interface query interval of IGMP
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
* @cmdsyntax  ip igmp query-interval <seconds>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandIpIgmpQueryInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argInterval = 1;
  L7_uint32 iface;
  L7_uint32 interval=0;
  L7_uint32 maxRespTime;
  L7_uint32 unit, numArgs, s, p;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpQueryIntvl_1);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpQueryIntvlNo);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    interval = FD_IGMP_QUERY_INTERVAL;
  }
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argInterval], &interval) == L7_SUCCESS)
    {
      if (( interval < L7_IGMP_QUERY_INTERVAL_MIN ) || ( interval > L7_IGMP_QUERY_INTERVAL_MAX ))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_common_OutsideValidRangeForQueryIntvlChooseAValFrom13600);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_common_QueryIntvlMustBeAnIntegerChooseAValFrom13600);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        continue;
      }
      if (usmDbMgmdInterfaceQueryMaxResponseTimeGet(unit, L7_AF_INET,
                                                    iface, &maxRespTime) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_common_SetQueryIntvl);
      }
      else
      {
        if ((interval * 10) <= maxRespTime)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_common_MaxRespTimeMustBeLessThanGrpMbrshipIntvl);
        }
        else
        {

          if (usmDbMgmdInterfaceQueryIntervalSet(unit, L7_AF_INET, iface, interval) != L7_SUCCESS)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_common_SetQueryIntvl);
          }
        }
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
* @purpose  Sets the interface version of IGMP or IGMP Proxy whichever is
*           configured on the given interface. If none of these two is
*           configured on the given interface then returns failure.
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
* @cmdsyntax  ip igmp version <version>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandIpIgmpVersion(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argVersion = 1;
  L7_uint32 iface;
  L7_uint32 version = FD_IGMP_DEFAULT_VER;
  L7_uint32 unit, numArgs, s, p;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpVer_1);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpVerNo);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    version=FD_IGMP_DEFAULT_VER;
  }
  else if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argVersion], &version) == L7_SUCCESS)
    {
      if (( version < L7_IGMP_VERSION_MIN ) || ( version > L7_IGMP_VERSION_MAX ))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_ipmcast_OutsideValidRangeForVerChooseAValFrom12);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_ipmcast_VerMustBeAnIntegerChooseAValFrom12);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        continue;
      }

      if (usmDbMgmdInterfaceVersionSet(unit, L7_AF_INET, iface, version) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_ipmcast_SetVer);
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
* @purpose  Sets the IGMP interface Max response time
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
* @cmdsyntax  ip igmp query-max-response-time <seconds>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandIpIgmpQueryMaxResponseTime(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMaxRespTime = 1;
  L7_uint32 iface;
  L7_uint32 maxRespTime=0;
  L7_uint32 queryInterval;
  L7_uint32 unit, numArgs, s, p;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpQueryMaxRespTime_1);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpQueryMaxRespTimeNo);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    maxRespTime = FD_IGMP_QUERY_RESPONSE_INTERVAL;
  }
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argMaxRespTime], &maxRespTime) == L7_SUCCESS)
    {
      if ((maxRespTime < L7_IGMP_RESPONSE_TIME_MIN) || (maxRespTime > L7_IGMP_RESPONSE_TIME_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_common_OutsideValidRangeForMaxRespTimeChooseAValFrom0255);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_common_MaxRespTimeMustBeAnIntegerChooseAValFrom0255);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        continue;
      }
      if (usmDbMgmdInterfaceQueryIntervalGet(unit, L7_AF_INET, iface,
                                             &queryInterval) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_MaxRespTime);
      }
      else
      {
        if ((queryInterval * 10) <= maxRespTime)
        {
          return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_common_MaxRespTimeMustBeLessThanGrpMbrshipIntvl);
        }
        else
        {
          if (usmDbMgmdInterfaceQueryMaxResponseTimeSet(unit, L7_AF_INET,
                                                        iface, maxRespTime) != L7_SUCCESS)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWrite (ewsContext, pStrInfo_common_MaxRespTime);
          }
        }
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
* @purpose  Sets the IGMP interface robustness
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
* @cmdsyntax  ip igmp robustness <robustness>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandIpIgmpRobustness(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argRobustness = 1;
  L7_uint32 iface;
  L7_uint32 robustness=0;
  L7_uint32 unit, numArgs, s, p;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpRobustness_1);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpRobustnessNo);
  }


  if (ewsContext->commType == CLI_NO_CMD)
  {
    robustness = FD_IGMP_ROBUST_VARIABLE;
  }
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argRobustness], &robustness) == L7_SUCCESS)
    {
      if (( robustness < L7_IGMP_ROBUST_MIN) || ( robustness > L7_IGMP_ROBUST_MAX ))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_common_OutsideValidRangeForRobustnessChooseAValFrom1255);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_common_RobustnessMustBeAnIntegerChooseAValFrom1255);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        continue;
      }

      if (usmDbMgmdInterfaceRobustnessSet(unit, L7_AF_INET, iface, robustness) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_common_SetRobustness);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the IGMP interface last member query interval
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
* @cmdsyntax  ip igmp interface last-member-query-interval <seconds>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandIpIgmpLastMemberQueryInterval(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argLastMembQueryInterval = 1;
  L7_uint32 iface;
  L7_uint32 lastMembQueryInterval=0;
  L7_uint32 unit, numArgs, s, p;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpLastMbrQueryIntvl_1);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpLastMbrQueryIntvlNo);
  }


  if (ewsContext->commType == CLI_NO_CMD)
  {
    lastMembQueryInterval = FD_IGMP_LAST_MEMBER_QUERY_INTERVAL;
  }
  else if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argLastMembQueryInterval], &lastMembQueryInterval) == L7_SUCCESS)
    {
      if (( lastMembQueryInterval < L7_IGMP_LAST_MEM_QUERY_INTERVAL_MIN) || ( lastMembQueryInterval > L7_IGMP_LAST_MEM_QUERY_INTERVAL_MAX ))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_common_OutsideValidRangeForLastMbrQueryIntvlChooseAValFrom0255);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_common_LastMbrQueryIntvlMustBeAnIntegerChooseAValFrom0255);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        continue;
      }

      if (usmDbMgmdInterfaceLastMembQueryIntervalSet(unit, L7_AF_INET, iface,
                                                     lastMembQueryInterval) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrInfo_common_SetLastMbrQueryIntvl);
        continue;
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
* @purpose  Resets the IGMP Proxy host interface status parameters
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip igmp-proxy reset-status
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpIgmpProxyResetHostStatus(EwsContext ewsContext,
                                                   L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs, s, p;
  L7_uint32 iface;
  L7_RC_t rc;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpProxyResetHostsTatus);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        continue;
      }

      if ( (rc = usmDbMgmdProxyResetHostStatus(unit, L7_AF_INET, iface)) != L7_SUCCESS )
      {
        if (rc == L7_NOT_SUPPORTED)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,pStrErr_ipmcast_SetIntfMode);
        }
        else
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext,pStrErr_ipmcast_SetIntfMode);
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}
#ifdef MGMD_TBD /*  CLI command is not there  */
/*********************************************************************
*
* @purpose  Sets the IGMP interface as querier
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
* @cmdsyntax  ip igmp-proxy querier-set
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpIgmpQuerierSet(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs, s, p;
  L7_uint32 iface;
  L7_RC_t rc;
  L7_uint32 intMode;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpQuerierSetNo );
  }
  else if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpQuerierSet );
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode = L7_ENABLE;

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          continue;
        }

        if ( (rc = usmDbMgmdInterfaceQuerierSet(unit, L7_AF_INET, iface, intMode)) != L7_SUCCESS )
        {
          if (rc == L7_NOT_SUPPORTED)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_ipmcast_SetIntfMode );
            ewsTelnetWriteAddBlanks (ewsContext, pStrErr_ipmcast_CfgRtrIgmpIntfSnoopingNotEnbld);
          }
          else
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrInfo_ipmcast_SetIntfMode_2);
          }
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    intMode = L7_DISABLE;
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          continue;
        }

        if ( (rc = usmDbMgmdInterfaceQuerierSet(unit, L7_AF_INET, iface,
                                                intMode)) != L7_SUCCESS )
        {
          if (rc == L7_NOT_SUPPORTED)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_ipmcast_SetIntfMode );
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_OnlyLogicalIntfsAreSupported );
          }
          else
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrInfo_ipmcast_SetIntfMode_2);
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
* @purpose  Sets the IGMP Proxy forwarding to enable or disable
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
* @cmdsyntax  ip igmp-proxy upstream-forward-disable
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpIgmpProxyUpstreamFwdDisable(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs, s, p;
  L7_uint32 iface;
  L7_RC_t rc;
  L7_uint32 intMode = FD_IGMP_INTF_DEFAULT_ADMIN_MODE;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpProxyUpstreamFwdDsblNo );
  }
  else if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpIgmpProxyUpstreamFwdDsbl);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode = L7_ENABLE;

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          continue;
        }

        if ( (rc = usmDbMgmdProxyUpstreamFwdDisable(unit, L7_AF_INET, iface,
                                                    intMode)) != L7_SUCCESS )
        {
          if (rc == L7_NOT_SUPPORTED)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_ipmcast_SetIntfMode );
            ewsTelnetWriteAddBlanks (ewsContext, pStrErr_ipmcast_CfgRtrIgmpIntfSnoopingNotEnbld);
          }
          else
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_ipmcast_SetIntfMode );
          }
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    intMode = L7_DISABLE ;
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {

      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          continue;
        }

        if ( (rc = usmDbMgmdProxyUpstreamFwdDisable(unit, L7_AF_INET, iface,
                                                    intMode)) != L7_SUCCESS )
        {
          if (rc == L7_NOT_SUPPORTED)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_ipmcast_SetIntfMode );
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_OnlyLogicalIntfsAreSupported );
          }
          else
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_OnlyLogicalIntfsAreSupported );
          }
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}
#endif
