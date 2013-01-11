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

#ifndef SR_SR_UTILS_H
#define SR_SR_UTILS_H

#include "sr_conf.h"


#include <stdio.h>

#include <stdlib.h>


#include <sys/types.h>

#include <netinet/in.h>

#include <arpa/inet.h>


#include <string.h>


#include "sr_type.h"
#include "sr_time.h"
#include "sr_proto.h"
#include "sr_snmp.h"
#include "diag.h"
#include "util_def.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpio.h"
#include "mib_tree.h"

extern int       seconds;
extern int       retries; /* number of times to retry request */
extern int       tretries; /* number of times to retry request */
extern int       PrintVarBind_format;

/*
 * The following variables are used to maintain information about what
 * SNMP version is being used, details about authentication info for
 * the version in use, and other info used for sending/receiving SNMP
 * packets.
 */
extern int communication_type;
extern int util_version;
extern char *util_name;
extern char *util_dest;
extern char *util_handle;


extern char            fn_snmpinfo_dat[];


extern char            fn_mgr_file[];

extern SR_INT32 global_req_id;

#if defined(SR_SNMPv3_PACKET)
extern OctetString *defaultContextSnmpID;
extern OctetString *defaultContext;
#endif	/* defined(SR_SNMPv3_PACKET) */

int CheckEndOfMIB
    SR_PROTOTYPE((struct _Pdu *pdu_ptr));

struct _VarBind *CreateVarBind
    SR_PROTOTYPE((char *name,
                  char *type,
                  char *value));

struct _VarBind *BuildNextVarBind
    SR_PROTOTYPE((char **argv,
                  int *in_index));

int InitializeUtility
    SR_PROTOTYPE((int type,
                  int *argc,
                  char *argv[],
		  char *default_util_dest,
		  char *default_util_handle,
		  char *usage_string));

Pdu *PerformRequest
    SR_PROTOTYPE((Pdu *out_pdu_ptr,
                  int *status));

int GetVariableType
    SR_PROTOTYPE((OID_TREE_ELE *ote, char *type));

int GetEnumFromString
    SR_PROTOTYPE((OID_TREE_ELE *ote, char *value, SR_INT32 *number));



#if defined(SR_SNMPv3_PACKET)
#ifndef SR_UNSECURABLE
int get_password
    SR_PROTOTYPE((char *prompt, 
		  char *pw, 
		  int maxlen));
#endif /* SR_UNSECURABLE */

int InitializeV3User
    SR_PROTOTYPE((char *input, int communication_type));

Pdu *PerformV3UserRequest
    SR_PROTOTYPE((Pdu *out_pdu_ptr,
                  int *status));

int InitV3CLU(
    const PARSER_RECORD_TYPE *rt[],
    int *rt_count);

int PromptForV3UserPasswords
    SR_PROTOTYPE((OctetString *snmpID, 
		  OctetString *userName));

#endif	/* defined(SR_SNMPv3_PACKET) */

#ifdef SR_SNMPv1_WRAPPER
Pdu *PerformCommunityRequest
   SR_PROTOTYPE((Pdu *out_pdu_ptr, int *status));
#endif /* SR_SNMPv1_WRAPPER */

void PrintRouteInfo
    SR_PROTOTYPE((VarBind *vb_list_ptr));

#define SEND_TRAP 1
#define SEND_INFORM 2
void SetUtilTrapFlag(int type, unsigned short port);


int ElementsInVarBindList
     SR_PROTOTYPE((VarBind *vblist));

int NonNullElementsInVarBindList
     SR_PROTOTYPE((VarBind *vblist));

int PduComplete
     SR_PROTOTYPE((Pdu *pdu_ptr));


void PrintResponseStats(int req_type, VarBind *vblist, int packet_count);

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif /* SR_SR_UTILS_H */
