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

#include "ptin/logger.h"
#include <stdio.h>
#include <string.h>
#include "ptin_globaldefs.h"

//DELETE /* Context strings */
//DELETE static const char *log_ctx_str[LOG_CONTEXT_LAST] = {
//DELETE     "LOG  ",
//DELETE     "IPC  ",
//DELETE     "HDLR ",
//DELETE     "MSG  ",
//DELETE     "INTF ",
//DELETE     "TRUNK",
//DELETE     "HAPI ",
//DELETE     "DTL  ",
//DELETE     "L2   ",
//DELETE     "XLATE",
//DELETE     "API  ",
//DELETE     "EVC  ",
//DELETE     "CTRL ",
//DELETE     "IGMP ",
//DELETE     "DHCP ",
//DELETE     "PPPOE",
//DELETE     "PROTB",
//DELETE     "ROUTI",
//DELETE     "SSM  ",
//DELETE     "PKT  ",
//DELETE     "CNFGR",
//DELETE     "MISC ",
//DELETE     "OAM  ",
//DELETE     "ERPS ",
//DELETE     "IPSG ",
//DELETE     "DAI  ",
//DELETE     "R2819",
//DELETE     "EVENT",
//DELETE     "SDK  ",
//DELETE     "START",
//DELETE     "OPENSAF",
//DELETE     "QOS"
//DELETE };



/* Example of contexts structure initialization
   IMPORTANT: the last parameter, file_id, MUST be updated upon execution of
   log_file_open() function, which returns the correct file_id */
