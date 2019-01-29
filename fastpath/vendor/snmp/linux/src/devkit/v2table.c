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

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>


#include <malloc.h>

#include <string.h>

#include "sr_snmp.h"
#include "lookup.h"
#include "v2table.h"
#include "diag.h"
SR_FILENAME

#include "oid_lib.h"

#define CmpInts(a,b) ((a)<(b)?-1:((a)>(b)))


static int CopyValues
    SR_PROTOTYPE((const int nominator, const SnmpType *typep, 
		  void  *newrec, const void *oldrec));

static void TransferValues
    SR_PROTOTYPE((const int nominator, const SnmpType *typep,
		  void *newrec, void *oldrec));

/* this routine compares values to see if they are equal */
int
CmpValues(nominator, typep, olddatap, newval)
const int nominator;
const SnmpType *typep;
const void *olddatap;
const ObjectSyntax *newval;
{
    int retval = 0;
    anytype_t *olddata = (anytype_t *) 
        ((char *) olddatap + typep[nominator].byteoffset);

    /* compare based on type */
    switch(typep[nominator].type) {
    /* signed 32-bit values */
    case INTEGER_TYPE:
        retval = CmpInts((SR_INT32) olddata->uint_val, newval->u.slval);
	break;

    /* unsigned 32-bit values */
    case COUNTER_TYPE:
    case GAUGE_TYPE:
    case TIME_TICKS_TYPE:
        retval = CmpInts(olddata->uint_val, newval->u.ulval);
	break;

    /* compound values */
    case OCTET_PRIM_TYPE:
	retval = CmpOctetStrings(olddata->octet_val, newval->u.osval);
	break;
    case OBJECT_ID_TYPE:
	retval = CmpOID(olddata->oid_val, newval->u.oidval);
	break;

    default:
	DPRINTF((APERROR, "CmpValues: can't handle type %d\n", 
		typep[nominator].type));
	return -1;
    }
    return(retval);
}

/* this routine writes a new value into a data structure */
static int
CopyValues(nominator, typep, newrec, oldrec)
const int nominator;
const SnmpType *typep;
void  *newrec;
const void *oldrec;
{
    int retval = 1;
    anytype_t *newvalue, *oldvalue;
/*
 *  Set up pointers
 */
    newvalue = (anytype_t *) ((unsigned long) newrec + 
        typep[nominator].byteoffset);
    oldvalue = (anytype_t *) ((unsigned long) oldrec + 
        typep[nominator].byteoffset);

    /* compare based on type */
    switch(typep[nominator].type) {
    /* 32-bit values */
    case INTEGER_TYPE:
    case COUNTER_TYPE:
    case GAUGE_TYPE:
    case TIME_TICKS_TYPE:
    case IP_ADDR_PRIM_TYPE:
	newvalue->uint_val = oldvalue->uint_val;
	break;

    /* compound values */
    case OCTET_PRIM_TYPE:
        if (newvalue->octet_val == NULL || oldvalue->octet_val == NULL ||
	    CmpOctetStrings(newvalue->octet_val, oldvalue->octet_val) != 0) {
            FreeOctetString(newvalue->octet_val);
            if (!(newvalue->octet_val = 
		  CloneOctetString(oldvalue->octet_val))) {
		retval = 0;
	    }
	}
	break;

    case OBJECT_ID_TYPE:
        if (newvalue->oid_val == NULL || oldvalue->oid_val == NULL ||
                CmpOID(newvalue->oid_val, oldvalue->oid_val) != 0) {
            FreeOID(newvalue->oid_val);
            if (!(newvalue->oid_val = CloneOID(oldvalue->oid_val))) {
		retval = 0;
	    }
	}
	break;

    default:
	DPRINTF((APERROR, "CopyValues: can't handle type %d\n", 
		typep[nominator].type));
	return -1;
    }

    return(retval);
}

/* this routine moves a datum from the old structure to the new one, clearing
 * the pointer to aggregate structures in the old data */
