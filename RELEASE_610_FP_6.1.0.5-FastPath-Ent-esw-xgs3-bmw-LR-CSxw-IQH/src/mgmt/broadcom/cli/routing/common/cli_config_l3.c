/*********************************************************************
 * LL   VV  VV LL   7   77   All Rights Reserved.
 * LL   VV  VV LL      77
 * LL    VVVV  LL     77
 * LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
 * </pre>
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/common/cli_config_l3.c
 *
 * @purpose config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  03/15/2001
 *
 * @author  Diogenes DeLosSantos
 * @author  Samip Garg
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include <errno.h>

/* For internet addr translation routines */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cliapi.h"
#include "l7_common.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "usmdb_1213_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "l7_relay_api.h"
#include "util_enumstr.h"

#include "ews.h"

/* layer 3 includes           */
#include "osapi.h"
#include "nimapi.h"
#include "usmdb_ip6_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_rtrdisc_api.h"
#include "clicommands_l3.h"
#ifdef L7_IPV6_PACKAGE
#include "clicommands_ipv6.h"
#endif
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_mib_relay_api.h"
#include "l3_commdefs.h"
#include "clicommands_card.h"
#include  "cli_config_script.h"

L7_uint32 vrid_context = L7_VR_NO_VRID;

/*********************************************************************
*
* @purpose    Enables or disables IP spoofing checking
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  config ip spoofing
*
* @cmdhelp
*
* @cmddescript  When enabled, a packet is discarded if the router
*               interface on which it is received is not the interface
*               on which its source IP address is reachable.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandConfigIpSpoofing(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMode = 1;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* check argument validity and # of parameter */
  if ( cliNumFunctionArgsGet() != 1)     /* parameter check */
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpSpoofing);
  }

  if (strlen(argv[index+argMode]) >= sizeof(strMode) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpSpoofing);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  OSAPI_STRNCPY_SAFE(strMode,argv[index+ argMode]);
  cliConvertToLowerCase(strMode);
  if (strcmp(strMode, pStrInfo_common_Enbl_2) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbIpSpoofingSet(unit, L7_ENABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_routing_SetSpoofing);
      }
    }
  }
  else if (strcmp(strMode, pStrInfo_common_Dsbl2) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbIpSpoofingSet(unit, L7_DISABLE) != L7_SUCCESS )
      {

        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_routing_SetSpoofing);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpSpoofing);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Create routing of a VLAN
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
* @notes The value for Vlan ID is the range from 1 to L7_DOT1Q_MAX_VLAN_ID.
*
* @Mode  Vlan database Config
*
* @cmdsyntax for normal command: vlan routing <vlan> [<interface ID>]
* @cmdsyntax for no command: no vlan routing <vlan>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpVlanRouting(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argVlanID = 1;
  L7_uint32 argIntfId = 2;
  L7_uint32 vlanID;
  L7_uint32 intfId = 0;    /* 0 is an invalid interface ID. Indicates that the user did not specify */
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t rc;
  L7_uchar8 *noSyntax = "Use 'no vlan routing <%d-%d>'.";
#ifdef L7_IPV6_PACKAGE
  L7_uint32 intIfNum;
#endif

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

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg != 1) && (numArg != 2))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,  
                                             pStrErr_routing_CfgIpVlanRouting, 
                                             L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID,
                                             1, L7_MAX_NUM_VLAN_INTF);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1,1,0,0, pStrErr_common_IncorrectInput, ewsContext,
                                            noSyntax, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    }
  }

  if (( cliConvertTo32BitUnsignedInteger(argv[index+argVlanID], &vlanID) != L7_SUCCESS) || 
      (vlanID < L7_DOT1Q_MIN_VLAN_ID || vlanID > L7_DOT1Q_MAX_VLAN_ID))
  {
    return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidVlanIdSpecified);
  }

  if (numArg == 2)
  {
    if ((cliConvertTo32BitUnsignedInteger(argv[index + argIntfId], &intfId) != L7_SUCCESS) ||
        ((intfId < 1) || (intfId > L7_MAX_NUM_VLAN_INTF)))
    {
      return cliSyntaxReturnPromptAddBlanks(0, 1, 0, 0, pStrErr_common_Error,  ewsContext, 
                                            "Interface ID is out of range");
    }
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ((rc = usmDbIpVlanRoutingIntfCreate(unit, vlanID, intfId)) == L7_TABLE_IS_FULL)
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_routing_MaxNumOfRoutingVlansReached);
      }
      else if (rc == L7_REQUEST_DENIED)
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_routing_CantCfgureVlanRoutingIntf2);
      }
      else if (rc == L7_ADDR_INUSE)
      {
        /* VLAN taken for port based routing already */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, 
                                               "VLAN ID already in use for a port based routing interface");
      }
      else if (rc == L7_ALREADY_CONFIGURED)
      {
        /* Interface ID already in use */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                               "Interface ID %u is already assigned to another interface",
                                               intfId);
      }
      else if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CantCfgureVlanRoutingIntf);
      }
#ifdef L7_IPV6_PACKAGE
      if((rc = usmDbIpVlanRtrVlanIdToIntIfNum(unit, vlanID,&intIfNum)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CantCfgureVlanRoutingIntf);
      }
      if((rc = usmDbIp6RtrIntfModeSet(intIfNum, L7_ENABLE)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CantCfgureVlanRoutingIntf);
      }
