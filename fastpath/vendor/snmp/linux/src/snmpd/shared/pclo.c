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

#include <malloc.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>

#include <netdb.h>

#include <string.h>

#include <netinet/in.h>


#include "sr_snmp.h"
#include "compat.h"
#include "diag.h"
#include "sr_trans.h"
#include "prnt_lib.h"
SR_FILENAME

#include "pclo.h"



#ifdef SR_EPIC
#include "sr_msg.h"
#include "lookup.h"
#include "v2table.h"
#ifdef SR_SNMPv3_PACKET
#include "snmpv3/v3type.h"
#include "snmpv3/v3supp.h"
#include "snmpv3/v3defs.h"
#endif /* SR_SNMPv3_PACKET */
#include "v1_msg.h"
#ifdef SR_SNMPv3_PACKET
#include "v3_msg.h"
#endif /* SR_SNMPv3_PACKET */
#include "sri/vbtable.h"
#include "sri/sr_kwmatch.h"
#include "epic.h"
#include "epiccore.h"
mode_t epic_client_access_level = 0700;
#endif /* SR_EPIC */

extern TransportInfo ipTrapTI;
char *send_port_range;


char *snmpPortBindAddr = NULL;
int bind_ip_type = BIND_DEFAULT;
#ifndef SR_SNMPv1_ADMIN
int bind_ip_proto = DEFAULT_IPV4;
#else /* SR_SNMPv1_ADMIN */
int bind_ip_proto = IPV4_ONLY;
#endif /* SR_SNMPv1_ADMIN */




SR_INT32 tcp_cache_interval = 0;
 
#define MIN_TCP_CACHE_INTERVAL 1
#define MAX_TCP_CACHE_INTERVAL 60*5*100 /* 5 minutes in centiseconds */

#ifdef SR_DEBUG
struct {
    char *name;
    SR_INT32 flag;
} LogFlags[] = {
    { "-apnone", 0 },
    { "-apwarn", APWARN },
    { "-aperror", APERROR },
    { "-aptrace", APTRACE },
    { "-apall", APTIMER|APTHREAD|APWARN|APTRACE|APERROR|APCONFIG|APPACKET|APTRAP|APACCESS|APVERBOSE },
    { "-aptimer", APTIMER },
    { "-apthread", APTHREAD },
    { "-apconfig", APCONFIG },
    { "-appacket", APPACKET },
    { "-aptrap", APTRAP },
    { "-apaccess", APACCESS },
    { "-apverbose", APVERBOSE },
    { "-apuser", APUSER },
    { "-apaudit", APAUDIT },
    { "-noapwarn", -(APWARN) },
    { "-noaperror", -(APERROR) },
    { "-noaptrace", -(APTRACE) },
    { "-noaptimer", -(APTIMER) },
    { "-noapthread", -(APTHREAD) },
    { "-noapconfig", -(APCONFIG) },
    { "-noappacket", -(APPACKET) },
    { "-noaptrap", -(APTRAP) },
    { "-noapaccess", -(APACCESS) },
    { "-noapverbose", -(APVERBOSE) },
    { "-noapuser", -(APUSER) },
    { "-noapaudit", -(APAUDIT) }
};

#define NLogFlags (sizeof(LogFlags)/sizeof(*LogFlags))
#endif /* SR_DEBUG */


