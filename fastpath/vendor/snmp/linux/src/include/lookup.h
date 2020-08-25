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

#ifndef SR_LOOKUP_H
#define	SR_LOOKUP_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef	EXACT
#define	EXACT	GET_REQUEST_TYPE
#endif	/* EXACT */

#ifndef	NEXT
#define	NEXT	GET_NEXT_REQUEST_TYPE
#endif	/* NEXT */

#define NEXT_SKIP 1

#define T_uint 0
#define T_octet 1
#define T_oid 2
#ifdef SR_FTL
#define	T_ieeeMacAddr	3
#else	/* SR_FTL */
#define	T_ieeeMacAddr	T_octet
#endif	/* SR_FTL */
/* T_var_octet and T_var_oid are used for variable length indexes which must
 * be length encoded. If T_var_octet or T_var_oid is given as the type, the
 * lookup functions are to be called without the length encoded. The lookup 
 * routines will sort the data as if the length were included.
 */
#define T_var_octet 4
#define T_var_oid 5

typedef union _anytype {
    SR_UINT32     uint_val;
    OctetString *octet_val;
    OID         *oid_val;
    UInt64      *uint64_val;
    Int64       *int64_val;
} anytype_t;

typedef struct _Index {
    short byteoffset;
    short type;
    anytype_t value;
} Index_t;

/* public functions */
int TableInsert(
    union _anytype ***pT,
    int *pTsize,
    const struct _Index *I,
    const int Isize,
    const int RowSize);

int TableLookup(
    union _anytype **T,
    const int Tsize,
    const struct _Index *I,
    const int Isize,
    const int searchType);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_LOOKUP_H */