#endif
    }
  }

  else if(  ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
#ifdef L7_IPV6_PACKAGE
      if((rc = usmDbIpVlanRtrVlanIdToIntIfNum(unit, vlanID,&intIfNum)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,pStrInfo_routing_UnableCfgIpVlanRoutingDel);
      }
      if((rc = usmDbIp6RtrIntfModeSet(intIfNum, L7_ENABLE)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,pStrInfo_routing_UnableCfgIpVlanRoutingDel);
      }
#endif
      if (usmDbIpVlanRoutingIntfDelete(unit, vlanID) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext,pStrInfo_routing_UnableCfgIpVlanRoutingDel);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure base VLAN ID for VLAN routing interfaces
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
* @notes The value for Vlan ID is the range from 1 to L7_DOT1Q_MAX_VLAN_ID.
*
* @Mode  global config
*
* @cmdsyntax for normal command: vlan internal allocation base <vlan-id>
* @cmdsyntax for no command: no vlan internal allocation base
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandVlanInternalAllocationBase(EwsContext ewsContext, L7_uint32 argc, 
                                                  const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argVlanID = 1;
  L7_uint32 vlanID;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,  
                                             "Use 'vlan internal allocation base <vlan-id>'.");
    }

    if ((cliConvertTo32BitUnsignedInteger(argv[index + argVlanID], &vlanID) != L7_SUCCESS)
        || (vlanID < L7_DOT1Q_MIN_VLAN_ID || vlanID > L7_DOT1Q_MAX_VLAN_ID))
    {
      return cliSyntaxReturnPromptAddBlanks(0, 1, 0, 0, pStrErr_common_Error, ewsContext, 
                                            pStrErr_common_InvalidVlanIdSpecified);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbBaseInternalVlanIdSet(vlanID) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, "Failed to set the base VLAN ID");
      }
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,  
                                             "Use 'no vlan internal allocation base'.");
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbBaseInternalVlanIdSet(FD_DOT1Q_BASE_INTERNAL_VLAN) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, "Failed to revert the base VLAN ID to its default");
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure whether internal VLAN IDs are allocated in ascending 
*           or descending order
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
* @Mode  global config
*
* @cmdsyntax for normal command: vlan internal allocation policy [ ascending | descending ]
* @cmdsyntax for no command: no vlan internal allocation policy
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandVlanInternalAllocationPolicy(EwsContext ewsContext, L7_uint32 argc, 
                                                    const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 argPolicy = 1;
  e_Internal_Vlan_Policy policy;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,  
                                             "Use 'vlan internal allocation policy [ ascending | descending ]'.");
    }

    if (strcmp(argv[index + argPolicy], "ascending") == 0)
      policy = DOT1Q_INT_VLAN_POLICY_ASCENDING;
    else if (strcmp(argv[index + argPolicy], "descending") == 0)
      policy = DOT1Q_INT_VLAN_POLICY_DESCENDING;
    else   
    {
      return cliSyntaxReturnPromptAddBlanks(0, 1, 0, 0, pStrErr_common_Error, ewsContext, 
                                            "Invalid VLAN allocation policy");
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbInternalVlanPolicySet(policy) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt(ewsContext, "Failed to set the allocation policy");
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,  
                                             "Use 'no vlan internal allocation policy'.");
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbInternalVlanPolicySet(FD_DOT1Q_INTERNAL_VLAN_POLICY) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, "Failed to revert the allocation policy to the default");
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the  interface bandwidth.
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
* @Mode  Interface Config
*
* @cmdsyntax for normal command: bandwidth<L7_L3_MIN_BW - L7_L3_MAX_BW>
*
* @cmdsyntax for no command: no bandwidth
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRtrIntfBandwidth(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argBwVal  = 1;
  L7_uint32 iface, s, p;
  L7_int32 bandwidth;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 numArg;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop (ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, pStrErr_routing_CfgIpIntf, L7_L3_MIN_BW, L7_L3_MAX_BW);
    return cliSyntaxReturnPrompt (ewsContext, stat);
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

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }
  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (( cliConvertTo32BitUnsignedInteger(argv[index+argBwVal], &bandwidth) == L7_SUCCESS)
        &&(( bandwidth >= L7_L3_MIN_BW ) && ( bandwidth <= L7_L3_MAX_BW )))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if(usmDbIfBandwidthSet(iface, bandwidth) == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_Set_3);
        }
      }
    }
    else
    {  /* failure */
      sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_routing_BandwidthIsOutOfRangeMustBeInRangeOfTo,
                        L7_L3_MIN_BW, L7_L3_MAX_BW);
      return cliSyntaxReturnPrompt (ewsContext, stat);
    }

  }

  else if(  ewsContext->commType == CLI_NO_CMD)
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIfBandwidthSet(iface, FD_IP_DEFAULT_BW ) == L7_FAILURE)
      {

        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_Set_3);
      }
    }

  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Sets whether net directed broadcasts are enabled or disabled.
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
* @Mode  Interface Config
*
* @cmdsyntax for normal command: ip netdirbcast
*
* @cmdsyntax for no command: no ip netdirbcast
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandNetDirectBcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if(!(numArg == 0 ))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpIntfNetDir);
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

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIpNetDirectBcastsSet(unit, iface, L7_ENABLE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_CouldNot,ewsContext,  pStrErr_routing_DirectedBcastsEnbl);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIpNetDirectBcastsSet(unit, iface, L7_DISABLE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_CouldNot,ewsContext,  pStrErr_routing_DirectedBcastsEnbl);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgIpIntfNetDir);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose
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
* @Mode  Global Config
*
* @cmdsyntax for normal command: ip address <slot/port> <ipaddr> <subnet-mask>
*
* @cmdsyntax for no command: no ip address <slot/port>
*
* @cmdhelp
*
* @cmddescript  check for valid IP addr and valid subnet mask.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpAddress(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 argSubnetMask = 2;
  L7_uint32 argSecondary = 3;
  L7_uint32 IPaddr;
  L7_uint32 subnetMask;
  L7_uint32 iface, s, p;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSubnetMask[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 intfType;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if ((numArg < 2) || (numArg > 3))
  {
    if (  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
          pStrErr_routing_CfgIpIntfNw);

      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    if(  ewsContext->commType == CLI_NO_CMD && numArg !=0)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
          pStrErr_routing_CfgIpNoIntfNw);

      return cliSyntaxReturnPrompt (ewsContext, "");
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

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if (cliSlotPortCpuCheck(s,p) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliPrompt(ewsContext);
  }
  if ((numArg == 2) || (numArg == 3))
  {
  /* check if valid IP addrs for both. IP addr and subnet mask*/
  memset (strIPaddr, 0,sizeof(strIPaddr));
  memset (strSubnetMask, 0,sizeof(strSubnetMask));
  if (strlen(argv[index+argIPaddr]) >= sizeof(strIPaddr) ||
      strlen(argv[index+argSubnetMask]) >= sizeof(strSubnetMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpIntfNw);
  }

  OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + argIPaddr]);
  OSAPI_STRNCPY_SAFE(strSubnetMask, argv[index + argSubnetMask]);

  if ( (usmDbInetAton(strIPaddr, &IPaddr) == L7_SUCCESS) &&
      (usmDbInetAton(strSubnetMask, &subnetMask) == L7_SUCCESS) )
  {
    if (  ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        /* Allow 31 or 32 bit netmask only on loopback interfaces */
        if ((usmDbIntfTypeGet(iface, &intfType) == L7_SUCCESS) &&
            (intfType == L7_LOOPBACK_INTF))
        {
          if (usmDbNetmaskValidate32(subnetMask) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSubnetMask32);
          }
        }
        else
        {
          if (usmDbNetmaskValidate(subnetMask) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSubnetMask);
          }
        }

        /* optional secondary IP address */
        if (numArg == 3 && strcmp (argv[index + argSecondary], pStrInfo_routing_Secondary_1) <= 0)
        {
          rc = usmDbIpRtrIntfSecondaryIpAddrAdd(unit, iface, IPaddr, subnetMask);
          if (rc != L7_SUCCESS)
          {
            switch (rc)
            {
              case L7_ERROR:
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                         pStrErr_common_CfgIpAddrConflict);
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                         pStrErr_common_IntfServiceAndNwPortCfg);
                break;
              case L7_NOT_SUPPORTED:
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,
                                         ewsContext, pStrErr_routing_CantSetIntfNw);
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                         pStrInfo_common_OnlyLogicalIntfsAreSupported);
                break;
              case L7_TABLE_IS_FULL:
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,
                                         ewsContext, pStrErr_routing_IntfTblFull);
                break;
              case L7_ALREADY_CONFIGURED:
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                         pStrErr_routing_IpAddrSecondarySameAsPrimary);
                break;
              case L7_REQUEST_DENIED:
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                         pStrErr_routing_IpAddrExistsAsNextHop);
                break;
              case L7_NOT_EXIST:
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                         pStrErr_routing_IpAddrSecondaryNoPrimary);
                break;
              default:
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                         pStrErr_routing_CantSetIntfNw);
                break;
            }
            return cliSyntaxReturnPrompt (ewsContext, "");
          }
        }
        else
        {
          rc = usmDbIpRtrIntfIPAddressSet(unit, iface,  IPaddr, subnetMask);
        }
        if (rc != L7_SUCCESS)
        {
          switch (rc)
          {
          case L7_ERROR:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgIpAddrConflict);
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_IntfServiceAndNwPortCfg);
            break;
          case L7_NOT_SUPPORTED:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_CantSetIntfNw);
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_OnlyLogicalIntfsAreSupported);
            break;
          case L7_TABLE_IS_FULL:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_IntfTblFull);
            break;
          case L7_ALREADY_CONFIGURED:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_IpAddrConflict);
            break;
          case L7_REQUEST_DENIED:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_IpAddrExistsAsNextHop);
            break;
         case L7_NOT_EXIST:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                    pStrErr_routing_IpAddressIntfUnnum);
            break;
         default:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_CantSetIntfNw);
          break;
         }
         return cliSyntaxReturnPrompt (ewsContext, "");
        }
      }
    }

    else if (  ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if ((numArg == 3) && (strcmp(argv[index + argSecondary], pStrInfo_routing_Secondary_1) <= 0))
        {
          rc = usmDbIpRtrIntfSecondaryIpAddrRemove(unit, iface, IPaddr, subnetMask);
        }
        else
        {
          rc = usmDbIpRtrIntfIPAddressRemove(unit, iface,  IPaddr, subnetMask);
        }
        if (rc != L7_SUCCESS)
        {
          if (rc == L7_FAILURE || rc == L7_ERROR)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantDelIpAddr);
          }
          cliSyntaxBottom(ewsContext);
        }
      }
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                             pStrErr_routing_CfgIpIntfNw);
    cliSyntaxBottom(ewsContext);
  }
  }

  else if ((numArg == 0) && (ewsContext->commType == CLI_NO_CMD))
  {

    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
    /* Removing all the addresses(primary+seconadary) configured on the interface */
     rc = usmDbIpRtrIntfIpAddressesRemove(unit, iface);
     if (rc != L7_SUCCESS)
     {
       ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantDelIpAddr);
     }
    }

  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose
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
* @notes  the range is
*
* @Mode  Global Config
*
* @cmdsyntax for normal command: arp timeout <seconds>
*
* @cmdsyntax for no command: no arp timeout
*
* @cmdhelp    configures the arp entry ageout time
*
* @cmddescript
* @L7_IP_ARP_AGE_TIME_DEF = 1200
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandArpTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSeconds = 1;
  L7_uint32 intSeconds;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg, dynamicRenew;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_CfgArpTimeout,
                            L7_IP_ARP_AGE_TIME_MIN, L7_IP_ARP_AGE_TIME_MAX);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argSeconds], &intSeconds) == L7_SUCCESS)
    {
      if ( ( intSeconds >= L7_IP_ARP_AGE_TIME_MIN ) && ( intSeconds <= L7_IP_ARP_AGE_TIME_MAX ) )
      {
        usmDbIpArpDynamicRenewGet(unit, &dynamicRenew);

        if((intSeconds < L7_IP_ARP_AGE_TIME_CHECK) &&
           (dynamicRenew == L7_DISABLE) && (cliIsStackingSupported() == L7_TRUE))
        {
          if((cliGetCharInputID() == CLI_INPUT_EMPTY) &&
              ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
          {
            cliSyntaxTop(ewsContext);
            cliSetCharInputID(1, ewsContext, argv);

            osapiSnprintf(stat, sizeof(stat), pStrInfo_routing_ArpTimeoutAltCmd, intSeconds);
            cliAlternateCommandSet(stat);

            return pStrInfo_routing_VerifyChngArpTimeoutLoadsCpu;
          }
          if((tolower(cliGetCharInput()) != 'y') &&
              ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
          {
            ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
            return cliSyntaxReturnPrompt(ewsContext, pStrInfo_routing_ArpTimeoutNotChngd);
          }
        }

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbIpArpAgeTimeSet(unit, intSeconds) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetAgeTimeout);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeArpAgeTimeValMustBeInRangeOfTo, L7_IP_ARP_AGE_TIME_MIN, L7_IP_ARP_AGE_TIME_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_CfgArpTimeout,
                              L7_IP_ARP_AGE_TIME_MIN, L7_IP_ARP_AGE_TIME_MAX);
    }
  }
  else if(  ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIpArpAgeTimeSet(unit, L7_IP_ARP_AGE_TIME_DEF) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetDefAgeTimeout);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Sets the maximum number of entries in the ARP cache
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
* @Mode  Global Config
*
* @cmdsyntax for normal command: arp cachesize <maxCacheSize>
*
* @cmdsyntax for no command: no arp cachesize
*
* @cmdhelp   configures the arp maximum number of entries in the cache.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandArpCacheSize(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argCacheSize = 1;
  L7_uint32 intCacheSize;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_CfgArpCacheSize_1,
                            L7_IP_ARP_CACHE_SIZE_MIN, L7_IP_ARP_CACHE_SIZE_MAX);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argCacheSize], &intCacheSize) == L7_SUCCESS)
    {
      if ( ( intCacheSize >= L7_IP_ARP_CACHE_SIZE_MIN ) && ( intCacheSize <= L7_IP_ARP_CACHE_SIZE_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbIpArpCacheSizeSet(unit, intCacheSize) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetCacheSize);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeArpCacheSizeValMustBeInRangeOfTo,
                                L7_IP_ARP_CACHE_SIZE_MIN, L7_IP_ARP_CACHE_SIZE_MAX);
      }
    }
    else /* else, not an integer incorrect syntax */
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_CfgArpCacheSize_1,
                              L7_IP_ARP_CACHE_SIZE_MIN, L7_IP_ARP_CACHE_SIZE_MAX);
    }
  }
  else if(  ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIpArpCacheSizeSet(unit, L7_IP_ARP_CACHE_SIZE_DEF) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetDefCacheSize);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Sets an ARP entry to be added to the ARP table
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
* @notes    An L7_ERROR if there is no routing interface registered for a subnet
*           to which this address belongs.
*
* @Mode  Global Config
*
* @cmdsyntax for normal command: arp <ipaddr> <macaddr>
*
* @cmdsyntax for no command: no arp <ipaddr>

