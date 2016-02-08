/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
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
 * getroute.c - a program to retreive and print a remote routing table.
 */


/*
getroute.c

     Getroute is an SNMP application that retrieves routing
     information from an entity by traversing the ipRouteDest,
     ipRouteIfIndex, ipRouteMetric1, ipRouteNextHop, ipRouteType,
     and ipRouteProto variable classes for each route found. It
     takes as arguments the address of the SNMP entity and a com-
     munity string to provide access to that entity or a USM username.

     Example:

        ./getroute -v[1|2c] myagent topSecret        (SNMPv1 community string)

                     or

        ./getroute -v3 myagent Guest         (SNMPv3 username)
*/

#include "sr_utils.h"
#include "diag.h"
SR_FILENAME
#include "prnt_lib.h"
#include "oid_lib.h"



static int return_value = 0;
static OID *init_oid_ptr = NULL;

int main
    SR_PROTOTYPE((int argc,
                  char *argv[]));

int
main(argc, argv)
    int             argc;
    char           *argv[];
{
    OID            *oid_ptr = NULL;
    VarBind        *vb_ptr = NULL, *temp_vb_ptr = NULL;
    Pdu            *req_pdu_ptr = NULL, *resp_pdu_ptr = NULL;
    SR_INT32        req_id = 0;
    int             status;
    char           *usage_string;

    usage_string =
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
        "\tagent_addr community/userName"
        "\n";


    if (InitializeUtility(SNMP_COMMUNICATIONS, &argc, argv,
                          NULL, NULL, usage_string)) {
        return_value = -1;
        goto finished;
    }

    /* start a dummy response PDU */
    /* we will throw this one away ... this one is to jump-start the loop */
    resp_pdu_ptr = MakePdu(GET_NEXT_REQUEST_TYPE, req_id, 0L, 0L,
                            NULL, NULL, 0L, 0L, 0L);

    init_oid_ptr = MakeOIDFromDot("ipRouteDest");

    /* Flesh out packet */
    if ((oid_ptr = MakeOIDFromDot("ipRouteDest")) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", "ipRouteDest");
        FreeOID(init_oid_ptr);
        init_oid_ptr = NULL;
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;

    if ((oid_ptr = MakeOIDFromDot("ipRouteIfIndex")) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", "ipRouteInIndex");
        FreeOID(init_oid_ptr);
        init_oid_ptr = NULL;
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;

    if ((oid_ptr = MakeOIDFromDot("ipRouteMetric1")) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", "ipRouteMetric1");
        FreeOID(init_oid_ptr);
        init_oid_ptr = NULL;
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;

    if ((oid_ptr = MakeOIDFromDot("ipRouteNextHop")) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", "ipRouteNextHop");
        FreeOID(init_oid_ptr);
        init_oid_ptr = NULL;
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;

    if ((oid_ptr = MakeOIDFromDot("ipRouteType")) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", "ipRouteType");
        FreeOID(init_oid_ptr);
        init_oid_ptr = NULL;
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;

    if ((oid_ptr = MakeOIDFromDot("ipRouteProto")) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", "ipRouteProto");
        FreeOID(init_oid_ptr);
        init_oid_ptr = NULL;
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;


    /* Make the PDU packlet */
    if (BuildPdu(resp_pdu_ptr) == -1) {
      DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
      goto finished;
    }

    while (1) {
        /*
         * make a new request pdu using the fields from the current response
         * pdu
         */
        req_pdu_ptr = MakePdu(GET_NEXT_REQUEST_TYPE, ++req_id, 0L, 0L,
                               NULL, NULL, 0L, 0L, 0L);

        /* point to the old (response pdu) */
        temp_vb_ptr = resp_pdu_ptr->var_bind_list;

        /* copy the OID fields from the old pdu to the new */
        /* value fields of the varbind are set to NULL on the request */
        while (temp_vb_ptr != NULL) {
            oid_ptr = MakeOID(temp_vb_ptr->name->oid_ptr, temp_vb_ptr->name->length);
            vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
            FreeOID(oid_ptr);
            oid_ptr = NULL;
            /* link it into the varbind of the pdu under construction */
            LinkVarBind(req_pdu_ptr, vb_ptr);
            vb_ptr = NULL;
            /* repeat for all varbinds in varbindlist */
            temp_vb_ptr = temp_vb_ptr->next_var;
        }                        /* while(temp... */

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

        /* check for error status stuff... */
        if (resp_pdu_ptr->u.normpdu.error_status != NO_ERROR) {
            if ((resp_pdu_ptr->u.normpdu.error_status == NO_ACCESS_ERROR) ||
                (resp_pdu_ptr->u.normpdu.error_status == NO_SUCH_NAME_ERROR)) {
                printf("End of MIB.");
                return_value = -1;
                goto finished;
            }
            PrintErrorCode(resp_pdu_ptr->u.normpdu.error_index,
                           resp_pdu_ptr->u.normpdu.error_status);
            return_value = -1;
            goto finished;
        }                        /* if ! NOERROR (was an error) */
        else {                /* no error was found */
#ifdef SR_SNMPv2_PDU
            if (CheckEndOfMIB(resp_pdu_ptr) == -1) {
                return_value = 0;
                goto finished;
            }
#endif                                /* SR_SNMPv2_PDU */
            /*
             * Check for termination case (only checking first one for
             * now)
             */
            if (CheckOID(init_oid_ptr, resp_pdu_ptr->var_bind_list->name) < 0) {
                return_value = -1;
                goto finished;
            }

            PrintRouteInfo(resp_pdu_ptr->var_bind_list);
        }                        /* end of else no error */
    }                                /* while (1) */


  finished:
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
}                                /* getroute.c */

