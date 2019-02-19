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
 * gettab.c - program to send get-bulk requests and process get-responses
 * until the entire table has been retrieved.  If a community string is
 * supplied for the community string/username field, this degenerates to a 
 * getmany.
 *
 * This utility needs a copy of snmpinfo.dat in the SR_MGR_CONF_DIR
 * (an environment variable) directory to run.
 */

/*
gettab.c

        Gettab is an SNMP application that retrieves an entire table using
         getbulk requests (or if it is a trivial get, it does a getmany) and
        uses the parts of the response for the next request. The arguments
        are the entity's address, the community string access to the SNMP
        entity or a USM username, and the table name is expressed as object 
        identifiers in either dot-notation or as the mib-variables as they
        appear in the MIB document. Gettab retrieves chunks of the
        table with each request and uses part of the response to form
        the next request. For instance, running the following:

        gettab -v3 localhost Guest ifTable         (SNMPv3 username)

                      or

        gettab -v[1|2c] localhost woof ifTable     (SNMPv1 community string)

        would retrieve the interface table.

*/

#include "sr_utils.h"
#include "diag.h"
SR_FILENAME
#include "prnt_lib.h"
#include "oid_lib.h"
#include "sri/vbtable.h"

extern int print_packet_stats;


#define MAX_REQS 1000L			/* Max repetitions in GetBulk */

static int request_type;
/*
 *  Prototypes for local functions
 */
static Pdu *do_lookup(char *tablename, int maxRepetitions);
static void print_list(VarBind *vblist);

/* ---------------------------------------------------------------------- */

