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


#include <string.h>

#include <malloc.h>

#include "sr_proto.h"
#include "sr_type.h"
#include "sr_snmp.h"
#include "oid_lib.h"
#include "diag.h"
#include "sri/vbtable.h"

SR_FILENAME

/*  ======================================================================
 *
 *  Non-Public Routines
 *
 *  ----------------------------------------------------------------------
 *
 *  compare_instance() - Compare the instance information between
 *  two objects in variable bindings, using a third OID as a template
 *  from which to get the length of the object information (and thus
 *  the length of the instancing information).
 *
 *  This is primarily a code cleanliness subroutine.
 *
 *  Arguments:
 *  i   (const VarBind *) vb1
 *	First variable binding to use in comparison
 *  i   (const VarBind *) vb2
 *	Second variable binding to use in comparison
 *  i 	(const OID *) baseOID1
 *	The base OID used in vb1.
 *  i 	(const OID *) baseOID2
 *	The base OID used in vb2.
 *  o 	(int) (function value)
 *	The value returned by CmpOID.  0 if bad arguments.
 */
static 
int 
compare_instance(
    const VarBind *vb1,
    const VarBind *vb2,
    const OID *baseOID1,
    const OID *baseOID2)
{
    OID oid1, oid2;

    if (!vb1 || !vb2 || !baseOID1 || !baseOID2) {
        return 0;
    }

    oid1.oid_ptr = vb1->name->oid_ptr + baseOID1->length;
    oid1.length = vb1->name->length - baseOID1->length;
    oid2.oid_ptr = vb2->name->oid_ptr + baseOID2->length;
    oid2.length = vb2->name->length - baseOID2->length;

    return CmpOID(&oid1, &oid2);
}

/*  ----------------------------------------------------------------------
 *
 *  return_a_row() - Return a conceptual row if one is ready
 *
 *  Arguments:
 *  i	(vbTable *) vbt
 *	varBind table descriptor
 *  o	(VarBind **) vblEnd
 *	Ptr to end of varBind list returned.
 *      Not changed if nothing ready.
 *  o	(VarBind *) (function value)
 *	Ptr to varBind list if a row is ready, 
 *	NULL otherwise.
 */
static 
VarBind *return_a_row(
    vbTable *vbt,
    VarBind **vblEnd
)
{
    int mincolnum, colnum, diff;
    VarBind *vb = NULL, *vbp = NULL, *minvb = NULL; 
/*
 *  We search the columns for the varbind with the lowest instance.
 *  That becomes the instance for the next row to be returned.
 *  If one or more columns do not have that instance, we fake up
 *  a null value for that specific oid/instance.
 */

    memset(vbt->smallestInstance, 0, vbt->nobjects * sizeof(unsigned char));

    for (mincolnum = 0; mincolnum < vbt->nobjects; mincolnum++) {
        if (vbt->top[mincolnum] != NULL) {
            break;
        }
    }
    if (mincolnum < vbt->nobjects) {
        vbt->smallestInstance[mincolnum] = 1;
    } else {
         return NULL;			/* all cols empty: we are done */
    }
/*
 *  We compare only instancing information; this allows us to
 *  have objects from different SNMP tables reported at the same
 *  time.
 */
    for (colnum = mincolnum + 1; colnum < vbt->nobjects; colnum++) {
        if (vbt->top[colnum]) {
            diff = compare_instance(vbt->top[mincolnum], vbt->top[colnum],
                       vbt->object[mincolnum], vbt->object[colnum]); 

            if (diff == 0) {
                vbt->smallestInstance[colnum] = 1;
            } else if (diff > 0) {
                mincolnum = colnum;
                memset(vbt->smallestInstance, 0, 
                    colnum * sizeof(unsigned char));
                vbt->smallestInstance[colnum] = 1;
            }
        }
    }
    minvb = vbt->top[mincolnum];
/*
 *  vbt->smallestInstance now contains 1 for each column with the
 *  small instance value.  Either one of three cases applies:
 *
 *  1)  There is a larger instance later in the column.  In this
 *      case we assume that the instance is really missing, and
 *      we fill it in with a dummy instance.
 *  2)  There is no larger instance later in the column.  We may
 *	get it on another retrieval; don't return this row yet.
 *  3)  There is a larger instance later in the column, but it
 * 	might get filled in later by an out-of-lexicographical-order
 *	response.  We just don't care; treat it as case 1.
 */

    for (colnum = 0; colnum < vbt->nobjects; colnum++) {
        if (vbt->smallestInstance[colnum] != 1) {
            if (vbt->top[colnum] == NULL && 
                !vbt->vbtTableDone &&
                !vbt->vbtTableDoneLexBad) 
            {
                return NULL;			/* case 2 */
            }
        }
    }
/*
 *  Either return the object in the row, or make up an empty
 *  one if no such object with the current instance exists.
 *  For handling out of lexicographical order errors, we have to
 *  remember the object/instance information for a column if we
 *  empty it.
 */
    for (colnum = 0; colnum < vbt->nobjects; colnum++) {
        VarBind *thisVb;
        if (vbt->smallestInstance[colnum]) {
            thisVb = vbt->top[colnum];
            vbt->top[colnum] = vbt->top[colnum]->next_var;
            if (vbt->top[colnum] == NULL) {
                vbt->bottom[colnum] = NULL;
                vbt->lastInstance[colnum] = CloneOID(thisVb->name);
            }
        } else {
            OID thisInstance;
            thisInstance.oid_ptr = minvb->name->oid_ptr +
                vbt->object[mincolnum]->length;
            thisInstance.length = minvb->name->length -
                vbt->object[mincolnum]->length;
            thisVb = MakeVarBindWithNull(vbt->object[colnum], 
                &thisInstance);
        }

        if (vb == NULL) {
            vb = thisVb;
            vbp = vb;
        } else {
            vbp->next_var = thisVb;
            vbp = vbp->next_var;
        }
    }
    vbp->next_var = NULL;

    *vblEnd = vbp;
    return vb;
}
/* ----------------------------------------------------------------------
 *
 *  handleRecoverableErrorStatus() - handle case where we have
 *  an error status on one of the varbinds, and it is one of the
 *  cases we do handle.
 *
 *  This routine called from vbTableAdd, and assumes that argument
 *  validation has already been done there.
 *
 *  Arguments:
 *
 *  same as vbTableAdd().
 */
