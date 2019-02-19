/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/security/ssl/clicommands_ssl.c
*
* @purpose create the cli commands for http secure-server Security Management functions
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
#include "clicommands_ssl.h"

#include "datatypes.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  Build the IP HTTP Secure-Server tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreePrivIpHttpSecure(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7;

  depth4 = ewsCliAddNode (depth3, pStrInfo_security_SecurePort_1,pStrInfo_security_IpHttpSecurePortCfg, commandIpHttpSecurePort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Portid, pStrInfo_security_IpHttpSecurePort, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode (depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode (depth3, pStrInfo_security_SecureProto,pStrInfo_security_IpHttpSecureProtoCfg, commandIpHttpSecureProtocol, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode (depth4, pStrInfo_security_Protolevel1, pStrInfo_security_IpHttpSecureProto, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode (depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode (depth5, pStrInfo_security_Protolevel2, pStrInfo_security_IpHttpSecureProto2, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode (depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode (depth3, pStrInfo_security_SecureSrvr,pStrInfo_security_IpHttpSecureSrvrCfg, commandIpHttpSecureServer, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode (depth3, pStrInfo_security_SecureSession,pStrInfo_security_IpHttpSecureSession, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode (depth4, pStrInfo_common_HardTimeout,pStrInfo_security_IpHttpSecureHardTimeoutCfg, commandIpHttpSecureHardTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode (depth5, NULL, pStrInfo_security_IpHttpSecureHardTimeoutCfg, NULL,
                          3, L7_NODE_UINT_RANGE, FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_MIN, FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_MAX);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Maxsessions,pStrInfo_security_IpHttpSecureMaxSessionCfg, commandIpHttpSecureMaxSessions, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode (depth5, NULL, pStrInfo_common_IpHttpMaxSessionCfg, NULL,
                          3, L7_NODE_UINT_RANGE, 0, FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode (depth4, pStrInfo_common_SoftTimeout,pStrInfo_security_IpHttpSecureSoftTimeoutCfg, commandIpHttpSecureSoftTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode (depth5, NULL, pStrInfo_security_IpHttpSecureSoftTimeoutCfg, NULL,
                          3, L7_NODE_UINT_RANGE, FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_MIN, FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_MAX);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Crypto Certificate tree.
*
*
* @param EwsCliCommandP depth3
*
* @returntype void
*
* @end
*
*********************************************************************/
void cliCryptoCertificateTree(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3,depth4, depth5;

  depth3= ewsCliAddNode(depth2, pStrInfo_security_Certificate_1, pStrInfo_security_CryptoCertificate,
          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

 depth4= ewsCliAddNode(depth3, pStrInfo_security_Generate, pStrInfo_security_CryptoCertificateGenerate,
          commandCryptoCertGenerate, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

 depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
  L7_NO_OPTIONAL_PARAMS);
}


