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

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "mibout.h"
#include "method.h"
#include "makevb.h"
#include "diag.h"
SR_FILENAME

/*----------------------------------------------------------------------
 * Retrieve data from the demo group. This is performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be reuturned.
 *----------------------------------------------------------------------*/
VarBind        *
mtrDemoGroup_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    VarBind        *vb = (VarBind *) NULL;
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    mtrDemoGroup_t *data;
    OID            *oid_ptr;
    void           *dp;

    static OctetString *nsap_ptr = NULL;

    UInt64         *a_big_number;

    a_big_number = NULL;

    if (nsap_ptr == NULL) {
	nsap_ptr = MakeOctetStringFromText("NS+47000580ffff000000012301230123456789ab01");
    }

    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 1 and the single
     * instance element be 0.
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically preceeed the current object type.
     */

    switch (searchType) {
    case EXACT:
	if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {
	    arg = object->nominator;
	}
	break;

    case NEXT:
	if (instLength <= 0) {
	    arg = object->nominator;
	}
	break;

    default:
	DPRINTF((APALWAYS, "snmpd: Internal error. (invalid search type in "));
	DPRINTF((APALWAYS, "snmp_get -- %d\n", searchType));
    }				/* switch */



    switch (arg) {
    case I_mtrBigNumber:
	if ((a_big_number = (void *) malloc(sizeof(UInt64))) == NULL) {
	    DPRINTF((APWARN, "mtrDemoGroup_get: Cannot malloc mtrBigNumber.\n"));
	    return ((VarBind *) NULL);
	}
	a_big_number->big_end = 4196413045;
	a_big_number->little_end = 1340680469;
	dp = (void *) a_big_number;
	break;
    case I_mtrNsapAddress:
	if ((dp = MakeOctetString(nsap_ptr->octet_ptr, nsap_ptr->length)) == NULL) {
	    DPRINTF((APWARN, "mtrDemoGroup_get: Cannot malloc mtrNsapAddress.\n"));
	    return ((VarBind *) NULL);
	}
	break;
    case I_mtrBitString:
	if ((dp = MakeBitString("\000\000", 2, 0)) == NULL) {
	    DPRINTF((APWARN, "mtrDemoGroup_get: Cannot malloc mtrBitString.\n"));
	    return ((VarBind *) NULL);
	}
	break;
    default:
	DPRINTF((APTRACE, "mtrDemoGroup_get: Illegal variable.\n"));
	return ((VarBind *) NULL);
    }				/* switch */

    return (MakeVarBind(object, &ZeroOid, dp));
}


#ifdef SETS
/*----------------------------------------------------------------------
 * Free the demo data object.
 *----------------------------------------------------------------------*/
static void
mtrDemoGroup_free(data)
    mtrDemoGroup_t *data;
{
    free(data);
}

/*----------------------------------------------------------------------
 * Cleanup after mtrDemoGroup set/undo.
 *----------------------------------------------------------------------*/
static int
mtrDemoGroup_cleanup(trash)
    doList_t       *trash;
{
    mtrDemoGroup_free(trash->data);
    mtrDemoGroup_free(trash->undodata);
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET requet to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *----------------------------------------------------------------------*/
int
mtrDemoGroup_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    VarBind        *vb = (VarBind *) NULL;
    int             inst_length = incoming->length - object->oid.length;

    /*
     * Validate the object instance: 1) It must be of length 1 2) and the
     * instance must be 0.
     */

    if (inst_length != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
	DPRINTF((APTRACE, "snmpd: Invalid instance\n"));
	return (NO_CREATION_ERROR);
    }

    /*
     * Add the SET request to the do-list element. Test the validity of the
     * value at this point.
     */

    (mtrDemoGroup_t *) (doCur->data) = (mtrDemoGroup_t *) malloc(sizeof(mtrDemoGroup_t));
    if (doCur->data == (mtrDemoGroup_t *) NULL) {
	DPRINTF((APALWAYS, "snmpd: cannot allocate memory\n"));
	return (GEN_ERROR);
    }

    doCur->setMethod = mtrDemoGroup_set;
    doCur->cleanupMethod = mtrDemoGroup_cleanup;
    doCur->state = SR_UNKNOWN;

    switch (object->nominator) {
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator "));
	DPRINTF((APALWAYS, "in mtrDemoGroup_test)\n"));
	return (GEN_ERROR);
    }				/* switch */

    doCur->state = SR_ADD_MODIFY;

    return (NO_ERROR);
}


/*----------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *
 * Note: In this case all related items are contained in the
 *    structure pointed to by "doCur->data," but in the more
 *    general case, this routine may need to scan the do-list to
 *    determine if there are other "related" objects. This is
 *    not recommended but is included for maximum flexibility.
*----------------------------------------------------------------------*/
int
mtrDemoGroup_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{


    return (NO_ERROR);
}

#endif
