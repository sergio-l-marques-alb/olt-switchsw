/*********************************************************************
 **********************************************************************
 * @file     log_server.c
 *
 * @purpose   Log server main implementation
 *
 * @component LOG
 *
 * @comments The LOG system is implemented as a front-end (runs in the callers
 *           task) and a back end (runs in the LOG task). The front end
 *           is responsible for logging messages to the persistent log
 *           and the in-memory log. The back end is responsible for
 *           logging messages to any syslog hosts and logging messages
 *           to the console.
 *
 *           Upon receipt of a message (logmsg invocation), the message
 *           is tested against the severity/component filters.
 *           If it does not pass any of the filters, logmsg simply
 *           returns. If an invocation passes at least one filter,
 *           a buffer is allocated and the message is formatted
 *           into the allocated buffer. Following that, if the
 *           persistent filter is passed, the message is logged into
 *           the appropriate persistent log. If the in-memory filter
 *           is passed, the message is logged into the in-memory log.
 *           If either the console or syslog filters are passed, the
 *           message buffer is queued to the back end for processing.
 *
 *
 * create 2/3/2004
 *
 * author Mclendon
 *
 **********************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "l7_common.h"
#include "l7_resources.h"
#include "flex.h"
#include "osapi.h"
#include "osapi_support.h"
#include "l7_cnfgr_api.h"
#include "default_cnfgr.h"
#include "sysapi.h"
#include "sysapi_hpc.h"
#include "simapi.h"
#include "unitmgr_api.h"
#include "usmdb_cnfgr_api.h"
#include "usmdb_unitmgr_api.h"
#include "cli_web_mgr_api.h"
#include "osapi_sem.h"

#include "log.h"
#include "log_cfg.h"
#include "usmdb_dns_client_api.h"
#include "l7utils_api.h"
#include "log_server.h"

#ifdef L7_MGMT_SECURITY_PACKAGE
int b64_ntop(u_char const *src, size_t srclength, char *target,size_t targsize);
int b64_pton(char const *src, u_char *target, size_t targsize);

int tls1_new(SSL *s);
void tls1_clear(SSL *s);

L7_int32 emailAlertSMTPServerSecureDataSend(SSL *ssl, L7_uchar8 *buf, L7_uint32 size);
L7_int32 emailAlertSMTPServerSecureDataRecv(SSL *ssl, L7_uchar8 *buf, L7_uint32 size);
#endif
L7_RC_t emailAlertTimerStart(void);
L7_RC_t emailAlertServerPollTimerStart(void);
L7_RC_t emailAlertServerPollTimerStop(void);
void loggingEmailAlertTraceWrite(emailAlertTraceFlags_t flag,L7_uchar8 *emailTrace);
void emailAlertDebugHelp(void);
void loggingEmailAlertTraceFlagsSet(L7_uint32 flags);

/** Global configuration parameters. */

/* The "name" of the queue. */
#define LOG_QUEUE     "LogQ"

/* Extra buffers for passing messages to the back end when the
 *   in-memory log is full. THis is also the size of the back end queue.
 */

#define LOG_EXTRA_BUFFERS 32

/* Log messages passed across systems in a stack have a
 * 4 byte header (2 bytes severity and 2 bytes component).
 */
#define LOG_STACK_HEADER_LEN  4

/* This struct declares the message passed to the backend task
 * for processing. Since the buffer (buf) is already preformatted,
 * we keep some of the more interesting items in native machine
 * format for filter and output processing.
 */
#define MAX_NUMBER_OF_LOG_MESSAGES  256

#define EMAIL_ALERT_MAX_TRACE_LEN   512
struct logMsg_s
{
  logMessages_t                msgtype;
  L7_LOG_SEVERITY_t       severity;
  L7_COMPONENT_IDS_t      component;
  L7_ushort16             msgLen;
  L7_char8 *              buffer;
  L7_char8                freeAfterWrite;
};

osapiTimerDescr_t *emailAlertTimer = L7_NULLPTR;
osapiTimerDescr_t *emailAlertServerPollTimer = L7_NULLPTR;
/* Are we properly initialized */
static L7_BOOL  logServerEnabled = L7_FALSE;

/* Pointer to the configuration data. */
static struct logCfg_s * logCfg = L7_NULL;

/* Pointer to the operational data. */
static struct logOps_s  * logOps = L7_NULL;

/* The queue for internal log messages. The backend
 * task (logTask) handles the slower message processing tasks.
 */
static void * logQueue = L7_NULL;

/*
    Folloing global buffer is to hold the non urgent log messages
    till they are sent to the SMTP server.
 */
static struct logMsg_s emailAlertBuffer[MAX_NUMBER_OF_LOG_MESSAGES+1];


L7_int32  emailAlertMsgCount;

emailAlertingStatsData_t emailAlertStats;
emailAlertingOperData_t emailAlertOperData;

  /* email alert tracing */

emailAlertTraceFlags_t emailAlertTraceFlags;


/** Global Status Parameters that are actual variables.
 * (Some are manifest constants).
 */

/* - We also use this for the ordinal numbering of messages */
static L7_uint32  logMessagesReceived  = 0;

/* - The counts of messages that could not be processed
 * by the log entity for various reasons.
 */
static L7_uint32  logMessagesToStackMgrDropped      = 0;
static L7_uint32  logMessagesResourceIssuesDropped  = 0;
static L7_uint32  logMessagesLoggingDisabledDropped = 0;

/*
 * Counts of messages dropped by each logging type.
 */
static L7_uint32  logMemMessagesIgnored     = 0;
static L7_uint32  logPersMessagesIgnored    = 0;
static L7_uint32  logConsoleMessagesIgnored = 0;

/* - The time of receipt of the last message. */
static L7_uint32  logReceiveTimestamp = 0;

/* - The count of messages relayed by the syslog entity */
static L7_uint32  logSyslogMessagesRelayed = 0;

/* - The time that a message was last delivered
 * to a syslog collector/relay.
 */
static L7_uint32  logSyslogMessageDeliveredTimestamp = 0;


/* Our mutex to protect access to shared data structures. */
static void * logTaskSyncSemaphore = L7_NULL;

/* Our task id */
static L7_int32  logServerTaskId = 0;

/** Local variables used for communication */

/** Our socket. We flag a closed socket with a -1. */
static int fd = -1;

/** Client local address. */
static L7_ulong32 localAddr;


/* We keep an array of pointers to the actual buffers
 * (allocated from a pool).
 */
static L7_char8 ** inMemoryLog = L7_NULL;

static L7_uint32  inMemoryLogCount = 0;

/*
 * The actual size of the current in memory buffer.  This may be different
 * from the configured size if the configuration has changed but a reset has
 * not yet been done.
 */

static L7_uint32  inMemoryLogSize = 0;

/** hhead ranges from [0..inMemoryLogSize-1] and is
 ** used to index the in memory log data. hhead is the
 ** index of the "next" entry in which to place a log entry.
 ** We don't really use a tail. The tail is 0 until the
 ** queue fills, and then it becomes hhead + 1 mod
** inMemoryLogSize. hhead is readable at any time.
 ** It is writable only with interrupts disabled.
 */

static L7_uint32 hhead = 0;

/*
 *
 * Normally, we would simply call the system buffer pool functions
 * to do buffer stuff,  but they potentially block and they very
 * definitely call logging - that's us.
 * While we love infinite recursive loops as much as the next guy,
 * we don't think it's all that appropriate for a base level
 * system utility. Instead, we use a completely non-blocking
 * and extremely lightweight buffer allocation scheme. See below.
 *
 */

/* A little overlay - it's not too tricky, I hope. See "push" below. */
struct block_s
{
  struct block_s * next;
};

/** This is the head of the free list. When a block is
 * allocated, we attempt to get it from the free list.
 * If the free list is exhausted, we get it from the
 * pool. See logBufferAlloc below for details.
 */

static struct block_s * poolHead = L7_NULL;

#if 0  /* TBD: Unused */
static L7_char8 * logFormatArgs(L7_char8 * format, ...);
#endif

/* Generic message displayed in log when problem encountered during formatting
 * of real log message
 */
static L7_char8 *pLogMsgFmtError = "(Unable to format log message)";

/*
 *  Initialize to logging config
 */
extern void logCfgInit(struct logCfg_s * cfg, struct logOps_s * ops);

/* Externs for persistent log code which was moved to the BSP */
/* This is crossing boundaries, but persistent logging belongs in BSP,not in application */
extern void logPersistentStatsClear();
extern void logPersistentInit();
extern L7_RC_t logWritePersistentLogsToConsole();
extern L7_RC_t logWriteMsgToFlash(L7_char8 * buf, L7_int32 len);
extern L7_uint32 logServerPersistentLogMessageCount();
extern L7_BOOL logCnfgrStateCheck (void);

#ifdef L7_DOT3AH_PACKAGE
extern L7_RC_t dot3ahDyingGaspCallBack( L7_uint32 eventNum, L7_uint32 component, L7_BOOL isComponent);
#endif

/**********************************************************************
 * @purpose  Attempt to obtain the LOG server mutex
 *
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FALIURE
 *
 * @notes    This is a blocking call. Caller must NOT hold the LOG
 *           server mutex.
 *
 * @end
 *********************************************************************/

L7_RC_t logTaskLock()
{
  return osapiSemaTake(logTaskSyncSemaphore, L7_WAIT_FOREVER);
}
/**********************************************************************
 * @purpose  Release the LOG server task mutex
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FALIURE
 *
 * @notes    Caller must hold the LOG server mutex
 *
 * @end
 *********************************************************************/

L7_RC_t logTaskUnlock()
{
  return osapiSemaGive(logTaskSyncSemaphore);
}

/**********************************************************************
 * @purpose Deallocate a buffer by pushing it onto the free list.
 *
 * @param  buf @b{(input)} pointer to buffer to push.
 *
 * @returns  nothing
 *
 * @notes  The buffer parameter must havebeen allocated by pop or
 *         or from the pool. See logBufferAlloc.
 *
 * @end
 *********************************************************************/

static void push(void * buf)
{
  struct block_s * block = (struct block_s *)buf;
  block->next = poolHead;
  poolHead = block;
}

/**********************************************************************
 * @purpose Allocate a buffer by getting it from the free list.
 *
 * @returns  allocated buffer or L7_NULL if free list is empty.
 *
 * @notes   The pool must be allocate (poolHead initialized).
 *           This precondition is checked.
 *
 * @end
 *********************************************************************/
static void * pop()
{
  struct block_s * block = L7_NULL;

  if (poolHead != L7_NULL)
  {
    block = poolHead;
    poolHead = poolHead->next;
  }
  return(void *)block;
}

/****************  Buffer Pool allocation and manipulation ************/

/*
 * The message pool consists of a pool of equal sized slices of memory.
 * This memory holds the formatted in memory log message text.
 * The inMemoryLog is an array of pointers to those slices.
 */

/* Pointer to the buffer pool. This is a single fixed size block. */
static void * messageBuffers = L7_NULL;

/** The single fixed size pool defined above is chopped into lot's of
 * twisty little passages, er... blocks, each one the same size. This is
 * the index to the next free (unallocated) buffer in the pool.
 */
static L7_uint32 messageBuffersIndex = 0;

/**********************************************************************
 * @purpose Deallocate the in-memory log buffer pool.
 *
 * @returns  nothing
 *
 * @notes  This routine deallocates the buffer pool for the in memory logs.
 *         The internal bookkeeping variable is reset.
 *
 * @end
 *********************************************************************/

static void logBufferPoolFree()
{
  if (messageBuffers)
    osapiFree(L7_LOG_COMPONENT_ID, messageBuffers);
  messageBuffers = L7_NULL;
  if (inMemoryLog)
    osapiFree(L7_LOG_COMPONENT_ID, inMemoryLog);
  inMemoryLog = L7_NULL;
  inMemoryLogSize = 0;
}

/* Adding a similer kind of frame work for persistent logging */
/**********************************************************************
 * @purpose Allocate an in-memory log buffer pool and the in memory log
 *          buffer space.
 *
* @param   size @b((input)) the number of log entries in the pool.
*
 * @returns  L7_SUCCESS if buffer allocated.
 * @returns  L7_FAILURE if buffer not allocated.
 *
 * @notes  This routine allocates a buffer pool for the in memory logs.
 *         If a buffer pool has been previously allocated, it is freed
 *         first. The buffer pool is allocated n the heap.
 *
 * @end
 *********************************************************************/

static L7_RC_t logBufferPoolAllocate( L7_uint32 size )
{
  messageBuffers      = osapiMalloc( L7_LOG_COMPONENT_ID,
                                     ( size + LOG_EXTRA_BUFFERS ) * ( L7_LOG_MESSAGE_LENGTH + LOG_STACK_HEADER_LEN ) );
  inMemoryLog         = osapiMalloc( L7_LOG_COMPONENT_ID, size * sizeof(L7_char8 *) );
  poolHead            = L7_NULL;
  messageBuffersIndex = 0;
  hhead               = 0;
  inMemoryLogSize     = size;
  return( ( messageBuffers && inMemoryLog ) ? L7_SUCCESS : L7_FAILURE );
}

/**********************************************************************
 * @purpose Deallocate all buffers in the pool.
 *
 * @returns  nothing.
 *
 * @notes  This routine is reentrant and thread safe.
 *         This routine is relatively lightweight, but probably
 *         should not be called from an interrupt.
 *
 *         This routine resets the internal buffer pool bookeeping.
 *         All allocated buffers are lost (not leaked).
 *         After this routine is called, it is as if no buffer was ever
 *         allocated from the pool.
 *
 * @preconditions The buffer pool must have been allocated via a call to
 *         logBufferPoolAllocate prior to use of this routine.
 *         This precondition is not checked.
 * @end
 *********************************************************************/

static void logBufferPoolReset()
{
  L7_uint32 key = osapiDisableInts();
  poolHead = L7_NULL;
  messageBuffersIndex = 0;
  hhead = 0;
  if (inMemoryLog)
    memset(inMemoryLog, '\0', inMemoryLogSize * sizeof(L7_char8 *));
  if (messageBuffers)
    memset(messageBuffers, '\0', (inMemoryLogSize + LOG_EXTRA_BUFFERS) *
           (L7_LOG_MESSAGE_LENGTH + LOG_STACK_HEADER_LEN));

  /* Clear Count varible also*/
  inMemoryLogCount = 0;
  logMessagesReceived = 0;
  osapiEnableInts(key);
}


/**********************************************************************
 * @purpose Allocate a fixed-size buffer from the pool.
 *
 * @returns  pointer to buffer if allocated - L7_NULL if not.
 *
 * @notes  This routine is reentrant and thread safe.
 *         I chose to disable interrupts here to make this routine
 *         callable in interrupt context. This routine is very
 *         lightweight.
 *
 *         The buffer pool allocation uses a MRU algorithm. Freed buffers
 *         are kept on a free list and reused. When the free list is
 *         exhausted, another buffer is allocated from the pool.
 *
 * @preconditions The buffer pool must have been allocated via a call to
 *         logBufferPoolAllocate prior to use of this routine.
 *         This precondition is not checked.
 * @end
 *********************************************************************/
static void * logBufferAlloc()
{
  L7_uint32 key = osapiDisableInts();
  void * block = pop();
  if (block == L7_NULL)
  {
    if (messageBuffersIndex < (inMemoryLogSize + 16))
    {
      block = (void *)((L7_uchar8 *)messageBuffers + (messageBuffersIndex++ *
                                                      (L7_LOG_MESSAGE_LENGTH + LOG_STACK_HEADER_LEN)));
    }
  }
  osapiEnableInts(key);
  return block;
}

/**********************************************************************
* @purpose Allocate a fixed-size buffer from the pool without locks.
*
* @returns  pointer to buffer if allocated - L7_NULL if not.
*
* @notes  This routine is reentrant and thread safe.
*         I chose to disable interrupts here to make this routine
*         callable in interrupt context. This routine is very
*         lightweight.
*
*         The buffer pool allocation uses a MRU algorithm. Freed buffers
*         are kept on a free list and reused. When the free list is
*         exhausted, another buffer is allocated from the pool.
*
* @preconditions The buffer pool must have been allocated via a call to
*         logBufferPoolAllocate prior to use of this routine.
*         This precondition is not checked.
* @end
*********************************************************************/
static void * logBufferAllocNoLock()
{
  /* L7_uint32 key = osapiDisableInts();*/
  void * block = pop();
  if (block == L7_NULL)
  {
    if (messageBuffersIndex < (inMemoryLogSize + 16))
    {
      block = (void *)(messageBuffers + (messageBuffersIndex++ *
                                         (L7_LOG_MESSAGE_LENGTH + LOG_STACK_HEADER_LEN)));
    }
  }
  /*osapiEnableInts(key);*/
  return block;
}

