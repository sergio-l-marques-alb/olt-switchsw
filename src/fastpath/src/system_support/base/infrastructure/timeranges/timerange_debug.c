/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2010
*
**********************************************************************
*
* @filename    timerange_debug.c
*
* @purpose     TIMERANGE debug functions
*
* @component   TIMERANGES
*
* @comments    none
*
* @create      29/11/2009
*
* @author      Siva Mannem
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "osapi_support.h"

#include "log.h"
#include "support_api.h"

#include "timerange.h"
#include "timerange_api.h"
#include "timerange_debug.h"


/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/

static L7_uchar8    timeRangeDebugFlags = 0;
static L7_BOOL      timeRangeIsDebugEnabled = L7_FALSE;


/*********************************************************************
* @purpose  Enable Debug Tracing in TIMERANGE
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
timeRangeDebugTraceEnable (void)
{
    if (timeRangeIsDebugEnabled == L7_TRUE)
    {
        TIMERANGE_DEBUG_PRINTF("\nTIMERANGE Debug Tracing is already Enabled.\n");
        return (L7_FAILURE);
    }

    timeRangeIsDebugEnabled = L7_TRUE;
    TIMERANGE_DEBUG_PRINTF("\nTIMERANGE Debug Tracing is Enabled.\n");

    return L7_SUCCESS;

} /* timeRangeDebugTraceEnable */


/*********************************************************************
* @purpose  Disable Debug Tracing in TIMERANGE
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
timeRangeDebugTraceDisable(void)
{
    if (timeRangeIsDebugEnabled != L7_TRUE)
    {
        TIMERANGE_DEBUG_PRINTF ("TIMERANGE Debug Tracing is already Disabled.\n");
        return (L7_FAILURE);
    }

    timeRangeIsDebugEnabled = L7_FALSE;
    TIMERANGE_DEBUG_PRINTF ("TIMERANGE Debug Tracing is Disabled.\n");

    return L7_SUCCESS;

} /* timeRangeDebugTraceDisable */


/*********************************************************************
* @purpose  Shows the usage of the TIMERANGE Debug Trace Utility
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
timeRangeDebugTraceHelp(void)
{
    TIMERANGE_DEBUG_PRINTF ("timeRangeDebugTraceEnable()  - Enable Debug Tracing in TIMERANGE\n");
    TIMERANGE_DEBUG_PRINTF ("timeRangeDebugTraceDisable() - Disable Debug Tracing in TIMERANGE\n");
    TIMERANGE_DEBUG_PRINTF ("timeRangeDebugTraceAllFlagsReset() - Disable Debug Tracing for All Events\n");
    TIMERANGE_DEBUG_PRINTF ("timeRangeDebugTraceFlagsShow() - Show the status of Trace Flags\n");
    TIMERANGE_DEBUG_PRINTF ("timeRangeDebugTraceFlagsSet(flag) - Enable Debug Tracing for the Specified Flag\n");
    TIMERANGE_DEBUG_PRINTF ("timeRangeDebugTraceFlagsReset(flag) - Disable Debug Tracing for the Specified Flag\n");
    TIMERANGE_DEBUG_PRINTF ("     Flags  ....\n");
    TIMERANGE_DEBUG_PRINTF ("       0   -> To Trace TIMERANGE Timer Events.\n");
    TIMERANGE_DEBUG_PRINTF ("       1   -> To Trace the TIMERANGE State changes.\n");
    TIMERANGE_DEBUG_PRINTF ("       2   -> To Trace the TIMERANGE CNFGR Events.\n");
    TIMERANGE_DEBUG_PRINTF ("       3   -> To Trace the TIMERANGE MODIFY Events.\n");

    return;

} /* timeRangeDebugTraceHelp */


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
timeRangeDebugTraceAllFlagsReset(void)
{
    timeRangeDebugFlags = 0;

    return;

} /* timeRangeDebugTraceAllFlagsReset */


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
timeRangeDebugTraceFlagsShow(void)
{
    TIMERANGE_DEBUG_PRINTF ("  Debug Trace Enabled                        ->   %s\n", (timeRangeIsDebugEnabled) ? "Y" : "N");
    TIMERANGE_DEBUG_PRINTF ("  Trace TIMERANGE Timer Events               ->   %s\n", (timeRangeDebugFlags &  1) ? "Y" : "N");
    TIMERANGE_DEBUG_PRINTF ("  Trace the TIMERANGE State changes          ->   %s\n", (timeRangeDebugFlags &  2) ? "Y" : "N");
    TIMERANGE_DEBUG_PRINTF ("  Trace the TIMERANGE CNFGR Events           ->   %s\n", (timeRangeDebugFlags &  4) ? "Y" : "N");
    TIMERANGE_DEBUG_PRINTF ("  Trace the TIMERANGE MODIFY Events          ->   %s\n", (timeRangeDebugFlags &  8) ? "Y" : "N");

    return;

} /* timeRangeDebugTraceFlagsShow */


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
timeRangeDebugTraceFlagsSet(L7_uint32 flag)
{
    if (flag < TIMERANGE_DEBUG_FLAGS_BITS)
    {
        timeRangeDebugFlags |= (1 << flag);
    }

    return;

} /* timeRangeDebugTraceFlagsSet */


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
timeRangeDebugTraceFlagsReset(L7_uint32 flag)
{
    if (flag < TIMERANGE_DEBUG_FLAGS_BITS)
    {
        timeRangeDebugFlags &= ~(1 << flag);
    }

    return;

} /* timeRangeDebugTraceFlagsReset */


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
timeRangeDebugTraceFlagCheck (L7_uint32 traceFlag)
{
    if (timeRangeIsDebugEnabled != L7_TRUE)
    {
        return L7_FALSE;
    }

    if (timeRangeDebugFlags & (1 << traceFlag))
    {
        return L7_TRUE;
    }

    return L7_FALSE;

} /* timeRangeDebugTraceFlagsCheck */