static void
TransferValues(nominator, typep, newrec, oldrec)
const int nominator;
const SnmpType *typep;
void *newrec;
void *oldrec;
{
    anytype_t *newvalue, *oldvalue;

/*
 *  Set up pointers
 */
    newvalue = (anytype_t *) ((unsigned long) newrec + 
        typep[nominator].byteoffset);
    oldvalue = (anytype_t *) ((unsigned long) oldrec + 
        typep[nominator].byteoffset);

    /* compare based on type */
    switch(typep[nominator].type) {
    /* 32-bit values */
    case INTEGER_TYPE:
    case COUNTER_TYPE:
    case GAUGE_TYPE:
    case TIME_TICKS_TYPE:
    case IP_ADDR_PRIM_TYPE:
        newvalue->uint_val = oldvalue->uint_val;
	break;

    /* compound values */
    case OCTET_PRIM_TYPE:
	if (newvalue->octet_val == NULL || oldvalue->octet_val == NULL ||  
               CmpOctetStrings(newvalue->octet_val, oldvalue->octet_val) != 0) {
	    FreeOctetString(newvalue->octet_val);
	    newvalue->octet_val = oldvalue->octet_val;
	    oldvalue->octet_val = NULL;
	}
	break;

    case OBJECT_ID_TYPE:
        if (newvalue->oid_val == NULL || oldvalue->oid_val == NULL ||
                CmpOID(newvalue->oid_val, oldvalue->oid_val) != 0) {
            FreeOID(newvalue->oid_val);
            newvalue->oid_val = oldvalue->oid_val;
            oldvalue->oid_val = NULL;
        }
	break;

    case COUNTER_64_TYPE:
        FreeUInt64( newvalue->uint64_val );
        newvalue->uint64_val = oldvalue->uint64_val; /* copy the pointer */
        oldvalue->uint64_val = NULL;
	break;

    default:
	DPRINTF((APERROR, "TransferValues: can't handle type %d\n", 
		typep[nominator].type));
	return;
    }
}

/* this routine copies all entries in a structure */
int
CopyEntries(maxNominator, typep, newvalue, oldvalue)
    const int maxNominator;
    const SnmpType *typep;
    void *newvalue;
    const void *oldvalue;
{
    int i;

    /* copy all data up to and including the max nominator */
    for(i = 0; i <= maxNominator; i++) {
        if(CopyValues(i, typep, newvalue, oldvalue) == 0) break;
    }
    return(i <= maxNominator ? 0 : 1);
}

/* this routine transfers all non-index entries in a structure */
void
TransferEntries(maxNominator, typep, newp, oldp)
const int maxNominator;
const SnmpType *typep;
void *newp;
void *oldp;
{
    int i;

    /* transfer all data up to and including the max nominator */
    for(i = 0; i <= maxNominator; i++) {
	if(typep[i].indextype == (-1)) {
	    TransferValues(i, typep, newp, oldp);
	}
    }
}

/* this routine frees all members of an entry */
void
FreeEntries(
    const SnmpType *typep,
    void *p)
{
    int i;
    anytype_t *value;

    if (p == NULL) {
        return;
    }

    /* free all data up to and including the max nominator */
    for(i = 0; typep[i].type != -1; i++) {
        value = (anytype_t *) ((unsigned long)p + typep[i].byteoffset);
        switch(typep[i].type) {
            case OCTET_PRIM_TYPE:
	        if (value->octet_val != NULL) {
	            FreeOctetString(value->octet_val);
	            value->octet_val = NULL;
	        }
	        break;
            case OBJECT_ID_TYPE:
                if (value->oid_val != NULL) {
                    FreeOID(value->oid_val);
                    value->oid_val = NULL;
                }
	        break;
            case COUNTER_64_TYPE:
                if (value->uint64_val != NULL) {
                    FreeUInt64(value->uint64_val);
                    value->uint64_val = NULL;
                }
                break;
        }
    }
}

