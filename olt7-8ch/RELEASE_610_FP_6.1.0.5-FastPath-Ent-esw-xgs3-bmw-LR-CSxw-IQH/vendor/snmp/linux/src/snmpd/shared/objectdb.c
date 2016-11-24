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

#include <sys/types.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include <unistd.h>








#include "sr_time.h"

#include "sr_snmp.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "snmpprot.h"
#include "snmpsupp.h"
#include "snmpd.h"
#include "sitedefs.h"
#include "snmpv2d.h"
#include "getvar.h"
#include "diag.h"
SR_FILENAME

#include "oid_lib.h"		/* for CmpNOID() and CmpOID() */
#include "objectdb.h"
#include "inst_lib.h"
#include "makevb.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "min_v.h"
#ifdef SR_DEBUG
#include "prnt_lib.h" 		/* for PrintOID() */
#endif /* SR_DEBUG */
#include "compat.h"



extern vacmViewTreeFamilyEntry_t *LookupViewFamily(       
    OctetString *viewName,
    int         *index);
 
extern SnmpV2Table agt_vacmViewTreeFamilyTable;



#define SNMPPART_DECL
#undef SR_SNMPPART_H
#undef STdeclare
#include "snmppart.h"
#undef  SNMPPART_DECL

/* the objects internal to the agent */
ObjectInfo      OidList[] = {
#include "snmpoid.c"
};

int             NumOIDelem = (sizeof(OidList) / sizeof(OidList[0]));


#define OidElem(i) (&OidList[i])
#define TotalObjects NumOIDelem





/* 
 * find_object:   This routine performs a binary search of the dispatch
 *                table array to find an OID match in table.
 *
 *     NOTE:  This function is only used for EMANATE/Lite and may not 
 *            work correctly if subtree registration is used.  
 *            If a subtree is registered in the dispatch table and 
 *            additional objects or subtrees *under* the subtree are
 *            also registered and those objects/subtrees come *before*
 *            the OID on which the lookup is being performed, then the
 *            correct subtree entry for the OID requested may not be
 *            able to be found in the dispatch table using the algorithm
 *            below. The binary search algorithm below works well for
 *            if individual ojects are registered but not for subtrees.
 */               
int
find_object(oid, searchType)
    OID            *oid;
    int             searchType;
{
    /*
     * Do a binary search in the OidList ObjectInfo array for an object
     * matching oid.
     */
    SR_INT32            low, high, mid = 0;
    int             cmp = -1;
    ObjectInfo     *oi;


    low = 0;
    high = TotalObjects - 1;

    while (low <= high) {
	mid = (low + high) / 2;

	oi = OidElem(mid);
	cmp = CmpNOID(oid, &(oi->oid), oi->oid.length);

	if (cmp < 0) {
	    high = mid - 1;
	}
	else if (cmp > 0) {
	    low = mid + 1;
	}
	else {
	    break;
	}
    }

    if (searchType == EXACT) {
	if (cmp == 0) {
	    return mid;
	}
    }
    else {			/* if(searchType == NEXT) */
	if (cmp > 0) {
	    mid++;
	}

	if (mid < TotalObjects) {
	    return mid;
	}
    }

    return -1;
}

#ifdef SR_SNMPv2_PDU
int
IsV2Type(type)
int type;
{
    if (type == COUNTER_64_TYPE) {
	return 1;
    }
    return 0;
}
#else
#define IsV2Type(x) 0
#endif /* SR_SNMPv2_PDU */


int
NextObjectClass(i, payload)
    int             i, payload;
{
    i++;


    while (i < TotalObjects) {

        /* Skip objects incompatible with PDU payload type */
#ifdef SR_SNMPv1_PACKET
        if (payload == SR_SNMPv1_PDU_PAYLOAD &&
            IsV2Type((int)(OidElem(i)->oidtype))) {
           i++;
           continue;
        } 
#endif /* SR_SNMPv1_PACKET */
 
        break;
 
    }

    if (i < TotalObjects) {
	return i;
    }
    return -1;
}

int
FindObjectClass(oid, searchType, payload)
    OID            *oid;
    int             searchType, payload;
{
    FNAME("FindObjectClass")
    int             i;

    if ((i = find_object(oid, searchType)) == -1) 
    {
	DPRINTF((APTRACE, "%s: findObject failed\n", Fname));
	return -1;
    }

#ifdef SR_SNMPv1_PACKET
    if (payload == SR_SNMPv1_PDU_PAYLOAD) {
	if (searchType == EXACT && IsV2Type((int)(OidElem(i)->oidtype))) {
	    DPRINTF((APTRACE, "%s: searchType exact on non v1 object\n", Fname));
	    return -1;
	}
	while (i < TotalObjects &&
	       IsV2Type((int)(OidElem(i)->oidtype))) {
	    i++;
	}
	if (i > TotalObjects) {
	    DPRINTF((APTRACE, "%s: ran off end of mib\n", Fname));
	    return -1;
	}
    }
#endif                          /* SR_SNMPv1_PACKET */


    return i;
}

