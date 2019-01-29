/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */


#include "sr_conf.h"



#include <stdio.h>


#include <ctype.h>

#include <errno.h>

#include <signal.h>

#include <malloc.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>


#include <netdb.h>


#include <sys/socket.h>

#include <sys/stat.h>

#include <sys/fcntl.h>

#include <fcntl.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <sys/file.h>


#include <string.h>








#include "sr_time.h"

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_snmp.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "sr_fdset.h"
#include "compat.h"
#ifdef DAEMONIZE
#include "sr_dmn.h"
#endif /* DAEMONIZE */
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "mapctx.h"
#include "method.h"
#include "mibout.h"
#include "snmpd.h"
#include "mgmt.h"
#include "tmq.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "trap.h"
#include "sr_ntfy.h"
#include "snmpv2d.h"
#include "agtinit.h"
#include "fnames.h"
#include "geninit.h"
#if (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE))
#include "notify.h"
#endif	/* (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE)) */
#include "agtctx.h"
#if (defined(SR_EPIC) || defined(SR_EMWEB_EPIC))
#include "sri/vbtable.h"
#include "epic.h"
#include "epiccore.h"
#endif	/* (defined(SR_EPIC) || defined(SR_EMWEB_EPIC)) */


#include "diag.h"
SR_FILENAME

#include "sitedefs.h"





#include "mib_tabl.h"
#include "snmp-mib.h"

/* LVL7 */
#include "snmp_api.h"
#include "snmpapi.h"
#include "snmp_util_api.h"

#include "l7_common.h"
#include "osapi_support.h"

extern int k_private_initialize(void);
/* LVL7 end */

#ifdef SR_CONFIG_FP
const PARSER_RECORD_TYPE *snmpd_rt[66];
int snmpd_rt_count;
#endif /* SR_CONFIG_FP */

#ifdef SR_CONFIG_FP
#define DEFAULT_SNMP_ENGINE_ID_SRC 1
#ifdef SR_SNMPv3_PACKET
ENGINE_ID engine_id;
#endif /* SR_SNMPv3_PACKET */
#endif /* SR_CONFIG_FP */



void usage(char *progname);


int is_broadcast
    SR_PROTOTYPE((SR_UINT32 ipSrc));

int PostTimerHook
    SR_PROTOTYPE((TimerQueue *TmQ,
                  SR_UINT32 id,
                  void *info,
                  SR_INT32 invoke));

int
is_broadcast(ipSrc)
    SR_UINT32   ipSrc;
{
    /* %%% needs further work */
    if (((ipSrc & 0xff) == 0xff) || ((ipSrc & 0xff) == 0)) {
        return 1;
    }
    return 0;
}

int CancelTimerHook
    SR_PROTOTYPE((TimerQueue *TmQ,
                  SR_UINT32 id,
                  int anymore ));

extern char *snmpPortBindAddr;
extern int bind_ip_type;
extern int bind_ip_proto;

#define PROXY_TRAP_BIND_ARG

#ifdef SR_DEBUG
/* LogBook *LogBk = NULL; LVL7 */
LogAppData logAppData = { SRLOG_FILE };
#define LOG_APP_DATA_ARG LogAppData *logAppData,
#else /* SR_DEBUG */
#define LOG_APP_DATA_ARG
#endif /* SR_DEBUG */

int process_command_line_option
    SR_PROTOTYPE((int *argc,
                  char **argv,
                  int *daemonize,
                  PROXY_TRAP_BIND_ARG
                  LOG_APP_DATA_ARG
                  int *s_port));

/* globals */




#ifdef SR_DEBUG
void MultiFunctionLogMessage
    SR_PROTOTYPE((LogEntry *log));
#endif /* SR_DEBUG */

/* POLL_INTERFACES and POLL_INTERVAL set in include/sr_conf.h */
#ifdef POLL_INTERFACES
void poll_interfaces(void);
time_t time(time_t *);
#endif				/* POLL_INTERFACES */

TRANS_INFO *snmpd_tInfo= NULL;

/* LVL7 */
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
/* for linux, open a single transport. v4 connections show as
   v4 mapped v6 connections. this is because linux cant open
   both v4 and v6 to inaddr_any.
*/
TransportInfo   snmpdTI6;
#else
TransportInfo   snmpdTI;
#endif
/* LVL7 */


TimerQueue *TimerQ;
extern int timerq_update;

time_t          time(time_t *);

RETSIGTYPE
quit(void)
{
    exit(1);
}

#ifdef TIMER_EXAMPLE
void 
test_time(SR_UINT32 TimeOutType, int TimerId, 
          SR_INT32 userData1, void *userData2)
{
    printf("test_timer\n");
    printf("TimeOutType: %u, TimerId: %d, userData1: %d, userData2: %s\n",
	     TimeOutType, TimerId, userData1, (char *)userData2);
}

