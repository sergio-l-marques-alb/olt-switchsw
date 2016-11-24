/* Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/common/cli_config_sflow.c
 *
 * @purpose sFlow config commands for CLI
 *
 * @component sflow
 *
 * @comments contains the code to build the root of the tree
 * @comments also contains functions that allow tree navigation
 *
 * @create  10/16/2007
 *
 * @author  Rajesh G
 * @end
 *
 **********************************************************************/
 
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "commdefs.h"
#include "cliapi.h"
#include "usmdb_util_api.h"
#include "cli_web_exports.h"
#include "cliutil.h"
#include "sysapi.h"
#include "l3_addrdefs.h"
#include "clicommands_debug.h"
#include "clicommands_card.h"
#include "clicommands_support.h"
#include "osapi_support.h"
#ifdef L7_STACKING_PACKAGE
#include "unitmgr_api.h"
#endif
#include "osapi.h"
#include "nimapi.h"
/* layer 3 includes           */
#include "usmdb_ip_api.h"

#include "datatypes.h"
#include "usmdb_support.h"
#include "usmdb_sflow.h"
#include "sflow_exports.h"

/**********************************************************************
*
* @purpose Function Configures sFlow Receiver Parameters
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end

*
**********************************************************************/

const L7_char8 *commandsFlowReceiver(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg,unit,tmpInd=1;
  L7_char8  strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  owner[FD_SFLOW_OWNER_LENGTH];
  L7_uint32 rcvrIdx, time = L7_NULL, argPort = FD_SFLOW_RCVR_PORT; 
  L7_uint32 maxSize = FD_SFLOW_MAX_DGRAM_SIZE;

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

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 5 ) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if ((ewsContext->commType == CLI_NO_CMD) && (numArg <1 || numArg> 4 ) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                            ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+1], &rcvrIdx) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if (rcvrIdx <1 || rcvrIdx > L7_SFLOW_MAX_RECEIVERS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  } 
  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    memset (owner, L7_EOS ,FD_SFLOW_OWNER_LENGTH);
    if(strlen(argv[tmpInd + 3]) >=  FD_SFLOW_OWNER_LENGTH)/* Owner string is 0 - 127 characters */
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                             ewsContext, pStrErr_common_InvalidRcvrOwner);
    }
    OSAPI_STRNCPY_SAFE(owner, argv[tmpInd + 3]);
    if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+5], &time) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvrTime);
    }
    if( usmdbsFlowRcvrOwnerSet(unit, rcvrIdx, owner, time) != L7_SUCCESS) 
    {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                               ewsContext, pStrErr_common_sFlowRcvrBusy);
     }
   }  
  if ( ewsContext->commType == CLI_NO_CMD && (numArg  >= 1 && numArg <5))
  {
     /* we can send default values here */
     if (numArg == 1)
     {
       memset(owner, L7_EOS, sizeof(owner));
       memset(strIPaddr, L7_EOS, sizeof(strIPaddr));

       OSAPI_STRNCPY_SAFE(strIPaddr, FD_SFLOW_DEFAULT_IP); 
       /* set to default values */
       argPort = FD_SFLOW_RCVR_PORT;
       time  = FD_SFLOW_RCVR_TIMEOUT;
       maxSize = FD_SFLOW_MAX_DGRAM_SIZE;

       if( (usmdbsFlowRcvrOwnerSet(unit, rcvrIdx, owner, time) != L7_SUCCESS) ||
           (usmdbsFlowRcvrAddressSet(unit, rcvrIdx, strIPaddr) != L7_SUCCESS)||
           (usmdbsFlowRcvrPortSet(unit, rcvrIdx, argPort) != L7_SUCCESS)||
           (usmdbsFlowRcvrMaximumDatagramSizeSet(unit, rcvrIdx, maxSize) != L7_SUCCESS))
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                                  ewsContext, pStrErr_common_InvalidsFlowRcvr);
       }
     }
     if (numArg == 2)
     {
       if ((usmdbsFlowRcvrOwnerSet(unit, rcvrIdx, owner, time) != L7_SUCCESS) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvr);
       }
     }
     if (numArg == 4)
     {
       if (usmdbsFlowRcvrTimeoutSet(unit, rcvrIdx, time) != L7_SUCCESS)
       {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                               ewsContext, pStrErr_common_InvalidsFlowRcvrTime);
       }
     }    
  }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
