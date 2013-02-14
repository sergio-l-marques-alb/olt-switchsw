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
getid.c

     Getid is an SNMP application to retrieve the variables sysDescr.0,
     sysObjectID.0, and sysUpTime.0 from an SNMP entity.
     The arguments are the entity's address and the community
     string or a SNMPv3 USM username
     needed for access to the SNMP entity. The primary
     purpose of this application is to illustrate the use of the
     SNMP library routines.

     If using SNMPv2c or SNMPv3, two extra variables, sysUpTime.2 and 1.2.0,
     are retrieved to demonstrate NO_SUCH_INSTANCE_EXCEPTION and
     NO_SUCH_OBJECT_EXCEPTION exceptions, respectively. If a manager
     uses a trilingual getid with a community string for the
     retrieval, the manager will get back a "No Such Variable Name"
     error. Use a SNMPv1 only getid to get back the values for
     sysDescr.0, sysObjectID.0, and sysUpTime.0. The manager
     could also use the getmany or getone utility.

     Example:

	./getid -v[1|2c] localhost public             (SNMPv1 community string)

		or

	./getid -v3 [-ctx contextName] localhost Guest         (SNMPv3 username)
*/

#include "sr_utils.h"
#include "diag.h"
SR_FILENAME
#include "prnt_lib.h"


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
    int             status;
    int             return_value = 0;
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

    /* start a PDU */
    pdu_ptr = MakePdu(GET_REQUEST_TYPE, global_req_id, 0L, 0L, NULL, NULL, 0L, 0L, 0L);


    oid_ptr = MakeOIDFromDot("sysDescr.0");
    if (oid_ptr == NULL) {
	fprintf(stderr, "Cannot translate variable class:  %s\n", "sysDescr.0");
	return_value = -1;
	goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(pdu_ptr, vb_ptr);	/* link VarBind into PDU */
    vb_ptr = NULL;

    oid_ptr = MakeOIDFromDot("sysObjectID.0");
    if (oid_ptr == NULL) {
	fprintf(stderr, "Cannot translate variable class:  %s\n", "sysObjectID.0");
	return_value = -1;
	goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(pdu_ptr, vb_ptr);	/* link VarBind into PDU */
    vb_ptr = NULL;

    oid_ptr = MakeOIDFromDot("sysUpTime.0");
    if (oid_ptr == NULL) {
	fprintf(stderr, "Cannot translate variable class:  %s\n", "sysUpTime.0");
	return_value = -1;
	goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(pdu_ptr, vb_ptr);	/* link VarBind into PDU */
    vb_ptr = NULL;

#ifdef SR_SNMPv2_PDU
    oid_ptr = MakeOIDFromDot("sysUpTime.2");
    if (oid_ptr == NULL) {
	fprintf(stderr, "Cannot translate variable class:  %s\n", "sysUpTime.2");
	return_value = -1;
	goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(pdu_ptr, vb_ptr);	/* link VarBind into PDU */
    vb_ptr = NULL;

    oid_ptr = MakeOIDFromDot("1.2.0");
    if (oid_ptr == NULL) {
	fprintf(stderr, "Cannot translate variable class:  %s\n", "1.2.0");
	return_value = -1;
	goto finished;
    }
    vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL);
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    LinkVarBind(pdu_ptr, vb_ptr);	/* link VarBind into PDU */
    vb_ptr = NULL;
#endif				/* SR_SNMPv2_PDU */


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
    }

  finished:
    if (in_pdu_ptr != NULL) {
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
    }
    CloseUp();
    return return_value;
}				/* getid.c */