int
main(
    int             argc,
    char           *argv[]
)
{
    Pdu            *requestPdu = NULL, *responsePdu = NULL;
    Pdu            *tmpRequestPdu = NULL;
    int             status;
    char           *usage_string;
    int            return_value = 0;	/* program return value */
    vbTable        *vbt;		/* Table where results are stored */
    VarBind        *vblist, *vb, *nextvb, *printVbList;   /* varBind ptrs */
    VarBind        *full_resp_vblist = NULL, *vblist_end;
    int 	   i;			/* Every program has an i */
    int		   allRowsDone = 0;	/* Flag set by vbTableAdd */
    int		   nonRepeaters = 0;	/* vbTableInitialize argument */
    int		   rows = -1;		/* vbTableInitialize argument */
    int            maxRows = -1;	/* vbTableInitialize argument */
    int            maxRepetitions;	/* GetBulk parameter */
    int            num_resp_pkts = 0;
    int            objects_in_scope = 0;
    int            total_objects = 0;

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
        "\tagent_addr community/userName table_name . . .\n\n"
        "For example, table_name can be:\n"
        "\t\t atTable\n"
        "\t\t ipAddrTable\n"
        "\t\t ipRouteTable\n"
        "\t\t ipNetToMediaTable\n"
        "\t\t tcpConnTable\n"
        "\t\t udpTable\n"
        "\t\t egpNeighTable\n"
#ifdef SR_SYS_OR_TABLE
        "\t\t snmpORTable\n"
#endif /* SR_SYS_OR_TABLE */
#if (defined(SR_SNMPv1_WRAPPER) && defined(SR_SNMPv3_ADMIN))
        "\t\t snmpCommunityTable\n"
#endif	/* (defined(SR_SNMPv1_WRAPPER) && defined(SR_SNMPv3_ADMIN)) */
#ifdef SR_SNMPv3_PACKET
        "\t\t usmUserTable\n"
#endif /* SR_SNMPv3_PACKET */
#ifdef SR_SNMPv3_ADMIN
        "\t\t vacmSecurityToGroupTable\n"
        "\t\t vacmAccessTable\n"
#endif /* SR_SNMPv3_ADMIN */
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


#ifndef SR_SNMPv2_PDU
    request_type = GET_NEXT_REQUEST_TYPE;
#else /* SR_SNMPv2_PDU */
#ifdef SR_SNMPv1_PACKET
    if (util_version == SR_SNMPv1_VERSION) {
        request_type = GET_NEXT_REQUEST_TYPE;
    } else {
        request_type = GET_BULK_REQUEST_TYPE;
    }
#else /* SR_SNMPv1_PACKET */
    request_type = GET_BULK_REQUEST_TYPE;
#endif /* SR_SNMPv1_PACKET */
#endif /* SR_SNMPv2_PDU */

    if (request_type == GET_NEXT_REQUEST_TYPE) {
        maxRepetitions = 0;
    } else {
        maxRepetitions = MAX_REQS;
    }

    /*
     * Find all elements of this tablename and put them into a varbind_list
     * in a PDU.  If multiple tables are given, they should have the
     * same instancing.  
     */
    for (i = 1; i < argc; i++) {
       if ((tmpRequestPdu = do_lookup(argv[i], maxRepetitions)) == NULL) {
           fprintf(stderr, "gettab: do_lookup failed\n");
           exit(-1);
       }
       if (requestPdu != NULL) {
           requestPdu->var_bind_end_ptr->next_var = 
               tmpRequestPdu->var_bind_list;
           requestPdu->var_bind_end_ptr = 
               tmpRequestPdu->var_bind_end_ptr;
           tmpRequestPdu->var_bind_list = NULL;
           FreePdu(tmpRequestPdu);
       } else {
           requestPdu = tmpRequestPdu;
       }
    }


    vbt = vbTableInitialize(
        requestPdu->var_bind_list, nonRepeaters, NULL, rows, maxRows);
    if (!vbt) {
       fprintf(stderr, "vbTableInitialize failed.  Giving up.\n");
       exit(-1);
    }

    while (1) {
        retries = tretries;
/*
 *  Clean up the previous responsePdu.  Since the varBinds have
 *  all been consumed by vbTableAdd(), null out the 
 *  varbind pointer so the varbinds won't get whacked.
 */
        if (responsePdu != NULL) {
             responsePdu->var_bind_list = NULL;
             responsePdu->var_bind_end_ptr = NULL;
             FreePdu(responsePdu);
        }

retry_snmp_request:
        /* Perform SNMP request */
        responsePdu = PerformRequest(requestPdu, &status);
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

        if (responsePdu->type != GET_RESPONSE_TYPE) {
            fprintf(stderr, 
                    "received non GET_RESPONSE_TYPE packet.  Exiting.\n");
            exit(-1);
        }
        num_resp_pkts++;
        DPRINTF((APTRACE, "%d varbinds received from agent.\n", 
                 responsePdu->num_varbinds));
        if (print_packet_stats) {
           if (full_resp_vblist == NULL) {
              full_resp_vblist = vblist_end = CloneVarBindList(responsePdu->var_bind_list);
           } else {
              while(vblist_end->next_var != NULL) {
                   vblist_end = vblist_end->next_var;
              }
              vblist_end->next_var = CloneVarBindList(responsePdu->var_bind_list);
              vblist_end = vblist_end->next_var;
           }
        }
 

/*
 *  Add response received to table, print any pending responses ready
 */
        status = vbTableAdd(vbt, responsePdu->var_bind_list, 
            responsePdu->u.normpdu.error_status,
            responsePdu->u.normpdu.error_index, &vblist);
        if (status < 0) {
            DPRINTF((APERROR, "vbTableAdd failed: %s\n", 
                     vbTableErrorStrings(status)));
            exit(-1);
        } 


        while ((printVbList = vbTableRetrieveRows(vbt, &allRowsDone)) != NULL) {
            DPRINTF((APTRACE, "Just called vbTableRetrieveRows\n"));
            print_list(printVbList);
            if (print_packet_stats) {
                PrintResponseStats(GET_BULK_REQUEST_TYPE, full_resp_vblist, num_resp_pkts);
                objects_in_scope += NonNullElementsInVarBindList(printVbList);
                total_objects += NonNullElementsInVarBindList(full_resp_vblist);
                printf("    Total Overrun:  %d objects\n", total_objects - objects_in_scope);
            }
            printf("\n");
            FreeVarBindList(printVbList);
        }

        if ((responsePdu->u.normpdu.error_status != NO_ERROR) &&
            (responsePdu->u.normpdu.error_status != NO_ACCESS_ERROR) &&
            (responsePdu->u.normpdu.error_status != NO_SUCH_NAME_ERROR)) {
            printf("Error seen on final protocol operation:\n   ");
            PrintErrorCode(responsePdu->u.normpdu.error_index,
                             responsePdu->u.normpdu.error_status);
        }

        if (status != VBT_OK || allRowsDone != 0) {
            /*
             *  Cleanup: the varbinds have already been consumed by the
             *  varbind table, so null out the pointers in the pdu before
             *  cleaning it up.
             */
            vbTableFree(vbt);
            responsePdu->var_bind_list = NULL;
            responsePdu->var_bind_end_ptr = NULL;
            FreePdu(responsePdu);
            FreePdu(requestPdu);
            if (vblist != NULL) {
                FreeVarBindList(vblist);
            }
            goto finished;
        }

        global_req_id++;

/* build up new request packet here 
 * For now, plug in req_id for the request id.  When
 * this pdu is actually used, the req_id will be updated correctly.
 */
        FreePdu(requestPdu);
        requestPdu = MakePdu(request_type, global_req_id, 0L,
            maxRepetitions, NULL, NULL, 0L, 0L, 0L);

        if (requestPdu == NULL) {
            fprintf(stderr, "Could not create new request PDU\n");
            exit(-1);
        }
/*
 *  Move the varBind list returned by vbTableAdd into the PDU.
 */
        for (vb = vblist; vb != NULL; vb = nextvb) {
            nextvb = vb->next_var;		/* save link */
            LinkVarBind(requestPdu, vb);
        }
/*
 *  ASN1ize the packet.
 */
        if (BuildPdu(requestPdu) == -1) {
            DPRINTF((APALWAYS, "BuildPdu failed.  Giving up.\n"));
            goto finished;
        }
    }                                /* while(1) */

finished:
    FreeVarBindList(full_resp_vblist);
    return return_value;
}                                /* main */

