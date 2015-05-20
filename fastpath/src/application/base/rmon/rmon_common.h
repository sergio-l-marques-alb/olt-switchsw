/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_common.h
*
* @purpose  includes the declarations/defines to be used
*           internal to the application code
*
* @component RMON
*
*
* @create  07/07/2008
*
* @author  asingh
* @end
*
**********************************************************************/

#ifndef RMON__COMMON__H
#define RMON__COMMON__H

#include <l7_common.h>
#include "rmon_exports.h"
#include "defaultconfig.h"

/* rmon stats defines*/
#define RMON_ETHER_STAT_ENTRY_MAX_NUM               L7_RMON_ETHER_STAT_ENTRY_MAX_NUM
#define RMON_ETHER_STAT_STATUS_VALID                1
#define RMON_ETHER_STAT_STATUS_CREATEREQUEST        2
#define RMON_ETHER_STAT_STATUS_UNDERCREATION        3
#define RMON_ETHER_STAT_STATUS_INVALID              4
#define RMON_ETHER_STAT_DEFAULT_DATASOURCE          1
#define RMON_ETHER_STAT_OWNER_MAX_LENGTH            127

/* rmon alarm defines */
#define RMON_ALARM_ENTRY_MIN_NUM                    1
#define RMON_ALARM_ENTRY_MAX_NUM                    L7_RMON_ALARM_ENTRY_MAX_NUM
#define RMON_ALARM_ENTRY_STATUS_VALID               1
#define RMON_ALARM_ENTRY_STATUS_UNDERCREATION       3
#define RMON_ALARM_ENTRY_DEFAULT_STATUS             RMON_ALARM_ENTRY_STATUS_CREATEREQUEST
#define RMON_ALARM_ENTRY_OWNER_MAX_LENGTH           127
#define RMON_ALARM_ENTRY_VARIABLE_MAX_LENGTH        127
#define RMON_UINT_MIN_NUM                           0
#define RMON_UINT_MAX_NUM                           2147483647
#define RMON_THOUSAND                               1000
#define RMON_HUNDRED                                100

/* rmon Event defines */
#define RMON_EVENT_ENTRY_MIN_NUM                        1
#define RMON_EVENT_ENTRY_MAX_NUM                        L7_RMON_EVENT_ENTRY_MAX_NUM


#define RMON_EVENT_ENTRY_STATUS_UNDERCREATION           3
#define RMON_EVENT_CONTROL_DEFAULT_STATUS               RMON_EVENT_CONTROL_ STATUS_CREATEREQUEST

#define RMON_LOG_ENTRY_DESCRIPTION_MAX_LENGTH           127

#define RMON_LOG_QUEUE_INDEX_MIN_NUM                    1

#define RMON_FLAG_RISING_EVENT                          1
#define RMON_FLAG_FALLING_EVENT                         2

#define RMON_LOG_INDEX_MAX_NUM                          65535

/* defines for history  */
#define RMON_HISTORY_CONTROL_HUNDRED                     100
#define RMON_HISTORY_CONTROL_THOUSAND                    1000

#define RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED          L7_RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED

#define RMON_HISTORY_CONTROL_STATUS_UNDERCREATION        3

#define RMON_HISTORY_CONTROL_DEFAULT_INTERVAL            1800


/*********************************************************************
*
* @purpose  RMON Ether Statistics structure
*
* @notes none
*
* @end
*
*********************************************************************/

typedef struct rmonEtherStatsEntry_s
{
  L7_ushort16 etherStatIndex;
  L7_uint32   dataSource;
  L7_uint32   dropEvents;
  L7_uint32   octets;
  L7_uint32   pkts;
  L7_uint32   broadcastPkts;
  L7_uint32   multicastPkts;
  L7_uint32   CRCAlignErrors;
  L7_uint32   undersizePkts;
  L7_uint32   oversizePkts;
  L7_uint32   fragments;
  L7_uint32   jabbers;
  L7_uint32   collisions;
  L7_ushort16 utilization;
  L7_uint32   pkts64Octets;
  L7_uint32   pkts65to127Octets;
  L7_uint32   pkts128to255Octets;
  L7_uint32   pkts256to511Octets;
  L7_uint32   pkts512to1023Octets;
  L7_uint32   pkts1024to1518Octets;
  L7_char8    owner[RMON_ETHER_STAT_OWNER_MAX_LENGTH+1];
  L7_ushort16 status;
  L7_uint32   interface;
  L7_BOOL     validEntry;
  } rmonEtherStatsEntry_t;

