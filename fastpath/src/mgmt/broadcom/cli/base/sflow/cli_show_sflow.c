/* Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/common/clicommands_sflow.c
 *
 * @purpose create the CLI show commands for sflow
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
 
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "commdefs.h"
#include "cliapi.h"
#include "usmdb_util_api.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "cliutil.h"
#include "sysapi.h"
#include "l3_addrdefs.h"
#include "clicommands_debug.h"
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


/** sflow show commands **/
/**********************************************************************
*
* @purpose This function is used to Display Agent information
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

const L7_char8 *commandShowsFlowAgent(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 numArg;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 version[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32  unit;



  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  numArg = cliNumFunctionArgsGet();
  if(numArg > 1)
  {
     return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_common_ShowSflowAgent,L7_NULLPTR );
  }
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  if ( (usmdbsFlowAgentVersionGet(unit, version) != L7_SUCCESS) ||
       (usmdbsFlowAgentAddressGet(unit, strIpAddr) != L7_SUCCESS))
  {
    return cliPrompt(ewsContext);
  }
 /*sFlow Version*/
  memset(buf, 0, sizeof(buf));
  osapiSnprintf(buf,sizeof(buf), "%s", version); 
  cliFormat(ewsContext, pStrInfo_base_sFlowVersion);
  ewsTelnetWrite(ewsContext, buf);
 /*Agent Ip Address*/
  cliFormat(ewsContext,pStrInfo_common_ApIpAddr);
  ewsTelnetWrite(ewsContext, strIpAddr);
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}
/**********************************************************************
*
* @purpose This function is used to Display sFlow Receivers information
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
const L7_char8 *commandShowsFlowReceivers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 owner[FD_SFLOW_OWNER_LENGTH];
  L7_inet_addr_t ipAddr;
  L7_uint32  unit, time, size, type, port, version, tmpIndex=1, numArg;
  L7_uint32 rcvrIdx = 0;


  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  numArg = cliNumFunctionArgsGet();
  if(numArg > 2)
  {
     return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput, 
                                            ewsContext, pStrErr_common_ShowSflowReceivers, L7_NULLPTR );
  }
  if( numArg == 1)  /* receiver index */
  {
    if (cliConvertTo32BitUnsignedInteger(argv[tmpIndex+2], &rcvrIdx) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
    }
    if (rcvrIdx <1 || rcvrIdx > L7_SFLOW_MAX_RECEIVERS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                             ewsContext, pStrErr_common_InvalidsFlowRcvrIndex);
    }    
  }

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  if( numArg == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrInfo_base_RcvrRcvrTimeMdataAddrPortDatagram);
    ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrInfo_base_IndexOwnerOutSizeVersion);
    ewsTelnetWrite(ewsContext,"\r\n---- -------------------------------- ---------- --------- ----- ---------------");
    do
    {
      if ( (usmdbsFlowRcvrOwnerGet(unit, tmpIndex, owner) != L7_SUCCESS) ||
           (usmdbsFlowRcvrTimeoutGet( unit,tmpIndex, &time) != L7_SUCCESS)||
           (usmdbsFlowRcvrAddressGet(unit, tmpIndex, &ipAddr) != L7_SUCCESS) ||
           (usmdbsFlowRcvrPortGet( unit, tmpIndex, &port) != L7_SUCCESS)||
           (usmdbSflowRcvrMaxDatagramSizeGet(unit, tmpIndex, &size)!= L7_SUCCESS))
      {
       return cliPrompt(ewsContext);
      }
      osapiSnprintf(buf,sizeof(buf), "\r\n%-5d", tmpIndex);
      ewsTelnetWrite(ewsContext, buf); 
      if ( strlen(owner) > 32 )
      {
        owner[32] = L7_EOS;
      }
      osapiSnprintf(buf,sizeof(buf), "%-33s", owner);
      ewsTelnetWrite(ewsContext, buf);
      osapiSnprintf(buf,sizeof(buf), "%-11u", time);
      ewsTelnetWrite(ewsContext, buf);
      osapiSnprintf(buf,sizeof(buf), "%-10d", size);
      ewsTelnetWrite(ewsContext, buf);
      if( inetAddrPrint(&ipAddr, strIpAddr) == L7_NULLPTR)
      {
        memset(strIpAddr, 0 , sizeof(strIpAddr));
      }
      osapiSnprintf(buf,sizeof(buf), "%-6d", port);
      ewsTelnetWrite(ewsContext, buf);
      osapiSnprintf(buf,sizeof(buf), "%-15s", strIpAddr);
      ewsTelnetWrite(ewsContext, buf);
      memset(owner, 0 , sizeof(owner));
    }while( usmdbsFlowRcvrEntryNextGet(unit, &tmpIndex) == L7_SUCCESS);
  }

  memset(owner, 0 , sizeof(owner));
  if( numArg == 1) /* for receiver index */
  {
    if ( (usmdbsFlowRcvrOwnerGet(unit, rcvrIdx, owner) != L7_SUCCESS) ||
           (usmdbsFlowRcvrTimeoutGet( unit,rcvrIdx, &time) != L7_SUCCESS)||
           (usmdbsFlowRcvrAddressGet(unit, rcvrIdx, &ipAddr) != L7_SUCCESS) ||
           (usmdbsFlowRcvrAddressTypeGet( unit, rcvrIdx, &type) != L7_SUCCESS)||
           (usmdbsFlowRcvrPortGet( unit, rcvrIdx, &port) != L7_SUCCESS)||
           (usmdbsFlowRcvrDatagramVersionGet(unit, rcvrIdx, &version) != L7_SUCCESS)||
           (usmdbSflowRcvrMaxDatagramSizeGet(unit, rcvrIdx, &size)!= L7_SUCCESS))
    {
      return cliPrompt(ewsContext);
    }
 
    memset(buf, 0, sizeof(buf));
    osapiSnprintf(buf,sizeof(buf), "%d", rcvrIdx);
    cliFormat(ewsContext, pStrInfo_base_sFlowRcvr );
    ewsTelnetWrite(ewsContext, buf);

    memset(buf, 0, sizeof(buf));
    osapiSnprintf(buf,sizeof(buf), "%s", owner);
    cliFormat(ewsContext, pStrInfo_base_sFlowOwner);
    ewsTelnetWrite(ewsContext, buf);
    
    memset(buf, 0, sizeof(buf));
    osapiSnprintf(buf,sizeof(buf), "%u", time);
    cliFormat(ewsContext, pStrInfo_base_sFlowTimeout);
    ewsTelnetWrite(ewsContext, buf);
  
    memset(buf, 0, sizeof(buf));
    if( inetAddrPrint(&ipAddr, strIpAddr) == L7_NULLPTR)
    {
      memset(strIpAddr, 0 , sizeof(strIpAddr));
    }
    osapiSnprintf(buf,sizeof(buf), "%-17s", strIpAddr);
    cliFormat(ewsContext, pStrInfo_common_IpAddr_1);
    ewsTelnetWrite(ewsContext, buf);
  
    memset(buf, 0, sizeof(buf));
    osapiSnprintf(buf,sizeof(buf), "%d", type);
    cliFormat(ewsContext, pStrinfo_common_Sflow_Rcvr_AddrType);
    ewsTelnetWrite(ewsContext, buf);

    memset(buf, 0, sizeof(buf));
    osapiSnprintf(buf,sizeof(buf), "%d", port);
    cliFormat(ewsContext, pStrInfo_common_Port_2);
    ewsTelnetWrite(ewsContext, buf);
    
    memset(buf, 0, sizeof(buf));
    osapiSnprintf(buf,sizeof(buf), "%d", version);
    cliFormat(ewsContext, pStrInfo_base_sFlowDatagramVersion);
    ewsTelnetWrite(ewsContext, buf);  

    memset(buf, 0, sizeof(buf));
    osapiSnprintf(buf,sizeof(buf), "%d", size);
    cliFormat(ewsContext, pStrInfo_base_sFlowDatagram);
    ewsTelnetWrite(ewsContext, buf);
  }
  
  cliSyntaxBottom(ewsContext);
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;;
  return cliPrompt(ewsContext);
}
/**********************************************************************
*
* @purpose This function is used to Display sFlow samplers information
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
const L7_char8 *commandShowsFlowSamplers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg,  unit;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 usp[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 rcvrIdx, rate, maxSize, interface = 0, u, s, p;
  L7_uint32 nextInterface;
  L7_RC_t rc = L7_SUCCESS;
  
 
  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_TRUE);
 
  numArg = cliNumFunctionArgsGet();
  if(numArg > 1)
  {
     return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_common_ShowSflowSamplers,L7_NULLPTR );
  } 
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  ewsTelnetWriteAddBlanks (1, 0, 1, 2, L7_NULLPTR, ewsContext, pStrInfo_base_SamplerRcvrPSamplingMaxHeader);
  ewsTelnetWriteAddBlanks (1, 0, 1, 2, L7_NULLPTR, ewsContext, pStrInfo_base_DataIndexRateSize);
  ewsTelnetWrite(ewsContext, "\r\n -----------  -------   -------------   ----------");

  rc = usmDbIntIfNumTypeFirstGet(unit, L7_PHYSICAL_INTF, 0, &interface);
  while (rc == L7_SUCCESS)
  {
    rc = usmDbUnitSlotPortGet(interface, &u, &s, &p);
    if (rc == L7_SUCCESS)
    {
      osapiSnprintf(usp,sizeof(usp), cliDisplayInterfaceHelp(u, s, p));
      if( (usmdbsFlowFsReceiverGet(unit, interface, 1, &rcvrIdx) == L7_SUCCESS) &&
          (usmdbsFlowFsPacketSamplingRateGet(unit, interface, 1, &rate) == L7_SUCCESS) &&
          (usmdbsFlowFsMaximumHeaderSizeGet(unit, interface, 1, &maxSize) == L7_SUCCESS) )
      { 
        memset(buf, 0X00, L7_CLI_MAX_STRING_LENGTH );
        osapiSnprintf(buf,sizeof(buf), "\r\n %-13s", usp );
        ewsTelnetWrite(ewsContext, buf);
        osapiSnprintf(buf,sizeof(buf), "%-10d", rcvrIdx );
        ewsTelnetWrite(ewsContext, buf);
        osapiSnprintf(buf,sizeof(buf), "%-16d", rate );
        ewsTelnetWrite(ewsContext, buf);        
        osapiSnprintf(buf,sizeof(buf), "%-10d", maxSize );
        ewsTelnetWrite(ewsContext, buf);
      }
      rc = usmDbGetNextPhysicalIntIfNumber(interface, &nextInterface); 
      if(rc == L7_SUCCESS)
      {
        interface = nextInterface;
      }
    }
  }

  cliSyntaxBottom(ewsContext); 
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;;
  return cliPrompt(ewsContext);
}
/**********************************************************************
*
* @purpose This function is used to Display sFlow Pollers information
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
const L7_char8 *commandShowsFlowPollers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg,  unit;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 usp[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 rcvrIdx, interval, u, s, p, interface = 1; /* first internal interface*/
  L7_uint32 sysIntfType, nextInterface;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_TRUE);
  
  numArg = cliNumFunctionArgsGet();
  if(numArg > 1)
  {
     return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_common_ShowSflowPollers,L7_NULLPTR );
  }
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  ewsTelnetWriteAddBlanks (1, 0, 1, 2, L7_NULLPTR, ewsContext, pStrInfo_base_PollerRcvrPoller);
  ewsTelnetWriteAddBlanks (1, 0, 1, 2, L7_NULLPTR, ewsContext, pStrInfo_base_DataIndexInterval);
  ewsTelnetWrite(ewsContext, "\r\n -----------   -------   -------");
  do
  {

    if (usmDbIntfTypeGet(interface, &sysIntfType) == L7_SUCCESS &&
         sysIntfType == L7_PHYSICAL_INTF)
    {
       rc = usmDbUnitSlotPortGet(interface, &u, &s, &p);
       if (rc != L7_SUCCESS)
       {
          return cliSyntaxReturnPrompt (ewsContext, "");
       }
       osapiSnprintf(usp,sizeof(usp), cliDisplayInterfaceHelp(u, s, p));
       if( (usmdbsFlowCpReceiverGet(unit, interface, 1, &rcvrIdx) == L7_SUCCESS) &&
           (usmdbsFlowCpIntervalGet(unit, interface, 1, &interval) == L7_SUCCESS) )
       {
         osapiSnprintf(buf,sizeof(buf), "\r\n %-14s", usp );
         ewsTelnetWrite(ewsContext, buf);
         osapiSnprintf(buf,sizeof(buf), "%-10d", rcvrIdx );
         ewsTelnetWrite(ewsContext, buf);
         osapiSnprintf(buf,sizeof(buf), "%-6d", interval );
         ewsTelnetWrite(ewsContext, buf);
         memset(buf, 0, sizeof(buf));
      }
    }
    rc =usmDbValidIntIfNumNext(interface, &nextInterface);
    if(rc == L7_SUCCESS)
    {
      interface = nextInterface;
    }

  }while ( rc == L7_SUCCESS);

  cliSyntaxBottom(ewsContext);
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;;
  return cliPrompt(ewsContext);
}