/* ----------------------------------------------------------------------
 * Look up the tablename in snmpinfo.dat.  If it exists and is an
 * Aggregate (table), collect all entries in the table into a varbind_list.
 * This could be a GET-NEXT (trivial) or a GET-BULK request.
 */
static Pdu            *
do_lookup(
    char           *tablename,
    int            maxRepetitions
)
{

    OID            *oid_ptr = NULL;
    OID            *tableOID = NULL;
    OID            *nameOID = NULL;
    VarBind        *vb_ptr = NULL;
    Pdu            *req_pdu_ptr = NULL;
    int            errorSeen = TRUE;
    FILE           *fp = NULL;
    int             found = FALSE;
    char            name[100];
    char            oid_val[100];
    char            type[40];
    char            access[40];
    char            buffer[256];


    found = FALSE;
    

    req_pdu_ptr = MakePdu(request_type, global_req_id, 0L, 
                          maxRepetitions, NULL, NULL, 0L, 0L, 0L);

    /*
     * Open file, search for the tablename specified in snmpinfo.dat
     */

    if ((fp = fopen(fn_snmpinfo_dat, "r")) == NULL) {
        fprintf(stderr, "do_lookup: could not open snmpinfo.dat\n");
        goto cleanup;
    }

    while ((found == FALSE) && (fgets(buffer, sizeof(buffer), fp) != NULL)) {
        if (sscanf(buffer, "%s %s %s %s", name, oid_val, type, access) != 4) {
            continue;
        }

        if ((strcmp(tablename, name) == 0) && 
            (strcmp(type, "Aggregate") == 0)) {
            found = TRUE;
        }
    }

    if (found == FALSE) {
        fprintf(stderr, "do_lookup: could not locate %s in snmpinfo.dat\n",
                tablename);
        goto cleanup;
    }

    /*
     * Now search for every column in this conceptual row and add it to the
     * pdu.
     */

    found = FALSE;
    while ((found == FALSE) && (fgets(buffer, sizeof(buffer), fp) != NULL)) {
        if (sscanf(buffer, "%s %s %s %s", name, oid_val, type, access) != 4) {
            continue;
        }
        if ((strcmp(type, "Aggregate") != 0) &&
            (strcmp(access, "not-accessible") != 0)) {
            found = TRUE;
        }
    }

    if (found == FALSE) {
        fprintf(stderr, 
            "do_lookup: could not find objects for %s in snmpinfo.dat\n",
            tablename);
        goto cleanup;
    }

    if ((oid_ptr = MakeOIDFromDot(oid_val)) == (OID *) NULL) {
        fprintf(stderr, "do_lookup: cannot make oid_ptr\n");
        goto cleanup;
    }

    if ((vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL)) == 
        (VarBind *) NULL) {
        fprintf(stderr, "do_lookup: MakeVarBindWithNull failed\n");
        goto cleanup;
    }
    FreeOID(oid_ptr);
    oid_ptr = NULL;			/* linked into vb_ptr */

    if (LinkVarBind(req_pdu_ptr, vb_ptr) == (short) -1) {
        fprintf(stderr, "do_lookup: could not link varbind\n");
        goto cleanup;
    }
    vb_ptr = NULL;			/* linked into req_pdu_ptr */

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if ( sscanf(buffer, "%s %s %s %s", name, oid_val, type,
                          access) != 4) {
            continue;
        }

        if (strcmp(access, "not-accessible") != 0) {
            /*
             * Add this table element to the varbind_list
             */
            if ((tableOID = MakeOIDFromDot(tablename)) == (OID *) NULL) {
                fprintf(stderr, "do_lookup: Could not make tableOID\n");
		goto cleanup;
            }
            if ((nameOID = MakeOIDFromDot(name)) == (OID *) NULL) {
                fprintf(stderr, "do_lookup: Could not make nameOID\n");
		goto cleanup;
            }
            if (CmpOIDClass(tableOID, nameOID) == 0) {
                if ((oid_ptr = MakeOIDFromDot(oid_val)) == (OID *) NULL) {
                    fprintf(stderr, "do_lookup: cannot make oid_ptr 2\n");
		    goto cleanup;
                }

                if ((vb_ptr = MakeVarBindWithNull(oid_ptr, (OID *) NULL)) ==
                    (VarBind *) NULL) {
                    fprintf(stderr, "do_lookup: cannot make vb_ptr 2\n");
		    goto cleanup;
                }
                FreeOID(oid_ptr);
                oid_ptr = NULL;

                if (LinkVarBind(req_pdu_ptr, vb_ptr) == (short) -1) {
                    fprintf(stderr, "do_lookup: could not link varbind 2\n");
		    goto cleanup;
                }
                vb_ptr = NULL;		/* now in req_pdu_ptr */
            }                        /* if CmpOIDClass(tableOID, ..... ) */

            else {                /* Have gotten all of the elements of the
                                 * table */
                break;
            }
            FreeOID(tableOID);
            tableOID = NULL;
            FreeOID(nameOID);
            nameOID = NULL;
        }                  /* if (strcmp(access, "not-accessible") != 0) */
    }                      /* while (fgets(....) != NULL */

    if (BuildPdu(req_pdu_ptr) == -1) {
        fprintf(stderr, "BuildPdu failed.  Giving up.\n");
        goto cleanup;
    }
    errorSeen = FALSE;

