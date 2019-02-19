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

#define        WANT_ALL_ERRORS

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>


#include <string.h>

#include <malloc.h>

#include "sr_snmp.h"
#include "oid_lib.h"		/* for CmpOID() */
#ifdef SR_DEBUG
#include "prnt_lib.h"		/* for PrintOID() */
#endif	/* SR_DEBUG */
#include "diag.h"
SR_FILENAME


#include "sr_trans.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "objinfo.h"		/* for protos of functions in this file */

/*
 * AllocObjectInfoList: allocates space for a NULL-terminated
 *                      list of ObjectInfo structures.
 *
 * Returns:
 *    a pointer to the successfully allocated memory, or
 *    NULL otherwise
 *
 */
ObjectInfo *
AllocObjectInfoList(size)
    SR_UINT32 size;
{
    ObjectInfo *newOiList = NULL;
    FNAME("AllocObjectInfoList")

    newOiList = (ObjectInfo *) malloc (sizeof(ObjectInfo) * (size));
    if (newOiList == NULL) {
	DPRINTF((APERROR, "%s: malloc failed\n", Fname));
    }
    else {
	memset(&(newOiList[size-1]), 0, sizeof(ObjectInfo));
    }

    return newOiList;
}


/*
 * FreeObjectInfoList: frees memory allocated in AllocObjectInfoList.
 *
 * Returns:
 *    a pointer to the successfully allocated memory, or
 *    NULL otherwise
 *
 */
void
FreeObjectInfoList(oiList)
    ObjectInfo *oiList;
{
    if (oiList != NULL) {
	free(oiList);
    }
}


/*
 * InsertObjectInfo: populates a ObjectInfo structure.
 *
 * Returns: void
 *
 */
void
InsertObjectInfo(
    ObjectInfo     *oi,
    SR_INT32        length,
    const SR_UINT32 *oid_ptr,
    char           *oidname,
    unsigned char   oidtype,
    unsigned char   oidstatus,
    unsigned char   nominator,
    unsigned char   priority,
    GetMethod       get,
    TestMethod      test)
{
    oi->oid.length = length;
    oi->oid.oid_ptr = (SR_UINT32 *)oid_ptr;
#ifndef LIGHT
    oi->oidname = oidname;
#else /* LIGHT */
#endif /* LIGHT */
    oi->oidtype = oidtype;
    oi->oidstatus = oidstatus;
    oi->nominator = nominator;
    oi->priority = priority;
    oi->get = get;
    oi->test = test;
}


/*
 * CmpObjectInfo: Compares the OIDs from two ObjectInfo structures.
 *
 * Returns:   0 if contents of ptr1 have the same length and values 
 *              as the contents of ptr2, 
 *          < 0 if contents of ptr1 are less than (or "smaller") than 
 *              the contents of ptr2,
 *          > 0 otherwise.
 *
 */
int
CmpObjectInfo(ptr1, ptr2) 
    const ObjectInfo *ptr1;
    const ObjectInfo *ptr2;
{
    return CmpOID(&(ptr1->oid), &(ptr2->oid));
}


/*
 * SortObjectInfoList: Sorts an array of ObjectInfo structures by
 *                     OID.  The last element is assumed to be a
 *                     NULL terminator and is ignored.
 *
 * Note: the array of ObjectInfo structures is modified!
 *
 */
void
SortObjectInfoList(oiList, size)
    ObjectInfo *oiList;
    SR_UINT32   size;
{
    if (oiList == NULL) {
        return;
    }
    qsort((char *) oiList, size - 1, sizeof(ObjectInfo), 
          (int (*)SR_PROTOTYPE((const void *, const void *))) CmpObjectInfo);

}

#ifdef SR_DEBUG
/*
 * PrintObjectInfoList: print the OIDs from an array of
 *                      ObjectInfo structures.
 * Arguments:
 *   1) oiList is a pointer into the array where printing
 *      should begin
 *   2) num is the maximum number of OIDs to print
 *
 * Returns: the number of OIDs printed
 *
 */
int
PrintObjectInfoList(oiList, num)
    ObjectInfo *oiList;
    SR_UINT32   num;
{
    int i = 0;

    while (num > 0 && oiList[i].oid.oid_ptr != NULL) {
	PrintOID(&oiList[i].oid);
	i++;
	num--;
    }

    return i;
}
#endif	/* SR_DEBUG */
