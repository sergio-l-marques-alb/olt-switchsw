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
#include "sitedefs.h"
#include "snmpd.h"

#include "diag.h"
#include "oid_lib.h"
SR_FILENAME



#include "objectdb.h"


extern void SendGetPduDoneEvent
    SR_PROTOTYPE((int serialNum));

int serialNum = 0;


/*------------------------------------------------------------
 * Process GET and GET-NEXT requests.
 *------------------------------------------------------------*/
Pdu            *
do_response(inpdu, contextInfo, limit, viewName, payload)
    Pdu            *inpdu;
    ContextInfo    *contextInfo;
    SR_UINT32       limit;
    OctetString    *viewName;
    int             payload;
{
    Pdu            *outpdu;
    VarBind        *invb, *outvb;
    int             searchType;
    int             varCounter;
    SR_INT32        req;
    SR_UINT32       total_so_far = 0;
    int             error,local_serialNum;
    SR_INT32        FailedVarBind = 0;
    OID            *incoming = NULL;
    SR_INT32        mms_remaining;

#ifdef SR_SNMPv2_PDU
    SR_INT32            non_repeaters;
    SR_INT32            max_repetitions;
    SR_INT32            N, M, R;
    SR_INT32            i, r;
    int             All_endOfMibView;
    VarBind        *LastNonRepeater;

    LastNonRepeater = NULL;
#endif				/* SR_SNMPv2_PDU */



    outpdu = NULL;
    outvb = NULL;
    invb = NULL;
    local_serialNum=++serialNum;


    searchType = inpdu->type;

#ifdef SR_SNMPv2_PDU
    if (searchType == GET_BULK_REQUEST_TYPE) {
	searchType = NEXT;
    }
#endif				/* SR_SNMPv2_PDU */

    req = inpdu->u.normpdu.request_id;

    /*
     * Allocate the pdu to use for the response if successful
     */

       if ((outpdu = MakePdu(GET_RESPONSE_TYPE, req, (SR_INT32) NO_ERROR, 
                          ((SR_INT32)0), (OID *) NULL, (OctetString *) NULL, 
                          ((SR_INT32)0), ((SR_INT32)0), ((SR_INT32)0))) == NULL) {
   	   return (NULL);
       }

#ifdef SR_SNMPv2_PDU
    if (inpdu->type != GET_BULK_REQUEST_TYPE) {
#endif				/* SR_SNMPv2_PDU */

	varCounter = 0;

	/*
	 * For each variable, look up the object, and retrieve its value.
	 */
	for (invb = inpdu->var_bind_list; invb; invb = invb->next_var) {
	    incoming = invb->name;
            mms_remaining = limit - total_so_far;
            SrUpdateContextInfoRecord(contextInfo, NCI_MMS_REMAINING, &mms_remaining);
 
	    if (searchType == EXACT) {
		outvb =
		    GetExactObjectInstance(incoming, viewName, contextInfo,
					   local_serialNum, payload);
	    }
	    else {		/* if(searchType == NEXT) */
		outvb =
		    GetNextObjectInstance(incoming, viewName, contextInfo,
					  local_serialNum, payload);
	    }

	    if (outvb == NULL) {
		FailedVarBind = varCounter + 1;
		error = NO_ACCESS_ERROR;

#ifdef SR_SNMPv2_PDU
		if (payload == SR_SNMPv2_PDU_PAYLOAD || 
                    payload == SR_AGGREGATE_PDU_PAYLOAD) {
		    error = GEN_ERROR;
		}
#endif				/* SR_SNMPv2_PDU */

#ifdef SR_SNMPv1_PACKET
                if (payload == SR_SNMPv1_PDU_PAYLOAD) { 
                    snmpData.snmpInBadCommunityUses++;
                }
#endif /* SR_SNMPv1_PACKET */

		goto fail;
	    }
	        total_so_far += FindLenVarBind(outvb);
	    if (total_so_far >= limit) { 
		FreeVarBind(outvb);
		error = TOO_BIG_ERROR;
		FailedVarBind = 0;
		goto fail;
	    }			/* if limit */
	    LinkVarBind(outpdu, outvb);
	    varCounter++;
	}			/* for */

	snmpData.snmpInTotalReqVars += varCounter;


	return outpdu;

#ifdef SR_SNMPv2_PDU
    }				/* endof not getbulk */
    else {			/* GET BULK request */

	/*
	 * Upon receipt of a GetBulkRequest-PDU, the receiving SNMPv2 entity
	 * processes each variable binding in the variable-binding list to
	 * produce a Response-PDU with its request-id field having the same
	 * value as in the request.  Processing begins by examining the
	 * values in the non-repeaters and max-repetitions fields.  If the
	 * value in the non-repeaters field is less than zero, then the value
	 * of the field is set to zero.  Similarly, if the value in the
	 * max-repetitions field is less than zero, then the value of the
	 * field is set to zero.
	 */

	non_repeaters = inpdu->u.bulkpdu.non_repeaters;
	max_repetitions = inpdu->u.bulkpdu.max_repetitions;
	if (non_repeaters < 0) {
	    non_repeaters = 0;
        }
	if (max_repetitions < 0) {
	    max_repetitions = 0;
        }
        DPRINTF((APTRACE,"do_response: request non-repeaters %d and max-repetitions %d\n",  non_repeaters, max_repetitions));

	/*
	 * The values of the non-repeaters and max-repetitions fields in the
	 * request specify the processing requested.  One variable binding in
	 * the Response-PDU is requested for the first N variable bindings in
	 * the request and M variable bindings are requested for each of the
	 * R remaining variable bindings in the request.  Consequently, the
	 * total number of requested variable bindings communicated by the
	 * request is given by N + (M * R), where N is the minimum of: a) the
	 * value of the non-repeaters field in the request, and b) the number
	 * of variable bindings in the request; M is the value of the
	 * max-repetitions field in the request; and R is the maximum of: a)
	 * number of variable bindings in the request - N, and b)  zero.
	 */

	N = MIN(non_repeaters, inpdu->num_varbinds);
	M = max_repetitions;
	R = MAX(inpdu->num_varbinds - N, 0);

	/*
	 * If N is greater than zero, the first through the (N)-th variable
	 * bindings of the Response-PDU are each produced as follows:
	 * 
	 * (1)  The variable is located which is in the lexicographically
	 * ordered list of the names of all variables which are accessible by
	 * this request and whose name is the first lexicographic successor
	 * of the variable binding's name in the incoming GetBulkRequest-PDU.
	 * The corresponding variable binding's name and value fields in the
	 * Response-PDU are set to the name and value of the located
	 * variable.
	 * 
	 * (2)  If the requested variable binding's name does not
	 * lexicographically precede the name of any variable accessible by
	 * this request, i.e., there is no lexicographic successor, then the
	 * corresponding variable binding produced in the Response-PDU has
	 * its value field set to `endOfMibView', and its name field set to
	 * the variable binding's name in the request.
	 */
	for (i = 1, invb = inpdu->var_bind_list; i <= N; i++, invb = invb->next_var) {
            mms_remaining = limit - total_so_far;
            SrUpdateContextInfoRecord(contextInfo, NCI_MMS_REMAINING,
                                      &mms_remaining);


	    outvb = GetNextObjectInstance(invb->name, viewName, contextInfo,
                                          local_serialNum, payload);

	    if (outvb == NULL) {
		FailedVarBind = i;
		error = GEN_ERROR;
		goto fail;
	    }
   	      total_so_far += FindLenVarBind(outvb);
	    if (total_so_far >= limit) {
                DPRINTF((APWARN, "do_response: getbulk exceeds mms size limit\n"));
		FreeVarBind(outvb);
		goto done;
	    }			/* if limit */
	    LinkVarBind(outpdu, outvb);
	}
	LastNonRepeater = outvb;

	/*
	 * If M and R are non-zero, the (N + 1)-th and subsequent variable
	 * bindings of the Response-PDU are each produced in a similar
	 * manner.  For each iteration i, such that i is greater than zero
	 * and less than or equal to M, and for each repeated variable, r,
	 * such that r is greater than zero and less than or equal to R, the
	 * (N + ( (i-1) * R ) + r)-th variable binding of the Response-PDU is
	 * produced as follows:
	 * 
	 * (1)  The variable which is in the lexicographically ordered list of
	 * the names of all variables which are accessible by this request
	 * and whose name is the (i)-th lexicographic successor of the (N +
	 * r)-th variable binding's name in the incoming GetBulkRequest-PDU
	 * is located and the variable binding's name and value fields are
	 * set to the name and value of the located variable.
	 * 
	 * (2)  If there is no (i)-th lexicographic successor, then the
	 * corresponding variable binding produced in the Response- PDU has
	 * its value field set to `endOfMibView', and its name field set to
	 * either the last lexicographic successor, or if there are no
	 * lexicographic successors, to the (N + r)-th variable binding's
	 * name in the request.
	 */

	for (i = 1; i <= M; i++) {
	    All_endOfMibView = 1;
	    if (i == 2) {
		if (LastNonRepeater != NULL) {
		    invb = LastNonRepeater->next_var;
		}
		else {
		    invb = outpdu->var_bind_list;
		}
	    }

	    for (r = 1; r <= R; r++, invb = invb->next_var) {
		incoming = invb->name;
                mms_remaining = limit - total_so_far;
                SrUpdateContextInfoRecord(contextInfo, NCI_MMS_REMAINING,
                                          &mms_remaining);

		outvb = GetNextObjectInstance(incoming, viewName, contextInfo,
					      local_serialNum, payload);

		if (outvb == NULL) {
		    FailedVarBind = N + r;
		    error = GEN_ERROR;
		    goto fail;
		}


		if (outvb->value.type != END_OF_MIB_VIEW_EXCEPTION) {
		    All_endOfMibView = 0;
		}

 		  total_so_far += FindLenVarBind(outvb);
		if (total_so_far < limit) {
		    LinkVarBind(outpdu, outvb);
		}		/* if limit */
		else {
		    FreeVarBind(outvb);
		    goto done;
		}
	    }
	    if (All_endOfMibView) {
		goto done;
            }
	}

done:

	/*
	 * While the maximum number of variable bindings in the Response-PDU
	 * is bounded by N + (M * R), the response may be generated with a
	 * lesser number of variable bindings (possibly zero) for either of
	 * two reasons.
	 * 
	 * (1)  If the size of the message encapsulating the Response-PDU
	 * containing the requested number of variable bindings would be
	 * greater than either a local constraint or the maximum message size
	 * of the request's source party, then the response is generated with
	 * a lesser number of variable bindings.  This lesser number is the
	 * ordered set of variable bindings with some of the variable
	 * bindings at the end of the set removed, such that the size of the
	 * message encapsulating the Response-PDU is approximately equal to
	 * but no greater than the minimum of the local constraint and the
	 * maximum message size of the request's source party.  Note that the
	 * number of variable bindings removed has no relationship to the
	 * values of N, M, or R.
	 * 
	 * (2)  The response may also be generated with a lesser number of
	 * variable bindings if for some value of iteration i, such that i is
	 * greater than zero and less than or equal to M, that all of the
	 * generated variable bindings have the value field set to the
	 * `endOfMibView'.  In this case, the variable bindings may be
	 * truncated after the (N + (i R))-th variable binding.
	 */



	return outpdu;

	/*
	 * If the processing of any variable binding fails for a reason other
	 * than listed above, then the Response-PDU is re-formatted with the
	 * same values in its request-id and variable-bindings fields as the
	 * received GetBulkRequest-PDU, with the value of its error-status
	 * field set to `genErr', and the value of its error-index field is
	 * set to the index of the failed variable binding.
	 */
    }				/* if BULK */
#endif				/* SR_SNMPv2_PDU */

fail:


    if (outpdu != NULL)
	FreePdu(outpdu);

        return make_error_pdu(GET_RESPONSE_TYPE, req, error,
    			      (int) FailedVarBind, inpdu,
			      payload);
}
