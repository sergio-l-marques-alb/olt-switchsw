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
 * inform.c - a program that sends out an inform-request until an ACK is
 * received or a maximum number of retries is reached.
 */

/*
inform.c

        An InformRequest PDU is a new type of PDU introduced with SNMPv2c. It
is used by a SNMPv2c/SNMPv3 entity that wishes to notify another application (a
SNMPv2c/SNMPv3 entity also acting in a manager role) of information in the MIB 
view of a USM username or community local to the sending application.  This 
utility shows the user how to send InformRequest PDUs.  The arguments are the 
entity's address, the SNMPv2c community string or SNMPv3 USM username
, snmpEventID.i (where i identifies the instance in the snmpEventEntry
table), the value associated with snmpEventID.i, which is an object identifier
in dot notation or as they appear in the MIB document, and the
number of retries for this InformRequest PDU.   When the
snmpAlarm, snmpAlarmEntry, snmpEvent, snmpEventEntry and snmpEventNotifyEntry
tables are fully implemented, the value for snmpEventID.i will be available
from a table.

For instance, running the following:

inform -v3 localhost Guest snmpEventID.2 snmpRisingAlarm 4

or 

inform -v2c localhost public snmpEventID.2 snmpRisingAlarm 4

would send a message containing the following varbinds(name and value):
      sysUpTime.0, snmpEventID.2,

The utility will then wait for the receiver to send back an acknowlegement
(ACK).  After waiting a certain amount of time, the PDU will be sent again and
again, until either an ACK is received or the maximum number of retries is
reached (in this case 4).
*/

#define SR_TRAP
#include "sr_utils.h"
#include "diag.h"
SR_FILENAME
#include "prnt_lib.h"

extern int print_packet_stats;


static VarBind *initial_vb_list;
static int      return_value = 0;

int main
    SR_PROTOTYPE((int argc,
                  char *argv[]));

