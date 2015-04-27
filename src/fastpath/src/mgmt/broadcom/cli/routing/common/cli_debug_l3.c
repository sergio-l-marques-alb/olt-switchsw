/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/common/cli_debug_l3.c
 *
 * @purpose Debug commands for CLI for Routing package
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/24/2006
 *
 * @author  Colin Verne
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "l7_common.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>

#include "clicommands_debug_l3.h"

/* NOTE:
 * Functions commandDebugOspfPacket() and commandDebugRipPacket() are moved into
 * routing/ospf and routing/rip directories. Add only IP specific debug
 * functions in this file
 */