/* calls a routine to search the table */
int
SearchTable(rtp, st)
const SnmpV2Table *rtp;
int st;
{
    int index;

    if (st == NEXT) {
        index = TableLookup((anytype_t **) rtp->tp,
                       rtp->nitems,
                       rtp->tip,
                       rtp->nindices,
                       EXACT);
        if (index != -1) {
            return index;
        }
    }

    if (st == NEXT_SKIP) {
        st = NEXT;
    }

    return TableLookup((anytype_t **) rtp->tp,
                       rtp->nitems,
                       rtp->tip,
                       rtp->nindices,
                       st);
}

/* this routine creates a new table entry and clears it out */
int
NewTableEntry(rtp)
SnmpV2Table *rtp;
{
    anytype_t **NewTab = NULL;
    anytype_t *NewRow = NULL;

    /* If the table has an index, use the normal table lookup/insertion */
    if (rtp->nindices > 0) {
        return(TableInsert((anytype_t ***) &rtp->tp, &rtp->nitems,
                           rtp->tip, rtp->nindices, rtp->rowsize));
    }

    /* No indices, so just create a new entry at the end of the table */
    if((NewTab = (anytype_t **)
        malloc((size_t)((rtp->nitems + 1) * sizeof(anytype_t *)))) == NULL) {
        goto fail;
    }
    if((NewRow = (anytype_t *)
               malloc((size_t)rtp->rowsize)) == NULL) {
        goto fail;
    }
#ifdef SR_CLEAR_MALLOC
    memset(NewRow, 0, rtp->rowsize);
#endif	/* SR_CLEAR_MALLOC */
    memcpy(NewTab, rtp->tp, rtp->nitems * sizeof(anytype_t *));
    NewTab[rtp->nitems] = NewRow;

    free((char *)rtp->tp);

    rtp->tp = (void **)NewTab;
    rtp->nitems++;

    return rtp->nitems - 1;

  fail:
    DPRINTF((APERROR, "NewTableEntry: malloc error adding row\n"));
    if(NewTab != NULL) {
        free((char *)NewTab);
    }
    if(NewRow != NULL) {
        free((char *)NewRow);
    }
    return -1;
}

/* this routine deletes an entry from a table */
void
RemoveTableEntry(rtp, index)
SnmpV2Table *rtp;
const int index;
{
    /* make sure it's a valid index */
    if(index < rtp->nitems) {
	/* copy over the entry with the rest of the table */
	memmove(rtp->tp + index, rtp->tp + (index + 1), 
		(rtp->nitems - (index + 1)) * sizeof(void *));
	rtp->nitems--;
    }
}
/* ----------------------------------------------------------------------
 *  PurgeTable(rtp) - delete all entries in a V2Table
 *
 *  i	(SnmpV2Table *) rtp
 *	Ptr to table to be purged.
 *
 *  i   (SnmpType *) typep
 *	Ptr to mib-compiler generated data structure containing
 *      the description of the entries in each table row.  This
 *	is required so that pointer references in each row are
 *	correctly deallocated.  This data structure will typically
 *	be named something like (mib-group)EntryTypeTable
 *
 *  i   (int) destroyFlag
 *	Destroy the table as much as is possible.  Deletes the
 *	value pointer array.  The table pointer itself cannot
 *	generally be deleted, as it is usually statically allocated.
 *
 *      Set this to 1 if you will no longer use the table.
 *      Set this to 0 to not delete the value pointer array.
 *
 *   When done, the table is empty, and ready for new entries
 *   (unless destroyFlag has been set).  For simplicity's sake, 
 *   we walk the table index, rather than do lookups.
 */
void
PurgeTable(SnmpV2Table *rtp, const SnmpType *typep, const int destroyFlag)
{
    while (rtp->nitems > 0) {
        FreeEntries(typep, rtp->tp[0]);
        free(rtp->tp[0]);
        RemoveTableEntry(rtp, 0);
    }
    if (destroyFlag == 1) {
        free(rtp->tp);
        rtp->tp = NULL;
    }
    return;
}
