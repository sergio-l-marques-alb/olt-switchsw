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

/*
traprcv.c

     traprcv is a program to receive traps from remote SNMP
     trap generating entities. It binds to the SNMP trap port
     (udp/162) to listen for the traps and thus must be run as
     root. It prints to standard output messages corresponding
     to the traps it has received. The primary purpose of this
     program is to demonstrate how traps are parsed using the
     SNMP library.
*/

#include "sr_conf.h"

#include <stdio.h>

#include <sys/types.h>

#include <netinet/in.h>

#include "sr_utils.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "v1_msg.h"
#include "oid_lib.h"	/* for MakeDotFromOID */

extern SnmpLcd snmp_lcd;

#ifdef SR_SNMPv3_PACKET
#include "snmpv3/v3type.h"
#include "v3_msg.h"
#endif /* SR_SNMPv3_PACKET */



#include "prnt_lib.h"
#include "diag.h"
SR_FILENAME


#define BIG_NUMBER 0x7fff

#define FROM_ADDR_TYPE struct sockaddr_in *

/* extern in snmpio.c set in InitializeTrapIO */
extern int fd;
extern int print_packet_stats;

static SR_INT32 packet_len;
static unsigned char *packet;

void  process_trap(int debug);

#ifdef SR_SNMPv2_PDU
void send_inform_reply(
    SnmpMessage *snmp_msg,
    Pdu *in_pdu_ptr,
    FROM_ADDR_TYPE from);
#endif /* SR_SNMPv2_PDU */

/* static functions */
static int get_trap_pdu(int debug);

#if defined(SR_SNMPv3_PACKET)
static int DoUtilReport(
    SnmpMessage *snmp_msg,
    FROM_ADDR_TYPE from);
#endif	/* defined(SR_SNMPv3_PACKET) */

#ifdef SR_SNMPv3_PACKET
static int DoV3UtilReport(
    SnmpMessage *snmp_msg,
    FROM_ADDR_TYPE from);
#endif /* SR_SNMPv3_PACKET */

static void output_non_trap(
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    FROM_ADDR_TYPE from);

static void output_v1_trap(
    char *descr,
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr);

#ifdef SR_SNMPv2_PDU
static void output_v2_trap(
    char *descr,
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    FROM_ADDR_TYPE from);
#endif	/* SR_SNMPv2_PDU */

static int ip_proto = SR_IPV4_PROTO;

int main(int argc, char *argv[]);

int
main(argc, argv)
    int             argc;
    char           *argv[];
{
    int             debug = FALSE;

    if (argc > 2) {
        fprintf(stderr, "usage: %s [-d]\n", argv[0]);
        return 1;
    }
    /*  malloc of packet since it's size may not be the default */
    packet = (unsigned char *)malloc(sr_pkt_size);

    if (InitializeUtility(TRAP_RCV_COMMUNICATIONS, &argc, argv,
                          "", "", "")) {
        return -1;
    }
#ifdef SR_DEBUG
    if (GetLogLevel() == APALL) {
        debug = TRUE;
    }
#endif /* SR_DEBUG */
    fprintf(stderr, "Waiting for traps.\n\n");

    /*
     * get_trap_pdu waits for traps until control-c is pressed. received
     * traps are printed, control-c causes exit
     */
    return get_trap_pdu(debug);
}                                /* main */


/* get_trap_pdu waits for traps until control-c is pressed.
 * received traps are printed, control-c causes exit
 */
static int
get_trap_pdu(debug)
    int             debug;
{

    while (1) {


            process_trap(debug);
            fflush(stdout);


    }                                /* while */

    return 1;  /* should be needed for compilation on VMS */

}


/*  initialize and open udp connection for receiving traps */



static void
output_non_trap(
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    FROM_ADDR_TYPE from)
{

    printf("Received non-trap message on snmp-trap port!!!\n");

    switch (ip_proto) {
	default:
            printf("From: %s\n", inet_ltoa(from->sin_addr.s_addr));
            break;
    }



    PrintPacket(snmp_msg->packlet->octet_ptr, snmp_msg->packlet->length);

}

