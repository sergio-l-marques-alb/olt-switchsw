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

#include <errno.h>


#include <malloc.h>

#include <stdlib.h>

#include <string.h>



#include "sr_type.h"
#include "sr_time.h"

#include "sr_snmp.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "snmpd.h"
#include "snmpv2d.h"


#include "frmt_lib.h"
#include "tc_time.h"
#include "prnt_lib.h"

#include "diag.h"
SR_FILENAME


#include "objectdb.h"
#include "chk_view.h"





#ifdef SETS

#ifdef SR_DEBUG
void MultiFunctionSetLogMessage
    SR_PROTOTYPE((LogEntry *log));
#endif /* SR_DEBUG */

/*----------------------------------------------------------------------
 * The do-list is a linked list with an allocated field. Traverse
 * the list, free the allocated element and the list itself.
 *----------------------------------------------------------------------*/
int cleanupDoList SR_PROTOTYPE((doList_t *doList));

int
cleanupDoList(doList)
    doList_t       *doList;
{
    doList_t       *p = doList;
    doList_t       *trash;
    int             cc;
    int             error_status = NO_ERROR;

    while (p != NULL) {
	trash = p;

	p = p->next;

	if (trash->data != NULL && trash->cleanupMethod != NULL) {

	    cc = (trash->cleanupMethod) (trash);

	    if (error_status == NO_ERROR && cc != NO_ERROR) {
		error_status = cc;
	    }
	    free((char *) trash);
	} else {
	    free((char *) trash);
        }
    }
    return error_status;
}				/* cleanupDoList */





/*----------------------------------------------------------------------
 * The do_sets function is divided into 3 passes.
 *
 * Pass 1: Perform simple error tests while building a do-list. Call
 *         the "test" method for the given object.
 *
 * Pass 2: Examine the results of testing all object sets. Some
 *         may not be valid if all "related" objects have not been
 *         set. (For example some tabular objects require several
 *         columns of a row to be specified)
 *
 * Pass 3: Perform the set method on each do-list item. If the set
 *         method fails there is problem with the test method.
 *
 *----------------------------------------------------------------------*/


/*

  Here is a thumbnail sketch of how do_sets has changed to incorporate undo:

  A new method type has been added to each doList element; the undoMethod.  The
  undoMethod has the same calling interface as the setMethod.  A new field has
  also been added to each doList element; the undodata field in analogy the
  the existing data field.

  It is the responsibility of the undoMethod, if called, to restore all mib
  variables which were affected by the set on the doList element which it
  is passed to the state they were in before the set operation.  If an
  undoMethod is unsuccessful in affecting this change, or if it is unable
  to assess whether or not it was successful, it should return undoFailed,
  or some other non 0 error code.  If it succeeds it should return noError.

  The undoMethod and undodata will be filled in by the test method, just as
  for the setMethod and data.  The undoMethod may be NULL, and if it is
  this indicates that the setMethod should not fail if the test method
  succeeds.  If undoMethod is not NULL, undodata should be filled in by the
  test method with a copy of what the row or scalar group looked like before
  the set began, stored in the same format as the data field.

  The freeMethod's name has been changed to cleanupMethod and its semantics
  have been changed as follows:

      1) it now returns an error code instead of void
      2) it has the added responsibility of finalizing any deletions made
         to groups/rows other than the one represented in undodata.  This
         is to allow the setMethod and undoMethod to cooperate by
         merely marking (or unmarking) rows in other groups for deletion.

  A further semantic change is made in pass 3 of do_sets in that doList
  elements which have undoMethods are set first so that if any of them fail,
  it will not be necessary to undo changes to rows/groups which lack
  undoMethods.

  If a set fails even though the cooresponding test succeeded, then further
  sets will be foregone and the undoMethod (if it exists) will be called for
  each doList item which had been successfully set.  If all the undo calls
  succeed then the error commitFailed is returned.  If one of the undo calls
  fails or does not exist, then further undo calls are foregone and the error
  undoFailed is returned.

  After the dust settles, either because a test failed, a set failed and undo's
  completed, an undo failed, or everything worked fine, the cleanupMethod for
  for each doList element is called.  If one of these fails, the others are
  still called, but an undoFailed error is returned.  It's a very bad thing
  for a cleanupMethod to fail.

  -David L. Battle
  July 1, 1993
 */



