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

#ifndef _SR_VBTABLE_H
#define _SR_VBTABLE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "sr_conf.h"

#include <malloc.h>

#include "sr_proto.h"
#include "sr_type.h"
#include "sr_snmp.h"
#include "sr_trans.h"

/*
 *  vbtable.h - Varbind Table routine support module
 *
 *  This code builds a table from received varbinds in support
 *  of the gettab utility and arltable utility.  It was written to
 *  resolve problems in both arltable.c and gettab.c, specifically
 *  with edge conditions and out-of-lexicographical ordering
 *  problems in some OEM SNMP agents.
 */

/*
 *  The top of the column is the end with the lexicographically lowest
 *  indexing information.   Generally varbinds are added at the bottom
 *  and taken off at the top.  The VarBind next_var pointers point
 *  towards the bottom.
 *
 *  The column state is used to differentiate between columns where
 *  we have seen end of column this pass (so we can discard subsequent
 *  varBinds that might have gone into this column) and columns where
 *  we have seen end of column in a previous pass, and therefore will not
 *  expect to see more varbinds for the column.
 */ 
typedef enum column_state_t {
    columnActive,
    columnDoneThisPass,
    columnDonePreviousPass
}  column_state;

typedef struct {
    int nobjects;			/* Number of objects in row */
    VarBind **top;			/* Top of each varBind column */
    VarBind **bottom;			/* Bottom of each varBind column */
    OID **object;			/* varBind list passed in */
    OID **lastInstance;			/* Last instance returned */
    column_state *state;		/* State of each varBind column */
    OID *initialInstance;		/* argument: Start point */
    unsigned char *smallestInstance;	/* Is this smallest instance seen? */
    SR_INT32 objnum;			/* Current object under consideration */

    SR_INT32 nonRepeatersCount;		/* argument: Nonrepeaters expected */
    SR_INT32 rowsPerCallback;		/* argument: ARL_SA_ROWS value */
    SR_INT32 maxRowsToReturn;		/* argument: ARL_SA_MAX_ROWS value */
    SR_INT32 rowsFromAgent;		/* Nmbr times we saw last obj in row */
    SR_INT32 goalRow;			/* Maybe ready to return at this row */
    SR_INT32 rowsReady;			/* Number of rows ready to go */
    SR_INT32 rowsToMgr;			/* Number of rows returned */

    VarBind *varbindsPendingReturn;	/* List of varBinds ready to return */
    VarBind *varbindsPendingReturnEnd;	/* ptr to end of list of vpr */
    SR_INT32 vbtTableReturnFormat;	/* Format of data to be returned */
    
    unsigned vbtTableDone:1;		/* Table is done */
    unsigned vbtTableDoneLexBad:1;	/* Table is done, bad lexicographic
                                           ordering encountered */
    unsigned vbtTableAllReturned:1;	/* We have returned everything */
} vbTable;


/*
 *  Public prototypes
 */

vbTable *
vbTableInitialize(
    VarBind * varBindList,		/* Ptr to variable binding list */
    int nonRepeaters,			/* nonRepeaters arg if GETBULK */
    OID *initialInstance,		/* Initial instance to return */ 
    SR_INT32 rows,                	/* Number rows to return at a time */
    SR_INT32 maxRows              	/* Max number of rows to return */
);

#define VBT_OK 0
#define VBT_TABLE_DONE 1
#define VBT_TABLE_DONE_LEX_BAD 2
#define VBT_MALLOC_FAILURE -1
#define VBT_BAD_NEXTREQUEST_ARGUMENT -2 
#define VBT_BAD_VBTABLE_ARGUMENT -3

int
vbTableAdd(
    vbTable *vbt,
    VarBind *vblist,
    SR_INT32 errorStatus,
    SR_INT32 errorIndex,
    VarBind **nextRequest
);


VarBind *vbTableRetrieveRows(
    vbTable *vbt,
    int *maxRowsDone
);

void vbTableFree(
    vbTable *vbt
);

const char *vbTableErrorStrings(
    int errorCode
);

#ifdef  __cplusplus
}
#endif	/* __cplusplus */

/* Do not put anything after this #endif */
#endif /* _SR_VBTABLE_H */