/**********************************************************************
 * @purpose Deallocate a buffer (return it to the pool).
 *
 * @param  b @b{(input)} Pointer to buffer - must have been obtained
 *                 by a call to logBufferAlloc
 *
 * @returns  nothing
 *
 * @notes  This routine is reentrant and thread safe.
 *         I chose to disable interrupts here to make this routine
 *         callable in interrupt context. This routine is very lightweight.
 * @end
 *********************************************************************/
static void logBufferFree(void * b)
{
  if (b)
  {
    L7_uint32 key = osapiDisableInts();
    push(b);
    osapiEnableInts(key);
  }
}

/**********************************************************************
* @purpose Change the size of the in-memory log buffer pool.
*
* @returns  L7_SUCCESS if buffer allocated.
* @returns  L7_FAILURE if buffer not allocated.
*
* @notes  This routine allocates a buffer pool for the in memory logs.
*         The buffer pool is allocated from the heap.
*
* @end
*********************************************************************/

L7_RC_t logBufferPoolResize( L7_uint32 size )
{
  void       *oldMessageBuffers  = messageBuffers;
  L7_char8  **oldInMemoryLog     = inMemoryLog;
  L7_uint32   oldInMemoryLogSize = inMemoryLogSize;
  L7_char8   *buf = L7_NULLPTR;

  L7_uint32   msgNdx;
  L7_uint32   ndx;
  L7_RC_t     rc;
  L7_uint32   key;

  if ( size == inMemoryLogSize )
  {
    return( L7_SUCCESS );
  }

  key = osapiDisableInts();

  if (inMemoryLogCount == 0)
  {
    logBufferPoolFree();
    rc = logBufferPoolAllocate( size );
    osapiEnableInts(key);
    return( rc );
  }

  if (hhead != 0)
  {
    msgNdx = hhead;
  }
  else
  {
    msgNdx = oldInMemoryLogSize;
  }

  rc = logBufferPoolAllocate( size );


  if ( L7_SUCCESS != rc )
  {
    osapiEnableInts(key);
    return( rc );
  }

  for ( ndx = msgNdx; ndx < oldInMemoryLogSize; ndx++ )
  {
    if ( NULL != oldInMemoryLog[ndx] )
    {
      if ( NULL != inMemoryLog[hhead] )
      {
        logBufferFree( inMemoryLog[hhead] );
      }
      buf=logBufferAllocNoLock();
      if (buf!=L7_NULLPTR)
      {
        memcpy(buf, oldInMemoryLog[ndx],LOG_STACK_HEADER_LEN+L7_LOG_MESSAGE_LENGTH);
        inMemoryLog[hhead] = buf;
        hhead = ((hhead + 1) >= inMemoryLogSize) ? 0 : hhead + 1;
      }
      oldInMemoryLog[ndx]=L7_NULL;
    }
  }

  for ( ndx = 0; ndx < msgNdx; ndx++ )
  {
    if ( NULL != oldInMemoryLog[ndx] )
    {
      if ( NULL != inMemoryLog[hhead] )
      {
        logBufferFree( inMemoryLog[hhead] );
      }
      buf=logBufferAllocNoLock();
      if (buf!=L7_NULLPTR)
      {
        memcpy(buf, oldInMemoryLog[ndx],LOG_STACK_HEADER_LEN+L7_LOG_MESSAGE_LENGTH);
        inMemoryLog[hhead] = buf;
        hhead = ((hhead + 1) >= inMemoryLogSize) ? 0 : hhead + 1;
      }
      oldInMemoryLog[ndx]=L7_NULL;

    }
  }

  osapiEnableInts(key);

  osapiFree( L7_LOG_COMPONENT_ID, oldMessageBuffers );
  osapiFree( L7_LOG_COMPONENT_ID, oldInMemoryLog );

  return( L7_SUCCESS );
}

/**********************************************************************
 * @purpose  Clear the statistics data.
 *
 * @returns  none
 *
 * @notes  This is a helper function
 *
 * @end
 *********************************************************************/

void logStatisticsClear()
{
  register L7_uint32 ndx;
  /** Initialize time history */
  for (ndx = 0; ndx < L7_LOG_MAX_HOSTS; ndx++)
  {
    logOps[ndx].syslogMessageForwardCount =
    logOps[ndx].syslogMessageIgnoredCount = 0;
  }

  logReceiveTimestamp               = 0;
  logSyslogMessagesRelayed          = 0;
  logSyslogMessageDeliveredTimestamp= 0;

  logMessagesToStackMgrDropped      = 0;
  logMessagesResourceIssuesDropped  = 0;
  logMessagesLoggingDisabledDropped = 0;

  logMemMessagesIgnored     = 0;
  logPersMessagesIgnored    = 0;
  logConsoleMessagesIgnored = 0;
  logPersistentStatsClear();
}

/*
 * These macros are used to test various conditions for the logs.
 * Specifically, they test whether a message should be logged to the
 * selected log or not. If the message should be logged, then these
 * macros return a non-zero (true) value. If the message should not
 * be logged, then these macros return a zero (false) value.
 * The order of the tests reflects my best guess as to which items
 * are most likely to be "true."
 */

#define MEMORY_LOG_ACTIVE(__sev__, __cmp__)                             \
  ((logCfg) &&                                                          \
   ((logCfg->cfg.inMemorySeverityFilter >= (__sev__)) &&                 \
   (logCfg->cfg.inMemoryAdminStatus == L7_ADMIN_MODE_ENABLE) &&         \
    ((logCfg->cfg.inMemoryComponentFilter == L7_ALL_COMPONENTS) ||        \
     (logCfg->cfg.inMemoryComponentFilter  == (__cmp__))) &&              \
  ((logCfg->cfg.inMemoryBehavior == L7_LOG_WRAP_BEHAVIOR)  ||           \
     (inMemoryLogCount <= L7_LOG_IN_MEMORY_LOG_COUNT))))


#define PERSISTENT_LOG_ACTIVE(__sev__)                                  \
  ((logCfg) &&                                                          \
   ((logCfg->cfg.persistentSeverityFilter >= (__sev__)) &&               \
    (logCfg->cfg.persistentAdminStatus == L7_ADMIN_MODE_ENABLE)))

/* Check the operational parameter debugConsoleSeverityFilter for
   'debug console' logging */
#define CONSOLE_LOG_ACTIVE(__sev__, __cmp__)                            \
  ((logCfg) &&                                                          \
   ((logCfg->cfg.consoleAdminStatus == L7_ADMIN_MODE_ENABLE) &&          \
    (logCfg->cfg.consoleSeverityFilter >= (__sev__)) &&                  \
    ((logCfg->cfg.consoleComponentFilter == L7_ALL_COMPONENTS) ||        \
     (logCfg->cfg.consoleComponentFilter == (__cmp__)))))

#define SYSLOG_LOG_ACTIVE()                                             \
  ((logCfg) &&                                                          \
   (logCfg->cfg.syslogAdminStatus == L7_ADMIN_MODE_ENABLE))

#define EMAILALERT_LOG_ACTIVE()                                             \
  ((logCfg) &&                                                          \
   (logCfg->cfg.emailAlertInfo.emailAlertingStatus == L7_ADMIN_MODE_ENABLE))


#define SYSLOG_FILTER_MATCH(__ndx__, __sev__, __cmp__)                  \
  ((logCfg) &&                                                          \
  ((logCfg->cfg.host[__ndx__].status == L7_ROW_STATUS_ACTIVE) &&        \
   (logCfg->cfg.host[__ndx__].severityFilter >= (__sev__)) &&           \
   ((logCfg->cfg.host[__ndx__].componentFilter == L7_ALL_COMPONENTS) || \
    (logCfg->cfg.host[__ndx__].componentFilter == (__cmp__)))))

/**********************************************************************
 * @purpose Check if display of traces is enabled on any connection
 *
 * @param   sev @b{(input)} severity level
 *
 * @returns  L7_TRUE  - if enabled on any connection
 *           L7_FALSE - if not enabled on any connection
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_BOOL logDebugTraceDisplayIsActive(L7_LOG_SEVERITY_t sev)
{

  if (sysapiDebugSupportDisplayModeGet() == L7_ENABLE)
    return L7_TRUE;

  return L7_FALSE;
}

/**********************************************************************
 * @purpose Obtain a formatted string.
 *
 * @param  format @b{(input)} format string
 *
 * @returns  pointer to a formatted string
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_char8 * logFormatArgs(L7_char8 * format, ...)
{
  static L7_char8  buf[L7_LOG_FORMAT_BUF_SIZE];
  va_list ap;
  L7_int32 rc;

  va_start(ap, format);
  rc = osapiVsnprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  if (rc < 0)
    (void)osapiStrncpySafe(buf, pLogMsgFmtError, sizeof(buf));

  return buf;
}


/**********************************************************************
 * @purpose Obtain a formatted date string.
 *
 * @param  now @b{(input)} time of the event (Unix Epoch).
 *
 * @returns  pointer to a formatted string
 *
 * @notes  This is probably really slow and also puts the year into the string.
 *         We don't need the year or the first four bytes of the returned string.
 *
 * @end
 *********************************************************************/
L7_char8 * logDateString(L7_uint32 now)
{
  return ctime((void *)&now) + 4;
}

/**********************************************************************
 * @purpose Obtain the configured syslog default facility
 *
 * @returns  the default facility
 *
 * @notes  This is an internal helper function - it's not a
 *         management function. Note the logging does NOT need to
 *         be initialized in order for this o run.
 *
 * @end
 *********************************************************************/

L7_LOG_FACILITY_t logDefaultFacilityGet()
{
  return(logCfg) ? logCfg->cfg.syslogDefaultFacility : FD_LOG_DEFAULT_LOG_FACILITY;
}

/**********************************************************************
 * @purpose Log an event.
 *
 * @param  severity @b((input)} See RFC 3164 Section 4.1.1 Table 2
 * @param  component @b((input)} Level 7 component id
 * @param  fileName @b{(input)} file name
 * @param  lineNum @b{(input)} line number
 * @param  eventCode @b{(input)} event code
 *
 * @returns  None
 *
 * @notes  This executes on the calling task thread. This routine may
 *         be called from interrupt context. We make no assumption
 *         about the callers memory storage for any arguments.
 *         This routine makes a copy of all arguments and formats
 *         them into a locally allocated buffer.
 *
 * @end
 *********************************************************************/

void l7_log_event(L7_LOG_SEVERITY_t severity, L7_COMPONENT_IDS_t component,
                  L7_char8 * fileName, L7_uint32 lineNum, L7_ulong32 eventCode)
{
  L7_char8 buf[L7_LOG_FORMAT_BUF_SIZE];
  L7_int32 rc;

#ifdef L7_DOT3AH_PACKAGE
  if (eventCode == L7_LOG_SEVERITY_EMERGENCY)
  {
    dot3ahDyingGaspCallBack(L7_LOG_SEVERITY_EMERGENCY, component, L7_TRUE);
    osapiSleep(2);
  }
#endif /* L7_DOT3AH_PACKAGE */
  rc = osapiSnprintf(buf, sizeof(buf), "Event(0x%lx)", eventCode);
  if (rc < 0)
    l7_log(severity, component, fileName, lineNum, pLogMsgFmtError);
  else
    l7_log(severity, component, fileName, lineNum, buf);

  /* Also log to FASTPATH event log */
  log_event(fileName, lineNum, eventCode);
}


/**********************************************************************
 * @purpose Log a message using a format string and variable argument list.
 *
 * @param  severity @b((input)} See RFC 3164 Section 4.1.1 Table 2
 * @param  component @b((input)} Level 7 component id
 * @param  fileName @b{(input)} file name
 * @param  lineNum @b{(input)} line number
 * @param  format @b{(input)} format string
 * @param  ... @b{(input)} additional arguments (per format string)
 *
 * @returns  None
 *
 * @notes  This executes on the calling task thread. This routine may
 *         be called from interrupt context. We make no assumption
 *         about the callers memory storage for any arguments.
 *         This routine makes a copy of all arguments and formats
 *         them into a locally allocated buffer.
 *
 * @end
 *********************************************************************/

/* PTin added: debug */
#if 1
static L7_BOOL logf_debug = L7_TRUE;

void l7_logf_enable(L7_BOOL enable)
{
  logf_debug = enable;
  printf("l7_logf enable set to %u!\r\n", enable);
}
#endif

void l7_logf(L7_LOG_SEVERITY_t severity, L7_COMPONENT_IDS_t component,
             L7_char8 * fileName, L7_uint32 lineNum, L7_char8 * format, ...)
{
  L7_char8 buf[L7_LOG_FORMAT_BUF_SIZE];
  va_list ap;
  L7_int32 rc;

  if (logf_debug)	/* PTin added: debug */
  {
    va_start(ap, format); 
    rc = osapiVsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);

    if (rc < 0)
      l7_log(severity, component, fileName, lineNum, pLogMsgFmtError);
    else
      l7_log(severity, component, fileName, lineNum, buf);
  }
}


/**********************************************************************
 * @purpose Format and record a message in the in-memory log.
 *
 * @param  severity @b((input)} See RFC 3164 Section 4.1.1 Table 2
 * @param  component @b((input)} Level 7 component id
 * @param  fileName @b{(input)} file name
 * @param  lineNum @b{(input)} line number
 * @param  nfo @b{(input)} extra information - null terminated string
 *
 * @returns  None
 *
 * @notes  This executes on the calling task thread. This routine may
 *         be called from interrupt context. We make no assumption
 *         about the callers memory storage for any arguments.
 *         This routine makes a copy of all arguments and formats
 *         them into a locally allocated buffer.
 *
 * @end
 *********************************************************************/

void l7_log(L7_LOG_SEVERITY_t severity, L7_COMPONENT_IDS_t component,
            L7_char8 * fileName, L7_uint32 lineNum, L7_char8 * nfo)
{
  L7_clocktime  ct;
  L7_uint32 unit = 0;

  (void)unitMgrNumberGet(&unit);

  ct.seconds = simAdjustedTimeGet();

  l7utilsFilenameStrip(&fileName);

  logmsg(logDefaultFacilityGet(), severity, component, ct,
         unit, (L7_uint32)osapiTaskIdSelf(), fileName,
         lineNum, nfo);

  unitMgrLogMsg(fileName, lineNum, nfo, severity, component);
}

/**********************************************************************
 * @purpose Display trace message to console if logging is not intialized
 *
 * @param  component @b((input)} Level 7 component id
 * @param  fileName @b{(input)} file name
 * @param  lineNum @b{(input)} line number
 * @param  nfo @b{(input)} extra information - null terminated string
 *
 * @returns  None
 *
 * @notes  This executes on the calling task thread. This routine may
 *         be called from interrupt context. We make no assumption
 *         about the callers memory storage for any arguments.
 *         This routine makes a copy of all arguments and formats
 *         them into a locally allocated buffer.
 *
 *         This routine is invoked to display trace messages during system
 *         initialization or clear config.
 *
 * @end
 *********************************************************************/
void log_user_trace(L7_COMPONENT_IDS_t component, L7_char8 * fileName,
                    L7_uint32 lineNum, L7_char8 * nfo, ...)
{
  L7_clocktime  ct;
  L7_char8    mnemonic[L7_COMPONENT_MNEMONIC_MAX_LEN];
  L7_int32    len;
  L7_char8 logInitBuf[L7_LOG_MESSAGE_LENGTH];

  (void)osapiUTCTimeGet(&ct);

  if (logCnfgrStateCheck() != L7_TRUE)
  {
    /* Logging is not yet initialized */
    /* Display a simplified msg */
    memset(logInitBuf, 0x00, sizeof(L7_char8)*L7_LOG_MESSAGE_LENGTH);
    memset(mnemonic, 0, L7_COMPONENT_MNEMONIC_MAX_LEN);
    strcpy (mnemonic, "UNKN");
    if (component != L7_LOG_COMPONENT_DEFAULT)
      usmDbComponentMnemonicGet(component, mnemonic);

    l7utilsFilenameStrip(&fileName);
    len = osapiSnprintf(logInitBuf, L7_LOG_MESSAGE_LENGTH,
                        "%.15s %s[%d]: %s(%d) --  %s",
                        logDateString(ct.seconds),
                        mnemonic, (L7_uint32)osapiTaskIdSelf(),
                        fileName, lineNum, nfo);


    sysapiPrintf("\r\n");
    if (len < 0)
    {
      sysapiPrintf(pLogMsgFmtError);
    }
    else
    {
      sysapiPrintf(logInitBuf);
    }
    sysapiPrintf("\r\n");
  }

}