#ifdef SR_SNMPv1_PACKET
static void
output_v1_trap(
    char *descr,
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr)
{
    char            *buffer = NULL;
    SR_UINT32        oidLen;

    /*
     * Allocate a buffer for the MakeDotFromOID() call.  The way we figure
     * the buffer size is this:
     *  The max number of characters in a 32-bit sub-identifier is 10,
     *      plus 1 for a dot totals 11.
     *  Multiply this by the number of subidentifiers.
     *  Add 256 to avoid problems with variables which have short OIDs but
     *      map to long names.
     */
    oidLen = 256 + (11 * pdu_ptr->u.trappdu.enterprise->length);
    if ((buffer =
         (char *) malloc((size_t)(oidLen))) == 0) {
        DPRINTF((APTRACE,
             "output_v1_trap: Could not malloc enterprise oid value buffer.\n"));
        return;
    }
    memset(buffer, 0, oidLen);

    if (MakeDotFromOID(pdu_ptr->u.trappdu.enterprise, buffer) != -1) {
        printf("Received %s Trap:\n", descr);
        /* print out the community string */
        printf("Community: ");
        if (PrintAscii(snmp_msg->u.v1.community) < 0) {
            /* cannot print as ascii, so print as octet string */
            PrintOctetString(snmp_msg->u.v1.community, 40);
        }
        printf("Enterprise: %s\n", buffer);
        printf("Agent-addr: %d.%d.%d.%d\n",
               pdu_ptr->u.trappdu.agent_addr->octet_ptr[0],
               pdu_ptr->u.trappdu.agent_addr->octet_ptr[1],
               pdu_ptr->u.trappdu.agent_addr->octet_ptr[2],
               pdu_ptr->u.trappdu.agent_addr->octet_ptr[3]);
        switch ((int) pdu_ptr->u.trappdu.generic_trap) {
        case 0:
            printf("Cold start trap.\n");
            break;
        case 1:
            printf("Warm start trap.\n");
            break;
        case 2:
            printf("Link down trap.\n");
            break;
        case 3:
            printf("Link up trap.\n");
            break;
        case 4:
            printf("Authentication failure trap.\n");
            break;
        case 5:
            printf("EGP Neighbor Loss trap.\n");
            break;
        case 6:
            printf("Enterprise Specific trap: %d\n",
                   (int)pdu_ptr->u.trappdu.specific_trap);
            break;
        default:
            printf("Unknown trap: %d\n", (int)pdu_ptr->u.trappdu.generic_trap);
            break;
        };

        printf("Time Ticks: %d\n", (int) pdu_ptr->u.trappdu.time_ticks);

        if (PrintVarBind_format == 0) {
            PrintVarBindList(pdu_ptr->var_bind_list);
        } else {
            PrintFormattedVarBindList(pdu_ptr->var_bind_list,
                                      PrintVarBind_format, 0);
        }
        if (print_packet_stats) {
            PrintResponseStats(TRAP_TYPE, pdu_ptr->var_bind_list, -1);
        }

	
    }
    else {
        fprintf(stderr, "traprcv:, pdu_ptr->u.trappdu.enterprise:\n");
    }
    free(buffer);
    buffer = NULL;
}
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2_PDU
static void
output_v2_trap(
    char *descr,
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    FROM_ADDR_TYPE from)
{
    printf("Received %s:\n", descr);

#ifdef SR_SNMPv2c_PACKET
    if (snmp_msg->version == SR_SNMPv2c_VERSION) {
        /* print out the community string */
        printf("Community: ");
        if (PrintAscii(snmp_msg->u.v1.community) < 0) {
            /* cannot print as ascii, so print as octet string */
            PrintOctetString(snmp_msg->u.v1.community, 40);
        }
    }
#endif /* SR_SNMPv2c_PACKET */

    switch (ip_proto) {
	default:
            printf("From: %s\n", inet_ltoa(from->sin_addr.s_addr));
            break;
    }



    if (PrintVarBind_format == 0) {
        PrintVarBindList(pdu_ptr->var_bind_list);
    } else {
        PrintFormattedVarBindList(pdu_ptr->var_bind_list,
                                  PrintVarBind_format, 0);
    }
    if (print_packet_stats) {
        PrintResponseStats(pdu_ptr->type, pdu_ptr->var_bind_list, -1);
    }

    if (pdu_ptr->type == INFORM_REQUEST_TYPE) {
        send_inform_reply(snmp_msg, pdu_ptr, from);
    }


}
#endif /* SR_SNMPv2_PDU */