void
hi(TimeOutDescriptor *none)
{
    printf("hi there!\n");
}
#endif /* TIMER_EXAMPLE */


#ifdef SR_DEBUG
#ifdef DAEMONIZE
extern int stderr_closed;
extern int logfile_open;
#else /* DAEMONIZE */
int stderr_closed = 0;
int logfile_open = 0;
#endif /* DAEMONIZE */
#endif /* SR_DEBUG */


int writeConfigFileFlag = FALSE;

SnmpLcd agt_snmp_lcd;


int
/* LVL7 */
snmpd_main(int argc, char **argv, char **envp)
/* LVL7 end */
{
    unsigned char   *in_packet, *out_packet;
    TransportInfo   *SrcTransportInfo = NULL, DstTransportInfo;
#ifdef SR_DEBUG
    char tstr[TSTR_SIZE];
#endif /* SR_DEBUG */


#ifdef USE_NETWORK_SERVICES
    struct servent *SimpleServ = NULL;
#endif				/* USE_NETWORK_SERVICES */
    int             in_packet_len, out_packet_len;
    int             s_port;
    int             s_trap_port;
#ifdef SR_SNMPv1_ADMIN
    OID *oid_temp;
#endif /* SR_SNMPv1_ADMIN */

    int   daemonize = 0;   /* daemonize by default */
    struct timeval  timeout_data, *timeout;
    SR_UINT32       curtime, howlong, bind_addr;
    fd_set          readfds;
    int             cc;
    int             maxfd = 0;
#ifdef SR_DEBUG
    SR_INT32        tmpLogLevel;
    int             fd_tmp;
    char           *fullpath = NULL;
#endif /* SR_DEBUG */

#ifndef WANT_SETUPENVIRON
    /* Copies of argc and argv which can be modified 
       locally without confusing ps */
    int argcount;
    char **argvec;  
#endif /* WANT_SETUPENVIRON */

    /* timer checking related variables */
    int wrap = 0;
    TimerEvent *p = NULL;

/*    int check = 0; LVL7 */
    int ret;
    TRANS_INFO *tmpTI = NULL;
    int success = 0;
#ifndef SR_SNMPv1_ADMIN
#endif /* SR_SNMPv1_ADMIN */

#ifdef POLL_INTERFACES
    time_t polltime = time((time_t *) NULL);
#endif				/* POLL_INTERFACES */


/* LVL7 start */
    (void)snmpSemaTake();
/* LVL7 end */

    timeout = &timeout_data;
    timeout->tv_usec = 0;
    timeout->tv_sec = POLL_INTERVAL;
    FD_ZERO(&readfds);

#ifdef OLD_CODE /* LVL7 */
#ifdef SR_DEBUG
/*
 * OpenLog() must be called before ANY function which invokes
 * DPRINTF or LOG!!
 */
    if((LogBk = OpenLog()) == NULL) {
	printf("%s: OpenLog() returned NULL\n", argv[0]);
	exit(1);
    }
#endif /* SR_DEBUG */
#endif /* OLD_CODE LVL7 */

    SetSigFunction(SIGINT, (SigFunc) quit);
#ifdef SR_EPIC
    SetSigFunction(SIGPIPE, (SigFunc) SIG_IGN);
#endif /* SR_EPIC */


#ifdef SR_DEBUG
    SetLogLevel(APERROR | APWARN);
    SetProgname(argv[0]);
    SetLogFunction(MultiFunctionLogMessage);
    SetLogUserData((void *) &logAppData);
#endif /* SR_DEBUG */

#ifdef WANT_SETUPENVIRON
#define ARGC argc
#define ARGV argv
#else /* WANT_SETUPENVIRON */
    argvec = malloc((argc+1)*SIZEOF_CHAR_P);
    if (!argvec) {
	DPRINTF((APERROR, "Out of memory.\n"));
	exit(1);
    };
    memcpy(argvec, argv, (argc+1)*SIZEOF_CHAR_P);
    argcount = argc;
#define ARGC argcount
#define ARGV argvec
#endif /* WANT_SETUPENVIRON */




    InitTimeNow();

    while (process_command_line_option(&ARGC,
				       ARGV,
                                       &daemonize,
#ifdef SR_DEBUG
                                       &logAppData,
#endif /* SR_DEBUG */
                                       &s_port));


    if (ARGC >= 2) {
	/* not all of the command line arguments were processed */
	usage(ARGV[0]);
	exit(1);
    }

/* LVL7 */
/*
    printf(STARTUP_BANNER_1);
    printf(STARTUP_BANNER_2);
*/
/* LVL7 end */

    /* malloc packet buffers, sr_pkt_size might not be default */
    in_packet = (unsigned char *)malloc(sr_pkt_size);
    out_packet = (unsigned char *)malloc(sr_pkt_size);


#ifdef SR_DEBUG
    if (logfile_open) {
	/* some previous code has reopened stderr as a file */
	logAppData.operFlags &= ~SRLOG_STDERR;
	logAppData.operFlags |= SRLOG_FILE;
	logAppData.logfile_fp = stderr;
    }
    else {
	if (logAppData.operFlags & SRLOG_FILE) /* user request? */ {
	    if (fullpath == NULL) {
		fullpath = GetLogFullPath("snmpd.log");
	    }
	    if (fullpath == NULL) {
		DPRINTF((APERROR, "Could not open log file\n"));
		logAppData.operFlags &= ~SRLOG_FILE;
	    }
	    else {
		if (logAppData.operFlags & SRLOG_APPEND) {
		    logAppData.logfile_fp = fopen(fullpath, "a");
		}
		else {
		    (void) unlink(fullpath);
		    fd_tmp = open(fullpath, O_WRONLY | O_CREAT, 0444);
		    if (fd_tmp == -1) {
			logAppData.operFlags &= ~SRLOG_FILE;
			logAppData.logfile_fp = NULL;
		    }
		    else {
			close(fd_tmp);
			logAppData.logfile_fp = fopen(fullpath, "w");
		    }
		}
		if (logAppData.logfile_fp == NULL) {
	   	    DPRINTF((APERROR, "Could not open log file %s\n",
			fullpath));
	   	    logAppData.operFlags &= ~SRLOG_FILE;
		}
		else {
                    tmpLogLevel = logAppData.operFlags;
                    logAppData.operFlags &= ~SRLOG_STDOUT;
                    logAppData.operFlags &= ~SRLOG_STDERR;
                    SetLogUserData((void *) &logAppData);
		    DPRINTF((APWARN, STARTUP_BANNER_1));
	   	    DPRINTF((APWARN, STARTUP_BANNER_2));
                    logAppData.operFlags = tmpLogLevel;
                    SetLogUserData((void *) &logAppData);
	  	    DPRINTF((APTRACE, "Successfully opened log file %s\n",
			fullpath));

                    if ((logAppData.operFlags & SRLOG_FILE) &&
                        (logAppData.operFlags & SRLOG_CLOSEFILE)) {
                           if( logAppData.logfile_fp != NULL ) {
                              fclose(logAppData.logfile_fp);
                              logAppData.logfile_fp = stderr;
                        }
                    }
		free(fullpath);
	      }
	}
      }
    }

    /*
     * The SRLOG_FILE bit is overloaded.  Before this point, it is set
     * if by default or by user request a file should be opened.  After
     * this point, bit set means that a file has been opened successfully.
     */

    if ((GetLogLevel() & ~(APERROR | APWARN)) &&
	(!(logAppData.operFlags & SRLOG_TRACEFILE))) {
	/*
	 * log messages other than APERROR and APWARN are enabled
	 * and the override switch is not turned on to force those
	 * messages into the log file, so don't daemonize
	 */
        if(!(logAppData.operFlags & SRLOG_CLOSEFILE)){
            logAppData.operFlags |= SRLOG_STDERR;  /* implied */
  	    if (logAppData.operFlags & SRLOG_FILE) {
	        DPRINTF((APWARN|APERROR,
		"Only APERROR and APWARN messages are being printed to "
		"the log file (override with -log_tracefile)\n"));
	    }

#ifdef DAEMONIZE
	    DPRINTF((APTRACE,
	     "Can't daemonize: log level above APERROR or APWARN enabled\n"));
	    daemonize = 0;
#endif /* DAEMONIZE */
       }
    }

#endif	/* SR_DEBUG */
    mib_oid_table = orig_mib_oid_table;

#ifdef SR_CONFIG_FP
    init_fnames();
#ifdef SR_GENERATE_CONFIGURATION
    GenerateAgentDefaults(fn_snmpd_file);
#endif /* SR_GENERATE_CONFIGURATION */
#endif /* SR_CONFIG_FP */


#ifdef DAEMONIZE
#ifdef SR_DEBUG
    if ((logAppData.operFlags & (SRLOG_STDOUT | SRLOG_STDERR)) != 0) {
	/*
	 * don't daemonize: a standard stream has been explicitly
	 * requested with -log_stderr or -log_stdout, or a standard stream
	 * has been implicitly requested with -d
	 */
	daemonize = 0;
    }
#endif				/* SR_DEBUG */

    if (daemonize) {
#ifdef SR_DEBUG
	if (!(logAppData.operFlags & SRLOG_FILE)) {
	    printf("Launching daemon with no log file\n");
	}
#endif /* SR_DEBUG */
	sr_daemon(0, 0, NULL);
    }
#endif				/* DAEMONIZE */

#ifdef SR_DEBUG
    if (stderr_closed) {
	logAppData.operFlags &= ~SRLOG_STDERR;
    }

    /*
     * The SRLOG_STDERR bit is overloaded.  Before this point, it is set
     * if by default or by user request the standard stream should be used.
     * After this point, bit set means that the standard stream is open.
     */
#endif	/* SR_DEBUG */

    /* get snmp and trap port values */
    s_port = GetSNMPPort();
    s_trap_port = GetSNMPTrapPort();


#ifdef WANT_SETUPENVIRON
    /* SetUpEnviron() scribbles on argv, so call it after all arg processing */
    /* It puts the port number in argv[0] so it will show up in ps */
    SetUpEnviron(argc, argv, envp);
#endif /* WANT_SETUPENVIRON */


    if((TimerQ = InitTimerQueue(NULL)) == NULL) {
	exit(1);
    }

/*    initialize(&agt_snmp_lcd);*/

#ifndef SR_SNMPv1_ADMIN
#endif /* SR_SNMPv1_ADMIN */

#define BINDTRIES 5

    /* set up network connection */
#ifdef OLD_CODE /* LVL7 */
    /*
     * if bind_ip_type == BIND_DEFAULT -> bind wildcard.
     * if bind_ip_type == BIND_IP_ALL  -> bind all individual IPaddresses.
     * GetTransportInfo will return both IPv6/IPv4 transport info if system
     * support IPv6.
     */
    ret = GetTransportInfo(bind_ip_type, (void **)&snmpd_tInfo,
                           snmpPortBindAddr, NULL);
    if ((snmpd_tInfo == NULL) || (ret <= 0)) {
        DPRINTF((APERROR, "Cannot get TransportInfo\n"));
        exit(1);
    }
    DPRINTF((APTRACE, "Opening transport to receive SNMP messages\n"));
    /*
     * if this host support both IPV4 and IPV6, open and bind a transport
     * based on the bind_ip_proto.
     *        DEFAULT_IPV4   0 : same as bind ipv4 transport only
     *        IPV4_ONLY      1 : bind ipv4 transport only
     *        IPV6_ONLY      2 : bind ipv6 transport only
     *        IPV4_IPV6      3 : bind both ippv4/ippv6 transport
     */
    for (tmpTI = snmpd_tInfo; tmpTI != NULL; tmpTI = tmpTI->next) {
        if ((bind_ip_proto == IPV4_ONLY) &&
                               (tmpTI->tInfo.type != SR_IP_TRANSPORT)) {
            continue;
        }
        if (OpenTransport(&tmpTI->tInfo)) {
            DPRINTF((APERROR, "Cannot open SNMP transport\n"));
            success = -1;
            break;
        }
        else {
            if (BindTransport(&tmpTI->tInfo, BINDTRIES)) {
                DPRINTF((APERROR, "Cannot bind SNMP transport %s\n",
                    FormatTransportString(tstr, sizeof(tstr), &tmpTI->tInfo)));
                DPRINTF((APERROR, "    is another agent running?\n"));
                success = -1;
                break;
            }
        }
    }
    if (success == -1) {
        /*
         * failed to bind a network interface or one of network interfaces.
         * just exit
         */
        for (tmpTI = snmpd_tInfo; tmpTI != NULL; tmpTI = tmpTI->next) {
            CloseTransport(&tmpTI->tInfo);
        }
        exit(1);
    }
#else /* LVL7 */
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    /* v4 uses v6 socket and contains v4 mapped address */
    memset(&snmpdTI6, 0, sizeof(TransportInfo));
    snmpdTI6.type = SR_IP6_TRANSPORT;
    snmpdTI6.t_ip6Port = osapiHtons((short) s_port );

    if (OpenTransport(&snmpdTI6)) {
        DPRINTF((APERROR, "Cannot open SNMP transport\n"));
	exit(1);
    }

    if (BindTransport(&snmpdTI6, BINDTRIES)) {
        DPRINTF((APERROR, "Cannot bind SNMP transport %s\n",
                FormatTransportString(tstr, sizeof(tstr), &snmpdTI6)));
        DPRINTF((APERROR, "    is another agent running?\n"));
	exit(1);
    }
#else

    /* set up network connection */
    memset(&snmpdTI, 0, sizeof(TransportInfo));
    snmpdTI.type = SR_IP_TRANSPORT;
    snmpdTI.t_ipPort = osapiHtons((short) s_port);

    if (snmpPortBindAddr != NULL) {
        bind_addr = inet_addr(snmpPortBindAddr);
	if (bind_addr == -1) {
	   DPRINTF((APERROR, "Cannot resolve snmp_bindaddr %s\n", snmpPortBindAddr));
	} else {
	   snmpdTI.t_ipAddr = bind_addr;
	   DPRINTF((APTRACE, "SNMP port bind restricted to address %s\n", 
                   snmpPortBindAddr));
        }
    }

    if (OpenTransport(&snmpdTI)) {
        DPRINTF((APERROR, "Cannot open SNMP transport\n"));
	exit(1);
    }

    if (BindTransport(&snmpdTI, BINDTRIES)) {
        DPRINTF((APERROR, "Cannot bind SNMP transport %s\n",
                FormatTransportString(tstr, sizeof(tstr), &snmpdTI)));
        DPRINTF((APERROR, "    is another agent running?\n"));
	exit(1);
    }

#endif
#endif /* OLD_CODE LVL7 */
#if defined(SR_SNMPv3_ADMIN)
#endif	/* defined(SR_SNMPv3_ADMIN) */


    /* This initializes all of MIB-2 except for the system and snmp
     * groups.  The k_initialize() function is exported from k_$(os).c */
    if (k_initialize() == -1) {
        DPRINTF((APWARN, "k_initialize failed.  Continuing anyway.\n"));
    }

    /* LVL7 */
    snmp_initialize(&agt_snmp_lcd);
    /* LVL7 end */


#ifdef SR_CONFIG_FP
    snmpd_rt_count = 0;
    system_rt(snmpd_rt, &snmpd_rt_count);
    engine_rt(snmpd_rt, &snmpd_rt_count);
#ifndef SR_SNMPv1_ADMIN
#endif /* SR_SNMPv1_ADMIN */
    snmpd_rt[snmpd_rt_count++] = NULL;

#ifdef SR_SNMPv3_PACKET
    engine_id.runtime_SNMP_ENGINE_ID_SRC = DEFAULT_SNMP_ENGINE_ID_SRC;
    engine_id.runtime_SNMP_ENGINE_ID = NULL;
#endif /* SR_SNMPv3_PACKET */
    Configure(snmpd_rt);

    /* LVL7 */
    (void)snmpSemaGive();
    SnmpWaitUntilState(L7_SNMP_STATE_RUN);
    (void)snmpSemaTake();
    /* LVL7 end */

#ifdef SR_SNMPv3_PACKET
    updateEngineID(&engine_id);
#endif /* SR_SNMPv3_PACKET */
#endif /* SR_CONFIG_FP */

#if (defined(SR_EPIC) || defined(SR_EMWEB_EPIC))
    if (EpicSetup() < 0) {
        DPRINTF((APERROR, "Cannot bind EPIC (EMANATE Protocol Interface Component) transport.\n"));
    }
#endif	/* (defined(SR_EPIC) || defined(SR_EMWEB_EPIC)) */


#ifdef SR_CONFIG_FP
    /* LVL7 */
    /*
    AgentWriteConfigFile(fn_snmpd_file, snmpd_rt);
    */
    /* LVL7 end */
#endif /* SR_CONFIG_FP */

    /* SrInitializeTraps() is called the first time a trap is sent */
    SendNotificationsSMIv2Params(coldStartTrap, NULL, NULL);

    /* daemonize used to be here, see above */
#ifdef TIMER_EXAMPLE
    {
	TimeOutDescriptor *tdp, tdp_data;

	tdp = &tdp_data;
	tdp->CallBack = hi;
	SetPeriodicTimeout(GetTimeNow(), 1500, (void *)tdp);
        SrSetPeriodicTimeout(GetTimeNow(), 1000, 0, 
                             (void *)test_time, 1, "test!!");
    }
#endif /* TIMER_EXAMPLE */

#ifdef SR_SNMPv1_ADMIN
    /* Force MakeOIDFromDot to be linked */
    oid_temp = MakeOIDFromDot("0.0");
#endif /* SR_SNMPv1_ADMIN */



/* LVL7 */
    (void)snmpSemaGive();
/* LVL7 end */

    /* now the main loop... */
    /* Beyond this point use DPRINTF(()) for all message */
    while (1) {
	do {
            FD_ZERO(&readfds);
	    CheckTimers(TimerQ);

#ifdef POLL_INTERFACES
	    if ((time((time_t *) NULL) - polltime) >= POLL_INTERVAL) {
		poll_interfaces();
		polltime = time((time_t *) NULL);
	    }
#endif				/* POLL_INTERFACES */

#ifdef OLD_CODE /* LVL7 */
        /* SR_FD_SET on multiple IPs */
        for (tmpTI = snmpd_tInfo; tmpTI != NULL; tmpTI = tmpTI->next) {
            if ((bind_ip_proto == IPV4_ONLY) &&
                                  (tmpTI->tInfo.type != SR_IP_TRANSPORT)) {
                continue;
            }
            if (tmpTI->tInfo.fd > 0) {
                SR_FD_SET(tmpTI->tInfo.fd, &readfds);
            }
            if (tmpTI->tInfo.fd > maxfd) {
                maxfd = tmpTI->tInfo.fd;
            }
        }
#else /* OLD_CODE LVL7 */
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
	    SR_FD_SET(snmpdTI6.fd, &readfds);
            maxfd = snmpdTI6.fd;
#else
	    SR_FD_SET(snmpdTI.fd, &readfds);
            maxfd = snmpdTI.fd;
#endif
#endif /* OLD_CODE LVL7 */


#if defined(SR_SNMPv3_ADMIN)
#endif	/* defined(SR_SNMPv3_ADMIN) */

#if (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE))
        maxfd = SrSetInformFds(&readfds, maxfd);
#endif	/* (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE)) */

#if (defined(SR_EPIC) || defined(SR_EMWEB_EPIC))
        maxfd = SetEpicFds(&readfds, maxfd);
#endif	/* (defined(SR_EPIC) || defined(SR_EMWEB_EPIC)) */

            
	    if (TimeQEmpty(TimerQ)) {
		timeout = NULL;	/* no timers pending, block indefinitely */
	    }
	    else {
                /*
                 * detect a clock wrap, and adjust a timer value in 
                 * the timer queue.
                 */
                curtime = GetTimeNowWrap(&wrap);
                if ((wrap) || (timerq_update == 1)) {
                    for (p = TimerQ->head->next; 
                                         p != TimerQ->tail; p = p->next) {
                        if (p->type == CURRENT) {
                            p->when = 0;
                        }
                        else {
                            p->type = CURRENT;
                        }
                    }
                    timerq_update = 0;
                }
                p = TimerQ->head->next;
                if (p->type == WRAP) {
                    howlong = 
                        ((SR_UINT32)MAX_WRAP - curtime) + p->when;
                }
                else {
                    if (p->when <= curtime) {
                        /* Timer should already have expired; poll. */
                        howlong = 0;
                    }
                    else {
                        howlong = p->when - curtime;
                    }
                }
		timeout = &timeout_data;
		timeout->tv_sec = howlong / 100;
		timeout->tv_usec = (howlong % 100) * 10000;
		if (timeout->tv_sec > SR_SELECT_MAX_TIMEOUT_SEC) {
		    timeout->tv_sec = SR_SELECT_MAX_TIMEOUT_SEC;
		}
		if (timeout->tv_usec > SR_SELECT_MAX_TIMEOUT_USEC) {
		    timeout->tv_usec = SR_SELECT_MAX_TIMEOUT_USEC;
		}
	    }


	} while ((cc = select(maxfd+1,
                              (SELECT_BITMASK_TYPE)&readfds,
                              (SELECT_BITMASK_TYPE)NULL,
                              (SELECT_BITMASK_TYPE)NULL,
			      timeout)) == 0);



	if (cc == -1) {
	    DPRINTF((APWARN, "select returned -1 on snmp socket\n"));
            continue;
	}


#if defined(SR_SNMPv3_ADMIN)
#endif	/* defined(SR_SNMPv3_ADMIN) */

#if (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE))
        cc = SrCheckInformFds(&readfds, cc);
        if (cc == 0) {
            continue;
        }
#endif	/* (defined(SR_SNMPv3_ADMIN) && defined(SR_NOTIFY_FULL_COMPLIANCE)) */

#ifdef OLD_CODE /* LVL7 */
            /* FD_ISSET on multiple IPs */
            for (tmpTI = snmpd_tInfo; tmpTI != NULL; tmpTI = tmpTI->next) {
                if ((bind_ip_proto == IPV4_ONLY) &&
                                  (tmpTI->tInfo.type != SR_IP_TRANSPORT)) {
                    continue;
                }
                if (FD_ISSET(tmpTI->tInfo.fd, &readfds)) {
                    SrcTransportInfo = &tmpTI->tInfo;
                    break;
                }
            }
#else /* OLD_CODE LVL7 */
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
	if (FD_ISSET(snmpdTI6.fd, &readfds)) /* snmp socket */
        {	
            SrcTransportInfo = &snmpdTI6;
	}
#else
	if (FD_ISSET(snmpdTI.fd, &readfds)) /* snmp socket */
        {	
            SrcTransportInfo = &snmpdTI;
	}
#endif
#endif /* OLD_CODE LVL7 */


#if (defined(SR_EPIC) || defined(SR_EMWEB_EPIC))
/*
 *  If any EPIC file descriptor is ready, take all due action,
 *  including completing the EPIC operation, if warranted.
 */
        if (CheckEpicFds(&readfds, 0)) {
            continue;
        }
#endif	/* (defined(SR_EPIC) || defined(SR_EMWEB_EPIC)) */

#ifdef OLD_CODE /* LVL7 */
        check = 0;
        for (tmpTI = snmpd_tInfo; tmpTI != NULL; tmpTI = tmpTI->next) {
            if ((bind_ip_proto == IPV4_ONLY) &&
                                  (tmpTI->tInfo.type != SR_IP_TRANSPORT)) {
                continue;
            }
            if (FD_ISSET(tmpTI->tInfo.fd, &readfds)) {
                    check = 1;
                    break;
            }
        }
        if (check) /* snmp socket */
#else /* OLD_CODE LVL7 */
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
        if (FD_ISSET(snmpdTI6.fd, &readfds)) 
#else
        if (FD_ISSET(snmpdTI.fd, &readfds)) /* snmp socket */
#endif
#endif /* OLD_CODE LVL7 */
        {

  /* LVL7 */
	in_packet_len = ReceiveFromTransport((char *)in_packet,
                                             sr_pkt_size,
                                             SrcTransportInfo, &DstTransportInfo);
  /* LVL7 end */

	if (in_packet_len < 0) {
	    DPRINTF((APWARN, "Received Zero-Length packet from %s\n",
                     FormatTransportString(tstr, sizeof(tstr),
                                           SrcTransportInfo)));

	    /*
	     * Delete this comment, but don't put fromlen=sizeof(from) here.
	     * See comment in revision history above.  -DLB
	     */

	    continue;
	}
/* LVL7
        DstTransportInfo.type = SrcTransportInfo->type;
        check = 0;
        switch (DstTransportInfo.type) {
	    case SR_IP_TRANSPORT:
                DstTransportInfo.t_ipAddr = (SR_UINT32)0;
                DstTransportInfo.t_ipPort = (unsigned short)0;
                check = 1;
                break;
            default:
                DPRINTF((APWARN, "Unknwon transport src type\n"));
                break;
        }
        if (!check) {
            continue;
        }
*/
/* LVL7 */
#if !defined(L7_IPV6_PACKAGE) && !defined(L7_IPV6_MGMT_PACKAGE)
	if (is_broadcast(SrcTransportInfo->t_ipAddr)) {
	    continue;		/* Don't reply to the Broadcast Address */
	}
#endif
/* LVL7 end */
	DPRINTF((APPACKET, "in_packet_len = %d\n", in_packet_len));
	DPRINTF((APPACKET, "\nPacket from: %s\n",
                 FormatTransportString(tstr, sizeof(tstr), SrcTransportInfo)));
	DPRINTF((APPACKET, " to: %s\n",
                 FormatTransportString(tstr, sizeof(tstr), &DstTransportInfo)));


/* LVL */
  (void)snmpSemaTake();
/* LVL7 end */
	out_packet_len = (int) SrDoMgmt(&agt_snmp_lcd,
                                        in_packet,
				        (unsigned short) in_packet_len,
				        SrcTransportInfo,
				        &DstTransportInfo,
				        out_packet,
				        sr_pkt_size);
/* LVL */
  (void)snmpSemaGive();
/* LVL7 end */


	/*
	 * Send the response.
	 */

	if (out_packet_len > 0) {
    /* LVL7 */
            if (SendToTransport((char *)out_packet,
                                out_packet_len, SrcTransportInfo, &DstTransportInfo) < 0) {
    /* LVL7 end */
		DPRINTF((APWARN, "Sendto failed.\n"));	/* %%% need errno */
	    }
	    else {
		snmpData.snmpOutPkts++;

#ifdef SR_CONFIG_FP
                if (writeConfigFileFlag == TRUE) {
                    DPRINTF((APCONFIG, "WRITING CONFIG FILE\n"));
                    AgentWriteConfigFile(fn_snmpd_file, snmpd_rt);
                    writeConfigFileFlag = FALSE;
                }
#endif /* SR_CONFIG_FP */

	    }
	}
    }

    }				/* while (1) */

    /* return 0;  never reached */
}				/* main */

