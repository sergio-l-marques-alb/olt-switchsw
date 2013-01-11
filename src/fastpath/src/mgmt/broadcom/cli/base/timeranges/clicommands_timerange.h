/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/timerange/clicommands_timerange.h
*
* @purpose create the tree for time range CLI 
*
* @component user interface
*
* @comments none
*
* @create  08 Dec 2009
*
* @author Siva Mannem
* @end
*
**********************************************************************/

#ifndef CLICOMMANDS_TIMERANGE_H
#define CLICOMMANDS_TIMERANGE_H

/* Begin Function Prototypes */
void buildTreePeriodicTimeEntryEndTime(EwsCliCommandP  depth1, L7_BOOL buildEndDay);
/*********************************************************************
*
* @purpose  Build the tree nodes for absolute ommand
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePeriodicTimeEntry(EwsCliCommandP  depth2);
/*********************************************************************
*
* @purpose  Build the tree nodes for absolute ommand
*
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeAbsoluteTimeEntry(EwsCliCommandP  depth2);

/*********************************************************************
*
* @purpose  Build the time-range command tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes 
*
* @end
*
*********************************************************************/
void buildTreeGlobalTimeRange(EwsCliCommandP depth1);
/*********************************************************************
*
* @purpose  To build the time range Menu
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*
*********************************************************************/
void buildTreeTimeRangeConfig();

/*********************************************************************
*
* @purpose  Creates the 'show time-range [<name>]' commands
*
* @param void
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void cliTreeShowTimeRange(EwsCliCommandP depth1);

/* End Function Prototypes */

#endif /* CLICOMMANDS_TIMERANGE_H */
