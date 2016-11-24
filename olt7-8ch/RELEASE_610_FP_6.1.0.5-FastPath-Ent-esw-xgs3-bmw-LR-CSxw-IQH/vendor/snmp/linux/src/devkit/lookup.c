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

#include <string.h>


#include <malloc.h>

#include "sr_snmp.h"
#include "lookup.h"
#include "diag.h"
SR_FILENAME

#include "oid_lib.h"

#define CmpInts(a,b) ((a)<(b)?-1:((a)>(b)))

static OctetString empty_octet_string = { NULL, 0 };
static OID empty_object_identifier = { 0, NULL };

/* static functions */
int CmpIndices
    SR_PROTOTYPE((const void *rec,
		  const Index_t *I,
		  const int Isize));

void InstallIndices
    SR_PROTOTYPE((const void *New,
		  const Index_t *I,
		  const int Isize));

#ifdef SR_FTL
static int CmpIEEEMacAddrs
    SR_PROTOTYPE((const OctetString *a1,
                  const OctetString *a2));


/* compares two octet string addresses */
static int
CmpIEEEMacAddrs(a1, a2)
    const OctetString *a1, *a2;
{
    return(memcmp((char *) a1->octet_ptr, (char *) a2->octet_ptr, 6));
}
#endif /* ifdef SR_FTL */

/* ----------------------------------------------------------------------
 * CmpIndices
 *
 *  CmpIndices checks the passed data structure record with the
 *  passed index array.  The index array specifies both the
 *  values to be matched against, and the offsets of the values
 *  in the data record.
 *
 * Arguments:
 *   i void *cur
 *      ptr to beginning of record data structure to be checked.
 *   i Index_t *I
 *      array of index structures
 *   i int Isize
 *      Number of Index_t elements in I.
 *
 *  Return Values:
 *
 *  -1 : Index is less than indicated table item
 *   0 : Index is equal to indicated table item
 *   1 : Index is greater than indicated table item
 */
int
CmpIndices(rec, I, Isize)
    const void *rec;
    const Index_t *I;
    const int Isize;
{
    int cmp = 0, i;
    const OctetString *os;
    const OID *oid;
    anytype_t *cur;

    for(i = 0; i < Isize; i++) {
        cur = (anytype_t *) ((unsigned long) rec + I[i].byteoffset);
	if(I[i].type == T_uint) {
	    cmp = CmpInts(I[i].value.uint_val, cur->uint_val);
	}
	else if(I[i].type == T_octet) {
            os = I[i].value.octet_val;
            if (os == NULL) {
                os = &empty_octet_string;
            }
	    cmp = CmpOctetStrings(os, cur->octet_val);
	}
	else if(I[i].type == T_var_octet) {
            os = I[i].value.octet_val;
            if (os == NULL) {
                os = &empty_octet_string;
            }
	    cmp = CmpOctetStringsWithLen(os, cur->octet_val);
	}
	else if(I[i].type == T_oid) {
            oid = I[i].value.oid_val;
            if (oid == NULL) {
                oid = &empty_object_identifier;
            }
	    cmp = CmpOID(oid, cur->oid_val);
	}
	else if(I[i].type == T_var_oid) {
            oid = I[i].value.oid_val;
            if (oid == NULL) {
                oid = &empty_object_identifier;
            }
	    cmp = CmpOIDWithLen(oid, cur->oid_val);
	}
#ifdef SR_FTL
	else if(I[i].type == T_ieeeMacAddr) {
	    cmp = CmpIEEEMacAddrs(I[i].value.octet_val,
				  cur->octet_val);
	}
#endif	/* SR_FTL */

	if(cmp != 0) {
	    break;
	}
    }
    return cmp;
}

void
InstallIndices(New, I, Isize)
    const void *New;
    const Index_t *I;
    const int Isize;
{
    int i;
    anytype_t *cur;

    /* do nothing if the index type is unknown */
    for(i = 0; i < Isize; i++) {
        cur = (anytype_t *) ((unsigned long) New + I[i].byteoffset);
	if(I[i].type == T_uint) {
	    cur->uint_val = I[i].value.uint_val;
	}
	else if((I[i].type == T_octet) || (I[i].type == T_var_octet)) {
	    cur->octet_val = CloneOctetString(I[i].value.octet_val);
	}
	else if((I[i].type == T_oid) || (I[i].type == T_var_oid)) {
	    cur->oid_val = CloneOID(I[i].value.oid_val);
	}
    }
}

/*
  TableInsert
   
  Return Values:

  0..(Tsize-1) : indicates index of existing or newly inserted table item
       -1      : indicates error during insertion
 */
int
TableInsert(pT, pTsize, I, Isize, RowSize)
anytype_t ***pT;
int *pTsize;
const Index_t *I;
const int Isize;
const int RowSize;
{
    FNAME("TableInsert")
    int pos;
    anytype_t **NewTab = NULL;
    anytype_t *NewRow = NULL;

    if(*pTsize < 0) {
	*pTsize = 0;
    }
    if((pos = TableLookup(*pT, *pTsize, I, Isize, EXACT)) == -1) {
        if((pos = TableLookup(*pT, *pTsize, I, Isize, NEXT)) == -1) {
	    pos = *pTsize;
        }
        else if(CmpIndices((*pT)[pos], I, Isize) == 0) {
	    return pos;
        }
    }
    else if(CmpIndices((*pT)[pos], I, Isize) == 0) {
	return pos;
    }
    if((NewTab = (anytype_t **)
	malloc((size_t)((*pTsize + 1) * sizeof(anytype_t *)))) == NULL) {
	goto fail;
    }
    if((NewRow = (anytype_t *)
	       malloc((size_t)RowSize)) == NULL) {
	goto fail;
    }
#ifdef SR_CLEAR_MALLOC
    memset(NewRow, 0, RowSize);
#endif	/* SR_CLEAR_MALLOC */
    InstallIndices(NewRow, I, Isize);

    memcpy(NewTab, *pT, pos * sizeof(anytype_t *));
    NewTab[pos] = NewRow;
    memcpy(&(NewTab[pos+1]), &((*pT)[pos]),
	   (*pTsize - pos) * sizeof(anytype_t *));

    free((char *)*pT);

    *pT = NewTab;
    (*pTsize)++;

    return pos;
  fail:
    DPRINTF((APERROR, "%s: malloc error adding row\n", Fname));
    if(NewTab != NULL) {
	free((char *)NewTab);
    }
    if(NewRow != NULL) {
	free((char *)NewRow);
    }
    return -1;
}

/*
  TableLookup
   
  Return Values:

  0..(Tsize-1) : indicates index of first table item which is >= to index
       -1      : indicates there is no table item >= index
 */
int
TableLookup(T, Tsize, I, Isize, searchType)
anytype_t **T;
const int Tsize;
const Index_t *I;
const int Isize;
const int searchType;
{
    int low, high, mid = 0;
    int cmp = -1;

    low = 0;
    high = Tsize-1;
    while(low <= high) {
	mid = (low + high)/2;

	cmp = CmpIndices(T[mid], I, Isize);

	if(cmp < 0) {
	    high = mid - 1;
	}
	else if(cmp > 0) {
	    low = mid + 1;
	}
	else {
	    break;
	}
    }

    if(searchType == EXACT && cmp != 0) {
	return -1;
    }

    if(searchType == NEXT && cmp == 0) {
	mid++;
    }

    if(cmp > 0) {
	mid++;
    }

    if(mid < Tsize) {
	return mid;
    }
    return -1;
}
