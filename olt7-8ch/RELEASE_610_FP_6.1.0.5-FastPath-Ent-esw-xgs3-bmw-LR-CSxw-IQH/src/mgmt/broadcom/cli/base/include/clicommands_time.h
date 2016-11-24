/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_time.h
*
* @purpose header for timezone commands in clicommands_time.h
*
* @component user interface
*
* @comments none
*
* @create  18/04/2007
*
* @author  drajendra
* @end
*
***********************************************************************/

#ifndef CLICOMMANDS_TIME_H
#define CLICOMMANDS_TIME_H

#define L7_CLI_ZERO   0
#define L7_CLI_ONE    1
#define L7_CLI_TWO    2
#define L7_CLI_THREE  3
#define L7_CLI_FOUR   4
#define L7_CLI_FIVE   5

const L7_char8  *commandClockTimeZone(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandClockSummerTimeDate(EwsContext ewsContext, L7_uint32 argc, 
                                            const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandClockSummerTimeRecurring (EwsContext ewsContext,
    L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 * commandNoSummerTime (EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index);
const L7_char8 * commandClockSummerTimeRecurringSpec (EwsContext ewsContext,
                          L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);

  /* summer time */
L7_RC_t cliRunningConfigSummerTime(EwsContext ewsContext, L7_uint32 unit);
  /* timezone*/
L7_RC_t cliRunningConfigClockTimeZone (EwsContext ewsContext, L7_uint32 unit);

void buildTreeShowClock(EwsCliCommandP depth2);
void  buildTreeGlobalClockTimeZone(EwsCliCommandP depth2);
void buildTreeGlobalClockSummerTimeDate(EwsCliCommandP  depth3);
void buildTreeGlobalClockSummerTimeRecurring(EwsCliCommandP  depth4);
void buildTreeGlobalClockSummerTimeRecurringSpec(EwsCliCommandP  depth4);
#endif /* CLICOMMANDS_TIME_H */
