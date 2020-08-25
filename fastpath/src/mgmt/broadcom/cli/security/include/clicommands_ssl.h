/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/security/ssl/clicommands_ssl.h
*
* @purpose header for clicommands_ssl.c
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

#ifndef CLICOMMANDSSSL_H
#define CLICOMMANDSSSL_H

const char *commandIpHttpSecureServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpHttpSecureProtocol(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const char *commandIpHttpSecurePort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandIpHttpSecureMaxSessions(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpHttpSecureHardTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpHttpSecureSoftTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

void buildTreePrivIpHttpSecure(EwsCliCommandP depth1);

void buildTreePrivShowIpHttp(EwsCliCommandP depth1);
void buildTreePrivIpHttp(EwsCliCommandP depth1);

void cliCryptoCertificateTree(EwsCliCommandP depth2);

const L7_char8 *commandCryptoCertGenerate(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 **argv, L7_uint32 index);

#endif