/* return non-zero if a might contain next of b */
/* note this could also be called "MightContainSomethingLargerThan" */
int
MightContainNextOf(a, b)
OID *a, *b;
{
    /* if a <= b then a must be a prefix of b */
    /* if a > b then fine */

    if(CmpOID(a,b) <= 0) {
	if((a->length <= b->length) && (CmpNOID(a, b, a->length) == 0)) {
	    return 1;
	}
	return 0;
    }
    return 1;
}



VarBind        *
GetExactObjectInstance(incoming, viewName, contextInfo, serialNum, payload)
    OID            *incoming;
    OctetString    *viewName;
    ContextInfo    *contextInfo;
    int             serialNum, payload;
{
    VarBind        *outvb;
    int             i;



    if ((i = FindObjectClass(incoming, EXACT, payload)) == -1 ||
	((viewName != (OctetString *) NULL) && 
        (CheckClassMIBView(&(OidElem(i)->oid), viewName) < 0))) {
	DPRINTF((APTRACE, "No such object.\n"));
	return exception(NO_SUCH_OBJECT_EXCEPTION, incoming, payload);
    }

    outvb = NULL;
    if ((viewName == (OctetString *) NULL) || 
        (CheckMIBView(incoming, viewName) > 0)) {

	GetMethod getfunc = OidElem(i)->get;

#ifndef LIGHT
	DPRINTF((APTRACE, "Searching for requested instance of %s\n",
		 OidElem(i)->oidname));
#endif				/* LIGHT */

	/* normal get method */
	    outvb = (*getfunc) (incoming, OidElem(i), EXACT,
			    contextInfo, serialNum);
            if (outvb != NULL) {
	        return outvb;
            }
	    goto no_instance_exception; 

    }


no_instance_exception:

#ifndef LIGHT
    DPRINTF((APTRACE, "No such instance.\n"));
#endif
        
    return exception(NO_SUCH_INSTANCE_EXCEPTION, incoming, payload);
}

/*
 * This function returns the next instace of any mib object which is greater
 * than incoming relative to the view identified by viewName.  contextInfo,
 * and serialNum are passed as parameters to any method routine call(s).
 * if payload is SNMPv1_DOMAIN then all v2 type objects (bitstrings,
 * counter64's) are skipped over.  Returns a varbind containing the next
 * accessible object, or an exception or NULL (depending on payload) if end of
 * mib is encountered.
 */

VarBind        *
GetNextObjectInstance(incoming, viewName, contextInfo, serialNum, payload)
    OID            *incoming;
    OctetString    *viewName;
    ContextInfo    *contextInfo;
    int             serialNum, payload;
{
    VarBind        *outvb, *tmpoutvb;
    int             dopost = 1;
    OID            *local_incoming = incoming;
    int             i;

    /* we do one search, from then on we just increment to the next object. */
    if ((i = FindObjectClass(local_incoming, NEXT, payload)) == -1) {
	goto end_of_mib;
    }

    if ( ((OidElem(i)->oidstatus & FULLY_QUALIFIED) == FULLY_QUALIFIED ) &&
         (CmpNOID(&(OidElem(i)->oid), local_incoming, 
                  OidElem(i)->oid.length) == 0) ) {
        /* match is a fully qualified OID equal to incoming OID, find
         * skip to next possible object class
         */
        i = NextObjectClass(i, payload);
    }



    if (MightContainNextOf(&(OidElem(i)->oid), local_incoming)){
       if ((OidElem(i)->oidstatus & FULLY_QUALIFIED) != FULLY_QUALIFIED &&
           CmpOID(incoming, &(OidElem(i)->oid)) < 0) {
           local_incoming = &(OidElem(i)->oid);
       } 
    }

    /*
     * Outer loop until we find a variable class we can access or run out of
     * variable classes.
     */
    do {
	outvb = NULL;
	if ((viewName == (OctetString *) NULL) || 
            ((dopost = CheckClassMIBView(&(OidElem(i)->oid), viewName)) >= 0)) {

	    /*
	     * Inner loop until we find an instance we can access or run out
	     * of instances (get method returns NULL).
	     */
	    do {
		GetMethod getfunc = OidElem(i)->get;
#ifndef LIGHT
		DPRINTF((APACCESS, "Searching for next instance of %s\n",
			 OidElem(i)->oidname));
#endif				/* LIGHT */
		        /* normal get method */
		        tmpoutvb = (*getfunc) (local_incoming, OidElem(i),
		    			       NEXT, contextInfo, serialNum);


		/* we put off freeing outvb until here because local_incoming
		   may be pointing at outvb->name */
                if (outvb != NULL) {

                    FreeVarBind(outvb);
                    outvb = NULL;
                }

		outvb = tmpoutvb;
		if (outvb != NULL) {
		    local_incoming = outvb->name;
		} else {
                    local_incoming = incoming;
                }

 
	    } while (outvb != NULL &&
		     dopost != 1 &&
		     CheckMIBView(outvb->name, viewName) < 0);
	}

	if (outvb != NULL)
	    break;

	i = NextObjectClass(i, payload);


        if (i != -1) {

           while (i != -1 && i < TotalObjects) {
              if ((OidElem(i)->oidstatus & FULLY_QUALIFIED) == FULLY_QUALIFIED){
                  if (CmpNOID(&(OidElem(i)->oid), incoming, 
                                  OidElem(i)->oid.length) == 0) {
                      /* match is a fully qualified OID equal to incoming OID, 
                       * skip to next possible object class
                       */
                      i = NextObjectClass(i, payload);
                      if (i == -1) {
                          break;
                      }
                      continue;
                  }
              }
              break;      /* all other cases */
           }

           if (i != -1) {
               local_incoming = &(OidElem(i)->oid);
           }
        }

    } while (i != -1);

    if (outvb != NULL) {
	return outvb;
    }

    /* fall through */

end_of_mib:
    DPRINTF((APTRACE, "End of mib.\n"));
    return exception(END_OF_MIB_VIEW_EXCEPTION, incoming, payload);
}