#ifdef SR_DEBUG
void
MultiFunctionLogMessage(log)
    LogEntry *log;
{
    const LogAppData        *data;
    void                    *temp;
    static char logname[] = "snmpd.log";
    static const LogAppData  defaultdata = {
	SRLOG_STDERR,
	0,
	NULL
    };

    if (log) {

	/*
	 * UserData in the LogEntry must be overwritten to pass
	 * the structure to other log functions.  Restore the
	 * old value from temp before returning from this function.
	 */
	temp = log->UserData;


	data = (LogAppData *) log->UserData;
	if (data == NULL) {
	    data = &defaultdata;
	}

        /* --------------- Standard I/O --------------- */
        if (data->operFlags & SRLOG_STDERR) {
            log->UserData = (void *) stderr;
            if (data->format == 0) {
                PrintShortLogMessage(log);
            }
            if (data->format == 1) {
                PrintLogMessage(log);
            }
        }
        if (data->operFlags & SRLOG_STDOUT) {
            log->UserData = (void *) stdout;
            if (data->format == 0) {
                PrintShortLogMessage(log);
            }
            if (data->format == 1) {
                PrintLogMessage(log);
            }
        }

        /* --------------- File I/O --------------- */

        if ((data->operFlags & SRLOG_FILE) &&
            (data->operFlags & SRLOG_CLOSEFILE)) {
            /* only use the close logfile if the process is daemonized */
               log->UserData = (char *)logname;
               PrintLogMessageToCloseFile(log);
               log->UserData = temp;
               return;
        }

        if (data->operFlags & SRLOG_FILE) {
	    if (log->LogLevel & ~(APERROR | APWARN)) {
	        /* this log message is neither APERROR nor APWARN */
	        if (!(data->operFlags & SRLOG_TRACEFILE) ||
                    !(data->operFlags & SRLOG_CLOSEFILE)) {
	            /* the -log_tracefile or -log_closefile flags 
                     *  have not been specified */
		    /* do not print this message to a file */
		    log->UserData = temp;
	            return;
	        }
	    }

            log->UserData = (void *) data->logfile_fp;

            if (data->format == 0) {
                PrintShortLogMessage(log);
            }
            if (data->format == 1) {
                PrintLogMessage(log);
            }
        }

	/* restore the old value from temp */
	log->UserData = temp;
    }
}


