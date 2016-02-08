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


#include <sys/types.h>

#include <string.h>

#include "sr_snmp.h"
#include "oid_lib.h"
#include "diag.h"
SR_FILENAME



/*
 * This is useful in all the scalar access methods.
 */
static const SR_UINT32 IDzero[] = {0};

#define LN_IDzero (sizeof( IDzero ) / sizeof(SR_UINT32))
const OID ZeroOid = {LN_IDzero, (SR_UINT32 *) IDzero};

/*
 * This is useful in several cases when a "NULL" OID is needed.
 */
static const SR_UINT32 _nullOID[] = {((SR_INT32)0), ((SR_INT32)0)};
const OID nullOID = {2, (SR_UINT32 *) _nullOID};



/*
 * CmpNOID:
 *
 * The former name (pre-snmp12.1.0.0 release) was cmp_n_oid().
 */
int
CmpNOID(oida, oidb, length)	/* if a < b return neg; if b < a return pos */
    const OID       *oida;
    const OID       *oidb;
    SR_INT32        length;
{
    SR_INT32        i, runlength, diff;

    if (oida->oid_ptr == NULL) {
	oida = &nullOID;
    }
    if (oidb->oid_ptr == NULL) {
	oidb = &nullOID;
    }
 
    runlength = MIN(oida->length, oidb->length);
    runlength = MIN(runlength, length);

    for (diff = i = 0; (i < runlength) && !diff; i++) {
	diff = (oida->oid_ptr[i] > oidb->oid_ptr[i])?1:
	((oida->oid_ptr[i] < oidb->oid_ptr[i])?-1: 0);
    }
 
    if (diff == 0 && (oida->length < length || oidb->length < length)) {
	diff = oida->length - oidb->length;
    }
 
    return ((int) diff);

}

/*
 * Inst2IP:
 *
 * Return an IP address derived from an OID instance.
 *     1) The IP address is in network byte order.
 *     2) A NEXT searchType returns the next possible IP address.
 *     3) Any instance elements beyond the 4th element are ignored.
 *
 * NOTE: The inst_lib.c file (in /snmpd/shared) has a similarly named routine,
 * InstToIP(), which performs a slightly different function. See inst_lib.c
 * for more information.
 *
 * The former name (pre-snmp12.1.0.0 release) was inst2ip().
 */
int
Inst2IP(inst, instLength, ipAddr, searchType)
    const SR_UINT32 inst[];
    int             instLength;
    SR_UINT32      *ipAddr;
    int             searchType;
{
    int             i, carry;
    SR_UINT32       tmp;

    *ipAddr = ((SR_INT32)0);

    if (searchType == GET_REQUEST_TYPE) {
	if (instLength != 4) {
	    return (0);
	}

	for (i = 3; i >= 0; i--) {
	    if (inst[i] > 255) {
		return (0);
	    }

	    ((unsigned char *) ipAddr)[i] = (unsigned char) inst[i];
	}

	return (1);
    }
    else {			/* must be a NEXT (GET_NEXT_REQUEST_TYPE) */
	carry = (instLength >= 4);

	for (i = ((instLength > 4) ? 3 : instLength - 1); i >= 0; i--) {
	    if ((tmp = inst[i] + carry) > 255) {
		*ipAddr = ((SR_INT32)0);
		carry = 1;
	    }
	    else {
		((unsigned char *) ipAddr)[i] = (unsigned char) tmp;
		carry = 0;
	    }
	}

	return ((carry == 0));
    }
}

/*
 * CmpOIDClass: 
 * 
 * The former name (pre-snmp12.1.0.0 release) was cmp_oid_class(). 
 */
int
CmpOIDClass(ptr1, ptr2)
    const OID *ptr1;
    const OID *ptr2;
{
    int             i;
    int             minimum = (int) ((ptr1->length < ptr2->length) ? ptr1->length : ptr2->length);

    /* compare until the shorter oid runs out */
    for (i = 0; i < minimum; i++) {
        /* First check if not equal.  This prevents two comparisons on
         * each interation. */
	if (ptr1->oid_ptr[i] != ptr2->oid_ptr[i]) {
	    if (ptr1->oid_ptr[i] > ptr2->oid_ptr[i]) {
	        return (1);
	    }
	    if (ptr1->oid_ptr[i] < ptr2->oid_ptr[i]) {
	        return (-1);
	    }
	}
    }

    /*
     * tie only indicates that they are equal up to the length of the shorter
     */
    return (0);
}

/*
 * CmpOID: Compares two OIDs.
 *
 * Returns:   0 if contents of ptr1 have the same length and values 
 *              as the contents of ptr2, 
 *          < 0 if contents of ptr1 are less than (or "smaller") than 
 *              the contents of ptr2,
 *          > 0 otherwise.
 *
 * The former name (pre-snmp12.1.0.0 release) was cmp_oid_values().
 */