static int
handleRecoverableErrorStatus(
    vbTable *vbt,
    VarBind *vblist,
    SR_INT32 errorStatus,
    SR_INT32 errorIndex,
    VarBind **nextRequest
)
 
{
    int objnum = vbt->objnum;
    VarBind *vb, *vbPrev = NULL, *nextVb;
    int varBindIndex = 0;

    for (vb = vblist; vb != NULL; vb = nextVb) { 
        nextVb = vb->next_var;
        varBindIndex++;
/*
 *  Skip over columns already done
 */
        do {
           objnum++;
            if (objnum >= vbt->nobjects) {
                objnum = 0;
            }
        } while (vbt->state[objnum] == columnDonePreviousPass);
/*
 *  If the current varBind is marked with the error, delete it, 
 *  and mark it's column as done.  We mark it as columnDonePreviousPass
 *  as we do not go back through the columns toggling state.
 *  Remove the varBind from the passed in list.
 *
 *  Note that as we always break out of the loop when we see
 *  the errorIndex varbind, vbPrev is always nonnull when dereferenced.
 *
 *  If the error varbind is the first varbind in the list, we
 *  have to do some objnum adjustment.
 */
        if (errorIndex == varBindIndex) {
            vbt->state[objnum] = columnDonePreviousPass;
            if (vb == vblist) {
                if (vb->next_var == NULL) {
                    *nextRequest = NULL;
                    FreeVarBindList(vblist);
                    return VBT_TABLE_DONE;
                } else {
                    vblist = vb->next_var;
                    FreeVarBind(vb);
                    vbt->objnum++;
                    if (vbt->objnum >= vbt->nobjects) {
                        vbt->objnum = 0;
                    }
                }
            } else {
                vbPrev->next_var = vb->next_var;
                FreeVarBind(vb);
            }
            break;
        } else {
            vbPrev = vb;
        }
    }

    *nextRequest = vblist;
    return VBT_OK;
}

 
/*  ======================================================================
 *
 *  Public Routines
 *
 * ----------------------------------------------------------------------
 *
 *  vbTableInitialize - Initialize a VarBind table
 *
 *  Arguments
 *  i	(VarBind *) varBindList
 *	List of variable bindings that the vbTable is to manage.
 *	The names are copied from this list, so the vbTable facility
 *	does not depend on this argument once vbTableInitialize has
 *	returned.  It is assumed that the variable bindings will
 *	not have instancing information and that the initial
 *	instance will be passed in via the initialInstance argument.
 *
 *  i	(int) nonRepeaters
 *	if this is a GET_BULK_REQUEST_TYPE, the number of variable
 *	bindings in the varBindList which are nonrepeaters.  On the
 *	initial call to vbTableAdd, the values will simply be added
 *	to the list of varBinds to be returned.  Pass 0 if 
 *      there are no non-repeaters, or this is a GET_NEXT request.
 *
 *  i	(OID *) initialInstance
 *	Initial instance to return.  If NULL, will use the first
 *	instance in the column.  Note that the first instance returned
 *	will be the instance _after_ initialInstance, since GetNext or
 *	GetBulk are used for information retrieval. vbTableInitialize does
 *	does not depend on this argument once this subroutine has returned.
 *
 *  i	(SR_INT32) rows
 *	Number of rows to return at a time in vbTableRetrieve
 *	0/1: Return one row at a atime
 *	n: Return n rows at a time.
 *	-1: Wait until all done before returning any rows.
 *
 *  i	(SR_INT32) maxRows
 *	Maximum number of rows to return.  
 *	-1: Return all rows.
 *
 *  o	(vbTable *) (function value)
 *	A pointer to the VarBind table structure, if successful, 
 *	otherwise NULL.
 *
 */