#ifdef SR_SNMPv2_PDU
/*
 * Construct an exception VarBind.  If payload is SR_SNMPv1_PDU_PAYLOAD,
 * returns NULL since SNMPv1 did not use exceptions.
 *
 * When only SR_SNMPv1_PACKET is defined this function is replaced by a
 * macro in snmpd.h which expands to NULL regardless of parameters.
 */

VarBind        *
exception(type, name, payload)
    int             type;
    OID            *name;
    int             payload;
{
    VarBind        *vb;

#ifdef SR_SNMPv1_PACKET
    if (payload == SR_SNMPv1_PDU_PAYLOAD) {
	return NULL;
    }
#endif /* SR_SNMPv1_PACKET */

    if ((vb = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APERROR, "exception: vb malloc\n"));
	return (NULL);
    }
#ifdef SR_CLEAR_MALLOC
    memset((char *) vb, 0, sizeof(VarBind));
#endif  /* SR_CLEAR_MALLOC */

    vb->name = CloneOID(name);	/* name */
    vb->name_size = FindLenOID(vb->name);
    vb->name_type = OBJECT_ID_TYPE;
    vb->value.type = type;
    vb->value.ul_value = 0;
    vb->value.sl_value = 0;
    vb->value.oid_value = NULL;
    vb->value.os_value = NULL;
    vb->value.uint64_value = NULL;
    vb->value.flags = 0;
    vb->value.child_supp_type = 0;
    vb->next_var = NULL;
    vb->data_length = 0;
    vb->value_size = 2;
    return (vb);
}

#endif				/* SR_SNMPv2_PDU */

ObjectInfo     *
GetOidElem(i)
    int             i;
{
    return OidElem(i);
}

VarBind        *
i_GetVar(contextInfo, searchType, var)
    ContextInfo *contextInfo;
    int searchType;
    OID *var;
{
    OctetString *viewName;
    int payload;

    if (var == NULL) {
      DPRINTF((APWARN, "i_GetVar:  request with NULL variable\n"));
      return NULL;
    }


#ifdef SR_SNMPv2_PDU

     payload = SR_SNMPv2_PDU_PAYLOAD;

#else  /* SR_SNMPv2_PDU */
    payload = SR_SNMPv1_PDU_PAYLOAD;
#endif /* SR_SNMPv2_PDU */

    if(contextInfo != NULL) {
        viewName = LookupViewFromContextInfo(contextInfo, GET_REQUEST_TYPE);
    }
    else {
	viewName = (OctetString *)NULL;
    }

    if (searchType == EXACT) {
	return GetExactObjectInstance(var, viewName, contextInfo, -1, payload);
    } else {
	return GetNextObjectInstance(var, viewName, contextInfo, -1, payload);
    }
}

VarBind *
GetVar(contextInfo, searchType, object)
    ContextInfo    *contextInfo;
    int            searchType;
    char           *object;
{
    OID            *_object;
    VarBind        *ret;

    if ((_object = MakeOIDFromDot(object)) == NULL) {
	return NULL;
    }
    ret = i_GetVar(contextInfo, searchType, _object);

    FreeOID(_object);

    return ret;
}




VarBind        *
agg_GetVar(contextInfo, searchType, var, serialNum, payload)
    ContextInfo *contextInfo;
    int searchType;
    OID *var;
    int serialNum;
    int payload;
{ 
    return NULL;
}

