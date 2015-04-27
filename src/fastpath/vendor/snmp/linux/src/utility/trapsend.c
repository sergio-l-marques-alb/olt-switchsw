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
trapsend.c

     Trapsend is a program to send trap messages to trap monitoring 
     stations.  It takes as arguments the IP address of
     the monitoring station (or its nodename), a community string or
     a USM username and the integer number that corresponds to the trap 
     to be sent.

     For example:

          trapsend -v[1|2c] localhost public 0   (SNMPv1 community string)

                    or

          trapsend -v3 localhost Guest coldStart   (SNMPv3 username)

     would send a cold-start trap to the machine suzzy.
*/

#define SR_TRAP
#include "sr_utils.h"
#include "diag.h"
SR_FILENAME
#include "prnt_lib.h"


#define SR_IP_TRANSPORT             0
int dst_trans = SR_IP_TRANSPORT;


int main
    SR_PROTOTYPE((int argc,
		  char *argv[]));

int
main(argc, argv)
    int             argc;
    char           *argv[];
{
#ifdef SR_SNMPv2_PDU
    FNAME("trapsend")
#endif /* SR_SNMPv2_PDU */
    OID            *oid_ptr = (OID *) NULL;
    OID            *oid_ptr2 = (OID *) NULL;
    OctetString    *os_ptr = NULL;
    VarBind        *vb_ptr = NULL;
    Pdu            *pdu_ptr = NULL;
    SR_INT32        generic_trap, specific_trap;
    SR_UINT32       timeticks;
    OID            *enterprise;
    int             i;
    SR_UINT32       local_ip_addr;
    char            buffer[80];
    int             cc;
    int             return_value = 0;
    int             status;
    static char     usage_string[] =
#ifdef SR_SNMPv1_PACKET
        "usage:  %s "
        "[-v1] dst_addr community generic_trap \\\n"
        "\t[specific_trap enterprise] \\\n"
        "\t[time_ticks [variable_name type value [variable_name type value . . .]]]\n"
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
#ifdef SR_SNMPv2_PDU
        "\n"
#endif /* SR_SNMPv2_PDU */
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2_PDU
        "usage:  %s "
#ifdef SR_SNMPv2c_PACKET
        "[-v2c] "
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
#ifdef NO_ALT_ENGINEID
        "[-v3] "
#else /* NO_ALT_ENGINEID */
        "[-v3 [-alt_engine_id]] "
#endif /* NO_ALT_ENGINEID */
#endif /* SR_SNMPv3_PACKET */
#if defined(SR_SNMPv3_PACKET)
        "\\\n\t[-ctxid contextID] [-ctx contextName] \\\n"
#endif	/* defined(SR_SNMPv3_PACKET) */
        "\tdst_addr community/userName snmpTrapOID_value(OID) \\\n"
        "\t[sysUpTime_value [variable_name type value [variable_name type value . . .]]]\n"
        "\t\t where type is:\n"
        "\t\t\t-i - integer \n"
        "\t\t\t-o - octet string \n"
        "\t\t\t-d - object identifier\n"
        "\t\t\t-a - ip_addr\n"
        "\t\t\t-c - counter\n"
        "\t\t\t-j - counter64\n"
        "\t\t\t-g - gauge\n"
        "\t\t\t-t - time_ticks\n"
        "\t\t\t-D - Display String\n"
        "\t\t\t-N - NULL"
#endif /* SR_SNMPv2_PDU */
        "\n";

    enterprise = NULL;
    timeticks = 0;

    /* this is a one time trap send */
#ifdef SR_SNMPv3_PACKET
    SetUtilTrapFlag(SEND_TRAP, (unsigned short) 21);
#endif /* SR_SNMPv3_PACKET */

    if (InitializeUtility(TRAP_SEND_COMMUNICATIONS,&argc, argv,
                          NULL, NULL, usage_string)) {
        return_value = -1;
        goto finished;
    }

    if (argc < 2) {
        fprintf(stderr, usage_string, argv[0], argv[0]);
        return_value = -1;
        goto finished;
    }


    /* start a PDU */


    switch (dst_trans) {
        default:
            /* get the local IP address here */
            local_ip_addr = GetLocalIPAddress();
            break;
    }

    /* convert ip addr to os_ptr */
    sprintf(buffer, "%02x %02x %02x %02x",
                    (unsigned char) ((local_ip_addr >> 24) & 0xFF),
                    (unsigned char) ((local_ip_addr >> 16) & 0xFF),
                    (unsigned char) ((local_ip_addr >> 8) & 0xFF),
                    (unsigned char) (local_ip_addr & 0xFF));
    os_ptr = MakeOctetStringFromHex(buffer);

#ifndef SR_SNMPv2_PDU

    cc = sscanf(argv[1], INT32_FORMAT, &generic_trap);
    if (cc != 1) {
        fprintf(stderr,"%s unable to parse trap type = %s\n", argv[0], argv[1]);
        fprintf(stderr, "using generic_trap = 0\n");
        generic_trap = 0;
    }
    if ((generic_trap < 0) || (generic_trap > 6)) {
        fprintf(stderr, "type must be in the range 1-6\n");
        return_value = -1;
        goto finished;
    }
    if (argc > 2) {
        cc = sscanf(argv[2], INT32_FORMAT, &specific_trap);
        if (cc != 1) {
            fprintf(stderr, "%s unable to parse specific-trap = %s\n",
                    argv[0], argv[2]);
            fprintf(stderr, "using specific_trap = 0\n");
            specific_trap = 0;
        }
        if (argc > 3) {
            enterprise = MakeOIDFromDot(argv[3]);
            if (enterprise == NULL) {
                fprintf(stderr, "%s: enterprise unknown: %s\n",
                        argv[0], argv[3]);
                return_value = -1;
                goto finished;
            }
        }
        else {
            enterprise = NULL;
        }
    }
    else {
        specific_trap = 0;
        enterprise = NULL;
    }
    if ((generic_trap == 6) && (enterprise == NULL)) {
        fprintf(stderr,
                "%s: generic trap 6 requires specific trap and enterprise\n",
                argv[0]);
        return_value = -1;
        goto finished;
    }
    if (argc > 4) {
        cc = sscanf(argv[4], UINT32_FORMAT, &timeticks);
        if (cc != 1) {
            fprintf(stderr, "%s unable to parse sysUpTime = %s\n",
                    argv[0], argv[4]);
            fprintf(stderr, "using sysUpTime = 0\n");
            timeticks = 0;
        }
    }
    else {
        timeticks = 0;
    }
    if (enterprise == NULL) {
        enterprise = MakeOIDFromDot("1.3.6.1.3.1.42.42.42.42");
    }
    pdu_ptr = MakePdu(TRAP_TYPE,
                      (SR_INT32)0,
                      (SR_INT32)0,
                      (SR_INT32)0,
                      enterprise,
                      os_ptr,
                      generic_trap,
                      specific_trap,
                      timeticks);
    if (pdu_ptr == NULL) {
        fprintf(stderr, "MakePdu failed\n");
        return_value = -1;
        goto finished;
    }
    enterprise = NULL;

    i = 5;

#else  /* SR_SNMPv2_PDU */

#ifdef SR_SNMPv1_PACKET
    if (util_version == SR_SNMPv1_VERSION) {
        cc = sscanf(argv[1], INT32_FORMAT, &generic_trap);
        if (cc != 1) {
            fprintf(stderr,"%s unable to parse trap type = %s\n", argv[0], argv[1]);
            fprintf(stderr, "using generic_trap = 0\n");
            generic_trap = 0;
        }
        if ((generic_trap < 0) || (generic_trap > 6)) {
            fprintf(stderr, "type must be in the range 1-6\n");
            return_value = -1;
            goto finished;
        }
        if (argc > 2) {
            cc = sscanf(argv[2], INT32_FORMAT, &specific_trap);
            if (cc != 1) {
                fprintf(stderr, "%s unable to parse specific-trap = %s\n",
                        argv[0], argv[2]);
                fprintf(stderr, "using specific_trap = 0\n");
                specific_trap = 0;
            }
            if (argc > 3) {
                enterprise = MakeOIDFromDot(argv[3]);
                if (enterprise == NULL) {
                    fprintf(stderr, "%s: enterprise unknown: %s\n",
                            argv[0], argv[3]);
                    return_value = -1;
                    goto finished;
                }
            }
            else {
                enterprise = NULL;
            }
        }
        else {
            specific_trap = 0;
            enterprise = NULL;
        }
        if ((generic_trap == 6) && (enterprise == NULL)) {
            fprintf(stderr,
                    "%s: generic trap 6 requires specific trap and enterprise\n",
                    argv[0]);
            return_value = -1;
            goto finished;
        }
        if (argc > 4) {
            cc = sscanf(argv[4], UINT32_FORMAT, &timeticks);
            if (cc != 1) {
                fprintf(stderr, "%s unable to parse sysUpTime = %s\n",
                        argv[0], argv[4]);
                fprintf(stderr, "using sysUpTime = 0\n");
                timeticks = 0;
            }
        }
        else {
            timeticks = 0;
        }

        if (enterprise == NULL) {
            enterprise = MakeOIDFromDot("1.3.6.1.3.1.42.42.42.42");
        }
        pdu_ptr = MakePdu(TRAP_TYPE,
                          (SR_INT32)0,
                          (SR_INT32)0,
                          (SR_INT32)0,
                          enterprise,
                          os_ptr,
                          generic_trap,
                          specific_trap,
                          timeticks);
        if (pdu_ptr == NULL) {
            fprintf(stderr, "MakePdu failed\n");
            return_value = -1;
            goto finished;
        }
        enterprise = NULL;
        i = 5;
        goto made_pdu;
    }
#endif /* SR_SNMPv1_PACKET */

    enterprise = MakeOIDFromDot(argv[1]);
    if (enterprise == NULL) {
        fprintf(stderr, "%s: TrapOID unknown: %s\n", argv[0], argv[1]);
        return_value = -1;
        goto finished;
    }
    if (argc > 2) {
        cc = sscanf(argv[2], UINT32_FORMAT, &timeticks);
        if (cc != 1) {
            fprintf(stderr, "%s unable to parse timeticks = %s\n",
                    argv[0], argv[2]);
            fprintf(stderr, "using timeticks = 0\n");
            timeticks = 0;
        }
    }

    /*
     * In an agent, would check to make sure snmpTrap and snmpTraps are
     * included in our view here.
     */

    pdu_ptr = MakePdu(SNMPv2_TRAP_TYPE,
                      (SR_INT32)++global_req_id,
                      (SR_INT32)0,
                      (SR_INT32)0,
                      (OID *) NULL,
                      (OctetString *)NULL,
                      (SR_INT32)0,
                      (SR_INT32)0,
                      (SR_UINT32)0);
    if (pdu_ptr == NULL) {
        DPRINTF((APWARN, "%s cannot make pdu.\n", Fname));
        goto cleanup;
    }

    /*
     * The first varbind is sysUpTime.0 and the second varbind is
     * snmpTrapOID.0.
     */
    if ((oid_ptr = MakeOIDFromDot("sysUpTime.0")) == NULL) {
        DPRINTF((APWARN, "trapsend: Could not make oid_ptr\n"));
        goto cleanup;
    }
    if ((vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL,
                                       (unsigned char) TIME_TICKS_TYPE,
                                       (void *) &timeticks)) == NULL) {
        DPRINTF((APWARN, "trapsend: Could not make vb_ptr\n"));
        goto cleanup;
    }
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    if (LinkVarBind(pdu_ptr, vb_ptr) == (short) -1) {
        DPRINTF((APWARN, "trapsend: Cannot link varbind with pdu_ptr\n"));
        goto cleanup;
    }
    vb_ptr = NULL;

    if ((oid_ptr = MakeOIDFromDot("snmpTrapOID.0")) == (OID *) NULL) {
        DPRINTF((APWARN, "trapsend: unable to make oid_ptr\n"));
        goto cleanup;
    }
    if ((vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL,
                                       (unsigned char) OBJECT_ID_TYPE,
                                       (void *) enterprise)) == NULL) {
        DPRINTF((APWARN, "trapsend: Could not make vb_ptr\n"));
        goto cleanup;
    }
    enterprise = NULL;
    FreeOID(oid_ptr);
    oid_ptr = NULL;
    if (LinkVarBind(pdu_ptr, vb_ptr) == (short) -1) {
        DPRINTF((APWARN, "trapsend: Cannot link varbind with pdu_ptr\n"));
        goto cleanup;
    }
    vb_ptr = NULL;

    i = 3;

  made_pdu:
