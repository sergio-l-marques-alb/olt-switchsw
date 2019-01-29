/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_show_debug.c
*
* @purpose debugging show commands for the cli
*
* @component Debug
*
* @comments none
*
* @create  09/15/2006
*
* @author  wjacobs
*
* @end
*
*********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "datatypes.h"
#include "clicommands_debug.h"
#include "cli_web_mgr_api.h"

#ifdef L7_SFLOW_PACKAGE
#include "usmdb_sflow.h"
#endif

#include "acl_exports.h"
#include "cli_web_exports.h"
#include "usmdb_common.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_mib_vrrp_api.h"
#include "usmdb_ospf_api.h"
#include "usmdb_ospfv3_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dhcp_client.h"
#include "usmdb_dhcp6c_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_igmp_api.h"
#include "usmdb_mib_dvmrp_api.h"
#include "usmdb_mib_mcast_api.h"
#include "usmdb_mib_pim_api.h"
#include "usmdb_pimsm_api.h"
#endif

#ifdef L7_BGP_PACKAGE
#include "clicommands_debug_bgp.h"
#include "usmdb_bgp4_api.h"
#endif
#ifdef L7_MCAST_PACKAGE
#include "strlib_ip_mcast_cli.h"
#include "strlib_ip_mcast6_cli.h"
#endif

#include "usmdb_iputil_api.h"
#ifdef L7_QOS_FLEX_PACKAGE_VOIP
#include "usmdb_qos_voip_api.h"
#include "strlib_qos_cli.h"
#endif

#ifdef L7_DOT3AH_PACKAGE
#include "usmdb_dot3ah.h"
#endif

#ifdef L7_MCAST_PACKAGE
static void mcastDebuggingShow(EwsContext ewsContext);
/*********************************************************************
*
* @purpose Displays Multicast Debugging Configuration
*
*
* @param EwsContext ewsContext
*
* @notes     Here we  display all mcast debugging information.
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

static void mcastDebuggingShow(EwsContext ewsContext)
{
  L7_BOOL mRxFlag = L7_FALSE, mTxFlag = L7_FALSE;
  L7_uchar8 dispMsg[L7_CLI_MAX_STRING_LENGTH];

   /* IGMP debug status */
   memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
   mRxFlag = mTxFlag = L7_FALSE;
   usmDbMgmdPacketDebugTraceFlagGet(L7_AF_INET, &mRxFlag, &mTxFlag);
   if ((mRxFlag == L7_TRUE) && (mTxFlag == L7_TRUE))
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugIgmpPktEnbld, pStrInfo_ipmcast_DebugMcastRxTx);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mRxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugIgmpPktEnbld, pStrInfo_ipmcast_DebugMcastRxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mTxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugIgmpPktEnbld, pStrInfo_ipmcast_DebugMcastTxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }

   /* DVMRP debug status */
   memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
   mRxFlag = mTxFlag = L7_FALSE;
   usmDbDvmrpPacketDebugTraceFlagGet(&mRxFlag, &mTxFlag);
   if ((mRxFlag == L7_TRUE) && (mTxFlag == L7_TRUE))
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugDvmrpPktEnbld, pStrInfo_ipmcast_DebugMcastRxTx);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mRxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugDvmrpPktEnbld, pStrInfo_ipmcast_DebugMcastRxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mTxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugDvmrpPktEnbld, pStrInfo_ipmcast_DebugMcastTxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }

   /* PIMDM debug status */
   memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
   mRxFlag = mTxFlag = L7_FALSE;
   usmDbPimdmPacketDebugTraceFlagGet(L7_AF_INET, &mRxFlag, &mTxFlag);
   if ((mRxFlag == L7_TRUE) && (mTxFlag == L7_TRUE))
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimdmPktEnbld, pStrInfo_ipmcast_DebugMcastRxTx);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mRxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimdmPktEnbld, pStrInfo_ipmcast_DebugMcastRxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mTxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimdmPktEnbld, pStrInfo_ipmcast_DebugMcastTxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }

   /* PIMSM debug status */
   memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
   mRxFlag = mTxFlag = L7_FALSE;
   usmDbPimsmPacketDebugTraceFlagGet(L7_AF_INET, &mRxFlag, &mTxFlag);
   if ((mRxFlag == L7_TRUE) && (mTxFlag == L7_TRUE))
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimsmPktEnbld, pStrInfo_ipmcast_DebugMcastRxTx);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mRxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimsmPktEnbld, pStrInfo_ipmcast_DebugMcastRxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mTxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimsmPktEnbld, pStrInfo_ipmcast_DebugMcastTxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }

   /* Mcache debug status */
   memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
   mRxFlag = mTxFlag = L7_FALSE;
   usmDbMcachePacketDebugTraceFlagGet(L7_AF_INET, &mRxFlag, &mTxFlag);
   if ((mRxFlag == L7_TRUE) && (mTxFlag == L7_TRUE))
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugMcachePktEnbld, pStrInfo_ipmcast_DebugMcastRxTx);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mRxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugMcachePktEnbld, pStrInfo_ipmcast_DebugMcastRxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mTxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugMcachePktEnbld, pStrInfo_ipmcast_DebugMcastTxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }

#ifdef L7_IPV6_PACKAGE
   /* MLD debug status */
   memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
   mRxFlag = mTxFlag = L7_FALSE;
   usmDbMgmdPacketDebugTraceFlagGet(L7_AF_INET6, &mRxFlag, &mTxFlag);
   if ((mRxFlag == L7_TRUE) && (mTxFlag == L7_TRUE))
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugMldPktEnbld, pStrInfo_ipmcast_DebugMcastRxTx);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mRxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugMldPktEnbld, pStrInfo_ipmcast_DebugMcastRxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mTxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugMldPktEnbld, pStrInfo_ipmcast_DebugMcastTxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }

   /* PIMDM v6 debug status */
   memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
   mRxFlag = mTxFlag = L7_FALSE;
   usmDbPimdmPacketDebugTraceFlagGet(L7_AF_INET6, &mRxFlag, &mTxFlag);
   if ((mRxFlag == L7_TRUE) && (mTxFlag == L7_TRUE))
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimdmV6PktEnbld, pStrInfo_ipmcast_DebugMcastRxTx);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mRxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimdmV6PktEnbld, pStrInfo_ipmcast_DebugMcastRxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mTxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimdmV6PktEnbld, pStrInfo_ipmcast_DebugMcastTxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }

   /* PIMSM-v6 debug status */
   memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
   mRxFlag = mTxFlag = L7_FALSE;
   usmDbPimsmPacketDebugTraceFlagGet(L7_AF_INET6, &mRxFlag, &mTxFlag);
   if ((mRxFlag == L7_TRUE) && (mTxFlag == L7_TRUE))
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimsmV6PktEnbld, pStrInfo_ipmcast_DebugMcastRxTx);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mRxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimsmV6PktEnbld, pStrInfo_ipmcast_DebugMcastRxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mTxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugPimsmV6PktEnbld, pStrInfo_ipmcast_DebugMcastTxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }

   /* Mcache-v6 debug status */
   memset(dispMsg, 0, L7_CLI_MAX_STRING_LENGTH);
   mRxFlag = mTxFlag = L7_FALSE;
   usmDbMcachePacketDebugTraceFlagGet(L7_AF_INET6, &mRxFlag, &mTxFlag);
   if ((mRxFlag == L7_TRUE) && (mTxFlag == L7_TRUE))
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugMcacheV6PktEnbld, pStrInfo_ipmcast_DebugMcastRxTx);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mRxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugMcacheV6PktEnbld, pStrInfo_ipmcast_DebugMcastRxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
   else if (mTxFlag == L7_TRUE)
   {
     osapiSnprintfcat(dispMsg, L7_CLI_MAX_STRING_LENGTH,"%s%s", 
                      pStrInfo_ipmcast_DebugMcacheV6PktEnbld, pStrInfo_ipmcast_DebugMcastTxOnly);
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, dispMsg);
   }
