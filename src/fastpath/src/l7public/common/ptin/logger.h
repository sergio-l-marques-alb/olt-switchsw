/**
 * logger.h 
 *  
 *  logger library wrapper for swdrv
 *  
 * Notes: 
 */

#ifndef _OLTSWDRV_LOGGER_H_
#define _OLTSWDRV_LOGGER_H_
 
#include "nbtools/logger.h"
#include "nbtools/logpcap.h"
#include "nbtools/libtime.h"
 
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
    LOG_CTX_ROUTING,
    LOG_CTX_PACKET,
    LOG_CTX_CNFGR,
    LOG_CTX_MISC,             /* Used on exceptional situations */
    LOG_CTX_OAM,
    LOG_CTX_ERPS,
    LOG_CTX_DAI,
    LOG_CTX_EVENTS,
    LOG_CTX_SDK,
    LOG_CTX_STARTUP,
    LOG_CTX_OPENSAF,
    LOG_CTX_QOS,
    LOG_CTX_RATE_MON_LIB,
    LOG_CTX_EVENT_RATE_MON,
    LOG_CTX_RFC2819,
    /* Last element */
    LOG_CONTEXT_LAST,
} log_context_t;


/** 
 * Returns the default_file_id value 
 */ 
extern uint32_t swdrv_logger_get_default_fileid(void);

/** 
 * Init logger 
 */ 
extern int swdrv_logger_init(void);

/**
 * Outputs (stdout) help on how to configure logger on-the-fly
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
 * Convert severity to text
 *  
 * @param[in] sev   severity 
 *
 * @return char*  Text
 */
extern const char* swdrv_logger_sev_text(int sev);

/**
 * Convert text to severity
 *  
 * @param[in] str   text
 *
 * @return int      severity
 */
extern int swdrv_logger_text_sev(char *str);

/**
 * Convert context to text
 *  
 * @param[in] ctx   context 
 *
 * @return char*  Text
 */
extern const char* swdrv_logger_ctx_text(int ctx);

/**
 * Gets severity level for a context
 *  
 * @param[in] ctx   context 
 *  
 * @return int      severity
 */
extern int swdrv_logger_sev_get(int ctx);

/** Calls display_time_regs() from nbtools libtime */
int swdrv_display_time_regs(void);

/** Calls reset_time_regs() from nbtools libtime */
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

#endif /* _OLTSWDRV_LOGGER_H_ */