* @cmdhelp  Configure an ARP entry to be added to the ARP table.
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandArp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 argMacAddr = 2;
  L7_uint32 intIPaddr = 0;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 intIfNum = L7_INVALID_INTF;
  L7_RC_t rc;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  if( !((argc == 3 && (ewsContext->commType == CLI_NORMAL_CMD)) ||
        (argc == 2 && (ewsContext->commType == CLI_NO_CMD))))
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, sizeof(stat),
                              pStrErr_routing_Arp_1, L7_IP_ARP_CACHE_SIZE_MIN, L7_IP_ARP_CACHE_SIZE_MAX,
                              L7_IP_ARP_RESP_TIME_MIN, L7_IP_ARP_RESP_TIME_MAX,
                              L7_IP_ARP_RETRIES_MIN, L7_IP_ARP_RETRIES_MAX,
                              L7_IP_ARP_AGE_TIME_MIN, L7_IP_ARP_AGE_TIME_MAX);
      ewsTelnetWrite( ewsContext, stat);
    }
    if(ewsContext->commType == CLI_NO_CMD)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, sizeof(stat),
                              pStrErr_routing_CfgNoArp);
      ewsTelnetWrite( ewsContext, stat);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  OSAPI_STRNCPY_SAFE(strIPaddr, argv[argIPaddr]);

  if (strlen(argv[argIPaddr]) >= sizeof(strIPaddr) || (usmDbInetAton(strIPaddr, &intIPaddr) != L7_SUCCESS))
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, sizeof(stat),
                              pStrErr_routing_Arp_1, L7_IP_ARP_CACHE_SIZE_MIN, L7_IP_ARP_CACHE_SIZE_MAX,
                              L7_IP_ARP_RESP_TIME_MIN, L7_IP_ARP_RESP_TIME_MAX,
                              L7_IP_ARP_RETRIES_MIN, L7_IP_ARP_RETRIES_MAX,
                              L7_IP_ARP_AGE_TIME_MIN, L7_IP_ARP_AGE_TIME_MAX);
      ewsTelnetWrite( ewsContext, stat);
    }
    if(ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,
                               pStrErr_routing_CfgNoArp);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (strlen(argv[argMacAddr]) >= sizeof(buf))
      {
      
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,
                                               pStrErr_routing_CfgArp);
      }

      OSAPI_STRNCPY_SAFE(buf, argv[argMacAddr]);

      memset ( strMacAddr, 0, sizeof(strMacAddr) );
      if ( cliConvertMac(buf, strMacAddr) == L7_TRUE )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ((rc = usmDbIpStaticArpAdd(unit, intIPaddr, intIfNum, strMacAddr))
              != L7_SUCCESS)
          {
            if (rc == L7_NOT_EXIST)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                       pStrErr_routing_StaticArpEntryNotLocal);
              cliSyntaxBottom(ewsContext);
              /* L7_NOT_EXIST indicates that the router does not have an
               * interface in a subnet that matches the target. This type
               * of preconfiguration is allowed. */
              /*************Set Flag for Script Successful******/
              ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
            }
            else
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext,
                                       pStrErr_routing_CantCreateArpEntry);
              cliSyntaxBottom(ewsContext);
            }
            return cliSyntaxReturnPrompt (ewsContext, "");
          }
        }
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr );
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,
                                               pStrErr_routing_CfgArp);
      }
    }
    else if(  ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ((rc = usmDbIpMapStaticArpDelete(unit, intIPaddr, intIfNum)) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext,
                                                 pStrErr_routing_CantDelArpEntry);
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
* @purpose    Sets the Arp request response timeout
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
* @notes  the range is
*
* @Mode  Global Config
*
* @cmdsyntax for normal command: arp resptime <seconds>
*
* @cmdsyntax for no command: no arp resptime
*
* @cmdhelp   configures the arp request response timeout.
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandArpRespTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSeconds = 1;
  L7_uint32 intSeconds;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_CfgArpRespTime_1,
                            L7_IP_ARP_RESP_TIME_MIN, L7_IP_ARP_RESP_TIME_MAX);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argSeconds], &intSeconds) == L7_SUCCESS)
    {
      if ( ( intSeconds >= L7_IP_ARP_RESP_TIME_MIN ) && ( intSeconds <= L7_IP_ARP_RESP_TIME_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbIpArpRespTimeSet(unit, intSeconds) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetRespTime);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeArpRespValMustBeInRangeOfTo,
                                L7_IP_ARP_RESP_TIME_MIN, L7_IP_ARP_RESP_TIME_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_CfgArpRespTime_1,
                              L7_IP_ARP_RESP_TIME_MIN, L7_IP_ARP_RESP_TIME_MAX);
    }
  }
  else if(  ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIpArpRespTimeSet(unit, L7_IP_ARP_RESP_TIME_DEF) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetDefRespTime);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Sets the Arp request max retries count
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
* @Mode  Global Config
*
* @cmdsyntax  config arp retries <retries value> Arp request max retries count
*
* @cmdsyntax for normal command: arp retries <1-10>
*
* @cmdsyntax for no command: no arp retries
*
* @cmdhelp    configures the arp count of maximum request for retries.
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandArpRetries(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argRetries = 1;
  L7_uint32 intRetries;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_CfgArpRetries_1,
                            L7_IP_ARP_RETRIES_MIN, L7_IP_ARP_RETRIES_MAX);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argRetries], &intRetries) == L7_SUCCESS)
    {
      if ( ( intRetries >= L7_IP_ARP_RETRIES_MIN ) && ( intRetries <= L7_IP_ARP_RETRIES_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbIpArpRetriesSet(unit, intRetries) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetRetries);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeArpRetriesValMustBeInRangeOfTo,
                                L7_IP_ARP_RETRIES_MIN, L7_IP_ARP_RETRIES_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_CfgArpRetries_1,
                              L7_IP_ARP_RETRIES_MIN, L7_IP_ARP_RETRIES_MAX);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIpArpRetriesSet(unit, L7_IP_ARP_RETRIES_DEF) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetDefRetries);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the link layer ecapsulation type for the packet.
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
* @Mode  Interface Config
*
* @cmdsyntax  encapsulation <Ethernet/SNAP>
*
* @No option is not valid for this command.
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandEncapsulation(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argEncapsType = 1;
  L7_uint32 iface, s, p;
  L7_char8 strEncapsType[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if( !(numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIntfEncaps_1);
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

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if (strlen(argv[index+argEncapsType]) >= sizeof(strEncapsType))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIntfEncaps_1);
  }
  OSAPI_STRNCPY_SAFE( strEncapsType, argv[index+ argEncapsType] );
  cliConvertToLowerCase(strEncapsType);
  if (strcmp(strEncapsType, pStrInfo_common_DhcpEtherNet) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIfEncapsulationTypeSet(iface, L7_ENCAP_ETHERNET) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetEncapsTo);
      }
    }
  }
  else if (strcmp(strEncapsType, pStrInfo_routing_Snap2) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbIfEncapsulationTypeSet(iface, L7_ENCAP_802) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetEncapsTosSnap);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIntfEncaps_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enables or disables routing for an interface.
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
* @Mode  Interface Config
*
* @cmdsyntax for normal command: routing
*
* @cmdsyntax for no command: no routing
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandInterfaceRouting(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 intMode;
  L7_uint32 intIface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if( !(numArg == 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIntfRouting);
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

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode =  L7_ENABLE;
  }
  else if(  ewsContext->commType == CLI_NO_CMD)
  {
    intMode =  L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIntfRouting);
  }

  unit = cliGetUnitId();
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbIpRtrIntfModeSet(unit, intIface, intMode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetIfRouting);
    }
  }

#ifdef L7_IPV6_PACKAGE
  if ( usmDbIp6RtrIntfModeSet(intIface, intMode) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSwitchIpv6);
  }
