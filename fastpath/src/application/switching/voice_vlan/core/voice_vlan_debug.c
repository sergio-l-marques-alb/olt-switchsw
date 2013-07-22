/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    voice_vlan_debug.c
* @purpose     Voice VLAN debug functions
* @component   Voice VLAN
* @comments    none
* @create      03/17/2009
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

#include "voice_vlan.h"
#include "voice_vlan_api.h"
#include "voice_vlan_debug.h"


/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/

static L7_uchar8    voiceVlanDebugFlags;
static L7_BOOL      voiceVlanIsDebugEnabled = L7_FALSE;


/*********************************************************************
* @purpose  Enable Debug Tracing in Voice VLAN
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
voiceVlanDebugTraceEnable(void)
{
    if (voiceVlanIsDebugEnabled == L7_TRUE)
    {
        VOICE_VLAN_DEBUG_PRINTF("\nVoice VLAN Debug Tracing is already Enabled.\n");
        return (L7_FAILURE);
    }

    voiceVlanIsDebugEnabled = L7_TRUE;
    VOICE_VLAN_DEBUG_PRINTF("\nVoice VLAN Debug Tracing is Enabled.\n");

    return L7_SUCCESS;

} /* voiceVlanDebugTraceEnable */


/*********************************************************************
* @purpose  Disable Debug Tracing in Voice VLAN
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
voiceVlanDebugTraceDisable(void)
{
    if (voiceVlanIsDebugEnabled != L7_TRUE)
    {
        VOICE_VLAN_DEBUG_PRINTF ("Voice VLAN Debug Tracing is already Disabled.\n");
        return (L7_FAILURE);
    }

    voiceVlanIsDebugEnabled = L7_FALSE;
    VOICE_VLAN_DEBUG_PRINTF ("Voice VLAN Debug Tracing is Disabled.\n");

    return L7_SUCCESS;

} /* voiceVlanDebugTraceDisable */


/*********************************************************************
* @purpose  Shows the usage of the Voice VLAN Debug Trace Utility
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
voiceVlanDebugTraceHelp(void)
{
    VOICE_VLAN_DEBUG_PRINTF ("voiceVlanDebugTraceEnable()  - Enable Debug Tracing in Voice VLAN\n");
    VOICE_VLAN_DEBUG_PRINTF ("voiceVlanDebugTraceDisable() - Disable Debug Tracing in Voice VLAN\n");
    VOICE_VLAN_DEBUG_PRINTF ("voiceVlanDebugTraceAllFlagsReset() - Disable Debug Tracing for All Events\n");
    VOICE_VLAN_DEBUG_PRINTF ("voiceVlanDebugTraceFlagsShow() - Show the status of Trace Flags\n");
    VOICE_VLAN_DEBUG_PRINTF ("voiceVlanDebugTraceFlagsSet(flag) - Enable Debug Tracing for the Specified Flag\n");
    VOICE_VLAN_DEBUG_PRINTF ("voiceVlanDebugTraceFlagsReset(flag) - Disable Debug Tracing for the Specified Flag\n");
    VOICE_VLAN_DEBUG_PRINTF ("     Flags  ....\n");
    VOICE_VLAN_DEBUG_PRINTF ("       1   -> To Trace Voice VLAN High Level\n");
    VOICE_VLAN_DEBUG_PRINTF ("       2   -> To Trace Voice VLAN Medium Level\n");
    VOICE_VLAN_DEBUG_PRINTF ("       3   -> To Trace Voice VLAN Debug Level\n");
    VOICE_VLAN_DEBUG_PRINTF ("       4   -> To Trace Voice VLAN Checkpoint Service (NSF Only)\n");

    return;

} /* voiceVlanDebugTraceHelp */


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
voiceVlanDebugTraceAllFlagsReset(void)
{
    voiceVlanDebugFlags = 0;

    return;

} /* voiceVlanDebugTraceAllFlagsReset */


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
voiceVlanDebugTraceFlagsShow(void)
{
    VOICE_VLAN_DEBUG_PRINTF ("  Debug Trace Enabled                     ->   %s\n", (voiceVlanIsDebugEnabled) ? "Y" : "N");
    VOICE_VLAN_DEBUG_PRINTF ("  Trace the Voice VLAN High Level         ->   %s\n", (voiceVlanDebugFlags &  2) ? "Y" : "N");
    VOICE_VLAN_DEBUG_PRINTF ("  Trace the Voice VLAN Medium Level       ->   %s\n", (voiceVlanDebugFlags &  4) ? "Y" : "N");
    VOICE_VLAN_DEBUG_PRINTF ("  Trace the Voice VLAN Debug Level        ->   %s\n", (voiceVlanDebugFlags &  8) ? "Y" : "N");
    VOICE_VLAN_DEBUG_PRINTF ("  Trace the Voice VLAN Checkpoint service ->   %s\n", (voiceVlanDebugFlags & 16) ? "Y" : "N");

    return;

} /* voiceVlanDebugTraceFlagsShow */


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
voiceVlanDebugTraceFlagsSet(L7_uint32 flag)
{
    if (flag < VOICE_VLAN_TRACE_FLAGS_BITS)
    {
        voiceVlanDebugFlags |= (1 << flag);
    }

    return;

} /* voiceVlanDebugTraceFlagsSet */


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
voiceVlanDebugTraceFlagsReset(L7_uint32 flag)
{
    if (flag < VOICE_VLAN_TRACE_FLAGS_BITS)
    {
        voiceVlanDebugFlags &= ~(1 << flag);
    }

    return;

} /* voiceVlanDebugTraceFlagsReset */


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
voiceVlanDebugTraceFlagCheck (L7_uint32 traceFlag)
{
    if (voiceVlanIsDebugEnabled != L7_TRUE)
    {
        return L7_FALSE;
    }

    if (voiceVlanDebugFlags & (1 << traceFlag))
    {
        return L7_TRUE;
    }

    return L7_FALSE;

} /* voiceVlanDebugTraceFlagCheck */


/*********************************************************************
* @purpose Trace voice vlan events - kept for compatibility with old
*          debug trace types
*
* @param   1 - enable tracing
*          0 - disable tracing
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/

void
voiceVlanTraceEvent(L7_uint32 debug)
{
    L7_uint32   i;

    if (debug == 0)
    {
        voiceVlanIsDebugEnabled = L7_FALSE;
        voiceVlanDebugTraceAllFlagsReset();
        return;
    }

    if (debug > VOICE_VLAN_TRACE_FLAGS_BITS)
    {
        debug = VOICE_VLAN_TRACE_FLAGS_BITS;
    }

    voiceVlanIsDebugEnabled = L7_TRUE;

    for (i = 1; i <= debug; i++)
    {
        voiceVlanDebugTraceFlagsSet(i);
    }

    return;

} /* voiceVlanTraceEvent */
