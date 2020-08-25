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
setany.c

     Setany performs a set request on the variables. The arguments
     are the agent address in internet dot notation, the community name
     for access to the SNMP entity or a USM username,
     and for a triplet for each variable to be
     set consisting of: the variable name in dot notation, an 'i', 'o',
     'd', 'a', 'c', 'g', or 't'  to  indicate  if  the  variables
     value  is being given as an integer, an octet string (in hex
     notation), an object identifier (in  dot  notation),  an  Ip
     address  (in  dot  notation),  a  counter, a gauge, or time-
     ticks, followed by the value.  For example:

     setany -v[1|2c] localhost suranet0 "ifAdminStatus.2" -i 3
                            or
     setany -v3 128.169.1.1 Guest "ifAdminStatus.2" -i 3

     to set the adminstrative status of interface 2 to 3 (down).

*/

#include "sr_utils.h"
#include "prnt_lib.h"

#include "diag.h"
SR_FILENAME

extern int print_packet_stats;



int main
    SR_PROTOTYPE((int argc,
                  char *argv[]));

int
main(argc, argv)
    int             argc;
    char           *argv[];
{
    int             return_value = 0;
    VarBind        *vb_ptr = NULL;
    Pdu            *req_pdu_ptr = NULL, *resp_pdu_ptr = NULL;
    int             i;
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
        "\t[-pkt_size number] \\\n"
        "\tagent_addr community/userName \\\n"
        "\tvariable_name type value [variable_name type value . . .]\n"
        "\t\t where type is:\n"
#if defined(SR_SNMPv2_PDU)
        "\t\t\t-b - bit string \n"
#endif	/* defined(SR_SNMPv2_PDU) */
        "\t\t\t-i - integer \n"
        "\t\t\t-o - octet string \n"
        "\t\t\t-d - object identifier\n"
        "\t\t\t-a - ip_addr\n"
        "\t\t\t-c - counter\n"
        "\t\t\t-g - gauge\n"
        "\t\t\t-t - time_ticks\n"
        "\t\t\t-T - DateAndTime\n"
        "\t\t\t-D - DisplayString\n"
        "\t\t\t-N - NULL"
        "\n";

    if (InitializeUtility(SNMP_COMMUNICATIONS, &argc, argv,
                          NULL, NULL, usage_string)) {
        return_value = -1;
        goto finished;
    }

    if (argc < 3) {
        fprintf(stderr, usage_string, argv[0]);
        return_value = -1;
        goto finished;
    }

    /* start a PDU */
    req_pdu_ptr = MakePdu(SET_REQUEST_TYPE, global_req_id, 0L, 0L, NULL, NULL, 0L, 0L, 0L);

    i = 1;
    while (argv[i] != NULL) {
	if ((vb_ptr = BuildNextVarBind(argv, &i)) == NULL) {
	    fprintf(stderr, usage_string, argv[0]);
	    return_value = 1;
	    goto finished;
	}


	/* COPY VarBind into PDU */
	LinkVarBind(req_pdu_ptr, vb_ptr);

	vb_ptr = NULL;
    }


    /* Make the PDU packlet */
    if (BuildPdu(req_pdu_ptr) == -1) {
      DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
      return_value = -1;
      goto finished;
    }

    /* Perform SNMP request */
    resp_pdu_ptr = PerformRequest(req_pdu_ptr, &status);
    if (status == SR_TIMEOUT) {
        return_value = -1;
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
	PrintErrorCode(resp_pdu_ptr->u.normpdu.error_index,
			 resp_pdu_ptr->u.normpdu.error_status);
    }
    else {
        if (PrintVarBind_format == 0) {
            PrintVarBindList(resp_pdu_ptr->var_bind_list);
        } else {
            PrintFormattedVarBindList(resp_pdu_ptr->var_bind_list,
                                      PrintVarBind_format, 0);
        }
        if (print_packet_stats) {
            PrintResponseStats(resp_pdu_ptr->type, resp_pdu_ptr->var_bind_list, -1);
        }

    }

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
}				/* setany.c */