void
process_trap(debug)
    int             debug;
{
    FNAME("process_trap")
    Pdu            *pdu_ptr = NULL;
    SnmpMessage    *snmp_msg = NULL;
    int             status;
    struct sockaddr_in from;
    int             fromlen = sizeof(from);
#ifdef SR_SNMPv2_PDU
    char            descr_string[80];
#endif	/* SR_SNMPv2_PDU */

    if (ip_proto == SR_IPV4_PROTO) {
        packet_len = recvfrom(fd, (char *)packet, sr_pkt_size, 0,
                              (struct sockaddr *)&from, &fromlen);
    }
    else {
	DPRINTF((APERROR, "Unknown ip_proto\n"));
	return;
    }



    /* possibly dump the packet */
    if (debug == TRUE) {
        PrintPacket(packet, packet_len);
    }

    snmp_msg = SrParseSnmpMessage(&snmp_lcd, NULL, NULL,
                                  packet, (int)packet_len);
    if (snmp_msg == NULL) {
        DPRINTF((APWARN, "%s: Error parsing packet\n", Fname));
        if (debug == TRUE) {
            PrintPacket(packet, packet_len);
        }
        return;
    }
    if (snmp_msg->error_code) {
        switch (snmp_msg->error_code) {
            case ASN_PARSE_ERROR:
                DPRINTF((APWARN, "%s: Error parsing packet\n", Fname));
                break;
#ifdef SR_SNMPv3_PACKET
            case SR_SNMP_UNKNOWN_SECURITY_MODELS:
                DPRINTF((APWARN, "%s: Error parsing packet, "
                                 "unknown security model\n", Fname));
                break;
            case SR_SNMP_INVALID_MSGS:
                DPRINTF((APWARN, "%s: Error parsing packet, "
                                 "invalid msg\n", Fname));
                break;
            case SR_SNMP_UNKNOWN_PDU_HANDLERS:
                DPRINTF((APWARN, "%s: Error parsing packet, "
                                 "unknown pdu handler\n", Fname));
                break;
            case SR_USM_UNSUPPORTED_SEC_LEVELS:
                DPRINTF((APWARN, "%s: Error parsing packet, "
                                 "unsupported sec level\n", Fname));
                break;
            case SR_USM_NOT_IN_TIME_WINDOWS:
                DPRINTF((APTRACE, "%s: Error parsing packet, "
                                 "not in time window\n", Fname));
                break;
            case SR_USM_UNKNOWN_USER_NAMES:
                DPRINTF((APWARN, "%s: Error parsing packet, "
                                 "unknown user name\n", Fname));
                break;
            case SR_USM_UNKNOWN_ENGINE_IDS:
                DPRINTF((APTRACE, "%s: Error parsing packet, "
                                 "unknown engine id\n", Fname));
                break;
            case SR_USM_WRONG_DIGESTS:
                DPRINTF((APWARN, "%s: Error parsing packet, "
                                 "wrong digest\n", Fname));
                break;
            case SR_USM_DECRYPTION_ERRORS:
                DPRINTF((APWARN, "%s: Error parsing packet, "
                                 "decyption error\n", Fname));
                break;
#endif /* SR_SNMPv3_PACKET */
            default:
                break;
        }
        switch (snmp_msg->error_code) {
#if defined(SR_SNMPv3_PACKET)
            case ASN_PARSE_ERROR:
#ifdef SR_SNMPv3_PACKET
            case SR_SNMP_UNKNOWN_SECURITY_MODELS:
            case SR_SNMP_INVALID_MSGS:
            case SR_SNMP_UNKNOWN_PDU_HANDLERS:
            case SR_USM_UNSUPPORTED_SEC_LEVELS:
            case SR_USM_NOT_IN_TIME_WINDOWS:
            case SR_USM_UNKNOWN_USER_NAMES:
            case SR_USM_UNKNOWN_ENGINE_IDS:
            case SR_USM_WRONG_DIGESTS:
            case SR_USM_DECRYPTION_ERRORS:
#endif /* SR_SNMPv3_PACKET */
                switch (ip_proto) {
                    default:
                        status = DoUtilReport(snmp_msg, &from);
                        break;
                }
                break;
#endif	/* defined(SR_SNMPv3_PACKET) */
            default:
                status = -1;
                break;
        }
        goto cleanup;
    }

    pdu_ptr = SrParsePdu(snmp_msg->packlet->octet_ptr,
                         snmp_msg->packlet->length);
    if (pdu_ptr == NULL) {
        fprintf(stderr, "Error parsing pdu packlet.\n");
        if (debug == TRUE) {
            PrintPacket(packet, packet_len);
        }
        goto cleanup;
    }

#define FROM_ADDR &from

#ifdef SR_SNMPv1_PACKET
    if (snmp_msg->version == SR_SNMPv1_VERSION) {
        if (pdu_ptr->type == TRAP_TYPE) {
            output_v1_trap("SNMPv1", snmp_msg, pdu_ptr);
        } else {
            switch (ip_proto) {
                default:
                    output_non_trap(snmp_msg, pdu_ptr, FROM_ADDR);
                    break;
            }
        }
        printf("\n");
        goto cleanup;
    }
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2_PDU
    switch (snmp_msg->version) {
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            strcpy(descr_string, "SNMPv2c ");
            break;
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
            strcpy(descr_string, "SNMPv3 ");
            switch (snmp_msg->u.v3.securityLevel) {
                case SR_SECURITY_LEVEL_NOAUTH:
                    strcat(descr_string, "noAuthNoPriv ");
                    break;
#ifndef SR_UNSECURABLE
                case SR_SECURITY_LEVEL_AUTH:
                    strcat(descr_string, "authNoPriv ");
                    break;
#ifndef SR_NO_PRIVACY
                case SR_SECURITY_LEVEL_PRIV:
                    strcat(descr_string, "authPriv ");
                    break;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
            }
            break;
#endif /* SR_SNMPv3_PACKET */
    }

    switch (pdu_ptr->type) {
        case SNMPv2_TRAP_TYPE:
            strcat(descr_string, "Trap");
            break;
        case INFORM_REQUEST_TYPE:
            strcat(descr_string, "Inform");
            break;
        default:
            printf("Received non-trap message on snmp-trap port!!!\n");
            goto cleanup;
    }
            
    switch (ip_proto) {
            default:
                 output_v2_trap(descr_string, snmp_msg, pdu_ptr, FROM_ADDR);
                 printf("\n");
                 break;
     }
#endif /* SR_SNMPv2_PDU */

  cleanup:
    if (pdu_ptr != NULL) {
        FreePdu(pdu_ptr);
        pdu_ptr = NULL;
    }
    if (snmp_msg != NULL) {
        SrFreeSnmpMessage(snmp_msg);
        snmp_msg = NULL;
    }
}

