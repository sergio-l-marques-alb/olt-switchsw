/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/radius/clicommands_radius.h
 *
 * @purpose header for radius commands in clicommands.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  4/08/2003
 *
 * @author  jflanagan
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_RADIUS_H
#define CLICOMMANDS_RADIUS_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "l7_common.h"

void buildTreeGlobalRadiusConfig(EwsCliCommandP depth1);
void buildTreePrivilegedRadiusShowRadius(EwsCliCommandP depth2);
void buildTreePrivilegedRadiusClear(EwsCliCommandP depth2);

/**********************************************************************/

/**********************************************************************/

/* CONFIG */
const L7_char8 *commandRadiusAccountingMode(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandRadiusServerHost(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandRadiusServerKey(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandRadiusServerKeyEncrypted(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandRadiusServerPrimary(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandRadiusServerRetransmit(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandRadiusServerTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

const L7_char8 *commandRadiusServerMsgAuth(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandRadiusAttribute4Set(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

/* SHOW */
const L7_char8 *commandShowRadiusAccounting(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandShowRadiusAcctStats(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

void radiusStatus(EwsContext ewsContext,L7_uint32 unit, L7_char8 *name, 
                     L7_char8 *host, L7_IP_ADDRESS_TYPE_t addrType,
                     L7_uint32 ipAddr,L7_BOOL nameBasedDisplay);

void radiusAuthServerDisplay(EwsContext ewsContext,L7_char8 *host, 
                     L7_IP_ADDRESS_TYPE_t addrType, L7_uint32 ipAddr,
                     L7_char8 *dnsAddrString, L7_char8 *ipAddrString);

void radiusAccountingStatus(EwsContext ewsContext,L7_uint32 unit, 
                     L7_char8 *name, L7_char8 *host,
                     L7_IP_ADDRESS_TYPE_t addrType,
                     L7_uint32 ipAddr,L7_BOOL nameBasedDisplay);

void radiusAcctServerDisplay(EwsContext ewsContext,L7_char8 *host, 
                     L7_IP_ADDRESS_TYPE_t addrType,L7_uint32 ipAddr);
                     

const L7_char8 *commandShowRadiusServers(EwsContext ewsContext,L7_uint32 argc,
                     const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandShowRadiusStatistics(EwsContext ewsContext,
                     L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandShowRadiusStats(EwsContext ewsContext,L7_uint32 argc,
                     const L7_char8 * * argv,L7_uint32 index);

/* CLEAR */
const L7_char8 *commandClearRadiusStatistics(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
/**********************************************************************/

#endif