/**********************************************************************
 * @purpose Format and record a message in the in-memory log.
 *
 * @param  facility @b((input)} See RFC 3164 Section 4.1.1 Table 1
 * @param  severity @b((input)} See RFC 3164 Section 4.1.1 Table 2
 * @param  component @b((input)} Level 7 component id
 * @param  ttime @b{(input)} time of event
 * @param  host @b{(input)} generating host
 * @param  tid @b{(input)} task id
 * @param  fileName @b{(input)} file name
 * @param  lineNum @b{(input)} line number
 * @param  nfo @b{(input)} extra information - null terminated string
 *
 * @returns  None
 *
 * @notes  This executes on the calling task thread. This routine may
 *         be called from interrupt context. We make no assumption
 *         about the callers memory storage for any arguments.
 *         This routine makes a copy of all arguments and formats
 *         them into a locally allocated buffer.
 *
 * @end
 *********************************************************************/
void logmsg(L7_LOG_FACILITY_t facility, L7_LOG_SEVERITY_t severity,
            L7_COMPONENT_IDS_t component, L7_clocktime ttime,
            L7_uint32 stk, L7_uint32 tid, L7_char8 * fileName,
            L7_uint32 lineNum, L7_char8 * nfo)
{
  L7_BOOL logPers = PERSISTENT_LOG_ACTIVE(severity);
  L7_BOOL logMem = MEMORY_LOG_ACTIVE(severity, component);
  L7_BOOL logBackend = SYSLOG_LOG_ACTIVE() |
						  EMAILALERT_LOG_ACTIVE() |
                       CONSOLE_LOG_ACTIVE(severity, component) |
                       logDebugTraceDisplayIsActive(severity);
  L7_char8    mnemonic[14];
  L7_char8    addr[24];   /* Large enough to hold a null terminated ip address. */
  L7_char8   *buf = L7_NULLPTR;
  L7_uint32   len;
  L7_uint32  ip;
  L7_uint32  key;
  struct     logMsg_s msg;
  L7_BOOL    freeBuf = L7_FALSE;
  const L7_int32   bufsiz = L7_LOG_MESSAGE_LENGTH;

  printf("L7_LOGF: cid=%u stk=%u tid=%u file=%s line=%u msg=\"%s\"\r\n",component,stk,tid,fileName,lineNum,nfo);

  /* NOTE: This function uses the fileName parm as is (caller is responsible
   *       for stripping off path info, if desired).
   */

  /* Count that we have been called. */
  logReceiveTimestamp = ttime.seconds;
  logMessagesReceived++;
  if (logServerEnabled == L7_FALSE)
  {
    return;
  }

  /* maintain counters keeping tracke of ignored messages due to configuration */
  if (!logMem)
  {
    logMemMessagesIgnored++;
  }
  if (!logPers)
  {
    logPersMessagesIgnored++;
  }
  if ( !CONSOLE_LOG_ACTIVE(severity, component) )
  {
    logConsoleMessagesIgnored++;
  }

  /* First, determine if we need to log anything at all??? */
  if (logMem || logPers || logBackend)
  {
    /* We really need to queue all this junk so that we do not
     * impose a penalty on the caller. But if we do, then the
     * caller can overwrite his buffer and we are left with
     * garbage or worse. Just grab a buffer and write it now
     * from the calling thread. And we REALLY need to replace
     * the ssysapiPrintf call with a tuned formatter.
     */
    buf = (L7_char8 *)logBufferAlloc();
    if (buf)
    {
      freeBuf = L7_TRUE;
      /* Format the buffer - we hide some
       * things in the first 4 octets.
       */
      buf[0] = (component & 0xff00) >> 8;
      buf[1] = (component & 0xff);
      buf[2] = (severity & 0xff00) >> 8;
      buf[3] = (severity & 0xff);

      sysapiIPFirstLocalAddress(&ip);
      osapiInetNtoa(ip, addr);

      memset(mnemonic, 0, 14);
      strcpy (mnemonic, "General");
      if (component != L7_LOG_COMPONENT_DEFAULT)
        (void)cnfgrApiComponentMnemonicGet(component, mnemonic);

      if (fileName != L7_NULL)
      {
        len = osapiSnprintf(buf + LOG_STACK_HEADER_LEN, bufsiz,
                            "<%d> %.15s %s-%d %s[%d]: %s(%d) %d %%%% ",
                            facility * 8 + severity, logDateString(ttime.seconds),
                            addr, stk, mnemonic, tid, fileName, lineNum, logMessagesReceived);
      }
      else
      {
        len = osapiSnprintf(buf + LOG_STACK_HEADER_LEN, bufsiz,
                            "<%d> %.15s %s-%d %s[%d]: %d %%%% ",
                            facility * 8 + severity, logDateString(ttime.seconds),
                            addr, stk, mnemonic, tid, logMessagesReceived);
      }

      /* Play some games here to avoid overwriting the end of buffer.
       *
       * NOTE:  The len returned by osapiSnprintf is one of the following:
       *          - the number of chars printed to buf (not counting the EOS),
       *          - the number of chars that would have been printed
       *              (not counting the EOS) had buf been large enough
       *              (per ANSI C99),
       *          - negative value if some other error occurred
       *
       *        The following check subtracts 1 from bufsiz so that the
       *        string copy only occurs if buf has room for at least one
       *        character in addition to the EOS.
       */
      if ((len >= 0) && (len < (bufsiz - 1)))
      {
        osapiStrncpySafe(buf + LOG_STACK_HEADER_LEN + len, nfo, bufsiz - len);
        len += strlen(buf + LOG_STACK_HEADER_LEN + len);
      }

      /* We do this first in case the box is crashing. */
      if ((logPers) &&
          (L7_TRUE != osapiIntContext()))
      {
        logWriteMsgToFlash(buf + LOG_STACK_HEADER_LEN, len);
      }

      if (logMem)
      {
        /* Place the buffer in the memory log */
        key = osapiDisableInts();
        if (inMemoryLog[hhead])
          logBufferFree((void *)inMemoryLog[hhead]);
        inMemoryLog[hhead] = buf;
        hhead = ((hhead + 1) >= inMemoryLogSize)
                ? 0 : hhead + 1;
        osapiEnableInts(key);
        inMemoryLogCount++;
        freeBuf = L7_FALSE;
      }

      if (logBackend)
      {
        msg.msgtype = LOG_MSG_EVENT;
        msg.buffer = buf;
        msg.component = component;
        msg.severity = severity;
        /* Mark if the buffer should be freed after write. */
        msg.freeAfterWrite = !logMem;
        /* This length is the length of the message part,
         * not including the header.
         */
        msg.msgLen = len;
        /* Queue it to back end. */
        if (osapiMessageSend(logQueue, &msg, sizeof (msg),
                             L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
        {
          logMessagesResourceIssuesDropped++;     /* Counter:  cannot send log msg to queue */
        }
        else
        {
          freeBuf = L7_FALSE;
        }
      }
      else
      {
        logMessagesLoggingDisabledDropped++;           /* Counter:  relevant logging disabled */
      }

      if (freeBuf == L7_TRUE)
      {
        logBufferFree(buf);
      }
    }
    else
    {
      logMessagesResourceIssuesDropped++;             /* Counter:  Cannot allocate resource for logging */
    }
  }
  else
  {
    logMessagesLoggingDisabledDropped++;               /* Counter: all logging disabled */
  }
}


/**********************************************************************
 * @purpose record a message from another stack member to the
 *         in-memory log.
 *
* @param    src_key {(input)}  Key (mac-address) of the unit that sent the msg
* @param    msg     {(input)}  The buffer holding the message
* @param    msg_len {(input)}  The buffer length of the message
 *
 * @returns  None
 *
 * @notes  This executes on the calling task thread. This routine may
 *         be called from interrupt context. We make no assumption
 *         about the callers memory storage for any arguments.
 *         This routine makes a copy of all arguments and formats
 *         them into a locallay allocated buffer.
 *
 * @end
 *********************************************************************/

#if defined(L7_STACKING_PACKAGE)

void logMsgBuffer(L7_enetMacAddr_t src_key,
                  L7_uchar8 * msgbuf, L7_uint32 msglen)
{
  L7_COMPONENT_IDS_t  component;
  L7_LOG_SEVERITY_t   severity;
  L7_BOOL logMem;
  L7_BOOL logPers;
  L7_BOOL logBackend;
  L7_char8 * buf = 0;;

  /* Count that we have been called. */
  logMessagesReceived++;
  if ((msgbuf == (L7_uchar8 *)L7_NULL) || (msglen < LOG_STACK_HEADER_LEN + 1))
    return;

  component = (L7_COMPONENT_IDS_t)((msgbuf[0] << 8) | msgbuf[1]);
  severity = (L7_LOG_SEVERITY_t)((msgbuf[2] << 8) | msgbuf[3]);

  /* Handle configuration messages from top of stack. */
  if ((component == 0xffff) && (severity == 0xffff))
  {
    struct logCfgMsg_s * msg = (struct logCfgMsg_s *)msgbuf;

    logCfg->cfg.consoleAdminStatus = msg->consoleAdminStatus;
    logCfg->cfg.consoleComponentFilter = msg->consoleComponentFilter;
    logCfg->cfg.consoleSeverityFilter = msg->consoleSeverityFilter;
    logCfg->cfg.inMemoryAdminStatus = msg->inMemoryAdminStatus;
    logCfg->cfg.inMemoryBehavior = msg->inMemoryBehavior;
    logCfg->cfg.inMemoryComponentFilter = msg->inMemoryComponentFilter;
    logCfg->cfg.inMemorySeverityFilter = msg->inMemorySeverityFilter;
    logCfg->cfg.persistentAdminStatus = msg->persistentAdminStatus;
    logCfg->cfg.persistentSeverityFilter = msg->persistentSeverityFilter;

    return;
  }

  /* Take no garbage. Just return success for munged messages. */
  if ((component >= L7_LAST_COMPONENT_ID) ||
      (component <= L7_FIRST_COMPONENT_ID))
    return;
  if (severity >= L7_LOG_SEVERITY_DEBUG)
    return;
  if (msgbuf[LOG_STACK_HEADER_LEN] != '<')
    return;

  /* Ok - it's probably good... */
  logMem = MEMORY_LOG_ACTIVE(severity, component);
  logPers = PERSISTENT_LOG_ACTIVE(severity);
  logBackend = SYSLOG_LOG_ACTIVE() |
				 EMAILALERT_LOG_ACTIVE() |
               CONSOLE_LOG_ACTIVE(severity, component) |
               logDebugTraceDisplayIsActive(severity);

  /* maintain counters keeping tracke of ignored messages due to configuration */
  if (!logMem)
  {
    logMemMessagesIgnored++;
  }
  if (!logPers)
  {
    logPersMessagesIgnored++;
  }
  if ( !CONSOLE_LOG_ACTIVE(severity, component) )
  {
    logConsoleMessagesIgnored++;
  }

  if (logMem || logPers || logBackend)
  {
    /* We do this first in case we are crashing. */
    if (logPers)
    {
      logWriteMsgToFlash((L7_char8 *)msgbuf + LOG_STACK_HEADER_LEN, (L7_uint32)msglen - LOG_STACK_HEADER_LEN);

    }
    /* Now start the in-memory and back end processing */
    buf = (L7_char8 *)logBufferAlloc();
    if (buf)
    {
      memcpy(buf, msgbuf, msglen);
      buf[msglen] = '\0';

      if (logMem)
      {
        L7_uint32 key;
        /* Place the buffer in the memory log */
        key = osapiDisableInts();
        if (inMemoryLog[hhead])
          logBufferFree(inMemoryLog[hhead]);
        inMemoryLog[hhead] = buf;
        hhead = ((hhead + 1) >= inMemoryLogSize)
                ? 0 : hhead + 1;
        osapiEnableInts(key);
        inMemoryLogCount++;
      }

      if (logBackend)
      {
        struct logMsg_s msg;

        msg.msgtype = LOG_MSG_EVENT;
        msg.buffer = buf;
        msg.component = component;
        msg.severity = severity;
        msg.freeAfterWrite = !logMem;
        msg.msgLen = (L7_ushort16)msglen;
        /* Queue it to back end. */
        if (osapiMessageSend(logQueue, &msg, sizeof (msg),
                             L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
        {
          if (msg.freeAfterWrite)
            logBufferFree(buf);

          logMessagesResourceIssuesDropped++;             /* Counter:  Cannot queue msg for logging */
        }
      }
    }
    else
    {
      logMessagesResourceIssuesDropped++;                 /* Counter: Cannot allocate resource for logging */
    }
  }
  else
  {
    logMessagesLoggingDisabledDropped++;                   /* Counter:  No logging enabled */
  }
}

#endif

/*************** Socket Handling *****************/

/**********************************************************************
 * @purpose  This routine closes the local socket.
 *
 * @returns  Status - L7_SUCCESS if socket was successfully opened.
 *
 * @notes    If the socket is already closed, this routine does
 *           not close it again. This routine uses the fd variable
 *           to flags socket open/closed.
 *           Closing the socket clears the resolved remote addresses
 *           stored in logOps[xxx].remoteAddr.
 *
 * @end
 *********************************************************************/

L7_RC_t logLocalSocketClose()
{
  L7_uint32 ndx;
  if (fd >= 0)
  {
    osapiShutdown(fd, L7_SHUT_RDWR);
    osapiSocketClose(fd);
    fd = -1;
  }
  /* Force resolution of every host again. */
  for (ndx = 0; ndx < L7_LOG_MAX_HOSTS; ndx++)
    logOps[ndx].remoteAddr = L7_NULL_IP_ADDR;
  return L7_SUCCESS;
}

/**********************************************************************
 * @purpose  This routine opens (or reopens) the local socket and binds to it.
 *
 * @returns  Status - L7_ERROR if socket could not be opened.
 *                   L7_SUCCESS if socket was successfully opened.
 *
 * @notes    If the socket is already open, this routine will close
 *           it first. This routine modifies the fd variable.
 *
 *
 * @end
 *********************************************************************/

static L7_RC_t logLocalSocketOpen()
{
  L7_sockaddr_in_t baddr;
  L7_uint32 zero = 0;

  logLocalSocketClose();
  /** Open a socket and begin querying the server. */
  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &fd) != L7_SUCCESS)
  {
    sysapiPrintf("socket open failed - errno %d\n", osapiErrnoGet());
    return L7_ERROR;
  }
  /** Bind to local port */
  baddr.sin_family = L7_AF_INET;
  baddr.sin_port = osapiHtons(logCfg->cfg.localPort);
  baddr.sin_addr.s_addr = L7_INADDR_ANY;

  if (osapiSocketBind(fd, (L7_sockaddr_t *)&baddr, sizeof(baddr)) != L7_SUCCESS)
  {
    sysapiPrintf("Can't bind socket to local address %lx port %d - errno %d\n",
                 localAddr, logCfg->cfg.localPort, osapiErrnoGet());
    return L7_ERROR;
  }
  /* We never read from this socket, so set maximum RX size to zero.
   *   ** A DOS may fill up the socket with garbage and cause the IP stack
   *     ** to run out of memory.
   *       */
  if (osapiSetsockopt(fd, L7_SOL_SOCKET, L7_SO_RCVBUF,(L7_char8 *) &zero, sizeof(zero))== L7_FAILURE)
  {
    sysapiPrintf("syslog - Can't set SO_RCVBUF to 0 - errno %d\n", osapiErrnoGet());
    return L7_ERROR;
  }
  return L7_SUCCESS;
}

/**********************************************************************
 * @purpose  Resolve an address into it's internal representation.
 *
 * @param    atype @b{(input) address type (unknown, IPv4, DNS)
 * @param    address @b{(input)} address string to resolve
 *
 * @returns  IP address (int)
 *
 * @notes    Returns 0 if unable to resolve address.
 *           Potentially invokes DNS to resolve address
 *
 * @end
 *********************************************************************/

static int logResolveAddress(L7_IP_ADDRESS_TYPE_t atype, L7_char8 * address)
{
  L7_uint32 ipAddr = L7_NULL;
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_char8  hostFQDN[L7_DNS_DOMAIN_NAME_SIZE_MAX];

  if (atype == L7_IP_ADDRESS_TYPE_DNS)
  {
    /* Resloving Ip address with DNS Lookup */
    if (usmDbDNSClientNameLookup(address, &status,
                                 hostFQDN, &ipAddr) == L7_SUCCESS)
    {
      return ipAddr;
    }
    else
    {
      return 0;
    }
  }
  else if (atype == L7_IP_ADDRESS_TYPE_IPV4)
  {
    return osapiInet_addr(address);
  }
  else
  {
    return 0;
  }
}

/**********************************************************************
 * @purpose  Send a log packet to a syslog host
 *           Simply returns without sending anything if hostIndex
 *           index is invalid or host address is not resolvable.
 *           Fails if socket specified by fd is not open.
 *
 * @param    hostIndex @b{(input) index into host table
 * @param    buf @b{(input) buffer to log
 * @param    len @b{(input) length of buffer
 * @returns  none
 *
 * @notes    May resolve host address and update logOps[XXX].remoteAddr
 *           Updates logSyslogMessagesRelayed and logSyslogMessageDeliveredTimestamp.
 *
 * @end
 *********************************************************************/

static L7_RC_t logWriteMsgToSyslog(L7_uint32 hostIndex,
                                   L7_char8 * buf, L7_int32 len)
{
  L7_uint32 bytesSent;
  L7_sockaddr_in_t saddr;

  /* Resolve address on first send */
  if (logOps[hostIndex].remoteAddr <= L7_NULL_IP_ADDR)
  {
    logOps[hostIndex].remoteAddr =
    logResolveAddress(logCfg->cfg.host[hostIndex].addressType,
                      logCfg->cfg.host[hostIndex].address);
  }
  if (logOps[hostIndex].remoteAddr <= L7_NULL_IP_ADDR)
  {
    return L7_ERROR;
  }

  saddr.sin_family = L7_AF_INET;
  saddr.sin_port = osapiHtons(logCfg->cfg.host[hostIndex].port);
  saddr.sin_addr.s_addr = osapiHtonl(logOps[hostIndex].remoteAddr);
  if (osapiSocketSendto(fd, buf, len, 0, (L7_sockaddr_t *)&saddr,sizeof(saddr),
                        &bytesSent) != L7_SUCCESS)
  {
    /* We just shut up and take it. */
    /* keep track of number of dropped messages per host */
    /* since syslog messages are only logged to those hosts , where severity level matches*/
    /* the messages dropped are only due to resouce issues */
    logOps[hostIndex].syslogMessageIgnoredCount++;
    return L7_ERROR;
  }
  else
  {
    logOps[hostIndex].syslogMessageForwardCount++;
    logSyslogMessagesRelayed++;
    logSyslogMessageDeliveredTimestamp = osapiUTCTimeNow();
    return L7_SUCCESS;
  }
}

/**
 * SECTION: Dispatch
 *
 * This section contains the main dispatch loop for the LOG server.
 *
 *
 *
 */

/**********************************************************************
 * @purpose  logTask
 *
 * @param    none
 *
 *
 * @returns  Nothing
 *
 * @notes    This is the main LOG task.
 *           The basic idea of this loop is that it will figure out what to
 *           to do based on:
 *             logCfg->cfg.syslogAdminStatus
 *             logCfg->cfg.persistenAdminStatus
 *             syslog host table
 *           Updates logMessagesDropped.
 *
 * @end
 *********************************************************************/

static void logTask()
{
  struct logMsg_s logMsg;
  register L7_uint32 hostNdx;
  L7_BOOL logConsole;

  if (logQueue == L7_NULL)
  {
    printf("logTask: queue not initialized\n");
    return;
  }

  /*  Start processing queued messages. */
  do
  {
    if (osapiMessageReceive(logQueue, &logMsg,
                            sizeof (struct logMsg_s), L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      /* Hmmm - what to do? */
      sysapiPrintf("logTask: osapiMsgReceive failed - errno (0x%x)\n",
                   osapiErrnoGet());
      osapiSleep(10);
      continue;
    }

    if(logMsg.msgtype == LOG_TIMER_EVENT)
    {

      if(logCfg->cfg.emailAlertInfo.emailAlertingStatus == L7_ADMIN_MODE_ENABLE)
      {
         if(logWriteMsgToSMTPServer(EMAIL_ALERT_TRIGGER_TIMER,L7_NULL) != L7_SUCCESS)
         {
            logTaskLock();
            emailAlertOperData.emailAlertingOperStatus = L7_DISABLE;
 		     emailAlertServerPollTimerStart();						
           logTaskUnlock();
         }
		  else if(emailAlertOperData.emailAlertingOperStatus == L7_DISABLE)
		  {
				emailAlertOperData.emailAlertingOperStatus = L7_ENABLE;
				emailAlertServerPollTimerStop();
		  }
  	     emailAlertTimerStart();
				 
      }

         continue;
    }
    /* Skip bad buffers. */
    if (logMsg.buffer[LOG_STACK_HEADER_LEN] != '<')
      continue;
    logTaskLock();
#if defined(L7_STACKING_PACKAGE)
    /** Log to all matching syslog hosts (only from management unit). */
    if ((logCfg->cfg.syslogAdminStatus == L7_ADMIN_MODE_ENABLE) &&
        (sysapiHpcTopOfStackGet() == SYSAPI_STACK_ROLE_MANAGEMENT_UNIT))
#else
    if (logCfg->cfg.syslogAdminStatus == L7_ADMIN_MODE_ENABLE)
#endif
    {
      /** No socket? Go get one */
      if (fd < 0)
      {
        logLocalSocketOpen();
        /** Still no socket, start again at top */
        if (fd < 0)
        {
          logTaskUnlock();
          continue;
        }
      }
      /* Loop through the hosts and see if we get a match.
       * We write to all matching hosts.
       */
      for (hostNdx = 0; hostNdx < L7_LOG_MAX_HOSTS; hostNdx++)
      {
        if (SYSLOG_FILTER_MATCH(hostNdx, logMsg.severity,
                                logMsg.component))
          logWriteMsgToSyslog(hostNdx,
                              logMsg.buffer + LOG_STACK_HEADER_LEN, logMsg.msgLen);
      }
    }

    logTaskUnlock();


      if((logCfg->cfg.emailAlertInfo.emailAlertingStatus == L7_ADMIN_MODE_ENABLE)&&
					(emailAlertOperData.emailAlertingOperStatus == L7_ENABLE))
      {

		 if(logMsg.component == L7_TRAPMGR_COMPONENT_ID)
		 {
			logMsg.severity = logCfg->cfg.emailAlertInfo.trapseverityLevel;
		 }

         if ((logMsg.severity <=  logCfg->cfg.emailAlertInfo.nonUrgentSeverityLevel) &&
				 	(logMsg.severity >  logCfg->cfg.emailAlertInfo.urgentSeverityLevel))
         {
    
			L7_char8 * logbuf =L7_NULL;

			logbuf = L7_NULL;
			logbuf =logBufferAlloc();
			if(logbuf != L7_NULL)
			{
	          memcpy(&emailAlertBuffer[emailAlertMsgCount], &logMsg, sizeof(logmsg));
				emailAlertBuffer[emailAlertMsgCount].buffer = logbuf;
				memcpy(emailAlertBuffer[emailAlertMsgCount].buffer,(logMsg.buffer+ LOG_STACK_HEADER_LEN),logMsg.msgLen);	
				emailAlertBuffer[emailAlertMsgCount].buffer[logMsg.msgLen] = '\0';
	          if(emailAlertMsgCount == MAX_NUMBER_OF_LOG_MESSAGES)
	          {
	                 if(logWriteMsgToSMTPServer(EMAIL_ALERT_TRIGGER_BUFFER_FULL,L7_NULL) != L7_SUCCESS)
	                 {
	                    emailAlertOperData.emailAlertingOperStatus = L7_DISABLE;
						   emailAlertServerPollTimerStart();										
	                 }
	          }
	          else
	          {
	                emailAlertMsgCount++;
	          }
			}
			else
			{
               if(logWriteMsgToSMTPServer(EMAIL_ALERT_TRIGGER_BUFFER_FULL,L7_NULL) != L7_SUCCESS)
               {
                  emailAlertOperData.emailAlertingOperStatus = L7_DISABLE;
				   emailAlertServerPollTimerStart();										
               }
		}
        }
        else if(logMsg.severity <=  logCfg->cfg.emailAlertInfo.urgentSeverityLevel)
        {
                 if(logWriteMsgToSMTPServer(EMAIL_ALERT_TRIGGER_EMERGENCY_MESSAGE,logMsg.buffer+ LOG_STACK_HEADER_LEN) != L7_SUCCESS)
                 {
                    emailAlertOperData.emailAlertingOperStatus = L7_DISABLE;
					   emailAlertServerPollTimerStart();										
                 }
        }

      }

    printf("LOGF: \"%s\"\r\n",(L7_char8 *) (logMsg.buffer + LOG_STACK_HEADER_LEN));

    /* Log to console? And yes, we do allow mgmt to
     * change things while writing to slow devices.
     */
    logConsole = L7_NULL;
    logConsole = CONSOLE_LOG_ACTIVE(logMsg.severity, logMsg.component) | logDebugTraceDisplayIsActive(logMsg.severity);
    if (logConsole)
    {
      L7_char8 * b = logMsg.buffer + LOG_STACK_HEADER_LEN;

      cliWebConsoleLog("\r\n");
      cliWebConsoleLog(b);
      cliWebConsoleLog("\r\n");
    }
    /* No memory leaks please */
    if (logMsg.freeAfterWrite != L7_FALSE)
      logBufferFree(logMsg.buffer);
  } while (1);
}

/*********************************************************************
* @purpose  This inits the email Alert default configuration
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void emailAlertInit(void)
{
    L7_uint32 loopCount;
	
	 emailAlertMsgCount=0;
    logCfg->cfg.emailAlertInfo.emailAlertingStatus = FD_EMAIL_ALERT_DEFAULT_STATUS;
    logCfg->cfg.emailAlertInfo.logDuration = FD_EMAIL_ALERT_DEFAULT_LOG_DURATION;
    logCfg->cfg.emailAlertInfo.nonUrgentSeverityLevel = FD_EMAIL_ALERT_DEFAULT_NON_URGENT_SEVERITY;
    logCfg->cfg.emailAlertInfo.urgentSeverityLevel = FD_EMAIL_ALERT_DEFAULT_URGENT_SEVERITY;
    logCfg->cfg.emailAlertInfo.trapseverityLevel= FD_EMAIL_ALERT_DEFAULT_TRAP_SEVERITY;		
	osapiStrncpy(logCfg->cfg.emailAlertInfo.fromAddress,FD_EMAIL_ALERT_DEFAULT_FROM_ADDRESS,sizeof(FD_EMAIL_ALERT_DEFAULT_FROM_ADDRESS));		
	osapiStrncpy(logCfg->cfg.emailAlertInfo.severityTable[L7_LOG_EMAIL_ALERT_URGENT-1].subject,"Urgent Log Messages",sizeof("Urgent Log Messages"));		
	osapiStrncpy(logCfg->cfg.emailAlertInfo.severityTable[L7_LOG_EMAIL_ALERT_NON_URGENT-1].subject,"Non Urgent Log Messages",sizeof("Non Urgent Log Messages"));			
	for (loopCount=0; loopCount < L7_MAX_NO_OF_SMTP_SERVERS; loopCount++)
	{

#ifdef  L7_MGMT_SECURITY_PACKAGE		
		logCfg->cfg.emailAlertInfo.smtpServers[loopCount].securityMode= FD_EMAIL_ALERT_SMTP_DEFAULT_SECURITY_MODE;
		logCfg->cfg.emailAlertInfo.smtpServers[loopCount].smtpPort = FD_EMAIL_ALERT_SMTP_DEFAULT_PORT;
		emailAlertOperData.smtp_con =L7_NULL;
#else
		logCfg->cfg.emailAlertInfo.smtpServers[loopCount].securityMode= L7_LOG_EMAIL_ALERT_NONE;
		logCfg->cfg.emailAlertInfo.smtpServers[loopCount].smtpPort = L7_EMAIL_ALERT_SMTP_NORMAL_PORT;
#endif
		logCfg->cfg.emailAlertInfo.smtpServers[loopCount].entryStatus= FD_EMAIL_ALERT_SMTP_DEFAULT_ENTRY_STATUS; 				
       memset(logCfg->cfg.emailAlertInfo.smtpServers[loopCount].smtpServerAddress,0,L7_EMAIL_ALERT_SMTP_MAX_ADDRESS_LEN);
		osapiStrncpy(logCfg->cfg.emailAlertInfo.smtpServers[loopCount].userid,FD_EMAIL_ALERT_SMTP_DEFAULT_USERID,sizeof(FD_EMAIL_ALERT_SMTP_DEFAULT_USERID));
		osapiStrncpy(logCfg->cfg.emailAlertInfo.smtpServers[loopCount].passwd,FD_EMAIL_ALERT_SMTP_DEFAULT_PASSWD ,sizeof(FD_EMAIL_ALERT_SMTP_DEFAULT_PASSWD ));
	}

	emailAlertTraceFlags=0;	
   emailAlertOperData.emailAlertingOperStatus = L7_ENABLE;
   emailAlertOperData.sd=0;

}


/*********************************************************************
* @purpose  Start the Email Alert periodic Timer. On expiration, send
*           an email with all the pending log messages to the SMTP server.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void emailAlertTimerCallback(void)
{
  struct logMsg_s msg;
  L7_RC_t rc;

  if (logQueue == L7_NULLPTR)
    return;

	if(emailAlertOperData.emailAlertingOperStatus == L7_DISABLE)
	{
		emailAlertOperData.emailAlertingOperStatus = L7_ENABLE;
		emailAlertServerPollTimerStop();
	}
  /* process event on our thread */
  memset((void *)&msg, 0, sizeof(struct logMsg_s) );
  msg.msgtype = LOG_TIMER_EVENT;

  rc = osapiMessageSend(logQueue, &msg, sizeof(struct logMsg_s),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    /* Log the  message */
  }
}

/*********************************************************************
* @purpose  Start the Email Alert periodic Timer. On expiration, send
*           an email with all the pending log messages to the SMTP server.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void emailAlertServerPollTimerCallback(void)
{
	if(emailAlertOperData.emailAlertingOperStatus == L7_DISABLE)
	{
		emailAlertOperData.emailAlertingOperStatus = L7_ENABLE;
		emailAlertServerPollTimerStop();
	}
}


/*********************************************************************
* @purpose  Start the Email alert periodic timer
*
* @param    none
*
* @returns  L7_SUCCESS
*           L7_FAILURE if timer already running
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t emailAlertTimerStart(void)
{
    /* reschedule the timer */
  osapiTimerAdd((void *)emailAlertTimerCallback, L7_NULL, L7_NULL,
                logCfg->cfg.emailAlertInfo.logDuration, &emailAlertTimer);

  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose  Stop the Email Alert Periodic timer.
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t emailAlertTimerStop(void)
{
  osapiTimerFree(emailAlertTimer);
  emailAlertTimer = L7_NULLPTR;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Start the Email alert server poll timer
*
* @param    none
*
* @returns  L7_SUCCESS
*           L7_FAILURE if timer already running
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t emailAlertServerPollTimerStart(void)
{
    /* reschedule the timer */
  osapiTimerAdd((void *)emailAlertServerPollTimerCallback, L7_NULL, L7_NULL,
                L7_EMAIL_ALERT_SERVER_POLL_TIME*60*1000, &emailAlertServerPollTimer);

  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose  Stop the Email Alert Periodic timer.
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t emailAlertServerPollTimerStop(void)
{
  osapiTimerFree(emailAlertServerPollTimer);
  emailAlertServerPollTimer = L7_NULLPTR;

  return L7_SUCCESS;
}



/**********************************************************************
 *
 * @purpose Server initialization for LOG component
 *
 * @param cfg @b{(input)} pointer to config data.
 *
 * @returns L7_SUCCESS, if success
 * @returns L7_FAILURE, if other failure
 *
 * @notes LOG is not really a component. It is a system level utility.
 *         printf is used here because sysapiPrintf may not be quite ready
 *         when this routine is called.
 *
 *         logShuffleFiles is presumed to have already been called when
 *         entering this routine. IPL code should call logShuffleFIles
 *         and then logIPLPersistentInit in order to log to the persistent
 *         log. This initialization will preserve those messages.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t logServerInit(struct logCfg_s * cfg)
{
  extern void logApiInit(struct logCfg_s *, struct logOps_s *);
  extern void logDebugInit(struct logCfg_s *, struct logOps_s *);

  if (logServerEnabled == L7_TRUE)
    return L7_SUCCESS;

  logPersistentInit();

  if (cfg == L7_NULL)
  {
    logWritePersistentLogsToConsole();
    printf("logServerInit: %s(%d) - unable to obtain configuration data!\n",
           __FILE__, __LINE__);
    return L7_FAILURE;
  }
  logCfg = cfg;

  /* Allocate the syslog collector/relay statistics data */
  logOps = osapiMalloc(L7_LOG_COMPONENT_ID, L7_LOG_MAX_HOSTS * sizeof (struct logOps_s));
  if (logOps == L7_NULL)
  {
    logWritePersistentLogsToConsole();
    printf("logServerInit: %s(%d): unable to allocate operational data!\n",
           __FILE__, __LINE__);
    return L7_FAILURE;
  }

  logApiInit(logCfg, logOps);
  logDebugInit(logCfg, logOps);
  logCfgInit(logCfg, logOps);

  logMessagesReceived = logServerPersistentLogMessageCount();

  /** semaphore creation for task protection over the common data. */
  logTaskSyncSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (logTaskSyncSemaphore == L7_NULL)
  {
    logWritePersistentLogsToConsole();
    printf("logServerInit %s(%d): unable to create semaphore!\n",
           __FILE__, __LINE__);
    return L7_FAILURE;
  }

  /*
   * Allocate our in-memory buffer space.
   */
  if (logBufferPoolAllocate( L7_LOG_IN_MEMORY_LOG_DEFAULT_COUNT ) != L7_SUCCESS)
  {
    logWritePersistentLogsToConsole();
    printf("logServerInit %s(%d): unable to allocate buffer pool!\n",
           __FILE__, __LINE__);
    return L7_FAILURE;
  }

  /* Create the logQueue */
  /* create LOG processing message queue */
  logQueue = osapiMsgQueueCreate(LOG_QUEUE, LOG_EXTRA_BUFFERS,
                                 sizeof(struct logMsg_s));
  if (logQueue == L7_NULL)
  {
    logBufferPoolFree();
    logWritePersistentLogsToConsole();
    printf("logServerInit %s(%d): unable to create message queue!\n",
           __FILE__, __LINE__);
    return L7_FAILURE;
  }

  /** create LOG client task */
  logServerTaskId = (L7_uint32)osapiTaskCreate("LOG",
                                               (void *)logTask,
                                               L7_NULL,
                                               L7_NULL,
                                               L7_DEFAULT_STACK_SIZE,
                                               L7_DEFAULT_TASK_PRIORITY,
                                               L7_DEFAULT_TASK_SLICE);

  if (logServerTaskId == L7_NULL)
  {
    logBufferPoolFree();
    logWritePersistentLogsToConsole();
    printf("logServerInit %s(%d): unable to create log task!\n",
           __FILE__, __LINE__);
    return L7_FAILURE;
  }

  /* Now we can accept log messages. */
  logServerEnabled = L7_TRUE;

  return L7_SUCCESS;
}


/**********************************************************************
 * @purpose  write in memory logs to file
 *
 * @param    file name @b{(input)} name of file to write
 *
 * @returns  L7_ERROR if null pointer argument or failed write
 * @returns  L7_SUCCESS if file written
 *
 * @notes  Modifies file system
 *
 * @end
 *********************************************************************/

L7_RC_t logWriteInMemoryLogToFile(L7_char8 * fileName)
{
  L7_uint32 size = 0;
  L7_uint32 ndx;
  L7_char8 buf[1024];
  L7_uint32 pad;
  L7_int32 fd;
  L7_int32  rc = L7_SUCCESS;
  L7_uint32 msgNdx;

  if (inMemoryLog == L7_NULL)
    return L7_ERROR;

  /* We scale the logMessagesReceived to be hhead - 1, i.e. the last message */
  if (hhead != 0)
    msgNdx = hhead;
  else
    msgNdx = inMemoryLogSize;

  if (osapiFsOpen(fileName, &fd) == L7_ERROR)
  {
    if (osapiFsCreateFile(fileName) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_LOG_COMPONENT_ID, 
              "logWriteInMemoryLogToFile: osapiFsCreateFile failed");
      return L7_ERROR;
    }
    if (osapiFsOpen(fileName, &fd) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_LOG_COMPONENT_ID,
              "logWriteInMemoryLogToFile: osapiFsOpen failed");
      return L7_ERROR;
    }
  }

  if (inMemoryLogCount == 0)
  {
    osapiFsClose(fd);
    return L7_SUCCESS;
  }
  for (ndx = msgNdx; ndx < inMemoryLogSize; ndx++)
  {
    if (inMemoryLog[ndx])
    {
      L7_uint32 len = strlen(inMemoryLog[ndx]+ LOG_STACK_HEADER_LEN);
      rc = osapiFsWriteNoClose(fd, inMemoryLog[ndx] + LOG_STACK_HEADER_LEN, len);
      if (rc != L7_SUCCESS)
        break;
      size += len + 2;
      rc = osapiFsWriteNoClose(fd, "\r\n", 2);
      if (rc != L7_SUCCESS)
        break;
    }
  }


  for (ndx = 0; ndx < msgNdx; ndx++)
  {
    if (inMemoryLog[ndx])
    {
      L7_uint32 len = strlen(inMemoryLog[ndx]+ LOG_STACK_HEADER_LEN);
      rc = osapiFsWriteNoClose(fd, inMemoryLog[ndx]+ LOG_STACK_HEADER_LEN, len);
      if (rc != L7_SUCCESS)
        break;
      size += len + 2;
      rc = osapiFsWriteNoClose(fd, "\r\n", 2);
      if (rc != L7_SUCCESS)
        break;
    }
  }

  if (rc != L7_SUCCESS)
  {
    if (osapiFsDeleteFile (fileName) != L7_SUCCESS )
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_ID,
              "logWriteInMemoryLogToFile: osapiFsDeleteFile failed");
    }
    return L7_ERROR;
  }

  pad = 1024 - (size % 1024);
  if (size % 1024 )
  {
    /* pad to make multiple of 1024 bytes for 1K xmodem */
    memset(buf, ' ', pad + 1);
    rc = osapiFsWriteNoClose(fd, buf, pad);
  }

  if (rc != L7_SUCCESS)
  {
    if (osapiFsDeleteFile (fileName) != L7_SUCCESS )
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_ID,
              "logWriteInMemoryLogToFile: osapiFsDeleteFile failed");
    }
    return L7_ERROR;
  }
  osapiFsClose(fd);
  return L7_SUCCESS;
}



