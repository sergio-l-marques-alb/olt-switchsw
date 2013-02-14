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
 * Getmany.c - program to send get-next-requests and process get-responses
 * until the entire variable class entered is traversed
 */

/*
getmany.c

     Getmany is an SNMPv1/SNMPv2c/SNMPv3 application to retrieve classes of
     variables from an SNMPv1/SNMPv2c/SNMPv3 entity. The arguments are the
     entity's address, the community string access to the SNMP entity
     or a USM username and the variable class name(s) is expressed as object
     identifiers in either dot-notation or as the mib-variables as they
     appear in the MIB document. Getmany retrieves the variable
     class by calling the SNMPv1/SNMPv2c/SNMPv3 entity with the variable
     class name to get the first variable in the class, and then cal-
     ling the entity again using the variable name returned in
     the previous call to retrieve the next variable in the
     class, utilizing the get-next aspect of the variable retrevial
     system. For instance, running the following:

     getmany -v[1|2c] localhost public ipRouteDest         (community string)
                  or
     getmany -v3 [-ctx contextName] localhost Guest ipRouteDest (username)

     will traverse the gateway's ipRouteDest variable class (the
     next gateway traveling to in the route for the given net-
     number, which makes up the rest of the variable name) The
     traversing of the variable space stops when all of the
     classes being polled return a variable of a class different
     than what was requested. Note that a gateway's entire vari-
     able tree can be traversed with a call of

     getmany -v[1|2c] localhost public iso             (community string)
                 or
     getmany -v3 [-ctx contextName] localhost Guest iso           (username)
*/

#include "sr_snmp.h"
#include "sr_utils.h"
#include "diag.h"
SR_FILENAME
#include "prnt_lib.h"
#include "oid_lib.h"

extern int print_packet_stats;