/*********************************************************************
*
* @purpose  RMON Alarm structure
*
* @notes none
*
* @end
*
*********************************************************************/
typedef struct rmonAlarmEntry_s {

        L7_uint32           index;
        L7_uint32           interval;
        L7_uint32                   sampleType;
        L7_uint32                   startupAlarm;
    L7_uint32           risingEventIndex;
        L7_uint32           fallingEventIndex;
    L7_uint32               value;
        L7_uint32                   prevValue;
        L7_uint32                   prevDeltaValue;
    L7_uint32               risingThreshold;
    L7_uint32               fallingThreshold;
    L7_char8                variable[RMON_ALARM_ENTRY_VARIABLE_MAX_LENGTH+1];
    L7_char8            owner[RMON_ALARM_ENTRY_OWNER_MAX_LENGTH+1];
        L7_uint32           status;
        L7_BOOL             isRisingEvent;
    L7_BOOL                 isFallingEvent;
    L7_BOOL             justStarted;
    L7_BOOL             validEntry;
    L7_uint32           alarmTimerCount;
} rmonAlarmEntry_t;

/*********************************************************************
*
* @purpose  RMON Event  structure
*
* @notes none
*
* @end
*
*********************************************************************/

typedef struct rmonLogEntry_s {
    L7_uint32           logEventIndex;
        L7_uint32           logIndex;
        L7_char8            description[RMON_LOG_ENTRY_DESCRIPTION_MAX_LENGTH+1];
        L7_uint32                   logTime;
    L7_BOOL             validEntry;
} rmonLogEntry_t;

typedef struct rmonEventEntry_s {

        L7_uint32           index;
        L7_char8            description[RMON_EVENT_ENTRY_DESCRIPTION_MAX_LENGTH+1];
        L7_uint32           type;
        L7_char8            community[RMON_EVENT_ENTRY_COMMUNITY_MAX_LENGTH+1];
        L7_uint32                   lastTimeSent;
        L7_char8            owner[RMON_EVENT_ENTRY_OWNER_MAX_LENGTH+1];
        L7_uint32           status;
    L7_uint32           lastLogIndex;
    L7_uint32           logQueueIndex;
    rmonLogEntry_t      logQueue[RMON_LOG_QUEUE_INDEX_MAX_NUM+1];
    L7_BOOL             validEntry;
} rmonEventEntry_t;

/*********************************************************************
*
* @purpose  RMON Ether History structure
*
* @notes none
*
* @end
*
*********************************************************************/

typedef struct rmonEtherHistoryEntry_s
{
  L7_uint32   historyIndex;
  L7_uint32   historySampleIndex;
  L7_uint32   intervalStart;
  L7_uint32   dropEvents;
  L7_uint32   octets;
  L7_uint32   pkts;
  L7_uint32   broadcastPkts;
  L7_uint32   multicastPkts;
  L7_uint32   CRCAlignErrors;
  L7_uint32   undersizePkts;
  L7_uint32   oversizePkts;
  L7_uint32   fragments;
  L7_uint32   jabbers;
  L7_uint32   collisions;
  L7_ushort16 utilization;
  L7_BOOL     validEtherHistoryEntry;
  } rmonEtherHistoryEntry_t;

/*********************************************************************
*
* @purpose  RMON History Control structure
*
* @notes none
*
* @end
*
*********************************************************************/

typedef struct rmonHistoryControlEntry_s
{
  L7_ushort16                 historyControlIndex;
  L7_uint32                   dataSource;
  L7_ushort16                 bucketsRequested;
  L7_ushort16                 bucketsGranted;
  L7_ushort16                 interval;
  L7_char8                    owner[RMON_HISTORY_CONTROL_OWNER_MAX_LENGTH+1];
  L7_ushort16                 status;
  L7_uint32                   lastSampleIndex;
  L7_uint32                   lastSampleTime;
  rmonEtherHistoryEntry_t     baseline;
  rmonEtherHistoryEntry_t     bucketQueue[RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED+1];
  L7_uint32                   interface;
  L7_uint32                   bucketNumber;
  L7_uint32                   timerHistory;
  L7_BOOL                     validHistoryControlEntry;

} rmonHistoryControlEntry_t;


