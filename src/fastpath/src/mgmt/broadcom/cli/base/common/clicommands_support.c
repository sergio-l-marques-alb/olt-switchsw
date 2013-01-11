/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_support.c
*
* @purpose Support commands
*
* @component User Interface
*
* @comments contains the code to build the root of the tree
* @comments also contains functions that allow tree navigation
*
* @create  07/14/2006
*
* @author  cpverne
* @end
*
*********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "clicommands_support.h"
#include "clicommands_usermgr.h"
#include "usmdb_support.h"

#ifdef L7_WIRELESS_PACKAGE
#include "clicommands_support_wireless.h"
#endif /* L7_WIRELESS_PACKAGE */

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#include "clicommands_support_cp.h"
#endif /* L7_CAPTIVEPORTAL_PACKAGE */
/*********************************************************************
*
* @purpose  Build the tree nodes for Privilege Exec mode Support commands
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes creates the tree nodes for Privilege Exec mode Support commands
*
* @end
*
*********************************************************************/
void cliTreePrivilegedSupport(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2,depth3;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Support, pStrInfo_base_SupportSys, 
                               cliSupportMode, 1, L7_NODE_HIDDEN_SUPPORT_NODE);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NODE_TYPE_MODE, L7_GLOBAL_SUPPORT_MODE);
}

/*********************************************************************
*
* @purpose  To build the Global Support tree
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*
*********************************************************************/
void buildTreeSupport()
{
  EwsCliCommandP depth1,depth2, depth3, depth4 = 0;
  EwsCliCommandP depth5;

  depth1 = ewsCliAddNode ( NULL, NULL, NULL, cliSupportMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_GLOBAL_SUPPORT_MODE, depth1);

  /*debug clear*/
  depth2 = ewsCliAddNode(depth1, pStrErr_common_CfgApProfileClrCmd, pStrInfo_base_SupportSysNoAll,
                   commandSupportClear, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);

  buildTreePrivSupportConsoleBreakIn(depth1);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, 
                           NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                           NULL, L7_NO_OPTIONAL_PARAMS);

  /*debug save*/
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Save_1, pStrInfo_base_SupportSave, 
                 commandSupportSave, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef LVL7_DEBUG_BREAKIN
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Show_1, pStrInfo_base_Show, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Console, pStrInfo_base_ShowConsole, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Breakin, pStrInfo_base_ShowBreakin, 
                         commandSupportShowConsoleBreakin, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS); 
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Usrs_1, pStrInfo_base_ShowUsrs, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LoginStats, pStrInfo_base_ShowUsrsLoginStats, 
                         commandShowUsersLoginStats, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
#else
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Show_1, pStrInfo_base_Show, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Usrs_1, pStrInfo_base_ShowUsrs, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LoginStats, pStrInfo_base_ShowUsrsLoginStats, 
                         commandShowUsersLoginStats, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
#endif
   

  /* Only display the node if a snapshot is registered for any category */
  if (usmDbSupportDebugCategoryRegistrationExists() == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_Snapshot, pStrInfo_base_SupportSnap_1, 
                           NULL, L7_NO_OPTIONAL_PARAMS);
    if (usmDbSupportDebugCategoryIsPresent(SUPPORT_CATEGORY_SYSTEM,0) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_base_BootSys, pStrInfo_base_SupportSnap_1,
               commandSupportSnapshotSystem,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                             NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbSupportDebugCategoryIsPresent(SUPPORT_CATEGORY_ROUTING,0) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_Routing_1, pStrInfo_base_SupportSnap,
               commandSupportSnapshotRouting,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                             NULL, L7_NO_OPTIONAL_PARAMS);
    }
    if (usmDbSupportDebugCategoryIsPresent(SUPPORT_CATEGORY_IPMCAST,0) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_Mcast_3, pStrInfo_base_SupportSnapMcast,
               commandSupportSnapshotMulticast,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                             NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }
  #ifdef L7_WIRELESS_PACKAGE
  /* support wireless ... */
  buildTreePrivledgedSupportWireless(depth1);
  #endif /*L7_WIRELESS_PACKAGE*/

  #ifdef L7_CAPTIVE_PORTAL_PACKAGE
   /* support captive-portal */
  buildTreePrivledgedSupportCaptivePortal(depth1);
  #endif /* L7_CAPTIVEPORTAL_PACKAGE */

}

/*********************************************************************
*
* @purpose  To build the console break-in Support tree
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*
*********************************************************************/
void buildTreePrivSupportConsoleBreakIn(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;

#ifdef LVL7_DEBUG_BREAKIN
  EwsCliCommandP depth4, depth5;
#endif

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Console, pStrInfo_base_SupportConsole, 
           commandSupportConsole, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#ifdef LVL7_DEBUG_BREAKIN
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Breakin, pStrInfo_base_breakin_Help, 
           commandSupportConsoleBreakin, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 
                         2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_String, pStrInfo_common_StringHelp, 
           commandSupportConsoleBreakinString, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
#endif
}

