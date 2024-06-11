/**
 * logger.h 
 *  
 *  logger library wrapper for swdrv
 *  
 * Notes: 
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_ /*change _LOGGER_H -> _LOGGER_H_ so we can include the library include file below*/
 
#include "nbtools/logger.h"
#include "nbtools/logpcap.h"
 
//typedef enum {
//  LOG_OUTPUT_DEFAULT=0,
//  LOG_OUTPUT_STDERR,
//  LOG_OUTOUT_STDOUT,
//  LOG_OUTPUT_FILE,
//  LOG_OUTPUT_FILE2,
//  LOG_OUTPUT_FILE3,
//  LOG_OUTPUT_MAX,
//  LOG_OUTPUT_UNINIT=0xff
//}log_output_t;

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
    LOG_CTX_OPENSAF,
    LOG_CTX_QOS,
    LOG_CTX_RATE_MON_LIB,
    LOG_CTX_EVENT_RATE_MON,
    /* Last element */
    LOG_CONTEXT_LAST,
} log_context_t;

typedef enum{
    LOG_STATIC = 0,
    LOG_DYNAMIC = 1,
} log_context_type_t;

#define LOG_CTX_DESC_MAXLEN 32
typedef struct 
{
    char        desc[LOG_CTX_DESC_MAXLEN+1]; /* description of each context */
    uint16_t    ctx_id;                      /* index of each context in the log_ctx_entries array of the logger */
    uint32_t    sev;                         /* severity of each context */
    uint8_t     type;                        /* Context type (static/dynamic) */
} swdrv_ctx_info_t;

//#define PT_LOG_PRINT(ctx , fmt , args... )

extern uint32_t swdrv_logger_get_default_fileid(void);

extern int swdrv_logger_init(void);

/**
 * Log help
 */
extern void swdrv_logger_help(void);

/**
 * Sets severity level for a group of contexts
 *
 * @param ctx_mask bitmap that defines which contexts are affected
 * (bit position corresponds to the context index)
 * @param sev severity threshold
 *
 * @return int Zero if OK, otherwise means error
 */
extern int swdrv_logger_sev_set(unsigned int ctx_mask, int sev);

/**
 * Set severity of a context using its index in the logger
 *
 * @param ctx_id index of the context affected
 * @param sev severity threshold
 *
 * @return int Zero if OK, otherwise means error
 */
extern int swdrv_logger_sev_set_index(uint16_t ctx_id, int sev);

/**
 * Reset all severity contexts to default
 * 
 * @return Zero if OK, otherwise means error
*/
extern uint32_t swdrv_logger_sev_reset(void);

/**
 * Gets contexts and corresponding severities and indexes in the logger
 *
 * @param[inout] array of struct containing context descriptions, severities and ids
 * @param[out]   num_ctxs in- max contexts, out - number of contexts retrieved
 *
 * @return
 */
extern int swdrv_logger_ctx_get(swdrv_ctx_info_t ret_ctx_sev[], uint32_t *n_ctxs);

/**
 * Convert severity to text
 *  
 * @param[in] sev   severity 
 *
 * @return char*  Text
 */
extern const char*
swdrv_logger_sev_text( int sev );

/**
 * Convert severity to text
 *  
 * @param[in] str   text
 *
 * @return int      severity
 */
extern int
swdrv_logger_text_sev( char *str );

/**
 * Convert context to text
 *  
 * @param[in] ctx   context 
 *
 * @return char*  Text
 */
extern const char*
swdrv_logger_ctx_text( int ctx );

/**
 * Gets severity level for a context
 *  
 * @param[in] ctx   context 
 *  
 * @return int      severity
 */
extern int
swdrv_logger_sev_get( int ctx );

/* Calls ptin_stats_libtime_db_dump() from ptin_stats */
int swdrv_display_time_regs(void);


/* Calls ptin_stats_libtimes_reset() from ptin_stats */
int swdrv_reset_time_regs(void);


#define PT_LOG_PEDANTIC xLOG_PEDANT
#define PT_LOG_TRACE    xLOG_TRACE
#define PT_LOG_DEBUG    xLOG_DEBUG
#define PT_LOG_INFO     xLOG_INFO
#define PT_LOG_NOTICE   xLOG_NOTICE
#define PT_LOG_WARN     xLOG_WARN
#define PT_LOG_ERR      xLOG_ERROR
#define PT_LOG_CRITIC   xLOG_CRITIC
#define PT_LOG_FATAL    xLOG_FATAL
#define PT_LOG_HEXDUMP  xLOG_HEXDUMP

#define TRACE_ENABLED(ctx) log_sev_check(ctx, LOG_SEV_TRACE)
#define DEBUG_ENABLED(ctx) log_sev_check(ctx, LOG_SEV_DEBUG)

#define logger_check    log_sev_check


/* ====================================================================================== */
/* ====================================================================================== */
/*
    +---------+
    | LOGPCAP |  
    +---------+
*/

/* Define the list of protocols starting with id 0 */
enum
{
    PROT_IGMP = 0,
    PROT_LACP,
    PROT_DSP,
    PROT_PPPOE,
    PROT_DHCPV4,
    PROT_DHCPV6,
    PROT_UDP,
    PROT_ICMPV6,
    PROT_IPSG,
    PROT_LAST
};

/**
 * Initializes logpcap lib with the protocols defined in logger.h
 * 
 * @author Andre Temprilho (19-Mar-19)
 * 
 * @return 0 - OK 
 */
int swdrv_logpcap_init(void);

/**
 * Modify logpcap protocols configurations/filters at run-time
 */
int swdrv_logpcap_prot_set(int prot_id, int port_id, uint8_t stream_mask);

void swdrv_logpcap_conf_dump(void);

void swdrv_logpcap_stream_conf_dump(int prot_id);

#define LOGPCAP_WRITE       logpcap_write


#define PTIN_PRINTF_COLOR_RESET      printf("\x1B[00m");
#define PTIN_PRINTF_RED(...)    { printf("\x1B[00;31;40m");printf(__VA_ARGS__); PTIN_PRINTF_COLOR_RESET }
#define PTIN_PRINTF_BOLD(...)   { printf("\x1B[01m");printf(__VA_ARGS__); PTIN_PRINTF_COLOR_RESET }
#define PTIN_PRINTF_YELLOW(...) { printf("\x1B[00;33;40m");printf(__VA_ARGS__); PTIN_PRINTF_COLOR_RESET }
#define PTIN_PRINTF_GREEN(...)  { printf("\x1B[00;32;40m");printf(__VA_ARGS__); PTIN_PRINTF_COLOR_RESET }
#define PTIN_PRINTF_BLUE(...)   { printf("\x1B[00;36;1m");printf(__VA_ARGS__); PTIN_PRINTF_COLOR_RESET }
#define PTIN_PRINTF_PURPLE(...) { printf("\x1B[00;35;1m");printf(__VA_ARGS__); PTIN_PRINTF_COLOR_RESET }

#define ENABLE_STR "\x1B[00;32;40mEnabled\x1B[00m"
#define DISABLE_STR "\x1B[00;31;40mDisabled\x1B[00m"


#endif /* _LOGGER_H_ */

