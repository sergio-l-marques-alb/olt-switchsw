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
getnext.c

     Getnext is an SNMP application to retrieve a set of 
     individual variables from an SNMP entity using a "GET-NEXT"
     request. The arguments are the entity's address, the com-
     munity string for access to the SNMP entity or a USM username
     and the variable name(s)
     expressed as either dot-notation or the variable name as it appears
     in the MIB document. It should be noted that since the function
     is a lexicographical Get Next, the variable returned will be
     the lexicographically greater fully qualified object identifier
     for what was entered. For instance

     getnext -v[1|2c] suzzy topSecret system interfaces    (community string)
                       or
     getnext -v3 localhost Guest system interfaces     (SNMPv3 username)

     would return the variables sysDescr.0 and ifNumber.0
*/

#include "sr_utils.h"
#include "diag.h"
SR_FILENAME
#include "prnt_lib.h"

extern int print_packet_stats;


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
    Pdu            *pdu_ptr = NULL, *in_pdu_ptr = NULL;
    int             num_requests;
    int             i;
    SR_INT32        req_id = 0;
    int             status;
    static char     usage_string[] =
        "usage:  %s "
#ifdef SR_SNMPv1_PACKET
        "[-v1] "
#endif /* SR_SNMPv1_PACKET */
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
        "\t[-pkt_size number] [-format string] \\\n"
        "\tagent_addr community/userName \\\n"
        "\tvariable_name [variable_name . . .]"
        "\n";

    if (InitializeUtility(SNMP_COMMUNICATIONS, &argc, argv,
                          NULL, NULL, usage_string)) {
        return_value = -1;
        goto finished;
    }

    if (argc < 2) {
        fprintf(stderr, usage_string, argv[0]);
        return_value = -1;
        goto finished;
    }


    /* start a PDU */
    pdu_ptr = MakePdu(GET_NEXT_REQUEST_TYPE, ++req_id, 0L, 0L, NULL, NULL, 0L, 0L, 0L);

    num_requests = argc - 1;
    for (i = 0; i < num_requests; i++) {
	oid_ptr = MakeOIDFromDot(argv[i + 1]);
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
	    fprintf(stderr, "Cannot translate variable class:  %s\n", argv[i + 1]);
	    return_value = -1;
	    goto finished;
	}

	vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
	FreeOID(oid_ptr);
	oid_ptr = NULL;

	/* COPY VarBind into PDU */
	LinkVarBind(pdu_ptr, vb_ptr);
	vb_ptr = NULL;
    }

    /* Make the PDU packlet */
    if (BuildPdu(pdu_ptr) == -1) {
      DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
      goto finished;
    }

    retries = tretries;
  retry_snmp_request:
    /* Perform SNMP request */
    in_pdu_ptr = PerformRequest(pdu_ptr, &status);
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

    FreePdu(pdu_ptr);
    pdu_ptr = NULL;

    /* check for error status stuff... */
    if (in_pdu_ptr->u.normpdu.error_status != NO_ERROR) {
	PrintErrorCode(in_pdu_ptr->u.normpdu.error_index,
			 in_pdu_ptr->u.normpdu.error_status);
    }
    else {
            if (PrintVarBind_format == 0) {
                PrintVarBindList(in_pdu_ptr->var_bind_list);
            } else {
                PrintFormattedVarBindList(in_pdu_ptr->var_bind_list,
                                          PrintVarBind_format, 0);
            }
            if (print_packet_stats) {
                PrintResponseStats(GET_NEXT_REQUEST_TYPE, in_pdu_ptr->var_bind_list, -1);
            }

    }
    return_value = 0;


  finished:
    if (in_pdu_ptr != NULL) {
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
    }
    CloseUp();
    return return_value;
}				/* getnext.c */

