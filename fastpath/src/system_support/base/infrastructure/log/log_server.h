/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename log_server.h
*
* @purpose Declares log server interface
*
* @component LOG
*
* @comments none
*
* @create 1/15/2004
*
* @author McLendon
* @end
*
**********************************************************************/

#ifndef __L7_LOG_SERVER_H__
#define __L7_LOG_SERVER_H__

/*************************************************************

*************************************************************/

#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "log_cfg.h"

/* Operational data type */

#define EMAIL_ALERT_LOG_TIME     60
#define EMAIL_ALERT_LOG_BUFFER_SIZE     1000
#define EMAIL_ALERT_SMTP_AUTHENTICATION_CMD  "AUTH PLAIN "
#define EMAIL_ALERT_SMTP_AUTHENTICATION_LOGIN_CMD  "AUTH LOGIN"
#define EMAIL_ALERT_SMTP_EHLO_CMD      "EHLO "
#define EMAIL_ALERT_SMTP_FROM_CMD      "MAIL FROM:"
#define EMAIL_ALERT_SMTP_RCPT_CMD       "RCPT TO:"
#define EMAIL_ALERT_SMTP_DATA_CMD      "data"
#define EMAIL_ALERT_SMTP_OK                   "220"
#define EMAIL_ALERT_SMTP_CODE_220                   "220"
#define EMAIL_ALERT_SMTP_CODE_250                   "250"
#define EMAIL_ALERT_SMTP_CODE_354                   "354"
#define EMAIL_ALERT_SMTP_CODE_235                   "235"
#define EMAIL_ALERT_SMTP_STARTTLS_CMD      "STARTTLS"

struct logOps_s
{
  /* Per host count of syslog messages relayed. */
  L7_uint32   syslogMessageForwardCount;

  /* Per host count of syslog messages ignored. */
  L7_uint32   syslogMessageIgnoredCount;

  /* The resolved address of the syslog collector in internal format */
  /* Set to L7_NULL_IP_ADDR if not resolved or on local socket close. */
  L7_uint32   remoteAddr;
};

typedef enum 
{
    LOG_MSG_EVENT = 0,
    LOG_TIMER_EVENT  
} logMessages_t;

typedef enum 
{
    EMAIL_ALERT_TRIGGER_TIMER = 0,
    EMAIL_ALERT_TRIGGER_BUFFER_FULL,
    EMAIL_ALERT_TRIGGER_EMERGENCY_MESSAGE
} emailAlertTrigger_t;


/**********************************************************************
* @purpose record a message from another stack member to the in-memory log.
*
* @end
*********************************************************************/

#if defined(L7_STACKING_PACKAGE)
extern void logMsgBuffer(L7_enetMacAddr_t src_key,
                         L7_uchar8 * msgbuf, L7_uint32 msglen);
#endif


/**********************************************************************
* @purpose  Fills out the Config data header with defaults
*
* @end
*********************************************************************/

extern void logCfgHeaderDefaultsSet(L7_uint32 ver);


/**********************************************************************
* @purpose  Fills out the Config data header with defaults
*
* @end
*********************************************************************/

extern void logCfgDefaultsSet(L7_uint32 ver);

/**********************************************************************
* @purpose  Checks if LOG config data has changed
*
* @end
*********************************************************************/

extern L7_BOOL logCfgHasDataChanged(void);

/**********************************************************************
* @purpose  Marks cfg data as changed
*
* @end
*********************************************************************/

extern void logCfgMarkDataChanged();

/**********************************************************************
* @purpose  Clear the statistics data.
*
* @end
*********************************************************************/

extern void logStatisticsClear();

/**********************************************************************
* @purpose  Return the number of logged messages and increment the counter
*
* @end
*********************************************************************/

L7_uint32 logMessagesReceivedInc();

/**********************************************************************
* @purpose  Attempt to obtain the LOG server mutex
*
* @end
*********************************************************************/

extern L7_RC_t logTaskLock();

/**********************************************************************
* @purpose  Release the LOG server task mutex
*
* @end
*********************************************************************/

extern L7_RC_t logTaskUnlock();

/**********************************************************************
* @purpose  Change the size of the in-memory log buffer pool.
*
* @end
*********************************************************************/

extern L7_RC_t logBufferPoolResize( L7_uint32 size );

/**********************************************************************
* @purpose  Initialize the logging subsystem
*
* @end
*********************************************************************/

extern L7_RC_t logServerInit();


/**********************************************************************
* @purpose  This routine closes the local socket.
*
* @end
*********************************************************************/

extern L7_RC_t logLocalSocketClose();

/**********************************************************************
* @purpose  get count of in memory log messages
*
* @end
*********************************************************************/

extern L7_RC_t logServerLogInMemoryCountGet(L7_uint32 * count);
/**********************************************************************
* @purpose  get count of received messages
*
* @end
*********************************************************************/

extern L7_RC_t logServerLogMessagesReceivedGet(L7_uint32 * count);
/**********************************************************************
* @purpose  get count of dropped messages
*
* @end
*********************************************************************/

extern L7_RC_t logServerLogMessagesDroppedGet(L7_uint32 * count);

/**********************************************************************
* @purpose  get count of syslog messages relayed
*
* @end
*********************************************************************/

extern L7_RC_t logServerLogSyslogMessagesRelayedGet(L7_uint32 * count);

/**********************************************************************
* @purpose  get last message received timestamp
*
* @end
*********************************************************************/

extern L7_RC_t logServerLogMessageReceivedTimeGet(L7_uint32 * tstamp);

/**********************************************************************
* @purpose  get last syslog message delivered timestamp
*
* @end
*********************************************************************/

extern L7_RC_t logServerLogSyslogMessageDeliveredTimeGet(L7_uint32 * tstamp);

/**********************************************************************
* @purpose  Get the "next" row in the in-memory log table.
*
* @end
*********************************************************************/

extern L7_RC_t logServerLogMsgInMemoryGetNext(L7_uint32 ndx, L7_char8 * buf,
                                  L7_uint32 * bufNdx);

/**********************************************************************
* @purpose  Get the specified row in the in-memory log table.
*
* @end
*********************************************************************/

extern L7_RC_t logServerLogMsgInMemoryGet(L7_uint32 ndx, L7_char8 * buf);

/**********************************************************************
* @purpose  Clear the buffered log.
*
* @end
*********************************************************************/

extern L7_RC_t logServerLogInMemoryClear();

/**********************************************************************
* @purpose  get count of dropped messages
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
* @notes  Returns count of messages not logged due to severity
*
* @end
*********************************************************************/

L7_RC_t logServerLogInMemoryMessagesDroppedGet( L7_uint32 *count );

/**********************************************************************
* @purpose  get count of dropped messages
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
* @notes  Returns count of messages not logged due to severity
*
* @end
*********************************************************************/

L7_RC_t logServerLogPersistentMessagesDroppedGet( L7_uint32 *count );

/**********************************************************************
* @purpose  get count of dropped messages
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
* @notes  Returns count of messages not logged due to severity
*
* @end
*********************************************************************/

L7_RC_t logServerLogConsoleMessagesDroppedGet( L7_uint32 *count );

  
/**********************************************************************
* @purpose  get count of dropped messages for a given syslog server 
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                 from[1..L7_LOG_MAX_HOSTS]  
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
* @notes  Returns count of messages not logged for a given syslog server
*         due to socket delivery issues
*
* @end
*********************************************************************/
 L7_RC_t logServerLogSyslogMessagesDroppedGet(L7_uint32 ndx,L7_uint32 *count );

#endif