void
usage(char *progname)
{
    (void) fprintf(stderr, "usage: %s [options]\n", progname);
    (void) fprintf(stderr, "\n");
    (void) fprintf(stderr, "options:\n");
    (void) fprintf(stderr, "\t-d                execute in the foreground window\n");
    (void) fprintf(stderr, "\t-trap_send_port PORT   send SNMP trap/inform messages from port PORT\n");
#ifdef SR_DEBUG
    (void) fprintf(stderr, "\t-apnone           no log messages\n");
    (void) fprintf(stderr, "\t-apwarn           log warning messages\n");
    (void) fprintf(stderr, "\t-aperror          log error messages\n");
    (void) fprintf(stderr, "\t-apconfig         log config file i/o messages\n");
    (void) fprintf(stderr, "\t-appacket         log SNMP packet build/parse messages\n");
    (void) fprintf(stderr, "\t-aptrap           log trap/inform messages\n");
    (void) fprintf(stderr, "\t-apaccess         log agent processing messages\n");
    (void) fprintf(stderr, "\t-aptimer          log timer debug messages\n");
    (void) fprintf(stderr, "\t-apthread         log thread debug messages\n");
    (void) fprintf(stderr, "\t-apverbose        log verbose debug messages\n");
    (void) fprintf(stderr, "\t-apuser           log user messages\n");
    (void) fprintf(stderr, "\t-hexdump          dump packets in hex\n");
    (void) fprintf(stderr, "\t-vbdump           dump packets as varbinds\n");
    (void) fprintf(stderr, "\t-aptrace          trace packet in application\n");
    (void) fprintf(stderr, "\t-apaudit          audit SET processing in application\n");
    (void) fprintf(stderr, "\t-apall            log all messages\n");
    (void) fprintf(stderr, "\t-log_format 0     use traditional log message format\n");
    (void) fprintf(stderr, "\t-log_format 1     use new log message format\n");
    (void) fprintf(stderr, "\t-log_stdout       allow log messages to go to standard output\n");
    (void) fprintf(stderr, "\t-log_nostdout     disallow log messages to go to standard output\n");
    (void) fprintf(stderr, "\t-log_stderr       allow log messages to go to standard error\n");
    (void) fprintf(stderr, "\t-log_nostderr     disallow log messages to go to standard error\n");
    (void) fprintf(stderr, "\t-log_file         allow log messages to go to snmpd.log\n");
    (void) fprintf(stderr, "\t-log_nofile       disallow log messages to go to snmpd.log\n");
    (void) fprintf(stderr, "\t-log_tracefile    allow trace messages to go to snmpd.log\n");
    (void) fprintf(stderr, "\t-log_notracefile  disallow trace messages to go to snmpd.log\n");
    (void) fprintf(stderr, "\t-log_closefile    close snmpd.log after each trace message.\n");
    (void) fprintf(stderr, "\t-log_append       append log messages to snmpd.log\n");
    (void) fprintf(stderr, "\t-log_noappend     discard previous contents of snmpd.log, if any\n");
#endif				/* SR_DEBUG */
#ifdef SR_EPIC
    (void) fprintf(stderr, "\t-access [u|o|g]   set epic client access level\n");
#endif /* SR_EPIC */
    (void) fprintf(stderr, "\t-large_v1_pdus    "
                           "lift 484 byte restriction for v1/v2c PDUs\n");
    (void) fprintf(stderr, "\t-help             print this usage line\n");
    (void) fprintf(stderr, "\t-pkt_size value   use supplied value as max packet size\n"); 
    (void) fprintf(stderr, "\t-snmp_bindaddr x.x.x.x   bind to SNMP port on addr x.x.x.x\n");
    (void) fprintf(stderr, "\t-bind_ip_all      bind all available IPs. do not use a wild card\n");





}

