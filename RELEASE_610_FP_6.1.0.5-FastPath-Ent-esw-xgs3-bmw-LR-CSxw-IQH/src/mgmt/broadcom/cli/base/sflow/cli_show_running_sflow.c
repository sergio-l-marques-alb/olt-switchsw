/* Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/sflow/clishow_running_sflow.c
 *
 * @purpose create show running commands for  sflow
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
#include "datatypes.h"
#include "commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_switching_cli.h"
#include "strlib_base_cli.h"

#include "usmdb_counters_api.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "defaultconfig.h"
#include "usmdb_sflow.h"

#include "cliapi.h"
#include "ews.h"
#include "ews_cli.h"

#include "cliutil.h"
#include "cli_show_running_config.h"
#include "clicommands_card.h" 
 
 /*********************************************************************
 * @purpose  To display sFlow  running configuratio
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 uint
 * @param    
 *
 * @returns  L7_RC_t
 * @returns  cliPrompt(ewsContext)
 *
 * @notes
 *
 @end
 *********************************************************************/
L7_RC_t cliRunningConfigSflowInfo(EwsContext ewsContext, L7_uint32 unit)
{
   L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 owner[FD_SFLOW_OWNER_LENGTH];
   L7_uint32 size, port, time = FD_SFLOW_SAMPLING_RATE; 
   L7_uint32 rcvrIdx =1;
   L7_inet_addr_t  ipAddr;
   L7_uchar8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
    
  
   do
   {
     memset(buf, L7_EOS, L7_CLI_MAX_STRING_LENGTH);
     memset(owner, L7_EOS, FD_SFLOW_OWNER_LENGTH);
     memset(strIpAddr, L7_EOS, L7_CLI_MAX_STRING_LENGTH);
     
     osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),
         pStrInfo_base_sFlowRcvrCmd, rcvrIdx);
     
     if ((usmdbsFlowRcvrOwnerGet(unit, rcvrIdx, owner) == L7_SUCCESS) &&
         (usmdbsFlowRcvrTimeoutGet( unit,rcvrIdx, &time) == L7_SUCCESS) )
     {
       if( strlen(owner) > 0)
       {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), 
         pStrInfo_base_sFlowRcvrCmd, rcvrIdx);
         
         osapiSnprintfcat(buf, sizeof(buf), "%s %s %s %u", pStrinfo_common_Sflow_Rcvr_Owner, owner, 
                           pStrinfo_common_Sflow_Rcvr_Timeout, time);
         EWSWRITEBUFFER (ewsContext, buf);
       }
       if(usmdbsFlowRcvrAddressGet(unit, rcvrIdx, &ipAddr) == L7_SUCCESS)
       { 
         if( inetAddrPrint(&ipAddr, strIpAddr) == L7_NULLPTR)
         {
           memset(strIpAddr, 0 , sizeof(strIpAddr));
         }
         if( osapiStrncmp(strIpAddr, FD_SFLOW_DEFAULT_IP, sizeof(FD_SFLOW_DEFAULT_IP)) != 0)
         {
           osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), 
           pStrInfo_base_sFlowRcvrCmd, rcvrIdx);
           osapiSnprintfcat(buf, sizeof(buf), "%s %s", pStrInfo_common_IpOption, strIpAddr);
           EWSWRITEBUFFER (ewsContext, buf);
         }
       }  
     }
     if( usmdbsFlowRcvrPortGet( unit, rcvrIdx, &port) == L7_SUCCESS)   
     {
       memset(buf, L7_EOS, L7_CLI_MAX_STRING_LENGTH);

       if( port != FD_SFLOW_RCVR_PORT)
       {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),
         pStrInfo_base_sFlowRcvrCmd, rcvrIdx);
         osapiSnprintfcat(buf, sizeof(buf), "%s %d", pStrinfo_common_Sflow_Rcvr_Port, port);
         EWSWRITEBUFFER (ewsContext, buf);
       }
     }
     if (usmdbSflowRcvrMaxDatagramSizeGet(unit, rcvrIdx, &size) == L7_SUCCESS)     
     {
       memset(buf, L7_EOS, L7_CLI_MAX_STRING_LENGTH);

       if (size != FD_SFLOW_MAX_DGRAM_SIZE)
       {
         osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),
         pStrInfo_base_sFlowRcvrCmd, rcvrIdx);
         osapiSnprintfcat(buf, sizeof(buf), "%s %d", pStrinfo_common_Sflow_Rcvr_MaxDatagram, size);
         EWSWRITEBUFFER (ewsContext, buf);
       }
     }
   }while(usmdbsFlowRcvrEntryNextGet(unit, &rcvrIdx) == L7_SUCCESS);
     
  return L7_SUCCESS;    
}


/*********************************************************************
* @purpose  To print the physical interface running configuration of sflow 
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
* @param    L7_uint32 interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*  @end
*********************************************************************/
L7_RC_t cliRunningConfigsFlowInterfaceInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val = 0;
  L7_uint32 instance = FD_SFLOW_INSTANCE;

  memset(buf, L7_EOS, sizeof(buf));

  if( usmdbsFlowFsReceiverGet(unit, interface, instance, &val) == L7_SUCCESS)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),
         pStrInfo_base_sFlowSamplerCmd); 

    osapiSnprintfcat(buf, sizeof(buf), "%d", val);
    EWSWRITEBUFFER (ewsContext, buf);
    
    memset(buf, L7_EOS, sizeof(buf));

    if(usmdbsFlowFsMaximumHeaderSizeGet(unit, interface, instance, &val) == L7_SUCCESS)
    {
      if( val != FD_SFLOW_DEFAULT_HEADER_SIZE)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),
        pStrInfo_base_sFlowSamplerCmd);

        osapiSnprintfcat(buf, sizeof(buf), "%s %d", pStrinfo_common_Sflow_SpMaxHSize, val);
        EWSWRITEBUFFER (ewsContext, buf);

        memset(buf, L7_EOS, sizeof(buf));
      }
    }
    
    if(usmdbsFlowFsPacketSamplingRateGet(unit, interface, instance, &val) == L7_SUCCESS)
    {
      if( val != FD_SFLOW_DEFAULT_SAMPLING_RATE)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),
        pStrInfo_base_sFlowSamplerCmd);

        osapiSnprintfcat(buf, sizeof(buf), "%s %d", pStrinfo_common_Sflow_SpRate, val);
        EWSWRITEBUFFER (ewsContext, buf);

        memset(buf, L7_EOS, sizeof(buf));
      }
    }
  }
  memset(buf, L7_EOS, sizeof(buf));
  if( usmdbsFlowCpReceiverGet(unit, interface, instance, &val) == L7_SUCCESS)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),
         pStrInfo_base_sFlowPollerCmd);

    osapiSnprintfcat(buf, sizeof(buf), "%d", val);
    EWSWRITEBUFFER (ewsContext, buf);

    memset(buf, L7_EOS, sizeof(buf));

    if(usmdbsFlowCpIntervalGet(unit, interface, instance, &val) == L7_SUCCESS)
    {
      if( val != FD_SFLOW_POLL_INTERVAL)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),
        pStrInfo_base_sFlowPollerCmd);

        osapiSnprintfcat(buf, sizeof(buf), "%s %d", pStrinfo_common_Sflow_CpInterval, val);
        EWSWRITEBUFFER (ewsContext, buf);

        memset(buf, L7_EOS, sizeof(buf));
      }
    }
  }
  return L7_SUCCESS;
}