*
* @purpose Function Configure sFlow Receiver ip Parameters
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end

*
**********************************************************************/

const L7_char8 *commandsFlowReceiverIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, tmpInd = 1, numArg, rcvrIdx;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_inet_addr_t IPaddr;
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }

  if ((ewsContext->commType == CLI_NORMAL_CMD || ewsContext->commType == CLI_NO_CMD) && ( numArg > 1) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                            ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+1], &rcvrIdx) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if (rcvrIdx <1 || rcvrIdx > L7_SFLOW_MAX_RECEIVERS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArg == 1))
  {
    memset(strIPaddr, L7_EOS, sizeof(strIPaddr));
    OSAPI_STRNCPY_SAFE(strIPaddr, argv[tmpInd + 3]);  
    if(usmDbParseInetAddrFromStr(strIPaddr, &IPaddr) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvrIp);
    }
    if(IPaddr.family == L7_AF_INET && 
       usmDbNetworkAddressValidate(IPaddr.addr.ipv4.s_addr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvrIp);
    }
    if(usmdbsFlowRcvrAddressSet(unit, rcvrIdx, strIPaddr) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrIp);
    }
  }
  if (( ewsContext->commType == CLI_NO_CMD))
  {
    OSAPI_STRNCPY_SAFE(strIPaddr, FD_SFLOW_DEFAULT_IP);
    if(usmdbsFlowRcvrAddressSet(unit, rcvrIdx, strIPaddr) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrIp);
    }
  }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}
/**********************************************************************
*
* @purpose Function Configure sFlow Receiver port Parameters
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end

*
**********************************************************************/
const L7_char8 *commandsFlowReceiverPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, tmpInd = 1, numArg, rcvrIdx;
  L7_uint32 port;
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }

  if ((ewsContext->commType == CLI_NORMAL_CMD || ewsContext->commType == CLI_NO_CMD) && ( numArg > 1) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+1], &rcvrIdx) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if (rcvrIdx <1 || rcvrIdx > L7_SFLOW_MAX_RECEIVERS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArg == 1))
  {
    if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+3], &port) != L7_SUCCESS ||
        port > L7_SFLOW_MAX_PORT)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvrPort);
    }
    if(usmdbsFlowRcvrPortSet(unit, rcvrIdx, port) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrPort);
    }
  }
  if (( ewsContext->commType == CLI_NO_CMD) && numArg == 0)
  {
    port = FD_SFLOW_RCVR_PORT;
    if(usmdbsFlowRcvrPortSet(unit, rcvrIdx, port) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrPort);
    }
  }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}
/**********************************************************************
*
* @purpose Function Configure sFlow Receiver maximum datagram  Parameters
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end

*
**********************************************************************/
const L7_char8 *commandsFlowReceiverMaxData(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 unit, tmpInd = 1, numArg, rcvrIdx;
  L7_uint32 maxDSize;
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }

  if ((ewsContext->commType == CLI_NORMAL_CMD || ewsContext->commType == CLI_NO_CMD) && ( numArg > 1) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+1], &rcvrIdx) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if (rcvrIdx <1 || rcvrIdx > L7_SFLOW_MAX_RECEIVERS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
  }
  if ((ewsContext->commType == CLI_NORMAL_CMD) && ( numArg == 1))
  {
    if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+3], &maxDSize) != L7_SUCCESS ||
         (maxDSize < L7_SFLOW_MIN_DATAGRAM_SIZE || maxDSize >L7_SFLOW_MAX_DATAGRAM_SIZE))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvrSize);
    }
    if(usmdbsFlowRcvrMaximumDatagramSizeSet(unit, rcvrIdx, maxDSize) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrSize);
    }
  }
  if (( ewsContext->commType == CLI_NO_CMD) && numArg == 0)
  {
    maxDSize = FD_SFLOW_MAX_DGRAM_SIZE;
    if(usmdbsFlowRcvrMaximumDatagramSizeSet(unit, rcvrIdx, maxDSize) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrSize);
    }
  }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}
