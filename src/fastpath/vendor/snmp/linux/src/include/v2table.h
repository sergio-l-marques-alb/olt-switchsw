/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
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
 * This file contains structure definitions and macros to assist in
 * table manipulation of SNMP V2 tables which use the RowStatus textual
 * convention.
 */

#ifndef SR_V2TABLE_H
#define SR_V2TABLE_H

#ifdef  __cplusplus
extern "C" {
#endif

/* default timeout parameters for row creation */
#define MAX_ROW_CREATION_TIME (SR_INT32)300000	/* 50 minutes in centiseconds */

/* this structure describes table entry properties */
typedef struct _snmpTypeS {
    short type;		/* data type */
    short readaccess;	/* read or read-write */
    unsigned short byteoffset;	/* offset of field into data structure */
    short indextype;	/* index priority or -1 if not an index */
} SnmpType;

/* this structure describes a table */
typedef struct _snmpV2TableS {
    void **tp;		/* pointer to the table */
    Index_t *tip;	/* pointer(s) to table index structures */
    int nitems;		/* number of table entries */
    int nindices;	/* number of table indices */
    int rowsize;	/* size of a table element */
} SnmpV2Table;

/* prototypes */
int CopyEntries(
    const int maxNominator,
    const SnmpType *typep,
    void *newvalue,
    const void *oldvalue);

void TransferEntries(
    const int nominator,
    const SnmpType *typep,
    void *newp,
    void *oldp);

void FreeEntries(const SnmpType *typep, void *p);

int SearchTable(const SnmpV2Table *rtp, int st);

int NewTableEntry(SnmpV2Table *rtp);

void RemoveTableEntry(SnmpV2Table *rtp, const int index);

int CmpValues(
    const int nominator,
    const SnmpType *typep,
    const void *olddatap,
    const ObjectSyntax *newval);

#ifdef  __cplusplus
}
#endif

#endif				/* SR_V2TABLE_H */