/**********************************************************************
 * @purpose  get count of in memory log messages
 *
 * @param    count @b{(input)} address of memory to be written
 *
 * @returns  L7_ERROR if null pointer argument
 * @returns  L7_SUCCESS if caller memory written.
 *
 * @notes  Modifies memory pointed to by pointer argument.
 *
 * @end
 *********************************************************************/

L7_RC_t logServerLogInMemoryCountGet(L7_uint32 * count)
{
  if (count == L7_NULL)
    return L7_ERROR;
  *count = inMemoryLogCount;
  return L7_SUCCESS;
}
/**********************************************************************
 * @purpose clear in memory log
 *
 *
 * @returns  L7_SUCCESS
 *
 * @notes  We don't free "all" buffers because they may be queued
 *
 * @end
 *********************************************************************/

L7_RC_t logServerLogInMemoryClear()
{
  logBufferPoolReset();
  return L7_SUCCESS;
}

/**********************************************************************
 * @purpose  get count of received messages
 *
 * @param    count @b{(output)} address of memory to be written
 *
 * @returns  L7_ERROR if null pointer argument
 * @returns  L7_SUCCESS if caller memory written.
 *
 * @notes  Modifies memory pointed to by pointer argument.
 *
 * @end
 *********************************************************************/

L7_RC_t logServerLogMessagesReceivedGet(L7_uint32 * count)
{
  if (count == L7_NULL)
    return L7_ERROR;
  *count = logMessagesReceived;
  return L7_SUCCESS;
}
/**********************************************************************
 * @purpose  get count of dropped messages
 *
 * @param    count @b{(output)} address of memory to be written
 *
 * @returns  L7_ERROR if null pointer argument
 * @returns  L7_SUCCESS if caller memory written.
 *
 * @notes  Modifies memory pointed to by pointer argument.
 * @notes  Returns count of messages not logged due to error conditions
 *
 * @end
 *********************************************************************/

