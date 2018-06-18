/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_exports.h
*
* @purpose  Defines constants and feature definition
*
* @component
*
* @comments
*
*
* @Notes
*
* @created 07/10/2008
*
* @author aloks
* @end
*
**********************************************************************/


#ifndef RMON_EXPORTS_H
#define RMON_EXPORTS_H

/* Following are defines specific to RMON */
#define L7_MATCH_RMON_EXACTGET  1
#define L7_MATCH_RMON_GETNEXT   2
#define L7_MATCH_RMON_EXACTSET  3

#define RMON_INT_MAX_NUM                            65535
#define RMON_INT_MIN_NUM                            1
#define RMON_ALARM_ENTRY_STATUS_CREATEREQUEST       2
#define RMON_ALARM_ENTRY_STATUS_INVALID             4
#define RMON_ALARM_ENTRY_STATUS_VALID               1
#define RMON_ALARM_ENTRY_SAMPLETYPE_ABSOLUTE  1
#define RMON_ALARM_ENTRY_SAMPLETYPE_DELTA     2

#define RMON_ALARM_ENTRY_STARTUPALARM_RISING  1
#define RMON_ALARM_ENTRY_STARTUPALARM_FALLING 2
#define RMON_ALARM_ENTRY_STARTUPALARM_RISINGFALLING  3
#define RMON_ALARM_ENTRY_INTERVAL_MIN         1
#define RMON_ALARM_ENTRY_INTERVAL_MAX         2147483647
#define RMON_ALARM_ENTRY_THLD_MIN             0
#define RMON_ALARM_ENTRY_THLD_MAX             2147483647

#define RMON_EVENT_ENTRY_STATUS_VALID                   1
#define RMON_EVENT_ENTRY_STATUS_CREATEREQUEST           2
#define RMON_EVENT_ENTRY_STATUS_INVALID                 4
#define RMON_EVENT_ENTRY_OWNER_MAX_LENGTH                   127
#define RMON_EVENT_ENTRY_DESCRIPTION_MAX_LENGTH         127
#define RMON_EVENT_ENTRY_COMMUNITY_MAX_LENGTH           127

#define RMON_EVENT_ENTRY_TYPE_NONE                      1
#define RMON_EVENT_ENTRY_TYPE_LOG                       2
#define RMON_EVENT_ENTRY_TYPE_TRAP                      3
#define RMON_EVENT_ENTRY_TYPE_LOGTRAP                   4
#define RMON_LOG_QUEUE_INDEX_MAX_NUM                    L7_RMON_LOG_QUEUE_INDEX_MAX_NUM

#define RMON_HISTORY_CONTROL_DEFAULT_BUCKETSREQUESTED    50

#define RMON_HISTORY_CONTROL_INTERVAL_ONE                30
#define RMON_HISTORY_CONTROL_INTERVAL_TWO                1800

#define RMON_HISTORY_CONTROL_ENTRY_MAX_NUM               L7_RMON_HISTORY_CONTROL_ENTRY_MAX_NUM
#define RMON_HISTORY_CONTROL_STATUS_CREATEREQUEST        2
#define RMON_HISTORY_CONTROL_STATUS_VALID                1
#define RMON_HISTORY_CONTROL_STATUS_INVALID              4
#define RMON_HISTORY_CONTROL_STATUS_UNDERCREATION        3
#define RMON_MIN_BUCKETS_REQ_VALUE                       1
#define RMON_MAX_BUCKETS_REQ_VALUE                       65535
#define RMON_MIN_HISTORY_PERIOD                          1
#define RMON_MAX_HISTORY_PERIOD                          2147483647
#define RMON_HISTORY_CONTROL_INTERVAL_MAX                3600
#define RMON_HISTORY_CONTROL_INTERVAL_MIN                1
#define RMON_HISTORY_CONTROL_OWNER_MAX_LENGTH            127

#define DEFAULT_HISTORY_TABLE_INITIALIZE()               L7_TRUE

/******************** conditional Override *****************************/

#ifdef INCLUDE_RMON_EXPORTS_OVERRIDES
#include "rmon_exports_overrides.h"
#endif

#endif  /* RMON_EXPORTS_H */
