/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/clicommands_tunnel.h
 *
 * @purpose Tunneling Macros and Function definitions.
 *
 * @component user interface
 *
 *
 * @create  07/14/2005
 *
 * @author  Ravi Saladi
 *
 *
 * @end
 *
 **********************************************************************/

#ifdef L7_IPV6_PACKAGE

#ifndef CLICOMMANDS_TUNNEL_H
#define CLICOMMANDS_TUNNEL_H

/*******************************************************************
** Includes
*******************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "nimapi.h"
#include "usmdb_ip_api.h"

#include "ew_types.h"
#include "ews.h"
#include "ews_def.h"
#include "cliutil.h"
#include "cliapi.h"
#include "clicommands_l3.h"

#include "datatypes.h"
#include "usmdb_util_api.h"
#include "l3end_api.h"
#include "l3_commdefs.h"
#include "l3_defaultconfig.h"
#include "usmdb_rlim_api.h"
#include "clicommands_card.h"
#include "osapi_support.h"

#ifdef L7_IPV6_PACKAGE
  #include "clicommands_ipv6.h"
#endif
#include "clicommands_tunnel.h"

/*******************************************************************
* Macro Definations
*******************************************************************/
#define L7_CLIMAX_TUNNELID           L7_MAX_NUM_TUNNEL_INTF-1
#define L7_CLIMIN_TUNNELID           0

/*******************************************************************
* IPv6 Tunneling Function prototypes
*******************************************************************/
void buildTreeShowTunnelInfo(EwsCliCommandP depth1);
void buildTreeTunnelInterfaceConfiguration(void);
void buildTreeInterfaceIpv6TunnelDestination(EwsCliCommandP depth1);
const L7_char8 *commandShowTunnelInfo(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliTunnelConfigPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliTunnelConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIPV6TunnelSource(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTunnelDestination(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTunnelModeIpv6Ip(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
L7_RC_t cliValidateTunnelId(EwsContext ewsContext, L7_char8 * buf, L7_uint32 * pVal);

/*******************************************************************
* End of the file
*******************************************************************/
#endif
#endif