Pdu            *
do_sets_noblock(inPdu, contextInfo, viewName, payload)
    Pdu            *inPdu;
    ContextInfo    *contextInfo;
    OctetString    *viewName;
    int             payload;
{
    FNAME("do_sets")
    doList_t       *doListHead = NULL;
    doList_t       *doListCur = NULL;
    doList_t       *temp = NULL;
    int             leftOver = 0;
    SR_INT32        req = inPdu->u.normpdu.request_id;
    int             error_index;
    int             cc;
    VarBind        *curVb;
    ObjectInfo     *curObj;
    int             i;
    int             error_status;
    int             NonUndoablesStarted = 0;



    /*
     * Pass 1: Scan the entire variable binding list. On errors, construct an
     * error pdu and return it.  On success NULL is returned.
     */
    error_index = 0;

    for (curVb = inPdu->var_bind_list; curVb != NULL; curVb = curVb->next_var) {
	error_index++;

#if defined(SR_SNMPv3_ADMIN)
        /* (1)   If the variable binding's name specifies an existing or non-
         * existent variable to which this request is/would be denied
         * access because it is/would not be in the appropriate MIB view,
         * then the value of the Response-PDU's error-status field is set
         * to "noAccess", and the value of its error-index field is set to
         * the index of the failed variable binding.
         */

	if (viewName != NULL && (CheckMIBView(curVb->name, viewName) < 0)) {
	    DPRINTF((APACCESS, "%s: variable not in mib view.\n", Fname));
	    error_status = NO_ACCESS_ERROR;
	    goto fail;
	}
#endif	/* defined(SR_SNMPv3_ADMIN) */

        /* (2)   Otherwise, if there are no variables which share the same
         * OBJECT IDENTIFIER prefix as the variable binding's name, and
         * which are able to be created or modified no matter what new
         * value is specified, then the value of the Response-PDU's
         * error-status field is set to "notWritable", and the value of
         * its error-index field is set to the index of the failed
         * variable binding.
	 */

	if ((i = FindObjectClass(curVb->name, EXACT, payload)) == -1) {
	    DPRINTF((APTRACE, "%s: can't find object\n", Fname));
	    error_status = NOT_WRITABLE_ERROR;
	    goto fail;
	}
	curObj = GetOidElem(i);



        /* (3)   Otherwise, if the variable binding's value field specifies,
         * according to the ASN.1 language, a type which is inconsistent
         * with that required for all variables which share the same
         * OBJECT IDENTIFIER prefix as the variable binding's name, then
         * the value of the Response-PDU's error-status field is set to
         * "wrongType", and the value of its error-index field is set to
         * the index of the failed variable binding.
	 */

	if (curObj->oidtype != curVb->value.type &&
	    curObj->oidtype != ANY_TYPE) {
	    DPRINTF((APTRACE, "%s: value wrong type for object being set\n",
		     Fname));

	    error_status = WRONG_TYPE_ERROR;
	    goto fail;
	}


	/*
         * (9)   Otherwise, if the variable binding's name specifies a variable
         * which exists but can not be modified no matter what new value
         * is specified, then the value of the Response-PDU's error-status
         * field is set to "notWritable", and the value of its error-index
         * field is set to the index of the failed variable binding.
	 */

	if ((curObj->oidstatus & SR_ACCESS_MASK) != SR_READ_WRITE
	    && (curObj->oidstatus & SR_ACCESS_MASK) != SR_OLD_READ_CREATE
	    && (curObj->oidstatus & SR_ACCESS_MASK) != SR_READ_CREATE) {

	    DPRINTF((APTRACE, 
                     "%s: Attempt to set read-only object\n", Fname));

	    error_status = NO_ACCESS_ERROR;

#ifdef SR_SNMPv2_PDU
	    if (payload == SR_SNMPv2_PDU_PAYLOAD || 
                payload == SR_AGGREGATE_PDU_PAYLOAD) {
	     error_status = NOT_WRITABLE_ERROR;
	    }
#endif				/* SR_SNMPv2_PDU */

	    goto fail;
	}


	/*
	 * This variable binding has passed preliminary tests. Add it to the
	 * do-list. "leftOver" indicates the last element of the do-list is
	 * actually empty and left over from the previous iteration. (See
	 * the test method call below for details)
	 */
	if (!leftOver) {
	    temp = NULL;
	    if ((temp = (doList_t *) malloc(sizeof(doList_t))) == NULL) {
		DPRINTF((APTRACE, "%s: malloc failed\n", Fname));

		if (temp != NULL) {
		    free((char *) temp);
                }

		error_status = RESOURCE_UNAVAILABLE_ERROR;
		goto fail;
	    }
#ifdef SR_CLEAR_MALLOC
	    memset(temp, 0, sizeof(doList_t));
#endif	/* SR_CLEAR_MALLOC */

    	    if (doListHead == NULL) {
	    	doListHead = temp;
	    }
	    else {
		doListCur->next = temp;
	    }

	    doListCur = temp;
	}

	doListCur->setMethod = NULL;
	doListCur->cleanupMethod = NULL;
	doListCur->state = SR_UNUSED;
	doListCur->data = (void *) NULL;
	doListCur->next = NULL;
	doListCur->vbPos = error_index;
	doListCur->undoMethod = NULL;
	doListCur->undodata = (void *) NULL;
	doListCur->readyMethod = NULL;
	doListCur->vbSubPos = 0;


	/*
	 * Call the test method for this object. This method verifies the
	 * object instance, the object type (class), and its value. The test
	 * method may combine this object with a prior, "related" object in
	 * the do-list. If so, "doListCur->setMethod" will be NULL,
	 * indicating the set method need not be called for this do-list
	 * item. If this happens, the variable "leftOver" is set to indicate
	 * that during the next iteration of this loop another do-list
	 * element does not need to be allocated. (Note: this may leave a
	 * single, unused element in the do-list. This will not cause an
	 * error.)
	 */

	cc = (*curObj->test) (curVb->name, curObj, &curVb->value,
			      doListHead, doListCur, contextInfo);

	leftOver = (doListCur->state == SR_UNUSED);

	if (cc != NO_ERROR) {
	    DPRINTF((APTRACE, "%s: test method returned error\n", Fname));
            error_status = cc;
	    goto fail;
	}
    } /* for loop for each varbind in the varbind list */

    DPRINTF((APTRACE, "%s: pass 1 of set request processing done\n", Fname));



    /*
     * Pass 2: Some objects are members of relations, and as such must be set
     * atomicly. For example columns in a row must be set at once. 
     */

    /*
     * Walk through the do-list. If state is SR_PENDING_STATE, the ready
     * method for that entry has not been called, so call it now.
     */
    for (doListCur = doListHead;
         doListCur != NULL;
         doListCur = doListCur->next) {
        if (doListCur->state == SR_PENDING_STATE && 
                       doListCur->state != SR_NO_OP) {
            if (doListCur->readyMethod != NULL) {
                cc = (*doListCur->readyMethod) (doListHead, doListCur, 
                      contextInfo);
                if (cc != NO_ERROR) {
                    error_status = cc;
                    goto fail;
                }
            }
        }
    }

    /*
     * Scan the do-list and determine if all nodes in the list are either empty
     * (can occur if related items are combined into a single do-list
     * element), SR_ADD_MODIFY, SR_REPLACE, or SR_DELETE.
     */
    for (doListCur = doListHead; doListCur != NULL; doListCur = doListCur->next) {
        if (doListCur->state != SR_UNUSED && doListCur->state !=SR_ADD_MODIFY &&
            doListCur->state != SR_DELETE && doListCur->state != SR_REPLACE &&
            doListCur->state != SR_NO_OP) {

            if (doListCur->state == SR_UNKNOWN) {
	        DPRINTF((APTRACE, "%s: All related objects not set\n", Fname));
	        error_status = INCONSISTENT_VALUE_ERROR;
            } else if (doListCur->state == INCONSISTENT_NAME_STATE) {
	        DPRINTF((APTRACE, "%s: Inconsistent name in value\n", Fname));
	        error_status = INCONSISTENT_NAME_ERROR;
            } else if (doListCur->state == INCONSISTENT_VALUE_STATE) {
	        DPRINTF((APTRACE, "%s: Inconsistent value\n", Fname));
	        error_status = INCONSISTENT_VALUE_ERROR;
            } else {
	        DPRINTF((APTRACE, "%s: Unknown state\n", Fname));
	        error_status = INCONSISTENT_VALUE_ERROR;
            }
	    error_index = doListCur->vbPos;
	    goto fail;
	}
    }

    DPRINTF((APTRACE, "%s: pass 2 of set request processing done\n", Fname));

    /*
     * Pass 3: Now actually do the set. For each element of the do-list in
     * which the set_func is non-NULL, call that set method. This should
     * never fail since all tests should be done in the test method.
     */

    /*
     * First do just the sets that have undo methods to maximize the
     * probability that we'll be able to undo everything if one of them
     * fails.
     */

    for (doListCur = doListHead;
         doListCur != NULL;
         doListCur = doListCur->next) {
        if (doListCur->setMethod != NULL && doListCur->undoMethod != NULL &&
            doListCur->state != SR_NO_OP) {
            cc = (*doListCur->setMethod) (doListHead, doListCur, contextInfo);
            if (cc != NO_ERROR) {
                goto undo;
            }
        }
    }


    /* Do the sets that don't have undo methods */
    for (doListCur = doListHead;
         doListCur != NULL;
         doListCur = doListCur->next) {
	if ((doListCur->setMethod != NULL) &&
            (doListCur->undoMethod == NULL &&
             doListCur->state != SR_NO_OP)) {

	    cc = (*doListCur->setMethod) (doListHead, doListCur, contextInfo);

	    if (cc != NO_ERROR) {
		goto undo;
	    }
	    NonUndoablesStarted = 1;
	}
    }

undo:
    if (doListCur != NULL) {
	/*
	 * If any of these assignments fail (even after all the previous
	 * validations), then all other assignments are undone, and the
	 * Response-PDU is modified to have the value of its error-status
	 * field set to `commitFailed', and the value of its error-index
	 * field set to the index of the failed variable binding.
	 */
	DPRINTF((APTRACE, "%s: set failed, undoing\n", Fname));
	for (temp = doListHead; temp != doListCur; temp = temp->next) {
	    if (temp->state == SR_NO_OP) {
                continue;
            }
	    if (temp->undoMethod == NULL) {
		if (NonUndoablesStarted) {
		    break;
		}
		continue;
	    }
	    cc = (*temp->undoMethod) (doListHead, temp, contextInfo);
	    if (cc != NO_ERROR) {
		break;
	    }
	}
	if (temp != doListCur) {
	    /*
	     * If and only if it is not possible to undo all the assignments,
	     * then the Response-PDU is modified to have the value of its
	     * error-status field set to `undoFailed', and the value of its
	     * error-index field is set to zero.  Note that implementations
	     * are strongly encouraged to take all possible measures to avoid
	     * use of either `commitFailed' or `undoFailed' - these two
	     * error-status codes are not to be taken as license to take the
	     * easy way out in an implementation.
	     */
	    DPRINTF((APTRACE, "%s: undo failed\n", Fname));
	    error_status = UNDO_FAILED_ERROR;
	    error_index = 0;
	}
	else {
	    error_status = COMMIT_FAILED_ERROR;
	    error_index = doListCur->vbPos;
	}
	goto fail;
    }

    DPRINTF((APTRACE, "%s: pass 3 of set request processing done\n", Fname));

    if (cleanupDoList(doListHead) != NO_ERROR) {
	error_status = UNDO_FAILED_ERROR;
	error_index = 0;
	goto fail;
    }

    snmpData.snmpInTotalSetVars += error_index;

    return (NULL);


fail:
    if (doListHead != NULL) {
	if (cleanupDoList(doListHead) != NO_ERROR) {
	    error_status = UNDO_FAILED_ERROR;
            error_index = 0;
	}
    }

#ifdef SR_SNMPv1_PACKET
    if ((payload == SR_SNMPv1_PDU_PAYLOAD) &&
        ((error_status == UNDO_FAILED_ERROR) ||
         (error_status == COMMIT_FAILED_ERROR))) {
        error_status = GEN_ERROR;
    }
    if ((payload == SR_SNMPv1_PDU_PAYLOAD) &&
        (error_status == NO_ACCESS_ERROR)) {
        snmpData.snmpInBadCommunityUses++;
    }
#endif /* SR_SNMPv1_PACKET */


       return (make_error_pdu(GET_RESPONSE_TYPE, req, error_status,
                              error_index, inPdu, payload));

}				/* do_sets() */