cleanup:
    if (oid_ptr != NULL) {
        FreeOID(oid_ptr);
    }
    if (tableOID != NULL) {
        FreeOID(tableOID);
    }
    if (nameOID != NULL) {
        FreeOID(nameOID);
    }
    if (vb_ptr != NULL) {
        FreeVarBindList(vb_ptr);
    }
    if (fp != NULL) {
        fclose(fp);
    }

    if (errorSeen) {
       FreePdu(req_pdu_ptr);
       return NULL;
    } else {
	return (req_pdu_ptr);
    }
}                                /* do_lookup() */
/* ----------------------------------------------------------------------
 *
 *  print_list - print out a list of VarBinds.
 *
 *  This module written only to maintain output compatibility with
 *  pre-vbtable version of gettab.  
 *
 *  Arguments:
 *  i	(VarBind *) vblist
 *	List of variable bindings to print
 */
static void
print_list(
   VarBind *vblist
)
{
    VarBind *vb;
    static char *buf = NULL;

    for (vb = vblist; vb != NULL; vb = vb->next_var) {
        if (vb->value.type == NULL_TYPE) {
            if (buf == NULL) {
                buf = (char *) malloc(4096);
                if (!buf) {
                    DPRINTF((APALWAYS, "malloc failure on buffer size 4096\n"));
                    exit(1);
                }
            }
            if (PrintVarBind_format == 0) {
                if (MakeDotFromOID(vb->name, buf) == -1) {
                    DPRINTF((APERROR, "MakeDotFromOID failed\n"));
                    return;
                }
            } else {
                if (MakeHFDotFromOID(vb->name, buf, 4096,
                                     PrintVarBind_format) < 0) {
                    DPRINTF((APWARN, "MakeHFDotFromOID failed\n"));
                    return;
                }
            }

            printf("%s = Unavailable\n", buf);
        } else {
            if (PrintVarBind_format == 0) {
                PrintVarBind(vb);
            } else {
                PrintFormattedVarBind(vb, NULL, PrintVarBind_format, 0);
            }

        }
    }
    return;
}

/* eof */
