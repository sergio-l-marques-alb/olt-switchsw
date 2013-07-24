/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config_dhcp_filtering.c
 *
 * @purpose show running config commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  01/07/2003
 *
 * @author  ARR
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "dot1s_exports.h"
#include "cli_web_exports.h"
#include "usmdb_dot1s_api.h"
#include "comm_mask.h"
#include "config_script_api.h"
#include "datatypes.h"
#ifndef _L7_OS_LINUX_
  #include <inetLib.h>    /* for converting from IP to L7_int32 */
#endif /* _L7_OS_LINUX_ */

#include "usmdb_dhcps_api.h"
#ifdef L7_DHCP_FILTER_PACKAGE
#include "usmdb_dhcp_filtering.h"
#include "clicommands_dhcp_filtering.h"
#endif
#include "cli_show_running_config.h"

/*********************************************************************
 * @purpose  To print the running configuration of Spanning Tree Features
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/

L7_RC_t cliRunningSpanningTreeInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;
  L7_ushort16 val16;
  L7_uint32 tempMstid;
  L7_uint32 mstid;
  L7_uint32 vlanId = 0;
  L7_uint32 *mstVids, mstVidsCount = 0;  
  L7_BOOL boolVal;

  /* spanningtree port config info */
  /* Spanning Tree Mode */
  if ( usmDbDot1sModeGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdEnable(ewsContext,val,FD_DOT1S_MODE,pStrInfo_common_SpanTree_2);
  }

  /* spanning-tree configuration name <name> */
  if ( usmDbDot1sConfigNameGet(unit, buf) == L7_SUCCESS )
  {
    cliShowCmdQuotedStrcmp (ewsContext, buf, FD_DOT1S_DEFAULT_CONFIG_NAME, pStrInfo_base_SpanTreeCfgName_1);
  }

  /* spanning-tree configuration revision <0-65535> */
  if (usmDbDot1sConfigRevisionGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext,val,FD_DOT1S_DEFAULT_CONFIG_REVISION,pStrInfo_base_SpanTreeCfgRevision_1);
  }

  if ( usmDbDot1sBpduGuardGet(unit, &boolVal) == L7_SUCCESS )
  {
    cliShowCmdTrue (ewsContext, boolVal, L7_FALSE, pStrInfo_base_SpanTreeBpduguard);
  }

  if ( usmDbDot1sBpduFilterGet(unit, &boolVal) == L7_SUCCESS )
  {
    cliShowCmdTrue (ewsContext, boolVal, L7_FALSE, pStrInfo_base_SpanTreeBpdufilterDefl);
  }

  if ( usmDbDot1sCistBridgeMaxAgeGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext,val,FD_DOT1S_BRIDGE_MAX_AGE,pStrInfo_base_SpanTreeMaxAge_1);
  }

  if ( usmDbDot1sCistBridgeFwdDelayGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt(ewsContext,val,FD_DOT1S_BRIDGE_FWD_DELAY,pStrInfo_base_SpanTreeForwardTime_1);
  }

  if ( usmDbDot1sBridgeMaxHopGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt (ewsContext, val, FD_DOT1S_BRIDGE_MAX_HOP, pStrInfo_base_SpanTreeMaxHops);
  }

  if ( usmDbDot1sBridgeTxHoldCountGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt (ewsContext, val, FD_DOT1S_BRIDGE_TX_HOLD_COUNT, pStrInfo_base_SpanTreeHoldCount_1);
  }

  if ( usmDbDot1sCistBridgeHelloTimeGet(unit, &val) == L7_SUCCESS )
  {
    cliShowCmdInt (ewsContext, val, FD_DOT1S_BRIDGE_HELLO_TIME, pStrInfo_base_SpanTreeHelloTime_1);
  }

  if ( usmDbDot1sForceVersionGet(unit, &val) == L7_SUCCESS )
  {
    if (( val != FD_DOT1S_FORCE_VERSION ) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      if (val == DOT1S_FORCE_VERSION_DOT1D )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SpanTreeForceversion8021d);
        EWSWRITEBUFFER(ewsContext, stat);
      }
      else if (val == DOT1S_FORCE_VERSION_DOT1W)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SpanTreeForceversion8021w);
        EWSWRITEBUFFER(ewsContext, stat);
      }
      else if (val == DOT1S_FORCE_VERSION_DOT1S )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SpanTreeForceversion8021s);
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  rc=usmDbDot1sInstanceFirstGet(unit, &mstid) ;
  do
  {
    cliShowCmdInt (ewsContext, mstid, L7_DOT1S_CIST_INSTANCE, pStrInfo_base_SpanTreeMstInst);

    /* Bridge Priority */
    if ( usmDbDot1sMstiBridgePriorityGet(unit, mstid, &val16) == L7_SUCCESS )
    {
      if ((val16 != FD_DOT1S_BRIDGE_PRIORITY)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_SpanTreeMstPri, mstid,val16);
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }

    if(mstid != L7_DOT1S_CIST_INSTANCE)
    {
      tempMstid = mstid;
      vlanId = 0;
      mstVidsCount = 0;
      mstVids = (L7_uint32 *)osapiMalloc(L7_CLI_WEB_COMPONENT_ID, sizeof(L7_uint32)*L7_MAX_VLANS);
      if (mstVids == L7_NULLPTR)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, stat, sizeof(stat), pStrInfo_common_AllocateMemoryToPerformOperation);
        EWSWRITEBUFFER(ewsContext, stat);  
        continue;     
      }           
      rc = usmDbDot1sInstanceVlanNextGet(unit, mstid, &tempMstid, vlanId, &vlanId);
      while(rc == L7_SUCCESS && tempMstid == mstid)
      {
        mstVids[mstVidsCount] = vlanId;
        mstVidsCount++;
        rc = usmDbDot1sInstanceVlanNextGet(unit, mstid, &tempMstid, vlanId, &vlanId);
      }     
      if(mstVidsCount > 0)
      {
        osapiSnprintf (stat, sizeof(stat), pStrInfo_base_SpanTreeMstVlan, mstid);
        displayRangeFormat(ewsContext, mstVids, mstVidsCount, stat);
      }   
      osapiFree(L7_CLI_WEB_COMPONENT_ID, mstVids);   
    }
  }
  while (usmDbDot1sInstanceNextGet(unit, mstid, &mstid) == L7_SUCCESS);

  return L7_SUCCESS;
}