/*********************************************************************
*
* @purpose    Initialize the RMON Ether Stat Mapping table.
*
* @returntype L7_SUCCESS    Ether Stat Mapping table is initialized.
*             L7_FAILURE    Initialization failed.
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsTableInitialize(void);

/*********************************************************************
*
* @purpose    Remove all configured entries in the table
*
* @returntype L7_SUCCESS    Ether Stat Mapping table entries are removed
*             L7_FAILURE    Unconfiguration failed.
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsTableUnconfigure(void);

/*********************************************************************
*
* @purpose    Insert  an EtherStat entry for a configured lag in the EtherStat Table,
*             and also in the Index Mapping table with default values
*
* @param      lagInterface  internal interface number for the lag
* @param      ifIndex       external  interface number for the lag
* @param      lagSnmpIndex  corresponding SNMP index in the mapping table
*
* @returntype L7_SUCCESS    if insertion was successful
*             L7_FAILURE    if insertion was not successful
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsTableInsert(L7_uint32 lagInterface, L7_uint32 ifIndex, L7_uint32 *lagSnmpIndex);

/*********************************************************************
*
* @purpose    Remove  an EtherStat entry for an unconfigured lag from the EtherStat Table,
*             and also from the Index Mapping table with default values
*
* @param      lagSnmpIndex  corresponding SNMP index in the mapping table
*
* @returntype L7_SUCCESS    if removal was successful
*             L7_FAILURE    if removal was not successful
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsTableRemove(L7_uint32 lagSnmpIndex);

/*********************************************************************
*
* @purpose    Sets the members of EtherStat entry to default values
*
* @param      ese       EntryStatEntry for which the default values have to be set
* @param      index     index of EtherStat Entry to create
* @param      ifIndex   ifIndex value of the port which has been created
* @param      Status    Status to be set( here the status is set to VALID)
*
* @returntype L7_SUCCESS    if setting of default values was successful
*             L7_FAILURE    if setting of default values was unsuccessful
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsEntryDefaultsSet(rmonEtherStatsEntry_t *ese, L7_uint32 Index, L7_uint32 intIfNum,
                               L7_uint32 ifIndex,L7_ushort16 status);

/*********************************************************************
*
* @purpose    Deletes an EtherStat entry with the input ifIndex value
*
* @param      ifIndex   ifIndex value of the etherStat entry to be deleted
*
* @returntype L7_SUCCESS    if deletion was successful
*             L7_FAILURE    if deletion was not successful
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsEntryDelete(L7_uint32 ifIndex);

/*********************************************************************
*
* @purpose    Creates an EtherStat entry with default values
*
* @param  intIfNum  @b{(input)}  internal interface number for this entry
* @param  ifIndex   @b{(input)}  ifIndex value of the port which has been created
* @param  Status    @b{(input)} Status to be set( here the status is set to VALID)
* @param  snmpIndex @b{(output)} snmp index selected for this interface
*
* @returntype L7_SUCCESS    if creation was successful
*             L7_FAILURE    if EtherStat Entry with that index exists, or if creation
*                            was not successful
*
* @notes  Selects the next available snmp index for the interface.
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsEntryCreate(L7_uint32 intIfNum, L7_uint32 ifIndex, L7_ushort16 Status,
                          L7_uint32 *Index);



/* RMON Alarm Function */

/*********************************************************************
*
* @purpose    Initialize the RMON alarm group.
*
* @returntype L7_SUCCESS    Alarm group is initialized.
*             L7_FAILURE    Initialization failed.
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonAlarmTableInitialize(void);

/*********************************************************************
*
* @purpose    Remove all configured entries in the table
*
* @returntype L7_SUCCESS    Alarm Mapping table entries are removed
*             L7_FAILURE    Unconfiguration failed.
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonAlarmTableUnconfigure(void);

/*********************************************************************
*
* @purpose    Deletes the AlarmEntry for  the specified index
*
* @param      Index         Index of AlarmEntry to be deleted
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonAlarmEntryDelete(L7_uint32 Index);

/*********************************************************************
*
* @purpose    Creates the default AlarmEntry for  the specified index
*
* @param      Index         Index of AlarmEntry
*             alm           struct of AlarmEntry
*             status        L7_uint32 to be set as default status
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonAlarmEntryDefaultsSet(rmonAlarmEntry_t* alm, L7_uint32 Index, L7_uint32 status);

/*********************************************************************
*
* @purpose    Starts polling for the Alarm
*
* @param      alm           struct of alarmEntry
*
* @returntype void          void
*
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t
rmonAlarmStart(rmonAlarmEntry_t* alm);

/*********************************************************************
*
* @purpose    Gets the sampled value for the alarm variable
*
* @param      buf           pointer to L7_char8 (alarmVariable)
*             value(output)   pointer to L7_uint32 (alarmValue)
*
* @returntype L7_SUCCESS  if sampling is successful
*             L7_FAILURE  if sampling is not successful
*
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t
rmonAlarmSampledValueGet(L7_char8 *buf, L7_uint32 *value);

/*  RMON  Event Functions */

