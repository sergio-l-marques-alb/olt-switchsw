/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/routing/cliutil_l3.h
*
* @purpose header for cliutil_l3.c
*
* @component user interface
*
* @comments none
*
* @create   09/16/2002
*
* @author  Jill Flanagan
* @end
*
**********************************************************************/

#ifndef CLIUTIL_L3_H
#define CLIUTIL_L3_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "commdefs.h"
#include "datatypes.h"

L7_RC_t cliValidateRtrIntf(EwsContext ewsContext, L7_uint32 iface);

L7_RC_t cliConvertRouterID(EwsContext ewsContext,
                           const L7_char8 * routerID, L7_uchar8 * routID);

L7_RC_t cliGetUnitSlotPortIntf(EwsContext ewsContext, L7_uint32 intfId,
                               L7_uint32 * intf, L7_uint32 * unit, L7_uint32 * slot, L7_uint32 * port, L7_uint32 intfType);

L7_char8 *cliGetIntfName(L7_uint32 intfNum, L7_uint32 unit, L7_uint32 slot, L7_uint32 port);

L7_BOOL cliIntfIsNamedByUSP(L7_uint32 intfNum);

L7_RC_t cliParseInterface(EwsContext ewsContext, L7_uint32 argc,
                          const L7_char8 **argv, L7_uint32 nextArg,
                          L7_uint32 *intIfNum);

#endif