/**********************************************************************
*
* @purpose This function is used tp add valid  receiver index to sFlow Sampler
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end

*
**********************************************************************/
const L7_char8 *commandInterfacesFlowSampler(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, tmpInd=1;
  L7_uint32 intInterface;
  L7_uint32 s, p, rcvrIdx;
  L7_uint32 instance = 1; /* currently we are supporting one instance */
  L7_uint32 numArg;
  L7_RC_t   rc = L7_FAILURE;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }
  if(unit == 0)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }
  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intInterface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  if( (numArg > 1 )|| !((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD) ) )
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowSampler);
  }

  if (numArg ==1 && ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+1], &rcvrIdx) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                                 ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
    }
    if (rcvrIdx < 1 || rcvrIdx > L7_SFLOW_MAX_RECEIVERS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                                 ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
    }
    rc = usmdbsFlowFsReceiverSet(unit, intInterface, instance, rcvrIdx);
    if (rc == L7_ALREADY_CONFIGURED)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowPollerRcvr);
    }
    else if(rc == L7_TABLE_IS_FULL)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                                 ewsContext, pStrinfo_common_sFlowFsLimitExceeded);
    }
    else if(rc == L7_FAILURE)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                                 ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
    }
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    rcvrIdx = L7_NULL;
    rc = usmdbsFlowFsReceiverSet(unit, intInterface, instance, rcvrIdx);
    if (rc == L7_ALREADY_CONFIGURED)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
    }
    else if( rc == L7_FAILURE)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                           ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
    }
  }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/************************************************************************
*
* @purpose This function is used to Configure sFlow sampler maximum header
*          Parameters
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*
*************************************************************************/
const L7_char8 *commandInterfacesFlowSpMaxHSize(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, tmpInd=1;
  L7_uint32 intInterface;
  L7_uint32 s, p, rcvrIdx, maxSize;
  L7_uint32 instance = 1; /* currently we are supporting one instance */
  L7_uint32 numArg;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }
  if(unit == 0)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }
  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intInterface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  if( (numArg > 1 )|| !((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD) ) )
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowSamplerMaxHSize);
  }
  if (numArg ==1 && ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+2], &maxSize) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowSpMaxHSize);
    }
    if (maxSize < L7_SFLOW_MIN_HEADER_SIZE|| maxSize > L7_SFLOW_MAX_HEADER_SIZE)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowSpMaxHSize);
    }
    if(usmdbsFlowFsReceiverGet(unit, intInterface, instance, &rcvrIdx)!= L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvrIndexOption);
    }
    if (usmdbsFlowFsMaximumHeaderSizeSet( unit, intInterface,  instance, maxSize) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowSpMaxHSize);
    }
  }
  if ( ewsContext->commType == CLI_NO_CMD)
  {
    maxSize = FD_SFLOW_DEFAULT_HEADER_SIZE;
    if(usmdbsFlowFsReceiverGet(unit, intInterface, instance, &rcvrIdx)!= L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrIndexOption);
    }
     
    if (usmdbsFlowFsMaximumHeaderSizeSet( unit, intInterface,  instance, maxSize) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                                 ewsContext, pStrErr_common_InvalidsFlowSpMaxHSize);
    }
  }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/**********************************************************************
*
* @purpose This function is used to configure sFlow sampling rate
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end

*
**********************************************************************/
const L7_char8 *commandInterfacesFlowSpRate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 unit, tmpInd=1;
  L7_uint32 intInterface;
  L7_uint32 s, p, rcvrIdx, rate;
  L7_uint32 instance = 1; /* currently we are supporting one instance */
  L7_uint32 numArg;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  if(unit == 0)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }
  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intInterface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  if( (numArg > 1 )|| !((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD) ) )
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowSampler);
  }
  if (numArg ==1 && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+2], &rate) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowSpRate);
    }
    if (rate< L7_SFLOW_MIN_SAMPLING_RATE || rate > L7_SFLOW_MAX_SAMPLING_RATE)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowSpRate);
    }
    if(usmdbsFlowFsReceiverGet(unit, intInterface, instance, &rcvrIdx)!= L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrIndexOption);
    }
    if (usmdbsFlowFsPacketSamplingRateSet( unit, intInterface,  instance, rate) != L7_SUCCESS)
    {       
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                               ewsContext, pStrErr_common_InvalidsFlowSpRate);
    }
    
  }
  if (ewsContext->commType == CLI_NO_CMD)
  {
    rate= FD_SFLOW_SAMPLING_RATE;
    if(usmdbsFlowFsReceiverGet(unit, intInterface, instance, &rcvrIdx)!= L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvrIndexOption);
    }
    if (usmdbsFlowFsPacketSamplingRateSet( unit, intInterface,  instance, rate) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowSpRate);
    }
  }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/**************************************************************************
*
* @purpose This function is used to add valid receiver index to sFlow Poller
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*
****************************************************************************/
const L7_char8 *commandInterfacesFlowPoller(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit, tmpInd=1;
  L7_uint32 intInterface;
  L7_uint32 s, p, rcvrIdx;
  L7_uint32 instance = 1; /* currently we are supporting one instance */
  L7_uint32 numArg;
  L7_RC_t   rc = L7_FAILURE;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  if(unit == 0)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }
  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intInterface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  if( (numArg > 1 )|| !((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD) ) )
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                              ewsContext, pStrErr_common_InvalidsFlowPoller);
  }
  if (numArg ==1 && ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+1], &rcvrIdx) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
    }
    if (rcvrIdx < 1 || rcvrIdx > L7_SFLOW_MAX_RECEIVERS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, 
                                                  pStrErr_common_InvalidsFlowRcvrIndex);
    }

    rc = usmdbsFlowCpReceiverSet(unit, intInterface, instance, rcvrIdx);
    if (rc == L7_ALREADY_CONFIGURED)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowPollerRcvr);
    }
    else if (rc == L7_FAILURE)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                                  pStrErr_common_InvalidsFlowRcvrIndex);
    }
  }
  if ( ewsContext->commType == CLI_NO_CMD)
  {
    rcvrIdx = L7_NULL;
    rc = usmdbsFlowCpReceiverSet(unit, intInterface, instance, rcvrIdx); 
    if( rc == L7_ALREADY_CONFIGURED) 
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
    }
    else if( rc == L7_FAILURE)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                                 ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
    }   
   }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/**********************************************************************
*
* @purpose This function is used to configure sFlow Poller Interval
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*
**********************************************************************/
const L7_char8 *commandInterfacesFlowCpInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 unit, tmpInd=1;
  L7_uint32 intInterface;
  L7_uint32 s, p, rcvrIdx, interval;
  L7_uint32 instance = 1; /* currently we are supporting one instance */
  L7_uint32 numArg;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  
  cliSyntaxTop(ewsContext);
  
  numArg = cliNumFunctionArgsGet();

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  if(unit == 0)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext,
                                           pStrErr_common_UnitId_1);
  }
  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &intInterface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  } 
  if( (numArg > 1 )|| !((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD) ) )
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowPollerInterval);
  }
  if (numArg ==1  && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    if (cliConvertTo32BitUnsignedInteger(argv[tmpInd+2], &interval) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowCpInterval);
    }
    if (interval< L7_SFLOW_MIN_POLLING_INTERVAL || interval > L7_SFLOW_MAX_POLLING_INTERVAL)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                              ewsContext, pStrErr_common_InvalidsFlowCpInterval);
    }
    if(usmdbsFlowCpReceiverGet(unit, intInterface, instance, &rcvrIdx)!= L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvrIndexOption);
    }
    if (usmdbsFlowCpIntervalSet( unit, intInterface,  instance, interval) != L7_SUCCESS)
    {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                                ewsContext, pStrErr_common_InvalidsFlowCpInterval);
    }
  }
  if (ewsContext->commType == CLI_NO_CMD)
  {
    interval= FD_SFLOW_POLL_INTERVAL;
    if(usmdbsFlowCpReceiverGet(unit, intInterface, instance, &rcvrIdx)!= L7_SUCCESS)
    {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                                 ewsContext, pStrErr_common_InvalidsFlowRcvrIndexOption);
    }
    if (usmdbsFlowCpIntervalSet( unit, intInterface,  instance, interval) != L7_SUCCESS)
    {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                                 ewsContext, pStrErr_common_InvalidsFlowCpInterval);
    }
  }
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