/*********************************************************************
*
* @purpose    Initialize the RMON event group.
*
* @returntype L7_SUCCESS    Alarm group is initialized.
*             L7_FAILURE    Initialization failed.
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEventTableInitialize(void);

/*********************************************************************
*
* @purpose    Remove all configured entries in the table
*
* @returntype L7_SUCCESS    Event Mapping table entries are removed
*             L7_FAILURE    Unconfiguration failed.
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEventTableUnconfigure(void);

/*********************************************************************
*
* @purpose    Deletes the EventEntry for  the specified index
*
* @param      Index         Index of EventEntry to be deleted
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEventEntryDelete(L7_uint32 Index);

/*********************************************************************
*
* @purpose    Creates the default EventEntry for  the specified index
*
* @param      Index         Index of EventEntry
*             evnt           struct of EventEntry
*             status        L7_uint32 to be set as default status
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEventEntryDefaultsSet(rmonEventEntry_t* evnt, L7_uint32 Index, L7_uint32 status);

 /*********************************************************************
*
* @purpose    To check whether a log queue with the given queueIndex exist
*              also it gets the corresponding logQueueNumber of the logQueue
*
* @param      evnt                      pointer to eventEntry
*             queueIndex                L7_uint32 (index of the log Queue)
*             logQueueNumber(output)    L7_uint32
*
* @returntype L7_SUCCESS    if success
*             L7_FAILURE    if failure
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonLogQueueCheck(rmonEventEntry_t* evnt ,L7_uint32 queueIndex, L7_uint32 *logQueueNumber);

/*********************************************************************
*
* @purpose    Generates Event as per the alarm raised
*
* @param      Index         index of EventEntry
*             almIndex      index of alarm entry
*             flag          determines the raising or falling event
*
* @returntype L7_SUCCESS  if member was generated
*             L7_FAILURE  if member was not generated
*
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t
rmonEventGenerate(L7_uint32 almIndex, L7_uint32 Index, L7_uint32 flag);

/*********************************************************************
*
* @purpose    Generates the LogEntry for  the specified index
*
* @param      evnt                      pointer to eventEntry
*             queueIndex                L7_uint32 (index of the log Queue)
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonLogEntryGenerate(rmonEventEntry_t* evnt, L7_uint32 queueIndex);

/*********************************************************************
*
* @purpose    Generates the Trap for  the specified index
*
* @param      almIndex      index of alarm entry
*             flag          determines the raising or falling event
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonTrapGenerate(L7_uint32 almIndex, L7_uint32 flag);

/* History functions */

/*********************************************************************
*
* @purpose    Initialize the RMON History Cintrol Mapping table.
*
* @returntype L7_SUCCESS    History Control  Mapping table is initialized.
*             L7_FAILURE    Initialization failed.
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlTableInitialize(void);

/*********************************************************************
*
* @purpose    Remove all configured entries in the table
*
* @returntype L7_SUCCESS    History Control Mapping table entries are removed
*             L7_FAILURE    Unconfiguration failed.
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlTableUnconfigure(void);

/************************************************************************************
*
* @purpose    Creates a HistoryControl entry with specified index, ifNumber, ifIndex,
*             controlInterval and Status values
*
* @param      index               index of HistoryControl Entry to create
* @param      intIfNum            internal interface number of HistoryControl Entry to create
* @param      ifIndex             ifIndex value of the port which has been created
* @param      controlInterval     sampling interval of HistoryControl Entry to create
* @param      Status              Status to be set( here the status is set to VALID)
*
* @returntype L7_SUCCESS    if creation was successful
*             L7_FAILURE    if HistoryControl Entry with that index exists, or if creation
*                            was not successful
*
* @notes
*
* @end
*
**************************************************************************************/
L7_RC_t
rmonHistoryControlEntryCreate(L7_uint32 Index, L7_uint32 intIfNum, L7_uint32 ifIndex,
                              L7_uint32 controlInterval, L7_ushort16 Status);