L7_RC_t logServerLogMessagesDroppedGet(L7_uint32 * count)
{
  if (count == L7_NULL)
    return L7_ERROR;
  *count = logMessagesResourceIssuesDropped;
  return L7_SUCCESS;
}

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

L7_RC_t logServerLogInMemoryMessagesDroppedGet( L7_uint32 *count )
{
  if ( L7_NULL == count )
  {
    return( L7_ERROR );
  }
  *count = logMemMessagesIgnored;
  return( L7_SUCCESS );
}

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

L7_RC_t logServerLogPersistentMessagesDroppedGet( L7_uint32 *count )
{
  if ( L7_NULL == count )
  {
    return( L7_ERROR );
  }
  *count = logPersMessagesIgnored;
  return( L7_SUCCESS );
}

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
L7_RC_t logServerLogConsoleMessagesDroppedGet( L7_uint32 *count )
{
  if ( L7_NULL == count )
  {
    return( L7_ERROR );
  }
  *count = logConsoleMessagesIgnored;
  return( L7_SUCCESS );
}


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
L7_RC_t logServerLogSyslogMessagesDroppedGet(L7_uint32 ndx,L7_uint32 *count )
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }

  /* take log task sync sema ? */
  *count = logOps[ndx-1].syslogMessageIgnoredCount;
  return( L7_SUCCESS );
}


/**********************************************************************
 * @purpose  get count of syslog messages relayed
 *
 * @param    count @b{(output)} address of memory to be written
 *
 * @returns  L7_ERROR if null pointer argument
 * @returns  L7_SUCCESS if caller memory written.
 *
 * @notes  Modifies memory pointed to by pointer argument.
 *
 * @end
 *********************************************************************/

L7_RC_t logServerLogSyslogMessagesRelayedGet(L7_uint32 * count)
{
  if (count == L7_NULL)
    return L7_ERROR;
  *count = logSyslogMessagesRelayed;
  return L7_SUCCESS;
}

/**********************************************************************
 * @purpose  get last message received timestamp
 *
 * @param    tstamp @b{(output)} address of memory to be written
 *
 * @returns  L7_ERROR if null pointer argument
 * @returns  L7_SUCCESS if caller memory written.
 *
 * @notes  Modifies memory pointed to by pointer argument.
 *
 * @end
 *********************************************************************/

L7_RC_t logServerLogMessageReceivedTimeGet(L7_uint32 * tstamp)
{
  if (tstamp == L7_NULL)
    return L7_ERROR;
  *tstamp = logReceiveTimestamp;
  return L7_SUCCESS;
}

/**********************************************************************
 * @purpose  get last syslog message delivered timestamp
 *
 * @param    tstamp @b{(output)} address of memory to be written
 *
 * @returns  L7_ERROR if null pointer argument
 * @returns  L7_SUCCESS if caller memory written.
 *
 * @notes  Modifies memory pointed to by pointer argument.
 *
 * @end
 *********************************************************************/

L7_RC_t logServerLogSyslogMessageDeliveredTimeGet(L7_uint32 * tstamp)
{
  if (tstamp == L7_NULL)
    return L7_ERROR;
  *tstamp = logSyslogMessageDeliveredTimestamp;
  return L7_SUCCESS;
}

/**********************************************************************
 * @purpose  Get the "next" row in the in-memory log table.
 *
 * @param    ndx @b{(input)} the putative index of a log table entry.
 * @param    buf @b{(output)} a buffer of at least L7_LOG_MESSAGE_LENGTH
 *                   bytes into which the message is written.
 * @param    bufNdx @b{(output)} the true index in the log table of the
 *                   returned buffer.
 *
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
 * @returns  L7_NOT_EXIST if ndx parameter is equal to or grater than
 *           the index of the last entry.
 *
 * @notes    The indices are continuously updated as messages are logged.
 *           Therefore, a slow reader and a fast writer (lot's of logging)
 *           may continuously retrieve the oldest message (which will not be
 *           the same).
 *           Use an ndx parameter of 0 to retrieve the oldest message
 *           in the log.
 *
 * @end
 *********************************************************************/

L7_RC_t logServerLogMsgInMemoryGetNext(L7_uint32 ndx, L7_char8 * buf,
                                       L7_uint32 * bufNdx)
{
  L7_uint32 msgNdx;
  /* Cache a copy of inMemoryLogCount so we aren't confused by the
     logging of new messages. */
  L7_uint32 mc = inMemoryLogCount;

  if ((buf == L7_NULL) || (bufNdx == L7_NULL))
    return L7_ERROR;

/* Can't do a get next on the last message or past the last message. */
  if (ndx >= mc)
    return L7_NOT_EXIST;
/* Maximum no. of buffered at a time
  if (ndx >=  inMemoryLogSize)
    return L7_NOT_EXIST; */

  /* Adjust indices less than our lowest index to the lowest index. */
  /* Adjust indices within our index range to the next index. */
  if ((ndx + inMemoryLogSize) <= mc)
  {
    /* Set to first message. */
    ndx = (mc - inMemoryLogSize) + 1;
  }
  else
  {
    /* Within range - increment. */
    ndx++;
  }

  /* We scale the logMessagesReceived to be hhead-1, i.e. the last message */
  /*if (inMemoryLogCount <= inMemoryLogSize)
    msgNdx = ndx ;
  else
    msgNdx = hhead + ndx;

  ndx++;  */
  if (mc <= inMemoryLogSize)
    msgNdx = ndx - 1;
  else
    msgNdx = (hhead + (inMemoryLogSize - 1)) - (mc - ndx);

  if (msgNdx >= inMemoryLogSize)
    msgNdx -= inMemoryLogSize;
  if (inMemoryLog[msgNdx])
  {
    osapiStrncpySafe(buf, inMemoryLog[msgNdx] + LOG_STACK_HEADER_LEN,
                     L7_LOG_MESSAGE_LENGTH);
    buf[L7_LOG_MESSAGE_LENGTH-1] = '\0';
    *bufNdx = ndx;
    return L7_SUCCESS;
  }
  else
  {
    return L7_NOT_EXIST;
  }
}
/**********************************************************************
 * @purpose  Get the specified row in the in-memory log table.
 *
 * @param    ndx @b{(output)} the putative index of a log table entry.
 * @param    buf @b{(output)}  a buffer of at least L7_LOG_MESSAGE_LENGTH
 *                   bytes into which the message is written.
 *
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR  if invalid parameter (buf is null).
 * @returns  L7_NOT_EXIST if ndx parameter specifies an entry that
 *                     does not exist.
 *
 * @notes    The indices are continuously updated as messages are logged.
 *           The possible range of indices is:
 *           [inMemoryLogCount - L7_LOG_IN_MEMORY_COUNT..inMemoryLogCount]
 * @end
 *********************************************************************/