int
CmpOID(ptr1, ptr2)
    const OID *ptr1;
    const OID *ptr2;
{
    int             i;
    int             minimum = (int) ((ptr1->length < ptr2->length) ? ptr1->length : ptr2->length);

    /* check for mismatched values */
    for (i = 0; i < minimum; i++) {
        /* First check if not equal.  This prevents two comparisons on
         * each interation. */
	if (ptr1->oid_ptr[i] != ptr2->oid_ptr[i]) {
	    if (ptr1->oid_ptr[i] > ptr2->oid_ptr[i]) {
	        return (1);
	    }
	    if (ptr1->oid_ptr[i] < ptr2->oid_ptr[i]) {
	        return (-1);
	    }
	}
    }

    /*
     * equal for as long as the shorter one holds out.  The longer should be
     * considered bigger
     */
    if (ptr1->length > ptr2->length)
	return (1);
    if (ptr1->length < ptr2->length)
	return (-1);


    /* they are equal for their entire mutual lengths */
    return (0);
}     /* CmpOID() */

/* this routine compares two oids (which cannot be NULL pointers)
 * that will have the length encoded when used as an index
 * and returns zero if they are equal in length and values, and a "difference"
 * value similar to strcmp()'s if they are not. */
/* Now returns 1 if oid1's element is greater than oid2's, or -1
   if oid2's element is greater. This will handle unsigned ints correctly  */
int
CmpOIDWithLen(oid1, oid2)
    const OID *oid1;
    const OID *oid2;
{
    SR_INT32 i, n;

    if (oid1->length != oid2->length) {
        return oid1->length - oid2->length;
    }
    n = oid1->length;
    for(i = 0; i < n; i++) {
        if(oid1->oid_ptr[i] != oid2->oid_ptr[i]) {
           if (oid1->oid_ptr[i] > oid2->oid_ptr[i])  {
              return (1);
           }
           else  { 
              return (-1);
           }
        }				/* end of if element different */
    }
    return (0);				/* return 0 since lengths are the same */
}					/* and all elements are the same */

/*
 * CatOID: allocates space and concatenates the contents of two OIDs into a
 *         new OID.
 *
 * Returns: a pointer to the new,
 *          0 if error
 *
 * The former name (pre-snmp12.1.0.0 release) was cat_oid().
 */
OID            *
CatOID(oida, oidb)
    const OID *oida;
    const OID *oidb;
{
    OID            *oid;
    int             i, j, length;

    if ((oida == NULL) || (oidb == NULL)) {
        return NULL;
    }
    if ((length = oida->length + oidb->length) > MAX_OID_SIZE) {
        DPRINTF((APPACKET, "CatOID: Too long: %d\n", length));
        return (NULL);
    }

    if ((oid = MakeOID(NULL, length)) == NULL) {
        DPRINTF((APWARN, "CatOID, MakeOID failed\n"));
        return (NULL);
    }

    for (j = i = 0; i < oida->length; oid->oid_ptr[j++] = oida->oid_ptr[i++]);
    for (i = 0; i < oidb->length; oid->oid_ptr[j++] = oidb->oid_ptr[i++]);
    oid->length = length;

    return (oid);
}                               /* end of CatOID */

 /*
  * CloneOID: Clone the OID pointed to by oid_ptr and return the pointer to
  *           the cloned OID.
  *
  * The former name (pre-snmp12.1.0.0 release) was clone_oid().
  */
OID            *
CloneOID(oid_ptr)
    const OID *oid_ptr;
{

    if (oid_ptr == NULL) {
        return (NULL);
    }
    return MakeOID(oid_ptr->oid_ptr, oid_ptr->length);
}

 /*
  * ClonePartialOID: Clone the indicated portion of oid_ptr and return the
  *                  cloned pointer.
  *
  * The former name (pre-snmp12.1.0.0 release) was clone_partial_oid().
  */
OID            *
ClonePartialOID(oid_ptr, start, len)
    const OID       *oid_ptr;
    int             start;
    int             len;
{
    if (oid_ptr == NULL) {
        return (NULL);
    }
    return MakeOID(&(oid_ptr->oid_ptr[start]),
		   MAX(0, MIN(len, oid_ptr->length - start)));
}

/*
 * CheckOID:
 *
 *   The former name (pre-snmp12.1.0.0 release) was chk_oid().
 */
int
CheckOID(oid1_ptr, oid2_ptr)
    const OID *oid1_ptr;
    const OID *oid2_ptr;
{
    int             i;

    for (i = 0; i < oid1_ptr->length; i++) {
        if (oid1_ptr->oid_ptr[i] < oid2_ptr->oid_ptr[i]) {
            return (-1);
        }
    }
    return (0);
}                               /* CheckOID() */
