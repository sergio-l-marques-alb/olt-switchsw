/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/clicommands_loopback.h
 *
 * @purpose create the cli commands for Routing functions
 *
 * @component user interface
 *
 *
 * @create  07/14/2005
 *
 * @author  Ravi Saladi
 *
 * @end
 *
 **********************************************************************/
#ifndef CLICOMMANDS_LOOPBACK_H
#define CLICOMMANDS_LOOPBACK_H

/*******************************************************************
* Macro Definations
*******************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "usmdb_util_api.h"
/* layer 3 includes           */
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

/*********************************************************************
* Macros
*********************************************************************/
#define L7_CLIMAX_LOOPBACKID           L7_MAX_NUM_LOOPBACK_INTF-1
#define L7_CLIMIN_LOOPBACKID           0

/*********************************************************************
*  Loopback Help Messages
*********************************************************************/

/*********************************************************************
*  Loopback Interface Function declarations
*********************************************************************/
void buildTreeLoopbackGlobalConfiguration(EwsCliCommandP depth1);
void buildTreeShowLoopbackInfo(EwsCliCommandP depth1);
void buildTreeLoopbackInterfaceConfiguration(void);

const L7_char8 *commandShowLoopbackInfo(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliLoopbackConfigPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliLoopbackConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
L7_RC_t cliValidateLoopbackId(EwsContext ewsContext, L7_char8 * buf, L7_uint32 * pVal);

/*******************************************************************
* End of the file
*******************************************************************/
 #endif