L7_RC_t logServerLogMsgInMemoryGet(L7_uint32 ndx, L7_char8 * buf)
{
  L7_uint32 msgNdx;
  /* Cache a copy of inMemoryLogCount so we aren't confused by the
     logging of new messages. */
  L7_uint32 mc = inMemoryLogCount;

  if (buf == L7_NULL)
    return L7_ERROR;

  /* Can't do a get past the last record or prior to the earliest record. */
  if ((ndx == 0) ||
      (ndx > mc) ||
      ((ndx + (inMemoryLogSize - 1)) < mc))
    return L7_NOT_EXIST;

  /* We scale the logMessagesReceived to be hhead - 1, i.e. the last message */
  if (mc <= inMemoryLogSize)
    msgNdx = ndx - 1;
  else
    msgNdx = (hhead + (inMemoryLogSize - 1)) - (mc - ndx);
  if (msgNdx >= inMemoryLogSize)
    msgNdx -= inMemoryLogSize;

  if (inMemoryLog[msgNdx])
  {
    osapiStrncpySafe(buf, inMemoryLog[msgNdx] + LOG_STACK_HEADER_LEN,
                     L7_LOG_MESSAGE_LENGTH);
    buf[L7_LOG_MESSAGE_LENGTH-1] = '\0';
    return L7_SUCCESS;
  }
  else
  {
    return L7_NOT_EXIST;
  }
}

/*********************************************************************
*
* @purpose  To establish the SMTP connection with the SMTP server.
*
*
* @returns  SSL pointer
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t emailAlertSMTPServerConnectionEstablishment(emailAlertTrigger_t trigger)
{
#ifdef L7_MGMT_SECURITY_PACKAGE
  SSL *con=NULL;
  SSL_CTX *ctx=NULL;
  L7_char8 buf1[100];		
#endif
  L7_int32 sd,i;
  L7_sockaddr_union_t sa;
  static L7_char8 buf[EMAIL_ALERT_LOG_BUFFER_SIZE];
  L7_int32 err;
  L7_int32       addrFamily;
  L7_RC_t rc= L7_FAILURE;		
  L7_LOG_EMAIL_ALERT_MSGTYPE_t   msgType;
  L7_inet_addr_t serverAddr;			
  L7_inet_addr_t serverAddr1;			

  if((trigger == EMAIL_ALERT_TRIGGER_TIMER )	||(trigger == EMAIL_ALERT_TRIGGER_BUFFER_FULL))
  {
		msgType = L7_LOG_EMAIL_ALERT_NON_URGENT;
  }
  else if(trigger == EMAIL_ALERT_TRIGGER_EMERGENCY_MESSAGE)
  {
		msgType = L7_LOG_EMAIL_ALERT_URGENT;
  }
  else 
  {
	return L7_FAILURE;
  }
	
	

  logTaskLock();

	if (usmDbParseInetAddrFromIPAddrHostNameStr(logCfg->cfg.emailAlertInfo.smtpServers[0].smtpServerAddress, &serverAddr1) != L7_SUCCESS)
	{
		if(logCfg->cfg.emailAlertInfo.smtpServers[0].addressType == L7_IP_ADDRESS_TYPE_DNS)
		{
			sysapiPrintf(" DNS lookup Failed \n");
		}
	    logTaskUnlock();		
		return L7_FAILURE;
	}

  logTaskUnlock();

  inetAddrNtoh(&serverAddr1, &serverAddr);			
  addrFamily =serverAddr.family;

  if (osapiSocketCreate(addrFamily, L7_SOCK_STREAM, 0, &sd) != L7_SUCCESS)
  {
    sysapiPrintf("socket open failed - errno %d\n", osapiErrnoGet());
    return L7_FAILURE;
  }

  if(sd == -1)
  {
    return L7_FAILURE;
  }

  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_CONN,"SOCKET CREATION IS SUCCESSFUL");						 			

  memset(&sa, 0, sizeof(L7_sockaddr_union_t));

  if(addrFamily == L7_AF_INET)
  {
      sa.u.sa4.sin_family      = serverAddr.family;
      logTaskLock();
      sa.u.sa4.sin_addr.s_addr = osapiHtonl(serverAddr.addr.ipv4.s_addr);
      sa.u.sa4.sin_port        = osapiHtons(((L7_ushort16 )logCfg->cfg.emailAlertInfo.smtpServers[0].smtpPort));
      logTaskUnlock();
  }
  else if(addrFamily == L7_AF_INET6)
  {
      sa.u.sa6.sin6_family      = serverAddr.family;
      logTaskLock();
      sa.u.sa6.sin6_port        = osapiHtons(((L7_ushort16 )logCfg->cfg.emailAlertInfo.smtpServers[0].smtpPort));
      memcpy(&sa.u.sa6.sin6_addr,&serverAddr.addr.ipv6,sizeof(L7_in6_addr_t));
      logTaskUnlock();
  }


  rc = osapiConnect(sd, (L7_sockaddr_t *)&sa,sizeof(sa));
  if(rc == L7_FAILURE)
  {
    osapiSocketClose(sd);
    return L7_FAILURE;
  }
  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_CONN,"OSAPI CONNECT IS SUCCESSFUL");						 			

#ifdef L7_MGMT_SECURITY_PACKAGE
if(logCfg->cfg.emailAlertInfo.smtpServers[0].securityMode == L7_LOG_EMAIL_ALERT_TLSV1)
{
  ctx=SSL_CTX_new(TLSv1_method());

  con=SSL_new(ctx);
  if(con == L7_NULL)
  {
    osapiSocketClose(sd);
    return L7_FAILURE;
  }


  SSL_set_fd (con, sd);

  if(tls1_new(con) != 1)
  {
    SSL_shutdown(con);
    osapiSocketClose(sd);
    return L7_FAILURE;
  }


  err = SSL_connect(con);
  if(err == -1)
  {
     tls1_clear(con);
    SSL_shutdown(con);
    osapiSocketClose(sd);
    return L7_FAILURE;
  }

  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_CONN,"TLS CONNECTION IS SUCCESSFUL");						 			

  err = emailAlertSMTPServerSecureDataRecv (con, buf, sizeof(buf) - 1);
  if(err == -1)
  {
     tls1_clear(con);
    SSL_shutdown(con);
    osapiSocketClose(sd);
    return L7_FAILURE;
  }
  buf[err] = '\0';
  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);	


  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_220) != L7_NULL)
  {
    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
    memcpy(buf,EMAIL_ALERT_SMTP_EHLO_CMD,strlen(EMAIL_ALERT_SMTP_EHLO_CMD));
     logTaskLock();
     osapiStrncat(buf,"broadcom.com", sizeof("broadcom.com"));
     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
     logTaskUnlock();
    err = emailAlertSMTPServerSecureDataSend (con, buf, strlen(buf));
    if(err == -1)
    {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
    }
    loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		

  }

  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  err = emailAlertSMTPServerSecureDataRecv (con, buf, sizeof(buf) - 1);
  if(err == -1)
  {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
  }
  buf[err] = '\0';

  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);
  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_STARTTLS_CMD) != L7_NULL)
  {
    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
    memcpy(buf,EMAIL_ALERT_SMTP_STARTTLS_CMD,strlen(EMAIL_ALERT_SMTP_STARTTLS_CMD));
    osapiStrncat(buf,"\r\n",sizeof("\r\n"));					
    err = emailAlertSMTPServerSecureDataSend (con, buf, strlen(buf));
    if(err == -1)
    {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
    }
  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);
	
  err = emailAlertSMTPServerSecureDataRecv (con, buf, sizeof(buf) - 1);
  if(err == -1)
  {
     tls1_clear(con);
    SSL_shutdown(con);
    osapiSocketClose(sd);
    return L7_FAILURE;
  }
  buf[err] = '\0';

  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);
	
  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_220) != L7_NULL)
  {
    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
    memcpy(buf,EMAIL_ALERT_SMTP_EHLO_CMD,strlen(EMAIL_ALERT_SMTP_EHLO_CMD));
     logTaskLock();
     osapiStrncat(buf,"broadcom.com", sizeof("broadcom.com"));
     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
     logTaskUnlock();
    err = emailAlertSMTPServerSecureDataSend (con, buf, strlen(buf));
    if(err == -1)
    {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
    }
    loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		

  }

  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  err = emailAlertSMTPServerSecureDataRecv (con, buf, sizeof(buf) - 1);
  if(err == -1)
  {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
  }
  buf[err] = '\0';
	
  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		

  }

  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_AUTHENTICATION_LOGIN_CMD) != L7_NULL)
  {
    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
    memcpy(buf,EMAIL_ALERT_SMTP_AUTHENTICATION_LOGIN_CMD,strlen(EMAIL_ALERT_SMTP_AUTHENTICATION_LOGIN_CMD));
    osapiStrncat(buf,"\r\n",sizeof("\r\n"));		
    err = emailAlertSMTPServerSecureDataSend (con, buf, strlen(buf));
    if(err == -1)
    {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
    }
    loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);
	  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
	  err = emailAlertSMTPServerSecureDataRecv (con, buf, sizeof(buf) - 1);
	  if(err == -1)
	  {
	      tls1_clear(con);
	      SSL_shutdown(con);
	      osapiSocketClose(sd);
	      return L7_FAILURE;
	  }
	  buf[err] = '\0';
     loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		

 	  b64_pton(buf+4,buf1,100);		

    if(osapiStrStr(buf1,"Username") != L7_NULL)
    {
	    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
		 b64_ntop(logCfg->cfg.emailAlertInfo.smtpServers[0].userid,strlen(logCfg->cfg.emailAlertInfo.smtpServers[0].userid),buf, EMAIL_ALERT_LOG_BUFFER_SIZE);	
	    osapiStrncat(buf,"\r\n",sizeof("\r\n"));					
	    err = emailAlertSMTPServerSecureDataSend (con, buf, strlen(buf));
	    if(err == -1)
	    {
	      tls1_clear(con);
	      SSL_shutdown(con);
	      osapiSocketClose(sd);
	      return L7_FAILURE;
	    }
	 	 loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);			
    }

	  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
	  err = emailAlertSMTPServerSecureDataRecv (con, buf, sizeof(buf) - 1);
	  if(err == -1)
	  {
	      tls1_clear(con);
	      SSL_shutdown(con);
	      osapiSocketClose(sd);
	      return L7_FAILURE;
	  }
	  buf[err] = '\0';
     loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		

	  memset(buf1,0,100);
 	  b64_pton(buf+4,buf1,100);				

    if(osapiStrStr(buf1,"Password") != L7_NULL)
    {
	    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
		 b64_ntop(logCfg->cfg.emailAlertInfo.smtpServers[0].passwd,strlen(logCfg->cfg.emailAlertInfo.smtpServers[0].passwd),buf, EMAIL_ALERT_LOG_BUFFER_SIZE);	
       osapiStrncat(buf,"\r\n",sizeof("\r\n"));					
	    err = emailAlertSMTPServerSecureDataSend (con, buf, strlen(buf));
	    if(err == -1)
	    {
	      tls1_clear(con);
	      SSL_shutdown(con);
	      osapiSocketClose(sd);
	      return L7_FAILURE;
	    }
		loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);			
    }
  }

  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  err = emailAlertSMTPServerSecureDataRecv (con, buf, sizeof(buf) - 1);
  if(err == -1)
  {
     tls1_clear(con);
    SSL_shutdown(con);
    osapiSocketClose(sd);
    return L7_FAILURE;
  }
  buf[err] = '\0';
  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);	

  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_235) != L7_NULL)
  {
    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
    memcpy(buf,EMAIL_ALERT_SMTP_FROM_CMD,strlen(EMAIL_ALERT_SMTP_FROM_CMD));
    logTaskLock();
	 osapiStrncat(buf,"<",sizeof("<"));	 
     osapiStrncat(buf,logCfg->cfg.emailAlertInfo.fromAddress,sizeof(logCfg->cfg.emailAlertInfo.fromAddress));
	 osapiStrncat(buf,">",sizeof(">"));	 		
     logTaskUnlock();
     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
    err = emailAlertSMTPServerSecureDataSend (con, buf, strlen(buf));
    if(err == -1)
    {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
    }
    loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		

  }
  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  err = emailAlertSMTPServerSecureDataRecv (con, buf, sizeof(buf) - 1);
  if(err == -1)
  {
     tls1_clear(con);
    SSL_shutdown(con);
    osapiSocketClose(sd);
    return L7_FAILURE;
  }
  buf[err] = '\0';
  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);	

for(i=0;i< L7_MAX_NO_OF_ADDRESSES;i++ )
{
  if(strlen(logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i]) != 0)
  {
	  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_250) != L7_NULL)
	  {
	    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
	    memcpy(buf,EMAIL_ALERT_SMTP_RCPT_CMD,strlen(EMAIL_ALERT_SMTP_RCPT_CMD));
	     logTaskLock();
		 osapiStrncat(buf,"<",sizeof("<"));	 
	    osapiStrncat(buf,logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i],sizeof(logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i]));
		 osapiStrncat(buf,">",sizeof(">"));	 		
	    logTaskUnlock();
	     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
	    err = emailAlertSMTPServerSecureDataSend (con, buf, strlen(buf));
	    if(err == -1)
	    {
	      tls1_clear(con);
	      SSL_shutdown(con);
	      osapiSocketClose(sd);
	      return L7_FAILURE;
	    }
		loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);			
	  }

	  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
	  err = emailAlertSMTPServerSecureDataRecv (con, buf, sizeof(buf) - 1);
	  if(err == -1)
	  {
	     tls1_clear(con);
	    SSL_shutdown(con);
	    osapiSocketClose(sd);
	    return L7_FAILURE;
	  }
	  buf[err] = '\0';
     loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		
	}
}

  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_250) != L7_NULL)
  {
    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
    memcpy(buf,EMAIL_ALERT_SMTP_DATA_CMD,strlen(EMAIL_ALERT_SMTP_DATA_CMD));
     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
    err = emailAlertSMTPServerSecureDataSend (con, buf, strlen(buf));

    if(err == -1)
    {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
    }
    loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		
  }

  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  err = emailAlertSMTPServerSecureDataRecv (con, buf, sizeof(buf) - 1);
  if(err == -1)
  {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
  }
  buf[err] = '\0';

  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);

  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_354) == L7_NULL)
  {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
  }

  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  memcpy(buf,"From: ",strlen("From: "));
  osapiStrncat(buf,logCfg->cfg.emailAlertInfo.fromAddress,sizeof(logCfg->cfg.emailAlertInfo.fromAddress));	
  osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 	

  for(i=0;i< L7_MAX_NO_OF_ADDRESSES;i++ )
  {
  	if(strlen(logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i]) != 0)
  	{
	  osapiStrncat(buf,"To: ", sizeof("To: "));		 	
	  osapiStrncat(buf,logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i],sizeof(logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i]));	
	  osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 	
  	}
  }

  err = emailAlertSMTPServerSecureDataSend (con, buf, strlen(buf));

  if(err == -1)
  {
      tls1_clear(con);
      SSL_shutdown(con);
      osapiSocketClose(sd);
      return L7_FAILURE;
  }
  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);	

	
}
#endif

if(logCfg->cfg.emailAlertInfo.smtpServers[0].securityMode == L7_LOG_EMAIL_ALERT_NONE)
{
  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  err = osapiRead (sd, buf, sizeof(buf) - 1);
  if(err == -1)
  {
    osapiSocketClose(sd);
    return L7_FAILURE;
  }
  buf[err] = '\0';

  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);
	
  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_220) != L7_NULL)
  {
    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
    memcpy(buf,EMAIL_ALERT_SMTP_EHLO_CMD,strlen(EMAIL_ALERT_SMTP_EHLO_CMD));
     logTaskLock();
     osapiStrncat(buf,"broadcom.com", sizeof("broadcom.com"));
     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
     logTaskUnlock();
    err = osapiWrite(sd, buf, strlen(buf));
    if(err == -1)
    {
      osapiSocketClose(sd);
      return L7_FAILURE;
    }
    loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		

  }

  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  err = osapiRead(sd, buf, sizeof(buf) - 1);
  if(err == -1)
  {
    osapiSocketClose(sd);
    return L7_FAILURE;
  }
  buf[err] = '\0';

  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);
  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_250) != L7_NULL)
  {
    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
    memcpy(buf,EMAIL_ALERT_SMTP_FROM_CMD,strlen(EMAIL_ALERT_SMTP_FROM_CMD));
     logTaskLock();
	 osapiStrncat(buf,"<",sizeof("<"));	 
    osapiStrncat(buf,logCfg->cfg.emailAlertInfo.fromAddress,sizeof(logCfg->cfg.emailAlertInfo.fromAddress));
	 osapiStrncat(buf,">",sizeof(">"));	 		
     logTaskUnlock();
     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
    err = osapiWrite(sd, buf, strlen(buf));
    if(err == -1)
    {
      osapiSocketClose(sd);
      return L7_FAILURE;
    }
    loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);
  }

  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  err = osapiRead(sd, buf, sizeof(buf) - 1);
  if(err == -1)
  {
    osapiSocketClose(sd);
    return L7_FAILURE;
  }
  buf[err] = '\0';
  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);

  for(i=0;i< L7_MAX_NO_OF_ADDRESSES;i++ )
  {
  	if(strlen(logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i]) != 0)
  	{
	  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_250) != L7_NULL)
	  {
	    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
	    memcpy(buf,EMAIL_ALERT_SMTP_RCPT_CMD,strlen(EMAIL_ALERT_SMTP_RCPT_CMD));
	     logTaskLock();
		 osapiStrncat(buf,"<",sizeof("<"));	 
	    osapiStrncat(buf,logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i],sizeof(logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i]));
		 osapiStrncat(buf,">",sizeof(">"));	 		
	    logTaskUnlock();
	     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
	    err = osapiWrite(sd, buf, strlen(buf));
	    if(err == -1)
	    {
	      osapiSocketClose(sd);
	      return L7_FAILURE;
	    }
	    loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);			
	  }

	  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
	  err = osapiRead(sd, buf, sizeof(buf) - 1);
	  if(err == -1)
	  {
	    osapiSocketClose(sd);
	    return L7_FAILURE;
	  }
	  buf[err] = '\0';
	  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		
  	}
  }

	
  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_250) != L7_NULL)
  {
    memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
    memcpy(buf,EMAIL_ALERT_SMTP_DATA_CMD,strlen(EMAIL_ALERT_SMTP_DATA_CMD));
     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
    err = osapiWrite(sd, buf, strlen(buf));

    if(err == -1)
    {
      osapiSocketClose(sd);
      return L7_FAILURE;
    }
	 loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);		
  }

  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  err = osapiRead(sd, buf, sizeof(buf) - 1);
  if(err == -1)
  {
    osapiSocketClose(sd);
    return L7_FAILURE;
  }
  buf[err] = '\0';
  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);

  if(osapiStrStr(buf,EMAIL_ALERT_SMTP_CODE_354) == L7_NULL)
  {
    osapiSocketClose(sd);
    return L7_FAILURE;
  }

  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
  memcpy(buf,"From: ",strlen("From: "));
  osapiStrncat(buf,logCfg->cfg.emailAlertInfo.fromAddress,sizeof(logCfg->cfg.emailAlertInfo.fromAddress));	
  osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 	


  for(i=0;i< L7_MAX_NO_OF_ADDRESSES;i++ )
  {
  	if(strlen(logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i]) != 0)
  	{
	  osapiStrncat(buf,"To: ", sizeof("To: "));		 	
	  osapiStrncat(buf,logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i],sizeof(logCfg->cfg.emailAlertInfo.severityTable[msgType-1].toAddress[i]));	
	  osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 			
  	}
  }

  err = osapiWrite(sd, buf, strlen(buf));

  if(err == -1)
  {
    osapiSocketClose(sd);
    return L7_FAILURE;
  }
  loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);	
	
}

emailAlertOperData.sd =sd;
#ifdef L7_MGMT_SECURITY_PACKAGE
if(logCfg->cfg.emailAlertInfo.smtpServers[0].securityMode == L7_LOG_EMAIL_ALERT_TLSV1)
{
	emailAlertOperData.smtp_con=con;
}
#endif

 return L7_SUCCESS;
}
#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose  Send data over the secure SSL connection.
*
* @param    ssl @b{(input)} pointer to secure connection object
* @param    buf @b{(input)} pointer to data
* @param    size @b{(input)} amount of data to send
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_int32 emailAlertSMTPServerSecureDataSend(SSL *ssl, L7_uchar8 *buf, L7_uint32 size)
{
    L7_int32            nWritten = 0;
    L7_int32            totalWritten = 0;
    L7_BOOL             terminateTunnel = L7_FALSE;
    L7_int32            ssl_wfd;


    ssl_wfd = SSL_get_wfd(ssl);

    if (ssl_wfd <= 0)
    {
        return -1;
    }


    while (size > L7_NULL)
    {
        /* Remove any previous errors */
        ERR_clear_error();

        nWritten = SSL_write(ssl, buf, size);


        switch(SSL_get_error(ssl, nWritten))
        {
            case SSL_ERROR_NONE:
                /* Some or all data was sent */
                size -= nWritten;
                totalWritten += nWritten;

                break;

            case SSL_ERROR_ZERO_RETURN:

                break;

            case SSL_ERROR_WANT_READ:

                /* Allow a read to occur first */

                break;

            case SSL_ERROR_WANT_WRITE:

                /* Try the write again */
                break;

            case SSL_ERROR_SSL:

                /* Error, the SSL connection should be terminated */
                terminateTunnel = L7_TRUE;
                break;

            case SSL_ERROR_SYSCALL:

                /* Error, the SSL connection should be terminated */
                terminateTunnel = L7_TRUE;
                break;

            default:

                /* Error, the SSL connection should be terminated */
                terminateTunnel = L7_TRUE;
                break;
        } /* case */

        if (terminateTunnel == L7_TRUE)
        {
            totalWritten = -1;
            break;
        }
    } /* while */

    return totalWritten;


}

