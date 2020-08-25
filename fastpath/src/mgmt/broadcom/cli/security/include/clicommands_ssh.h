/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/security/ssh/clicommands_ssh.h
*
* @purpose header for clicommands_ssh.c
*
* @component user interface
*
* @comments none
*
* @create  09/12/03
*
* @author  Kim Mans
* @end
*
**********************************************************************/

#ifndef CLICOMMANDSSSH_H
#define CLICOMMANDSSSH_H


const char *commandIpSshServerEnable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpSshProtocol(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandShowIpSsh(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

void buildTreePrivShowIpSsh(EwsCliCommandP depth1);
void buildTreePrivIpSsh(EwsCliCommandP depth1);
void buildTreePrivSWMgmtSSHConConf(EwsCliCommandP depth1);
void buildTreeGlobalCryptoSsh(EwsCliCommandP depth2);

const L7_char8  *commandCryptoGenerateSshRSAKey(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandCryptoGenerateSshDSAKey(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);
#endif
