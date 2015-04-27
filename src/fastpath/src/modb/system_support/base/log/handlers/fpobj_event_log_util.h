enum L7_event_log_members{
  EVENT,
  FILENAME,
  LINE_NUMBER,
  TASKID,
  CODE,
  DAYS,
  HOURS,
  MINS,
  SECS,
};

/*L7_UNIT_CURRENT is defined to be 0 but as per unitmgr the standalone unit is 1. 
this is causing infinite loop in case of event log get.
*/
#define L7_EVENT_LOG_UNIT_CURRENT 1

int L7_event_log_string_get(char *msg,int type,char* delim,char* outstr);