Pdu            *
do_sets(inPdu, contextInfo, viewName, payload)
    Pdu            *inPdu;
    ContextInfo    *contextInfo;
    OctetString    *viewName;
    int             payload;
{
    return do_sets_noblock(inPdu, contextInfo, viewName, payload);
}

/*----------------------------------------------------------------------
 * This function should never be called. If it is, "do_sets()" or the
 * object data base has been corrupted.
 *----------------------------------------------------------------------*/
/*ARGSUSED*/
int
readOnly_test(incoming, object, value, doHead, doCur, auth)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *auth;
{
    FNAME("readOnly_test")
    DPRINTF((APALWAYS, "%s: Internal error; attempted to set a read-only variable.\n",
	     Fname));
    return (GEN_ERROR);
}				/* readOnly_test() */

#ifdef SR_DEBUG
void
PrintSetAuditMessage(SnmpMessage *snmp_msg, Pdu *in_pdu_ptr, 
                     SR_INT32 success, TransportInfo *srcTI)
{
    char *buffer = NULL;
    OctetString *time_os = NULL;
    VarBind *cur_vb;
    LogFunc old_log_ptr;
    char tstr[32];

    if (!(GetLogLevel() & APAUDIT)) {
        goto done;
    }

    if ( (snmp_msg == NULL) || (in_pdu_ptr == NULL) || (srcTI == NULL) ) {
        goto done;
    }
 
    buffer = malloc(4096);
    if (buffer == NULL) {
        goto done;
    }

    old_log_ptr = SetLogFunction(MultiFunctionSetLogMessage);

#ifdef SR_CLEAR_MALLOC
    memset(buffer, 0, 4096); 
#endif /* SR_CLEAR_MALLOC */

    if (success) {
         DPRINTF((APAUDIT, "\nSuccessful SET request."));
    }
    else {
         DPRINTF((APAUDIT, "\nUnsuccessful SET request."));
    }
    switch (snmp_msg->version) {
#ifdef SR_SNMPv1_WRAPPER
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
            DPRINTF((APAUDIT, "SNMPv1 SET request."));
            SPrintAscii(snmp_msg->u.v1.community, buffer);
            DPRINTF((APAUDIT, "Community: %s", buffer));
            break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            DPRINTF((APAUDIT, "SNMPv2c SET request."));
            SPrintAscii(snmp_msg->u.v1.community, buffer);
            DPRINTF((APAUDIT, "Community: %s", buffer));
            break;
#endif /* SR_SNMPv2c_PACKET */
#endif /* SR_SNMPv1_WRAPPER */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
            if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_NOAUTH) {
                DPRINTF((APAUDIT, "SNMPv3 noauth/nopriv SET request."));
            }
#ifndef SR_UNSECURABLE
            else if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_AUTH) {
                DPRINTF((APAUDIT, "SNMPv3 auth/nopriv SET request."));
            }