/*********************************************************************
*
* @purpose  Recv data over the secure SSL connection.
*
* @param    ssl @b{(input)} pointer to the SSL connection object
* @param    buf @b{(input)} pointer to data
* @param    size @b{(input)} amount of data to recv
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_int32 emailAlertSMTPServerSecureDataRecv(SSL *ssl, L7_uchar8 *buf, L7_uint32 size)
{

    L7_uint32           max_fd;
    L7_int32            result;
    fd_set              read_fds, write_fds;
    struct timeval      timeout;
    L7_int32            rc;
    L7_int32            ssl_rfd;
    L7_int32            nread = 0, nreadTotal = 0;
    L7_BOOL             terminateTunnel = L7_FALSE, BlockedOnRead, BlockedOnWrite;
    L7_uchar8           *bufPtr = buf;

    unsigned long       l;


    ssl_rfd = SSL_get_rfd(ssl);

    if (ssl_rfd <= 0)
    {
        return -1;
    }

    /*
    Set read socket to non-blocking temporarily
    */

    osapiSocketNonBlockingModeSet(ssl_rfd, L7_TRUE);

    bzero(buf, size);

    do
    {
        BlockedOnRead = L7_FALSE;
        BlockedOnWrite = L7_FALSE;

        /* Remove any previous errors */
        ERR_clear_error();

        nread = SSL_read(ssl, bufPtr, size-nreadTotal);

        if(nread >=0)
        {
            nreadTotal += nread;
            bufPtr += nread;
        }


        switch(SSL_get_error(ssl, nread))
        {

            case SSL_ERROR_NONE:

                /* Return the data read */

                break;

            case SSL_ERROR_ZERO_RETURN:
                /* end of data - SSL connection has closed */
                terminateTunnel = L7_TRUE;

                break;

            case SSL_ERROR_WANT_READ:
                BlockedOnRead = L7_TRUE;

                /* Waiting for the incomming SSL record to complete */
                /* Remove any previous errors */
                ERR_clear_error();
                break;

            case SSL_ERROR_WANT_WRITE:
                BlockedOnWrite = L7_TRUE;

                /* Retry the read after a write */
                /* Remove any previous errors */
                ERR_clear_error();

                break;

            case SSL_ERROR_SYSCALL:

                l = ERR_peek_error();

                if ((nread == 0) && (l == 0))
                {
                    /* peer closed connection, drop the connection */
                    terminateTunnel = L7_TRUE;
                }
                else if ((nread == 0) || (errno == EWOULDBLOCK) || (errno == EAGAIN))
                {
                    /* Need to check when socket will be readable again */
                    BlockedOnRead = L7_TRUE;

                    /* Remove any previous errors */
                    ERR_clear_error();
                }
                else
                {
                    /* an error occured, drop the connection */
                    terminateTunnel = L7_TRUE;
                }

                break;

            case SSL_ERROR_SSL:

              l = ERR_peek_error();

               if ((nread == 0) || (errno == EWOULDBLOCK) || (errno == EAGAIN))
                {
                    /* Need to check when socket will be readable again */
                    BlockedOnRead = L7_TRUE;

                    /* Remove any previous errors */
                    ERR_clear_error();
                }
                else if ((nread == -1) && (errno == 0 ) )
                {
                    /* Need to check when socket will be readable again */
                    BlockedOnRead = L7_TRUE;

                    /* Remove any previous errors */
                    ERR_clear_error();

                }
                else
                {
                    /* an error occured, drop the connection */
                    terminateTunnel = L7_TRUE;
                }

                break;

            default:

                /* an error occured, drop the connection */
                terminateTunnel = L7_TRUE;

                break;
        }

        if (terminateTunnel == L7_TRUE)
        {
            /*
            ** Revert to a blocking SSL connection for easier shutdown
            */
            osapiSocketNonBlockingModeSet(ssl_rfd, L7_FALSE);

            return -1;
        }

        rc = SSL_get_shutdown(ssl);

        if (rc & SSL_RECEIVED_SHUTDOWN)
        {
            osapiSocketNonBlockingModeSet(ssl_rfd, L7_FALSE);
            return -1;
        }

        if (nreadTotal == size)
        {
            osapiSocketNonBlockingModeSet(ssl_rfd, L7_FALSE);
            return nreadTotal;
        }

        if (SSL_pending(ssl) && (BlockedOnRead != L7_TRUE) && (BlockedOnWrite != L7_TRUE))
        {
            continue;
        }

        timeout.tv_sec  = 1;
        timeout.tv_usec = L7_NULL;

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        if (BlockedOnRead == L7_TRUE)
        {
            FD_SET(ssl_rfd, &read_fds);
        }
        if (BlockedOnWrite == L7_TRUE)
        {
            FD_SET(ssl_rfd, &write_fds);
        }

        max_fd = ssl_rfd;

        result = select(max_fd + 1, &read_fds, &write_fds, L7_NULLPTR, &timeout);

        if (result < L7_NULL)
        {
            /* An error has occured */


            /*
            ** Revert to a blocking SSL connection for easier shutdown
            */
            osapiSocketNonBlockingModeSet(ssl_rfd, L7_FALSE);

            return -1;
        }
        else if (result == 0)
        {
            /*
            If the select times out, return what we have so far
            */
            osapiSocketNonBlockingModeSet(ssl_rfd, L7_FALSE);
            return nreadTotal;
        }

        if (FD_ISSET(ssl_rfd, &read_fds))
        {
            continue;
        }

        if (FD_ISSET(ssl_rfd, &write_fds))
        {
            continue;
        }

    } while (nreadTotal < size);

    return (rc & SSL_RECEIVED_SHUTDOWN) ? -1 : nreadTotal;


}

#endif
/*********************************************************************
*
* @purpose  To send the non urgent messages on SMTP connection.
*
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t logWriteMsgToSMTPServer(emailAlertTrigger_t trigger, L7_char8 *logBuf  )
{
#ifdef L7_MGMT_SECURITY_PACKAGE
    SSL*smtp_con=L7_NULL;
#endif
    L7_int32 sd;

    if((emailAlertSMTPServerConnectionEstablishment(trigger))!= L7_FAILURE)
    {
      L7_char8 buf[EMAIL_ALERT_LOG_BUFFER_SIZE];
      L7_int32 err=0;

	   loggingEmailAlertTraceWrite(EMAILALERT_SMTP_CONN,"SMTP connection is established");						 			

#ifdef L7_MGMT_SECURITY_PACKAGE
		if(logCfg->cfg.emailAlertInfo.smtpServers[0].securityMode == L7_LOG_EMAIL_ALERT_TLSV1)
		{
		   sd = emailAlertOperData.sd;
		   smtp_con = emailAlertOperData.smtp_con;
          if((sd == -1)||(smtp_con == L7_NULL))
          {
				emailAlertStats.noEmailFailures++;
				return L7_FAILURE;
          }
			 

			if((trigger  == EMAIL_ALERT_TRIGGER_TIMER )||(trigger  == EMAIL_ALERT_TRIGGER_BUFFER_FULL))
			{
				if(emailAlertMsgCount ==0)
				{
                osapiSocketClose(sd);
	             SSL_shutdown(smtp_con);								
				    emailAlertStats.noEmailFailures++;							 
	             return L7_SUCCESS;
				}

		       memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
				memcpy(buf,"Subject:  ",strlen("Subject:  "));								
				osapiStrncat(buf,logCfg->cfg.emailAlertInfo.severityTable[1].subject,strlen(logCfg->cfg.emailAlertInfo.severityTable[1].subject));								
				osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
		       err = emailAlertSMTPServerSecureDataSend (smtp_con, buf, strlen(buf));

		       if(err == -1)
		       {
                osapiSocketClose(sd);
	             SSL_shutdown(smtp_con);								
				  emailAlertStats.noEmailFailures++;							 
				  return L7_FAILURE;
		       }
				loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);

			  if(emailAlertMsgCount != MAX_NUMBER_OF_LOG_MESSAGES)
			  {
					emailAlertMsgCount--;
			  }

		      while(emailAlertMsgCount >= 0 )
		      {
		        memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
		        memcpy(buf,emailAlertBuffer[emailAlertMsgCount].buffer,strlen(emailAlertBuffer[emailAlertMsgCount].buffer));
			    osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
		        err = emailAlertSMTPServerSecureDataSend (smtp_con, buf, strlen(buf));

		        if(err == -1)
		        {
	                osapiSocketClose(sd);
		             SSL_shutdown(smtp_con);
					   emailAlertStats.noEmailFailures++;
		             return L7_FAILURE;
		        }
			    loggingEmailAlertTraceWrite(EMAILALERT_NON_URGENT_MSGS,buf);						 												 
				 logBufferFree(emailAlertBuffer[emailAlertMsgCount].buffer);
		        emailAlertMsgCount--;				 
		      }

		      if(emailAlertMsgCount < 0)
		      {
		        memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
		        memcpy(buf,".",strlen("."));
			     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
		        err = emailAlertSMTPServerSecureDataSend (smtp_con, buf, strlen(buf));

		        if(err == -1)
		        {
	                osapiSocketClose(sd);
		             SSL_shutdown(smtp_con);
					   emailAlertStats.noEmailFailures++;								 
		            return L7_FAILURE;
		        }
				 emailAlertMsgCount	= 0;					
		      }
		  }
		  else if(trigger  == EMAIL_ALERT_TRIGGER_EMERGENCY_MESSAGE) 
		  {
				if(logBuf != L7_NULL)
				{
	              memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
	              memcpy(buf,"Subject:  ",strlen("Subject:  "));								
	              osapiStrncat(buf,logCfg->cfg.emailAlertInfo.severityTable[0].subject,strlen(logCfg->cfg.emailAlertInfo.severityTable[0].subject));								
		  	       osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
	              osapiStrncat(buf,logBuf,strlen(logBuf));
		  	       osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
			       err = emailAlertSMTPServerSecureDataSend (smtp_con, buf, strlen(buf));
							
			       if(err == -1)
			       {
	                osapiSocketClose(sd);
		             SSL_shutdown(smtp_con);
					  emailAlertStats.noEmailFailures++;
		            return L7_FAILURE;
			       }
				   loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);						 
				}

		        err = emailAlertSMTPServerSecureDataSend (smtp_con, "\r\n", strlen("\r\n"));					 
		        if(err == -1)
		        {
                osapiSocketClose(sd);
	             SSL_shutdown(smtp_con);								
				  emailAlertStats.noEmailFailures++;							 
				  return L7_FAILURE;
		        }

			    loggingEmailAlertTraceWrite(EMAILALERT_URGENT_MSGS,buf);						 												 
					

				 memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);						
			     memcpy(buf,".", sizeof("."));		 					 
			     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
		        err = emailAlertSMTPServerSecureDataSend (smtp_con, buf, strlen(buf));

		        if(err == -1)
		        {
                osapiSocketClose(sd);
	             SSL_shutdown(smtp_con);
				  emailAlertStats.noEmailFailures++;							 
				  return L7_FAILURE;
		        }

				  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
				  err = emailAlertSMTPServerSecureDataRecv(smtp_con, buf, sizeof(buf) - 1);
				  if(err == -1)
				  {
				    osapiSocketClose(sd);
 	               SSL_shutdown(smtp_con);						
					 emailAlertStats.noEmailFailures++;								 
				    return L7_FAILURE;
				  }
				  buf[err] = '\0';

				
		  }
	  }
#endif			
		if(logCfg->cfg.emailAlertInfo.smtpServers[0].securityMode == L7_LOG_EMAIL_ALERT_NONE)
		{
		   sd = emailAlertOperData.sd;
          if(sd == -1)
          {
				emailAlertStats.noEmailFailures++;
				return L7_FAILURE;
          }

			if((trigger  == EMAIL_ALERT_TRIGGER_TIMER )||(trigger  == EMAIL_ALERT_TRIGGER_BUFFER_FULL))
			{
				if(emailAlertMsgCount ==0)
				{
                osapiSocketClose(sd);
				  emailAlertStats.noEmailFailures++;								
	             return L7_SUCCESS;
				}

		       memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
				memcpy(buf,"Subject:  ",strlen("Subject:  "));								
				osapiStrncat(buf,logCfg->cfg.emailAlertInfo.severityTable[1].subject,strlen(logCfg->cfg.emailAlertInfo.severityTable[1].subject));								
				osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
		       err = osapiWrite(sd, buf, strlen(buf));

		       if(err == -1)
		       {
                osapiSocketClose(sd);
				  emailAlertStats.noEmailFailures++;								
				  return L7_FAILURE;
		       }
			   loggingEmailAlertTraceWrite(EMAILALERT_SMTP_MSGS,buf);						 					 

			  if(emailAlertMsgCount != MAX_NUMBER_OF_LOG_MESSAGES)
			  {
					emailAlertMsgCount--;
			  }

		      while(emailAlertMsgCount >= 0 )
		      {
		        memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
		        memcpy(buf,emailAlertBuffer[emailAlertMsgCount].buffer,strlen(emailAlertBuffer[emailAlertMsgCount].buffer));
			    osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
		        err = osapiWrite(sd, buf, strlen(buf));

		        if(err == -1)
		        {
	                osapiSocketClose(sd);
					  emailAlertStats.noEmailFailures++;									
					  return L7_FAILURE;
		        }
			    loggingEmailAlertTraceWrite(EMAILALERT_NON_URGENT_MSGS,buf);						 						
				 logBufferFree(emailAlertBuffer[emailAlertMsgCount].buffer);						
		        emailAlertMsgCount--;
		      }

		      if(emailAlertMsgCount < 0)
		      {
		        memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
		        memcpy(buf,".",strlen("."));
			     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
		        err = osapiWrite(sd, buf, strlen(buf));

		        if(err == -1)
		        {
	                osapiSocketClose(sd);
  					  emailAlertStats.noEmailFailures++;									
					  return L7_FAILURE;
		        }
				 emailAlertMsgCount	=0;					
		      }
		  }
		  else if(trigger  == EMAIL_ALERT_TRIGGER_EMERGENCY_MESSAGE) 
		  {
				if(logBuf != L7_NULL)
				{
	              memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
	              memcpy(buf,"Subject:  ",strlen("Subject:  "));								
	              osapiStrncat(buf,logCfg->cfg.emailAlertInfo.severityTable[0].subject,strlen(logCfg->cfg.emailAlertInfo.severityTable[0].subject));								
		  	       osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
	              osapiStrncat(buf,logBuf,strlen(logBuf));
		  	       osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
			       err = osapiWrite(sd, buf, strlen(buf));
							
			       if(err == -1)
			       {
	                osapiSocketClose(sd);
					  emailAlertStats.noEmailFailures++;									
					  return L7_FAILURE;
			       }
				    loggingEmailAlertTraceWrite(EMAILALERT_URGENT_MSGS,buf);						 												 
				}


		        err = osapiWrite(sd, "\r\n", strlen("\r\n"));					 
		        if(err == -1)
		        {
                osapiSocketClose(sd);
				  emailAlertStats.noEmailFailures++;								
				  return L7_FAILURE;
		        }


					

				 memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);						
			     memcpy(buf,".", sizeof("."));		 					 
			     osapiStrncat(buf,"\r\n", sizeof("\r\n"));		 
		        err = osapiWrite(sd, buf, strlen(buf));

		        if(err == -1)
		        {
                osapiSocketClose(sd);
             	  emailAlertStats.noEmailFailures++;								
				  return L7_FAILURE;
		        }

				  memset(buf,0,EMAIL_ALERT_LOG_BUFFER_SIZE);
				  err = osapiRead(sd, buf, sizeof(buf) - 1);
				  if(err == -1)
				  {
				    osapiSocketClose(sd);
					 emailAlertStats.noEmailFailures++;						
				    return L7_FAILURE;
				  }
				  buf[err] = '\0';

						
		  }
		}

       emailAlertStats.noEmailsSent++;					
	   emailAlertStats.timeSinceLastMail=  osapiUpTimeRaw();
      return L7_SUCCESS;
    }
	else
	{
	   loggingEmailAlertTraceWrite(EMAILALERT_SMTP_CONN,"SMTP connection is not established");						 			
	}

	emailAlertStats.noEmailFailures++;
	logemailAlertSendEmailFailureTrap(emailAlertStats.noEmailFailures);
    return L7_FAILURE;

}

/**********************************************************************
 * @purpose  To print the email alert stats
 *
 * @notes
 *
 * @end
 *********************************************************************/