vbTable *
vbTableInitialize(
    VarBind *varBindList,		/* Ptr to variable binding list */
    int  nonRepeaters,			/* nonRepeaters arg if GETBULK */
    OID *initialInstance,		/* Initial instance to return */
    SR_INT32 rows,			/* Number rows to return at a time */
    SR_INT32 maxRows			/* Max number of rows to return */
)
{
    vbTable *vbt = NULL;
    VarBind *vbp = NULL;		/* Ptr into vb list in loops */
    int i;				/* lcv */

    if (!(vbt = (vbTable *) malloc(sizeof(vbTable)))) {
        return NULL;
    }
    memset((void *) vbt, 0, sizeof(vbTable));
/*
 *   We are only interested in non-repeater varbinds for table fill
 *   calls.
 */
    if (nonRepeaters > 0) {
        for (i = 0; i < nonRepeaters && varBindList != NULL; i++) {
            varBindList = varBindList->next_var;
        }
        vbt->nonRepeatersCount = nonRepeaters;
    }

    for (vbp = varBindList; vbp != NULL; vbp = vbp->next_var) {
       vbt->nobjects++;
    }
/*
 *  Initialize various structures.   Copy OID's from
 *  VarBind list.
 */
    vbt->top = (VarBind **) malloc(vbt->nobjects * sizeof(VarBind *));
    vbt->bottom = (VarBind **) malloc(vbt->nobjects * sizeof(VarBind *));
    vbt->object = (OID **) malloc(vbt->nobjects * sizeof(OID *));
    vbt->state = (column_state *) malloc(vbt->nobjects * sizeof(column_state));
    vbt->smallestInstance = (unsigned char *) 
        malloc(vbt->nobjects * sizeof(unsigned char));
    vbt->lastInstance = (OID **) malloc(vbt->nobjects * sizeof(OID *));

    if (!(vbt->top && vbt->bottom && vbt->state && vbt->object && 
          vbt->smallestInstance && vbt->lastInstance)) {
        goto fail;
    }

    memset((void *) vbt->top, 0, vbt->nobjects * sizeof(VarBind *));
    memset((void *) vbt->bottom, 0, vbt->nobjects * sizeof(VarBind *));
    memset((void *) vbt->object, 0, vbt->nobjects * sizeof(char));
    memset((void *) vbt->lastInstance, 0, vbt->nobjects * sizeof(OID *));

    vbp = varBindList;
    for (i = 0; i < vbt->nobjects; i++) {
        vbt->state[i] = columnActive;
        vbt->object[i] = CloneOID(vbp->name);
        vbp = vbp->next_var;
    }

    if (initialInstance) {
        vbt->initialInstance = CloneOID(initialInstance);
    }
    vbt->rowsPerCallback = rows;
    if (vbt->rowsPerCallback == 0) vbt->rowsPerCallback = 1;
    vbt->maxRowsToReturn = maxRows;
    vbt->objnum = vbt->nobjects;
    vbt->rowsFromAgent = -1;
    vbt->rowsToMgr = 0;
    vbt->goalRow = rows;
    return vbt;

fail:
    vbTableFree(vbt);
    return NULL;
}