/*********************************************************************
*
* @purpose    Sets the members of HistoryControl entry to default values
*
* @param      hce             HistoryControlEntry for which the default values have to be set
* @param      index           index of HistoryControl Entry to create
* @param      ifIndex         ifIndex value of the port which has been created
* @param      controlInterval sampling interval for a history bucket
* @param      Status          Status to be set( here the status is set to VALID)
*
* @returntype L7_SUCCESS    if setting of default values was successful
*             L7_FAILURE    if setting of default values was unsuccessful
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlEntryDefaultsSet(rmonHistoryControlEntry_t *hce, L7_uint32 Index,
                               L7_uint32 intIfNum, L7_uint32 ifIndex,
                               L7_uint32 controlInterval, L7_ushort16 status);

/*********************************************************************
*
* @purpose    Insert  an HistoryControl entry for a configured lag in the HistoryControl Table,
*             and also in the Index Mapping table with default values
*
* @param      lagInterface  internal interface number for the lag
* @param      ifIndex       external  interface number for the lag
* @param      controlInterval       history control interval for this entry
* @param      lagSnmpIndex  corresponding SNMP index in the mapping table
*
* @returntype L7_SUCCESS    if insertion was successful
*             L7_FAILURE    if insertion was not successful
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlTableInsert(L7_uint32 lagInterface, L7_uint32 ifIndex,
                              L7_uint32 controlInterval,L7_uint32 *lagSnmpIndex);

/*********************************************************************
*
* @purpose    Remove  an HistoryControl entry for an unconfigured lag from the HistoryControl Table,
*             and also from the Index Mapping table with default values
*
* @param      lagSnmpIndex  corresponding SNMP index in the mapping table
*
* @returntype L7_SUCCESS    if removal was successful
*             L7_FAILURE    if removal was not successful
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlTableRemove(L7_uint32 lagSnmpIndex);

/*********************************************************************
*
* @purpose    Fills the baseline bucket of HistoryControl Entry
*
* @param      hce           HistoryControIEntry for which baseline bucket is being filled
*
* @returntype L7_SUCCESS    if baseline bucket filling was successful
*             L7_FAILURE    if baseline bucket filling was not successful
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlBaselineEntryFill(rmonHistoryControlEntry_t* hce);

/*********************************************************************
*
* @purpose    Fills the EtherHistory entry of HistoryControl Entry
*
* @param      hce           HistoryControIEntry for which EtherHistory entry is being filled
*             hb            EtherHistory entry being filled
*             bucketCount  this is bucketCount, or, historySampleIndex corresponding to the
*                           EtherHistory entry
*
*
* @returntype L7_SUCCESS    if bucket filling was successful
*             L7_FAILURE    if bucket filling was not successful
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryEntryFill(rmonHistoryControlEntry_t* hce,L7_uint32 bucketNumber);

/*********************************************************************
*
* @purpose    Deletes a HistoryControl entry with the input ifIndex value
*
* @param      ifIndex           ifIndex value of the HistoryControl entry to be deleted
* @param      controlInterval   sampling interval for a history bucket
*
* @returntype L7_SUCCESS    if deletion was successful
*             L7_FAILURE    if deletion was not successful
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlEntryDelete(L7_uint32 ifIndex, L7_uint32 controlInterval);

/*********************************************************************
*
* @purpose    Stop the timer for bucketqueue filling
*
* @param      hce     pointer to HistoryControlEntry object doing the polling
*
* @returntype L7_SUCCESS  always
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlBlockTimerStop(rmonHistoryControlEntry_t* hce);

/*********************************************************************
*
* @purpose    To check whether a bucket with the given SampleIndex exists or not
*             for the particular HistoryControl entry, also it gets the corresponding
*             BucketNumber of the BucketQueue
*
* @param      hce            pointer to HistoryControlEntry object doing the polling
*             SampleIndex    historyControlSampleIndex for this sample
*             BucketNumber   bucketNumber in the bucketQueue for this sampleIndex
*
* @returntype L7_SUCCESS  always
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlIsBucketAvailableCheck(rmonHistoryControlEntry_t* hce,L7_uint32 SampleIndex,
                                         L7_uint32 *BucketNumber);


#endif /* RMON__COMMON__H  */
