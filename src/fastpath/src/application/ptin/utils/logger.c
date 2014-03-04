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

#define MAX_OUTBUF_LEN          512 /* Output buffer max length */
#define MAX_FILE_LEN            15  /* Filename max length */
#define MAX_FUNC_LEN            35  /* Function max length */
#define MAX_LINE_LEN            5   /* Line# max length */
#define MAX_FILEFUNCLINE_LEN    30  /* Filename+function+line# max length */
#define MAX_TIMESTAMP_LEN       24  /* Timestamp max length*/

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
    "SSM  ",
    "PKT  ",
    "CNFGR",
    "MISC ",
    "OAM  ",
    "ERPS ",
    "START",
};

/* Severity color */
static int log_sev_color[LOG_SEV_LAST] = {
    LOG_COLOR_DEFAULT,  /* (off) */
    LOG_COLOR_DEFAULT,  /* Always print */
    LOG_BRIGHT_RED,
    LOG_BRIGHT_RED,
    LOG_BRIGHT_RED,
    LOG_BRIGHT_YELLOW,
    LOG_COLOR_YELLOW,
    LOG_COLOR_DEFAULT,
    LOG_BRIGHT_MAGENTA,
    LOG_COLOR_DEFAULT,
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
    "\x1B[01;37m"   /* Bright White */
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
};

/* Logger default configuration
 * NOTE: it is assumed that entries are sorted by context indexes! */
static struct log_cfg_entry_s log_cfg[LOG_CONTEXT_LAST] = {
    {LOG_CTX_LOGGER,            LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_IPC,               LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_MSGHANDLER,   LOG_SEV_DEBUG,       LOG_COLOR_MAGENTA},
    {LOG_CTX_PTIN_MSG,          LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_INTF,         LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_TRUNKS,       LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_HAPI,         LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_DTL,          LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_L2,           LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_XLATE,        LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_API,          LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_EVC,          LOG_SEV_DEBUG,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_CONTROL,      LOG_SEV_INFO ,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_IGMP,         LOG_SEV_INFO ,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_DHCP,         LOG_SEV_INFO ,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_PPPOE,        LOG_SEV_INFO ,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_SSM,          LOG_SEV_INFO ,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_PACKET,       LOG_SEV_INFO ,       LOG_COLOR_DEFAULT},
    {LOG_CTX_PTIN_CNFGR,        LOG_SEV_INFO ,       LOG_COLOR_DEFAULT},
    {LOG_CTX_MISC,              LOG_SEV_INFO ,       LOG_COLOR_DEFAULT},
    {LOG_CTX_OAM,               LOG_SEV_INFO ,       LOG_COLOR_DEFAULT},
    {LOG_CTX_ERPS,              LOG_SEV_INFO ,       LOG_COLOR_DEFAULT},
    {LOG_CTX_STARTUP,           LOG_SEV_TRACE,       LOG_COLOR_DEFAULT},
};

typedef enum {
  WRITE_UNLOCK,
  WRITE_LOCK
}write_lock_t;

static struct s_outFile {
  log_output_t output;
  write_lock_t lock;
  FILE *stream;
} outFile = { LOG_OUTPUT_UNINIT, WRITE_UNLOCK, NULL };


/**
 * Outputs (stdout) help on how to configure logger on-the-fly
 */
void log_help(void) {
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
}

/**
 * Initialize logger
 * 
 * @param output : type of output
 */
void log_init(log_output_t output)
{
  if (outFile.output != LOG_OUTPUT_UNINIT)
  {
    fprintf(stderr,"log already initialized with output = %u\r\n", outFile.output );
    return;
  }

  if ( output == LOG_OUTOUT_STDOUT )
  {
    outFile.lock    = WRITE_LOCK;
    outFile.output  = LOG_OUTOUT_STDOUT;
    outFile.stream  = stdout;
    outFile.lock    = WRITE_UNLOCK;
  }
  else if ( output == LOG_OUTPUT_STDERR )
  {
    outFile.lock    = WRITE_LOCK;
    outFile.output  = LOG_OUTPUT_STDERR;
    outFile.stream  = stderr;
    outFile.lock    = WRITE_UNLOCK;
  }
  else if ( output == LOG_OUTPUT_FILE )
  {
    if ( NULL != outFile.stream)
      return;
    
    outFile.lock = WRITE_LOCK;

    outFile.stream = fopen( LOG_OUTPUT_FILE_DEFAULT , "a+");

    if (NULL == outFile.stream)
    {
      fprintf(stderr,"log NOT initialized error %d \"%s\"\r\n", errno, strerror(errno) );
    }
    else
    {
      outFile.output  = LOG_OUTPUT_FILE;
      fprintf(stdout,"log initialized at \"%s\"\r\n", LOG_OUTPUT_FILE_DEFAULT );
      //LOG_INFO(LOG_OUTPUT_FILE, LOG_CTX_LOGGER, "log initialized at \"%s\"", LOG_OUTPUT_FILE_DEFAULT );
    }

    outFile.lock = WRITE_UNLOCK;
  }
  else
  {
    outFile.output = LOG_OUTPUT_UNINIT;
    fprintf(stderr,"Invalid output identifier: %d \r\n", output );
  }
}