#ifndef SR_NO_PRIVACY
            else if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_PRIV) {
                DPRINTF((APAUDIT, "SNMPv3 auth/priv SET request."));
            }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
            SPrintAscii(snmp_msg->u.v3.userName, buffer);
            DPRINTF((APAUDIT, "Username: %s", buffer));
            break;
#endif /* SR_SNMPv3_PACKET */
        default:
            break;
    }

    DPRINTF((APAUDIT, "Message from %s.", 
             FormatTransportString(tstr, sizeof(tstr), srcTI)));

#ifdef SR_CLEAR_MALLOC
    memset(buffer, 0, 4096); 
#endif /* SR_CLEAR_MALLOC */

    time_os = GetSystemDateAndTime(NULL, 8);
    if (time_os != NULL) {
        SPrintDateAndTime(time_os, buffer, 4096);
        DPRINTF((APAUDIT, "Time: %s", buffer));    
        FreeOctetString(time_os);
        time_os = NULL;
    }
     
    DPRINTF((APAUDIT, "VarBind List: "));
    for (cur_vb = in_pdu_ptr->var_bind_list; cur_vb != NULL; cur_vb = cur_vb->next_var) {
   
#ifdef SR_CLEAR_MALLOC
        memset(buffer, 0, 4096); 
#endif /* SR_CLEAR_MALLOC */
        SPrintVarBind(cur_vb, buffer);
        DPRINTF((APAUDIT, "%s", buffer));
    }
    DPRINTF((APAUDIT, "\n"));
    SetLogFunction(old_log_ptr);