#endif
}
#endif
/*********************************************************************
*
* @purpose Displays Debugging Configuration
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes     Here we  display all debugging information.
*
* @cmdsyntax    show debugging
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowDebugging(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;         /* New variable Added */
  L7_BOOL txFlag,rxFlag;
#if defined(L7_IPV6_MGMT_PACKAGE) || defined(L7_IPV6_PACKAGE)
  L7_BOOL mode;
#endif
  L7_uchar8 family = L7_AF_INET;

#ifdef L7_QOS_FLEX_PACKAGE_ACL 
#ifdef L7_ROUTING_PACKAGE
  L7_char8 str[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;
#endif
#endif

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
  L7_uint32 SIPTraceFlag, H323TraceFlag, SCCPTraceFlag, MGCPTraceFlag;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
#endif

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowDebug_1);
  }

  if (cliWebDebugTraceDisplayModeGet(cliCurrentHandleGet ()) == L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_DebugConsoleEnbld);   
  }

  if (usmDbDot3adPacketDebugTraceFlagGet() == L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugLacpPktEnbld);
  }

  usmDbDot1sPacketDebugTraceFlagGet(&txFlag,&rxFlag);

  if(txFlag)
  {
    if(rxFlag)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugSpanTreePktEnbld);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugSpanTreePktTxEnbld);
    }
  }
  else if(rxFlag)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugSpanTreePktRxEnbld);
  }
#ifdef L7_SFLOW_PACKAGE
  /* sFlow Transmit debugging info */
  if (usmDbComponentPresentCheck(L7_NULL, L7_SFLOW_COMPONENT_ID))
  {
    usmDbsFlowPacketDebugTraceFlagGet(&txFlag);
  
    if (txFlag)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugsFlowPacketEnabled);
    }
  }
#endif
  usmDbSnoopPacketDebugTraceFlagGet(&txFlag,&rxFlag,family);

  if(txFlag)
  {
    if(rxFlag)
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_ENABLED(family));
    }
    else
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_TX_ENABLED(family));
    }
  }
  else if(rxFlag)
  {
    ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_RX_ENABLED(family));
  }

  usmDbSnoopPacketDebugTraceFlagGet(&txFlag,&rxFlag,L7_AF_INET6);

  if(txFlag)
  {
    if(rxFlag)
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_ENABLED(L7_AF_INET6));
    }
    else
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_TX_ENABLED(L7_AF_INET6));
    }
  }
  else if(rxFlag)
  {
    ewsTelnetWrite(ewsContext, CLISYNTAX_DEBUG_SNOOP_PACKET_RX_ENABLED(L7_AF_INET6));
  }

  if (usmDbPingPacketDebugTraceFlagGet() == L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugPingPktEnbld);
  }

  usmDbDot1xPacketDebugTraceFlagGet(&txFlag,&rxFlag);

  if(txFlag)
  {
    if(rxFlag)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot1xPktEnbld);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot1xPktTxEnbld);
    }
  }
  else if(rxFlag)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot1xPktRxEnbld);
  }

#ifdef L7_DOT3AH_PACKAGE
  usmDbDot3ahPacketDebugTraceFlagGet(&txFlag,&rxFlag);

  if(txFlag)
  {
    if(rxFlag)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot3ahPktEnbld);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot3ahPktTxEnbld);
    }
  }
  else if(rxFlag)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DebugDot3ahPktRxEnbld);
  }
#endif

  #ifdef L7_BGP_PACKAGE
  if (usmDbBgpPacketDebugTraceFlagGet() == L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DebugBgpPktEnbld);
  }
  #endif /*L7_PACKAGE_BGP*/

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_OSPF_PACKAGE
  if (usmDbOspfPacketDebugTraceFlagGet() == L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DebugOspfPktEnbld);
  }
