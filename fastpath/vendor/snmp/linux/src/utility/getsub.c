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
 * getsub.c - a program to retreive and print all or part of a remote
 * routing table
 */


/*
getsub.c

     Getsub is an SNMP application that retrieves routing
     information from an entity by traversing the ipRouteDest,
     ipRouteIfIndex, ipRouteMetric1, ipRouteNextHop, ipRouteType,
     and ipRouteProto variable classes for each subnet found. It
     takes as arguments the address of the SNMP entity, a com-
     munity string to provide access to that entity or a USM username
     , and, optionally, a net or subnet address qualifier. Only those 
     portions of the routing table which match the net or subnet address 
     qualifier are retrieved. For example, if one were to enter:

     getsub -v[1|2c] myagent topSecret 192.244        (SNMPv1 community string)
                   or
     getsub -v3 myagent Guest 192.244         (SNMPv3 username)

     getsub would retrieve all entries beginning with 192.244.  Similarly,

     getsub -v[1|2c] myagent topSecret 128.139.4     (SNMPv1 community string)
                   or
     getsub -v3 myagent Guest 128.139.4       (SNMPv3 username)

     would retrieve all entries beginning with 128.139.4.  Finally,

     getsub -v[1|2c] myagent topSecret               (SNMPv1 community string)
                   or
     getsub -v3 myagent Guest                 (SNMPv3 username)

     would retrieve the entire routing table.
*/

#include "sr_utils.h"
#include "diag.h"
SR_FILENAME
#include "prnt_lib.h"
#include "oid_lib.h"


static int return_value = 0;

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
    VarBind        *resp_vb_ptr = NULL;
    Pdu            *req_pdu_ptr = NULL, *resp_pdu_ptr = NULL;
    SR_INT32        req_id = 0;
    int             status;
    int             subnet_length;
    int             subnet[4];
    char            buffer[255];
    char            temp[255];
    int             i, k;
    char            var_name[255];
    char           *usage_string;
    OID            *oid_name[6];

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
        "\tagent_addr community/userName \\\n"
        "\t[subnet address]"
        "\n";


    for (i = 0; i < 6; i++) {
	oid_name[i] = NULL;
    }

    if (InitializeUtility(SNMP_COMMUNICATIONS, &argc, argv,
                          NULL, NULL, usage_string)) {
        return_value = -1;
        goto finished;
    }

    subnet[0] = 0;
    subnet[1] = 0;
    subnet[2] = 0;
    subnet[3] = 0;

    if (argc == 2) {
        subnet_length = sscanf(argv[1], "%d.%d.%d.%d", &subnet[0], &subnet[1],
                               &subnet[2], &subnet[3]);
    }
    else {
        subnet_length = 0;
    }

    buffer[0] = '\0';
    for (i = 0; i < subnet_length; i++) {
        sprintf(temp, ".%d", subnet[i]);
        strcat(buffer, temp);
    }

    /* start a dummy response PDU */
    /* we will throw this one away ... this one is to jump-start the loop */
    resp_pdu_ptr = MakePdu(GET_NEXT_REQUEST_TYPE, req_id, 0L, 0L,
                            NULL, NULL, 0L, 0L, 0L);

    strcpy(var_name, "ipRouteDest");
    strcat(var_name, buffer);
    oid_name[0] = MakeOIDFromDot(var_name);

    strcpy(var_name, "ipRouteIfIndex");
    strcat(var_name, buffer);
    oid_name[1] = MakeOIDFromDot(var_name);

    strcpy(var_name, "ipRouteMetric1");
    strcat(var_name, buffer);
    oid_name[2] = MakeOIDFromDot(var_name);

    strcpy(var_name, "ipRouteNextHop");
    strcat(var_name, buffer);
    oid_name[3] = MakeOIDFromDot(var_name);

    strcpy(var_name, "ipRouteType");
    strcat(var_name, buffer);
    oid_name[4] = MakeOIDFromDot(var_name);

    strcpy(var_name, "ipRouteProto");
    strcat(var_name, buffer);
    oid_name[5] = MakeOIDFromDot(var_name);

    /* Flesh out packet */
    strcpy(var_name, "ipRouteDest");
    strcat(var_name, buffer);
    if ((oid_ptr = MakeOIDFromDot(var_name)) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", buffer);
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;

    strcpy(var_name, "ipRouteIfIndex");
    strcat(var_name, buffer);
    if ((oid_ptr = MakeOIDFromDot(var_name)) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", buffer);
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;

    strcpy(var_name, "ipRouteMetric1");
    strcat(var_name, buffer);
    if ((oid_ptr = MakeOIDFromDot(var_name)) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", buffer);
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;

    strcpy(var_name, "ipRouteNextHop");
    strcat(var_name, buffer);
    if ((oid_ptr = MakeOIDFromDot(var_name)) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", buffer);
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;

    strcpy(var_name, "ipRouteType");
    strcat(var_name, buffer);
    if ((oid_ptr = MakeOIDFromDot(var_name)) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", buffer);
        return_value = -1;
        goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(resp_pdu_ptr, vb_ptr);
    vb_ptr = NULL;

    strcpy(var_name, "ipRouteProto");
    strcat(var_name, buffer);
    if ((oid_ptr = MakeOIDFromDot(var_name)) == NULL) {
        fprintf(stderr, "Cannot translate variable class:  %s\n", buffer);
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

    k = 0;
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
             * Check for termination case.
             * check all OIDs in response PDU.
             */
            resp_vb_ptr = resp_pdu_ptr->var_bind_list;
            for (k = 0; k < 6; k++) {
                if (CheckOID(oid_name[k], resp_vb_ptr->name) < 0) {
                    return_value = -1;
                    goto finished;
                }
                resp_vb_ptr = resp_vb_ptr->next_var;
            }
            PrintRouteInfo(resp_pdu_ptr->var_bind_list);
        }                        /* end of else no error */
    }                                /* while (1) */


  finished:
    if (req_pdu_ptr != NULL) {
        FreePdu(req_pdu_ptr);
        req_pdu_ptr = NULL;
    }
    if (resp_pdu_ptr != NULL) {
        FreePdu(resp_pdu_ptr);
        resp_pdu_ptr = NULL;
    }
    for (i = 0; i < 6; i++) {
	if (oid_name[i] != NULL) {
            FreeOID(oid_name[i]);
            oid_name[i] = NULL;
        }
    }
    CloseUp();
    return return_value;
}                                /* getsub.c */