log_ctx_entry_t contexts[LOG_CONTEXT_LAST] =
{
    {"LOGGER"     , "LOG"     , LOG_SEV_DEBUG, LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"IPC"        , "IPC"     , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"MSGHANDLER" , "HDLR"    , LOG_SEV_INFO , LOG_COLOR_MAGENTA, LOG_MODE_FILE, 0},
    {"MSG"        , "MSG"     , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"INTF"       , "INTF"    , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"TRUNKS"     , "TRUNK"   , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"HAPI"       , "HAPI"    , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"DTL"        , "DTL"     , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"L2"         , "L2"      , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"XLATE"      , "XLATE"   , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"API"        , "API"     , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"EVC"        , "EVC"     , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"CONTROL"    , "CTRL"    , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"IGMP"       , "IGMP"    , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"DHCP"       , "DHCP"    , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"PPPOE"      , "PPPOE"   , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"PROTB"      , "PROTB"   , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"ROUTING"    , "ROUTI"   , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"SSM"        , "SSM"     , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"PACKET"     , "PKT"     , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"CNFGR"      , "CNFGR"   , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"MISC"       , "MISC"    , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"OAM"        , "OAM"     , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"ERPS"       , "ERPS"    , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"IPSG"       , "IPSG"    , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"DAI"        , "DAI"     , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"RFC2819"    , "R2819"   , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"EVENTS"     , "EVENT"   , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"SDK"        , "SDK"     , LOG_SEV_DEBUG, LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"STARTUP"    , "START"   , LOG_SEV_TRACE, LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"OPENSAF"    , "OPENSA"  , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"QOS"        , "QOS"     , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"RATEMONLIB" , "RMNLIB"  , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
    {"RATEMON_SD" , "RTMON"   , LOG_SEV_INFO , LOG_COLOR_DEFAULT, LOG_MODE_FILE, 0},
};


/**
 * Outputs (stdout) help on how to configure logger on-the-fly
 */
void swdrv_logger_help(void)
{
    int i;

    printf("\nContext mask\n");
    for ( i=0; i<LOG_CONTEXT_LAST; i++ )
        printf("  0x%08X - %s\n", 1<<i, contexts[i].description);

    fflush(stdout);

    logger_conf_dump();
}

static uint32_t default_file_id=-1;
uint32_t swdrv_logger_get_default_fileid(void)
{
    return default_file_id;
}


int swdrv_logger_init(void)
{
    int           i;
    log_ret_t     ret;
    uint32_t      /*file_id,*/ file_id2, file_id3;
    //log_config_t  config = { "App Teste", 10000, 0xC0A80101, 2222 };//unchanged default (from "example.c" file)
    static log_config_t  config = { "swdrv", 10000, 0x7F000001, 2222 };
    log_file_conf_t fc;


    /* Initialize */
    ret = logger_init(&config);
    if (ret != 0)
    {
        printf("Error: logger_init(&config) ret=%d\n", ret);
        return 1;
    }

    /* Open/create the log files */
    i=sizeof(fc.filename)-1;    
    fc.filename[i]      =0;
    strncpy(fc.filename, LOG_OUTPUT_FILE_DEFAULT, i);
    fc.max_size         =1UL<<21;
    fc.nr_files         =5;
    fc.use_compression  =1;
    if (0 != logger_file_open(&fc, &default_file_id)) {
        printf("Error: logger_file_open(%s, 1200, &default_file_id) ret=%d\n", LOG_OUTPUT_FILE_DEFAULT, ret);
        return 2;
    }

    strncpy(fc.filename, LOG_OUTPUT_FILE_DEFAULT2, i);
    if (0 != logger_file_open(&fc, &file_id2)) {
        printf("Error: logger_file_open(%s, 1200, &file_id) ret=%d\n", LOG_OUTPUT_FILE_DEFAULT2, ret);
        return 3;
    }

    strncpy(fc.filename, LOG_OUTPUT_FILE_DEFAULT3, i);
    if (0 != logger_file_open(&fc, &file_id3)) {
        printf("Error: logger_file_open(%s, 1200, &file_id) ret=%d\n", LOG_OUTPUT_FILE_DEFAULT3, ret);
        return 4;
    }

    /* Update the contexts structure with the correct file_id */
    for (i=0; i<LOG_CONTEXT_LAST ; i++) {
        //strncpy(contexts[i].description, log_ctx_str[i], LOG_CTX_DESC_MAXLEN);
        //strncpy(contexts[i].tag, log_ctx_str[i], LOG_CTX_TAG_MAXLEN); contexts[i].tag[LOG_CTX_TAG_MAXLEN]=0;
        contexts[i].file_id     = default_file_id;
    }
    contexts[LOG_CTX_EVENTS].file_id =  file_id3;

// Move SDK events to the main Swdrv log file (temporatily)
//    contexts[LOG_CTX_SDK].file_id =     file_id2;


    /* Configure the logger contexts */
    ret = logger_ctx_config(contexts, LOG_CONTEXT_LAST);    //n_ctx);
    if (ret != 0)
    {
        printf("Error: logger_ctx_config(contexts, n_ctx) ret=%d\n", ret);
        return 5;
    }

    logger_conf_dump();

    xLOG_NOTICE(LOG_CTX_STARTUP, "Logger initialized!");

    return 0;
}//swdrv_logger_init


/**
 * Sets severity level for a group of contexts
 *
 * @param ctx_mask bitmap that defines which contexts are affected
 * (bit position corresponds to the context index)
 * @param sev severity threshold
 *
 * @return int Zero if OK, otherwise means error
 */
L7_int32 swdrv_logger_sev_set(unsigned int ctx_mask, int sev)
{
    int ctx;

    printf("Running %s function...\r\n", __FUNCTION__);
    PT_LOG_NOTICE(LOG_CTX_LOGGER,"Going to change severity attributes: ctx_mask=0x%X sev=%u",
                  ctx_mask, sev);

    for (ctx = 0; ctx < LOG_CONTEXT_LAST; ctx++)
    {
        if ((ctx_mask >> ctx) & 1)
        {
            (void) log_sev_set(ctx, sev);
            printf("Log severity of %s context modified to %u\r\n", contexts[ctx].tag, sev);
            PT_LOG_NOTICE(LOG_CTX_LOGGER,"Log severity of %s context modified to %u", contexts[ctx].tag, sev);
        }
    }

    printf("Done!\r\n");
    PT_LOG_NOTICE(LOG_CTX_LOGGER,"Done!");

    return 0;
}

L7_int32 swdrv_logger_sev_set_(unsigned int ctx, int sev)
{
    (void) log_sev_set(ctx, sev);
    return 0;
}

/**
 * Convert severity to text
 *  
 * @param[in] sev   severity 
 *
 * @return char*  Text
 */
const char*
swdrv_logger_sev_text( int sev )
{
  char      *str;
  log_ret_t  rv;

  rv = log_sev_enum2str( sev, &str );
  if ( rv != LOG_OK ) {
    return "unknown";
  }
  return str;
}

/**
 * Convert severity to text
 *  
 * @param[in] str   text
 *
 * @return int      severity
 */
int
swdrv_logger_text_sev( char *str )
{
  int sev;

  for ( sev = 0 ; sev < LOG_SEV_LAST ; sev++ ) {
    if ( strcmp( str , swdrv_logger_sev_text(sev) ) == 0 ) {
      return sev;
    }
  }
  return -1;
}

/**
 * Convert context to text
 *  
 * @param[in] ctx   context 
 *
 * @return char*  Text
 */
const char*
swdrv_logger_ctx_text( int ctx )
{
  if ( ( ctx < 0 ) || ( ctx >= LOG_CONTEXT_LAST ) ) {
    return "unknown";
  }
  return (const char*)contexts[ctx].tag;
}

/**
 * Gets severity level for a context
 *  
 * @param[in] ctx   context 
 *  
 * @return int      severity
 */
int
swdrv_logger_sev_get( int ctx )
{
  log_ret_t       rv;
  log_severity_t  sev;

  rv = log_sev_get( ctx , &sev );
  if ( rv != LOG_OK ) {
    return 0;
  }
  return (int)sev;
}

#if 0
/* ====================================================================================== */
/* ====================================================================================== */
/*
    +---------+
    | LOGPCAP |  
    +---------+
*/

/* List of protocols to be initialized */
logpcap_prot_entry_t protocols[PROT_LAST] =
{
    {"IGMP",    { 0 }, 0, LOGPCAP_REG_MODE_RAW,  {{0}, {0}, 0x0000} },
    {"LACP",    { 0 }, 0, LOGPCAP_REG_MODE_RAW,  {{0}, {0}, 0x0000} },
    {"DSP",     { 0 }, 0, LOGPCAP_REG_MODE_RAW,  {{0}, {0}, 0x0000} }
};

/* Number of protocols */
int n_prot = sizeof(protocols) / sizeof(protocols[0]);

/* File index assigned by logpcap upon logpcap_file_open() call */
uint32_t file_id;       /* Update logpcap_prot_entry_t 3rd parameter with this value */



/**
 * Initializes logpcap lib with the protocols defined in logger.h
 * 
 * @author Andre Temprilho (19-Mar-19)
 * 
 * @return 0 - OK 
 */
int swdrv_logpcap_init(void)
{
    logpcap_ret_t       ret;
    int i;

    logpcap_config_t configLogPcap = {"SWDRV", 10000, 0xC0A80101, 2222};

    logpcap_file_conf_t file_conf =     {LOGPCAP_OUTPUT_FILE_DEFAULT,
                                        (1UL<<21),                             /* file max size */
                                        8,                                     /* nr of files (rotations) */
                                        1,                                     /* 0 - no compression; 1 - gzip compression */
                                        65535,                                 /* Snaplenght */
                                        LOGPCAP_PCAP_HDR_NETWORK_ETHERNET};    /* Ethernet */

    /* logpcap init */
    ret = logpcap_init(&configLogPcap);
    if (ret != LOGPCAP_OK)
    {
        printf("Error: logpcap_init(&configLogPcap) ret=%d\n", ret);
        return 1;
    }

    /* Open/create the logpcap log files */
    ret = logpcap_file_open(&file_conf, &file_id);
    if (ret != LOGPCAP_OK)
    {
        printf("Error: logpcap_file_open(\"%s\", %u, &file_id) ret=%d\n",
               file_conf.filename,
               file_conf.max_size,
               ret);
        return 1;
    }

    /* Update the protocols structure with the correct file_id */
    protocols[PROT_IGMP].file_id    = file_id;
    protocols[PROT_LACP].file_id    = file_id;
    protocols[PROT_DSP].file_id     = file_id;
    /* Update for other protocols as well (as new ones are created) */

    /* Enable all packet streams */
    for (i = 0; i < n_prot; i++)
    {
        memset(&(protocols[i].port[0]), LOGPCAP_STREAM_OFF, 256);  /* 256 interfaces to use */
    }

    /* Configure the logpcap protocols (in a static way) */
    ret = logpcap_prot_config(protocols, n_prot);
    if (ret != LOGPCAP_OK)
    {
        printf("Error: logpcap_prot_config(protocols, n_prot) ret=%d\n", ret);
        return 1;
    }

    /* Dump configurations */
    logpcap_conf_dump();
    logpcap_stream_conf_dump(-1); /* View all protocols */

    return 0;
}
/**
 * Modify logpcap protocols configurations/filters at run-time
 */
int swdrv_logpcap_prot_set(int prot_id, int port_id, uint8_t stream_mask)
{
    return logpcap_prot_set(prot_id, port_id, stream_mask);
}

void swdrv_logpcap_conf_dump(void)
{
    logpcap_conf_dump();
}

void swdrv_logpcap_stream_conf_dump(int prot_id)
{
    logpcap_stream_conf_dump(prot_id);
}
#endif