/**
 * Deinitialize logger
 */
void log_deinit(void)
{
  if (outFile.output == LOG_OUTPUT_UNINIT)
  {
    fprintf(stderr,"log already uninitialized!\r\n" );
    return;
  }

  outFile.lock = WRITE_LOCK;

  if ( outFile.output == LOG_OUTPUT_FILE )
  {
    fclose( outFile.stream );
  }
    
  outFile.output  = LOG_OUTPUT_UNINIT;
  outFile.stream  = (FILE *) NULL;
  outFile.lock = WRITE_UNLOCK;

  fprintf(stdout,"log uninitialized!\r\n" );
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
  char * file_name;
  
  /* Check if logger is initialized */
  if (outFile.output == LOG_OUTPUT_UNINIT)
  {
    log_init(output);
    return;
  }

  /* If output is a file */
  if ( output == LOG_OUTPUT_FILE )
  {
    file_name = (output_file_path!=NULL && output_file_path[0]!='\0') ? output_file_path : LOG_OUTPUT_FILE_DEFAULT;

    /* Firstly, try to open the new file */
    temp_stream = fopen( file_name, "a+");

    /* If error, do nothing */
    if (NULL == outFile.stream)
    {
      fprintf(stdout,"log NOT initialized error %d \"%s\"\r\n", errno, strerror(errno) );
      return;
    }

    /* Otherwise, close the previous file */
    outFile.lock = WRITE_LOCK;
    if ( outFile.output == LOG_OUTPUT_FILE )
    {
      fclose(outFile.stream);
    }
    outFile.output  = LOG_OUTPUT_FILE;
    outFile.stream  = temp_stream;
    outFile.lock    = WRITE_UNLOCK;
    fprintf(stdout,"log redirected to \"%s\"\r\n", file_name );
    LOG_INFO(LOG_OUTPUT_FILE, LOG_CTX_LOGGER, "log redirected to \"%s\"", file_name );
  }
  else
  {
    /* Unconfig previous configuration */
    outFile.lock = WRITE_LOCK;
    if ( outFile.output == LOG_OUTPUT_FILE)
    {
      fclose(outFile.stream);
    }
    outFile.output  = LOG_OUTPUT_UNINIT;
    outFile.stream  = (FILE *) NULL;
    outFile.lock = WRITE_UNLOCK;

    /* Make new configuration */
    if ( output == LOG_OUTOUT_STDOUT )
    {
      outFile.lock    = WRITE_LOCK;
      outFile.output  = LOG_OUTOUT_STDOUT;
      outFile.stream  = stdout;
      outFile.lock    = WRITE_UNLOCK;
      fprintf(stdout,"log redirected to stdout\r\n");
    }
    else if ( output == LOG_OUTPUT_STDERR )
    {
      outFile.lock    = WRITE_LOCK;
      outFile.output  = LOG_OUTPUT_STDERR;
      outFile.stream  = stderr;
      outFile.lock    = WRITE_UNLOCK;
      fprintf(stdout,"log redirected to stderr\r\n");
    }
    else
    {
      fprintf(stderr,"Invalid output identifier: %d \r\n", output );
      LOG_INFO(LOG_OUTPUT_FILE, LOG_CTX_LOGGER, "Invalid output identifier: %d", output );
    }
  }
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
            LOG_INFO(LOG_CTX_LOGGER, "%s severity level set to %s", log_ctx_str[ctx], log_sev_str[sev]);
        }
    }

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
            LOG_INFO(LOG_CTX_LOGGER, "%s color set to '%s**** COLOR ****%s'", log_ctx_str[ctx], log_colors[color], log_colors[LOG_COLOR_DEFAULT]);
        }
    }

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
    else
    {
        /* Add the () to the function name (without line nr) */
        nchars += snprintf(filefunc+nchars, MAX_OUTBUF_LEN-nchars, "()");
    }

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
    if (outFile.lock==WRITE_LOCK)  return;

    /* Print to where? */
    if ( outFile.output!=LOG_OUTPUT_UNINIT && outFile.stream!=NULL )
    {
      stream = outFile.stream;
    }

    /* Output it... */
    fprintf( stream, "%s%.*s%s\r\n", color, MAX_OUTBUF_LEN, outbuf, log_colors[LOG_COLOR_DEFAULT]);

    /* fflush for files */
    if (stream!=stdout && stream!=stderr)
    {
      fflush(stream);
    }

    return;
}

