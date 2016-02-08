/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2006-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_debug.c
 *
 * @purpose show running config commands for the debug commands
 *
 * @component user interface
 *
 * @comments
 *
 * @create  09/15/2006
 *
 * @author  wjacobs
 * @end
 *
 **********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_ospf_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_util_api.h"
#include "comm_mask.h"
#include "cliapi.h"
#include "cli_web_mgr_api.h"
#include "config_script_api.h"
#include "datatypes.h"
#ifndef _L7_OS_LINUX_
  #include <inetLib.h>    /* for converting from IP to L7_int32 */
#endif /* _L7_OS_LINUX_ */

#include "clicommands_debug.h"
#include "cli_show_running_config.h"
#ifdef L7_BGP_PACKAGE
#include "usmdb_bgp4_api.h"
#endif
#ifdef L7_SFLOW_PACKAGE
#include "usmdb_sflow.h"
#endif/*sflow package */
#ifdef L7_ISDP_PACKAGE
#include "usmdb_isdp_api.h"
#endif /* L7_ISDP_PACKAGE */
/*********************************************************************
 * @purpose  To print the running configuration of debugging info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 **********************************************************************/
L7_RC_t cliRunningConfigDebuggingInfo(EwsContext ewsContext)
{
  L7_uint32 val;
  L7_RC_t ret_val = L7_SUCCESS;
  L7_BOOL txFlag,rxFlag;
  L7_int32 current_handle;
  L7_uchar8 family = L7_AF_INET;
 #ifdef L7_ISDP_PACKAGE
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;
#endif /* L7_ISDP_PACKAGE */

  /*---------------------------------------------*/
  /*    debug console                            */
  /*---------------------------------------------*/
  current_handle = cliCurrentHandleGet ();
  val = cliWebDebugTraceDisplayModeGet(current_handle);

  cliShowCmdTrue(ewsContext,val,L7_FALSE,pStrInfo_base_DebugConsole_1);

  /*---------------------------------------------*/
  /*    dot3ad                                   */
  /*---------------------------------------------*/
  val = usmDbDot3adPacketDebugTraceFlagGet();

  cliShowCmdTrue(ewsContext,val,L7_FALSE,pStrInfo_base_DebugLacpPkt_1);

  /*---------------------------------------------*/
  /*    dot1s                                    */
  /*---------------------------------------------*/
  usmDbDot1sPacketDebugTraceFlagGet(&txFlag, &rxFlag);
  if(txFlag == L7_TRUE && rxFlag == L7_TRUE)
  {
    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DebugSpanTreeBpdu);
  }
  else if(rxFlag == L7_TRUE)
  {
    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DebugSpanTreeBpduReceive);
    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugSpanTreeBpduTx);
  }
  else if(txFlag == L7_TRUE)
  {
    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugSpanTreeBpduReceive);
    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DebugSpanTreeBpduTx);
  }
  else if(EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
  {
    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugSpanTreeBpduReceive);
    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugSpanTreeBpduTx);
  }

  /*---------------------------------------------*/
  /*    igmp snooping                            */
  /*---------------------------------------------*/
  family = L7_AF_INET;
  if (usmDbSnoopPacketDebugTraceFlagGet(&txFlag,&rxFlag, family) == L7_SUCCESS)
  {
    if(txFlag == L7_TRUE && rxFlag == L7_TRUE)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DebugIgmpsnoopingPkt);
    }
    else if(rxFlag == L7_TRUE)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DebugIgmpsnoopingPktReceive);
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugIgmpsnoopingPktTx);
    }
    else if(txFlag == L7_TRUE)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugIgmpsnoopingPktReceive);
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DebugIgmpsnoopingPktTx);
    }
    else if(EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugIgmpsnoopingPktReceive);
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugIgmpsnoopingPktTx);
    }
  }

  /*---------------------------------------------*/
  /*    mld snooping                            */
  /*---------------------------------------------*/
  family = L7_AF_INET6;
  if (usmDbSnoopPacketDebugTraceFlagGet(&txFlag,&rxFlag, family) == L7_SUCCESS)
  {
    if(txFlag == L7_TRUE && rxFlag == L7_TRUE)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DebugMldsnoopingPkt);
    }
    else if(rxFlag == L7_TRUE)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DebugMldsnoopingPktReceive);
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugMldsnoopingPktTx);
    }
    else if(txFlag == L7_TRUE)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugMldsnoopingPktReceive);
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DebugMldsnoopingPktTx);
    }
    else if(EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugMldsnoopingPktReceive);
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoDebugMldsnoopingPktTx);
    }
  }

  /*---------------------------------------------*/
  /*    ping                                     */
  /*---------------------------------------------*/
  val = usmDbPingPacketDebugTraceFlagGet();

  cliShowCmdTrue(ewsContext,val,L7_FALSE,pStrInfo_base_DebugPingPkt_2);

 #ifdef L7_BGP_PACKAGE

  /*---------------------------------------------*/
  /*    bgp                                      */
  /*---------------------------------------------*/
  val = usmDbBgpPacketDebugTraceFlagGet();

  cliShowCmdTrue(ewsContext,val,L7_FALSE,pStrInfo_base_DebugBgpPkt_2);
 #endif /*L7_PACKAGE_BGP*/

 #ifdef L7_SFLOW_PACKAGE
 /*---------------------------------------------*/
 /*    sFlow                                    */
 /*---------------------------------------------*/
  if (usmDbComponentPresentCheck(L7_NULL, L7_SFLOW_COMPONENT_ID))
  {
    usmDbsFlowPacketDebugTraceFlagGet(&txFlag); 
    if (txFlag == L7_TRUE)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DebugsFlowPacket);
    }
  }
 #endif

 #ifdef L7_ROUTING_PACKAGE

  /*---------------------------------------------*/
  /*    ospf                                     */
  /*---------------------------------------------*/
 #ifdef L7_OSPF_PACKAGE
  val = usmDbOspfPacketDebugTraceFlagGet();

  cliShowCmdTrue(ewsContext,val,L7_FALSE,pStrInfo_base_DebugOspfPkt_2);
 #endif

  /*---------------------------------------------*/
  /*    rip                                      */
  /*---------------------------------------------*/
 #ifdef L7_RIP_PACKAGE
  val = usmDbRipMapPacketDebugTraceFlagGet();

  cliShowCmdTrue(ewsContext,val,L7_FALSE,pStrInfo_base_DebugRipPkt_2);
 #endif

 #endif /*L7_PACKAGE_ROUTING*/

  /*---------------------------------------------*/
  /*    isdp                                     */
  /*---------------------------------------------*/
 #ifdef L7_ISDP_PACKAGE

  /* Debug Trace Packet Tx mode */
  memset (buf, 0, sizeof(buf));
  rc = usmdbIsdpDebugTracePacketTxModeGet(&val);
  if (val != FD_ISDP_DEFAULT_TX_TRACE_MODE)
  {
    if(val == L7_ENABLE)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DebugTracePacketTxEnbl);
      EWSWRITEBUFFER(ewsContext, buf);
    }
    else
    {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DebugNoTracePacketTxEnbl);
        EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* Debug Trace Packet Rx mode */
  memset (buf, 0, sizeof(buf));
  rc = usmdbIsdpDebugTracePacketRxModeGet(&val);
  if (val != FD_ISDP_DEFAULT_RX_TRACE_MODE)
  {
    if(val == L7_ENABLE)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DebugTracePacketRxEnbl);
      EWSWRITEBUFFER(ewsContext, buf);
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DebugNoTracePacketRxEnbl);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* Debug Trace Events mode */
  memset (buf, 0, sizeof(buf));
  rc = usmdbIsdpDebugTraceEventsModeGet(&val);
  if (val != FD_ISDP_DEFAULT_EVENTS_TRACE_MODE)
  {
    if(val == L7_ENABLE) 
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DebugTraceEventsEnbl);
      EWSWRITEBUFFER(ewsContext, buf);
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DebugNoTraceEventsEnbl);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

 #endif /* L7_ISDP_PACKAGE */

  return ret_val;
}
