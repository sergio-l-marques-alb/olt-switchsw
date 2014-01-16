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

#define PTIN_MGMD_LOG_OUTPUT_FILE_DEFAULT "/var/log/mgmd.log"

#define PTIN_MGMD_LOG_OUTPUT_DEFAULT  PTIN_MGMD_LOG_OUTOUT_STDOUT

typedef enum {
  PTIN_MGMD_LOG_OUTPUT_UNINIT=0,
  PTIN_MGMD_LOG_OUTPUT_STDERR,
  PTIN_MGMD_LOG_OUTOUT_STDOUT,
  PTIN_MGMD_LOG_OUTPUT_FILE
}ptin_mgmd_log_output_t;

/* Context
 * NOTE: new items must be added to default config (log_cfg[])* 
 *       and to log_ctx_str[] !!! */
typedef enum {
    PTIN_MGMD_LOG_CTX_LOGGER = 0,       /* Used to log logger config functions */
    PTIN_MGMD_LOG_CTX_PTIN_IGMP,
    PTIN_MGMD_LOG_CTX_PTIN_TIMER,
    PTIN_MGMD_LOG_CTX_PTIN_FIFO,
    PTIN_MGMD_LOG_CTX_STARTUP,
    /* Last element */
    PTIN_MGMD_LOG_CONTEXT_LAST,
} ptin_mgmd_log_context_t;

/* Colors list */
typedef enum {
    PTIN_MGMD_LOG_COLOR_DEFAULT = 0,
    /* Normal */
    PTIN_MGMD_LOG_COLOR_BLACK,
    PTIN_MGMD_LOG_COLOR_RED,
    PTIN_MGMD_LOG_COLOR_GREEN,
    PTIN_MGMD_LOG_COLOR_YELLOW,
    PTIN_MGMD_LOG_COLOR_BLUE,
    PTIN_MGMD_LOG_COLOR_MAGENTA,
    PTIN_MGMD_LOG_COLOR_CYAN,
    LOG_COLOR_WHITE,
    /* Bright */
    PTIN_MGMD_LOG_BRIGHT_BLACK,
    PTIN_MGMD_LOG_BRIGHT_RED,
    PTIN_MGMD_LOG_BRIGHT_GREEN,
    PTIN_MGMD_LOG_BRIGHT_YELLOW,
    PTIN_MGMD_LOG_BRIGHT_BLUE,
    PTIN_MGMD_LOG_BRIGHT_MAGENTA,
    PTIN_MGMD_LOG_BRIGHT_CYAN,
    PTIN_MGMD_LOG_BRIGHT_WHITE,
    /* Last element */
    PTIN_MGMD_LOG_COLOR_LAST,
} ptiin_mgmd_log_color_t;

/* Severity */
typedef enum {
    PTIN_MGMD_LOG_OFF = 0,        /* Turn of any print */
    PTIN_MGMD_LOG_SEV_PRINT,      /* Always print */
    PTIN_MGMD_LOG_SEV_FATAL,
    PTIN_MGMD_LOG_SEV_CRITICAL,
    PTIN_MGMD_LOG_SEV_ERROR,
    PTIN_MGMD_LOG_SEV_WARNING,
    PTIN_MGMD_LOG_SEV_NOTICE,
    PTIN_MGMD_LOG_SEV_INFO,
    PTIN_MGMD_LOG_SEV_DEBUG,
    PTIN_MGMD_LOG_SEV_TRACE,
    /* Last element */
    PTIN_MGMD_LOG_SEV_LAST,
} ptin_mgmd_log_severity_t;

/* Log configuration entry */
struct ptin_mgmd_log_cfg_entry_s {
    int            context;
    ptin_mgmd_log_severity_t severity;
    int            color;
};

/**
 * Initialize logger
 * 
 * @param output : type of output
 */
extern void ptin_mgmd_log_init(ptin_mgmd_log_output_t output);

/**
 * Deinitialize logger
 */
extern void ptin_mgmd_log_deinit(void);

/**
 * Redirect logger to a specific file
 *  
 * @param output : type of output
 * @param output_file_path : path and file name
 */
extern void ptin_mgmd_log_redirect(ptin_mgmd_log_output_t output, char* output_file_path);

/**
 * Log help
 */
extern void ptin_mgmd_log_help(void);

/**
 * Sets severity level for a group of contexts
 * 
 * @param ctx_mask bitmap that defines which contexts are affected 
 * (bit position corresponds to the context index) 
 * @param sev severity threshold
 * 
 * @return int Zero if OK, otherwise means error
 */
extern int ptin_mgmd_log_sev_set(unsigned int ctx_mask, int sev);

/**
 * Sets a color for a group of contexts
 * 
 * @param ctx_mask bitmap that defines which contexts are affected 
 * (bit position corresponds to the context index) 
 * @param color color array index of desired color (log_color_t)
 * 
 * @return int Zero if OK, otherwise means error
 */
extern int ptin_mgmd_log_color_set(unsigned int ctx_mask, int color);


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
extern void ptin_mgmd_log_print(ptin_mgmd_log_context_t ctx, ptin_mgmd_log_severity_t sev, char const *file,
               char const *func, int line, char const *fmt, ...);


#define PTIN_MGMD_LOG_TRACE( ctx , fmt , args... ) \
    ptin_mgmd_log_print( ctx, PTIN_MGMD_LOG_SEV_TRACE, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PTIN_MGMD_LOG_DEBUG( ctx , fmt , args... ) \
    ptin_mgmd_log_print( ctx, PTIN_MGMD_LOG_SEV_DEBUG, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PTIN_MGMD_LOG_INFO( ctx , fmt , args... ) \
    ptin_mgmd_log_print( ctx, PTIN_MGMD_LOG_SEV_INFO, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PTIN_MGMD_LOG_NOTICE( ctx , fmt , args... ) \
    ptin_mgmd_log_print( ctx, PTIN_MGMD_LOG_SEV_NOTICE, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PTIN_MGMD_LOG_WARNING( ctx , fmt , args... ) \
    ptin_mgmd_log_print( ctx, PTIN_MGMD_LOG_SEV_WARNING, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PTIN_MGMD_LOG_ERR( ctx , fmt , args... ) \
    ptin_mgmd_log_print( ctx, PTIN_MGMD_LOG_SEV_ERROR, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PTIN_MGMD_LOG_CRITICAL( ctx , fmt , args... ) \
    ptin_mgmd_log_print( ctx, PTIN_MGMD_LOG_SEV_CRITICAL, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
#define PTIN_MGMD_LOG_FATAL( ctx , fmt , args... ) \
    ptin_mgmd_log_print( ctx, PTIN_MGMD_LOG_SEV_FATAL, NULL, __FUNCTION__, __LINE__, fmt, ##args )
    
    
#define PTIN_MGMD_LOG_PRINT( ctx , fmt , args... ) \
    ptin_mgmd_log_print( ctx, PTIN_MGMD_LOG_SEV_PRINT, NULL, NULL, 0, fmt, ##args )
    

#endif /* _LOGGER_H */