#endif /* SR_SNMPv2_PDU */

    oid_ptr = NULL;
    vb_ptr = NULL;
    os_ptr = NULL;


    /* put additional VarBinds onto the VarBindList */
    if (i < argc) {
        while (argv[i] != NULL) {
            if ((vb_ptr = BuildNextVarBind(argv, &i)) == NULL) {
                fprintf(stderr, usage_string, argv[0], argv[0]);
                return_value = 1;
                goto finished;
            }

            /* COPY VarBind into PDU */
            if (LinkVarBind(pdu_ptr, vb_ptr) == (short) -1) {
                DPRINTF((APWARN, "trapsend: Cannot link varbind with pdu_ptr\n"));
                goto cleanup;
            }
        }
        vb_ptr = NULL;
    }

    if (BuildPdu(pdu_ptr) == -1) {
      DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
      goto finished;
    }

    PerformRequest(pdu_ptr, &status);
    if (status != 0) {
        fprintf(stderr, "%s:  send failed\n", argv[0]);
        /* clean up time */
        goto cleanup;
    }


cleanup:
    /* clean up time */
    if (pdu_ptr != NULL) {
        FreePdu(pdu_ptr);
        pdu_ptr = NULL;
    }
    if (os_ptr != NULL) {
        FreeOctetString(os_ptr);
        os_ptr = NULL;
    }
    if (oid_ptr != NULL) {
        FreeOID(oid_ptr);
        oid_ptr = NULL;
    }
    if (oid_ptr2 != NULL) {
        FreeOID(oid_ptr2);
        oid_ptr2 = NULL;
    }
    if (vb_ptr != NULL) {
        FreeVarBind(vb_ptr);
        vb_ptr = NULL;
    }
    if (enterprise != NULL) {
        FreeOID(enterprise);
        enterprise = NULL;
    }
  finished:
    return return_value;
}