/* ----------------------------------------------------------------------
 *
 *  vbTableAdd - add varbinds from agent response packet to table
 *
 *  Arguments:
 *    i (vbTable *) vbt
 *	VarBind table, as initialized by vbTableInitialize
 *    i (const VarBind *) vblist
 *	List of VarBinds to add to table
 *    i (const SR_INT32)  errorStatus
 *	The error status, if any, associated with the incoming varBind list.
 *	Should be NO_ERROR if no error.
 *    i (const SR_INT32)  errorIndex
 *	The error index, if any, associated with the incoming varBind list.
 *	Ignored if errorStatus is NO_ERROR.
 *    o (VarBind **) nextRequest
 *	List of VarBinds for next request.  
 *	Note that after the first request, nonrepeaters are not
 *	permitted.
 *    o (int) (function value)
 *	Status of operation  (in vbtable.h)
 *	    VBT_OK - Operation successful
 *	    VBT_TABLE_DONE - Operation successful, table is complete
 *	    VBT_MALLOC_FAILURE - malloc failure, vbt freed.
 *	    VBT_TABLE_DONE_LEX_BAD  - Operation successful, table is
 *		complete, out of lexicographical order varbind seen.
 *	    VBT_BAD_NEXTREQUEST_ARGUMENT
 *
 *      Use vbTableErrorStrings() to get textual representation of
 *	these errors.
 *
 *   Notes:
 *	Additions to the table can continue to take place even after
 *	retrieval has started.  However, once the returned varBinds
 *	indicate that the table is done, or an instance has been repeated,
 *	this routine will return a non-OK error status and take no action.
 *
 *      The errorStatus and errorIndex are required to generate correct
 *	requests subsequent when certain boundry conditions are hit.  
 */
