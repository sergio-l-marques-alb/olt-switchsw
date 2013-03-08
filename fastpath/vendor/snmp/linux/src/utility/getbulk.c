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
 * getbulk.c
 *
 *   For example, running the following:
 *       getbulk -v3 localhost Guest 1 4 iso iso
 *
 *   will return one non-repeater and 4 repeaters, with the values being
 *   sysDescr.0, sysDescr.0, sysObjectID.0, sysUpTime.0, and sysContact.0
 *
 *   Getbulk only works with SNMPv2c and SNMPv3.
 */


#include "sr_utils.h"
#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME
#include "prnt_lib.h"

extern int print_packet_stats;


static VarBind *vb_ptr;
long            non_repeaters;
long            max_repetitions;

int main
    SR_PROTOTYPE((int argc,
                  char *argv[]));

int
main(argc, argv)
    int             argc;
    char           *argv[];
{
    OID            *oid_ptr = NULL;
    Pdu            *req_pdu_ptr = NULL, *resp_pdu_ptr = NULL;
    int             num_requests;
    int             i;
    int             status;
    int             return_value = 0;
    char           *usage_string;

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
        "\t[-format string]  \\\n"
        "\tagent_addr community/userName non_repeaters max_repetitions \\\n"
        "\tvariable_name [variable_name . . .]"
        "\n";

#endif /* SR_SNMPv2_PDU */

    if (InitializeUtility(SNMP_COMMUNICATIONS, &argc, argv,
                          NULL, NULL, usage_string)) {
        return_value = -1;
        goto finished;
    }


#ifdef SR_SNMPv1_PACKET
    if (util_version == SR_SNMPv1_VERSION) {
        fprintf(stderr, "Cannot use SNMPv1 for GETBULK requests.\n");
        return_value = -1;
        goto finished;
    }
#endif /* SR_SNMPv1_PACKET */

    if (argc < 4) {
        fprintf(stderr, usage_string, argv[0]);
        return_value = -1;
        goto finished;
    }


    /*** Extract non_repeaters and max_repetitions from command line ***/
    if (sscanf(argv[1], "%ld", &non_repeaters) != 1) {
        fprintf(stderr, "Cannot translate non_repeaters: %s\n", argv[1]);
        return_value = -1;
        goto finished;
    }
    if (sscanf(argv[2], "%ld", &max_repetitions) != 1) {
        fprintf(stderr, "Cannot translate max_repetitions: %s\n", argv[2]);
        return_value = -1;
        goto finished;
    }

    req_pdu_ptr = MakePdu(GET_BULK_REQUEST_TYPE, ++global_req_id, non_repeaters,
                           max_repetitions, NULL, NULL, 0L, 0L, 0L);

    num_requests = argc - 3;
    for (i = 0; i < num_requests; i++) {
        oid_ptr = MakeOIDFromDot(argv[i + 3]);
        /*
         *  Pathological case: argument is "iso" or "1".  We turn this
         *  into 1.0 as a convenience to the user.
         */
        if (oid_ptr != NULL) { 
            if (oid_ptr->length == 1) {
                if (oid_ptr->oid_ptr[0] == 1) {
                    FreeOID(oid_ptr);
                    oid_ptr = MakeOIDFromDot("1.0");
                } else {
                    FreeOID(oid_ptr);
                    oid_ptr = NULL;
                }
            }
        }
        if (oid_ptr == NULL) {
            fprintf(stderr, "Cannot translate variable class: %s\n", argv[i+3]);
            return_value = -1;
            goto finished;
        }

        vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
        FreeOID(oid_ptr);
        oid_ptr = NULL;

        /* COPY VarBind into PDU */
        LinkVarBind(req_pdu_ptr, vb_ptr);
        vb_ptr = NULL;
    }

    /* Make the PDU packlet */
    if (BuildPdu(req_pdu_ptr) == -1) {
       DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
       goto finished;
    }

    retries = tretries;
  retry_snmp_request:
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
        return_value = -1;
        goto finished;
    }

    /* check for error status stuff... */
    if (resp_pdu_ptr->u.normpdu.error_status != NO_ERROR) {
        if (resp_pdu_ptr->u.normpdu.error_status == NO_ACCESS_ERROR) {
            printf("End of MIB.\n");
            return_value = 0;
            goto finished;
        }
        PrintErrorCode(resp_pdu_ptr->u.normpdu.error_index,
                         resp_pdu_ptr->u.normpdu.error_status);

        return_value = -1;
        goto finished;
    }                        /* if ! NOERROR (was an error) */
    else {                        /* no error was found */

        if (PrintVarBind_format == 0) {
            PrintVarBindList(resp_pdu_ptr->var_bind_list);
        } else {
            PrintFormattedVarBindList(resp_pdu_ptr->var_bind_list,
                                      PrintVarBind_format, 0);
        }
        if (print_packet_stats) {
            PrintResponseStats(GET_BULK_REQUEST_TYPE, resp_pdu_ptr->var_bind_list, 1);
        }

    }                        /* end of else no error */

  finished:
    if (req_pdu_ptr != NULL) {
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
    }
    if (resp_pdu_ptr != NULL) {
        FreePdu(resp_pdu_ptr);
        resp_pdu_ptr = NULL;
    }
    CloseUp();
    return return_value;
}                                /* getbulk.c */