#ifdef SR_SNMPv2_PDU
void
send_inform_reply(
    SnmpMessage    *snmp_msg,
    Pdu            *pdu_ptr,
    FROM_ADDR_TYPE from)
{
    FNAME("send_inform_reply")
    unsigned char   outMsg[SR_DEFAULT_MAX_MSG_SIZE];
    int             outMsg_len;


   
    pdu_ptr->type = GET_RESPONSE_TYPE;
    if (BuildPdu(pdu_ptr) == -1) {
        DPRINTF((APPACKET, "%s: Failed to build PDU.\n", Fname));
        goto finished;
    }

    if (SrBuildSnmpMessage(snmp_msg, pdu_ptr, &snmp_lcd) == -1) {
        DPRINTF((APALWAYS, "%s: Failed to build SnmpMessage.\n", Fname));
        goto finished;
    }
    outMsg_len = (int) snmp_msg->packlet->length;

    memcpy((char *) outMsg, (char *) snmp_msg->packlet->octet_ptr, outMsg_len);

    switch (ip_proto) {
        default:
            if (sendto(fd, (char *) outMsg, outMsg_len, 0, 
                       (struct sockaddr *)from, sizeof(*from)) < 0) {
                DPRINTF((APWARN, 
                    "%s: Could not send reply to InformRequest\n", Fname));
                goto finished;
            }
            break;
    }



finished:
    return;
}                                /* send_inform_reply() */