#endif

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Set the IP Router Admin Mode for the master switch
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
* @notes Routing is needed b/se the switch-wide one is a master switch.
*
* @Mode  Global Config
*
* @cmdsyntax for normal command: ip routing
*
* @cmdsyntax for no command: no ip routing
*
* @cmdhelp  Configures the IP Router Admin Mode for the master switch.
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpRouting(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 0 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRouting);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbIpRtrAdminModeSet(unit, L7_ENABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSwitch);
      }
    }

  }

  else if(  ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbIpRtrAdminModeSet(unit, L7_DISABLE)!= L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSwitch);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRouting);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose     Add a static route entry
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
* @notes    The next hop router IP address is generally used to reach the
*           route.
*
* @Mode  Global Config
*
* @cmdsyntax for normal command:  ip route <ip addr> <subnet mask> {<nextHopRtr> | Null0} [preference]
*
* @cmdsyntax for no command: several variations are allowed:
*
*                no ip route <ip addr> <mask> <preference>
*                  Reverts the preference of static routes to this destination
*                  to the default preference.
*
*                no ip route <ip addr> <mask> {<nextHopRtr> | Null0}
*                  Deletes an individual next hop for a specific destination.
*
*                no ip route <ip addr> <mask>
*                  Deletes all next hops for a specific destination.
*
* @cmdhelp
*
* @cmddescript    The value for [next hop] is a valid IP address of the
*                 next hop router.
*
* @end
*
*
*********************************************************************/
const L7_char8 *commandIpRoute(EwsContext ewsContext, L7_uint32 argc,
                               const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 tmpInd = 1;
  L7_uint32 argSubnetMask = 2;
  L7_uint32 argNextHopRtr = 3;
  L7_uint32 argPref = 4;
  L7_uint32 argNo3 = 3;
  L7_uint32 longSubnetMask, longIPaddr, longNextHopRtr = 0, longTemp;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSubnetMask[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strNextHopRtr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strArg3[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit, numArg, metric;
  L7_uint32 pref = 0;
  L7_BOOL revertPref = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum = L7_INVALID_INTF;
  L7_RT_ENTRY_FLAGS_t routeFlags = 0;

  cliSyntaxTop(ewsContext);

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg < 2 || numArg > 4))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                           pStrErr_common_CfgRtrRoute);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && (numArg < 1 || numArg > 3))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                           pStrErr_routing_CfgRtrNoRoute);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg == 3 && strcmp(argv[tmpInd+3], pStrInfo_common_Metric_1) == 0)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+4], &metric) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_InvalidMetricVal_1);
      }

      memset (strIPaddr, 0, sizeof(strIPaddr));
      memset (strSubnetMask, 0, sizeof(strSubnetMask));
      OSAPI_STRNCPY_SAFE(strIPaddr, argv[tmpInd + argIPaddr]);
      OSAPI_STRNCPY_SAFE(strSubnetMask, argv[tmpInd + argSubnetMask]);

      if ((usmDbInetAton(strIPaddr, &longIPaddr) == L7_SUCCESS) &&
          (usmDbInetAton(strSubnetMask, &longSubnetMask) == L7_SUCCESS))
      {
        if ((longSubnetMask != 0) && usmDbNetmaskIsContiguous(longSubnetMask) != L7_TRUE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NonContiguousMask);
        }
        if (usmDbIpStaticRouteAddrIsValid(unit, longIPaddr, longSubnetMask) != L7_TRUE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InvalidStaticRouteIp);
        }
        rc = rtoRouteMetricChange(longIPaddr, longSubnetMask, metric);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_CantSetMetric);
        }
        else
        {
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
      }
    }
    else
    {
      /* Valid forms of command are,
       * ip route addr mask nexthop
       * ip route addr mask nexthop pref */
      /* normal form of command always has next hop IP address. Get it. */
      if (strlen(argv[tmpInd+argNextHopRtr]) >= sizeof(strNextHopRtr))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                               pStrErr_common_CfgRtrRoute);
      }

      OSAPI_STRNCPY_SAFE(strNextHopRtr, argv[tmpInd + argNextHopRtr]);
      if (strcmp(strNextHopRtr, pStrInfo_common_IntfNull0) != 0)
      {
        if (usmDbInetAton(strNextHopRtr, &longNextHopRtr) != L7_SUCCESS ||
            usmDbNetworkAddressValidate(longNextHopRtr) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_CfgIpAddrErrorNextHop);
        }
      }
      else
      {
        /* Adding a static reject route (i.e., a static route with next-hop interface as "Null0") */
        routeFlags |= L7_RTF_REJECT;
      }
      if (routeFlags & L7_RTF_REJECT)
      {
        argPref = 4;  /* preference argument will be 4th argument while creating reject route */
      }
      if (numArg == 3)
      {
        /* parse route preference */
        if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+argPref], &pref) == L7_SUCCESS)
        {
          if ((pref < L7_RTO_PREFERENCE_MIN) || (pref > L7_RTO_PREFERENCE_MAX))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRoutePrefVal);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRoutePrefVal);
        }
      }
      else if (numArg == 2)
      {
        /* taking the default route preference for a static route */
        usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_STATIC, &pref);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                               pStrErr_common_CfgRtrRoute);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    OSAPI_STRNCPY_SAFE(strNextHopRtr, argv[tmpInd + argNextHopRtr]);
    if (strcmp(strNextHopRtr, pStrInfo_common_IntfNull0) != 0)
    {
      /* Valid forms of command are,
       * no ip route addr mask pref
       * no ip route addr mask nexthop */
        /* Get optional preference or next hop arg */
      if (numArg == 2)
      {
        OSAPI_STRNCPY_SAFE(strArg3, argv[tmpInd + argNo3]);
        if (strchr(strArg3, '.') == NULL)
        {
          /* Assume this arg is a preference value */
          if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+argNo3],
                                               &pref) == L7_SUCCESS)
          {
            revertPref = L7_TRUE;
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrNoRoute);
          }
        }
        else
        {
          /* Assume this arg is a next hop IP address */
          if (usmDbInetAton(strArg3, &longNextHopRtr) != L7_SUCCESS ||
              usmDbNetworkAddressValidate(longNextHopRtr) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_CfgIpAddrErrorNextHop);
          }
        }
      }
      else if (numArg == 1)
      {
        longNextHopRtr = 0;      /* indicates all next hops are to be deleted */
      }
    }
    else
    {
      routeFlags = L7_RTF_REJECT;
    }
  }

  /* Destination address and mask are command to all normal and no forms.
   * Get them. */
  if (strlen(argv[tmpInd+argIPaddr]) >= sizeof(strIPaddr) ||
      strlen(argv[tmpInd+argSubnetMask]) >= sizeof(strSubnetMask))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrRoute);
  }

  memset (strIPaddr, 0,sizeof(strIPaddr));
  memset (strSubnetMask, 0,sizeof(strSubnetMask));
  OSAPI_STRNCPY_SAFE(strIPaddr, argv[tmpInd + argIPaddr]);
  OSAPI_STRNCPY_SAFE(strSubnetMask, argv[tmpInd + argSubnetMask]);

  if ((usmDbInetAton(strIPaddr, &longIPaddr) == L7_SUCCESS) &&
      (usmDbInetAton(strSubnetMask, &longSubnetMask) == L7_SUCCESS))
  {
    if ((longSubnetMask != 0) && usmDbNetmaskIsContiguous(longSubnetMask) != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NonContiguousMask);
    }
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbIpStaticRouteAddrIsValid(unit, longIPaddr, longSubnetMask) != L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InvalidStaticRouteIp);
      }
      else if (usmDbIpAddrIsCfgRtrIntf(unit, longNextHopRtr, &longTemp) == L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InvalidNextHopRtrIp_1);
      }

      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbIpStaticRouteAdd(unit, longIPaddr, longSubnetMask,
                                   longNextHopRtr, pref, intIfNum, routeFlags);
      switch (rc)
      {
      case L7_SUCCESS:
        break;
      case L7_ERROR:
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_NextHopNumExceed);
        break;
      case L7_REQUEST_DENIED:
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InvalidNextHopRtrIp);
        break;
      case L7_TABLE_IS_FULL:
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_StaticRouteTblFull);
        break;
      case L7_NOT_EXIST:
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_CantSetStaticRoute);
        break;
      default:
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgIpv6RouteCfgErr);
        break;
      }
    }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (revertPref == L7_TRUE)
      {
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbIpStaticRoutePrefRevert(unit, longIPaddr,
                                           longSubnetMask, pref) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     pStrErr_common_StaticRoutePrefRevertFail);
            cliSyntaxBottom(ewsContext);
          }
        }
      }
      else
      {
        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbIpStaticRouteDelete(unit, longIPaddr, longSubnetMask,
                                       longNextHopRtr, intIfNum, routeFlags)  != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_StaticRouteDelFail);
            cliSyntaxBottom(ewsContext);
          }
        }
      }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                               pStrErr_common_CfgRtrRoute);
      cliSyntaxBottom(ewsContext);
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext,
                             pStrErr_routing_CfgVrrpIpAddrError);
    cliSyntaxBottom(ewsContext);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Add a default route entry.
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
* @Mode  Global Config
*
* @cmdsyntax for normal command:  ip route default <nexthopip>
*
* @cmdsyntax for no command: no ip route default
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpRouteDefault(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argNextHopRtr = 1;
  L7_uint32 argPref = 2;
  L7_uint32 longNextHopRtr = 0, longTemp;
  L7_char8 strNextHopRtr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strArg[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 pref;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum = L7_INVALID_INTF;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg < 1 || numArg > 2))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                           pStrErr_routing_CfgRtrRouteDefl);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                           pStrErr_routing_CfgRtrNoRouteDefl);
  }

  /* test if the nexthopip address is valid */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argNextHopRtr]) >= sizeof(strNextHopRtr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                             pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }
    OSAPI_STRNCPY_SAFE(strNextHopRtr, argv[index + argNextHopRtr]);
    if ((usmDbInetAton(strNextHopRtr, &longNextHopRtr) == L7_SUCCESS))
    {
      if (usmDbNetworkAddressValidate(longNextHopRtr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext,
                                               pStrErr_routing_CfgIpAddrErrorNextHop);
      }

      if (usmDbIpAddrIsCfgRtrIntf(unit, longNextHopRtr, &longTemp) == L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                               pStrErr_routing_InvalidNextHopRtrIp_1);
      }

      if (numArg == 2)
      {
        /* In normal mode of command, pref is 2nd parameter */
        if (cliConvertTo32BitUnsignedInteger (argv[index+argPref], &pref) == L7_SUCCESS)
        {
          if ((pref < L7_RTO_PREFERENCE_MIN) || (pref > L7_RTO_PREFERENCE_MAX))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                                   pStrErr_common_CfgRoutePrefVal);
        }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                                 pStrErr_common_CfgRoutePrefVal);
        }
      }
      else if (usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_STATIC, &pref) != L7_SUCCESS)
      {
        usmDbIpRouterPreferenceGet(unit, RTO_STATIC, &pref);
      }

      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbIpStaticRouteAdd(unit, 0, 0, longNextHopRtr,
                                   pref, intIfNum, L7_RTF_DEFAULT_ROUTE);
      switch (rc)
      {
      case L7_SUCCESS:
        break;
      case L7_ERROR:
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_NextHopNumExceed);
        break;
      case L7_REQUEST_DENIED:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     pStrErr_routing_InvalidNextHopRtrIp);
            break;
          case L7_TABLE_IS_FULL:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     pStrErr_routing_DeflRouteTblFull);
            break;
          case L7_NOT_EXIST:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     pStrErr_routing_CantSetDefRoute);
            break;
          default:
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                     pStrErr_routing_CantSetDeflRoute);
            break;
        }
      }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                               pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      cliSyntaxBottom(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg == 1)
    {
      memset (strArg, 0,sizeof(strArg));
      if (strlen(argv[index+1]) >= sizeof(strArg))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                               pStrErr_routing_CfgRtrNoRouteDefl);
      }
      OSAPI_STRNCPY_SAFE(strArg, argv[index + 1]);

      if (strchr(strArg, '.') == NULL)
      {

        if ((cliConvertTo32BitUnsignedInteger (argv[index+1], &pref) == L7_SUCCESS) &&
            ((pref >= L7_RTO_PREFERENCE_MIN) && (pref <= L7_RTO_PREFERENCE_MAX)))
        {
          if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if (usmDbIpStaticRoutePrefRevert(unit, 0, 0, pref) != L7_SUCCESS)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                       pStrErr_common_StaticRoutePrefRevertFail);
              cliSyntaxBottom(ewsContext);
            }
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                                 pStrErr_routing_CfgRtrNoRouteDefl);
        }
      }

      /* Assume this arg IP address */
      else
      {
        if (usmDbInetAton(strArg, &longNextHopRtr) != L7_SUCCESS ||
            usmDbNetworkAddressValidate(longNextHopRtr) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext,
                                                 pStrErr_routing_CfgIpAddrErrorNextHop);
        }

        if (usmDbIpAddrIsCfgRtrIntf(unit, longNextHopRtr, &longTemp) == L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                                 pStrErr_routing_InvalidNextHopRtrIp_1);
        }

        if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbIpStaticRouteDelete(unit, 0, 0, longNextHopRtr,
                                       L7_INVALID_INTF, 0)  != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                     pStrErr_common_StaticRouteDelFail);
            cliSyntaxBottom(ewsContext);
          }
        }
      }
    }
    /* numArg == 0 */
    else
    {
      if (usmDbIpStaticRouteDelete(unit, 0, 0, 0, L7_INVALID_INTF, 0) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                 pStrErr_common_StaticRouteDelFail);
        cliSyntaxBottom(ewsContext);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configures the preferences of Static Routes
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
* @Mode  Global Config
*
* @cmdsyntax for normal command:  ip route distance <1-255>
*
* @cmdsyntax for no command: no ip route distance
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpRouteDistance(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argPref = 1;
  L7_uint32 intPref;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRoutePref);
    }
    if( ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrNoRoutePref);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argPref], &intPref) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrRoutePref);
    }

    if (intPref > L7_RTO_PREFERENCE_MAX || intPref < L7_RTO_PREFERENCE_MIN)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrRoutePref);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ((rc = usmDbIpRouterPreferenceSet(unit, ROUTE_PREF_STATIC, intPref)) != L7_SUCCESS)
        {
          if (rc == L7_ALREADY_CONFIGURED)
          {
            ewsTelnetPrintf (ewsContext, "\r\n%s %s\r\n%s", pStrErr_common_CouldNot,
                          pStrErr_common_CantSetPref, pStrInfo_common_PrefValAlreadyInUse);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetPref);
          }
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbIpRouterPreferenceSet(unit, ROUTE_PREF_STATIC, L7_RTO_STATIC_PREFERENCE_DEF) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefPref);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enables or disables Router Discovery on an interface
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
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip irdp [ { holdtime <max-interval-9000> |
*             maxadvertinterval <4-1800> | minadvertinterval <3-maxinterval> | preference
*             <-2147483648 - 2147483647> | address <address> [ <number> ] } ]
*
* @cmdsyntax for no command: no ip irdp [{holdtime | maxadvertinterval |
*                                        minadvertinterval | preference | address}]
*
* @cmdhelp
*
* @cmddescript  default value is enable
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpIrdp(EwsContext ewsContext, L7_uint32 argc,
                              const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMode = 2;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 iface, s, p;
  L7_BOOL flag;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag!=L7_EXECUTE_SCRIPT)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

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

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if( argc == 2)
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      flag = L7_TRUE;
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ( usmDbRtrDiscAdvertiseSet(unit, iface, flag) != L7_SUCCESS )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetRtrDiscOnIf);
        }
      }
    }
    else if( ewsContext->commType == CLI_NO_CMD)
    {
      flag = L7_FALSE;

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ( usmDbRtrDiscAdvertiseSet(unit, iface, flag) != L7_SUCCESS )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetRtrDiscOnIf);
        }
      }

    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrIrdp);
    }
  }
  else
  {
    if (strlen(argv[argMode]) >= sizeof(strMode))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrIrdp);
    }

    OSAPI_STRNCPY_SAFE(strMode,argv[argMode]);
    cliConvertToLowerCase(strMode);

    if (strcmp(strMode, pStrInfo_routing_MaxAdvertIntvl) == 0)
    {
      IrdpHandleMaxadvertinterval(ewsContext, argc, argv, iface);
    }

    else if (strcmp(strMode, pStrInfo_routing_MinAdvertIntvl) == 0)
    {
      IrdpHandleMinadvertinterval(ewsContext, argc, argv, iface);
    }

    else if (strcmp(strMode, pStrInfo_common_HoldTime_1) == 0)
    {
      IrdpHandleHoldTime(ewsContext, argc, argv, iface);
    }

    else if (strcmp(strMode, pStrInfo_common_Pref_1) == 0)
    {
      IrdpHandlePreference(ewsContext, argc, argv, iface);
    }

    else if (strcmp(strMode, pStrInfo_common_Addr_4) == 0)
    {
      IrdpHandleAddress(ewsContext, argc, argv, iface);
    }

  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configures Maximum Hop Count for BOOTP/DHCP relay
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
* @Mode  Global Config
*
* @cmdsyntax for normal command:  bootpdhcprelay maxhopcount <1-16>
*
* @cmdsyntax for no command: no bootpdhcprelay maxhopcount
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandBootpDhcpRelayMaxHopCount (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMaxHopCount = 1;
  L7_char8 strMaxHopCount[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 maxHopCount;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelayMaxHopCount_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelayNoMaxHopCount);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argMaxHopCount]) >= sizeof(strMaxHopCount))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InvalidHopCount);
    }
    OSAPI_STRNCPY_SAFE(strMaxHopCount,argv[index + argMaxHopCount]);

    if (( cliConvertTo32BitUnsignedInteger(strMaxHopCount, &maxHopCount) != L7_SUCCESS)
        || (maxHopCount < L7_DHCP_MIN_HOP_COUNT || maxHopCount > L7_DHCP_MAX_HOP_COUNT))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InvalidHopCount);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbBootpDhcpRelayMaxHopCountSet(unit, maxHopCount) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetHopCount);
      }
    }
  }

  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbBootpDhcpRelayMaxHopCountSet(unit, FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_MAXHOPCOUNT) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetDefHopCount);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures Minimum Wait  Time for BOOTP/DHCP relay
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
* @Mode  Global Config
*
* @cmdsyntax for normal command:  bootpdhcprelay minwaittime <0-100>
*
* @cmdsyntax for no command: no bootpdhcprelay minwaittime
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandBootpDhcpRelayMinWaitTime (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMinWaitTime = 1;
  L7_uint32 minWaitTime;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelayMinWaitTime_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelayNoMinWaitTime);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((cliConvertTo32BitUnsignedInteger(argv[index+argMinWaitTime], &minWaitTime) != L7_SUCCESS) || (minWaitTime < L7_DHCP_MIN_WAIT_TIME || minWaitTime > L7_DHCP_MAX_WAIT_TIME) )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_InvalidMinWaitTime);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbBootpDhcpRelayMinWaitTimeSet(unit, minWaitTime) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetMinWaitTime);
      }
    }
  }

  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbBootpDhcpRelayMinWaitTimeSet(unit, FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_MINWAITTIME) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetDefMinWaitTime);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures Adminmode Mode for BOOTP/DHCP relay
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
* @Mode  Global Config
*
* @cmdsyntax for normal command:  bootpdhcprelay enable
*
* @cmdsyntax for no command: no bootpdhcprelay enable
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandBootpDhcpRelayEnable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( !(( cliNumFunctionArgsGet() == 0) && ((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelayAdminMode);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( usmDbIpHelperAdminModeSet(L7_ENABLE) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetAdminMode);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbIpHelperAdminModeSet(L7_DISABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetAdminMode);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelayAdminMode);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures Server/Next Relay agent IPaddress for BOOTP/DHCP relay
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
* @notes This command is deprecated but is retained for migration.
*
* @Mode  Global Config
*
* @cmdsyntax for normal command:  bootpdhcprelay serverip <ipaddr>
*
* @cmdsyntax for no command: no bootpdhcprelay serverip
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandBootpDhcpRelayServerIp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 intIPaddr = 0;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelaySrvrIp_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelayNoSrvrIp);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argIPaddr]) >= sizeof(strIPaddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelaySrvrIp_1);
    }
    OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + argIPaddr]);

    if ( usmDbInetAton(strIPaddr, &intIPaddr) == L7_SUCCESS )
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if( usmDbIpHelperAddressAdd(IH_INTF_ANY, UDP_PORT_BOOTP_SERV, intIPaddr) != L7_SUCCESS)
        {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, "This command is deprecated!\r\n",  
                                                ewsContext, 
                                                pStrErr_routing_useHelperIpAddr);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelaySrvrIp_1);
    }
  }

  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbIpHelperAddressDelete(IH_INTF_ANY, UDP_PORT_BOOTP_SERV, intIPaddr) != L7_SUCCESS )
      {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, "This command is deprecated!\r\n",
                                              ewsContext,
                                              pStrErr_routing_useHelperIpAddr);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelaySrvrIp_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures Circuit ID Option Mode for BOOTP/DHCP relay
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
* @Mode  Global Config
*
* @cmdsyntax for normal command:  bootpdhcprelay cidoptmode
*
* @cmdsyntax for no command: no bootpdhcprelay cidoptmode
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandBootpDhcpRelayCIdOptMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( !(( cliNumFunctionArgsGet() == 0) && ((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelayCircuitIdOptionMode_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbBootpDhcpRelayCircuitIdOptionModeSet(unit, L7_ENABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetCidOptMode);
      }
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_SetCidOptMode);
      cliSyntaxBottom(ewsContext);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbBootpDhcpRelayCircuitIdOptionModeSet(unit, L7_DISABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetCidOptMode);
      }
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_UnSetCidOptMode);
      cliSyntaxBottom(ewsContext);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRelayCircuitIdOptionMode_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enables or Disables proxy ARP on the router
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
* @Mode  Interface Config
*
* @cmdsyntax for normal command: ip proxy-arp
*
* @cmdsyntax for no command: no ip proxy-arp
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandProxyArp(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if(!(numArg == 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpProxyArp);
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

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( usmDbProxyArpSet(unit, iface, L7_ENABLE) != L7_SUCCESS )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_SetProxyArp);
      cliSyntaxBottom(ewsContext);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if ( usmDbProxyArpSet(unit, iface, L7_DISABLE) != L7_SUCCESS )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_SetProxyArpNo);
      cliSyntaxBottom(ewsContext);
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_SetProxyArp);
    cliSyntaxBottom(ewsContext);
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enables or Disables local proxy ARP on the router
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
* @Mode  Interface Config
*
* @cmdsyntax for normal command: ip loacl-proxy-arp
*
* @cmdsyntax for no command: no ip local-proxy-arp
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandLocalProxyArp(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (!(numArg == 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpLocalProxyArp);
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

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if (  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( usmDbLocalProxyArpSet(unit, iface, L7_ENABLE) != L7_SUCCESS )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_SetLocalProxyArp);
      cliSyntaxBottom(ewsContext);
    }
  }
  else if ( ewsContext->commType == CLI_NO_CMD)
  {
    if ( usmDbLocalProxyArpSet(unit, iface, L7_DISABLE) != L7_SUCCESS )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_SetLocalProxyArpNo );
      cliSyntaxBottom(ewsContext);
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_SetLocalProxyArp);
    cliSyntaxBottom(ewsContext);
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine to handle Max Advertisment Interval command for IRDP
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype cliPrompt(ewsContext)
*
* @notes Called by the commandIpIrdp function.
*
*
* @end
*
*********************************************************************/

