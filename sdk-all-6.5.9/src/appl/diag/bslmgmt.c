/*
 * $Id: $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Broadcom System Log Management
 */

#include <sal/core/libc.h> 
#include <sal/appl/io.h> 
#include <shared/bsl.h>
#include <shared/bslnames.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/bsltrace.h>
#include <appl/diag/bslfile.h>
#include <appl/diag/bslsink.h>
#include <appl/diag/bslmgmt.h>

/* PTin added */
#if 1
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define LOG_MSG_MAX_MSG_SIZE    512
#define MAX_OUTBUF_LEN          512 /* Output buffer max length */
#define MAX_FILE_LEN            15  /* Filename max length */
#define MAX_FUNC_LEN            35  /* Function max length */
#define MAX_LINE_LEN            5   /* Line# max length */
#define MAX_FILEFUNCLINE_LEN    30  /* Filename+function+line# max length */
#define MAX_TIMESTAMP_LEN       24  /* Timestamp max length*/
#define MAX_LOG_LINES         4096  /* Max File Lines */

/* Severity */
typedef enum {
    LOG_OFF = 0,        /* Turn of any print */
    LOG_SEV_PRINT,      /* Always print */
    LOG_SEV_FATAL,
    LOG_SEV_CRITICAL,
    LOG_SEV_ERROR,
    LOG_SEV_WARNING,
    LOG_SEV_NOTICE,
    LOG_SEV_INFO,
    LOG_SEV_VERBOSE,
    LOG_SEV_DEBUG,
    LOG_SEV_TRACE,
    /* Last element */
    LOG_SEV_LAST,
} log_severity_t;

/* Severity strings */
static const char *log_sev_str[LOG_SEV_LAST] = {
    "OFF",
    "[PRINT]   ",
    "[FATAL]   ",
    "[CRITICAL]",
    "[ERROR]   ",
    "[WARNING] ",
    "[NOTICE]  ",
    "[INFO]    ",
    "[VERBOSE] ",
    "[DEBUG]   ",
    "[TRACE]   ",
};

/**
 * Composes a string with a timestamp
 * 
 * @param output Pointer to the output string
 * 
 * @return char* Returns the same input pointer
 */
static char* get_time(char* output)
{
    struct timeval   tv;
    struct tm        date;
    time_t           timeabs;

    if (output) {
        time (&timeabs);
        localtime_r (&timeabs, &date);
        gettimeofday (&tv, NULL);
        sprintf (output, "%04d%02d%02d-%02dh%02dm%02d.%03d",
                 date.tm_year+1900, date.tm_mon+1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec, (int)(tv.tv_usec/1000));
    }

    return output;
}

/**
 * Prints a log message
 * 
 * @param ctx  Context
 * @param sev  Severity
 * @param file Filename (can be NULL)
 * @param func Function name (can be NULL)
 * @param line Line# (if zero, is ignored)
 * @param fmt  Format string+ arguments (like printf)
 */
void logger_print(log_severity_t sev, char const *file,
                  char const *func, int line, char const *fmt, ...)
{
    va_list vargs;
    char    timestamp[MAX_TIMESTAMP_LEN];
    char    filefunc[MAX_OUTBUF_LEN];
    char    outbuf[MAX_OUTBUF_LEN];
    int     maxlen;
    int     offset;
    int     nchars;

    /* Validate input parameters */
    if ( (sev < 0) || (sev >= LOG_SEV_LAST) ) {
        return;
    }

    /* Determine maximum length for file+func+line and compose string */
    nchars = 0;
    maxlen = 0;
    filefunc[0] = '\0';
    if ( file != NULL ) {
        maxlen += MAX_FILE_LEN;
        nchars += snprintf(filefunc+nchars, MAX_OUTBUF_LEN-nchars, "%s ", file);
    }
    if ( func != NULL ) {
        maxlen += MAX_FUNC_LEN;
        nchars += snprintf(filefunc+nchars, MAX_OUTBUF_LEN-nchars, "%s", func);
    }
    if ( line > 0 ) {
        maxlen += MAX_LINE_LEN;
        nchars += snprintf(filefunc+nchars, MAX_OUTBUF_LEN-nchars, "(%d)", line);
    }
    #if 0
    else
    {
        /* Add the () to the function name (without line nr) */
        nchars += snprintf(filefunc+nchars, MAX_OUTBUF_LEN-nchars, "()");
    }
    #endif

    /* Determine offset to truncate string size */
    offset = 0;
    if ( nchars > maxlen ) {
        offset = nchars - maxlen;
        filefunc[offset+0] = '.';
        filefunc[offset+1] = '.';
        filefunc[offset+2] = '.';
    }

    /* Print to a string the std pattern */
    nchars = snprintf(outbuf, MAX_OUTBUF_LEN, "%s %-*s %s ", get_time(timestamp),
                      maxlen, filefunc+offset, log_sev_str[sev]);

    /* Continue printing the log itself */
    va_start(vargs, fmt);
    vsnprintf (outbuf + nchars, MAX_OUTBUF_LEN-nchars, fmt, vargs);
    va_end(vargs);

    /* Output it... */
    fprintf(stdout, "%.*s\r\n", MAX_OUTBUF_LEN, outbuf);
}

