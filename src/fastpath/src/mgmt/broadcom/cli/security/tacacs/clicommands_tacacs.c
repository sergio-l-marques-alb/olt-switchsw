/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_tacacs.c
 *
 * @purpose create the tree for CLI TACACS+
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/03/2005
 *
 * @author  Rama Krishna Hazari
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "clicommands_tacacs.h"

/*********************************************************************
* @purpose  To Build the TACACS host tree
*
* @param    EwsCliCommandP depth1		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeGlobalConfigTacacs(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_TacacsSrvr_1, pStrInfo_base_Tacacs, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Host_2, pStrInfo_base_TacacsHost, cliTacacsMode,
                          2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode (depth3, pStrInfo_common_HostAddr,
                          pStrInfo_base_TacacsHostAddr, NULL, 2,
                          L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                          2, L7_NODE_TYPE_MODE, L7_TACACS_MODE);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Key_3, pStrInfo_base_TacacsKeyString,
                          commandTACACSGblKey, 2, L7_NO_COMMAND_SUPPORTED,
                          L7_STATUS_BOTH);
  depth4 = ewsCliAddNode (depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                          L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode (depth3, pStrInfo_base_KeyString,
                          pStrInfo_base_TacacsGlobalKeyStringLen, NULL,
                          L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                          L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode (depth3, pStrInfo_base_Encrypted,
                          pStrInfo_base_TacacsEncryptedKey, commandTACACSGblKeyEncrypted,
                          L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode (depth4, pStrInfo_base_KeyString,
                          pStrInfo_base_TacacsEncryptedKeyString, NULL,
                          3, L7_NODE_STRING_RANGE, CLITACACS_KEY_STRING_LEN*2, CLITACACS_KEY_STRING_LEN*2);
  depth6 = ewsCliAddNode (depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                          L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Timeout_2, pStrInfo_base_TacacsTimeout,
                          commandTACACSGblTimeout, 2, L7_NO_COMMAND_SUPPORTED,
                          L7_STATUS_BOTH);
  depth4 = ewsCliAddNode (depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                          L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode (depth3, pStrInfo_base_Timeout,
                          pStrInfo_base_TacacsGlobalTimeoutRange, NULL,
                          L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                          L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
* @purpose  To Build the tacacs tree in global configuration mode
*
* @param    EwsCliCommandP depth1		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeTacacsConfig(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;/*, depth6, depth7, depth8;
                                           EwsCliCommandP depth9, depth10;*/

  /* Building the LLDP Command Tree */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Key_3, pStrInfo_base_TacacsKeyString,
                         commandTACACSKey, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_KeyString,
                         pStrInfo_base_TacacsKeyStringLen, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_base_Encrypted,
                          pStrInfo_base_TacacsEncryptedKey, commandTACACSKeyEncrypted,
                          L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode (depth3, pStrInfo_base_KeyString,
                          pStrInfo_base_TacacsEncryptedKeyString, NULL,
                          L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                          L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Port_4, pStrInfo_base_TacacsPort,
                         commandTACACSPort, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_PortNum, pStrInfo_base_TacacsPortRange,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_SwitchPri_1, pStrInfo_base_TacacsPri,
                         commandTACACSPriority, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Pri, pStrInfo_base_TacacsPriRange,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Timeout_2, pStrInfo_base_TacacsTimeout,
                         commandTACACSTimeout, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Timeout, pStrInfo_base_TacacsTimeoutRange,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
* @purpose  To Build the tacacs show tree in enable mode
*
* @param    EwsCliCommandP depth1		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeTacacsShow(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  /* Building the TACACS show Command Tree */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_TacacsAuth, pStrInfo_base_TacacsShow,
                         commandTACACSShow, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_HostAddr, pStrInfo_common_ApProfileRadiusIpAddr,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
}