const L7_char8 *IrdpHandleMaxadvertinterval( EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 iface)
{

  L7_uint32 paramMode = 3;
  L7_uint32 unit;
  L7_uint32 intSeconds, min, max, holdtime;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (argc != 4)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrDiscMaxIntvl_1);
    }
    if ( cliConvertTo32BitUnsignedInteger(argv[paramMode], &intSeconds) == L7_SUCCESS)
    {
      usmDbRtrDiscMinAdvIntervalGet(unit, iface, &min);
      if (min < L7_RTR_DISC_MAXINTERVAL_MIN)
      {
        min = L7_RTR_DISC_MAXINTERVAL_MIN;
      }
      usmDbRtrDiscAdvLifetimeGet(unit, iface, &holdtime);
      max = L7_RTR_DISC_MAXINTERVAL_MAX;
      if (holdtime < max)
      {
        max = holdtime;
      }
      if ( ( intSeconds >= min ) && ( intSeconds <= max ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbRtrDiscMaxAdvIntervalSet(unit, iface, intSeconds) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetMaxIntvl);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_CantSetRtrDiscOnIfOutOfRange, min, max);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrDiscMaxIntvl_1);
    }

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (argc != 3)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrNoDiscMaxIntvl);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRtrDiscMaxAdvIntervalSet(unit, iface, L7_RTR_DISC_MAXINTERVAL_DEF) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetDefMaxIntvl);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine to handle Min Advertisment Interval command for IRDP
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype cliPrompt(ewsContext)
*
* @notes Called by the commandIpIrdp function.
*
*
* @end
*
*********************************************************************/
const L7_char8 *IrdpHandleMinadvertinterval( EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 iface)
{
  L7_uint32 paramMode = 3;
  L7_uint32 time, unit;
  L7_RC_t rc;
  L7_uint32 intSeconds;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  rc = usmDbRtrDiscMaxAdvIntervalGet(unit, iface, &time);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (argc != 4)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrDiscMinIntvl_1);
    }
    if(( cliConvertTo32BitUnsignedInteger(argv[paramMode], &intSeconds) == L7_SUCCESS)
       && ( ( intSeconds >= L7_RTR_DISC_MININTERVAL_MIN ) && ( intSeconds <= time ) ))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRtrDiscMinAdvIntervalSet(unit, iface, intSeconds) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetMinIntvl);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeMinIntvlTimeValMustBeInRangeOfTo, L7_RTR_DISC_MININTERVAL_MIN, time);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (argc != 3)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrNoDiscMinIntvl);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRtrDiscMinAdvIntervalSet(unit, iface, L7_RTR_DISC_MININTERVAL_DEF(time)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetDefMinIntvl);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine to handle Hold Time command for IRDP
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype cliPrompt(ewsContext)
*
* @notes Called by the commandIpIrdp function.
*
*
* @end
*
*********************************************************************/
const L7_char8 *IrdpHandleHoldTime( EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 iface)
{
  L7_uint32 paramMode = 3;
  L7_uint32 time, unit;
  L7_RC_t rc;
  L7_uint32 intSeconds;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  rc =  usmDbRtrDiscMaxAdvIntervalGet(unit, iface, &time);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (argc != 4)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrDiscHoldTime);
    }
    if (( cliConvertTo32BitUnsignedInteger(argv[paramMode], &intSeconds) == L7_SUCCESS)
        && ( ( intSeconds >= time ) && ( intSeconds <= L7_RTR_DISC_LIFETIME_MAX ) ))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if(usmDbRtrDiscAdvLifetimeSet(unit, iface, intSeconds) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetHoldTime);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeHoldtimeValMustBeInRangeOfTo, time, L7_RTR_DISC_LIFETIME_MAX);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (argc != 3)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrNoDiscHoldTime);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRtrDiscAdvLifetimeSet(unit, iface, L7_RTR_DISC_LIFETIME_DEF(time)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetDefHoldTime);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine to handle Preference command for IRDP
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype cliPrompt(ewsContext)
*
* @notes Called by the commandIpIrdp function.
*
*
* @end
*
*********************************************************************/
const L7_char8 *IrdpHandlePreference( EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 iface)
{
  L7_uint32 paramMode = 3;
  L7_int32 intPreference;
  L7_char8 strPreference[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (argc != 4)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrDiscPref_1);
    }
    OSAPI_STRNCPY_SAFE(strPreference, argv[paramMode]);
    if (( cliCheckIfInteger(strPreference) == L7_SUCCESS) && (cliConvertTo32BitSignedInteger (strPreference, &intPreference) == L7_SUCCESS))
    {
      if (errno == ERANGE)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangePrefValMustBeInRangeOfTo, L7_RTR_DISC_PREFERENCE_MIN, L7_RTR_DISC_PREFERENCE_MAX);
      }

      if ( ( intPreference >= (L7_int32)L7_RTR_DISC_PREFERENCE_MIN ) && ( intPreference <= (L7_int32)L7_RTR_DISC_PREFERENCE_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbRtrDiscPreferenceLevelSet(unit, iface, intPreference) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_common_CantSetPref);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangePrefValMustBeInRangeOfTo, L7_RTR_DISC_PREFERENCE_MIN, L7_RTR_DISC_PREFERENCE_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrDiscPref_1);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (argc != 3)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrNoDiscPref);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRtrDiscPreferenceLevelSet(unit, iface, L7_RTR_DISC_PREFERENCE_DEF) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_common_CantSetDefPref);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine to handle address command for IRDP
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype cliPrompt(ewsContext)
*
* @notes Called by the commandIpIrdp function.
*
*
* @end
*
*********************************************************************/
const L7_char8 *IrdpHandleAddress( EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 iface)
{
  L7_uint32 paramMode = 3;
  L7_uint32 IPaddr =  CLIDEFAULTIPADDRESS;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (argc != 4)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrDiscAddr_1);
    }
    if (strlen(argv[paramMode]) >= sizeof(strIPaddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    }
    OSAPI_STRNCPY_SAFE(strIPaddr, argv[paramMode]);

    if ( (cliStrCaseCmp(strIPaddr,pStrInfo_routing_IpIrdpAddr,strlen(pStrInfo_routing_IpIrdpAddr)) != L7_SUCCESS) &&
        (cliStrCaseCmp(strIPaddr,pStrInfo_common_255255255255,strlen(pStrInfo_common_255255255255)) != L7_SUCCESS) )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_IncorrectIpAddr);
    }

    if (usmDbInetAton(strIPaddr, &IPaddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrDiscAddr_1);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRtrDiscAdvAddrSet(unit, iface, IPaddr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetAdd);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (argc != 3)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrNoDiscAddr);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRtrDiscAdvAddrSet(unit, iface, IPaddr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetDefAdd);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Clears the dynamic ARP entries from the ARP Cache
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
* @Mode  Privileged EXEC mode
*
* @cmdsyntax for normal command: clear arp-cache [gateway]
*
* @cmdsyntax for no command: NA
*
* @cmdhelp
*
* @cmddescript If the optional [gateway] parameter is specified,
*           the dynamic entries of type gateway are cleared as well.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearArpCache(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_BOOL gateway = L7_FALSE;
  L7_uint32 argGateway = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ClrArpCache_1);
  }
  unit = cliGetUnitId();

  if (strcmp(argv[index+argGateway],pStrInfo_common_Gateway_2) == 0)
  {
    gateway = L7_TRUE;
  }
  else
  {
    gateway = L7_FALSE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbIpArpCacheClear(unit, gateway) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_ClrArpCacheError);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose  Clears the IP Stack's ARP entries from the ARP Cache
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
* @Mode  Privileged EXEC mode
*
* @cmdsyntax for normal command: clear arp-switch
*
* @cmdsyntax for no command: NA
*
* @cmdhelp
*
*
* @end
*
*********************************************************************/
const L7_char8  *commandClearArpSwitch(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ClrArpSwitch);
    ;
  }
  unit = cliGetUnitId();

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  if (usmDbIpArpSwitchClear(unit) != L7_SUCCESS)
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_ClrArpCacheError);
  }
 /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enables or Disables dynamic renew of the ARP entries
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
* @Mode  Privileged EXEC mode
*
* @cmdsyntax for normal command: arp dynamicrenew
*
* @cmdsyntax for no command: no arp dynamicrenew
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandArpDynamicRenew(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, mode=L7_DISABLE, arpTimeout;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ArpDynRenew_1);
  }
  unit = cliGetUnitId();

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;

    usmDbIpArpAgeTimeGet(unit, &arpTimeout);

    if((arpTimeout < L7_IP_ARP_AGE_TIME_CHECK) && (cliIsStackingSupported() == L7_TRUE))
    {
      if((cliGetCharInputID() == CLI_INPUT_EMPTY) &&
         ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
      {
        cliSyntaxTop(ewsContext);
        cliSetCharInputID(1, ewsContext, argv);

        osapiSnprintf(buf, sizeof(buf), pStrInfo_routing_NoArpDynamicRenewAltCmd);
        cliAlternateCommandSet(buf);

        return pStrInfo_routing_VerifyChngArpTimeoutLoadsCpu;
      }
      if((tolower(cliGetCharInput()) != 'y') &&
         ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt(ewsContext, pStrInfo_routing_ArpDynamicRenewNotDisabled);
      }
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbIpArpDynamicRenewSet(unit, mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_CfgDynArpRenewError);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Removes the specified IP Address from the ARP Cache
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
* @Mode  Privileged EXEC mode
*
* @cmdsyntax for normal command: arp purge <ipaddr>
*
* @cmdsyntax for no command: no arp purge <ipaddr>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandArpPurge(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_IP_ADDR_t ipAddr;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argIpAddr = 1;
  L7_arpEntry_t pArp;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                           pStrErr_routing_ArpPurge_1);
  }
  unit = cliGetUnitId();

  OSAPI_STRNCPY_SAFE(strIPaddr,argv[index+argIpAddr]);
  if (usmDbInetAton(strIPaddr, &ipAddr)  != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext,
                                           pStrErr_routing_IpConvertionError);
  }

  /* Check whether the specified address coresponds to a Dynamic or Gateway ARP Entry or not */
  if (usmDbIpArpEntryGet(unit, ipAddr, L7_INVALID_INTF, &pArp) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext,
                                           pStrErr_routing_IpAddrRemovalFromArpCache);
  }
  if (((pArp.flags & L7_ARP_LOCAL)==0x01) || ((pArp.flags & L7_ARP_STATIC)==0x20))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                           pStrErr_routing_ArpPurgeIpAddr);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbIpArpEntryPurge(unit, ipAddr, L7_INVALID_INTF) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                                             ewsContext, pStrErr_routing_IpAddrRemovalFromArpCache);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the MTU size for IP protocol.
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
* @Mode  Interface Config
*
* @cmdsyntax for normal command: ip mtu <0-9000>
*
* @cmdsyntax for no command: no ip mtu
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpMtu(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIpMtuSize  = 1;
  L7_uint32 iface, s, p;
  L7_int32 ipMtuSize;

  L7_uint32 numArg;
  L7_uint32 unit;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop (ewsContext);

  numArg = cliNumFunctionArgsGet();

  if((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpMtu);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgNoIpMtu);
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

  if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    L7_uint32 maxIpMtu;
    if (usmDbIntfMaxIpMtuGet(iface, &maxIpMtu) != L7_SUCCESS)
    {
      maxIpMtu = L7_L3_MAX_IP_MTU;
    }
    if (( cliConvertTo32BitUnsignedInteger(argv[index+argIpMtuSize], &ipMtuSize) == L7_SUCCESS) &&
        (( ipMtuSize >= L7_L3_MIN_IP_MTU ) && ( ipMtuSize <= maxIpMtu )))
    {
      L7_RC_t rc = usmDbIntfIpMtuSet(unit, iface, ipMtuSize);
      if (rc == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, ewsContext,
                                               pStrErr_common_CfgMaxFrameSize);
      }
      else if (rc == L7_ERROR)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,
                                               pStrErr_routing_IpMtuOutOfRange,
                                               L7_L3_MIN_IP_MTU, maxIpMtu);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,
                                             pStrErr_routing_IpMtuOutOfRange,
                                             L7_L3_MIN_IP_MTU, maxIpMtu);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (usmDbIntfIpMtuSet(unit, iface, FD_IP_DEFAULT_IP_MTU) == L7_FAILURE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CfgMaxFrameSize);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_SetIpAddr);
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets sending of ICMP Unreachables mode.
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
* @Mode  Interface Config
*
* @cmdsyntax for normal command: ip unreachables
*
* @cmdsyntax for no command: no ip unreachables
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpUnreachables(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 numArg;
  L7_uint32 unit;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop (ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpUnreachables);
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgNoIpUnreachables);
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

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbIpMapICMPUnreachablesModeSet(unit, iface, L7_ENABLE) != L7_SUCCESS) 
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetIpUnreachables);          }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (usmDbIpMapICMPUnreachablesModeSet(unit, iface, L7_DISABLE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetIpUnreachables);          }
    }
  }
  
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets sending of ICMP Redirects mode.
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
* @Mode  Interface Config
*
* @cmdsyntax for normal command: ip redirects
*
* @cmdsyntax for no command: no ip redirects
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpRedirects(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 numArg;
  L7_uint32 unit;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop (ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpRedirects);
    }
   else if(ewsContext->commType == CLI_NO_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgNoIpRedirects);
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

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }

  if (cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbIpMapIfICMPRedirectsModeSet(unit, iface, L7_ENABLE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetIpRedirects);             }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (usmDbIpMapIfICMPRedirectsModeSet(unit, iface, L7_DISABLE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetIpRedirects);             }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets sending of ICMP Redirects mode.
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
* @Mode  Global Config
*
* @cmdsyntax for normal command: ip redirects
*
* @cmdsyntax for no command: no ip redirects
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpGlobalRedirects(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop (ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpRedirects);
    }
     else if(ewsContext->commType == CLI_NO_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgNoIpRedirects);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbIpMapRtrICMPRedirectsModeSet( L7_ENABLE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetIpRedirects);             }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (usmDbIpMapRtrICMPRedirectsModeSet( L7_DISABLE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetIpRedirects);             }
    }

   }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}