int 
vbTableAdd(
    vbTable *vbt,
    VarBind *vblist,
    SR_INT32 errorStatus,
    SR_INT32 errorIndex,
    VarBind **nextRequest
)
{

    VarBind *vb = NULL, *nextVb = NULL, *vbp = NULL;
    int diff;
    int objnum = vbt->objnum;
    int retstatus = VBT_OK;
    int validInstanceSeen = FALSE;	/* check that all instances are 
                                           not instances already seen for
 					   each given object */
#define FUDGE_INSTANCES 1
#ifdef FUDGE_INSTANCES
/*
 *  These variables store the column number for the column with
 *  the minimum instance before and after the column with the most
 *  recently received object, and the instancing information for those
 *  instances.
 */
    int trailingColMinInstNum = -1;
    int leadingColMinInstNum = -1;
    OID trailingColMinInstance = {0, NULL};
    OID leadingColMinInstance = {0, NULL};
#endif /* FUDGE_INSTANCES */

    if (vbt == NULL) {
        return VBT_BAD_VBTABLE_ARGUMENT;
    }
    if (nextRequest == NULL) {
        return VBT_BAD_NEXTREQUEST_ARGUMENT;
    }
    *nextRequest = NULL;
    if (vbt->vbtTableDone) {
        return VBT_TABLE_DONE;
    }
    if (vbt->vbtTableDoneLexBad) {
        return VBT_TABLE_DONE_LEX_BAD;
    }
    if (vbt->vbtTableAllReturned) {
        return VBT_TABLE_DONE;
    }

/*
 *  If we are called with some error condition we don't handle, 
 *  close down the acquisition of data.
 */
    if ((errorStatus != NO_ERROR) &&
        (errorStatus != NO_ACCESS_ERROR) &&
        (errorStatus != NO_SUCH_NAME_ERROR)) {
        retstatus = VBT_TABLE_DONE;
        FreeVarBindList(vblist);
        goto done;
    }
/*
 *  If we are called with error conditions we do handle, handle
 *  them seperately.  
 */
    if (errorStatus != NO_ERROR) {
         retstatus = handleRecoverableErrorStatus(vbt, vblist, errorStatus,
             errorIndex, nextRequest);
        goto done;
    }
/*
 *  If we have nonRepeaters present, suck them off into a separate list.
 *  We assume we will see this only on the first incoming PDU.
 */
    if (vbt->nonRepeatersCount != 0) {
       vbt->varbindsPendingReturn = vblist;
       while (vbt->nonRepeatersCount != 0) {
            vbt->varbindsPendingReturnEnd = vblist;
            vblist = vblist->next_var;
            vbt->nonRepeatersCount--;
       }
       vbt->varbindsPendingReturnEnd->next_var = NULL;
    }

/* --- main varBind by varBind loop --- */

    for (vb = vblist; vb != NULL; vb = nextVb) {
        nextVb = vb->next_var;			/* save forward link */
/*
 *  Skip over columns already done
 */
        do {
           objnum++;
            if (objnum >= vbt->nobjects) {
                objnum = 0;
                vbt->rowsFromAgent++;
            }
        } while (vbt->state[objnum] == columnDonePreviousPass);

/*
 *  If we finished off the current column this pass, toss the varbind,
 *  and jump to the end of the vb loop.  This happens if 
 *  the column completed while handling a previous varBind in the
 *  varBindList passed in.  
 */
        if (vbt->state[objnum] == columnDoneThisPass) {
            FreeVarBind(vb);
            validInstanceSeen = TRUE;
        }
/*
 *  If this instance does not belong in this column (an
 *  object mismatch), or we hit the end of the visible MIB space,
 *  then it means this column is done.  We mark the column done 
 *  and discard the varBind.
 */
        else if ((CmpOIDClass(vb->name, vbt->object[objnum])) ||
               (vb->value.type == END_OF_MIB_VIEW_EXCEPTION)) {
            vbt->state[objnum] = columnDoneThisPass;
            FreeVarBind(vb);
            validInstanceSeen = TRUE;
        }
/*
 *  If this is the first instance in the column, sew it in.
 *  Out of lexicographic ordering case:  we have to check to see
 *  if we have already printed out an instance greater than the
 *  instance to be inserted.  If so, we drop the instance.
 *  If we had returned some instance and in the process emptied the
 *  column, free the object/instance information.
 */
        else if (vbt->bottom[objnum] == NULL) {
            diff = 1;			/* assume we'll insert */
            if (vbt->lastInstance[objnum] != NULL) {
                 diff = CmpOID(vb->name, vbt->lastInstance[objnum]); 
            }
            if (diff > 0) {
               vbt->bottom[objnum] = vbt->top[objnum] = vb;
               vb->next_var = NULL;
               validInstanceSeen = TRUE;
               if (vbt->lastInstance[objnum] != NULL) {
                    FreeOID(vbt->lastInstance[objnum]);
                    vbt->lastInstance[objnum] = NULL;
               }
            } else {
               FreeVarBind(vb);
            }
        }
/*
 *  We compare against the highest instance yet seen in this
 *  column.  In the ideal world, diff > 0 (the object encountered
 *  is lexicographically greater than the last guy added to the
 *  column), so we sew him in at the bottom.  The list is
 *  ordered from top (lowest instance; next object to be returned
 *  to caller) to bottom (highest instance), with links pointing
 *  towards the bottom.
 */
        else {
            diff = CmpOID(vb->name, vbt->bottom[objnum]->name);
            if (diff > 0) {
                vbt->bottom[objnum]->next_var = vb;
                vbt->bottom[objnum] = vb;
                vb->next_var = NULL;
                validInstanceSeen = TRUE;
            }
/* 
 *  v--- out of lexicographic ordering handling starts here ---v
 *
 *   The next block of code is to deal with bad lexicographical ordering
 *   from agent.   If the agent behaves correctly, none of this
 *   code should ever be executed.  It has been validated against
 *   Microsoft agents.
 *
 *   We ignore duplicate instances.  If we get to the end of 
 *   the PDU and have not seen one non-duplicate instance, we
 *   we give up with VBT_TABLE_DONE_LEX_BAD.  This is how we
 *   avoid looping.
 *
 *   If this instance is equal to the largest instance in the column, we
 *   toss him.
 */
            else if (diff == 0) {
                FreeVarBind(vb);
            }
/*
 *   Try to sew this varbind into the table at the appropriate location,
 *   trying from the top (since the links point down).  Dump him if he
 *   is a duplicate, or if he is smaller than any existing instances
 *   and retrieval has started (rows extracted from list != 0).
 */
            else {
                diff = CmpOID(vb->name, vbt->top[objnum]->name);
                if (diff < 0) {
                    if (vbt->rowsReady != 0 || vbt->rowsToMgr != 0) {
                        FreeVarBind(vb);
                    } else {
                        vb->next_var = vbt->top[objnum];
                        vbt->top[objnum] = vb;
                        validInstanceSeen = TRUE;
                    }
                } else if (diff == 0) {
                    FreeVarBind(vb);
                } else {  /* diff > 0 */
                    for (vbp = vbt->top[objnum]; 
                         vbp->next_var != NULL;
                         vbp = vbp->next_var)
                    {
                        diff = CmpOID(vb->name, vbp->next_var->name);
                        if (diff < 0) {
                            vb->next_var = vbp->next_var;
                            vbp->next_var = vb;
                            validInstanceSeen = TRUE;
                            break;
                        } else if (diff == 0) {
                            FreeVarBind(vb);
                            break;
                        } 
                    }
/*
 *  By the time we get here, we will have inserted the varbind
 *  or given up on him 
 */
                }   /* bad topo insertion loop */
            }    /* bad topo insertion case */
/* ^--- out of lexicalgraphic ordering handling ends here ---^ */
        }    /* insertion case */
    }    /* VarBindList loop */
/*
 *  If we have not seen any nonduplicate instances this pass
 *  we give up.
 */
    if (validInstanceSeen == FALSE) {
         retstatus = VBT_TABLE_DONE_LEX_BAD;
         goto done;
     }
/*
 *  Store column number of last column (object) we saw.
 *  Convert any columnDoneThisPass to columnDonePreviousPass.
 *  Build the next prototype varBind list.
 *  We build the next prototype varBind list in two passes.
 *  First we calculate the minimun instance both after (trailing)
 *  and before (leading) the last object we received, to build up a
 *  full row request.  We then
 *  build the next prototype varBind list using these minimum values.
 *  This done on suggestion by Dave Perkins that we _don't_ want to
 *  have holes in the varBinds we request, as this vitiates caching
 *  in the agent.  On querying David Battle (at Cisco) he thinks this
 *  is critical, too.
 *
 *  The next request template will look like 
 *
 *  [  . . . . . . l l l l l l ]
 *  [  t t t t t t . . . . . . ] 
 *               ^
 *
 *  where
 *
 *  .:  Not being requested in next request
 *  l:  Object requested with the minimum instance seen before 
 *      last object received
 *  t:  Object requested with the minimun instance seen 
 *  ^:  (Last object received)
 *
 */     
    vbt->objnum = objnum;   /* vbt->objnum stores last object seen */
    *nextRequest = NULL;
    do {
        objnum++;
        if (objnum == vbt->nobjects) {
            objnum = 0;
        }

        if (vbt->state[objnum] == columnDoneThisPass) {
            vbt->state[objnum] = columnDonePreviousPass; 
        }
#ifdef FUDGE_INSTANCES
        else if (vbt->state[objnum] == columnActive) {
            if (vbt->bottom[objnum] != NULL) {
                if (objnum > vbt->objnum) {
                    if (trailingColMinInstNum == -1) {
                        trailingColMinInstNum = objnum;
                    } else if (compare_instance(
                               vbt->bottom[objnum],
                               vbt->bottom[trailingColMinInstNum],
                               vbt->object[objnum],
                               vbt->object[trailingColMinInstNum]) < 0) 
                    {
                        trailingColMinInstNum = objnum;
                    }
                } else {
                    if (leadingColMinInstNum == -1) {
                        leadingColMinInstNum = objnum;
                    } else if (compare_instance(
                               vbt->bottom[objnum],
                               vbt->bottom[leadingColMinInstNum],
                               vbt->object[objnum],
                               vbt->object[leadingColMinInstNum]) < 0) 
                    {
                        leadingColMinInstNum = objnum;
                    }
                }
            }
        }
    } while (objnum != vbt->objnum);

/* 
 *  If we have identified leading and trailing columns with
 *  minimum instances, copy out the instance information pointers 
 *  for later merging with base OIDs for the getnext/bulk request.  
 */

    if (trailingColMinInstNum != -1) {
        trailingColMinInstance.oid_ptr = 
             vbt->bottom[trailingColMinInstNum]->name->oid_ptr +
             vbt->object[trailingColMinInstNum]->length;
         trailingColMinInstance.length = 
             vbt->bottom[trailingColMinInstNum]->name->length -
             vbt->object[trailingColMinInstNum]->length;
    }

    if (leadingColMinInstNum != -1) {
        leadingColMinInstance.oid_ptr = 
             vbt->bottom[leadingColMinInstNum]->name->oid_ptr +
             vbt->object[leadingColMinInstNum]->length;
         leadingColMinInstance.length = 
             vbt->bottom[leadingColMinInstNum]->name->length -
             vbt->object[leadingColMinInstNum]->length;
    }

    do {
        objnum++;
        if (objnum == vbt->nobjects) {
            objnum = 0;
        }   

        if (vbt->state[objnum] == columnActive)
#else /* FUDGE_INSTANCES */
        else if (vbt->state[objnum] == columnActive) 
#endif /* FUDGE_INSTANCES */
        {
            VarBind *new = NULL; 
            if (vbt->bottom[objnum] != NULL) {
#ifdef FUDGE_INSTANCES
                if (objnum > vbt->objnum) {
                    if (trailingColMinInstNum != -1) {
                        new = MakeVarBindWithNull(vbt->object[objnum],
                            &trailingColMinInstance);
                    }
                } else {
                    if (leadingColMinInstNum != -1) {
                        new = MakeVarBindWithNull(vbt->object[objnum],
                            &leadingColMinInstance);
                    }
                }
#else /* FUDGE_INSTANCES */
                new = MakeVarBindWithNull(vbt->bottom[objnum]->name, NULL);
#endif /* FUDGE_INSTANCES */
            } 
            if (!new) {
                new = MakeVarBindWithNull(vbt->object[objnum], 
                      vbt->initialInstance);
            }
            if (new == NULL) {
                retstatus = VBT_MALLOC_FAILURE;
                goto done;
            }
            if (*nextRequest == NULL) {
                *nextRequest = new;
                vbp = *nextRequest;
            } else {
                vbp->next_var = new;
                vbp = vbp->next_var;
            }
            vbp->next_var = NULL;
        }

    } while (objnum != vbt->objnum);

    if (*nextRequest == NULL) {
        retstatus = VBT_TABLE_DONE;
    }

done:
    switch (retstatus) {
    case VBT_OK:
    case VBT_BAD_NEXTREQUEST_ARGUMENT:
    default:					/* hush up compiler */
        return retstatus;

    case VBT_TABLE_DONE_LEX_BAD:
        vbt->vbtTableDoneLexBad = 1;
	/* fall through to cleanup */

    case VBT_TABLE_DONE:
    case VBT_MALLOC_FAILURE:
        vbt->vbtTableDone = 1;
	/* fall through to cleanup */
    }

    if (*nextRequest != NULL) {
        FreeVarBindList(*nextRequest);
        *nextRequest = NULL;
    }
    return retstatus;
}

