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

/* define SR_EMWEB_EPIC here to run EMWEB examples */

#include "sr_conf.h"
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "sr_time.h"

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_snmp.h"
#include "prnt_lib.h"
#include "sri/vbtable.h"
#include "epic.h"
#include "epiccore.h"
#include "diag.h"
SR_FILENAME
#include "sitedefs.h"
#include "mib_tabl.h"
#include "snmp-mib.h"


static  EpicAuthHandle *eah;  /* globalize across module */

/*
 * For VxWorks the entry point will be unique so as not to confuse snmpd
 * with other "main" routines.  The VxWorks shell does no support argc/argv
 * All functions are called with up to ten arguments.  Unused arguments are
 * passed in with zero (0) value.
 *
 */
int
td_main(arg1_in, arg2_in, arg3_in, arg4_in, arg5_in,
	   arg6_in, arg7_in, arg8_in, arg9_in, arg10_in)
    char           *arg1_in, *arg2_in, *arg3_in, *arg4_in, *arg5_in;
    char           *arg6_in, *arg7_in, *arg8_in, *arg9_in, *arg10_in;
{
     void printMeHere(EpicCallerHandle handle,
         VarBind *vb,
         SR_INT32 status,
         SR_INT32 errorIndex,
         EpicSession *sd);
     VarBind *vb, *vb2;
     int status;
     OID *object, *object2;
     void *val;
     char answer[5];

     printf("To test set functionality, enter \"s\", otherwise just press return:");
     fgets(answer,sizeof(answer),stdin);
     if (tolower(answer[0]) == 's') goto set;

#ifdef SR_EMWEB_EPIC
/*
 *  The following eah is allocated simply to verify the operation
 *  works.  It is never used in this test.
 */
     if (!(eah = createEpicAuthHandle(EMWEB_EPIC_SECURITY_MODEL, 
            EMWEB_EPIC_DEFAULT_USER))) {
        printf("td: createEpicAuthHandle returned null\n");
        exit(1);
     }
#endif /* SR_EMWEB_EPIC */
     if (!(eah = makeEpicAuthHandle(EPIC_SECURITY_MODEL,  
            EPIC_DEFAULT_USER, NULL, NULL))) {
        printf("td: makeEpicAuthHandle returned null\n");
        exit(1);
     }

     /* start at mib2 system group */
     if (!(object = MakeOIDFromDot("1.3.6.1.2.1.1")))
     {
        printf("td: MakeOIDFromDot returned null\n");
        exit(1);
     }
     if (!(vb = MakeVarBindWithNull(object, (OID *) NULL))) {
        printf("td: MakeVarBindWIthNull returned null\n");
        exit(1);
     }
     status = doEpicOperation(
          GET_NEXT_REQUEST_TYPE, 
/*              GET_REQUEST_TYPE, */
          (EpicCallerHandle) "my epic handle",
          EPIC_DEFAULT_CONTEXT,
          eah,
          SR_EPIC_SEC_LEVEL_MAX,
          vb,
          printMeHere);

     if (status != DEO_OK) {
          printf("doEpicOperation failed w/ status %d\n", status);
          exit(1);
     }
     exit(0);

/* --- set test --- */
set:
#ifndef SR_EMWEB_EPIC
     if (!(eah = makeEpicAuthHandle(EPIC_SECURITY_MODEL, "epicAdmin", NULL, NULL))) {
        printf("td: makeEpicAuthHandle returned null\n");
        exit(1);
     }
#else /* SR_EMWEB_EPIC */
     if (!(eah = makeEpicAuthHandle(EMWEB_EPIC_SECURITY_MODEL, "emwebAdmin", NULL, NULL))) {
        printf("td: makeEpicAuthHandle returned null\n");
        exit(1);
     }
#endif /* SR_EMWEB_EPIC */

/*
 *  Try setting sysName.0
 */
     if (!(object2 = MakeOIDFromDot("1.3.6.1.2.1.1.5.0")))
     {
        printf("td: MakeOIDFromDot returned null\n");
        exit(1);
     }
     if (!(val = MakeOctetStringFromText("sysName.0 set test"))) {
        printf("td: MakeOctetStringFromText returned null\n");
        exit(1);
     }

     if (!(vb2 = MakeVarBindWithValue(object2, 0, OCTET_PRIM_TYPE, val))) {
        printf("td: MakeVarBindWithNull returned null\n");
        exit(1);
     }

     status = doEpicOperation(
          SET_REQUEST_TYPE, 
          (EpicCallerHandle) "a set request",
          EPIC_DEFAULT_CONTEXT,
          eah,
          SR_EPIC_SEC_LEVEL_MAX,
          vb2,
          printMeHere);

     exit(0);
}

/*
 *  call back routine for testing epic
 */
extern void printMeHere(EpicCallerHandle handle,
    VarBind *vb, SR_INT32 status,
    SR_INT32 errorIndex, EpicSession *sd)
{
    VarBind *newvbl;
    
    printf("---- callback called with the following arguments ---\n");
    printf("    status:     %d\n", status);
    printf("    errorIndex: %d\n", errorIndex);
    printf("    handle: %s\n", (char *) handle);
    printf("    varbind list:\n");
    PrintVarBindList(vb);
    
    switch (vb->value.type) {
    case NO_SUCH_OBJECT_EXCEPTION:
    case NO_SUCH_INSTANCE_EXCEPTION:
    case END_OF_MIB_VIEW_EXCEPTION:
        status = -1;		/* don't start new operation */
        break;
    default:
        break;
    }
/*
 *  If we are printing the result of a set request, don't try
 *  to keep doing the mib walk.
 */
    if (!strcmp(handle, "a set request")) {
        status = -2;
    }
    
    if (status == 0) {
        newvbl = CloneVarBindList(vb);
        FreeVarBindList(vb);
        status = doEpicOperation(
             GET_NEXT_REQUEST_TYPE,
             (EpicCallerHandle) "my epic handle",
             EPIC_DEFAULT_CONTEXT,
             eah,
             SR_EPIC_SEC_LEVEL_MAX,
             newvbl,
             printMeHere);
    
        if (status != DEO_OK) {
             printf("doEpicOperation failed w/ status %d\n", status);
             exit(1);
         }
    } 
    return;
}