void emailAlertDebugStatsShow()
{
  sysapiPrintf("\nemail Alert Status\n");

  sysapiPrintf("Global Admin Status: %s\n",
               L7_ADMIN_MODE_ENABLE == logCfg->cfg.emailAlertInfo.emailAlertingStatus? "Enabled" : "Disabled" );

  sysapiPrintf("No of emails Sent: %d\n",
               emailAlertStats.noEmailsSent);

  sysapiPrintf("No of email Failures: %d\n",
               emailAlertStats.noEmailFailures);

  sysapiPrintf("Time since last email was sent: %d\n",
               emailAlertStats.timeSinceLastMail);

}
/**********************************************************************
 * @purpose  To generate Trap Message
 *
 * @notes
 *
 * @end
 *********************************************************************/

void emailAlertDebugGenerateTrapMsg()
{

  L7_char8 logbuffer[L7_CLI_MAX_STRING_LENGTH] = "Testing Trap message";

  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_TRAPMGR_COMPONENT_ID, logbuffer);

}

/*********************************************************************
* @purpose  Set the Email Alert trace flags
*
* @param    flags  - Trace Flags
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void loggingEmailAlertTraceFlagsSet(L7_uint32 flags)
{
  emailAlertTraceFlags = flags;
}

/*********************************************************************
* @purpose  Set the Email Alert trace flags
*
* @param    emaiAlertTraceFlags  - Trace Flags
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void loggingEmailAlertTraceWrite(emailAlertTraceFlags_t flag,L7_uchar8 *emailTrace)
{
  L7_uchar8 debugMsg[EMAIL_ALERT_MAX_TRACE_LEN];
  L7_uint32 stackUptime;
  L7_uint32 secs;
  L7_uint32 msecs;
	
	if(!(emailAlertTraceFlags& flag))
	{
    	return;
	}
		
   if (emailTrace == NULL)
   {
		return;
   }

	stackUptime = simSystemUpTimeMsecGet();
	secs = stackUptime / 1000;
	msecs = stackUptime % 1000;
	 

	osapiSnprintf(debugMsg, EMAIL_ALERT_MAX_TRACE_LEN, "\n%d.%03d %s",
	              secs, msecs,  emailTrace);

	printf("%s\n",debugMsg);

}


/**********************************************************************
 * @purpose  To print the email alert stats
 *
 * @notes
 *
 * @end
 *********************************************************************/

void emailAlertDebugConfigShow()
{
  sysapiPrintf("\nemail Alert Config\n");

  sysapiPrintf("Global Admin Status: %s\n",
               L7_ADMIN_MODE_ENABLE == logCfg->cfg.emailAlertInfo.emailAlertingStatus? "Enabled" : "Disabled" );

  sysapiPrintf("Global Operational Status: %s\n",
               L7_ENABLE == emailAlertOperData.emailAlertingOperStatus? "Enabled" : "Disabled" );

  sysapiPrintf("Email Alert From address: %s\n",
               logCfg->cfg.emailAlertInfo.fromAddress);

  sysapiPrintf("Email alert To Address: %s\n",
               logCfg->cfg.emailAlertInfo.severityTable[0].toAddress[0] );

  sysapiPrintf("Email alert log duration: %d\n",
               logCfg->cfg.emailAlertInfo.logDuration);

  sysapiPrintf("Email alert SMTP server address: %s\n",
               logCfg->cfg.emailAlertInfo.smtpServers[0].smtpServerAddress);

  sysapiPrintf("Email alert SMTP server port: %x\n",
               (L7_ushort16)logCfg->cfg.emailAlertInfo.smtpServers[0].smtpPort);

  sysapiPrintf("Email alert urgent severity level: %d\n",
               logCfg->cfg.emailAlertInfo.urgentSeverityLevel);

  sysapiPrintf("Email alert non urgent severity level: %d\n",
               logCfg->cfg.emailAlertInfo.nonUrgentSeverityLevel);

  sysapiPrintf("Email alert userid: %s\n",
               logCfg->cfg.emailAlertInfo.smtpServers[0].userid);

  sysapiPrintf("Email alert passwd: %s\n",
               logCfg->cfg.emailAlertInfo.smtpServers[0].passwd);


}

/*********************************************************************
* @purpose  To print above debug commands for help
*
* @param    void
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void emailAlertDebugHelp(void)
{
  printf("\nEmail Alert Debug commands supported:\n");
  printf("-----------------------------\n");
  printf("1. emailAlertDebugStatsShow()           - Dump the email alert debug statistics.\n");
  printf("2. emailAlertDebugConfigShow()         - Dump the email alert config.\n");
  printf("3. loggingEmailAlertTraceFlagsSet()        - Set the flags for email Alert tracing.\n");
}


/**********************************************************************
 * @purpose  Show the status of the log server
 *
 * @notes    Print the status of the log server to stdout.
 *
 * @end
 *********************************************************************/

void logStatusShow()
{
  register L7_int32 i;

  sysapiPrintf("\nLOG Status\n");
  sysapiPrintf("Global Admin Status: %s\n",
               L7_ADMIN_MODE_ENABLE == logCfg->cfg.globalAdminStatus ? "Enabled" : "Disabled" );
  sysapiPrintf("Memory admin status: %s\n",
               logCfg->cfg.inMemoryAdminStatus ? "Enabled" : "Disabled");
  sysapiPrintf("Memory behavior: %s\n",
               logCfg->cfg.inMemoryBehavior == L7_LOG_STOP_ON_FULL_BEHAVIOR
               ? "Stop on full" : "Wrap");
  sysapiPrintf("Memory component filter: %d\n",
               logCfg->cfg.inMemoryComponentFilter);
  sysapiPrintf("Memory severity filter: %d\n",
               logCfg->cfg.inMemorySeverityFilter);
  sysapiPrintf("Memory log size (configured): %d entries\n",
               logCfg->cfg.inMemorySize);
  sysapiPrintf("Memory log size (actual): %d entries\n",
               inMemoryLogSize );
  sysapiPrintf("Memory log count:  %d entries\n",
               inMemoryLogCount );
  sysapiPrintf("Console admin status: %s\n",
               logCfg->cfg.consoleAdminStatus ? "Enabled" : "Disabled");
  sysapiPrintf("Console component filter: %d\n",
               logCfg->cfg.consoleComponentFilter);
  sysapiPrintf("Console severity filter: %d\n",
               logCfg->cfg.consoleSeverityFilter);
  sysapiPrintf("Local port: %hd\n",
               logCfg->cfg.localPort);
  sysapiPrintf("Persistent admin status: %s\n",
               logCfg->cfg.persistentAdminStatus ? "Enabled" : "Disabled");
  sysapiPrintf("Persistent severity filter: %d\n",
               logCfg->cfg.persistentSeverityFilter);
  sysapiPrintf("Persistent log count:  %d entries\n",
               logServerPersistentLogMessageCount() );
  sysapiPrintf("Syslog admin status: %s\n",
               logCfg->cfg.syslogAdminStatus ? "Enabled" : "Disabled");
  sysapiPrintf("Syslog default facility: %d\n",
               logCfg->cfg.syslogDefaultFacility);
  for (i = 0; i < L7_LOG_MAX_HOSTS; i++)
  {
    sysapiPrintf("\thost[%d] status: %d\n",i,
                 logCfg->cfg.host[i].status);
    if ((logCfg->cfg.host[i].status == L7_ROW_STATUS_ACTIVE) ||
        (logCfg->cfg.host[i].status == L7_ROW_STATUS_NOT_IN_SERVICE))
    {
      sysapiPrintf("\thost[%d] address: %s\n", i,
                   logCfg->cfg.host[i].address);
      sysapiPrintf("\thost[%d] address type: %d\n", i,
                   logCfg->cfg.host[i].addressType);
      sysapiPrintf("\thost[%d] port: %d\n", i,
                   logCfg->cfg.host[i].port);
      sysapiPrintf("\thost[%d] component: %d\n", i,
                   logCfg->cfg.host[i].componentFilter);
      sysapiPrintf("\thost[%d] severity: %d\n", i,
                   logCfg->cfg.host[i].severityFilter);
      sysapiPrintf("\thost[%d] description: %.*s\n", i, L7_LOG_HOST_DESCRIPTION_LEN,
                   logCfg->cfg.host[i].description );
      sysapiPrintf("\thost[%d] messages sent:  %d\n", i,
                   logOps[i].syslogMessageForwardCount );
    }
  }

  sysapiPrintf("Log messages received: %d\n", logMessagesReceived);

  sysapiPrintf("Log messages dropped due to lack of resource: %d\n", logMessagesResourceIssuesDropped);
  sysapiPrintf("Log messages not sent to stack manager : %d\n", logMessagesToStackMgrDropped);
  sysapiPrintf("Log messages dropped due to logging disabled: %d\n", logMessagesLoggingDisabledDropped);

  sysapiPrintf("Last receive time: %d\n", logReceiveTimestamp);
  sysapiPrintf("Memory log messages dropped due to configuration: %d\n", logMemMessagesIgnored );
  sysapiPrintf("Persistent log messages dropped due to configuration: %d\n", logPersMessagesIgnored );
  sysapiPrintf("Console log messages dropped due to configuration: %d\n", logConsoleMessagesIgnored );
  sysapiPrintf("Syslog messages relayed: %d\n", logSyslogMessagesRelayed);
  sysapiPrintf("Syslog message delivered time: %d\n",
               logSyslogMessageDeliveredTimestamp);
  sysapiPrintf("Startup log message count: %d\n", logServerPersistentLogMessageCount());


}

/**********************************************************************
 * @purpose  Print the buffered logs to stdout.
 *
 *
 * @end
 *********************************************************************/

void logMemoryShow(L7_uint32 rev)
{
  L7_uint32 cnt;
  L7_uint32 maxCount = (inMemoryLogCount > inMemoryLogSize)
                       ? inMemoryLogSize : inMemoryLogCount;

  L7_uint32 ndx;

  if ( rev == 0)
  {
    ndx = (inMemoryLogCount > inMemoryLogSize) ? hhead : 0;
  }
  else
  {
    ndx = (inMemoryLogCount > L7_LOG_IN_MEMORY_LOG_COUNT) ?
          (inMemoryLogCount % L7_LOG_IN_MEMORY_LOG_COUNT) : inMemoryLogCount;
    ndx--;
  }

  sysapiPrintf("\nBuffered Log Count: %d\n", maxCount);
  if (maxCount)
  {
    sysapiPrintf("\nBuffered Log\n");
  }
  for (cnt = 0; cnt < maxCount; cnt++)
  {
    if (inMemoryLog[ndx])
    {
      sysapiPrintf(inMemoryLog[ndx] + LOG_STACK_HEADER_LEN);
      sysapiPrintf("\n");
    }
    if ( rev == 0)
    {
      ndx = (++ndx >= L7_LOG_IN_MEMORY_LOG_COUNT) ? 0 : ndx;
    }
    else
    {
      ndx = (ndx != 0) ? ndx : L7_LOG_IN_MEMORY_LOG_COUNT;
      ndx--;
    }
  }
}

