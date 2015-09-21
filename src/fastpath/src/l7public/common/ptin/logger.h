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

#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdio.h>

typedef enum {
  LOG_OUTPUT_DEFAULT=0,
  LOG_OUTPUT_STDERR,
  LOG_OUTOUT_STDOUT,
  LOG_OUTPUT_FILE,
  LOG_OUTPUT_FILE2,
  LOG_OUTPUT_FILE3,
  LOG_OUTPUT_MAX,
  LOG_OUTPUT_UNINIT=0xff
}log_output_t;

/* Context
 * NOTE: new items must be added to default config (log_cfg[])* 
 *       and to log_ctx_str[] !!! */
typedef enum {
    LOG_CTX_LOGGER = 0,       /* Used to log logger config functions */
    LOG_CTX_IPC,
    LOG_CTX_MSGHANDLER,
    LOG_CTX_MSG,
    LOG_CTX_INTF,
    LOG_CTX_TRUNKS,
    LOG_CTX_HAPI,
    LOG_CTX_DTL,
    LOG_CTX_L2,
    LOG_CTX_XLATE,
    LOG_CTX_API,
    LOG_CTX_EVC,
    LOG_CTX_CONTROL,
    LOG_CTX_IGMP,
    LOG_CTX_DHCP,
    LOG_CTX_PPPOE,
    LOG_CTX_PROTB,
    LOG_CTX_ROUTING,
    LOG_CTX_SSM,
    LOG_CTX_PACKET,
    LOG_CTX_CNFGR,
    LOG_CTX_MISC,             /* Used on exceptional situations */
    LOG_CTX_OAM,
    LOG_CTX_ERPS,
    LOG_CTX_IPSG,
    LOG_CTX_DAI,
    LOG_CTX_RFC2819,
    LOG_CTX_EVENTS,
    LOG_CTX_SDK,
    LOG_CTX_STARTUP,    
    /* Last element */
    LOG_CONTEXT_LAST,
} log_context_t;

/* Colors list */
typedef enum {
    LOG_COLOR_DEFAULT = 0,
    /* Normal */
    LOG_COLOR_BLACK,
    LOG_COLOR_RED,
    LOG_COLOR_GREEN,
    LOG_COLOR_YELLOW,
    LOG_COLOR_BLUE,
    LOG_COLOR_MAGENTA,
    LOG_COLOR_CYAN,
    LOG_COLOR_WHITE,
    /* Bright */
    LOG_BRIGHT_BLACK,
    LOG_BRIGHT_RED,
    LOG_BRIGHT_GREEN,
    LOG_BRIGHT_YELLOW,
    LOG_BRIGHT_BLUE,
    LOG_BRIGHT_MAGENTA,
    LOG_BRIGHT_CYAN,
    LOG_BRIGHT_WHITE,
    /* Dark */
    LOG_DARK_BLACK,
    LOG_DARK_RED,
    LOG_DARK_GREEN,
    LOG_DARK_YELLOW,
    LOG_DARK_BLUE,
    LOG_DARK_MAGENTA,
    LOG_DARK_CYAN,
    LOG_DARK_WHITE,
    /* Last element */
    LOG_COLOR_LAST,
} log_color_t;

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

/* Log configuration entry */
struct log_cfg_entry_s {
    log_context_t   context;
    log_severity_t  severity;
    log_color_t     color;
    log_output_t    output;
};

/**
 * Initialize logger
 * 
 * @param default_output : type of output
 */
extern void logger_init(log_output_t default_output);

/**
 * Deinitialize logger
 */
extern void logger_deinit(void);

/**
 * Initialize stream descriptors for each output type
 * 
 * @param output : type of output
 */
extern void logger_output_file_set(log_output_t output, char *filename);

/**
 * Redirect logger to a specific file
 *  
 * @param output : type of output
 * @param output_file_path : path and file name
 */
extern void logger_redirect(log_output_t output, char* output_file_path);

/**
 * Log help
 */
extern void logger_help(void);

/**
 * Sets severity level for a group of contexts
 * 
 * @param ctx_mask bitmap that defines which contexts are affected 
 * (bit position corresponds to the context index) 
 * @param sev severity threshold
 * 
 * @return int Zero if OK, otherwise means error
 */
extern int logger_sev_set(unsigned int ctx_mask, int sev);

/**
 * Sets a color for a group of contexts
 * 
 * @param ctx_mask bitmap that defines which contexts are affected 
 * (bit position corresponds to the context index) 
 * @param color color array index of desired color (log_color_t)
 * 
 * @return int Zero if OK, otherwise means error
 */
extern int logger_color_set(unsigned int ctx_mask, int color);


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
extern void logger_print(log_context_t ctx, log_severity_t sev, char const *file,
                         char const *func, int line, char const *fmt, ...);


#define PT_LOG_TRACE( ctx , fmt , args... ) \
        logger_print( ctx, LOG_SEV_TRACE, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PT_LOG_DEBUG( ctx , fmt , args... ) \
        logger_print( ctx, LOG_SEV_DEBUG, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PT_LOG_VERB( ctx , fmt , args... ) \
        logger_print( ctx, LOG_SEV_VERBOSE, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PT_LOG_INFO( ctx , fmt , args... ) \
        logger_print( ctx, LOG_SEV_INFO, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PT_LOG_NOTICE( ctx , fmt , args... ) \
        logger_print( ctx, LOG_SEV_NOTICE, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PT_LOG_WARN( ctx , fmt , args... ) \
        logger_print( ctx, LOG_SEV_WARNING, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PT_LOG_ERR( ctx , fmt , args... ) \
        logger_print( ctx, LOG_SEV_ERROR, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PT_LOG_CRITIC( ctx , fmt , args... ) \
        logger_print( ctx, LOG_SEV_CRITICAL, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PT_LOG_FATAL( ctx , fmt , args... ) \
        logger_print( ctx, LOG_SEV_FATAL, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
    
#define PT_LOG_PRINT( ctx , fmt , args... ) \
        logger_print( ctx, LOG_SEV_PRINT, NULL, NULL, 0, fmt, ##args )
    
#define LOGGER_PRINT( ctx , fmt , args... ) \
        PT_LOG_PRINT( ctx , fmt , args... )

#endif /* _LOGGER_H */
