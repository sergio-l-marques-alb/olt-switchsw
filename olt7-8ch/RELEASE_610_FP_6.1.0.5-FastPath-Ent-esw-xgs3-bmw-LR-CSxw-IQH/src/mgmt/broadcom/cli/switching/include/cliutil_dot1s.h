/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/dot1s/cliutil_dot1s.h
 *
 * @purpose header for cliutil_dot1s.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   10/30/2002
 *
 * @author  Jill Flanagan
 * @end
 *
 **********************************************************************/

#ifndef CLIUTIL_SPANTREE_H
#define CLIUTIL_SPANTREE_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include <commdefs.h>
#include <datatypes.h>

/*********************************************************************
* @purpose  Implements the set spanning tree instance logic that is
*           required by 2 cli commands.
*
* @param    ewsContext   context for error messages
* @param    instance     spanning tree instance
* @param    priority     priority for this instance
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/

void cliSpanTreeSetInstancePriority(EwsContext ewsContext, L7_uint32 instance,
                                    L7_uint32 priority);

/*********************************************************************
* @purpose  Query and display the list of FIDs and VIDs associated with the
*           specified mstid.  Display the count for each followed by the
*           list in a table.
*
* @param    ewsContext   context for error messages
* @param    mstid     spanning tree instance
* @param    displayLines max Lines to output
* @param    firstPass   required first time this is called
*
* @returns L7_SUCCESS
* @returns L7_FAILURE if paging is required
* @returns L7_ERROR  if not enough space to display headings
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t cliSpanTreeShowFidsAndVlans(EwsContext ewsContext, L7_uint32 mstid,
                                    L7_uint32 displayLines, L7_BOOL firstPass );

/*********************************************************************
* @purpose  Query and display the list of MSTIDs
*
* @param    ewsContext   context for error messages
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t cliSpanTreeShowMstList(EwsContext ewsContext );

/*********************************************************************
* @purpose  Format the provided char* into 802.1 identifier format
*
* @param    stat output string
* @param    stat output string buffer size
* @param    idIn input string to be formatted
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/

void cliSpanTreeFormatBridgeIdentifier( L7_char8 * stat, L7_uint32 statSize, L7_uchar8 * idIn );

/*********************************************************************
* @purpose  Format the provided ushort16 port identifier into a string
*           displayable in the format xx:xx.
*
* @param    stat output string. Must be at least 3 bytes long.
* @param    stat output string buffer size
* @param    val16 input val
*
* @returns void
*
* @notes Adapted from example in Kernighan and Ritchie, p 64.
*
* @end
*********************************************************************/

void cliSpanTreeFormatPortIdentifier( L7_char8 * stat, L7_uint32 statSize, L7_ushort16 val16 );

/*********************************************************************
* @purpose  Display the statistics for the specified port
*
* @param    ewsContext to write the output
* @param    interface associated with this port
*
* @returns void
*
* @notes
*
* @end
*********************************************************************/

void cliSpanTreeDisplayPortStats( EwsContext ewsContext, L7_uint32 iface );

/*********************************************************************
* @purpose  Get and return a string containing the role of the specified port
*
* @param    unit index
* @param    instance spanningtree instance to use
* @param    iface
* @param    role buffer to store the return string
* @param    size of buffer
*
* @returns void
*
* @notes The returned string will be null terminated.  The role
*        buffer must be at least L7_DOT1S_MAX_PORT_ROLE_LENGTH+1 in size;
*
* @end
*********************************************************************/

L7_RC_t cliSpanTreeGetPortRole( L7_uint32 unit, L7_uint32 instance,
                               L7_uint32 iface, L7_char8 * role,
                               L7_uint32 size );

/*********************************************************************
* @purpose  Get and return port forwarding state for the specified port
*           and instance.
*
* @param    unit index
* @param    instance spanningtree instance to use
* @param    iface
* @param    role buffer to store the return string
* @param    size of buffer
*
* @returns void
*
* @notes The returned string will be null terminated.  The role
*        buffer must be at least L7_DOT1S_MAX_PORTFWDSTATE_LENGTH+1 in size;
*
* @end
*********************************************************************/

L7_RC_t cliSpanTreeGetPortFwdState( L7_uint32 unit, L7_uint32 instance,
                                   L7_uint32 iface, L7_char8 * stat,
                                   L7_uint32 size );

/*********************************************************************
 *
 * @purpose check user's input to determine if it is printable
 *
 *
 * @param *input         the argument entered by user
 * @param length         length of string to check
 *
 * @returns   L7_SUCCESS if all characters are printable
 * @returns   L7_FAILURE if there is one or more nonprintable characters
 *
 * @notes
 *
 *
 * @end
 *
 ********************************************************************/
L7_RC_t cliIsPrintable(L7_char8 * input, L7_uint32 length);

/*********************************************************************
 *
 * @purpose Determine if the specified port is valid for spanning tree
 *          and generate a failure message if not.
 *
 *
 * @param unit         the unit index (for a stacking environment)
 * @param iface        the interface to check
 *
 * @return   L7_SUCCESS the interface is valid
 * @return   L7_FAILURE the interface is not a valid type for dot1s.
 *
 * @note An interface is considered valid for dot1s if it is one of
 *       the following types:    USM_PHYSICAL_INTF or USM_LAG_INTF
 *       In case of failure, the failure message is written to the
 *       cli context.  The caller needs only to return.
 *
 *
 * @end
 *
 ********************************************************************/
L7_RC_t cliValidateDot1sInterface(EwsContext ewsContext, L7_uint32 unit, L7_uint32 iface);

/*********************************************************************
 *
 * @purpose Determine if the specified port is valid for spanning tree
 *          and generate error message based on message parameter.
 *
 *
 * @param ewsContext   context for error messages
 * @param unit         the unit index (for a stacking environment)
 * @param iface        the interface to check
 * @param message      L7_BOOL generate error message or not
 *
 * @return   L7_SUCCESS the interface is valid
 * @return   L7_FAILURE the interface is not a valid type for dot1s.
 *
 * @note An interface is considered valid for dot1s if it is one of
 *       the following types:    USM_PHYSICAL_INTF or USM_LAG_INTF
 *       In case of failure, the failure message is written to the
 *       cli context.  The caller needs only to return.
 *
 *
 * @end
 *
 ********************************************************************/
L7_RC_t cliValidateDot1sInterfaceMessage(EwsContext ewsContext,
                                         L7_uint32 unit, L7_uint32 iface,
                                         L7_BOOL message );
#endif