#endif                                /* SR_SNMPv2_PDU */

#if defined(SR_SNMPv3_PACKET)
static int
DoUtilReport(
    SnmpMessage *snmp_msg,
    FROM_ADDR_TYPE from)
{
#ifdef SR_SNMPv3_PACKET
    if (snmp_msg->version == SR_SNMPv3_VERSION) {
        return DoV3UtilReport(snmp_msg, from);
    }
#endif /* SR_SNMPv3_PACKET */
    return -1;
}
#endif	/* defined(SR_SNMPv3_PACKET) */

#ifdef SR_SNMPv3_PACKET
extern SnmpV3Lcd util_snmp_v3_lcd;

static int
DoV3UtilReport(
    SnmpMessage *snmp_msg,
    FROM_ADDR_TYPE from)
{
    FNAME("DoV3UtilReport")
    SR_INT32 req_id = 0x7fffffff;
    OID *oid;
    SR_UINT32 value;
    VarBind *vb;
    Pdu *pdu;
    const unsigned char *wptr, *end_ptr;
    short type;
    int status = -1;

    if (!snmp_msg->u.v3.reportableFlag) {
        return -1;
    }

    
    oid = NULL;
    vb = NULL;
    pdu = NULL;

    if (snmp_msg == NULL) {
        goto cleanup;
    }

    switch (snmp_msg->error_code) {
        case ASN_PARSE_ERROR:
            oid = MakeOIDFromDot("1.3.6.1.2.1.11.6.0");
            value = 0;
            break;
        case SR_SNMP_UNKNOWN_SECURITY_MODELS:
            oid = MakeOIDFromDot("1.3.6.1.6.3.11.2.1.1.0");
            value = 0;
            break;
        case SR_SNMP_INVALID_MSGS:
            oid = MakeOIDFromDot("1.3.6.1.6.3.11.2.1.2.0");
            value = 0;
            break;
        case SR_SNMP_UNKNOWN_PDU_HANDLERS:
            oid = MakeOIDFromDot("1.3.6.1.6.3.11.2.1.3.0");
            value = 0;
            break;
        case SR_USM_UNSUPPORTED_SEC_LEVELS:
            oid = MakeOIDFromDot("1.3.6.1.6.3.15.1.1.1.0");
            value = 0;
            break;
        case SR_USM_NOT_IN_TIME_WINDOWS:
            oid = MakeOIDFromDot("1.3.6.1.6.3.15.1.1.2.0");
            value = 0;
            break;
        case SR_USM_UNKNOWN_USER_NAMES:
            oid = MakeOIDFromDot("1.3.6.1.6.3.15.1.1.3.0");
            value = 0;
            break;
        case SR_USM_UNKNOWN_ENGINE_IDS:
            oid = MakeOIDFromDot("1.3.6.1.6.3.15.1.1.4.0");
            value = 0;
            break;
        case SR_USM_WRONG_DIGESTS:
            oid = MakeOIDFromDot("1.3.6.1.6.3.15.1.1.5.0");
            value = 0;
            break;
        case SR_USM_DECRYPTION_ERRORS:
            oid = MakeOIDFromDot("1.3.6.1.6.3.15.1.1.6.0");
            value = 0;
            break;
        default:
            goto cleanup;
    }
    NULL_CHECK(oid, (APPACKET, "cannot make oid for report PDU"), cleanup);

    vb = MakeVarBindWithValue(oid, NULL, COUNTER_TYPE, (void *)&value);
    NULL_CHECK(vb, (APPACKET, "cannot make varbind for report PDU"), cleanup);

    if (snmp_msg->u.v3.contextSnmpEngineID != NULL) {
        FreeOctetString(snmp_msg->u.v3.contextSnmpEngineID);
    }
    if (snmp_msg->u.v3.contextName != NULL) {
        FreeOctetString(snmp_msg->u.v3.contextName);
    }
    snmp_msg->u.v3.contextSnmpEngineID =
        CloneOctetString(util_snmp_v3_lcd.snmpEngineData->snmpEngineID);
    if (snmp_msg->u.v3.contextSnmpEngineID == NULL) {
        DPRINTF((APPACKET, "cannot make contextSnmpEngineID\n"));
        goto cleanup;
    }
    snmp_msg->u.v3.contextName = MakeOctetString(NULL, 0);
    if (snmp_msg->u.v3.contextName == NULL) {
        DPRINTF((APPACKET, "cannot make contextName\n"));
        goto cleanup;
    }

    if (snmp_msg->packlet) {
        wptr = snmp_msg->packlet->octet_ptr;
        end_ptr = wptr + snmp_msg->packlet->length;
        if (ParseType(&wptr, end_ptr) != -1) {
            if (ParseLength(&wptr, end_ptr) != -1) {
                req_id = ParseInt(&wptr, end_ptr, &type);
                if (type == -1) {
                    req_id = 0x7fffffff;
                }
            }
        }
    }
    pdu = MakePdu(REPORT_TYPE,
                  req_id,
                  (SR_INT32)0,
                  (SR_INT32)0,
                  (OID *)NULL,
                  (OctetString *)NULL,
                  (SR_INT32)0,
                  (SR_INT32)0,
                  (SR_INT32)0);
    NULL_CHECK(pdu, (APPACKET, "cannot make report PDU"), cleanup);

    LinkVarBind(pdu, vb);
    vb = NULL;

    if (BuildPdu(pdu) == -1) {
        DPRINTF((APPACKET, "%s: Failed to build PDU.\n", Fname));
        goto cleanup;
    }

    if (SrBuildSnmpMessage(snmp_msg, pdu, &snmp_lcd) == -1) {
        DPRINTF((APPACKET, "Failed to build Report SnmpMessage"));
        goto cleanup;
    }

    switch (ip_proto) {
	default:
            if (sendto(fd,
                       (char *) snmp_msg->packlet->octet_ptr,
                       snmp_msg->packlet->length,
                       0,
                       (struct sockaddr *)from,
                       sizeof(*from)) < 0) {
                DPRINTF((APWARN, 
                    "%s: Could not send report to InformRequest\n", Fname));
                goto cleanup;
            }
            break;
    }

    status = 0;

  cleanup:
    if (oid) {
        FreeOID(oid);
    }
    if (vb) {
        FreeVarBind(vb);
    }
    if (pdu) {
        FreePdu(pdu);
    }
    return status;
}
#endif /* SR_SNMPv3_PACKET */

