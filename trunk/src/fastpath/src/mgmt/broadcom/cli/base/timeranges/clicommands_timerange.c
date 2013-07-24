/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/timerange/clicommands_timerange.c
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
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "timerange_exports.h"
const L7_char8 *commandPeriodicTimeRangeEntry(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 **argv,
                                              L7_uint32 index);

const L7_char8 *commandAbsoluteTimeRangeEntry(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 **argv,
                                              L7_uint32 index);

const char *commandShowTimeRange(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 **argv,
                                 L7_uint32 index);

const L7_char8 *cliTimeRangeMode(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 **argv,
                                 L7_uint32 index);

/*********************************************************************
*
* @purpose  Build the tree for end time node for periodic command
*
* @param EwsCliCommandP depth
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePeriodicTimeEntryEndTime(EwsCliCommandP  depth1, L7_BOOL buildEndDay)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_time_fmt,
                         pStrInfo_base_clisummertime_st_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_periodic_to,
                         pStrInfo_base_periodic_to_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (buildEndDay == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_periodic_mon,
                           pStrInfo_base_periodic_mon_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(depth4, pStrInfo_base_time_fmt,
                           pStrInfo_base_clisummertime_et_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                  NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_periodic_tue,
                           pStrInfo_base_periodic_tue_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(depth4, pStrInfo_base_time_fmt,
                           pStrInfo_base_clisummertime_et_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                  NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_periodic_wed,
                           pStrInfo_base_periodic_wed_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(depth4, pStrInfo_base_time_fmt,
                           pStrInfo_base_clisummertime_et_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                  L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_periodic_thu,
                           pStrInfo_base_periodic_thu_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(depth4, pStrInfo_base_time_fmt,
                           pStrInfo_base_clisummertime_et_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    ewsCliAddNode(depth5, pStrInfo_common_Cr,
                  pStrInfo_common_NewLine, NULL, 2,
                  L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_periodic_fri,
                           pStrInfo_base_periodic_fri_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(depth4, pStrInfo_base_time_fmt,
                           pStrInfo_base_clisummertime_et_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    ewsCliAddNode(depth5, pStrInfo_common_Cr,
                  pStrInfo_common_NewLine, NULL, 2,
                  L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_periodic_sat,
                           pStrInfo_base_periodic_sat_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(depth4, pStrInfo_base_time_fmt, 
                           pStrInfo_base_clisummertime_et_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    ewsCliAddNode(depth5, pStrInfo_common_Cr,
                  pStrInfo_common_NewLine, NULL, 2,
                  L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_periodic_sun,
                           pStrInfo_base_periodic_sun_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(depth4, pStrInfo_base_time_fmt,
                           pStrInfo_base_clisummertime_et_help, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                  NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  }
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_time_fmt,
                         pStrInfo_base_clisummertime_et_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,
                         pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
*
* @purpose  Build the tree for saturday node for periodic command
*
* @param EwsCliCommandP depth
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeSatPeriodicTimeEntry(EwsCliCommandP  depth, L7_BOOL buildEndDay)
{
  EwsCliCommandP depth1;
  depth1 = ewsCliAddNode(depth, pStrInfo_base_periodic_sat,
                         pStrInfo_base_periodic_sat_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreePeriodicTimeEntryEndTime(depth1, buildEndDay);
}

/*********************************************************************
*
* @purpose  Build the tree for friday node for periodic command
*
* @param EwsCliCommandP depth
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeFriPeriodicTimeEntry(EwsCliCommandP  depth, L7_BOOL buildEndDay)
{
  EwsCliCommandP depth1;
  depth1 = ewsCliAddNode(depth, pStrInfo_base_periodic_fri,
                         pStrInfo_base_periodic_fri_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreePeriodicTimeEntryEndTime(depth1, buildEndDay);
  buildTreeSatPeriodicTimeEntry(depth1, L7_FALSE);
}

/*********************************************************************
*
* @purpose  Build the tree for thursday node for periodic command
*
* @param EwsCliCommandP depth
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeThuPeriodicTimeEntry(EwsCliCommandP  depth, L7_BOOL buildEndDay)
{
  EwsCliCommandP depth1;
  depth1 = ewsCliAddNode(depth, pStrInfo_base_periodic_thu,
                         pStrInfo_base_periodic_thu_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreePeriodicTimeEntryEndTime(depth1, buildEndDay);
  buildTreeFriPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeSatPeriodicTimeEntry(depth1, L7_FALSE);
}

/*********************************************************************
*
* @purpose  Build the tree for wednesday node for periodic command
*
* @param EwsCliCommandP depth
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeWedPeriodicTimeEntry(EwsCliCommandP  depth, L7_BOOL buildEndDay)
{
  EwsCliCommandP depth1;
  depth1 = ewsCliAddNode(depth, pStrInfo_base_periodic_wed,
                         pStrInfo_base_periodic_wed_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreePeriodicTimeEntryEndTime(depth1, buildEndDay);
  buildTreeThuPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeFriPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeSatPeriodicTimeEntry(depth1, L7_FALSE);
}

/*********************************************************************
*
* @purpose  Build the tree for tuesday node for periodic command
*
* @param EwsCliCommandP depth
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeTuePeriodicTimeEntry(EwsCliCommandP  depth, L7_BOOL buildEndDay)
{
  EwsCliCommandP depth1;
  depth1 = ewsCliAddNode(depth, pStrInfo_base_periodic_tue,
                         pStrInfo_base_periodic_tue_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreePeriodicTimeEntryEndTime(depth1, buildEndDay);
  buildTreeWedPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeThuPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeFriPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeSatPeriodicTimeEntry(depth1, L7_FALSE);
}

/*********************************************************************
*
* @purpose  Build the tree for monday node for periodic command
*
* @param EwsCliCommandP depth
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeMonPeriodicTimeEntry(EwsCliCommandP  depth, L7_BOOL buildEndDay)
{
  EwsCliCommandP depth1;
  depth1 = ewsCliAddNode(depth, pStrInfo_base_periodic_mon,
                         pStrInfo_base_periodic_mon_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreePeriodicTimeEntryEndTime(depth1, buildEndDay);
  buildTreeTuePeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeWedPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeThuPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeFriPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeSatPeriodicTimeEntry(depth1, L7_FALSE);
}

/*********************************************************************
*
* @purpose  Build the tree for sunday node for periodic command
*
* @param EwsCliCommandP depth
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeSunPeriodicTimeEntry(EwsCliCommandP  depth, L7_BOOL buildEndDay)
{
  EwsCliCommandP depth1;
  depth1 = ewsCliAddNode(depth, pStrInfo_base_periodic_sun,
                         pStrInfo_base_periodic_sun_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreePeriodicTimeEntryEndTime(depth1, buildEndDay);
  buildTreeMonPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeTuePeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeWedPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeThuPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeFriPeriodicTimeEntry(depth1, L7_FALSE);
  buildTreeSatPeriodicTimeEntry(depth1, L7_FALSE);
}

/*********************************************************************
*
* @purpose  Build the tree nodes for periodic command
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
void buildTreePeriodicTimeEntry(EwsCliCommandP  depth2)
{
  EwsCliCommandP depth3;
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_periodic_daily,
                         pStrInfo_base_periodic_daily_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH); 

  buildTreePeriodicTimeEntryEndTime(depth3, L7_FALSE);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_periodic_weekdays,
                         pStrInfo_base_periodic_weekdays_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH); 

  buildTreePeriodicTimeEntryEndTime(depth3, L7_FALSE);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_periodic_weekends,
                         pStrInfo_base_periodic_weekends_help, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH); 

  buildTreePeriodicTimeEntryEndTime(depth3, L7_FALSE);
  buildTreeSunPeriodicTimeEntry(depth2, L7_TRUE);
  buildTreeMonPeriodicTimeEntry(depth2, L7_TRUE);
  buildTreeTuePeriodicTimeEntry(depth2, L7_TRUE);
  buildTreeWedPeriodicTimeEntry(depth2, L7_TRUE);
  buildTreeThuPeriodicTimeEntry(depth2, L7_TRUE);
  buildTreeFriPeriodicTimeEntry(depth2, L7_TRUE);
  buildTreeSatPeriodicTimeEntry(depth2, L7_TRUE);
}

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
void buildTreeAbsoluteTimeEntry(EwsCliCommandP  depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8;
  EwsCliCommandP depth9, depth10, depth11, depth12, depth13 ;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_start, 
                         pStrInfo_base_startHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3,
                         pStrInfo_base_time_fmt, 
                         pStrInfo_base_clisummertime_st_help,NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4,
                         pStrInfo_base_date_range, 
                         pStrInfo_base_absolute_sd_help, NULL, 
                         L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5,
                         pStrInfo_base_mon, 
                         pStrInfo_base_absolute_sm_help, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6,NULL, 
                         pStrInfo_base_absolute_sy_help,NULL,
                         3, L7_NODE_UINT_RANGE, 
                         L7_TIMERANGE_MIN_YEAR, L7_TIMERANGE_MAX_YEAR);
  depth8 = ewsCliAddNode(depth7,
                         pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, NULL, 
                         L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7,
                         pStrInfo_base_end, 
                         pStrInfo_base_endHelp, NULL, 
                         L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8,
                         pStrInfo_base_time_fmt, 
                         pStrInfo_base_clisummertime_et_help,NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth10 = ewsCliAddNode(depth9,
                         pStrInfo_base_date_range,
                         pStrInfo_base_absolute_ed_help, NULL, 
                         L7_NO_OPTIONAL_PARAMS);
  depth11 = ewsCliAddNode(depth10,
                          pStrInfo_base_mon,
                          pStrInfo_base_absolute_em_help, NULL,
                          L7_NO_OPTIONAL_PARAMS);
  depth12 = ewsCliAddNode(depth11, NULL, 
                          pStrInfo_base_absolute_ey_help,NULL,
                          3, L7_NODE_UINT_RANGE, 
                          L7_TIMERANGE_MIN_YEAR, L7_TIMERANGE_MAX_YEAR);

  depth13 = ewsCliAddNode(depth12,
                          pStrInfo_common_Cr,
                          pStrInfo_common_NewLine, NULL,
                          L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, 
                         pStrInfo_base_end,
                         pStrInfo_base_endHelp, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3,
                         pStrInfo_base_time_fmt,
                         pStrInfo_base_clisummertime_et_help,NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_date_range,
                         pStrInfo_base_absolute_ed_help, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5,
                         pStrInfo_base_mon,
                         pStrInfo_base_absolute_em_help, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth7 = ewsCliAddNode(depth6,NULL,
                         pStrInfo_base_absolute_ey_help, NULL,
                         3, L7_NODE_UINT_RANGE, 
                         L7_TIMERANGE_MIN_YEAR, L7_TIMERANGE_MAX_YEAR);

  depth8 = ewsCliAddNode(depth7,
                         pStrInfo_common_Cr,
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

}

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
void buildTreeGlobalTimeRange(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_timerange,
                         pStrInfo_base_timerangeHelp, cliTimeRangeMode, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Name,
                         pStrInfo_base_timerangeName, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr,
                         pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}
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
void buildTreeTimeRangeConfig()
{
  EwsCliCommandP depth1, depth2, depth2a, depth3;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, 
                         cliTimeRangeMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_TIMERANGE_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_periodic,
                         pStrInfo_base_periodicHelp, 
                         commandPeriodicTimeRangeEntry, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreePeriodicTimeEntry(depth2);
  
  depth2a = ewsCliAddNode (depth1, pStrInfo_common_Exit, 
                           pStrInfo_common_ToExitMode, NULL,
                           L7_NO_OPTIONAL_PARAMS );
  depth3 = ewsCliAddNode (depth2a, pStrInfo_common_Cr,
                          pStrInfo_common_NewLine, 
                          NULL, L7_NO_OPTIONAL_PARAMS );

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_absolute,
                         pStrInfo_base_absoluteHelp,
                         commandAbsoluteTimeRangeEntry, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreeAbsoluteTimeEntry(depth2);
}
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
void cliTreeShowTimeRange(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_timerange,
                         pStrInfo_base_ShowTimeRangeSummary,
                         commandShowTimeRange, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Name,
                         pStrInfo_base_timerangeName, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr,
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr,
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  return;
}