int
main(argc, argv)
    int             argc;
    char           *argv[];
{
    OID            *oid_ptr = NULL;
    VarBind        *vb_ptr = NULL;
    VarBind        *vb_end = NULL;
    Pdu            *req_pdu_ptr = NULL, *resp_pdu_ptr = NULL;
    int             i;
    int             status;
    OID            *snmpEvent_val;
    OID            *falarm;
    OID            *ralarm;
    OID            *oalarm;
    SR_UINT32       now;
    char           *usage_string;

    snmpEvent_val = (OID *) NULL;
    falarm = (OID *) NULL;
    ralarm = (OID *) NULL;
    oalarm = (OID *) NULL;

#ifndef SR_SNMPv2_PDU

    usage_string = "usage: %s: Cannot use SNMPv1 for GETBULK requests.\n";

#else /* SR_SNMPv2_PDU */

    usage_string =
        "usage:  %s "
#ifdef SR_SNMPv2c_PACKET
        "[-v2c] "
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        "[-v3] "
#endif /* SR_SNMPv3_PACKET */
#if defined(SR_SNMPv3_PACKET)
        "\\\n\t[-ctxid contextID] [-ctx contextName] "
#endif	/* defined(SR_SNMPv3_PACKET) */
        "\\\n\t[-d] [-timeout seconds] [-retries number] \\\n"
        "\t[-pkt_size number] \\\n"
        "\tdst_addr community/userName snmpTrapOID_value(OID) \\\n"
        "\tvariable_name type value [variable_name type value . . .]\n"
        "\t\t where type is:\n"
        "\t\t\t-i - integer \n"
        "\t\t\t-o - octet string \n"
        "\t\t\t-d - object identifier\n"
        "\t\t\t-a - ip_addr\n"
        "\t\t\t-c - counter\n"
        "\t\t\t-g - gauge\n"
        "\t\t\t-t - time_ticks\n"
        "\t\t\t-D - Display String\n"
        "\t\t\t-N - NULL"
        "\n";

#endif /* SR_SNMPv2_PDU */

#ifdef SR_SNMPv3_PACKET
    SetUtilTrapFlag(SEND_INFORM, (unsigned short) 22);
#endif /* SR_SNMPv3_PACKET */


    if (InitializeUtility(INFORM_COMMUNICATIONS, &argc, argv,
                          NULL, NULL, usage_string)) {
        return_value = -1;
        goto finished;
    }

#ifdef SR_SNMPv1_PACKET
    if (util_version == SR_SNMPv1_VERSION) {
        fprintf(stderr, "Cannot use SNMPv1 for INFORM requests.\n");
        return_value = -1;
        goto finished;
    }
#endif /* SR_SNMPv1_PACKET */


    if (argc < 2) {
        fprintf(stderr, usage_string, argv[0]);
        return_value = -1;
        goto finished;
    }


    req_pdu_ptr = MakePdu(INFORM_REQUEST_TYPE, global_req_id, 0L, 0L,
                           NULL, NULL, 0L, 0L, 0L);

    if ((oid_ptr = MakeOIDFromDot("sysUpTime.0")) == NULL) {
        DPRINTF((APWARN,
                 "%s: Could not make oid_ptr for sysUpTime.0\n", argv[0]));
        goto cleanup;
    }

    now = GetTimeNow();

    if ((vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL,
                                       (unsigned char) TIME_TICKS_TYPE,
                                       (void *) &now)) == NULL) {
        DPRINTF((APWARN,
                 "%s: Could not make vb_ptr for sysUpTime.0\n", argv[0]));
        goto cleanup;
    }
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    if (LinkVarBind(req_pdu_ptr, vb_ptr) == (short) -1) {
        DPRINTF((APWARN,
                 "inform: Could not link varbind with req_pdu_ptr\n"));
        goto cleanup;
    }
    vb_ptr = NULL;

    if ((oid_ptr = MakeOIDFromDot("1.3.6.1.6.3.1.1.4.1.0")) == NULL) {
        DPRINTF((APWARN,
                 "inform: Could not make oid for snmpTrapOID.0\n"));
        goto cleanup;
    }
    if ((snmpEvent_val = MakeOIDFromDot(argv[1])) == NULL) {
        DPRINTF((APWARN,
                 "inform: Could not make snmpTrapOID.0 value from %s\n",
                 argv[1]));
        goto cleanup;
    }
    if ((vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL,
                                       (unsigned char) OBJECT_ID_TYPE,
                                       (void *) snmpEvent_val)) == NULL) {
        DPRINTF((APWARN,
                 "inform: Could not make vb_ptr for snmpEventID\n"));
        goto cleanup;
    }
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    if (LinkVarBind(req_pdu_ptr, vb_ptr) == (short) -1) {
        DPRINTF((APWARN,
                 "inform: Could not link varbind with req_pdu_ptr\n"));
        goto cleanup;
    }
    vb_ptr = NULL;

    /*
     * If a SNMPv2c/SNMPv3 entity was really sending an InformRequest PDU, 
     * access checks would be made on all varbinds (see trapsend.c for an 
     * example)
     */

    /* put additional VarBinds onto the VarBindList */
    i = 2;
    vb_end = initial_vb_list;
    if (vb_end != NULL) {
        while (vb_end->next_var != NULL) {
            vb_end = vb_end->next_var;
        }
    }
    if (i < argc) {
        while (argv[i] != NULL) {
            if ((vb_ptr = BuildNextVarBind(argv, &i)) == NULL) {
                fprintf(stderr, usage_string, argv[0]);
                return_value = 1;
                goto finished;
            }

            /* COPY VarBind into PDU */
            if (LinkVarBind(req_pdu_ptr, vb_ptr) == (short) -1) {
                DPRINTF((APWARN, "inform: Cannot link varbind with pdu_ptr\n"));
                goto cleanup;
            }
        }
        vb_ptr = NULL;
    }

    snmpEvent_val = NULL;

    retries = tretries;
  retry_snmp_request:
    req_pdu_ptr->u.normpdu.request_id = ++global_req_id;

    /* Make the PDU packlet */
    if (BuildPdu(req_pdu_ptr) == -1) {
      DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
      goto finished;
    }

    /* Perform SNMP request */
    resp_pdu_ptr = PerformRequest(req_pdu_ptr, &status);
    if (status == SR_TIMEOUT) {
        retries--;
        if (retries > 0) {
            DPRINTF((APALWAYS, "retrying . . .\n"));
            goto retry_snmp_request;
        }
        DPRINTF((APALWAYS, "giving up . . .\n"));
        goto finished;
    }
    if (status == SR_ERROR) {
        return_value = -1;
        goto finished;
    }

    FreePdu(req_pdu_ptr);
    req_pdu_ptr = NULL;

    if (resp_pdu_ptr->type != GET_RESPONSE_TYPE) {
        fprintf(stderr, "received non GET_RESPONSE_TYPE packet.  Exiting.\n");
        goto cleanup;
    }

    if (resp_pdu_ptr->u.normpdu.error_status != NO_ERROR) {
        PrintErrorCode(resp_pdu_ptr->u.normpdu.error_index,
                         resp_pdu_ptr->u.normpdu.error_status);
        goto cleanup;
    }
    else {
        if (PrintVarBind_format == 0) {
            PrintVarBindList(resp_pdu_ptr->var_bind_list);
        } else {
            PrintFormattedVarBindList(resp_pdu_ptr->var_bind_list,
                                      PrintVarBind_format, 0);
        }
        if (print_packet_stats) {
            PrintResponseStats(INFORM_REQUEST_TYPE, resp_pdu_ptr->var_bind_list, -1);
        }

        goto cleanup;
    }

cleanup:

    /* clean up time */
    if (req_pdu_ptr != NULL) {
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
    }
    if (resp_pdu_ptr != NULL) {
        FreePdu(resp_pdu_ptr);
        resp_pdu_ptr = NULL;
    }
    if (oid_ptr != NULL) {
        FreeOID(oid_ptr);
        oid_ptr = NULL;
    }
    if (vb_ptr != NULL) {
        FreeVarBind(vb_ptr);
        vb_ptr = NULL;
    }
    if (snmpEvent_val != NULL) {
        FreeOID(snmpEvent_val);
        snmpEvent_val = NULL;
    }
  finished:
    return return_value;
}                                /* main() */
