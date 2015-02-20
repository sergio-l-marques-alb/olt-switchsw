/**
 * logger.h 
 *  
 * Provides logging features 
 *  
 * Created on: 2011/06/18 
 * Author:     Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 *  
 * Notes: 
 */

#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
//#include <stdlib.h> //Added this lib for system()

#define MAX_OUTBUF_LEN          512 /* Output buffer max length */
#define MAX_FILE_LEN            15  /* Filename max length */
#define MAX_FUNC_LEN            35  /* Function max length */
#define MAX_LINE_LEN            5   /* Line# max length */
#define MAX_FILEFUNCLINE_LEN    30  /* Filename+function+line# max length */
#define MAX_TIMESTAMP_LEN       24  /* Timestamp max length*/
#define MAX_LOG_LINES         4096  /* Max File Lines */

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
    "[DEBUG]   ",
    "[TRACE]   ",
};

/* Context strings */
static const char *log_ctx_str[LOG_CONTEXT_LAST] = {
    "LOG  ",
    "IPC  ",
    "HDLR ",
    "MSG  ",
    "INTF ",
    "TRUNK",
    "HAPI ",
    "DTL  ",
    "L2   ",
    "XLATE",
    "API  ",
    "EVC  ",
    "CTRL ",
    "IGMP ",
    "DHCP ",
    "PPPOE",
    "PROTB",
    "ROUTI",
    "SSM  ",
    "PKT  ",
    "CNFGR",
    "MISC ",
    "OAM  ",
    "ERPS ",
    "IPSG ",
    "DAI  ",
    "EVENT",
    "SDK  ",
    "START",    
};

/* Severity color */
static int log_sev_color[LOG_SEV_LAST] = {
    LOG_COLOR_DEFAULT,  /* (off) */
    LOG_COLOR_DEFAULT,  /* Always print */
    LOG_BRIGHT_RED,     /* LOG_SEV_FATAL    */
    LOG_BRIGHT_RED,     /* LOG_SEV_CRITICAL */
    LOG_BRIGHT_RED,     /* LOG_SEV_ERROR    */
    LOG_BRIGHT_YELLOW,  /* LOG_SEV_WARNING  */
    LOG_COLOR_YELLOW,   /* LOG_SEV_NOTICE   */
    LOG_BRIGHT_WHITE,   /* LOG_SEV_INFO     */
    LOG_COLOR_DEFAULT,  /* LOG_SEV_DEBUG    */
    LOG_COLOR_DEFAULT,  /* LOG_SEV_TRACE    */
};

/* Colors escape string */
static char *log_colors[LOG_COLOR_LAST] = {
    "\x1B[00m",     /* Reset */
    "\x1B[00;30m",  /* Black */
    "\x1B[00;31m",  /* Red */
    "\x1B[00;32m",  /* Green */
    "\x1B[00;33m",  /* Yellow */
    "\x1B[00;34m",  /* Blue */
    "\x1B[00;35m",  /* Magenta */
    "\x1B[00;36m",  /* Cyan */
    "\x1B[00;37m",  /* White */
    "\x1B[01;30m",  /* Bright Black */
    "\x1B[01;31m",  /* Bright Red */
    "\x1B[01;32m",  /* Bright Green */
    "\x1B[01;33m",  /* Bright Yellow */
    "\x1B[01;34m",  /* Bright Blue */
    "\x1B[01;35m",  /* Bright Magenta */
    "\x1B[01;36m",  /* Bright Cyan */
    "\x1B[01;37m",  /* Bright White */
    "\x1B[02;30m",  /* Dark Black */
    "\x1B[02;31m",  /* Dark Red */
    "\x1B[02;32m",  /* Dark Green */
    "\x1B[02;33m",  /* Dark Yellow */
    "\x1B[02;34m",  /* Dark Blue */
    "\x1B[02;35m",  /* Dark Magenta */
    "\x1B[02;36m",  /* Dark Cyan */
    "\x1B[02;37m"   /* Dark White */
};

