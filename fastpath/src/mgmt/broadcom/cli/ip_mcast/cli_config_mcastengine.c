/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/ip_mcast/cli_config_mcastengine.c
 *
 * @purpose config commands for the mcast forwarding engine cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  15/05/2002
 *
 * @author  srikrishnas
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_ip_mcast_common.h"
#include "strlib_routing_cli.h"
#include "strlib_ip_mcast_cli.h"
#include "l3_mcast_commdefs.h"
#include <usmdb_mib_mcast_api.h>
#include "clicommands_mcast.h"
#include <usmdb_igmp_api.h>

#include "l3_mcast_defaultconfig.h"
#include "clicommands_card.h"
#include "cli_config_script.h"
#include "usmdb_util_api.h"
#include "usmdb_ip_api.h"
#include "cli_web_exports.h"
#include "osapi.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_mcast_api.h"
#endif


/*********************************************************************
* @purpose    Configures the mcast administrative mode of
* the router to enable or disable.
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip multicast
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpMulticast(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, numArgs;
  L7_uint32 intMode = FD_MCAST_DEFAULT_ADMIN_MODE;

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

  if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpMcastRoutingNo);
  }
  else if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_DsblIpMcastRouting);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode = L7_ENABLE;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbMcastAdminModeSet(unit, intMode) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_ipmcast_SetMcastAdminMode);
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    intMode = FD_MCAST_DEFAULT_ADMIN_MODE;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbMcastAdminModeSet(unit, intMode) != L7_SUCCESS )
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_ipmcast_AdminMode);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose    Configures the mcast TTL threshold
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip multicast ttl-threshold <ttl-value>
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpMulticastTtlThreshold(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argTtl = 1;
  L7_uint32 iface;
  L7_uint32 unit, numArgs, s, p;
  L7_char8 strTtlThreshold[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 ttlThreshold = FD_MCAST_INTF_DEFAULT_TTL_THRESHOLD;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 1 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpMcastTtlThresh);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpMcastTtlThreshNo);
  }

  if((ewsContext->commType == CLI_NO_CMD) && ( numArgs == 0 ))
  {
    ttlThreshold = FD_MCAST_INTF_DEFAULT_TTL_THRESHOLD;
  }
  else if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs == 1 ))
  {
    /*     check for valid ttlthreshold value  */
    OSAPI_STRNCPY_SAFE(strTtlThreshold,argv[index+ argTtl]);
    if ( cliCheckIfInteger(strTtlThreshold) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_Error,  ewsContext, pStrErr_ipmcast_ValTtlThresh);
    }
    else
    {
      (void)cliConvertTo32BitUnsignedInteger(strTtlThreshold, &ttlThreshold);
    }
  }

  /* ttlthreshold should be between 0 and 255 */
  if (( ttlThreshold >= L7_MCAST_TTL_THRESHOLD_MIN) && ( ttlThreshold <= L7_MCAST_TTL_THRESHOLD_MAX))
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

        if ( usmDbMcastIpMRouteInterfaceTtlSet(unit, iface, ttlThreshold) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_ipmcast_SpecifiedIntfIsNotValidForSettingTtlThresh);
        }
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_Error,  ewsContext, pStrErr_ipmcast_ValTtlThresh);
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Creates a static route in the static mcast table which will be
*         used for checking the RPF in multicast packet forwarding.
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip multicast staticroute <sourceipaddr> <mask> <rpfipaddr> <metric> <slot/port>
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpMulticastStaticroute(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSrcIp = 1;
  L7_uint32 argMask = 2;
  L7_uint32 argRpfIp = 3;
  L7_uint32 argMetric = 4;
  L7_uint32 iface = 0;
  L7_uint32 unit = 1, numArgs;
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMetric[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ipVal, ipRpf, ipMask, metric;
  L7_inet_addr_t inetIpVal, inetIpRpf, inetIpMask;
  L7_uchar8 addrFamily = L7_AF_INET;
  L7_RC_t rc = L7_FAILURE;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs < 4 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_ipmcast_IpMcastStaticRoute);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 2 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpMcastStaticRouteNo);
  }

  OSAPI_STRNCPY_SAFE(strIpAddr,argv[index + argSrcIp]);
  if (usmDbInetAton(strIpAddr, (L7_uint32 *)&ipVal) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_McastInValSrcIp);
  }
  inetAddressSet(addrFamily, &ipVal, &inetIpVal);

  OSAPI_STRNCPY_SAFE(strIpAddr,argv[index + argMask]);
  if (usmDbInetAton(strIpAddr, (L7_uint32 *)&ipMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ipmcast_SubnetMask_2);
  }
  if (usmDbNetmaskIsContiguous(ipMask) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NonContiguousMask);
  }
  inetAddressSet(addrFamily, &ipMask, &inetIpMask);

  if (inetAddrIsHostBitSet (&inetIpVal, &inetIpMask) == L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpMcastHostBitsSet);
  }

  if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbMcastStaticMRouteDelete(unit, addrFamily, &inetIpVal, &inetIpMask) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_ipmcast_DelStaticRoute);
      }
    }
  }
  else if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    OSAPI_STRNCPY_SAFE(strIpAddr,argv[index + argRpfIp]);
    if (usmDbInetAton(strIpAddr, (L7_uint32 *)&ipRpf) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_McastInValRpfIp);
    }

    /*check for valid metric value  */
    OSAPI_STRNCPY_SAFE(strMetric,argv[index+ argMetric]);
    if ( cliCheckIfInteger(strMetric) == L7_SUCCESS)
    {
      (void)cliConvertTo32BitUnsignedInteger(strMetric, &metric);
      /* metric should be between 1 and 255 */
      if (( metric >= L7_MCAST_STATICROUTE_METRIC_MIN) && ( metric <= L7_MCAST_STATICROUTE_METRIC_MAX))
      {
        inetAddressSet(addrFamily, &ipRpf, &inetIpRpf);
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ((rc = usmDbMcastStaticMRouteAdd(unit, addrFamily, &inetIpVal, &inetIpMask, &inetIpRpf, iface, metric)) != L7_SUCCESS )
          {
            switch (rc)
            {
              case L7_FAILURE:
                cliSyntaxTop(ewsContext);
                return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_ipmcast_AddStaticRoute);

              case L7_ERROR:
                cliSyntaxTop(ewsContext);
                return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput,  ewsContext, pStrErr_ipmcast_StaticMRouteSrcRpfAddrError);
             
              case L7_NOT_SUPPORTED:
                cliSyntaxTop(ewsContext);
                return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput,  ewsContext,  pStrErr_ipmcast_AddrError);  
              case L7_REQUEST_DENIED:
                cliSyntaxTop(ewsContext);
                return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput,  ewsContext,  pStrErr_ipmcast_AddrErrorLocal);
              default:
                break;
            }
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_ipmcast_ValMetric);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose   Adds an administrative scope multicast boundary specified by <groupipaddr>  <mask>
*            & <slot/port> for which this multicast administrative boundary is applicable
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  L7_uint32 index
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
* @notes
*
* @cmdsyntax  ip mcast boundary <group ipaddr> <mask>
* @cmdhelp
* @cmddescript
* @end
*
*********************************************************************/
const L7_char8 *commandIpMcastBoundary(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argGrpIp = 1;
  L7_uint32 argMask = 2;
  L7_uint32 iface, s, p;
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ipVal, ipMask, rc;
  L7_uint32 unit, numArgs;
  L7_inet_addr_t inetIpVal, inetIpMask;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if((ewsContext->commType == CLI_NORMAL_CMD) && ( numArgs != 2 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpMcastBoundary);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && ( numArgs != 2 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_IpMcastBoundaryNo);
  }

  OSAPI_STRNCPY_SAFE(strIpAddr,argv[index + argGrpIp]);
  if (usmDbInetAton(strIpAddr, (L7_uint32 *)&ipVal) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_TacacsInValIp);
  }
  if((ipVal < L7_MCAST_ADMINSCOPE_ADDR_MIN) || (ipVal > L7_MCAST_ADMINSCOPE_ADDR_MAX))
  {
    cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrInfo_ipmcast_GrpAddrIsOutsideAdminScopeIpv4McastRange);
  }

  OSAPI_STRNCPY_SAFE(strIpAddr,argv[index + argMask]);

  if (usmDbInetAton(strIpAddr, (L7_uint32 *)&ipMask) != L7_SUCCESS)
  {
    cliSyntaxTop(ewsContext);
    ewsTelnetWriteAddBlanks (0, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ipmcast_SubnetMask_2);
    return cliPrompt(ewsContext);
  }
  if (usmDbNetmaskIsContiguous(ipMask) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NonContiguousMask);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      inetAddressSet(L7_AF_INET, &ipVal, &inetIpVal);
      inetAddressSet(L7_AF_INET, &ipMask, &inetIpMask);

      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        rc = usmDbMcastMrouteBoundaryAdd(unit, iface, &inetIpVal, &inetIpMask);

        if (rc != L7_SUCCESS)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        }

        switch(rc)
        {
        case L7_FAILURE:
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_ipmcast_AddEntryToMcastBoundaryTbl_1);
          break;
  
        case L7_TABLE_IS_FULL:
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_MaxNumOfAdminScopeEntriesReached);
          break;
  
        case L7_ERROR:
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_UserInput,  ewsContext, pStrInfo_ipmcast_GrpAddrHasHostBitsSet);
          break;
  
        case L7_ALREADY_CONFIGURED:
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_BoundaryAlreadyExistsOnIntf);
          break;
  
        default:
          break;
        }
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      inetAddressSet(L7_AF_INET, &ipVal, &inetIpVal);
      inetAddressSet(L7_AF_INET, &ipMask, &inetIpMask);

      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        rc = usmDbMcastMrouteBoundaryDelete(unit, iface, &inetIpVal, &inetIpMask);
        if ( rc == L7_FAILURE )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_ipmcast_UnableToDelAdminScopeBoundary);
        }
  
        if ( rc == L7_ALREADY_CONFIGURED )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_NoSuchBoundaryExistsOnIntf);
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