/* ----------------------------------------------------------------------
 *
 *  vbTableRetrieveRows - Return the varBinds in the table
 *
 *  Arguments:
 *  i	(vbTable *) vbt
 *	Table to empty
 *
 *  o   (int *) maxRowsDone
 *	If return 1, then we have hit maxRows.  No need to do another
 * 	get.  Otherwise unaltered.
 *
 *  o	(VarBind *) (function value)
 *	Variable binding list to print.
 */
VarBind *
vbTableRetrieveRows(vbTable *vbt, int *maxRowsDone)
{
    VarBind *vblEnd = NULL;
    VarBind *vbl = NULL;		/* scratch varBind list ptr */


 
    if (vbt == NULL) {
        return NULL;
    }
    if (vbt->vbtTableAllReturned) {
        if (maxRowsDone) {
            *maxRowsDone = 1;
        }
        return NULL;
    }
/*
 *  We try to defer calling return_a_row as reasonably long as
 *  possible to accomodate buggy agents returning out-of-lexicographical
 *  order varBinds.  This increases the chance that table holes will be
 *  filled down the road.
 */ 

    if (!(vbt->vbtTableDone || 
          vbt->vbtTableDoneLexBad ||
          (vbt->rowsPerCallback == 1) ) )
    {
        if (vbt->rowsFromAgent == -1) {
            return NULL;
        }
        if (vbt->rowsFromAgent < vbt->goalRow) {
            return NULL;
        }
    }
/*
 *  Get as many rows as we can
 */
    while ((vbl = return_a_row(vbt, &vblEnd)) != NULL) {
        if (vbt->varbindsPendingReturn == NULL) {
            vbt->varbindsPendingReturn = vbl;
        } else { 
            vbt->varbindsPendingReturnEnd->next_var = vbl;
        }
        vbt->varbindsPendingReturnEnd = vblEnd;
        vbt->rowsReady++;

/* 
 *  If we have hit maxRowsToReturn, return what we have and set
 *  the "we're done" flag, so that on the next call we know we're
 *  done and we return NULL.
 */

        if ((vbt->rowsReady + vbt->rowsToMgr) == vbt->maxRowsToReturn) {
            vbt->vbtTableAllReturned = 1;	/* kills vbTableAdd */
            vbl = vbt->varbindsPendingReturn;
            vbt->varbindsPendingReturn = NULL;
            if (maxRowsDone) {
               *maxRowsDone = 1;
            }
            return vbl;
        }
/*
 *  If we have hit rowsPerCallback, return what we have
 */
        if (vbt->rowsReady == vbt->rowsPerCallback) {
            vbl = vbt->varbindsPendingReturn;
            vbt->varbindsPendingReturn = NULL;
            vbt->rowsToMgr += vbt->rowsReady;
            vbt->rowsReady = 0;
            vbt->goalRow += vbt->rowsPerCallback;
            return vbl;
        }
    }

/*
 *  If we are no longer retrieving rows from the agent, and have
 *  a few (less than rowsPerCallback) rows to return, return them
 *  now
 */
    if ((vbt->varbindsPendingReturn != NULL) && 
         (vbt->vbtTableDone ||
         vbt->vbtTableDoneLexBad)) 
    {
        vbt->vbtTableAllReturned = 1;
        vbl = vbt->varbindsPendingReturn;
        vbt->varbindsPendingReturn = NULL;
        return vbl;
    }
/*
 *  Set a new goal row 
 */
    vbt->goalRow = vbt->rowsToMgr + vbt->rowsPerCallback - vbt->rowsReady;
    return NULL;
}