/* Colors escape string */
static char *log_colors_str[LOG_COLOR_LAST] = {
    "Reset",
    "Black",
    "Red",
    "Green",
    "Yellow",
    "Blue",
    "Magenta",
    "Cyan",
    "White",
    "Bright Black",
    "Bright Red",
    "Bright Green",
    "Bright Yellow",
    "Bright Blue",
    "Bright Magenta",
    "Bright Cyan",
    "Bright White",
    "Dark Black",
    "Dark Red",
    "Dark Green",
    "Dark Yellow",
    "Dark Blue",
    "Dark Magenta",
    "Dark Cyan",
    "Dark White",
};

/* Logger default configuration
 * NOTE: it is assumed that entries are sorted by context indexes! */
static struct log_cfg_entry_s log_cfg[LOG_CONTEXT_LAST] = {
    {LOG_CTX_LOGGER,            LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_IPC,               LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_MSGHANDLER,   LOG_SEV_INFO ,       LOG_COLOR_MAGENTA,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_MSG,          LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_INTF,         LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_TRUNKS,       LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_HAPI,         LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_DTL,          LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_L2,           LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_XLATE,        LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_API,          LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_EVC,          LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_CONTROL,      LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_IGMP,         LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_DHCP,         LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_PPPOE,        LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_PROTB,        LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_ROUTING,      LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_SSM,          LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_PACKET,       LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_PTIN_CNFGR,        LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_MISC,              LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_OAM,               LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_ERPS,              LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_IPSG,              LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_DAI,               LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_EVENTS,            LOG_SEV_INFO ,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
    {LOG_CTX_SDK,               LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_FILE2  },
    {LOG_CTX_STARTUP,           LOG_SEV_TRACE,       LOG_COLOR_DEFAULT,     LOG_OUTPUT_DEFAULT},
};

typedef enum {
  WRITE_UNLOCK,
  WRITE_LOCK
}write_lock_t;

static struct s_outFile {
  log_output_t output;
  write_lock_t lock;
  FILE        *stream[LOG_OUTPUT_MAX];
} outFile = { LOG_OUTPUT_UNINIT, WRITE_UNLOCK };


/**
 * Outputs (stdout) help on how to configure logger on-the-fly
 */
void log_help(void)
{
    int i;

    printf("Logger help:\n"
           "\n"
           "Configuration functions\n"
           "  log_sev_set  (<ctx_mask>, <severity>)\n"
           "  log_color_set(<ctx_mask>, <color>)\n");

    printf("\nContext mask\n");
    for ( i=0; i<LOG_CONTEXT_LAST; i++ )
        printf("  0x%08X - %s\n", 1<<i, log_ctx_str[i]);

    printf("\nSeverity levels\n");
    for ( i=0; i<LOG_SEV_LAST; i++ )
        printf("  %02d - %s\n", i, log_sev_str[i]);

    printf("\nColors list\n");
    for ( i=0; i<LOG_COLOR_LAST; i++ )
        printf("  %02d - %s%s%s\n", i, log_colors[i], log_colors_str[i], log_colors[LOG_COLOR_DEFAULT]);

    fflush(stdout);
}

/**
 * Initialize logger
 * 
 * @param default_output : type of output
 */
void log_init(log_output_t default_output)
{
  log_deinit();

  outFile.lock = WRITE_LOCK;

  /* Initialize stream descriptors */
  memset(outFile.stream, 0x00, sizeof(outFile.stream));
  outFile.stream[LOG_OUTPUT_STDERR] = stderr;
  outFile.stream[LOG_OUTOUT_STDOUT] = stdout;

  outFile.output  = LOG_OUTPUT_UNINIT;
  outFile.lock    = WRITE_UNLOCK;

  fprintf(stdout,"General log initialization done!\r\n");

  /* Default output */
  if (default_output < LOG_OUTPUT_MAX)
  {
    outFile.lock    = WRITE_LOCK;
    outFile.output  = default_output;
    outFile.lock    = WRITE_UNLOCK;

    fprintf(stdout,"log initialized for default output %u\r\n", default_output);
  }
}

/**
 * Deinitialize logger
 */
void log_deinit(void)
{
  int i;

  outFile.lock = WRITE_LOCK;

  /* Close file descriptors */
  for (i = LOG_OUTPUT_FILE; i < LOG_OUTPUT_MAX; i++)
  {
    if (outFile.stream[i] != NULL)
    {
      fclose(outFile.stream[i]);
      outFile.stream[i] = NULL;
    }
  }

  outFile.output  = LOG_OUTPUT_UNINIT;
  outFile.lock = WRITE_UNLOCK;

  fprintf(stdout,"log deinitialized!\r\n" );
}

/**
 * Initialize stream descriptors for each output type
 * 
 * @param output : type of output
 */
void log_output_file_set(log_output_t output, char *filename)
{
  FILE *stream;

  /* Check if logger is initialized */
  if (outFile.output == LOG_OUTPUT_UNINIT)
  {
    fprintf(stdout,"Logger not initialized\r\n");
    return;
  }

  if (output < LOG_OUTPUT_FILE || output >= LOG_OUTPUT_MAX )
  {
    fprintf(stderr,"Invalid output %u\r\n", output );
    return;
  }

  outFile.lock = WRITE_LOCK;

  /* Close file descriptor */
  if (outFile.stream[output] != NULL)
  {
    fclose(outFile.stream[output]);
    outFile.stream[output] = NULL;
  }

  /* Open and set new file */
  if (filename != NULL && filename[0] != '\0')
  {
    stream = fopen(filename, "a+"); 

    if (NULL == stream)
    {
      fprintf(stderr,"Output %u NOT initialized: error %d \"%s\"\r\n", output, errno, strerror(errno) );
    }
    else
    {
      outFile.stream[output] = stream;
      fprintf(stdout,"Log initialized for output %u: filename=\"%s\"\r\n", output, filename );
    }
  }

  outFile.lock = WRITE_UNLOCK;
}

/**
 * Redirect logger to a specific file
 *  
 * @param output : type of output
 * @param output_file_path : path and file name
 */
void log_redirect(log_output_t output, char* output_file_path)
{
  FILE * temp_stream;
  
  /* Check if logger is initialized */
  if (outFile.output == LOG_OUTPUT_UNINIT)
  {
    fprintf(stdout,"Logger not initialized\r\n");
    return;
  }

  if (output < LOG_OUTPUT_FILE || output >= LOG_OUTPUT_MAX ||
      output_file_path == NULL || output_file_path[0] == '\0')
  {
    fprintf(stdout,"Invalid arguments\r\n");
    return;
  }

  /* Firstly, try to open the new file */
  temp_stream = fopen( output_file_path, "a+");

  /* If error, do nothing */
  if (NULL == temp_stream)
  {
    fprintf(stdout,"log NOT initialized error %d \"%s\"\r\n", errno, strerror(errno) );
    return;
  }

  /* Otherwise, close the previous file */
  outFile.lock = WRITE_LOCK;

  if ( outFile.stream[output] != NULL )
  {
    fclose(outFile.stream[output]);
    outFile.stream[output] = NULL;
  }

  /* Save new file descriptor */
  outFile.stream[output] = temp_stream;

  outFile.lock = WRITE_UNLOCK;
  fprintf(stdout,"Log redirected to \"%s\" for output %u\r\n", output_file_path, output);
}

/**
 * Sets severity level for a group of contexts
 * 
 * @param ctx_mask bitmap that defines which contexts are affected 
 * (bit position corresponds to the context index) 
 * @param sev severity threshold
 * 
 * @return int Zero if OK, otherwise means error
 */
int log_sev_set(unsigned int ctx_mask, int sev)
{
    unsigned int ctx;

    /* Validate input parameters */
    if ( (sev < 0) || (sev >= LOG_SEV_LAST) )
        return 1;

    for ( ctx = 0; ctx_mask != 0; ctx_mask >>= 1, ctx++ ) {
        if ( ctx >= LOG_CONTEXT_LAST )
            break;

        if ( ctx_mask & 1 ) {
            log_cfg[ctx].severity = sev;
            fprintf(stdout,"%s severity level set to %s\r\n", log_ctx_str[ctx], log_sev_str[sev]);
        }
    }

    fflush(stdout);

    return 0;
}

/**  
 * Sets a color for a group of contexts
 * 
 * @param ctx_mask bitmap that defines which contexts are affected 
 * (bit position corresponds to the context index) 
 * @param color color array index of desired color (log_color_t)
 * 
 * @return int Zero if OK, otherwise means error
 */
int log_color_set(unsigned int ctx_mask, int color)
{
    unsigned int ctx;

    /* Validate input parameters */
    if ( (color < 0) || (color >= LOG_COLOR_LAST) )
        return 1;

    for ( ctx = 0; ctx_mask != 0; ctx_mask >>= 1, ctx++ ) {
        if ( ctx >= LOG_CONTEXT_LAST )
            break;

        if ( ctx_mask & 1 ) {
            log_cfg[ctx].color = color;
            fprintf(stdout,"%s color set to '%s**** COLOR ****%s'\r\n", log_ctx_str[ctx], log_colors[color], log_colors[LOG_COLOR_DEFAULT]);
        }
    }

    fflush(stdout);

    return 0;
}

/**
 * Sets output for a particular list of contexts
 * 
 * @param ctx_mask bitmap that defines which contexts are affected 
 * (bit position corresponds to the context index) 
 * @param sev severity threshold
 * 
 * @return int Zero if OK, otherwise means error
 */
int log_output_set(unsigned int ctx_mask, int output)
{
    unsigned int ctx;

    /* Validate input parameters */
    if ( (output < 0) || (output >= LOG_OUTPUT_MAX) )
        return 1;

    for ( ctx = 0; ctx_mask != 0; ctx_mask >>= 1, ctx++ ) {
        if ( ctx >= LOG_CONTEXT_LAST )
            break;

        if ( ctx_mask & 1 ) {
            log_cfg[ctx].output = output;
            fprintf(stdout,"%s: output set to %u\r\n", log_ctx_str[ctx], output);
        }
    }

    fflush(stdout);

    return 0;
}


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

static unsigned int max_log_lines = 0;
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
void log_print(log_context_t ctx, log_severity_t sev, char const *file,
               char const *func, int line, char const *fmt, ...)
{
    va_list vargs;
    char    timestamp[MAX_TIMESTAMP_LEN];
    char    filefunc[MAX_OUTBUF_LEN];
    char    outbuf[MAX_OUTBUF_LEN];
    int     maxlen;
    int     offset;
    int     nchars;
    char   *color;
    FILE   *stream = stdout;

    /* Validate input parameters */
    if ( (ctx < 0) || (ctx >= LOG_CONTEXT_LAST) ||
         (sev < 0) || (sev >= LOG_SEV_LAST) ) {
        return;
    }

    /* For the requested context, check if log severity allows print */
    if ( sev > log_cfg[ctx].severity )
        return;

    /* Get color: give priority to context color (only if != default)*/
    if ( log_cfg[ctx].color == LOG_COLOR_DEFAULT )
        color = log_colors[log_sev_color[sev]];
    else
        color = log_colors[log_cfg[ctx].color];

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
    nchars = snprintf(outbuf, MAX_OUTBUF_LEN, "%s %s %-*s %s ", get_time(timestamp),
                      log_ctx_str[ctx], maxlen, filefunc+offset, log_sev_str[sev]);

    /* Continue printing the log itself */
    va_start(vargs, fmt);
    vsnprintf (outbuf + nchars, MAX_OUTBUF_LEN-nchars, fmt, vargs);
    va_end(vargs);

    /* Can we print? */
    if (outFile.lock == WRITE_LOCK)  return;

    /* Use stream inside log_cfg structure (only if not default) */
    if ((log_cfg[ctx].output > LOG_OUTPUT_DEFAULT && log_cfg[ctx].output < LOG_OUTPUT_MAX) &&
        outFile.stream[log_cfg[ctx].output] != NULL)
    {
      stream = outFile.stream[log_cfg[ctx].output];
    }
    /* For default stream, use generic defined at log_init */
    else if ((outFile.output >= LOG_OUTPUT_DEFAULT && outFile.output < LOG_OUTPUT_MAX) &&
              outFile.stream[outFile.output] != NULL) 
    {
      stream = outFile.stream[outFile.output];
    }

    /* Output it... */
    fprintf( stream, "%s%.*s%s\r\n", color, MAX_OUTBUF_LEN, outbuf, log_colors[LOG_COLOR_DEFAULT]);

    /* fflush for files */
    if (stream!=stdout && stream!=stderr)
    {
      if(++max_log_lines >= MAX_LOG_LINES)
      {
        /*To be replaced with semaphore  or signal approach*/
        //system("/usr/local/ptin/sbin/logrotate /etc/logrotate.conf.d&"); 
                
        max_log_lines = 0;        
      }
      fflush(stream);
    }

    return;
}

