/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    lldp_debug.c
* @purpose     LLDP debug functions
* @component   LLDP
* @comments    none
* @create      03/14/2009
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "osapi_support.h"

#include "log.h"
#include "support_api.h"
#include "l7utils_inet_addr_api.h"

#include "lldp.h"
#include "lldp_api.h"
#include "lldp_debug.h"


/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/

static L7_uchar8    lldpDebugFlags;
static L7_BOOL      lldpIsDebugEnabled = L7_FALSE;


/*********************************************************************
* @purpose  Enable Debug Tracing in LLDP
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully enabled.
* @returns  L7_FAILURE  if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t
lldpDebugTraceEnable (void)
{
    if (lldpIsDebugEnabled == L7_TRUE)
    {
        LLDP_DEBUG_PRINTF("\nLLDP Debug Tracing is already Enabled.\n");
        return (L7_FAILURE);
    }

    lldpIsDebugEnabled = L7_TRUE;
    LLDP_DEBUG_PRINTF("\nLLDP Debug Tracing is Enabled.\n");

    return L7_SUCCESS;

} /* lldpDebugTraceEnable */


/*********************************************************************
* @purpose  Disable Debug Tracing in LLDP
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully disabled.
* @returns  L7_FAILURE  if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t
lldpDebugTraceDisable(void)
{
    if (lldpIsDebugEnabled != L7_TRUE)
    {
        LLDP_DEBUG_PRINTF ("LLDP Debug Tracing is already Disabled.\n");
        return (L7_FAILURE);
    }

    lldpIsDebugEnabled = L7_FALSE;
    LLDP_DEBUG_PRINTF ("LLDP Debug Tracing is Disabled.\n");

    return L7_SUCCESS;

} /* lldpDebugTraceDisable */


/*********************************************************************
* @purpose  Shows the usage of the LLDP Debug Trace Utility
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/

void
lldpDebugTraceHelp(void)
{
    LLDP_DEBUG_PRINTF ("lldpDebugTraceEnable()  - Enable Debug Tracing in LLDP\n");
    LLDP_DEBUG_PRINTF ("lldpDebugTraceDisable() - Disable Debug Tracing in LLDP\n");
    LLDP_DEBUG_PRINTF ("lldpDebugTraceAllFlagsReset() - Disable Debug Tracing for All Events\n");
    LLDP_DEBUG_PRINTF ("lldpDebugTraceFlagsShow() - Show the status of Trace Flags\n");
    LLDP_DEBUG_PRINTF ("lldpDebugTraceFlagsSet(flag) - Enable Debug Tracing for the Specified Flag\n");
    LLDP_DEBUG_PRINTF ("lldpDebugTraceFlagsReset(flag) - Disable Debug Tracing for the Specified Flag\n");
    LLDP_DEBUG_PRINTF ("     Flags  ....\n");
    LLDP_DEBUG_PRINTF ("       %d   -> To Trace LLDP Timer Events.\n", LLDP_DEBUG_TIMER);
    LLDP_DEBUG_PRINTF ("       %d   -> To Trace the Protocol Control Packets Path.\n", LLDP_DEBUG_PROTO);
    LLDP_DEBUG_PRINTF ("       %d   -> To Trace the LLDP MED Events.\n", LLDP_DEBUG_MED);
    LLDP_DEBUG_PRINTF ("       %d   -> To Trace the LLDP POE MED Events.\n", LLDP_DEBUG_POE_MED);
    LLDP_DEBUG_PRINTF ("       %d   -> To Trace the LLDP Checkpoint Service (NSF Only).\n", LLDP_DEBUG_CHECKPOINT);
    LLDP_DEBUG_PRINTF ("       %d   -> To Trace the LLDP Checkpoint Service Verbose level (NSF Only).\n", LLDP_DEBUG_CHECKPOINT_VERBOSE);
    LLDP_DEBUG_PRINTF ("       %d   -> To Trace the Protocol Control Packets Path verbosely.\n", LLDP_DEBUG_PROTO_VERBOSE);

    return;

} /* lldpDebugTraceHelp */


/*********************************************************************
* @purpose  Clear all trace flags
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/

void
lldpDebugTraceAllFlagsReset(void)
{
    lldpDebugFlags = 0;

    return;

} /* lldpDebugTraceAllFlagsReset */


/*********************************************************************
* @purpose  Shows the Enabled/Disabled Trace flags
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/

void
lldpDebugTraceFlagsShow(void)
{
    LLDP_DEBUG_PRINTF ("  Debug Trace Enabled                     ->   %s\n", (lldpIsDebugEnabled) ? "Y" : "N");
    LLDP_DEBUG_PRINTF ("  Trace LLDP Timer Events                 ->   %s\n", (lldpDebugFlags & (1 << LLDP_DEBUG_TIMER)) ? "Y" : "N");
    LLDP_DEBUG_PRINTF ("  Trace the Protocol Control Packets Path ->   %s\n", (lldpDebugFlags & (1 << LLDP_DEBUG_PROTO)) ? "Y" : "N");
    LLDP_DEBUG_PRINTF ("  Trace the LLDP MED Events               ->   %s\n", (lldpDebugFlags & (1 << LLDP_DEBUG_MED)) ? "Y" : "N");
    LLDP_DEBUG_PRINTF ("  Trace the LLDP POE MED Events           ->   %s\n", (lldpDebugFlags & (1 << LLDP_DEBUG_POE_MED)) ? "Y" : "N");
    LLDP_DEBUG_PRINTF ("  Trace the LLDP Checkpoint service       ->   %s\n", (lldpDebugFlags & (1 << LLDP_DEBUG_CHECKPOINT)) ? "Y" : "N");
    LLDP_DEBUG_PRINTF ("  Trace the LLDP Checkpoint verbose level ->   %s\n", (lldpDebugFlags & (1 << LLDP_DEBUG_CHECKPOINT_VERBOSE)) ? "Y" : "N");
    LLDP_DEBUG_PRINTF ("  Trace the Protocol Control Path verbose ->   %s\n", (lldpDebugFlags & (1 << LLDP_DEBUG_PROTO_VERBOSE)) ? "Y" : "N");

    return;

} /* lldpDebugTraceFlagsShow */


/*********************************************************************
* @purpose  Set a particular tracelevel
*
* @param    flag   {(input)} Trace Level
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/

void
lldpDebugTraceFlagsSet(L7_uint32 flag)
{
    if (flag < LLDP_DEBUG_FLAGS_BITS)
    {
        lldpDebugFlags |= (1 << flag);
    }

    return;

} /* lldpDebugTraceFlagsSet */


/*********************************************************************
* @purpose  Reset a particular tracelevel
*
* @param    flag   {(input)} Trace Level
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/

void
lldpDebugTraceFlagsReset(L7_uint32 flag)
{
    if (flag < LLDP_DEBUG_FLAGS_BITS)
    {
        lldpDebugFlags &= ~(1 << flag);
    }

    return;

} /* lldpDebugTraceFlagsReset */


/*********************************************************************
* @purpose
*
* @param    flag   {(input)} Trace Level
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/

L7_BOOL
lldpDebugTraceFlagCheck (L7_uint32 traceFlag)
{
    if (lldpIsDebugEnabled != L7_TRUE)
    {
        return L7_FALSE;
    }

    if (lldpDebugFlags & (1 << traceFlag))
    {
        return L7_TRUE;
    }

    return L7_FALSE;

} /* lldpDebugTraceFlagsCheck */
