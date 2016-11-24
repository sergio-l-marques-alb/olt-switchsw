/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/security/ssh/clicommands_ssh.c
*
* @purpose create the cli commands for ssh Security Management functions
*
* @component user interface
*
*
* @create  09/12/2003
*
* @author  Kim Mans
*
*
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "cliapi.h"
#include "usmdb_util_api.h"

#include "osapi.h"
#include "nimapi.h"
#include "usmdb_ip_api.h"
#include "clicommands_ssh.h"
#include "usmdb_sshd_api.h"

#include "datatypes.h"
#include "usmdb_util_api.h"

void cliCryptoCertificateTree(EwsCliCommandP depth2);
/*********************************************************************
*
* @purpose  Build the Routing Interface VRRP tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreePrivIpSsh(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  /* Router Config mode*/

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_Ssh_1,pStrInfo_security_IpSshCfg, commandIpSshServerEnable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode (depth3, pStrInfo_common_Proto_1, pStrInfo_security_IpSshProtoCfg, commandIpSshProtocol, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode (depth4, pStrInfo_security_Protolevel1, pStrInfo_security_IpSshProto, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode (depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode (depth5, pStrInfo_security_Protolevel2, pStrInfo_security_IpSshProto2, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode (depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Srvr_1,pStrInfo_security_IpSshCfg, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Enbl_2,pStrInfo_security_EnableIpSshServer, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}
/*********************************************************************
*
* @purpose  Build the Show IP SSH tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreePrivShowIpSsh(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5;

/* DEPTH3 = "ip " */
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Ssh_1, pStrInfo_security_ShowIpSsh, commandShowIpSsh, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the ssh connections tree
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/

void buildTreePrivSWMgmtSSHConConf(EwsCliCommandP depth3)
{

    EwsCliCommandP depth4, depth5, depth6, depth7;

  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Sshcon, pStrInfo_security_SshCon, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Maxsessions, pStrInfo_security_SshConMaxSessions, commandSshConMaxSessions, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_security_Range0to5, pStrInfo_security_SshConMaxSessions, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Timeout_2, pStrInfo_security_SshConTimeout, commandSshConTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range1to160, pStrInfo_security_SshConTimeout, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}
/*********************************************************************
*
* @purpose  Build the Global Configuration SSH Crypto Tree
*
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeGlobalCryptoSsh(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  depth2 = ewsCliAddNode(depth1, pStrInfo_security_Crypto, pStrInfo_security_CryptoKeyManagement, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  cliCryptoCertificateTree(depth2);

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_Key, pStrInfo_security_CryptoKeyManagement, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Generate, pStrInfo_security_KeyGenerate,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Dsa, pStrInfo_security_GenerateDsa, commandCryptoGenerateSshDSAKey,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Rsa, pStrInfo_security_GenerateRsa, commandCryptoGenerateSshRSAKey,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