#endif				/* SR_DEBUG */




TimerQueue *
GetTimerQueue()
{
    return TimerQ;
}
#ifdef OLD_CODE /* LVL7 */
#ifdef SR_DEBUG
LogBook *
GetLogBook()
{
    return LogBk;
}
#endif /* SR_DEBUG */
#endif /* OLD_CODE LVL7 */

int
PostTimerHook(TimerQueue *TmQ, 
              SR_UINT32   id,
              void       *info,
              SR_INT32    invoke)
{
    TimeOutDescriptor *tdp = NULL;
    /* we are in the monolithic agent.  just call the callback from here */
    if (info != NULL) {
        tdp = (TimeOutDescriptor *) info;
        if (invoke == DONOT_CALL_CALLBACK) {
            (*(SrTodCallbackFunction)tdp->CallBack)(tdp->TimeOutType, 
                                                    tdp->TimerId, 
                                                    tdp->UserData1, 
                                                    tdp->UserData2);
            free(tdp);
            tdp = NULL;
        }
        else {
            (*tdp->CallBack)(tdp);
        }
    }
    return 1;
}

int
CancelTimerHook(TmQ, id, anymore)
     TimerQueue *TmQ;
     SR_UINT32 id;
     int anymore;
{
  return 1;	/* just for the compiler */
}