done:
    if (buffer != NULL) {
        free(buffer);
    }
    return;
}

void
MultiFunctionSetLogMessage(log)
    LogEntry *log;
{
    const LogAppData        *data;
    void                    *temp;
    static const LogAppData  defaultdata = {
        SRLOG_STDERR,
        0,
        NULL
    };

    if (log) {

        /*
         * UserData in the LogEntry must be overwritten to pass
         * the structure to other log functions.  Restore the
         * old value from temp before returning from this function.
         */
        temp = log->UserData;


        data = (LogAppData *) log->UserData;
        if (data == NULL) {
            data = &defaultdata;
        }

        /* --------------- Standard I/O --------------- */
        if (data->operFlags & SRLOG_STDERR) {
            log->UserData = (void *) stderr;
            PrintSetLogMessage(log);
        }
        if (data->operFlags & SRLOG_STDOUT) {
            log->UserData = (void *) stdout;
            PrintSetLogMessage(log);
        }

        /* --------------- File I/O --------------- */
        if (data->operFlags & SRLOG_FILE) {
            if (log->LogLevel & ~(APERROR | APWARN)) {
                /* this log message is neither APERROR nor APWARN */
                if (!(data->operFlags & SRLOG_TRACEFILE)) {
                    /* the -log_tracefile flag has not been specified */
                    /* do not print this message to a file */
                    log->UserData = temp;
                    return;
                }
            }
            log->UserData = (void *) data->logfile_fp;
            PrintSetLogMessage(log);
        }


        /* restore the old value from temp */
        log->UserData = temp;
    }
}
#endif /* SR_DEBUG */
#endif				/* SETS */
