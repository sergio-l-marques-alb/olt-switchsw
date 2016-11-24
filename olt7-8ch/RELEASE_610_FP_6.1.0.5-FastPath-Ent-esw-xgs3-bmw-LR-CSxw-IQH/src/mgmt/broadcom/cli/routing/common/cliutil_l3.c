/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/cliutil_l3.c
 *
 * @purpose assorted functions for cli routing commands
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   09/16/2002
 *
 * @author  Jill Flanagan
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "cliapi.h"
#include "usmdb_1213_api.h"
#include "cli_web_exports.h"
#include "usmdb_iputil_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_rlim_api.h"
#include "usmdb_util_api.h"

#ifndef _L7_OS_LINUX_
#include <vxworks_config.h>       /* for DEFAULT_MAX_CONNECTIONS */
#endif /* _L7_OS_LINUX_ */

#include "clicommands_loopback.h"
#include "rlim_api.h"

#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"
#endif

/*********************************************************************
 *
 * @purpose determine if the specified interface is valid for routing
 *
 * @param ewsContext
 * @param iface (input) interface to validate
 *
 * @returns  L7_SUCCESS  The interface is valid for routing
 * @returns  L7_FAILURE  The interface is not valid for routing
 *
 * @notes   If port-based routing is supported, any slot/port is
 *          considered valid.  This routine assumes the range for
 *          slot and port have already been verified.
 * @end
 *
 *********************************************************************/