int
process_command_line_option(
    int *argc,
    char **argv,
    int *daemonize,
#ifdef SR_DEBUG
    LogAppData *logAppData,
#endif /* SR_DEBUG */
    int *s_port
    )
{
    int num_to_eat;
#ifdef SR_DEBUG
    int i;
#endif /* SR_DEBUG */    
#ifdef SR_EPIC
    unsigned int user_access = 1;
    unsigned int group_access = 0;
    unsigned int other_access = 0;
    int pos;
#endif /* SR_EPIC */
 

    if (*argc < 2) {
        return 0;
    }

#ifdef SR_DEBUG

    for (i = 0; i < NLogFlags; i++) {
        if (kw_match(argv[1], LogFlags[i].name)) {
            if (LogFlags[i].flag == 0) {
                SetLogLevel(LogFlags[i].flag);
            }
            else if (LogFlags[i].flag > 0) {
                SetLogLevel(GetLogLevel() | LogFlags[i].flag);
	    }
            else {
                SetLogLevel(GetLogLevel() & ~(-(LogFlags[i].flag)));
            }
            num_to_eat = 1;
            goto done;
        }
    }

    if (kw_match(argv[1], "-hex[dump")) {
	packdump(NULL, NULL, 0, NULL, NULL, 1);
        num_to_eat = 1;
	goto done;
    }

    if (kw_match(argv[1], "-vbdum[p")) {
        packdump(NULL, NULL, 0, NULL, NULL, 2);
        num_to_eat = 1;
        goto done;
    }



    if (kw_match(argv[1], "-log_fo[rmat")) {
	if (!isdigit((unsigned char)*argv[2])) {
	    usage(argv[0]);
	    exit(-1);
	}
	logAppData->format = (SR_INT32)(*argv[2] - '0');
	if (strlen(argv[2]) > 1 || logAppData->format > SRLOG_MAX_FORMAT) {
	    fprintf(stderr, "%s: unrecognized log format value\n", argv[2]);
	    exit(-1);
	}
	num_to_eat = 2;
	goto done;
    }
    if (kw_match(argv[1], "-log_stde[rr")) {
        if (logAppData != NULL) {
            logAppData->operFlags |= SRLOG_STDERR;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_nostde[rr")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_STDERR;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_fi[le")) {
        if (logAppData != NULL) {
            logAppData->operFlags |= SRLOG_FILE;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_nof[ile")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_FILE;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_t[racefile")) {
        if (logAppData != NULL) {
            logAppData->operFlags |= SRLOG_TRACEFILE;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_not[racefile")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_TRACEFILE;
        }
        num_to_eat = 1;
        goto done;
    }

    if (kw_match(argv[1], "-log_closef[ile")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_STDERR;
            logAppData->operFlags &= ~SRLOG_STDOUT;
            logAppData->operFlags |= SRLOG_FILE;
            logAppData->operFlags |= SRLOG_CLOSEFILE;
        }
        num_to_eat = 1;
        goto done;
    }

    if (kw_match(argv[1], "-log_a[ppend")) {
        if (logAppData != NULL) {
            logAppData->operFlags |= SRLOG_APPEND;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_noa[ppend")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_APPEND;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_stdo[ut")) {
        if (logAppData != NULL) {
            logAppData->operFlags |= SRLOG_STDOUT;
        }
        num_to_eat = 1;
        goto done;
    }
    if (kw_match(argv[1], "-log_nostdo[ut")) {
        if (logAppData != NULL) {
            logAppData->operFlags &= ~SRLOG_STDOUT;
        }
        num_to_eat = 1;
        goto done;
    }
#endif /* SR_DEBUG */


    if (kw_match(argv[1], "-pkt[_size"))  {
        if ( (*argc < 3) || (!isdigit((unsigned char)*argv[2])) ) {
           usage(argv[0]);
           exit(-1);
          }
        
        sr_pkt_size = atoi(argv[2]);
        if (sr_pkt_size < 484)  {
            fprintf(stderr, "max packet size minimum value is 484 bytes. \n");
            fprintf(stderr, "setting max packet size to 484. \n");
           sr_pkt_size = 484;
        }
        if (sr_pkt_size > 2147483647)  {
            fprintf(stderr, "max packet size maximum value is 2147483647 bytes. \n");
            fprintf(stderr, "setting max packet size to 2147483647. \n"); 
           sr_pkt_size = 2147483647;
        }
        num_to_eat = 2;
        goto done;
     }

     if (kw_match(argv[1], "-snmp_bindaddr")) {
        if (*argc < 3) {
           usage(argv[0]);
           exit(-1);
        }

        snmpPortBindAddr = strdup(argv[2]);
        num_to_eat = 2;
        goto done;
     }

     if (kw_match(argv[1], "-tcp_cache")) {
            if (tcp_cache_interval != 0) {
                fprintf (stderr, "multiple -tcp_cache values?\n");
            }
            if (*argc < 3) {
                usage(argv[0]);
                exit(-1);
            }
            tcp_cache_interval = (SR_INT32) strtol(argv[2], (char **)NULL, 10);
 
            if ((tcp_cache_interval < MIN_TCP_CACHE_INTERVAL) ||
                (tcp_cache_interval > MAX_TCP_CACHE_INTERVAL)) {
                fprintf (stderr,
                    "tcp_cache value must be >= %d and <= %d but is %d\n",
                     MIN_TCP_CACHE_INTERVAL,
                     MAX_TCP_CACHE_INTERVAL,
                     tcp_cache_interval);
                usage(argv[0]);
                exit(-1);
            }
            num_to_eat = 2;
            goto done;
    }

    if (kw_match(argv[1], "-bind_ip_all")) {
        bind_ip_type = BIND_IP_ALL;
        num_to_eat = 1;
        goto done;
    }




#ifdef SR_EPIC
    if (kw_match(argv[1], "-access")) {
        pos = 2;
        if (pos >= *argc) {
            usage(argv[0]);
        }
        if (strchr(argv[pos], 'u')) {
            user_access = 1;
        } else {
            user_access = 0;
        }
        if (strchr(argv[pos], 'g')) {
            group_access = 1;
        } else {
            group_access = 0;
        }
        if (strchr(argv[pos], 'o')) {
            other_access = 1;
        } else {
            other_access = 0;
        }

        epic_client_access_level = (0700 * user_access) +
                                   (070 * group_access) +
                                   (07 * other_access);

        num_to_eat = 2;
        goto done;
    }
#endif /* SR_EPIC */

    if (kw_match(argv[1], "-d[aemonize")
        ) {
        if (daemonize != NULL) {
            *daemonize = 0;
        }
#ifdef SR_DEBUG
        logAppData->operFlags |= SRLOG_STDERR;
#endif /* SR_DEBUG */
        num_to_eat = 1;
        goto done;
    }

#define BINDTRIES 5
    if (kw_match(argv[1], "-trap[_send_port")) {

        if (*argc < 3) {
           usage(argv[0]);
           exit(-1);
          }
        send_port_range = strdup(argv[2]);
        num_to_eat = 2;
        goto done;
    }


    if (kw_match(argv[1], "-p[ort")) {
        if (*argc < 3) {
            usage(argv[0]);
            exit(-1);
        }

        if (!isdigit((unsigned char)*argv[2])) {
            usage(argv[0]);
            exit(-1);
        }

        DPRINTF((APWARN, "%s is no longer supported, use SR_SNMP_TEST_PORT\n", argv[1]));
        num_to_eat = 2;
        goto done;
    }

    if (kw_match(argv[1], "-h[elp")) {
        usage(argv[0]);
        exit(0);
    }

    if (kw_match(argv[1], "-large_v1[_pdus")) {
	extern int use_large_v1_pdu;  /* see snmpd/shared/agtctxv[12].c */
	use_large_v1_pdu = 1;
        num_to_eat = 1;
        goto done;
    }


    return 0;

  done:
    memcpy(argv + 1, argv + num_to_eat + 1,
           sizeof(char *) * ((*argc) - num_to_eat));
    (*argc) -= num_to_eat;
    return 1;
}

void
packdump(
    char *message,
    unsigned char *packet,
    SR_UINT32 length,
    TransportInfo *ti,
    void *temp,
    int flag)
{
#ifdef SR_DEBUG
    SR_UINT32 i, pos;
    char *buffer;
    char buf[64];
    Pdu *pdu_ptr = NULL;
    VarBind *var_bind;
    SnmpMessage *snmp_msg;
    static unsigned int option = 0x00000000;

    if ((flag == 0) && (option != 0x00000000)){
        buffer = (char *)malloc(256);
        if (buffer == NULL) {
            return;
        }
        buffer[0]='\0';

        if (ti == NULL) {
            strcpy(buf, "<unknown>");
        } else {
            if (FormatTransportString(buf, sizeof(buf), ti) == NULL) {
                strcpy(buf, "<unknown>");
            }
        }
        if (message != NULL) {
            printf("\n%s, addr = %s, length = %lu:\n", message, buf,
                    (unsigned long)length);
        }

        if (option & HEXDUMP){
            strcpy(buffer, "  ");
            for (i = 0, pos = 1; i < length; i++) {
                sprintf(buf, "%2.2x ", (unsigned int)packet[i]);
                strcat(buffer, buf);
                if ((pos % 16) == 0) {
                    pos = 1;
                    printf("%s\n", buffer);
                    strcpy(buffer, "  ");
                }
                else {
                    pos++;
                }
            }
            if (pos > 1) {
                printf("%s\n", buffer);
            }
        }

        free(buffer);

        if (option & VBDUMP){
            if (!strncmp(message, "Incoming Packet", 15)) {
                snmp_msg = (SnmpMessage *)temp;
                if (snmp_msg->packlet == NULL) {
                   return;
                }
                if ((pdu_ptr = SrParsePdu(snmp_msg->packlet->octet_ptr, 
                    snmp_msg->packlet->length)) != NULL) {
	            PrintVarBindList(pdu_ptr->var_bind_list);
	         }
             }
             else {
                 var_bind = (VarBind *)temp;
	         PrintVarBindList(var_bind);
	     }
         }
    }
    else{
	if (flag == 1)
	    option = option | HEXDUMP;
        if (flag == 2)
	    option = option | VBDUMP;
    }

    if (pdu_ptr != NULL) {
        FreePdu(pdu_ptr);
        pdu_ptr=NULL;
    }
#endif /* SR_DEBUG */
}