#endif
#ifdef L7_RIP_PACKAGE
  if (usmDbRipMapPacketDebugTraceFlagGet() == L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DebugRipPktEnbld);
  }
#endif 


#ifdef L7_IPV6_PACKAGE
  if (usmDbOspfv3PacketDebugTraceFlagGet() == L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DebugOspfV3PktEnbld);
  }
#endif

#ifdef L7_VRRP_PACKAGE
  if (usmDbIpVrrpPacketDebugTraceFlagGet() == L7_TRUE)
  {
     ewsTelnetWriteAddBlanks(1,0,0,0, L7_NULLPTR, ewsContext, pStrErr_base_DebugVrrpPktEnbld);
  }
#endif
  if (usmDbArpPacketDebugTraceFlagGet() == L7_TRUE)
  {
    ewsTelnetWriteAddBlanks(1,0,0,0, L7_NULLPTR ,ewsContext, PStrErr_base_DebugArpPktEnbld);
  }

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  for(i =L7_MIN_ACL_ID; i<=L7_MAX_ACL_ID; i++)
  {
    if (usmDbIpPacketDebugTraceFlagGet(i,&rxFlag)== L7_SUCCESS)
    {
      if(rxFlag == L7_TRUE)
      {
        memset(str,0x00,sizeof(str));
        sprintfAddBlanks(1,0,0,0,L7_NULLPTR,str,pStrErr_base_DebugAclPktEnbld,i);
        ewsTelnetWrite(ewsContext, str);
      }
    }
  }
#endif /*L7_QOS_FLEX_PACKAGE_ACL*/

#ifdef L7_MCAST_PACKAGE
    mcastDebuggingShow(ewsContext);
#endif
#endif /*L7_PACKAGE_ROUTING*/

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_VOIP_COMPONENT_ID) == L7_TRUE)
  {
    if (usmDbQosVoIPDebugTraceAllFlagGet(&SIPTraceFlag, &H323TraceFlag, 
                                         &SCCPTraceFlag, &MGCPTraceFlag) == L7_SUCCESS)
    {
      if (SIPTraceFlag == L7_ENABLE)
      {
        sprintf(buf,"%s %s", pStrInfo_qos_AutoVoIPProtoSIP, pStrInfo_base_TracingEnbld);
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, buf);
      }

      if (H323TraceFlag == L7_ENABLE)
      {
        sprintf(buf,"%s %s", pStrInfo_qos_AutoVoIPProtoH323, pStrInfo_base_TracingEnbld);
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, buf);
      }

      if (SCCPTraceFlag == L7_ENABLE)
      {
        sprintf(buf,"%s %s", pStrInfo_qos_AutoVoIPProtoSCCP, pStrInfo_base_TracingEnbld);
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, buf);
      }
    }
  }
#endif
  if (usmDbIpDhcpClientDebugTraceFlagGet(&rxFlag, &txFlag) == L7_SUCCESS)
  {
    if (txFlag == L7_TRUE)
    {
      if (rxFlag == L7_TRUE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                 pStrErr_base_DebugDhcpEnbld);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                                 pStrErr_base_DebugDhcpPktTxEnbld);
      }
    }
    else if (rxFlag == L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                               pStrErr_base_DebugDhcpPktRxEnbld);
    }
  }

#if defined(L7_IPV6_MGMT_PACKAGE) || defined(L7_IPV6_PACKAGE)
  if ((usmDbDhcp6ClientDebugTraceFlagGet(&mode) == L7_SUCCESS) && (mode == L7_TRUE))
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                             pStrInfo_base_DebugIPv6DhcpEnbld);    
  }
#endif /* L7_IPV6_MGMT_PACKAGE || L7_IPV6_PACKAGE */

  return cliSyntaxReturnPrompt (ewsContext, "");
}