int
GetEmanateVersion()
{
#define MAX_SUPPORTED_EMANATE_VERSION 3
    return MAX_SUPPORTED_EMANATE_VERSION;
}

/* LVL7 */

/* Hook to allow external functions to call Configure with the snmpd_rt strutcure) */
void
SnmpLoadConfiguration()
{
  (void)snmpSemaTake();
  terminate();
  snmp_initialize(&agt_snmp_lcd);
  
#ifdef SR_CONFIG_FP
  snmpd_rt_count = 0;
  system_rt(snmpd_rt, &snmpd_rt_count);
  engine_rt(snmpd_rt, &snmpd_rt_count);
  snmpd_rt[snmpd_rt_count++] = NULL;
  Configure(snmpd_rt);
  (void)snmpSemaGive();
#endif /* SR_CONFIG_FP */
}

void
SnmpSaveConfiguration()
{
#ifndef SR_USE_HARD_IO
  (void)snmpSemaTake();
  if (writeConfigFileFlag == TRUE) {
      DPRINTF((APCONFIG, "WRITING CONFIG FILE\n"));
      AgentWriteConfigFile(fn_snmpd_file, snmpd_rt);
      writeConfigFileFlag = FALSE;
  }
  (void)snmpSemaGive();
#endif /* SR_USE_HARD_IO */
}

/*********************************************************************
*
* @purpose  Starts SNMP task function
*
* @notes    none 
*
* @end
*********************************************************************/
void 
L7_snmp_task ()
{
  static char *argv[] = {"snmpd", "-d", "-apall"};
  
  snmpd_main(1, argv, L7_NULLPTR);
}

/* LVL7 end */

