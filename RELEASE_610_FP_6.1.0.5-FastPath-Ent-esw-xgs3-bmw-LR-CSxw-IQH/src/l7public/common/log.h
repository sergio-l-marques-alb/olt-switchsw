/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename log.h
*
* @purpose This file contains definitions used by log module
*
* @component
*
* @comments
*
* @create
*
* @author various
*
* @end
*
**********************************************************************/


#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "l7_common.h"

#ifndef OSAPIPROTO_HEADER
#include "osapi.h"
#endif

#ifndef FLEX_H
#include "flex.h"
#endif

/* Maximum message log size */
#define LOG_MSG_MAX_MSG_SIZE                   512         
#define L7_LOG_FORMAT_BUF_SIZE                 LOG_MSG_MAX_MSG_SIZE
#define EVENTLOG_MAX_SIZE                      (16*1024)

#define LOG_FILE_NAME                          "log2.bin"
#define ASCII_LOG_FILE_NAME                    "asciilog.bin"
#define MSG_LOG_FILE_NAME                      "msg.txt"
#define TRAP_LOG_FILE_NAME                     "trap.txt"
#ifdef L7_DHCP_SNOOPING_PACKAGE
#define DHCP_SNOOPING_UPLOAD_FILE_NAME                 "dsUploadDb.txt" 
#define DHCP_SNOOPING_DOWNLOAD_FILE_NAME               "dsDownloadDb.txt" 
#endif

/*This is the place where valgrind log files are stored. The file name should be the 
 same as specified in rc.fastpath*/
#define VALG_LOG_PATH                           "/usr/local/ptin/log/fastpath/bin"
#define VALG_LOG_FILE_NAME                      "valgrind_run.log.rss"


/* Default component */
#define L7_LOG_COMPONENT_DEFAULT               ((L7_COMPONENT_IDS_t) L7_ALL_COMPONENTS)

/* Log behavior for buffered log. */
typedef enum
{
  L7_LOG_WRAP_BEHAVIOR,
  L7_LOG_STOP_ON_FULL_BEHAVIOR
} L7_LOG_FULL_BEHAVIOR_t;


/* LOG FACILITIIES */

typedef enum
{
  /* kernel messages */
  L7_LOG_FACILITY_KERNEL = 0,
  /* user-level messages */
  L7_LOG_FACILITY_USER = 1,
 /* mail system */
  L7_LOG_FACILITY_MAIL = 2,
  /* system daemons */
  L7_LOG_FACILITY_SYSTEM = 3,
  /* security/authorization messages (note 1) */
  L7_LOG_FACILITY_SECURITY = 4,
  /* messages generated internally by syslogd */
  L7_LOG_FACILITY_SYSLOG = 5,
  /* line printer subsystem */
  L7_LOG_FACILITY_LPR = 6,
  /* network news subsystem */
  L7_LOG_FACILITY_NNTP = 7,
  /* UUCP subsystem */
  L7_LOG_FACILITY_UUCP = 8,
  /* clock daemon (note 2) */
  L7_LOG_FACILITY_CRON = 9,
  /* security/authorization messages (note 1) */
  L7_LOG_FACILITY_AUTH = 10,
  /* FTP daemon */
  L7_LOG_FACILITY_FTP = 11,
  /* NTP subsystem */
  L7_LOG_FACILITY_NTP = 12,
  /* log audit (note 1) */
  L7_LOG_FACILITY_AUDIT = 13,
  /* log alert (note 1) */
  L7_LOG_FACILITY_ALERT = 14,
  /* clock daemon (note 2) */
  L7_LOG_FACILITY_CLOCK = 15,
  /* local use 0  (local0) */
  L7_LOG_FACILITY_LOCAL0 = 16,
  /* local use 1  (local1) */
  L7_LOG_FACILITY_LOCAL1 = 17,
  /* local use 2  (local2) */
  L7_LOG_FACILITY_LOCAL2 = 18,
  /* local use 3  (local3) */
  L7_LOG_FACILITY_LOCAL3 = 19,
  /* local use 4  (local4) */
  L7_LOG_FACILITY_LOCAL4 = 20,
  /* local use 5  (local5) */
  L7_LOG_FACILITY_LOCAL5 = 21,
  /* local use 6  (local6) */
  L7_LOG_FACILITY_LOCAL6 = 22,
  /* local use 7  (local7) */
  L7_LOG_FACILITY_LOCAL7 = 23,
  /* All facilities */
  L7_LOG_FACILITY_ALL = 0xff
} L7_LOG_FACILITY_t;


