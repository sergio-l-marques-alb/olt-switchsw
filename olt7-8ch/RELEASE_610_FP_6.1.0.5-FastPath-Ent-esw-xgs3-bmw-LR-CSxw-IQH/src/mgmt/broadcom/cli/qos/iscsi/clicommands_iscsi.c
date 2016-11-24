/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2008
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/iscsi/clicommands_iscsi.c
 *
 * @purpose create iscsi commands
 *
 * @component user interface
 *
 * @comments contains the code to build the root of the tree
 * @comments also contains functions that allow tree navigation
 *
 * @create  11/24/2008
 *
 * @author Rajakrishna
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "cliapi.h"
#include "clicommands_iscsi.h"
#include "clicommands_card.h"
#include "usmdb_util_api.h"
#include "cos_exports.h"
#include "iscsi_exports.h"

/*********************************************************************
*
* @purpose  Build the tree nodes for Global iscsi commands
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobalIscsi(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7,depth8,depth9,depth10;
  EwsCliCommandP depthn,depthnplus1,depthTemp,depthNext;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;


  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_Iscsi_1, pStrInfo_qos_IscsiHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    /*enable */
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_IscsiEnable, pStrInfo_qos_IscsiEnableHelp, commandIscsiEnable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    /* iscsi target port */

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_IscsiTarget, pStrInfo_qos_IscsiTargetHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_IscsiPort, pStrInfo_qos_IscsiPortHelp, commandIscsiTargetPort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_IscsiPortVal1, pStrInfo_qos_IscsiPortValHelp, NULL, 5,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_UINT_RANGE, 1, 65535);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_qos_IscsiAddress, pStrInfo_qos_IscsiAddressHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_qos_IscsiAddressVal, pStrInfo_qos_IscsiAddressValHelp, NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_qos_IscsiName, pStrInfo_qos_IscsiNameHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth9 = ewsCliAddNode(depth8, pStrInfo_qos_IscsiNameVal, pStrInfo_qos_IscsiNameValHelp, NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_qos_IscsiName, pStrInfo_qos_IscsiNameHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_qos_IscsiNameVal, pStrInfo_qos_IscsiNameValHelp, NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depthNext = depth5;
  for(i=2;i<=16;i++)
  {
    depthn = depthNext;
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_IscsiPortValn, i);
    depthnplus1 = ewsCliAddNode(depthn, buf, pStrInfo_qos_IscsiPortValHelp, NULL, 5,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_UINT_RANGE, 1, 65535);
    depthn = depthnplus1;
    depthNext = depthn;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depthTemp = depthn;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_IscsiAddress, pStrInfo_qos_IscsiAddressHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depthn = depthnplus1;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_IscsiAddressVal, pStrInfo_qos_IscsiAddressValHelp, NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depthn = depthnplus1;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_IscsiName, pStrInfo_qos_IscsiNameHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depthn = depthnplus1;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_IscsiNameVal, pStrInfo_qos_IscsiNameValHelp, NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depthn = depthnplus1;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depthn = depthTemp;

    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_IscsiName, pStrInfo_qos_IscsiNameHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depthn = depthnplus1;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_IscsiNameVal, pStrInfo_qos_IscsiNameValHelp, NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depthn = depthnplus1;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

/* iscsi cos */
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_IscsiCos, pStrInfo_qos_IscsiCosHelp, commandIscsiCos, 2, 
                         L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_IscsiVpt, pStrInfo_qos_IscsiVptHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_IscsiVptVal, pStrInfo_qos_IscsiVptValHelp, NULL, 3, L7_NODE_UINT_RANGE, L7_DOT1P_MIN_PRIORITY, L7_DOT1P_MAX_PRIORITY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_qos_IscsiRemark, pStrInfo_qos_IscsiRemarkHelp, NULL, 2, 
                       L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_IscsiDscp, pStrInfo_qos_IscsiDscpHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_IscsiDscpVal,pStrInfo_qos_IscsiDscpValHelp, NULL, 3, L7_NODE_UINT_RANGE, L7_ACL_MIN_DSCP, L7_ACL_MAX_DSCP);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_qos_IscsiRemark, pStrInfo_qos_IscsiRemarkHelp, NULL, 2, 
                       L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);


        /* iscsi aging time */
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_IscsiAging, pStrInfo_qos_IscsiAgingHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_IscsiTime, pStrInfo_qos_IscsiAgingHelp, commandIscsiAgingTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_IscsiTimeVal, pStrInfo_qos_IscsiTimeValHelp, NULL, 3, L7_NODE_UINT_RANGE, ISCSI_SESSION_TIME_OUT_MINUTES_MIN, ISCSI_SESSION_TIME_OUT_MINUTES_MAX);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}


/*********************************************************************
*
* @purpose  Build the tree to display iscsi config
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void  buildTreePrivShowIscsi(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3,depth4,depth5,depth6;

  depth3 = ewsCliAddNode(depth2,pStrInfo_qos_Iscsi_1, pStrInfo_qos_IscsiShowHelp, commandShowIscsi, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  
  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_IscsiSessions, pStrInfo_qos_IscsiSessionsHelp, commandShowIscsiSessions, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_IscsiSessionsDetailed, pStrInfo_qos_IscsiSessionsDetailedHelp, NULL,L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  return;
}