/**
 * Callback for LOGs printing
 * 
 * @author mruas (12/30/2014)
 * 
 * @param meta_data 
 * @param format 
 * @param args 
 * 
 * @return int 
 */
int hapiBroadCmPrint(bsl_meta_t *meta_data, const char *format, va_list args)
{
  int logit = 0, printit = 0;
  char buf[LOG_MSG_MAX_MSG_SIZE];
  log_severity_t ptin_log_sev = LOG_SEV_PRINT;

  if (meta_data->severity > bslenable_get(meta_data->layer, meta_data->source)) {
      return 0;
  }

  if( (meta_data == NULL) )
  {
    /* Always treat no meta data as an immediate print to the console */
    logit = 0;
    printit = 1;
  }
  else if (meta_data->layer == bslLayerAppl && meta_data->source == bslSourceShell)
  {
    /* Always print APPL layer to stdout */
    logit = 0;
    printit = 1;
  }
  else if (meta_data->severity <= bslSeverityFatal)
  {
    logit = 1;
    printit = 0;
    ptin_log_sev = LOG_SEV_FATAL;
  }
  else if (meta_data->severity <= bslSeverityError)
  {
    logit = 1;
    printit = 0;
    ptin_log_sev = LOG_SEV_ERROR;
  }
  else if (meta_data->severity <= bslSeverityWarn)
  {
    logit = 1;
    printit = 0;
    ptin_log_sev = LOG_SEV_WARNING;
  }
  else if (bsl_check(meta_data->layer, meta_data->source, meta_data->severity, meta_data->unit))
  {
    logit = 1;
    printit = 0;

    if (meta_data->severity <= bslSeverityInfo)
    {
      ptin_log_sev = LOG_SEV_INFO;
    }
    else if (meta_data->severity <= bslSeverityVerbose)
    {
      ptin_log_sev = LOG_SEV_VERBOSE;
    }
    else if (meta_data->severity <= bslSeverityDebug)
    {
      ptin_log_sev = LOG_SEV_DEBUG;
    }
    else
    {
      ptin_log_sev = LOG_SEV_TRACE;
    }
  }

  if (printit)
  {
    vprintf(format,args);
  }
  else if (logit)   /* PTin modified: Logs */
  {
    /* 
     * only allow the write to happen to either syslog or dapiTrace 
     * in order to reduce time.
     */
    int rc = 0;

    rc = vsnprintf(buf, sizeof (buf), format, args);
  
    if (rc <= 0)
    {
      return 0;  
    }
    else
    {  
      if ( rc >= sizeof(buf)) 
      {
        rc = sizeof(buf) - 1;
        /* make sure that the string is terminated */
        buf[rc] =  '\0';
      }

      /* get rid of new lines */
      if (buf[rc-1] == '\n') buf[rc-1] = '\0';

      logger_print(ptin_log_sev, meta_data->file, meta_data->func, meta_data->line, "%s", buf);
    }
  }

  return 0;
}
#endif

/*
 * Output hook for core BSL configuration
 */
STATIC int
bslmgmt_out_hook(bsl_meta_t *meta, const char *format, va_list args)
{
    int rv = 0;
    int sink_rv;
    va_list args_copy;
    bslsink_sink_t *sink = bslsink_sink_find_by_id(0);

    if (meta->severity > bslenable_get(meta->layer, meta->source)) {
        return 0;
    }

    while (sink != NULL) {
        /* Avoid consuming same arg list twice. */
        va_copy(args_copy, args);
        sink_rv = bslsink_out(sink, meta, format, args_copy);
        va_end(args_copy); 
        if (sink_rv > 0) {
            rv = sink_rv;
        }
        sink = sink->next;
    }
    return rv;
}

/*
 * Check hook for core BSL configuration
 */
STATIC int
bslmgmt_check_hook(bsl_packed_meta_t meta_pack)
{
    int layer, source, severity;

    layer = BSL_LAYER_GET(meta_pack);
    source = BSL_SOURCE_GET(meta_pack);
    severity = BSL_SEVERITY_GET(meta_pack);

    return (severity <= bslenable_get(layer, source));
}

int
bslmgmt_cleanup(void)
{
    return bslsink_cleanup();
}

int
bslmgmt_init(void)
{
    bsl_config_t bsl_config;

    bslenable_reset_all();

    bsl_config_t_init(&bsl_config);
    bsl_config.out_hook = bslmgmt_out_hook;
    //bsl_config.out_hook = hapiBroadCmPrint;   /* PTin modified */
    bsl_config.check_hook = bslmgmt_check_hook;
    bsl_init(&bsl_config);

    /* Initialize output hook */
    bslsink_init();

    /* Create console sink */
    bslcons_init();

#ifndef NO_FILEIO
    /* Create file sink */
    bslfile_init();
#endif

#ifdef BSL_TRACE_INCLUDE
    /* Create trace sink */
    bsltrace_init();
    /* Turn on debug so that trace sink can catch it */
    bslmgmt_set(bslLayerBcm, bslSourceTrace, bslSeverityDebug);
    bslmgmt_set(bslLayerSoc, bslSourceTrace, bslSeverityDebug);
#endif

    return 0;
}