/* LOG SEVERITY LEVELS */

typedef enum
{
  /* Emergency (0): system is unusable. System failure has occurred. */
  L7_LOG_SEVERITY_EMERGENCY = 0,
  /* Alert (1): action must be taken immediately.
  * Unrecoverable failure of a component. System failure likely.
  */
  L7_LOG_SEVERITY_ALERT = 1,
  /* Critical (2): critical conditions.
  * Recoverable failure of a component that may lead to system failure.
  */
  L7_LOG_SEVERITY_CRITICAL = 2,
  /*  Error (3): error conditions. Recoverable failure of a component. */
  L7_LOG_SEVERITY_ERROR = 3,
  /* Warning (4): warning conditions. Minor failure,
  * e.g. misconfiguration of a component.
  */
  L7_LOG_SEVERITY_WARNING = 4,
  /*  Notice(5): normal but significant conditions. */
  L7_LOG_SEVERITY_NOTICE = 5,
  /* Informational(6): informational messages. */
  L7_LOG_SEVERITY_INFO = 6,
  /* Debug(7): debug-level messages. */
  L7_LOG_SEVERITY_DEBUG = 7,
} L7_LOG_SEVERITY_t;


/*-------------------------------------------------------------------*/
/*
 *  DEBUG TRACE MACROS: 
 *
 *  Purpose:  The macros defined in this section are available to provide 
 *            support for the CLI "debug" command. 
 *
 *            These macros are RESTRICTED to those trace
 *            points which will be logged as a result of
 *            invocation of a CLI "debug" command.
 *
 *            The output of all trace points invoked via the use of
 *            these macros MUST be documented in end user documentation.
 *
 */
/*-------------------------------------------------------------------*/
/* Log Msg with Component specifier.  This message log should always have the
   same severity as LOG_MSG.     */
