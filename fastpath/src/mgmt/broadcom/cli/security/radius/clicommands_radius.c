/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/radius/clicommands_radius.c
 *
 * @purpose create the tree for CLI Radius commands
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  22/07/2003
 *
 * @author  chinmoyb
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "clicommands_radius.h"

/*********************************************************************
*
* @purpose  Build the radius configuration commands tree
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
void buildTreeGlobalRadiusConfig(EwsCliCommandP depth1)
{
  /* depth1 = Global Config Mode */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8, depth9, depth10, depth11;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_RadiusAuth, pStrInfo_security_RadiusParam, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Acct, pStrInfo_security_RadiusAcct, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_ApShowRunningMode, pStrInfo_common_ApProfileRadiusAcct, commandRadiusAccountingMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Srvr_1, pStrInfo_common_ApProfileRadiusSrvr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Attr_1, pStrInfo_security_RadiusAttr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Num4, pStrInfo_security_RadiusAttr, commandRadiusAttribute4Set, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_security_Ipaddr_1, pStrInfo_common_ApProfileRadiusIpAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Host_2, pStrInfo_common_ApProfileRadiusSrvrHost, commandRadiusServerHost, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Acct_1, pStrInfo_security_CfgRadiusAcct, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Host, pStrInfo_common_ApProfileRadiusHost, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Port_4,pStrInfo_security_RadiusPort2,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Port,pStrInfo_security_AcctPortRange1And65535,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_secuirty_serverName,pStrInfo_security_serverNameHelp,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_secuirty_serverName1,pStrInfo_security_serverNameHelp1,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Port_4,pStrInfo_security_RadiusPort2,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Port,pStrInfo_security_AcctPortRange1And65535,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth11 = ewsCliAddNode(depth10, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_LogFacilityAuth, pStrInfo_security_RadiusSrvrHostAuth, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Host, pStrInfo_common_ApProfileRadiusHost, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Port_4,pStrInfo_security_RadiusPort1,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Port,pStrInfo_security_RadiusPort,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_secuirty_serverName,pStrInfo_security_serverNameHelp,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_secuirty_serverName1,pStrInfo_security_serverNameHelp1,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Port_4,pStrInfo_security_RadiusPort1,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Port,pStrInfo_security_RadiusPort,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth11 = ewsCliAddNode(depth10, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Key_3, pStrInfo_security_RadiusSrvrKey, commandRadiusServerKey, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Acct_1, pStrInfo_security_RadiusSrvrKeyAcct, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Host, pStrInfo_common_ApProfileRadiusHost, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_Encrypted, pStrInfo_security_RadiusSrvrKeyEncrypted, commandRadiusServerKeyEncrypted, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_base_Passwd, pStrInfo_base_CfgEncrypt, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_LogFacilityAuth, pStrInfo_security_RadiusSrvrKeyAuth, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Host, pStrInfo_common_ApProfileRadiusHost, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_Encrypted, pStrInfo_security_RadiusSrvrKeyEncrypted, commandRadiusServerKeyEncrypted, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_base_Passwd, pStrInfo_base_CfgEncrypt, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Msgauth, pStrInfo_security_RadiusSrvrMsgAuth, commandRadiusServerMsgAuth, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Host, pStrInfo_common_ApProfileRadiusHost, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Primary_1, pStrInfo_security_CfgRadiusSrvrPrimary, commandRadiusServerPrimary, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Host, pStrInfo_common_ApProfileRadiusHost, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_OspfTrapReTx, pStrInfo_security_RadiusSrvrReTx, commandRadiusServerRetransmit, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Retries, pStrErr_security_RadiusSrvrReTxVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Timeout_2, pStrInfo_security_RadiusSrvrTimeout, commandRadiusServerTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Seconds_1, pStrInfo_security_RadiusSrvrTimeoutVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the radius server show commands tree
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
void buildTreePrivilegedRadiusShowRadius(EwsCliCommandP depth2)
{
  /* depth2 = show */
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8;
  /* show radius */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_RadiusAuth, pStrInfo_security_Radius, commandShowRadiusServers, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show radius statistics <dns_name | ipaddr > */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dot1xShowStats, pStrInfo_security_RadiusStats, commandShowRadiusStatistics, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Host, pStrInfo_security_RadiusHost, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show radius statistics name <servername> */
  depth5= ewsCliAddNode(depth4, pStrInfo_secuirty_serverName, pStrInfo_security_serverNameHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_secuirty_serverName1, pStrInfo_security_serverNameHelp1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  
  /* show radius accounting .. */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Acct, pStrInfo_security_ShowAcct,commandShowRadiusAccounting , L7_NO_OPTIONAL_PARAMS);
  /* show radius accounting statistics name <servername> */
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Dot1xShowStats, pStrInfo_security_ShowAcct, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_secuirty_serverName, pStrInfo_security_serverNameHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_secuirty_serverName1, pStrInfo_security_serverNameHelp1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* show radius accounting statistics <dns_name | ipaddr > */  
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Host, pStrInfo_security_ShowAcctHost, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS); 

  /* show radius accounting <dns_name | ipaddr > */  
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Host, pStrInfo_security_ShowAcctHost, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);   
  /* show radius accounting name */    
  depth5 = ewsCliAddNode(depth4, pStrInfo_secuirty_serverName, pStrInfo_security_RadiusAcctSrvrs, NULL, L7_NO_OPTIONAL_PARAMS);  
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* show radius accounting name <servername> */
  depth6 = ewsCliAddNode(depth5, pStrInfo_secuirty_serverName1, pStrInfo_security_serverNameHelp1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);   
  
  /* show radius servers */
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Srvrs, pStrInfo_security_RadiusSrvrs, NULL, L7_NO_OPTIONAL_PARAMS);  
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* show radius servers <ipaddr | dnsname> */
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Host, pStrInfo_common_ApProfileRadiusHost, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* show radius servers name  */
  depth5 = ewsCliAddNode(depth4, pStrInfo_secuirty_serverName, pStrInfo_security_serverNameHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* show radius servers name <servername> */
  depth6 = ewsCliAddNode(depth5, pStrInfo_secuirty_serverName1, pStrInfo_security_serverNameHelp1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  

}

/*********************************************************************
*
* @purpose  Build the radius statistics clear commands tree
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
void buildTreePrivilegedRadiusClear(EwsCliCommandP depth2)
{
  /* depth2 = clear */
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_RadiusAuth, pStrInfo_security_ClrRadius, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dot1xShowStats, pStrInfo_security_ClrRadius, commandClearRadiusStatistics, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