static VarBind *vb_ptr;
static int      return_value = 0;
static OID     *init_oid_ptr;

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
    int             i, status;
    int             orig_retries;
    int             packet_count = 0;
    VarBind         *full_resp_vblist = NULL, *vblist_end;
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
    orig_retries = retries;

    if (argc < 2) {
        fprintf(stderr, usage_string, argv[0]);
        return_value = -1;
        goto finished;
    }


    /* start a dummy response PDU */
    /* we will throw this one away ... this one is to jump-start the loop */
    resp_pdu_ptr = MakePdu(GET_NEXT_REQUEST_TYPE, global_req_id, 0L, 0L,
                            NULL, NULL, 0L, 0L, 0L);

    num_requests = argc - 1;
    init_oid_ptr = MakeOIDFromDot(argv[1]);
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
            fprintf(stderr, "Cannot translate MIB variable: %s\n", argv[i + 1]);
            return_value = -1;
            goto finished;
        }

        vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);

        FreeOID(oid_ptr);
        oid_ptr = NULL;

        /* COPY VarBind into PDU */
        LinkVarBind(resp_pdu_ptr, vb_ptr);
        vb_ptr = NULL;
    }

    /* Make the PDU packlet */
    if (BuildPdu(resp_pdu_ptr) == -1) {
      DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
      goto finished;
    }

    /*
     * we now have a ficticious response pdu as if it came from parsing a
     * response
     */

    while (1) {
        VarBind *tmpVb = NULL;
        /*
         * make a new request pdu using the fields from the current response
         * pdu
         */
        req_pdu_ptr = MakePdu(GET_NEXT_REQUEST_TYPE, ++global_req_id, 0L, 0L,
                               NULL, NULL, 0L, 0L, 0L);

        /* point to the old (response pdu) */
        tmpVb = resp_pdu_ptr->var_bind_list;

        /* copy the OID fields from the old pdu to the new */
        /* value fields of the varbind are set to NULL on the request */
        while (tmpVb != NULL) {
             oid_ptr = MakeOID(tmpVb->name->oid_ptr, tmpVb->name->length);
             vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
             FreeOID(oid_ptr);
             oid_ptr = NULL;

             /* link it into the varbind of the pdu under construction */
             LinkVarBind(req_pdu_ptr, vb_ptr);
             /* repeat for all varbinds in varbindlist */
             tmpVb = tmpVb->next_var;
        }                        /* while(temp...) */

        /* Copy in the secpack/dssp varbinds */

        /* Make the PDU packlet */
        if (BuildPdu(req_pdu_ptr) == -1) {
          DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
          goto finished;
        }
        FreePdu(resp_pdu_ptr);

        retries = tretries;
  retry_snmp_request:
        /* Perform SNMP request */
        resp_pdu_ptr = PerformRequest(req_pdu_ptr, &status);
        if (status == SR_TIMEOUT) {
            retries--;
            if (retries > 0) {
                DPRINTF((APALWAYS, "retrying . . .\n"));
                FreePdu(resp_pdu_ptr);
                resp_pdu_ptr = NULL;
                goto retry_snmp_request;
            }
            DPRINTF((APALWAYS, "giving up . . .\n"));
            goto finished;
        }
	else if (status == SR_ERROR) {
            return_value = -1;
            goto finished;
        }
	else {
	    /* when we receive a good reply, we forgive all earlier timeouts */
	    retries = orig_retries;
	}

        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;

        if (resp_pdu_ptr->type != GET_RESPONSE_TYPE) {
            DPRINTF((APALWAYS, "%s: received non GET_RESPONSE_TYPE packet.\n",
                     argv[0]));
            return_value = -1;
            goto finished;
        }

        /* check for error status stuff... */
        if (resp_pdu_ptr->u.normpdu.error_status != NO_ERROR) {
            if ((resp_pdu_ptr->u.normpdu.error_status == NO_ACCESS_ERROR) ||
                (resp_pdu_ptr->u.normpdu.error_status == NO_SUCH_NAME_ERROR)) {
                printf("End of MIB.\n");
                return_value = 0;
                goto finished;
            }
            PrintErrorCode(resp_pdu_ptr->u.normpdu.error_index,
                           resp_pdu_ptr->u.normpdu.error_status);
            return_value = -1;
            goto finished;
        } else {
            if (print_packet_stats) {
                packet_count++;
            }
#ifdef SR_SNMPv2_PDU
            if (CheckEndOfMIB(resp_pdu_ptr) == -1) {
                return_value = 0;
                goto finished;
            }
#endif /* SR_SNMPv2_PDU */
            
            if (print_packet_stats) {
                /* add response to stats */
                if (full_resp_vblist == NULL) {
                   full_resp_vblist = vblist_end = 
                                  CloneVarBindList(resp_pdu_ptr->var_bind_list);
                } else {
                   while(vblist_end->next_var != NULL) {
                        vblist_end = vblist_end->next_var;
                   }
                   vblist_end->next_var = 
                                  CloneVarBindList(resp_pdu_ptr->var_bind_list);
                   vblist_end = vblist_end->next_var;
                }
            }

            /*
             * Check for termination case (only checking first one for
             * now)
             */
            if (CheckOID(init_oid_ptr, resp_pdu_ptr->var_bind_list->name) < 0) {
                return_value = 0;
                goto finished;
            }

            if (PrintVarBind_format == 0) {
                PrintVarBindList(resp_pdu_ptr->var_bind_list);
            } else {
                PrintFormattedVarBindList(resp_pdu_ptr->var_bind_list,
                                          PrintVarBind_format, 0);
            }


        }                        /* end of else no error */
    }                                /* while (1) */

  finished:

    if (return_value == 0 && print_packet_stats != 0) {
        PrintResponseStats(GET_NEXT_REQUEST_TYPE, full_resp_vblist, packet_count);
    }
 
    if (init_oid_ptr != NULL) {
        FreeOID(init_oid_ptr);
        init_oid_ptr = NULL;
    }
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
}