#define LOG_USER_TRACE(__comp__,__fmt__, __args__... )                      \
  if (logCnfgrStateCheck() != L7_TRUE)                                      \
  {                                                                         \
    log_user_trace(__comp__,                                                \
                   __FILE__, __LINE__, __fmt__, ## __args__);               \
  }                                                                         \
  else                                                                      \
  {                                                                        \
    l7_logf(L7_LOG_SEVERITY_DEBUG, __comp__,                               \
            __FILE__, __LINE__, __fmt__, ## __args__);                      \
  }

/*-------------------------------------------------------------------*/
/*
 *  BASIC SYSLOG MACROS: 
 *
 *  Purpose:  These are available to provide basic syslog support. 
 *
 */
/*-------------------------------------------------------------------*/

/* Log with a pre-formatted string */

#define L7_LOG(__sev__, __comp__, __nfo__)                                      \
          l7_log(__sev__, __comp__, __FILE__, __LINE__, __nfo__)          


/* Log with a format string and variable arguments */

#define L7_LOGF(__sev__, __comp__, __fmt__,__args__...)                         \
          l7_logf(__sev__, __comp__, __FILE__, __LINE__, __fmt__, ## __args__)
          

/*_------------------------------------------------------------------*/
/*
 *  BONUS SYSLOG MACROS: 
 *
 *  Purpose:  These are available to allow for extended debug.
 *            These should primarily be used in debug routines.
 *
 */
/*-------------------------------------------------------------------*/

 
/* Log with a pre-formatted string - no component */

#define L7_LOGNC(__sev__, __nfo__)                                              \
          l7_log(__sev__, L7_LOG_COMPONENT_DEFAULT,                             \
                 __FILE__, __LINE__, __nfo__)

/* Log with a format string and variable arguments - no component */

#define L7_LOGFNC(__sev__, __fmt__, __args__... )                               \
          l7_logf(__sev__, L7_LOG_COMPONENT_DEFAULT,                            \
                  __FILE__, __LINE__, __fmt__, ## __args__)



/*-------------------------------------------------------------------*/
/*
 *  EXTENDED LOG MACROS:  
 *
 *  
 *  To ensure consistent assignment of severity levels for
 *  similar types of messages across FASTPATH, the FASTPATH
 *  extended macros should be used instead.
 * 
 *  The extended log macros are mapped to a syslog macro above.  
 *  Each of the extended macros is mapped to a fixed severity. 
 *  Some are mapped to the default component.
 *
 *
 */
/*-------------------------------------------------------------------*/
             
/* Default Msg Log Macro for all logs  */
#define LOG_MSG(__fmt__, __args__... )                                          \
          l7_logf(L7_LOG_SEVERITY_INFO, L7_LOG_COMPONENT_DEFAULT,               \
                  (char *)__FILE__, __LINE__, __fmt__, ## __args__)

/* Log Msg with Component specifier.  This message log should always have the
   same severity as LOG_MSG.     */
#define LOG_COMPONENT_MSG(__comp__,__fmt__, __args__... )                       \
          l7_logf(L7_LOG_SEVERITY_INFO, __comp__,                               \
                  __FILE__, __LINE__, __fmt__, ## __args__)

#define LOG_DEBUG_MSG(__fmt__, __args__... )                                          \
          l7_logf(L7_LOG_SEVERITY_DEBUG, L7_LOG_COMPONENT_DEFAULT,               \
                  __FILE__, __LINE__, __fmt__, ## __args__)


#define LOG_ERROR(error_code)                                                   \
          log_error_code ((L7_uint32) (error_code), __FILE__, __LINE__)

#define LOG_EVENT(event_code)                                                   \
          l7_log_event(L7_LOG_SEVERITY_CRITICAL, L7_LOG_COMPONENT_DEFAULT,      \
                       __FILE__, __LINE__, event_code)

#ifdef L7_DEBUG
#define L7_assert(test)  \
        if ((test) !=0)    \
        { LOG_ERROR(1); }
#else
#define L7_assert(test)  \
        if ((test) !=0)    \
        { LOG_MSG("Assertion failed"); }
#endif



/*-------------------------------------------------------------------*/
/*
 *  EXTENDED LOG INITIALIZATION PARAMETERS
 *
 */
/*-------------------------------------------------------------------*/


/* Defines used to enable/disable displaying events on the TTY and 
   logging to FLASH 
*/
#define LOG_ENABLE_TTY		1
#define LOG_DISABLE_TTY 	2
#define LOG_ENABLE_FLASH 	5
#define LOG_DISABLE_FLASH 	6
#define LOG_ENABLE_RAM		7
#define LOG_DISABLE_RAM		8


/* Log entry type indicates which processor caused the error, and
** format of the error log entry. 
*/
#define LOGFMT_EVENT         0x00000001
#define LOGFMT_ERROR         0x00000002
#define LOGFMT_MACH_CHECK    0x00000003
#define LOGFMT_WATCHDOG      0x00000004


/* Reset types.
*/
#define POWER_ON_RESET  ((long) 0)
#define BUTTON_RESET    ((long) 1)
#define WATCHDOG_RESET  ((long) 2)
#define USER_RESET      ((long) 3)



/*-------------------------------------------------------------------*/
/*
 *  EXTENDED LOG FUNCTION PROTOTYPES  
 *
 */
/*-------------------------------------------------------------------*/

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

extern void l7_log_event(L7_LOG_SEVERITY_t severity, L7_COMPONENT_IDS_t component,
                         L7_char8 * fileName, L7_uint32 lineNum, L7_ulong32 eventCode);


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

extern void l7_logf(L7_LOG_SEVERITY_t severity, L7_COMPONENT_IDS_t component, 
                    L7_char8 * fileName, L7_uint32 lineNum, L7_char8 * format, ...);


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

extern void l7_log(L7_LOG_SEVERITY_t severity, L7_COMPONENT_IDS_t component,
                   L7_char8 * fileName, L7_uint32 lineNum, L7_char8 * nfo);


/**********************************************************************
* @purpose Format and record a message in the logs.
*
* @param  facility @b((input)} See RFC 3164 Section 4.1.1 Table 1
* @param  severity @b((input)} See RFC 3164 Section 4.1.1 Table 2
* @param  component @b((input)} Level 7 component id
* @param  ttime @b{(input)} time of event
* @param  host @b{(input)} generating host
* @param  tid @b{(input)} task id
* @param  fileName @b{(input)} file name
* @param  line @b{(input)} line number
* @param  nfo @b{(input)} extra information - null terminated string
*
* @returns  None
*
* @notes  This executes on the calling task thread. This routine may
*         be called from interrupt context. We make no assumption
*         about the callers memory storage for any arguments.
*         This routine makes a copy of all arguments and formats
*         them into a locally allocated buffer.
*         Implementation of this function is in log_server.c.
*
*
* @end
*********************************************************************/


extern void logmsg(L7_LOG_FACILITY_t facility, L7_LOG_SEVERITY_t severity, 
                   L7_COMPONENT_IDS_t component, L7_clocktime ttime, L7_uint32 stk,
                   L7_uint32 tid, L7_char8 * fileName, L7_uint32 line, L7_char8 * nfo);

/*********************************************************************
* @purpose  Log error and reset the box.
*
* @param    error_code - 32-bit error code.
* @param    file_name  - File where the error ocurred.
* @param    line_num - Line number where the error occurred.
*
* @returns  none
*
* @notes    This function may be called from an interrupt handler.
*
* @end
*********************************************************************/
extern void log_error_code (L7_uint32 err_code, 
                            L7_char8 *file_name,
                            L7_uint32 line_num);

/*********************************************************************
* @purpose  Log fault and reset the box.
*
* @param    error_code - 32-bit error code.
* @param    arg1 - arg8  - Platform-specific parameters.
*
* @returns  none
*
* @notes    This function may be called from an interrupt handler.
*
* @end
*********************************************************************/
extern void log_fault_code (L7_uint32 err_code,
                            L7_uint32 arg1,
                            L7_uint32 arg2,
                            L7_uint32 arg3,
                            L7_uint32 arg4,
                            L7_uint32 arg5,
                            L7_uint32 arg6,
                            L7_uint32 arg7,
                            L7_uint32 arg8);

/**********************************************************************
* @purpose  Return the default facility
*
* @param    void
*
* @notes    Implementation of this function is in log_server.c.
* @end
*********************************************************************/
extern L7_LOG_FACILITY_t logDefaultFacilityGet();


/**********************************************************************
* @purpose Log an event
*
* @param  *file         @b((input)} Pointer to file name
* @param  line          @b((input)} line number
* @param  error_code    @b((input)} error code
*
* @returns  void
*
* @notes  None.
*
* @end
*********************************************************************/
extern void log_event (char * file, unsigned long line, unsigned long error_code);

/**********************************************************************
* @purpose Log an event, even from an interrupt handler
*
* @param  *file         @b((input)} Pointer to file name
* @param  line          @b((input)} line number
* @param  error_code    @b((input)} error code
*
* @returns  void
*
* @notes  None.
*
* @end
*********************************************************************/
extern void log_event_int (char * file, unsigned long line, unsigned long error_code);

/**********************************************************************
* @purpose Log a message
*
* @param  unit          @b((input)} unit number
* @param  *file         @b((input)} Pointer to file name
* @param  line          @b((input)} line number
* @param  *buf          @b((input)} pointer to message
*
* @returns  void
*
* @notes  None.
*
* @end
*********************************************************************/
extern void log_msg (unsigned int unit, char * file, unsigned long line, char * buf);


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
extern void log_user_trace(L7_COMPONENT_IDS_t component, L7_char8 * fileName, 
                           L7_uint32 lineNum, L7_char8 * nfo, ...);


/*********************************************************************
* @purpose  Check whether logging is ready.
*
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
extern L7_BOOL logCnfgrStateCheck (void);

#ifdef __cplusplus
}
#endif


#endif