/* ----------------------------------------------------------------------
 *
 *  vbTableFree - housekeeping routine to free vbTable structure
 *
 *  Argument:
 *  i	(vbTable *) vbt
 *	Table to free
 *
 *  Notes:
 *	vbTableFree is careful to check before freeing objects; so
 *	partially allocated tables may be safely freed
 */
void 
vbTableFree(vbTable *vbt) 
{
    int i;

    if (vbt == NULL) {
        return;
    }

    if (vbt->top != NULL) {
        for (i = 0; i < vbt->nobjects; i++) {
            if (vbt->top[i]) {
                FreeVarBindList(vbt->top[i]);
            }
        }
        free(vbt->top);
    }

    if (vbt->bottom != NULL) {
        free(vbt->bottom);
    }

    if (vbt->state != NULL) {
        free(vbt->state);
    }


    if (vbt->object != NULL) {
        for (i = 0; i < vbt->nobjects; i++) {
            if (vbt->object[i]) {
                FreeOID(vbt->object[i]);
            }
        }
        free(vbt->object);
    }
    
    if (vbt->smallestInstance != NULL) {
        free(vbt->smallestInstance);
    }

    if (vbt->varbindsPendingReturn != NULL) {
        FreeVarBindList(vbt->varbindsPendingReturn);
    }

    if (vbt->initialInstance != NULL) {
        FreeOID(vbt->initialInstance);
    }

    if (vbt->lastInstance != NULL) {
        for (i = 0; i < vbt->nobjects; i++) {
            if (vbt->lastInstance[i]) {
                FreeOID(vbt->lastInstance[i]);
            }
        }
        free(vbt->lastInstance);
    }

    free(vbt);
    return;
}

/* ----------------------------------------------------------------------
 *
 *  vbTableErrorStrings() - translate a vbTable error code
 *
 *  Arguments:
 *  i	(const int) errorCode
 *	Error code returned by vbTableAdd 
 *
 *  o	(const char *) (function value*
 *	Text associated with error message.  This text is
 *	statically allocated; don't free it.
 */
const char *
vbTableErrorStrings(int errorCode)
{
    switch (errorCode) {
    case VBT_OK:
	return "Operation successful";
    case VBT_TABLE_DONE: 
	return "Operation successful, table is complete";
    case VBT_MALLOC_FAILURE:
	return "malloc failure, vbt structure freed.";
    case VBT_TABLE_DONE_LEX_BAD:
        return "Table is complete;  all varbinds in last packet duplicates.";
    case VBT_BAD_NEXTREQUEST_ARGUMENT:
	return "Bad nextRequest argument pointer";
    case VBT_BAD_VBTABLE_ARGUMENT:
	return "Bad vbt (varBindTable) argument pointer";
    default:
        return "Unknown error code";
    }
}







/* eof */