L7_RC_t cliValidateRtrIntf(EwsContext ewsContext, L7_uint32 iface)
{
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  if( usmDbValidateRtrIntf(unit, iface) == L7_FAILURE )
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWrite( ewsContext,  pStrInfo_routing_AValidRoutingIntfMustBeSpecified);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  check whether virtual router ID is in 1-255 range or not
 *
 * @param EwsContext ewsContext
 * @param const L7_char8 *routerID
 * @param  L7_uchar8 *routID
 * @returns L7_RC_t
 *
 * @notes none
 *
 *********************************************************************/

L7_RC_t cliConvertRouterID(EwsContext ewsContext, const L7_char8 * routerID, L7_uchar8 * routID)
{

  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intRouterId;

  OSAPI_STRNCPY_SAFE(buf, routerID);

  if (cliCheckIfInteger(buf) == L7_SUCCESS)
  {
    if (( cliConvertTo32BitUnsignedInteger(buf, &intRouterId) == L7_SUCCESS) 
        && ((intRouterId >= L7_ROUTING_VRRP_MIN_VRID) && (intRouterId <= L7_ROUTING_VRRP_MAX_VRID)))
    {
      *routID = intRouterId;
      return L7_SUCCESS;
    }
  }

  *routID = 0;
  osapiSnprintfAddBlanks (0, 0, 0, 0, pStrErr_common_Error, buf, sizeof(buf),  
      pStrErr_routing_RtrIdMustBeInRangeOfTo, L7_ROUTING_VRRP_MIN_VRID, L7_ROUTING_VRRP_MAX_VRID);
  ewsTelnetWrite( ewsContext, buf);
  return L7_FAILURE;
}

/*********************************************************************
 *
 * @purpose To check the LoopbackId whether it is in appropriate range or not.
 *
 * @param L7_char8 *buf
 *
 * @returns  L7_SUCCESS  means the tunnelId is in the valid range
 * @returns  L7_FAILURE  means the tunnelId is not in the valid range
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cliValidateLoopbackId(EwsContext ewsContext,
    L7_char8 * buf, L7_uint32 * pVal)
{
  if((cliConvertTo32BitUnsignedInteger(buf, pVal) == L7_SUCCESS) &&
      (*pVal >= L7_CLIMIN_LOOPBACKID  && *pVal <= L7_CLIMAX_LOOPBACKID))
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}
/*********************************************************************
 * @purpose Parse a CLI command string for an interface name.
 *
 * @param   the usual stuff, plus....
 *          nextArg - the argument number in argv to process as an interface name
 *          intIfNum - internal interface number of the interface named in the 
 *                     command
 *
 * @returns  L7_SUCCESS  if interface parsed and converted to valid 
 *                       internal interface number
 * @returns  L7_FAILURE  otherwise
 *
 * @notes   The interface can be named as a u/s/p (or just s/p for non-stacking)
 *          or it can be a loopback interface number given as "loopback n" where
 *          n is an integer. Could also be a tunnel. 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cliParseInterface(EwsContext ewsContext, L7_uint32 argc,
    const L7_char8 **argv, L7_uint32 nextArg,
    L7_uint32 *intIfNum)
{
#ifdef L7_RLIM_PACKAGE
  L7_uint32 loopbackID;
#endif
  L7_uint32 unit, slot, port;

#ifdef L7_IPV6_PACKAGE
#ifdef L7_RLIM_PACKAGE
  L7_uint32 tunnelID;
#endif
#endif

#ifdef L7_RLIM_PACKAGE
  /* Get the internal interface number of the numbered interface. */
  if ((strcmp(argv[nextArg], RLIM_INTF_LOOPBACK_NAME_PREFIX) == 0))
  {
    if (cliValidateLoopbackId(ewsContext, (L7_char8 *)argv[nextArg+1],
          &loopbackID) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (usmDbRlimLoopbackIntIfNumGet(loopbackID, intIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else

#ifdef L7_IPV6_PACKAGE
  /* check if it's a tunnel interface name */
  if(strcmp(argv[nextArg], RLIM_INTF_TUNNEL_NAME_PREFIX) == 0)
  {
    if(cliValidateTunnelId(ewsContext, (L7_char8 *)argv[nextArg+1], &tunnelID)
        != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if(usmDbRlimTunnelIntIfNumGet(tunnelID, intIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    else
    {
      if(usmDbUnitSlotPortGet(*intIfNum, &unit, &slot, &port) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  }
  else
#endif
#endif
  
  {
    /* Not a loopback or tunnel interface. Get usp. */
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[nextArg], &unit, &slot, &port) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, intIfNum) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
    else  /* non-stacking */
    {
      /* NOTE: No need to check the value of `unit` as
       *       ID of a standalone switch is always `U_IDX` (=> 1).
       */
      if (cliSlotPortToIntNum(ewsContext, argv[nextArg], &slot, &port, 
            intIfNum) != L7_SUCCESS)
        return L7_FAILURE;
    }

    if (cliSlotPortCpuCheck(slot, port) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (cliIntfIsNamedByUSP(*intIfNum) == L7_FALSE)
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}


#ifdef L7_IPV6_PACKAGE
#ifdef L7_RLIM_PACKAGE
/*********************************************************************
 *
 * @purpose Validating the tunnelId
 *
 * @param L7_char8 *buf
 *
 * @returns  L7_SUCCESS  means the tunnelId is in the valid range
 * @returns  L7_FAILURE  means the tunnelId is not in the valid range
 *
 * @end
 *
 *********************************************************************/

L7_RC_t cliValidateTunnelId(EwsContext ewsContext, L7_char8 * buf,
    L7_uint32 * pVal)
{
  if ((cliConvertTo32BitUnsignedInteger(buf, pVal) == L7_SUCCESS) &&
      (*pVal >= L7_CLIMIN_TUNNELID  && *pVal <= L7_CLIMAX_TUNNELID))
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}
#endif
#endif

/*********************************************************************
 *
 * @purpose This is an utility function to
 *          get the interface, slot, port and unit from the tunnel id.
 *
 * @param L7_uint32 tunnelId
 * @param L7_uint32 *intf
 * @param L7_uint32 *slot
 * @param L7_uint32 *port
 * @param L7_uint32 *unit
 * @param L7_uint32 intfType
 *
 * @returns  L7_SUCCESS  means the tunnelId is in the valid range
 * @returns  L7_FAILURE  means the tunnelId is not in the valid range
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cliGetUnitSlotPortIntf(EwsContext ewsContext, L7_uint32 intfId,
    L7_uint32 * intf, L7_uint32 * unit, L7_uint32 * slot, L7_uint32 * port,
    L7_uint32 intfType)
{
  if(cliIsStackingSupported() == L7_TRUE)
  {
    *unit = EWSUNIT(ewsContext);
  }
  else
  {
    *unit = cliGetUnitId();
  }

  *slot = EWSSLOT(ewsContext);
  *port = EWSPORT(ewsContext);

  if (cliSlotPortCpuCheck(*slot, *port) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  switch(intfType)
  {

#ifdef L7_RLIM_PACKAGE
#ifdef L7_IPV6_PACKAGE
    case L7_TUNNEL_INTF:
      {
        if(usmDbRlimTunnelIntIfNumGet(intfId, intf)!= L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_routing_TunnelIntf);
          cliSyntaxBottom(ewsContext);
          return L7_FAILURE;
        }

        if(cliValidateRtrIntf(ewsContext, *intf) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          cliSyntaxBottom(ewsContext);
          return L7_FAILURE;
        }
        break;
      }
#endif
    case L7_LOOPBACK_INTF:
      {
        if(usmDbRlimLoopbackIntIfNumGet(intfId, intf) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_LoopBackIntfNotFound);
          cliSyntaxBottom(ewsContext);
          return L7_FAILURE;
        }

        if(cliValidateRtrIntf(ewsContext, *intf) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          cliSyntaxBottom(ewsContext);
          return L7_FAILURE;
        }
        break;
      }
#endif
    default:
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_InvalidIntfType);
        return L7_FAILURE;
        break;
      }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose This is an utility function returns the name of the inter-
 *          face according to its type.
 *
 * @param   L7_uint32 intfNum  number of the interface
 * @param   L7_uint32 unit     unit number
 * @param   L7_uint32 slot     slot number
 * @param   L7_uint32 port     port number
 *
 * @enums L7_LOOPBACK_INTF and L7_LOOPBACK_INTF  are interface types
 *         defined in the structure L7_INTF_TYPES_t
 *
 * @returns Interface display string
 *
 * @end
 *********************************************************************/
L7_char8 *cliGetIntfName(L7_uint32 intfNum, L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  L7_uint32 sysIntfType;
  static L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
#ifdef L7_RLIM_PACKAGE
  L7_uint32 loopbackId, tunnelId;
#endif

  if(nimGetIntfType(intfNum, &sysIntfType) == L7_SUCCESS)
  {
    switch (sysIntfType)
    {
#ifdef L7_RLIM_PACKAGE
      case L7_LOOPBACK_INTF:
        if (usmDbRlimLoopbackIdGet(intfNum, &loopbackId) == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf),  "%s %d", RLIM_INTF_LOOPBACK_NAME_PREFIX, loopbackId);
        }
        break;
      case L7_TUNNEL_INTF:
        if (usmDbRlimTunnelIdGet(intfNum, &tunnelId) == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf),  "%s %d", RLIM_INTF_TUNNEL_NAME_PREFIX, tunnelId);
        }
        break;
#endif
      default:
        osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(unit, slot, port));
        break;
    }
  }
  return buf;
}

/*********************************************************************
 *
 * @purpose This is an utility function validates the interface
 *
 * @param   L7_uint32 intfNum  Number of the interface
 *
 * @enums L7_LOOPBACK_INTF and L7_LOOPBACK_INTF  are interface types
 *        defined in the structure L7_INTF_TYPES_t
 *
 * @returns L7_TRUE if it is a loopback or tunnel interface
 * @returns L7_FALSE for all other.
 *
 * @end
 *********************************************************************/
L7_BOOL cliIntfIsNamedByUSP(L7_uint32 intfNum)
{
  L7_uint32 intfType;
  if ((usmDbIntfTypeGet(intfNum, &intfType) == L7_SUCCESS) &&
      (intfType == L7_LOOPBACK_INTF || intfType == L7_TUNNEL_INTF))
  {
    return L7_FALSE;
  }
  else
  {
    return L7_TRUE;
  }
}

/*********************************************************************
*
* @purpose  Checks the validity of the match options entered by the user
*
*
* @param const L7_char8 **matchArray
* @param L7_uint32 numArg
* @param L7_uint32 *matchOptions
*
*
* @returntype L7_RC_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure

*
* @notes none
*
*
* @end
*
*********************************************************************/
L7_RC_t compareMatchOptions(const L7_char8 * * matchArray, L7_uint32 numArg, 
                            L7_uint32 matchPos, L7_uint32 * matchOptions, L7_uint32 * newPos)
{
  L7_uint32 position, argMatchType = 0;

  position = matchPos+1;
  if (position > numArg)
  {
    /*ewsTelnetWrite( ewsContext, "Insufficient Arguments: match type not specified");*/
    return L7_ERROR;
  }

  while (position <= numArg)
  {
    if (strcmp(matchArray[position],pStrInfo_common_Internal_2)==0)
    {
      argMatchType = argMatchType | L7_OSPF_METRIC_TYPE_INTERNAL;
      position = position+1;
    }
    else if (strcmp(matchArray[position], pStrInfo_common_External) == 0)
    {
      position = position+1;
      if (position > numArg)
      {
        return L7_ERROR;
      }  
      if (strcmp(matchArray[position],pStrInfo_common_RateMbps) == 0)
      {
        argMatchType = argMatchType | L7_OSPF_METRIC_TYPE_EXT1;
        position = position+1;
      }
      else if (strcmp(matchArray[position],pStrInfo_common_Rate2Mbps_1) == 0)
      {
        argMatchType = argMatchType | L7_OSPF_METRIC_TYPE_EXT2;
        position = position+1;
      }
      else
      {
        return L7_ERROR;
      }
    }
    else if (strcmp(matchArray[position], pStrInfo_common_NssaExternal_1) == 0)
    {
      position = position+1;
      if (position > numArg)
      {
        return L7_ERROR;
      }  
      if (strcmp(matchArray[position],pStrInfo_common_RateMbps) == 0)
      {
        argMatchType = argMatchType | L7_OSPF_METRIC_TYPE_NSSA_EXT1;
        position = position+1;
      }
      else if (strcmp(matchArray[position],pStrInfo_common_Rate2Mbps_1) == 0)
      {
        argMatchType = argMatchType | L7_OSPF_METRIC_TYPE_NSSA_EXT2;
        position = position+1;
      }
      else
      {
        return L7_ERROR;
      }
    }
    else
    {
      return L7_ERROR;
    }
  }

  if (newPos != L7_NULLPTR)
  {
    *newPos = position;
  }
  *matchOptions = argMatchType;
  return L7_SUCCESS;
}