/*********************************************************************
*
* @purpose  Sets sending of ICMP Echo Reply mode.
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
* @Mode  Global Config
*
* @cmdsyntax for normal command: ip icmp echo-reply
*
* @cmdsyntax for no command: no ip icmp echo-reply
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpIcmpEchoReply(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop (ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpIcmpEchoReply);
    }
   else if(ewsContext->commType == CLI_NO_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgNoIpIcmpEchoReply);
    }
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbIpMapRtrICMPEchoReplyModeSet(L7_ENABLE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetIpIcmpEchoReply);             }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (usmDbIpMapRtrICMPEchoReplyModeSet(L7_DISABLE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetIpIcmpEchoReply);         }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets ICMP Rate Limiting Parameters
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
* @Mode  Global Config
*
* @cmdsyntax for normal command: ip icmp error-interval <0-2147483647> [<1-200>]
*
* @cmdsyntax for no command: no ip icmp error-interval
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpIcmpErrorInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg, error_interval, burstsize;
  
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop (ewsContext);

  numArg = cliNumFunctionArgsGet();

  error_interval = FD_RTR_DEFAULT_ICMP_RATE_LIMIT_INTERVAL;
  burstsize = FD_RTR_DEFAULT_ICMP_RATE_LIMIT_SIZE;
  
  if((ewsContext->commType == CLI_NORMAL_CMD) && ((numArg == 0) || (numArg > 2)))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgIpIcmpErrorInterval);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && (numArg != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgNoIpIcmpErrorInterval);
  }
 
  if (numArg >0) 
  {
     if( usmDbIpMapRtrICMPRatelimitGet(&burstsize, &error_interval) != L7_SUCCESS)
     {
       error_interval = FD_RTR_DEFAULT_ICMP_RATE_LIMIT_INTERVAL;
       burstsize = FD_RTR_DEFAULT_ICMP_RATE_LIMIT_SIZE; 
     }
     if(( cliConvertTo32BitUnsignedInteger(argv[index+1], &error_interval) == L7_SUCCESS) &&
        (( error_interval >= L7_L3_ICMP_RATE_LIMIT_MIN_INTERVAL ) &&
         ( error_interval <= L7_L3_ICMP_RATE_LIMIT_MAX_INTERVAL)))
     {
       if (numArg == 2) 
       {
         if((cliConvertTo32BitUnsignedInteger(argv[index+2], &burstsize) != L7_SUCCESS)  ||
           (( burstsize < L7_L3_ICMP_RATE_LIMIT_MIN_BURST_SIZE ) ||
           ( burstsize > L7_L3_ICMP_RATE_LIMIT_MAX_BURST_SIZE)))
         {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,
                                               pStrErr_routing_IpIcmpBucketSize,
                                               L7_L3_ICMP_RATE_LIMIT_MIN_BURST_SIZE,
                                               L7_L3_ICMP_RATE_LIMIT_MAX_BURST_SIZE);
         }
       }
       
     }
     else
     {
         return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,
                                               pStrErr_routing_IpIcmpErrorIntervalOutOfRange,
                                               L7_L3_ICMP_RATE_LIMIT_MIN_INTERVAL,
                                               L7_L3_ICMP_RATE_LIMIT_MAX_INTERVAL);
     }
   
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
      if (usmDbIpMapRtrICMPRatelimitSet(burstsize, error_interval) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CantSetIpIcmpErrorInterval);        
      }
  }


  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose  Configures Admin mode IP helper
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Global Config
*
* @cmdsyntax for normal command:  ip helper enable
*
* @cmdsyntax for no command: no ip helper enable
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpHelperEnable(EwsContext ewsContext, L7_uint32 argc, 
                                      const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, "\r\nIncorrect input! Use 'no bootpdhcprelay enable'.");
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if ((ewsContext->commType != CLI_NORMAL_CMD) ||
           (cliNumFunctionArgsGet() != 0))
  {
    ewsTelnetWrite(ewsContext, "\r\nIncorrect input! Use 'bootpdhcprelay enable'.");
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbIpHelperAdminModeSet(L7_ENABLE) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, "\r\nCould not set Administration Mode.");
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (usmDbIpHelperAdminModeSet(L7_DISABLE) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, "\r\nCould not set Administration Mode.");
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Add a global IP helper entry.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip helper-address [ server-address ] [ dest-udp-port | dhcp | domain | 
*                  isakmp | mobile-ip | nameserver | netbios-dgm | netbios-ns | 
*                  ntp | pim-auto-rp | rip | tacacs | tftp | time ]
*
* @cmdhelp Configure helper-address entry.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpHelperAddressSet(EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 **argv,
                                          L7_uint32 index)
{
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg;
  L7_uint32 serverAddr;     /* IPv4 address of server */
  L7_uint32 udpPort = IH_UDP_PORT_DEFAULT;
  L7_RC_t rc;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  /* Error Checking for command Type */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ((numArg != 1) && (numArg != 2))
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_HELPER_ADDRESS);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg > 2)
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_NO_HELPER_ADDRESS);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    ewsTelnetWrite(ewsContext, CLISYNTAX_HELPER_ADDRESS);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  bzero(strIpAddr, sizeof(strIpAddr));
  osapiStrncpySafe(strIpAddr, argv[index + 1], sizeof(strIpAddr));

  if ((numArg > 0) && (usmDbInetAton(strIpAddr, &serverAddr) != L7_SUCCESS))
  {
    ewsTelnetWrite(ewsContext, "\r\nError: Invalid server address");
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (numArg == 2)
  {
    /* UDP port specified. Convert string, either name or number as string, to UDP port number */
    udpPort = strUtilUdpPortNameToNumber(argv[index+2]);
    if (udpPort == IH_UDP_PORT_DEFAULT)
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_HELPER_ADDRESS);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if ((udpPort < L7_IP_HELPER_MIN_PORT) || (udpPort > L7_IP_HELPER_MAX_PORT))
    {
      memset(buf, 0, sizeof(buf));
      sprintf(buf, CLIERR_HELPER_ADDRESS_UDP_PORT_RANGE, L7_IP_HELPER_MIN_PORT, L7_IP_HELPER_MAX_PORT);
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /* global entries cannot be discard entries */
      rc = usmDbIpHelperAddressAdd(IH_INTF_ANY, udpPort, serverAddr);
      if (rc == L7_ERROR)
      {
        ewsTelnetWrite(ewsContext, "\r\nServer address is invalid.");
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      else if (rc == L7_ALREADY_CONFIGURED)
      { 
        ewsTelnetWrite(ewsContext, "\r\nA duplicate entry is already configured.");
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      } 
      else if (rc == L7_TABLE_IS_FULL)
      {
        ewsTelnetWrite(ewsContext, "\r\nThe maximum number of IP helper addresses has already been configured.");
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      else if (rc == L7_ADDR_INUSE)
      {
        ewsTelnetWrite(ewsContext, "\r\nIP helper address cannot be an address on this router.");
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      else if (rc != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, "\r\nFailed to set IP helper address.");
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (numArg == 0)
      {
        /* Clear all global entries */
        if (usmDbIpHelperGlobalAddressesClear() != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, "\r\nFailed to clear all global IP helper addresses.");
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
      else 
      {
        rc = usmDbIpHelperAddressDelete(IH_INTF_ANY, udpPort, serverAddr);
        if (rc == L7_NOT_EXIST)
        {
          ewsTelnetWrite(ewsContext, "\r\nIP helper address not found.");
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        else if (rc != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, "\r\nFailed to delete IP helper address.");
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  set the ip helper-address entry in interface mode
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip helper-address { <server-address> | discard  } [ <dest-udp-port> | 
*                dhcp | domain | isakmp | mobile-ip | nameserver | netbios-dgm | 
*                netbios-ns | ntp | pim-auto-rp | rip | tacacs | tftp | time ]
*
* @cmdhelp Configure IP helper entry on an interface.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIntfIpHelperAddressSet(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 **argv,
                                              L7_uint32 index)
{
  L7_uint32 serverAddr = 0;
  L7_char8  strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intIfNum, slot, port;
  L7_uint32 unit;
  L7_uint32 udpPort = IH_UDP_PORT_DEFAULT;
  L7_uint32 numArg;
  L7_BOOL discard = L7_FALSE;
  L7_RC_t rc;


  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* Error Checking for command Type */
  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    /* have to either specify a server address or "discard" */
    if ((numArg != 1) && (numArg != 2))
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_HELPER_ADDRESS);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {
    if (numArg > 2)
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_NO_HELPER_ADDRESS);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
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
  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
  }
  /* Validate the Routing Interface Number. */
  if (cliValidateRtrIntf(ewsContext, intIfNum) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt(ewsContext, CLISYNTAX_INVALIDROUTERINTF);
  }


  if (numArg > 0)
  {
    bzero(strIpAddr, sizeof(strIpAddr));
    osapiStrncpySafe(strIpAddr, argv[index + 1], sizeof(strIpAddr));

    if (strcmp(strIpAddr, "discard") == 0)
    {
      discard = L7_TRUE;
    }
    else if (usmDbInetAton(strIpAddr, &serverAddr) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, "\r\nError: Invalid server address");
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  if (numArg == 2)
  {
    udpPort = strUtilUdpPortNameToNumber(argv[index+2]);
    if (udpPort == IH_UDP_PORT_DEFAULT)
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_HELPER_ADDRESS);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if ((udpPort < L7_IP_HELPER_MIN_PORT) || (udpPort > L7_IP_HELPER_MAX_PORT))
    {
      memset(buf, 0, sizeof(buf));
      sprintf(buf, CLIERR_HELPER_ADDRESS_UDP_PORT_RANGE, L7_IP_HELPER_MIN_PORT, L7_IP_HELPER_MAX_PORT);
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        if (discard)
        {
          rc = usmDbIpHelperDiscardEntryAdd(intIfNum, udpPort);
        }
        else
        {
          rc = usmDbIpHelperAddressAdd(intIfNum, udpPort, serverAddr);
        }
        if (rc != L7_SUCCESS)
        {
          if (rc == L7_ERROR)
          {
            ewsTelnetWrite(ewsContext, "\r\nError: Invalid Server Address.");
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          }
          else if (rc == L7_ALREADY_CONFIGURED)
          { 
            ewsTelnetWrite(ewsContext, "\r\nError: A duplicate entry is already configured.");
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          } 
          else if (rc == L7_TABLE_IS_FULL)
          {
            ewsTelnetWrite(ewsContext, "\r\nError: The maximum number of IP helper addresses has already been configured.");
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          }
          else if (rc == L7_ADDR_INUSE)
          {
            ewsTelnetWrite(ewsContext, "\r\nError: IP helper address cannot be an address on this router.");
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          }
          else if (rc == L7_REQUEST_DENIED)
          {
            ewsTelnetWrite(ewsContext, "ERROR: Server cannot be in a subnet on an interface where the helper address is configured.");
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          }
          else if (rc == L7_NOT_SUPPORTED)
          {
            ewsTelnetWrite(ewsContext, "ERROR: An IP helper address cannot be configured on this type of interface.");
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          }
          else 
          {
            ewsTelnetWrite(ewsContext, "ERROR: Failed to set IP helper address.");
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          }
        }
      }
      else if (ewsContext->commType == CLI_NO_CMD)
      {
        if (numArg == 0)
        {
          /* Clear all relay entries on this interface */
          if (usmDbIpHelperAddressesRemove(intIfNum) != L7_SUCCESS)
          {
            ewsTelnetWrite(ewsContext, "ERROR: Failed to clear all IP helper addresses on the interface.");       
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          }
        }
        else 
        {
          if (discard)
          {
            rc = usmDbIpHelperDiscardEntryDelete(intIfNum, udpPort);
          }
          else
          {
            rc = usmDbIpHelperAddressDelete(intIfNum, udpPort, serverAddr);
          }
          if (rc != L7_SUCCESS)
          {
            if (rc == L7_NOT_EXIST)
            {
              ewsTelnetWrite(ewsContext, "ERROR: IP helper entry not found.");
              cliSyntaxBottom(ewsContext);
              return cliPrompt(ewsContext);
            }
            else 
            {
              ewsTelnetWrite(ewsContext, "ERROR: Failed to delete IP helper entry.");
              cliSyntaxBottom(ewsContext);
              return cliPrompt(ewsContext);
            }
          }
        }
      }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

